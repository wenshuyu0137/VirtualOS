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

#include "board_task_timer.h"

void board_task_timer_init(uint32_t m_ms, hal_irqn_cb_f f_timeout)
{
	timer_parameter_struct task_timer;
	rcu_periph_clock_enable(RCU_TIMER1);
	timer_deinit(TIMER1);
	//64M
	//T=（period+1）*（prescaler+1）/TIMxCLK
	task_timer.alignedmode = TIMER_COUNTER_EDGE;
	task_timer.clockdivision = TIMER_CKDIV_DIV1;
	task_timer.counterdirection = TIMER_COUNTER_UP;
	task_timer.period = (m_ms * 10 - 1);
	task_timer.prescaler = 6399;
	timer_init(TIMER1, &task_timer);
	timer_flag_clear(TIMER1, TIMER_INT_UP);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	nvic_irq_enable(TIMER1_IRQn, 0);
	board_irqn_cb_register(f_timeout, HAL_IRQ_TYPE_TIM1);
}

void board_task_timer_start(void)
{
	timer_enable(TIMER1);
}
