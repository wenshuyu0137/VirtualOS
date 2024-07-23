/**
 * @file queue.h
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

#ifndef _VIRTUAL_OS_QUEUE_H
#define _VIRTUAL_OS_QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define POWER_OF_TWO(x) (x && !(x & x - 1))
#define Q_MIN(a, b) (((a) <= (b)) ? (a) : (b))

typedef size_t q_size;

typedef struct {
	uint8_t *buf;
	q_size unit_bytes; //每个单元字节数
	q_size buf_size; //单元个数，必须为2的整数次幂
	volatile q_size rd;
	volatile q_size wr;
} queue_info_t;

typedef struct {
	uint16_t size;
	uint8_t *buf;
} Q_BLOCK_T;

/*注: 所有接口非线程安全*/

int queue_init(queue_info_t *q, q_size unit_bytes, uint8_t *buf, q_size count);
q_size queue_add(queue_info_t *q, uint8_t *data, q_size units);
q_size queue_get(queue_info_t *q, uint8_t *data, q_size units);
q_size queue_peek(queue_info_t *q, uint8_t *data, q_size units);
bool is_queue_empty(queue_info_t *q);

void queue_reset(queue_info_t *q);
void queue_reset_safe_from_add(queue_info_t *q);
void queue_reset_safe_from_get(queue_info_t *q);

void queue_advance_rd(queue_info_t *q, q_size units);
void queue_advance_wr(queue_info_t *q, q_size units);

#endif /*_VIRTUAL_OS_QUEUE_H*/
