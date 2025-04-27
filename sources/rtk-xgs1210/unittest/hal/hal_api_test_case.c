/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision: 82367 $
 * $Date: 2017-09-21 13:59:33 +0800 (Thu, 21 Sep 2017) $
 *
 * Purpose : Definition of HAL API test APIs in the SDK
 *
 * Feature : HAL API test APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <private/drv/swcore/swcore.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#endif
#if defined(CONFIG_SDK_RTL8380)
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#endif
#include <hal/common/halctrl.h>
#include <hal/common/miim.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/phy/identify.h>
#include <rtk/port.h>
#include <hal/hal_test_case.h>
#include <common/unittest_util.h>
#include <ioal/mem32.h>


/* Define symbol used for test input */
/* Common */
#define TEST_MIN_CHIP_REV   (0)
#define TEST_MAX_CHIP_REV   (CHIP_REV_ID_MAX)
#define TEST_MIN_UNIT_ID    0
#define TEST_MAX_UNIT_ID    RTK_MAX_UNIT_ID

/*
 * Function Declaration
 */
int32
hal_api_halFindDevice_test(uint32 caseNo, uint32 unit)
{
    uint32 chip_id, rev_id;
    rt_device_t *dev;

    drv_swcore_cid_get(unit, &chip_id, &rev_id);

    /* Case 1: too large revision id and expected return NULL */
    dev = hal_find_device(chip_id, CHIP_REV_ID_MAX+1);
    RT_TEST_IS_EQUAL_INT("Wrong! ", (int)dev, (int)NULL);

    /* Case 2: Exist chip_id with exact match revision and expected find device */
    dev = hal_find_device(chip_id, CHIP_REV_ID_A);
    RT_TEST_IS_NOT_EQUAL_INT("Wrong! ", (int)dev, (int)NULL);
    RT_TEST_IS_EQUAL_INT("Wrong! ", dev->chip_id, chip_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", dev->chip_rev_id, CHIP_REV_ID_A);

    /* Case 3: Exist chip_id with newer revision and expected find nearest device
         * Assume support revision A only; need to change when supported_devices array is updated
         */
    dev = hal_find_device(chip_id, CHIP_REV_ID_C);
    RT_TEST_IS_NOT_EQUAL_INT("Wrong! ", (int)dev, (int)NULL);
    RT_TEST_IS_EQUAL_INT("Wrong! ", dev->chip_id, chip_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", dev->chip_rev_id, CHIP_REV_ID_A);

    /* Case 4: Non-exist chip_id and expected return NULL */
    dev = hal_find_device(0x23795916, CHIP_REV_ID_A);
    RT_TEST_IS_EQUAL_INT("Wrong! ", (int)dev, (int)NULL);


    return RT_ERR_OK;
} /* end of hal_api_halFindDevice_test */

int32
hal_api_halGetDriverId_test(uint32 caseNo, uint32 unit)
{
    int8  inter_result1 = 1;
    int8  inter_result2 = 1;
    int32  expect_result = 1;
    uint32  chip_id, rev_id;
    int32   ret = RT_ERR_FAILED;
    uint32  *pDriver_id = NULL, *pDriver_rev_id = NULL;
    uint32  driver_id, driver_rev_id;

    if (RT_ERR_CHIP_NOT_SUPPORTED == hal_get_driver_id(unit, 0, &driver_id, &driver_rev_id))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    drv_swcore_cid_get(unit, &chip_id, &rev_id);

    /* Case 1: input NULL pointer and expected return RT_ERR_NULL_POINTER */
    ret = hal_get_driver_id(chip_id, rev_id, pDriver_id, pDriver_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = hal_get_driver_id(chip_id, rev_id, pDriver_id, &driver_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = hal_get_driver_id(chip_id, rev_id, &driver_id, pDriver_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);

    /* Case 2: Non-exist chip_id and expected return RT_ERR_CHIP_NOT_FOUND */
    ret = hal_get_driver_id(0x23795916, rev_id, &driver_id, &driver_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_CHIP_NOT_FOUND);

    /* Case 3: Exist chip_id & rev_id and expected return RT_ERR_OK */
    ret = hal_get_driver_id(chip_id, rev_id, &driver_id, &driver_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);


    /* Case 4: Formal Test Code */
    {

        uint32  chipId[UNITTEST_MAX_TEST_VALUE];
        int32   chipId_result[UNITTEST_MAX_TEST_VALUE];
        int32   chipId_index;
        int32   chipId_last;

        uint32  chipRevId[UNITTEST_MAX_TEST_VALUE];
        int32   chipRevId_result[UNITTEST_MAX_TEST_VALUE];
        int32   chipRevId_index;
        int32   chipRevId_last;

        uint32  result_driver_id, result_driver_rev_id;
        UNITTEST_TEST_VALUE_ASSIGN(chip_id, chip_id, chipId, chipId_result, chipId_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_CHIP_REV, TEST_MIN_CHIP_REV, chipRevId, chipRevId_result, chipRevId_last);

        for (chipId_index = 0; chipId_index <= chipId_last; chipId_index++)
        {
            inter_result1 = chipId_result[chipId_index];
            for (chipRevId_index = 0; chipRevId_index <= chipRevId_last; chipRevId_index++)
            {
                inter_result2 = chipRevId_result[chipRevId_index];
                ret = hal_get_driver_id(chipId[chipId_index], chipRevId[chipRevId_index], (uint32*)&result_driver_id, (uint32*)&result_driver_rev_id);
                expect_result = (inter_result1 && inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("hal_get_driver_id (chip_id 0x%x, chip_rev_id 0x%x)"
                                    , ret, expect_result, chipId_result[chipId_index], chipRevId_result[chipRevId_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("hal_get_driver_id (chip_id 0x%x, chip_rev_id 0x%x)"
                                        , ret, RT_ERR_OK, chipId_result[chipId_index], chipRevId_result[chipRevId_index]);
                }
            }
        }
    }

    return RT_ERR_OK;
} /* end of hal_api_halGetDriverId_test */

int32
hal_api_halGetChipId_test(uint32 caseNo, uint32 unit)
{
    uint32  chip_id, rev_id;
    uint32  *pChip_id = NULL, *pChip_rev_id = NULL;
    int32   ret = RT_ERR_FAILED;

    /* Case 1: input NULL pointer and expected return RT_ERR_NULL_POINTER */
    ret = drv_swcore_cid_get(unit, pChip_id, pChip_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = drv_swcore_cid_get(unit, pChip_id, &rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = drv_swcore_cid_get(unit, &chip_id, pChip_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);

    /* Case 2: Get chip_id & rev_id and expected return RT_ERR_OK */
    ret = drv_swcore_cid_get(unit, &chip_id, &rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
#if defined(CONFIG_SDK_MODEL_MODE)
    if(HWP_8390_50_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", chip_id, RTL8390M_CHIP_ID);
        RT_TEST_IS_EQUAL_INT("Wrong! ", rev_id, CHIP_REV_ID_A);
    }
    if(HWP_8380_30_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", chip_id, RTL8382M_CHIP_ID);
        RT_TEST_IS_EQUAL_INT("Wrong! ", rev_id, CHIP_REV_ID_A);
    }
#else
    osal_printf("Unit 0: chip_id[0x%x], rev_id[0x%x]\n", chip_id, rev_id);
#endif /* end of defined(CONFIG_SDK_MODEL_MODE) */

    return RT_ERR_OK;
} /* end of hal_api_halGetChipId_test */

int32
hal_api_halIsPpBlockCheck_test(uint32 caseNo, uint32 unit)
{
    uint32  *pIsPpBlock = NULL, *pPpBlockIdx = NULL;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    uint32  isPpBlock, ppBlockIdx;
#endif
    int32   ret = RT_ERR_FAILED;

    /* Case 1: input NULL pointer and expected return RT_ERR_NULL_POINTER */
    ret = hal_isPpBlock_check(unit, 0, pIsPpBlock, pPpBlockIdx);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);

    /* Case 2: Check different addr result */
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        ret = hal_isPpBlock_check(unit, 0x8000, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, TRUE);
        ret = hal_isPpBlock_check(unit, 0xFFFFFF, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, FALSE);
        ret = hal_isPpBlock_check(unit, 0x9A7C, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, TRUE);
        ret = hal_isPpBlock_check(unit, 0x9A80, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, FALSE);
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        ret = hal_isPpBlock_check(unit, 0xd560, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, TRUE);
        ret = hal_isPpBlock_check(unit, 0xFFFFFF, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, FALSE);
        ret = hal_isPpBlock_check(unit, 0xe3df, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, TRUE);
        ret = hal_isPpBlock_check(unit, 0xe3e0, &isPpBlock, &ppBlockIdx);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", isPpBlock, FALSE);
    }
#else
    osal_printf("Undefined chip compiler option!!\n");
    return RT_ERR_FAILED;
#endif

    return RT_ERR_OK;
} /* end of hal_api_halIsPpBlockCheck_test */

int32
hal_api_halFindDriver_test(uint32 caseNo, uint32 unit)
{
    rt_driver_t *pDriver = NULL;
#if !defined(CONFIG_SDK_MODEL_MODE)
    uint32 driver_id, driver_rev_id;
    uint32 chip_id, rev_id;
    int32   ret = RT_ERR_FAILED;
#endif

    /* Case 1: */
#if defined(CONFIG_SDK_MODEL_MODE)
  #if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        pDriver = hal_find_driver(RTL8390M_CHIP_ID, CHIP_REV_ID_A);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (int) pDriver, (int) &rtl8390_a_driver);
        pDriver = hal_find_driver(RTL8390M_CHIP_ID, CHIP_REV_ID_C);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (int) pDriver, (int) NULL);
    }
  #endif
  #if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        pDriver = hal_find_driver(RTL8380M_CHIP_ID, CHIP_REV_ID_A);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (int) pDriver, (int) &rtl8380_a_driver);
        pDriver = hal_find_driver(RTL8380M_CHIP_ID, CHIP_REV_ID_C);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (int) pDriver, (int) NULL);
    }
  #endif
#else
    ret = drv_swcore_cid_get(unit, &chip_id, &rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    ret = hal_get_driver_id(chip_id, rev_id, &driver_id, &driver_rev_id);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    pDriver = hal_find_driver(driver_id, driver_rev_id);
    RT_TEST_IS_NOT_EQUAL_INT("Wrong! ", (int) pDriver, (int) NULL);
#endif

    return RT_ERR_OK;
} /* end of hal_api_halFindDriver_test */

int32
hal_api_halInit_test(uint32 caseNo, uint32 unit)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t *pHalCtrl = NULL;

    /* Case 1: */
    ret = hal_init();
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    pHalCtrl = hal_ctrlInfo_get(unit);
    RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_flags, (HAL_CHIP_ATTACHED | HAL_CHIP_INITED));

    return RT_ERR_OK;
} /* end of hal_api_halInit_test */

int32
hal_api_halCtrlInfoGet_test(uint32 caseNo)
{
#if 0 /* for user-mode only */
    uint32  store_flags = 0;
    int32   ret = RT_ERR_FAILED;
    hal_control_t *pHalCtrl = NULL;

    /* Case 1: */
    ret = hal_init();
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    store_flags = hal_ctrl[0].chip_flags;
    hal_ctrl[0].chip_flags = 0;
    pHalCtrl = hal_ctrlInfo_get(0);
    RT_TEST_IS_EQUAL_INT("Wrong! ", (int) pHalCtrl, (int) NULL);
    hal_ctrl[0].chip_flags = store_flags;
    pHalCtrl = hal_ctrlInfo_get(0);
    RT_TEST_IS_NOT_EQUAL_INT("Wrong! ", (int) pHalCtrl, (int) NULL);
#endif

    return RT_ERR_OK;
} /* end of hal_api_halCtrlInfoGet_test */

int32
hal_api_regArrayFieldRead_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
    uint32  i, j;
#endif

    uint32  reg_idx;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_array_field_read(unit, reg_idx, -1, -1, 0, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_array_field_read(unit, 0, -1, -1, 0, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
    /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  org_reg_data, reg_data, field_val, expected_field_data;

        reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &org_reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", org_reg_data);

        reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &test_reg_data);

        reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);

        expected_field_data = (test_reg_data >> 0) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_L2_HASH_ALGOf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_L2_HASH_ALGOf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 1) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV4_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IPV4_MC_HASH_KEY_FMTf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 3) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV6_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IPV6_MC_HASH_KEY_FMTf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 5) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_FVID_CMPf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IP_MC_FVID_CMPf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 6) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_SA_ALL_ZERO_LRNf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_SA_ALL_ZERO_LRNf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 7) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LINK_DOWN_P_INVLDf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_LINK_DOWN_P_INVLDf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 8) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_DIP_CHKf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IP_MC_DIP_CHKf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 9) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_MC_BC_SA_DROPf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_MC_BC_SA_DROPf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 10) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_PPPOE_PARSE_ENf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_PPPOE_PARSE_ENf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 11) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LUTCAM_ENf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_LUTCAM_ENf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 12) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FORBID_ACTf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_FORBID_ACTf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 14) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FLUSH_NOTIFY_ENf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_FLUSH_NOTIFY_ENf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        /* Restore register to original value before run this testing */
        reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &org_reg_data);
      }

      /* Case 1: Global One dimension array checking */
      {
        uint32  i;
        uint32  vlanProfile[8][2] = {{0x0000040a, 0x01f59726}, {0x000005f7, 0x00123456},
                                     {0x00000a84, 0x01abcedf}, {0x00000e53, 0x00987654},
                                     {0x0000030d, 0x01555555}, {0x0000029b, 0x00002233},
                                     {0x00000004, 0x01aaaaaa}, {0x00000390, 0x00161616}};
        uint32  org_reg_data[8][2], reg_data[8][2], field_val, expected_field_data;

        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        /* Keep the register value here and will restore after testing */
        for (i = 0; i < 8; i++)
        {
            reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &org_reg_data[i][0]);
            //osal_printf("CYPRESS_VLAN_PROFILEr[%d][0] = 0x%x, CYPRESS_VLAN_PROFILEr[%d][1] = 0x%x\n", i, org_reg_data[i][0], i, org_reg_data[i][1]);
        }
        /* Write the testing register value to chip */
        for (i = 0; i < 8; i++)
        {
            reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &vlanProfile[i][0]);
        }
        /* Read from register and check same as setting value or not? */
        for (i = 0; i < 8; i++)
        {
            reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            //osal_printf("CYPRESS_VLAN_PROFILEr[%d][0] = 0x%x, CYPRESS_VLAN_PROFILEr[%d][1] = 0x%x\n", i, reg_data[i][0], i, reg_data[i][1]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
        }

        for (i = 0; i < 8; i++)
        {
            expected_field_data = (vlanProfile[i][0] >> 0) & 0xFFF;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_IP6_UNKN_MC_FLD_PMSKf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

            expected_field_data = (vlanProfile[i][1] >> 13) & 0xFFF;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_IP4_UNKN_MC_FLD_PMSKf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

            expected_field_data = (vlanProfile[i][1] >> 1) & 0xFFF;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_L2_UNKN_MC_FLD_PMSKf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

            expected_field_data = (vlanProfile[i][1] >> 0) & 0x1;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_L2_LRN_ENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
        }

        /* Restore register to original value before run this testing */
        for (i = 0; i < 8; i++)
        {
            reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &org_reg_data[i][0]);
        }
      }
      /* Case 2: Global Two dimension array checking */
      {
        uint32  portPpbVlan[8] = {0x00003028, 0x0000afe8,
                                  0x0000740a, 0x00009726,
                                  0x0000e3b5, 0x00009274,
                                  0x00001010, 0x0000ef12};
        uint32  reg_data, field_val, expected_field_data;

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[%d, %d]r = 0x%x\n", i, j, reg_data);
            }
        }
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &portPpbVlan[j]);
            }
        }
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[%d, %d]r = 0x%x\n", i, j, reg_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portPpbVlan[j]);
            }
        }
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                expected_field_data = (portPpbVlan[j] >> 4) & 0xFFF;
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[reg_%d_field_%d] = 0x%x\n", i, j, expected_field_data);
                ret = reg_array_field_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, CYPRESS_PPB_VIDf, &field_val);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

                expected_field_data = (portPpbVlan[j] >> 1) & 0x7;
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[reg_%d_field_%d] = 0x%x\n", i, j, expected_field_data);
                ret = reg_array_field_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, CYPRESS_PPB_PRIf, &field_val);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

                expected_field_data = (portPpbVlan[j] >> 0) & 0x1;
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[reg_%d_field_%d] = 0x%x\n", i, j, expected_field_data);
                ret = reg_array_field_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, CYPRESS_VALIDf, &field_val);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            }
        }
        /* Restore register to original value before run this testing */
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, 0);
            }
        }
      }
      /* Case 3: PER_PORT One dimension array checking */
      {
        uint32  portBasedCtrl[53] = {0x7491740a, 0xadf59726, 0xe3c8e3b5, 0x24569274,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x63fead94};
        uint32  reg_data, field_val, expected_field_data;

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            //osal_printf("CYPRESS_MAC_PORT_CTRL[%d]r = 0x%x\n", i, reg_data);
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &portBasedCtrl[i]);
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            //osal_printf("CYPRESS_MAC_PORT_CTRL[%d]r = 0x%x\n", i, reg_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portBasedCtrl[i]);
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            expected_field_data = (portBasedCtrl[i] >> 28) & 0xF;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_IPG_MIN_RX_SELf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 8) & 0xFFFFF;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_IPG_LENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 7) & 0x1;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_BYP_TX_CRCf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 6) & 0x1;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_PASS_ALL_MODE_ENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 4) & 0x3;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_LATE_COLI_THRf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 3) & 0x1;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_RX_CHK_CRC_ENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 2) & 0x1;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_BKPRES_ENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 1) & 0x1;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_TX_ENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
            expected_field_data = (portBasedCtrl[i] >> 0) & 0x1;
            ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_RX_ENf, &field_val);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);
        }

        /* Restore register to original value before run this testing */
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            value = 0;
            reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &value);
        }
      }
    }
