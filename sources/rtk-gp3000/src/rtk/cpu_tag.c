/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : CPU tag API
 *
 * Feature : Provide the APIs to handle CPU tag
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/cpu_tag.h>
#include <osal/print.h>
#include <dal/apollo/raw/apollo_raw_cpu.h>
#include <dal/apollo/gpon/gpon_platform.h>



/*
 * Symbol Definition
 */
#define ETHER_MAC_LEN                   12
#define ETHER_TYPE_OFFSET               12

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

void rtk_cpu_tag_retrieve(struct sk_buff *skb)
{
#if 0
    rtk_cpu_tag_t *cpu_tag;
    int32 tag_len=0;
    uint8 hwaddr[ETHER_MAC_LEN];
    rtk_cpu_tag_fmt_t format=CPU_TAG_FMT_END;
    int32 idx;

    osal_memset(&skb->rx_tag, 0, sizeof(apollo_rx_tag_t));

    cpu_tag = (rtk_cpu_tag_t *)&skb->data[ETHER_TYPE_OFFSET];

    if(TYPE_REALTEK == cpu_tag->rx_pon_tag.ether_type)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"retrive cpu tag");
        apollo_raw_cpu_tag_format_get(&format);

        if (CPU_TAG_FMT_NORMAL == format)
        {
            tag_len = RTK_CPU_TAG_LEN_NORMAL;
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"cpu tag len %d",tag_len);

            skb->rx_tag.cpu_tag = 2;
            skb->rx_tag.src_port = cpu_tag->normal_tag.spa;

            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"cpu tag: ");
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," spa %d",cpu_tag->normal_tag.spa);
        }
        else if(CPU_TAG_FMT_APOLLO == format)
        {
#if 0
            if((APOLLO_SPA == PTP_ENABLE_PORT) && (APOLLO_REASON == PTP))
                tag_len = RTK_CPU_TAG_LEN_PTP;
            else
#endif
                tag_len =RTK_CPU_TAG_LEN_APOLLO;
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"cpu tag len %d",tag_len);

            skb->rx_tag.cpu_tag = 1;
            skb->rx_tag.pkt_type = 0;
            skb->rx_tag.orig_format = cpu_tag->rx_pon_tag.org;
            skb->rx_tag.p_ctrl = cpu_tag->rx_pon_tag.pctrl;
            skb->rx_tag.pon_sid = cpu_tag->rx_pon_tag.sid;
            skb->rx_tag.src_port = cpu_tag->rx_pon_tag.spa;
            skb->rx_tag.reason = cpu_tag->rx_pon_tag.reason;
            skb->rx_tag.inter_prio = cpu_tag->rx_pon_tag.priority;

            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"cpu tag: ");
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," reason %d",cpu_tag->rx_pon_tag.reason);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," priority %d",cpu_tag->rx_pon_tag.priority);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," ttl_pmask %d",cpu_tag->rx_pon_tag.ttl_pmask);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," l3r %d",cpu_tag->rx_pon_tag.l3r);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," org %d",cpu_tag->rx_pon_tag.org);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," spa %d",cpu_tag->rx_pon_tag.spa);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," pctrl %d",cpu_tag->rx_pon_tag.pctrl);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," sid %d",cpu_tag->rx_pon_tag.sid);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," ext_pmask %d\n\r",cpu_tag->rx_pon_tag.ext_pmask);
        }

        for(idx = 0 ; idx<ETHER_MAC_LEN ; idx++)
        {
            hwaddr[idx] = skb->data[idx];
        }

        skb_pull(skb,tag_len);

        for(idx = 0 ; idx < ETHER_MAC_LEN ; idx++)
        {
            skb->data[idx] = hwaddr[idx]  ;
        }

        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"skb data:\n\r");
#if 0
        for(idx =0; idx < skb->len; idx+=4)
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x-%02x-%02x-%02x",
                        skb->data[idx], skb->data[idx+1], skb->data[idx+2], skb->data[idx+3]);

        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"\n\r");
#else
        for(idx =0; idx < skb->len; idx++)
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," %02x",
                        skb->data[idx]);
