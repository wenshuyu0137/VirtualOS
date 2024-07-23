#include "hal_iic.h"
#include "board_iic.h"

void hal_i2c_deinit(HAL_I2C_ID_E i2cx)
{
	board_i2c_deinit(i2cx);
}

void hal_i2c_init(HAL_I2C_ID_E i2cx)
{
	board_i2c_init(i2cx);
}

uint8_t hal_i2c_send_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	return (board_i2c_send_bytes(i2cx, slave_addr, reg_addr, data, len));
}

uint8_t hal_i2c_rcv_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *rcv_buf, uint8_t len)
{
	return (board_i2c_rcv_bytes(i2cx, slave_addr, reg_addr, rcv_buf, len));
}