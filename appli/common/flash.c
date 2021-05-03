/*
 * flash.c
 *
 *  Created on: 16 jan. 2020
 *      Author: Nirgal
 */

#include "flash.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"

#define FLASH_ADDRESS_BEGIN	0x70000
#define FLASH_SIZE			0x10000
#define FLASH_ADDRESS_END	(FLASH_ADDRESS_BEGIN+FLASH_SIZE)


uint32_t err_code;
ret_code_t rc;
nrf_fstorage_api_t * p_fs_api;


void fs_event_handler(nrf_fstorage_evt_t * evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t m_fs) =
{
    .evt_handler = fs_event_handler,
    .start_addr = FLASH_ADDRESS_BEGIN,
    .end_addr = FLASH_ADDRESS_END,
};



void fs_event_handler(nrf_fstorage_evt_t * evt){
    if (evt->result != NRF_SUCCESS){


    } else {

    }
}


void FLASHWRITER_init(void)
{
	p_fs_api = &nrf_fstorage_nvmc;
	rc = nrf_fstorage_init(&m_fs, p_fs_api , 0);
}

/*
 * address doit aller de 0 à 0x1000, et être multiple de 4.
 */
uint32_t FLASHWRITER_read(uint32_t address)
{
	uint32_t  data_read = 0;
	if(address < FLASH_SIZE)
		rc = nrf_fstorage_read(&m_fs, FLASH_ADDRESS_BEGIN+address, &data_read, 4);
	return data_read;
}

/*
 * address doit aller de 0 à 0x1000, et être multiple de 4.
 */
running_e FLASHWRITER_write(uint32_t address, uint32_t value)
{
	if(address < FLASH_SIZE)
	{
		FLASHWRITER_erase(address);
		rc = nrf_fstorage_write(&m_fs, FLASH_ADDRESS_BEGIN+address, &value, 4, 0);
	}

	running_e ret = END_ERROR;
	if(FLASHWRITER_read(address) == value)
		ret = END_OK;
	return ret;
}

/*
 * address doit aller de 0 à 0x1000, et être multiple de 4.
 */
void FLASHWRITER_erase(uint32_t address){
	rc = nrf_fstorage_erase(&m_fs, FLASH_ADDRESS_BEGIN+address, 4, 0);
}



