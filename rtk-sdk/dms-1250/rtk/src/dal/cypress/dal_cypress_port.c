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
 * Purpose : Definition those public Port APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
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
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <dal/dal_mapper.h>
#include <dal/dal_phy.h>
#include <dal/cypress/dal_cypress_port.h>
#include <dal/cypress/dal_cypress_vlan.h>
#include <dal/dal_linkMon.h>
#include <dal/dal_mgmt.h>
#include <rtk/port.h>
#include <rtk/default.h>
#include <ioal/mem32.h>


/*
 * Symbol Definition
 */
typedef struct dal_cypress_mac_info_s {
    uint8   admin_enable[RTK_MAX_NUM_OF_PORTS];
    rtk_port_swMacPollPhyStatus_t   swMacPollPhyStatus[RTK_MAX_NUM_OF_PORTS]; /* SW-MAC-POLL-PHY */
} dal_cypress_mac_info_t;

/*
 * Data Declaration
 */
static uint32               port_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         port_sem[RTK_MAX_NUM_OF_UNIT];

static dal_cypress_mac_info_t   *pMac_info[RTK_MAX_NUM_OF_UNIT];
static dal_link_change_callback_f   link_change_callback_f[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* vlan semaphore handling */
#define PORT_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(port_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define PORT_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(port_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


/*
 * Function Declaration
 */
#if !defined(__BOOTLOADER__)
static int32 _dal_cypress_port_init_config(uint32 unit);
#endif/* !defined(__BOOTLOADER__) */

int32 _dal_cypress_port_swMacPollPhyEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *penable);

/* Module Name    : port     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_cypress_portMapper_init
 * Description:
 *      Hook port module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook port module before calling any port APIs.
 */
int32
dal_cypress_portMapper_init(dal_mapper_t *pMapper)
{
#ifndef __BOOTLOADER__
    pMapper->port_init = dal_cypress_port_init;
    pMapper->port_link_get = dal_cypress_port_link_get;
    pMapper->port_speedDuplex_get = dal_cypress_port_speedDuplex_get;
    pMapper->port_flowctrl_get = dal_cypress_port_flowctrl_get;
    pMapper->port_phyAutoNegoEnable_set = dal_cypress_port_phyAutoNegoEnable_set;
    pMapper->port_phyForceModeAbility_set = dal_cypress_port_phyForceModeAbility_set;
    pMapper->port_cpuPortId_get = dal_cypress_port_cpuPortId_get;
    pMapper->port_isolation_get = dal_cypress_port_isolation_get;
    pMapper->port_isolation_set = dal_cypress_port_isolation_set;
    pMapper->port_isolation_add = dal_cypress_port_isolation_add;
    pMapper->port_isolation_del = dal_cypress_port_isolation_del;
    pMapper->port_adminEnable_get = dal_cypress_port_adminEnable_get;
    pMapper->port_adminEnable_set = dal_cypress_port_adminEnable_set;
    pMapper->port_backpressureEnable_get = dal_cypress_port_backpressureEnable_get;
    pMapper->port_backpressureEnable_set = dal_cypress_port_backpressureEnable_set;
    pMapper->port_linkChange_register = dal_cypress_port_linkChange_register;
    pMapper->port_linkChange_unregister = dal_cypress_port_linkChange_unregister;
    pMapper->port_txEnable_get = dal_cypress_port_txEnable_get;
    pMapper->port_txEnable_set = dal_cypress_port_txEnable_set;
    pMapper->port_rxEnable_get = dal_cypress_port_rxEnable_get;
    pMapper->port_rxEnable_set = dal_cypress_port_rxEnable_set;
    pMapper->port_specialCongest_get = dal_cypress_port_specialCongest_get;
    pMapper->port_specialCongest_set = dal_cypress_port_specialCongest_set;
    pMapper->port_flowCtrlEnable_get = dal_cypress_port_flowCtrlEnable_get;
    pMapper->port_flowCtrlEnable_set = dal_cypress_port_flowCtrlEnable_set;
    pMapper->port_linkMedia_get = dal_cypress_port_linkMedia_get;
    pMapper->port_vlanBasedIsolationEntry_get = dal_cypress_port_vlanBasedIsolationEntry_get;
    pMapper->port_vlanBasedIsolationEntry_set = dal_cypress_port_vlanBasedIsolationEntry_set;
    pMapper->port_vlanBasedIsolation_vlanSource_get = dal_cypress_port_vlanBasedIsolation_vlanSource_get;
    pMapper->port_vlanBasedIsolation_vlanSource_set = dal_cypress_port_vlanBasedIsolation_vlanSource_set;
    pMapper->port_fiberOAMLoopBackEnable_set = dal_cypress_port_fiberOAMLoopBackEnable_set;
#else
    /* mapper for U-Boot */
    pMapper->port_isolation_set = dal_cypress_port_isolation_set;
#endif
    return RT_ERR_OK;

}

