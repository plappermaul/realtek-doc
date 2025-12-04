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
#ifdef CONFIG_RTK_TIME_SYNC

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

#include <sys/socket.h> 
#include <linux/netlink.h> 
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <arpa/inet.h>
#include <hal/common/halctrl.h>
#include <ioal/mem32.h>

#include <rtk/time.h>
#include <rtk/ponmac.h>

#include "time_sync.h"
#include "time_sync_monitor.h"
#include "time_sync_intr.h"

/*
 * Symbol Definition
 */
int monMsgQId=-1;

extern rtk_ponmac_mode_t pon_mode;
extern rtk_enable_t link_status;
extern rtk_enable_t tod_update;

extern int debug;

/*
 * Macro Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

static int time_sync_monitor_msg_send(void)
{
    timeSyncMsgBuf_t pmsg;

    if(monMsgQId==-1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Error queue is not ready!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    memset(&pmsg,0,sizeof(timeSyncMsgBuf_t));

    pmsg.mtype=TIME_SYNC_MQ_MONITOR;
    
    if(msgsnd(monMsgQId,&pmsg,sizeof(time_sync_cmd_t),0)<0)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

static void monitor_timer_handler(int sig, siginfo_t *si, void *uc)
{
    if(time_sync_monitor_msg_send()!=RT_ERR_OK)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
    }
}

void monitor_timer_init(int timeSec)
{
    struct sigaction sa;
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = monitor_timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(TIME_SYNC_MONITOR_TIMER_SIG, &sa, NULL) == -1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't create timer signal!\n", __FUNCTION__, __LINE__);
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIME_SYNC_MONITOR_TIMER_SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't create timer!\n", __FUNCTION__, __LINE__);
    }

    its.it_value.tv_sec = timeSec;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't set timer!\n", __FUNCTION__, __LINE__);
    }
}

static void time_sync_monitorFunction(void)
{
    int ret = RT_ERR_OK;;
    rtk_time_timeStamp_t timeStamp;

    if ((ret = rtk_time_curTime_latch()) != RT_ERR_OK)
    {
        DBG_PRINT(2,"[Time Sync:%s:%d] Can't latch time\n",__FUNCTION__,__LINE__);
    }

    if ((ret = rtk_time_curTime_get(&timeStamp)) != RT_ERR_OK)
    {
        DBG_PRINT(2,"[Time Sync:%s:%d] Can't get current time\n",__FUNCTION__,__LINE__);
    }

    DBG_PRINT(2,"[Time Sync:%s:%d] timeStamp.sec=%lld link_status=%d tod_update=%d\n",__FUNCTION__,__LINE__,timeStamp.sec,link_status,tod_update);
        
#ifdef CONFIG_RTK_PTP_MASTER
    if((link_status==ENABLED)&&(tod_update==ENABLED))
    {
        enable1588MasterProcess();
    }
    else
    {
        disable1588MasterProcess();
    }
#endif

#ifdef CONFIG_RTK_PPSTOD
    if((timeStamp.sec>=GPS_UTC_DIFF_SEC)&&(link_status==ENABLED)&&(tod_update==ENABLED))
    {
        enable1PPSToDProcess();
    }
    else
    {
        disable1PPSToDProcess();
    }
#endif
}

void *time_sync_monitorHandle(void *argu)
{
    int msgLen,ret;
    timeSyncMsgBuf_t pmsg;

    while(1)
    {
        msgLen = msgrcv(monMsgQId, &pmsg, sizeof(time_sync_cmd_t), TIME_SYNC_MQ_PRI_MAX, 0 );
        if(msgLen <= 0)
        {
            continue;
        }
        
        switch(pmsg.mtype)
        {
            case TIME_SYNC_MQ_MONITOR:
                time_sync_monitorFunction();
                break;
            case TIME_SYNC_MQ_CMD:
                DBG_PRINT(2,"[Time Sync:%s:%d] TIME_SYNC_MQ_CMD!\n", __FUNCTION__, __LINE__);
                break;
            default:
                DBG_PRINT(1,"[Time Sync:%s:%d] Wrong message type!\n", __FUNCTION__, __LINE__);
                continue;
                break;
        }
    }

}

#endif
