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

#ifndef __DAL_OAM_TEST_CASE_H__
#define __DAL_OAM_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_oam_autoDyingGaspEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_dyingGaspWaitTime_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_loopbackMacSwapEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_portLoopbackMuxAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmPcp_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmCfi_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmTpid_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstLifetime_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmMepid_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmIntervalField_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmMdl_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstTagStatus_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstVid_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstMaid_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstTxStatus_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstInterval_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmRxInstVid_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmTxInstPort_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmRxInstPort_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmPortEthDmEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_cfmCcmInstAliveTime_test(uint32 caseNo, uint32 unit);
extern int32 dal_oam_dyingGaspSend_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_OAM_TEST_CASE_H__*/

