/*
 * HTU21D.h
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */

#ifndef INC_HTU21D_H_
#define INC_HTU21D_H_

#ifdef __cpluplus
extern "C" {
#endif

#define HTU21D_I2C_ADDRESS  0x40
#define TRIGGER_TEMP_MEASURE  0xE3
#define TRIGGER_HUMD_MEASURE  0xE5

void Init_HTU21D(void);

int HTU21D_ReadTemperature(float *temperature);
int HTU21D_ReadHumidity(float *humidity);

#ifdef __cplusplus
}
#endif


#endif /* INC_HTU21D_H_ */
