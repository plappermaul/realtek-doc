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
 * Purpose : VLAN SDK test case
 *
 * Feature : Provide the APIs to access VLAN
 *
 */

#ifndef __DAL_VLAN_TEST_CASE_H__
#define __DAL_VLAN_TEST_CASE_H__


extern int32 dal_vlan_test(uint32 caseNo);
extern int32 dal_vlan_Fid_test(uint32 caseNo);
extern int32 dal_vlan_portFid_test(uint32 caseNo);
extern int32 dal_vlan_portPriority_test(uint32 caseNo);
extern int32 dal_vlan_accpetFrameType_test(uint32 caseNo);
extern int32 dal_vlan_portEgrTagKeepType_test(uint32 caseNo);
extern int32 dal_vlan_portIgrFilterEnable_test(uint32 caseNo);
extern int32 dal_vlan_misc_test(uint32 caseNo);
extern int32 dal_vlan_leaky_test(uint32 caseNo);
extern int32 dal_vlan_portProtoVlan_test(uint32 caseNo);
extern int32 dal_vlan_priority_test(uint32 caseNo);
extern int32 dal_vlan_priorityEnable_test(uint32 caseNo);
extern int32 dal_vlan_protoGroup_test(uint32 caseNo);
extern int32 dal_vlan_reservedVidAction_test(uint32 caseNo);
extern int32 dal_vlan_stg_test(uint32 caseNo);
extern int32 dal_vlan_tagMode_test(uint32 caseNo);
extern int32 dal_vlan_transparent_test(uint32 caseNo);
extern int32 dal_vlan_vlanFunc_test(uint32 caseNo);
extern int32 dal_vlan_portLeaky_test(uint32 caseNo);
extern int32 dal_vlan_keepType_test(uint32 caseNo);

#endif /* __DAL_VLAN_TEST_CASE_H__ */
