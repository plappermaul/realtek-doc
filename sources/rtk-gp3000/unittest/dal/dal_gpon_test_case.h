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
 * Purpose : GMac Driver API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#ifndef __DAL_GPON_TEST_CASE_H__
#define __DAL_GPON_TEST_CASE_H__


int32 dal_gpon_sn_test(uint32 testcase);
int32 dal_gpon_para_test(uint32 testcase);
int32 dal_gpon_tcont_test(uint32 testcase);
int32 dal_gpon_dsFlow_test(uint32 testcase);
int32 dal_gpon_usFlow_test(uint32 testcase);
int32 dal_gpon_mcFilter_test(uint32 testcase);
int32 dal_gpon_misc_test(uint32 testcase);

#endif /* __DAL_GPON_TEST_CASE_H__ */
