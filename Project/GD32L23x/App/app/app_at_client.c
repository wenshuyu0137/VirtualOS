/**
 * @file app_at_client.c
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

#include "app_at_client.h"

static int fd = -1;

static size_t read_f(char *buf, size_t len)
{
	if (fd >= 0)
		return dal_read(fd, (uint8_t *)buf, len);

	return 0;
}

static size_t write_f(char *buf, size_t len)
{
	if (fd >= 0)
		return dal_write(fd, (uint8_t *)buf, len);

	return 0;
}

static at_client_opts_t opts = {
	.f_read = read_f,
	.f_write = write_f,
};

static void at_test(const char *const content, size_t len, bool result)
{
	write_f((char *)content, len); //回读
}

static void at_what(const char *const content, size_t len, bool result)
{
	write_f((char *)content, len); //回读
}

static at_client_handle_t maps[] = {
	[AT_CLIENT_CMD_AT] = { .cmd_type = AT_CLIENT_CMD_AT, .handle_f = at_test },
	[AT_CLIENT_CMD_WHAT] = { .cmd_type = AT_CLIENT_CMD_WHAT, .handle_f = at_what },
};

static void app_at_cmd_test_task(void)
{
	static uint16_t counter = 0;
	if (counter == 1000) {
		at_client_send(AT_CLIENT_CMD_AT);
		counter = 0;
	}
	counter += APP_AT_CLIENT_TASK_PERIOD;
}

static void app_at_cmd_what_task(void)
{
	static uint16_t counter = 0;
	if (counter == 1500) {
		at_client_send(AT_CLIENT_CMD_WHAT);
		counter = 0;
	}
	counter += APP_AT_CLIENT_TASK_PERIOD;
}

static void app_at_client_master(void)
{
	//所有主动发送发的任务
	app_at_cmd_test_task();

	app_at_cmd_what_task();
}

/***************************************API***************************************/

void app_at_client_init(void)
{
	fd = dal_open("/dev/serial_485");

	at_client_init(&opts, maps, sizeof(maps) / sizeof(maps[0]));
}

void app_at_client_task(void)
{
	at_client_dispatch();

	app_at_client_master();
}