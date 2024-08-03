#ifndef _APP_PWM_LED_H
#define _APP_PWM_LED_H

#include "dal_device.h"

#define APP_PWM_LED_TASK_PERIOD 10

void app_pwdm_led_init();
void app_pwm_led_task(void);

#endif /*_APP_PWM_LED_H*/
