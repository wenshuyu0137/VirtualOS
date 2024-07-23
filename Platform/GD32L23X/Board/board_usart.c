/**
 * @file board_usart.c
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
 * copies or substantial usart_instance[id].chip_info.gd_usartions of the Software.
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

#include "board_usart.h"

#define GD_USART_ID_MAX 3 /*三个串口 1,3,4*/

static board_usart_instance_t usart_instance[GD_USART_ID_MAX] = {
	[0] = {
		.chip_info = {
			.clk = RCU_USART1,
			.gd_usart = USART1,
		},
		.tx_channel = {
			.dma_channel = DMA_CH0,
			.irqn = DMA_Channel0_IRQn,
		},
		.rx_channel = {
			.dma_channel = DMA_CH1,
			.irqn = USART1_IRQn,
		},

	},
	[1] = {
		.chip_info = {
			.clk = RCU_UART3,
			.gd_usart = UART3,
		},
		.tx_channel = {
			.dma_channel = DMA_CH2,
			.irqn = DMA_Channel2_IRQn,
		},
		.rx_channel = {
			.dma_channel = DMA_CH3,
			.irqn = UART3_IRQn,
		},
	},
	[2] = {
		.chip_info = {
			.clk = RCU_USART1,
			.gd_usart = USART1,
		},
		.tx_channel = {
			.dma_channel = DMA_CH4,
			.irqn = DMA_Channel4_IRQn,
		},
		.rx_channel = {
			.dma_channel = DMA_CH5,
			.irqn = UART3_IRQn,
		},

	},
};

static uint32_t acquire_baudrate(hal_baudrate_e baudrate)
{
	switch (baudrate) {
	case HAL_UART_BAUDRATE_1200:
		return 1200;

	case HAL_UART_BAUDRATE_4800:
		return 4800;

	case HAL_UART_BAUDRATE_9600:
		return 9600;

	case HAL_UART_BAUDRATE_19200:
		return 19200;

	case HAL_UART_BAUDRATE_38400:
		return 38400;

	case HAL_UART_BAUDRATE_57600:
		return 57600;

	case HAL_UART_BAUDRATE_115200:
		return 115200;

	case HAL_UART_BAUDRATE_250000:
		return 250000;

	case HAL_UART_BAUDRATE_921600:
		return 921600;

	case HAL_UART_BAUDRATE_1000000:
		return 1000000;

	default:
		return 0XFFFFFFFF;
	}
}

/*ID 转数组索引*/
static uint8_t hal_convert_to_board(hal_usart_id_e hal_usart_id)
{
	switch (hal_usart_id) {
	case HAL_USART_ID_1:
		return 0;
		break;

	case HAL_USART_ID_3:
		return 1;
		break;

	case HAL_USART_ID_4:
		return 2;
		break;

	default:
		return 0;
		break;
	}
};

/**
 * @brief 串口接收回调函数装饰器
 *
 * @param p_instance 串口实例指针
 * @param cb 回调函数
 */
static void usart_idle_decorate(int instance_id)
{
	uint16_t cur_data = usart_instance[instance_id].rx_channel.queue.buf_size - dma_transfer_number_get(usart_instance[instance_id].rx_channel.dma_channel);
	uint16_t recieve_size =
		(cur_data - usart_instance[instance_id].rx_channel.pre_trans_bytes) & (usart_instance[instance_id].rx_channel.queue.buf_size - 1);
	queue_advance_wr(&usart_instance[instance_id].rx_channel.queue, recieve_size);
	usart_instance[instance_id].rx_channel.pre_trans_bytes = cur_data;

	if (usart_instance[instance_id].rx_channel.cb) {
		usart_instance[instance_id].rx_channel.cb();
	}
}

#define DECORATE_CALLBACK(_x)                                                                                                                                  \
	static void usart##_x##_idle_decorate(void)                                                                                                            \
	{                                                                                                                                                      \
		usart_idle_decorate(_x);                                                                                                                       \
	}

DECORATE_CALLBACK(0)
DECORATE_CALLBACK(1)
DECORATE_CALLBACK(2)

typedef void (*decorate_cb_f)(void);
decorate_cb_f decorates[GD_USART_ID_MAX] = { usart0_idle_decorate, usart1_idle_decorate, usart2_idle_decorate };

static inline bool is_usart_valid(hal_usart_t *p_usart)
{
	return (p_usart && (p_usart->usart_id == HAL_USART_ID_1 || p_usart->usart_id == HAL_USART_ID_3 || p_usart->usart_id == HAL_USART_ID_4));
}

