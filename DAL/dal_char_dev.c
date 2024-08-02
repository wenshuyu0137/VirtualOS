/**
 * @file dal_char_dev.c
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
#include "dal_char_dev.h"

typedef struct {
	dml_file_opts_t *opts;
	bool is_used;
	int fd;
} fd_t;

#define FD_SIZE 64
static fd_t fds[FD_SIZE] = { 0 };

static int alloc_fd(void)
{
	for (uint16_t i = 0; i < FD_SIZE; i++) {
		if (!fds[i].is_used) {
			fds[i].is_used = true;
			fds[i].opts = NULL;
			return fds[i].fd;
		}
	}
	return -1; // 没有可用的文件描述符
}

static void free_fd(int fd)
{
	if (fd >= 0 && fd < FD_SIZE) {
		fds[fd].is_used = false;
		fds[fd].opts = NULL;
	}
}

static int check_fd(int fd, dml_file_opts_t **opts)
{
	if (fd < 0 || fd >= FD_SIZE)
		return -6; // 非法文件描述符

	if (!fds[fd].opts || !opts)
		return -3; // 操作异常

	*opts = fds[fd].opts;
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
		return -5; // 设备不存在

	int new_fd = alloc_fd();
	if (new_fd == -1)
		return -1; // 打开设备过多

	if (!dev->opts || !dev->opts->open) {
		free_fd(new_fd);
		return -3; // 操作异常
	}

	dml_dev_err_e ret = dev->opts->open();
	if (ret < 0) {
		free_fd(new_fd);
		return ret;
	}

	fds[new_fd].opts = dev->opts;
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
	dml_file_opts_t *opts;
	int err = check_fd(fd, &opts);
	if (err < 0)
		return err;

	if (!opts->close)
		return -3; // 操作异常

	dml_dev_err_e ret = opts->close();
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
	dml_file_opts_t *opts;
	int err = check_fd(fd, &opts);
	if (err < 0)
		return err;

	if (!opts->read)
		return -3; // 操作异常

	return opts->read(buf, len);
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
	dml_file_opts_t *opts;
	int err = check_fd(fd, &opts);
	if (err < 0)
		return err;

	if (!opts->write)
		return -3; // 操作异常

	return opts->write(buf, len);
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
	dml_file_opts_t *opts;
	int err = check_fd(fd, &opts);
	if (err < 0)
		return err;

	if (!opts->ioctrl)
		return -3; // 操作异常

	return opts->ioctrl(cmd, argc);
}

void fd_init(void) __attribute__((constructor(102))); // 第二优先级构造
void fd_init(void)
{
	for (uint16_t i = 0; i < FD_SIZE; i++) {
		fds[i].fd = 3 + i; //前三个文件描述符保留
		fds[i].opts = NULL;
		fds[i].is_used = false;
	}
}
