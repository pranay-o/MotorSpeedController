#include "app/app_layer.h"

#include "hw/hw_layer.h"
#include "io/io_layer.h"
#include "uart_cli.h"

void app_layer_init(void)
{
    hw_layer_init();
    io_layer_init();
}

void app_layer_tick(void)
{
    uart_cli_poll();
}

