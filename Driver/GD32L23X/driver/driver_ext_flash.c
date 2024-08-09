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
#include <stdlib.h>
#include "gd32l23x.h"

static const char ext_flash_name[] = "/dev/ext_flash";
static dml_dev_err_e ext_flash_open(dml_dev_t *file);
static dml_dev_err_e ext_flash_close(dml_dev_t *file);
static dml_dev_err_e ext_flash_ioctrl(dml_dev_t *file, int cmd, void *arg);
static int ext_flash_read(dml_dev_t *file, uint8_t *buf, size_t len);
static int ext_flash_write(dml_dev_t *file, const uint8_t *buf, size_t len);
static int ext_flash_seek(dml_dev_t *file, int offset, dml_lseek_whence_e whence);

#define USED_SPI SPI1
#define USED_RCU RCU_SPI1

#define EXT_FLASH_READ_CMD (0x03)
#define EXT_FLASH_WRITE_CMD (0x02)
#define EXT_FLASH_WRITE_ENABLE_CMD (0x06)
#define EXT_FLASH_DUMMY_CMD (0xA5)
#define EXT_FLASH_ERASE_CMD (0x20)
#define EXT_FLASH_GET_STATUS_CMD (0x05)
#define WR_FINISH_FLAG (0x01)

#define SPI_FLASH_PAGE_SIZE (0x100)
#define SPI_FLASH_SECTOR_SIZE (0x1000)

#define FLASH_SIZE (1024 * 1024)

typedef struct {
	uint32_t flash_offset; //地址偏移
	bool is_ext_flash_opened;
} ext_flash_data_t;

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

static void flash_wait_for_write_end(void)
{
	uint8_t flash_status = 0;

	cs_low();

	spi_send_and_recv(EXT_FLASH_GET_STATUS_CMD);

	do {
		flash_status = spi_send_and_recv(EXT_FLASH_DUMMY_CMD);
	} while (SET == (flash_status & WR_FINISH_FLAG));

	cs_high();
}

static void flash_write_enable(void)
{
	cs_low();

	spi_send_and_recv(EXT_FLASH_WRITE_ENABLE_CMD);

	cs_high();
}

/*擦除整块芯片*/
void flash_chip_erase()
{
	flash_write_enable();

	cs_low();

	spi_send_and_recv(0x60);

	cs_high();

	flash_wait_for_write_end();
}

//擦除
static void flash_sector_erase(uint32_t address)
{
	flash_write_enable();

	cs_low();

	spi_send_and_recv(EXT_FLASH_ERASE_CMD);

	spi_send_and_recv((address & 0xFF0000) >> 16);
	spi_send_and_recv((address & 0xFF00) >> 8);
	spi_send_and_recv(address & 0xFF);

	cs_high();

	flash_wait_for_write_end();
}

static void flash_page_write(const uint8_t *buf, uint32_t address, size_t size)
{
	flash_write_enable();

	cs_low();

	spi_send_and_recv(EXT_FLASH_WRITE_CMD);
	spi_send_and_recv((address & 0xFF0000) >> 16);
	spi_send_and_recv((address & 0xFF00) >> 8);
	spi_send_and_recv(address & 0xFF);

	while (size--) {
		spi_send_and_recv((uint16_t)*buf);
		buf++;
	}
	cs_high();

	flash_wait_for_write_end();
}

/************************************************************************************************************/

static dml_dev_err_e ext_flash_open(dml_dev_t *file)
{
	if (file->is_opened)
		return DML_DEV_ERR_OCCUPIED;

	rcu_periph_clock_enable(RCU_GPIOB);

	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

	gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_13); //SCK MISO MOSI
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

	cs_high();

	rcu_periph_clock_enable(USED_RCU);
	spi_parameter_struct spi_init_struct;
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);
	spi_enable(USED_SPI);

	// flash_chip_erase();

	ext_flash_data_t *private = (ext_flash_data_t *)malloc(sizeof(ext_flash_data_t));
	private->flash_offset = 0;
	private->is_ext_flash_opened = true;
	file->private_data = (void *)private;

	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e ext_flash_close(dml_dev_t *file)
{
	//低功耗 TODO

	if (file->private_data) {
		free(file->private_data);
		file->private_data = NULL;
	}

	file->is_opened = false;
	return DML_DEV_ERR_NONE;
}

