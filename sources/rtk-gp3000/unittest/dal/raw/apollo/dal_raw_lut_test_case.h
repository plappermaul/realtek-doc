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
 * Purpose : Classification Driver API test
 *
 * Feature : Provide the APIs to LUT fratures
 *
 */

#ifndef __DAL_LUT_TEST_CASE_H__
#define __DAL_LUT_TEST_CASE_H__

extern int32 dal_lut_raw_entry_test(uint32 caseNo);
extern int32 dal_lut_raw_l2UcEntry_test(uint32 caseNo);
extern int32 dal_lut_raw_l2UcEntryAll_test(uint32 caseNo);
extern int32 dal_lut_raw_l2McDslEntry_test(uint32 caseNo);
extern int32 dal_lut_raw_l3McDslEntry_test(uint32 caseNo);
extern int32 dal_lut_raw_l3McRouteEntry_test(uint32 caseNo);
extern int32 dal_lut_raw_test(uint32 caseNo);
extern int32 dal_lut_raw_action_test(uint32 caseNo);
extern int32 dal_lut_raw_ipmcTable_test(uint32 caseNo);
extern int32 dal_lut_raw_unknIp4Mc_test(uint32 caseNo);
extern int32 dal_lut_raw_unknIp6Mc_test(uint32 caseNo);
extern int32 dal_lut_raw_unknL2Mc_test(uint32 caseNo);
extern int32 dal_lut_raw_unknMcPri_test(uint32 caseNo);
extern int32 dal_lut_raw_writeHashEntry_test(uint32 caseNo);
#endif /* __DAL_LUT_TEST_CASE_H__ */
