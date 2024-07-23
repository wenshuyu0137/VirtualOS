#include "hal_irqn_cb.h"
#include "board_irqn_cb.h"

/**
 * @brief 获取中断回调函数指针
 *
 * @param iqr_type 回调类型
 * @return hal_irqn_cb_f
 */
hal_irqn_cb_f hal_irqn_accquire(HAL_IQR_TYPE_E iqr_type)
{
	return board_irqn_accquire(iqr_type);
}