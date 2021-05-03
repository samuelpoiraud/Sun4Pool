/*
 * DS18B20.c
 *
 *  Created on: 3 mai 2021
 *      Author: Nirgal
 */
#include "../appli/config.h"

#if USE_DS18B20

#include "../appli/common/systick.h"
#include "../appli/common/gpio.h"

#ifndef DS18B20_PIN
	#define DS18B20_PIN	(13)
#endif

static volatile bool_e initialized = FALSE;

static uint8_t DS18B20_Start (void);
static void DS18B20_Write (uint8_t data);
static uint8_t DS18B20_Read (void);

void DS18B20_init(void)
{
	if(!initialized)
	{
		GPIO_configure_input_output_opendrain(DS18B20_PIN, GPIO_PIN_CNF_PULL_Pullup);   // set the pin as output opendrain
		initialized = TRUE;
	}
}

int16_t DS18B20_get_temperature(void)
{
	uint8_t msb, lsb;
	msb = 0;
	lsb = 0;
	uint8_t presence;

	if(!initialized)
		DS18B20_init();

	presence = DS18B20_Start();
	if(presence)
	{
		SYSTICK_delay_us(1000);
		DS18B20_Write (0xCC);  // skip ROM
		DS18B20_Write (0x44);  // convert t
		SYSTICK_delay_ms(800);

		presence = DS18B20_Start();
		if(presence)
		{
			SYSTICK_delay_us(1000);
			DS18B20_Write (0xCC);  // skip ROM
			DS18B20_Write (0xBE);  // Read Scratch-pad


			msb = DS18B20_Read();
			lsb = DS18B20_Read();
		}
	}

	return U16FROMU8(msb, lsb);
}

static uint8_t DS18B20_Start (void)
{
	uint8_t response = 0;
	GPIO_write(DS18B20_PIN, 0);	 // pull the pin low
	SYSTICK_delay_us(480);   // delay according to datasheet

	//Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);    // set the pin as input
	SYSTICK_delay_us(80);    // delay according to datasheet

	if (!(GPIO_read(DS18B20_PIN)))
		response = 1;    // if the pin is low i.e the presence pulse is detected
	else
		response = -1;

	SYSTICK_delay_us(400); // 480 us delay totally.

	return response;
}

static void DS18B20_Write (uint8_t data)
{
	for(int i=0; i<8; i++)
	{

		if((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1
			GPIO_write(DS18B20_PIN, 0);  // pull the pin LOW
			SYSTICK_delay_us(1);  // wait for 1 us

			GPIO_write(DS18B20_PIN, 1);
			SYSTICK_delay_us(50);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0
			GPIO_write(DS18B20_PIN, 0);
			SYSTICK_delay_us(50);  // wait for 60 us

			GPIO_write(DS18B20_PIN, 1);
		}
	}
}

static uint8_t DS18B20_Read (void)
{
	uint8_t value=0;

	for(int i=0;i<8;i++)
	{
		GPIO_write(DS18B20_PIN, 0);  // pull the data pin LOW
		SYSTICK_delay_us(2);  // wait for 2 us

		GPIO_write(DS18B20_PIN,1); // set as input
		if(GPIO_read(DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		SYSTICK_delay_us(60);  // wait for 60 us
	}
	return value;
}

#endif
