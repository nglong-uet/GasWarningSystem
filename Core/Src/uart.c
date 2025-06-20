#include "uart.h"

void UART1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    GPIOA->MODER &= ~((3U << (UART1_TX_PIN * 2)) | (3U << (UART1_RX_PIN * 2)));
    GPIOA->MODER |= (2U << (UART1_TX_PIN * 2)) | (2U << (UART1_RX_PIN * 2));
    GPIOA->OTYPER &= ~((1U << UART1_TX_PIN) | (1U << UART1_RX_PIN));
    GPIOA->OSPEEDR |= (3U << (UART1_TX_PIN * 2)) | (3U << (UART1_RX_PIN * 2));
    GPIOA->AFR[1] |= (7U << ((UART1_TX_PIN - 8) * 4)) | (7U << ((UART1_RX_PIN - 8) * 4));
    USART1->CR1 = 0;
    USART1->BRR = SystemCoreClock / UART1_BAUDRATE;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void UART1_SendString(char *str) {
    while (*str) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = *str++;
    }
}

void UART1_ReceiveString(char *buffer, uint8_t max_len) {
    uint8_t i = 0;
    while (i < max_len - 1) {
        while (!(USART1->SR & USART_SR_RXNE));
        char c = USART1->DR;
        if (c == '\n') {
            buffer[i] = '\0';
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}
