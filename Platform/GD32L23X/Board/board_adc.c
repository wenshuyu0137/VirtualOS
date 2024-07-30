// /**
//  * @file board_adc.c
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

// #include "board_adc.h"
// #include "board_irqn_cb.h"

// static uint16_t adc_r_buffer[HAL_ADC_CHAN_MAX];
// static uint16_t adc_i_buffer[HAL_ADC_CHAN_MAX];

// static uint8_t rank = 0;

// //rank与通道映射表
// static uint8_t rank_channel_map[HAL_ADC_CHAN_MAX];

// void _adc_dma_config(uint32_t memory, uint32_t peripheral, uint16_t dma_size, hal_irqn_cb_f dma_cb)
// {
// 	dma_parameter_struct adc_dma_param;
// 	rcu_periph_clock_enable(RCU_DMA);
// 	dma_deinit(DMA_CH6);
// 	adc_dma_param.direction = DMA_PERIPHERAL_TO_MEMORY;
// 	adc_dma_param.memory_addr = memory;
// 	adc_dma_param.memory_inc = ENABLE;
// 	adc_dma_param.memory_width = DMA_MEMORY_WIDTH_16BIT;
// 	adc_dma_param.number = dma_size;
// 	adc_dma_param.periph_addr = peripheral;
// 	adc_dma_param.periph_inc = DISABLE;
// 	adc_dma_param.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
// 	adc_dma_param.priority = DMA_PRIORITY_HIGH;
// 	adc_dma_param.request = DMA_REQUEST_ADC;
// 	dma_circulation_enable(DMA_CH6);
// 	dma_init(DMA_CH6, &adc_dma_param);
// 	adc_dma_mode_enable();
// 	nvic_irq_disable(DMA_Channel6_IRQn);
// 	dma_interrupt_disable(DMA_CH6, DMA_INT_FTF);
// 	board_irqn_cb_register(dma_cb, HAL_IRQ_TYPE_DMA1_Channel6); //注册中断
// }

// int _insert_config(hal_adc_channel_param_t *insert_cfg)
// {
// 	if (insert_cfg->channel_num > 4) {
// 		return -1;
// 	}

// 	adc_channel_length_config(ADC_INSERTED_CHANNEL, insert_cfg->channel_num);

// 	if (insert_cfg->channel_num > 3) {
// 		return -1; //注入组只有4个槽
// 	}

// 	for (uint8_t i = 0; i < insert_cfg->channel_num; i++) {
// 		adc_inserted_channel_config(rank, insert_cfg->channel_info[i].channel, ADC_SAMPLETIME_239POINT5);
// 		rank_channel_map[insert_cfg->channel_info[i].channel] = rank;
// 		rank++;
// 		adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_NONE);

// 		if (insert_cfg->channel_info[i].channel >= 16) {
// 			adc_channel_16_to_19(BIT(insert_cfg->channel_info[i].channel + 7),
// 					     ENABLE); //内部通道
// 		}

// 		else {
// 			ADC_IO_INIT_HELPER(&(insert_cfg->channel_info[i].adc_io));
// 		}
// 	}

// 	adc_external_trigger_config(ADC_INSERTED_CHANNEL, ENABLE); //外部触发
// 	return 0;
// }

// int _regular_config(hal_adc_channel_param_t *regular_cfg)
// {
// 	if (regular_cfg->channel_num > 15) {
// 		return -1;
// 	}

// 	adc_channel_length_config(ADC_REGULAR_CHANNEL, regular_cfg->channel_num);

// 	for (uint8_t i = 0; i < regular_cfg->channel_num; i++) {
// 		adc_regular_channel_config(rank, regular_cfg->channel_info[i].channel, ADC_SAMPLETIME_239POINT5);
// 		rank_channel_map[regular_cfg->channel_info[i].channel] = rank;
// 		rank++;
// 		adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);

// 		if (regular_cfg->channel_info[i].channel >= 16) {
// 			adc_channel_16_to_19(BIT(regular_cfg->channel_info[i].channel + 7), ENABLE);
// 		}

// 		else {
// 			ADC_IO_INIT_HELPER(&(regular_cfg->channel_info[i].adc_io));
// 		}
// 	}

// 	adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
// 	_adc_dma_config((uint32_t)adc_r_buffer, (uint32_t)(&ADC_RDATA), regular_cfg->channel_num, regular_cfg->cb);
// 	return 0;
// }

// int board_adc_init(hal_adc_cfg_t *adc_cfg)
// {
// 	rcu_periph_clock_enable(RCU_ADC);
// 	rcu_adc_clock_config(RCU_ADCCK_APB2_DIV16);
// 	adc_deinit();
// 	adc_special_function_config(ADC_SCAN_MODE, ENABLE);
// 	adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);

// 	//先初始化注入组,rank必须为0-3
// 	if (adc_cfg->insert_param) {
// 		if (_insert_config(adc_cfg->insert_param) == -1) {
// 			return -1;
// 		}
// 	}

// 	if (adc_cfg->regular_param) {
// 		if (_regular_config(adc_cfg->regular_param) == -1) {
// 			return -1;
// 		}
// 	}

// 	adc_resolution_config(ADC_RESOLUTION_12B);
// 	adc_oversample_mode_config(ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_8B, ADC_OVERSAMPLING_RATIO_MUL256);
// 	adc_oversample_mode_enable();
// 	adc_channel_16_to_19(ADC_INTERNAL_CHANNEL_SWITCH, ENABLE);
// 	adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
// 	adc_enable();
// 	adc_calibration_enable();
// 	return 0;
// }

// void board_adc_start(hal_adc_channel_type_e type)
// {
// 	dma_channel_enable(DMA_CH6);
// 	adc_software_trigger_enable(type + 1);
// }

// uint16_t board_adc_rdata_get(hal_adc_channel_t *regular)
// {
// 	return adc_r_buffer[rank_channel_map[regular->channel]];
// }

// uint16_t board_adc_idata_get(hal_adc_channel_t *insert)
// {
// 	return adc_inserted_data_read(rank_channel_map[insert->channel]);
// }
