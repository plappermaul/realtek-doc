#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>

#include <rtk/init.h>

#include <dal/dal_mapper.h>
#include <dal/dal_common.h>
#include <dal/ca8279/dal_ca8279_mapper.h>
#include <dal/ca8279/dal_ca8279_switch.h>
#include <dal/ca8279/dal_ca8279_l2.h>
#include <dal/ca8279/dal_ca8279_vlan.h>
#include <dal/ca8279/dal_ca8279_port.h>
#include <dal/ca8279/dal_ca8279_qos.h>
#include <dal/ca8279/dal_ca8279_stat.h>
#include <dal/ca8279/dal_ca8279_rate.h>
#include <dal/ca8279/dal_ca8279_sec.h>
#include <dal/ca8279/dal_ca8279_intr.h>
#include <dal/ca8279/dal_ca8279_epon.h>
#include <dal/ca8279/dal_ca8279_trap.h>
#include <dal/ca8279/dal_ca8279_i2c.h>
#include <dal/ca8279/dal_ca8279_led.h>
#include <dal/ca8279/dal_ca8279_ponmac.h>
#include <dal/ca8279/dal_ca8279_gpon.h>
#include <dal/ca8279/dal_ca8279_mirror.h>
#include <dal/ca8279/dal_ca8279_gpio.h>

#include "ni-drv/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"
#include "cortina-api/include/pkt_tx.h"
#include "cortina-api/include/port.h"
#include "cortina-api/include/special_packet.h"
#include "cortina-api/include/epon.h"
#include "cortina-api/include/classifier.h"
#include "cortina-api/include/vlan.h"

#if defined(CONFIG_COMMON_RT_API)
#include <dal/ca8279/dal_rt_ca8279_gpon.h>
#include <dal/ca8279/dal_rt_ca8279_ponmisc.h>
#include <dal/ca8279/dal_rt_ca8279_epon.h>
#endif

int32
dal_ca8279_success(void)
{
    if(ca_rtk_debug & 0x4)
    {
       printk(KERN_INFO "%s: byPass RTK API, just return RT_ERR_OK!\n", __func__);
    }

    return RT_ERR_OK;
}

