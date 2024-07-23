#ifndef _VIRTUAL_OS_HAL_IIC_H
#define _VIRTUAL_OS_HAL_IIC_H

#include <stdint.h>

typedef enum {
	HAL_I2C_0,
	HAL_I2C_1,
	HAL_I2C_2,
} HAL_I2C_ID_E;

void hal_i2c_deinit(HAL_I2C_ID_E i2cx);
void hal_i2c_init(HAL_I2C_ID_E i2cx);

uint8_t hal_i2c_send_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
uint8_t hal_i2c_rcv_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *rcv_buf, uint8_t len);
#endif