/**
 * @file driver_485.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-01
 * 
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#include "driver_485.h"
#include "dml_init.h"
#include "gd32l23x.h"
#include "queue.h"
#include "stimer.h"

static const char serial_485_name[] = "/dev/serial_485";
static dml_dev_err_e serial_485_open(void);
static dml_dev_err_e serial_485_close(void);
static dml_dev_err_e serial_485_ioctrl(int cmd, void *arg);
static int serial_485_read(uint8_t *buf, size_t len);
static int serial_485_write(const uint8_t *buf, size_t len);

static bool is_serial_opened = false;

#define RX_FIFO_SIZE 512
#define TX_BUF_SIZE 256
static uint8_t rx_fifo[RX_FIFO_SIZE] = { 0 };
static uint8_t tx_buf[TX_BUF_SIZE] = { 0 };

static queue_info_t recv_q;

uint16_t *recv_size = NULL;

static void usart_init(void)
{
	rcu_periph_clock_enable(RCU_DMA);
	usart_deinit(USART1);

	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_11); //Enable
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	gpio_bit_write(GPIOC, GPIO_PIN_11, RESET); //接收状态

	rcu_periph_clock_enable(RCU_USART1);

	usart_word_length_set(USART1, USART_WL_8BIT);
	usart_parity_config(USART1, USART_PM_NONE);
	usart_baudrate_set(USART1, 115200);

	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_5); //Tx
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);
	usart_stop_bit_set(USART1, USART_STB_1BIT);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_5) usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);
	dma_deinit(DMA_CH0);
	dma_parameter_struct tx_dma_param;
	tx_dma_param.direction = DMA_MEMORY_TO_PERIPHERAL;
	tx_dma_param.memory_addr = (uint32_t)tx_buf;
	tx_dma_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	tx_dma_param.memory_width = DMA_MEMORY_WIDTH_8BIT;
	tx_dma_param.number = TX_BUF_SIZE;
	tx_dma_param.periph_addr = (uint32_t)(&(USART_TDATA(USART1)));
	tx_dma_param.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	tx_dma_param.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	tx_dma_param.priority = DMA_PRIORITY_HIGH;
	tx_dma_param.request = DMA_REQUEST_USART1_TX;
	dma_init(DMA_CH0, &tx_dma_param);
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE); //TX_DMA
	nvic_irq_enable(DMA_Channel0_IRQn, 0);
	dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);

	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_6); //Rx
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_6);
	usart_receive_config(USART1, USART_RECEIVE_ENABLE); //RX_DMA
	queue_init(&recv_q, 1, rx_fifo, RX_FIFO_SIZE);
	dma_deinit(DMA_CH1);
	dma_parameter_struct rx_dma_param;
	rx_dma_param.direction = DMA_PERIPHERAL_TO_MEMORY;
	rx_dma_param.memory_addr = (uint32_t)rx_fifo;
	rx_dma_param.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	rx_dma_param.memory_width = DMA_MEMORY_WIDTH_8BIT;
	rx_dma_param.number = RX_FIFO_SIZE;
	rx_dma_param.periph_addr = (uint32_t)(&(USART_RDATA(USART1)));
	rx_dma_param.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	rx_dma_param.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	rx_dma_param.priority = DMA_PRIORITY_ULTRA_HIGH;
	rx_dma_param.request = DMA_REQUEST_USART1_RX;
	dma_circulation_enable(DMA_CH1);
	dma_init(DMA_CH1, &rx_dma_param);
	usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);
	dma_channel_enable(DMA_CH1);
	nvic_irq_enable(USART1_IRQn, 0);
	usart_interrupt_enable(USART1, USART_INT_IDLE);

	usart_enable(USART1);
}

void into_recieve(void)
{
	gpio_bit_write(GPIOC, GPIO_PIN_11, RESET);
}

//DMA发送完成中断
void DMA_Channel0_IRQHandler(void)
{
	if (RESET != dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)) {
		defer_task_add(into_recieve, 1); //延迟1ms拉低引脚,转为接受状态
		memset(tx_buf, 0, TX_BUF_SIZE);
		dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_FTF);
	}
}



//空闲中断
static uint16_t pre_recv = 0;
void USART1_IRQHandler(void)
{
	if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE) != RESET) {
		uint16_t cur_index = RX_FIFO_SIZE - dma_transfer_number_get(DMA_CH1);
		uint16_t cur_recv = (cur_index - pre_recv) & (RX_FIFO_SIZE - 1);
		pre_recv = cur_index;
		queue_advance_wr(&recv_q, cur_recv);

		if (recv_size)
			*recv_size = cur_recv;

		usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
	}
}

static dml_dev_err_e serial_485_open(void)
{
	if (is_serial_opened)
		return DML_DEV_ERR_OCCUPIED;

	is_serial_opened = true;

	usart_init();

	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e serial_485_close(void)
{
	is_serial_opened = false;
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e serial_485_ioctrl(int cmd, void *arg)
{
	switch (cmd) {
	default:
		break;
	}
	return DML_DEV_ERR_NONE;
}

static int serial_485_write(const uint8_t *buf, size_t len)
{
	if (!is_serial_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	gpio_bit_write(GPIOC, GPIO_PIN_11, SET); //拉高使能引脚为发送状态

	dma_channel_disable(DMA_CH0);
	memcpy(tx_buf, buf, len);

	dma_transfer_number_config(DMA_CH0, len);
	dma_channel_enable(DMA_CH0);

	return len;
}

static int serial_485_read(uint8_t *buf, size_t len)
{
	if (!is_serial_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	return queue_get(&recv_q, buf, len);
}

static dml_file_opts_t serial_485_red_dev = {
	.close = serial_485_close,
	.ioctrl = serial_485_ioctrl,
	.name = serial_485_name,
	.open = serial_485_open,
	.read = serial_485_read,
	.write = serial_485_write,
};

void serial_485_init(void)
{
	dml_register_device(&serial_485_red_dev);
}

EXPORT_DIRVER(serial_485_init)
