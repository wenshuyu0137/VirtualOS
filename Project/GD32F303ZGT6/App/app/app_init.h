#ifndef _APP_INIT_H
#define _APP_INIT_H

#include <stdint.h>
#include "app_led.h"
#include "app_beep.h"
#include "stimer.h"

void task_dispatch_looper(void);
void app_task_init(void);

#endif