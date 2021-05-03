/*
 * WS2812B_asm.s
 *
 *  Created on: 3 mai 2016
 *      Author: Samuel Poiraud
 *  Cette fonction est r�dig�e en assembleur pour respecter scrupuleusement les d�lais de production des signaux pour les leds de la matrice.
 */

 .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

  .data


  .section  .text.User_section
	.align 2

	  .global WS2812B_send_pixels
  WS2812B_send_pixels:
  //R0 : adresse du tableau de pixels (32 bits pour chaque led...)
  //R1 : pin	: c'est la valeur � inscrire dans les registres set_register et clear_register pour mettre � 1 ou � 0 le port demand�
  //R2 : Adresse du registre clear_register du GPIO concern�
  //R3 : Adresse du registre set_register du GPIO concern�
  //pile : nombre de leds.

	//R4 : bit � envoyer
	//R5 : compteur de boucle pour les d�lais
	//R6 : num�ro du bit envoy� dans le pixel.
	//R7 : nombre de leds
	//R8 : pixel courant
	//R9 : compteur de leds
  	MOV IP, SP
  	STMDB SP! ,{R4-R9}
    LDR R7, [IP]	//on charge le nombre de leds dans R7
    MOV R9, #0		//on commence � la led 0
	loop_leds:
		MOV R6, #23
		LDR R8, [R0, R9, LSL #2]	//on charge le pixel courant...
		loop:
			LSR R4, R8, R6
			ANDS R4, R4, #1
			BEQ write0
			write1:
				STR R1, [R3]	//mise � 1 du port
				NOP		//boucle �tat haut pour un 1 logique
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				STR R1, [R2]	//mise � 0 du port
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				B end_loop
			write0:
				STR R1, [R3]	//mise � 1 du port
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				STR R1, [R2]	//mise � 0 du port
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				NOP
				B end_loop
			end_loop:
			SUB R6, #1
			CMP R6, #-1
		BNE loop
	ADD R9, #1
	SUBS R7, #1
	BNE loop_leds

	LDMIA SP! ,{R4-R9}
	BX LR



