/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : A Linux Ethernet driver for the Realtek Switch SOC.
 *
 * Feature : NIC module
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/ethtool.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/time.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#include <linux/export.h>
#endif
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/time.h>
#include <dev_config.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/print.h>
#include <osal/memory.h>
#include <osal/thread.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <drv/nic/nic.h>
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
#include <drv/l2ntfy/l2ntfy.h>
#endif
#include <rtnic/rtnic_drv.h>
#include <hwp/hw_profile.h>
#include <rtk/switch.h>

/*
 * Symbol Definition
 */
#define PKTBUF_ALLOC(size)    osal_alloc(size)
#define PKTBUF_FREE(pktbuf)   osal_free(pktbuf)

/* System support multiple rx handler priority mechanism
 * The number of rx handler priority is defined in NIC_RX_CB_PRIORITY_NUMBER symbol
 * - 0 is highest priority
 * - NIC_RX_CB_PRIORITY_NUMBER-1 is lowest priority
 *
 * When higher priority rx handler return NIC_RX_HANDLED_OWNED, the lower priority
 * rx handler will not charge to be callback.
 *
 * You need to care rx handler sequence in your system, for example:
 * If you have RRCP & RTNIC rx handler both; you maybe define RRCP rx handler is 0 and
 * RTNIC rx handler is 1 depend on your application.
 */
#define RTNIC_RX_HANDLER_PRIORITY   1

/*
 * Data Declaration
 */
struct rtnic_priv
{
    uint16  ready;
    uint32  msg_enable;
    uint8   traffic_enable;
    struct net_device   *pNdev;
    struct net_device_stats ndev_stats;
    uint32  rx_queue_num;
    uint32  tx_queue_num;
    spinlock_t  lock;
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
unsigned char gNIC_ADDRESS[6];
#endif
static uint32               rtnic_init_status = INIT_NOT_COMPLETED;
static rtnic_handler_list_t *pHandlerListHead = NULL;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtnic_pkt_alloc
 * Description:
 *      packet allocation
 * Input:
 *      unit     - unit id
 *      size     - alloc size
 *      flags    - alloc flags
 * Output:
 *      ppPacket - pointer buffer to the allocated packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 rtnic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    uint32 shift;
    struct sk_buff *pSkb;
    drv_nic_pkt_t *pPacket;

    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (NULL == pPacket)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "allocate nic packet buffer fail\n");
        return RT_ERR_FAILED;
    }

    /* Allocate more space than spcified, so that we can make the whole DMA block has its own cache lines, and guarantee the coherence futher*/
    pSkb = dev_alloc_skb(size + RTNIC_PKTLEN_RSVD + (32 - ((size + RTNIC_PKTLEN_RSVD)%32)) + 28);
    if (NULL == pSkb)
    {
        PKTBUF_FREE(pPacket);
        RT_LOG(LOG_DEBUG, MOD_NIC, "allocate skb fail\n");
        return RT_ERR_FAILED;
    }

    /* Make the begining address of DMA block cache line (32-byte) aligned */
    //pSkb->data = (void*)((uint32)(pSkb->data + 28) & 0xffffffe0);
    shift = 32 - ((uint32)(uintptr)pSkb->data & 0x1f);  //convert for both 32/64-bit system
    pSkb->data += shift;
    skb_reserve(pSkb, RTNIC_PKTLEN_RSVD);


    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = pSkb->head;
    pPacket->data = pSkb->data;
    pPacket->tail = pSkb->data;
    pPacket->end = (uint8*)skb_end_pointer(pSkb);
    pPacket->length = 0;
    pPacket->buf_id = (void *)pSkb;
    pPacket->next = NULL;


    *ppPacket = pPacket;

    return RT_ERR_OK;
} /* end of rtnic_pkt_alloc */

