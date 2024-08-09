#include "app_init.h"

static TASK_T m_led_task = STIMER_TASK_CTOR(app_led_task, Period_to_Tick(APP_LED_TASK_PERIOD), STIMER_TYPE_PERIODIC);
// static TASK_T m_485_task = STIMER_TASK_CTOR(app_485_task, Period_to_Tick(APP_485_TASK_PERIOD), STIMER_TYPE_PERIODIC);
// static TASK_T m_at_client_task = STIMER_TASK_CTOR(app_at_client_task, Period_to_Tick(APP_AT_CLIENT_TASK_PERIOD), STIMER_TYPE_PERIODIC);
// static TASK_T m_at_server_task = STIMER_TASK_CTOR(app_at_server_task, Period_to_Tick(APP_AT_SERVER_TASK_PERIOD), STIMER_TYPE_PERIODIC);
static TASK_T m_ext_flash_task = STIMER_TASK_CTOR(app_ext_flash_task, Period_to_Tick(APP_EXT_FLASH_TASK_PERIOD), STIMER_TYPE_PERIODIC);

void app_task_init(void)
{
	app_led_init();
	// app_485_init();
	// app_at_client_init();
	// app_at_server_init();
	app_ext_flash_init();
}

void task_dispatch_looper(void)
{
	static uint32_t pre_tick;
	uint32_t cur_tick;
	uint32_t i;

	stimer_task_add(&m_led_task);
	// stimer_task_add(&m_485_task);
	// stimer_task_add(&m_at_client_task);
	// stimer_task_add(&m_at_server_task);
	stimer_task_add(&m_ext_flash_task);

	stimer_start();

	while (1) {
		cur_tick = stimer_get_tick();

		if (cur_tick != pre_tick) {
			++pre_tick;
			stimer_task_dispatch();
		}
	}
}