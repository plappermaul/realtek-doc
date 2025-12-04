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
 * Purpose : PORT SDK test case
 *
 * Feature : Provide the APIs to access PORT
 *
 */

#ifndef __DAL_L2_TEST_CASE_H__
#define __DAL_L2_TEST_CASE_H__

extern int32 dal_l2_age_test(uint32 caseNo);
extern int32 dal_l2_extPortEgrFilterMask_test(uint32 caseNo);
extern int32 dal_l2_flushLinkDownPortAddrEnable_test(uint32 caseNo);
extern int32 dal_l2_illegalPortMoveAction_test(uint32 caseNo);
extern int32 dal_l2_portAgingEnable_test(uint32 caseNo);
extern int32 dal_l2_ipmcGroupLookupMissHash_test(uint32 caseNo);
extern int32 dal_l2_ipmcMode_test(uint32 caseNo);
extern int32 dal_l2_limitLearningCnt_test(uint32 caseNo);
extern int32 dal_l2_limitLearningCntAction_test(uint32 caseNo);
extern int32 dal_l2_srcPortEgrFilterMask_test(uint32 caseNo);
extern int32 dal_l2_newMacOp_test(uint32 caseNo);
extern int32 dal_l2_lookupMissAction_test(uint32 caseNo);
extern int32 dal_l2_lookupMissFloodPortMask_test(uint32 caseNo);
extern int32 dal_l2_addr_test(uint32 caseNo);
extern int32 dal_l2_mcastAddr_test(uint32 caseNo);
extern int32 dal_l2_ipMcastAddr_test(uint32 caseNo);
extern int32 dal_l2_ipmcGroup_test(uint32 caseNo);
extern int32 dal_l2_portLimitLearningCnt_test(uint32 caseNo);
extern int32 dal_l2_portLimitLearningCntAction_test(uint32 caseNo);
extern int32 dal_l2_portLimitLearningOverStatus_test(uint32 caseNo);
extern int32 dal_l2_limitLearningOverStatus_test(uint32 caseNo);
extern int32 dal_l2_portLookupMissAction_test(uint32 caseNo);
extern int32 dal_l2_portIpmcAction_test(uint32 caseNo);

#endif /* __DAL_L2_TEST_CASE_H__ */
