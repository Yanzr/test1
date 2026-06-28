#ifndef _SCHELDER_H_
#define _SCHELDER_H_

#include "bsp_system.h"


void schelder_init(void);
void schelder_run(void);

#include "stdint.h"

void Delay_us(uint32_t nus);
void Delay_ms(uint16_t nms);
#endif

