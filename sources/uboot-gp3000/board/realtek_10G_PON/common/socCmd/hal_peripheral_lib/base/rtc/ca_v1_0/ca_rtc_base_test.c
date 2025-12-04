/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "peripheral.h"
#include <linux/delay.h>

HAL_CA_RTC_OP HalRTCOp;
RTC_ADAPTER RTC_Adap;



VOID
SetAlarm(
    IN  VOID    *Adap
)
{
    PRTC_ADAPTER pRTC_Adap = (PRTC_ADAPTER)Adap;
    if(pRTC_Adap->alarm_type > 3 || pRTC_Adap->alarm_type==0){
        printf("wrong alarm test type\n");
        return;
    }
    
    //test 6 types alarm
    HalRTCClrAllIntrCACommon();

    SetAlarmTimeCACommon(pRTC_Adap);

    if(pRTC_Adap->alarm_type == 1) //secondly, alarm every second, so no need to set time register
        pRTC_Adap->IC_INRT_MSK = BIT_INT_SECONDLY;
    else if (pRTC_Adap->alarm_type == 2) //minutely
        pRTC_Adap->IC_INRT_MSK = BIT_INT_MINUTELY;
    else if (pRTC_Adap->alarm_type == 3) //hourly
        pRTC_Adap->IC_INRT_MSK = BIT_INT_HOURLY;
    else if (pRTC_Adap->alarm_type == 4) //weekly
        pRTC_Adap->IC_INRT_MSK = BIT_INT_WEEKLY;
    else if (pRTC_Adap->alarm_type == 5) //monthly
        pRTC_Adap->IC_INRT_MSK = BIT_INT_MONTHLY;
    else if (pRTC_Adap->alarm_type == 6) //yearly
        pRTC_Adap->IC_INRT_MSK = BIT_INT_YEARLY;

    if(pRTC_Adap->wakeup){
        pRTC_Adap->IC_INRT_MSK |= BIT_INT_WAKEUP;
        SetWakeupSelCACommon();
    } else {
        ClrWakeupSelCACommon();
    }

    HalRTCEnableIntrCACommon(pRTC_Adap);
}




/*
<0 Test Item, Dec> 
<1 Loop, Dec> 
<2 year, Dec> 
<3 month, Dec> 
<4 week, Dec> 
<5 day, Dec> 
<6 hour, Dec> 
<7 minute, Dec> 
<8 second, Dec> 
<9 alarm type, Dec>
*/

