#ifndef _VIRTUAL_OS_BOARD_RTC_H
#define _VIRTUAL_OS_BOARD_RTC_H

#include "hal.h"
#include "hal_rtc.h"
#include "gd32l23x.h"

void board_rtc_init(uint32_t timestamp);
void board_rtc_set_time(uint32_t timestamp);
uint32_t board_rtc_get_timestamp(void);
void board_rtc_wkup_cfg(uint16_t timeout_ms, rtc_wkuo_cg_f cb);
#endif /*_VIRTUAL_OS_BOARD_RTC_H*/