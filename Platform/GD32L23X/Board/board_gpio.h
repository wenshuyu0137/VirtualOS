#ifndef _GPIO_H
#define _GPIO_H

#include "gd32l23x.h"
#include "hal_gpio.h"

bool board_gpio_deinit(hal_gpio_t *p_io);
bool board_gpio_init(hal_gpio_t *p_io, hal_gpio_cfg_t *gpio_cfg);

hal_gpio_pin_level_e board_gpio_pin_read(hal_gpio_t *p_io);
void board_gpio_pin_toggle(hal_gpio_t *p_io);
void board_gpio_pin_write(hal_gpio_t *p_io, hal_gpio_pin_level_e level);

#endif
