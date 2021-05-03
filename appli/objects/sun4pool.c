/*
 * sun4pool.c
 *
 *  Created on: 1 mai 2021
 *      Author: Nirgal
 */
#include "../config.h"
#include "sun4pool.h"
#include "../common/systick.h"
#include "../common/gpio.h"
#include "../common/parameters.h"
#include "../../bsp/DS18B20.h"


#if OBJECT_ID == OBJECT_SUN4POOL
#define DEFAULT_POOR_RTC_VALUE_AT_RESET	(9*60*60*1000)	//il est 9h du mat quand on allume... choix arbitraire...
#define DEFAULT_NIGHT_HOUR						(21*60*60*1000)	//à 21h le soir ; on entre dans la nuit
#define DEFAULT_DAY_HOUR						(8*60*60*1000)	//à 8h le matin ; on sort de la nuit

#define DEFAULT_FILTER_PUMP_DURATION  	(1000*60*60)	//1h de pompage par défaut.
#define DEFAULT_FILTER_PUMP_PERIOD		(4*1000*60*60)	//4h  entre deux pompages.
#define DEFAULT_SOLAR_HEATER_PUMP_DURATION  	(70*1000)	//70s de pompage par défaut.
#define DEFAULT_SOLAR_HEATER_PUMP_PERIOD		(15*60*1000)	//15mn entre deux pompages.

#define PERIOD_READ_TEMPERATURE			(1000)

static uint32_t poor_rtc = DEFAULT_POOR_RTC_VALUE_AT_RESET;

static volatile uint32_t remaining_time_before_turn_off_pump[RELAY_NB];
static volatile uint32_t t = 0;
static volatile uint32_t t_filter_pump = 0;
static volatile uint32_t t_solar_heater_pump = 0;
static volatile bool_e night = FALSE;


static void SUN4POOL_set_hour_callback(int32_t new_value);
static void SUN4POOL_set_relay(relay_e relay, bool_e state, uint32_t duration);
static void SUN4POOL_state_machine_filter_pump(void);
static void SUN4POOL_state_machine_solar_heater(void);

static void SUN4POOL_init(void)
{
	for(relay_e r = 0; r<RELAY_NB; r++)
		remaining_time_before_turn_off_pump[r] = 0;
}


