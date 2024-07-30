// /**
//  * @file hal_usart.h
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

// #ifndef _VIRTUAL_OS_HAL_USART_H
// #define _VIRTUAL_OS_HAL_USART_H

// #include <stdint.h>
// #include <stdbool.h>
// #include "hal_gpio.h"
// #include "queue.h"

// /*串口ID*/
// typedef enum {
// 	HAL_USART_ID_0,
// 	HAL_USART_ID_1,
// 	HAL_USART_ID_2,
// 	HAL_USART_ID_3,
// 	HAL_USART_ID_4,
// 	HAL_USART_ID_5,
// 	HAL_USART_ID_6,
// 	HAL_USART_ID_7,
// } hal_usart_id_e;

// /*波特率*/
// typedef enum {
// 	HAL_UART_BAUDRATE_1200,
// 	HAL_UART_BAUDRATE_2400,
// 	HAL_UART_BAUDRATE_4800,
// 	HAL_UART_BAUDRATE_9600,
// 	HAL_UART_BAUDRATE_19200,
// 	HAL_UART_BAUDRATE_38400,
// 	HAL_UART_BAUDRATE_57600,
// 	HAL_UART_BAUDRATE_115200,
// 	HAL_UART_BAUDRATE_250000,
// 	HAL_UART_BAUDRATE_921600,
// 	HAL_UART_BAUDRATE_1000000,
// } hal_baudrate_e;

// /*数据位*/
// typedef enum {
// 	HAL_USART_DATA_8BIT,
// 	HAL_USART_DATA_9BIT,
// } hal_usart_data_len_e;

// /*校验位*/
// typedef enum {
// 	HAL_USART_PARITY_NONE,
// 	HAL_USART_PARITY_ODD,
// 	HAL_USART_PARITY_EVEN,
// } hal_usart_parity_e;

// /*停止位*/
// typedef enum {
// 	HAL_USART_STOP_1_BIT,
// 	HAL_USART_STOP_0_5_BIT,
// 	HAL_USART_STOP_2_BIT,
// 	HAL_USART_STOP_1_5_BIT,
// } hal_usart_stop_bits_e;

// /*硬件控制流*/
// typedef enum {
// 	HAL_USART_HARD_FLOW_NONE,
// 	HAL_USART_HARD_FLOW_CTS,
// 	HAL_USART_HARD_FLOW_RTS,
// 	HAL_USART_HARD_FLOW_RTS_CTS,
// } hal_usart_hard_flow_e;

// /*收发模式*/
// typedef enum {
// 	HAL_USART_MODE_SND_ONLY = (1 << 0),
// 	HAL_USART_MODE_RCV_ONLY = (1 << 1),
// 	HAL_USART_MODE_BOTH = (HAL_USART_MODE_SND_ONLY | HAL_USART_MODE_RCV_ONLY),
// } hal_usart_mode_e;

// /*硬件配置项*/
// typedef struct {
// 	uint16_t baudrate : 4;
// 	uint16_t flow : 3;
// 	uint16_t mode : 2;
// 	uint16_t data_len : 2;
// 	uint16_t stop_bits : 2;
// 	uint16_t parity : 2;
// 	uint16_t duplex : 1;
// } hal_usart_hw_cfg_t;

// /*硬件配置快捷宏定义*/
// #define HAL_USART_HW_CFG_HELPER(bdr, flw, mod, len, stb, prt, dpx)                                                                                             \
// 	((hal_usart_hw_cfg_t){                                                                                                                                 \
// 		.baudrate = (bdr) & 0x0F,                                                                                                                      \
// 		.flow = (flw) & 0x07,                                                                                                                          \
// 		.mode = (mod) & 0x03,                                                                                                                          \
// 		.data_len = (len) & 0x03,                                                                                                                      \
// 		.stop_bits = (stb) & 0x03,                                                                                                                     \
// 		.parity = (prt) & 0x03,                                                                                                                        \
// 		.duplex = (dpx) & 0x01,                                                                                                                        \
// 	})

// /*中断回调*/
// typedef void (*usart_cb)(void);

// /*发送与接收的配置项*/
// typedef struct {
// 	uint8_t *buffer;
// 	uint16_t buffer_size;
// 	usart_cb callback; //中断回调
// 	hal_gpio_t io; // IO口
// } hal_usart_usr_cfg_t;

// /*串口实例*/
// typedef struct {
// 	hal_usart_id_e usart_id;
// 	void *priv_data;
// } hal_usart_t;

// /*快捷实例化*/
// #define HAL_USART_CTOR(_U) ((hal_usart_t){ .usart_id = _U, .priv_data = (void *)0 })

// /**********************************************************API**********************************************************/
// bool hal_usart_init(hal_usart_t *p_usart, hal_usart_hw_cfg_t *hw_cfg, hal_usart_usr_cfg_t *usr_tx_cfg, hal_usart_usr_cfg_t *usr_rx_cfg);
// size_t hal_usart_write(hal_usart_t *p_usart, uint8_t *buf, size_t len);
// size_t hal_usart_read(hal_usart_t *p_usart, uint8_t *data, size_t len);
// int hal_usart_flush(hal_usart_t *p_usart, hal_usart_mode_e mode);

// #endif /*_VIRTUAL_OS_HAL_USART_H*/