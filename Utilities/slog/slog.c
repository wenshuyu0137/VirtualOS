/**
 * @file slog.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 日志组件
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

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "slog.h"

typedef struct slog_tag {
	LOG_LEVEL_E level;
	char *p_buf;
	int buf_bytes;
	low_stream stream;
} slog_t;

#if defined(LOG_ON)

static char _log_buff[SLOG_BUFF_BYTES];

static slog_t m_slog_info = {
	.level = LOGV,
	.p_buf = &_log_buff[0],
	.buf_bytes = SLOG_BUFF_BYTES,
	.stream = NULL,
};

void slog(LOG_LEVEL_E lv, char *fmt, ...)
{
	va_list args;
	int len;

	if (!m_slog_info.stream || lv < m_slog_info.level) {
		return;
	}

	va_start(args, fmt);
	len = vsnprintf(m_slog_info.p_buf, m_slog_info.buf_bytes, fmt, args);
	va_end(args);
	m_slog_info.stream(m_slog_info.p_buf, len);
}

void slog_hexdump(const char *tag, void *buf, uint16_t size)
{
	uint8_t *p_buf = (uint8_t *)buf;
	slog(LOGE, "%s:", tag);

	for (int i = 0; i < size; i++) {
		if (i % 16 == 0) {
			slog(LOGE, "\r\n%04X-%04X: ", i, i + 15);
		}

		slog(LOGE, "0x%02X ", p_buf[i]);
	}

	slog(LOGE, "\r\n");
}

#else
static slog_t m_slog_info;
#endif

void slog_set_level(LOG_LEVEL_E lv)
{
	if (lv > LOGE) {
		return;
	}

	m_slog_info.level = lv;
}

void slog_set_low_stream(low_stream stream)
{
	m_slog_info.stream = stream;
}

void slog_set_buffer(char *p_buf, int buf_bytes)
{
	m_slog_info.p_buf = p_buf;
	m_slog_info.buf_bytes = buf_bytes;
}
