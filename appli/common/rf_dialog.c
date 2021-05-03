/*
 * rc_dialog.c
 *
 *  Created on: 10 févr. 2021
 *      Author: Guillaume  & Thomas
 */
#include "../config.h"
#include "secretary.h"
#include "rf_dialog.h"
#include "parameters.h"
//Reception e transmission RF

static uint32_t my_device_id = -1;	//constitué de 3 octets d'identifiant unique et 1 octet d'OBJECT_ID
static uint32_t my_base_station_id = 0xFFFFFFFF;
static uint8_t index_msg_cnt = 0;

void RF_DIALOG_init(void)
{
	my_device_id = (NRF_FICR->DEVICEID[0] << 8) | OBJECT_ID;
	my_base_station_id = PARAMETERS_get(PARAM_MY_BASE_STATION_ID);
}

uint32_t RF_DIALOG_get_my_base_station_id(void)
{
	return my_base_station_id;
}


static callback_fun_t callback_pong = NULL;

void RF_DIALOG_set_callback_pong(callback_fun_t new_callback)
{
	callback_pong = new_callback;
}

void RF_DIALOG_process_rx_object(nrf_esb_payload_t * payload){

	uint8_t msg_id;
	uint32_t recipient;
	uint32_t emitter;
	if(payload->length > BYTE_POS_DATASIZE && payload->length <= NRF_ESB_MAX_PAYLOAD_LENGTH)
	{
		recipient = U32FROMU8( payload->data[BYTE_POS_RECIPIENTS],  payload->data[BYTE_POS_RECIPIENTS+1],  payload->data[BYTE_POS_RECIPIENTS+2],  payload->data[BYTE_POS_RECIPIENTS+3]);
		emitter = U32FROMU8( payload->data[BYTE_POS_EMITTER],  payload->data[BYTE_POS_EMITTER+1],  payload->data[BYTE_POS_EMITTER+2],  payload->data[BYTE_POS_EMITTER+3]);
		msg_id = payload->data[BYTE_POS_MSG_ID];

		switch(msg_id){
			case RECENT_RESET :{//objet demarre (rf dialog init)
				break;
			}
			case ASK_FOR_SOFTWARE_RESET :{//demande de la station de reset l'objet
				NVIC_SystemReset();
				break;
			}
			case PING :{
				RF_DIALOG_send_msg_id_to_basestation(PONG,0,NULL);
				break;
			}
			case PONG :{

				if(callback_pong != NULL)
					callback_pong();
				break;
			}
			case EVENT_OCCURED :{
				break;
			}
			case PARAMETER_IS :{
				//objet répond demande ask à la base

				break;
			}
			case PARAMETER_ASK :{
				param_id_e param;
				uint8_t datas[5];
				uint32_t value;
				param = payload->data[BYTE_POS_DATAS];
				value = PARAMETERS_get(param);
				datas[0] = param;
				datas[1] = (value>>24)&0xFF;
				datas[2] = (value>>16)&0xFF;
				datas[3] = (value>>8)&0xFF;
				datas[4] = (value>>0)&0xFF;
				RF_DIALOG_send_msg_id_to_basestation(PARAMETER_IS,5,datas);
				break;
			}
			case PARAMETER_WRITE :{
				// la base impose un parametre a l'objet
				param_id_e param;
				uint32_t value;
				param = payload->data[BYTE_POS_DATAS];
				value = U32FROMU8( payload->data[BYTE_POS_DATAS+1],  payload->data[BYTE_POS_DATAS+2],  payload->data[BYTE_POS_DATAS+3],  payload->data[BYTE_POS_DATAS+4]);
				if(param < PARAM_32_BITS_NB)
					PARAMETERS_update(param, value);
				else
					PARAMETERS_update_custom(param, &payload->data[BYTE_POS_DATAS+1]);

				break;
			}
			case I_HAVE_NO_SERVER_ID :{
				//RF_DIALOG_send_msg_id_to_basestation(I_HAVE_NO_SERVER_ID,0,NULL);
				break;
			}

			case YOUR_SERVER_ID_IS :{
				//server=server_id;
				break;
			}

			default:
				break;
		}
	}
}

void RF_DIALOG_process_rx_basestation(nrf_esb_payload_t * payload){

	uint8_t msg_id;
	uint32_t recipient;
	uint32_t emitter;
	if(payload->length > BYTE_POS_DATASIZE && payload->length <= NRF_ESB_MAX_PAYLOAD_LENGTH)
	{
		recipient = U32FROMU8( payload->data[BYTE_POS_RECIPIENTS],  payload->data[BYTE_POS_RECIPIENTS+1],  payload->data[BYTE_POS_RECIPIENTS+2],  payload->data[BYTE_POS_RECIPIENTS+3]);
		emitter = U32FROMU8( payload->data[BYTE_POS_EMITTER],  payload->data[BYTE_POS_EMITTER+1],  payload->data[BYTE_POS_EMITTER+2],  payload->data[BYTE_POS_EMITTER+3]);

		msg_id = payload->data[BYTE_POS_MSG_ID];

		switch(msg_id){
			case RECENT_RESET :{//objet demarre (rf dialog init)
				//RF_DIALOG_send_msg_id_to_object(PARAMETER_ASK,/*datasize*/,/*data*/);
				//TODO gérer cet événement de RESET d'objet dans une autre couche logicielle... pour voir s'il faut envoyer des messages sauvegardés "long terme" pour cet objet.
				break;
			}
			case ASK_FOR_SOFTWARE_RESET :{
				/* la station ne peut pas recevoir un software reset d'un objet, on ignore ce message.*/
				break;
			}
			case PING :{
				//l'emmeteur du PING est le destinataire du PONG.
				RF_DIALOG_send_msg_id_to_object(emitter, PONG,0,NULL);
				break;
			}
			case PONG :{

				if(callback_pong != NULL)
					callback_pong();
				break;
			}
			case EVENT_OCCURED :{
				/* traitement d'un événement à définir*/

				break;
			}
			case PARAMETER_IS :{
				/* parameter = value*/

				break;
			}
			case PARAMETER_ASK :{
				/* ne peut pas recevoir de parameter_ask*/
				break;
			}
			case PARAMETER_WRITE :{
				// la base impose un parametre a l'objet
				break;
			}
			case I_HAVE_NO_SERVER_ID :{
				uint8_t basestation[4];
				basestation[0] = (my_base_station_id>>24)&0xFF;
				basestation[1] = (my_base_station_id>>16)&0xFF;
				basestation[2] = (my_base_station_id>>8)&0xFF;
				basestation[3] = (my_base_station_id>>0)&0xFF;
				RF_DIALOG_send_msg_id_to_object(emitter, YOUR_SERVER_ID_IS,4,basestation);
				//TODO remplacer le FFFFFFFF par notre identifiant, en tant que basestation
				break;
			}

			case YOUR_SERVER_ID_IS :{

				break;
			}

			default:
				break;
		}
	}
}

