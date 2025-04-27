/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 82847 $
 * $Date: 2017-10-23 10:50:59 +0800 (Mon, 23 Oct 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) l2 address
 *
 */

#include <dal/rtrpc/rtrpc_l2.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <dal/rtrpc/rtrpc_msg.h>


int32
rtrpc_l2_init(uint32 unit)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    SETSOCKOPT(RTDRV_L2_INIT, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    GETSOCKOPT(RTDRV_L2_FLUSH_LINK_DOWN_PORT_ADDR_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_common.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.enable = enable;
    SETSOCKOPT(RTDRV_L2_FLUSH_LINK_DOWN_PORT_ADDR_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_flush.unit = unit;
    osal_memcpy(&config.l2_flush.flush, pConfig, sizeof(rtk_l2_flushCfg_t));
    SETSOCKOPT(RTDRV_L2_UCASTADDR_FLUSH, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_learningCnt_get(uint32 unit, uint32 *pMac_cnt)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_LEARNING_CNT_GET, &l2_learn, rtdrv_l2_learnCnt_t, 1);
    *pMac_cnt = l2_learn.mac_cnt;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_LEARNING_CNT_GET, &l2_learn, rtdrv_l2_learnCnt_t, 1);
    *pMac_cnt = l2_learn.mac_cnt;

    return RT_ERR_OK;
}

int32
rtrpc_l2_fidLearningCnt_get(uint32 unit, uint32 fid_macLimit_idx, uint32 *pNum)
{
    rtdrv_l2_learnFidCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnFidCnt_t));
    l2_learn.unit = unit;
    l2_learn.entryIdx = fid_macLimit_idx;
    GETSOCKOPT(RTDRV_L2_FID_LEARNING_CNT_GET, &l2_learn, rtdrv_l2_learnFidCnt_t, 1);
    *pNum = l2_learn.mac_cnt;

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.type = type;
    osal_memcpy(&config.l2_macLimit.cnt, pLimitCnt, sizeof(rtk_l2_macCnt_t));
    GETSOCKOPT(RTDRV_L2_MAC_LEARNING_CNT_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLimitCnt, &config.l2_macLimit.cnt, sizeof(rtk_l2_macCnt_t));

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_limitLearningCnt_get(uint32 unit, uint32 *pMac_cnt)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_CNT_GET, &l2_learn, rtdrv_l2_learnCnt_t, 1);
    *pMac_cnt = l2_learn.mac_cnt;

    return RT_ERR_OK;
}

