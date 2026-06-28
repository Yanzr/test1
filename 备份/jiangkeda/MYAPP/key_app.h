#ifndef _KEY_APP_H_
#define _KEY_APP_H_

#include "bsp_system.h"
void key_proc(void);


extern uint8_t key_down;
typedef enum {
    BTN_STA_IDLE,
    BTN_STA_LONG_PRESS,
	BTN_STA_LONG_PRESS_UP,
    BTN_STA_CLICK,
    BTN_STA_DOUBLE_CLICK,
} button_state_t;

typedef struct btn {
    uint16_t ticks;
    uint8_t level;
    uint8_t id;
    uint8_t state;
    uint8_t repeat;
    uint8_t debouce_cnt;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    button_state_t btn_state;
    void (*button_handler)(struct btn *btn);
} button;

void key_init(void);
void key_proc(void);



#endif

