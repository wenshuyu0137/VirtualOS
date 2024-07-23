#include "hal_timer.h"
#include "board_timer.h"

void hal_timer_base_init(HAL_TIMER_ID_E timerx, HAL_TIMER_BASE_INFO_T *base_info)
{
	board_timer_base_init(timerx, base_info);
}

void hal_timer_oc_init(HAL_TIMER_ID_E timerx, HAL_TIMER_CHANNEL_ID_E channel, HAL_TIMER_OC_INFO_T *oc_info)
{
	board_timer_oc_init(timerx, channel, oc_info);
}

void hal_timer_oc_set_pulse(HAL_TIMER_ID_E timerx, HAL_TIMER_CHANNEL_ID_E channel, uint32_t value)
{
	board_timer_oc_set_pulse(timerx, channel, value);
}

void hal_timer_start(HAL_TIMER_ID_E timerx)
{
	board_timer_start(timerx);
}

void hal_timer_stop(HAL_TIMER_ID_E timerx)
{
	board_timer_stop(timerx);
}

void hal_timer_close(HAL_TIMER_ID_E timerx, HAL_TIMER_IT_MODE_E it_mode)
{
	board_timer_close(timerx, it_mode);
}

void hal_timer_it_config(HAL_TIMER_ID_E timerx, HAL_TIMER_IT_MODE_E it_mode)
{
	board_timer_it_config(timerx, it_mode);
}