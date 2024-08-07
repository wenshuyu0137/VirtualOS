/**
 * @file app_485.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-01
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

#include "app_485.h"

static int fd = -1;

static int read_f(uint8_t *buf, size_t len)
{
	if (fd >= 0)
		return dal_read(fd, buf, len);

	return fd;
}

static int write_f(const uint8_t *buf, size_t len)
{
	if (fd >= 0)
		return dal_write(fd, buf, len);

	return fd;
}

static file_opts opts = {
	.read = read_f,
	.write = write_f,
};

static cpe_handler_err_e index_10_test(size_t *len, uint8_t *p_in_out)
{
	uint8_t test[] = { 0x12, 0x34, 0x56 };
	memcpy(p_in_out, test, sizeof(test));
	*len = sizeof(test);
	return CPE_HANDLER_ERR_NONE;
}

static cpe_handler_t maps[] = { { .handler = index_10_test, .index = 10 } };

/***********************************************API***********************************************/

void app_485_init(void)
{
	fd = dal_open("/dev/serial_485");

	cep_init(maps, sizeof(maps) / sizeof(maps[0]), &opts);
}

void app_485_task(void)
{
	cep_dispatch();
}