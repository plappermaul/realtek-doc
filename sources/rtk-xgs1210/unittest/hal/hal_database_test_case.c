/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision: 73148 $
 * $Date: 2016-11-09 11:42:29 +0800 (Wed, 09 Nov 2016) $
 *
 * Purpose : Definition of HAL database test APIs in the SDK
 *
 * Feature : HAL database test APIs
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
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#include <rtk/port.h>
#include <hal/hal_test_case.h>
#include <hal/hal_database_test_case.h>
#include <common/unittest_util.h>
#if defined(CONFIG_SDK_RTL8380)
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#endif


#define RT_REG_TRAVS_BY_UNIT(_unit)\
    for (i = 0; i < _hal_chip_regEnd_get(_unit); i++)

/*
 * Function Declaration
 */

int32
hal_database_halctrl_test(uint32 caseNo, uint32 unit)
{
    uint32          i;
    uint32          max_port;
    hal_control_t   *pHalCtrl;

    osal_printf("Start %s Case\n", __FUNCTION__);

    max_port = HWP_PORT_COUNT(unit);
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_flags, HAL_CHIP_INITED | HAL_CHIP_ATTACHED);
  #if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_id, RTL8390M_CHIP_ID);
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_rev_id, CHIP_REV_ID_A);
    }
  #elif defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_id, RTL8382M_CHIP_ID);
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->chip_rev_id, CHIP_REV_ID_A);
    }
  #endif
    RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->semaphore, (uint32) NULL);
    RT_TEST_IS_NOT_EQUAL_INT("Wrong! ", (uint32) pHalCtrl->pChip_driver, (uint32) NULL);
    RT_TEST_IS_NOT_EQUAL_INT("Wrong! ", (uint32) pHalCtrl->pDev_info, (uint32) NULL);

    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "Device & Port Information as following: ");
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "========================================");
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "Chip ID: 0x%x, Revision ID = 0x%x", pHalCtrl->chip_id, pHalCtrl->chip_rev_id);
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "Total Port Number = %d", HWP_PORT_COUNT(unit));

    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "CPU Port: %d ", HWP_CPU_MACID(unit));
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "========================================");


    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "PHY Probe Result as following: ");
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "========================================");
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "PHY_No  model_id    rev_id  phydrv_idx  ");
    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "========================================");
#if defined(__MODEL_USER__)
  #ifndef CONFIG_SDK_MODEL_MODE
    for (i = 0; i < 24; i++)
    {
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  " [%02d]     0x%02x      0x%01x          %d",
                       i, pHalCtrl->pPhy_ctrl[i]->phy_model_id,
                       pHalCtrl->pPhy_ctrl[i]->phy_rev_id,
                       pHalCtrl->pPhy_ctrl[i]->pPhydrv->phydrv_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->pPhy_ctrl[i]->phy_model_id, PHY_MODEL_ID_RTL8208G);
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->pPhy_ctrl[i]->phy_rev_id, PHY_REV_NO_B);
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->pPhy_ctrl[i]->pPhydrv->phydrv_index, RT_PHYDRV_RTL8208G);
    }

    for (i = 24; i < 28; i++)
    {
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  " [%02d]     0x%02x      0x%01x          %d",
                       i, pHalCtrl->pPhy_ctrl[i]->phy_model_id,
                       pHalCtrl->pPhy_ctrl[i]->phy_rev_id,
                       pHalCtrl->pPhy_ctrl[i]->pPhydrv->phydrv_index);
        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->pPhy_ctrl[i]->phy_rev_id, PHY_REV_NO_A);
    }
  #endif
#elif defined(CONFIG_SDK_FPGA_PLATFORM)
//    for (i = 0; ((i < max_port) && HWP_ETHER_PORT(unit, i)); i++)
//    {
//        RT_LOG(LOG_WARNING, MOD_UNITTEST,  " [%02d]     0x%02x      0x%01x          %d",
//                       i, pHalCtrl->pPhy_ctrl[i]->phy_model_id,
//                       pHalCtrl->pPhy_ctrl[i]->phy_rev_id,
//                       pHalCtrl->pPhy_ctrl[i]->pPhydrv->phydrv_index);
//        RT_TEST_IS_EQUAL_INT("Wrong! ", pHalCtrl->pPhy_ctrl[i]->phy_rev_id, PHY_REV_NO_C);
//    }
#else
    for (i = 0; ((i < max_port) && HWP_ETHER_PORT(unit, i)); i++)
    {
        if (NULL == pHalCtrl->pPhy_ctrl[i])
        {
            RT_LOG(LOG_WARNING, MOD_UNITTEST, "pHalCtrl->pPhy_ctrl[%d] is NULL.", i);
            continue;
        }
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  " [%02d]     0x%02x      0x%01x          %d",
                       i, pHalCtrl->pPhy_ctrl[i]->phy_model_id,
                       pHalCtrl->pPhy_ctrl[i]->phy_rev_id,
                       pHalCtrl->pPhy_ctrl[i]->pPhydrv->phydrv_index);
    }
#endif

    RT_LOG(LOG_WARNING, MOD_UNITTEST,  "========================================");

    return RT_ERR_OK;
}