int32
rtrpc_l2_limitLearningCnt_set(uint32 unit, uint32 mac_cnt)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.mac_cnt = mac_cnt;
    SETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_CNT_SET, &l2_learn, rtdrv_l2_learnCnt_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLimitLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_LIMIT_LEARNING_CNT_GET, &l2_learn, rtdrv_l2_learnCnt_t, 1);
    *pMac_cnt = l2_learn.mac_cnt;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLimitLearningCnt_set(uint32 unit, rtk_port_t port, uint32 mac_cnt)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    l2_learn.mac_cnt = mac_cnt;
    SETSOCKOPT(RTDRV_L2_PORT_LIMIT_LEARNING_CNT_SET, &l2_learn, rtdrv_l2_learnCnt_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.type = type;
    osal_memcpy(&config.l2_macLimit.cnt, pLimitCnt, sizeof(rtk_l2_macCnt_t));
    GETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_NUM_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLimitCnt, &config.l2_macLimit.cnt, sizeof(rtk_l2_macCnt_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.type = type;
    osal_memcpy(&config.l2_macLimit.cnt, pLimitCnt, sizeof(rtk_l2_macCnt_t));
    SETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_NUM_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_limitLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_CNT_ACT_GET, &l2_learn, rtdrv_l2_learnCnt_t, 1);
    *pAction = l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_limitLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.action = action;
    SETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_CNT_ACT_SET, &l2_learn, rtdrv_l2_learnCnt_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLimitLearningCntAction_get(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t *pAction)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_LIMIT_LEARNING_CNT_ACT_GET, &l2_learn, rtdrv_l2_learnCnt_t, 1);
    *pAction = l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLimitLearningCntAction_set(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t action)
{
    rtdrv_l2_learnCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCnt_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    l2_learn.action = action;
    SETSOCKOPT(RTDRV_L2_PORT_LIMIT_LEARNING_CNT_ACT_SET, &l2_learn, rtdrv_l2_learnCnt_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_fidLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction)
{
    rtdrv_l2_learnFidCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnFidCnt_t));
    l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_FID_LIMIT_LEARNING_CNT_ACT_GET, &l2_learn, rtdrv_l2_learnFidCnt_t, 1);
    *pAction = l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_fidLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action)
{
    rtdrv_l2_learnFidCnt_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnFidCnt_t));
    l2_learn.unit = unit;
    l2_learn.action = action;
    SETSOCKOPT(RTDRV_L2_FID_LIMIT_LEARNING_CNT_ACT_SET, &l2_learn, rtdrv_l2_learnFidCnt_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.type = type;
    osal_memcpy(&config.l2_macLimit.action, pAction, sizeof(rtk_l2_macLimitAction_t));
    GETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_ACT_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pAction, &config.l2_macLimit.action, sizeof(rtk_l2_macLimitAction_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.type = type;
    osal_memcpy(&config.l2_macLimit.action, pAction, sizeof(rtk_l2_macLimitAction_t));
    SETSOCKOPT(RTDRV_L2_LIMIT_LEARNING_ACT_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_fidLimitLearningEntry_get(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.fidLimitEntryId = fid_macLimit_idx;
    GETSOCKOPT(RTDRV_L2_FID_LIMIT_LEARNING_ENTRY_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFidMacLimitEntry, &config.l2_macLimit.entry, sizeof(rtk_l2_fidMacLimitEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_fidLimitLearningEntry_set(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.fidLimitEntryId = fid_macLimit_idx;
    osal_memcpy(&config.l2_macLimit.entry, pFidMacLimitEntry, sizeof(rtk_l2_fidMacLimitEntry_t));
    SETSOCKOPT(RTDRV_L2_FID_LIMIT_LEARNING_ENTRY_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_macLimit.unit = unit;
    config.l2_macLimit.fidLimitEntryId = fid_macLimit_idx;
    SETSOCKOPT(RTDRV_L2_FID_LEARNING_CNT_RESET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_aging_get(uint32 unit, uint32 *pAging_time)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));
    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2_AGING_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pAging_time = unit_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_l2_aging_set(uint32 unit, uint32 aging_time)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));
    unit_cfg.unit = unit;
    unit_cfg.data = aging_time;
    SETSOCKOPT(RTDRV_L2_AGING_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.type = type;
    GETSOCKOPT(RTDRV_L2_AGING_TIME_GET, &config, rtdrv_l2Cfg_t, 1);
    *pAging_time = config.l2_age.ageTime;

    return RT_ERR_OK;
}

int32
rtrpc_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.type = type;
    config.l2_age.ageTime = aging_time ;
    SETSOCKOPT(RTDRV_L2_AGING_TIME_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_AGING_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_age.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.port = port;
    config.l2_age.enable  = enable ;
    SETSOCKOPT(RTDRV_L2_PORT_AGING_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_trkAgingEnable_get(uint32 unit, rtk_trk_t trunk, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.trunk = trunk;
    GETSOCKOPT(RTDRV_L2_TRK_AGING_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_age.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_trkAgingEnable_set(uint32 unit, rtk_trk_t trunk, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.trunk = trunk;
    config.l2_age.enable  = enable;
    SETSOCKOPT(RTDRV_L2_TRK_AGING_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo)
{
    rtdrv_l2_learnCfg_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_HASH_ALGO_GET, &l2_learn, rtdrv_l2_learnCfg_t, 1);
    *pHash_algo = l2_learn.hash_algo;

    return RT_ERR_OK;
}

int32
rtrpc_l2_hashAlgo_set(uint32 unit, uint32 hash_algo)
{
    rtdrv_l2_learnCfg_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_learn.unit = unit;
    l2_learn.hash_algo = hash_algo;
    SETSOCKOPT(RTDRV_L2_HASH_ALGO_SET, &l2_learn, rtdrv_l2_learnCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_hash.unit = unit;
    config.l2_hash.bucket = bucket;
    GETSOCKOPT(RTDRV_L2_BUCKET_HASH_ALGO_GET, &config, rtdrv_l2Cfg_t, 1);
    *pHash_algo = config.l2_hash.hashAlgo;

    return RT_ERR_OK;
}

int32
rtrpc_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_hash.unit = unit;
    config.l2_hash.bucket = bucket;
    config.l2_hash.hashAlgo = hash_algo;
    SETSOCKOPT(RTDRV_L2_BUCKET_HASH_ALGO_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_vlanMode_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pVlanMode)
{
    rtdrv_l2_learnCfg_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_VLANMODE_GET, &l2_learn, rtdrv_l2_learnCfg_t, 1);
    *pVlanMode = l2_learn.vlanMode;

    return RT_ERR_OK;
}

int32
rtrpc_l2_vlanMode_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t vlanMode)
{
    rtdrv_l2_learnCfg_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    l2_learn.vlanMode = vlanMode;
    SETSOCKOPT(RTDRV_L2_VLANMODE_SET, &l2_learn, rtdrv_l2_learnCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_learningFullAction_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_LEARNING_FULL_ACT_GET, &config, rtdrv_l2Cfg_t, 1);
    *pAction = config.l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_learningFullAction_set(uint32 unit, rtk_action_t action)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    config.l2_learn.action= action;
    SETSOCKOPT(RTDRV_L2_LEARNING_FULL_ACT_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portNewMacOp_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    config.l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_NEW_MAC_OP_GET, &config, rtdrv_l2Cfg_t, 1);
    *pLrnMode = config.l2_learn.lrnMode;
    *pFwdAction = config.l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portNewMacOp_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    config.l2_learn.port = port;
    config.l2_learn.lrnMode = lrnMode;
    config.l2_learn.action = fwdAction;
    SETSOCKOPT(RTDRV_L2_PORT_NEW_MAC_OP_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_init(
    uint32              unit,
    rtk_vlan_t          vid,
    rtk_mac_t           *pMac,
    rtk_l2_ucastAddr_t  *pL2_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    config.l2_ucAddr.vid= vid;
    osal_memcpy(&config.l2_ucAddr.mac, pMac, sizeof(rtk_mac_t));
    GETSOCKOPT(RTDRV_L2_ADDR_INIT, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2_addr, &config.l2_ucAddr.ucast, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    osal_memcpy(&config.l2_ucAddr.ucast, pL2_addr, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_ADDR_ADD, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2_addr, &config.l2_ucAddr.ucast, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    config.l2_ucAddr.vid= vid;
    osal_memcpy(&config.l2_ucAddr.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L2_ADDR_DEL, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    osal_memcpy(&config.l2_ucAddr.ucast, pL2_addr, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_ADDR_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2_addr, &config.l2_ucAddr.ucast, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    osal_memcpy(&config.l2_ucAddr.ucast, pL2_addr, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_ADDR_SET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2_addr, &config.l2_ucAddr.ucast, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_delAll(uint32 unit, uint32 include_static)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    config.l2_ucAddr.include_static = include_static;
    SETSOCKOPT(RTDRV_L2_ADDR_DEL_ALL, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_nextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    uint32              include_static,
    rtk_l2_ucastAddr_t  *pL2_data)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    config.l2_ucAddr.scan_idx = *pScan_idx;
    config.l2_ucAddr.include_static = include_static;
    osal_memcpy(&config.l2_ucAddr.ucast, pL2_data, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXT_VALID_ADDR_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2_data, &config.l2_ucAddr.ucast, sizeof(rtk_l2_ucastAddr_t));
    *pScan_idx = config.l2_ucAddr.scan_idx;

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcast_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    config.l2_mcAddr.vid= vid;
    osal_memcpy(&config.l2_mcAddr.mac, pMac, sizeof(rtk_mac_t));
    GETSOCKOPT(RTDRV_L2_MCAST_ADDR_INIT, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcast_addr, &config.l2_mcAddr.mcast, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    osal_memcpy(&config.l2_mcAddr.mcast, pMcast_addr, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_MCAST_ADDR_ADD, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcast_addr, &config.l2_mcAddr.mcast, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}


int32
rtrpc_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    config.l2_mcAddr.vid= vid;
    osal_memcpy(&config.l2_mcAddr.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L2_MCAST_ADDR_DEL, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    osal_memcpy(&config.l2_mcAddr.mcast, pMcast_addr, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_MCAST_ADDR_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcast_addr, &config.l2_mcAddr.mcast, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    osal_memcpy(&config.l2_mcAddr.mcast, pMcast_addr, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_MCAST_ADDR_SET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcast_addr, &config.l2_mcAddr.mcast, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    rtdrv_l2_mcastAddrData_t mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&mcast_data, 0, sizeof(rtdrv_l2_mcastAddrData_t));
    mcast_data.unit = unit;
    osal_memcpy(&(mcast_data.m_data), pMcast_addr, sizeof(rtk_l2_mcastAddr_t));
    SETSOCKOPT(RTDRV_L2_MCAST_ADDR_ADDBYINDEX, &mcast_data, rtdrv_l2_mcastAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_setByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    osal_memcpy(&config.l2_mcAddr.mcast, pMcast_addr, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_MCAST_ADDR_SET_BY_INDEX, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcast_addr, &config.l2_mcAddr.mcast, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastAddr_delIgnoreIndex(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    config.l2_mcAddr.vid= vid;
    osal_memcpy(&config.l2_mcAddr.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L2_MCAST_ADDR_DEL_IGNORE_INDEX, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pL2_data)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_mcAddr.unit = unit;
    config.l2_mcAddr.scan_idx = *pScan_idx;
    osal_memcpy(&config.l2_mcAddr.mcast, pL2_data, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXT_VALID_MCAST_ADDR_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2_data, &config.l2_mcAddr.mcast, sizeof(rtk_l2_mcastAddr_t));
    *pScan_idx = config.l2_mcAddr.scan_idx;

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipmcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode)
{
    rtdrv_l2_learnCfg_t               l2_config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_config, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_config.unit = unit;
    GETSOCKOPT(RTDRV_L2_IPMCMODE_GET, &l2_config, rtdrv_l2_learnCfg_t, 1);
    *pMode = l2_config.ipmcMode;

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipmcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode)
{
    rtdrv_l2_learnCfg_t               l2_config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_config, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_config.unit = unit;
    l2_config.ipmcMode = mode;
    SETSOCKOPT(RTDRV_L2_IPMCMODE_SET, &l2_config, rtdrv_l2_learnCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddrExt_init(uint32 unit, rtk_l2_ipMcastHashKey_t *pIpMcast_hashKey, rtk_l2_ipMcastAddr_t *pIpMcast_addr)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    osal_memcpy(&ipMcast_data.ipMcast_hashKey, pIpMcast_hashKey, sizeof(rtk_l2_ipMcastHashKey_t));
    osal_memcpy(&ipMcast_data.ip_m_data, pIpMcast_addr, sizeof(rtk_l2_ipMcastAddr_t));
    GETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_EX_INIT, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);
    osal_memcpy(pIpMcast_addr, &ipMcast_data.ip_m_data, sizeof(rtk_l2_ipMcastAddr_t));

    return RT_ERR_OK;
}


int32
rtrpc_l2_ipMcastAddr_add(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    osal_memcpy(&ipMcast_data.ip_m_data, pIpmcast_addr, sizeof(rtk_l2_ipMcastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_ADD, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddr_del(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    ipMcast_data.ip_m_data.sip = sip;
    ipMcast_data.ip_m_data.dip = dip;
    ipMcast_data.ip_m_data.rvid = vid;
    SETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_DEL, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddr_get(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    osal_memcpy(&ipMcast_data.ip_m_data, pIpmcast_addr, sizeof(rtk_l2_ipMcastAddr_t));
    GETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_GET, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);
    osal_memcpy(pIpmcast_addr, &ipMcast_data.ip_m_data, sizeof(rtk_l2_ipMcastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddr_set(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    osal_memcpy(&ipMcast_data.ip_m_data, pIpmcast_addr, sizeof(rtk_l2_ipMcastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_SET, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddr_addByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpMcast_addr)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    osal_memcpy(&ipMcast_data.ip_m_data, pIpMcast_addr, sizeof(rtk_l2_ipMcastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_ADDBYINDEX, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddr_setByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    osal_memcpy(&ipMcast_data.ip_m_data, pIpmcast_addr, sizeof(rtk_l2_ipMcastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_SET_BY_INDEX, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddr_delIgnoreIndex(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    ipMcast_data.ip_m_data.sip = sip;
    ipMcast_data.ip_m_data.dip = dip;
    ipMcast_data.ip_m_data.rvid = vid;
    SETSOCKOPT(RTDRV_L2_IP_MCAST_ADDR_DEL_IGNORE_INDEX, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_l2_nextValidIpMcastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ipMcastAddr_t    *pL2_data)
{
    rtdrv_l2_ipMcstAddrData_t ipMcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ipMcast_data, 0, sizeof(rtdrv_l2_ipMcstAddrData_t));
    ipMcast_data.unit = unit;
    ipMcast_data.index = *pScan_idx;
    osal_memcpy(&ipMcast_data.ip_m_data, pL2_data, sizeof(rtk_l2_ipMcastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXT_VALID_IPMCASTADDR_GET, &ipMcast_data, rtdrv_l2_ipMcstAddrData_t, 1);
    *pScan_idx = ipMcast_data.index;
    osal_memcpy(pL2_data, &ipMcast_data.ip_m_data, sizeof(rtk_l2_ipMcastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddrChkEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2_learnCfg_t               l2_config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_config, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_config.unit = unit;
    GETSOCKOPT(RTDRV_L2_IPMC_DIP_CHK_GET, &l2_config, rtdrv_l2_learnCfg_t, 1);
    *pEnable = l2_config.dip_check;

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcastAddrChkEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2_learnCfg_t               l2_config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_config, 0, sizeof(rtdrv_l2_learnCfg_t));
    l2_config.unit = unit;
    l2_config.dip_check = enable;
    SETSOCKOPT(RTDRV_L2_IPMC_DIP_CHK_SET, &l2_config, rtdrv_l2_learnCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcstFidVidCompareEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2_common_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_l2_common_t));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2_IPMC_VLAN_COMPARE_GET, &cfg, rtdrv_l2_common_t, 1);
    *pEnable = cfg.value;

    return RT_ERR_OK;
}

int32
rtrpc_l2_ipMcstFidVidCompareEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2_common_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_l2_common_t));
    cfg.unit = unit;
    cfg.value = enable;
    SETSOCKOPT(RTDRV_L2_IPMC_VLAN_COMPARE_SET, &cfg, rtdrv_l2_common_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6mcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode)
{
    rtdrv_unitCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_unitCfg_t));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2_IP6MCASTMODE_GET, &cfg, rtdrv_unitCfg_t, 1);
    *pMode = cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6mcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode)
{
    rtdrv_unitCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_unitCfg_t));
    cfg.unit = unit;
    cfg.data = mode;
    SETSOCKOPT(RTDRV_L2_IP6MCASTMODE_SET, &cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6CareByte_get(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 *pCareByte)
{
    rtdrv_l2_hashCareByte_t l2_hashCareByte;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_hashCareByte, 0, sizeof(rtdrv_l2_hashCareByte_t));
    l2_hashCareByte.unit = unit;
    l2_hashCareByte.type = type;
    GETSOCKOPT(RTDRV_L2_HASHCAREBYTE_GET, &l2_hashCareByte, rtdrv_l2_hashCareByte_t, 1);
    *pCareByte = l2_hashCareByte.value;

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6CareByte_set(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 careByte)
{
    rtdrv_l2_hashCareByte_t l2_hashCareByte;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_hashCareByte, 0, sizeof(rtdrv_l2_hashCareByte_t));
    l2_hashCareByte.unit = unit;
    l2_hashCareByte.type = type;
    l2_hashCareByte.value = careByte;
    SETSOCKOPT(RTDRV_L2_HASHCAREBYTE_SET, &l2_hashCareByte, rtdrv_l2_hashCareByte_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddrExt_init(uint32 unit, rtk_l2_ip6McastHashKey_t *pIp6Mcast_hashKey, rtk_l2_ip6McastAddr_t *pIp6Mcast_addr)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIp6Mcast_hashKey, sizeof(rtk_l2_ip6McastAddr_t));
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIp6Mcast_addr, sizeof(rtk_l2_ip6McastAddr_t));
    GETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_EX_INIT, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);
    osal_memcpy(pIp6Mcast_addr, &ip6Mcast_data.ip6_m_data, sizeof(rtk_l2_ip6McastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_add(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIpmcast_addr, sizeof(rtk_l2_ip6McastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_ADD, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_del(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    ip6Mcast_data.ip6_m_data.sip = sip;
    ip6Mcast_data.ip6_m_data.dip = dip;
    ip6Mcast_data.ip6_m_data.rvid = vid;
    SETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_DEL, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_get(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIpmcast_addr, sizeof(rtk_l2_ip6McastAddr_t));
    GETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_GET, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);
    osal_memcpy(pIpmcast_addr, &ip6Mcast_data.ip6_m_data, sizeof(rtk_l2_ip6McastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_set(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIpmcast_addr, sizeof(rtk_l2_ip6McastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_SET, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_addByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpMcast_addr)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIpMcast_addr, sizeof(rtk_l2_ip6McastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_ADDBYINDEX, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_setByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    osal_memcpy(&ip6Mcast_data.ip6_m_data, pIpmcast_addr, sizeof(rtk_l2_ip6McastAddr_t));
    SETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_SET_BY_INDEX, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_ip6McastAddr_delIgnoreIndex(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid)
{
    rtdrv_l2_ip6McstAddrData_t ip6Mcast_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6Mcast_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6Mcast_data.unit = unit;
    ip6Mcast_data.ip6_m_data.sip = sip;
    ip6Mcast_data.ip6_m_data.dip = dip;
    ip6Mcast_data.ip6_m_data.rvid = vid;
    SETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_DEL_IGNORE_INDEX, &ip6Mcast_data, rtdrv_l2_ip6McstAddrData_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_nextValidIp6McastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ip6McastAddr_t    *pL2_data)
{
    rtdrv_l2_ip6McstAddrData_t ip6_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&ip6_data, 0, sizeof(rtdrv_l2_ip6McstAddrData_t));
    ip6_data.unit = unit;
    ip6_data.index = *pScan_idx;
    osal_memcpy(&ip6_data.ip6_m_data, pL2_data, sizeof(rtk_l2_ip6McastAddr_t));
    GETSOCKOPT(RTDRV_L2_IP6_MCAST_ADDR_GETNEXT, &ip6_data, rtdrv_l2_ip6McstAddrData_t, 1);
    *pScan_idx = ip6_data.index;
    osal_memcpy(pL2_data, &ip6_data.ip6_m_data, sizeof(rtk_l2_ip6McastAddr_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portmask.unit = unit;
    config.l2_portmask.index = *pFwdIndex;
    GETSOCKOPT(RTDRV_L2_MCAST_FWD_INDEX_ALLOC, &config, rtdrv_l2Cfg_t, 1);
    *pFwdIndex = config.l2_portmask.index;

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portmask.unit = unit;
    config.l2_portmask.index = index;
    SETSOCKOPT(RTDRV_L2_MCAST_FWD_INDEX_FREE, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastFwdIndexFreeCount_get(uint32 unit, uint32 *pFreeCount)
{
    rtdrv_l2_fwdTblEntry_t  entryContent;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&entryContent, 0, sizeof(rtdrv_l2_fwdTblEntry_t));

    entryContent.unit = unit;
    GETSOCKOPT(RTDRV_L2_MCASTFWDINDEXFREECOUNT_GET, &entryContent, rtdrv_l2_fwdTblEntry_t, 1);
    *pFreeCount = entryContent.freeCount;

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_mcastFwdPortmask_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          *pCrossVlan)
{
    rtdrv_l2_fwdTblEntry_t  entryContent;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&entryContent, 0, sizeof(rtdrv_l2_fwdTblEntry_t));

    entryContent.unit = unit;
    entryContent.entryIdx = index;
    GETSOCKOPT(RTDRV_L2_MCAST_FWD_PORTMASK_GET, &entryContent, rtdrv_l2_fwdTblEntry_t, 1);
    osal_memcpy(pPortmask, &entryContent.portMask, sizeof(rtk_portmask_t));
    *pCrossVlan = entryContent.crossVlan;

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastFwdPortmask_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          crossVlan)
{
    rtdrv_l2_fwdTblEntry_t  entryContent;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&entryContent, 0, sizeof(rtdrv_l2_fwdTblEntry_t));

    entryContent.unit = unit;
    entryContent.entryIdx = index;
    entryContent.crossVlan = crossVlan;
    osal_memcpy(&entryContent.portMask, pPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_MCAST_FWD_PORTMASK_SET, &entryContent, rtdrv_l2_fwdTblEntry_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portmask.unit = unit;
    config.l2_portmask.index= index;
    GETSOCKOPT(RTDRV_L2_MCAST_FWD_PORTMASK_ENTRY_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pPortmask, &config.l2_portmask.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portmask.unit = unit;
    config.l2_portmask.index= index;
    osal_memcpy(&config.l2_portmask.portmask, pPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_MCAST_FWD_PORTMASK_ENTRY_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    config.l2_ucAddr.vid= vid;
    osal_memcpy(&config.l2_ucAddr.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L2_CPU_MAC_ADDR_ADD, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    config.l2_ucAddr.vid= vid;
    osal_memcpy(&config.l2_ucAddr.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L2_CPU_MAC_ADDR_DEL, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
int32
rtrpc_l2_legalPortMoveAction_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    rtdrv_l2_portAct_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_portAct_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_LEGAL_MOVETO_ACTION_GET, &l2_learn, rtdrv_l2_portAct_t, 1);
    *pFwdAction = l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_legalPortMoveAction_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    rtdrv_l2_portAct_t l2_action;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_action, 0, sizeof(rtdrv_l2_portAct_t));
    l2_action.unit = unit;
    l2_action.port = port;
    l2_action.action = fwdAction;
    SETSOCKOPT(RTDRV_L2_PORT_LEGAL_MOVETO_ACTION_SET, &l2_action, rtdrv_l2_portAct_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_dynamicPortMoveForbidAction_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_l2_common_t common;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&common, 0, sizeof(rtdrv_l2_common_t));
    common.unit = unit;
    GETSOCKOPT(RTDRV_L2_DYNM_PORTMOVE_FORBID_ACTION_GET, &common, rtdrv_l2_common_t, 1);
    *pAction    = common.value;

    return RT_ERR_OK;
}

int32
rtrpc_l2_dynamicPortMoveForbidAction_set(uint32 unit, rtk_action_t action)
{
    rtdrv_l2_common_t common;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&common, 0, sizeof(rtdrv_l2_common_t));
    common.unit     = unit;
    common.value    = action;
    SETSOCKOPT(RTDRV_L2_DYNM_PORTMOVE_FORBID_ACTION_SET, &common, rtdrv_l2_common_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_l2_portMoveAction_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.type = type;
    osal_memcpy(&config.l2_portMove.action, pAction, sizeof(rtk_l2_portMoveAct_t));
    GETSOCKOPT(RTDRV_L2_PORT_MOVE_ACT_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pAction, &config.l2_portMove.action, sizeof(rtk_l2_portMoveAct_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.type = type;
    osal_memcpy(&config.l2_portMove.action, pAction, sizeof(rtk_l2_portMoveAct_t));
    SETSOCKOPT(RTDRV_L2_PORT_MOVE_ACT_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}
int32
rtrpc_l2_portMoveLearn_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.type = type;
    osal_memcpy(&config.l2_portMove.learn, pLearn, sizeof(rtk_l2_portMoveLrn_t));
    GETSOCKOPT(RTDRV_L2_PORT_MOVE_LEARN_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLearn, &config.l2_portMove.learn, sizeof(rtk_l2_portMoveLrn_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_portMoveLearn_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.type = type;
    osal_memcpy(&config.l2_portMove.learn, pLearn, sizeof(rtk_l2_portMoveLrn_t));
    SETSOCKOPT(RTDRV_L2_PORT_MOVE_LEARN_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_legalPortMoveFlushAddrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_l2_common_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_l2_common_t));
    cfg.unit = unit;
    cfg.port = port;
    GETSOCKOPT(RTDRV_L2_LEGAL_MOVETO_FLUSH_ENABLE_GET, &cfg, rtdrv_l2_common_t, 1);
    *pEnable = cfg.value;

    return RT_ERR_OK;
}

int32
rtrpc_l2_legalPortMoveFlushAddrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_l2_common_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_l2_common_t));
    cfg.unit = unit;
    cfg.port = port;
    cfg.value = enable;
    SETSOCKOPT(RTDRV_L2_LEGAL_MOVETO_FLUSH_ENABLE_SET, &cfg, rtdrv_l2_common_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_staticPortMoveAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pFwdAction)
{
    rtdrv_l2_portAct_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_portAct_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    GETSOCKOPT(RTDRV_L2_STTC_PORT_MOVE_ACTION_GET, &l2_learn, rtdrv_l2_portAct_t, 1);
    *pFwdAction = l2_learn.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_staticPortMoveAction_set(uint32 unit, rtk_port_t port, rtk_action_t fwdAction)
{
    rtdrv_l2_portAct_t l2_learn;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_learn, 0, sizeof(rtdrv_l2_portAct_t));
    l2_learn.unit = unit;
    l2_learn.port = port;
    l2_learn.action = fwdAction;
    SETSOCKOPT(RTDRV_L2_STTC_PORT_MOVE_ACTION_SET, &l2_learn, rtdrv_l2_portAct_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_lookUpMiss.unit = unit;
    config.l2_lookUpMiss.type = type;
    GETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFlood_portmask, &config.l2_lookUpMiss.flood_portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMask_set(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_lookUpMiss.unit = unit;
    config.l2_lookUpMiss.type = type;
    osal_memcpy(&config.l2_lookUpMiss.flood_portmask, pFlood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_lookUpMiss.unit = unit;
    config.l2_lookUpMiss.type = type;
    config.l2_lookUpMiss.port = flood_port;
    SETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_ADD, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_lookUpMiss.unit = unit;
    config.l2_lookUpMiss.type = type;
    config.l2_lookUpMiss.port = flood_port;
    SETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_DEL, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMask_setByIndex(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx, rtk_portmask_t *pFlood_portmask)
{
    rtdrv_l2_lkMiss_t l2_lkmiss;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_lkmiss, 0, sizeof(rtdrv_l2_lkMiss_t));
    l2_lkmiss.unit = unit;
    l2_lkmiss.type = type;
    l2_lkmiss.index = idx;
    l2_lkmiss.portMask = *pFlood_portmask;
    SETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOOD_PMSK_SET_WITH_IDX, &l2_lkmiss, rtdrv_l2_lkMiss_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMaskIdx_get(uint32 unit, rtk_l2_lookupMissType_t type, uint32 *pIdx)
{
    rtdrv_l2_lkMiss_t l2_lkmiss;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_lkmiss, 0, sizeof(rtdrv_l2_lkMiss_t));
    l2_lkmiss.unit = unit;
    l2_lkmiss.type = type;
    GETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOODPORTMASK_IDX_GET, &l2_lkmiss, rtdrv_l2_lkMiss_t, 1);
    *pIdx = l2_lkmiss.index;

    return RT_ERR_OK;
}

int32
rtrpc_l2_lookupMissFloodPortMaskIdx_set(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx)
{
    rtdrv_l2_lkMiss_t l2_lkmiss;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_lkmiss, 0, sizeof(rtdrv_l2_lkMiss_t));
    l2_lkmiss.unit = unit;
    l2_lkmiss.type = type;
    l2_lkmiss.index = idx;
    SETSOCKOPT(RTDRV_L2_LOOKUP_MISS_FLOODPORTMASK_IDX_SET, &l2_lkmiss, rtdrv_l2_lkMiss_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    rtdrv_l2_lkMiss_t l2_action;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_action, 0, sizeof(rtdrv_l2_lkMiss_t));
    l2_action.unit = unit;
    l2_action.port = port;
    l2_action.type = type;
    GETSOCKOPT(RTDRV_L2_PORT_LOOKUP_MISS_ACTION_GET, &l2_action, rtdrv_l2_lkMiss_t, 1);
    *pAction = l2_action.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    rtdrv_l2_lkMiss_t l2_action;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_action, 0, sizeof(rtdrv_l2_lkMiss_t));
    l2_action.unit = unit;
    l2_action.port = port;
    l2_action.type = type;
    l2_action.action = action;
    SETSOCKOPT(RTDRV_L2_PORT_LOOKUP_MISS_ACTION_SET, &l2_action, rtdrv_l2_lkMiss_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_lookUpMiss.unit = unit;
    config.l2_lookUpMiss.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_UCAST_LOOKUP_MISS_ACTION_GET, &config, rtdrv_l2Cfg_t, 1);
    *pAction = config.l2_lookUpMiss.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_lookUpMiss.unit = unit;
    config.l2_lookUpMiss.port = port;
    config.l2_lookUpMiss.action = action;
    SETSOCKOPT(RTDRV_L2_PORT_UCAST_LOOKUP_MISS_ACTION_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    GETSOCKOPT(RTDRV_L2_SRC_PORT_EGR_FILTER_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFilter_portmask, &config.l2_common.srcPortFilterPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    osal_memcpy(&config.l2_common.srcPortFilterPortmask, pFilter_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_SRC_PORT_EGR_FILTER_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.port = filter_port;
    SETSOCKOPT(RTDRV_L2_SRC_PORT_EGR_FILTER_ADD, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.port = filter_port;
    SETSOCKOPT(RTDRV_L2_SRC_PORT_EGR_FILTER_DEL, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_exceptionAddrAction_get(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    *pAction)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.exceptType = exceptType;
    GETSOCKOPT(RTDRV_L2_EXCEPTION_ADDR_ACTION_GET, &config, rtdrv_l2Cfg_t, 1);
    *pAction = config.l2_common.action;

    return RT_ERR_OK;
}

int32
rtrpc_l2_exceptionAddrAction_set(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    action)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.exceptType = exceptType;
    config.l2_common.action = action;
    SETSOCKOPT(RTDRV_L2_EXCEPTION_ADDR_ACTION_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry)
{
    rtdrv_l2Cfg_search_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_search_t));
    config.unit = unit;
    config.index = index;
    GETSOCKOPT(RTDRV_L2_ADDR_ENTRY_GET, &config, rtdrv_l2Cfg_search_t, 1);
    osal_memcpy(pL2_entry, &config.l2_entry, sizeof(rtk_l2_entry_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_conflictAddr_get(
    uint32          unit,
    rtk_l2_entry_t  *pL2Addr,
    rtk_l2_entry_t  *pCfAddrList,
    uint32          cfAddrList_size,
    uint32          *pCf_retCnt)
{
    rtdrv_l2Cfg_search_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_search_t));
    config.unit = unit;
    osal_memcpy(&config.l2_entry, pL2Addr, sizeof(rtk_l2_entry_t));
    config.cfAddrList_size = cfAddrList_size;
    GETSOCKOPT(RTDRV_L2_CONFLICT_ADDR_GET, &config, rtdrv_l2Cfg_search_t, 1);
    osal_memcpy(pCfAddrList, config.cfAddrList, (sizeof(rtk_l2_entry_t) * cfAddrList_size));
    (*pCf_retCnt) = config.cf_retCnt;

    return RT_ERR_OK;
}


int32
rtrpc_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_ZERO_SA_LEARNING_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_learn.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    config.l2_learn.enable = enable;
    SETSOCKOPT(RTDRV_L2_ZERO_SA_LEARNING_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_secureMacMode_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2_common_t l2_common;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_common, 0, sizeof(rtdrv_l2_common_t));
    l2_common.unit = unit;
    GETSOCKOPT(RTDRV_L2_SECURE_MAC_MODE_GET, &l2_common, rtdrv_l2_common_t, 1);
    *pEnable = l2_common.value;

    return RT_ERR_OK;
}

int32
rtrpc_l2_secureMacMode_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2_common_t l2_common;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_common, 0, sizeof(rtdrv_l2_common_t));
    l2_common.unit = unit;
    l2_common.value = enable;
    SETSOCKOPT(RTDRV_L2_SECURE_MAC_MODE_SET, &l2_common, rtdrv_l2_common_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.port = port;
    GETSOCKOPT(RTDRV_L2_PORT_DYNM_PORTMOVE_FORBID_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_portMove.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.port = port;
    config.l2_portMove.enable = enable;
    SETSOCKOPT(RTDRV_L2_PORT_DYNM_PORTMOVE_FORBID_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_l2_trkDynamicPortMoveForbidEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.trunk= tid;
    GETSOCKOPT(RTDRV_L2_TRK_DYNM_PORTMOVE_FORBID_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_portMove.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_trkDynamicPortMoveForbidEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_portMove.unit = unit;
    config.l2_portMove.trunk = tid;
    config.l2_portMove.enable = enable;
    SETSOCKOPT(RTDRV_L2_TRK_DYNM_PORTMOVE_FORBID_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.port= port;
    config.l2_common.filterMode = filterMode;
    GETSOCKOPT(RTDRV_L2_PORT_MAC_FILTER_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_common.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.port= port;
    config.l2_common.filterMode = filterMode;
    config.l2_common.enable = enable;
    SETSOCKOPT(RTDRV_L2_PORT_MAC_FILTER_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_hwNextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_nextValidType_t type,
    rtk_l2_entry_t  *pEntry)
{
    rtdrv_l2Cfg_search_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_search_t));
    config.unit     = unit;
    config.scan_idx = *pScan_idx;
    config.type     = type;
    GETSOCKOPT(RTDRV_L2_HW_NEXT_VALID_ADDR_GET, &config, rtdrv_l2Cfg_search_t, 1);
    osal_memcpy(pEntry, &config.l2_entry, sizeof(rtk_l2_entry_t));
    *pScan_idx = config.scan_idx;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 *pArg)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.port= port;
    config.l2_common.portCtrlType = type;
    GETSOCKOPT(RTDRV_L2_PORT_CTRL_TYPE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pArg = config.l2_common.arg;

    return RT_ERR_OK;
}

int32
rtrpc_l2_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 arg)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.port= port;
    config.l2_common.portCtrlType = type;
    config.l2_common.arg =arg;
    SETSOCKOPT(RTDRV_L2_PORT_CTRL_TYPE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_status_get(uint32 unit, rtk_l2_stsType_t type, uint32 index, uint32 *pArg)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit       = unit;
    config.l2_common.stsType    = type;
    config.l2_common.port       = index;
    GETSOCKOPT(RTDRV_L2_STATUS_TYPE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pArg = config.l2_common.value;

    return RT_ERR_OK;
}

int32
rtrpc_l2_stkLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    GETSOCKOPT(RTDRV_L2_STK_LEARNING_ENABLE_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_learn.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_stkLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_learn.unit = unit;
    config.l2_learn.enable = enable;
    SETSOCKOPT(RTDRV_L2_STK_LEARNING_ENABLE_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_stkKeepUcastEntryValid_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    GETSOCKOPT(RTDRV_L2_STK_KEEP_AGE_VALID_GET, &config, rtdrv_l2Cfg_t, 1);
    *pEnable = config.l2_age.enable;

    return RT_ERR_OK;
}

int32
rtrpc_l2_stkKeepUcastEntryValid_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_age.unit = unit;
    config.l2_age.enable = enable;
    SETSOCKOPT(RTDRV_L2_STK_KEEP_AGE_VALID_SET, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2_entryCnt_get(uint32 unit, rtk_l2_entryType_t type, uint32 *pCnt)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.entryType = type;
    GETSOCKOPT(RTDRV_L2_ENTRY_CNT_GET, &config, rtdrv_l2Cfg_t, 1);
    *pCnt = config.l2_common.value;

    return RT_ERR_OK;
}/* end of rtk_l2_entryCnt_get */

int32
rtrpc_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_common.unit = unit;
    config.l2_common.macHashIdx.vid = pMacHashIdx->vid;
    osal_memcpy(&config.l2_common.macHashIdx.mac, &pMacHashIdx->mac, sizeof(rtk_mac_t));
    GETSOCKOPT(RTDRV_L2_MAC_HASHIDX_GET, &config, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMacHashIdx, &config.l2_common.macHashIdx, sizeof(rtk_l2_macHashIdx_t));

    return RT_ERR_OK;
}

int32
rtrpc_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac)
{
    rtdrv_l2Cfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_l2Cfg_t));
    config.l2_ucAddr.unit = unit;
    osal_memcpy(&config.l2_ucAddr.mac, pMac, sizeof(rtk_mac_t));
    config.l2_ucAddr.include_static = include_static;
    SETSOCKOPT(RTDRV_L2_ADDR_DELBYMAC, &config, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_l2Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l2_flushLinkDownPortAddrEnable_get = rtrpc_l2_flushLinkDownPortAddrEnable_get;
    pMapper->l2_flushLinkDownPortAddrEnable_set = rtrpc_l2_flushLinkDownPortAddrEnable_set;
    pMapper->l2_ucastAddr_flush = rtrpc_l2_ucastAddr_flush;
    pMapper->l2_macLearningCnt_get = rtrpc_l2_macLearningCnt_get;
    pMapper->l2_limitLearningNum_get = rtrpc_l2_limitLearningNum_get;
    pMapper->l2_limitLearningNum_set = rtrpc_l2_limitLearningNum_set;
    pMapper->l2_limitLearningAction_get = rtrpc_l2_limitLearningAction_get;
    pMapper->l2_limitLearningAction_set = rtrpc_l2_limitLearningAction_set;
    pMapper->l2_fidLimitLearningEntry_get = rtrpc_l2_fidLimitLearningEntry_get;
    pMapper->l2_fidLimitLearningEntry_set = rtrpc_l2_fidLimitLearningEntry_set;
    pMapper->l2_fidLearningCnt_reset = rtrpc_l2_fidLearningCnt_reset;
    pMapper->l2_agingTime_get = rtrpc_l2_agingTime_get;
    pMapper->l2_agingTime_set = rtrpc_l2_agingTime_set;
    pMapper->l2_portAgingEnable_get = rtrpc_l2_portAgingEnable_get;
    pMapper->l2_portAgingEnable_set = rtrpc_l2_portAgingEnable_set;
    pMapper->l2_trkAgingEnable_get = rtrpc_l2_trkAgingEnable_get;
    pMapper->l2_trkAgingEnable_set = rtrpc_l2_trkAgingEnable_set;
    pMapper->l2_bucketHashAlgo_get = rtrpc_l2_bucketHashAlgo_get;
    pMapper->l2_bucketHashAlgo_set = rtrpc_l2_bucketHashAlgo_set;
    pMapper->l2_vlanMode_get = rtrpc_l2_vlanMode_get;
    pMapper->l2_vlanMode_set = rtrpc_l2_vlanMode_set;
    pMapper->l2_learningFullAction_get = rtrpc_l2_learningFullAction_get;
    pMapper->l2_learningFullAction_set = rtrpc_l2_learningFullAction_set;
    pMapper->l2_portNewMacOp_get = rtrpc_l2_portNewMacOp_get;
    pMapper->l2_portNewMacOp_set = rtrpc_l2_portNewMacOp_set;
    pMapper->l2_addr_init = rtrpc_l2_addr_init;
    pMapper->l2_addr_add = rtrpc_l2_addr_add;
    pMapper->l2_addr_del = rtrpc_l2_addr_del;
    pMapper->l2_addr_get = rtrpc_l2_addr_get;
    pMapper->l2_addr_set = rtrpc_l2_addr_set;
    pMapper->l2_addr_delAll = rtrpc_l2_addr_delAll;
    pMapper->l2_nextValidAddr_get = rtrpc_l2_nextValidAddr_get;
    pMapper->l2_mcastAddr_init = rtrpc_l2_mcastAddr_init;
    pMapper->l2_mcastAddr_add = rtrpc_l2_mcastAddr_add;
    pMapper->l2_mcastAddr_del = rtrpc_l2_mcastAddr_del;
    pMapper->l2_mcastAddr_get = rtrpc_l2_mcastAddr_get;
    pMapper->l2_mcastAddr_set = rtrpc_l2_mcastAddr_set;
    pMapper->l2_mcastAddr_addByIndex = rtrpc_l2_mcastAddr_addByIndex;
    pMapper->l2_mcastAddr_setByIndex= rtrpc_l2_mcastAddr_setByIndex;
    pMapper->l2_mcastAddr_delIgnoreIndex= rtrpc_l2_mcastAddr_delIgnoreIndex;
    pMapper->l2_nextValidMcastAddr_get = rtrpc_l2_nextValidMcastAddr_get;
    pMapper->l2_ipmcMode_get = rtrpc_l2_ipmcMode_get;
    pMapper->l2_ipmcMode_set = rtrpc_l2_ipmcMode_set;
    pMapper->l2_ipMcastAddrExt_init = rtrpc_l2_ipMcastAddrExt_init;
    pMapper->l2_ipMcastAddr_add = rtrpc_l2_ipMcastAddr_add;
    pMapper->l2_ipMcastAddr_del = rtrpc_l2_ipMcastAddr_del;
    pMapper->l2_ipMcastAddr_get = rtrpc_l2_ipMcastAddr_get;
    pMapper->l2_ipMcastAddr_set = rtrpc_l2_ipMcastAddr_set;
    pMapper->l2_ipMcastAddr_addByIndex = rtrpc_l2_ipMcastAddr_addByIndex;
    pMapper->l2_ipMcastAddr_setByIndex= rtrpc_l2_ipMcastAddr_setByIndex;
    pMapper->l2_ipMcastAddr_delIgnoreIndex= rtrpc_l2_ipMcastAddr_delIgnoreIndex;
    pMapper->l2_nextValidIpMcastAddr_get = rtrpc_l2_nextValidIpMcastAddr_get;
    pMapper->l2_ipMcastAddrChkEnable_get = rtrpc_l2_ipMcastAddrChkEnable_get;
    pMapper->l2_ipMcastAddrChkEnable_set = rtrpc_l2_ipMcastAddrChkEnable_set;
    pMapper->l2_ipMcstFidVidCompareEnable_get = rtrpc_l2_ipMcstFidVidCompareEnable_get;
    pMapper->l2_ipMcstFidVidCompareEnable_set = rtrpc_l2_ipMcstFidVidCompareEnable_set;
    pMapper->l2_ip6mcMode_get = rtrpc_l2_ip6mcMode_get;
    pMapper->l2_ip6mcMode_set = rtrpc_l2_ip6mcMode_set;
    pMapper->l2_ip6CareByte_get = rtrpc_l2_ip6CareByte_get;
    pMapper->l2_ip6CareByte_set = rtrpc_l2_ip6CareByte_set;
    pMapper->l2_ip6McastAddrExt_init = rtrpc_l2_ip6McastAddrExt_init;
    pMapper->l2_ip6McastAddr_add = rtrpc_l2_ip6McastAddr_add;
    pMapper->l2_ip6McastAddr_del = rtrpc_l2_ip6McastAddr_del;
    pMapper->l2_ip6McastAddr_get = rtrpc_l2_ip6McastAddr_get;
    pMapper->l2_ip6McastAddr_set = rtrpc_l2_ip6McastAddr_set;
    pMapper->l2_ip6McastAddr_addByIndex = rtrpc_l2_ip6McastAddr_addByIndex;
    pMapper->l2_ip6McastAddr_setByIndex = rtrpc_l2_ip6McastAddr_setByIndex;
    pMapper->l2_ip6McastAddr_delIgnoreIndex = rtrpc_l2_ip6McastAddr_delIgnoreIndex;
    pMapper->l2_nextValidIp6McastAddr_get = rtrpc_l2_nextValidIp6McastAddr_get;
    pMapper->l2_mcastFwdIndex_alloc = rtrpc_l2_mcastFwdIndex_alloc;
    pMapper->l2_mcastFwdIndex_free = rtrpc_l2_mcastFwdIndex_free;
    pMapper->l2_mcastFwdIndexFreeCount_get = rtrpc_l2_mcastFwdIndexFreeCount_get;
    pMapper->l2_mcastFwdPortmaskEntry_get = rtrpc_l2_mcastFwdPortmaskEntry_get;
    pMapper->l2_mcastFwdPortmaskEntry_set = rtrpc_l2_mcastFwdPortmaskEntry_set;
    pMapper->l2_cpuMacAddr_add = rtrpc_l2_cpuMacAddr_add;
    pMapper->l2_cpuMacAddr_del = rtrpc_l2_cpuMacAddr_del;
    pMapper->l2_portMoveAction_get = rtrpc_l2_portMoveAction_get;
    pMapper->l2_portMoveAction_set = rtrpc_l2_portMoveAction_set;
    pMapper->l2_portMoveLearn_get = rtrpc_l2_portMoveLearn_get;
    pMapper->l2_portMoveLearn_set = rtrpc_l2_portMoveLearn_set;
    pMapper->l2_legalPortMoveFlushAddrEnable_get = rtrpc_l2_legalPortMoveFlushAddrEnable_get;
    pMapper->l2_legalPortMoveFlushAddrEnable_set = rtrpc_l2_legalPortMoveFlushAddrEnable_set;
    pMapper->l2_staticPortMoveAction_get = rtrpc_l2_staticPortMoveAction_get;
    pMapper->l2_staticPortMoveAction_set = rtrpc_l2_staticPortMoveAction_set;
    pMapper->l2_lookupMissFloodPortMask_get = rtrpc_l2_lookupMissFloodPortMask_get;
    pMapper->l2_lookupMissFloodPortMask_set = rtrpc_l2_lookupMissFloodPortMask_set;
    pMapper->l2_lookupMissFloodPortMask_add = rtrpc_l2_lookupMissFloodPortMask_add;
    pMapper->l2_lookupMissFloodPortMask_del = rtrpc_l2_lookupMissFloodPortMask_del;
    pMapper->l2_lookupMissFloodPortMask_setByIndex = rtrpc_l2_lookupMissFloodPortMask_setByIndex;
    pMapper->l2_lookupMissFloodPortMaskIdx_get = rtrpc_l2_lookupMissFloodPortMaskIdx_get;
    pMapper->l2_lookupMissFloodPortMaskIdx_set = rtrpc_l2_lookupMissFloodPortMaskIdx_set;
    pMapper->l2_portLookupMissAction_get = rtrpc_l2_portLookupMissAction_get;
    pMapper->l2_portLookupMissAction_set = rtrpc_l2_portLookupMissAction_set;
    pMapper->l2_portUcastLookupMissAction_get = rtrpc_l2_portUcastLookupMissAction_get;
    pMapper->l2_portUcastLookupMissAction_set = rtrpc_l2_portUcastLookupMissAction_set;
    pMapper->l2_srcPortEgrFilterMask_get = rtrpc_l2_srcPortEgrFilterMask_get;
    pMapper->l2_srcPortEgrFilterMask_set = rtrpc_l2_srcPortEgrFilterMask_set;
    pMapper->l2_srcPortEgrFilterMask_add = rtrpc_l2_srcPortEgrFilterMask_add;
    pMapper->l2_srcPortEgrFilterMask_del = rtrpc_l2_srcPortEgrFilterMask_del;
    pMapper->l2_exceptionAddrAction_get = rtrpc_l2_exceptionAddrAction_get;
    pMapper->l2_exceptionAddrAction_set = rtrpc_l2_exceptionAddrAction_set;
    pMapper->l2_addrEntry_get = rtrpc_l2_addrEntry_get;
    pMapper->l2_conflictAddr_get = rtrpc_l2_conflictAddr_get;
    pMapper->l2_zeroSALearningEnable_get = rtrpc_l2_zeroSALearningEnable_get;
    pMapper->l2_zeroSALearningEnable_set = rtrpc_l2_zeroSALearningEnable_set;
    pMapper->l2_secureMacMode_get = rtrpc_l2_secureMacMode_get;
    pMapper->l2_secureMacMode_set = rtrpc_l2_secureMacMode_set;
    pMapper->l2_portDynamicPortMoveForbidEnable_get = rtrpc_l2_portDynamicPortMoveForbidEnable_get;
    pMapper->l2_portDynamicPortMoveForbidEnable_set = rtrpc_l2_portDynamicPortMoveForbidEnable_set;
    pMapper->l2_trkDynamicPortMoveForbidEnable_get = rtrpc_l2_trkDynamicPortMoveForbidEnable_get;
    pMapper->l2_trkDynamicPortMoveForbidEnable_set = rtrpc_l2_trkDynamicPortMoveForbidEnable_set;
    pMapper->l2_portMacFilterEnable_get = rtrpc_l2_portMacFilterEnable_get;
    pMapper->l2_portMacFilterEnable_set = rtrpc_l2_portMacFilterEnable_set;
    pMapper->l2_hwNextValidAddr_get = rtrpc_l2_hwNextValidAddr_get;
    pMapper->l2_portCtrl_get = rtrpc_l2_portCtrl_get;
    pMapper->l2_portCtrl_set = rtrpc_l2_portCtrl_set;
    pMapper->l2_status_get = rtrpc_l2_status_get;
    pMapper->l2_stkLearningEnable_get = rtrpc_l2_stkLearningEnable_get;
    pMapper->l2_stkLearningEnable_set = rtrpc_l2_stkLearningEnable_set;
    pMapper->l2_stkKeepUcastEntryValid_get = rtrpc_l2_stkKeepUcastEntryValid_get;
    pMapper->l2_stkKeepUcastEntryValid_set = rtrpc_l2_stkKeepUcastEntryValid_set;
    pMapper->l2_entryCnt_get = rtrpc_l2_entryCnt_get;
    pMapper->l2_hashIdx_get = rtrpc_l2_hashIdx_get;
    pMapper->l2_addr_delByMac = rtrpc_l2_addr_delByMac;
    return RT_ERR_OK;
}