#endif


    return RT_ERR_OK;

} /* end of hal_api_regArrayFieldRead_test */

int32
hal_api_regArrayFieldWrite_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
    uint32  i, j;
#endif

    uint32  reg_idx;
    uint32  value = 0;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_array_field_write(unit, reg_idx, -1, -1, 0, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  org_reg_data, reg_data, expected_field_data;

        reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &org_reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", org_reg_data);

        expected_field_data = (test_reg_data >> 0) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_L2_HASH_ALGOf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_L2_HASH_ALGOf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 1) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV4_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IPV4_MC_HASH_KEY_FMTf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 3) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV6_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IPV6_MC_HASH_KEY_FMTf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 5) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_FVID_CMPf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IP_MC_FVID_CMPf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 6) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_SA_ALL_ZERO_LRNf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_SA_ALL_ZERO_LRNf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 7) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LINK_DOWN_P_INVLDf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_LINK_DOWN_P_INVLDf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 8) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_DIP_CHKf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_IP_MC_DIP_CHKf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 9) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_MC_BC_SA_DROPf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_MC_BC_SA_DROPf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 10) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_PPPOE_PARSE_ENf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_PPPOE_PARSE_ENf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 11) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LUTCAM_ENf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_LUTCAM_ENf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 12) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FORBID_ACTf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_FORBID_ACTf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 14) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FLUSH_NOTIFY_ENf] = 0x%x\n", expected_field_data);
        ret = reg_array_field_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, CYPRESS_FLUSH_NOTIFY_ENf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (reg_data&0x7FFF), (test_reg_data&0x7FFF));

        /* Restore to default */
        reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &org_reg_data);
      }

      /* Case 1: Global One dimension array checking */
      {
        uint32  i;
        uint32  vlanProfile[8][2] = {{0x0000040a, 0x01f59726}, {0x000005f7, 0x00123456},
                                     {0x00000a84, 0x01abcedf}, {0x00000e53, 0x00987654},
                                     {0x0000030d, 0x01555555}, {0x0000029b, 0x00002233},
                                     {0x00000004, 0x01aaaaaa}, {0x00000390, 0x00161616}};
        uint32  org_reg_data[8][2], reg_data[8][2], expected_field_data;

        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        /* Keep the register value here and will restore after testing */
        for (i = 0; i < 8; i++)
        {
            reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &org_reg_data[i][0]);
            //osal_printf("CYPRESS_VLAN_PROFILEr[%d][0] = 0x%x, CYPRESS_VLAN_PROFILEr[%d][1] = 0x%x\n", i, org_reg_data[i][0], i, org_reg_data[i][1]);
        }

        for (i = 0; i < 8; i++)
        {
            expected_field_data = (vlanProfile[i][0] >> 0) & 0xFFF;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_IP6_UNKN_MC_FLD_PMSKf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

            expected_field_data = (vlanProfile[i][1] >> 13) & 0xFFF;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_IP4_UNKN_MC_FLD_PMSKf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

            expected_field_data = (vlanProfile[i][1] >> 1) & 0xFFF;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_L2_UNKN_MC_FLD_PMSKf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

            expected_field_data = (vlanProfile[i][1] >> 0) & 0x1;
            //osal_printf("CYPRESS_VLAN_PROFILEr[field_%d] = 0x%x\n", i, expected_field_data);
            ret = reg_array_field_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, CYPRESS_L2_LRN_ENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        /* Read from register and check same as setting value or not? */
        for (i = 0; i < 8; i++)
        {
            reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            //osal_printf("CYPRESS_VLAN_PROFILEr[%d][0] = 0x%x, CYPRESS_VLAN_PROFILEr[%d][1] = 0x%x\n", i, reg_data[i][0], i, reg_data[i][1]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
        }

        /* Restore register to original value before run this testing */
        for (i = 0; i < 8; i++)
        {
            reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &org_reg_data[i][0]);
        }
      }
      /* Case 2: Global Two dimension array checking */
      {
        uint32  portPpbVlan[8] = {0x00003028, 0x0000afe8,
                                  0x0000740a, 0x00009726,
                                  0x0000e3b5, 0x00009274,
                                  0x00001010, 0x0000ef12};
        uint32  reg_data, expected_field_data;

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[%d, %d]r = 0x%x\n", i, j, reg_data);
            }
        }
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                expected_field_data = (portPpbVlan[j] >> 4) & 0xFFF;
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[reg_%d_field_%d] = 0x%x\n", i, j, expected_field_data);
                ret = reg_array_field_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, CYPRESS_PPB_VIDf, &expected_field_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

                expected_field_data = (portPpbVlan[j] >> 1) & 0x7;
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[reg_%d_field_%d] = 0x%x\n", i, j, expected_field_data);
                ret = reg_array_field_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, CYPRESS_PPB_PRIf, &expected_field_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

                expected_field_data = (portPpbVlan[j] >> 0) & 0x1;
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[reg_%d_field_%d] = 0x%x\n", i, j, expected_field_data);
                ret = reg_array_field_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, CYPRESS_VALIDf, &expected_field_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            }
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[%d, %d]r = 0x%x\n", i, j, reg_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portPpbVlan[j]);
            }
        }

        /* Restore register to original value before run this testing */
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, 0);
            }
        }
      }
      /* Case 3: PER_PORT One dimension array checking */
      {
        uint32  portBasedCtrl[53] = {0x7491740a, 0xadf59726, 0xe3c8e3b5, 0x24569274,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x63fead94};
        uint32  reg_data, expected_field_data;

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            //osal_printf("CYPRESS_MAC_PORT_CTRL[%d]r = 0x%x\n", i, reg_data);
        }

        for (i = 0; i <= 52; i++)
        {
            expected_field_data = (portBasedCtrl[i] >> 28) & 0xF;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_IPG_MIN_RX_SELf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 8) & 0xFFFFF;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_IPG_LENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 7) & 0x1;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_BYP_TX_CRCf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 6) & 0x1;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_PASS_ALL_MODE_ENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 4) & 0x3;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_LATE_COLI_THRf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 3) & 0x1;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_RX_CHK_CRC_ENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 2) & 0x1;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_BKPRES_ENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 1) & 0x1;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_TX_ENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            expected_field_data = (portBasedCtrl[i] >> 0) & 0x1;
            ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, CYPRESS_RX_ENf, &expected_field_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            //osal_printf("CYPRESS_MAC_PORT_CTRL[%d]r = 0x%x\n", i, reg_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portBasedCtrl[i]);
        }

        /* Restore register to original value before run this testing */
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            value = 0;
            reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &value);
        }
      }
    }
