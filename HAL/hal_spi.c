#include "hal_spi.h"
#include "board_spi.h"

void hal_spi_deinit(HAL_SPI_ID_E spix)
{
	board_spi_deinit(spix);
}

void hal_spi_init(HAL_SPI_ID_E spix, HAL_SPI_CFG_T *spi_cfg)
{
	board_spi_init(spix, spi_cfg);
}

uint8_t hal_spi_send_byte(HAL_SPI_ID_E spix, uint8_t byte)
{
	return board_spi_send_byte(spix, byte);
}

void hal_qspi_en_dis_cfg(HAL_SPI_ID_E spix, bool flag)
{
	board_qspi_en_dis_cfg(spix, flag);
}

void hal_qspi_wr_rd_enable(HAL_SPI_ID_E spix, HAL_QSPI_DIR_E dir)
{
	board_qspi_wr_rd_enable(spix, dir);
}