#if !defined(CONFIG_RG_G3_SERIES_DEVELOPMENT)   // CONFIG_RG_G3_SERIES_DEVELOPMENT
static dal_mapper_t dal_ca8279_mapper =
{
    0,//Kem TBD
    ._init = NULL,

    /*sec*/
    .sec_init = dal_ca8279_sec_init,
    .sec_portAttackPreventState_get = dal_ca8279_sec_portAttackPreventState_get,
    .sec_portAttackPreventState_set = dal_ca8279_sec_portAttackPreventState_set,
    .sec_attackPrevent_get = dal_ca8279_sec_attackPrevent_get,
    .sec_attackPrevent_set = dal_ca8279_sec_attackPrevent_set,
    .sec_attackFloodThresh_get = dal_ca8279_sec_attackFloodThresh_get,
    .sec_attackFloodThresh_set = dal_ca8279_sec_attackFloodThresh_set,
    .sec_attackFloodThreshUnit_get = dal_ca8279_sec_attackFloodThreshUnit_get,
    .sec_attackFloodThreshUnit_set = dal_ca8279_sec_attackFloodThreshUnit_set,

    /* statistics */
    .stat_init                                  = dal_ca8279_stat_init,
    .stat_global_reset                          = NULL,
    .stat_port_reset                            = dal_ca8279_stat_port_reset,
    .stat_log_reset                             = NULL,
    .stat_hostCnt_reset                         = NULL,
    .stat_hostCnt_get                           = NULL,
    .stat_hostState_get                         = NULL,
    .stat_hostState_set                         = NULL,
    .stat_rst_cnt_value_set                     = NULL,
    .stat_rst_cnt_value_get                     = NULL,
    .stat_global_get                            = NULL,
    .stat_global_getAll                         = NULL,
    .stat_port_get                              = dal_ca8279_stat_port_get,
    .stat_port_getAll                           = dal_ca8279_stat_port_getAll,
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
    .switch_init = dal_ca8279_switch_init,
    .switch_phyPortId_get = dal_ca8279_switch_phyPortId_get,
    .switch_logicalPort_get = NULL,
    .switch_port2PortMask_set = NULL,
    .switch_port2PortMask_clear = NULL,
    .switch_portIdInMask_check = NULL,
    .switch_maxPktLenLinkSpeed_get = NULL,
    .switch_maxPktLenLinkSpeed_set = NULL,
    .switch_mgmtMacAddr_get = NULL,
    .switch_mgmtMacAddr_set = NULL,
    .switch_chip_reset = dal_ca8279_switch_chip_reset,
    .switch_version_get = dal_ca8279_switch_version_get,
    .switch_patch_info_get = NULL,
    .switch_csExtId_get = NULL,
    .switch_maxPktLenByPort_get = dal_ca8279_switch_maxPktLenByPort_get,
    .switch_maxPktLenByPort_set = dal_ca8279_switch_maxPktLenByPort_set,
    .switch_changeDuplex_get = NULL,
    .switch_changeDuplex_set = NULL,
    .switch_system_init = NULL,
    .switch_thermal_get = dal_ca8279_switch_thermal_get,

    /* Port */
    .port_init                                  = dal_ca8279_port_init,
    .port_link_get                              = dal_ca8279_port_link_get,
    .port_speedDuplex_get                       = dal_ca8279_port_speedDuplex_get,
    .port_flowctrl_get                          = dal_ca8279_port_flowctrl_get,
    .port_phyAutoNegoEnable_get                 = dal_ca8279_port_phyAutoNegoEnable_get,
    .port_phyAutoNegoEnable_set                 = dal_ca8279_port_phyAutoNegoEnable_set,
    .port_phyAutoNegoAbility_get                = dal_ca8279_port_phyAutoNegoAbility_get,
    .port_phyAutoNegoAbility_set                = dal_ca8279_port_phyAutoNegoAbility_set,
    .port_phyForceModeAbility_get               = dal_ca8279_port_phyForceModeAbility_get,
    .port_phyForceModeAbility_set               = dal_ca8279_port_phyForceModeAbility_set,
    .port_phyMasterSlave_get                    = NULL,
    .port_phyMasterSlave_set                    = NULL,
    .port_phyTestMode_get                       = NULL,
    .port_phyTestMode_set                       = NULL,
    .port_phyReg_get                            = dal_ca8279_port_phyReg_get,
    .port_phyReg_set                            = dal_ca8279_port_phyReg_set,
    .port_cpuPortId_get                         = NULL,
    .port_isolation_get                         = NULL,
    .port_isolation_set                         = NULL,
    .port_isolationExt_get                      = NULL,
    .port_isolationExt_set                      = NULL,
    .port_isolationL34_get                      = NULL,
    .port_isolationL34_set                      = NULL,
    .port_isolationExtL34_get                   = NULL,
    .port_isolationExtL34_set                   = NULL,
    .port_isolationEntry_get                    = dal_ca8279_port_isolationEntry_get,
    .port_isolationEntry_set                    = dal_ca8279_port_isolationEntry_set,
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
    .port_adminEnable_get                       = dal_ca8279_port_adminEnable_get,
    .port_adminEnable_set                       = dal_ca8279_port_adminEnable_set,
    .port_specialCongest_get                    = NULL,
    .port_specialCongest_set                    = NULL,
    .port_specialCongestStatus_get              = NULL,
    .port_specialCongestStatus_clear            = NULL,
    .port_greenEnable_get                       = NULL,
    .port_greenEnable_set                       = NULL,
    .port_phyCrossOverMode_get                  = NULL,
    .port_phyCrossOverMode_set                  = NULL,
    .port_phyPowerDown_set                      = dal_ca8279_port_phyPowerDown_set,
    .port_phyPowerDown_get                      = dal_ca8279_port_phyPowerDown_get,
    .port_enhancedFid_get                       = NULL,
    .port_enhancedFid_set                       = NULL,
    .port_rtctResult_get                        = NULL,
    .port_rtct_start                            = NULL,
    .port_macForceAbility_set                   = dal_ca8279_port_macForceAbility_set,
    .port_macForceAbility_get                   = dal_ca8279_port_macForceAbility_get,
    .port_macForceAbilityState_set              = dal_ca8279_port_macForceAbilityState_set,
    .port_macForceAbilityState_get              = dal_ca8279_port_macForceAbilityState_get,
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
    .trap_init                                  = dal_ca8279_trap_init,
    .trap_reasonTrapToCpuPriority_get           = NULL,
    .trap_reasonTrapToCpuPriority_set           = NULL,
    .trap_igmpCtrlPkt2CpuEnable_get             = NULL,
    .trap_igmpCtrlPkt2CpuEnable_set             = NULL,
    .trap_mldCtrlPkt2CpuEnable_get              = NULL,
    .trap_mldCtrlPkt2CpuEnable_set              = NULL,
    .trap_portIgmpMldCtrlPktAction_get          = dal_ca8279_trap_portIgmpMldCtrlPktAction_get,
    .trap_portIgmpMldCtrlPktAction_set          = dal_ca8279_trap_portIgmpMldCtrlPktAction_set,
    .trap_l2McastPkt2CpuEnable_get              = NULL,
    .trap_l2McastPkt2CpuEnable_set              = NULL,
    .trap_ipMcastPkt2CpuEnable_get              = NULL,
    .trap_ipMcastPkt2CpuEnable_set              = NULL,
    .trap_rmaAction_get                         = NULL,
    .trap_rmaAction_set                         = NULL,
    .trap_rmaPri_get                            = NULL,
    .trap_rmaPri_set                            = NULL,
    .trap_oamPduAction_get                      = dal_ca8279_trap_oamPduAction_get,
    .trap_oamPduAction_set                      = dal_ca8279_trap_oamPduAction_set,
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
    .svlan_init = dal_ca8279_success,
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
    .vlan_init                                  = dal_ca8279_vlan_init,
    .vlan_create                                = dal_ca8279_vlan_create,
    .vlan_destroy                               = dal_ca8279_vlan_destroy,
    .vlan_destroyAll                            = dal_ca8279_vlan_destroyAll,
    .vlan_fid_get                               = NULL,
    .vlan_fid_set                               = NULL,
    .vlan_fidMode_get                           = dal_ca8279_vlan_fidMode_get,
    .vlan_fidMode_set                           = dal_ca8279_vlan_fidMode_set,
    .vlan_port_get                              = dal_ca8279_vlan_port_get,
    .vlan_port_set                              = dal_ca8279_vlan_port_set,
    .vlan_stg_get                               = NULL,
    .vlan_stg_set                               = NULL,
    .vlan_portAcceptFrameType_get               = NULL,
    .vlan_portAcceptFrameType_set               = NULL,
    .vlan_vlanFunctionEnable_get                = dal_ca8279_vlan_vlanFunctionEnable_get,
    .vlan_vlanFunctionEnable_set                = dal_ca8279_vlan_vlanFunctionEnable_set,
    .vlan_portIgrFilterEnable_get               = NULL,
    .vlan_portIgrFilterEnable_set               = NULL,
    .vlan_leaky_get                             = NULL,
    .vlan_leaky_set                             = NULL,
    .vlan_portLeaky_get                         = NULL,
    .vlan_portLeaky_set                         = NULL,
    .vlan_keepType_get                          = NULL,
    .vlan_keepType_set                          = NULL,
    .vlan_portPvid_get                          = dal_ca8279_vlan_portPvid_get,
    .vlan_portPvid_set                          = dal_ca8279_vlan_portPvid_set,
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
    .mirror_init                                = dal_ca8279_mirror_init,
    .mirror_portBased_set                       = dal_ca8279_mirror_portBased_set,
    .mirror_portBased_get                       = dal_ca8279_mirror_portBased_get,
    .mirror_portIso_set                         = NULL,
    .mirror_portIso_get                         = NULL,

    /* STP */
    .stp_init = dal_ca8279_success,
    .stp_mstpState_get = NULL,
    .stp_mstpState_set = NULL,

    /*rate*/
    .rate_init = dal_ca8279_rate_init,
    .rate_portIgrBandwidthCtrlRate_get = dal_ca8279_rate_portIgrBandwidthCtrlRate_get,
    .rate_portIgrBandwidthCtrlRate_set = dal_ca8279_rate_portIgrBandwidthCtrlRate_set,
    .rate_portIgrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_portIgrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_portEgrBandwidthCtrlRate_get = dal_ca8279_rate_portEgrBandwidthCtrlRate_get,
    .rate_portEgrBandwidthCtrlRate_set = dal_ca8279_rate_portEgrBandwidthCtrlRate_set,
    .rate_egrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_egrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_portEgrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_portEgrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_egrQueueBwCtrlEnable_get = dal_ca8279_rate_egrQueueBwCtrlEnable_get,
    .rate_egrQueueBwCtrlEnable_set = dal_ca8279_rate_egrQueueBwCtrlEnable_set,
    .rate_egrQueueBwCtrlMeterIdx_get = dal_ca8279_rate_egrQueueBwCtrlMeterIdx_get,
    .rate_egrQueueBwCtrlMeterIdx_set = dal_ca8279_rate_egrQueueBwCtrlMeterIdx_set,
    .rate_egrQueueBwCtrlRate_get = NULL,
    .rate_egrQueueBwCtrlRate_set = NULL,
    .rate_stormControlMeterIdx_get = dal_ca8279_rate_stormControlMeterIdx_get,
    .rate_stormControlMeterIdx_set = dal_ca8279_rate_stormControlMeterIdx_set,
    .rate_stormControlPortEnable_get = dal_ca8279_rate_stormControlPortEnable_get,
    .rate_stormControlPortEnable_set = dal_ca8279_rate_stormControlPortEnable_set,
    .rate_stormControlEnable_get = NULL,
    .rate_stormControlEnable_set = NULL,
    .rate_stormBypass_set = NULL,
    .rate_stormBypass_get = NULL,
    .rate_shareMeter_set = dal_ca8279_rate_shareMeter_set, 
    .rate_shareMeter_get = dal_ca8279_rate_shareMeter_get, 
    .rate_shareMeterBucket_set = dal_ca8279_rate_shareMeterBucket_set,
    .rate_shareMeterBucket_get = dal_ca8279_rate_shareMeterBucket_get,
    .rate_shareMeterExceed_get = NULL,
    .rate_shareMeterExceed_clear = NULL,
    .rate_shareMeterMode_set = dal_ca8279_rate_shareMeterMode_set,
    .rate_shareMeterMode_get = dal_ca8279_rate_shareMeterMode_get,
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
    .time_init = dal_ca8279_success,
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
    .acl_init = dal_ca8279_success,
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
    .l2_init                                    = dal_ca8279_l2_init,
    .l2_flushLinkDownPortAddrEnable_get         = dal_ca8279_l2_flushLinkDownPortAddrEnable_get,
    .l2_flushLinkDownPortAddrEnable_set         = dal_ca8279_l2_flushLinkDownPortAddrEnable_set,
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
    .l2_portLimitLearningCnt_get                = dal_ca8279_l2_portLimitLearningCnt_get,
    .l2_portLimitLearningCnt_set                = dal_ca8279_l2_portLimitLearningCnt_set,
    .l2_portLimitLearningCntAction_get          = dal_ca8279_l2_portLimitLearningCntAction_get,
    .l2_portLimitLearningCntAction_set          = dal_ca8279_l2_portLimitLearningCntAction_set,
    .l2_aging_get                               = dal_ca8279_l2_aging_get,
    .l2_aging_set                               = dal_ca8279_l2_aging_set,
    .l2_portAgingEnable_get                     = dal_ca8279_l2_portAgingEnable_get,
    .l2_portAgingEnable_set                     = dal_ca8279_l2_portAgingEnable_set,
    .l2_lookupMissAction_get                    = dal_ca8279_l2_lookupMissAction_get,
    .l2_lookupMissAction_set                    = dal_ca8279_l2_lookupMissAction_set,
    .l2_portLookupMissAction_get                = dal_ca8279_l2_portLookupMissAction_get,
    .l2_portLookupMissAction_set                = dal_ca8279_l2_portLookupMissAction_set,
    .l2_lookupMissFloodPortMask_get             = NULL,
    .l2_lookupMissFloodPortMask_set             = NULL,
    .l2_lookupMissFloodPortMask_add             = NULL,
    .l2_lookupMissFloodPortMask_del             = NULL,
    .l2_newMacOp_get                            = NULL,
    .l2_newMacOp_set                            = NULL,
    .l2_nextValidAddr_get                       = dal_ca8279_l2_nextValidAddr_get,
    .l2_nextValidAddrOnPort_get                 = dal_ca8279_l2_nextValidAddrOnPort_get,
    .l2_nextValidMcastAddr_get                  = NULL,
    .l2_nextValidIpMcastAddr_get                = NULL,
    .l2_nextValidEntry_get                      = dal_ca8279_l2_nextValidEntry_get,
    .l2_addr_add                                = dal_ca8279_l2_addr_add,
    .l2_addr_del                                = dal_ca8279_l2_addr_del,
    .l2_addr_get                                = dal_ca8279_l2_addr_get,
    .l2_addr_delAll                             = dal_ca8279_l2_addr_delAll,
    .l2_mcastAddr_add                           = NULL,
    .l2_mcastAddr_del                           = NULL,
    .l2_mcastAddr_get                           = NULL,
    .l2_illegalPortMoveAction_get               = dal_ca8279_l2_illegalPortMoveAction_get,
    .l2_illegalPortMoveAction_set               = dal_ca8279_l2_illegalPortMoveAction_set,
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
    .intr_init = dal_ca8279_intr_init,
    .intr_polarity_set = NULL,
    .intr_polarity_get = NULL,
    .intr_imr_set = dal_ca8279_intr_imr_set,
    .intr_imr_get = dal_ca8279_intr_imr_get,
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
#if defined(CONFIG_COMMON_RT_API)
    .rt_intr_isr_callback_register = dal_rt_ca8279_intr_isr_rx_callback_register,
#endif

    /*cpu*/
    .cpu_init = dal_ca8279_success,
    .cpu_trapInsertTagByPort_set = NULL,
    .cpu_trapInsertTagByPort_get = NULL,
    .cpu_tagAwareByPort_set = NULL,
    .cpu_tagAwareByPort_get = NULL,

    /* QoS */
    .qos_init = dal_ca8279_qos_init,
    .qos_priSelGroup_get = dal_ca8279_qos_priSelGroup_get,
    .qos_priSelGroup_set = dal_ca8279_qos_priSelGroup_set,
    .qos_portPri_get = NULL,
    .qos_portPri_set = NULL,
    .qos_dscpPriRemapGroup_get = NULL,
    .qos_dscpPriRemapGroup_set = NULL,
    .qos_1pPriRemapGroup_get = NULL,
    .qos_1pPriRemapGroup_set = NULL,
    .qos_priMap_get = dal_ca8279_qos_priMap_get,
    .qos_priMap_set = dal_ca8279_qos_priMap_set,
    .qos_portPriMap_get = dal_ca8279_qos_portPriMap_get,
    .qos_portPriMap_set = dal_ca8279_qos_portPriMap_set,
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
    .qos_schedulingQueue_get = dal_ca8279_qos_schedulingQueue_get,
    .qos_schedulingQueue_set = dal_ca8279_qos_schedulingQueue_set,
    .qos_portPriSelGroup_get = dal_ca8279_qos_portPriSelGroup_get,
    .qos_portPriSelGroup_set = dal_ca8279_qos_portPriSelGroup_set,
    .qos_schedulingType_get = NULL,
    .qos_schedulingType_set = NULL,
    .qos_portDot1pRemarkSrcSel_get = NULL,
    .qos_portDot1pRemarkSrcSel_set = NULL,

    /* Classification */
    .classify_init                              = dal_ca8279_success,
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
    .gpon_init = dal_ca8279_gpon_init,
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
    .gpon_onuState_set = dal_ca8279_gpon_onuState_set,
    .gpon_onuState_get = dal_ca8279_gpon_onuState_get,
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
    .gpon_password_set = dal_ca8279_gpon_password_set,

#if defined(CONFIG_COMMON_RT_API)
    /*RT GPON common API*/
    .rt_gpon_init                       = dal_rt_ca8279_gpon_init,    
    .rt_gpon_serialNumber_set           = dal_rt_ca8279_gpon_serialNumber_set,
    .rt_gpon_registrationId_set         = dal_rt_ca8279_gpon_registrationId_set,
    .rt_gpon_activate                   = dal_rt_ca8279_gpon_activate,
    .rt_gpon_deactivate                 = dal_rt_ca8279_gpon_deactivate,
    .rt_gpon_omci_tx                    = dal_rt_ca8279_gpon_omci_tx,
    .rt_gpon_omci_rx_callback_register  = dal_rt_ca8279_gpon_omci_rx_callback_register,
    .rt_gpon_onuState_get               = dal_rt_ca8279_gpon_onuState_get,      
    .rt_gpon_tcont_set                  = dal_rt_ca8279_gpon_tcont_set,
    .rt_gpon_tcont_get                  = dal_rt_ca8279_gpon_tcont_get,
    .rt_gpon_tcont_del                  = dal_rt_ca8279_gpon_tcont_del,
    .rt_gpon_usFlow_set                 = dal_rt_ca8279_gpon_usFlow_set,
    .rt_gpon_usFlow_get                 = dal_rt_ca8279_gpon_usFlow_get,
    .rt_gpon_usFlow_del                 = dal_rt_ca8279_gpon_usFlow_del,
    .rt_gpon_usFlow_delAll              = dal_rt_ca8279_gpon_usFlow_delAll,
    .rt_gpon_dsFlow_set                 = dal_rt_ca8279_gpon_dsFlow_set,
    .rt_gpon_dsFlow_get                 = dal_rt_ca8279_gpon_dsFlow_get,
    .rt_gpon_dsFlow_del                 = dal_rt_ca8279_gpon_dsFlow_del,
    .rt_gpon_dsFlow_delAll              = dal_rt_ca8279_gpon_dsFlow_delAll,
    .rt_gpon_ponQueue_set               = dal_rt_ca8279_gpon_ponQueue_set,
    .rt_gpon_ponQueue_get               = dal_rt_ca8279_gpon_ponQueue_get,
    .rt_gpon_ponQueue_del               = dal_rt_ca8279_gpon_ponQueue_del,
    .rt_gpon_scheInfo_get               = dal_rt_ca8279_gpon_scheInfo_get,
    .rt_gpon_flowCounter_get            = dal_rt_ca8279_gpon_flowCounter_get,
    .rt_gpon_pmCounter_get              = dal_rt_ca8279_gpon_pmCounter_get,
    .rt_gpon_ponTag_get                 = dal_rt_ca8279_gpon_ponTag_get,
    .rt_gpon_msk_set                    = dal_rt_ca8279_gpon_msk_set,
    .rt_gpon_omci_mic_generate          = dal_rt_ca8279_gpon_omci_mic_generate,
    .rt_gpon_mcKey_set                  = dal_rt_ca8279_gpon_mcKey_set,
    .rt_gpon_attribute_get              = dal_rt_ca8279_gpon_attribute_get,
    .rt_gpon_attribute_set              = dal_rt_ca8279_gpon_attribute_set,
    .rt_gpon_omcc_get                   = dal_rt_ca8279_gpon_omcc_get,
    .rt_gpon_omci_mirror_set            = dal_rt_ca8279_gpon_omci_mirror_set,
    .rt_ponmisc_init                    = dal_rt_ca8279_ponmisc_init,
    .rt_ponmisc_mode_get                = dal_rt_ca8279_ponmisc_mode_get,
    .rt_epon_init                       = dal_rt_ca8279_epon_init,  
    .rt_epon_oam_tx                     = dal_rt_ca8279_epon_oam_tx,
    .rt_epon_oam_rx_callback_register   = dal_rt_ca8279_epon_oam_rx_callback_register,
    .rt_epon_dyinggasp_set              = dal_rt_ca8279_epon_dyinggasp_set,
    .rt_epon_llid_entry_set             = dal_rt_ca8279_epon_llid_entry_set,
    .rt_epon_llid_entry_get             = dal_rt_ca8279_epon_llid_entry_get,
    .rt_epon_registerReq_get            = dal_rt_ca8279_epon_registerReq_get,
    .rt_epon_registerReq_set            = dal_rt_ca8279_epon_registerReq_set,
    .rt_epon_churningKey_get            = dal_rt_ca8279_epon_churningKey_get,
    .rt_epon_churningKey_set            = dal_rt_ca8279_epon_churningKey_set,
    .rt_epon_usFecState_get             = dal_rt_ca8279_epon_usFecState_get,
    .rt_epon_usFecState_set             = dal_rt_ca8279_epon_usFecState_set,
    .rt_epon_dsFecState_get             = dal_rt_ca8279_epon_dsFecState_get,
    .rt_epon_dsFecState_set             = dal_rt_ca8279_epon_dsFecState_set,
    .rt_epon_mibCounter_get             = dal_rt_ca8279_epon_mibCounter_get,
    .rt_epon_losState_get               = dal_rt_ca8279_epon_losState_get,

#endif
    /*PON MAC*/
    .ponmac_init = dal_ca8279_ponmac_init,
    .ponmac_queue_add = dal_ca8279_ponmac_queue_add,
    .ponmac_queue_get = dal_ca8279_ponmac_queue_get,
    .ponmac_queue_del = dal_ca8279_ponmac_queue_del,
    .ponmac_flow2Queue_set = NULL,
    .ponmac_flow2Queue_get = NULL,
    .ponmac_mode_get = NULL,
    .ponmac_mode_set = NULL,
    .ponmac_queueDrainOut_set  = NULL,
    .ponmac_opticalPolarity_get = NULL,
    .ponmac_opticalPolarity_set = NULL,

    .ponmac_losState_get = dal_ca8279_ponmac_losState_get,
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
    .ponmac_egrBandwidthCtrlRate_get = dal_ca8279_ponmac_egrBandwidthCtrlRate_get,
    .ponmac_egrBandwidthCtrlRate_set = dal_ca8279_ponmac_egrBandwidthCtrlRate_set,
    .ponmac_egrScheduleIdRate_get = NULL,
    .ponmac_egrScheduleIdRate_set = NULL,
    .ponmac_egrBandwidthCtrlIncludeIfg_get = dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_get,
    .ponmac_egrBandwidthCtrlIncludeIfg_set = dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_set,
    .ponmac_egrScheduleIdIncludeIfg_get = NULL,
    .ponmac_egrScheduleIdIncludeIfg_set = NULL,
    .ponmac_txDisableGpio_get = dal_ca8279_ponmac_txDisableGpio_get,
    .ponmac_txDisableGpio_set = dal_ca8279_ponmac_txDisableGpio_set,
    .ponmac_txPowerDisableGpio_get = dal_ca8279_ponmac_txPowerDisableGpio_get,
    .ponmac_txPowerDisableGpio_set = dal_ca8279_ponmac_txPowerDisableGpio_set,

    /*gpio function*/
    .gpio_init = dal_ca8279_gpio_init,
    .gpio_state_set = dal_ca8279_gpio_state_set,
    .gpio_state_get = dal_ca8279_gpio_state_get,
    .gpio_mode_set = dal_ca8279_gpio_mode_set,
    .gpio_mode_get = dal_ca8279_gpio_mode_get,
    .gpio_databit_get = dal_ca8279_gpio_databit_get,
    .gpio_databit_set = dal_ca8279_gpio_databit_set,
    .gpio_intr_get = NULL,
    .gpio_intr_set = NULL,
    .gpio_intrStatus_get = NULL,
    .gpio_intrStatus_clean = NULL,

    /* RLDP and RLPP */
    .rldp_init = dal_ca8279_success,
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
    .led_init               = dal_ca8279_led_init,
    .led_operation_get      = NULL,
    .led_operation_set      = NULL,
    .led_serialMode_get     = NULL,
    .led_serialMode_set     = NULL,
    .led_blinkRate_get      = dal_ca8279_led_blinkRate_get,
    .led_blinkRate_set      = dal_ca8279_led_blinkRate_set,
    .led_config_set         = dal_ca8279_success,
    .led_config_get         = dal_ca8279_success,
    .led_modeForce_get      = dal_ca8279_led_modeForce_get,
    .led_modeForce_set      = dal_ca8279_led_modeForce_set,
    .led_parallelEnable_get = dal_ca8279_success,
    .led_parallelEnable_set = dal_ca8279_success,
    .led_ponAlarm_get       = NULL,
    .led_ponAlarm_set       = NULL,
    .led_ponWarning_get     = NULL,
    .led_ponWarning_set     = NULL,


    /* I2C */
    .i2c_init = dal_ca8279_i2c_init,
    .i2c_enable_set = dal_ca8279_i2c_enable_set,
    .i2c_enable_get = dal_ca8279_i2c_enable_get,
    .i2c_width_set = NULL,
    .i2c_width_get = NULL,
    .i2c_write = dal_ca8279_i2c_write,
    .i2c_read = dal_ca8279_i2c_read,
    .i2c_clock_set = dal_ca8279_i2c_clock_set,
    .i2c_clock_get = dal_ca8279_i2c_clock_get,
    .i2c_addrWidth_set = NULL,
    .i2c_addrWidth_get = NULL,
    .i2c_dataWidth_set = NULL,
    .i2c_dataWidth_get = NULL,
    .i2c_seq_read = dal_ca8279_i2c_seq_read,
    .i2c_seq_write = dal_ca8279_i2c_seq_write,

    /* EPON */
    .epon_init = dal_ca8279_epon_init,
    .epon_intrMask_get = NULL,
    .epon_intrMask_set = NULL,
    .epon_intr_get = NULL,
    .epon_intr_disableAll = NULL,
    .epon_llid_entry_set = dal_ca8279_epon_llid_entry_set,
    .epon_llid_entry_get = dal_ca8279_epon_llid_entry_get,
    .epon_forceLaserState_set = NULL,
    .epon_forceLaserState_get = NULL,
    .epon_laserTime_set = NULL,
    .epon_laserTime_get = NULL,
    .epon_syncTime_get = NULL,
    .epon_registerReq_get = dal_ca8279_epon_registerReq_get,
    .epon_registerReq_set = dal_ca8279_epon_registerReq_set,
    .epon_churningKey_set = dal_ca8279_epon_churningKey_set,
    .epon_churningKey_get = dal_ca8279_epon_churningKey_get,
    .epon_usFecState_get = dal_ca8279_epon_usFecState_get,
    .epon_usFecState_set = dal_ca8279_epon_usFecState_set,
    .epon_dsFecState_get = dal_ca8279_epon_dsFecState_get,
    .epon_dsFecState_set = dal_ca8279_epon_dsFecState_set,
    .epon_mibCounter_get = dal_ca8279_epon_mibCounter_get,
    .epon_mibGlobal_reset = NULL,
    .epon_mibLlidIdx_reset = NULL,
    .epon_losState_get = dal_ca8279_epon_losState_get,
    .epon_mpcpTimeoutVal_get = NULL,
    .epon_mpcpTimeoutVal_set = NULL,
    .epon_opticalPolarity_set = NULL,
    .epon_opticalPolarity_get = NULL,
    .epon_fecState_get = NULL,
    .epon_fecState_set = NULL,
    .epon_llidEntryNum_get = dal_ca8279_epon_llidEntryNum_get,
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

#else //RG developement using, just registered the RTK API that when develope state need.
static dal_mapper_t dal_ca8279_mapper =
{
    0,//Kem TBD
    ._init = dal_ca8279_success,

    /*sec*/
    .sec_init = dal_ca8279_sec_init,
    .sec_portAttackPreventState_get = dal_ca8279_sec_portAttackPreventState_get,
    .sec_portAttackPreventState_set = dal_ca8279_sec_portAttackPreventState_set,
    .sec_attackPrevent_get = dal_ca8279_sec_attackPrevent_get,
    .sec_attackPrevent_set = dal_ca8279_sec_attackPrevent_set,
    .sec_attackFloodThresh_get = dal_ca8279_sec_attackFloodThresh_get,
    .sec_attackFloodThresh_set = dal_ca8279_sec_attackFloodThresh_set,

    /* statistics */
    .stat_init                                  = dal_ca8279_stat_init,
    .stat_global_reset                          = dal_ca8279_success,
    .stat_port_reset                            = dal_ca8279_stat_port_reset,
    .stat_log_reset                             = dal_ca8279_success,
    .stat_hostCnt_reset                         = dal_ca8279_success,
    .stat_hostCnt_get                           = dal_ca8279_success,
    .stat_hostState_get                         = dal_ca8279_success,
    .stat_hostState_set                         = dal_ca8279_success,
    .stat_rst_cnt_value_set                     = dal_ca8279_success,
    .stat_rst_cnt_value_get                     = dal_ca8279_success,
    .stat_global_get                            = dal_ca8279_success,
    .stat_global_getAll                         = dal_ca8279_success,
    .stat_port_get                              = dal_ca8279_stat_port_get,
    .stat_port_getAll                           = dal_ca8279_stat_port_getAll,
    .stat_log_get                               = dal_ca8279_success,
    .stat_log_ctrl_set                          = dal_ca8279_success,
    .stat_log_ctrl_get                          = dal_ca8279_success,
    .stat_mib_cnt_mode_get                      = dal_ca8279_success,
    .stat_mib_cnt_mode_set                      = dal_ca8279_success,
    .stat_mib_latch_timer_get                   = dal_ca8279_success,
    .stat_mib_latch_timer_set                   = dal_ca8279_success,
    .stat_mib_sync_mode_get                     = dal_ca8279_success,
    .stat_mib_sync_mode_set                     = dal_ca8279_success,
    .stat_pktInfo_get                           = dal_ca8279_success,


    /* Switch */
    .switch_init = dal_ca8279_switch_init,
    .switch_phyPortId_get = dal_ca8279_switch_phyPortId_get,
    .switch_logicalPort_get = dal_ca8279_success,
    .switch_port2PortMask_set = dal_ca8279_success,
    .switch_port2PortMask_clear = dal_ca8279_success,
    .switch_portIdInMask_check = dal_ca8279_success,
    .switch_maxPktLenLinkSpeed_get = dal_ca8279_success,
    .switch_maxPktLenLinkSpeed_set = dal_ca8279_success,
    .switch_mgmtMacAddr_get = dal_ca8279_success,
    .switch_mgmtMacAddr_set = dal_ca8279_success,
    .switch_chip_reset = dal_ca8279_success,
    .switch_version_get = dal_ca8279_switch_version_get,
    .switch_patch_info_get = dal_ca8279_success,
    .switch_csExtId_get = dal_ca8279_success,
    .switch_maxPktLenByPort_get = dal_ca8279_success,
    .switch_maxPktLenByPort_set = dal_ca8279_success,
    .switch_changeDuplex_get = dal_ca8279_success,
    .switch_changeDuplex_set = dal_ca8279_success,
    .switch_system_init = dal_ca8279_success,

    /* Port */
    .port_init                                  = dal_ca8279_port_init,
    .port_link_get                              = dal_ca8279_port_link_get,
    .port_speedDuplex_get                       = dal_ca8279_port_speedDuplex_get,
    .port_flowctrl_get                          = dal_ca8279_port_flowctrl_get,
    .port_phyAutoNegoEnable_get                 = dal_ca8279_success,
    .port_phyAutoNegoEnable_set                 = dal_ca8279_success,
    .port_phyAutoNegoAbility_get                = dal_ca8279_success,
    .port_phyAutoNegoAbility_set                = dal_ca8279_success,
    .port_phyForceModeAbility_get               = dal_ca8279_success,
    .port_phyForceModeAbility_set               = dal_ca8279_success,
    .port_phyMasterSlave_get                    = dal_ca8279_success,
    .port_phyMasterSlave_set                    = dal_ca8279_success,
    .port_phyTestMode_get                       = dal_ca8279_success,
    .port_phyTestMode_set                       = dal_ca8279_success,
    .port_phyReg_get                            = dal_ca8279_port_phyReg_get,
    .port_phyReg_set                            = dal_ca8279_port_phyReg_set,
    .port_cpuPortId_get                         = dal_ca8279_success,
    .port_isolation_get                         = dal_ca8279_success,
    .port_isolation_set                         = dal_ca8279_success,
    .port_isolationExt_get                      = dal_ca8279_success,
    .port_isolationExt_set                      = dal_ca8279_success,
    .port_isolationL34_get                      = dal_ca8279_success,
    .port_isolationL34_set                      = dal_ca8279_success,
    .port_isolationExtL34_get                   = dal_ca8279_success,
    .port_isolationExtL34_set                   = dal_ca8279_success,
    .port_isolationEntry_get                    = dal_ca8279_success,
    .port_isolationEntry_set                    = dal_ca8279_success,
    .port_isolationEntryExt_get                 = dal_ca8279_success,
    .port_isolationEntryExt_set                 = dal_ca8279_success,
    .port_isolationCtagPktConfig_get            = dal_ca8279_success,
    .port_isolationCtagPktConfig_set            = dal_ca8279_success,
    .port_isolationL34PktConfig_get             = dal_ca8279_success,
    .port_isolationL34PktConfig_set             = dal_ca8279_success,
    .port_isolationIpmcLeaky_get                = dal_ca8279_success,
    .port_isolationIpmcLeaky_set                = dal_ca8279_success,
    .port_isolationPortLeaky_get                = dal_ca8279_success,
    .port_isolationPortLeaky_set                = dal_ca8279_success,
    .port_isolationLeaky_get                    = dal_ca8279_success,
    .port_isolationLeaky_set                    = dal_ca8279_success,
    .port_macRemoteLoopbackEnable_get           = dal_ca8279_success,
    .port_macRemoteLoopbackEnable_set           = dal_ca8279_success,
    .port_macLocalLoopbackEnable_get            = dal_ca8279_success,
    .port_macLocalLoopbackEnable_set            = dal_ca8279_success,
    .port_adminEnable_get                       = dal_ca8279_port_adminEnable_get,
    .port_adminEnable_set                       = dal_ca8279_port_adminEnable_set,
    .port_specialCongest_get                    = dal_ca8279_success,
    .port_specialCongest_set                    = dal_ca8279_success,
    .port_specialCongestStatus_get              = dal_ca8279_success,
    .port_specialCongestStatus_clear            = dal_ca8279_success,
    .port_greenEnable_get                       = dal_ca8279_success,
    .port_greenEnable_set                       = dal_ca8279_success,
    .port_phyCrossOverMode_get                  = dal_ca8279_success,
    .port_phyCrossOverMode_set                  = dal_ca8279_success,
    .port_phyPowerDown_set                      = dal_ca8279_port_phyPowerDown_set,
    .port_phyPowerDown_get                      = dal_ca8279_port_phyPowerDown_get,
    .port_enhancedFid_get                       = dal_ca8279_success,
    .port_enhancedFid_set                       = dal_ca8279_success,
    .port_rtctResult_get                        = dal_ca8279_success,
    .port_rtct_start                            = dal_ca8279_success,
    .port_macForceAbility_set                   = dal_ca8279_success,
    .port_macForceAbility_get                   = dal_ca8279_success,
    .port_macForceAbilityState_set              = dal_ca8279_success,
    .port_macForceAbilityState_get              = dal_ca8279_success,
    .port_macExtMode_set                        = dal_ca8279_success,
    .port_macExtMode_get                        = dal_ca8279_success,
    .port_macExtRgmiiDelay_set                  = dal_ca8279_success,
    .port_macExtRgmiiDelay_get                  = dal_ca8279_success,
    .port_gigaLiteEnable_set                    = dal_ca8279_success,
    .port_gigaLiteEnable_get                    = dal_ca8279_success,
    .port_serdesMode_set                        = dal_ca8279_success,
    .port_serdesMode_get                        = dal_ca8279_success,
    .port_serdesNWay_set                        = dal_ca8279_success,
    .port_serdesNWay_get                        = dal_ca8279_success,

    /*oam*/
    .oam_init = dal_ca8279_success,
    .oam_parserAction_set = dal_ca8279_success,
    .oam_parserAction_get = dal_ca8279_success,
    .oam_multiplexerAction_set = dal_ca8279_success,
    .oam_multiplexerAction_get = dal_ca8279_success,

    /* Trap */
    .trap_init                                  = dal_ca8279_trap_init,
    .trap_reasonTrapToCpuPriority_get           = dal_ca8279_success,
    .trap_reasonTrapToCpuPriority_set           = dal_ca8279_success,
    .trap_igmpCtrlPkt2CpuEnable_get             = dal_ca8279_success,
    .trap_igmpCtrlPkt2CpuEnable_set             = dal_ca8279_success,
    .trap_mldCtrlPkt2CpuEnable_get              = dal_ca8279_success,
    .trap_mldCtrlPkt2CpuEnable_set              = dal_ca8279_success,
    .trap_portIgmpMldCtrlPktAction_get          = dal_ca8279_trap_portIgmpMldCtrlPktAction_get,
    .trap_portIgmpMldCtrlPktAction_set          = dal_ca8279_trap_portIgmpMldCtrlPktAction_set,
    .trap_l2McastPkt2CpuEnable_get              = dal_ca8279_success,
    .trap_l2McastPkt2CpuEnable_set              = dal_ca8279_success,
    .trap_ipMcastPkt2CpuEnable_get              = dal_ca8279_success,
    .trap_ipMcastPkt2CpuEnable_set              = dal_ca8279_success,
    .trap_rmaAction_get                         = dal_ca8279_success,
    .trap_rmaAction_set                         = dal_ca8279_success,
    .trap_rmaPri_get                            = dal_ca8279_success,
    .trap_rmaPri_set                            = dal_ca8279_success,
    .trap_oamPduAction_get                      = dal_ca8279_trap_oamPduAction_get,
    .trap_oamPduAction_set                      = dal_ca8279_trap_oamPduAction_set,
    .trap_portOamPduAction_get                  = dal_ca8279_success,
    .trap_portOamPduAction_set                  = dal_ca8279_success,
    .trap_oamPduPri_get                         = dal_ca8279_success,
    .trap_oamPduPri_set                         = dal_ca8279_success,
    .trap_uniTrapPriorityEnable_set             = dal_ca8279_success,
    .trap_uniTrapPriorityEnable_get             = dal_ca8279_success,
    .trap_uniTrapPriorityPriority_set           = dal_ca8279_success,
    .trap_uniTrapPriorityPriority_get           = dal_ca8279_success,
    .trap_cpuTrapHashMask_set                   = dal_ca8279_success,
    .trap_cpuTrapHashMask_get                   = dal_ca8279_success,
    .trap_cpuTrapHashPort_set                   = dal_ca8279_success,
    .trap_cpuTrapHashPort_get                   = dal_ca8279_success,
    .trap_cpuTrapHashState_set                  = dal_ca8279_success,
    .trap_cpuTrapHashState_get                  = dal_ca8279_success,
    .trap_cpuTrapInicHashState_set              = dal_ca8279_success,
    .trap_cpuTrapInicHashState_get              = dal_ca8279_success,
    .trap_cpuTrapInicHashSelect_set             = dal_ca8279_success,
    .trap_cpuTrapInicHashSelect_get             = dal_ca8279_success,

    /*SVLAN*/
    .svlan_init = dal_ca8279_success,
    .svlan_create = dal_ca8279_success,
    .svlan_destroy = dal_ca8279_success,
    .svlan_portSvid_get = dal_ca8279_success,
    .svlan_portSvid_set = dal_ca8279_success,
    .svlan_servicePort_get = dal_ca8279_success,
    .svlan_servicePort_set = dal_ca8279_success,
    .svlan_memberPort_set = dal_ca8279_success,
    .svlan_memberPort_get = dal_ca8279_success,
    .svlan_tpidEntry_get = dal_ca8279_success,
    .svlan_tpidEntry_set = dal_ca8279_success,
    .svlan_priorityRef_set = dal_ca8279_success,
    .svlan_priorityRef_get = dal_ca8279_success,
    .svlan_sp2c_add = dal_ca8279_success,
    .svlan_sp2c_get = dal_ca8279_success,
    .svlan_sp2c_del = dal_ca8279_success,
    .svlan_untagAction_set = dal_ca8279_success,
    .svlan_untagAction_get = dal_ca8279_success,
    .svlan_trapPri_get                          = dal_ca8279_success,
    .svlan_trapPri_set                          = dal_ca8279_success,
    .svlan_deiKeepState_get                     = dal_ca8279_success,
    .svlan_deiKeepState_set                     = dal_ca8279_success,
    .svlan_svlanFunctionEnable_get              = dal_ca8279_success,
    .svlan_svlanFunctionEnable_set              = dal_ca8279_success,
    .svlan_tpidEnable_get                       = dal_ca8279_success,
    .svlan_tpidEnable_set                       = dal_ca8279_success,
    .svlan_sp2cPriority_add                     = dal_ca8279_success,
    .svlan_sp2cPriority_get                     = dal_ca8279_success,
    .svlan_extPortPvid_get                      = dal_ca8279_success,
    .svlan_extPortPvid_set                      = dal_ca8279_success,

     /* VLAN */
    .vlan_init                                  = dal_ca8279_vlan_init,
    .vlan_create                                = dal_ca8279_success,
    .vlan_destroy                               = dal_ca8279_success,
    .vlan_destroyAll                            = dal_ca8279_success,
    .vlan_fid_get                               = dal_ca8279_success,
    .vlan_fid_set                               = dal_ca8279_success,
    .vlan_fidMode_get                           = dal_ca8279_success,
    .vlan_fidMode_set                           = dal_ca8279_success,
    .vlan_port_get                              = dal_ca8279_success,
    .vlan_port_set                              = dal_ca8279_success,
    .vlan_stg_get                               = dal_ca8279_success,
    .vlan_stg_set                               = dal_ca8279_success,
    .vlan_portAcceptFrameType_get               = dal_ca8279_success,
    .vlan_portAcceptFrameType_set               = dal_ca8279_success,
    .vlan_vlanFunctionEnable_get                = dal_ca8279_vlan_vlanFunctionEnable_get,
    .vlan_vlanFunctionEnable_set                = dal_ca8279_vlan_vlanFunctionEnable_set,
    .vlan_portIgrFilterEnable_get               = dal_ca8279_success,
    .vlan_portIgrFilterEnable_set               = dal_ca8279_success,
    .vlan_leaky_get                             = dal_ca8279_success,
    .vlan_leaky_set                             = dal_ca8279_success,
    .vlan_portLeaky_get                         = dal_ca8279_success,
    .vlan_portLeaky_set                         = dal_ca8279_success,
    .vlan_keepType_get                          = dal_ca8279_success,
    .vlan_keepType_set                          = dal_ca8279_success,
    .vlan_portPvid_get                          = dal_ca8279_success,
    .vlan_portPvid_set                          = dal_ca8279_success,
    .vlan_extPortPvid_get                       = dal_ca8279_success,
    .vlan_extPortPvid_set                       = dal_ca8279_success,
    .vlan_protoGroup_get                        = dal_ca8279_success,
    .vlan_protoGroup_set                        = dal_ca8279_success,
    .vlan_portProtoVlan_get                     = dal_ca8279_success,
    .vlan_portProtoVlan_set                     = dal_ca8279_success,
    .vlan_tagMode_get                           = dal_ca8279_success,
    .vlan_tagMode_set                           = dal_ca8279_success,
    .vlan_cfiKeepEnable_get                     = dal_ca8279_success,
    .vlan_cfiKeepEnable_set                     = dal_ca8279_success,
    .vlan_reservedVidAction_get                 = dal_ca8279_success,
    .vlan_reservedVidAction_set                 = dal_ca8279_success,
    .vlan_tagModeIp4mc_get                      = dal_ca8279_success,
    .vlan_tagModeIp4mc_set                      = dal_ca8279_success,
    .vlan_tagModeIp6mc_get                      = dal_ca8279_success,
    .vlan_tagModeIp6mc_set                      = dal_ca8279_success,
    .vlan_lutSvlanHashState_set                 = dal_ca8279_success,
    .vlan_lutSvlanHashState_get                 = dal_ca8279_success,
    .vlan_extPortProtoVlan_set                  = dal_ca8279_success,
    .vlan_extPortProtoVlan_get                  = dal_ca8279_success,
    .vlan_extPortmaskIndex_set                  = dal_ca8279_success,
    .vlan_extPortmaskIndex_get                  = dal_ca8279_success,
    .vlan_extPortmaskCfg_set                    = dal_ca8279_success,
    .vlan_extPortmaskCfg_get                    = dal_ca8279_success,


    /* Mirror */
    .mirror_init                                = dal_ca8279_mirror_init,
    .mirror_portBased_set                       = dal_ca8279_mirror_portBased_set,
    .mirror_portBased_get                       = dal_ca8279_mirror_portBased_get,
    .mirror_portIso_set                         = dal_ca8279_success,
    .mirror_portIso_get                         = dal_ca8279_success,

    /* STP */
    .stp_init = dal_ca8279_success,
    .stp_mstpState_get = dal_ca8279_success,
    .stp_mstpState_set = dal_ca8279_success,

    /*rate*/
    .rate_init = dal_ca8279_rate_init,
    .rate_portIgrBandwidthCtrlRate_get = dal_ca8279_rate_portIgrBandwidthCtrlRate_get,
    .rate_portIgrBandwidthCtrlRate_set = dal_ca8279_rate_portIgrBandwidthCtrlRate_set,
    .rate_portIgrBandwidthCtrlIncludeIfg_get = dal_ca8279_success,
    .rate_portIgrBandwidthCtrlIncludeIfg_set = dal_ca8279_success,
    .rate_portEgrBandwidthCtrlRate_get = dal_ca8279_rate_portEgrBandwidthCtrlRate_get,
    .rate_portEgrBandwidthCtrlRate_set = dal_ca8279_rate_portEgrBandwidthCtrlRate_set,
    .rate_egrBandwidthCtrlIncludeIfg_get = dal_ca8279_success,
    .rate_egrBandwidthCtrlIncludeIfg_set = dal_ca8279_success,
    .rate_portEgrBandwidthCtrlIncludeIfg_get = dal_ca8279_success,
    .rate_portEgrBandwidthCtrlIncludeIfg_set = dal_ca8279_success,
    .rate_egrQueueBwCtrlEnable_get = dal_ca8279_success,
    .rate_egrQueueBwCtrlEnable_set = dal_ca8279_success,
    .rate_egrQueueBwCtrlMeterIdx_get = dal_ca8279_success,
    .rate_egrQueueBwCtrlMeterIdx_set = dal_ca8279_success,
    .rate_egrQueueBwCtrlRate_get = dal_ca8279_success,
    .rate_egrQueueBwCtrlRate_set = dal_ca8279_success,
    .rate_stormControlMeterIdx_get = dal_ca8279_rate_stormControlMeterIdx_get,
    .rate_stormControlMeterIdx_set = dal_ca8279_rate_stormControlMeterIdx_set,
    .rate_stormControlPortEnable_get = dal_ca8279_rate_stormControlPortEnable_get,
    .rate_stormControlPortEnable_set = dal_ca8279_rate_stormControlPortEnable_set,
    .rate_stormControlEnable_get = dal_ca8279_success,
    .rate_stormControlEnable_set = dal_ca8279_success,
    .rate_stormBypass_set = dal_ca8279_success,
    .rate_stormBypass_get = dal_ca8279_success,
    .rate_shareMeter_set = dal_ca8279_rate_shareMeter_set, 
    .rate_shareMeter_get = dal_ca8279_rate_shareMeter_get, 
    .rate_shareMeterBucket_set = dal_ca8279_rate_shareMeterBucket_set,
    .rate_shareMeterBucket_get = dal_ca8279_rate_shareMeterBucket_get,
    .rate_shareMeterExceed_get = dal_ca8279_success,
    .rate_shareMeterExceed_clear = dal_ca8279_success,
    .rate_shareMeterMode_set = dal_ca8279_rate_shareMeterMode_set,
    .rate_shareMeterMode_get = dal_ca8279_rate_shareMeterMode_get,
    .rate_hostIgrBwCtrlState_set = dal_ca8279_success,
    .rate_hostIgrBwCtrlState_get = dal_ca8279_success,
    .rate_hostEgrBwCtrlState_set = dal_ca8279_success,
    .rate_hostEgrBwCtrlState_get = dal_ca8279_success,
    .rate_hostBwCtrlMeterIdx_set = dal_ca8279_success,
    .rate_hostBwCtrlMeterIdx_get = dal_ca8279_success,
    .rate_hostMacAddr_set = dal_ca8279_success,
    .rate_hostMacAddr_get = dal_ca8279_success,
    .rate_hostIgrBwCtrlMeterIdx_get = dal_ca8279_success,
    .rate_hostIgrBwCtrlMeterIdx_set = dal_ca8279_success,
    .rate_hostEgrBwCtrlMeterIdx_get = dal_ca8279_success,
    .rate_hostEgrBwCtrlMeterIdx_set = dal_ca8279_success,



    /* PTP Function */
    .time_portTransparentEnable_set = dal_ca8279_success,
    .time_portTransparentEnable_get = dal_ca8279_success,
    .time_init = dal_ca8279_success,
    .time_portPtpEnable_get = dal_ca8279_success,
    .time_portPtpEnable_set = dal_ca8279_success,
    .time_curTime_latch = dal_ca8279_success,
    .time_curTime_get = dal_ca8279_success,
    .time_refTime_get = dal_ca8279_success,
    .time_refTime_set = dal_ca8279_success,
    .time_frequency_set = dal_ca8279_success,
    .time_frequency_get = dal_ca8279_success,
    .time_ptpIgrMsgAction_set = dal_ca8279_success,
    .time_ptpIgrMsgAction_get = dal_ca8279_success,
    .time_ptpEgrMsgAction_set = dal_ca8279_success,
    .time_ptpEgrMsgAction_get = dal_ca8279_success,
    .time_meanPathDelay_set = dal_ca8279_success,
    .time_meanPathDelay_get = dal_ca8279_success,
    .time_rxTime_set = dal_ca8279_success,
    .time_rxTime_get = dal_ca8279_success,
    .time_ponTodTime_set = dal_ca8279_success,
    .time_ponTodTime_get = dal_ca8279_success,
    .time_portPtpTxIndicator_get = dal_ca8279_success,
    .time_todEnable_get = dal_ca8279_success,
    .time_todEnable_set = dal_ca8279_success,
    .time_ppsEnable_get = dal_ca8279_success,
    .time_ppsEnable_set = dal_ca8279_success,
    .time_ppsMode_get = dal_ca8279_success,
    .time_ppsMode_set = dal_ca8279_success,
    .time_freeTime_get = dal_ca8279_success,

    /*acl*/
    .acl_init = dal_ca8279_success,
    .acl_template_set = dal_ca8279_success,
    .acl_template_get = dal_ca8279_success,
    .acl_fieldSelect_set = dal_ca8279_success,
    .acl_fieldSelect_get = dal_ca8279_success,
    .acl_igrRuleEntry_get = dal_ca8279_success,
    .acl_igrRuleField_add = dal_ca8279_success,
    .acl_igrRuleEntry_add = dal_ca8279_success,
    .acl_igrRuleEntry_del = dal_ca8279_success,
    .acl_igrRuleEntry_delAll = dal_ca8279_success,
    .acl_igrUnmatchAction_set = dal_ca8279_success,
    .acl_igrUnmatchAction_get = dal_ca8279_success,
    .acl_igrState_set = dal_ca8279_success,
    .acl_igrState_get = dal_ca8279_success,
    .acl_ipRange_set = dal_ca8279_success,
    .acl_ipRange_get = dal_ca8279_success,
    .acl_vidRange_set = dal_ca8279_success,
    .acl_vidRange_get = dal_ca8279_success,
    .acl_portRange_set = dal_ca8279_success,
    .acl_portRange_get = dal_ca8279_success,
    .acl_packetLengthRange_set = dal_ca8279_success,
    .acl_packetLengthRange_get = dal_ca8279_success,
    .acl_igrPermitState_set = dal_ca8279_success,
    .acl_igrPermitState_get = dal_ca8279_success,
    .acl_dbgInfo_get = dal_ca8279_success,
    .acl_dbgHitReason_get = dal_ca8279_success,


     /* L2 */
    .l2_init                                    = dal_ca8279_l2_init,
    .l2_flushLinkDownPortAddrEnable_get         = dal_ca8279_l2_flushLinkDownPortAddrEnable_get,
    .l2_flushLinkDownPortAddrEnable_set         = dal_ca8279_l2_flushLinkDownPortAddrEnable_set,
    .l2_ucastAddr_flush                         = dal_ca8279_success,
    .l2_table_clear                             = dal_ca8279_success,
    .l2_limitLearningOverStatus_get             = dal_ca8279_success,
    .l2_limitLearningOverStatus_clear           = dal_ca8279_success,
    .l2_learningCnt_get                         = dal_ca8279_success,
    .l2_limitLearningCnt_get                    = dal_ca8279_success,
    .l2_limitLearningCnt_set                    = dal_ca8279_success,
    .l2_limitLearningCntAction_get              = dal_ca8279_success,
    .l2_limitLearningCntAction_set              = dal_ca8279_success,
    .l2_limitLearningEntryAction_get            = dal_ca8279_success,
    .l2_limitLearningEntryAction_set            = dal_ca8279_success,
    .l2_limitLearningPortMask_get               = dal_ca8279_success,
    .l2_limitLearningPortMask_set               = dal_ca8279_success,
    .l2_portLimitLearningOverStatus_get         = dal_ca8279_success,
    .l2_portLimitLearningOverStatus_clear       = dal_ca8279_success,
    .l2_portLearningCnt_get                     = dal_ca8279_success,
    .l2_portLimitLearningCnt_get                = dal_ca8279_l2_portLimitLearningCnt_get,
    .l2_portLimitLearningCnt_set                = dal_ca8279_l2_portLimitLearningCnt_set,
    .l2_portLimitLearningCntAction_get          = dal_ca8279_l2_portLimitLearningCntAction_get,
    .l2_portLimitLearningCntAction_set          = dal_ca8279_l2_portLimitLearningCntAction_set,
    .l2_aging_get                               = dal_ca8279_l2_aging_get,
    .l2_aging_set                               = dal_ca8279_l2_aging_set,
    .l2_portAgingEnable_get                     = dal_ca8279_l2_portAgingEnable_get,
    .l2_portAgingEnable_set                     = dal_ca8279_l2_portAgingEnable_set,
    .l2_lookupMissAction_get                    = dal_ca8279_success,
    .l2_lookupMissAction_set                    = dal_ca8279_success,
    .l2_portLookupMissAction_get                = dal_ca8279_l2_portLookupMissAction_get,
    .l2_portLookupMissAction_set                = dal_ca8279_l2_portLookupMissAction_set,
    .l2_lookupMissFloodPortMask_get             = dal_ca8279_success,
    .l2_lookupMissFloodPortMask_set             = dal_ca8279_success,
    .l2_lookupMissFloodPortMask_add             = dal_ca8279_success,
    .l2_lookupMissFloodPortMask_del             = dal_ca8279_success,
    .l2_newMacOp_get                            = dal_ca8279_success,
    .l2_newMacOp_set                            = dal_ca8279_success,
    .l2_nextValidAddr_get                       = dal_ca8279_l2_nextValidAddr_get,
    .l2_nextValidAddrOnPort_get                 = dal_ca8279_success,
    .l2_nextValidMcastAddr_get                  = dal_ca8279_success,
    .l2_nextValidIpMcastAddr_get                = dal_ca8279_success,
    .l2_nextValidEntry_get                      = dal_ca8279_l2_nextValidEntry_get,
    .l2_addr_add                                = dal_ca8279_l2_addr_add,
    .l2_addr_del                                = dal_ca8279_l2_addr_del,
    .l2_addr_get                                = dal_ca8279_l2_addr_get,
    .l2_addr_delAll                             = dal_ca8279_l2_addr_delAll,
    .l2_mcastAddr_add                           = dal_ca8279_success,
    .l2_mcastAddr_del                           = dal_ca8279_success,
    .l2_mcastAddr_get                           = dal_ca8279_success,
    .l2_illegalPortMoveAction_get               = dal_ca8279_l2_illegalPortMoveAction_get,
    .l2_illegalPortMoveAction_set               = dal_ca8279_l2_illegalPortMoveAction_set,
    .l2_ipmcMode_get                            = dal_ca8279_success,
    .l2_ipmcMode_set                            = dal_ca8279_success,
    .l2_ipmcVlanMode_get                        = dal_ca8279_success,
    .l2_ipmcVlanMode_set                        = dal_ca8279_success,
    .l2_ipv6mcMode_get                          = dal_ca8279_success,
    .l2_ipv6mcMode_set                          = dal_ca8279_success,
    .l2_ipmcGroupLookupMissHash_get             = dal_ca8279_success,
    .l2_ipmcGroupLookupMissHash_set             = dal_ca8279_success,
    .l2_ipmcGroup_add                           = dal_ca8279_success,
    .l2_ipmcGroupExtPortmask_add                = dal_ca8279_success,
    .l2_ipmcGroupExtMemberIdx_add               = dal_ca8279_success,
    .l2_ipmcGroup_del                           = dal_ca8279_success,
    .l2_ipmcGroup_get                           = dal_ca8279_success,
    .l2_ipmcGroupExtPortmask_get                = dal_ca8279_success,
    .l2_ipmcGroupExtMemberIdx_get               = dal_ca8279_success,
    .l2_ipmcSipFilter_set                       = dal_ca8279_success,
    .l2_ipmcSipFilter_get                       = dal_ca8279_success,
    .l2_portIpmcAction_get                      = dal_ca8279_success,
    .l2_portIpmcAction_set                      = dal_ca8279_success,
    .l2_ipMcastAddr_add                         = dal_ca8279_success,
    .l2_ipMcastAddr_del                         = dal_ca8279_success,
    .l2_ipMcastAddr_get                         = dal_ca8279_success,
    .l2_srcPortEgrFilterMask_get                = dal_ca8279_success,
    .l2_srcPortEgrFilterMask_set                = dal_ca8279_success,
    .l2_extPortEgrFilterMask_get                = dal_ca8279_success,
    .l2_extPortEgrFilterMask_set                = dal_ca8279_success,
    .l2_camState_set                            = dal_ca8279_success,
    .l2_camState_get                            = dal_ca8279_success,
    .l2_vidUnmatchAction_get                    = dal_ca8279_success,
    .l2_vidUnmatchAction_set                    = dal_ca8279_success,
    .l2_extMemberConfig_get                     = dal_ca8279_success,
    .l2_extMemberConfig_set                     = dal_ca8279_success,
    .l2_ip6mcReservedAddrEnable_set             = dal_ca8279_success,
    .l2_ip6mcReservedAddrEnable_get             = dal_ca8279_success,
    .l2_hashValue_get                           = dal_ca8279_success,
    
     /*interrupt*/
    .intr_init = dal_ca8279_intr_init,
    .intr_polarity_set = dal_ca8279_success,
    .intr_polarity_get = dal_ca8279_success,
    .intr_imr_set = dal_ca8279_intr_imr_set,
    .intr_imr_get = dal_ca8279_intr_imr_get,
    .intr_ims_get = dal_ca8279_success,
    .intr_ims_clear = dal_ca8279_success,
    .intr_speedChangeStatus_get = dal_ca8279_success,
    .intr_speedChangeStatus_clear = dal_ca8279_success,
    .intr_linkupStatus_get = dal_ca8279_success,
    .intr_linkupStatus_clear = dal_ca8279_success,
    .intr_linkdownStatus_get = dal_ca8279_success,
    .intr_linkdownStatus_clear = dal_ca8279_success,
    .intr_gphyStatus_get = dal_ca8279_success,
    .intr_gphyStatus_clear = dal_ca8279_success,
    .intr_imr_restore = dal_ca8279_success,
    .intr_isr_set = dal_ca8279_success,
    .intr_isr_get = dal_ca8279_success,
    .intr_isr_counter_dump = dal_ca8279_success,

    /*cpu*/
    .cpu_init = dal_ca8279_success,
    .cpu_trapInsertTagByPort_set = dal_ca8279_success,
    .cpu_trapInsertTagByPort_get = dal_ca8279_success,
    .cpu_tagAwareByPort_set = dal_ca8279_success,
    .cpu_tagAwareByPort_get = dal_ca8279_success,

    /* QoS */
    .qos_init = dal_ca8279_qos_init,
    .qos_priSelGroup_get = dal_ca8279_success,
    .qos_priSelGroup_set = dal_ca8279_success,
    .qos_portPri_get = dal_ca8279_success,
    .qos_portPri_set = dal_ca8279_success,
    .qos_dscpPriRemapGroup_get = dal_ca8279_success,
    .qos_dscpPriRemapGroup_set = dal_ca8279_success,
    .qos_1pPriRemapGroup_get = dal_ca8279_success,
    .qos_1pPriRemapGroup_set = dal_ca8279_success,
    .qos_priMap_get = dal_ca8279_success,
    .qos_priMap_set = dal_ca8279_success,
    .qos_portPriMap_get = dal_ca8279_success,
    .qos_portPriMap_set = dal_ca8279_success,
    .qos_1pRemarkEnable_get = dal_ca8279_success,
    .qos_1pRemarkEnable_set = dal_ca8279_success,
    .qos_1pRemarkGroup_get = dal_ca8279_success,
    .qos_1pRemarkGroup_set = dal_ca8279_success,
    .qos_dscpRemarkEnable_get = dal_ca8279_success,
    .qos_dscpRemarkEnable_set = dal_ca8279_success,
    .qos_dscpRemarkGroup_get = dal_ca8279_success,
    .qos_dscpRemarkGroup_set = dal_ca8279_success,
    .qos_fwd2CpuPriRemap_get = dal_ca8279_success,
    .qos_fwd2CpuPriRemap_set = dal_ca8279_success,
    .qos_portDscpRemarkSrcSel_get = dal_ca8279_success,
    .qos_portDscpRemarkSrcSel_set = dal_ca8279_success,
    .qos_dscp2DscpRemarkGroup_get = dal_ca8279_success,
    .qos_dscp2DscpRemarkGroup_set = dal_ca8279_success,
    .qos_schedulingQueue_get = dal_ca8279_qos_schedulingQueue_get,
    .qos_schedulingQueue_set = dal_ca8279_qos_schedulingQueue_set,
    .qos_portPriSelGroup_get = dal_ca8279_success,
    .qos_portPriSelGroup_set = dal_ca8279_success,
    .qos_schedulingType_get = dal_ca8279_success,
    .qos_schedulingType_set = dal_ca8279_success,
    .qos_portDot1pRemarkSrcSel_get = dal_ca8279_success,
    .qos_portDot1pRemarkSrcSel_set = dal_ca8279_success,

    /* Classification */
    .classify_init                              = dal_ca8279_success,
    .classify_cfgEntry_add                      = dal_ca8279_success,
    .classify_cfgEntry_get                      = dal_ca8279_success,
    .classify_cfgEntry_del                      = dal_ca8279_success,
    .classify_field_add                         = dal_ca8279_success,
    .classify_unmatchAction_set                 = dal_ca8279_success,
    .classify_unmatchAction_get                 = dal_ca8279_success,
    .classify_cf_sel_set                        = dal_ca8279_success,
    .classify_cf_sel_get                        = dal_ca8279_success,
    .classify_trapPri_set                       = dal_ca8279_success,
    .classify_trapPri_get                       = dal_ca8279_success,

    /* GPON */
    .gpon_init = dal_ca8279_gpon_init,
    .gpon_resetState_set = dal_ca8279_success,
    .gpon_resetDoneState_get = dal_ca8279_success,
    .gpon_version_get = dal_ca8279_success,
    .gpon_test_get = dal_ca8279_success,
    .gpon_test_set = dal_ca8279_success,
    .gpon_topIntrMask_get = dal_ca8279_success,
    .gpon_topIntrMask_set = dal_ca8279_success,
    .gpon_topIntr_get = dal_ca8279_success,
    .gpon_topIntr_disableAll = dal_ca8279_success,
    .gpon_gtcDsIntr_get = dal_ca8279_success,
    .gpon_gtcDsIntrDlt_get = dal_ca8279_success,
    .gpon_gtcDsIntrMask_get = dal_ca8279_success,
    .gpon_gtcDsIntrMask_set = dal_ca8279_success,
    .gpon_onuId_set = dal_ca8279_success,
    .gpon_onuId_get = dal_ca8279_success,
    .gpon_onuState_set = dal_ca8279_gpon_onuState_set,
    .gpon_onuState_get = dal_ca8279_gpon_onuState_get,
    .gpon_dsBwmapCrcCheckState_set = dal_ca8279_success,
    .gpon_dsBwmapCrcCheckState_get = dal_ca8279_success,
    .gpon_dsBwmapFilterOnuIdState_set = dal_ca8279_success,
    .gpon_dsBwmapFilterOnuIdState_get = dal_ca8279_success,
    .gpon_dsPlendStrictMode_set = dal_ca8279_success,
    .gpon_dsPlendStrictMode_get = dal_ca8279_success,
    .gpon_dsScrambleState_set = dal_ca8279_success,
    .gpon_dsScrambleState_get = dal_ca8279_success,
    .gpon_dsFecBypass_set = dal_ca8279_success,
    .gpon_dsFecBypass_get = dal_ca8279_success,
    .gpon_dsFecThrd_set = dal_ca8279_success,
    .gpon_dsFecThrd_get = dal_ca8279_success,
    .gpon_extraSnTxTimes_set = dal_ca8279_success,
    .gpon_extraSnTxTimes_get = dal_ca8279_success,
    .gpon_dsPloamNomsg_set = dal_ca8279_success,
    .gpon_dsPloamNomsg_get = dal_ca8279_success,
    .gpon_dsPloamOnuIdFilterState_set = dal_ca8279_success,
    .gpon_dsPloamOnuIdFilterState_get = dal_ca8279_success,
    .gpon_dsPloamBcAcceptState_set = dal_ca8279_success,
    .gpon_dsPloamBcAcceptState_get = dal_ca8279_success,
    .gpon_dsPloamDropCrcState_set = dal_ca8279_success,
    .gpon_dsPloamDropCrcState_get = dal_ca8279_success,
    .gpon_cdrLosStatus_get = dal_ca8279_success,
    .gpon_optLosStatus_get = dal_ca8279_success,
    .gpon_losCfg_set = dal_ca8279_success,
    .gpon_losCfg_get = dal_ca8279_success,
    .gpon_dsPloam_get = dal_ca8279_success,
    .gpon_dsPloam_getAll = dal_ca8279_success,
    .gpon_tcont_get = dal_ca8279_success,
    .gpon_tcont_set = dal_ca8279_success,
    .gpon_tcont_del = dal_ca8279_success,
    .gpon_dsGemPort_get = dal_ca8279_success,
    .gpon_dsGemPort_set = dal_ca8279_success,
    .gpon_dsGemPort_del = dal_ca8279_success,
    .gpon_dsGemPortPktCnt_get = dal_ca8279_success,
    .gpon_dsGemPortByteCnt_get = dal_ca8279_success,
    .gpon_dsGtcMiscCnt_get = dal_ca8279_success,
    .gpon_dsOmciPti_set = dal_ca8279_success,
    .gpon_dsOmciPti_get = dal_ca8279_success,
    .gpon_dsEthPti_set = dal_ca8279_success,
    .gpon_dsEthPti_get = dal_ca8279_success,
    .gpon_aesKeySwitch_set = dal_ca8279_success,
    .gpon_aesKeySwitch_get = dal_ca8279_success,
    .gpon_aesKeyWord_set = dal_ca8279_success,
    .gpon_aesKeyWordActive_set = dal_ca8279_success,
    .gpon_irq_get = dal_ca8279_success,
    .gpon_dsGemPortEthRxCnt_get = dal_ca8279_success,
    .gpon_dsGemPortEthFwdCnt_get = dal_ca8279_success,
    .gpon_dsGemPortMiscCnt_get = dal_ca8279_success,
    .gpon_dsGemPortFcsCheckState_get = dal_ca8279_success,
    .gpon_dsGemPortFcsCheckState_set = dal_ca8279_success,
    .gpon_dsGemPortBcPassState_set = dal_ca8279_success,
    .gpon_dsGemPortBcPassState_get = dal_ca8279_success,
    .gpon_dsGemPortNonMcPassState_set = dal_ca8279_success,
    .gpon_dsGemPortNonMcPassState_get = dal_ca8279_success,
    .gpon_dsGemPortMacFilterMode_set = dal_ca8279_success,
    .gpon_dsGemPortMacFilterMode_get = dal_ca8279_success,
    .gpon_dsGemPortMacForceMode_set = dal_ca8279_success,
    .gpon_dsGemPortMacForceMode_get = dal_ca8279_success,
    .gpon_dsGemPortMacEntry_set = dal_ca8279_success,
    .gpon_dsGemPortMacEntry_get = dal_ca8279_success,
    .gpon_dsGemPortMacEntry_del = dal_ca8279_success,
    .gpon_dsGemPortFrameTimeOut_set = dal_ca8279_success,
    .gpon_dsGemPortFrameTimeOut_get = dal_ca8279_success,
    .gpon_ipv4McAddrPtn_get = dal_ca8279_success,
    .gpon_ipv4McAddrPtn_set = dal_ca8279_success,
    .gpon_ipv6McAddrPtn_get = dal_ca8279_success,
    .gpon_ipv6McAddrPtn_set = dal_ca8279_success,
    .gpon_gtcUsIntr_get = dal_ca8279_success,
    .gpon_gtcUsIntrDlt_get = dal_ca8279_success,
    .gpon_gtcUsIntrMask_get = dal_ca8279_success,
    .gpon_gtcUsIntrMask_set = dal_ca8279_success,
    .gpon_forceLaser_set = dal_ca8279_success,
    .gpon_forceLaser_get = dal_ca8279_success,
    .gpon_forcePRBS_set = dal_ca8279_success,
    .gpon_forcePRBS_get = dal_ca8279_success,
    .gpon_ploamState_set = dal_ca8279_success,
    .gpon_ploamState_get = dal_ca8279_success,
    .gpon_indNrmPloamState_set = dal_ca8279_success,
    .gpon_indNrmPloamState_get = dal_ca8279_success,
    .gpon_dbruState_set = dal_ca8279_success,
    .gpon_dbruState_get = dal_ca8279_success,
    .gpon_usScrambleState_set = dal_ca8279_success,
    .gpon_usScrambleState_get = dal_ca8279_success,
    .gpon_usBurstPolarity_set = dal_ca8279_success,
    .gpon_usBurstPolarity_get = dal_ca8279_success,
    .gpon_eqd_set = dal_ca8279_success,
    .gpon_eqd_get = dal_ca8279_success,
    .gpon_laserTime_set = dal_ca8279_success,
    .gpon_laserTime_get = dal_ca8279_success,
    .gpon_burstOverhead_set = dal_ca8279_success,
    .gpon_usPloam_set = dal_ca8279_success,
    .gpon_usAutoPloam_set = dal_ca8279_success,
    .gpon_usPloamCrcGenState_set = dal_ca8279_success,
    .gpon_usPloamCrcGenState_get = dal_ca8279_success,
    .gpon_usPloamOnuIdFilterState_set = dal_ca8279_success,
    .gpon_usPloamOnuIdFilter_get = dal_ca8279_success,
    .gpon_usPloamBuf_flush = dal_ca8279_success,
    .gpon_usGtcMiscCnt_get = dal_ca8279_success,
    .gpon_rdi_set = dal_ca8279_success,
    .gpon_rdi_get = dal_ca8279_success,
    .gpon_usSmalSstartProcState_set = dal_ca8279_success,
    .gpon_usSmalSstartProcState_get = dal_ca8279_success,
    .gpon_usSuppressLaserState_set = dal_ca8279_success,
    .gpon_usSuppressLaserState_get = dal_ca8279_success,
    .gpon_gemUsIntr_get = dal_ca8279_success,
    .gpon_gemUsIntrMask_get = dal_ca8279_success,
    .gpon_gemUsIntrMask_set = dal_ca8279_success,
    .gpon_gemUsForceIdleState_set = dal_ca8279_success,
    .gpon_gemUsForceIdleState_get = dal_ca8279_success,
    .gpon_gemUsPtiVector_set = dal_ca8279_success,
    .gpon_gemUsPtiVector_get = dal_ca8279_success,
    .gpon_gemUsEthCnt_get = dal_ca8279_success,
    .gpon_gemUsGemCnt_get = dal_ca8279_success,
    .gpon_gemUsPortCfg_set = dal_ca8279_success,
    .gpon_gemUsPortCfg_get = dal_ca8279_success,
    .gpon_gemUsDataByteCnt_get = dal_ca8279_success,
    .gpon_gemUsIdleByteCnt_get = dal_ca8279_success,
    .gpon_dbruPeriod_get = dal_ca8279_success,
    .gpon_dbruPeriod_set = dal_ca8279_success,
    .gpon_gtcDsIntrDlt_check = dal_ca8279_success,
    .gpon_gtcUsIntrDlt_check = dal_ca8279_success,
    .gpon_gemUsIntrDlt_check = dal_ca8279_success,
    .gpon_rogueOnt_set = dal_ca8279_success,
    .gpon_drainOutDefaultQueue_set = dal_ca8279_success,
    .gpon_autoDisTx_set = dal_ca8279_success,
    .gpon_scheInfo_get = dal_ca8279_success,
    .gpon_dataPath_reset = dal_ca8279_success,
    .gpon_dsOmciCnt_get = dal_ca8279_success,
    .gpon_usOmciCnt_get = dal_ca8279_success,
    .gpon_gtcDsTodSuperFrame_set = dal_ca8279_success,
    .gpon_gtcDsTodSuperFrame_get = dal_ca8279_success,
    .gpon_dbruBlockSize_get = dal_ca8279_success,
    .gpon_dbruBlockSize_set = dal_ca8279_success,
    .gpon_usLaserDefault_get = dal_ca8279_success,
    .gpon_flowctrl_adjust_byFlowNum = dal_ca8279_success,

    /*PON MAC*/
    .ponmac_init = dal_ca8279_ponmac_init,
    .ponmac_queue_add = dal_ca8279_ponmac_queue_add,
    .ponmac_queue_get = dal_ca8279_ponmac_queue_get,
    .ponmac_queue_del = dal_ca8279_ponmac_queue_del,
    .ponmac_flow2Queue_set = dal_ca8279_success,
    .ponmac_flow2Queue_get = dal_ca8279_success,
    .ponmac_mode_get = dal_ca8279_success,
    .ponmac_mode_set = dal_ca8279_success,
    .ponmac_queueDrainOut_set  = dal_ca8279_success,
    .ponmac_opticalPolarity_get = dal_ca8279_success,
    .ponmac_opticalPolarity_set = dal_ca8279_success,

    .ponmac_losState_get = dal_ca8279_ponmac_losState_get,
    .ponmac_serdesCdr_reset = dal_ca8279_success,
    .ponmac_linkState_get = dal_ca8279_success,

    .ponmac_forceBerNotify_set = dal_ca8279_success,
    .ponmac_bwThreshold_set = dal_ca8279_success,
    .ponmac_bwThreshold_get = dal_ca8279_success,
    .ponmac_maxPktLen_set = dal_ca8279_success,
    .ponmac_sidValid_get = dal_ca8279_success,
    .ponmac_sidValid_set = dal_ca8279_success,
    .ponmac_schedulingType_get = dal_ca8279_success,
    .ponmac_schedulingType_set = dal_ca8279_success,
    .ponmac_egrBandwidthCtrlRate_get = dal_ca8279_ponmac_egrBandwidthCtrlRate_get,
    .ponmac_egrBandwidthCtrlRate_set = dal_ca8279_ponmac_egrBandwidthCtrlRate_set,
    .ponmac_egrScheduleIdRate_get = dal_ca8279_success,
    .ponmac_egrScheduleIdRate_set = dal_ca8279_success,
    .ponmac_egrBandwidthCtrlIncludeIfg_get = dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_get,
    .ponmac_egrBandwidthCtrlIncludeIfg_set = dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_set,
    .ponmac_egrScheduleIdIncludeIfg_get = dal_ca8279_success,
    .ponmac_egrScheduleIdIncludeIfg_set = dal_ca8279_success,
    .ponmac_txDisableGpio_get = dal_ca8279_ponmac_txDisableGpio_get,
    .ponmac_txDisableGpio_set = dal_ca8279_ponmac_txDisableGpio_set,
    .ponmac_txPowerDisableGpio_get = dal_ca8279_ponmac_txPowerDisableGpio_get,
    .ponmac_txPowerDisableGpio_set = dal_ca8279_ponmac_txPowerDisableGpio_set,

    /*gpio function*/
    .gpio_init = dal_ca8279_gpio_init,
    .gpio_state_set = dal_ca8279_gpio_state_set,
    .gpio_state_get = dal_ca8279_gpio_state_get,
    .gpio_mode_set = dal_ca8279_gpio_mode_set,
    .gpio_mode_get = dal_ca8279_gpio_mode_get,
    .gpio_databit_get = dal_ca8279_gpio_databit_get,
    .gpio_databit_set = dal_ca8279_gpio_databit_set,
    .gpio_intr_get = dal_ca8279_success,
    .gpio_intr_set = dal_ca8279_success,
    .gpio_intrStatus_get = dal_ca8279_success,
    .gpio_intrStatus_clean = dal_ca8279_success,

    /* RLDP and RLPP */
    .rldp_init = dal_ca8279_success,
    .rldp_config_set = dal_ca8279_success,
    .rldp_config_get = dal_ca8279_success,
    .rldp_portConfig_set = dal_ca8279_success,
    .rldp_portConfig_get = dal_ca8279_success,
    .rldp_status_get = dal_ca8279_success,
    .rldp_portStatus_get = dal_ca8279_success,
    .rldp_portStatus_clear = dal_ca8279_success,
    .rlpp_init = dal_ca8279_success,
    .rlpp_trapType_set = dal_ca8279_success,
    .rlpp_trapType_get = dal_ca8279_success,

    /*led*/
    .led_init               = dal_ca8279_led_init,
    .led_operation_get      = dal_ca8279_success,
    .led_operation_set      = dal_ca8279_success,
    .led_serialMode_get     = dal_ca8279_success,
    .led_serialMode_set     = dal_ca8279_success,
    .led_blinkRate_get      = dal_ca8279_led_blinkRate_get,
    .led_blinkRate_set      = dal_ca8279_led_blinkRate_set,
    .led_config_set         = dal_ca8279_success,
    .led_config_get         = dal_ca8279_success,
    .led_modeForce_get      = dal_ca8279_led_modeForce_get,
    .led_modeForce_set      = dal_ca8279_led_modeForce_set,
    .led_parallelEnable_get = dal_ca8279_success,
    .led_parallelEnable_set = dal_ca8279_success,
    .led_ponAlarm_get       = dal_ca8279_success,
    .led_ponAlarm_set       = dal_ca8279_success,
    .led_ponWarning_get     = dal_ca8279_success,
    .led_ponWarning_set     = dal_ca8279_success,


    /* I2C */
    .i2c_init = dal_ca8279_i2c_init,
    .i2c_enable_set = dal_ca8279_i2c_enable_set,
    .i2c_enable_get = dal_ca8279_i2c_enable_get,
    .i2c_width_set = dal_ca8279_success,
    .i2c_width_get = dal_ca8279_success,
    .i2c_write = dal_ca8279_i2c_write,
    .i2c_read = dal_ca8279_i2c_read,
    .i2c_clock_set = dal_ca8279_success,
    .i2c_clock_get = dal_ca8279_success,
    .i2c_addrWidth_set = dal_ca8279_success,
    .i2c_addrWidth_get = dal_ca8279_success,
    .i2c_dataWidth_set = dal_ca8279_success,
    .i2c_dataWidth_get = dal_ca8279_success,

    /* EPON */
    .epon_init = dal_ca8279_epon_init,
    .epon_intrMask_get = dal_ca8279_success,
    .epon_intrMask_set = dal_ca8279_success,
    .epon_intr_get = dal_ca8279_success,
    .epon_intr_disableAll = dal_ca8279_success,
    .epon_llid_entry_set = dal_ca8279_epon_llid_entry_set,
    .epon_llid_entry_get = dal_ca8279_epon_llid_entry_get,
    .epon_forceLaserState_set = dal_ca8279_success,
    .epon_forceLaserState_get = dal_ca8279_success,
    .epon_laserTime_set = dal_ca8279_success,
    .epon_laserTime_get = dal_ca8279_success,
    .epon_syncTime_get = dal_ca8279_success,
    .epon_registerReq_get = dal_ca8279_epon_registerReq_get,
    .epon_registerReq_set = dal_ca8279_epon_registerReq_set,
    .epon_churningKey_set = dal_ca8279_epon_churningKey_set,
    .epon_churningKey_get = dal_ca8279_epon_churningKey_get,
    .epon_usFecState_get = dal_ca8279_epon_usFecState_get,
    .epon_usFecState_set = dal_ca8279_epon_usFecState_set,
    .epon_dsFecState_get = dal_ca8279_epon_dsFecState_get,
    .epon_dsFecState_set = dal_ca8279_epon_dsFecState_set,
    .epon_mibCounter_get = dal_ca8279_epon_mibCounter_get,
    .epon_mibGlobal_reset = dal_ca8279_success,
    .epon_mibLlidIdx_reset = dal_ca8279_success,
    .epon_losState_get = dal_ca8279_epon_losState_get,
    .epon_mpcpTimeoutVal_get = dal_ca8279_success,
    .epon_mpcpTimeoutVal_set = dal_ca8279_success,
    .epon_opticalPolarity_set = dal_ca8279_success,
    .epon_opticalPolarity_get = dal_ca8279_success,
    .epon_fecState_get = dal_ca8279_success,
    .epon_fecState_set = dal_ca8279_success,
    .epon_llidEntryNum_get = dal_ca8279_epon_llidEntryNum_get,
    .epon_thresholdReport_set = dal_ca8279_success,
    .epon_thresholdReport_get = dal_ca8279_success,
    .epon_dbgInfo_get = dal_ca8279_success,
    .epon_multiLlidMode_set = dal_ca8279_success,
    .epon_multiLlidMode_get = dal_ca8279_success,
    .epon_reportQueueSet_set = dal_ca8279_success,
    .epon_reportQueueSet_get = dal_ca8279_success,
    .epon_queueThresholdReport_set = dal_ca8279_success,
    .epon_queueThresholdReport_get = dal_ca8279_success,
    .epon_autoPowerSavingState_get = dal_ca8279_success,
    .epon_autoPowerSavingState_set = dal_ca8279_success,
    .epon_autoPowerSavingOeRecoverTime_get = dal_ca8279_success,
    .epon_autoPowerSavingOeRecoverTime_set = dal_ca8279_success,



    .ppstod_init = dal_ca8279_success,
    .ppstod_delay_get = dal_ca8279_success,
    .ppstod_delay_set = dal_ca8279_success,
    .ppstod_pulseWidth_get = dal_ca8279_success,
    .ppstod_pulseWidth_set = dal_ca8279_success,
    .ppstod_mode_get = dal_ca8279_success,
    .ppstod_mode_set = dal_ca8279_success,
    .ppstod_sarpGpsWeek_get = dal_ca8279_success,
    .ppstod_sarpGpsWeek_set = dal_ca8279_success,
    .ppstod_sarpUtcSec_get = dal_ca8279_success,
    .ppstod_sarpUtcSec_set = dal_ca8279_success,
    .ppstod_frameLen_get = dal_ca8279_success,
    .ppstod_frameLen_set = dal_ca8279_success,
    .ppstod_frameData_get = dal_ca8279_success,
    .ppstod_frameData_set = dal_ca8279_success,
    .ppstod_baudrate_get = dal_ca8279_success,
    .ppstod_baudrate_set = dal_ca8279_success,

    /* PBO */
    /* PON PBO */
    .pbo_init = dal_ca8279_success,
    /* Switch PBO */
    .swPbo_init = dal_ca8279_success,
    .swPbo_portState_set = dal_ca8279_success,
    .swPbo_portState_get = dal_ca8279_success,
    .swPbo_portThreshold_set = dal_ca8279_success,
    .swPbo_portThreshold_get = dal_ca8279_success,
    .swPbo_queueThreshold_set = dal_ca8279_success,
    .swPbo_queueThreshold_get = dal_ca8279_success,
    .swPbo_portUsedPageCount_get = dal_ca8279_success,
    .swPbo_portUsedPageCount_clear = dal_ca8279_success,
    .swPbo_queueUsedPageCount_get = dal_ca8279_success,
    .swPbo_queueUsedPageCount_clear = dal_ca8279_success,
    .swPbo_portDscCount_get = dal_ca8279_success,
    .swPbo_portDscCount_clear = dal_ca8279_success,

    /*MDIO*/
    .mdio_init = dal_ca8279_success,
    .mdio_cfg_set = dal_ca8279_success,
    .mdio_cfg_get = dal_ca8279_success,
    .mdio_c22_write = dal_ca8279_success,
    .mdio_c22_read = dal_ca8279_success,
    .mdio_c45_write = dal_ca8279_success,
    .mdio_c45_read = dal_ca8279_success,
};

#endif //!defined(CONFIG_RG_G3_SERIES_DEVELOPMENT)

struct proc_dir_entry* ca_rtk_proc_dir = NULL;
struct proc_dir_entry* ca_rtk_debug_entry = NULL;

struct proc_dir_entry* ca_rtk_send_pkt_dir = NULL;
struct proc_dir_entry* ca_rtk_to_pon_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_sid_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_dmac_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_smac_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_cvlan_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_dot1p_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_etype_entry = NULL;
struct proc_dir_entry* ca_rtk_to_pon_delay_ms_entry = NULL;

struct proc_dir_entry* ca_rtk_fe_pass_dir = NULL;
struct proc_dir_entry* ca_rtk_fe_pass_entry = NULL;
struct proc_dir_entry* ca_rtk_fe_pass_sid_entry = NULL;
struct proc_dir_entry* ca_rtk_fe_pass_cvlan_entry = NULL;
struct proc_dir_entry* ca_rtk_fe_pass_port_entry = NULL;
struct proc_dir_entry* ca_rtk_fe_pass_us_add_entry = NULL;
struct proc_dir_entry* ca_rtk_fe_pass_ds_rmv_entry = NULL;


static struct task_struct *pSendToPonThread[5]={NULL,NULL,NULL,NULL,NULL};

static int ca_rtk_debug_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "0x00000001 : RT_PARAM_CHK check error message\n");
    seq_printf(seq, "0x00000002 : RT_PARAM_CHK check error with dump stack\n");
    seq_printf(seq, "0x00000004 : Dump dal success message\n");
    seq_printf(seq, "0x00000008 : RTK_API_LOCK message\n");
    seq_printf(seq, "0x00000010 : RTK_API_LOCK with dump stack\n");
    seq_printf(seq, "ca rtk debug status: 0x%x\n", ca_rtk_debug);

    return 0;
}

