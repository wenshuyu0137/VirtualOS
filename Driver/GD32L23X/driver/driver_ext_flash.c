/**
 * @file driver_ext_flash.c
 * @author wenshuyu (wsy2161826815@163.com)
 * @brief 外部FLASH访问驱动
 * @version 0.1
 * @date 2024-08-09
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

#include "dml_init.h"
#include "dml_char_device.h"
#include "gd32l23x.h"

static const char ext_flash_name[] = "/dev/ext_flash";
static dml_dev_err_e ext_flash_open(void);
static dml_dev_err_e ext_flash_close(void);
static dml_dev_err_e ext_flash_ioctrl(int cmd, void *arg);
static int ext_flash_read(uint8_t *buf, size_t len);
static int ext_flash_write(const uint8_t *buf, size_t len);
static int ext_flash_seek(long offset, int whence);

static bool is_ext_flash_opened = false;

#define USED_SPI SPI1
#define USED_RCU RCU_SPI1

#define EXT_FLASH_READ_CMD 0x03
#define EXT_FLASH_WRITE_CMD 0x02

typedef struct {
	uint32_t flash_offset;

}ext_flash_data_t;

static ext_flash_data_t private_data = {
	.flash_offset = 0,
};

static void cs_high(void)
{
	gpio_bit_write(GPIOB, GPIO_PIN_12, SET);
}

static void cs_low(void)
{
	gpio_bit_write(GPIOB, GPIO_PIN_12, RESET);
}

static uint16_t spi_send_and_recv(uint16_t data)
{
    while (RESET == spi_i2s_flag_get(USED_SPI, SPI_FLAG_TBE))
		;

	spi_i2s_data_transmit(USED_SPI, data);

	while (RESET == spi_i2s_flag_get(USED_SPI, SPI_FLAG_RBNE))
		;

	return (spi_i2s_data_receive(USED_SPI));
}

static dml_dev_err_e ext_flash_open(void)
{
	if (is_ext_flash_opened)
		return DML_DEV_ERR_OCCUPIED;

	is_ext_flash_opened = true;

	rcu_periph_clock_enable(RCU_GPIOB);

	gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_12); //CS
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

	gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_13); //SCK
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

	gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_14); //MISO
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

	gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_15); //MOSI
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

	cs_high();

	rcu_periph_clock_enable(USED_RCU);
	spi_parameter_struct spi_param;
	spi_param.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_param.device_mode = SPI_MASTER;
	spi_param.endian = SPI_ENDIAN_MSB;
	spi_param.frame_size = SPI_FRAMESIZE_8BIT;
	spi_param.nss = SPI_NSS_SOFT;
	spi_param.prescale = SPI_PSC_32;
	spi_param.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
	spi_init(USED_SPI, &spi_param);
	spi_enable(USED_SPI);

	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e ext_flash_close(void)
{
	//低功耗 TODO
	is_ext_flash_opened = false;
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e ext_flash_ioctrl(int cmd, void *arg)
{
	return DML_DEV_ERR_NONE;
}

static int ext_flash_read(uint8_t *buf, size_t len)
{
	if (!is_ext_flash_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	
    cs_low();

    spi_send_and_recv(EXT_FLASH_READ_CMD);

	return 1;
}

static int ext_flash_write(const uint8_t *buf, size_t len)
{
	if (!is_ext_flash_opened)
		return DML_DEV_ERR_UNAVALIABLE;

	gpio_bit_write(GPIOB, GPIO_PIN_5, (*buf ? SET : RESET));
	return 1;
}

static int ext_flash_seek(long offset, int whence)
{

}

static dml_file_opts_t ext_flash_red_dev = {
	.close = ext_flash_close,
	.ioctrl = ext_flash_ioctrl,
	.open = ext_flash_open,
	.read = ext_flash_read,
	.write = ext_flash_write,
	.lseek = ext_flash_seek,
};

void ext_flash_init(void)
{
	dml_register_device(&ext_flash_red_dev, ext_flash_name);
}

EXPORT_DIRVER(ext_flash_init) //注册驱动
