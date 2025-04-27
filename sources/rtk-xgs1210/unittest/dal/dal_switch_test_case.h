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

#ifndef __DAL_SWITCH_TEST_CASE_H__
#define __DAL_SWITCH_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_switch_chksumFailAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_recalcCRCEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_mgmtMacAddr_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_IPv4Addr_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_maxPktLenLinkSpeed_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_maxPktLenTagLenCntIncEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_snapMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_cpuMaxPktLen_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_pppoePassthrough_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_deviceInfo_test(uint32 caseNo, uint32 unit);
extern int32 dal_switch_pkt2CpuTypeFormat_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_SWITCH_TEST_CASE_H__*/

