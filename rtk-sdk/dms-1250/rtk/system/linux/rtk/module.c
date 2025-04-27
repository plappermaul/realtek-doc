/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Export the public APIs in lower layer module in the SDK.
 *
 * Feature : Export the public APIs in lower layer module
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/util/rt_util_serdes.h>
#include <common/debug/rt_log.h>
#include <osal/print.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/mac_debug.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/chip.h>
#include <hal/mac/drv/drv.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/driver.h>
#include <dal/dal_phy.h>
#include <dal/dal_linkMon.h>
#include <dal/mango/dal_mango_l2.h>
#include <rtk/init.h>
#include <rtk/eee.h>
#include <rtk/flowctrl.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/tunnel.h>
#include <rtk/mirror.h>
#include <rtk/acl.h>
#include <rtk/pie.h>
#include <rtk/port.h>
#include <rtk/qos.h>
#include <rtk/rate.h>
#include <rtk/sec.h>
#include <rtk/stat.h>
#include <rtk/stp.h>
#include <rtk/switch.h>
#include <rtk/time.h>
#include <rtk/trap.h>
#include <rtk/trunk.h>
#include <rtk/vlan.h>
#include <rtk/bpe.h>
#include <rtk/led.h>
#include <rtk/diag.h>
#include <rtk/sds.h>

#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310))
#include <rtk/oam.h>
#include <rtk/mpls.h>
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)) */

#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
#include <rtk/mcast.h>
#include <rtk/ipmcast.h>
#include <rtk/stack.h>
#endif

#if defined(CONFIG_SDK_RTL9310)
#include <rtk/openflow.h>
#endif

#if defined(CONFIG_SDK_DRIVER_TEST_MODULE) || defined(CONFIG_SDK_DRIVER_TEST)
#include <hal/common/miim.h>
#include <hal/phy/identify.h>
#include <hal/mac/mem.h>
#endif
#include <hal/mac/serdes.h>

#include <hwp/hw_profile.h>
#include <common/util/rt_util_led.h>


#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
  #include <hal/mac/rtl8295.h>
  #include <hal/phy/phy_rtl8295_patch.h>
  #include <hal/phy/phy_rtl8295.h>
#endif /* defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) */

#if defined(CONFIG_SDK_RTL8390)
  #include <hal/phy/phy_rtl8390.h>
  #include <hal/common/miim.h>
#endif /* defined(CONFIG_SDK_RTL8390) */

#include <hal/phy/phy_common.h>
#include <dal/rtrpc/rtrpc_msg.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
static int __init rtk_sdk_init(void)
{
    int32 ret;

    if (RT_ERR_OK != (ret = rtk_init()))
    {
        RT_INIT_ERR(ret, (MOD_INIT), "RTK Driver Module init FAIL!\n");
        return -1;
    }

    return 0;
}

static void __exit rtk_sdk_exit(void)
{
    RT_INIT_MSG("Exit RTK Driver Module....OK\n");
}

module_init(rtk_sdk_init);
module_exit(rtk_sdk_exit);

MODULE_DESCRIPTION("Switch SDK RTK Driver Module");
MODULE_LICENSE("GPL");

/* RTK functions */
EXPORT_SYMBOL(rtk_init);
EXPORT_SYMBOL(rtk_unit_attach);
EXPORT_SYMBOL(rtk_unit_detach);

EXPORT_SYMBOL(rtk_flowctrl_init);
EXPORT_SYMBOL(rtk_flowctrl_portPauseOnAction_get);
EXPORT_SYMBOL(rtk_flowctrl_portPauseOnAction_set);
EXPORT_SYMBOL(rtk_flowctrl_portPauseOnAllowedPageNum_get);
EXPORT_SYMBOL(rtk_flowctrl_portPauseOnAllowedPageNum_set);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPktLen_get);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPktLen_set);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPktNum_get);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPktNum_set);
EXPORT_SYMBOL(rtk_flowctrl_igrGuarEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_igrGuarEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemPauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemPauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_igrJumboSystemPauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrJumboSystemPauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_igrPauseThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_igrPauseThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_portIgrPortThreshGroupSel_get);
EXPORT_SYMBOL(rtk_flowctrl_portIgrPortThreshGroupSel_set);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemCongestThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemCongestThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_igrJumboSystemCongestThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrJumboSystemCongestThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_igrCongestThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_igrCongestThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_jumboModeStatus_get);
EXPORT_SYMBOL(rtk_flowctrl_jumboModeEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_jumboModeEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_jumboModeLength_get);
EXPORT_SYMBOL(rtk_flowctrl_jumboModeLength_set);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemUtilThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemUtilThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortQueueDropEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortQueueDropEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrCpuQueueDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrCpuQueueDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_portEgrDropRefCongestEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_portEgrDropRefCongestEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_portEgrDropThreshGroupSel_get);
EXPORT_SYMBOL(rtk_flowctrl_portEgrDropThreshGroupSel_set);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_portEgrQueueDropForceEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_portEgrQueueDropForceEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_igrQueueDropEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_igrQueueDropEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_igrQueuePauseDropThreshGroupSel_get);
EXPORT_SYMBOL(rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set);
EXPORT_SYMBOL(rtk_flowctrl_igrQueueDropThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_igrQueueDropThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_igrQueuePauseThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_igrQueuePauseThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_igrQueueDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrQueueDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_portHolTrafficDropEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_portHolTrafficDropEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_holTrafficTypeDropEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_holTrafficTypeDropEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_specialCongestThreshold_get);
EXPORT_SYMBOL(rtk_flowctrl_specialCongestThreshold_set);
EXPORT_SYMBOL(rtk_flowctrl_e2eCascadePortThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_e2eCascadePortThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_e2eRemotePortPauseThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_e2eRemotePortPauseThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_e2eRemotePortCongestThreshGroup_get);
EXPORT_SYMBOL(rtk_flowctrl_e2eRemotePortCongestThreshGroup_set);
EXPORT_SYMBOL(rtk_flowctrl_portE2eRemotePortThreshGroupSel_get);
EXPORT_SYMBOL(rtk_flowctrl_portE2eRemotePortThreshGroupSel_set);
EXPORT_SYMBOL(rtk_flowctrl_tagPauseEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_tagPauseEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_halfConsecutiveRetryEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_halfConsecutiveRetryEnable_set);

EXPORT_SYMBOL(rtk_l2_init);
EXPORT_SYMBOL(rtk_l2_flushLinkDownPortAddrEnable_get);
EXPORT_SYMBOL(rtk_l2_flushLinkDownPortAddrEnable_set);
EXPORT_SYMBOL(rtk_l2_ucastAddr_flush);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_learningCnt_get);
EXPORT_SYMBOL(rtk_l2_portLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_fidLearningCnt_get);
#endif
EXPORT_SYMBOL(rtk_l2_macLearningCnt_get);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_limitLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCnt_set);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCnt_set);
#endif
EXPORT_SYMBOL(rtk_l2_limitLearningNum_get);
EXPORT_SYMBOL(rtk_l2_limitLearningNum_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_limitLearningCntAction_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCntAction_set);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCntAction_get);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCntAction_set);
EXPORT_SYMBOL(rtk_l2_fidLearningCntAction_get);
EXPORT_SYMBOL(rtk_l2_fidLearningCntAction_set);
#endif
EXPORT_SYMBOL(rtk_l2_limitLearningAction_get);
EXPORT_SYMBOL(rtk_l2_limitLearningAction_set);
EXPORT_SYMBOL(rtk_l2_fidLimitLearningEntry_get);
EXPORT_SYMBOL(rtk_l2_fidLimitLearningEntry_set);
EXPORT_SYMBOL(rtk_l2_fidLearningCnt_reset);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_aging_get);
EXPORT_SYMBOL(rtk_l2_aging_set);
#endif
EXPORT_SYMBOL(rtk_l2_agingTime_get);
EXPORT_SYMBOL(rtk_l2_agingTime_set);
EXPORT_SYMBOL(rtk_l2_portAgingEnable_get);
EXPORT_SYMBOL(rtk_l2_portAgingEnable_set);
EXPORT_SYMBOL(rtk_l2_trkAgingEnable_get);
EXPORT_SYMBOL(rtk_l2_trkAgingEnable_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_hashAlgo_get);
EXPORT_SYMBOL(rtk_l2_hashAlgo_set);
#endif
EXPORT_SYMBOL(rtk_l2_bucketHashAlgo_get);
EXPORT_SYMBOL(rtk_l2_bucketHashAlgo_set);
EXPORT_SYMBOL(rtk_l2_vlanMode_get);
EXPORT_SYMBOL(rtk_l2_vlanMode_set);
EXPORT_SYMBOL(rtk_l2_learningFullAction_get);
EXPORT_SYMBOL(rtk_l2_learningFullAction_set);
EXPORT_SYMBOL(rtk_l2_portNewMacOp_get);
EXPORT_SYMBOL(rtk_l2_portNewMacOp_set);
EXPORT_SYMBOL(rtk_l2_addr_init);
EXPORT_SYMBOL(rtk_l2_addr_add);
EXPORT_SYMBOL(rtk_l2_addr_del);
EXPORT_SYMBOL(rtk_l2_addr_get);
EXPORT_SYMBOL(rtk_l2_addr_set);
EXPORT_SYMBOL(rtk_l2_addr_delAll);
EXPORT_SYMBOL(rtk_l2_nextValidAddr_get);
EXPORT_SYMBOL(rtk_l2_mcastAddr_init);
EXPORT_SYMBOL(rtk_l2_mcastAddr_add);
EXPORT_SYMBOL(rtk_l2_mcastAddr_del);
EXPORT_SYMBOL(rtk_l2_mcastAddr_get);
EXPORT_SYMBOL(rtk_l2_mcastAddr_set);
EXPORT_SYMBOL(rtk_l2_mcastAddr_addByIndex);
EXPORT_SYMBOL(rtk_l2_mcastAddr_setByIndex);
EXPORT_SYMBOL(rtk_l2_mcastAddr_delIgnoreIndex);
EXPORT_SYMBOL(rtk_l2_nextValidMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_ipmcMode_get);
EXPORT_SYMBOL(rtk_l2_ipmcMode_set);
EXPORT_SYMBOL(rtk_l2_ipMcastAddrExt_init);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_add);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_del);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_set);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_addByIndex);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_setByIndex);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_delIgnoreIndex);
EXPORT_SYMBOL(rtk_l2_nextValidIpMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_ipMcastAddrChkEnable_get);
EXPORT_SYMBOL(rtk_l2_ipMcastAddrChkEnable_set);
EXPORT_SYMBOL(rtk_l2_ipMcstFidVidCompareEnable_get);
EXPORT_SYMBOL(rtk_l2_ipMcstFidVidCompareEnable_set);
EXPORT_SYMBOL(rtk_l2_ip6mcMode_get);
EXPORT_SYMBOL(rtk_l2_ip6mcMode_set);
EXPORT_SYMBOL(rtk_l2_ip6CareByte_get);
EXPORT_SYMBOL(rtk_l2_ip6CareByte_set);
EXPORT_SYMBOL(rtk_l2_ip6McastAddrExt_init);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_add);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_del);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_get);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_set);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_addByIndex);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_setByIndex);
EXPORT_SYMBOL(rtk_l2_ip6McastAddr_delIgnoreIndex);
EXPORT_SYMBOL(rtk_l2_nextValidIp6McastAddr_get);
EXPORT_SYMBOL(rtk_l2_mcastFwdIndex_alloc);
EXPORT_SYMBOL(rtk_l2_mcastFwdIndex_free);
EXPORT_SYMBOL(rtk_l2_mcastFwdIndexFreeCount_get);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_mcastFwdPortmask_set);
EXPORT_SYMBOL(rtk_l2_mcastFwdPortmask_get);
#endif
EXPORT_SYMBOL(rtk_l2_mcastFwdPortmaskEntry_get);
EXPORT_SYMBOL(rtk_l2_mcastFwdPortmaskEntry_set);
EXPORT_SYMBOL(rtk_l2_cpuMacAddr_add);
EXPORT_SYMBOL(rtk_l2_cpuMacAddr_del);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_l2_legalPortMoveAction_get);
EXPORT_SYMBOL(rtk_l2_legalPortMoveAction_set);
EXPORT_SYMBOL(rtk_l2_dynamicPortMoveForbidAction_get);
EXPORT_SYMBOL(rtk_l2_dynamicPortMoveForbidAction_set);
#endif
EXPORT_SYMBOL(rtk_l2_portMoveAction_get);
EXPORT_SYMBOL(rtk_l2_portMoveAction_set);
EXPORT_SYMBOL(rtk_l2_portMoveLearn_get);
EXPORT_SYMBOL(rtk_l2_portMoveLearn_set);
EXPORT_SYMBOL(rtk_l2_legalPortMoveFlushAddrEnable_get);
EXPORT_SYMBOL(rtk_l2_legalPortMoveFlushAddrEnable_set);
EXPORT_SYMBOL(rtk_l2_staticPortMoveAction_get);
EXPORT_SYMBOL(rtk_l2_staticPortMoveAction_set);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_get);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_set);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_add);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_del);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_setByIndex);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMaskIdx_get);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMaskIdx_set);
EXPORT_SYMBOL(rtk_l2_portLookupMissAction_get);
EXPORT_SYMBOL(rtk_l2_portLookupMissAction_set);
EXPORT_SYMBOL(rtk_l2_portUcastLookupMissAction_get);
EXPORT_SYMBOL(rtk_l2_portUcastLookupMissAction_set);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_get);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_set);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_add);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_del);
EXPORT_SYMBOL(rtk_l2_exceptionAddrAction_get);
EXPORT_SYMBOL(rtk_l2_exceptionAddrAction_set);
EXPORT_SYMBOL(rtk_l2_addrEntry_get);
EXPORT_SYMBOL(rtk_l2_conflictAddr_get);
EXPORT_SYMBOL(rtk_l2_zeroSALearningEnable_get);
EXPORT_SYMBOL(rtk_l2_zeroSALearningEnable_set);
EXPORT_SYMBOL(rtk_l2_secureMacMode_get);
EXPORT_SYMBOL(rtk_l2_secureMacMode_set);
EXPORT_SYMBOL(rtk_l2_portDynamicPortMoveForbidEnable_get);
EXPORT_SYMBOL(rtk_l2_portDynamicPortMoveForbidEnable_set);
EXPORT_SYMBOL(rtk_l2_trkDynamicPortMoveForbidEnable_get);
EXPORT_SYMBOL(rtk_l2_trkDynamicPortMoveForbidEnable_set);
EXPORT_SYMBOL(rtk_l2_portMacFilterEnable_get);
EXPORT_SYMBOL(rtk_l2_portMacFilterEnable_set);
EXPORT_SYMBOL(rtk_l2_hwNextValidAddr_get);
EXPORT_SYMBOL(rtk_l2_portCtrl_get);
EXPORT_SYMBOL(rtk_l2_portCtrl_set);
EXPORT_SYMBOL(rtk_l2_status_get);
EXPORT_SYMBOL(rtk_l2_stkLearningEnable_get);
EXPORT_SYMBOL(rtk_l2_stkLearningEnable_set);
EXPORT_SYMBOL(rtk_l2_stkKeepUcastEntryValid_get);
EXPORT_SYMBOL(rtk_l2_stkKeepUcastEntryValid_set);
EXPORT_SYMBOL(rtk_l2_entryCnt_get);
EXPORT_SYMBOL(rtk_l2_hashIdx_get);
EXPORT_SYMBOL(rtk_l2_addr_delByMac);

