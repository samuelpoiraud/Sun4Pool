#ifndef MFRC522_H
#define MFRC522_H

#include "appli/config.h"
/*
typedef uint8_t byte;
typedef uint8_t PCD_Register;
typedef uint8_t PCD_Command;
typedef uint8_t PCD_RxGain;
typedef uint8_t PICC_Command;
typedef uint8_t PICC_Type;
typedef uint8_t StatusCode;
typedef uint8_t MIFARE_Misc;
		// Page 0: Command and status
		//						  0x00			// reserved for future use
#define		CommandReg				(0x01 << 1)// starts and stops command execution
#define		ComIEnReg				(0x02 << 1)// enable and disable interrupt request control bits
#define		DivIEnReg				(0x03 << 1)// enable and disable interrupt request control bits
#define		ComIrqReg				(0x04 << 1)// interrupt request bits
#define		DivIrqReg				(0x05 << 1)// interrupt request bits
#define		ErrorReg				(0x06 << 1)// error bits showing the error status of the last command executed
#define		Status1Reg				(0x07 << 1)// communication status bits
#define		Status2Reg				(0x08 << 1)// receiver and transmitter status bits
#define		FIFODataReg				(0x09 << 1)// input and output of 64 byte FIFO buffer
#define		FIFOLevelReg                            (0x0A << 1)// number of bytes stored in the FIFO buffer
#define		WaterLevelReg                           (0x0B << 1)// level for FIFO underflow and overflow warning
#define		ControlReg				(0x0C << 1)// miscellaneous control registers
#define		BitFramingReg                           (0x0D << 1)// adjustments for bit-oriented frames
#define		CollReg					(0x0E << 1)// bit position of the first bit-collision detected on the RF interface
		//						  0x0F			// reserved for future use

		// Page 1: Command
		// 						  0x10			// reserved for future use
#define		ModeReg					(0x11 << 1)// defines general modes for transmitting and receiving
#define		TxModeReg				(0x12 << 1)// defines transmission data rate and framing
#define		RxModeReg				(0x13 << 1)// defines reception data rate and framing
#define		TxControlReg                            (0x14 << 1)// controls the logical behavior of the antenna driver pins TX1 and TX2
#define		TxASKReg				(0x15 << 1)// controls the setting of the transmission modulation
#define		TxSelReg				(0x16 << 1)// selects the internal sources for the antenna driver
#define		RxSelReg				(0x17 << 1)// selects internal receiver settings
#define		RxThresholdReg                          (0x18 << 1)// selects thresholds for the bit decoder
#define		DemodReg				(0x19 << 1)// defines demodulator settings
		// 						  0x1A			// reserved for future use
		// 						  0x1B			// reserved for future use
#define		MfTxReg					(0x1C << 1)// controls some MIFARE communication transmit parameters
#define		MfRxReg					(0x1D << 1)// controls some MIFARE communication receive parameters
		// 						  0x1E			// reserved for future use
#define		SerialSpeedReg                          (0x1F << 1)// selects the speed of the serial UART interface

		// Page 2: Configuration
		// 						  0x20			// reserved for future use
#define		CRCResultRegH                           (0x21 << 1)// shows the MSB and LSB values of the CRC calculation
#define		CRCResultRegL                           (0x22 << 1)
		// 						  0x23			// reserved for future use
#define		ModWidthReg				(0x24 << 1)// controls the ModWidth setting?
		// 						  0x25			// reserved for future use
#define		RFCfgReg				(0x26 << 1)// configures the receiver gain
#define		GsNReg					(0x27 << 1)// selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define		CWGsPReg				(0x28 << 1)// defines the conductance of the p-driver output during periods of no modulation
#define		ModGsPReg				(0x29 << 1)// defines the conductance of the p-driver output during periods of modulation
#define		TModeReg				(0x2A << 1)// defines settings for the internal timer
#define		TPrescalerReg                           (0x2B << 1)// the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
#define         TReloadRegH                             (0x2C << 1)//1 defines the 16-bit timer reload value
#define		TReloadRegL				(0x2D << 1)//2
#define		TCounterValueRegH                       (0x2E << 1)// shows the 16-bit timer value
#define		TCounterValueRegL                       (0x2F << 1)

		// Page 3: Test Registers
		// 						  0x30			// reserved for future use
#define		TestSel1Reg				(0x31 << 1)// general test signal configuration
#define		TestSel2Reg				(0x32 << 1)// general test signal configuration
#define		TestPinEnReg                            (0x33 << 1)// enables pin output driver on pins D1 to D7
#define		TestPinValueReg                         (0x34 << 1)// defines the values for D1 to D7 when it is used as an I/O bus
#define		TestBusReg				(0x35 << 1)// shows the status of the internal test bus
#define		AutoTestReg				(0x36 << 1)// controls the digital self-test
#define		VersionReg				(0x37 << 1)// shows the software version
#define		AnalogTestReg                           (0x38 << 1)// controls the pins AUX1 and AUX2
#define		TestDAC1Reg				(0x39 << 1)// defines the test value for TestDAC1
#define		TestDAC2Reg				(0x3A << 1)// defines the test value for TestDAC2
#define		TestADCReg				(0x3B << 1		// shows the value of ADC I and Q channels
		// 						  0x3C			// reserved for production tests
		// 						  0x3D			// reserved for production tests
		// 						  0x3E			// reserved for production tests
		// 						  0x3F			// reserved for production tests


#define		PCD_Idle				(0x00)	// no action, cancels current command execution
#define         PCD_Mem					(0x01)	// stores 25 bytes into the internal buffer
#define		PCD_GenerateRandomID                    (0x02)	// generates a 10-byte random ID number
#define		PCD_CalcCRC				(0x03)	// activates the CRC coprocessor or performs a self-test
#define		PCD_Transmit                            (0x04)	// transmits data from the FIFO buffer
#define		PCD_NoCmdChange                         (0x07)	// no command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit
#define		PCD_Receive				(0x08)	// activates the receiver circuits
#define		PCD_Transceive                          (0x0C)	// transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define		PCD_MFAuthent                           (0x0E)	// performs the MIFARE standard authentication as a reader
#define		PCD_SoftReset                           (0x0F)		// resets the MFRC522



#define		RxGain_18dB				(0x00 << 4)// 000b - 18 dB, minimum
#define		RxGain_23dB				(0x01 << 4)// 001b - 23 dB
#define		RxGain_18dB_2                           (0x02 << 4)// 010b - 18 dB, it seems 010b is a duplicate for 000b
#define		RxGain_23dB_2                           (0x03 << 4)// 011b - 23 dB, it seems 011b is a duplicate for 001b
#define		RxGain_33dB				(0x04 << 4)// 100b - 33 dB, average, and typical default
#define		RxGain_38dB				(0x05 << 4)// 101b - 38 dB
#define		RxGain_43dB				(0x06 << 4)// 110b - 43 dB
#define		RxGain_48dB				(0x07 << 4)// 111b - 48 dB, maximum
#define		RxGain_min				(0x00 << 4)// 000b - 18 dB, minimum, convenience for RxGain_18dB
#define		RxGain_avg				(0x04 << 4)// 100b - 33 dB, average, convenience for RxGain_33dB
#define		RxGain_max				(0x07 << 4		// 111b - 48 dB, maximum, convenience for RxGain_48dB

#define         PICC_CMD_REQA			(0x26)	// REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
#define		PICC_CMD_WUPA			(0x52)	// Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
#define		PICC_CMD_CT			(0x88)	// Cascade Tag. Not really a command, but used during anti collision.
#define		PICC_CMD_SEL_CL1		(0x93)	// Anti collision/Select, Cascade Level 1
#define		PICC_CMD_SEL_CL2		(0x95)	// Anti collision/Select, Cascade Level 2
#define		PICC_CMD_SEL_CL3		(0x97)	// Anti collision/Select, Cascade Level 3
#define		PICC_CMD_HLTA			(0x50)	// HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
#define		PICC_CMD_RATS           (0xE0,     // Request command for Answer To Reset.
// The commands used for MIFARE Classic (from http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Section 9)
// Use PCD_MFAuthent to authenticate access to a sector, then use these commands to read/write/modify the blocks on the sector.
// The read/write commands can also be used for MIFARE Ultralight.
#define		PICC_CMD_MF_AUTH_KEY_A	(0x60)	// Perform authentication with Key A
#define		PICC_CMD_MF_AUTH_KEY_B	(0x61)	// Perform authentication with Key B
#define		PICC_CMD_MF_READ		(0x30)	// Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
#define		PICC_CMD_MF_WRITE		(0xA0)	// Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
#define		PICC_CMD_MF_DECREMENT	(0xC0)	// Decrements the contents of a block and stores the result in the internal data register.
#define	PICC_CMD_MF_INCREMENT           (0xC1)	// Increments the contents of a block and stores the result in the internal data register.
#define	PICC_CMD_MF_RESTORE		(0xC2)	// Reads the contents of a block into the internal data register.
#define	PICC_CMD_MF_TRANSFER            (0xB0)	// Writes the contents of the internal data register to a block.
        // The commands used for MIFARE Ultralight (from http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 8.6)
        // The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE Ultralight.
#define	PICC_CMD_UL_WRITE		(0xA2		// Writes one 4 byte page to the PICC.

#define PICC_TYPE_UNKNOWN		(0x00)
#define	PICC_TYPE_ISO_14443_4           (0x01)// PICC compliant with ISO/IEC 14443-4
#define	PICC_TYPE_ISO_18092		(0x02) 	// PICC compliant with ISO/IEC 18092 (NFC)
#define	PICC_TYPE_MIFARE_MINI           (0x03)// MIFARE Classic protocol, 320 bytes
#define	PICC_TYPE_MIFARE_1K		(0x04)// MIFARE Classic protocol, 1KB
#define	PICC_TYPE_MIFARE_4K		(0x05)// MIFARE Classic protocol, 4KB
#define	PICC_TYPE_MIFARE_UL		(0x06)// MIFARE Ultralight or Ultralight C
#define	PICC_TYPE_MIFARE_PLUS           (0x07)// MIFARE Plus
#define	PICC_TYPE_MIFARE_DESFIRE        (0x08)// MIFARE DESFire
#define	PICC_TYPE_TNP3XXX		(0x09)// Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
#define	PICC_TYPE_NOT_COMPLETE          (0xff)	// SAK indicates UID is not complete.

#define	STATUS_OK			(0x00)// Success
#define	STATUS_ERROR			(0x01)// Error in communication
#define	STATUS_COLLISION		(0x02)// Collission detected
#define	STATUS_TIMEOUT			(0x03)// Timeout in communication.
#define	STATUS_NO_ROOM			(0x04)// A buffer is not big enough.
#define	STATUS_INTERNAL_ERROR           (0x05)// Internal error in the code. Should not happen ;-)
#define	STATUS_INVALID			(0x06)// Invalid argument.
#define	STATUS_CRC_WRONG		(0x07)// The CRC_A does not match
#define	STATUS_MIFARE_NACK		(0xff)	// A MIFARE PICC responded with NAK.

#define		MF_ACK					(0xA)		// The MIFARE Classic uses a 4 bit ACK/NAK. Any other value than 0xA is NAK.
#define		MF_KEY_SIZE				(6)			// A Mifare Crypto1 key is 6 bytes.
// A struct used for passing the UID of a PICC.
typedef struct {
        byte		size;			// Number of bytes in the UID. 4, 7 or 10.
        byte		uidByte[10];
        byte		sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} Uid;

// A struct used for passing a MIFARE Crypto1 key
typedef struct {
        byte		keyByte[MF_KEY_SIZE];
} MIFARE_Key;
/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////
void PCD_WriteRegister(PCD_Register reg, byte value);
void PCD_WriteRegister_long(PCD_Register reg, byte count, byte *values);
byte PCD_ReadRegister(PCD_Register reg);
void PCD_ReadRegister_long(PCD_Register reg, byte count, byte *values, byte rxAlign); //TODO rxAlign = 0
void PCD_SetRegisterBitMask(PCD_Register reg, byte mask);
void PCD_ClearRegisterBitMask(PCD_Register reg, byte mask);
StatusCode PCD_CalculateCRC(byte *data, byte length, byte *result);
/////////////////////////////////////////////////////////////////////////////////////
// Functions for manipulating the MFRC522
/////////////////////////////////////////////////////////////////////////////////////
void PCD_Init();
void PCD_Reset();
void PCD_AntennaOn();
void PCD_AntennaOff();
byte PCD_GetAntennaGain();
void PCD_SetAntennaGain(byte mask);
bool PCD_PerformSelfTest();

/////////////////////////////////////////////////////////////////////////////////////
// Power control functions
/////////////////////////////////////////////////////////////////////////////////////
void PCD_SoftPowerDown();
void PCD_SoftPowerUp();
/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with PICCs
/////////////////////////////////////////////////////////////////////////////////////
StatusCode PCD_TransceiveData(byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC); // TODO eclip
StatusCode PCD_CommunicateWithPICC(byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC); //TODO eclip
StatusCode PICC_RequestA(byte *bufferATQA, byte *bufferSize);
StatusCode PICC_WakeupA(byte *bufferATQA, byte *bufferSize);
StatusCode PICC_REQA_or_WUPA(byte command, byte *bufferATQA, byte *bufferSize);
StatusCode PICC_Select(Uid *uid, byte validBits); //TODO eclip
StatusCode PICC_HaltA();

/////////////////////////////////////////////////////////////////////////////////////
// Convenience functions - does not add extra functionality
/////////////////////////////////////////////////////////////////////////////////////
bool PICC_IsNewCardPresent();
bool PICC_ReadCardSerial();
*/
#include "appli/config.h"
#include "nrfx.h"
#include "nrfx_spi.h"

