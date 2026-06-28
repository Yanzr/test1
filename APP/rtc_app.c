#include "rtc_app.h"
#include "rtc.h"

RTC_TimeTypeDef   time;
RTC_DateTypeDef   date;



void rtc_proc(void)
{
  HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	

}




