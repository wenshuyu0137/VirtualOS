/**
 * @file dal_device.h
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

#ifndef _VIRTUAL_OS_DAL_CHAR_DEV_H
#define _VIRTUAL_OS_DAL_CHAR_DEV_H

#include "dml_char_device.h"

#define RESERVED_FD_NUM 3 //前三个文件描述符为内部保留值

/**
 * 小于0错误码 -6 非法文件描述符
 * 小于0错误码 -5 设备不存在
 * 小于0错误码 -4 设备被占用
 * 小于0错误码 -3 操作异常 例如只读的进行写操作
 * 小于0错误码 -2 不可使用(例如没打开)
 * 小于0错误码 -1 打开设备过多
 * 
 */

/**
 * @brief 打开设备
 * 
 * @param dev_name 设备名
 * @return int 返回文件描述符或错误码
 * @return 小于0 参考错误码
 * @return >=0 成功，返回文件描述符
 */
int dal_open(const char *dev_name);

/**
 * @brief 关闭设备
 * 
 * @param fd 文件描述符
 * @return int 返回状态码
 * @return 小于0 参考错误码
 * @return 0 成功
 */
int dal_close(int fd);

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
int dal_read(int fd, uint8_t *buf, size_t len);

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
int dal_write(int fd, const uint8_t *buf, size_t len);

typedef enum {
	DAL_LSEEK_WHENCE_HEAD, //将指针设置到0地址
	DAL_LSEEK_WHENCE_CUR, //将指针设置到上一次地址
	DAL_LSEEK_WHENCE_TAIL, //将指针设置到末尾地址
} dal_lseek_whence_e;

/**
 * @brief 文件偏移
 * 
 * @param fd 文件描述符
 * @param offset 偏移值 正负表示向后与向前
 * @param whence 基于某个位置的偏移
 * @return int 成功返回0 失败返回1
 */
int dal_lseek(int fd, int offset, dal_lseek_whence_e whence);

#endif
