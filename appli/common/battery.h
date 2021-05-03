/*
 * battery.h
 *
 *  Created on: 10 févr. 2021
 *      Author: norab
 */

#ifndef APPLI_COMMON_BATTERY_H_
#define APPLI_COMMON_BATTERY_H_

#define PIN_MEASURE_VBAT 31

// 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_MV_PER_LSB   (0.73242188F)

void MEASURE_VBAT_init(void);
uint16_t MEASURE_VBAT_get(void);
uint8_t MEASURE_VBAT_get_level(void);
uint8_t mvToPercent(float mvolts);

#endif /* APPLI_COMMON_BATTERY_H_ */
