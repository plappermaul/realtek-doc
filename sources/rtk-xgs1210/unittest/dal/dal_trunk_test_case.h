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

#ifndef __DAL_TRUNK_TEST_CASE_H__
#define __DAL_TRUNK_TEST_CASE_H__

#include <common/rt_type.h>

int32 dal_trunk_port_test(uint32 caseNo, uint32 unit);
int32 dal_trunk_distributionAlgorithmParam_test(uint32 caseNo, uint32 unit);
int32 dal_trunk_distributionAlgorithmShift_test(uint32 caseNo, uint32 unit);
int32 dal_trunk_trafficSeparate_test(uint32 caseNo, uint32 unit);
int32 dal_trunk_distributionAlgorithmBind_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_TRUNK_TEST_CASE_H__*/

