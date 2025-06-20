/*
 * system_config.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_SYSTEM_CONFIG_H_
#define INC_SYSTEM_CONFIG_H_

#include "stm32f4xx.h"

extern volatile uint32_t tick;

void SystemClock_Config(void);
void delay_ms(uint32_t ms);

#endif /* INC_SYSTEM_CONFIG_H_ */
