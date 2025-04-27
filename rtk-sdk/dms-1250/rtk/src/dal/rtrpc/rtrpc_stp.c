/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spanning tree (1D, 1w, 1s)
 *
 */

#include <rtk/stp.h>
#include <dal/rtrpc/rtrpc_stp.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_stp_mstpState_get(uint32 unit, uint32 msti, rtk_port_t port, rtk_stp_state_t *pStp_state)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    stp_cfg.msti = msti;
    stp_cfg.port = port;
    GETSOCKOPT(RTDRV_STP_MSTP_STATE_GET, &stp_cfg, rtdrv_stpCfg_t, 1);
    *pStp_state = stp_cfg.stp_state;

    return RT_ERR_OK;
}

int32 rtrpc_stp_mstpState_set(uint32 unit, uint32 msti, rtk_port_t port, rtk_stp_state_t stp_state)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    stp_cfg.msti = msti;
    stp_cfg.port = port;
    stp_cfg.stp_state = stp_state;
    SETSOCKOPT(RTDRV_STP_MSTP_STATE_SET, &stp_cfg, rtdrv_stpCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stp_mstpInstance_create(uint32 unit, uint32 msti)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    stp_cfg.msti = msti;
    SETSOCKOPT(RTDRV_STP_MSTP_INSTANCE_CREATE, &stp_cfg, rtdrv_stpCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stp_mstpInstance_destroy(uint32 unit, uint32 msti)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    stp_cfg.msti = msti;
    SETSOCKOPT(RTDRV_STP_MSTP_INSTANCE_DESTROY, &stp_cfg, rtdrv_stpCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stp_isMstpInstanceExist_get(uint32 unit, uint32 msti, uint32 *pMsti_exist)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    stp_cfg.msti = msti;
    GETSOCKOPT(RTDRV_STP_MSTP_INSTANCE_EXIST_GET, &stp_cfg, rtdrv_stpCfg_t, 1);
    *pMsti_exist = stp_cfg.msti_isExist;

    return RT_ERR_OK;
}

int32 rtrpc_stp_mstpInstanceMode_get(uint32 unit, rtk_stp_mstiMode_t *pMsti_mode)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    GETSOCKOPT(RTDRV_STP_MSTP_MODE_GET, &stp_cfg, rtdrv_stpCfg_t, 1);
    *pMsti_mode = stp_cfg.msti_mode;

    return RT_ERR_OK;
}

int32 rtrpc_stp_mstpInstanceMode_set(uint32 unit, rtk_stp_mstiMode_t msti_mode)
{
    rtdrv_stpCfg_t stp_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&stp_cfg, 0, sizeof(rtdrv_stpCfg_t));
    stp_cfg.unit = unit;
    stp_cfg.msti_mode = msti_mode;
    SETSOCKOPT(RTDRV_STP_MSTP_MODE_SET, &stp_cfg, rtdrv_stpCfg_t, 1);

    return RT_ERR_OK;
}
int32
rtrpc_stpMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stp_mstpInstance_create = rtrpc_stp_mstpInstance_create;
    pMapper->stp_mstpInstance_destroy = rtrpc_stp_mstpInstance_destroy;
    pMapper->stp_isMstpInstanceExist_get = rtrpc_stp_isMstpInstanceExist_get;
    pMapper->stp_mstpState_get = rtrpc_stp_mstpState_get;
    pMapper->stp_mstpState_set = rtrpc_stp_mstpState_set;
    pMapper->stp_mstpInstanceMode_get = rtrpc_stp_mstpInstanceMode_get;
    pMapper->stp_mstpInstanceMode_set = rtrpc_stp_mstpInstanceMode_set;
    return RT_ERR_OK;
}
