/**
 * @file dal_char_dev.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
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

#include <stdint.h>
#include "dal_char_dev.h"

/**
 * @brief 打开一个设备
 * 
 * @param dev_name 
 * @return dal_dev_err_e 参考枚举变量
 */
dal_dev_err_e dal_open(const char *dev_name)
{
	dml_char_dev_t *dev = dml_find_device(dev_name);
	if (!dev)
		return DAL_DEV_ERR_NOT_EXIST;

	//TODO
	return DAL_DEV_ERR_NONE;
}

/**
 * @brief 关闭一个设备
 * 
 * @param dev_name 
 * @return dal_dev_err_e 参考枚举变量
 */
dal_dev_err_e dal_close(const char *dev_name)
{
	dml_char_dev_t *dev = dml_find_device(dev_name);
	if (!dev)
		return DAL_DEV_ERR_NOT_EXIST;

	//TODO
	return DAL_DEV_ERR_NONE;
}

/**
 * @brief 读取设备的值
 * 
 * @param dev_name 设备名
 * @param buf 存储缓冲区
 * @param len 读取长度
 * @return int 返回读取成功的长度,小于0为错误值
 */
int dal_read(const char *dev_name, uint8_t *buf, size_t len)
{
	dml_char_dev_t *dev = dml_find_device(dev_name);
	if (!dev)
		return DAL_DEV_ERR_NOT_EXIST;

	int ret = dev->read(buf, len);

	return ret;
}

/**
 * @brief 写入数据的设备
 * 
 * @param dev_name 设备名
 * @param buf 写入缓冲区
 * @param len 写入长度
 * @return int 返回写入成功的长度,小于0为错误值
 */
int dal_write(const char *dev_name, uint8_t *buf, size_t len)
{
	dml_char_dev_t *dev = dml_find_device(dev_name);
	if (!dev)
		return DAL_DEV_ERR_NOT_EXIST;

	int ret = dev->write(buf, len);

	return ret;
}

/**
 * @brief 实现除读写之外的设备操作
 * 
 * @param dev_name 设备名
 * @param cmd 指令
 * @param argc 通用参数指针
 * @return dal_dev_err_e 参考枚举变量
 */
dal_dev_err_e dal_ioctrl(const char *dev_name, uint8_t cmd, void *argc)
{
	//TODO
}