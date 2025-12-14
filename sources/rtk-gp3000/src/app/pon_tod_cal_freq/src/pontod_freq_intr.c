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
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include "module/pontod_freq/pontod_freq_drv_main.h"
#include "pontod_freq_main.h"
#include "pontod_freq_calc.h"
#include "pontod_freq_intr.h"

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>

#include "module/intr_bcaster/intr_bcaster.h"
#include <rtk/ponmac.h>
#include <rtk/gpon.h>

/*
 * Symbol Definition
 */

int gIntrSocketFd=-1;

/*
 * Macro Definition
 */

/*
 * Data Declaration
 */

extern int debug;
extern rtk_enable_t qAdjustFlag;
/*
 * Function Declaration
 */

static int pontod_freq_intr_socket_recv(int fd,intrBcasterMsg_t *pMsgData)
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

void *pontod_freq_intrHandle(void *argu)
{
    intrBcasterMsg_t msgData;
    
    while (1)
    {
        if (RT_ERR_OK == pontod_freq_intr_socket_recv(gIntrSocketFd, &msgData))
        {
            switch(msgData.intrType)
            {
                case MSG_TYPE_ONU_STATE:
                    DBG_PRINT(2,"[%s:%d] MSG_TYPE_ONU_STATE msgData.intrSubType=%d\n", __FUNCTION__, __LINE__,msgData.intrSubType);
                    if(msgData.intrSubType==RTK_GPONMAC_FSM_STATE_O1)
                    {
                        if(qAdjustFlag==DISABLED)
                            pontod_freq_set_cal_ratio();
                        //Set the moudle to restart collect
                        system("echo 1 > /proc/pontod_freq_drv/recollect");
                    }
                    break;
                case MSG_TYPE_EPON_INTR_LOS:
                    DBG_PRINT(2,"[%s:%d] MSG_TYPE_EPON_INTR_LOS msgData.intrStatus=%d\n", __FUNCTION__, __LINE__,msgData.intrStatus);
                    if(msgData.intrStatus==ENABLED)
                    {
                        if(qAdjustFlag==DISABLED)
                            pontod_freq_set_cal_ratio();
                        //Set the moudle to restart collect
                        system("echo 1 > /proc/pontod_freq_drv/recollect");
                    }
                    break;
                default:
                    DBG_PRINT(1,"[%s:%d] Unknown interrupt\n", __FUNCTION__, __LINE__);
                    break;
            }
        }
    }

}

#endif
