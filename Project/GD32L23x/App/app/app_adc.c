#include "app_adc.h"

static hal_adc_channel_t all_channel[APP_ADC_CHANNEL_TYPE_MAX] = {
	[APP_ADC_CHANNEL_TYPE_MOS_TEMP] = {
		.adc_io = {
			.HAL_GPIO_PIN = HAL_GPIO_PIN_3,
			.HAL_GPIO_PORT = HAL_GPIOC,
		},
		.channel = HAL_ADC_CHAN_12,
	},
	[APP_ADC_CHANNEL_TYPE_MCU_TEMP] = {
		.adc_io = {
			.HAL_GPIO_PIN = 0, //内部通道不用GPIO
			.HAL_GPIO_PORT = 0,
		},
		.channel = HAL_ADC_CHAN_16,
	},
};

void app_adc_init(void)
{
	//规则组
	hal_adc_channel_param_t m_regular_param = {
		.cb = NULL,
		.channel_info = all_channel,
		.channel_num = APP_ADC_CHANNEL_TYPE_MAX,
	};
	hal_adc_cfg_t adc_cfg = {
		.regular_param = &m_regular_param,
		.insert_param = NULL,
	};
	hal_adc_init(&adc_cfg);
}

void app_adc_task(void)
{
	hal_adc_rdata_get(&all_channel[APP_ADC_CHANNEL_TYPE_MOS_TEMP]);
	hal_adc_idata_get(&all_channel[APP_ADC_CHANNEL_TYPE_MCU_TEMP]);
}