static inline bool is_usart_used_rx(uint8_t mode)
{
	return (mode & HAL_USART_MODE_RCV_ONLY);
}

static void _usart_rx_cfg(board_usart_instance_t *p_instance, hal_usart_usr_cfg_t *usr_rx_cfg)
{
	USART_IO_INIT_HELPER(&(usr_rx_cfg->io));
	usart_receive_config(p_instance->chip_info.gd_usart, USART_RECEIVE_ENABLE);
	queue_init(&(p_instance->rx_channel.queue), 1, usr_rx_cfg->buffer, usr_rx_cfg->buffer_size);
	dma_deinit(p_instance->rx_channel.dma_channel);
	dma_parameter_struct rx_dma_param;
	rx_dma_param.direction = DMA_PERIPHERAL_TO_MEMORY;
	rx_dma_param.memory_addr = (uint32_t)usr_rx_cfg->buffer;
	rx_dma_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	rx_dma_param.memory_width = DMA_MEMORY_WIDTH_8BIT;
	rx_dma_param.number = usr_rx_cfg->buffer_size;
	rx_dma_param.periph_addr = (uint32_t)(&(USART_RDATA(p_instance->chip_info.gd_usart)));
	rx_dma_param.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	rx_dma_param.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	rx_dma_param.priority = DMA_PRIORITY_ULTRA_HIGH;

	if (p_instance->id == 0) {
		rx_dma_param.request = DMA_REQUEST_USART1_RX;
	}

	else if (p_instance->id == 1) {
		rx_dma_param.request = DMA_REQUEST_UART3_RX;
	}

	else if (p_instance->id == 2) {
		rx_dma_param.request = DMA_REQUEST_UART4_RX;
	}

	dma_circulation_enable(p_instance->rx_channel.dma_channel);
	dma_init(p_instance->rx_channel.dma_channel, &rx_dma_param);
	usart_dma_receive_config(p_instance->chip_info.gd_usart, USART_RECEIVE_DMA_ENABLE);
	dma_channel_enable(p_instance->rx_channel.dma_channel);
	nvic_irq_enable(p_instance->rx_channel.irqn, 0);
	p_instance->rx_channel.cb = usr_rx_cfg->callback;
	usart_interrupt_enable(p_instance->chip_info.gd_usart, USART_INT_IDLE);
}

static void _usart_tx_cfg(board_usart_instance_t *p_instance, hal_usart_usr_cfg_t *usr_tx_cfg)
{
	USART_IO_INIT_HELPER(&(usr_tx_cfg->io));
	usart_transmit_config(p_instance->chip_info.gd_usart, USART_TRANSMIT_ENABLE);
	queue_init(&(p_instance->tx_channel.queue), 1, usr_tx_cfg->buffer, usr_tx_cfg->buffer_size);
	dma_deinit(p_instance->tx_channel.dma_channel);
	dma_parameter_struct tx_dma_param;
	tx_dma_param.direction = DMA_MEMORY_TO_PERIPHERAL;
	tx_dma_param.memory_addr = (uint32_t)usr_tx_cfg->buffer;
	tx_dma_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	tx_dma_param.memory_width = DMA_MEMORY_WIDTH_8BIT;
	tx_dma_param.number = usr_tx_cfg->buffer_size;
	tx_dma_param.periph_addr = (uint32_t)(&(USART_TDATA(p_instance->chip_info.gd_usart)));
	tx_dma_param.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	tx_dma_param.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	tx_dma_param.priority = DMA_PRIORITY_ULTRA_HIGH;

	if (p_instance->id == 0) {
		tx_dma_param.request = DMA_REQUEST_USART1_TX;
	}

	else if (p_instance->id == 1) {
		tx_dma_param.request = DMA_REQUEST_UART3_TX;
	}

	else if (p_instance->id == 2) {
		tx_dma_param.request = DMA_REQUEST_UART4_TX;
	}

	dma_init(p_instance->tx_channel.dma_channel, &tx_dma_param);
	usart_dma_transmit_config(p_instance->chip_info.gd_usart, USART_TRANSMIT_DMA_ENABLE);
	nvic_irq_enable(p_instance->tx_channel.irqn, 0);
	p_instance->tx_channel.cb = usr_tx_cfg->callback;
	dma_interrupt_enable(p_instance->tx_channel.dma_channel, DMA_INT_FTF);
}

static inline bool is_usart_used_tx(uint8_t mode)
{
	return (mode & HAL_USART_MODE_SND_ONLY);
}

