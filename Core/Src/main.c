#include "stm32f4xx.h"
#include "system_config.h"
#include "i2c_lcd.h"
#include "mq2_sensor.h"
#include "relay.h"
#include "servo.h"
#include "uart.h"
#include "rgb_led.h"
#include <stdio.h>

int main(void) {
    SystemClock_Config();
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= (1 << 10);
    GPIOA->ODR &= ~(1 << 5);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~(3U << (13 * 2));
    GPIOC->PUPDR |= (1U << (13 * 2));
    GPIOC->MODER &= ~(3U << (0 * 2));
    GPIOC->PUPDR |= (1U << (0 * 2));
    I2C1_Init();
    LCD_Init();
    ADC_Init();
    Relay_GPIO_Init();
    Servo_Init();
    UART1_Init();
    RGB_LED_Init();
    TIM2_Init();

    char buffer[20];
    char uart_buffer[10];
    char uart_cmd[2];
    float ppm;
    uint8_t system_active = 0;
    uint8_t last_system_active = 0;
    uint8_t last_sw1_state = 1;
    uint8_t last_sw2_state = 1;
    uint8_t alert_state;
    uint32_t last_update = 0;

    LCD_SetCursor(0, 0);
    LCD_SendString("MQ2 Gas Monitor ");
    LCD_SetCursor(1, 0);
    LCD_SendString("Sys: LOADING... ");

    Servo_SetAngle(180);
    delay_ms(2000);
    Servo_SetAngle(0);
    delay_ms(2000);

    snprintf(uart_buffer, sizeof(uart_buffer), "S%d\n", system_active);
    UART1_SendString(uart_buffer);

    while (1) {
        uint8_t current_sw1_state = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
        if (last_sw1_state == 1 && current_sw1_state == 0) {
            system_active = !system_active;
            delay_ms(100);
        }
        last_sw1_state = current_sw1_state;

        uint8_t current_sw2_state = (GPIOC->IDR & (1 << 0)) ? 1 : 0;
        if (last_sw2_state == 1 && current_sw2_state == 0) {
            system_active = 0;
            last_system_active = 0;
            RELAY1_GPIO_PORT->ODR &= ~(1U << RELAY1_PIN);
            RELAY2_GPIO_PORT->ODR &= ~(1U << RELAY2_PIN);
            Servo_SetAngle(0);
            GPIOA->ODR &= ~(1 << 5);
            LCD_SetCursor(0, 0);
            LCD_SendString("MQ2 Gas Monitor ");
            LCD_SetCursor(1, 0);
            LCD_SendString("Sys: LOADING... ");
            delay_ms(2000);
            snprintf(uart_buffer, sizeof(uart_buffer), "S%d\n", system_active);
            UART1_SendString(uart_buffer);
            delay_ms(100);
        }
        last_sw2_state = current_sw2_state;

        if (USART1->SR & USART_SR_RXNE) {
            UART1_ReceiveString(uart_cmd, sizeof(uart_cmd));
            if (uart_cmd[0] == '1') {
                system_active = 1;
            } else if (uart_cmd[0] == '0') {
                system_active = 0;
            }
        }

        if (system_active != last_system_active) {
            snprintf(uart_buffer, sizeof(uart_buffer), "S%d\n", system_active);
            UART1_SendString(uart_buffer);
            last_system_active = system_active;
        }

        if (system_active) {
            ppm = MQ2_ReadPPM();
            alert_state = GetGasAlertState(ppm);

            if (alert_state >= 2) {
                Servo_SetAngle(180);
            } else {
                Servo_SetAngle(0);
            }

            if (alert_state >= 2) {
                RELAY1_GPIO_PORT->ODR |= (1U << RELAY1_PIN);
            } else {
                RELAY1_GPIO_PORT->ODR &= ~(1U << RELAY1_PIN);
            }

            if (alert_state == 3) {
                RELAY2_GPIO_PORT->ODR |= (1U << RELAY2_PIN);
            } else {
                RELAY2_GPIO_PORT->ODR &= ~(1U << RELAY2_PIN);
            }

            if (tick - last_update >= 1000) {
                GPIOA->ODR |= (1 << 5);
                snprintf(uart_buffer, sizeof(uart_buffer), "%.1f %d\n", ppm, alert_state);
                UART1_SendString(uart_buffer);

                LCD_SetCursor(0, 0);
                LCD_SendString("                ");
                LCD_SetCursor(0, 0);
                snprintf(buffer, sizeof(buffer), "Gas:%.1f ppm", ppm);
                LCD_SendString(buffer);

                LCD_SetCursor(1, 0);
                switch (alert_state) {
                    case -1: LCD_SendString("Error    "); break;
                    case 0: LCD_SendString("No Gas   "); break;
                    case 1: LCD_SendString("Low Gas  "); break;
                    case 2: LCD_SendString("High Gas "); break;
                    case 3: LCD_SendString("Danger!  "); break;
                    default: LCD_SendString("Error    "); break;
                }
                LCD_SetCursor(1, 9);
                LCD_SendString("Sys:ON ");

                GPIOA->ODR &= ~(1 << 5);
                last_update = tick;
            }
        } else {
        	Servo_SetAngle(0);
            LCD_SetCursor(0, 0);
            LCD_SendString("Gas:---- ppm    ");
            LCD_SetCursor(1, 0);
            LCD_SendString("Stopped  Sys:OFF");
            GPIOA->ODR &= ~(1 << 5);
            RELAY1_GPIO_PORT->ODR &= ~(1U << RELAY1_PIN);
            RELAY2_GPIO_PORT->ODR &= ~(1U << RELAY2_PIN);
            Servo_SetAngle(0);
        }
        RGB_LED_SetState(alert_state, system_active, ppm);
    }
}
