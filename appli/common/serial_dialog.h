/*
 * serial_dialog.h
 *
 *  Created on: 8 oct. 2018
 *      Author: Nirgal
 */

#ifndef BURGER_DIALOG_H_
#define BURGER_DIALOG_H_

#include <stdint.h>


#define SOH		0xBA		//Start Of Header
#define EOT		0xDA		//End Of Transmission

void SERIAL_DIALOG_init(void);
void SERIAL_DIALOG_puts(char * s);

void SERIAL_DIALOG_process_main(void);
void SERIAL_DIALOG_send_msg(uint8_t size, uint8_t * datas);

#endif /* BURGER_DIALOG_H_ */
