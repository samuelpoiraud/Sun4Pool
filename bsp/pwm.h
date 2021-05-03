/*
 * pwm.h
 *
 *  Created on: 22/02/2021
 *      Author: Nirgal
 */

#ifndef PWM_H
#define PWM_H


#include "../appli/config.h"
#include "nrfx_pwm.h"

void PWM_init(uint8_t * pins, uint8_t pin_nb, nrf_pwm_clk_t base_clock, uint16_t top_value);
void PWM_set_duty(uint8_t pin_id, uint16_t duty);

#endif /* PWM_H */