/*
 * Force Pause
 */
void
_dal_cypress_port_macForceFc_set(int32 unit, rtk_port_t port, rtk_enable_t forceFc, rtk_enable_t txPause, rtk_enable_t rxPause)
{
    int ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d forceFC=%u txP=%u rxP=%u", unit, port, forceFc, txPause, rxPause);

    val = (forceFc == ENABLED) ? 1 : 0;
    ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_MAC_FORCE_FC_ENf, &val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
    }

    if (forceFc == ENABLED)
    {
        val = (txPause == ENABLED) ? 1 : 0;
        ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_TX_PAUSE_ENf, &val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        }

        val = (rxPause == ENABLED) ? 1 : 0;
        ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_RX_PAUSE_ENf, &val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        }
    }
}


/* Function Name:
 *      dal_cypress_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_cypress_port_init(uint32 unit)
{
#if !defined(__BOOTLOADER__)
    int32   ret;
#endif/* !defined(__BOOTLOADER__) */

    RT_INIT_REENTRY_CHK(port_init[unit]);
    port_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    port_sem[unit] = osal_sem_mutex_create();
    if (0 == port_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    pMac_info[unit] = (dal_cypress_mac_info_t *)osal_alloc(sizeof(dal_cypress_mac_info_t));
    if (NULL == pMac_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pMac_info[unit], 0, sizeof(dal_cypress_mac_info_t));

    /* init callback function for link change */
    link_change_callback_f[unit] = 0;

    /* set init flag to complete init */
    port_init[unit] = INIT_COMPLETED;

#if !defined(__BOOTLOADER__)
    if (( ret = _dal_cypress_port_init_config(unit)) != RT_ERR_OK)
    {
        port_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pMac_info[unit]);
        pMac_info[unit] = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port default configuration init failed");
        return ret;
    }
#endif/* !defined(__BOOTLOADER__) */

    return RT_ERR_OK;
}/* end of dal_cypress_port_init */

/* Function Name:
 *      dal_cypress_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 * Output:
 *      pStatus              - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
dal_cypress_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  val;
    rtk_enable_t    swMacPollPhyEn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    if ((_dal_cypress_port_swMacPollPhyEnable_get(unit, port, &swMacPollPhyEn) == RT_ERR_OK) && (swMacPollPhyEn == ENABLED))
    {
        if (PHY_RESL_STS_LINK(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus) == PORT_LINKUP)
            *pStatus = PORT_LINKUP;
        else
            *pStatus = PORT_LINKDOWN;

        PORT_SEM_UNLOCK(unit);

        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pStatus=%d", *pStatus);
        return RT_ERR_OK;
    }


    /* get value from CHIP*/
    /* read twice for latency value */
    ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_STSr, port, REG_ARRAY_INDEX_NONE, CYPRESS_LINK_STSf, &val);
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_LINK_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_LINK_STSf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (TRUE == val)
    {
        *pStatus = PORT_LINKUP;
    }
    else
    {
        *pStatus = PORT_LINKDOWN;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pStatus=%d", *pStatus);

    return RT_ERR_OK;
}/* end of dal_cypress_port_link_get */

/* Function Name:
 *      dal_cypress_port_txEnable_set
 * Description:
 *      Set the TX enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of TX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_cypress_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    PORT_SEM_LOCK(unit);

    /* programming value on CHIP*/
    if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_TX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_cypress_port_txEnable_set */

/* Function Name:
 *      dal_cypress_port_rxEnable_set
 * Description:
 *      Set the RX enable status of the specific port
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      enable         - enable status of RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_cypress_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    PORT_SEM_LOCK(unit);

    /* programming value on CHIP*/
    if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_RX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_cypress_port_rxEnable_set */


/* Function Name:
 *      dal_cypress_port_txEnable_get
 * Description:
 *      Get the TX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port TX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_cypress_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                      CYPRESS_MAC_PORT_CTRLr,
                      port,
                      REG_ARRAY_INDEX_NONE,
                      CYPRESS_TX_ENf,
                      &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_cypress_port_txEnable_get */

