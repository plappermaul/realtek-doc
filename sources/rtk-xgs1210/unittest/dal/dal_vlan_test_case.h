/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of DAL test APIs in the SDK
 *
 * Feature : DAL test APIs
 *
 */
#ifndef __DAL_VLAN_TEST_CASE_H__
#define __DAL_VLAN_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_vlan_table_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_stg_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_port_modify_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portInnerAcceptFrameType_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrExtraTagEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrFilterEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrInnerPriSource_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrInnerTpid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrOuterTpid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrOuterTpidMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrTagKeepEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrExtraTagEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrExtraTpid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_mcastLeakyEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrInnerTpid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrOuterTpid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrTagKeepEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portOuterAcceptFrameType_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portOuterProtoVlan_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portOuterPvid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portProtoVlan_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portInnerPvid_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portTpidEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_protoGroup_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_l2UcastLookupMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_l2McastLookupMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_profileIdx_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_profile_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrFilter_test(uint32 caseNo, uint32 unit);
#if 0
extern int32 dal_vlan_mcastLeakyPortEnable_test(uint32 caseNo, uint32 unit);
#endif
extern int32 dal_vlan_portInnerPvidMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portOuterPvidMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_innerTpidEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_outerTpidEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_extraTpidEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrInnerTagSts_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrOuterTagSts_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_igrVlanCnvtBlkMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_egrVlanCnvtDblTagEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_egrVlanCnvtVidSource_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portVlanAggrEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_leakyStpFilter_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_except_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrCnvtDfltAct_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portIgrTagKeepType_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrTagKeepType_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portVlanAggrVidSource_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portVlanAggrPriTagVidSource_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrVlanCnvtVidSource_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrVlanCnvtVidTarget_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_portEgrVlanCnvtLookupMissAct_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_macBasedVlan_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_igrVlanCnvtEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_egrVlanCnvtEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_igrVlanCnvtEntry_delAll_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_egrVlanCnvtEntry_delAll_test(uint32 caseNo, uint32 unit);
extern int32 dal_vlan_egrVlanCnvtRangeCheckVid_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_VLAN_TEST_CASE_H__*/

