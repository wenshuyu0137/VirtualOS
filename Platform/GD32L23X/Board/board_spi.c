#include "board_spi.h"
#include "board_wwdt.h"

uint32_t get_spi_rcu(HAL_SPI_ID_E spix)
{
	return (spix == HAL_SPI_0 ? RCU_SPI0 : RCU_SPI1);
}

uint32_t get_spi(HAL_SPI_ID_E spix)
{
	uint32_t m_spi;

	switch (spix) {
	case HAL_SPI_0:
		m_spi = SPI0;
		break;

	case HAL_SPI_1:
		m_spi = SPI1;
		break;

	case HAL_SPI_2:
		m_spi = 0xffffffff;
		break;

	default:
		m_spi = 0xffffffff;
		break;
	}

	return m_spi;
}

uint32_t get_pol_pha(HAL_SPI_CFG_T *spi_cfg)
{
	uint32_t val;

	switch (spi_cfg->polarity_and_phase) {
	case HAL_SPI_LOW_POL_LEFT_EDGE:
		val = SPI_CK_PL_LOW_PH_1EDGE;
		break;

	case HAL_SPI_HIGH_POL_LEFT_EDGE:
		val = SPI_CK_PL_HIGH_PH_1EDGE;
		break;

	case HAL_SPI_LOW_POL_RIGHT_EDGE:
		val = SPI_CK_PL_LOW_PH_2EDGE;
		break;

	case HAL_SPI_HIGH_POL_RIGHT_EDGE:
		val = SPI_CK_PL_HIGH_PH_2EDGE;
		break;

	default:
		val = 0xffffffff;
		break;
	}

	return val;
}

uint32_t get_deviece_mode(HAL_SPI_CFG_T *spi_cfg)
{
	return (spi_cfg->device_mode == HAL_SPI_MASTER ? SPI_MASTER : SPI_SLAVE);
}

uint32_t get_endian(HAL_SPI_CFG_T *spi_cfg)
{
	return (spi_cfg->endian == HAL_SPI_ENDIAN_LSB ? SPI_ENDIAN_LSB : SPI_ENDIAN_MSB);
}

uint32_t get_frame_size(HAL_SPI_CFG_T *spi_cfg)
{
	return CTL1_FRAMESIZE(spi_cfg->frame_size + 3);
}

uint32_t get_nss(HAL_SPI_CFG_T *spi_cfg)
{
	return (spi_cfg->nss_mode == HAL_NSS_SOFT ? SPI_NSS_SOFT : SPI_NSS_HARD);
}

uint32_t get_prescal(HAL_SPI_CFG_T *spi_cfg)
{
	return CTL0_PSC(spi_cfg->spi_prescal);
}

uint32_t get_trans_mode(HAL_SPI_CFG_T *spi_cfg)
{
	uint32_t mode;

	switch (spi_cfg->trans_mode) {
	case HAL_SPI_MODE_FULLDOPLEX:
		mode = SPI_TRANSMODE_FULLDUPLEX;
		break;

	case HAL_SPI_MODE_RCV_ONLY:
		mode = SPI_TRANSMODE_RECEIVEONLY;
		break;

	case HAL_SPI_MODE_BI_RCV:
		mode = SPI_TRANSMODE_BDRECEIVE;
		break;

	case HAL_SPI_MODE_BI_TRANSMIT:
		mode = SPI_TRANSMODE_BDTRANSMIT;
		break;

	default:
		mode = 0xffffffff;
		break;
	}

	return mode;
}

uint32_t get_fifo_access(HAL_SPI_CFG_T *spi_cfg)
{
	return (spi_cfg->rx_fifo_access == HAL_SPI_FIFO_SIZE_BYTE ? SPI_BYTE_ACCESS : SPI_HALFWORD_ACCESS);
}

void board_spi_deinit(HAL_SPI_ID_E spix)
{
	spi_disable(get_spi(spix));
	spi_i2s_deinit(get_spi(spix));
	rcu_periph_clock_disable(get_spi_rcu(spix));
}

void board_spi_init(HAL_SPI_ID_E spix, HAL_SPI_CFG_T *spi_cfg)
{
	spi_parameter_struct spi_param;
	rcu_periph_clock_enable(get_spi_rcu(spix));
	spi_param.clock_polarity_phase = get_pol_pha(spi_cfg);
	spi_param.device_mode = get_deviece_mode(spi_cfg);
	spi_param.endian = get_endian(spi_cfg);
	spi_param.frame_size = get_frame_size(spi_cfg);
	spi_param.nss = get_nss(spi_cfg);
	spi_param.prescale = get_prescal(spi_cfg);
	spi_param.trans_mode = get_trans_mode(spi_cfg);
	spi_init(get_spi(spix), &spi_param);

	if (spix == HAL_SPI_0) {
		spi_fifo_access_size_config(get_spi(spix), get_fifo_access(spi_cfg));
	}

	if (spi_cfg->use_crc) {
		spi_crc_polynomial_set(get_spi(spix), 7); //默认7
	}

	if (spi_cfg->use_qspi && (spix == HAL_SPI_0)) {
		spi_quad_io23_output_enable(get_spi(spix));
	}

	spi_enable(get_spi(spix));
}

uint8_t board_spi_send_byte(HAL_SPI_ID_E spix, uint8_t byte)
{
	while (RESET == spi_i2s_flag_get(get_spi(spix), SPI_FLAG_TBE))
		;

	spi_i2s_data_transmit(get_spi(spix), byte);

	while (RESET == spi_i2s_flag_get(get_spi(spix), SPI_FLAG_RBNE))
		;

	return (spi_i2s_data_receive(get_spi(spix)));
}

void board_qspi_en_dis_cfg(HAL_SPI_ID_E spix, HAL_STATUS_E flag)
{
	if (flag) {
		spi_quad_enable(get_spi(spix));
	}

	else {
		spi_quad_disable(get_spi(spix));
	}
}

void board_qspi_wr_rd_enable(HAL_SPI_ID_E spix, HAL_QSPI_DIR_E dir)
{
	if (dir == HAL_QSPI_DIR_WR) {
		spi_quad_write_enable(get_spi(spix));
	}

	else {
		spi_quad_read_enable(get_spi(spix));
	}
}