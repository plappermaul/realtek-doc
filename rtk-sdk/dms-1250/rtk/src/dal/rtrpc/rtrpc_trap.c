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
 *           1) trap
 *
 */

#include <rtk/trap.h>
#include <dal/rtrpc/rtrpc_trap.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    trap_cfg.unit = unit;
    osal_memcpy(&trap_cfg.rma_frame, pRma_frame, sizeof(rtk_mac_t));
    GETSOCKOPT(RTDRV_TRAP_RMAACTION_GET, &trap_cfg, rtdrv_trapCfg_t, 1);
    *pRma_action = trap_cfg.rma_action;

    return RT_ERR_OK;
}


int32 rtrpc_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    trap_cfg.unit = unit;
    osal_memcpy(&trap_cfg.rma_frame, pRma_frame, sizeof(rtk_mac_t));
    trap_cfg.rma_action = rma_action;
    SETSOCKOPT(RTDRV_TRAP_RMAACTION_SET, &trap_cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}


int32 rtrpc_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.bypassStp_frame = frameType;
    GETSOCKOPT(RTDRV_TRAP_BYPASS_STP_GET, &trap_cfg, rtdrv_trapCfg_t, 1);
    *pEnable = trap_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.bypassStp_frame = frameType;
    trap_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_BYPASS_STP_SET, &trap_cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.bypassVlan_frame = frameType;
    GETSOCKOPT(RTDRV_TRAP_BYPASS_VLAN_GET, &trap_cfg, rtdrv_trapCfg_t, 1);
    *pEnable = trap_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.bypassVlan_frame = frameType;
    trap_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_BYPASS_VLAN_SET, &trap_cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma)
{
    rtdrv_trapL2userRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapL2userRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.rma_index = userDefine_idx;
    GETSOCKOPT(RTDRV_TRAP_USERDEFINERMA_GET, &trap_cfg, rtdrv_trapL2userRmaCfg_t, 1);
    osal_memcpy(pUserDefinedRma, &trap_cfg.rma_frame, sizeof(rtk_trap_userDefinedRma_t));

    return RT_ERR_OK;
} /* end of rtk_trap_userDefineRma_get */


int32 rtrpc_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma)
{
    rtdrv_trapL2userRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapL2userRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.rma_index = userDefine_idx;
    osal_memcpy(&trap_cfg.rma_frame, pUserDefinedRma, sizeof(rtk_trap_userDefinedRma_t));
    SETSOCKOPT(RTDRV_TRAP_USERDEFINERMA_SET, &trap_cfg, rtdrv_trapL2userRmaCfg_t, 1);

    return RT_ERR_OK;
}  /* end of rtk_trap_userDefineRma_set */


int32 rtrpc_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    rtdrv_trapL2userRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapL2userRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.rma_index = userDefine_idx;
    GETSOCKOPT(RTDRV_TRAP_USERDEFINERMAENABLE_GET, &trap_cfg, rtdrv_trapL2userRmaCfg_t, 1);
    *pEnable = trap_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    rtdrv_trapL2userRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapL2userRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.rma_index = userDefine_idx;
    trap_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_USERDEFINERMAENABLE_SET, &trap_cfg, rtdrv_trapL2userRmaCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction)
{
    rtdrv_trapL2userRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapL2userRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.rma_index = userDefine_idx;
    GETSOCKOPT(RTDRV_TRAP_USERDEFINERMAACTION_GET, &trap_cfg, rtdrv_trapL2userRmaCfg_t, 1);
    *pAction = trap_cfg.rma_action;

    return RT_ERR_OK;
}


int32 rtrpc_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action)
{
    rtdrv_trapL2userRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapL2userRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.rma_index = userDefine_idx;
    trap_cfg.rma_action = action;
    SETSOCKOPT(RTDRV_TRAP_USERDEFINERMAACTION_SET, &trap_cfg, rtdrv_trapL2userRmaCfg_t, 1);

    return RT_ERR_OK;
}


int32 rtrpc_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.frameType = frameType;
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMEACTION_GET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);
    *pAction = trap_cfg.rma_action;

    return RT_ERR_OK;
}


int32 rtrpc_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.frameType = frameType;
    trap_cfg.rma_action = action;
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMEACTION_SET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);

    return RT_ERR_OK;
}


int32 rtrpc_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.frameType = frameType;
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMEPRI_GET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);
    *pPriority = trap_cfg.priority;

    return RT_ERR_OK;
} /* end of rtk_trap_mgmtFramePri_get */

int32 rtrpc_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.frameType = frameType;
    trap_cfg.priority = priority;
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMEPRI_SET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_mgmtFramePri_set */

