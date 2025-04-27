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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <dal/dal_phy.h>
#include <dal/mango/dal_mango_port.h>
#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_stack.h>
#include <rtk/port.h>
#include <rtk/default.h>

#include <dal/mango/dal_mango_sds.h>
#include <hal/common/miim.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_rtl9310.h>

#include <hal/mac/serdes.h>


/*
 * Symbol Definition
 */
typedef struct dal_mango_mac_info_s {
    uint8   admin_enable[RTK_MAX_NUM_OF_PORTS];
    uint8   green_enable[RTK_MAX_NUM_OF_PORTS];
} dal_mango_mac_info_t;

/*
 * Data Declaration
 */
static uint32               port_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         port_sem[RTK_MAX_NUM_OF_UNIT];

static dal_mango_mac_info_t   *pMac_info[RTK_MAX_NUM_OF_UNIT];
static dal_link_change_callback_f   link_change_callback_f[RTK_MAX_NUM_OF_UNIT];

extern uint32 phy_watchdog_cnt;
extern uint32 fiber_rx_watchdog_cnt;

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
static int32 _dal_mango_port_init_config(uint32 unit);

extern int32 dal_waMon_phyReconfig_portMaskSet(uint32 unit, rtk_port_t port);

/* Module Name    : port     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_mango_portMacSyncPhySts_process
 * Description:
 *      Sync phy link status to Mac for the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      None
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      None
 */
