#include "led_app.h"


uint8_t ucled[8]={0,1,0,1,0,1,0,1};

void ucled_disp(uint8_t *ucled)
{
	uint8_t temp=0x00;
	static uint8_t temp_old=0xff;
	
	for(uint8_t i=0 ;i<8;i++)
	{
	temp|=ucled[i]<<(7-i);
	}
	
	if(temp!=temp_old)
	{
	GPIOC->ODR &=0X00FF;
	GPIOC->ODR |=~(temp<<8);
	GPIOD->BSRR |=0x01<<2;
	GPIOD->BRR |=0x01<<2;
	temp_old=temp;	
	}
	
	
}

void led_proc(void)
{
	ucled_disp(ucled);
}

