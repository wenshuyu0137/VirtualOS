// /**
//  * @file board_usart.h
//  * @author wenshuyu (wsy2161826815@163.com)
//  * @brief
//  * @version 0.1
//  * @date 2024-07-06
//  *
//  * @copyright Copyright (c) 2024
//  *
//  * Permission is hereby granted, free of charge, to any person obtaining a copy
//  * of this software and associated documentation files (the "Software"), to deal
//  * in the Software without restriction, including without limitation the rights
//  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  * copies of the Software, and to permit persons to whom the Software is
//  * furnished to do so, subject to the following conditions:
//  *
//  * The above copyright notice and this permission notice shall be included in all
//  * copies or substantial portions of the Software.
//  *
//  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  * SOFTWARE.
//  *
//  */

// #ifndef _VIRTUAL_OS_BOARD_USART_H
// #define _VIRTUAL_OS_BOARD_USART_H

// #include <stdio.h>
// #include <stdint.h>
// #include "gd32l23x.h"
// #include "gd32l23x_it.h"
// #include "hal_usart.h"
// #include "queue.h"
// #include "board_irqn_cb.h"

// typedef void (*board_usart_cb_f)(void);

// typedef struct {
// 	IRQn_Type irqn; //中断号
// 	dma_channel_enum dma_channel; //DMA通道
// 	uint16_t pre_trans_bytes; //上一次的发送值
// 	queue_info_t queue; //队列
// 	board_usart_cb_f cb;
// } board_usart_channel_t;

// typedef struct {
// 	uint32_t clk; //RCU时钟
// 	uint32_t gd_usart; //GD具体的串口号
// } chip_info_t;

// typedef struct {
// 	uint8_t id;
// 	chip_info_t chip_info;
// 	board_usart_channel_t tx_channel;
// 	board_usart_channel_t rx_channel;
// } board_usart_instance_t;

// #define USART_IO_INIT_HELPER(_io)                                                                                                                              \
// 	(rcu_periph_clock_enable(RCU_REGIDX_BIT(AHBEN_REG_OFFSET, 17 + ((_io)->HAL_GPIO_PORT))),                                                               \
// 	 gpio_af_set(((GPIO_BASE + (0x00000400U * ((_io)->HAL_GPIO_PORT)))), GPIO_AF_7, BIT((_io)->HAL_GPIO_PIN)),                                             \
// 	 gpio_mode_set(GPIO_BASE + (0x00000400U * ((_io)->HAL_GPIO_PORT)), GPIO_MODE_AF, GPIO_PUPD_PULLUP, BIT((_io)->HAL_GPIO_PIN)),                          \
// 	 gpio_output_options_set(((GPIO_BASE + (0x00000400U * ((_io)->HAL_GPIO_PORT)))), GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, BIT((_io)->HAL_GPIO_PIN)))

// bool board_usart_init(hal_usart_t *p_usart, hal_usart_hw_cfg_t *hw_cfg, hal_usart_usr_cfg_t *usr_tx_cfg, hal_usart_usr_cfg_t *usr_rx_cfg);
// size_t board_usart_write(hal_usart_t *p_usart, uint8_t *buf, size_t len);
// size_t board_usart_read(hal_usart_t *p_usart, uint8_t *data, size_t len);
// int board_usart_flush(hal_usart_t *p_usart, hal_usart_mode_e mode);
// #endif /*_VIRTUAL_OS_BOARD_USART_H*/