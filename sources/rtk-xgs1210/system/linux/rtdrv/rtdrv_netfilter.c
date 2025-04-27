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
 * $Revision: 98576 $
 * $Date: 2019-07-11 14:45:28 +0800 (Thu, 11 Jul 2019) $
 *
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Module.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Module
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>

#include <asm/uaccess.h>
#include <linux/netfilter.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
#include <linux/module.h>
#endif
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/mem.h>
#include <osal/print.h>
#include <hal/mac/mem.h>
#include <ioal/mem32.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/watchdog/watchdog.h>
#include <private/drv/rtl8231/rtl8231.h>
#include <drv/gpio/ext_gpio.h>
#include <hwp/hw_profile.h>
#include <hwp/hwp_util.h>
#if defined(CONFIG_SDK_UART1)
#include <drv/uart/uart.h>
#endif
#include <rtdrv/rtdrv_netfilter.h>
#if (defined(CONFIG_SDK_DRIVER_TEST) || defined(CONFIG_SDK_DRIVER_TEST_MODULE))
#include <sdk/sdk_test.h>
#include <common/unittest_util.h>
#endif
#include <drv/gpio/gpio.h>
#if defined(CONFIG_SDK_DRIVER_I2C)
#include <drv/i2c/i2c.h>
#endif
#if defined(CONFIG_SDK_DRIVER_SPI)
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
#endif
#include <dal/dal_phy.h>
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
#include <hal/mac/rtl8295.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_rtl8295_patch.h>
#include <hal/phy/phy_rtl8295.h>
#endif/* defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)  */
#include <osal/memory.h>
#include <hwp/hw_profile.h>
#include <hal/mac/serdes.h>

#if defined(CONFIG_SDK_RTL8390)
#include <hal/phy/phy_rtl8390.h>
#endif
#include <dal/rtrpc/rtrpc_msg.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern struct nf_sockopt_ops rtdrv_sockopts;

#if defined(CONFIG_SDK_APP_DIAG_EXT)
extern struct nf_sockopt_ops rtdrv_ext_sockopts;
#endif

static rtdrv_module_t rtdrv_module_db_set[RTDRV_MODULE_MAX_NUM];
static rtdrv_module_t rtdrv_module_db_get[RTDRV_MODULE_MAX_NUM];
rtdrv_test_module_t sdkTest_drv = {
    .mode_get = NULL,
    .mode_set = NULL,
    .run = NULL,
    .run_id = NULL,
};


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

int32 rtdrv_cmdFunc_register(rtdrv_setGet_t access, uint32 offset, int32 (*func)(int, void *, rtdrv_union_t *))
{
    int module_index = offset >> RTDRV_MODULE_SHIFT;

    if ((module_index < 0) || (module_index > RTDRV_MODULE_MAX_NUM))
        return RT_ERR_FAILED;

    if (NULL == func)
        return RT_ERR_OK;

    if (RTDRV_SET == access)
    {
        if (TRUE == rtdrv_module_db_set[module_index].valid)
            return RT_ERR_FAILED;

        rtdrv_module_db_set[module_index].valid  = TRUE;
        rtdrv_module_db_set[module_index].offset = offset;
        rtdrv_module_db_set[module_index].func   = func;
    }
    else
    {
        if (TRUE == rtdrv_module_db_get[module_index].valid)
            return RT_ERR_FAILED;

        rtdrv_module_db_get[module_index].valid  = TRUE;
        rtdrv_module_db_get[module_index].offset = offset;
        rtdrv_module_db_get[module_index].func   = func;
    }
    return RT_ERR_OK;
}
EXPORT_SYMBOL(rtdrv_cmdFunc_register);
EXPORT_SYMBOL(sdkTest_drv);

int32 do_rtdrv_set_ctl_init(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** INIT **/
        case RTDRV_INIT_RTKAPI:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_init();

        default:
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_l2(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* L2 */
        case RTDRV_L2_INIT:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_init(buf->l2_cfg.l2_common.unit);
            break;

        case RTDRV_L2_FLUSH_LINK_DOWN_PORT_ADDR_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_flushLinkDownPortAddrEnable_set(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.enable);
            break;

        case RTDRV_L2_UCASTADDR_FLUSH:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ucastAddr_flush(buf->l2_cfg.l2_flush.unit, &buf->l2_cfg.l2_flush.flush);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_LIMIT_LEARNING_CNT_SET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_limitLearningCnt_set(buf->l2_learn.unit, buf->l2_learn.mac_cnt);
            break;

        case RTDRV_L2_PORT_LIMIT_LEARNING_CNT_SET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_portLimitLearningCnt_set(buf->l2_learn.unit, buf->l2_learn.port, buf->l2_learn.mac_cnt);
            break;
#endif
        case RTDRV_L2_LIMIT_LEARNING_NUM_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningNum_set(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.type, &buf->l2_cfg.l2_macLimit.cnt);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_LIMIT_LEARNING_CNT_ACT_SET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_limitLearningCntAction_set(buf->l2_learn.unit, buf->l2_learn.action);
            break;

        case RTDRV_L2_PORT_LIMIT_LEARNING_CNT_ACT_SET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_portLimitLearningCntAction_set(buf->l2_learn.unit, buf->l2_learn.port, buf->l2_learn.action);
            break;

        case RTDRV_L2_FID_LIMIT_LEARNING_CNT_ACT_SET:
            rtk_copy_from(&buf->l2_FidLearn, user, sizeof(rtdrv_l2_learnFidCnt_t));
            ret = rtk_l2_fidLearningCntAction_set(buf->l2_FidLearn.unit, buf->l2_FidLearn.action);
            break;
#endif
        case RTDRV_L2_LIMIT_LEARNING_ACT_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningAction_set(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.type, &buf->l2_cfg.l2_macLimit.action);
            break;

        case RTDRV_L2_FID_LIMIT_LEARNING_ENTRY_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_fidLimitLearningEntry_set(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.fidLimitEntryId, &buf->l2_cfg.l2_macLimit.entry);
            break;

        case RTDRV_L2_FID_LEARNING_CNT_RESET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_fidLearningCnt_reset(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.fidLimitEntryId);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_AGING_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_l2_aging_set(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;

#endif
        case RTDRV_L2_AGING_TIME_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_agingTime_set(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.type, buf->l2_cfg.l2_age.ageTime);;
            break;

        case RTDRV_L2_PORT_AGING_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portAgingEnable_set(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.port, buf->l2_cfg.l2_age.enable);
            break;

        case RTDRV_L2_TRK_AGING_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_trkAgingEnable_set(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.trunk, buf->l2_cfg.l2_age.enable);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_HASH_ALGO_SET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_hashAlgo_set(buf->l2_learnCfg.unit, buf->l2_learnCfg.hash_algo);
            break;
#endif
        case RTDRV_L2_BUCKET_HASH_ALGO_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_bucketHashAlgo_set(buf->l2_cfg.l2_hash.unit, buf->l2_cfg.l2_hash.bucket, buf->l2_cfg.l2_hash.hashAlgo);
            break;

        case RTDRV_L2_VLANMODE_SET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_vlanMode_set(buf->l2_learnCfg.unit, buf->l2_learnCfg.port, buf->l2_learnCfg.vlanMode);
            break;

        case RTDRV_L2_LEARNING_FULL_ACT_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_learningFullAction_set(buf->l2_cfg.l2_learn.unit, buf->l2_cfg.l2_learn.action);
            break;

        case RTDRV_L2_PORT_NEW_MAC_OP_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portNewMacOp_set(buf->l2_cfg.l2_learn.unit, buf->l2_cfg.l2_learn.port, buf->l2_cfg.l2_learn.lrnMode, buf->l2_cfg.l2_learn.action);
            break;

        case RTDRV_L2_ADDR_DEL:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_del(buf->l2_cfg.l2_ucAddr.unit, buf->l2_cfg.l2_ucAddr.vid, &buf->l2_cfg.l2_ucAddr.mac);
            break;

        case RTDRV_L2_ADDR_DEL_ALL:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_delAll(buf->l2_cfg.l2_ucAddr.unit, buf->l2_cfg.l2_ucAddr.include_static);
            break;

        case RTDRV_L2_MCAST_ADDR_DEL:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_del(buf->l2_cfg.l2_mcAddr.unit, buf->l2_cfg.l2_mcAddr.vid, &buf->l2_cfg.l2_mcAddr.mac);
            break;

        case RTDRV_L2_MCAST_ADDR_DEL_IGNORE_INDEX:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_delIgnoreIndex(buf->l2_cfg.l2_mcAddr.unit, buf->l2_cfg.l2_mcAddr.vid, &buf->l2_cfg.l2_mcAddr.mac);
            break;

        case RTDRV_L2_MCAST_ADDR_ADDBYINDEX:
            rtk_copy_from(&buf->mcast_data, user, sizeof(rtdrv_l2_mcastAddrData_t));
            ret = rtk_l2_mcastAddr_addByIndex(buf->mcast_data.unit, &(buf->mcast_data.m_data));
            break;

        case RTDRV_L2_IPMCMODE_SET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_ipmcMode_set(buf->l2_learnCfg.unit, buf->l2_learnCfg.ipmcMode);
            break;

        case RTDRV_L2_IP_MCAST_ADDR_ADD:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtk_l2_ipMcastAddr_t));
            ret = rtk_l2_ipMcastAddr_add(buf->ipMcast_data.unit, &buf->ipMcast_data.ip_m_data);
            break;

        case RTDRV_L2_IP_MCAST_ADDR_DEL:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtk_l2_ipMcastAddr_t));
            ret = rtk_l2_ipMcastAddr_del(buf->ipMcast_data.unit, buf->ipMcast_data.ip_m_data.sip, buf->ipMcast_data.ip_m_data.dip,
                                         buf->ipMcast_data.ip_m_data.rvid);
            break;

        case RTDRV_L2_IP_MCAST_ADDR_DEL_IGNORE_INDEX:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtk_l2_ipMcastAddr_t));
            ret = rtk_l2_ipMcastAddr_delIgnoreIndex(buf->ipMcast_data.unit, buf->ipMcast_data.ip_m_data.sip, buf->ipMcast_data.ip_m_data.dip,
                                         buf->ipMcast_data.ip_m_data.rvid);
            break;

        case RTDRV_L2_IP_MCAST_ADDR_SET:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtk_l2_ipMcastAddr_t));
            ret = rtk_l2_ipMcastAddr_set(buf->ipMcast_data.unit, &buf->ipMcast_data.ip_m_data);
            break;

        case RTDRV_L2_IP_MCAST_ADDR_SET_BY_INDEX:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtk_l2_ipMcastAddr_t));
            ret = rtk_l2_ipMcastAddr_setByIndex(buf->ipMcast_data.unit, &buf->ipMcast_data.ip_m_data);
            break;

        case RTDRV_L2_IP_MCAST_ADDR_ADDBYINDEX:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtk_l2_ipMcastAddr_t));
            ret = rtk_l2_ipMcastAddr_addByIndex(buf->ipMcast_data.unit, &buf->ipMcast_data.ip_m_data);
            break;

        case RTDRV_L2_IPMC_DIP_CHK_SET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_ipMcastAddrChkEnable_set(buf->l2_learnCfg.unit, buf->l2_learnCfg.dip_check);
            break;

        case RTDRV_L2_IPMC_VLAN_COMPARE_SET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_ipMcstFidVidCompareEnable_set(buf->l2_common.unit, buf->l2_common.value);
            break;

        case RTDRV_L2_IP6MCASTMODE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_l2_ip6mcMode_set(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;

        case RTDRV_L2_HASHCAREBYTE_SET:
            rtk_copy_from(&buf->l2_hashCareByte, user, sizeof(rtdrv_l2_hashCareByte_t));
            ret = rtk_l2_ip6CareByte_set(buf->l2_hashCareByte.unit, buf->l2_hashCareByte.type, buf->l2_hashCareByte.value);
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_ADD:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_add(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.ip6_m_data);
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_DEL:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_del(buf->ip6Mcast_data.unit, buf->ip6Mcast_data.ip6_m_data.sip, buf->ip6Mcast_data.ip6_m_data.dip,
                                         buf->ip6Mcast_data.ip6_m_data.rvid);
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_DEL_IGNORE_INDEX:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_delIgnoreIndex(buf->ip6Mcast_data.unit, buf->ip6Mcast_data.ip6_m_data.sip, buf->ip6Mcast_data.ip6_m_data.dip,
                                         buf->ip6Mcast_data.ip6_m_data.rvid);
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_SET:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_set(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.ip6_m_data);
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_SET_BY_INDEX:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_setByIndex(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.ip6_m_data);
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_ADDBYINDEX:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_addByIndex(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.ip6_m_data);
            break;

        case RTDRV_L2_MCAST_FWD_INDEX_FREE:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastFwdIndex_free(buf->l2_cfg.l2_portmask.unit, buf->l2_cfg.l2_portmask.index);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_MCAST_FWD_PORTMASK_SET:
            rtk_copy_from(&buf->l2_fwdEntryContent, user, sizeof(rtdrv_l2_fwdTblEntry_t));
            ret = rtk_l2_mcastFwdPortmask_set(buf->l2_fwdEntryContent.unit,
                buf->l2_fwdEntryContent.entryIdx, &buf->l2_fwdEntryContent.portMask, buf->l2_fwdEntryContent.crossVlan);
            break;
#endif
        case RTDRV_L2_MCAST_FWD_PORTMASK_ENTRY_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastFwdPortmaskEntry_set(buf->l2_cfg.l2_portmask.unit, buf->l2_cfg.l2_portmask.index, &buf->l2_cfg.l2_portmask.portmask);
            break;

        case RTDRV_L2_CPU_MAC_ADDR_ADD:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_cpuMacAddr_add(buf->l2_cfg.l2_ucAddr.unit, buf->l2_cfg.l2_ucAddr.vid, &buf->l2_cfg.l2_ucAddr.mac);
            break;

        case RTDRV_L2_CPU_MAC_ADDR_DEL:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_cpuMacAddr_del(buf->l2_cfg.l2_ucAddr.unit, buf->l2_cfg.l2_ucAddr.vid, &buf->l2_cfg.l2_ucAddr.mac);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_PORT_LEGAL_MOVETO_ACTION_SET:
            rtk_copy_from(&buf->l2_action, user, sizeof(rtdrv_l2_portAct_t));
            ret = rtk_l2_legalPortMoveAction_set(buf->l2_action.unit, buf->l2_action.port, buf->l2_action.action);
            break;

        case RTDRV_L2_DYNM_PORTMOVE_FORBID_ACTION_SET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_dynamicPortMoveForbidAction_set(buf->l2_common.unit, buf->l2_common.value);
            break;

#endif
        case RTDRV_L2_PORT_MOVE_ACT_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portMoveAction_set(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.type, &buf->l2_cfg.l2_portMove.action);
            break;

        case RTDRV_L2_PORT_MOVE_LEARN_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portMoveLearn_set(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.type, &buf->l2_cfg.l2_portMove.learn);
            break;

        case RTDRV_L2_LEGAL_MOVETO_FLUSH_ENABLE_SET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_legalPortMoveFlushAddrEnable_set(buf->l2_common.unit, buf->l2_common.port, buf->l2_common.value);
            break;

        case RTDRV_L2_STTC_PORT_MOVE_ACTION_SET:
            rtk_copy_from(&buf->l2_action, user, sizeof(rtdrv_l2_portAct_t));
            ret = rtk_l2_staticPortMoveAction_set(buf->l2_action.unit, buf->l2_action.port, buf->l2_action.action);
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_set(buf->l2_cfg.l2_lookUpMiss.unit, buf->l2_cfg.l2_lookUpMiss.type, &buf->l2_cfg.l2_lookUpMiss.flood_portmask);
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_ADD:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_add(buf->l2_cfg.l2_lookUpMiss.unit, buf->l2_cfg.l2_lookUpMiss.type, buf->l2_cfg.l2_lookUpMiss.port);
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_DEL:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_del(buf->l2_cfg.l2_lookUpMiss.unit, buf->l2_cfg.l2_lookUpMiss.type, buf->l2_cfg.l2_lookUpMiss.port);
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOOD_PMSK_SET_WITH_IDX:
            rtk_copy_from(&buf->l2_lkMiss, user, sizeof(rtdrv_l2_lkMiss_t));
            ret = rtk_l2_lookupMissFloodPortMask_setByIndex(buf->l2_lkMiss.unit, buf->l2_lkMiss.type, buf->l2_lkMiss.index, &buf->l2_lkMiss.portMask);
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOODPORTMASK_IDX_SET:
            rtk_copy_from(&buf->l2_lkMiss, user, sizeof(rtdrv_l2_lkMiss_t));
            ret = rtk_l2_lookupMissFloodPortMaskIdx_set(buf->l2_lkMiss.unit, buf->l2_lkMiss.type, buf->l2_lkMiss.index);
            break;

        case RTDRV_L2_PORT_LOOKUP_MISS_ACTION_SET:
            rtk_copy_from(&buf->l2_lkMiss, user, sizeof(rtdrv_l2_lkMiss_t));
            ret = rtk_l2_portLookupMissAction_set(buf->l2_lkMiss.unit, buf->l2_lkMiss.port, buf->l2_lkMiss.type, buf->l2_lkMiss.action);
            break;

        case RTDRV_L2_PORT_UCAST_LOOKUP_MISS_ACTION_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portUcastLookupMissAction_set(buf->l2_cfg.l2_lookUpMiss.unit, buf->l2_cfg.l2_lookUpMiss.port, buf->l2_cfg.l2_lookUpMiss.action);
            break;

        case RTDRV_L2_SRC_PORT_EGR_FILTER_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_srcPortEgrFilterMask_set(buf->l2_cfg.l2_common.unit, &buf->l2_cfg.l2_common.srcPortFilterPortmask);
            break;

        case RTDRV_L2_SRC_PORT_EGR_FILTER_ADD:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_srcPortEgrFilterMask_add(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.port);
            break;

        case RTDRV_L2_SRC_PORT_EGR_FILTER_DEL:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_srcPortEgrFilterMask_del(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.port);
            break;

        case RTDRV_L2_EXCEPTION_ADDR_ACTION_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_exceptionAddrAction_set(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.exceptType, buf->l2_cfg.l2_common.action);
            break;

        case RTDRV_L2_ZERO_SA_LEARNING_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_zeroSALearningEnable_set(buf->l2_cfg.l2_learn.unit, buf->l2_cfg.l2_learn.enable);
            break;

        case RTDRV_L2_SECURE_MAC_MODE_SET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_secureMacMode_set(buf->l2_common.unit, buf->l2_common.value);
            break;

        case RTDRV_L2_PORT_DYNM_PORTMOVE_FORBID_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portDynamicPortMoveForbidEnable_set(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.port, buf->l2_cfg.l2_portMove.enable);
            break;

        case RTDRV_L2_TRK_DYNM_PORTMOVE_FORBID_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_trkDynamicPortMoveForbidEnable_set(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.trunk, buf->l2_cfg.l2_portMove.enable);
            break;

        case RTDRV_L2_PORT_MAC_FILTER_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portMacFilterEnable_set(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.port, buf->l2_cfg.l2_common.filterMode, buf->l2_cfg.l2_common.enable);
            break;

        case RTDRV_L2_PORT_CTRL_TYPE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portCtrl_set(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.port, buf->l2_cfg.l2_common.portCtrlType, buf->l2_cfg.l2_common.arg);
            break;

        case RTDRV_L2_STK_LEARNING_ENABLE_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_stkLearningEnable_set(buf->l2_cfg.l2_learn.unit, buf->l2_cfg.l2_learn.enable);
            break;

        case RTDRV_L2_STK_KEEP_AGE_VALID_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_stkKeepUcastEntryValid_set(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.enable);
            break;

        case RTDRV_L2_ADDR_DELBYMAC:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_delByMac(buf->l2_cfg.l2_ucAddr.unit, &buf->l2_cfg.l2_ucAddr.mac);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }

    return ret;
}

int32 do_rtdrv_set_ctl_l2ntfy(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    /*L2 Notification module*/
        case RTDRV_L2NTFY_ENABLE_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_enable_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.enable);
            break;

        case RTDRV_L2NTFY_BACK_PRESSURE_THR_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_backPressureThresh_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.thresh);
            break;

        case RTDRV_L2NTFY_EVENT_ENABLE_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_notificationEventEnable_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.event, buf->l2ntfy_cfg.enable);
            break;

        case RTDRV_L2NTFY_COUNTER_DUMP:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_counter_dump(buf->l2ntfy_cfg.unit);
            break;

        case RTDRV_L2NTFY_COUNTER_CLEAR:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_counter_clear(buf->l2ntfy_cfg.unit);
            break;

        case RTDRV_L2NTFY_DBG_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_debug_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.dbgFlag);
            break;

        case RTDRV_L2NTFY_EVENT_DUMP:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_event_dump(buf->l2ntfy_cfg.unit);
            break;

        case RTDRV_L2NTFY_DST_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_dst_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.dst);
            break;

        case RTDRV_L2NTFY_RESET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_reset(buf->l2ntfy_cfg.unit);
            break;

        case RTDRV_L2NTFY_MAGIC_NUM_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_magicNum_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.magicNum);
            break;

        case RTDRV_L2NTFY_MACADDR_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_macAddr_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.addrType, &buf->l2ntfy_cfg.mac);
            break;

        case RTDRV_L2NTFY_MAXEVENT_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_maxEvent_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.maxEvent);
            break;

        case RTDRV_L2NTFY_TIMEOUT_SET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_timeout_set(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.mode, buf->l2ntfy_cfg.timeout);
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_l3(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /*L3*/
        case RTDRV_L3_INIT :
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_l3_init(buf->unit_cfg.unit);
            break;

        case RTDRV_L3_ROUTE_ROUTEENTRY_SET:
            rtk_copy_from(&buf->l3_route_entry, user, sizeof(rtdrv_l3_routeEntry_t));
            ret = rtk_l3_routeEntry_set(buf->l3_route_entry.unit, buf->l3_route_entry.index, &buf->l3_route_entry.entry);
            break;

        case RTDRV_L3_ROUTE_SWITCHMACADDR_SET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_l3_routeSwitchMacAddr_set(buf->l3_config.unit, buf->l3_config.index, &buf->l3_config.mac);
            break;

        case RTDRV_L3_INFO_T_INIT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_info_t_init(&buf->l3_cfg.info);
            break;

        case RTDRV_L3_ROUTERMACENTRY_SET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_routerMacEntry_set(buf->l3_cfg.unit, buf->l3_cfg.index, &buf->l3_cfg.macEntry);
            break;

        case RTDRV_L3_INTF_T_INIT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intf_t_init(&buf->l3_cfg.intf);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_INTF_CREATE:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intf_create(buf->l3_cfg.unit, &buf->l3_cfg.intf);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_INTF_DESTROY:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intf_destroy(buf->l3_cfg.unit, buf->l3_cfg.intfId);
            break;

        case RTDRV_L3_INTF_DESTROYALL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intf_destroyAll(buf->l3_cfg.unit);
            break;

        case RTDRV_L3_INTF_SET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intf_set(buf->l3_cfg.unit, buf->l3_cfg.type, &buf->l3_cfg.intf);
            break;

        case RTDRV_L3_INTFSTATS_RESET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intfStats_reset(buf->l3_cfg.unit, buf->l3_cfg.intfId);
            break;

        case RTDRV_L3_VRRP_ADD:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_vrrp_add(buf->l3_cfg.unit, buf->l3_cfg.vid, buf->l3_cfg.vrId);
            break;

        case RTDRV_L3_VRRP_DEL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_vrrp_del(buf->l3_cfg.unit, buf->l3_cfg.vid, buf->l3_cfg.vrId);
            break;

        case RTDRV_L3_VRRP_DELALL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_vrrp_delAll(buf->l3_cfg.unit, buf->l3_cfg.vid);
            break;

        case RTDRV_L3_NEXTHOP_T_INIT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_nextHop_t_init(&buf->l3_cfg.nextHop);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_NEXTHOP_CREATE:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_nextHop_create(buf->l3_cfg.unit, buf->l3_cfg.flags, &buf->l3_cfg.nextHop, &buf->l3_cfg.nhId);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_NEXTHOP_DESTROY:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_nextHop_destroy(buf->l3_cfg.unit, buf->l3_cfg.nhId);
            break;

        case RTDRV_L3_ECMP_CREATE:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_ecmp_create(buf->l3_cfg.unit, buf->l3_cfg.flags, buf->l3_cfg.nhIdCnt, &buf->l3_cfg.nhIdArray[0], &buf->l3_cfg.ecmpId);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ECMP_DESTROY:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_ecmp_destroy(buf->l3_cfg.unit, buf->l3_cfg.ecmpPId);
            break;

        case RTDRV_L3_ECMP_ADD:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_ecmp_add(buf->l3_cfg.unit, buf->l3_cfg.ecmpId, buf->l3_cfg.nhId);
            break;

        case RTDRV_L3_ECMP_DEL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_ecmp_del(buf->l3_cfg.unit, buf->l3_cfg.ecmpId, buf->l3_cfg.nhId);
            break;

        case RTDRV_L3_KEY_T_INIT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_key_t_init(&buf->l3_cfg.key);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_HOST_T_INIT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_t_init(&buf->l3_cfg.host);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_HOST_ADD:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_add(buf->l3_cfg.unit, &buf->l3_cfg.host);
            break;

        case RTDRV_L3_HOST_DEL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_del(buf->l3_cfg.unit, &buf->l3_cfg.host);
            break;

        case RTDRV_L3_HOST_DELALL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_delAll(buf->l3_cfg.unit, buf->l3_cfg.flags);
            break;

        case RTDRV_L3_HOST_DEL_BYNETWORK:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_del_byNetwork(buf->l3_cfg.unit, &buf->l3_cfg.route);
            break;

        case RTDRV_L3_HOST_DEL_BYINTFID:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_del_byIntfId(buf->l3_cfg.unit, buf->l3_cfg.intfId, buf->l3_cfg.flags);
            break;

        case RTDRV_L3_ROUTE_T_INIT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_t_init(&buf->l3_cfg.route);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ROUTE_ADD:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_add(buf->l3_cfg.unit, &buf->l3_cfg.route);
            break;

        case RTDRV_L3_ROUTE_DEL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_del(buf->l3_cfg.unit, &buf->l3_cfg.route);
            break;

        case RTDRV_L3_ROUTE_DELALL:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_delAll(buf->l3_cfg.unit, buf->l3_cfg.flags);
            break;

        case RTDRV_L3_ROUTE_DEL_BYINTFID:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_del_byIntfId(buf->l3_cfg.unit, buf->l3_cfg.flags, buf->l3_cfg.intfId);
            break;

        case RTDRV_L3_GLOBALCTRL_SET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_globalCtrl_set(buf->l3_cfg.unit, buf->l3_cfg.globalCtrlType, buf->l3_cfg.arg);
            break;

        case RTDRV_L3_INTFCTRL_SET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intfCtrl_set(buf->l3_cfg.unit, buf->l3_cfg.intfId, buf->l3_cfg.type, buf->l3_cfg.arg);
            break;

        case RTDRV_L3_PORTCTRL_SET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_portCtrl_set(buf->l3_cfg.unit, buf->l3_cfg.port, buf->l3_cfg.type, buf->l3_cfg.arg);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_mcast(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        /*MCAST*/
        case RTDRV_MCAST_INIT:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_mcast_init(buf->unit_cfg.unit);
            break;

        case RTDRV_MCAST_GROUP_DESTROY:
            rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
            ret = rtk_mcast_group_destroy(buf->mcast_cfg.unit, buf->mcast_cfg.group);
            break;

        case RTDRV_MCAST_NEXTHOP_ADD:
            rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
            ret = rtk_mcast_egrIf_add(buf->mcast_cfg.unit, buf->mcast_cfg.group, &buf->mcast_cfg.nhEntry);
            break;

        case RTDRV_MCAST_NEXTHOP_DEL:
            rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
            ret = rtk_mcast_egrIf_del(buf->mcast_cfg.unit, buf->mcast_cfg.group, &buf->mcast_cfg.nhEntry);
            break;

        case RTDRV_MCAST_NEXTHOP_DELALL:
            rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
            ret = rtk_mcast_egrIf_delAll(buf->mcast_cfg.unit, buf->mcast_cfg.group);
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_ipmc(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))

        /*IPMCAST*/
        case RTDRV_IPMC_INIT:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_ipmc_init(buf->unit_cfg.unit);
            break;

        case RTDRV_IPMC_ADDR_ADD:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_addr_add(buf->ipmc_cfg.unit, &buf->ipmc_cfg.ipmcEntry);
            break;

        case RTDRV_IPMC_ADDR_DEL:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_addr_del(buf->ipmc_cfg.unit, &buf->ipmc_cfg.ipmcEntry);
            break;

        case RTDRV_IPMC_ADDR_DELALL:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_addr_delAll(buf->ipmc_cfg.unit, buf->ipmc_cfg.flags);
            break;

        case RTDRV_IPMC_STAT_MONT_CREATE:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_statMont_create(buf->ipmc_cfg.unit, &buf->ipmc_cfg.statMont);
            break;

        case RTDRV_IPMC_STAT_MONT_DESTROY:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_statMont_destroy(buf->ipmc_cfg.unit, &buf->ipmc_cfg.statMont);
            break;

        case RTDRV_IPMC_STAT_RESET:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_statCntr_reset(buf->ipmc_cfg.unit, &buf->ipmc_cfg.statKey);
            break;

        case RTDRV_IPMC_GLOBALCTRL_SET:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_globalCtrl_set(buf->ipmc_cfg.unit, buf->ipmc_cfg.globalCtrlType, buf->ipmc_cfg.arg);
            break;
    #endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_tunnel(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /*Tunnel*/
#if defined(CONFIG_SDK_RTL9310)
        case RTDRV_TUNNEL_INIT:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_init(buf->tunnel_cfg.unit);
            break;

        case RTDRV_TUNNEL_INFO_T_INIT:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_info_t_init(&buf->tunnel_cfg.info);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTF_T_INIT:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intf_t_init(&buf->tunnel_cfg.intf);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTF_CREATE:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intf_create(buf->tunnel_cfg.unit, &buf->tunnel_cfg.intf);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTF_DESTROY:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intf_destroy(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId);
            break;

        case RTDRV_TUNNEL_INTF_DESTROYALL:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intf_destroyAll(buf->tunnel_cfg.unit);
            break;

        case RTDRV_TUNNEL_INTF_SET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intf_set(buf->tunnel_cfg.unit, &buf->tunnel_cfg.intf);
            break;

        case RTDRV_TUNNEL_INTFPATHID_SET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intfPathId_set(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId, buf->tunnel_cfg.pathId);
            break;

        case RTDRV_TUNNEL_INTFPATH_SET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intfPath_set(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId, buf->tunnel_cfg.nhDmacIdx, buf->tunnel_cfg.l3EgrIntfIdx);
            break;

        case RTDRV_TUNNEL_INTFSTATS_RESET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intfStats_reset(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId);
            break;

        case RTDRV_TUNNEL_QOSPROFILE_SET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_qosProfile_set(buf->tunnel_cfg.unit, buf->tunnel_cfg.idx, buf->tunnel_cfg.profile);
            break;

        case RTDRV_TUNNEL_GLOBALCTRL_SET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_globalCtrl_set(buf->tunnel_cfg.unit, buf->tunnel_cfg.type, buf->tunnel_cfg.arg);
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_vxlan(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /*VXLAN*/
#if defined(CONFIG_SDK_RTL9310)
        case RTDRV_VXLAN_INIT:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_init(buf->vxlan_cfg.unit);
            break;

        case RTDRV_VXLAN_VNI_ADD:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_vni_add(buf->vxlan_cfg.unit, &buf->vxlan_cfg.entry);
            rtk_copy_to(user, &buf->vxlan_cfg, sizeof(rtdrv_vxlanCfg_t));
            break;

        case RTDRV_VXLAN_VNI_DEL:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_vni_del(buf->vxlan_cfg.unit, &buf->vxlan_cfg.entry);
            break;

        case RTDRV_VXLAN_VNI_DELALL:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_vni_delAll(buf->vxlan_cfg.unit);
            break;

        case RTDRV_VXLAN_VNI_SET:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_vni_set(buf->vxlan_cfg.unit, &buf->vxlan_cfg.entry);
            break;

        case RTDRV_VXLAN_GLOBALCTRL_SET:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_globalCtrl_set(buf->vxlan_cfg.unit, buf->vxlan_cfg.type, buf->vxlan_cfg.arg);
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_port(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /** PORT **/
        case RTDRV_PORT_EN_AUTONEGO_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyAutoNegoEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_AUTONEGO_ABIL_SET:
            rtk_copy_from(&buf->autonego_ability, user, sizeof(rtdrv_port_autoNegoAbility_t));
            ret = rtk_port_phyAutoNegoAbility_set(buf->autonego_ability.unit, buf->autonego_ability.port,
                                                  &buf->autonego_ability.ability);
            break;

        case RTDRV_PORT_FORCE_MODE_ABIL_SET:
            rtk_copy_from(&buf->forcemode_ability, user, sizeof(rtdrv_port_forceModeAbility_t));
            ret = rtk_port_phyForceModeAbility_set(buf->forcemode_ability.unit, buf->forcemode_ability.port,
                                                   buf->forcemode_ability.speed, buf->forcemode_ability.duplex,
                                                   buf->forcemode_ability.flowctrl);
            break;

        case RTDRV_PORT_PHY_REG_SET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phyReg_set(buf->phy_data.unit, buf->phy_data.port, buf->phy_data.page,
                                      buf->phy_data.reg, buf->phy_data.data);
            break;

        case RTDRV_PORT_PHY_EXT_PARK_PAGE_REG_SET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phyExtParkPageReg_set(buf->phy_data.unit, buf->phy_data.port, buf->phy_data.page,
                                      buf->phy_data.extPage, buf->phy_data.parkPage, buf->phy_data.reg, buf->phy_data.data);
            break;

        case RTDRV_PORT_PHYMASK_EXT_PARK_PAGE_REG_SET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phymaskExtParkPageReg_set(buf->phy_data.unit, &buf->phy_data.portmask, buf->phy_data.page,
                                      buf->phy_data.extPage, buf->phy_data.parkPage, buf->phy_data.reg, buf->phy_data.data);
            break;

        case RTDRV_PORT_PHY_MMD_REG_SET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phyMmdReg_set(buf->phy_data.unit, buf->phy_data.port, buf->phy_data.mmdAddr,
                                      buf->phy_data.reg, buf->phy_data.data);
            break;

        case RTDRV_PORT_PHYMASK_MMD_REG_SET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phymaskMmdReg_set(buf->phy_data.unit, &buf->phy_data.portmask, buf->phy_data.mmdAddr,
                                      buf->phy_data.reg, buf->phy_data.data);
            break;

        case RTDRV_PORT_MASTER_SLAVE_SET:
            rtk_copy_from(&buf->masterSlave_cfg, user, sizeof(rtdrv_port_masterSlave_t));
            ret = rtk_port_phyMasterSlave_set(buf->masterSlave_cfg.unit, buf->masterSlave_cfg.port, buf->masterSlave_cfg.masterSlaveCfg);
            break;

        case RTDRV_PORT_ISOLATION_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolation_set(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.portmask);
            break;

        case RTDRV_PORT_ISOLATIONEXT_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationExt_set(buf->port_cfg.unit, buf->port_cfg.devID, buf->port_cfg.port, &buf->port_cfg.portmask);
            break;

         case RTDRV_PORT_ISOLATION_ADD:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolation_add(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.targetPort);
            break;

        case RTDRV_PORT_ISOLATION_DEL:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolation_del(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.targetPort);
            break;

        case RTDRV_PORT_ISOLATION_RESTRICT_ROUTE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationRestrictRoute_set(buf->port_cfg.unit, buf->port_cfg.data);
            break;

        case RTDRV_PORT_EN_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_adminEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_BACK_PRESSURE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_backpressureEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_PHY_MEDIA_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyComboPortMedia_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.media);
            break;

        case RTDRV_PORT_GREEN_ENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_greenEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_GIGA_LITE_ENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_gigaLiteEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_2PT5G_LITE_ENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_2pt5gLiteEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_PHY_CROSSOVERMODE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyCrossOverMode_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_TX_EN_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_txEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_RX_EN_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_rxEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_PHY_FIBER_MEDIA_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyComboPortFiberMedia_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.fiber_media);
            break;

        case RTDRV_PORT_LINKDOWN_POWERSAVING_ENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_linkDownPowerSavingEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_VLAN_ISOLATION_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_vlanBasedIsolationEntry_set(buf->port_cfg.unit, buf->port_cfg.index, &buf->port_cfg.vlanIsoEntry);
            break;

        case RTDRV_PORT_VLAN_ISOLATION_VLANSOURCE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_vlanBasedIsolation_vlanSource_set(buf->port_cfg.unit, buf->port_cfg.vlanIsoSrc);
            break;

        case RTDRV_PORT_VLAN_ISOLATION_EGRBYPASS_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_vlanBasedIsolationEgrBypass_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_DOWNSPEEDENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_downSpeedEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;
        case RTDRV_PORT_FIBERDOWNSPEEDENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberDownSpeedEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;
        case RTDRV_PORT_FIBERNWAYFORCELINKENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberNwayForceLinkEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;
        case RTDRV_PORT_FIBERUNIDIRENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberUnidirEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.enable);
            break;
        case RTDRV_PORT_FIBEROAMLOOPBACKENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberOAMLoopBackEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;
        case RTDRV_PORT_10GMEDIA_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_10gMedia_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.media_10g);
            break;
        case RTDRV_PORT_PHYLOOPBACKENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyLoopBackEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_PHYFIBERTXDIS_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyFiberTxDis_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.enable);
            break;

        case RTDRV_PORT_PHYFIBERTXDISPIN_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyFiberTxDisPin_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_PORT_FIBERRXENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberRxEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.enable);
            break;

        case RTDRV_PORT_PHYIEEETESTMODE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyIeeeTestMode_set(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.testMode);
            break;
        case RTDRV_PORT_PHYPOLAR_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyPolar_set(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.polarCtrl);
            break;
        case RTDRV_PORT_PHYEYEMONITOR_START:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyEyeMonitor_start(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.sdsId, buf->port_cfg.data);
            break;
        case RTDRV_PORT_IMAGEFLASH_LOAD:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_imageFlash_load(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.image_size, buf->port_cfg.image);
            break;
        case RTDRV_PORT_PHYSDS_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phySds_set(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.sdsCfg);
            break;
        case RTDRV_PORT_FORCE_MODE_FLOW_CTRL_MODE_SET:
        {
            rtk_port_flowctrl_mode_t    fcMode;
            rtk_copy_from(&buf->port_flowctrl, user, sizeof(rtdrv_port_flowctrl_t));
            fcMode.tx_pause = buf->port_flowctrl.tx_status;
            fcMode.rx_pause = buf->port_flowctrl.rx_status;
            ret = rtk_port_phyForceFlowctrlMode_set(buf->port_flowctrl.unit, buf->port_flowctrl.port, &fcMode);
            break;
        }
        case RTDRV_PORT_PHYRESET_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyReset_set(buf->port_cfg.unit, buf->port_cfg.port);
            break;
        case RTDRV_PORT_PHYLEDMODE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyLedMode_set(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.phyLedMode);
            break;

        case RTDRV_PORT_PHYLEDCTRL_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyLedCtrl_set(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.phyLedCtrl);
            break;
        case RTDRV_PORT_PHYEYEPARAM_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phySdsEyeParam_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.sdsId, &buf->port_cfg.eyeParam);
            break;

        case RTDRV_PORT_MDI_LOOPBACK_ENABLE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyMdiLoopbackEnable_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.enable);
            break;

        case RTDRV_PORT_SPECL_CGST_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_specialCongest_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_vlan(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** VLAN **/
        case RTDRV_VLAN_PORT_SET:
            rtk_copy_from(&buf->vlan_port_data, user, sizeof(rtdrv_vlan_port_t));
            ret = rtk_vlan_port_set(buf->vlan_port_data.unit, buf->vlan_port_data.vid, &buf->vlan_port_data.member,
                                    &buf->vlan_port_data.untag);
            break;

        case RTDRV_VLAN_PORT_PVID_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portPvid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PROTO_GROUP_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_protoGroup_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &(buf->vlan_cfg.protoGroup));
            break;

        case RTDRV_VLAN_PORT_PROTO_VLAN_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portProtoVlan_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.idx, &(buf->vlan_cfg.protoVlanCfg));
            break;

        case RTDRV_VLAN_PORT_IGR_TPID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrTpid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGR_TPID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTpid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGR_TPID_SRC_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTpidSrc_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_IGR_EXTRA_TAG_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrExtraTagEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_IGR_VLAN_TRANSPARENT_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanTransparentEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGR_VLAN_TRANSPARENT_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanTransparentEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_ACCEPT_FRAME_TYPE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portAcceptFrameType_set(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EN_MCAST_LEAKY_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_vlan_mcastLeakyEnable_set(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;

        case RTDRV_VLAN_SVLMODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_svlMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.mode);
            break;

        case RTDRV_VLAN_SVLFID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_svlFid_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.fid);
            break;

        case RTDRV_VLAN_STG_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_stg_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_L2_LOOKUP_SVL_FID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2LookupSvlFid_set(buf->vlan_cfg.unit, buf->vlan_cfg.macType, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_CREATE:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_create(buf->vlan_cfg.unit, buf->vlan_cfg.vid);
            break;

        case RTDRV_VLAN_DESTROY:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_destroy(buf->vlan_cfg.unit, buf->vlan_cfg.vid);
            break;

        case RTDRV_VLAN_DESTROY_ALL:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_vlan_destroyAll(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGR_FILTER_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrFilterEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_ADD:
            rtk_copy_from(&buf->vlan_port_data, user, sizeof(rtdrv_vlan_port_t));
            ret = rtk_vlan_port_add(buf->vlan_port_data.unit, buf->vlan_port_data.vid, buf->vlan_port_data.port,
                                    buf->vlan_port_data.is_untag);
            break;

        case RTDRV_VLAN_PORT_DEL:
            rtk_copy_from(&buf->vlan_port_data, user, sizeof(rtdrv_vlan_port_t));
            ret = rtk_vlan_port_del(buf->vlan_port_data.unit, buf->vlan_port_data.vid, buf->vlan_port_data.port);
            break;

        case RTDRV_VLAN_MCASTGROUP_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_mcastGroup_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.groupId);
            break;

        case RTDRV_VLAN_LUTMODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2LookupMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.macType, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_GROUPMASK_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_groupMask_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.groupMask);
            break;

        case RTDRV_VLAN_PROFILE_IDX_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_profileIdx_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PROFILE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_profile_set(buf->vlan_cfg.unit, buf->vlan_cfg.data, &buf->vlan_cfg.profile);
            break;

        case RTDRV_VLAN_PORT_FORWARD_VLAN_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portFwdVlan_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.tagMode, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_IGR_FILTER_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrFilter_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_PVID_MODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPvidMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type,buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_MAC_BASED_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlan_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    buf->vlan_cfg.data, &buf->vlan_cfg.mac, buf->vlan_cfg.vid,
                    buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_MAC_BASED_WITH_MSK_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanWithMsk_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    buf->vlan_cfg.data, &buf->vlan_cfg.mac, &buf->vlan_cfg.msk, buf->vlan_cfg.vid,
                    buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_MAC_BASED_WITH_PORT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanWithPort_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    buf->vlan_cfg.data, &buf->vlan_cfg.mac, &buf->vlan_cfg.msk,
                    buf->vlan_cfg.port, buf->vlan_cfg.port_msk, buf->vlan_cfg.vid, buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_MAC_BASED_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portMacBasedVlanEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_MAC_BASED_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,&buf->vlan_cfg.macEntry);
            break;

        case RTDRV_VLAN_MACBASEDVLANENTRY_ADD:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanEntry_add(buf->vlan_cfg.unit, &buf->vlan_cfg.macBasedEntry);
            break;

        case RTDRV_VLAN_MACBASEDVLANENTRY_DEL:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanEntry_del(buf->vlan_cfg.unit, &buf->vlan_cfg.macBasedEntry);
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlan_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    buf->vlan_cfg.data, buf->vlan_cfg.sip, buf->vlan_cfg.sip_msk,
                    buf->vlan_cfg.vid, buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_WITH_PORT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlanWithPort_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    buf->vlan_cfg.data, buf->vlan_cfg.sip, buf->vlan_cfg.sip_msk,
                    buf->vlan_cfg.port, buf->vlan_cfg.port_msk, buf->vlan_cfg.vid, buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIpSubnetBasedVlanEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlanEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx,&buf->vlan_cfg.ipEntry);
            break;

        case RTDRV_VLAN_IPSUBNETBASEDVLANENTRY_ADD:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlanEntry_add(buf->vlan_cfg.unit, &buf->vlan_cfg.ipEntry);
            break;

        case RTDRV_VLAN_IPSUBNETBASEDVLANENTRY_DEL:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlanEntry_del(buf->vlan_cfg.unit, &buf->vlan_cfg.ipEntry);
            break;

        case RTDRV_VLAN_TPID_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tpidEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.tagType, buf->vlan_cfg.idx, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EGR_TAG_STS_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTagSts_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_IGRVLANCNVT_BLKMODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtBlkMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_IGRVLANCNVT_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.igrCnvtEntry);
            break;

        case RTDRV_VLAN_PORTIGRVLANCNVTENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanCnvtEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_EGRVLANCNVT_DBLTAG_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtDblTagEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EGRVLANCNVT_VIDSRC_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtVidSource_set(buf->vlan_cfg.unit, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EGRVLANCNVT_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.egrCnvtEntry);
            break;

        case RTDRV_VLAN_PORTEGRVLANCNVTENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_AGGRENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_aggrEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_PORT_VLANAGGR_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_VLANAGGR_PRI_ENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrPriEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_LEAKYSTPFILTER_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_leakyStpFilter_set(buf->vlan_cfg.unit, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EXCEPT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_except_set(buf->vlan_cfg.unit, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORTIGRCNVTDFLTACT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrCnvtDfltAct_set(buf->vlan_cfg.unit,
                    buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_IGRVLANCNVTENTRY_DELALL:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtEntry_delAll(buf->vlan_cfg.unit);
            break;

        case RTDRV_VLAN_EGRVLANCNVTENTRY_DELALL:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtEntry_delAll(buf->vlan_cfg.unit);
            break;

        case RTDRV_VLAN_PORT_IGRTAGKEEPTYPE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrTagKeepType_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data, buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_PORT_EGRTAGKEEPTYPE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTagKeepType_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data, buf->vlan_cfg.data1);
            break;

        case RTDRV_VLAN_PORT_VLANAGGRCTRL_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrCtrl_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.vlanAggrCtrl);
            break;

        case RTDRV_VLAN_PORT_EGRVLANCNVTVIDSOURCE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtVidSource_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGRVLANCNVTVIDTARGET_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtVidTarget_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORTIGRCNVTLUMISACT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanCnvtLuMisAct_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORTEGRCNVTLUMISACT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtLuMisAct_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_IGRVLANCNVTRANGECHECKSET_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanCnvtRangeCheckSet_set(buf->vlan_cfg.unit,buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_IGRVLANCNVTRANGECHECKENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtRangeCheckEntry_set(buf->vlan_cfg.unit,buf->vlan_cfg.setIdx, buf->vlan_cfg.idx, &buf->vlan_cfg.rangeCheck);
            break;

         case RTDRV_VLAN_EGRVLANCNVTRANGECHECKSET_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtRangeCheckSet_set(buf->vlan_cfg.unit,buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EGRVLANCNVTRANGECHECKENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtRangeCheckEntry_set(buf->vlan_cfg.unit,buf->vlan_cfg.setIdx, buf->vlan_cfg.idx, &buf->vlan_cfg.rangeCheck);
            break;

#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
        case RTDRV_VLAN_PORT_VLANAGGRVIDSOURCE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrVidSource_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_VLANAGGRPRITAGVIDSOURCE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrPriTagVidSource_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_UCAST_LUTMODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2UcastLookupMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_MCAST_LUTMODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2McastLookupMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_INNER_ACCEPT_FRAME_TYPE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portInnerAcceptFrameType_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_VLAN_PORT_OUTER_ACCEPT_FRAME_TYPE_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portOuterAcceptFrameType_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_VLAN_PORT_INNER_PVID_MODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portInnerPvidMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_OUTER_PVID_MODE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portOuterPvidMode_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_INNER_PVID_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portInnerPvid_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_VLAN_PORT_OUTER_PVID_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portOuterPvid_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data);
            break;

        case RTDRV_VLAN_INNER_TPID_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_innerTpidEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_OUTER_TPID_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_outerTpidEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EXTRA_TPID_ENTRY_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extraTpidEntry_set(buf->vlan_cfg.unit, buf->vlan_cfg.idx, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_IGR_INNER_TPID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrInnerTpid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_IGR_OUTER_TPID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrOuterTpid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGR_INNER_TPID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrInnerTpid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGR_OUTER_TPID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrOuterTpid_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EGR_INNER_TAG_STS_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrInnerTagSts_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_EGR_OUTER_TAG_STS_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrOuterTagSts_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_EGRVLANCNVTLOOKUPMISSACT_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.data);
            break;
#endif

        case RTDRV_VLAN_EGRVLANCNVTRANGECHECKVID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtRangeCheckVid_set(buf->vlan_cfg.unit,
                    buf->vlan_cfg.idx, &buf->vlan_cfg.rangeCheck);
            break;

        case RTDRV_VLAN_ECIDPMSK_ADD:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ecidPmsk_add(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.entry);
            break;

        case RTDRV_VLAN_ECIDPMSK_DEL:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ecidPmsk_del(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.entry);
            break;

        case RTDRV_VLAN_TRKVLANAGGRENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_trkVlanAggrEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.tid, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_TRKVLANAGGRPRIENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_trkVlanAggrPriEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.tid, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_TRKVLANAGGRCTRL_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_trkVlanAggrCtrl_set(buf->vlan_cfg.unit, buf->vlan_cfg.tid, buf->vlan_cfg.vlanAggrCtrl);
            break;

        case RTDRV_VLAN_PORTPRIVATEVLANENABLE_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPrivateVlanEnable_set(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.enable);
            break;

        case RTDRV_VLAN_INTFID_SET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_intfId_set(buf->vlan_cfg.unit, buf->vlan_cfg.vid, buf->vlan_cfg.intfId);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_oam(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
    /*OAM*/
        case RTDRV_OAM_INIT :
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_oam_init(buf->port_cfg.unit);
            break;

        case RTDRV_OAM_PORTDYINGGASPPAYLOAD_SET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_portDyingGaspPayload_set(buf->dyingGasp_cfg.unit,
                    buf->dyingGasp_cfg.port, buf->dyingGasp_cfg.payload,
                    buf->dyingGasp_cfg.cnt);
            break;

        case RTDRV_OAM_DYINGGASPSEND_SET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_dyingGaspSend_set(buf->dyingGasp_cfg.unit, buf->dyingGasp_cfg.enable);
            break;

        case RTDRV_OAM_AUTODYINGGASPENABLE_SET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_autoDyingGaspEnable_set(buf->dyingGasp_cfg.unit, buf->dyingGasp_cfg.port, buf->dyingGasp_cfg.enable);
            break;

        case RTDRV_OAM_DYINGGASPWAITTIME_SET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_dyingGaspWaitTime_set(buf->dyingGasp_cfg.unit, buf->dyingGasp_cfg.waitTime);
            break;

        case RTDRV_OAM_DYINGGASPPKTCNT_SET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_dyingGaspPktCnt_set(buf->dyingGasp_cfg.unit, buf->dyingGasp_cfg.cnt);
            break;

        case RTDRV_OAM_LOOPBACKMACSWAPENABLE_SET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_oam_loopbackMacSwapEnable_set(buf->misc_cfg.unit,
                                                    buf->misc_cfg.loopbackEnable);
            break;

        case RTDRV_OAM_PORTLOOPBACKMUXACTION_SET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_oam_portLoopbackMuxAction_set(buf->misc_cfg.unit,
                    buf->misc_cfg.port, buf->misc_cfg.action);
            break;

        case RTDRV_OAM_PDULEARNINGENABLE_SET:
            rtk_copy_from(&buf->oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_pduLearningEnable_set(buf->oam_cfg.unit, buf->oam_cfg.enable);
            break;

        case RTDRV_OAM_CFMCCMPCP_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmPcp_set(buf->ccm_cfg.unit,
                                        buf->ccm_cfg.ccmFrame.outer_pri);
            break;

        case RTDRV_OAM_CFMCCMCFI_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmCfi_set(buf->ccm_cfg.unit,
                                        buf->ccm_cfg.ccmFrame.outer_dei);
            break;

        case RTDRV_OAM_CFMCCMTPID_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmTpid_set(buf->ccm_cfg.unit,
                                         buf->ccm_cfg.ccmFrame.outer_tpid);
            break;

        case RTDRV_OAM_CFMCCMRESETLIFETIME_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstLifetime_set(buf->ccm_cfg.unit,
                                                  buf->ccm_cfg.cfmIdx,
                                                  buf->ccm_cfg.ccmFlag);
            break;
#if (defined CONFIG_SDK_RTL8390)
        case RTDRV_OAM_CFMCCMMEPID_SET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_oam_cfmCcmMepid_set(buf->misc_cfg.unit,
                                          buf->misc_cfg.mepid);
            break;

        case RTDRV_OAM_CFMCCMINTERVALFIELD_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmIntervalField_set(buf->ccm_cfg.unit,
                                                  buf->ccm_cfg.ccmFlag);
            break;

        case RTDRV_OAM_CFMCCMMDL_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmMdl_set(buf->cfm_cfg.unit,
                                        buf->cfm_cfg.cfmCfg.md_level);
            break;
#endif  /* (defined CONFIG_SDK_RTL8390) */

        case RTDRV_OAM_CFMCCMINSTTXMEPID_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxMepid_set(buf->cfm_cfg.unit,
                    buf->cfm_cfg.instance, buf->cfm_cfg.mepid);
            break;

        case RTDRV_OAM_CFMCCMINSTTXINTERVALFIELD_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxIntervalField_set(buf->cfm_cfg.unit,
                    buf->cfm_cfg.instance, buf->cfm_cfg.interval);
            break;

        case RTDRV_OAM_CFMCCMINSTTXMDL_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxMdl_set(buf->cfm_cfg.unit,
                    buf->cfm_cfg.instance, buf->cfm_cfg.mdl);
            break;

        case RTDRV_OAM_CFMCCMINSTTAGSTATUS_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTagStatus_set(buf->cfm_cfg.unit,
                                                  buf->cfm_cfg.cfmIdx,
                                                  buf->cfm_cfg.enable);
            break;

        case RTDRV_OAM_CFMCCMINSTVID_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstVid_set(buf->ccm_cfg.unit,
                                            buf->ccm_cfg.cfmIdx,
                                            buf->ccm_cfg.ccmFrame.outer_vid);
            break;

        case RTDRV_OAM_CFMCCMINSTMAID_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstMaid_set(buf->cfm_cfg.unit,
                                             buf->cfm_cfg.cfmIdx,
                                             buf->cfm_cfg.maid);
            break;

        case RTDRV_OAM_CFMCCMINSTTXSTATUS_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxStatus_set(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.cfmIdx,
                                                 buf->cfm_cfg.enable);
            break;

        case RTDRV_OAM_CFMCCMINSTINTERVAL_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstInterval_set(buf->ccm_cfg.unit,
                                                 buf->ccm_cfg.cfmIdx,
                                                 buf->ccm_cfg.ccmInterval);
            break;
#if (defined CONFIG_SDK_RTL8390)
        case RTDRV_OAM_CFMCCMTXINSTPORT_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmTxInstPort_set(buf->ccm_cfg.unit,
                                               buf->ccm_cfg.cfmIdx,
                                               buf->ccm_cfg.portIdx,
                                               buf->ccm_cfg.port);
            break;
#endif  /* (defined CONFIG_SDK_RTL8390) */
        case RTDRV_OAM_CFMCCMRXINSTVID_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmRxInstVid_set(buf->ccm_cfg.unit,
                                              buf->ccm_cfg.cfmIdx,
                                              buf->ccm_cfg.ccmFrame.outer_vid);
            break;
#if (defined CONFIG_SDK_RTL8390)
        case RTDRV_OAM_CFMCCMRXINSTPORT_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmRxInstPort_set(buf->ccm_cfg.unit,
                                               buf->ccm_cfg.cfmIdx,
                                               buf->ccm_cfg.portIdx,
                                               buf->ccm_cfg.port);
            break;
#endif  /* (defined CONFIG_SDK_RTL8390) */
        case RTDRV_OAM_CFMCCMINSTTXMEMBER_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstTxMember_set(buf->ccm_cfg.unit, buf->ccm_cfg.instance, &buf->ccm_cfg.member);
            break;

        case RTDRV_OAM_CFMCCMINSTRXMEMBER_SET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstRxMember_set(buf->ccm_cfg.unit, buf->ccm_cfg.instance, &buf->ccm_cfg.member);
            break;

        case RTDRV_OAM_CFMETHDMPORTENABLE_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmPortEthDmEnable_set(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.port,
                                                 buf->cfm_cfg.enable);
            break;

        case RTDRV_OAM_CFMETHDMTXDELAY_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmTxDelay_set(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.txDelay);
            break;

        case RTDRV_OAM_CFMETHDMREFTIME_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRefTime_set(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.timeStamp);
            break;

        case RTDRV_OAM_CFMETHDMREFTIMEENABLE_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRefTimeEnable_set(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.enable);
            break;

        case RTDRV_OAM_CFMETHDMREFTIMEFREQ_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRefTimeFreq_set(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.freq);
            break;

        case RTDRV_OAM_LINKFAULTMONENABLE_SET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_linkFaultMonEnable_set(buf->cfm_cfg.unit, buf->cfm_cfg.enable);
            break;
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)) */

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_bpe(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** BPE **/
        case RTDRV_BPE_INIT:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_init(buf->bpe_cfg.unit);
            break;

        case RTDRV_BPE_PORTFWDMODE_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portFwdMode_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.mode);
            break;

        case RTDRV_BPE_PORTECIDNAMESPACEGROUPID_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portEcidNameSpaceGroupId_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.groupId);
            break;

        case RTDRV_BPE_PORTPCID_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portPcid_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.type, buf->bpe_cfg.pcid);
            break;

        case RTDRV_BPE_PORTPCIDACT_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portPcidAct_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.action);
            break;

        case RTDRV_BPE_PORTEGRTAGSTS_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portEgrTagSts_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.status);
            break;

        case RTDRV_BPE_PORTEGRVLANTAGSTS_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portEgrVlanTagSts_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.vlan_status);
            break;

        case RTDRV_BPE_PVIDENTRY_ADD:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_pvidEntry_add(buf->bpe_cfg.unit, &buf->bpe_cfg.pvid_entry);
            break;

        case RTDRV_BPE_PVIDENTRY_DEL:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_pvidEntry_del(buf->bpe_cfg.unit, &buf->bpe_cfg.pvid_entry);
            break;

        case RTDRV_BPE_FWDENTRY_ADD:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_fwdEntry_add(buf->bpe_cfg.unit, &buf->bpe_cfg.fwd_entry);
            break;

        case RTDRV_BPE_FWDENTRY_DEL:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_fwdEntry_del(buf->bpe_cfg.unit, &buf->bpe_cfg.fwd_entry);
            break;

        case RTDRV_BPE_GLOBALCTRL_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_globalCtrl_set(buf->bpe_cfg.unit, buf->bpe_cfg.type, buf->bpe_cfg.arg);
            break;

        case RTDRV_BPE_PORTCTRL_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portCtrl_set(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.type, buf->bpe_cfg.arg);
            break;

        case RTDRV_BPE_PRIREMARKING_SET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_priRemarking_set(buf->bpe_cfg.unit, buf->bpe_cfg.src, buf->bpe_cfg.val, buf->bpe_cfg.pri);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_stp(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** STP **/
        case RTDRV_STP_MSTP_STATE_SET:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpState_set(buf->stp_cfg.unit, buf->stp_cfg.msti, buf->stp_cfg.port, buf->stp_cfg.stp_state);
            break;

        case RTDRV_STP_MSTP_INSTANCE_CREATE:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpInstance_create(buf->stp_cfg.unit, buf->stp_cfg.msti);
            break;

        case RTDRV_STP_MSTP_INSTANCE_DESTROY:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpInstance_destroy(buf->stp_cfg.unit, buf->stp_cfg.msti);
            break;

        case RTDRV_STP_MSTP_MODE_SET:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpInstanceMode_set(buf->stp_cfg.unit, buf->stp_cfg.msti_mode);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_reg(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** REG **/
        case RTDRV_REG_REGISTER_SET:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ioal_mem32_write(buf->reg_cfg.unit, buf->reg_cfg.reg, buf->reg_cfg.value);
            ret = RT_ERR_OK; /*xxx_reg_register_set(buf->reg_cfg.unit, buf->reg_cfg.reg, buf->reg_cfg.value);*/
            break;

        case RTDRV_TABLE_WRITE:
            rtk_copy_from(&buf->tbl_cfg, user, sizeof(rtdrv_tblCfg_t));
            ret = table_write(buf->tbl_cfg.unit, buf->tbl_cfg.table, buf->tbl_cfg.addr, buf->tbl_cfg.value);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_counter(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** COUNTER **/
        case RTDRV_COUNTER_GLOBAL_RESET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_global_reset(buf->counter_cfg.unit);
            break;

        case RTDRV_COUNTER_PORT_RESET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_port_reset(buf->counter_cfg.unit, buf->counter_cfg.port);
            break;

        case RTDRV_COUNTER_TAGLENCNT_SET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_tagLenCntIncEnable_set(buf->counter_cfg.unit, buf->counter_cfg.tagCnt_type, buf->counter_cfg.enable);
            break;

        case RTDRV_COUNTER_STACKHDRLENCNT_SET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_stackHdrLenCntIncEnable_set(buf->counter_cfg.unit, buf->counter_cfg.type, buf->counter_cfg.enable);
            break;

        case RTDRV_COUNTER_FLEXCNTR_CFG_SET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_flexibleCntRange_set(buf->counter_cfg.unit, buf->counter_cfg.cntr_idx, &buf->counter_cfg.range);
            break;

        case RTDRV_COUNTER_ENABLE_SET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_enable_set(buf->counter_cfg.unit, buf->counter_cfg.enable);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_time(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** TIME **/
        case RTDRV_TIME_PORT_PTP_ENABLE_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpEnable_set(buf->time_cfg.unit, buf->time_cfg.port, buf->time_cfg.enable);
            break;

        case RTDRV_TIME_PORT_REF_TIME_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTime_set(buf->time_cfg.unit, buf->time_cfg.portmask, buf->time_cfg.timeStamp);
            break;

        case RTDRV_TIME_PORT_REF_TIME_ADJUST_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTimeAdjust_set(buf->time_cfg.unit, buf->time_cfg.portmask, buf->time_cfg.sign, buf->time_cfg.timeStamp);
            break;

        case RTDRV_TIME_PORT_REF_TIME_ENABLE_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTimeEnable_set(buf->time_cfg.unit, buf->time_cfg.portmask, buf->time_cfg.enable);
            break;

        case RTDRV_TIME_PORT_REF_TIME_FREQ_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTimeFreq_set(buf->time_cfg.unit, buf->time_cfg.portmask, buf->time_cfg.freq);
            break;

        case RTDRV_TIME_PORT_MAC_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpMacAddr_set(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.mac);
            break;

        case RTDRV_TIME_PORT_MAC_RANGE_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpMacAddrRange_set(buf->time_cfg.unit, buf->time_cfg.port, buf->time_cfg.range);
            break;

        case RTDRV_TIME_PORT_VLAN_TPID_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpVlanTpid_set(buf->time_cfg.unit, buf->time_cfg.port, buf->time_cfg.type, buf->time_cfg.idx, buf->time_cfg.tpid);
            break;

        case RTDRV_TIME_PORT_OPER_SET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpOper_set(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.operCfg);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_trap(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** TRAP **/
        case RTDRV_TRAP_RMAACTION_SET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaAction_set(buf->trap_cfg.unit, &buf->trap_cfg.rma_frame, buf->trap_cfg.rma_action);
            break;

        case RTDRV_TRAP_BYPASS_STP_SET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_bypassStp_set(buf->trap_cfg.unit, buf->trap_cfg.bypassStp_frame, buf->trap_cfg.enable);
            break;

        case RTDRV_TRAP_BYPASS_VLAN_SET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_bypassVlan_set(buf->trap_cfg.unit, buf->trap_cfg.bypassVlan_frame, buf->trap_cfg.enable);
            break;

        case RTDRV_TRAP_USERDEFINERMA_SET:
            rtk_copy_from(&buf->l2_trap_cfg, user, sizeof(rtdrv_trapL2userRmaCfg_t));
            ret = rtk_trap_userDefineRma_set(buf->l2_trap_cfg.unit, buf->l2_trap_cfg.rma_index, &buf->l2_trap_cfg.rma_frame);
            break;

        case RTDRV_TRAP_USERDEFINERMAENABLE_SET:
            rtk_copy_from(&buf->l2_trap_cfg, user, sizeof(rtdrv_trapL2userRmaCfg_t));
            ret = rtk_trap_userDefineRmaEnable_set(buf->l2_trap_cfg.unit, buf->l2_trap_cfg.rma_index, buf->l2_trap_cfg.enable);
            break;

        case RTDRV_TRAP_USERDEFINERMAACTION_SET:
            rtk_copy_from(&buf->l2_trap_cfg, user, sizeof(rtdrv_trapL2userRmaCfg_t));
            ret = rtk_trap_userDefineRmaAction_set(buf->l2_trap_cfg.unit, buf->l2_trap_cfg.rma_index, buf->l2_trap_cfg.rma_action);
            break;

        case RTDRV_TRAP_MGMTFRAMEACTION_SET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFrameAction_set(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.frameType, buf->mgm_trap_cfg.rma_action);
            break;

        case RTDRV_TRAP_MGMTFRAMEPRI_SET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFramePri_set(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.frameType, buf->mgm_trap_cfg.priority);
            break;

        case RTDRV_TRAP_MGMTFRAMEQID_SET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFrameQueue_set(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.qType, buf->mgm_trap_cfg.qid);
            break;

        case RTDRV_TRAP_PORTMGMTFRAMEACTION_SET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_portMgmtFrameAction_set(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.port,
                        buf->mgm_trap_cfg.frameType, buf->mgm_trap_cfg.rma_action);
            break;

        case RTDRV_TRAP_PKTWITHCFIACTION_SET:
            rtk_copy_from(&buf->other_trap_cfg, user, sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_pktWithCFIAction_set(buf->other_trap_cfg.unit,
                    buf->other_trap_cfg.action);
            break;

        case RTDRV_TRAP_PKTWITHOUTERCFIACTION_SET:
            rtk_copy_from(&buf->other_trap_cfg, user, sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_pktWithOuterCFIAction_set(buf->other_trap_cfg.unit,
                    buf->other_trap_cfg.action);
            break;

        case RTDRV_TRAP_PKTWITHCFIPRI_SET:
            rtk_copy_from(&buf->other_trap_cfg, user, sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_pktWithCFIPri_set(buf->other_trap_cfg.unit,
                    buf->other_trap_cfg.priority);
            break;
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
        case RTDRV_TRAP_CFMUNKNOWNFRAMEACT_SET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_trap_cfmUnknownFrameAct_set(buf->misc_cfg.unit,
                                                  buf->misc_cfg.action);
            break;

        case RTDRV_TRAP_CFMLOOPBACKACT_SET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmLoopbackLinkTraceAct_set(buf->cfm_trap_cfg.unit,
                                             buf->cfm_trap_cfg.md_level,
                                             buf->cfm_trap_cfg.action);
            break;

        case RTDRV_TRAP_CFMCCMACT_SET:
            rtk_copy_from(&buf->oam_trap_cfg, user, sizeof(rtdrv_trapOamCfg_t));
            ret = rtk_trap_cfmCcmAct_set(buf->oam_trap_cfg.unit,
                                         buf->oam_trap_cfg.md_level,
                                         buf->oam_trap_cfg.action);
            break;

        case RTDRV_TRAP_CFMETHDMACT_SET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmEthDmAct_set(buf->cfm_trap_cfg.unit,
                                           buf->cfm_trap_cfg.md_level,
                                           buf->cfm_trap_cfg.action);
            break;
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
        case RTDRV_TRAP_CFMFRAMETRAPPRI_SET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmFrameTrapPri_set(buf->cfm_trap_cfg.unit, buf->cfm_trap_cfg.priority);
            break;

        case RTDRV_TRAP_OAMPDUACTION_SET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_oamPDUAction_set(buf->cfm_trap_cfg.unit, buf->cfm_trap_cfg.action);
            break;

        case RTDRV_TRAP_OAMPDUPRI_SET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_oamPDUPri_set(buf->cfm_trap_cfg.unit, buf->cfm_trap_cfg.priority);
            break;

        case RTDRV_TRAP_PORTOAMLOOPBACKPARACTION_SET:
            rtk_copy_from(&buf->oam_trap_cfg, user, sizeof(rtdrv_trapOamCfg_t));
            ret = rtk_trap_portOamLoopbackParAction_set(buf->oam_trap_cfg.unit,
                    buf->oam_trap_cfg.port, buf->oam_trap_cfg.action);
            break;

        case RTDRV_TRAP_ROUTEEXCEPTIONACTION_SET:
            rtk_copy_from(&buf->routeException_trap_cfg, user,
                    sizeof(rtdrv_trapRouteExceptionCfg_t));
            ret = rtk_trap_routeExceptionAction_set(
                    buf->routeException_trap_cfg.unit,
                    buf->routeException_trap_cfg.type,
                    buf->routeException_trap_cfg.action);
            break;

        case RTDRV_TRAP_ROUTEEXCEPTIONPRI_SET:
            rtk_copy_from(&buf->routeException_trap_cfg, user,
                    sizeof(rtdrv_trapRouteExceptionCfg_t));
            ret = rtk_trap_routeExceptionPri_set(
                    buf->routeException_trap_cfg.unit,
                    buf->routeException_trap_cfg.type,
                    buf->routeException_trap_cfg.priority);
            break;

        case RTDRV_TRAP_USERDEFINERMALEARNINGENABLE_SET:
            rtk_copy_from(&buf->mgmuser_trap_cfg, user,
                    sizeof(rtdrv_trapUserMgmRmaCfg_t));
            ret = rtk_trap_userDefineRmaLearningEnable_set(
                    buf->mgmuser_trap_cfg.unit, buf->mgmuser_trap_cfg.mgmt_idx,
                    buf->mgmuser_trap_cfg.enable);
            break;

        case RTDRV_TRAP_RMALEARNINGENABLE_SET:
            rtk_copy_from(&buf->trap_cfg, user,
                    sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaLearningEnable_set(
                    buf->trap_cfg.unit, &buf->trap_cfg.rma_frame,
                    buf->trap_cfg.enable);
            break;

        case RTDRV_TRAP_MGMTFRAMELEARNINGENABLE_SET:
            rtk_copy_from(&buf->mgm_trap_cfg, user,
                    sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFrameLearningEnable_set(buf->mgm_trap_cfg.unit,
                    buf->mgm_trap_cfg.frameType, buf->mgm_trap_cfg.enable);
            break;

        case RTDRV_TRAP_MGMTFRAMEMGMTVLANENABLE_SET:
            rtk_copy_from(&buf->other_trap_cfg, user,
                    sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_mgmtFrameMgmtVlanEnable_set(
                    buf->other_trap_cfg.unit, buf->other_trap_cfg.enable);
            break;

        case RTDRV_TRAP_BPDUFLOODPORTMASK_SET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_bpduFloodPortmask_set(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            break;

        case RTDRV_TRAP_EAPOLFLOODPORTMASK_SET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_eapolFloodPortmask_set(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            break;

        case RTDRV_TRAP_LLDPFLOODPORTMASK_SET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_lldpFloodPortmask_set(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            break;

        case RTDRV_TRAP_USERDEFINEFLOODPORTMASK_SET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_userDefineFloodPortmask_set(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            break;

        case RTDRV_TRAP_RMAFLOODPORTMASK_SET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_rmaFloodPortmask_set(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            break;

        case RTDRV_TRAP_RMACANCELMIRROR_SET:
            rtk_copy_from(&buf->trap_cfg, user,sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaCancelMirror_set(buf->trap_cfg.unit, buf->trap_cfg.enable);
            break;

        case RTDRV_TRAP_RMAGROUPACTION_SET:
            rtk_copy_from(&buf->rma_grp_act_cfg, user,
                    sizeof(rtdrv_rmaGroupType_t));
            ret = rtk_trap_rmaGroupAction_set(
                    buf->rma_grp_act_cfg.unit, buf->rma_grp_act_cfg.rmaGroup_frameType, buf->rma_grp_act_cfg.rma_action);
            break;

        case RTDRV_TRAP_RMAGROUPLEARNINGENABLE_SET:
            rtk_copy_from(&buf->rma_grp_lrn_cfg, user,
                    sizeof(rtdrv_rmaGroupLearn_t));
            ret = rtk_trap_rmaGroupLearningEnable_set(
                    buf->rma_grp_lrn_cfg.unit, buf->rma_grp_lrn_cfg.rmaGroup_frameType, buf->rma_grp_lrn_cfg.enable);
            break;

        case RTDRV_TRAP_MGMTFRAMESELFARPENABLE_SET:
            rtk_copy_from(&buf->other_trap_cfg, user,
                    sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_mgmtFrameSelfARPEnable_set(
                    buf->other_trap_cfg.unit, buf->other_trap_cfg.enable);
            break;

        case RTDRV_TRAP_RMALOOKUPMISSACTIONENABLE_SET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaLookupMissActionEnable_set(buf->trap_cfg.unit, buf->trap_cfg.enable);
            break;

        case RTDRV_TRAP_CFMACT_SET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cfmAct_set(buf->cfm_trap_cfg.unit, buf->cfm_trap_cfg.type,
                    buf->cfm_trap_cfg.md_level, buf->cfm_trap_cfg.action);
            break;

        case RTDRV_TRAP_CFMTARGET_SET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cfmTarget_set(buf->cfm_trap_cfg.unit, buf->cfm_trap_cfg.target);
            break;

        case RTDRV_TRAP_OAMTARGET_SET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_oamTarget_set(buf->trap_cfg.unit, buf->trap_cfg.target);
            break;

        case RTDRV_TRAP_MGMTFRAMETARGET_SET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_mgmtFrameTarget_set(buf->trap_cfg.unit, buf->trap_cfg.target);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_acl(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** ACL **/
        case RTDRV_ACL_ENTRY_DATA_WRITE:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryField_write(
                    buf->acl_cfg.unit,
                    buf->acl_cfg.phase,
                    buf->acl_cfg.index,
                    buf->acl_cfg.field_type,
                    buf->acl_cfg.field_data,
                    buf->acl_cfg.field_mask);
            break;

        case RTDRV_ACL_RULEVALIDATE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleValidate_set(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.status);
            break;

        case RTDRV_ACL_RULEENTRY_WRITE:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntry_write(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.entry_buffer);
            break;

        case RTDRV_ACL_RULEENTRYFIELD_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryField_set(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.entry_buffer,
                    buf->acl_cfg.field_type, buf->acl_cfg.field_data,
                    buf->acl_cfg.field_mask);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEOPERATION_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleOperation_set(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, &buf->acl_cfg.oper);
            break;

        case RTDRV_ACL_RULEACTION_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleAction_set(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, &buf->acl_cfg.action);
            break;

        case RTDRV_ACL_BLOCKPWRENABLE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_blockPwrEnable_set(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, buf->acl_cfg.status);
            break;

        case RTDRV_ACL_BLOCKAGGREGATORENABLE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_blockGroupEnable_set(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, buf->acl_cfg.blk_group,
                    buf->acl_cfg.status);
            break;

        case RTDRV_ACL_STATPKTCNT_CLEAR:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_statPktCnt_clear(buf->acl_cfg.unit, buf->acl_cfg.index);
            break;

        case RTDRV_ACL_STATBYTECNT_CLEAR:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_statByteCnt_clear(buf->acl_cfg.unit, buf->acl_cfg.index);
            break;

        case RTDRV_ACL_STAT_CLEARALL:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_stat_clearAll(buf->acl_cfg.unit);
            break;

        case RTDRV_ACL_RANGECHECKL4PORT_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckL4Port_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_l4Port);
            break;

        case RTDRV_ACL_RANGECHECKVID_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckVid_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_vid);
            break;

        case RTDRV_ACL_METER_MODE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_meterMode_set(buf->acl_cfg.unit, buf->acl_cfg.blockIdx, buf->acl_cfg.meterMode);
            break;

        case RTDRV_ACL_METER_BURST_SIZE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_meterBurstSize_set(buf->acl_cfg.unit,
                    buf->acl_cfg.meterMode, &buf->acl_cfg.burstSize);
            break;

        case RTDRV_ACL_RANGECHECKSRCPORT_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckSrcPort_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_port);
            break;

        case RTDRV_ACL_RANGECHECKPACKETLEN_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckPacketLen_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_pktLen);
            break;
        case RTDRV_ACL_LOOPBACKENABLE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_loopBackEnable_set(buf->acl_cfg.unit, buf->acl_cfg.enable);
            break;
        case RTDRV_ACL_LIMITLOOPBACKTIMES_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_limitLoopbackTimes_set(buf->acl_cfg.unit, buf->acl_cfg.lb_times);
            break;
        case RTDRV_ACL_PORTLOOKUPENABLE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_portLookupEnable_set(buf->acl_cfg.unit,
                    buf->acl_cfg.port, buf->acl_cfg.status);
            break;

        case RTDRV_ACL_LOOKUPMISSACT_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_lookupMissAct_set(buf->acl_cfg.unit,
                    buf->acl_cfg.port, buf->acl_cfg.lmAct);
            break;

        case RTDRV_ACL_RANGECHECKFIELDSEL_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckFieldSelector_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_fieldSel);
            break;
        case RTDRV_ACL_PARTITION_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_partition_set(buf->acl_cfg.unit, buf->acl_cfg.blockIdx);
            break;

        case RTDRV_ACL_TEMPLATEFIELDINTENTVLANTAG_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_templateFieldIntentVlanTag_set(buf->acl_cfg.unit, buf->acl_cfg.tagType);
            break;

        case RTDRV_ACL_RANGECHECKDSTPORT_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckDstPort_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_port);
            break;

        case RTDRV_ACL_BLOCKRESULTMODE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_blockResultMode_set(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, buf->acl_cfg.blk_mode);
            break;

        case RTDRV_ACL_PORTPHASELOOKUPENABLE_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_portPhaseLookupEnable_set(buf->acl_cfg.unit,
                    buf->acl_cfg.port, buf->acl_cfg.phase, buf->acl_cfg.status);
            break;

        case RTDRV_ACL_TEMPLATESELECTOR_SET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_templateSelector_set(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, buf->acl_cfg.template_idx);
            break;

        case RTDRV_ACL_STATCNT_CLEAR:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_statCnt_clear(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.mode);
            break;

        case RTDRV_ACL_RULE_DEL:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_rule_del(buf->acl_cfg.unit, buf->acl_cfg.phase, &buf->acl_cfg.clrIdx);
            break;

        case RTDRV_ACL_RULE_MOVE:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_rule_move(buf->acl_cfg.unit, buf->acl_cfg.phase, &buf->acl_cfg.mv);
            break;

        case RTDRV_ACL_RULEENTRYFIELD_VALIDATE:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryField_validate(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.field_type);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_pie(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** PIE **/
        case RTDRV_PIE_METER_INCLUDE_IFG_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterIncludeIfg_set(buf->pie_cfg.unit, buf->pie_cfg.ifg_include);
            break;

        case RTDRV_PIE_METER_ENTRY_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterEntry_set(buf->pie_cfg.unit, buf->pie_cfg.meterIdx, &buf->pie_cfg.meterEntry);
            break;

        case RTDRV_PIE_BLOCKLOOKUPENABLE_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_blockLookupEnable_set(buf->pie_cfg.unit,
                    buf->pie_cfg.blockIdx, buf->pie_cfg.status);
            break;

        case RTDRV_PIE_BLOCKGROUPING_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_blockGrouping_set(buf->pie_cfg.unit,
                    buf->pie_cfg.blockIdx, buf->pie_cfg.grpId, buf->pie_cfg.logicId);
            break;

        case RTDRV_PIE_TEMPLATE_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_template_set(buf->pie_cfg.unit, buf->pie_cfg.index,
                    &buf->pie_cfg.template);
            break;

        case RTDRV_PIE_RANGECHECKIP_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_pie_rangeCheckIp_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_ip);
            break;

        case RTDRV_PIE_RANGECHECK_SET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_pie_rangeCheck_set(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range);
            break;

        case RTDRV_PIE_FIELDSELECTOR_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_fieldSelector_set(buf->pie_cfg.unit,
                    buf->pie_cfg.index, &buf->pie_cfg.fs);
            break;

        case RTDRV_PIE_PHASE_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_phase_set(buf->pie_cfg.unit, buf->pie_cfg.blockIdx, buf->pie_cfg.phase);
            break;

        case RTDRV_PIE_TEMPLATE_VLANSEL_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_templateVlanSel_set(buf->pie_cfg.unit, buf->pie_cfg.phase, buf->pie_cfg.templateIdx, buf->pie_cfg.vlanSel);
            break;

        case RTDRV_PIE_METER_DPSEL_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterDpSel_set(buf->pie_cfg.unit, buf->pie_cfg.dpSel);
            break;

        case RTDRV_PIE_ARPMACSEL_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_arpMacSel_set(buf->pie_cfg.unit, buf->pie_cfg.arpMacSel);
            break;

        case RTDRV_PIE_INTFSEL_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_intfSel_set(buf->pie_cfg.unit, buf->pie_cfg.intfSel);
            break;

        case RTDRV_PIE_TEMPLATEVLANFMTSEL_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_templateVlanFmtSel_set(buf->pie_cfg.unit, buf->pie_cfg.phase, buf->pie_cfg.templateIdx, buf->pie_cfg.vlanFmtSel);
            break;

        case RTDRV_PIE_METERTRTCMTYPE_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterTrtcmType_set(buf->pie_cfg.unit, buf->pie_cfg.type);
            break;

        case RTDRV_PIE_FILTER1BR_SET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_filter1BR_set(buf->pie_cfg.unit, buf->pie_cfg.status);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_qos(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** QOS **/
        case RTDRV_QOS_QUEUE_NUM_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_queueNum_set(buf->qos_cfg.unit, buf->qos_cfg.queue_num);
            break;

        case RTDRV_QOS_PRI_MAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priMap_set(buf->qos_cfg.unit, buf->qos_cfg.queue_num, &buf->qos_cfg.pri2qid);
            break;

        case RTDRV_QOS_PRI2QID_MAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_pri2QidMap_set(buf->qos_cfg.unit, buf->qos_cfg.int_pri, buf->qos_cfg.queue);
            break;

        case RTDRV_QOS_CPUQID2QID_MAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_cpuQid2QidMap_set(buf->qos_cfg.unit, buf->qos_cfg.cpuQid, buf->qos_cfg.queue);
            break;

        case RTDRV_QOS_CPUQID2SQID_MAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_cpuQid2StackQidMap_set(buf->qos_cfg.unit, buf->qos_cfg.cpuQid, buf->qos_cfg.queue);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_1P_PRI_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pPriRemap_set(buf->qos_cfg.unit, buf->qos_cfg.dot1p_pri, buf->qos_cfg.int_pri);
            break;

        case RTDRV_QOS_OUTER_1P_PRI_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pPriRemap_set(buf->qos_cfg.unit, buf->qos_cfg.dot1p_pri, buf->qos_cfg.dei, buf->qos_cfg.int_pri);
            break;
#endif

        case RTDRV_QOS_DEI_DP_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiDpRemap_set(buf->qos_cfg.unit, buf->qos_cfg.dei, buf->qos_cfg.dp);
            break;

        case RTDRV_QOS_PORT_DEI_SRC_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDEISrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.deiSrc);
            break;

        case RTDRV_QOS_PORT_DP_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDpSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, &(buf->qos_cfg.weightOfDpSel));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_DSCP_DP_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpDpRemap_set(buf->qos_cfg.unit, buf->qos_cfg.dscp, buf->qos_cfg.dp);
            break;
#endif
        case RTDRV_QOS_DP_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dpRemap_set(buf->qos_cfg.unit, buf->qos_cfg.dpSrcType, buf->qos_cfg.dpSrcRemap, buf->qos_cfg.dp);
            break;

        case RTDRV_QOS_PRI_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priRemap_set(buf->qos_cfg.unit, buf->qos_cfg.priSrcType, buf->qos_cfg.priSrcRemap, buf->qos_cfg.int_pri);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_DSCP_PRI_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpPriRemap_set(buf->qos_cfg.unit, buf->qos_cfg.dscp, buf->qos_cfg.int_pri);
            break;

        case RTDRV_QOS_PORT_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.int_pri);
            break;

        case RTDRV_QOS_PORT_INNER_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portInnerPri_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.int_pri);
            break;

        case RTDRV_QOS_PORT_OUTER_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuterPri_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.int_pri);
            break;
#endif

        case RTDRV_QOS_DP_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dpSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.dpSrcType);
            break;

        case RTDRV_QOS_PRI_SEL_GROUP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priSelGroup_set(buf->qos_cfg.unit, buf->qos_cfg.index, &(buf->qos_cfg.priSelWeight));
            break;

        case RTDRV_QOS_PORT_PRI_SEL_GROUP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriSelGroup_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.index);
            break;

        case RTDRV_QOS_PORT_1P_REMARK_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pRemarkEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.remark_enable);
            break;

        case RTDRV_QOS_1P_REMARKING_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarking_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_1p, buf->qos_cfg.rmkval_1p, buf->qos_cfg.dot1p_pri);
            break;

        case RTDRV_QOS_1P_REMARK_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarkSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_1p);
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPri_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.dot1p_dflt_pri);
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_EXT_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPriExt_set(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, buf->qos_cfg.dot1p_dflt_pri);
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPriSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.dflt_src_1p);
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_EXT_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPriSrcSelExt_set(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, buf->qos_cfg.dflt_src_1p);
            break;

        case RTDRV_QOS_1P_DFLT_PRI_CFG_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pDfltPriCfgSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.dot1p_dflt_cfg_dir);
            break;

        case RTDRV_QOS_PORT_OUT_1P_REMARK_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOut1pRemarkEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.remark_enable);
            break;

        case RTDRV_QOS_OUTER_1P_REMARKING_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pRemarking_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_outer1p, buf->qos_cfg.rmkval_outer1p, buf->qos_cfg.dot1p_pri);
            break;

        case RTDRV_QOS_OUT_1P_REMARK_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pRemarkSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_outer1p);
            break;

        case RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPri_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.out1p_dflt_pri);
            break;

        case RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_EXT_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPriExt_set(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, buf->qos_cfg.out1p_dflt_pri);
            break;

        case RTDRV_QOS_OUTER_1P_DFLT_PRI_CFG_SRC_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pDfltPriCfgSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.out1p_dflt_cfg_dir);
            break;

        case RTDRV_QOS_PORT_DSCP_REMARK_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDscpRemarkEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.remark_enable);
            break;

        case RTDRV_QOS_DSCP_REMARKING_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarking_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_dscp, buf->qos_cfg.rmkval_dscp, buf->qos_cfg.dscp);
            break;

        case RTDRV_QOS_DSCP_REMARK_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarkSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_dscp);
            break;

        case RTDRV_QOS_PORT_DEI_REMARK_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDeiRemarkEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.remark_enable);
            break;

        case RTDRV_QOS_DEI_REMARKING_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiRemarking_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_dei, buf->qos_cfg.rmkval_dei, buf->qos_cfg.dei);
            break;

        case RTDRV_QOS_DEI_REMARK_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiRemarkSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_dei);
            break;

        case RTDRV_QOS_PORT_DEI_REMARK_TAG_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDeiRemarkTagSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.deiSrc);
            break;

        case RTDRV_QOS_1P_DFLT_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pDfltPri_set(buf->qos_cfg.unit, buf->qos_cfg.dot1p_dflt_pri);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_1P_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemark_set(buf->qos_cfg.unit, buf->qos_cfg.int_pri, buf->qos_cfg.dot1p_pri);
            break;

        case RTDRV_QOS_OUT_1P_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pRemark_set(buf->qos_cfg.unit, buf->qos_cfg.int_pri, buf->qos_cfg.dot1p_pri);
            break;
#endif

        case RTDRV_QOS_PORT_OUT_1P_DFLT_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPriSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.out1p_dflt_src);
            break;

        case RTDRV_QOS_PORT_OUT_1P_DFLT_SRC_SEL_EXT_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPriSrcSelExt_set(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, buf->qos_cfg.out1p_dflt_src);
            break;

        case RTDRV_QOS_1P_DFLT_PRI_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pDfltPriSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.dflt_src_1p);
            break;

        case RTDRV_QOS_OUTER_1P_DFLT_PRI_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pDfltPri_set(buf->qos_cfg.unit, buf->qos_cfg.out1p_dflt_pri);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_DSCP_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemark_set(buf->qos_cfg.unit, buf->qos_cfg.int_pri, buf->qos_cfg.dscp);
            break;

        case RTDRV_QOS_DSCP2DSCP_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2DscpRemark_set(buf->qos_cfg.unit, buf->qos_cfg.org_dscp, buf->qos_cfg.dscp);
            break;

        case RTDRV_QOS_DSCP2DOT1P_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2Dot1pRemark_set(buf->qos_cfg.unit, buf->qos_cfg.org_dscp, buf->qos_cfg.dot1p_pri);
            break;

        case RTDRV_QOS_DSCP2OUT1P_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2Outer1pRemark_set(buf->qos_cfg.unit, buf->qos_cfg.org_dscp, buf->qos_cfg.dot1p_pri);
            break;

        case RTDRV_QOS_DEI_REMARK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiRemark_set(buf->qos_cfg.unit, buf->qos_cfg.dp, buf->qos_cfg.dei);
            break;
#endif

        case RTDRV_QOS_PORT_OUTER_1P_REMARK_SRC_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pRemarkSrcSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.rmksrc_outer1p);
            break;

        case RTDRV_QOS_SCHEDULING_ALGORITHM_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingAlgorithm_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.scheduling_type);
            break;

        case RTDRV_QOS_SCHEDULING_QUEUE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingQueue_set(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.qweights);
            break;

        case RTDRV_QOS_CONG_AVOID_ALGO_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidAlgo_set(buf->qos_cfg.unit, buf->qos_cfg.congAvoid_algo);
            break;

        case RTDRV_QOS_PORT_CONG_AVOID_ALGO_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portCongAvoidAlgo_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.congAvoid_algo);
            break;

        case RTDRV_QOS_CONG_AVOID_SYS_THRESH_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidSysThresh_set(buf->qos_cfg.unit, buf->qos_cfg.dp, &(buf->qos_cfg.congAvoid_thresh));
            break;

        case RTDRV_QOS_CONG_AVOID_SYS_DROP_PROBABILITY_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidSysDropProbability_set(buf->qos_cfg.unit, buf->qos_cfg.dp, buf->qos_cfg.data);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_THRESH_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidGlobalQueueThresh_set(buf->qos_cfg.unit, buf->qos_cfg.queue, buf->qos_cfg.dp, &(buf->qos_cfg.congAvoid_thresh));
            break;

        case RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_DROP_PROBABILITY_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidGlobalQueueDropProbability_set(buf->qos_cfg.unit, buf->qos_cfg.queue, buf->qos_cfg.dp, buf->qos_cfg.data);
            break;
#endif

        case RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_CONFIG_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidGlobalQueueConfig_set(buf->qos_cfg.unit, buf->qos_cfg.queue, buf->qos_cfg.dp, &(buf->qos_cfg.congAvoid_thresh));
            break;

        case RTDRV_QOS_AVB_SR_CLASS_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portAvbStreamReservationClassEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.srClass, buf->qos_cfg.enable);
            break;

        case RTDRV_QOS_AVB_SR_CONFIG_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_avbStreamReservationConfig_set(buf->qos_cfg.unit, &buf->qos_cfg.srConf);
            break;

        case RTDRV_QOS_PKT2CPU_PRI_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_pkt2CpuPriRemap_set(buf->qos_cfg.unit, buf->qos_cfg.int_pri, buf->qos_cfg.new_pri);
            break;

        case RTDRV_QOS_RSPAN_PRI_REMAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_rspanPriRemap_set(buf->qos_cfg.unit, buf->qos_cfg.rspan_pri, buf->qos_cfg.int_pri);
            break;

        case RTDRV_QOS_PRI2IGR_QUEUE_MAP_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri2IgrQMap_set(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.pri2qid);
            break;

        case RTDRV_QOS_PRI2IGR_QUEUE_MAP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri2IgrQMapEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.enable);
            break;

        case RTDRV_QOS_PORT_IGR_QUEUE_WEIGHT_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portIgrQueueWeight_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.queue, buf->qos_cfg.data);
            break;

        case RTDRV_QOS_INVLD_DSCP_VAL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_invldDscpVal_set(buf->qos_cfg.unit, buf->qos_cfg.dscp);
            break;

        case RTDRV_QOS_INVLD_DSCP_MASK_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_invldDscpMask_set(buf->qos_cfg.unit, buf->qos_cfg.dscp);
            break;

        case RTDRV_QOS_PORT_INVLD_DSCP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portInvldDscpEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.enable);
            break;

        case RTDRV_QOS_INVLD_DSCP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_invldDscpEnable_set(buf->qos_cfg.unit, buf->qos_cfg.enable);
            break;
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_SYS_PORT_PRI_REMAP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriRemapEnable_set(buf->qos_cfg.unit, buf->qos_cfg.enable);
            break;
#endif

        case RTDRV_QOS_SYS_PORT_PRI_REMAP_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_sysPortPriRemapSel_set(buf->qos_cfg.unit, buf->qos_cfg.portPriRemap_type);
            break;

        case RTDRV_QOS_PORT_PORT_PRI_REMAP_SEL_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPortPriRemapSel_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.portPriRemap_type);
            break;

        case RTDRV_QOS_PORT_IPRI_REMAP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portInnerPriRemapEnable_set(buf->qos_cfg.unit, buf->qos_cfg.enable);
            break;

        case RTDRV_QOS_PORT_OPRI_REMAP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuterPriRemapEnable_set(buf->qos_cfg.unit, buf->qos_cfg.enable);
            break;

        case RTDRV_QOS_PRI_REMAP_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priRemapEnable_set(buf->qos_cfg.unit, buf->qos_cfg.priSrcType, buf->qos_cfg.enable);
            break;

        case RTDRV_QOS_PORT_QUEUE_STRICT_ENABLE_SET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portQueueStrictEnable_set(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.queue, buf->qos_cfg.enable);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_trunk(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** TRUNK **/
        case RTDRV_TRUNK_MODE_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_mode_set(buf->trunk_cfg.unit, buf->trunk_cfg.mode);
            break;

        case RTDRV_TRUNK_PORT_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_port_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, &buf->trunk_cfg.trk_member);
            break;

        case RTDRV_TRUNK_LOCAL_PORT_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_localPort_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, &buf->trunk_cfg.trk_member);
            break;

        case RTDRV_TRUNK_EGR_PORT_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_egrPort_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, &buf->trunk_cfg.trk_egr_ports);
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_BIND_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmBind_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, buf->trunk_cfg.algo_id);
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_BIND_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmTypeBind_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, buf->trunk_cfg.bindType, buf->trunk_cfg.algo_id);
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_PARAM_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmParam_set(buf->trunk_cfg.unit, buf->trunk_cfg.algo_id, buf->trunk_cfg.algo_bitmask);
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_PARAM_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmTypeParam_set(buf->trunk_cfg.unit, buf->trunk_cfg.paramType, buf->trunk_cfg.algo_id, buf->trunk_cfg.algo_bitmask);
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmShift_set(buf->trunk_cfg.unit, buf->trunk_cfg.algo_id, &buf->trunk_cfg.shift);
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_GBL_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmShiftGbl_set(buf->trunk_cfg.unit, &buf->trunk_cfg.shift);
            break;

        case RTDRV_TRUNK_TRAFFIC_SEPARATE_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparate_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, buf->trunk_cfg.separate);
            break;

        case RTDRV_TRUNK_TRAFFIC_SEPARATE_ENABLE_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparateEnable_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, buf->trunk_cfg.separate, buf->trunk_cfg.enable);
            break;

        case RTDRV_TRUNK_TRAFFIC_SEPARATE_DIVISION_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparateDivision_set(buf->trunk_cfg.unit, buf->trunk_cfg.enable);
            break;

        case RTDRV_TRUNK_TUNNEL_HASH_SRC_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_tunnelHashSrc_set(buf->trunk_cfg.unit, buf->trunk_cfg.tunnelHashSrc);
            break;

        case RTDRV_TRUNK_STACK_TRUNK_PORT_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_stkTrkPort_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid,  &buf->trunk_cfg.trk_member);
            break;

        case RTDRV_TRUNK_STACK_TRUNK_HASH_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_stkTrkHash_set(buf->trunk_cfg.unit, buf->trunk_cfg.stkTrkHash);
            break;

        case RTDRV_TRUNK_STACK_DIST_ALGO_TYPE_BIND_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_stkDistributionAlgorithmTypeBind_set(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid,
                buf->trunk_cfg.bindType, buf->trunk_cfg.algo_id);
            break;

        case RTDRV_TRUNK_LOCALFIRST_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_localFirst_set(buf->trunk_cfg.unit, buf->trunk_cfg.localFirst);
            break;

        case RTDRV_TRUNK_LOCALFIRSTFAILOVER_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_localFirstFailOver_set(buf->trunk_cfg.unit, buf->trunk_cfg.congstAvoid, buf->trunk_cfg.linkFailAvoid);
            break;

        case RTDRV_TRUNK_SRCPORTMAP_SET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_srcPortMap_set(buf->trunk_cfg.unit, buf->trunk_cfg.devPort, buf->trunk_cfg.isTrkMbr, buf->trunk_cfg.trk_gid);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_debug(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** DEBUG **/
        case RTDRV_DEBUG_EN_LOG_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_enable_set(buf->unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGLV_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_level_set(buf->unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGLVMASK_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_mask_set(buf->unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGTYPE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_type_set(buf->unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGFORMAT_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_format_set(buf->unit_cfg.data);
            break;

        case RTDRV_DEBUG_MODMASK_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_moduleMask_set(buf->unit_cfg.data64);
            break;

        case RTDRV_DEBUG_MEM_WRITE:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = debug_mem_write(buf->reg_cfg.unit, buf->reg_cfg.reg, buf->reg_cfg.value);
            break;

        case RTDRV_DEBUG_MEM_SHOW:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = debug_mem_show(buf->reg_cfg.unit);
            break;

        case RTDRV_DEBUG_MEM_BASEADDR_SHOW:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = debug_mem_baseAddr_show(buf->reg_cfg.unit);
            break;

        case RTDRV_DEBUG_HSB_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsb(buf->unit_cfg.unit);
            break;

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
        case RTDRV_DEBUG_HSM_IDX_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsmIdx(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
        case RTDRV_DEBUG_HSM_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsm(buf->unit_cfg.unit);
            break;
#endif

        case RTDRV_DEBUG_HSA_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsa(buf->unit_cfg.unit);
            break;

#if defined(CONFIG_SDK_RTL9310)
        case RTDRV_DEBUG_HSB_OPENFLOW_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsb_openflow(buf->unit_cfg.unit);
            break;
        case RTDRV_DEBUG_HSM_OPENFLOW_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsm_openflow(buf->unit_cfg.unit);
            break;
        case RTDRV_DEBUG_HSA_OPENFLOW_DUMP:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_dumpHsa_openflow(buf->unit_cfg.unit);
            break;
        case RTDRV_DEBUG_REPCTQ_STICK_ENABLE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_repctQueueStickEnable_set(buf->unit_cfg.unit, buf->unit_cfg.enable);
            break;
        case RTDRV_DEBUG_REPCTQ_FETCH_ENABLE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_repctQueueFetchEnable_set(buf->unit_cfg.unit, buf->unit_cfg.enable);
            break;
        case RTDRV_DEBUG_FLOWCTRL_RPECT_QUEUE_USED_PAGE_CNT_RESET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_resetFlowCtrlRepctQueueUsedPageCnt(buf->unit_cfg.unit);
            break;
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
        case RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_INGRESS_RESET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_resetFlowCtrlIgrPortUsedPageCnt(buf->unit_cfg.unit, buf->unit_cfg.port);
            break;
        case RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_EGRESS_RESET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_resetFlowCtrlEgrPortUsedPageCnt(buf->unit_cfg.unit, buf->unit_cfg.port);
            break;
        case RTDRV_DEBUG_FLOWCTRL_SYSTEM_USED_PAGE_CNT_RESET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_resetFlowCtrlSystemUsedPageCnt(buf->unit_cfg.unit);
            break;
#endif

        case RTDRV_DEBUG_SDS_RXCALI_ENABLE_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = mac_debug_sds_rxCaliEnable_set(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.en);
            break;
        case RTDRV_DEBUG_SDS_RXCALI_START:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = mac_debug_sds_rxCali(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.cnt);
            break;
        case RTDRV_DEBUG_SDS_RXCALI_DEBUG_ENABLE_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = mac_debug_sds_rxCali_debugEnable_set(buf->sds_cfg.en);
            break;

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)|| defined(CONFIG_SDK_RTL9310)
        case RTDRV_DEBUG_WATCHDOG_MON_ENABLE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_setWatchdogMonitorEnable(buf->unit_cfg.enable);
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_uart(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_UART1)
        case RTDRV_UART1_PUTC:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = drv_uart_putc(buf->unit_cfg.unit, buf->unit_cfg.data8);
            break;

        case RTDRV_UART1_BAUDRATE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = drv_uart_baudrate_set(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;
        case RTDRV_UART1_INTERFACE_SET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = drv_uart_interface_set(buf->unit_cfg.unit, buf->unit_cfg.data);
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_mirror(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** MIRROR **/
        case RTDRV_MIRROR_GROUP_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_group_set(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.mirrorEntry);
            break;

        case RTDRV_MIRROR_RSPAN_IGR_MODE_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_rspanIgrMode_set(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, buf->mirror_cfg.data);
            break;

        case RTDRV_MIRROR_RSPAN_EGR_MODE_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_rspanEgrMode_set(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, buf->mirror_cfg.data);
            break;

        case RTDRV_MIRROR_RSPAN_TAG_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_rspanTag_set(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.rspan_tag);
            break;

        case RTDRV_MIRROR_SFLOW_MIRROR_SAMPLE_RATE_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowMirrorSampleRate_set(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, buf->mirror_cfg.data);
            break;

        case RTDRV_MIRROR_EGRQUEUE_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_egrQueue_set(buf->mirror_cfg.unit, buf->mirror_cfg.enable, buf->mirror_cfg.qid);
            break;

        case RTDRV_MIRROR_SFLOW_PORT_IGR_SAMPLE_RATE_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowPortIgrSampleRate_set(buf->mirror_cfg.unit, buf->mirror_cfg.port, buf->mirror_cfg.data);
            break;

        case RTDRV_MIRROR_SFLOW_PORT_EGR_SAMPLE_RATE_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowPortEgrSampleRate_set(buf->mirror_cfg.unit, buf->mirror_cfg.port, buf->mirror_cfg.data);
            break;

        case RTDRV_MIRROR_SFLOW_SAMPLE_CTRL_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowSampleCtrl_set(buf->mirror_cfg.unit, buf->mirror_cfg.sample_ctrl);
            break;

        case RTDRV_MIRROR_SFLOWSAMPLETARGET_SET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowSampleTarget_set(buf->mirror_cfg.unit, buf->mirror_cfg.target);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_stack(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** STACK **/
        case RTDRV_STACK_INIT:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_init(buf->stack_cfg.unit);
            break;

        case RTDRV_STACK_PORT_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_port_set(buf->stack_cfg.unit, &buf->stack_cfg.stkPorts);
            break;

        case RTDRV_STACK_UNIT_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_devId_set(buf->stack_cfg.unit, buf->stack_cfg.myDevID);
            break;

        case RTDRV_STACK_MASTERUNIT_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_masterDevId_set(buf->stack_cfg.unit, buf->stack_cfg.masterDevID);
            break;

        case RTDRV_STACK_LOOPGUARD_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_loopGuard_set(buf->stack_cfg.unit, buf->stack_cfg.enable);
            break;

        case RTDRV_STACK_UNITPORTMAP_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_devPortMap_set(buf->stack_cfg.unit, buf->stack_cfg.dstDevID, &buf->stack_cfg.stkPorts);
            break;

        case RTDRV_STACK_NONUCASTBLOCKPORT_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_nonUcastBlockPort_set(buf->stack_cfg.unit, buf->stack_cfg.srcDevID, &buf->stack_cfg.stkPorts);
            break;

        case RTDRV_STACK_RMTINTRTXENABLE_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrTxEnable_set(buf->stack_cfg.unit, buf->stack_cfg.enable);
            break;

        case RTDRV_STACK_RMTINTRTXTRIGGERENABLE_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrTxTriggerEnable_set(buf->stack_cfg.unit, buf->stack_cfg.enable);
            break;

        case RTDRV_STACK_RMTINTRRXSEQCMPMARGIN_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrRxSeqCmpMargin_set(buf->stack_cfg.unit, buf->stack_cfg.margin);
            break;

        case RTDRV_STACK_RMTINTRRXFORCEUPDATEENABLE_SET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrRxForceUpdateEnable_set(buf->stack_cfg.unit, buf->stack_cfg.enable);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_flowctrl(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** FLOWCTRL **/
        case RTDRV_FLOWCTRL_INIT:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_init(buf->flowctrl_cfg.unit);
            break;

        case RTDRV_FLOWCTRL_PORT_PAUSEON_ACTION_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portPauseOnAction_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.pauseOn_action);
            break;

        case RTDRV_FLOWCTRL_PORT_PAUSEON_ALLOWED_PAGENUM_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portPauseOnAllowedPageNum_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.data);
            break;

        case RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTLEN_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_pauseOnAllowedPktLen_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.data);
            break;

        case RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTNUM_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_pauseOnAllowedPktNum_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.data);
            break;

        case RTDRV_FLOWCTRL_IGR_GUAR_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrGuarEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_IGR_SYS_PAUSE_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrSystemPauseThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_IGR_JUMBO_SYS_PAUSE_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrJumboSystemPauseThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_IGR_PAUSE_THR_GROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrPauseThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_PORT_IGR_PORT_THR_GROUP_SEL_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portIgrPortThreshGroupSel_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.grp_idx);
            break;

        case RTDRV_FLOWCTRL_IGR_SYS_CONGEST_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrSystemCongestThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_IGR_JUMBO_SYS_CONGEST_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrJumboSystemCongestThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_IGR_CONGEST_THR_GROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrCongestThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_JUMBO_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_jumboModeEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_JUMBO_LEN_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_jumboModeLength_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.data);
            break;

        case RTDRV_FLOWCTRL_EGR_SYS_UTIL_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrSystemUtilThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            break;

        case RTDRV_FLOWCTRL_EGR_SYS_DROP_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrSystemDropThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortDropThresh_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_DROP_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortDropEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_QUEUE_DROP_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortQueueDropEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrQueueDropThresh_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_EGR_CPU_QUEUE_DROP_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrCpuQueueDropThresh_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_PORT_EGR_DROP_REFCONGEST_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portEgrDropRefCongestEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_GROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortDropThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_GROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrQueueDropThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_PORT_EGR_DROP_THR_GROUP_SEL_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portEgrDropThreshGroupSel_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.grp_idx);
            break;

        case RTDRV_FLOWCTRL_EGR_QUEUE_DROP_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrQueueDropEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.queue, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_PORT_EGR_QUEUE_DROP_FORCE_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portEgrQueueDropForceEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.queue, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_DROP_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueueDropEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.queue, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_DROP_THR_GROUP_SEL_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.grp_idx);
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_GROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueueDropThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, buf->flowctrl_cfg.queue,&buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_THR_GROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueuePauseThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueueDropThresh_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.queue,&buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_PORT_HOL_TRAFFIC_DROP_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portHolTrafficDropEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_HOL_TRAFFIC_TYPE_DROP_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_holTrafficTypeDropEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.type, buf->flowctrl_cfg.enable);
            break;

        case RTDRV_FLOWCTRL_SPECIAL_CONGEST_THR_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_specialCongestThreshold_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_E2E_CASCADE_PORT_THRESH_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_e2eCascadePortThresh_set(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_E2E_REMOTE_PORTPAUSETHRESHGROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_e2eRemotePortPauseThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_E2E_REMOTE_PORTCONGESTTHRESHGROUP_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_e2eRemotePortCongestThreshGroup_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.dropThresh);
            break;

        case RTDRV_FLOWCTRL_E2E_PORT_REMOTE_PORT_THRESH_GROUP_SEL_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portE2eRemotePortThreshGroupSel_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.grp_idx);
            break;

        case RTDRV_FLOWCTRL_TAGPAUSE_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_tagPauseEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.tag_pause_en);
            break;

        case RTDRV_FLOWCTRL_HALF_CONSECUTIVE_RETRY_ENABLE_SET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_halfConsecutiveRetryEnable_set(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.half_retry_en);
            break;


        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_rate(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** RATE **/
        case RTDRV_RATE_INIT:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_init(buf->rate_cfg.unit);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_RATE_IGR_INCLUDE_IFG_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBandwidthCtrlIncludeIfg_set(buf->rate_cfg.unit, buf->rate_cfg.ifg_include);
            break;

        case RTDRV_RATE_EGR_INCLUDE_IFG_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_set(buf->rate_cfg.unit, buf->rate_cfg.ifg_include);
            break;

        case RTDRV_RATE_STORM_INCLUDE_IFG_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlIncludeIfg_set(buf->rate_cfg.unit, buf->rate_cfg.ifg_include);
            break;

#endif
        case RTDRV_RATE_INCLUDE_IFG_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_includeIfg_set(buf->rate_cfg.unit, buf->rate_cfg.module, buf->rate_cfg.ifg_include);
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwCtrlEnable_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwCtrlRate_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_IGR_BWCTRL_LOW_THRESH_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBandwidthLowThresh_set(buf->rate_cfg.unit, buf->rate_cfg.thresh);
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_RATE_PORT_IGR_BWCTRL_HIGH_THRESH_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthHighThresh_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.thresh);
            break;
#endif

        case RTDRV_RATE_IGR_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBwCtrlBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_IGR_PORT_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrPortBwCtrlBurstSize_set(buf->rate_cfg.unit,buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwCtrlBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.igrBwCfg);
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_EXCEED_RESET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthCtrlExceed_reset(buf->rate_cfg.unit,buf->rate_cfg.port);
            break;

        case RTDRV_RATE_IGR_BWCTRL_BYPASS_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBandwidthCtrlBypass_set(buf->rate_cfg.unit, buf->rate_cfg.igrBypassType, buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_FLOWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwFlowctrlEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlRate_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_IGR_QUEUE_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrQueueBwCtrlBurstSize_set(buf->rate_cfg.unit,buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_EXCEED_RESET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlExceed_reset(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue);
            break;

        case RTDRV_RATE_PORT_EGR_BWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBwCtrlEnable_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_EGR_BWCTRL_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBwCtrlRate_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_PORT_EGR_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBwCtrlBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_CPU_PORT_EGR_BWCTRL_RATE_MODE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_cpuEgrBandwidthCtrlRateMode_set(buf->rate_cfg.unit, buf->rate_cfg.rate_mode);
            break;

        case RTDRV_RATE_EGR_PORT_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrPortBwCtrlBurstSize_set(buf->rate_cfg.unit,buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueBwCtrlEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueBwCtrlRate_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueBwCtrlBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlRate_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_MODE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.assured_mode);
            break;

        case RTDRV_RATE_EGR_QUEUE_FIXED_BWCTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueFixedBandwidthEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_EGR_QUEUE_BWCTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlBurstSize_set(buf->rate_cfg.unit,buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlRate_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_STORM_CTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlBurstSize_set(buf->rate_cfg.unit,buf->rate_cfg.storm_type,buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlBurstSize_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_EXCEED_RESET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlExceed_reset(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type);
            break;

        case RTDRV_RATE_STORM_CTRL_RATE_MODE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlRateMode_set(buf->rate_cfg.unit,buf->rate_cfg.storm_rate_mode);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_RATE_MODE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlRateMode_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_rate_mode);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_TYPE_SEL_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlTypeSel_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.storm_type, buf->rate_cfg.storm_sel);
            break;

        case RTDRV_RATE_STORM_CTRL_BYPASS_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlBypass_set(buf->rate_cfg.unit, buf->rate_cfg.stormBypassType, buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoEnable_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_proto_type,buf->rate_cfg.enable);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_RATE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoRate_set(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_proto_type,buf->rate_cfg.rate);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_BURST_SIZE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoBurstSize_set(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.storm_proto_type, buf->rate_cfg.burst_size);
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_EXCEED_RESET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoExceed_reset(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.storm_proto_type);
            break;

        case RTDRV_RATE_STORM_CTRL_PROTO_VLAN_CONSTRT_ENABLE_SET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormCtrlProtoVlanConstrtEnable_set(buf->rate_cfg.unit, buf->rate_cfg.storm_proto_type, buf->rate_cfg.enable);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_switch(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SWITCH **/
        case RTDRV_SWITCH_CPU_MAX_PKTLEN_SET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_cpuMaxPktLen_set(buf->switch_cfg.unit, buf->switch_cfg.dir, buf->switch_cfg.maxLen);
            break;

        case RTDRV_SWITCH_MAX_PKTLEN_LINK_SPEED_SET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenLinkSpeed_set(buf->switch_cfg.unit, buf->switch_cfg.speed, buf->switch_cfg.maxLen);
            break;

        case RTDRV_SWITCH_PORT_MAX_PKTLEN_LINK_SPEED_SET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_portMaxPktLenLinkSpeed_set(buf->switch_cfg.unit, buf->switch_cfg.port, buf->switch_cfg.speed, buf->switch_cfg.maxLen);
            break;

        case RTDRV_SWITCH_MAX_PKTLEN_TAGLENCNT_SET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenTagLenCntIncEnable_set(buf->switch_cfg.unit, buf->switch_cfg.enable);
            break;

        case RTDRV_SWITCH_SNAP_MODE_SET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_snapMode_set(buf->switch_cfg.unit, buf->switch_cfg.snapMode);
            break;

        case RTDRV_SWITCH_PORT_MAX_PKTLEN_TAGLENCNT_INCENABLE_SET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_portMaxPktLenTagLenCntIncEnable_set(buf->switch_cfg.unit, buf->switch_cfg.port, buf->switch_cfg.enable);
            break;

        case RTDRV_SWITCH_CHKSUMFAILACTION_SET:
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_chksumFailAction_set(buf->switch_cfgParam.unit, buf->switch_cfgParam.port,
                buf->switch_cfgParam.failType, buf->switch_cfgParam.action);
            break;

        case RTDRV_SWITCH_RECALCCRCENABLE_SET:
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_recalcCRCEnable_set(buf->switch_cfgParam.unit, buf->switch_cfgParam.port, buf->switch_cfgParam.enable);
            break;

        case RTDRV_SWITCH_MGMTMACADDR_SET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_mgmtMacAddr_set(buf->switch_cfgInfo.unit, &buf->switch_cfgInfo.mac);
            break;

        case RTDRV_SWITCH_IPV4ADDR_SET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_IPv4Addr_set(buf->switch_cfgInfo.unit, buf->switch_cfgInfo.ipv4Addr);
            break;

        case RTDRV_SWITCH_PPPOE_IP_PARSE_ENABLE_SET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_pppoeIpParseEnable_set(buf->switch_cfgInfo.unit, buf->switch_cfgInfo.enable);
            break;

#if defined(CONFIG_SDK_DRIVER_WATCHDOG)
        case RTDRV_SWITCH_WATCHDOG_ENABLE_SET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = drv_watchdog_enable_set(buf->switch_cfgInfo.unit, buf->switch_cfgInfo.enable);
            break;
        case RTDRV_SWITCH_WATCHDOG_KICK:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = drv_watchdog_kick(buf->switch_cfgInfo.unit);
            break;

        case RTDRV_SWITCH_WATCHDOG_SCALE_SET:
            rtk_copy_from(&buf->watchdog_cfg, user, sizeof(rtdrv_watchdogCfgInfo_t));
            ret = drv_watchdog_scale_set(buf->switch_cfgInfo.unit, buf->watchdog_cfg.scale);
            break;
        case RTDRV_SWITCH_WATCHDOG_THRESHOLD_SET:
            rtk_copy_from(&buf->watchdog_cfg, user, sizeof(rtdrv_watchdogCfgInfo_t));
            ret = drv_watchdog_threshold_set(buf->watchdog_cfg.unit, &buf->watchdog_cfg.threshold);
            break;
#endif //CONFIG_SDK_DRIVER_WATCHDOG

#if defined(CONFIG_SDK_TC_DRV)
        case RTDRV_SWITCH_TC_ENABLE_SET:
            rtk_copy_from(&buf->tc_cfg, user, sizeof(rtdrv_tcCfgInfo_t));
            ret = drv_tc_enable_set(buf->tc_cfg.unit, buf->tc_cfg.id, buf->tc_cfg.enable);
            break;

        case RTDRV_SWITCH_TC_MODE_SET:
            rtk_copy_from(&buf->tc_cfg, user, sizeof(rtdrv_tcCfgInfo_t));
            ret = drv_tc_mode_set(buf->tc_cfg.unit, buf->tc_cfg.id, buf->tc_cfg.mode);
            break;

        case RTDRV_SWITCH_TC_DIVFACTOR_SET:
            rtk_copy_from(&buf->tc_cfg, user, sizeof(rtdrv_tcCfgInfo_t));
            ret = drv_tc_divFactor_set(buf->tc_cfg.unit, buf->tc_cfg.id, buf->tc_cfg.value);
            break;

        case RTDRV_SWITCH_TC_DATAINITVALUE_SET:
            rtk_copy_from(&buf->tc_cfg, user, sizeof(rtdrv_tcCfgInfo_t));
            ret = drv_tc_dataInitValue_set(buf->tc_cfg.unit, buf->tc_cfg.id, buf->tc_cfg.value);
            break;
#endif //CONFIG_SDK_TC_DRV

        case RTDRV_SWITCH_PKT2CPUTYPEFORMAT_SET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_pkt2CpuTypeFormat_set(buf->switch_cfgInfo.unit,
                    buf->switch_cfgInfo.trap_type, buf->switch_cfgInfo.format);
            break;

        case RTDRV_SWITCH_CPU_PKT_TRUNCATE_EN_SET:
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_cpuPktTruncateEnable_set(buf->switch_cfgParam.unit, buf->switch_cfgParam.enable);
            break;

        case RTDRV_SWITCH_CPU_PKT_TRUNCATE_LEN_SET:
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_cpuPktTruncateLen_set(buf->switch_cfgParam.unit, buf->switch_cfgParam.maxLen);
            break;

        case RTDRV_SWITCH_FLEXTBLFMT_SET:
            rtk_copy_from(&buf->switch_cfgTable, user, sizeof(rtdrv_switchCfgTable_t));
            ret = rtk_switch_flexTblFmt_set(buf->switch_cfgTable.unit, buf->switch_cfgTable.tbl_fmt);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_sys(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SYSTEM **/
        case RTDRV_SYS_HWP_UNIT_ADD:
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_nic(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
    /** NIC **/
        case RTDRV_NIC_RX_START:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_rx_start(buf->nic_cfg.unit);
            break;

        case RTDRV_NIC_RX_STOP:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_rx_stop(buf->nic_cfg.unit);
            break;

        case RTDRV_NIC_DEBUG_SET:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_dbg_set(buf->nic_cfg.unit, buf->nic_cfg.flags);
            break;

        case RTDRV_NIC_COUNTER_DUMP:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_cntr_dump(buf->nic_cfg.unit);
            break;

        case RTDRV_NIC_COUNTER_CLEAR:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_cntr_clear(buf->nic_cfg.unit);
            break;

        case RTDRV_NIC_BUFFER_DUMP:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_ringbuf_dump(buf->nic_cfg.unit);
            break;

        case RTDRV_NIC_PKTHDR_MBUF_DUMP:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_pktHdrMBuf_dump(buf->nic_cfg.unit, buf->nic_cfg.mode, buf->nic_cfg.start,
                                              buf->nic_cfg.end, buf->nic_cfg.flags);
            break;

        case RTDRV_NIC_TAG_SET:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_tag_set(buf->nic_cfg.unit, buf->nic_cfg.tagStatus, buf->nic_cfg.txTag, &buf->nic_cfg.portmask);
            break;

        case RTDRV_NIC_TXDATA_SET:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_txData_set(buf->nic_cfg.unit, buf->nic_cfg.isAuto, buf->nic_cfg.txData, buf->nic_cfg.len);
            break;

        case RTDRV_NIC_DIAGPKT_SEND:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_diagPkt_send(buf->nic_cfg.unit, buf->nic_cfg.num);
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_sdk(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SDK **/
        case RTDRV_SDK_TEST:
            rtk_copy_from(&buf->sdk_cfg, user, sizeof(rtdrv_sdkCfg_t));
            if (sdkTest_drv.run != NULL)
                ret = sdkTest_drv.run(buf->sdk_cfg.unit, buf->sdk_cfg.item);
            break;

        case RTDRV_SDK_TEST_ID:
            rtk_copy_from(&buf->sdk_cfg, user, sizeof(rtdrv_sdkCfg_t));
            if (sdkTest_drv.run_id != NULL)
                ret = sdkTest_drv.run_id(buf->sdk_cfg.unit, buf->sdk_cfg.start, buf->sdk_cfg.end);
            break;

        case RTDRV_SDK_TEST_MODE_SET:
            rtk_copy_from(&buf->sdk_cfg, user, sizeof(rtdrv_sdkCfg_t));
            if (sdkTest_drv.mode_set != NULL)
                ret = sdkTest_drv.mode_set(buf->sdk_cfg.mode);
            break;
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_eee(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** EEE **/
        case RTDRV_EEE_PORT_ENABLE_SET:
            rtk_copy_from(&buf->eee_cfg, user, sizeof(rtdrv_eeeCfg_t));
            ret = rtk_eee_portEnable_set(buf->eee_cfg.unit, buf->eee_cfg.port, buf->eee_cfg.enable);
            break;

        case RTDRV_EEEP_PORT_ENABLE_SET:
            rtk_copy_from(&buf->eee_cfg, user, sizeof(rtdrv_eeeCfg_t));
            ret = rtk_eeep_portEnable_set(buf->eee_cfg.unit, buf->eee_cfg.port, buf->eee_cfg.enable);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_sec(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SEC **/
        case RTDRV_SEC_PORT_ATTACK_PREVENT_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portAttackPrevent_set(buf->sec_cfg.unit, buf->sec_cfg.port, buf->sec_cfg.attack_type,
                                    buf->sec_cfg.action);
            break;

        case RTDRV_SEC_PORT_ATTACK_PREVENT_ENABLE_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portAttackPreventEnable_set(buf->sec_cfg.unit, buf->sec_cfg.port, buf->sec_cfg.enable);
            break;

        case RTDRV_SEC_ATTACK_PREVENT_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackPreventAction_set(buf->sec_cfg.unit, buf->sec_cfg.attack_type,
                                    buf->sec_cfg.action);
            break;

        case RTDRV_SEC_MIN_IPV6_FRAG_LEN_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_minIPv6FragLen_set(buf->sec_cfg.unit, buf->sec_cfg.data);
            break;

        case RTDRV_SEC_MAX_PING_LEN_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_maxPingLen_set(buf->sec_cfg.unit, buf->sec_cfg.data);
            break;

        case RTDRV_SEC_MIN_TCP_HDR_LEN_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_minTCPHdrLen_set(buf->sec_cfg.unit, buf->sec_cfg.data);
            break;

        case RTDRV_SEC_SMURF_NETMASK_LEN_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_smurfNetmaskLen_set(buf->sec_cfg.unit, buf->sec_cfg.data);
            break;

        case RTDRV_SEC_TRAPTARGET_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_trapTarget_set(buf->sec_cfg.unit, buf->sec_cfg.target);
            break;

        case RTDRV_SEC_IPMACBINDACTION_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_ipMacBindAction_set(buf->sec_cfg.unit, buf->sec_cfg.lumisAct, buf->sec_cfg.matchAct, buf->sec_cfg.mismatchAct);
            break;

        case RTDRV_SEC_PORTIPMACBINDENABLE_SET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portIpMacBindEnable_set(buf->sec_cfg.unit, buf->sec_cfg.port, buf->sec_cfg.type, buf->sec_cfg.enable);
            break;

        case RTDRV_SEC_IPMACBINDENTRY_ADD:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_ipMacBindEntry_add(buf->sec_cfg.unit, &buf->sec_cfg.entry);
            break;

        case RTDRV_SEC_IPMACBINDENTRY_DEL:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_ipMacBindEntry_del(buf->sec_cfg.unit, &buf->sec_cfg.entry);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_led(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** LED **/
        case RTDRV_LED_SYS_ENABLE_SET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_sysEnable_set(buf->led_cfg.unit, buf->led_cfg.type, buf->led_cfg.enable);
            break;

        case RTDRV_LED_PORTLEDENTITYSWCTRLENABLE_SET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_portLedEntitySwCtrlEnable_set(buf->led_cfg.unit,
                    buf->led_cfg.port, buf->led_cfg.entity, buf->led_cfg.enable);
            break;

        case RTDRV_LED_SWCTRL_START:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_swCtrl_start(buf->led_cfg.unit);
            break;

        case RTDRV_LED_PORTLEDENTITYSWCTRLMODE_SET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_portLedEntitySwCtrlMode_set(buf->led_cfg.unit,
                    buf->led_cfg.port, buf->led_cfg.entity, buf->led_cfg.media,
                    buf->led_cfg.mode);
            break;

        case RTDRV_LED_SYSMODE_SET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_sysMode_set(buf->led_cfg.unit, buf->led_cfg.mode);
            break;

        case RTDRV_LED_BLINKTIME_SET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_blinkTime_set(buf->led_cfg.unit, buf->led_cfg.time);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_mpls(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* MPLS */
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310))
        case RTDRV_MPLS_INIT:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_init(buf->mpls_cfg.unit);
            break;

        case RTDRV_MPLS_TTLINHERIT_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_ttlInherit_set(buf->mpls_cfg.unit, buf->mpls_cfg.u.inherit);
            break;

        case RTDRV_MPLS_ENABLE_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_enable_set(buf->mpls_cfg.unit, buf->mpls_cfg.enable);
            break;

        case RTDRV_MPLS_TRAPTARGET_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_trapTarget_set(buf->mpls_cfg.unit, buf->mpls_cfg.u.target);
            break;

        case RTDRV_MPLS_EXCEPTIONCTRL_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_exceptionCtrl_set(buf->mpls_cfg.unit, buf->mpls_cfg.u.exceptionType, buf->mpls_cfg.action);
            break;

        case RTDRV_MPLS_NEXTHOP_CREATE:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_nextHop_create(buf->mpls_cfg.unit, &buf->mpls_cfg.u.nexthop, &buf->mpls_cfg.pathId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_NEXTHOP_DESTROY:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_nextHop_destroy(buf->mpls_cfg.unit, buf->mpls_cfg.pathId);
            break;

        case RTDRV_MPLS_NEXTHOP_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_nextHop_set(buf->mpls_cfg.unit, buf->mpls_cfg.pathId, &buf->mpls_cfg.u.nexthop);
            break;

        case RTDRV_MPLS_ENCAP_CREATE:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_encap_create(buf->mpls_cfg.unit, &buf->mpls_cfg.u.encap, &buf->mpls_cfg.entryId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_ENCAP_DESTROY:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_encap_destroy(buf->mpls_cfg.unit, buf->mpls_cfg.entryId);
            break;

        case RTDRV_MPLS_ENCAP_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_encap_set(buf->mpls_cfg.unit, buf->mpls_cfg.entryId, &buf->mpls_cfg.u.encap);
            break;

        case RTDRV_MPLS_HASHALGO_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_hashAlgo_set(buf->mpls_cfg.unit, buf->mpls_cfg.hashAlgo);
            break;

        case RTDRV_MPLS_DECAP_CREATE:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_decap_create(buf->mpls_cfg.unit, &buf->mpls_cfg.u.decap, &buf->mpls_cfg.entryId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_DECAP_DESTROY:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_decap_destroy(buf->mpls_cfg.unit, buf->mpls_cfg.entryId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_DECAP_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_decap_set(buf->mpls_cfg.unit, buf->mpls_cfg.entryId, &buf->mpls_cfg.u.decap);
            break;

        case RTDRV_MPLS_EGRTCMAP_SET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_egrTcMap_set(buf->mpls_cfg.unit, &buf->mpls_cfg.src, buf->mpls_cfg.tc);
            break;
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)) */

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_sc(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
        case RTDRV_DIAG_SC_REG_WRITE:
            rtk_copy_from(&buf->sc_regInfo, user, sizeof(rtdrv_scRegInfo_t));
            ret = hal_rtl8295_reg_write(buf->sc_regInfo.unit, buf->sc_regInfo.port, buf->sc_regInfo.addr, buf->sc_regInfo.data);
            break;
        case RTDRV_DIAG_SC_SDS_WRITE:
            rtk_copy_from(&buf->sc_sdsInfo, user, sizeof(rtdrv_scSdsInfo_t));
            ret = hal_rtl8295_sds_write(buf->sc_sdsInfo.unit, buf->sc_sdsInfo.port, buf->sc_sdsInfo.sds, buf->sc_sdsInfo.page, buf->sc_sdsInfo.reg, buf->sc_sdsInfo.data);
            break;
        case RTDRV_DIAG_SC_PATCH:
            rtk_copy_from(&buf->sc_patch, user, sizeof(rtdrv_scPatch_t));
            ret = _dal_phy_debugCmd_set(buf->sc_patch.unit, buf->sc_patch.port, buf->sc_patch.mdxMacId, buf->sc_patch.sds, buf->sc_patch.name);
            break;
        case RTDRV_DIAG_SC_PATCH_DBG:
            rtk_copy_from(&buf->sc_patchDbg, user, sizeof(rtdrv_scPatch_t));
            ret = phy_8295_patch_debugEnable_set(buf->sc_patchDbg.enable);
            break;
#endif /* defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) */
#if defined(CONFIG_SDK_RTL8295R)
  #if defined(CONFIG_SDK_RTL8390)
        case RTDRV_DIAG_SC_8390_10GMEDIA_SET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = phy_8390_10gMedia_set(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.media_10g);
            break;
  #endif
        case RTDRV_DIAG_SC_8295R_RXCALICONF_SET:
            {
                phy_8295_rxCaliConf_t   rxCaliConf;

                rtk_copy_from(&buf->rxCaliConf, user, sizeof(rtdrv_8295_rxCaliConf_t));
                if ((ret = phy_8295r_rxCaliConfPort_get(buf->rxCaliConf.unit, buf->rxCaliConf.port, &rxCaliConf)) == RT_ERR_OK)
                {
                    rxCaliConf.dacLongCableOffset         = buf->rxCaliConf.dacLongCableOffset;
                    rxCaliConf.s1.rxCaliEnable            = buf->rxCaliConf.s1rxCaliEnable;
                    rxCaliConf.s1.tap0InitVal             = buf->rxCaliConf.s1tap0InitVal;
                    rxCaliConf.s1.vthMinThr               = buf->rxCaliConf.s1vthMinThr;
                    rxCaliConf.s1.eqHoldEnable            = buf->rxCaliConf.s1eqHoldEnable;
                    rxCaliConf.s1.dfeTap1_4Enable         = buf->rxCaliConf.s1dfeTap1_4Enable;
                    rxCaliConf.s1.dfeAuto                 = buf->rxCaliConf.s1dfeAuto;
                    rxCaliConf.s0.rxCaliEnable            = buf->rxCaliConf.s0rxCaliEnable;
                    rxCaliConf.s0.tap0InitVal             = buf->rxCaliConf.s0tap0InitVal;
                    rxCaliConf.s0.vthMinThr               = buf->rxCaliConf.s0vthMinThr  ;
                    rxCaliConf.s0.eqHoldEnable            = buf->rxCaliConf.s0eqHoldEnable;
                    rxCaliConf.s0.dfeTap1_4Enable         = buf->rxCaliConf.s0dfeTap1_4Enable;
                    rxCaliConf.s0.dfeAuto                 = buf->rxCaliConf.s0dfeAuto;

                    ret = phy_8295r_rxCaliConfPort_set(buf->rxCaliConf.unit, buf->rxCaliConf.port, &rxCaliConf);
                }
            }
            break;
#endif/* defined(CONFIG_SDK_RTL8295R) */
#if defined(CONFIG_SDK_RTL8214QF)
        case RTDRV_DIAG_SC_8214QF_RXCALICONF_SET:
            {
                phy_8295_rxCaliConf_t   rxCaliConf;

                rtk_copy_from(&buf->rxCaliConf, user, sizeof(rtdrv_8295_rxCaliConf_t));
                if ((ret = phy_8214qf_rxCaliConf_get(buf->rxCaliConf.unit, buf->rxCaliConf.port, &rxCaliConf)) == RT_ERR_OK)
                {
                    rxCaliConf.dacLongCableOffset         = buf->rxCaliConf.dacLongCableOffset;
                    rxCaliConf.s1.rxCaliEnable            = buf->rxCaliConf.s1rxCaliEnable;
                    rxCaliConf.s1.tap0InitVal             = buf->rxCaliConf.s1tap0InitVal;
                    rxCaliConf.s1.vthMinThr               = buf->rxCaliConf.s1vthMinThr;
                    rxCaliConf.s1.eqHoldEnable            = buf->rxCaliConf.s1eqHoldEnable;
                    rxCaliConf.s1.dfeTap1_4Enable         = buf->rxCaliConf.s1dfeTap1_4Enable;
                    rxCaliConf.s1.dfeAuto                 = buf->rxCaliConf.s1dfeAuto;
                    rxCaliConf.s0.rxCaliEnable            = buf->rxCaliConf.s0rxCaliEnable;
                    rxCaliConf.s0.tap0InitVal             = buf->rxCaliConf.s0tap0InitVal;
                    rxCaliConf.s0.vthMinThr               = buf->rxCaliConf.s0vthMinThr  ;
                    rxCaliConf.s0.eqHoldEnable            = buf->rxCaliConf.s0eqHoldEnable;
                    rxCaliConf.s0.dfeTap1_4Enable         = buf->rxCaliConf.s0dfeTap1_4Enable;
                    rxCaliConf.s0.dfeAuto                 = buf->rxCaliConf.s0dfeAuto;


                    ret = phy_8214qf_rxCaliConf_set(buf->rxCaliConf.unit, buf->rxCaliConf.port, &rxCaliConf);
                }
            }
            break;
#endif/* defined(CONFIG_SDK_RTL8214QF) */
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_rtl8231(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL8231)
    /** RTL8231 **/
        case RTDRV_RTL8231_I2C_WRITE:
            rtk_copy_from(&buf->rtl8231_cfg, user, sizeof(rtdrv_rtl8231Cfg_t));
            ret = drv_rtl8231_i2c_write(buf->rtl8231_cfg.unit, buf->rtl8231_cfg.phyId_or_slaveAddr, buf->rtl8231_cfg.reg_addr, buf->rtl8231_cfg.data);
            break;

        case RTDRV_RTL8231_MDC_WRITE:
            rtk_copy_from(&buf->rtl8231_cfg, user, sizeof(rtdrv_rtl8231Cfg_t));
            ret = drv_rtl8231_mdc_write(buf->rtl8231_cfg.unit, buf->rtl8231_cfg.phyId_or_slaveAddr, buf->rtl8231_cfg.page, buf->rtl8231_cfg.reg_addr, buf->rtl8231_cfg.data);
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_set_ctl_extgpio(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL8231)
        case RTDRV_EXTGPIO_DEV_INIT:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_dev_init(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, &buf->extGpio_cfg.extGpio_devConfData);
            break;

        case RTDRV_EXTGPIO_DEV_ENABLE_SET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_devEnable_set(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.data);
            break;

        case RTDRV_EXTGPIO_SYNC_ENABLE_SET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_syncEnable_set(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.data);
            break;

        case RTDRV_EXTGPIO_SYNC_START:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_sync_start(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev);
            break;

        case RTDRV_EXTGPIO_PIN_INIT:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_pin_init(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, &buf->extGpio_cfg.extGpio_confData);
            break;

        case RTDRV_EXTGPIO_DATABIT_SET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_dataBit_set(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, buf->extGpio_cfg.data);
            break;

        case RTDRV_EXTGPIO_REG_WRITE:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_reg_write(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.reg, buf->extGpio_cfg.data);
            break;

        case RTDRV_EXTGPIO_DIRECTION_SET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_direction_set(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, buf->extGpio_cfg.data);
            break;
        case RTDRV_EXTGPIO_I2C_INIT:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_i2c_init(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, buf->extGpio_cfg.data);
            break;

        case RTDRV_EXTGPIO_I2C_WRITE:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_i2c_write(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.reg, buf->extGpio_cfg.data);
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_gpio(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
    /** Internal GPIO **/
        case RTDRV_GPIO_PIN_INIT:
            rtk_copy_from(&buf->gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = drv_gpio_pin_init(buf->gpio_cfg.unit, buf->gpio_cfg.gpioId, buf->gpio_cfg.function, buf->gpio_cfg.direction, buf->gpio_cfg.interruptEnable);
            break;

        case RTDRV_GPIO_DATABIT_INIT:
            rtk_copy_from(&buf->gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = drv_gpio_dataBit_init(buf->gpio_cfg.unit, buf->gpio_cfg.gpioId, buf->gpio_cfg.data);
            break;

        case RTDRV_GPIO_DATABIT_SET:
            rtk_copy_from(&buf->gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = drv_gpio_dataBit_set(buf->gpio_cfg.unit, buf->gpio_cfg.gpioId, buf->gpio_cfg.data);
            break;
#endif/* CONFIG_SDK_DRIVER_GPIO */

#if defined(CONFIG_SDK_RTL8231) || defined(CONFIG_SDK_DRIVER_GPIO)
        case RTDRV_GENCTRL_GPIO_DEV_INIT:
            rtk_copy_from(&buf->genCtrlGPIO_cfg, user, sizeof(rtdrv_generalCtrlGpioCfg_t));
            ret = drv_generalCtrlGPIO_dev_init(buf->genCtrlGPIO_cfg.unit, buf->genCtrlGPIO_cfg.dev, &buf->genCtrlGPIO_cfg.genCtrl_gpioDev);
            break;
        case RTDRV_GENCTRL_GPIO_PIN_INIT:
            rtk_copy_from(&buf->genCtrlGPIO_cfg, user, sizeof(rtdrv_generalCtrlGpioCfg_t));
            ret = drv_generalCtrlGPIO_pin_init(buf->genCtrlGPIO_cfg.unit, buf->genCtrlGPIO_cfg.dev, buf->genCtrlGPIO_cfg.gpioId, &buf->genCtrlGPIO_cfg.genCtrl_gpioPin);
            break;
        case RTDRV_GENCTRL_GPIO_DEV_ENABLE:
            rtk_copy_from(&buf->genCtrlGPIO_cfg, user, sizeof(rtdrv_generalCtrlGpioCfg_t));
            ret = drv_generalCtrlGPIO_devEnable_set(buf->genCtrlGPIO_cfg.unit, buf->genCtrlGPIO_cfg.dev, buf->genCtrlGPIO_cfg.data);
            break;
        case RTDRV_GENCTRL_GPIO_DATABIT_SET:
            rtk_copy_from(&buf->genCtrlGPIO_cfg, user, sizeof(rtdrv_generalCtrlGpioCfg_t));
            ret = drv_generalCtrlGPIO_dataBit_set(buf->genCtrlGPIO_cfg.unit, buf->genCtrlGPIO_cfg.dev, buf->genCtrlGPIO_cfg.gpioId, buf->genCtrlGPIO_cfg.data);
            break;
#endif //defined(CONFIG_SDK_RTL8231) || defined(CONFIG_SDK_DRIVER_GPIO)

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_i2c(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_I2C)
        /**I2C**/
        case RTDRV_I2C_INIT:
            rtk_copy_from(&buf->i2c_cfg, user, sizeof(rtdrv_i2c_devCfg_t));
            ret = drv_i2c_init(buf->i2c_cfg.unit_id);
            break;
        case RTDRV_I2C_DEV_INIT:
        {
            i2c_devConf_t                   i2c_dev;

            rtk_copy_from(&buf->i2c_cfg, user, sizeof(rtdrv_i2c_devCfg_t));
            i2c_dev.device_id = buf->i2c_cfg.device_id;
            i2c_dev.mem_addr_width= buf->i2c_cfg.mem_addr_width;
            i2c_dev.dev_addr= buf->i2c_cfg.dev_addr;
            i2c_dev.data_width= buf->i2c_cfg.data_width;
            i2c_dev.clk_freq= buf->i2c_cfg.scl_freq;
            i2c_dev.scl_delay= buf->i2c_cfg.scl_delay;
            i2c_dev.scl_dev= buf->i2c_cfg.scl_dev;
            i2c_dev.scl_pin_id= buf->i2c_cfg.scl_pin_id;
            i2c_dev.sda_dev= buf->i2c_cfg.sda_dev;
            i2c_dev.sda_pin_id= buf->i2c_cfg.sda_pin_id;
            i2c_dev.i2c_interface_id = buf->i2c_cfg.i2c_interface_id;
            i2c_dev.read_type = buf->i2c_cfg.read_type;
            ret = drv_i2c_dev_init(buf->i2c_cfg.unit_id, &i2c_dev);
            break;
        }
        case RTDRV_I2C_WRITE:
            rtk_copy_from(&buf->i2c_cfg, user, sizeof(rtdrv_i2c_devCfg_t));
            ret = drv_i2c_write(buf->i2c_cfg.unit_id, buf->i2c_cfg.device_id, buf->i2c_cfg.reg_idx, &buf->i2c_cfg.rwdata[0]);
           break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_spi(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_SPI)
        case RTDRV_SPI_WRITE:
            rtk_copy_from(&buf->spi_cfg, user, sizeof(rtdrv_spiCfg_t));
            ret = drv_spi_write(buf->spi_cfg.unit, buf->spi_cfg.addr, &buf->spi_cfg.data);
            break;
        case RTDRV_SPI_INIT:
            rtk_copy_from(&buf->spi_initInfo, user, sizeof(rtdrv_spiInitInfo_t));
            ret = drv_spiPin_init(buf->spi_initInfo.unit, &buf->spi_initInfo.init_info);
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_diag(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /* DIAG */
        case RTDRV_DIAG_RTCTENABLE_SET:
            rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
            ret = rtk_diag_rtctEnable_set(buf->diag_cfg.unit, &buf->diag_cfg.portmask);
            break;
        #if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
         case RTDRV_DIAG_DEBUG_FIELD_SET:
             rtk_copy_from(&buf->diag_debug_cfg, user, sizeof(rtdrv_diag_debug_cfg_t));
             ret = rtk_diag_table_reg_field_set((uint32)buf->diag_debug_cfg.unit, &buf->diag_debug_cfg.diag_debug);
             break;
        #endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_set_ctl_openflow(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL9310)
        /* OpenFlow */
        case RTDRV_OF_INIT:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_init(buf->openflow_cfg.unit);
            break;

        case RTDRV_OF_CLASSIFIER_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_classifier_set(buf->openflow_cfg.unit, buf->openflow_cfg.classifyType, buf->openflow_cfg.classifyData);
            break;

        case RTDRV_OF_FLOWENTRYVALIDATE_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryValidate_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.valid);
            break;

        case RTDRV_OF_FLOWENTRYFIELD_WRITE:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryField_write(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.matchFieldType, buf->openflow_cfg.fieldData, buf->openflow_cfg.fieldMask);
            break;

        case RTDRV_OF_FLOWENTRYOPERATION_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryOperation_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.pOperation);
            break;

        case RTDRV_OF_FLOWENTRYINSTRUCTION_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryInstruction_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.flowInsData);
            break;

        case RTDRV_OF_FLOWENTRY_DEL:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntry_del(buf->openflow_cfg.unit, buf->openflow_cfg.phase, &buf->openflow_cfg.clrIdx);
            break;

        case RTDRV_OF_FLOWENTRY_MOVE:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntry_move(buf->openflow_cfg.unit, buf->openflow_cfg.phase, &buf->openflow_cfg.moveIdx);
            break;

        case RTDRV_OF_FTTEMPLATESELECTOR_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_ftTemplateSelector_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.block_idx, buf->openflow_cfg.template_idx);
            break;

        case RTDRV_OF_FLOWCNTMODE_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowCntMode_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.mode);
            break;

        case RTDRV_OF_FLOWCNT_CLEAR:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowCnt_clear(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.flowCntType);
            break;

        case RTDRV_OF_FLOWCNTTHRESH_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowCntThresh_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.threshold);
            break;

        case RTDRV_OF_TTLEXCPT_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_ttlExcpt_set(buf->openflow_cfg.unit, buf->openflow_cfg.action);
            break;

        case RTDRV_OF_MAXLOOPBACK_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_maxLoopback_set(buf->openflow_cfg.unit, buf->openflow_cfg.times);
            break;

        case RTDRV_OF_L2FLOWTBLMATCHFIELD_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowTblMatchField_set(buf->openflow_cfg.unit, buf->openflow_cfg.l2Field);
            break;

        case RTDRV_OF_L2FLOWENTRY_ADD:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntry_add(buf->openflow_cfg.unit, &buf->openflow_cfg.l2Entry);
            break;

        case RTDRV_OF_L2FLOWENTRY_DEL:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntry_del(buf->openflow_cfg.unit, &buf->openflow_cfg.l2Entry);
            break;

        case RTDRV_OF_L2FLOWENTRY_DELALL:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntry_delAll(buf->openflow_cfg.unit);
            break;

        case RTDRV_OF_L2FLOWTBLHASHALGO_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowTblHashAlgo_set(buf->openflow_cfg.unit, buf->openflow_cfg.block, buf->openflow_cfg.algo);
            break;

        case RTDRV_OF_L3FLOWTBLPRI_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3FlowTblPri_set(buf->openflow_cfg.unit, buf->openflow_cfg.table);
            break;

        case RTDRV_OF_L3CAMFLOWTBLMATCHFIELD_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamFlowTblMatchField_set(buf->openflow_cfg.unit, buf->openflow_cfg.l3CamField);
            break;

        case RTDRV_OF_L3HASHFLOWTBLMATCHFIELD_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowTblMatchField_set(buf->openflow_cfg.unit, buf->openflow_cfg.l3HashField);
            break;

        case RTDRV_OF_L3HASHFLOWTBLHASHALGO_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowTblHashAlgo_set(buf->openflow_cfg.unit, buf->openflow_cfg.block, buf->openflow_cfg.algo);
            break;

        case RTDRV_OF_L3CAMFLOWENTRY_ADD:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamFlowEntry_add(buf->openflow_cfg.unit, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.l3CamEntry);
            break;

        case RTDRV_OF_L3CAMFLOWENTRY_DEL:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamFlowEntry_del(buf->openflow_cfg.unit, buf->openflow_cfg.idx);
            break;

        case RTDRV_OF_L3CAMFLOWENTRY_MOVE:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamFlowEntry_move(buf->openflow_cfg.unit, &buf->openflow_cfg.moveIdx);
            break;

        case RTDRV_OF_L3HASHFLOWENTRY_ADD:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowEntry_add(buf->openflow_cfg.unit, &buf->openflow_cfg.l3HashEntry);
            break;

        case RTDRV_OF_L3HASHFLOWENTRY_DEL:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowEntry_del(buf->openflow_cfg.unit, &buf->openflow_cfg.l3HashEntry);
            break;

        case RTDRV_OF_L3HASHFLOWENTRY_DELALL:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowEntry_delAll(buf->openflow_cfg.unit);
            break;

        case RTDRV_OF_GROUPENTRY_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_groupEntry_set(buf->openflow_cfg.unit, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.grpEntry);
            break;

        case RTDRV_OF_GROUPTBLHASHPARA_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_groupTblHashPara_set(buf->openflow_cfg.unit, &buf->openflow_cfg.para);
            break;

        case RTDRV_OF_ACTIONBUCKET_SET:
            rtk_copy_from(&buf->openflowAB_cfg, user, sizeof(rtdrv_openflowABCfg_t));
            ret = rtk_of_actionBucket_set(buf->openflowAB_cfg.unit, buf->openflowAB_cfg.entry_idx, &buf->openflowAB_cfg.actionBktEntry);
            break;

        case RTDRV_OF_TRAPTARGET_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_trapTarget_set(buf->openflow_cfg.unit, buf->openflow_cfg.target);
            break;

        case RTDRV_OF_TBLMISSACTION_SET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_tblMissAction_set(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.tblMissAct);
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_set_ctl_serdes(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* SerDes */
        case RTDRV_SERDES_REG_SET:
            rtk_copy_from(&buf->serdes_reg_cfg, user, sizeof(rtdrv_serdes_reg_t));
            ret = hal_serdes_reg_set(buf->serdes_reg_cfg.unit, buf->serdes_reg_cfg.sdsId, buf->serdes_reg_cfg.page, buf->serdes_reg_cfg.reg, buf->serdes_reg_cfg.data);
            break;

        case RTDRV_SERDES_SYMERR_CLEAR:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_symErr_clear(buf->sds_cfg.unit, buf->sds_cfg.sds);
            break;

        case RTDRV_SDS_TESTMODE_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_testMode_set(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.mode);
            break;

        case RTDRV_SDS_RX_RST:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_sds_rx_rst(buf->sds_cfg.unit, buf->sds_cfg.sds);
            break;

        case RTDRV_SDS_LEQ_ADAPT:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_leq_adapt(buf->sds_cfg.unit, buf->sds_cfg.sds);
            break;

        case RTDRV_SDS_XSGNWAYEN_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_xsgNwayEn_set(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.en);
            break;

        case RTDRV_SDS_CMUBAND_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_cmuBand_set(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.en, buf->sds_cfg.cnt);
            break;
        case RTDRV_SDS_EYEPARAM_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_eyeParam_set(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.param);
            break;

        case RTDRV_SDS_EYEMONITOR_START:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_eyeMonitor_start(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.frameNum);
            break;

        case RTDRV_SDS_RXCALICONF_SET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_rxCaliConf_set(buf->sds_cfg.unit, buf->sds_cfg.sds, buf->sds_cfg.conf);
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 rtdrv_setFunc_dispatch(int cmd, void *user_in, unsigned int len)
{
    int32   ret = RT_ERR_FAILED;
    void    *user = (char *)user_in + sizeof(rtdrv_msgHdr_t);
    int     module_offset, module_index;
    rtdrv_union_t                   *buf = NULL;

    module_offset = cmd & RTDRV_MODULE_MASK;
    module_index = module_offset >> RTDRV_MODULE_SHIFT;

    if ((module_index < 0) || (module_index > RTDRV_MODULE_MAX_NUM))
    {
        return EPROTONOSUPPORT;
    }

    buf = (rtdrv_union_t*)osal_alloc(sizeof(rtdrv_union_t));
    if(!buf)
        return ENOMEM;

    osal_memset(buf, 0, sizeof(rtdrv_union_t));

    if (rtdrv_module_db_set[module_index].valid == TRUE)
    {
        ret = rtdrv_module_db_set[module_index].func(cmd, user, buf);
    }

    rtk_copy_to(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    osal_free(buf);
    return 0;
}

/* Function Name:
 *      do_rtdrv_set_ctl
 * Description:
 *      This function is called whenever a process tries to do setsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_set_ctl(struct sock *sk, int cmd, void *user_in, unsigned int len)
{
    return rtdrv_setFunc_dispatch(cmd, user_in, len);
}
EXPORT_SYMBOL(do_rtdrv_set_ctl);
EXPORT_SYMBOL(rtdrv_setFunc_dispatch);


int32 rtdrv_getFunc_dispatch(int cmd, void *user_in, int *len)
{
    int32   ret = RT_ERR_FAILED;
    void    *user = (char *)user_in + sizeof(rtdrv_msgHdr_t);
    int     module_offset, module_index;
    rtdrv_union_t                   *buf = NULL;

    module_offset = cmd & RTDRV_MODULE_MASK;
    module_index = module_offset >> RTDRV_MODULE_SHIFT;

    if ((module_index < 0) || (module_index > RTDRV_MODULE_MAX_NUM))
    {
        return EPROTONOSUPPORT;
    }

    buf = (rtdrv_union_t*)osal_alloc(sizeof(rtdrv_union_t));

    if(!buf)
        return ENOMEM;

    osal_memset(buf, 0, sizeof(rtdrv_union_t));

    if (rtdrv_module_db_get[module_index].valid == TRUE)
        ret = rtdrv_module_db_get[module_index].func(cmd, user, buf);

    rtk_copy_to(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    osal_free(buf);
    return 0;

}

/* Function Name:
 *      do_rtdrv_get_ctl
 * Description:
 *      This function is called whenever a process tries to do getsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 * Output:
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_get_ctl(struct sock *sk, int cmd, void *user_in, int *len)
{
    return rtdrv_getFunc_dispatch(cmd,user_in,len);
}
EXPORT_SYMBOL(do_rtdrv_get_ctl);
EXPORT_SYMBOL(rtdrv_getFunc_dispatch);


int32 do_rtdrv_get_ctl_l2(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* L2 */
        case RTDRV_L2_FLUSH_LINK_DOWN_PORT_ADDR_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_flushLinkDownPortAddrEnable_get(buf->l2_cfg.l2_common.unit, &buf->l2_cfg.l2_common.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_LEARNING_CNT_GET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_learningCnt_get(buf->l2_learn.unit, &buf->l2_learn.mac_cnt);
            rtk_copy_to(user, &buf->l2_learn, sizeof(rtdrv_l2_learnCnt_t));
            break;

        case RTDRV_L2_PORT_LEARNING_CNT_GET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_portLearningCnt_get(buf->l2_learn.unit, buf->l2_learn.port, &buf->l2_learn.mac_cnt);
            rtk_copy_to(user, &buf->l2_learn, sizeof(rtdrv_l2_learnCnt_t));
            break;

        case RTDRV_L2_FID_LEARNING_CNT_GET:
            rtk_copy_from(&buf->l2_FidLearn, user, sizeof(rtdrv_l2_learnFidCnt_t));
            ret = rtk_l2_fidLearningCnt_get(buf->l2_FidLearn.unit, buf->l2_FidLearn.entryIdx, &buf->l2_FidLearn.mac_cnt);
            rtk_copy_to(user, &buf->l2_FidLearn, sizeof(rtdrv_l2_learnFidCnt_t));
            break;
#endif
        case RTDRV_L2_MAC_LEARNING_CNT_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_macLearningCnt_get(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.type, &buf->l2_cfg.l2_macLimit.cnt);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_LIMIT_LEARNING_CNT_GET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_limitLearningCnt_get(buf->l2_learn.unit, &buf->l2_learn.mac_cnt);
            rtk_copy_to(user, &buf->l2_learn, sizeof(rtdrv_l2_learnCnt_t));
            break;

        case RTDRV_L2_PORT_LIMIT_LEARNING_CNT_GET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_portLimitLearningCnt_get(buf->l2_learn.unit, buf->l2_learn.port, &buf->l2_learn.mac_cnt);
            rtk_copy_to(user, &buf->l2_learn, sizeof(rtdrv_l2_learnCnt_t));
            break;
#endif
        case RTDRV_L2_LIMIT_LEARNING_NUM_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
             ret = rtk_l2_limitLearningNum_get(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.type, &buf->l2_cfg.l2_macLimit.cnt);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_LIMIT_LEARNING_CNT_ACT_GET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_limitLearningCntAction_get(buf->l2_learn.unit, &buf->l2_learn.action);
            rtk_copy_to(user, &buf->l2_learn, sizeof(rtdrv_l2_learnCnt_t));
            break;

        case RTDRV_L2_PORT_LIMIT_LEARNING_CNT_ACT_GET:
            rtk_copy_from(&buf->l2_learn, user, sizeof(rtdrv_l2_learnCnt_t));
            ret = rtk_l2_portLimitLearningCntAction_get(buf->l2_learn.unit, buf->l2_learn.port, &buf->l2_learn.action);
            rtk_copy_to(user, &buf->l2_learn, sizeof(rtdrv_l2_learnCnt_t));
            break;

        case RTDRV_L2_FID_LIMIT_LEARNING_CNT_ACT_GET:
            rtk_copy_from(&buf->l2_FidLearn, user, sizeof(rtdrv_l2_learnFidCnt_t));
            ret = rtk_l2_fidLearningCntAction_get(buf->l2_FidLearn.unit, &buf->l2_FidLearn.action);
            rtk_copy_to(user, &buf->l2_FidLearn, sizeof(rtdrv_l2_learnFidCnt_t));
            break;
#endif
        case RTDRV_L2_LIMIT_LEARNING_ACT_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningAction_get(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.type, &buf->l2_cfg.l2_macLimit.action);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_FID_LIMIT_LEARNING_ENTRY_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_fidLimitLearningEntry_get(buf->l2_cfg.l2_macLimit.unit, buf->l2_cfg.l2_macLimit.fidLimitEntryId, &buf->l2_cfg.l2_macLimit.entry);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_AGING_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_l2_aging_get(buf->unit_cfg.unit, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif
        case RTDRV_L2_AGING_TIME_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_agingTime_get(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.type, &buf->l2_cfg.l2_age.ageTime);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORT_AGING_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portAgingEnable_get(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.port, &buf->l2_cfg.l2_age.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_TRK_AGING_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_trkAgingEnable_get(buf->l2_cfg.l2_age.unit, buf->l2_cfg.l2_age.trunk, &buf->l2_cfg.l2_age.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_HASH_ALGO_GET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_hashAlgo_get(buf->l2_learnCfg.unit, &buf->l2_learnCfg.hash_algo);
            rtk_copy_to(user, &buf->l2_learnCfg, sizeof(rtdrv_l2_learnCfg_t));
            break;
#endif
        case RTDRV_L2_BUCKET_HASH_ALGO_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_bucketHashAlgo_get(buf->l2_cfg.l2_hash.unit, buf->l2_cfg.l2_hash.bucket, &buf->l2_cfg.l2_hash.hashAlgo);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_VLANMODE_GET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_vlanMode_get(buf->l2_learnCfg.unit, buf->l2_learnCfg.port, &buf->l2_learnCfg.vlanMode);
            rtk_copy_to(user, &buf->l2_learnCfg, sizeof(rtdrv_l2_learnCfg_t));
            break;

        case RTDRV_L2_LEARNING_FULL_ACT_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_learningFullAction_get(buf->l2_cfg.l2_learn.unit, &buf->l2_cfg.l2_learn.action);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORT_NEW_MAC_OP_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portNewMacOp_get(buf->l2_cfg.l2_learn.unit, buf->l2_cfg.l2_learn.port, &buf->l2_cfg.l2_learn.lrnMode, &buf->l2_cfg.l2_learn.action);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_INIT:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_init(buf->l2_cfg.l2_ucAddr.unit, buf->l2_cfg.l2_ucAddr.vid, &buf->l2_cfg.l2_ucAddr.mac, &buf->l2_cfg.l2_ucAddr.ucast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_ADD:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_add(buf->l2_cfg.l2_ucAddr.unit, &buf->l2_cfg.l2_ucAddr.ucast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_get(buf->l2_cfg.l2_ucAddr.unit, &buf->l2_cfg.l2_ucAddr.ucast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_set(buf->l2_cfg.l2_ucAddr.unit, &buf->l2_cfg.l2_ucAddr.ucast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXT_VALID_ADDR_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidAddr_get(buf->l2_cfg.l2_ucAddr.unit, &buf->l2_cfg.l2_ucAddr.scan_idx, buf->l2_cfg.l2_ucAddr.include_static, &buf->l2_cfg.l2_ucAddr.ucast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCAST_ADDR_INIT:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_init(buf->l2_cfg.l2_mcAddr.unit, buf->l2_cfg.l2_mcAddr.vid, &buf->l2_cfg.l2_mcAddr.mac, &buf->l2_cfg.l2_mcAddr.mcast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCAST_ADDR_ADD:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_add(buf->l2_cfg.l2_mcAddr.unit, &buf->l2_cfg.l2_mcAddr.mcast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCAST_ADDR_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_get(buf->l2_cfg.l2_mcAddr.unit, &buf->l2_cfg.l2_mcAddr.mcast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCAST_ADDR_SET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_set(buf->l2_cfg.l2_mcAddr.unit, &buf->l2_cfg.l2_mcAddr.mcast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCAST_ADDR_SET_BY_INDEX:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_setByIndex(buf->l2_cfg.l2_mcAddr.unit, &buf->l2_cfg.l2_mcAddr.mcast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXT_VALID_MCAST_ADDR_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidMcastAddr_get(buf->l2_cfg.l2_mcAddr.unit, &buf->l2_cfg.l2_mcAddr.scan_idx, &buf->l2_cfg.l2_mcAddr.mcast);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCMODE_GET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_ipmcMode_get(buf->l2_learnCfg.unit, &buf->l2_learnCfg.ipmcMode);
            rtk_copy_to(user, &buf->l2_learnCfg, sizeof(rtdrv_l2_learnCfg_t));
            break;

        case RTDRV_L2_IP_MCAST_ADDR_EX_INIT:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtdrv_l2_ipMcstAddrData_t));
            ret = rtk_l2_ipMcastAddrExt_init(buf->ipMcast_data.unit, &buf->ipMcast_data.ipMcast_hashKey, &buf->ipMcast_data.ip_m_data);
            rtk_copy_to(user, &buf->ipMcast_data, sizeof(rtdrv_l2_ipMcstAddrData_t));
            break;

        case RTDRV_L2_IP_MCAST_ADDR_GET:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtdrv_l2_ipMcstAddrData_t));
            ret = rtk_l2_ipMcastAddr_get(buf->ipMcast_data.unit, &buf->ipMcast_data.ip_m_data);
            rtk_copy_to(user, &buf->ipMcast_data, sizeof(rtdrv_l2_ipMcstAddrData_t));
            break;

        case RTDRV_L2_NEXT_VALID_IPMCASTADDR_GET:
            rtk_copy_from(&buf->ipMcast_data, user, sizeof(rtdrv_l2_ipMcstAddrData_t));
            ret = rtk_l2_nextValidIpMcastAddr_get(buf->ipMcast_data.unit, &buf->ipMcast_data.index, &buf->ipMcast_data.ip_m_data);
            rtk_copy_to(user, &buf->ipMcast_data, sizeof(rtdrv_l2_ipMcstAddrData_t));
            break;

        case RTDRV_L2_IPMC_DIP_CHK_GET:
            rtk_copy_from(&buf->l2_learnCfg, user, sizeof(rtdrv_l2_learnCfg_t));
            ret = rtk_l2_ipMcastAddrChkEnable_get(buf->l2_learnCfg.unit, &buf->l2_learnCfg.dip_check);
            rtk_copy_to(user, &buf->l2_learnCfg, sizeof(rtdrv_l2_learnCfg_t));
            break;

        case RTDRV_L2_IPMC_VLAN_COMPARE_GET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_ipMcstFidVidCompareEnable_get(buf->l2_common.unit, &buf->l2_common.value);
            rtk_copy_to(user, &buf->l2_common, sizeof(rtdrv_l2_common_t));
            break;

        case RTDRV_L2_IP6MCASTMODE_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_l2_ip6mcMode_get(buf->unit_cfg.unit, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_L2_HASHCAREBYTE_GET:
            rtk_copy_from(&buf->l2_hashCareByte, user, sizeof(rtdrv_l2_hashCareByte_t));
            ret = rtk_l2_ip6CareByte_get(buf->l2_hashCareByte.unit, buf->l2_hashCareByte.type, &buf->l2_hashCareByte.value);
            rtk_copy_to(user, &buf->l2_hashCareByte, sizeof(rtdrv_l2_hashCareByte_t));
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_EX_INIT:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddrExt_init(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.ip6Mcast_hashKey, &buf->ip6Mcast_data.ip6_m_data);
            rtk_copy_to(user, &buf->ip6Mcast_data, sizeof(rtdrv_l2_ip6McstAddrData_t));
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_GET:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_ip6McastAddr_get(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.ip6_m_data);
            rtk_copy_to(user, &buf->ip6Mcast_data, sizeof(rtdrv_l2_ip6McstAddrData_t));
            break;

        case RTDRV_L2_IP6_MCAST_ADDR_GETNEXT:
            rtk_copy_from(&buf->ip6Mcast_data, user, sizeof(rtdrv_l2_ip6McstAddrData_t));
            ret = rtk_l2_nextValidIp6McastAddr_get(buf->ip6Mcast_data.unit, &buf->ip6Mcast_data.index, &buf->ip6Mcast_data.ip6_m_data);
            rtk_copy_to(user, &buf->ip6Mcast_data, sizeof(rtdrv_l2_ip6McstAddrData_t));
            break;

        case RTDRV_L2_MCAST_FWD_INDEX_ALLOC:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastFwdIndex_alloc(buf->l2_cfg.l2_portmask.unit, &buf->l2_cfg.l2_portmask.index);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCASTFWDINDEXFREECOUNT_GET:
            rtk_copy_from(&buf->l2_fwdEntryContent, user, sizeof(rtdrv_l2_fwdTblEntry_t));
            ret = rtk_l2_mcastFwdIndexFreeCount_get(buf->l2_fwdEntryContent.unit, &buf->l2_fwdEntryContent.freeCount);
            rtk_copy_to(user, &buf->l2_fwdEntryContent, sizeof(rtdrv_l2_fwdTblEntry_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_MCAST_FWD_PORTMASK_GET:
            rtk_copy_from(&buf->l2_fwdEntryContent, user, sizeof(rtdrv_l2_fwdTblEntry_t));
            ret = rtk_l2_mcastFwdPortmask_get(buf->l2_fwdEntryContent.unit,
                buf->l2_fwdEntryContent.entryIdx, &buf->l2_fwdEntryContent.portMask, &buf->l2_fwdEntryContent.crossVlan);
            rtk_copy_to(user, &buf->l2_fwdEntryContent, sizeof(rtdrv_l2_fwdTblEntry_t));
            break;
#endif
        case RTDRV_L2_MCAST_FWD_PORTMASK_ENTRY_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastFwdPortmaskEntry_get(buf->l2_cfg.l2_portmask.unit, buf->l2_cfg.l2_portmask.index, &buf->l2_cfg.l2_portmask.portmask);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_L2_PORT_LEGAL_MOVETO_ACTION_GET:
            rtk_copy_from(&buf->l2_action, user, sizeof(rtdrv_l2_portAct_t));
            ret = rtk_l2_legalPortMoveAction_get(buf->l2_action.unit, buf->l2_action.port, &buf->l2_action.action);
            rtk_copy_to(user, &buf->l2_action, sizeof(rtdrv_l2_portAct_t));
            break;

        case RTDRV_L2_DYNM_PORTMOVE_FORBID_ACTION_GET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_dynamicPortMoveForbidAction_get(buf->l2_common.unit, &buf->l2_common.value);
            rtk_copy_to(user, &buf->l2_common, sizeof(rtdrv_l2_common_t));
            break;
#endif
        case RTDRV_L2_PORT_MOVE_ACT_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portMoveAction_get(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.type, &buf->l2_cfg.l2_portMove.action);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORT_MOVE_LEARN_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portMoveLearn_get(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.type, &buf->l2_cfg.l2_portMove.learn);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LEGAL_MOVETO_FLUSH_ENABLE_GET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_legalPortMoveFlushAddrEnable_get(buf->l2_common.unit, buf->l2_common.port, &buf->l2_common.value);
            rtk_copy_to(user, &buf->l2_common, sizeof(rtdrv_l2_common_t));
            break;

        case RTDRV_L2_STTC_PORT_MOVE_ACTION_GET:
            rtk_copy_from(&buf->l2_action, user, sizeof(rtdrv_l2_portAct_t));
            ret = rtk_l2_staticPortMoveAction_get(buf->l2_action.unit, buf->l2_action.port, &buf->l2_action.action);
            rtk_copy_to(user, &buf->l2_action, sizeof(rtdrv_l2_portAct_t));
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOOD_PORTMASK_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_get(buf->l2_cfg.l2_lookUpMiss.unit, buf->l2_cfg.l2_lookUpMiss.type, &buf->l2_cfg.l2_lookUpMiss.flood_portmask);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LOOKUP_MISS_FLOODPORTMASK_IDX_GET:
            rtk_copy_from(&buf->l2_lkMiss, user, sizeof(rtdrv_l2_lkMiss_t));
            ret = rtk_l2_lookupMissFloodPortMaskIdx_get(buf->l2_lkMiss.unit, buf->l2_lkMiss.type, &buf->l2_lkMiss.index);
            rtk_copy_to(user, &buf->l2_lkMiss, sizeof(rtdrv_l2_lkMiss_t));
            break;

        case RTDRV_L2_PORT_LOOKUP_MISS_ACTION_GET:
            rtk_copy_from(&buf->l2_lkMiss, user, sizeof(rtdrv_l2_lkMiss_t));
            ret = rtk_l2_portLookupMissAction_get(buf->l2_lkMiss.unit, buf->l2_lkMiss.port, buf->l2_lkMiss.type, &buf->l2_lkMiss.action);
            rtk_copy_to(user, &buf->l2_lkMiss, sizeof(rtdrv_l2_lkMiss_t));
            break;

        case RTDRV_L2_PORT_UCAST_LOOKUP_MISS_ACTION_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portUcastLookupMissAction_get(buf->l2_cfg.l2_lookUpMiss.unit, buf->l2_cfg.l2_lookUpMiss.port, &buf->l2_cfg.l2_lookUpMiss.action);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_SRC_PORT_EGR_FILTER_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_srcPortEgrFilterMask_get(buf->l2_cfg.l2_common.unit, &buf->l2_cfg.l2_common.srcPortFilterPortmask);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_EXCEPTION_ADDR_ACTION_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_exceptionAddrAction_get(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.exceptType, &buf->l2_cfg.l2_common.action);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_ENTRY_GET:
            rtk_copy_from(&buf->l2_search, user, sizeof(rtdrv_l2Cfg_search_t));
            ret = rtk_l2_addrEntry_get(buf->l2_search.unit, buf->l2_search.index, &buf->l2_search.l2_entry);
            rtk_copy_to(user, &buf->l2_search, sizeof(rtdrv_l2Cfg_search_t));
            break;

        case RTDRV_L2_CONFLICT_ADDR_GET:
            rtk_copy_from(&buf->l2_search, user, sizeof(rtdrv_l2Cfg_search_t));
            ret = rtk_l2_conflictAddr_get(buf->l2_search.unit, &buf->l2_search.l2_entry, buf->l2_search.cfAddrList, buf->l2_search.cfAddrList_size, &buf->l2_search.cf_retCnt);
            rtk_copy_to(user, &buf->l2_search, sizeof(rtdrv_l2Cfg_search_t));
            break;

        case RTDRV_L2_ZERO_SA_LEARNING_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_zeroSALearningEnable_get(buf->l2_cfg.l2_learn.unit, &buf->l2_cfg.l2_learn.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_SECURE_MAC_MODE_GET:
            rtk_copy_from(&buf->l2_common, user, sizeof(rtdrv_l2_common_t));
            ret = rtk_l2_secureMacMode_get(buf->l2_common.unit, &buf->l2_common.value);
            rtk_copy_to(user, &buf->l2_common, sizeof(rtdrv_l2_common_t));
            break;

        case RTDRV_L2_PORT_DYNM_PORTMOVE_FORBID_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portDynamicPortMoveForbidEnable_get(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.port, &buf->l2_cfg.l2_portMove.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_TRK_DYNM_PORTMOVE_FORBID_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_trkDynamicPortMoveForbidEnable_get(buf->l2_cfg.l2_portMove.unit, buf->l2_cfg.l2_portMove.trunk, &buf->l2_cfg.l2_portMove.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORT_MAC_FILTER_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portMacFilterEnable_get(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.port, buf->l2_cfg.l2_common.filterMode, &buf->l2_cfg.l2_common.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_HW_NEXT_VALID_ADDR_GET:
            rtk_copy_from(&buf->l2_search, user, sizeof(rtdrv_l2Cfg_search_t));
            ret = rtk_l2_hwNextValidAddr_get(buf->l2_search.unit, &buf->l2_search.scan_idx, buf->l2_search.type, &buf->l2_search.l2_entry);
            rtk_copy_to(user, &buf->l2_search, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORT_CTRL_TYPE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portCtrl_get(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.port, buf->l2_cfg.l2_common.portCtrlType, &buf->l2_cfg.l2_common.arg);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_STATUS_TYPE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_status_get(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.stsType, buf->l2_cfg.l2_common.port, &buf->l2_cfg.l2_common.value);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_STK_LEARNING_ENABLE_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_stkLearningEnable_get(buf->l2_cfg.l2_learn.unit, &buf->l2_cfg.l2_learn.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_STK_KEEP_AGE_VALID_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_stkKeepUcastEntryValid_get(buf->l2_cfg.l2_age.unit, &buf->l2_cfg.l2_age.enable);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ENTRY_CNT_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_entryCnt_get(buf->l2_cfg.l2_common.unit, buf->l2_cfg.l2_common.entryType, &buf->l2_cfg.l2_common.value);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MAC_HASHIDX_GET:
            rtk_copy_from(&buf->l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_hashIdx_get(buf->l2_cfg.l2_common.unit, &buf->l2_cfg.l2_common.macHashIdx);
            rtk_copy_to(user, &buf->l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}



int32 do_rtdrv_get_ctl_l2ntfy(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    /*L2 Notification module*/
        case RTDRV_L2NTFY_ENABLE_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_enable_get(buf->l2ntfy_cfg.unit, &buf->l2ntfy_cfg.enable);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_BACK_PRESSURE_THR_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_backPressureThresh_get(buf->l2ntfy_cfg.unit, &buf->l2ntfy_cfg.thresh);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_EVENT_ENABLE_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_notificationEventEnable_get(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.event, &buf->l2ntfy_cfg.enable);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_DBG_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_debug_get(buf->l2ntfy_cfg.unit, &buf->l2ntfy_cfg.dbgFlag);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_DST_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_dst_get(buf->l2ntfy_cfg.unit, &buf->l2ntfy_cfg.dst);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_MAGIC_NUM_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_magicNum_get(buf->l2ntfy_cfg.unit, &buf->l2ntfy_cfg.magicNum);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_MACADDR_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_macAddr_get(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.addrType, &buf->l2ntfy_cfg.mac);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_MAXEVENT_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_maxEvent_get(buf->l2ntfy_cfg.unit, &buf->l2ntfy_cfg.maxEvent);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;

        case RTDRV_L2NTFY_TIMEOUT_GET:
            rtk_copy_from(&buf->l2ntfy_cfg, user, sizeof(rtdrv_l2ntfyCfg_t));
            ret = drv_l2ntfy_timeout_get(buf->l2ntfy_cfg.unit, buf->l2ntfy_cfg.mode, &buf->l2ntfy_cfg.timeout);
            rtk_copy_to(user, &buf->l2ntfy_cfg, sizeof(rtdrv_l2ntfyCfg_t));
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}




int32 do_rtdrv_get_ctl_l3(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /*L3*/
        case RTDRV_L3_ROUTE_ROUTEENTRY_GET:
            rtk_copy_from(&buf->l3_route_entry, user, sizeof(rtdrv_l3_routeEntry_t));
            ret = rtk_l3_routeEntry_get(buf->l3_route_entry.unit, buf->l3_route_entry.index, &buf->l3_route_entry.entry);
            rtk_copy_to(user, &buf->l3_route_entry, sizeof(rtdrv_l3_routeEntry_t));
            break;

        case RTDRV_L3_ROUTE_SWITCHMACADDR_GET:
            rtk_copy_from(&buf->l3_config, user, sizeof(rtdrv_l3_config_t));
            ret = rtk_l3_routeSwitchMacAddr_get(buf->l3_config.unit, buf->l3_config.index, &buf->l3_config.mac);
            rtk_copy_to(user, &buf->l3_config, sizeof(rtdrv_l3_config_t));
            break;

        case RTDRV_L3_INFO_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_info_get(buf->l3_cfg.unit, &buf->l3_cfg.info);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ROUTERMACENTRY_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_routerMacEntry_get(buf->l3_cfg.unit, buf->l3_cfg.index, &buf->l3_cfg.macEntry);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_INTF_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intf_get(buf->l3_cfg.unit, buf->l3_cfg.type, &buf->l3_cfg.intf);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_INTFSTATS_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intfStats_get(buf->l3_cfg.unit, buf->l3_cfg.intfId, &buf->l3_cfg.stats);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_VRRP_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_vrrp_get(buf->l3_cfg.unit, buf->l3_cfg.vid, buf->l3_cfg.vrIdArraySize, &buf->l3_cfg.vrIdArray[0], &buf->l3_cfg.vrIdCount);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_NEXTHOP_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_nextHop_get(buf->l3_cfg.unit, buf->l3_cfg.nhId, &buf->l3_cfg.nextHop);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_NEXTHOPPATH_FIND:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_nextHopPath_find(buf->l3_cfg.unit, &buf->l3_cfg.nextHop, &buf->l3_cfg.nhId);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ECMP_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_ecmp_get(buf->l3_cfg.unit, buf->l3_cfg.ecmpId, buf->l3_cfg.nhIdArraySize, &buf->l3_cfg.nhIdArray[0], &buf->l3_cfg.nhIdCount);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ECMP_FIND:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_ecmp_find(buf->l3_cfg.unit, buf->l3_cfg.nhIdCount, &buf->l3_cfg.nhIdArray[0], &buf->l3_cfg.ecmpId);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_HOST_FIND:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_find(buf->l3_cfg.unit, &buf->l3_cfg.host);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_HOSTCONFLICT_GET:
            {
                rtk_l3_host_t *pHosts;
                rtdrv_l3Cfg_t *pUser = user;

                rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
                if (NULL == (pHosts = osal_alloc(sizeof(rtk_l3_host_t) * buf->l3_cfg.maxHost)))
                {
                    ret = RT_ERR_FAILED;
                }
                else
                {
                    ret = rtk_l3_hostConflict_get(buf->l3_cfg.unit, &buf->l3_cfg.key, pHosts, buf->l3_cfg.maxHost, &buf->l3_cfg.hostCount);
                    rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
                    rtk_copy_to(pUser->pHostArray, pHosts, sizeof(rtk_l3_host_t) * buf->l3_cfg.maxHost);
                    osal_free(pHosts);
                }
            }
            break;

        case RTDRV_L3_HOST_AGE:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_age(buf->l3_cfg.unit, buf->l3_cfg.flags, buf->l3_cfg.fHostCb, (void *)buf->l3_cfg.cookie);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_HOST_GETNEXT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_host_getNext(buf->l3_cfg.unit, buf->l3_cfg.flags, &buf->l3_cfg.base, &buf->l3_cfg.host);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ROUTE_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_get(buf->l3_cfg.unit, &buf->l3_cfg.route);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ROUTE_AGE:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_age(buf->l3_cfg.unit, buf->l3_cfg.flags, buf->l3_cfg.fRouteCb, (void *)buf->l3_cfg.cookie);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_ROUTE_GETNEXT:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_route_getNext(buf->l3_cfg.unit, buf->l3_cfg.flags, &buf->l3_cfg.base, &buf->l3_cfg.route);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_GLOBALCTRL_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_globalCtrl_get(buf->l3_cfg.unit, buf->l3_cfg.globalCtrlType, &buf->l3_cfg.arg);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_INTFCTRL_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_intfCtrl_get(buf->l3_cfg.unit, buf->l3_cfg.intfId, buf->l3_cfg.type, &buf->l3_cfg.arg);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;

        case RTDRV_L3_PORTCTRL_GET:
            rtk_copy_from(&buf->l3_cfg, user, sizeof(rtdrv_l3Cfg_t));
            ret = rtk_l3_portCtrl_get(buf->l3_cfg.unit, buf->l3_cfg.port, buf->l3_cfg.type, &buf->l3_cfg.arg);
            rtk_copy_to(user, &buf->l3_cfg, sizeof(rtdrv_l3Cfg_t));
            break;


        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_mcast(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        /*MCAST*/
        case RTDRV_MCAST_GROUP_CREATE:
            rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
            ret = rtk_mcast_group_create(buf->mcast_cfg.unit, buf->mcast_cfg.flags, buf->mcast_cfg.type, &buf->mcast_cfg.group);
            rtk_copy_to(user, &buf->mcast_cfg, sizeof(rtdrv_mcastCfg_t));
            break;

        case RTDRV_MCAST_GROUP_GETNEXT:
            rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
            ret = rtk_mcast_group_getNext(buf->mcast_cfg.unit, buf->mcast_cfg.type, &buf->mcast_cfg.base, &buf->mcast_cfg.group);
            rtk_copy_to(user, &buf->mcast_cfg, sizeof(rtdrv_mcastCfg_t));
            break;

        case RTDRV_MCAST_NEXTHOP_GET:
            {
                rtk_mcast_egrif_t      *kNhArry;
                rtdrv_mcastCfg_t        *pUser = user;
                rtk_copy_from(&buf->mcast_cfg, user, sizeof(rtdrv_mcastCfg_t));
                kNhArry = osal_alloc(sizeof(rtk_mcast_egrif_t) * buf->mcast_cfg.maxNum);
                if (kNhArry == NULL)
                {
                    ret = RT_ERR_FAILED;
                }
                else
                {
                    ret = rtk_mcast_egrIf_get(buf->mcast_cfg.unit, buf->mcast_cfg.group, buf->mcast_cfg.maxNum, kNhArry, &buf->mcast_cfg.nexthopNum);
                    rtk_copy_to(user, &buf->mcast_cfg, sizeof(rtdrv_mcastCfg_t));
                    rtk_copy_to(pUser->pNhArry, kNhArry, sizeof(rtk_mcast_egrif_t) * buf->mcast_cfg.maxNum);
                    osal_free(kNhArry);
                }
                break;
            }
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_ipmc(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        /*IPMCAST*/
        case RTDRV_IPMC_ADDR_INIT:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_addr_t_init(&buf->ipmc_cfg.ipmcEntry);
            rtk_copy_to(user, &buf->ipmc_cfg, sizeof(rtdrv_ipmcCfg_t));
            break;

        case RTDRV_IPMC_ADDR_FIND:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_addr_find(buf->ipmc_cfg.unit, &buf->ipmc_cfg.ipmcEntry);
            rtk_copy_to(user, &buf->ipmc_cfg, sizeof(rtdrv_ipmcCfg_t));
            break;

        case RTDRV_IPMC_NEXTVALID_ADDR_GET:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_nextValidAddr_get(buf->ipmc_cfg.unit, buf->ipmc_cfg.flags, &buf->ipmc_cfg.base, &buf->ipmc_cfg.ipmcEntry);
            rtk_copy_to(user, &buf->ipmc_cfg, sizeof(rtdrv_ipmcCfg_t));
            break;

        case RTDRV_IPMC_STAT_GET:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_statCntr_get(buf->ipmc_cfg.unit, &buf->ipmc_cfg.statKey, &buf->ipmc_cfg.statCnt);
            rtk_copy_to(user, &buf->ipmc_cfg, sizeof(rtdrv_ipmcCfg_t));
            break;

        case RTDRV_IPMC_GLOBALCTRL_GET:
            rtk_copy_from(&buf->ipmc_cfg, user, sizeof(rtdrv_ipmcCfg_t));
            ret = rtk_ipmc_globalCtrl_get(buf->ipmc_cfg.unit, buf->ipmc_cfg.globalCtrlType, &buf->ipmc_cfg.arg);
            rtk_copy_to(user, &buf->ipmc_cfg, sizeof(rtdrv_ipmcCfg_t));
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_tunnel(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /* Tunnel */
#if defined(CONFIG_SDK_RTL9310)
        case RTDRV_TUNNEL_INFO_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_info_get(buf->tunnel_cfg.unit, &buf->tunnel_cfg.info);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTF_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intf_get(buf->tunnel_cfg.unit, &buf->tunnel_cfg.intf);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTFPATHID_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intfPathId_get(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId, &buf->tunnel_cfg.pathId);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTFPATH_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intfPath_get(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId, &buf->tunnel_cfg.nhDmacIdx, &buf->tunnel_cfg.l3EgrIntfIdx);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_INTFSTATS_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_intfStats_get(buf->tunnel_cfg.unit, buf->tunnel_cfg.intfId, &buf->tunnel_cfg.stats);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_QOSPROFILE_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_qosProfile_get(buf->tunnel_cfg.unit, buf->tunnel_cfg.idx, &buf->tunnel_cfg.profile);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;

        case RTDRV_TUNNEL_GLOBALCTRL_GET:
            rtk_copy_from(&buf->tunnel_cfg, user, sizeof(rtdrv_tunnelCfg_t));
            ret = rtk_tunnel_globalCtrl_get(buf->tunnel_cfg.unit, buf->tunnel_cfg.type, &buf->tunnel_cfg.arg);
            rtk_copy_to(user, &buf->tunnel_cfg, sizeof(rtdrv_tunnelCfg_t));
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_get_ctl_vxlan(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /* Tunnel */
#if defined(CONFIG_SDK_RTL9310)
        case RTDRV_VXLAN_VNI_GET:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_vni_get(buf->vxlan_cfg.unit, &buf->vxlan_cfg.entry);
            rtk_copy_to(user, &buf->vxlan_cfg, sizeof(rtdrv_vxlanCfg_t));
            break;

        case RTDRV_VXLAN_VNI_GETNEXT:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_vni_getNext(buf->vxlan_cfg.unit, &buf->vxlan_cfg.base, &buf->vxlan_cfg.entry);
            rtk_copy_to(user, &buf->vxlan_cfg, sizeof(rtdrv_vxlanCfg_t));
            break;

        case RTDRV_VXLAN_GLOBALCTRL_GET:
            rtk_copy_from(&buf->vxlan_cfg, user, sizeof(rtdrv_vxlanCfg_t));
            ret = rtk_vxlan_globalCtrl_get(buf->vxlan_cfg.unit, buf->vxlan_cfg.type, &buf->vxlan_cfg.arg);
            rtk_copy_to(user, &buf->vxlan_cfg, sizeof(rtdrv_vxlanCfg_t));
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_port(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** PORT **/
        case RTDRV_PORT_LINK_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_link_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SPEED_DUPLEX_GET:
            rtk_copy_from(&buf->speed_duplex, user, sizeof(rtdrv_port_speedDuplex_t));
            ret = rtk_port_speedDuplex_get(buf->speed_duplex.unit, buf->speed_duplex.port, &buf->speed_duplex.speed,
                                           &buf->speed_duplex.duplex);
            rtk_copy_to(user, &buf->speed_duplex, sizeof(rtdrv_port_speedDuplex_t));
            break;

        case RTDRV_PORT_FLOW_CTRL_GET:
            rtk_copy_from(&buf->port_flowctrl, user, sizeof(rtdrv_port_flowctrl_t));
            ret = rtk_port_flowctrl_get(buf->port_flowctrl.unit, buf->port_flowctrl.port, &buf->port_flowctrl.tx_status,
                                        &buf->port_flowctrl.rx_status);
            rtk_copy_to(user, &buf->port_flowctrl, sizeof(rtdrv_port_flowctrl_t));
            break;

        case RTDRV_PORT_EN_AUTONEGO_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyAutoNegoEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_AUTONEGO_ABIL_GET:
            rtk_copy_from(&buf->autonego_ability, user, sizeof(rtdrv_port_autoNegoAbility_t));
            ret = rtk_port_phyAutoNegoAbility_get(buf->autonego_ability.unit, buf->autonego_ability.port,
                                                  &buf->autonego_ability.ability);
            rtk_copy_to(user, &buf->autonego_ability, sizeof(rtdrv_port_autoNegoAbility_t));
            break;

        case RTDRV_PORT_FORCE_MODE_ABIL_GET:
            rtk_copy_from(&buf->forcemode_ability, user, sizeof(rtdrv_port_forceModeAbility_t));
            ret = rtk_port_phyForceModeAbility_get(buf->forcemode_ability.unit, buf->forcemode_ability.port,
                                                   &buf->forcemode_ability.speed, &buf->forcemode_ability.duplex,
                                                   &buf->forcemode_ability.flowctrl);
            rtk_copy_to(user, &buf->forcemode_ability, sizeof(rtdrv_port_forceModeAbility_t));
            break;

        case RTDRV_PORT_CPU_PORT_ID_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_port_cpuPortId_get(buf->unit_cfg.unit, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_PORT_PHY_REG_GET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phyReg_get(buf->phy_data.unit, buf->phy_data.port, buf->phy_data.page, buf->phy_data.reg,
                                      &buf->phy_data.data);
            rtk_copy_to(user, &buf->phy_data, sizeof(rtdrv_port_phyReg_t));
            break;

        case RTDRV_PORT_PHY_EXT_PARK_PAGE_REG_GET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phyExtParkPageReg_get(buf->phy_data.unit, buf->phy_data.port, buf->phy_data.page, buf->phy_data.extPage,
                buf->phy_data.parkPage, buf->phy_data.reg, &buf->phy_data.data);
            rtk_copy_to(user, &buf->phy_data, sizeof(rtdrv_port_phyReg_t));
            break;

        case RTDRV_PORT_PHY_MMD_REG_GET:
            rtk_copy_from(&buf->phy_data, user, sizeof(rtdrv_port_phyReg_t));
            ret = rtk_port_phyMmdReg_get(buf->phy_data.unit, buf->phy_data.port, buf->phy_data.mmdAddr, buf->phy_data.reg, &buf->phy_data.data);
            rtk_copy_to(user, &buf->phy_data, sizeof(rtdrv_port_phyReg_t));
            break;

        case RTDRV_PORT_MASTER_SLAVE_GET:
            rtk_copy_from(&buf->masterSlave_cfg, user, sizeof(rtdrv_port_masterSlave_t));
            ret = rtk_port_phyMasterSlave_get(buf->masterSlave_cfg.unit, buf->masterSlave_cfg.port, &buf->masterSlave_cfg.masterSlaveCfg, &buf->masterSlave_cfg.masterSlaveActual);
            rtk_copy_to(user, &buf->masterSlave_cfg, sizeof(rtdrv_port_masterSlave_t));
            break;

        case RTDRV_PORT_ISOLATION_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolation_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.portmask);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONEXT_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationExt_get(buf->port_cfg.unit, buf->port_cfg.devID, buf->port_cfg.port, &buf->port_cfg.portmask);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATION_RESTRICT_ROUTE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationRestrictRoute_get(buf->port_cfg.unit, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_EN_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_adminEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_BACK_PRESSURE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_backpressureEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHY_MEDIA_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyComboPortMedia_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.media);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_GREEN_ENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_greenEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_GIGA_LITE_ENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_gigaLiteEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_2PT5G_LITE_ENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_2pt5gLiteEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHY_CROSSOVERMODE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyCrossOverMode_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_TX_EN_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_txEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_RX_EN_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_rxEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHY_FIBER_MEDIA_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyComboPortFiberMedia_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.fiber_media);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_LINKMEDIA_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_linkMedia_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data, &buf->port_cfg.media);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_LINKDOWN_POWERSAVING_ENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_linkDownPowerSavingEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_VLAN_ISOLATION_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_vlanBasedIsolationEntry_get(buf->port_cfg.unit, buf->port_cfg.index, &buf->port_cfg.vlanIsoEntry);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_VLAN_ISOLATION_VLANSOURCE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_vlanBasedIsolation_vlanSource_get(buf->port_cfg.unit, &buf->port_cfg.vlanIsoSrc);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_VLAN_ISOLATION_EGRBYPASS_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_vlanBasedIsolationEgrBypass_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_DOWNSPEEDENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_downSpeedEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_FIBERDOWNSPEEDENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberDownSpeedEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_FIBERNWAYFORCELINKENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberNwayForceLinkEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_FIBERUNIDIRENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberUnidirEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.enable);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYLOOPBACKENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyLoopBackEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.enable);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_10GMEDIA_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_10gMedia_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.media_10g);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHY_CROSSOVERSTATUS_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyCrossOverStatus_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
        break;

        case RTDRV_PORT_FIBERRXENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_fiberRxEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.enable);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_PHYPOLAR_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyPolar_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.polarCtrl);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_PHYSDS_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phySds_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.sdsCfg);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_PHYSDSRXCALISTATUS_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phySdsRxCaliStatus_get(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.data, &buf->port_cfg.phySdsRxCaliStatus);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_FORCE_MODE_FLOW_CTRL_MODE_GET:
        {
            rtk_port_flowctrl_mode_t    fcMode;
            rtk_copy_from(&buf->port_flowctrl, user, sizeof(rtdrv_port_flowctrl_t));
            ret = rtk_port_phyForceFlowctrlMode_get(buf->port_flowctrl.unit, buf->port_flowctrl.port, &fcMode);
            buf->port_flowctrl.tx_status = fcMode.tx_pause;
            buf->port_flowctrl.rx_status = fcMode.rx_pause;
            rtk_copy_to(user, &buf->port_flowctrl, sizeof(rtdrv_port_flowctrl_t));
            break;
        }
        case RTDRV_PORT_PHYLINKSTATUS_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyLinkStatus_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYPEERAUTONEGOABILITY_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_port_autoNegoAbility_t));
            ret = rtk_port_phyPeerAutoNegoAbility_get(buf->autonego_ability.unit, buf->autonego_ability.port, &buf->autonego_ability.ability);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_port_autoNegoAbility_t));
            break;

        case RTDRV_PORT_PHYMACINTFSERDESMODE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyMacIntfSerdesMode_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.sdsCfg.sdsMode);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYLEDCTRL_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyLedCtrl_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.phyLedCtrl);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYMACINTFSERDESLINKSTATUS_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyMacIntfSerdesLinkStatus_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.status);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYEYEPARAM_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phySdsEyeParam_get(buf->port_cfg.unit, buf->port_cfg.port, buf->port_cfg.sdsId, &buf->port_cfg.eyeParam);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_MDI_LOOPBACK_ENABLE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyMdiLoopbackEnable_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.enable);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SPECL_CGST_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_specialCongest_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_oam(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
     /*OAM*/
        case RTDRV_OAM_AUTODYINGGASPENABLE_GET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_autoDyingGaspEnable_get(buf->dyingGasp_cfg.unit, buf->dyingGasp_cfg.port, &buf->dyingGasp_cfg.enable);
            rtk_copy_to(user, &buf->dyingGasp_cfg, sizeof(rtdrv_oamDyingGaspCfg_t));
            break;

        case RTDRV_OAM_DYINGGASPWAITTIME_GET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_dyingGaspWaitTime_get(buf->dyingGasp_cfg.unit, &buf->dyingGasp_cfg.waitTime);
            rtk_copy_to(user, &buf->dyingGasp_cfg, sizeof(rtdrv_oamDyingGaspCfg_t));
            break;

        case RTDRV_OAM_DYINGGASPPKTCNT_GET:
            rtk_copy_from(&buf->dyingGasp_cfg, user, sizeof(rtdrv_oamDyingGaspCfg_t));
            ret = rtk_oam_dyingGaspPktCnt_get(buf->dyingGasp_cfg.unit, &buf->dyingGasp_cfg.cnt);
            rtk_copy_to(user, &buf->dyingGasp_cfg, sizeof(rtdrv_oamDyingGaspCfg_t));
            break;

        case RTDRV_OAM_LOOPBACKMACSWAPENABLE_GET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_oam_loopbackMacSwapEnable_get(buf->misc_cfg.unit,
                                                    &buf->misc_cfg.loopbackEnable);
            rtk_copy_to(user, &buf->misc_cfg, sizeof(rtdrv_oamCfmMiscCfg_t));
            break;

        case RTDRV_OAM_PORTLOOPBACKMUXACTION_GET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_oam_portLoopbackMuxAction_get(buf->misc_cfg.unit,
                    buf->misc_cfg.port, &buf->misc_cfg.action);
            rtk_copy_to(user, &buf->misc_cfg, sizeof(rtdrv_oamCfmMiscCfg_t));
            break;

        case RTDRV_OAM_PDULEARNINGENABLE_GET:
            rtk_copy_from(&buf->oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_pduLearningEnable_get(buf->oam_cfg.unit, &buf->oam_cfg.enable);
            rtk_copy_to(user, &buf->oam_cfg, sizeof(rtdrv_oamCfg_t));
            break;

        case RTDRV_OAM_CFMCCMPCP_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmPcp_get(buf->ccm_cfg.unit,
                                        &buf->ccm_cfg.ccmFrame.outer_pri);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMCFI_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmCfi_get(buf->ccm_cfg.unit,
                                        &buf->ccm_cfg.ccmFrame.outer_dei);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMTPID_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmTpid_get(buf->ccm_cfg.unit,
                                         &buf->ccm_cfg.ccmFrame.outer_tpid);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMRESETLIFETIME_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstLifetime_get(buf->ccm_cfg.unit,
                                                  buf->ccm_cfg.cfmIdx,
                                                  &buf->ccm_cfg.ccmFlag);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMMEPID_GET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_oam_cfmCcmMepid_get(buf->misc_cfg.unit,
                                          &buf->misc_cfg.mepid);
            rtk_copy_to(user, &buf->misc_cfg, sizeof(rtdrv_oamCfmMiscCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINTERVALFIELD_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmIntervalField_get(buf->ccm_cfg.unit,
                                                  &buf->ccm_cfg.ccmFlag);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMMDL_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmMdl_get(buf->cfm_cfg.unit,
                                        &buf->cfm_cfg.cfmCfg.md_level);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTTXMEPID_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxMepid_get(buf->cfm_cfg.unit,
                    buf->cfm_cfg.instance, &buf->cfm_cfg.mepid);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTTXINTERVALFIELD_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxIntervalField_get(buf->cfm_cfg.unit,
                    buf->cfm_cfg.instance, &buf->cfm_cfg.interval);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTTXMDL_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxMdl_get(buf->cfm_cfg.unit,
                    buf->cfm_cfg.instance, &buf->cfm_cfg.mdl);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTTAGSTATUS_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTagStatus_get(buf->cfm_cfg.unit,
                                                  buf->cfm_cfg.cfmIdx,
                                                  &buf->cfm_cfg.enable);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTVID_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstVid_get(buf->ccm_cfg.unit,
                                            buf->ccm_cfg.cfmIdx,
                                            &buf->ccm_cfg.ccmFrame.outer_vid);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTMAID_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstMaid_get(buf->cfm_cfg.unit,
                                             buf->cfm_cfg.cfmIdx,
                                             &buf->cfm_cfg.maid);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTTXSTATUS_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmCcmInstTxStatus_get(buf->cfm_cfg.unit,
                                                 buf->cfm_cfg.cfmIdx,
                                                 &buf->cfm_cfg.enable);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTINTERVAL_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstInterval_get(buf->ccm_cfg.unit,
                                                 buf->ccm_cfg.cfmIdx,
                                                 &buf->ccm_cfg.ccmInterval);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMTXINSTPORT_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmTxInstPort_get(buf->ccm_cfg.unit,
                                               buf->ccm_cfg.cfmIdx,
                                               buf->ccm_cfg.portIdx,
                                               &buf->ccm_cfg.port);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMRXINSTVID_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmRxInstVid_get(buf->ccm_cfg.unit,
                                              buf->ccm_cfg.cfmIdx,
                                              &buf->ccm_cfg.ccmFrame.outer_vid);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMRXINSTPORT_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmRxInstPort_get(buf->ccm_cfg.unit,
                                               buf->ccm_cfg.cfmIdx,
                                               buf->ccm_cfg.portIdx,
                                               &buf->ccm_cfg.port);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMKEEPALIVE_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstAliveTime_get(buf->ccm_cfg.unit,
                                              buf->ccm_cfg.cfmIdx,
                                              buf->ccm_cfg.portIdx,
                                              &buf->ccm_cfg.ccmInterval);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTTXMEMBER_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstTxMember_get(buf->ccm_cfg.unit, buf->ccm_cfg.instance, &buf->ccm_cfg.member);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMCCMINSTRXMEMBER_GET:
            rtk_copy_from(&buf->ccm_cfg, user, sizeof(rtdrv_oamCcmCfg_t));
            ret = rtk_oam_cfmCcmInstRxMember_get(buf->ccm_cfg.unit, buf->ccm_cfg.instance, &buf->ccm_cfg.member);
            rtk_copy_to(user, &buf->ccm_cfg, sizeof(rtdrv_oamCcmCfg_t));
            break;

        case RTDRV_OAM_CFMETHDMPORTENABLE_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmPortEthDmEnable_get(buf->cfm_cfg.unit,
                                              buf->cfm_cfg.port,
                                              &buf->cfm_cfg.enable);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        case RTDRV_OAM_CFMETHDMRXTIMESTAMP_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRxTimestamp_get(buf->cfm_cfg.unit,
                                              buf->cfm_cfg.index,
                                              &buf->cfm_cfg.timeStamp);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;
        case RTDRV_OAM_CFMETHDMTXDELAY_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmTxDelay_get(buf->cfm_cfg.unit,
                                              &buf->cfm_cfg.txDelay);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;
        case RTDRV_OAM_CFMETHDMREFTIME_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRefTime_get(buf->cfm_cfg.unit,
                                              &buf->cfm_cfg.timeStamp);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;
        case RTDRV_OAM_CFMETHDMREFTIMEENABLE_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRefTimeEnable_get(buf->cfm_cfg.unit,
                                              &buf->cfm_cfg.enable);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;
        case RTDRV_OAM_CFMETHDMREFTIMEFREQ_GET:
            rtk_copy_from(&buf->cfm_cfg, user, sizeof(rtdrv_oamCfmCfg_t));
            ret = rtk_oam_cfmEthDmRefTimeFreq_get(buf->cfm_cfg.unit,
                                              &buf->cfm_cfg.freq);
            rtk_copy_to(user, &buf->cfm_cfg, sizeof(rtdrv_oamCfmCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_vlan(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {

    /** VLAN **/
        case RTDRV_VLAN_PORT_GET:
            rtk_copy_from(&buf->vlan_port_data, user, sizeof(rtdrv_vlan_port_t));
            ret = rtk_vlan_port_get(buf->vlan_port_data.unit, buf->vlan_port_data.vid, &buf->vlan_port_data.member,
                                    &buf->vlan_port_data.untag);
            rtk_copy_to(user, &buf->vlan_port_data, sizeof(rtdrv_vlan_port_t));
            break;

        case RTDRV_VLAN_MCASTGROUP_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_mcastGroup_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.groupId);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_PVID_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portPvid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_VLAN_PROTO_GROUP_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_protoGroup_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.protoGroup);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_PROTO_VLAN_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portProtoVlan_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.idx, &buf->vlan_cfg.protoVlanCfg);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGR_TPID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrTpid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGR_TPID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTpid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGR_TPID_SRC_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTpidSrc_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGR_EXTRA_TAG_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrExtraTagEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGR_VLAN_TRANSPARENT_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanTransparentEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGR_VLAN_TRANSPARENT_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanTransparentEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_ACCEPT_FRAME_TYPE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portAcceptFrameType_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;


        case RTDRV_VLAN_EN_MCAST_LEAKY_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rtk_vlan_mcastLeakyEnable_get(buf->unit_cfg.unit, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_VLAN_SVLMODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_svlMode_get(buf->vlan_cfg.unit, &buf->vlan_cfg.mode);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_SVLFID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_svlFid_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.fid);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_STG_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_stg_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_L2_LOOKUP_SVL_FID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2LookupSvlFid_get(buf->vlan_cfg.unit, buf->vlan_cfg.macType, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGR_FILTER_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrFilterEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_LUTMODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2LookupMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid,buf->vlan_cfg.macType, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_GROUPMASK_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_groupMask_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.groupMask);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PROFILE_IDX_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_profileIdx_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PROFILE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_profile_get(buf->vlan_cfg.unit, buf->vlan_cfg.data, &buf->vlan_cfg.profile);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_FORWARD_VLAN_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portFwdVlan_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.tagMode, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGR_FILTER_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrFilter_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGRTAGKEEPTYPE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrTagKeepType_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data, &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGRTAGKEEPTYPE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTagKeepType_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data, &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_PVID_MODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPvidMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.port,buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_MAC_BASED_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlan_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    &buf->vlan_cfg.data, &buf->vlan_cfg.mac, &buf->vlan_cfg.vid,
                    &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_MAC_BASED_WITH_MSK_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanWithMsk_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    &buf->vlan_cfg.data, &buf->vlan_cfg.mac, &buf->vlan_cfg.msk, &buf->vlan_cfg.vid,
                    &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_MAC_BASED_WITH_PORT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanWithPort_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    &buf->vlan_cfg.data, &buf->vlan_cfg.mac, &buf->vlan_cfg.msk,
                    &buf->vlan_cfg.port, &buf->vlan_cfg.port_msk, &buf->vlan_cfg.vid, &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_MAC_BASED_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portMacBasedVlanEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port,&buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_MAC_BASED_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_macBasedVlanEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,&buf->vlan_cfg.macEntry);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlan_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    &buf->vlan_cfg.data, &buf->vlan_cfg.sip, &buf->vlan_cfg.sip_msk, &buf->vlan_cfg.vid,
                    &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_WITH_PORT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlanWithPort_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,
                    &buf->vlan_cfg.data, &buf->vlan_cfg.sip, &buf->vlan_cfg.sip_msk,
                    &buf->vlan_cfg.port, &buf->vlan_cfg.port_msk, &buf->vlan_cfg.vid, &buf->vlan_cfg.data1);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIpSubnetBasedVlanEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port,&buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IP_SUBNET_BASED_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ipSubnetBasedVlanEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx,&buf->vlan_cfg.ipEntry);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TPID_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tpidEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.tagType, buf->vlan_cfg.idx, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGR_TAG_STS_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTagSts_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IGRVLANCNVT_BLKMODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtBlkMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IGRVLANCNVT_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.igrCnvtEntry);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTIGRVLANCNVTENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanCnvtEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGRVLANCNVT_DBLTAG_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtDblTagEnable_get(buf->vlan_cfg.unit, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGRVLANCNVT_VIDSRC_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtVidSource_get(buf->vlan_cfg.unit, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGRVLANCNVT_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.egrCnvtEntry);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTEGRVLANCNVTENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_AGGRENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_aggrEnable_get(buf->vlan_cfg.unit, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_VLANAGGR_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_VLANAGGR_PRI_ENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrPriEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_LEAKYSTPFILTER_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_leakyStpFilter_get(buf->vlan_cfg.unit, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EXCEPT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_except_get(buf->vlan_cfg.unit, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTIGRCNVTDFLTACT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrCnvtDfltAct_get(buf->vlan_cfg.unit,
                    buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_VLANAGGRCTRL_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrCtrl_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.vlanAggrCtrl);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGRVLANCNVTVIDSOURCE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtVidSource_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGRVLANCNVTVIDTARGET_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtVidTarget_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTIGRCNVTLUMISACT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanCnvtLuMisAct_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTEGRCNVTLUMISACT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtLuMisAct_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, buf->vlan_cfg.type, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IGRVLANCNVTHITINDICATION_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtHitIndication_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, buf->vlan_cfg.flag, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGRVLANCNVTHITINDICATION_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtHitIndication_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, buf->vlan_cfg.flag, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IGRVLANCNVTRANGECHECKSET_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrVlanCnvtRangeCheckSet_get(buf->vlan_cfg.unit,buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_IGRVLANCNVTRANGECHECKENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_igrVlanCnvtRangeCheckEntry_get(buf->vlan_cfg.unit,buf->vlan_cfg.setIdx, buf->vlan_cfg.idx, &buf->vlan_cfg.rangeCheck);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

         case RTDRV_VLAN_EGRVLANCNVTRANGECHECKSET_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtRangeCheckSet_get(buf->vlan_cfg.unit,buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGRVLANCNVTRANGECHECKENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtRangeCheckEntry_get(buf->vlan_cfg.unit,buf->vlan_cfg.setIdx, buf->vlan_cfg.idx, &buf->vlan_cfg.rangeCheck);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
        case RTDRV_VLAN_PORT_VLANAGGRVIDSOURCE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrVidSource_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_VLANAGGRPRITAGVIDSOURCE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portVlanAggrPriTagVidSource_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_UCAST_LUTMODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2UcastLookupMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_MCAST_LUTMODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_l2McastLookupMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_INNER_ACCEPT_FRAME_TYPE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portInnerAcceptFrameType_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_VLAN_PORT_OUTER_ACCEPT_FRAME_TYPE_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portOuterAcceptFrameType_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_VLAN_PORT_INNER_PVID_MODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portInnerPvidMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            break;

        case RTDRV_VLAN_PORT_OUTER_PVID_MODE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portOuterPvidMode_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_INNER_PVID_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portInnerPvid_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_VLAN_PORT_OUTER_PVID_GET:
            rtk_copy_from(&buf->port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_vlan_portOuterPvid_get(buf->port_cfg.unit, buf->port_cfg.port, &buf->port_cfg.data);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_INNER_TPID_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_innerTpidEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_OUTER_TPID_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_outerTpidEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EXTRA_TPID_ENTRY_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extraTpidEntry_get(buf->vlan_cfg.unit, buf->vlan_cfg.idx, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGR_INNER_TPID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrInnerTpid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_IGR_OUTER_TPID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrOuterTpid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGR_INNER_TPID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrInnerTpid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGR_OUTER_TPID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrOuterTpid_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGR_INNER_TAG_STS_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrInnerTagSts_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EGR_OUTER_TAG_STS_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrOuterTagSts_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_EGRVLANCNVTLOOKUPMISSACT_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.data);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;
#endif

        case RTDRV_VLAN_EGRVLANCNVTRANGECHECKVID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_egrVlanCnvtRangeCheckVid_get(buf->vlan_cfg.unit,
                    buf->vlan_cfg.idx, &buf->vlan_cfg.rangeCheck);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_ECIDPMSK_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ecidPmsk_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.entry);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_ECIDPMSKNEXTVALID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_ecidPmskNextValid_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.entry);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TRKVLANAGGRENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_trkVlanAggrEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.tid, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TRKVLANAGGRPRIENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_trkVlanAggrPriEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.tid, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TRKVLANAGGRCTRL_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_trkVlanAggrCtrl_get(buf->vlan_cfg.unit, buf->vlan_cfg.tid, &buf->vlan_cfg.vlanAggrCtrl);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTPRIVATEVLANENABLE_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPrivateVlanEnable_get(buf->vlan_cfg.unit, buf->vlan_cfg.port, &buf->vlan_cfg.enable);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_INTFID_GET:
            rtk_copy_from(&buf->vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_intfId_get(buf->vlan_cfg.unit, buf->vlan_cfg.vid, &buf->vlan_cfg.intfId);
            rtk_copy_to(user, &buf->vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_bpe(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** BPE **/
        case RTDRV_BPE_PORTFWDMODE_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portFwdMode_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, &buf->bpe_cfg.mode);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PORTECIDNAMESPACEGROUPID_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portEcidNameSpaceGroupId_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, &buf->bpe_cfg.groupId);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PORTPCID_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portPcid_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.type, &buf->bpe_cfg.pcid);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PORTPCIDACT_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portPcidAct_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, &buf->bpe_cfg.action);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PORTEGRTAGSTS_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portEgrTagSts_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, &buf->bpe_cfg.status);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PORTEGRVLANTAGSTS_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portEgrVlanTagSts_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, &buf->bpe_cfg.vlan_status);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PVIDENTRY_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_pvidEntry_get(buf->bpe_cfg.unit, &buf->bpe_cfg.pvid_entry);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PVIDENTRYNEXTVALID_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_pvidEntryNextValid_get(buf->bpe_cfg.unit, &buf->bpe_cfg.scan_idx, &buf->bpe_cfg.pvid_entry);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_FWDENTRY_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_fwdEntry_get(buf->bpe_cfg.unit, &buf->bpe_cfg.fwd_entry);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_FWDENTRYNEXTVALID_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_fwdEntryNextValid_get(buf->bpe_cfg.unit, &buf->bpe_cfg.scan_idx, &buf->bpe_cfg.fwd_entry);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_GLOBALCTRL_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_globalCtrl_get(buf->bpe_cfg.unit, buf->bpe_cfg.type, &buf->bpe_cfg.arg);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PORTCTRL_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_portCtrl_get(buf->bpe_cfg.unit, buf->bpe_cfg.port, buf->bpe_cfg.type, &buf->bpe_cfg.arg);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        case RTDRV_BPE_PRIREMARKING_GET:
            rtk_copy_from(&buf->bpe_cfg, user, sizeof(rtdrv_bpeCfg_t));
            ret = rtk_bpe_priRemarking_get(buf->bpe_cfg.unit, buf->bpe_cfg.src, buf->bpe_cfg.val, &buf->bpe_cfg.pri);
            rtk_copy_to(user, &buf->bpe_cfg, sizeof(rtdrv_bpeCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_stp(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** STP **/
        case RTDRV_STP_MSTP_STATE_GET:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpState_get(buf->stp_cfg.unit, buf->stp_cfg.msti, buf->stp_cfg.port, &buf->stp_cfg.stp_state);
            rtk_copy_to(user, &buf->stp_cfg, sizeof(rtdrv_stpCfg_t));
            break;

        case RTDRV_STP_MSTP_INSTANCE_EXIST_GET:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_isMstpInstanceExist_get(buf->stp_cfg.unit, buf->stp_cfg.msti, &buf->stp_cfg.msti_isExist);
            rtk_copy_to(user, &buf->stp_cfg, sizeof(rtdrv_stpCfg_t));
            break;

        case RTDRV_STP_MSTP_MODE_GET:
            rtk_copy_from(&buf->stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpInstanceMode_get(buf->stp_cfg.unit, &buf->stp_cfg.msti_mode);
            rtk_copy_to(user, &buf->stp_cfg, sizeof(rtdrv_stpCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_reg(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** REG **/
        case RTDRV_REG_REGISTER_GET:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ioal_mem32_read(buf->reg_cfg.unit, buf->reg_cfg.reg, &buf->reg_cfg.value);
            ret = RT_ERR_OK; /*xxx_reg_register_get(buf->reg_cfg.unit, buf->reg_cfg.reg, &buf->reg_cfg.value);*/
            rtk_copy_to(user, &buf->reg_cfg, sizeof(rtdrv_regCfg_t));
            break;

        case RTDRV_REG_IDX2ADDR_GET:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = reg_idx2Addr_get(buf->reg_cfg.unit, (uint32)buf->reg_cfg.reg, &buf->reg_cfg.value);
            rtk_copy_to(user, &buf->reg_cfg, sizeof(rtdrv_regCfg_t));
            break;

        case RTDRV_REG_IDXMAX_GET:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = reg_idxMax_get(buf->reg_cfg.unit, &buf->reg_cfg.value);
            rtk_copy_to(user, &buf->reg_cfg, sizeof(rtdrv_regCfg_t));
            break;

        case RTDRV_REG_INFO_GET:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = reg_info_get(buf->reg_cfg.unit, (uint32)buf->reg_cfg.reg, &buf->reg_cfg.data);
            rtk_copy_to(user, &buf->reg_cfg, sizeof(rtdrv_regCfg_t));
            break;

        case RTDRV_TABLE_READ:
            rtk_copy_from(&buf->tbl_cfg, user, sizeof(rtdrv_tblCfg_t));
            ret = table_read(buf->tbl_cfg.unit, buf->tbl_cfg.table, buf->tbl_cfg.addr, buf->tbl_cfg.value);
            rtk_copy_to(user, &buf->reg_cfg, sizeof(rtdrv_tblCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_counter(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** COUNTER **/
        case RTDRV_COUNTER_GLOBAL_GET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_global_get(buf->counter_cfg.unit, buf->counter_cfg.cntr_idx, &(buf->counter_cfg.cntr));
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_GLOBAL_GETALL:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_global_getAll(buf->counter_cfg.unit, &buf->counter_cfg.global_cnt);
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_PORT_GET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_port_get(buf->counter_cfg.unit, buf->counter_cfg.port, buf->counter_cfg.cntr_idx, &(buf->counter_cfg.cntr));
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_PORT_GETALL:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_port_getAll(buf->counter_cfg.unit, buf->counter_cfg.port, &buf->counter_cfg.port_cnt);
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_TAGLENCNT_GET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_tagLenCntIncEnable_get(buf->counter_cfg.unit, buf->counter_cfg.tagCnt_type, &buf->counter_cfg.enable);
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_STACKHDRLENCNT_GET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_stackHdrLenCntIncEnable_get(buf->counter_cfg.unit, buf->counter_cfg.type, &buf->counter_cfg.enable);
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_FLEXCNTR_CFG_GET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_flexibleCntRange_get(buf->counter_cfg.unit, buf->counter_cfg.cntr_idx, &buf->counter_cfg.range);
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        case RTDRV_COUNTER_ENABLE_GET:
            rtk_copy_from(&buf->counter_cfg, user, sizeof(rtdrv_counterCfg_t));
            ret = rtk_stat_enable_get(buf->counter_cfg.unit, &buf->counter_cfg.enable);
            rtk_copy_to(user, &buf->counter_cfg, sizeof(rtdrv_counterCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_time(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** TIME **/
        case RTDRV_TIME_PORT_PTP_ENABLE_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpEnable_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.enable);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_PTP_RX_TIME_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpRxTimestamp_get(buf->time_cfg.unit, buf->time_cfg.port, buf->time_cfg.identifier, &buf->time_cfg.timeStamp);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_PTP_TX_TIME_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpTxTimestamp_get(buf->time_cfg.unit, buf->time_cfg.port, buf->time_cfg.identifier, &buf->time_cfg.timeStamp);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_REF_TIME_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTime_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.timeStamp);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_REF_TIME_ENABLE_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTimeEnable_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.enable);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_REF_TIME_FREQ_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portRefTimeFreq_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.freq);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_CORRECTION_FIELD_TRANSPARENT_VALUE_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_correctionFieldTransparentValue_get(buf->time_cfg.unit, buf->time_cfg.oriCf, buf->time_cfg.rxTimeStamp, &buf->time_cfg.transCf);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_MAC_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpMacAddr_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.mac);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_MAC_RANGE_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpMacAddrRange_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.range);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_VLAN_TPID_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpVlanTpid_get(buf->time_cfg.unit, buf->time_cfg.port, buf->time_cfg.type, buf->time_cfg.idx, &buf->time_cfg.tpid);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_OPER_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpOper_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.operCfg);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORT_LATCH_TIME_GET:
            rtk_copy_from(&buf->time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpLatchTime_get(buf->time_cfg.unit, buf->time_cfg.port, &buf->time_cfg.timeStamp);
            rtk_copy_to(user, &buf->time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_trap(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** TRAP **/
        case RTDRV_TRAP_RMAACTION_GET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaAction_get(buf->trap_cfg.unit, &buf->trap_cfg.rma_frame, &buf->trap_cfg.rma_action);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_BYPASS_STP_GET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_bypassStp_get(buf->trap_cfg.unit, buf->trap_cfg.bypassStp_frame, &buf->trap_cfg.enable);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_BYPASS_VLAN_GET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_bypassVlan_get(buf->trap_cfg.unit, buf->trap_cfg.bypassVlan_frame, &buf->trap_cfg.enable);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_USERDEFINERMA_GET:
            rtk_copy_from(&buf->l2_trap_cfg, user, sizeof(rtdrv_trapL2userRmaCfg_t));
            ret = rtk_trap_userDefineRma_get(buf->l2_trap_cfg.unit, buf->l2_trap_cfg.rma_index, &buf->l2_trap_cfg.rma_frame);
            rtk_copy_to(user, &buf->l2_trap_cfg, sizeof(rtdrv_trapL2userRmaCfg_t));
            break;

        case RTDRV_TRAP_USERDEFINERMAENABLE_GET:
            rtk_copy_from(&buf->l2_trap_cfg, user, sizeof(rtdrv_trapL2userRmaCfg_t));
            ret = rtk_trap_userDefineRmaEnable_get(buf->l2_trap_cfg.unit, buf->l2_trap_cfg.rma_index, &buf->l2_trap_cfg.enable);
            rtk_copy_to(user, &buf->l2_trap_cfg, sizeof(rtdrv_trapL2userRmaCfg_t));
            break;

        case RTDRV_TRAP_USERDEFINERMAACTION_GET:
            rtk_copy_from(&buf->l2_trap_cfg, user, sizeof(rtdrv_trapL2userRmaCfg_t));
            ret = rtk_trap_userDefineRmaAction_get(buf->l2_trap_cfg.unit, buf->l2_trap_cfg.rma_index, &buf->l2_trap_cfg.rma_action);
            rtk_copy_to(user, &buf->l2_trap_cfg, sizeof(rtdrv_trapL2userRmaCfg_t));
            break;

        case RTDRV_TRAP_MGMTFRAMEACTION_GET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFrameAction_get(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.frameType, (rtk_mgmt_action_t*)&buf->mgm_trap_cfg.rma_action);
            rtk_copy_to(user, &buf->mgm_trap_cfg, sizeof(rtdrv_trapMgmRmaCfg_t));
            break;

        case RTDRV_TRAP_MGMTFRAMEPRI_GET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFramePri_get(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.frameType, &buf->mgm_trap_cfg.priority);
            rtk_copy_to(user, &buf->mgm_trap_cfg, sizeof(rtdrv_trapMgmRmaCfg_t));
            break;

        case RTDRV_TRAP_MGMTFRAMEQID_GET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFrameQueue_get(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.qType, &buf->mgm_trap_cfg.qid);
            rtk_copy_to(user, &buf->mgm_trap_cfg, sizeof(rtdrv_trapMgmRmaCfg_t));
            break;

        case RTDRV_TRAP_PORTMGMTFRAMEACTION_GET:
            rtk_copy_from(&buf->mgm_trap_cfg, user, sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_portMgmtFrameAction_get(buf->mgm_trap_cfg.unit, buf->mgm_trap_cfg.port,
                        buf->mgm_trap_cfg.frameType, (rtk_mgmt_action_t*)&buf->mgm_trap_cfg.rma_action);
            rtk_copy_to(user, &buf->mgm_trap_cfg, sizeof(rtdrv_trapMgmRmaCfg_t));
            break;

        case RTDRV_TRAP_PKTWITHCFIACTION_GET:
            rtk_copy_from(&buf->other_trap_cfg, user, sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_pktWithCFIAction_get(buf->other_trap_cfg.unit,
                    &buf->other_trap_cfg.action);
            rtk_copy_to(user, &buf->other_trap_cfg, sizeof(rtdrv_trapOtherCfg_t));
            break;

        case RTDRV_TRAP_PKTWITHOUTERCFIACTION_GET:
            rtk_copy_from(&buf->other_trap_cfg, user, sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_pktWithOuterCFIAction_get(buf->other_trap_cfg.unit,
                    &buf->other_trap_cfg.action);
            rtk_copy_to(user, &buf->other_trap_cfg, sizeof(rtdrv_trapOtherCfg_t));
            break;

        case RTDRV_TRAP_PKTWITHCFIPRI_GET:
            rtk_copy_from(&buf->other_trap_cfg, user, sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_pktWithCFIPri_get(buf->other_trap_cfg.unit,
                    &buf->other_trap_cfg.priority);
            rtk_copy_to(user, &buf->other_trap_cfg, sizeof(rtdrv_trapOtherCfg_t));
            break;
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
        case RTDRV_TRAP_CFMUNKNOWNFRAMEACT_GET:
            rtk_copy_from(&buf->misc_cfg, user, sizeof(rtdrv_oamCfmMiscCfg_t));
            ret = rtk_trap_cfmUnknownFrameAct_get(buf->misc_cfg.unit,
                                                  &buf->misc_cfg.action);
            rtk_copy_to(user, &buf->misc_cfg, sizeof(rtdrv_oamCfmMiscCfg_t));
            break;

        case RTDRV_TRAP_CFMLOOPBACKACT_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmLoopbackLinkTraceAct_get(buf->cfm_trap_cfg.unit,
                                              buf->cfm_trap_cfg.md_level,
                                              &buf->cfm_trap_cfg.action);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;

        case RTDRV_TRAP_CFMCCMACT_GET:
            rtk_copy_from(&buf->oam_trap_cfg, user, sizeof(rtdrv_trapOamCfg_t));
            ret = rtk_trap_cfmCcmAct_get(buf->oam_trap_cfg.unit,
                                         buf->oam_trap_cfg.md_level,
                                         &buf->oam_trap_cfg.action);
            rtk_copy_to(user, &buf->oam_trap_cfg, sizeof(rtdrv_trapOamCfg_t));
            break;

        case RTDRV_TRAP_CFMETHDMACT_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmEthDmAct_get(buf->cfm_trap_cfg.unit,
                                           buf->cfm_trap_cfg.md_level,
                                           &buf->cfm_trap_cfg.action);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
        case RTDRV_TRAP_CFMFRAMETRAPPRI_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmFrameTrapPri_get(buf->cfm_trap_cfg.unit, &buf->cfm_trap_cfg.priority);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;

        case RTDRV_TRAP_OAMPDUACTION_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_oamPDUAction_get(buf->cfm_trap_cfg.unit, &buf->cfm_trap_cfg.action);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;

        case RTDRV_TRAP_OAMPDUPRI_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_oamPDUPri_get(buf->cfm_trap_cfg.unit, &buf->cfm_trap_cfg.priority);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;

        case RTDRV_TRAP_PORTOAMLOOPBACKPARACTION_GET:
            rtk_copy_from(&buf->oam_trap_cfg, user, sizeof(rtdrv_trapOamCfg_t));
            ret = rtk_trap_portOamLoopbackParAction_get(buf->oam_trap_cfg.unit,
                    buf->oam_trap_cfg.port, &buf->oam_trap_cfg.action);
            rtk_copy_to(user, &buf->oam_trap_cfg, sizeof(rtdrv_trapOamCfg_t));
            break;

        case RTDRV_TRAP_ROUTEEXCEPTIONACTION_GET:
            rtk_copy_from(&buf->routeException_trap_cfg, user,
                    sizeof(rtdrv_trapRouteExceptionCfg_t));
            ret = rtk_trap_routeExceptionAction_get(
                    buf->routeException_trap_cfg.unit,
                    buf->routeException_trap_cfg.type,
                    &buf->routeException_trap_cfg.action);
            rtk_copy_to(user, &buf->routeException_trap_cfg,
                    sizeof(rtdrv_trapRouteExceptionCfg_t));
            break;

        case RTDRV_TRAP_ROUTEEXCEPTIONPRI_GET:
            rtk_copy_from(&buf->routeException_trap_cfg, user,
                    sizeof(rtdrv_trapRouteExceptionCfg_t));
            ret = rtk_trap_routeExceptionPri_get(
                    buf->routeException_trap_cfg.unit,
                    buf->routeException_trap_cfg.type,
                    &buf->routeException_trap_cfg.priority);
            rtk_copy_to(user, &buf->routeException_trap_cfg,
                    sizeof(rtdrv_trapRouteExceptionCfg_t));
            break;

        case RTDRV_TRAP_USERDEFINERMALEARNINGENABLE_GET:
            rtk_copy_from(&buf->mgmuser_trap_cfg, user,
                    sizeof(rtdrv_trapUserMgmRmaCfg_t));
            ret = rtk_trap_userDefineRmaLearningEnable_get(
                    buf->mgmuser_trap_cfg.unit, buf->mgmuser_trap_cfg.mgmt_idx,
                    &buf->mgmuser_trap_cfg.enable);
            rtk_copy_to(user, &buf->mgmuser_trap_cfg,
                    sizeof(rtdrv_trapUserMgmRmaCfg_t));
            break;

        case RTDRV_TRAP_RMALEARNINGENABLE_GET:
            rtk_copy_from(&buf->trap_cfg, user,
                    sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaLearningEnable_get(
                    buf->trap_cfg.unit, &buf->trap_cfg.rma_frame,
                    &buf->trap_cfg.enable);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_MGMTFRAMELEARNINGENABLE_GET:
            rtk_copy_from(&buf->mgm_trap_cfg, user,
                    sizeof(rtdrv_trapMgmRmaCfg_t));
            ret = rtk_trap_mgmtFrameLearningEnable_get(buf->mgm_trap_cfg.unit,
                    buf->mgm_trap_cfg.frameType, &buf->mgm_trap_cfg.enable);
            rtk_copy_to(user, &buf->mgm_trap_cfg,
                    sizeof(rtdrv_trapMgmRmaCfg_t));
            break;

        case RTDRV_TRAP_MGMTFRAMEMGMTVLANENABLE_GET:
            rtk_copy_from(&buf->other_trap_cfg, user,
                    sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_mgmtFrameMgmtVlanEnable_get(buf->other_trap_cfg.unit,
                    &buf->other_trap_cfg.enable);
            rtk_copy_to(user, &buf->other_trap_cfg,
                    sizeof(rtdrv_trapOtherCfg_t));
            break;

        case RTDRV_TRAP_BPDUFLOODPORTMASK_GET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_bpduFloodPortmask_get(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            rtk_copy_to(user, &buf->flood_pmsk_cfg,sizeof(rtdrv_floodPmskCfg_t));
            break;

        case RTDRV_TRAP_EAPOLFLOODPORTMASK_GET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_eapolFloodPortmask_get(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            rtk_copy_to(user, &buf->flood_pmsk_cfg,sizeof(rtdrv_floodPmskCfg_t));
            break;

        case RTDRV_TRAP_LLDPFLOODPORTMASK_GET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_lldpFloodPortmask_get(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            rtk_copy_to(user, &buf->flood_pmsk_cfg,sizeof(rtdrv_floodPmskCfg_t));
            break;

        case RTDRV_TRAP_USERDEFINEFLOODPORTMASK_GET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_userDefineFloodPortmask_get(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            rtk_copy_to(user, &buf->flood_pmsk_cfg,sizeof(rtdrv_floodPmskCfg_t));
            break;

        case RTDRV_TRAP_RMAFLOODPORTMASK_GET:
            rtk_copy_from(&buf->flood_pmsk_cfg, user,sizeof(rtdrv_floodPmskCfg_t));
            ret = rtk_trap_rmaFloodPortmask_get(buf->flood_pmsk_cfg.unit, &buf->flood_pmsk_cfg.pmsk);
            rtk_copy_to(user, &buf->flood_pmsk_cfg,sizeof(rtdrv_floodPmskCfg_t));
            break;

        case RTDRV_TRAP_RMACANCELMIRROR_GET:
            rtk_copy_from(&buf->trap_cfg, user,sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaCancelMirror_get(buf->trap_cfg.unit, &buf->trap_cfg.enable);
            rtk_copy_to(user, &buf->trap_cfg,sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_RMAGROUPACTION_GET:
            rtk_copy_from(&buf->rma_grp_act_cfg, user,
                    sizeof(rtdrv_rmaGroupType_t));
            ret = rtk_trap_rmaGroupAction_get(
                    buf->rma_grp_act_cfg.unit, buf->rma_grp_act_cfg.rmaGroup_frameType, &buf->rma_grp_act_cfg.rma_action);
            rtk_copy_to(user, &buf->rma_grp_act_cfg,
                    sizeof(rtdrv_rmaGroupType_t));
            break;

        case RTDRV_TRAP_RMAGROUPLEARNINGENABLE_GET:
            rtk_copy_from(&buf->rma_grp_lrn_cfg, user,
                    sizeof(rtdrv_rmaGroupLearn_t));
            ret = rtk_trap_rmaGroupLearningEnable_get(
                    buf->rma_grp_lrn_cfg.unit, buf->rma_grp_lrn_cfg.rmaGroup_frameType, &buf->rma_grp_lrn_cfg.enable);
            rtk_copy_to(user, &buf->rma_grp_lrn_cfg,
                    sizeof(rtk_enable_t));
            break;

        case RTDRV_TRAP_MGMTFRAMESELFARPENABLE_GET:
            rtk_copy_from(&buf->other_trap_cfg, user,
                    sizeof(rtdrv_trapOtherCfg_t));
            ret = rtk_trap_mgmtFrameSelfARPEnable_get(buf->other_trap_cfg.unit,
                    &buf->other_trap_cfg.enable);
            rtk_copy_to(user, &buf->other_trap_cfg,
                    sizeof(rtdrv_trapOtherCfg_t));
            break;

        case RTDRV_TRAP_RMALOOKUPMISSACTIONENABLE_GET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaLookupMissActionEnable_get(buf->trap_cfg.unit, &buf->trap_cfg.enable);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_CFMACT_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmAct_get(buf->cfm_trap_cfg.unit, buf->cfm_trap_cfg.type,
                    buf->cfm_trap_cfg.md_level, &buf->cfm_trap_cfg.action);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;

        case RTDRV_TRAP_CFMTARGET_GET:
            rtk_copy_from(&buf->cfm_trap_cfg, user, sizeof(rtdrv_trapCfmCfg_t));
            ret = rtk_trap_cfmTarget_get(buf->cfm_trap_cfg.unit, &buf->cfm_trap_cfg.target);
            rtk_copy_to(user, &buf->cfm_trap_cfg, sizeof(rtdrv_trapCfmCfg_t));
            break;

        case RTDRV_TRAP_OAMTARGET_GET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_oamTarget_get(buf->trap_cfg.unit, &buf->trap_cfg.target);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_MGMTFRAMETARGET_GET:
            rtk_copy_from(&buf->trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_mgmtFrameTarget_get(buf->trap_cfg.unit, &buf->trap_cfg.target);
            rtk_copy_to(user, &buf->trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_acl(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** ACL **/
        case RTDRV_ACL_ENTRY_FIELD_SIZE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryFieldSize_get(buf->acl_cfg.unit, buf->acl_cfg.field_type, &buf->acl_cfg.size);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_ENTRY_SIZE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntrySize_get(buf->acl_cfg.unit, buf->acl_cfg.phase, &buf->acl_cfg.size);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_ENTRY_DATA_READ:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntry_read(buf->acl_cfg.unit, buf->acl_cfg.phase, buf->acl_cfg.index, buf->acl_cfg.entry_buffer);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEVALIDATE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleValidate_get(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, &buf->acl_cfg.status);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEENTRYFIELD_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryField_get(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.entry_buffer,
                    buf->acl_cfg.field_type, buf->acl_cfg.field_data,
                    buf->acl_cfg.field_mask);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEENTRYFIELD_READ:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryField_read(buf->acl_cfg.unit,
                    buf->acl_cfg.phase, buf->acl_cfg.index,
                    buf->acl_cfg.field_type, buf->acl_cfg.field_data,
                    buf->acl_cfg.field_mask);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEENTRYFIELD_CHECK:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleEntryField_check(buf->acl_cfg.unit,
                    buf->acl_cfg.phase, buf->acl_cfg.field_type);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEOPERATION_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleOperation_get(buf->acl_cfg.unit,
                    buf->acl_cfg.phase, buf->acl_cfg.index,
                    &buf->acl_cfg.oper);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEACTION_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleAction_get(buf->acl_cfg.unit,
                    buf->acl_cfg.phase, buf->acl_cfg.index,
                    &buf->acl_cfg.action);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;
#if ((defined CONFIG_SDK_RTL8390) || (defined CONFIG_SDK_RTL8380))
        case RTDRV_ACL_BLOCKPWRENABLE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_blockPwrEnable_get(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, &buf->acl_cfg.status);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_BLOCKAGGREGATORENABLE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_blockGroupEnable_get(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, buf->acl_cfg.blk_group,
                    &buf->acl_cfg.status);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_STATPKTCNT_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_statPktCnt_get(buf->acl_cfg.unit, buf->acl_cfg.index,
                    &buf->acl_cfg.size);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_STATBYTECNT_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_statByteCnt_get(buf->acl_cfg.unit, buf->acl_cfg.index,
                    &buf->acl_cfg.count);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_METER_MODE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_meterMode_get(buf->acl_cfg.unit, buf->acl_cfg.blockIdx, &buf->acl_cfg.meterMode);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_METER_BURST_SIZE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_meterBurstSize_get(buf->acl_cfg.unit, buf->acl_cfg.meterMode, &buf->acl_cfg.burstSize);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RANGECHECKL4PORT_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckL4Port_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_l4Port);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;

        case RTDRV_ACL_RANGECHECKVID_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckVid_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_vid);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;

        case RTDRV_ACL_RANGECHECKSRCPORT_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckSrcPort_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_port);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;

        case RTDRV_ACL_RANGECHECKPACKETLEN_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckPacketLen_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_pktLen);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;
#endif  /* defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) */
#if (defined CONFIG_SDK_RTL9300)
        case RTDRV_ACL_LOOPBACKENABLE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_loopBackEnable_get(buf->acl_cfg.unit, &buf->acl_cfg.enable);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;
#endif
#if ((defined CONFIG_SDK_RTL9300) || (defined CONFIG_SDK_RTL9310))
        case RTDRV_ACL_LIMITLOOPBACKTIMES_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_limitLoopbackTimes_get(buf->acl_cfg.unit, &buf->acl_cfg.lb_times);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;
#endif
#if (defined CONFIG_SDK_RTL8380)
        case RTDRV_ACL_PORTLOOKUPENABLE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_portLookupEnable_get(buf->acl_cfg.unit,
                    buf->acl_cfg.port, &buf->acl_cfg.status);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_LOOKUPMISSACT_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_lookupMissAct_get(buf->acl_cfg.unit,
                    buf->acl_cfg.port, &buf->acl_cfg.lmAct);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RANGECHECKFIELDSEL_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckFieldSelector_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_fieldSel);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;
#endif  /* (defined CONFIG_SDK_RTL8380) */
#if (defined CONFIG_SDK_RTL8390)
        case RTDRV_ACL_PARTITION_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_partition_get(buf->acl_cfg.unit, &buf->acl_cfg.blockIdx);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_TEMPLATEFIELDINTENTVLANTAG_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_templateFieldIntentVlanTag_get(buf->acl_cfg.unit,
                    &buf->acl_cfg.tagType);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RANGECHECKDSTPORT_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_acl_rangeCheckDstPort_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_port);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;

        case RTDRV_ACL_BLOCKRESULTMODE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_blockResultMode_get(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, &buf->acl_cfg.blk_mode);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;
#endif  /* defined(CONFIG_SDK_RTL8390) */
        case RTDRV_ACL_PORTPHASELOOKUPENABLE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_portPhaseLookupEnable_get(buf->acl_cfg.unit,
                    buf->acl_cfg.port, buf->acl_cfg.phase, &buf->acl_cfg.status);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_TEMPLATESELECTOR_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_templateSelector_get(buf->acl_cfg.unit,
                    buf->acl_cfg.blockIdx, &buf->acl_cfg.template_idx);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_STATCNT_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_statCnt_get(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, buf->acl_cfg.mode, &buf->acl_cfg.cnt);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_RULEHITINDICATION_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ruleHitIndication_get(buf->acl_cfg.unit,
                    buf->acl_cfg.phase, buf->acl_cfg.index,
                    buf->acl_cfg.reset, &buf->acl_cfg.isHit);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_FIELDUSR2TMPLATE_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_fieldUsr2Template_get(buf->acl_cfg.unit, buf->acl_cfg.phase, buf->acl_cfg.field_type, &buf->acl_cfg.usr2tmplte);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_TEMPLATEID_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_templateId_get(buf->acl_cfg.unit, buf->acl_cfg.phase,
                    buf->acl_cfg.index, &buf->acl_cfg.entry_template_id);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_pie(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** PIE **/
        case RTDRV_PIE_METER_INCLUDE_IFG_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterIncludeIfg_get(buf->pie_cfg.unit, &buf->pie_cfg.ifg_include);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_METER_ENTRY_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterEntry_get(buf->pie_cfg.unit, buf->pie_cfg.meterIdx, &buf->pie_cfg.meterEntry);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_METER_EXCEED_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterExceed_get(buf->pie_cfg.unit, buf->pie_cfg.meterIdx, &buf->pie_cfg.isExceed);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_METER_EXCEED_AGGREGATION_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterExceedAggregation_get(buf->pie_cfg.unit, &buf->pie_cfg.exceedMask);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_BLOCKLOOKUPENABLE_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_blockLookupEnable_get(buf->pie_cfg.unit,
                    buf->pie_cfg.blockIdx, &buf->pie_cfg.status);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_BLOCKGROUPING_GET:
            rtk_copy_from(&buf->acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_pie_blockGrouping_get(buf->pie_cfg.unit,
                    buf->pie_cfg.blockIdx, &buf->pie_cfg.grpId,
                    &buf->pie_cfg.logicId);
            rtk_copy_to(user, &buf->acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_PIE_TEMPLATE_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_template_get(buf->pie_cfg.unit,
                    buf->pie_cfg.index, &buf->pie_cfg.template);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_TEMPLATEFIELD_CHECK:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_templateField_check(buf->pie_cfg.unit,
                    buf->pie_cfg.phase, buf->pie_cfg.field_type);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_RANGECHECKIP_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_pie_rangeCheckIp_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range_ip);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;

        case RTDRV_PIE_RANGECHECK_GET:
            rtk_copy_from(&buf->rangeCheck_cfg, user, sizeof(rtdrv_rangeCheckCfg_t));
            ret = rtk_pie_rangeCheck_get(buf->rangeCheck_cfg.unit,
                    buf->rangeCheck_cfg.index, &buf->rangeCheck_cfg.range);
            rtk_copy_to(user, &buf->rangeCheck_cfg, sizeof(rtdrv_rangeCheckCfg_t));
            break;

        case RTDRV_PIE_FIELDSELECTOR_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_fieldSelector_get(buf->pie_cfg.unit, buf->pie_cfg.index,
                    &buf->pie_cfg.fs);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_PHASE_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_phase_get(buf->pie_cfg.unit, buf->pie_cfg.blockIdx, (rtk_pie_phase_t *)&buf->pie_cfg.phase);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_TEMPLATE_VLANSEL_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_templateVlanSel_get(buf->pie_cfg.unit, buf->pie_cfg.phase, buf->pie_cfg.templateIdx, &buf->pie_cfg.vlanSel);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_METER_DPSEL_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterDpSel_get(buf->pie_cfg.unit, &buf->pie_cfg.dpSel);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_ARPMACSEL_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_arpMacSel_get(buf->pie_cfg.unit, &buf->pie_cfg.arpMacSel);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_INTFSEL_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_intfSel_get(buf->pie_cfg.unit, &buf->pie_cfg.intfSel);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_TEMPLATEVLANFMTSEL_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_templateVlanFmtSel_get(buf->pie_cfg.unit, buf->pie_cfg.phase, buf->pie_cfg.templateIdx, &buf->pie_cfg.vlanFmtSel);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_METERTRTCMTYPE_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_meterTrtcmType_get(buf->pie_cfg.unit, &buf->pie_cfg.type);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        case RTDRV_PIE_FILTER1BR_GET:
            rtk_copy_from(&buf->pie_cfg, user, sizeof(rtdrv_pieCfg_t));
            ret = rtk_pie_filter1BR_get(buf->pie_cfg.unit, &buf->pie_cfg.status);
            rtk_copy_to(user, &buf->pie_cfg, sizeof(rtdrv_pieCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_qos(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** QOS **/
        case RTDRV_QOS_QUEUE_NUM_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_queueNum_get(buf->qos_cfg.unit, &buf->qos_cfg.queue_num);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI_MAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priMap_get(buf->qos_cfg.unit, buf->qos_cfg.queue_num, &buf->qos_cfg.pri2qid);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI2QID_MAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_pri2QidMap_get(buf->qos_cfg.unit, buf->qos_cfg.int_pri, &buf->qos_cfg.queue);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_CPUQID2QID_MAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_cpuQid2QidMap_get(buf->qos_cfg.unit, buf->qos_cfg.cpuQid, &buf->qos_cfg.queue);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_CPUQID2SQID_MAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_cpuQid2StackQidMap_get(buf->qos_cfg.unit, buf->qos_cfg.cpuQid, &buf->qos_cfg.queue);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_1P_PRI_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pPriRemap_get(buf->qos_cfg.unit, buf->qos_cfg.dot1p_pri, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_OUTER_1P_PRI_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pPriRemap_get(buf->qos_cfg.unit, buf->qos_cfg.dot1p_pri, buf->qos_cfg.dei, &(buf->qos_cfg.int_pri));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif

        case RTDRV_QOS_DEI_DP_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiDpRemap_get(buf->qos_cfg.unit, buf->qos_cfg.dei, &buf->qos_cfg.dp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_DEI_SRC_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDEISrcSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.deiSrc);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_DP_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDpSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &(buf->qos_cfg.weightOfDpSel));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_DSCP_DP_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpDpRemap_get(buf->qos_cfg.unit, buf->qos_cfg.dscp, &buf->qos_cfg.dp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif
        case RTDRV_QOS_DP_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dpRemap_get(buf->qos_cfg.unit, buf->qos_cfg.dpSrcType, buf->qos_cfg.dpSrcRemap, &buf->qos_cfg.dp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priRemap_get(buf->qos_cfg.unit, buf->qos_cfg.priSrcType, buf->qos_cfg.priSrcRemap, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_DSCP_PRI_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpPriRemap_get(buf->qos_cfg.unit, buf->qos_cfg.dscp, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_INNER_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portInnerPri_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OUTER_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuterPri_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif

        case RTDRV_QOS_DP_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dpSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.dpSrcType);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI_SEL_GROUP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priSelGroup_get(buf->qos_cfg.unit, buf->qos_cfg.index, &(buf->qos_cfg.priSelWeight));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_PRI_SEL_GROUP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriSelGroup_get(buf->qos_cfg.unit, buf->qos_cfg.port, &(buf->qos_cfg.index));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_1P_REMARK_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pRemarkEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.remark_enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1P_REMARKING_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarking_get(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_1p, buf->qos_cfg.rmkval_1p, &buf->qos_cfg.dot1p_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1P_REMARK_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarkSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.rmksrc_1p);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPri_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.dot1p_dflt_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_EXT_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPriExt_get(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, &buf->qos_cfg.dot1p_dflt_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPriSrcSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.dflt_src_1p);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_1P_DFLT_PRI_SRC_SEL_EXT_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_port1pDfltPriSrcSelExt_get(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, &buf->qos_cfg.dflt_src_1p);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1P_DFLT_PRI_CFG_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pDfltPriCfgSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.dot1p_dflt_cfg_dir);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OUT_1P_REMARK_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOut1pRemarkEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.remark_enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_OUTER_1P_REMARKING_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pRemarking_get(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_outer1p, buf->qos_cfg.rmkval_outer1p, &buf->qos_cfg.dot1p_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_OUT_1P_REMARK_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pRemarkSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.rmksrc_outer1p);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPri_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.out1p_dflt_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OUTER_1P_DFLT_PRI_EXT_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPriExt_get(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, &buf->qos_cfg.out1p_dflt_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_OUTER_1P_DFLT_PRI_CFG_SRC_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pDfltPriCfgSrcSel_get(buf->qos_cfg.unit,&buf->qos_cfg.out1p_dflt_cfg_dir);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_DSCP_REMARK_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDscpRemarkEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.remark_enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCP_REMARKING_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarking_get(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_dscp, buf->qos_cfg.rmkval_dscp, &buf->qos_cfg.dscp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCP_REMARK_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarkSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.rmksrc_dscp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_DEI_REMARK_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDeiRemarkEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.remark_enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DEI_REMARKING_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiRemarking_get(buf->qos_cfg.unit, buf->qos_cfg.rmksrc_dei, buf->qos_cfg.rmkval_dei, &buf->qos_cfg.dei);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_DEI_REMARK_TAG_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDeiRemarkTagSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &(buf->qos_cfg.deiSrc));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DEI_REMARK_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiRemarkSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.rmksrc_dei);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1P_DFLT_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pDfltPri_get(buf->qos_cfg.unit, &buf->qos_cfg.dot1p_dflt_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_1P_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemark_get(buf->qos_cfg.unit, buf->qos_cfg.int_pri, &buf->qos_cfg.dot1p_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_OUT_1P_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pRemark_get(buf->qos_cfg.unit, buf->qos_cfg.int_pri, &buf->qos_cfg.dot1p_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif

        case RTDRV_QOS_PORT_OUTER_1P_DFLT_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPriSrcSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &(buf->qos_cfg.out1p_dflt_src));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OUTER_1P_DFLT_SRC_SEL_EXT_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pDfltPriSrcSelExt_get(buf->qos_cfg.unit, buf->qos_cfg.devID, buf->qos_cfg.port, &(buf->qos_cfg.out1p_dflt_src));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1P_DFLT_PRI_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pDfltPriSrcSel_get(buf->qos_cfg.unit, &buf->qos_cfg.dflt_src_1p);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_OUTER_1P_DFLT_PRI_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_outer1pDfltPri_get(buf->qos_cfg.unit, &buf->qos_cfg.out1p_dflt_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_DSCP_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemark_get(buf->qos_cfg.unit, buf->qos_cfg.int_pri, &buf->qos_cfg.dscp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCP2DSCP_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2DscpRemark_get(buf->qos_cfg.unit, buf->qos_cfg.org_dscp, &buf->qos_cfg.dscp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCP2DOT1P_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2Dot1pRemark_get(buf->qos_cfg.unit, buf->qos_cfg.org_dscp, &buf->qos_cfg.dot1p_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCP2OUT1P_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2Outer1pRemark_get(buf->qos_cfg.unit, buf->qos_cfg.org_dscp, &buf->qos_cfg.dot1p_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DEI_REMARK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_deiRemark_get(buf->qos_cfg.unit, buf->qos_cfg.dp, &(buf->qos_cfg.dei));
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif

        case RTDRV_QOS_SCHEDULING_ALGORITHM_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingAlgorithm_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.scheduling_type);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_SCHEDULING_QUEUE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingQueue_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.qweights);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_CONG_AVOID_ALGO_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidAlgo_get(buf->qos_cfg.unit, &buf->qos_cfg.congAvoid_algo);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_CONG_AVOID_ALGO_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portCongAvoidAlgo_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.congAvoid_algo);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_CONG_AVOID_SYS_THRESH_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidSysThresh_get(buf->qos_cfg.unit, buf->qos_cfg.dp, &buf->qos_cfg.congAvoid_thresh);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_CONG_AVOID_SYS_DROP_PROBABILITY_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidSysDropProbability_get(buf->qos_cfg.unit, buf->qos_cfg.dp, &buf->qos_cfg.data);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_THRESH_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidGlobalQueueThresh_get(buf->qos_cfg.unit, buf->qos_cfg.queue, buf->qos_cfg.dp, &buf->qos_cfg.congAvoid_thresh);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_DROP_PROBABILITY_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidGlobalQueueDropProbability_get(buf->qos_cfg.unit, buf->qos_cfg.queue, buf->qos_cfg.dp, &buf->qos_cfg.data);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif

        case RTDRV_QOS_CONG_AVOID_GLOBAL_QUEUE_CONFIG_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_congAvoidGlobalQueueConfig_get(buf->qos_cfg.unit, buf->qos_cfg.queue, buf->qos_cfg.dp, &buf->qos_cfg.congAvoid_thresh);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_AVB_SR_CLASS_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portAvbStreamReservationClassEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.srClass, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_AVB_SR_CONFIG_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_avbStreamReservationConfig_get(buf->qos_cfg.unit, &buf->qos_cfg.srConf);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PKT2CPU_PRI_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_pkt2CpuPriRemap_get(buf->qos_cfg.unit, buf->qos_cfg.int_pri, &buf->qos_cfg.new_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_RSPAN_PRI_REMAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_rspanPriRemap_get(buf->qos_cfg.unit, buf->qos_cfg.rspan_pri, &buf->qos_cfg.int_pri);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI2IGR_QUEUE_MAP_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri2IgrQMap_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.pri2qid);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI2IGR_QUEUE_MAP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri2IgrQMapEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_IGR_QUEUE_WEIGHT_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portIgrQueueWeight_get(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.queue, &buf->qos_cfg.data);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OUTER_1P_REMARK_SRC_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuter1pRemarkSrcSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.rmksrc_outer1p);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_INVLD_DSCP_VAL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_invldDscpVal_get(buf->qos_cfg.unit, &buf->qos_cfg.dscp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_INVLD_DSCP_MASK_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_invldDscpMask_get(buf->qos_cfg.unit, &buf->qos_cfg.dscp);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_INVLD_DSCP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portInvldDscpEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_INVLD_DSCP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_invldDscpEnable_get(buf->qos_cfg.unit, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_QOS_SYS_PORT_PRI_REMAP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriRemapEnable_get(buf->qos_cfg.unit, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
#endif

        case RTDRV_QOS_SYS_PORT_PRI_REMAP_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_sysPortPriRemapSel_get(buf->qos_cfg.unit, &buf->qos_cfg.portPriRemap_type);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_PORT_PRI_REMAP_SEL_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPortPriRemapSel_get(buf->qos_cfg.unit, buf->qos_cfg.port, &buf->qos_cfg.portPriRemap_type);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_IPRI_REMAP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portInnerPriRemapEnable_get(buf->qos_cfg.unit, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_OPRI_REMAP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portOuterPriRemapEnable_get(buf->qos_cfg.unit, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRI_REMAP_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priRemapEnable_get(buf->qos_cfg.unit, buf->qos_cfg.priSrcType, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORT_QUEUE_STRICT_ENABLE_GET:
            rtk_copy_from(&buf->qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portQueueStrictEnable_get(buf->qos_cfg.unit, buf->qos_cfg.port, buf->qos_cfg.queue, &buf->qos_cfg.enable);
            rtk_copy_to(user, &buf->qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_trunk(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** TRUNK **/
        case RTDRV_TRUNK_MODE_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_mode_get(buf->trunk_cfg.unit, &buf->trunk_cfg.mode);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_PORT_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_port_get(buf->trunk_cfg.unit,buf->trunk_cfg.trk_gid, &buf->trunk_cfg.trk_member);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_LOCAL_PORT_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_localPort_get(buf->trunk_cfg.unit,buf->trunk_cfg.trk_gid, &buf->trunk_cfg.trk_member);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_EGR_PORT_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_egrPort_get(buf->trunk_cfg.unit,buf->trunk_cfg.trk_gid, &buf->trunk_cfg.trk_egr_ports);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_BIND_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmBind_get(buf->trunk_cfg.unit,buf->trunk_cfg.trk_gid, &buf->trunk_cfg.algo_id);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_BIND_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmTypeBind_get(buf->trunk_cfg.unit,buf->trunk_cfg.trk_gid,buf->trunk_cfg.bindType, &buf->trunk_cfg.algo_id);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_PARAM_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmParam_get(buf->trunk_cfg.unit,buf->trunk_cfg.algo_id, &buf->trunk_cfg.algo_bitmask);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_PARAM_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmTypeParam_get(buf->trunk_cfg.unit,buf->trunk_cfg.paramType, buf->trunk_cfg.algo_id, &buf->trunk_cfg.algo_bitmask);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmShift_get(buf->trunk_cfg.unit,buf->trunk_cfg.algo_id, &buf->trunk_cfg.shift);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_GBL_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithmShiftGbl_get(buf->trunk_cfg.unit, &buf->trunk_cfg.shift);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_TRAFFIC_SEPARATE_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparate_get(buf->trunk_cfg.unit,buf->trunk_cfg.trk_gid, &buf->trunk_cfg.separate);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_TRAFFIC_SEPARATE_ENABLE_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparateEnable_get(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid, buf->trunk_cfg.separate, &buf->trunk_cfg.enable);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_TRAFFIC_SEPARATE_DIVISION_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparateDivision_get(buf->trunk_cfg.unit, &buf->trunk_cfg.enable);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_TUNNEL_HASH_SRC_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_tunnelHashSrc_get(buf->trunk_cfg.unit, &buf->trunk_cfg.tunnelHashSrc);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_STACK_TRUNK_PORT_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_stkTrkPort_get(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid,  &buf->trunk_cfg.trk_member);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;


        case RTDRV_TRUNK_STACK_TRUNK_HASH_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_stkTrkHash_get(buf->trunk_cfg.unit, &buf->trunk_cfg.stkTrkHash);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_STACK_DIST_ALGO_TYPE_BIND_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_stkDistributionAlgorithmTypeBind_get(buf->trunk_cfg.unit, buf->trunk_cfg.trk_gid,
                buf->trunk_cfg.bindType, &buf->trunk_cfg.algo_id);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_LOCALFIRST_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_localFirst_get(buf->trunk_cfg.unit, &buf->trunk_cfg.localFirst);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_LOCALFIRSTFAILOVER_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_localFirstFailOver_get(buf->trunk_cfg.unit, &buf->trunk_cfg.congstAvoid, &buf->trunk_cfg.linkFailAvoid);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;


        case RTDRV_TRUNK_SRCPORTMAP_GET:
            rtk_copy_from(&buf->trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_srcPortMap_get(buf->trunk_cfg.unit, buf->trunk_cfg.devPort, &buf->trunk_cfg.isTrkMbr, &buf->trunk_cfg.trk_gid);
            rtk_copy_to(user, &buf->trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_debug(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** DEBUG **/
        case RTDRV_DEBUG_EN_LOG_GET:
            ret = rt_log_enable_get(&buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGLV_GET:
            ret = rt_log_level_get(&buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGLVMASK_GET:
            ret = rt_log_mask_get(&buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGTYPE_GET:
            ret = rt_log_type_get(&buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGFORMAT_GET:
            ret = rt_log_format_get(&buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_MODMASK_GET:
            ret = rt_log_moduleMask_get(&buf->unit_cfg.data64);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGCFG_GET:
            ret = rt_log_config_get((uint32 *)&buf->log_cfg);
            rtk_copy_to(user, &buf->log_cfg, sizeof(rtdrv_logCfg_t));
            break;

        case RTDRV_DEBUG_MEM_READ:
            rtk_copy_from(&buf->reg_cfg, user, sizeof(rtdrv_regCfg_t));
            ret = debug_mem_read(buf->reg_cfg.unit, buf->reg_cfg.reg, &buf->reg_cfg.value);
            rtk_copy_to(user, &buf->reg_cfg, sizeof(rtdrv_regCfg_t));
            break;

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)||defined(CONFIG_SDK_RTL9310)||defined(CONFIG_SDK_RTL9300)
        case RTDRV_DEBUG_MIB_DBG_CNTR_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getDbgCntr(buf->unit_cfg.unit, buf->unit_cfg.mibType, &buf->unit_cfg.cntr);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        case RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_INGRESS_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlIgrPortUsedPageCnt(buf->unit_cfg.unit, buf->unit_cfg.port, &buf->unit_cfg.cntr, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_EGRESS_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlEgrPortUsedPageCnt(buf->unit_cfg.unit, buf->unit_cfg.port, &buf->unit_cfg.cntr, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_FLOWCTRL_SYSTEM_USED_PAGE_CNT_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlSystemUsedPageCnt(buf->unit_cfg.unit, &buf->unit_cfg.cntr, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_FLOWCTRL_PORT_QUEUE_USED_PAGE_CNT_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlPortQueueUsedPageCnt(buf->unit_cfg.unit, buf->unit_cfg.port,
                &buf->unit_cfg.qCntr, &buf->unit_cfg.qMaxCntr);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        case RTDRV_DEBUG_WATCHDOG_CNT_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getWatchdogCnt(buf->unit_cfg.unit, buf->unit_cfg.cntr, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        case RTDRV_DEBUG_FLOWCTRL_SYSTEM_PUB_USED_PAGE_CNT_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlSystemPublicUsedPageCnt(buf->unit_cfg.unit, &buf->unit_cfg.cntr, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
        case RTDRV_DEBUG_FLOWCTRL_SYSTEM_PUB_RSRC_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlSystemPublicResource_get(buf->unit_cfg.unit, &buf->unit_cfg.cntr);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif
#if defined(CONFIG_SDK_RTL9310)

        case RTDRV_DEBUG_FLOWCTRL_SYSTEM_IGR_QUEUE_USED_PAGE_CNT_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlSystemIgrQueueUsedPageCnt(buf->unit_cfg.unit,&buf->unit_cfg.qCntr, &buf->unit_cfg.qMaxCntr);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_FLOWCTRL_PORT_QUEUE_USED_PAGE_CNT_INGRESS_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlPortIgrQueueUsedPageCnt(buf->unit_cfg.unit, buf->unit_cfg.port,
                &buf->unit_cfg.qCntr, &buf->unit_cfg.qMaxCntr);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_FLOWCTRL_RPECT_QUEUE_CNTR_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_getFlowCtrlRepctQueueCntrInfo(buf->unit_cfg.unit, &buf->unit_cfg.repctCntr);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_REPCTQ_EMPTY_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_repctQueueEmptyStatus_get(buf->unit_cfg.unit, &buf->unit_cfg.enable);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
        case RTDRV_DEBUG_REPCTQ_STICK_ENABLE_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_repctQueueStickEnable_get(buf->unit_cfg.unit, &buf->unit_cfg.enable);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
        case RTDRV_DEBUG_REPCTQ_FETCH_ENABLE_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = hal_repctQueueFetchEnable_get(buf->unit_cfg.unit, &buf->unit_cfg.enable);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif

        case RTDRV_DEBUG_SDS_RXCALI_ENABLE_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = mac_debug_sds_rxCaliEnable_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.en);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        case RTDRV_DEBUG_SDS_RXCALI_STATUS_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = mac_debug_sds_rxCaliStatus_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.rxCaliStatus);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        case RTDRV_DEBUG_PHY_CMD_GET:
            rtk_copy_from(&buf->phy_debug_cfg, user, sizeof(rtdrv_phyDebugCfg_t));
            ret = _dal_phy_debug_cmd(buf->phy_debug_cfg.unit, buf->phy_debug_cfg.cmd_str, &buf->phy_debug_cfg.portmask,
                        buf->phy_debug_cfg.param1, buf->phy_debug_cfg.param2, buf->phy_debug_cfg.param3, buf->phy_debug_cfg.param5, buf->phy_debug_cfg.param5);
            rtk_copy_to(user, &buf->port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_uart(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_UART1)
        case RTDRV_UART1_GETC:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = drv_uart_getc(buf->unit_cfg.unit, &buf->unit_cfg.data8, buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_UART1_BAUDRATE_GET:
            rtk_copy_from(&buf->unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = drv_uart_baudrate_get(buf->unit_cfg.unit, &buf->unit_cfg.data);
            rtk_copy_to(user, &buf->unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_mirror(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** MRIIOR **/
        case RTDRV_MIRROR_GROUP_INIT:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_group_init(buf->mirror_cfg.unit,&buf->mirror_cfg.mirrorEntry);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_GROUP_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_group_get(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.mirrorEntry);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_RSPAN_IGR_MODE_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_rspanIgrMode_get(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.data);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_RSPAN_EGR_MODE_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_rspanEgrMode_get(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.data);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_RSPAN_TAG_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_rspanTag_get(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.rspan_tag);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_SFLOW_MIRROR_SAMPLE_RATE_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowMirrorSampleRate_get(buf->mirror_cfg.unit, buf->mirror_cfg.mirror_id, &buf->mirror_cfg.data);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_EGRQUEUE_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_egrQueue_get(buf->mirror_cfg.unit, &buf->mirror_cfg.enable, &buf->mirror_cfg.qid);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_SFLOW_PORT_IGR_SAMPLE_RATE_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowPortIgrSampleRate_get(buf->mirror_cfg.unit, buf->mirror_cfg.port, &buf->mirror_cfg.data);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_SFLOW_PORT_EGR_SAMPLE_RATE_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowPortEgrSampleRate_get(buf->mirror_cfg.unit, buf->mirror_cfg.port, &buf->mirror_cfg.data);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_SFLOW_SAMPLE_CTRL_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowSampleCtrl_get(buf->mirror_cfg.unit, &buf->mirror_cfg.sample_ctrl);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_SFLOWSAMPLETARGET_GET:
            rtk_copy_from(&buf->mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_sflowSampleTarget_get(buf->mirror_cfg.unit, &buf->mirror_cfg.target);
            rtk_copy_to(user, &buf->mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_stack(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** STACK **/
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case RTDRV_STACK_PORT_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_port_get(buf->stack_cfg.unit, &buf->stack_cfg.stkPorts);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_UNIT_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_devId_get(buf->stack_cfg.unit, &buf->stack_cfg.myDevID);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_MASTERUNIT_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_masterDevId_get(buf->stack_cfg.unit, &buf->stack_cfg.masterDevID);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_LOOPGUARD_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_loopGuard_get(buf->stack_cfg.unit, &buf->stack_cfg.enable);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_UNITPORTMAP_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_devPortMap_get(buf->stack_cfg.unit, buf->stack_cfg.dstDevID, &buf->stack_cfg.stkPorts);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_NONUCASTBLOCKPORT_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_nonUcastBlockPort_get(buf->stack_cfg.unit, buf->stack_cfg.srcDevID, &buf->stack_cfg.stkPorts);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_RMTINTRTXENABLE_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrTxEnable_get(buf->stack_cfg.unit, &buf->stack_cfg.enable);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_RMTINTRTXTRIGGERENABLE_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrTxTriggerEnable_get(buf->stack_cfg.unit, &buf->stack_cfg.enable);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_RMTINTRRXSEQCMPMARGIN_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrRxSeqCmpMargin_get(buf->stack_cfg.unit, &buf->stack_cfg.margin);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_RMTINTRRXFORCEUPDATEENABLE_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrRxForceUpdateEnable_get(buf->stack_cfg.unit, &buf->stack_cfg.enable);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;

        case RTDRV_STACK_RMTINTRINFO_GET:
            rtk_copy_from(&buf->stack_cfg, user, sizeof(rtdrv_stackCfg_t));
            ret = rtk_stack_rmtIntrInfo_get(buf->stack_cfg.unit, &buf->stack_cfg.info);
            rtk_copy_to(user, &buf->stack_cfg, sizeof(rtdrv_stackCfg_t));
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_flowctrl(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** FLOWCRTL **/
        case RTDRV_FLOWCTRL_PORT_PAUSEON_ACTION_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portPauseOnAction_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.pauseOn_action);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PORT_PAUSEON_ALLOWED_PAGENUM_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portPauseOnAllowedPageNum_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.data);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTLEN_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_pauseOnAllowedPktLen_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.data);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PAUSEON_ALLOWED_PKTNUM_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_pauseOnAllowedPktNum_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.data);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_GUAR_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrGuarEnable_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_SYS_PAUSE_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrSystemPauseThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_JUMBO_SYS_PAUSE_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrJumboSystemPauseThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_PAUSE_THR_GROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrPauseThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PORT_IGR_PORT_THR_GROUP_SEL_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portIgrPortThreshGroupSel_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.grp_idx);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_SYS_CONGEST_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrSystemCongestThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_JUMBO_SYS_CONGEST_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrJumboSystemCongestThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_CONGEST_THR_GROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrCongestThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_JUMBO_STS_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_jumboModeStatus_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.data);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_JUMBO_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_jumboModeEnable_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_JUMBO_LEN_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_jumboModeLength_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.data);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_SYS_UTIL_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrSystemUtilThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.thresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_SYS_DROP_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrSystemDropThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortDropThresh_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_DROP_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortDropEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_QUEUE_DROP_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortQueueDropEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrQueueDropThresh_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_CPU_QUEUE_DROP_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrCpuQueueDropThresh_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PORT_EGR_DROP_REFCONGEST_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portEgrDropRefCongestEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_PORT_DROP_THR_GROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrPortDropThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_QUEUE_DROP_THR_GROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrQueueDropThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PORT_EGR_DROP_THR_GROUP_SEL_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portEgrDropThreshGroupSel_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.grp_idx);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_EGR_QUEUE_DROP_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_egrQueueDropEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.queue,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PORT_EGR_QUEUE_DROP_FORCE_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portEgrQueueDropForceEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.queue,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_DROP_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueueDropEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, buf->flowctrl_cfg.queue,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_DROP_THR_GROUP_SEL_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueuePauseDropThreshGroupSel_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.grp_idx);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_GROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueueDropThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, buf->flowctrl_cfg.queue,&buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_PAUSE_THR_GROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueuePauseThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, buf->flowctrl_cfg.queue, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_IGR_QUEUE_DROP_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_igrQueueDropThresh_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.queue,&buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_PORT_HOL_TRAFFIC_DROP_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portHolTrafficDropEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_HOL_TRAFFIC_TYPE_DROP_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_holTrafficTypeDropEnable_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.type,  &buf->flowctrl_cfg.enable);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_SPECIAL_CONGEST_THR_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_specialCongestThreshold_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_E2E_CASCADE_PORT_THRESH_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_e2eCascadePortThresh_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_E2E_REMOTE_PORTPAUSETHRESHGROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_e2eRemotePortPauseThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_E2E_REMOTE_PORTCONGESTTHRESHGROUP_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_e2eRemotePortCongestThreshGroup_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.grp_idx, &buf->flowctrl_cfg.dropThresh);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_E2E_PORT_REMOTE_PORT_THRESH_GROUP_SEL_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_portE2eRemotePortThreshGroupSel_get(buf->flowctrl_cfg.unit, buf->flowctrl_cfg.port, &buf->flowctrl_cfg.grp_idx);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

       case RTDRV_FLOWCTRL_TAGPAUSE_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_tagPauseEnable_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.tag_pause_en);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        case RTDRV_FLOWCTRL_HALF_CONSECUTIVE_RETRY_ENABLE_GET:
            rtk_copy_from(&buf->flowctrl_cfg, user, sizeof(rtdrv_flowctrlCfg_t));
            ret = rtk_flowctrl_halfConsecutiveRetryEnable_get(buf->flowctrl_cfg.unit, &buf->flowctrl_cfg.half_retry_en);
            rtk_copy_to(user, &buf->flowctrl_cfg, sizeof(rtdrv_flowctrlCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_rate(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** RATE **/
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_RATE_IGR_INCLUDE_IFG_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBandwidthCtrlIncludeIfg_get(buf->rate_cfg.unit, &buf->rate_cfg.ifg_include);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGR_INCLUDE_IFG_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_get(buf->rate_cfg.unit, &buf->rate_cfg.ifg_include);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORM_INCLUDE_IFG_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlIncludeIfg_get(buf->rate_cfg.unit, &buf->rate_cfg.ifg_include);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
#endif
        case RTDRV_RATE_INCLUDE_IFG_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_includeIfg_get(buf->rate_cfg.unit, buf->rate_cfg.module, &buf->rate_cfg.ifg_include);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwCtrlEnable_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwCtrlRate_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_IGR_BWCTRL_LOW_THRESH_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBandwidthLowThresh_get(buf->rate_cfg.unit, &buf->rate_cfg.thresh);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        case RTDRV_RATE_PORT_IGR_BWCTRL_HIGH_THRESH_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthHighThresh_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.thresh);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
#endif

        case RTDRV_RATE_IGR_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBwCtrlBurstSize_get(buf->rate_cfg.unit, &buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_IGR_PORT_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrPortBwCtrlBurstSize_get(buf->rate_cfg.unit,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwCtrlBurstSize_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.igrBwCfg);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_EXCEED_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthCtrlExceed_get(buf->rate_cfg.unit,buf->rate_cfg.port,&buf->rate_cfg.isExceed);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_IGR_BWCTRL_BYPASS_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrBandwidthCtrlBypass_get(buf->rate_cfg.unit, buf->rate_cfg.igrBypassType, &buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_BWCTRL_FLOWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBwFlowctrlEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlRate_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_IGR_QUEUE_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_igrQueueBwCtrlBurstSize_get(buf->rate_cfg.unit,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlBurstSize_get(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, &buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_IGR_QUEUE_BWCTRL_EXCEED_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrQueueBwCtrlExceed_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.isExceed);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_BWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBwCtrlEnable_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_BWCTRL_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBwCtrlRate_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBwCtrlBurstSize_get(buf->rate_cfg.unit, buf->rate_cfg.port, &buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_CPU_PORT_EGR_BWCTRL_RATE_MODE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_cpuEgrBandwidthCtrlRateMode_get(buf->rate_cfg.unit, &buf->rate_cfg.rate_mode);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGR_PORT_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrPortBwCtrlBurstSize_get(buf->rate_cfg.unit,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueBwCtrlEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueBwCtrlRate_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueBwCtrlBurstSize_get(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, &buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlRate_get(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, &buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.queue, &buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_EGR_QUEUE_ASSURED_BWCTRL_MODE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.assured_mode);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGR_QUEUE_FIXED_BWCTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueFixedBandwidthEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.queue,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGR_QUEUE_BWCTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlBurstSize_get(buf->rate_cfg.unit,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlRate_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,&buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORM_CTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlBurstSize_get(buf->rate_cfg.unit,buf->rate_cfg.storm_type,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlBurstSize_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_EXCEED_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlExceed_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_type,&buf->rate_cfg.isExceed);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORM_CTRL_RATE_MODE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlRateMode_get(buf->rate_cfg.unit,&buf->rate_cfg.storm_rate_mode);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_RATE_MODE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlRateMode_get(buf->rate_cfg.unit,buf->rate_cfg.port,&buf->rate_cfg.storm_rate_mode);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_TYPE_SEL_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlTypeSel_get(buf->rate_cfg.unit, buf->rate_cfg.port, buf->rate_cfg.storm_type, &buf->rate_cfg.storm_sel);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORM_CTRL_BYPASS_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlBypass_get(buf->rate_cfg.unit, buf->rate_cfg.stormBypassType, &buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoEnable_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_proto_type,&buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_RATE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoRate_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_proto_type,&buf->rate_cfg.rate);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_BURST_SIZE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoBurstSize_get(buf->rate_cfg.unit,buf->rate_cfg.port,buf->rate_cfg.storm_proto_type,&buf->rate_cfg.burst_size);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORT_STORM_CTRL_PROTO_EXCEED_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portStormCtrlProtoExceed_get(buf->rate_cfg.unit,buf->rate_cfg.port, buf->rate_cfg.storm_proto_type, &buf->rate_cfg.isExceed);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORM_CTRL_PROTO_VLAN_CONSTRT_ENABLE_GET:
            rtk_copy_from(&buf->rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormCtrlProtoVlanConstrtEnable_get(buf->rate_cfg.unit, buf->rate_cfg.storm_proto_type, &buf->rate_cfg.enable);
            rtk_copy_to(user, &buf->rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_switch(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SWITCH **/
        case RTDRV_SWITCH_CPU_MAX_PKTLEN_GET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_cpuMaxPktLen_get(buf->switch_cfg.unit, buf->switch_cfg.dir, &buf->switch_cfg.maxLen);
            rtk_copy_to(user, &buf->switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_MAX_PKTLEN_LINK_SPEED_GET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenLinkSpeed_get(buf->switch_cfg.unit, buf->switch_cfg.speed, &buf->switch_cfg.maxLen);
            rtk_copy_to(user, &buf->switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PORT_MAX_PKTLEN_LINK_SPEED_GET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_portMaxPktLenLinkSpeed_get(buf->switch_cfg.unit, buf->switch_cfg.port, buf->switch_cfg.speed, &buf->switch_cfg.maxLen);
            rtk_copy_to(user, &buf->switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_MAX_PKTLEN_TAGLENCNT_GET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenTagLenCntIncEnable_get(buf->switch_cfg.unit, &buf->switch_cfg.enable);
            rtk_copy_to(user, &buf->switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_SNAP_MODE_GET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_snapMode_get(buf->switch_cfg.unit, &buf->switch_cfg.snapMode);
            rtk_copy_to(user, &buf->switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PORT_MAX_PKTLEN_TAGLENCNT_INCENABLE_GET:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_portMaxPktLenTagLenCntIncEnable_get(buf->switch_cfg.unit, buf->switch_cfg.port, &buf->switch_cfg.enable);
            rtk_copy_to(user, &buf->switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_DEVICE_INFO_GET:
            rtk_copy_from(&buf->switch_devInfo, user, sizeof(rtdrv_switchDevInfo_t));
            ret = rtk_switch_deviceInfo_get(buf->switch_devInfo.unit, &buf->switch_devInfo.devInfo);
            rtk_copy_to(user, &buf->switch_devInfo, sizeof(rtdrv_switchDevInfo_t));
            break;
        case RTDRV_SWITCH_DEVICE_CAPABILITY_PRINT:
            rtk_copy_from(&buf->switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_deviceCapability_print(buf->switch_cfg.unit);
            break;

        case RTDRV_SWITCH_CHKSUMFAILACTION_GET :
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_chksumFailAction_get(buf->switch_cfgParam.unit, buf->switch_cfgParam.port,
                buf->switch_cfgParam.failType, &buf->switch_cfgParam.action);
            rtk_copy_to(user, &buf->switch_cfgParam, sizeof(rtdrv_switchCfgParam_t));
            break;

        case RTDRV_SWITCH_RECALCCRCENABLE_GET :
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_recalcCRCEnable_get(buf->switch_cfgParam.unit, buf->switch_cfgParam.port,
                &buf->switch_cfgParam.enable);
            rtk_copy_to(user, &buf->switch_cfgParam, sizeof(rtdrv_switchCfgParam_t));
            break;

        case RTDRV_SWITCH_MGMTMACADDR_GET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_mgmtMacAddr_get(buf->switch_cfgInfo.unit, &buf->switch_cfgInfo.mac);
            rtk_copy_to(user, &buf->switch_cfgInfo, sizeof(rtdrv_switchCfgInfo_t));
            break;

        case RTDRV_SWITCH_IPV4ADDR_GET :
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_IPv4Addr_get(buf->switch_cfgInfo.unit, &buf->switch_cfgInfo.ipv4Addr);
            rtk_copy_to(user, &buf->switch_cfgInfo, sizeof(rtdrv_switchCfgInfo_t));
            break;

        case RTDRV_SWITCH_PKT2CPUTYPEFORMAT_GET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_pkt2CpuTypeFormat_get(buf->switch_cfgInfo.unit,
                    buf->switch_cfgInfo.trap_type, &buf->switch_cfgInfo.format);
            rtk_copy_to(user, &buf->switch_cfgInfo, sizeof(rtdrv_switchCfgInfo_t));
            break;

        case RTDRV_SWITCH_PPPOE_IP_PARSE_ENABLE_GET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = rtk_switch_pppoeIpParseEnable_get(buf->switch_cfgInfo.unit, &buf->switch_cfgInfo.enable);
            rtk_copy_to(user, &buf->switch_cfgInfo, sizeof(rtdrv_switchCfgInfo_t));
            break;

#if defined(CONFIG_SDK_DRIVER_WATCHDOG)
        case RTDRV_SWITCH_WATCHDOG_ENABLE_GET:
            rtk_copy_from(&buf->switch_cfgInfo, user, sizeof(rtdrv_switchCfgInfo_t));
            ret = drv_watchdog_enable_get(buf->switch_cfgInfo.unit, &buf->switch_cfgInfo.enable);
            rtk_copy_to(user, &buf->switch_cfgInfo, sizeof(rtdrv_switchCfgInfo_t));
            break;

        case RTDRV_SWITCH_WATCHDOG_SCALE_GET:
            rtk_copy_from(&buf->watchdog_cfg, user, sizeof(rtdrv_watchdogCfgInfo_t));
            ret = drv_watchdog_scale_get(buf->switch_cfgInfo.unit, &buf->watchdog_cfg.scale);
            rtk_copy_to(user, &buf->watchdog_cfg, sizeof(rtdrv_watchdogCfgInfo_t));
            break;

        case RTDRV_SWITCH_WATCHDOG_THRESHOLD_GET:
            rtk_copy_from(&buf->watchdog_cfg, user, sizeof(rtdrv_watchdogCfgInfo_t));
            ret = drv_watchdog_threshold_get(buf->watchdog_cfg.unit, &buf->watchdog_cfg.threshold);
            rtk_copy_to(user, &buf->watchdog_cfg, sizeof(rtdrv_watchdogCfgInfo_t));
            break;
#endif //CONFIG_SDK_DRIVER_WATCHDOG

#if defined(CONFIG_SDK_TC_DRV)
        case RTDRV_SWITCH_TC_COUNTERVALUE_GET:
            rtk_copy_from(&buf->tc_cfg, user, sizeof(rtdrv_tcCfgInfo_t));
            ret = drv_tc_counterValue_get(buf->tc_cfg.unit, buf->tc_cfg.id, &buf->tc_cfg.value);
            rtk_copy_to(user, &buf->tc_cfg, sizeof(rtdrv_tcCfgInfo_t));
            break;
#endif //CONFIG_SDK_TC_DRV

        case RTDRV_SWITCH_CPU_PKT_TRUNCATE_EN_GET:
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_cpuPktTruncateEnable_get(buf->switch_cfgParam.unit, &buf->switch_cfgParam.enable);
            rtk_copy_to(user, &buf->switch_cfgParam, sizeof(rtdrv_switchCfgParam_t));
            break;

        case RTDRV_SWITCH_CPU_PKT_TRUNCATE_LEN_GET:
            rtk_copy_from(&buf->switch_cfgParam, user, sizeof(rtdrv_switchCfgParam_t));
            ret = rtk_switch_cpuPktTruncateLen_get(buf->switch_cfgParam.unit, &buf->switch_cfgParam.maxLen);
            rtk_copy_to(user, &buf->switch_cfgParam, sizeof(rtdrv_switchCfgParam_t));
            break;

        case RTDRV_SWITCH_FLEXTBLFMT_GET:
            rtk_copy_from(&buf->switch_cfgTable, user, sizeof(rtdrv_switchCfgTable_t));
            ret = rtk_switch_flexTblFmt_get(buf->switch_cfgTable.unit, &buf->switch_cfgTable.tbl_fmt);
            rtk_copy_to(user, &buf->switch_cfgTable, sizeof(rtdrv_switchCfgTable_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

#ifdef CONFIG_RISE
int32 (*rtdrv_rise_dc_show)(uint32) = NULL;
int32 (*rtdrv_rise_dcbox_show)(int,void *,void *) = NULL;
int32 (*rtdrv_rise_ta_show)(uint32) = NULL;
int32 (*rtdrv_rise_cfg_show)(uint32) = NULL;
int32 (*rtdrv_rise_port_show)(uint32) = NULL;
int32 (*rtdrv_rise_cmd_show)(uint32, char*, int) = NULL;
int32 (*rtdrv_rise_application_kickoff)(uint32 *) = NULL;

int32 rtdrv_callbackDcShow_reg(int32 (*func)(uint32))
{
    rtdrv_rise_dc_show = func;
    return RT_ERR_OK;
}
int32 rtdrv_callbackDcBoxShow_reg(int32 (*func)(int,void *,void *))
{
    rtdrv_rise_dcbox_show = func;
    return RT_ERR_OK;
}
int32 rtdrv_callbackTaShow_reg(int32 (*func)(uint32))
{
    rtdrv_rise_ta_show = func;
    return RT_ERR_OK;
}
int32 rtdrv_callbackCfgShow_reg(int32 (*func)(uint32))
{
    rtdrv_rise_cfg_show = func;
    return RT_ERR_OK;
}
int32 rtdrv_callbackPortShow_reg(int32 (*func)(uint32))
{
    rtdrv_rise_port_show = func;
    return RT_ERR_OK;
}
int32 rtdrv_callbackCmdShow_reg(int32 (*func)(uint32, char *,int))
{
    rtdrv_rise_cmd_show = func;
    return RT_ERR_OK;
}

int32 rtdrv_callbackAppKickoff_reg(int32 (*func)(uint32 *))
{
    rtdrv_rise_application_kickoff = func;
    return RT_ERR_OK;
}


EXPORT_SYMBOL(rtdrv_callbackDcShow_reg);
EXPORT_SYMBOL(rtdrv_callbackDcBoxShow_reg);
EXPORT_SYMBOL(rtdrv_callbackTaShow_reg);
EXPORT_SYMBOL(rtdrv_callbackCfgShow_reg);
EXPORT_SYMBOL(rtdrv_callbackPortShow_reg);
EXPORT_SYMBOL(rtdrv_callbackCmdShow_reg);
EXPORT_SYMBOL(rtdrv_callbackAppKickoff_reg);



#endif //CONFIG_RISE

int32 do_rtdrv_get_ctl_sys(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SYSTEM **/
        case RTDRV_SYS_HWP_DUMP_INFO:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            ret = hwp_info_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_HWP_DUMP_PARSED:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            ret = hwp_parsedInfo_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_HWP_DUMP_UNIT:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            ret = hwp_unit_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
#ifdef CONFIG_RISE
        case RTDRV_SYS_RTSTK_DC_SHOW:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_dc_show)
                ret = rtdrv_rise_dc_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_RTSTK_DCBOX_SHOW:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_dcbox_show)
                ret = rtdrv_rise_dcbox_show(2, NULL, NULL);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_RTSTK_TA_SHOW:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_ta_show)
                ret = rtdrv_rise_ta_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_RTSTK_CFG_SHOW:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_cfg_show)
                ret = rtdrv_rise_cfg_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_RTSTK_PORT_SHOW:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_port_show)
                ret = rtdrv_rise_port_show(buf->sys_cfg.unit);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_RTSTK_CMD_SHOW:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_cmd_show)
                ret = rtdrv_rise_cmd_show(buf->sys_cfg.unit, buf->sys_cfg.cmd_str, buf->sys_cfg.cmd_int);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));
            break;
        case RTDRV_SYS_RISE_KICKOFF_WAIT:
            rtk_copy_from(&buf->sys_cfg, user, sizeof(rtdrv_sysCfg_t));
            if (rtdrv_rise_application_kickoff)
                ret = rtdrv_rise_application_kickoff(&buf->sys_cfg.tmp);
            rtk_copy_to(user, &buf->sys_cfg, sizeof(rtdrv_sysCfg_t));

            break;
#endif //CONFIG_RISE
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_get_ctl_nic(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
    /** NIC **/
        case RTDRV_NIC_DEBUG_GET:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_dbg_get(buf->nic_cfg.unit, &buf->nic_cfg.flags);
            rtk_copy_to(user, &buf->nic_cfg, sizeof(rtdrv_nicCfg_t));
            break;

        case RTDRV_NIC_RX_STATUS_GET:
            rtk_copy_from(&buf->nic_cfg, user, sizeof(rtdrv_nicCfg_t));
            ret = drv_nic_rx_status_get(buf->nic_cfg.unit, &buf->nic_cfg.rx_status);
            rtk_copy_to(user, &buf->nic_cfg, sizeof(rtdrv_nicCfg_t));
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_sdk(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SDK **/
        case RTDRV_SDK_TEST_MODE_GET:
            rtk_copy_from(&buf->sdk_cfg, user, sizeof(rtdrv_sdkCfg_t));
            if (sdkTest_drv.mode_get != NULL)
                ret = sdkTest_drv.mode_get(&buf->sdk_cfg.mode);
            rtk_copy_to(user, &buf->sdk_cfg, sizeof(rtdrv_sdkCfg_t));
            break;
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_eee(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** EEE **/
        case RTDRV_EEE_PORT_ENABLE_GET:
            rtk_copy_from(&buf->eee_cfg, user, sizeof(rtdrv_eeeCfg_t));
            ret = rtk_eee_portEnable_get(buf->eee_cfg.unit, buf->eee_cfg.port, &buf->eee_cfg.enable);
            rtk_copy_to(user, &buf->eee_cfg, sizeof(rtdrv_eeeCfg_t));
            break;

        case RTDRV_EEE_PORT_STATE_GET:
            rtk_copy_from(&buf->eee_cfg, user, sizeof(rtdrv_eeeCfg_t));
            ret = rtk_eee_portState_get(buf->eee_cfg.unit, buf->eee_cfg.port, &buf->eee_cfg.enable);
            rtk_copy_to(user, &buf->eee_cfg, sizeof(rtdrv_eeeCfg_t));
            break;

        case RTDRV_EEEP_PORT_ENABLE_GET:
            rtk_copy_from(&buf->eee_cfg, user, sizeof(rtdrv_eeeCfg_t));
            ret = rtk_eeep_portEnable_get(buf->eee_cfg.unit, buf->eee_cfg.port, &buf->eee_cfg.enable);
            rtk_copy_to(user, &buf->eee_cfg, sizeof(rtdrv_eeeCfg_t));
            break;

        case RTDRV_EEE_PORT_POWER_STATE_GET:
            rtk_copy_from(&buf->eee_cfg, user, sizeof(rtdrv_eeeCfg_t));
            ret = rtk_eee_portPowerState_get(buf->eee_cfg.unit, buf->eee_cfg.port, buf->eee_cfg.direction, &buf->eee_cfg.state);
            rtk_copy_to(user, &buf->eee_cfg, sizeof(rtdrv_eeeCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_sec(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** SEC **/
        case RTDRV_SEC_PORT_ATTACK_PREVENT_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portAttackPrevent_get(buf->sec_cfg.unit, buf->sec_cfg.port, buf->sec_cfg.attack_type
                                        , &buf->sec_cfg.action);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_PORT_ATTACK_PREVENT_ENABLE_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portAttackPreventEnable_get(buf->sec_cfg.unit, buf->sec_cfg.port, &buf->sec_cfg.enable);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_ATTACK_PREVENT_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackPreventAction_get(buf->sec_cfg.unit, buf->sec_cfg.attack_type
                                        , &buf->sec_cfg.action);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_MIN_IPV6_FRAG_LEN_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_minIPv6FragLen_get(buf->sec_cfg.unit, &buf->sec_cfg.data);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_MAX_PING_LEN_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_maxPingLen_get(buf->sec_cfg.unit, &buf->sec_cfg.data);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_MIN_TCP_HDR_LEN_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_minTCPHdrLen_get(buf->sec_cfg.unit, &buf->sec_cfg.data);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_SMURF_NETMASK_LEN_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_smurfNetmaskLen_get(buf->sec_cfg.unit, &buf->sec_cfg.data);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_TRAPTARGET_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_trapTarget_get(buf->sec_cfg.unit, &buf->sec_cfg.target);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_IPMACBINDACTION_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_ipMacBindAction_get(buf->sec_cfg.unit, &buf->sec_cfg.lumisAct, &buf->sec_cfg.matchAct, &buf->sec_cfg.mismatchAct);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_PORTIPMACBINDENABLE_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portIpMacBindEnable_get(buf->sec_cfg.unit, buf->sec_cfg.port, buf->sec_cfg.type, &buf->sec_cfg.enable);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_IPMACBINDENTRY_GETNEXT:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_ipMacBindEntry_getNext(buf->sec_cfg.unit, &buf->sec_cfg.base, &buf->sec_cfg.entry);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_ATTACKPREVENTHIT_GET:
            rtk_copy_from(&buf->sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackPreventHit_get(buf->sec_cfg.unit, buf->sec_cfg.attack_type, &buf->sec_cfg.data);
            rtk_copy_to(user, &buf->sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_led(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /** LED **/
        case RTDRV_LED_SYS_ENABLE_GET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_sysEnable_get(buf->led_cfg.unit, buf->led_cfg.type, &buf->led_cfg.enable);
            rtk_copy_to(user, &buf->led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_PORTLEDENTITYSWCTRLENABLE_GET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_portLedEntitySwCtrlEnable_get(buf->led_cfg.unit,
                    buf->led_cfg.port, buf->led_cfg.entity, &buf->led_cfg.enable);
            rtk_copy_to(user, &buf->led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_PORTLEDENTITYSWCTRLMODE_GET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_portLedEntitySwCtrlMode_get(buf->led_cfg.unit,
                    buf->led_cfg.port, buf->led_cfg.entity, buf->led_cfg.media,
                    &buf->led_cfg.mode);
            rtk_copy_to(user, &buf->led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_SYSMODE_GET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_sysMode_get(buf->led_cfg.unit, &buf->led_cfg.mode);
            rtk_copy_to(user, &buf->led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_BLINKTIME_GET:
            rtk_copy_from(&buf->led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_blinkTime_get(buf->led_cfg.unit, &buf->led_cfg.time);
            rtk_copy_to(user, &buf->led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_mpls(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
        /* MPLS */
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310))
        case RTDRV_MPLS_TTLINHERIT_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_ttlInherit_get(buf->mpls_cfg.unit, &buf->mpls_cfg.u.inherit);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_ENABLE_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_enable_get(buf->mpls_cfg.unit, &buf->mpls_cfg.enable);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_TRAPTARGET_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_trapTarget_get(buf->mpls_cfg.unit, &buf->mpls_cfg.u.target);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_EXCEPTIONCTRL_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_exceptionCtrl_get(buf->mpls_cfg.unit, buf->mpls_cfg.u.exceptionType, &buf->mpls_cfg.action);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_NEXTHOP_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_nextHop_get(buf->mpls_cfg.unit, buf->mpls_cfg.pathId, &buf->mpls_cfg.u.nexthop);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_ENCAP_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_encap_get(buf->mpls_cfg.unit, buf->mpls_cfg.entryId, &buf->mpls_cfg.u.encap);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_ENCAPID_FIND:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_encapId_find(buf->mpls_cfg.unit, &buf->mpls_cfg.u.encap, &buf->mpls_cfg.entryId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_HASHALGO_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_hashAlgo_get(buf->mpls_cfg.unit, &buf->mpls_cfg.hashAlgo);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_DECAP_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_decap_get(buf->mpls_cfg.unit, buf->mpls_cfg.entryId, &buf->mpls_cfg.u.decap);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_DECAPID_FIND:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_decapId_find(buf->mpls_cfg.unit, &buf->mpls_cfg.u.decap, &buf->mpls_cfg.entryId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_EGRTCMAP_GET:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_egrTcMap_get(buf->mpls_cfg.unit, &buf->mpls_cfg.src, &buf->mpls_cfg.tc);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_NEXTHOP_CREATE_ID:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_nextHop_create_id(buf->mpls_cfg.unit, &buf->mpls_cfg.u.nexthop, buf->mpls_cfg.pathId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;

        case RTDRV_MPLS_ENCAP_CREATE_ID:
            rtk_copy_from(&buf->mpls_cfg, user, sizeof(rtdrv_mplsCfg_t));
            ret = rtk_mpls_encap_create_id(buf->mpls_cfg.unit, &buf->mpls_cfg.u.encap, buf->mpls_cfg.entryId);
            rtk_copy_to(user, &buf->mpls_cfg, sizeof(rtdrv_mplsCfg_t));
            break;
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)) */

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_sc(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
        case RTDRV_DIAG_SC_REG_READ:
            rtk_copy_from(&buf->sc_regInfo, user, sizeof(rtdrv_scRegInfo_t));
            ret = hal_rtl8295_reg_read(buf->sc_regInfo.unit, buf->sc_regInfo.port, buf->sc_regInfo.addr, &buf->sc_regInfo.data);
            rtk_copy_to(user, &buf->sc_regInfo, sizeof(rtdrv_scRegInfo_t));
            break;
        case RTDRV_DIAG_SC_SDS_READ:
            rtk_copy_from(&buf->sc_sdsInfo, user, sizeof(rtdrv_scSdsInfo_t));
            ret = hal_rtl8295_sds_read(buf->sc_sdsInfo.unit, buf->sc_sdsInfo.port, buf->sc_sdsInfo.sds, buf->sc_sdsInfo.page, buf->sc_sdsInfo.reg, &buf->sc_sdsInfo.data);
            rtk_copy_to(user, &buf->sc_sdsInfo, sizeof(rtdrv_scSdsInfo_t));
            break;
#endif
#if defined(CONFIG_SDK_RTL8295R)
        case RTDRV_DIAG_SC_8295R_RXCALICONF_GET:
            {
                phy_8295_rxCaliConf_t   rxCaliConf;

                rtk_copy_from(&buf->rxCaliConf, user, sizeof(rtdrv_8295_rxCaliConf_t));
                ret = phy_8295r_rxCaliConfPort_get(buf->rxCaliConf.unit, buf->rxCaliConf.port, &rxCaliConf);
                buf->rxCaliConf.dacLongCableOffset         = rxCaliConf.dacLongCableOffset;
                buf->rxCaliConf.s1rxCaliEnable             = rxCaliConf.s1.rxCaliEnable;
                buf->rxCaliConf.s1tap0InitVal              = rxCaliConf.s1.tap0InitVal;
                buf->rxCaliConf.s1vthMinThr                = rxCaliConf.s1.vthMinThr;
                buf->rxCaliConf.s1eqHoldEnable             = rxCaliConf.s1.eqHoldEnable;
                buf->rxCaliConf.s1dfeTap1_4Enable          = rxCaliConf.s1.dfeTap1_4Enable;
                buf->rxCaliConf.s1dfeAuto                  = rxCaliConf.s1.dfeAuto;
                buf->rxCaliConf.s0rxCaliEnable             = rxCaliConf.s0.rxCaliEnable;
                buf->rxCaliConf.s0tap0InitVal              = rxCaliConf.s0.tap0InitVal;
                buf->rxCaliConf.s0vthMinThr                = rxCaliConf.s0.vthMinThr;
                buf->rxCaliConf.s0eqHoldEnable             = rxCaliConf.s0.eqHoldEnable;
                buf->rxCaliConf.s0dfeTap1_4Enable          = rxCaliConf.s0.dfeTap1_4Enable;
                buf->rxCaliConf.s0dfeAuto                  = rxCaliConf.s0.dfeAuto;

                rtk_copy_to(user, &buf->rxCaliConf, sizeof(rtdrv_8295_rxCaliConf_t));
            }
            break;
#endif/* defined(CONFIG_SDK_RTL8295R) */
#if defined(CONFIG_SDK_RTL8214QF)
        case RTDRV_DIAG_SC_8214QF_RXCALICONF_GET:
            {
                phy_8295_rxCaliConf_t   rxCaliConf;

                rtk_copy_from(&buf->rxCaliConf, user, sizeof(rtdrv_8295_rxCaliConf_t));
                ret = phy_8214qf_rxCaliConf_get(buf->rxCaliConf.unit, buf->rxCaliConf.port, &rxCaliConf);
                buf->rxCaliConf.dacLongCableOffset         = rxCaliConf.dacLongCableOffset;
                buf->rxCaliConf.s1rxCaliEnable             = rxCaliConf.s1.rxCaliEnable;
                buf->rxCaliConf.s1tap0InitVal              = rxCaliConf.s1.tap0InitVal;
                buf->rxCaliConf.s1vthMinThr                = rxCaliConf.s1.vthMinThr;
                buf->rxCaliConf.s1eqHoldEnable             = rxCaliConf.s1.eqHoldEnable;
                buf->rxCaliConf.s1dfeTap1_4Enable          = rxCaliConf.s1.dfeTap1_4Enable;
                buf->rxCaliConf.s1dfeAuto                  = rxCaliConf.s1.dfeAuto;
                buf->rxCaliConf.s0rxCaliEnable             = rxCaliConf.s0.rxCaliEnable;
                buf->rxCaliConf.s0tap0InitVal              = rxCaliConf.s0.tap0InitVal;
                buf->rxCaliConf.s0vthMinThr                = rxCaliConf.s0.vthMinThr;
                buf->rxCaliConf.s0eqHoldEnable             = rxCaliConf.s0.eqHoldEnable;
                buf->rxCaliConf.s0dfeTap1_4Enable          = rxCaliConf.s0.dfeTap1_4Enable;
                buf->rxCaliConf.s0dfeAuto                  = rxCaliConf.s0.dfeAuto;

                rtk_copy_to(user, &buf->rxCaliConf, sizeof(rtdrv_8295_rxCaliConf_t));
            }
            break;
#endif/* defined(CONFIG_SDK_RTL8214QF) */

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_rtl8231(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL8231)
    /** RTL8231 **/
        case RTDRV_RTL8231_I2C_READ:
            rtk_copy_from(&buf->rtl8231_cfg, user, sizeof(rtdrv_rtl8231Cfg_t));
            ret = drv_rtl8231_i2c_read(buf->rtl8231_cfg.unit, buf->rtl8231_cfg.phyId_or_slaveAddr, buf->rtl8231_cfg.reg_addr, &buf->rtl8231_cfg.data);
            rtk_copy_to(user, &buf->rtl8231_cfg, sizeof(rtdrv_rtl8231Cfg_t));
            break;

        case RTDRV_RTL8231_MDC_READ:
            rtk_copy_from(&buf->rtl8231_cfg, user, sizeof(rtdrv_rtl8231Cfg_t));
            ret = drv_rtl8231_mdc_read(buf->rtl8231_cfg.unit, buf->rtl8231_cfg.phyId_or_slaveAddr, buf->rtl8231_cfg.page, buf->rtl8231_cfg.reg_addr, &buf->rtl8231_cfg.data);
            rtk_copy_to(user, &buf->rtl8231_cfg, sizeof(rtdrv_rtl8231Cfg_t));
            break;

        case RTDRV_EXTGPIO_DEV_READY_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_devReady_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_DEV_ENABLE_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_devEnable_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_SYNC_ENABLE_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_syncEnable_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_SYNC_STATUS_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_syncStatus_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_DATABIT_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_dataBit_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_REG_READ:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_reg_read(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.reg, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_DEV_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_dev_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, &buf->extGpio_cfg.extGpio_devConfData);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_PIN_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_pin_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, &buf->extGpio_cfg.extGpio_confData);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_DIRECTION_GET:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_direction_get(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.gpioId, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;

        case RTDRV_EXTGPIO_I2C_READ:
            rtk_copy_from(&buf->extGpio_cfg, user, sizeof(rtdrv_extGpioCfg_t));
            ret = drv_extGpio_i2c_read(buf->extGpio_cfg.unit, buf->extGpio_cfg.dev, buf->extGpio_cfg.reg, &buf->extGpio_cfg.data);
            rtk_copy_to(user, &buf->extGpio_cfg, sizeof(rtdrv_extGpioCfg_t));
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_gpio(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
        case RTDRV_GPIO_DATABIT_GET:
            rtk_copy_from(&buf->gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = drv_gpio_dataBit_get(buf->gpio_cfg.unit, buf->gpio_cfg.gpioId, &buf->gpio_cfg.data);
            rtk_copy_to(user, &buf->gpio_cfg, sizeof(rtdrv_gpioCfg_t));
            break;
#endif /* CONFIG_SDK_DRIVER_GPIO */

#if defined(CONFIG_SDK_RTL8231) || defined(CONFIG_SDK_DRIVER_GPIO)
        case RTDRV_GENCTRL_GPIO_DATABIT_GET:
            rtk_copy_from(&buf->gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = drv_generalCtrlGPIO_dataBit_get(buf->genCtrlGPIO_cfg.unit, buf->genCtrlGPIO_cfg.dev, buf->genCtrlGPIO_cfg.gpioId, &buf->genCtrlGPIO_cfg.data);
            if(ret != RT_ERR_OK)
                osal_printf("\nGet drv_generalCtrlGPIO_dataBit_get() ERROR = %d\n",ret);
            rtk_copy_to(user, &buf->gpio_cfg, sizeof(rtdrv_gpioCfg_t));
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_i2c(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_I2C)
        case RTDRV_I2C_READ:
            rtk_copy_from(&buf->i2c_cfg, user, sizeof(rtdrv_i2c_devCfg_t));
            ret = drv_i2c_read(buf->i2c_cfg.unit_id, buf->i2c_cfg.device_id, buf->i2c_cfg.reg_idx, &buf->i2c_cfg.rwdata[0]);
            rtk_copy_to(user, &buf->i2c_cfg, sizeof(rtdrv_i2c_devCfg_t));
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}
int32 do_rtdrv_get_ctl_spi(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_DRIVER_SPI)
        case RTDRV_SPI_READ:
            rtk_copy_from(&buf->spi_cfg, user, sizeof(rtdrv_spiCfg_t));
            ret = drv_spi_read(buf->spi_cfg.unit, buf->spi_cfg.addr, &buf->spi_cfg.data);
            rtk_copy_to(user, &buf->spi_cfg, sizeof(rtdrv_spiCfg_t));
            break;
#endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_diag(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* DIAG */
        case RTDRV_DIAG_RTCTRESULT_GET:
            rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
            ret = rtk_diag_portRtctResult_get(buf->diag_cfg.unit, buf->diag_cfg.port, &buf->diag_cfg.rtctResult);
            rtk_copy_to(user, &buf->diag_cfg, sizeof(rtdrv_diagCfg_t));
            break;
        case RTDRV_DIAG_TABLE_WHOLE_READ:
            rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
            ret = rtk_diag_table_whole_read((uint32)buf->diag_cfg.unit, (uint32)buf->diag_cfg.target_index);
            rtk_copy_to(user, &buf->diag_cfg, sizeof(rtdrv_diagCfg_t));
            break;
        case RTDRV_DIAG_TABLE_INDEX_NAME:
            rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
            ret = rtk_diag_table_index_name((uint32)buf->diag_cfg.unit, (uint32)buf->diag_cfg.target_index);
            rtk_copy_to(user, &buf->diag_cfg, sizeof(rtdrv_diagCfg_t));
            break;
         case RTDRV_DIAG_REG_WHOLE_READ:
            rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
            ret = rtk_diag_reg_whole_read((uint32)buf->diag_cfg.unit);
            rtk_copy_to(user, &buf->diag_cfg, sizeof(rtdrv_diagCfg_t));
            break;
         case RTDRV_DIAG_PERIPHERAL_REG_READ:
             rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
             ret = rtk_diag_peripheral_register_dump((uint32)buf->diag_cfg.unit);
             rtk_copy_to(user, &buf->diag_cfg, sizeof(rtdrv_diagCfg_t));
             break;
         case RTDRV_DIAG_PHY_REG_READ:
             rtk_copy_from(&buf->diag_cfg, user, sizeof(rtdrv_diagCfg_t));
             ret = rtk_diag_phy_reg_whole_read((uint32)buf->diag_cfg.unit);
             rtk_copy_to(user, &buf->diag_cfg, sizeof(rtdrv_diagCfg_t));
             break;
        case RTDRV_DIAG_TABLE_ENTRY_READ:
             rtk_copy_from(&buf->table_entry_info, user, sizeof(rtdrv_diag_tableEntryRead_t));
             ret = rtk_diag_tableEntry_read(buf->table_entry_info.unit, buf->table_entry_info.table_index, buf->table_entry_info.ent_start_index, buf->table_entry_info.ent_end_index, buf->table_entry_info.detail);
             rtk_copy_to(user, &buf->table_entry_info, sizeof(rtdrv_diag_tableEntryRead_t));
             break;
         #if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
         case RTDRV_DIAG_DEBUG_FIELD_GET:
             rtk_copy_from(&buf->diag_debug_cfg, user, sizeof(rtdrv_diag_debug_cfg_t));
             ret = rtk_diag_table_reg_field_get((uint32)buf->diag_debug_cfg.unit, &buf->diag_debug_cfg.diag_debug);
             rtk_copy_to(user, &buf->diag_debug_cfg, sizeof(rtdrv_diag_debug_cfg_t));
             break;
        #endif

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_openflow(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
#if defined(CONFIG_SDK_RTL9310)
        /* OpenFlow */
        case RTDRV_OF_CLASSIFIER_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_classifier_get(buf->openflow_cfg.unit, buf->openflow_cfg.classifyType, &buf->openflow_cfg.classifyData);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWMATCHFIELDSIZE_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowMatchFieldSize_get(buf->openflow_cfg.unit, buf->openflow_cfg.matchFieldType, &buf->openflow_cfg.field_size);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYSIZE_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntrySize_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, &buf->openflow_cfg.entry_size);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYVALIDATE_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryValidate_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.pValid);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYFIELDLIST_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryFieldList_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, &buf->openflow_cfg.matchFieldList);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYFIELD_CHECK:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryField_check(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.matchFieldType);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYSETFIELD_CHECK:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntrySetField_check(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.field_id, buf->openflow_cfg.setFieldType);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYFIELD_READ:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryField_read(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.matchFieldType, buf->openflow_cfg.fieldData, buf->openflow_cfg.fieldMask);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYOPERATION_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryOperation_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.pOperation);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYINSTRUCTION_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryInstruction_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.flowInsData);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWENTRYHITSTS_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowEntryHitSts_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.reset, &buf->openflow_cfg.isHit);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FTTEMPLATESELECTOR_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_ftTemplateSelector_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.block_idx, &buf->openflow_cfg.template_idx);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWCNTMODE_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowCntMode_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.mode);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWCNT_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowCnt_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, buf->openflow_cfg.flowCntType, &buf->openflow_cfg.flowCnt);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWCNTTHRESH_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowCntThresh_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.threshold);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_TTLEXCPT_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_ttlExcpt_get(buf->openflow_cfg.unit, &buf->openflow_cfg.action);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_MAXLOOPBACK_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_maxLoopback_get(buf->openflow_cfg.unit, &buf->openflow_cfg.times);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L2FLOWTBLMATCHFIELD_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowTblMatchField_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l2Field);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L2FLOWENTRYSETFIELD_CHECK:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntrySetField_check(buf->openflow_cfg.unit, buf->openflow_cfg.field_id, buf->openflow_cfg.setFieldType);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L2FLOWENTRY_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntry_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l2Entry);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L2FLOWENTRYNEXTVALID_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntryNextValid_get(buf->openflow_cfg.unit, &buf->openflow_cfg.scan_idx, &buf->openflow_cfg.l2Entry);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L2FLOWTBLHASHALGO_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowTblHashAlgo_get(buf->openflow_cfg.unit, buf->openflow_cfg.block, &buf->openflow_cfg.algo);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L2FLOWENTRYHITSTS_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l2FlowEntryHitSts_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l2Entry, buf->openflow_cfg.reset, &buf->openflow_cfg.isHit);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3FLOWTBLPRI_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3FlowTblPri_get(buf->openflow_cfg.unit, &buf->openflow_cfg.table);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3CAMFLOWTBLMATCHFIELD_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamFlowTblMatchField_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l3CamField);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3HASHFLOWTBLMATCHFIELD_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowTblMatchField_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l3HashField);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3HASHFLOWTBLHASHALGO_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowTblHashAlgo_get(buf->openflow_cfg.unit, buf->openflow_cfg.block, &buf->openflow_cfg.algo);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3FLOWENTRYSETFIELD_CHECK:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3FlowEntrySetField_check(buf->openflow_cfg.unit, buf->openflow_cfg.field_id, buf->openflow_cfg.setFieldType);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3CAMFLOWENTRY_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamFlowEntry_get(buf->openflow_cfg.unit, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.l3CamEntry);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3CAMFLOWENTRYHITSTS_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3CamflowEntryHitSts_get(buf->openflow_cfg.unit, buf->openflow_cfg.entry_idx, buf->openflow_cfg.reset, &buf->openflow_cfg.isHit);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3HASHFLOWENTRY_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowEntry_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l3HashEntry);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3HASHFLOWENTRYNEXTVALID_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashFlowEntryNextValid_get(buf->openflow_cfg.unit, &buf->openflow_cfg.scan_idx, &buf->openflow_cfg.l3HashEntry);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_L3HASHFLOWENTRYHITSTS_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_l3HashflowEntryHitSts_get(buf->openflow_cfg.unit, &buf->openflow_cfg.l3HashEntry, buf->openflow_cfg.reset, &buf->openflow_cfg.isHit);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_GROUPENTRY_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_groupEntry_get(buf->openflow_cfg.unit, buf->openflow_cfg.entry_idx, &buf->openflow_cfg.grpEntry);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_GROUPTBLHASHPARA_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_groupTblHashPara_get(buf->openflow_cfg.unit, &buf->openflow_cfg.para);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_ACTIONBUCKET_GET:
            rtk_copy_from(&buf->openflowAB_cfg, user, sizeof(rtdrv_openflowABCfg_t));
            ret = rtk_of_actionBucket_get(buf->openflowAB_cfg.unit, buf->openflowAB_cfg.entry_idx, &buf->openflowAB_cfg.actionBktEntry);
            rtk_copy_to(user, &buf->openflowAB_cfg, sizeof(rtdrv_openflowABCfg_t));
            break;

        case RTDRV_OF_TRAPTARGET_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_trapTarget_get(buf->openflow_cfg.unit, &buf->openflow_cfg.target);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_TBLMISSACTION_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_tblMissAction_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, &buf->openflow_cfg.tblMissAct);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;

        case RTDRV_OF_FLOWTBLCNT_GET:
            rtk_copy_from(&buf->openflow_cfg, user, sizeof(rtdrv_openflowCfg_t));
            ret = rtk_of_flowTblCnt_get(buf->openflow_cfg.unit, buf->openflow_cfg.phase, buf->openflow_cfg.flowtblCntType, &buf->openflow_cfg.tblCnt);
            rtk_copy_to(user, &buf->openflow_cfg, sizeof(rtdrv_openflowCfg_t));
            break;
#endif
        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 do_rtdrv_get_ctl_serdes(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* SerDes */
        case RTDRV_SERDES_REG_GET:
            rtk_copy_from(&buf->serdes_reg_cfg, user, sizeof(rtdrv_serdes_reg_t));
            ret = hal_serdes_reg_get(buf->serdes_reg_cfg.unit, buf->serdes_reg_cfg.sdsId, buf->serdes_reg_cfg.page, buf->serdes_reg_cfg.reg, &buf->serdes_reg_cfg.data);
            rtk_copy_to(user, &buf->serdes_reg_cfg, sizeof(rtdrv_serdes_reg_t));
            break;

        case RTDRV_SERDES_SYMERR_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_symErr_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.symErr);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        case RTDRV_SERDES_LINKSTS_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_linkSts_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.linkSts);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        case RTDRV_SDS_TESTMODECNT_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_testModeCnt_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.cnt);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        case RTDRV_SDS_LEQ_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_leq_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.cnt);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        case RTDRV_SDS_CMUBAND_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_cmuBand_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.cnt);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;
        case RTDRV_SDS_EYEPARAM_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_eyeParam_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.param);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;
        case RTDRV_SDS_RXCALICONF_GET:
            rtk_copy_from(&buf->sds_cfg, user, sizeof(rtdrv_sdsCfg_t));
            ret = rtk_sds_rxCaliConf_get(buf->sds_cfg.unit, buf->sds_cfg.sds, &buf->sds_cfg.conf);
            rtk_copy_to(user, &buf->sds_cfg, sizeof(rtdrv_sdsCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}


int32 do_rtdrv_get_ctl_hwp(int cmd, void *user, rtdrv_union_t *buf)
{
    int32 ret = RT_ERR_FAILED;

    switch(cmd)
    {
    /* HWP */
        case RTDRV_HWP_UNIT_GET_NEXT:
            rtk_copy_from(&buf->hwp_unit_cfg, user, sizeof(rtdrv_hwp_unitCfg_t));
            ret = hwp_unit_get_next(buf->hwp_unit_cfg.unit, &buf->hwp_unit_cfg.nextUnit);
            rtk_copy_to(user, &buf->hwp_unit_cfg, sizeof(rtdrv_hwp_unitCfg_t));
            break;

        default:
            ret = RT_ERR_FAILED;
            break;
    }
    return ret;
}

int32 rtdrv_modules_init(void)
{
    int i;

    for(i=0;i<RTDRV_MODULE_MAX_NUM;i++)
    {
        rtdrv_module_db_set[i].valid = FALSE;
        rtdrv_module_db_get[i].valid = FALSE;
    }

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_INIT_OFFSET, do_rtdrv_set_ctl_init);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_INIT_OFFSET, NULL);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_L2_OFFSET, do_rtdrv_set_ctl_l2);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_L2_OFFSET, do_rtdrv_get_ctl_l2);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_L2NTFY_OFFSET,do_rtdrv_set_ctl_l2ntfy);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_L2NTFY_OFFSET,do_rtdrv_get_ctl_l2ntfy);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_L3_OFFSET, do_rtdrv_set_ctl_l3);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_L3_OFFSET, do_rtdrv_get_ctl_l3);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_MCAST_OFFSET,do_rtdrv_set_ctl_mcast);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_MCAST_OFFSET,do_rtdrv_get_ctl_mcast);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_IPMC_OFFSET,do_rtdrv_set_ctl_ipmc);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_IPMC_OFFSET,do_rtdrv_get_ctl_ipmc);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_TUNNEL_OFFSET,do_rtdrv_set_ctl_tunnel);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_TUNNEL_OFFSET,do_rtdrv_get_ctl_tunnel);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_PORT_OFFSET,do_rtdrv_set_ctl_port);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_PORT_OFFSET,do_rtdrv_get_ctl_port);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_VLAN_OFFSET,do_rtdrv_set_ctl_vlan);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_VLAN_OFFSET,do_rtdrv_get_ctl_vlan);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_STP_OFFSET,do_rtdrv_set_ctl_stp);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_STP_OFFSET,do_rtdrv_get_ctl_stp);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_REG_OFFSET,do_rtdrv_set_ctl_reg);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_REG_OFFSET,do_rtdrv_get_ctl_reg);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_COUNTER_OFFSET,do_rtdrv_set_ctl_counter);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_COUNTER_OFFSET,do_rtdrv_get_ctl_counter);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_TIME_OFFSET,do_rtdrv_set_ctl_time);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_TIME_OFFSET,do_rtdrv_get_ctl_time);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_TRAP_OFFSET,do_rtdrv_set_ctl_trap);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_TRAP_OFFSET,do_rtdrv_get_ctl_trap);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_QOS_OFFSET,do_rtdrv_set_ctl_qos);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_QOS_OFFSET,do_rtdrv_get_ctl_qos);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_TRUNK_OFFSET,do_rtdrv_set_ctl_trunk);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_TRUNK_OFFSET,do_rtdrv_get_ctl_trunk);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_MIRROR_OFFSET,do_rtdrv_set_ctl_mirror);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_MIRROR_OFFSET,do_rtdrv_get_ctl_mirror);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_STACK_OFFSET,do_rtdrv_set_ctl_stack);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_STACK_OFFSET,do_rtdrv_get_ctl_stack);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_FLOWCTRL_OFFSET,do_rtdrv_set_ctl_flowctrl);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_FLOWCTRL_OFFSET,do_rtdrv_get_ctl_flowctrl);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_RATE_OFFSET,do_rtdrv_set_ctl_rate);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_RATE_OFFSET,do_rtdrv_get_ctl_rate);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SWITCH_OFFSET,do_rtdrv_set_ctl_switch);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SWITCH_OFFSET,do_rtdrv_get_ctl_switch);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SYS_OFFSET,do_rtdrv_set_ctl_sys);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SYS_OFFSET,do_rtdrv_get_ctl_sys);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_NIC_OFFSET,do_rtdrv_set_ctl_nic);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_NIC_OFFSET,do_rtdrv_get_ctl_nic);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_OAM_OFFSET,do_rtdrv_set_ctl_oam);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_OAM_OFFSET,do_rtdrv_get_ctl_oam);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_EEE_OFFSET,do_rtdrv_set_ctl_eee);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_EEE_OFFSET,do_rtdrv_get_ctl_eee);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SEC_OFFSET,do_rtdrv_set_ctl_sec);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SEC_OFFSET,do_rtdrv_get_ctl_sec);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_ACL_OFFSET,do_rtdrv_set_ctl_acl);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_ACL_OFFSET,do_rtdrv_get_ctl_acl);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_PIE_OFFSET,do_rtdrv_set_ctl_pie);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_PIE_OFFSET,do_rtdrv_get_ctl_pie);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_MPLS_OFFSET,do_rtdrv_set_ctl_mpls);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_MPLS_OFFSET,do_rtdrv_get_ctl_mpls);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_DIAG_OFFSET,do_rtdrv_set_ctl_diag);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_DIAG_OFFSET,do_rtdrv_get_ctl_diag);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_DEBUG_OFFSET,do_rtdrv_set_ctl_debug);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_DEBUG_OFFSET,do_rtdrv_get_ctl_debug);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_UART_OFFSET,do_rtdrv_set_ctl_uart);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_UART_OFFSET,do_rtdrv_get_ctl_uart);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_LED_OFFSET,do_rtdrv_set_ctl_led);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_LED_OFFSET,do_rtdrv_get_ctl_led);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_RTL8231_OFFSET,do_rtdrv_set_ctl_rtl8231);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_RTL8231_OFFSET,do_rtdrv_get_ctl_rtl8231);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_EXTGPIO_OFFSET,do_rtdrv_set_ctl_extgpio);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_EXTGPIO_OFFSET,NULL);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_GPIO_OFFSET,do_rtdrv_set_ctl_gpio);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_GPIO_OFFSET,do_rtdrv_get_ctl_gpio);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SPI_OFFSET,do_rtdrv_set_ctl_spi);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SPI_OFFSET,do_rtdrv_get_ctl_spi);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_VXLAN_OFFSET,do_rtdrv_set_ctl_vxlan);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_VXLAN_OFFSET,do_rtdrv_get_ctl_vxlan);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_OPENFLOW_OFFSET,do_rtdrv_set_ctl_openflow);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_OPENFLOW_OFFSET,do_rtdrv_get_ctl_openflow);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_I2C_OFFSET,do_rtdrv_set_ctl_i2c);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_I2C_OFFSET,do_rtdrv_get_ctl_i2c);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_HWP_OFFSET,NULL);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_HWP_OFFSET,do_rtdrv_get_ctl_hwp);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SDK_OFFSET,do_rtdrv_set_ctl_sdk);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SDK_OFFSET,do_rtdrv_get_ctl_sdk);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_BPE_OFFSET,do_rtdrv_set_ctl_bpe);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_BPE_OFFSET,do_rtdrv_get_ctl_bpe);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SERDES_OFFSET,do_rtdrv_set_ctl_serdes);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SERDES_OFFSET,do_rtdrv_get_ctl_serdes);

    rtdrv_cmdFunc_register(RTDRV_SET, RTDRV_SC_OFFSET,do_rtdrv_set_ctl_sc);
    rtdrv_cmdFunc_register(RTDRV_GET, RTDRV_SC_OFFSET,do_rtdrv_get_ctl_sc);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtdrv_init
 * Description:
 *      Init driver and register netfilter socket option
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 __init rtdrv_init(void)
{
    int32 ret=RT_ERR_FAILED;

    RT_INIT_MSG("RTDRV Driver Module Initialize\n");

    /* register netfilter socket option */
    if (nf_register_sockopt(&rtdrv_sockopts))
    {
        RT_INIT_ERR(ret, (MOD_INIT), "nf_register_sockopt failed.\n");
        return ret;
    }



#if defined(CONFIG_SDK_APP_DIAG_EXT)

    RT_INIT_MSG("RTDRV EXT Driver Module Initialize\n");
    /* register netfilter socket option */
    if (nf_register_sockopt(&rtdrv_ext_sockopts))
    {
        RT_INIT_ERR(ret, (MOD_INIT), "nf_register_sockopt failed.\n");
        return ret;
    }

#endif /* CONFIG_SDK_APP_DIAG_EXT */

    rtdrv_modules_init();

    return RT_ERR_OK;
}

/* Function Name:
 *      rtdrv_exit
 * Description:
 *      Exit driver and unregister netfilter socket option
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void __exit rtdrv_exit(void)
{
    nf_unregister_sockopt(&rtdrv_sockopts);

    RT_INIT_MSG("Exit RTDRV Driver Module\n");

#if defined(CONFIG_SDK_APP_DIAG_EXT)
    nf_unregister_sockopt(&rtdrv_ext_sockopts);
    RT_INIT_MSG("Exit RTDRV EXT Driver Module\n");
#endif /* CONFIG_SDK_APP_DIAG_EXT */
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
struct nf_sockopt_ops rtdrv_sockopts = {
    { NULL, NULL }, PF_INET,
    RTDRV_BASE_CTL, RTDRV_SET_MAX+1, do_rtdrv_set_ctl, NULL,
    RTDRV_BASE_CTL, RTDRV_GET_MAX+1, do_rtdrv_get_ctl, NULL
};
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
struct nf_sockopt_ops rtdrv_sockopts = {
    { NULL, NULL }, PF_INET,
    RTDRV_BASE_CTL, RTDRV_SET_MAX+1, do_rtdrv_set_ctl,
    RTDRV_BASE_CTL, RTDRV_GET_MAX+1, do_rtdrv_get_ctl
};
#endif


module_init(rtdrv_init);
module_exit(rtdrv_exit);

MODULE_DESCRIPTION ("Switch SDK User/Kernel Driver Module");
MODULE_LICENSE("GPL");