#endif
    return RT_ERR_OK;

} /* end of hal_api_regArrayFieldWrite_test */

int32
hal_api_regArrayRead_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

#if defined(CONFIG_SDK_RTL8390)
        uint32  i, j;
#endif
    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_array_read(unit, reg_idx, -1, -1, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_array_read(unit, 0, -1, -1, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
      /* Case 0: Checking error return case from _reg_addr_find API
       * RT_ERR_REG_ARRAY_INDEX_1 - invalid index 1 of register array
       * RT_ERR_REG_ARRAY_INDEX_2 - invalid index 2 of register array
       */
      {
        /* Global Zero dimension array checking */
        uint32  test_reg_data = 0x7491b40a;
        uint32  org_reg_data, reg_data;

        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, 0, -2, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, 0, 1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, -2, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, 1, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &org_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &test_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (reg_data&0x7FFF), (test_reg_data&0x7FFF));

        /* Restore to default */
        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &org_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
      }

      /* Case 1: Global One dimension array checking */
      {
        uint32  vlanProfile[8][2] = {{0x0000040a, 0x01f59726}, {0x000005f7, 0x00123456},
                                     {0x00000a84, 0x01abcedf}, {0x00000e53, 0x00987654},
                                     {0x0000030d, 0x01555555}, {0x0000029b, 0x00002233},
                                     {0x00000004, 0x01aaaaaa}, {0x00000390, 0x00161616}};
        uint32  reg_data[8][2];

        ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, 0, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, -2, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, 7, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, 8, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);

        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i < 8; i++)
        {
            ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
        }

        /* Restore to default */
        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][1]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
      }
      /* Case 2: Global Two dimension array checking */
      {
        uint32  portPpbVlan[8] = {0x00003028, 0x0000afe8,
                                  0x0000740a, 0x00009726,
                                  0x0000e3b5, 0x00009274,
                                  0x00001010, 0x0000ef12};
        uint32  reg_data;

        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, -2, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 52, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 53, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);

        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, 0, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, -2, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, 7, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, 8, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &portPpbVlan[j]);
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[%d, %d]r = 0x%x\n", i, j, portPpbVlan[j]);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            }
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
                //osal_printf("CYPRESS_VLAN_PORT_PPB_VLAN[%d, %d]r = 0x%x\n", i, j, reg_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portPpbVlan[j]);
            }
        }
        /* Restore register to original value before run this testing */
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, 0);
            }
        }
      }
      /* Case 3: PER_PORT One dimension array checking */
      {
        uint32  portBasedCtrl[53] = {0x7491740a, 0xadf59726, 0xe3c8e3b5, 0x24569274,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x63fead94};
        uint32  reg_data;

        ret = reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, 0, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, -2, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, 52, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, 53, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &portBasedCtrl[i]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            ret = reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portBasedCtrl[i]);
        }

        /* Restore to default */
        reg_data = 0;
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
      }
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_regArrayRead_test */


