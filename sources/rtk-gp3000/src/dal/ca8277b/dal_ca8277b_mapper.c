/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <dal/dal_mapper.h>
#include <dal/dal_common.h>
#include <dal/ca8277b/dal_ca8277b_mapper.h>

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/kernel.h>
#include <linux/string.h>
#else
#include <string.h>
#endif


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static dal_mapper_t dal_ca8277b_mapper =
{
    0,//Kem TBD
    ._init = NULL,

    /*sec*/
    .sec_init = NULL,
    .sec_portAttackPreventState_get = NULL,
    .sec_portAttackPreventState_set = NULL,
    .sec_attackPrevent_get = NULL,
    .sec_attackPrevent_set = NULL,
    .sec_attackFloodThresh_get = NULL,
    .sec_attackFloodThresh_set = NULL,
    .sec_attackFloodThreshUnit_get = NULL,
    .sec_attackFloodThreshUnit_set = NULL,

    /* statistics */
    .stat_init                                  = NULL,
    .stat_global_reset                          = NULL,
    .stat_port_reset                            = NULL,
    .stat_log_reset                             = NULL,
    .stat_hostCnt_reset                         = NULL,
    .stat_hostCnt_get                           = NULL,
    .stat_hostState_get                         = NULL,
    .stat_hostState_set                         = NULL,
    .stat_rst_cnt_value_set                     = NULL,
    .stat_rst_cnt_value_get                     = NULL,
    .stat_global_get                            = NULL,
    .stat_global_getAll                         = NULL,
    .stat_port_get                              = NULL,
    .stat_port_getAll                           = NULL,
    .stat_log_get                               = NULL,
    .stat_log_ctrl_set                          = NULL,
    .stat_log_ctrl_get                          = NULL,
    .stat_mib_cnt_mode_get                      = NULL,
    .stat_mib_cnt_mode_set                      = NULL,
    .stat_mib_latch_timer_get                   = NULL,
    .stat_mib_latch_timer_set                   = NULL,
    .stat_mib_sync_mode_get                     = NULL,
    .stat_mib_sync_mode_set                     = NULL,
    .stat_pktInfo_get                           = NULL,


    /* Switch */
    .switch_init = NULL,
    .switch_phyPortId_get = NULL,
    .switch_logicalPort_get = NULL,
    .switch_port2PortMask_set = NULL,
    .switch_port2PortMask_clear = NULL,
    .switch_portIdInMask_check = NULL,
    .switch_maxPktLenLinkSpeed_get = NULL,
    .switch_maxPktLenLinkSpeed_set = NULL,
    .switch_mgmtMacAddr_get = NULL,
    .switch_mgmtMacAddr_set = NULL,
    .switch_chip_reset = NULL,
    .switch_version_get = NULL,
    .switch_patch_info_get = NULL,
    .switch_csExtId_get = NULL,
    .switch_maxPktLenByPort_get = NULL,
    .switch_maxPktLenByPort_set = NULL,
    .switch_changeDuplex_get = NULL,
    .switch_changeDuplex_set = NULL,
    .switch_system_init = NULL,

    /* Port */
    .port_init                                  = NULL,
    .port_link_get                              = NULL,
    .port_speedDuplex_get                       = NULL,
    .port_flowctrl_get                          = NULL,
    .port_phyAutoNegoEnable_get                 = NULL,
    .port_phyAutoNegoEnable_set                 = NULL,
    .port_phyAutoNegoAbility_get                = NULL,
    .port_phyAutoNegoAbility_set                = NULL,
    .port_phyForceModeAbility_get               = NULL,
    .port_phyForceModeAbility_set               = NULL,
    .port_phyMasterSlave_get                    = NULL,
    .port_phyMasterSlave_set                    = NULL,
    .port_phyTestMode_get                       = NULL,
    .port_phyTestMode_set                       = NULL,
    .port_phyReg_get                            = NULL,
    .port_phyReg_set                            = NULL,
    .port_cpuPortId_get                         = NULL,
    .port_isolation_get                         = NULL,
    .port_isolation_set                         = NULL,
    .port_isolationExt_get                      = NULL,
    .port_isolationExt_set                      = NULL,
    .port_isolationL34_get                      = NULL,
    .port_isolationL34_set                      = NULL,
    .port_isolationExtL34_get                   = NULL,
    .port_isolationExtL34_set                   = NULL,
    .port_isolationEntry_get                    = NULL,
    .port_isolationEntry_set                    = NULL,
    .port_isolationEntryExt_get                 = NULL,
    .port_isolationEntryExt_set                 = NULL,
    .port_isolationCtagPktConfig_get            = NULL,
    .port_isolationCtagPktConfig_set            = NULL,
    .port_isolationL34PktConfig_get             = NULL,
    .port_isolationL34PktConfig_set             = NULL,
    .port_isolationIpmcLeaky_get                = NULL,
    .port_isolationIpmcLeaky_set                = NULL,
    .port_isolationPortLeaky_get                = NULL,
    .port_isolationPortLeaky_set                = NULL,
    .port_isolationLeaky_get                    = NULL,
    .port_isolationLeaky_set                    = NULL,
    .port_macRemoteLoopbackEnable_get           = NULL,
    .port_macRemoteLoopbackEnable_set           = NULL,
    .port_macLocalLoopbackEnable_get            = NULL,
    .port_macLocalLoopbackEnable_set            = NULL,
    .port_adminEnable_get                       = NULL,
    .port_adminEnable_set                       = NULL,
    .port_specialCongest_get                    = NULL,
    .port_specialCongest_set                    = NULL,
    .port_specialCongestStatus_get              = NULL,
    .port_specialCongestStatus_clear            = NULL,
    .port_greenEnable_get                       = NULL,
    .port_greenEnable_set                       = NULL,
    .port_phyCrossOverMode_get                  = NULL,
    .port_phyCrossOverMode_set                  = NULL,
    .port_phyPowerDown_set                      = NULL,
    .port_phyPowerDown_get                      = NULL,
    .port_enhancedFid_get                       = NULL,
    .port_enhancedFid_set                       = NULL,
    .port_rtctResult_get                        = NULL,
    .port_rtct_start                            = NULL,
    .port_macForceAbility_set                   = NULL,
    .port_macForceAbility_get                   = NULL,
    .port_macForceAbilityState_set              = NULL,
    .port_macForceAbilityState_get              = NULL,
    .port_macExtMode_set                        = NULL,
    .port_macExtMode_get                        = NULL,
    .port_macExtRgmiiDelay_set                  = NULL,
    .port_macExtRgmiiDelay_get                  = NULL,
    .port_gigaLiteEnable_set                    = NULL,
    .port_gigaLiteEnable_get                    = NULL,
    .port_serdesMode_set                        = NULL,
    .port_serdesMode_get                        = NULL,
    .port_serdesNWay_set                        = NULL,
    .port_serdesNWay_get                        = NULL,

    /*oam*/
    .oam_init = NULL,
    .oam_parserAction_set = NULL,
    .oam_parserAction_get = NULL,
    .oam_multiplexerAction_set = NULL,
    .oam_multiplexerAction_get = NULL,

    /* Trap */
    .trap_init                                  = NULL,
    .trap_reasonTrapToCpuPriority_get           = NULL,
    .trap_reasonTrapToCpuPriority_set           = NULL,
    .trap_igmpCtrlPkt2CpuEnable_get             = NULL,
    .trap_igmpCtrlPkt2CpuEnable_set             = NULL,
    .trap_mldCtrlPkt2CpuEnable_get              = NULL,
    .trap_mldCtrlPkt2CpuEnable_set              = NULL,
    .trap_portIgmpMldCtrlPktAction_get          = NULL,
    .trap_portIgmpMldCtrlPktAction_set          = NULL,
    .trap_l2McastPkt2CpuEnable_get              = NULL,
    .trap_l2McastPkt2CpuEnable_set              = NULL,
    .trap_ipMcastPkt2CpuEnable_get              = NULL,
    .trap_ipMcastPkt2CpuEnable_set              = NULL,
    .trap_rmaAction_get                         = NULL,
    .trap_rmaAction_set                         = NULL,
    .trap_rmaPri_get                            = NULL,
    .trap_rmaPri_set                            = NULL,
    .trap_portOamPduAction_get                  = NULL,
    .trap_portOamPduAction_set                  = NULL,
    .trap_oamPduPri_get                         = NULL,
    .trap_oamPduPri_set                         = NULL,
    .trap_uniTrapPriorityEnable_set             = NULL,
    .trap_uniTrapPriorityEnable_get             = NULL,
    .trap_uniTrapPriorityPriority_set           = NULL,
    .trap_uniTrapPriorityPriority_get           = NULL,
    .trap_cpuTrapHashMask_set                   = NULL,
    .trap_cpuTrapHashMask_get                   = NULL,
    .trap_cpuTrapHashPort_set                   = NULL,
    .trap_cpuTrapHashPort_get                   = NULL,
    .trap_cpuTrapHashState_set                  = NULL,
    .trap_cpuTrapHashState_get                  = NULL,
    .trap_cpuTrapInicHashState_set              = NULL,
    .trap_cpuTrapInicHashState_get              = NULL,
    .trap_cpuTrapInicHashSelect_set             = NULL,
    .trap_cpuTrapInicHashSelect_get             = NULL,

    /*SVLAN*/
    .svlan_init = NULL,
    .svlan_create = NULL,
    .svlan_destroy = NULL,
    .svlan_portSvid_get = NULL,
    .svlan_portSvid_set = NULL,
    .svlan_servicePort_get = NULL,
    .svlan_servicePort_set = NULL,
    .svlan_memberPort_set = NULL,
    .svlan_memberPort_get = NULL,
    .svlan_tpidEntry_get = NULL,
    .svlan_tpidEntry_set = NULL,
    .svlan_priorityRef_set = NULL,
    .svlan_priorityRef_get = NULL,
    .svlan_sp2c_add = NULL,
    .svlan_sp2c_get = NULL,
    .svlan_sp2c_del = NULL,
    .svlan_untagAction_set = NULL,
    .svlan_untagAction_get = NULL,
    .svlan_trapPri_get                          = NULL,
    .svlan_trapPri_set                          = NULL,
    .svlan_deiKeepState_get                     = NULL,
    .svlan_deiKeepState_set                     = NULL,
    .svlan_svlanFunctionEnable_get              = NULL,
    .svlan_svlanFunctionEnable_set              = NULL,
    .svlan_tpidEnable_get                       = NULL,
    .svlan_tpidEnable_set                       = NULL,
    .svlan_sp2cPriority_add                     = NULL,
    .svlan_sp2cPriority_get                     = NULL,
    .svlan_extPortPvid_get                      = NULL,
    .svlan_extPortPvid_set                      = NULL,

     /* VLAN */
    .vlan_init                                  = NULL,
    .vlan_create                                = NULL,
    .vlan_destroy                               = NULL,
    .vlan_destroyAll                            = NULL,
    .vlan_fid_get                               = NULL,
    .vlan_fid_set                               = NULL,
    .vlan_fidMode_get                           = NULL,
    .vlan_fidMode_set                           = NULL,
    .vlan_port_get                              = NULL,
    .vlan_port_set                              = NULL,
    .vlan_stg_get                               = NULL,
    .vlan_stg_set                               = NULL,
    .vlan_portAcceptFrameType_get               = NULL,
    .vlan_portAcceptFrameType_set               = NULL,
    .vlan_vlanFunctionEnable_get                = NULL,
    .vlan_vlanFunctionEnable_set                = NULL,
    .vlan_portIgrFilterEnable_get               = NULL,
    .vlan_portIgrFilterEnable_set               = NULL,
    .vlan_leaky_get                             = NULL,
    .vlan_leaky_set                             = NULL,
    .vlan_portLeaky_get                         = NULL,
    .vlan_portLeaky_set                         = NULL,
    .vlan_keepType_get                          = NULL,
    .vlan_keepType_set                          = NULL,
    .vlan_portPvid_get                          = NULL,
    .vlan_portPvid_set                          = NULL,
    .vlan_extPortPvid_get                       = NULL,
    .vlan_extPortPvid_set                       = NULL,
    .vlan_protoGroup_get                        = NULL,
    .vlan_protoGroup_set                        = NULL,
    .vlan_portProtoVlan_get                     = NULL,
    .vlan_portProtoVlan_set                     = NULL,
    .vlan_tagMode_get                           = NULL,
    .vlan_tagMode_set                           = NULL,
    .vlan_cfiKeepEnable_get                     = NULL,
    .vlan_cfiKeepEnable_set                     = NULL,
    .vlan_reservedVidAction_get                 = NULL,
    .vlan_reservedVidAction_set                 = NULL,
    .vlan_tagModeIp4mc_get                      = NULL,
    .vlan_tagModeIp4mc_set                      = NULL,
    .vlan_tagModeIp6mc_get                      = NULL,
    .vlan_tagModeIp6mc_set                      = NULL,
    .vlan_lutSvlanHashState_set                 = NULL,
    .vlan_lutSvlanHashState_get                 = NULL,
    .vlan_extPortProtoVlan_set                  = NULL,
    .vlan_extPortProtoVlan_get                  = NULL,
    .vlan_extPortmaskIndex_set                  = NULL,
    .vlan_extPortmaskIndex_get                  = NULL,
    .vlan_extPortmaskCfg_set                    = NULL,
    .vlan_extPortmaskCfg_get                    = NULL,

    /* Mirror */
    .mirror_init                                = NULL,
    .mirror_portBased_set                       = NULL,
    .mirror_portBased_get                       = NULL,
    .mirror_portIso_set                         = NULL,
    .mirror_portIso_get                         = NULL,

    /* STP */
    .stp_init = NULL,
    .stp_mstpState_get = NULL,
    .stp_mstpState_set = NULL,

    /*rate*/
    .rate_init = NULL,
    .rate_portIgrBandwidthCtrlRate_get = NULL,
    .rate_portIgrBandwidthCtrlRate_set = NULL,
    .rate_portIgrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_portIgrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_portEgrBandwidthCtrlRate_get = NULL,
    .rate_portEgrBandwidthCtrlRate_set = NULL,
    .rate_egrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_egrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_portEgrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_portEgrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_egrQueueBwCtrlEnable_get = NULL,
    .rate_egrQueueBwCtrlEnable_set = NULL,
    .rate_egrQueueBwCtrlMeterIdx_get = NULL,
    .rate_egrQueueBwCtrlMeterIdx_set = NULL,
    .rate_egrQueueBwCtrlRate_get = NULL,
    .rate_egrQueueBwCtrlRate_set = NULL,
    .rate_stormControlMeterIdx_get = NULL,
    .rate_stormControlMeterIdx_set = NULL,
    .rate_stormControlPortEnable_get = NULL,
    .rate_stormControlPortEnable_set = NULL,
    .rate_stormControlEnable_get = NULL,
    .rate_stormControlEnable_set = NULL,
    .rate_stormBypass_set = NULL,
    .rate_stormBypass_get = NULL,
    .rate_shareMeter_set = NULL, 
    .rate_shareMeter_get = NULL, 
    .rate_shareMeterBucket_set = NULL,
    .rate_shareMeterBucket_get = NULL,
    .rate_shareMeterExceed_get = NULL,
    .rate_shareMeterExceed_clear = NULL,
    .rate_shareMeterMode_set = NULL,
    .rate_shareMeterMode_get = NULL,
    .rate_hostIgrBwCtrlState_set = NULL,
    .rate_hostIgrBwCtrlState_get = NULL,
    .rate_hostEgrBwCtrlState_set = NULL,
    .rate_hostEgrBwCtrlState_get = NULL,
    .rate_hostBwCtrlMeterIdx_set = NULL,
    .rate_hostBwCtrlMeterIdx_get = NULL,
    .rate_hostMacAddr_set = NULL,
    .rate_hostMacAddr_get = NULL,
    .rate_hostIgrBwCtrlMeterIdx_get = NULL,
    .rate_hostIgrBwCtrlMeterIdx_set = NULL,
    .rate_hostEgrBwCtrlMeterIdx_get = NULL,
    .rate_hostEgrBwCtrlMeterIdx_set = NULL,



    /* PTP Function */
    .time_portTransparentEnable_set = NULL,
    .time_portTransparentEnable_get = NULL,
    .time_init = NULL,
    .time_portPtpEnable_get = NULL,
    .time_portPtpEnable_set = NULL,
    .time_curTime_latch = NULL,
    .time_curTime_get = NULL,
    .time_refTime_get = NULL,
    .time_refTime_set = NULL,
    .time_frequency_set = NULL,
    .time_frequency_get = NULL,
    .time_ptpIgrMsgAction_set = NULL,
    .time_ptpIgrMsgAction_get = NULL,
    .time_ptpEgrMsgAction_set = NULL,
    .time_ptpEgrMsgAction_get = NULL,
    .time_meanPathDelay_set = NULL,
    .time_meanPathDelay_get = NULL,
    .time_rxTime_set = NULL,
    .time_rxTime_get = NULL,
    .time_ponTodTime_set = NULL,
    .time_ponTodTime_get = NULL,
    .time_portPtpTxIndicator_get = NULL,
    .time_todEnable_get = NULL,
    .time_todEnable_set = NULL,
    .time_ppsEnable_get = NULL,
    .time_ppsEnable_set = NULL,
    .time_ppsMode_get = NULL,
    .time_ppsMode_set = NULL,
    .time_freeTime_get = NULL,

    /*acl*/
    .acl_init = NULL,
    .acl_template_set = NULL,
    .acl_template_get = NULL,
    .acl_fieldSelect_set = NULL,
    .acl_fieldSelect_get = NULL,
    .acl_igrRuleEntry_get = NULL,
    .acl_igrRuleField_add = NULL,
    .acl_igrRuleEntry_add = NULL,
    .acl_igrRuleEntry_del = NULL,
    .acl_igrRuleEntry_delAll = NULL,
    .acl_igrUnmatchAction_set = NULL,
    .acl_igrUnmatchAction_get = NULL,
    .acl_igrState_set = NULL,
    .acl_igrState_get = NULL,
    .acl_ipRange_set = NULL,
    .acl_ipRange_get = NULL,
    .acl_vidRange_set = NULL,
    .acl_vidRange_get = NULL,
    .acl_portRange_set = NULL,
    .acl_portRange_get = NULL,
    .acl_packetLengthRange_set = NULL,
    .acl_packetLengthRange_get = NULL,
    .acl_igrPermitState_set = NULL,
    .acl_igrPermitState_get = NULL,
    .acl_dbgInfo_get = NULL,
    .acl_dbgHitReason_get = NULL,


     /* L2 */
    .l2_init                                    = NULL,
    .l2_flushLinkDownPortAddrEnable_get         = NULL,
    .l2_flushLinkDownPortAddrEnable_set         = NULL,
    .l2_ucastAddr_flush                         = NULL,
    .l2_table_clear                             = NULL,
    .l2_limitLearningOverStatus_get             = NULL,
    .l2_limitLearningOverStatus_clear           = NULL,
    .l2_learningCnt_get                         = NULL,
    .l2_limitLearningCnt_get                    = NULL,
    .l2_limitLearningCnt_set                    = NULL,
    .l2_limitLearningCntAction_get              = NULL,
    .l2_limitLearningCntAction_set              = NULL,
    .l2_limitLearningEntryAction_get            = NULL,
    .l2_limitLearningEntryAction_set            = NULL,
    .l2_limitLearningPortMask_get               = NULL,
    .l2_limitLearningPortMask_set               = NULL,
    .l2_portLimitLearningOverStatus_get         = NULL,
    .l2_portLimitLearningOverStatus_clear       = NULL,
    .l2_portLearningCnt_get                     = NULL,
    .l2_portLimitLearningCnt_get                = NULL,
    .l2_portLimitLearningCnt_set                = NULL,
    .l2_portLimitLearningCntAction_get          = NULL,
    .l2_portLimitLearningCntAction_set          = NULL,
    .l2_aging_get                               = NULL,
    .l2_aging_set                               = NULL,
    .l2_portAgingEnable_get                     = NULL,
    .l2_portAgingEnable_set                     = NULL,
    .l2_lookupMissAction_get                    = NULL,
    .l2_lookupMissAction_set                    = NULL,
    .l2_portLookupMissAction_get                = NULL,
    .l2_portLookupMissAction_set                = NULL,
    .l2_lookupMissFloodPortMask_get             = NULL,
    .l2_lookupMissFloodPortMask_set             = NULL,
    .l2_lookupMissFloodPortMask_add             = NULL,
    .l2_lookupMissFloodPortMask_del             = NULL,
    .l2_newMacOp_get                            = NULL,
    .l2_newMacOp_set                            = NULL,
    .l2_nextValidAddr_get                       = NULL,
    .l2_nextValidAddrOnPort_get                 = NULL,
    .l2_nextValidMcastAddr_get                  = NULL,
    .l2_nextValidIpMcastAddr_get                = NULL,
    .l2_nextValidEntry_get                      = NULL,
    .l2_addr_add                                = NULL,
    .l2_addr_del                                = NULL,
    .l2_addr_get                                = NULL,
    .l2_addr_delAll                             = NULL,
    .l2_mcastAddr_add                           = NULL,
    .l2_mcastAddr_del                           = NULL,
    .l2_mcastAddr_get                           = NULL,
    .l2_illegalPortMoveAction_get               = NULL,
    .l2_illegalPortMoveAction_set               = NULL,
    .l2_ipmcMode_get                            = NULL,
    .l2_ipmcMode_set                            = NULL,
    .l2_ipmcVlanMode_get                        = NULL,
    .l2_ipmcVlanMode_set                        = NULL,
    .l2_ipv6mcMode_get                          = NULL,
    .l2_ipv6mcMode_set                          = NULL,
    .l2_ipmcGroupLookupMissHash_get             = NULL,
    .l2_ipmcGroupLookupMissHash_set             = NULL,
    .l2_ipmcGroup_add                           = NULL,
    .l2_ipmcGroupExtPortmask_add                = NULL,
    .l2_ipmcGroupExtMemberIdx_add               = NULL,
    .l2_ipmcGroup_del                           = NULL,
    .l2_ipmcGroup_get                           = NULL,
    .l2_ipmcGroupExtPortmask_get                = NULL,
    .l2_ipmcGroupExtMemberIdx_get               = NULL,
    .l2_ipmcSipFilter_set                       = NULL,
    .l2_ipmcSipFilter_get                       = NULL,
    .l2_portIpmcAction_get                      = NULL,
    .l2_portIpmcAction_set                      = NULL,
    .l2_ipMcastAddr_add                         = NULL,
    .l2_ipMcastAddr_del                         = NULL,
    .l2_ipMcastAddr_get                         = NULL,
    .l2_srcPortEgrFilterMask_get                = NULL,
    .l2_srcPortEgrFilterMask_set                = NULL,
    .l2_extPortEgrFilterMask_get                = NULL,
    .l2_extPortEgrFilterMask_set                = NULL,
    .l2_camState_set                            = NULL,
    .l2_camState_get                            = NULL,
    .l2_vidUnmatchAction_get                    = NULL,
    .l2_vidUnmatchAction_set                    = NULL,
    .l2_extMemberConfig_get                     = NULL,
    .l2_extMemberConfig_set                     = NULL,
    .l2_ip6mcReservedAddrEnable_set             = NULL,
    .l2_ip6mcReservedAddrEnable_get             = NULL,
    .l2_hashValue_get                           = NULL,

     /*interrupt*/
    .intr_init = NULL,
    .intr_polarity_set = NULL,
    .intr_polarity_get = NULL,
    .intr_imr_set = NULL,
    .intr_imr_get = NULL,
    .intr_ims_get = NULL,
    .intr_ims_clear = NULL,
    .intr_speedChangeStatus_get = NULL,
    .intr_speedChangeStatus_clear = NULL,
    .intr_linkupStatus_get = NULL,
    .intr_linkupStatus_clear = NULL,
    .intr_linkdownStatus_get = NULL,
    .intr_linkdownStatus_clear = NULL,
    .intr_gphyStatus_get = NULL,
    .intr_gphyStatus_clear = NULL,
    .intr_imr_restore = NULL,
    .intr_isr_set = NULL,
    .intr_isr_get = NULL,
    .intr_isr_counter_dump = NULL,

    /*cpu*/
    .cpu_init = NULL,
    .cpu_trapInsertTagByPort_set = NULL,
    .cpu_trapInsertTagByPort_get = NULL,
    .cpu_tagAwareByPort_set = NULL,
    .cpu_tagAwareByPort_get = NULL,

    /* QoS */
    .qos_init = NULL,
    .qos_priSelGroup_get = NULL,
    .qos_priSelGroup_set = NULL,
    .qos_portPri_get = NULL,
    .qos_portPri_set = NULL,
    .qos_dscpPriRemapGroup_get = NULL,
    .qos_dscpPriRemapGroup_set = NULL,
    .qos_1pPriRemapGroup_get = NULL,
    .qos_1pPriRemapGroup_set = NULL,
    .qos_priMap_get = NULL,
    .qos_priMap_set = NULL,
    .qos_portPriMap_get = NULL,
    .qos_portPriMap_set = NULL,
    .qos_1pRemarkEnable_get = NULL,
    .qos_1pRemarkEnable_set = NULL,
    .qos_1pRemarkGroup_get = NULL,
    .qos_1pRemarkGroup_set = NULL,
    .qos_dscpRemarkEnable_get = NULL,
    .qos_dscpRemarkEnable_set = NULL,
    .qos_dscpRemarkGroup_get = NULL,
    .qos_dscpRemarkGroup_set = NULL,
    .qos_fwd2CpuPriRemap_get = NULL,
    .qos_fwd2CpuPriRemap_set = NULL,
    .qos_portDscpRemarkSrcSel_get = NULL,
    .qos_portDscpRemarkSrcSel_set = NULL,
    .qos_dscp2DscpRemarkGroup_get = NULL,
    .qos_dscp2DscpRemarkGroup_set = NULL,
    .qos_schedulingQueue_get = NULL,
    .qos_schedulingQueue_set = NULL,
    .qos_portPriSelGroup_get = NULL,
    .qos_portPriSelGroup_set = NULL,
    .qos_schedulingType_get = NULL,
    .qos_schedulingType_set = NULL,
    .qos_portDot1pRemarkSrcSel_get = NULL,
    .qos_portDot1pRemarkSrcSel_set = NULL,

    /* Classification */
    .classify_init                              = NULL,
    .classify_cfgEntry_add                      = NULL,
    .classify_cfgEntry_get                      = NULL,
    .classify_cfgEntry_del                      = NULL,
    .classify_field_add                         = NULL,
    .classify_unmatchAction_set                 = NULL,
    .classify_unmatchAction_get                 = NULL,
    .classify_cf_sel_set                        = NULL,
    .classify_cf_sel_get                        = NULL,
    .classify_trapPri_set                       = NULL,
    .classify_trapPri_get                       = NULL,

    /* GPON */
    .gpon_init = NULL,
    .gpon_resetState_set = NULL,
    .gpon_resetDoneState_get = NULL,
    .gpon_version_get = NULL,
    .gpon_test_get = NULL,
    .gpon_test_set = NULL,
    .gpon_topIntrMask_get = NULL,
    .gpon_topIntrMask_set = NULL,
    .gpon_topIntr_get = NULL,
    .gpon_topIntr_disableAll = NULL,
    .gpon_gtcDsIntr_get = NULL,
    .gpon_gtcDsIntrDlt_get = NULL,
    .gpon_gtcDsIntrMask_get = NULL,
    .gpon_gtcDsIntrMask_set = NULL,
    .gpon_onuId_set = NULL,
    .gpon_onuId_get = NULL,
    .gpon_onuState_set = NULL,
    .gpon_onuState_get = NULL,
    .gpon_dsBwmapCrcCheckState_set = NULL,
    .gpon_dsBwmapCrcCheckState_get = NULL,
    .gpon_dsBwmapFilterOnuIdState_set = NULL,
    .gpon_dsBwmapFilterOnuIdState_get = NULL,
    .gpon_dsPlendStrictMode_set = NULL,
    .gpon_dsPlendStrictMode_get = NULL,
    .gpon_dsScrambleState_set = NULL,
    .gpon_dsScrambleState_get = NULL,
    .gpon_dsFecBypass_set = NULL,
    .gpon_dsFecBypass_get = NULL,
    .gpon_dsFecThrd_set = NULL,
    .gpon_dsFecThrd_get = NULL,
    .gpon_extraSnTxTimes_set = NULL,
    .gpon_extraSnTxTimes_get = NULL,
    .gpon_dsPloamNomsg_set = NULL,
    .gpon_dsPloamNomsg_get = NULL,
    .gpon_dsPloamOnuIdFilterState_set = NULL,
    .gpon_dsPloamOnuIdFilterState_get = NULL,
    .gpon_dsPloamBcAcceptState_set = NULL,
    .gpon_dsPloamBcAcceptState_get = NULL,
    .gpon_dsPloamDropCrcState_set = NULL,
    .gpon_dsPloamDropCrcState_get = NULL,
    .gpon_cdrLosStatus_get = NULL,
    .gpon_optLosStatus_get = NULL,
    .gpon_losCfg_set = NULL,
    .gpon_losCfg_get = NULL,
    .gpon_dsPloam_get = NULL,
    .gpon_dsPloam_getAll = NULL,
    .gpon_tcont_get = NULL,
    .gpon_tcont_set = NULL,
    .gpon_tcont_del = NULL,
    .gpon_dsGemPort_get = NULL,
    .gpon_dsGemPort_set = NULL,
    .gpon_dsGemPort_del = NULL,
    .gpon_dsGemPortPktCnt_get = NULL,
    .gpon_dsGemPortByteCnt_get = NULL,
    .gpon_dsGtcMiscCnt_get = NULL,
    .gpon_dsOmciPti_set = NULL,
    .gpon_dsOmciPti_get = NULL,
    .gpon_dsEthPti_set = NULL,
    .gpon_dsEthPti_get = NULL,
    .gpon_aesKeySwitch_set = NULL,
    .gpon_aesKeySwitch_get = NULL,
    .gpon_aesKeyWord_set = NULL,
    .gpon_aesKeyWordActive_set = NULL,
    .gpon_irq_get = NULL,
    .gpon_dsGemPortEthRxCnt_get = NULL,
    .gpon_dsGemPortEthFwdCnt_get = NULL,
    .gpon_dsGemPortMiscCnt_get = NULL,
    .gpon_dsGemPortFcsCheckState_get = NULL,
    .gpon_dsGemPortFcsCheckState_set = NULL,
    .gpon_dsGemPortBcPassState_set = NULL,
    .gpon_dsGemPortBcPassState_get = NULL,
    .gpon_dsGemPortNonMcPassState_set = NULL,
    .gpon_dsGemPortNonMcPassState_get = NULL,
    .gpon_dsGemPortMacFilterMode_set = NULL,
    .gpon_dsGemPortMacFilterMode_get = NULL,
    .gpon_dsGemPortMacForceMode_set = NULL,
    .gpon_dsGemPortMacForceMode_get = NULL,
    .gpon_dsGemPortMacEntry_set = NULL,
    .gpon_dsGemPortMacEntry_get = NULL,
    .gpon_dsGemPortMacEntry_del = NULL,
    .gpon_dsGemPortFrameTimeOut_set = NULL,
    .gpon_dsGemPortFrameTimeOut_get = NULL,
    .gpon_ipv4McAddrPtn_get = NULL,
    .gpon_ipv4McAddrPtn_set = NULL,
    .gpon_ipv6McAddrPtn_get = NULL,
    .gpon_ipv6McAddrPtn_set = NULL,
    .gpon_gtcUsIntr_get = NULL,
    .gpon_gtcUsIntrDlt_get = NULL,
    .gpon_gtcUsIntrMask_get = NULL,
    .gpon_gtcUsIntrMask_set = NULL,
    .gpon_forceLaser_set = NULL,
    .gpon_forceLaser_get = NULL,
    .gpon_forcePRBS_set = NULL,
    .gpon_forcePRBS_get = NULL,
    .gpon_ploamState_set = NULL,
    .gpon_ploamState_get = NULL,
    .gpon_indNrmPloamState_set = NULL,
    .gpon_indNrmPloamState_get = NULL,
    .gpon_dbruState_set = NULL,
    .gpon_dbruState_get = NULL,
    .gpon_usScrambleState_set = NULL,
    .gpon_usScrambleState_get = NULL,
    .gpon_usBurstPolarity_set = NULL,
    .gpon_usBurstPolarity_get = NULL,
    .gpon_eqd_set = NULL,
    .gpon_eqd_get = NULL,
    .gpon_laserTime_set = NULL,
    .gpon_laserTime_get = NULL,
    .gpon_burstOverhead_set = NULL,
    .gpon_usPloam_set = NULL,
    .gpon_usAutoPloam_set = NULL,
    .gpon_usPloamCrcGenState_set = NULL,
    .gpon_usPloamCrcGenState_get = NULL,
    .gpon_usPloamOnuIdFilterState_set = NULL,
    .gpon_usPloamOnuIdFilter_get = NULL,
    .gpon_usPloamBuf_flush = NULL,
    .gpon_usGtcMiscCnt_get = NULL,
    .gpon_rdi_set = NULL,
    .gpon_rdi_get = NULL,
    .gpon_usSmalSstartProcState_set = NULL,
    .gpon_usSmalSstartProcState_get = NULL,
    .gpon_usSuppressLaserState_set = NULL,
    .gpon_usSuppressLaserState_get = NULL,
    .gpon_gemUsIntr_get = NULL,
    .gpon_gemUsIntrMask_get = NULL,
    .gpon_gemUsIntrMask_set = NULL,
    .gpon_gemUsForceIdleState_set = NULL,
    .gpon_gemUsForceIdleState_get = NULL,
    .gpon_gemUsPtiVector_set = NULL,
    .gpon_gemUsPtiVector_get = NULL,
    .gpon_gemUsEthCnt_get = NULL,
    .gpon_gemUsGemCnt_get = NULL,
    .gpon_gemUsPortCfg_set = NULL,
    .gpon_gemUsPortCfg_get = NULL,
    .gpon_gemUsDataByteCnt_get = NULL,
    .gpon_gemUsIdleByteCnt_get = NULL,
    .gpon_dbruPeriod_get = NULL,
    .gpon_dbruPeriod_set = NULL,
    .gpon_gtcDsIntrDlt_check = NULL,
    .gpon_gtcUsIntrDlt_check = NULL,
    .gpon_gemUsIntrDlt_check = NULL,
    .gpon_rogueOnt_set = NULL,
    .gpon_drainOutDefaultQueue_set = NULL,
    .gpon_autoDisTx_set = NULL,
    .gpon_scheInfo_get = NULL,
    .gpon_dataPath_reset = NULL,
    .gpon_dsOmciCnt_get = NULL,
    .gpon_usOmciCnt_get = NULL,
    .gpon_gtcDsTodSuperFrame_set = NULL,
    .gpon_gtcDsTodSuperFrame_get = NULL,
    .gpon_dbruBlockSize_get = NULL,
    .gpon_dbruBlockSize_set = NULL,
    .gpon_usLaserDefault_get = NULL,
    .gpon_flowctrl_adjust_byFlowNum = NULL,

    /*PON MAC*/
    .ponmac_init = NULL,
    .ponmac_queue_add = NULL,
    .ponmac_queue_get = NULL,
    .ponmac_queue_del = NULL,
    .ponmac_flow2Queue_set = NULL,
    .ponmac_flow2Queue_get = NULL,
    .ponmac_mode_get = NULL,
    .ponmac_mode_set = NULL,
    .ponmac_queueDrainOut_set  = NULL,
    .ponmac_opticalPolarity_get = NULL,
    .ponmac_opticalPolarity_set = NULL,

    .ponmac_losState_get = NULL,
    .ponmac_serdesCdr_reset = NULL,
    .ponmac_linkState_get = NULL,

    .ponmac_forceBerNotify_set = NULL,
    .ponmac_bwThreshold_set = NULL,
    .ponmac_bwThreshold_get = NULL,
    .ponmac_maxPktLen_set = NULL,
    .ponmac_sidValid_get = NULL,
    .ponmac_sidValid_set = NULL,
    .ponmac_schedulingType_get = NULL,
    .ponmac_schedulingType_set = NULL,
    .ponmac_egrBandwidthCtrlRate_get = NULL,
    .ponmac_egrBandwidthCtrlRate_set = NULL,
    .ponmac_egrScheduleIdRate_get = NULL,
    .ponmac_egrScheduleIdRate_set = NULL,
    .ponmac_egrBandwidthCtrlIncludeIfg_get = NULL,
    .ponmac_egrBandwidthCtrlIncludeIfg_set = NULL,
    .ponmac_egrScheduleIdIncludeIfg_get = NULL,
    .ponmac_egrScheduleIdIncludeIfg_set = NULL,
    .ponmac_txDisableGpio_get = NULL,
    .ponmac_txDisableGpio_set = NULL,
    .ponmac_txPowerDisableGpio_get = NULL,
    .ponmac_txPowerDisableGpio_set = NULL,

    /*gpio function*/
    .gpio_init = NULL,
    .gpio_state_set = NULL,
    .gpio_state_get = NULL,
    .gpio_mode_set = NULL,
    .gpio_mode_get = NULL,
    .gpio_databit_get = NULL,
    .gpio_databit_set = NULL,
    .gpio_intr_get = NULL,
    .gpio_intr_set = NULL,
    .gpio_intrStatus_get = NULL,
    .gpio_intrStatus_clean = NULL,

    /* RLDP and RLPP */
    .rldp_init = NULL,
    .rldp_config_set = NULL,
    .rldp_config_get = NULL,
    .rldp_portConfig_set = NULL,
    .rldp_portConfig_get = NULL,
    .rldp_status_get = NULL,
    .rldp_portStatus_get = NULL,
    .rldp_portStatus_clear = NULL,
    .rlpp_init = NULL,
    .rlpp_trapType_set = NULL,
    .rlpp_trapType_get = NULL,

    /*led*/
    .led_init               = NULL,
    .led_operation_get      = NULL,
    .led_operation_set      = NULL,
    .led_serialMode_get     = NULL,
    .led_serialMode_set     = NULL,
    .led_blinkRate_get      = NULL,
    .led_blinkRate_set      = NULL,
    .led_config_set         = NULL,
    .led_config_get         = NULL,
    .led_modeForce_get      = NULL,
    .led_modeForce_set      = NULL,
    .led_parallelEnable_get = NULL,
    .led_parallelEnable_set = NULL,
    .led_ponAlarm_get       = NULL,
    .led_ponAlarm_set       = NULL,
    .led_ponWarning_get     = NULL,
    .led_ponWarning_set     = NULL,


    /* I2C */
    .i2c_init = NULL,
    .i2c_enable_set = NULL,
    .i2c_enable_get = NULL,
    .i2c_width_set = NULL,
    .i2c_width_get = NULL,
    .i2c_write = NULL,
    .i2c_read = NULL,
    .i2c_clock_set = NULL,
    .i2c_clock_get = NULL,
    .i2c_addrWidth_set = NULL,
    .i2c_addrWidth_get = NULL,
    .i2c_dataWidth_set = NULL,
    .i2c_dataWidth_get = NULL,

    /* EPON */
    .epon_init = NULL,
    .epon_intrMask_get = NULL,
    .epon_intrMask_set = NULL,
    .epon_intr_get = NULL,
    .epon_intr_disableAll = NULL,
    .epon_llid_entry_set = NULL,
    .epon_llid_entry_get = NULL,
    .epon_forceLaserState_set = NULL,
    .epon_forceLaserState_get = NULL,
    .epon_laserTime_set = NULL,
    .epon_laserTime_get = NULL,
    .epon_syncTime_get = NULL,
    .epon_registerReq_get = NULL,
    .epon_registerReq_set = NULL,
    .epon_churningKey_set = NULL,
    .epon_churningKey_get = NULL,
    .epon_usFecState_get = NULL,
    .epon_usFecState_set = NULL,
    .epon_dsFecState_get = NULL,
    .epon_dsFecState_set = NULL,
    .epon_mibCounter_get = NULL,
    .epon_mibGlobal_reset = NULL,
    .epon_mibLlidIdx_reset = NULL,
    .epon_losState_get = NULL,
    .epon_mpcpTimeoutVal_get = NULL,
    .epon_mpcpTimeoutVal_set = NULL,
    .epon_opticalPolarity_set = NULL,
    .epon_opticalPolarity_get = NULL,
    .epon_fecState_get = NULL,
    .epon_fecState_set = NULL,
    .epon_llidEntryNum_get = NULL,
    .epon_thresholdReport_set = NULL,
    .epon_thresholdReport_get = NULL,
    .epon_dbgInfo_get = NULL,
    .epon_multiLlidMode_set = NULL,
    .epon_multiLlidMode_get = NULL,
    .epon_reportQueueSet_set = NULL,
    .epon_reportQueueSet_get = NULL,
    .epon_queueThresholdReport_set = NULL,
    .epon_queueThresholdReport_get = NULL,
    .epon_autoPowerSavingState_get = NULL,
    .epon_autoPowerSavingState_set = NULL,
    .epon_autoPowerSavingOeRecoverTime_get = NULL,
    .epon_autoPowerSavingOeRecoverTime_set = NULL,



    .ppstod_init = NULL,
    .ppstod_delay_get = NULL,
    .ppstod_delay_set = NULL,
    .ppstod_pulseWidth_get = NULL,
    .ppstod_pulseWidth_set = NULL,
    .ppstod_mode_get = NULL,
    .ppstod_mode_set = NULL,
    .ppstod_sarpGpsWeek_get = NULL,
    .ppstod_sarpGpsWeek_set = NULL,
    .ppstod_sarpUtcSec_get = NULL,
    .ppstod_sarpUtcSec_set = NULL,
    .ppstod_frameLen_get = NULL,
    .ppstod_frameLen_set = NULL,
    .ppstod_frameData_get = NULL,
    .ppstod_frameData_set = NULL,
    .ppstod_baudrate_get = NULL,
    .ppstod_baudrate_set = NULL,

    /* PBO */
    /* PON PBO */
    .pbo_init = NULL,
    /* Switch PBO */
    .swPbo_init = NULL,
    .swPbo_portState_set = NULL,
    .swPbo_portState_get = NULL,
    .swPbo_portThreshold_set = NULL,
    .swPbo_portThreshold_get = NULL,
    .swPbo_queueThreshold_set = NULL,
    .swPbo_queueThreshold_get = NULL,
    .swPbo_portUsedPageCount_get = NULL,
    .swPbo_portUsedPageCount_clear = NULL,
    .swPbo_queueUsedPageCount_get = NULL,
    .swPbo_queueUsedPageCount_clear = NULL,
    .swPbo_portDscCount_get = NULL,
    .swPbo_portDscCount_clear = NULL,

    /*MDIO*/
    .mdio_init = NULL,
    .mdio_cfg_set = NULL,
    .mdio_cfg_get = NULL,
    .mdio_c22_write = NULL,
    .mdio_c22_read = NULL,
    .mdio_c45_write = NULL,
    .mdio_c45_read = NULL,
};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Module Name    :  */

