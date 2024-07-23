#include "hal_task_timer.h"
#include "board_task_timer.h"

void hal_task_timer_init(uint32_t ms, hal_irqn_cb_f f_timeout)
{
	board_task_timer_init(ms, f_timeout);
}

void hal_task_timer_start(void)
{
	board_task_timer_start();
}