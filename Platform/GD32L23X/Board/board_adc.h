// /**
//  * @file board_adc.h
//  * @author wenshuyu (wsy2161826815@163.com)
//  * @brief
//  * @version 0.1
//  * @date 2024-07-12
//  *
//  * @copyright Copyright (c) 2024
//  *
//  * Permission is hereby granted, free of charge, to any person obtaining a copy
//  * of this software and associated documentation files (the "Software"), to deal
//  * in the Software without restriction, including without limitation the rights
//  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  * copies of the Software, and to permit persons to whom the Software is
//  * furnished to do so, subject to the following conditions:
//  *
//  * The above copyright notice and this permission notice shall be included in all
//  * copies or substantial usart_instance[id].chip_info.gd_usartions of the Software.
//  *
//  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  * SOFTWARE.
//  *
//  */

// #ifndef _BOARD_ADC_H
// #define _BOARD_ADC_H

// #include <stdio.h>
// #include "hal_adc.h"
// #include "gd32l23x.h"
// #include "gd32l23x_it.h"

// #define ADC_IO_INIT_HELPER(_io)                                                                                                                                \
// 	(rcu_periph_clock_enable(RCU_REGIDX_BIT(AHBEN_REG_OFFSET, 17 + ((_io)->HAL_GPIO_PORT))),                                                               \
// 	 gpio_mode_set(GPIO_BASE + (0x00000400U * ((_io)->HAL_GPIO_PORT)), GPIO_MODE_ANALOG, GPIO_PUPD_NONE, BIT((_io)->HAL_GPIO_PIN)))

// int board_adc_init(hal_adc_cfg_t *adc_cfg);
// void board_adc_start(hal_adc_channel_type_e type);
// uint16_t board_adc_rdata_get(hal_adc_channel_t *regular);
// uint16_t board_adc_idata_get(hal_adc_channel_t *insert);

// #endif