#ifdef CONFIG_SDK_RTL9310
EXPORT_SYMBOL(_dal_mango_l2_nexthop_add);
EXPORT_SYMBOL(_dal_mango_l2_nexthop_del);
EXPORT_SYMBOL(_dal_mango_l2_mcastNexthop_add);
EXPORT_SYMBOL(_dal_mango_l2_mcastNexthop_del);
#endif
EXPORT_SYMBOL(rtk_mirror_init);
EXPORT_SYMBOL(rtk_mirror_group_init);
EXPORT_SYMBOL(rtk_mirror_group_get);
EXPORT_SYMBOL(rtk_mirror_group_set);
EXPORT_SYMBOL(rtk_mirror_rspanIgrMode_get);
EXPORT_SYMBOL(rtk_mirror_rspanIgrMode_set);
EXPORT_SYMBOL(rtk_mirror_rspanEgrMode_get);
EXPORT_SYMBOL(rtk_mirror_rspanEgrMode_set);
EXPORT_SYMBOL(rtk_mirror_rspanTag_get);
EXPORT_SYMBOL(rtk_mirror_rspanTag_set);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleRate_get);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleRate_set);
EXPORT_SYMBOL(rtk_mirror_egrQueue_get);
EXPORT_SYMBOL(rtk_mirror_egrQueue_set);
EXPORT_SYMBOL(rtk_mirror_sflowPortIgrSampleRate_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortIgrSampleRate_set);
EXPORT_SYMBOL(rtk_mirror_sflowPortEgrSampleRate_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortEgrSampleRate_set);
EXPORT_SYMBOL(rtk_mirror_sflowSampleCtrl_get);
EXPORT_SYMBOL(rtk_mirror_sflowSampleCtrl_set);
EXPORT_SYMBOL(rtk_mirror_sflowSampleTarget_get);
EXPORT_SYMBOL(rtk_mirror_sflowSampleTarget_set);

EXPORT_SYMBOL(rtk_port_init);
EXPORT_SYMBOL(rtk_port_link_get);
EXPORT_SYMBOL(rtk_port_adminEnable_get);
EXPORT_SYMBOL(rtk_port_adminEnable_set);
EXPORT_SYMBOL(rtk_port_speedDuplex_get);
EXPORT_SYMBOL(rtk_port_flowctrl_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoEnable_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoEnable_set);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbilityLocal_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbility_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbility_set);
EXPORT_SYMBOL(rtk_port_phyForceModeAbility_get);
EXPORT_SYMBOL(rtk_port_phyForceModeAbility_set);
EXPORT_SYMBOL(rtk_port_phyForceFlowctrlMode_get);
EXPORT_SYMBOL(rtk_port_phyForceFlowctrlMode_set);
EXPORT_SYMBOL(rtk_port_phyMasterSlave_get);
EXPORT_SYMBOL(rtk_port_phyMasterSlave_set);
EXPORT_SYMBOL(rtk_port_phyReg_get);
EXPORT_SYMBOL(rtk_port_phyReg_set);
EXPORT_SYMBOL(rtk_port_phyExtParkPageReg_get);
EXPORT_SYMBOL(rtk_port_phyExtParkPageReg_set);
EXPORT_SYMBOL(rtk_port_phymaskExtParkPageReg_set);
EXPORT_SYMBOL(rtk_port_phyMmdReg_get);
EXPORT_SYMBOL(rtk_port_phyMmdReg_set);
EXPORT_SYMBOL(rtk_port_phymaskMmdReg_set);
EXPORT_SYMBOL(rtk_port_cpuPortId_get);
EXPORT_SYMBOL(rtk_port_isolation_get);
EXPORT_SYMBOL(rtk_port_isolation_set);
EXPORT_SYMBOL(rtk_port_isolationExt_get);
EXPORT_SYMBOL(rtk_port_isolationExt_set);
EXPORT_SYMBOL(rtk_port_isolation_add);
EXPORT_SYMBOL(rtk_port_isolation_del);
EXPORT_SYMBOL(rtk_port_isolationRestrictRoute_get);
EXPORT_SYMBOL(rtk_port_isolationRestrictRoute_set);
EXPORT_SYMBOL(rtk_port_phyComboPortMedia_get);
EXPORT_SYMBOL(rtk_port_phyComboPortMedia_set);
EXPORT_SYMBOL(rtk_port_backpressureEnable_get);
EXPORT_SYMBOL(rtk_port_backpressureEnable_set);
EXPORT_SYMBOL(rtk_port_greenEnable_get);
EXPORT_SYMBOL(rtk_port_greenEnable_set);
EXPORT_SYMBOL(rtk_port_gigaLiteEnable_get);
EXPORT_SYMBOL(rtk_port_gigaLiteEnable_set);
EXPORT_SYMBOL(rtk_port_2pt5gLiteEnable_get);
EXPORT_SYMBOL(rtk_port_2pt5gLiteEnable_set);
EXPORT_SYMBOL(rtk_port_txEnable_get);
EXPORT_SYMBOL(rtk_port_txEnable_set);
EXPORT_SYMBOL(rtk_port_rxEnable_get);
EXPORT_SYMBOL(rtk_port_rxEnable_set);
EXPORT_SYMBOL(rtk_port_specialCongest_get);
EXPORT_SYMBOL(rtk_port_specialCongest_set);
EXPORT_SYMBOL(rtk_port_linkMon_enable);
EXPORT_SYMBOL(rtk_port_linkMon_disable);
EXPORT_SYMBOL(dal_linkMonPolling_stop);
EXPORT_SYMBOL(rtk_port_linkMon_register);
EXPORT_SYMBOL(rtk_port_linkMon_unregister);
EXPORT_SYMBOL(rtk_port_linkMon_swScanPorts_set);
EXPORT_SYMBOL(rtk_port_linkMon_swScanPorts_get);
EXPORT_SYMBOL(rtk_port_phyCrossOverMode_get);
EXPORT_SYMBOL(rtk_port_phyCrossOverMode_set);
EXPORT_SYMBOL(rtk_port_phyCrossOverStatus_get);
EXPORT_SYMBOL(rtk_port_flowCtrlEnable_get);
EXPORT_SYMBOL(rtk_port_flowCtrlEnable_set);
EXPORT_SYMBOL(rtk_port_phyComboPortFiberMedia_get);
EXPORT_SYMBOL(rtk_port_phyComboPortFiberMedia_set);
EXPORT_SYMBOL(rtk_port_linkMedia_get);
EXPORT_SYMBOL(rtk_port_linkDownPowerSavingEnable_get);
EXPORT_SYMBOL(rtk_port_linkDownPowerSavingEnable_set);
EXPORT_SYMBOL(rtk_port_vlanBasedIsolationEntry_get);
EXPORT_SYMBOL(rtk_port_vlanBasedIsolationEntry_set);
EXPORT_SYMBOL(rtk_port_vlanBasedIsolation_vlanSource_get);
EXPORT_SYMBOL(rtk_port_vlanBasedIsolation_vlanSource_set);
EXPORT_SYMBOL(rtk_port_vlanBasedIsolationEgrBypass_get);
EXPORT_SYMBOL(rtk_port_vlanBasedIsolationEgrBypass_set);
EXPORT_SYMBOL(rtk_port_phyReconfig_register);
EXPORT_SYMBOL(rtk_port_phyReconfig_unregister);
EXPORT_SYMBOL(rtk_port_downSpeedEnable_get);
EXPORT_SYMBOL(rtk_port_downSpeedEnable_set);
EXPORT_SYMBOL(rtk_port_downSpeedStatus_get);
EXPORT_SYMBOL(rtk_port_fiberDownSpeedEnable_get);
EXPORT_SYMBOL(rtk_port_fiberDownSpeedEnable_set);
EXPORT_SYMBOL(rtk_port_fiberNwayForceLinkEnable_get);
EXPORT_SYMBOL(rtk_port_fiberNwayForceLinkEnable_set);
EXPORT_SYMBOL(rtk_port_fiberUnidirEnable_get);
EXPORT_SYMBOL(rtk_port_fiberUnidirEnable_set);
EXPORT_SYMBOL(rtk_port_fiberOAMLoopBackEnable_set);
EXPORT_SYMBOL(rtk_port_10gMedia_set);
EXPORT_SYMBOL(rtk_port_10gMedia_get);
EXPORT_SYMBOL(rtk_port_phyLoopBackEnable_get);
EXPORT_SYMBOL(rtk_port_phyLoopBackEnable_set);
EXPORT_SYMBOL(rtk_port_phyFiberTxDis_set);
EXPORT_SYMBOL(rtk_port_phyFiberTxDisPin_set);
EXPORT_SYMBOL(rtk_port_fiberRxEnable_get);
EXPORT_SYMBOL(rtk_port_fiberRxEnable_set);
EXPORT_SYMBOL(rtk_port_phyIeeeTestMode_set);
EXPORT_SYMBOL(rtk_port_phyPolar_get);
EXPORT_SYMBOL(rtk_port_phyPolar_set);
EXPORT_SYMBOL(rtk_port_phyEyeMonitor_start);
EXPORT_SYMBOL(rtk_port_phyEyeMonitorInfo_get);
EXPORT_SYMBOL(rtk_port_imageFlash_load);
EXPORT_SYMBOL(rtk_port_phySds_get);
EXPORT_SYMBOL(rtk_port_phySds_set);
EXPORT_SYMBOL(rtk_port_phySdsRxCaliStatus_get);
EXPORT_SYMBOL(rtk_port_phyReset_set);
EXPORT_SYMBOL(rtk_port_phyLinkStatus_get);
EXPORT_SYMBOL(rtk_port_phyPeerAutoNegoAbility_get);
EXPORT_SYMBOL(rtk_port_phyMacIntfSerdesMode_get);
EXPORT_SYMBOL(rtk_port_phyLedMode_set);
EXPORT_SYMBOL(rtk_port_phyLedCtrl_get);
EXPORT_SYMBOL(rtk_port_phyLedCtrl_set);
EXPORT_SYMBOL(rtk_port_phyMacIntfSerdesLinkStatus_get);
EXPORT_SYMBOL(rtk_port_phySdsEyeParam_get);
EXPORT_SYMBOL(rtk_port_phySdsEyeParam_set);
EXPORT_SYMBOL(rtk_port_phyMdiLoopbackEnable_get);
EXPORT_SYMBOL(rtk_port_phyMdiLoopbackEnable_set);
EXPORT_SYMBOL(rtk_port_phyIntr_init);
EXPORT_SYMBOL(rtk_port_phyIntrEnable_get);
EXPORT_SYMBOL(rtk_port_phyIntrEnable_set);
EXPORT_SYMBOL(rtk_port_phySdsLeq_get);
EXPORT_SYMBOL(rtk_port_phySdsLeq_set);
EXPORT_SYMBOL(rtk_port_phyIntrStatus_get);
EXPORT_SYMBOL(rtk_port_phyIntrMask_get);
EXPORT_SYMBOL(rtk_port_phyIntrMask_set);
EXPORT_SYMBOL(rtk_port_phySdsTestMode_set);
EXPORT_SYMBOL(rtk_port_phySdsTestModeCnt_get);
EXPORT_SYMBOL(rtk_port_phyDbgCounter_get);
EXPORT_SYMBOL(rtk_phy_debug_get);
EXPORT_SYMBOL(rtk_phy_debug_batch_port_set);
EXPORT_SYMBOL(rtk_phy_debug_batch_op_set);
EXPORT_SYMBOL(rtk_port_miscCtrl_get);
EXPORT_SYMBOL(rtk_port_miscCtrl_set);
EXPORT_SYMBOL(rtk_port_phySdsReg_get);
EXPORT_SYMBOL(rtk_port_phySdsReg_set);


