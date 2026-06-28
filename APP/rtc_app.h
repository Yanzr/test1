#ifndef _RTC_APP_H_
#define _RTC_APP_H_

#include "bsp_system.h"

void rtc_proc(void);

extern RTC_TimeTypeDef   time;
extern RTC_DateTypeDef   date;
#endif



