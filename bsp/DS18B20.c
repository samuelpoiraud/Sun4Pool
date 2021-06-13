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

#define DS18B20_delay_us		SYSTICK_delay_us
#define DS18B20_delay_ms		SYSTICK_delay_ms
#define DS18B20_pin_write(x)	GPIO_write(DS18B20_PIN, x);
#define DS18B20_pin_read()		GPIO_read(DS18B20_PIN)


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
		DS18B20_delay_ms(800);

		presence = DS18B20_Start();
		if(presence)
		{
			DS18B20_delay_us(1000);
			DS18B20_Write (0xCC);  // skip ROM
			DS18B20_Write (0xBE);  // Read Scratch-pad


			lsb = DS18B20_Read();
			msb = DS18B20_Read();
		}
	}

	return U16FROMU8(msb, lsb);
}

static uint8_t DS18B20_Start (void)
{
	uint8_t response = 0;
	DS18B20_pin_write(0);	 // pull the pin low
	DS18B20_delay_us(480);   // delay according to datasheet

	DS18B20_pin_write(1);
	DS18B20_delay_us(80);    // delay according to datasheet

	if (!(GPIO_read(DS18B20_PIN)))
		response = 1;    // if the pin is low i.e the presence pulse is detected

	DS18B20_delay_us(400); // 480 us delay totally.

	return response;
}

static void DS18B20_Write (uint8_t data)
{
	for(int i=0; i<8; i++)
	{
		if((data & (1<<i))!=0)  // if the bit is high
		{	// write 1
			DS18B20_pin_write(0);  // pull the pin LOW
			DS18B20_delay_us(1);  // wait for 1 us

			DS18B20_pin_write(1);
			DS18B20_delay_us(60);  // wait for 60 us
		}

		else  // if the bit is low
		{	// write 0

			DS18B20_pin_write(0);
			DS18B20_delay_us(60);  // wait for 60 us

			DS18B20_pin_write(1);
			DS18B20_delay_us(15);	//wait for pull up !
		}
	}
}

static uint8_t DS18B20_Read (void)
{
	uint8_t value=0;

	for(int i=0;i<8;i++)
	{
		DS18B20_pin_write(0);  // pull the data pin LOW
		DS18B20_delay_us(2);  // wait for 2 us

		DS18B20_pin_write(1); // set as input
		DS18B20_delay_us(10);  // wait for pullup if the sensor do not write 0
		if(DS18B20_pin_read())  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		DS18B20_delay_us(50);  // wait for the remaining 50 us (50+10 = 60)
	}
	return value;
}

#endif