EXPORT_SYMBOL(rtk_qos_init);
EXPORT_SYMBOL(rtk_qos_dpSrcSel_get);
EXPORT_SYMBOL(rtk_qos_dpSrcSel_set);
EXPORT_SYMBOL(rtk_qos_priSelGroup_get);
EXPORT_SYMBOL(rtk_qos_priSelGroup_set);
EXPORT_SYMBOL(rtk_qos_portPriSelGroup_get);
EXPORT_SYMBOL(rtk_qos_portPriSelGroup_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_portPri_get);
EXPORT_SYMBOL(rtk_qos_portPri_set);
EXPORT_SYMBOL(rtk_qos_portInnerPri_get);
EXPORT_SYMBOL(rtk_qos_portInnerPri_set);
EXPORT_SYMBOL(rtk_qos_portOuterPri_get);
EXPORT_SYMBOL(rtk_qos_portOuterPri_set);
#endif
EXPORT_SYMBOL(rtk_qos_deiDpRemap_get);
EXPORT_SYMBOL(rtk_qos_deiDpRemap_set);
EXPORT_SYMBOL(rtk_qos_dpRemap_get);
EXPORT_SYMBOL(rtk_qos_dpRemap_set);
EXPORT_SYMBOL(rtk_qos_portDEISrcSel_get);
EXPORT_SYMBOL(rtk_qos_portDEISrcSel_set);
EXPORT_SYMBOL(rtk_qos_portDpSel_get);
EXPORT_SYMBOL(rtk_qos_portDpSel_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_dscpDpRemap_get);
EXPORT_SYMBOL(rtk_qos_dscpDpRemap_set);
#endif
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_dscpPriRemap_get);
EXPORT_SYMBOL(rtk_qos_dscpPriRemap_set);
EXPORT_SYMBOL(rtk_qos_1pPriRemap_get);
EXPORT_SYMBOL(rtk_qos_1pPriRemap_set);
EXPORT_SYMBOL(rtk_qos_outer1pPriRemap_get);
EXPORT_SYMBOL(rtk_qos_outer1pPriRemap_set);
#endif
EXPORT_SYMBOL(rtk_qos_priRemap_get);
EXPORT_SYMBOL(rtk_qos_priRemap_set);
EXPORT_SYMBOL(rtk_qos_queueNum_get);
EXPORT_SYMBOL(rtk_qos_queueNum_set);
EXPORT_SYMBOL(rtk_qos_priMap_get);
EXPORT_SYMBOL(rtk_qos_priMap_set);
EXPORT_SYMBOL(rtk_qos_pri2QidMap_get);
EXPORT_SYMBOL(rtk_qos_pri2QidMap_set);
EXPORT_SYMBOL(rtk_qos_cpuQid2QidMap_get);
EXPORT_SYMBOL(rtk_qos_cpuQid2QidMap_set);
EXPORT_SYMBOL(rtk_qos_cpuQid2StackQidMap_get);
EXPORT_SYMBOL(rtk_qos_cpuQid2StackQidMap_set);
EXPORT_SYMBOL(rtk_qos_port1pRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_port1pRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_1pRemarking_get);
EXPORT_SYMBOL(rtk_qos_1pRemarking_set);
EXPORT_SYMBOL(rtk_qos_port1pDfltPri_get);
EXPORT_SYMBOL(rtk_qos_port1pDfltPri_set);
EXPORT_SYMBOL(rtk_qos_port1pDfltPriExt_get);
EXPORT_SYMBOL(rtk_qos_port1pDfltPriExt_set);
EXPORT_SYMBOL(rtk_qos_port1pDfltPriSrcSel_get);
EXPORT_SYMBOL(rtk_qos_port1pDfltPriSrcSel_set);
EXPORT_SYMBOL(rtk_qos_port1pDfltPriSrcSelExt_get);
EXPORT_SYMBOL(rtk_qos_port1pDfltPriSrcSelExt_set);
EXPORT_SYMBOL(rtk_qos_1pDfltPriCfgSrcSel_get);
EXPORT_SYMBOL(rtk_qos_1pDfltPriCfgSrcSel_set);
EXPORT_SYMBOL(rtk_qos_portOut1pRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_portOut1pRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_outer1pRemarking_get);
EXPORT_SYMBOL(rtk_qos_outer1pRemarking_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPri_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPri_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPriExt_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPriExt_set);
EXPORT_SYMBOL(rtk_qos_portDscpRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_portDscpRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_dscpRemarking_get);
EXPORT_SYMBOL(rtk_qos_dscpRemarking_set);
EXPORT_SYMBOL(rtk_qos_portDeiRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_portDeiRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_deiRemarking_get);
EXPORT_SYMBOL(rtk_qos_deiRemarking_set);
EXPORT_SYMBOL(rtk_qos_deiRemarkSrcSel_get);
EXPORT_SYMBOL(rtk_qos_deiRemarkSrcSel_set);
EXPORT_SYMBOL(rtk_qos_1pDfltPri_set);
EXPORT_SYMBOL(rtk_qos_1pDfltPri_get);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_1pRemark_get);
EXPORT_SYMBOL(rtk_qos_1pRemark_set);
EXPORT_SYMBOL(rtk_qos_outer1pRemark_get);
EXPORT_SYMBOL(rtk_qos_outer1pRemark_set);
#endif
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPriSrcSel_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPriSrcSel_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPriSrcSelExt_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pDfltPriSrcSelExt_set);
EXPORT_SYMBOL(rtk_qos_1pDfltPriSrcSel_get);
EXPORT_SYMBOL(rtk_qos_1pDfltPriSrcSel_set);
EXPORT_SYMBOL(rtk_qos_outer1pDfltPri_get);
EXPORT_SYMBOL(rtk_qos_outer1pDfltPri_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_dscpRemark_get);
EXPORT_SYMBOL(rtk_qos_dscpRemark_set);
EXPORT_SYMBOL(rtk_qos_dscp2DscpRemark_get);
EXPORT_SYMBOL(rtk_qos_dscp2DscpRemark_set);
#endif
EXPORT_SYMBOL(rtk_qos_1pRemarkSrcSel_get);
EXPORT_SYMBOL(rtk_qos_1pRemarkSrcSel_set);

EXPORT_SYMBOL(rtk_qos_outer1pRemarkSrcSel_get);
EXPORT_SYMBOL(rtk_qos_outer1pRemarkSrcSel_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_dscp2Dot1pRemark_get);
EXPORT_SYMBOL(rtk_qos_dscp2Dot1pRemark_set);
EXPORT_SYMBOL(rtk_qos_dscp2Outer1pRemark_get);
EXPORT_SYMBOL(rtk_qos_dscp2Outer1pRemark_set);
EXPORT_SYMBOL(rtk_qos_deiRemark_set);
EXPORT_SYMBOL(rtk_qos_deiRemark_get);
#endif
EXPORT_SYMBOL(rtk_qos_dscpRemarkSrcSel_get);
EXPORT_SYMBOL(rtk_qos_dscpRemarkSrcSel_set);
EXPORT_SYMBOL(rtk_qos_portDeiRemarkTagSel_get);
EXPORT_SYMBOL(rtk_qos_portDeiRemarkTagSel_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pRemarkSrcSel_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pRemarkSrcSel_set);
EXPORT_SYMBOL(rtk_qos_outer1pDfltPriCfgSrcSel_get);
EXPORT_SYMBOL(rtk_qos_outer1pDfltPriCfgSrcSel_set);
EXPORT_SYMBOL(rtk_qos_schedulingAlgorithm_get);
EXPORT_SYMBOL(rtk_qos_schedulingAlgorithm_set);
EXPORT_SYMBOL(rtk_qos_schedulingQueue_get);
EXPORT_SYMBOL(rtk_qos_schedulingQueue_set);
EXPORT_SYMBOL(rtk_qos_congAvoidAlgo_get);
EXPORT_SYMBOL(rtk_qos_congAvoidAlgo_set);
EXPORT_SYMBOL(rtk_qos_portCongAvoidAlgo_get);
EXPORT_SYMBOL(rtk_qos_portCongAvoidAlgo_set);
EXPORT_SYMBOL(rtk_qos_congAvoidSysThresh_get);
EXPORT_SYMBOL(rtk_qos_congAvoidSysThresh_set);
EXPORT_SYMBOL(rtk_qos_congAvoidSysDropProbability_get);
EXPORT_SYMBOL(rtk_qos_congAvoidSysDropProbability_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_congAvoidGlobalQueueThresh_get);
EXPORT_SYMBOL(rtk_qos_congAvoidGlobalQueueThresh_set);
EXPORT_SYMBOL(rtk_qos_congAvoidGlobalQueueDropProbability_get);
EXPORT_SYMBOL(rtk_qos_congAvoidGlobalQueueDropProbability_set);
#endif
EXPORT_SYMBOL(rtk_qos_congAvoidGlobalQueueConfig_get);
EXPORT_SYMBOL(rtk_qos_congAvoidGlobalQueueConfig_set);
EXPORT_SYMBOL(rtk_qos_portAvbStreamReservationClassEnable_get);
EXPORT_SYMBOL(rtk_qos_portAvbStreamReservationClassEnable_set);
EXPORT_SYMBOL(rtk_qos_avbStreamReservationConfig_get);
EXPORT_SYMBOL(rtk_qos_avbStreamReservationConfig_set);
EXPORT_SYMBOL(rtk_qos_pkt2CpuPriRemap_get);
EXPORT_SYMBOL(rtk_qos_pkt2CpuPriRemap_set);
EXPORT_SYMBOL(rtk_qos_rspanPriRemap_get);
EXPORT_SYMBOL(rtk_qos_rspanPriRemap_set);
EXPORT_SYMBOL(rtk_qos_portPri2IgrQMapEnable_get);
EXPORT_SYMBOL(rtk_qos_portPri2IgrQMapEnable_set);
EXPORT_SYMBOL(rtk_qos_portPri2IgrQMap_get);
EXPORT_SYMBOL(rtk_qos_portPri2IgrQMap_set);
EXPORT_SYMBOL(rtk_qos_portIgrQueueWeight_get);
EXPORT_SYMBOL(rtk_qos_portIgrQueueWeight_set);
EXPORT_SYMBOL(rtk_qos_invldDscpVal_get);
EXPORT_SYMBOL(rtk_qos_invldDscpVal_set);
EXPORT_SYMBOL(rtk_qos_invldDscpMask_get);
EXPORT_SYMBOL(rtk_qos_invldDscpMask_set);
EXPORT_SYMBOL(rtk_qos_portInvldDscpEnable_get);
EXPORT_SYMBOL(rtk_qos_portInvldDscpEnable_set);
EXPORT_SYMBOL(rtk_qos_invldDscpEnable_get);
EXPORT_SYMBOL(rtk_qos_invldDscpEnable_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_qos_portPriRemapEnable_get);
EXPORT_SYMBOL(rtk_qos_portPriRemapEnable_set);
#endif
EXPORT_SYMBOL(rtk_qos_sysPortPriRemapSel_get);
EXPORT_SYMBOL(rtk_qos_sysPortPriRemapSel_set);
EXPORT_SYMBOL(rtk_qos_portPortPriRemapSel_get);
EXPORT_SYMBOL(rtk_qos_portPortPriRemapSel_set);
EXPORT_SYMBOL(rtk_qos_portInnerPriRemapEnable_get);
EXPORT_SYMBOL(rtk_qos_portInnerPriRemapEnable_set);
EXPORT_SYMBOL(rtk_qos_portOuterPriRemapEnable_get);
EXPORT_SYMBOL(rtk_qos_portOuterPriRemapEnable_set);
EXPORT_SYMBOL(rtk_qos_priRemapEnable_get);
EXPORT_SYMBOL(rtk_qos_priRemapEnable_set);
EXPORT_SYMBOL(rtk_qos_portQueueStrictEnable_get);
EXPORT_SYMBOL(rtk_qos_portQueueStrictEnable_set);


EXPORT_SYMBOL(rtk_rate_init);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_stormControlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_stormControlIncludeIfg_set);
#endif
EXPORT_SYMBOL(rtk_rate_includeIfg_get);
EXPORT_SYMBOL(rtk_rate_includeIfg_set);
EXPORT_SYMBOL(rtk_rate_portIgrBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portIgrBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portIgrBwCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portIgrBwCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_igrBandwidthLowThresh_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthLowThresh_set);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthHighThresh_get);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthHighThresh_set);
#endif
EXPORT_SYMBOL(rtk_rate_igrBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_igrBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_igrPortBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_igrPortBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portIgrBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portIgrBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlExceed_get);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlExceed_reset);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlBypass_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlBypass_set);
EXPORT_SYMBOL(rtk_rate_portIgrBwFlowctrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portIgrBwFlowctrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_igrQueueBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_igrQueueBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlExceed_get);
EXPORT_SYMBOL(rtk_rate_portIgrQueueBwCtrlExceed_reset);
EXPORT_SYMBOL(rtk_rate_portEgrBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portEgrBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portEgrBwCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portEgrBwCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_portEgrBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portEgrBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_cpuEgrBandwidthCtrlRateMode_get);
EXPORT_SYMBOL(rtk_rate_cpuEgrBandwidthCtrlRateMode_set);
EXPORT_SYMBOL(rtk_rate_egrPortBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_egrPortBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueBwCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueBwCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlMode_get);
EXPORT_SYMBOL(rtk_rate_portEgrQueueAssuredBwCtrlMode_set);
EXPORT_SYMBOL(rtk_rate_egrQueueFixedBandwidthEnable_get);
EXPORT_SYMBOL(rtk_rate_egrQueueFixedBandwidthEnable_set);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_stormControlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_stormControlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlExceed_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlExceed_reset);
EXPORT_SYMBOL(rtk_rate_stormControlRateMode_get);
EXPORT_SYMBOL(rtk_rate_stormControlRateMode_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlRateMode_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlRateMode_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlTypeSel_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlTypeSel_set);
EXPORT_SYMBOL(rtk_rate_stormControlBypass_get);
EXPORT_SYMBOL(rtk_rate_stormControlBypass_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoEnable_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoEnable_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoRate_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoRate_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoBurstSize_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoBurstSize_set);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoExceed_get);
EXPORT_SYMBOL(rtk_rate_portStormCtrlProtoExceed_reset);
EXPORT_SYMBOL(rtk_rate_stormCtrlProtoVlanConstrtEnable_get);
EXPORT_SYMBOL(rtk_rate_stormCtrlProtoVlanConstrtEnable_set);


EXPORT_SYMBOL(rtk_stat_init);
EXPORT_SYMBOL(rtk_stat_global_reset);
EXPORT_SYMBOL(rtk_stat_port_reset);
EXPORT_SYMBOL(rtk_stat_global_get);
EXPORT_SYMBOL(rtk_stat_global_getAll);
EXPORT_SYMBOL(rtk_stat_port_get);
EXPORT_SYMBOL(rtk_stat_port_getAll);
EXPORT_SYMBOL(rtk_stat_tagLenCntIncEnable_get);
EXPORT_SYMBOL(rtk_stat_tagLenCntIncEnable_set);
EXPORT_SYMBOL(rtk_stat_stackHdrLenCntIncEnable_get);
EXPORT_SYMBOL(rtk_stat_stackHdrLenCntIncEnable_set);
EXPORT_SYMBOL(rtk_stat_flexibleCntRange_get);
EXPORT_SYMBOL(rtk_stat_flexibleCntRange_set);
EXPORT_SYMBOL(rtk_stat_enable_get);
EXPORT_SYMBOL(rtk_stat_enable_set);

EXPORT_SYMBOL(rtk_bpe_init);
EXPORT_SYMBOL(rtk_bpe_portFwdMode_get);
EXPORT_SYMBOL(rtk_bpe_portFwdMode_set);
EXPORT_SYMBOL(rtk_bpe_portEcidNameSpaceGroupId_get);
EXPORT_SYMBOL(rtk_bpe_portEcidNameSpaceGroupId_set);
EXPORT_SYMBOL(rtk_bpe_portPcid_get);
EXPORT_SYMBOL(rtk_bpe_portPcid_set);
EXPORT_SYMBOL(rtk_bpe_portPcidAct_get);
EXPORT_SYMBOL(rtk_bpe_portPcidAct_set);
EXPORT_SYMBOL(rtk_bpe_portEgrTagSts_get);
EXPORT_SYMBOL(rtk_bpe_portEgrTagSts_set);
EXPORT_SYMBOL(rtk_bpe_portEgrVlanTagSts_get);
EXPORT_SYMBOL(rtk_bpe_portEgrVlanTagSts_set);
EXPORT_SYMBOL(rtk_bpe_pvidEntry_add);
EXPORT_SYMBOL(rtk_bpe_pvidEntry_del);
EXPORT_SYMBOL(rtk_bpe_pvidEntry_get);
EXPORT_SYMBOL(rtk_bpe_pvidEntryNextValid_get);
EXPORT_SYMBOL(rtk_bpe_fwdEntry_add);
EXPORT_SYMBOL(rtk_bpe_fwdEntry_del);
EXPORT_SYMBOL(rtk_bpe_fwdEntry_get);
EXPORT_SYMBOL(rtk_bpe_fwdEntryNextValid_get);
EXPORT_SYMBOL(rtk_bpe_globalCtrl_get);
EXPORT_SYMBOL(rtk_bpe_globalCtrl_set);
EXPORT_SYMBOL(rtk_bpe_portCtrl_get);
EXPORT_SYMBOL(rtk_bpe_portCtrl_set);
EXPORT_SYMBOL(rtk_bpe_priRemarking_get);
EXPORT_SYMBOL(rtk_bpe_priRemarking_set);

