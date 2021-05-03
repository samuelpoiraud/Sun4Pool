/*
 * nrf52_i2c.h
 *
 *  Created on: 9 févr. 2021
 *      Author: Nirgal
 */

#ifndef BSP_NRF52_I2C_H_
#define BSP_NRF52_I2C_H_
#include "appli/config.h"

void I2C_init(uint8_t device_address);

running_e I2C_register_read(uint8_t register_address, uint8_t * destination, uint8_t number_of_bytes);

running_e I2C_register_write(uint8_t register_address, uint8_t value);

running_e I2C_write(uint8_t * data, uint8_t size);

running_e I2C_read(uint8_t * data, uint8_t size);


#endif /* BSP_NRF52_I2C_H_ */
