#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/version.h>


#include <rtk/init.h>
#include <rtk/rt/rt_init.h>

#include <dal/dal_mapper.h>
#include <dal/dal_common.h>
#include <dal/rtl9607f/dal_rtl9607f_mapper.h>
#include <dal/rtl9607f/dal_rtl9607f_switch.h>
#include <dal/rtl9607f/dal_rtl9607f_l2.h>
#include <dal/rtl9607f/dal_rtl9607f_vlan.h>
#include <dal/rtl9607f/dal_rtl9607f_port.h>
#include <dal/rtl9607f/dal_rtl9607f_qos.h>
#include <dal/rtl9607f/dal_rtl9607f_stat.h>
#include <dal/rtl9607f/dal_rtl9607f_rate.h>
#include <dal/rtl9607f/dal_rtl9607f_sec.h>
#include <dal/rtl9607f/dal_rtl9607f_intr.h>
#include <dal/rtl9607f/dal_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rtl9607f_trap.h>
#include <dal/rtl9607f/dal_rtl9607f_i2c.h>
#include <dal/rtl9607f/dal_rtl9607f_led.h>
#include <dal/rtl9607f/dal_rtl9607f_ponmac.h>
#include <dal/rtl9607f/dal_rtl9607f_gpon.h>
#include <dal/rtl9607f/dal_rtl9607f_mirror.h>
#include <dal/rtl9607f/dal_rtl9607f_gpio.h>
#include <dal/rtl9607f/dal_rtl9607f_mdio.h>
#include <dal/rtl9607f/dal_rtl9607f_time.h>

#include "ni-drv-07f/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"
#include "cortina-api/include/pkt_tx.h"
#include "cortina-api/include/port.h"
#include "cortina-api/include/special_packet.h"
#include "cortina-api/include/epon.h"
#include "cortina-api/include/classifier.h"
#include "cortina-api/include/vlan.h"
#include "cortina-api/include/xgpon.h"
#include "cortina-api/include/gpon.h"

#if defined(CONFIG_COMMON_RT_API)
#include <dal/rtl9607f/dal_rt_rtl9607f_gpon.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_ponmisc.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_l2.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_cls.h>
#endif

#include "rtk/rt/rt_ponmisc.h"

#include <soc/cortina/registers.h>

#include <aal_common.h>
#include "aal_pon.h"
#include "aal_gpon.h"

int32
dal_rtl9607f_success(void)
{
    if(ca_rtk_debug & 0x4)
    {
       printk(KERN_INFO "%s: byPass RTK API, just return RT_ERR_OK!\n", __func__);
    }

    return RT_ERR_OK;
}

