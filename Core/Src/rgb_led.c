#include "rgb_led.h"

volatile uint8_t current_alert_state = 0;
volatile float current_ppm = 0.0f;

void RGB_LED_Init(void) {
    RCC->AHB1ENR |= RGB_RCC_AHB1ENR;
    RGB_GPIO_PORT->MODER &= ~((3U << (RGB_RED_PIN * 2)) | (3U << (RGB_GREEN_PIN * 2)) | (3U << (RGB_BLUE_PIN * 2)));
    RGB_GPIO_PORT->MODER |= (1U << (RGB_RED_PIN * 2)) | (1U << (RGB_GREEN_PIN * 2)) | (1U << (RGB_BLUE_PIN * 2));
    RGB_GPIO_PORT->OTYPER &= ~((1U << RGB_RED_PIN) | (1U << RGB_GREEN_PIN) | (1U << RGB_BLUE_PIN));
    RGB_GPIO_PORT->OSPEEDR |= (3U << (RGB_RED_PIN * 2)) | (3U << (RGB_GREEN_PIN * 2)) | (3U << (RGB_BLUE_PIN * 2));
    RGB_GPIO_PORT->PUPDR &= ~((3U << (RGB_RED_PIN * 2)) | (3U << (RGB_GREEN_PIN * 2)) | (3U << (RGB_BLUE_PIN * 2)));
    RGB_GPIO_PORT->ODR &= ~((1U << RGB_RED_PIN) | (1U << RGB_GREEN_PIN) | (1U << RGB_BLUE_PIN));
}

void TIM2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 = 0;
    TIM2->PSC = (SystemCoreClock / 1000000) - 1;
    TIM2->ARR = 10000;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void RGB_LED_SetState(uint8_t alert_state, uint8_t system_active, float ppm) {
    RGB_GPIO_PORT->ODR &= ~((1U << RGB_RED_PIN) | (1U << RGB_GREEN_PIN) | (1U << RGB_BLUE_PIN));

    if (!system_active) {
        RGB_GPIO_PORT->ODR |= (1U << RGB_GREEN_PIN);
        TIM2->CR1 &= ~TIM_CR1_CEN;
    } else {
        current_alert_state = alert_state;
        current_ppm = ppm;
        if (alert_state == -1) {
            TIM2->CR1 &= ~TIM_CR1_CEN;
        } else if (alert_state == 0) {
            RGB_GPIO_PORT->ODR |= (1U << RGB_BLUE_PIN);
            TIM2->CR1 &= ~TIM_CR1_CEN;
        } else if (alert_state == 1) {
            RGB_GPIO_PORT->ODR |= (1U << RGB_RED_PIN) | (1U << RGB_GREEN_PIN);
            TIM2->CR1 &= ~TIM_CR1_CEN;
        } else if (alert_state == 2) {
            TIM2->CR1 &= ~TIM_CR1_CEN;
            TIM2->ARR = 500000;
            TIM2->CNT = 0;
            TIM2->CR1 |= TIM_CR1_CEN;
            RGB_GPIO_PORT->ODR |= (1U << RGB_RED_PIN);
        } else if (alert_state == 3) {
            float frequency = 2.0f + (ppm - 2000.0f) * (8.0f / 8000.0f);
            frequency = (frequency < 2.0f) ? 2.0f : (frequency > 10.0f) ? 10.0f : frequency;
            uint32_t period_us = (uint32_t)(1000000.0f / (2.0f * frequency));
            TIM2->CR1 &= ~TIM_CR1_CEN;
            TIM2->ARR = period_us;
            TIM2->CNT = 0;
            TIM2->CR1 |= TIM_CR1_CEN;
            RGB_GPIO_PORT->ODR |= (1U << RGB_RED_PIN);
        }
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        if (current_alert_state == 2 || current_alert_state == 3) {
            RGB_GPIO_PORT->ODR ^= (1U << RGB_RED_PIN);
        }
    }
}
