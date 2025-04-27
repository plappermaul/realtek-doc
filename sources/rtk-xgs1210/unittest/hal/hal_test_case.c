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
 * $Revision: 83908 $
 * $Date: 2017-12-01 17:06:12 +0800 (Fri, 01 Dec 2017) $
 *
 * Purpose : Definition of HAL test APIs in the SDK
 *
 * Feature : HAL test APIs
 *
 */

/*
 * Include Files
 */
#if defined(__MODEL_USER__)
#include <rtl_glue.h>
#include <vmac.h>
#include <modelTest.h>
#include <rtl_assert.h>
#endif
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <private/drv/swcore/swcore.h>
#include <hal/chipdef/allreg.h>
#include <hal/mac/reg.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#include <rtk/port.h>
#include <hal/hal_test_case.h>

#if defined(__MODEL_USER__)
#else
enum IC_TYPE
{
    IC_TYPE_MIN = 0,    /* to enumerate all IC_TYPE */
    IC_TYPE_REAL = 0,   /* to test FPGA/IC/VERA */
    IC_TYPE_MODEL,      /* to test model code */
    IC_TYPE_MAX         /* to enumerate all IC_TYPE */
};

#define IS_EQUAL_INT(str, v1, v2, _f_, _l_,i) \
    do{if ((v1) != (v2)) { \
        osal_printf("\t%s(%u): %s\n\t%s: %d(0x%08x), %d(0x%08x)\n", _f_, _l_, i==IC_TYPE_REAL?"IC MODE":"MODEL MODE",str, v1, v1,v2,v2); \
        return RT_ERR_FAILED; \
    }}while(0)

#define IS_NOT_EQUAL_INT(str, v1, v2, _f_, _l_,i) \
    do{if ((v1) == (v2)) { \
        osal_printf("\t%s(%u): %s\n\t%s: %d(0x%08x), %d(0x%08x)\n", _f_, _l_,i==IC_TYPE_REAL?"IC MODE":"MODEL MODE", str, v1, v1, v2, v2); \
        return RT_ERR_FAILED; \
    }}while(0)


#endif

/*
 * Function Declaration
 */

