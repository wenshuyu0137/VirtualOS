#include "app_wifi_server.h"

#define BUFFER_SIZE 512

static hal_usart_t to_bms_serial = HAL_USART_CTOR(HAL_USART_ID_1);
static uint8_t to_bms_rx_fifo[BUFFER_SIZE];
static uint8_t to_bms_tx_buffer[BUFFER_SIZE];

static hal_usart_t to_at_serial = HAL_USART_CTOR(HAL_USART_ID_2);
static uint8_t to_at_rx_fifo[BUFFER_SIZE];
static uint8_t to_at_tx_buffer[BUFFER_SIZE];

static uint8_t middle_fifo[BUFFER_SIZE];

size_t server_opt_read(char *read_buf, size_t size)
{
	return hal_usart_read(&to_bms_serial, (uint8_t *)read_buf, size); //从BMS读取数据
}

size_t server_opt_write(char *write_buf, size_t size)
{
	return hal_usart_write(&to_bms_serial, (uint8_t *)write_buf, size);
}

at_server_opts_t server_opts = {
	.f_read = server_opt_read,
	.f_write = server_opt_write,
};

static at_server_response_e at_empty_handle(at_server_msg_t *msg, char *out, size_t *out_size)
{
	hal_usart_write(&to_at_serial, (uint8_t *)msg->complete, strlen(msg->complete)); //因为只是代发,所以不用处理各种数据
	return AT_SERVER_RESPONSE_NONE;
}

static at_server_response_e at_CWJAP_handle(at_server_msg_t *msg, char *out, size_t *out_size)
{
	hal_usart_write(&to_at_serial, (uint8_t *)msg->complete, strlen(msg->complete)); //因为只是代发,所以不用处理各种数据
	return AT_SERVER_RESPONSE_NONE;
}

static at_server_handle_t at_server_handle_maps[AT_SERVER_CMD_MAX] = {
	[AT_SERVER_CMD_AT] = { AT_SERVER_CMD_AT, at_empty_handle },
	[AT_SERVER_CMD_CWJAP] = { AT_SERVER_CMD_CWJAP, at_CWJAP_handle },
};

void app_wifi_server_init(void)
{
	hal_usart_hw_cfg_t universal_cfg = HAL_USART_HW_CFG_HELPER(HAL_UART_BAUDRATE_115200, 0, HAL_USART_MODE_BOTH, 0, 0, 0, 0);
	hal_usart_usr_cfg_t rx_cfg = {
		.buffer = to_bms_rx_fifo,
		.buffer_size = BUFFER_SIZE,
		.callback = NULL,
		.io = { .HAL_GPIO_PORT = HAL_GPIOD, .HAL_GPIO_PIN = HAL_GPIO_PIN_6 },
	};
	hal_usart_usr_cfg_t tx_cfg = {
		.buffer = to_bms_tx_buffer,
		.buffer_size = BUFFER_SIZE,
		.callback = NULL,
		.io = { .HAL_GPIO_PORT = HAL_GPIOD, .HAL_GPIO_PIN = HAL_GPIO_PIN_5 },
	};
	hal_usart_init(&to_bms_serial, &universal_cfg, &tx_cfg, &rx_cfg);
	rx_cfg.buffer = to_at_rx_fifo;
	tx_cfg.buffer = to_at_tx_buffer;
	hal_usart_init(&to_at_serial, &universal_cfg, &tx_cfg, &rx_cfg);
	at_server_init(&server_opts, at_server_handle_maps, COUNTOF(at_server_handle_maps));
}

void app_wifi_server_task(void)
{
	at_server_dispatch();
	uint8_t buf[BUFFER_SIZE];
	size_t ret = hal_usart_read(&to_at_serial, buf, BUFFER_SIZE);

	if (ret) {
		hal_usart_write(&to_bms_serial, buf, ret);
	}
}
