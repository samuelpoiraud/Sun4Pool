/*
 * pwm.c
 *
 *  Created on: 22/02/2021
 *      Author: Nirgal
 */


#include "pwm.h"
#include "sdk_config.h"
#include "nrf_gpio.h"
#include "nrfx_config.h"
#include "nrfx_timer.h"

#if USE_PWM

#define MAX_PIN_NB	8
static uint8_t pins_numbers[MAX_PIN_NB];
static uint8_t initialized_pins_nb = 0;

static nrfx_pwm_t instance_pwm_0123 = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_t instance_pwm_4567 = NRFX_PWM_INSTANCE(1);
static nrf_pwm_sequence_t sequence_0123;
static nrf_pwm_sequence_t sequence_4567;
static nrf_pwm_values_individual_t duties_0123[] = {0};
static nrf_pwm_values_individual_t duties_4567[] = {0};

/*
 * Cette fonction initialise jusqu'à 8 PWMs sur deux périphériques différents.
 * pins : tableau, de 'pin_nb' cases, contenant les numéros des pins concernées
 * pin_nb : nombre de pins dans le tableau pins, de 1 à 8.
 * base clock : horloge utilisée par le périphérique PWM. Défaut si 0 : 16MHz
 * top_value : définie la période de la PWM (considérant 'top_value' événements à la base_clock choisie). Defaut si 0 : 1000.
 */
void PWM_init(uint8_t * pins, uint8_t pin_nb, nrf_pwm_clk_t base_clock, uint16_t top_value)
{
	nrfx_pwm_config_t pwm_config = (nrfx_pwm_config_t){                                                                          \
			.output_pins  = { 	(pin_nb>0)?pins[0]:NRFX_PWM_PIN_NOT_USED,
								(pin_nb>1)?pins[1]:NRFX_PWM_PIN_NOT_USED,
								(pin_nb>2)?pins[2]:NRFX_PWM_PIN_NOT_USED,
								(pin_nb>3)?pins[3]:NRFX_PWM_PIN_NOT_USED },
			.irq_priority = 2,
			.base_clock   = (base_clock==0)?NRF_PWM_CLK_16MHz:base_clock,
			.count_mode   = (nrf_pwm_mode_t)NRF_PWM_MODE_UP_AND_DOWN,
			.top_value    = (top_value==0)?1000:top_value,
			.load_mode    = (nrf_pwm_dec_load_t)NRF_PWM_LOAD_INDIVIDUAL,
			.step_mode    = (nrf_pwm_dec_step_t)NRF_PWM_STEP_TRIGGERED//NRF_PWM_STEP_AUTO
		};


	nrfx_pwm_init(&instance_pwm_0123, &pwm_config, NULL);

	for(uint8_t i = 0; i<4; i++)
		pwm_config.output_pins[i] = (pin_nb>4+i)?pins[4+i]:NRFX_PWM_PIN_NOT_USED;

	nrfx_pwm_init(&instance_pwm_4567, &pwm_config, NULL);

	duties_0123[0].channel_0 = 0;
	duties_0123[0].channel_1 = 0;
	duties_0123[0].channel_2 = 0;
	duties_0123[0].channel_3 = 0;

	duties_4567[0].channel_0 = 0;
	duties_4567[0].channel_1 = 0;
	duties_4567[0].channel_2 = 0;
	duties_4567[0].channel_3 = 0;

	sequence_0123.values.p_individual = duties_0123;
	sequence_0123.length = NRF_PWM_VALUES_LENGTH(duties_0123);
	sequence_0123.repeats = 0;
	sequence_0123.end_delay = 0;

	sequence_4567.values.p_individual = duties_4567;
	sequence_4567.length = NRF_PWM_VALUES_LENGTH(duties_4567);
	sequence_4567.repeats = 0;
	sequence_4567.end_delay = 0;

	nrfx_pwm_simple_playback(&instance_pwm_0123, &sequence_0123, 1, NRFX_PWM_FLAG_LOOP);
	nrfx_pwm_simple_playback(&instance_pwm_4567, &sequence_4567, 1, NRFX_PWM_FLAG_LOOP);

	//initialisation des broches en sortie.
	initialized_pins_nb = MIN(pin_nb, 8);
	for(uint8_t i = 0; i<initialized_pins_nb; i++)
	{
		pins_numbers[i] = pins[i];
		nrf_gpio_cfg_output(pins[i]);
	}
}


void PWM_set_duty(uint8_t pin_id, uint16_t duty)
{
	if(pin_id < initialized_pins_nb)
	{
		switch(pin_id)
		{
			case 0: duties_0123[0].channel_0 = duty;	break;
			case 1: duties_0123[0].channel_1 = duty;	break;
			case 2: duties_0123[0].channel_2 = duty;	break;
			case 3: duties_0123[0].channel_3 = duty;	break;
			case 4: duties_4567[0].channel_0 = duty;	break;
			case 5:	duties_4567[0].channel_1 = duty;	break;
			case 6: duties_4567[0].channel_2 = duty;	break;
			case 7: duties_4567[0].channel_3 = duty;	break;
			default:
				break;
		}
		if(pin_id<4)
			nrfx_pwm_simple_playback(&instance_pwm_0123, &sequence_0123, 1, NRFX_PWM_FLAG_LOOP);
		else
			nrfx_pwm_simple_playback(&instance_pwm_4567, &sequence_4567, 1, NRFX_PWM_FLAG_LOOP);
	}
}

#endif
