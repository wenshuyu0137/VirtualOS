/**
 * @file board_led_1.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-31
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

#include "board_led_1.h"
#include "gd32l23x.h"

const char led_name[] = "LED_RED";
dml_dev_err_e led_open(void);
dml_dev_err_e led_close(void);
dml_dev_err_e led_ioctrl(void *arg);
int led_read(uint8_t *buf, size_t len);
int led_write(const uint8_t *buf, size_t len);

static dml_char_dev_t led_red_dev = {
	.close = led_close,
	.ioctrl = led_ioctrl,
	.name = led_name,
	.open = led_open,
	.read = led_read,
	.write = led_write,
};

dml_dev_err_e led_open(void)
{
	return DML_DEV_ERR_NONE;
}

dml_dev_err_e led_close(void)
{
	return DML_DEV_ERR_NONE;
}

dml_dev_err_e led_ioctrl(void *arg)
{
	return DML_DEV_ERR_NONE;
}

int led_read(uint8_t *buf, size_t len)
{
	gpio_input_bit_get(GPIOB, GPIO_PIN_5);
    return 1;
}

int led_write(const uint8_t *buf, size_t len)
{
	gpio_bit_write(GPIOB, GPIO_PIN_5, (*buf ? SET : RESET));
    return 1;
}

//注册LED设备
void led_red_init(void)
{
	gpio_deinit(GPIOB);
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	dml_register_device(&led_red_dev);
}