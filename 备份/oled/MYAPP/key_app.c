#include "key_app.h"

uint8_t key_down;


button btns[4];

static void Btn_0_handler(button *btn)
{
    switch (btn->btn_state) {
        case BTN_STA_IDLE:
            break;
        case BTN_STA_LONG_PRESS:
            printf("0 long press!\r\n");
            break;
		
        case BTN_STA_CLICK:
            printf("0 click!\r\n");
			key_down=1;
            break;
        case BTN_STA_DOUBLE_CLICK:
            printf("0 double click!\r\n");
		    break;
        case BTN_STA_LONG_PRESS_UP:
            printf("0 long press up!\r\n");		
            break;
    }
}

static void Btn_1_handler(button *btn)
{
    switch (btn->btn_state) {
        case BTN_STA_IDLE:
            break;
        case BTN_STA_LONG_PRESS:
            printf("1 long press!\r\n");
            break;
        case BTN_STA_CLICK:
            printf("1 click!\r\n");
			key_down=2;
            break;
        case BTN_STA_DOUBLE_CLICK:
            printf("1 double click!\r\n");
			break;
        case BTN_STA_LONG_PRESS_UP:
            printf("1 long press up!\r\n");			
            break;
    }
}

static void Btn_2_handler(button *btn)
{
    switch (btn->btn_state) {
        case BTN_STA_IDLE:
            break;
        case BTN_STA_LONG_PRESS:
            printf("2 long press!\r\n");
            break;
        case BTN_STA_CLICK:
			key_down=3;
            printf("2 click!%d\r\n",key_down);
			
            break;
        case BTN_STA_DOUBLE_CLICK:
            printf("2 double click!\r\n");
			break;
        case BTN_STA_LONG_PRESS_UP:
            printf("2 long press up!\r\n");			
            break;
    }
}

static void Btn_3_handler(button *btn)
{
    switch (btn->btn_state) {
        case BTN_STA_IDLE:
            break;
        case BTN_STA_LONG_PRESS:
            printf("3 long press!\r\n");
            break;
        case BTN_STA_CLICK:
            printf("3 click!%d\r\n",key_down);
			key_down=4;
            break;
        case BTN_STA_DOUBLE_CLICK:
            printf("3 double click!\r\n");
        case BTN_STA_LONG_PRESS_UP:
            printf("3 long press up!\r\n");			
            break;
    }

    // btn - btn->id 算出基地址
    if (BTN_STA_LONG_PRESS == ((btn - btn->id) + 2)->btn_state && BTN_STA_LONG_PRESS == ((btn - btn->id) + 3)->btn_state) {
        printf("btn0 state : %d\r\n", (int)((btn - btn->id) + 0)->btn_state);
        printf("btn1 state : %d\r\n", (int)((btn - btn->id) + 1)->btn_state);
        printf("btn2 state : %d\r\n", (int)((btn - btn->id) + 2)->btn_state);
        printf("btn3 state : %d\r\n", (int)((btn - btn->id) + 3)->btn_state);
        printf("btn2 & 3 long press\r\n");
    }

    if (BTN_STA_LONG_PRESS == ((btn - btn->id) + 2)->btn_state && BTN_STA_CLICK == ((btn - btn->id) + 3)->btn_state) {
        printf("btn0 state : %d\r\n", (int)((btn - btn->id) + 0)->btn_state);
        printf("btn1 state : %d\r\n", (int)((btn - btn->id) + 1)->btn_state);
        printf("btn2 state : %d\r\n", (int)((btn - btn->id) + 2)->btn_state);
        printf("btn3 state : %d\r\n", (int)((btn - btn->id) + 3)->btn_state);
        printf("btn2 long press & btn3 click\r\n");
    }

    if (BTN_STA_LONG_PRESS == ((btn - btn->id) + 2)->btn_state && BTN_STA_DOUBLE_CLICK == ((btn - btn->id) + 3)->btn_state) {
        printf("btn0 state : %d\r\n", (int)((btn - btn->id) + 0)->btn_state);
        printf("btn1 state : %d\r\n", (int)((btn - btn->id) + 1)->btn_state);
        printf("btn2 state : %d\r\n", (int)((btn - btn->id) + 2)->btn_state);
        printf("btn3 state : %d\r\n", (int)((btn - btn->id) + 3)->btn_state);
        printf("btn2 long press & btn3 double click\r\n");
    }
}

void key_init(void)
{
    btns[0].GPIOx = GPIOB;
    btns[0].GPIO_Pin = GPIO_PIN_10;
    btns[0].level = 1;
    btns[0].id = 0;
    btns[0].button_handler = &Btn_0_handler;

    btns[1].GPIOx = GPIOB;
    btns[1].GPIO_Pin = GPIO_PIN_11;
    btns[1].level = 1;
    btns[1].id = 1;
    btns[1].button_handler = &Btn_1_handler;

    btns[2].GPIOx = GPIOA;
    btns[2].GPIO_Pin = GPIO_PIN_11;
    btns[2].level = 1;
    btns[2].id = 2;
    btns[2].button_handler = &Btn_2_handler;

    btns[3].GPIOx = GPIOA;
    btns[3].GPIO_Pin = GPIO_PIN_12;
    btns[3].level = 1;
    btns[3].id = 3;
    btns[3].button_handler = &Btn_3_handler;
}

