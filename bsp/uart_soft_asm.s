/*
 * uart_soft_asm.s
 *
 *  Created on: 22/02/2021
 *      Author: Samuel Poiraud
 *  Cette fonction est rédigée en assembleur pour respecter scrupuleusement les délais de production des signaux UART
 */

 .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

  .data


  .section  .text.User_section
	.align 2

	  .global UART_SOFT_asm_putc
  UART_SOFT_asm_putc:
  //R0 : caractère à transmettre (8 bits...)
  //R1 : pin	: c'est la valeur à inscrire dans les registres set_register et clear_register pour mettre à 1 ou à 0 le port demandé
  //R2 : Adresse du registre clear_register du GPIO concerné
  //R3 : Adresse du registre set_register du GPIO concerné

	//R4 : bit à envoyer
	//R5 : compteur de boucle pour les délais
	//R6 : numéro du prochain bit à envoyer dans l'octet (1 bit start + 8 bits de données + 1 bit de stop)
	//R7 : compteur du délai
  	STMDB SP! ,{R4-R7}
		MOV R6, #0		//Le prochain bit à envoyer sera le bit 0 (LSB first)
		STR R1, [R2]	//mise à 0 du port pendant le bit de start
		B delay_bit		//on va attendre durant l'envoi du bit de start

		loop:
			LSR R4, R0, R6			//on extrait le bit de poids R6
			ANDS R4, R4, #1			//on récupère ce bit
			ADD R6, #1				//on flèche le bit suivant pour le prochain passage
			BEQ write0				//si le bit est un 0, on va mettre le port à 0
			write1:
				STR R1, [R3]	//mise à 1 du port
				B delay_bit

			write0:
				STR R1, [R2]	//mise à 0 du port
				B delay_bit

			delay_bit:
				//boucle d'attente
				LDR R7,=#175
				delay_loop:
					SUBS R7, #1
					BNE delay_loop

			CMP R6, #8	//on compare le prochain bit à envoyer avec la valeur 8
			BLT loop	//s'il s'agit de l'un des bits n°0 à 7, on va dans loop pour envoyer le bit concerné

			STR R1, [R3]	//mise à 1 du port
			ADD R6, #1
			BEQ	delay_bit	//si le prochain bit vaut 8, c'est qu'on doit envoyer un bit de stop, on retourne dans le delay

			//Sinon, on a fini..!

	LDMIA SP! ,{R4-R7}
	BX LR



