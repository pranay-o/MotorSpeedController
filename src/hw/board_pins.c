#include "hw/board_pins.h"

const gpio_driver_pin_t BOARD_PIN_PHASE_U_HIGH = {
    .port = GPIOA,
    .pin = GPIO_PIN_8,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_PHASE_U_LOW = {
    .port = GPIOC,
    .pin = GPIO_PIN_13,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_PHASE_V_HIGH = {
    .port = GPIOA,
    .pin = GPIO_PIN_9,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_PHASE_V_LOW = {
    .port = GPIOB,
    .pin = GPIO_PIN_14,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_PHASE_W_HIGH = {
    .port = GPIOA,
    .pin = GPIO_PIN_10,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_PHASE_W_LOW = {
    .port = GPIOB,
    .pin = GPIO_PIN_9,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_UART_DEBUG_TX = {
    .port = GPIOA,
    .pin = GPIO_PIN_2,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_UART_DEBUG_RX = {
    .port = GPIOB,
    .pin = GPIO_PIN_4,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_I2C_SCL = {
    .port = GPIOA,
    .pin = GPIO_PIN_15,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_I2C_SDA = {
    .port = GPIOB,
    .pin = GPIO_PIN_7,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_STATUS_LED = {
    .port = GPIOA,
    .pin = GPIO_PIN_4,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_GATE_ENABLE = {
    .port = GPIOA,
    .pin = GPIO_PIN_7,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_BRAKE = {
    .port = GPIOB,
    .pin = GPIO_PIN_6,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_DI_FAULT = {
    .port = GPIOB,
    .pin = GPIO_PIN_10,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_DI_AUX1 = {
    .port = GPIOC,
    .pin = GPIO_PIN_14,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

const gpio_driver_pin_t BOARD_PIN_DI_AUX2 = {
    .port = GPIOC,
    .pin = GPIO_PIN_6,
    .active_polarity = GPIO_DRIVER_ACTIVE_HIGH
};

