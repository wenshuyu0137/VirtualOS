#ifndef _APP_WIFI_H
#define _APP_WIFI_H

#define APP_WIFI_TASK_PERIOD 10

#include <stdint.h>
#include "hal_usart.h"
#include "hal_gpio.h"
#include "stimer.h"
#include "at_client.h"

void app_wifi_init(void);
void app_wifi_task(void);

#endif /*_APP_WIFI_H*/