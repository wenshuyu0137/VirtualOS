#include "app_mcu_gpio.h"

void app_gpio_task(void)
{
	hal_gpio_pin_toggle("LED1");
}