int32 hal_api_regArrayWrite_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
    uint32  i, j;
#endif
    uint32  reg_idx;
    uint32  value = 0;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_array_write(unit, reg_idx, -1, -1, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
      /* Case 0: Checking error return case from _reg_addr_find API
       * RT_ERR_REG_ARRAY_INDEX_1 - invalid index 1 of register array
       * RT_ERR_REG_ARRAY_INDEX_2 - invalid index 2 of register array
       */
      {
        /* Global Zero dimension array checking */
        uint32  test_reg_data = 0x7491b40a;
        uint32  reg_data;

        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, 0, -2, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, 0, 1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, -2, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, 1, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &test_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_array_read(unit, CYPRESS_L2_CTRL_0r, -1, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (reg_data&0x7FFF), (test_reg_data&0x7FFF));

        /* Restore to default */
        reg_data = 0;
        ret = reg_array_write(unit, CYPRESS_L2_CTRL_0r, -1, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
      }

      /* Case 1: Global One dimension array checking */
      {
        uint32  vlanProfile[8][2] = {{0x0000040a, 0x01f59726}, {0x000005f7, 0x00123456},
                                     {0x00000a84, 0x01abcedf}, {0x00000e53, 0x00987654},
                                     {0x0000030d, 0x01555555}, {0x0000029b, 0x00002233},
                                     {0x00000004, 0x01aaaaaa}, {0x00000390, 0x00161616}};
        uint32  reg_data[8][2];

        ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, 0, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, -2, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, 7, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, 8, &reg_data[0][0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);

        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i < 8; i++)
        {
            ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
        }


        /* Restore to default */
        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
      }
      /* Case 2: Global Two dimension array checking */
      {
        uint32  portPpbVlan[8] = {0x00003028, 0x0000afe8,
                                  0x0000740a, 0x00009726,
                                  0x0000e3b5, 0x00009274,
                                  0x00001010, 0x0000ef12};
        uint32  reg_data;

        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, -2, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 52, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 53, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);

        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, 0, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, -2, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);
        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, 7, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, 0, 8, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_2);

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                ret = reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &portPpbVlan[j]);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            }
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                ret = reg_array_read(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
                RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portPpbVlan[j]);
            }
        }
        /* Restore to default */
        reg_data = 0;
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            for (j = 0; j <= 7; j++)
            {
                reg_array_write(unit, CYPRESS_VLAN_PORT_PPB_VLANr, i, j, &reg_data);
            }
        }
      }
      /* Case 3: PER_PORT One dimension array checking */
      {
        uint32  portBasedCtrl[53] = {0x7491740a, 0xadf59726, 0xe3c8e3b5, 0x24569274,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0x10101010, 0xabcdef12, 0xffffffff, 0x00000000,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x09089458, 0xfeabc984, 0x55005555, 0x00555500,
                                     0xa57f8364, 0x64773526, 0x647783fa, 0xfae87438,
                                     0xaa0000aa, 0xaa00aa00, 0x43742874, 0x7722def8,
                                     0x73ffaabb, 0x73673463, 0x74637655, 0x7463feee,
                                     0x63fead94};
        uint32  reg_data;

        ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, 0, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, -2, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);
        ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, 52, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, 53, -1, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_REG_ARRAY_INDEX_1);

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &portBasedCtrl[i]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            ret = reg_array_read(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data, portBasedCtrl[i]);
        }

        /* Restore to default */
        reg_data = 0;
        for (i = 0; i <= 52; i++)
        {
            /* In FPGA board, only partial ports are exist, skip testing in non-existing ports */
            uint32  port_exist;

            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, i, port_exist);
            if (port_exist == 0)
                continue;

            ret = reg_array_write(unit, CYPRESS_MAC_PORT_CTRLr, i, -1, &reg_data);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
      }
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_regArrayWrite_test */

int32
hal_api_regFieldGet_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx, field_idx;
    uint32  *pValue = NULL, value, data = 0;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    field_idx = HAL_GET_MAX_REGFIELD_IDX(unit);
    ret = reg_field_get(unit, reg_idx, 0, &value, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_field_get(unit, 0, field_idx, &value, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_field_get(unit, 0, 0, pValue, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {

      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  org_reg_data, reg_data, field_val, expected_field_data;

        reg_read(unit, CYPRESS_L2_CTRL_0r, &org_reg_data);

        reg_write(unit, CYPRESS_L2_CTRL_0r, &test_reg_data);

        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);

        expected_field_data = (test_reg_data >> 0) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_L2_HASH_ALGOf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 1) & 0x3;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV4_MC_HASH_KEY_FMTf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 3) & 0x3;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV6_MC_HASH_KEY_FMTf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 5) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_FVID_CMPf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 6) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_SA_ALL_ZERO_LRNf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 7) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LINK_DOWN_P_INVLDf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 8) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_DIP_CHKf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 9) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_MC_BC_SA_DROPf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 10) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_PPPOE_PARSE_ENf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 11) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LUTCAM_ENf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 12) & 0x3;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FORBID_ACTf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 14) & 0x1;
        ret = reg_field_get(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FLUSH_NOTIFY_ENf, &field_val, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        /* Restore to default */
        reg_data = 0;
        reg_write(unit, CYPRESS_L2_CTRL_0r, &reg_data);
      }
    }
#endif
    return RT_ERR_OK;

} /* end of hal_api_regFieldGet_test */

