/*
 * mq2_sensor.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_MQ2_SENSOR_H_
#define INC_MQ2_SENSOR_H_

#include <stdint.h>

#define RL_VALUE     5.0f
#define ADC_MAX      4095.0f
#define VCC          5.0f
#define FIXED_RO     5.0f

extern float LPGCurve[3];

void ADC_Init(void);
float MQ2_ReadPPM(void);
uint8_t GetGasAlertState(float ppm);

#endif /* INC_MQ2_SENSOR_H_ */