static dal_mapper_t dal_rtl9607f_mapper =
{
    0,//Kem TBD
    ._init = NULL,

    /*sec*/
    .sec_init = dal_rtl9607f_sec_init,
    .sec_portAttackPreventState_get = dal_rtl9607f_sec_portAttackPreventState_get,
    .sec_portAttackPreventState_set = dal_rtl9607f_sec_portAttackPreventState_set,
    .sec_attackPrevent_get = dal_rtl9607f_sec_attackPrevent_get,
    .sec_attackPrevent_set = dal_rtl9607f_sec_attackPrevent_set,
    .sec_attackFloodThresh_get = dal_rtl9607f_sec_attackFloodThresh_get,
    .sec_attackFloodThresh_set = dal_rtl9607f_sec_attackFloodThresh_set,
    .sec_attackFloodThreshUnit_get = dal_rtl9607f_sec_attackFloodThreshUnit_get,
    .sec_attackFloodThreshUnit_set = dal_rtl9607f_sec_attackFloodThreshUnit_set,

    /* statistics */
    .stat_init                                  = dal_rtl9607f_stat_init,
    .stat_global_reset                          = NULL,
    .stat_port_reset                            = dal_rtl9607f_stat_port_reset,
    .stat_log_reset                             = NULL,
    .stat_hostCnt_reset                         = NULL,
    .stat_hostCnt_get                           = NULL,
    .stat_hostState_get                         = NULL,
    .stat_hostState_set                         = NULL,
    .stat_rst_cnt_value_set                     = NULL,
    .stat_rst_cnt_value_get                     = NULL,
    .stat_global_get                            = NULL,
    .stat_global_getAll                         = NULL,
    .stat_port_get                              = dal_rtl9607f_stat_port_get,
    .stat_port_getAll                           = dal_rtl9607f_stat_port_getAll,
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
    .switch_init = dal_rtl9607f_switch_init,
    .switch_phyPortId_get = dal_rtl9607f_switch_phyPortId_get,
    .switch_logicalPort_get = NULL,
    .switch_port2PortMask_set = NULL,
    .switch_port2PortMask_clear = NULL,
    .switch_portIdInMask_check = NULL,
    .switch_maxPktLenLinkSpeed_get = NULL,
    .switch_maxPktLenLinkSpeed_set = NULL,
    .switch_mgmtMacAddr_get = NULL,
    .switch_mgmtMacAddr_set = NULL,
    .switch_chip_reset = dal_rtl9607f_switch_chip_reset,
    .switch_version_get = dal_rtl9607f_switch_version_get,
    .switch_patch_info_get = NULL,
    .switch_csExtId_get = NULL,
    .switch_maxPktLenByPort_get = dal_rtl9607f_switch_maxPktLenByPort_get,
    .switch_maxPktLenByPort_set = dal_rtl9607f_switch_maxPktLenByPort_set,
    .switch_changeDuplex_get = NULL,
    .switch_changeDuplex_set = NULL,
    .switch_system_init = NULL,
    .switch_thermal_get = dal_rtl9607f_switch_thermal_get,

    /* Port */
    .port_init                                  = dal_rtl9607f_port_init,
    .port_link_get                              = dal_rtl9607f_port_link_get,
    .port_speedDuplex_get                       = dal_rtl9607f_port_speedDuplex_get,
    .port_flowctrl_get                          = dal_rtl9607f_port_flowctrl_get,
    .port_phyAutoNegoEnable_get                 = dal_rtl9607f_port_phyAutoNegoEnable_get,
    .port_phyAutoNegoEnable_set                 = dal_rtl9607f_port_phyAutoNegoEnable_set,
    .port_phyAutoNegoAbility_get                = dal_rtl9607f_port_phyAutoNegoAbility_get,
    .port_phyAutoNegoAbility_set                = dal_rtl9607f_port_phyAutoNegoAbility_set,
    .port_phyForceModeAbility_get               = dal_rtl9607f_port_phyForceModeAbility_get,
    .port_phyForceModeAbility_set               = dal_rtl9607f_port_phyForceModeAbility_set,
    .port_phyMasterSlave_get                    = NULL,
    .port_phyMasterSlave_set                    = NULL,
    .port_phyTestMode_get                       = dal_rtl9607f_port_phyTestMode_get,
    .port_phyTestMode_set                       = dal_rtl9607f_port_phyTestMode_set,
    .port_phyReg_get                            = dal_rtl9607f_port_phyReg_get,
    .port_phyReg_set                            = dal_rtl9607f_port_phyReg_set,
    .port_cpuPortId_get                         = NULL,
    .port_isolation_get                         = NULL,
    .port_isolation_set                         = NULL,
    .port_isolationExt_get                      = NULL,
    .port_isolationExt_set                      = NULL,
    .port_isolationL34_get                      = NULL,
    .port_isolationL34_set                      = NULL,
    .port_isolationExtL34_get                   = NULL,
    .port_isolationExtL34_set                   = NULL,
    .port_isolationEntry_get                    = dal_rtl9607f_port_isolationEntry_get,
    .port_isolationEntry_set                    = dal_rtl9607f_port_isolationEntry_set,
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
    .port_adminEnable_get                       = dal_rtl9607f_port_adminEnable_get,
    .port_adminEnable_set                       = dal_rtl9607f_port_adminEnable_set,
    .port_specialCongest_get                    = NULL,
    .port_specialCongest_set                    = NULL,
    .port_specialCongestStatus_get              = NULL,
    .port_specialCongestStatus_clear            = NULL,
    .port_greenEnable_get                       = dal_rtl9607f_port_greenEnable_get,
    .port_greenEnable_set                       = dal_rtl9607f_port_greenEnable_set,
    .port_phyCrossOverMode_get                  = NULL,
    .port_phyCrossOverMode_set                  = NULL,
    .port_phyPowerDown_set                      = dal_rtl9607f_port_phyPowerDown_set,
    .port_phyPowerDown_get                      = dal_rtl9607f_port_phyPowerDown_get,
    .port_enhancedFid_get                       = NULL,
    .port_enhancedFid_set                       = NULL,
    .port_rtctResult_get                        = dal_rtl9607f_port_rtctResult_get,
    .port_rtct_start                            = dal_rtl9607f_port_rtct_start,
    .port_macForceAbility_set                   = dal_rtl9607f_port_macForceAbility_set,
    .port_macForceAbility_get                   = dal_rtl9607f_port_macForceAbility_get,
    .port_macForceAbilityState_set              = NULL,
    .port_macForceAbilityState_get              = dal_rtl9607f_port_macForceAbilityState_get,
    .port_macExtMode_set                        = NULL,
    .port_macExtMode_get                        = NULL,
    .port_macExtRgmiiDelay_set                  = NULL,
    .port_macExtRgmiiDelay_get                  = NULL,
    .port_gigaLiteEnable_set                    = NULL,
    .port_gigaLiteEnable_get                    = NULL,
    .port_serdesMode_set                        = dal_rtl9607f_port_serdesMode_set,
    .port_serdesMode_get                        = dal_rtl9607f_port_serdesMode_get,
    .port_serdesNWay_set                        = dal_rtl9607f_port_serdesNWay_set,
    .port_serdesNWay_get                        = dal_rtl9607f_port_serdesNWay_get,
    .port_serdesModeSpeed_set                   = dal_rtl9607f_port_serdesModeSpeed_set,
    .port_serdesModeSpeed_get                   = dal_rtl9607f_port_serdesModeSpeed_get,
    .port_serdesTxRxPolarity_set                = dal_rtl9607f_port_serdesTxRxPolarity_set,
    .port_serdesTxRxPolarity_get                = dal_rtl9607f_port_serdesTxRxPolarity_get,
    .port_serdesStatus_get                      = dal_rtl9607f_port_serdesStatus_get,
    .port_eeeEnable_get                         = dal_rtl9607f_port_eeeEnable_get,
    .port_eeeEnable_set                         = dal_rtl9607f_port_eeeEnable_set,

    /*oam*/
    .oam_init = NULL,
    .oam_parserAction_set = NULL,
    .oam_parserAction_get = NULL,
    .oam_multiplexerAction_set = NULL,
    .oam_multiplexerAction_get = NULL,

    /* Trap */
    .trap_init                                  = dal_rtl9607f_trap_init,
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
    .trap_oamPduAction_get                      = dal_rtl9607f_trap_oamPduAction_get,
    .trap_oamPduAction_set                      = dal_rtl9607f_trap_oamPduAction_set,
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
    .trap_omciAction_get                      = dal_rtl9607f_trap_omciAction_get,
    .trap_omciAction_set                      = dal_rtl9607f_trap_omciAction_set,

    /*SVLAN*/
    .svlan_init = dal_rtl9607f_success,
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
    .vlan_init                                  = dal_rtl9607f_vlan_init,
    .vlan_create                                = dal_rtl9607f_vlan_create,
    .vlan_destroy                               = dal_rtl9607f_vlan_destroy,
    .vlan_destroyAll                            = dal_rtl9607f_vlan_destroyAll,
    .vlan_fid_get                               = NULL,
    .vlan_fid_set                               = NULL,
    .vlan_fidMode_get                           = dal_rtl9607f_vlan_fidMode_get,
    .vlan_fidMode_set                           = dal_rtl9607f_vlan_fidMode_set,
    .vlan_port_get                              = dal_rtl9607f_vlan_port_get,
    .vlan_port_set                              = dal_rtl9607f_vlan_port_set,
    .vlan_stg_get                               = NULL,
    .vlan_stg_set                               = NULL,
    .vlan_portAcceptFrameType_get               = NULL,
    .vlan_portAcceptFrameType_set               = NULL,
    .vlan_vlanFunctionEnable_get                = dal_rtl9607f_vlan_vlanFunctionEnable_get,
    .vlan_vlanFunctionEnable_set                = dal_rtl9607f_vlan_vlanFunctionEnable_set,
    .vlan_portIgrFilterEnable_get               = NULL,
    .vlan_portIgrFilterEnable_set               = NULL,
    .vlan_leaky_get                             = NULL,
    .vlan_leaky_set                             = NULL,
    .vlan_portLeaky_get                         = NULL,
    .vlan_portLeaky_set                         = NULL,
    .vlan_keepType_get                          = NULL,
    .vlan_keepType_set                          = NULL,
    .vlan_portPvid_get                          = dal_rtl9607f_vlan_portPvid_get,
    .vlan_portPvid_set                          = dal_rtl9607f_vlan_portPvid_set,
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
    .mirror_init                                = dal_rtl9607f_mirror_init,
    .mirror_portBased_set                       = dal_rtl9607f_mirror_portBased_set,
    .mirror_portBased_get                       = dal_rtl9607f_mirror_portBased_get,
    .mirror_portIso_set                         = NULL,
    .mirror_portIso_get                         = NULL,

    /* STP */
    .stp_init = dal_rtl9607f_success,
    .stp_mstpState_get = NULL,
    .stp_mstpState_set = NULL,

    /*rate*/
    .rate_init = dal_rtl9607f_rate_init,
    .rate_portIgrBandwidthCtrlRate_get = dal_rtl9607f_rate_portIgrBandwidthCtrlRate_get,
    .rate_portIgrBandwidthCtrlRate_set = dal_rtl9607f_rate_portIgrBandwidthCtrlRate_set,
    .rate_portIgrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_portIgrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_portEgrBandwidthCtrlRate_get = dal_rtl9607f_rate_portEgrBandwidthCtrlRate_get,
    .rate_portEgrBandwidthCtrlRate_set = dal_rtl9607f_rate_portEgrBandwidthCtrlRate_set,
    .rate_egrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_egrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_portEgrBandwidthCtrlIncludeIfg_get = NULL,
    .rate_portEgrBandwidthCtrlIncludeIfg_set = NULL,
    .rate_egrQueueBwCtrlEnable_get = NULL,
    .rate_egrQueueBwCtrlEnable_set = NULL,
    .rate_egrQueueBwCtrlMeterIdx_get = NULL,
    .rate_egrQueueBwCtrlMeterIdx_set = NULL,
    .rate_egrQueueBwCtrlRate_get = dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_get,
    .rate_egrQueueBwCtrlRate_set = dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_set,
    .rate_stormControlMeterIdx_get = dal_rtl9607f_rate_stormControlMeterIdx_get,
    .rate_stormControlMeterIdx_set = dal_rtl9607f_rate_stormControlMeterIdx_set,
    .rate_stormControlPortEnable_get = dal_rtl9607f_rate_stormControlPortEnable_get,
    .rate_stormControlPortEnable_set = dal_rtl9607f_rate_stormControlPortEnable_set,
    .rate_stormControlEnable_get = NULL,
    .rate_stormControlEnable_set = NULL,
    .rate_stormBypass_set = NULL,
    .rate_stormBypass_get = NULL,
    .rate_shareMeter_set = dal_rtl9607f_rate_shareMeter_set,
    .rate_shareMeter_get = dal_rtl9607f_rate_shareMeter_get,
    .rate_shareMeterBucket_set = dal_rtl9607f_rate_shareMeterBucket_set,
    .rate_shareMeterBucket_get = dal_rtl9607f_rate_shareMeterBucket_get,
    .rate_shareMeterExceed_get = NULL,
    .rate_shareMeterExceed_clear = NULL,
    .rate_shareMeterMode_set = dal_rtl9607f_rate_shareMeterMode_set,
    .rate_shareMeterMode_get = dal_rtl9607f_rate_shareMeterMode_get,
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
    .time_init = dal_rtl9607f_success,
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
    .acl_init = dal_rtl9607f_success,
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
    .l2_init                                    = dal_rtl9607f_l2_init,
    .l2_flushLinkDownPortAddrEnable_get         = dal_rtl9607f_l2_flushLinkDownPortAddrEnable_get,
    .l2_flushLinkDownPortAddrEnable_set         = dal_rtl9607f_l2_flushLinkDownPortAddrEnable_set,
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
    .l2_portLimitLearningCnt_get                = dal_rtl9607f_l2_portLimitLearningCnt_get,
    .l2_portLimitLearningCnt_set                = dal_rtl9607f_l2_portLimitLearningCnt_set,
    .l2_portLimitLearningCntAction_get          = dal_rtl9607f_l2_portLimitLearningCntAction_get,
    .l2_portLimitLearningCntAction_set          = dal_rtl9607f_l2_portLimitLearningCntAction_set,
    .l2_aging_get                               = dal_rtl9607f_l2_aging_get,
    .l2_aging_set                               = dal_rtl9607f_l2_aging_set,
    .l2_portAgingEnable_get                     = dal_rtl9607f_l2_portAgingEnable_get,
    .l2_portAgingEnable_set                     = dal_rtl9607f_l2_portAgingEnable_set,
    .l2_lookupMissAction_get                    = dal_rtl9607f_l2_lookupMissAction_get,
    .l2_lookupMissAction_set                    = dal_rtl9607f_l2_lookupMissAction_set,
    .l2_portLookupMissAction_get                = dal_rtl9607f_l2_portLookupMissAction_get,
    .l2_portLookupMissAction_set                = dal_rtl9607f_l2_portLookupMissAction_set,
    .l2_lookupMissFloodPortMask_get             = NULL,
    .l2_lookupMissFloodPortMask_set             = NULL,
    .l2_lookupMissFloodPortMask_add             = NULL,
    .l2_lookupMissFloodPortMask_del             = NULL,
    .l2_newMacOp_get                            = dal_rtl9607f_l2_newMacOp_get,
    .l2_newMacOp_set                            = dal_rtl9607f_l2_newMacOp_set,
    .l2_nextValidAddr_get                       = dal_rtl9607f_l2_nextValidAddr_get,
    .l2_nextValidAddrOnPort_get                 = dal_rtl9607f_l2_nextValidAddrOnPort_get,
    .l2_nextValidMcastAddr_get                  = NULL,
    .l2_nextValidIpMcastAddr_get                = NULL,
    .l2_nextValidEntry_get                      = dal_rtl9607f_l2_nextValidEntry_get,
    .l2_addr_add                                = dal_rtl9607f_l2_addr_add,
    .l2_addr_del                                = dal_rtl9607f_l2_addr_del,
    .l2_addr_get                                = dal_rtl9607f_l2_addr_get,
    .l2_addr_delAll                             = dal_rtl9607f_l2_addr_delAll,
    .l2_mcastAddr_add                           = NULL,
    .l2_mcastAddr_del                           = NULL,
    .l2_mcastAddr_get                           = NULL,
    .l2_illegalPortMoveAction_get               = dal_rtl9607f_l2_illegalPortMoveAction_get,
    .l2_illegalPortMoveAction_set               = dal_rtl9607f_l2_illegalPortMoveAction_set,
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
    .intr_init = dal_rtl9607f_intr_init,
    .intr_polarity_set = NULL,
    .intr_polarity_get = NULL,
    .intr_imr_set = dal_rtl9607f_intr_imr_set,
    .intr_imr_get = dal_rtl9607f_intr_imr_get,
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
    .rt_intr_isr_callback_register = dal_rt_rtl9607f_intr_isr_rx_callback_register,
#endif

    /*cpu*/
    .cpu_init = dal_rtl9607f_success,
    .cpu_trapInsertTagByPort_set = NULL,
    .cpu_trapInsertTagByPort_get = NULL,
    .cpu_tagAwareByPort_set = NULL,
    .cpu_tagAwareByPort_get = NULL,

    /* QoS */
    .qos_init = dal_rtl9607f_qos_init,
    .qos_priSelGroup_get = dal_rtl9607f_qos_priSelGroup_get,
    .qos_priSelGroup_set = dal_rtl9607f_qos_priSelGroup_set,
    .qos_portPri_get = NULL,
    .qos_portPri_set = NULL,
    .qos_dscpPriRemapGroup_get = dal_rtl9607f_qos_dscpPriRemapGroup_get,
    .qos_dscpPriRemapGroup_set = dal_rtl9607f_qos_dscpPriRemapGroup_set,
    .qos_1pPriRemapGroup_get = NULL,
    .qos_1pPriRemapGroup_set = NULL,
    .qos_priMap_get = dal_rtl9607f_qos_priMap_get,
    .qos_priMap_set = dal_rtl9607f_qos_priMap_set,
    .qos_portPriMap_get = dal_rtl9607f_qos_portPriMap_get,
    .qos_portPriMap_set = dal_rtl9607f_qos_portPriMap_set,
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
    .qos_schedulingQueue_get = dal_rtl9607f_qos_schedulingQueue_get,
    .qos_schedulingQueue_set = dal_rtl9607f_qos_schedulingQueue_set,
    .qos_portPriSelGroup_get = dal_rtl9607f_qos_portPriSelGroup_get,
    .qos_portPriSelGroup_set = dal_rtl9607f_qos_portPriSelGroup_set,
    .qos_schedulingType_get = NULL,
    .qos_schedulingType_set = NULL,
    .qos_portDot1pRemarkSrcSel_get = NULL,
    .qos_portDot1pRemarkSrcSel_set = NULL,

    /* Classification */
    .classify_init                              = dal_rtl9607f_success,
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

#if defined(CONFIG_GPON_FEATURE)
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
    .gpon_password_set = NULL,
#endif

#if defined(CONFIG_COMMON_RT_API)
    .rt_l2_ivlSvl_get                   = dal_rt_rtl9607f_l2_ivlSvl_get,
    .rt_l2_ivlSvl_set                   = dal_rt_rtl9607f_l2_ivlSvl_set,
    .rt_cls_init                        = dal_rt_rtl9607f_cls_init,
    .rt_cls_veipPortRule_set            = dal_rt_rtl9607f_cls_veipPortRule_set,
    .rt_cls_rule_add                    = dal_rt_rtl9607f_cls_rule_add,
    .rt_cls_rule_delete                 = dal_rt_rtl9607f_cls_rule_delete,
    .rt_cls_rule_get                    = dal_rt_rtl9607f_cls_rule_get,
    .rt_cls_extend_rule_add             = dal_rt_rtl9607f_cls_extend_rule_add,
    .rt_cls_extend_rule_delete          = dal_rt_rtl9607f_cls_extend_rule_delete,
    .rt_cls_extend_rule_get             = dal_rt_rtl9607f_cls_extend_rule_get,
    .rt_cls_pon_sid_set                 = dal_rt_rtl9607f_cls_pon_sid_set,
    .rt_cls_fwdPort_set                 = dal_rt_rtl9607f_cls_fwdPort_set,
    .rt_cls_vlan_paser_set              = dal_rt_rtl9607f_cls_vlan_paser_set,
    .rt_cls_vlan_paser_get              = dal_rt_rtl9607f_cls_vlan_paser_get,
    .rt_time_init                       = dal_rtl9607f_time_init,
    .rt_time_curTime_get                = dal_rtl9607f_rt_time_curTime_get,
    .rt_time_offsetTime_set             = dal_rtl9607f_rt_time_offsetTime_set,
    .rt_time_frequency_set              = dal_rtl9607f_rt_time_frequency_set,
    .rt_time_frequency_get              = dal_rtl9607f_rt_time_frequency_get,
    .rt_time_ptpPortEnable_set          = dal_rtl9607f_rt_time_ptpPortEnable_set,
    .rt_time_ptpPortEnable_get          = dal_rtl9607f_rt_time_ptpPortEnable_get,
    .rt_time_ptpIgrAction_set           = dal_rtl9607f_rt_time_ptpIgrAction_set,
    .rt_time_ptpIgrAction_get           = dal_rtl9607f_rt_time_ptpIgrAction_get,
    .rt_time_ptpEgrAction_set           = dal_rtl9607f_rt_time_ptpEgrAction_set,
    .rt_time_ptpEgrAction_get           = dal_rtl9607f_rt_time_ptpEgrAction_get,
    .rt_time_ptp_tx                     = dal_rtl9607f_rt_time_ptp_tx,
    .rt_time_ptp_rx_callback_register   = dal_rtl9607f_rt_time_ptp_rx_callback_register,
    .rt_time_ponTodTime_set             = dal_rtl9607f_rt_time_ponTodTime_set,
    .rt_time_ponTodTime_get             = dal_rtl9607f_rt_time_ponTodTime_get,
    .rt_time_ponTodTimeEnable_set       = dal_rtl9607f_rt_time_ponTodTimeEnable_set,
    .rt_time_ponTodTimeEnable_get       = dal_rtl9607f_rt_time_ponTodTimeEnable_get,
    .rt_trap_portIgmpMldPktAction_get   = dal_rtl9607f_trap_portIgmpMldPktAction_get,
    .rt_trap_portIgmpMldPktAction_set   = dal_rtl9607f_trap_portIgmpMldPktAction_set,
    .rt_trap_portDhcpPktAction_get      = dal_rtl9607f_trap_portDhcpPktAction_get,
    .rt_trap_portDhcpPktAction_set      = dal_rtl9607f_trap_portDhcpPktAction_set,
    .rt_trap_portPppoePktAction_get     = dal_rtl9607f_trap_portPppoePktAction_get,
    .rt_trap_portPppoePktAction_set     = dal_rtl9607f_trap_portPppoePktAction_set,
    .rt_trap_portStpPktAction_get       = dal_rtl9607f_trap_portStpPktAction_get,
    .rt_trap_portStpPktAction_set       = dal_rtl9607f_trap_portStpPktAction_set,
    .rt_trap_portHostPktAction_get      = dal_rtl9607f_trap_portHostPktAction_get,
    .rt_trap_portHostPktAction_set      = dal_rtl9607f_trap_portHostPktAction_set,
    .rt_trap_portEthertypePktAction_get = dal_rtl9607f_trap_portEthertypePktAction_get,
    .rt_trap_portEthertypePktAction_set = dal_rtl9607f_trap_portEthertypePktAction_set,
    .rt_trap_portArpPktAction_get       = dal_rtl9607f_trap_portArpPktAction_get,
    .rt_trap_portArpPktAction_set       = dal_rtl9607f_trap_portArpPktAction_set,
    .rt_trap_portLoopDetectPktAction_get = dal_rtl9607f_trap_portLoopDetectPktAction_get,
    .rt_trap_portLoopDetectPktAction_set = dal_rtl9607f_trap_portLoopDetectPktAction_set,
    .rt_trap_portDot1xPktAction_get     = dal_rtl9607f_trap_portDot1xPktAction_get,
    .rt_trap_portDot1xPktAction_set     = dal_rtl9607f_trap_portDot1xPktAction_set,
    .rt_trap_pattern_get                = dal_rtl9607f_trap_pattern_get,
    .rt_trap_pattern_set                = dal_rtl9607f_trap_pattern_set,
    .rt_trap_priority_get               = dal_rtl9607f_trap_priority_get,
    .rt_trap_priority_set               = dal_rtl9607f_trap_priority_set,
    .rt_qos_ingress_qos_set             = dal_rtl9607f_qos_ingress_qos_set,
    .rt_qos_ingress_qos_del             = dal_rtl9607f_qos_ingress_qos_del,
    .rt_qos_ingress_qos_get             = dal_rtl9607f_qos_ingress_qos_get,
    .rt_port_macLoopbackEnable_get      = dal_rtl9607f_port_macLoopbackEnable_get,
    .rt_port_macLoopbackEnable_set      = dal_rtl9607f_port_macLoopbackEnable_set,
    .rt_stat_vlanCnt_filter_set         = dal_rtl9607f_stat_vlanCnt_filter_set,
    .rt_stat_vlanCnt_filter_get         = dal_rtl9607f_stat_vlanCnt_filter_get,
    .rt_stat_vlanCnt_mib_get            = dal_rtl9607f_stat_vlanCnt_mib_get,
    .rt_stat_vlanCnt_mib_reset          = dal_rtl9607f_stat_vlanCnt_mib_reset,
    /*RT GPON common API*/
#if defined(CONFIG_GPON_FEATURE)
    .rt_gpon_init                       = dal_rt_rtl9607f_gpon_init,
    .rt_gpon_serialNumber_set           = dal_rt_rtl9607f_gpon_serialNumber_set,
    .rt_gpon_serialNumber_get           = dal_rt_rtl9607f_gpon_serialNumber_get,
    .rt_gpon_registrationId_set         = dal_rt_rtl9607f_gpon_registrationId_set,
    .rt_gpon_registrationId_get         = dal_rt_rtl9607f_gpon_registrationId_get,
    .rt_gpon_activate                   = dal_rt_rtl9607f_gpon_activate,
    .rt_gpon_deactivate                 = dal_rt_rtl9607f_gpon_deactivate,
    .rt_gpon_omci_tx                    = dal_rt_rtl9607f_gpon_omci_tx,
    .rt_gpon_omci_rx_callback_register  = dal_rt_rtl9607f_gpon_omci_rx_callback_register,
    .rt_gpon_onuState_get               = dal_rt_rtl9607f_gpon_onuState_get,
    .rt_gpon_tcont_set                  = dal_rt_rtl9607f_gpon_tcont_set,
    .rt_gpon_tcont_get                  = dal_rt_rtl9607f_gpon_tcont_get,
    .rt_gpon_tcont_del                  = dal_rt_rtl9607f_gpon_tcont_del,
    .rt_gpon_usFlow_set                 = dal_rt_rtl9607f_gpon_usFlow_set,
    .rt_gpon_usFlow_get                 = dal_rt_rtl9607f_gpon_usFlow_get,
    .rt_gpon_usFlow_del                 = dal_rt_rtl9607f_gpon_usFlow_del,
    .rt_gpon_usFlow_delAll              = dal_rt_rtl9607f_gpon_usFlow_delAll,
    .rt_gpon_dsFlow_set                 = dal_rt_rtl9607f_gpon_dsFlow_set,
    .rt_gpon_dsFlow_get                 = dal_rt_rtl9607f_gpon_dsFlow_get,
    .rt_gpon_dsFlow_del                 = dal_rt_rtl9607f_gpon_dsFlow_del,
    .rt_gpon_dsFlow_delAll              = dal_rt_rtl9607f_gpon_dsFlow_delAll,
    .rt_gpon_loop_gemport_set           = dal_rt_rtl9607f_loop_gemPort_set,
    .rt_gpon_loop_gemport_get           = dal_rt_rtl9607f_loop_gemPort_get,
    .rt_gpon_ponQueue_set               = dal_rt_rtl9607f_gpon_ponQueue_set,
    .rt_gpon_ponQueue_get               = dal_rt_rtl9607f_gpon_ponQueue_get,
    .rt_gpon_ponQueue_del               = dal_rt_rtl9607f_gpon_ponQueue_del,
    .rt_gpon_scheInfo_get               = dal_rt_rtl9607f_gpon_scheInfo_get,
    .rt_gpon_flowCounter_get            = dal_rt_rtl9607f_gpon_flowCounter_get,
    .rt_gpon_pmCounter_get              = dal_rt_rtl9607f_gpon_pmCounter_get,
    .rt_gpon_ponTag_get                 = dal_rt_rtl9607f_gpon_ponTag_get,
    .rt_gpon_msk_set                    = dal_rt_rtl9607f_gpon_msk_set,
    .rt_gpon_omci_mic_generate          = dal_rt_rtl9607f_gpon_omci_mic_generate,
    .rt_gpon_mcKey_set                  = dal_rt_rtl9607f_gpon_mcKey_set,
    .rt_gpon_attribute_get              = dal_rt_rtl9607f_gpon_attribute_get,
    .rt_gpon_attribute_set              = dal_rt_rtl9607f_gpon_attribute_set,
    .rt_gpon_usFlow_phyData_get         = dal_rt_rtl9607f_gpon_usFlow_phyData_get,
    .rt_gpon_omcc_get                   = dal_rt_rtl9607f_gpon_omcc_get,
    .rt_gpon_omci_mirror_set            = dal_rt_rtl9607f_gpon_omci_mirror_set,
    .rt_gpon_omci_mirror_get            = dal_rt_rtl9607f_gpon_omci_mirror_get,
    .rt_gpon_fec_get                    = dal_rt_rtl9607f_gpon_fec_get,
    .rt_gpon_ploam_rx_register          = dal_rt_rtl9607f_gpon_ploam_rx_register,
#endif
#if defined(CONFIG_EPON_FEATURE)
    .rt_epon_init                       = dal_rt_rtl9607f_epon_init,
    .rt_epon_oam_tx                     = dal_rt_rtl9607f_epon_oam_tx,
    .rt_epon_oam_rx_callback_register   = dal_rt_rtl9607f_epon_oam_rx_callback_register,
    .rt_epon_dyinggasp_set              = dal_rt_rtl9607f_epon_dyinggasp_set,
    .rt_epon_mpcp_info_get              = dal_rtl9607f_epon_mpcp_info_get,
    .rt_epon_mpcp_queue_threshold_get   = dal_rtl9607f_epon_mpcp_queue_threshold_get,
    .rt_epon_mpcp_queue_threshold_set   = dal_rtl9607f_epon_mpcp_queue_threshold_set,
    .rt_epon_llid_entry_set             = dal_rt_rtl9607f_epon_llid_entry_set,
    .rt_epon_llid_entry_get             = dal_rt_rtl9607f_epon_llid_entry_get,
    .rt_epon_registerReq_get            = dal_rt_rtl9607f_epon_registerReq_get,
    .rt_epon_registerReq_set            = dal_rt_rtl9607f_epon_registerReq_set,
    .rt_epon_churningKey_get            = dal_rt_rtl9607f_epon_churningKey_get,
    .rt_epon_churningKey_set            = dal_rt_rtl9607f_epon_churningKey_set,
    .rt_epon_usFecState_get             = dal_rt_rtl9607f_epon_usFecState_get,
    .rt_epon_usFecState_set             = dal_rt_rtl9607f_epon_usFecState_set,
    .rt_epon_dsFecState_get             = dal_rt_rtl9607f_epon_dsFecState_get,
    .rt_epon_dsFecState_set             = dal_rt_rtl9607f_epon_dsFecState_set,
    .rt_epon_mibCounter_get             = dal_rt_rtl9607f_epon_mibCounter_get,
    .rt_epon_losState_get               = dal_rt_rtl9607f_epon_losState_get,
    .rt_epon_lofState_get               = dal_rt_rtl9607f_epon_lofState_get,
    .rt_epon_mpcp_gate_timer_set        = dal_rt_rtl9607f_epon_mpcp_gate_timer_set,
    .rt_epon_mpcp_gate_timer_get        = dal_rt_rtl9607f_epon_mpcp_gate_timer_get,
    .rt_epon_info_notify_callback_register = dal_rt_rtl9607f_epon_info_notify_callback_register,
#endif
#if defined(CONFIG_COMMON_RT_PONMISC)
    .rt_ponmisc_init                    = dal_rt_rtl9607f_ponmisc_init,
    .rt_ponmisc_mode_get                = dal_rt_rtl9607f_ponmisc_mode_get,
    .rt_ponmisc_mode_set                = dal_rt_rtl9607f_ponmisc_mode_set,
    .rt_ponmisc_ponQueue_mode_get       = dal_rt_rtl9607f_ponmisc_ponQueue_mode_get,
    .rt_ponmisc_burstPolarityReverse_get = dal_rt_rtl9607f_ponmisc_burstPolarityReverse_get,
    .rt_ponmisc_burstPolarityReverse_set = dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set,
    .rt_ponmisc_forceLaserState_get     = dal_rt_rtl9607f_ponmisc_forceLaserState_get,
    .rt_ponmisc_forceLaserState_set     = dal_rt_rtl9607f_ponmisc_forceLaserState_set,
    .rt_ponmisc_forcePRBS_get           = dal_rt_rtl9607f_ponmisc_forcePRBS_get,
    .rt_ponmisc_forcePRBS_set           = dal_rt_rtl9607f_ponmisc_forcePRBS_set,
    .rt_ponmisc_forcePRBS_rx_get        = dal_rt_rtl9607f_ponmisc_forcePRBS_rx_get,
    .rt_ponmisc_forcePRBS_rx_set        = dal_rt_rtl9607f_ponmisc_forcePRBS_rx_set,
    .rt_ponmisc_prbsRxStatus_get        = dal_rt_rtl9607f_ponmisc_prbsRxStatus_get,
    .rt_ponmisc_selfRogue_cfg_get       = dal_rt_rtl9607f_ponmisc_selfRogue_cfg_get,
    .rt_ponmisc_selfRogue_cfg_set       = dal_rt_rtl9607f_ponmisc_selfRogue_cfg_set,
    .rt_ponmisc_selfRogue_count_get     = dal_rt_rtl9607f_ponmisc_selfRogue_count_get,
    .rt_ponmisc_selfRogue_count_clear   = dal_rt_rtl9607f_ponmisc_selfRogue_count_clear,
    .rt_ponmisc_rxlosPolarityReverse_get = dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_get,
    .rt_ponmisc_rxlosPolarityReverse_set = dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_set,
#endif
#endif

#if defined(CONFIG_GPON_FEATURE)||defined(CONFIG_EPON_FEATURE)||defined(CONFIG_COMMON_RT_PONMISC)
    /*PON MAC*/
    .ponmac_init = dal_rtl9607f_ponmac_init,
    .ponmac_queue_add = dal_rtl9607f_ponmac_queue_add,
    .ponmac_queue_get = dal_rtl9607f_ponmac_queue_get,
    .ponmac_queue_del = dal_rtl9607f_ponmac_queue_del,
    .ponmac_flow2Queue_set = NULL,
    .ponmac_flow2Queue_get = NULL,
    .ponmac_mode_get = NULL,
    .ponmac_mode_set = NULL,
    .ponmac_queueDrainOut_set  = NULL,
    .ponmac_opticalPolarity_get = NULL,
    .ponmac_opticalPolarity_set = NULL,

    .ponmac_losState_get = dal_rtl9607f_ponmac_losState_get,
    .ponmac_serdesCdr_reset = dal_rtl9607f_ponmac_serdesCdr_reset,
    .ponmac_linkState_get = dal_ca9607f_ponmac_linkState_get,

    .ponmac_forceBerNotify_set = NULL,
    .ponmac_bwThreshold_set = NULL,
    .ponmac_bwThreshold_get = NULL,
    .ponmac_maxPktLen_set = NULL,
    .ponmac_sidValid_get = NULL,
    .ponmac_sidValid_set = NULL,
    .ponmac_schedulingType_get = NULL,
    .ponmac_schedulingType_set = NULL,
    .ponmac_egrBandwidthCtrlRate_get = dal_rtl9607f_ponmac_egrBandwidthCtrlRate_get,
    .ponmac_egrBandwidthCtrlRate_set = dal_rtl9607f_ponmac_egrBandwidthCtrlRate_set,
    .ponmac_egrScheduleIdRate_get = NULL,
    .ponmac_egrScheduleIdRate_set = NULL,
    .ponmac_egrBandwidthCtrlIncludeIfg_get = dal_rtl9607f_ponmac_egrBandwidthCtrlIncludeIfg_get,
    .ponmac_egrBandwidthCtrlIncludeIfg_set = dal_rtl9607f_ponmac_egrBandwidthCtrlIncludeIfg_set,
    .ponmac_egrScheduleIdIncludeIfg_get = NULL,
    .ponmac_egrScheduleIdIncludeIfg_set = NULL,
    .ponmac_txDisableGpio_get = dal_rtl9607f_ponmac_txDisableGpio_get,
    .ponmac_txDisableGpio_set = dal_rtl9607f_ponmac_txDisableGpio_set,
    .ponmac_txPowerDisableGpio_get = dal_rtl9607f_ponmac_txPowerDisableGpio_get,
    .ponmac_txPowerDisableGpio_set = dal_rtl9607f_ponmac_txPowerDisableGpio_set,
#endif
    /*gpio function*/
    .gpio_init = dal_rtl9607f_gpio_init,
    .gpio_state_set = dal_rtl9607f_gpio_state_set,
    .gpio_state_get = dal_rtl9607f_gpio_state_get,
    .gpio_mode_set = dal_rtl9607f_gpio_mode_set,
    .gpio_mode_get = dal_rtl9607f_gpio_mode_get,
    .gpio_databit_get = dal_rtl9607f_gpio_databit_get,
    .gpio_databit_set = dal_rtl9607f_gpio_databit_set,
    .gpio_intr_get = NULL,
    .gpio_intr_set = NULL,
    .gpio_intrStatus_get = NULL,
    .gpio_intrStatus_clean = NULL,

    /* RLDP and RLPP */
    .rldp_init = dal_rtl9607f_success,
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
    .led_init               = dal_rtl9607f_led_init,
    .led_operation_get      = NULL,
    .led_operation_set      = NULL,
    .led_serialMode_get     = NULL,
    .led_serialMode_set     = NULL,
    .led_blinkRate_get      = dal_rtl9607f_led_blinkRate_get,
    .led_blinkRate_set      = dal_rtl9607f_led_blinkRate_set,
    .led_config_set         = dal_rtl9607f_success,
    .led_config_get         = dal_rtl9607f_success,
    .led_modeForce_get      = dal_rtl9607f_led_modeForce_get,
    .led_modeForce_set      = dal_rtl9607f_led_modeForce_set,
    .led_parallelEnable_get = dal_rtl9607f_success,
    .led_parallelEnable_set = dal_rtl9607f_success,
    .led_ponAlarm_get       = NULL,
    .led_ponAlarm_set       = NULL,
    .led_ponWarning_get     = NULL,
    .led_ponWarning_set     = NULL,


    /* I2C */
    .i2c_init = dal_rtl9607f_i2c_init,
    .i2c_enable_set = dal_rtl9607f_i2c_enable_set,
    .i2c_enable_get = dal_rtl9607f_i2c_enable_get,
    .i2c_width_set = NULL,
    .i2c_width_get = NULL,
    .i2c_write = dal_rtl9607f_i2c_write,
    .i2c_seq_write = dal_rtl9607f_i2c_seq_write,
    .i2c_read = dal_rtl9607f_i2c_read,
    .i2c_seq_read = dal_rtl9607f_i2c_seq_read,
    .i2c_clock_set = dal_rtl9607f_i2c_clock_set,
    .i2c_clock_get = dal_rtl9607f_i2c_clock_get,
    .i2c_addrWidth_set = NULL,
    .i2c_addrWidth_get = NULL,
    .i2c_dataWidth_set = NULL,
    .i2c_dataWidth_get = NULL,

#if defined(CONFIG_EPON_FEATURE)
    /* EPON */
    .epon_init = dal_rtl9607f_epon_init,
    .epon_intrMask_get = NULL,
    .epon_intrMask_set = NULL,
    .epon_intr_get = NULL,
    .epon_intr_disableAll = NULL,
    .epon_llid_entry_set = dal_rtl9607f_epon_llid_entry_set,
    .epon_llid_entry_get = dal_rtl9607f_epon_llid_entry_get,
    .epon_forceLaserState_set = NULL,
    .epon_forceLaserState_get = NULL,
    .epon_laserTime_set = NULL,
    .epon_laserTime_get = NULL,
    .epon_syncTime_get = NULL,
    .epon_registerReq_get = dal_rtl9607f_epon_registerReq_get,
    .epon_registerReq_set = dal_rtl9607f_epon_registerReq_set,
    .epon_churningKey_set = dal_rtl9607f_epon_churningKey_set,
    .epon_churningKey_get = dal_rtl9607f_epon_churningKey_get,
    .epon_usFecState_get = dal_rtl9607f_epon_usFecState_get,
    .epon_usFecState_set = dal_rtl9607f_epon_usFecState_set,
    .epon_dsFecState_get = dal_rtl9607f_epon_dsFecState_get,
    .epon_dsFecState_set = dal_rtl9607f_epon_dsFecState_set,
    .epon_mibCounter_get = dal_rtl9607f_epon_mibCounter_get,
    .epon_mibGlobal_reset = NULL,
    .epon_mibLlidIdx_reset = NULL,
    .epon_losState_get = dal_rtl9607f_epon_losState_get,
    .epon_mpcpTimeoutVal_get = NULL,
    .epon_mpcpTimeoutVal_set = NULL,
    .epon_opticalPolarity_set = NULL,
    .epon_opticalPolarity_get = NULL,
    .epon_fecState_get = NULL,
    .epon_fecState_set = NULL,
    .epon_llidEntryNum_get = dal_rtl9607f_epon_llidEntryNum_get,
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
#endif

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
    .mdio_init = dal_rtl9607f_mdio_init,
    .mdio_cfg_set = dal_rtl9607f_mdio_cfg_set,
    .mdio_cfg_get = dal_rtl9607f_mdio_cfg_get,
    .mdio_c22_write = dal_rtl9607f_mdio_c22_write,
    .mdio_c22_read = dal_rtl9607f_mdio_c22_read,
    .mdio_c45_write = dal_rtl9607f_mdio_c45_write,
    .mdio_c45_read = dal_rtl9607f_mdio_c45_read,
#if defined(CONFIG_COMMON_RT_API)
    .mdio_write = dal_rtl9607f_mdio_write,
    .mdio_read = dal_rtl9607f_mdio_read,
#endif
};