/* Function Name:
 *      rtnic_pkt_free
 * Description:
 *      free allocated packet
 * Input:
 *      unit    - unit id
 *      pPacket - pointer buffer to the packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 rtnic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    struct sk_buff *pSkb;

    if (NULL == pPacket)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "pPacket is NULL\n");
        return RT_ERR_FAILED;
    }

    pSkb = (struct sk_buff *)(pPacket->buf_id);
    if (NULL == pSkb)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "skb is NULL\n");
        return RT_ERR_FAILED;
    }

    /*
     * Always exactly free this packet
     * to prevent 'page allocation failure' problem when ISR hold all CPU time continuously
     */
#if 0
    dev_kfree_skb_any(pSkb);
#else
    dev_kfree_skb(pSkb);
#endif

    PKTBUF_FREE(pPacket);

    return RT_ERR_OK;
} /* end of rtnic_pkt_free */

/* Function Name:
 *      _rtnic_dequeue_thread
 * Description:
 *      (Thread) to dequeue the rx packet for each handler
 * Input:
 *      pInput - point to the handler struct of the creator
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void _rtnic_dequeue_thread(void *pInput)
{
    uint32  burst = 0;
    rtnic_handler_list_t *pHandlerList = (rtnic_handler_list_t *)pInput;


    if (NULL == pInput)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "pInput is NULL\n");

        return;
    }

    /* forever loop */
    for (;;)
    {
        /* deQueue */
        if (pHandlerList->pPkt[pHandlerList->deQueueCnt] != NULL)
        {
            pHandlerList->handler.rx_cbf(pHandlerList->pPkt[pHandlerList->deQueueCnt], pHandlerList->handler.pCookie);
            pHandlerList->pPkt[pHandlerList->deQueueCnt] = NULL;
            pHandlerList->deQueueCnt = ((pHandlerList->deQueueCnt + 1) % RTNIC_HANDLER_QUEUE_LENGTH);

            burst += 1;
            if (burst > RTNIC_HANDLER_BURST_MAX)
            {
                /* force to sleep for a tick */
                osal_time_usleep(10000);
                burst = 0;
            }
        }
        else
        {
            /* Sleep 10ms for release cpu resource.
               Sleep time equals 1 Tick time. If over one tick, it's easy to get queue full. */
            osal_time_usleep(10000);
            burst = 0;
        }

        if (pHandlerList->status)
        {
            break;
        }
    }

    pHandlerList->status = -1;

    osal_thread_exit(0);
} /* end of _l2g_dequeue_thread */

/* Function Name:
 *      rtnic_handler_register
 * Description:
 *      Register the packet Handler
 * Input:
 *      pHandler - point to the handler parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_MEMORY
 * Note:
 *      None
 */
