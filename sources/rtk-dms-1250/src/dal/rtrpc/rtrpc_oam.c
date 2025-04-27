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
 *           1) oam
 *
 */

#include <rtk/oam.h>
#include <dal/rtrpc/rtrpc_oam.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_oam_init(uint32 unit)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    SETSOCKOPT(RTDRV_OAM_INIT, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_oam_init */

int32 rtrpc_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port,
    uint8 *pPayload, uint32 len)
{
    rtdrv_oamDyingGaspCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamDyingGaspCfg_t));

    config.unit = unit;
    config.port = port;
    config.cnt = len;
    osal_memcpy((char*)config.payload, (char*)pPayload, len);
    SETSOCKOPT(RTDRV_OAM_PORTDYINGGASPPAYLOAD_SET, &config, rtdrv_oamDyingGaspCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_oamDyingGaspCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamDyingGaspCfg_t));

    config.unit = unit;
    config.enable = enable;
    SETSOCKOPT(RTDRV_OAM_DYINGGASPSEND_SET, &config, rtdrv_oamDyingGaspCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_oam_dyingGaspSend_set */

int32 rtrpc_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_oamDyingGaspCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamDyingGaspCfg_t));

    config.unit = unit;
    config.port = port;
    GETSOCKOPT(RTDRV_OAM_AUTODYINGGASPENABLE_GET, &config, rtdrv_oamDyingGaspCfg_t, 1);
    *pEnable = config.enable;

    return RT_ERR_OK;
}/* end of rtk_oam_autoDyingGaspEnable_get */


int32 rtrpc_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_oamDyingGaspCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamDyingGaspCfg_t));

    config.unit = unit;
    config.port = port;
    config.enable = enable;
    SETSOCKOPT(RTDRV_OAM_AUTODYINGGASPENABLE_SET, &config, rtdrv_oamDyingGaspCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_oam_autoDyingGaspEnable_set */

int32 rtrpc_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime)
{
    rtdrv_oamDyingGaspCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamDyingGaspCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_DYINGGASPWAITTIME_GET, &config, rtdrv_oamDyingGaspCfg_t, 1);
    *pTime = config.waitTime;

    return RT_ERR_OK;
} /* end of rtk_oam_dyingGaspWaitTime_get */


int32 rtrpc_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time)
{
    rtdrv_oamDyingGaspCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamDyingGaspCfg_t));

    config.unit = unit;
    config.waitTime = time;
    SETSOCKOPT(RTDRV_OAM_DYINGGASPWAITTIME_SET, &config, rtdrv_oamDyingGaspCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_oam_dyingGaspWaitTime_set */

int32 rtrpc_oam_loopbackMacSwapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_LOOPBACKMACSWAPENABLE_GET, &config, rtdrv_oamCfmMiscCfg_t, 1);

    *pEnable = config.loopbackEnable;

    return RT_ERR_OK;
}   /* end of rtk_oam_loopbackMacSwapEnable_get */

int32 rtrpc_oam_loopbackMacSwapEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    config.loopbackEnable = enable;
    SETSOCKOPT(RTDRV_OAM_LOOPBACKMACSWAPENABLE_SET, &config, rtdrv_oamCfmMiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_loopbackMacSwapEnable_set */

int32 rtrpc_oam_portLoopbackMuxAction_get(uint32 unit, rtk_port_t port,
        rtk_action_t *pAction)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    config.port = port;
    GETSOCKOPT(RTDRV_OAM_PORTLOOPBACKMUXACTION_GET, &config, rtdrv_oamCfmMiscCfg_t, 1);

    *pAction = config.action;

    return RT_ERR_OK;
}   /* end of rtk_oam_portLoopbackMuxAction_get */

