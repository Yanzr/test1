#include "lcd_app.h"
#include "stdio.h"


void Lcd_Sprinrf(uint8_t line,char *format,...)	
{
 char string[21];
	va_list arg;
	va_start(arg,format);
	vsprintf(string, format, arg);
	va_end(arg);
	LCD_DisplayStringLine(line,(u8 *)string);


}
uint8_t NUMMM;
void lcd_proc(void)
{
	
	mcp4017_write(100);
	mcp4017_read(&NUMMM);
	Lcd_Sprinrf(Line0,"ADC1:%.2f     ",adc_value[0]);
	Lcd_Sprinrf(Line1,"ADC2:%.2f     ",adc_value[1]);
	Lcd_Sprinrf(Line2,"R:%d  ",NUMMM);
	Lcd_Sprinrf(Line3,"hou:%d  ",time.Hours);
	Lcd_Sprinrf(Line4,"sec:%d  ",time.Seconds);
	



}
	
