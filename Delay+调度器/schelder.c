#include "schelder.h"

uint8_t task_num;

typedef struct
{
	void (*task_func)(void);
	uint32_t rate_ms;
	uint32_t last_run;
} task_t;




static  task_t scheduler_task[]=
{
	{uart_proc,10,0},
	{dht11_proc,1000,0},
//	{mq2_proc,100,0},
//	{key_proc,10,0}
	
};

void schelder_init(void)
{
  	task_num=sizeof(scheduler_task)/sizeof(task_t);
}

void schelder_run(void)
{
	uint32_t now_time;
	for(uint8_t i=0;i<task_num;i++)
	{
		now_time=HAL_GetTick();
		if(now_time>=scheduler_task[i].rate_ms+scheduler_task[i].last_run)
		{
		scheduler_task[i].last_run=now_time;
		scheduler_task[i].task_func();
				
		}
	
	}

}


/* USER CODE BEGIN 1 */
#include "tim.h"
/*
    普通定时器实现us延时
*/
void Delay_us(uint32_t nus)
{
 
    uint16_t  differ = 0xffff-nus-5;
    //设置定时器2的技术初始值
  __HAL_TIM_SetCounter(&htim1,differ);
  //开启定时器
  HAL_TIM_Base_Start(&htim1);
 
  while( differ<0xffff-5)
    {
        differ = __HAL_TIM_GetCounter(&htim1);
    };
 //关闭定时器
  HAL_TIM_Base_Stop(&htim1);
}
/*
    普通定时器实现ms延时，可直接使用HAL库函数HAL_delay（）
*/
void Delay_ms(uint16_t nms)
{
    uint32_t i;
    for(i=0;i<nms;i++) Delay_us(1000);
}









