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


#include "pkt_redirect_user.h"

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>

#include "ptp_master_main.h"
#include "ptp_master_rx.h"
#include "ptp_master_timer.h"

#include <rtdrv/rtdrv_netfilter.h>

#include <netinet/in.h>
#include <rtk/rtusr/include/rtusr_util.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
int pktRedirect_sock=-1;

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

static void ptp_packet_parsing(unsigned short dataLen,unsigned char *data)
{
    uint8 cpu_offset=0;
    int i;

    DBG_PRINT(3,"PTP packet len=%d pkt=\n",dataLen);
    for(i=0;i<dataLen && i<PTP_MAX_PKT_SIZE;i++)
    {
        DBG_PRINT(3,"0x%02x ",data[i]);
    }
    DBG_PRINT(3,"\n");

    if(dataLen < 48) //48 is PTP header size
    {
        DBG_PRINT(2,"Error PTP packet len=%d pkt=\n",dataLen);
        for(i=0;i<dataLen;i++)
        {
            DBG_PRINT(2,"0x%02x ",data[i]);
        }
        DBG_PRINT(2,"\n");
        return;
    }

    if((data[12] == 0x88 ) && (data[13] == 0x99))
    {
        cpu_offset=sizeof(ptp_pkt_cputag_t);

        if(data[15] == 243)
        {
            DBG_PRINT(2,"Rx Mirror packet no parsing len=%d pkt=\n",dataLen);
            for(i=0;i<dataLen;i++)
            {
                DBG_PRINT(2,"0x%02x ",data[i]);
            }
            DBG_PRINT(2,"\n");

            return;
        }
    }

    switch(data[PTP_MTYPE_offset+cpu_offset])
    {
        case PTP_Sync:
            DBG_PRINT(2,"Parsing PTP_Sync packet\n");
            break;
        case PTP_Delay_Req:
            DBG_PRINT(2,"Parsing PTP_Delay_Req packet\n");
            break;
        case PTP_Pdelay_Req:
            DBG_PRINT(2,"Parsing PTP_Pdelay_Req packet\n");
            break;
        case PTP_Pdelay_Resp:
            DBG_PRINT(2,"Parsing PTP_Pdelay_Resp packet\n");
            break;
        case PTP_Follow_Up:
            DBG_PRINT(2,"Parsing PTP_Follow_Up packet\n");
            break;
        case PTP_Delay_Resp:
            DBG_PRINT(2,"Parsing PTP_Delay_Resp packet\n");
            break;
        case PTP_Pdelay_Resp_Follow_Up:
            DBG_PRINT(2,"Parsing PTP_Pdelay_Resp_Follow_Up packet\n");
            break;
        case PTP_Announce:
            DBG_PRINT(2,"Parsing PTP_Announce packet\n");
            return;
            break;
        case PTP_Rx_Mirror:
            DBG_PRINT(2,"Parsing PTP_Rx_Mirror packet\n");
            break;
        default:
            DBG_PRINT(2,"Unsupport Message Type packet len=%d pkt=\n",dataLen);
            for(i=0;i<dataLen &&i<PTP_MAX_PKT_SIZE;i++)
            {
                DBG_PRINT(2,"0x%02x ",data[i]);
            }
            DBG_PRINT(2,"\n");
            return;
    }

    DBG_PRINT(2,"DA=%02x:%02x:%02x:%02x:%02x:%02x\n",data[PTP_DA_offset+cpu_offset],data[PTP_DA_offset+cpu_offset+1],data[PTP_DA_offset+cpu_offset+2],
                                                     data[PTP_DA_offset+cpu_offset+3],data[PTP_DA_offset+cpu_offset+4],data[PTP_DA_offset+cpu_offset+5]);
    DBG_PRINT(2,"SA=%02x:%02x:%02x:%02x:%02x:%02x\n",data[PTP_SA_offset+cpu_offset],data[PTP_SA_offset+cpu_offset+1],data[PTP_SA_offset+cpu_offset+2],
                                                     data[PTP_SA_offset+cpu_offset+3],data[PTP_SA_offset+cpu_offset+4],data[PTP_SA_offset+cpu_offset+5]);
    DBG_PRINT(2,"ETH=0x%02x%02x\n",data[PTP_ETHERTYPE_offset+cpu_offset],data[PTP_ETHERTYPE_offset+cpu_offset+1]);
    DBG_PRINT(2,"MTYPE=0x%02x\n",data[PTP_MTYPE_offset+cpu_offset]);
    DBG_PRINT(2,"VER=0x%02x\n",data[PTP_VER_offset+cpu_offset]);
    DBG_PRINT(2,"MLEN=0x%02x%02x\n",data[PTP_MLEN_offset+cpu_offset],data[PTP_MLEN_offset+cpu_offset+1]);
    DBG_PRINT(2,"DOMAIN=0x%02x\n",data[PTP_DMAIN_offset+cpu_offset]);
    DBG_PRINT(2,"FLAG=0x%02x%02x\n",data[PTP_FLAG_offset+cpu_offset],data[PTP_FLAG_offset+cpu_offset+1]);
    DBG_PRINT(2,"CFNS=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_CFNS_offset+cpu_offset],data[PTP_CFNS_offset+cpu_offset+1],data[PTP_CFNS_offset+cpu_offset+2]
                                                   ,data[PTP_CFNS_offset+cpu_offset+3],data[PTP_CFNS_offset+cpu_offset+4],data[PTP_CFNS_offset+cpu_offset+5]);
    DBG_PRINT(2,"CFSUBNS=0x%02x%02x\n",data[PTP_CFSUBNS_offset+cpu_offset],data[PTP_CFSUBNS_offset+cpu_offset+1]);
    DBG_PRINT(2,"CLOCKID=0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[PTP_CLOCKID_offset+cpu_offset],data[PTP_CLOCKID_offset+cpu_offset+1],data[PTP_CLOCKID_offset+cpu_offset+2],data[PTP_CLOCKID_offset+cpu_offset+3]
                                                              ,data[PTP_CLOCKID_offset+cpu_offset+4],data[PTP_CLOCKID_offset+cpu_offset+5],data[PTP_CLOCKID_offset+cpu_offset+6],data[PTP_CLOCKID_offset+cpu_offset+7]);
    DBG_PRINT(2,"SRCPORTID=0x%02x%02x\n",data[PTP_SRCPORTID_offset+cpu_offset],data[PTP_SRCPORTID_offset+cpu_offset+1]);
    DBG_PRINT(2,"SQID=0x%02x%02x\n",data[PTP_SQID_offset+cpu_offset],data[PTP_SQID_offset+cpu_offset+1]);
    DBG_PRINT(2,"CTRL=0x%02x\n",data[PTP_CTRL_offset+cpu_offset]);
    DBG_PRINT(2,"MGPERIOD=0x%02x\n",data[PTP_MGPERIOD_offset+cpu_offset]);

    switch(data[PTP_MTYPE_offset+cpu_offset])
    {
        case PTP_Sync:
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            break;
        case PTP_Delay_Req:
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            break;
        case PTP_Pdelay_Req:
            DBG_PRINT(2,"PTP_Pdelay_Req packet\n");
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            break;
        case PTP_Pdelay_Resp:
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            DBG_PRINT(2,"REQCLOCKID=0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[PTP_REQCLOCKID_offset+cpu_offset],data[PTP_REQCLOCKID_offset+cpu_offset+1],data[PTP_REQCLOCKID_offset+cpu_offset+2],data[PTP_REQCLOCKID_offset+cpu_offset+3]
                                                                         ,data[PTP_REQCLOCKID_offset+cpu_offset+4],data[PTP_REQCLOCKID_offset+cpu_offset+5],data[PTP_REQCLOCKID_offset+cpu_offset+6],data[PTP_REQCLOCKID_offset+cpu_offset+7]);
            DBG_PRINT(2,"REQPORTID=0x%02x%02x\n",data[PTP_REQPORTID_offset+cpu_offset],data[PTP_REQPORTID_offset+cpu_offset+1]);
            break;
        case PTP_Follow_Up:
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            break;
        case PTP_Delay_Resp:
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            DBG_PRINT(2,"REQCLOCKID=0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[PTP_REQCLOCKID_offset+cpu_offset],data[PTP_REQCLOCKID_offset+cpu_offset+1],data[PTP_REQCLOCKID_offset+cpu_offset+2],data[PTP_REQCLOCKID_offset+cpu_offset+3]
                                                                         ,data[PTP_REQCLOCKID_offset+cpu_offset+4],data[PTP_REQCLOCKID_offset+cpu_offset+5],data[PTP_REQCLOCKID_offset+cpu_offset+6],data[PTP_REQCLOCKID_offset+cpu_offset+7]);
            DBG_PRINT(2,"REQPORTID=0x%02x%02x\n",data[PTP_REQPORTID_offset+cpu_offset],data[PTP_REQPORTID_offset+cpu_offset+1]);
            break;
        case PTP_Pdelay_Resp_Follow_Up:
            DBG_PRINT(2,"TSSEC=0x%02x%02x%02x%02x%02x%02x\n",data[PTP_TSSEC_offset+cpu_offset],data[PTP_TSSEC_offset+cpu_offset+1],data[PTP_TSSEC_offset+cpu_offset+2]
                                                            ,data[PTP_TSSEC_offset+cpu_offset+3],data[PTP_TSSEC_offset+cpu_offset+4],data[PTP_TSSEC_offset+cpu_offset+5]);
            DBG_PRINT(2,"TSNSEC=0x%02x%02x%02x%02x\n",data[PTP_TSNSEC_offset+cpu_offset],data[PTP_TSNSEC_offset+cpu_offset+1]
                                                     ,data[PTP_TSNSEC_offset+cpu_offset+2],data[PTP_TSNSEC_offset+cpu_offset+3]);
            DBG_PRINT(2,"REQCLOCKID=0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[PTP_REQCLOCKID_offset+cpu_offset],data[PTP_REQCLOCKID_offset+cpu_offset+1],data[PTP_REQCLOCKID_offset+cpu_offset+2],data[PTP_REQCLOCKID_offset+cpu_offset+3]
                                                                         ,data[PTP_REQCLOCKID_offset+cpu_offset+4],data[PTP_REQCLOCKID_offset+cpu_offset+5],data[PTP_REQCLOCKID_offset+cpu_offset+6],data[PTP_REQCLOCKID_offset+cpu_offset+7]);
            DBG_PRINT(2,"REQPORTID=0x%02x%02x\n",data[PTP_REQPORTID_offset+cpu_offset],data[PTP_REQPORTID_offset+cpu_offset+1]);
            break;
    }

}

