/**
 * @file driver_beep.c
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

#include "dml_init.h"
#include "dml_char_device.h"
#include "dml_init.h"
#include "gd32f30x.h"

static const char beep_name[] = "/dev/beep";
static dml_dev_err_e beep_open(void);
static dml_dev_err_e beep_close(void);
static dml_dev_err_e beep_ioctrl(int cmd, void *arg);
static int beep_read(uint8_t *buf, size_t len);
static int beep_write(const uint8_t *buf, size_t len);

static bool is_beep_opened = false;

static dml_dev_err_e beep_open(void)
{
	if (is_beep_opened)
		return DML_DEV_ERR_OCCUPIED;

	is_beep_opened = true;

	rcu_periph_clock_enable(RCU_GPIOF);
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e beep_close(void)
{
	is_beep_opened = false;
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e beep_ioctrl(int cmd, void *arg)
{
	return DML_DEV_ERR_NONE;
}

static int beep_read(uint8_t *buf, size_t len)
{
	if (!is_beep_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	gpio_input_bit_get(GPIOF, GPIO_PIN_9);
	return 1;
}

static int beep_write(const uint8_t *buf, size_t len)
{
	if (!is_beep_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	gpio_bit_write(GPIOF, GPIO_PIN_9, (*buf ? SET : RESET));
	return 1;
}

static dml_file_opts_t beep_dev = {
	.close = beep_close,
	.ioctrl = beep_ioctrl,
	.name = beep_name,
	.open = beep_open,
	.read = beep_read,
	.write = beep_write,
};

void beep_init(void)
{
	dml_register_device(&beep_dev);
}

EXPORT_DIRVER(beep_init) //注册驱动