void button_handler(button *btn)
{
    uint8_t gpio_level = HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_Pin);

    if (btn->state > 0)
        btn->ticks++;

    if (btn->level != gpio_level) 
	{
        if (++(btn->debouce_cnt) >= 3)
		{
            btn->level = gpio_level;
            btn->debouce_cnt = 0;
        }
    } 
	else 
	{
        btn->debouce_cnt = 0;
    }

    switch (btn->state) {
        case 0:
            if (btn->level == 0) {
                btn->state = 1;
                btn->ticks = 0;
                btn->repeat = 1;
            } else {
                btn->btn_state = BTN_STA_IDLE;
            }
            break;
        case 1:
            if (btn->level != 0) {
                btn->ticks = 0;
                btn->state = 2;
            } else if (btn->ticks >= 50) {
                btn->btn_state = BTN_STA_LONG_PRESS;
                btn->button_handler(btn);
                btn->ticks = 0;
                btn->repeat = 0;
            }
            break;
        case 2:
            if (btn->ticks > 15) {
                if (btn->repeat == 1) {
                    btn->btn_state = BTN_STA_CLICK;
                    btn->button_handler(btn);
                } else if (btn->repeat == 2) {
                    btn->btn_state = BTN_STA_DOUBLE_CLICK;
                    btn->button_handler(btn);
                }
                btn->state = 0;
            } else {
                if (btn->level == 0) {
                    btn->repeat++;
                    btn->state = 1;
                    btn->ticks = 0;
                }
            }
            break;
    }
}


void key_task(button *btn)
{
    // 读取按键当前电平
    uint8_t gpio_level = HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_Pin);
    
    // 如果按键状态大于 0，则递增计时器
    if (btn->state > 0)
        btn->ticks++;

    // 如果当前电平与按键记录的电平不同，进行去抖动处理
    if (btn->level != gpio_level)
    {
        // 计数达到 3 次，确认电平变化
        if (++btn->debouce_cnt >= 3)
        {
            btn->level = gpio_level; // 更新电平
            btn->debouce_cnt = 0;    // 重置去抖动计数器
        }
    }
    else
    {
        btn->debouce_cnt = 0; // 电平没有变化，重置去抖动计数器
    }

    // 按键状态机
    switch (btn->state)
    {
        case 0: // 初始状态
            if (btn->level == 0) // 按键按下
            {
                btn->ticks = 0;   // 重置计时器
                btn->repeat = 1;  // 初始按键重复计数
                btn->state = 1;   // 进入按键按下状态
            }
			else
			{
				 btn->btn_state = BTN_STA_IDLE;
			}
            break;

        case 1: // 按键按下状态
            if (btn->level != 0) // 按键松开
            {
                if (btn->ticks >= 30) // 按键按下到松手超过300ms //如果大于30则不显示抬起，只显示长按
                {	
                     btn->btn_state = BTN_STA_LONG_PRESS_UP;
                     btn->button_handler(btn);
//////////////////////////////////////////////////////////////////////////////////////////////							
					 btn->state = 0;                 
                     btn->repeat = 0;    // 防止释放的时候再次触发单击事件				
                }
				else                  // 按键按下到松手未超过300ms 
				{
					btn->ticks = 0; // 重置计时器
					btn->state = 2; // 进入按键释放状态
				}
            }
			else if(btn->ticks >= 30)   //按键未松开也就是一直长按
			{ 		
//////////////////////////////////////////////////////////////////////////////////////////////				
			    btn->btn_state = BTN_STA_LONG_PRESS;
                btn->button_handler(btn);
///////////////////////////////////////////////////////////////////////////////////////////////				
				btn->repeat = 0;    // 防止释放的时候再次触发单击事件
				//btn->ticks = 0;   //////注释是一直执行长按，不注释是，执行1次长按
			}
            break;
			
        case 2: // 按键释放状态
            if (btn->ticks >= 20) //松手超过300ms 计时器达到阈值
            {
               	btn->state = 0; // 返回初始状态.
//////////////////////////////////////////////////////////////////////////////////////////////					
				if (btn->repeat == 1) 
				{
                    btn->btn_state = BTN_STA_CLICK;
                    btn->button_handler(btn);
                } 
					else if (btn->repeat == 2) 
				{
                    btn->btn_state = BTN_STA_DOUBLE_CLICK;
                    btn->button_handler(btn);					
                }
//////////////////////////////////////////////////////////////////////////////////////////////	
				
            }
            else                   //松手后300ms内再次按下
            {
                if (btn->level == 0) // 按键再次按下
                {
                    btn->repeat++;    // 递增重复计数
                    btn->ticks = 0;   // 重置计时器
                    btn->state = 1;   // 返回按键按下状态
                }
            }
            break;
    }
	

}

void key_proc(void)
{
	key_down=0;
    for (uint8_t i = 0; i < 4; i++) {
        key_task(&btns[i]);
    }
	
	  if (key_down != 0)
  {
    auto_menu_handle_key(key_down); // 将按键传递给菜单系统进行处理
  }
  key_down=0;
}