int32 rtrpc_trap_mgmtFrameQueue_get(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t *pQid)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.qType = qType;
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMEQID_GET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);
    *pQid = trap_cfg.qid;

    return RT_ERR_OK;
} /* end of rtk_trap_mgmtFrameQueue_get */

int32 rtrpc_trap_mgmtFrameQueue_set(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t qid)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.qType = qType;
    trap_cfg.qid = qid;
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMEQID_SET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_mgmtFrameQueue_set */

int32 rtrpc_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.port = port;
    trap_cfg.frameType = frameType;
    GETSOCKOPT(RTDRV_TRAP_PORTMGMTFRAMEACTION_GET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);
    *pAction = trap_cfg.rma_action;

    return RT_ERR_OK;
} /* end of rtk_trap_portMgmtFrameAction_get */


int32 rtrpc_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    rtdrv_trapMgmRmaCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.port = port;
    trap_cfg.frameType = frameType;
    trap_cfg.rma_action = action;
    SETSOCKOPT(RTDRV_TRAP_PORTMGMTFRAMEACTION_SET, &trap_cfg, rtdrv_trapMgmRmaCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_portMgmtFrameAction_set */


int32 rtrpc_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_trapOtherCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapOtherCfg_t));
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_PKTWITHCFIACTION_GET, &trap_cfg, rtdrv_trapOtherCfg_t, 1);
    *pAction = trap_cfg.action;

    return RT_ERR_OK;
} /* end of rtk_trap_pktWithCFIAction_get */


int32 rtrpc_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action)
{
    rtdrv_trapOtherCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapOtherCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.action = action;
    SETSOCKOPT(RTDRV_TRAP_PKTWITHCFIACTION_SET, &trap_cfg, rtdrv_trapOtherCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_pktWithCFIAction_set */


int32 rtrpc_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_trapOtherCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapOtherCfg_t));
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_PKTWITHOUTERCFIACTION_GET, &trap_cfg, rtdrv_trapOtherCfg_t, 1);
    *pAction = trap_cfg.action;

    return RT_ERR_OK;
} /* end of rtk_trap_pktWithOuterCFIAction_get */


int32 rtrpc_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action)
{
    rtdrv_trapOtherCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapOtherCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.action = action;
    SETSOCKOPT(RTDRV_TRAP_PKTWITHOUTERCFIACTION_SET, &trap_cfg, rtdrv_trapOtherCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_pktWithOuterCFIAction_set */

int32 rtrpc_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    rtdrv_trapOtherCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapOtherCfg_t));
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_PKTWITHCFIPRI_GET, &trap_cfg, rtdrv_trapOtherCfg_t, 1);
    *pPriority = trap_cfg.priority;

    return RT_ERR_OK;
} /* end of rtk_trap_pktWithCFIPri_get */


int32 rtrpc_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority)
{
    rtdrv_trapOtherCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapOtherCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.priority = priority;
    SETSOCKOPT(RTDRV_TRAP_PKTWITHCFIPRI_SET, &trap_cfg, rtdrv_trapOtherCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_pktWithCFIPri_set */

int32 rtrpc_trap_cfmFrameTrapPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    rtdrv_trapCfmCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_CFMFRAMETRAPPRI_GET, &trap_cfg, rtdrv_trapCfmCfg_t, 1);
    *pPriority = trap_cfg.priority;

    return RT_ERR_OK;
} /* end of rtk_trap_cfmFrameTrapPri_get */


int32 rtrpc_trap_cfmFrameTrapPri_set(uint32 unit, rtk_pri_t priority)
{
    rtdrv_trapCfmCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.priority = priority;
    SETSOCKOPT(RTDRV_TRAP_CFMFRAMETRAPPRI_SET, &trap_cfg, rtdrv_trapCfmCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_cfmFrameTrapPri_set */

int32 rtrpc_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_trapCfmCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_OAMPDUACTION_GET, &trap_cfg, rtdrv_trapCfmCfg_t, 1);
    *pAction = trap_cfg.action;

    return RT_ERR_OK;
} /* end of rtk_trap_oamPDUAction_get */

int32 rtrpc_trap_oamPDUAction_set(uint32 unit, rtk_action_t action)
{
    rtdrv_trapCfmCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.action = action;
    SETSOCKOPT(RTDRV_TRAP_OAMPDUACTION_SET, &trap_cfg, rtdrv_trapCfmCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_oamPDUAction_set */

int32 rtrpc_trap_oamPDUPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    rtdrv_trapCfmCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_OAMPDUPRI_GET, &trap_cfg, rtdrv_trapCfmCfg_t, 1);
    *pPriority = trap_cfg.priority;

    return RT_ERR_OK;
} /* end of rtk_trap_oamPDUPri_get */

