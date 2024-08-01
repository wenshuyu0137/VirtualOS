/**
 * @file driver_led_green.c
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

#include "driver_led_gree.h"
#include "dml_init.h"

static const char led_name[] = "/dev/led_green";
static dml_dev_err_e led_open(void);
static dml_dev_err_e led_close(void);
static dml_dev_err_e led_ioctrl(int cmd, void *arg);
static int led_read(uint8_t *buf, size_t len);
static int led_write(const uint8_t *buf, size_t len);

static bool is_led_opened = false;

static dml_dev_err_e led_open(void)
{
	if (is_led_opened)
		return DML_DEV_ERR_OCCUPIED;

	is_led_opened = true;

	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e led_close(void)
{
	gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_5); //低功耗设置为输入模式

	is_led_opened = false;
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e led_ioctrl(int cmd, void *arg)
{
	return DML_DEV_ERR_NONE;
}

static int led_read(uint8_t *buf, size_t len)
{
	if(!is_led_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	gpio_input_bit_get(GPIOB, GPIO_PIN_5);
	return 1;
}

static int led_write(const uint8_t *buf, size_t len)
{
	if(!is_led_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	gpio_bit_write(GPIOB, GPIO_PIN_5, (*buf ? SET : RESET));
	return 1;
}

static dml_file_opts_t led_red_dev = {
	.close = led_close,
	.ioctrl = led_ioctrl,
	.name = led_name,
	.open = led_open,
	.read = led_read,
	.write = led_write,
};

void led_green_init(void)
{
	dml_register_device(&led_red_dev);
}

EXPORT_DIRVER(led_green_init) //注册驱动
