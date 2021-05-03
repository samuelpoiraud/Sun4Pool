/*
 * flashWriter.h
 *
 *  Created on: 16 janv. 2020
 *      Author: Pierre Legeais
 */

#ifndef SRC_FLASHWRITER_H_
#define SRC_FLASHWRITER_H_
#include "appli/config.h"
#include <stdint.h>

void FLASHWRITER_init(void);

uint32_t FLASHWRITER_read(uint32_t address);

running_e FLASHWRITER_write(uint32_t address, uint32_t value);

void FLASHWRITER_erase(uint32_t address);



#endif /* SRC_FLASHWRITER_H_ */
