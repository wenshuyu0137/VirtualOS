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
	if (fd >= RESERVED_FD_NUM)
		return dal_read(fd, buf, len);

	return fd;
}

static int write_f(const uint8_t *buf, size_t len)
{
	if (fd >= RESERVED_FD_NUM)
		return dal_write(fd, buf, len);

	return fd;
}

//测试回复
static cep_err_e index_10_test(size_t *len, uint8_t *p_in_out)
{
	uint8_t test[] = { 0x12, 0x34, 0x56 };
	memcpy(p_in_out, test, sizeof(test));
	*len = sizeof(test);
	return CPE_ERR_NONE;
}

//测试不回复
static cep_err_e index_100_test(size_t *len, uint8_t *p_in_out)
{
	uint8_t test[] = { 0x12, 0x34, 0x56 };
	memcpy(p_in_out, test, sizeof(test));
	*len = sizeof(test);
	return CPE_ERR_EMIT;
}

static void index_32_master_task(void)
{
	static uint16_t counter = 0;
	uint8_t test[] = { 0x78, 0x9A, 0xBC };
	if (counter == 1000) {
		cep_report(CEP_CMD_LOW_FREQ, 32, test, sizeof(test));
		counter = 0;
	}
	counter += APP_485_TASK_PERIOD;
}

/**
 * @brief 所有的主动上报任务
 * 
 */
static void master_task(void)
{
	index_32_master_task();
}

static cpe_handler_t maps[] = { { .handler = index_10_test, .index = 10 }, { .handler = index_100_test, .index = 100 } };

static file_opts opts = {
	.read = read_f,
	.write = write_f,
};
/***********************************************API***********************************************/

void app_485_init(void)
{
	fd = dal_open("/dev/serial_485");

	cep_init(maps, sizeof(maps) / sizeof(maps[0]), &opts);
}

void app_485_task(void)
{
	master_task();

	cep_dispatch();
}