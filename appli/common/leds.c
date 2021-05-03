/*
 * leds.C
 *
 *  Created on: 28 janv. 2021
 *      Author: Nirgal
 */
#include "../config.h"
#include "leds.h"
#include "systick.h"
#include "gpio.h"

typedef struct
{
	bool_e initialized;
	uint8_t pin;
	led_mode_e mode;
	uint32_t period;
	uint32_t current_time;
	uint32_t on_time;
	uint32_t nb_fash_remaining;
}leds_t;

static leds_t leds[LED_ID_NB];

void LEDS_init(bool_e i_have_led_battery)
{
	for(uint8_t i = 0; i<LED_ID_NB; i++)
		leds[i].initialized = FALSE;			//nettoyage du tableau.

	//TODO   //ajoute par défaut la led network + led battery si indiqué

	Systick_add_callback_function(&LED_process_ms);
	//TODO déclare au systick la fonction LED_process_ms() qui sera désormais appelée à chaque ms.
}


void LED_add(led_id_e id, uint8_t pin)
{
	leds[id].pin = pin;

	GPIO_init();
	GPIO_configure(pin, GPIO_PIN_CNF_PULL_Pullup, true);//configure la pin de la led concernée en sortie
	leds[id].initialized = true;//enregistre la led comme "initialisée"
}

void LED_set(led_id_e id, led_mode_e mode)
{
	leds[id].mode = mode;
	leds[id].current_time = 0;
	leds[id].period = 0;
	leds[id].nb_fash_remaining = 0;
	if(mode == LED_MODE_ON){
		GPIO_write(leds[id].pin, true);
	}else if (mode == LED_MODE_OFF){
		GPIO_write(leds[id].pin, false);
	}else if(mode == LED_MODE_BLINK)
	{
		leds[id].period = 1000;
		leds[id].on_time = 500;
		GPIO_write(leds[id].pin, true);
	}else if(mode == LED_MODE_FLASH)
	{
		leds[id].period = 1000;
		leds[id].on_time = 10;
		GPIO_write(leds[id].pin, true);
	}
}

void LED_toggle(led_id_e id)
{
	if(leds[id].mode == LED_MODE_OFF)
		LED_set(id, LED_MODE_ON);
	else
		LED_set(id, LED_MODE_OFF);
}


led_mode_e LED_get(led_id_e id)
{
	return leds[id].mode;
}

void LED_set_flash_limited_nb(led_id_e id, uint32_t  nb_flash, uint32_t period)
{
	leds[id].mode = LED_MODE_FLASH_LIMITED_NUMBER;
	leds[id].current_time = 0;
	leds[id].period = period;
	leds[id].on_time = 10;
	if(nb_flash)
	{
		GPIO_write(leds[id].pin, true);
		leds[id].nb_fash_remaining = nb_flash - 1;
	}
}

void LED_process_ms(void)
{
	for(uint8_t id = 0; id<LED_ID_NB; id++)
	{
		led_mode_e mode;
		mode = leds[id].mode;
		if(mode == LED_MODE_BLINK || mode == LED_MODE_FLASH || mode == LED_MODE_FLASH_LIMITED_NUMBER)
		{
			if(leds[id].current_time >= leds[id].period)
			{
				leds[id].current_time = 0;
				if(mode == LED_MODE_FLASH_LIMITED_NUMBER)
				{
					if(leds[id].nb_fash_remaining)
					{
						leds[id].nb_fash_remaining--;
						GPIO_write(leds[id].pin, true);
					}
				}
				else
					GPIO_write(leds[id].pin, true);
			}
			else
			{
				if(leds[id].current_time == leds[id].on_time)
					GPIO_write(leds[id].pin, false);

				leds[id].current_time++;
			}
		}
	}
}

