/*
 * stm32f1_xpt2046.h
 *
 *  Created on: Nov 17, 2016
 *      Author: louisz
 */



#ifndef BSP_LCD_ILI9341_STM32F1_XPT2046_H_
#define BSP_LCD_ILI9341_STM32F1_XPT2046_H_

#include "../appli/config.h"
#include "../appli/common/macro_types.h"
#include "nrfx_spi.h"
#include "ili9341.h"
#include "nrf_gpio.h"

#if OBJECT_ID == OBJECT_TOUCH_SCREEN

typedef enum{
	XPT2046_COORDINATE_RAW,
	XPT2046_COORDINATE_SCREEN_RELATIVE
}XPT2046_coordinateMode_e;

void XPT2046_init(void);
void XPT2046_demo(void);
bool_e XPT2046_getCoordinates(Sint16 * pX, Sint16 * pY, XPT2046_coordinateMode_e coordinateMode);
bool_e XPT2046_getMedianCoordinates(Sint16 * pX, Sint16 * pY, XPT2046_coordinateMode_e coordinateMode);
bool_e XPT2046_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, XPT2046_coordinateMode_e coordinateMode);


#define XPT2046_USE_PIN_IRQ_TO_CHECK_TOUCH

#endif /* BSP_LCD_ILI9341_STM32F1_XPT2046_H_ */

#endif
