#ifndef _APP_MCU_GPIO_H
#define _APP_MCU_GPIO_H

#include "dal_device.h"

#define APP_LED_TASK_PERIOD 400

void app_led_init(void);
void app_led_task(void);

#endif
