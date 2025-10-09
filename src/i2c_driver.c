#include "i2c_driver.h"

static uint16_t i2c_driver_resolve_address(const i2c_driver_device_t *device)
{
    return (uint16_t)(device->address << 1);
}

bool i2c_driver_is_ready(I2C_HandleTypeDef *handle, uint32_t timeout_ms)
{
    if (handle == NULL)
    {
        return false;
    }

    uint32_t start = HAL_GetTick();
    do
    {
        if (HAL_I2C_GetState(handle) == HAL_I2C_STATE_READY)
        {
            return true;
        }
    } while ((HAL_GetTick() - start) < timeout_ms);

    return HAL_I2C_GetState(handle) == HAL_I2C_STATE_READY;
}

bool i2c_driver_ping(const i2c_driver_device_t *device, uint32_t timeout_ms)
{
    if ((device == NULL) || (device->handle == NULL))
    {
        return false;
    }

    return HAL_I2C_IsDeviceReady(device->handle,
                                 i2c_driver_resolve_address(device),
                                 2,
                                 timeout_ms) == HAL_OK;
}

HAL_StatusTypeDef i2c_driver_read(const i2c_driver_device_t *device,
                                  uint8_t *buffer,
                                  size_t length,
                                  uint32_t timeout_ms)
{
    if ((device == NULL) || (device->handle == NULL) || (buffer == NULL) || (length == 0U))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Receive(device->handle,
                                  i2c_driver_resolve_address(device),
                                  buffer,
                                  (uint16_t)length,
                                  timeout_ms);
}

HAL_StatusTypeDef i2c_driver_write(const i2c_driver_device_t *device,
                                   const uint8_t *buffer,
                                   size_t length,
                                   uint32_t timeout_ms)
{
    if ((device == NULL) || (device->handle == NULL) || (buffer == NULL) || (length == 0U))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Transmit(device->handle,
                                   i2c_driver_resolve_address(device),
                                   (uint8_t *)buffer,
                                   (uint16_t)length,
                                   timeout_ms);
}

HAL_StatusTypeDef i2c_driver_read_register(const i2c_driver_device_t *device,
                                           uint8_t reg,
                                           uint8_t *buffer,
                                           size_t length,
                                           uint32_t timeout_ms)
{
    if ((device == NULL) || (device->handle == NULL) || (buffer == NULL) || (length == 0U))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Mem_Read(device->handle,
                             i2c_driver_resolve_address(device),
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             buffer,
                             (uint16_t)length,
                             timeout_ms);
}

HAL_StatusTypeDef i2c_driver_write_register(const i2c_driver_device_t *device,
                                            uint8_t reg,
                                            const uint8_t *data,
                                            size_t length,
                                            uint32_t timeout_ms)
{
    if ((device == NULL) || (device->handle == NULL) || (data == NULL) || (length == 0U))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Mem_Write(device->handle,
                              i2c_driver_resolve_address(device),
                              reg,
                              I2C_MEMADD_SIZE_8BIT,
                              (uint8_t *)data,
                              (uint16_t)length,
                              timeout_ms);
}

