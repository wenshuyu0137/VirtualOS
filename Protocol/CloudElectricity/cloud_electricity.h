/**
 * @file cloud_electricity.h
 * @author wenshuyu (wenshuyu991@hellobike.com)
 * @brief 九号云电协议,支持从机与主机模式,支持粘包断包处理
 * @version 0.1
 * @date 2024-08-07
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

#ifndef _CLOUD_ELECTRICITY_PROTOCOL_H
#define _CLOUD_ELECTRICITY_PROTOCOL_H

//cep ---> clould electricity protocol 云电协议

#include <stdint.h>
#include "queue.h"

typedef enum {
	CEP_CMD_LOW_FREQ = 0xE1, //低频 CMD E1
	CEP_CMD_HIGH_FREQ, //高频 CMD E2
	CEP_CMD_FLEXABLE_DATA, //可变长数据 CMD E3 (似乎启用)
	CEP_CMD_LOG_STATUS, //日志状态 CMD E4
	CEP_CMD_CLOULD_NOTIFY, //云端下发 CMD E5
	CEP_CMD_LOG_CONTENT, //日志内容 CMD E6
} cep_cmd_e;

typedef enum {
	CPE_ERR_NONE, //无错误
	CPE_ERR_EMIT, //触发错误
} cep_err_e;

typedef struct {
	int (*read)(uint8_t *buf, size_t len);
	int (*write)(const uint8_t *buf, size_t len);
} file_opts;

typedef cep_err_e (*handler_f)(size_t *len, uint8_t *p_in_out);

typedef struct {
	uint8_t index; //索引
	handler_f handler; //对应的处理函数
} cpe_handler_t;

/*******************************************************************API*******************************************************************/

/**
 * @brief 协议初始化
 * 
 * @param handlers 回调处理表
 * @param size 表大小
 * @param opts 读写回调指针
 * @return int 成功返回0 失败返回1
 */
int cep_init(cpe_handler_t *handlers, size_t size, file_opts *opts);

/**
 * @brief 协议调度处理
 * 
 */
void cep_dispatch(void);

/**
 * @brief 主动发送接口
 * 
 * @param cmd 指令
 * @param index 数据索引
 * @param data 数据内容
 * @param len 数据长度
 */
void cep_report(cep_cmd_e cmd, uint8_t index, uint8_t *data, uint8_t len);

#endif /*_CLOUD_ELECTRICITY_PROTOCOL_H*/