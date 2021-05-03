/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include "../appli/config.h"
#if USE_MPU6050
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf52_i2c.h"
#include "mpu6050.h"
#include "nrf_delay.h"
#include "appli/common/gpio.h"

/*lint ++flb "Enter library region" */

#define FORMAT_TRAME_TEAPOT 1  					// Pour test petit avion
//#define FORMAT_TRAME_ARTIFICIAL_HORIZON 0  	// Pour test panneau de bord A320

#define M_PI 1

//static const uint8_t expected_who_am_i_9250 = 0x71; // !< Expected value to get from WHO_AM_I register.
static const uint8_t expected_who_am_i = 0x68U; // !< Expected value to get from WHO_AM_I register.

static bool_e initialized = FALSE;

void mpu6050_i2c_init(uint8_t device_address)
{
	running_e sub;
	I2C_init(device_address);

	SYSTICK_delay_ms(500);

    // Do a reset on signal paths
    uint8_t reset_value = 0x04U | 0x02U | 0x01U; // Resets gyro, accelerometer and temperature sensor signal paths.
    sub = mpu6050_register_write(ADDRESS_SIGNAL_PATH_RESET, reset_value);

    // Read and verify product ID
    if(sub == END_OK)
    	initialized = mpu6050_verify_product_id();


    while(!initialized)
    {
    	initialized = mpu6050_verify_product_id();
    	SYSTICK_delay_ms(100);
    }
}
/*
 * @brief	Initialise le module MPU6050 en activant son alimentation, puis en configurant les registres internes du MPU6050.
 * @param	GPIOx et GPIO_PIN_x indiquent la broche oÃ¹ l'on a reliÃ© l'alimentation Vcc du MPU6050.
 * 			Indiquer NULL dans GPIOx s'il est alimentÃ© en direct.
 * 			Cette possibilitÃ© d'alimentation par la broche permet le reset du module par le microcontrÃ´leur.
 * @param	DataStruct : fournir le pointeur vers une structure qui sera Ã  conserver pour les autres appels des fonctions de ce module logiciel.
 * @param 	DeviceNumber : 					voir MPU6050_Device_t
 * @param	AccelerometerSensitivity : 		voir MPU6050_Accelerometer_t
 * @param	GyroscopeSensitivity :			voir MPU6050_Gyroscope_t
 */
