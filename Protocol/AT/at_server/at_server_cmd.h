/**
 * @file at_server_cmd.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 用户自定义添加服务端所支持的AT指令
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

#ifndef _VIRTUAL_OS_AT_SERVER_CMD_H
#define _VIRTUAL_OS_AT_SERVER_CMD_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define AT_SERVER_PREFIX "AT"

// 行结束符
#ifdef SERVER_SUFFIX_WITH_CR
#define SERVER_AT_SUFFIX '\r'
#elif defined(SERVER_SUFFIX_WITH_LF)
#define SERVER_AT_SUFFIX '\n'
#else
#define SERVER_AT_SUFFIX "\r\n"
#endif

// 所有的指令枚举变量，用户自定义添加
typedef enum {
	AT_SERVER_CMD_AT, // 测试指令
	AT_SERVER_CMD_CWJAP, //连接WIFI指令

	/* Reserved */
	AT_SERVER_CMD_MAX,
} at_server_cmd_type_e;

typedef struct {
	const char *const cmd; //没有前缀,没有后缀只有具体的指令
	const at_server_cmd_type_e cmd_type; // 命令类型
} at_server_cmd_t;

extern const at_server_cmd_t at_server_cmd[AT_SERVER_CMD_MAX];

#endif /*_VIRTUAL_OS_AT_SERVER_CMD_H*/