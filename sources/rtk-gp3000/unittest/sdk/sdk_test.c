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
 *
 * Purpose : Definition of SDK test APIs in the SDK
 *
 * Feature : SDK test APIs
 *
 */

/*
 * Include Files
 */

#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <common/unittest_util.h>


#include <sdk/sdk_test.h>
#include <rtk/init.h>

/*for RTK API*/
#include <hal/hal_api_test_case.h>
#include <hal/hal_reg_test_case.h>
#include <dal/dal_gpon_test_case.h>
#include <dal/dal_l34_test_case.h>
#include <dal/dal_qos_test_case.h>
#include <dal/dal_svlan_test_case.h>
#include <dal/dal_acl_test_case.h>
#include <dal/dal_oam_test_case.h>
#include <dal/dal_ponmac_test_case.h>
#include <dal/dal_classf_test_case.h>
#include <dal/dal_stp_test_case.h>
#include <dal/dal_rate_test_case.h>
#include <dal/dal_sec_test_case.h>
#include <dal/dal_led_test_case.h>
#include <dal/dal_switch_test_case.h>
#include <dal/dal_vlan_test_case.h>
#include <dal/dal_cpu_test_case.h>
#include <dal/dal_mirror_test_case.h>
#include <dal/dal_trunk_test_case.h>
#include <dal/dal_port_test_case.h>
#include <dal/dal_l2_test_case.h>
#include <dal/dal_rldp_test_case.h>
#include <dal/dal_stat_test_case.h>
#include <dal/dal_trap_test_case.h>
#include <dal/dal_dot1x_test_case.h>
#include <dal/dal_intr_test_case.h>


