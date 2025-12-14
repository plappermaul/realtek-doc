/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
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
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Interrupt Driver API test
 *
 * Feature : Provide the APIs to access classification fratures
 *
 */

#ifndef __DAL_INTR_TEST_CASE_H__
#define __DAL_INTR_TEST_CASE_H__

extern int32 dal_intr_imr_test(uint32 caseNo);
extern int32 dal_intr_ims_test(uint32 caseNo);
extern int32 dal_intr_sts_test(uint32 caseNo);
extern int32 dal_intr_polarity_test(uint32 caseNo);

#endif /* __DAL_INTR_TEST_CASE_H__ */
