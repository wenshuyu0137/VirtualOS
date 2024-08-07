/**
 * @file cloud_electricity.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
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

#include <stdint.h>
#include "queue.h"

typedef enum {
	CEP_FRAME_TYPE_LOW_FREQ = 0xE1, //低频 CMD E1
	CEP_FRAME_TYPE_HIGH_FREQ, //高频 CMD E2
	CEP_FRAME_TYPE_FLEXABLE_DATA, //可变长数据 CMD E3 (似乎启用)
	CEP_FRAME_TYPE_LOG_STATUS, //日志状态 CMD E4
	CEP_FRAME_TYPE_CLOULD_NOTIFY, //云端下发 CMD E5
	CEP_FRAME_TYPE_LOG_CONTENT, //日志内容 CMD E6
} cep_frame_type_e;

typedef struct {
	int (*read)(uint8_t *buf, size_t len);
	int (*write)(const uint8_t *buf, size_t len);
} file_opts;

typedef enum {
	CPE_HANDLER_ERR_NONE, //无错误
	CPE_HANDLER_ERR_NOT_REPLY, //不回复
} cpe_handler_err_e;

typedef cpe_handler_err_e (*handler_f)(size_t *len, uint8_t *p_in_out);

typedef struct {
	uint8_t index; //索引
	handler_f handler; //对应的处理函数
} cpe_handler_t;

typedef struct {
	cpe_handler_t *handlers;
	size_t size;
} cpe_hanlder_maps_t; //处理表

/*******************************************************************API*******************************************************************/

int cep_init(cpe_handler_t *handlers, size_t size, file_opts *opts);
void cep_dispatch(void);

#endif /*_CLOUD_ELECTRICITY_PROTOCOL_H*/