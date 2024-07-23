#ifndef _BOARD_SPI_H
#define _BOARD_SPI_H

#include "hal_spi.h"
#include "gd32l23x.h"

void board_spi_deinit(HAL_SPI_ID_E spix);
void board_spi_init(HAL_SPI_ID_E spix, HAL_SPI_CFG_T *spi_cfg);

/**
 * @brief send one byte and recieve the response
*/
uint8_t board_spi_send_byte(HAL_SPI_ID_E spix, uint8_t byte);

void board_qspi_en_dis_cfg(HAL_SPI_ID_E spix, HAL_STATUS_E flag);
void board_qspi_wr_rd_enable(HAL_SPI_ID_E spix, HAL_QSPI_DIR_E dir);
#endif