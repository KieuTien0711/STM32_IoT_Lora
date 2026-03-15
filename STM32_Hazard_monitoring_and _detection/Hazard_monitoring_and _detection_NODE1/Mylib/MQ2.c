#include "MQ2.h"
#include "main.h" // Neu ban dung HAL trong main, co the bo sung
#include <stdio.h>

static float R0 = -1; // Bien luu gia tri R0, private trong file nay

// Ham doc ADC tu MQ2
static uint32_t MQ2_ReadADC(MQ2_Typedef *mq2)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = mq2->channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(mq2->hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_ADC_Start(mq2->hadc);
    HAL_ADC_PollForConversion(mq2->hadc, 100);
    uint32_t adc_val = HAL_ADC_GetValue(mq2->hadc);
    HAL_ADC_Stop(mq2->hadc);

    return adc_val;
}

// Tinh Rs tu gia tri ADC
static double MQ2_CalcRs(uint32_t adc_val)
{
    if (adc_val == 0) return -1;

    double Vout = ((double)adc_val / 4095.0) * VCC;
    if (Vout <= 0) return -1;

    double Rs = RL * ((VCC / Vout) - 1.0);
    return Rs;
}

// Ham hieu chuan R0
void calibrate_mq2(MQ2_Typedef *mq2)
{
    int sampleCount = 50;
    double rs_sum = 0;
    int validSamples = 0;

    for (int i = 0; i < sampleCount; i++)
    {
        uint32_t adc_val = MQ2_ReadADC(mq2);
        double rs = MQ2_CalcRs(adc_val);
        if (rs > 0)
        {
            rs_sum += rs;
            validSamples++;
        }
        HAL_Delay(100);
    }

    if (validSamples > 0)
    {
        double rs_avg = rs_sum / validSamples;
        R0 = rs_avg / 9.83; // Ti le Rs/R0 trong khong khi sach theo datasheet
    }
    else
    {
        R0 = -1;
    }
}

// Ham doc ppm
void read_mq2_ppm(MQ2_Typedef *mq2)
{
    if (R0 <= 0)
    {
        calibrate_mq2(mq2);
        if (R0 <= 0)
        {
            mq2->ppm_value = -1;
            return;
        }
    }

    uint32_t adc_val = MQ2_ReadADC(mq2);
    mq2->adc_value = adc_val;

    double rs = MQ2_CalcRs(adc_val);
    if (rs <= 0)
    {
        mq2->ppm_value = -1;
        return;
    }

    double ratio = rs / R0;
    double ppm = A * pow(ratio, B);

    if (ppm < 0) ppm = 0;

    mq2->ppm_value = ppm;
}

// Ham xu ly chinh, goi ham doc ppm
void mq2_handle(MQ2_Typedef *mq2)
{
    read_mq2_ppm(mq2);
}

// Khoi tao MQ2
void mq2_init(MQ2_Typedef *mq2, ADC_HandleTypeDef *hadc, uint32_t channel)
{
    mq2->hadc = hadc;
    mq2->channel = channel;
    mq2->adc_value = 0;
    mq2->ppm_value = 0;

    calibrate_mq2(mq2);
}
