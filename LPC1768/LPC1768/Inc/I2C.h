/*
 * I2C.h
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#define I2C2_SPEED 					400000
#define I2C2_MAX_RETRANSMISSIONS	3

void Init_I2C2(void);
void I2C2_AcquireBus(void);
void I2C2_ReleaseBus(void);
// Wrapper functions to call I2C driver
unsigned int I2C2_SendBytes(int devaddr, char * data, int len);
unsigned int I2C2_ReceiveBytes(int devaddr, int regaddr, char * data, int len);

#ifdef __cplusplus
}
#endif

#endif /* INC_I2C_H_ */
