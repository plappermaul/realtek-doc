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
 * Purpose : VLAN SDK test case
 *
 * Feature : Provide the APIs to access Trunk
 *
 */

#ifndef __DAL_TRUNK_TEST_CASE_H__
#define __DAL_TRUNK_TEST_CASE_H__


extern int32 dal_trunk_Port_test(uint32 caseNo);
extern int32 dal_trunk_distributionAlgorithm_test(uint32 caseNo);
extern int32 dal_trunk_hashMappingTable_test(uint32 caseNo);
extern int32 dal_trunk_mode_test(uint32 caseNo);
extern int32 dal_trunk_trafficPause_test(uint32 caseNo);
extern int32 dal_trunk_separateType_test(uint32 caseNo);

#endif /* __DAL_TRUNK_TEST_CASE_H__ */