int32 rtrpc_trap_oamPDUPri_set(uint32 unit, rtk_pri_t priority)
{
    rtdrv_trapCfmCfg_t  trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    trap_cfg.unit = unit;
    trap_cfg.priority = priority;
    SETSOCKOPT(RTDRV_TRAP_OAMPDUPRI_SET, &trap_cfg, rtdrv_trapCfmCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_oamPDUPri_set */
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
int32
rtrpc_trap_cfmUnknownFrameAct_get(uint32 unit, rtk_action_t *action)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_CFMUNKNOWNFRAMEACT_GET, &config, rtdrv_oamCfmMiscCfg_t, 1);
    *action = config.action;

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmUnknownFrameAct_get */

int32
rtrpc_trap_cfmUnknownFrameAct_set(uint32 unit, rtk_action_t action)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    config.action = action;
    SETSOCKOPT(RTDRV_TRAP_CFMUNKNOWNFRAMEACT_SET, &config, rtdrv_oamCfmMiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmUnknownFrameAct_set */

int32
rtrpc_trap_cfmLoopbackLinkTraceAct_get(uint32 unit, uint32 level,
                            rtk_action_t *action)
{
    rtdrv_trapCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfmCfg_t));

    config.unit     = unit;
    config.md_level = level;
    GETSOCKOPT(RTDRV_TRAP_CFMLOOPBACKACT_GET, &config, rtdrv_trapCfmCfg_t, 1);
    *action = config.action;

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmLoopbackLinkTraceAct_get */

int32
rtrpc_trap_cfmLoopbackLinkTraceAct_set(uint32 unit, uint32 level,
                            rtk_action_t action)
{
    rtdrv_trapCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfmCfg_t));

    config.unit     = unit;
    config.md_level = level;
    config.action   = action;
    SETSOCKOPT(RTDRV_TRAP_CFMLOOPBACKACT_SET, &config, rtdrv_trapCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmLoopbackLinkTraceAct_set */

int32
rtrpc_trap_cfmCcmAct_get(uint32 unit, uint32 level,
                       rtk_trap_oam_action_t *action)
{
    rtdrv_trapOamCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOamCfg_t));

    config.unit     = unit;
    config.md_level = level;
    GETSOCKOPT(RTDRV_TRAP_CFMCCMACT_GET, &config, rtdrv_trapOamCfg_t, 1);
    *action = config.action;

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmCcmAct_get */

int32
rtrpc_trap_cfmCcmAct_set(uint32 unit, uint32 level,
                       rtk_trap_oam_action_t action)
{
    rtdrv_trapOamCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOamCfg_t));

    config.unit     = unit;
    config.md_level = level;
    config.action   = action;
    SETSOCKOPT(RTDRV_TRAP_CFMCCMACT_SET, &config, rtdrv_trapOamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmCcmAct_set */

int32
rtrpc_trap_cfmEthDmAct_get(uint32 unit, uint32 level,
                         rtk_action_t *action)
{
    rtdrv_trapCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfmCfg_t));

    config.unit     = unit;
    config.md_level = level;
    GETSOCKOPT(RTDRV_TRAP_CFMETHDMACT_GET, &config, rtdrv_trapCfmCfg_t, 1);
    *action = config.action;

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmEthDmAct_get */

int32
rtrpc_trap_cfmEthDmAct_set(uint32 unit, uint32 level,
                         rtk_action_t action)
{
    rtdrv_trapCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfmCfg_t));

    config.unit     = unit;
    config.md_level = level;
    config.action   = action;
    SETSOCKOPT(RTDRV_TRAP_CFMETHDMACT_SET, &config, rtdrv_trapOamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmEthDmAct_set */
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
int32 rtrpc_trap_portOamLoopbackParAction_get(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t *pAction)
{
    rtdrv_trapOamCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOamCfg_t));

    config.unit = unit;
    config.port = port;
    GETSOCKOPT(RTDRV_TRAP_PORTOAMLOOPBACKPARACTION_GET, &config, rtdrv_trapOamCfg_t, 1);

    *pAction = config.action;

    return RT_ERR_OK;
}   /* end of rtk_trap_portOamLoopbackParAction_get */

int32 rtrpc_trap_portOamLoopbackParAction_set(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t action)
{
    rtdrv_trapOamCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOamCfg_t));

    config.unit = unit;
    config.port = port;
    config.action = action;
    SETSOCKOPT(RTDRV_TRAP_PORTOAMLOOPBACKPARACTION_SET, &config, rtdrv_trapOamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_portOamLoopbackParAction_set */

int32
rtrpc_trap_routeExceptionAction_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_action_t *pAction)
{
    rtdrv_trapRouteExceptionCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapRouteExceptionCfg_t));
    config.unit = unit;
    config.type = type;

    GETSOCKOPT(RTDRV_TRAP_ROUTEEXCEPTIONACTION_GET, &config,
            rtdrv_trapRouteExceptionCfg_t, 1);

    *pAction = config.action;

    return RT_ERR_OK;
}    /* end of rtk_trap_routeExceptionAction_get */

