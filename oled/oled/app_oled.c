#include "app_oled.h"
#include <stdbool.h>


extern double rms1;
extern double rms2;
extern volatile float pwm_output_channel_1;
extern bool Pwm_Pid_Ctrl;
extern uint16_t data;
extern double avg1;
extern double avg2;
uint16_t buffer[128];

uint32_t count = 0;

bool oled_disp_mode;

void OLED_Proc(void)
{
   OLED_operate_gram(PEN_CLEAR);
    
    if(oled_disp_mode == false)
    {
//         LcdSprintf(COLUMN0, LINE0, "sys_count1:%.2f",adc_value[0]);
//		 LcdSprintf(COLUMN0, LINE1, "sys_count2:%.2f",adc_value[1]);
//		 LcdSprintf(COLUMN0, LINE2, "sys_count3:%.2f",adc_value[2]);
//		 LcdSprintf(COLUMN0, LINE3, "key:%d",key_down);
		auto_menu_display();
    }
    else
    {
        OLED_Draw_Line(buffer);
    }
    
    OLED_refresh_gram();
}
