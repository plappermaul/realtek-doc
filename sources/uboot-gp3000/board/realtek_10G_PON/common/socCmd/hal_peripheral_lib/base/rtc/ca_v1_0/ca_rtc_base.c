/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "platform_autoconf.h" 
#include "diag.h"
#include "peripheral.h"
#include <linux/delay.h>


VOID
RTCInterruptHandle(
    IN  VOID *Data

)
{
    PRTC_ADAPTER pRTCDatAdapter = (PRTC_ADAPTER) Data;

    u32 interrupt_status;
    interrupt_status = HAL_RTC_READ32(REG_CA_RTC_INT_STATUS);
    /*printf("RTC interrupt status: 0x%x\n",interrupt_status);*/
    //disable all interrupt (there should be only an interrupt is enabled at a time)
    HAL_RTC_WRITE32(REG_CA_RTC_INT_EN,0x0);
    ReadCurrentTimeCACommon(pRTCDatAdapter);
    /*printf("Read Current Time = %x/%x/%x weekday %x %x:%x:%x\n",
            pRTCDatAdapter->readtime.year,
            pRTCDatAdapter->readtime.month,
            pRTCDatAdapter->readtime.day,
            pRTCDatAdapter->readtime.week,
            pRTCDatAdapter->readtime.hour,
            pRTCDatAdapter->readtime.min,
            pRTCDatAdapter->readtime.sec);*/
}

BOOL
HalRTCInitCACommon(
    IN  VOID    *Data
)
{
    IRQ_HANDLE          RTCIrqHandle;
    u16 delay_i;

    PRTC_ADAPTER pRTCDatAdapter = (PRTC_ADAPTER) Data;

    RTCIrqHandle.Data = (u32) pRTCDatAdapter;
    RTCIrqHandle.IrqNum = 86; //32+54
    RTCIrqHandle.IrqFun = (IRQ_FUN) RTCInterruptHandle;
    RTCIrqHandle.Priority = 0;    

    if(!BIT_GET_WRITE_ENABLE(HAL_RTC_READ32(REG_CA_RTC_CTRL))) //if it's 0
        HAL_RTC_WRITE32(REG_CA_RTC_CTRL, HAL_RTC_READ32(REG_CA_RTC_CTRL) | BIT(2) | BIT_SET_WRITE_ENABLE(0x1));

    //to delay 3 sec for init complete

    for(delay_i=0;delay_i<3;delay_i++)
        mdelay(1000);

    if(!HAL_RTC_READ32(REG_CA_RTC_BATTERY_OK)){
        HAL_RTC_WRITE32(REG_CA_RTC_BATTERY_OK,HAL_RTC_READ32(REG_CA_RTC_BATTERY_OK) | 0x1);
        printf("RTC battery drained during SoC powered off.\n");
    }


    HAL_RTC_WRITE32(REG_CA_RTC_BATTERY_OK,HAL_RTC_READ32(REG_CA_RTC_BATTERY_OK) & ~BIT12);
    
    #if CONFIG_DEBUG_LOG
    //printf("RTC_WRITE_ENABLE(%x): %x\n", (RTC_REG_BASE+REG_CA_RTC_CTRL), HAL_RTC_READ32(REG_CA_RTC_CTRL));
    #endif

    //clear mis-triggered interrupt
    HAL_RTC_READ32(REG_CA_RTC_INT_STATUS);
    HAL_RTC_WRITE32(REG_CA_RTC_INT_EN,0x0);

    //register interrupt
    //arm
    //irq_install_handler(RTCIrqHandle.IrqNum, RTCIrqHandle.IrqFun, RTCIrqHandle.Data);
    irq_install_handler(RTCIrqHandle.IrqNum, RTCInterruptHandle, RTCIrqHandle.Data);

    printf("RTC init Pass\n");
    return _TRUE;
}

BOOL
HalRTCdeInitCACommon(
    IN  VOID    *Data
)
{
    HAL_RTC_WRITE32(REG_CA_RTC_CTRL,HAL_RTC_READ32(REG_CA_RTC_CTRL) & ~BIT0);
    return _TRUE;
}

