/*
 * RC522.C
 *
 *  Created on: 11 févr. 2021
 *      Author: paulq
 */

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "appli/common/gpio.h"
#include "appli/common/systick.h"
#include "RC522.h"
#include <string.h>
#include "app_error.h"

#if OBJECT_ID == OBJECT_RFID
#if USE_RC522
#include "nrf52_spi.h"

static uint8_t m_tx_buf[2];// = TEST_STRING; /**< TX buffer. */
static uint8_t m_rx_buf[1]; /**< RX buffer. */
static uint8_t m_write_buf[1]; /**< RX buffer. */
static const uint8_t m_length = 1; /**< Transfer length. */
static const uint8_t m_tx_length = 2; /**< Transfer length. */


void mfrc522_write(uint8_t reg, uint8_t data)
{
	uint8_t data_to_send[2];
	data_to_send[0] = ((reg<<1)&0x7E);	//on envoie l'adresse du registre que l'on souhaite écrire, décalée au milieu de l'octet. Le bit de poids fort à 0 indique qu'on souhaite écrire !
	data_to_send[1] = data;

	SPI_write(data_to_send, 2);
}

uint8_t mfrc522_read(uint8_t reg)
{
	uint8_t data_to_send[2];
	data_to_send[0] = ((reg<<1)&0x7E)|0x80;	//on envoie l'adresse du registre que l'on souhaite lire, décalée au milieu de l'octet. Le bit de poids fort indique qu'on souhaite lire !
	data_to_send[1] = 0x00;
	uint8_t data_to_read[2];

	SPI_xfer(data_to_send, 2, data_to_read, 2);

	return data_to_read[1];	//on retourne l'octet lu lors du second échange.
}


/*
static ret_code_t hardware_init(void)
{
    ret_code_t err_code;


    nrf_gpio_cfg_output(RC522_CS_PIN);

    nrfx_spi_config_t spi_config = NRFX_SPI_DEFAULT_CONFIG;

    spi_config.sck_pin  = RC522_SCK_PIN;
    spi_config.miso_pin = RC522_MISO_PIN;
    spi_config.mosi_pin = RC522_MOSI_PIN;
    spi_config.ss_pin   = RC522_CS_PIN;

    err_code = nrfx_spi_init(&spi, &spi_config, NULL, NULL);
}


void PCD_WriteRegister(PCD_Register reg, byte value){
        nrf_gpio_pin_write(RC522_CS_PIN,0);
        m_tx_buf[0] = reg &0x7E;
        m_tx_buf[1] = value;
        memset(m_rx_buf, 0, m_length);
        SPI_xfer(&m_tx_buf[0], m_length, NULL, 0);
        nrf_gpio_pin_write(RC522_CS_PIN,1);
}
void PCD_WriteRegister_long(PCD_Register reg, byte count, byte *values){
		nrf_gpio_pin_write(RC522_CS_PIN,0);
        m_tx_buf[0] = reg & 0x7E;
        memset(m_rx_buf, 0, m_length);

        SPI_xfer(&m_tx_buf[0], m_length, NULL, 0);


	for (byte index = 0; index < count; index++) {
	  memset(m_rx_buf, 0, m_length);
         // debug_info("long value[%d]:%x",index,values[index]);
          SPI_xfer(&values[index], m_length, NULL, 0);

	}

nrf_gpio_pin_write(RC522_CS_PIN,1);
}
byte PCD_ReadRegister(PCD_Register reg){
        nrf_gpio_pin_write(RC522_CS_PIN,0);
        m_tx_buf[0] = (0x80 | (reg & 0x7E));
        memset(m_rx_buf, 0, m_length);
        SPI_xfer(&m_tx_buf[0], m_length, m_rx_buf, m_length);

        memset(m_rx_buf, 0, m_length);
        uint8_t _reg;
        _reg = 0x00;
        SPI_xfer(&_reg, m_length, m_rx_buf, m_length);


        nrf_gpio_pin_write(RC522_CS_PIN,1);
        return m_rx_buf[0];

}
void PCD_ReadRegister_long(PCD_Register reg, byte count, byte *values, byte rxAlign){
    nrf_gpio_pin_write(RC522_CS_PIN,0);
	if (count == 0) {
		return;
	}
        byte address = 0x80 | (reg& 0x7E);				// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	    byte index = 0;
        count--;

        memset(m_rx_buf, 0, m_length);

        SPI_xfer(&address, m_length, m_rx_buf, m_length);

        if (rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
		// Create bit mask for bit positions rxAlign..7
		byte mask = (0xFF << rxAlign) & 0xFF;
		// Read value and tell that we want to read the same address again.
        memset(m_rx_buf, 0, m_length);
        SPI_xfer(&address, m_length, m_rx_buf, m_length);
        byte value = m_rx_buf[0];
		// Apply mask to both current value of values[0] and the new data in value.
		values[0] = (values[0] & ~mask) | (value & mask);
		index++;
	}

        while (index < count) {
        memset(m_rx_buf, 0, m_length);
        SPI_xfer(&address, m_length, m_rx_buf, m_length);
        values[index] = m_rx_buf[0];
        index++;
	}
        memset(m_rx_buf, 0, m_length);

        uint8_t _reg = 0x00;
        SPI_xfer(&_reg, m_length, m_rx_buf, m_length);
        values[index] = m_rx_buf[0];
        nrf_gpio_pin_write(RC522_CS_PIN,1);
}


*/

