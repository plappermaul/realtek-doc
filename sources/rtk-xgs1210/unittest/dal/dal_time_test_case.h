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

#ifndef __DAL_TIME_TEST_CASE_H__
#define __DAL_TIME_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_time_refTime_test(uint32 caseNo, uint32 unit);
extern int32 dal_time_refTimeAdjust_test(uint32 caseNo, uint32 unit);
extern int32 dal_time_portPtpEnable_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_TIME_TEST_CASE_H__*/
