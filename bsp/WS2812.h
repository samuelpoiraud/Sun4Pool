 #ifndef NEOPIXEL_H
 #define NEOPIXEL_H


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#define COLOR_BLUE	0x0000FF
#define COLOR_RED	0x00FF00
#define COLOR_GREEN	0xFF0000
#define COLOR_BLACK	0x000000
#define COLOR_WHITE	0xFFFFFF
#define COLOR_LOW_WHITE	0x7F7F7F

#define COLOR_YELLOW 0xFFFF00



void WS2812_init(uint32_t pin_num, uint16_t num_leds);


/*
 * @brief	Cette fonction envoie des pixels vers la matrice de leds.
 * @note	les envois de pixels sont sous-traités à la fonction assembleur WS2812S_send_pixel
 * 			Cette fonction est rédigée en assembleur pour respecter scrupuleusement les délais de production des signaux pour les leds de la matrice.
 * 			Remarque : les interruptions sont désactivées temporairement pendant l'exécution de cette fonction pour éviter qu'elles provoquent des 'pauses' lors de la production des signaux.
 * 			La durée d'exécution de cette fonction est de l'ordre de 2,5ms. Durée pendant laquelle aucune interruption ne peut survenir !!!
 * @param 	pixels est un tableau de 64 cases absolument...
 * @note	attention, le tableau de pixels correspond aux leds dans l'ordre où elles sont câblées. Sur la matrice 8x8, elles sont reliées en serpentin ! (et non en recommancant à gauche à chaque nouvelle ligne)...
 */
void WS2812_refresh(void);

void WS2812_wait_before_next_refresh(void);

void WS2812_clear(void);


void WS2812_display_only_one_pixel(uint32_t pixel, uint32_t background, uint8_t rank);

void WS2812_display_full(uint32_t pixel);

void WS2812_display_matrix(uint32_t * pixels, uint32_t size);

#endif
