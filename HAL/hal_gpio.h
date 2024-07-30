/**
 * @file hal_gpio.h
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

#ifndef _VIRTUAL_OS_HAL_GPIO_H
#define _VIRTUAL_OS_HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	HAL_GPIO_ERROR_NONE, //无错误
	HAL_GPIO_DEV_NOT_EXSIT, //设备不存在
	HAL_GPIO_DEV_OPT_EXCEPTION, //异常操作,；例如对输出的引脚读取操作
} hal_gpio_error_e;

typedef enum {
	HAL_GPIO_CONFIG_DEINIT, //复位
	HAL_GPIO_CONFIG_LP_MODE, //低功耗
	HAL_GPIO_CONFIG_CALLBACK_SET, //设置中断回调

} hal_gpio_config_e;

/**********************************************************USER API**********************************************************/
hal_gpio_error_e hal_gpio_pin_read(const char *const gpio_dev, uint8_t *level);
hal_gpio_error_e hal_gpio_pin_write(const char *const gpio_dev, uint8_t level);
hal_gpio_error_e hal_gpio_pin_toggle(const char *const gpio_dev);
hal_gpio_error_e hal_gpio_config(const char *const gpio_dev, hal_gpio_config_e type, void (*cb)(void));

#endif