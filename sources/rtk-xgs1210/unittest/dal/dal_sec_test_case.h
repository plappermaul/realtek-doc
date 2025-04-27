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

#ifndef __DAL_SEC_TEST_CASE_H__
#define __DAL_SEC_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_sec_portAttackPrevent_test(uint32 caseNo, uint32 unit);
extern int32 dal_sec_portAttackPreventEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_sec_attackPreventAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_sec_minIPv6FragLen_test(uint32 caseNo, uint32 unit);
extern int32 dal_sec_maxPingLen_test(uint32 caseNo, uint32 unit);
extern int32 dal_sec_minTCPHdrLen_test(uint32 caseNo, uint32 unit);
extern int32 dal_sec_smurfNetmaskLen_test(uint32 caseNo, uint32 unit);


#endif/*__DAL_SEC_TEST_CASE_H__*/


