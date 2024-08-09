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

/**
 * @brief 设备操作结果的错误码
 */
typedef enum {
	DML_DEV_ERR_NOT_EXIST = -5, // 设备不存在
	DML_DEV_ERR_OCCUPIED, // 设备被占用
	DML_DEV_ERR_EXCEPTION, // 操作异常，例如对只读设备进行写操作
	DML_DEV_ERR_UNAVALIABLE, // 设备不可用（例如未打开）
	DML_DEV_ERR_OVERFLOW, // 超过最大设备数量
	DML_DEV_ERR_NONE, // 无错误
} dml_dev_err_e;

typedef struct dml_file_opts_t dml_file_opts_t;

typedef struct {
	const char *name; // 设备的名称或标识符
	dml_file_opts_t *opts; // 指向设备操作函数的指针
	void *private_data; // 设备的私有数据或上下文信息
} dml_dev_t;

struct dml_file_opts_t {
	dml_dev_err_e (*open)(dml_dev_t *file); // 打开设备
	dml_dev_err_e (*close)(dml_dev_t *file); // 关闭设备
	dml_dev_err_e (*ioctrl)(dml_dev_t *file, int cmd, void *arg); // 设备控制
	int (*read)(dml_dev_t *file, uint8_t *buf, size_t len); // 读取数据
	int (*write)(dml_dev_t *file, const uint8_t *buf, size_t len); // 写入数据
	int (*lseek)(dml_dev_t *file, long offset, int whence); // 修改文件偏移
};

void dml_dev_table_init(void);
dml_dev_t *dml_find_device(const char *name);
bool dml_register_device(dml_file_opts_t *device, const char *name);
bool dml_unregister_device(const char *name);
bool dml_list_all_devices(char ***device_names, size_t *num_devices);
void free_device_names(char **device_names, size_t num_devices);

#endif /*_VIRTUAL_OS_DML_CHAR_DEVICE_H*/
