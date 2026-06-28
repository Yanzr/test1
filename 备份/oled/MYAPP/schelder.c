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
	{OLED_Proc,100,0},
	{adc_proc,10,0},
	{key_proc,10,0}
	
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










