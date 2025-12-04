/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of HAL API test APIs in the SDK
 *
 * Feature : HAL API test APIs
 *
 */

#ifndef __HAL_API_TEST_CASE_H__
#define __HAL_API_TEST_CASE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Function Declaration
 */

extern int32
hal_api_halFindDevice_test(uint32 caseNo);

extern int32
hal_api_halGetDriverId_test(uint32 caseNo);

extern int32
hal_api_halGetChipId_test(uint32 caseNo);

extern int32
hal_api_halIsPpBlockCheck_test(uint32 caseNo);

extern int32
hal_api_halFindDriver_test(uint32 caseNo);

extern int32
hal_api_halInit_test(uint32 caseNo);

extern int32
hal_api_halCtrlInfoGet_test(uint32 caseNo);

extern int32
hal_api_regArrayFieldRead_test(uint32 caseNo);

extern int32
hal_api_regArrayFieldWrite_test(uint32 caseNo);

extern int32
hal_api_regArrayRead_test(uint32 caseNo);

extern int32
hal_api_regArrayWrite_test(uint32 caseNo);

extern int32
hal_api_regFieldGet_test(uint32 caseNo);

extern int32
hal_api_regFieldSet_test(uint32 caseNo);

extern int32
hal_api_regFieldRead_test(uint32 caseNo);

extern int32
hal_api_regFieldWrite_test(uint32 caseNo);

extern int32
hal_api_regIdx2AddrGet_test(uint32 caseNo);

extern int32
hal_api_regIdxMaxGet_test(uint32 caseNo);

extern int32
hal_api_regRead_test(uint32 caseNo);

extern int32
hal_api_regWrite_test(uint32 caseNo);

extern int32
hal_api_tableFieldByteGet_test(uint32 caseNo);

extern int32
hal_api_tableFieldByteSet_test(uint32 caseNo);

extern int32
hal_api_tableFieldGet_test(uint32 caseNo);

extern int32
hal_api_tableFieldSet_test(uint32 caseNo);

extern int32
hal_api_tableRead_test(uint32 caseNo);

extern int32
hal_api_tableSizeGet_test(uint32 caseNo);

extern int32
hal_api_tableWrite_test(uint32 caseNo);

/* HAL API test case: miim.c */
extern int32
hal_api_halMiimReadWrite_test(uint32 caseNo);

extern int32
hal_api_phyAutoNegoAbilityGetSet_test(uint32 caseNo);

extern int32
hal_api_phyAutoNegoEnableGetSet_test(uint32 caseNo);

extern int32
hal_api_phyDuplexGetSet_test(uint32 caseNo);

extern int32
hal_api_phyEnableSet_test(uint32 caseNo);

extern int32
hal_api_phyMediaGetSet_test(uint32 caseNo);

extern int32
hal_api_phySpeedGetSet_test(uint32 caseNo);

extern int32
hal_api_phyIdentifyFind_test(uint32 caseNo);

extern int32
hal_api_phyIdentifyOUICheck_test(uint32 caseNo);

extern int32
hal_api_phyIdentifyPhyidGet_test(uint32 caseNo);

extern int32
hal_api_regReadPerformance_test(uint32 caseNo);

extern int32
hal_api_miimReadPerformance_test(uint32 caseNo);

#endif  /* __HAL_API_TEST_CASE_H__ */
