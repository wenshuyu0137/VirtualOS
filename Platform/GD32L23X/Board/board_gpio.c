#include "board_gpio.h"

static int8_t is_board_gpio_valid(hal_gpio_t *p_io)
{
	return (((p_io->HAL_GPIO_PIN < HAL_GPIO_PIN_MAX) && (p_io->HAL_GPIO_PORT < HAL_PORT_MAX)) ? 1 : -1);
}

static uint32_t get_gpio_rcu(hal_gpio_t *p_io)
{
	return RCU_REGIDX_BIT(AHBEN_REG_OFFSET, (17U + (p_io->HAL_GPIO_PORT)));
}

static uint32_t get_port(hal_gpio_t *p_io)
{
	uint32_t port;

	switch (p_io->HAL_GPIO_PORT) {
	case HAL_GPIOA:
		port = GPIOA;
		break;

	case HAL_GPIOB:
		port = GPIOB;
		break;

	case HAL_GPIOC:
		port = GPIOC;
		break;

	case HAL_GPIOD:
		port = GPIOD;
		break;

	case HAL_GPIOF:
		port = GPIOF;
		break;

	default:
		port = 0xFFFFFFFF;
		break;
	}

	return port;
}

static uint32_t get_pin(hal_gpio_t *p_io)
{
	return ((uint32_t)((uint32_t)0x01U << (p_io->HAL_GPIO_PIN)));
}

bool board_gpio_deinit(hal_gpio_t *p_io)
{
	if (!is_board_gpio_valid(p_io)) {
		return false;
	}

	gpio_deinit(get_port(p_io));
	return true;
}

bool board_gpio_init(hal_gpio_t *p_io, hal_gpio_cfg_t *gpio_cfg)
{
	if (!is_board_gpio_valid(p_io)) {
		return false;
	}
	uint32_t port, pin;
	rcu_periph_clock_enable(get_gpio_rcu(p_io));
	port = get_port(p_io);
	pin = get_pin(p_io);
	gpio_mode_set(port, CTL_CLTR((gpio_cfg->HAL_GPIO_MODE)), PUD_PUPD((gpio_cfg->HAL_GPIO_PUPD)), pin);

	if (gpio_cfg->HAL_GPIO_AF == HAL_GPIO_AF_NONE) {
		gpio_cfg->HAL_GPIO_AF = HAL_GPIO_AF_0;
	}

	gpio_af_set(port, AF(gpio_cfg->HAL_GPIO_AF), pin);

	if (gpio_cfg->HAL_GPIO_DIR == HAL_GPIO_DIR_OUT) {
		gpio_output_options_set(port, gpio_cfg->HAL_GPIO_OTYPE, OSPD_OSPD0((gpio_cfg->HAL_GPIO_SPEED)), pin);
	}

	return true;
}

hal_gpio_pin_level_e board_gpio_pin_read(hal_gpio_t *p_io)
{
	return (hal_gpio_pin_level_e)gpio_input_bit_get(get_port(p_io), get_pin(p_io));
}

void board_gpio_pin_write(hal_gpio_t *p_io, hal_gpio_pin_level_e level)
{
	gpio_bit_write(get_port(p_io), get_pin(p_io), (level == HAL_GPIO_PIN_HIGH ? SET : RESET));
}

void board_gpio_pin_toggle(hal_gpio_t *p_io)
{
	gpio_bit_toggle(get_port(p_io), get_pin(p_io));
}
