#include "mq2_sensor.h"
#include "system_config.h"
#include <math.h>

float LPGCurve[3] = {2.3f, 0.21f, -0.47f};

void ADC_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    GPIOA->MODER |= (3U << (0 * 2));
    GPIOA->PUPDR &= ~(3U << (0 * 2));
    ADC1->CR1 = 0;
    ADC1->CR1 |= (1U << 8);
    ADC1->CR2 = 0;
    ADC1->SMPR2 |= (5U << ADC_SMPR2_SMP0_Pos);
    ADC1->SQR1 = 0;
    ADC1->SQR3 = 0;
    ADC1->CR2 |= ADC_CR2_ADON;
    delay_ms(10);
}

float MQ2_ReadPPM(void) {
    const int enhanced_read_sample_times = 10;
    float rs = 0.0f;
    float Vout;
    uint16_t adc_value;

    delay_ms(100);

    for (int i = 0; i < enhanced_read_sample_times; i++) {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        uint32_t timeout = 1000000;
        while (!(ADC1->SR & ADC_SR_EOC) && timeout--) {
            if (timeout == 0) return -1.0f;
        }

        adc_value = ADC1->DR;
        Vout = ((float)adc_value / ADC_MAX) * VCC;

        if (Vout < 0.1f) return -2.0f;

        float rs_sample = RL_VALUE * (VCC - Vout) / Vout;
        if (rs_sample <= 0.0f || rs_sample > 1000.0f) {
            continue;
        }
        rs += rs_sample;
        delay_ms(50);
    }

    rs /= (float)enhanced_read_sample_times;
    if (rs <= 0.0f) return 0.0f;

    float rs_ro_ratio = rs / FIXED_RO;
    float ppm = powf(10.0f, ((log10f(rs_ro_ratio) - LPGCurve[1]) / LPGCurve[2]) + LPGCurve[0]);

    if (ppm < 0.0f) ppm = 0.0f;
    if (isnan(ppm) || ppm > 10000.0f) ppm = 10000.0f;

    return ppm;
}

uint8_t GetGasAlertState(float ppm) {
    if (ppm < 0.0f) return -1;
    else {
        if (ppm < 200.0f) return 0;
        else if (ppm < 800.0f) return 1;
        else if (ppm < 2000.0f) return 2;
        else return 3;
    }
}
