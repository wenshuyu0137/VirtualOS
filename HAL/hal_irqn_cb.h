#ifndef _VIRTUAL_OS_IRQN_CALLBACK_H
#define _VIRTUAL_OS_IRQN_CALLBACK_H

typedef void (*hal_irqn_cb_f)(void); //中断回调函数指针

typedef enum {
	HAL_IRQ_TYPE_USART0, // 串口0中断
	HAL_IRQ_TYPE_USART1, // 串口1中断
	HAL_IRQ_TYPE_USART2, // 串口2中断
	HAL_IRQ_TYPE_USART3, // 串口3中断
	HAL_IRQ_TYPE_USART4, // 串口4中断
	HAL_IRQ_TYPE_USART5, // 串口5中断
	HAL_IRQ_TYPE_USART6, // 串口6中断
	HAL_IRQ_TYPE_USART7, // 串口7中断
	HAL_IRQ_TYPE_USART8, // 串口8中断
	HAL_IRQ_TYPE_DMA1_Channel0, // DMA1通道0中断
	HAL_IRQ_TYPE_DMA1_Channel1, // DMA1通道1中断
	HAL_IRQ_TYPE_DMA1_Channel2, // DMA1通道2中断
	HAL_IRQ_TYPE_DMA1_Channel3, // DMA1通道3中断
	HAL_IRQ_TYPE_DMA1_Channel4, // DMA1通道4中断
	HAL_IRQ_TYPE_DMA1_Channel5, // DMA1通道5中断
	HAL_IRQ_TYPE_DMA1_Channel6, // DMA1通道6中断
	HAL_IRQ_TYPE_DMA1_Channel7, // DMA1通道7中断
	HAL_IRQ_TYPE_DMA2_Channel0, // DMA2通道0中断
	HAL_IRQ_TYPE_DMA2_Channel1, // DMA2通道1中断
	HAL_IRQ_TYPE_DMA2_Channel2, // DMA2通道2中断
	HAL_IRQ_TYPE_DMA2_Channel3, // DMA2通道3中断
	HAL_IRQ_TYPE_DMA2_Channel4, // DMA2通道4中断
	HAL_IRQ_TYPE_DMA2_Channel5, // DMA2通道5中断
	HAL_IRQ_TYPE_DMA2_Channel6, // DMA2通道6中断
	HAL_IRQ_TYPE_DMA2_Channel7, // DMA2通道7中断
	HAL_IRQ_TYPE_TIM1, // 定时器1中断
	HAL_IRQ_TYPE_TIM2, // 定时器2中断
	HAL_IRQ_TYPE_TIM3, // 定时器3中断
	HAL_IRQ_TYPE_TIM4, // 定时器4中断
	HAL_IRQ_TYPE_TIM5, // 定时器5中断
	HAL_IRQ_TYPE_TIM6, // 定时器6中断
	HAL_IRQ_TYPE_TIM7, // 定时器7中断
	HAL_IRQ_TYPE_TIM8, // 定时器8中断
	HAL_IRQ_TYPE_TIM9, // 定时器9中断
	HAL_IRQ_TYPE_TIM10, // 定时器10中断
	HAL_IRQ_TYPE_TIM11, // 定时器11中断
	HAL_IRQ_TYPE_TIM12, // 定时器12中断
	HAL_IRQ_TYPE_TIM13, // 定时器13中断
	HAL_IRQ_TYPE_TIM14, // 定时器14中断
	HAL_IRQ_TYPE_TIM15, // 定时器15中断
	HAL_IRQ_TYPE_EXTI0, // 外部中断0
	HAL_IRQ_TYPE_EXTI1, // 外部中断1
	HAL_IRQ_TYPE_EXTI2, // 外部中断2
	HAL_IRQ_TYPE_EXTI3, // 外部中断3
	HAL_IRQ_TYPE_EXTI4, // 外部中断4
	HAL_IRQ_TYPE_EXTI5, // 外部中断5
	HAL_IRQ_TYPE_EXTI6, // 外部中断6
	HAL_IRQ_TYPE_EXTI7, // 外部中断7
	HAL_IRQ_TYPE_EXTI8, // 外部中断8
	HAL_IRQ_TYPE_EXTI9, // 外部中断9
	HAL_IRQ_TYPE_EXTI10, // 外部中断10
	HAL_IRQ_TYPE_EXTI11, // 外部中断11
	HAL_IRQ_TYPE_EXTI12, // 外部中断12
	HAL_IRQ_TYPE_EXTI13, // 外部中断13
	HAL_IRQ_TYPE_EXTI14, // 外部中断14
	HAL_IRQ_TYPE_EXTI15, // 外部中断15
	HAL_IRQ_TYPE_ADC1, // ADC1中断
	HAL_IRQ_TYPE_ADC2, // ADC2中断
	HAL_IRQ_TYPE_ADC3, // ADC3中断
	HAL_IRQ_TYPE_RTC, // RTC中断
	HAL_IRQ_TYPE_WWDG, // 看门狗中断
	HAL_IRQ_TYPE_SYSTICK, // 系统滴答定时器中断

	HAL_IRQ_TYPE_MAX,
} HAL_IQR_TYPE_E;

/**
 * @brief 获取中断回调函数指针
 *
 * @param irq_type 回调类型
 * @return hal_irqn_cb_f
 */
hal_irqn_cb_f hal_irqn_accquire(HAL_IQR_TYPE_E irq_type);

#endif // _VIRTUAL_OS_IRQN_CALLBACK_H
