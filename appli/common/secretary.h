/*
 * secretary.h
 *
 *  Created on: 21 nov. 2019
 *      Author: Nirgal
 */

#ifndef APPLI_SECRETARY_H_
#define APPLI_SECRETARY_H_

#include "nrf_esb.h"
#include <stdint.h>
#include "../config.h"


void SECRETARY_esb_event_handler(nrf_esb_evt_t const * p_event);

void SECRETARY_process_main(void);

void SECRETARY_init(void);

void SECRETARY_process_msg_from_uart(uint8_t size, uint8_t * datas);

void SECRETARY_process_msg_to_uart(nrf_esb_payload_t * payload);

void SECRETARY_send_msg(uint8_t size, uint8_t * datas);

_Bool SECRETARY_toggle_debug_mode(void);

void SECRETARY_consume_fifo(void);


#endif /* APPLI_SECRETARY_H_ */
