/**
 * @file at_server.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 轻量级简易AT服务端协议组件,依赖队列组件与字符串哈希组件
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

#include "at_server.h"

#define AT_READ_BUFFER_MAX_SIZE (512)

static uint16_t start_magic = 0x13AE;

static queue_info_t read_q;
static char read_q_buf[AT_READ_BUFFER_MAX_SIZE];

static hash_table_t handler_hash_table;

typedef struct {
	at_server_opts_t *opts;
	at_server_handle_t *handle_maps;
	size_t map_size;

} at_server_t;

static at_server_t m_at_server;

typedef enum {
	AT_SERVER_PARSER_STATE_PREFIX, //前缀(AT)
	AT_SERVER_PARSER_STATE_SPLIT, //分隔符 判断 + 号
	AT_SERVER_PARSER_STATE_TYPE, //具体指令
	AT_SERVER_PARSER_STATE_ARGV, //具体参数
	AT_SERVER_PARSER_STATE_SUFFIX, //结束符(例如\r\n)
} at_server_parser_state_e;

typedef struct {
	at_server_parser_state_e state; //状态
	uint8_t cmd_type_len; //指令索引

	uint16_t forward;
	uint16_t anchor;

	uint16_t pdu_len;

	uint8_t cur_len; //当前比较索引
	uint8_t compare_len; //比较长度

	uint8_t argv_index; //参数长度索引
	bool in_quotes; //在引号内
	at_server_msg_t correct; //解析完后的包
} server_parser_t;

static server_parser_t m_parser;

static q_size check_pdu_remain(void)
{
	return read_q.wr - m_parser.forward;
}

static uint8_t get_rx_remain()
{
	return read_q.buf[m_parser.forward & (read_q.buf_size - 1)];
}

static void clear_compare(void)
{
	m_parser.cur_len = 0;
	m_parser.compare_len = 0;
}

static void rebase()
{
	clear_compare();
	m_parser.state = AT_SERVER_PARSER_STATE_PREFIX;
	m_parser.compare_len = 2;
	read_q.rd = m_parser.anchor + 1;
	m_parser.anchor = read_q.rd;
	m_parser.forward = read_q.rd;
}

static void pdu_clear(void)
{
	memset(&m_parser.correct, 0, sizeof(at_server_msg_t));
	m_parser.pdu_len = 0;
	m_parser.cmd_type_len = 0;
	m_parser.argv_index = 0;
}

static void reset()
{
	clear_compare();
	m_parser.state = AT_SERVER_PARSER_STATE_PREFIX;
	m_parser.compare_len = 2;
	read_q.rd = m_parser.forward;
	m_parser.anchor = read_q.rd;
}

static void task_handle(server_parser_t *p)
{
	at_server_handler_f handle_f = (at_server_handler_f)hash_find(&handler_hash_table, p->correct.cmd, NULL);

	if (!handle_f)
		return;

	char cache[AT_READ_BUFFER_MAX_SIZE] = { 0 };
	size_t ret_len = 0;
	at_server_response_e resp = handle_f(&p->correct, cache, &ret_len);

	switch (resp) {
	case AT_SERVER_RESPONSE_NONE:
		break;

	case AT_SERVER_RESPONSE_READY:
		m_at_server.opts->f_write(cache, ret_len);
		break;

	case AT_SERVER_RESPONSE_ERROR:
		break;

	default:
		break;
	}
}

static bool will_to_suffix(server_parser_t *p, char c)
{
#if defined(SERVER_SUFFIX_WITH_CR) || defined(SERVER_SUFFIX_WITH_LF)

	if (c == SERVER_AT_SUFFIX) {
		p.state = AT_SERVER_PARSER_STATE_SUFFIX;
		clear_compare();
		p.compare_len = 1;
#else

	if (c == SERVER_AT_SUFFIX[0]) {
		p->state = AT_SERVER_PARSER_STATE_SUFFIX;
		clear_compare();
		p->compare_len = strlen(SERVER_AT_SUFFIX);
#endif
		return true;
	}

	return false;
}

static void _recv_parser(queue_info_t *r_q)
{
	if (is_queue_empty(r_q))
		return;

	char c;

	bool exit_loop = false;
	bool is_pdu_valid = false;
	static bool with_prefix = false; //例如 ATA\r\n这样错误指令

	while (check_pdu_remain()) {
		c = get_rx_remain();
		switch (m_parser.state) {
		case AT_SERVER_PARSER_STATE_PREFIX:

			if (will_to_suffix(&m_parser, c)) {
				m_parser.state = AT_SERVER_PARSER_STATE_SUFFIX;
				break;
			} else if (c == AT_SERVER_PREFIX[m_parser.cur_len++]) {
				m_parser.correct.complete[m_parser.pdu_len++] = c;
				++m_parser.forward;
				with_prefix = false;
				if (m_parser.cur_len == m_parser.compare_len) {
					m_parser.state = AT_SERVER_PARSER_STATE_SPLIT;
					clear_compare();
					with_prefix = true;
				}
			} else
				rebase();

			break;

		case AT_SERVER_PARSER_STATE_SPLIT:
			if (will_to_suffix(&m_parser, c)) {
				m_parser.state = AT_SERVER_PARSER_STATE_SUFFIX;
				break;
			} else if (c == '+') {
				m_parser.correct.complete[m_parser.pdu_len++] = c;
				++m_parser.forward;
				m_parser.state = AT_SERVER_PARSER_STATE_TYPE;
			} else {
				rebase();
			}

			break;

		case AT_SERVER_PARSER_STATE_TYPE:

			if (will_to_suffix(&m_parser, c)) {
				m_parser.correct.cmd[m_parser.cmd_type_len] = '\0'; //确保状态切换前指令部分是一个完整的字符串
				m_parser.state = AT_SERVER_PARSER_STATE_SUFFIX;
				break;
			}

			m_parser.correct.complete[m_parser.pdu_len++] = c;
			++m_parser.forward;

			if (c == '=') {
				m_parser.correct.handle_type = AT_SERVER_HANDLE_SET;
				m_parser.correct.cmd[m_parser.cmd_type_len] = '\0'; //确保状态切换前指令部分是一个完整的字符串
				m_parser.state = AT_SERVER_PARSER_STATE_ARGV;
			} else if (c == '?') {
				m_parser.correct.handle_type = AT_SERVER_HANDLE_QUERY;
				m_parser.correct.cmd[m_parser.cmd_type_len] = '\0'; //确保状态切换前指令部分是一个完整的字符串
				m_parser.state = AT_SERVER_PARSER_STATE_ARGV;
			} else
				m_parser.correct.cmd[m_parser.cmd_type_len++] = c; //存储具体的指令

			break;

		case AT_SERVER_PARSER_STATE_ARGV:

			if (will_to_suffix(&m_parser, c)) {
				// 结束前,确保最后一个参数部分以\0结尾
				m_parser.correct.argv[m_parser.correct.argc][m_parser.argv_index++] = '\0';
				m_parser.state = AT_SERVER_PARSER_STATE_SUFFIX;
				break;
			}

			m_parser.correct.complete[m_parser.pdu_len++] = c;
			++m_parser.forward;

			if (c == '\"') {
				// 切换引号状态
				m_parser.in_quotes = !m_parser.in_quotes;
				if (!m_parser.in_quotes) {
					//每一对双引号结束为一个参数
					m_parser.correct.argc++;
					m_parser.correct.argv[m_parser.correct.argc][m_parser.argv_index++] = '\0'; // 确保前一个参数以\0结尾
					m_parser.argv_index = 0; //参数索引清零
				}

			} else if (c == ',' && !m_parser.in_quotes) {
			} else {
				// 引号中的具体参数
				m_parser.correct.argv[m_parser.correct.argc][m_parser.argv_index++] = c;
			}

			break;

		case AT_SERVER_PARSER_STATE_SUFFIX:

			m_parser.correct.complete[m_parser.pdu_len++] = c;
			++m_parser.forward;
			m_parser.cur_len++;

			if (m_parser.cur_len == m_parser.compare_len) {
				exit_loop = true; //退出while

				reset();

				if (with_prefix)
					is_pdu_valid = true; //处理正确完整的包
			}

			break;
		default:
			break;
		}

		if (exit_loop)
			break;
	}

	if (!is_pdu_valid)
		return;

	if (m_parser.pdu_len < sizeof(m_parser.correct.complete))
		m_parser.correct.complete[m_parser.pdu_len++] = '\0';
	else {
		m_parser.correct.complete[sizeof(m_parser.correct.complete) - 1] = '\0';
		m_parser.pdu_len = sizeof(m_parser.correct.complete);
	}

	task_handle(&m_parser);
	pdu_clear();
}

static void r_dispatch(void)
{
	char r_buffer[AT_READ_BUFFER_MAX_SIZE] = { 0 };
	size_t rcv_size = m_at_server.opts->f_read(r_buffer, AT_READ_BUFFER_MAX_SIZE);

	if (!rcv_size)
		return;

	int ret = queue_add(&read_q, (uint8_t *)r_buffer, rcv_size);

	if (ret != rcv_size)
		return;

	_recv_parser(&read_q);
}

/**********************************************************API**********************************************************/

bool at_server_init(at_server_opts_t *opts, at_server_handle_t *handle_maps, size_t map_size)
{
	if (!opts || !opts->f_read || !opts->f_write || !handle_maps)
		return false;

	m_at_server.opts = opts;
	m_at_server.handle_maps = handle_maps;
	m_at_server.map_size = map_size;
	queue_init(&read_q, 1, (uint8_t *)read_q_buf, AT_READ_BUFFER_MAX_SIZE);
	init_hash_table(&handler_hash_table, AT_SERVER_CMD_MAX * 3, NULL, NULL);

	for (size_t i = 0; i < map_size; i++)
		hash_insert(&handler_hash_table, at_server_cmd[handle_maps[i].cmd_type].cmd, (void *)handle_maps[i].handle_f);

	start_magic = 0;
	reset(&m_parser);
	return true;
}

void at_server_dispatch(void)
{
	if (start_magic)
		return;

	r_dispatch();
}