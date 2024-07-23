/**
 * @file at_client.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 轻量级简易AT客户端协议组件,仅依赖queue.c/.h队列文件
 * @version 0.1
 * @date 2024-07-12
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

#include "at_client.h"

#define AT_READ_BUFFER_MAX_SIZE (512)

static uint8_t start_magic = 0x83;

static queue_info_t read_q;
static char read_q_buf[AT_READ_BUFFER_MAX_SIZE];

static queue_info_t write_q;
static at_client_cmd_t *write_q_buf[AT_CLIENT_CMD_MAX];

static queue_info_t sync_handle_q;
static at_client_cmd_t *sync_handle_buf[AT_CLIENT_CMD_MAX];

typedef struct {
	at_client_opts_t *opts;
	at_client_handle_t *handle_maps;
	size_t map_size;
} at_client_t;

static at_client_t m_at_client;

static const char *const success_str = "OK";
static const char *const fail_str = "ERROR";

typedef enum {
	AT_CLIENT_PARSER_STATE_START, //开始
	AT_CLIENT_PARSER_STATE_RECIEVING, //接收中
	AT_CLIENT_PARSER_STATE_SUFFIX, //换行
	AT_CLIENT_PARSER_STATE_OK, //判断OK结束
	AT_CLIENT_PARSER_STATE_ERROR, //判断ERROR结束
	AT_CLIENT_PARSER_STATE_FINISH, //一包解析完成
} at_client_paraer_state_e;

typedef struct {
	uint16_t forward;
	uint16_t line_index;
	char parser_pdu[AT_READ_BUFFER_MAX_SIZE];
	at_client_paraer_state_e state;
	uint8_t compare_len;
	uint8_t cur_len;
	bool result;
} client_parser_t;

static client_parser_t m_parser;

static void rebase(client_parser_t *p)
{
	p->compare_len = 0;
	p->cur_len = 0;
}

static void reset(client_parser_t *p)
{
	memset(p, 0, sizeof(client_parser_t));
}

static void s_dispatch(void)
{
	at_client_cmd_t *cmd_content = NULL;
	int ret = queue_get(&write_q, (uint8_t *)&cmd_content, sizeof(at_client_cmd_t *));

	if (ret != sizeof(at_client_cmd_t *)) {
		return;
	}

	size_t len = strlen(cmd_content->cmd);
	m_at_client.opts->f_write((char *)cmd_content->cmd, len);
}

static void handle_response(const char *const response, size_t len, bool result)
{
	at_client_cmd_t *to_handle_cmd = NULL;
	int ret = queue_get(&sync_handle_q, (uint8_t *)&to_handle_cmd, sizeof(at_client_cmd_t *));

	if (ret != sizeof(at_client_cmd_t *)) {
		return;
	}

	for (size_t i = 0; i < m_at_client.map_size; i++) {
		if (to_handle_cmd->cmd_type == m_at_client.handle_maps[i].cmd_type) {
			m_at_client.handle_maps[i].handle_f(response, len, result);
		}
	}
}

static bool will_to_suffix(client_parser_t *p, char c)
{
#if defined(CLIENT_SUFFIX_WITH_CR) || defined(CLIENT_SUFFIX_WITH_LF)

	if (c == AT_CLIENT_SUFFIX) {
		p.state = AT_CLIENT_PARSER_STATE_SUFFIX;
		rebase(p);
		p.compare_len = 1;
#else

	if (c == AT_CLIENT_SUFFIX[0]) {
		p->state = AT_CLIENT_PARSER_STATE_SUFFIX;
		rebase(p);
		p->compare_len = strlen(AT_CLIENT_SUFFIX);
#endif
		return true;
	}

	return false;
}

static void _recv_parser(queue_info_t *r_q)
{
	if (is_queue_empty(r_q)) {
		return;
	}

	char c;
	bool exit_loop = false;
	bool is_pdu_valid = false;

	while (!is_queue_empty(r_q)) {
		switch (m_parser.state) {
		case AT_CLIENT_PARSER_STATE_START:
			m_parser.state = AT_CLIENT_PARSER_STATE_RECIEVING;
			break;

		case AT_CLIENT_PARSER_STATE_RECIEVING:
			queue_peek(r_q, (uint8_t *)&c, 1);

			if (will_to_suffix(&m_parser, c)) {
				//结束符
				m_parser.state = AT_CLIENT_PARSER_STATE_SUFFIX;
				break;
			}

			else if (c == success_str[0]) {
				// O
				m_parser.state = AT_CLIENT_PARSER_STATE_OK;
				m_parser.compare_len = strlen(success_str);
				m_parser.cur_len = 0;
			}

			else if (c == fail_str[0]) {
				// E
				m_parser.state = AT_CLIENT_PARSER_STATE_ERROR;
				m_parser.compare_len = strlen(fail_str);
				m_parser.cur_len = 0;
			}

			else {
				queue_get(r_q, (uint8_t *)&c, 1);
				m_parser.parser_pdu[m_parser.forward++] = c;
			}

			break;

		case AT_CLIENT_PARSER_STATE_OK:
			queue_peek(r_q, (uint8_t *)&c, 1);

			if (will_to_suffix(&m_parser, c)) {
				//结束符
				m_parser.state = AT_CLIENT_PARSER_STATE_SUFFIX;
			}

			else if (c == success_str[m_parser.cur_len]) {
				queue_get(r_q, (uint8_t *)&c, 1);
				m_parser.parser_pdu[m_parser.forward++] = c;
			}

			else {
				//是O 但不是OK 或者接收到OK但没有换行结束符 继续返回接收
				rebase(&m_parser);
				m_parser.state = AT_CLIENT_PARSER_STATE_RECIEVING;
			}

			break;

		case AT_CLIENT_PARSER_STATE_ERROR:
			queue_peek(r_q, (uint8_t *)&c, 1);

			if (will_to_suffix(&m_parser, c)) {
				//结束符
				m_parser.state = AT_CLIENT_PARSER_STATE_SUFFIX;
			}

			else if (c == fail_str[m_parser.cur_len]) {
				queue_get(r_q, (uint8_t *)&c, 1);
				m_parser.parser_pdu[m_parser.forward++] = c;
			}

			else {
				//是E 但不是完整的ERROR 或者接收到ERROR但没有换行结束符 继续返回接收
				rebase(&m_parser);
				m_parser.state = AT_CLIENT_PARSER_STATE_RECIEVING;
			}

			break;

		case AT_CLIENT_PARSER_STATE_SUFFIX:
			queue_get(r_q, (uint8_t *)&c, 1);
			m_parser.parser_pdu[m_parser.forward++] = c;
			m_parser.cur_len++;

			if (m_parser.cur_len == m_parser.compare_len) {
				m_parser.state = AT_CLIENT_PARSER_STATE_FINISH;
				break;
			}

		case AT_CLIENT_PARSER_STATE_FINISH:
			exit_loop = true; //退出while
			is_pdu_valid = true; //处理正确完整的包
			break;

		default:
			break;
		}

		if (exit_loop) {
			break;
		}
	}

	if (!is_pdu_valid) {
		return;
	}

	if (m_parser.forward < sizeof(m_parser.parser_pdu)) {
		m_parser.parser_pdu[m_parser.forward++] = '\0';
	}

	else {
		m_parser.parser_pdu[sizeof(m_parser.parser_pdu) - 1] = '\0';
		m_parser.forward = sizeof(m_parser.parser_pdu);
	}

	handle_response(m_parser.parser_pdu, m_parser.forward, m_parser.result);
	reset(&m_parser);
}

static void r_dispatch(void)
{
	char r_buffer[AT_READ_BUFFER_MAX_SIZE] = { 0 };
	size_t rcv_size = m_at_client.opts->f_read(r_buffer, AT_READ_BUFFER_MAX_SIZE);

	if (!rcv_size) {
		return;
	}

	int ret = queue_add(&read_q, (uint8_t *)r_buffer, rcv_size);

	if (ret != rcv_size) {
		return;
	}

	_recv_parser(&read_q);
}

/**********************************************************API**********************************************************/
bool at_client_init(at_client_opts_t *opts, at_client_handle_t *handle_maps, size_t map_size)
{
	if (!opts || !opts->f_read || !opts->f_write || !handle_maps) {
		return false;
	}

	m_at_client.opts = opts;
	m_at_client.handle_maps = handle_maps;
	m_at_client.map_size = map_size;
	queue_init(&write_q, 1, (uint8_t *)write_q_buf, AT_CLIENT_CMD_MAX * sizeof(at_client_cmd_t *));
	queue_init(&sync_handle_q, 1, (uint8_t *)sync_handle_buf, AT_CLIENT_CMD_MAX * sizeof(at_client_cmd_t *));
	queue_init(&read_q, 1, (uint8_t *)read_q_buf, AT_READ_BUFFER_MAX_SIZE);
	reset(&m_parser);
	start_magic = 0;
	return true;
}

void at_client_dispatch(void)
{
	if (start_magic) {
		return;
	}

	s_dispatch();
	r_dispatch();
}

void at_client_send(at_client_cmd_type_e cmd_type)
{
	const at_client_cmd_t *const msg = &at_client_cmd[cmd_type];
	queue_add(&sync_handle_q, (uint8_t *)&msg, sizeof(at_client_cmd_t *));
	queue_add(&write_q, (uint8_t *)&msg, sizeof(at_client_cmd_t *));
}