/*
 * servo.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include <stdint.h>

#define SERVO_PIN            6  // PA6 - Chân điều khiển Servo
#define SERVO_GPIO_PORT      GPIOA // Port của chân Servo (PA6 thuộc GPIOA)
#define SERVO_RCC_AHB1ENR    RCC_AHB1ENR_GPIOAEN // Clock enable cho GPIOA
#define SERVO_PWM_PERIOD     20000
#define SERVO_MIN_PULSE      500
#define SERVO_MAX_PULSE      2500

void Servo_Init(void);
void Servo_SetAngle(uint16_t angle);

#endif /* INC_SERVO_H_ */
