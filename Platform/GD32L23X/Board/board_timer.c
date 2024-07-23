#include "board_timer.h"

uint32_t get_timer_id(HAL_TIMER_ID_E timerx)
{
	uint32_t b_timer;

	switch (timerx) {
	case HAL_TIMER_ID1:
		b_timer = TIMER1;
		break;

	case HAL_TIMER_ID2:
		b_timer = TIMER2;
		break;

	case HAL_TIMER_ID5:
		b_timer = TIMER5;
		break;

	case HAL_TIMER_ID6:
		b_timer = TIMER6;
		break;

	case HAL_TIMER_ID8:
		b_timer = TIMER8;
		break;

	case HAL_TIMER_ID11:
		b_timer = TIMER11;
		break;

	default:
		b_timer = 0xffffffff;
		break;
	}

	return b_timer;
}

uint32_t get_timer_rcu(HAL_TIMER_ID_E timerx)
{
	uint32_t b_rcu;

	switch (timerx) {
	case HAL_TIMER_ID1:
		b_rcu = RCU_TIMER1;
		break;

	case HAL_TIMER_ID2:
		b_rcu = RCU_TIMER2;
		break;

	case HAL_TIMER_ID5:
		b_rcu = RCU_TIMER5;
		break;

	case HAL_TIMER_ID6:
		b_rcu = RCU_TIMER6;
		break;

	case HAL_TIMER_ID8:
		b_rcu = RCU_TIMER8;
		break;

	case HAL_TIMER_ID11:
		b_rcu = RCU_TIMER11;
		break;

	default:
		b_rcu = 0xffffffff;
		break;
	}

	return b_rcu;
}

void board_timer_base_init(HAL_TIMER_ID_E timerx, HAL_TIMER_BASE_INFO_T *base_info)
{
	timer_parameter_struct b_info;
	uint32_t b_timer = get_timer_id(timerx);
	rcu_periph_clock_enable(get_timer_rcu(timerx));
	timer_deinit(b_timer);
	timer_struct_para_init(&b_info);
	b_info.alignedmode = CTL0_CAM(base_info->alignmode);
	b_info.clockdivision = CTL0_CKDIV(base_info->div_mode);
	b_info.counterdirection = (base_info->direction ? ((uint16_t)TIMER_CTL0_DIR) : ((uint16_t)0x0000U));
	b_info.period = base_info->period;
	b_info.prescaler = base_info->prescalar;
	timer_init(b_timer, &b_info);

	if (base_info->use_auto_reload_shadow) {
		timer_auto_reload_shadow_enable(b_timer);
	}

	else {
		timer_auto_reload_shadow_disable(b_timer);
	}
}

void board_timer_oc_init(HAL_TIMER_ID_E timerx, HAL_TIMER_CHANNEL_ID_E channel, HAL_TIMER_OC_INFO_T *oc_info)
{
	timer_oc_parameter_struct oc_param;
	uint32_t m_timer = get_timer_id(timerx);
	timer_channel_output_struct_para_init(&oc_param);
	oc_param.outputstate = (uint16_t)(oc_info->use_channel);
	oc_param.ocpolarity = (oc_info->polarity ? ((uint16_t)0x0000U) : ((uint16_t)0x0002U));
	timer_channel_output_config(m_timer, (uint16_t)(channel), &oc_param);
	timer_channel_output_pulse_value_config(m_timer, (uint16_t)(channel), oc_info->pulse);
	timer_channel_output_mode_config(m_timer, (uint16_t)(channel), ((uint16_t)(oc_info->oc_mode << 4)));
	timer_channel_output_shadow_config(m_timer, (uint16_t)(channel), (oc_info->use_oc_shadow ? ((uint16_t)0x0008U) : 0));
}

void board_timer_oc_set_pulse(HAL_TIMER_ID_E timerx, HAL_TIMER_CHANNEL_ID_E channel, uint32_t value)
{
	timer_channel_output_pulse_value_config(get_timer_id(timerx), (uint16_t)(channel), value);
}

void board_timer_start(HAL_TIMER_ID_E timerx)
{
	rcu_periph_clock_enable(get_timer_rcu(timerx));
	timer_enable(get_timer_id(timerx));
}

void board_timer_close(HAL_TIMER_ID_E timerx, HAL_TIMER_IT_MODE_E it_mode)
{
	timer_deinit(get_timer_id(timerx));
	rcu_periph_clock_disable(get_timer_rcu(timerx));
	timer_interrupt_disable(get_timer_id(timerx), BIT(it_mode));
}

void board_timer_stop(HAL_TIMER_ID_E timerx)
{
	timer_disable(get_timer_id(timerx));
	rcu_periph_clock_disable(get_timer_rcu(timerx));
}

void board_timer_it_config(HAL_TIMER_ID_E timerx, HAL_TIMER_IT_MODE_E it_mode)
{
	timer_flag_clear(get_timer_id(timerx), BIT(it_mode));
	timer_interrupt_enable(get_timer_id(timerx), BIT(it_mode));
}