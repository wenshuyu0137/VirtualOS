#ifndef _VIRTUAL_OS_TASK_TIMER_H
#define _VIRTUAL_OS_TASK_TIMER_H

#include "hal_task_timer.h"
#include "hal_irqn_cb.h"
#include <stdint.h>

void hal_task_timer_init(uint32_t ms, hal_irqn_cb_f f_timeout);
void hal_task_timer_start(void);
#endif
