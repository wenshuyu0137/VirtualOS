#include "app_led.h"

void app_led_task(void)
{
	static uint8_t level = 1;
	dal_write("LED_RED", &level, 1);
	level ^= 1;
}