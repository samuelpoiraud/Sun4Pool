/*
 * nmos_gnd.c
 *
 *  Created on: 12 fevr. 2021
 *      Author: Thibault.R
 */
#include "../appli/config.h"

#if USE_NMOS_GND

#include "nmos_gnd.h"
#include "../appli/common/gpio.h"

void NMOS_On(void)
{
	GPIO_configure(MOSFET_GND, NRF_GPIO_PIN_NOPULL, TRUE);
	GPIO_write(MOSFET_GND, TRUE);
}

void NMOS_Off(void)
{
	GPIO_write(MOSFET_GND, FALSE);
}


#endif