int32 rtrpc_oam_portLoopbackMuxAction_set(uint32 unit, rtk_port_t port,
        rtk_action_t action)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    config.port = port;
    config.action = action;
    SETSOCKOPT(RTDRV_OAM_PORTLOOPBACKMUXACTION_SET, &config, rtdrv_oamCfmMiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_portLoopbackMuxAction_set */

int32
rtrpc_oam_cfmCcmPcp_get(uint32 unit, uint32 *pcp)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMCCMPCP_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *pcp = config.ccmFrame.outer_pri;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmPcp_get */

int32
rtrpc_oam_cfmCcmPcp_set(uint32 unit, uint32 pcp)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit                 = unit;
    config.ccmFrame.outer_pri   = pcp;
    SETSOCKOPT(RTDRV_OAM_CFMCCMPCP_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmPcp_set */

int32
rtrpc_oam_cfmCcmCfi_get(uint32 unit, uint32 *cfi)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMCCMCFI_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *cfi = config.ccmFrame.outer_dei;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmCfi_get */

int32
rtrpc_oam_cfmCcmCfi_set(uint32 unit, uint32 cfi)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit                 = unit;
    config.ccmFrame.outer_dei   = cfi;
    SETSOCKOPT(RTDRV_OAM_CFMCCMCFI_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmCfi_set */

int32
rtrpc_oam_cfmCcmTpid_get(uint32 unit, uint32 *tpid)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMCCMTPID_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *tpid = config.ccmFrame.outer_tpid;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmTpid_get */

int32
rtrpc_oam_cfmCcmTpid_set(uint32 unit, uint32 tpid)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit                 = unit;
    config.ccmFrame.outer_tpid  = tpid;
    SETSOCKOPT(RTDRV_OAM_CFMCCMTPID_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmTpid_set */

int32
rtrpc_oam_cfmCcmInstLifetime_get(uint32 unit, uint32 instance,
                                uint32 *lifetime)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMRESETLIFETIME_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *lifetime = config.ccmFlag;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstLifetime_get */

int32
rtrpc_oam_cfmCcmInstLifetime_set(uint32 unit, uint32 instance,
                                uint32 lifetime)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.ccmFlag  = lifetime;
    SETSOCKOPT(RTDRV_OAM_CFMCCMRESETLIFETIME_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstLifetime_set */
int32
rtrpc_oam_cfmCcmMepid_get(uint32 unit, uint32 *mepid)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMCCMMEPID_GET, &config, rtdrv_oamCfmMiscCfg_t, 1);
    *mepid = config.mepid;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmMepid_get */

int32
rtrpc_oam_cfmCcmMepid_set(uint32 unit, uint32 mepid)
{
    rtdrv_oamCfmMiscCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmMiscCfg_t));

    config.unit     = unit;
    config.mepid    = mepid;
    SETSOCKOPT(RTDRV_OAM_CFMCCMMEPID_SET, &config, rtdrv_oamCfmMiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmMepid_set */

int32
rtrpc_oam_cfmCcmIntervalField_get(uint32 unit, uint32 *interval)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMCCMINTERVALFIELD_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *interval = config.ccmFlag;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmIntervalField_get */

int32
rtrpc_oam_cfmCcmIntervalField_set(uint32 unit, uint32 interval)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.ccmFlag  = interval;
    SETSOCKOPT(RTDRV_OAM_CFMCCMINTERVALFIELD_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmIntervalField_set */

int32
rtrpc_oam_cfmCcmMdl_get(uint32 unit, uint32 *mdl)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMCCMMDL_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *mdl = config.cfmCfg.md_level;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmMdl_get */

int32
rtrpc_oam_cfmCcmMdl_set(uint32 unit, uint32 mdl)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit             = unit;
    config.cfmCfg.md_level  = mdl;
    SETSOCKOPT(RTDRV_OAM_CFMCCMMDL_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmMdl_set */

int32
rtrpc_oam_cfmCcmInstTagStatus_get(uint32 unit, uint32 instance,
                                rtk_enable_t *enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTTAGSTATUS_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *enable = config.enable;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTagStatus_get */

int32
rtrpc_oam_cfmCcmInstTagStatus_set(uint32 unit, uint32 instance,
                                rtk_enable_t enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.enable   = enable;
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTTAGSTATUS_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTagStatus_set */

int32
rtrpc_oam_cfmCcmInstVid_get(uint32 unit, uint32 instance,
                          rtk_vlan_t *vid)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTVID_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *vid = config.ccmFrame.outer_vid;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmInstVid_get */

int32
rtrpc_oam_cfmCcmInstVid_set(uint32 unit, uint32 instance,
                          rtk_vlan_t vid)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit                 = unit;
    config.cfmIdx               = instance;
    config.ccmFrame.outer_vid   = vid;
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTVID_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstVid_set */

int32
rtrpc_oam_cfmCcmInstMaid_get(uint32 unit, uint32 instance,
                           uint32 *maid)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTMAID_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *maid = config.maid;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmInstMaid_get */

int32
rtrpc_oam_cfmCcmInstMaid_set(uint32 unit, uint32 instance,
                           uint32 maid)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.maid     = maid;
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTMAID_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstMaid_set */

int32
rtrpc_oam_cfmCcmInstTxStatus_get(uint32 unit, uint32 instance,
                               rtk_enable_t *enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXSTATUS_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *enable = config.enable;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxStatus_get */

int32
rtrpc_oam_cfmCcmInstTxStatus_set(uint32 unit, uint32 instance,
                               rtk_enable_t enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.enable   = enable;
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXSTATUS_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmInstTxStatus_set */

int32
rtrpc_oam_cfmCcmInstInterval_get(uint32 unit, uint32 instance,
                               uint32 *interval)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTINTERVAL_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *interval = config.ccmInterval;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstInterval_get */

int32
rtrpc_oam_cfmCcmInstInterval_set(uint32 unit, uint32 instance,
                               uint32 interval)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.ccmInterval = interval;
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTINTERVAL_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstInterval_set */

int32
rtrpc_oam_cfmCcmTxInstPort_get(uint32 unit, uint32 instance,
                             uint32 index, rtk_port_t *port)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.portIdx  = index;
    GETSOCKOPT(RTDRV_OAM_CFMCCMTXINSTPORT_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *port = config.port;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmTxInstPort_get */

int32
rtrpc_oam_cfmCcmTxInstPort_set(uint32 unit, uint32 instance,
                             uint32 index, rtk_port_t port)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.portIdx  = index;
    config.port     = port;
    SETSOCKOPT(RTDRV_OAM_CFMCCMTXINSTPORT_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmTxInstPort_set */

int32
rtrpc_oam_cfmCcmRxInstVid_get(uint32 unit, uint32 instance,
                            rtk_vlan_t *vid)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    GETSOCKOPT(RTDRV_OAM_CFMCCMRXINSTVID_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *vid = config.ccmFrame.outer_vid;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmRxInstVid_get */

int32
rtrpc_oam_cfmCcmRxInstVid_set(uint32 unit, uint32 instance,
                            rtk_vlan_t vid)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit                 = unit;
    config.cfmIdx               = instance;
    config.ccmFrame.outer_vid   = vid;
    SETSOCKOPT(RTDRV_OAM_CFMCCMRXINSTVID_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmRxInstVid_set */

int32
rtrpc_oam_cfmCcmRxInstPort_get(uint32 unit, uint32 instance,
                             uint32 index, rtk_port_t *port)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.portIdx  = index;
    GETSOCKOPT(RTDRV_OAM_CFMCCMRXINSTPORT_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *port = config.port;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmRxInstPort_get */

int32
rtrpc_oam_cfmCcmRxInstPort_set(uint32 unit, uint32 instance,
                             uint32 index, rtk_port_t port)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.portIdx  = index;
    config.port     = port;
    SETSOCKOPT(RTDRV_OAM_CFMCCMRXINSTPORT_SET, &config, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmRxInstPort_set */

int32
rtrpc_oam_cfmCcmInstAliveTime_get(uint32 unit, uint32 instance,
                            uint32 index, uint32 *time)
{
    rtdrv_oamCcmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCcmCfg_t));

    config.unit     = unit;
    config.cfmIdx   = instance;
    config.portIdx  = index;
    GETSOCKOPT(RTDRV_OAM_CFMCCMKEEPALIVE_GET, &config, rtdrv_oamCcmCfg_t, 1);
    *time = config.ccmInterval;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstAliveTime_get */


int32
rtrpc_oam_cfmPortEthDmEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    config.port = port;
    GETSOCKOPT(RTDRV_OAM_CFMETHDMPORTENABLE_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *pEnable = config.enable;

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmPortEthDmEnable_get */

int32
rtrpc_oam_cfmPortEthDmEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    config.port = port;
    config.enable = enable;
    SETSOCKOPT(RTDRV_OAM_CFMETHDMPORTENABLE_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmPortEthDmEnable_set */

 int32
rtrpc_oam_cfmEthDmRxTimestamp_get(
    uint32 unit,
    uint32 index,
    rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));
    config.unit = unit;
    config.index = index;
    GETSOCKOPT(RTDRV_OAM_CFMETHDMRXTIMESTAMP_GET, &config, rtdrv_oamCfmCfg_t, 1);
    osal_memcpy(pTimeStamp, &config.timeStamp, sizeof(config.timeStamp));

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmTxDelay_get(
    uint32 unit,
    rtk_oam_cfmEthDmTxDelay_t *pTxDelay)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMETHDMTXDELAY_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *pTxDelay = config.txDelay;

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmTxDelay_set(
    uint32                      unit,
    rtk_oam_cfmEthDmTxDelay_t txDelay)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    config.txDelay = txDelay;
    SETSOCKOPT(RTDRV_OAM_CFMETHDMTXDELAY_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmRefTime_get(uint32 unit, rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMETHDMREFTIME_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *pTimeStamp = config.timeStamp;

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmRefTime_set(uint32 unit, rtk_time_timeStamp_t timeStamp)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    config.timeStamp = timeStamp;
    SETSOCKOPT(RTDRV_OAM_CFMETHDMREFTIME_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmRefTimeEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMETHDMREFTIMEENABLE_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *pEnable = config.enable;

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmRefTimeEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    config.enable = enable;
    SETSOCKOPT(RTDRV_OAM_CFMETHDMREFTIMEENABLE_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmRefTimeFreq_get(uint32 unit, uint32 *pFreq)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_OAM_CFMETHDMREFTIMEFREQ_GET, &config, rtdrv_oamCfmCfg_t, 1);
    *pFreq = config.freq;

    return RT_ERR_OK;
}

int32
rtrpc_oam_cfmEthDmRefTimeFreq_set(uint32 unit, uint32 freq)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.unit = unit;
    config.freq = freq;
    SETSOCKOPT(RTDRV_OAM_CFMETHDMREFTIMEFREQ_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt)
{
    rtdrv_oamDyingGaspCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_oamDyingGaspCfg_t));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_OAM_DYINGGASPPKTCNT_GET, &cfg, rtdrv_oamDyingGaspCfg_t, 1);

    *pCnt = cfg.cnt;

    return RT_ERR_OK;
}   /* end of rtk_oam_dyingGaspPktCnt_get */

int32
rtrpc_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt)
{
    rtdrv_oamDyingGaspCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_oamDyingGaspCfg_t));
    cfg.unit = unit;
    cfg.cnt = cnt;
    SETSOCKOPT(RTDRV_OAM_DYINGGASPPKTCNT_SET, &cfg, rtdrv_oamDyingGaspCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_dyingGaspPktCnt_set */

int32 rtrpc_oam_linkFaultMonEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_oamCfmCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_oamCfmCfg_t));

    config.enable = enable;
    SETSOCKOPT(RTDRV_OAM_LINKFAULTMONENABLE_SET, &config, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_linkFaultMonEnable_set */

int32
rtrpc_oam_pduLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_oamCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OAM_PDULEARNINGENABLE_GET, &cfg, rtdrv_oamCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_oam_pduLearningEnable_get */

int32
rtrpc_oam_pduLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_oamCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_OAM_PDULEARNINGENABLE_SET, &cfg, rtdrv_oamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_pduLearningEnable_set */

int32
rtrpc_oam_cfmCcmInstTxMepid_get(uint32 unit, uint32 instance, uint32 *mepid)
{
    rtdrv_oamCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == mepid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXMEPID_GET, &cfg, rtdrv_oamCfmCfg_t, 1);
    osal_memcpy(mepid, &cfg.mepid, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxMepid_get */

int32
rtrpc_oam_cfmCcmInstTxMepid_set(uint32 unit, uint32 instance, uint32 mepid)
{
    rtdrv_oamCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    osal_memcpy(&cfg.mepid, &mepid, sizeof(uint32));
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXMEPID_SET, &cfg, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxMepid_set */

int32
rtrpc_oam_cfmCcmInstTxIntervalField_get(uint32 unit, uint32 instance, uint32 *interval)
{
    rtdrv_oamCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == interval), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXINTERVALFIELD_GET, &cfg, rtdrv_oamCfmCfg_t, 1);
    osal_memcpy(interval, &cfg.interval, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxIntervalField_get */

int32
rtrpc_oam_cfmCcmInstTxIntervalField_set(uint32 unit, uint32 instance, uint32 interval)
{
    rtdrv_oamCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    osal_memcpy(&cfg.interval, &interval, sizeof(uint32));
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXINTERVALFIELD_SET, &cfg, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxIntervalField_set */

int32
rtrpc_oam_cfmCcmInstTxMdl_get(uint32 unit, uint32 instance, uint32 *mdl)
{
    rtdrv_oamCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == mdl), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXMDL_GET, &cfg, rtdrv_oamCfmCfg_t, 1);
    osal_memcpy(mdl, &cfg.mdl, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxMdl_get */

int32
rtrpc_oam_cfmCcmInstTxMdl_set(uint32 unit, uint32 instance, uint32 mdl)
{
    rtdrv_oamCfmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCfmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    osal_memcpy(&cfg.mdl, &mdl, sizeof(uint32));
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXMDL_SET, &cfg, rtdrv_oamCfmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxMdl_set */

int32
rtrpc_oam_cfmCcmInstTxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    rtdrv_oamCcmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == member), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCcmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXMEMBER_GET, &cfg, rtdrv_oamCcmCfg_t, 1);
    osal_memcpy(member, &cfg.member, sizeof(rtk_oam_cfmInstMember_t));

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxMember_get */

int32
rtrpc_oam_cfmCcmInstTxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    rtdrv_oamCcmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == member), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCcmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    osal_memcpy(&cfg.member, member, sizeof(rtk_oam_cfmInstMember_t));
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTTXMEMBER_SET, &cfg, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstTxMember_set */

int32
rtrpc_oam_cfmCcmInstRxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    rtdrv_oamCcmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == member), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCcmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    GETSOCKOPT(RTDRV_OAM_CFMCCMINSTRXMEMBER_GET, &cfg, rtdrv_oamCcmCfg_t, 1);
    osal_memcpy(member, &cfg.member, sizeof(rtk_oam_cfmInstMember_t));

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstRxMember_get */

int32
rtrpc_oam_cfmCcmInstRxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    rtdrv_oamCcmCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == member), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_oamCcmCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.instance, &instance, sizeof(uint32));
    osal_memcpy(&cfg.member, member, sizeof(rtk_oam_cfmInstMember_t));
    SETSOCKOPT(RTDRV_OAM_CFMCCMINSTRXMEMBER_SET, &cfg, rtdrv_oamCcmCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_cfmCcmInstRxMember_set */
int32
rtrpc_oamMapper_init(dal_mapper_t *pMapper)
{
    pMapper->oam_portDyingGaspPayload_set = rtrpc_oam_portDyingGaspPayload_set;
    pMapper->oam_dyingGaspSend_set = rtrpc_oam_dyingGaspSend_set;
    pMapper->oam_autoDyingGaspEnable_get = rtrpc_oam_autoDyingGaspEnable_get;
    pMapper->oam_autoDyingGaspEnable_set = rtrpc_oam_autoDyingGaspEnable_set;
    pMapper->oam_dyingGaspWaitTime_get = rtrpc_oam_dyingGaspWaitTime_get;
    pMapper->oam_dyingGaspWaitTime_set = rtrpc_oam_dyingGaspWaitTime_set;
    pMapper->oam_loopbackMacSwapEnable_get = rtrpc_oam_loopbackMacSwapEnable_get;
    pMapper->oam_loopbackMacSwapEnable_set = rtrpc_oam_loopbackMacSwapEnable_set;
    pMapper->oam_portLoopbackMuxAction_get = rtrpc_oam_portLoopbackMuxAction_get;
    pMapper->oam_portLoopbackMuxAction_set = rtrpc_oam_portLoopbackMuxAction_set;
    pMapper->oam_cfmCcmPcp_get = rtrpc_oam_cfmCcmPcp_get;
    pMapper->oam_cfmCcmPcp_set = rtrpc_oam_cfmCcmPcp_set;
    pMapper->oam_cfmCcmCfi_get = rtrpc_oam_cfmCcmCfi_get;
    pMapper->oam_cfmCcmCfi_set = rtrpc_oam_cfmCcmCfi_set;
    pMapper->oam_cfmCcmTpid_get = rtrpc_oam_cfmCcmTpid_get;
    pMapper->oam_cfmCcmTpid_set = rtrpc_oam_cfmCcmTpid_set;
    pMapper->oam_cfmCcmInstLifetime_get = rtrpc_oam_cfmCcmInstLifetime_get;
    pMapper->oam_cfmCcmInstLifetime_set = rtrpc_oam_cfmCcmInstLifetime_set;
    pMapper->oam_cfmCcmMepid_get = rtrpc_oam_cfmCcmMepid_get;
    pMapper->oam_cfmCcmMepid_set = rtrpc_oam_cfmCcmMepid_set;
    pMapper->oam_cfmCcmIntervalField_get = rtrpc_oam_cfmCcmIntervalField_get;
    pMapper->oam_cfmCcmIntervalField_set = rtrpc_oam_cfmCcmIntervalField_set;
    pMapper->oam_cfmCcmMdl_get = rtrpc_oam_cfmCcmMdl_get;
    pMapper->oam_cfmCcmMdl_set = rtrpc_oam_cfmCcmMdl_set;
    pMapper->oam_cfmCcmInstTagStatus_get = rtrpc_oam_cfmCcmInstTagStatus_get;
    pMapper->oam_cfmCcmInstTagStatus_set = rtrpc_oam_cfmCcmInstTagStatus_set;
    pMapper->oam_cfmCcmInstVid_get = rtrpc_oam_cfmCcmInstVid_get;
    pMapper->oam_cfmCcmInstVid_set = rtrpc_oam_cfmCcmInstVid_set;
    pMapper->oam_cfmCcmInstMaid_get = rtrpc_oam_cfmCcmInstMaid_get;
    pMapper->oam_cfmCcmInstMaid_set = rtrpc_oam_cfmCcmInstMaid_set;
    pMapper->oam_cfmCcmInstTxStatus_get = rtrpc_oam_cfmCcmInstTxStatus_get;
    pMapper->oam_cfmCcmInstTxStatus_set = rtrpc_oam_cfmCcmInstTxStatus_set;
    pMapper->oam_cfmCcmInstInterval_get = rtrpc_oam_cfmCcmInstInterval_get;
    pMapper->oam_cfmCcmInstInterval_set = rtrpc_oam_cfmCcmInstInterval_set;
    pMapper->oam_cfmCcmTxInstPort_get = rtrpc_oam_cfmCcmTxInstPort_get;
    pMapper->oam_cfmCcmTxInstPort_set = rtrpc_oam_cfmCcmTxInstPort_set;
    pMapper->oam_cfmCcmRxInstVid_get = rtrpc_oam_cfmCcmRxInstVid_get;
    pMapper->oam_cfmCcmRxInstVid_set = rtrpc_oam_cfmCcmRxInstVid_set;
    pMapper->oam_cfmCcmRxInstPort_get = rtrpc_oam_cfmCcmRxInstPort_get;
    pMapper->oam_cfmCcmRxInstPort_set = rtrpc_oam_cfmCcmRxInstPort_set;
    pMapper->oam_cfmCcmKeepalive_get = rtrpc_oam_cfmCcmInstAliveTime_get;
    pMapper->oam_cfmPortEthDmEnable_get = rtrpc_oam_cfmPortEthDmEnable_get;
    pMapper->oam_cfmPortEthDmEnable_set = rtrpc_oam_cfmPortEthDmEnable_set;
    pMapper->oam_cfmEthDmRxTimestamp_get = rtrpc_oam_cfmEthDmRxTimestamp_get;
    pMapper->oam_cfmEthDmTxDelay_get = rtrpc_oam_cfmEthDmTxDelay_get;
    pMapper->oam_cfmEthDmTxDelay_set = rtrpc_oam_cfmEthDmTxDelay_set;
    pMapper->oam_cfmEthDmRefTime_get = rtrpc_oam_cfmEthDmRefTime_get;
    pMapper->oam_cfmEthDmRefTime_set = rtrpc_oam_cfmEthDmRefTime_set;
    pMapper->oam_cfmEthDmRefTimeEnable_get = rtrpc_oam_cfmEthDmRefTimeEnable_get;
    pMapper->oam_cfmEthDmRefTimeEnable_set = rtrpc_oam_cfmEthDmRefTimeEnable_set;
    pMapper->oam_cfmEthDmRefTimeFreq_get = rtrpc_oam_cfmEthDmRefTimeFreq_get;
    pMapper->oam_cfmEthDmRefTimeFreq_set = rtrpc_oam_cfmEthDmRefTimeFreq_set;
    pMapper->oam_dyingGaspPktCnt_get = rtrpc_oam_dyingGaspPktCnt_get;
    pMapper->oam_dyingGaspPktCnt_set = rtrpc_oam_dyingGaspPktCnt_set;
    pMapper->oam_pduLearningEnable_get = rtrpc_oam_pduLearningEnable_get;
    pMapper->oam_pduLearningEnable_set = rtrpc_oam_pduLearningEnable_set;
    pMapper->oam_cfmCcmInstTxMepid_get = rtrpc_oam_cfmCcmInstTxMepid_get;
    pMapper->oam_cfmCcmInstTxMepid_set = rtrpc_oam_cfmCcmInstTxMepid_set;
    pMapper->oam_cfmCcmInstTxIntervalField_get = rtrpc_oam_cfmCcmInstTxIntervalField_get;
    pMapper->oam_cfmCcmInstTxIntervalField_set = rtrpc_oam_cfmCcmInstTxIntervalField_set;
    pMapper->oam_cfmCcmInstTxMdl_get = rtrpc_oam_cfmCcmInstTxMdl_get;
    pMapper->oam_cfmCcmInstTxMdl_set = rtrpc_oam_cfmCcmInstTxMdl_set;
    pMapper->oam_cfmCcmInstTxMember_get = rtrpc_oam_cfmCcmInstTxMember_get;
    pMapper->oam_cfmCcmInstTxMember_set = rtrpc_oam_cfmCcmInstTxMember_set;
    pMapper->oam_cfmCcmInstRxMember_get = rtrpc_oam_cfmCcmInstRxMember_get;
    pMapper->oam_cfmCcmInstRxMember_set = rtrpc_oam_cfmCcmInstRxMember_set;
    return RT_ERR_OK;
}
