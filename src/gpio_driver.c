#include "gpio_driver.h"

static bool gpio_driver_is_valid(const gpio_driver_pin_t *channel)
{
    return (channel != NULL) && (channel->port != NULL);
}

static GPIO_PinState gpio_driver_resolve_state(const gpio_driver_pin_t *channel, bool active_state)
{
    if (channel->active_polarity == GPIO_DRIVER_ACTIVE_LOW)
    {
        active_state = !active_state;
    }

    return active_state ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void gpio_driver_write(const gpio_driver_pin_t *channel, bool active_state)
{
    if (!gpio_driver_is_valid(channel))
    {
        return;
    }

    HAL_GPIO_WritePin(channel->port, channel->pin, gpio_driver_resolve_state(channel, active_state));
}

void gpio_driver_set_active(const gpio_driver_pin_t *channel)
{
    gpio_driver_write(channel, true);
}

void gpio_driver_set_inactive(const gpio_driver_pin_t *channel)
{
    gpio_driver_write(channel, false);
}

void gpio_driver_toggle(const gpio_driver_pin_t *channel)
{
    if (!gpio_driver_is_valid(channel))
    {
        return;
    }

    HAL_GPIO_TogglePin(channel->port, channel->pin);
}

bool gpio_driver_read_active(const gpio_driver_pin_t *channel)
{
    if (!gpio_driver_is_valid(channel))
    {
        return false;
    }

    GPIO_PinState pin_state = HAL_GPIO_ReadPin(channel->port, channel->pin);
    bool is_active = (pin_state == GPIO_PIN_SET);

    if (channel->active_polarity == GPIO_DRIVER_ACTIVE_LOW)
    {
        is_active = !is_active;
    }

    return is_active;
}

GPIO_PinState gpio_driver_read_raw(const gpio_driver_pin_t *channel)
{
    if (!gpio_driver_is_valid(channel))
    {
        return GPIO_PIN_RESET;
    }

    return HAL_GPIO_ReadPin(channel->port, channel->pin);
}
