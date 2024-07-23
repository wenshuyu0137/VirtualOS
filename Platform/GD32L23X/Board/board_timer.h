#ifndef _BOARD_TIMER_H
#define _BOARD_TIMER_H

#include "hal_timer.h"
#include "gd32l23x.h"

void board_timer_base_init(HAL_TIMER_ID_E timerx, HAL_TIMER_BASE_INFO_T *base_info);
void board_timer_oc_init(HAL_TIMER_ID_E timerx, HAL_TIMER_CHANNEL_ID_E channel, HAL_TIMER_OC_INFO_T *oc_info);
void board_timer_start(HAL_TIMER_ID_E timerx);
void board_timer_stop(HAL_TIMER_ID_E timerx);
void board_timer_close(HAL_TIMER_ID_E timerx, HAL_TIMER_IT_MODE_E it_mode);
void board_timer_it_config(HAL_TIMER_ID_E timerx, HAL_TIMER_IT_MODE_E it_mode);
void board_timer_oc_set_pulse(HAL_TIMER_ID_E timerx, HAL_TIMER_CHANNEL_ID_E channel, uint32_t value);
#endif