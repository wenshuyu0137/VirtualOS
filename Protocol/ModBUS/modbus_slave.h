/**
 * @file modbus_slave.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef _VIRTUAL_OS_MODBUS_SLAVE_H
#define _VIRTUAL_OS_MODBUS_SLAVE_H

#include "crc.h"
#include "modbus.h"
#include "queue.h"

#define MODBUS_SPECIAL_VALUE (0xF4)

typedef enum {
	RX_STATE_ADDR,
	RX_STATE_FUNC,
	RX_STATE_INFO,
	RX_STATE_DATA,
	RX_STATE_CRC,
} RX_STATE_E;

typedef struct {
	uint8_t reg_h;
	uint8_t reg_l;
	uint8_t num_h;
	uint8_t num_l;
} PDU_READ_T;

typedef struct {
	uint8_t reg_h;
	uint8_t reg_l;
	uint8_t num_h;
	uint8_t num_l;
	uint8_t len;
} PDU_WRITE_T;

typedef struct {
	uint8_t func;
	uint8_t pdu_in;
	uint8_t pdu_len;

	uint16_t cal_crc;
	RX_STATE_E state;
	q_size anchor;
	q_size forward;

	queue_info_t rx_q;
	union {
		PDU_READ_T read;
		PDU_WRITE_T write;
		uint8_t data[MODBUS_DATA_BYTES_MAX];
	} pdu;

} msg_state_t;

typedef struct {
	uint8_t len;
	uint8_t slave_addr;
	volatile uint8_t start_flag;
	modbus_serial_opt_t *p_opts;
	rtu_address_validator f_validator;
	uint16_t data_in_out[MODBUS_REG_NUM_MAX];
	msg_state_t msg_state;
} modbus_slave_t;

typedef enum {
	MODBUS_SLAVE_OPT_CODE_SUCCESS, //响应回复
	MODBUS_SLAVE_OPT_CODE_ERR_FUNC,
	MODBUS_SLAVE_OPT_CODE_ERR_REG,
	MODBUS_SLAVE_OPT_CODE_ERR_REGNUM,
	MODBUS_SLAVE_OPT_CODE_ERR_NOT_REPLY, //不回复
	MODBUS_SLAVE_OPT_CODE_ERR,
} modbus_slave_opt_code_e;

typedef modbus_slave_opt_code_e (*modbus_slave_handler)(uint16_t addr, uint8_t func, uint16_t reg, uint16_t reg_num, uint16_t *p_in_out);

typedef struct {
	uint16_t start;
	uint16_t end;
	modbus_slave_handler handle;
} modbus_slave_handler_t;

typedef struct {
	uint16_t num;
	modbus_slave_handler_t *table;
} modbus_slave_handler_table_t;

int modbus_slave_init(modbus_serial_opt_t *p_serial_opt, rtu_address_validator f_validator);
void modbus_slave_set_table(modbus_slave_handler_t *p_handler_table, uint16_t num);
void modbus_slave_poll(void);
#endif /*_VIRTUAL_OS_MODBUS_SLAVE_H*/