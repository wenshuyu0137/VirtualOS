/**
 * @file dal_device.c
 * @brief 设备抽象层字符设备接口
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
#include <stdbool.h>
#include "dal_device.h"

/**
 * 小于0错误码 -6 非法文件描述符
 * 小于0错误码 -5 设备不存在
 * 小于0错误码 -4 设备被占用
 * 小于0错误码 -3 操作异常 例如只读的进行写操作,或空指针
 * 小于0错误码 -2 不可使用(例如没打开)
 * 小于0错误码 -1 打开设备过多
 * 
 */

typedef struct {
	dml_dev_t *dev;
	bool is_used;
	int fd;
} fd_t;

#define FD_SIZE 64 //支持64个文件描述符
static fd_t fds[FD_SIZE] = { 0 };

static int alloc_fd(void)
{
	for (uint16_t i = RESERVED_FD_NUM; i < FD_SIZE; i++) {
		if (!fds[i].is_used) {
			fds[i].is_used = true;
			fds[i].dev = NULL;
			return fds[i].fd;
		}
	}
	return -1;
}

static void free_fd(int fd)
{
	if (fd >= 0 && fd < FD_SIZE) {
		fds[fd].is_used = false;
		fds[fd].dev = NULL;
	}
}

static int check_fd(int fd, dml_dev_t **dev)
{
	if (fd < RESERVED_FD_NUM || fd >= FD_SIZE)
		return -6;

	if (!fds[fd].dev || !dev)
		return -3;

	*dev = fds[fd].dev;
	return 0;
}

/**
 * @brief 打开设备
 * 
 * @param dev_name 设备名
 * @return int 返回文件描述符或错误码
 * @return 小于0 参考错误码
 * @return >=0 成功，返回文件描述符
 */
int dal_open(const char *dev_name)
{
	dml_dev_t *dev = dml_find_device(dev_name);
	if (!dev)
		return -5;

	int new_fd = alloc_fd();
	if (new_fd == -1)
		return -1;

	if (!dev->opts || !dev->opts->open) {
		free_fd(new_fd);
		return -3;
	}

	dml_dev_err_e ret = dev->opts->open(dev);
	if (ret < 0) {
		free_fd(new_fd);
		return ret;
	}

	fds[new_fd].dev = dev;
	return new_fd;
}

/**
 * @brief 关闭设备
 * 
 * @param fd 文件描述符
 * @return int 返回状态码
 * @return 小于0 参考错误码
 * @return 0 成功
 */
int dal_close(int fd)
{
	dml_dev_t *dev;
	int err = check_fd(fd, &dev);
	if (err < 0)
		return err;

	if (!dev->opts->close)
		return -3;

	dml_dev_err_e ret = dev->opts->close(dev);
	if (ret < 0)
		return ret;

	free_fd(fd);
	return 0;
}

/**
 * @brief 读取设备的值
 * 
 * @param fd 文件描述符
 * @param buf 存储缓冲区
 * @param len 读取长度
 * @return int 返回读取成功的长度或错误码
 * @return 小于0 参考错误码
 * @return >=0 成功，返回读取的长度
 */
int dal_read(int fd, uint8_t *buf, size_t len)
{
	dml_dev_t *dev;
	int err = check_fd(fd, &dev);
	if (err < 0)
		return err;

	if (!dev->opts->close)
		return -3;

	return dev->opts->read(dev, buf, len);
}

/**
 * @brief 写入数据到设备
 * 
 * @param fd 文件描述符
 * @param buf 写入缓冲区
 * @param len 写入长度
 * @return int 返回写入成功的长度或错误码
 * @return 小于0 参考错误码
 * @return >=0 成功，返回写入的长度
 */
int dal_write(int fd, const uint8_t *buf, size_t len)
{
	dml_dev_t *dev;
	int err = check_fd(fd, &dev);
	if (err < 0)
		return err;

	if (!dev->opts->close)
		return -3;

	return dev->opts->write(dev, buf, len);
}

/**
 * @brief 实现除读写之外的设备操作
 * 
 * @param fd 文件描述符
 * @param cmd 指令 参考dal_reserved_cmd_e 保留指令
 * @param argc 通用参数指针
 * @return int 参考枚举变量
 * @return 小于0 参考错误码
 * @return >=0 成功，返回操作结果
 */
int dal_ioctrl(int fd, int cmd, void *argc)
{
	dml_dev_t *dev;
	int err = check_fd(fd, &dev);
	if (err < 0)
		return err;

	if (!dev->opts->close)
		return -3;

	return dev->opts->ioctrl(dev, cmd, argc);
}

/**
 * @brief 文件偏移
 * 
 * @param fd 文件描述符
 * @param offset 偏移值 正负表示向后与向前
 * @param whence 基于某个位置的偏移
 * @return int 成功返回0 失败返回1
 */
int dal_lseek(int fd, int offset, dal_lseek_whence_e whence)
{
	dml_dev_t *dev;
	int err = check_fd(fd, &dev);
	if (err < 0)
		return err;

	if (!dev->opts->close)
		return -3;

	return dev->opts->lseek(dev, offset, (dml_lseek_whence_e)whence);
}

void fd_init(void) __attribute__((constructor(102)));
void fd_init(void)
{
	for (uint16_t i = 0; i < RESERVED_FD_NUM; i++) {
		fds[i].fd = i;
		fds[i].dev = NULL;
		fds[i].is_used = true;
	}

	for (uint16_t i = RESERVED_FD_NUM; i < FD_SIZE; i++) {
		fds[i].fd = i;
		fds[i].dev = NULL;
		fds[i].is_used = false;
	}
}
