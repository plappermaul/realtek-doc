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

#include "module/intr_bcaster/intr_bcaster.h"

#include "time_sync.h"
#include "time_sync_monitor.h"
#include "time_sync_intr.h"

/*
 * Symbol Definition
 */
#ifdef CONFIG_RTK_PTP_MASTER
static int ptpmaster_enable=DISABLED;
#define PTPMASTER_PATHLEN 100
char ptpmaster_sh[PTPMASTER_PATHLEN]="/etc/ptpmaster.sh";
#endif

#ifdef CONFIG_RTK_PPSTOD
static int ppstod_enable=DISABLED;
#define PPSTOD_PATHLEN 100
char ppstod_sh[PPSTOD_PATHLEN]="/etc/ppstod.sh";
#endif

int debug=0;

extern int monMsgQId;

extern rtk_ponmac_mode_t pon_mode;
extern int gIntrSocketFd;
extern rtk_enable_t link_status;

/*
 * Macro Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

void time_sync_exit(int signo) 
{
    DBG_PRINT(1,"process Kill signo=%d %s %s %d\n",signo,__FILE__,__FUNCTION__,__LINE__);

    rtk_ponmac_synce_set(DISABLED);

#ifdef CONFIG_RTK_PTP_MASTER
    disable1588MasterProcess();
    exit1588MasterProcess();
#endif

#ifdef CONFIG_RTK_PPSTOD
    disable1PPSToDProcess();
    exit1PPSToDProcess();
#endif
    exit(1);
}  

#ifdef CONFIG_RTK_PTP_MASTER
void init1588MasterProcess(void)
{

}

void exit1588MasterProcess(void)
{

}

void enable1588MasterProcess(void)
{
    char tmp[200]={0};
    if(ptpmaster_enable==DISABLED)
    {
        ptpmaster_enable=ENABLED;
        sprintf(tmp,"%s %s",ptpmaster_sh,"start");
        system(tmp);
    }
}

void disable1588MasterProcess(void)
{
    char tmp[200]={0};
    if(ptpmaster_enable==ENABLED)
    {
        ptpmaster_enable=DISABLED;
        sprintf(tmp,"%s %s",ptpmaster_sh,"stop");
        system(tmp);
    }
}
#endif

#ifdef CONFIG_RTK_PPSTOD
void init1PPSToDProcess(void)
{
    //setting output to uart0
    system("echo 1 > /proc/ppstod_drv/output");
}
void exit1PPSToDProcess(void)
{
    //setting output to uart0
    system("echo 0 > /proc/ppstod_drv/output");
}

void enable1PPSToDProcess(void)
{
    char tmp[200]={0};
    if(ppstod_enable==DISABLED)
    {
        ppstod_enable=ENABLED;
        sprintf(tmp,"%s %s",ppstod_sh,"start");
        system(tmp);
    }
}

void disable1PPSToDProcess(void)
{
    char tmp[200]={0};
    if(ppstod_enable==ENABLED)
    {
        ppstod_enable=DISABLED;
        sprintf(tmp,"%s %s",ppstod_sh,"stop");
        system(tmp);
    }
}
#endif

static int32 time_sync_configfile_set(int argc,char *argv[])
{
    int cmd = 1,help=0,dump =0;
    int32 ret = RT_ERR_OK;

    while (cmd < argc)
    {
        if (!strcmp("-d", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc) 
            {
                help=1;
                break;
            }
            debug=atoi(argv[cmd]);
            cmd++;
        }
#ifdef CONFIG_RTK_PTP_MASTER
        else if (!strcmp("-fptpmaster", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            memset(ptpmaster_sh,0,PTPMASTER_PATHLEN);
            memcpy(ptpmaster_sh,argv[cmd],((strlen(argv[cmd])+1)>PTPMASTER_PATHLEN)?PTPMASTER_PATHLEN:strlen(argv[cmd]));
            cmd++;
            printf("ptpmaster_sh=%s\n",ptpmaster_sh);
        }
#endif
#ifdef CONFIG_RTK_PPSTOD
        else if (!strcmp("-fppstod", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            memset(ppstod_sh,0,PPSTOD_PATHLEN);
            memcpy(ppstod_sh,argv[cmd],((strlen(argv[cmd])+1)>PPSTOD_PATHLEN)?PPSTOD_PATHLEN:strlen(argv[cmd]));
            cmd++;
            printf("ppstod_sh=%s\n",ppstod_sh);
        }
#endif
        else if (!strcmp("-h", argv[cmd]))
        {
            help=1;
            cmd++;
            break;
        }
        else
        {
            cmd++;
        }
    }
    
    if(help==1)
    {
        printf("%-15s  %-35s\n","-d","Debug printf 0,1,2");
#ifdef CONFIG_RTK_PTP_MASTER
        printf("%-15s  %-35s\n","-fptpmaster","PTPMaster script path");
#endif
#ifdef CONFIG_RTK_PPSTOD
        printf("%-15s  %-35s\n","-fppstod","1PPSToD script path");
#endif
        return RT_ERR_INPUT;
    }
    return ret;
}


int
main(
    int argc,
    char *argv[])
{
    pthread_t monitorThread;
    pthread_t intrThread;
    int ret;
    struct sockaddr_nl sa;
    unsigned int sd, sync;

    signal(SIGHUP,time_sync_exit);
    signal(SIGINT,time_sync_exit);
    signal(SIGQUIT,time_sync_exit);
    signal(SIGTERM,time_sync_exit);
    
    if(time_sync_configfile_set(argc,argv)!=RT_ERR_OK)
        return 0;

    //Initialize PON SyncE
    rtk_ponmac_synce_set(ENABLED);

    //Initialize PON ToD
    
    //Initialize 1588 Master
#ifdef CONFIG_RTK_PTP_MASTER
    init1588MasterProcess();
#endif
    //Initialize 1PPS+ToD
#ifdef CONFIG_RTK_PPSTOD
    init1PPSToDProcess();
#endif

    //Initialize for monitorThread
    if((ret = rtk_ponmac_mode_get(&pon_mode)) != RT_ERR_OK)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] get pon mode fail!\n", __FUNCTION__, __LINE__);
        return ;
    }

    if(pon_mode == PONMAC_MODE_GPON)
    {
        DBG_PRINT(2,"[Time Sync:%s:%d] gpon mode!\n", __FUNCTION__, __LINE__);
    }
    else if(pon_mode == PONMAC_MODE_EPON)
    {
        DBG_PRINT(2,"[Time Sync:%s:%d] epon mode!\n", __FUNCTION__, __LINE__);
    }
    else
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] wrong pon mode!\n", __FUNCTION__, __LINE__);
        return ;
    }

    monMsgQId = msgget(TIME_SYNC_MSG_KEY , 666 | IPC_CREAT);
    if(monMsgQId==-1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Can't initialize message queue!\n", __FUNCTION__, __LINE__);
        return ;
    }

    monitor_timer_init(TIME_SYNC_MONITOR_TIME);

    //Initialize for intrThread
    if((gIntrSocketFd=socket(PF_NETLINK, SOCK_RAW, INTR_BCASTER_NETLINK_TYPE))==-1)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Netlink socket can't create!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = (1 << MSG_TYPE_ONU_STATE) | (1 << MSG_TYPE_EPON_INTR_LOS) | (1<<MSG_TYPE_TOD_UPDATE_ENABLE);
    if (-1 == bind(gIntrSocketFd, (struct sockaddr*)&sa, sizeof(struct sockaddr)))
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] Netlink socket can't bind!\n", __FUNCTION__, __LINE__);
        close(gIntrSocketFd);
        return 0;
    }

    if((ret = rtk_ponmac_linkState_get(pon_mode, &sd, &sync))!= RT_ERR_OK)
    {
        DBG_PRINT(1,"[Time Sync:%s:%d] get link state faile!\n", __FUNCTION__, __LINE__);
        close(gIntrSocketFd);
        return 0;
    }
    
    if((sd == 1) && (sync == 1))
    {
        link_status=ENABLED;
        DBG_PRINT(2,"[Time Sync:%s:%d] ONU link up!\n", __FUNCTION__, __LINE__);
    }
    else
    {
        link_status=DISABLED;
        DBG_PRINT(2,"[Time Sync:%s:%d] ONU link down!\n", __FUNCTION__, __LINE__);
    }
    
    pthread_create(&monitorThread, NULL, &time_sync_monitorHandle, NULL);
    pthread_create(&intrThread, NULL, &time_sync_intrHandle, NULL);

    while(1)
    {
        sleep(100);
    }

    return 0;
}

#endif
