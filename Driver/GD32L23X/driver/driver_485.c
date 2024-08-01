/**
 * @file driver_485.c
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

#include "driver_485.h"
#include "dml_init.h"
#include "gd32l23x.h"

static const char serial_485_name[] = "/dev/serial_485";
static dml_dev_err_e serial_485_open(void);
static dml_dev_err_e serial_485_close(void);
static dml_dev_err_e serial_485_ioctrl(int cmd, void *arg);
static int serial_485_read(uint8_t *buf, size_t len);
static int serial_485_write(const uint8_t *buf, size_t len);

static dml_dev_err_e serial_485_open(void)
{
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e serial_485_close(void)
{
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e serial_485_ioctrl(int cmd, void *arg)
{
	return DML_DEV_ERR_NONE;
}

static int serial_485_read(uint8_t *buf, size_t len)
{
	;
	return 1;
}

static int serial_485_write(const uint8_t *buf, size_t len)
{
	
	return 1;
}

static dml_file_opts_t serial_485_red_dev = {
	.close = serial_485_close,
	.ioctrl = serial_485_ioctrl,
	.name = serial_485_name,
	.open = serial_485_open,
	.read = serial_485_read,
	.write = serial_485_write,
};

void serial_485_init(void)
{

	dml_register_device(&serial_485_red_dev);
}

EXPORT_DIRVER(serial_485_init)

