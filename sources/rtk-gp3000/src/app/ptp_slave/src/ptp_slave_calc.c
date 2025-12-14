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
 * Purpose : Main function of the PTP Slave protocol stack user application
 *           It create four additional threads for packet Rx ,adjust freq ,adjust offset state control
 *
 * Feature : Start point of the PTP Slave protocol stack. Use individual threads
 *           for packet Rx ,adjust freq ,adjust offset and state control
 *
 */
#ifdef CONFIG_RTK_PTP_SLAVE
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
#include <sys/wait.h>

#include <rtk/time.h>

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>

#include "ptp_slave_rx.h"
#include "ptp_slave_main.h"
#include "ptp_slave_calc.h"

#include "module/intr_bcaster/intr_bcaster.h"

/*
 * Symbol Definition
 */
 
/*
 * Macro Definition
 */
#define SAMPLE_MAX_NUM 100
/*
 * Data Declaration
 */
static int msgQId=-1;
static double preRatio=0;
static uint8 clockid[10]={0};

static uint8 offsetIdx=0;
static int64 sampleOffset[SAMPLE_MAX_NUM];
static uint64 round_num=1;
static int64 preAvgOffset=0;
static int64 specialOffset=0;

static int64 preT1=0;
static int64 preT2=0;
static int64 preT3=0;
static int64 preT4=0;

rtk_enable_t adj_freq=ENABLED;
rtk_enable_t adj_offset=ENABLED;

int gIntrSocketFd = -1;


/*
 * Function Declaration
 */
