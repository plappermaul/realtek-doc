/*
 * Copyright (C) 2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Main function of the Time Synchronous protocol stack user application
 *           It create two additional threads for packet Rx and state control
 *
 * Feature : Start point of the Time Synchronous protocol stack. Use individual threads
 *           for packet Rx and state control
 *
 */
#ifdef CONFIG_RTK_PON_TOD_TIME_FREQ

/*
 * Include Files
 */
#include <stdio.h>
#include <semaphore.h>
#include <string.h> 
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <malloc.h> 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>

#include "rtk/time.h"

#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/syscall.h>

#include "module/pontod_freq/pontod_freq_drv_main.h"
#include "pontod_freq_main.h"
#include "pontod_freq_calc.h"

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
/*
 * Symbol Definition
 */

/*
 * Macro Definition
 */

/*
 * Data Declaration
 */
static uint32 storeCnt=0;

static double max_ratio=1;
static double min_ratio=1;
static double avg_ratio=1;
static double total_ratio=0;
extern uint32 sample_num;
extern uint32 avg_num;

extern rtk_enable_t overflow_flag;
extern uint32 tickMaxValue;
extern uint64 freeTimeMaxValue;
extern int debug;
extern rtk_enable_t logging_flag;
extern char filePath[200];
extern rtk_enable_t adjustFreq_flag;
extern rtk_enable_t qAdjustFlag;

#define RTK_API_ERR_CHK(op, ret)\
    do {\
        if ((ret = (op)) != RT_ERR_OK)\
        {\
            printf("[%s:%d] Rtk API Error! ret=%d \n", __FUNCTION__, __LINE__,ret); \
        }\
    } while(0)

/*
 * Function Declaration
 */

void pontod_freq_cal_logging(char *logBuf,int len)
{
    int wp=-1;

    if(debug>=2)
    {
        printf("%s",logBuf);
    }

    if(logging_flag==ENABLED)
    {
        wp = open(filePath,O_RDWR|O_CREAT|O_APPEND);
        if(wp == -1)
        {
            printf("[%s %d] cant't create filePath=%s logBuf=%s\n",__FUNCTION__,__LINE__,filePath,logBuf);
            return ;
        }
        write(wp,logBuf,len);
        close(wp);
    }
}