/* Function Name:
 *      dal_cypress_port_rxEnable_get
 * Description:
 *      Get the RX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_cypress_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                      CYPRESS_MAC_PORT_CTRLr,
                      port,
                      REG_ARRAY_INDEX_NONE,
                      CYPRESS_RX_ENf,
                      &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_cypress_port_rxEnable_get */

/* Function Name:
 *      dal_cypress_port_specialCongest_get
 * Description:
 *      Get the congest seconds of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pSecond - pointer to congest timer (seconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
dal_cypress_port_specialCongest_get(uint32 unit, rtk_port_t port, uint32* pSecond)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_SC_P_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_port_specialCongest_get */

/* Function Name:
 *      dal_cypress_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, second=%d", unit, port, second);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((second > RTK_PORT_SPEC_CONGEST_TIME_MAX), RT_ERR_OUT_OF_RANGE);

    PORT_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_SC_P_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMTf, &second)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, CYPRESS_SC_P_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMT_Hf, &second)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_port_specialCongest_set */

/* Function Name:
 *      dal_cypress_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 * Output:
 *      pSpeed               - pointer to the port speed
 *      pDuplex              - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN   - link down port status
 * Note:
 *      1. The speed type of the port is as following:
 *         - PORT_SPEED_10M
 *         - PORT_SPEED_100M
 *         - PORT_SPEED_1000M
 *
 *      2. The duplex mode of the port is as following:
 *         - HALF_DUPLEX
 *         - FULL_DUPLEX
 */
