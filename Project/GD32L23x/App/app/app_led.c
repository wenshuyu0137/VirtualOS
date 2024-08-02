#include "app_led.h"

void app_led_task(void)
{
	static uint8_t level = 1;
	int fd = dal_open("/dev/led_green");

	if (fd) {
		dal_write(fd, &level, 1);
		dal_close(fd);
	}

	level ^= 1;
}