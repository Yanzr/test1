#ifndef _TIM_APP_H_
#define _TIM_APP_H_
#include "bsp_system.h"

void pwm_set_duty(float Duty,uint8_t ch);
void ic_proc(void);
void pwm_set_frequency(int Frequency);



extern uint32_t tim_ic_buffer[64];// 定义存储输入捕获值的缓冲区
extern uint32_t tim_ic_val ;// 最终计算得到的输入捕获值
extern uint32_t tim_ic_temp ;// 临时存储输入捕获计算的中间值
#endif


