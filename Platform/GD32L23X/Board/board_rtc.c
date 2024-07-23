#include "board_rtc.h"
#include <time.h>
#include "board_irqn_cb.h"

#define BKP_VALUE 0x32F0

static uint32_t RTCSRC_FLAG = 0;

static int bcd_to_dec(int bcd)
{
	return (bcd - (bcd >> 4) * 6);
}

static int dec_to_bcd(int dec)
{
	return (dec + (dec / 10) * 6);
}

static void rtc_setup(uint32_t *timestamp)
{
	rtc_parameter_struct rtc_param;
	struct tm *timeval;
	time_t temp_timestamp = (time_t)(*timestamp);
	timeval = localtime(&temp_timestamp);
	rtc_param.am_pm = ((dec_to_bcd(timeval->tm_hour) < 12) ? RTC_AM : RTC_PM);
	rtc_param.display_format = RTC_24HOUR;
	rtc_param.factor_asyn = 0x7F;
	rtc_param.factor_syn = 0xFF;
	rtc_param.day_of_week = (timeval->tm_wday == 0) ? (0x07) : (timeval->tm_wday);
	rtc_param.second = dec_to_bcd(timeval->tm_sec);
	rtc_param.minute = dec_to_bcd(timeval->tm_min);
	rtc_param.hour = dec_to_bcd(timeval->tm_hour);
	rtc_param.date = dec_to_bcd(timeval->tm_mday);
	rtc_param.month = dec_to_bcd(timeval->tm_mon + 1);
	rtc_param.year = dec_to_bcd((timeval->tm_year + 1900) % 100);
	rtc_init(&rtc_param);
	rcu_all_reset_flag_clear();
	RTC_BKP0 = BKP_VALUE;
}

void board_rtc_init(uint32_t timestamp)
{
	rcu_periph_clock_enable(RCU_PMU);
	rcu_periph_clock_enable(RCU_BKP);
	pmu_backup_write_enable();
	rcu_osci_on(RCU_IRC32K);
	rcu_osci_stab_wait(RCU_IRC32K);
	rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
	rcu_periph_clock_enable(RCU_RTC);
	rtc_register_sync_wait();
	//get rcu source
	RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
	uint32_t ts = timestamp + (3600 << 3); //GMT

	if ((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)) {
		rtc_setup(&ts);
	}
}

void board_rtc_set_time(uint32_t timestamp)
{
	rcu_periph_clock_enable(RCU_PMU);
	rcu_periph_clock_enable(RCU_BKP);
	pmu_backup_write_enable();
	rcu_osci_on(RCU_LXTAL);
	rcu_osci_stab_wait(RCU_LXTAL);
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
	rcu_periph_clock_enable(RCU_RTC);
	rtc_register_sync_wait();
	uint32_t ts = timestamp + (3600 << 3); //GMT
	rtc_setup(&ts);
}

uint32_t board_rtc_get_timestamp(void)
{
	return 0; //TODO 
}

void board_rtc_wkup_cfg(uint16_t timeout_ms, rtc_wkuo_cg_f cb)
{
	rtc_wakeup_disable();
	rtc_flag_clear(RTC_FLAG_WT);
	exti_flag_clear(EXTI_20);
	exti_interrupt_flag_clear(EXTI_20);
	nvic_irq_enable(RTC_WKUP_IRQn, 0);
	exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_RISING);
	//rtc_wakeup_clock_set(CTL_WTCS(wkup_cfg->HAL_RTC_WKUP_CLK));
	rtc_wakeup_clock_set(WAKEUP_CKSPRE);
	rtc_wakeup_timer_set(timeout_ms - 1);
	rtc_wakeup_enable();
	rtc_interrupt_enable(RTC_INT_WAKEUP);
	board_irqn_cb_register(cb, HAL_IRQ_TYPE_RTC);
}