EXPORT_SYMBOL(rtk_stp_init);
EXPORT_SYMBOL(rtk_stp_mstpInstance_create);
EXPORT_SYMBOL(rtk_stp_mstpInstance_destroy);
EXPORT_SYMBOL(rtk_stp_isMstpInstanceExist_get);
EXPORT_SYMBOL(rtk_stp_mstpState_get);
EXPORT_SYMBOL(rtk_stp_mstpState_set);
EXPORT_SYMBOL(rtk_stp_mstpInstanceMode_get);
EXPORT_SYMBOL(rtk_stp_mstpInstanceMode_set);

EXPORT_SYMBOL(rtk_switch_init);
EXPORT_SYMBOL(rtk_switch_deviceInfo_get);
EXPORT_SYMBOL(rtk_switch_deviceCapability_print);
EXPORT_SYMBOL(rtk_switch_cpuMaxPktLen_get);
EXPORT_SYMBOL(rtk_switch_cpuMaxPktLen_set);
EXPORT_SYMBOL(rtk_switch_maxPktLenLinkSpeed_get);
EXPORT_SYMBOL(rtk_switch_maxPktLenLinkSpeed_set);
EXPORT_SYMBOL(rtk_switch_maxPktLenTagLenCntIncEnable_get);
EXPORT_SYMBOL(rtk_switch_maxPktLenTagLenCntIncEnable_set);
EXPORT_SYMBOL(rtk_switch_chksumFailAction_get);
EXPORT_SYMBOL(rtk_switch_chksumFailAction_set);
EXPORT_SYMBOL(rtk_switch_recalcCRCEnable_get);
EXPORT_SYMBOL(rtk_switch_recalcCRCEnable_set);
EXPORT_SYMBOL(rtk_switch_mgmtMacAddr_get);
EXPORT_SYMBOL(rtk_switch_mgmtMacAddr_set);
EXPORT_SYMBOL(rtk_switch_IPv4Addr_get);
EXPORT_SYMBOL(rtk_switch_IPv4Addr_set);
EXPORT_SYMBOL(rtk_switch_pkt2CpuTypeFormat_get);
EXPORT_SYMBOL(rtk_switch_pkt2CpuTypeFormat_set);
EXPORT_SYMBOL(rtk_switch_pppoeIpParseEnable_get);
EXPORT_SYMBOL(rtk_switch_pppoeIpParseEnable_set);
EXPORT_SYMBOL(rtk_switch_snapMode_get);
EXPORT_SYMBOL(rtk_switch_snapMode_set);
EXPORT_SYMBOL(rtk_switch_cpuPktTruncateEnable_get);
EXPORT_SYMBOL(rtk_switch_cpuPktTruncateEnable_set);
EXPORT_SYMBOL(rtk_switch_cpuPktTruncateLen_get);
EXPORT_SYMBOL(rtk_switch_cpuPktTruncateLen_set);
EXPORT_SYMBOL(rtk_switch_portMaxPktLenLinkSpeed_get);
EXPORT_SYMBOL(rtk_switch_portMaxPktLenLinkSpeed_set);
EXPORT_SYMBOL(rtk_switch_portMaxPktLenTagLenCntIncEnable_get);
EXPORT_SYMBOL(rtk_switch_portMaxPktLenTagLenCntIncEnable_set);
EXPORT_SYMBOL(rtk_switch_flexTblFmt_get);
EXPORT_SYMBOL(rtk_switch_flexTblFmt_set);

EXPORT_SYMBOL(rtk_time_init);
EXPORT_SYMBOL(rtk_time_portPtpEnable_get);
EXPORT_SYMBOL(rtk_time_portPtpEnable_set);
EXPORT_SYMBOL(rtk_time_portPtpRxTimestamp_get);
EXPORT_SYMBOL(rtk_time_portPtpTxTimestamp_get);
EXPORT_SYMBOL(rtk_time_portPtpTxTimestampCallback_register);
EXPORT_SYMBOL(rtk_time_portRefTime_get);
EXPORT_SYMBOL(rtk_time_portRefTime_set);
EXPORT_SYMBOL(rtk_time_portRefTimeAdjust_set);
EXPORT_SYMBOL(rtk_time_portRefTimeEnable_get);
EXPORT_SYMBOL(rtk_time_portRefTimeEnable_set);
EXPORT_SYMBOL(rtk_time_portRefTimeFreq_get);
EXPORT_SYMBOL(rtk_time_portRefTimeFreq_set);
EXPORT_SYMBOL(rtk_time_correctionFieldTransparentValue_get);
EXPORT_SYMBOL(rtk_time_portPtpMacAddr_get);
EXPORT_SYMBOL(rtk_time_portPtpMacAddr_set);
EXPORT_SYMBOL(rtk_time_portPtpMacAddrRange_get);
EXPORT_SYMBOL(rtk_time_portPtpMacAddrRange_set);
EXPORT_SYMBOL(rtk_time_portPtpVlanTpid_get);
EXPORT_SYMBOL(rtk_time_portPtpVlanTpid_set);
EXPORT_SYMBOL(rtk_time_portPtpOper_get);
EXPORT_SYMBOL(rtk_time_portPtpOper_set);
EXPORT_SYMBOL(rtk_time_portPtpLatchTime_get);
EXPORT_SYMBOL(rtk_time_portPtpRefTimeFreqCfg_get);
EXPORT_SYMBOL(rtk_time_portPtpRefTimeFreqCfg_set);
EXPORT_SYMBOL(rtk_time_portPtpTxInterruptStatus_get);
EXPORT_SYMBOL(rtk_time_portPtpInterruptEnable_get);
EXPORT_SYMBOL(rtk_time_portPtpInterruptEnable_set);
EXPORT_SYMBOL(rtk_time_portPtpTxTimestampFifo_get);
EXPORT_SYMBOL(rtk_time_portPtp1PPSOutput_get);
EXPORT_SYMBOL(rtk_time_portPtp1PPSOutput_set);
EXPORT_SYMBOL(rtk_time_portPtpClockOutput_get);
EXPORT_SYMBOL(rtk_time_portPtpClockOutput_set);
EXPORT_SYMBOL(rtk_time_portPtpOutputSigSel_get);
EXPORT_SYMBOL(rtk_time_portPtpOutputSigSel_set);
EXPORT_SYMBOL(rtk_time_portPtpTransEnable_get);
EXPORT_SYMBOL(rtk_time_portPtpTransEnable_set);
EXPORT_SYMBOL(rtk_time_portPtpLinkDelay_get);
EXPORT_SYMBOL(rtk_time_portPtpLinkDelay_set);
EXPORT_SYMBOL(rtk_time_portPtpFullSec_get);

EXPORT_SYMBOL(rtk_trap_init);
EXPORT_SYMBOL(rtk_trap_rmaAction_get);
EXPORT_SYMBOL(rtk_trap_rmaAction_set);
EXPORT_SYMBOL(rtk_trap_bypassStp_get);
EXPORT_SYMBOL(rtk_trap_bypassStp_set);
EXPORT_SYMBOL(rtk_trap_bypassVlan_get);
EXPORT_SYMBOL(rtk_trap_bypassVlan_set);
EXPORT_SYMBOL(rtk_trap_userDefineRma_get);
EXPORT_SYMBOL(rtk_trap_userDefineRma_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaEnable_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaEnable_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaAction_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaAction_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameAction_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameAction_set);
EXPORT_SYMBOL(rtk_trap_mgmtFramePri_get);
EXPORT_SYMBOL(rtk_trap_mgmtFramePri_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameQueue_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameQueue_set);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameAction_get);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameAction_set);
EXPORT_SYMBOL(rtk_trap_pktWithCFIAction_get);
EXPORT_SYMBOL(rtk_trap_pktWithCFIAction_set);
EXPORT_SYMBOL(rtk_trap_pktWithOuterCFIAction_get);
EXPORT_SYMBOL(rtk_trap_pktWithOuterCFIAction_set);
EXPORT_SYMBOL(rtk_trap_pktWithCFIPri_get);
EXPORT_SYMBOL(rtk_trap_pktWithCFIPri_set);
EXPORT_SYMBOL(rtk_trap_cfmAct_get);
EXPORT_SYMBOL(rtk_trap_cfmAct_set);
EXPORT_SYMBOL(rtk_trap_cfmTarget_get);
EXPORT_SYMBOL(rtk_trap_cfmTarget_set);
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
EXPORT_SYMBOL(rtk_trap_cfmUnknownFrameAct_get);
EXPORT_SYMBOL(rtk_trap_cfmUnknownFrameAct_set);
EXPORT_SYMBOL(rtk_trap_cfmLoopbackLinkTraceAct_get);
EXPORT_SYMBOL(rtk_trap_cfmLoopbackLinkTraceAct_set);
EXPORT_SYMBOL(rtk_trap_cfmCcmAct_get);
EXPORT_SYMBOL(rtk_trap_cfmCcmAct_set);
EXPORT_SYMBOL(rtk_trap_cfmEthDmAct_get);
EXPORT_SYMBOL(rtk_trap_cfmEthDmAct_set);
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapPri_get);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapPri_set);
EXPORT_SYMBOL(rtk_trap_oamPDUAction_get);
EXPORT_SYMBOL(rtk_trap_oamPDUAction_set);
EXPORT_SYMBOL(rtk_trap_oamPDUPri_get);
EXPORT_SYMBOL(rtk_trap_oamPDUPri_set);
EXPORT_SYMBOL(rtk_trap_portOamLoopbackParAction_get);
EXPORT_SYMBOL(rtk_trap_portOamLoopbackParAction_set);
EXPORT_SYMBOL(rtk_trap_routeExceptionAction_get);
EXPORT_SYMBOL(rtk_trap_routeExceptionAction_set);
EXPORT_SYMBOL(rtk_trap_routeExceptionPri_get);
EXPORT_SYMBOL(rtk_trap_routeExceptionPri_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameMgmtVlanEnable_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameMgmtVlanEnable_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameSelfARPEnable_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameSelfARPEnable_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaLearningEnable_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaLearningEnable_set);
EXPORT_SYMBOL(rtk_trap_rmaLearningEnable_get);
EXPORT_SYMBOL(rtk_trap_rmaLearningEnable_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameLearningEnable_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameLearningEnable_set);
EXPORT_SYMBOL(rtk_trap_bpduFloodPortmask_get);
EXPORT_SYMBOL(rtk_trap_bpduFloodPortmask_set);
EXPORT_SYMBOL(rtk_trap_eapolFloodPortmask_get);
EXPORT_SYMBOL(rtk_trap_eapolFloodPortmask_set);
EXPORT_SYMBOL(rtk_trap_lldpFloodPortmask_get);
EXPORT_SYMBOL(rtk_trap_lldpFloodPortmask_set);
EXPORT_SYMBOL(rtk_trap_userDefineFloodPortmask_get);
EXPORT_SYMBOL(rtk_trap_userDefineFloodPortmask_set);
EXPORT_SYMBOL(rtk_trap_rmaFloodPortmask_get);
EXPORT_SYMBOL(rtk_trap_rmaFloodPortmask_set);
EXPORT_SYMBOL(rtk_trap_rmaCancelMirror_get);
EXPORT_SYMBOL(rtk_trap_rmaCancelMirror_set);
EXPORT_SYMBOL(rtk_trap_rmaGroupAction_get);
EXPORT_SYMBOL(rtk_trap_rmaGroupAction_set);
EXPORT_SYMBOL(rtk_trap_rmaGroupLearningEnable_get);
EXPORT_SYMBOL(rtk_trap_rmaGroupLearningEnable_set);
EXPORT_SYMBOL(rtk_trap_rmaLookupMissActionEnable_get);
EXPORT_SYMBOL(rtk_trap_rmaLookupMissActionEnable_set);
EXPORT_SYMBOL(rtk_trap_oamTarget_get);
EXPORT_SYMBOL(rtk_trap_oamTarget_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameTarget_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameTarget_set);
EXPORT_SYMBOL(rtk_trap_capwapInvldHdr_get);
EXPORT_SYMBOL(rtk_trap_capwapInvldHdr_set);

EXPORT_SYMBOL(rtk_trunk_init);
EXPORT_SYMBOL(rtk_trunk_mode_get);
EXPORT_SYMBOL(rtk_trunk_mode_set);
EXPORT_SYMBOL(rtk_trunk_port_get);
EXPORT_SYMBOL(rtk_trunk_port_set);
EXPORT_SYMBOL(rtk_trunk_localPort_get);
EXPORT_SYMBOL(rtk_trunk_localPort_set);
EXPORT_SYMBOL(rtk_trunk_egrPort_get);
EXPORT_SYMBOL(rtk_trunk_egrPort_set);
EXPORT_SYMBOL(rtk_trunk_tunnelHashSrc_get);
EXPORT_SYMBOL(rtk_trunk_tunnelHashSrc_set);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmBind_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmBind_set);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmTypeBind_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmTypeBind_set);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmParam_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmParam_set);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmTypeParam_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmTypeParam_set);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmShift_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmShift_set);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmShiftGbl_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithmShiftGbl_set);
EXPORT_SYMBOL(rtk_trunk_trafficSeparate_get);
EXPORT_SYMBOL(rtk_trunk_trafficSeparate_set);
EXPORT_SYMBOL(rtk_trunk_trafficSeparateEnable_get);
EXPORT_SYMBOL(rtk_trunk_trafficSeparateEnable_set);
EXPORT_SYMBOL(rtk_trunk_trafficSeparateDivision_get);
EXPORT_SYMBOL(rtk_trunk_trafficSeparateDivision_set);
EXPORT_SYMBOL(rtk_trunk_stkTrkPort_get);
EXPORT_SYMBOL(rtk_trunk_stkTrkPort_set);
EXPORT_SYMBOL(rtk_trunk_stkTrkHash_get);
EXPORT_SYMBOL(rtk_trunk_stkTrkHash_set);
EXPORT_SYMBOL(rtk_trunk_stkDistributionAlgorithmTypeBind_get);
EXPORT_SYMBOL(rtk_trunk_stkDistributionAlgorithmTypeBind_set);
EXPORT_SYMBOL(rtk_trunk_localFirst_get);
EXPORT_SYMBOL(rtk_trunk_localFirst_set);
EXPORT_SYMBOL(rtk_trunk_localFirstFailOver_get);
EXPORT_SYMBOL(rtk_trunk_localFirstFailOver_set);
EXPORT_SYMBOL(rtk_trunk_srcPortMap_get);
EXPORT_SYMBOL(rtk_trunk_srcPortMap_set);

