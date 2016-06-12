/*
 * SEVENSEGMENT.h
 *
 *  Created on: May 30, 2016
 *      Author: vpcol
 */

#ifndef SEVENSEGMENT_H_
#define SEVENSEGMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SEVENSEG_I2C_ADDR	0x21
#define SEVENSEG_PORT_REG	0x14	// Output latch A
#define SEVENSEG_PORT_CFG	0x00	// Direction Cfg reg

void Init_SevenSeg(void);
void SEVENSEG_Set(unsigned char num);

#ifdef __cplusplus
}
#endif


#endif /* SEVENSEGMENT_H_ */
