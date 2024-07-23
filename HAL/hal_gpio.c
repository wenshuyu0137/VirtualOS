/**
 * @file hal_gpio.c
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

#include "hal_gpio.h"
#include "board_gpio.h"

static bool is_hal_gpio_valid(hal_gpio_t *p_io)
{
	return (p_io != 0) ? true : false;
}

void hal_gpio_deinit(hal_gpio_t *p_io)
{
	board_gpio_deinit(p_io);
}

bool hal_gpio_init(hal_gpio_t *p_io, hal_gpio_cfg_t *gpio_cfg)
{
	return (is_hal_gpio_valid(p_io) && gpio_cfg) ? (board_gpio_init(p_io, gpio_cfg), true) : false;
}

hal_gpio_pin_level_e hal_gpio_pin_read(hal_gpio_t *p_io)
{
	return board_gpio_pin_read(p_io);
}

void hal_gpio_pin_write(hal_gpio_t *p_io, hal_gpio_pin_level_e level)
{
	board_gpio_pin_write(p_io, level);
}

void hal_gpio_pin_toggle(hal_gpio_t *p_io)
{
	board_gpio_pin_toggle(p_io);
}