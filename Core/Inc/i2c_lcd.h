/*
 * i2c_lcd.h
 *
 *  Created on: Jun 18, 2025
 *      Author: admin
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#include <stdint.h>

#define I2C_SCL_PIN 8
#define I2C_SDA_PIN 9
#define LCD_I2C_ADDR (0x27 << 1)

void I2C1_Init(void);
void I2C1_SendBytes(uint8_t addr, uint8_t *data, uint8_t len);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_Init(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Clear(void);

#endif /* INC_I2C_LCD_H_ */