int32
rtrpc_trap_routeExceptionAction_set(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_action_t action)
{
    rtdrv_trapRouteExceptionCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapRouteExceptionCfg_t));
    config.unit = unit;
    config.type = type;
    config.action = action;

    SETSOCKOPT(RTDRV_TRAP_ROUTEEXCEPTIONACTION_SET, &config,
            rtdrv_trapRouteExceptionCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_routeExceptionAction_set */

int32
rtrpc_trap_routeExceptionPri_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_pri_t *pPriority)
{
    rtdrv_trapRouteExceptionCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapRouteExceptionCfg_t));
    config.unit = unit;
    config.type = type;

    GETSOCKOPT(RTDRV_TRAP_ROUTEEXCEPTIONPRI_GET, &config,
            rtdrv_trapRouteExceptionCfg_t, 1);

    *pPriority = config.priority;

    return RT_ERR_OK;
}    /* end of rtk_trap_routeExceptionPri_get */

int32
rtrpc_trap_routeExceptionPri_set(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_pri_t priority)
{
    rtdrv_trapRouteExceptionCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapRouteExceptionCfg_t));
    config.unit = unit;
    config.type = type;
    config.priority = priority;

    SETSOCKOPT(RTDRV_TRAP_ROUTEEXCEPTIONPRI_SET, &config,
            rtdrv_trapRouteExceptionCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_routeExceptionPri_set */

int32
rtrpc_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx,
        rtk_enable_t *pEnable)
{
    rtdrv_trapUserMgmRmaCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapUserMgmRmaCfg_t));
    config.unit = unit;
    config.mgmt_idx = userDefine_idx;

    GETSOCKOPT(RTDRV_TRAP_USERDEFINERMALEARNINGENABLE_GET, &config,
            rtdrv_trapUserMgmRmaCfg_t, 1);

    *pEnable = config.enable;

    return RT_ERR_OK;
}   /* end of rtk_trap_userDefineRmaLearningEnable_set */

int32
rtrpc_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx,
        rtk_enable_t enable)
{
    rtdrv_trapUserMgmRmaCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapUserMgmRmaCfg_t));
    config.unit = unit;
    config.mgmt_idx = userDefine_idx;
    config.enable = enable;

    SETSOCKOPT(RTDRV_TRAP_USERDEFINERMALEARNINGENABLE_SET, &config,
            rtdrv_trapUserMgmRmaCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_userDefineRmaLearningEnable_set */

int32
rtrpc_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame,
        rtk_enable_t *pEnable)
{
    rtdrv_trapCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfg_t));
    config.unit = unit;
    osal_memcpy(&config.rma_frame, pRma_frame, sizeof(rtk_mac_t));
    GETSOCKOPT(RTDRV_TRAP_RMALEARNINGENABLE_GET, &config,
        rtdrv_trapCfg_t, 1);

    *pEnable = config.enable;

    return RT_ERR_OK;
} /* end of rtk_trap_rmaLearningEnable_set */

int32
rtrpc_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame,
        rtk_enable_t enable)
{
    rtdrv_trapCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfg_t));
    config.unit = unit;
    osal_memcpy(&config.rma_frame, pRma_frame, sizeof(rtk_mac_t));
    config.enable = enable;

    SETSOCKOPT(RTDRV_TRAP_RMALEARNINGENABLE_SET, &config,
        rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_trap_rmaLearningEnable_set */

int32
rtrpc_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType,
        rtk_enable_t *pEnable)
{
    rtdrv_trapMgmRmaCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    config.unit = unit;
    config.frameType = frameType;
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMELEARNINGENABLE_GET, &config,
            rtdrv_trapMgmRmaCfg_t, 1);

    *pEnable = config.enable;

    return RT_ERR_OK;
} /* end of rtk_trap_mgmtFrameLearningEnable_get */

int32
rtrpc_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType,
        rtk_enable_t enable)
{
    rtdrv_trapMgmRmaCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapMgmRmaCfg_t));
    config.unit = unit;
    config.frameType = frameType;
    config.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMELEARNINGENABLE_SET, &config,
            rtdrv_trapMgmRmaCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_mgmtFrameLearningEnable_set */

int32
rtrpc_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_trapOtherCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOtherCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMEMGMTVLANENABLE_GET, &config,
            rtdrv_trapOtherCfg_t, 1);

    *pEnable = config.enable;

    return RT_ERR_OK;
}    /* end of rtk_trap_mgmtFrameMgmtVlanEnable_get */

