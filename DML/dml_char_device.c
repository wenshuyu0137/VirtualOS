/**
 * @file dml_char_device.c
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

#include "dml_char_device.h"
#include "string_hash.h"

static bool is_dev_table_initialized = false;

#define DEVICE_TABLE_SIZE 192 // 32 * 3个设备 Hash表大小建议为3倍

static list_item tables[DEVICE_TABLE_SIZE];
static hash_table_t dev_table = {
	.table = tables,
	.table_size = DEVICE_TABLE_SIZE,
	.lock_callback = NULL,
	.unlock_callback = NULL,
};

/**
 * @brief 设备注册表初始化,上电/复位时调用
 * 
 */
void dml_dev_table_init(void)
{
	if (is_dev_table_initialized)
		return;

	for (size_t i = 0; i < DEVICE_TABLE_SIZE; i++)
		list_init(&(dev_table.table[i]));

	is_dev_table_initialized = true;
}

/**
 * @brief 根据设备名查找设备
 * 
 * @param name 设备名
 * @return dml_dev_t* 具体的设备类型,没有返回NULL
 */
dml_dev_t *dml_find_device(const char *name)
{
	if (!is_dev_table_initialized || !name)
		return NULL;

	hash_error_t err;
	void *priv = hash_find(&dev_table, name, &err);
	return (err == HASH_SUCCESS) ? (dml_dev_t *)priv : NULL;
}

/**
 * @brief 注册设备驱动到设备表
 * 
 * @param device 
 * @return true 
 * @return false 
 */
bool dml_register_device(dml_file_opts_t *opts)
{
	if (!is_dev_table_initialized || !opts || !opts->name)
		return false;

	// 设备已存在
	if (dml_find_device(opts->name))
		return false;

	dml_dev_t *dev = (dml_dev_t *)malloc(sizeof(dml_dev_t));
	dev->opts = opts;
	dev->is_open = false;

	return hash_insert(&dev_table, opts->name, (void *)dev) == HASH_SUCCESS;
}

/**
 * @brief 取消注册设备驱动
 * 
 * @param name 设备名
 * @return true 
 * @return false 
 */
bool dml_unregister_device(const char *name)
{
	if (!is_dev_table_initialized || !name)
		return false;

	hash_error_t err = hash_delete(&dev_table, name);
	return (err == HASH_SUCCESS);
}

/**
 * @brief 列出所有已经注册的设备,返回后一定要释放device_names的内存，一定要注意,可以调用下面的free_device_names来释放
 * 
 * @param device_names 存放所有字符串数组的指针
 * @param num_devices 所有的设备格式
 * @return true 
 * @return false 
 */
bool dml_list_all_devices(char ***device_names, size_t *num_devices)
{
	if (!is_dev_table_initialized || !device_names || !num_devices)
		return false;

	if (hash_get_all_keys(&dev_table, device_names, num_devices) != HASH_SUCCESS)
		return false;

	return true;
}

/**
 * @brief 辅助函数，释放获取设备名的数组
 * 
 * @param device_names 获取的数组名
 * @param num_devices 设备数量
 */
void free_device_names(char **device_names, size_t num_devices)
{
	if (device_names) {
		for (size_t i = 0; i < num_devices; ++i) {
			free(device_names[i]);
		}
		free(device_names);
	}
}