EXPORT_SYMBOL(rtk_vlan_init);
EXPORT_SYMBOL(rtk_vlan_create);
EXPORT_SYMBOL(rtk_vlan_destroy);
EXPORT_SYMBOL(rtk_vlan_destroyAll);
EXPORT_SYMBOL(rtk_vlan_port_add);
EXPORT_SYMBOL(rtk_vlan_port_del);
EXPORT_SYMBOL(rtk_vlan_port_get);
EXPORT_SYMBOL(rtk_vlan_port_set);
EXPORT_SYMBOL(rtk_vlan_mcastGroup_get);
EXPORT_SYMBOL(rtk_vlan_mcastGroup_set);
EXPORT_SYMBOL(rtk_vlan_svlMode_get);
EXPORT_SYMBOL(rtk_vlan_svlMode_set);
EXPORT_SYMBOL(rtk_vlan_svlFid_get);
EXPORT_SYMBOL(rtk_vlan_svlFid_set);
EXPORT_SYMBOL(rtk_vlan_stg_get);
EXPORT_SYMBOL(rtk_vlan_stg_set);
EXPORT_SYMBOL(rtk_vlan_l2LookupSvlFid_get);
EXPORT_SYMBOL(rtk_vlan_l2LookupSvlFid_set);
EXPORT_SYMBOL(rtk_vlan_l2LookupMode_get);
EXPORT_SYMBOL(rtk_vlan_l2LookupMode_set);
EXPORT_SYMBOL(rtk_vlan_groupMask_get);
EXPORT_SYMBOL(rtk_vlan_groupMask_set);
EXPORT_SYMBOL(rtk_vlan_profileIdx_get);
EXPORT_SYMBOL(rtk_vlan_profileIdx_set);
EXPORT_SYMBOL(rtk_vlan_profile_get);
EXPORT_SYMBOL(rtk_vlan_profile_set);
EXPORT_SYMBOL(rtk_vlan_portFwdVlan_get);
EXPORT_SYMBOL(rtk_vlan_portFwdVlan_set);
EXPORT_SYMBOL(rtk_vlan_portAcceptFrameType_get);
EXPORT_SYMBOL(rtk_vlan_portAcceptFrameType_set);
EXPORT_SYMBOL(rtk_vlan_portIgrFilter_get);
EXPORT_SYMBOL(rtk_vlan_portIgrFilter_set);
EXPORT_SYMBOL(rtk_vlan_portEgrFilterEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrFilterEnable_set);
EXPORT_SYMBOL(rtk_vlan_mcastLeakyEnable_get);
EXPORT_SYMBOL(rtk_vlan_mcastLeakyEnable_set);
EXPORT_SYMBOL(rtk_vlan_portPvid_get);
EXPORT_SYMBOL(rtk_vlan_portPvid_set);
EXPORT_SYMBOL(rtk_vlan_portPrivateVlanEnable_get);
EXPORT_SYMBOL(rtk_vlan_portPrivateVlanEnable_set);
EXPORT_SYMBOL(rtk_vlan_portPvidMode_get);
EXPORT_SYMBOL(rtk_vlan_portPvidMode_set);
EXPORT_SYMBOL(rtk_vlan_protoGroup_get);
EXPORT_SYMBOL(rtk_vlan_protoGroup_set);
EXPORT_SYMBOL(rtk_vlan_portProtoVlan_get);
EXPORT_SYMBOL(rtk_vlan_portProtoVlan_set);
EXPORT_SYMBOL(rtk_vlan_portMacBasedVlanEnable_get);
EXPORT_SYMBOL(rtk_vlan_portMacBasedVlanEnable_set);
EXPORT_SYMBOL(rtk_vlan_macBasedVlan_get);
EXPORT_SYMBOL(rtk_vlan_macBasedVlan_set);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanWithMsk_get);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanWithMsk_set);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanWithPort_get);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanWithPort_set);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanEntry_get);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanEntry_set);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanEntry_add);
EXPORT_SYMBOL(rtk_vlan_macBasedVlanEntry_del);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlan_get);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlan_set);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlanWithPort_get);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlanWithPort_set);
EXPORT_SYMBOL(rtk_vlan_portIpSubnetBasedVlanEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIpSubnetBasedVlanEnable_set);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlanEntry_get);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlanEntry_set);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlanEntry_add);
EXPORT_SYMBOL(rtk_vlan_ipSubnetBasedVlanEntry_del);
EXPORT_SYMBOL(rtk_vlan_tpidEntry_get);
EXPORT_SYMBOL(rtk_vlan_tpidEntry_set);
EXPORT_SYMBOL(rtk_vlan_portIgrTpid_get);
EXPORT_SYMBOL(rtk_vlan_portIgrTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTpid_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTpidSrc_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTpidSrc_set);
EXPORT_SYMBOL(rtk_vlan_portIgrExtraTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrExtraTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTagSts_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTagSts_set);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanTransparentEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanTransparentEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanTransparentEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanTransparentEnable_set);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtBlkMode_get);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtBlkMode_set);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtEntry_get);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtEntry_set);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanCnvtEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanCnvtEnable_set);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtDblTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtDblTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtVidSource_get);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtVidSource_set);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtEntry_get);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtEntry_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtEnable_set);
EXPORT_SYMBOL(rtk_vlan_aggrEnable_get);
EXPORT_SYMBOL(rtk_vlan_aggrEnable_set);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrEnable_get);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrEnable_set);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrPriEnable_get);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrPriEnable_set);
EXPORT_SYMBOL(rtk_vlan_leakyStpFilter_get);
EXPORT_SYMBOL(rtk_vlan_leakyStpFilter_set);
EXPORT_SYMBOL(rtk_vlan_except_get);
EXPORT_SYMBOL(rtk_vlan_except_set);
EXPORT_SYMBOL(rtk_vlan_portIgrCnvtDfltAct_get);
EXPORT_SYMBOL(rtk_vlan_portIgrCnvtDfltAct_set);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanCnvtLuMisAct_get);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanCnvtLuMisAct_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtLuMisAct_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtLuMisAct_set);
EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepType_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepType_get);
EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepType_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepType_set);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrCtrl_get);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrCtrl_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidSource_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidSource_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidTarget_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidTarget_set);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtHitIndication_get);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtHitIndication_get);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtEntry_delAll);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtEntry_delAll);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanCnvtRangeCheckSet_get);
EXPORT_SYMBOL(rtk_vlan_portIgrVlanCnvtRangeCheckSet_set);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtRangeCheckEntry_get);
EXPORT_SYMBOL(rtk_vlan_igrVlanCnvtRangeCheckEntry_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtRangeCheckSet_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtRangeCheckSet_set);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtRangeCheckEntry_get);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtRangeCheckEntry_set);
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
EXPORT_SYMBOL(rtk_vlan_portVlanAggrVidSource_get);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrVidSource_set);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrPriTagVidSource_get);
EXPORT_SYMBOL(rtk_vlan_portVlanAggrPriTagVidSource_set);
EXPORT_SYMBOL(rtk_vlan_l2UcastLookupMode_get);
EXPORT_SYMBOL(rtk_vlan_l2UcastLookupMode_set);
EXPORT_SYMBOL(rtk_vlan_l2McastLookupMode_get);
EXPORT_SYMBOL(rtk_vlan_l2McastLookupMode_set);
EXPORT_SYMBOL(rtk_vlan_portInnerAcceptFrameType_get);
EXPORT_SYMBOL(rtk_vlan_portInnerAcceptFrameType_set);
EXPORT_SYMBOL(rtk_vlan_portOuterAcceptFrameType_get);
EXPORT_SYMBOL(rtk_vlan_portOuterAcceptFrameType_set);
EXPORT_SYMBOL(rtk_vlan_portInnerPvid_get);
EXPORT_SYMBOL(rtk_vlan_portInnerPvid_set);
EXPORT_SYMBOL(rtk_vlan_portOuterPvid_get);
EXPORT_SYMBOL(rtk_vlan_portOuterPvid_set);
EXPORT_SYMBOL(rtk_vlan_portInnerPvidMode_get);
EXPORT_SYMBOL(rtk_vlan_portInnerPvidMode_set);
EXPORT_SYMBOL(rtk_vlan_portOuterPvidMode_get);
EXPORT_SYMBOL(rtk_vlan_portOuterPvidMode_set);
EXPORT_SYMBOL(rtk_vlan_portIgrInnerTpid_get);
EXPORT_SYMBOL(rtk_vlan_portIgrInnerTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTpid_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTpid_set);
EXPORT_SYMBOL(rtk_vlan_portIgrOuterTpid_get);
EXPORT_SYMBOL(rtk_vlan_portIgrOuterTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTpid_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTpid_set);
EXPORT_SYMBOL(rtk_vlan_innerTpidEntry_get);
EXPORT_SYMBOL(rtk_vlan_innerTpidEntry_set);
EXPORT_SYMBOL(rtk_vlan_outerTpidEntry_get);
EXPORT_SYMBOL(rtk_vlan_outerTpidEntry_set);
EXPORT_SYMBOL(rtk_vlan_extraTpidEntry_get);
EXPORT_SYMBOL(rtk_vlan_extraTpidEntry_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTagSts_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTagSts_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTagSts_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTagSts_set);
EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtLookupMissAct_get);
EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtLookupMissAct_set);
#endif
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtRangeCheckVid_get);
EXPORT_SYMBOL(rtk_vlan_egrVlanCnvtRangeCheckVid_set);
EXPORT_SYMBOL(rtk_vlan_ecidPmsk_add);
EXPORT_SYMBOL(rtk_vlan_ecidPmsk_del);
EXPORT_SYMBOL(rtk_vlan_ecidPmsk_get);
EXPORT_SYMBOL(rtk_vlan_ecidPmskNextValid_get);
EXPORT_SYMBOL(rtk_vlan_trkVlanAggrEnable_get);
EXPORT_SYMBOL(rtk_vlan_trkVlanAggrEnable_set);
EXPORT_SYMBOL(rtk_vlan_trkVlanAggrPriEnable_get);
EXPORT_SYMBOL(rtk_vlan_trkVlanAggrPriEnable_set);
EXPORT_SYMBOL(rtk_vlan_trkVlanAggrCtrl_get);
EXPORT_SYMBOL(rtk_vlan_trkVlanAggrCtrl_set);
EXPORT_SYMBOL(rtk_vlan_intfId_get);
EXPORT_SYMBOL(rtk_vlan_intfId_set);

EXPORT_SYMBOL(rtk_eee_init);
EXPORT_SYMBOL(rtk_eee_portEnable_get);
EXPORT_SYMBOL(rtk_eee_portEnable_set);
EXPORT_SYMBOL(rtk_eeep_portEnable_get);
EXPORT_SYMBOL(rtk_eeep_portEnable_set);
EXPORT_SYMBOL(rtk_eee_portState_get);
EXPORT_SYMBOL(rtk_eee_portPowerState_get);


EXPORT_SYMBOL(rtk_l3_init);
EXPORT_SYMBOL(rtk_l3_routeEntry_get);
EXPORT_SYMBOL(rtk_l3_routeEntry_set);
EXPORT_SYMBOL(rtk_l3_routeSwitchMacAddr_get);
EXPORT_SYMBOL(rtk_l3_routeSwitchMacAddr_set);
EXPORT_SYMBOL(rtk_l3_info_t_init);
EXPORT_SYMBOL(rtk_l3_info_get);
EXPORT_SYMBOL(rtk_l3_routerMacEntry_get);
EXPORT_SYMBOL(rtk_l3_routerMacEntry_set);
EXPORT_SYMBOL(rtk_l3_intf_t_init);
EXPORT_SYMBOL(rtk_l3_intf_create);
EXPORT_SYMBOL(rtk_l3_intf_destroy);
EXPORT_SYMBOL(rtk_l3_intf_destroyAll);
EXPORT_SYMBOL(rtk_l3_intf_get);
EXPORT_SYMBOL(rtk_l3_intf_set);
EXPORT_SYMBOL(rtk_l3_intfStats_get);
EXPORT_SYMBOL(rtk_l3_intfStats_reset);
EXPORT_SYMBOL(rtk_l3_vrrp_add);
EXPORT_SYMBOL(rtk_l3_vrrp_del);
EXPORT_SYMBOL(rtk_l3_vrrp_delAll);
EXPORT_SYMBOL(rtk_l3_vrrp_get);
EXPORT_SYMBOL(rtk_l3_nextHop_t_init);
EXPORT_SYMBOL(rtk_l3_nextHop_create);
EXPORT_SYMBOL(rtk_l3_nextHop_destroy);
EXPORT_SYMBOL(rtk_l3_nextHop_get);
EXPORT_SYMBOL(rtk_l3_nextHopPath_find);
EXPORT_SYMBOL(rtk_l3_ecmp_create);
EXPORT_SYMBOL(rtk_l3_ecmp_destroy);
EXPORT_SYMBOL(rtk_l3_ecmp_get);
EXPORT_SYMBOL(rtk_l3_ecmp_add);
EXPORT_SYMBOL(rtk_l3_ecmp_del);
EXPORT_SYMBOL(rtk_l3_ecmp_find);
EXPORT_SYMBOL(rtk_l3_key_t_init);
EXPORT_SYMBOL(rtk_l3_host_t_init);
EXPORT_SYMBOL(rtk_l3_host_add);
EXPORT_SYMBOL(rtk_l3_host_del);
EXPORT_SYMBOL(rtk_l3_host_del_byNetwork);
EXPORT_SYMBOL(rtk_l3_host_del_byIntfId);
EXPORT_SYMBOL(rtk_l3_host_delAll);
EXPORT_SYMBOL(rtk_l3_host_find);
EXPORT_SYMBOL(rtk_l3_hostConflict_get);
EXPORT_SYMBOL(rtk_l3_host_age);
EXPORT_SYMBOL(rtk_l3_host_getNext);
EXPORT_SYMBOL(rtk_l3_route_t_init);
EXPORT_SYMBOL(rtk_l3_route_add);
EXPORT_SYMBOL(rtk_l3_route_del);
EXPORT_SYMBOL(rtk_l3_route_get);
EXPORT_SYMBOL(rtk_l3_route_del_byIntfId);
EXPORT_SYMBOL(rtk_l3_route_delAll);
EXPORT_SYMBOL(rtk_l3_route_age);
EXPORT_SYMBOL(rtk_l3_route_getNext);
EXPORT_SYMBOL(rtk_l3_globalCtrl_get);
EXPORT_SYMBOL(rtk_l3_globalCtrl_set);
EXPORT_SYMBOL(rtk_l3_intfCtrl_get);
EXPORT_SYMBOL(rtk_l3_intfCtrl_set);
EXPORT_SYMBOL(rtk_l3_portCtrl_get);
EXPORT_SYMBOL(rtk_l3_portCtrl_set);
EXPORT_SYMBOL(rtk_mcast_init);
EXPORT_SYMBOL(rtk_mcast_group_create);
EXPORT_SYMBOL(rtk_mcast_group_destroy);
EXPORT_SYMBOL(rtk_mcast_group_getNext);
EXPORT_SYMBOL(rtk_mcast_egrIf_get);
EXPORT_SYMBOL(rtk_mcast_egrIf_add);
EXPORT_SYMBOL(rtk_mcast_egrIf_del);
EXPORT_SYMBOL(rtk_mcast_egrIf_delAll);