/*for RAW driver*/
#ifdef CONFIG_SDK_APOLLO
#include <dal/raw/apollo/dal_raw_acl_test_case.h>
#include <dal/raw/apollo/dal_raw_cf_test_case.h>
#include <dal/raw/apollo/dal_raw_l34_test_case.h>
#include <dal/raw/apollo/dal_raw_lut_test_case.h>
#include <dal/raw/apollo/dal_raw_svlan_test_case.h>
#include <dal/raw/apollo/dal_raw_cpu_test_case.h>
#include <dal/raw/apollo/dal_raw_qos_test_case.h>
#include <dal/raw/apollo/dal_raw_dot1x_test_case.h>
#include <dal/raw/apollo/dal_raw_vlan_test_case.h>
#include <dal/raw/apollo/dal_raw_meter_test_case.h>
#include <dal/raw/apollo/dal_raw_rma_test_case.h>
#include <dal/raw/apollo/dal_raw_isolation_test_case.h>
#include <dal/raw/apollo/dal_raw_port_test_case.h>
#include <dal/raw/apollo/dal_raw_intr_test_case.h>
#include <dal/raw/apollo/dal_raw_trap_test_case.h>
#include <dal/raw/apollo/dal_raw_flowctrl_test_case.h>
#include <dal/raw/apollo/dal_raw_trunk_test_case.h>
#include <dal/raw/apollo/dal_raw_mirror_test_case.h>
#include <dal/raw/apollo/dal_raw_storm_test_case.h>
#include <dal/raw/apollo/dal_raw_rate_test_case.h>
#include <dal/raw/apollo/dal_raw_autofallback_test_case.h>
#include <dal/raw/apollo/dal_raw_stat_test_case.h>
#include <dal/raw/apollo/dal_raw_sec_test_case.h>
#endif
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
unit_test_case_t unitTestCase[] =
{

    /* HAL Module Test Case */
    /* HAL API test case: chip.c */
    UNIT_TEST_CASE(3, hal_api_halIsPpBlockCheck_test, GRP_ALL|GRP_HAL),
    UNIT_TEST_CASE(20, hal_api_halInit_test, GRP_ALL|GRP_HAL),

    /*HAL Register test case*/
    UNIT_TEST_CASE(30, hal_reg_def_test, GRP_ALL|GRP_HAL),
    UNIT_TEST_CASE(31, hal_reg_rw_test, GRP_ALL|GRP_HAL),

    /*DOT1X test case */
      /*API*/
	UNIT_TEST_CASE(1000, dal_dot1x_unauthPacketOper_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1001, dal_dot1x_portBasedEnable_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1002, dal_dot1x_portBasedAuthStatus_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1003, dal_dot1x_portBasedDirection_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1004, dal_dot1x_macBasedEnable_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1005, dal_dot1x_macBasedDirection_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1006, dal_dot1x_guestVlan_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1007, dal_dot1x_guestVlanBehavior_test, GRP_ALL|GRP_DOT1X),
	UNIT_TEST_CASE(1008, dal_dot1x_trapPri_test, GRP_ALL|GRP_DOT1X),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(1500, dal_dot1x_raw_test, GRP_ALL|GRP_QOS),
#endif


    /*FLOWCTRL test case */
      /*API*/
      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(4500, dal_flowctrl_raw_test, GRP_ALL|GRP_FLOWCTRL),
#endif


    /*L2 test case */
    /*LUT test*/
      /*API*/
    UNIT_TEST_CASE(5000, dal_l2_age_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5001, dal_l2_extPortEgrFilterMask_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5002, dal_l2_flushLinkDownPortAddrEnable_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5003, dal_l2_illegalPortMoveAction_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5004, dal_l2_portAgingEnable_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5005, dal_l2_ipmcGroupLookupMissHash_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5006, dal_l2_ipmcMode_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5007, dal_l2_limitLearningCnt_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5008, dal_l2_limitLearningCntAction_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5009, dal_l2_srcPortEgrFilterMask_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5010, dal_l2_newMacOp_test, GRP_ALL|GRP_L2),
#if !defined(CONFIG_SDK_RTL9602C)
    UNIT_TEST_CASE(5011, dal_l2_lookupMissAction_test, GRP_ALL|GRP_L2),
#endif
    UNIT_TEST_CASE(5012, dal_l2_lookupMissFloodPortMask_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5013, dal_l2_addr_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5014, dal_l2_mcastAddr_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5015, dal_l2_ipMcastAddr_test, GRP_ALL|GRP_L2),
#if !defined(CONFIG_SDK_RTL9602C)
    UNIT_TEST_CASE(5016, dal_l2_ipmcGroup_test, GRP_ALL|GRP_L2),
#endif
    UNIT_TEST_CASE(5017, dal_l2_portLimitLearningCnt_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5018, dal_l2_portLimitLearningCntAction_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5019, dal_l2_portLimitLearningOverStatus_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5020, dal_l2_limitLearningOverStatus_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5021, dal_l2_portLookupMissAction_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5022, dal_l2_portIpmcAction_test, GRP_ALL|GRP_L2),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(5500, dal_lut_raw_entry_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5501, dal_lut_raw_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5502, dal_lut_raw_action_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5503, dal_lut_raw_ipmcTable_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5504, dal_lut_raw_unknIp4Mc_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5505, dal_lut_raw_unknIp6Mc_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5506, dal_lut_raw_unknL2Mc_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5507, dal_lut_raw_unknMcPri_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5508, dal_lut_raw_l2UcEntry_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5509, dal_lut_raw_l2McDslEntry_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5510, dal_lut_raw_l3McDslEntry_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5511, dal_lut_raw_l3McRouteEntry_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5512, dal_lut_raw_l2UcEntryAll_test, GRP_ALL|GRP_L2),
    UNIT_TEST_CASE(5513, dal_lut_raw_writeHashEntry_test, GRP_ALL|GRP_L2),
#endif

    /*Port efid test*/
    /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(5600, dal_l2_raw_efid_test, GRP_ALL|GRP_L2),
#endif


   /* OAM test */
      /*API*/
    UNIT_TEST_CASE(9000, dal_oam_test, GRP_ALL|GRP_OAM),



    /* DAL PIE test case */
    /* ACL test */
      /*API*/
    UNIT_TEST_CASE(10000, dal_acl_test, GRP_ALL|GRP_PIE),
    UNIT_TEST_CASE(10001, dal_acl_test_ruleEntry, GRP_ALL|GRP_PIE),
    UNIT_TEST_CASE(10002, dal_acl_test_rangeCheck, GRP_ALL|GRP_PIE),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(10500, dal_raw_aclRule64_test, GRP_ALL|GRP_PIE),
    UNIT_TEST_CASE(10501, dal_raw_aclRule128_test, GRP_ALL|GRP_PIE),
    UNIT_TEST_CASE(10502, dal_raw_aclAct_test, GRP_ALL|GRP_PIE),
    UNIT_TEST_CASE(10503, dal_acl_raw_test, GRP_ALL|GRP_PIE),
#endif



    /* Port test case */
    /* Isolation test */
      /*API*/
    UNIT_TEST_CASE(11000, dal_port_adminEnable_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11001, dal_port_cpuPortId_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11002, dal_port_enhancedFid_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11003, dal_port_isolation_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11004, dal_port_isolationExt_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11005, dal_port_isolationL34_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11006, dal_port_isolationExtL34_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11007, dal_port_isolationEntry_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11008, dal_port_isolationEntryExt_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11009, dal_port_isolationCtagPktConfig_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11010, dal_port_isolationL34PktConfig_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11011, dal_port_linkStatus_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11012, dal_port_macLocalLoopbackEnable_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11013, dal_port_specialCongest_test, GRP_ALL|GRP_PORT),
    UNIT_TEST_CASE(11014, dal_port_specialCongestStatus_test, GRP_ALL|GRP_PORT),



      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(11500, dal_isolation_raw_test, GRP_ALL|GRP_PORT),
#endif

    /*Port force dmp*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(11510, dal_port_raw_forceDmp_test, GRP_ALL|GRP_PORT),
#endif

    /* Special congest*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(11520, dal_port_raw_congest_test, GRP_ALL|GRP_PORT),
#endif

    /* QoS test */
      /*API*/
    UNIT_TEST_CASE(12000, dal_qos_priSelGroup_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12001, dal_qos_portPri_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12002, dal_qos_dscpPriRemapGroup_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12003, dal_qos_1pPriRemapGroup_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12004, dal_qos_priMap_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12005, dal_qos_portPriMap_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12006, dal_qos_1pRemarkEnable_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12007, dal_qos_1pRemarkGroup_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12008, dal_qos_dscpRemarkEnable_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12009, dal_qos_dscpRemarkGroup_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12010, dal_qos_portDscpRemarkSrcSel_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12011, dal_qos_dscp2DscpRemarkGroup_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12012, dal_qos_fwd2CpuPriRemap_test, GRP_ALL|GRP_QOS),
    UNIT_TEST_CASE(12013, dal_qos_schedulingQueue_test, GRP_ALL|GRP_QOS),


      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(12500, dal_qos_raw_test, GRP_ALL|GRP_QOS),
#endif

    /* Rate test case */
    UNIT_TEST_CASE(13000, dal_rate_portIgrBandwidthCtrlRate_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13001, dal_rate_portIgrBandwidthCtrlIncludeIfg_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13002, dal_rate_portEgrBandwidthCtrlRate_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13003, dal_rate_egrBandwidthCtrlIncludeIfg_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13004, dal_rate_egrQueueBwCtrlEnable_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13005, dal_rate_egrQueueBwCtrlMeterIdx_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13006, dal_rate_stormControlMeterIdx_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13007, dal_rate_stormControlPortEnable_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13008, dal_rate_stormControlEnable_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13009, dal_rate_stormBypass_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13011, dal_rate_shareMeter_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13012, dal_rate_shareMeterBucket_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13013, dal_rate_shareMeterExceed_test, GRP_ALL|GRP_RATE),

    /* Shared meter */
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(13500, dal_meter_raw_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13501, dal_raw_storm_state_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13502, dal_raw_storm_meter_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13503, dal_raw_storm_alt_test, GRP_ALL|GRP_RATE),
    UNIT_TEST_CASE(13504, dal_rate_raw_test, GRP_ALL|GRP_RATE),
#endif


    /* SEC test */
      /*API*/
    UNIT_TEST_CASE(14000, dal_sec_portAttackPreventState_test, GRP_ALL|GRP_SEC),
    UNIT_TEST_CASE(14001, dal_sec_attackPrevent_test, GRP_ALL|GRP_SEC),
    UNIT_TEST_CASE(14002, dal_sec_attackFloodThresh_test, GRP_ALL|GRP_SEC),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(14500, dal_raw_sec_test, GRP_ALL|GRP_SEC),
#endif


    /*stp*/
    UNIT_TEST_CASE(15000, dal_stp_mstpState_test, GRP_ALL|GRP_STP),


     /*Classification test*/
      /*API*/
    UNIT_TEST_CASE(16000, dal_classf_rule_range_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16001, dal_classf_usAct_range_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16002, dal_classf_dsAct_range_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16003, dal_classf_misc_range_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16004, dal_classf_rule_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16005, dal_classf_action_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16006, dal_classf_entry_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16007, dal_classf_misc_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16008, dal_classf_test1, GRP_ALL|GRP_CLASSF),


      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(16500, dal_raw_cf_ruleAll_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16501, dal_raw_cf_downStreamAll_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16502, dal_raw_cf_upStreamAll_test, GRP_ALL|GRP_CLASSF),
    UNIT_TEST_CASE(16503, dal_raw_cf_cfg_test, GRP_ALL|GRP_CLASSF),
#endif


    /*switch*/
    UNIT_TEST_CASE(19000, dal_switch_maxPktLenLinkSpeed_test, GRP_ALL|GRP_SWITCH),
    UNIT_TEST_CASE(19001, dal_switch_mgmtMacAddr_test, GRP_ALL|GRP_SWITCH),
    UNIT_TEST_CASE(19002, dal_switch_maxPktLenByPort_test, GRP_ALL|GRP_SWITCH),



    /* TRAP_BASE */
      /* API */
    UNIT_TEST_CASE(20000, dal_trap_igmpCtrlPkt2CpuEnable_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20001, dal_trap_ipMcastPkt2CpuEnable_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20002, dal_trap_l2McastPkt2CpuEnable_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20003, dal_trap_mldCtrlPkt2CpuEnable_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20004, dal_trap_oamPduAction_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20005, dal_trap_oamPduPri_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20006, dal_trap_reasonTrapToCpuPriority_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20007, dal_trap_rmaAction_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20008, dal_trap_rmaPri_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20009, dal_trap_portIgmpMldCtrlPktAction_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20010, dal_trap_uniTrapPriorityEnable_test, GRP_ALL|GRP_TRAP),




      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(20500, dal_rma_raw_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20501, dal_raw_trap_igmp_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20502, dal_raw_trap_igmp_BypassStorm_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20503, dal_raw_trap_igmp_ChecksumError_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20504, dal_raw_trap_igmp_IsolationLeaky_test, GRP_ALL|GRP_TRAP),
    UNIT_TEST_CASE(20505, dal_raw_trap_igmp_vlanLeaky_test, GRP_ALL|GRP_TRAP),
#endif

    /* TRUNK BASE */
      /*API*/
    UNIT_TEST_CASE(21000, dal_trunk_Port_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21001, dal_trunk_distributionAlgorithm_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21002, dal_trunk_hashMappingTable_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21003, dal_trunk_mode_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21004, dal_trunk_trafficPause_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21005, dal_trunk_separateType_test, GRP_ALL|GRP_TRUNK),


      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(21500, dal_raw_trunk_memberPort_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21501, dal_raw_trunk_mode_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21502, dal_raw_trunk_hashMapping_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21503, dal_raw_trunk_hashAlgorithm_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21504, dal_raw_trunk_flowControl_test, GRP_ALL|GRP_TRUNK),
    UNIT_TEST_CASE(21505, dal_raw_trunk_flood_test, GRP_ALL|GRP_TRUNK),
#endif

    /*VLAN test*/
      /*API*/
    UNIT_TEST_CASE(22000, dal_vlan_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22001, dal_vlan_Fid_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22002, dal_vlan_portFid_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22003, dal_vlan_portPriority_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22004, dal_vlan_accpetFrameType_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22005, dal_vlan_portEgrTagKeepType_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22006, dal_vlan_portIgrFilterEnable_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22007, dal_vlan_misc_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22008, dal_vlan_leaky_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22009, dal_vlan_portProtoVlan_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22010, dal_vlan_priority_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22011, dal_vlan_priorityEnable_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22012, dal_vlan_protoGroup_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22013, dal_vlan_reservedVidAction_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22014, dal_vlan_stg_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22015, dal_vlan_tagMode_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22016, dal_vlan_transparent_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22017, dal_vlan_vlanFunc_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22018, dal_vlan_portLeaky_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22019, dal_vlan_keepType_test, GRP_ALL|GRP_VLAN),


      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(22500, dal_vlan_raw_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22501, dal_vlan_raw_4kEntry_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22502, dal_vlan_raw_memberConfig_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22503, dal_vlan_raw_protoAndPbCfg_test, GRP_ALL|GRP_VLAN),
    UNIT_TEST_CASE(22504, dal_vlan_raw_portAndProtoBasedEntry_test, GRP_ALL|GRP_VLAN),
#endif


    /* SVLAN test */
      /*API*/
    UNIT_TEST_CASE(23000, dal_svlan_test, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23001, dal_svlan_test_differChip, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23002, dal_svlan_test_memberPortEntry, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23003, dal_svlan_test_ipmcL2mc2s, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23004, dal_svlan_test_sp2c, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23005, dal_svlan_test_c2s, GRP_ALL|GRP_SVLAN),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(23500, dal_svlan_raw_test, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23501, dal_svlan_raw_mbrCfg_test, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23502, dal_svlan_raw_mc2sCfg_test, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23503, dal_svlan_raw_c2sCfg_test, GRP_ALL|GRP_SVLAN),
    UNIT_TEST_CASE(23504, dal_svlan_raw_sp2cCfg_test, GRP_ALL|GRP_SVLAN),
#endif


    /* MIB test */
      /*API*/
    UNIT_TEST_CASE(27000, dal_stat_test, GRP_ALL|GRP_STATS),
      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(27500, dal_raw_stat_test, GRP_ALL|GRP_STATS),
#endif


    /* GPON test */
      /*API*/
#if CONFIG_GPON_VERSION==1
    UNIT_TEST_CASE(28000, dal_gpon_sn_test, GRP_ALL|GRP_GPON),
    UNIT_TEST_CASE(28001, dal_gpon_para_test, GRP_ALL|GRP_GPON),
    UNIT_TEST_CASE(28002, dal_gpon_tcont_test, GRP_ALL|GRP_GPON),
    UNIT_TEST_CASE(28003, dal_gpon_dsFlow_test, GRP_ALL|GRP_GPON),
    UNIT_TEST_CASE(28004, dal_gpon_usFlow_test, GRP_ALL|GRP_GPON),
    UNIT_TEST_CASE(28005, dal_gpon_mcFilter_test, GRP_ALL|GRP_GPON),
    UNIT_TEST_CASE(28006, dal_gpon_misc_test, GRP_ALL|GRP_GPON),
#endif
/*RAW driver*/

    /* L34 test */
      /*API*/
      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(29000, dal_l34_netifTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29001, dal_l34_arpTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29002, dal_l34_pppoeTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29003, dal_l34_routingTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29004, dal_l34_nexthopTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29005, dal_l34_extIntIPTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29006, dal_l34_naptInboundTable_test, GRP_ALL|GRP_L34),
    UNIT_TEST_CASE(29007, dal_l34_naptOutboundTable_test, GRP_ALL|GRP_L34),
#endif

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(29500, dal_raw_l34_raw_test, GRP_ALL|GRP_L34),
#endif

    /* CPU test */
      /*API*/
    UNIT_TEST_CASE(31000, dal_cpu_port_test, GRP_ALL|GRP_CPU),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(31500, dal_raw_cpu_port_test, GRP_ALL|GRP_CPU),
#endif

    /* Intr test */
      /*API*/
    UNIT_TEST_CASE(32000, dal_intr_imr_test, GRP_ALL|GRP_INTR),
    UNIT_TEST_CASE(32001, dal_intr_ims_test, GRP_ALL|GRP_INTR),
    UNIT_TEST_CASE(32002, dal_intr_sts_test, GRP_ALL|GRP_INTR),
    UNIT_TEST_CASE(32003, dal_intr_polarity_test, GRP_ALL|GRP_INTR),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(32500, dal_raw_intr_imr_test, GRP_ALL|GRP_INTR),
    UNIT_TEST_CASE(32501, dal_raw_intr_ims_test, GRP_ALL|GRP_INTR),
    UNIT_TEST_CASE(32502, dal_raw_intr_sts_test, GRP_ALL|GRP_INTR),
    UNIT_TEST_CASE(32503, dal_raw_intr_polarity_test, GRP_ALL|GRP_INTR),
#endif

    /* Mirror test */
      /*API*/
    UNIT_TEST_CASE(33000, dal_mirrorPort_test, GRP_ALL|GRP_MIRROR),
    UNIT_TEST_CASE(33001, dal_mirrorIsolation_test, GRP_ALL|GRP_MIRROR),

      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(33500, dal_raw_mirror_cfg_test, GRP_ALL|GRP_MIRROR),
    UNIT_TEST_CASE(33501, dal_raw_mirror_isolation_test, GRP_ALL|GRP_MIRROR),
#endif

    /* Auto Fallback test */
      /*API*/
      /*RAW driver*/
#ifdef CONFIG_SDK_APOLLO
    UNIT_TEST_CASE(34500, dal_raw_autoFallback_state_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34501, dal_raw_autoFallback_MaxMonitorCount_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34502, dal_raw_autoFallback_MaxErrorCount_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34503, dal_raw_autoFallback_reducePL_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34504, dal_raw_autoFallback_timeoutIgnore_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34505, dal_raw_autoFallback_timeoutTH_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34506, dal_raw_autoFallback_timer_test, GRP_ALL|GRP_AFB),
    UNIT_TEST_CASE(34507, dal_raw_autoFallback_validFlow_test, GRP_ALL|GRP_AFB),
#endif

    /* PON MAC test */
      /*API*/
    UNIT_TEST_CASE(35000, dal_ponmac_queue_test, GRP_ALL|GRP_PONMAC),
    UNIT_TEST_CASE(35001, dal_ponmac_flow2Queue_test, GRP_ALL|GRP_PONMAC),


    /* LED test */
      /*API*/
    UNIT_TEST_CASE(36000, dal_led_operation_test, GRP_ALL|GRP_LED),
    UNIT_TEST_CASE(36001, dal_led_serialMode_test, GRP_ALL|GRP_LED),
    UNIT_TEST_CASE(36002, dal_led_blinkRate_test, GRP_ALL|GRP_LED),
    UNIT_TEST_CASE(36003, dal_led_config_test, GRP_ALL|GRP_LED),
    UNIT_TEST_CASE(36004, dal_led_modeForce_test, GRP_ALL|GRP_LED),

    /* RLDP test */
      /*API*/
#if defined(CONFIG_SDK_RTL9602C) || defined (CONFIG_SDK_RTL9607C)
    UNIT_TEST_CASE(37000, dal_rldp_test, GRP_ALL|GRP_RLDP),
#endif

    /* Final case, DO NOT remove it */
    UNIT_TEST_CASE(100000, NULL, GRP_ALL),
};

