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

#ifndef __DAL_RATE_TEST_CASE_H__
#define __DAL_RATE_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_rate_portEgrBwCtrlEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portEgrBwCtrlRate_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portEgrQueueBwCtrlEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portEgrQueueBwCtrlRate_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portIgrBwCtrlEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portIgrBwCtrlRate_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portIgrBwFlowctrlEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_igrBandwidthCtrlIncludeIfg_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_egrBandwidthCtrlIncludeIfg_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portStormCtrlBurstSize_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portStormCtrlExceed_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_stormControlIncludeIfg_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portStormCtrlRate_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_igrBandwidthCtrlBypass_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_igrBandwidthLowThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portIgrBandwidthHighThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portStormCtrlProtoRate_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_stormControlProtoExceed_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_stormControlBypass_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portStormCtrlTypeSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portIgrBwCtrlRate_10G_test(uint32 caseNo, uint32 unit);
extern int32 dal_rate_portIgrBwCtrlRate_10G_test(uint32 caseNo, uint32 unit);


#endif/*__DAL_SEC_TEST_CASE_H__*/

