/*
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

#include <rtdrv/rtdrv_netfilter.h>

#include <netinet/in.h>
#include <rtk/rtusr/include/rtusr_util.h>

/*
 * Symbol Definition
 */
 
/*
 * Macro Definition
 */

/*
 * Data Declaration
 */

int pktRedirect_sock=-1;

/*
 * Function Declaration
 */

void *ptp_slave_rxThread(void *argu)
{
    int ret;
    ptpMsgBuf_t pmsg;
    int i;
    
    if(ptk_redirect_userApp_reg(pktRedirect_sock, PR_USER_UID_PTPSLAVE, PTP_SLAVE_MAX_PKT_SIZE)<0)
        printf("Register PTP Slave Packet Redirect Fail.\n");

    memset(&pmsg,0,sizeof(ptpMsgBuf_t));

    while(1)
    {
        ret = ptk_redirect_userApp_recvPkt(pktRedirect_sock, PTP_SLAVE_MAX_PKT_SIZE, &pmsg.mtext.pkt_len, pmsg.mtext.pkt);
        if(ret <= 0)
        {
            printf("[PTP Slave rx:%s:%d] receive fail %d\n", __FUNCTION__, __LINE__, ret);
            continue;
        }
  
        DBG_PRINT(2,"Receiving PTP Slave packet len=%d pkt=\n",pmsg.mtext.pkt_len);
        for(i=0;i<pmsg.mtext.pkt_len;i++)
        {
            DBG_PRINT(2,"0x%02x ",pmsg.mtext.pkt[i]);
        }
        DBG_PRINT(2,"\n");

        if((pmsg.mtext.pkt[12] == 0x88 ) && (pmsg.mtext.pkt[13] == 0x99)
            && ((pmsg.mtext.pkt[15] == 240) || (pmsg.mtext.pkt[15] == 241 || (pmsg.mtext.pkt[15] == 242) || (pmsg.mtext.pkt[15] == 243))))
        {
            if(ptp_slave_rx_pkt_msg_send(&pmsg)!=RT_ERR_OK)
            {
                printf("[PTP Slave rx:%s:%d] Error message send!\n", __FUNCTION__, __LINE__);
            }
        }
        else
        {
            DBG_PRINT(2,"[PTP Slave rx:%s:%d] CPU tag 0x%02x%02x reason %d is not correct\n", __FUNCTION__, __LINE__
                ,pmsg.mtext.pkt[12],pmsg.mtext.pkt[13]
                ,pmsg.mtext.pkt[15]);
        }      
    }
    close(pktRedirect_sock);
}

void ptp_slave_rx_exit(void)
{
    ptk_redirect_userApp_dereg(pktRedirect_sock, PR_USER_UID_PTPSLAVE);
    close(pktRedirect_sock);
}

void ptp_slave_pkt_send(unsigned short dataLen,unsigned char *data,unsigned int portmask)
{
    rtdrv_pktdbg_t pkt_info;
    int i;

    DBG_PRINT(2,"Sending PTP Slave packet len=%d pkt=\n",dataLen);
    for(i=0;i<dataLen;i++)
    {
        DBG_PRINT(2,"0x%02x ",data[i]);
    }
    DBG_PRINT(2,"\n");

    memset(&pkt_info,0,sizeof(rtdrv_pktdbg_t));

    pkt_info.tx_count = 1;
    pkt_info.length = dataLen;
    pkt_info.tx_info.tx_portmask=portmask;
    memcpy(&pkt_info.buf[0], &data[0], pkt_info.length);
    SETSOCKOPT(RTDRV_PKT_SEND, &pkt_info, rtdrv_pktdbg_t, 1);
}

#endif
