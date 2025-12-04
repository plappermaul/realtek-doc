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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : PORT SDK test case
 *
 * Feature : Provide the APIs to access DOT1X
 *
 */

#ifndef __DAL_DOT1X_TEST_CASE_H__
#define __DAL_DOT1X_TEST_CASE_H__

extern int32 dal_dot1x_unauthPacketOper_test(uint32 caseNo);
extern int32 dal_dot1x_portBasedEnable_test(uint32 caseNo);
extern int32 dal_dot1x_portBasedAuthStatus_test(uint32 caseNo);
extern int32 dal_dot1x_portBasedDirection_test(uint32 caseNo);
extern int32 dal_dot1x_macBasedEnable_test(uint32 caseNo);
extern int32 dal_dot1x_macBasedDirection_test(uint32 caseNo);
extern int32 dal_dot1x_guestVlan_test(uint32 caseNo);
extern int32 dal_dot1x_guestVlanBehavior_test(uint32 caseNo);
extern int32 dal_dot1x_trapPri_test(uint32 caseNo);

#endif /* __DAL_DOT1X_TEST_CASE_H__ */