void *ptp_master_rxThread(void *argu)
{
    int ret;
    ptpMsgBuf_t pmsg;
    int i;

    if(ptk_redirect_userApp_reg(pktRedirect_sock, PR_USER_UID_PTPMASTER, PTP_MAX_PKT_SIZE)<0)
        printf("Register Packet Redirect Fail.\n");

    memset(&pmsg,0,sizeof(ptpMsgBuf_t));

    while(1)
    {
        ret = ptk_redirect_userApp_recvPkt(pktRedirect_sock, PTP_MAX_PKT_SIZE, &pmsg.mtext.pkt_len, pmsg.mtext.pkt);
        if(ret <= 0)
        {
            printf("[PTP rx:%s:%d] receive fail %d\n", __FUNCTION__, __LINE__, ret);
            continue;
        }
  
        if((pmsg.mtext.pkt[12] == 0x88 ) && (pmsg.mtext.pkt[13] == 0x99)
            && ((pmsg.mtext.pkt[15] == 240) || (pmsg.mtext.pkt[15] == 241 || (pmsg.mtext.pkt[15] == 242) || (pmsg.mtext.pkt[15] == 243))))
        {
            if(ptp_rx_pkt_msg_send(&pmsg)!=RT_ERR_OK)
            {
                printf("[PTP rx:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
            }

            if(pmsg.mtext.pkt[15] == 243)
            {
                DBG_PRINT(2,"***Receive PTP Packet Tx Mirror***\n");
            }
            else
            {
                DBG_PRINT(2,"***Receive PTP Packet***\n");
            }
            ptp_packet_parsing(pmsg.mtext.pkt_len,pmsg.mtext.pkt);
        }
        else
        {
            DBG_PRINT(2,"[PTP rx:%s:%d] CPU tag 0x%02x%02x reason %d is not correct\n", __FUNCTION__, __LINE__
                ,pmsg.mtext.pkt[12],pmsg.mtext.pkt[13]
                ,pmsg.mtext.pkt[15]);
        }      
    }
    ptk_redirect_userApp_dereg(pktRedirect_sock, PR_USER_UID_PTPMASTER);
    close(pktRedirect_sock);
}

void ptp_master_rx_exit(void)
{
    ptk_redirect_userApp_dereg(pktRedirect_sock, PR_USER_UID_PTPMASTER);
    close(pktRedirect_sock);
}

void ptp_master_pkt_send(unsigned short dataLen,unsigned char *data,unsigned int portmask)
{
    rtdrv_pktdbg_t pkt_info;
    
    DBG_PRINT(2,"***Send PTP Packet***\n");
    ptp_packet_parsing(dataLen,data);

    memset(&pkt_info,0,sizeof(rtdrv_pktdbg_t));

    pkt_info.tx_count = 1;
    pkt_info.length = dataLen;    
    pkt_info.tx_info.tx_portmask=portmask;
    memcpy(&pkt_info.buf[0], &data[0], pkt_info.length);
    SETSOCKOPT(RTDRV_PKT_SEND, &pkt_info, rtdrv_pktdbg_t, 1);
}

#endif
