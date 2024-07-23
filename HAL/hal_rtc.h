#ifndef _VIRTUAL_OS_HAL_RTC_H
#define _VIRTUAL_OS_HAL_RTC_H

#include "hal.h"
#include <stdbool.h>
#include <stdint.h>

#define TIME_STAMP_MIN (1672502400) //2023-01-01 00:00:00

typedef void (*rtc_wkuo_cg_f)(void); //RTC唤醒中断回调

/**
 * @brief 初始化RTC时钟
 *
 * @param timestamp 时间戳
 */
void hal_rtc_init(uint32_t timestamp);

/**
 * @brief 设置当前时间
 *
 * @param timestamp 时间戳
 */
void hal_rtc_set_time(uint32_t timestamp);

/**
 * @brief 获取当前时间戳
 *
 * @return uint32_t
 */
uint32_t hal_rtc_get_timestamp(void);

/**
 * @brief 设置RTC唤醒时间
 *
 * @param timeout 毫秒
 * @param cb 中断回调
 */
void hal_rtc_wkup_cfg(uint16_t timeout_ms, rtc_wkuo_cg_f cb);

#endif