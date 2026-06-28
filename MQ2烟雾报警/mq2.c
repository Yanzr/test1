#include "mq2.h"

uint32_t dma_buff[30];
float adc_value;
float ppm;
float R0=35.904;
float RL=4.7;
float RS=0;

void mq2_proc(void)
{
    adc_value= 0;
    for (uint8_t i = 0; i < 30; i++)
    {
        adc_value+= dma_buff[i];
    }
    adc_value = adc_value/ 30 * 5.0f / 4095;
	
	
	RS=	((5.0f-adc_value)/adc_value)*RL;
	ppm = pow((RS/(R0*11.5428)),-1.5278);
	printf("ppm:%.2f\n",ppm);
}

