#ifndef _APP_MCU_GPIO_H
#define _APP_MCU_GPIO_H

#include "hal_gpio.h"

#define APP_LED_TASK_PERIOD 1000

void app_gpio_init(void);
void app_gpio_task(void);

#endif