EXPORT_SYMBOL(rtk_ipmc_init);
EXPORT_SYMBOL(rtk_ipmc_addr_t_init);
EXPORT_SYMBOL(rtk_ipmc_addr_add);
EXPORT_SYMBOL(rtk_ipmc_addr_find);
EXPORT_SYMBOL(rtk_ipmc_addr_del);
EXPORT_SYMBOL(rtk_ipmc_addr_delAll);
EXPORT_SYMBOL(rtk_ipmc_nextValidAddr_get);
EXPORT_SYMBOL(rtk_ipmc_statMont_create);
EXPORT_SYMBOL(rtk_ipmc_statMont_destroy);
EXPORT_SYMBOL(rtk_ipmc_statCntr_reset);
EXPORT_SYMBOL(rtk_ipmc_statCntr_get);
EXPORT_SYMBOL(rtk_ipmc_globalCtrl_set);
EXPORT_SYMBOL(rtk_ipmc_globalCtrl_get);
EXPORT_SYMBOL(rtk_oam_init);
EXPORT_SYMBOL(rtk_oam_portDyingGaspPayload_set);
EXPORT_SYMBOL(rtk_oam_dyingGaspSend_set);
EXPORT_SYMBOL(rtk_oam_autoDyingGaspEnable_get);
EXPORT_SYMBOL(rtk_oam_autoDyingGaspEnable_set);
EXPORT_SYMBOL(rtk_oam_dyingGaspWaitTime_get);
EXPORT_SYMBOL(rtk_oam_dyingGaspWaitTime_set);
EXPORT_SYMBOL(rtk_oam_loopbackMacSwapEnable_get);
EXPORT_SYMBOL(rtk_oam_loopbackMacSwapEnable_set);
EXPORT_SYMBOL(rtk_oam_portLoopbackMuxAction_get);
EXPORT_SYMBOL(rtk_oam_portLoopbackMuxAction_set);
EXPORT_SYMBOL(rtk_oam_pduLearningEnable_get);
EXPORT_SYMBOL(rtk_oam_pduLearningEnable_set);

EXPORT_SYMBOL(rtk_oam_cfmCcmPcp_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmPcp_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmCfi_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmCfi_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmTpid_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmTpid_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstLifetime_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstLifetime_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmMepid_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmMepid_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmIntervalField_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmIntervalField_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmMdl_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmMdl_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxMepid_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxMepid_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxIntervalField_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxIntervalField_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxMdl_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxMdl_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTagStatus_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTagStatus_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstVid_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstVid_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstMaid_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstMaid_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxStatus_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxStatus_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstInterval_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstInterval_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmTxInstPort_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmTxInstPort_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmRxInstVid_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmRxInstVid_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmRxInstPort_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmRxInstPort_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstAliveTime_get);
EXPORT_SYMBOL(rtk_oam_cfmPortEthDmEnable_set);
EXPORT_SYMBOL(rtk_oam_cfmPortEthDmEnable_get);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRefTimeEnable_get);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRefTimeFreq_set);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRefTimeEnable_set);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRxTimestamp_get);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRefTimeFreq_get);
EXPORT_SYMBOL(rtk_oam_cfmEthDmTxDelay_get);
EXPORT_SYMBOL(rtk_oam_cfmEthDmTxDelay_set);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRefTime_get);
EXPORT_SYMBOL(rtk_oam_cfmEthDmRefTime_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxMember_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstTxMember_set);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstRxMember_get);
EXPORT_SYMBOL(rtk_oam_cfmCcmInstRxMember_set);
EXPORT_SYMBOL(rtk_oam_dyingGaspPktCnt_get);
EXPORT_SYMBOL(rtk_oam_dyingGaspPktCnt_set);
EXPORT_SYMBOL(rtk_oam_linkFaultMonEnable_set);
EXPORT_SYMBOL(rtk_acl_init);
EXPORT_SYMBOL(rtk_acl_portLookupEnable_get);
EXPORT_SYMBOL(rtk_acl_portLookupEnable_set);
EXPORT_SYMBOL(rtk_acl_lookupMissAct_get);
EXPORT_SYMBOL(rtk_acl_lookupMissAct_set);
EXPORT_SYMBOL(rtk_acl_rangeCheckFieldSelector_get);
EXPORT_SYMBOL(rtk_acl_rangeCheckFieldSelector_set);
EXPORT_SYMBOL(rtk_acl_partition_get);
EXPORT_SYMBOL(rtk_acl_partition_set);
EXPORT_SYMBOL(rtk_acl_blockResultMode_get);
EXPORT_SYMBOL(rtk_acl_blockResultMode_set);
EXPORT_SYMBOL(rtk_acl_templateFieldIntentVlanTag_get);
EXPORT_SYMBOL(rtk_acl_templateFieldIntentVlanTag_set);
EXPORT_SYMBOL(rtk_acl_rangeCheckDstPort_get);
EXPORT_SYMBOL(rtk_acl_rangeCheckDstPort_set);
EXPORT_SYMBOL(rtk_acl_ruleValidate_get);
EXPORT_SYMBOL(rtk_acl_ruleValidate_set);
EXPORT_SYMBOL(rtk_acl_ruleEntryFieldSize_get);
EXPORT_SYMBOL(rtk_acl_ruleEntrySize_get);
EXPORT_SYMBOL(rtk_acl_ruleEntry_read);
EXPORT_SYMBOL(rtk_acl_ruleEntry_write);
EXPORT_SYMBOL(rtk_acl_ruleEntryField_get);
EXPORT_SYMBOL(rtk_acl_ruleEntryField_set);
EXPORT_SYMBOL(rtk_acl_ruleEntryField_read);
EXPORT_SYMBOL(rtk_acl_ruleEntryField_write);
EXPORT_SYMBOL(rtk_acl_ruleOperation_get);
EXPORT_SYMBOL(rtk_acl_ruleOperation_set);
EXPORT_SYMBOL(rtk_acl_ruleAction_get);
EXPORT_SYMBOL(rtk_acl_ruleAction_set);
EXPORT_SYMBOL(rtk_acl_blockPwrEnable_get);
EXPORT_SYMBOL(rtk_acl_blockPwrEnable_set);
EXPORT_SYMBOL(rtk_acl_blockGroupEnable_get);
EXPORT_SYMBOL(rtk_acl_blockGroupEnable_set);
EXPORT_SYMBOL(rtk_acl_statPktCnt_get);
EXPORT_SYMBOL(rtk_acl_statPktCnt_clear);
EXPORT_SYMBOL(rtk_acl_statByteCnt_get);
EXPORT_SYMBOL(rtk_acl_statByteCnt_clear);
EXPORT_SYMBOL(rtk_acl_stat_clearAll);
EXPORT_SYMBOL(rtk_acl_meterMode_get);
EXPORT_SYMBOL(rtk_acl_meterMode_set);
EXPORT_SYMBOL(rtk_acl_meterBurstSize_get);
EXPORT_SYMBOL(rtk_acl_meterBurstSize_set);
EXPORT_SYMBOL(rtk_acl_rangeCheckL4Port_get);
EXPORT_SYMBOL(rtk_acl_rangeCheckL4Port_set);
EXPORT_SYMBOL(rtk_acl_rangeCheckVid_get);
EXPORT_SYMBOL(rtk_acl_rangeCheckVid_set);
EXPORT_SYMBOL(rtk_acl_rangeCheckSrcPort_get);
EXPORT_SYMBOL(rtk_acl_rangeCheckSrcPort_set);
EXPORT_SYMBOL(rtk_acl_rangeCheckPacketLen_get);
EXPORT_SYMBOL(rtk_acl_rangeCheckPacketLen_set);
EXPORT_SYMBOL(rtk_acl_ruleEntryField_check);
EXPORT_SYMBOL(rtk_acl_loopBackEnable_get);
EXPORT_SYMBOL(rtk_acl_loopBackEnable_set);
EXPORT_SYMBOL(rtk_acl_limitLoopbackTimes_get);
EXPORT_SYMBOL(rtk_acl_limitLoopbackTimes_set);
EXPORT_SYMBOL(rtk_acl_portPhaseLookupEnable_get);
EXPORT_SYMBOL(rtk_acl_portPhaseLookupEnable_set);
EXPORT_SYMBOL(rtk_acl_templateSelector_get);
EXPORT_SYMBOL(rtk_acl_templateSelector_set);
EXPORT_SYMBOL(rtk_acl_statCnt_get);
EXPORT_SYMBOL(rtk_acl_statCnt_clear);
EXPORT_SYMBOL(rtk_acl_ruleHitIndication_get);
EXPORT_SYMBOL(rtk_acl_ruleHitIndicationMask_get);
EXPORT_SYMBOL(rtk_acl_rule_del);
EXPORT_SYMBOL(rtk_acl_rule_move);
EXPORT_SYMBOL(rtk_acl_ruleEntryField_validate);
EXPORT_SYMBOL(rtk_acl_fieldUsr2Template_get);
EXPORT_SYMBOL(rtk_acl_templateId_get);

EXPORT_SYMBOL(rtk_pie_init);
EXPORT_SYMBOL(rtk_pie_phase_get);
EXPORT_SYMBOL(rtk_pie_phase_set);
EXPORT_SYMBOL(rtk_pie_blockLookupEnable_get);
EXPORT_SYMBOL(rtk_pie_blockLookupEnable_set);
EXPORT_SYMBOL(rtk_pie_blockGrouping_get);
EXPORT_SYMBOL(rtk_pie_blockGrouping_set);
EXPORT_SYMBOL(rtk_pie_template_get);
EXPORT_SYMBOL(rtk_pie_template_set);
EXPORT_SYMBOL(rtk_pie_templateField_check);
EXPORT_SYMBOL(rtk_pie_rangeCheckIp_get);
EXPORT_SYMBOL(rtk_pie_rangeCheckIp_set);
EXPORT_SYMBOL(rtk_pie_rangeCheck_get);
EXPORT_SYMBOL(rtk_pie_rangeCheck_set);
EXPORT_SYMBOL(rtk_pie_fieldSelector_get);
EXPORT_SYMBOL(rtk_pie_fieldSelector_set);
EXPORT_SYMBOL(rtk_pie_meterIncludeIfg_get);
EXPORT_SYMBOL(rtk_pie_meterIncludeIfg_set);
EXPORT_SYMBOL(rtk_pie_meterExceed_get);
EXPORT_SYMBOL(rtk_pie_meterExceedAggregation_get);
EXPORT_SYMBOL(rtk_pie_meterEntry_get);
EXPORT_SYMBOL(rtk_pie_meterEntry_set);
EXPORT_SYMBOL(rtk_pie_templateVlanSel_get);
EXPORT_SYMBOL(rtk_pie_templateVlanSel_set);
EXPORT_SYMBOL(rtk_pie_meterDpSel_get);
EXPORT_SYMBOL(rtk_pie_meterDpSel_set);
EXPORT_SYMBOL(rtk_pie_arpMacSel_get);
EXPORT_SYMBOL(rtk_pie_arpMacSel_set);
EXPORT_SYMBOL(rtk_pie_intfSel_get);
EXPORT_SYMBOL(rtk_pie_intfSel_set);
EXPORT_SYMBOL(rtk_pie_templateVlanFmtSel_get);
EXPORT_SYMBOL(rtk_pie_templateVlanFmtSel_set);
EXPORT_SYMBOL(rtk_pie_meterTrtcmType_get);
EXPORT_SYMBOL(rtk_pie_meterTrtcmType_set);
EXPORT_SYMBOL(rtk_pie_filter1BR_get);
EXPORT_SYMBOL(rtk_pie_filter1BR_set);

EXPORT_SYMBOL(rtk_sec_init);
EXPORT_SYMBOL(rtk_sec_portAttackPrevent_get);
EXPORT_SYMBOL(rtk_sec_portAttackPrevent_set);
EXPORT_SYMBOL(rtk_sec_portAttackPreventEnable_get);
EXPORT_SYMBOL(rtk_sec_portAttackPreventEnable_set);
EXPORT_SYMBOL(rtk_sec_attackPreventAction_get);
EXPORT_SYMBOL(rtk_sec_attackPreventAction_set);
EXPORT_SYMBOL(rtk_sec_minIPv6FragLen_get);
EXPORT_SYMBOL(rtk_sec_minIPv6FragLen_set);
EXPORT_SYMBOL(rtk_sec_maxPingLen_get);
EXPORT_SYMBOL(rtk_sec_maxPingLen_set);
EXPORT_SYMBOL(rtk_sec_minTCPHdrLen_get);
EXPORT_SYMBOL(rtk_sec_minTCPHdrLen_set);
EXPORT_SYMBOL(rtk_sec_smurfNetmaskLen_get);
EXPORT_SYMBOL(rtk_sec_smurfNetmaskLen_set);
EXPORT_SYMBOL(rtk_sec_trapTarget_get);
EXPORT_SYMBOL(rtk_sec_trapTarget_set);
EXPORT_SYMBOL(rtk_sec_ipMacBindAction_get);
EXPORT_SYMBOL(rtk_sec_ipMacBindAction_set);
EXPORT_SYMBOL(rtk_sec_portIpMacBindEnable_get);
EXPORT_SYMBOL(rtk_sec_portIpMacBindEnable_set);
EXPORT_SYMBOL(rtk_sec_ipMacBindEntry_add);
EXPORT_SYMBOL(rtk_sec_ipMacBindEntry_del);
EXPORT_SYMBOL(rtk_sec_ipMacBindEntry_getNext);
EXPORT_SYMBOL(rtk_sec_attackPreventHit_get);

