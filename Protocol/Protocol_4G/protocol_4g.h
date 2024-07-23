#ifndef _PROTOCOL_4G_H
#define _PROTOCOL_4G_H

#include "queue.h"
#include "crc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define QUEUE_BUFF_SIZE 512 //必须为2的偶数次幂 256不够

#define PROTOCOL_4G_SPECIAL_VALUE 0x9F1A
#define FRAME_HEAD_MAGIC_1 0x5A
#define FRAME_HEAD_MAGIC_2 0xA5
#define FRAME_OVERHEAD 9 // 起始字节 + 发送源 + 主题 + 大小 + CRC

typedef enum {
	PROTOCOL_REPLY_STATE_NONE, //不回复
	PROTOCOL_REPLY_STATE_EMPTY, //回复空数据
	PROTOCOL_REPLY_STATE_READY, //有回复数据
	PROTOCOL_REPLY_STATE_ERROR, //错误(不回复)
} PROTOCOL_REPLY_STATE_E;

typedef enum {
	FRAME_FROM_BMS = 1,
	FRAME_FROM_4G = 2,
} FRAME_FROM_E;

typedef enum {
	RECV_STATE_FRAME_HEAD,
	RECV_STATE_FRAME_TYPE,
	RECV_STATE_FRAME_TOPIC,
	RECV_STATE_FRAME_SIZE,
	RECV_STATE_FRAME_CONTENT,
	RECV_STATE_FRAME_CRC,
} RECV_STATE_E;

typedef bool (*validator)(uint16_t address);
typedef void (*protocol_4g_init)(void);
typedef int (*protocol_4g_recv)(uint8_t *recv_addr, uint16_t recv_bytes);
typedef int (*protocol_4g_send)(uint8_t *send_addr, uint16_t send_bytes);

/**
 * @brief Topic接收处理
 * @param p_in 接收的数据
 * @param in_size 接收的长度
 * @param p_out 处理接收后回复的数据
 * @param out_size 回复的长度
 *
 * @return 自行处理,参考PROTOCOL_REPLY_STATE_E枚举
 */
typedef PROTOCOL_REPLY_STATE_E (*protocol_4g_handle)(uint8_t *p_in, uint16_t in_size, uint8_t *p_out, uint16_t *out_size);

typedef struct {
	uint16_t topic;
	protocol_4g_handle f_handle;
} usr_4g_handle_t;

typedef struct {
	uint16_t num;
	usr_4g_handle_t *tables;
} handle_4g_table_t;

typedef struct {
	protocol_4g_init f_init;
	protocol_4g_recv f_recv;
	protocol_4g_send f_send;
} protocol_4g_opts_t;

typedef struct {
	uint16_t frame_head;
	FRAME_FROM_E frame_from;
	uint16_t topic;
	uint16_t frame_size;

	uint16_t pdu_in;
	uint16_t extern_in;
	queue_info_t rx_q;
	queue_info_t tx_q;
	RECV_STATE_E state;
	volatile q_size anchor;
	volatile q_size forward;

	uint8_t pdu[QUEUE_BUFF_SIZE];
	uint16_t pdu_crc;
	uint16_t cal_crc;

} frame_t;

typedef struct {
	protocol_4g_opts_t *opts;
	validator is_addr_valid;
	volatile uint16_t start_magic;
	frame_t frame_msg;
} instance_4g_t;

#define GET_HIGH_FROM_U16(u) (((u) >> 8) & 0xff)
#define GET_LOW_FROM_U16(u) ((u) & 0xff)

/**************************************************API**************************************************/
void protocol_4G_init(protocol_4g_opts_t *opts, validator f_valid);
void protocol_4G_dispatch(void);
void protocol_4g_tran_set(uint16_t topic, uint8_t *addr, uint16_t size);
void protocol_4g_handle_set(usr_4g_handle_t *tables, size_t num);

#endif