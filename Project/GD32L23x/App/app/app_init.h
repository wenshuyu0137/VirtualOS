#ifndef _APP_INIT_H
#define _APP_INIT_H

#include <stdint.h>
#include "app_led.h"
#include "app_485.h"
#include "app_at_client.h"
#include "app_at_server.h"
#include "app_ext_flash.h"
#include "stimer.h"

void task_dispatch_looper(void);
void app_task_init(void);

#endif