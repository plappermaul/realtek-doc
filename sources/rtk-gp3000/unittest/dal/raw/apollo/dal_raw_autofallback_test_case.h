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
 * Purpose : Auto Fallback Driver API test
 *
 * Feature : Provide the APIs to access Auto Fallback fratures
 *
 */

#ifndef __DAL_RAW_AUTOFALLBACK_TEST_CASE_H__
#define __DAL_RAW_AUTOFALLBACK_TEST_CASE_H__

extern int32 dal_raw_autoFallback_state_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_MaxMonitorCount_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_MaxErrorCount_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_reducePL_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_timeoutIgnore_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_timeoutTH_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_timer_test(uint32 caseNo);
extern int32 dal_raw_autoFallback_validFlow_test(uint32 caseNo);

#endif /* __DAL_RAW_AUTOFALLBACK_TEST_CASE_H__ */
