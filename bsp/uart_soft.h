/*
 * uart_soft.h
 *
 *  Created on: 22 févr. 2021
 *      Author: Nirgal
 */

#ifndef BSP_UART_SOFT_H_
#define BSP_UART_SOFT_H_

#include "appli/config.h"

	#if USE_UART_SOFT

		void UART_SOFT_init(uint8_t id, uint8_t * pins, uint8_t pin_nb);

		void UART_SOFT_set_default_uart_for_printf(uint8_t id);

		void UART_SOFT_putc(uint8_t id, uint8_t c);

		void UART_SOFT_puts(uint8_t id, char * s);

		void UART_SOFT_printf(const char *format, ...);

		void UART_SOFT_test(void);

	#endif

#endif /* BSP_UART_SOFT_H_ */
