#ifndef _VIRTUAL_OS_APP_ADC_H
#define _VIRTUAL_OS_APP_ADC_H

#include "hal_adc.h"

typedef enum {
	APP_ADC_CHANNEL_TYPE_MOS_TEMP,
	APP_ADC_CHANNEL_TYPE_MCU_TEMP,

	APP_ADC_CHANNEL_TYPE_MAX,
} app_adc_channel_type_e;

#endif /*_VIRTUAL_OS_APP_ADC_H*/