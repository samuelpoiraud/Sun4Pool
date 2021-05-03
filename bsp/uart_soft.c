/*
 * uart_soft.c
 *
 *  Created on: 22 févr. 2021
 *      Author: Nirgal
 */

#include "uart_soft.h"
#if USE_UART_SOFT
#define MAX_UART_NB	3


#include "appli/common/gpio.h"
#include "appli/common/systick.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
	uint8_t tx_pins;
	bool_e initialized;
}UART_SOFT_handler_t;


extern void UART_SOFT_asm_putc(uint8_t c, uint32_t pin, uint32_t * clear_register, uint32_t * set_register);

static UART_SOFT_handler_t handler[MAX_UART_NB];
static uint8_t default_uart_for_printf = 0;

void UART_SOFT_init(uint8_t id, uint8_t * pins, uint8_t pin_nb)
{
	uint8_t i;
	for(i=0; i<pin_nb; i++)
	{
		handler[i].tx_pins = pins[i];
		handler[i].initialized = TRUE;
		GPIO_configure(pins[i], GPIO_PIN_CNF_PULL_Disabled, TRUE);
	}
	for(; i<MAX_UART_NB; i++)
	{
		handler[i].tx_pins = 0;
		handler[i].initialized = FALSE;
	}
}

void UART_SOFT_set_default_uart_for_printf(uint8_t id)
{
	if(handler[id].initialized)
		default_uart_for_printf = id;
}


void UART_SOFT_putc(uint8_t id, uint8_t c)
{
	if(handler[id].initialized)
	{
		uint32_t pin = 1UL<<(handler[id].tx_pins);
		uint32_t * pclr;
		uint32_t * pset;
		pclr = (uint32_t *)&NRF_P0->OUTCLR;
		pset = (uint32_t *)&NRF_P0->OUTSET;

		//////////////////
		//Section critique
		__disable_irq();

		UART_SOFT_asm_putc(c, pin, pclr, pset);

		__enable_irq();
		//Fin de section critique
		//////////////////
	}
}


void UART_SOFT_puts(uint8_t id, char * s)
{
	if(handler[id].initialized)
	{
		uint32_t pin = 1UL<<(handler[id].tx_pins);
		uint32_t * pclr;
		uint32_t * pset;
		pclr = (uint32_t *)&NRF_P0->OUTCLR;
		pset = (uint32_t *)&NRF_P0->OUTSET;
		for(uint16_t i = 0; s[i]; i++)
		{
			//////////////////
			//Section critique
			__disable_irq();

			UART_SOFT_asm_putc(s[i], pin, pclr, pset);

			__enable_irq();
			//Fin de section critique
			//////////////////
		}
	}
}

void UART_SOFT_printf(const char *format, ...)
{
	char buffer[256];
	va_list args_list;
	va_start(args_list, format);
	vsnprintf(buffer, 256, format, args_list);
	va_end(args_list);
	UART_SOFT_puts(default_uart_for_printf, buffer);
}

void UART_SOFT_test(void)
{
	uint8_t pin = 19;
	UART_SOFT_init(0, &pin, 1);
	UART_SOFT_set_default_uart_for_printf(0);
	while(1)
	{
		UART_SOFT_putc(0, 0x55);
		SYSTICK_delay_ms(1);
		UART_SOFT_puts(0, "! This text is not sent by a peripheral, but full managed by CPU and asm software\n");
		SYSTICK_delay_ms(100);
		UART_SOFT_printf("And there is %d specific printf function available\n", 1);

		SYSTICK_delay_ms(100);
		SYSTICK_delay_ms(100);
	}
}

#endif

