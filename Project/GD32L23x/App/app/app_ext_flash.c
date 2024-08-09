/**
 * @file app_ext_flash.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-09
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

#include "app_ext_flash.h"

static int fd = -1;
int ret;
static uint8_t flash_read_buf[256] = { 0 };
static uint8_t write_buf[] = "Hello World!";

void app_ext_flash_init(void)
{
	fd = dal_open("/dev/ext_flash");
	// ret = dal_lseek(fd, 256, DAL_LSEEK_WHENCE_HEAD); //在起始位置偏移20字节
	dal_write(fd, write_buf, sizeof(write_buf));
}

void app_ext_flash_task(void)
{
	if (fd >= RESERVED_FD_NUM) {
		// ret = dal_write(fd, write_buf, sizeof(write_buf));
		ret = dal_read(fd, flash_read_buf, 256);
		ret = dal_lseek(fd, 0, DAL_LSEEK_WHENCE_CUR); //偏移不变
	}
}