int32 rtnic_handler_register(rtnic_handler_t *pHandler)
{
    rtnic_handler_list_t *pHandlerListNew = NULL;
    rtnic_handler_list_t *pHandlerList = NULL;
    rtnic_handler_list_t *pHandlerListLast = NULL;
    int32 ret = RT_ERR_OK;
    uint32 i = 0;
    uint32 threadPri = 255;

    RT_PARAM_CHK(NULL == pHandler, RT_ERR_NULL_POINTER);

    if ((pHandlerListNew = osal_alloc(sizeof(rtnic_handler_list_t))) != NULL)
    {
        osal_memcpy(&pHandlerListNew->handler, pHandler, sizeof(rtnic_handler_t));
        osal_memset(&pHandlerListNew->pPkt, 0, sizeof(drv_nic_pkt_t *) * RTNIC_HANDLER_QUEUE_LENGTH);

        for(i = 0 ; i < ETHER_ADDR_LEN ; i++)
            pHandlerListNew->handler.smac[i] &= pHandler->smac_caremask[i];

        for(i = 0 ; i < ETHER_ADDR_LEN ; i++)
            pHandlerListNew->handler.dmac[i] &= pHandler->dmac_caremask[i];

        pHandlerListNew->enQueueCnt = 0;
        pHandlerListNew->deQueueCnt = 0;
        pHandlerListNew->status = 0;

        if (NULL == pHandlerListHead)
        {
            /* List to head */
            pHandlerListNew->next = NULL;
            pHandlerListHead = pHandlerListNew;
        }
        else
        {
            //pHandlerListLast = gL2gHandlerListHead;
            pHandlerList = pHandlerListHead;

            while (NULL != pHandlerList)
            {
                if (pHandlerListNew->handler.priority < pHandlerList->handler.priority)
                {
                     pHandlerListNew->next = pHandlerList;

                     if (NULL != pHandlerListLast)
                         pHandlerListLast->next = pHandlerListNew;
                     else
                         pHandlerListHead = pHandlerListNew;

                     break;
                }

                pHandlerListLast = pHandlerList;
                pHandlerList = pHandlerList->next;
            }

            // Link to tail
            if (NULL == pHandlerList)
            {
                pHandlerListLast->next = pHandlerListNew;
                pHandlerListNew->next = NULL;
            }
        }

        pHandlerList = pHandlerListHead;

        while (pHandlerList != NULL)
        {
            RT_LOG(LOG_DEBUG, MOD_NIC, "%p name %s pri %u\n",pHandlerList, pHandlerList->handler.pName, pHandlerList->handler.priority);
            pHandlerList = pHandlerList->next;
        }
    }
    else
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "debug(Out of memory)\n");

        ret = RT_ERR_MEM_ALLOC;  /* Out of memory: failed */
    }

    /* create a kernel thread to dequeue packet */
    if (0 == ret)
    {
        if (pHandlerListNew->handler.priority < RTNIC_HANDLER_PRI_DFLT)
            threadPri = RT_THREAD_PRI_HIGH;
        else if ((pHandlerListNew->handler.priority > RTNIC_HANDLER_PRI_DFLT) && (pHandlerListNew->handler.priority < RTNIC_HANDLER_PRI_LOW))
            threadPri = RT_THREAD_PRI_DFLT;
        else if ((pHandlerListNew->handler.priority > RTNIC_HANDLER_PRI_LOW) && (pHandlerListNew->handler.priority < RTNIC_HANDLER_PRI_END))
            threadPri = RT_THREAD_PRI_LOW;

        if ((osal_thread_t)(uintptr)NULL == (pHandlerListNew->rxThreadId = osal_thread_create(pHandler->pName, 32768, threadPri, (void *)_rtnic_dequeue_thread, pHandlerListNew)))
        {
            RT_LOG(LOG_DEBUG, MOD_NIC, "%s create failed\n", pHandler->pName);

            return RT_ERR_FAILED;
        }
    }

    return ret;
}