void RF_DIALOG_send_msg_id_to_basestation(msg_id_e msg_id, uint8_t datasize, uint8_t * datas)
{
	uint8_t msg_to_send[NRF_ESB_MAX_PAYLOAD_LENGTH];
	uint8_t size = 0;

	msg_to_send[BYTE_POS_RECIPIENTS]   = (my_base_station_id>>24)	&0xFF;
	msg_to_send[BYTE_POS_RECIPIENTS+1] = (my_base_station_id>>16)	&0xFF;
	msg_to_send[BYTE_POS_RECIPIENTS+2] = (my_base_station_id>>8)	&0xFF;
	msg_to_send[BYTE_POS_RECIPIENTS+3] = (my_base_station_id>>0)	&0xFF;

	msg_to_send[BYTE_POS_EMITTER]   = (OBJECT_ID >>24) & 0xFF;
	msg_to_send[BYTE_POS_EMITTER+1] = (OBJECT_ID >>16) & 0xFF;
	msg_to_send[BYTE_POS_EMITTER+2] = (OBJECT_ID >>8) & 0xFF;
	msg_to_send[BYTE_POS_EMITTER+3] = (OBJECT_ID >>0) & 0xFF;


	msg_to_send[BYTE_POS_MSG_CNT] = index_msg_cnt;
	index_msg_cnt++;

	msg_to_send[BYTE_POS_MSG_ID] = msg_id;

	datasize = MIN(datasize, MAX_DATA_SIZE);
	msg_to_send[BYTE_POS_DATASIZE] = datasize;

	for(uint8_t i = 0; i<datasize; i++)
	{
		msg_to_send[BYTE_POS_DATAS+i] = datas[i];
	}


	SECRETARY_send_msg(BYTE_POS_DATAS+datasize, msg_to_send);
}


void RF_DIALOG_send_msg_id_to_object(recipient_e obj_id,msg_id_e msg_id, uint8_t datasize, uint8_t * datas){
	uint8_t msg_to_send[NRF_ESB_MAX_PAYLOAD_LENGTH];
		uint8_t size = 0;

		msg_to_send[BYTE_POS_RECIPIENTS]   = (obj_id>>24)	&0xFF;
		msg_to_send[BYTE_POS_RECIPIENTS+1] = (obj_id>>16)	&0xFF;
		msg_to_send[BYTE_POS_RECIPIENTS+2] = (obj_id>>8)	&0xFF;
		msg_to_send[BYTE_POS_RECIPIENTS+3] = (obj_id>>0)	&0xFF;

		msg_to_send[BYTE_POS_EMITTER]   = (0xFF >>24) & 0xFF;	//TODO gérer l'identifiant unique station (autre que FFFFFFFF)
		msg_to_send[BYTE_POS_EMITTER+1] = (0xFF >>16) & 0xFF;
		msg_to_send[BYTE_POS_EMITTER+2] = (0xFF >>8) & 0xFF;
		msg_to_send[BYTE_POS_EMITTER+3] = (0xFF >>0) & 0xFF;

		msg_to_send[BYTE_POS_MSG_CNT] = index_msg_cnt;
		index_msg_cnt++;

		msg_to_send[BYTE_POS_MSG_ID] = msg_id;

		datasize = MIN(datasize, MAX_DATA_SIZE);
		msg_to_send[BYTE_POS_DATASIZE] = datasize;

		for(uint8_t i = 0; i<datasize; i++)
		{
			msg_to_send[BYTE_POS_DATAS+i] = datas[i];
		}


		SECRETARY_send_msg(BYTE_POS_DATAS+datasize, msg_to_send);
}

void RF_dialog_sample_bank(void) // C'EST UN EXEMPLE!!!!!
{
	RF_DIALOG_send_msg_id_to_basestation(RECENT_RESET, 0, NULL);
	uint8_t param_id = 12;
	RF_DIALOG_send_msg_id_to_basestation(PARAMETER_ASK, 1, &param_id);

	uint8_t tab[4] = {0xCA, 0xFE, 0xDE, 0xCA};
	RF_DIALOG_send_msg_id_to_basestation(YOUR_SERVER_ID_IS, 4, tab);

}