static dml_dev_err_e ext_flash_ioctrl(dml_dev_t *file, int cmd, void *arg)
{
	return DML_DEV_ERR_NONE;
}

static int ext_flash_read(dml_dev_t *file, uint8_t *buf, size_t len)
{
	if (file->is_opened)
		return DML_DEV_ERR_OCCUPIED;

	ext_flash_data_t *flash_data = (ext_flash_data_t *)file->private_data;

	uint32_t cur_offset = flash_data->flash_offset;

	cs_low();

	spi_send_and_recv(EXT_FLASH_READ_CMD);

	spi_send_and_recv((cur_offset & 0xFF0000) >> 16);
	spi_send_and_recv((cur_offset & 0xFF00) >> 8);
	spi_send_and_recv(cur_offset & 0xFF);

	while (len--) {
		*buf = spi_send_and_recv(EXT_FLASH_DUMMY_CMD);
		buf++;
	}

	cs_high();

	return len;
}

static int ext_flash_write(dml_dev_t *file, const uint8_t *buf, size_t len)
{
	if (file->is_opened)
		return DML_DEV_ERR_OCCUPIED;

	ext_flash_data_t *flash_data = (ext_flash_data_t *)file->private_data;

	uint32_t cur_offset = flash_data->flash_offset;

	uint32_t first_sector = cur_offset / SPI_FLASH_SECTOR_SIZE;
	uint32_t last_sector = (cur_offset + len - 1) / SPI_FLASH_SECTOR_SIZE;

	// 擦除涉及的扇区
	for (uint32_t sector = first_sector; sector <= last_sector; sector++)
		flash_sector_erase(sector * SPI_FLASH_SECTOR_SIZE);

	uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

	addr = cur_offset % SPI_FLASH_PAGE_SIZE;
	count = SPI_FLASH_PAGE_SIZE - addr;
	num_of_page = len / SPI_FLASH_PAGE_SIZE;
	num_of_single = len % SPI_FLASH_PAGE_SIZE;

	if (0 == addr) {
		if (0 == num_of_page) {
			flash_page_write(buf, cur_offset, len);
		} else {
			while (num_of_page--) {
				flash_page_write(buf, cur_offset, SPI_FLASH_PAGE_SIZE);
				cur_offset += SPI_FLASH_PAGE_SIZE;
				buf += SPI_FLASH_PAGE_SIZE;
			}
			flash_page_write(buf, cur_offset, num_of_single);
		}
	} else {
		if (0 == num_of_page) {
			if (num_of_single > count) {
				temp = num_of_single - count;
				flash_page_write(buf, cur_offset, count);
				cur_offset += count;
				buf += count;
				flash_page_write(buf, cur_offset, temp);
			} else {
				flash_page_write(buf, cur_offset, len);
			}
		} else {
			len -= count;
			num_of_page = len / SPI_FLASH_PAGE_SIZE;
			num_of_single = len % SPI_FLASH_PAGE_SIZE;

			flash_page_write(buf, cur_offset, count);
			cur_offset += count;
			buf += count;

			while (num_of_page--) {
				flash_page_write(buf, cur_offset, SPI_FLASH_PAGE_SIZE);
				cur_offset += SPI_FLASH_PAGE_SIZE;
				buf += SPI_FLASH_PAGE_SIZE;
			}

			if (0 != num_of_single) {
				flash_page_write(buf, cur_offset, num_of_single);
			}
		}
	}

	return len;
}

static int ext_flash_seek(dml_dev_t *file, int offset, dml_lseek_whence_e whence)
{
	if (file->is_opened)
		return DML_DEV_ERR_OCCUPIED;

	ext_flash_data_t *flash_data = (ext_flash_data_t *)file->private_data;

	uint32_t cur_offset = flash_data->flash_offset;

	uint32_t anchor;

	switch (whence) {
	case DML_LSEEK_WHENCE_HEAD: //头
		anchor = 0;
		break;
	case DML_LSEEK_WHENCE_CUR: //当前
		anchor = cur_offset;
		break;
	case DML_LSEEK_WHENCE_TAIL: //尾
		anchor = FLASH_SIZE - 1;
		break;
	default:
		anchor = cur_offset;
		break;
	}

	uint32_t new_offset = anchor + offset; //隐式处理溢出

	new_offset = new_offset % FLASH_SIZE;

	flash_data->flash_offset = new_offset;
	return 0;
}

/************************************************************************************************************/

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
