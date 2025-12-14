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
#include "module/intr_bcaster/intr_bcaster.h"

#include "pontod_freq_main.h"
#include "pontod_freq_calc.h"
#include "pontod_freq_intr.h"

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
/*
 * Symbol Definition
 */

extern int gIntrSocketFd;

/*
 * Macro Definition
 */

/*
 * Data Declaration
 */

static int sk=-1;

static uint32 pontod_freq_nl_type=PONTOD_FREQ_NETLINK_TYPE;
rtk_enable_t overflow_flag=DISABLED;
uint32 tickMaxValue=UINT32_MAX;
uint64 freeTimeMaxValue=87960930222040;//2199023255551*40
static rtk_enable_t deamon_flag=ENABLED;
rtk_enable_t logging_flag=ENABLED;
char filePath[200]="/var/config/pontodFreq.data";
rtk_enable_t adjustFreq_flag=ENABLED;
rtk_enable_t qAdjustFlag=DISABLED;
uint32 sample_num=0;
uint32 avg_num=8;

int debug=0;
/*
 * Function Declaration
 */

int nl_send(char* data,uint16 dataLen)
{
    int ret;
    int netlinkLen;
    //int pid = getpid();
    int pid = (int)syscall(SYS_gettid);
    struct nlmsghdr *nlh = NULL;
    char* pData=NULL;
    struct sockaddr_nl dest_addr;
    struct msghdr msg;
    struct iovec iov;

    netlinkLen = NLMSG_SPACE(dataLen);
    nlh = (struct nlmsghdr *)malloc(netlinkLen);
    if(NULL == nlh)
    {
        return -1;
    }

    /* Clear all structures */
    memset(nlh, 0x0, sizeof(netlinkLen));
    memset(&dest_addr, 0x0, sizeof(struct sockaddr_nl));
    memset(&iov, 0x0, sizeof(struct iovec));
    memset(&msg, 0x0, sizeof(struct msghdr));

    /* Fill the netlink message header */ 
    nlh->nlmsg_len = netlinkLen;
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 0;

    /* Fill in the netlink message payload */ 
    pData = (char *) NLMSG_DATA(nlh);
    memcpy(pData, data, dataLen);

    memset(&msg,0,sizeof(struct msghdr));
    memset(&iov,0,sizeof(struct iovec));
    memset((unsigned char *)&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;    /* For Linux Kernel */ 
    dest_addr.nl_groups = 0; /* unicast */ 

    iov.iov_base = (void *) nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *) &dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    ret = sendmsg(sk, &msg, 0);
    if(ret < 0)
    {
        DBG_PRINT(1,"[%s:%d] Error sendmsg %d %d %s\n", __FUNCTION__, __LINE__, ret,errno, strerror( errno ));
        return -2;
    }

    free(nlh);

    return ret;
}

int nl_recv(uint16 mtu,char *data,uint16 *dataLen)
{
    int ret;
    int netlinkLen;
    //int pid = getpid();
    int pid = (int)syscall(SYS_gettid);
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;
    unsigned char *recvData;

    netlinkLen = NLMSG_SPACE(mtu);
    nlh = (struct nlmsghdr *)malloc(netlinkLen);
    if(NULL == nlh)
    {
        return -1;
    }

    /* Clear all structures */
    memset(nlh, 0x0, sizeof(netlinkLen));
    memset(&dest_addr, 0x0, sizeof(struct sockaddr_nl));
    memset(&iov, 0x0, sizeof(struct iovec));
    memset(&msg, 0x0, sizeof(struct msghdr));

    /* Fill the netlink message header */ 
    nlh->nlmsg_len = netlinkLen;
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 0;

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    ret = recvmsg(sk, &msg, 0);
    if(ret > 0)
    {
        recvData = NLMSG_DATA(msg.msg_iov->iov_base);
        memcpy(data, recvData, nlh->nlmsg_len - NLMSG_HDRLEN);
        *dataLen = (nlh->nlmsg_len - NLMSG_HDRLEN);
    }
    free(nlh);

    return ret;
}

