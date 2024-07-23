/**
 * @file slog.h
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef _VIRTUAL_OS_SLOG_H
#define _VIRTUAL_OS_SLOG_H

#include <stdint.h>

#define LOGV 0
#define LOGI 1
#define LOGD 2
#define LOGW 3
#define LOGE 4

#define LOG_ON
#define LOG_LVL LOGI

typedef uint8_t LOG_LEVEL_E;

#ifndef SLOG_BUFF_BYTES
#define SLOG_BUFF_BYTES (256)
#endif

/**
 * 底层发送接口函数
 * @param data 发送的内容
 * @param len 数据长度
 */
typedef void (*low_stream)(const char *data, int len);

#if defined(LOG_ON)
/**
 * 日志输出接口，该接口非线程安全
 * @param lv @LOG_LEVEL_E, 当前日志输出等级
 * @param fmt 字符串的格式化
 */
void slog(LOG_LEVEL_E lv, char *fmt, ...);
void slog_hexdump(const char *tag, void *buf, uint16_t size);

#if (LOG_LVL <= LOGV)
#define slog_v(...) slog(LOGV, __VA_ARGS__)
#else
#define slog_v(...)
#endif

#if (LOG_LVL <= LOGI)
#define slog_i(...) slog(LOGI, __VA_ARGS__)
#else
#define slog_i(...)
#endif

#if (LOG_LVL <= LOGD)
#define slog_d(...) slog(LOGD, __VA_ARGS__)
#else
#define slog_d(...)
#endif

#if (LOG_LVL <= LOGW)
#define slog_w(...) slog(LOGW, __VA_ARGS__)
#else
#define slog_w(...)
#endif

#if (LOG_LVL <= LOGE)
#define slog_e(...) slog(LOGE, __VA_ARGS__)
#else
#define slog_e(...)
#endif

#if (LOG_LVL <= LOGE)
#define slog_h(tag, buf, size) slog_hexdump(tag, buf, size)
#else
#define slog_h(tag, buf, size)
#endif

#else
#define slog(...)
#define slog_v(...)
#define slog_i(...)
#define slog_d(...)
#define slog_w(...)
#define slog_e(...)
#define slog_h(tag, buf, size)
#endif

/**
 * 设置日志调试等级，大于等于lv等级的日志才会被输出
 * @param lv @LOG_LEVEL_E, 日志等级
 */
void slog_set_level(LOG_LEVEL_E lv);

/**
 * 设置底层字符发送方法
 * @param stream 接口函数
 */
void slog_set_low_stream(low_stream stream);

/**
 * 设置日志缓存指针以及大小，默认内部已有256字节缓存
 * @param p_buf 日志缓存指针
 * @param buf_bytes 缓存大小
 */
void slog_set_buffer(char *p_buf, int buf_bytes);

#endif /*_VIRTUAL_OS_SLOG_H*/