int32
rtrpc_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_trapOtherCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOtherCfg_t));
    config.unit = unit;
    config.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMEMGMTVLANENABLE_SET, &config,
            rtdrv_trapOtherCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_mgmtFrameMgmtVlanEnable_set */

int32
rtrpc_trap_bpduFloodPortmask_get(uint32 unit, rtk_portmask_t * pBPDU_flood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_BPDUFLOODPORTMASK_GET, &config,rtdrv_floodPmskCfg_t, 1);

    osal_memcpy(pBPDU_flood_portmask, &config.pmsk, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}    /* end of rtk_trap_bpduFloodPortmask_get */

int32
rtrpc_trap_bpduFloodPortmask_set(uint32 unit, rtk_portmask_t * pBPDU_flood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    osal_memcpy(&config.pmsk, pBPDU_flood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRAP_BPDUFLOODPORTMASK_SET, &config,rtdrv_floodPmskCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_bpduFloodPortmask_set */

int32
rtrpc_trap_eapolFloodPortmask_get(uint32 unit, rtk_portmask_t * pEapol_flood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_EAPOLFLOODPORTMASK_GET, &config,rtdrv_floodPmskCfg_t, 1);

    osal_memcpy(pEapol_flood_portmask, &config.pmsk, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}    /* end of rtk_trap_bpduFloodPortmask_get */

int32
rtrpc_trap_eapolFloodPortmask_set(uint32 unit, rtk_portmask_t * pEapol_flood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    osal_memcpy(&config.pmsk, pEapol_flood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRAP_EAPOLFLOODPORTMASK_SET, &config,rtdrv_floodPmskCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_bpduFloodPortmask_set */

int32
rtrpc_trap_lldpFloodPortmask_get(uint32 unit, rtk_portmask_t * pFlood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_LLDPFLOODPORTMASK_GET, &config,rtdrv_floodPmskCfg_t, 1);

    osal_memcpy(pFlood_portmask, &config.pmsk, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32
rtrpc_trap_lldpFloodPortmask_set(uint32 unit, rtk_portmask_t * pFlood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    osal_memcpy(&config.pmsk, pFlood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRAP_LLDPFLOODPORTMASK_SET, &config,rtdrv_floodPmskCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trap_userDefineFloodPortmask_get(uint32 unit, rtk_portmask_t * pflood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_USERDEFINEFLOODPORTMASK_GET, &config,rtdrv_floodPmskCfg_t, 1);

    osal_memcpy(pflood_portmask, &config.pmsk, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32
rtrpc_trap_userDefineFloodPortmask_set(uint32 unit, rtk_portmask_t * pflood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    osal_memcpy(&config.pmsk, pflood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRAP_USERDEFINEFLOODPORTMASK_SET, &config,rtdrv_floodPmskCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trap_rmaFloodPortmask_get(uint32 unit, rtk_portmask_t * pRma_flood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_RMAFLOODPORTMASK_GET, &config,rtdrv_floodPmskCfg_t, 1);

    osal_memcpy(pRma_flood_portmask, &config.pmsk, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}    /* end of rtk_trap_rmaFloodPortmask_get */

int32
rtrpc_trap_rmaFloodPortmask_set(uint32 unit, rtk_portmask_t * pRma_flood_portmask)
{
    rtdrv_floodPmskCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_floodPmskCfg_t));
    config.unit = unit;
    osal_memcpy(&config.pmsk, pRma_flood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRAP_RMAFLOODPORTMASK_SET, &config,rtdrv_floodPmskCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_rmaFloodPortmask_set */

int32
rtrpc_trap_rmaCancelMirror_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_trapCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_RMACANCELMIRROR_GET, &config, rtdrv_trapCfg_t,1);

    *pEnable = config.enable;

    return RT_ERR_OK;
}

int32
rtrpc_trap_rmaCancelMirror_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_trapCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapCfg_t));
    config.unit = unit;
    config.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_RMACANCELMIRROR_SET, &config, rtdrv_trapCfg_t,1);

    return RT_ERR_OK;
}

int32
rtrpc_trap_rmaGroupAction_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t * pRma_action)
{
    rtdrv_rmaGroupType_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_rmaGroupType_t));
    config.unit = unit;
    config.rmaGroup_frameType = rmaGroup_frameType;
    GETSOCKOPT(RTDRV_TRAP_RMAGROUPACTION_GET, &config,
            rtdrv_rmaGroupType_t, 1);

    osal_memcpy(pRma_action, &config.rma_action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}    /* end of rtk_trap_rmaGroupAction_get */

int32
rtrpc_trap_rmaGroupAction_set(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t rma_action)
{
    rtdrv_rmaGroupType_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_rmaGroupType_t));
    config.unit = unit;
    config.rmaGroup_frameType = rmaGroup_frameType;
    osal_memcpy(&config.rma_action, &rma_action, sizeof(rtk_action_t));

    SETSOCKOPT(RTDRV_TRAP_RMAGROUPACTION_SET, &config,
            rtdrv_rmaGroupType_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_rmaGroupAction_set */

int32
rtrpc_trap_rmaGroupLearningEnable_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_enable_t * pEnable)
{
    rtdrv_rmaGroupLearn_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_rmaGroupLearn_t));
    config.unit = unit;
    config.rmaGroup_frameType = rmaGroup_frameType;
    GETSOCKOPT(RTDRV_TRAP_RMAGROUPLEARNINGENABLE_GET, &config,
            rtdrv_rmaGroupType_t, 1);

    *pEnable = config.enable;

    return RT_ERR_OK;
}    /* end of rtk_trap_rmaGroupLearningEnable_get */

