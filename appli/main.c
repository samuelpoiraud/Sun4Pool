#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/unistd.h>

#include "config.h"
#include "main.h"

#include "components/libraries/util/sdk_common.h"
#include "nrf.h"
#include "nrf52.h"
#include "nrfx.h"
#include "components/libraries/log/nrf_log_ctrl.h"
#include "components/libraries/log/nrf_log_default_backends.h"
#include "nrf_gpio.h"
#include "common/macro_types.h"
#include "common/secretary.h"
#include "common/serial_dialog.h"
#include "common/systick.h"
#include "common/leds.h"
#include "common/buttons.h"
#include "common/gpio.h"
#include "common/parameters.h"

//Tout les includes des header des objets.
#include "objects/sun4pool.h"

void button_network_process_short_press(void);
void button_network_process_long_press(void);
void button_network_process_5press(void);

#undef NRF_LOG_ENABLED
#define NRF_LOG_ENABLED 1

void banque_de_fonctions_utiles(void)
{
	__unused uint32_t i;
	nrf_gpio_range_cfg_input(13, 16, GPIO_PIN_CNF_PULL_Pulldown);
	i = nrf_gpio_pin_read(16);
}

void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    NRF_NVMC->ICACHECNF = 1;	//Turn CPU cache ON.
}

void button_network_process(void)
{
	//LED_set_flash_limited_nb(LED_ID_NETWORK, 3, 200);
}

int main(void)
{
	//Dï¿½marrage de l'horloge.
    clocks_start();

    //Lancement du timer systick
    Systick_init();

    //Initialisation GPIO
    GPIO_init();

    BUTTONS_init();

    //Initialisation du SERIAL_DIALOG
#if USE_SERIAL_DIALOG
    SERIAL_DIALOG_init();	//initialise l'UART et permet les dialogues avec le PC de dï¿½bogage.
#endif

    //Fin de l'initialisation
    debug_printf("NRF52832 initialisé\n");
    volatile char id;
    id = OBJECT_ID;
    debug_printf("My id is %d. I am \"%s\"\n", id, object_id_to_string(id));

    PARAMETERS_init();

    //LED_add(LED_ID_NETWORK, PIN_LED_NETWORK);
	//LED_add(LED_ID_BATTERY, PIN_LED_BATTERY);
	//LED_set(LED_ID_BATTERY, LED_MODE_ON);


	SECRETARY_init();

	//BUTTONS_add(BUTTON_NETWORK, PIN_BUTTON_NETWORK, TRUE, &button_network_process_short_press, NULL, &button_network_process_long_press, &button_network_process_5press);

    while (1)
    {
    	//Code commun Ã  tout les objets
    	SECRETARY_process_main();

    	BUTTONS_process_main();

    	//Orientation du main vers chaque code de chaque objets
    		#if OBJECT_ID == OBJECT_BASE_STATION

    		#endif


    		#if OBJECT_ID == OBJECT_SUN4POOL
    			SUN4POOL_state_machine();

    		#endif
    }
}



char * object_id_to_string(uint8_t id)
{
	char * ret = "unknown object";

	switch(id)
	{
		case OBJECT_BASE_STATION:	ret = "Base Station";		break;
		case OBJECT_SUN4POOL:		ret = "Sun4Pool";		break;
		default:
			break;
	}
	return ret;
}



void button_network_process_short_press(void)
{
	//LED_toggle(LED_ID_NETWORK);
	//TODO envoi d'un ping... (avec ï¿½ventuellement extinction de la led ï¿½ la rï¿½ception du Pong ?)
}

void button_network_process_long_press(void)
{

}


void button_network_process_5press(void)
{
	// TODO reset usine...
}






