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

#ifndef __DAL_L2_TEST_CASE_H__
#define __DAL_L2_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_l2_flushLinkDownPortAddrEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_portLimitLearningCnt_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_portLimitLearningCntAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_fidLimitLearningEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_aging_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_hashAlgo_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_vlanMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_portNewMacOp_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_addr_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_addr_del_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_addr_set_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_addr_getNext_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastBlockPortmask_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipmcMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_illegalPortMoveAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_legalPortMoveAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_lookupMissFloodPortMask_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_lookupMissFloodPortMask_add_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_srcPortEgrFilterMask_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_srcPortEgrFilterMask_add_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_exceptionAddrAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_del_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_set_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_getNext_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_del_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_set_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_getNext_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_addr_test1(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_test1(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_test1(uint32 caseNo, uint32 unit);
extern int32 dal_l2_limitLearningCnt_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_limitLearningCntAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_fidLearningCntAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_portAgingEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_zeroSALearningEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_staticPortMoveAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_legalPortMoveFlushAddrEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastFwdPortmask_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_cpuMacAddr_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddrChkEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcstFidVidCompareEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_addByIndex_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_addByIndex_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6mcMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6CareByte_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_del_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_set_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_addByIndex_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_getNext_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_portLookupMissAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastFwdIndex_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_addr_all_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_mcastAddr_all_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ipMcastAddr_all_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_DIPSIP_all_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_ip6McastAddr_DIPFVID_all_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_secureMacMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_portDynamicPortMoveForbidEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_l2_dynamicPortMoveForbidAction_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_L2_TEST_CASE_H__*/