int32
rtrpc_trap_rmaGroupLearningEnable_set(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_enable_t enable)
{
    rtdrv_rmaGroupLearn_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_rmaGroupLearn_t));
    config.unit = unit;
    config.rmaGroup_frameType = rmaGroup_frameType;
    config.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_RMAGROUPLEARNINGENABLE_SET, &config,
            rtdrv_rmaGroupLearn_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_rmaGroupLearningEnable_set */

int32
rtrpc_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_trapOtherCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOtherCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMESELFARPENABLE_GET, &config,
            rtdrv_trapOtherCfg_t, 1);

    *pEnable = config.enable;

    return RT_ERR_OK;
}    /* end of rtk_trap_mgmtFrameSelfARPEnable_get */

int32
rtrpc_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_trapOtherCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_trapOtherCfg_t));
    config.unit = unit;
    config.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMESELFARPENABLE_SET, &config,
            rtdrv_trapOtherCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_trap_mgmtFrameSelfARPEnable_set */

int32
rtrpc_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    trap_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRAP_RMALOOKUPMISSACTIONENABLE_GET, &trap_cfg, rtdrv_trapCfg_t, 1);
    osal_memcpy(pEnable, &trap_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_trap_rmaLookupMissActionEnable_get */

int32
rtrpc_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_trapCfg_t trap_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trap_cfg, 0, sizeof(rtdrv_trapCfg_t));
    /* function body */
    trap_cfg.unit = unit;
    trap_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TRAP_RMALOOKUPMISSACTIONENABLE_SET, &trap_cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_rmaLookupMissActionEnable_set */

int32
rtrpc_trap_cfmAct_get(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t *pAction)
{
    rtdrv_trapCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_trap_cfmType_t));
    osal_memcpy(&cfg.md_level, &mdl, sizeof(uint32));
    GETSOCKOPT(RTDRV_TRAP_CFMACT_GET, &cfg, rtdrv_trapCfmCfg_t, 1);
    osal_memcpy(pAction, &cfg.action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmAct_get */

int32
rtrpc_trap_cfmAct_set(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t action)
{
    rtdrv_trapCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_trap_cfmType_t));
    osal_memcpy(&cfg.md_level, &mdl, sizeof(uint32));
    osal_memcpy(&cfg.action, &action, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_TRAP_CFMACT_SET, &cfg, rtdrv_trapCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmAct_set */

int32
rtrpc_trap_cfmTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    rtdrv_trapCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_TRAP_CFMTARGET_GET, &cfg, rtdrv_trapCfmCfg_t, 1);
    osal_memcpy(pTarget, &cfg.target, sizeof(rtk_trapTarget_t));

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmTarget_get */

int32
rtrpc_trap_cfmTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    rtdrv_trapCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfmCfg_t));
    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.target, &target, sizeof(rtk_trapTarget_t));
    SETSOCKOPT(RTDRV_TRAP_CFMTARGET_SET, &cfg, rtdrv_trapCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_cfmTarget_set */

int32
rtrpc_trap_oamTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    rtdrv_trapCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfg_t));
    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_TRAP_OAMTARGET_GET, &cfg, rtdrv_trapCfg_t, 1);
    osal_memcpy(pTarget, &cfg.target, sizeof(rtk_trapTarget_t));

    return RT_ERR_OK;
}   /* end of rtk_trap_oamTarget_get */

int32
rtrpc_trap_oamTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    rtdrv_trapCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfg_t));
    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.target, &target, sizeof(rtk_trapTarget_t));
    SETSOCKOPT(RTDRV_TRAP_OAMTARGET_SET, &cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_oamTarget_set */

int32
rtrpc_trap_mgmtFrameTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    rtdrv_trapCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfg_t));
    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_TRAP_MGMTFRAMETARGET_GET, &cfg, rtdrv_trapCfg_t, 1);
    osal_memcpy(pTarget, &cfg.target, sizeof(rtk_trapTarget_t));

    return RT_ERR_OK;
}   /* end of rtk_trap_mgmtFrameTarget_get */

