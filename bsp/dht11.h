/*
 * 	dht11.h
 *
 *  Created on: 02 fevr. 2021
 *      Author: Thibault.R
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "../appli/config.h"

#if USE_DHT11

#include <stdint.h>

extern uint8_t humidity_int;
extern uint8_t humidity_dec;
extern uint8_t temperature_int;
extern uint8_t temperature_dec;

void DHT11_demo(void);

void DHT11_main(void);

void DHT11_init(uint16_t GPIO_PIN_x);

running_e DHT11_state_machine_get_datas(uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec);

#endif /* DHT11_H_ */
#endif
