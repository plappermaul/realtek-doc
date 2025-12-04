/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _CA_RTC_TEST_H_
#define _CA_RTC_TEST_H_

typedef struct _RTC_VERI_PARA_ {
        u8  VeriItem;
        u8  VeriLoop;
        u8  year;
        u8  month;
        u8  week;
        u8  day;
        u8  hour;
        u8  min;
        u8  sec;      
        u8  alarm_type; // 1:secondly, 2: minutely, 3:hourly
}RTC_VERI_PARA,*PRTC_VERI_PARA;


typedef enum _RTC_VERI_ITEM_ {
    READ_CURRENT_TIME       = 1,    
		LOAD_NEW_TIME           = 2,
		ALARM_TEST              = 3,
		WAKEUP_INTERRUPT        = 4,
		BATTERY_CHECK           = 5,
		READ_RIPPLE_COUNTER     = 6,
		NONSECURE_ACCESS        = 7,
		READ_TIME_5MIN          = 8
}RTC_VERI_ITEM, *PRTC_VERI_ITEM;

#endif

