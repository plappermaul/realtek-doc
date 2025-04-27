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
 *           1) counter
 *
 */

#include <dal/rtrpc/rtrpc_counter.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_stat_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    GETSOCKOPT(RTDRV_COUNTER_ENABLE_GET, &counter_cfg, rtdrv_counterCfg_t, 1);
    *pEnable = counter_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_stat_enable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.enable = enable;
    SETSOCKOPT(RTDRV_COUNTER_ENABLE_SET, &counter_cfg, rtdrv_counterCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.cntr_idx = cntr_idx;
    GETSOCKOPT(RTDRV_COUNTER_GLOBAL_GET, &counter_cfg, rtdrv_counterCfg_t, 1);
    osal_memcpy(pCntr, &counter_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
}

int32 rtrpc_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    GETSOCKOPT(RTDRV_COUNTER_GLOBAL_GETALL, &counter_cfg, rtdrv_counterCfg_t, 1);
    osal_memcpy(pGlobal_cntrs, &counter_cfg.global_cnt, sizeof(rtk_stat_global_cntr_t));

    return RT_ERR_OK;
}

int32 rtrpc_stat_global_reset(uint32 unit)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    SETSOCKOPT(RTDRV_COUNTER_GLOBAL_RESET, &counter_cfg, rtdrv_counterCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.port = port;
    counter_cfg.cntr_idx = cntr_idx;
    GETSOCKOPT(RTDRV_COUNTER_PORT_GET, &counter_cfg, rtdrv_counterCfg_t, 1);
    osal_memcpy(pCntr, &counter_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
}

int32 rtrpc_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.port = port;
    GETSOCKOPT(RTDRV_COUNTER_PORT_GETALL, &counter_cfg, rtdrv_counterCfg_t, 1);
    osal_memcpy(pPort_cntrs, &counter_cfg.port_cnt, sizeof(rtk_stat_port_cntr_t));

    return RT_ERR_OK;
}

int32 rtrpc_stat_port_reset(uint32 unit, rtk_port_t port)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.port = port;
    SETSOCKOPT(RTDRV_COUNTER_PORT_RESET, &counter_cfg, rtdrv_counterCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.tagCnt_type = tagCnt_type;
    GETSOCKOPT(RTDRV_COUNTER_TAGLENCNT_GET, &counter_cfg, rtdrv_counterCfg_t, 1);
    *pEnable = counter_cfg.enable;

    return RT_ERR_OK;
} /* end of rtk_stat_tagLenCntIncEnable_get */

int32 rtrpc_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.tagCnt_type = tagCnt_type;
    counter_cfg.enable = enable;
    SETSOCKOPT(RTDRV_COUNTER_TAGLENCNT_SET, &counter_cfg, rtdrv_counterCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_stat_tagLenCntIncEnable_set */

int32 rtrpc_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.type = type;
    GETSOCKOPT(RTDRV_COUNTER_STACKHDRLENCNT_GET, &counter_cfg, rtdrv_counterCfg_t, 1);
    *pEnable = counter_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.type = type;
    counter_cfg.enable = enable;
    SETSOCKOPT(RTDRV_COUNTER_STACKHDRLENCNT_SET, &counter_cfg, rtdrv_counterCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.cntr_idx = idx;
    GETSOCKOPT(RTDRV_COUNTER_FLEXCNTR_CFG_GET, &counter_cfg, rtdrv_counterCfg_t, 1);
    osal_memcpy(pRange, &counter_cfg.range, sizeof(rtk_stat_flexCntSet_t));

    return RT_ERR_OK;
} /* end of rtk_stat_flexibleCntRange_get */

int32 rtrpc_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    rtdrv_counterCfg_t counter_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&counter_cfg, 0, sizeof(rtdrv_counterCfg_t));
    counter_cfg.unit = unit;
    counter_cfg.cntr_idx = idx;
    osal_memcpy(&counter_cfg.range, pRange, sizeof(rtk_stat_flexCntSet_t));
    SETSOCKOPT(RTDRV_COUNTER_FLEXCNTR_CFG_SET, &counter_cfg, rtdrv_counterCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_stat_flexibleCntRange_set */

