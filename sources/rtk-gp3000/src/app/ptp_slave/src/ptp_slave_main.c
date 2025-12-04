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
 *           It create three additional threads for packet Rx ,adjust freq ,offset and state control
 *
 * Feature : Start point of the PTP Slave protocol stack. Use individual threads
 *           for packet Rx ,adjust freq ,offset and state control
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
#include <sys/stat.h>

#include <rtk/time.h>
#include <rtk/l2.h>
#include <rtk/intr.h>

#include "pkt_redirect_user.h"

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>

#include "ptp_slave_rx.h"
#include "ptp_slave_main.h"
#include "ptp_slave_calc.h"

#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>

#if defined(CONFIG_RTL9607C_SERIES)
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_table_struct.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#endif

/*
 * Symbol Definition
 */
 
/*
 * Macro Definition
 */

/*
 * Data Declaration
 */
static int msgQId=-1;
int debug=1;
static uint8 bestClockId[10]={0};
static uint8 slaveClockid[10]={0};

static rtk_enable_t deamon_flag=ENABLED;

char delay_req_packet[DELAY_REQ_LEN]= {
    0x01,0x1b,0x19,0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,0x88,0xf7,0x01,0x02,
    0x00,0x2c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x22,0xff,0xff,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x01,0x7f,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

extern int pktRedirect_sock;

/*
 * Function Declaration
 */

int32 ptp_slave_rx_pkt_msg_send(ptpMsgBuf_t *pmsg_ptr)
{
    if(pmsg_ptr==NULL)
    {
        printf("[PTP Slave:%s:%d] Error packet pointer!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    if(msgQId==-1)
    {
        printf("[PTP Slave:%s:%d] Error queue is not ready!\n", __FUNCTION__, __LINE__);
        return RT_ERR_QUEUE_ID;
    }

    if(pmsg_ptr->mtext.pkt_len>PTP_SLAVE_MAX_PKT_SIZE)
    {
        printf("[PTP Slave:%s:%d] Error packet size!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    pmsg_ptr->mtype=MQ_SLAVE_RX_PACKET;

    //size add sizeof(pmsg.mtype) because msgsnd may have some issue, it need more 4byte.
    if(msgsnd(msgQId,(void *)pmsg_ptr,pmsg_ptr->mtext.pkt_len+sizeof(pmsg_ptr->mtype),IPC_NOWAIT)<0)
    {
        printf("[PTP Slave:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

void *ptp_slave_mainThread(void *argu)
{
    int msgLen,i,tag_offset;
    ptpMsgBuf_t pmsg;
    ptp_packet_t *pkt_ptr;
    ptpFDMsgBuf_t fmsg;
    ptp_delayTime_t *dTime_ptr;
    ptp_pkt_cputag_t *cpuTagPtr;
    ptp_slaveTS_t slaveTs;
    uint16 sqid=0,*SQIDPtr;
    uint32 freq;
    int32 ret = RT_ERR_OK;

    pkt_ptr=&pmsg.mtext;
    dTime_ptr=&fmsg.mtext;

    slaveTs.state=PTP_STOP;
    slaveTs.syncNum=0;

    msgQId = msgget(PTP_SLAVE_MSG_KEY , 666 | IPC_CREAT);
    if(msgQId==-1)
    {
        printf("[PTP Slave:%s:%d] Can't initialize message queue!\n", __FUNCTION__, __LINE__);
    }
    while(1)
    {
        msgLen = msgrcv(msgQId,(void *)&pmsg, sizeof(ptp_packet_t), MQ_SLAVE_PRI_MAX, MSG_NOERROR);
        if(msgLen <= 0)
        {
            DBG_PRINT(1,"[PTP Slave:%s:%d] No message from queue!\n", __FUNCTION__, __LINE__);
            continue;
        }
        
        switch(pmsg.mtype)
        {
            case MQ_SLAVE_RX_PACKET:
                cpuTagPtr = (ptp_pkt_cputag_t *)&(pkt_ptr->pkt[12]);

                tag_offset = sizeof(ptp_pkt_cputag_t);
                if((pkt_ptr->pkt[PTP_ETHERTYPE_offset+tag_offset] == 0x88 ) && (pkt_ptr->pkt[PTP_ETHERTYPE_offset+1+tag_offset] == 0xa8))
                {
                    tag_offset+=4;
                }
                if((pkt_ptr->pkt[PTP_ETHERTYPE_offset+tag_offset] == 0x81 ) && (pkt_ptr->pkt[PTP_ETHERTYPE_offset+1+tag_offset] == 0x00))
                {
                    tag_offset+=4;
                }

                if((pkt_ptr->pkt[PTP_ETHERTYPE_offset+tag_offset] == 0x88 ) && (pkt_ptr->pkt[PTP_ETHERTYPE_offset+1+tag_offset] == 0xf7))
                {
                    if((pkt_ptr->pkt[PTP_MTYPE_offset+tag_offset]&0xf)==PTP_Announce)
                    {
                        /*announce*/
                        DBG_PRINT(1,"[PTP Slave:%s:%d] receiving announce!\n",__FUNCTION__,__LINE__);

                        if(pkt_ptr->pkt_len < (ANNOUNCE_LEN+tag_offset))
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                            for(i=0;i<pkt_ptr->pkt_len && i<PTP_SLAVE_MAX_PKT_SIZE;i++)
                            {
                                DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                            }
                            DBG_PRINT(1,"\n");
                            break;
                        }
                        /*select best grand master*/
                        if(memcmp(bestClockId,&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10)!=0)
                        {
                            /*changing best grand master*/
                            memcpy(bestClockId,&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10);
                            memset(&slaveTs,0,sizeof(ptp_slaveTS_t));
                            slaveTs.state=PTP_START;
                            slaveTs.syncNum=0;
                            memset(dTime_ptr,0,sizeof(ptp_delayTime_t));
                        }
                        break;
                    }
                    
                    if((pkt_ptr->pkt[PTP_MTYPE_offset+tag_offset]&0xf)==PTP_Sync)
                    {                        
                        if((pkt_ptr->pkt[PTP_FLAG_offset+tag_offset]&(1<<1))==0)
                        {
                            /*one-step sync*/
                            DBG_PRINT(1,"[PTP Slave:%s:%d] receiving one-step sync!\n",__FUNCTION__,__LINE__);
                            
                            if(pkt_ptr->pkt_len < (SYN_LEN+tag_offset))
                            {
                                DBG_PRINT(1,"[PTP Slave:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                                for(i=0;i<pkt_ptr->pkt_len && i<PTP_SLAVE_MAX_PKT_SIZE;i++)
                                {
                                    DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                                }
                                DBG_PRINT(1,"\n");
                                break;
                            }

                            if(memcmp(bestClockId,&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10)!=0)
                            {
                                DBG_PRINT(1,"[PTP Slave:%s:%d] best clock id is not correct\n",__FUNCTION__,__LINE__);
                                break;
                            }

                            if(slaveTs.state!=PTP_START)
                            {
                                DBG_PRINT(1,"[PTP Slave:%s:%d] delay measurement mechanism is not ready!\n",__FUNCTION__,__LINE__);
                                slaveTs.syncNum++;
                                break;
                            }
                            
                            /*store sequence id*/
                            slaveTs.sqid=ntohs(*((uint16 *)&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset])));

                            /*store T1 and T2*/
                            memcpy(dTime_ptr->t1.sec,&(pkt_ptr->pkt[PTP_TSSEC_offset+tag_offset]),6);
                            memcpy(dTime_ptr->t1.nanosec,&(pkt_ptr->pkt[PTP_TSNSEC_offset+tag_offset]),4);

                            memcpy(dTime_ptr->t2.sec,cpuTagPtr->sec,6);
                            memcpy(dTime_ptr->t2.nanosec,cpuTagPtr->nanosec,4);
                            
                            /*send delay_req*/
                            memcpy(&delay_req_packet[PTP_SQID_offset],&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset]),2);

                            SQIDPtr=(uint16 *)&delay_req_packet[PTP_SQID_offset];
                            *SQIDPtr=htons(slaveTs.sqid);

                            ptp_slave_pkt_send(DELAY_REQ_LEN,delay_req_packet,1<<(cpuTagPtr->word&0x3));
                            break;
                        }
                        else
                        {
                            /*two-step sync*/
                            DBG_PRINT(1,"[PTP Slave:%s:%d] receiving two-step sync!\n",__FUNCTION__,__LINE__);

                            if(pkt_ptr->pkt_len < (SYN_LEN+tag_offset))
                            {
                                DBG_PRINT(1,"[PTP Slave:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                                for(i=0;i<pkt_ptr->pkt_len && i<PTP_SLAVE_MAX_PKT_SIZE;i++)
                                {
                                    DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                                }
                                DBG_PRINT(1,"\n");
                                break;
                            }
                            /*store sequence id*/
                            slaveTs.sqid=ntohs(*((uint16 *)&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset])));

                            /*store T2*/
                            memcpy(dTime_ptr->t2.sec,cpuTagPtr->sec,6);
                            memcpy(dTime_ptr->t2.nanosec,cpuTagPtr->nanosec,4);
                            break;
                        }
                    }
                    
                    if((pkt_ptr->pkt[PTP_MTYPE_offset+tag_offset]&0xf)==PTP_Follow_Up)
                    {
                        /*two-step follow_up*/
                        DBG_PRINT(1,"[PTP Slave:%s:%d] receiving two-step follow_up!\n",__FUNCTION__,__LINE__);
                        
                        if(pkt_ptr->pkt_len < (FOLLOW_UP_LEN+tag_offset))
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                            for(i=0;i<pkt_ptr->pkt_len && i<PTP_SLAVE_MAX_PKT_SIZE;i++)
                            {
                                DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                            }
                            DBG_PRINT(1,"\n");
                            break;
                        }

                        if(memcmp(bestClockId,&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10)!=0)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] best clock id is not correct\n",__FUNCTION__,__LINE__);
                            break;
                        }

                        if(slaveTs.state!=PTP_START)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] delay measurement mechanism is not start!\n",__FUNCTION__,__LINE__);
                            slaveTs.syncNum++;
                            break;
                        }

                        /*check sequence id*/
                        sqid=ntohs(*((uint16 *)&pkt_ptr->pkt[PTP_SQID_offset+tag_offset]));
                        
                        if(slaveTs.sqid!=sqid)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] sequence id is not correct %d\n",__FUNCTION__,__LINE__,sqid);
                            break;
                        }

                        /*store T1*/
                        memcpy(dTime_ptr->t1.sec,&(pkt_ptr->pkt[PTP_TSSEC_offset+tag_offset]),6);
                        memcpy(dTime_ptr->t1.nanosec,&(pkt_ptr->pkt[PTP_TSNSEC_offset+tag_offset]),4);
                        
                        /*send delay_req*/
                        memcpy(&delay_req_packet[PTP_SQID_offset],&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset]),2);

                        SQIDPtr=(uint16 *)&delay_req_packet[PTP_SQID_offset];
                        *SQIDPtr=htons(slaveTs.sqid);

                        ptp_slave_pkt_send(DELAY_REQ_LEN,delay_req_packet,1<<(cpuTagPtr->word&0x3));
                        break;
                    }
                    
                    if((pkt_ptr->pkt[PTP_MTYPE_offset+tag_offset]&0xf)==PTP_Delay_Resp)
                    {
                        /*delay_resp*/
                        DBG_PRINT(1,"[PTP Slave:%s:%d] receiving delay_resp!\n",__FUNCTION__,__LINE__);

                        if(pkt_ptr->pkt_len < (DELAY_RESP_LEN+tag_offset))
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                            for(i=0;i<pkt_ptr->pkt_len && i<PTP_SLAVE_MAX_PKT_SIZE;i++)
                            {
                                DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                            }
                            DBG_PRINT(1,"\n");
                            break;
                        }

                        if(memcmp(bestClockId,&(pkt_ptr->pkt[PTP_CLOCKID_offset+tag_offset]),10)!=0)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] best clock id is not correct\n",__FUNCTION__,__LINE__);
                            break;
                        }
                        
                        if(memcmp(slaveClockid,&(pkt_ptr->pkt[PTP_REQCLOCKID_offset+tag_offset]),10)!=0)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] request clock id is not correct\n",__FUNCTION__,__LINE__);
                            break;
                        }

                        if(slaveTs.state!=PTP_START)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] delay measurement mechanism is not start!\n",__FUNCTION__,__LINE__);
                            break;
                        }

                        /*check sequence id*/
                        sqid=ntohs(*((uint16 *)&(pkt_ptr->pkt[PTP_SQID_offset+tag_offset])));
                        
                        if(slaveTs.sqid!=sqid)
                        {
                            DBG_PRINT(1,"[PTP Slave:%s:%d] sequence id is not correct %d\n",__FUNCTION__,__LINE__,sqid);
                            break;
                        }
                        
                        /*store T4*/
                        memcpy(dTime_ptr->t4.sec,&(pkt_ptr->pkt[PTP_TSSEC_offset+tag_offset]),6);
                        memcpy(dTime_ptr->t4.nanosec,&(pkt_ptr->pkt[PTP_TSNSEC_offset+tag_offset]),4);

                        slaveTs.state=PTP_STOP;

                        /*inform measuring thread*/
                        
                        if(ptp_delay_msg_send(&fmsg)!=RT_ERR_OK)
                        {
                            printf("[PTP Slave:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
                        }
                        
                        memset(&slaveTs,0,sizeof(ptp_slaveTS_t));
                        slaveTs.state=PTP_START;
                        memset(dTime_ptr,0,sizeof(ptp_delayTime_t));
                        break;
                    }
                    
                }

                /*special for tx mirror*/
                if(cpuTagPtr->reason == 243 )
                {
                    DBG_PRINT(1,"[PTP Slave:%s:%d] receiving tx mirror Delay_Req!\n",__FUNCTION__,__LINE__);

                    if(slaveTs.state!=PTP_START)
                    {
                        DBG_PRINT(1,"[PTP Slave:%s:%d] delay measurement mechanism is not start!\n",__FUNCTION__,__LINE__);
                        break;
                    }

                    /*store T3*/
                    memcpy(dTime_ptr->t3.sec,cpuTagPtr->sec,6);
                    memcpy(dTime_ptr->t3.nanosec,cpuTagPtr->nanosec,4);
                    
                    break;
                }
                
                DBG_PRINT(1,"[PTP Slave:%s:%d] Not supporting packet len=%d!\n", __FUNCTION__, __LINE__,pkt_ptr->pkt_len);
                for(i=0;i<pkt_ptr->pkt_len && i<PTP_SLAVE_MAX_PKT_SIZE;i++)
                {
                    DBG_PRINT(1,"0x%02x ",pkt_ptr->pkt[i]);
                }
                DBG_PRINT(1,"\n");
                break;
            default:
                printf("[PTP Slave:%s:%d] Wrong message type!\n", __FUNCTION__, __LINE__);
                continue;
                break;
        }
    }
}

