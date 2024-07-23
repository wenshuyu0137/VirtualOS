#ifndef _BOARD_EXTI_H
#define _BOARD_EXTI_H

#include "gd32l23x.h"
#include "hal_exti.h"

#include "gd32l23x_misc.h"
#include "gd32l23x_it.h"

void board_exti_deinit(void);
void board_exti_init(HAL_EXTI_CFG_T *exti_cfg);

void board_nvic_irq_enable(uint8_t irqn, uint8_t priority);
void board_nvic_irq_disable(uint8_t irqn);

void board_exti_enable(HAL_EXTI_LINE_E exit_line);
void board_exti_disable(HAL_EXTI_LINE_E exit_line);

#endif