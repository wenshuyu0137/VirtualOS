#include "app_mcu_gpio.h"

static hal_gpio_t led1_gpio = {
	.HAL_GPIO_PIN = HAL_GPIO_PIN_4,
	.HAL_GPIO_PORT = HAL_GPIOB,
};

static hal_gpio_cfg_t led1_gpio_cfg = {
	.HAL_GPIO_AF = HAL_GPIO_AF_NONE,
	.HAL_GPIO_DIR = HAL_GPIO_DIR_OUT,
	.HAL_GPIO_MODE = HAL_GPIO_MODE_OUTPUT,
	.HAL_GPIO_OTYPE = HAL_GPIO_OTYPE_PP,
	.HAL_GPIO_PUPD = HAL_GPIO_PUPD_NONE,
	.HAL_GPIO_SPEED = HAL_GPIO_SPEED_50M,
};

void app_gpio_init(void)
{
	hal_gpio_init(&led1_gpio, &led1_gpio_cfg);
}

void app_gpio_task(void)
{
	hal_gpio_pin_toggle(&led1_gpio);
}