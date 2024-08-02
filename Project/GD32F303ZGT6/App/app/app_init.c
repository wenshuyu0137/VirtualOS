#include "app_init.h"

static TASK_T m_led_task = STIMER_TASK_CTOR(app_led_task, Period_to_Tick(APP_LED_TASK_PERIOD), STIMER_TYPE_PERIODIC);
static TASK_T m_beep_task = STIMER_TASK_CTOR(app_beep_task, Period_to_Tick(APP_BEEP_TASK_PERIOD), STIMER_TYPE_PERIODIC);


void app_task_init(void)
{
	
}

void task_dispatch_looper(void)
{
	static uint32_t pre_tick;
	uint32_t cur_tick;
	uint32_t i;

	stimer_task_add(&m_led_task);
	stimer_task_add(&m_beep_task);

	stimer_start();

	while (1) {
		cur_tick = stimer_get_tick();

		if (cur_tick != pre_tick) {
			++pre_tick;
			stimer_task_dispatch();
		}
	}
}