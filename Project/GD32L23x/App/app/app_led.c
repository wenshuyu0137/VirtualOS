#include "app_led.h"

#define LED_GREEN_NAME "/dev/led_green"

void app_led_task(void)
{
	static uint8_t level = 1;
	dal_open(LED_GREEN_NAME);
	dal_write(LED_GREEN_NAME, &level, 1);
	// dal_close(LED_GREEN_NAME);
	level ^= 1;
}