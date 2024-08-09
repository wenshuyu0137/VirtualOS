#include "app_led.h"

static int fd = -1;

void app_led_init(void)
{
	fd = dal_open("/dev/led_green");
}

void app_led_task(void)
{
	static uint8_t level = 1;

	if (fd >= 3)
		dal_write(fd, &level, 1);

	level ^= 1;
}