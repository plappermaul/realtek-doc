/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision: 73171 $
 * $Date: 2016-11-09 17:03:46 +0800 (Wed, 09 Nov 2016) $
 *
 * Purpose : Definition of HAL mechanism test APIs in the SDK
 *
 * Feature : HAL mechanism test APIs
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
#include <private/drv/swcore/swcore.h>
#include <hal/chipdef/allreg.h>
#include <hal/mac/reg.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#include <rtk/port.h>
#include <hal/hal_test_case.h>
#include <common/unittest_util.h>
#if defined(CONFIG_SDK_RTL8380)
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#endif

/*
 * Function Declaration
 */

/* HAL Testing Case */
int32
hal_mechanism_probe_test(uint32 caseNo, uint32 unit)
{
    uint32      temp_chip_id, temp_chip_rev_id;
    uint32      temp_driver_id, temp_driver_rev_id;
    int32       ret;
    rt_driver_t *pMdriver;

    osal_printf("Start %s Case\n", __FUNCTION__);

    /* Get chip_id/chip_rev_id and check the value */
    if ((ret = drv_swcore_cid_get(unit, &temp_chip_id, &temp_chip_rev_id)) != RT_ERR_OK)
        return ret;

    /* Get driver_id/driver_rev_id and check the value */
    if ((ret = hal_get_driver_id(temp_chip_id, temp_chip_rev_id,
         &temp_driver_id, &temp_driver_rev_id)) != RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_RTL8380)
    RT_TEST_IS_EQUAL_INT("Wrong! ", temp_driver_id, RTL8380M_CHIP_ID);
    RT_TEST_IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_A);
#endif

    /* Verify the revision compatible mechanism */
    if ((ret = hal_get_driver_id(temp_chip_id, temp_chip_rev_id+2,
         &temp_driver_id, &temp_driver_rev_id)) != RT_ERR_OK)
        return ret;
#if defined (CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", temp_driver_id, RTL8390M_CHIP_ID);
        RT_TEST_IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_A);
    }
#endif
#if defined (CONFIG_SDK_RTL8390)
    if(HWP_8380_30_FAMILY(unit))
    {

        RT_TEST_IS_EQUAL_INT("Wrong! ", temp_driver_id, RTL8380M_CHIP_ID);
        RT_TEST_IS_EQUAL_INT("Wrong! ", temp_driver_rev_id, CHIP_REV_ID_A);
    }
#endif
    if ((pMdriver = hal_find_driver(temp_driver_id, temp_driver_rev_id)) == NULL)
        return ret;
#if defined (CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", pMdriver->type, RT_DRIVER_RTL8390_A);
    }
#endif
#if defined (CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        RT_TEST_IS_EQUAL_INT("Wrong! ", pMdriver->type, RT_DRIVER_RTL8380_A);
    }
#endif
    RT_TEST_IS_EQUAL_INT("Wrong! ", pMdriver->vendor_id, REALTEK_VENDOR_ID);

    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver Found as following information:");
    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver type is %d", pMdriver->type);
    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver vendor ID is 0x%x", pMdriver->vendor_id);
    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver ID is 0x%x", pMdriver->driver_id);
    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver revision ID is 0x%x", pMdriver->driver_rev_id);
    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver register list pointer is 0x%x", (uint32) pMdriver->pReg_list);
    RT_LOG(LOG_WARNING, MOD_UNITTEST, "Driver table list pointer is 0x%x", (uint32) pMdriver->pTable_list);

    return RT_ERR_OK;
}

