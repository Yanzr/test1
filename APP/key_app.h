#ifndef _KEY_APP_H_
#define _KEY_APP_H_

#include "bsp_system.h"
void key_proc(void);

extern uint8_t key_val;

extern uint16_t short_count;
extern uint16_t long_count;
extern uint16_t twocount;



void key_init(void);
void key_state(void);
#endif