void pontod_freq_cal_ratio(PonToDNLData_t* item)
{
    uint32 SF_tick;
    uint64 SF_time;
    uint32 SE_tick;
    uint64 SE_time;
    rtk_enable_t SF_overflow=DISABLED;
    rtk_enable_t SE_overflow=DISABLED;
    rtk_enable_t illegalFreq=DISABLED;
    int len=0;
    char logBuf[1000];
    double freq_ratio=0;
    double tmp_ratio=0;

    if((item->val.timeTick.F_time <= item->val.timeTick.S_time)||
        (item->val.timeTick.E_time <= item->val.timeTick.S_time)||
        (item->val.timeTick.F_tick == item->val.timeTick.S_tick)||
        (item->val.timeTick.E_tick == item->val.timeTick.S_tick))
    {
        len+=sprintf(logBuf+len,"#####################\n");
        len+=sprintf(logBuf+len,"[%s:%d] tick or time is error\n",__FUNCTION__,__LINE__);
        len+=sprintf(logBuf+len,"[%s:%d] S tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.S_tick,item->val.timeTick.S_time);
        len+=sprintf(logBuf+len,"[%s:%d] F tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.F_tick,item->val.timeTick.F_time);
        len+=sprintf(logBuf+len,"[%s:%d] E tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.E_tick,item->val.timeTick.E_time);
        len+=sprintf(logBuf+len,"#####################\n");
        pontod_freq_cal_logging(logBuf,len);
        return;
    }

    //calculate interval of Start and Freerun
    if(item->val.timeTick.F_tick < item->val.timeTick.S_tick) //overflow
    {
        if(overflow_flag==ENABLED)
        {
            SF_tick = tickMaxValue - item->val.timeTick.S_tick + item->val.timeTick.F_tick;
            SF_overflow = ENABLED;
        }
        else
        {
            len+=sprintf(logBuf+len,"#####################\n");
            len+=sprintf(logBuf+len,"[%s:%d] S tick to F tick is overflow\n",__FUNCTION__,__LINE__);
            len+=sprintf(logBuf+len,"[%s:%d] S tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.S_tick,item->val.timeTick.S_time);
            len+=sprintf(logBuf+len,"[%s:%d] F tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.F_tick,item->val.timeTick.F_time);
            len+=sprintf(logBuf+len,"[%s:%d] E tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.E_tick,item->val.timeTick.E_time);
            len+=sprintf(logBuf+len,"#####################\n");
            pontod_freq_cal_logging(logBuf,len);
            return;
        }

    }
    else
    {
        SF_tick = item->val.timeTick.F_tick - item->val.timeTick.S_tick;
    }

    SF_time = item->val.timeTick.F_time - item->val.timeTick.S_time;

    //calculate interval of Start and End
    if(item->val.timeTick.E_tick < item->val.timeTick.S_tick) //overflow
    {
        if(overflow_flag==ENABLED)
        {
            SE_tick = tickMaxValue - item->val.timeTick.S_tick+item->val.timeTick.E_tick;
            SE_overflow = ENABLED;
        }
        else
        {
            len+=sprintf(logBuf+len,"#####################\n");
            len+=sprintf(logBuf+len,"[%s:%d] S tick to E tick is overflow\n",__FUNCTION__,__LINE__);
            len+=sprintf(logBuf+len,"[%s:%d] S tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.S_tick,item->val.timeTick.S_time);
            len+=sprintf(logBuf+len,"[%s:%d] F tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.F_tick,item->val.timeTick.F_time);
            len+=sprintf(logBuf+len,"[%s:%d] E tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeTick.E_tick,item->val.timeTick.E_time);
            len+=sprintf(logBuf+len,"#####################\n");
            pontod_freq_cal_logging(logBuf,len);
            return;
        }
    }
    else
    {
        SE_tick = item->val.timeTick.E_tick - item->val.timeTick.S_tick;
    }

    SE_time = item->val.timeTick.E_time - item->val.timeTick.S_time;


    storeCnt++;

    len+=sprintf(logBuf+len,"##########%u##########\n",storeCnt);
    len+=sprintf(logBuf+len,"S tick=%u time=%llu\n",item->val.timeTick.S_tick,item->val.timeTick.S_time);
    len+=sprintf(logBuf+len,"F tick=%u time=%llu\n",item->val.timeTick.F_tick,item->val.timeTick.F_time);
    len+=sprintf(logBuf+len,"E tick=%u time=%llu\n",item->val.timeTick.E_tick,item->val.timeTick.E_time);
    len+=sprintf(logBuf+len,"%sSF tick=%u time=%llu time/tick==%.9llf\n",(SF_overflow==ENABLED)?"overflow ":"",SF_tick,SF_time,(double)((double)SF_time / (double)SF_tick));
    len+=sprintf(logBuf+len,"%sSE tick=%u time=%llu time/tick==%.9llf\n",(SE_overflow==ENABLED)?"overflow ":"",SE_tick,SE_time,(double)((double)SE_time / (double)SE_tick));
    
    //calculate frequency ratio
    freq_ratio=(double)(((double)((double)SE_time / (double)SE_tick)) / ((double)((double)SF_time / (double)SF_tick)));

    if(sample_num==0) // not minus maximum and minimum ratio
    {
        if(storeCnt==1)
        {
            avg_ratio=freq_ratio;
            max_ratio=freq_ratio;
            min_ratio=freq_ratio;
        }
        else
        {
            if(max_ratio<freq_ratio)
                max_ratio=freq_ratio;

            if(min_ratio>freq_ratio)
                min_ratio=freq_ratio;
            //check the average range not viration too much
            if(freq_ratio>avg_ratio*1.5||freq_ratio<avg_ratio*0.5)
            {
                illegalFreq=ENABLED;
            }
            else
            {
                avg_ratio=avg_ratio*0.8+freq_ratio*0.2;
            }
        }

        if(qAdjustFlag==ENABLED)
            pontod_freq_set_cal_ratio();

        len+=sprintf(logBuf+len,"%sFreq_Ratio=%.9llf Avg=%.9llf\n",(illegalFreq==ENABLED)?"illegal ":"",freq_ratio,avg_ratio);
        len+=sprintf(logBuf+len,"Max=%.9llf Min=%.9llf\n",max_ratio,min_ratio);
    }
    else //minus maximum and minimum ratio
    {
        if(storeCnt%sample_num==1)
        {
            //First time don't need to calculate frequency ratio
            if(storeCnt!=1)
            {
                tmp_ratio=(total_ratio-max_ratio-min_ratio)/avg_num; //remove the max and min value

                if(avg_ratio!=1)
                {
                    avg_ratio=avg_ratio*0.7+tmp_ratio*0.3;
                }
                else
                {
                    avg_ratio=tmp_ratio;
                }

                if(qAdjustFlag==ENABLED)
                    pontod_freq_set_cal_ratio();
            }

            max_ratio=freq_ratio;
            min_ratio=freq_ratio;
            total_ratio=freq_ratio;
        }
        else
        {
            total_ratio+=freq_ratio;
        }

        //Check ratio with maximum
        if(max_ratio<freq_ratio)
        {
            max_ratio=freq_ratio;
        }

        //Check ratio with minimum
        if(min_ratio>freq_ratio)
        {
            min_ratio=freq_ratio;
        }

        len+=sprintf(logBuf+len,"Freq_Ratio=%.9llf Total=%.9llf Avg=%.9llf\n",freq_ratio,total_ratio,avg_ratio);
        len+=sprintf(logBuf+len,"Max=%.9llf Min=%.9llf\n",max_ratio,min_ratio);
    }
    pontod_freq_cal_logging(logBuf,len);
}

void pontod_freq_cal_intr_ratio(PonToDNLData_t* item)
{
    uint32 SE_tick;
    uint64 SE_time;
    uint64 SEF_time;
    uint64 SES_time;
    rtk_enable_t SE_overflow=DISABLED;
    rtk_enable_t SEF_overflow=DISABLED;
    int len=0;
    char logBuf[1000];

    if((item->val.timeIntr.E_time <= item->val.timeIntr.S_time)||
        (item->val.timeIntr.E_tick == item->val.timeIntr.S_tick)||
        (item->val.timeIntr.EF_time == item->val.timeIntr.SF_time)||
        (item->val.timeIntr.ES_time <= item->val.timeIntr.SS_time))
    {
        len+=sprintf(logBuf+len,"Intr#################\n");
        len+=sprintf(logBuf+len,"[%s:%d] tick or time is error\n",__FUNCTION__,__LINE__);
        len+=sprintf(logBuf+len,"S tick=%u time=%llu\n",item->val.timeIntr.S_tick,item->val.timeIntr.S_time);
        len+=sprintf(logBuf+len,"E tick=%u time=%llu\n",item->val.timeIntr.E_tick,item->val.timeIntr.E_time);
        len+=sprintf(logBuf+len,"S Freerun time=%llu\n",item->val.timeIntr.SF_time);
        len+=sprintf(logBuf+len,"E Freerun time=%llu\n",item->val.timeIntr.EF_time);
        len+=sprintf(logBuf+len,"S System time=%llu\n",item->val.timeIntr.SS_time);
        len+=sprintf(logBuf+len,"E System time=%llu\n",item->val.timeIntr.ES_time);
        len+=sprintf(logBuf+len,"#####################\n");
        pontod_freq_cal_logging(logBuf,len);
        return;
    }

    //calculate interval of ToD Start and End
    if(item->val.timeIntr.E_tick < item->val.timeIntr.S_tick) //overflow
    {
        if(overflow_flag==ENABLED)
        {
            SE_tick = tickMaxValue - item->val.timeIntr.S_tick+item->val.timeIntr.E_tick + 1;
            SE_overflow = ENABLED;
        }
        else
        {
            len+=sprintf(logBuf+len,"Intr#################\n");
            len+=sprintf(logBuf+len,"[%s:%d] S tick to E tick is overflow\n",__FUNCTION__,__LINE__);
            len+=sprintf(logBuf+len,"[%s:%d] S tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeIntr.S_tick,item->val.timeIntr.S_time);
            len+=sprintf(logBuf+len,"[%s:%d] E tick=%u time=%llu\n",__FUNCTION__,__LINE__,item->val.timeIntr.E_tick,item->val.timeIntr.E_time);
            len+=sprintf(logBuf+len,"#####################\n");
            pontod_freq_cal_logging(logBuf,len);
            return;
        }
    }
    else
    {
        SE_tick = item->val.timeIntr.E_tick - item->val.timeIntr.S_tick;
    }

    SE_time = item->val.timeIntr.E_time - item->val.timeIntr.S_time;

    //calculate interval of freerun time
    if(item->val.timeIntr.EF_time < item->val.timeIntr.SF_time) //overflow
    {
        if(overflow_flag==ENABLED)
        {
            SEF_time = freeTimeMaxValue - item->val.timeIntr.SF_time + item->val.timeIntr.EF_time + 1;
            SEF_overflow = ENABLED;
        }
        else
        {
            len+=sprintf(logBuf+len,"Intr#################\n");
            len+=sprintf(logBuf+len,"[%s:%d] Freerun time is overflow\n",__FUNCTION__,__LINE__);
            len+=sprintf(logBuf+len,"[%s:%d] Start Freerun time=%llu\n",__FUNCTION__,__LINE__,item->val.timeIntr.SF_time);
            len+=sprintf(logBuf+len,"[%s:%d] End Freerun time=%llu\n",__FUNCTION__,__LINE__,item->val.timeIntr.EF_time);
            len+=sprintf(logBuf+len,"#####################\n");
            pontod_freq_cal_logging(logBuf,len);
            return;
        }
    }
    else
    {
        SEF_time=item->val.timeIntr.EF_time-item->val.timeIntr.SF_time;
    }

    //calculate interval of system time
    SES_time=item->val.timeIntr.ES_time-item->val.timeIntr.SS_time;

    storeCnt++;

    len+=sprintf(logBuf+len,"Intr######%u##########\n",storeCnt);
    len+=sprintf(logBuf+len,"S tick=%u time=%llu\n",item->val.timeIntr.S_tick,item->val.timeIntr.S_time);
    len+=sprintf(logBuf+len,"E tick=%u time=%llu\n",item->val.timeIntr.E_tick,item->val.timeIntr.E_time);
    len+=sprintf(logBuf+len,"%sSE tick=%u time=%llu\n",(SE_overflow==ENABLED)?"overflow ":"",SE_tick,SE_time);
    len+=sprintf(logBuf+len,"S Freerun time=%llu\n",item->val.timeIntr.SF_time);
    len+=sprintf(logBuf+len,"E Freerun time=%llu\n",item->val.timeIntr.EF_time);
    len+=sprintf(logBuf+len,"%sSEF time=%llu\n",(SEF_overflow==ENABLED)?"overflow ":"",SEF_time);
    len+=sprintf(logBuf+len,"S System time=%llu\n",item->val.timeIntr.SS_time);
    len+=sprintf(logBuf+len,"E System time=%llu\n",item->val.timeIntr.ES_time);
    len+=sprintf(logBuf+len,"SES time=%llu\n",SES_time);

    pontod_freq_cal_logging(logBuf,len);
}

void pontod_freq_cal_auto_ratio(PonToDNLData_t* item)
{
    uint64 SEF_time;
    uint64 SES_time;
    rtk_enable_t SEF_overflow=DISABLED;
    int len=0;
    char logBuf[1000];

    if((item->val.timeAuto.EF_time == item->val.timeAuto.SF_time)||
        (item->val.timeAuto.ES_time <= item->val.timeAuto.SS_time))
    {
        len+=sprintf(logBuf+len,"Auto#################\n");
        len+=sprintf(logBuf+len,"[%s:%d] tick or time is error\n",__FUNCTION__,__LINE__);
        len+=sprintf(logBuf+len,"S Freerun time=%llu\n",item->val.timeAuto.SF_time);
        len+=sprintf(logBuf+len,"E Freerun time=%llu\n",item->val.timeAuto.EF_time);
        len+=sprintf(logBuf+len,"S System time=%llu\n",item->val.timeAuto.SS_time);
        len+=sprintf(logBuf+len,"E System time=%llu\n",item->val.timeAuto.ES_time);
        len+=sprintf(logBuf+len,"#####################\n");
        pontod_freq_cal_logging(logBuf,len);
        return;
    }

    //calculate interval of freerun time
    if(item->val.timeAuto.EF_time < item->val.timeAuto.SF_time) //overflow
    {
        if(overflow_flag==ENABLED)
        {
            SEF_time = freeTimeMaxValue - item->val.timeAuto.SF_time + item->val.timeAuto.EF_time;
            SEF_overflow = ENABLED;
        }
        else
        {
            len+=sprintf(logBuf+len,"Auto#################\n");
            len+=sprintf(logBuf+len,"[%s:%d] Freerun time is overflow\n",__FUNCTION__,__LINE__);
            len+=sprintf(logBuf+len,"[%s:%d] Start Freerun time=%llu\n",__FUNCTION__,__LINE__,item->val.timeAuto.SF_time);
            len+=sprintf(logBuf+len,"[%s:%d] End Freerun time=%llu\n",__FUNCTION__,__LINE__,item->val.timeAuto.EF_time);
            len+=sprintf(logBuf+len,"#####################\n");
            pontod_freq_cal_logging(logBuf,len);
            return;
        }
    }
    else
    {
        SEF_time=item->val.timeIntr.EF_time-item->val.timeIntr.SF_time;
    }

    //calculate interval of system time
    SES_time=item->val.timeIntr.ES_time-item->val.timeIntr.SS_time;

    storeCnt++;

    len+=sprintf(logBuf+len,"Auto######%u##########\n",storeCnt);
    len+=sprintf(logBuf+len,"S Freerun time=%llu\n",item->val.timeAuto.SF_time);
    len+=sprintf(logBuf+len,"E Freerun time=%llu\n",item->val.timeAuto.EF_time);
    len+=sprintf(logBuf+len,"%sSEF time=%llu\n",(SEF_overflow==ENABLED)?"overflow ":"",SEF_time);
    len+=sprintf(logBuf+len,"S System time=%llu\n",item->val.timeAuto.SS_time);
    len+=sprintf(logBuf+len,"E System time=%llu\n",item->val.timeAuto.ES_time);
    len+=sprintf(logBuf+len,"SES time=%llu\n",SES_time);

    pontod_freq_cal_logging(logBuf,len);
}

void pontod_freq_set_cal_ratio(void)
{
    uint32 oriFreq,adjFreq;
    int32 ret = RT_ERR_OK;
    int len=0;
    char logBuf[1000];

    if(adjustFreq_flag==ENABLED)
    {
        if(avg_ratio!=1)
        {
            len+=sprintf(logBuf+len,"##########Write Time Frequency##########\n");

            RTK_API_ERR_CHK(rtk_time_frequency_get(&oriFreq),ret);
            len+=sprintf(logBuf+len,"oriFreq=%d\n",oriFreq);

            adjFreq=(uint32)((double)oriFreq*avg_ratio);
            if(adjFreq==0)
            {
                len+=sprintf(logBuf+len,"adjFreq is zero\n");
            }
            else if(adjFreq==oriFreq)
            {
                len+=sprintf(logBuf+len,"adjFreq=%d is the same oriFreq\n",adjFreq);
            }
            else
            {
                RTK_API_ERR_CHK(rtk_time_frequency_set(adjFreq),ret);

                len+=sprintf(logBuf+len,"adjFreq=%d\n",adjFreq);
            }

            pontod_freq_cal_logging(logBuf,len);

            avg_ratio=1;
        }
    }
}

void pontod_freq_set_cal_reinit(void)
{
    //Set back the ratio
    max_ratio=1;
    min_ratio=1;
    avg_ratio=1;

    //Set back the storeCnt
    storeCnt=0;
}

#endif
