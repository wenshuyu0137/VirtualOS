#ifndef _APP_INIT_H
#define _APP_INIT_H

#include <stdint.h>
#include "stimer.h"
#include "app_led.h"
#include "app_beep.h"
#include "app_pwm_led.h"

void task_dispatch_looper(void);
void app_task_init(void);

#endif