int32 dal_mango_portMacSyncPhySts_process(int32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  val, spd_force_val, force_mac_spd, spd_sts, tx_no_pkt;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Force link up porcessing of port %d started.\n", port);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* Get TX_EN, RX_EN and Force Mode Conguration */
    if ((ret = reg_array_field_read(unit, MANGO_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, &spd_force_val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, &force_mac_spd)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Disable MAC TX and RX */
    if ((ret = drv_port_txEnable_set(unit, port, DISABLED, DRV_RTL9310_PORT_TXRX_EN_MOD_MAC_SYNC_PHY)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = drv_port_rxEnable_set(unit, port, DISABLED, DRV_RTL9310_PORT_TXRX_EN_MOD_MAC_SYNC_PHY)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Force MAC link down */
    if ((ret = drv_rtl9310_portMacForceLink_set(unit, port, ENABLED,
            PORT_LINKDOWN, DRV_RTL9310_FRCLINK_MODULE_DRAINOUT)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Force MAC speed change */
    if ((ret = reg_array_field_read(unit, MANGO_MAC_LINK_SPD_STSr, port, REG_ARRAY_INDEX_NONE, MANGO_SPD_STSf, &spd_sts)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    switch (spd_sts)
    {
        case 2:      /* 1Gbps   */
            val = 3; /* 500Mbps */
            break;
        case 4:      /* 10Gbps */
            val = 6; /* 5Gbps  */
            break;
        case 6:      /* 5Gbps  */
            val = 4; /* 10Gbps */
            break;
        case 8:      /* 5G proprietary */
            val = 5; /* 2.5Gbps */
            break;
        default:
            val = 2; /* 1Gbps   */
            break;
    }

    if ((ret = reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    val = 0x1;
    if ((ret = reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Sync Phy link status to MAC */
    val = 0x1;
    if ((ret = reg_array_field_write(unit, MANGO_SMI_BYPASS_ABLTY_LOCK_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BYPASS_ABLTY_LOCKf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PER_PORT_MAC_DEBUG0r, port, REG_ARRAY_INDEX_NONE, MANGO_TX_NO_PKTf, &tx_no_pkt)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    val = 0;
    if ((ret = reg_array_field_write(unit, MANGO_SMI_BYPASS_ABLTY_LOCK_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BYPASS_ABLTY_LOCKf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Restore Force MAC speed configuration */
    if ((ret = reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, &force_mac_spd)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, &spd_force_val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Restore Force MAC link configuration */
    if ((ret = drv_rtl9310_portMacForceLink_set(unit, port, DISABLED,
            PORT_LINKUP, DRV_RTL9310_FRCLINK_MODULE_DRAINOUT)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Restore TX_EN and RX_EN Conguration */
    if ((ret = drv_port_txEnable_set(unit, port, ENABLED, DRV_RTL9310_PORT_TXRX_EN_MOD_MAC_SYNC_PHY)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = drv_port_rxEnable_set(unit, port, ENABLED, DRV_RTL9310_PORT_TXRX_EN_MOD_MAC_SYNC_PHY)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    if (FALSE == tx_no_pkt)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_phySds_set
 * Description:
 *      Configure PHY SerDes
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
dal_mango_port_phySds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    rtk_sdsCfg_t    intSdsCfg;
    rtk_enable_t    oriPollingSts;
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PHY), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == sdsCfg), RT_ERR_NULL_POINTER);

    /* function body */
    PORT_SEM_LOCK(unit);

    // pre-config for polling mode change
    if (HWP_PHY_EXIST(unit, port))
    {
        // SerDes off
        intSdsCfg.sdsMode = RTK_MII_DISABLE;
        RT_ERR_HDL(dal_phy_portSds_set(unit, port, &intSdsCfg), ERR, ret);

        // disable polling port mask
        RT_ERR_HDL(hal_miim_pollingEnable_get(unit, port, &oriPollingSts), ERR, ret);
        RT_ERR_HDL(hal_miim_pollingEnable_set(unit, port, DISABLED), ERR, ret);

        osal_time_mdelay(50);
    }

    // Change SerDes mode
    RT_ERR_HDL(dal_phy_portSds_set(unit, port, sdsCfg), ERR1, ret);

    if (HWP_PHY_EXIST(unit, port))
    {
        if (RTK_MII_SGMII == sdsCfg->sdsMode)
        {
            val = 0x2;
        }
        else
        {
            val = 0x0;
        }

        // polling mode change
        RT_ERR_HDL(reg_array_field_write(unit, MANGO_SMI_PHY_ABLTY_GET_SELr,
                port, REG_ARRAY_INDEX_NONE, MANGO_PHY_ABLTY_GET_SELf, &val), ERR1, ret);
    }

ERR1:
    if (HWP_PHY_EXIST(unit, port))
    {
        // rollback polling port mask
        hal_miim_pollingEnable_set(unit, port, oriPollingSts);
    }
ERR:
    PORT_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_port_phySds_set */

/* Function Name:
 *      dal_mango_portMapper_init
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
dal_mango_portMapper_init(dal_mapper_t *pMapper)
{
#ifndef __BOOTLOADER__
    pMapper->port_isolation_get = dal_mango_port_isolation_get;
    pMapper->port_isolation_set = dal_mango_port_isolation_set;
    pMapper->port_isolationExt_get = dal_mango_port_isolationExt_get;
    pMapper->port_isolationExt_set = dal_mango_port_isolationExt_set;
    pMapper->port_isolation_add = dal_mango_port_isolation_add;
    pMapper->port_isolation_del = dal_mango_port_isolation_del;
    pMapper->port_isolationRestrictRoute_get = dal_mango_port_isolationRestrictRoute_get;
    pMapper->port_isolationRestrictRoute_set = dal_mango_port_isolationRestrictRoute_set;
    pMapper->port_vlanBasedIsolationEntry_get = dal_mango_port_vlanBasedIsolationEntry_get;
    pMapper->port_vlanBasedIsolationEntry_set = dal_mango_port_vlanBasedIsolationEntry_set;
    pMapper->port_vlanBasedIsolationEgrBypass_get = dal_mango_port_vlanBasedIsolationEgrBypass_get;
    pMapper->port_vlanBasedIsolationEgrBypass_set = dal_mango_port_vlanBasedIsolationEgrBypass_set;

    /* port related function */
    pMapper->port_init = dal_mango_port_init;
    pMapper->port_link_get = dal_mango_port_link_get;
    pMapper->port_speedDuplex_get = dal_mango_port_speedDuplex_get;
    pMapper->port_flowctrl_get = dal_mango_port_flowctrl_get;
    pMapper->port_cpuPortId_get = dal_mango_port_cpuPortId_get;
    pMapper->port_adminEnable_get = dal_mango_port_adminEnable_get;
    pMapper->port_adminEnable_set = dal_mango_port_adminEnable_set;
    pMapper->port_backpressureEnable_get = dal_mango_port_backpressureEnable_get;
    pMapper->port_backpressureEnable_set = dal_mango_port_backpressureEnable_set;
    pMapper->port_linkChange_register = dal_mango_port_linkChange_register;
    pMapper->port_linkChange_unregister = dal_mango_port_linkChange_unregister;
    pMapper->port_txEnable_get = dal_mango_port_txEnable_get;
    pMapper->port_txEnable_set = dal_mango_port_txEnable_set;
    pMapper->port_rxEnable_get = dal_mango_port_rxEnable_get;
    pMapper->port_rxEnable_set = dal_mango_port_rxEnable_set;
    pMapper->port_specialCongest_get = dal_mango_port_specialCongest_get;
    pMapper->port_specialCongest_set = dal_mango_port_specialCongest_set;
    pMapper->port_flowCtrlEnable_get = dal_mango_port_flowCtrlEnable_get;
    pMapper->port_flowCtrlEnable_set = dal_mango_port_flowCtrlEnable_set;
    pMapper->port_linkMedia_get = dal_mango_port_linkMedia_get;
    pMapper->port_fiberOAMLoopBackEnable_set = dal_mango_port_fiberOAMLoopBackEnable_set;
    pMapper->port_fiberUnidirEnable_get = dal_mango_port_fiberUnidirEnable_get;
    pMapper->port_fiberUnidirEnable_set = dal_mango_port_fiberUnidirEnable_set;
    pMapper->port_phyAutoNegoEnable_set = dal_mango_port_phyAutoNegoEnable_set;
    pMapper->port_phyForceModeAbility_set = dal_mango_port_phyForceModeAbility_set;
    pMapper->port_phySds_set = dal_mango_port_phySds_set;

    pMapper->port_miscCtrl_get = dal_mango_port_miscCtrl_get;
    pMapper->port_miscCtrl_set = dal_mango_port_miscCtrl_set;
#else
    /* mapper for U-Boot */
    pMapper->port_isolation_set = dal_mango_port_isolation_set;
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_FINISH
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_mango_port_init(uint32 unit)
{
    #ifndef __BOOTLOADER__
    rtk_phy_macIntfSdsLinkStatus_t  phySdslinkInfo;
    rtk_sds_linkSts_t               macSdsLinkInfo;
    uint32                          phy, port, sds;
    #endif
    int32   ret;

    RT_INIT_REENTRY_CHK(port_init[unit]);
    port_init[unit] = INIT_NOT_COMPLETED;

    #ifndef __BOOTLOADER__
    osal_time_mdelay(1000);

    HWP_PHY_TRAVS(unit, phy)
    {
        port = HWP_PHY_BASE_MACID_BY_IDX(unit, phy);
        sds = HWP_PORT_SDSID(unit, port);

        if (RTK_PHYTYPE_RTL8218D_NMP == HWP_PHY_MODEL_BY_PORT(unit, port) ||
                RTK_PHYTYPE_RTL8218D == HWP_PHY_MODEL_BY_PORT(unit, port))
        {
            ret = phy_macIntfSerdesLinkStatus_get(unit, port, &phySdslinkInfo);
            if (RT_ERR_SMI == ret)
                continue;
            else if (ret != RT_ERR_OK)
                return ret;
        }
        else
            continue;

        RT_ERR_CHK(phy_rtl9310_linkSts_get(unit, sds, &macSdsLinkInfo), ret);

        if (PORT_LINKDOWN == phySdslinkInfo.link_status[0] || (macSdsLinkInfo.sts != 0x1FF) || (macSdsLinkInfo.sts1 != 0x1FF))
        {
            osal_printf("[ERR] port %d, 0x%x 0x%x 0x%x\n", port, phySdslinkInfo.link_status[0], macSdsLinkInfo.sts, macSdsLinkInfo.sts1);
            return RT_ERR_NOT_FINISH;
        }
    }
    #endif  /* __BOOTLOADER__ */

    /* create semaphore */
    port_sem[unit] = osal_sem_mutex_create();
    if (0 == port_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    pMac_info[unit] = (dal_mango_mac_info_t *)osal_alloc(sizeof(dal_mango_mac_info_t));
    if (NULL == pMac_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pMac_info[unit], 0, sizeof(dal_mango_mac_info_t));

    /* init callback function for link change */
    link_change_callback_f[unit] = 0;

    /* set init flag to complete init */
    port_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if (( ret = _dal_mango_port_init_config(unit)) != RT_ERR_OK)
        {
            port_init[unit] = INIT_NOT_COMPLETED;
            osal_free(pMac_info[unit]);
            pMac_info[unit] = NULL;
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port default configuration init failed");
            return ret;
        }
    }

    return RT_ERR_OK;
}/* end of dal_mango_port_init */

/* Function Name:
 *      dal_mango_port_link_get
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
dal_mango_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    /* read twice for latency value */
    ret = reg_array_field_read(unit, MANGO_MAC_LINK_STSr, port, REG_ARRAY_INDEX_NONE, MANGO_LINK_STSf, &val);
    if ((ret = reg_array_field_read(unit,
                          MANGO_MAC_LINK_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_LINK_STSf,
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
}/* end of dal_mango_port_link_get */

/* Function Name:
 *      dal_mango_port_txEnable_set
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
dal_mango_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    RT_ERR_CHK(drv_port_txEnable_set(unit, port, enable, DRV_RTL9310_PORT_TXRX_EN_MOD_PORT), ret);

    return RT_ERR_OK;
}/* end of dal_mango_port_txEnable_set */

/* Function Name:
 *      dal_mango_port_rxEnable_set
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
 *      This API is not supported for serdes port.
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
dal_mango_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_portmask_t  stkPmsk;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    RT_ERR_CHK(dal_mango_stack_port_get(unit, &stkPmsk), ret);
    if (RTK_PORTMASK_IS_PORT_SET(stkPmsk, port) && HWP_SERDES_PORT(unit, port))
    {
        return RT_ERR_STACK_PORT_RX_EN;
    }

    RT_ERR_CHK(drv_port_rxEnable_set(unit, port, enable, DRV_RTL9310_PORT_TXRX_EN_MOD_PORT), ret);

    return RT_ERR_OK;
}/* end of dal_mango_port_rxEnable_set */


/* Function Name:
 *      dal_mango_port_txEnable_get
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
dal_mango_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(drv_port_txEnable_get(unit, port, pEnable), ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_mango_port_txEnable_get */

/* Function Name:
 *      dal_mango_port_rxEnable_get
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
dal_mango_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(drv_port_rxEnable_get(unit, port, pEnable), ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_mango_port_rxEnable_get */

/* Function Name:
 *      dal_mango_port_specialCongest_get
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
dal_mango_port_specialCongest_get(uint32 unit, rtk_port_t port, uint32* pSecond)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    PORT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_SC_PORT_TIMERr, port, REG_ARRAY_INDEX_NONE,
            MANGO_CNGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_port_specialCongest_get */

/* Function Name:
 *      dal_mango_port_specialCongest_set
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
dal_mango_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second)
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
    if ((ret = reg_array_field_write(unit, MANGO_SC_PORT_TIMERr, port, REG_ARRAY_INDEX_NONE,
            MANGO_CNGST_SUST_TMR_LMTf, &second)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_SC_PORT_TIMERr, port, REG_ARRAY_INDEX_NONE,
            MANGO_CNGST_SUST_TMR_LMT_Hf, &second)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_port_specialCongest_set */

/* Function Name:
 *      dal_mango_port_speedDuplex_get
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
dal_mango_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    rtk_port_linkStatus_t  link_status;
    uint32  speed;
    uint32  duplex;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Check Link status */
    if ((ret = dal_mango_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
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

    /* get speed value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MANGO_MAC_LINK_SPD_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_SPD_STSf,
                          &speed)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* get duplex value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MANGO_MAC_LINK_DUP_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_DUP_STSf,
                          &duplex)) != RT_ERR_OK)
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
        case 0x7:
            *pSpeed = PORT_SPEED_1000M;
            break;
        case 0x3:
            *pSpeed = PORT_SPEED_500M;
            break;
        case 0x4:
            *pSpeed = PORT_SPEED_10G;
            break;
        case 0x5:
            *pSpeed = PORT_SPEED_2_5G;
            break;
        case 0x6:
        case 0x8:
            *pSpeed = PORT_SPEED_5G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pSpeed=%d, pDuplex=%d",
           *pSpeed, *pDuplex);

    return RT_ERR_OK;
}/* end of dal_mango_port_speedDuplex_get */

/* Function Name:
 *      dal_mango_port_flowctrl_get
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
dal_mango_port_flowctrl_get(
    uint32            unit,
    rtk_port_t        port,
    uint32            *pTxStatus,
    uint32            *pRxStatus)
{
    int32   ret;
    uint32  rxPause, txPause;
    rtk_port_linkStatus_t  link_status;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Check Link status */
    if ((ret = dal_mango_port_link_get(unit, port, &link_status)) != RT_ERR_OK)
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

    /* get tx pause value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MANGO_MAC_TX_PAUSE_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_TX_PAUSE_STSf,
                          &txPause)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* get rx pause value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MANGO_MAC_RX_PAUSE_STSr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_RX_PAUSE_STSf,
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
}/* end of dal_mango_port_flowctrl_get */

/* Function Name:
 *      dal_mango_port_cpuPortId_get
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
dal_mango_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort)
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
}

/* Function Name:
 *      dal_mango_port_isolation_get
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
dal_mango_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    int32 ret;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    index = HAL_UNIT_TO_DEV_ID(unit) * 64 + port;

    PORT_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, MANGO_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_PORT_ISO_CTRLt, MANGO_PORT_ISO_CTRL_PMSKtf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pPortmask, 0),
        RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolation_set
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
dal_mango_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{

    int32 ret;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pPortmask), RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pPortmask, 0),
        RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    index = HAL_UNIT_TO_DEV_ID(unit) * 64 + port;
    osal_memset(&port_iso_entry, 0, sizeof(port_iso_entry_t));

    PORT_SEM_LOCK(unit);

    if ((ret = table_field_set(unit, MANGO_PORT_ISO_CTRLt, MANGO_PORT_ISO_CTRL_PMSKtf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_write(unit, MANGO_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolationExt_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit                - unit id
 *      devID               - source dev id
 *      srcPort             - source port id
 * Output:
 *      pPortmask           - pointer to the portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In stacking system, each (dev, port) pair should be assigned the permit outgoing portmask of this device.
 */
int32
dal_mango_port_isolationExt_get(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask)
{
    int32 ret;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, devID=%d, port=%d", unit, devID, srcPort);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    index = devID * 64 + srcPort;

    PORT_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, MANGO_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_PORT_ISO_CTRLt, MANGO_PORT_ISO_CTRL_PMSKtf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pPortmask, 0),
        RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolationExt_set
 * Description:
 *      Set the port isolation portmask for packet received from specified unit and port.
 * Input:
 *      unit            - unit id
 *      devID           - source dev id
 *      srcPort         - source port id
 *      portmask        - pointer to the portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      In stacking system, each (dev, port) pair should be assigned the permit outgoing portmask of this device.
 */
int32
dal_mango_port_isolationExt_set(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask)
{
    int32 ret;
    uint32 index;
    port_iso_entry_t port_iso_entry;

    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pPortmask), RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, devID=%d, port=%d, pmsk0=0x%x, pmsk1=0x%x", unit, devID, srcPort,
        RTK_PORTMASK_WORD_GET(*pPortmask, 0),
        RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    index = devID * 64 + srcPort;
    osal_memset(&port_iso_entry, 0, sizeof(port_iso_entry_t));

    PORT_SEM_LOCK(unit);

    if ((ret = table_field_set(unit, MANGO_PORT_ISO_CTRLt, MANGO_PORT_ISO_CTRL_PMSKtf, &(pPortmask->bits[0]), (uint32 *) &port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_write(unit, MANGO_PORT_ISO_CTRLt, index, (uint32 *)&port_iso_entry)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "return failed ret value = %x", ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolation_add
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
dal_mango_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
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

    ret = dal_mango_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, iso_port);

    ret = dal_mango_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolation_del
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
dal_mango_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
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

    ret = dal_mango_port_isolation_get(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, iso_port);

    ret = dal_mango_port_isolation_set(unit, port, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolationRestrictRoute_get
 * Description:
 *      Get status of routed packet whether restrict by port isolation
 * Input:
 *      unit     - unit id
 * Output:
 *      pEnable - pointer to enable status of port isolation restrict route
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_mango_port_isolationRestrictRoute_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_PORT_ISO_RESTRICT_ROUTE_CTRLr, MANGO_RESTRICT_ROUTEf, pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_isolationRestrictRoute_set
 * Description:
 *      Set status of routed packet whether restrict by port isolation
 * Input:
 *      unit     - unit id
 *      enable - enable status of port isolaiton restrict route
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_mango_port_isolationRestrictRoute_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,enable=%d", unit,enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    PORT_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_PORT_ISO_RESTRICT_ROUTE_CTRLr, MANGO_RESTRICT_ROUTEf, &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_adminEnable_get
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
dal_mango_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
}

/* Function Name:
 *      dal_mango_port_adminEnable_set
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
dal_mango_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

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
        /* programming value on CHIP*/
        if ((ret = drv_port_txEnable_set(unit, port, enable,
                DRV_RTL9310_PORT_TXRX_EN_MOD_ADMIN_EN)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
        if ((ret = drv_port_rxEnable_set(unit, port, enable,
                DRV_RTL9310_PORT_TXRX_EN_MOD_ADMIN_EN)) != RT_ERR_OK)
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
        if ((ret = drv_port_txEnable_set(unit, port, enable,
                DRV_RTL9310_PORT_TXRX_EN_MOD_ADMIN_EN)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
        if ((ret = drv_port_rxEnable_set(unit, port, enable,
                DRV_RTL9310_PORT_TXRX_EN_MOD_ADMIN_EN)) != RT_ERR_OK)
        {
            PORT_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
            return ret;
        }
    }

    pMac_info[unit]->admin_enable[port] = enable;
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_backpressureEnable_get
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
dal_mango_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
                          MANGO_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_BKPRES_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_backpressureEnable_set
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
dal_mango_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
                          MANGO_MAC_PORT_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_BKPRES_ENf,
                          &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_linkChange_register
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
dal_mango_port_linkChange_register(uint32 unit, dal_link_change_callback_f link_change_callback)
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
}

/* Function Name:
 *      dal_mango_port_linkChange_unregister
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
dal_mango_port_linkChange_unregister(uint32 unit)
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
}

/* Function Name:
 *      _dal_mango_port_init_config
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
static int32
_dal_mango_port_init_config(uint32 unit)
{
    hal_control_t   *pHalCtrl;
    rtk_portmask_t  portmask;
    rtk_port_t      port;
    int32           ret, i;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    HWP_GET_ALL_PORTMASK(unit, portmask);

    for(i = 0; i < RTK_MAX_NUM_OF_UNIT; i++)
    {
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            /* Config MAC */
            if (HWP_PORT_EXIST(unit, port))
            {
                if ((ret = dal_mango_port_isolationExt_set(unit, i, port, &portmask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port init set port isolation portmask failed");
                    return ret;
                }
            }
        }
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        /* Config PHY in port that PHY exist */
        if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
        {
            if ((ret = dal_mango_port_adminEnable_set(unit, port, RTK_DEFAULT_PORT_ADMIN_ENABLE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "Port init enable port failed");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_vlanBasedIsolationEntry_get
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
dal_mango_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
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
    if ((ret = reg_array_field_read(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VB_ISO_MBR_0f, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VB_ISO_MBR_1f, &pEntry->portmask.bits[1])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VID_LOWERf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VID_UPPERf, &pEntry->vid_high)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "index=%d, enable=%d, lower vid=%d,high_vid=%d, pmsk0=0x%x, pmsk1=0x%x",
        index, pEntry->enable, pEntry->vid,pEntry->vid_high,
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 0),
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 1));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_vlanBasedIsolationEntry_set
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
dal_mango_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, index=%d", unit, index);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "enable=%d, vid=%d, ,high_vid=%d, pmsk0=0x%x, pmsk1=0x%x",
        pEntry->enable, pEntry->vid, pEntry->vid_high,
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 0),
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 1));


    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pEntry->enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, &pEntry->portmask), RT_ERR_PORT_MASK);

    PORT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VALIDf, &pEntry->enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VB_ISO_MBR_0f, &pEntry->portmask.bits[0])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VB_ISO_MBR_1f, &pEntry->portmask.bits[1])) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VID_LOWERf, &pEntry->vid)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PORT_ISO_VB_ISO_PMSK_CTRLr,
                        REG_ARRAY_INDEX_NONE, index, MANGO_VID_UPPERf, &pEntry->vid_high)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_vlanBasedIsolationEgrBypass_get
 * Description:
 *      Get egress port bypass status of VLAN-based port isolation
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to egress port bypass status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_port_vlanBasedIsolationEgrBypass_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    PORT_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          MANGO_PORT_ISO_VB_EGR_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_VB_ISO_EGR_BYPASSf,
                          pEnable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }
    PORT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_vlanBasedIsolationEgrBypass_set
 * Description:
 *      Set egress port bypass status of VLAN-based port isolation
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
dal_mango_port_vlanBasedIsolationEgrBypass_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32  ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable=%d", unit, port,enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_NULL_POINTER);

    /* set value to CHIP*/
    PORT_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          MANGO_PORT_ISO_VB_EGR_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_VB_ISO_EGR_BYPASSf,
                          &enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "ret=0x%x", ret);
        return ret;
    }
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_flowCtrlEnable_get
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
dal_mango_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
} /* end of dal_mango_port_flowCtrlEnable_get */

int32
_dal_mango_port_macForceFc_set(int32 unit, rtk_port_t port, rtk_enable_t forceFc,
    rtk_enable_t txPause, rtk_enable_t rxPause)
{
    int ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d forceFC=%u txP=%u rxP=%u", unit, port, forceFc, txPause, rxPause);

    val = (forceFc == ENABLED) ? 1 : 0;
    RT_ERR_CHK(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, &val), ret);

    if (forceFc == ENABLED)
    {
        val = (txPause == ENABLED) ? 1 : 0;
        RT_ERR_CHK(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr,
                port, REG_ARRAY_INDEX_NONE, MANGO_SMI_TX_PAUSE_ENf, &val), ret);

        val = (rxPause == ENABLED) ? 1 : 0;
        RT_ERR_CHK(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr,
                port, REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, &val), ret);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_port_macForceFc_set */

/* Function Name:
 *      dal_mango_port_flowCtrlEnable_set
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
dal_mango_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_enable_t                flowctrl_enable;
    rtk_port_phy_ability_t      ability;
    rtk_port_speed_t            speed;
    rtk_port_duplex_t           duplex;
    rtk_port_flowctrl_mode_t    fcMode;
    rtk_enable_t                nway_enable;
    int32                       ret;

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

    fcMode.tx_pause = enable;
    fcMode.rx_pause = enable;

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

    if ((ret = dal_phy_portForceModeAbility_set(unit, port, speed, duplex, enable)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    RT_ERR_CHK(dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable), ret);
    if (DISABLED == nway_enable)
    {
        PORT_SEM_LOCK(unit);
        /* Force flowcontrol */
        RT_ERR_HDL(_dal_mango_port_macForceFc_set(unit, port, ENABLED,
                fcMode.tx_pause, fcMode.rx_pause), ERR, ret);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;

ERR:
    PORT_SEM_UNLOCK(unit);
    return ret;
} /* end of dal_mango_port_flowCtrlEnable_set */

/* Function Name:
 *      dal_mango_port_linkMedia_get
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
dal_mango_port_linkMedia_get(uint32 unit, rtk_port_t port,
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
    if ((ret = reg_array_field_read(unit, MANGO_MAC_LINK_STSr, port,
                          REG_ARRAY_INDEX_NONE, MANGO_LINK_STSf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_MAC_LINK_STSr, port,
                          REG_ARRAY_INDEX_NONE, MANGO_LINK_STSf, &val)) != RT_ERR_OK)
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
        if ((ret = reg_array_field_read(unit, MANGO_MAC_LINK_MEDIA_STSr, port,
                          REG_ARRAY_INDEX_NONE, MANGO_MEDIA_STSf, &val)) != RT_ERR_OK)
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
} /* end of dal_mango_port_linkMedia_get */

/* Function Name:
 *      dal_mango_port_fiberUnidirEnable_get
 * Description:
 *      Get fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable       - - pointer to the enable status of fiber unidirection
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
dal_mango_port_fiberUnidirEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    PORT_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_FIB_UNIDIR_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_FIB_UNIDIR_ENf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_port_fiberUnidirEnable_get */

/* Function Name:
 *      dal_mango_port_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of fiber unidirection
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
dal_mango_port_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    PORT_SEM_LOCK(unit);

    if(ENABLED == enable)
        val = 1;
    else
        val = 0;

    if ((ret = dal_phy_fiberUnidirEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FIB_UNIDIR_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_FIB_UNIDIR_ENf, &val)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (HWP_PHY_EXIST(unit, port))
    {
        if (ENABLED == enable)
        {
            val = 0x2;
            RT_ERR_HDL(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, &val), ERR, ret);
            val = 1;
            RT_ERR_HDL(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, &val), ERR, ret);
        }
        else
        {
            val = 0;
            RT_ERR_HDL(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, &val), ERR, ret);
        }
    }

ERR:
    PORT_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_port_fiberUnidirEnable_set */

/* Function Name:
 *      dal_mango_port_fiberOAMLoopBackEnable_set
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
dal_mango_port_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port,
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
        dal_mango_port_rxEnable_set(unit,  port,  ENABLED);
    else
        dal_mango_port_rxEnable_set(unit,  port,  DISABLED);

    /*Step2: Disable PHY Digital Loopback or Not */
    PORT_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = dal_phy_portFiberOAMLoopBackEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        PORT_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port %u Error Code: 0x%X", port, ret);
        return ret;
    }

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_port_OAMLoopBack_set */

/* Function Name:
 *      dal_mango_port_phyAutoNegoEnable_set
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
 *         you can freely switch the mode without calling ability setting API again
 */
int32
dal_mango_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_flowctrl_mode_t    fcMode;
    rtk_enable_t                flowctrl_enable;
    rtk_port_speed_t            speed;
    rtk_port_duplex_t           duplex;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%u, port=%u, enable=%d", unit, port, enable);

    RT_ERR_CHK(dal_phy_portAutoNegoEnable_set(unit, port, enable), ret);

    if (ENABLED == enable)
    {
        PORT_SEM_LOCK(unit);
        /* Disable Force flowcontrol */
        RT_ERR_HDL(_dal_mango_port_macForceFc_set(unit, port, DISABLED,
                ENABLED, ENABLED), ERR, ret);
        PORT_SEM_UNLOCK(unit);

    }
    else
    {
        RT_ERR_CHK(dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode), ret);
        RT_ERR_CHK(dal_phy_portForceModeAbility_get(unit, port, &speed, &duplex,
                &flowctrl_enable), ret);

        if (flowctrl_enable == DISABLED)
        {
            fcMode.tx_pause = DISABLED;
            fcMode.rx_pause = DISABLED;
        }

        /* Force flowcontrol */
        PORT_SEM_LOCK(unit);
        RT_ERR_HDL(_dal_mango_port_macForceFc_set(unit, port, ENABLED,
                fcMode.tx_pause, fcMode.rx_pause), ERR, ret);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;

ERR:
    PORT_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_mango_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_mango_port_phyForceModeAbility_set
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
dal_mango_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    rtk_enable_t nway_enable;
    rtk_port_flowctrl_mode_t    fcMode;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%u,port=%u,speed=%u,duplex=%u,flowControl=%u", unit, port, speed, duplex, flowControl);

    RT_ERR_CHK(dal_phy_portForceModeAbility_set(unit, port, speed, duplex, flowControl), ret);

    RT_ERR_CHK(dal_phy_portAutoNegoEnable_get(unit, port, &nway_enable), ret);

    if (DISABLED == nway_enable)
    {
        RT_ERR_CHK(dal_phy_portForceFlowctrlMode_get(unit, port, &fcMode), ret);

        if (flowControl == DISABLED)
        {
            fcMode.tx_pause = DISABLED;
            fcMode.rx_pause = DISABLED;
        }

        /* Force flowcontrol */
        PORT_SEM_LOCK(unit);
        RT_ERR_HDL(_dal_mango_port_macForceFc_set(unit, port, ENABLED,
                fcMode.tx_pause, fcMode.rx_pause), ERR, ret);
        PORT_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;

ERR:
    PORT_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_mango_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_mango_port_sem_lock
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
int32 dal_mango_port_sem_lock(uint32 unit)
{
    PORT_SEM_LOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_sem_unlock
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
int32 dal_mango_port_sem_unlock(uint32 unit)
{
    PORT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_port_miscCtrl_get
 * Description:
 *      Get port specific misc settings
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_mango_port_miscCtrl_get(uint32 unit, rtk_port_t port,
    rtk_portMiscCtrl_t ctrl_type, uint32 *pValue)
{
    int32   ret;
    uint32  sds, asds;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,ctrl_type=%d", unit, port, ctrl_type);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_PORT_MISC_CTRL_END <= ctrl_type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    PORT_SEM_LOCK(unit);

    switch(ctrl_type)
    {
        case RTK_PORT_MISC_CTRL_IPG_STK_MODE:
            sds = HWP_PORT_SDSID(unit, port);
            RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);

            RT_ERR_HDL(SDS_FIELD_R(unit, asds, 0x6, 13, 5, 5, pValue), ERR, ret);
            break;
        default:
            PORT_SEM_UNLOCK(unit);
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

ERR:    
    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_port_miscCtrl_get */

/* Function Name:
 *      dal_mango_port_miscCtrl_set
 * Description:
 *      Set port specific misc settings
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_mango_port_miscCtrl_set(uint32 unit, rtk_port_t port,
    rtk_portMiscCtrl_t ctrl_type, uint32 value)
{
    int32   ret;
    uint32  sds, asds;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%d,ctrl_type=%d,value=%d", unit, port, ctrl_type, value);

    /* check Init status */
    RT_INIT_CHK(port_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_PORT_MISC_CTRL_END <= ctrl_type), RT_ERR_INPUT);

    /* function body */
    PORT_SEM_LOCK(unit);
    switch(ctrl_type)
    {
        case RTK_PORT_MISC_CTRL_IPG_STK_MODE:
            sds = HWP_PORT_SDSID(unit, port);
            RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);

            RT_ERR_HDL(SDS_FIELD_W(unit, asds, 0x6, 13, 5, 5, value), ERR, ret);
            break;
        default:
            PORT_SEM_UNLOCK(unit);
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

ERR:   

    PORT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_port_miscCtrl_set */




