#include "uart_cli.h"

#include "main.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>

#define UART_CLI_TX_TIMEOUT_MS (25U)

static UART_HandleTypeDef *cli_uart = NULL;
static uint8_t rx_byte = 0U;

static char rx_line[UART_CLI_MAX_LINE_LENGTH] = {0};
static size_t rx_line_length = 0U;

static char pending_line[UART_CLI_MAX_LINE_LENGTH] = {0};
static volatile bool pending_line_ready = false;

static bool echo_enabled = true;

static const uart_cli_command_t *command_table[UART_CLI_MAX_COMMANDS] = {0};
static size_t command_count = 0U;

static void uart_cli_start_receive(void);
static void uart_cli_handle_line(char *line);
static void uart_cli_print_prompt(void);
static void uart_cli_send(const char *text, bool append_newline);
static const uart_cli_command_t *uart_cli_find_command(const char *name);
static void uart_cli_handle_help(const char *args);

static const uart_cli_command_t help_command = {
    .name = "help",
    .handler = uart_cli_handle_help,
    .help = "List available commands"
};

void uart_cli_init(UART_HandleTypeDef *huart)
{
    if (huart == NULL)
    {
        return;
    }

    cli_uart = huart;
    rx_line_length = 0U;
    pending_line_ready = false;
    command_count = 0U;

    (void)uart_cli_register_command(&help_command);
    uart_cli_print_prompt();
    uart_cli_start_receive();
}

void uart_cli_set_echo_enabled(bool enabled)
{
    echo_enabled = enabled;
}

bool uart_cli_register_command(const uart_cli_command_t *command)
{
    if ((command == NULL) || (command->name == NULL) || (command->handler == NULL))
    {
        return false;
    }

    for (size_t idx = 0; idx < command_count; ++idx)
    {
        if (strcmp(command_table[idx]->name, command->name) == 0)
        {
            return false;
        }
    }

    if (command_count >= UART_CLI_MAX_COMMANDS)
    {
        return false;
    }

    command_table[command_count++] = command;
    return true;
}

void uart_cli_poll(void)
{
    if ((cli_uart == NULL) || !pending_line_ready)
    {
        return;
    }

    char local_buffer[UART_CLI_MAX_LINE_LENGTH];
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    memcpy(local_buffer, pending_line, sizeof(local_buffer));
    pending_line_ready = false;
    if (primask == 0U)
    {
        __enable_irq();
    }

    uart_cli_handle_line(local_buffer);
    uart_cli_print_prompt();
}

HAL_StatusTypeDef uart_cli_write(const char *text)
{
    if ((cli_uart == NULL) || (text == NULL))
    {
        return HAL_ERROR;
    }

    size_t length = strlen(text);
    if (length == 0U)
    {
        return HAL_OK;
    }

    return HAL_UART_Transmit(cli_uart, (uint8_t *)text, (uint16_t)length, UART_CLI_TX_TIMEOUT_MS);
}

HAL_StatusTypeDef uart_cli_write_line(const char *text)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (text != NULL)
    {
        status = uart_cli_write(text);
    }

    if (status == HAL_OK)
    {
        status = uart_cli_write("\r\n");
    }

    return status;
}

void uart_cli_on_uart_rx_complete(UART_HandleTypeDef *huart)
{
    if ((huart == NULL) || (huart != cli_uart))
    {
        return;
    }

    uint8_t byte = rx_byte;

    if ((byte == '\r') || (byte == '\n'))
    {
        if (echo_enabled)
        {
            (void)uart_cli_write("\r\n");
        }

        if (!pending_line_ready)
        {
            rx_line[rx_line_length] = '\0';
            memcpy(pending_line, rx_line, sizeof(pending_line));
            pending_line_ready = true;
        }
        rx_line_length = 0U;
    }
    else if ((byte == '\b') || (byte == 0x7FU))
    {
        if (rx_line_length > 0U)
        {
            rx_line_length--;
            if (echo_enabled)
            {
                (void)uart_cli_write("\b \b");
            }
        }
    }
    else if (isprint(byte) != 0)
    {
        if (rx_line_length < (UART_CLI_MAX_LINE_LENGTH - 1U))
        {
            rx_line[rx_line_length++] = (char)byte;
            if (echo_enabled)
            {
                char echo_char[2] = {(char)byte, '\0'};
                (void)uart_cli_write(echo_char);
            }
        }
    }
    else
    {
        /* Ignore non-printable characters */
    }

    uart_cli_start_receive();
}

void uart_cli_on_uart_error(UART_HandleTypeDef *huart)
{
    if ((huart == NULL) || (huart != cli_uart))
    {
        return;
    }

    uart_cli_start_receive();
}

static void uart_cli_start_receive(void)
{
    if (cli_uart != NULL)
    {
        (void)HAL_UART_Receive_IT(cli_uart, &rx_byte, 1U);
    }
}

static void uart_cli_trim(char **text)
{
    if ((text == NULL) || (*text == NULL))
    {
        return;
    }

    char *start = *text;
    while ((*start != '\0') && isspace((unsigned char)*start))
    {
        start++;
    }

    char *end = start + strlen(start);
    while ((end > start) && isspace((unsigned char)*(end - 1)))
    {
        end--;
    }
    *end = '\0';
    *text = start;
}

static void uart_cli_handle_line(char *line)
{
    if (line == NULL)
    {
        return;
    }

    char *cursor = line;
    uart_cli_trim(&cursor);

    if (*cursor == '\0')
    {
        return;
    }

    char *args = cursor;
    while ((*args != '\0') && !isspace((unsigned char)*args))
    {
        args++;
    }

    if (*args != '\0')
    {
        *args++ = '\0';
        uart_cli_trim(&args);
    }

    const uart_cli_command_t *command = uart_cli_find_command(cursor);
    if (command != NULL)
    {
        command->handler(args);
    }
    else
    {
        uart_cli_write_line("Unknown command. Type 'help'.");
    }
}

static const uart_cli_command_t *uart_cli_find_command(const char *name)
{
    if (name == NULL)
    {
        return NULL;
    }

    for (size_t idx = 0; idx < command_count; ++idx)
    {
        if (strcmp(command_table[idx]->name, name) == 0)
        {
            return command_table[idx];
        }
    }

    return NULL;
}

static void uart_cli_handle_help(const char *args)
{
    (void)args;
    uart_cli_write_line("Available commands:");
    for (size_t idx = 0; idx < command_count; ++idx)
    {
        const uart_cli_command_t *cmd = command_table[idx];
        if (cmd->help != NULL)
        {
            uart_cli_write(cmd->name);
            uart_cli_write(" - ");
            uart_cli_write_line(cmd->help);
        }
        else
        {
            uart_cli_write_line(cmd->name);
        }
    }
}

static void uart_cli_print_prompt(void)
{
    uart_cli_send("> ", false);
}

static void uart_cli_send(const char *text, bool append_newline)
{
    if (text != NULL)
    {
        (void)uart_cli_write(text);
    }

    if (append_newline)
    {
        (void)uart_cli_write("\r\n");
    }
}

__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_cli_on_uart_rx_complete(huart);
}

__weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uart_cli_on_uart_error(huart);
}
