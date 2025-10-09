#include "usb_cdc_driver.h"

#include <string.h>

#ifndef USB_CDC_DRIVER_MAX_PACKET_SIZE
#define USB_CDC_DRIVER_MAX_PACKET_SIZE (64U)
#endif

static usb_cdc_driver_backend_t driver_backend = {0};
static uint8_t rx_buffer[USB_CDC_DRIVER_RX_BUFFER_SIZE] = {0};
static volatile size_t rx_head = 0U;
static volatile size_t rx_tail = 0U;

static size_t usb_cdc_driver_next_index(size_t index)
{
    return (index + 1U) % USB_CDC_DRIVER_RX_BUFFER_SIZE;
}

static void usb_cdc_driver_critical_enter(uint32_t *primask)
{
    *primask = __get_PRIMASK();
    __disable_irq();
}

static void usb_cdc_driver_critical_exit(uint32_t primask)
{
    if (primask == 0U)
    {
        __enable_irq();
    }
}

void usb_cdc_driver_init(const usb_cdc_driver_backend_t *backend)
{
    usb_cdc_driver_set_backend(backend);
    uint32_t primask;
    usb_cdc_driver_critical_enter(&primask);
    rx_head = 0U;
    rx_tail = 0U;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    usb_cdc_driver_critical_exit(primask);
}

void usb_cdc_driver_set_backend(const usb_cdc_driver_backend_t *backend)
{
    if (backend != NULL)
    {
        driver_backend = *backend;
    }
    else
    {
        driver_backend.transmit = NULL;
    }
}

bool usb_cdc_driver_connected(void)
{
    return driver_backend.transmit != NULL;
}

size_t usb_cdc_driver_bytes_available(void)
{
    uint32_t primask;
    usb_cdc_driver_critical_enter(&primask);

    size_t available;
    if (rx_head >= rx_tail)
    {
        available = rx_head - rx_tail;
    }
    else
    {
        available = (USB_CDC_DRIVER_RX_BUFFER_SIZE - rx_tail) + rx_head;
    }

    usb_cdc_driver_critical_exit(primask);
    return available;
}

int usb_cdc_driver_read_byte(void)
{
    int value = -1;
    uint32_t primask;
    usb_cdc_driver_critical_enter(&primask);

    if (rx_head != rx_tail)
    {
        value = rx_buffer[rx_tail];
        rx_tail = usb_cdc_driver_next_index(rx_tail);
    }

    usb_cdc_driver_critical_exit(primask);
    return value;
}

size_t usb_cdc_driver_read(uint8_t *buffer, size_t max_length)
{
    if ((buffer == NULL) || (max_length == 0U))
    {
        return 0U;
    }

    size_t read_count = 0U;
    int value = 0;
    while ((read_count < max_length) && ((value = usb_cdc_driver_read_byte()) >= 0))
    {
        buffer[read_count++] = (uint8_t)value;
    }

    return read_count;
}

int32_t usb_cdc_driver_write(const uint8_t *data, size_t length, uint32_t timeout_ms)
{
    if (!usb_cdc_driver_connected() || (data == NULL) || (length == 0U))
    {
        return -1;
    }

    if (timeout_ms == 0U)
    {
        timeout_ms = USB_CDC_DRIVER_DEFAULT_TIMEOUT_MS;
    }

    size_t total_sent = 0U;
    uint32_t start_tick = HAL_GetTick();

    while (total_sent < length)
    {
        size_t remaining = length - total_sent;
        uint16_t chunk_length = (uint16_t)((remaining > USB_CDC_DRIVER_MAX_PACKET_SIZE)
                                               ? USB_CDC_DRIVER_MAX_PACKET_SIZE
                                               : remaining);

        uint8_t status = driver_backend.transmit(&data[total_sent], chunk_length);
        if (status == USB_CDC_DRIVER_BACKEND_OK)
        {
            total_sent += chunk_length;
            continue;
        }

        if (status != USB_CDC_DRIVER_BACKEND_BUSY)
        {
            return -2;
        }

        if ((HAL_GetTick() - start_tick) >= timeout_ms)
        {
            return -3;
        }
    }

    return (int32_t)total_sent;
}

int32_t usb_cdc_driver_write_string(const char *text, uint32_t timeout_ms)
{
    if (text == NULL)
    {
        return -1;
    }

    return usb_cdc_driver_write((const uint8_t *)text, strlen(text), timeout_ms);
}

void usb_cdc_driver_on_receive(const uint8_t *data, size_t length)
{
    if ((data == NULL) || (length == 0U))
    {
        return;
    }

    uint32_t primask;
    usb_cdc_driver_critical_enter(&primask);

    for (size_t idx = 0; idx < length; ++idx)
    {
        rx_buffer[rx_head] = data[idx];
        size_t next_head = usb_cdc_driver_next_index(rx_head);
        if (next_head == rx_tail)
        {
            rx_tail = usb_cdc_driver_next_index(rx_tail);
        }
        rx_head = next_head;
    }

    usb_cdc_driver_critical_exit(primask);
}

