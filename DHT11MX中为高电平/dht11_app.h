#ifndef _DHT11_APP_H_
#define _DHT11_APP_H

#include "bsp_system.h"
#include "sys.h"

//PA11
#define DHT11_IO_IN()  {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=3<<12;} 
//IO操作函数   
#define	DHT11_DQ_OUT PAout(11)
#define	DHT11_DQ_IN  PAin(11) 

uint8_t DHT11_Init(void);//初始化DHT11
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//读取数据
void dht11_proc(void);

#endif


