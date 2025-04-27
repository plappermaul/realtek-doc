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
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <dal/dal_phy.h>
#include <dal/maple/dal_maple_port.h>
#include <dal/maple/dal_maple_vlan.h>
#include <dal/dal_mgmt.h>
#include <rtk/port.h>
#include <rtk/default.h>
#include <private/drv/swcore/swcore_rtl8380.h>
#include <ioal/mem32.h>
#include <hal/phy/phy_rtl8380.h>

/*
 * Symbol Definition
 */
typedef struct dal_maple_mac_info_s
{
    uint8   admin_enable[RTK_MAX_NUM_OF_PORTS];
    uint8   green_enable[RTK_MAX_NUM_OF_PORTS];
    uint8   gigaLite_enable[RTK_MAX_NUM_OF_PORTS];
} dal_maple_mac_info_t;

typedef struct dal_maple_phy_info_s
{
    uint8   force_mode_speed[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_duplex[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_flowControl[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_pause[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_asy_pause[RTK_MAX_NUM_OF_PORTS];
    uint8   cross_over_mode[RTK_MAX_NUM_OF_PORTS];
} dal_maple_phy_info_t;

/*
 * Data Declaration
 */
static uint32               port_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         port_sem[RTK_MAX_NUM_OF_UNIT];
static dal_maple_mac_info_t   *pMac_info[RTK_MAX_NUM_OF_UNIT];
static dal_maple_phy_info_t   *pPhy_info[RTK_MAX_NUM_OF_UNIT];
static dal_link_change_callback_f   link_change_callback_f[RTK_MAX_NUM_OF_UNIT];


/*
 * Macro Definition
 */
/* port semaphore handling */
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
static int32 _dal_maple_port_init_config(uint32 unit);
#endif/* !defined(__BOOTLOADER__) */

/* Module Name    : port   */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_maple_portMapper_init
 * Description:
 *      Hook port module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook port module before calling any port APIs.
 */
int32
dal_maple_portMapper_init(dal_mapper_t *pMapper)
{
#ifndef __BOOTLOADER__
    pMapper->port_init = dal_maple_port_init;
    pMapper->port_link_get = dal_maple_port_link_get;
    pMapper->port_speedDuplex_get = dal_maple_port_speedDuplex_get;
    pMapper->port_flowctrl_get = dal_maple_port_flowctrl_get;
    pMapper->port_phyAutoNegoEnable_set = dal_maple_port_phyAutoNegoEnable_set;
    pMapper->port_phyAutoNegoAbility_get = dal_maple_port_phyAutoNegoAbility_get;
    pMapper->port_phyAutoNegoAbility_set = dal_maple_port_phyAutoNegoAbility_set;
    pMapper->port_phyForceModeAbility_get = dal_maple_port_phyForceModeAbility_get;
    pMapper->port_phyForceModeAbility_set = dal_maple_port_phyForceModeAbility_set;
    pMapper->port_cpuPortId_get = dal_maple_port_cpuPortId_get;
    pMapper->port_isolation_get = dal_maple_port_isolation_get;
    pMapper->port_isolation_set = dal_maple_port_isolation_set;
    pMapper->port_isolation_add = dal_maple_port_isolation_add;
    pMapper->port_isolation_del = dal_maple_port_isolation_del;
    pMapper->port_adminEnable_get = dal_maple_port_adminEnable_get;
    pMapper->port_adminEnable_set = dal_maple_port_adminEnable_set;
    pMapper->port_backpressureEnable_get = dal_maple_port_backpressureEnable_get;
    pMapper->port_backpressureEnable_set = dal_maple_port_backpressureEnable_set;
    pMapper->port_linkChange_register = dal_maple_port_linkChange_register;
    pMapper->port_linkChange_unregister = dal_maple_port_linkChange_unregister;
    pMapper->port_txEnable_get = dal_maple_port_txEnable_get;
    pMapper->port_txEnable_set = dal_maple_port_txEnable_set;
    pMapper->port_rxEnable_get = dal_maple_port_rxEnable_get;
    pMapper->port_rxEnable_set = dal_maple_port_rxEnable_set;
    pMapper->port_specialCongest_get = dal_maple_port_specialCongest_get;
    pMapper->port_specialCongest_set = dal_maple_port_specialCongest_set;
    pMapper->port_flowCtrlEnable_get = dal_maple_port_flowCtrlEnable_get;
    pMapper->port_flowCtrlEnable_set = dal_maple_port_flowCtrlEnable_set;
    pMapper->port_linkMedia_get = dal_maple_port_linkMedia_get;
    pMapper->port_vlanBasedIsolationEntry_get = dal_maple_port_vlanBasedIsolationEntry_get;
    pMapper->port_vlanBasedIsolationEntry_set = dal_maple_port_vlanBasedIsolationEntry_set;
    pMapper->port_vlanBasedIsolation_vlanSource_get = dal_maple_port_vlanBasedIsolation_vlanSource_get;
    pMapper->port_vlanBasedIsolation_vlanSource_set = dal_maple_port_vlanBasedIsolation_vlanSource_set;
    pMapper->port_fiberOAMLoopBackEnable_set = dal_maple_port_fiberOAMLoopBackEnable_set;
#else
    /* mapper for U-Boot */
    pMapper->port_isolation_set = dal_maple_port_isolation_set;
#endif
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1) Module must be initialized before using all of APIs in this module
 */
int32
dal_maple_port_init(uint32 unit)
{
    int32   ret;
    uint32 value;
    uint32 port=0;
    rtk_portmask_t portmask;

    RTK_PORTMASK_RESET(portmask);
    RT_INIT_REENTRY_CHK(port_init[unit]);
    port_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    port_sem[unit] = osal_sem_mutex_create();
    if (0 == port_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    pMac_info[unit] = (dal_maple_mac_info_t *)osal_alloc(sizeof(dal_maple_mac_info_t));
    if (NULL == pMac_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pMac_info[unit], 0, sizeof(dal_maple_mac_info_t));

    pPhy_info[unit] = (dal_maple_phy_info_t *)osal_alloc(sizeof(dal_maple_phy_info_t));
    if (NULL == pPhy_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "memory allocate failed");
        osal_free(pMac_info[unit]);
        pMac_info[unit] = NULL;
        return RT_ERR_FAILED;
    }

    osal_memset(pPhy_info[unit], 0, sizeof(dal_maple_phy_info_t));

    /* init callback function for link change */
    link_change_callback_f[unit] = 0;

    /* set init flag to complete init */
    port_init[unit] = INIT_COMPLETED;


    /******************Auto Recovery Init***********************/
    /*Enable Auto Software Queue Reset featrue*/
    value = 0x1;
    if ((ret = reg_field_write(unit, MAPLE_AUTO_SWQRST_CTRLr, MAPLE_SWQRST_SYS_THR_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    value = 0x1;
    if ((ret = reg_field_write(unit, MAPLE_AUTO_SWQRST_CTRLr, MAPLE_SWQRST_P_THR_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    /*Set system threshold*/
    value = 0x7c9;
    if ((ret = reg_field_write(unit, MAPLE_AUTO_SWQRST_CTRLr, MAPLE_SWQRST_SYS_THRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
     /*flow control port24-27 14fc open , set LED_CLK = 96ns*/
    if ((ret = reg_field_read(unit, MAPLE_DMY_REG8r, MAPLE_DUMMYf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    value = value &(~0x7);
    value = value | 0x7;
    if ((ret = reg_field_write(unit, MAPLE_DMY_REG8r, MAPLE_DUMMYf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }


    /******************Giga-Lite MAC Polling Access Always Enable for 838X***********************/
    if (HWP_8380_FAMILY_ID(unit))
    {
        /*Port0-Port23*/
        value = 0x1;
        if ((ret = reg_field_write(unit,
                              MAPLE_SMI_GLB_CTRLr,
                              MAPLE_SMI_GLITE_ACCESS_23_0f,
                              &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        /*Port24-Port27*/
        value = 0x1;
        if ((ret = reg_field_write(unit,
                              MAPLE_SMI_GLB_CTRLr,
                              MAPLE_SMI_GLITE_ACCESS_27_24f,
                              &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    /* Initial CPU port MAC ID */
    if ((ret = dal_maple_port_cpuPortId_set(unit, (rtk_port_t)HWP_CPU_MACID(unit))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    HWP_GET_ALL_PORTMASK(unit, portmask);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PORT_EXIST(unit, port))
        {
            if ((ret = dal_maple_port_isolation_set(unit, port, &portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port%u init isolation port failed", port);
                /* continue the process */
                continue;
            }
        }
    }

#if !defined(__BOOTLOADER__)
    /******************Port Settings Init***********************/
    if (( ret = _dal_maple_port_init_config(unit)) != RT_ERR_OK)
    {
        port_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pMac_info[unit]);
        pMac_info[unit] = NULL;
        osal_free(pPhy_info[unit]);
        pPhy_info[unit] = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port default configuration init failed");
        return ret;
    }
#endif/* !defined(__BOOTLOADER__) */

    return RT_ERR_OK;
} /* end of dal_maple_port_init */

/* Function Name:
 *      dal_maple_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
dal_maple_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  val  = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if (port < 28)
    {
        reg_field_read(unit, MAPLE_MAC_LINK_STSr, MAPLE_LINK_STS_27_0f, &val);
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_STSr, MAPLE_LINK_STS_27_0f, &val)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
        val = (val >> port) & 0x1;
    }

    if (port == 28)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_MAC_FORCE_MODE_CTRLr, port, \
            REG_ARRAY_INDEX_NONE, MAPLE_FORCE_LINK_ENf, &val)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    /* translate chip's value to definition */
    if (TRUE == val)
    {
        *pStatus = PORT_LINKUP;
    }
    else
    {
        *pStatus = PORT_LINKDOWN;
    }

    PORT_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pStatus=%d", *pStatus);

    return RT_ERR_OK;
} /* end of dal_maple_port_link_get */

/* Function Name:
 *      dal_maple_port_txEnable_set
 * Description:
 *      Set the TX enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of TX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_maple_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    /* programming value on CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_TXRX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        val |= 0x1UL;
    }
    else
    {
        val &= (~0x1UL);
    }

    if ((ret = reg_array_field_write(unit,
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_TXRX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_txEnable_set */

/* Function Name:
 *      dal_maple_port_rxEnable_set
 * Description:
 *      Set the RX enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_maple_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    /* programming value on CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_TXRX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        val |= 0x2;
    }
    else
    {
        val &= (~0x2UL);
    }

    if ((ret = reg_array_field_write(unit,
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_TXRX_ENf,
                          &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_rxEnable_set */

/* Function Name:
 *      dal_maple_port_txEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_maple_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
                      MAPLE_MAC_PORT_CTRLr,
                      port,
                      REG_ARRAY_INDEX_NONE,
                      MAPLE_TXRX_ENf,
                      &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (1 == (value & 0x1))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_port_txEnable_get */

/* Function Name:
 *      dal_maple_port_rxEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_maple_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
                      MAPLE_MAC_PORT_CTRLr,
                      port,
                      REG_ARRAY_INDEX_NONE,
                      MAPLE_TXRX_ENf,
                      &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (0x2 == (value & 0x2))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_port_rxEnable_get */

/* Function Name:
 *      dal_maple_port_specialCongest_get
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_maple_port_specialCongest_get(uint32 unit, rtk_port_t port, uint32* pSecond)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MAPLE_SC_P_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_CNGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_specialCongest_get */

/* Function Name:
 *      dal_maple_port_specialCongest_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, second=%d",
           unit, port, second);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(second > RTK_PORT_SPEC_CONGEST_TIME_MAX, RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MAPLE_SC_P_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_CNGST_SUST_TMR_LMTf, &second)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_specialCongest_set */

/* Function Name:
 *      dal_maple_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      1) The speed type of the port is as following:
 *         - PORT_SPEED_10M
 *         - PORT_SPEED_100M
 *         - PORT_SPEED_1000M
 *      2) The duplex mode of the port is as following:
 *         - HALF_DUPLEX
 *         - FULL_DUPLEX
 */
int32
dal_maple_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32   ret;
    uint32  speed;
    uint32  duplex = 0, duplex_27_0;
    rtk_port_linkStatus_t  link_status;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    /* Check Link status */
    if ((ret = dal_maple_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
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

    if (port < 28)
    {
        /* get speed value from CHIP*/
        if ((ret = reg_array_field_read(unit,
                              MAPLE_MAC_LINK_SPD_STSr,
                              port,
                              REG_ARRAY_INDEX_NONE,
                              MAPLE_SPD_STS_27_0f,
                              &speed)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        /* get duplex value from CHIP*/
        if ((ret = reg_field_read(unit,
                              MAPLE_MAC_LINK_DUP_STSr,
                              MAPLE_DUP_STS_27_0f,
                              &duplex_27_0)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
        duplex = ((duplex_27_0 >> port) & 0x1);

    }

    if (port == 28)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_MAC_FORCE_MODE_CTRLr, port, \
            REG_ARRAY_INDEX_NONE, MAPLE_SPD_SELf, &speed)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = reg_array_field_read(unit, MAPLE_MAC_FORCE_MODE_CTRLr, port, \
            REG_ARRAY_INDEX_NONE, MAPLE_DUP_SELf, &speed)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    PORT_SEM_UNLOCK(unit);

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
            if ((24 == port) || (26 == port))
            {
                *pSpeed = PORT_SPEED_2G;  /*only for port24 & port26*/
            }
            else
            {
                *pSpeed = PORT_SPEED_500M;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    /* translate chip's value to definition */
    if (1 == duplex)
    {
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else
    {
        *pDuplex = PORT_HALF_DUPLEX;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pSpeed=%d, pDuplex=%d",
           *pSpeed, *pDuplex);

    return RT_ERR_OK;
} /* end of dal_maple_port_speedDuplex_get */

/* Function Name:
 *      dal_maple_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
dal_maple_port_flowctrl_get(
    uint32            unit,
    rtk_port_t        port,
    uint32            *pTxStatus,
    uint32            *pRxStatus)
{
    int32   ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);


    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_MAC_RX_PAUSE_STSr, MAPLE_RX_PAUSE_STS_27_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pRxStatus = (val & (1<<port)) ? 1 : 0;

    if ((ret = reg_field_read(unit, MAPLE_MAC_TX_PAUSE_STSr, MAPLE_TX_PAUSE_STS_27_0f, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pTxStatus = (val & (1<<port)) ? 1 : 0;

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pRxStatus=%d, pTxStatus=%d", *pRxStatus, *pTxStatus);

    return RT_ERR_OK;
} /* end of dal_maple_port_flowctrl_get */

/* Function Name:
 *      dal_maple_port_phyAutoNegoEnablePortmask_set
 * Description:
 *      Set PHY ability of the specific port(s)
 * Input:
 *      unit        - unit id
 *      portMask    - list of ports
 *      enable      - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      1) ENABLED : switch to PHY auto negotiation mode
 *         DISABLED: switch to PHY force mode
 *      2) Once the abilities of both auto-nego and force mode are set,
 *         you can freely swtich the mode without calling ability setting API again
 */
int32
dal_maple_port_phyAutoNegoEnablePortmask_set(uint32 unit, rtk_portmask_t portMask, rtk_enable_t enable)
{
    int32   ret,port;
    rtk_port_phy_ability_t ability;
    rtk_port_flowctrl_mode_t    fcMode;
    uint32  reg_idx,temp;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, enable=%d",
           unit, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    for(port=0;port<RTK_MAX_NUM_OF_PORTS;port++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portMask,port))
            continue;
        RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    }
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    for(port=0;port<RTK_MAX_NUM_OF_PORTS;port++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portMask,port))
            continue;
        if ((ret = dal_phy_portAutoNegoEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    /*Delay 100ms*/
    osal_time_usleep(100000);

    if (ENABLED == enable)
    {
        for(port=0;port<RTK_MAX_NUM_OF_PORTS;port++)
        {
            if(!RTK_PORTMASK_IS_PORT_SET(portMask,port))
                continue;

            /* Need to configure [MAC_FORCE_MODE_CTRL] register  when PHY is force mode */
            reg_idx = MAPLE_MAC_FORCE_MODE_CTRLr;

            temp = 0x0;     /*Reset MAC FORCE Flow Control to Disable [MAC FLOW CONTROL FORCE ENABLE]*/
            ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_MAC_FORCE_FC_ENf, &temp);
            if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            /* get value from CHIP*/
            if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }


            ability.FC = pPhy_info[unit]->auto_mode_pause[port];
            ability.AsyFC = pPhy_info[unit]->auto_mode_asy_pause[port];


            /* get value from CHIP*/
            if ((ret = dal_phy_portAutoNegoAbility_set(unit, port, &ability)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }
        }
    }

    if (DISABLED == enable)
    {
       for(port=0;port<RTK_MAX_NUM_OF_PORTS;port++)
        {
            if(!RTK_PORTMASK_IS_PORT_SET(portMask,port))
                continue;

            /*SS-316, forbid 1000H*/
            if(pPhy_info[unit]->force_mode_duplex[port] == PORT_HALF_DUPLEX)
            {
                /* set speed to CHIP*/
                if ((ret = dal_phy_portSpeed_set(unit, port, pPhy_info[unit]->force_mode_speed[port])) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                /* set duplex to CHIP*/
                if ((ret = dal_phy_portDuplex_set(unit, port, pPhy_info[unit]->force_mode_duplex[port])) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }
            }
            else
            {
                /* set duplex to CHIP*/
                if ((ret = dal_phy_portDuplex_set(unit, port, pPhy_info[unit]->force_mode_duplex[port])) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                /* set speed to CHIP*/
                if ((ret = dal_phy_portSpeed_set(unit, port, pPhy_info[unit]->force_mode_speed[port])) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }
            }

            if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            ability.FC = pPhy_info[unit]->force_mode_flowControl[port]; /* ENABLED */
            ability.AsyFC = pPhy_info[unit]->force_mode_flowControl[port]; /* ENABLED */


            /* Need to configure [MAC_FORCE_MODE_CTRL] register  when PHY is force mode */
            reg_idx = MAPLE_MAC_FORCE_MODE_CTRLr;

            temp = 0x1;     /*Set MAC FORCE Flow Control*/
            ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_MAC_FORCE_FC_ENf, &temp);
            if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            if (DISABLED == pPhy_info[unit]->force_mode_flowControl[port])
            {
                fcMode.rx_pause = DISABLED;
                fcMode.tx_pause = DISABLED;
            }
            else
            {
                if ((ret = dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode)) != RT_ERR_OK)
                {
                    PORT_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }
            }

            temp = fcMode.rx_pause;
            ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_RX_PAUSE_ENf, &temp);
            if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            temp = fcMode.tx_pause;
            ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_TX_PAUSE_ENf, &temp);
            if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            /* get value from CHIP*/
            if ((ret = dal_phy_portAutoNegoAbility_set(unit, port, &ability)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }
        }
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_phyAutoNegoEnablePortmask_set */


/* Function Name:
 *      dal_maple_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port(s)
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      1) ENABLED : switch to PHY auto negotiation mode
 *         DISABLED: switch to PHY force mode
 *      2) Once the abilities of both auto-nego and force mode are set,
 *         you can freely swtich the mode without calling ability setting API again
 */
int32
dal_maple_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_portmask_t myPortMask;

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);

    RTK_PORTMASK_RESET(myPortMask);
    RTK_PORTMASK_PORT_SET(myPortMask,port);

    return dal_maple_port_phyAutoNegoEnablePortmask_set(unit, myPortMask, enable);

} /* end of dal_maple_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_maple_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pAbility - pointer to the PHY ability
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
dal_maple_port_phyAutoNegoAbility_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32   ret;
    rtk_enable_t  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    osal_memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));

    PORT_SEM_LOCK(unit);

    if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, pAbility)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (DISABLED == enable)
    {
        pAbility->FC    = pPhy_info[unit]->auto_mode_pause[port];
        pAbility->AsyFC = pPhy_info[unit]->auto_mode_asy_pause[port];
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Half_10=%d, Full_10=%d, \
           Half_100=%d, Full_100=%d, Half_1000=%d, Full_1000=%d, FC=%d, AsyFC=%d",
           pAbility->Half_10, pAbility->Full_10, pAbility->Half_100, pAbility->Full_100,
           pAbility->Half_1000, pAbility->Full_1000, pAbility->FC, pAbility->AsyFC);

    return RT_ERR_OK;
} /* end of dal_maple_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_maple_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) You can set these abilities no matter which mode PHY currently stays on
 */
int32
dal_maple_port_phyAutoNegoAbility_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32   ret;
    rtk_enable_t    enable;
    uint32  reg_idx,temp;


    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HWP_FE_PORT(unit, port)) && (pAbility->Full_1000 == ABILITY_BIT_ON), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Half_10=%d, Full_10=%d, Half_100=%d, Full_100=%d, \
           Half_1000=%d, Full_1000=%d, FC=%d, AsyFC=%d",
           pAbility->Half_10, pAbility->Full_10, pAbility->Half_100, pAbility->Full_100,
           pAbility->Half_1000, pAbility->Full_1000, pAbility->FC, pAbility->AsyFC);

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "dal_phy_portAutoNegoEnable_get(unit=%u, port=%u) failed!!",\
               unit, port);
        return ret;
    }

    if (DISABLED == enable)
    {
        pPhy_info[unit]->auto_mode_pause[port] = pAbility->FC;
        pPhy_info[unit]->auto_mode_asy_pause[port] = pAbility->AsyFC;
        pAbility->FC = pPhy_info[unit]->force_mode_flowControl[port];
        pAbility->AsyFC = pPhy_info[unit]->force_mode_flowControl[port];
    }

    PORT_SEM_LOCK(unit);

    /* Need to configure [MAC_FORCE_MODE_CTRL] register  when PHY is force mode */
    reg_idx = MAPLE_MAC_FORCE_MODE_CTRLr;

    temp = 0x0;     /*Reset MAC FORCE Flow Control to Disable [MAC FLOW CONTROL FORCE ENABLE]*/
    ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_MAC_FORCE_FC_ENf, &temp);
    if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = dal_phy_portAutoNegoAbility_set(unit, port, pAbility)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        pPhy_info[unit]->auto_mode_pause[port] = pAbility->FC;
        pPhy_info[unit]->auto_mode_asy_pause[port] = pAbility->AsyFC;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_maple_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      unit         - unit id
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
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
dal_maple_port_phyForceModeAbility_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl)
{
    int32   ret;
    rtk_enable_t    enable;
    rtk_port_phy_ability_t  ability;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFlowControl), RT_ERR_NULL_POINTER);

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    PORT_SEM_LOCK(unit);

    if (ENABLED == enable)
    {
        *pSpeed = pPhy_info[unit]->force_mode_speed[port];
        *pDuplex = pPhy_info[unit]->force_mode_duplex[port];
        *pFlowControl = pPhy_info[unit]->force_mode_flowControl[port];
    }
    else
    {
        /* get value from CHIP*/
        if ((ret = dal_phy_portSpeed_get(unit, port, pSpeed)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        /* get value from CHIP*/
        if ((ret = dal_phy_portDuplex_get(unit, port, pDuplex)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        *pFlowControl = ability.FC;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pSpeed=%d, pDuplex=%d \
           pFlowControl=%d", *pSpeed, *pDuplex, *pFlowControl);

    return RT_ERR_OK;
} /* end of dal_maple_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_maple_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      speed       - port speed
 *      duplex      - port duplex mode
 *      flowControl - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      1) You can set these abilities no matter which mode PHY currently stays on
 *      2) The speed type of the port is as following:
 *         - PORT_SPEED_10M
 *         - PORT_SPEED_100M
 *      3) The duplex mode of the port is as following:
 *         - HALF_DUPLEX
 *         - FULL_DUPLEX
 */
int32
dal_maple_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    int32   ret;
    rtk_enable_t    enable;
    rtk_port_phy_ability_t ability;
    rtk_port_flowctrl_mode_t    fcMode;
    uint32  reg_idx,temp;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, speed=%d, duplex=%d \
           flowControl=%d", unit, port, speed, duplex, flowControl);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(speed >= PORT_SPEED_END, RT_ERR_PHY_SPEED);
    RT_PARAM_CHK((HWP_FE_PORT(unit, port)) && speed == PORT_SPEED_1000M, RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((duplex == PORT_HALF_DUPLEX) && (speed == PORT_SPEED_1000M), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK(duplex >= PORT_DUPLEX_END, RT_ERR_PHY_DUPLEX);
    RT_PARAM_CHK(flowControl >= RTK_ENABLE_END, RT_ERR_INPUT);

    if ((ret = dal_phy_portAutoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    PORT_SEM_LOCK(unit);

    pPhy_info[unit]->force_mode_speed[port] = speed;
    pPhy_info[unit]->force_mode_duplex[port] = duplex;
    pPhy_info[unit]->force_mode_flowControl[port] = flowControl;

    if (DISABLED == enable)
    {
        /*SS-316, forbid 1000H*/
        if(duplex == PORT_HALF_DUPLEX)
        {
            /* set value to CHIP*/
            if ((ret = dal_phy_portSpeed_set(unit, port, speed)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            if ((ret = dal_phy_portDuplex_set(unit, port, duplex)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }
        }
        else
        {
            if ((ret = dal_phy_portDuplex_set(unit, port, duplex)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            /* set value to CHIP*/
            if ((ret = dal_phy_portSpeed_set(unit, port, speed)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }
        }

        if ((ret = dal_phy_portAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (ENABLED == flowControl)
        {
            ability.FC = ENABLED;
            ability.AsyFC = ENABLED;
        }
        else
        {
            ability.FC = DISABLED;
            ability.AsyFC = DISABLED;
        }

        /* Need to configure [MAC_FORCE_MODE_CTRL] register  when PHY is force mode */
        reg_idx = MAPLE_MAC_FORCE_MODE_CTRLr;

        temp = 0x1;     /*Always set MAC FORCE Flow Control*/
        ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_MAC_FORCE_FC_ENf, &temp);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (DISABLED == flowControl)
        {
            fcMode.rx_pause = DISABLED;
            fcMode.tx_pause = DISABLED;
        }
        else
        {
            if ((ret = dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode)) != RT_ERR_OK)
            {
                PORT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }
        }

        temp = fcMode.rx_pause;
        ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_RX_PAUSE_ENf, &temp);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        temp = fcMode.tx_pause;
        ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_TX_PAUSE_ENf, &temp);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }


        if ((ret = dal_phy_portAutoNegoAbility_set(unit, port, &ability)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_maple_port_cpuPortId_get
 * Description:
 *      Get CPU port id of the specific unit
 * Input:
 *      unit  - unit id
 * Output:
 *      pPort - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_MAC_CPU_PORT_CTRLr, MAPLE_CPU_PORTf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pPort = 24;
            break;
        case 1:
            *pPort = 26;
            break;
        case 2:
            *pPort = 28;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pPort=%d", *pPort);

    return RT_ERR_OK;
} /* end of dal_maple_port_cpuPortId_get */

/* Function Name:
 *      dal_maple_port_cpuPortId_set
 * Description:
 *      Set CPU port id of the specific unit
 * Input:
 *      unit - unit id
 *      port - CPU port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_maple_port_cpuPortId_set(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    switch (port)
    {
        case 24:
            value = 0;
            break;
        case 26:
            value = 1;
            break;
        case 28:
            value = 2;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_MAC_CPU_PORT_CTRLr, MAPLE_CPU_PORTf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d", port);

    return RT_ERR_OK;
} /* end of dal_maple_port_cpuPortId_set */

/* Function Name:
 *      dal_maple_port_isolation_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) Default value of each port is 1
 *      2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
dal_maple_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get speed value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PORT_ISO_CTRLr, REG_ARRAY_INDEX_NONE, port, MAPLE_P_ISO_MBR_0f
                        , &pPortmask->bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pPortmask=0x%X", (*pPortmask).bits[0]);

    return RT_ERR_OK;
} /* end of dal_maple_port_isolation_get */

/* Function Name:
 *      dal_maple_port_isolation_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pPortmask   - pointer to the portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      1) Default value of each port is 1
 *      2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
dal_maple_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pPortmask), RT_ERR_PORT_MASK);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, portmask=0x%X",
           unit, port, (*pPortmask).bits[0]);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* write isolation mask to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_PORT_ISO_CTRLr, REG_ARRAY_INDEX_NONE, port, MAPLE_P_ISO_MBR_0f
                        , &pPortmask->bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_isolation_set */

/* Function Name:
 *      dal_maple_port_isolation_add
 * Description:
 *      Add an isolation port to the certain port
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      iso_port - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      1) Default value of each port is 1
 *      2) Port and iso_port will be isolated when this API is called
 *      3) The iso_port to the relative portmask bit will be set to 1
 */
int32
dal_maple_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
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

    ret = dal_maple_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, iso_port);

    ret = dal_maple_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_port_isolation_add */

/* Function Name:
 *      dal_maple_port_isolation_del
 * Description:
 *      Delete an existing isolation port of the certain port
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      iso_port - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      1) Default value of each port is 1
 *      2) Isolated status between the port and the iso_port is removed when this API is called
 *      3) The iso_port to the relative portmask bit will be set to 0
 */
int32
dal_maple_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
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

    ret = dal_maple_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, iso_port);

    ret = dal_maple_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_port_isolation_del */

/* Function Name:
 *      dal_maple_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
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
dal_maple_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
} /* end of dal_maple_port_adminEnable_get */

/* Function Name:
 *      dal_maple_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - port admin configuration
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
dal_maple_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
        value = 0x3;
        /* programming value on CHIP*/
        if ((ret = reg_array_field_write(unit,
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_TXRX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
    {
        if ((ret = dal_phy_portEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    if (DISABLED == enable)
    {
        /* programming value on CHIP*/
        value = 0x0;
        if ((ret = reg_array_field_write(unit,
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_TXRX_ENf,
                          &value)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    pMac_info[unit]->admin_enable[port] = enable;
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_adminEnable_set */

/* Function Name:
 *      dal_maple_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of backpressure in half duplex mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2) Used to support backpressure in half mode.
 */
int32
dal_maple_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                         MAPLE_BKPRES_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_port_backpressureEnable_get */

/* Function Name:
 *      dal_maple_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of backpressure in half duplex mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1) The mac local loopback enable status of the port is as following:
 *         - DISABLE
 *         - ENABLE
 *      2) Used to support backpressure in half mode.
 */
int32
dal_maple_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
                          MAPLE_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_BKPRES_ENf,
                          &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_backpressureEnable_set */

/* Function Name:
 *      dal_maple_port_linkChange_register
 * Description:
 *      Register callback function for notification of link change
 * Input:
 *      unit                 - unit id
 *      link_change_callback - Callback function for link change
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_port_linkChange_register(uint32 unit, dal_link_change_callback_f link_change_callback)
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
} /* End of dal_maple_port_linkChange_register */

/* Function Name:
 *      dal_maple_port_linkChange_unregister
 * Description:
 *      Unregister callback function for notification of link change
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 */
int32
dal_maple_port_linkChange_unregister(uint32 unit)
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
} /* End of dal_maple_port_linkChange_unregister */

#if !defined(__BOOTLOADER__)
/* Function Name:
 *      _dal_maple_port_init_config
 * Description:
 *      Initialize default configuration for port module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1) Module must be initialized before using all of APIs in this module
 */

static int32
_dal_maple_port_init_config(uint32 unit)
{
    int32   ret;
    rtk_port_t  port;
    rtk_portmask_t  portmask;
    rtk_port_phy_ability_t phy_ability;
    rtk_port_flowctrl_mode_t    fcMode;
    //rtk_port_crossOver_mode_t   mode;
    rtk_port_speed_t    speed;
    rtk_port_duplex_t   duplex;
    rtk_enable_t   gigaLite;

    phy_ability.Half_10 = RTK_DEFAULT_PORT_10HALF_CAPABLE;
    phy_ability.Full_10 = RTK_DEFAULT_PORT_10FULL_CAPABLE;
    phy_ability.Half_100 = RTK_DEFAULT_PORT_100HALF_CAPABLE;
    phy_ability.Full_100 = RTK_DEFAULT_PORT_100FULL_CAPABLE;
    phy_ability.FC = RTK_DEFAULT_PORT_PAUSE_CAPABILITY;
    phy_ability.AsyFC = RTK_DEFAULT_PORT_ASYPAUSE_CAPABILITY;


    RTK_PORTMASK_RESET(portmask);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        fcMode.rx_pause = ENABLED;
        fcMode.tx_pause = ENABLED;
        RT_ERR_CHK(dal_phy_portForceFlowctrlMode_set(unit, port, &fcMode), ret);

        if (HWP_FE_PORT(unit, port))
        {
            phy_ability.Half_1000 = DISABLED;
            phy_ability.Full_1000 = DISABLED;
        }
        else
        {
            phy_ability.Half_1000 = RTK_DEFAULT_PORT_1000HALF_CAPABLE;
            phy_ability.Full_1000 = RTK_DEFAULT_PORT_1000FULL_CAPABLE;
        }

        /* Config PHY in port that PHY exist */
        if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
        {
            pMac_info[unit]->admin_enable[port] = RTK_ENABLE_END;
            if ((ret = dal_maple_port_adminEnable_set(unit, port, RTK_DEFAULT_PORT_ADMIN_ENABLE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port%u init enable port failed", port);
                /* continue the process */
                continue;
            }

             if (!HWP_IS_CPU_PORT(unit, port) && !HWP_SERDES_PORT(unit, port))
            {
                if ((ret = dal_maple_port_phyAutoNegoAbility_set(unit, port, &phy_ability)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port%u init set autonegotiation ability failed", port);
                    /* continue the process */
                    continue;
                }

                RTK_PORTMASK_PORT_SET(portmask,port);

                speed = PORT_SPEED_END;
                /* get value from CHIP*/
                if ((ret = dal_phy_portSpeed_get(unit, port, &speed)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                pPhy_info[unit]->force_mode_speed[port] = PORT_SPEED_100M;
                duplex = PORT_DUPLEX_END;
                /* get value from CHIP*/
                if ((ret = dal_phy_portDuplex_get(unit, port, &duplex)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }

                pPhy_info[unit]->force_mode_duplex[port] = duplex;

                if ((ret = dal_phy_portGigaLiteEnable_get(unit, port, &gigaLite)) == RT_ERR_OK)
                {
                    pMac_info[unit]->gigaLite_enable[port] = gigaLite;
                }
            }
        }
    }

    if ((ret = dal_maple_port_phyAutoNegoEnablePortmask_set(unit, portmask, RTK_DEFAULT_PORT_AUTONEGO_ENABLE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port init enable PHY autonegotiation failed");
        return ret;
    }


    return RT_ERR_OK;
} /* end of _dal_maple_port_init_config */
#endif/* !defined(__BOOTLOADER__) */

/* Function Name:
 *      dal_maple_port_linkDownGreenEnable_get
 * Description:
 *      Get the statue of linkdown green feature of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of linkdown green feature
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
dal_maple_port_linkDownGreenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    *pEnable = pMac_info[unit]->green_enable[port];

    return RT_ERR_OK;
} /* end of dal_maple_port_linkDownGreenEnable_get */

/* Function Name:
 *      dal_maple_port_vlanBasedIsolationEntry_get
 * Description:
 *      Get the vlan-based isolation entry in the specific unit
 * Input:
 *      unit   - unit id
 *      index  - index id
 * Output:
 *      pEntry - pointer to vlan-based port isolation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t *pEntry)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MAPLE_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MAPLE_VIDf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MAPLE_VB_ISO_MBRf, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "valid=%d, vid=%d, pPortmask=0x%X",
        pEntry->enable, pEntry->vid, pEntry->portmask.bits[0]);

    return RT_ERR_OK;
} /* end of dal_maple_port_vlanBasedIsolationEntry_get */

/* Function Name:
 *      dal_maple_port_vlanBasedIsolationEntry_set
 * Description:
 *      Set the vlan-based isolation entry in the specific unit
 * Input:
 *      unit   - unit id
 *      index  - index id
 *      pEntry - pointer to vlan-based port isolation entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_maple_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t *pEntry)
{
    int32   ret;
    uint32  i, vid;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "valid=%d, vid=%d, pPortmask=0x%X",
        pEntry->enable, pEntry->vid, pEntry->portmask.bits[0]);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RT_ERR_OK != hwp_portmaskValid_Chk(unit, &(pEntry->portmask))), RT_ERR_PORT_MASK);

    PORT_SEM_LOCK(unit);

    /* Search exist entry */
    for (i = 0; i < HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit); i++)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                            REG_ARRAY_INDEX_NONE, i, MAPLE_VIDf, &vid)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
            return ret;
        }

        if (vid != 0 && vid == pEntry->vid)
        {
            if (index != i)
            {
                PORT_SEM_UNLOCK(unit);
                return RT_ERR_PORT_VLAN_ISO_VID_EXIST_IN_OTHER_IDX;
            }
        }
    }

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MAPLE_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MAPLE_VIDf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_PORT_ISO_VB_ISO_PM_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MAPLE_VB_ISO_MBRf, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_vlanBasedIsolationEntry_set */

/* Function Name:
 *      dal_maple_port_vlanBasedIsolation_vlanSource_get
 * Description:
 *      Get comparing VID type of VLAN-based port isolation
 * Input:
 *      unit   - unit id
 * Output:
 *      pVlanSrc - point to vlan isolation source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_port_vlanBasedIsolation_vlanSource_get(uint32 unit, rtk_port_vlanIsolationSrc_t *pVlanSrc)
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
    if ((ret = reg_field_read(unit, MAPLE_PORT_ISO_VB_CTRLr, MAPLE_VLAN_TYPEf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    switch(value)
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
} /* end of dal_maple_port_vlanBasedIsolation_vlanSource_get */

/* Function Name:
 *      dal_maple_port_vlanBasedIsolation_vlanSource_set
 * Description:
 *      Set comparing VID type of VLAN-based port isolation
 * Input:
 *      unit    - unit id
 *      vlanSrc - vlan isolation source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_port_vlanBasedIsolation_vlanSource_set(uint32 unit, rtk_port_vlanIsolationSrc_t vlanSrc)
{
    int32   ret;
    uint32 value;

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
    if ((ret = reg_field_write(unit, MAPLE_PORT_ISO_VB_CTRLr, MAPLE_VLAN_TYPEf, &value)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_vlanBasedIsolation_vlanSource_set */

/* Function Name:
 *      dal_maple_port_flowCtrlEnable_get
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
 */
int32
dal_maple_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
        if ((ret = dal_maple_port_phyAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
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
        if ((ret = dal_maple_port_phyForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
        (*pEnable) = flowctrl_enable;
    }

    return RT_ERR_OK;
} /* end of dal_maple_port_flowCtrlEnable_get */

/* Function Name:
 *      dal_maple_port_flowCtrlEnable_set
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
 */
int32
dal_maple_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    rtk_enable_t    flowctrl_enable;
    rtk_port_phy_ability_t  ability;
    rtk_port_speed_t    speed;
    rtk_port_duplex_t   duplex;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED) && (enable != ENABLED), RT_ERR_INPUT);

    if ((ret = dal_maple_port_phyAutoNegoAbility_get(unit, port, &ability)) != RT_ERR_OK)
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
    if ((ret = dal_maple_port_phyAutoNegoAbility_set(unit, port, &ability)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if ((ret = dal_maple_port_phyForceModeAbility_get(unit, port, &speed, &duplex, &flowctrl_enable)) != RT_ERR_OK)
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
    if ((ret = dal_maple_port_phyForceModeAbility_set(unit, port, speed, duplex, flowctrl_enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_port_flowCtrlEnable_set */



/* Function Name:
 *      dal_maple_port_linkMedia_get
 * Description:
 *      Get link status and media
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pStatus - link status
 *      pMedia - link media
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
dal_maple_port_linkMedia_get(uint32 unit, rtk_port_t port,
    rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia)
{
    uint32  val = 0;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_read(unit, MAPLE_MAC_LINK_STSr, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_read(unit, MAPLE_MAC_LINK_STSr, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (0 == (val & (1UL<<port)))
        *pStatus = PORT_LINKDOWN;
    else
    {
        *pStatus = PORT_LINKUP;
        if ((ret = reg_read(unit, MAPLE_MAC_LINK_MEDIA_STSr, &val)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (0 == (val & (1UL<<port)))
            *pMedia = PORT_MEDIA_COPPER;
        else
            *pMedia = PORT_MEDIA_FIBER;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_port_linkMedia_get */

/* Function Name:
 *      dal_maple_port_fiberOAMLoopBackEnable_set
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
dal_maple_port_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,enable=%d",unit, port, enable);
    RT_PARAM_CHK((!HWP_SERDES_PORT(unit, port)) && (!(HWP_GE_PORT(unit, port)&&HWP_COMBO_PORT(unit, port))), RT_ERR_PORT_ID);


    /*Step1: Disable MAC RX or Enable MAC RX */
    if(enable == DISABLED)
        dal_maple_port_rxEnable_set(unit,  port,  ENABLED);
    else
        dal_maple_port_rxEnable_set(unit,  port,  DISABLED);

    /*Step2: Disable PHY Digital Loopback or Not */
    PORT_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = dal_phy_portFiberOAMLoopBackEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port %d Error Code: 0x%X", port, ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_maple_port_OAMLoopBack_set */

/* Function Name:
 *      dal_maple_port_init_check
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
int32 dal_maple_port_init_check(uint32 unit)
{
    RT_INIT_CHK(port_init[unit]);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_port_sem_lock
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
int32 dal_maple_port_sem_lock(uint32 unit)
{
    PORT_SEM_LOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_port_sem_unlock
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
int32 dal_maple_port_sem_unlock(uint32 unit)
{
    PORT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}



