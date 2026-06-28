#include "system.h"


void system_init(void)
{

	//GPIOC高八位设置为1
	GPIOC->ODR |=0XFF00;
	//置位GPIOD2第二位
	GPIOD->BSRR |=0X01<<2;
	//复位GPIOD2第二位
	GPIOD->BRR |= 0X01<<2;

}
