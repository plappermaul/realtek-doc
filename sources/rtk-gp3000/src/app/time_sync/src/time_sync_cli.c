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

#include "time_sync.h"
#include "time_sync_monitor.h"

/*
 * Symbol Definition
 */
static int msgQId=-1;


/*
 * Macro Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

int time_sync_monitor_msg_send(void)
{
    timeSyncMsgBuf_t pmsg;
    msgQId = msgget(TIME_SYNC_MSG_KEY , 666 | IPC_CREAT);

    if(msgQId==-1)
    {
        printf("[Time Sync CLI:%s:%d] Can't initialize message queue!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    memset(&pmsg,0,sizeof(timeSyncMsgBuf_t));

    pmsg.mtype=TIME_SYNC_MQ_CMD;
    
    if(msgsnd(msgQId,&pmsg,sizeof(time_sync_cmd_t),0)<0)
    {
        printf("[Time Sync CLI:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int
main(
    int argc,
    char *argv[])
{

    time_sync_monitor_msg_send();

    return 0;
}

#endif
