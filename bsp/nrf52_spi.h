/*
 * nrf52_spi.h
 *
 *  Created on: 18 févr. 2021
 *      Author: Nirgal
 */

#ifndef BSP_NRF52_SPI_H_
#define BSP_NRF52_SPI_H_

#include "nrfx_spi.h"
#include "appli/config.h"
#if USE_SPI

	void SPI_init(void);

	void SPI_uninit(void);

	void SPI_setFrequency(nrf_spi_frequency_t frequency);

	void SPI_write(const void * data, size_t size);

	void SPI_read(uint8_t * data, size_t size);

	void SPI_xfer(const void * datawrite, size_t sizewrite, void * dataread, size_t sizeread);

#endif

#endif /* BSP_NRF52_SPI_H_ */
