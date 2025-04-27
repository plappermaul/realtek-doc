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
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) mirror
 *
 */

#include <dal/rtrpc/rtrpc_mirror.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    GETSOCKOPT(RTDRV_MIRROR_GROUP_INIT, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    osal_memcpy(pMirrorEntry, &(mirror_cfg.mirrorEntry), sizeof(rtk_mirror_entry_t));

    return RT_ERR_OK;
}

int32 rtrpc_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit         = unit;
    mirror_cfg.mirror_id    = mirror_id;
    GETSOCKOPT(RTDRV_MIRROR_GROUP_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    osal_memcpy(pMirrorEntry, &(mirror_cfg.mirrorEntry), sizeof(rtk_mirror_entry_t));

    return RT_ERR_OK;
}

int32 rtrpc_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.mirror_id = mirror_id;
    osal_memcpy(&mirror_cfg.mirrorEntry, pMirrorEntry, sizeof(rtk_mirror_entry_t));
    SETSOCKOPT(RTDRV_MIRROR_GROUP_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_mirror_rspanIgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t *pIgrMode)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit    = unit;
    mirror_cfg.mirror_id    = mirror_id;
    GETSOCKOPT(RTDRV_MIRROR_RSPAN_IGR_MODE_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    *pIgrMode = mirror_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_mirror_rspanIgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t igrMode)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.mirror_id = mirror_id;
    mirror_cfg.data = igrMode;
    SETSOCKOPT(RTDRV_MIRROR_RSPAN_IGR_MODE_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit    = unit;
    mirror_cfg.mirror_id    = mirror_id;
    GETSOCKOPT(RTDRV_MIRROR_RSPAN_EGR_MODE_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    *pEgrMode = mirror_cfg.data;

    return RT_ERR_OK;
}


int32 rtrpc_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.mirror_id = mirror_id;
    mirror_cfg.data = egrMode;
    SETSOCKOPT(RTDRV_MIRROR_RSPAN_EGR_MODE_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit    = unit;
    mirror_cfg.mirror_id = mirror_id;
    GETSOCKOPT(RTDRV_MIRROR_RSPAN_TAG_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    osal_memcpy(pTag, &(mirror_cfg.rspan_tag), sizeof(rtk_mirror_rspanTag_t));

    return RT_ERR_OK;
}

int32 rtrpc_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.mirror_id = mirror_id;
    osal_memcpy(&(mirror_cfg.rspan_tag), pTag, sizeof(rtk_mirror_rspanTag_t));
    SETSOCKOPT(RTDRV_MIRROR_RSPAN_TAG_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit    = unit;
    mirror_cfg.mirror_id    = mirror_id;
    GETSOCKOPT(RTDRV_MIRROR_SFLOW_MIRROR_SAMPLE_RATE_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    *pRate = mirror_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.mirror_id    = mirror_id;
    mirror_cfg.data = rate;
    SETSOCKOPT(RTDRV_MIRROR_SFLOW_MIRROR_SAMPLE_RATE_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_mirror_egrQueue_get(uint32 unit, rtk_enable_t *pEnable, rtk_qid_t *pQid)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit    = unit;
    GETSOCKOPT(RTDRV_MIRROR_EGRQUEUE_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    *pEnable = mirror_cfg.enable;
    *pQid = mirror_cfg.qid;

    return RT_ERR_OK;
}

int32
rtrpc_mirror_egrQueue_set(uint32 unit, rtk_enable_t enable, rtk_qid_t qid)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.enable = enable;
    mirror_cfg.qid = qid;
    SETSOCKOPT(RTDRV_MIRROR_EGRQUEUE_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_mirror_sflowPortIgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit     = unit;
    mirror_cfg.port     = port;
    GETSOCKOPT(RTDRV_MIRROR_SFLOW_PORT_IGR_SAMPLE_RATE_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    *pRate = mirror_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_mirror_sflowPortIgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.port = port;
    mirror_cfg.data = rate;
    SETSOCKOPT(RTDRV_MIRROR_SFLOW_PORT_IGR_SAMPLE_RATE_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_mirror_sflowPortEgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit     = unit;
    mirror_cfg.port     = port;
    GETSOCKOPT(RTDRV_MIRROR_SFLOW_PORT_EGR_SAMPLE_RATE_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    *pRate = mirror_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_mirror_sflowPortEgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    rtdrv_mirrorCfg_t mirror_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mirror_cfg, 0, sizeof(mirror_cfg));

    mirror_cfg.unit = unit;
    mirror_cfg.port = port;
    mirror_cfg.data = rate;
    SETSOCKOPT(RTDRV_MIRROR_SFLOW_PORT_EGR_SAMPLE_RATE_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_mirror_sflowSampleCtrl_get(uint32 unit, rtk_sflowSampleCtrl_t *pCtrl)
{
    rtdrv_mirrorCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_MIRROR_SFLOW_SAMPLE_CTRL_GET, &cfg, rtdrv_mirrorCfg_t, 1);
    *pCtrl = cfg.sample_ctrl;

    return RT_ERR_OK;
}    /* end of rtk_mirror_sflowSampleCtrl_get */

int32
rtrpc_mirror_sflowSampleCtrl_set(uint32 unit, rtk_sflowSampleCtrl_t ctrl)
{
    rtdrv_mirrorCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.unit        = unit;
    cfg.sample_ctrl = ctrl;
    SETSOCKOPT(RTDRV_MIRROR_SFLOW_SAMPLE_CTRL_SET, &cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_mirror_sflowSampleCtrl_set */

int32
rtrpc_mirror_sflowSampleTarget_get(uint32 unit, rtk_sflow_sampleTarget_t *pTarget)
{
    rtdrv_mirrorCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_MIRROR_SFLOWSAMPLETARGET_GET, &cfg, rtdrv_mirrorCfg_t, 1);
    osal_memcpy(pTarget, &cfg.target, sizeof(rtk_sflow_sampleTarget_t));

    return RT_ERR_OK;
}   /* end of rtk_mirror_sflowSampleTarget_get */

int32
rtrpc_mirror_sflowSampleTarget_set(uint32 unit, rtk_sflow_sampleTarget_t target)
{
    rtdrv_mirrorCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.target, &target, sizeof(rtk_sflow_sampleTarget_t));
    SETSOCKOPT(RTDRV_MIRROR_SFLOWSAMPLETARGET_SET, &cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mirror_sflowSampleTarget_set */
int32
rtrpc_mirrorMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mirror_group_init = rtrpc_mirror_group_init;
    pMapper->mirror_group_get = rtrpc_mirror_group_get;
    pMapper->mirror_group_set = rtrpc_mirror_group_set;
    pMapper->mirror_rspanIgrMode_get = rtrpc_mirror_rspanIgrMode_get;
    pMapper->mirror_rspanIgrMode_set = rtrpc_mirror_rspanIgrMode_set;
    pMapper->mirror_rspanEgrMode_get = rtrpc_mirror_rspanEgrMode_get;
    pMapper->mirror_rspanEgrMode_set = rtrpc_mirror_rspanEgrMode_set;
    pMapper->mirror_rspanTag_get = rtrpc_mirror_rspanTag_get;
    pMapper->mirror_rspanTag_set = rtrpc_mirror_rspanTag_set;
    pMapper->mirror_sflowMirrorSampleRate_get = rtrpc_mirror_sflowMirrorSampleRate_get;
    pMapper->mirror_sflowMirrorSampleRate_set = rtrpc_mirror_sflowMirrorSampleRate_set;
    pMapper->mirror_egrQueue_get = rtrpc_mirror_egrQueue_get;
    pMapper->mirror_egrQueue_set = rtrpc_mirror_egrQueue_set;
    pMapper->mirror_sflowPortIgrSampleRate_get = rtrpc_mirror_sflowPortIgrSampleRate_get;
    pMapper->mirror_sflowPortIgrSampleRate_set = rtrpc_mirror_sflowPortIgrSampleRate_set;
    pMapper->mirror_sflowPortEgrSampleRate_get = rtrpc_mirror_sflowPortEgrSampleRate_get;
    pMapper->mirror_sflowPortEgrSampleRate_set = rtrpc_mirror_sflowPortEgrSampleRate_set;
    pMapper->mirror_sflowSampleCtrl_get = rtrpc_mirror_sflowSampleCtrl_get;
    pMapper->mirror_sflowSampleCtrl_set = rtrpc_mirror_sflowSampleCtrl_set;
    pMapper->mirror_sflowSampleTarget_get = rtrpc_mirror_sflowSampleTarget_get;
    pMapper->mirror_sflowSampleTarget_set = rtrpc_mirror_sflowSampleTarget_set;
    return RT_ERR_OK;
}
