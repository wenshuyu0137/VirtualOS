/**
 * @file app_485.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-01
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

#include "app_485.h"

#define APP_485_DEVICE_NAME "/dev/serial_485"

static uint16_t recv_size = 0; //接受的数据长度在这

void app_485_init(void)
{
	dal_open(APP_485_DEVICE_NAME); //打开设备
}

void app_485_task(void)
{
	uint8_t recv_buf[64] = { 0 };
	int ret = dal_read(APP_485_DEVICE_NAME, recv_buf, 64);
	if (ret)
		dal_write(APP_485_DEVICE_NAME, recv_buf, ret);
	// dal_write(APP_485_DEVICE_NAME,APP_485_DEVICE_NAME,sizeof(APP_485_DEVICE_NAME));
}