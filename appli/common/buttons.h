/*
 * buttons.h
 *
 *  Created on: 20 janv. 2021
 *      Author: Nirgal
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_
#include "../config.h"
#include "macro_types.h"


typedef enum
{
	BUTTON_NETWORK,
	BUTTON_USER0,
	BUTTON_USER1,
	BUTTON_ALERTE,
	BUTTON_NB	//Nombre max de boutons
}button_id_e;

typedef enum
{
	BUTTON_EVENT_NONE,
	BUTTON_PRESS_EVENT,
	BUTTON_RELEASE_EVENT
}button_event_e;

typedef enum
{
	INIT_BUTTON,
    IDLE_READING_BUTTON,
    BUTTON_WAIT_FOR_LONG_PRESS,
    WAIT_RELEASE_BUTTON,
    BUTTON_5_FAST_PRESS,
    POWERDOWN
}button_event_functions_e;


void BUTTONS_init(void);

void BUTTONS_process_main(void);

void BUTTONS_add(button_id_e id, uint8_t pin, bool_e pullup, callback_fun_t callback_short_press, callback_fun_t callback_short_release, callback_fun_t callback_long_press, callback_fun_t callback_long_release);


void BUTTONS_set_short_press_callback(button_id_e id, callback_fun_t callback);

void BUTTONS_set_short_release_callback(button_id_e id, callback_fun_t callback);

void BUTTONS_set_long_press_callback(button_id_e id, callback_fun_t callback);

void BUTTONS_set_long_release_callback(button_id_e id, callback_fun_t callback);

void BUTTONS_process_ms(void);

void BUTTONS_get_event(button_event_e * event, button_id_e * button);

bool_e BUTTONS_read(button_id_e id);

void BUTTONS_network_process(void);

void BUTTONS_network_test(void);


#endif /* BUTTONS_H_ */
