#include "hw/hw_layer.h"

#include "hw/board_pins.h"
#include "gpio_driver.h"

void hw_layer_init(void)
{
    /* Default outputs to a safe state before higher layers take control. */
    gpio_driver_set_inactive(&BOARD_PIN_GATE_ENABLE);
    gpio_driver_set_inactive(&BOARD_PIN_BRAKE);
    gpio_driver_set_inactive(&BOARD_PIN_STATUS_LED);
}

