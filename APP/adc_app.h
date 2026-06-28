#ifndef _ADC_APP_H_
#define _ADC_APP_H_
#include "bsp_system.h"

void adc_proc(void);
extern uint32_t dma_buff[2][30];
extern float adc_value[2];
#endif