int32
hal_api_regFieldSet_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx, field_idx;
    uint32  *pData = NULL, value = 0, data;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    field_idx = HAL_GET_MAX_REGFIELD_IDX(unit);
    ret = reg_field_set(unit, reg_idx, 0, &value, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_field_set(unit, 0, field_idx, &value, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_field_set(unit, 0, 0, &value, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  reg_data, expected_field_data;
        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);

        expected_field_data = (test_reg_data >> 0) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_L2_HASH_ALGOf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_L2_HASH_ALGOf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 1) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV4_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV4_MC_HASH_KEY_FMTf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 3) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV6_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV6_MC_HASH_KEY_FMTf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 5) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_FVID_CMPf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_FVID_CMPf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 6) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_SA_ALL_ZERO_LRNf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_SA_ALL_ZERO_LRNf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 7) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LINK_DOWN_P_INVLDf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LINK_DOWN_P_INVLDf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 8) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_DIP_CHKf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_DIP_CHKf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 9) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_MC_BC_SA_DROPf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_MC_BC_SA_DROPf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 10) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_PPPOE_PARSE_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_PPPOE_PARSE_ENf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 11) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LUTCAM_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LUTCAM_ENf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 12) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FORBID_ACTf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FORBID_ACTf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 14) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FLUSH_NOTIFY_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_set(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FLUSH_NOTIFY_ENf, &expected_field_data, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_write(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (reg_data&0x7FFF), (test_reg_data&0x7FFF));

        /* Restore to default */
        reg_data = 0;
        reg_write(unit, CYPRESS_L2_CTRL_0r, &reg_data);
      }
    }
#endif

    return RT_ERR_OK;

} /* end of hal_api_regFieldSet_test */

int32
hal_api_regFieldRead_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_field_read(unit, reg_idx, 0, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_field_read(unit, 0, 0, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  reg_data, field_val, expected_field_data;

        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);

        reg_write(unit, CYPRESS_L2_CTRL_0r, &test_reg_data);

        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);

        expected_field_data = (test_reg_data >> 0) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_L2_HASH_ALGOf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_L2_HASH_ALGOf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 1) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV4_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV4_MC_HASH_KEY_FMTf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 3) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV6_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV6_MC_HASH_KEY_FMTf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 5) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_FVID_CMPf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_FVID_CMPf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 6) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_SA_ALL_ZERO_LRNf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_SA_ALL_ZERO_LRNf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 7) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LINK_DOWN_P_INVLDf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LINK_DOWN_P_INVLDf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 8) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_DIP_CHKf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_DIP_CHKf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 9) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_MC_BC_SA_DROPf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_MC_BC_SA_DROPf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 10) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_PPPOE_PARSE_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_PPPOE_PARSE_ENf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 11) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LUTCAM_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LUTCAM_ENf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 12) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FORBID_ACTf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FORBID_ACTf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        expected_field_data = (test_reg_data >> 14) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FLUSH_NOTIFY_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FLUSH_NOTIFY_ENf, &field_val);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", field_val, expected_field_data);

        /* Restore to default */
        reg_data = 0;
        reg_write(unit, CYPRESS_L2_CTRL_0r, &reg_data);
      }
    }
#endif
    return RT_ERR_OK;

} /* end of hal_api_regFieldRead_test */

int32
hal_api_regFieldWrite_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx;
    uint32  value = 0;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_field_write(unit, reg_idx, 0, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  reg_data, expected_field_data;

        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);

        expected_field_data = (test_reg_data >> 0) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_L2_HASH_ALGOf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_L2_HASH_ALGOf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 1) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV4_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV4_MC_HASH_KEY_FMTf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 3) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IPV6_MC_HASH_KEY_FMTf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IPV6_MC_HASH_KEY_FMTf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 5) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_FVID_CMPf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_FVID_CMPf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 6) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_SA_ALL_ZERO_LRNf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_SA_ALL_ZERO_LRNf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 7) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LINK_DOWN_P_INVLDf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LINK_DOWN_P_INVLDf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 8) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_IP_MC_DIP_CHKf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_IP_MC_DIP_CHKf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 9) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_MC_BC_SA_DROPf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_MC_BC_SA_DROPf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 10) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_PPPOE_PARSE_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_PPPOE_PARSE_ENf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 11) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_LUTCAM_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_LUTCAM_ENf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 12) & 0x3;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FORBID_ACTf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FORBID_ACTf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        expected_field_data = (test_reg_data >> 14) & 0x1;
        //osal_printf("CYPRESS_L2_CTRL_0r[CYPRESS_FLUSH_NOTIFY_ENf] = 0x%x\n", expected_field_data);
        ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_FLUSH_NOTIFY_ENf, &expected_field_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        //osal_printf("CYPRESS_L2_CTRL_0r = 0x%x\n", reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (reg_data&0x7FFF), (test_reg_data&0x7FFF));

        /* Restore to default */
        reg_data = 0;
        reg_write(unit, CYPRESS_L2_CTRL_0r, &reg_data);
      }
    }
#endif
    return RT_ERR_OK;

} /* end of hal_api_regFieldWrite_test */

