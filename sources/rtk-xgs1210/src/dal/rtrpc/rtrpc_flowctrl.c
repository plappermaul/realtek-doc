/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 83991 $
 * $Date: 2017-12-05 13:12:14 +0800 (Tue, 05 Dec 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) flow control
 *
 */

#include <dal/rtrpc/rtrpc_flowctrl.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_flowctrl_init(uint32 unit)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_INIT, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portPauseOnAction_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    *pAction)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_PAUSEON_ACTION_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pAction = flowctrl_cfg.pauseOn_action;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portPauseOnAction_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    action)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.pauseOn_action = action;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_PAUSEON_ACTION_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portPauseOnAllowedPageNum_get(uint32 unit, rtk_port_t port, uint32 *pPageNum)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_PAUSEON_ALLOWED_PAGENUM_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pPageNum = flowctrl_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portPauseOnAllowedPageNum_set(uint32 unit, rtk_port_t port, uint32 pageNum)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.data = pageNum;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_PAUSEON_ALLOWED_PAGENUM_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_pauseOnAllowedPktLen_get(uint32 unit, rtk_port_t port, uint32 *pPktLen)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTLEN_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pPktLen = flowctrl_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_pauseOnAllowedPktLen_set(uint32 unit, rtk_port_t port, uint32 pktLen)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.data = pktLen;
    SETSOCKOPT(RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTLEN_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_pauseOnAllowedPktNum_get(uint32 unit, rtk_port_t port, uint32 *pPktNum)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTNUM_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pPktNum = flowctrl_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_pauseOnAllowedPktNum_set(uint32 unit, rtk_port_t port, uint32 pktNum)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.data = pktNum;
    SETSOCKOPT(RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTNUM_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrGuarEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_GUAR_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrGuarEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_GUAR_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_SYS_PAUSE_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_SYS_PAUSE_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrJumboSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_JUMBO_SYS_PAUSE_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrJumboSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_JUMBO_SYS_PAUSE_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_PAUSE_THR_GROUP_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_PAUSE_THR_GROUP_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_IGR_PORT_THR_GROUP_SEL_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pGrp_idx = flowctrl_cfg.grp_idx;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.grp_idx = grp_idx;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_IGR_PORT_THR_GROUP_SEL_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_SYS_CONGEST_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_SYS_CONGEST_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrJumboSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_JUMBO_SYS_CONGEST_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrJumboSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_JUMBO_SYS_CONGEST_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_CONGEST_THR_GROUP_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_CONGEST_THR_GROUP_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_jumboModeStatus_get(uint32 unit, uint32 *pStatus)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_JUMBO_STS_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pStatus = flowctrl_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_jumboModeEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_JUMBO_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_jumboModeEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_JUMBO_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_jumboModeLength_get(uint32 unit, uint32 *pLength)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_JUMBO_LEN_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pLength = flowctrl_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_jumboModeLength_set(uint32 unit, uint32 length)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.data = length;
    SETSOCKOPT(RTDRV_FLOWCTRL_JUMBO_LEN_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrSystemUtilThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_SYS_UTIL_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.thresh, sizeof(rtk_flowctrl_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrSystemUtilThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    osal_memcpy(&flowctrl_cfg.thresh, pThresh, sizeof(rtk_flowctrl_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_SYS_UTIL_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_SYS_DROP_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    osal_memcpy(&flowctrl_cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_SYS_DROP_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrPortDropThresh_get(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrPortDropThresh_set(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    osal_memcpy(&flowctrl_cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrPortDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_DROP_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrPortDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_DROP_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrPortQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_QUEUE_DROP_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrPortQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_QUEUE_DROP_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.queue = queue;
    osal_memcpy(&flowctrl_cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_CPU_QUEUE_DROP_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.queue = queue;
    osal_memcpy(&flowctrl_cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_CPU_QUEUE_DROP_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_EGR_DROP_REFCONGEST_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_EGR_DROP_REFCONGEST_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrPortDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_GROUP_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pThresh = flowctrl_cfg.dropThresh;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrPortDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.dropThresh = *pThresh;
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_GROUP_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_GROUP_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pThresh = flowctrl_cfg.dropThresh;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.queue = queue;
    flowctrl_cfg.dropThresh = *pThresh;
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_GROUP_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portEgrDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_EGR_DROP_THR_GROUP_SEL_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pGrp_idx = flowctrl_cfg.grp_idx;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portEgrDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.grp_idx = grp_idx;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_EGR_DROP_THR_GROUP_SEL_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_EGR_QUEUE_DROP_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_egrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.queue = queue;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_EGR_QUEUE_DROP_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portEgrQueueDropForceEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_EGR_QUEUE_DROP_FORCE_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portEgrQueueDropForceEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.queue = queue;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_EGR_QUEUE_DROP_FORCE_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_DROP_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.queue = queue;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_DROP_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_DROP_THR_GROUP_SEL_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pGrp_idx = flowctrl_cfg.grp_idx;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.grp_idx = grp_idx;
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_DROP_THR_GROUP_SEL_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_GROUP_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pThresh = flowctrl_cfg.dropThresh;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.queue = queue;
    flowctrl_cfg.dropThresh = *pThresh;
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_GROUP_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueuePauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_THR_GROUP_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pThresh = flowctrl_cfg.dropThresh;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueuePauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.grp_idx = grp_idx;
    flowctrl_cfg.queue = queue;
    flowctrl_cfg.dropThresh = *pThresh;
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_THR_GROUP_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.queue = queue;
    GETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &flowctrl_cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_igrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.queue = queue;
    osal_memcpy(&flowctrl_cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    GETSOCKOPT(RTDRV_FLOWCTRL_PORT_HOL_TRAFFIC_DROP_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.port = port;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_PORT_HOL_TRAFFIC_DROP_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.type = type;
    GETSOCKOPT(RTDRV_FLOWCTRL_HOL_TRAFFIC_TYPE_DROP_ENABLE_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = flowctrl_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.type = type;
    flowctrl_cfg.enable = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_HOL_TRAFFIC_TYPE_DROP_ENABLE_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_specialCongestThreshold_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_SPECIAL_CONGEST_THR_GET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);
    *pThresh = flowctrl_cfg.dropThresh;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_specialCongestThreshold_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t flowctrl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&flowctrl_cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    flowctrl_cfg.unit = unit;
    flowctrl_cfg.dropThresh = *pThresh;
    SETSOCKOPT(RTDRV_FLOWCTRL_SPECIAL_CONGEST_THR_SET, &flowctrl_cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_e2eCascadePortThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_FLOWCTRL_E2E_CASCADE_PORT_THRESH_GET, &cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_e2eCascadePortThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_E2E_CASCADE_PORT_THRESH_SET, &cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.grp_idx, &grp_idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_FLOWCTRL_E2E_REMOTE_PORTPAUSETHRESHGROUP_GET, &cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.grp_idx, &grp_idx, sizeof(uint32));
    osal_memcpy(&cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_E2E_REMOTE_PORTPAUSETHRESHGROUP_SET, &cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.grp_idx, &grp_idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_FLOWCTRL_E2E_REMOTE_PORTCONGESTTHRESHGROUP_GET, &cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pThresh, &cfg.dropThresh, sizeof(rtk_flowctrl_drop_thresh_t));

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.grp_idx, &grp_idx, sizeof(uint32));
    osal_memcpy(&cfg.dropThresh, pThresh, sizeof(rtk_flowctrl_drop_thresh_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_E2E_REMOTE_PORTCONGESTTHRESHGROUP_SET, &cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGrp_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_FLOWCTRL_E2E_PORT_REMOTE_PORT_THRESH_GROUP_SEL_GET, &cfg, rtdrv_flowctrlCfg_t, 1);
    osal_memcpy(pGrp_idx, &cfg.grp_idx, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.grp_idx, &grp_idx, sizeof(uint32));
    SETSOCKOPT(RTDRV_FLOWCTRL_E2E_PORT_REMOTE_PORT_THRESH_GROUP_SEL_SET, &cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_tagPauseEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_FLOWCTRL_TAGPAUSE_ENABLE_GET, &cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = cfg.tag_pause_en;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_tagPauseEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tag_pause_en, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_FLOWCTRL_TAGPAUSE_ENABLE_SET, &cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_halfConsecutiveRetryEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_FLOWCTRL_HALF_CONSECUTIVE_RETRY_ENABLE_GET, &cfg, rtdrv_flowctrlCfg_t, 1);
    *pEnable = cfg.half_retry_en;

    return RT_ERR_OK;
}

int32
rtrpc_flowctrl_halfConsecutiveRetryEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_flowctrlCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_flowctrlCfg_t));
    cfg.unit = unit;
    cfg.half_retry_en = enable;
    SETSOCKOPT(RTDRV_FLOWCTRL_HALF_CONSECUTIVE_RETRY_ENABLE_SET, &cfg, rtdrv_flowctrlCfg_t, 1);

    return RT_ERR_OK;
}



int32
rtrpc_flowctrlMapper_init(dal_mapper_t *pMapper)
{
    pMapper->flowctrl_portPauseOnAction_get = rtrpc_flowctrl_portPauseOnAction_get;
    pMapper->flowctrl_portPauseOnAction_set = rtrpc_flowctrl_portPauseOnAction_set;
    pMapper->flowctrl_portPauseOnAllowedPageNum_get = rtrpc_flowctrl_portPauseOnAllowedPageNum_get;
    pMapper->flowctrl_portPauseOnAllowedPageNum_set = rtrpc_flowctrl_portPauseOnAllowedPageNum_set;
    pMapper->flowctrl_pauseOnAllowedPktLen_get = rtrpc_flowctrl_pauseOnAllowedPktLen_get;
    pMapper->flowctrl_pauseOnAllowedPktLen_set = rtrpc_flowctrl_pauseOnAllowedPktLen_set;
    pMapper->flowctrl_pauseOnAllowedPktNum_get = rtrpc_flowctrl_pauseOnAllowedPktNum_get;
    pMapper->flowctrl_pauseOnAllowedPktNum_set = rtrpc_flowctrl_pauseOnAllowedPktNum_set;
    pMapper->flowctrl_igrGuarEnable_get = rtrpc_flowctrl_igrGuarEnable_get;
    pMapper->flowctrl_igrGuarEnable_set = rtrpc_flowctrl_igrGuarEnable_set;
    pMapper->flowctrl_igrSystemPauseThresh_get = rtrpc_flowctrl_igrSystemPauseThresh_get;
    pMapper->flowctrl_igrSystemPauseThresh_set = rtrpc_flowctrl_igrSystemPauseThresh_set;
    pMapper->flowctrl_igrJumboSystemPauseThresh_get = rtrpc_flowctrl_igrJumboSystemPauseThresh_get;
    pMapper->flowctrl_igrJumboSystemPauseThresh_set = rtrpc_flowctrl_igrJumboSystemPauseThresh_set;
    pMapper->flowctrl_igrPauseThreshGroup_get = rtrpc_flowctrl_igrPauseThreshGroup_get;
    pMapper->flowctrl_igrPauseThreshGroup_set = rtrpc_flowctrl_igrPauseThreshGroup_set;
    pMapper->flowctrl_portIgrPortThreshGroupSel_get = rtrpc_flowctrl_portIgrPortThreshGroupSel_get;
    pMapper->flowctrl_portIgrPortThreshGroupSel_set = rtrpc_flowctrl_portIgrPortThreshGroupSel_set;
    pMapper->flowctrl_igrSystemCongestThresh_get = rtrpc_flowctrl_igrSystemCongestThresh_get;
    pMapper->flowctrl_igrSystemCongestThresh_set = rtrpc_flowctrl_igrSystemCongestThresh_set;
    pMapper->flowctrl_igrJumboSystemCongestThresh_get = rtrpc_flowctrl_igrJumboSystemCongestThresh_get;
    pMapper->flowctrl_igrJumboSystemCongestThresh_set = rtrpc_flowctrl_igrJumboSystemCongestThresh_set;
    pMapper->flowctrl_igrCongestThreshGroup_get = rtrpc_flowctrl_igrCongestThreshGroup_get;
    pMapper->flowctrl_igrCongestThreshGroup_set = rtrpc_flowctrl_igrCongestThreshGroup_set;
    pMapper->flowctrl_egrSystemDropThresh_get = rtrpc_flowctrl_egrSystemDropThresh_get;
    pMapper->flowctrl_egrSystemDropThresh_set = rtrpc_flowctrl_egrSystemDropThresh_set;
    pMapper->flowctrl_egrPortDropThresh_get = rtrpc_flowctrl_egrPortDropThresh_get;
    pMapper->flowctrl_egrPortDropThresh_set = rtrpc_flowctrl_egrPortDropThresh_set;
    pMapper->flowctrl_egrPortDropEnable_get = rtrpc_flowctrl_egrPortDropEnable_get;
    pMapper->flowctrl_egrPortDropEnable_set = rtrpc_flowctrl_egrPortDropEnable_set;
    pMapper->flowctrl_egrPortQueueDropEnable_get = rtrpc_flowctrl_egrPortQueueDropEnable_get;
    pMapper->flowctrl_egrPortQueueDropEnable_set = rtrpc_flowctrl_egrPortQueueDropEnable_set;
    pMapper->flowctrl_egrQueueDropThresh_get = rtrpc_flowctrl_egrQueueDropThresh_get;
    pMapper->flowctrl_egrQueueDropThresh_set = rtrpc_flowctrl_egrQueueDropThresh_set;
    pMapper->flowctrl_egrCpuQueueDropThresh_get = rtrpc_flowctrl_egrCpuQueueDropThresh_get;
    pMapper->flowctrl_egrCpuQueueDropThresh_set = rtrpc_flowctrl_egrCpuQueueDropThresh_set;
    pMapper->flowctrl_portEgrDropRefCongestEnable_get = rtrpc_flowctrl_portEgrDropRefCongestEnable_get;
    pMapper->flowctrl_portEgrDropRefCongestEnable_set = rtrpc_flowctrl_portEgrDropRefCongestEnable_set;
    pMapper->flowctrl_egrPortDropThreshGroup_get = rtrpc_flowctrl_egrPortDropThreshGroup_get;
    pMapper->flowctrl_egrPortDropThreshGroup_set = rtrpc_flowctrl_egrPortDropThreshGroup_set;
    pMapper->flowctrl_egrQueueDropThreshGroup_get = rtrpc_flowctrl_egrQueueDropThreshGroup_get;
    pMapper->flowctrl_egrQueueDropThreshGroup_set = rtrpc_flowctrl_egrQueueDropThreshGroup_set;
    pMapper->flowctrl_portEgrDropThreshGroupSel_get = rtrpc_flowctrl_portEgrDropThreshGroupSel_get;
    pMapper->flowctrl_portEgrDropThreshGroupSel_set = rtrpc_flowctrl_portEgrDropThreshGroupSel_set;
    pMapper->flowctrl_egrQueueDropEnable_get = rtrpc_flowctrl_egrQueueDropEnable_get;
    pMapper->flowctrl_egrQueueDropEnable_set = rtrpc_flowctrl_egrQueueDropEnable_set;
    pMapper->flowctrl_portEgrQueueDropForceEnable_get = rtrpc_flowctrl_portEgrQueueDropForceEnable_get;
    pMapper->flowctrl_portEgrQueueDropForceEnable_set = rtrpc_flowctrl_portEgrQueueDropForceEnable_set;
    pMapper->flowctrl_igrQueueDropEnable_get = rtrpc_flowctrl_igrQueueDropEnable_get;
    pMapper->flowctrl_igrQueueDropEnable_set = rtrpc_flowctrl_igrQueueDropEnable_set;
    pMapper->flowctrl_igrQueuePauseDropThreshGroupSel_get = rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_get;
    pMapper->flowctrl_igrQueuePauseDropThreshGroupSel_set = rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_set;
    pMapper->flowctrl_igrQueueDropThreshGroup_get = rtrpc_flowctrl_igrQueueDropThreshGroup_get;
    pMapper->flowctrl_igrQueueDropThreshGroup_set = rtrpc_flowctrl_igrQueueDropThreshGroup_set;
    pMapper->flowctrl_igrQueuePauseThreshGroup_get = rtrpc_flowctrl_igrQueuePauseThreshGroup_get;
    pMapper->flowctrl_igrQueuePauseThreshGroup_set = rtrpc_flowctrl_igrQueuePauseThreshGroup_set;
    pMapper->flowctrl_igrQueueDropThresh_get = rtrpc_flowctrl_igrQueueDropThresh_get;
    pMapper->flowctrl_igrQueueDropThresh_set = rtrpc_flowctrl_igrQueueDropThresh_set;
    pMapper->flowctrl_portHolTrafficDropEnable_get = rtrpc_flowctrl_portHolTrafficDropEnable_get;
    pMapper->flowctrl_portHolTrafficDropEnable_set = rtrpc_flowctrl_portHolTrafficDropEnable_set;
    pMapper->flowctrl_holTrafficTypeDropEnable_get = rtrpc_flowctrl_holTrafficTypeDropEnable_get;
    pMapper->flowctrl_holTrafficTypeDropEnable_set = rtrpc_flowctrl_holTrafficTypeDropEnable_set;
    pMapper->flowctrl_specialCongestThreshold_get = rtrpc_flowctrl_specialCongestThreshold_get;
    pMapper->flowctrl_specialCongestThreshold_set = rtrpc_flowctrl_specialCongestThreshold_set;
    pMapper->flowctrl_e2eCascadePortThresh_get = rtrpc_flowctrl_e2eCascadePortThresh_get;
    pMapper->flowctrl_e2eCascadePortThresh_set = rtrpc_flowctrl_e2eCascadePortThresh_set;
    pMapper->flowctrl_e2eRemotePortPauseThreshGroup_get = rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_get;
    pMapper->flowctrl_e2eRemotePortPauseThreshGroup_set = rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_set;
    pMapper->flowctrl_e2eRemotePortCongestThreshGroup_get = rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_get;
    pMapper->flowctrl_e2eRemotePortCongestThreshGroup_set = rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_set;
    pMapper->flowctrl_portE2eRemotePortThreshGroupSel_get = rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_get;
    pMapper->flowctrl_portE2eRemotePortThreshGroupSel_set = rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_set;
    pMapper->flowctrl_tagPauseEnable_get = rtrpc_flowctrl_tagPauseEnable_get;
    pMapper->flowctrl_tagPauseEnable_set = rtrpc_flowctrl_tagPauseEnable_set;
    pMapper->flowctrl_halfConsecutiveRetryEnable_get = rtrpc_flowctrl_halfConsecutiveRetryEnable_get;
    pMapper->flowctrl_halfConsecutiveRetryEnable_set = rtrpc_flowctrl_halfConsecutiveRetryEnable_set;
    return RT_ERR_OK;
}
