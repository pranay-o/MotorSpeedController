#include "io/io_layer.h"

#include "uart_cli.h"
#include "usart.h"

void io_layer_init(void)
{
    /* Bring up the UART CLI on USART2 so higher layers can register commands. */
    uart_cli_init(&huart2);
}

