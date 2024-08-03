/**
 * @file driver_rgb_green.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-03
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

#include "dml_init.h"
#include "dml_char_device.h"
#include "dml_init.h"
#include "gd32f30x.h"

static const char pwm_green_name[] = "/dev/pwm_green";
static dml_dev_err_e pwm_green_open(void);
static dml_dev_err_e pwm_green_close(void);
static dml_dev_err_e pwm_green_ioctrl(int cmd, void *arg);
static int pwm_green_read(uint8_t *buf, size_t len);
static int pwm_green_write(const uint8_t *buf, size_t len);

static bool is_pwm_green_opened = false;

#define TIMER_PERIOD (500 - 1)

static dml_dev_err_e pwm_green_open(void)
{
	if (is_pwm_green_opened)
		return DML_DEV_ERR_OCCUPIED;

	is_pwm_green_opened = true;

	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

	/* TIMER0 configuration: generate PWM signals with different duty cycles:
       TIMER0CLK = SystemCoreClock / 120 = 1MHz */
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER0);

	/* TIMER0 configuration */
	timer_initpara.prescaler = 119;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = TIMER_PERIOD;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER0, &timer_initpara);

	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
	timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
	timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_HIGH;
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

	timer_primary_output_config(TIMER0, ENABLE);
	timer_auto_reload_shadow_enable(TIMER0);
	timer_enable(TIMER0);

	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e pwm_green_close(void)
{
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, TIMER_PERIOD + 1);

	is_pwm_green_opened = false;
	return DML_DEV_ERR_NONE;
}

typedef enum {
	PWM_LED_CMD_E_SET_PERIOD,
	PWM_LED_CMD_E_SET_DUTY,
} PWM_LED_CMD_E;

static dml_dev_err_e pwm_green_ioctrl(int cmd, void *arg)
{
	switch (cmd) {
	case PWM_LED_CMD_E_SET_PERIOD:
		timer_autoreload_value_config(TIMER0, *(int *)arg);
		break;
	case PWM_LED_CMD_E_SET_DUTY:
		timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, *(int *)arg);
		break;
	default:
		break;
	}
	return DML_DEV_ERR_NONE;
}

static int pwm_green_read(uint8_t *buf, size_t len)
{
	if (!is_pwm_green_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	return 0;
}

static int pwm_green_write(const uint8_t *buf, size_t len)
{
	if (!is_pwm_green_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	return 0;
}

static dml_file_opts_t rgb_pwm_green_dev = {
	.close = pwm_green_close,
	.ioctrl = pwm_green_ioctrl,
	.open = pwm_green_open,
	.read = pwm_green_read,
	.write = pwm_green_write,
};

void rgb_pwm_green_init(void)
{
	dml_register_device(&rgb_pwm_green_dev, pwm_green_name);
}

EXPORT_DIRVER(rgb_pwm_green_init)
