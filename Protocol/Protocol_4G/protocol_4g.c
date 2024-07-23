/**
 * @file protocol_4g.c
 * @author wenshuyu (wenhsuyu991@hellobike.com)
 * @brief 4G协议
 * @version 0.1
 * @date 2024-04-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "protocol_4g.h"

static instance_4g_t inst_4g = { .start_magic = PROTOCOL_4G_SPECIAL_VALUE };
static handle_4g_table_t handle_tables = {
	.num = 0,
	.tables = NULL,
};

static uint8_t rx_queue_buff[QUEUE_BUFF_SIZE];
static uint8_t tx_queue_buff[QUEUE_BUFF_SIZE];
static uint8_t frame_buff[QUEUE_BUFF_SIZE];

static uint8_t p_cache[QUEUE_BUFF_SIZE];
static uint16_t cache_size = 0;

static uint8_t delay_times = 1;
static volatile uint8_t trans_flag = 0;

void protocol_4g_tran_set(uint16_t topic, uint8_t *addr, uint16_t size);
void protocol_4g_handle_set(usr_4g_handle_t *tables, size_t num);

static inline int is_not_start(void)
{
	return (inst_4g.start_magic == PROTOCOL_4G_SPECIAL_VALUE);
}

static uint16_t check_rx_remain(frame_t *p_frame)
{
	return p_frame->rx_q.wr - p_frame->forward;
}

static uint8_t get_rx_remain(frame_t *p_frame)
{
	return p_frame->rx_q.buf[p_frame->forward & (p_frame->rx_q.buf_size - 1)];
}

static uint8_t get_ex_len(RECV_STATE_E state)
{
	uint8_t len = 0;

	switch (state) {
	case RECV_STATE_FRAME_HEAD:
		len = 2;
		break;

	case RECV_STATE_FRAME_TYPE:
		len = 1;
		break;

	case RECV_STATE_FRAME_TOPIC:
		len = 2;
		break;

	case RECV_STATE_FRAME_SIZE:
		len = 2;
		break;

	case RECV_STATE_FRAME_CRC:
		len = 2;
		break;

	default:
		break;
	}

	return len;
}

static uint8_t is_type_valid(uint8_t c)
{
	return c == FRAME_FROM_4G || c == FRAME_FROM_BMS;
}

static void rebase_parser(frame_t *p_frame)
{
	p_frame->state = RECV_STATE_FRAME_HEAD;
	p_frame->cal_crc = 0xFFFF;
	p_frame->pdu_in = 0;
	p_frame->extern_in = get_ex_len(RECV_STATE_FRAME_HEAD);
	p_frame->rx_q.rd = p_frame->anchor + 1;
	p_frame->anchor = p_frame->rx_q.rd;
	p_frame->forward = p_frame->rx_q.rd;
}

static void flush_parser(frame_t *p_frame)
{
	p_frame->state = RECV_STATE_FRAME_HEAD;
	p_frame->cal_crc = 0xFFFF;
	p_frame->pdu_in = 0;
	p_frame->extern_in = get_ex_len(RECV_STATE_FRAME_HEAD);
	p_frame->rx_q.rd = p_frame->forward;
	p_frame->anchor = p_frame->rx_q.rd;
}

static uint16_t get_one_tx_frame(queue_info_t *tx_q)
{
	q_size high_len = (tx_q->rd + 5) & (tx_q->buf_size - 1);
	q_size low_len = (tx_q->rd + 6) & (tx_q->buf_size - 1);
	return (tx_q->buf[high_len] << 8 | tx_q->buf[low_len]) + FRAME_OVERHEAD;
}

static bool _recv_parser(frame_t *p_frame)
{
	uint8_t c;

	while (check_rx_remain(p_frame)) {
		c = get_rx_remain(p_frame);
		++p_frame->forward;

		switch (p_frame->state) {
		case RECV_STATE_FRAME_HEAD:
			p_frame->frame_head &= ~(0xff << (8 * (--p_frame->extern_in)));
			p_frame->frame_head |= c << (8 * (p_frame->extern_in));
			p_frame->cal_crc = crc16_update(p_frame->cal_crc, c);

			if (!p_frame->extern_in) {
				if (inst_4g.is_addr_valid(p_frame->frame_head)) {
					p_frame->state = RECV_STATE_FRAME_TYPE;
				}

				else {
					rebase_parser(p_frame);
				}
			}

			break;

		case RECV_STATE_FRAME_TYPE:
			if (is_type_valid(c)) {
				p_frame->frame_from = c;
				p_frame->extern_in = get_ex_len(RECV_STATE_FRAME_TOPIC);
				p_frame->state = RECV_STATE_FRAME_TOPIC;
				p_frame->cal_crc = crc16_update(p_frame->cal_crc, c);
			}

			else {
				rebase_parser(p_frame);
			}

			break;

		case RECV_STATE_FRAME_TOPIC:
			p_frame->topic &= ~(0xff << (8 * (--p_frame->extern_in)));
			p_frame->topic |= c << (8 * (p_frame->extern_in));
			p_frame->cal_crc = crc16_update(p_frame->cal_crc, c);

			if (!p_frame->extern_in) {
				p_frame->extern_in = get_ex_len(RECV_STATE_FRAME_SIZE);
				p_frame->state = RECV_STATE_FRAME_SIZE;
			}

			break;

		case RECV_STATE_FRAME_SIZE:
			p_frame->frame_size &= ~(0xff << (8 * (--p_frame->extern_in)));
			p_frame->frame_size |= c << (8 * (p_frame->extern_in));
			p_frame->cal_crc = crc16_update(p_frame->cal_crc, c);

			if (!p_frame->extern_in) {
				if (!p_frame->frame_size) {
					p_frame->extern_in = get_ex_len(RECV_STATE_FRAME_CRC);
					p_frame->state = RECV_STATE_FRAME_CRC;
				}

				else {
					p_frame->state = RECV_STATE_FRAME_CONTENT;
				}
			}

			break;

		case RECV_STATE_FRAME_CONTENT:
			p_frame->pdu[p_frame->pdu_in++] = c;
			p_frame->cal_crc = crc16_update(p_frame->cal_crc, c);

			if (p_frame->pdu_in >= p_frame->frame_size) {
				p_frame->extern_in = get_ex_len(RECV_STATE_FRAME_CRC);
				p_frame->state = RECV_STATE_FRAME_CRC;
				p_frame->pdu_in = 0;
			}

			break;

		case RECV_STATE_FRAME_CRC:
			p_frame->pdu_crc &= ~(0xff << (8 * (--p_frame->extern_in)));
			p_frame->pdu_crc |= c << (8 * (p_frame->extern_in));

			if (!p_frame->extern_in) {
				p_frame->cal_crc = (p_frame->cal_crc >> 8) | (p_frame->cal_crc << 8);

				if (p_frame->pdu_crc == p_frame->cal_crc) {
					flush_parser(p_frame);
					return true;
				}

				else {
					rebase_parser(p_frame);
				}
			}

			break;

		default:
			break;
		}
	}

	return false;
}

static void _dispatch_msg(frame_t *frame)
{
	PROTOCOL_REPLY_STATE_E ret = PROTOCOL_REPLY_STATE_NONE;

	if (frame->frame_from != FRAME_FROM_4G) {
		return;
	}

	for (uint16_t i = 0; i < handle_tables.num; i++) {
		if (handle_tables.tables[i].topic != frame->topic || !handle_tables.tables[i].f_handle) {
			continue;
		}

		memset(p_cache, 0, QUEUE_BUFF_SIZE);
		cache_size = 0;
		ret = handle_tables.tables[i].f_handle(frame->pdu, frame->frame_size, p_cache, &cache_size);
		memset(frame->pdu, 0, QUEUE_BUFF_SIZE);
		frame->frame_size = 0;
		break;
	}

	if (ret == PROTOCOL_REPLY_STATE_READY) {
		protocol_4g_tran_set(frame->topic, p_cache, cache_size);
	}

	else if (ret == PROTOCOL_REPLY_STATE_EMPTY) {
		protocol_4g_tran_set(frame->topic, NULL, 0);
	}
}

static void r_dispatch(void)
{
	int ptk_len = inst_4g.opts->f_recv(frame_buff, QUEUE_BUFF_SIZE);

	if (ptk_len > 0) {
		queue_add(&(inst_4g.frame_msg.rx_q), frame_buff, ptk_len);

		if (_recv_parser(&(inst_4g.frame_msg))) {
			_dispatch_msg(&(inst_4g.frame_msg));
		}
	}
}

static void s_dispatch(void)
{
	if (is_queue_empty(&inst_4g.frame_msg.tx_q)) {
		return;
	}

	if (trans_flag) {
		--trans_flag;
		return;
	}

	uint8_t buf[QUEUE_BUFF_SIZE];
	memset(buf, 0, QUEUE_BUFF_SIZE);
	uint16_t size = get_one_tx_frame(&inst_4g.frame_msg.tx_q);
	queue_get(&inst_4g.frame_msg.tx_q, buf, size);
	inst_4g.opts->f_send(buf, size);
	trans_flag = delay_times;
}

void protocol_4g_handle_set(usr_4g_handle_t *tables, size_t num)
{
	handle_tables.tables = tables;
	handle_tables.num = num;
}

void protocol_4g_tran_set(uint16_t topic, uint8_t *addr, uint16_t size)
{
	if (QUEUE_BUFF_SIZE - size < FRAME_OVERHEAD) {
		return;
	}

	uint8_t buf[QUEUE_BUFF_SIZE];
	memset(buf, 0, QUEUE_BUFF_SIZE);
	uint16_t i = 0;
	buf[i++] = FRAME_HEAD_MAGIC_1;
	buf[i++] = FRAME_HEAD_MAGIC_2;
	buf[i++] = FRAME_FROM_BMS;
	buf[i++] = GET_HIGH_FROM_U16(topic);
	buf[i++] = GET_LOW_FROM_U16(topic);
	buf[i++] = GET_HIGH_FROM_U16(size);
	buf[i++] = GET_LOW_FROM_U16(size);

	if (addr && size) {
		memcpy(buf + i, addr, size);
		i += size;
	}

	uint16_t crc = crc16_update_bytes(0xffff, buf, i);
	buf[i++] = GET_LOW_FROM_U16(crc);
	buf[i++] = GET_HIGH_FROM_U16(crc);
	queue_add(&inst_4g.frame_msg.tx_q, buf, i);
}

void protocol_4G_init(protocol_4g_opts_t *opts, validator f_valid)
{
	if (!is_not_start() || !opts || !f_valid || !opts->f_init || !opts->f_recv || !opts->f_send) {
		return;
	}

	inst_4g.is_addr_valid = f_valid;
	inst_4g.opts = opts;
	queue_init(&inst_4g.frame_msg.rx_q, sizeof(uint8_t), rx_queue_buff, QUEUE_BUFF_SIZE);
	queue_init(&inst_4g.frame_msg.tx_q, sizeof(uint8_t), tx_queue_buff, QUEUE_BUFF_SIZE);
	inst_4g.frame_msg.anchor = 0;
	inst_4g.frame_msg.forward = 0;
	inst_4g.frame_msg.state = RECV_STATE_FRAME_HEAD;
	inst_4g.frame_msg.extern_in = get_ex_len(RECV_STATE_FRAME_HEAD);
	inst_4g.frame_msg.cal_crc = 0xFFFF;
	opts->f_init();
	inst_4g.start_magic = 0;
}

void protocol_4G_dispatch(void)
{
	if (is_not_start()) {
		return;
	}

	s_dispatch();
	r_dispatch();
}
