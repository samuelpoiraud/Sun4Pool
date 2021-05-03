/*
 * battery.c
 *
 *  Created on: 9 févr. 2021
 *      Author: norab
 */

#include "adc.h"
#include "battery.h"
#include "nrfx_saadc.h"


void MEASURE_VBAT_init(void)
{
	ADC_init();
}


/**@Measure Vbat (in mV) **/
uint16_t MEASURE_VBAT_get(void){
	int16_t value;
	ADC_read(CHANNEL_ADC_MEASURE_VBAT, &value);
	int32_t mV;
	mV = value;
	mV *= 3300;
	mV /= 4096;

	return mV*2;	//tension batterie = 2*tension mesurée sur l'entrée ADC (pont diviseur /2 !)
}

/**@Measure Vbat (in %) **/
uint8_t MEASURE_VBAT_get_level(void){
	uint8_t Vbat_percent = mvToPercent(MEASURE_VBAT_get());
	return Vbat_percent;
}

uint8_t mvToPercent(float mvolts) {
  uint8_t battery_level;

  if (mvolts >= 4200) {
    battery_level = 100;
  } else if (mvolts > 3900) {
    battery_level = 100 - ((4200 - mvolts) * 58) / 100;
  } else if (mvolts > 3800) {
    battery_level = 42 - ((3900 - mvolts) * 24) / 160;
  } else if (mvolts > 3600) {
    battery_level = 18 - ((3800 - mvolts) * 12) / 300;
  } else if (mvolts > 3000) {
    battery_level = 6 - ((3600 - mvolts) * 6) / 340;
  } else {
    battery_level = 0;
  }

  return battery_level;
}
