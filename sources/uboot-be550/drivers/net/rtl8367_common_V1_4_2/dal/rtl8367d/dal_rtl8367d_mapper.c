
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
#include <dal/dal_mapper.h>
#include <dal/rtl8367d/dal_rtl8367d_mapper.h>
#include <dal/rtl8367d/dal_rtl8367d_switch.h>
#include <dal/rtl8367d/dal_rtl8367d_eee.h>
#include <dal/rtl8367d/dal_rtl8367d_igmp.h>
#include <dal/rtl8367d/dal_rtl8367d_mirror.h>
#include <dal/rtl8367d/dal_rtl8367d_rate.h>
#include <dal/rtl8367d/dal_rtl8367d_storm.h>
#include <dal/rtl8367d/dal_rtl8367d_trap.h>
#include <dal/rtl8367d/dal_rtl8367d_port.h>
#include <dal/rtl8367d/dal_rtl8367d_dot1x.h>
#include <dal/rtl8367d/dal_rtl8367d_trunk.h>
#include <dal/rtl8367d/dal_rtl8367d_leaky.h>
#include <dal/rtl8367d/dal_rtl8367d_rldp.h>
#include <dal/rtl8367d/dal_rtl8367d_vlan.h>
#include <dal/rtl8367d/dal_rtl8367d_svlan.h>
#include <dal/rtl8367d/dal_rtl8367d_led.h>
#include <dal/rtl8367d/dal_rtl8367d_cpu.h>
#include <dal/rtl8367d/dal_rtl8367d_stat.h>
#include <dal/rtl8367d/dal_rtl8367d_l2.h>
#include <dal/rtl8367d/dal_rtl8367d_interrupt.h>
#include <dal/rtl8367d/dal_rtl8367d_acl.h>
#include <dal/rtl8367d/dal_rtl8367d_qos.h>
#include <dal/rtl8367d/dal_rtl8367d_gpio.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static dal_mapper_t dal_rtl8367d_mapper =
{
    /* Switch */
    .switch_init = dal_rtl8367d_switch_init,
    .switch_portMaxPktLen_set = dal_rtl8367d_switch_portMaxPktLen_set,
    .switch_portMaxPktLen_get = dal_rtl8367d_switch_portMaxPktLen_get,
    .switch_maxPktLenCfg_set = dal_rtl8367d_switch_maxPktLenCfg_set,
    .switch_maxPktLenCfg_get = dal_rtl8367d_switch_maxPktLenCfg_get,
    .switch_greenEthernet_set = NULL,
    .switch_greenEthernet_get = NULL,

    /* eee */
    .eee_init = NULL,
    .eee_portEnable_set = dal_rtl8367d_eee_portEnable_set,
    .eee_portEnable_get = dal_rtl8367d_eee_portEnable_get,

    /* led */
    .led_enable_set = dal_rtl8367d_led_enable_set,
    .led_enable_get = dal_rtl8367d_led_enable_get,
    .led_operation_set = dal_rtl8367d_led_operation_set,
    .led_operation_get = dal_rtl8367d_led_operation_get,
    .led_modeForce_set = dal_rtl8367d_led_modeForce_set,
    .led_modeForce_get = dal_rtl8367d_led_modeForce_get,
    .led_blinkRate_set = dal_rtl8367d_led_blinkRate_set,
    .led_blinkRate_get = dal_rtl8367d_led_blinkRate_get,
    .led_groupConfig_set = dal_rtl8367d_led_groupConfig_set,
    .led_groupConfig_get = dal_rtl8367d_led_groupConfig_get,
    .led_groupAbility_set = dal_rtl8367d_led_groupAbility_set,
    .led_groupAbility_get = dal_rtl8367d_led_groupAbility_get,
    .led_serialMode_set = dal_rtl8367d_led_serialMode_set,
    .led_serialMode_get = dal_rtl8367d_led_serialMode_get,
    .led_OutputEnable_set = dal_rtl8367d_led_OutputEnable_set,
    .led_OutputEnable_get = dal_rtl8367d_led_OutputEnable_get,
    .led_serialModePortmask_set = NULL,
    .led_serialModePortmask_get = NULL,

    /* oam */
    .oam_init = NULL,
    .oam_state_set = NULL,
    .oam_state_get = NULL,
    .oam_parserAction_set = NULL,
    .oam_parserAction_get = NULL,
    .oam_multiplexerAction_set = NULL,
    .oam_multiplexerAction_get = NULL,

    /* cpu */
    .cpu_enable_set = dal_rtl8367d_cpu_enable_set,
    .cpu_enable_get = dal_rtl8367d_cpu_enable_get,
    .cpu_tagPort_set = dal_rtl8367d_cpu_tagPort_set,
    .cpu_tagPort_get = dal_rtl8367d_cpu_tagPort_get,
    .cpu_awarePort_set = dal_rtl8367d_cpu_awarePort_set,
    .cpu_awarePort_get = dal_rtl8367d_cpu_awarePort_get,
    .cpu_tagPosition_set = dal_rtl8367d_cpu_tagPosition_set,
    .cpu_tagPosition_get = dal_rtl8367d_cpu_tagPosition_get,
    .cpu_tagLength_set = dal_rtl8367d_cpu_tagLength_set,
    .cpu_tagLength_get = dal_rtl8367d_cpu_tagLength_get,
    .cpu_acceptLength_set = dal_rtl8367d_cpu_acceptLength_set,
    .cpu_acceptLength_get = dal_rtl8367d_cpu_acceptLength_get,
    .cpu_priRemap_set = dal_rtl8367d_cpu_priRemap_set,
    .cpu_priRemap_get = dal_rtl8367d_cpu_priRemap_get,

    /* stat */
    .stat_global_reset = dal_rtl8367d_stat_global_reset,
    .stat_port_reset = dal_rtl8367d_stat_port_reset,
    .stat_queueManage_reset = dal_rtl8367d_stat_queueManage_reset,
    .stat_global_get = dal_rtl8367d_stat_global_get,
    .stat_global_getAll = dal_rtl8367d_stat_global_getAll,
    .stat_port_get = dal_rtl8367d_stat_port_get,
    .stat_port_getAll = dal_rtl8367d_stat_port_getAll,
    .stat_logging_counterCfg_set = dal_rtl8367d_stat_logging_counterCfg_set,
    .stat_logging_counterCfg_get = dal_rtl8367d_stat_logging_counterCfg_get,
    .stat_logging_counter_reset = dal_rtl8367d_stat_logging_counter_reset,
    .stat_logging_counter_get = dal_rtl8367d_stat_logging_counter_get,
    .stat_lengthMode_set = dal_rtl8367d_stat_lengthMode_set,
    .stat_lengthMode_get = dal_rtl8367d_stat_lengthMode_get,

    /* l2 */
    .l2_init = dal_rtl8367d_l2_init,
    .l2_addr_add = dal_rtl8367d_l2_addr_add,
    .l2_addr_get = dal_rtl8367d_l2_addr_get,
    .l2_addr_next_get = dal_rtl8367d_l2_addr_next_get,
    .l2_addr_del = dal_rtl8367d_l2_addr_del,
    .l2_mcastAddr_add = dal_rtl8367d_l2_mcastAddr_add,
    .l2_mcastAddr_get = dal_rtl8367d_l2_mcastAddr_get,
    .l2_mcastAddr_next_get = dal_rtl8367d_l2_mcastAddr_next_get,
    .l2_mcastAddr_del = dal_rtl8367d_l2_mcastAddr_del,
    .l2_ipMcastAddr_add = dal_rtl8367d_l2_ipMcastAddr_add,
    .l2_ipMcastAddr_get = dal_rtl8367d_l2_ipMcastAddr_get,
    .l2_ipMcastAddr_next_get = dal_rtl8367d_l2_ipMcastAddr_next_get,
    .l2_ipMcastAddr_del = dal_rtl8367d_l2_ipMcastAddr_del,
    .l2_ipVidMcastAddr_add = NULL,
    .l2_ipVidMcastAddr_get = NULL,
    .l2_ipVidMcastAddr_next_get = NULL,
    .l2_ipVidMcastAddr_del = NULL,
    .l2_ucastAddr_flush = dal_rtl8367d_l2_ucastAddr_flush,
    .l2_table_clear = dal_rtl8367d_l2_table_clear,
    .l2_table_clearStatus_get = dal_rtl8367d_l2_table_clearStatus_get,
    .l2_flushLinkDownPortAddrEnable_set = dal_rtl8367d_l2_flushLinkDownPortAddrEnable_set,
    .l2_flushLinkDownPortAddrEnable_get = dal_rtl8367d_l2_flushLinkDownPortAddrEnable_get,
    .l2_agingEnable_set = dal_rtl8367d_l2_agingEnable_set,
    .l2_agingEnable_get = dal_rtl8367d_l2_agingEnable_get,
    .l2_limitLearningCnt_set = dal_rtl8367d_l2_limitLearningCnt_set,
    .l2_limitLearningCnt_get = dal_rtl8367d_l2_limitLearningCnt_get,
    .l2_limitSystemLearningCnt_set = dal_rtl8367d_l2_limitSystemLearningCnt_set,
    .l2_limitSystemLearningCnt_get = dal_rtl8367d_l2_limitSystemLearningCnt_get,
    .l2_limitLearningCntAction_set = dal_rtl8367d_l2_limitLearningCntAction_set,
    .l2_limitLearningCntAction_get = dal_rtl8367d_l2_limitLearningCntAction_get,
    .l2_limitSystemLearningCntAction_set = dal_rtl8367d_l2_limitSystemLearningCntAction_set,
    .l2_limitSystemLearningCntAction_get = dal_rtl8367d_l2_limitSystemLearningCntAction_get,
    .l2_limitSystemLearningCntPortMask_set = dal_rtl8367d_l2_limitSystemLearningCntPortMask_set,
    .l2_limitSystemLearningCntPortMask_get = dal_rtl8367d_l2_limitSystemLearningCntPortMask_get,
    .l2_learningCnt_get = dal_rtl8367d_l2_learningCnt_get,
    .l2_floodPortMask_set = dal_rtl8367d_l2_floodPortMask_set,
    .l2_floodPortMask_get = dal_rtl8367d_l2_floodPortMask_get,
    .l2_localPktPermit_set = dal_rtl8367d_l2_localPktPermit_set,
    .l2_localPktPermit_get = dal_rtl8367d_l2_localPktPermit_get,
    .l2_aging_set = dal_rtl8367d_l2_aging_set,
    .l2_aging_get = dal_rtl8367d_l2_aging_get,
    .l2_ipMcastAddrLookup_set = dal_rtl8367d_l2_ipMcastAddrLookup_set,
    .l2_ipMcastAddrLookup_get = dal_rtl8367d_l2_ipMcastAddrLookup_get,
    .l2_ipMcastForwardRouterPort_set = NULL,
    .l2_ipMcastForwardRouterPort_get = NULL,
    .l2_ipMcastGroupEntry_add = dal_rtl8367d_l2_ipMcastGroupEntry_add,
    .l2_ipMcastGroupEntry_del = dal_rtl8367d_l2_ipMcastGroupEntry_del,
    .l2_ipMcastGroupEntry_get = dal_rtl8367d_l2_ipMcastGroupEntry_get,
    .l2_entry_get = dal_rtl8367d_l2_entry_get,
    .l2_lookupHitIsolationAction_set = dal_rtl8367d_l2_lookupHitIsolationAction_set,
    .l2_lookupHitIsolationAction_get = dal_rtl8367d_l2_lookupHitIsolationAction_get,

    /* interrupt */
    .int_polarity_set = dal_rtl8367d_int_polarity_set,
    .int_polarity_get = dal_rtl8367d_int_polarity_get,
    .int_control_set = dal_rtl8367d_int_control_set,
    .int_control_get = dal_rtl8367d_int_control_get,
    .int_status_set = dal_rtl8367d_int_status_set,
    .int_status_get = dal_rtl8367d_int_status_get,
    .int_advanceInfo_get = dal_rtl8367d_int_advanceInfo_get,

    /* acl */
    .filter_igrAcl_init = dal_rtl8367d_filter_igrAcl_init,
    .filter_igrAcl_field_add = dal_rtl8367d_filter_igrAcl_field_add,
    .filter_igrAcl_cfg_add = dal_rtl8367d_filter_igrAcl_cfg_add,
    .filter_igrAcl_cfg_del = dal_rtl8367d_filter_igrAcl_cfg_del,
    .filter_igrAcl_cfg_delAll = dal_rtl8367d_filter_igrAcl_cfg_delAll,
    .filter_igrAcl_cfg_get = dal_rtl8367d_filter_igrAcl_cfg_get,
    .filter_igrAcl_unmatchAction_set = dal_rtl8367d_filter_igrAcl_unmatchAction_set,
    .filter_igrAcl_unmatchAction_get = dal_rtl8367d_filter_igrAcl_unmatchAction_get,
    .filter_igrAcl_state_set = dal_rtl8367d_filter_igrAcl_state_set,
    .filter_igrAcl_state_get = dal_rtl8367d_filter_igrAcl_state_get,
    .filter_igrAcl_template_set = dal_rtl8367d_filter_igrAcl_template_set,
    .filter_igrAcl_template_get = dal_rtl8367d_filter_igrAcl_template_get,
    .filter_igrAcl_field_sel_set = dal_rtl8367d_filter_igrAcl_field_sel_set,
    .filter_igrAcl_field_sel_get = dal_rtl8367d_filter_igrAcl_field_sel_get,
    .filter_iprange_set = dal_rtl8367d_filter_iprange_set,
    .filter_iprange_get = dal_rtl8367d_filter_iprange_get,
    .filter_vidrange_set = NULL,
    .filter_vidrange_get = NULL,
    .filter_portrange_set = NULL,
    .filter_portrange_get = NULL,
    .filter_igrAclPolarity_set = NULL,
    .filter_igrAclPolarity_get = NULL,

    /* mirror */
    .mirror_portBased_set = dal_rtl8367d_mirror_portBased_set,
    .mirror_portBased_get = dal_rtl8367d_mirror_portBased_get,
    .mirror_portIso_set = dal_rtl8367d_mirror_portIso_set,
    .mirror_portIso_get = dal_rtl8367d_mirror_portIso_get,
    .mirror_vlanLeaky_set = dal_rtl8367d_mirror_vlanLeaky_set,
    .mirror_vlanLeaky_get = dal_rtl8367d_mirror_vlanLeaky_get,
    .mirror_isolationLeaky_set = dal_rtl8367d_mirror_isolationLeaky_set,
    .mirror_isolationLeaky_get = dal_rtl8367d_mirror_isolationLeaky_get,
    .mirror_keep_set = dal_rtl8367d_mirror_keep_set,
    .mirror_keep_get = dal_rtl8367d_mirror_keep_get,
    .mirror_override_set = dal_rtl8367d_mirror_override_set,
    .mirror_override_get = dal_rtl8367d_mirror_override_get,

    /* port */
    .port_phyAutoNegoAbility_set = dal_rtl8367d_port_phyAutoNegoAbility_set,
    .port_phyAutoNegoAbility_get = dal_rtl8367d_port_phyAutoNegoAbility_get,
    .port_phyForceModeAbility_set = dal_rtl8367d_port_phyForceModeAbility_set,
    .port_phyForceModeAbility_get = dal_rtl8367d_port_phyForceModeAbility_get,
    .port_phyStatus_get = dal_rtl8367d_port_phyStatus_get,
    .port_macForceLink_set = dal_rtl8367d_port_macForceLink_set,
    .port_macForceLink_get = dal_rtl8367d_port_macForceLink_get,
    .port_macForceLinkExt_set = dal_rtl8367d_port_macForceLinkExt_set,
    .port_macForceLinkExt_get = dal_rtl8367d_port_macForceLinkExt_get,
    .port_macStatus_get = dal_rtl8367d_port_macStatus_get,
    .port_macLocalLoopbackEnable_set = dal_rtl8367d_port_macLocalLoopbackEnable_set,
    .port_macLocalLoopbackEnable_get = dal_rtl8367d_port_macLocalLoopbackEnable_get,
    .port_phyReg_set = dal_rtl8367d_port_phyReg_set,
    .port_phyReg_get = dal_rtl8367d_port_phyReg_get,
    .port_phyOCPReg_set = dal_rtl8367d_port_phyOCPReg_set,
    .port_phyOCPReg_get = dal_rtl8367d_port_phyOCPReg_get,
    .port_backpressureEnable_set = dal_rtl8367d_port_backpressureEnable_set,
    .port_backpressureEnable_get = dal_rtl8367d_port_backpressureEnable_get,
    .port_adminEnable_set = dal_rtl8367d_port_adminEnable_set,
    .port_adminEnable_get = dal_rtl8367d_port_adminEnable_get,
    .port_isolation_set = dal_rtl8367d_port_isolation_set,
    .port_isolation_get = dal_rtl8367d_port_isolation_get,
    .port_rgmiiDelayExt_set = dal_rtl8367d_port_rgmiiDelayExt_set,
    .port_rgmiiDelayExt_get = dal_rtl8367d_port_rgmiiDelayExt_get,
    .port_phyEnableAll_set = dal_rtl8367d_port_phyEnableAll_set,
    .port_phyEnableAll_get = dal_rtl8367d_port_phyEnableAll_get,
    .port_efid_set = NULL,
    .port_efid_get = NULL,
    .port_phyComboPortMedia_set = dal_rtl8367d_port_phyComboPortMedia_set,
    .port_phyComboPortMedia_get = dal_rtl8367d_port_phyComboPortMedia_get,
    .port_rtctEnable_set = dal_rtl8367d_port_rtctEnable_set,
    .port_rtctDisable_set = dal_rtl8367d_port_rtctDisable_set,
    .port_rtctResult_get = dal_rtl8367d_port_rtctResult_get,
    .port_sds_reset = NULL,
    .port_sgmiiLinkStatus_get = dal_rtl8367d_port_sgmiiLinkStatus_get,
    .port_sgmiiNway_set = dal_rtl8367d_port_sgmiiNway_set,
    .port_sgmiiNway_get = dal_rtl8367d_port_sgmiiNway_get,
    .port_fiberAbilityExt_set = NULL,
    .port_fiberAbilityExt_get = NULL,
    .port_autoDos_set = dal_rtl8367d_port_autoDos_set,
    .port_autoDos_get = dal_rtl8367d_port_autoDos_get,
    .port_fiberAbility_set = dal_rtl8367d_port_fiberAbility_set,
    .port_fiberAbility_get = dal_rtl8367d_port_fiberAbility_get,
    .port_phyMdx_set = dal_rtl8367d_port_phyMdx_set,
    .port_phyMdx_get = dal_rtl8367d_port_phyMdx_get,
    .port_phyMdxStatus_get = dal_rtl8367d_port_phyMdxStatus_get,
    .port_phyTestMode_set = dal_rtl8367d_port_phyTestMode_set,
    .port_phyTestMode_get = dal_rtl8367d_port_phyTestMode_get,

    /* Trap */
    .trap_unknownUnicastPktAction_set = dal_rtl8367d_trap_unknownUnicastPktAction_set,
    .trap_unknownUnicastPktAction_get = dal_rtl8367d_trap_unknownUnicastPktAction_get,
    .trap_unknownMacPktAction_set = NULL,
    .trap_unknownMacPktAction_get = NULL,
    .trap_unmatchMacPktAction_set = NULL,
    .trap_unmatchMacPktAction_get = NULL,
    .trap_unmatchMacMoving_set = dal_rtl8367d_trap_unmatchMacMoving_set,
    .trap_unmatchMacMoving_get = dal_rtl8367d_trap_unmatchMacMoving_get,
    .trap_unknownMcastPktAction_set = dal_rtl8367d_trap_unknownMcastPktAction_set,
    .trap_unknownMcastPktAction_get = dal_rtl8367d_trap_unknownMcastPktAction_get,
    .trap_lldpEnable_set = dal_rtl8367d_trap_lldpEnable_set,
    .trap_lldpEnable_get = dal_rtl8367d_trap_lldpEnable_get,
    .trap_reasonTrapToCpuPriority_set = dal_rtl8367d_trap_reasonTrapToCpuPriority_set,
    .trap_reasonTrapToCpuPriority_get = dal_rtl8367d_trap_reasonTrapToCpuPriority_get,
    .trap_rmaAction_set = dal_rtl8367d_trap_rmaAction_set,
    .trap_rmaAction_get = dal_rtl8367d_trap_rmaAction_get,
    .trap_rmaKeepFormat_set = dal_rtl8367d_trap_rmaKeepFormat_set,
    .trap_rmaKeepFormat_get = dal_rtl8367d_trap_rmaKeepFormat_get,
    .trap_portUnknownMacPktAction_set = dal_rtl8367d_trap_portUnknownMacPktAction_set,
    .trap_portUnknownMacPktAction_get = dal_rtl8367d_trap_portUnknownMacPktAction_get,
    .trap_portUnmatchMacPktAction_set = dal_rtl8367d_trap_portUnmatchMacPktAction_set,
    .trap_portUnmatchMacPktAction_get = dal_rtl8367d_trap_portUnmatchMacPktAction_get,

    /* IGMP */
    .igmp_init = NULL,
    .igmp_state_set = NULL,
    .igmp_state_get = NULL,
    .igmp_static_router_port_set = NULL,
    .igmp_static_router_port_get = NULL,
    .igmp_protocol_set = dal_rtl8367d_igmp_protocol_set,
    .igmp_protocol_get = dal_rtl8367d_igmp_protocol_get,
    .igmp_fastLeave_set = NULL,
    .igmp_fastLeave_get = NULL,
    .igmp_maxGroup_set = NULL,
    .igmp_maxGroup_get = NULL,
    .igmp_currentGroup_get = NULL,
    .igmp_tableFullAction_set = NULL,
    .igmp_tableFullAction_get = NULL,
    .igmp_checksumErrorAction_set = NULL,
    .igmp_checksumErrorAction_get = NULL,
    .igmp_leaveTimer_set = NULL,
    .igmp_leaveTimer_get = NULL,
    .igmp_queryInterval_set = NULL,
    .igmp_queryInterval_get = NULL,
    .igmp_robustness_set = NULL,
    .igmp_robustness_get = NULL,
    .igmp_dynamicRouterPortAllow_set = NULL,
    .igmp_dynamicRouterPortAllow_get = NULL,
    .igmp_dynamicRouterPort_get = NULL,
    .igmp_suppressionEnable_set = NULL,
    .igmp_suppressionEnable_get = NULL,
    .igmp_portRxPktEnable_set = NULL,
    .igmp_portRxPktEnable_get = NULL,
    .igmp_groupInfo_get = NULL,
    .igmp_ReportLeaveFwdAction_set = NULL,
    .igmp_ReportLeaveFwdAction_get = NULL,
    .igmp_dropLeaveZeroEnable_set = NULL,
    .igmp_dropLeaveZeroEnable_get = NULL,
    .igmp_bypassGroupRange_set = dal_rtl8367d_igmp_bypassGroupRange_set,
    .igmp_bypassGroupRange_get = dal_rtl8367d_igmp_bypassGroupRange_get,

    /* Storm */
    .rate_stormControlMeterIdx_set = dal_rtl8367d_rate_stormControlMeterIdx_set,
    .rate_stormControlMeterIdx_get = dal_rtl8367d_rate_stormControlMeterIdx_get,
    .rate_stormControlPortEnable_set = dal_rtl8367d_rate_stormControlPortEnable_set,
    .rate_stormControlPortEnable_get = dal_rtl8367d_rate_stormControlPortEnable_get,
    .storm_bypass_set = dal_rtl8367d_storm_bypass_set,
    .storm_bypass_get = dal_rtl8367d_storm_bypass_get,
    .rate_stormControlExtPortmask_set = dal_rtl8367d_rate_stormControlExtPortmask_set,
    .rate_stormControlExtPortmask_get = dal_rtl8367d_rate_stormControlExtPortmask_get,
    .rate_stormControlExtEnable_set = dal_rtl8367d_rate_stormControlExtEnable_set,
    .rate_stormControlExtEnable_get = dal_rtl8367d_rate_stormControlExtEnable_get,
    .rate_stormControlExtMeterIdx_set = dal_rtl8367d_rate_stormControlExtMeterIdx_set,
    .rate_stormControlExtMeterIdx_get = dal_rtl8367d_rate_stormControlExtMeterIdx_get,

    /* Rate */
    .rate_shareMeter_set = dal_rtl8367d_rate_shareMeter_set,
    .rate_shareMeter_get = dal_rtl8367d_rate_shareMeter_get,
    .rate_shareMeterBucket_set = dal_rtl8367d_rate_shareMeterBucket_set,
    .rate_shareMeterBucket_get = dal_rtl8367d_rate_shareMeterBucket_get,
    .rate_igrBandwidthCtrlRate_set = dal_rtl8367d_rate_igrBandwidthCtrlRate_set,
    .rate_igrBandwidthCtrlRate_get = dal_rtl8367d_rate_igrBandwidthCtrlRate_get,
    .rate_egrBandwidthCtrlRate_set = dal_rtl8367d_rate_egrBandwidthCtrlRate_set,
    .rate_egrBandwidthCtrlRate_get = dal_rtl8367d_rate_egrBandwidthCtrlRate_get,
    .rate_egrQueueBwCtrlEnable_set = dal_rtl8367d_rate_egrQueueBwCtrlEnable_set,
    .rate_egrQueueBwCtrlEnable_get = dal_rtl8367d_rate_egrQueueBwCtrlEnable_get,
    .rate_egrQueueBwCtrlRate_set = dal_rtl8367d_rate_egrQueueBwCtrlRate_set,
    .rate_egrQueueBwCtrlRate_get = dal_rtl8367d_rate_egrQueueBwCtrlRate_get,

    /* I2C */
    .i2c_init = NULL,
    .i2c_data_read = NULL,
    .i2c_data_write = NULL,
    .i2c_mode_set = NULL,
    .i2c_mode_get = NULL,
    .i2c_gpioPinGroup_set = NULL,
    .i2c_gpioPinGroup_get = NULL,
#if 0
    /*PTP*/
    .ptp_init = dal_rtl8367d_ptp_init,
    .ptp_mac_set = dal_rtl8367d_ptp_mac_set,
    .ptp_mac_get = dal_rtl8367d_ptp_mac_get,
    .ptp_tpid_set = dal_rtl8367d_ptp_tpid_set,
    .ptp_tpid_get = dal_rtl8367d_ptp_tpid_get,
    .ptp_refTime_set = dal_rtl8367d_ptp_refTime_set,
    .ptp_refTime_get = dal_rtl8367d_ptp_refTime_get,
    .ptp_refTimeAdjust_set = dal_rtl8367d_ptp_refTimeAdjust_set,
    .ptp_refTimeEnable_set = dal_rtl8367d_ptp_refTimeEnable_set,
    .ptp_refTimeEnable_get = dal_rtl8367d_ptp_refTimeEnable_get,
    .ptp_portEnable_set = dal_rtl8367d_ptp_portEnable_set,
    .ptp_portEnable_get = dal_rtl8367d_ptp_portEnable_get,
    .ptp_portTimestamp_get = dal_rtl8367d_ptp_portTimestamp_get,
    .ptp_intControl_set = dal_rtl8367d_ptp_intControl_set,
    .ptp_intControl_get = dal_rtl8367d_ptp_intControl_get,
    .ptp_intStatus_get = dal_rtl8367d_ptp_intStatus_get,
    .ptp_portIntStatus_set = dal_rtl8367d_ptp_portIntStatus_set,
    .ptp_portIntStatus_get = dal_rtl8367d_ptp_portIntStatus_get,
    .ptp_portTrap_set = dal_rtl8367d_ptp_portTrap_set,
    .ptp_portTrap_get = dal_rtl8367d_ptp_portTrap_get,
#endif
    /*QoS*/
    .qos_init = dal_rtl8367d_qos_init,
    .qos_priSel_set = dal_rtl8367d_qos_priSel_set,
    .qos_priSel_get = dal_rtl8367d_qos_priSel_get,
    .qos_1pPriRemap_set = dal_rtl8367d_qos_1pPriRemap_set,
    .qos_1pPriRemap_get = dal_rtl8367d_qos_1pPriRemap_get,
    .qos_1pRemarkSrcSel_set = dal_rtl8367d_qos_1pRemarkSrcSel_set,
    .qos_1pRemarkSrcSel_get = dal_rtl8367d_qos_1pRemarkSrcSel_get,
    .qos_dscpPriRemap_set = dal_rtl8367d_qos_dscpPriRemap_set,
    .qos_dscpPriRemap_get = dal_rtl8367d_qos_dscpPriRemap_get,
    .qos_portPri_set = dal_rtl8367d_qos_portPri_set,
    .qos_portPri_get = dal_rtl8367d_qos_portPri_get,
    .qos_queueNum_set = dal_rtl8367d_qos_queueNum_set,
    .qos_queueNum_get = dal_rtl8367d_qos_queueNum_get,
    .qos_priMap_set = dal_rtl8367d_qos_priMap_set,
    .qos_priMap_get = dal_rtl8367d_qos_priMap_get,
    .qos_schedulingQueue_set = dal_rtl8367d_qos_schedulingQueue_set,
    .qos_schedulingQueue_get = dal_rtl8367d_qos_schedulingQueue_get,
    .qos_1pRemarkEnable_set = dal_rtl8367d_qos_1pRemarkEnable_set,
    .qos_1pRemarkEnable_get = dal_rtl8367d_qos_1pRemarkEnable_get,
    .qos_1pRemark_set = dal_rtl8367d_qos_1pRemark_set,
    .qos_1pRemark_get = dal_rtl8367d_qos_1pRemark_get,
    .qos_dscpRemarkEnable_set = dal_rtl8367d_qos_dscpRemarkEnable_set,
    .qos_dscpRemarkEnable_get = dal_rtl8367d_qos_dscpRemarkEnable_get,
    .qos_dscpRemark_set = dal_rtl8367d_qos_dscpRemark_set,
    .qos_dscpRemark_get = dal_rtl8367d_qos_dscpRemark_get,
    .qos_dscpRemarkSrcSel_set = dal_rtl8367d_qos_dscpRemarkSrcSel_set,
    .qos_dscpRemarkSrcSel_get = dal_rtl8367d_qos_dscpRemarkSrcSel_get,
    .qos_dscpRemark2Dscp_set = NULL,
    .qos_dscpRemark2Dscp_get = NULL,
    .qos_portPriSelIndex_set = dal_rtl8367d_qos_portPriSelIndex_set,
    .qos_portPriSelIndex_get = dal_rtl8367d_qos_portPriSelIndex_get,
    .qos_schedulingType_set = dal_rtl8367d_qos_schedulingType_set,
    .qos_schedulingType_get = dal_rtl8367d_qos_schedulingType_get,


    /*VLAN*/
    .vlan_init = dal_rtl8367d_vlan_init,
    .vlan_set = dal_rtl8367d_vlan_set,
    .vlan_get = dal_rtl8367d_vlan_get,
    .vlan_egrFilterEnable_set = dal_rtl8367d_vlan_egrFilterEnable_set,
    .vlan_egrFilterEnable_get = dal_rtl8367d_vlan_egrFilterEnable_get,
    .vlan_mbrCfg_set = NULL,
    .vlan_mbrCfg_get = NULL,
    .vlan_portPvid_set = dal_rtl8367d_vlan_portPvid_set,
    .vlan_portPvid_get = dal_rtl8367d_vlan_portPvid_get,
    .vlan_portIgrFilterEnable_set = dal_rtl8367d_vlan_portIgrFilterEnable_set,
    .vlan_portIgrFilterEnable_get = dal_rtl8367d_vlan_portIgrFilterEnable_get,
    .vlan_portAcceptFrameType_set = dal_rtl8367d_vlan_portAcceptFrameType_set,
    .vlan_portAcceptFrameType_get = dal_rtl8367d_vlan_portAcceptFrameType_get,
    .vlan_tagMode_set = dal_rtl8367d_vlan_tagMode_set,
    .vlan_tagMode_get = dal_rtl8367d_vlan_tagMode_get,
    .vlan_transparent_set = dal_rtl8367d_vlan_transparent_set,
    .vlan_transparent_get = dal_rtl8367d_vlan_transparent_get,
    .vlan_keep_set = dal_rtl8367d_vlan_keep_set,
    .vlan_keep_get = dal_rtl8367d_vlan_keep_get,
    .vlan_stg_set = dal_rtl8367d_vlan_stg_set,
    .vlan_stg_get = dal_rtl8367d_vlan_stg_get,
    .vlan_protoAndPortBasedVlan_add = NULL,
    .vlan_protoAndPortBasedVlan_get = NULL,
    .vlan_protoAndPortBasedVlan_del = NULL,
    .vlan_protoAndPortBasedVlan_delAll = NULL,
    .vlan_portFid_set = dal_rtl8367d_vlan_portFid_set,
    .vlan_portFid_get = dal_rtl8367d_vlan_portFid_get,
    .vlan_UntagDscpPriorityEnable_set = NULL,
    .vlan_UntagDscpPriorityEnable_get = NULL,
    .stp_mstpState_set = dal_rtl8367d_stp_mstpState_set,
    .stp_mstpState_get = dal_rtl8367d_stp_mstpState_get,
    .vlan_reservedVidAction_set = dal_rtl8367d_vlan_reservedVidAction_set,
    .vlan_reservedVidAction_get = dal_rtl8367d_vlan_reservedVidAction_get,
    .vlan_realKeepRemarkEnable_set = dal_rtl8367d_vlan_realKeepRemarkEnable_set,
    .vlan_realKeepRemarkEnable_get = dal_rtl8367d_vlan_realKeepRemarkEnable_get,
    .vlan_reset = dal_rtl8367d_vlan_reset,

    /*dot1x*/
    .dot1x_unauthPacketOper_set = dal_rtl8367d_dot1x_unauthPacketOper_set,
    .dot1x_unauthPacketOper_get = dal_rtl8367d_dot1x_unauthPacketOper_get,
    .dot1x_eapolFrame2CpuEnable_set = dal_rtl8367d_dot1x_eapolFrame2CpuEnable_set,
    .dot1x_eapolFrame2CpuEnable_get = dal_rtl8367d_dot1x_eapolFrame2CpuEnable_get,
    .dot1x_portBasedEnable_set = dal_rtl8367d_dot1x_portBasedEnable_set,
    .dot1x_portBasedEnable_get = dal_rtl8367d_dot1x_portBasedEnable_get,
    .dot1x_portBasedAuthStatus_set = dal_rtl8367d_dot1x_portBasedAuthStatus_set,
    .dot1x_portBasedAuthStatus_get = dal_rtl8367d_dot1x_portBasedAuthStatus_get,
    .dot1x_portBasedDirection_set = dal_rtl8367d_dot1x_portBasedDirection_set,
    .dot1x_portBasedDirection_get = dal_rtl8367d_dot1x_portBasedDirection_get,
    .dot1x_macBasedEnable_set = NULL,
    .dot1x_macBasedEnable_get = NULL,
    .dot1x_macBasedAuthMac_add = NULL,
    .dot1x_macBasedAuthMac_del = NULL,
    .dot1x_macBasedDirection_set = NULL,
    .dot1x_macBasedDirection_get = NULL,
    .dot1x_guestVlan_set = NULL,
    .dot1x_guestVlan_get = NULL,
    .dot1x_guestVlan2Auth_set = NULL,
    .dot1x_guestVlan2Auth_get = NULL,

    /*SVLAN*/
    .svlan_init = dal_rtl8367d_svlaninit,
    .svlan_servicePort_add = dal_rtl8367d_svlanservicePort_add,
    .svlan_servicePort_get = dal_rtl8367d_svlanservicePort_get,
    .svlan_servicePort_del = dal_rtl8367d_svlanservicePort_del,
    .svlan_tpidEntry_set = dal_rtl8367d_svlantpidEntry_set,
    .svlan_tpidEntry_get = dal_rtl8367d_svlantpidEntry_get,
    .svlan_priorityRef_set = dal_rtl8367d_svlanpriorityRef_set,
    .svlan_priorityRef_get = dal_rtl8367d_svlanpriorityRef_get,
    .svlan_memberPortEntry_set = dal_rtl8367d_svlanmemberPortEntry_set,
    .svlan_memberPortEntry_get = dal_rtl8367d_svlanmemberPortEntry_get,
    .svlan_memberPortEntry_adv_set = NULL,
    .svlan_memberPortEntry_adv_get = NULL,
    .svlan_defaultSvlan_set = dal_rtl8367d_svlandefaultSvlan_set,
    .svlan_defaultSvlan_get = dal_rtl8367d_svlandefaultSvlan_get,
    .svlan_c2s_add = dal_rtl8367d_svlanc2s_add,
    .svlan_c2s_del = dal_rtl8367d_svlanc2s_del,
    .svlan_c2s_get = dal_rtl8367d_svlanc2s_get,
    .svlan_untag_action_set = dal_rtl8367d_svlanuntag_action_set,
    .svlan_untag_action_get = dal_rtl8367d_svlanuntag_action_get,
    .svlan_unmatch_action_set = NULL,
    .svlan_unmatch_action_get = NULL,
    .svlan_dmac_vidsel_set = NULL,
    .svlan_dmac_vidsel_get = NULL,
    .svlan_ipmc2s_add = NULL,
    .svlan_ipmc2s_del = NULL,
    .svlan_ipmc2s_get = NULL,
    .svlan_l2mc2s_add = NULL,
    .svlan_l2mc2s_del = NULL,
    .svlan_l2mc2s_get = NULL,
    .svlan_sp2c_add = dal_rtl8367d_svlan_sp2c_add,
    .svlan_sp2c_get = dal_rtl8367d_svlan_sp2c_get,
    .svlan_sp2c_del = dal_rtl8367d_svlan_sp2c_del,
    .svlan_lookupType_set = NULL,
    .svlan_lookupType_get = NULL,
    .svlan_trapPri_set = dal_rtl8367d_svlantrapPri_set,
    .svlan_trapPri_get = dal_rtl8367d_svlantrapPri_get,
    .svlan_unassign_action_set = dal_rtl8367d_svlanunassign_action_set,
    .svlan_unassign_action_get = dal_rtl8367d_svlanunassign_action_get,

    /*RLDP*/
    .rldp_config_set = dal_rtl8367d_rldp_config_set,
    .rldp_config_get = dal_rtl8367d_rldp_config_get,
    .rldp_portConfig_set = dal_rtl8367d_rldp_portConfig_set,
    .rldp_portConfig_get = dal_rtl8367d_rldp_portConfig_get,
    .rldp_status_get = dal_rtl8367d_rldp_status_get,
    .rldp_portStatus_get = dal_rtl8367d_rldp_portStatus_get,
    .rldp_portStatus_set = dal_rtl8367d_rldp_portStatus_set,
    .rldp_portLoopPair_get = dal_rtl8367d_rldp_portLoopPair_get,


    /*trunk*/
    .trunk_port_set = dal_rtl8367d_trunk_port_set,
    .trunk_port_get = dal_rtl8367d_trunk_port_get,
    .trunk_distributionAlgorithm_set = dal_rtl8367d_trunk_distributionAlgorithm_set,
    .trunk_distributionAlgorithm_get = dal_rtl8367d_trunk_distributionAlgorithm_get,
    .trunk_trafficSeparate_set = dal_rtl8367d_trunk_trafficSeparate_set,
    .trunk_trafficSeparate_get = dal_rtl8367d_trunk_trafficSeparate_get,
    .trunk_mode_set = dal_rtl8367d_trunk_mode_set,
    .trunk_mode_get = dal_rtl8367d_trunk_mode_get,
    .trunk_trafficPause_set = dal_rtl8367d_trunk_trafficPause_set,
    .trunk_trafficPause_get = dal_rtl8367d_trunk_trafficPause_get,
    .trunk_hashMappingTable_set = dal_rtl8367d_trunk_hashMappingTable_set,
    .trunk_hashMappingTable_get = dal_rtl8367d_trunk_hashMappingTable_get,
    .trunk_portQueueEmpty_get = dal_rtl8367d_trunk_portQueueEmpty_get,

    /*leaky*/
    .leaky_vlan_set = dal_rtl8367d_leaky_vlan_set,
    .leaky_vlan_get = dal_rtl8367d_leaky_vlan_get,
    .leaky_portIsolation_set = dal_rtl8367d_leaky_portIsolation_set,
    .leaky_portIsolation_get = dal_rtl8367d_leaky_portIsolation_get,

    /*GPIO*/
    .gpio_input_get = dal_rtl8367d_gpio_input_get,
    .gpio_output_set = dal_rtl8367d_gpio_output_set,
    .gpio_output_get = dal_rtl8367d_gpio_output_get,
    .gpio_state_set = dal_rtl8367d_gpio_state_set,
    .gpio_state_get = dal_rtl8367d_gpio_state_get,
    .gpio_mode_set = dal_rtl8367d_gpio_mode_set,
    .gpio_mode_get = dal_rtl8367d_gpio_mode_get,
    .gpio_aclEnClear_set = dal_rtl8367d_gpio_aclEnClear_set,
    .gpio_aclEnClear_get = dal_rtl8367d_gpio_aclEnClear_get,

	/*ASIC*/
	.asic_setAsicReg = rtl8367d_setAsicReg,
	.asic_getAsicReg = rtl8367d_getAsicReg,
	.asic_setAsicPHYOCPReg = dal_rtl8367d_setAsicPHYOCPReg,
	.asic_getAsicPHYOCPReg = dal_rtl8367d_getAsicPHYOCPReg,
	.asic_setAsicPHYReg = dal_rtl8367d_setAsicPHYReg,
	.asic_getAsicPHYReg = dal_rtl8367d_getAsicPHYReg,

};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Module Name    :  */

/* Function Name:
 *      dal_rtl8367d_mapper_get
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
dal_mapper_t *dal_rtl8367d_mapper_get(void)
{

    return &dal_rtl8367d_mapper;
} /* end of dal_rtl8367d_mapper_get */