/*
 * Function Declaration
 */

/* Function Name:
 *      sdktest_run
 * Description:
 *      Test one test case or group test cases in the SDK for one specified device.
 * Input:
 *      unit  - unit id
 *      *pStr - string context
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
sdktest_run(uint32 unit, uint8 *pStr)
{
    int32 lower, upper;
    int i;
    int32 ret;
    int32 totalCase = 0;
    int32 okCase = 0;
    int32 failCase = 0;
    int32 failCaseIdx[sizeof(unitTestCase) / sizeof(unitTestCase[0])];
    uint64 groupmask = GRP_ALL;

    lower = 0;
    upper = unitTestCase[sizeof(unitTestCase) / sizeof(unitTestCase[0]) - 1].no;

    if (!osal_strcmp(pStr, "all"))
        groupmask = GRP_ALL;
    else if (!osal_strcmp(pStr, "port"))
        groupmask = GRP_PORT;
    else if (!osal_strcmp(pStr, "hal"))
        groupmask = GRP_HAL;
    else if (!osal_strcmp(pStr, "dot1x"))
        groupmask = GRP_DOT1X;
    else if (!osal_strcmp(pStr, "eee"))
        groupmask = GRP_EEE;
    else if (!osal_strcmp(pStr, "flowctrl"))
        groupmask = GRP_FLOWCTRL;
    else if (!osal_strcmp(pStr, "l2"))
        groupmask = GRP_L2;
    else if (!osal_strcmp(pStr, "l3"))
        groupmask = GRP_L3;
    else if (!osal_strcmp(pStr, "oam"))
        groupmask = GRP_OAM;
    else if (!osal_strcmp(pStr, "pie"))
        groupmask = GRP_PIE;
    else if (!osal_strcmp(pStr, "qos"))
        groupmask = GRP_QOS;
    else if (!osal_strcmp(pStr, "rate"))
        groupmask = GRP_RATE;
    else if (!osal_strcmp(pStr, "sec"))
        groupmask = GRP_SEC;
    else if (!osal_strcmp(pStr, "stp"))
        groupmask = GRP_STP;
    else if (!osal_strcmp(pStr, "switch"))
        groupmask = GRP_SWITCH;
    else if (!osal_strcmp(pStr, "trap"))
        groupmask = GRP_TRAP;
    else if (!osal_strcmp(pStr, "trunk"))
        groupmask = GRP_TRUNK;
    else if (!osal_strcmp(pStr, "vlan"))
        groupmask = GRP_VLAN;
    else if (!osal_strcmp(pStr, "svlan"))
        groupmask = GRP_SVLAN;
    else if (!osal_strcmp(pStr, "filter"))
        groupmask = GRP_FILTER;
    else if (!strcmp(pStr, "nic"))
        groupmask = GRP_NIC;
    else if (!strcmp(pStr, "osal"))
        groupmask = GRP_OSAL;
    else if (!strcmp(pStr, "stats"))
        groupmask = GRP_STATS;
    else if (!strcmp(pStr, "gpon"))
        groupmask = GRP_GPON;
    else if (!strcmp(pStr, "l34"))
        groupmask = GRP_L34;
    else if (!strcmp(pStr, "ponmac"))
        groupmask = GRP_PONMAC;
    else if (!strcmp(pStr, "classf"))
        groupmask = GRP_CLASSF;
    else if (!strcmp(pStr, "led"))
        groupmask = GRP_LED;
    else if (!strcmp(pStr, "rldp"))
        groupmask = GRP_RLDP;
    else if (!strcmp(pStr, "intr"))
        groupmask = GRP_INTR;


    for (i = 0; i < sizeof(unitTestCase) / sizeof(unitTestCase[0]); i++)
    {
        unit_test_case_t *pCase = &unitTestCase[i];

        if (pCase->no < lower || pCase->no > upper)
            continue;
        if ((pCase->group & groupmask) == 0)
            continue;
        if (pCase->fp == NULL)
            continue;

        totalCase++;
        osal_printf("Running Test Case %d: %s() ...\n", pCase->no, pCase->name);

        ret = pCase->fp(pCase->no);

        if (ret == RT_ERR_OK)
        {
            osal_printf(" ok!\n\n");
            okCase++;
        }
        else
        {
            osal_printf("\033[31;43m fail!! \033[m\n\n");
            failCaseIdx[failCase] = i;
            failCase++;
        }

    }

    osal_printf(">>Total Case: %d,  Pass: %d,  Fail: %d\n\n", totalCase, okCase, failCase);
    if (failCase > 0)
    {
        /* list fail cases */
        int i;

        osal_printf("  +-- Failed Cases ----------------------------------------------------------+\n");
        for (i = 0; i < failCase; i++)
        {
            osal_printf("  | case %-5d  %-60s |\n", unitTestCase[failCaseIdx[i]].no,
                           unitTestCase[failCaseIdx[i]].name);
        }
        osal_printf("  +--------------------------------------------------------------------------+\007\n\n");
    }

    return RT_ERR_OK;
} /* end of sdktest_run */

