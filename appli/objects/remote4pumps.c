/*
 * sun4pool.c
 *
 *  Created on: 1 mai 2021
 *      Author: Nirgal
 */
#include "../config.h"
#include "remote4pumps.h"
#include "../common/systick.h"
#include "../common/gpio.h"
#include "../common/parameters.h"
#include "../common/rf_dialog.h"
#include "../../bsp/mpu6050.h"

#if OBJECT_ID == OBJECT_REMOTE4PUMPS

#define DURATION_ONE_HOUR	(3600*1000)
#define DURATION_ONE_MINUTE	(60*1000)

static volatile uint32_t t = 0;

typedef enum
{
	DIRECTION_NONE,
	DIRECTION_X_UP,
	DIRECTION_X_DOWN,
	DIRECTION_Y_UP,
	DIRECTION_Y_DOWN,
	DIRECTION_Z_UP,
	DIRECTION_Z_DOWN
}direction_e;

direction_e REMOTE4PUMPS_read_gravity_direction(void);
void REMOTE4PUMPS_process_ms(void);
void REMOTE4PUMPS_send_msg(param_id_e param, uint32_t duration);

static MPU6050_t MPU6050_Data;

static void REMOTE4PUMPS_init(void)
{

}


void REMOTE4PUMPS_state_machine(void)
{
	typedef enum
	{
		INIT,
		READ_GRAVITY_TO_CHOSE_MODE,
		MODE_FILTER_PUMP_ON,
		MODE_FILTER_PUMP_OFF,
		MODE_SOLARHEATER_PUMP_SET,
		MODE_SOLARHEATER_PUMP_OFF,
		MODE_SOLARHEATER_PUMP_WAIT_EVENT,
		DONE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static uint32_t nb_minutes_for_solar_heater = 0;
	bool_e entrance;
	entrance = (state!=previous_state)?TRUE:FALSE;
	previous_state = state;
	direction_e direction;
	direction = REMOTE4PUMPS_read_gravity_direction();
	//debug_printf("d=%d\n",direction);
	SYSTICK_delay_ms(100);
	switch(state)
	{
		case INIT:
			REMOTE4PUMPS_init();
			Systick_add_callback_function(&REMOTE4PUMPS_process_ms);
			state = READ_GRAVITY_TO_CHOSE_MODE;
			nb_minutes_for_solar_heater = 0;
			break;
		case READ_GRAVITY_TO_CHOSE_MODE:
			switch(direction)
			{
				case DIRECTION_Y_UP:
					state = MODE_FILTER_PUMP_ON;
					break;
				case DIRECTION_Y_DOWN:
					state = MODE_FILTER_PUMP_OFF;
					break;
				case DIRECTION_X_UP:
					state = MODE_SOLARHEATER_PUMP_SET;
					break;
				case DIRECTION_X_DOWN:
					state = MODE_SOLARHEATER_PUMP_OFF;
					break;
				default:
					break;
			}
			break;
		case MODE_FILTER_PUMP_ON:
			if(entrance)
			{
				debug_printf("MODE_FILTER_PUMP_ON\n");
				REMOTE4PUMPS_send_msg(PARAM_FORCE_FILTER_PUMP_ON_FOR_X_MS, DURATION_ONE_HOUR);
			}
			if(direction == DIRECTION_Y_DOWN)		//une fois dans le mode FILTER_PUMP, on s'autorise à passer du ON au OFF...
				state = MODE_FILTER_PUMP_OFF;
			break;
		case MODE_FILTER_PUMP_OFF:
			if(entrance)
			{
				debug_printf("MODE_FILTER_PUMP_OFF\n");
				REMOTE4PUMPS_send_msg(PARAM_FORCE_FILTER_PUMP_OFF_FOR_X_MS, DURATION_ONE_HOUR);
			}
			if(direction == DIRECTION_Y_UP)			//une fois dans le mode FILTER_PUMP, on s'autorise à passer du ON au OFF...
				state = MODE_FILTER_PUMP_ON;
			break;



		case MODE_SOLARHEATER_PUMP_SET:
			if(entrance)
			{
				nb_minutes_for_solar_heater++;
				debug_printf("MODE_SOLARHEATER_PUMP_SET : %dmn\n", nb_minutes_for_solar_heater);
				REMOTE4PUMPS_send_msg(PARAM_SET_TIME_TO_FORCE_SOLAR_HEATER_PUMP, DURATION_ONE_MINUTE*nb_minutes_for_solar_heater);
			}
			if(direction != DIRECTION_X_UP)			//une fois dans le mode SOLAR_HEATER, on s'autorise à passer du ON au OFF...
				state = MODE_SOLARHEATER_PUMP_WAIT_EVENT;
			break;
		case MODE_SOLARHEATER_PUMP_OFF:
			if(entrance)
			{
				debug_printf("MODE_SOLARHEATER_PUMP_OFF\n");
				REMOTE4PUMPS_send_msg(PARAM_SET_TIME_TO_FORCE_SOLAR_HEATER_PUMP, 0);
				nb_minutes_for_solar_heater = 0;
			}
			if(direction != DIRECTION_X_DOWN)			//une fois dans le mode SOLAR_HEATER, on s'autorise à passer du ON au OFF...
				state = MODE_SOLARHEATER_PUMP_WAIT_EVENT;
			break;
		case MODE_SOLARHEATER_PUMP_WAIT_EVENT:
			if(entrance)
				t = 400;
			if(!t)
			{
				switch(direction)
				{
					case DIRECTION_X_UP:
						state = MODE_SOLARHEATER_PUMP_SET;
						break;
					case DIRECTION_X_DOWN:
						state = MODE_SOLARHEATER_PUMP_OFF;
						break;
					default:
						break;
				}
			}
			break;
		case DONE:
			break;
		default:
			break;
	}
}


direction_e REMOTE4PUMPS_read_gravity_direction(void)
{
	static bool_e initialized = FALSE;
	if(!initialized)
	{
		GPIO_configure(MPU6050_VCC_PIN, GPIO_PIN_CNF_PULL_Disabled, TRUE);
		GPIO_write(MPU6050_VCC_PIN, 1);
		SYSTICK_delay_ms(100);
		MPU6050_Init(&MPU6050_Data, MPU6050_Accelerometer_4G, MPU6050_Gyroscope_1000s);
		initialized = TRUE;
	}
	direction_e ret = DIRECTION_NONE;
	MPU6050_ReadAllType1(&MPU6050_Data);
	if(MPU6050_Data.Accelerometer_X > 6000)
		ret = DIRECTION_X_UP;
	else if(MPU6050_Data.Accelerometer_X < -6000)
		ret = DIRECTION_X_DOWN;
	else if(MPU6050_Data.Accelerometer_Y > 6000)
		ret = DIRECTION_Y_UP;
	else if(MPU6050_Data.Accelerometer_Y < -6000)
		ret = DIRECTION_Y_DOWN;
	else if(MPU6050_Data.Accelerometer_Z > 6000)
		ret = DIRECTION_Z_UP;
	else if(MPU6050_Data.Accelerometer_Z < -6000)
		ret = DIRECTION_Z_DOWN;
	return ret;
}

void REMOTE4PUMPS_process_ms(void)
{
	if(t)
		t--;
}

void REMOTE4PUMPS_send_msg(param_id_e param, uint32_t duration)
{
	uint8_t datas[5];
	datas[0] = param;
	datas[1] = (duration >> 24)&0xFF;
	datas[2] = (duration >> 16)&0xFF;
	datas[3] = (duration >> 8)&0xFF;
	datas[4] = (duration >> 0)&0xFF;
	RF_DIALOG_send_msg_id_to_object(0xDDDDDD42, PARAMETER_WRITE, 5, datas);
}


#endif