uint32 rtnic_pkt_handler(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    rtnic_handler_list_t *pHandlerList = pHandlerListHead;
    uint16 *pEthertype = (uint16 *)(pPacket->data + 12);
    uint8  *pProtocol = (uint8 *)(pPacket->data + 23);
    uint16 *pSrcPort = (uint16 *)(pPacket->data + 34);
    uint16 *pDstPort = (uint16 *)(pPacket->data + 36);
    drv_nic_pkt_t *pPktCopy;
    rtnic_handler_t *handler = NULL;
    rtnic_act_t action = RTNIC_HANDLER_ACT_NONE;
    uint8 *data = NULL;

    RT_PARAM_CHK(NULL == pPacket, NIC_RX_NOT_HANDLED);

    /* parsing this packet */
    if (pPacket->rx_tag.svid_tagged)
    {
        pEthertype += 2;
        pProtocol += 4;
        pSrcPort += 2;
        pDstPort += 2;
    }

    if (pPacket->rx_tag.cvid_tagged)
    {
        pEthertype += 2;
        pProtocol += 4;
        pSrcPort += 2;
        pDstPort += 2;
    }

    /* RFC-1042 SNAP */
    if (0x05FF >= *pEthertype)
    {
        uint8 *pData = (uint8 *)pEthertype;

        if (pData[2] == 0xAA && pData[3] == 0xAA && pData[4] == 0x3)
        {
            pEthertype += 4;
        }
        else
            pEthertype += 1;
    }

    while (pHandlerList != NULL)
    {
        handler = &(pHandlerList->handler);
        data = pPacket->data;

        if (!handler->noCondition)
        {
            if(handler->ethertype != 0 && handler->ethertype != *pEthertype)    goto _NEXT;
            if(handler->proto != 0 && handler->proto != *pProtocol)             goto _NEXT;
            if(handler->dport != 0 && handler->dport != *pDstPort)              goto _NEXT;
            if(handler->sport != 0 && handler->sport != *pSrcPort)              goto _NEXT;

            if((*(data + 0) & handler->dmac_caremask[0]) != handler->dmac[0]) goto _NEXT;
            if((*(data + 1) & handler->dmac_caremask[1]) != handler->dmac[1]) goto _NEXT;
            if((*(data + 2) & handler->dmac_caremask[2]) != handler->dmac[2]) goto _NEXT;
            if((*(data + 3) & handler->dmac_caremask[3]) != handler->dmac[3]) goto _NEXT;
            if((*(data + 4) & handler->dmac_caremask[4]) != handler->dmac[4]) goto _NEXT;
            if((*(data + 5) & handler->dmac_caremask[5]) != handler->dmac[5]) goto _NEXT;

            if((*(data + 6) & handler->smac_caremask[0]) != handler->smac[0]) goto _NEXT;
            if((*(data + 7) & handler->smac_caremask[1]) != handler->smac[1]) goto _NEXT;
            if((*(data + 8) & handler->smac_caremask[2]) != handler->smac[2]) goto _NEXT;
            if((*(data + 9) & handler->smac_caremask[3]) != handler->smac[3]) goto _NEXT;
            if((*(data + 10) & handler->smac_caremask[4]) != handler->smac[4]) goto _NEXT;
            if((*(data + 11) & handler->smac_caremask[5]) != handler->smac[5]) goto _NEXT;
        }

        action = pHandlerList->handler.action;

        if (pHandlerList->handler.chk_cbf != NULL)
            action = pHandlerList->handler.chk_cbf(pPacket, pCookie);

        /* match */
        if (action == RTNIC_HANDLER_ACT_TRAP)
        {
            /* enQueue */
            if (pHandlerList->pPkt[pHandlerList->enQueueCnt] == NULL)
            {
                pHandlerList->pPkt[pHandlerList->enQueueCnt] = (drv_nic_pkt_t *)pPacket;
                pHandlerList->enQueueCnt = ((pHandlerList->enQueueCnt + 1) % RTNIC_HANDLER_QUEUE_LENGTH);
            }
            else
            {
                /* queue is full -> need to free this packet by ourself */
                rtnic_pkt_free(unit, pPacket);
            }

            return NIC_RX_HANDLED_OWNED;
        }
        else if (action == RTNIC_HANDLER_ACT_COPY)
        {
            /* enQueue */
            if (pHandlerList->pPkt[pHandlerList->enQueueCnt] == NULL)
            {
                if (RT_ERR_OK == drv_nic_pkt_alloc(unit, (pPacket->tail - pPacket->data), 0, &pPktCopy))
                {
                    pPktCopy->tail = pPktCopy->data + (pPacket->tail - pPacket->data);
                    pPktCopy->length = pPktCopy->tail - pPktCopy->data;
                    osal_memcpy(pPktCopy->data, pPacket->data, pPktCopy->length);
                    osal_memcpy(&pPktCopy->rx_tag, &pPacket->rx_tag, sizeof(pPktCopy->rx_tag));

                    pHandlerList->pPkt[pHandlerList->enQueueCnt] = pPktCopy;
                    pHandlerList->enQueueCnt = ((pHandlerList->enQueueCnt + 1) % RTNIC_HANDLER_QUEUE_LENGTH);
                }
                else
                {
                    RT_LOG(LOG_DEBUG, MOD_NIC, "alloc packet failed to copy!\n");
                }
            }
        }
        else if (action == RTNIC_HANDLER_ACT_DROP)
        {
            drv_nic_pkt_free(unit, pPacket);

            return NIC_RX_HANDLED_OWNED;
        }
_NEXT:
        pHandlerList = pHandlerList->next;
    }

    goto _exit;

_exit:
    return NIC_RX_NOT_HANDLED;

}

