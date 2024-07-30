/**
 * @file board_gpio_v1.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-30
 * 
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#include "gd32l23x.h"
#include "board_gpio.h"
#include "string_hash.h"
#include <string.h>

static uint32_t init_flag = 0x641ac; //初始化标志

static hash_table_t gpio_hash_table; //GPIO哈希表

typedef enum {
	BOARD_GPIO_DIR_IN,
	BOARD_GPIO_DIR_OUT,
} board_gpio_dir;

typedef struct {
	char *dev_name; //设备名
	void (*irq_cb)(void); //GPIO中断
	uint32_t gpio_rcu;
	uint32_t gpio_port; //端口号
	uint32_t gpio_pin; //引脚号
	uint32_t gpio_af; //IO复用
	uint32_t mode; //模式
	uint32_t out_mode; //模式
	uint32_t pull_mode;
	board_gpio_dir dir; //方向
} board_gpio_t;

board_gpio_t gpios[] = {
	{ .dev_name = "LED1",
	  .dir = BOARD_GPIO_DIR_OUT,
	  .gpio_af = GPIO_AF_0,
	  .gpio_pin = GPIO_PIN_4,
	  .gpio_port = GPIOB,
	  .irq_cb = NULL,
	  .mode = GPIO_MODE_OUTPUT,
	  .gpio_rcu = RCU_GPIOB,
	  .pull_mode = GPIO_PUPD_NONE,
	  .out_mode = GPIO_OTYPE_PP },
};

/**
 * @brief 初始化GPIO设备Hash表
 * 
 */
static void board_table_init(void)
{
	if (!init_flag)
		return;

	hash_error_t ret = init_hash_table(&gpio_hash_table, 64, NULL, NULL);

	init_flag = 0;
}

/**
 * @brief 判断设备是否存在
 * 
 * @param gpio_name 设备名
 * @return true 
 * @return false 
 */
static bool is_dev_exsit(const char *const gpio_name)
{
	hash_error_t ret;
	void *private = hash_find(&gpio_hash_table, gpio_name, &ret);
	return ret == HASH_SUCCESS;
}

void board_gpio_v1_init(void)
{
	board_table_init();

	for (uint16_t i = 0; i < (sizeof(gpios) / sizeof(gpios[0])); i++) {
		gpio_deinit(gpios[i].gpio_port);
		rcu_periph_clock_enable(gpios[i].gpio_rcu);
		gpio_mode_set(gpios[i].gpio_port, gpios[i].mode, gpios[i].pull_mode, gpios[i].gpio_pin);
		gpio_output_options_set(gpios[i].gpio_port, gpios[i].out_mode, GPIO_OSPEED_50MHZ, gpios[i].gpio_pin);

		hash_insert(&gpio_hash_table, gpios[i].dev_name, (void *)&gpios[i]);
	}
}

hal_gpio_error_e board_gpio_pin_read(const char *const gpio_dev, uint8_t *level)
{
	hash_error_t err;
	board_gpio_t *dev = (board_gpio_t *)hash_find(&gpio_hash_table, gpio_dev, &err);
	if (err)
		return HAL_GPIO_DEV_NOT_EXSIT; //设备不存在

	if (dev->dir = BOARD_GPIO_DIR_OUT)
		return HAL_GPIO_ERROR_NONE; //输出的引脚，不可读

	*level = gpio_input_bit_get(dev->gpio_port, dev->gpio_pin);

	return HAL_GPIO_ERROR_NONE;
}

hal_gpio_error_e board_gpio_pin_write(const char *const gpio_dev, uint8_t level)
{
	hash_error_t err;
	board_gpio_t *dev = (board_gpio_t *)hash_find(&gpio_hash_table, gpio_dev, &err);
	if (err)
		return HAL_GPIO_DEV_NOT_EXSIT; //设备不存在

	if (dev->dir = BOARD_GPIO_DIR_IN)
		return HAL_GPIO_ERROR_NONE; //输入的引脚，不可写

	gpio_bit_write(dev->gpio_port, dev->gpio_pin, level ? SET : RESET);

	return HAL_GPIO_ERROR_NONE;
}

hal_gpio_error_e board_gpio_pin_toggle(const char *const gpio_dev)
{
	hash_error_t err;
	board_gpio_t *dev = (board_gpio_t *)hash_find(&gpio_hash_table, gpio_dev, &err);
	if (err)
		return HAL_GPIO_DEV_NOT_EXSIT; //设备不存在

	if (dev->dir = BOARD_GPIO_DIR_IN)
		return HAL_GPIO_ERROR_NONE; //输入的引脚，不可写

	gpio_bit_toggle(dev->gpio_port, dev->gpio_pin);

	return HAL_GPIO_ERROR_NONE;
}

hal_gpio_error_e board_gpio_config(const char *const gpio_dev, hal_gpio_config_e type, void (*cb)(void))
{
	hash_error_t err;
	board_gpio_t *dev = (board_gpio_t *)hash_find(&gpio_hash_table, gpio_dev, &err);
	if (err)
		return HAL_GPIO_DEV_NOT_EXSIT; //设备不存在

	switch (type) {
	case HAL_GPIO_CONFIG_DEINIT:
        gpio_deinit(dev->gpio_port);
		break;
    case HAL_GPIO_CONFIG_LP_MODE:
        //浮空输入 关始终
		break;
    case HAL_GPIO_CONFIG_CALLBACK_SET:
        dev->irq_cb = cb;
		break;
	default:
		break;
	}
}