void SUN4POOL_state_machine(void)
{
	typedef enum
	{
		INIT,
		RUN
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (state!=previous_state)?TRUE:FALSE;
	previous_state = state;

	switch(state)
	{
		case INIT:
			SUN4POOL_init();
			Systick_add_callback_function(&SUN4POOL_process_ms);
			PARAMETERS_init();
			PARAMETERS_enable(PARAM_MS_SINCE_MIDNIGHT, DEFAULT_POOR_RTC_VALUE_AT_RESET, FALSE, &SUN4POOL_set_hour_callback);
			PARAMETERS_enable(PARAM_FILTER_PUMP_DURATION, DEFAULT_FILTER_PUMP_DURATION, TRUE, NULL);
			PARAMETERS_enable(PARAM_FILTER_PUMP_PERIOD, DEFAULT_FILTER_PUMP_PERIOD, TRUE, NULL);
			PARAMETERS_enable(PARAM_SOLAR_HEATER_PUMP_DURATION, DEFAULT_SOLAR_HEATER_PUMP_DURATION, TRUE, NULL);
			PARAMETERS_enable(PARAM_SOLAR_HEATER_PUMP_PERIOD, DEFAULT_SOLAR_HEATER_PUMP_PERIOD, TRUE, NULL);
			PARAMETERS_enable(PARAM_DAY_HOUR, DEFAULT_DAY_HOUR, TRUE, NULL);
			PARAMETERS_enable(PARAM_NIGHT_HOUR, DEFAULT_NIGHT_HOUR, TRUE, NULL);

			state = RUN;
			break;
		case RUN:
			SUN4POOL_state_machine_filter_pump();
			SUN4POOL_state_machine_solar_heater();
			if(!t)
			{
				t = PERIOD_READ_TEMPERATURE;
				int16_t temperature = DS18B20_get_temperature();
				debug_printf("%d°C\n",temperature);
			}
			break;
		default:
			break;
	}
}


static void SUN4POOL_set_hour_callback(int32_t new_value)
{
	poor_rtc = new_value;
	night = ((new_value < PARAMETERS_get(PARAM_DAY_HOUR)) || (new_value > PARAMETERS_get(PARAM_NIGHT_HOUR)))?TRUE:FALSE;
}


void SUN4POOL_process_ms(void)
{
	//la RTC du pauvre : un compteur qui compte !
	poor_rtc++;
	if(poor_rtc >= 24*60*60*1000)
		poor_rtc = 0;
	if(!night && poor_rtc == PARAMETERS_get(PARAM_NIGHT_HOUR))
		night = TRUE;
	else if(night && poor_rtc == PARAMETERS_get(PARAM_DAY_HOUR))
		night = FALSE;
	PARAMETERS_set(PARAM_MS_SINCE_MIDNIGHT, poor_rtc);

	for(relay_e r = 0; r<RELAY_NB; r++)
	{
		if(remaining_time_before_turn_off_pump[r])
		{
			remaining_time_before_turn_off_pump[r]--;
			if(!remaining_time_before_turn_off_pump[r])
			{
				SUN4POOL_set_relay(r, FALSE, 0);	//on éteint la pompe !
			}
		}
	}

	if(t)
		t--;
	if(t_filter_pump)
		t_filter_pump--;
	if(t_solar_heater_pump)
		t_solar_heater_pump--;
}



static void SUN4POOL_state_machine_filter_pump(void)
{
	typedef enum
	{
		INIT,
		WAIT_2S,
		RUN_FILTER,
		NIGHT_MODE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (state!=previous_state)?TRUE:FALSE;
	previous_state = state;

	switch(state)
	{
		case INIT:

			state = WAIT_2S;
			break;
		case WAIT_2S:
			if(entrance)
				t_filter_pump = 2000;
			if(!t_filter_pump)
				state = RUN_FILTER;
			break;
		case RUN_FILTER:
			if(entrance)
				t_filter_pump = 0;

			if(!t_filter_pump)
			{
				t_filter_pump = PARAMETERS_get(PARAM_FILTER_PUMP_PERIOD);
				SUN4POOL_set_relay(RELAY_FILTER_PUMP, TRUE, PARAMETERS_get(PARAM_FILTER_PUMP_DURATION));
			}

			if(night)
				state = NIGHT_MODE;
			break;
		case NIGHT_MODE:
			if(!night)
				state = RUN_FILTER;
			break;
		default:
			break;
	}
}



static void SUN4POOL_state_machine_solar_heater(void)
{
	typedef enum
	{
		INIT,
		WAIT_1S,
		RUN_SOLAR_HEATER,
		NIGHT_MODE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (state!=previous_state)?TRUE:FALSE;
	previous_state = state;

	switch(state)
	{
		case INIT:

			state = WAIT_1S;
			break;
		case WAIT_1S:
			if(entrance)
				t_solar_heater_pump = 1000;
			if(!t_solar_heater_pump)
				state = RUN_SOLAR_HEATER;
			break;
		case RUN_SOLAR_HEATER:
			if(entrance)
				t_solar_heater_pump = 0;

			if(!t_solar_heater_pump)
			{
				t_solar_heater_pump = PARAMETERS_get(PARAM_SOLAR_HEATER_PUMP_PERIOD);
				SUN4POOL_set_relay(RELAY_SOLAR_HEATER, TRUE, PARAMETERS_get(PARAM_SOLAR_HEATER_PUMP_DURATION));
			}
			if(night)
				state = NIGHT_MODE;
		case NIGHT_MODE:
			if(!night)
				state = RUN_SOLAR_HEATER;
			break;
		default:
			break;
	}
}


//fonction privée !
static void SUN4POOL_set_relay(relay_e relay, bool_e state, uint32_t duration)
{
	static bool_e initialized = FALSE;
	if(!initialized)
	{
		GPIO_configure(GPIO_RELAY_FILTER_PUMP, GPIO_PIN_CNF_PULL_Disabled, TRUE);
		GPIO_configure(GPIO_RELAY_SOLAR_HEATER, GPIO_PIN_CNF_PULL_Disabled, TRUE);
		initialized = TRUE;
	}
	switch(relay)
	{
		case RELAY_FILTER_PUMP:
			GPIO_write(GPIO_RELAY_FILTER_PUMP, state);
			break;
		case RELAY_SOLAR_HEATER:
			GPIO_write(GPIO_RELAY_SOLAR_HEATER, state);
			break;
		default:
			break;
	}
	if(relay < RELAY_NB)
	{
		if(state && duration)		//si on vient d'allumer la pompe, et que duration est non nul, on programme son extinction.
			remaining_time_before_turn_off_pump[relay] = duration;
	}

}

#endif

