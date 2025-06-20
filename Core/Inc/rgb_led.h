/*
 * rbg_led.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_RGB_LED_H_
#define INC_RGB_LED_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define RGB_RED_PIN          3
#define RGB_GREEN_PIN        4
#define RGB_BLUE_PIN         5
#define RGB_GPIO_PORT        GPIOB
#define RGB_RCC_AHB1ENR      RCC_AHB1ENR_GPIOBEN

extern volatile uint8_t current_alert_state;
extern volatile float current_ppm;

void RGB_LED_Init(void);
void RGB_LED_SetState(uint8_t alert_state, uint8_t system_active, float ppm);
void TIM2_Init(void);

#endif /* INC_RGB_LED_H_ */
