/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of HAL test APIs in the SDK
 *
 * Feature : HAL test APIs
 *
 */

#ifndef __HAL_TEST_CASE_H__
#define __HAL_TEST_CASE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Function Declaration
 */

/* Port Testing Case */
extern int32
testPort_1(uint32 caseNo);

/* HAL Testing Case */
extern int32
testHal_1(uint32 caseNo);

extern int32
testHal_2(uint32 caseNo);

extern int32
testComboMedia_1(uint32 caseNo);

extern int32
testComboMedia_2(uint32 caseNo);

extern int32
testComboMedia_3(uint32 caseNo);

extern int32
testRegField_check(uint32 caseNo);

extern int32
testRegDefaultVal_check(uint32 caseNo);

#endif  /* __HAL_TEST_CASE_H__ */
