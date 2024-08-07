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
	CEP_FRAME_STATE_SRC, //电池:0x07 云端:0x3D 上位机:0x09
	CEP_FRAME_STATE_DST, //电池:0x07 云端:0x3D 上位机:0x09
	CEP_FRAME_STATE_CMD, //E1 - E6
	CEP_FRAME_STATE_IDX, //数据索引
	CEP_FRAME_STATE_DATA, //数据内容
	CEP_FRAME_STATE_CHECK, //校验码 未知
} cep_frame_state_e;

typedef struct {
	uint8_t index; //数据索引
	size_t pdu_len; //数据长度
	uint8_t src; //来源
	uint8_t cmd; //来源
	uint8_t data[256];
} correct_pdu_t;

typedef struct {
	queue_info_t *rx_q; //接收队列
	q_size forword; //辅助解析索引
	q_size anchor; //辅助解析索引

	cep_frame_state_e state; //状态机

	correct_pdu_t correct; //完整的数据帧

	uint16_t check_code; //校验码
} cep_msg_pdu_t;

typedef struct {
	uint32_t satrt_falg; //起始标志
	file_opts *opts; //读写回调
} cep_t;

static cep_t m_cep = {
	.satrt_falg = 0x957AEB8C, //随便写的
	.opts = NULL,
};

static cpe_hanlder_maps_t m_maps = {
	.handlers = NULL,
	.size = 0,
}; //所有处理回调映射表
static cep_msg_pdu_t m_cep_pdu; //处理解析

#define CEP_FIFO_SIZE 256
static uint8_t q_fifo[CEP_FIFO_SIZE] = { 0 };
static queue_info_t m_cep_q; //队列

typedef struct {
	uint8_t cur_len;
	uint8_t dest_len;
} state_with_len_t; //某个需要接收的数据长度
state_with_len_t head_state = { .cur_len = 0, .dest_len = 2 };
state_with_len_t data_state = { .cur_len = 0, .dest_len = 0 };
state_with_len_t check_state = { .cur_len = 0, .dest_len = 2 };