/* LED */
EXPORT_SYMBOL(rtk_led_init);
EXPORT_SYMBOL(rtk_led_sysEnable_get);
EXPORT_SYMBOL(rtk_led_sysEnable_set);
EXPORT_SYMBOL(rtk_led_portLedEntitySwCtrlEnable_get);
EXPORT_SYMBOL(rtk_led_portLedEntitySwCtrlEnable_set);
EXPORT_SYMBOL(rtk_led_swCtrl_start);
EXPORT_SYMBOL(rtk_led_portLedEntitySwCtrlMode_get);
EXPORT_SYMBOL(rtk_led_portLedEntitySwCtrlMode_set);
EXPORT_SYMBOL(rtk_led_sysMode_get);
EXPORT_SYMBOL(rtk_led_sysMode_set);
EXPORT_SYMBOL(rtk_led_blinkTime_get);
EXPORT_SYMBOL(rtk_led_blinkTime_set);

/* MPLS */
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310))
EXPORT_SYMBOL(rtk_mpls_init);
EXPORT_SYMBOL(rtk_mpls_ttlInherit_get);
EXPORT_SYMBOL(rtk_mpls_ttlInherit_set);
EXPORT_SYMBOL(rtk_mpls_enable_get);
EXPORT_SYMBOL(rtk_mpls_enable_set);
EXPORT_SYMBOL(rtk_mpls_trapTarget_get);
EXPORT_SYMBOL(rtk_mpls_trapTarget_set);
EXPORT_SYMBOL(rtk_mpls_exceptionCtrl_get);
EXPORT_SYMBOL(rtk_mpls_exceptionCtrl_set);
EXPORT_SYMBOL(rtk_mpls_nextHop_create);
EXPORT_SYMBOL(rtk_mpls_nextHop_destroy);
EXPORT_SYMBOL(rtk_mpls_nextHop_get);
EXPORT_SYMBOL(rtk_mpls_nextHop_set);
EXPORT_SYMBOL(rtk_mpls_encap_create);
EXPORT_SYMBOL(rtk_mpls_encap_destroy);
EXPORT_SYMBOL(rtk_mpls_encap_get);
EXPORT_SYMBOL(rtk_mpls_encap_set);
EXPORT_SYMBOL(rtk_mpls_encapId_find);
EXPORT_SYMBOL(rtk_mpls_hashAlgo_get);
EXPORT_SYMBOL(rtk_mpls_hashAlgo_set);
EXPORT_SYMBOL(rtk_mpls_decap_create);
EXPORT_SYMBOL(rtk_mpls_decap_destroy);
EXPORT_SYMBOL(rtk_mpls_decap_get);
EXPORT_SYMBOL(rtk_mpls_decap_set);
EXPORT_SYMBOL(rtk_mpls_decapId_find);
EXPORT_SYMBOL(rtk_mpls_egrTcMap_get);
EXPORT_SYMBOL(rtk_mpls_egrTcMap_set);
EXPORT_SYMBOL(rtk_mpls_nextHop_create_id);
EXPORT_SYMBOL(rtk_mpls_encap_create_id);
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)) */

#if defined(CONFIG_SDK_RTL9310)
/* Tunnel */
EXPORT_SYMBOL(rtk_tunnel_init);
EXPORT_SYMBOL(rtk_tunnel_info_t_init);
EXPORT_SYMBOL(rtk_tunnel_info_get);
EXPORT_SYMBOL(rtk_tunnel_intf_t_init);
EXPORT_SYMBOL(rtk_tunnel_intf_create);
EXPORT_SYMBOL(rtk_tunnel_intf_destroy);
EXPORT_SYMBOL(rtk_tunnel_intf_destroyAll);
EXPORT_SYMBOL(rtk_tunnel_intf_get);
EXPORT_SYMBOL(rtk_tunnel_intf_set);
EXPORT_SYMBOL(rtk_tunnel_intfPathId_get);
EXPORT_SYMBOL(rtk_tunnel_intfPathId_set);
EXPORT_SYMBOL(rtk_tunnel_intfPath_get);
EXPORT_SYMBOL(rtk_tunnel_intfPath_set);
EXPORT_SYMBOL(rtk_tunnel_intfStats_get);
EXPORT_SYMBOL(rtk_tunnel_intfStats_reset);
EXPORT_SYMBOL(rtk_tunnel_qosProfile_get);
EXPORT_SYMBOL(rtk_tunnel_qosProfile_set);
EXPORT_SYMBOL(rtk_tunnel_globalCtrl_get);
EXPORT_SYMBOL(rtk_tunnel_globalCtrl_set);
EXPORT_SYMBOL(rtk_capwap_udpPort_get);
EXPORT_SYMBOL(rtk_capwap_udpPort_set);

/* VXLAN */
EXPORT_SYMBOL(rtk_vxlan_init);
EXPORT_SYMBOL(rtk_vxlan_vni_add);
EXPORT_SYMBOL(rtk_vxlan_vni_del);
EXPORT_SYMBOL(rtk_vxlan_vni_delAll);
EXPORT_SYMBOL(rtk_vxlan_vni_get);
EXPORT_SYMBOL(rtk_vxlan_vni_set);
EXPORT_SYMBOL(rtk_vxlan_vni_getNext);
EXPORT_SYMBOL(rtk_vxlan_globalCtrl_get);
EXPORT_SYMBOL(rtk_vxlan_globalCtrl_set);

/* OpenFlow */
EXPORT_SYMBOL(rtk_of_init);
EXPORT_SYMBOL(rtk_of_classifier_get);
EXPORT_SYMBOL(rtk_of_classifier_set);
EXPORT_SYMBOL(rtk_of_flowMatchFieldSize_get);
EXPORT_SYMBOL(rtk_of_flowEntrySize_get);
EXPORT_SYMBOL(rtk_of_flowEntryValidate_get);
EXPORT_SYMBOL(rtk_of_flowEntryValidate_set);
EXPORT_SYMBOL(rtk_of_flowEntryFieldList_get);
EXPORT_SYMBOL(rtk_of_flowEntryField_check);
EXPORT_SYMBOL(rtk_of_flowEntryField_read);
EXPORT_SYMBOL(rtk_of_flowEntryField_write);
EXPORT_SYMBOL(rtk_of_flowEntryOperation_get);
EXPORT_SYMBOL(rtk_of_flowEntryOperation_set);
EXPORT_SYMBOL(rtk_of_flowEntryInstruction_get);
EXPORT_SYMBOL(rtk_of_flowEntryInstruction_set);
EXPORT_SYMBOL(rtk_of_flowEntrySetField_check);
EXPORT_SYMBOL(rtk_of_flowEntryHitSts_get);
EXPORT_SYMBOL(rtk_of_flowEntry_del);
EXPORT_SYMBOL(rtk_of_flowEntry_move);
EXPORT_SYMBOL(rtk_of_ftTemplateSelector_get);
EXPORT_SYMBOL(rtk_of_ftTemplateSelector_set);
EXPORT_SYMBOL(rtk_of_flowCntMode_get);
EXPORT_SYMBOL(rtk_of_flowCntMode_set);
EXPORT_SYMBOL(rtk_of_flowCnt_get);
EXPORT_SYMBOL(rtk_of_flowCnt_clear);
EXPORT_SYMBOL(rtk_of_flowCntThresh_get);
EXPORT_SYMBOL(rtk_of_flowCntThresh_set);
EXPORT_SYMBOL(rtk_of_ttlExcpt_get);
EXPORT_SYMBOL(rtk_of_ttlExcpt_set);
EXPORT_SYMBOL(rtk_of_maxLoopback_get);
EXPORT_SYMBOL(rtk_of_maxLoopback_set);
EXPORT_SYMBOL(rtk_of_l2FlowTblMatchField_get);
EXPORT_SYMBOL(rtk_of_l2FlowTblMatchField_set);
EXPORT_SYMBOL(rtk_of_l2FlowEntrySetField_check);
EXPORT_SYMBOL(rtk_of_l2FlowEntry_get);
EXPORT_SYMBOL(rtk_of_l2FlowEntryNextValid_get);
EXPORT_SYMBOL(rtk_of_l2FlowEntry_add);
EXPORT_SYMBOL(rtk_of_l2FlowEntry_del);
EXPORT_SYMBOL(rtk_of_l2FlowEntry_delAll);
EXPORT_SYMBOL(rtk_of_l2FlowEntryHitSts_get);
EXPORT_SYMBOL(rtk_of_l2FlowTblHashAlgo_get);
EXPORT_SYMBOL(rtk_of_l2FlowTblHashAlgo_set);
EXPORT_SYMBOL(rtk_of_l3FlowTblPri_get);
EXPORT_SYMBOL(rtk_of_l3FlowTblPri_set);
EXPORT_SYMBOL(rtk_of_l3CamFlowTblMatchField_get);
EXPORT_SYMBOL(rtk_of_l3CamFlowTblMatchField_set);
EXPORT_SYMBOL(rtk_of_l3HashFlowTblMatchField_get);
EXPORT_SYMBOL(rtk_of_l3HashFlowTblMatchField_set);
EXPORT_SYMBOL(rtk_of_l3HashFlowTblHashAlgo_get);
EXPORT_SYMBOL(rtk_of_l3HashFlowTblHashAlgo_set);
EXPORT_SYMBOL(rtk_of_l3FlowEntrySetField_check);
EXPORT_SYMBOL(rtk_of_l3CamFlowEntry_get);
EXPORT_SYMBOL(rtk_of_l3CamFlowEntry_add);
EXPORT_SYMBOL(rtk_of_l3CamFlowEntry_del);
EXPORT_SYMBOL(rtk_of_l3CamFlowEntry_move);
EXPORT_SYMBOL(rtk_of_l3CamflowEntryHitSts_get);
EXPORT_SYMBOL(rtk_of_l3HashFlowEntry_get);
EXPORT_SYMBOL(rtk_of_l3HashFlowEntryNextValid_get);
EXPORT_SYMBOL(rtk_of_l3HashFlowEntry_add);
EXPORT_SYMBOL(rtk_of_l3HashFlowEntry_del);
EXPORT_SYMBOL(rtk_of_l3HashFlowEntry_delAll);
EXPORT_SYMBOL(rtk_of_l3HashflowEntryHitSts_get);
EXPORT_SYMBOL(rtk_of_groupEntry_get);
EXPORT_SYMBOL(rtk_of_groupEntry_set);
EXPORT_SYMBOL(rtk_of_groupTblHashPara_get);
EXPORT_SYMBOL(rtk_of_groupTblHashPara_set);
EXPORT_SYMBOL(rtk_of_actionBucket_get);
EXPORT_SYMBOL(rtk_of_actionBucket_set);
EXPORT_SYMBOL(rtk_of_trapTarget_get);
EXPORT_SYMBOL(rtk_of_trapTarget_set);
EXPORT_SYMBOL(rtk_of_tblMissAction_get);
EXPORT_SYMBOL(rtk_of_tblMissAction_set);
EXPORT_SYMBOL(rtk_of_flowTblCnt_get);
#endif

/* Stacking */
EXPORT_SYMBOL(rtk_stack_init);
EXPORT_SYMBOL(rtk_stack_cascadeMode_init);
EXPORT_SYMBOL(rtk_stack_port_get);
EXPORT_SYMBOL(rtk_stack_port_set);
EXPORT_SYMBOL(rtk_stack_devId_get);
EXPORT_SYMBOL(rtk_stack_devId_set);
EXPORT_SYMBOL(rtk_stack_masterDevId_get);
EXPORT_SYMBOL(rtk_stack_masterDevId_set);
EXPORT_SYMBOL(rtk_stack_loopGuard_get);
EXPORT_SYMBOL(rtk_stack_loopGuard_set);
EXPORT_SYMBOL(rtk_stack_devPortMap_get);
EXPORT_SYMBOL(rtk_stack_devPortMap_set);
EXPORT_SYMBOL(rtk_stack_nonUcastBlockPort_get);
EXPORT_SYMBOL(rtk_stack_nonUcastBlockPort_set);
EXPORT_SYMBOL(rtk_stack_rmtIntrTxEnable_get);
EXPORT_SYMBOL(rtk_stack_rmtIntrTxEnable_set);
EXPORT_SYMBOL(rtk_stack_rmtIntrTxTriggerEnable_get);
EXPORT_SYMBOL(rtk_stack_rmtIntrTxTriggerEnable_set);
EXPORT_SYMBOL(rtk_stack_rmtIntrRxSeqCmpMargin_get);
EXPORT_SYMBOL(rtk_stack_rmtIntrRxSeqCmpMargin_set);
EXPORT_SYMBOL(rtk_stack_rmtIntrRxForceUpdateEnable_get);
EXPORT_SYMBOL(rtk_stack_rmtIntrRxForceUpdateEnable_set);
EXPORT_SYMBOL(rtk_stack_rmtIntrInfo_get);
EXPORT_SYMBOL(rtk_stack_shrink_get);
EXPORT_SYMBOL(rtk_stack_shrink_set);


/* DIAG */
EXPORT_SYMBOL(rtk_diag_init);
EXPORT_SYMBOL(rtk_diag_portRtctResult_get);
EXPORT_SYMBOL(rtk_diag_rtctEnable_set);
EXPORT_SYMBOL(rtk_diag_table_whole_read);
EXPORT_SYMBOL(rtk_diag_tableEntry_read);
EXPORT_SYMBOL(rtk_diag_tableEntryDatareg_write);
EXPORT_SYMBOL(rtk_diag_tableEntry_write);
EXPORT_SYMBOL(rtk_diag_reg_whole_read);
EXPORT_SYMBOL(rtk_diag_peripheral_register_dump);
EXPORT_SYMBOL(rtk_diag_phy_reg_whole_read);
EXPORT_SYMBOL(rtk_diag_table_index_name);

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
EXPORT_SYMBOL(rtk_diag_table_reg_field_get);
EXPORT_SYMBOL(rtk_diag_table_reg_field_set);
EXPORT_SYMBOL(table_field_mac_set);
EXPORT_SYMBOL(table_field_mac_get);
#endif
// REG get
EXPORT_SYMBOL(rtk_diag_reg_get);
EXPORT_SYMBOL(rtk_diag_regField_get);
EXPORT_SYMBOL(rtk_diag_regArray_get);
EXPORT_SYMBOL(rtk_diag_regArrayField_get);

// REG set
EXPORT_SYMBOL(rtk_diag_reg_set);
EXPORT_SYMBOL(rtk_diag_regField_set);
EXPORT_SYMBOL(rtk_diag_regArray_set);
EXPORT_SYMBOL(rtk_diag_regArrayField_set);

// REG/Field info get
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
EXPORT_SYMBOL(rtk_diag_regInfoByStr_get);
EXPORT_SYMBOL(rtk_diag_regFieldInfo_get);
EXPORT_SYMBOL(rtk_diag_regInfoByStr_match);
#endif

