/*
 * gpio.h
 *
 *  Created on: 13 janv. 2021
 *      Author: Nirgal
 */

#ifndef APPLI_GPIO_H_
#define APPLI_GPIO_H_

#include "../config.h"
#include "modules/nrfx/hal/nrf_gpio.h"

void GPIO_init(void);

void GPIO_configure(uint8_t pin, nrf_gpio_pin_pull_t pull, bool_e output);
void GPIO_configure_input_output_opendrain(int8_t pin, nrf_gpio_pin_pull_t pull);

__inline void GPIO_write(uint8_t pin, bool_e value)
{
	if(value)
		NRF_P0->OUTSET = (1 << (pin));
	else
		NRF_P0->OUTCLR = (1 << (pin));
}


__inline bool_e GPIO_read(uint8_t pin)
{
	return (NRF_P0->IN >> pin)&1;
}



#endif /* APPLI_GPIO_H_ */