static int ca_rtk_debug_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        ca_rtk_debug = simple_strtoul(tmpBuf, NULL, 16);
        printk("write ca rtk debug to 0x%x\n", ca_rtk_debug);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_debug_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_debug_read, inode->i_private);
}

static const struct file_operations ca_rtk_debug_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_debug_open_proc,
    .write          = ca_rtk_debug_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

unsigned short ponDataLen = 64;
unsigned char ponData[64] = {
    0x00,0x00,0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x00,0x11,0x88,0xa8,0x00,0x01,
    0x81,0x00,0x01,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned int PON_DELAY_MS = 200;
unsigned int PON_SID = 7;
unsigned char PON_DMAC[6] = {0x00,0x00,0x00,0x00,0x00,0x22};
unsigned char PON_SMAC[6] = {0x00,0x00,0x00,0x00,0x00,0x11};
unsigned int PON_CVLAN = 320;
unsigned int PON_DOT1P = 5;
unsigned int PON_ETYPE = 0xffff;

static int send_to_pon_thread(void *data)
{
    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    unsigned char tmp[60];
    int ret,i;

    memcpy(ponData,PON_DMAC,6);
    memcpy(ponData+6,PON_SMAC,6);
    ponData[14] = PON_SID>>8;
    ponData[15] = PON_SID&0xff;
    ponData[14] |= PON_DOT1P<<5;

    ponData[18] = PON_CVLAN>>8;
    ponData[19] = PON_CVLAN&0xff;
    ponData[18] |= PON_DOT1P<<5;

    ponData[20] = PON_ETYPE>>8;
    ponData[21] = PON_ETYPE&0xff;

    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    halPkt.cos            = 7;
    halPkt.device_id      = 0;
    halPkt.src_port_id    = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    halPkt.dst_port_id    = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    halPkt.pkt_type       = 0;
    halPkt.pkt_len        = ponDataLen;
    halPkt.pkt_data       = (ca_pkt_block_t *)&pkt_data;
    halPkt.pkt_data->len  = ponDataLen;
    halPkt.pkt_data->data = ponData;
    halPkt.flags          = CA_TX_BYPASS_FWD_ENGINE;
    halPkt.dst_sub_port_id = 0;

    if(ponDataLen < 60)
    {
        memset(tmp,0,60);
        memcpy(tmp,ponData,ponDataLen);
        halPkt.pkt_len        = 60;
        halPkt.pkt_data->len  = 60;
        halPkt.pkt_data->data = tmp;
    }

    printk("sid=%d\n",PON_SID);
    printk("dmac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_DMAC[0],PON_DMAC[1],PON_DMAC[2],PON_DMAC[3],PON_DMAC[4],PON_DMAC[5]);
    printk("smac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_SMAC[0],PON_SMAC[1],PON_SMAC[2],PON_SMAC[3],PON_SMAC[4],PON_SMAC[5]);
    printk("cvlan=%d\n",PON_CVLAN);
    printk("dot1p=%d\n",PON_DOT1P);
    printk("etpype=0x%04x\n",PON_ETYPE);

    printk("\nponData=\n");
    for(i=0;i<ponDataLen;i++)
    {
        printk("%02x ",ponData[i]);
        if((i+1)%16 == 0)
            printk("\n");
    }
    printk("\n");

    printk("per packet delay ms=%d\n",PON_DELAY_MS);

    while(!kthread_should_stop())
    {
        if ((ret = __ca_tx(0, &halPkt)) != CA_E_OK)
        {
            printk("%s %d : TX to PON Error!!\n",__FUNCTION__,__LINE__);
        }

        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(msecs_to_jiffies(PON_DELAY_MS));
    }
    return 0;
}

static int ca_rtk_to_pon_read(struct seq_file *seq, void *v)
{
    int i;
    seq_printf(seq, "sid=%d\n",PON_SID);
    seq_printf(seq, "dmac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_DMAC[0],PON_DMAC[1],PON_DMAC[2],PON_DMAC[3],PON_DMAC[4],PON_DMAC[5]);
    seq_printf(seq, "smac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_SMAC[0],PON_SMAC[1],PON_SMAC[2],PON_SMAC[3],PON_SMAC[4],PON_SMAC[5]);
    seq_printf(seq, "cvlan=%d\n",PON_CVLAN);
    seq_printf(seq, "dot1p=%d\n",PON_DOT1P);
    seq_printf(seq, "etpype=0x%04x\n",PON_ETYPE);

    seq_printf(seq, "\nponData=\n");
    for(i=0;i<ponDataLen;i++)
    {
        seq_printf(seq, "%02x ",ponData[i]);
        if((i+1)%16 == 0)
            seq_printf(seq, "\n");
    }
    seq_printf(seq, "\n");

    seq_printf(seq, "per packet delay ms=%d\n",PON_DELAY_MS);

    return 0;
}

static int ca_rtk_to_pon_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int i;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16) == 1)
        {
            for(i=0;i<5;i++)
            {
                if(pSendToPonThread[i]==NULL)
                {
                    pSendToPonThread[i]=kthread_create(send_to_pon_thread, NULL, "send_to_pon_thread");
                    wake_up_process(pSendToPonThread[i]);
                    return count;
                }
            }
            printk("can't create thread reach to max number\n");
        }
        else
        {
            for(i=0;i<5;i++)
            {
                if(pSendToPonThread[i]!=NULL)
                {
                    kthread_stop(pSendToPonThread[i]);
                    pSendToPonThread[i]=NULL;
                }
            }
        }
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_open_proc,
    .write          = ca_rtk_to_pon_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_sid_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "sid=%d\n",PON_SID);

    return 0;
}

static int ca_rtk_to_pon_sid_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PON_SID = simple_strtoul(tmpBuf, NULL, 10);
        printk("sid=%d\n",PON_SID);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_sid_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_sid_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_sid_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_sid_open_proc,
    .write          = ca_rtk_to_pon_sid_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_dmac_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "dmac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_DMAC[0],PON_DMAC[1],PON_DMAC[2],PON_DMAC[3],PON_DMAC[4],PON_DMAC[5]);

    return 0;
}