int32 ptp_delay_msg_send(ptpFDMsgBuf_t *ptpFDMsgBuf_ptr)
{
    if(ptpFDMsgBuf_ptr==NULL)
    {
        printf("[PTP Slave Delay:%s:%d] Error delayTime_ptr is NULL!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    if(msgQId==-1)
    {
        printf("[PTP Slave Delay:%s:%d] Error queue is not ready!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    ptpFDMsgBuf_ptr->mtype=MQ_SLAVE_DELAY_TIME;

    if(msgsnd(msgQId,(void *)ptpFDMsgBuf_ptr,sizeof(ptp_delayTime_t),IPC_NOWAIT)<0)
    {
        printf("[PTP Slave Delay:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int64 gOffset = 0;
uint8 startAdjustOffset = 0;

//uint8 testCnt=0;

static void ptp_set_offest(int64 offset)
{
    uint32 sign;
    rtk_time_timeStamp_t timeStamp;
    int i;
    int64 avgOffset=0;
    int32 ret = RT_ERR_OK;

//if(testCnt >= 3)
//{
    if(startAdjustOffset == 0)
    {
        DBG_PRINT(1,"adjust Offset to=%lld\n",offset);
        gOffset = offset;
        startAdjustOffset = 1;
    }

//    testCnt = 0;
//}
//else
//{
//    testCnt++;
//}

//    /*setting offset*/
//    if(offset<0)
//    {
//        sign = PTP_REF_SIGN_POSTIVE;
//        timeStamp.sec=llabs(offset)/1e9;
//        timeStamp.nsec=(uint32)(llabs(offset)-(timeStamp.sec*1e9));
//        
//    }
//    else if(offset>0)
//    {
//        sign = PTP_REF_SIGN_NEGATIVE;
//        timeStamp.sec=offset/1e9;
//        timeStamp.nsec=(uint32)(offset-(timeStamp.sec*1e9));
//    }
//    else
//    {
//        DBG_PRINT(1,"%s timeStamp.sec=0 timeStamp.nsec=0.000000000\n","no need adjust");
//        return ;
//    }
//
//    DBG_PRINT(1,"%s timeStamp.sec=%lld timeStamp.nsec=%09d\n",(sign==PTP_REF_SIGN_POSTIVE)?"increase":"decrease", timeStamp.sec,timeStamp.nsec);
//
//    RTK_API_ERR_CHK(rtk_time_refTime_set(sign, timeStamp),ret);
//
}

static void ptp_set_freq_by_ratio(double ratio)
{
    double adjRatio=0;
    uint32 oriFreq,adjFreq;
    int i;
    int32 ret = RT_ERR_OK;
    uint32 val;
        
    if(ratio>1.100000 || ratio <0.900000)
    {
        preRatio=0;
        DBG_PRINT(1,"freqRatio=%.9llf is not in range 0.9~1.1\n",ratio);
        return ;
    }

    if(preRatio==0)
    {
        adjRatio=ratio;
    }
    else
    {
        adjRatio=preRatio*0.3+ratio*0.7;
    }

    DBG_PRINT(1,"Average freqRatio=%.9llf preFreqRatio=%.9llf adjFreqRatio=%.9llf\n",ratio,preRatio,adjRatio);
    
    preRatio=adjRatio;

    if(adjRatio<0.000001)
    {
        DBG_PRINT(1,"adjFreqRatio is zero\n");
        return ;
    }

    if(adjRatio<1.000000001 && adjRatio>0.999999999)
    {
        DBG_PRINT(1,"adjFreqRatio is too small\n");
        return ;
    }

    RTK_API_ERR_CHK(rtk_time_frequency_get(&oriFreq),ret);

    adjFreq=(uint32)((double)oriFreq*adjRatio);
    
    if(adjFreq==0)
    {
        DBG_PRINT(1,"adjFreq is zero\n");
        return ;
    }
    
    if(adjFreq==oriFreq)
    {
        DBG_PRINT(1,"adjFreq=%d is the same oriFreq\n",adjFreq);
        return ;
    }

    DBG_PRINT(1,"adjFreq=%d oriFreq=%d\n",adjFreq,oriFreq);

    RTK_API_ERR_CHK(rtk_time_frequency_set(adjFreq),ret);

    return ;
}

int64 setT1 = 0;

void *ptp_slave_calcThread(void *argu)
{
    int msgLen,i;
    ptpFDMsgBuf_t pmsg;
    ptp_delayTime_t *delayTime_ptr;
    ptp_ts_t masterTs,slaveTs;
    int64 mT,sT;
    double ratio;
    ptp_ts_t T1s,T2s,T3s,T4s;
    int64 T1,T2,T3,T4,delay,offset;
    
    delayTime_ptr=&pmsg.mtext;

    msgQId = msgget(PTP_SLAVE_CALC_MSG_KEY , 666 | IPC_CREAT);
    if(msgQId==-1)
    {
        printf("[PTP Slave Freq:%s:%d] Can't initialize message queue!\n", __FUNCTION__, __LINE__);
    }
    while(1)
    {
        msgLen = msgrcv(msgQId,(void *)&pmsg, sizeof(ptp_delayTime_t), MQ_SLAVE_F_PRI_MAX, MSG_NOERROR);
        if(msgLen <= 0)
        {
            printf("[PTP Slave Freq:%s:%d] No message from queue!\n", __FUNCTION__, __LINE__);
            continue;
        }
        
        switch(pmsg.mtype)
        {
            case MQ_SLAVE_DELAY_TIME:                
                T1s.sec=0;
                for(i=0;i<6;i++)
                {
                    T1s.sec=T1s.sec*256+delayTime_ptr->t1.sec[i];
                }
                
                T1s.nsec=0;
                for(i=0;i<4;i++)
                {
                    T1s.nsec=T1s.nsec*256+delayTime_ptr->t1.nanosec[i];
                }
                
                T1=T1s.sec;
                T1=T1*1e9;
                T1=T1+T1s.nsec;

                T2s.sec=0;
                for(i=0;i<6;i++)
                {
                    T2s.sec=T2s.sec*256+delayTime_ptr->t2.sec[i];
                }
                
                T2s.nsec=0;
                for(i=0;i<4;i++)
                {
                    T2s.nsec=T2s.nsec*256+delayTime_ptr->t2.nanosec[i];
                }
                
                T2=T2s.sec;
                T2=T2*1e9;
                T2=T2+T2s.nsec;

                T3s.sec=0;
                for(i=0;i<6;i++)
                {
                    T3s.sec=T3s.sec*256+delayTime_ptr->t3.sec[i];
                }
                
                T3s.nsec=0;
                for(i=0;i<4;i++)
                {
                    T3s.nsec=T3s.nsec*256+delayTime_ptr->t3.nanosec[i];
                }
                
                T3=T3s.sec;
                T3=T3*1e9;
                T3=T3+T3s.nsec;

                T4s.sec=0;
                for(i=0;i<6;i++)
                {
                    T4s.sec=T4s.sec*256+delayTime_ptr->t4.sec[i];
                }
                
                T4s.nsec=0;
                for(i=0;i<4;i++)
                {
                    T4s.nsec=T4s.nsec*256+delayTime_ptr->t4.nanosec[i];
                }
                
                T4=T4s.sec;
                T4=T4*1e9;
                T4=T4+T4s.nsec;

                DBG_PRINT(1,"T1=%lld\n",T1);
                DBG_PRINT(1,"T2=%lld\n",T2);
                DBG_PRINT(1,"T3=%lld\n",T3);
                DBG_PRINT(1,"T4=%lld\n",T4);

                if(T2==T3)
                {
                    DBG_PRINT(1,"T2==T3\n");
                    break;
                }

                /* delay= ((t2 - t3) + (t4 - t1)) / 2 */
                delay=((T2-T3)+(T4-T1))/2;
                
                /* offset = t2 - t1 - delay */
                offset=(T2-T1)-delay;

                DBG_PRINT(1,"delay=%lld offset=%lld\n",delay,offset);

                if(adj_offset==ENABLED)
                {
                    if((T1 - setT1) > 1900000000)
                    {
                        ptp_set_offest(offset);
                        setT1 = T1;
                    }
                }

                /*set frequency*/
                if(preT1==0 || T1<=preT1 || preT2==0 || T2<=preT2
                    ||preT3==0 || T3<=preT3 || preT4==0 || T4<=preT4)
                {
                    preT1=T1;
                    preT2=T2;
                    preT3=T3;
                    preT4=T3;
                    break;
                }

                ratio=((double)((double)(T1-preT1))/((double)(T2-preT2+offset)) + (double)((double)(T4-preT4))/((double)(T3-preT3+offset)))/2;

//                ratio=(double)((double)(T1-preT1))/((double)(T2-preT2+offset));

                if(adj_freq==ENABLED)
                {
                    if(llabs(offset) < 90000)
                    {
                        ptp_set_freq_by_ratio(ratio);
                    }
                    else
                    {
                        rtk_time_frequency_set(0x4000000);
                    }
                }

                DBG_PRINT(1,"T1 T2 ratio=%.9llf\n",(double)((double)(T1-preT1))/((double)(T2-preT2+offset)),offset);
                DBG_PRINT(1,"T3 T4 ratio=%.9llf\n",(double)((double)(T4-preT4))/((double)(T3-preT3+offset)),offset);
                    
                preT1=T1;
                preT2=T2;
                preT3=T3;
                preT4=T4;

                break;
            default:
                break;
        }
    }
}

static int pps_intr_socket_recv(int fd,intrBcasterMsg_t *pMsgData)
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

uint8 testInt=0;

void *ptp_slave_ppsThread(void *argu)
{
    intrBcasterMsg_t msgData;
    struct sockaddr_nl sa;
    uint32 sign;
    rtk_time_timeStamp_t timeStamp,curTimeStamp;
    uint32 oriFreq,adjFreq;
    int ret;

    if((gIntrSocketFd=socket(PF_NETLINK, SOCK_RAW, INTR_BCASTER_NETLINK_TYPE))==-1)
    {
        DBG_PRINT(1,"[PTP Slave Freq:%s:%d] Netlink socket can't create!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = (1 << MSG_TYPE_PTP);
    if (-1 == bind(gIntrSocketFd, (struct sockaddr*)&sa, sizeof(struct sockaddr)))
    {
        DBG_PRINT(1,"[PTP Slave Freq:%s:%d] Netlink socket can't bind!\n", __FUNCTION__, __LINE__);
        close(gIntrSocketFd);
        return 0;
    }
    
    while (1)
    {
        if (RT_ERR_OK == pps_intr_socket_recv(gIntrSocketFd, &msgData))
        {
            switch(msgData.intrType)
            {
                case MSG_TYPE_PTP:
#if 0
                    RTK_API_ERR_CHK(rtk_time_curTime_latch(),ret);
                    RTK_API_ERR_CHK(rtk_time_curTime_get(&curTimeStamp),ret);
                    DBG_PRINT(1,"EVENT curTimeStamp sec=%lld nsec=%09d\n",curTimeStamp.sec,curTimeStamp.nsec);

                    if(testInt >= 3)
                    {
                        sign = PTP_REF_SIGN_NEGATIVE;
                        timeStamp.sec=0;
                        timeStamp.nsec=(uint32)25893172;
                        RTK_API_ERR_CHK(rtk_time_refTime_set(sign, timeStamp),ret);
                        DBG_PRINT(1,"%s timeStamp.sec=%lld timeStamp.nsec=%09d\n",(sign==PTP_REF_SIGN_POSTIVE)?"increase":"decrease", timeStamp.sec,timeStamp.nsec);
                        testInt=0;
                    }
                    else
                    {
                        testInt++;
                    }
#endif
                    //Time offset
                    if(startAdjustOffset == 1)
                    {
                        if(gOffset != 0)
                        {
                            if(gOffset<0)
                            {
                                sign = PTP_REF_SIGN_POSTIVE;
                                timeStamp.sec=llabs(gOffset)/1e9;
                                timeStamp.nsec=(uint32)(llabs(gOffset)-(timeStamp.sec*1e9));
                                
                            }
                            else if(gOffset>0)
                            {
                                sign = PTP_REF_SIGN_NEGATIVE;
                                timeStamp.sec=gOffset/1e9;
                                timeStamp.nsec=(uint32)(gOffset-(timeStamp.sec*1e9));
                            }
                        
                            RTK_API_ERR_CHK(rtk_time_refTime_set(sign, timeStamp),ret);

                            DBG_PRINT(1,"%s timeStamp.sec=%lld timeStamp.nsec=%09d\n",(sign==PTP_REF_SIGN_POSTIVE)?"increase":"decrease", timeStamp.sec,timeStamp.nsec);
                        }
                        startAdjustOffset = 0;
                    }

//                    DBG_PRINT(1,"[%s:%d] MSG_TYPE_PTP\n", __FUNCTION__, __LINE__);

                    break;
                default:
                    DBG_PRINT(1,"[%s:%d] Unknown interrupt\n", __FUNCTION__, __LINE__);
                    break;
            }
        }
    }
}

#endif
