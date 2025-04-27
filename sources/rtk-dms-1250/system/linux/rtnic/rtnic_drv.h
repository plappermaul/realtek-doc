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

#ifndef __RTNIC_DRV_H__
#define __RTNIC_DRV_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */
#define RTNIC_UNIT_ID               (0)
#define RTNIC_TX_TIMEOUT            (10*HZ)
#define RTNIC_PKTLEN_RSVD           (2)     /* Reserve 2 bytes for IP alignment */
#define RTNIC_HANDLER_QUEUE_LENGTH  (300)
#define RTNIC_HANDLER_BURST_MAX     (160)
#define RTNIC_MAX_PKTLEN            (1600)

/* Thread priority */
#define RT_THREAD_PRI_DFLT          (66)  /* POSIX nice -10 */
#define RT_THREAD_PRI_HIGH          (35)  /* POSIX nice -15 */
#define RT_THREAD_PRI_LOW           (100) /* POSIX nice -5 */

typedef enum rtnic_act_e
{
    RTNIC_HANDLER_ACT_NONE = 0,
    RTNIC_HANDLER_ACT_TRAP,
    RTNIC_HANDLER_ACT_COPY,
    RTNIC_HANDLER_ACT_DROP,
    RTNIC_HANDLER_ACT_END
} rtnic_act_t;

typedef enum rtnic_handler_pri_e
{
    /* HIGH priority */
    RTNIC_HANDLER_PRI_HIGH = 0,
    RTNIC_HANDLER_PRI_ACL,
    RTNIC_HANDLER_PRI_PORT_SECURITY,
    RTNIC_HANDLER_PRI_STP,
    RTNIC_HANDLER_PRI_RLDP,
    RTNIC_HANDLER_PRI_LACP,
    RTNIC_HANDLER_PRI_UDLD,
    RTNIC_HANDLER_PRI_DAI,
    RTNIC_HANDLER_PRI_DHCP_CLIENT,
    RTNIC_HANDLER_PRI_DHCP_SERVER,
    RTNIC_HANDLER_PRI_VLAN_TXLATE,

    /* DFLT priority */
    RTNIC_HANDLER_PRI_DFLT,
    RTNIC_HANDLER_PRI_IGMP,
    RTNIC_HANDLER_PRI_MLD,
    RTNIC_HANDLER_PRI_GVRP,
    RTNIC_HANDLER_PRI_ERPS,
    RTNIC_HANDLER_PRI_VOICE_VLAN_OUI,

    /* LOW priority */
    RTNIC_HANDLER_PRI_LOW,
    RTNIC_HANDLER_PRI_LLDP,
    RTNIC_HANDLER_PRI_DOT1X_EAPOL,
    RTNIC_HANDLER_PRI_DOT1X_RADIUS,
    RTNIC_HANDLER_PRI_DOT1X_ARP,
    RTNIC_HANDLER_PRI_SOFTWARE_L2_LEARN,
    RTNIC_HANDLER_PRI_WEBAUTH,
    RTNIC_HANDLER_PRI_CDP,

    RTNIC_HANDLER_PRI_END
} rtnic_handler_pri_t;

/* For handler */
typedef int32 (*rtnic_handler_rx_cb_f)(drv_nic_pkt_t *pPkt, void *pCookie);
typedef rtnic_act_t (*rtnic_handler_pre_chk_cb_f)(drv_nic_pkt_t *pPkt, void *pCookie);

/*
 * Data Declaration
 */
typedef struct rtnic_handler_s
{
    uint8       priority;
    uint8       noCondition;
    uint8       dmac[6];
    uint8       dmac_caremask[6];
    uint8       smac[6];
    uint8       smac_caremask[6];
    uint16      ethertype;
    uint8       proto;
    uint16      dport;
    uint16      sport;

    rtnic_act_t   action;
    rtnic_handler_rx_cb_f rx_cbf;
    rtnic_handler_pre_chk_cb_f chk_cbf;
    uint8       rx_careMask;
    char        *pName;
    void        *pCookie;
} rtnic_handler_t;

typedef struct rtnic_handler_list_s
{
    rtnic_handler_t handler;
    drv_nic_pkt_t *pPkt[RTNIC_HANDLER_QUEUE_LENGTH];
    uint16        enQueueCnt;
    uint16        deQueueCnt;
    uint32        rxThreadId;
    volatile int32 status;
    struct rtnic_handler_list_s *next;
} rtnic_handler_list_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
uint32 rtnic_pkt_handler(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);

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
int32 rtnic_handler_register(rtnic_handler_t *pHandler);

#endif  /*__RTNIC_DRV_H__*/

