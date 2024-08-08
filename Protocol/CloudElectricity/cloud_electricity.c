/**
 * @file cloud_electricity.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
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

#include "cloud_electricity.h"

//cep ---> clould electricity protocol 云电协议

#define GET_U8_HIGH_FROM_U16(__X) ((uint8_t)((__X & 0xFF00) >> 8))
#define GET_U8_LOW_FROM_U16(__X) ((uint8_t)(__X & 0xFF))
#define COMBINE_U8_2_U16(__H, __L) ((uint16_t)(((uint8_t)__H << 8) | (uint8_t)__L))

typedef enum {
	CEP_FRAME_STATE_HEAD, //5AA5
	CEP_FRAME_STATE_SIZE, //数据长度
	CEP_FRAME_STATE_SRC, //来源-->电池:0x07 云端:0x3D 上位机:0x09
	CEP_FRAME_STATE_DST, //目标-->电池:0x07 云端:0x3D 上位机:0x09
	CEP_FRAME_STATE_CMD, //E1 - E6
	CEP_FRAME_STATE_IDX, //数据索引
	CEP_FRAME_STATE_DATA, //有效数据内容
	CEP_FRAME_STATE_CHECK, //校验码 算法未知
} cep_frame_state_e;

#define R_FIFO_SIZE 256
static uint8_t r_fifo[R_FIFO_SIZE] = { 0 };
static queue_info_t m_cep_r_q;

#define T_FIFO_SIZE 256
static uint8_t t_fifo[T_FIFO_SIZE] = { 0 };
static queue_info_t m_cep_t_q;

typedef struct {
	uint8_t index; //数据索引
	size_t pdu_len; //有效数据长度
	uint8_t src; //来源
	uint8_t cmd; //指令
	uint8_t data[256]; //有效数据内容
} correct_pdu_t;

typedef struct {
	queue_info_t *rx_q;
	queue_info_t *tx_q;
	q_size forword;
	q_size anchor;

	cep_frame_state_e state;

	correct_pdu_t correct;
} cep_msg_pdu_t;

typedef struct {
	cep_msg_pdu_t pdu;
	uint16_t satrt_falg;
	file_opts *opts;
} cep_t;

static cep_t m_cep = {
	.pdu = 0,
	.satrt_falg = 0x957A,
	.opts = NULL,
};

typedef struct {
	cpe_handler_t *handlers;
	size_t size;
} cpe_hanlder_maps_t;

static cpe_hanlder_maps_t m_maps = {
	.handlers = NULL,
	.size = 0,
};

typedef struct {
	uint8_t cur_len;
	uint8_t dest_len;
} state_with_len_t;
state_with_len_t head_state = { .cur_len = 0, .dest_len = 2 };
state_with_len_t data_state = { .cur_len = 0, .dest_len = 0 };
state_with_len_t check_state = { .cur_len = 0, .dest_len = 2 };

void cep_report(cep_cmd_e cmd, uint8_t index, uint8_t *data, uint8_t len);

static void clear_state_with_len(void)
{
	head_state = (state_with_len_t){ .cur_len = 0, .dest_len = 2 };
	data_state = (state_with_len_t){ .cur_len = 0, .dest_len = 0 };
	check_state = (state_with_len_t){ .cur_len = 0, .dest_len = 2 };
}

static q_size check_pdu_remain(cep_msg_pdu_t *cep_msg)
{
	return cep_msg->rx_q->wr - cep_msg->forword;
}

static uint8_t get_rx_remain(cep_msg_pdu_t *cep_msg)
{
	return cep_msg->rx_q->buf[cep_msg->forword & (cep_msg->rx_q->buf_size - 1)];
}

static void reset(cep_msg_pdu_t *cep_msg)
{
	clear_state_with_len();
	cep_msg->state = CEP_FRAME_STATE_HEAD;
	cep_msg->rx_q->rd = cep_msg->forword;
	cep_msg->anchor = cep_msg->rx_q->rd;
}

static void rebase(cep_msg_pdu_t *cep_msg)
{
	memset(cep_msg->correct.data, 0, 256);
	clear_state_with_len();
	cep_msg->state = CEP_FRAME_STATE_HEAD;
	cep_msg->rx_q->rd = cep_msg->anchor + 1;
	cep_msg->anchor = cep_msg->rx_q->rd;
	cep_msg->forword = cep_msg->rx_q->rd;
}

static int _recieve_parser(cep_msg_pdu_t *cep_msg)
{
	uint8_t c;
	static uint8_t check_low = 0;
	static uint8_t check_high = 0;

	while (check_pdu_remain(cep_msg)) {
		c = get_rx_remain(cep_msg);

		switch (cep_msg->state) {
		case CEP_FRAME_STATE_HEAD:
			if (head_state.cur_len < head_state.dest_len) {
				if (head_state.cur_len == 0 && c == 0x5A) {
					head_state.cur_len++;
					++cep_msg->forword;
				} else if (head_state.cur_len == 1 && c == 0xA5) {
					head_state.cur_len++;
					++cep_msg->forword;
				} else
					rebase(cep_msg);

			} else
				cep_msg->state = CEP_FRAME_STATE_SIZE;

			break;

		case CEP_FRAME_STATE_SIZE:

			data_state.dest_len = c;
			++cep_msg->forword;
			cep_msg->state = CEP_FRAME_STATE_SRC;

			break;
		case CEP_FRAME_STATE_SRC:
			if (c == 0x3D || c == 0x09) {
				cep_msg->correct.src = c;
				++cep_msg->forword;
				cep_msg->state = CEP_FRAME_STATE_DST;

			} else
				rebase(cep_msg);

			break;
		case CEP_FRAME_STATE_DST:
			if (c == 0x07) {
				++cep_msg->forword;
				cep_msg->state = CEP_FRAME_STATE_CMD;
			} else
				rebase(cep_msg);

			break;
		case CEP_FRAME_STATE_CMD:
			if (c >= 0xE1 && c <= 0xE6) {
				cep_msg->correct.cmd = c;
				++cep_msg->forword;
				cep_msg->state = CEP_FRAME_STATE_IDX;
			} else
				rebase(cep_msg);

			break;
		case CEP_FRAME_STATE_IDX:

			cep_msg->correct.index = c;
			++cep_msg->forword;
			cep_msg->state = CEP_FRAME_STATE_DATA;
			break;
		case CEP_FRAME_STATE_DATA:
			if (data_state.cur_len < data_state.dest_len) {
				cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
				++cep_msg->forword;
				data_state.cur_len++;
			} else
				cep_msg->state = CEP_FRAME_STATE_CHECK;

			break;
		case CEP_FRAME_STATE_CHECK:
			if (check_state.cur_len < check_state.dest_len) {
				if (check_state.cur_len == 0) {
					check_low = c;
					check_state.cur_len++;
					++cep_msg->forword;
				} else if (check_state.cur_len == 1) {
					check_high = c;
					check_state.cur_len++;
					++cep_msg->forword;

					//TODO 校验规则未知
					if (COMBINE_U8_2_U16(check_high, check_low) == 0xFFFF) {
						reset(cep_msg);
						return 0;
					} else
						rebase(cep_msg);
				}
			}
			break;
		default:
			break;
		}
	}
	return 1;
}

static void notify_handler(correct_pdu_t *correct)
{
	for (uint8_t i = 0; i < m_maps.size; i++) {
		if (correct->index == m_maps.handlers[i].index && m_maps.handlers[i].handler) {
			cep_err_e ret = m_maps.handlers[i].handler(&correct->pdu_len, correct->data);
			if (ret == CPE_ERR_NONE) //回复数据
				cep_report(correct->cmd, correct->index, correct->data, correct->pdu_len);
		}
	}
}

static void cep_r_dispatch(void)
{
	uint8_t buf[256];
	int ret = m_cep.opts->read(buf, 256);
	if (!ret)
		return;

	queue_add(m_cep.pdu.rx_q, buf, ret);

	if (_recieve_parser(&m_cep.pdu) == 0)
		notify_handler(&m_cep.pdu.correct);
}

static void cep_s_dispatch(void)
{
	if (is_queue_empty(m_cep.pdu.tx_q))
		return;

	q_size data_len_index = (m_cep.pdu.tx_q->rd + 2) & (m_cep.pdu.tx_q->buf_size - 1);

	uint8_t tx_len = m_cep.pdu.tx_q->buf[data_len_index] + 9;

	uint8_t buf[T_FIFO_SIZE];

	queue_get(m_cep.pdu.tx_q, buf, tx_len);

	m_cep.opts->write(buf, tx_len);
}

/*******************************************************************API*******************************************************************/
/**
 * @brief 协议初始化
 *  
 * @param maps 处理映射表
 * @param opts 读写回调
 * @return int 成功 0 失败 1
 */
