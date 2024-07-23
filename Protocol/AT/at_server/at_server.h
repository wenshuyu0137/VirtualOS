/**
 * @file at_server.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 轻量级简易AT服务端协议组件,依赖队列组件与字符串哈希组件
 * @version 0.1
 * @date 2024-07-17
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

#ifndef _VIRTUAL_OS_AT_SERVER_H
#define _VIRTUAL_OS_AT_SERVER_H

#include <stdint.h>
#include "at_server_cmd.h"
#include "queue.h"
#include "string_hash.h"

typedef size_t (*at_server_opt_read)(char *read_buf, size_t size);
typedef size_t (*at_server_opt_write)(char *write_buf, size_t size);

typedef struct {
	at_server_opt_read f_read;
	at_server_opt_write f_write;
} at_server_opts_t;

typedef enum {
	AT_SERVER_HANDLE_SET, //设置指令
	AT_SERVER_HANDLE_QUERY, //查询指令
} at_server_handle_e;

typedef enum {
	AT_SERVER_RESPONSE_NONE, //不回复
	AT_SERVER_RESPONSE_READY, //回复
	AT_SERVER_RESPONSE_ERROR, //错误
} at_server_response_e;

typedef struct {
	char complete[256]; //完整的一条指令 限制长度255 + '\0'位
	char cmd[16]; //具体某一个指令 限制长度15 + '\0'位
	char argv[8][32]; //参数 限制最多8个参数 每个参数长度最多31 + '\0'位
	uint8_t argc; //参数条数
	at_server_handle_e handle_type; //查询指令还是设置指令
} at_server_msg_t;

/**
 * @brief 回调函数,处理每一个AT指令的请求
 *
 * @param msg 请求指令所包含的信息
 * @param out 处理完后回复的数据
 * @param out_size 处理完后回复的数据长度
 */
typedef at_server_response_e (*at_server_handler_f)(at_server_msg_t *msg, char *out, size_t *out_size);

typedef struct {
	at_server_cmd_type_e cmd_type;
	at_server_handler_f handle_f;
} at_server_handle_t;

/**********************************************************API**********************************************************/

bool at_server_init(at_server_opts_t *opts, at_server_handle_t *handle_maps, size_t map_size);
void at_server_dispatch(void);

#endif /*_VIRTUAL_OS_AT_SERVER_H*/