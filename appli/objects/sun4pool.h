/*
 * sun4pool.h
 *
 *  Created on: 1 mai 2021
 *      Author: Nirgal
 */

#ifndef APPLI_OBJECTS_SUN4POOL_H_
#define APPLI_OBJECTS_SUN4POOL_H_


void SUN4POOL_state_machine(void);

void SUN4POOL_process_ms(void);


typedef enum
{
	RELAY_FILTER_PUMP,
	RELAY_SOLAR_HEATER,
	RELAY_NB
}relay_e;


#endif /* APPLI_OBJECTS_SUN4POOL_H_ */
