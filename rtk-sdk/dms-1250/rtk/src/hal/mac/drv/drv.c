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
 * $Revision$
 * $Date$
 *
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/drv/drv.h>
#include <hal/mac/reg.h>
#include <hal/mac/mac_probe.h>
#include <soc/type.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <dal/dal_construct.h>

#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#endif

/*
 * Function Declaration
 */

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
/* Function Name:
 *      table_name_get
 * Description:
 *      Get table name of the specified register index.
 * Input:
 *      unit  - unit id
 *      table   - register index
 * Output:
 *      pData - pointer buffer of table name
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
table_name_get(uint32 unit, uint32 table, char *pData)
{

    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, table=%d", unit, table);
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pChip_driver->pTable_list)
    {
        return RT_ERR_NULL_POINTER;
    }else{
        osal_memcpy(pData, pHalCtrl->pChip_driver->pTable_list[table]->name, TBL_NAME_LEN);
    }

    return RT_ERR_OK;
} /* end of table_name_get */
#endif

#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
/* Function Name:
 *      rtl83xx_mdcSem_callback
 * Description:
 *      Take/Give MDC/MDIO semaphore resource by lower layer in specified device.
 * Input:
 *      unit - unit id
 *      type - semaphore type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 * Note:
 *      The type value 0 mean lock the semaphore; 1 mean unlock the semaphore.
 */
int32
rtl83xx_mdcSem_callback(uint32 unit, uint32 type)
{
    if (type == 0) /* LOCK */
    {
        PHY_SEM_LOCK(unit);
    }
    else
    {
        PHY_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
} /* end of rtl83xx_mdcSem_callback */
#endif


/* Function Name:
 *      drv_phy_smiCmdComplete_wait
 * Description:
 *      MAC access PHY wait command ready
 * Input:
 *      unit  - unit id
 *      ctrl_reg  - SMI access PHY control reg
 *      cmd_mask  - command complete bit mask
 *      fail_mask - fail bit mask
 *      timeout   - timeout time in milli-second
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_phy_smiCmdComplete_wait(int32 unit, uint32 ctrl_reg, uint32 cmd_mask, uint32 fail_mask, uint32 timeout)
{
    osal_usecs_t    t, now, t_wait=0;
    uint32          regVal = 0;
    int32           chkCnt=0;

    osal_time_usecs_get(&t);
    do {
        chkCnt++;
        osal_time_usecs_get(&now);
        if (reg_read(unit, ctrl_reg, &regVal) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }

        if (0 == (regVal & cmd_mask))
        {
            if (regVal & fail_mask)
            {
                //RT_ERR(RT_ERR_SMI, MOD_HAL, "SMI cmd fail");
            #if defined(CONFIG_SDK_RTL8390)
                if (HWP_8390_50_FAMILY(unit))
                {
                    uint32  val;

                    val = 0;
                    reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_CMDf, &val, &ctrl_reg);
                    val = 1; /* write 1 to clear */
                    reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val, &ctrl_reg);
                    reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &ctrl_reg);
                }
            #endif
                return RT_ERR_SMI;
            }
            else
            {
                return RT_ERR_OK;
            }
        }

        t_wait += ((now >= t) ? (now - t) : (0xFFFFFFFF - t + now));
        t = now;
    } while ((t_wait <= timeout) || (chkCnt <= 25));

    if (t_wait > timeout)
    {
        //RT_ERR(RT_ERR_TIMEOUT, MOD_HAL, "Timeout chkCnt = %u", chkCnt);
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}



