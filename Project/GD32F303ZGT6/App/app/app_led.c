#include "app_led.h"

void app_led_task(void)
{
	static uint8_t level = 1;

	int fd1 = dal_open("/dev/led_green");
	int fd2 = dal_open("/dev/led_red");

	if (fd1) {
		dal_write(fd1, &level, sizeof(level));
		dal_close(fd1);
	}

	level ^= 1;

	if (fd2) {
		dal_write(fd2, &level, sizeof(level));
		dal_close(fd2);
	}
}