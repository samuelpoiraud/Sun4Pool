/*
 * nrf52_i2c.c
 *
 *  Created on: 9 févr. 2021
 *      Author: Nirgal
 */
#include "appli/config.h"
#include "modules/nrfx/drivers/include/nrfx_twi.h"
#include "appli/common/systick.h"
#if USE_TWI
static uint8_t m_device_address;          // !< Device address in bits [7:1]
static volatile uint32_t t = 0;
static volatile nrfx_twi_evt_type_t event = FALSE;
static volatile bool_e flag_event = FALSE;
static volatile bool_e initialized = FALSE;
static const nrfx_twi_t m_twi = NRFX_TWI_INSTANCE(TWI_INSTANCE_ID);

void twi_handler(nrfx_twi_evt_t const * p_event, void * p_context);


void I2C_process_ms(void)
{
	if(t)
		t--;
}

void I2C_init(uint8_t device_address)
{
	bool transfer_succeeded = true;
	ret_code_t err_code;
	m_device_address = (uint8_t)(device_address);   // init "m_device_address"  pour les autres fonctions

	// Reinitialize it in managed non-blocking mode.
	// Initialize the manager.
	nrfx_twi_config_t const config_async = { // TODO the config should move to the nrfx namespace instead of nrfx
		.scl                = I2C_SCL_PIN_NB,
		.sda                = I2C_SDA_PIN_NB,
		.frequency          = NRF_TWI_FREQ_400K,
		.interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
		.hold_bus_uninit     = false
	};

    err_code = nrfx_twi_init(&m_twi, &config_async, &twi_handler, NULL);
    if(err_code == NRF_SUCCESS)
    	nrfx_twi_enable(&m_twi);

	Systick_add_callback_function(&I2C_process_ms);

	initialized = TRUE;
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrfx_twi_evt_t const * p_event, void * p_context)
{
    flag_event = TRUE;
    event = p_event->type;
    switch (p_event->type)
    {
        case NRFX_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRFX_TWI_XFER_RX)
            {

            }
            break;
        case NRFX_TWI_EVT_ADDRESS_NACK:
        	break;
        case NRFX_TWI_EVT_DATA_NACK:
        	break;
        default:
            break;
    }
}


running_e I2C_register_read(uint8_t register_address, uint8_t * destination, uint8_t number_of_bytes)
{
    typedef enum
    {
    	INIT = 0,
		IDLE,
		TX,
		RX,
		TIMEOUT
    }state_e;
    static state_e state = INIT;
    static state_e previous_state = INIT;
    bool_e entrance;
    entrance = (previous_state != state)?TRUE:FALSE;
    previous_state = state;

	running_e ret = IN_PROGRESS;
    ret_code_t err_code;

	switch(state)
	{
		case INIT:
			if(initialized)
				state = IDLE;
			else
				ret = END_ERROR;
			break;
		case IDLE:
			state = TX;
		break;
		case TX:
			if(entrance)
			{
				t = 100;
				flag_event = FALSE;
			 	err_code = nrfx_twi_tx(&m_twi, m_device_address, &register_address, 1, true);
				if(err_code != NRF_SUCCESS)
				{
					ret = END_ERROR;
					state = IDLE;
				}
			}
			if(flag_event)
				state = RX;
			else if(!t)
				state = TIMEOUT;
			break;
		case RX:
			if(entrance)
			{
				flag_event = FALSE;
				err_code = nrfx_twi_rx(&m_twi, m_device_address, destination, number_of_bytes);
				t = 100;
				if(err_code != NRF_SUCCESS)
				{
					state = IDLE;
					ret = END_ERROR;
				}
			}

			if(flag_event)
			{
				state = IDLE;
				ret = END_OK;
			}
			else if(!t)
				state = TIMEOUT;
			break;
		case TIMEOUT:
			ret = END_TIMEOUT;
			state = IDLE;
			break;

		default:
			break;
	}

    return ret;
}


running_e I2C_register_write(uint8_t register_address, uint8_t value)
{
	typedef enum
	{
		INIT = 0,
		IDLE,
		TX,
		RX,
		TIMEOUT
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (previous_state != state)?TRUE:FALSE;
	previous_state = state;

	running_e ret = IN_PROGRESS;
	ret_code_t err_code;

	switch(state)
	{
		case INIT:
			//if(initialized)
			state = IDLE;
			break;
		case IDLE:
			state = TX;
		break;
		case TX:
			if(entrance)
			{
				t = 100;
				uint8_t reg[2] = {register_address, value};
				flag_event = FALSE;
				err_code = nrfx_twi_tx(&m_twi, m_device_address, reg, sizeof(reg), false);
				if(err_code != NRF_SUCCESS)
				{
					ret = END_ERROR;
					state = IDLE;
				}
			}
			if(flag_event)
			{
				state = IDLE;
				ret = END_OK;
			}
			else if(!t)
				state = TIMEOUT;
			break;
		case TIMEOUT:
			ret = END_TIMEOUT;
			state = IDLE;
			break;

		default:
			break;
	}

	return ret;
}



running_e I2C_write(uint8_t * data, uint8_t size)
{
	typedef enum
	{
		INIT = 0,
		IDLE,
		TX,
		RX,
		TIMEOUT
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (previous_state != state)?TRUE:FALSE;
	previous_state = state;

	running_e ret = IN_PROGRESS;
	ret_code_t err_code;

	switch(state)
	{
		case INIT:
			//if(initialized)
			state = IDLE;
			break;
		case IDLE:
			state = TX;
		break;
		case TX:
			if(entrance)
			{
				t = 100;
				flag_event = FALSE;
				err_code = nrfx_twi_tx(&m_twi, m_device_address, data, size, false);
				if(err_code != NRF_SUCCESS)
				{
					ret = END_ERROR;
					state = IDLE;
				}
			}
			if(flag_event)
			{
				state = IDLE;
				ret = END_OK;
			}
			else if(!t)
				state = TIMEOUT;
			break;
		case TIMEOUT:
			ret = END_TIMEOUT;
			state = IDLE;
			break;

		default:
			break;
	}

	return ret;
}



running_e I2C_read(uint8_t * data, uint8_t size)
{
    typedef enum
    {
    	INIT = 0,
		IDLE,
		TX,
		RX,
		TIMEOUT
    }state_e;
    static state_e state = INIT;
    static state_e previous_state = INIT;
    bool_e entrance;
    entrance = (previous_state != state)?TRUE:FALSE;
    previous_state = state;

	running_e ret = IN_PROGRESS;
    ret_code_t err_code;

	switch(state)
	{
		case INIT:
			if(initialized)
				state = IDLE;
			else
				ret = END_ERROR;
			break;
		case IDLE:
			state = RX;
		break;
		case RX:
			if(entrance)
			{
				flag_event = FALSE;
				err_code = nrfx_twi_rx(&m_twi, m_device_address, data, size);
				t = 100;
				if(err_code != NRF_SUCCESS)
				{
					state = IDLE;
					ret = END_ERROR;
				}
			}

			if(flag_event)
			{
				state = IDLE;
				ret = END_OK;
			}
			else if(!t)
				state = TIMEOUT;
			break;
		case TIMEOUT:
			ret = END_TIMEOUT;
			state = IDLE;
			break;

		default:
			break;
	}

    return ret;
}

#endif