int32
rtrpc_trap_mgmtFrameTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    rtdrv_trapCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_trapCfg_t));
    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.target, &target, sizeof(rtk_trapTarget_t));
    SETSOCKOPT(RTDRV_TRAP_MGMTFRAMETARGET_SET, &cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trap_mgmtFrameTarget_set */

int32
rtrpc_trap_capwapInvldHdr_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_trapCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_trapCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_TRAP_CAPWAPINVLDHDR_GET, &cfg, rtdrv_trapCfg_t, 1);
    memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtrpc_trap_capwapInvldHdr_get */

int32
rtrpc_trap_capwapInvldHdr_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_trapCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_trapCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_TRAP_CAPWAPINVLDHDR_SET, &cfg, rtdrv_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_trap_capwapInvldHdr_set */

int32
rtrpc_trapMapper_init(dal_mapper_t *pMapper)
{
    pMapper->trap_rmaAction_get = rtrpc_trap_rmaAction_get;
    pMapper->trap_rmaAction_set = rtrpc_trap_rmaAction_set;
    pMapper->trap_rmaGroupAction_get = rtrpc_trap_rmaGroupAction_get;
    pMapper->trap_rmaGroupAction_set = rtrpc_trap_rmaGroupAction_set;
    pMapper->trap_rmaLearningEnable_get = rtrpc_trap_rmaLearningEnable_get;
    pMapper->trap_rmaLearningEnable_set = rtrpc_trap_rmaLearningEnable_set;
    pMapper->trap_rmaGroupLearningEnable_get = rtrpc_trap_rmaGroupLearningEnable_get;
    pMapper->trap_rmaGroupLearningEnable_set = rtrpc_trap_rmaGroupLearningEnable_set;
    pMapper->trap_bypassStp_get = rtrpc_trap_bypassStp_get;
    pMapper->trap_bypassStp_set = rtrpc_trap_bypassStp_set;
    pMapper->trap_bypassVlan_get = rtrpc_trap_bypassVlan_get;
    pMapper->trap_bypassVlan_set = rtrpc_trap_bypassVlan_set;
    pMapper->trap_userDefineRma_get = rtrpc_trap_userDefineRma_get;
    pMapper->trap_userDefineRma_set = rtrpc_trap_userDefineRma_set;
    pMapper->trap_userDefineRmaEnable_get = rtrpc_trap_userDefineRmaEnable_get;
    pMapper->trap_userDefineRmaEnable_set = rtrpc_trap_userDefineRmaEnable_set;
    pMapper->trap_userDefineRmaAction_get = rtrpc_trap_userDefineRmaAction_get;
    pMapper->trap_userDefineRmaAction_set = rtrpc_trap_userDefineRmaAction_set;
    pMapper->trap_userDefineRmaLearningEnable_get = rtrpc_trap_userDefineRmaLearningEnable_get;
    pMapper->trap_userDefineRmaLearningEnable_set = rtrpc_trap_userDefineRmaLearningEnable_set;
    pMapper->trap_mgmtFrameAction_get = rtrpc_trap_mgmtFrameAction_get;
    pMapper->trap_mgmtFrameAction_set = rtrpc_trap_mgmtFrameAction_set;
    pMapper->trap_mgmtFramePri_get = rtrpc_trap_mgmtFramePri_get;
    pMapper->trap_mgmtFramePri_set = rtrpc_trap_mgmtFramePri_set;
    pMapper->trap_mgmtFrameQueue_get = rtrpc_trap_mgmtFrameQueue_get;
    pMapper->trap_mgmtFrameQueue_set = rtrpc_trap_mgmtFrameQueue_set;
    pMapper->trap_mgmtFrameLearningEnable_get = rtrpc_trap_mgmtFrameLearningEnable_get;
    pMapper->trap_mgmtFrameLearningEnable_set = rtrpc_trap_mgmtFrameLearningEnable_set;
    pMapper->trap_portMgmtFrameAction_get = rtrpc_trap_portMgmtFrameAction_get;
    pMapper->trap_portMgmtFrameAction_set = rtrpc_trap_portMgmtFrameAction_set;
    pMapper->trap_pktWithCFIAction_get = rtrpc_trap_pktWithCFIAction_get;
    pMapper->trap_pktWithCFIAction_set = rtrpc_trap_pktWithCFIAction_set;
    pMapper->trap_pktWithOuterCFIAction_get = rtrpc_trap_pktWithOuterCFIAction_get;
    pMapper->trap_pktWithOuterCFIAction_set = rtrpc_trap_pktWithOuterCFIAction_set;
    pMapper->trap_pktWithCFIPri_get = rtrpc_trap_pktWithCFIPri_get;
    pMapper->trap_pktWithCFIPri_set = rtrpc_trap_pktWithCFIPri_set;
    pMapper->trap_cfmFrameTrapPri_get = rtrpc_trap_cfmFrameTrapPri_get;
    pMapper->trap_cfmFrameTrapPri_set = rtrpc_trap_cfmFrameTrapPri_set;
    pMapper->trap_oamPDUAction_get = rtrpc_trap_oamPDUAction_get;
    pMapper->trap_oamPDUAction_set = rtrpc_trap_oamPDUAction_set;
    pMapper->trap_oamPDUPri_get = rtrpc_trap_oamPDUPri_get;
    pMapper->trap_oamPDUPri_set = rtrpc_trap_oamPDUPri_set;
    pMapper->trap_portOamLoopbackParAction_get = rtrpc_trap_portOamLoopbackParAction_get;
    pMapper->trap_portOamLoopbackParAction_set = rtrpc_trap_portOamLoopbackParAction_set;
    pMapper->trap_routeExceptionAction_get = rtrpc_trap_routeExceptionAction_get;
    pMapper->trap_routeExceptionAction_set = rtrpc_trap_routeExceptionAction_set;
    pMapper->trap_routeExceptionPri_get = rtrpc_trap_routeExceptionPri_get;
    pMapper->trap_routeExceptionPri_set = rtrpc_trap_routeExceptionPri_set;
    pMapper->trap_mgmtFrameMgmtVlanEnable_get = rtrpc_trap_mgmtFrameMgmtVlanEnable_get;
    pMapper->trap_mgmtFrameMgmtVlanEnable_set = rtrpc_trap_mgmtFrameMgmtVlanEnable_set;
    pMapper->trap_mgmtFrameSelfARPEnable_get = rtrpc_trap_mgmtFrameSelfARPEnable_get;
    pMapper->trap_mgmtFrameSelfARPEnable_set = rtrpc_trap_mgmtFrameSelfARPEnable_set;
    pMapper->trap_bpduFloodPortmask_get = rtrpc_trap_bpduFloodPortmask_get;
    pMapper->trap_bpduFloodPortmask_set = rtrpc_trap_bpduFloodPortmask_set;
    pMapper->trap_eapolFloodPortmask_get = rtrpc_trap_eapolFloodPortmask_get;
    pMapper->trap_eapolFloodPortmask_set = rtrpc_trap_eapolFloodPortmask_set;
    pMapper->trap_lldpFloodPortmask_get = rtrpc_trap_lldpFloodPortmask_get;
    pMapper->trap_lldpFloodPortmask_set = rtrpc_trap_lldpFloodPortmask_set;
    pMapper->trap_userDefineFloodPortmask_get = rtrpc_trap_userDefineFloodPortmask_get;
    pMapper->trap_userDefineFloodPortmask_set = rtrpc_trap_userDefineFloodPortmask_set;
    pMapper->trap_rmaFloodPortmask_get = rtrpc_trap_rmaFloodPortmask_get;
    pMapper->trap_rmaFloodPortmask_set = rtrpc_trap_rmaFloodPortmask_set;
    pMapper->trap_rmaCancelMirror_get = rtrpc_trap_rmaCancelMirror_get;
    pMapper->trap_rmaCancelMirror_set = rtrpc_trap_rmaCancelMirror_set;
    pMapper->trap_rmaLookupMissActionEnable_get = rtrpc_trap_rmaLookupMissActionEnable_get;
    pMapper->trap_rmaLookupMissActionEnable_set = rtrpc_trap_rmaLookupMissActionEnable_set;
    pMapper->trap_cfmAct_get = rtrpc_trap_cfmAct_get;
    pMapper->trap_cfmAct_set = rtrpc_trap_cfmAct_set;
    pMapper->trap_cfmTarget_get = rtrpc_trap_cfmTarget_get;
    pMapper->trap_cfmTarget_set = rtrpc_trap_cfmTarget_set;
    pMapper->trap_oamTarget_get = rtrpc_trap_oamTarget_get;
    pMapper->trap_oamTarget_set = rtrpc_trap_oamTarget_set;
    pMapper->trap_mgmtFrameTarget_get = rtrpc_trap_mgmtFrameTarget_get;
    pMapper->trap_mgmtFrameTarget_set = rtrpc_trap_mgmtFrameTarget_set;
    pMapper->trap_capwapInvldHdr_get = rtrpc_trap_capwapInvldHdr_get;
    pMapper->trap_capwapInvldHdr_set = rtrpc_trap_capwapInvldHdr_set;    
    return RT_ERR_OK;
}
