/*
 * BH1750.c
 *
 *  Created on: May 30, 2016
 *      Author: vpcol
 */

#include "BH1750.h"
#include "I2C.h"
#include "lpc17xx_i2c.h"


void Init_BH1750()
{
	// Power on BH1750
	unsigned char data[2];
	data[0] = BH1750_POWER_ON;

	I2C_M_SETUP_Type I2CRxTx;

	I2CRxTx.sl_addr7bit = BH1750_I2CADDR;
	I2CRxTx.tx_data = &data[0];
	I2CRxTx.tx_length = 1;
	I2CRxTx.rx_data = NULL;
	I2CRxTx.rx_length = 0;
	I2CRxTx.retransmissions_max = 3;

	//I2C2_AcquireBus();
	I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);
	//I2C2_ReleaseBus();

	// Start continous BH1750_CONTINUOUS_HIGH_RES_MODE
	data[0] = BH1750_CONTINUOUS_HIGH_RES_MODE;

	I2CRxTx.sl_addr7bit = BH1750_I2CADDR;
	I2CRxTx.tx_data = &data[0];
	I2CRxTx.tx_length = 1;
	I2CRxTx.rx_data = NULL;
	I2CRxTx.rx_length = 0;
	I2CRxTx.retransmissions_max = 3;

	//I2C2_AcquireBus();
	I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);
	//I2C2_ReleaseBus();
}

int BH1750_GetLux(float * result)
{
	int retval;
	unsigned char data[2];

	// Send to register 0x0, 0x00 - all output
	I2C_M_SETUP_Type I2CRxTx;

	I2CRxTx.sl_addr7bit = BH1750_I2CADDR;
	I2CRxTx.tx_data = NULL;
	I2CRxTx.tx_length = 0;
	// Leave RX options null
	I2CRxTx.rx_data = &data[0];
	I2CRxTx.rx_length = 2;
	I2CRxTx.retransmissions_max = 3;

	I2C2_AcquireBus();
	retval = I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);
	I2C2_ReleaseBus();

	if ( retval == SUCCESS)
	{
		*result =  (float) ((data[0] << 8) | data[1]) / 1.2;

		return 0;
	}

	return -1;
}
