#ifndef MQ2_H
#define MQ2_H

#include "stm32f1xx_hal.h"
#include <math.h>

/* --- Cau hinh gia tri --- */
#define RL      5.0     // Load resistor (kOhm), thuong dung 5k - 47k
#define VCC     5.0     // Dien ap cap cho cam bien (V), thuong la 5V

// He so trong do thi log-log tren datasheet MQ2 (LPG gas)
#define A       2800.0  // He so A
#define B       -2.2    // He so B

typedef struct
{
    ADC_HandleTypeDef *hadc; // ADC handler
    uint32_t channel;        // Kenh ADC su dung
    uint16_t adc_value;      // Gia tri ADC doc duoc
    float ppm_value;         // Nong do khi doc duoc (PPM)
} MQ2_Typedef;

/* --- Prototype ham --- */
void mq2_init(MQ2_Typedef *mq2, ADC_HandleTypeDef *hadc, uint32_t channel);
void mq2_handle(MQ2_Typedef *mq2);
void calibrate_mq2(MQ2_Typedef *mq2);

#endif
