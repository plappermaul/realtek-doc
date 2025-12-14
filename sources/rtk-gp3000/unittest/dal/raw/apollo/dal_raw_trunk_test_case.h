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
 * Purpose : Trunk Driver API test
 *
 * Feature : Provide the APIs to access Trunk fratures
 *
 */

#ifndef __DAL_RAW_TRUNK_TEST_CASE_H__
#define __DAL_RAW_TRUNK_TEST_CASE_H__

extern int32 dal_raw_trunk_memberPort_test(uint32 caseNo);
extern int32 dal_raw_trunk_mode_test(uint32 caseNo);
extern int32 dal_raw_trunk_hashMapping_test(uint32 caseNo);
extern int32 dal_raw_trunk_hashAlgorithm_test(uint32 caseNo);
extern int32 dal_raw_trunk_flowControl_test(uint32 caseNo);
extern int32 dal_raw_trunk_flood_test(uint32 caseNo);

#endif /* __DAL_RAW_TRUNK_TEST_CASE_H__ */
