/**
 * @file dml_char_device.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 提供设备驱动注册的相关接口
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

#ifndef _VIRTUAL_OS_DML_CHAR_DEVICE_H
#define _VIRTUAL_OS_DML_CHAR_DEVICE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
	DML_DEV_ERR_NOT_EXIST = -5, //设备不存在
	DML_DEV_ERR_OCCUPIED, //设备被占用
	DML_DEV_ERR_EXCEPTION, //操作异常 例如只读的进行写操作
	DML_DEV_ERR_UNAVALIABLE, //不可使用(例如没打开)
	DAL_DEV_ERR_OVERFLOW, //已经打开的设备超过最大可用设备
	// 0
	DML_DEV_ERR_NONE, //无错误
} dml_dev_err_e;

typedef struct {
	const char *name;
	dml_dev_err_e (*open)(void);
	dml_dev_err_e (*close)(void);
	dml_dev_err_e (*ioctrl)(int cmd, void *arg);
	int (*read)(uint8_t *buf, size_t len);
	int (*write)(const uint8_t *buf, size_t len);
} dml_file_opts_t;

typedef struct {
	dml_file_opts_t *opts;

	//TODO
} dml_dev_t;

void dml_dev_table_init(void);
dml_dev_t *dml_find_device(const char *name);
bool dml_register_device(dml_file_opts_t *device);
bool dml_unregister_device(const char *name);
bool dml_list_all_devices(char ***device_names, size_t *num_devices);
void free_device_names(char **device_names, size_t num_devices);

#endif /*_VIRTUAL_OS_DML_CHAR_DEVICE_H*/