/* Port Testing Case */
int32
testPort_1(uint32 caseNo)
{
    uint32  val, temp_field;
    int32   ret;

    ret = reg_read(0, MAC_LINK_STATUS, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg MAC_LINK_STATUS", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, MAC_SPEED_STATUS0, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg MAC_SPEED_STATUS0", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, MAC_SPEED_STATUS1, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg MAC_SPEED_STATUS1", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, MAC_DUPLEX_STATUS, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg MAC_DUPLEX_STATUS", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, MAC_TX_PAUSE_STATUS, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg MAC_TX_PAUSE_STATUS", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, MAC_RX_PAUSE_STATUS, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg MAC_RX_PAUSE_STATUS", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, GLOBAL_MAC_INTERNAL_STATUS, &val);
    IS_EQUAL_INT("FAIL! testPort_1 read reg GLOBAL_MAC_INTERNAL_STATUS", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    /* temp_field should be 0x1234 */
    ret = reg_field_write(0, CPU_TAG_ID_CONTROL, CPU_TAG_ID, 0x1234);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, CPU_TAG_ID_CONTROL, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_field_get(0, CPU_TAG_ID_CONTROL, CPU_TAG_ID, &temp_field, val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);
    IS_EQUAL_INT("Wrong! ", temp_field, 0x1234, __FUNCTION__, __LINE__,1);

#if 0
    /* EN_DEBUG = 1, SEL_DEBUG = 0x00abcdef */
    ret = reg_read(0, GLOBAL_MAC_DEBUG_CONTROL, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_field_write(0, GLOBAL_MAC_DEBUG_CONTROL, EN_DEBUG, 0x1);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_field_write(0, GLOBAL_MAC_DEBUG_CONTROL, SEL_DEBUG, 0x00abcdef);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, GLOBAL_MAC_DEBUG_CONTROL, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_field_get(0, GLOBAL_MAC_DEBUG_CONTROL, EN_DEBUG, &temp_field, val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);
    IS_EQUAL_INT("Wrong! ", temp_field, 0x1, __FUNCTION__, __LINE__,1);

    ret = reg_field_get(0, GLOBAL_MAC_DEBUG_CONTROL, SEL_DEBUG, &temp_field, val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);
    IS_EQUAL_INT("Wrong! ", temp_field, 0x00abcdef, __FUNCTION__, __LINE__,1);

    ret = reg_write(0, GLOBAL_MAC_DEBUG_CONTROL, 0);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);
#endif

    ret = reg_read(0, GLOBAL_MAC_DEBUG_DATA0, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, GLOBAL_MAC_DEBUG_DATA1, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, MAC_FEFI_STATUS, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    ret = reg_read(0, COPPER_AND_FIBER_SELECT_STATUS, &val);
    IS_EQUAL_INT("FAIL! ", ret, RT_ERR_OK, __FUNCTION__, __LINE__, IC_TYPE_MIN);

    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testPort_1 Pass", caseNo);

    return RT_ERR_OK;
}


/* HAL Testing Case */
int32
testHal_1(uint32 caseNo)
{
    uint32      temp_chip_id, temp_chip_rev_id;
    uint32      temp_driver_id, temp_driver_rev_id;
    int32       ret;
    rt_driver_t *pMdriver;

    /* Get chip_id/chip_rev_id and check the value */
    if ((ret = drv_swcore_cid_get(0, &temp_chip_id, &temp_chip_rev_id)) != RT_ERR_OK)
        return ret;

    /* Get driver_id/driver_rev_id and check the value */
    if ((ret = hal_get_driver_id(temp_chip_id, temp_chip_rev_id,
         &temp_driver_id, &temp_driver_rev_id)) != RT_ERR_OK)
        return ret;

    IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_A, __FUNCTION__, __LINE__,1);

    /* Verify the revision compatible mechanism */
    if ((ret = hal_get_driver_id(temp_chip_id, temp_chip_rev_id+2,
         &temp_driver_id, &temp_driver_rev_id)) != RT_ERR_OK)
        return ret;

    IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_A, __FUNCTION__, __LINE__,1);

    if ((pMdriver = hal_find_driver(temp_driver_id, temp_driver_rev_id)) == NULL)
        return ret;
    //Temp remove the code that use RT_DRIVER_RTL8389_A.
    //IS_EQUAL_INT("Wrong! ", pMdriver->type, RT_DRIVER_RTL8389_A, __FUNCTION__, __LINE__,1);
    IS_EQUAL_INT("Wrong! ", pMdriver->vendor_id, REALTEK_VENDOR_ID, __FUNCTION__, __LINE__,1);

    osal_printf("Driver Found as following information:\n");
    osal_printf("Driver type is %d\n", pMdriver->type);
    osal_printf("Driver vendor ID is 0x%x\n", pMdriver->vendor_id);
    osal_printf("Driver ID is 0x%x\n", pMdriver->driver_id);
    osal_printf("Driver revision ID is 0x%x\n", pMdriver->driver_rev_id);
    osal_printf("Driver register list pointer is 0x%x\n", (uint32) pMdriver->pReg_list);
    osal_printf("Driver table list pointer is 0x%x\n", (uint32) pMdriver->pTable_list);

#if 0
    /* Verify the revision compatible mechanism */
    if ((ret = hal_get_driver_id(temp_chip_id, temp_chip_rev_id+3,
         &temp_driver_id, &temp_driver_rev_id)) != RT_ERR_OK)
        return ret;

    IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_D, __FUNCTION__, __LINE__,1);

    /* Verify the revision compatible mechanism */
    if ((ret = hal_get_driver_id(temp_chip_id, temp_chip_rev_id+10,
         &temp_driver_id, &temp_driver_rev_id)) != RT_ERR_OK)
        return ret;

    IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_D, __FUNCTION__, __LINE__,1);
#endif

//    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testHal_1 Pass", caseNo);

    return RT_ERR_OK;
}

