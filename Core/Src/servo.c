#include "servo.h"
#include "stm32f4xx_hal.h"

void Servo_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Kích hoạt clock cho GPIOA
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // Kích hoạt clock cho TIM3
	GPIOA->MODER &= ~(3U << (SERVO_PIN * 2));
	GPIOA->MODER |= (2U << (SERVO_PIN * 2)); // Chế độ Alternate Function
	GPIOA->OTYPER &= ~(1U << SERVO_PIN);
	GPIOA->OSPEEDR |= (3U << (SERVO_PIN * 2)); // Tốc độ cao
	GPIOA->AFR[0] |= (2U << (SERVO_PIN * 4)); // AF2 cho TIM3_CH1 (PA6)
	TIM3->CR1 = 0;
	TIM3->PSC = (SystemCoreClock / 1000000) - 1; // Tick 1µs
	TIM3->ARR = SERVO_PWM_PERIOD - 1; // Chu kỳ 20ms
	TIM3->CCMR1 &= ~(TIM_CCMR1_OC1M); // Xóa OC1M
	TIM3->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos); // PWM mode 1
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // Bật preload
	TIM3->CCER |= TIM_CCER_CC1E; // Bật kênh 1
	TIM3->CCR1 = SERVO_MIN_PULSE; // Độ rộng xung ban đầu (0°)
	TIM3->EGR |= TIM_EGR_UG; // Cập nhật thanh ghi
	TIM3->CR1 |= TIM_CR1_CEN; // Bật Timer
}

void Servo_SetAngle(uint16_t angle) {
	if (angle > 180) angle = 180;
	uint32_t pulse = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle) / 180;
	TIM3->CCR1 = pulse; // Sử dụng CCR1 cho kênh 1
}

