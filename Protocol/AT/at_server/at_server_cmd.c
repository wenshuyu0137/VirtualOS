/**
 * @file at_server_cmd.c
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

#include "at_server_cmd.h"

#define AT_SERVER_CMD_HELPER(_S, _T) ((at_server_cmd_t){ .cmd = (_S), .cmd_type = (_T) })

const at_server_cmd_t at_server_cmd[AT_SERVER_CMD_MAX] = {
	[AT_SERVER_CMD_AT] = AT_SERVER_CMD_HELPER("", AT_SERVER_CMD_AT), // 空指令，用于测试通讯
	[AT_SERVER_CMD_CWJAP] = AT_SERVER_CMD_HELPER("CWJAP", AT_SERVER_CMD_CWJAP), // 例如AT+CWJAP="espressif","1234567890" 只取CWJAP部分
};