VOID    
RTCTestApp(
    IN  VOID    *InPutData
)
{
    PRTC_VERI_PARA pRTCVerParaTmp = (PRTC_VERI_PARA)InPutData;
    RTC_TIME my_time;
    u8 loop_count;
    u32 year,month,week,day,hour,min,sec;
    //memcpy(&RTC_Adap.settime,((u8*)pRTCVerParaTmp)+2,sizeof(u8)*7);
    
    RTC_Adap.settime.year = pRTCVerParaTmp->year;
    RTC_Adap.settime.month = pRTCVerParaTmp->month;
    RTC_Adap.settime.week = pRTCVerParaTmp->week;
    RTC_Adap.settime.day = pRTCVerParaTmp->day;
    RTC_Adap.settime.hour = pRTCVerParaTmp->hour;
    RTC_Adap.settime.min = pRTCVerParaTmp->min;
    RTC_Adap.settime.sec = pRTCVerParaTmp->sec;

    PHAL_CA_RTC_OP pHalRTCOp = (PHAL_CA_RTC_OP)&HalRTCOp;
    HalRTCOpInit(pHalRTCOp);

    /*printf("start testing...\n");*/

    u8 count;
    u8 delaycount;
    
    switch(pRTCVerParaTmp->VeriItem)
    {
        case 0:
            //init
            printf("RTC Init...\n");
            pHalRTCOp->HalRTCInit(&RTC_Adap);
        break;
        
    	case READ_CURRENT_TIME: // 1
            printf("RTC Read Current Time...\n");
            ReadCurrentTimeCACommon(&RTC_Adap);
            /*printf("Read Current Time = %x/%x/%x weekday %x %x:%x:%x\n",
                RTC_Adap.readtime.year,
                RTC_Adap.readtime.month,
                RTC_Adap.readtime.day,
                RTC_Adap.readtime.week,
                RTC_Adap.readtime.hour,
                RTC_Adap.readtime.min,
                RTC_Adap.readtime.sec);*/

            if(RTC_Adap.readtime.year>0 ||
               RTC_Adap.readtime.month>0 ||
               RTC_Adap.readtime.week>0 ||
               RTC_Adap.readtime.day>0 ||
               RTC_Adap.readtime.hour>0 ||
               RTC_Adap.readtime.min>0 ||
               RTC_Adap.readtime.sec>0)
            {
               printf("Pass\n");
            } else {
               printf("Fail\n");
            }
        break;

        case LOAD_NEW_TIME: // 2
            printf("RTC Load New Time...\n");
            LoadNewTimeCACommon(&RTC_Adap);
            mdelay(1000);
            ReadCurrentTimeCACommon(&RTC_Adap);

            /*printf("Load New Time = %x/%x/%x weekday %x %x:%x:%x\n",
                RTC_Adap.readtime.year,
                RTC_Adap.readtime.month,
                RTC_Adap.readtime.day,
                RTC_Adap.readtime.week,
                RTC_Adap.readtime.hour,
                RTC_Adap.readtime.min,
                RTC_Adap.readtime.sec);*/

            if(RTC_Adap.readtime.year > RTC_Adap.settime.year ||
               RTC_Adap.readtime.month > RTC_Adap.settime.month ||
               RTC_Adap.readtime.week > RTC_Adap.settime.week ||
               RTC_Adap.readtime.day > RTC_Adap.settime.day ||
               RTC_Adap.readtime.hour > RTC_Adap.settime.hour ||
               RTC_Adap.readtime.min > RTC_Adap.settime.min ||
               RTC_Adap.readtime.sec > RTC_Adap.settime.sec)
            {
               printf("Pass\n");
            } else {
               printf("Fail\n");
            }
        break;

        case ALARM_TEST: // 3
            printf("RTC Alarm test...\n");
//            for(loop_count=0;loop_count<pRTCVerParaTmp->VeriLoop;loop_count++)
            //RTC_Adap.alarm_type = pRTCVerParaTmp->alarm_type;
            RTC_Adap.alarm_type = 0x1;
            RTC_Adap.wakeup = 0;
            SetAlarm(&RTC_Adap);
            mdelay(1000);
            printf("RTC_Adap.wakeup=%d\n",RTC_Adap.wakeup);
            if (RTC_Adap.wakeup == 0)
            {
                printf("Pass\n");
            } else {
                printf("Fail\n");
            }
        break;

        case WAKEUP_INTERRUPT: // 4
            printf("RTC WAKEUP interrupt...\n");
            //no cut power test here, find no wakeup pin out
            //RTC_Adap.alarm_type = pRTCVerParaTmp->alarm_type;
            RTC_Adap.alarm_type = 0x1;
            RTC_Adap.wakeup = 1;
            SetAlarm(&RTC_Adap);
            mdelay(1000);
            printf("RTC_Adap.wakeup=%d\n",RTC_Adap.wakeup);
            if (RTC_Adap.wakeup == 1)
            {
                printf("Pass\n");
            } else {
                printf("Fail\n");
            }
        break;

        case BATTERY_CHECK: // 5 //should already be init at init function 
            printf("RTC ASIC battery check...\n");
            if(!HAL_RTC_READ8(REG_CA_RTC_BATTERY_OK)){
                printf("RTC ASIC battery drained during SoC powered off\n");
                HAL_RTC_WRITE8(REG_CA_RTC_BATTERY_OK,0x1);
            }else{
                printf("Pass\n");
            }
        break;

        case READ_RIPPLE_COUNTER: // 6
            printf("RTC read ripple counter...\n");
            ReadRippleCounterCACommon();
            mdelay(1000);
        break;

        case NONSECURE_ACCESS: // 7
            printf("RTC nonsecure access...\n");
            //to check if not secure acceess, will fail to write reg
            //change_to_non_secure(); //once set cant be set to secure again
            LoadNewTimeCACommon(&RTC_Adap);
            ReadCurrentTimeCACommon(&RTC_Adap);
            /*printf("Read Current Time = %x/%x/%x weekday %x %x:%x:%x\n",
                RTC_Adap.readtime.year,
                RTC_Adap.readtime.month,
                RTC_Adap.readtime.day,
                RTC_Adap.readtime.week,
                RTC_Adap.readtime.hour,
                RTC_Adap.readtime.min,
                RTC_Adap.readtime.sec);*/
            mdelay(1000);
            if(RTC_Adap.readtime.year > RTC_Adap.settime.year ||
               RTC_Adap.readtime.month > RTC_Adap.settime.month ||
               RTC_Adap.readtime.week > RTC_Adap.settime.week ||
               RTC_Adap.readtime.day > RTC_Adap.settime.day ||
               RTC_Adap.readtime.hour > RTC_Adap.settime.hour ||
               RTC_Adap.readtime.min > RTC_Adap.settime.min ||
               RTC_Adap.readtime.sec > RTC_Adap.settime.sec)
            {
               printf("Pass\n");
            } else {
               printf("Fail\n");
            } 
        break;

        case READ_TIME_5MIN: // 8
            printf("RTC read time 5 minute...\n");
            for(count=0;count=3;count++){
                ReadCurrentTimeCACommon(&RTC_Adap);
                /*printf("Read Current Time = %x/%x/%x weekday %x %x:%x:%x\n",
                    RTC_Adap.readtime.year,
                    RTC_Adap.readtime.month,
                    RTC_Adap.readtime.day,
                    RTC_Adap.readtime.week,
                    RTC_Adap.readtime.hour,
                    RTC_Adap.readtime.min,
                    RTC_Adap.readtime.sec);*/
                for(delaycount=0;delaycount<5000;delaycount++)
                    udelay(1000);
            }
        break;
		default:
		break;
	}
    
    //de-init
    //pHalRTCOp->HalRTCdeInit(NULL);
    
}

