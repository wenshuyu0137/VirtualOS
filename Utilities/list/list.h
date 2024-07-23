/**
 * @file list.h
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

#ifndef _VIRTUAL_OS_LINK_LIST_H
#define _VIRTUAL_OS_LINK_LIST_H

#include <stdint.h>
#include <stddef.h>

#define LIST_NULL ((void *)0)

typedef struct list_item {
	struct list_item *pre;
	struct list_item *next;
} list_item;

#define container_of(ptr, type, member)                                                                                                                        \
	({                                                                                                                                                     \
		const typeof(((type *)0)->member) *__mptr = (ptr);                                                                                             \
		(type *)((char *)__mptr - offsetof(type, member));                                                                                             \
	})

#define list_for_each_safe(pos, n, head) for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/*注: 所有接口非线程安全*/

void list_init(list_item *head);
uint8_t list_delete_item(list_item *item);
list_item *list_delete_tail(list_item *head);
uint8_t list_add_tail(list_item *head, list_item *item);

#endif /*_VIRTUAL_OS_LINK_LIST_H*/
