/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : PHY 8226 Driver
 *
 * Feature : PHY 8226 Driver
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/nic_rtl8226/nic_rtl8226.h>

/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */



/*
 * Function Declaration
 */




/* Function Name:
 *      MmdPhyRead
 * Description:
 *      Read MMD register
 * Input:
 *      hDevice
 *      dev - MMD
 *      addr - register address
 * Output:
 *      data - register data
 * Return:
 *      SUCCESS
 *      FAILURE
 * Note:
 *      None
 */
BOOLEAN
MmdPhyRead(
    IN  HANDLE hDevice,
    IN  UINT16 dev,
    IN  UINT16 addr,
    OUT UINT16 *data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;
    uint32          data_tmp = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(hDevice.unit)) == NULL)
        return FAILURE;

    if ((hDevice.port >= RTK_MAX_PORT_PER_UNIT) || (pHalCtrl->pPhy_ctrl[hDevice.port] == NULL) || (pHalCtrl->pPhy_ctrl[hDevice.port]->pPhydrv == NULL))
        return FAILURE;

    ret = (pHalCtrl->pPhy_ctrl[hDevice.port]->pPhydrv->fPhydrv_reg_mmd_get(hDevice.unit, hDevice.port, (uint32)dev, (uint32)addr, &data_tmp));

    RT_LOG(LOG_INFO, MOD_PORT, "MmdPhyRead: unit=%u port=%u dev=%u addr=0x%X data=0x%X (rv=%x)\n", hDevice.unit, hDevice.port, dev, addr, data_tmp, ret);
    if (ret == RT_ERR_OK)
    {
        *data = (UINT16)data_tmp;
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}


/* Function Name:
 *      MmdPhyWrite
 * Description:
 *      Write MMD register
 * Input:
 *      hDevice
 *      dev - MMD
 *      addr - register address
 *      data - register data
 * Output:
 *      None
 * Return:
 *      SUCCESS
 *      FAILURE
 * Note:
 *      None
 */
BOOLEAN
MmdPhyWrite(
    IN HANDLE hDevice,
    IN UINT16 dev,
    IN UINT16 addr,
    IN UINT16 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(hDevice.unit)) == NULL)
        return FAILURE;

    if ((hDevice.port >= RTK_MAX_PORT_PER_UNIT) || (pHalCtrl->pPhy_ctrl[hDevice.port] == NULL) || (pHalCtrl->pPhy_ctrl[hDevice.port]->pPhydrv == NULL))
        return FAILURE;

    ret = (pHalCtrl->pPhy_ctrl[hDevice.port]->pPhydrv->fPhydrv_reg_mmd_set(hDevice.unit, hDevice.port, (uint32)dev, (uint32)addr, (uint32)data));

    RT_LOG(LOG_INFO, MOD_PORT, "MmdPhyWrite: unit=%u port=%u dev=%u addr=0x%X data=0x%X (rv=%X)\n", hDevice.unit, hDevice.port, dev, addr, data, ret);
    if (ret == RT_ERR_OK)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}