struct proc_dir_entry* ca_rtk_proc_dir = NULL;
struct proc_dir_entry* ca_rtk_debug_entry = NULL;

struct proc_dir_entry* ponmisc_proc_dir = NULL;
struct proc_dir_entry* ponmisc_cfg = NULL;

struct proc_dir_entry* ca_rtk_time_entry = NULL;


static int ca_rtk_debug_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "0x00000001 : RT_PARAM_CHK check error message\n");
    seq_printf(seq, "0x00000002 : RT_PARAM_CHK check error with dump stack\n");
    seq_printf(seq, "0x00000004 : Dump dal success message\n");
    seq_printf(seq, "0x00000008 : RTK_API_LOCK message\n");
    seq_printf(seq, "0x00000010 : RTK_API_LOCK with dump stack\n");
    seq_printf(seq, "0x00000020 : RT_ERR_CHK check error message\n");
    seq_printf(seq, "0x00000040 : Dump dal_rate database\n");
    seq_printf(seq, "0x00000080 : RT_ERR message\n");
    seq_printf(seq, "0x00000100 : RT_DBG message\n");
    seq_printf(seq, "0x00000200 : RT_LOG message\n");
    seq_printf(seq, "0x00000400 : DAL_DBG message\n");
    seq_printf(seq, "0x00000400 : RTL9607F_LOG_DBG message\n");
    seq_printf(seq, "ca rtk debug status: 0x%x\n", ca_rtk_debug);

    return 0;
}

