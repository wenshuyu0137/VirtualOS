/**
 * @file stimer.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 前后台调度组件
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

#include "stimer.h"

#define HIT_LIST_MASK (STIMER_TASK_HIT_LIST_MAX - 1)
#define HIT_LIST_IDX(t) ((m_timer.pre_tick + (t)) & HIT_LIST_MASK)

TASK_T defer_pool[MAX_DEFER_TASK];

struct timer {
	volatile uint32_t pre_tick;
	volatile uint32_t cur_tick;
	volatile int run_flag;
	stimer_base_start f_start;
	list_item long_tick_list;
	list_item defer_task_list;
	list_item hit_task_list[STIMER_TASK_HIT_LIST_MAX];
};

static struct timer m_timer;

static inline int is_timer_run(void)
{
	return m_timer.run_flag == 1;
}

static inline void _timer_uptate(void)
{
	++m_timer.cur_tick;
}

#define DEFER_INIT_HELPER(_id)                                                                                                                                 \
	(TASK_T)                                                                                                                                               \
	{                                                                                                                                                      \
		.f_entry = LIST_NULL, .period = 0, .arrive = 0, .item.next = LIST_NULL, .item.pre = LIST_NULL, .defer_type = UNUSE_DEFER, .id = _id,           \
	}

static TASK_T *defer_task_allocate(void)
{
	for (int i = 0; i < MAX_DEFER_TASK; i++) {
		if (defer_pool[i].defer_type == UNUSE_DEFER) {
			return &defer_pool[i];
		}
	}

	return NULL;
}

static void defer_task_free(TASK_T *task)
{
	if (task != NULL) {
		task->f_entry = NULL;
		task->defer_type = UNUSE_DEFER;
		task->arrive = 0;
		task->period = 0;
	}
}

int defer_task_add(stimer_entry f_entry, uint32_t ms)
{
	if (!is_timer_run()) {
		return -1;
	}

	TASK_T *p_task = defer_task_allocate();

	if (!p_task) {
		return -1;
	}

	p_task->f_entry = f_entry;
	p_task->period = Period_to_Tick(ms);
	p_task->arrive = 0;
	p_task->defer_type = USE_DEFER;
	list_add_tail(&(m_timer.defer_task_list), &(p_task->item));
	return 0;
}

int stimer_init(struct timer_port *port)
{
	if (!port || !port->f_init || !port->f_start)
		return -1;

	m_timer.cur_tick = 0;
	m_timer.pre_tick = 0;
	m_timer.run_flag = 0;
	list_init(&(m_timer.long_tick_list));
	list_init(&(m_timer.defer_task_list));

	for (int i = 0; i < MAX_DEFER_TASK; i++)
		defer_pool[i] = DEFER_INIT_HELPER(i + 1);

	for (uint8_t i = 0; i < STIMER_TASK_HIT_LIST_MAX; i++)
		list_init(&(m_timer.hit_task_list[i]));

	port->f_init(STIMER_PERIOD_PER_TICK_MS, _timer_uptate);
	m_timer.f_start = port->f_start;
	return 0;
}

static inline int is_task_valid(const TASK_T *p_task)
{
	return (p_task && p_task->f_entry && p_task->period > 0);
}

static inline void _add_timer(uint32_t period, list_item *item)
{
	list_delete_item(item);

	if (period > STIMER_TASK_HIT_LIST_MAX)
		list_add_tail(&(m_timer.long_tick_list), item);
	else
		list_add_tail(&(m_timer.hit_task_list[HIT_LIST_IDX(period)]), item);
}

int stimer_task_add(TASK_T *p_task)
{
	if (!is_task_valid(p_task))
		return -1;

	p_task->arrive = 0;
	_add_timer(p_task->period, &(p_task->item));
	return 0;
}

int stimer_task_del(TASK_T *p_task)
{
	if (!is_task_valid(p_task))
		return -1;

	p_task->arrive = 0;
	return list_delete_item(&(p_task->item));
}

void stimer_start(void)
{
	if (m_timer.f_start) {
		m_timer.f_start();
		m_timer.run_flag = 1;
	}
}

uint32_t inline stimer_get_tick(void)
{
	return m_timer.cur_tick;
}

void stimer_task_dispatch(void)
{
	uint32_t idx, remain;
	struct list_item *cur_item, *next_item;
	TASK_T *task;

	if (!is_timer_run() || (m_timer.pre_tick == m_timer.cur_tick))
		return;

	++m_timer.pre_tick;
	idx = HIT_LIST_IDX(0);

	if (idx == 0) {
		list_for_each_safe (cur_item, next_item, &(m_timer.long_tick_list)) {
			task = container_of(cur_item, TASK_T, item);
			task->arrive += STIMER_TASK_HIT_LIST_MAX;
			remain = task->period - task->arrive;

			if (remain == 0) {
				task->f_entry();
				task->arrive = 0;

				if (task->carry_type == STIMER_TYPE_ONESHOT)
					list_delete_item(cur_item);

			}

			else if (remain < STIMER_TASK_HIT_LIST_MAX)
				_add_timer(remain, cur_item);
		}
	}

	list_for_each_safe (cur_item, next_item, &(m_timer.hit_task_list[idx])) {
		task = container_of(cur_item, TASK_T, item);
		task->f_entry();

		if (task->carry_type == STIMER_TYPE_ONESHOT)
			list_delete_item(cur_item);
		else
			task->arrive = -idx;
		_add_timer(task->period, &(task->item));
	}

	list_for_each_safe (cur_item, next_item, &(m_timer.defer_task_list)) {
		task = container_of(cur_item, TASK_T, item);

		if (++task->arrive >= task->period) {
			task->f_entry();
			list_delete_item(cur_item);
			defer_task_free(task);
		}
	}
}