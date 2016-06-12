/*
 * SEVENSEGMENT.c
 *
 *  Created on: May 30, 2016
 *      Author: vpcol
 */

#include "SEVENSEGMENT.h"
#include "I2C.h"
#include "lpc17xx_i2c.h"

static const unsigned char numbers[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00 };

void Init_SevenSeg(void)
{
	unsigned char data[2];
	data[0] = SEVENSEG_PORT_CFG; // Config Reg for Port A
	data[1] = 0x0; // Port A all outputs

	// Send to register 0x0, 0x00 - all output
	I2C_M_SETUP_Type I2CRxTx;

	I2CRxTx.sl_addr7bit = SEVENSEG_I2C_ADDR;
	I2CRxTx.tx_data = (uint8_t *) &data[0];
	I2CRxTx.tx_length = 2;
	// Leave RX options null
	I2CRxTx.rx_data = NULL;
	I2CRxTx.rx_length = 0;
	I2CRxTx.retransmissions_max = 3;

	I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);

}

void SEVENSEG_Set(unsigned char num)
{

	unsigned char data[2];
	data[0] = SEVENSEG_PORT_REG; // Config Reg for Port A
	data[1] = numbers[num]; // Port A all outputs

	// Send to register 0x0, 0x00 - all output
	I2C_M_SETUP_Type I2CRxTx;

	I2CRxTx.sl_addr7bit = SEVENSEG_I2C_ADDR;
	I2CRxTx.tx_data = (uint8_t *) &data[0];
	I2CRxTx.tx_length = 2;
	// Leave RX options null
	I2CRxTx.rx_data = NULL;
	I2CRxTx.rx_length = 0;
	I2CRxTx.retransmissions_max = 3;

	I2C2_AcquireBus();
	I2C_MasterTransferData(LPC_I2C2, &I2CRxTx, I2C_TRANSFER_POLLING);
	I2C2_ReleaseBus();
}
