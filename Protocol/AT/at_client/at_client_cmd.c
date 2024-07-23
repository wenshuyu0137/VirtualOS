/**
 * @file at_client_cmd.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 用户自定义添加所使用的AT指令
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

#include "at_client_cmd.h"

#define AT_CLIENT_CMD_HELPER(_S, _T) ((at_client_cmd_t){ .cmd = (_S), .cmd_type = (_T) })

const at_client_cmd_t at_client_cmd[AT_CLIENT_CMD_MAX] = {
	[AT_CLIENT_CMD_AT] = AT_CLIENT_CMD_HELPER("AT\r\n", AT_CLIENT_CMD_AT), // 空指令，用于测试通讯
};
