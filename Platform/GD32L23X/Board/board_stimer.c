/**
 * @file board_task_timer.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
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

#include "board_stimer.h"
#include "dml_init.h"

stimer_timeout_process stimer_cb = NULL; //定时器中断

/**
 * @brief 实现任务调度定时器的初始化 
 * 
 * @param period 周期(ms)
 * @param f_timeout 中断回调函数,在定时器中断中调用
 */
static void _stimer_base_init(uint32_t period, stimer_timeout_process f_timeout);
static void _stimer_base_start(void);
static struct timer_port m_tmr = {
		.f_init = _stimer_base_init,
		.f_start = _stimer_base_start,
	};

static void _stimer_base_init(uint32_t period, stimer_timeout_process f_timeout)
{
	timer_parameter_struct task_timer;
	rcu_periph_clock_enable(RCU_TIMER1);
	timer_deinit(TIMER1);

	//64M
	//T=（period+1）*（prescaler+1）/TIMxCLK

	task_timer.alignedmode = TIMER_COUNTER_EDGE;
	task_timer.clockdivision = TIMER_CKDIV_DIV1;
	task_timer.counterdirection = TIMER_COUNTER_UP;
	task_timer.period = (period * 10 - 1);
	task_timer.prescaler = 6399;
	timer_init(TIMER1, &task_timer);
	timer_flag_clear(TIMER1, TIMER_INT_UP);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	nvic_irq_enable(TIMER1_IRQn, 0);

	stimer_cb = f_timeout; //中断回调
}

/**
 * @brief 启动调度定时器
 * 
 */
static void _stimer_base_start(void)
{
	timer_enable(TIMER1);
}

void TIMER1_IRQHandler(void)
{
	if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) != RESET) {
		if (stimer_cb) {
			stimer_cb();
		}
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
	}
}

EXPORT_DIRVER(platform_stimer_init);
void platform_stimer_init(void)
{
	stimer_init(&m_tmr);
}