//initial setting
static int32 ptp_init_setting(void)
{
    rtk_time_ptpMsgType_t ptpMgType;
    uint32 port;
    int32 ret = RT_ERR_OK;
    rtk_mac_t mac;
    uint32 freq;
    uint32 data;

    RTK_API_ERR_CHK(rtk_core_init(),ret);
        
    RTK_API_ERR_CHK(rtk_time_init(),ret);

#if defined(CONFIG_RTL9607C_SERIES)
    data = 1;
    RTK_API_ERR_CHK(reg_field_write(RTL9607C_SDS_ANA_COM_REG09r, RTL9607C_REG_RX_SD_POR_SELf, &data),ret);
#endif

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
    }

    for(ptpMgType=PTP_MSG_TYPE_SYNC;ptpMgType<PTP_MSG_TYPE_END;ptpMgType++)
    {
        RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(ptpMgType,PTP_IGR_ACTION_NONE),ret);
        RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(ptpMgType,PTP_EGR_ACTION_NONE),ret);
    }
       
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_SYNC,PTP_IGR_ACTION_TRAP2CPU),ret);
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_FOLLOW_UP,PTP_IGR_ACTION_TRAP2CPU),ret);
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_SYNC_ONE_STEP,PTP_IGR_ACTION_TRAP2CPU),ret);
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_DELAY_RESP,PTP_IGR_ACTION_TRAP2CPU),ret);
    RTK_API_ERR_CHK(rtk_time_ptpIgrMsgAction_set(PTP_MSG_TYPE_ANNOUNCE_SIGNALING,PTP_IGR_ACTION_TRAP2CPU),ret);

    RTK_API_ERR_CHK(rtk_time_ptpEgrMsgAction_set(PTP_MSG_TYPE_DELAY_REQ,PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU),ret);
    
    PROC_TRAP_WRITE(PTP_Sync,PR_USER_UID_PTPSLAVE);
    PROC_TRAP_WRITE(PTP_Follow_Up,PR_USER_UID_PTPSLAVE);
    PROC_TRAP_WRITE(PTP_Delay_Resp,PR_USER_UID_PTPSLAVE);
    PROC_TRAP_WRITE(PTP_Announce,PR_USER_UID_PTPSLAVE);
    PROC_TRAP_WRITE(PTP_Rx_Mirror,PR_USER_UID_PTPSLAVE);

    RTK_API_ERR_CHK(rtk_switch_mgmtMacAddr_get(&mac),ret);
    memcpy(&delay_req_packet[PTP_SA_offset],mac.octet,ETHER_ADDR_LEN);

    memcpy(&slaveClockid[0],&mac.octet[0],3);
    slaveClockid[3]=0xff;
    slaveClockid[4]=0xfe;
    memcpy(&slaveClockid[5],&mac.octet[3],3);

    memcpy(&delay_req_packet[PTP_CLOCKID_offset],&slaveClockid[0],10);

	RTK_API_ERR_CHK(rtk_intr_isr_set(INTR_TYPE_PTP,ENABLED),ret);
    RTK_API_ERR_CHK(rtk_intr_imr_set(INTR_TYPE_PTP,ENABLED),ret);
    
    return ret;
}