int32
testHal_2(uint32 caseNo)
{
    uint32          i;
    uint32          min_port, max_port;
    uint32          unit;
#if defined(__MODEL_USER__)
    int32           ret;
#endif
    hal_control_t   *pHalCtrl;

    unit = 0;
#if defined(__MODEL_USER__)
    if ((ret = hal_init()) != RT_ERR_OK)
        return ret;
#endif

    min_port = TEST_PORT_ID_MIN(unit);
    max_port = TEST_PORT_ID_MAX(unit);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_flags, HAL_CHIP_INITED | HAL_CHIP_ATTACHED, __FUNCTION__, __LINE__,1);
    IS_EQUAL_INT("Wrong! ", pHalCtrl->semaphore, (uint32) NULL, __FUNCTION__, __LINE__,1);
    IS_NOT_EQUAL_INT("Wrong! ", (uint32) pHalCtrl->pChip_driver, (uint32) NULL, __FUNCTION__, __LINE__,1);
    IS_NOT_EQUAL_INT("Wrong! ", (uint32) pHalCtrl->pDev_info, (uint32) NULL, __FUNCTION__, __LINE__,1);

    osal_printf("Device & Port Information as following: \n");
    osal_printf("========================================\n");
    osal_printf("Chip ID: 0x%x, Revision ID = 0x%x\n", pHalCtrl->chip_id, pHalCtrl->chip_rev_id);
    osal_printf("Total Port Number = %d\n", HWP_PORT_COUNT(unit));

    osal_printf("CPU Port: %d \n", pHalCtrl->pDev_info->pPortinfo->cpuPort);
    osal_printf("========================================\n");


    osal_printf("PHY Probe Result as following: \n");
    osal_printf("========================================\n");
    osal_printf("PHY_No  model_id    rev_id  phydrv_idx  \n");
    osal_printf("========================================\n");

    for (i = min_port; ((i < max_port) && HWP_ETHER_PORT(unit, i)); i++)
    {
        if (NULL == pHalCtrl->pPhy_ctrl[i])
        {
            osal_printf("pHalCtrl->pPhy_ctrl[%d] is NULL.\n", i);
            continue;
        }
        osal_printf(" [%02d]     0x%02x      0x%01x          %d\n",
                       i, pHalCtrl->pPhy_ctrl[i]->phy_model_id,
                       pHalCtrl->pPhy_ctrl[i]->phy_rev_id,
                       pHalCtrl->pPhy_ctrl[i]->pPhydrv->phydrv_index);
    }

    osal_printf("========================================\n");

//    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testHal_2 Pass", caseNo);

    return RT_ERR_OK;
}

int32
testComboMedia_1(uint32 caseNo)
{
    uint32  i;
    int32   ret = RT_ERR_FAILED;

    /* Set All combo port media to copper */
    for (i = 24; i < 28; i++)
    {
        if ((ret = rtk_port_phyComboPortMedia_set(0, i, PORT_MEDIA_COPPER)) != RT_ERR_OK)
        {
            osal_printf("set port %d media type FAILED\n", i);
            return ret;
        }
        osal_printf("set port %d media type is [COPPER]\n", i);
    }
    return RT_ERR_OK;
}

int32
testComboMedia_2(uint32 caseNo)
{
    uint32  i;
    int32   ret = RT_ERR_FAILED;

    /* Set All combo port media to fiber */
    for (i = 24; i < 28; i++)
    {
        if ((ret = rtk_port_phyComboPortMedia_set(0, i, PORT_MEDIA_FIBER)) != RT_ERR_OK)
        {
            osal_printf("set port %d media type FAILED\n", i);
            return ret;
        }
        osal_printf("set port %d media type is [FIBER]\n", i);
    }

    return RT_ERR_OK;
}