static int ca_rtk_to_pon_dmac_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[30] = {0};
    int len = (count > 29) ? 29 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        sscanf(tmpBuf,"%02x:%02x:%02x:%02x:%02x:%02x",&PON_DMAC[0],&PON_DMAC[1],&PON_DMAC[2],&PON_DMAC[3],&PON_DMAC[4],&PON_DMAC[5]);
        printk("dmac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_DMAC[0],PON_DMAC[1],PON_DMAC[2],PON_DMAC[3],PON_DMAC[4],PON_DMAC[5]);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_dmac_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_dmac_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_dmac_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_dmac_open_proc,
    .write          = ca_rtk_to_pon_dmac_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_smac_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "smac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_SMAC[0],PON_SMAC[1],PON_SMAC[2],PON_SMAC[3],PON_SMAC[4],PON_SMAC[5]);

    return 0;
}

static int ca_rtk_to_pon_smac_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[30] = {0};
    int len = (count > 29) ? 29 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        sscanf(tmpBuf,"%02x:%02x:%02x:%02x:%02x:%02x",&PON_SMAC[0],&PON_SMAC[1],&PON_SMAC[2],&PON_SMAC[3],&PON_SMAC[4],&PON_SMAC[5]);
        printk("smac=%02x:%02x:%02x:%02x:%02x:%02x\n",PON_SMAC[0],PON_SMAC[1],PON_SMAC[2],PON_SMAC[3],PON_SMAC[4],PON_SMAC[5]);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_smac_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_smac_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_smac_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_smac_open_proc,
    .write          = ca_rtk_to_pon_smac_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_cvlan_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "cvlan=%d\n",PON_CVLAN);

    return 0;
}