static int32 ptp_configfile_set(int argc,char *argv[])
{
    int cmd = 1,help=0,dump =0;
    int32 ret = RT_ERR_OK;

    while (cmd < argc)
    {
        if (!strcmp("-af", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            if(atoi(argv[cmd])==1)
                adj_freq=ENABLED;
            else
                adj_freq=DISABLED;
            cmd++;
        }
        else if (!strcmp("-ao", argv[cmd]))
        {
            cmd++;
            if(cmd>=argc)
            {
                help=1;
                break;
            }
            if(atoi(argv[cmd])==1)
                adj_offset=ENABLED;
            else
                adj_offset=DISABLED;
            cmd++;;
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
        printf("%-5s  %-35s\n","-ao","Enable Adjust Offset 1:y 2:n");
        printf("%-5s  %-35s\n","-af","Enable Adjust Freqneucy 1:y 2:n");
        printf("%-5s  %-35s\n","-da","Daemon 0:disable 1:enable");
        printf("%-5s  %-35s\n","-du","Dump PTP Slave Configuration");
        printf("%-5s  %-35s\n","-d","Debug printf 0,1,2");
        printf("%-5s  %-35s\n","-h","Help");
        return RT_ERR_INPUT;
    }

    if(dump==1)
    {
        printf("Adjust Frequency : %s\n",(adj_freq==ENABLED)?"Enable":"Disable");
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

void ptp_slave_exit(int signo) 
{
    uint32 data;

#if defined(CONFIG_RTL9607C_SERIES)
    data = 0;
    reg_field_write(RTL9607C_SDS_ANA_COM_REG09r, RTL9607C_REG_RX_SD_POR_SELf, &data);
#endif

    ptp_slave_rx_exit();
    
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
    pthread_t calcThread;
    pthread_t ppsIntrThread;

    if(ptp_configfile_set(argc,argv)!=RT_ERR_OK)
        return 0;

    system("insmod /lib/modules/ptp_drv.ko 2>/dev/null 1>/dev/null");

    signal(SIGHUP,ptp_slave_exit);
    signal(SIGINT,ptp_slave_exit);
    signal(SIGQUIT,ptp_slave_exit);
    signal(SIGTERM,ptp_slave_exit);

    if(deamon_flag==ENABLED)
    {
        daemon_init();
    }

    if(ptp_init_setting()!=RT_ERR_OK)
        return 0;
    
    pktRedirect_sock = socket(PF_NETLINK, SOCK_RAW,NETLINK_USERSOCK);
    if(pktRedirect_sock == -1)
    {
        printf("[PTP Slave:%s:%d] can't create socket\n", __FUNCTION__, __LINE__);
        return 0;
    }

    /*thread initial*/
    pthread_create(&mainThread, NULL, &ptp_slave_mainThread, NULL);
    pthread_create(&rxThread, NULL, &ptp_slave_rxThread, NULL);
    pthread_create(&calcThread, NULL, &ptp_slave_calcThread, NULL);
    pthread_create(&ppsIntrThread, NULL, &ptp_slave_ppsThread, NULL);

    while(1)
    {
        sleep(100);
    }
    
    return 0;
}

#endif
