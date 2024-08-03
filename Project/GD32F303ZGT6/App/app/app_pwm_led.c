#include "app_pwm_led.h"

#define MAX_DUTY 500
#define CMD_SET_DUTY 1

int fd = -1;

void app_pwdm_led_init()
{
	fd = dal_open("/dev/pwm_blue");
}

void app_pwm_led_task(void)
{
	if (fd < 0)
		return;

	static int duty = 0;
	static int direction = 1;

	dal_ioctrl(fd, CMD_SET_DUTY, &duty);

	if (direction == 1) {
		if (duty < MAX_DUTY) {
			duty += APP_PWM_LED_TASK_PERIOD;
		} else {
			direction = -1;
		}
	} else {
		if (duty > 0) {
			duty -= APP_PWM_LED_TASK_PERIOD;
		} else {
			direction = 1;
		}
	}
}