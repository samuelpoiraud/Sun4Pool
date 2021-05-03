/*
 * gpio.c
 *
 *  Created on: 13 janv. 2021
 *      Author: Nirgal
 */
#include "gpio.h"

#include "nrf52.h"

static void GPIO_configure_pins_9_and_10_as_gpio(void);

void GPIO_init(void)
{

	//Spécificité sur pins 9 et 10 qui ne sont pas par défaut utilisables en tant que GPIO.
	GPIO_configure_pins_9_and_10_as_gpio();
}

void GPIO_configure(uint8_t pin, nrf_gpio_pin_pull_t pull, bool_e output)
{
	if(output)
		nrf_gpio_cfg_output(pin);
	else
		nrf_gpio_cfg_input(pin, pull);
}

void GPIO_configure_input_output_opendrain(int8_t pin, nrf_gpio_pin_pull_t pull)
{
	 nrf_gpio_cfg(
	        pin,
	        NRF_GPIO_PIN_DIR_OUTPUT,
	        NRF_GPIO_PIN_INPUT_CONNECT,
	        pull,
	        NRF_GPIO_PIN_S0D1,
	        NRF_GPIO_PIN_NOSENSE);
}


static void GPIO_configure_pins_9_and_10_as_gpio(void)
{
	if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)){
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NVIC_SystemReset();
	}
}


