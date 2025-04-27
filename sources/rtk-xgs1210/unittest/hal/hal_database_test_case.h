/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of HAL database test APIs in the SDK
 *
 * Feature : HAL database test APIs
 *
 */

#ifndef __HAL_DATABASE_TEST_CASE_H__
#define __HAL_DATABASE_TEST_CASE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Function Declaration
 */

extern int32
hal_database_halctrl_test(uint32 caseNo, uint32 unit);

extern int32
hal_database_regFieldDef_test(uint32 caseNo, uint32 unit);

#if defined(CONFIG_SDK_REG_DFLT_VAL)
extern int32
hal_database_regDefaultVal_test(uint32 caseNo, uint32 unit);
#endif

extern int32
hal_database_tableFieldDef_test(uint32 caseNo, uint32 unit);

uint32
_hal_chip_regEnd_get(uint32 unit);



#endif  /* __HAL_DATABASE_TEST_CASE_H__ */