static int ca_rtk_to_pon_cvlan_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PON_CVLAN = simple_strtoul(tmpBuf, NULL, 10);
        printk("cvlan=%d\n",PON_CVLAN);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_cvlan_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_cvlan_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_cvlan_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_cvlan_open_proc,
    .write          = ca_rtk_to_pon_cvlan_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_dot1p_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "dot1p=%d\n",PON_DOT1P);

    return 0;
}

static int ca_rtk_to_pon_dot1p_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PON_DOT1P = simple_strtoul(tmpBuf, NULL, 10);
        printk("dot1p=%d\n",PON_DOT1P);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_dot1p_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_dot1p_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_dot1p_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_dot1p_open_proc,
    .write          = ca_rtk_to_pon_dot1p_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_etype_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "etype=0x%04x\n",PON_ETYPE);

    return 0;
}

static int ca_rtk_to_pon_etype_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PON_ETYPE = simple_strtoul(tmpBuf, NULL, 16);
        printk("etype=0x%04x\n",PON_ETYPE);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_etype_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_etype_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_etype_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_etype_open_proc,
    .write          = ca_rtk_to_pon_etype_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_to_pon_delay_ms_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "delay_ms=%d\n",PON_DELAY_MS);

    return 0;
}

static int ca_rtk_to_pon_delay_ms_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PON_DELAY_MS = simple_strtoul(tmpBuf, NULL, 10);
        printk("delay_ms=%d\n",PON_DELAY_MS);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_to_pon_delay_ms_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_to_pon_delay_ms_read, inode->i_private);
}