VOID
HalRTCClrIntrCACommon(
    IN  VOID    *Data
){
    

}

VOID
HalRTCClrAllIntrCACommon(
){
    HAL_RTC_WRITE32(REG_CA_RTC_INT_EN,0x0);
}


VOID
HalRTCEnableIntrCACommon(
    IN  VOID    *Data
){
    PRTC_ADAPTER pRTCDatAdapter = (PRTC_ADAPTER) Data;
    u8 RTCIntrToEn = pRTCDatAdapter->IC_INRT_MSK;
    HAL_RTC_WRITE32(REG_CA_RTC_INT_EN,RTCIntrToEn);   
}


VOID
ReadCurrentTimeCACommon(
    IN  VOID    *Data
)
{
    PRTC_ADAPTER pRTCDatAdapter = (PRTC_ADAPTER) Data;
    u32 change_indi_1, change_indi_2;
    u32 year,month,week,day,hour,min,sec;

    change_indi_1 = BIT_GET_CHANGING(HAL_RTC_READ32(REG_CA_RTC_CHANGING));
    year = BIT_GET_BCD_YEAR(HAL_RTC_READ32(REG_CA_RTC_BCD_YEAR));
    month = BIT_GET_BCD_MONTH(HAL_RTC_READ32(REG_CA_RTC_BCD_MONTH));
    week = BIT_GET_BCD_WEEK(HAL_RTC_READ32(REG_CA_RTC_BCD_WEEK));
    day = BIT_GET_BCD_DAY(HAL_RTC_READ32(REG_CA_RTC_BCD_DAY));
    hour = BIT_GET_BCD_HOUR(HAL_RTC_READ32(REG_CA_RTC_BCD_HOUR));
    min = BIT_GET_BCD_MIN(HAL_RTC_READ32(REG_CA_RTC_BCD_MIN));
    sec = BIT_GET_BCD_SEC(HAL_RTC_READ32(REG_CA_RTC_BCD_SEC));
    change_indi_2 = BIT_GET_CHANGING(HAL_RTC_READ32(REG_CA_RTC_CHANGING));

    if(change_indi_1 == 1 && change_indi_2==0){ //read agian
        year = BIT_GET_BCD_YEAR(HAL_RTC_READ32(REG_CA_RTC_BCD_YEAR));
        month = BIT_GET_BCD_MONTH(HAL_RTC_READ32(REG_CA_RTC_BCD_MONTH));
        week = BIT_GET_BCD_WEEK(HAL_RTC_READ32(REG_CA_RTC_BCD_WEEK));
        day = BIT_GET_BCD_DAY(HAL_RTC_READ32(REG_CA_RTC_BCD_DAY));
        hour = BIT_GET_BCD_HOUR(HAL_RTC_READ32(REG_CA_RTC_BCD_HOUR));
        min = BIT_GET_BCD_MIN(HAL_RTC_READ32(REG_CA_RTC_BCD_MIN));
        sec = BIT_GET_BCD_SEC(HAL_RTC_READ32(REG_CA_RTC_BCD_SEC));
    }
    pRTCDatAdapter->readtime.year = year;
    pRTCDatAdapter->readtime.month = month;
    pRTCDatAdapter->readtime.week = week;
    pRTCDatAdapter->readtime.day = day;
    pRTCDatAdapter->readtime.hour = hour;
    pRTCDatAdapter->readtime.min = min;
    pRTCDatAdapter->readtime.sec = sec;
}