void board_usart_hw_cfg(hal_usart_t *p_usart, hal_usart_hw_cfg_t *hw_cfg)
{
	uint8_t id = hal_convert_to_board(p_usart->usart_id);
	rcu_periph_clock_enable(usart_instance[id].chip_info.clk);
	usart_word_length_set(usart_instance[id].chip_info.gd_usart, CTL0_WL(hw_cfg->data_len));
	usart_stop_bit_set(usart_instance[id].chip_info.gd_usart, CTL1_STB(hw_cfg->stop_bits));
	usart_parity_config(usart_instance[id].chip_info.gd_usart, CTL0_PM(hw_cfg->parity));
	usart_baudrate_set(usart_instance[id].chip_info.gd_usart, acquire_baudrate(hw_cfg->baudrate));

	if (hw_cfg->flow == HAL_USART_HARD_FLOW_CTS) {
		usart_hardware_flow_cts_config(usart_instance[id].chip_info.gd_usart, USART_CTS_ENABLE);
	}

	else if (hw_cfg->flow == HAL_USART_HARD_FLOW_RTS) {
		usart_hardware_flow_rts_config(usart_instance[id].chip_info.gd_usart, USART_RTS_ENABLE);
	}

	else if (hw_cfg->flow == HAL_USART_HARD_FLOW_RTS_CTS) {
		usart_hardware_flow_cts_config(usart_instance[id].chip_info.gd_usart, USART_CTS_ENABLE);
		usart_hardware_flow_rts_config(usart_instance[id].chip_info.gd_usart, USART_RTS_ENABLE);
	}

	if (hw_cfg->duplex) {
		usart_halfduplex_enable(usart_instance[id].chip_info.gd_usart);
	}
}

bool board_usart_init(hal_usart_t *p_usart, hal_usart_hw_cfg_t *hw_cfg, hal_usart_usr_cfg_t *usr_tx_cfg, hal_usart_usr_cfg_t *usr_rx_cfg)
{
	board_usart_instance_t *p;

	if (!is_usart_valid(p_usart)) {
		return false;
	}

	rcu_periph_clock_enable(RCU_DMA);
	uint8_t id = hal_convert_to_board(p_usart->usart_id);
	usart_deinit(usart_instance[id].chip_info.gd_usart);
	p = &usart_instance[id];
	p->id = id;
	board_usart_hw_cfg(p_usart, hw_cfg);

	if (is_usart_used_tx(hw_cfg->mode) && usr_tx_cfg) {
		_usart_tx_cfg(p, usr_tx_cfg);
		board_irqn_cb_register(p->tx_channel.cb,
				       HAL_IRQ_TYPE_DMA1_Channel0 + id * 2); //DMA通道 0,2,4
	}

	if (is_usart_used_rx(hw_cfg->mode) && usr_rx_cfg) {
		_usart_rx_cfg(p, usr_rx_cfg);
		board_irqn_cb_register(decorates[id],
				       HAL_IRQ_TYPE_USART1 + id); //注册装饰器
	}

	p_usart->priv_data = p;
	usart_enable(usart_instance[id].chip_info.gd_usart);
	return 0;
}

size_t board_usart_write(hal_usart_t *p_usart, uint8_t *data, size_t len)
{
	board_usart_instance_t *p = (board_usart_instance_t *)p_usart->priv_data;

	if (!is_usart_valid(p_usart)) {
		return 0;
	}

	len = len > 512 ? 512 : len;
	memcpy(p->tx_channel.queue.buf, data, len);
	dma_channel_disable(p->tx_channel.dma_channel);
	dma_flag_clear(p->tx_channel.dma_channel, DMA_FLAG_FTF);
	dma_interrupt_flag_clear(p->tx_channel.dma_channel, DMA_INT_FLAG_FTF);
	dma_transfer_number_config(p->tx_channel.dma_channel, len);
	dma_channel_enable(p->tx_channel.dma_channel);
	return len;
}

size_t board_usart_read(hal_usart_t *p_usart, uint8_t *data, size_t len)
{
	board_usart_instance_t *p = (board_usart_instance_t *)(p_usart->priv_data);

	if (!is_usart_valid(p_usart)) {
		return 0;
	}

	return queue_get(&(p->rx_channel.queue), data, len);
}

int board_usart_flush(hal_usart_t *p_usart, hal_usart_mode_e mode)
{
	board_usart_instance_t *p = (board_usart_instance_t *)(p_usart->priv_data);

	if (!is_usart_valid(p_usart)) {
		return -1;
	}

	if (is_usart_used_tx(mode)) {
		queue_reset_safe_from_get(&(p->tx_channel.queue));
	}

	if (is_usart_used_rx(mode)) {
		queue_reset_safe_from_get(&(p->rx_channel.queue));
	}

	return 0;
}
