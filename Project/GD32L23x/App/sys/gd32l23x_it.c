/*!
    \file    gd32l23x_it.c
    \brief   interrupt service routines

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "gd32l23x_it.h"
#include "systick.h"
#include "hal_irqn_cb.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
	/* if Hard Fault exception occurs, go to infinite loop */
	while (1) {
	}
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/

void SysTick_Handler(void)
{
	delay_decrement();
}

void EXTI0_IRQHandler(void)
{
}

void EXTI2_IRQHandler(void)
{
}

void EXTI5_9_IRQHandler(void)
{
}

void EXTI10_15_IRQHandler(void)
{
}

void USART1_IRQHandler(void)
{
	if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE) != RESET) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_USART1)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_USART1)();
		}

		usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
	}
}

void UART3_IRQHandler(void)
{
	if (usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE) != RESET) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_USART3)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_USART3)();
		}

		usart_interrupt_flag_clear(UART3, USART_INT_FLAG_IDLE);
	}
}

void UART4_IRQHandler(void)
{
	if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE) != RESET) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_USART4)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_USART4)();
		}

		usart_interrupt_flag_clear(UART4, USART_INT_FLAG_IDLE);
	}
}

void RTC_WKUP_IRQHandler(void)
{
	if (rtc_flag_get(RTC_FLAG_WT) != RESET) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_RTC)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_RTC)();
		}

		rtc_flag_clear(RTC_FLAG_WT);
	}
}

void DMA_Channel0_IRQHandler(void)
{
	if (RESET != dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_DMA1_Channel0)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_DMA1_Channel0)();
		}

		dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_FTF);
	}
}

void DMA_Channel2_IRQHandler(void)
{
}

void DMA_Channel4_IRQHandler(void)
{
}

void DMA_Channel6_IRQHandler(void)
{
	if (RESET != dma_interrupt_flag_get(DMA_CH6, DMA_INT_FLAG_FTF)) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_DMA1_Channel6)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_DMA1_Channel6)();
		}

		dma_interrupt_flag_clear(DMA_CH6, DMA_INT_FLAG_FTF);
	}
}

void TIMER1_IRQHandler(void)
{
	if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) != RESET) {
		if (hal_irqn_accquire(HAL_IRQ_TYPE_TIM1)) {
			hal_irqn_accquire(HAL_IRQ_TYPE_TIM1)();
		}

		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
	}
}

void TIMER2_IRQHandler(void)
{
}

void TIMER5_IRQHandler(void)
{
}

void CMP0_IRQHandler(void)
{
}
