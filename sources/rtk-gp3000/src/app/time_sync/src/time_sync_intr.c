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

#include <rtk/ponmac.h>
#include <rtk/gpon.h>

#include "module/intr_bcaster/intr_bcaster.h"

#include "time_sync.h"
#include "time_sync_monitor.h"
#include "time_sync_intr.h"

/*
 * Symbol Definition
 */
rtk_ponmac_mode_t pon_mode=PONMAC_MODE_GPON;
rtk_enable_t link_status=DISABLED;
rtk_enable_t tod_update=DISABLED;

extern int debug;

int gIntrSocketFd=-1;

/*
 * Macro Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

static void tod_update_timer_handler(int sig, siginfo_t *si, void *uc)
{
    DBG_PRINT(2,"[Time Sync:%s:%d] tod_update_timer_handler\n", __FUNCTION__, __LINE__);
    if(link_status==ENABLED)
    {
        tod_update=ENABLED;
    }
    else
    {
        tod_update=DISABLED;
    }
}

static void tod_update_timer_init(int timeSec)
{
    struct sigaction sa;
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = tod_update_timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(TIME_SYNC_TOD_UPDATE_TIMER_SIG, &sa, NULL) == -1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't create timer signal!\n", __FUNCTION__, __LINE__);
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIME_SYNC_TOD_UPDATE_TIMER_SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't create timer!\n", __FUNCTION__, __LINE__);
    }

    its.it_value.tv_sec = timeSec;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't set timer!\n", __FUNCTION__, __LINE__);
    }
}

static int time_sync_intr_socket_recv(int fd,intrBcasterMsg_t *pMsgData)
{
    int                 recvLen;
    int                 nlmsgLen;
    struct nlmsghdr     *pNlh = NULL;
    struct iovec        iov;
    struct sockaddr_nl  da;
    struct msghdr       msg;

    if (!pMsgData)
        return RT_ERR_INPUT;

    // allocate memory
    nlmsgLen = NLMSG_SPACE(MAX_BYTE_PER_INTR_BCASTER_MSG);
    pNlh = (struct nlmsghdr *)malloc(nlmsgLen);
    if (!pNlh)
        return RT_ERR_INPUT;

    // fill struct nlmsghdr
    memset(pNlh, 0, nlmsgLen);

    // fill struct iovec
    memset(&iov, 0, sizeof(struct iovec));
    iov.iov_base = (void *)pNlh;
    iov.iov_len = nlmsgLen;

    // fill struct sockaddr_nl
    memset(&da, 0, sizeof(struct sockaddr_nl));
    da.nl_family = AF_NETLINK;

    // fill struct msghdr
    memset(&msg, 0x0, sizeof(struct msghdr));
    msg.msg_name = (void *)&da;
    msg.msg_namelen = sizeof(da);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // receive message
    recvLen = recvmsg(fd, &msg, 0);
    if (!NLMSG_OK(pNlh, recvLen))
        return RT_ERR_INPUT;

    memcpy(pMsgData, NLMSG_DATA(pNlh), MAX_BYTE_PER_INTR_BCASTER_MSG);
    free(pNlh);

    return RT_ERR_OK;
}

void *time_sync_intrHandle(void *argu)
{
    intrBcasterMsg_t msgData;

    while (1)
    {
        if (RT_ERR_OK == time_sync_intr_socket_recv(gIntrSocketFd, &msgData))
        {
            switch(msgData.intrType)
            {
                case MSG_TYPE_ONU_STATE:
                    DBG_PRINT(1,"[Time Sync:%s:%d] MSG_TYPE_ONU_STATE msgData.intrSubType=%d\n", __FUNCTION__, __LINE__,msgData.intrSubType);
                    if(msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O1
                        ||msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O6)
                    {
                        link_status=DISABLED;
                        tod_update=DISABLED;
                    }
                    else if(msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O2
                        ||msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O3
                        ||msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O4
                        ||msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O5
                        ||msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O7)
                    {
                        link_status=ENABLED;
                    }
                    else
                    {
                        DBG_PRINT(1,"[Time Sync:%s:%d] MSG_TYPE_ONU_STATE wrong onu state\n", __FUNCTION__, __LINE__);
                    }
                    break;
                case MSG_TYPE_EPON_INTR_LOS:
                    DBG_PRINT(1,"[Time Sync:%s:%d] MSG_TYPE_EPON_INTR_LOS msgData.intrStatus=%d\n", __FUNCTION__, __LINE__,msgData.intrStatus);
                    if(msgData.intrStatus==ENABLED)
                    {
                        link_status=DISABLED;
                        tod_update=DISABLED;
                    }
                    else if(msgData.intrStatus==DISABLED)
                    {
                        link_status=ENABLED;
                    }
                    else
                    {
                        DBG_PRINT(1,"[Time Sync:%s:%d] MSG_TYPE_EPON_INTR_LOS wrong status\n", __FUNCTION__, __LINE__);
                    }
                    break;
                case MSG_TYPE_TOD_UPDATE_ENABLE:
                    DBG_PRINT(1,"[Time Sync:%s:%d] MSG_TYPE_TOD_UPDATE_ENABLE\n", __FUNCTION__, __LINE__);
                    tod_update_timer_init(TIME_SYNC_TOD_UPDATE_TIME);
                    break;
                default:
                    DBG_PRINT(1,"[Time Sync:%s:%d] Unknown interrupt\n", __FUNCTION__, __LINE__);
                    break;
            }
        }
    }

}

#endif
