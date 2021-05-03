/*
 * dht11.c
 *
 *  Created on: 10 fevr. 2021
 *      Author: Thibault.R
 */
#include "../appli/config.h"

#if USE_DHT11

#include "dht11.h"
#include "nmos_gnd.h"
#include <stdbool.h>
#include "../appli/common/gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "../appli/common/leds.h"
#include "../appli/common/macro_types.h"
#include "../appli/common/systick.h"

/*
https://www.mouser.com/ds/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf

La ligne bidirectionelle de "data" du DHT11 doit être tirée au Vdd par une résistance de 4,7k.
Vdd doit être entre 3 et 5V.

Etant donné que le STM32F103 est alimenté en 3,3V, il est recommandé d'alimenter ce capteur en 3,3V.
On peut toutefois aller au delà, mais il faut s'assurer que la broche utilisée pour le dialogue soit tolérante 5V en entrée.


*/
static bool_e compute_frame(uint64_t datas, uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec);
static void DHT11_callback_exti(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);


static uint16_t DHT11_pin;
static bool_e initialized = FALSE;

#define NB_BITS	42	//les deux bits de poids fort n'appartiennent pas aux données utiles. (il s'agit de la réponse du capteur avant la trame utile).
static volatile bool_e flag_end_of_reception = FALSE;
static volatile uint64_t trame;
static volatile uint8_t index = 0;
static volatile uint32_t t = 0;

uint8_t humidity_int;
uint8_t humidity_dec;
uint8_t temperature_int;
uint8_t temperature_dec;

void DHT11_set_pin_direction(bool_e out)
{
	if(out)
	{
		nrf_drv_gpiote_in_uninit(DHT11_pin);
		nrf_drv_gpiote_in_event_disable(DHT11_pin);
		nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
		nrf_drv_gpiote_out_init(DHT11_pin, &out_config);
	}
	else
	{
		nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
		in_config.pull = NRF_GPIO_PIN_PULLUP;
		nrf_drv_gpiote_in_init(DHT11_pin, &in_config, &DHT11_callback_exti);
		nrf_drv_gpiote_in_event_enable(DHT11_pin, true);
	}
}

void DHT11_init(uint16_t GPIO_PIN_x)
{
	DHT11_pin = GPIO_PIN_x;

	nrf_drv_gpiote_init();

	GPIO_write(DHT11_pin, 1);
	DHT11_set_pin_direction(FALSE);

	initialized = TRUE;
}

//Fonction demo pour tester le DHT11
void DHT11_demo(void)
{

	DHT11_init(DHT11_PIN);
	while(1)
	{

		switch(DHT11_state_machine_get_datas(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec))
		{
			case END_OK:
 				debug_printf("DHT11 h=%d,%d | t=%d,%d\n",humidity_int, humidity_dec, temperature_int, temperature_dec);
 				LED_set(LED_ID_NETWORK, LED_MODE_ON);
 				SYSTICK_delay_ms(1000);
 				LED_set(LED_ID_NETWORK, LED_MODE_OFF);
				break;
			case END_ERROR:
				debug_printf("DHT11 read error (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				SYSTICK_delay_ms(1000);
				break;
			case END_TIMEOUT:
				debug_printf("DHT11 timeout (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				SYSTICK_delay_ms(1000);
				break;
			default:
				break;
		}
	}
}

//Fonction pour utiliser le DHT11 --> Vous devez declarer en EXTERN dans votre .h les 4 variables humidity/temperature int & dec
void DHT11_main(void)
{

	DHT11_init(DHT11_PIN);
	while(DHT11_state_machine_get_datas(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec) != END_OK)
	{
	}
}



static void DHT11_process_ms(void)
{
	if(t)
		t--;
}


static void DHT11_callback_exti(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	static uint32_t rising_time_us = 0;
	if(pin!=DHT11_pin)
		return;
	if(index < NB_BITS)
	{
		if(GPIO_read(DHT11_pin))
		{
			rising_time_us = SYSTICK_get_time_us();	//on enregistre la date du front montant (en microsecondes)
		}
		else
		{
			uint32_t falling_time_us;
			falling_time_us = SYSTICK_get_time_us(); //on conserve la différence entre le front montant et le front descendant
			if(falling_time_us - rising_time_us > 50)
				trame |= (uint64_t)(1) << (NB_BITS - 1 - index);
			index++;
		}
	}

	if(index == NB_BITS)
	{
		flag_end_of_reception = TRUE;
	}
}
//Attention, fonction blocante pendant 4ms !
running_e DHT11_state_machine_get_datas(uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec)
{
	typedef enum
	{
		INIT,
		SEND_START_SIGNAL,
		WAIT_DHT_ANSWER,
		TIMEOUT,
		END_OF_RECEPTION,
		WAIT_BEFORE_NEXT_ASK
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (state != previous_state)?TRUE:FALSE;
	previous_state = state;
	running_e ret = IN_PROGRESS;


	switch(state)
	{
		case INIT:
			if(initialized)
			{
				state = SEND_START_SIGNAL;
				Systick_add_callback_function(&DHT11_process_ms);
			}
			else
			{
				debug_printf("You should call DHT11_init(...)\n");
				ret = END_ERROR;
			}
			break;

		case SEND_START_SIGNAL:
			if(entrance)
			{
				t = 20;
				index = 0;
				trame = 0;
				flag_end_of_reception = FALSE;
				DHT11_set_pin_direction(TRUE);	//configurer pin en sortie
				GPIO_write(DHT11_pin, 0);
			}
			if(!t)
			{
				GPIO_write(DHT11_pin, 1);
				DHT11_set_pin_direction(FALSE);	//configurer pin en entrée, avec détection it externe
				state = WAIT_DHT_ANSWER;
				//début de la surveillance des fronts
			}
			break;
		case WAIT_DHT_ANSWER:
			if(entrance)
			{
				t = 10;
			}
			if(flag_end_of_reception)
				state = END_OF_RECEPTION;
			if(!t)
				state = TIMEOUT;
			break;
		case TIMEOUT:
			ret = END_TIMEOUT;
			t = 100;
			state = WAIT_BEFORE_NEXT_ASK;
			break;
		case END_OF_RECEPTION:
			if(compute_frame(trame, humidity_int, humidity_dec, temperature_int, temperature_dec))
				ret = END_OK;
			else
				ret = END_ERROR;
			t = 1000;
			state = WAIT_BEFORE_NEXT_ASK;
			break;
		case WAIT_BEFORE_NEXT_ASK:
			if(!t)
				state = SEND_START_SIGNAL;
			break;
		default:
			break;

	}
	return ret;
}

static bool_e compute_frame(uint64_t datas, uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec)
{
	bool_e ret = FALSE;
	*humidity_int = (uint8_t)(datas >> 32);
	*humidity_dec = (uint8_t)(datas >> 24);
	*temperature_int = (uint8_t)(datas >> 16);
	*temperature_dec = (uint8_t)(datas >> 8);
	//debug_printf(" - %d+%d+%d+%d= %d (%d)", *humidity_int, *humidity_dec, *temperature_int, *temperature_dec, (uint8_t)(*humidity_int + *humidity_dec + *temperature_int + *temperature_dec), (uint8_t)(datas));
	//checksum
	if((uint8_t)(*humidity_int + *humidity_dec + *temperature_int + *temperature_dec) == (uint8_t)(datas))
		ret = TRUE;
	return ret;
}

#endif
