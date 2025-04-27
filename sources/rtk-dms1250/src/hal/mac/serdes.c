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
 * Purpose : SerDes indirect access APIs in the SDK.
 *
 * Feature : SerDes indirect access APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#include <hal/mac/serdes.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */
#define SERDES_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(hal_ctrl[unit].sds_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define SERDES_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(hal_ctrl[unit].sds_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */


/* Function Name:
 *      hal_serdes_reg_get
 * Description:
 *      Get SerDes register through indirect access
 * Input:
 *      unit    - unit id
 *      sdsId   - SerDes index
 *      page    - page
 *      reg     - register index/address
 * Output:
 *      pData   - register value
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
int32
hal_serdes_reg_get(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_mac_serdes_read == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    SERDES_SEM_LOCK(unit);
    ret = MACDRV(pHalCtrl)->fMdrv_mac_serdes_read(unit, sdsId, page, reg, pData);
    SERDES_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      hal_serdes_reg_set
 * Description:
 *      Set SerDes register through indirect access
 * Input:
 *      unit    - unit id
 *      sdsId   - SerDes index
 *      page    - page
 *      reg     - register index/address
 *      data    - register value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
int32
hal_serdes_reg_set(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_mac_serdes_write == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    SERDES_SEM_LOCK(unit);
    ret = MACDRV(pHalCtrl)->fMdrv_mac_serdes_write(unit, sdsId, page, reg, data);
    SERDES_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      hal_sds_field_read
 * Description:
 *      Read SerDes field configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - SerDes Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
hal_sds_field_read(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data)
{
    uint32  configVal, len, mask;
    int32   ret;

    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    if (endBit < startBit)
        return RT_ERR_INPUT;

    RT_ERR_CHK(hal_serdes_reg_get(unit, sds, page, reg, &configVal), ret);

    len = endBit - startBit + 1;

    if (32 == len)
        *data = configVal;
    else
    {
        mask = (1 << len) - 1;
        *data = (configVal >> startBit) & mask;
    }

    return RT_ERR_OK;
}   /* end of hal_sds_field_read */

/* Function Name:
 *      hal_sds_field_write
 * Description:
 *      Write SerDes field configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
hal_sds_field_write(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data)
{
    uint32  configVal, len, mask;
    int32   ret;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_mac_serdes_read == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;


    len = endBit - startBit + 1;

    if (endBit < startBit)
        return RT_ERR_INPUT;
    
    SERDES_SEM_LOCK(unit);
    if (32 == len)
        configVal = data;
    else
    {
        mask = (1 << len) - 1;
        
        ret = MACDRV(pHalCtrl)->fMdrv_mac_serdes_read(unit, sds, page, reg, &configVal);

        configVal &= ~(mask << startBit);
        configVal |= ((data & mask)<< startBit);
    }

    ret = MACDRV(pHalCtrl)->fMdrv_mac_serdes_write(unit, sds, page, reg, configVal);
    SERDES_SEM_UNLOCK(unit);

    return ret;
}   /* end of hal_sds_field_write */

/* Function Name:
 *      hal_mac_serdes_rst
 * Description:
 *      Reset MAC serdes.
 * Input:
 *      unit   - unit id
 *      sdsId   - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_OUT_OF_RANGE  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
hal_mac_serdes_rst(uint32 unit, uint32 sdsId)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, sds_no=%d", unit, sdsId);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MACDRV(pHalCtrl)->fMdrv_mac_serdes_rst == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    SERDES_SEM_LOCK(unit);
    ret = MACDRV(pHalCtrl)->fMdrv_mac_serdes_rst(unit, sdsId);
    SERDES_SEM_UNLOCK(unit);

    return ret;
} /* end of hal_mac_serdes_rst */

