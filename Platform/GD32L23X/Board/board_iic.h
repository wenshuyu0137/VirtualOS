#ifndef _BOARD_IIC_H
#define _BOARD_IIC_H

#include "gd32l23x.h"
#include "hal_iic.h"

#define IIC_OUT_TIME 50000
void board_i2c_deinit(HAL_I2C_ID_E i2cx);
void board_i2c_init(HAL_I2C_ID_E i2cx);

uint8_t board_i2c_send_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
uint8_t board_i2c_rcv_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *rcv_buf, uint8_t len);
#endif