static const struct file_operations ca_rtk_to_pon_delay_ms_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_to_pon_delay_ms_open_proc,
    .write          = ca_rtk_to_pon_delay_ms_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

ca_uint32_t usFePassIndex = -1;
ca_uint32_t dsFePassIndex = -1;

unsigned int PASS_SID = 7;
unsigned int PASS_CVLAN = 320;
unsigned int PASS_PORT = 2;
unsigned int PASS_US_ADD = 0;
unsigned int PASS_DS_RMV = 0;

static int ca_rtk_fe_pass_read(struct seq_file *seq, void *v)
{
    printk("PASS_SID=%d\n",PASS_SID);
    printk("PASS_CVLAN=%d\n",PASS_CVLAN);
    printk("PASS_PORT=%d\n",PASS_PORT);
    printk("PASS_US_ADD=%d\n",PASS_US_ADD);
    printk("PASS_DS_RMV=%d\n",PASS_DS_RMV);

    seq_printf(seq, "usFePassIndex=%d\n",usFePassIndex);
    seq_printf(seq, "dsFePassIndex=%d\n",dsFePassIndex);

    return 0;
}

static int ca_rtk_fe_pass_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16) == 1)
        {
            printk("PASS_SID=%d\n",PASS_SID);
            printk("PASS_CVLAN=%d\n",PASS_CVLAN);
            printk("PASS_PORT=%d\n",PASS_PORT);
            printk("PASS_US_ADD=%d\n",PASS_US_ADD);
            printk("PASS_DS_RMV=%d\n",PASS_DS_RMV);

            memset(&key,0,sizeof(ca_classifier_key_t));
            memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
            memset(&action,0,sizeof(ca_classifier_action_t));

            key.src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,PASS_PORT);
            key_mask.src_port = 1;
            if(PASS_US_ADD == 0)
            {
                key.l2.vlan_count = 1;
                key_mask.l2 = 1;
                key.l2.vlan_otag.vlan_max.vid = PASS_CVLAN;
                key.l2.vlan_otag.vlan_min.vid = PASS_CVLAN;
                key_mask.l2_mask.vlan_otag = 1;
            }

            action.forward = CA_CLASSIFIER_FORWARD_PORT;
            action.dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON,7);
            
            if(PASS_US_ADD == 1)
            {
                action.options.inner_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
                action.options.inner_vid = PASS_CVLAN;
                action.options.masks.inner_vlan_act = 1;
                action.options.masks.inner_tpid = 1;
            }

            action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
            action.options.outer_vid = PASS_SID;
            action.options.masks.outer_vlan_act = 1;
            action.options.masks.outer_tpid = 1;

            ca_classifier_rule_add(0,7,&key,&key_mask,&action,&usFePassIndex);

            memset(&key,0,sizeof(ca_classifier_key_t));
            memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
            memset(&action,0,sizeof(ca_classifier_action_t));

            key.src_port = CA_PORT_ID(CA_PORT_TYPE_EPON,7);
            key_mask.src_port = 1;
            key.key_handle.flow_id = CA_UINT16_INVALID;
            key.key_handle.gem_index = CA_UINT16_INVALID;
            key.key_handle.llid_cos_index = CA_UINT16_INVALID;
            key_mask.key_handle = TRUE;
            key.l2.vlan_count = 1;
            key_mask.l2 = 1;
            key.l2.vlan_otag.vlan_max.vid = PASS_CVLAN;
            key.l2.vlan_otag.vlan_min.vid = PASS_CVLAN;
            key_mask.l2_mask.vlan_otag = 1;
            key_mask.l2_mask.vlan_otag_mask.vid =1;

            action.forward = CA_CLASSIFIER_FORWARD_PORT;
            action.dest.port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,PASS_PORT);

            if(PASS_DS_RMV == 1)
            {
                action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_POP;
                action.options.masks.outer_vlan_act = 1;
            }

            ca_classifier_rule_add(0,7,&key,&key_mask,&action,&dsFePassIndex);
        }
        else
        {
            ca_classifier_rule_delete(0,usFePassIndex);
            usFePassIndex = -1;
            ca_classifier_rule_delete(0,dsFePassIndex);
            dsFePassIndex = -1;
        }
        
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_fe_pass_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_fe_pass_read, inode->i_private);
}

