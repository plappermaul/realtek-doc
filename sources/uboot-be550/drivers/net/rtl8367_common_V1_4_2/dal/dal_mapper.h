/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DAL_MAPPER_H__
#define __DAL_MAPPER_H__

/*
 * Include Files
 */
#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_switch.h>
#include <../oam.h>
#include <../cpu.h>
#include <../stat.h>
#include <../l2.h>
#include <../interrupt.h>
#include <../acl.h>
#include <../mirror.h>
#include <../port.h>
#include <../trap.h>
#include <../igmp.h>
#include <../storm.h>
#include <../rate.h>
#include <../i2c.h>
#include <../ptp.h>
#include <../qos.h>
#include <../vlan.h>
#include <../dot1x.h>
#include <../svlan.h>
#include <../rldp.h>
#include <../trunk.h>
#include <../leaky.h>
#include <../gpio.h>
#include <../led.h>
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef struct dal_mapper_s {

    /* switch */
    rtk_api_ret_t   (*switch_init)(void);
    rtk_api_ret_t   (*switch_portMaxPktLen_set)(rtk_port_t, rtk_switch_maxPktLen_linkSpeed_t, rtk_uint32);
    rtk_api_ret_t   (*switch_portMaxPktLen_get)(rtk_port_t, rtk_switch_maxPktLen_linkSpeed_t, rtk_uint32 *);
    rtk_api_ret_t   (*switch_maxPktLenCfg_set)(rtk_uint32, rtk_uint32);
    rtk_api_ret_t   (*switch_maxPktLenCfg_get)(rtk_uint32, rtk_uint32 *);
    rtk_api_ret_t   (*switch_greenEthernet_set)(rtk_enable_t);
    rtk_api_ret_t   (*switch_greenEthernet_get)(rtk_enable_t *);

    /* eee */
    rtk_api_ret_t   (*eee_init)(void);
    rtk_api_ret_t   (*eee_portEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t   (*eee_portEnable_get)(rtk_port_t, rtk_enable_t *);

    /* led */
    rtk_api_ret_t   (*led_enable_set)(rtk_led_group_t, rtk_portmask_t *);
    rtk_api_ret_t   (*led_enable_get)(rtk_led_group_t, rtk_portmask_t *);
    rtk_api_ret_t   (*led_operation_set)(rtk_led_operation_t );
    rtk_api_ret_t   (*led_operation_get)(rtk_led_operation_t *);
    rtk_api_ret_t   (*led_modeForce_set)(rtk_port_t, rtk_led_group_t, rtk_led_force_mode_t);
    rtk_api_ret_t   (*led_modeForce_get)(rtk_port_t, rtk_led_group_t, rtk_led_force_mode_t *);
    rtk_api_ret_t   (*led_blinkRate_set)(rtk_led_blink_rate_t);
    rtk_api_ret_t   (*led_blinkRate_get)(rtk_led_blink_rate_t *);
    rtk_api_ret_t   (*led_groupConfig_set)(rtk_led_group_t, rtk_led_congig_t);
    rtk_api_ret_t   (*led_groupConfig_get)(rtk_led_group_t, rtk_led_congig_t *);
    rtk_api_ret_t   (*led_groupAbility_set)(rtk_led_group_t, rtk_led_ability_t *);
    rtk_api_ret_t   (*led_groupAbility_get)(rtk_led_group_t, rtk_led_ability_t *);
    rtk_api_ret_t   (*led_serialMode_set)(rtk_led_active_t);
    rtk_api_ret_t   (*led_serialMode_get)(rtk_led_active_t *);
    rtk_api_ret_t   (*led_OutputEnable_set)(rtk_enable_t);
    rtk_api_ret_t   (*led_OutputEnable_get)(rtk_enable_t *);
    rtk_api_ret_t   (*led_serialModePortmask_set)(rtk_led_serialOutput_t, rtk_portmask_t *);
    rtk_api_ret_t   (*led_serialModePortmask_get)(rtk_led_serialOutput_t *, rtk_portmask_t *);

    /* oam */
    rtk_api_ret_t (*oam_init)(void);
    rtk_api_ret_t (*oam_state_set)(rtk_enable_t);
    rtk_api_ret_t (*oam_state_get)(rtk_enable_t *);
    rtk_api_ret_t (*oam_parserAction_set)(rtk_port_t, rtk_oam_parser_act_t );
    rtk_api_ret_t (*oam_parserAction_get)(rtk_port_t, rtk_oam_parser_act_t *);
    rtk_api_ret_t (*oam_multiplexerAction_set)(rtk_port_t, rtk_oam_multiplexer_act_t );
    rtk_api_ret_t (*oam_multiplexerAction_get)(rtk_port_t, rtk_oam_multiplexer_act_t *);

    /* cpu */
    rtk_api_ret_t (*cpu_enable_set)(rtk_enable_t);
    rtk_api_ret_t (*cpu_enable_get)(rtk_enable_t *);
    rtk_api_ret_t (*cpu_tagPort_set)(rtk_port_t, rtk_cpu_insert_t);
    rtk_api_ret_t (*cpu_tagPort_get)(rtk_port_t *, rtk_cpu_insert_t *);
    rtk_api_ret_t (*cpu_awarePort_set)(rtk_portmask_t *);
    rtk_api_ret_t (*cpu_awarePort_get)(rtk_portmask_t *);
    rtk_api_ret_t (*cpu_tagPosition_set)(rtk_cpu_position_t);
    rtk_api_ret_t (*cpu_tagPosition_get)(rtk_cpu_position_t *);
    rtk_api_ret_t (*cpu_tagLength_set)(rtk_cpu_tag_length_t);
    rtk_api_ret_t (*cpu_tagLength_get)(rtk_cpu_tag_length_t *);
    rtk_api_ret_t (*cpu_acceptLength_set)(rtk_cpu_rx_length_t);
    rtk_api_ret_t (*cpu_acceptLength_get)(rtk_cpu_rx_length_t *);
    rtk_api_ret_t (*cpu_priRemap_set)(rtk_pri_t, rtk_pri_t);
    rtk_api_ret_t (*cpu_priRemap_get)(rtk_pri_t, rtk_pri_t *);

    /* stat */
    rtk_api_ret_t (*stat_global_reset)(void);
    rtk_api_ret_t (*stat_port_reset)(rtk_port_t);
    rtk_api_ret_t (*stat_queueManage_reset)(void);
    rtk_api_ret_t (*stat_global_get)(rtk_stat_global_type_t, rtk_stat_counter_t *);
    rtk_api_ret_t (*stat_global_getAll)(rtk_stat_global_cntr_t *);
    rtk_api_ret_t (*stat_port_get)(rtk_port_t, rtk_stat_port_type_t, rtk_stat_counter_t *);
    rtk_api_ret_t (*stat_port_getAll)(rtk_port_t, rtk_stat_port_cntr_t *);
    rtk_api_ret_t (*stat_logging_counterCfg_set)(rtk_uint32, rtk_logging_counter_mode_t, rtk_logging_counter_type_t);
    rtk_api_ret_t (*stat_logging_counterCfg_get)(rtk_uint32, rtk_logging_counter_mode_t *, rtk_logging_counter_type_t *);
    rtk_api_ret_t (*stat_logging_counter_reset)(rtk_uint32);
    rtk_api_ret_t (*stat_logging_counter_get)(rtk_uint32, rtk_uint32 *);
    rtk_api_ret_t (*stat_lengthMode_set)(rtk_stat_lengthMode_t, rtk_stat_lengthMode_t);
    rtk_api_ret_t (*stat_lengthMode_get)(rtk_stat_lengthMode_t *, rtk_stat_lengthMode_t *);

    /* l2 */
    rtk_api_ret_t (*l2_init)(void);
    rtk_api_ret_t (*l2_addr_add)(rtk_mac_t *, rtk_l2_ucastAddr_t *);
    rtk_api_ret_t (*l2_addr_get)(rtk_mac_t *, rtk_l2_ucastAddr_t *);
    rtk_api_ret_t (*l2_addr_next_get)(rtk_l2_read_method_t, rtk_port_t, rtk_uint32 *, rtk_l2_ucastAddr_t *);
    rtk_api_ret_t (*l2_addr_del)(rtk_mac_t *, rtk_l2_ucastAddr_t *);
    rtk_api_ret_t (*l2_mcastAddr_add)(rtk_l2_mcastAddr_t *);
    rtk_api_ret_t (*l2_mcastAddr_get)(rtk_l2_mcastAddr_t *);
    rtk_api_ret_t (*l2_mcastAddr_next_get)(rtk_uint32 *, rtk_l2_mcastAddr_t *);
    rtk_api_ret_t (*l2_mcastAddr_del)(rtk_l2_mcastAddr_t *);
    rtk_api_ret_t (*l2_ipMcastAddr_add)(rtk_l2_ipMcastAddr_t *);
    rtk_api_ret_t (*l2_ipMcastAddr_get)(rtk_l2_ipMcastAddr_t *);
    rtk_api_ret_t (*l2_ipMcastAddr_next_get)(rtk_uint32 *, rtk_l2_ipMcastAddr_t *);
    rtk_api_ret_t (*l2_ipMcastAddr_del)(rtk_l2_ipMcastAddr_t *);
    rtk_api_ret_t (*l2_ipVidMcastAddr_add)(rtk_l2_ipVidMcastAddr_t *);
    rtk_api_ret_t (*l2_ipVidMcastAddr_get)(rtk_l2_ipVidMcastAddr_t *);
    rtk_api_ret_t (*l2_ipVidMcastAddr_next_get)(rtk_uint32 *, rtk_l2_ipVidMcastAddr_t *);
    rtk_api_ret_t (*l2_ipVidMcastAddr_del)(rtk_l2_ipVidMcastAddr_t *);
    rtk_api_ret_t (*l2_ucastAddr_flush)(rtk_l2_flushCfg_t *);
    rtk_api_ret_t (*l2_table_clear)(void);
    rtk_api_ret_t (*l2_table_clearStatus_get)(rtk_l2_clearStatus_t *);
    rtk_api_ret_t (*l2_flushLinkDownPortAddrEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*l2_flushLinkDownPortAddrEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*l2_agingEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*l2_agingEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*l2_limitLearningCnt_set)(rtk_port_t, rtk_mac_cnt_t);
    rtk_api_ret_t (*l2_limitLearningCnt_get)(rtk_port_t, rtk_mac_cnt_t *);
    rtk_api_ret_t (*l2_limitSystemLearningCnt_set)(rtk_mac_cnt_t);
    rtk_api_ret_t (*l2_limitSystemLearningCnt_get)(rtk_mac_cnt_t *);
    rtk_api_ret_t (*l2_limitLearningCntAction_set)(rtk_port_t, rtk_l2_limitLearnCntAction_t);
    rtk_api_ret_t (*l2_limitLearningCntAction_get)(rtk_port_t, rtk_l2_limitLearnCntAction_t *);
    rtk_api_ret_t (*l2_limitSystemLearningCntAction_set)(rtk_l2_limitLearnCntAction_t);
    rtk_api_ret_t (*l2_limitSystemLearningCntAction_get)(rtk_l2_limitLearnCntAction_t *);
    rtk_api_ret_t (*l2_limitSystemLearningCntPortMask_set)(rtk_portmask_t *);
    rtk_api_ret_t (*l2_limitSystemLearningCntPortMask_get)(rtk_portmask_t *);
    rtk_api_ret_t (*l2_learningCnt_get)(rtk_port_t port, rtk_mac_cnt_t *);
    rtk_api_ret_t (*l2_floodPortMask_set)(rtk_l2_flood_type_t, rtk_portmask_t *);
    rtk_api_ret_t (*l2_floodPortMask_get)(rtk_l2_flood_type_t, rtk_portmask_t *);
    rtk_api_ret_t (*l2_localPktPermit_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*l2_localPktPermit_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*l2_aging_set)(rtk_l2_age_time_t);
    rtk_api_ret_t (*l2_aging_get)(rtk_l2_age_time_t *);
    rtk_api_ret_t (*l2_ipMcastAddrLookup_set)(rtk_l2_ipmc_lookup_type_t);
    rtk_api_ret_t (*l2_ipMcastAddrLookup_get)(rtk_l2_ipmc_lookup_type_t *);
    rtk_api_ret_t (*l2_ipMcastForwardRouterPort_set)(rtk_enable_t);
    rtk_api_ret_t (*l2_ipMcastForwardRouterPort_get)(rtk_enable_t *);
    rtk_api_ret_t (*l2_ipMcastGroupEntry_add)(ipaddr_t, rtk_uint32, rtk_portmask_t *);
    rtk_api_ret_t (*l2_ipMcastGroupEntry_del)(ipaddr_t, rtk_uint32);
    rtk_api_ret_t (*l2_ipMcastGroupEntry_get)(ipaddr_t, rtk_uint32, rtk_portmask_t *);
    rtk_api_ret_t (*l2_entry_get)(rtk_l2_addr_table_t *);
    rtk_api_ret_t (*l2_lookupHitIsolationAction_set)(rtk_l2_lookupHitIsolationAction_t);
    rtk_api_ret_t (*l2_lookupHitIsolationAction_get)(rtk_l2_lookupHitIsolationAction_t *);

    /* interrupt */
    rtk_api_ret_t (*int_polarity_set)(rtk_int_polarity_t);
    rtk_api_ret_t (*int_polarity_get)(rtk_int_polarity_t *);
    rtk_api_ret_t (*int_control_set)(rtk_int_type_t, rtk_enable_t);
    rtk_api_ret_t (*int_control_get)(rtk_int_type_t, rtk_enable_t *);
    rtk_api_ret_t (*int_status_set)(rtk_int_status_t *);
    rtk_api_ret_t (*int_status_get)(rtk_int_status_t *);
    rtk_api_ret_t (*int_advanceInfo_get)(rtk_int_advType_t, rtk_int_info_t *);

    /* acl */
    rtk_api_ret_t (*filter_igrAcl_init)(void);
    rtk_api_ret_t (*filter_igrAcl_field_add)(rtk_filter_cfg_t *, rtk_filter_field_t *);
    rtk_api_ret_t (*filter_igrAcl_cfg_add)(rtk_filter_id_t, rtk_filter_cfg_t *, rtk_filter_action_t *, rtk_filter_number_t *);
    rtk_api_ret_t (*filter_igrAcl_cfg_del)(rtk_filter_id_t);
    rtk_api_ret_t (*filter_igrAcl_cfg_delAll)(void);
    rtk_api_ret_t (*filter_igrAcl_cfg_get)(rtk_filter_id_t, rtk_filter_cfg_raw_t *, rtk_filter_action_t *);
    rtk_api_ret_t (*filter_igrAcl_unmatchAction_set)(rtk_port_t, rtk_filter_unmatch_action_t);
    rtk_api_ret_t (*filter_igrAcl_unmatchAction_get)(rtk_port_t, rtk_filter_unmatch_action_t *);
    rtk_api_ret_t (*filter_igrAcl_state_set)(rtk_port_t, rtk_filter_state_t);
    rtk_api_ret_t (*filter_igrAcl_state_get)(rtk_port_t, rtk_filter_state_t *);
    rtk_api_ret_t (*filter_igrAcl_template_set)(rtk_filter_template_t *);
    rtk_api_ret_t (*filter_igrAcl_template_get)(rtk_filter_template_t *);
    rtk_api_ret_t (*filter_igrAcl_field_sel_set)(rtk_uint32, rtk_field_sel_t, rtk_uint32);
    rtk_api_ret_t (*filter_igrAcl_field_sel_get)(rtk_uint32, rtk_field_sel_t *, rtk_uint32 *);
    rtk_api_ret_t (*filter_iprange_set)(rtk_uint32, rtk_filter_iprange_t, ipaddr_t, ipaddr_t);
    rtk_api_ret_t (*filter_iprange_get)(rtk_uint32, rtk_filter_iprange_t *, ipaddr_t *, ipaddr_t *);
    rtk_api_ret_t (*filter_vidrange_set)(rtk_uint32, rtk_filter_vidrange_t, rtk_uint32, rtk_uint32);
    rtk_api_ret_t (*filter_vidrange_get)(rtk_uint32, rtk_filter_vidrange_t *, rtk_uint32 *, rtk_uint32 *);
    rtk_api_ret_t (*filter_portrange_set)(rtk_uint32, rtk_filter_portrange_t, rtk_uint32, rtk_uint32);
    rtk_api_ret_t (*filter_portrange_get)(rtk_uint32, rtk_filter_portrange_t *, rtk_uint32 *, rtk_uint32 *);
    rtk_api_ret_t (*filter_igrAclPolarity_set)(rtk_uint32);
    rtk_api_ret_t (*filter_igrAclPolarity_get)(rtk_uint32 *);

    /* mirror */
    rtk_api_ret_t (*mirror_portBased_set)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    rtk_api_ret_t (*mirror_portBased_get)(rtk_port_t *, rtk_portmask_t *, rtk_portmask_t *);
    rtk_api_ret_t (*mirror_portIso_set)(rtk_enable_t);
    rtk_api_ret_t (*mirror_portIso_get)(rtk_enable_t *);
    rtk_api_ret_t (*mirror_vlanLeaky_set)(rtk_enable_t , rtk_enable_t);
    rtk_api_ret_t (*mirror_vlanLeaky_get)(rtk_enable_t *, rtk_enable_t *);
    rtk_api_ret_t (*mirror_isolationLeaky_set)(rtk_enable_t, rtk_enable_t );
    rtk_api_ret_t (*mirror_isolationLeaky_get)(rtk_enable_t *, rtk_enable_t *);
    rtk_api_ret_t (*mirror_keep_set)(rtk_mirror_keep_t);
    rtk_api_ret_t (*mirror_keep_get)(rtk_mirror_keep_t *);
    rtk_api_ret_t (*mirror_override_set)(rtk_enable_t, rtk_enable_t, rtk_enable_t);
    rtk_api_ret_t (*mirror_override_get)(rtk_enable_t *, rtk_enable_t *, rtk_enable_t *);

    /* port */
    rtk_api_ret_t (*port_phyAutoNegoAbility_set)(rtk_port_t, rtk_port_phy_ability_t *);
    rtk_api_ret_t (*port_phyAutoNegoAbility_get)(rtk_port_t, rtk_port_phy_ability_t *);
    rtk_api_ret_t (*port_phyForceModeAbility_set)(rtk_port_t, rtk_port_phy_ability_t *);
    rtk_api_ret_t (*port_phyForceModeAbility_get)(rtk_port_t, rtk_port_phy_ability_t *);
    rtk_api_ret_t (*port_phyStatus_get)(rtk_port_t, rtk_port_linkStatus_t *, rtk_port_speed_t *, rtk_port_duplex_t *);
    rtk_api_ret_t (*port_macForceLink_set)(rtk_port_t, rtk_port_mac_ability_t *);
    rtk_api_ret_t (*port_macForceLink_get)(rtk_port_t, rtk_port_mac_ability_t *);
    rtk_api_ret_t (*port_macForceLinkExt_set)(rtk_port_t, rtk_mode_ext_t, rtk_port_mac_ability_t *);
    rtk_api_ret_t (*port_macForceLinkExt_get)(rtk_port_t, rtk_mode_ext_t *, rtk_port_mac_ability_t *);
    rtk_api_ret_t (*port_macStatus_get)(rtk_port_t, rtk_port_mac_ability_t *);
    rtk_api_ret_t (*port_macLocalLoopbackEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*port_macLocalLoopbackEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*port_phyReg_set)(rtk_port_t, rtk_port_phy_reg_t, rtk_port_phy_data_t);
    rtk_api_ret_t (*port_phyReg_get)(rtk_port_t, rtk_port_phy_reg_t, rtk_port_phy_data_t *);
    rtk_api_ret_t (*port_phyOCPReg_set)(rtk_port_t, rtk_uint32, rtk_uint32);
    rtk_api_ret_t (*port_phyOCPReg_get)(rtk_port_t, rtk_uint32, rtk_uint32 *);
    rtk_api_ret_t (*port_backpressureEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*port_backpressureEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*port_adminEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*port_adminEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*port_isolation_set)(rtk_port_t, rtk_portmask_t *);
    rtk_api_ret_t (*port_isolation_get)(rtk_port_t, rtk_portmask_t *);
    rtk_api_ret_t (*port_rgmiiDelayExt_set)(rtk_port_t, rtk_data_t, rtk_data_t);
    rtk_api_ret_t (*port_rgmiiDelayExt_get)(rtk_port_t, rtk_data_t *, rtk_data_t *);
    rtk_api_ret_t (*port_phyEnableAll_set)(rtk_enable_t);
    rtk_api_ret_t (*port_phyEnableAll_get)(rtk_enable_t *);
    rtk_api_ret_t (*port_efid_set)(rtk_port_t, rtk_data_t);
    rtk_api_ret_t (*port_efid_get)(rtk_port_t, rtk_data_t *);
    rtk_api_ret_t (*port_phyComboPortMedia_set)(rtk_port_t, rtk_port_media_t);
    rtk_api_ret_t (*port_phyComboPortMedia_get)(rtk_port_t, rtk_port_media_t *);
    rtk_api_ret_t (*port_rtctEnable_set)(rtk_portmask_t *);
    rtk_api_ret_t (*port_rtctDisable_set)(rtk_portmask_t *);
    rtk_api_ret_t (*port_rtctResult_get)(rtk_port_t, rtk_rtctResult_t *);
    rtk_api_ret_t (*port_sds_reset)(rtk_port_t);
    rtk_api_ret_t (*port_sgmiiLinkStatus_get)(rtk_port_t, rtk_data_t *, rtk_data_t *, rtk_port_linkStatus_t *);
    rtk_api_ret_t (*port_sgmiiNway_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*port_sgmiiNway_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*port_fiberAbilityExt_set)(rtk_port_t, rtk_uint32, rtk_uint32);
    rtk_api_ret_t (*port_fiberAbilityExt_get)(rtk_port_t, rtk_uint32 *, rtk_uint32 *);
    rtk_api_ret_t (*port_autoDos_set)(rtk_port_autoDosType_t, rtk_enable_t);
    rtk_api_ret_t (*port_autoDos_get)(rtk_port_autoDosType_t, rtk_enable_t *);
    rtk_api_ret_t (*port_fiberAbility_set)(rtk_port_t, rtk_port_fiber_ability_t *);
    rtk_api_ret_t (*port_fiberAbility_get)(rtk_port_t, rtk_port_fiber_ability_t *);
    rtk_api_ret_t (*port_phyMdx_set)(rtk_port_t, rtk_port_phy_mdix_mode_t);
    rtk_api_ret_t (*port_phyMdx_get)(rtk_port_t, rtk_port_phy_mdix_mode_t *);
    rtk_api_ret_t (*port_phyMdxStatus_get)(rtk_port_t, rtk_port_phy_mdix_status_t *);
    rtk_api_ret_t (*port_phyTestMode_set)(rtk_port_t, rtk_port_phy_test_mode_t);
    rtk_api_ret_t (*port_phyTestMode_get)(rtk_port_t, rtk_port_phy_test_mode_t *);

    /* trap */
    rtk_api_ret_t (*trap_unknownUnicastPktAction_set)(rtk_port_t, rtk_trap_ucast_action_t);
    rtk_api_ret_t (*trap_unknownUnicastPktAction_get)(rtk_port_t, rtk_trap_ucast_action_t *);
    rtk_api_ret_t (*trap_unknownMacPktAction_set)(rtk_trap_ucast_action_t);
    rtk_api_ret_t (*trap_unknownMacPktAction_get)(rtk_trap_ucast_action_t *);
    rtk_api_ret_t (*trap_unmatchMacPktAction_set)(rtk_trap_ucast_action_t);
    rtk_api_ret_t (*trap_unmatchMacPktAction_get)(rtk_trap_ucast_action_t *);
    rtk_api_ret_t (*trap_unmatchMacMoving_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*trap_unmatchMacMoving_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*trap_unknownMcastPktAction_set)(rtk_port_t, rtk_mcast_type_t, rtk_trap_mcast_action_t);
    rtk_api_ret_t (*trap_unknownMcastPktAction_get)(rtk_port_t, rtk_mcast_type_t, rtk_trap_mcast_action_t *);
    rtk_api_ret_t (*trap_lldpEnable_set)(rtk_enable_t);
    rtk_api_ret_t (*trap_lldpEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*trap_reasonTrapToCpuPriority_set)(rtk_trap_reason_type_t, rtk_pri_t);
    rtk_api_ret_t (*trap_reasonTrapToCpuPriority_get)(rtk_trap_reason_type_t, rtk_pri_t *);
    rtk_api_ret_t (*trap_rmaAction_set)(rtk_trap_type_t, rtk_trap_rma_action_t);
    rtk_api_ret_t (*trap_rmaAction_get)(rtk_trap_type_t, rtk_trap_rma_action_t *);
    rtk_api_ret_t (*trap_rmaKeepFormat_set)(rtk_trap_type_t, rtk_enable_t);
    rtk_api_ret_t (*trap_rmaKeepFormat_get)(rtk_trap_type_t, rtk_enable_t *);
    rtk_api_ret_t (*trap_portUnknownMacPktAction_set)(rtk_port_t, rtk_trap_ucast_action_t);
    rtk_api_ret_t (*trap_portUnknownMacPktAction_get)(rtk_port_t, rtk_trap_ucast_action_t *);
    rtk_api_ret_t (*trap_portUnmatchMacPktAction_set)(rtk_port_t, rtk_trap_ucast_action_t);
    rtk_api_ret_t (*trap_portUnmatchMacPktAction_get)(rtk_port_t, rtk_trap_ucast_action_t *);

    /* IGMP */
    rtk_api_ret_t (*igmp_init)(void);
    rtk_api_ret_t (*igmp_state_set)(rtk_enable_t);
    rtk_api_ret_t (*igmp_state_get)(rtk_enable_t *);
    rtk_api_ret_t (*igmp_static_router_port_set)(rtk_portmask_t *);
    rtk_api_ret_t (*igmp_static_router_port_get)(rtk_portmask_t *);
    rtk_api_ret_t (*igmp_protocol_set)(rtk_port_t, rtk_igmp_protocol_t, rtk_igmp_action_t);
    rtk_api_ret_t (*igmp_protocol_get)(rtk_port_t, rtk_igmp_protocol_t, rtk_igmp_action_t *);
    rtk_api_ret_t (*igmp_fastLeave_set)(rtk_enable_t);
    rtk_api_ret_t (*igmp_fastLeave_get)(rtk_enable_t *);
    rtk_api_ret_t (*igmp_maxGroup_set)(rtk_port_t, rtk_uint32);
    rtk_api_ret_t (*igmp_maxGroup_get)(rtk_port_t, rtk_uint32 *);
    rtk_api_ret_t (*igmp_currentGroup_get)(rtk_port_t, rtk_uint32 *);
    rtk_api_ret_t (*igmp_tableFullAction_set)(rtk_igmp_tableFullAction_t);
    rtk_api_ret_t (*igmp_tableFullAction_get)(rtk_igmp_tableFullAction_t *);
    rtk_api_ret_t (*igmp_checksumErrorAction_set)(rtk_igmp_checksumErrorAction_t);
    rtk_api_ret_t (*igmp_checksumErrorAction_get)(rtk_igmp_checksumErrorAction_t *);
    rtk_api_ret_t (*igmp_leaveTimer_set)(rtk_uint32);
    rtk_api_ret_t (*igmp_leaveTimer_get)(rtk_uint32 *);
    rtk_api_ret_t (*igmp_queryInterval_set)(rtk_uint32);
    rtk_api_ret_t (*igmp_queryInterval_get)(rtk_uint32 *);
    rtk_api_ret_t (*igmp_robustness_set)(rtk_uint32);
    rtk_api_ret_t (*igmp_robustness_get)(rtk_uint32 *);
    rtk_api_ret_t (*igmp_dynamicRouterPortAllow_set)(rtk_portmask_t *);
    rtk_api_ret_t (*igmp_dynamicRouterPortAllow_get)(rtk_portmask_t *);
    rtk_api_ret_t (*igmp_dynamicRouterPort_get)(rtk_igmp_dynamicRouterPort_t *);
    rtk_api_ret_t (*igmp_suppressionEnable_set)(rtk_enable_t, rtk_enable_t);
    rtk_api_ret_t (*igmp_suppressionEnable_get)(rtk_enable_t *, rtk_enable_t *);
    rtk_api_ret_t (*igmp_portRxPktEnable_set)(rtk_port_t, rtk_igmp_rxPktEnable_t *);
    rtk_api_ret_t (*igmp_portRxPktEnable_get)(rtk_port_t, rtk_igmp_rxPktEnable_t *);
    rtk_api_ret_t (*igmp_groupInfo_get)(rtk_uint32, rtk_igmp_groupInfo_t *);
    rtk_api_ret_t (*igmp_ReportLeaveFwdAction_set)(rtk_igmp_ReportLeaveFwdAct_t);
    rtk_api_ret_t (*igmp_ReportLeaveFwdAction_get)(rtk_igmp_ReportLeaveFwdAct_t *);
    rtk_api_ret_t (*igmp_dropLeaveZeroEnable_set)(rtk_enable_t);
    rtk_api_ret_t (*igmp_dropLeaveZeroEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*igmp_bypassGroupRange_set)(rtk_igmp_bypassGroup_t, rtk_enable_t);
    rtk_api_ret_t (*igmp_bypassGroupRange_get)(rtk_igmp_bypassGroup_t, rtk_enable_t *);

    /* Storm */
    rtk_api_ret_t (*rate_stormControlMeterIdx_set)(rtk_port_t, rtk_rate_storm_group_t, rtk_uint32);
    rtk_api_ret_t (*rate_stormControlMeterIdx_get)(rtk_port_t, rtk_rate_storm_group_t, rtk_uint32 *);
    rtk_api_ret_t (*rate_stormControlPortEnable_set)(rtk_port_t, rtk_rate_storm_group_t, rtk_enable_t);
    rtk_api_ret_t (*rate_stormControlPortEnable_get)(rtk_port_t, rtk_rate_storm_group_t, rtk_enable_t *);
    rtk_api_ret_t (*storm_bypass_set)(rtk_storm_bypass_t, rtk_enable_t);
    rtk_api_ret_t (*storm_bypass_get)(rtk_storm_bypass_t, rtk_enable_t *);
    rtk_api_ret_t (*rate_stormControlExtPortmask_set)(rtk_portmask_t *);
    rtk_api_ret_t (*rate_stormControlExtPortmask_get)(rtk_portmask_t *);
    rtk_api_ret_t (*rate_stormControlExtEnable_set)(rtk_rate_storm_group_t, rtk_enable_t);
    rtk_api_ret_t (*rate_stormControlExtEnable_get)(rtk_rate_storm_group_t, rtk_enable_t *);
    rtk_api_ret_t (*rate_stormControlExtMeterIdx_set)(rtk_rate_storm_group_t, rtk_uint32);
    rtk_api_ret_t (*rate_stormControlExtMeterIdx_get)(rtk_rate_storm_group_t, rtk_uint32 *);

    /* Rate */
    rtk_api_ret_t (*rate_shareMeter_set)(rtk_meter_id_t, rtk_meter_type_t, rtk_rate_t, rtk_enable_t);
    rtk_api_ret_t (*rate_shareMeter_get)(rtk_meter_id_t, rtk_meter_type_t *, rtk_rate_t *, rtk_enable_t *);
    rtk_api_ret_t (*rate_shareMeterBucket_set)(rtk_meter_id_t, rtk_uint32);
    rtk_api_ret_t (*rate_shareMeterBucket_get)(rtk_meter_id_t, rtk_uint32 *);
    rtk_api_ret_t (*rate_igrBandwidthCtrlRate_set)(rtk_port_t, rtk_rate_t,  rtk_enable_t, rtk_enable_t);
    rtk_api_ret_t (*rate_igrBandwidthCtrlRate_get)(rtk_port_t, rtk_rate_t *, rtk_enable_t *, rtk_enable_t *);
    rtk_api_ret_t (*rate_egrBandwidthCtrlRate_set)(rtk_port_t, rtk_rate_t,  rtk_enable_t);
    rtk_api_ret_t (*rate_egrBandwidthCtrlRate_get)(rtk_port_t, rtk_rate_t *, rtk_enable_t *);
    rtk_api_ret_t (*rate_egrQueueBwCtrlEnable_set)(rtk_port_t, rtk_qid_t, rtk_enable_t);
    rtk_api_ret_t (*rate_egrQueueBwCtrlEnable_get)(rtk_port_t, rtk_qid_t, rtk_enable_t *);
    rtk_api_ret_t (*rate_egrQueueBwCtrlRate_set)(rtk_port_t, rtk_qid_t, rtk_meter_id_t);
    rtk_api_ret_t (*rate_egrQueueBwCtrlRate_get)(rtk_port_t, rtk_qid_t, rtk_meter_id_t *);

    /* I2C */
    rtk_api_ret_t (*i2c_init)(void);
    rtk_api_ret_t (*i2c_data_read)(rtk_uint8, rtk_uint32, rtk_uint32 *);
    rtk_api_ret_t (*i2c_data_write)(rtk_uint8, rtk_uint32, rtk_uint32);
    rtk_api_ret_t (*i2c_mode_set)(rtk_I2C_16bit_mode_t );
    rtk_api_ret_t (*i2c_mode_get)(rtk_I2C_16bit_mode_t *);
    rtk_api_ret_t (*i2c_gpioPinGroup_set)(rtk_I2C_gpio_pin_t);
    rtk_api_ret_t (*i2c_gpioPinGroup_get)(rtk_I2C_gpio_pin_t *);

    /*PTP*/
    rtk_api_ret_t (*ptp_init)(void);
    rtk_api_ret_t (*ptp_mac_set)(rtk_mac_t );
    rtk_api_ret_t (*ptp_mac_get)(rtk_mac_t *);
    rtk_api_ret_t (*ptp_tpid_set)(rtk_ptp_tpid_t, rtk_ptp_tpid_t);
    rtk_api_ret_t (*ptp_tpid_get)(rtk_ptp_tpid_t *, rtk_ptp_tpid_t *);
    rtk_api_ret_t (*ptp_refTime_set)(rtk_ptp_timeStamp_t );
    rtk_api_ret_t (*ptp_refTime_get)(rtk_ptp_timeStamp_t *);
    rtk_api_ret_t (*ptp_refTimeAdjust_set)(rtk_ptp_sys_adjust_t, rtk_ptp_timeStamp_t);
    rtk_api_ret_t (*ptp_refTimeEnable_set)(rtk_enable_t);
    rtk_api_ret_t (*ptp_refTimeEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*ptp_portEnable_set)(rtk_port_t, rtk_enable_t );
    rtk_api_ret_t (*ptp_portEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*ptp_portTimestamp_get)( rtk_port_t, rtk_ptp_msgType_t, rtk_ptp_info_t *);
    rtk_api_ret_t (*ptp_intControl_set)(rtk_ptp_intType_t, rtk_enable_t);
    rtk_api_ret_t (*ptp_intControl_get)(rtk_ptp_intType_t, rtk_enable_t *);
    rtk_api_ret_t (*ptp_intStatus_get)(rtk_ptp_intStatus_t *);
    rtk_api_ret_t (*ptp_portIntStatus_set)(rtk_port_t, rtk_ptp_intStatus_t );
    rtk_api_ret_t (*ptp_portIntStatus_get)(rtk_port_t, rtk_ptp_intStatus_t *);
    rtk_api_ret_t (*ptp_portTrap_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*ptp_portTrap_get)(rtk_port_t, rtk_enable_t *);

    /*QoS*/
    rtk_api_ret_t (*qos_init)(rtk_queue_num_t);
    rtk_api_ret_t (*qos_priSel_set)(rtk_qos_priDecTbl_t, rtk_priority_select_t *);
    rtk_api_ret_t (*qos_priSel_get)(rtk_qos_priDecTbl_t, rtk_priority_select_t *);
    rtk_api_ret_t (*qos_1pPriRemap_set)(rtk_pri_t, rtk_pri_t);
    rtk_api_ret_t (*qos_1pPriRemap_get)(rtk_pri_t, rtk_pri_t *);
    rtk_api_ret_t (*qos_1pRemarkSrcSel_set)(rtk_qos_1pRmkSrc_t );
    rtk_api_ret_t (*qos_1pRemarkSrcSel_get)(rtk_qos_1pRmkSrc_t *);
    rtk_api_ret_t (*qos_dscpPriRemap_set)(rtk_dscp_t, rtk_pri_t );
    rtk_api_ret_t (*qos_dscpPriRemap_get)(rtk_dscp_t, rtk_pri_t *);
    rtk_api_ret_t (*qos_portPri_set)(rtk_port_t, rtk_pri_t ) ;
    rtk_api_ret_t (*qos_portPri_get)(rtk_port_t, rtk_pri_t *) ;
    rtk_api_ret_t (*qos_queueNum_set)(rtk_port_t, rtk_queue_num_t);
    rtk_api_ret_t (*qos_queueNum_get)(rtk_port_t, rtk_queue_num_t *);
    rtk_api_ret_t (*qos_priMap_set)(rtk_queue_num_t, rtk_qos_pri2queue_t *);
    rtk_api_ret_t (*qos_priMap_get)(rtk_queue_num_t, rtk_qos_pri2queue_t *);
    rtk_api_ret_t (*qos_schedulingQueue_set)(rtk_port_t, rtk_qos_queue_weights_t *);
    rtk_api_ret_t (*qos_schedulingQueue_get)(rtk_port_t, rtk_qos_queue_weights_t *);
    rtk_api_ret_t (*qos_1pRemarkEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*qos_1pRemarkEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*qos_1pRemark_set)(rtk_pri_t, rtk_pri_t);
    rtk_api_ret_t (*qos_1pRemark_get)(rtk_pri_t, rtk_pri_t *);
    rtk_api_ret_t (*qos_dscpRemarkEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*qos_dscpRemarkEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*qos_dscpRemark_set)(rtk_pri_t, rtk_dscp_t);
    rtk_api_ret_t (*qos_dscpRemark_get)(rtk_pri_t, rtk_dscp_t *);
    rtk_api_ret_t (*qos_dscpRemarkSrcSel_set)(rtk_qos_dscpRmkSrc_t);
    rtk_api_ret_t (*qos_dscpRemarkSrcSel_get)(rtk_qos_dscpRmkSrc_t *);
    rtk_api_ret_t (*qos_dscpRemark2Dscp_set)(rtk_dscp_t, rtk_dscp_t);
    rtk_api_ret_t (*qos_dscpRemark2Dscp_get)(rtk_dscp_t, rtk_dscp_t *);
    rtk_api_ret_t (*qos_portPriSelIndex_set)(rtk_port_t, rtk_qos_priDecTbl_t);
    rtk_api_ret_t (*qos_portPriSelIndex_get)(rtk_port_t, rtk_qos_priDecTbl_t *);
    rtk_api_ret_t (*qos_schedulingType_set)(rtk_qos_scheduling_type_t);
    rtk_api_ret_t (*qos_schedulingType_get)(rtk_qos_scheduling_type_t *);

    /*VLAN*/
    rtk_api_ret_t (*vlan_init)(void);
    rtk_api_ret_t (*vlan_set)(rtk_vlan_t, rtk_vlan_cfg_t *);
    rtk_api_ret_t (*vlan_get)(rtk_vlan_t, rtk_vlan_cfg_t *);
    rtk_api_ret_t (*vlan_egrFilterEnable_set)(rtk_enable_t);
    rtk_api_ret_t (*vlan_egrFilterEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*vlan_mbrCfg_set)(rtk_uint32, rtk_vlan_mbrcfg_t *);
    rtk_api_ret_t (*vlan_mbrCfg_get)(rtk_uint32, rtk_vlan_mbrcfg_t *);
    rtk_api_ret_t (*vlan_portPvid_set)(rtk_port_t, rtk_vlan_t, rtk_pri_t);
    rtk_api_ret_t (*vlan_portPvid_get)(rtk_port_t, rtk_vlan_t *, rtk_pri_t *);
    rtk_api_ret_t (*vlan_portIgrFilterEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*vlan_portIgrFilterEnable_get)(rtk_port_t, rtk_enable_t *);
    rtk_api_ret_t (*vlan_portAcceptFrameType_set)(rtk_port_t, rtk_vlan_acceptFrameType_t);
    rtk_api_ret_t (*vlan_portAcceptFrameType_get)(rtk_port_t, rtk_vlan_acceptFrameType_t *);
    rtk_api_ret_t (*vlan_tagMode_set)(rtk_port_t, rtk_vlan_tagMode_t);
    rtk_api_ret_t (*vlan_tagMode_get)(rtk_port_t, rtk_vlan_tagMode_t *);
    rtk_api_ret_t (*vlan_transparent_set)(rtk_port_t, rtk_portmask_t *);
    rtk_api_ret_t (*vlan_transparent_get)(rtk_port_t , rtk_portmask_t *);
    rtk_api_ret_t (*vlan_keep_set)(rtk_port_t, rtk_portmask_t *);
    rtk_api_ret_t (*vlan_keep_get)(rtk_port_t, rtk_portmask_t *);
    rtk_api_ret_t (*vlan_stg_set)(rtk_vlan_t, rtk_stp_msti_id_t);
    rtk_api_ret_t (*vlan_stg_get)(rtk_vlan_t, rtk_stp_msti_id_t *);
    rtk_api_ret_t (*vlan_protoAndPortBasedVlan_add)(rtk_port_t, rtk_vlan_protoAndPortInfo_t *);
    rtk_api_ret_t (*vlan_protoAndPortBasedVlan_get)(rtk_port_t , rtk_vlan_proto_type_t, rtk_vlan_protoVlan_frameType_t, rtk_vlan_protoAndPortInfo_t *);
    rtk_api_ret_t (*vlan_protoAndPortBasedVlan_del)(rtk_port_t , rtk_vlan_proto_type_t, rtk_vlan_protoVlan_frameType_t);
    rtk_api_ret_t (*vlan_protoAndPortBasedVlan_delAll)(rtk_port_t);
    rtk_api_ret_t (*vlan_portFid_set)(rtk_port_t port, rtk_enable_t, rtk_fid_t);
    rtk_api_ret_t (*vlan_portFid_get)(rtk_port_t port, rtk_enable_t *, rtk_fid_t *);
    rtk_api_ret_t (*vlan_UntagDscpPriorityEnable_set)(rtk_enable_t);
    rtk_api_ret_t (*vlan_UntagDscpPriorityEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*stp_mstpState_set)(rtk_stp_msti_id_t, rtk_port_t, rtk_stp_state_t);
    rtk_api_ret_t (*stp_mstpState_get)(rtk_stp_msti_id_t, rtk_port_t, rtk_stp_state_t *);
    rtk_api_ret_t (*vlan_reservedVidAction_set)(rtk_vlan_resVidAction_t, rtk_vlan_resVidAction_t);
    rtk_api_ret_t (*vlan_reservedVidAction_get)(rtk_vlan_resVidAction_t *, rtk_vlan_resVidAction_t *);
    rtk_api_ret_t (*vlan_realKeepRemarkEnable_set)(rtk_enable_t );
    rtk_api_ret_t (*vlan_realKeepRemarkEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*vlan_reset)(void);

    /*dot1x*/
    rtk_api_ret_t (*dot1x_unauthPacketOper_set)(rtk_port_t, rtk_dot1x_unauth_action_t);
    rtk_api_ret_t (*dot1x_unauthPacketOper_get)(rtk_port_t, rtk_dot1x_unauth_action_t *);
    rtk_api_ret_t (*dot1x_eapolFrame2CpuEnable_set)(rtk_enable_t);
    rtk_api_ret_t (*dot1x_eapolFrame2CpuEnable_get)(rtk_enable_t *);
    rtk_api_ret_t (*dot1x_portBasedEnable_set)(rtk_port_t port, rtk_enable_t);
    rtk_api_ret_t (*dot1x_portBasedEnable_get)(rtk_port_t port, rtk_enable_t *);
    rtk_api_ret_t (*dot1x_portBasedAuthStatus_set)(rtk_port_t, rtk_dot1x_auth_status_t);
    rtk_api_ret_t (*dot1x_portBasedAuthStatus_get)(rtk_port_t, rtk_dot1x_auth_status_t *);
    rtk_api_ret_t (*dot1x_portBasedDirection_set)(rtk_port_t, rtk_dot1x_direction_t);
    rtk_api_ret_t (*dot1x_portBasedDirection_get)(rtk_port_t, rtk_dot1x_direction_t *);
    rtk_api_ret_t (*dot1x_macBasedEnable_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*dot1x_macBasedEnable_get)(rtk_port_t , rtk_enable_t *);
    rtk_api_ret_t (*dot1x_macBasedAuthMac_add)(rtk_port_t, rtk_mac_t *, rtk_fid_t);
    rtk_api_ret_t (*dot1x_macBasedAuthMac_del)(rtk_port_t, rtk_mac_t *, rtk_fid_t);
    rtk_api_ret_t (*dot1x_macBasedDirection_set)(rtk_dot1x_direction_t);
    rtk_api_ret_t (*dot1x_macBasedDirection_get)(rtk_dot1x_direction_t *);
    rtk_api_ret_t (*dot1x_guestVlan_set)(rtk_vlan_t );
    rtk_api_ret_t (*dot1x_guestVlan_get)(rtk_vlan_t *);
    rtk_api_ret_t (*dot1x_guestVlan2Auth_set)(rtk_enable_t);
    rtk_api_ret_t (*dot1x_guestVlan2Auth_get)(rtk_enable_t *);

    /*SVLAN*/
    rtk_api_ret_t (*svlan_init)(void);
    rtk_api_ret_t (*svlan_servicePort_add)(rtk_port_t port);
    rtk_api_ret_t (*svlan_servicePort_get)(rtk_portmask_t *);
    rtk_api_ret_t (*svlan_servicePort_del)(rtk_port_t);
    rtk_api_ret_t (*svlan_tpidEntry_set)(rtk_uint32);
    rtk_api_ret_t (*svlan_tpidEntry_get)(rtk_uint32 *);
    rtk_api_ret_t (*svlan_priorityRef_set)(rtk_svlan_pri_ref_t);
    rtk_api_ret_t (*svlan_priorityRef_get)(rtk_svlan_pri_ref_t *);
    rtk_api_ret_t (*svlan_memberPortEntry_set)(rtk_uint32, rtk_svlan_memberCfg_t *);
    rtk_api_ret_t (*svlan_memberPortEntry_get)(rtk_uint32, rtk_svlan_memberCfg_t *);
    rtk_api_ret_t (*svlan_memberPortEntry_adv_set)(rtk_uint32, rtk_svlan_memberCfg_t *);
    rtk_api_ret_t (*svlan_memberPortEntry_adv_get)(rtk_uint32, rtk_svlan_memberCfg_t *);
    rtk_api_ret_t (*svlan_defaultSvlan_set)(rtk_port_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_defaultSvlan_get)(rtk_port_t, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_c2s_add)(rtk_vlan_t, rtk_port_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_c2s_del)(rtk_vlan_t, rtk_port_t);
    rtk_api_ret_t (*svlan_c2s_get)(rtk_vlan_t, rtk_port_t, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_untag_action_set)(rtk_svlan_untag_action_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_untag_action_get)(rtk_svlan_untag_action_t *, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_unmatch_action_set)(rtk_svlan_unmatch_action_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_unmatch_action_get)(rtk_svlan_unmatch_action_t *, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_dmac_vidsel_set)(rtk_port_t, rtk_enable_t);
    rtk_api_ret_t (*svlan_dmac_vidsel_get)(rtk_port_t , rtk_enable_t *);
    rtk_api_ret_t (*svlan_ipmc2s_add)(ipaddr_t, ipaddr_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_ipmc2s_del)(ipaddr_t, ipaddr_t);
    rtk_api_ret_t (*svlan_ipmc2s_get)(ipaddr_t, ipaddr_t, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_l2mc2s_add)(rtk_mac_t, rtk_mac_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_l2mc2s_del)(rtk_mac_t, rtk_mac_t);
    rtk_api_ret_t (*svlan_l2mc2s_get)(rtk_mac_t, rtk_mac_t, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_sp2c_add)(rtk_vlan_t, rtk_port_t, rtk_vlan_t);
    rtk_api_ret_t (*svlan_sp2c_get)(rtk_vlan_t, rtk_port_t, rtk_vlan_t *);
    rtk_api_ret_t (*svlan_sp2c_del)(rtk_vlan_t, rtk_port_t);
    rtk_api_ret_t (*svlan_lookupType_set)(rtk_svlan_lookupType_t);
    rtk_api_ret_t (*svlan_lookupType_get)(rtk_svlan_lookupType_t *);
    rtk_api_ret_t (*svlan_trapPri_set)(rtk_pri_t);
    rtk_api_ret_t (*svlan_trapPri_get)(rtk_pri_t *);
    rtk_api_ret_t (*svlan_unassign_action_set)(rtk_svlan_unassign_action_t);
    rtk_api_ret_t (*svlan_unassign_action_get)(rtk_svlan_unassign_action_t *);

    /*RLDP*/
    rtk_api_ret_t (*rldp_config_set)(rtk_rldp_config_t *);
    rtk_api_ret_t (*rldp_config_get)(rtk_rldp_config_t *);
    rtk_api_ret_t (*rldp_portConfig_set)(rtk_port_t, rtk_rldp_portConfig_t *);
    rtk_api_ret_t (*rldp_portConfig_get)(rtk_port_t, rtk_rldp_portConfig_t *);
    rtk_api_ret_t (*rldp_status_get)(rtk_rldp_status_t *);
    rtk_api_ret_t (*rldp_portStatus_get)(rtk_port_t, rtk_rldp_portStatus_t *);
    rtk_api_ret_t (*rldp_portStatus_set)(rtk_port_t, rtk_rldp_portStatus_t *);
    rtk_api_ret_t (*rldp_portLoopPair_get)(rtk_port_t, rtk_portmask_t *);

    /*trunk*/
    rtk_api_ret_t (*trunk_port_set)(rtk_trunk_group_t, rtk_portmask_t *);
    rtk_api_ret_t (*trunk_port_get)(rtk_trunk_group_t, rtk_portmask_t *);
    rtk_api_ret_t (*trunk_distributionAlgorithm_set)(rtk_trunk_group_t, rtk_uint32);
    rtk_api_ret_t (*trunk_distributionAlgorithm_get)(rtk_trunk_group_t, rtk_uint32 *);
    rtk_api_ret_t (*trunk_trafficSeparate_set)(rtk_trunk_group_t, rtk_trunk_separateType_t);
    rtk_api_ret_t (*trunk_trafficSeparate_get)(rtk_trunk_group_t, rtk_trunk_separateType_t *);
    rtk_api_ret_t (*trunk_mode_set)(rtk_trunk_mode_t);
    rtk_api_ret_t (*trunk_mode_get)(rtk_trunk_mode_t *);
    rtk_api_ret_t (*trunk_trafficPause_set)(rtk_trunk_group_t, rtk_enable_t);
    rtk_api_ret_t (*trunk_trafficPause_get)(rtk_trunk_group_t, rtk_enable_t *);
    rtk_api_ret_t (*trunk_hashMappingTable_set)(rtk_trunk_group_t, rtk_trunk_hashVal2Port_t *);
    rtk_api_ret_t (*trunk_hashMappingTable_get)(rtk_trunk_group_t, rtk_trunk_hashVal2Port_t *);
    rtk_api_ret_t (*trunk_portQueueEmpty_get)(rtk_portmask_t *);

    /*leaky*/
    rtk_api_ret_t (*leaky_vlan_set)(rtk_leaky_type_t, rtk_enable_t);
    rtk_api_ret_t (*leaky_vlan_get)(rtk_leaky_type_t, rtk_enable_t *);
    rtk_api_ret_t (*leaky_portIsolation_set)(rtk_leaky_type_t, rtk_enable_t);
    rtk_api_ret_t (*leaky_portIsolation_get)(rtk_leaky_type_t, rtk_enable_t *);

    /*GPIO*/
    rtk_api_ret_t (*gpio_input_get)(rtk_uint32, rtk_uint32 *);
    rtk_api_ret_t (*gpio_output_set)(rtk_uint32, rtk_uint32);
    rtk_api_ret_t (*gpio_output_get)(rtk_uint32, rtk_uint32 *);
    rtk_api_ret_t (*gpio_state_set)(rtk_uint32, rtk_enable_t);
    rtk_api_ret_t (*gpio_state_get)(rtk_uint32, rtk_enable_t *);
    rtk_api_ret_t (*gpio_mode_set)(rtk_uint32, rtk_gpio_mode_t);
    rtk_api_ret_t (*gpio_mode_get)(rtk_uint32, rtk_gpio_mode_t *);
    rtk_api_ret_t (*gpio_aclEnClear_set)(rtk_uint32);
    rtk_api_ret_t (*gpio_aclEnClear_get)(rtk_uint32, rtk_enable_t *);

	/*ASIC*/
	rtk_api_ret_t (*asic_setAsicReg)(rtk_uint32, rtk_uint32);
	rtk_api_ret_t (*asic_getAsicReg)(rtk_uint32, rtk_uint32 *);
	rtk_api_ret_t (*asic_setAsicPHYOCPReg)(rtk_uint32, rtk_uint32, rtk_uint32);
	rtk_api_ret_t (*asic_getAsicPHYOCPReg)(rtk_uint32, rtk_uint32, rtk_uint32 *);
	rtk_api_ret_t (*asic_setAsicPHYReg)(rtk_uint32, rtk_uint32, rtk_uint32);
	rtk_api_ret_t (*asic_getAsicPHYReg)(rtk_uint32, rtk_uint32, rtk_uint32 *);

} dal_mapper_t;


#endif /* __DAL_MAPPER_H __ */
