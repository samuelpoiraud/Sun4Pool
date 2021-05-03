/*
 * nrf52_spi.c
 *
 *  Created on: 18 févr. 2021
 *      Author: Nirgal
 */

#include "appli/config.h"
#include "nrf52_spi.h"

#if USE_SPI

static const nrfx_spi_t spi = NRFX_SPI_INSTANCE(SPI_INSTANCE);

void SPI_init(void)
{
	nrfx_spi_config_t spi_config = NRFX_SPI_DEFAULT_CONFIG;

	spi_config.sck_pin  = SPI_SCK_PIN;
	spi_config.miso_pin = SPI_MISO_PIN;
	spi_config.mosi_pin = SPI_MOSI_PIN;
	spi_config.ss_pin   = NRFX_SPI_PIN_NOT_USED;

	nrfx_spi_init(&spi, &spi_config, NULL, NULL);
}

void SPI_uninit(void)
{
	nrfx_spi_uninit(&spi);
}


void SPI_setFrequency(nrf_spi_frequency_t frequency)
{
	nrf_spi_frequency_set(spi.p_reg, frequency);
}

void SPI_write(const void * data, size_t size)
{
	nrfx_spi_xfer_desc_t desc = {
	.p_tx_buffer = data,
	.tx_length = size,
	.p_rx_buffer = NULL,
	.rx_length = 0
	};
    APP_ERROR_CHECK(nrfx_spi_xfer(&spi, &desc, 0));
}


void SPI_read(uint8_t * data, size_t size)
{
	uint8_t data_to_send[size];
	for(uint8_t i = 0; i<size; i++)
		data_to_send[i] = 0x00;

	nrfx_spi_xfer_desc_t desc = {
	.p_tx_buffer = data_to_send,
	.tx_length = size,
	.p_rx_buffer = data,
	.rx_length = size
	};

	nrfx_spi_xfer(&spi, &desc, 0);
}

void SPI_xfer(const void * datawrite, size_t sizewrite, void * dataread, size_t sizeread)
{
	nrfx_spi_xfer_desc_t desc = {
		.p_tx_buffer = datawrite,
		.tx_length = sizewrite,
		.p_rx_buffer = dataread,
		.rx_length = sizeread
	};
    APP_ERROR_CHECK(nrfx_spi_xfer(&spi, &desc, 0));
}

#endif
