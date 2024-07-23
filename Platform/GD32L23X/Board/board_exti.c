#include "board_exti.h"

void board_exti_deinit(void)
{
	exti_deinit();
}

void board_exti_init(HAL_EXTI_CFG_T *exti_cfg)
{
	rcu_periph_clock_enable(RCU_SYSCFG);
	syscfg_exti_line_config(exti_cfg->HAL_EXTI_PORT_SOURCE, exti_cfg->HAL_EXTI_PIN_SOURCE);
	exti_init(BIT(exti_cfg->HAL_EXTI_LINE), (exti_mode_enum)exti_cfg->HAL_EXTI_MODE, (exti_trig_type_enum)exti_cfg->HAL_EXTI_TRIG_TYPE);
	exti_interrupt_flag_clear(BIT(exti_cfg->HAL_EXTI_LINE));
}

void board_nvic_irq_enable(uint8_t irqn, uint8_t priority)
{
	nvic_irq_enable(irqn, priority);
}

void board_nvic_irq_disable(uint8_t irqn)
{
	nvic_irq_disable(irqn);
}

void board_exti_enable(HAL_EXTI_LINE_E exit_line)
{
	exti_interrupt_enable(BIT(exit_line));
}

void board_exti_disable(HAL_EXTI_LINE_E exit_line)
{
	exti_interrupt_disable(BIT(exit_line));
}
