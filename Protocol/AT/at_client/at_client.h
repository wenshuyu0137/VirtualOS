/**
 * @file at_client.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 轻量级简易AT客户端协议组件
 * @version 0.1
 * @date 2024-07-12
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

#ifndef _VIRTUAL_OS_AT_CLIENT_H
#define _VIRTUAL_OS_AT_CLIENT_H

#include <stdint.h>
#include "at_client_cmd.h"
#include "queue.h"

typedef size_t (*at_client_opt_read)(char *read_buf, size_t size);
typedef size_t (*at_client_opt_write)(char *write_buf, size_t size);

typedef struct {
	at_client_opt_read f_read;
	at_client_opt_write f_write;
} at_client_opts_t;

/**
 * @brief 注册接口
 *
 * @param content 收到的字符串回复(包含了结束符\0)
 * @param len 字节长度(包含了结束符\0)
 * @param result 收到是OK还是ERROR
 */
typedef void (*at_client_handler_f)(const char *const content, size_t len, bool result);

typedef struct {
	at_client_cmd_type_e cmd_type;
	at_client_handler_f handle_f;
} at_client_handle_t;

/**********************************************************API**********************************************************/

bool at_client_init(at_client_opts_t *opts, at_client_handle_t *handle_maps, size_t map_size);
void at_client_dispatch(void);
void at_client_send(at_client_cmd_type_e cmd_type);

#endif /* _VIRTUAL_OS_AT_CLIENT_H */