static ssize_t ca_rtk_debug_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        ca_rtk_debug = simple_strtoul(tmpBuf, NULL, 16);
        printk("write ca rtk debug to 0x%x\n", ca_rtk_debug);
        if (0x40 & ca_rtk_debug)
        {
            _dal_rtl9607f_rate_glb_db_dump();
        }

        return count;
    }
    return -EFAULT;
}

static int ca_rtk_debug_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_debug_read, inode->i_private);
}

#if defined(CONFIG_GPON_FEATURE)||defined(CONFIG_EPON_FEATURE)
static int ponmisc_cfg_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "echo init > /proc/ca_rtk/ponmisc : diag rt_ponmisc init\n");
    seq_printf(seq, "echo bstPolRvs $state > /proc/ca_rtk/ponmisc : diag rt_ponmisc set burstPolarityReverse state [enable|disable]\n");
    seq_printf(seq, "  $state=1 means enable, $state=0 means disable\n");
    seq_printf(seq, "echo modSpd $pon_mode $pon_speed > /proc/ca_rtk/ponmisc : diag rt_ponmisc set mode $pon_mode speed $pon_speed\n");
    seq_printf(seq, "  $pon_mode=0/1/2/3/4/5/6 means GPON/EPON/NGPON2/XFI/FIBER/SGMII/USXGMII\n");
    seq_printf(seq, "  $pon_speed=0/1/2/3/4 means 2.5G/10G-asym/10G-sym/NGPON-asym/NGPON-sym\n");

    return 0;
}

