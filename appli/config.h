/*
 * config.h
 *
 *  Created on: 21 nov. 2019
 *      Author: Nirgal
 */

#ifndef CONFIG_CONFIG_H_
#define CONFIG_CONFIG_H_

#include <stdint.h>
#include "common/macro_types.h"
#include "modules/nrfx/mdk/nrf52.h"
#include "modules/nrfx/mdk/nrf52_bitfields.h"
#include "components/softdevice/s132/headers/nrf_error.h"
//#include "../config/nrfx_config.h"
//#include "modules/nrfx/hal/nrf_gpio.h"..


//liste des objets
#define OBJECT_BASE_STATION			0
#define OBJECT_SUN4POOL				0x42
#define OBJECT_REMOTE4PUMPS			0x43
#define OBJECTS_NB					2


///////////////////////////////////////////////////////////////////////////////////
//Numéro de l'objet pour lequel on compile
#define OBJECT_ID		OBJECT_SUN4POOL
///////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////
//Config propres a chaque objets.

#if OBJECT_ID == OBJECT_SUN4POOL
	#define GPIO_RELAY_FILTER_PUMP		19
	#define GPIO_RELAY_SOLAR_HEATER		20

#endif

#if OBJECT_ID == OBJECT_REMOTE4PUMPS
	#define MPU6050_VCC_PIN 27
#endif
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//Configs communes à tout les objets.


#ifndef PIN_UART_TX
	#define PIN_UART_TX			6
#endif
#ifndef PIN_UART_RX
	#define PIN_UART_RX			8
#endif

#define PIN_BUTTON_NETWORK	18
#define PIN_LED_NETWORK		19
#define PIN_LED_BATTERY		20
#define PIN_RESET			21

#define USE_SPI	0





#define ENABLE_POWERDOWN_FROM_MCU		1	//si 1 : permet de couper l'alim avec un appui long sur le bouton poussoir. Impose le maintient du bouton pendant 1 seconde au démarrage.


#ifndef USE_SERIAL_DIALOG		//si aucun objet n'a défini ceci é 0... alors on active la brique SERIAL_DIALOG
	#define USE_SERIAL_DIALOG	1
#endif

//pour voir les IRQ Radio...
#define SP_DEBUG_RADIO_IRQ_INIT()		nrf_gpio_cfg_output(12)
#define SP_DEBUG_RADIO_IRQ_SET()		NRF_P0->OUTSET = (1 << (12))
#define SP_DEBUG_RADIO_IRQ_RESET()		NRF_P0->OUTCLR = (1 << (12))


#define OFF_BUTTON_LONG_PRESS_DURATION	2000	//durée de l'appui sur le bouton OFF qui déclenche l'extinction.
#define AUTO_OFF_IF_NO_EVENT_DURATION	(30*60*1000)	//extinction automatique au bout de 30mn

#define TIMESLOT_DURATION	1	//ms

#define OFFSET_TRANSMISSION_DURATION	13440

void uart_puts(char * s);
uint32_t debug_printf(char * format, ...);




#define USE_ADC						1
#define USE_ADC_CHANNEL_AIN0		0	//P0.02
#define USE_ADC_CHANNEL_AIN1		0	//P0.03
#define USE_ADC_CHANNEL_AIN2		0	//P0.04
#define USE_ADC_CHANNEL_AIN3		0	//P0.05
#define USE_ADC_CHANNEL_AIN4		0	//P0.06
#define USE_ADC_CHANNEL_AIN7		0	//P0.31

#define ADC_USE_CHANNELS_NB			(USE_ADC_CHANNEL_AIN0+USE_ADC_CHANNEL_AIN1+USE_ADC_CHANNEL_AIN7)

#define CHANNEL_ADC_MEASURE_VBAT	7
#if OBJECT_ID == OBJECT_VENTILATOR
	#define CHANNEL_ADC_MCP9701			1
#endif

#ifndef USE_PWM
	#define USE_PWM			1
#endif


#ifndef USE_DS18B20
	#define USE_DS18B20		(OBJECT_ID == OBJECT_SUN4POOL)
#endif

#ifndef USE_DHT11
	#define USE_DHT11		0
#endif

#ifndef USE_NMOS_GND
	#define USE_NMOS_GND	0
#endif

#ifndef USE_MPU6050
	#define USE_MPU6050		(OBJECT_ID == OBJECT_REMOTE4PUMPS)
#endif

#ifndef USE_BH1750FVI
	#define USE_BH1750FVI	0
#endif

#ifndef USE_MATRIX
	#define USE_MATRIX		0
#endif

#ifndef USE_BMP180
	#define USE_BMP180		0
#endif

#ifndef USE_EPAPER
	#define USE_EPAPER		0
#endif

#ifndef USE_UART_SOFT
	#define USE_UART_SOFT	0
#endif

#ifndef USE_MCP9804
	#define USE_MCP9804     0
#endif

#if USE_MPU6050 || USE_BMP180 ||USE_BH1750FVI//   || USE_... || USE...
	#define USE_TWI	1
	#ifndef I2C_SDA_PIN_NB
		#define	I2C_SDA_PIN_NB	25
	#endif
	#ifndef I2C_SCL_PIN_NB
		#define	I2C_SCL_PIN_NB	26
	#endif
#else
	#define USE_TWI	0
#endif

#if USE_MCP9804
	#ifndef I2C_SDA_PIN_NB
		#define	I2C_SDA_PIN_NB	5
	#endif
	#ifndef I2C_SCL_PIN_NB
		#define	I2C_SCL_PIN_NB	6
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////////
#ifndef NRF52832_XXAA
	#define NRF52832_XXAA
#endif
#include "nrf.h"


#endif /* CONFIG_CONFIG_H_ */

