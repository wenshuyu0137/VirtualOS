#include "hal_rtc.h"
#include "board_rtc.h"

/**
 * @brief 初始化RTC时钟
 *
 * @param timestamp 时间戳
 */
void hal_rtc_init(uint32_t timestamp)
{
	board_rtc_init(timestamp);
}

/**
 * @brief 设置当前时间
 *
 * @param timestamp 时间戳
 */
void hal_rtc_set_time(uint32_t timestamp)
{
	board_rtc_set_time(timestamp);
}

/**
 * @brief 获取当前时间戳
 *
 * @return uint32_t
 */
uint32_t hal_rtc_get_timestamp(void)
{
	return board_rtc_get_timestamp();
}

/**
 * @brief 设置RTC唤醒时间
 *
 * @param timeout 毫秒
 * @param cb 中断回调
 */
void hal_rtc_wkup_cfg(uint16_t timeout_ms, rtc_wkuo_cg_f cb)
{
	board_rtc_wkup_cfg(timeout_ms, cb);
}
