#include "tim_app.h"
#include "tim.h"



void pwm_set_duty(float Duty,uint8_t ch)
{
	if(ch==1)
	{
		
	if (Duty < 0.0f) Duty = 0.0f; // 确保 Duty 不小于 0
    if (Duty > 100.0f) Duty = 100.0f; // 确保 Duty 不大于 100
	
 //   TIM2->CCR1 = (TIM2->ARR + 1) * (Duty / 100.0f);
		
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Duty);
	}
	
	if(ch==2)
	{
	if (Duty < 0.0f) Duty = 0.0f; // 确保 Duty 不小于 0
    if (Duty > 100.0f) Duty = 100.0f; // 确保 Duty 不大于 100
	
 //   TIM2->CCR2 = (TIM2->ARR + 1) * (Duty / 100.0f);
		
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Duty);
	}
	
}




void pwm_set_frequency(int Frequency)
{
	if (Frequency <= 0) return; // 确保 Frequency 大于 0

	 uint32_t TIM2_CLK=80000000;
	
	 uint32_t ARR_Value =TIM2_CLK/(Frequency)-1;
	
	 TIM2->CCR1 = (ARR_Value + 1) * (TIM2->CCR1 / (float)(TIM2->ARR + 1));
	
	 TIM2->ARR=ARR_Value;	
	
	 TIM2->EGR = TIM_EGR_UG;

}

//uint32_t tim_ic_buffer[64];// 定义存储输入捕获值的缓冲区
//uint32_t tim_ic_val = 0;// 最终计算得到的输入捕获值
//uint32_t tim_ic_temp = 0;// 临时存储输入捕获计算的中间值

///**
// * @brief 处理输入捕获数据
// * 
// * 此函数用于处理输入捕获数据。它计算tim_ic_buffer中所有捕获值的平均值，
// * 然后将其转换为对应的频率值（单位为Hz），并存储在tim_ic_val中。
// * 最后，调用limit_value函数对tim_ic_val进行限制，确保其在1到20000之间。
// */
//void ic_proc(void)
//{
//    tim_ic_temp = 0;
//    
//    // 对缓冲区中的捕获值求和
//    for (int i = 0; i < 64; i++)
//    {
//        tim_ic_temp += tim_ic_buffer[i];
//    }

//    // 计算平均值
//    tim_ic_temp /= 64;

//    // 计算频率值，单位为Hz
//    tim_ic_val = (int)((float)(1000.0f * 1000.0f) / (float)tim_ic_temp);

//    // 将计算得到的频率值限制在1到20000之间
//    limit_value(&tim_ic_val, 1, 500, 20000);
//}