int32
testComboMedia_3(uint32 caseNo)
{
    uint32  i, media;
    int32   ret = RT_ERR_FAILED;

    /* Get All combo port media information */
    for (i = 24; i < 28; i++)
    {
        if ((ret = rtk_port_phyComboPortMedia_get(0, i, &media)) != RT_ERR_OK)
        {
            osal_printf("get port %d media type FAILED\n", i);
            return ret;
        }
        switch(media)
        {
            case PORT_MEDIA_COPPER:
                osal_printf("port %d media type is [COPPER]\n", i);
                break;
            case PORT_MEDIA_FIBER:
                osal_printf("port %d media type is [FIBER]\n", i);
                break;
            default:
                osal_printf("port %d media type is [UNKNOWN]\n", i);
                break;
        }
    }
    return RT_ERR_OK;
}

int32
testRegField_check(uint32 caseNo)
{
    uint32  i, j, mask;
    uint32  field_num;
    uint32  field_check_value;
    uint32  succ = 0, fail = 0;
    hal_control_t   *pHalCtrl;
    rtk_reg_t       *pReg_list;
    rtk_regField_t  *pFields;

    if ((pHalCtrl = hal_ctrlInfo_get(0)) == NULL)
        return RT_ERR_FAILED;

    for (i = 0; i < REG_LIST_END; i++) /* REG_LIST_END */
    {
        pReg_list = (pHalCtrl->pChip_driver->pReg_list) + i;
        field_num = pReg_list->field_num;
        field_check_value = 0;
        for (j = 0; j < field_num; j++)
        {
            pFields = (pReg_list->pFields) + j;
            if (pFields->len < 32)
                mask = (1 << pFields->len) - 1;
            else
                mask = 0xFFFFFFFF;
            field_check_value |= (mask << pFields->lsp);
        }

        /* Check */
        if (field_check_value != 0xFFFFFFFF)
        {
            RT_LOG(LOG_WARNING, MOD_UNITTEST,  "reg_idx %d wrong", i);
            fail++;
        }
        else
        {
            succ++;
        }
    }

    if ((REG_LIST_END == succ) && (0 == fail))
    {
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testRegField_check Pass", caseNo);
        return RT_ERR_OK;
    }
    else
    {
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "success counter %d; failure counter %d", succ, fail);
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testRegField_check Fail", caseNo);
        return RT_ERR_FAILED;
    }
}

int32
testRegDefaultVal_check(uint32 caseNo)
{
    uint32  i, j, mask, reg_val, reg_dflt_val;
    uint32  field_num;
    uint32  field_check_value;
    uint32  succ = 0, fail = 0;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;
    rtk_reg_t       *pReg_list;
    rtk_regField_t  *pFields;

    if ((pHalCtrl = hal_ctrlInfo_get(0)) == NULL)
        return RT_ERR_FAILED;

    for (i = 0; i < REG_LIST_END; i++) /* REG_LIST_END */
    {
        pReg_list = (pHalCtrl->pChip_driver->pReg_list) + i;
        field_num = pReg_list->field_num;
        field_check_value = 0;
        for (j = 0; j < field_num; j++)
        {
            pFields = (pReg_list->pFields) + j;
            if (pFields->len < 32)
                mask = (1 << pFields->len) - 1;
            else
                mask = 0xFFFFFFFF;

            /* Only check the RW field type */
            if (FIELD_TYPE_RW == pFields->type)
                field_check_value |= (mask << pFields->lsp);
        }

        if ((ret = reg_read(0, i, &reg_val)) != RT_ERR_OK)
        {
            RT_LOG(LOG_WARNING, MOD_UNITTEST,  "reg_idx %d read wrong", i);
            continue;
        }

        reg_dflt_val = pReg_list->default_value;

        /* Check */
        if ((reg_val & field_check_value) != (reg_dflt_val & field_check_value))
        {
            osal_printf("reg_idx %d diff C(0x%08x) vs. D(0x%08x)\n", i, (reg_val & field_check_value), (reg_dflt_val & field_check_value));
            fail++;
        }
        else
        {
            succ++;
        }
    }

    if ((REG_LIST_END == succ) && (0 == fail))
    {
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testRegDefaultVal_check all default", caseNo);
        return RT_ERR_OK;
    }
    else
    {
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "same counter %d; diff. counter %d", succ, fail);
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "caseNo 0x%x: testRegDefaultVal_check not all default", caseNo);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}