void mfrc522_init()
{
	uint8_t byte;
	mfrc522_reset();

	mfrc522_write(TModeReg, 0x8D);
    mfrc522_write(TPrescalerReg, 0x3E);
    mfrc522_write(TReloadReg_1, 30);
    mfrc522_write(TReloadReg_2, 0);
	mfrc522_write(TxASKReg, 0x40);
	mfrc522_write(ModeReg, 0x3D);

	//Dump sauvage à des fins de débogage...
	for(uint8_t reg = 0 ; reg<=Page3_Reserved_5; reg++)
	{
		debug_printf("[0x%02x]=[0x%02x]\n",reg, mfrc522_read(reg));
	}


	byte = mfrc522_read(TxControlReg);
	if(!(byte&0x03))
	{
		mfrc522_write(TxControlReg,byte|0x03);
	}
}



void mfrc522_reset()
{
	//mfrc522_write(CommandReg,SoftReset_CMD);
}

uint8_t	mfrc522_request(uint8_t req_mode, uint8_t * tag_type)
{
	uint8_t  status;
	uint32_t backBits;//The received data bits

	mfrc522_write(BitFramingReg, 0x07);//TxLastBists = BitFramingReg[2..0]	???

	tag_type[0] = req_mode;
	status = mfrc522_to_card(Transceive_CMD, tag_type, 1, tag_type, &backBits);

	if ((status != CARD_FOUND) || (backBits != 0x10))
	{
		status = ERROR;
	}

	return status;
}

uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len)
{
	uint8_t status = ERROR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint8_t	tmp;
    uint32_t i;

    switch (cmd)
    {
        case MFAuthent_CMD:		//Certification cards close
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case Transceive_CMD:	//Transmit FIFO data
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }

    //mfrc522_write(ComIEnReg, irqEn|0x80);	//Interrupt request
    n=mfrc522_read(ComIrqReg);
    mfrc522_write(ComIrqReg,n&(~0x80));//clear all interrupt bits
    n=mfrc522_read(FIFOLevelReg);
    mfrc522_write(FIFOLevelReg,n|0x80);//flush FIFO data

	mfrc522_write(CommandReg, Idle_CMD);	//NO action; Cancel the current cmd???

	//Writing data to the FIFO
    for (i=0; i<send_data_len; i++)
    {
		mfrc522_write(FIFODataReg, send_data[i]);
	}

	//Execute the cmd
	mfrc522_write(CommandReg, cmd);
    if (cmd == Transceive_CMD)
    {
		n=mfrc522_read(BitFramingReg);
		mfrc522_write(BitFramingReg,n|0x80);
	}

	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
    do
    {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = mfrc522_read(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	tmp=mfrc522_read(BitFramingReg);
	mfrc522_write(BitFramingReg,tmp&(~0x80));

    if (i != 0)
    {
        if(!(mfrc522_read(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = CARD_FOUND;
            if (n & irqEn & 0x01)
            {
				status = CARD_NOT_FOUND;			//??
			}

            if (cmd == Transceive_CMD)
            {
               	n = mfrc522_read(FIFOLevelReg);
              	lastBits = mfrc522_read(ControlReg) & 0x07;
                if (lastBits)
                {
					*back_data_len = (n-1)*8 + lastBits;
				}
                else
                {
					*back_data_len = n*8;
				}

                if (n == 0)
                {
					n = 1;
				}
                if (n > MAX_LEN)
                {
					n = MAX_LEN;
				}

				//Reading the received data in FIFO
                for (i=0; i<n; i++)
                {
					back_data[i] = mfrc522_read(FIFODataReg);
				}
            }
        }
        else
        {
			status = ERROR;
		}

    }

    //SetBitMask(ControlReg,0x80);           //timer stops
    //mfrc522_write(cmdReg, PCD_IDLE);

    return status;
}


uint8_t mfrc522_get_card_serial(uint8_t * serial_out)
{
	uint8_t status;
    uint8_t i;
	uint8_t serNumCheck=0;
    uint32_t unLen;

	mfrc522_write(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serial_out[0] = PICC_ANTICOLL;
    serial_out[1] = 0x20;
    status = mfrc522_to_card(Transceive_CMD, serial_out, 2, serial_out, &unLen);

    if (status == CARD_FOUND)
	{
		//Check card serial number
		for (i=0; i<4; i++)
		{
		 	serNumCheck ^= serial_out[i];
		}
		if (serNumCheck != serial_out[i])
		{
			status = ERROR;
		}
    }
    return status;
}


#endif

#endif
