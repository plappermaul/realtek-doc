/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Definition of IGMP module
 *
 * Feature : The file includes IGMP kernel functions
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>

#include <net/sock.h>
#include <net/netlink.h>

#include <pkt_redirect.h>
#include <module/gpon/gpon.h>
#include <rtk/ponmac.h>

#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#else
#include "re8686_nic.h"
#endif

#define BUF_SIZE        1600
#if defined(CONFIG_RTL9600_SERIES) || defined(CONFIG_RTL9601B_SERIES)
#define IGMP_MLD_REASON 215
#define RXHOOK_PORTMASK 0xFF
#endif
#if defined(CONFIG_RTL9602C_SERIES)
#define IGMP_MLD_REASON 127
#define RXHOOK_PORTMASK 0xFF
#endif
#if defined(CONFIG_RTL9607C_SERIES)
#define IGMP_MLD_REASON 219
#define RXHOOK_PORTMASK 0x7FF
#endif

typedef struct igmp_msg_s
{
    uint32              sid;            //sid
    uint32              len;            //pkt_length
    uint32              portMask;       //portmask
    uint8               msg[BUF_SIZE];  //payload
} igmp_msg_t;

typedef struct igmp_msg_work_s
{
    struct work_struct  work;
    igmp_msg_t          msgData;
}igmp_ctrl_work_t;

extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);
extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

#define VTAG2VLANTCI(v) (( (((v) & 0xff00)>>8) | (((v) & 0x00ff)<<8) ) + 1)

void skb_push_qtag(struct sk_buff *pSkb, unsigned short usVid, unsigned char ucPriority)
{
    if (usVid)
    {
        /*push switch header*/
        skb_push(pSkb, 4); /*pSkb->data -= 4; pSkb->len += 4*/
        memmove(pSkb->data,
                pSkb->data + 4,
                (2 * 6 /* MAC_ADDR_LEN */));
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */)) = 0x81;
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */) + 1) = 0x00;
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */) + 2) = (unsigned char)(((usVid >> 8) & 0xF) | (ucPriority << 5));
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */) + 3) = usVid & 0xFF;
    }
}

int igmp_ctrl_pkt_rx(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
    int ret;

    /* Filter IGMP or MLD CTRL frames */
    if(IGMP_MLD_REASON == GMAC_RXINFO_REASON(pRxInfo))
    {
    	/*Add port and vlan info into data*/
		skb->vlan_tci = (GMAC_RXINFO_CTAGVA(pRxInfo)) ? VTAG2VLANTCI(GMAC_RXINFO_CVLAN_TAG(pRxInfo)) : 0;
    	skb_push_qtag(skb, skb->vlan_tci & 0xfff /* VLAN_VID_MASK */, 0);
    	skb_push(skb, 3);
		skb->data[0] = GMAC_RXINFO_SRC_PORT_NUM(pRxInfo) + 1;
		skb->data[1] = ((skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) >> 8) & 0xF;
		skb->data[2] = (skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) & 0xFF;
		//printk("%s %d 0x%x 0x%x 0x%x %d\n", __FUNCTION__, __LINE__, skb->data[0], skb->data[1], skb->data[2], skb->len);

        ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_IGMPCTRL, 1, skb->len, skb->data);
        if (ret)
        {
            printk("send to user app (%d) fail (%d)\n", PR_USER_UID_IGMPCTRL, ret);
        }
        /* Left the skb to be free by caller */
        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;
}

static void send_igmp(struct work_struct *p)
{
	int ret;
    struct tx_info ptxInfo;
    rtk_ponmac_mode_t mode;
    rtk_port_t ponPort;

	igmp_ctrl_work_t *pWork = (igmp_ctrl_work_t*) container_of(p, igmp_ctrl_work_t, work);

    if (0 != rtk_ponmac_mode_get(&mode))
    {
        printk("pon mode get failed %s()@%d\n", __FUNCTION__, __LINE__);
        return;
    }

    if (0 != rtk_gpon_port_get(&ponPort))
    {
        printk("pon port get failed %s()@%d\n", __FUNCTION__, __LINE__);
        return;
    }

    memset(&ptxInfo, 0, sizeof(struct tx_info));

    if (PONMAC_MODE_GPON == mode && (pWork->msgData.portMask & (1 << ponPort)))
    {
        //for US
        GMAC_TXINFO_CPUTAG_PSEL(&ptxInfo) = 1;
        GMAC_TXINFO_TX_DST_STREAM_ID(&ptxInfo) = pWork->msgData.sid;
    }
    GMAC_TXINFO_DISLRN(&ptxInfo) = 1;
    GMAC_TXINFO_KEEP(&ptxInfo) = 1;
    GMAC_TXINFO_CPUTAG(&ptxInfo)  = 1;
    //GMAC_TXINFO_EFID(&ptxInfo)  = 1;
    GMAC_TXINFO_TX_PMASK(&ptxInfo)  = pWork->msgData.portMask;

    ret = re8686_tx_with_Info(pWork->msgData.msg, pWork->msgData.len, &ptxInfo);
    kfree(pWork);
	return;

}

static void igmp_send_to_nic(uint16 len, uint8 *pPayload)
{
    igmp_ctrl_work_t *pWork = NULL;
    uint8 *ptr = NULL;
    uint16 msg_len;

    ptr = pPayload;

    pWork =  kzalloc(sizeof(typeof(*pWork)), GFP_KERNEL);
    if (!pWork)
        return;

    msg_len = ((len <= BUF_SIZE) ? len : BUF_SIZE);

    INIT_WORK(&pWork->work, send_igmp);
    pWork->msgData.len = msg_len - sizeof(uint32) - sizeof(uint32);
    memcpy(&pWork->msgData.portMask, ptr, sizeof(uint32));
    ptr += sizeof(uint32);
    memcpy(&pWork->msgData.sid, ptr, sizeof(uint32));
    ptr += sizeof(uint32);
    memcpy(&pWork->msgData.msg[0], ptr, pWork->msgData.len);

	schedule_work(&pWork->work);
    return;

}

int __init igmp_drv_init(void)
{
    /* reg tx */
	if (0 != pkt_redirect_kernelApp_reg(PR_KERNEL_UID_IGMPCTRL, igmp_send_to_nic))
	{
	    printk("reg packet redirect failed, %s()@%d\n", __FUNCTION__, __LINE__);
		return -1;
	}
    /* reg rx */
    drv_nic_register_rxhook(RXHOOK_PORTMASK, RE8686_RXPRI_MUTICAST, igmp_ctrl_pkt_rx);

    return 0;
}

void __exit igmp_drv_exit(void)
{
    /* dereg tx */
    if (0 != pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_IGMPCTRL))
	{
	    printk("dereg packet redirect failed, %s()@%d\n", __FUNCTION__, __LINE__);
		return;
	}
    /* dereg rx */
    drv_nic_unregister_rxhook(RXHOOK_PORTMASK, RE8686_RXPRI_MUTICAST, igmp_ctrl_pkt_rx);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek IGMP kernel module");
MODULE_AUTHOR("RealTek");

module_init(igmp_drv_init);
module_exit(igmp_drv_exit);
