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

#ifndef __DAL_STAT_TEST_CASE_H__
#define __DAL_STAT_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_stat_readPerformance_test(uint32 caseNo, uint32 unit);
extern int32 dal_stat_readPerformance2_test(uint32 caseNo, uint32 unit);
extern int32 dal_stat_tagLenCntIncEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_stat_enable_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_STAT_TEST_CASE_H__*/

