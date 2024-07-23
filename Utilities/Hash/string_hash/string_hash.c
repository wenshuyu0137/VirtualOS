/**
 * @file string_hash.c
 * @author wenshuyu
 * @brief 字符串哈希查找
 * @version 0.1
 * @date 2024-07-17
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

#include "string_hash.h"

// FNV-1a hash
static uint32_t hash(const char *key, size_t table_size)
{
	uint32_t hash = 2166136261u;

	while (*key) {
		hash ^= (uint8_t)*key++;
		hash *= 16777619;
	}

	return hash % table_size; //防止超出
}

hash_error_t init_hash_table(hash_table_t *hash_table, size_t table_size, void (*lock_callback)(void), void (*unlock_callback)(void))
{
	hash_table->table = (list_item *)malloc(table_size * sizeof(list_item));

	if (!hash_table->table) {
		return HASH_POINT_ERROR;
	}

	for (size_t i = 0; i < table_size; i++) {
		list_init(&hash_table->table[i]);
	}

	hash_table->table_size = table_size;
	hash_table->lock_callback = lock_callback;
	hash_table->unlock_callback = unlock_callback;
	return HASH_SUCCESS;
}

hash_error_t hash_insert(hash_table_t *hash_table, const char *key, void *private_data)
{
	if (!key || !private_data) {
		return HASH_POINT_ERROR;
	}

	uint32_t index = hash(key, hash_table->table_size);

	if (hash_table->lock_callback) {
		hash_table->lock_callback();
	}

	list_item *pos, *tmp;
	list_for_each_safe (pos, tmp, &hash_table->table[index]) {
		//重复直接替换
		string_hash_node_t *node = container_of(pos, string_hash_node_t, list);

		if (strcmp(node->key, key) == 0) {
			node->private_data = private_data;

			if (hash_table->unlock_callback) {
				hash_table->unlock_callback();
			}

			return HASH_SUCCESS;
		}
	}

	string_hash_node_t *new_node = (string_hash_node_t *)malloc(sizeof(string_hash_node_t));

	if (!new_node) {
		if (hash_table->unlock_callback) {
			hash_table->unlock_callback();
		}

		return HASH_POINT_ERROR;
	}

	new_node->key = (char *)malloc(strlen(key) + 1);
	if (!new_node->key) {
		free(new_node);

		if (hash_table->unlock_callback) {
			hash_table->unlock_callback();
		}

		return HASH_POINT_ERROR;
	}
	strcpy(new_node->key, key);

	new_node->private_data = private_data;
	list_add_tail(&new_node->list, &hash_table->table[index]);

	if (hash_table->unlock_callback) {
		hash_table->unlock_callback();
	}

	return HASH_SUCCESS;
}

void *hash_find(hash_table_t *hash_table, const char *key, hash_error_t *error)
{
	if (!key) {
		if (error) {
			*error = HASH_POINT_ERROR;
		}

		return NULL;
	}

	uint32_t index = hash(key, hash_table->table_size);

	if (hash_table->lock_callback) {
		hash_table->lock_callback();
	}

	list_item *pos, *tmp;
	list_for_each_safe (pos, tmp, &hash_table->table[index]) {
		string_hash_node_t *node = container_of(pos, string_hash_node_t, list);

		if (strcmp(node->key, key) == 0) {
			if (hash_table->unlock_callback) {
				hash_table->unlock_callback();
			}

			if (error) {
				*error = HASH_SUCCESS;
			}

			return node->private_data;
		}
	}

	if (hash_table->unlock_callback) {
		hash_table->unlock_callback();
	}

	if (error) {
		*error = HASH_KEY_NOT_FOUND;
	}

	return NULL;
}

hash_error_t hash_delete(hash_table_t *hash_table, const char *key)
{
	if (!key) {
		return HASH_POINT_ERROR;
	}

	uint32_t index = hash(key, hash_table->table_size);

	if (hash_table->lock_callback) {
		hash_table->lock_callback();
	}

	list_item *pos, *tmp;
	list_for_each_safe (pos, tmp, &hash_table->table[index]) {
		string_hash_node_t *node = container_of(pos, string_hash_node_t, list);

		if (strcmp(node->key, key) == 0) {
			list_delete_item(pos);
			free(node->key);
			free(node);

			if (hash_table->unlock_callback) {
				hash_table->unlock_callback();
			}

			return HASH_SUCCESS;
		}
	}

	if (hash_table->unlock_callback) {
		hash_table->unlock_callback();
	}

	return HASH_KEY_NOT_FOUND;
}

void destroy_hash_table(hash_table_t *hash_table)
{
	for (size_t i = 0; i < hash_table->table_size; ++i) {
		list_item *pos, *tmp;
		list_for_each_safe (pos, tmp, &hash_table->table[i]) {
			string_hash_node_t *node = container_of(pos, string_hash_node_t, list);
			list_delete_item(pos);
			free(node->key);
			free(node);
		}
	}

	free(hash_table->table);
}