/* Function Name:
 *      rtnic_rx_callback
 * Description:
 *      packet RX callback function
 * Input:
 *      unit    - unit id
 *      pPacket - pointer buffer to the packet
 *      pCookie - cookie data buffer
 * Output:
 *      None
 * Return:
 *      NIC_RX_HANDLED_OWNED
 *      NIC_RX_NOT_HANDLED
 * Note:
 *      None
 */
static drv_nic_rx_t rtnic_rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    struct net_device *pNdev = (struct net_device *)pCookie;
    struct rtnic_priv *pPriv = netdev_priv(pNdev);
    struct sk_buff *pSkb;
    int32  retval;

    if (NULL == pPacket)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "pPacket is NULL\n");
        goto _exit;
    }

    if (NIC_RX_HANDLED_OWNED == rtnic_pkt_handler(unit, pPacket, pCookie))
    {
        return NIC_RX_HANDLED_OWNED;
    }

    pSkb = (struct sk_buff *)(pPacket->buf_id);
    if (NULL == pSkb)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "skb is NULL\n");
        goto _exit;
    }

    /* save the packet info into some fields */
    pSkb->data = pPacket->data;
    skb_set_tail_pointer(pSkb, pPacket->length);
    pSkb->len = pPacket->length;
    if (pPriv)
    {
        pPriv->ndev_stats.rx_packets++;
        pPriv->ndev_stats.rx_bytes += pSkb->len;
    }

    pSkb->dev = pNdev;
    pSkb->protocol = eth_type_trans(pSkb, pNdev);
    pSkb->ip_summed = CHECKSUM_UNNECESSARY;
    pNdev->last_rx = jiffies;

    PKTBUF_FREE(pPacket);


    /* To kernel procotol-stack */
#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
    retval = netif_rx_ni(pSkb);
#else
    retval = netif_rx(pSkb);
#endif

    if (NET_RX_DROP == retval)
    {
        if (pPriv)
        {
            pPriv->ndev_stats.rx_dropped++;
        }
    }

    return NIC_RX_HANDLED_OWNED;

_exit:
    return NIC_RX_NOT_HANDLED;
} /* end of rtnic_rx_callback */

/* Function Name:
 *      rtnic_tx_callback
 * Description:
 *      packet TX callback function
 * Input:
 *      unit    - unit id
 *      pPacket - pointer buffer to the packet
 *      pCookie - cookie data buffer
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void rtnic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    struct net_device *pNdev = (struct net_device *)pCookie;
    struct rtnic_priv *pPriv = netdev_priv(pNdev);
    struct sk_buff *pSkb;

    if (NULL == pPacket)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "pPacket is NULL\n");
        goto _exit;
    }

    pSkb = (struct sk_buff *)(pPacket->buf_id);
    if (NULL == pSkb)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "skb is NULL\n");
        PKTBUF_FREE(pPacket);
        goto _exit;
    }

    if (pPriv)
    {
        pPriv->ndev_stats.tx_packets++;
        pPriv->ndev_stats.tx_bytes += pSkb->len;
    }

    dev_kfree_skb_irq(pSkb);

    PKTBUF_FREE(pPacket);

_exit:
    netif_wake_queue(pNdev);
    return;
} /* end of rtnic_tx_callback */

/* Function Name:
 *      rtnic_open
 * Description:
 *      open function of the kernel module
 * Input:
 *      pNdev - net_device struct handled by the kernel
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 rtnic_open(struct net_device *pNdev)
{
    struct rtnic_priv *pPriv = netdev_priv(pNdev);
    uint32 unit;

    if (netif_msg_ifup(pPriv))
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "%s: enabling interface.\n", pNdev->name);
    }

    netif_start_queue(pNdev);

    HWP_UNIT_TRAVS_LOCAL(unit){
        if(HWP_NIC_SUPPORT(unit))
            drv_nic_rx_start(unit);
    }

    return RT_ERR_OK;
} /* end of rtnic_open */

