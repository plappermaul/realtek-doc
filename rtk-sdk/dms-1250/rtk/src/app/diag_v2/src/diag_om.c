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
 * Purpose : Define diag shell database.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag shell database.
 */

#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <hwp/hw_profile.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
#endif

static uint32  current_unit_id = DIAG_OM_UNIT_ID_DEFAULT;
compatible_status_t  backwardCompatible_flag = DIAG_NOT_BACKWARD_COMPATIBLE;

static rtk_switch_devInfo_t chip_devInfo;

int32
diag_om_get_unitId(int *unit)
{
    if (NULL == unit)
        return RT_ERR_FAILED;

    *unit = current_unit_id;
    return RT_ERR_OK;
} /* end of diag_get_chip_id */

int32
diag_om_set_unitId(int unit)
{
    if (unit > DIAG_OM_UNIT_ID_MAX)
        return RT_ERR_FAILED;

    current_unit_id = unit;
    return RT_ERR_OK;
} /* end of diag_set_chip_id */

int32
diag_om_get_deviceInfo(uint32 unit, rtk_switch_devInfo_t *pDevInfo)
{
    if (unit > DIAG_OM_UNIT_ID_MAX)
        return RT_ERR_FAILED;

    if (NULL == pDevInfo)
        return RT_ERR_FAILED;

    if (unit == current_unit_id)
    {
        osal_memcpy(pDevInfo, &chip_devInfo, sizeof(rtk_switch_devInfo_t));
    }
    else
    {
        osal_memset(pDevInfo, 0, sizeof(rtk_switch_devInfo_t));
        if (rtk_switch_deviceInfo_get(unit, pDevInfo) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of diag_om_get_deviceInfo */

int32
diag_om_set_deviceInfo(uint32 unit)
{
    int32                rv;
    rtk_switch_devInfo_t devInfo;

    if (unit > DIAG_OM_UNIT_ID_MAX)
        return RT_ERR_FAILED;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    if ((rv = rtk_switch_deviceInfo_get(unit, &devInfo)) != RT_ERR_OK)
    {
        diag_util_printf("Get information of unit %u fail. [%d]\n", unit, rv);
        return RT_ERR_FAILED;
    }

    osal_memcpy(&chip_devInfo, &devInfo, sizeof(rtk_switch_devInfo_t));
    return RT_ERR_OK;
} /* end of diag_om_set_deviceInfo */

int32
diag_om_get_chipId(uint32 unit, uint32 chipId)
{
    if (unit > DIAG_OM_UNIT_ID_MAX)
        return FALSE;

    if (unit != current_unit_id)
    {
        return FALSE;
    }

    if (chipId == chip_devInfo.chipId)
        return TRUE;
    else
        return FALSE;
} /* end of diag_om_get_chipId */

int32
diag_om_get_familyId(uint32 unit, uint32 familyId)
{
    if (unit > DIAG_OM_UNIT_ID_MAX)
        return FALSE;

    if (unit != current_unit_id)
    {
        return FALSE;
    }

    if (familyId == chip_devInfo.familyId)
        return TRUE;
    else
        return FALSE;
} /* end of diag_om_get_familyId */

int32
diag_om_get_testChipId(uint32 unit, uint32 chipId)
{
    if (unit > DIAG_OM_UNIT_ID_MAX)
        return FALSE;

    if (unit != current_unit_id)
    {
        return FALSE;
    }

    if ((chipId & 0xffff) == (chip_devInfo.chipId & 0xffff))
        return TRUE;
    else
        return FALSE;
} /* end of diag_om_get_testChipId */


int32
diag_om_get_firstAvailableUnit(uint32 *unit)
{
    int32                   nextUnit;

    if (hwp_unit_get_next((-1), &nextUnit) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    *unit = (uint32)nextUnit;
    return RT_ERR_OK;
}/* end of diag_om_get_firstAvailableUnit */

/*
 * Get diag shell prompt string
 */
int32
diag_om_get_promptString(uint8 *buf, uint32 bufSz, uint32 unit)
{
    return snprintf((char *)buf, bufSz, "RTK.%u> ", unit);
} /* end of diag_om_get_prompt */

int32
diag_om_set_backwardCompatible(compatible_status_t status)
{
    if (status > DIAG_BACKWARD_COMPATIBLE_END)
        return RT_ERR_FAILED;

    backwardCompatible_flag = status;
    return RT_ERR_OK;
} /* end of diag_om_get_deviceInfo */

