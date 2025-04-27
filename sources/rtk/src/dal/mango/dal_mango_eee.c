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
 * $Revision: 72921 $
 * $Date: 2016-11-01 11:57:28 +0800 (Tue, 01 Nov 2016) $
 *
 * Purpose : Definition those public EEE routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) EEE enable/disable
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_port.h>
#include <dal/mango/dal_mango_eee.h>
#include <dal/dal_phy.h>
#include <rtk/default.h>
#include <rtk/eee.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32       eee_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

/*
 * Macro Declaration
 */
#define EEE_SEM_LOCK(unit)    \
do {\
    if (dal_mango_port_sem_lock(unit) != RT_ERR_OK)\
        return RT_ERR_SEM_LOCK_FAILED;\
} while(0)

#define EEE_SEM_UNLOCK(unit)   \
do {\
    if (dal_mango_port_sem_unlock(unit) != RT_ERR_OK)\
        return RT_ERR_SEM_UNLOCK_FAILED;\
} while(0)


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_eeeMapper_init
 * Description:
 *      Hook eee module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook eee module before calling any eee APIs.
 */
int32
dal_mango_eeeMapper_init(dal_mapper_t *pMapper)
{
    pMapper->eee_init = dal_mango_eee_init;
    pMapper->eee_portEnable_get = dal_mango_eee_portEnable_get;
    pMapper->eee_portEnable_set = dal_mango_eee_portEnable_set;
    pMapper->eee_portState_get = dal_mango_eee_portState_get;
    pMapper->eee_portPowerState_get = dal_mango_eee_portPowerState_get;
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_eee_init
 * Description:
 *      Initialize EEE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize EEE module before calling any EEE APIs.
 */
int32
dal_mango_eee_init(uint32 unit)
{
    rtk_port_t  port;

    RT_LOG(LOG_DEBUG, (MOD_EEE|MOD_DAL), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(eee_init[unit]);
    //eee_init[unit] = INIT_NOT_COMPLETED;

    eee_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /*Disable All Port EEE*/
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            /* Config PHY in port that PHY exist */
            if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
            {
                /*Not check ret value,because some phy may not support*/
                dal_mango_eee_portEnable_set(unit, port, DISABLED);
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_mango_eee_init */

/* Module Name    : EEE                */
/* Sub-module Name: EEE enable/disable */

/* Function Name:
 *      dal_mango_eee_portEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_eee_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(eee_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "unit=%d, port=%d, pEnable=%x"
            , unit, port, pEnable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    EEE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit,
                          MANGO_EEE_PORT_TX_ENr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_PORT_TX_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "reg_array_field_read failure (ret = 0x%x) unit=%d, port=%d", ret, unit, port);
        return ret;
    }

    EEE_SEM_UNLOCK(unit);
    return ret;
} /* end of dal_mango_eee_portEnable_get */

/* Function Name:
 *      dal_mango_eee_portEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_eee_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(eee_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "unit=%d, port=%d, enable=%u"
            , unit, port, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    EEE_SEM_LOCK(unit);

    /* set value to CHIP */
    ret = dal_phy_portEeeEnable_set(unit, port, enable);
    if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
    {
        EEE_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "dal_phy_portEeeEnable_set failure (ret = 0x%x) unit=%d, port=%d", ret, unit, port);
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          MANGO_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_100M_ENf,
                          &enable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          MANGO_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_500M_ENf,
                          &enable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          MANGO_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_1000M_ENf,
                          &enable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          MANGO_MAC_FORCE_MODE_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_10G_ENf,
                          &enable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          MANGO_EEE_PORT_TX_ENr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_PORT_TX_ENf,
                          &enable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          MANGO_EEE_PORT_RX_ENr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_PORT_RX_ENf,
                          &enable)) != RT_ERR_OK)
    {
        EEE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    EEE_SEM_UNLOCK(unit);
    return ret;
} /* end of dal_mango_eee_portEnable_set */

/* Function Name:
 *      dal_mango_eee_portState_get
 * Description:
 *      Get the EEE nego result state of a port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pState - pointer to the EEE port nego result state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_eee_portState_get(uint32 unit, rtk_port_t port, rtk_enable_t *pState)
{
    int32 ret;
    rtk_port_linkStatus_t link;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(eee_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = dal_mango_port_link_get(unit, port, &link)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
        return ret;
    }

    EEE_SEM_LOCK(unit);
    /* get entry from CHIP*/
    *pState = DISABLED;

    if(PORT_LINKUP == link)
    {
        if ((ret = reg_array_field_read(unit,
                              MANGO_MAC_EEE_ABLTYr,
                              port,
                              REG_ARRAY_INDEX_NONE,
                              MANGO_EEE_ABLTYf,
                              pState)) != RT_ERR_OK)
        {
            EEE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
            return ret;
        }
    }

    EEE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "*pState=%x", *pState);

    return RT_ERR_OK;
} /* end of dal_mango_eee_portState_get */

/* Function Name:
 *      dal_mango_eee_portPowerState_get
 * Description:
 *      Get the EEE power state of a port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      direction    - ingress or egress direction
 * Output:
 *      pState - pointer to the EEE port power state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_eee_portPowerState_get(uint32 unit, rtk_port_t port, rtk_eee_direction_t direction, rtk_eee_power_state_t *pState)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "unit=%d port=%d direction=%d", unit, port, direction);

    /* check Init status */
    RT_INIT_CHK(eee_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((direction >= EEE_DIRECTION_END), RT_ERR_INPUT);

    EEE_SEM_LOCK(unit);
    /* get entry from CHIP*/

    if(EEE_DIRECTION_IGR == direction)
    {
        if ((ret = reg_array_field_read(unit,
                  MANGO_EEE_PORT_RX_STSr,
                  port,
                  REG_ARRAY_INDEX_NONE,
                  MANGO_EEE_EEEP_RX_STSf,
                  pState)) != RT_ERR_OK)
        {
            EEE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
            return ret;
        }
        /*read twice to clear latched value*/
        if ((ret = reg_array_field_read(unit,
                  MANGO_EEE_PORT_RX_STSr,
                  port,
                  REG_ARRAY_INDEX_NONE,
                  MANGO_EEE_EEEP_RX_STSf,
                  pState)) != RT_ERR_OK)
        {
            EEE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_read(unit,
                  MANGO_EEE_PORT_TX_STSr,
                  port,
                  REG_ARRAY_INDEX_NONE,
                  MANGO_EEE_EEEP_TX_STSf,
                  pState)) != RT_ERR_OK)
        {
            EEE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
            return ret;
        }
        /*read twice to clear latched value*/
        if ((ret = reg_array_field_read(unit,
                  MANGO_EEE_PORT_TX_STSr,
                  port,
                  REG_ARRAY_INDEX_NONE,
                  MANGO_EEE_EEEP_TX_STSf,
                  pState)) != RT_ERR_OK)
        {
            EEE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_EEE), "");
            return ret;
        }
    }

    EEE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_EEE), "*pState=%x", *pState);

    return RT_ERR_OK;
} /* end of dal_mango_eee_portPowerState_get */

