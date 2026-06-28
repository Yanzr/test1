#include "adc_app.h"

uint32_t dma_buff[120];
float adc_value[4];


void adc_proc(void)
{
    adc_value[0] = adc_value[1] = adc_value[2] = adc_value[3] = 0;

    for (uint8_t i = 0; i < 30; i++)
    {
        adc_value[0] += dma_buff[i * 4 + 0];
        adc_value[1] += dma_buff[i * 4 + 1];
        adc_value[2] += dma_buff[i * 4 + 2];
        adc_value[3] += dma_buff[i * 4 + 3];
    }

    adc_value[0] = adc_value[0] / 30 * 3.3f / 4095;
    adc_value[1] = adc_value[1] / 30 * 3.3f / 4095;
    adc_value[2] = adc_value[2] / 30 * 3.3f / 4095;
    adc_value[3] = adc_value[3] / 30 * 3.3f / 4095;

}

