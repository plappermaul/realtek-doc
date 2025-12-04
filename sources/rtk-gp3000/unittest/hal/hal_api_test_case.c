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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <osal/memory.h>
//#include <osal/time.h>
//#include <drv/swcore/chip.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>

#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollo/apollo_table_struct.h>

#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
//#include <hal/phy/identify.h>
//#include <rtk/port.h>
#include <hal/hal_test_case.h>
#include <common/unittest_util.h>
#include <hal/hal_api_test_case.h>

/* Define symbol used for test input */
/* Common */
#define TEST_MIN_CHIP_REV   (0)
#define TEST_MAX_CHIP_REV   (CHIP_REV_ID_MAX)

/*
 * Function Declaration
 */
int32
hal_api_halFindDevice_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_halFindDevice_test */

int32
hal_api_halGetDriverId_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_halGetDriverId_test */

int32
hal_api_halGetChipId_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_halGetChipId_test */

int32
hal_api_halIsPpBlockCheck_test(uint32 caseNo)
{
    uint32  *pIsPpBlock = NULL;
    uint32  isPpBlock;
    int32   ret = RT_ERR_FAILED;

    /* Case 1: input NULL pointer and expected return RT_ERR_NULL_POINTER */
    ret = hal_isPpBlock_check(0, pIsPpBlock);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);

    /* Case 2: Check different addr result */    
    ret = hal_isPpBlock_check(0, &isPpBlock);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, TRUE);
    ret = hal_isPpBlock_check(0xFFFFFF, &isPpBlock);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, FALSE);
    ret = hal_isPpBlock_check(0x1CFF, &isPpBlock);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, TRUE);
    ret = hal_isPpBlock_check(0x1D00, &isPpBlock);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, FALSE);
    return RT_ERR_OK;
} /* end of hal_api_halIsPpBlockCheck_test */

int32
hal_api_halFindDriver_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_halFindDriver_test */

int32
hal_api_halInit_test(uint32 caseNo)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t *pHalCtrl = NULL;

    /* Case 1: */
    ret = hal_init();
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    pHalCtrl = hal_ctrlInfo_get();
    RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_flags, (HAL_CHIP_ATTACHED | HAL_CHIP_INITED));

    return RT_ERR_OK;
} /* end of hal_api_halInit_test */

int32
hal_api_halCtrlInfoGet_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_halCtrlInfoGet_test */

int32
hal_api_regArrayFieldRead_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_regArrayFieldRead_test */

int32
hal_api_regArrayFieldWrite_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regArrayFieldWrite_test */

int32
hal_api_regArrayRead_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regArrayRead_test */

int32
hal_api_regArrayWrite_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regArrayWrite_test */

int32
hal_api_regFieldGet_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regFieldGet_test */

int32
hal_api_regFieldSet_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regFieldSet_test */

int32
hal_api_regFieldRead_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regFieldRead_test */

int32
hal_api_regFieldWrite_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regFieldWrite_test */

int32
hal_api_regIdx2AddrGet_test(uint32 caseNo)
{
    uint32  reg_idx, i, add_range_error = 0;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX();
    ret = reg_idx2Addr_get(0, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = reg_idx2Addr_get(reg_idx, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);

    for (i = 0; i < reg_idx; i++)
    {
        ret = reg_idx2Addr_get(i, &value);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        //osal_printf("reg_addr = 0x%x, SWCORE_BASE = 0x%x\n", value, SWCORE_BASE);
        if (value >= 0x1000000)
        {
            //osal_printf("reg_idx=%d, reg_addr=0x%x\n", i, value);
            add_range_error = 1;
        }
        RT_TEST_IS_EQUAL_INT("Wrong! ", add_range_error, 0);
    }

    return RT_ERR_OK;
} /* end of hal_api_regIdx2AddrGet_test */

int32
hal_api_regIdxMaxGet_test(uint32 caseNo)
{
    uint32  reg_idx;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX();
    ret = reg_idxMax_get(pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = reg_idxMax_get(&value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", value, reg_idx);

    return RT_ERR_OK;
} /* end of hal_api_regIdxMaxGet_test */

int32
hal_api_regRead_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regRead_test */

int32
hal_api_regWrite_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_regWrite_test */

int32
hal_api_tableFieldByteGet_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_tableFieldByteGet_test */

int32
hal_api_tableFieldByteSet_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_tableFieldByteSet_test */

int32
hal_api_tableFieldGet_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_tableFieldGet_test */

int32
hal_api_tableFieldSet_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_tableFieldSet_test */

int32
hal_api_tableRead_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_tableRead_test */

int32
hal_api_tableSizeGet_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_tableSizeGet_test */

int32
hal_api_tableWrite_test(uint32 caseNo)
{
    return RT_ERR_OK;
} /* end of hal_api_tableWrite_test*/



int32
hal_api_regReadPerformance_test(uint32 caseNo)
{

    return RT_ERR_OK;
} /* end of hal_api_regReadPerformance_test */

