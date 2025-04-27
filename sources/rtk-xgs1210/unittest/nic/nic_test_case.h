/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of NIC test APIs in the SDK
 *
 * Feature : NIC test APIs
 *
 */

#ifndef __NIC_TEST_CASE_H__
#define __NIC_TEST_CASE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Function Declaration
 */

/* Nic Testing Case */
extern int32 nic_case2_test(uint32 caseNo, uint32 unit);
extern int32 nic_case3_test(uint32 caseNo, uint32 unit);
extern int32 nic_case4_test(uint32 caseNo, uint32 unit);
extern int32 nic_case5_test(uint32 caseNo, uint32 unit);
extern int32 nic_case6_test(uint32 caseNo, uint32 unit);
extern int32 nic_case7_test(uint32 caseNo, uint32 unit);
extern int32 nic_case8_test(uint32 caseNo, uint32 unit);
extern int32 nic_case9_test(uint32 caseNo, uint32 unit);
extern int32 nic_case10_test(uint32 caseNo, uint32 unit);
extern int32 nic_case11_test(uint32 caseNo, uint32 unit);
extern int32 nic_case12_test(uint32 caseNo, uint32 unit);
extern int32 nic_case13_test(uint32 caseNo, uint32 unit);
extern int32 nic_pausePktPerformance_test(uint32 caseNo, uint32 unit);


extern int32 nic_8380_case0_test(uint32 caseNo, uint32 unit);


#endif  /* __NIC_TEST_CASE_H__ */

