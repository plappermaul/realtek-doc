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
#include <sys/stat.h>

#include <rtk/time.h>
#include <rtk/l2.h>

#include "pkt_redirect_user.h"

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>

#include "ptp_master_rx.h"
#include "ptp_master_main.h"
#include "ptp_master_timer.h"

#include <arpa/inet.h>

#include <hal/common/halctrl.h>


/*
 * Symbol Definition
 */

/*
 * Macro Definition
 */


/*
 * Data Declaration
 */
char announce_packet[ANNOUNCE_LEN]= {
    0x01,0x1b,0x19,0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x0b,0x02,
    0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x37,0x05,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x80,0xf8,0xfe,
    0xff,0xff,0x80,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x01,0xa0
};

char sync_packet[SYN_LEN]= {
    0x01,0x1b,0x19,0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x00,0x02,
    0x00,0x2c,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

char follow_up_packet[FOLLOW_UP_LEN]= {
    0x01,0x1b,0x19,0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x08,0x02,
    0x00,0x2c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x02,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

char delay_resp_packet[DELAY_RESP_LEN]= {
    0x01,0x1b,0x19,0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x09,0x02,
    0x00,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x03,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

char pdelay_resp_packet[PDELAY_RESP_LEN]= {
    0x01,0x80,0xc2,0x00,0x00,0x0e,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x03,0x02,
    0x00,0x36,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x05,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

char pdelay_resp_follow_up_packet[PDELAY_RESP_FOLLOW_UP_LEN]= {
    0x01,0x80,0xc2,0x00,0x00,0x0e,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x0a,0x02,
    0x00,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x05,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

static int msgQId=-1;

static uint16 ANN_SQID=1;
static uint16 SQID=1;

static PTP_MODE_t ptp_cur_mode=PTP_MODE_TWO_STEP;
static rtk_enable_t ptp_announce=ENABLED;
static rtk_enable_t ptp_peer=ENABLED;

static uint16 ptp_announce_time=ANNOUNCE_INTER_TIME;
static uint16 ptp_sync_s_time=SYNC_INTER_TIME;
static uint16 ptp_sync_ms_time=0;

static uint8 gm_p1=128;
static uint8 gm_p2=128;
static uint32 gm_clock_qual=0xf8feffff;
static uint8 gm_time_src=0xa0;

static rtk_enable_t deamon_flag=ENABLED;

static uint8 utpMask=0x0;


/*for cpu reason 243, rx mirror packet information is not enough*/
static char store_pre_packet=PTP_Sync;

extern int pktRedirect_sock;

int debug=0;

/*
 * Function Declaration
 */

int32 ptp_announce_timeout_msg_send(void)
{
    ptpTimeMsgBuf_t pmsg;

    if(msgQId==-1)
    {
        printf("[PTP Master:%s:%d] Error queue is not ready!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    memset(&pmsg,0,sizeof(ptpTimeMsgBuf_t));

    pmsg.mtype=MQ_ANNOUNCE_TIMEOUT;

    if(msgsnd(msgQId,(void *)&pmsg,sizeof(pmsg.mtext),IPC_NOWAIT)<0)
    {
        printf("[PTP Master:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int32 ptp_sync_timeout_msg_send(void)
{
    ptpTimeMsgBuf_t pmsg;

    if(msgQId==-1)
    {
        printf("[PTP Master:%s:%d] Error queue is not ready!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    memset(&pmsg,0,sizeof(ptpTimeMsgBuf_t));

    pmsg.mtype=MQ_SYNC_TIMEOUT;

    if(msgsnd(msgQId,(void *)&pmsg,sizeof(pmsg.mtext),IPC_NOWAIT)<0)
    {
        printf("[PTP Master:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int32 ptp_rx_pkt_msg_send(ptpMsgBuf_t *pmsg_ptr)
{
    if(pmsg_ptr==NULL)
    {
        printf("[PTP Master:%s:%d] Error packet pointer!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    if(msgQId==-1)
    {
        printf("[PTP Master:%s:%d] Error queue is not ready!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    if(pmsg_ptr->mtext.pkt_len>PTP_MAX_PKT_SIZE)
    {
        printf("[PTP Master:%s:%d] Error packet size!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    pmsg_ptr->mtype=MQ_RX_PACKET;

    //size add sizeof(pmsg.mtype) because msgsnd may have some issue, it need more 4byte.
    if(msgsnd(msgQId,(void *)pmsg_ptr,pmsg_ptr->mtext.pkt_len+sizeof(pmsg_ptr->mtype),IPC_NOWAIT)<0)
    {
        printf("[PTP Master:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

void *ptp_master_mainThread(void *argu)
{
    ptpMsgBuf_t pmsg;
    ptp_packet_t *pkt_ptr;
    int msgLen,i,tag_offset;
    uint16 *SQIDPtr;
    rtk_time_timeStamp_t timeStamp;
    ptp_pkt_cputag_t *cpuTagPtr;
    int32 ret = RT_ERR_OK;

    msgQId = msgget(PTP_MASTER_MSG_KEY , 666 | IPC_CREAT);
    if(msgQId==-1)
    {
        printf("[PTP Master:%s:%d] Can't initialize message queue!\n", __FUNCTION__, __LINE__);
    }

    pkt_ptr=&pmsg.mtext;
    
    while(1)
    {
        msgLen = msgrcv(msgQId,(void *)&pmsg, sizeof(ptp_packet_t), MQ_PRI_MAX, MSG_NOERROR);
        if(msgLen <= 0)
        {
            DBG_PRINT(1,"[PTP Master:%s:%d] No message from queue!\n", __FUNCTION__, __LINE__);
            continue;
        }
        
        switch(pmsg.mtype)
        {
            case MQ_ANNOUNCE_TIMEOUT:
                SQIDPtr=(uint16 *)&announce_packet[PTP_SQID_offset];
                *SQIDPtr=htons(ANN_SQID);
                ptp_master_pkt_send(ANNOUNCE_LEN,announce_packet,utpMask);
                ANN_SQID++;
                break;
            case MQ_SYNC_TIMEOUT:
                SQIDPtr=(uint16 *)&sync_packet[PTP_SQID_offset];
                *SQIDPtr=htons(SQID);
                ptp_master_pkt_send(SYN_LEN,sync_packet,utpMask);
                SQID++;
                
                if((ptp_cur_mode==PTP_MODE_TWO_STEP) && (ptp_peer==ENABLED))
                    store_pre_packet=PTP_Sync;
                break;
            case MQ_RX_PACKET:
                cpuTagPtr = (ptp_pkt_cputag_t *)&(pkt_ptr->pkt[12]);
                timeStamp.sec=0;
                for(i=0;i<6;i++)
                {
                    timeStamp.sec=timeStamp.sec*256+cpuTagPtr->sec[i];
                }
                timeStamp.nsec=0;
                for(i=0;i<4;i++)
                {
                    timeStamp.nsec=timeStamp.nsec*256+cpuTagPtr->nanosec[i];
                }

                tag_offset = sizeof(ptp_pkt_cputag_t);
                if((pkt_ptr->pkt[PTP_ETHERTYPE_offset+tag_offset] == 0x88 ) && (pkt_ptr->pkt[PTP_ETHERTYPE_offset+1+tag_offset] == 0xa8))
                {
                    tag_offset+=4;
                }
                if((pkt_ptr->pkt[PTP_ETHERTYPE_offset+tag_offset] == 0x81 ) && (pkt_ptr->pkt[PTP_ETHERTYPE_offset+1+tag_offset] == 0x00))
                {
                    tag_offset+=4;
                }

                /*check ptp ethertype*/
                if((pkt_ptr->pkt[PTP_ETHERTYPE_offset+tag_offset] == 0x88 ) && (pkt_ptr->pkt[PTP_ETHERTYPE_offset+1+tag_offset] == 0xf7))
                {
                    if((pkt_ptr->pkt[PTP_MTYPE_offset+tag_offset]&0xf)==PTP_Delay_Req)
                    {
                        /*delay_req*/
                        DBG_PRINT(1,"[PTP Master:%s:%d] receiving Delay_Req pkt_len=%d sec=%lld nsec=%u from %d!\n"
                            ,__FUNCTION__,__LINE__,pkt_ptr->pkt_len,timeStamp.sec,timeStamp.nsec,cpuTagPtr->word&0x3);

                        if(pkt_ptr->pkt_len < (DELAY_REQ_LEN+tag_offset))
                        {
                            DBG_PRINT(1,"[PTP Master:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                            for(i=0;i<pkt_ptr->pkt_len && i<PTP_MAX_PKT_SIZE;i++)
                            {
                                DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                            }
                            DBG_PRINT(1,"\n");
                            break;
                        }

                        memcpy(&delay_resp_packet[PTP_DMAIN_offset],&(pkt_ptr->pkt[PTP_DMAIN_offset+tag_offset]),1);
                        memcpy(&delay_resp_packet[PTP_SQID_offset],&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset]),2);
                        memcpy(&delay_resp_packet[PTP_CFNS_offset],&(pkt_ptr->pkt[PTP_CFNS_offset+tag_offset]),8);
                        
                        /*Putting T4 in timestamp field*/
                        memcpy(&delay_resp_packet[PTP_TSSEC_offset],&cpuTagPtr->sec[0],6);
                        memcpy(&delay_resp_packet[PTP_TSNSEC_offset],&cpuTagPtr->nanosec[0],4);

                        memcpy(&delay_resp_packet[PTP_REQCLOCKID_offset],&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10);

                        DBG_PRINT(1,"[PTP Master:%s:%d] sending Delay_Resp!\n", __FUNCTION__, __LINE__);
                        
                        ptp_master_pkt_send(DELAY_RESP_LEN,delay_resp_packet,1<<(cpuTagPtr->word&0x3));
                        break;
                    }

                    if(ptp_peer==ENABLED)
                    {
                        if((pkt_ptr->pkt[PTP_MTYPE_offset+tag_offset]&0xf)==PTP_Pdelay_Req)
                        {
                            if(ptp_cur_mode == PTP_MODE_ONE_STEP)
                            {
                                /*one-step pdelay_req*/
                                DBG_PRINT(1,"[PTP Master:%s:%d] receiving Peer Delay_Req pkt_len=%d sec=%lld nsec=%u from %d!\n"
                                    ,__FUNCTION__,__LINE__,pkt_ptr->pkt_len,timeStamp.sec,timeStamp.nsec,cpuTagPtr->word&0x3);
                                
                                if(pkt_ptr->pkt_len < (PDELAY_REQ_LEN+tag_offset))
                                {
                                    DBG_PRINT(1,"[PTP Master:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                                    for(i=0;i<pkt_ptr->pkt_len && i<PTP_MAX_PKT_SIZE;i++)
                                    {
                                        DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                                    }
                                    DBG_PRINT(1,"\n");
                                    break;
                                }

                                memcpy(&pdelay_resp_packet[PTP_DMAIN_offset],&(pkt_ptr->pkt[PTP_DMAIN_offset+tag_offset]),1);
                                memcpy(&pdelay_resp_packet[PTP_SQID_offset],&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset]),2);
                                memcpy(&pdelay_resp_packet[PTP_CFNS_offset],&(pkt_ptr->pkt[PTP_CFNS_offset+tag_offset]),8);
                                
                                memcpy(&pdelay_resp_packet[PTP_REQCLOCKID_offset],&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10);                                

                                /*Putting T3-T2 in correction field*/
                                if((ret=rtk_time_rxTime_set(timeStamp))!=RT_ERR_OK)
                                {
                                    DBG_PRINT(1,"[PTP Master:%s:%d] Setting Putting T3-T2 in correction field is error!\n", __FUNCTION__, __LINE__);
                                }
                                
                                DBG_PRINT(1,"[PTP Master:%s:%d] sending Peer Delay_Resp!\n", __FUNCTION__, __LINE__);

                                ptp_master_pkt_send(PDELAY_RESP_LEN,pdelay_resp_packet,1<<(cpuTagPtr->word&0x3));
                                break;
                            }
                            else if(ptp_cur_mode == PTP_MODE_TWO_STEP)
                            {
                                /*two-step pdelay_req*/
                                DBG_PRINT(1,"[PTP Master:%s:%d] receiving Peer Delay_Req pkt_len=%d sec=%lld nsec=%u from %d!\n"
                                    ,__FUNCTION__,__LINE__,pkt_ptr->pkt_len,timeStamp.sec,timeStamp.nsec,cpuTagPtr->word&0x3);

                                if(pkt_ptr->pkt_len < (PDELAY_REQ_LEN+tag_offset))
                                {
                                    DBG_PRINT(1,"[PTP Master:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                                    for(i=0;i<pkt_ptr->pkt_len && i<PTP_MAX_PKT_SIZE;i++)
                                    {
                                        DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                                    }
                                    DBG_PRINT(1,"\n");
                                    break;
                                }
                                
                                memcpy(&pdelay_resp_packet[PTP_DMAIN_offset],&(pkt_ptr->pkt[PTP_DMAIN_offset+tag_offset]),1);
                                memcpy(&pdelay_resp_packet[PTP_SQID_offset],&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset]),2);
                                memcpy(&pdelay_resp_packet[PTP_CFNS_offset],&(pkt_ptr->pkt[PTP_CFNS_offset+tag_offset]),8);

                                /*Putting T2 in timestamp field*/
                                memcpy(&pdelay_resp_packet[PTP_TSSEC_offset],&cpuTagPtr->sec[0],6);
                                memcpy(&pdelay_resp_packet[PTP_TSNSEC_offset],&cpuTagPtr->nanosec[0],4);
                                
                                memcpy(&pdelay_resp_packet[PTP_REQCLOCKID_offset],&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10);

                                DBG_PRINT(1,"[PTP Master:%s:%d] sending Peer Delay_Resp!\n", __FUNCTION__, __LINE__);

                                ptp_master_pkt_send(PDELAY_RESP_LEN,pdelay_resp_packet,1<<(cpuTagPtr->word&0x3));
                                
                                store_pre_packet=PTP_Pdelay_Resp;
                                break;
                            }
                        }
                    }                    
                }

                /*handling tx mirror packet*/
                if(ptp_cur_mode == PTP_MODE_TWO_STEP)
                {
                    if(cpuTagPtr->reason == 243)/*special for tx mirror*/
                    {
                        if((ptp_peer==ENABLED)&&(store_pre_packet==PTP_Pdelay_Resp))
                        {
                            DBG_PRINT(1,"[PTP Master:%s:%d] receiving tx mirror Pdelay_Resp sec=%lld nsec=%u from %d!\n"
                                ,__FUNCTION__,__LINE__,timeStamp.sec,timeStamp.nsec,cpuTagPtr->word&0x3);
                            
                            memcpy(&pdelay_resp_follow_up_packet[PTP_DMAIN_offset],&pdelay_resp_packet[PTP_DMAIN_offset] ,1);
                            memcpy(&pdelay_resp_follow_up_packet[PTP_SQID_offset],&pdelay_resp_packet[PTP_SQID_offset] ,2);
 
                            /*Putting T3  in timestamp*/
                            memcpy(&pdelay_resp_follow_up_packet[PTP_TSSEC_offset],&cpuTagPtr->sec[0],6);
                            memcpy(&pdelay_resp_follow_up_packet[PTP_TSNSEC_offset],&cpuTagPtr->nanosec[0],4);
 
                            memcpy(&pdelay_resp_follow_up_packet[PTP_REQCLOCKID_offset],&pdelay_resp_packet[PTP_REQCLOCKID_offset],10);
 
                            DBG_PRINT(1,"[PTP Master:%s:%d] sending Pdelay_Resp_Follow_Up!\n", __FUNCTION__, __LINE__);
 
                            /*Because the pdelay_resp will send-back each port in two-step, we send follof_up to corresponding send-back pdelay_resp port*/
                            ptp_master_pkt_send(PDELAY_RESP_FOLLOW_UP_LEN,pdelay_resp_follow_up_packet,1<<(cpuTagPtr->word&0x3));
                            break;
                        }
 
                        DBG_PRINT(1,"[PTP Master:%s:%d] receiving tx mirror Sync sec=%lld nsec=%u from %d!\n"
                            ,__FUNCTION__,__LINE__,timeStamp.sec,timeStamp.nsec,cpuTagPtr->word&0x3);
                        
                        memcpy(&follow_up_packet[PTP_DMAIN_offset],&sync_packet[PTP_DMAIN_offset] ,1);
                        memcpy(&follow_up_packet[PTP_SQID_offset],&sync_packet[PTP_SQID_offset] ,2);
 
                        /*Putting T1 from send-back sync in timestamp*/
                        memcpy(&follow_up_packet[PTP_TSSEC_offset],&cpuTagPtr->sec[0],6);
                        memcpy(&follow_up_packet[PTP_TSNSEC_offset],&cpuTagPtr->nanosec[0],4);
 
                        DBG_PRINT(1,"[PTP Master:%s:%d] sending Follow_Up!\n", __FUNCTION__, __LINE__);
 
                        /*Because the sync will send-back each port in two-step, we send follof_up to corresponding send-back sync port*/
                        ptp_master_pkt_send(FOLLOW_UP_LEN,follow_up_packet,1<<(cpuTagPtr->word&0x3));
                        break;
                    }
                }
                
                DBG_PRINT(1,"[PTP Master:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                for(i=0;i<pkt_ptr->pkt_len && i<PTP_MAX_PKT_SIZE;i++)
                {
                    DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                }
                DBG_PRINT(1,"\n");
                break;
            default:
                printf("[PTP Master:%s:%d] Wrong message type!\n", __FUNCTION__, __LINE__);
                continue;
                break;
        }
    }
}

//initial setting
static int32 ptp_init_setting(void)
{
    rtk_time_ptpMsgType_t ptpMgType;
    uint32 port,i,qual;
    uint64 clock_id;
    int32 ret = RT_ERR_OK;
    rtk_mac_t mac;

    RTK_API_ERR_CHK(rtk_core_init(),ret);

    RTK_API_ERR_CHK(rtk_time_init(),ret);

    for(port=HAL_GET_MIN_CPU_PORT();port<=HAL_GET_MAX_CPU_PORT();port++)
    {
        if(HAL_IS_CPU_PORT(port))
        {
            RTK_API_ERR_CHK(rtk_time_portPtpEnable_set(port,ENABLED),ret);
        }
    }

    for(port=HAL_GET_MIN_ETHER_PORT();port<=HAL_GET_MAX_ETHER_PORT();port++)
    {
        if(HAL_IS_PON_PORT(port))
            continue;
        if(!HAL_IS_ETHER_PORT(port))
            continue;

        RTK_API_ERR_CHK(rtk_time_portPtpEnable_set(port,ENABLED),ret);
        utpMask |= 1<<port;
    }

    for(ptpMgType=PTP_MSG_TYPE_SYNC;ptpMgType<PTP_MSG_TYPE_END;ptpMgType++)
    {
        RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(ptpMgType,PTP_IGR_ACTION_NONE),ret);
        RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(ptpMgType,PTP_EGR_ACTION_NONE),ret);
    }
    
    sync_packet[PTP_MGPERIOD_offset]=SYNC_INTER_TIME;
    announce_packet[PTP_MGPERIOD_offset]=ANNOUNCE_INTER_TIME;

    RTK_API_ERR_CHK(rtk_switch_mgmtMacAddr_get(&mac),ret);

    memcpy(&announce_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);
    memcpy(&sync_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);
    memcpy(&follow_up_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);
    memcpy(&delay_resp_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);
    memcpy(&pdelay_resp_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);
    memcpy(&pdelay_resp_follow_up_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);

    announce_packet[PTP_GM_PRI1_offset]=gm_p1;
    qual=gm_clock_qual;
    for(i=0;i<4;i++)
    {
        announce_packet[PTP_GM_CQ_offset+3-i]=qual%256;
        qual=qual/256;
    }
    announce_packet[PTP_GM_PRI2_offset]=gm_p2;

    memcpy(&announce_packet[PTP_GM_ID_offset],&mac.octet[0],3);
    announce_packet[PTP_GM_ID_offset+3]=0xff;
    announce_packet[PTP_GM_ID_offset+4]=0xfe;
    memcpy(&announce_packet[PTP_GM_ID_offset+5],&mac.octet[3],3);

    announce_packet[PTP_GM_TIMESRC_offset]=gm_time_src;

    memcpy(&announce_packet[PTP_CLOCKID_offset],&announce_packet[PTP_GM_ID_offset],8);
    memcpy(&sync_packet[PTP_CLOCKID_offset],&announce_packet[PTP_GM_ID_offset],8);
    memcpy(&follow_up_packet[PTP_CLOCKID_offset],&announce_packet[PTP_GM_ID_offset],8);
    memcpy(&delay_resp_packet[PTP_CLOCKID_offset],&announce_packet[PTP_GM_ID_offset],8);
    memcpy(&pdelay_resp_packet[PTP_CLOCKID_offset],&announce_packet[PTP_GM_ID_offset],8);
    memcpy(&pdelay_resp_follow_up_packet[PTP_CLOCKID_offset],&announce_packet[PTP_GM_ID_offset],8);

    return ret;
}

//one-step setting
static int32 ptp_one_step_setting(void)
{
    uint16* flagPtr;
    int32 ret = RT_ERR_OK;
    
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_DELAY_REQ,PTP_IGR_ACTION_TRAP2CPU),ret);
    RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(PTP_MSG_TYPE_SYNC_ONE_STEP,PTP_EGR_ACTION_LATCH_TIME),ret);

    PROC_TRAP_WRITE(PTP_Delay_Req,PR_USER_UID_PTPMASTER);

    flagPtr=&announce_packet[PTP_FLAG_offset];
    *flagPtr=(*flagPtr)&((~(1<<9))&0xffff);
    
    flagPtr=&sync_packet[PTP_FLAG_offset];
    *flagPtr=(*flagPtr)&((~(1<<9))&0xffff);
    
    flagPtr=&follow_up_packet[PTP_FLAG_offset];
    *flagPtr=(*flagPtr)&((~(1<<9))&0xffff);

    if(ptp_peer==ENABLED)
    {
        RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP,PTP_IGR_ACTION_TRAP2CPU),ret);
        RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(PTP_MSG_TYPE_PDELAY_RESP,PTP_EGR_ACTION_MODIFY_CORRECTION),ret);

        PROC_TRAP_WRITE(PTP_Pdelay_Req,PR_USER_UID_PTPMASTER);
        
        flagPtr=&pdelay_resp_packet[PTP_FLAG_offset];
        *flagPtr=(*flagPtr)&((~(1<<9))&0xffff);
        
        flagPtr=&pdelay_resp_follow_up_packet[PTP_FLAG_offset];
        *flagPtr=(*flagPtr)&((~(1<<9))&0xffff);
    }
    
    return ret;
}

//two-step setting
static int32 ptp_two_step_setting(void)
{
    uint16* flagPtr;
    int32 ret = RT_ERR_OK;
    FILE *fp;
    
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_DELAY_REQ,PTP_IGR_ACTION_TRAP2CPU),ret);
    RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(PTP_MSG_TYPE_SYNC,PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU),ret);
    
    PROC_TRAP_WRITE(PTP_Delay_Req,PR_USER_UID_PTPMASTER);
    PROC_TRAP_WRITE(PTP_Rx_Mirror,PR_USER_UID_PTPMASTER);
    
    flagPtr=&announce_packet[PTP_FLAG_offset];
    *flagPtr=(*flagPtr)|(((1<<9)&0xffff));
    
    flagPtr=&sync_packet[PTP_FLAG_offset];
    *flagPtr=(*flagPtr)|((((1<<9)&0xffff)));

    flagPtr=&follow_up_packet[PTP_FLAG_offset];
    *flagPtr=(*flagPtr)|((((1<<9)&0xffff)));

    if(ptp_peer==ENABLED)
    {
        RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP,PTP_IGR_ACTION_TRAP2CPU),ret);
        RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(PTP_MSG_TYPE_PDELAY_RESP,PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU),ret);

        PROC_TRAP_WRITE(PTP_Pdelay_Req,PR_USER_UID_PTPMASTER);
        
        flagPtr=&pdelay_resp_packet[PTP_FLAG_offset];
        *flagPtr=(*flagPtr)|((((1<<9)&0xffff)));
        
        flagPtr=&pdelay_resp_follow_up_packet[PTP_FLAG_offset];
        *flagPtr=(*flagPtr)|((((1<<9)&0xffff)));  
    }

    return ret;
}

static int32 ptp_configfile_set(int argc,char *argv[])
{
    int cmd = 1,help=0, dump =0;
    int32 ret = RT_ERR_OK;

    while (cmd < argc)
    {
        if (!strcmp("-s", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            if(atoi(argv[cmd])==1)
                ptp_cur_mode=PTP_MODE_ONE_STEP;
            else if(atoi(argv[cmd])==2)
                ptp_cur_mode=PTP_MODE_TWO_STEP;
            cmd++;
        }
        else if (!strcmp("-at", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            ptp_announce_time=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-st", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            ptp_sync_s_time=atoi(argv[cmd]);
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            ptp_sync_ms_time=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-pa", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            if(atoi(argv[cmd])==1)
                ptp_announce=ENABLED;
            else
                ptp_announce=DISABLED;
            cmd++;
        }
        else if (!strcmp("-pp", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            if(atoi(argv[cmd])==1)
                ptp_peer=ENABLED;
            else
                ptp_peer=DISABLED;
            cmd++;
        }
        else if (!strcmp("-p1", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            gm_p1=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-p2", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            gm_p2=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-cq", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            gm_clock_qual=atoi(argv[cmd]);
            cmd++;
        }
        else if (!strcmp("-ts", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            gm_time_src=atoi(argv[cmd]);
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
        printf("%-5s  %-35s\n","-s","PTP Mode 1:one-step 2:two-step");
        printf("%-5s  %-35s\n","-at","Announce Interval Time");
        printf("%-5s  %-35s\n","-st","Syc Interval Time ex: -st sec msec");
        printf("%-5s  %-35s\n","-pa","Send Announce 1:y 2:n");
        printf("%-5s  %-35s\n","-pp","Peer Delay Support 1:y 2:n");
        printf("%-5s  %-35s\n","-p1","Announce Priority 1");
        printf("%-5s  %-35s\n","-p2","Announce Priority 2");
        printf("%-5s  %-35s\n","-cq","Announce Clock Quality");
        printf("%-5s  %-35s\n","-ts","Announce Time Source");
        printf("%-5s  %-35s\n","-da","Daemon 0:disable 1:enable");
        printf("%-5s  %-35s\n","-du","Dump PTP Master Configuration");
        printf("%-5s  %-35s\n","-d","Debug printf 0,1,2,3");
        printf("%-5s  %-35s\n","-h","Help");
        return RT_ERR_INPUT;
    }

    if(dump==1)
    {
        printf("PTP Master running in %s\n",(ptp_cur_mode==PTP_MODE_ONE_STEP)?"one-step":"two-step");
        printf("Announce interval time : %d sec\n",ptp_announce_time);
        printf("Syc interval time : %d sec %d msec\n",ptp_sync_s_time,ptp_sync_ms_time);
        printf("PTP Send Announce : %s\n",(ptp_announce==ENABLED)?"Enable":"Disable");
        printf("PTP Peer Delay Support : %s\n",(ptp_peer==ENABLED)?"Enable":"Disable");
        printf("Announce info...\n");
        printf("Priority1=%d\n",gm_p1);
        printf("Priority2=%d\n",gm_p2);
        printf("Clock Qualify=0x%08x\n",gm_clock_qual);
        printf("Clock Source=0x%02x\n",gm_time_src);
        printf("Deamon=%d\n",deamon_flag);
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

void ptp_master_exit(int signo) 
{
    ptp_master_rx_exit();

    system("rmmod ptp_drv.ko 2>/dev/null 1>/dev/null");

    exit(1);
}  

int
main(
    int argc,
    char *argv[])
{
    pthread_t mainThread;
    pthread_t rxThread;

    if(ptp_configfile_set(argc,argv)!=RT_ERR_OK)
        return 0;

    system("insmod /lib/modules/ptp_drv.ko 2>/dev/null 1>/dev/null");

    signal(SIGHUP,ptp_master_exit);
    signal(SIGINT,ptp_master_exit);
    signal(SIGQUIT,ptp_master_exit);
    signal(SIGTERM,ptp_master_exit);

    if(deamon_flag==ENABLED)
    {
        daemon_init();
    }

    if(ptp_init_setting()!=RT_ERR_OK)
        return 0;

    /*one-step & two-step setting*/
    if(ptp_cur_mode == PTP_MODE_ONE_STEP)
    {        
        if(ptp_one_step_setting()!=RT_ERR_OK)
            return 0;
    }
    else if(ptp_cur_mode == PTP_MODE_TWO_STEP)
    {
        if(ptp_two_step_setting()!=RT_ERR_OK)
            return 0;
    }
    else
    {
        printf("[PTP Master:%s:%d] current mode is not support\n",__FUNCTION__,__LINE__);
        return 0;
    }

    pktRedirect_sock = socket(PF_NETLINK, SOCK_RAW,NETLINK_USERSOCK);
    if(pktRedirect_sock == -1)
    {
        printf("[PTP Master:%s:%d] can't create socket\n", __FUNCTION__, __LINE__);
        return 0;
    }

    /*thread initial*/
    pthread_create(&mainThread, NULL, &ptp_master_mainThread, NULL);
    pthread_create(&rxThread, NULL, &ptp_master_rxThread, NULL);

    /*timer initial*/
    if(ptp_announce==ENABLED)
        announce_timer_init(ptp_announce_time);

    sync_timer_init(ptp_sync_s_time,ptp_sync_ms_time);
    
    while(1)
    {
        sleep(100);
    }
    
    return 0;
}

#endif
