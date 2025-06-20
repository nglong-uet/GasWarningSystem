/*
 * relay.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_RELAY_H_
#define INC_RELAY_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define RELAY1_PIN           1
#define RELAY1_GPIO_PORT     GPIOB
#define RELAY1_RCC_AHB1ENR   RCC_AHB1ENR_GPIOBEN
#define RELAY2_PIN           2
#define RELAY2_GPIO_PORT     GPIOB
#define RELAY2_RCC_AHB1ENR   RCC_AHB1ENR_GPIOBEN

void Relay_GPIO_Init(void);

#endif /* INC_RELAY_H_ */