/* Function Name:
 *      sdktest_run_id
 * Description:
 *      Test some test cases from start to end case index in the SDK for one specified device.
 * Input:
 *      unit  - unit id
 *      start - start test case number
 *      end   - end test case number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
sdktest_run_id(uint32 unit, uint32 start, uint32 end)
{
    int32 lower, upper;
    int i;
    int32 ret;
    int32 totalCase = 0;
    int32 okCase = 0;
    int32 failCase = 0;
    int32 failCaseIdx[sizeof(unitTestCase) / sizeof(unitTestCase[0])];

    lower = start;
    upper = end;

    for (i = 0; i < sizeof(unitTestCase) / sizeof(unitTestCase[0]); i++)
    {
        unit_test_case_t *pCase = &unitTestCase[i];

        if (pCase->no < lower || pCase->no > upper)
            continue;
        if (pCase->fp == NULL)
            continue;

        totalCase++;
        osal_printf("Running Test Case %d: %s() ...\n", pCase->no, pCase->name);

        ret = pCase->fp(pCase->no);

        if (ret == RT_ERR_OK)
        {
            osal_printf(" ok!\n\n");
            okCase++;
        }
        else
        {
            osal_printf("\033[31;43m fail!! \033[m\n\n");
            failCaseIdx[failCase] = i;
            failCase++;
        }

    }

    osal_printf(">>Total Case: %d,  Pass: %d,  Fail: %d\n\n", totalCase, okCase, failCase);
    if (failCase > 0)
    {
        /* list fail cases */
        int i;

        osal_printf("  +-- Failed Cases ----------------------------------------------------------+\n");
        for (i = 0; i < failCase; i++)
        {
            osal_printf("  | case %-5d  %-60s |\n", unitTestCase[failCaseIdx[i]].no,
                           unitTestCase[failCaseIdx[i]].name);
        }
        osal_printf("  +--------------------------------------------------------------------------+\007\n\n");
    }

    return RT_ERR_OK;
} /* end of sdktest_run_id */
