/**
 * @file app_at_server.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-08
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

#include "app_at_server.h"

static int fd = -1;

static size_t read_f(char *buf, size_t len)
{
	if (fd >= 0)
		return (size_t)dal_read(fd, (uint8_t *)buf, len);

	return 0;
}

static size_t write_f(char *buf, size_t len)
{
	if (fd >= 0)
		return (size_t)dal_write(fd, (uint8_t *)buf, len);

	return 0;
}

static at_server_opts_t opts = {
	.f_read = read_f,
	.f_write = write_f,
};

static at_server_response_e at_test_handle(at_server_msg_t *msg, char *out, size_t *out_size)
{
	char test[] = "hello world\n";

	memcpy(out, test, sizeof(test));
	*out_size = sizeof(test);

	return AT_SERVER_RESPONSE_READY;
}

static at_server_response_e at_CWJAP_handle(at_server_msg_t *msg, char *out, size_t *out_size)
{
	switch (msg->handle_type) {
	case AT_SERVER_HANDLE_QUERY: {
		char test[] = "find cmd\n";

		memcpy(out, test, sizeof(test));
		*out_size = sizeof(test);
	}

	break;
	case AT_SERVER_HANDLE_SET: {
		char test[] = "set cmd\r\n";
		*out_size = 0;
		memcpy(out, test, sizeof(test));
		*out_size += sizeof(test);

		for (int i = 0; i < msg->argc; i++) {
			memcpy(out + *out_size, msg->argv[i], strlen(msg->argv[i]));
			*out_size += strlen(msg->argv[i]);

			memcpy(out + *out_size, "\r\n", strlen("\r\n"));
			*out_size += strlen("\r\n");
		}
	}

	break;
	default:
		break;
	}

	return AT_SERVER_RESPONSE_READY;
}

at_server_handle_t maps[] = {
	[AT_SERVER_CMD_AT] = { .cmd_type = AT_SERVER_CMD_AT, .handle_f = at_test_handle },
	[AT_SERVER_CMD_CWJAP] = { .cmd_type = AT_SERVER_CMD_CWJAP, .handle_f = at_CWJAP_handle },
};

void app_at_server_init(void)
{
	fd = dal_open("/dev/serial_485");

	at_server_init(&opts, maps, sizeof(maps) / sizeof(maps[0]));
}

void app_at_server_task(void)
{
	at_server_dispatch();
}