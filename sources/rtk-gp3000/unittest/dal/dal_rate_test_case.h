/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : rate Driver test case
 *
 * Feature : test rate API
 *
 */

#ifndef __DAL_RATE_TEST_CASE_H__
#define __DAL_RATE_TEST_CASE_H__

int32 dal_rate_portIgrBandwidthCtrlRate_test(uint32 testcase);
int32 dal_rate_portIgrBandwidthCtrlIncludeIfg_test(uint32 testcase);

int32 dal_rate_portEgrBandwidthCtrlRate_test(uint32 testcase);
int32 dal_rate_egrBandwidthCtrlIncludeIfg_test(uint32 testcase);
int32 dal_rate_egrQueueBwCtrlEnable_test(uint32 testcase);
int32 dal_rate_egrQueueBwCtrlMeterIdx_test(uint32 testcase);
int32 dal_rate_stormControlMeterIdx_test(uint32 testcase);
int32 dal_rate_stormControlPortEnable_test(uint32 testcase);
int32 dal_rate_stormControlEnable_test(uint32 testcase);
int32 dal_rate_stormBypass_test(uint32 testcase);
int32 dal_rate_shareMeter_test(uint32 testcase);
int32 dal_rate_shareMeterBucket_test(uint32 testcase);
int32 dal_rate_shareMeterExceed_test(uint32 testcase);

#endif /* __DAL_RATE_TEST_CASE_H__ */

