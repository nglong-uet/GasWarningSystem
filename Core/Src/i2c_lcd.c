#include "i2c_lcd.h"
#include "system_config.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

void I2C1_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &= ~((3U << (I2C_SCL_PIN * 2)) | (3U << (I2C_SDA_PIN * 2)));
    GPIOB->MODER |= (2U << (I2C_SCL_PIN * 2)) | (2U << (I2C_SDA_PIN * 2));
    GPIOB->OTYPER |= (1U << I2C_SCL_PIN) | (1U << I2C_SDA_PIN);
    GPIOB->OSPEEDR |= (3U << (I2C_SCL_PIN * 2)) | (3U << (I2C_SDA_PIN * 2));
    GPIOB->PUPDR |= (1U << (I2C_SCL_PIN * 2)) | (1U << (I2C_SDA_PIN * 2));
    GPIOB->AFR[1] |= (4U << ((I2C_SCL_PIN - 8) * 4)) | (4U << ((I2C_SDA_PIN - 8) * 4));
    I2C1->CR1 = 0;
    I2C1->CR2 = 42;
    I2C1->CCR = 210;
    I2C1->TRISE = 43;
    I2C1->CR1 |= I2C_CR1_PE;
}


void I2C1_SendBytes(uint8_t addr, uint8_t *data, uint8_t len) {
    uint32_t timeout = 1000000;
    while (I2C1->SR2 & I2C_SR2_BUSY && timeout--) {
        if (timeout == 0) return;
    }
    I2C1->CR1 |= I2C_CR1_START;
    timeout = 1000000;
    while (!(I2C1->SR1 & I2C_SR1_SB) && timeout--) {
        if (timeout == 0) return;
    }
    I2C1->DR = addr;
    timeout = 1000000;
    while (!(I2C1->SR1 & I2C_SR1_ADDR) && timeout--) {
        if (timeout == 0) return;
    }
    (void)I2C1->SR2;
    for (uint8_t i = 0; i < len; i++) {
        timeout = 1000000;
        while (!(I2C1->SR1 & I2C_SR1_TXE) && timeout--) {
            if (timeout == 0) return;
        }
        I2C1->DR = data[i];
    }
    timeout = 1000000;
    while (!(I2C1->SR1 & I2C_SR1_BTF) && timeout--) {
        if (timeout == 0) return;
    }
    I2C1->CR1 |= I2C_CR1_STOP;
}

void LCD_SendCommand(uint8_t cmd) {
    uint8_t data[4];
    data[0] = (cmd & 0xF0) | 0x0C;
    data[1] = (cmd & 0xF0) | 0x08;
    data[2] = ((cmd << 4) & 0xF0) | 0x0C;
    data[3] = ((cmd << 4) & 0xF0) | 0x08;
    I2C1_SendBytes(LCD_I2C_ADDR, data, 4);
    delay_ms(2);
}

void LCD_SendData(uint8_t data) {
    uint8_t buf[4];
    buf[0] = (data & 0xF0) | 0x0D;
    buf[1] = (data & 0xF0) | 0x09;
    buf[2] = ((data << 4) & 0xF0) | 0x0D;
    buf[3] = ((data << 4) & 0xF0) | 0x09;
    I2C1_SendBytes(LCD_I2C_ADDR, buf, 4);
    delay_ms(2);
}

void LCD_SendString(char *str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}

void LCD_Init(void) {
    delay_ms(50);
    LCD_SendCommand(0x30);
    delay_ms(5);
    LCD_SendCommand(0x30);
    delay_ms(1);
    LCD_SendCommand(0x30);
    delay_ms(1);
    LCD_SendCommand(0x02);
    delay_ms(1);
    LCD_SendCommand(0x28);
    delay_ms(1);
    LCD_SendCommand(0x08);
    delay_ms(1);
    LCD_SendCommand(0x01);
    delay_ms(2);
    LCD_SendCommand(0x06);
    delay_ms(1);
    LCD_SendCommand(0x0C);
    delay_ms(1);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCommand(pos);
    delay_ms(2);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    delay_ms(2);
}
