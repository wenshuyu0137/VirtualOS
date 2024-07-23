#include "hal_exti.h"
#include "board_exti.h"

void hal_exti_deinit(void)
{
	board_exti_deinit();
}

void hal_exti_init(HAL_EXTI_CFG_T *exti_cfg)
{
	board_exti_init(exti_cfg);
}

void hal_nvic_irq_enable(uint8_t irqn, uint8_t priority)
{
	board_nvic_irq_enable(irqn, priority);
}

void hal_nvic_irq_disable(uint8_t irqn)
{
	board_nvic_irq_disable(irqn);
}

void hal_exti_enable(HAL_EXTI_LINE_E exit_line)
{
	return board_exti_enable(exit_line);
}

void hal_exti_disable(HAL_EXTI_LINE_E exit_line)
{
	return board_exti_disable(exit_line);
}
