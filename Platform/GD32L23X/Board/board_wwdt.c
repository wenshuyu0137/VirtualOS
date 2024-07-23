/**
 * @file board_wwdt.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "board_wwdt.h"

void board_wwdt_init(void)
{
	rcu_periph_clock_enable(RCU_WWDGT);
	wwdgt_deinit();
	wwdgt_config(0x7F, 0x7F, WWDGT_CFG_PSC_DIV512);
	wwdgt_enable();
}

void board_wwdt_ctrl(HAL_WWDT_CTRL_E ctrl)
{
	switch (ctrl) {
	case HAL_WWDT_DEINIT:
		wwdgt_deinit();
		break;

	case HAL_WWDT_STOP:
		rcu_periph_clock_disable(RCU_WWDGT);
		break;

	case HAL_WWDT_START:
		wwdgt_enable();
		break;

	case HAL_WWDT_FEED:
		wwdgt_counter_update(0x7F);
		break;

	default:
		break;
	}
}