#endif
    }
#endif
}

void rtk_cpu_tag_insert(struct sk_buff *skb)
{
#if 0
    rtk_cpu_tag_t cpu_tag;
    rtk_cpu_tag_fmt_t format=CPU_TAG_FMT_END;
    uint8 buf[ETHER_MAC_LEN+RTK_CPU_TAG_LEN_MAX];
    int32 idx;
    uint32 tag_len;

    if(skb->tx_tag.cpu_tag != 0)
    {
        osal_memset(&cpu_tag, 0, sizeof(rtk_cpu_tag_t));
        apollo_raw_cpu_tag_format_get(&format);

        if (CPU_TAG_FMT_NORMAL == format)
        {
            tag_len = RTK_CPU_TAG_LEN_NORMAL;
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"insert cpu tag:");
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," tx_port_mask: 0x%x",skb->tx_tag.tx_port_mask);

            cpu_tag.normal_tag.ether_type = TYPE_REALTEK;
            cpu_tag.normal_tag.protocol = 0x4;
            cpu_tag.normal_tag.spa = (skb->tx_tag.tx_port_mask & 0xFF);
        }
        else if(CPU_TAG_FMT_APOLLO == format)
        {
            tag_len = RTK_CPU_TAG_LEN_TX;
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"insert cpu tag:");
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," keep: %d",skb->tx_tag.keep);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," v_sel: %d",skb->tx_tag.v_sel);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," dis_lrn: %d",skb->tx_tag.dis_lrn);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," cpu_l3cs: %d",skb->tx_tag.cpu_l3cs);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," cpu_l4cs: %d",skb->tx_tag.cpu_l4cs);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," as_pri: %d",skb->tx_tag.as_pri);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," cpu_pri: %d",skb->tx_tag.cpu_pri);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," efid: %d",skb->tx_tag.efid);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," ehan_fid: %d",skb->tx_tag.ehan_fid);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," tx_port_mask: 0x%x",skb->tx_tag.tx_port_mask);
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI," tx_pon_sid: %d\n\r",skb->tx_tag.tx_pon_sid);

            cpu_tag.tx_tag.ether_type = TYPE_REALTEK;
            cpu_tag.tx_tag.protocol = 0x4;
            cpu_tag.tx_tag.l3cs = (skb->tx_tag.cpu_l3cs & 0x1);
            cpu_tag.tx_tag.l4cs = (skb->tx_tag.cpu_l4cs & 0x1);
            cpu_tag.tx_tag.tx_pmask = (skb->tx_tag.tx_port_mask & 0x3F);
            cpu_tag.tx_tag.efid = (skb->tx_tag.efid & 0x1);
            cpu_tag.tx_tag.enhan_fid = (skb->tx_tag.ehan_fid & 0x7);
            cpu_tag.tx_tag.prio_sel = (skb->tx_tag.as_pri & 0x1);
            cpu_tag.tx_tag.prio = (skb->tx_tag.cpu_pri & 0x7);
            cpu_tag.tx_tag.keep = (skb->tx_tag.keep & 0x1);
            cpu_tag.tx_tag.vsel = (skb->tx_tag.v_sel & 0x1);
            cpu_tag.tx_tag.dis_lrn = (skb->tx_tag.dis_lrn & 0x1);
            cpu_tag.tx_tag.l2br = 0;
            cpu_tag.tx_tag.sid = (skb->tx_tag.tx_pon_sid & 0x7F);
        }

        osal_memcpy(buf, skb->data, ETHER_MAC_LEN);

        skb_push(skb, tag_len);
        osal_memcpy(skb->data, buf, ETHER_MAC_LEN);
        osal_memcpy((skb->data + ETHER_MAC_LEN), &cpu_tag, tag_len);

        GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"skb buf:");
        for(idx =0; idx < skb->len; idx+=4)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_OMCI,"%02x-%02x-%02x-%02x",
                        skb->data[idx], skb->data[idx+1], skb->data[idx+2], skb->data[idx+3]);
        }
    }
#endif
}