#if USE_RC522

static   void spi_event_handler(nrfx_spi_evt_t const * p_event,void *p_context);
static   uint8_t spi_transmit(uint8_t data);
static   void mfrc522_write(uint8_t reg, uint8_t data);
static   uint8_t mfrc522_read(uint8_t reg);
void mfrc522_init();
void mfrc522_reset();






/*
 * mfrc522.h
 *
 * Copyright 2013 Shimon <shimon@monistit.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdint.h>


//command set
#define Idle_CMD 				0x00
#define Mem_CMD					0x01
#define GenerateRandomId_CMD	0x02
#define CalcCRC_CMD				0x03
#define Transmit_CMD			0x04
#define NoCmdChange_CMD			0x07
#define Receive_CMD				0x08
#define Transceive_CMD			0x0C
#define Reserved_CMD			0x0D
#define MFAuthent_CMD			0x0E
#define SoftReset_CMD			0x0F


//Page 0 ==> Command and Status
#define Page0_Reserved_1 	0x00
#define CommandReg			0x01
#define ComIEnReg			0x02
#define DivIEnReg			0x03
#define ComIrqReg			0x04
#define DivIrqReg			0x05
#define ErrorReg			0x06
#define Status1Reg			0x07
#define Status2Reg			0x08
#define FIFODataReg			0x09
#define FIFOLevelReg		0x0A
#define WaterLevelReg		0x0B
#define ControlReg			0x0C
#define BitFramingReg		0x0D
#define CollReg				0x0E
#define Page0_Reserved_2	0x0F

//Page 1 ==> Command
#define Page1_Reserved_1	0x10
#define ModeReg				0x11
#define TxModeReg			0x12
#define RxModeReg			0x13
#define TxControlReg		0x14
#define TxASKReg			0x15
#define TxSelReg			0x16
#define RxSelReg			0x17
#define RxThresholdReg		0x18
#define	DemodReg			0x19
#define Page1_Reserved_2	0x1A
#define Page1_Reserved_3	0x1B
#define MfTxReg				0x1C
#define MfRxReg				0x1D
#define Page1_Reserved_4	0x1E
#define SerialSpeedReg		0x1F

//Page 2 ==> CFG
#define Page2_Reserved_1	0x20
#define CRCResultReg_1		0x21
#define CRCResultReg_2		0x22
#define Page2_Reserved_2	0x23
#define ModWidthReg			0x24
#define Page2_Reserved_3	0x25
#define RFCfgReg			0x26
#define GsNReg				0x27
#define CWGsPReg			0x28
#define ModGsPReg			0x29
#define TModeReg			0x2A
#define TPrescalerReg		0x2B
#define TReloadReg_1		0x2C
#define TReloadReg_2		0x2D
#define TCounterValReg_1	0x2E
#define TCounterValReg_2 	0x2F

//Page 3 ==> TestRegister
#define Page3_Reserved_1 	0x30
#define TestSel1Reg			0x31
#define TestSel2Reg			0x32
#define TestPinEnReg		0x33
#define TestPinValueReg		0x34
#define TestBusReg			0x35
#define AutoTestReg			0x36
#define VersionReg			0x37
#define AnalogTestReg		0x38
#define TestDAC1Reg			0x39
#define TestDAC2Reg			0x3A
#define TestADCReg			0x3B
#define Page3_Reserved_2 	0x3C
#define Page3_Reserved_3	0x3D
#define Page3_Reserved_4	0x3E
#define Page3_Reserved_5	0x3F

#define CARD_FOUND		1
#define CARD_NOT_FOUND	2
#define ERROR			3

#define MAX_LEN			16

//Card types
#define Mifare_UltraLight 	0x4400
#define Mifare_One_S50		0x0400
#define Mifare_One_S70		0x0200
#define Mifare_Pro_X		0x0800
#define Mifare_DESFire		0x4403

// Mifare_One card command word
# define PICC_REQIDL          0x26               // find the antenna area does not enter hibernation
# define PICC_REQALL          0x52               // find all the cards antenna area
# define PICC_ANTICOLL        0x93               // anti-collision
# define PICC_SElECTTAG       0x93               // election card
# define PICC_AUTHENT1A       0x60               // authentication key A
# define PICC_AUTHENT1B       0x61               // authentication key B
# define PICC_READ            0x30               // Read Block
# define PICC_WRITE           0xA0               // write block
# define PICC_DECREMENT       0xC0               // debit
# define PICC_INCREMENT       0xC1               // recharge
# define PICC_RESTORE         0xC2               // transfer block data to the buffer
# define PICC_TRANSFER        0xB0               // save the data in the buffer
# define PICC_HALT            0x50               // Sleep

void mfrc522_init();
void mfrc522_reset();
void mfrc522_write(uint8_t reg, uint8_t data);
uint8_t mfrc522_read(uint8_t reg);
uint8_t	mfrc522_request(uint8_t req_mode, uint8_t * tag_type);
uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len);
uint8_t mfrc522_get_card_serial(uint8_t * serial_out);

#endif
#endif
