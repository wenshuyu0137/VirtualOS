/**
 * @file hal_gpio.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef _VIRTUAL_OS_HAL_GPIO_H
#define _VIRTUAL_OS_HAL_GPIO_H

#include "hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	HAL_GPIOA,
	HAL_GPIOB,
	HAL_GPIOC,
	HAL_GPIOD,
	HAL_GPIOE,
	HAL_GPIOF,
	HAL_PORT_MAX,
} hal_gpio_port_e;

typedef enum {
	HAL_GPIO_PIN_0,
	HAL_GPIO_PIN_1,
	HAL_GPIO_PIN_2,
	HAL_GPIO_PIN_3,
	HAL_GPIO_PIN_4,
	HAL_GPIO_PIN_5,
	HAL_GPIO_PIN_6,
	HAL_GPIO_PIN_7,
	HAL_GPIO_PIN_8,
	HAL_GPIO_PIN_9,
	HAL_GPIO_PIN_10,
	HAL_GPIO_PIN_11,
	HAL_GPIO_PIN_12,
	HAL_GPIO_PIN_13,
	HAL_GPIO_PIN_14,
	HAL_GPIO_PIN_15,
	HAL_GPIO_PIN_MAX,
} hal_gpio_pin_e;

typedef struct {
	hal_gpio_pin_e HAL_GPIO_PIN;
	hal_gpio_port_e HAL_GPIO_PORT;
} hal_gpio_t;

typedef enum {
	HAL_GPIO_PIN_LOW,
	HAL_GPIO_PIN_HIGH,
} hal_gpio_pin_level_e;

typedef enum {
	HAL_GPIO_MODE_INPUT,
	HAL_GPIO_MODE_OUTPUT,
	HAL_GPIO_MODE_AF,
	HAL_GPIO_MODE_ANALOG,
} hal_gpio_mode_e;

//复用功能阅读数据手册
typedef enum {
	HAL_GPIO_AF_0,
	HAL_GPIO_AF_1,
	HAL_GPIO_AF_2,
	HAL_GPIO_AF_3,
	HAL_GPIO_AF_4,
	HAL_GPIO_AF_5,
	HAL_GPIO_AF_6,
	HAL_GPIO_AF_7,
	HAL_GPIO_AF_8,
	HAL_GPIO_AF_9,
	HAL_GPIO_AF_NONE,
} hal_gpio_af_e;

typedef enum {
	HAL_GPIO_SPEED_2M,
	HAL_GPIO_SPEED_10M,
	HAL_GPIO_SPEED_NONE,
	HAL_GPIO_SPEED_50M,
} hal_gpio_speed_e;

typedef enum {
	HAL_GPIO_PUPD_NONE,
	HAL_GPIO_PUPD_PULLUP,
	HAL_GPIO_PUPD_PULLDOWN,
} hal_gpio_pupd_e;

typedef enum {
	HAL_GPIO_OTYPE_PP,
	HAL_GPIO_OTYPE_OD,
} hal_gpio_otype_e;

typedef enum {
	HAL_GPIO_DIR_OUT,
	HAL_GPIO_DIR_IN,
} hal_gpio_dir_e;

typedef struct {
	hal_gpio_af_e HAL_GPIO_AF; //
	hal_gpio_mode_e HAL_GPIO_MODE;
	hal_gpio_pupd_e HAL_GPIO_PUPD;
	hal_gpio_dir_e HAL_GPIO_DIR;

	//输出模式,输入模式时可以不设置以下参数
	hal_gpio_speed_e HAL_GPIO_SPEED;
	hal_gpio_otype_e HAL_GPIO_OTYPE;
} hal_gpio_cfg_t;

void hal_gpio_deinit(hal_gpio_t *p_io);
bool hal_gpio_init(hal_gpio_t *p_io, hal_gpio_cfg_t *gpio_cfg);
hal_gpio_pin_level_e hal_gpio_pin_read(hal_gpio_t *p_io);
void hal_gpio_pin_write(hal_gpio_t *p_io, hal_gpio_pin_level_e level);
void hal_gpio_pin_toggle(hal_gpio_t *p_io);

#endif