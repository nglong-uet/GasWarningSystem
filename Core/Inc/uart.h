/*
 * uart.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define UART1_TX_PIN         9
#define UART1_RX_PIN         10
#define UART1_BAUDRATE       9600

void UART1_Init(void);
void UART1_SendString(char *str);
void UART1_ReceiveString(char *buffer, uint8_t max_len);

#endif /* INC_UART_H_ */
