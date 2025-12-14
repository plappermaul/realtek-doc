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
 * $Revision: 40647 $
 * $Date:  $
 *
 * Purpose : Main function of the PTP Master protocol stack user application
 *           It create two additional threads for packet Rx and state control
 *
 * Feature : Start point of the PTP Master protocol stack. Use individual threads
 *           for packet Rx and state control
 *
 */

#ifdef CONFIG_RTK_PTP_MASTER

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
#include <sys/socket.h> 
#include <linux/netlink.h> 
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>

#include "ptp_master_rx.h"
#include "ptp_master_main.h"
#include "ptp_master_timer.h"

/*
 * Symbol Definition
 */
#define ANNOUNCE_TIMER_SIG SIGRTMIN+5
#define SYNC_TIMER_SIG SIGRTMIN+6

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

static void
announce_timer_handler(int sig, siginfo_t *si, void *uc)
{
    if(ptp_announce_timeout_msg_send()!=RT_ERR_OK)
    {
        printf("[PTP Timer:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
    }
}

void announce_timer_init(int timeSec)
{
    struct sigaction sa;
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = announce_timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(ANNOUNCE_TIMER_SIG, &sa, NULL) == -1)
    {
        printf("[PTP Timer:%s:%d] Can't create timer signal!\n", __FUNCTION__, __LINE__);
    }


    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = ANNOUNCE_TIMER_SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        printf("[PTP Timer:%s:%d] Can't create timer!\n", __FUNCTION__, __LINE__);
    }

    its.it_value.tv_sec = timeSec;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        printf("[PTP Timer:%s:%d] Can't set timer!\n", __FUNCTION__, __LINE__);
    }
}

static void
sync_timer_handler(int sig, siginfo_t *si, void *uc)
{
    if(ptp_sync_timeout_msg_send()!=RT_ERR_OK)
    {
        printf("[PTP Timer:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
    }
}

void sync_timer_init(int timeSec,int timeMSec)
{
    struct sigaction sa;
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sync_timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SYNC_TIMER_SIG, &sa, NULL) == -1)
    {
        printf("[PTP Timer:%s:%d] Can't create timer signal!\n", __FUNCTION__, __LINE__);
    }


    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SYNC_TIMER_SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        printf("[PTP Timer:%s:%d] Can't create timer!\n", __FUNCTION__, __LINE__);
    }


    its.it_value.tv_sec = timeSec;
    its.it_value.tv_nsec = (long)timeMSec*1e6;

    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        printf("[PTP Timer:%s:%d] Can't set timer!\n", __FUNCTION__, __LINE__);
    }
}

#endif