int32
dal_cypress_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32   ret;
    uint32  speed;
    uint32  duplex;
    rtk_port_linkStatus_t  link_status;
    uint32  sts_500M;
    rtk_enable_t    swMacPollPhyEn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Check Link status */
    if ((ret = dal_cypress_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* if link status is down, should not process anymore and return error */
    if (PORT_LINKDOWN == link_status)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port link down");
        return RT_ERR_PORT_LINKDOWN;
    }

    PORT_SEM_LOCK(unit);

    if ((_dal_cypress_port_swMacPollPhyEnable_get(unit, port, &swMacPollPhyEn) == RT_ERR_OK) && (swMacPollPhyEn == ENABLED))
    {
        *pDuplex = PHY_RESL_STS_DUPLEX(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        *pSpeed = PHY_RESL_STS_SPEED(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        PORT_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }

    /* get speed value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_LINK_SPD_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_SPD_STSf,
                          &speed)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* get duplex value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_LINK_DUP_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_DUP_STSf,
                          &duplex)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_LINK_500M_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_LINK_500M_STSf,
                          &sts_500M)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == duplex)
    {
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else
    {
        *pDuplex = PORT_HALF_DUPLEX;
    }

    if (1 == sts_500M)
    {
        *pSpeed = PORT_SPEED_500M;
    }
    else
    {
        /* extract port's speed value */
        switch (speed)
        {
            case 0x0:
                *pSpeed = PORT_SPEED_10M;
                break;
            case 0x1:
                *pSpeed = PORT_SPEED_100M;
                break;
            case 0x2:
                *pSpeed = PORT_SPEED_1000M;
                break;
            case 0x3:
                *pSpeed = PORT_SPEED_10G;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pSpeed=%d, pDuplex=%d",
           *pSpeed, *pDuplex);

    return RT_ERR_OK;
}/* end of dal_cypress_port_speedDuplex_get */

/* Function Name:
 *      dal_cypress_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      unit         - unit id
 *      port         - port id
 * Output:
 *      pTxStatus     - pointer to the negotiation result of the Tx flow control
 *      pRxStatus     - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN   - link down port status
 * Note:
 *      None
 */
int32
dal_cypress_port_flowctrl_get(
    uint32            unit,
    rtk_port_t        port,
    uint32            *pTxStatus,
    uint32            *pRxStatus)
{
    int32   ret;
    uint32  rxPause, txPause;
    rtk_port_linkStatus_t  link_status;
    rtk_enable_t        swMacPollPhyEn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Check Link status */
    if ((ret = dal_cypress_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* if link status is down, should not process anymore and return error */
    if (PORT_LINKDOWN == link_status)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port link down");
        return RT_ERR_PORT_LINKDOWN;
    }

    PORT_SEM_LOCK(unit);

    if ((_dal_cypress_port_swMacPollPhyEnable_get(unit, port, &swMacPollPhyEn) == RT_ERR_OK) && (swMacPollPhyEn == ENABLED))
    {
        *pTxStatus = PHY_RESL_STS_TXPAUSE(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        *pRxStatus = PHY_RESL_STS_RXPAUSE(pMac_info[unit]->swMacPollPhyStatus[port].reslStatus);
        PORT_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }

    /* get tx pause value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_TX_PAUSE_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_TX_PAUSE_STSf,
                          &txPause)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* get rx pause value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_RX_PAUSE_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_RX_PAUSE_STSf,
                          &rxPause)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if(1 == txPause)
    {
        *pTxStatus = ENABLED;
    }
    else
    {
        *pTxStatus = DISABLED;
    }

    if(1 == rxPause)
    {
        *pRxStatus = ENABLED;
    }
    else
    {
        *pRxStatus = DISABLED;
    }


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pTxStatus=%d, pRxStatus=%d",
           *pTxStatus, *pRxStatus);

    return RT_ERR_OK;
}/* end of dal_cypress_port_flowctrl_get */

/* Function Name:
 *      dal_cypress_port_cpuPortId_get
 * Description:
 *      Get CPU port id of the specific unit
 * Input:
 *      unit                 - unit id
 * Output:
 *      pPort               - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    *pPort = HWP_CPU_MACID(unit);

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pPort=%d", *pPort);

    return RT_ERR_OK;
}/* end of dal_cypress_port_cpuPortId_get */

/* Function Name:
 *      dal_cypress_port_isolation_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pPortmask          - pointer to the portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      1. Default value of each port is 1
 *      2. Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
dal_cypress_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_P_ISO_MBR_0f
                        , &pPortmask->bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_P_ISO_MBR_1f
                        , &pPortmask->bits[1])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pPortmask, 0),
        RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    return RT_ERR_OK;
}/* end of dal_cypress_port_isolation_get */

/* Function Name:
 *      dal_cypress_port_isolation_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pPortmask       - pointer to the portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      1. Default value of each port is 1
 *      2. Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
dal_cypress_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pPortmask), RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, pmsk0=0x%x, pmsk1=0x%x",
           unit, port,
           RTK_PORTMASK_WORD_GET(*pPortmask, 0),
           RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    PORT_SEM_LOCK(unit);

    /* write isolation mask to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_P_ISO_MBR_0f
                        , &pPortmask->bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_P_ISO_MBR_1f
                        , &pPortmask->bits[1])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_isolation_add
 * Description:
 *      Add an isolation port to the certain port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      iso_port      - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. Default value of each port is 1
 *      2. Port and iso_port will be isolated when this API is called
 *      3. The iso_port to the relative portmask bit will be set to 1
 *      4. This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_cypress_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
{
    int32 ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, iso_port=%d",
           unit, port, iso_port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, iso_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_cypress_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, iso_port);

    ret = dal_cypress_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_isolation_del
 * Description:
 *      Delete an existing isolation port of the certain port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      iso_port      - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. Default value of each port is 1
 *      2. Isolated status between the port and the iso_port is removed when this API is called
 *      3. The iso_port to the relative portmask bit will be set to 0
 *      4. This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_cypress_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
{
    int32 ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, iso_port=%d",
           unit, port, iso_port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, iso_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_cypress_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, iso_port);

    ret = dal_cypress_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      pEnable           - pointer to the port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    *pEnable = pMac_info[unit]->admin_enable[port];

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_port_adminEnable_get */

/* Function Name:
 *      dal_cypress_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      enable             - port admin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, port admin=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);
    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);
    PORT_SEM_LOCK(unit);

    if (ENABLED == enable)
    {
        value = 0x1;
        /* programming value on CHIP*/
        if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_TX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_RX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
    }

    if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
    {
        if ((ret = dal_phy_portEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
    }

    if (DISABLED == enable)
    {
        /* programming value on CHIP*/
        value = 0x0;
        if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_TX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_RX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
    }

    pMac_info[unit]->admin_enable[port] = enable;
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_port_adminEnable_set */


/* Function Name:
 *      dal_cypress_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable        - pointer to the enable status of backpressure in half duplex mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2. Used to support backpressure in half mode.
 */
int32
dal_cypress_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_BKPRES_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_port_backpressureEnable_get */

/* Function Name:
 *      dal_cypress_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of backpressure in half duplex mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2. Used to support backpressure in half mode.
 */
int32
dal_cypress_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          CYPRESS_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_BKPRES_ENf,
                          &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_port_backpressureEnable_set */

/* Function Name:
 *      dal_cypress_port_linkChange_register
 * Description:
 *      Register callback function for notification of link change
 * Input:
 *      unit           - unit id
 *      link_change_callback      - Callback function for link change
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_cypress_port_linkChange_register(uint32 unit, dal_link_change_callback_f link_change_callback)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, link_change_callback=%x",
           unit, link_change_callback);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == link_change_callback), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    link_change_callback_f[unit] = link_change_callback;

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* End of dal_cypress_port_linkChange_register */

/* Function Name:
 *      dal_cypress_port_linkChange_unregister
 * Description:
 *      Unregister callback function for notification of link change
 * Input:
 *      unit           - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 */
int32
dal_cypress_port_linkChange_unregister(uint32 unit)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d",
           unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */

    PORT_SEM_LOCK(unit);

    link_change_callback_f[unit] = NULL;

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* End of dal_cypress_port_linkChange_unregister */

/* Function Name:
 *      _dal_cypress_port_init_config
 * Description:
 *      Initialize default configuration for port module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
#if !defined(__BOOTLOADER__)
static int32
_dal_cypress_port_init_config(uint32 unit)
{
    hal_control_t   *pHalCtrl;
    rtk_portmask_t  portmask;
    rtk_port_t      port;
    int32           ret;

    /* Some serdes control register value seting for RTL8218 + RTL8389M/RTL8389L */
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    HWP_GET_ALL_PORTMASK(unit, portmask);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        /* Config MAC */
        if (HWP_PORT_EXIST(unit, port))
        {
            if ((ret = dal_cypress_port_isolation_set(unit, port, &portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port init set port isolation portmask failed");
                return ret;
            }
        }

        /* Config PHY in port that PHY exist */
        if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
        {
            if ((ret = dal_cypress_port_adminEnable_set(unit, port, RTK_DEFAULT_PORT_ADMIN_ENABLE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port%u init enable port failed", port);
                /* continue the process */
                continue;
            }
        }
    }//for(port)

    return RT_ERR_OK;
}
#endif /* !defined(__BOOTLOADER__) */

/* Function Name:
 *      dal_cypress_port_vlanBasedIsolationEntry_get
 * Description:
 *      Get VLAN-based port isolation entry
 * Input:
 *      unit   - unit id
 *      index  - index id
 * Output:
 *      pEntry - pointer to vlan-based port isolation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_cypress_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VID_LOWERf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VID_UPPERf, &pEntry->vid_high)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VB_ISO_MBR_0f, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VB_ISO_MBR_1f, &pEntry->portmask.bits[1])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "enable=%d, vid=%d, pmsk0=0x%x, pmsk1=0x%x",
        pEntry->enable, pEntry->vid,
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 0),
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 1));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_vlanBasedIsolationEntry_set
 * Description:
 *      Set VLAN-based port isolation entry
 * Input:
 *      unit   - unit id
 *      index  - index id
 *      pEntry - pointer to vlan-based port isolation entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_PORT_VLAN_ISO_VID_EXIST_IN_OTHER_IDX - vid exists in other entry
 * Note:
 *      None
 */
int32
dal_cypress_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "enable=%d, vid=%d, pmsk0=0x%x, pmsk1=0x%x",
        pEntry->enable, pEntry->vid,
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 0),
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 1));


    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pEntry->enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vid > RTK_VLAN_ID_MAX) || (pEntry->vid_high > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RT_ERR_OK != hwp_portmaskValid_Chk(unit, &(pEntry->portmask))), RT_ERR_PORT_MASK);

    PORT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VID_LOWERf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VID_UPPERf, &pEntry->vid_high)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VB_ISO_MBR_0f, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, CYPRESS_VB_ISO_MBR_1f, &pEntry->portmask.bits[1])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_vlanBasedIsolation_vlanSource_get
 * Description:
 *      Get comparing VID type of VLAN-based port isolation
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of linkdown green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_cypress_port_vlanBasedIsolation_vlanSource_get(uint32 unit, rtk_port_vlanIsolationSrc_t *pVlanSrc)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pVlanSrc), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_PORT_ISO_VB_CTRLr, CYPRESS_VLAN_TYPEf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pVlanSrc = VLAN_ISOLATION_SRC_INNER;
            break;
        case 1:
            *pVlanSrc = VLAN_ISOLATION_SRC_OUTER;
            break;
        case 2:
            *pVlanSrc = VLAN_ISOLATION_SRC_FORWARD;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pVlanSrc=%d", *pVlanSrc);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_vlanBasedIsolation_vlanSource_set
 * Description:
 *      Set comparing VID type of VLAN-based port isolation
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of linkdown green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_port_vlanBasedIsolation_vlanSource_set(uint32 unit, rtk_port_vlanIsolationSrc_t vlanSrc)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, vlanSrc=%d", unit, vlanSrc);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((vlanSrc >= VLAN_ISOLATION_SRC_END), RT_ERR_INPUT);

    switch (vlanSrc)
    {
        case VLAN_ISOLATION_SRC_INNER:
            value = 0;
            break;
        case VLAN_ISOLATION_SRC_OUTER:
            value = 1;
            break;
        case VLAN_ISOLATION_SRC_FORWARD:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    PORT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_PORT_ISO_VB_CTRLr, CYPRESS_VLAN_TYPEf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_flowCtrlEnable_get
 * Description:
 *      Get the flow control status of the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pEnable - pointer to the status of the flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      The API get the flow control status by port based, no matter N-WAY is enabled or disabled.
 *      This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_cypress_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    rtk_enable_t    nway_enable, flowctrl_enable;
    rtk_port_phy_ability_t  ability;
    rtk_port_speed_t    speed;
    rtk_port_duplex_t   duplex;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if (ENABLED == nway_enable)
    {
        if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
        if (ability.FC && ability.AsyFC)
            (*pEnable) = ENABLED;
        else
            (*pEnable) = DISABLED;
    }
    else
    {
        if ((ret = dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
        (*pEnable) = flowctrl_enable;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_port_flowCtrlEnable_get */

/* Function Name:
 *      dal_cypress_port_flowCtrlEnable_set
 * Description:
 *      Set the flow control status to the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      enable    - enable status of flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      The API is apply the flow control status by port based, no matter N-WAY is enabled or disabled.
 *      This API can not use sem lock since it takes use of dal layer API
 */
int32
dal_cypress_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    rtk_enable_t    flowctrl_enable;
    rtk_port_phy_ability_t  ability;
    rtk_port_speed_t    speed;
    rtk_port_duplex_t   duplex;
    rtk_port_flowctrl_mode_t    fcMode;
    rtk_enable_t    nway_enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED) && (enable != ENABLED), RT_ERR_INPUT);

    if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }
    if (ENABLED == enable)
    {
        ability.FC = 1;
        ability.AsyFC = 1;
    }
    else
    {
        ability.FC = 0;
        ability.AsyFC = 0;
    }
    if ((ret = dal_phy_portAutoNegoAbility_set(unit, port, &ability)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if (ENABLED == enable)
    {
        fcMode.tx_pause = ENABLED;
        fcMode.rx_pause = ENABLED;
    }
    else
    {
        fcMode.tx_pause = DISABLED;
        fcMode.rx_pause = DISABLED;
    }
    if ((ret = dal_phy_portForceFlowctrlMode_set(unit, port, &fcMode)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if ((ret = dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }
    if (ENABLED == enable)
    {
        flowctrl_enable = ENABLED;
    }
    else
    {
        flowctrl_enable = DISABLED;
    }
    if ((ret = dal_phy_portForceModeAbility_set(unit, port, speed, duplex, flowctrl_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,fail 0x%X", unit, port, ret);
        return ret;
    }
    if (DISABLED == nway_enable)
    {
        PORT_SEM_LOCK(unit);
        /* Force flowcontrol */
        _dal_cypress_port_macForceFc_set(unit, port, ENABLED, fcMode.tx_pause, fcMode.rx_pause);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
} /* end of dal_cypress_port_flowCtrlEnable_set */

/* Function Name:
 *      dal_cypress_port_linkMedia_get
 * Description:
 *      Get link status and media
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_port_linkMedia_get(uint32 unit, rtk_port_t port,
    rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* link status need to get twice */
    if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_STSr, port,
                          REG_ARRAY_INDEX_NONE, CYPRESS_LINK_STSf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_STSr, port,
                          REG_ARRAY_INDEX_NONE, CYPRESS_LINK_STSf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (0 == val)
    {
        *pStatus = PORT_LINKDOWN;
        /* media should be ignored when link down */
        /*pMedia = PORT_MEDIA_COPPER;*/
    }
    else
    {
        *pStatus = PORT_LINKUP;
        if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_MEDIA_STSr, port,
                          REG_ARRAY_INDEX_NONE, CYPRESS_MEDIA_STSf, &val)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (0 == val)
            *pMedia = PORT_MEDIA_COPPER;
        else
            *pMedia = PORT_MEDIA_FIBER;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_port_linkMedia_get */

/* Function Name:
 *      dal_cypress_port_fiberOAMLoopBackEnable_set
 * Description:
 *      Set OAM Loopback featrue of the specific Fiber-port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber down speed status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
dal_cypress_port_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,enable=%d",unit, port, enable);
    RT_PARAM_CHK((!HWP_SERDES_PORT(unit, port)) && ((!HWP_FIBER_PORT(unit, port))) && (!(HWP_GE_PORT(unit, port) && HWP_COMBO_PORT(unit, port))), RT_ERR_PORT_ID);

    /*Step1: Disable MAC RX or Enable MAC RX */
    if(enable == DISABLED)
        dal_cypress_port_rxEnable_set(unit,  port,  ENABLED);
    else
        dal_cypress_port_rxEnable_set(unit,  port,  DISABLED);

    /*Step2: Disable PHY Digital Loopback or Not */
    PORT_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = dal_phy_portFiberOAMLoopBackEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port %u Error Code: 0x%X", port, ret);
        return ret;
    }

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    if (  (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22)
        ||(HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF_NC5))
    {
        if (enable == ENABLED)
        {
            rtk_port_speed_t    speed;
            if (dal_phy_portSpeed_get(unit, port, &speed) == RT_ERR_OK)
            {
                uint32      spd_sel, sel;
                if (speed == PORT_SPEED_10G)
                    spd_sel = 0x3;
                else if (speed == PORT_SPEED_1000M)
                    spd_sel = 0x2;
                else if (speed == PORT_SPEED_100M)
                    spd_sel = 0x1;
                else
                    spd_sel = 0x2;
                /* force link/speed/duplex */
                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_SPD_SELf, &spd_sel)) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                sel = 1;
                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_DUP_SELf, &sel)) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_FORCE_LINK_ENf, &sel)) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_MAC_FORCE_ENf, &sel)) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

            }
        }
        else
        {
            uint32      sel = 0;
            if ((ret = reg_array_field_write(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port,
                REG_ARRAY_INDEX_NONE, CYPRESS_MAC_FORCE_ENf, &sel)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

        }
    }
#endif

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_port_OAMLoopBack_set */

/* Function Name:
 *      dal_cypress_port_init_check
 * Description:
 *      Check Port module init flag
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT
 * Note:
 *      None
 */
int32 dal_cypress_port_init_check(uint32 unit)
{
    RT_INIT_CHK(port_init[unit]);
    return RT_ERR_OK;
}


/* Function Name:
 *      _dal_cypress_port_macForce_set
 * Description:
 *      configure force mode
 * Input:
 *      unit    - unit id
 *      sdsId   - serdes ID
 *      mode   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_CHIP_NOT_SUPPORTED   - serdes mode is not supported by the chip
 * Note:
 *      None
 */
void
_dal_cypress_port_macForceMode_set(uint32 unit, rtk_port_t port,
                    rtk_port_swMacPollPhyStatus_t *pphyStatus_prev,
                    rtk_port_swMacPollPhyStatus_t *pphyStatus)
{
    uint32          txPause, rxPause;

    txPause = PHY_RESL_STS_TXPAUSE(pphyStatus->reslStatus);
    rxPause = PHY_RESL_STS_RXPAUSE(pphyStatus->reslStatus);
    if (  (txPause != PHY_RESL_STS_TXPAUSE(pphyStatus_prev->reslStatus))
        ||(rxPause != PHY_RESL_STS_RXPAUSE(pphyStatus_prev->reslStatus)))
    {
        _dal_cypress_port_macForceFc_set(unit, port, 1, txPause, rxPause);
    }
}


/* Function Name:
 *      _dal_cypress_port_swMacPollPhyEnable_get
 * Description:
 *      Enable status of Software Polling of the port
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_dal_cypress_port_swMacPollPhyEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *penable)
{
#if defined(CONFIG_SDK_RTL8295R) /* 95R MAC-POLL-PHY */
    if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22))
    {
        *penable = ENABLED;
    }
    else
