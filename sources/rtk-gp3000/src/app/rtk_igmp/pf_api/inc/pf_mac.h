/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:
 * $Date:
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __PF_MAC_H__
#define __PF_MAC_H__


#include <sys_def.h>

#include <rtk/switch.h>
#include <rtk/l2.h>
#include <rtk/trap.h>
#include <rtk/gpon.h>


#define GPON_DEV_MAX_MACTBL_NUM 256

typedef struct vlan_fid_s
{
    int32 bValid;
    uint32 uiVid;
    uint32 uiFid;
    rtk_portmask_t stPhyMask;
    rtk_portmask_t stPhyMaskUntag;
}vlan_fid_t;

typedef struct
{
    mac_address_t   mac_addr;
    uint16          tdVid;
    rtk_portmask_t  port_mask;
    uint8           care_vid;
}mac_mcast_t;

typedef enum rsv_mac_action_e
{
    PASS_TYPE_CPU_ONLY = 0,
    PASS_TYPE_ALLPORTS,
    PASS_TYPE_CPU_EXCLUDE,
    PASS_TYPE_DROP,
    PASS_TYPE_END
} rsv_mac_pass_action_t;

typedef enum mcast_unknow_act_e
{
    MCAST_ACT_FORWARD = 0,
    MCAST_ACT_DROP,
    MCAST_ACT_TRAP2CPU,
    MCAST_ACT_ROUTER_PORT,
    MCAST_ACT_END
} mcast_unknow_act_t;

typedef enum mcast_dir_port_e
{
    MCAST_DIR_PORT_SPECLAN  = (1 << 0),
    MCAST_DIR_PORT_UPLINK   = (1 << 1),
    MCAST_DIR_PORT_ALLLAN   = (1 << 2),
    MCAST_DIR_PORT_END      = (MCAST_DIR_PORT_UPLINK | MCAST_DIR_PORT_ALLLAN)
} mcast_dir_port_t;

typedef struct
{
    mcast_dir_port_t    dir;
    rtk_portmask_t      port_mask;
} mcast_dir_port_info_t;

typedef struct
{
	uint32 	vid;
    uint32  dip;
    uint32  sip;
    rtk_portmask_t port_mask;
	rtk_portmask_t filter_mode_mask; /* include 0, exclude 1*/
	int32 isGrpRefB;
    uint8           care_vid;
}ip_mcast_t;


#define FID_INVALID_ID     0xFFFF

extern void pf_flush_mc_vlan_mac_by_vid(int32 vid);

extern int32 pf_mc_vlan_mem_get(uint32 uiVid, rtk_portmask_t *pstPhyMask, rtk_portmask_t *pstPhyMaskUntag);

extern int32 pf_mac_mcast_set_by_mac(mac_mcast_t stMacMcast);

extern int32 pf_mac_mcast_del_by_mac(mac_mcast_t stMacMcast);

extern int32 pf_mac_mcast_get_by_mac(mac_mcast_t stMacMcast, mac_mcast_t *pStMacMcast);

extern int32 pf_unknow_mcast_action_set(mcast_dir_port_info_t dir_port_info, uint32 type, mcast_unknow_act_t enActMode);

extern int32 pf_igmp_packet_action_set(uint32 ulAction, uint32 port);

extern int32 pf_lookupmiss_flood_portmask_set(uint32 type, rtk_portmask_t *pFlood_portmask);

extern int32 pf_lookupmiss_flood_portmask_get(uint32 type, rtk_portmask_t *pFlood_portmask);

extern int32 pf_ipMcastMode_set(uint32 mode);

extern int32 pf_ipMcGroup_get(ip_mcast_t stIpMcast);

extern int32 pf_ipMcastEntry_add(ip_mcast_t stIpMcast);

extern int32 pf_ipMcastEntry_del(ip_mcast_t stIpMcast);

extern int32 pf_ipMcastEntry_get(ip_mcast_t *pStIpMcast);

extern int32 pf_ipMcastEntry_set(ip_mcast_t stIpMcast);

extern int32 pf_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);

/* GPON MAC */
extern int32 pf_gmac_filter_mode_set(uint32 mode);

extern int32 pf_gpon_macEntry_del(mac_address_t mac);

extern int32 pf_gpon_macEntry_add(mac_address_t mac);

extern int32 pf_gpon_macEntry_get(mac_address_t mac);

extern int32 mcast_ipv4_to_mcast_mac(ipaddr_t ip, mac_address_t mac);
#endif /* __PF_MAC_H__ */
