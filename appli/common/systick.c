/*
 * systick.c
 *
 *  Created on: 28 janv. 2021
 *      Author: Nirgal
 */
#include "../config.h"
#include "systick.h"


#define MAX_CALLBACK_FUNCTION_NB	16

//Tableau de pointeurs sur fonctions qui doivent �tre appel�es p�riodiquement (1ms) par l'IT systick.
static callback_fun_t callback_functions[MAX_CALLBACK_FUNCTION_NB];
static bool_e initialized = FALSE;

void Systick_init(void)
{
	uint8_t i;
	for(i = 0; i<MAX_CALLBACK_FUNCTION_NB; i++)
		callback_functions[i] = NULL;
	SysTick_Config(SystemCoreClock / 1000);
	initialized = TRUE;
}

static volatile uint32_t absolute_time;

//Routine d'interruption appel�e automatiquement � chaque ms.
void SysTick_Handler(void)
{
	if(!initialized)
		Systick_init();
	absolute_time++;

	uint8_t i;
	for(i = 0; i<MAX_CALLBACK_FUNCTION_NB; i++)
	{
		if(callback_functions[i])
			(*callback_functions[i])();		//Appels des fonctions.
	}
}

//Ajout d'une fonction callback dans le tableau, si une place est disponible
bool_e Systick_add_callback_function(callback_fun_t func)
{
	uint8_t i;
	if(!initialized)
		Systick_init();

	for(i = 0; i<MAX_CALLBACK_FUNCTION_NB; i++)
	{
		if(!callback_functions[i])	//On a trouv� une place libre ?
		{
			callback_functions[i] = func;
			return TRUE;
		}
	}
	return FALSE;	//Pas de place libre !

}

//Retrait d'une fonction callback, si elle existe
bool_e Systick_remove_callback_function(callback_fun_t func)
{
	uint8_t i;
	if(!initialized)
		Systick_init();
	for(i = 0; i<MAX_CALLBACK_FUNCTION_NB; i++)
	{
		if(callback_functions[i] == func)	//On a trouv� la fonction � retirer ! ?
		{
			callback_functions[i] = NULL;
			return TRUE;
		}
	}
	return FALSE;	//On a pas trouv� la fonction � retirer
}

uint32_t SYSTICK_get_time_us(void)
{

	uint32_t t_us;
	uint32_t t_ms;
	__disable_irq();
	t_us = 1000 - SysTick->VAL / 64;
	t_ms = absolute_time;
	__enable_irq();

	return t_ms*1000 + t_us;
}

void SYSTICK_delay_ms(uint32_t duration)
{
	uint32_t local;

	local = absolute_time;
	while(absolute_time < local + duration);
}

void SYSTICK_delay_us(uint32_t duration)
{
	uint32_t local;
	local = SYSTICK_get_time_us();
	while(SYSTICK_get_time_us() < local + duration);
}
