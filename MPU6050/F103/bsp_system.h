#ifndef _BSP_SYSTEM_H
#define _BSP_SYSTEM_H

#include "schelder.h"
#include "main.h"

#include "uart_app.h"
#include "mq2.h"
#include "dht11_app.h"
#include "mpu6050_app.h"



//////////////////////
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"
/////////////////////
#include "ringbuffer.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "i2c.h"

extern uint32_t dma_buff[30];
extern float adc_value;




#endif