bool MPU6050_Init(MPU6050_t* DataStruct, MPU6050_Accelerometer_t AccelerometerSensitivity, MPU6050_Gyroscope_t GyroscopeSensitivity)
{
	uint8_t temp;

	// Initialize Power !
	#ifdef MPU6050_VCC_PIN
		GPIO_configure(MPU6050_VCC_PIN, GPIO_PIN_CNF_PULL_Disabled, TRUE);
		GPIO_write(MPU6050_VCC_PIN, FALSE);
		SYSTICK_delay_ms(500);
		GPIO_write(MPU6050_VCC_PIN, TRUE);
	#endif

	/* Initialize I2C */
	mpu6050_i2c_init(MPU_6050_I2C_ADDRESS);

	/* Wakeup MPU6050 */
	mpu6050_register_write(MPU6050_PWR_MGMT_1, 0x00);

	/* Config accelerometer */
	//temp = I2C_Read(MPU6050_I2C, DataStruct->Address, MPU6050_ACCEL_CONFIG);
	mpu6050_register_read(MPU6050_ACCEL_CONFIG, &temp, 1);
	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
	//I2C_Write(MPU6050_I2C, DataStruct->Address, MPU6050_ACCEL_CONFIG, temp);
	mpu6050_register_write(MPU6050_ACCEL_CONFIG, temp);

	/* Config gyroscope */
	//temp = I2C_Read(MPU6050_I2C, DataStruct->Address, MPU6050_GYRO_CONFIG);
	mpu6050_register_read(MPU6050_GYRO_CONFIG, &temp, 1);
	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;
	//I2C_Write(MPU6050_I2C, DataStruct->Address, MPU6050_GYRO_CONFIG, temp);
	mpu6050_register_write(MPU6050_GYRO_CONFIG, temp);

	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
		case MPU6050_Accelerometer_2G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2;
			break;
		case MPU6050_Accelerometer_4G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4;
			break;
		case MPU6050_Accelerometer_8G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8;
			break;
		case MPU6050_Accelerometer_16G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16;
			//no break
		default:
			break;
	}

	switch (GyroscopeSensitivity) {
		case MPU6050_Gyroscope_250s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250;
			break;
		case MPU6050_Gyroscope_500s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500;
			break;
		case MPU6050_Gyroscope_1000s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000;
			break;
		case MPU6050_Gyroscope_2000s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000;
			// no break
		default:
			break;
	}

	/* Return OK */
	return MPU6050_Result_Ok;
}
bool mpu6050_verify_product_id(void)
{
    uint8_t who_am_i = 0x55;

    if (mpu6050_register_read(ADDRESS_WHO_AM_I, &who_am_i, 1) == END_OK)
    {
        if (who_am_i != expected_who_am_i)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool mpu6050_register_write(uint8_t register_address, uint8_t value)
{
	running_e sub;
	do
	{
		sub = I2C_register_write(register_address, value);
	}while(sub == IN_PROGRESS);
	return sub;
}



running_e mpu6050_register_read(uint8_t register_address, uint8_t * destination, uint8_t number_of_bytes)
{
	running_e sub;
	do
	{
		sub = I2C_register_read(register_address, destination, number_of_bytes);
	}while(sub == IN_PROGRESS);
    return sub;
}


bool MPU6050_ReadAllType2(TYPE_2_MPU6050_t* DataStruct) {
	uint8_t data[14];
	int16_t temp;

	/* Read full raw data, 14bytes */
//	I2C_ReadMulti(MPU6050_I2C, DataStruct->Address, MPU6050_ACCEL_XOUT_H, data, 14);
//MPU6050_Init(&MPU6050_Data, GPIOA, GPIO_PIN_0, MPU6050_Device_0, MPU6050_Accelerometer_8G, MPU6050_Gyroscope_2000s) != MPU6050_Result_Ok

	mpu6050_register_read(MPU6050_ACCEL_XOUT_H, data, 14);
	/* Format accelerometer data */
	DataStruct->Accelerometer_x_h = data[0];
	DataStruct->Accelerometer_x_l = data[1];
	DataStruct->Accelerometer_y_h = data[2];
	DataStruct->Accelerometer_y_l = data[3];
	DataStruct->Accelerometer_z_h = data[4];
	DataStruct->Accelerometer_z_l = data[5];
	DataStruct->Gyroscope_x_h= data[8];
	DataStruct->Gyroscope_x_l= data[9];
	DataStruct->Gyroscope_y_h= data[10];
	DataStruct->Gyroscope_y_l= data[11];
	DataStruct->Gyroscope_z_h= data[12];
	DataStruct->Gyroscope_z_l= data[13];

	/* Format temperature */
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (float)((float)((int16_t)temp) / (float)340.0 + (float)36.53);


	/* Return OK */
	return MPU6050_Result_Ok;

}
 bool MPU6050_ReadAllType1(MPU6050_t* DataStruct) {
	uint8_t data[14];
	int16_t temp;
	for(uint8_t i = 0; i<14 ; i++)
		data[i] = 0x55;
	/* Read full raw data, 14bytes */
	//	I2C_ReadMulti(MPU6050_I2C, DataStruct->Address, MPU6050_ACCEL_XOUT_H, data, 14);
	//	MPU6050_Init(&MPU6050_Data, MPU6050_Accelerometer_8G, MPU6050_Gyroscope_2000s);

	mpu6050_register_read(MPU6050_ACCEL_XOUT_H, data, 14);
	/* Format accelerometer data */
	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Format temperature */
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (float)((float)((int16_t)temp) / (float)340.0 + (float)36.53);

	/* Format gyroscope data */
	DataStruct->Gyroscope_X = (int16_t)(data[8] << 8 | data[9]);
	DataStruct->Gyroscope_Y = (int16_t)(data[10] << 8 | data[11]);
	DataStruct->Gyroscope_Z = (int16_t)(data[12] << 8 | data[13]);

	/* Return OK */
	return MPU6050_Result_Ok;
}

void mpu6050_test(uint8_t device_address) {
	MPU6050_t MPU6050_Data;
/* Initialize MPU6050 sensor */
    MPU6050_Init(&MPU6050_Data, MPU6050_Accelerometer_4G, MPU6050_Gyroscope_1000s);

//--------------------------------------------------------------------------//
//	pour Test reception de la trame sur console simulateur Teapot  			//
//																			//
//--------------------------------------------------------------------------//

    // get the first available port (use EITHER this OR the specific port code below)
    // DA - changed the port to second one in the list (for my laptop)
    //String portName = Serial.list()[1];

    // get a specific serial port (use EITHER this OR the first-available code above)
    //String portName = "COM5";

    // open the serial port
    //DA - changed the serial data rate to 38400
    //port = new Serial(this, portName, 38400);
//--------------------------------------------------------------------------//
//	teapotPacket[2]=0x32;
//	teapotPacket[3]=0x33;
//	teapotPacket[4]=0x34;
//	teapotPacket[5]=0x35;
//	teapotPacket[6]=0x36;
//	teapotPacket[7]=0x37;
//	teapotPacket[8]=0x38;
//	teapotPacket[9]=0x39;
// Test ok avec affichage println("Char : " + (char)ch);

	while (1) {
	    MPU6050_Init(&MPU6050_Data, MPU6050_Accelerometer_2G, MPU6050_Gyroscope_500s);

		/* format Teapot  (Type2) */
#ifdef FORMAT_TRAME_TEAPOT
		TYPE_2_MPU6050_t TYPE_2_MPU6050_Data;
		uint8_t teapotPacket[14]={'$',0x02,0,0,0,0,0,0,0,0,0x00,0x00,'\r','\n'};

		/* Read all data from sensor to TYPE 2 struct */
	    MPU6050_ReadAllType2(&TYPE_2_MPU6050_Data);
		teapotPacket[2]=TYPE_2_MPU6050_Data.Gyroscope_x_h;
		teapotPacket[3]=TYPE_2_MPU6050_Data.Gyroscope_x_l;
		teapotPacket[4]=TYPE_2_MPU6050_Data.Accelerometer_y_h;
		teapotPacket[5]=TYPE_2_MPU6050_Data.Accelerometer_y_l;
		teapotPacket[6]=TYPE_2_MPU6050_Data.Accelerometer_x_h;
		teapotPacket[7]=TYPE_2_MPU6050_Data.Accelerometer_x_l;
		teapotPacket[8]=TYPE_2_MPU6050_Data.Accelerometer_z_h;
		teapotPacket[9]=TYPE_2_MPU6050_Data.Accelerometer_z_l;
//		teapotPacket[8]=TYPE_2_MPU6050_Data.Gyroscope_x_h;
//		teapotPacket[9]=TYPE_2_MPU6050_Data.Gyroscope_x_l;

		//--------------------------------------------------------------------------//
		//	pour Test reception de la trame avec valeurs réelles i2c brutes			//
		//	sur console Docklight	(cabler mpu6050,fermer console TEAPOT, 			//
		//	ouvrir Docklight)														//
		//																			//
		//--------------------------------------------------------------------------//
		printf("$%c%c%c%c%c%c%c%c%c%d\r\n",teapotPacket[1],teapotPacket[2],teapotPacket[3],teapotPacket[4],teapotPacket[5],
			  teapotPacket[6],teapotPacket[7],teapotPacket[8],teapotPacket[9],teapotPacket[11]);

		//--------------------------------------------------------------------------//
		//	pour Test reception de la trame en ascii								//
		//	sur console Docklight	(cabler mpu6050,fermer console TEAPOT, 			//
		//	ouvrir Docklight)														//
		//																			//
		//--------------------------------------------------------------------------//
//			teapotPacket[2]=0x32;
//			teapotPacket[3]=0x33;
//			teapotPacket[4]=0x34;
//			teapotPacket[5]=0x35;
//			teapotPacket[6]=0x36;
//			teapotPacket[7]=0x37;
//			teapotPacket[8]=0x38;
//			teapotPacket[9]=0x39;
//		printf("$%c%c%c%c%c%c%c%c%c%d\r\n",teapotPacket[1],teapotPacket[2],teapotPacket[3],teapotPacket[4],teapotPacket[5],
//			  teapotPacket[6],teapotPacket[7],teapotPacket[8],teapotPacket[9],teapotPacket[11]);

//		teapotPacket[11]++;

		/* for test on uart docklight Format data */
		/*printf("Accelerometer\n- X:%d\n- Y:%d\n- Z:%d\nGyroscope\n- X:%d\n- Y:%d\n- Z:%d\nTemperature\n- %3.1f°\n\n\n",
			MPU6050_Data.Accelerometer_X,
			MPU6050_Data.Accelerometer_Y,
			MPU6050_Data.Accelerometer_Z,
			MPU6050_Data.Gyroscope_X,
			MPU6050_Data.Gyroscope_Y,
			MPU6050_Data.Gyroscope_Z,
			MPU6050_Data.Temperature
		);*/

//		gyro_x += MPU6050_Data.Gyroscope_X;
//		gyro_y += MPU6050_Data.Gyroscope_Y;
//		gyro_z += MPU6050_Data.Gyroscope_Z;

		/*printf("AX%4d\tAY%4d\tAZ%4d\tGX%4d\tGY%4d\tGZ%4d\tgx%4ld°\tgy%4ld°\tgz%4ld°\tT%3.1f°\n",
						MPU6050_Data.Accelerometer_X/410,	//environ en %
						MPU6050_Data.Accelerometer_Y/410,	//environ en %
						MPU6050_Data.Accelerometer_Z/410,	//environ en %
						MPU6050_Data.Gyroscope_X,
						MPU6050_Data.Gyroscope_Y,
						MPU6050_Data.Gyroscope_Z,
						gyro_x/16400,						//environ en °
						gyro_y/16400,						//environ en °
						gyro_z/16400,						//environ en °
						MPU6050_Data.Temperature);*/
#endif


// format Artificial horizon (Type1)
#ifdef FORMAT_TRAME_ARTIFICIAL_HORIZON
		uint16_t ypr[3];

	    //----------------------------------------------------------------------------------------------//
	    //	pour Test reception de la trame sur console simulateur Artificial Horizon Compass  			//
	    //																								//
	    //----------------------------------------------------------------------------------------------//
		// code ino
//#ifdef OUTPUT_READABLE_YAWPITCHROLL
//     // display Euler angles in degrees
//     mpu.dmpGetQuaternion(&q, fifoBuffer);
//     mpu.dmpGetGravity(&gravity, &q);
//     mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
//     //Serial.print("Phi: ");
//     Serial.print(ypr[2] * 18/M_PI);
//     //Serial.print("\t theta: ");
//     Serial.print(" ");
//     Serial.print(ypr[1] * 180/M_PI);
//     //Serial.print("\t Psi: ");
//     Serial.print(" ");
//     Serial.println(ypr[0] * 180/M_PI);
     //delay(100);
 //#endif
//		void loop() {
	//	  /* Update all the values */
//		  while (i2cRead(0x3B, i2cData, 14));
//		  accX = ((i2cData[0] << 8) | i2cData[1]);
//		  accY = ((i2cData[2] << 8) | i2cData[3]);
//		  accZ = ((i2cData[4] << 8) | i2cData[5]);
//		  tempRaw = (i2cData[6] << 8) | i2cData[7];
//		  gyroX = (i2cData[8] << 8) | i2cData[9];
//		  gyroY = (i2cData[10] << 8) | i2cData[11];
//		  gyroZ = (i2cData[12] << 8) | i2cData[13];
//
//		  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
//		  timer = micros();
//
// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
// It is then converted from radians to degrees
//		#ifdef RESTRICT_PITCH // Eq. 25 and 26
//		  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
//		  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
//		#else // Eq. 28 and 29
//		  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
//		  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
//		#endif
		//
		//  code Processing
	/*    String input = port.readStringUntil('\n');
	   if(input != null){
	    input = trim(input);
	   String[] values = split(input, " ");
	  if(values.length == 3){
	   float phi = float(values[0]);
	   float theta = float(values[1]);
	   float psi = float(values[2]);
	   print(phi);
	   print(theta);
	   println(psi);
	   Phi = phi;
	   Theta = theta;
	   Psi = psi;  */

		MPU6050_ReadAllType1(&MPU6050_Data);

		ypr[0]=MPU6050_Data.Accelerometer_X;
		ypr[1]=MPU6050_Data.Accelerometer_Y;
		ypr[2]=MPU6050_Data.Accelerometer_Z;

		printf("Phi: ");
		printf("%3.1f°",(float)(ypr[2] * 18/M_PI));
		printf("\t theta: ");
		printf(" ");
		printf("%3.1f°",(float)(ypr[1] * 180/M_PI));
		printf("\t Psi: ");
		printf(" ");
		printf("%3.1f°",(float)(ypr[0] * 180/M_PI));
		printf("\n");

#endif

		/* Little delay */
		nrf_delay_ms(50);
	}  // end of while(1);
}  // end of void
#endif //USE MPU6050
