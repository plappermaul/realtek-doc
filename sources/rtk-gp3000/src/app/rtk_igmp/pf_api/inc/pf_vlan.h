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

#ifndef __PF_VLAN_H__
#define __PF_VLAN_H__

#include <sys_def.h>

#include <rtk/vlan.h>
#include <rtk/svlan.h>

typedef enum sys_vlan_event_type_e
{
    VLAN_EVENT_CREATE,
    VLAN_EVENT_DESTROY,
} sys_vlan_event_type_t;

/* For vlan event notifier */
typedef struct sys_vlan_event_s
{
    uint16 vid;
    sys_vlan_event_type_t eventType;
} sys_vlan_event_t;

extern int32 pf_mc_translation_vlan_member_add
    (uint32 usrvlan, uint32 mvlan, rtk_portmask_t stPhyMask, rtk_portmask_t stPhyMaskUntag, rtk_portmask_t stPhyMasksvlanUntag);

extern int32 pf_mc_translation_vlan_member_remove(uint32 usrvlan,uint32 mvlan, rtk_portmask_t stPhyMask);

extern int32 pf_mc_vlan_member_add(uint32 ulVlanId, rtk_portmask_t stPhyMask, rtk_portmask_t stPhyMaskUntag);

extern int32 pf_mc_vlan_member_remove(uint32 ulVlanId, rtk_portmask_t stPhyMask);

extern int32 pf_port_c2s_entry_add(uint32 uiLPort, uint32 ulCvid, uint32 ulSvid);

extern int32 pf_port_c2s_entry_delete(uint32 uiLPort, uint32 ulCvid, uint32 ulSvid);

extern int32 pf_port_sp2c_entry_add(uint32 uiLPort, uint32 ulSvid, uint32 ulCvid);

extern int32 pf_port_sp2c_entry_delete(uint32 uiLPort, uint32 ulSvid);

extern int32 pf_vlan_entry_create(uint32 ulVlanId);

extern int32 pf_vlan_entry_delete(uint32 ulVlanId);

#endif /* __PF_VLAN_H__ */
