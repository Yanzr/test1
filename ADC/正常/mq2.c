#include "mq2.h"

uint32_t dma_buff[30];

float adc_value = 0;
float voltage = 0;
float RS = 0;
float RL = 4.7;
float R0 = 35.904;
float ppm = 0;

void mq2_task(void)
{
    adc_value = 0;
    for (uint8_t i = 0; i < 30; i++)
    {
        adc_value += dma_buff[i];
    }
    voltage = (float)(adc_value / 30.0f) / 4095 * 5.0f;
//    printf("voltage:%f\r\n", voltage);
    RS = ((5.0f - voltage) / voltage) * RL;
    // 测丙烷：Rs / R0 = 11.5428 * ppm ^ (-0.6549)
    ppm = pow((RS / (R0 * 11.5428)), -1.5278);
    printf("ppm:%.2f\r\n", ppm);
}

// �����ȡ
// void mq2_task(void)
//{
//     HAL_ADC_Start(&hadc1);
//     HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//     if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
//     {
//         adc_value = HAL_ADC_GetValue(&hadc1);
//         voltage= (float)adc_value/4095*5.0f;
//         printf("voltage:%f\r\n",voltage);
//     }
// }
