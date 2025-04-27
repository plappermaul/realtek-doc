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
 *
 * $Revision: 80330 $
 * $Date: 2017-07-03 16:27:03 +0800 (Mon, 03 Jul 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) eee
 *
 */

#include <rtk/eee.h>
#include <dal/rtrpc/rtrpc_eee.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_eee_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_eeeCfg_t eee_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&eee_cfg, 0, sizeof(eee_cfg));
    eee_cfg.unit = unit;
    eee_cfg.port = port;
    GETSOCKOPT(RTDRV_EEE_PORT_ENABLE_GET, &eee_cfg, rtdrv_eeeCfg_t, 1);
    *pEnable = eee_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_eee_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_eeeCfg_t eee_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&eee_cfg, 0, sizeof(eee_cfg));
    eee_cfg.unit = unit;
    eee_cfg.port = port;
    eee_cfg.enable = enable;
    SETSOCKOPT(RTDRV_EEE_PORT_ENABLE_SET, &eee_cfg, rtdrv_eeeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_eee_portState_get(
    uint32 unit,
    rtk_port_t port,
    rtk_enable_t *pState)
{
    rtdrv_eeeCfg_t eee_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&eee_cfg, 0, sizeof(eee_cfg));
    eee_cfg.unit = unit;
    eee_cfg.port = port;

    GETSOCKOPT(RTDRV_EEE_PORT_STATE_GET, &eee_cfg, rtdrv_eeeCfg_t, 1);

    *pState = eee_cfg.enable;

    return RT_ERR_OK;
}    /* end of rtk_eee_portState_get */

int32 rtrpc_eeep_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_eeeCfg_t eee_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&eee_cfg, 0, sizeof(eee_cfg));
    eee_cfg.unit = unit;
    eee_cfg.port = port;
    GETSOCKOPT(RTDRV_EEEP_PORT_ENABLE_GET, &eee_cfg, rtdrv_eeeCfg_t, 1);
    *pEnable = eee_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_eee_portPowerState_get(
    uint32 unit,
    rtk_port_t port,
    rtk_eee_direction_t direction,
    rtk_eee_power_state_t *pState)
{
    rtdrv_eeeCfg_t eee_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&eee_cfg, 0, sizeof(eee_cfg));
    eee_cfg.unit = unit;
    eee_cfg.port = port;
    eee_cfg.direction = direction;
    GETSOCKOPT(RTDRV_EEE_PORT_POWER_STATE_GET, &eee_cfg, rtdrv_eeeCfg_t, 1);
    *pState = eee_cfg.state;

    return RT_ERR_OK;
}   /* end of rtk_eee_portPowerState_get */


int32 rtrpc_eeep_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_eeeCfg_t eee_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&eee_cfg, 0, sizeof(eee_cfg));
    eee_cfg.unit = unit;
    eee_cfg.port = port;
    eee_cfg.enable = enable;
    SETSOCKOPT(RTDRV_EEEP_PORT_ENABLE_SET, &eee_cfg, rtdrv_eeeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_eeeMapper_init(dal_mapper_t *pMapper)
{
    pMapper->eee_portEnable_get = rtrpc_eee_portEnable_get;
    pMapper->eee_portEnable_set = rtrpc_eee_portEnable_set;
    pMapper->eee_portState_get = rtrpc_eee_portState_get;
    pMapper->eee_portPowerState_get = rtrpc_eee_portPowerState_get;
    pMapper->eeep_portEnable_get = rtrpc_eeep_portEnable_get;
    pMapper->eeep_portEnable_set = rtrpc_eeep_portEnable_set;
    return RT_ERR_OK;
}