int32
hal_api_regIdx2AddrGet_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx, i, add_range_error = 0;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_idx2Addr_get(unit, 0, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = reg_idx2Addr_get(unit, reg_idx, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);

    for (i = 0; i < reg_idx; i++)
    {
        ret = reg_idx2Addr_get(unit, i, &value);
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
hal_api_regIdxMaxGet_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_idxMax_get(unit, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = reg_idxMax_get(unit, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", value, reg_idx);

    return RT_ERR_OK;
} /* end of hal_api_regIdxMaxGet_test */

int32
hal_api_regRead_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx;
    uint32  *pValue = NULL, value;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_read(unit, reg_idx, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = reg_read(unit, 0, pValue);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {

      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  org_reg_data, reg_data;

        ret = reg_read(unit, CYPRESS_L2_CTRL_0r, &org_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_write(unit, CYPRESS_L2_CTRL_0r, &test_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data & 0x7FFF, test_reg_data & 0x7FFF);

        /* Restore to original value */
        ret = reg_write(unit, CYPRESS_L2_CTRL_0r, &org_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
      }

      /* Case 1: Global One dimension array checking */
      {
        uint32  vlanProfile[8][2] = {{0x0000040a, 0x01f59726}, {0x000005f7, 0x00123456},
                                     {0x00000a84, 0x01abcedf}, {0x00000e53, 0x00987654},
                                     {0x0000030d, 0x01555555}, {0x0000029b, 0x00002233},
                                     {0x00000004, 0x01aaaaaa}, {0x00000390, 0x00161616}};
        uint32  reg_data[8][2];
        uint32  i;

        for (i = 0; i < 8; i++)
        {
            ret = reg_write(unit, CYPRESS_VLAN_PROFILEr, &vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = reg_read(unit, CYPRESS_VLAN_PROFILEr, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
        }

        /* Restore to default */
        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = reg_read(unit, CYPRESS_VLAN_PROFILEr, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[0][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[0][1]);
        }

        /* Restore to default */
        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
      }
    }
#endif
    return RT_ERR_OK;

} /* end of hal_api_regRead_test */

int32
hal_api_regWrite_test(uint32 caseNo, uint32 unit)
{
    uint32  reg_idx;
    uint32  value = 0;
    int32   ret = RT_ERR_FAILED;

    reg_idx = HAL_GET_MAX_REG_IDX(unit);
    ret = reg_write(unit, reg_idx, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {

      /* Case 0: Global Zero dimension array checking */
      {
        uint32  test_reg_data = 0x7491b40a;
        uint32  org_reg_data, reg_data;

        ret = reg_read(unit, CYPRESS_L2_CTRL_0r, &org_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_write(unit, CYPRESS_L2_CTRL_0r, &test_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = reg_read(unit, CYPRESS_L2_CTRL_0r, &reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", (reg_data&0x7FFF), (test_reg_data&0x7FFF));

        /* Restore to default */
        ret = reg_write(unit, CYPRESS_L2_CTRL_0r, &org_reg_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
      }

      /* Case 1: Global One dimension array checking */
      {
        uint32  vlanProfile[8][2] = {{0x0000040a, 0x01f59726}, {0x000005f7, 0x00123456},
                                     {0x00000a84, 0x01abcedf}, {0x00000e53, 0x00987654},
                                     {0x0000030d, 0x01555555}, {0x0000029b, 0x00002233},
                                     {0x00000004, 0x01aaaaaa}, {0x00000390, 0x00161616}};
        uint32  reg_data[8][2];
        uint32  i;

        for (i = 0; i < 8; i++)
        {
            ret = reg_write(unit, CYPRESS_VLAN_PROFILEr, &vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = reg_read(unit, CYPRESS_VLAN_PROFILEr, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
        }

        /* Restore to default */
        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }

        for (i = 0; i < 8; i++)
        {
            ret = reg_write(unit, CYPRESS_VLAN_PROFILEr, &vlanProfile[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            if (i == 0)
            {
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
            }
            else
            {
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], 0);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], 0);
                ret = reg_array_read(unit, CYPRESS_VLAN_PROFILEr, -1, 0, &reg_data[i][0]);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][0], vlanProfile[i][0]);
                RT_TEST_IS_EQUAL_INT("Wrong! ", reg_data[i][1], vlanProfile[i][1]);
            }
        }

        /* Restore to default */
        osal_memset(&reg_data[0][0], 0, sizeof(reg_data));
        for (i = 0; i < 8; i++)
        {
            ret = reg_array_write(unit, CYPRESS_VLAN_PROFILEr, -1, i, &reg_data[i][0]);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
      }
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_regWrite_test */

int32
hal_api_tableFieldByteGet_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
    uint32  i;
    uint8   buff[8];
    uint32  vlan_uc_lookup_mode, vlan_mc_lookup_mode, vlan_profile_index, fid_data, vlan_mbr_data[2];
    vlan_entry_t  vlan_data;
#endif
    uint32  table_idx;
    uint32  data, *pData = NULL;
    int32   ret = RT_ERR_FAILED;
    uint8   value, *pValue = NULL;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    ret = table_field_byte_get(unit, table_idx, 0, &value, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_field_byte_get(unit, 0, 0, pValue, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_byte_get(unit, 0, 0, &value, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_byte_get(unit, 0, 100, &value, &data);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {

        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        osal_memset(&buff, 0, sizeof(buff));
        /* set fid and msti */
        fid_data = 0x53;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &fid_data, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set unicast lookup mode */
        vlan_uc_lookup_mode = 0x1;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &vlan_uc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set mulicast lookup mode */
        vlan_mc_lookup_mode = 0x0;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &vlan_mc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set profile index */
        vlan_profile_index = 0x5;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &vlan_profile_index, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set member set */
        vlan_mbr_data[0] = 0x12345678;
        vlan_mbr_data[1] = 0x0009abcd;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &vlan_mbr_data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_write(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Get back to check correct or not? */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));

        for (i = 0; i < sizeof(vlan_data)/sizeof(uint32); i++)
            RT_TEST_IS_EQUAL_INT("Wrong! ", vlan_data.entry_data[i], 0);
        ret = table_read(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_byte_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[0], fid_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_byte_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[0], vlan_uc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_byte_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[0], vlan_mc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_byte_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[0], vlan_profile_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_byte_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[0], (vlan_mbr_data[1]>>16)&0xFF);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[1], (vlan_mbr_data[1]>>8)&0xFF);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[2], (vlan_mbr_data[1])&0xFF);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[3], (vlan_mbr_data[0]>>24)&0xFF);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[4], (vlan_mbr_data[0]>>16)&0xFF);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[5], (vlan_mbr_data[0]>>8)&0xFF);
        RT_TEST_IS_EQUAL_INT("Wrong! ", buff[6], (vlan_mbr_data[0])&0xFF);

        /* Restore */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        ret = table_write(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    }

#endif

    return RT_ERR_OK;

} /* end of hal_api_tableFieldByteGet_test */

int32
hal_api_tableFieldByteSet_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
    uint32  i;
    uint8   buff[8];
    uint32  vlan_uc_lookup_mode, vlan_mc_lookup_mode, vlan_profile_index, fid_data, vlan_mbr_data[2];
    vlan_entry_t  vlan_data;
#endif

    uint32  table_idx;
    uint32  data[2], *pData = NULL;
    int32   ret = RT_ERR_FAILED;
    uint8   value, *pValue = NULL;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    ret = table_field_byte_set(unit, table_idx, 0, &value, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_field_byte_set(unit, 0, 0, pValue, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_byte_set(unit, 0, 0, &value, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_byte_set(unit, 0, 100, &value, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        osal_memset(buff, 0, sizeof(buff));
        fid_data = 0x53;
        buff[0] = fid_data;
        ret = table_field_byte_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        vlan_uc_lookup_mode = 0x1;
        buff[0] = vlan_uc_lookup_mode;
        ret = table_field_byte_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        vlan_mc_lookup_mode = 0x0;
        buff[0] = vlan_mc_lookup_mode;
        ret = table_field_byte_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        vlan_profile_index = 0x5;
        buff[0] = vlan_profile_index;
        ret = table_field_byte_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        vlan_mbr_data[0] = 0x12345678;
        vlan_mbr_data[1] = 0x0009abcd;
        buff[0] = (vlan_mbr_data[1]>>16) & 0xFF;
        buff[1] = (vlan_mbr_data[1]>>8) & 0xFF;
        buff[2] = (vlan_mbr_data[1]>>0) & 0xFF;
        buff[3] = (vlan_mbr_data[0]>>24) & 0xFF;
        buff[4] = (vlan_mbr_data[0]>>16) & 0xFF;
        buff[5] = (vlan_mbr_data[0]>>8) & 0xFF;
        buff[6] = (vlan_mbr_data[0]) & 0xFF;
        ret = table_field_byte_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_write(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Get back to check correct or not? */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));

        for (i = 0; i < sizeof(vlan_data)/sizeof(uint32); i++)
            RT_TEST_IS_EQUAL_INT("Wrong! ", vlan_data.entry_data[i], 0);
        ret = table_read(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], fid_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_uc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_profile_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mbr_data[0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[1], vlan_mbr_data[1]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Restore */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        ret = table_write(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_tableFieldByteSet_test */

int32
hal_api_tableFieldGet_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
        uint32  i;
        uint8   buff[8];
        uint32  vlan_uc_lookup_mode, vlan_mc_lookup_mode, vlan_profile_index, fid_data, vlan_mbr_data[2];
        vlan_entry_t  vlan_data;
#endif

    uint32  table_idx;
    uint32  data[2], *pData = NULL;
    int32   ret = RT_ERR_FAILED;
    uint32  value, *pValue = NULL;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    ret = table_field_get(unit, table_idx, 0, &value, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_field_get(unit, 0, 0, pValue, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_get(unit, 0, 0, &value, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_get(unit, 0, 100, &value, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        osal_memset(&buff, 0, sizeof(buff));
        /* set fid and msti */
        fid_data = 0x53;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &fid_data, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set unicast lookup mode */
        vlan_uc_lookup_mode = 0x1;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &vlan_uc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set mulicast lookup mode */
        vlan_mc_lookup_mode = 0x0;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &vlan_mc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set profile index */
        vlan_profile_index = 0x5;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &vlan_profile_index, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set member set */
        vlan_mbr_data[0] = 0x12345678;
        vlan_mbr_data[1] = 0x0009abcd;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &vlan_mbr_data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_write(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Get back to check correct or not? */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));

        for (i = 0; i < sizeof(vlan_data)/sizeof(uint32); i++)
            RT_TEST_IS_EQUAL_INT("Wrong! ", vlan_data.entry_data[i], 0);
        ret = table_read(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], fid_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_uc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_profile_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mbr_data[0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[1], vlan_mbr_data[1]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Restore */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        ret = table_write(unit, CYPRESS_VLANt, 30, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_tableFieldGet_test */

int32
hal_api_tableFieldSet_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
        uint32  i;
        uint8   buff[8];
        uint32  vlan_uc_lookup_mode, vlan_mc_lookup_mode, vlan_profile_index, fid_data, vlan_mbr_data[2];
        vlan_entry_t  vlan_data;
#endif
    uint32  table_idx;
    uint32  data[2], *pData = NULL;
    int32   ret = RT_ERR_FAILED;
    uint32  value, *pValue = NULL;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    ret = table_field_set(unit, table_idx, 0, &value, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_field_set(unit, 0, 0, pValue, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_set(unit, 0, 0, &value, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
    ret = table_field_set(unit, 0, 100, &value, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        osal_memset(&buff, 0, sizeof(buff));
        /* set fid and msti */
        fid_data = 0xa6;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &fid_data, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set unicast lookup mode */
        vlan_uc_lookup_mode = 0x0;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &vlan_uc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set mulicast lookup mode */
        vlan_mc_lookup_mode = 0x1;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &vlan_mc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set profile index */
        vlan_profile_index = 0x6;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &vlan_profile_index, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set member set */
        vlan_mbr_data[0] = 0x39648852;
        vlan_mbr_data[1] = 0x000b73fd;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &vlan_mbr_data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_write(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Get back to check correct or not? */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));

        for (i = 0; i < sizeof(vlan_data)/sizeof(uint32); i++)
            RT_TEST_IS_EQUAL_INT("Wrong! ", vlan_data.entry_data[i], 0);
        ret = table_read(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], fid_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_uc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_profile_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mbr_data[0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[1], vlan_mbr_data[1]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Restore */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        ret = table_write(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_tableFieldSet_test */

int32
hal_api_tableRead_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
        uint32  i;
        uint32  vlan_uc_lookup_mode, vlan_mc_lookup_mode, vlan_profile_index, fid_data, vlan_mbr_data[2];
        uint8   buff[8];
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    uint32  size;
    vlan_entry_t  vlan_data;
#endif
    uint32  table_idx;
    uint32  data[2], *pData = NULL;
    int32   ret = RT_ERR_FAILED;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    /* Case 1: table access via INCR & INDR registers */
    ret = table_read(unit, table_idx, 0, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_read(unit, 0, 0, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        ret = table_size_get(unit, CYPRESS_VLANt, &size);
        ret = table_write(unit, CYPRESS_VLANt, size, (uint32 *)&vlan_data);
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8390_50_FAMILY(unit))
    {
        ret = table_size_get(unit, MAPLE_VLANt, &size);
        ret = table_write(unit, MAPLE_VLANt, size, (uint32 *)&vlan_data);
    }
#endif
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        osal_memset(&buff, 0, sizeof(buff));
        /* set fid and msti */
        fid_data = 0xa6;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &fid_data, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set unicast lookup mode */
        vlan_uc_lookup_mode = 0x0;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &vlan_uc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set mulicast lookup mode */
        vlan_mc_lookup_mode = 0x1;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &vlan_mc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set profile index */
        vlan_profile_index = 0x6;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &vlan_profile_index, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set member set */
        vlan_mbr_data[0] = 0x39648852;
        vlan_mbr_data[1] = 0x000b73fd;
        buff[0] = (vlan_mbr_data[1]>>16) & 0xFF;
        buff[1] = (vlan_mbr_data[1]>>8) & 0xFF;
        buff[2] = (vlan_mbr_data[1]>>0) & 0xFF;
        buff[3] = (vlan_mbr_data[0]>>24) & 0xFF;
        buff[4] = (vlan_mbr_data[0]>>16) & 0xFF;
        buff[5] = (vlan_mbr_data[0]>>8) & 0xFF;
        buff[6] = (vlan_mbr_data[0]) & 0xFF;
        ret = table_field_byte_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, buff, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_write(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Get back to check correct or not? */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));

        for (i = 0; i < sizeof(vlan_data)/sizeof(uint32); i++)
            RT_TEST_IS_EQUAL_INT("Wrong! ", vlan_data.entry_data[i], 0);
        ret = table_read(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], fid_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_uc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_profile_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mbr_data[0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[1], vlan_mbr_data[1]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Restore */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        ret = table_write(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_tableRead_test */

int32
hal_api_tableSizeGet_test(uint32 caseNo, uint32 unit)
{
    uint32  table_idx;
    uint32  size, *pSize = NULL;
    int32   ret = RT_ERR_FAILED;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    ret = table_size_get(unit, table_idx, &size);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_size_get(unit, 0, pSize);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
        ret = table_size_get(unit, CYPRESS_VLANt, &size);
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8390_50_FAMILY(unit))
        ret = table_size_get(unit, MAPLE_VLANt, &size);
#endif
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    RT_TEST_IS_EQUAL_INT("Wrong! ", size, 4096);

    return RT_ERR_OK;
} /* end of hal_api_tableSizeGet_test */

int32
hal_api_tableWrite_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390)
        uint32  i;
        uint32  vlan_uc_lookup_mode, vlan_mc_lookup_mode, vlan_profile_index, fid_data, vlan_mbr_data[2];
        uint8   buff[8];
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    uint32  size;
    vlan_entry_t  vlan_data;
#endif
    uint32  table_idx;
    uint32  data[2], *pData = NULL;
    int32   ret = RT_ERR_FAILED;

    table_idx = HAL_GET_MAX_TABLE_IDX(unit);

    ret = table_write(unit, table_idx, 0, &data[0]);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
    ret = table_write(unit, 0, 0, pData);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_NULL_POINTER);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        ret = table_size_get(unit, CYPRESS_VLANt, &size);
        ret = table_write(unit, CYPRESS_VLANt, size, (uint32 *)&vlan_data);
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8390_50_FAMILY(unit))
    {
        ret = table_size_get(unit, MAPLE_VLANt, &size);
        ret = table_write(unit, MAPLE_VLANt, size, (uint32 *)&vlan_data);
    }
#endif
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OUT_OF_RANGE);
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        osal_memset(&buff, 0, sizeof(buff));
        /* set fid and msti */
        fid_data = 0xa6;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &fid_data, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set unicast lookup mode */
        vlan_uc_lookup_mode = 0x0;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &vlan_uc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set mulicast lookup mode */
        vlan_mc_lookup_mode = 0x1;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &vlan_mc_lookup_mode, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set profile index */
        vlan_profile_index = 0x6;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &vlan_profile_index, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        /* set member set */
        vlan_mbr_data[0] = 0x39648852;
        vlan_mbr_data[1] = 0x000b73fd;
        ret = table_field_set(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &vlan_mbr_data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_write(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Get back to check correct or not? */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));

        for (i = 0; i < sizeof(vlan_data)/sizeof(uint32); i++)
            RT_TEST_IS_EQUAL_INT("Wrong! ", vlan_data.entry_data[i], 0);
        ret = table_read(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_FID_MSTItf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], fid_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_UCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_uc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_L2_HASH_KEY_MCtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mc_lookup_mode);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_VLAN_PROFILEtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_profile_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        ret = table_field_get(unit, CYPRESS_VLANt, CYPRESS_VLAN_MBRtf, &data[0], (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[0], vlan_mbr_data[0]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", data[1], vlan_mbr_data[1]);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

        /* Restore */
        osal_memset(&vlan_data, 0, sizeof(vlan_data));
        ret = table_write(unit, CYPRESS_VLANt, 4094, (uint32 *)&vlan_data);
        RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
    }
#endif
    return RT_ERR_OK;
} /* end of hal_api_tableWrite_test*/


/* HAL API test case: miim.c */
int32
hal_api_halMiimReadWrite_test(uint32 caseNo, uint32 unit)
{
    uint32  data, port;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == phy_reg_get(unit, 0, 0, 0, &data)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == phy_reg_set(unit, 0, 0, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, port))
        {
            /* Under Coding */
        }
    }

    return RT_ERR_OK;
} /* end of hal_api_halMiimReadWrite_test */

int32
hal_api_phyAutoNegoAbilityGetSet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, port;
    rtk_port_phy_ability_t  ability, result;
    int32   ret = RT_ERR_FAILED;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == phy_autoNegoAbility_get(unit, 0, &ability)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == phy_autoNegoAbility_set(unit, 0, &ability)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            memset(&ability, 0, sizeof(rtk_port_phy_ability_t));
            memset(&result, 0, sizeof(rtk_port_phy_ability_t));
            ability.Full_10 = 1;
            ability.Half_100 = 1;
            if (HWP_GE_PORT(unit, i))
            {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                /* The 8390/8380 possible connect to new 8218B/8218FB PHY, and the PHY ability 1000Half is RO */
                ability.Half_1000 = 0;
#else
                ability.Half_1000 = 1;
#endif
                ability.Full_1000 = 1;
            }
            ability.AsyFC = 1;

            ret = phy_autoNegoAbility_set(unit, i, &ability);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_autoNegoAbility_get(unit, i, &result);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            if (HWP_GE_PORT(unit, i))
            {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                /* The 8390/8380 possible connect to new 8218B/8218FB PHY, and the PHY ability 1000Half is RO */
                RT_TEST_IS_EQUAL_INT("Wrong! ", result.Half_1000, DISABLED);
#else
                RT_TEST_IS_EQUAL_INT("Wrong! ", result.Half_1000, ENABLED);
#endif
                RT_TEST_IS_EQUAL_INT("Wrong! ", result.Full_1000, ENABLED);
            }
            else
            {
                RT_TEST_IS_EQUAL_INT("Wrong! ", result.Half_1000, DISABLED);
                RT_TEST_IS_EQUAL_INT("Wrong! ", result.Full_1000, DISABLED);
            }
            RT_TEST_IS_EQUAL_INT("Wrong! ", result.Full_10, ENABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", result.Half_100, ENABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", result.AsyFC, ENABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", result.Half_10, DISABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", result.Full_100, DISABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", result.FC, DISABLED);
        }
    }

    return RT_ERR_OK;
} /* end of hal_api_phyAutoNegoAbilityGetSet_test */

int32
hal_api_phyAutoNegoEnableGetSet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, enable, port;
    int32   ret = RT_ERR_FAILED;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == phy_autoNegoEnable_get(unit, 0, &enable)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == phy_autoNegoEnable_set(unit, 0, ENABLED)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            ret = phy_autoNegoEnable_set(unit, i, ENABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_autoNegoEnable_get(unit, i, &enable);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", enable, ENABLED);
            ret = phy_autoNegoEnable_set(unit, i, DISABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_autoNegoEnable_get(unit, i, &enable);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", enable, DISABLED);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyAutoNegoEnableGetSet_test */

int32
hal_api_phyDuplexGetSet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, duplex, port;
    int32   ret = RT_ERR_FAILED;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == phy_duplex_get(unit, 0, &duplex)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == phy_duplex_set(unit, 0, PORT_HALF_DUPLEX)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            ret = phy_duplex_set(unit, i, PORT_HALF_DUPLEX);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_duplex_get(unit, i, &duplex);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", duplex, PORT_HALF_DUPLEX);
            ret = phy_duplex_set(unit, i, PORT_FULL_DUPLEX);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_duplex_get(unit, i, &duplex);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", duplex, PORT_FULL_DUPLEX);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyDuplexGetSet_test */

int32
hal_api_phyEnableSet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, port;
    int32   ret = RT_ERR_FAILED;

    if (RT_ERR_CHIP_NOT_SUPPORTED == phy_enable_set(unit, 0, ENABLED))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            ret = phy_enable_set(unit, i, ENABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_enable_set(unit, i, DISABLED);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyEnableSet_test */

int32
hal_api_phyMediaGetSet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, media, port;
    int32   ret = RT_ERR_FAILED;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == phy_media_get(unit, 0, &media)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == phy_media_set(unit, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i) && HWP_COMBO_PORT(unit, i))
        {
            ret = phy_media_set(unit, i, PORT_MEDIA_COPPER);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_media_get(unit, i, &media);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", media, PORT_MEDIA_COPPER);
            ret = phy_media_set(unit, i, PORT_MEDIA_FIBER);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_media_get(unit, i, &media);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", media, PORT_MEDIA_FIBER);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyMediaGetSet_test */

int32
hal_api_phySpeedGetSet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, speed, port;
    int32   ret = RT_ERR_FAILED;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == phy_speed_get(unit, 0, &speed)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == phy_speed_set(unit, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            ret = phy_speed_set(unit, i, PORT_SPEED_10M);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_speed_get(unit, i, &speed);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", speed, PORT_SPEED_10M);
            ret = phy_speed_set(unit, i, PORT_SPEED_100M);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            ret = phy_speed_get(unit, i, &speed);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            RT_TEST_IS_EQUAL_INT("Wrong! ", speed, PORT_SPEED_100M);
            if (HWP_GE_PORT(unit, i))
            {
                ret = phy_speed_set(unit, i, PORT_SPEED_1000M);
                /* Skip some PHY if it is not supported the 1000M */
                if (ret != RT_ERR_CHIP_NOT_SUPPORTED)
                {
                    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                    ret = phy_speed_get(unit, i, &speed);
                    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
                    RT_TEST_IS_EQUAL_INT("Wrong! ", speed, PORT_SPEED_1000M);
                }
            }
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phySpeedGetSet_test */

int32
hal_api_phyIdentifyFind_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, port;
    rt_phyctrl_t *pPhyCtrl = NULL;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            pPhyCtrl = (rt_phyctrl_t *) phy_identify_find(unit, i);
            RT_TEST_IS_NOT_EQUAL_INT("phy_identify_find(unit 0, port %u) NOT FOUND!!", (int)pPhyCtrl, (int)NULL, i);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyIdentifyFind_test */

int32
hal_api_phyIdentifyOUICheck_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, port;
    int32   ret = RT_ERR_OK;

    if (RT_ERR_CHIP_NOT_SUPPORTED == phy_identify_OUI_check(unit, 0))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            ret = phy_identify_OUI_check(unit, i);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyIdentifyOUICheck_test */

int32
hal_api_phyIdentifyPhyidGet_test(uint32 caseNo, uint32 unit)
{
    uint32  i = 0, model_id, rev_id, port;
    int32   ret = RT_ERR_OK;

    if (RT_ERR_CHIP_NOT_SUPPORTED == phy_identify_phyid_get(unit, 0, &model_id, &rev_id))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, i))
        {
            ret = phy_identify_phyid_get(unit, i, &model_id, &rev_id);
            RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);
            osal_printf("unit %d, port %d: PHY model_id[0x%x], rev_id[0x%x]\n", unit, i, model_id, rev_id);
        }
    }
    return RT_ERR_OK;
} /* end of hal_api_phyIdentifyPhyidGet_test */

int32
hal_api_regReadPerformance_test(uint32 caseNo, uint32 unit)
{
    uint32  value, i = 0, j, reg_value[64], addr;
    int32   ret = RT_ERR_FAILED;
    osal_usecs_t    usec_before = 0, usec_after = 0;

    ret = reg_idxMax_get(unit, &value);
    RT_TEST_IS_EQUAL_INT("Wrong! ", ret, RT_ERR_OK);

    ret = osal_time_usecs_get(&usec_before);
    for (j= 0; j<1000; j++)
    {
        for (i = 0; i < value; i++)
        {
            ret = reg_read(unit, i, &reg_value[0]);
        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access Register Number: %u\n", value);
    osal_printf("Access one register nanosecond (reg_read) = %u\n", (usec_after-usec_before)/value);

    ret = osal_time_usecs_get(&usec_before);
    for (j= 0; j<1000; j++)
    {
        for (i = 0; i < value; i++)
        {
            ret = reg_idx2Addr_get(unit, i, &addr);
            ioal_mem32_read(unit, (addr), &reg_value[0]);

        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access Register Number: %u\n", value);
    osal_printf("Access one register nanosecond (reg_idx2Addr_get) = %u\n", (usec_after-usec_before)/value);

    ret = osal_time_usecs_get(&usec_before);
    for (j= 0; j<1000; j++)
    {
        for (i = 0; i < value; i++)
        {
            ioal_mem32_read(unit, addr, &reg_value[0]);
        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access Register Number: %u\n", value);
    osal_printf("Access one register nanosecond (memory access) = %u\n", (usec_after-usec_before)/value);

    return RT_ERR_OK;
} /* end of hal_api_regReadPerformance_test */

int32
hal_api_miimReadPerformance_test(uint32 caseNo, uint32 unit)
{
#define CHECKBUSY_TIMES (3000)

    uint32  value, i = 0, j, data;
    int32   ret;
    osal_usecs_t    usec_before = 0, usec_after = 0;

    value = 100;
    ret = osal_time_usecs_get(&usec_before);
    if (ret != RT_ERR_OK)
        return ret;
    for (j= 0; j<1000; j++)
    {
        for (i = 0; i < value; i++)
        {
            ret = phy_reg_get(unit, 0, 0, 0, &data);
        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access PHY Register Number: %u\n", value);
    osal_printf("Access one PHY register nanosecond (phy_reg_get) = %u\n", (usec_after-usec_before)/value);

    ret = osal_time_usecs_get(&usec_before);
    for (j= 0; j<1000; j++)
    {
        for (i = 0; i < value; i++)
        {

#if defined(CONFIG_SDK_RTL8380)
#endif
        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access PHY Register Number: %u\n", value);
    osal_printf("Access one PHY register nanosecond (memory access) = %u\n", (usec_after-usec_before)/value);

    return RT_ERR_OK;
} /* end of hal_api_miimReadPerformance_test */