static const struct file_operations ca_rtk_fe_pass_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_fe_pass_open_proc,
    .write          = ca_rtk_fe_pass_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_fe_pass_sid_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "sid=%d\n",PASS_SID);

    return 0;
}

static int ca_rtk_fe_pass_sid_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PASS_SID = simple_strtoul(tmpBuf, NULL, 10);
        printk("sid=%d\n",PASS_SID);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_fe_pass_sid_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_fe_pass_sid_read, inode->i_private);
}

static const struct file_operations ca_rtk_fe_pass_sid_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_fe_pass_sid_open_proc,
    .write          = ca_rtk_fe_pass_sid_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_fe_pass_cvlan_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "cvlan=%d\n",PASS_CVLAN);

    return 0;
}

static int ca_rtk_fe_pass_cvlan_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PASS_CVLAN = simple_strtoul(tmpBuf, NULL, 10);
        printk("cvlan=%d\n",PASS_CVLAN);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_fe_pass_cvlan_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_fe_pass_cvlan_read, inode->i_private);
}

static const struct file_operations ca_rtk_fe_pass_cvlan_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_fe_pass_cvlan_open_proc,
    .write          = ca_rtk_fe_pass_cvlan_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_fe_pass_port_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "port=%d\n",PASS_PORT);

    return 0;
}

