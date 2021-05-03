/* Lava
 * 
 * WS2812B Tricolor LED (neopixel) controller
 *
 *
 * Example code:
*/
 
#include "appli/config.h"
#include "components/libraries/delay/nrf_delay.h"
#include "nrfx_gpiote.h"
#include "modules/nrfx/mdk/nrf52.h"
#include "WS2812.h"

#ifndef MAX_NB_LEDS
	#define MAX_NB_LEDS		32
#endif

//Fonction définie en assembleur.
//extern void WS2812B_send_pixel(uint32_t pixel, uint32_t pin, uint32_t * clear_register, uint32_t * set_register);
extern void WS2812B_send_pixels(uint32_t * pixel, uint32_t pin, uint32_t * clear_register, uint32_t * set_register, uint32_t nb_leds);


typedef struct {
	uint32_t pin_id;
	uint16_t leds_number;
	uint32_t pixels[MAX_NB_LEDS];
} neopixel_strip_t;


static bool_e updated = FALSE;
static neopixel_strip_t strip;


void WS2812_init(uint32_t pin_num, uint16_t num_leds)
{
	strip.pin_id = pin_num;
	strip.leds_number = num_leds;
	nrf_gpio_cfg_output(pin_num);

	WS2812_clear();
	WS2812_refresh();
}



volatile uint32_t t_leds = 0;


/*
 * @brief	Cette fonction envoie des pixels vers la matrice de leds.
 * @note	les envois de pixels sont sous-traités à la fonction assembleur WS2812S_send_pixel
 * 			Cette fonction est rédigée en assembleur pour respecter scrupuleusement les délais de production des signaux pour les leds de la matrice.
 * 			Remarque : les interruptions sont désactivées temporairement pendant l'exécution de cette fonction pour éviter qu'elles provoquent des 'pauses' lors de la production des signaux.
 * 			La durée d'exécution de cette fonction est de l'ordre de 2,5ms. Durée pendant laquelle aucune interruption ne peut survenir !!!
 * @param 	pixels est un tableau de 64 cases absolument...
 * @note	attention, le tableau de pixels correspond aux leds dans l'ordre où elles sont câblées. Sur la matrice 8x8, elles sont reliées en serpentin ! (et non en recommancant à gauche à chaque nouvelle ligne)...
 */
void WS2812_refresh(void)
{
	//nrf_gpio_pin_clear(PIN);
	uint32_t pin;
	pin = 1UL<<strip.pin_id;
	uint32_t * pclr;
	uint32_t * pset;
	pclr = (uint32_t *)&NRF_P0->OUTCLR;
	pset = (uint32_t *)&NRF_P0->OUTSET;
	__disable_irq();
	//for(i=0;i<strip.leds_number;i++)
	//	WS2812B_send_pixel(strip.pixels[i], pin, pclr, pset);
	WS2812B_send_pixels(strip.pixels, pin, pclr, pset, strip.leds_number);
	__enable_irq();
}

void WS2812_wait_before_next_refresh(void)
{
	NRF_P0->OUTCLR = (1UL << strip.pin_id);
	nrf_delay_us(50);
}

void WS2812_clear(void)
{
	for(uint32_t i = 0; i < strip.leds_number; i++)
		strip.pixels[i] = COLOR_BLACK;
	updated = TRUE;
}



void WS2812_display_only_one_pixel(uint32_t pixel, uint32_t background, uint8_t rank)
{
	uint32_t i;
	for(i=0;i<strip.leds_number;i++)
		strip.pixels[i]=(i==rank)?pixel:background;
}

void WS2812_display_full(uint32_t pixel)
{
	uint32_t i;
	for(i=0;i<strip.leds_number;i++)
		strip.pixels[i]=pixel;
}

void WS2812_display_matrix(uint32_t * pixels, uint32_t size)
{
	uint32_t nb_leds;
	nb_leds = MIN(size, strip.leds_number);
	for(uint32_t i = 0; i<nb_leds; i++)
		strip.pixels[i]=pixels[i];
}