int32
hal_database_regFieldDef_test(uint32 caseNo, uint32 unit)
{
    uint32  i, j, mask;
    uint32  field_num;
    uint32  field_check_value;
    uint32  succ = 0, fail = 0, field_array = 0;
    hal_control_t   *pHalCtrl;
    rtk_reg_t       *pReg_list;
    rtk_regField_t  *pFields;

    osal_printf("Start %s Case\n", __FUNCTION__);
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;


    RT_REG_TRAVS_BY_UNIT(unit)
    {
        pReg_list = pHalCtrl->pChip_driver->pReg_list[i];
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
            if (pHalCtrl->pChip_driver->pReg_list[i]->bit_offset % 32)
                field_array++;
            else
            {
                RT_LOG(LOG_WARNING, MOD_UNITTEST,  "reg_idx %d wrong", i);
                fail++;
            }
        }
        else
        {
            succ++;
        }
    }
    if ((HWP_CHIP_FAMILY_ID(unit) && (_hal_chip_regEnd_get(unit) == (succ+field_array))) && (0 == fail))
    {
        osal_printf("success counter %d; failure counter %d; field_array %d\n", succ, fail, field_array);
        return RT_ERR_OK;
    }
    else
    {
        osal_printf("success counter %d; failure counter %d; field_array %d\n", succ, fail, field_array);
        return RT_ERR_FAILED;
    }
}

#if defined(CONFIG_SDK_REG_DFLT_VAL)
int32
hal_database_regDefaultVal_test(uint32 caseNo, uint32 unit)
{
    uint32  i, j, mask, reg_val, reg_dflt_val;
    uint32  field_num;
    uint32  field_check_value;
    uint32  succ = 0, fail = 0;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;
    rtk_reg_t       *pReg_list;
    rtk_regField_t  *pFields;

    osal_printf("Start %s Case\n", __FUNCTION__);
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    RT_REG_TRAVS_BY_UNIT(unit)
    {
        pReg_list = pHalCtrl->pChip_driver->pReg_list[i];
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

        if ((ret = reg_read(unit, i, &reg_val)) != RT_ERR_OK)
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
    if (((HWP_CHIP_FAMILY_ID(unit)) && (_hal_chip_regEnd_get(unit) == (succ+field_array))) && (0 == fail))
    {
        return RT_ERR_OK;
    }
    else
    {
        osal_printf("success counter %d; failure counter %d\n", succ, fail);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}
#endif

int32
hal_database_tableFieldDef_test(uint32 caseNo, uint32 unit)
{
    uint32  i, j, mask;
    uint32  field_num;
    //uint32  field_check_value;
    uint32  succ = 0, check = 0;
    hal_control_t       *pHalCtrl;
    rtk_table_t         *pTable_list;
    rtk_tableField_t    *pFields;
    uint32  table_entry_size = 0;
    uint32  *pTableBuffer = NULL;

    osal_printf("Start %s Case\n", __FUNCTION__);
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    RT_REG_TRAVS_BY_UNIT(unit)
    {
        pTable_list = pHalCtrl->pChip_driver->pTable_list[i];
        field_num = pTable_list->field_num;
        //field_check_value = 0;
        table_entry_size = pTable_list->datareg_num;
        RT_LOG(LOG_WARNING, MOD_UNITTEST,  "table_index[%d] entry_size is %d", i, table_entry_size);

        if ((pTableBuffer = (uint32 *) osal_alloc(table_entry_size * sizeof(uint32))) == NULL)
            return RT_ERR_FAILED;
        osal_memset(pTableBuffer, 0, table_entry_size * sizeof(uint32));

        for (j = 0; j < field_num; j++)
        {
            pFields = (pTable_list->pFields) + j;
            if (pFields->len < 32)
                mask = (1 << pFields->len) - 1;
            else
                mask = 0xFFFFFFFF;
            //field_check_value |= (mask << pFields->lsp);
            table_field_set(unit, i, j, &mask, pTableBuffer);
            RT_LOG(LOG_WARNING, MOD_UNITTEST,  "table_index[%d].field[%d] lsp=%d, len=%d", i, j, pFields->lsp, pFields->len);
        }

        /* Check */
        for (j = 0; j < table_entry_size; j++)
        {
            if (*(pTableBuffer+j) != 0xFFFFFFFF)
            {
                RT_LOG(LOG_WARNING, MOD_UNITTEST,  "====== table_idx[%d] check: some bits undefined!!", i);
                check++;
                break;
            }
        }
        if (j == table_entry_size)
            succ++;
    }
    if (((HWP_CHIP_FAMILY_ID(unit)) && (_hal_chip_regEnd_get(unit) == (succ))) && (0 == check))
    {
        osal_printf("success counter %d; checking counter %d\n", succ, check);
        return RT_ERR_OK;
    }
    else
    {
        osal_printf("success counter %d; checking counter %d\n", succ, check);
        return RT_ERR_FAILED;
    }
}

uint32 _hal_chip_regEnd_get(uint32 unit)
{

    uint32 list_end = 0;

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_30_FAMILY(unit))
        list_end = MAPLE_REG_LIST_END;
#endif
#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
        list_end = CYPRESS_REG_LIST_END;
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
        list_end = LONGAN_REG_LIST_END;

#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
        list_end = MANGO_REG_LIST_END;
#endif

return list_end;

}