static ssize_t ponmisc_cfg_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmp[32] = { 0 },*p,*para[4]={NULL};
    int len = (count > 31) ? 31 : count;
    int n=0, i;
    int32 ret;
    rt_ponmisc_ponMode_t mode;
    rt_ponmisc_ponSpeed_t speed;

    if (buffer && !copy_from_user(tmp, buffer, len)) {
        //printk("count=%d, cmd: %s", count, tmp);
        p=tmp;
        para[0]=p;
        for(i=0;i<count;i++)
        {
            if(*p=='\n')
                break;
            if(*p==' ')
            {
                *p='\0';
                para[++n]=++p;
                continue;
            }
            p++;

            if(n>=3)
                break;
        }

        if(n>=3)
            return (count);

        if (strncmp(para[0], "init", 4) == 0)
        {
            ret = dal_rt_rtl9607f_ponmisc_init();
            if(ret != RT_ERR_OK)
                printk("dal_rt_rtl9607f_ponmisc_init fail, ret=0x%x\n", ret);
            else
                printk("dal_rt_rtl9607f_ponmisc_init OK\n");
        }
        else if (strncmp(para[0], "bstPolRvs", 9) == 0)
        {
            if (strncmp(para[1], "enable", 6) == 0)
                ret = dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set(RT_PONMISC_POLARITY_REVERSE_ON);
            else
                ret = dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set(RT_PONMISC_POLARITY_REVERSE_OFF);
            if(ret != RT_ERR_OK)
                printk("dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set fail, ret=0x%x\n", ret);
            else
                printk("dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set OK\n");
        }
        else if (strncmp(para[0], "modSpd", 6) == 0)
        {
            mode=simple_strtoul(para[1], NULL, 10);
            speed=simple_strtoul(para[2], NULL, 10);
            ret = dal_rt_rtl9607f_ponmisc_mode_set(mode, speed);
            if(ret != RT_ERR_OK)
                printk("dal_rt_rtl9607f_ponmisc_mode_set fail, ret=0x%x\n", ret);
            else
                printk("dal_rt_rtl9607f_ponmisc_mode_set %d %d OK\n", mode, speed);
        }
        else if (strncmp(para[0], "rxLosRvsPol", 11) == 0)
        {
            if (strncmp(para[1], "enable", 6) == 0)
                ret = dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_set(RT_PONMISC_POLARITY_REVERSE_ON);
            else
                ret = dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_set(RT_PONMISC_POLARITY_REVERSE_OFF);
            if(ret != RT_ERR_OK)
                printk("dal_rt_rtl8277c_ponmisc_rxlosPolarityReverse_set fail, ret=0x%x\n", ret);
            else
                printk("dal_rt_rtl8277c_ponmisc_rxlosPolarityReverse_set OK\n");
        }
        else
            return (count);

    }
    return (count);
}

