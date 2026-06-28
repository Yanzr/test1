#include "key_app.h"

uint8_t key_val=0;
uint8_t key_old=0;
uint8_t key_up=0;
uint8_t key_down=0;



uint8_t key_read(void)
{
	uint8_t temp=0;
	
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
		temp=1;	
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
		temp=2;		
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
		temp=3;
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
		temp=4;
	
	return temp;
}


void key_proc(void)
{
	key_val=key_read();
	key_up=key_val&(key_val^key_old);
	key_down=~key_val&(key_val^key_old);
	key_old=key_val;
	
	if(key_val!=0)
	ucled[key_val]^=1;
	
}
	
uint16_t short_count;
uint16_t long_count;
uint16_t twocount;
////////////////////////////////状态机///////////////////////////////////////////////////
typedef struct 
{
  GPIO_TypeDef *gpiox; //指向 GPIO 端口的指针，例如 GPIOA、GPIOB 等。
	  uint16_t pin;  //指定 GPIO 引脚，例如 GPIO_PIN_0、GPIO_PIN_1 等。
	  uint16_t ticks;//用于计时的变量，通常用于去抖动处理。
	  uint8_t level; // 当前按键的电平状态，高电平或低电平。
	  uint8_t id;  // 按键的唯一标识符，可以用于区分不同的按键。
	  uint8_t state;// 按键的当前状态，可能用于表示按键是否被按下或释放。
	  uint8_t debouce_cnt;//按键去抖动计数器，用于防止按键抖动引起的误触发。
	  uint8_t repeat;  // 按键重复按下的次数。
} button;

// Function prototypes

void on_button1_short_press(void);
void on_button1_long_press(void);
void on_button2_short_press(void);
void on_button2_long_press(void);
void on_button3_long_press_up(void);
void on_button3_double_click(void);
void on_button3_and_4_pressed(void);




button btns[4]; //按键数组
//按键初始化函数
void key_init(void)
{
	  //初始化第一个按键
	  btns[0].gpiox = GPIOB; //指定GPIO端口
	  btns[0].pin = GPIO_PIN_0; //指定引脚
	  btns[0].level = 1; //设置初始电平
	  btns[0].id = 0; //设置按键ID
	  //初始化第二个按键
	  btns[1].gpiox = GPIOB;
	  btns[1].pin = GPIO_PIN_1;
	  btns[1].level = 1;
	  btns[1].id = 1;
	  //初始化第三个按键
	  btns[2].gpiox = GPIOB;
	  btns[2].pin = GPIO_PIN_2;
	  btns[2].level = 1;//状态机函数 
	  btns[2].id = 2;
	  //初始化第四个按键
	  btns[3].gpiox = GPIOA;
	  btns[3].pin = GPIO_PIN_0;
	  btns[3].level = 1;
	  btns[3].id = 3;
}



void key_task(button *btn)
{
    // 读取按键当前电平
    uint8_t gpio_level = HAL_GPIO_ReadPin(btn ->gpiox, btn ->pin);
    
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
            break;

        case 1: // 按键按下状态
            if (btn->level != 0) // 按键松开
            {
                if (btn->ticks >= 30) // 按键按下到松手超过300ms
                {	
					
					if (btn->id == 2)
					{
//////////////////////////////////////////////////////////////////////////////////////////////							
                     on_button3_long_press_up();//按键长按后松手执行的操作	
			
					}
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
                if (btn->id == 0)
				{
				on_button1_long_press(); /////按键一直长按
				}
				 else if (btn->id == 1)
				{
				on_button2_long_press(); /////按键一直长按
				}
///////////////////////////////////////////////////////////////////////////////////////////////				
				btn->repeat = 0;    // 防止释放的时候再次触发单击事件
			}
            break;
			
        case 2: // 按键释放状态
            if (btn->ticks >= 30) //松手超过300ms 计时器达到阈值
            {
                btn->state = 0; // 返回初始状态
//////////////////////////////////////////////////////////////////////////////////////////////					
                if(btn->repeat==1)//单击代码
				{	
					if (btn->id == 0)
					{
					on_button1_short_press();
					}
					else if (btn->id == 1)
					{
					on_button2_short_press();
					}	
				}
				else if(btn->repeat==2)//双击
				{					
					if (btn->id == 2)
					{
					on_button3_double_click();
					}		
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

void key_state(void)
{
    // 遍历所有按键
    for(uint8_t i = 0; i <  sizeof(btns)/sizeof(btns[0]); i++)
    {
        // 处理每个按键的状态
        key_task(&btns[i]);
    }
	
	/////////////////同时按下////////////////////////////
	    if (btns[2].level == 0 && btns[3].level == 0) // 如果按键3和4都按下
    {
        on_button3_and_4_pressed(); // 调用同时按下的处理函数
    }
}



void on_button1_short_press(void)
{
    // Implement the short press action for button 2
	short_count++;
}

void on_button1_long_press(void)
{
    // Implement the short press action for button 2
	long_count++;
}
void on_button2_short_press(void)
{
    // Implement the short press action for button 2
	short_count--;
}
void on_button2_long_press(void)
{
    // Implement the short press action for button 2
	long_count--;
}

void on_button3_long_press_up(void)
{
    // Implement the short press action for button 2
	short_count=0;
}
void on_button3_double_click(void)
{
    // Implement the double-click action for button 3
	twocount++;
}


void on_button3_and_4_pressed(void)
{
    // Implement the action for pressing button 3 and button 4 simultaneously
	twocount=0;
}









