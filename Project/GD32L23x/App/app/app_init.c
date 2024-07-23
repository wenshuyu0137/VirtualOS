#include "app_init.h"

void app_init(void)
{
	hal_platform_clock_init(); //平台时钟
	app_stimer_init(); //调度计时器
	app_gpio_init();
	app_wifi_init();
}

static TASK_T m_led_task = STIMER_TASK_CTOR(app_gpio_task, Period_to_Tick(APP_LED_TASK_PERIOD), STIMER_TYPE_PERIODIC);
static TASK_T m_wifi_task = STIMER_TASK_CTOR(app_wifi_task, Period_to_Tick(APP_WIFI_TASK_PERIOD), STIMER_TYPE_PERIODIC);

void task_dispatch_looper(void)
{
	static uint32_t pre_tick;
	uint32_t cur_tick;
	uint32_t i;
	stimer_task_add(&m_led_task);
	stimer_task_add(&m_wifi_task);
	stimer_start();

	while (1) {
		cur_tick = stimer_get_tick();

		if (cur_tick != pre_tick) {
			++pre_tick;
			stimer_task_dispatch();
		}
	}
}