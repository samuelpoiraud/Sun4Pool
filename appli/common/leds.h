/*
 * leds.h
 *
 *  Created on: 28 janv. 2021
 *      Author: Nirgal
 */

#ifndef APPLI_COMMON_LEDS_H_
#define APPLI_COMMON_LEDS_H_

typedef enum{
	 LED_MODE_OFF,
	 LED_MODE_ON,
	 LED_MODE_BLINK,
	 LED_MODE_FLASH,
	 LED_MODE_FLASH_LIMITED_NUMBER
	 //TODO
}led_mode_e;

typedef enum{
	 LED_ID_NETWORK = 0,
	 LED_ID_BATTERY,
	 LED_ID_USER0,
	 LED_ID_USER1,
	 LED_ID_NB		//nombre max de leds
}led_id_e;


void LEDS_init(bool_e i_have_led_battery);


void LED_add(led_id_e id, uint8_t pin);

void LED_set(led_id_e id, led_mode_e mode);

void LED_process_ms(void);

void LED_toggle(led_id_e id);

led_mode_e LED_get(led_id_e id);


#endif /* APPLI_COMMON_LEDS_H_ */