#endif /* CONFIG_SDK_RTL8295R */
#if defined(CONFIG_SDK_PHY_CUST1)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_CUST1)
    {
        *penable = ENABLED;
    }
    else
#endif /* CONFIG_SDK_PHY_CUST1 */
    {
        *penable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_cypress_port_swMacPollPhy
 * Description:
 *      polling 8295R link status for 96M
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_dal_cypress_port_swMacPollPhy(uint32 unit, rtk_port_t port)
{
    rtk_enable_t    enable;
    int32           ret;
    rtk_port_swMacPollPhyStatus_t   phyStatus;

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);
    _dal_cypress_port_swMacPollPhyEnable_get(unit, port, &enable);
    if (enable == DISABLED)
    {
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        goto EXIT;
    }

    osal_memset(&phyStatus, 0, sizeof(rtk_port_swMacPollPhyStatus_t));
    if ((ret = _dal_phy_portSwMacPollPhyStatus_get(unit, port, &phyStatus)) != RT_ERR_OK)
    {
        goto EXIT;
    }

    /* MAC Force Mode */
    _dal_cypress_port_macForceMode_set(unit, port, &pMac_info[unit]->swMacPollPhyStatus[port], &phyStatus);

    /* record the PHY status */
    osal_memcpy(&pMac_info[unit]->swMacPollPhyStatus[port], &phyStatus, sizeof(rtk_port_swMacPollPhyStatus_t));

  EXIT:
    PORT_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_cypress_port_sem_lock
 * Description:
 *      Lock Port Module Sem
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_SEM_LOCK_FAILED
 * Note:
 *      None
 */
int32 dal_cypress_port_sem_lock(uint32 unit)
{
    PORT_SEM_LOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_sem_unlock
 * Description:
 *      Unlock Port Module Sem
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_SEM_LOCK_FAILED
 * Note:
 *      None
 */
int32 dal_cypress_port_sem_unlock(uint32 unit)
{
    PORT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 *      speed                 - port speed
 *      duplex                - port duplex mode
 *      flowControl           - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_PHY_SPEED       - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX      - invalid PHY duplex setting
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *
 */
int32
dal_cypress_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    rtk_enable_t nway_enable;
    rtk_port_flowctrl_mode_t    fcMode;
    int32   ret;

    if ((ret = dal_phy_portForceModeAbility_set(unit, port, speed, duplex, flowControl)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,portForceModeAbility set fail 0x%X",unit, port, ret);
        return ret;
    }

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,fail 0x%X", unit, port, ret);
        return ret;
    }

    if (DISABLED == nway_enable)
    {
        if ((ret = dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,get fcMode fail 0x%X", unit, port, ret);
            return ret;
        }

        PORT_SEM_LOCK(unit);
        /* Force flowcontrol */
        if (flowControl == ENABLED)
        {
            _dal_cypress_port_macForceFc_set(unit, port, ENABLED, fcMode.tx_pause, fcMode.rx_pause);
        }
        else
        {
            _dal_cypress_port_macForceFc_set(unit, port, ENABLED, DISABLED, DISABLED);
        }
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_cypress_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      unit                 - unit id
 *      port                 - port id
 *      enable               - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_INPUT          - input parameter out of range
 * Note:
 *      1. ENABLED : switch to PHY auto negotiation mode
 *         DISABLED: switch to PHY force mode
 *      2. Once the abilities of both auto-nego and force mode are set,
 *         you can freely swtich the mode without calling ability setting API again
 */
int32
dal_cypress_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_flowctrl_mode_t    fcMode;
    rtk_enable_t                flowctrl_enable;
    rtk_port_speed_t            speed;
    rtk_port_duplex_t           duplex;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, enable=%d", unit, port, enable);

    if ((ret = dal_phy_portAutoNegoEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        return ret;
    }

    if (ENABLED == enable)
    {
        PORT_SEM_LOCK(unit);
        /* Disable Force flowcontrol */
        _dal_cypress_port_macForceFc_set(unit, port, DISABLED, ENABLED, ENABLED);
        PORT_SEM_UNLOCK(unit);

    }
    else
    {
        if ((dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode) == RT_ERR_OK) &&
            (dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable) == RT_ERR_OK))
        {
            PORT_SEM_LOCK(unit);
            /* Force flowcontrol */
            if (flowctrl_enable == ENABLED)
            {
                _dal_cypress_port_macForceFc_set(unit, port, ENABLED, fcMode.tx_pause, fcMode.rx_pause);
            }
            else
            {
                _dal_cypress_port_macForceFc_set(unit, port, ENABLED, DISABLED, DISABLED);
            }
            PORT_SEM_UNLOCK(unit);
        }
        else
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,portForceFlowctrlMode or portForceModeAbility get fail", unit, port);
        }
    }

    return RT_ERR_OK;
}