#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
EXPORT_SYMBOL(rtk_diag_tableInfoByStr_get);
EXPORT_SYMBOL(rtk_diag_tableInfoByStr_match);
EXPORT_SYMBOL(rtk_diag_tableFieldInfo_get);
#endif
EXPORT_SYMBOL(rtk_diag_tableEntry_get);
EXPORT_SYMBOL(rt_util_tblEntry2Field);
EXPORT_SYMBOL(rt_util_field2TblEntry);
EXPORT_SYMBOL(rtk_diag_tableEntry_set);
EXPORT_SYMBOL(rt_util_serdesTxEyeParam_scan);

EXPORT_SYMBOL(reg_idx2Addr_get);
EXPORT_SYMBOL(reg_idxMax_get);
EXPORT_SYMBOL(reg_info_get);
EXPORT_SYMBOL(rt_error_numToStr);
EXPORT_SYMBOL(reg_read);
EXPORT_SYMBOL(reg_write);
EXPORT_SYMBOL(table_find);
EXPORT_SYMBOL(table_write);
EXPORT_SYMBOL(table_read);


EXPORT_SYMBOL(hal_dumpHsb);
EXPORT_SYMBOL(hal_dumpHsa);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(hal_dumpHsm);
#endif
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
EXPORT_SYMBOL(hal_dumpHsmIdx);
#endif
#if defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(hal_dumpHsb_openflow);
EXPORT_SYMBOL(hal_dumpHsm_openflow);
EXPORT_SYMBOL(hal_dumpHsa_openflow);
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)||defined(CONFIG_SDK_RTL9310)||defined(CONFIG_SDK_RTL9300)
EXPORT_SYMBOL(hal_getDbgCntr);
#endif
#if defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(hal_getDbgEncapCntr);
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(hal_resetFlowCtrlIgrPortUsedPageCnt);
EXPORT_SYMBOL(hal_resetFlowCtrlEgrPortUsedPageCnt);
EXPORT_SYMBOL(hal_resetFlowCtrlSystemUsedPageCnt);
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
EXPORT_SYMBOL(hal_getFlowCtrlIgrPortUsedPageCnt);
EXPORT_SYMBOL(hal_getFlowCtrlEgrPortUsedPageCnt);
EXPORT_SYMBOL(hal_getFlowCtrlSystemUsedPageCnt);
EXPORT_SYMBOL(hal_getFlowCtrlPortQueueUsedPageCnt);
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)|| defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(hal_getWatchdogCnt);
EXPORT_SYMBOL(hal_setWatchdogMonitorEnable);
#endif
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
EXPORT_SYMBOL(hal_getFlowCtrlSystemPublicResource_get);
EXPORT_SYMBOL(hal_getFlowCtrlSystemPublicUsedPageCnt);
#endif
#if defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(hal_getFlowCtrlSystemIgrQueueUsedPageCnt);
EXPORT_SYMBOL(hal_getFlowCtrlRepctQueueCntrInfo);
EXPORT_SYMBOL(hal_repctQueueEmptyStatus_get);
EXPORT_SYMBOL(hal_repctQueueStickEnable_get);
EXPORT_SYMBOL(hal_repctQueueStickEnable_set);
EXPORT_SYMBOL(hal_repctQueueFetchEnable_get);
EXPORT_SYMBOL(hal_repctQueueFetchEnable_set);
EXPORT_SYMBOL(hal_resetFlowCtrlRepctQueueUsedPageCnt);
EXPORT_SYMBOL(hal_getFlowCtrlPortIgrQueueUsedPageCnt);
#endif

#if defined(CONFIG_SDK_MODEL_MODE)
/* for model & testcase */
EXPORT_SYMBOL(reg_field_get);
EXPORT_SYMBOL(reg_field_set);
//EXPORT_SYMBOL(reg_array_read);
//EXPORT_SYMBOL(reg_array_write);
#endif

#if defined(CONFIG_SDK_DRIVER_TEST_MODULE) || defined(CONFIG_SDK_DRIVER_TEST)
EXPORT_SYMBOL(hal_init);
EXPORT_SYMBOL(phy_speed_get);
EXPORT_SYMBOL(phy_reg_get);
EXPORT_SYMBOL(hal_find_device);
EXPORT_SYMBOL(phy_identify_find);
//EXPORT_SYMBOL(table_size_get);
//EXPORT_SYMBOL(reg_write);

EXPORT_SYMBOL(reg_field_get);
//EXPORT_SYMBOL(table_field_set);
EXPORT_SYMBOL(hal_isPpBlock_check);
EXPORT_SYMBOL(reg_field_set);
EXPORT_SYMBOL(table_field_byte_get);
//EXPORT_SYMBOL(reg_array_field_write);
EXPORT_SYMBOL(phy_reg_set);
//EXPORT_SYMBOL(table_field_get);
EXPORT_SYMBOL(phy_media_get);
EXPORT_SYMBOL(phy_duplex_get);
EXPORT_SYMBOL(hal_get_driver_id);
//EXPORT_SYMBOL(reg_read);
EXPORT_SYMBOL(phy_enable_set);
EXPORT_SYMBOL(phy_speed_set);
EXPORT_SYMBOL(reg_array_read);
EXPORT_SYMBOL(hal_portMaxBandwidth_ret);
//EXPORT_SYMBOL(reg_field_write);
EXPORT_SYMBOL(phy_identify_OUI_check);
EXPORT_SYMBOL(table_field_byte_set);
//EXPORT_SYMBOL(reg_field_read);
//EXPORT_SYMBOL(reg_array_field_read);
//EXPORT_SYMBOL(reg_array_write);
EXPORT_SYMBOL(phy_autoNegoEnable_get);
EXPORT_SYMBOL(hal_ctrlInfo_get);
EXPORT_SYMBOL(hal_find_driver);
EXPORT_SYMBOL(phy_autoNegoAbility_set);
EXPORT_SYMBOL(phy_duplex_set);
EXPORT_SYMBOL(phy_identify_phyid_get);
EXPORT_SYMBOL(phy_autoNegoAbility_get);

EXPORT_SYMBOL(phy_autoNegoEnable_set);
EXPORT_SYMBOL(phy_media_set);
EXPORT_SYMBOL(phy_reg_portmask_set);

EXPORT_SYMBOL(phy_broadcastEnable_set);
EXPORT_SYMBOL(phy_broadcastID_set);
EXPORT_SYMBOL(phy_reg_broadcast_set);

#endif

//#if defined(CONFIG_SDK_APP_DIAG_EXT)
EXPORT_SYMBOL(reg_field_read);
EXPORT_SYMBOL(reg_field_write);
EXPORT_SYMBOL(reg_array_read);
EXPORT_SYMBOL(reg_array_write);
EXPORT_SYMBOL(reg_array_field_read);
EXPORT_SYMBOL(reg_array_field_write);
EXPORT_SYMBOL(table_field_set);
EXPORT_SYMBOL(table_field_get);
EXPORT_SYMBOL(table_size_get);
//#endif

EXPORT_SYMBOL(bitop_numberOfSetBitsInArray);
EXPORT_SYMBOL(rt_bitop_findFirstBit);
EXPORT_SYMBOL(rt_bitop_findFirstBitInAaray);
EXPORT_SYMBOL(rt_bitop_findFirstZeroBitInAaray);
EXPORT_SYMBOL(rt_bitop_findLastBitInAaray);
EXPORT_SYMBOL(rt_bitop_findIdxBitInAaray);


EXPORT_SYMBOL(hal_serdes_reg_get);
EXPORT_SYMBOL(hal_serdes_reg_set);


/* Data */
EXPORT_SYMBOL(hal_ctrl);
#if defined(CONFIG_SDK_RTL8390)
EXPORT_SYMBOL(rtl8390_a_driver);
#endif
#if defined(CONFIG_SDK_RTL8380)
EXPORT_SYMBOL(rtl8380_a_driver);
#endif
#if defined(CONFIG_SDK_RTL9300)
EXPORT_SYMBOL(rtl9300_a_driver);
#endif
#if defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(rtl9310_a_driver);
#endif


#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
 EXPORT_SYMBOL(reg_model_write_register);
 EXPORT_SYMBOL(reg_model_array_read_register);
 #endif
#endif

EXPORT_SYMBOL(rtk_swled_on);
EXPORT_SYMBOL(rtk_boxID_led_set);
EXPORT_SYMBOL(rtk_boxID_led_init);
EXPORT_SYMBOL(rtk_masterLedEnable_set);


#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
  EXPORT_SYMBOL(hal_rtl8295_reg_read);
  EXPORT_SYMBOL(hal_rtl8295_reg_write);
  EXPORT_SYMBOL(hal_rtl8295_sds_read);
  EXPORT_SYMBOL(hal_rtl8295_sds_write);
  EXPORT_SYMBOL(phy_8295_diag_set);
  EXPORT_SYMBOL(phy_8295_patch_debugEnable_set);
#endif /* defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) */

#if defined(CONFIG_SDK_RTL8295R)
  EXPORT_SYMBOL(phy_8295r_10gMedia_set);
  EXPORT_SYMBOL(phy_8295r_portSdsRxCaliEnable_set);
  EXPORT_SYMBOL(phy_8295r_rxCaliConfPort_set);
  EXPORT_SYMBOL(phy_8295r_rxCaliConfPort_get);
#endif/* CONFIG_SDK_RTL8295R */
#if defined(CONFIG_SDK_RTL8214QF)
  EXPORT_SYMBOL(phy_8214qf_rxCaliConf_set);
  EXPORT_SYMBOL(phy_8214qf_rxCaliConf_get);
#endif/* CONFIG_SDK_RTL8214QF */

#if defined(CONFIG_SDK_RTL8390)
  EXPORT_SYMBOL(phy_8390_10gMedia_set);
  EXPORT_SYMBOL(phy_ptpRefTime_get);
  EXPORT_SYMBOL(phy_ptpIgrTpid_get);
  EXPORT_SYMBOL(phy_ptpIgrTpid_set);
  EXPORT_SYMBOL(phy_ptpSwitchMacAddr_get);
  EXPORT_SYMBOL(phy_ptpSwitchMacAddr_set);
  EXPORT_SYMBOL(phy_ptpSwitchMacRange_get);
  EXPORT_SYMBOL(phy_ptpSwitchMacRange_set);
  EXPORT_SYMBOL(phy_ptpRefTimeFreq_get);
  EXPORT_SYMBOL(phy_ptpRefTimeFreq_set);
  EXPORT_SYMBOL(phy_ptpInterruptStatus_get);
  EXPORT_SYMBOL(phy_ptpInterruptEnable_get);
  EXPORT_SYMBOL(phy_ptpInterruptEnable_set);
#endif

  EXPORT_SYMBOL(phy_field_read);
  EXPORT_SYMBOL(phy_field_write);

EXPORT_SYMBOL(_dal_phy_debugCmd_set);
EXPORT_SYMBOL(_dal_phy_debug_cmd);

/* SerDes */
EXPORT_SYMBOL(rtk_sds_symErr_get);
EXPORT_SYMBOL(rtk_sds_symErr_clear);
EXPORT_SYMBOL(rtk_sds_linkSts_get);
EXPORT_SYMBOL(rtk_sds_testModeCnt_get);
EXPORT_SYMBOL(rtk_sds_testMode_set);
EXPORT_SYMBOL(rtk_sds_rx_rst);
EXPORT_SYMBOL(rtk_sds_leq_adapt);
EXPORT_SYMBOL(rtk_sds_leq_get);
EXPORT_SYMBOL(rtk_sds_leq_set);
EXPORT_SYMBOL(rtk_sds_xsgNwayEn_set);
EXPORT_SYMBOL(rtk_sds_cmuBand_get);
EXPORT_SYMBOL(rtk_sds_cmuBand_set);
EXPORT_SYMBOL(rtk_sds_eyeMonitor_start);
EXPORT_SYMBOL(rtk_sds_eyeParam_get);
EXPORT_SYMBOL(rtk_sds_eyeParam_set);
EXPORT_SYMBOL(rtk_sds_rxCaliConf_get);
EXPORT_SYMBOL(rtk_sds_rxCaliConf_set);
EXPORT_SYMBOL(rtk_sds_info_get);

EXPORT_SYMBOL(mac_debug_sds_rxCali);
EXPORT_SYMBOL(mac_debug_sds_rxCaliEnable_get);
EXPORT_SYMBOL(mac_debug_sds_rxCaliStatus_get);
EXPORT_SYMBOL(mac_debug_sds_rxCaliEnable_set);
EXPORT_SYMBOL(mac_debug_sds_rxCali_debugEnable_set);

EXPORT_SYMBOL(rtk_sds_eyeMonitorInfo_get);
EXPORT_SYMBOL(rtk_sds_loopback_get);
EXPORT_SYMBOL(rtk_sds_loopback_set);
EXPORT_SYMBOL(rtk_sds_ctrl_get);
EXPORT_SYMBOL(rtk_sds_ctrl_set);
EXPORT_SYMBOL(rtk_port_phyCtrl_get);
EXPORT_SYMBOL(rtk_port_phyCtrl_set);
EXPORT_SYMBOL(rtk_port_phyLiteEnable_get);
EXPORT_SYMBOL(rtk_port_phyLiteEnable_set);

EXPORT_SYMBOL(rtk_port_macsecReg_get);
EXPORT_SYMBOL(rtk_port_macsecReg_set);
EXPORT_SYMBOL(rtk_macsec_port_cfg_set);
EXPORT_SYMBOL(rtk_macsec_port_cfg_get);
EXPORT_SYMBOL(rtk_macsec_sc_create);
EXPORT_SYMBOL(rtk_macsec_sc_get);
EXPORT_SYMBOL(rtk_macsec_sc_del);
EXPORT_SYMBOL(rtk_macsec_sc_status_get);
EXPORT_SYMBOL(rtk_macsec_sa_create);
EXPORT_SYMBOL(rtk_macsec_sa_get);
EXPORT_SYMBOL(rtk_macsec_sa_del);
EXPORT_SYMBOL(rtk_macsec_sa_activate);
EXPORT_SYMBOL(rtk_macsec_rxsa_disable);
EXPORT_SYMBOL(rtk_macsec_txsa_disable);
EXPORT_SYMBOL(rtk_macsec_stat_clear);
EXPORT_SYMBOL(rtk_macsec_stat_port_get);
EXPORT_SYMBOL(rtk_macsec_stat_txsa_get);
EXPORT_SYMBOL(rtk_macsec_stat_rxsa_get);
EXPORT_SYMBOL(rtk_macsec_intr_status_get);

#ifdef CONFIG_RISE
EXPORT_SYMBOL(rtrpc_txFunc_reg);
EXPORT_SYMBOL(rtrpc_unitIdTranslateFunc_reg);
#endif