int cep_init(cpe_handler_t *handlers, size_t size, file_opts *opts)
{
	if (!handlers || !opts || !opts->read || !opts->write || !m_cep.satrt_falg)
		return 1;

	m_maps.handlers = handlers;
	m_maps.size = size;
	m_cep.opts = opts;
	queue_init(&m_cep_r_q, 1, r_fifo, R_FIFO_SIZE);
	queue_init(&m_cep_t_q, 1, t_fifo, T_FIFO_SIZE);

	m_cep.pdu.anchor = 0;
	m_cep.pdu.forword = 0;
	m_cep.pdu.rx_q = &m_cep_r_q;
	m_cep.pdu.tx_q = &m_cep_t_q;
	m_cep.pdu.state = CEP_FRAME_STATE_HEAD;

	m_cep.satrt_falg = 0;
	return 0;
}

/**
 * @brief 协议调度
 * 
 */
void cep_dispatch(void)
{
	if (m_cep.satrt_falg)
		return;

	cep_r_dispatch();

	cep_s_dispatch();
}

/**
 * @brief 主动发送接口
 * 
 * @param cmd 指令
 * @param index 数据索引
 * @param data 数据内容
 * @param len 数据长度
 */
void cep_report(cep_cmd_e cmd, uint8_t index, uint8_t *data, uint8_t len)
{
	uint8_t frame[256];
	uint8_t i = 0;

	frame[i++] = 0x5A;
	frame[i++] = 0xA5;
	frame[i++] = len;
	frame[i++] = 0x07; //来源是电池?
	frame[i++] = 0x09; //回给上位机? TODO
	frame[i++] = cmd;
	frame[i++] = index;
	memcpy(frame + i, data, len);
	i += len;

	frame[i++] = 0xFF; //TODO 校验方式未知
	frame[i++] = 0xFF;

	queue_add(m_cep.pdu.tx_q, frame, i);
}