static void clear_state_with_len(void)
{
	//复位状态
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

/**
 * @brief 重置解析状态
 * 
 * @param cep_msg 
 */
static void reset(cep_msg_pdu_t *cep_msg)
{
	clear_state_with_len();
	cep_msg->state = CEP_FRAME_STATE_HEAD;
	cep_msg->rx_q->rd = cep_msg->forword;
	cep_msg->anchor = cep_msg->rx_q->rd;
}

/**
 * @brief 调整解析状态
 * 
 * @param cep_msg 
 */
static void rebase(cep_msg_pdu_t *cep_msg)
{
	memset(cep_msg->correct.data, 0, 256);
	clear_state_with_len();
	cep_msg->state = CEP_FRAME_STATE_HEAD; //复位状态机
	cep_msg->rx_q->rd = cep_msg->anchor + 1; //滑动窗口前移
	cep_msg->anchor = cep_msg->rx_q->rd;
	cep_msg->forword = cep_msg->rx_q->rd;
}

static int _recieve_parser(cep_msg_pdu_t *cep_msg)
{
	uint8_t c;
	static uint8_t check_low = 0;
	static uint8_t check_high = 0;

	while (check_pdu_remain(cep_msg)) {
		c = get_rx_remain(cep_msg); //取队首字节

		switch (cep_msg->state) {
		case CEP_FRAME_STATE_HEAD: //帧头
			if (head_state.cur_len < head_state.dest_len) {
				if (head_state.cur_len == 0 && c == 0x5A) {
					// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
					head_state.cur_len++;
					++cep_msg->forword; //辅助索引前移
				} else if (head_state.cur_len == 1 && c == 0xA5) {
					// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
					head_state.cur_len++;
					++cep_msg->forword; //辅助索引前移
				} else {
					rebase(cep_msg);
				}
			} else {
				cep_msg->state = CEP_FRAME_STATE_SIZE; //状态转移
			}
			break;

		case CEP_FRAME_STATE_SIZE:
			// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;

			data_state.dest_len = c; //更新要读取的数据长度
			++cep_msg->forword; //辅助索引前移
			cep_msg->state = CEP_FRAME_STATE_SRC; //状态转移

			break;
		case CEP_FRAME_STATE_SRC:
			//帧来源 只能是上位机或者云端
			if (c == 0x3D || c == 0x09) {
				// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
				cep_msg->correct.src = c; //保存来源
				++cep_msg->forword; //辅助索引前移
				cep_msg->state = CEP_FRAME_STATE_DST; //状态转移

			} else {
				rebase(cep_msg);
			}
			break;
		case CEP_FRAME_STATE_DST:
			//帧目标 只能是电池
			if (c == 0x07) {
				// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
				++cep_msg->forword; //辅助索引前移
				cep_msg->state = CEP_FRAME_STATE_CMD; //状态转移
			} else {
				rebase(cep_msg);
			}
			break;
		case CEP_FRAME_STATE_CMD:
			//指令
			if (c >= 0xE1 && c <= 0xE6) {
				// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
				cep_msg->correct.cmd = c; //保存指令
				++cep_msg->forword; //辅助索引前移
				cep_msg->state = CEP_FRAME_STATE_IDX; //状态转移
			} else {
				rebase(cep_msg);
			}
			break;
		case CEP_FRAME_STATE_IDX:
			//索引
			// cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;

			cep_msg->correct.index = c; //保存数据索引
			++cep_msg->forword; //辅助索引前移
			cep_msg->state = CEP_FRAME_STATE_DATA; //状态转移
			break;
		case CEP_FRAME_STATE_DATA:
			if (data_state.cur_len < data_state.dest_len) {
				//保存数据
				cep_msg->correct.data[cep_msg->correct.pdu_len++] = c;
				++cep_msg->forword; //辅助索引前移
				data_state.cur_len++;
			} else {
				cep_msg->state = CEP_FRAME_STATE_CHECK; //状态转移
			}
			break;
		case CEP_FRAME_STATE_CHECK:
			if (check_state.cur_len < check_state.dest_len) {
				if (check_state.cur_len == 0) {
					check_low = c; //低字节
					check_state.cur_len++;
					++cep_msg->forword; //辅助索引前移
				} else if (check_state.cur_len == 1) {
					check_high = c; //高字节
					check_state.cur_len++;
					++cep_msg->forword; //辅助索引前移

					//TODO 校验规则未知
					if (COMBINE_U8_2_U16(check_high, check_low) == 0xFFFF) {
						// cep_msg->correct.data[cep_msg->correct.pdu_len++] = check_low;
						// cep_msg->correct.data[cep_msg->correct.pdu_len++] = check_high;
						reset(cep_msg);
						return 0;
					} else {
						rebase(cep_msg);
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return 1;
}

static void ack_frame(correct_pdu_t *correct)
{
	uint8_t frame[256] = { 0 };
	memset(frame, 0, 256);
	uint8_t index = 0;

	frame[index++] = 0x5A;
	frame[index++] = 0xA5;
	frame[index++] = correct->pdu_len;
	frame[index++] = 0x07; //来源是电池
	frame[index++] = correct->src; //回给来源
	frame[index++] = correct->cmd;
	frame[index++] = correct->index;
	memcpy(frame + index, correct->data, correct->pdu_len);
	index += correct->pdu_len;

	frame[index++] = 0xFF; //TODO 校验方式未知
	frame[index++] = 0xFF;

	m_cep.opts->write(frame, index);

	memset(correct, 0, sizeof(correct_pdu_t));
}

static void notify_handler(correct_pdu_t *correct)
{
	for (uint8_t i = 0; i < m_maps.size; i++) {
		if (correct->index == m_maps.handlers[i].index && m_maps.handlers[i].handler) {
			cpe_handler_err_e ret = m_maps.handlers[i].handler(&correct->pdu_len, correct->data);
			if (ret == CPE_HANDLER_ERR_NONE)
				ack_frame(correct); //回复数据
		}
	}
}

static void cep_r_dispatch(void)
{
	uint8_t buf[256];
	int ret = m_cep.opts->read(buf, 256); //队列非空
	if (!ret)
		return;

	queue_add(m_cep_pdu.rx_q, buf, ret);

	if (_recieve_parser(&m_cep_pdu) == 0) //解析出完整包
		notify_handler(&m_cep_pdu.correct); //通知回调
}

static void cep_s_dispatch(void)
{
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
	queue_init(&m_cep_q, 1, q_fifo, CEP_FIFO_SIZE);

	m_cep_pdu.check_code = 0;
	m_cep_pdu.anchor = 0;
	m_cep_pdu.forword = 0;
	m_cep_pdu.rx_q = &m_cep_q;
	m_cep_pdu.state = CEP_FRAME_STATE_HEAD;

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