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
 * Feature : Provide the APIs of CPU tag
 *
 */

#ifndef __RTK_CPU_TAG_H__
#define __RTK_CPU_TAG_H__

/*
 * Include Files
 */



#define TYPE_REALTEK				 0x8899
#define REALTEK_PROTO                0x4

/* CPU tag length */
#define RTK_CPU_TAG_LEN_NORMAL       4
#define RTK_CPU_TAG_LEN_APOLLO       8
#define RTK_CPU_TAG_LEN_PTP          10
#define RTK_CPU_TAG_LEN_TX           12
#define RTK_CPU_TAG_LEN_MAX          12

typedef union rtk_cpu_tag_u
{
    struct
    {
        uint16  ether_type;
        uint8   protocol;
        uint8   reason;             /* Reason to CPU */

        uint8   priority:3;         /* internal priority */
        uint8   ttl_pmask:5;        /* TTL-1 extension Pmask */

        uint8   l3r:1;              /* L3R */
        uint8   org:1;              /* ORG */
        uint8   rsv1:3;             /* reserved */
        uint8   spa:3;              /* SPA */

        uint16  rsv2:2;             /* reserved */
        uint16  pctrl:1;            /* P Ctrl */
        uint16  sid:7;              /* Pon stream ID */
        uint16  ext_pmask:6;        /* extension Pmask */
    } rx_pon_tag;
#if 0
    struct
    {
        uint16  ether_type;
        uint8   protocol;
        uint8   reason;             /* Reason to CPU */

        uint8   priority:3;         /* Assigned priority */
        uint8   rsv1:5;             /* reserved */

        uint8   rsv2:5;             /* reserved */
        uint8   spa:3;              /* SPA */

        uint16  timestamp_sec;      /* timestamp seconds */
        uint16  timestamp_nsec;     /* timestamp nano-seconds */
    } rx_ptp_tag;
#endif
    struct
    {
        uint16  ether_type;
        uint8   protocol;

        uint8   l3cs:1;             /* L3CS */
        uint8   l4cs:1;             /* L4CS */
        uint8   tx_pmask:6;         /* Tx portmask or VID */

        uint8   efid:1;             /* EFID select */
        uint8   enhan_fid:3;        /* EFID */
        uint8   prio_sel:1;         /* Priority select */
        uint8   prio:3;             /* Priority */

        uint8   keep:1;             /* Keep */
        uint8   vsel:1;             /* VID select */
        uint8   dis_lrn:1;          /* disable learning */
        uint8   rsv1:5;             /* reserved */

        uint8   rsv2;               /* reserved */

        uint8   l2br:1;             /* L2 bridge */
        uint8   sid:7;              /* PON stream ID */

        uint16  rsv3;               /* reserved */

        uint16  rsv4;               /* reserved */
    } tx_tag;

    struct
    {
        uint16  ether_type;
        uint8   protocol;
        uint8   spa;                /* SPA */
    } normal_tag;

}rtk_cpu_tag_t;


extern void rtk_cpu_tag_retrieve(struct sk_buff *skb);
extern void rtk_cpu_tag_insert(struct sk_buff *skb);

#endif  /* __RTK_CPU_TAG_H__ */