/* Function Name:
 *      dal_ca8277b_init
 * Description:
 *      Initilize DAL of smart switch
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - initialize success
 *      RT_ERR_FAILED - initialize fail
 * Note:
 *      RTK must call this function before do other kind of action.
 */
int32 dal_ca8277b_init(void)
{
    return RT_ERR_OK;
} /* end of dal_ca8277b_init */



/* Function Name:
 *      dal_ca8277b_mapper_get
 * Description:
 *      Get DAL mapper function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      dal_mapper_t *     - mapper pointer
 * Note:
 */
dal_mapper_t *dal_ca8277b_mapper_get(void)
{
    return &dal_ca8277b_mapper;
} /* end of dal_ca8277b_mapper_get */



void dal_ca8277b_mapper_register(dal_mapper_t *pMapper)
{
    int32 ret;

    memcpy(&dal_ca8277b_mapper,pMapper,sizeof(dal_mapper_t));
#if 0
    /*initial function*/
    if((ret = dal_ca8277b_mapper.switch_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.l2_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.vlan_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.stat_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.rate_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.qos_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.intr_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.port_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.sec_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.epon_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.trap_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.i2c_init(0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.led_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.gpon_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.mirror_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.ponmac_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

    if((ret = dal_ca8277b_mapper.gpio_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }

#if defined(CONFIG_COMMON_RT_API)
    if((ret = dal_ca8277b_mapper.rt_ponmisc_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
    }
#endif
#endif
}
#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(dal_ca8277b_mapper_register);
#endif
