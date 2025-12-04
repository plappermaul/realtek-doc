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
 * Purpose : L34 Raw driver test case
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#ifndef __DAL_RAW_SVLAN_TEST_CASE_H__
#define __DAL_RAW_SVLAN_TEST_CASE_H__


int32 dal_svlan_raw_test(uint32 caseNo);
int32 dal_svlan_raw_mbrCfg_test(uint32 caseNo);
int32 dal_svlan_raw_mc2sCfg_test(uint32 caseNo);
int32 dal_svlan_raw_c2sCfg_test(uint32 caseNo);
int32 dal_svlan_raw_sp2cCfg_test(uint32 caseNo);


#endif /* __DAL_RAW_SVLAN_TEST_CASE_H__ */