/* Function Name:
 *      rtnic_close
 * Description:
 *      close function of the kernel module
 * Input:
 *      pNdev - net_device struct handled by the kernel
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 rtnic_close(struct net_device *pNdev)
{
    struct rtnic_priv *pPriv = netdev_priv(pNdev);
    uint32 unit;

    if (netif_msg_ifdown(pPriv))
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "%s: disabling interface.\n", pNdev->name);
    }

    HWP_UNIT_TRAVS_LOCAL(unit){
        if(HWP_NIC_SUPPORT(unit))
        drv_nic_rx_stop(unit);
    }
    netif_stop_queue(pNdev);

    return RT_ERR_OK;
} /* end of rtnic_close */

/* Function Name:
 *      rtnic_get_stats
 * Description:
 *      get the stats of the net device
 * Input:
 *      pNdev - net_device struct handled by the kernel
 * Output:
 *      None
 * Return:
 *      net_device_stats structure
 * Note:
 *      None
 */
static struct net_device_stats *rtnic_get_stats(struct net_device *pNdev)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29))
    struct rtnic_priv *pPriv = pNdev->priv;
#else
    struct rtnic_priv *pPriv = (struct rtnic_priv *)netdev_priv(pNdev);
#endif
    return (&pPriv->ndev_stats);
} /* end of rtnic_get_stats */

/* Function Name:
 *      rtnic_start_xmit
 * Description:
 *      Tx start function of the kernel module
 * Input:
 *      pSkb  - raw packet data
 *      pNdev - net_device struct handled by the kernel
 * Output:
 *      None
 * Return:
 *      NET_XMIT_SUCCESS
 *      NET_XMIT_DROP
 * Note:
 *      None
 */
static int32 rtnic_start_xmit(struct sk_buff *pSkb, struct net_device *pNdev)
{
    struct rtnic_priv *pPriv = netdev_priv(pNdev);
    drv_nic_pkt_t *pPacket;

    /* stopping send other packet */
    netif_stop_queue(pNdev);

    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (NULL == pPacket)
    {
        pPriv->ndev_stats.tx_errors++;
        dev_kfree_skb(pSkb);
        RT_LOG(LOG_DEBUG, MOD_NIC, "Out of memory\n");
        goto xmit_exit;
    }
    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));

    /* setting the CPU tx tag */
    pPacket->as_txtag = 0;

    /* raw packet */
    pPacket->buf_id = (void *)pSkb;
    pPacket->head = pSkb->head;
    pPacket->data = pSkb->data;
    pPacket->tail = pSkb->data + pSkb->len;
    pPacket->end = (uint8*)skb_end_pointer(pSkb);
    pPacket->length = pSkb->len;
    pPacket->next = NULL;

    if (RT_ERR_OK == drv_nic_pkt_tx(HWP_MY_UNIT_ID(), pPacket, rtnic_tx_callback, (void *)pNdev))
    {
        pNdev->trans_start = jiffies;
    }
    else
    {
        pPriv->ndev_stats.tx_dropped++;
        PKTBUF_FREE(pPacket);
        dev_kfree_skb(pSkb);
    }

xmit_exit:
    netif_start_queue(pNdev);

    return NETDEV_TX_OK;
} /* end of rtnic_start_xmit */

/* Function Name:
 *      rtnic_tx_timeout
 * Description:
 *      Tx timeout function of the kernel module
 * Input:
 *      pNdev - net_device struct handled by the kernel
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void rtnic_tx_timeout(struct net_device *pNdev)
{
    RT_LOG(LOG_DEBUG, MOD_NIC, "Tx Timeout!!! Can't send packet\n");
    return;
} /* end of rtnic_tx_timeout */

