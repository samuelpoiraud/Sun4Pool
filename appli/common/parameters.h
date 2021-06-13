/*
 * parameters.h
 *
 *  Created on: 15 févr. 2021
 *      Author: meven
 */

#ifndef BSP_PARAMETERS_H_
#define BSP_PARAMETERS_H_
#include "macro_types.h"
#include "../config.h"

//Liste des paramètres, tout objets confondus.
typedef enum
{
	PARAM_UNKNOW = 0,
	PARAM_MY_BASE_STATION_ID,
	PARAM_ACTUATOR_STATE,
	PARAM_SENSOR_VALUE,
	PARAM_ALARM_TRESHOLD,
	PARAM_ALARM_WAY,
	PARAM_TEMPERATURE,
	PARAM_HYGROMETRY,
	PARAM_COLOR,
	PARAM_REFRESH_PERIOD,
	PARAM_PRESSURE,
	PARAM_BRIGHTNESS,
	PARAM_WINDSPEED,
	PARAM_PLUVIOMETRY,
	PARAM_SCREEN_COLOR,
	PARAM_MODE,
	PARAM_MS_SINCE_MIDNIGHT,
	PARAM_FILTER_PUMP_DURATION,
	PARAM_FILTER_PUMP_PERIOD,
	PARAM_SOLAR_HEATER_PUMP_DURATION,
	PARAM_SOLAR_HEATER_PUMP_PERIOD,
	PARAM_NIGHT_HOUR,
	PARAM_DAY_HOUR,
	PARAM_FORCE_FILTER_PUMP_ON_FOR_X_MS,		//force le fonctionnement de la pompe de filtrage pendant 1h
	PARAM_FORCE_FILTER_PUMP_OFF_FOR_X_MS,		//force le non fonctionnement de la pompe de filtrage pendant 1h
	PARAM_SET_TIME_TO_FORCE_SOLAR_HEATER_PUMP,	//ajoute une durée de forcage de la pompe du réchauffeur solaire

	PARAM_32_BITS_NB,	//avant ce define, tout les paramètres tiennent sur 32 bits.

	//Paramètres dont la taille dépasse 32 bits... et dont le traitement est spécifiquement confié à l'objet par des fonctions de callback.
	PARAM_TEXT_PART0,
	PARAM_TEXT_PART1,
	PARAM_TEXT_PART2,
	PARAM_TEXT_PART3,
	PARAM_TEXT_PART4,
	PARAM_TEXT_PART5,
	PARAM_TEXT_PART6,
	PARAM_TEXT_PART7,
	PARAM_NB
}param_id_e;

typedef enum
{
	ALARM_WAY_MAX,	//le seuil est un maximum à ne pas dépasser
	ALARM_WAY_MIN,	//le seuil est un minimum à ne pas dépasser
	ALARM_WAY_DIFF, //le seuil est une valeur à ne pas quitter (pour un capteur tout ou rien ... !)
}alarm_way_e;



void PARAMETERS_init(void);

//chaque objet doit appeler cette fonction pour chacun de ses paramètres
void PARAMETERS_enable(param_id_e param_id, int32_t default_value, bool_e value_saved_in_flash, callback_fun_i32_t callback);

//cette fonction est appelée suite au fait que la station met à jour une valeur
void PARAMETERS_update(param_id_e param_id, int32_t new_value);

//cette fonction est appelée par l'objet qui met à jour une valeur (accessible sur demande par la station)
void PARAMETERS_set(param_id_e param_id, uint32_t value);

void PARAMETERS_update_custom(param_id_e param_id, uint8_t * datas);

void PARAMETERS_restore_from_flash(void);

void PARAMETERS_read_from_flash(param_id_e param_id);

//permet de récupérer la valeur d'un paramètre
int32_t PARAMETERS_get(param_id_e param_id);







#endif /* BSP_PARAMETERS_H_ */