static int ponmisc_cfg_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ponmisc_cfg_read, inode->i_private);
}
#endif

static int ca_rtk_time_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "echo 0 [ldpid] [ts_oft] > /proc/ca_rtk/time : send one step ts to ldpid \n");
    seq_printf(seq, "echo 1 [ldpid] > /proc/ca_rtk/time : send two step ts to ldpid \n");
    seq_printf(seq, "echo 2 [lspid] > /proc/ca_rtk/time : set L2 ethertype 0x88f7 PTP trap to CPU \n");
    seq_printf(seq, "echo 3 [lspid] > /proc/ca_rtk/time : set UDP l4_DP 319 or 320 PTP trap to CPU \n");
    seq_printf(seq, "echo 4 [sec] [nsec] > /proc/ca_rtk/time : set pontod load time after 10sec \n");


    return 0;
}

uint32 ptpRX_callback(uint32 srcPort, uint32 msgLen,uint8 *pMsg, rt_time_timeStamp_t timeStamp)
{
    printk("srcPort = %d msgLen=%d timestmp = sec:%llu nsec:%u\n",srcPort,msgLen,timeStamp.sec,timeStamp.nsec);

    print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, pMsg, msgLen, true);

    return 0;
}

static ssize_t ca_rtk_time_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmp[32] = { 0 },*p,*para[4]={NULL};
    int len = (count > 31) ? 31 : count;
    int n=0, i;

    if (buffer && !copy_from_user(tmp, buffer, len)) {
        p=tmp;
        para[0]=p;
        for(i=0;i<count;i++)
        {
            if(*p=='\n')
                break;
            if(*p==' ')
            {
                *p='\0';
                para[++n]=++p;
                continue;
            }
            p++;

            if(n>=3)
                break;
        }

        if(n>=3)
            return (count);

        if(simple_strtoul(para[0], NULL, 10) == 0)
        {
            rt_time_timeStamp_t cur_ts,timeStamp;
            uint8 port;
            uint16 ts_oft;
            ca_uint8_t packet[64] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            timeStamp.sec = 0;
            timeStamp.nsec = 0;
            port = simple_strtoul(para[1], NULL, 10);
            ts_oft = simple_strtoul(para[2], NULL, 10);
            dal_rtl9607f_rt_time_ptpPortEnable_set(port,ENABLED);
            dal_rtl9607f_rt_time_ptpEgrAction_set(RT_PTP_EGR_ACTION_ONESTEP);
            dal_rtl9607f_rt_time_curTime_get(&cur_ts);
            dal_rtl9607f_rt_time_ptp_tx(port,64,packet,RT_PTP_EGR_TYPE_L2,ts_oft,0,&timeStamp);

            printk("one-step ldpid = %d before send time = sec:%llu nsec:%u, return timestmp = sec:%llu nsec:%u\n",port,cur_ts.sec,cur_ts.nsec,timeStamp.sec,timeStamp.nsec);
        }
        else if(simple_strtoul(para[0], NULL, 10) == 1)
        {
            rt_time_timeStamp_t cur_ts,timeStamp;
            uint8 port;
            ca_uint8_t packet[64] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            timeStamp.sec = 0;
            timeStamp.nsec = 0;
            port = simple_strtoul(para[1], NULL, 10);

            dal_rtl9607f_rt_time_ptpPortEnable_set(port,ENABLED);
            dal_rtl9607f_rt_time_ptpEgrAction_set(RT_PTP_EGR_ACTION_TWOSTEP);
            dal_rtl9607f_rt_time_curTime_get(&cur_ts);
            dal_rtl9607f_rt_time_ptp_tx(port,64,packet,RT_PTP_EGR_TYPE_L2,0,0,&timeStamp);

            printk("two-step ldpid = %d before send time = sec:%llu nsec:%u, return timestmp = sec:%llu nsec:%u\n",port,cur_ts.sec,cur_ts.nsec,timeStamp.sec,timeStamp.nsec);
        }
        else if(simple_strtoul(para[0], NULL, 10) == 2)
        {
            uint8 port;

            port = simple_strtoul(para[1], NULL, 10);

            dal_rtl9607f_rt_time_ptpPortEnable_set(port,ENABLED);
            dal_rtl9607f_rt_time_ptpIgrAction_set(RT_PTP_IGR_TYPE_L2,RT_PTP_IGR_ACTION_TRAP2CPU);
            dal_rtl9607f_rt_time_ptp_rx_callback_register(ptpRX_callback);
        }
        else if(simple_strtoul(para[0], NULL, 10) == 3)
        {
            uint8 port;

            port = simple_strtoul(para[1], NULL, 10);

            dal_rtl9607f_rt_time_ptpPortEnable_set(port,ENABLED);
            dal_rtl9607f_rt_time_ptpIgrAction_set(RT_PTP_IGR_TYPE_UDP,RT_PTP_IGR_ACTION_TRAP2CPU);
            dal_rtl9607f_rt_time_ptp_rx_callback_register(ptpRX_callback);
        }
        else if(simple_strtoul(para[0], NULL, 10) == 4)
        {
            rt_pon_tod_t ponTod;
            ca_uint32_t pon_mode = aal_pon_mac_mode_get(0);
            uint32 local_time;
            aal_gpon_stats_t gpon_stats;
            rt_time_timeStamp_t cur_ts;

            switch(pon_mode)
            {
                case ONU_PON_MAC_EPON_1G:
                case ONU_PON_MAC_EPON_D10G:
                case ONU_PON_MAC_EPON_BI10G:
                    //1TQ = 16ns, 62500TQ=1sec
                    aal_mpcp_local_time_get(0,0,&local_time);

                    ponTod.ponMode = 0;
                    ponTod.startPoint.localTime = local_time + 625000;
                    ponTod.timeStamp.sec = simple_strtoull(para[1],NULL,10);
                    ponTod.timeStamp.nsec = simple_strtoul(para[2],NULL,10);
                    dal_rtl9607f_rt_time_ponTodTime_set(ponTod);
                    dal_rtl9607f_rt_time_ponTodTimeEnable_set(ENABLED);

                    dal_rtl9607f_rt_time_curTime_get(&cur_ts);
                    printk("previous local time=%lu, timestamp sec:%llu nsec:%u \n",local_time,cur_ts.sec,cur_ts.nsec);
                    ca_mdelay(10000);

                    aal_mpcp_local_time_get(0,0,&local_time);
                    dal_rtl9607f_rt_time_curTime_get(&cur_ts);
                    printk("afer 10sec local time=%lu, timestamp sec:%llu nsec:%u \n",local_time,cur_ts.sec,cur_ts.nsec);

                    break;
                case ONU_PON_MAC_GPON:
                case ONU_PON_MAC_GPON_BI2G5:
                    //1superframe = 125us, 8000superframe=1sec
                    aal_gpon_port_stats_get(0,0,&gpon_stats);

                    ponTod.ponMode = 1;
                    ponTod.startPoint.superFrame = gpon_stats.superframe + 80000;
                    ponTod.timeStamp.sec = simple_strtoull(para[1],NULL,10);
                    ponTod.timeStamp.nsec = simple_strtoul(para[2],NULL,10);
                    dal_rtl9607f_rt_time_ponTodTime_set(ponTod);
                    dal_rtl9607f_rt_time_ponTodTimeEnable_set(ENABLED);

                    dal_rtl9607f_rt_time_curTime_get(&cur_ts);
                    printk("previous superframe=%lu, timestamp sec:%llu nsec:%u \n",gpon_stats.superframe,cur_ts.sec,cur_ts.nsec);
                    ca_mdelay(10000);

                    aal_gpon_port_stats_get(0,0,&gpon_stats);
                    dal_rtl9607f_rt_time_curTime_get(&cur_ts);
                    printk("afer 10sec superframe=%lu, timestamp sec:%llu nsec:%u \n",gpon_stats.superframe,cur_ts.sec,cur_ts.nsec);

                    break;
                case ONU_PON_MAC_XGPON1:
                case ONU_PON_MAC_XGSPON:
#if defined(CONFIG_10G_GPON_FEATURE)
                    /*TBD*/
#endif
                    break;
                default:
                        printk("not support PON mode=%d\n",pon_mode);
                    break;
            }
        }

    }
    return (count);
}

