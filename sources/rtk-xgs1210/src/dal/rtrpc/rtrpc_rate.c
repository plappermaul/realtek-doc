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
 * $Revision: 82414 $
 * $Date: 2017-09-22 16:44:16 +0800 (Fri, 22 Sep 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) rate
 *
 */

#include <dal/rtrpc/rtrpc_rate.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_rate_init(uint32 unit)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    SETSOCKOPT(RTDRV_RATE_INIT, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_rate_igrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_IGR_INCLUDE_IFG_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIfg_include = rate_cfg.ifg_include;

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.ifg_include = ifg_include;
    SETSOCKOPT(RTDRV_RATE_IGR_INCLUDE_IFG_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_egrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_EGR_INCLUDE_IFG_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIfg_include = rate_cfg.ifg_include;

    return RT_ERR_OK;
}

int32 rtrpc_rate_egrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.ifg_include = ifg_include;
    SETSOCKOPT(RTDRV_RATE_EGR_INCLUDE_IFG_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_STORM_INCLUDE_IFG_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIfg_include = rate_cfg.ifg_include;

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.ifg_include = ifg_include;
    SETSOCKOPT(RTDRV_RATE_STORM_INCLUDE_IFG_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.module = module;
    GETSOCKOPT(RTDRV_RATE_INCLUDE_IFG_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIfg_include = rate_cfg.ifg_include;

    return RT_ERR_OK;
}

int32 rtrpc_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.module = module;
    rate_cfg.ifg_include = ifg_include;
    SETSOCKOPT(RTDRV_RATE_INCLUDE_IFG_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrBandwidthLowThresh_get(uint32 unit, uint32 *pLowThresh)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_IGR_BWCTRL_LOW_THRESH_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pLowThresh = rate_cfg.thresh;

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrBandwidthLowThresh_set(uint32 unit, uint32 lowThresh)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.thresh = lowThresh;
    SETSOCKOPT(RTDRV_RATE_IGR_BWCTRL_LOW_THRESH_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_rate_portIgrBandwidthHighThresh_get(uint32 unit, rtk_port_t port, uint32 *pHighThresh)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_HIGH_THRESH_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pHighThresh = rate_cfg.thresh;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBandwidthHighThresh_set(uint32 unit, rtk_port_t port, uint32 highThresh)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.thresh = highThresh;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_HIGH_THRESH_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_IGR_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_IGR_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrPortBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_IGR_PORT_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrPortBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_IGR_PORT_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    osal_memcpy(pCfg, &rate_cfg.igrBwCfg, sizeof(rtk_rate_igrBwBurst_cfg_t));

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    osal_memcpy(&rate_cfg.igrBwCfg, pCfg, sizeof(rtk_rate_igrBwBurst_cfg_t));
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_EXCEED_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIsExceed = rate_cfg.isExceed;

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;

    SETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_EXCEED_RESET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.igrBypassType = bypassType;
    GETSOCKOPT(RTDRV_RATE_IGR_BWCTRL_BYPASS_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.igrBypassType = bypassType;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_IGR_BWCTRL_BYPASS_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_FLOWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_BWCTRL_FLOWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rate_igrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_IGR_QUEUE_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32
rtrpc_rate_igrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_IGR_QUEUE_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portIgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrQueueBwCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue,
    uint32                  *pIsExceed)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_EXCEED_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIsExceed = rate_cfg.isExceed;

    return RT_ERR_OK;
}

int32
rtrpc_rate_portIgrQueueBwCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    SETSOCKOPT(RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_EXCEED_RESET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_BWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_BWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_BWCTRL_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_BWCTRL_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, uint32 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pRate_mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_CPU_PORT_EGR_BWCTRL_RATE_MODE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate_mode = rate_cfg.rate_mode;

    return RT_ERR_OK;
}

int32 rtrpc_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t rate_mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.rate_mode = rate_mode;
    SETSOCKOPT(RTDRV_RATE_CPU_PORT_EGR_BWCTRL_RATE_MODE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_EGR_PORT_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_egrPortBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_EGR_PORT_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlRate_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
     *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlRate_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
     *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlMode_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_rate_assuredMode_t *pMode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    GETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_MODE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pMode = rate_cfg.assured_mode;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portEgrQueueAssuredBwCtrlMode_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_rate_assuredMode_t mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue = queue;
    rate_cfg.assured_mode = mode;
    SETSOCKOPT(RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_MODE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_egrQueueFixedBandwidthEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue   = queue;
    GETSOCKOPT(RTDRV_RATE_EGR_QUEUE_FIXED_BWCTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable    = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_egrQueueFixedBandwidthEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.queue   = queue;
    rate_cfg.enable  = enable;
    SETSOCKOPT(RTDRV_RATE_EGR_QUEUE_FIXED_BWCTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    GETSOCKOPT(RTDRV_RATE_EGR_QUEUE_BWCTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32
rtrpc_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_EGR_QUEUE_BWCTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlRate_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, uint32 *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlRate_set(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, uint32 rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlBurstSize_get(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.storm_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_STORM_CTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlBurstSize_set(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.storm_type = storm_type;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_STORM_CTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;

    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;
    rate_cfg.burst_size = burst_size;

    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlExceed_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, uint32 *pIsExceed)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;

    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_EXCEED_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIsExceed = rate_cfg.isExceed;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlExceed_reset(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;

    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_EXCEED_RESET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlRateMode_get(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;

    GETSOCKOPT(RTDRV_RATE_STORM_CTRL_RATE_MODE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate_mode = rate_cfg.storm_rate_mode;

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlRateMode_set(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.storm_rate_mode = rate_mode;
    SETSOCKOPT(RTDRV_RATE_STORM_CTRL_RATE_MODE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlRateMode_get(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;

    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_RATE_MODE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate_mode = rate_cfg.storm_rate_mode;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlRateMode_set(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_rate_mode = rate_mode;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_RATE_MODE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlTypeSel_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t *pStorm_sel)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;

    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_TYPE_SEL_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pStorm_sel = rate_cfg.storm_sel;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlTypeSel_set(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t storm_sel)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_type = storm_type;
    rate_cfg.storm_sel = storm_sel;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_TYPE_SEL_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.stormBypassType = bypassType;
    GETSOCKOPT(RTDRV_RATE_STORM_CTRL_BYPASS_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.stormBypassType = bypassType;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_STORM_CTRL_BYPASS_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoEnable_get(uint32 unit, rtk_port_t port, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoEnable_set(uint32 unit, rtk_port_t port, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoRate_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pRate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_RATE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pRate = rate_cfg.rate;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;
    rate_cfg.rate = rate;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_RATE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 *pBurst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_BURST_SIZE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pBurst_size = rate_cfg.burst_size;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  burst_size)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;
    rate_cfg.burst_size = burst_size;
    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_BURST_SIZE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoExceed_get(uint32 unit, rtk_port_t port, rtk_rate_storm_proto_group_t storm_type, uint32 *pIsExceed)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;

    GETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_EXCEED_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pIsExceed = rate_cfg.isExceed;

    return RT_ERR_OK;
}

int32 rtrpc_rate_portStormCtrlProtoExceed_reset(uint32 unit, rtk_port_t port, rtk_rate_storm_proto_group_t storm_type)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.port = port;
    rate_cfg.storm_proto_type = storm_type;

    SETSOCKOPT(RTDRV_RATE_PORT_STORM_CTRL_PROTO_EXCEED_RESET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormCtrlProtoVlanConstrtEnable_get(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t *pEnable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.storm_proto_type = storm_type;
    GETSOCKOPT(RTDRV_RATE_STORM_CTRL_PROTO_VLAN_CONSTRT_ENABLE_GET, &rate_cfg, rtdrv_rateCfg_t, 1);
    *pEnable = rate_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_rate_stormCtrlProtoVlanConstrtEnable_set(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t enable)
{
    rtdrv_rateCfg_t rate_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rate_cfg, 0, sizeof(rtdrv_rateCfg_t));
    rate_cfg.unit = unit;
    rate_cfg.storm_proto_type = storm_type;
    rate_cfg.enable = enable;
    SETSOCKOPT(RTDRV_RATE_STORM_CTRL_PROTO_VLAN_CONSTRT_ENABLE_SET, &rate_cfg, rtdrv_rateCfg_t, 1);

    return RT_ERR_OK;
}







int32
rtrpc_rateMapper_init(dal_mapper_t *pMapper)
{
    pMapper->rate_includeIfg_get = rtrpc_rate_includeIfg_get;
    pMapper->rate_includeIfg_set = rtrpc_rate_includeIfg_set;
    pMapper->rate_portIgrBwCtrlEnable_get = rtrpc_rate_portIgrBwCtrlEnable_get;
    pMapper->rate_portIgrBwCtrlEnable_set = rtrpc_rate_portIgrBwCtrlEnable_set;
    pMapper->rate_portIgrBwCtrlRate_get = rtrpc_rate_portIgrBwCtrlRate_get;
    pMapper->rate_portIgrBwCtrlRate_set = rtrpc_rate_portIgrBwCtrlRate_set;
    pMapper->rate_igrBandwidthLowThresh_get = rtrpc_rate_igrBandwidthLowThresh_get;
    pMapper->rate_igrBandwidthLowThresh_set = rtrpc_rate_igrBandwidthLowThresh_set;
    pMapper->rate_igrBwCtrlBurstSize_get = rtrpc_rate_igrBwCtrlBurstSize_get;
    pMapper->rate_igrBwCtrlBurstSize_set = rtrpc_rate_igrBwCtrlBurstSize_set;
    pMapper->rate_igrPortBwCtrlBurstSize_get = rtrpc_rate_igrPortBwCtrlBurstSize_get;
    pMapper->rate_igrPortBwCtrlBurstSize_set = rtrpc_rate_igrPortBwCtrlBurstSize_set;
    pMapper->rate_portIgrBwCtrlBurstSize_get = rtrpc_rate_portIgrBwCtrlBurstSize_get;
    pMapper->rate_portIgrBwCtrlBurstSize_set = rtrpc_rate_portIgrBwCtrlBurstSize_set;
    pMapper->rate_portIgrBandwidthCtrlExceed_get = rtrpc_rate_portIgrBandwidthCtrlExceed_get;
    pMapper->rate_portIgrBandwidthCtrlExceed_reset = rtrpc_rate_portIgrBandwidthCtrlExceed_reset;
    pMapper->rate_igrBandwidthCtrlBypass_get = rtrpc_rate_igrBandwidthCtrlBypass_get;
    pMapper->rate_igrBandwidthCtrlBypass_set = rtrpc_rate_igrBandwidthCtrlBypass_set;
    pMapper->rate_portIgrBwFlowctrlEnable_get = rtrpc_rate_portIgrBwFlowctrlEnable_get;
    pMapper->rate_portIgrBwFlowctrlEnable_set = rtrpc_rate_portIgrBwFlowctrlEnable_set;
    pMapper->rate_portIgrQueueBwCtrlEnable_get = rtrpc_rate_portIgrQueueBwCtrlEnable_get;
    pMapper->rate_portIgrQueueBwCtrlEnable_set = rtrpc_rate_portIgrQueueBwCtrlEnable_set;
    pMapper->rate_portIgrQueueBwCtrlRate_get = rtrpc_rate_portIgrQueueBwCtrlRate_get;
    pMapper->rate_portIgrQueueBwCtrlRate_set = rtrpc_rate_portIgrQueueBwCtrlRate_set;
    pMapper->rate_igrQueueBwCtrlBurstSize_get = rtrpc_rate_igrQueueBwCtrlBurstSize_get;
    pMapper->rate_igrQueueBwCtrlBurstSize_set = rtrpc_rate_igrQueueBwCtrlBurstSize_set;
    pMapper->rate_portIgrQueueBwCtrlBurstSize_get = rtrpc_rate_portIgrQueueBwCtrlBurstSize_get;
    pMapper->rate_portIgrQueueBwCtrlBurstSize_set = rtrpc_rate_portIgrQueueBwCtrlBurstSize_set;
    pMapper->rate_portIgrQueueBwCtrlExceed_get = rtrpc_rate_portIgrQueueBwCtrlExceed_get;
    pMapper->rate_portIgrQueueBwCtrlExceed_reset = rtrpc_rate_portIgrQueueBwCtrlExceed_reset;
    pMapper->rate_portEgrBwCtrlEnable_get = rtrpc_rate_portEgrBwCtrlEnable_get;
    pMapper->rate_portEgrBwCtrlEnable_set = rtrpc_rate_portEgrBwCtrlEnable_set;
    pMapper->rate_portEgrBwCtrlRate_get = rtrpc_rate_portEgrBwCtrlRate_get;
    pMapper->rate_portEgrBwCtrlRate_set = rtrpc_rate_portEgrBwCtrlRate_set;
    pMapper->rate_portEgrBwCtrlBurstSize_get = rtrpc_rate_portEgrBwCtrlBurstSize_get;
    pMapper->rate_portEgrBwCtrlBurstSize_set = rtrpc_rate_portEgrBwCtrlBurstSize_set;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_get = rtrpc_rate_cpuEgrBandwidthCtrlRateMode_get;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_set = rtrpc_rate_cpuEgrBandwidthCtrlRateMode_set;
    pMapper->rate_egrPortBwCtrlBurstSize_get = rtrpc_rate_egrPortBwCtrlBurstSize_get;
    pMapper->rate_egrPortBwCtrlBurstSize_set = rtrpc_rate_egrPortBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueBwCtrlEnable_get = rtrpc_rate_portEgrQueueBwCtrlEnable_get;
    pMapper->rate_portEgrQueueBwCtrlEnable_set = rtrpc_rate_portEgrQueueBwCtrlEnable_set;
    pMapper->rate_portEgrQueueBwCtrlRate_get = rtrpc_rate_portEgrQueueBwCtrlRate_get;
    pMapper->rate_portEgrQueueBwCtrlRate_set = rtrpc_rate_portEgrQueueBwCtrlRate_set;
    pMapper->rate_portEgrQueueBwCtrlBurstSize_get = rtrpc_rate_portEgrQueueBwCtrlBurstSize_get;
    pMapper->rate_portEgrQueueBwCtrlBurstSize_set = rtrpc_rate_portEgrQueueBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlEnable_get = rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlEnable_set = rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlRate_get = rtrpc_rate_portEgrQueueAssuredBwCtrlRate_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlRate_set = rtrpc_rate_portEgrQueueAssuredBwCtrlRate_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlBurstSize_get = rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlBurstSize_set = rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlMode_get = rtrpc_rate_portEgrQueueAssuredBwCtrlMode_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlMode_set = rtrpc_rate_portEgrQueueAssuredBwCtrlMode_set;
    pMapper->rate_egrQueueFixedBandwidthEnable_get = rtrpc_rate_egrQueueFixedBandwidthEnable_get;
    pMapper->rate_egrQueueFixedBandwidthEnable_set = rtrpc_rate_egrQueueFixedBandwidthEnable_set;
    pMapper->rate_egrQueueBwCtrlBurstSize_get = rtrpc_rate_egrQueueBwCtrlBurstSize_get;
    pMapper->rate_egrQueueBwCtrlBurstSize_set = rtrpc_rate_egrQueueBwCtrlBurstSize_set;
    pMapper->rate_portStormCtrlEnable_get = rtrpc_rate_portStormCtrlEnable_get;
    pMapper->rate_portStormCtrlEnable_set = rtrpc_rate_portStormCtrlEnable_set;
    pMapper->rate_portStormCtrlRate_get = rtrpc_rate_portStormCtrlRate_get;
    pMapper->rate_portStormCtrlRate_set = rtrpc_rate_portStormCtrlRate_set;
    pMapper->rate_portStormCtrlBurstSize_get = rtrpc_rate_portStormCtrlBurstSize_get;
    pMapper->rate_portStormCtrlBurstSize_set = rtrpc_rate_portStormCtrlBurstSize_set;
    pMapper->rate_portStormCtrlExceed_get = rtrpc_rate_portStormCtrlExceed_get;
    pMapper->rate_portStormCtrlExceed_reset = rtrpc_rate_portStormCtrlExceed_reset;
    pMapper->rate_portStormCtrlRateMode_get = rtrpc_rate_portStormCtrlRateMode_get;
    pMapper->rate_portStormCtrlRateMode_set = rtrpc_rate_portStormCtrlRateMode_set;
    pMapper->rate_portStormCtrlTypeSel_get = rtrpc_rate_portStormCtrlTypeSel_get;
    pMapper->rate_portStormCtrlTypeSel_set = rtrpc_rate_portStormCtrlTypeSel_set;
    pMapper->rate_portStormCtrlProtoEnable_get = rtrpc_rate_portStormCtrlProtoEnable_get;
    pMapper->rate_portStormCtrlProtoEnable_set = rtrpc_rate_portStormCtrlProtoEnable_set;
    pMapper->rate_portStormCtrlProtoRate_get = rtrpc_rate_portStormCtrlProtoRate_get;
    pMapper->rate_portStormCtrlProtoRate_set = rtrpc_rate_portStormCtrlProtoRate_set;
    pMapper->rate_portStormCtrlProtoBurstSize_get = rtrpc_rate_portStormCtrlProtoBurstSize_get;
    pMapper->rate_portStormCtrlProtoBurstSize_set = rtrpc_rate_portStormCtrlProtoBurstSize_set;
    pMapper->rate_portStormCtrlProtoExceed_get = rtrpc_rate_portStormCtrlProtoExceed_get;
    pMapper->rate_portStormCtrlProtoExceed_reset = rtrpc_rate_portStormCtrlProtoExceed_reset;
    pMapper->rate_stormControlBypass_get = rtrpc_rate_stormControlBypass_get;
    pMapper->rate_stormControlBypass_set = rtrpc_rate_stormControlBypass_set;
    pMapper->rate_stormControlBurstSize_get = rtrpc_rate_stormControlBurstSize_get;
    pMapper->rate_stormControlBurstSize_set = rtrpc_rate_stormControlBurstSize_set;
    pMapper->rate_stormControlRateMode_get = rtrpc_rate_stormControlRateMode_get;
    pMapper->rate_stormControlRateMode_set = rtrpc_rate_stormControlRateMode_set;
    pMapper->rate_stormCtrlProtoVlanConstrtEnable_get = rtrpc_rate_stormCtrlProtoVlanConstrtEnable_get;
    pMapper->rate_stormCtrlProtoVlanConstrtEnable_set = rtrpc_rate_stormCtrlProtoVlanConstrtEnable_set;
    return RT_ERR_OK;
}
