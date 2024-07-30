#ifndef _APP_INIT_H
#define _APP_INIT_H

#include <stdint.h>
#include "hal.h"
#include "app_stimer.h"
#include "app_mcu_gpio.h"

void app_init(void);
void task_dispatch_looper(void);

#endif