static int ca_rtk_fe_pass_port_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        PASS_PORT = simple_strtoul(tmpBuf, NULL, 10);
        printk("port=%d\n",PASS_PORT);
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_fe_pass_port_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_fe_pass_port_read, inode->i_private);
}

static const struct file_operations ca_rtk_fe_pass_port_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_fe_pass_port_open_proc,
    .write          = ca_rtk_fe_pass_port_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_fe_pass_us_add_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "us add=%d\n",PASS_US_ADD);

    return 0;
}

static int ca_rtk_fe_pass_us_add_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 10)==1)
        {
            PASS_US_ADD = 1;
        }
        else
        {
            PASS_US_ADD = 0;
        }
        
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_fe_pass_us_add_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_fe_pass_us_add_read, inode->i_private);
}

static const struct file_operations ca_rtk_fe_pass_us_add_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_fe_pass_us_add_open_proc,
    .write          = ca_rtk_fe_pass_us_add_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int ca_rtk_fe_pass_ds_rmv_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "ds rmv=%d\n",PASS_DS_RMV);

    return 0;
}

static int ca_rtk_fe_pass_ds_rmv_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 10)==1)
        {
            PASS_DS_RMV = 1;
        }
        else
        {
            PASS_DS_RMV = 0;
        }
        
        return count;
    }
    return -EFAULT;
}

static int ca_rtk_fe_pass_ds_rmv_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_fe_pass_ds_rmv_read, inode->i_private);
}

static const struct file_operations ca_rtk_fe_pass_ds_rmv_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_fe_pass_ds_rmv_open_proc,
    .write          = ca_rtk_fe_pass_ds_rmv_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static int __init init_ca_rtk(void)
{
    dal_ca8279_mapper_register(&dal_ca8279_mapper);

    if (NULL == ca_rtk_proc_dir) {
        ca_rtk_proc_dir = proc_mkdir ("ca_rtk", NULL);
    }

    if(ca_rtk_proc_dir)
    {
        ca_rtk_debug_entry = proc_create("debug", 0644, ca_rtk_proc_dir, &ca_rtk_debug_fop);
        if (!ca_rtk_debug_entry) {
            printk("ca_rtk_debug_entry, create proc failed!");
        }

        if (NULL == ca_rtk_send_pkt_dir) {
            ca_rtk_send_pkt_dir = proc_mkdir ("send_pkt", ca_rtk_proc_dir);
        }

        if(ca_rtk_send_pkt_dir)
        {
            ca_rtk_to_pon_entry = proc_create("to_pon", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_fop);
            if (!ca_rtk_to_pon_entry) {
                printk("ca_rtk_to_pon_entry, create proc failed!");
            }

            ca_rtk_to_pon_sid_entry = proc_create("pon_sid", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_sid_fop);
            if (!ca_rtk_to_pon_sid_entry) {
                printk("ca_rtk_to_pon_sid_entry, create proc failed!");
            }

            ca_rtk_to_pon_dmac_entry = proc_create("pon_dmac", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_dmac_fop);
            if (!ca_rtk_to_pon_dmac_entry) {
                printk("ca_rtk_to_pon_dmac_entry, create proc failed!");
            }

            ca_rtk_to_pon_smac_entry = proc_create("pon_smac", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_smac_fop);
            if (!ca_rtk_to_pon_smac_entry) {
                printk("ca_rtk_to_pon_smac_entry, create proc failed!");
            }

            ca_rtk_to_pon_cvlan_entry = proc_create("pon_cvlan", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_cvlan_fop);
            if (!ca_rtk_to_pon_cvlan_entry) {
                printk("ca_rtk_to_pon_cvlan_entry, create proc failed!");
            }

            ca_rtk_to_pon_dot1p_entry = proc_create("pon_dot1p", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_dot1p_fop);
            if (!ca_rtk_to_pon_dot1p_entry) {
                printk("ca_rtk_to_pon_dot1p_entry, create proc failed!");
            }

            ca_rtk_to_pon_etype_entry = proc_create("pon_etype", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_etype_fop);
            if (!ca_rtk_to_pon_etype_entry) {
                printk("ca_rtk_to_pon_etype_entry, create proc failed!");
            }

            ca_rtk_to_pon_delay_ms_entry = proc_create("pon_delay_ms", 0644, ca_rtk_send_pkt_dir, &ca_rtk_to_pon_delay_ms_fop);
            if (!ca_rtk_to_pon_delay_ms_entry) {
                printk("ca_rtk_to_pon_delay_ms_entry, create proc failed!");
            }
        }

        if (NULL == ca_rtk_fe_pass_dir) {
            ca_rtk_fe_pass_dir = proc_mkdir ("fe_pass", ca_rtk_proc_dir);
        }

        if(ca_rtk_fe_pass_dir)
        {
            ca_rtk_fe_pass_entry = proc_create("pass", 0644, ca_rtk_fe_pass_dir, &ca_rtk_fe_pass_fop);
            if (!ca_rtk_fe_pass_entry) {
                printk("ca_rtk_fe_pass_entry, create proc failed!");
            }

            ca_rtk_fe_pass_sid_entry = proc_create("sid", 0644, ca_rtk_fe_pass_dir, &ca_rtk_fe_pass_sid_fop);
            if (!ca_rtk_fe_pass_sid_entry) {
                printk("ca_rtk_fe_pass_sid_entry, create proc failed!");
            }

            ca_rtk_fe_pass_cvlan_entry = proc_create("cvlan", 0644, ca_rtk_fe_pass_dir, &ca_rtk_fe_pass_cvlan_fop);
            if (!ca_rtk_fe_pass_cvlan_entry) {
                printk("ca_rtk_fe_pass_cvlan_entry, create proc failed!");
            }

            ca_rtk_fe_pass_port_entry = proc_create("port", 0644, ca_rtk_fe_pass_dir, &ca_rtk_fe_pass_port_fop);
            if (!ca_rtk_fe_pass_port_entry) {
                printk("ca_rtk_fe_pass_port_entry, create proc failed!");
            }

            ca_rtk_fe_pass_us_add_entry = proc_create("us_add", 0644, ca_rtk_fe_pass_dir, &ca_rtk_fe_pass_us_add_fop);
            if (!ca_rtk_fe_pass_us_add_entry) {
                printk("ca_rtk_fe_pass_us_add_entry, create proc failed!");
            }

            ca_rtk_fe_pass_ds_rmv_entry = proc_create("ds_rmv", 0644, ca_rtk_fe_pass_dir, &ca_rtk_fe_pass_ds_rmv_fop);
            if (!ca_rtk_fe_pass_ds_rmv_entry) {
                printk("ca_rtk_fe_pass_ds_rmv_entry, create proc failed!");
            }
        }

    }

    return 0;
}

static void __exit exit_ca_rtk(void)
{
    if(ca_rtk_to_pon_entry)
    {
        remove_proc_entry("to_pon",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_entry = NULL;
    }

    if(ca_rtk_to_pon_sid_entry)
    {
        remove_proc_entry("pon_sid",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_sid_entry = NULL;
    }

    if(ca_rtk_to_pon_dmac_entry)
    {
        remove_proc_entry("pon_dmac",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_dmac_entry = NULL;
    }

    if(ca_rtk_to_pon_smac_entry)
    {
        remove_proc_entry("pon_smac",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_smac_entry = NULL;
    }

    if(ca_rtk_to_pon_cvlan_entry)
    {
        remove_proc_entry("pon_cvlan",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_cvlan_entry = NULL;
    }

    if(ca_rtk_to_pon_dot1p_entry)
    {
        remove_proc_entry("pon_dot1p",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_dot1p_entry = NULL;
    }

    if(ca_rtk_to_pon_etype_entry)
    {
        remove_proc_entry("pon_etype",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_etype_entry = NULL;
    }

    if(ca_rtk_to_pon_delay_ms_entry)
    {
        remove_proc_entry("pon_delay_ms",ca_rtk_send_pkt_dir);
        ca_rtk_to_pon_delay_ms_entry = NULL;
    }

    if(ca_rtk_fe_pass_entry)
    {
        remove_proc_entry("pass",ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_entry = NULL;
    }

    if(ca_rtk_fe_pass_sid_entry)
    {
        remove_proc_entry("sid",ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_sid_entry = NULL;
    }

    if(ca_rtk_fe_pass_cvlan_entry)
    {
        remove_proc_entry("cvlan",ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_cvlan_entry = NULL;
    }
    
    if(ca_rtk_fe_pass_port_entry)
    {
        remove_proc_entry("port",ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_port_entry = NULL;
    }

    if(ca_rtk_fe_pass_us_add_entry)
    {
        remove_proc_entry("us_add",ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_us_add_entry = NULL;
    }

     if(ca_rtk_fe_pass_ds_rmv_entry)
    {
        remove_proc_entry("ds_rmv",ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_ds_rmv_entry = NULL;
    }

    if(ca_rtk_send_pkt_dir)
    {
        proc_remove(ca_rtk_send_pkt_dir);
        ca_rtk_send_pkt_dir = NULL;
    }

    if(ca_rtk_fe_pass_dir)
    {
        proc_remove(ca_rtk_fe_pass_dir);
        ca_rtk_fe_pass_dir = NULL;
    }

    if(ca_rtk_debug_entry)
    {
        remove_proc_entry("debug", ca_rtk_proc_dir);
        ca_rtk_debug_entry = NULL;
    }

    if(ca_rtk_proc_dir)
    {
        proc_remove(ca_rtk_proc_dir);
        ca_rtk_proc_dir = NULL;
    }
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek CA8279 DAL module");
MODULE_AUTHOR("Realtek");
module_init(init_ca_rtk);
module_exit(exit_ca_rtk); 

module_param(synfin_prio,uint,0644);
module_param(icmp_flood_prio,uint,0644);
module_param(syn_flood_prio,uint,0644);
module_param(fin_flood_prio,uint,0644);
