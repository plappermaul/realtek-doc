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
 * $Revision: 83418 $
 * $Date: 2017-11-14 18:05:06 +0800 (Tue, 14 Nov 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) QoS
 *
 */

#include <dal/rtrpc/rtrpc_qos.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_qos_queueNum_get(uint32 unit, uint32 *pQueue_num)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_QUEUE_NUM_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pQueue_num = qos_cfg.queue_num;

    return RT_ERR_OK;
}

int32 rtrpc_qos_queueNum_set(uint32 unit, uint32 queue_num)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue_num = queue_num;
    SETSOCKOPT(RTDRV_QOS_QUEUE_NUM_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_priMap_get(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue_num = queue_num;
    GETSOCKOPT(RTDRV_QOS_PRI_MAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pPri2qid, &qos_cfg.pri2qid, sizeof(rtk_qos_pri2queue_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_priMap_set(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue_num = queue_num;
    osal_memcpy(&qos_cfg.pri2qid, pPri2qid, sizeof(rtk_qos_pri2queue_t));
    SETSOCKOPT(RTDRV_QOS_PRI_MAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dot1p_pri = dot1p_pri;
    GETSOCKOPT(RTDRV_QOS_1P_PRI_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pInt_pri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t int_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dot1p_pri = dot1p_pri;
    qos_cfg.int_pri = int_pri;
    SETSOCKOPT(RTDRV_QOS_1P_PRI_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = pri;
    GETSOCKOPT(RTDRV_QOS_PRI2QID_MAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pQid = qos_cfg.queue;

    return RT_ERR_OK;
}

int32 rtrpc_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = pri;
    qos_cfg.queue = qid;
    SETSOCKOPT(RTDRV_QOS_PRI2QID_MAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_cpuQid2QidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.cpuQid = cpuQid;
    GETSOCKOPT(RTDRV_QOS_CPUQID2QID_MAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pQid = qos_cfg.queue;

    return RT_ERR_OK;
}

int32 rtrpc_qos_cpuQid2QidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.cpuQid = cpuQid;
    qos_cfg.queue = qid;
    SETSOCKOPT(RTDRV_QOS_CPUQID2QID_MAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_cpuQid2StackQidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.cpuQid = cpuQid;
    GETSOCKOPT(RTDRV_QOS_CPUQID2SQID_MAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pQid = qos_cfg.queue;

    return RT_ERR_OK;
}

int32 rtrpc_qos_cpuQid2StackQidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.cpuQid = cpuQid;
    qos_cfg.queue = qid;
    SETSOCKOPT(RTDRV_QOS_CPUQID2SQID_MAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_dpSrcSel_get(uint32 unit, rtk_qos_dpSrc_t *pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_DP_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.dpSrcType;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dpSrcSel_set(uint32 unit, rtk_qos_dpSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dpSrcType = type;
    SETSOCKOPT(RTDRV_QOS_DP_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.index = grp_idx;
    GETSOCKOPT(RTDRV_QOS_PRI_SEL_GROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pWeightOfPriSel, &(qos_cfg.priSelWeight), sizeof(rtk_qos_priSelWeight_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.index = grp_idx;
    osal_memcpy(&(qos_cfg.priSelWeight), pWeightOfPriSel, sizeof(rtk_qos_priSelWeight_t));
    SETSOCKOPT(RTDRV_QOS_PRI_SEL_GROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_PRI_SEL_GROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPriSelGrp_idx = qos_cfg.index;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.index = priSelGrp_idx;
    SETSOCKOPT(RTDRV_QOS_PORT_PRI_SEL_GROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_portInnerPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_INNER_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portInnerPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.int_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PORT_INNER_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuterPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUTER_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuterPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.int_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PORT_OUTER_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_deiDpRemap_get(uint32 unit, uint32 dei, uint32 * pDp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dei = dei;
    GETSOCKOPT(RTDRV_QOS_DEI_DP_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDp = qos_cfg.dp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiDpRemap_set(uint32 unit, uint32 dei, uint32 dp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dei = dei;
    qos_cfg.dp = dp;
    SETSOCKOPT(RTDRV_QOS_DEI_DP_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDEISrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_DEI_SRC_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.deiSrc;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDEISrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.deiSrc = type;
    SETSOCKOPT(RTDRV_QOS_PORT_DEI_SRC_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDpSel_get(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_DP_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pWeightOfDpSel, &(qos_cfg.weightOfDpSel), sizeof(rtk_qos_dpSelWeight_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDpSel_set(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    osal_memcpy(&(qos_cfg.weightOfDpSel), pWeightOfDpSel, sizeof(rtk_qos_dpSelWeight_t));
    SETSOCKOPT(RTDRV_QOS_PORT_DP_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_dscpDpRemap_get(uint32 unit, uint32 dscp, uint32 *pDp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dscp = dscp;
    GETSOCKOPT(RTDRV_QOS_DSCP_DP_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDp = qos_cfg.dp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpDpRemap_set(uint32 unit, uint32 dscp, uint32 dp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dscp = dscp;
    qos_cfg.dp = dp;
    SETSOCKOPT(RTDRV_QOS_DSCP_DP_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_dpRemap_get(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 * pDp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dpSrcType = src;
    qos_cfg.dpSrcRemap = srcVal;
    GETSOCKOPT(RTDRV_QOS_DP_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDp = qos_cfg.dp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dpRemap_set(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 dp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dpSrcType = src;
    qos_cfg.dpSrcRemap = srcVal;
    qos_cfg.dp = dp;
    SETSOCKOPT(RTDRV_QOS_DP_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.priSrcType = src;
    qos_cfg.priSrcRemap = srcVal;
    GETSOCKOPT(RTDRV_QOS_PRI_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.priSrcType = src;
    qos_cfg.priSrcRemap = srcVal;
    qos_cfg.int_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PRI_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_dscpPriRemap_get(uint32 unit, uint32 dscp, rtk_pri_t *pInt_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dscp = dscp;
    GETSOCKOPT(RTDRV_QOS_DSCP_PRI_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pInt_pri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpPriRemap_set(uint32 unit, uint32 dscp, rtk_pri_t int_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dscp = dscp;
    qos_cfg.int_pri = int_pri;
    SETSOCKOPT(RTDRV_QOS_DSCP_PRI_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_portPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pInt_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pInt_pri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portPri_set(uint32 unit, rtk_port_t port, rtk_pri_t int_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.int_pri = int_pri;
    SETSOCKOPT(RTDRV_QOS_PORT_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_outer1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t * pInt_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dei  = dei;
    qos_cfg.dot1p_pri = dot1p_pri;
    GETSOCKOPT(RTDRV_QOS_OUTER_1P_PRI_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pInt_pri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_outer1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t int_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dei = dei;
    qos_cfg.dot1p_pri = dot1p_pri;
    qos_cfg.int_pri = int_pri;
    SETSOCKOPT(RTDRV_QOS_OUTER_1P_PRI_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_1P_REMARK_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.remark_enable;

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.remark_enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_1P_REMARK_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_1p = src;
    qos_cfg.rmkval_1p = val;
    GETSOCKOPT(RTDRV_QOS_1P_REMARKING_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.dot1p_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_1p = src;
    qos_cfg.rmkval_1p = val;
    qos_cfg.dot1p_pri = pri;
    SETSOCKOPT(RTDRV_QOS_1P_REMARKING_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.dot1p_dflt_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.dot1p_dflt_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_EXT_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.dot1p_dflt_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    qos_cfg.dot1p_dflt_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_EXT_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pSrc = qos_cfg.dflt_src_1p;

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.dflt_src_1p = src;
    SETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_EXT_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pSrc = qos_cfg.dflt_src_1p;

    return RT_ERR_OK;
}

int32 rtrpc_qos_port1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    qos_cfg.dflt_src_1p = src;
    SETSOCKOPT(RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_EXT_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_1pDfltCfgSrc_t *pCfg)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_1P_DFLT_PRI_CFG_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pCfg = qos_cfg.dot1p_dflt_cfg_dir;

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_1pDfltCfgSrc_t cfg)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dot1p_dflt_cfg_dir = cfg;
    SETSOCKOPT(RTDRV_QOS_1P_DFLT_PRI_CFG_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUT_1P_REMARK_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.remark_enable;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.remark_enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_OUT_1P_REMARK_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_outer1p = src;
    qos_cfg.rmkval_outer1p = val;
    GETSOCKOPT(RTDRV_QOS_OUTER_1P_REMARKING_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.dot1p_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_outer1p = src;
    qos_cfg.rmkval_outer1p = val;
    qos_cfg.dot1p_pri = pri;
    SETSOCKOPT(RTDRV_QOS_OUTER_1P_REMARKING_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.out1p_dflt_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.out1p_dflt_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_EXT_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri = qos_cfg.out1p_dflt_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    qos_cfg.out1p_dflt_pri = pri;
    SETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_EXT_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_DSCP_REMARK_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.remark_enable;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.remark_enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_DSCP_REMARK_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_dscp = src;
    qos_cfg.rmkval_dscp = val;
    GETSOCKOPT(RTDRV_QOS_DSCP_REMARKING_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDscp = qos_cfg.dscp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_dscp = src;
    qos_cfg.rmkval_dscp = val;
    qos_cfg.dscp = dscp;
    SETSOCKOPT(RTDRV_QOS_DSCP_REMARKING_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDeiRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_DEI_REMARK_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.remark_enable;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDeiRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.remark_enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_DEI_REMARK_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiRemarking_get(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 *pDei)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_dei = src;
    qos_cfg.rmkval_dei = val;
    GETSOCKOPT(RTDRV_QOS_DEI_REMARKING_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDei = qos_cfg.dei;

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiRemarking_set(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 dei)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_dei = src;
    qos_cfg.rmkval_dei = val;
    qos_cfg.dei = dei;
    SETSOCKOPT(RTDRV_QOS_DEI_REMARKING_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiRemarkSrcSel_get(uint32 unit, rtk_qos_deiRmkSrc_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_DEI_REMARK_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.rmksrc_dei;

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiRemarkSrcSel_set(uint32 unit, rtk_qos_deiRmkSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_dei = type;
    SETSOCKOPT(RTDRV_QOS_DEI_REMARK_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pDfltPri_get(uint32 unit, rtk_pri_t * pDot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_1P_DFLT_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDot1p_pri = qos_cfg.dot1p_dflt_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dot1p_dflt_pri = dot1p_pri;
    SETSOCKOPT(RTDRV_QOS_1P_DFLT_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_1P_REMARK_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.rmksrc_1p;

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_1p = type;
    SETSOCKOPT(RTDRV_QOS_1P_REMARK_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = int_pri;
    GETSOCKOPT(RTDRV_QOS_1P_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDot1p_pri = qos_cfg.dot1p_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = int_pri;
    qos_cfg.dot1p_pri = dot1p_pri;
    SETSOCKOPT(RTDRV_QOS_1P_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_outer1pRemarkSrcSel_get(uint32 unit, rtk_qos_outer1pRmkSrc_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_OUT_1P_REMARK_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.rmksrc_outer1p;

    return RT_ERR_OK;
}

int32 rtrpc_qos_outer1pRemarkSrcSel_set(uint32 unit, rtk_qos_outer1pRmkSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_outer1p = type;
    SETSOCKOPT(RTDRV_QOS_OUT_1P_REMARK_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_DFLT_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.out1p_dflt_src;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.out1p_dflt_src = type;
    SETSOCKOPT(RTDRV_QOS_PORT_OUT_1P_DFLT_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_DFLT_SRC_SEL_EXT_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.out1p_dflt_src;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portOuter1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.devID = devID;
    qos_cfg.port = port;
    qos_cfg.out1p_dflt_src = type;
    SETSOCKOPT(RTDRV_QOS_PORT_OUT_1P_DFLT_SRC_SEL_EXT_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_DSCP_REMARK_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.rmksrc_dscp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.rmksrc_dscp = type;
    SETSOCKOPT(RTDRV_QOS_DSCP_REMARK_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = int_pri;
    GETSOCKOPT(RTDRV_QOS_DSCP_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDscp = qos_cfg.dscp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = int_pri;
    qos_cfg.dscp = dscp;
    SETSOCKOPT(RTDRV_QOS_DSCP_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscp2Dot1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.org_dscp = dscp;
    GETSOCKOPT(RTDRV_QOS_DSCP2DOT1P_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDot1p_pri = qos_cfg.dot1p_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscp2Dot1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.org_dscp = dscp;
    qos_cfg.dot1p_pri = dot1p_pri;
    SETSOCKOPT(RTDRV_QOS_DSCP2DOT1P_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscp2Outer1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.org_dscp = dscp;
    GETSOCKOPT(RTDRV_QOS_DSCP2OUT1P_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDot1p_pri = qos_cfg.dot1p_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscp2Outer1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.org_dscp = dscp;
    qos_cfg.dot1p_pri = dot1p_pri;
    SETSOCKOPT(RTDRV_QOS_DSCP2OUT1P_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.org_dscp = dscp;
    GETSOCKOPT(RTDRV_QOS_DSCP2DSCP_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDscp = qos_cfg.dscp;

    return RT_ERR_OK;
}

int32 rtrpc_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.org_dscp = dscp;
    qos_cfg.dscp = rmkDscp;
    SETSOCKOPT(RTDRV_QOS_DSCP2DSCP_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t * pDot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = int_pri;
    GETSOCKOPT(RTDRV_QOS_OUT_1P_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDot1p_pri = qos_cfg.dot1p_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.int_pri = int_pri;
    qos_cfg.dot1p_pri = dot1p_pri;
    SETSOCKOPT(RTDRV_QOS_OUT_1P_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiRemark_get(uint32 unit, uint32 dp, uint32 *pDei)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dp = dp;
    GETSOCKOPT(RTDRV_QOS_DEI_REMARK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDei = qos_cfg.dei;

    return RT_ERR_OK;
}

int32 rtrpc_qos_deiRemark_set(uint32 unit, uint32 dp, uint32 dei)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dp = dp;
    qos_cfg.dei = dei;
    SETSOCKOPT(RTDRV_QOS_DEI_REMARK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_portDeiRemarkTagSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_DEI_REMARK_TAG_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.deiSrc;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portDeiRemarkTagSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.deiSrc = type;
    SETSOCKOPT(RTDRV_QOS_PORT_DEI_REMARK_TAG_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_schedulingAlgorithm_get(uint32 unit, rtk_port_t port, rtk_qos_scheduling_type_t *pScheduling_type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_SCHEDULING_ALGORITHM_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pScheduling_type = qos_cfg.scheduling_type;

    return RT_ERR_OK;
}

int32 rtrpc_qos_schedulingAlgorithm_set(uint32 unit, rtk_port_t port, rtk_qos_scheduling_type_t scheduling_type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.scheduling_type = scheduling_type;
    SETSOCKOPT(RTDRV_QOS_SCHEDULING_ALGORITHM_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_SCHEDULING_QUEUE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pQweights, &qos_cfg.qweights, sizeof(rtk_qos_queue_weights_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    osal_memcpy(&qos_cfg.qweights, pQweights, sizeof(rtk_qos_queue_weights_t));
    SETSOCKOPT(RTDRV_QOS_SCHEDULING_QUEUE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_CONG_AVOID_ALGO_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pAlgo    = qos_cfg.congAvoid_algo;

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.congAvoid_algo  = algo;
    SETSOCKOPT(RTDRV_QOS_CONG_AVOID_ALGO_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portCongAvoidAlgo_get(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t *pAlgo)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_CONG_AVOID_ALGO_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pAlgo    = qos_cfg.congAvoid_algo;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portCongAvoidAlgo_set(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t algo)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.congAvoid_algo  = algo;
    SETSOCKOPT(RTDRV_QOS_PORT_CONG_AVOID_ALGO_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidSysThresh_get(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dp   = dp;
    GETSOCKOPT(RTDRV_QOS_CONG_AVOID_SYS_THRESH_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pCongAvoidThresh, &(qos_cfg.congAvoid_thresh), sizeof(rtk_qos_congAvoidThresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidSysThresh_set(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.dp      = dp;
    osal_memcpy(&(qos_cfg.congAvoid_thresh), pCongAvoidThresh, sizeof(rtk_qos_congAvoidThresh_t));
    SETSOCKOPT(RTDRV_QOS_CONG_AVOID_SYS_THRESH_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidSysDropProbability_get(
    uint32  unit,
    uint32  dp,
    uint32  *pProbability)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dp   = dp;
    GETSOCKOPT(RTDRV_QOS_CONG_AVOID_SYS_DROP_PROBABILITY_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pProbability = qos_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidSysDropProbability_set(
    uint32  unit,
    uint32  dp,
    uint32  probability)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.dp      = dp;
    qos_cfg.data    = probability;
    SETSOCKOPT(RTDRV_QOS_CONG_AVOID_SYS_DROP_PROBABILITY_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32 rtrpc_qos_congAvoidGlobalQueueThresh_get(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue = queue;
    qos_cfg.dp = dp;
    GETSOCKOPT(RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_THRESH_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pCongAvoidThresh, &(qos_cfg.congAvoid_thresh), sizeof(rtk_qos_congAvoidThresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidGlobalQueueThresh_set(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue = queue;
    qos_cfg.dp = dp;
    osal_memcpy(&(qos_cfg.congAvoid_thresh), pCongAvoidThresh, sizeof(rtk_qos_congAvoidThresh_t));
    SETSOCKOPT(RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_THRESH_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidGlobalQueueDropProbability_get(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      *pProbability)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue = queue;
    qos_cfg.dp = dp;
    GETSOCKOPT(RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_DROP_PROBABILITY_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pProbability = qos_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidGlobalQueueDropProbability_set(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      probability)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue = queue;
    qos_cfg.dp = dp;
    qos_cfg.data = probability;
    SETSOCKOPT(RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_DROP_PROBABILITY_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32 rtrpc_qos_congAvoidGlobalQueueConfig_get(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue = queue;
    qos_cfg.dp = dp;
    GETSOCKOPT(RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_CONFIG_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pCongAvoidThresh, &(qos_cfg.congAvoid_thresh), sizeof(rtk_qos_congAvoidThresh_t));

    return RT_ERR_OK;
}

int32 rtrpc_qos_congAvoidGlobalQueueConfig_set(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.queue = queue;
    qos_cfg.dp = dp;
    osal_memcpy(&(qos_cfg.congAvoid_thresh), pCongAvoidThresh, sizeof(rtk_qos_congAvoidThresh_t));
    SETSOCKOPT(RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_CONFIG_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port    = port;
    qos_cfg.srClass = srClass;
    GETSOCKOPT(RTDRV_QOS_AVB_SR_CLASS_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port    = port;
    qos_cfg.srClass = srClass;
    qos_cfg.enable  = enable;
    SETSOCKOPT(RTDRV_QOS_AVB_SR_CLASS_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    GETSOCKOPT(RTDRV_QOS_AVB_SR_CONFIG_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pSrConf = qos_cfg.srConf;

    return RT_ERR_OK;
}

int32 rtrpc_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.srConf  = *pSrConf;
    SETSOCKOPT(RTDRV_QOS_AVB_SR_CONFIG_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_qos_pkt2CpuPriRemap_get(uint32 unit, rtk_pri_t intPri, rtk_pri_t *pNewPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.int_pri = intPri;
    GETSOCKOPT(RTDRV_QOS_PKT2CPU_PRI_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pNewPri = qos_cfg.new_pri;

    return RT_ERR_OK;
}

int32 rtrpc_qos_pkt2CpuPriRemap_set(uint32 unit, rtk_pri_t intPri, rtk_pri_t newPri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.int_pri = intPri;
    qos_cfg.new_pri = newPri;
    SETSOCKOPT(RTDRV_QOS_PKT2CPU_PRI_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_rspanPriRemap_get(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t *pInt_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.rspan_pri = rspan_pri;
    GETSOCKOPT(RTDRV_QOS_RSPAN_PRI_REMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pInt_pri = qos_cfg.int_pri;

    return RT_ERR_OK;
}

int32
rtrpc_qos_rspanPriRemap_set(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t int_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.rspan_pri = rspan_pri;
    qos_cfg.int_pri = int_pri;
    SETSOCKOPT(RTDRV_QOS_RSPAN_PRI_REMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portPri2IgrQMap_get(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PRI2IGR_QUEUE_MAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pPri2qid = qos_cfg.pri2qid;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portPri2IgrQMap_set(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port = port;
    qos_cfg.pri2qid = *pPri2qid;
    SETSOCKOPT(RTDRV_QOS_PRI2IGR_QUEUE_MAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_qos_portPri2IgrQMapEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PRI2IGR_QUEUE_MAP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portPri2IgrQMapEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port = port;
    qos_cfg.enable= enable;
    SETSOCKOPT(RTDRV_QOS_PRI2IGR_QUEUE_MAP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_qos_portIgrQueueWeight_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pQweight)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port = port;
    qos_cfg.queue = queue;
    GETSOCKOPT(RTDRV_QOS_PORT_IGR_QUEUE_WEIGHT_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pQweight = qos_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portIgrQueueWeight_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 qWeight)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit    = unit;
    qos_cfg.port = port;
    qos_cfg.queue = queue;
    qos_cfg.data = qWeight;
    SETSOCKOPT(RTDRV_QOS_PORT_IGR_QUEUE_WEIGHT_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_1P_DFLT_PRI_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.dflt_src_1p;

    return RT_ERR_OK;
}

int32
rtrpc_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dflt_src_1p = type;
    SETSOCKOPT(RTDRV_QOS_1P_DFLT_PRI_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portOuter1pRemarkSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t * pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_REMARK_SRC_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.rmksrc_outer1p;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portOuter1pRemarkSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.rmksrc_outer1p = type;
    SETSOCKOPT(RTDRV_QOS_PORT_OUTER_1P_REMARK_SRC_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t * pDot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_OUTER_1P_DFLT_PRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDot1p_pri = qos_cfg.out1p_dflt_pri;

    return RT_ERR_OK;
}

int32
rtrpc_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.out1p_dflt_pri = dot1p_pri;
    SETSOCKOPT(RTDRV_QOS_OUTER_1P_DFLT_PRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_OUTER_1P_DFLT_PRI_CFG_SRC_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDflt_sel = qos_cfg.out1p_dflt_cfg_dir;

    return RT_ERR_OK;
}

int32
rtrpc_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.out1p_dflt_cfg_dir = dflt_sel;
    SETSOCKOPT(RTDRV_QOS_OUTER_1P_DFLT_PRI_CFG_SRC_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_INVLD_DSCP_VAL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDscp = qos_cfg.dscp;

    return RT_ERR_OK;
}

int32
rtrpc_qos_invldDscpVal_set(uint32 unit, uint32 dscp)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dscp = dscp;
    SETSOCKOPT(RTDRV_QOS_INVLD_DSCP_VAL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_invldDscpMask_get(uint32 unit, uint32 *pDscpMask)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_INVLD_DSCP_MASK_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pDscpMask = qos_cfg.dscp;

    return RT_ERR_OK;
}

int32
rtrpc_qos_invldDscpMask_set(uint32 unit, uint32 dscpMask)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.dscp = dscpMask;
    SETSOCKOPT(RTDRV_QOS_INVLD_DSCP_MASK_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portInvldDscpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_INVLD_DSCP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portInvldDscpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_INVLD_DSCP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_invldDscpEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_INVLD_DSCP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_invldDscpEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_INVLD_DSCP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_qos_portPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_SYS_PORT_PRI_REMAP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portPriRemapEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_SYS_PORT_PRI_REMAP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_qos_sysPortPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_SYS_PORT_PRI_REMAP_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.portPriRemap_type;

    return RT_ERR_OK;
}

int32
rtrpc_qos_sysPortPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.portPriRemap_type = type;
    SETSOCKOPT(RTDRV_QOS_SYS_PORT_PRI_REMAP_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portPortPriRemapSel_get(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t *pType)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    GETSOCKOPT(RTDRV_QOS_PORT_PORT_PRI_REMAP_SEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pType = qos_cfg.portPriRemap_type;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portPortPriRemapSel_set(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t type)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.portPriRemap_type = type;
    SETSOCKOPT(RTDRV_QOS_PORT_PORT_PRI_REMAP_SEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portInnerPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_PORT_IPRI_REMAP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portInnerPriRemapEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_IPRI_REMAP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portOuterPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    GETSOCKOPT(RTDRV_QOS_PORT_OPRI_REMAP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_portOuterPriRemapEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_OPRI_REMAP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_priRemapEnable_get(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.priSrcType = src;
    GETSOCKOPT(RTDRV_QOS_PRI_REMAP_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_qos_priRemapEnable_set(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.priSrcType = src;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_PRI_REMAP_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_qos_portQueueStrictEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.queue = queue;
    GETSOCKOPT(RTDRV_QOS_PORT_QUEUE_STRICT_ENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    *pEnable = qos_cfg.enable;

    return RT_ERR_OK;
}


int32
rtrpc_qos_portQueueStrictEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    rtdrv_qosCfg_t qos_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&qos_cfg, 0, sizeof(rtdrv_qosCfg_t));
    qos_cfg.unit = unit;
    qos_cfg.port = port;
    qos_cfg.queue = queue;
    qos_cfg.enable = enable;
    SETSOCKOPT(RTDRV_QOS_PORT_QUEUE_STRICT_ENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_qosMapper_init(dal_mapper_t *pMapper)
{
    pMapper->qos_priSelGroup_get = rtrpc_qos_priSelGroup_get;
    pMapper->qos_priSelGroup_set = rtrpc_qos_priSelGroup_set;
    pMapper->qos_portPriSelGroup_get = rtrpc_qos_portPriSelGroup_get;
    pMapper->qos_portPriSelGroup_set = rtrpc_qos_portPriSelGroup_set;
    pMapper->qos_sysPortPriRemapSel_get = rtrpc_qos_sysPortPriRemapSel_get;
    pMapper->qos_sysPortPriRemapSel_set = rtrpc_qos_sysPortPriRemapSel_set;
    pMapper->qos_portPortPriRemapSel_get = rtrpc_qos_portPortPriRemapSel_get;
    pMapper->qos_portPortPriRemapSel_set = rtrpc_qos_portPortPriRemapSel_set;
    pMapper->qos_dpSrcSel_get = rtrpc_qos_dpSrcSel_get;
    pMapper->qos_dpSrcSel_set = rtrpc_qos_dpSrcSel_set;
    pMapper->qos_portDEISrcSel_get = rtrpc_qos_portDEISrcSel_get;
    pMapper->qos_portDEISrcSel_set = rtrpc_qos_portDEISrcSel_set;
    pMapper->qos_portDpSel_get = rtrpc_qos_portDpSel_get;
    pMapper->qos_portDpSel_set = rtrpc_qos_portDpSel_set;
    pMapper->qos_deiDpRemap_get = rtrpc_qos_deiDpRemap_get;
    pMapper->qos_deiDpRemap_set = rtrpc_qos_deiDpRemap_set;
    pMapper->qos_dpRemap_get = rtrpc_qos_dpRemap_get;
    pMapper->qos_dpRemap_set = rtrpc_qos_dpRemap_set;
    pMapper->qos_priRemap_get = rtrpc_qos_priRemap_get;
    pMapper->qos_priRemap_set = rtrpc_qos_priRemap_set;
    pMapper->qos_queueNum_get = rtrpc_qos_queueNum_get;
    pMapper->qos_queueNum_set = rtrpc_qos_queueNum_set;
    pMapper->qos_priMap_get = rtrpc_qos_priMap_get;
    pMapper->qos_priMap_set = rtrpc_qos_priMap_set;
    pMapper->qos_pri2QidMap_get = rtrpc_qos_pri2QidMap_get;
    pMapper->qos_pri2QidMap_set = rtrpc_qos_pri2QidMap_set;
    pMapper->qos_cpuQid2QidMap_get = rtrpc_qos_cpuQid2QidMap_get;
    pMapper->qos_cpuQid2QidMap_set = rtrpc_qos_cpuQid2QidMap_set;
    pMapper->qos_cpuQid2StackQidMap_get = rtrpc_qos_cpuQid2StackQidMap_get;
    pMapper->qos_cpuQid2StackQidMap_set = rtrpc_qos_cpuQid2StackQidMap_set;
    pMapper->qos_port1pRemarkEnable_get = rtrpc_qos_port1pRemarkEnable_get;
    pMapper->qos_port1pRemarkEnable_set = rtrpc_qos_port1pRemarkEnable_set;
    pMapper->qos_1pRemarking_get = rtrpc_qos_1pRemarking_get;
    pMapper->qos_1pRemarking_set = rtrpc_qos_1pRemarking_set;
    pMapper->qos_1pRemarkSrcSel_get = rtrpc_qos_1pRemarkSrcSel_get;
    pMapper->qos_1pRemarkSrcSel_set = rtrpc_qos_1pRemarkSrcSel_set;
    pMapper->qos_port1pDfltPri_get = rtrpc_qos_port1pDfltPri_get;
    pMapper->qos_port1pDfltPri_set = rtrpc_qos_port1pDfltPri_set;
    pMapper->qos_port1pDfltPriExt_get = rtrpc_qos_port1pDfltPriExt_get;
    pMapper->qos_port1pDfltPriExt_set = rtrpc_qos_port1pDfltPriExt_set;
    pMapper->qos_port1pDfltPriSrcSel_get = rtrpc_qos_port1pDfltPriSrcSel_get;
    pMapper->qos_port1pDfltPriSrcSel_set = rtrpc_qos_port1pDfltPriSrcSel_set;
    pMapper->qos_port1pDfltPriSrcSelExt_get = rtrpc_qos_port1pDfltPriSrcSelExt_get;
    pMapper->qos_port1pDfltPriSrcSelExt_set = rtrpc_qos_port1pDfltPriSrcSelExt_set;
    pMapper->qos_1pDfltPriCfgSrcSel_get = rtrpc_qos_1pDfltPriCfgSrcSel_get;
    pMapper->qos_1pDfltPriCfgSrcSel_set = rtrpc_qos_1pDfltPriCfgSrcSel_set;
    pMapper->qos_1pDfltPriSrcSel_get = rtrpc_qos_1pDfltPriSrcSel_get;
    pMapper->qos_1pDfltPriSrcSel_set = rtrpc_qos_1pDfltPriSrcSel_set;
    pMapper->qos_1pDfltPri_get = rtrpc_qos_1pDfltPri_get;
    pMapper->qos_1pDfltPri_set = rtrpc_qos_1pDfltPri_set;
    pMapper->qos_portOut1pRemarkEnable_get = rtrpc_qos_portOut1pRemarkEnable_get;
    pMapper->qos_portOut1pRemarkEnable_set = rtrpc_qos_portOut1pRemarkEnable_set;
    pMapper->qos_portOuter1pRemarkSrcSel_get = rtrpc_qos_portOuter1pRemarkSrcSel_get;
    pMapper->qos_portOuter1pRemarkSrcSel_set = rtrpc_qos_portOuter1pRemarkSrcSel_set;
    pMapper->qos_outer1pRemarking_get = rtrpc_qos_outer1pRemarking_get;
    pMapper->qos_outer1pRemarking_set = rtrpc_qos_outer1pRemarking_set;
    pMapper->qos_outer1pRemarkSrcSel_get = rtrpc_qos_outer1pRemarkSrcSel_get;
    pMapper->qos_outer1pRemarkSrcSel_set = rtrpc_qos_outer1pRemarkSrcSel_set;
    pMapper->qos_portOuter1pDfltPri_get = rtrpc_qos_portOuter1pDfltPri_get;
    pMapper->qos_portOuter1pDfltPri_set = rtrpc_qos_portOuter1pDfltPri_set;
    pMapper->qos_portOuter1pDfltPriExt_get = rtrpc_qos_portOuter1pDfltPriExt_get;
    pMapper->qos_portOuter1pDfltPriExt_set = rtrpc_qos_portOuter1pDfltPriExt_set;
    pMapper->qos_portOuter1pDfltPriSrcSel_get = rtrpc_qos_portOuter1pDfltPriSrcSel_get;
    pMapper->qos_portOuter1pDfltPriSrcSel_set = rtrpc_qos_portOuter1pDfltPriSrcSel_set;
    pMapper->qos_portOuter1pDfltPriSrcSelExt_get = rtrpc_qos_portOuter1pDfltPriSrcSelExt_get;
    pMapper->qos_portOuter1pDfltPriSrcSelExt_set = rtrpc_qos_portOuter1pDfltPriSrcSelExt_set;
    pMapper->qos_outer1pDfltPriCfgSrcSel_get = rtrpc_qos_outer1pDfltPriCfgSrcSel_get;
    pMapper->qos_outer1pDfltPriCfgSrcSel_set = rtrpc_qos_outer1pDfltPriCfgSrcSel_set;
    pMapper->qos_outer1pDfltPri_get = rtrpc_qos_outer1pDfltPri_get;
    pMapper->qos_outer1pDfltPri_set = rtrpc_qos_outer1pDfltPri_set;
    pMapper->qos_portDscpRemarkEnable_get = rtrpc_qos_portDscpRemarkEnable_get;
    pMapper->qos_portDscpRemarkEnable_set = rtrpc_qos_portDscpRemarkEnable_set;
    pMapper->qos_dscpRemarking_get = rtrpc_qos_dscpRemarking_get;
    pMapper->qos_dscpRemarking_set = rtrpc_qos_dscpRemarking_set;
    pMapper->qos_dscpRemarkSrcSel_get = rtrpc_qos_dscpRemarkSrcSel_get;
    pMapper->qos_dscpRemarkSrcSel_set = rtrpc_qos_dscpRemarkSrcSel_set;
    pMapper->qos_portDeiRemarkEnable_get = rtrpc_qos_portDeiRemarkEnable_get;
    pMapper->qos_portDeiRemarkEnable_set = rtrpc_qos_portDeiRemarkEnable_set;
    pMapper->qos_portDeiRemarkTagSel_get = rtrpc_qos_portDeiRemarkTagSel_get;
    pMapper->qos_portDeiRemarkTagSel_set = rtrpc_qos_portDeiRemarkTagSel_set;
    pMapper->qos_deiRemarking_get = rtrpc_qos_deiRemarking_get;
    pMapper->qos_deiRemarking_set = rtrpc_qos_deiRemarking_set;
    pMapper->qos_deiRemarkSrcSel_get = rtrpc_qos_deiRemarkSrcSel_get;
    pMapper->qos_deiRemarkSrcSel_set = rtrpc_qos_deiRemarkSrcSel_set;
    pMapper->qos_schedulingAlgorithm_get = rtrpc_qos_schedulingAlgorithm_get;
    pMapper->qos_schedulingAlgorithm_set = rtrpc_qos_schedulingAlgorithm_set;
    pMapper->qos_schedulingQueue_get = rtrpc_qos_schedulingQueue_get;
    pMapper->qos_schedulingQueue_set = rtrpc_qos_schedulingQueue_set;
    pMapper->qos_congAvoidAlgo_get = rtrpc_qos_congAvoidAlgo_get;
    pMapper->qos_congAvoidAlgo_set = rtrpc_qos_congAvoidAlgo_set;
    pMapper->qos_portCongAvoidAlgo_get = rtrpc_qos_portCongAvoidAlgo_get;
    pMapper->qos_portCongAvoidAlgo_set = rtrpc_qos_portCongAvoidAlgo_set;
    pMapper->qos_congAvoidSysThresh_get = rtrpc_qos_congAvoidSysThresh_get;
    pMapper->qos_congAvoidSysThresh_set = rtrpc_qos_congAvoidSysThresh_set;
    pMapper->qos_congAvoidSysDropProbability_get = rtrpc_qos_congAvoidSysDropProbability_get;
    pMapper->qos_congAvoidSysDropProbability_set = rtrpc_qos_congAvoidSysDropProbability_set;
    pMapper->qos_congAvoidGlobalQueueConfig_get = rtrpc_qos_congAvoidGlobalQueueConfig_get;
    pMapper->qos_congAvoidGlobalQueueConfig_set = rtrpc_qos_congAvoidGlobalQueueConfig_set;
    pMapper->qos_portAvbStreamReservationClassEnable_get = rtrpc_qos_portAvbStreamReservationClassEnable_get;
    pMapper->qos_portAvbStreamReservationClassEnable_set = rtrpc_qos_portAvbStreamReservationClassEnable_set;
    pMapper->qos_avbStreamReservationConfig_get = rtrpc_qos_avbStreamReservationConfig_get;
    pMapper->qos_avbStreamReservationConfig_set = rtrpc_qos_avbStreamReservationConfig_set;
    pMapper->qos_pkt2CpuPriRemap_get = rtrpc_qos_pkt2CpuPriRemap_get;
    pMapper->qos_pkt2CpuPriRemap_set = rtrpc_qos_pkt2CpuPriRemap_set;
    pMapper->qos_rspanPriRemap_get = rtrpc_qos_rspanPriRemap_get;
    pMapper->qos_rspanPriRemap_set = rtrpc_qos_rspanPriRemap_set;
    pMapper->qos_portPri2IgrQMapEnable_get = rtrpc_qos_portPri2IgrQMapEnable_get;
    pMapper->qos_portPri2IgrQMapEnable_set = rtrpc_qos_portPri2IgrQMapEnable_set;
    pMapper->qos_portPri2IgrQMap_get = rtrpc_qos_portPri2IgrQMap_get;
    pMapper->qos_portPri2IgrQMap_set = rtrpc_qos_portPri2IgrQMap_set;
    pMapper->qos_portIgrQueueWeight_get = rtrpc_qos_portIgrQueueWeight_get;
    pMapper->qos_portIgrQueueWeight_set = rtrpc_qos_portIgrQueueWeight_set;
    pMapper->qos_invldDscpVal_get = rtrpc_qos_invldDscpVal_get;
    pMapper->qos_invldDscpVal_set = rtrpc_qos_invldDscpVal_set;
    pMapper->qos_invldDscpMask_get = rtrpc_qos_invldDscpMask_get;
    pMapper->qos_invldDscpMask_set = rtrpc_qos_invldDscpMask_set;
    pMapper->qos_portInvldDscpEnable_get = rtrpc_qos_portInvldDscpEnable_get;
    pMapper->qos_portInvldDscpEnable_set = rtrpc_qos_portInvldDscpEnable_set;
    pMapper->qos_invldDscpEnable_get = rtrpc_qos_invldDscpEnable_get;
    pMapper->qos_invldDscpEnable_set = rtrpc_qos_invldDscpEnable_set;
    pMapper->qos_portInnerPriRemapEnable_get = rtrpc_qos_portInnerPriRemapEnable_get;
    pMapper->qos_portInnerPriRemapEnable_set = rtrpc_qos_portInnerPriRemapEnable_set;
    pMapper->qos_portOuterPriRemapEnable_get = rtrpc_qos_portOuterPriRemapEnable_get;
    pMapper->qos_portOuterPriRemapEnable_set = rtrpc_qos_portOuterPriRemapEnable_set;
    pMapper->qos_priRemapEnable_get = rtrpc_qos_priRemapEnable_get;
    pMapper->qos_priRemapEnable_set = rtrpc_qos_priRemapEnable_set;
    pMapper->qos_portQueueStrictEnable_get = rtrpc_qos_portQueueStrictEnable_get;
    pMapper->qos_portQueueStrictEnable_set = rtrpc_qos_portQueueStrictEnable_set;
    return RT_ERR_OK;
}
