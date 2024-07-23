/**
 * @file hal_adc.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-12
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

#ifndef _VIRTUAL_OS_HAL_ADC_H
#define _VIRTUAL_OS_HAL_ADC_H

#include "hal.h"
#include <stdint.h>
#include "queue.h"
#include "hal_gpio.h"

typedef void (*adc_cb_f)(void); //ACC中断回调

//通道类型
typedef enum {
	HAL_DMA_CHANNEL_TYPE_REGULAR, //规则组
	HAL_DMA_CHANNEL_TYPE_INSERT, //注入组
} hal_adc_channel_type_e;

//ADC通道
typedef enum {
	HAL_ADC_CHAN_0,
	HAL_ADC_CHAN_1,
	HAL_ADC_CHAN_2,
	HAL_ADC_CHAN_3,
	HAL_ADC_CHAN_4,
	HAL_ADC_CHAN_5,
	HAL_ADC_CHAN_6,
	HAL_ADC_CHAN_7,
	HAL_ADC_CHAN_8,
	HAL_ADC_CHAN_9,
	HAL_ADC_CHAN_10,
	HAL_ADC_CHAN_11,
	HAL_ADC_CHAN_12,
	HAL_ADC_CHAN_13,
	HAL_ADC_CHAN_14,
	HAL_ADC_CHAN_15,
	HAL_ADC_CHAN_16,
	HAL_ADC_CHAN_17,
	HAL_ADC_CHAN_18,
	HAL_ADC_CHAN_19,

	HAL_ADC_CHAN_MAX,
} hal_adc_channel_e;

typedef struct {
	hal_adc_channel_e channel; //通道
	hal_gpio_t adc_io; //IO口
} hal_adc_channel_t;

typedef struct {
	uint8_t channel_num; //通道数量
	hal_adc_channel_t *channel_info; //通道配置
	adc_cb_f cb; //中断回调
} hal_adc_channel_param_t;

typedef struct {
	hal_adc_channel_param_t *regular_param; //规则组
	hal_adc_channel_param_t *insert_param; //注入组
} hal_adc_cfg_t;

//实例化宏
#define HAL_ADC_CHANNEL_CTOR(_channel, _port, _pin)                                                                                                            \
	((hal_adc_channel_t){                                                                                                                                  \
		.channel = (_channel),                                                                                                                         \
		.adc_io.HAL_GPIO_PORT = (_port),                                                                                                               \
		.adc_io.HAL_GPIO_PIN = (_pin),                                                                                                                 \
	})

/**********************************************************API**********************************************************/
int hal_adc_init(hal_adc_cfg_t *adc_cfg);
void hal_adc_start(hal_adc_channel_type_e type);
uint16_t hal_adc_rdata_get(hal_adc_channel_t *regular);
uint16_t hal_adc_idata_get(hal_adc_channel_t *insert);
#endif