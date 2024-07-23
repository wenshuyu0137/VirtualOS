#include "app_wifi.h"
#include "SEGGER_RTT.h"

#define RX_FIFO_SIZE 512
#define TX_BUFFER_SIZE 512

static hal_usart_t m_serial = HAL_USART_CTOR(HAL_USART_ID_1);
static hal_gpio_t serial_en_io = { .HAL_GPIO_PORT = HAL_GPIOC, .HAL_GPIO_PIN = HAL_GPIO_PIN_11 };
static hal_gpio_cfg_t serial_en_io_cfg = {
	.HAL_GPIO_AF = HAL_GPIO_AF_NONE,
	.HAL_GPIO_DIR = HAL_GPIO_DIR_OUT,
	.HAL_GPIO_MODE = HAL_GPIO_MODE_OUTPUT,
	.HAL_GPIO_OTYPE = HAL_GPIO_OTYPE_PP,
	.HAL_GPIO_PUPD = HAL_GPIO_PUPD_NONE,
	.HAL_GPIO_SPEED = HAL_GPIO_SPEED_50M,
};

static uint8_t rx_fifo[RX_FIFO_SIZE];
static uint8_t tx_buffer[RX_FIFO_SIZE];
static size_t app_wifi_read(char *buf, size_t len);
static size_t app_wifi_write(char *buf, size_t len);
static at_client_opts_t at_opts = {
	.f_read = app_wifi_read,
	.f_write = app_wifi_write,
};

static void to_rcv_mode(void)
{
	hal_gpio_pin_write(&serial_en_io, HAL_GPIO_PIN_LOW);
}

static void tx_cb(void)
{
	defer_task_add(to_rcv_mode, STIMER_PERIOD_PER_TICK_MS);
}

static void rx_cb(void)
{
}

static size_t app_wifi_read(char *buf, size_t len)
{
	return hal_usart_read(&m_serial, (uint8_t *)buf, len);
}

static size_t app_wifi_write(char *buf, size_t len)
{
	hal_gpio_pin_write(&serial_en_io, HAL_GPIO_PIN_HIGH);
	return hal_usart_write(&m_serial, (uint8_t *)buf, len);
}

void at_test(const char *const content, size_t len, bool result)
{
	RTT_PRINT("%s", content);
}

static at_client_handle_t m_at_maps[AT_CLIENT_CMD_MAX] = {
	[AT_CLIENT_CMD_AT] = { .cmd_type = AT_CLIENT_CMD_AT, .handle_f = at_test },
};

static void app_at_test(void)
{
	static uint16_t counter = 0;

	if (counter >= SECOND(3)) {
		at_client_send(AT_CLIENT_CMD_AT);
		counter = 0;
	}

	counter += APP_WIFI_TASK_PERIOD;
}

static void write_test(void)
{
	uint8_t buf[3] = { 0x12, 0x34, 0x56 };
	static uint16_t counter = 0;

	if (counter >= SECOND(1)) {
		hal_gpio_pin_write(&serial_en_io, HAL_GPIO_PIN_HIGH);
		hal_usart_write(&m_serial, buf, 3);
		counter = 0;
	}

	counter += APP_WIFI_TASK_PERIOD;
}

static void app_wifi_requests(void)
{
	// app_at_test();
	write_test();
}

void app_wifi_init(void)
{
	hal_gpio_init(&serial_en_io, &serial_en_io_cfg);
	hal_usart_hw_cfg_t serial_hw_cfg = HAL_USART_HW_CFG_HELPER(HAL_UART_BAUDRATE_115200, 0, HAL_USART_MODE_BOTH, 0, 0, 0, 0);
	hal_usart_usr_cfg_t tx_cfg = {
		.buffer = tx_buffer,
		.buffer_size = TX_BUFFER_SIZE,
		.callback = tx_cb,
		.io = { .HAL_GPIO_PORT = HAL_GPIOD, .HAL_GPIO_PIN = HAL_GPIO_PIN_5 },
	};
	hal_usart_usr_cfg_t rx_cfg = {
		.buffer = rx_fifo,
		.buffer_size = RX_FIFO_SIZE,
		.callback = rx_cb,
		.io = { .HAL_GPIO_PORT = HAL_GPIOD, .HAL_GPIO_PIN = HAL_GPIO_PIN_6 },
	};
	hal_usart_init(&m_serial, &serial_hw_cfg, &tx_cfg, &rx_cfg);
	hal_gpio_pin_write(&serial_en_io, HAL_GPIO_PIN_LOW);
	at_client_init(&at_opts, m_at_maps, COUNTOF(m_at_maps));
}

void app_wifi_task(void)
{
	app_wifi_requests();
	at_client_dispatch();
}