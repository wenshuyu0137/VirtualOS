/**
 * @file dml_init.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 设备管理层的初始化,在main函数启动之前进行调用
 * @version 0.1
 * @date 2024-07-31
 * 
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#include "dml_init.h"
#include "board_led_1.h"
#include "SEGGER_RTT.h"
#include "stimer.h"
#include "systick.h"
/**
 * @brief 设备管理层初始化函数
 * 
 */
void dml_init(void) __attribute__((constructor)); //main函数启动之前调用

void dml_init(void)
{
    dev_table_init(); //初始化设备表
    systick_config(); //滴答定时器 1ms
    platform_stimer_init(); //框架调度定时器
    
    led_red_init(); //LED设备注册
    RTT_PRINT("hello world\n");
}