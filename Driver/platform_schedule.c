/**
 * @file platform_schedule.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 框架调度核心实现,请提供一个定时器用于框架调度
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

/******************************************************************************************
 *                                      此部分禁止修改                                      *
 ******************************************************************************************/

#include "dml_init.h"
#include "stimer.h"

stimer_timeout_process stimer_cb = NULL;
static void _stimer_base_init(uint32_t period, stimer_timeout_process f_timeout);
static void _stimer_base_start(void);

struct timer_port m_tmr = {
	.f_init = _stimer_base_init,
	.f_start = _stimer_base_start,
};

void platform_stimer_init(void)
{
	stimer_init(&m_tmr);
}

EXPORT_DIRVER(platform_stimer_init);

/******************************************************************************************/

#if 0
#error "你必须手动实现以下函数的具体内容,并在中断处理函数中调用f_timeout回调函数"
#error "实现后请手动关闭这条错误警告"
#endif

#define GD32L23x
// #define GD32F30x

#ifdef GD32L23x

#include "gd32l23x.h"

/**
 * @brief 此处必须实现一个定时器的初始化的操作
 * 
 * @param period 周期
 * @param f_timeout 中断回调, stimer_cb = f_timeout; 保存到stimer_cb，在中断函数中调用
 */
static void _stimer_base_init(uint32_t period, stimer_timeout_process f_timeout)
{
	timer_parameter_struct task_timer;
	rcu_periph_clock_enable(RCU_TIMER1);
	timer_deinit(TIMER1);

	task_timer.alignedmode = TIMER_COUNTER_EDGE;
	task_timer.clockdivision = TIMER_CKDIV_DIV1;
	task_timer.counterdirection = TIMER_COUNTER_UP;
	task_timer.period = (period * 10 - 1);
	task_timer.prescaler = 64 * 100 - 1; //64M
	timer_init(TIMER1, &task_timer);
	timer_flag_clear(TIMER1, TIMER_INT_UP);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	nvic_irq_enable(TIMER1_IRQn, 0);

	stimer_cb = f_timeout;
}

/**
 * @brief 实现定时器的启动函数
 * 
 */
static void _stimer_base_start(void)
{
	timer_enable(TIMER1);
}

/**
 * @brief 中断处理函数，调用回调函数
 * 
 */
void TIMER1_IRQHandler(void)
{
	if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) != RESET) {
		if (stimer_cb)
			stimer_cb();

		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
	}
}

#elif defined(GD32F30x)

#include "gd32f30x.h"

/**
 * @brief 此处必须实现一个定时器的初始化的操作
 * 
 * @param period 周期
 * @param f_timeout 中断回调, stimer_cb = f_timeout; 保存到stimer_cb，在中断函数中调用
 */
static void _stimer_base_init(uint32_t period, stimer_timeout_process f_timeout)
{
	timer_parameter_struct task_timer;
	rcu_periph_clock_enable(RCU_TIMER1);
	timer_deinit(TIMER1);

	task_timer.alignedmode = TIMER_COUNTER_EDGE;
	task_timer.clockdivision = TIMER_CKDIV_DIV1;
	task_timer.counterdirection = TIMER_COUNTER_UP;
	task_timer.period = (period * 10 - 1);
	task_timer.prescaler = 120 * 100 - 1; //120M
	timer_init(TIMER1, &task_timer);
	timer_flag_clear(TIMER1, TIMER_INT_UP);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	nvic_irq_enable(TIMER1_IRQn, 0, 0);

	stimer_cb = f_timeout;
}

/**
 * @brief 实现定时器的启动函数
 * 
 */
static void _stimer_base_start(void)
{
	timer_enable(TIMER1);
}

/**
 * @brief 中断处理函数，调用回调函数
 * 
 */
void TIMER1_IRQHandler(void)
{
	if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) != RESET) {
		if (stimer_cb)
			stimer_cb();

		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
	}
}

#endif
