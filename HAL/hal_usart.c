/**
 * @file hal_usart.c
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

#include "hal_usart.h"
#include "board_usart.h"

/**
 * @brief 判断串口是否初始化
 *
 * @param p_usart 串口实例指针
 * @return bool
 */
static inline bool is_usart_initial(const hal_usart_t *p_usart)
{
	return (p_usart && p_usart->priv_data);
}

/**
 * @brief 串口初始化
 *
 * @param p_usart 串口实例指针
 * @param hw_cfg 串口硬件配置
 * @param usr_tx_cfg 发送配置
 * @param usr_rx_cfg 接收配置
 * @return bool
 */
bool hal_usart_init(hal_usart_t *p_usart, hal_usart_hw_cfg_t *hw_cfg, hal_usart_usr_cfg_t *usr_tx_cfg, hal_usart_usr_cfg_t *usr_rx_cfg)
{
	return board_usart_init(p_usart, hw_cfg, usr_tx_cfg, usr_rx_cfg);
}

/**
 * @brief 串口发送
 *
 * @param p_usart 串口实例指针
 * @param buf 发送数据缓冲区
 * @param len 数据长度
 * @return int 成功返回发送数据长度 失败返回0
 */
size_t hal_usart_write(hal_usart_t *p_usart, uint8_t *buf, size_t len)
{
	if (!is_usart_initial(p_usart)) {
		return 0;
	}

	return board_usart_write(p_usart, buf, len);
}

/**
 * @brief 读取读数据到接收缓冲区
 *
 * @param p_usart 串口实例指针
 * @param buf 接收数据缓冲区
 * @param len 数据长度
 * @return int 成功返回读取数据长度 失败返回0
 */
size_t hal_usart_read(hal_usart_t *p_usart, uint8_t *buf, size_t len)
{
	if (!is_usart_initial(p_usart)) {
		return 0;
	}

	return board_usart_read(p_usart, buf, len);
}

/**
 * @brief 清空缓冲区
 *
 * @param p_usart 串口实例指针
 * @param mode 缓冲区类型
 * @return int 成功返回刷新的数据长度 失败返回-1
 */
int hal_usart_flush(hal_usart_t *p_usart, hal_usart_mode_e mode)
{
	if (!is_usart_initial(p_usart)) {
		return -1;
	}

	return board_usart_flush(p_usart, mode);
}