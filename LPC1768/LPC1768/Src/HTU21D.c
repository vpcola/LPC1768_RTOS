/*
 * HTU21D.c
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */

#include "lpc17xx_i2c.h"
#include "I2C.h"
#include "HTU21D.h"

void Init_HTU21D(void)
{
	// HTU21D is connected to I2C2

	// Todo: Send configuration options here
	// such as calibration data to the device
}

int HTU21D_ReadTemperature(float * temperature)
{
	int retval;
	char rxbuff[3];
	char txdata = TRIGGER_TEMP_MEASURE;
	float rawtemp = 0.0;
	//float temp = -1.0;

	I2C_M_SETUP_Type I2CRxTx;

	I2CRxTx.sl_addr7bit = HTU21D_I2C_ADDRESS;
	I2CRxTx.tx_data = (uint8_t *) &txdata;
	I2CRxTx.tx_length = 1;
	// Leave RX options null
	I2CRxTx.rx_data = (uint8_t *) &rxbuff[0];
	I2CRxTx.rx_length = 3;
	I2CRxTx.retransmissions_max = 3;

	I2C2_AcquireBus();
	retval = I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);
	I2C2_ReleaseBus();

	if ( retval == SUCCESS)
	{
		// Algorithm from datasheet to compute temperature.
		unsigned int rawTemperature = ((unsigned int) rxbuff[0] << 8) | (unsigned int) rxbuff[1];
		rawTemperature &= 0xFFFC;

		rawtemp = rawTemperature / (float)65536; //2^16 = 65536
		*temperature = -46.85 + (175.72 * rawtemp); //From page 14

		return 0;
	}else
		return -1;
}

int HTU21D_ReadHumidity(float *humidity)
{
	int retval;
	char rxbuff[3];
	char txdata = TRIGGER_HUMD_MEASURE;
	float rawhumid = 0.0;

	I2C_M_SETUP_Type I2CRxTx;

	I2CRxTx.sl_addr7bit = HTU21D_I2C_ADDRESS;
	I2CRxTx.tx_data = (uint8_t *) &txdata;
	I2CRxTx.tx_length = 1;
	// Leave RX options null
	I2CRxTx.rx_data = (uint8_t *) &rxbuff[0];
	I2CRxTx.rx_length = 3;
	I2CRxTx.retransmissions_max = 3;

	I2C2_AcquireBus();
	retval = I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);
	I2C2_ReleaseBus();

	if ( retval == SUCCESS)
	{
		// Algorithm from datasheet to compute temperature.
		unsigned int rawHumidity = ((unsigned int) rxbuff[0] << 8) | (unsigned int) rxbuff[1];
		rawHumidity &= 0xFFFC;

		rawhumid = rawHumidity / (float)65536; //2^16 = 65536
		*humidity = -46.85 + (175.72 * rawhumid); //From page 14

		return 0;
	}else
		return -1;
}