static int ca_rtk_time_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ca_rtk_time_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ca_rtk_debug_fop = {
    .proc_open           = ca_rtk_debug_open_proc,
    .proc_write          = ca_rtk_debug_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};

#if defined(CONFIG_GPON_FEATURE)||defined(CONFIG_EPON_FEATURE)
static const struct proc_ops ponmisc_cfg_fop = {
    .proc_open           = ponmisc_cfg_open_proc,
    .proc_write          = ponmisc_cfg_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#endif

static const struct proc_ops ca_rtk_time_fop = {
    .proc_open           = ca_rtk_time_open_proc,
    .proc_write          = ca_rtk_time_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations ca_rtk_debug_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_debug_open_proc,
    .write          = ca_rtk_debug_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

#if defined(CONFIG_GPON_FEATURE)||defined(CONFIG_EPON_FEATURE)
static const struct file_operations ponmisc_cfg_fop = {
    .owner          = THIS_MODULE,
    .open           = ponmisc_cfg_open_proc,
    .write          = ponmisc_cfg_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static const struct file_operations ca_rtk_time_fop = {
    .owner          = THIS_MODULE,
    .open           = ca_rtk_time_open_proc,
    .write          = ca_rtk_time_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int __init init_ca_rtk(void)
{
    dal_rtl9607f_mapper_register(&dal_rtl9607f_mapper);

    if (NULL == ca_rtk_proc_dir) {
        ca_rtk_proc_dir = proc_mkdir ("ca_rtk", NULL);
    }

    if(ca_rtk_proc_dir)
    {
        ca_rtk_debug_entry = proc_create("debug", 0644, ca_rtk_proc_dir, &ca_rtk_debug_fop);
        if (!ca_rtk_debug_entry) {
            printk("ca_rtk_debug_entry, create proc failed!");
        }

#if defined(CONFIG_GPON_FEATURE)||defined(CONFIG_EPON_FEATURE)
        ponmisc_cfg = proc_create("ponmisc", 0644, ca_rtk_proc_dir, &ponmisc_cfg_fop);
        if (!ponmisc_cfg)
        {
            printk("ponmisc_cfg , create proc failed!");
        }
#endif

        ca_rtk_time_entry = proc_create("time", 0644, ca_rtk_proc_dir, &ca_rtk_time_fop);
        if (!ca_rtk_time_entry) {
            printk("ca_rtk_time_entry, create proc failed!");
        }
    }

    rt_init_without_pon();

    return 0;
}

static void __exit exit_ca_rtk(void)
{
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

    if(ca_rtk_time_entry)
    {
        remove_proc_entry("time", ca_rtk_proc_dir);
        ca_rtk_time_entry = NULL;
    }

    dal_rtl9607f_mapper_unregister();
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek RTL9607F DAL module");
MODULE_AUTHOR("Realtek");
module_init(init_ca_rtk);
module_exit(exit_ca_rtk);


/*dummy definition please remove it*/
#define DUMMY_PRINT_ERR {dump_stack();printk("%s %s %d dummy function please remove it\n",__FILE__,__FUNCTION__,__LINE__);return CA_E_OK;}
/*dummy definition please remove it*/