/* Function Name:
 *      rtnic_ioctl
 * Description:
 *      ioctl function of the kernel module
 * Input:
 *      pNdev - net_device struct handled by the kernel
 *      pRq   - interface request structure used for socket ioctl
 *      cmd   - ioctl command type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 rtnic_ioctl (struct net_device *pNdev, struct ifreq *pRq, int32 cmd)
{
    return RT_ERR_OK;
} /* end of rtnic_ioctl */

/* Function Name:
 *      rtnic_set_multicast_list
 * Description:
 *      set multicast list
 * Input:
 *      pNdev - net_device struct handled by the kernel
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void rtnic_set_multicast_list(struct net_device *pNdev)
{
    return;
} /* end of rtnic_set_multicast_list */

/* Function Name:
 *      rtnic_set_mac_address
 * Description:
 *      set mac address
 * Input:
 *      pNdev - net_device struct handled by the kernel
 *      ptr   - data buffer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 rtnic_set_mac_address(struct net_device *pNdev, void *ptr)
{
    struct rtnic_priv *pPriv = netdev_priv(pNdev);
    unsigned long flags;
    struct sockaddr *sa = ptr;
    uint32 i;
    uint32  unit;

    spin_lock_irqsave(&pPriv->lock, flags);

    for (i = 0; i < 6; ++i)
        pNdev->dev_addr[i] = sa->sa_data[i];

    spin_unlock_irqrestore(&pPriv->lock, flags);

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        rtk_switch_mgmtMacAddr_set(unit, (rtk_mac_t *)sa->sa_data);
    }

    return RT_ERR_OK;
} /* end of rtnic_set_mac_address */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29))
const struct net_device_ops rtnic_netdev_ops = {
    .ndo_open = rtnic_open,
    .ndo_stop = rtnic_close,
    .ndo_do_ioctl = rtnic_ioctl,
    .ndo_tx_timeout = rtnic_tx_timeout,
    .ndo_start_xmit = rtnic_start_xmit,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0))
    .ndo_set_multicast_list = rtnic_set_multicast_list,
#else
    .ndo_set_rx_mode = rtnic_set_multicast_list,
#endif
    .ndo_set_mac_address = rtnic_set_mac_address,
    .ndo_get_stats = rtnic_get_stats,
};
#endif


/* Function Name:
 *      rtnic_init
 * Description:
 *      Init rtnic driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      -ENOMEM
 *      -EIO
 * Note:
 *      None
 */
static int32 __init rtnic_init(void)
{
    struct net_device *pNdev;
    struct rtnic_priv *pPriv;
    drv_nic_initCfg_t initCfg;
    int32             ret;
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    rtk_l2ntfy_dst_t  now_dst;
#endif
    RT_INIT_REENTRY_CHK(rtnic_init_status);

    RT_INIT_MSG("RTNIC Driver Module Initialize\n");

    pNdev = alloc_etherdev(sizeof(struct rtnic_priv));
    if (NULL == pNdev)
    {
#if defined(__RTNIC_MODULE__)
        RT_INIT_MSG("FAIL\n");
#endif
        RT_INIT_MSG("#cause: Out of Memory!!\n");
        return -ENOMEM;
    }

    pPriv = netdev_priv(pNdev);
    osal_memset(pPriv, 0, sizeof(struct rtnic_priv));
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29))
    SET_MODULE_OWNER(pNdev);
    pPriv->pNdev = pNdev;
    pNdev->open = rtnic_open;
    pNdev->stop = rtnic_close;
    pNdev->do_ioctl = rtnic_ioctl;
    pNdev->tx_timeout = rtnic_tx_timeout;
    pNdev->hard_start_xmit = rtnic_start_xmit;
    pNdev->set_multicast_list = rtnic_set_multicast_list;
    pNdev->set_mac_address = rtnic_set_mac_address;
    pNdev->get_stats = rtnic_get_stats;
#else
    pPriv->pNdev = pNdev;
    pNdev->netdev_ops = &rtnic_netdev_ops;
