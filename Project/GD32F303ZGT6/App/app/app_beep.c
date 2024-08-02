#include "app_beep.h"
#include "stdint.h"
#include "dal_char_dev.h"

void app_beep_task(void)
{
	static uint8_t level = 1;

	int fd = dal_open("/dev/beep");

	if (fd) {
		// dal_write(fd, &level, sizeof(level));
		dal_close(fd);
	}

	level ^= 1;
}