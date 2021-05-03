/*
 * parameter.c
 *
 *  Created on: 15 févr. 2021
 *      Author: meven
 */

#include "appli/config.h"
#include "parameters.h"
#include "flash.h"

typedef struct
{
	bool_e enable;
	bool_e updated;
	bool_e value_saved_in_flash;	//TODO gérer cette fonctionnalité
	uint32_t value;
	callback_fun_i32_t	callback;
}params_t;

static params_t params[PARAM_NB];


//Cette fonction doit être appelée lors de l'init, avant l'init des objets.
void PARAMETERS_init(void)
{
	for(uint8_t i = 0; i<PARAM_32_BITS_NB; i++)
	{
		params[i] = (params_t){
			.enable = FALSE,
			.updated = FALSE,
			.value = 0,
			.callback = NULL
			};
	}
	FLASHWRITER_init();
}

//chaque objet doit appeler cette fonction pour chacun de ses paramètres
void PARAMETERS_enable(param_id_e param_id, int32_t default_value, bool_e value_saved_in_flash, callback_fun_i32_t callback)
{
	params[param_id].enable = TRUE;
	params[param_id].value_saved_in_flash = value_saved_in_flash;
	params[param_id].callback = callback;
	params[param_id].updated = FALSE;
	params[param_id].value = default_value;
	if(value_saved_in_flash)
	{
		PARAMETERS_read_from_flash(param_id);	//la default value sera écrasée si ce paramètre dispose d'une valeur en flash !
	}
}

void PARAMETERS_update(param_id_e param_id, int32_t new_value)
{
	if(param_id < PARAM_32_BITS_NB && params[param_id].enable)
	{
		params[param_id].value = new_value;
		params[param_id].updated = TRUE;
		if(params[param_id].callback != NULL)
			params[param_id].callback(new_value);

		if(params[param_id].value_saved_in_flash)
		{
			//sauvegarder le paramètre en flash...
			uint32_t address = (uint32_t)param_id * 4;
			FLASHWRITER_write(address, params[param_id].value_saved_in_flash);
		}
	}
}

//cette fonction se destine aux paramètres spécifiques dont la valeur ne peut se contenter de 32 bits.
//dans ce cas, on confie à une callback le traitement des données... exprimées sous forme d'un paquet d'octet.
void PARAMETERS_update_custom(param_id_e param_id, uint8_t * datas)
{
	if(param_id > PARAM_32_BITS_NB && param_id < PARAM_NB && params[param_id].enable)
	{
		params[param_id].updated = TRUE;
		if(params[param_id].callback != NULL)
			params[param_id].callback((uint32_t)datas);	//on transmets l'adresse des données à traiter.... de façon un peu violente.
	}
}


void PARAMETERS_read_from_flash(param_id_e param_id)
{
	//TODO si le paramètre est activé, et que son value_saved_in_flash est vrai, on va chercher sa valeur en flash.
	//Etape1 : on lit la première adresse de la flash pour savoir si des données s'y trouvent
	//Etape2 : Si des données s'y trouvent, on va chercher ce paramètre
		//pour toute donnée lue à 0xFFFFFFFF -> on préfère la valeur par défaut (=on ne mets pas à jour le paramètre)

	if(params[param_id].enable && params[param_id].value_saved_in_flash){
		uint32_t address = (uint32_t)param_id * 4;
		uint32_t save_flash = FLASHWRITER_read(address);
		if(save_flash != 0){
			PARAMETERS_get(address);
		}
	}

}


//Cette fonction sauvegarde en flash tout les paramètres dont la value_saved_in_flash est vrai, et dont la valeur est différente ce celle présente en flash !
void PARAMETERS_save_to_flash(void)
{
	PARAMETERS_init();
	for(uint8_t i = 0; i<PARAM_32_BITS_NB; i++){
		uint32_t address = i * 4;
		if(params[i].value_saved_in_flash && params[i].value != FLASHWRITER_read(address)){
			FLASHWRITER_write(address, params[i].value);
		}
	}
}

int32_t PARAMETERS_get(param_id_e param_id)
{
	return params[param_id].value;
}


void PARAMETERS_set(param_id_e param_id, uint32_t value)
{
	params[param_id].value = value;
}

/*
//Orientation du main vers chaque code de chaque objets
    		#if id_object == OBJECT_BASE_STATION


    		#endif


    		#if id_object == OBJECT_SMART_LIGHT

			state_e state_led;

			typedef enum{
				DAY,
				NIGHT,
				FILM
			}mode_e;

			uint32_t brightness;

    		#endif

    		#if id_object == OBJECT_NIGHT_LIGHT

			state_e state_led;

			color_t color;

    		#endif

    		#if id_object == OBJECT_BRIGHTNESS_SENSOR

			uint32_t brightness;

    		#endif

    		#if id_object == OBJECT_STATION_METEO_INT

			uint32_t temperature;

			uint32_t pressure;

			uint32_t humidity;

    		#endif

    		#if id_object == OBJECT_OUT_WEATHER_STATION

			uint32_t temperature;

			uint32_t pressure;

			uint32_t humidity;

			uint32_t wind_speed;

			uint32_t rain;

    		#endif

    		#if id_object == OBJECT_ROLLER_SHUTTER



    		#endif

    		#if id_object == OBJECT_ALARM

			state_e alert;

    		#endif

    		#if id_object == OBJECT_FIRE_DETECTOR

			uint32_t smoke_concentration;

			uint32_t CO_concentration;

			uint32_t gas_concentration;

    		#endif

    		#if id_object == OBJECT_WINE_DEGUSTATION

			uint32_t temperature;

    		#endif

    		#if id_object == OBJECT_VENTILATOR

			uint32_t temperature;

    		#endif

    		#if id_object == OBJECT_GSM

			state_e sending_alert;

			//a compléter

    		#endif

    		#if id_object == OBJECT_FALL_SENSOR

			uint32_t limit_alert;

			parameters_e limit;

			state_e alert;

			state_e state_object;

    		#endif

    		#if id_object == OBJECT_TRACKER_GPS



    		#endif

    		#if id_object == OBJECT_RFID



    		#endif

    		#if id_object == OBJECT_TRACKER_GPS


    		#endif

    		#if id_object == OBJECT_VOICE_CONTROL


    		#endif

    		#if id_object == OBJECT_TOUCH_SCREEN


    		#endif

    		#if id_object == OBJECT_E_PAPER


    		#endif

    		#if id_object == OBJECT_MATRIX_LEDS

			state_e state_object;

    		#endif



*/