#endif
    pNdev->watchdog_timeo = RTNIC_TX_TIMEOUT;

    pNdev->irq = rtk_dev[RTK_DEV_NIC].irq;
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19))
    rtk_switch_mgmtMacAddr_get(HWP_MY_UNIT_ID(), (rtk_mac_t *)&pNdev->dev_addr);
    //osal_memcpy(pNdev->dev_addr, mac, 6);
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
    rtk_switch_mgmtMacAddr_get(HWP_MY_UNIT_ID(), (rtk_mac_t *)&gNIC_ADDRESS);
    pNdev->dev_addr = (unsigned char *)&gNIC_ADDRESS;
    pNdev->rtnl_link_ops = NULL;
#endif
    spin_lock_init(&pPriv->lock);

    if (register_netdev(pNdev) != 0)
    {
#if defined(__RTNIC_MODULE__)
        RT_INIT_MSG("FAIL\n");
#endif
        RT_INIT_MSG("#cause: Couldn't Register The Device!!\n");
        goto out;
    }

    /* NIC Initialization */
    initCfg.pkt_alloc = rtnic_pkt_alloc;
    initCfg.pkt_free = rtnic_pkt_free;
    initCfg.pkt_size = RTNIC_MAX_PKTLEN;

//    HWP_UNIT_TRAVS_LOCAL(unit){
//        if(!HWP_NIC_SUPPORT(unit))
//            continue;

        if (RT_ERR_OK != (ret = drv_nic_init(HWP_MY_UNIT_ID(), &initCfg)))
        {
          #if defined(__RTNIC_MODULE__)
            RT_INIT_MSG("FAIL(unit=%d)\n",HWP_MY_UNIT_ID());
          #endif
            RT_INIT_MSG("#cause: drv_nic_init(%d) failed!! %d\n", HWP_MY_UNIT_ID(),ret);
            goto out;
        }
//    }

    /* Register NIC Rx Handler */
    if (RT_ERR_OK != drv_nic_rx_register(HWP_MY_UNIT_ID(), NIC_RX_CB_PRIORITY_MAX, rtnic_rx_callback, (void *)pNdev, 0))
    {
      #if defined(__RTNIC_MODULE__)
        RT_INIT_MSG("FAIL\n");
      #endif
        RT_INIT_MSG("#cause: NIC Rx Handler Registration Failed!!\n");
        goto out;
    }

#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    drv_l2ntfy_dst_get(HWP_MY_UNIT_ID(), &now_dst);
    if((L2NTFY_DST_PKT_TO_LOCAL == now_dst)
        || (L2NTFY_DST_PKT_TO_MASTER == now_dst))
    {
        /* Register L2 notification Rx Handler */
        if (RT_ERR_OK != drv_nic_rx_register(HWP_MY_UNIT_ID(), L2NTFY_RX_HANDLER_PRIORITY, drv_l2ntfy_pkt_handler, NULL, 0))
        {
#if defined(__RTNIC_MODULE__)
          RT_INIT_MSG("FAIL\n");
#endif
          RT_INIT_MSG("#cause: L2 notification Handler Registration Failed!!\n");
          goto out;
        }
    }
#endif

    rtnic_init_status = INIT_COMPLETED;

    return RT_ERR_OK;

out:
    if (NULL != pNdev)
    {
        unregister_netdev(pNdev);
        free_netdev(pNdev);
    }

    RT_INIT_MSG("Init Switch Ethernet Driver....FAIL\n");


    return -EIO;
} /* end of rtnic_init */

/* Function Name:
 *      rtnic_exit
 * Description:
 *      Exit rtnic driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void __exit rtnic_exit(void)
{

    RT_INIT_MSG("Exit RTNIC Driver Module....OK\n");
    return;
} /* end of rtnic_exit */

module_init(rtnic_init);
module_exit(rtnic_exit);

MODULE_DESCRIPTION ("Switch SDK Ethernet Driver Module");
MODULE_LICENSE("GPL");

