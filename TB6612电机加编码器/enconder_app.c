#include "enconder_app.h"
#include "tim.h"


int16_t Encoder_GetLocation(uint8_t n)
{
    if (n == 1)
    {
        return (int16_t)__HAL_TIM_GET_COUNTER(&htim3); // 获取 TIM3 的计数值
    }
    else if (n == 2)
    {
        return (int16_t)__HAL_TIM_GET_COUNTER(&htim4); // 获取 TIM4 的计数值
    }
    return 0;
}

/**
 * @brief 设置编码器计数器的值
 * @param n: 定时器编号 (1 对应 TIM3，2 对应 TIM4)
 * @param Location: 要设置的计数值
 */
void Encoder_SetLocation(uint8_t n, int16_t Location)
{
    if (n == 1)
    {
        __HAL_TIM_SET_COUNTER(&htim3, (uint16_t)Location); // 设置 TIM3 的计数值
    }
    else if (n == 2)
    {
        __HAL_TIM_SET_COUNTER(&htim4, (uint16_t)Location); // 设置 TIM4 的计数值
    }
}

uint8_t tim2ITcount=0;
short Encoder1Count=0,Encoder2Count=0;
float motor1Speed=0,motor2Speed=0;

/*
计算电机的速度。笔者这里使用了减速电机，减速比为1：21.3，也就是说，电机轴转21.3圈，
经变速齿轮后，假设变速齿轮上负载了一个轮子， 轮子转一圈，实际电机轴转了21.3圈。
我使用的编码器轴转一圈，输出11个脉冲。定时器中，我们使用了双通道计数，会对脉冲四倍频。
也就是说，轮转一圈，定时器的到的脉冲数为：21.3*11*4。如果我们获得1s的脉冲数，再除以一圈产生的脉冲数，能得到轮子1s转的圈数。
上文，我们已经得到了10ms的脉冲数，*100不就是1s的脉冲数。  *Π*D则是()/s的距离，（）决定于D

*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
		{ 
	tim2ITcount++;
	if(tim2ITcount ==100){	//10ms进一次中断，
	tim2ITcount=0;

	  Encoder1Count = (short)__HAL_TIM_GET_COUNTER(&htim3);	//编码器1计数值
	  Encoder2Count = (short)__HAL_TIM_GET_COUNTER(&htim4); //编码器2计数值

	  
	 __HAL_TIM_SET_COUNTER(&htim3,0);
	 __HAL_TIM_SET_COUNTER(&htim4,0);
	  
	  motor1Speed = (float)Encoder1Count*100*3.14*2/21.3/11/4; //每秒多少cm
	  motor2Speed = -(float)Encoder2Count*100*3.14*2/21.3/11/4;		
						}
		}	

}

