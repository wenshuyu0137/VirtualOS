/**
 * @file stimer.h
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

#ifndef _VIRTUAL_OS_STIMER_H
#define _VIRTUAL_OS_STIMER_H

#include <stdint.h>
#include "list.h"

#define STIMER_PERIOD_PER_TICK_MS (2)
#define STIMER_TASK_HIT_LIST_MAX (32)
#define MAX_DEFER_TASK (16)

#define Period_to_Tick(p) ((((uint32_t)(p) >= STIMER_PERIOD_PER_TICK_MS)) ? ((uint32_t)(p) / STIMER_PERIOD_PER_TICK_MS) : 1U)

typedef void (*stimer_timeout_process)(void);
typedef void (*stimer_base_init)(uint32_t period, stimer_timeout_process f_timeout);
typedef void (*stimer_base_start)(void);

typedef void (*stimer_entry)(void);

struct timer_port {
	volatile stimer_base_init f_init;
	volatile stimer_base_start f_start;
};

typedef enum {
	STIMER_TYPE_ONESHOT,
	STIMER_TYPE_PERIODIC,
} stimer_type_e;

typedef struct stimer_task {
	stimer_entry f_entry;
	uint32_t period;
	uint32_t arrive;
	list_item item;
	stimer_type_e carry_type;
	uint8_t reservere;
	uint8_t id;
} TASK_T;

#define STIMER_TASK_CTOR(_entry, _PERIOD, _type)                                                                                                               \
	(TASK_T)                                                                                                                                               \
	{                                                                                                                                                      \
		.f_entry = (_entry), .period = (_PERIOD), .arrive = 0, .item.next = LIST_NULL, .item.pre = LIST_NULL, .reservere = 1, .carry_type = (_type),   \
		.id = 0,                                                                                                                                       \
	}

int stimer_init(struct timer_port *port);

void stimer_start(void);

uint32_t stimer_get_tick(void);

int stimer_task_add(TASK_T *p_task);

int stimer_task_del(TASK_T *p_task);

void stimer_task_dispatch(void);

int defer_task_add(stimer_entry f_entry, uint32_t ms);

#endif /*_VIRTUAL_OS_STIMER_H*/