VOID
ReadRippleCounterCACommon(

)
{
    u32 ripple_counter_1,ripple_counter_2,ripple_counter_3,ripple_counter_4;
    u8 read1_ok=0;
    u8 read2_ok=0;
    HAL_RTC_WRITE32(REG_CA_RTC_RIPPLE_COUNTER,BIT_RIPPLE_COUNTER_READ_EN);
    ripple_counter_1 = BIT_GET_RIPPLE_COUNTER(HAL_RTC_READ32(REG_CA_RTC_RIPPLE_COUNTER));
    ripple_counter_2 = BIT_GET_RIPPLE_COUNTER(HAL_RTC_READ32(REG_CA_RTC_RIPPLE_COUNTER));
    
    if(ripple_counter_1 != ripple_counter_2){
        //printf("Read Ripple Counter twice get different value: 0x%x, 0x%x\n",ripple_counter_1,ripple_counter_2);
        printf("Fail\n");
    }
    else{
        //printf("Read Ripple Counter = 0x%x\n",ripple_counter_1);
        read1_ok=1;
    }
    HAL_RTC_WRITE32(REG_CA_RTC_RIPPLE_COUNTER,0x0);

    mdelay(100);
    HAL_RTC_WRITE32(REG_CA_RTC_RIPPLE_COUNTER,BIT_RIPPLE_COUNTER_READ_EN);
    ripple_counter_3 = BIT_GET_RIPPLE_COUNTER(HAL_RTC_READ32(REG_CA_RTC_RIPPLE_COUNTER));
    ripple_counter_4 = BIT_GET_RIPPLE_COUNTER(HAL_RTC_READ32(REG_CA_RTC_RIPPLE_COUNTER));
    
    if(ripple_counter_3 != ripple_counter_4){
        //printf("Read Ripple Counter twice get different value: 0x%x, 0x%x\n",ripple_counter_1,ripple_counter_2);
        printf("Fail\n");
    }
    else{
        //printf("Read Ripple Counter = 0x%x\n",ripple_counter_1);
        read2_ok=1;
    }
    /*if(read1_ok && read2_ok)
        printf("Read Ripple Counter (100ms)= 0x%x, 0x%x\n",ripple_counter_1,ripple_counter_3 );
    */
    HAL_RTC_WRITE32(REG_CA_RTC_RIPPLE_COUNTER,0x0);
    printf("Pass\n");
}

VOID
LoadNewTimeCACommon(
    IN  VOID    *Data
)
{
    PRTC_ADAPTER pRTCDatAdapter = (PRTC_ADAPTER) Data;

    u32  int_save;
    int_save = HAL_RTC_READ32(REG_CA_RTC_INT_EN);
    HAL_RTC_WRITE32(REG_CA_RTC_INT_EN,0x0);

    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_YEAR, pRTCDatAdapter->settime.year);
    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_MONTH, pRTCDatAdapter->settime.month);
    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_WEEK, pRTCDatAdapter->settime.week);
    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_DAY, pRTCDatAdapter->settime.day);
    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_HOUR, pRTCDatAdapter->settime.hour);
    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_MIN, pRTCDatAdapter->settime.min);
    HAL_RTC_WRITE32(REG_CA_RTC_LOAD_BCD_SEC, pRTCDatAdapter->settime.sec);
    
    mdelay(10);
    
    HAL_RTC_WRITE32(REG_CA_RTC_INT_EN,int_save);
}

VOID
SetAlarmTimeCACommon(
    IN  VOID    *Adap
)
{
    PRTC_ADAPTER pRTCDatAdapter = (PRTC_ADAPTER) Adap;

    //for test,  set min and sec is enough
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_MIN, pRTCDatAdapter->settime.min);
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_SEC, pRTCDatAdapter->settime.sec);
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_HOUR, pRTCDatAdapter->settime.hour);
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_DAY, pRTCDatAdapter->settime.day);
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_WEEK, pRTCDatAdapter->settime.week);
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_MONTH, pRTCDatAdapter->settime.month);
    HAL_RTC_WRITE32(REG_CA_RTC_ALARM_YEAR, pRTCDatAdapter->settime.year);
}

VOID
SetWakeupSelCACommon(
)
{
    HAL_RTC_WRITE32(REG_CA_RTC_CTRL, HAL_RTC_READ32(REG_CA_RTC_CTRL) | BIT_SEL_WAKEUP_INT);
}

VOID
ClrWakeupSelCACommon(
)
{
    HAL_RTC_WRITE32(REG_CA_RTC_CTRL, HAL_RTC_READ32(REG_CA_RTC_CTRL) & ~BIT_SEL_WAKEUP_INT);
}


