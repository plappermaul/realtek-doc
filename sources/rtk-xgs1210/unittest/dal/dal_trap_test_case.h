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

#ifndef __DAL_TRAP_TEST_CASE_H__
#define __DAL_TRAP_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_trap_rmaAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_userDefineRma_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_userDefineRmaAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_mgmtFrameAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_mgmtFramePri_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_portMgmtFrameAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_cfmFrameTrapPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_rmaGroupAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_rmaLearningEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_rmaGroupLearningEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_bypassStp_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_bypassVlan_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_userDefineRmaEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_userDefineRmaLearningEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_mgmtFrameLearningEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_pktWithCFIAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_pktWithOuterCFIAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_pktWithCFIPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_oamPDUAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_oamPDUPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_cfmUnknownFrameAct_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_cfmLoopbackLinkTraceAct_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_cfmCcmAct_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_cfmEthDmAct_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_portOamLoopbackParAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_routeExceptionAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_routeExceptionPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_trap_mgmtFrameMgmtVlanEnable_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_TRAP_TEST_CASE_H__*/

