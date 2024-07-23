#include "board_iic.h"
uint32_t get_iic(HAL_I2C_ID_E i2cx)
{
	uint32_t i2c;

	switch (i2cx) {
	case HAL_I2C_0:
		i2c = I2C0;
		break;

	case HAL_I2C_1:
		i2c = I2C1;
		break;

	case HAL_I2C_2:
		i2c = I2C2;
		break;

	default:
		i2c = 0xffffffff;
		break;
	}

	return i2c;
}

uint32_t get_iic_rcu(HAL_I2C_ID_E i2cx)
{
	uint32_t rcu;

	switch (i2cx) {
	case HAL_I2C_0:
		rcu = RCU_I2C0;
		break;

	case HAL_I2C_1:
		rcu = RCU_I2C1;
		break;

	case HAL_I2C_2:
		rcu = RCU_I2C2;
		break;

	default:
		rcu = 0xffffffff;
		break;
	}

	return rcu;
}

void board_i2c_deinit(HAL_I2C_ID_E i2cx)
{
	i2c_deinit(get_iic(i2cx));
}

void board_i2c_init(HAL_I2C_ID_E i2cx)
{
	uint32_t m_iic = get_iic(i2cx);
	uint32_t iic_rcu = get_iic_rcu(i2cx);
	rcu_periph_clock_enable(iic_rcu);
	i2c_timing_config(m_iic, 0, 0x3, 0);
	i2c_master_clock_config(m_iic, 0x13, 0x36);
	i2c_enable(m_iic);
}

uint8_t board_i2c_WaitReg(uint32_t i2c_periph, uint32_t bitMask, uint8_t u8TrueFalse)
{
	uint32_t u32Cnt = 0;

	while (i2c_flag_get(i2c_periph, bitMask) != u8TrueFalse) {
		u32Cnt++;

		if (u32Cnt > IIC_OUT_TIME) {
			i2c_stop_on_bus(i2c_periph);
			i2c_disable(i2c_periph);
			i2c_enable(i2c_periph);
			return 1;
		}
	}

	return 0;
}

uint8_t board_i2c_send_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	uint32_t m_iic = get_iic(i2cx);
	i2c_master_addressing(m_iic, slave_addr, I2C_MASTER_TRANSMIT);
	i2c_transfer_byte_number_config(m_iic, len + 1);
	i2c_automatic_end_disable(m_iic);
	I2C_STAT(m_iic) |= I2C_STAT_TBE;

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_I2CBSY,
			      0)) { //I2C_FLAG_I2CBSY����0����
		return 1;
	}

	i2c_start_on_bus(m_iic);

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_TBE, SET)) {
		return 1;
	}

	i2c_data_transmit(m_iic, reg_addr);

	while (len--) {
		//while(!i2c_flag_get(m_iic, I2C_FLAG_TI));
		if (board_i2c_WaitReg(m_iic, I2C_FLAG_TI,
				      SET)) { //��ʱ������I2C_FLAG_TI ==SET���˳�
			return 1;
		}

		i2c_data_transmit(m_iic, *data);
		data++;
	}

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_TC, SET)) {
		return 1;
	}

	i2c_stop_on_bus(m_iic);

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_STPDET, SET)) {
		return 1;
	}

	i2c_flag_clear(m_iic, I2C_FLAG_STPDET);
	return 0;
}

//volatile uint32_t gu32Cntaasdf=0;
uint8_t board_i2c_rcv_bytes(HAL_I2C_ID_E i2cx, uint8_t slave_addr, uint8_t reg_addr, uint8_t *rcv_buf, uint8_t len)
{
	uint32_t m_iic = get_iic(i2cx);
	uint32_t u32Temp;
	I2C_STAT(m_iic) |= I2C_STAT_TBE;
	i2c_master_addressing(m_iic, slave_addr, I2C_MASTER_TRANSMIT);
	i2c_transfer_byte_number_config(m_iic, 1);
	i2c_automatic_end_disable(m_iic);

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_I2CBSY, 0)) {
		return 1;
	}

	i2c_start_on_bus(m_iic);

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_TBE, SET)) {
		return 1;
	}

	i2c_data_transmit(m_iic, reg_addr);

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_TC, SET)) {
		return 1;
	}

	i2c_master_addressing(m_iic, slave_addr, I2C_MASTER_RECEIVE);
	i2c_transfer_byte_number_config(m_iic, len);
	i2c_start_on_bus(m_iic);

	while (board_i2c_WaitReg(m_iic, I2C_FLAG_TBE, SET)) {
		return 1;
	}

	while (len) {
		if (board_i2c_WaitReg(m_iic, I2C_FLAG_RBNE, SET)) {
			return 1;
		}

		*rcv_buf = i2c_data_receive(m_iic);
		rcv_buf++;
		len--;
	}

	i2c_stop_on_bus(m_iic);

	if (board_i2c_WaitReg(m_iic, I2C_FLAG_STPDET, SET)) {
		return 5;
	}

	i2c_flag_clear(m_iic, I2C_FLAG_STPDET);
	return 0;
}