static int32 pontod_configfile_set(int argc,char *argv[])
{
    int cmd = 1,help=0, dump =0;
    int32 ret = RT_ERR_OK;

    while (cmd < argc)
    {
        if (!strcmp("-nt", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            if(atoi(argv[cmd])>=32 || atoi(argv[cmd])<0)
            {
                help=1;
                break;
            }
            pontod_freq_nl_type=atoi(argv[cmd]);
            cmd++;
        }
        if (!strcmp("-n", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            overflow_flag=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-m", argv[cmd]))
        {
            char *stopstring;
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            tickMaxValue=strtoul(argv[cmd],&stopstring,10);
            cmd++;
        }
        else if (!strcmp("-da", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            deamon_flag=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-l", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            logging_flag=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-a", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            adjustFreq_flag=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-q", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            qAdjustFlag=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-p", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            sprintf(filePath,"%s",argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-s", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc || atoi(argv[cmd])<0 || atoi(argv[cmd])==1 || atoi(argv[cmd])==2)
            {
                help=1;
                break;
            }
            sample_num=atoi(argv[cmd]);
            avg_num=sample_num-2; // minus maximum ratio and minimum ratio
            cmd++;
        }
        else if (!strcmp("-du", argv[cmd]))
        {
            dump =1;
            cmd++;
        }
        else if (!strcmp("-d", argv[cmd]))
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
        printf("%-5s  %-35s\n","-nt","Config Netlink Type range 0~31");
        printf("%-5s  %-35s\n","-n","Set calculateing overflow");
        printf("%-5s  %-35s\n","-m","Set max superframe or localtime value 0~4294967295");
        printf("%-5s  %-35s\n","-da","Daemon 0:disable 1:enable");
        printf("%-5s  %-35s\n","-l","Enable Logging 0:disable 1:enable");
        printf("%-5s  %-35s\n","-a","Adjust Frequency 0:disable 1:enable");
        printf("%-5s  %-35s\n","-q","Adjust Quickly or Deregister 0:deregister 1:quickly");
        printf("%-5s  %-35s\n","-p","Logging File Path");
        printf("%-5s  %-35s\n","-s","Sample ToD num 0,3~UINT32_MAX");
        printf("%-5s  %-35s\n","-du","Dump PON ToD Freq Configuration");
        printf("%-5s  %-35s\n","-d","Debug printf 0,1,2");
        printf("%-5s  %-35s\n","-h","Help");
        return RT_ERR_INPUT;
    }

    if(dump==1)
    {
        printf("Netlink Type=%d\n",pontod_freq_nl_type);
        printf("Calculating overflow=%d\n",overflow_flag);
        printf("Maximum superframe or localtime value=%lu\n",tickMaxValue);
        printf("Deamon=%d\n",deamon_flag);
        printf("Logging=%d\n",logging_flag);
        printf("Logging File Path=%s\n",filePath);
        printf("Sample Num=%d\n",sample_num);
        printf("Adjust Frequency=%d\n",adjustFreq_flag);
        printf("Adjust Quickly=%d\n",qAdjustFlag);
        printf("Debug Level : %d\n",debug);
    }
    
    return ret;
}

void daemon_init(void)
{
    int     i;
    pid_t   pid;
 
    if ( (pid = fork()) != 0)
        exit(0);            /* parent terminates */
 
    /* 1st child continues */
    setsid();               /* become session leader */
 
    signal(SIGHUP, SIG_IGN);
    if ( (pid = fork()) != 0)
        exit(0);            /* 1st child terminates */
 
    /* 2nd child continues */
    if(chdir("/")!=0)       /* change working directory */
        exit(0);

    umask(0);               /* clear our file mode creation mask */
}

void pontod_freq_exit(int signo) 
{
    PonToDNLData_t data;
    int32 ret = RT_ERR_OK;

    data.type=TYPE_PONTOD_PID_DREG;

    nl_send((char *)&data,sizeof(PonToDNLData_t));

    exit(1);
}

int
main(
    int argc,
    char *argv[])
{
    char tmp[100];
    char tmpCmd[200];
    struct sockaddr_nl nladdr;
    int pid = (int)syscall(SYS_gettid);
    PonToDNLData_t data;
    uint16 dataLen=0;
    char cmd[64];
    FILE *fp;
    struct sockaddr_nl sa;
    pthread_t intrThread;

    if(pontod_configfile_set(argc,argv)!=RT_ERR_OK)
        return 0;

    signal(SIGHUP,pontod_freq_exit);
    signal(SIGINT,pontod_freq_exit);
    signal(SIGQUIT,pontod_freq_exit);
    signal(SIGTERM,pontod_freq_exit);

    if(deamon_flag==ENABLED)
    {
        daemon_init();
    }


    //Initialize pontod interrupt handle
    if((gIntrSocketFd=socket(PF_NETLINK, SOCK_RAW, INTR_BCASTER_NETLINK_TYPE))==-1)
    {
        printf("[%s:%d] Netlink socket can't create!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = (1 << MSG_TYPE_ONU_STATE) | (1 << MSG_TYPE_EPON_INTR_LOS);
    if (-1 == bind(gIntrSocketFd, (struct sockaddr*)&sa, sizeof(struct sockaddr)))
    {
        printf("[%s:%d] Netlink socket can't bind!\n", __FUNCTION__, __LINE__);
        close(gIntrSocketFd);
        return 0;
    }

    pthread_create(&intrThread, NULL, &pontod_freq_intrHandle, NULL);

    //check module shold be disable
    fp = popen("cat /proc/pontod_freq_drv/enable","r");
    if(fp == NULL)
    {
        printf("[%s:%d] unable to read on /proc/pontod_freq_drv/enabled\n", __FUNCTION__, __LINE__);
        return 0;
    }
    fgets(tmp,sizeof(tmp),fp);

    //pontod_freq enable status: %d
    if(strlen(tmp)>=28)
    {
        //1 means module enable
        if(atoi(tmp+27)==1)
        {
            //if module have been enabled, close process
            printf("[%s:%d] Module have been enabled\n", __FUNCTION__, __LINE__);
            return 0;
        }
    }
    pclose(fp);

    sprintf(tmpCmd,"echo %d > /proc/pontod_freq_drv/nl_type",pontod_freq_nl_type);
    system(tmpCmd);
    system("echo 1 > /proc/pontod_freq_drv/enable");

    //create netlink socket for communicating with pontod driver
    if((sk = socket(PF_NETLINK, SOCK_RAW,pontod_freq_nl_type))==-1)
    {
        printf("[%s:%d] Netlink socket can't create!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    //bind thread id to sock_fd
    nladdr.nl_family = PF_NETLINK;
    nladdr.nl_pad = 0;
    nladdr.nl_pid = pid;
    nladdr.nl_groups =0;
    if((bind(sk, (struct sockaddr*)&nladdr, sizeof(nladdr)))==-1)
    {
        printf("[%s:%d] Netlink socket can't bind!\n", __FUNCTION__, __LINE__);
        close(sk);
        return 0;
    }

    data.type=TYPE_PONTOD_PID_REG;
    data.val.pid=pid;

    nl_send((char *)&data,sizeof(PonToDNLData_t));

    if(nl_recv(sizeof(PonToDNLData_t),(char *)&data,&dataLen) <=0)
    {
        printf("[%s:%d] receive reg ack fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    DBG_PRINT(2,"[%s:%d] type=%d pid=%d dataLen=%d\n", __FUNCTION__, __LINE__, data.type,data.val.pid,dataLen);

    while(1)
    {
        memset(&data,0,sizeof(PonToDNLData_t));
        dataLen=0;
        if(nl_recv(sizeof(PonToDNLData_t),(char *)&data,&dataLen) <=0)
        {
            DBG_PRINT(1,"[%s:%d] receive fail\n", __FUNCTION__, __LINE__);
            continue;
        }

        DBG_PRINT(2,"[%s:%d] type=%d dataLen=%d\n", __FUNCTION__, __LINE__, data.type,dataLen);

        if(data.type==TYPE_PONTOD_TIME_TICK)
        {
            pontod_freq_cal_ratio(&data);
        }
        else if(data.type==TYPE_PONTOD_TIME_INTR)
        {
            pontod_freq_cal_intr_ratio(&data);
        }
        else if(data.type==TYPE_PONTOD_TIME_AUTO)
        {
            pontod_freq_cal_auto_ratio(&data);
        }
        else if(data.type==TYPE_PONTOD_RECOLLECT)
        {
            pontod_freq_set_cal_reinit();
        }
        else if(data.type==TYPE_PONTOD_SET_FREQ)
        {
            pontod_freq_set_cal_ratio();
        }
        else
        {
            DBG_PRINT(1,"[%s:%d] receive unknown type %d\n", __FUNCTION__, __LINE__, data.type);
        }
    }

    close(sk);

    return 0;
}

#endif
