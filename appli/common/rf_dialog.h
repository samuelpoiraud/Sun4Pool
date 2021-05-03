/*
 * rf_dialog.h
 *
 *  Created on: 10 févr. 2021
 *      Author: Guillaume & Thomas
 */

#ifndef APPLI_COMMON_RF_DIALOG_H_
#define APPLI_COMMON_RF_DIALOG_H_
#include "appli/config.h"
#include "nrf_esb.h"

//Constitution d'un message.
//				Master Group RECIPIENTS(6) MSG_ID DATASIZE DATAS
#define BYTE_POS_RECIPIENTS	(0)
	#define BYTE_QTY_RECIPIENTS		(4)
#define BYTE_POS_EMITTER	(BYTE_POS_RECIPIENTS+BYTE_QTY_RECIPIENTS)
	#define BYTE_QTY_EMITTER		(4)
#define BYTE_POS_MSG_CNT	(BYTE_POS_EMITTER+BYTE_QTY_RECIPIENTS)
#define BYTE_POS_MSG_ID		(BYTE_POS_MSG_CNT+1)
#define BYTE_POS_DATASIZE	(BYTE_POS_MSG_ID+1)
#define BYTE_POS_DATAS		(BYTE_POS_DATASIZE+1)
#define MAX_DATA_SIZE		(32-BYTE_POS_DATAS)


typedef enum{
	RECENT_RESET 				= 0x02,
	ASK_FOR_SOFTWARE_RESET		= 0x03,
	PING						= 0x16,
	PONG						= 0x06,
	EVENT_OCCURED				= 0x30,
	PARAMETER_IS				= 0x40,
	PARAMETER_ASK				= 0x41,
	PARAMETER_WRITE				= 0x42,
	I_HAVE_NO_SERVER_ID			= 0xFD,
	YOUR_SERVER_ID_IS			= 0xFE,
}msg_id_e;

typedef enum{
	// TODO ID en 32 bits pour tous les objets
	BASE_STATION		= 0xFFFFFFFF,
    SMART_LIGHT		    = 1,
	NIGHT_LIGHT		    = 2,
	BRIGHTNESS_SENSOR   = 4,
	STATION_METEO_INT   = 5,
	OUT_WEATHER_STATION = 6,
	ROLLER_SHUTTER      = 7,
	ALARM               = 8,
	FIRE_DETECTOR       = 9,
	WINE_DEGUSTATION    = 10,
	VENTILATOR          = 11,
	GSM                 = 12,
	FALL_SENSOR         = 13,
	TRACKER_GPS         = 16,
	RFID                = 17,
	VOICE_CONTROL       = 19,
	TOUCH_SCREEN        = 20,
	E_PAPER             = 21,
	MATRIX_LEDS		    = 24,
	NB				    = 25,
}recipient_e;

uint32_t RF_DIALOG_get_my_base_station_id(void);
void RF_DIALOG_send_msg_id_to_basestation(msg_id_e msg_id, uint8_t datasize, uint8_t * datas);
void RF_DIALOG_send_msg_id_to_object(recipient_e obj_id,msg_id_e msg_id, uint8_t datasize, uint8_t * datas);
void RF_DIALOG_process_rx_basestation(nrf_esb_payload_t * payload);
void RF_DIALOG_process_rx_object(nrf_esb_payload_t * payload);

#endif /* APPLI_COMMON_RF_DIALOG_H_ */
