/*
 * UART.h
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#ifdef __cpluplus
extern "C" {
#endif

// Function prototypes
void Init_UART3(int baudrate);
// Functions to send/receive data from UART
unsigned int UART3_SendBytes(char *data, int len);
unsigned int UART3_ReceiveBytes(char * data, int len);


#ifdef __cplusplus
}
#endif

#endif /* INC_UART_H_ */
