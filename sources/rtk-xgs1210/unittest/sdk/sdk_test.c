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
 * $Revision: 74758 $
 * $Date: 2016-12-28 16:08:11 +0800 (Wed, 28 Dec 2016) $
 *
 * Purpose : Definition of SDK test APIs in the SDK
 *
 * Feature : SDK test APIs
 *
 */

/*
 * Include Files
 */

#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <common/unittest_util.h>
#if (!defined(CONFIG_SDK_MODEL_MODE) && !defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE))
#include <osal/osal_test_case.h>
#include <nic/nic_test_case.h>
#endif
#include <hal/hal_api_test_case.h>
#include <hal/hal_database_test_case.h>
#include <hal/hal_mechanism_test_case.h>
#include <dal/dal_vlan_test_case.h>
#include <dal/dal_qos_test_case.h>
#include <dal/dal_eee_test_case.h>
#include <dal/dal_flowctrl_test_case.h>
#include <dal/dal_trunk_test_case.h>
#include <dal/dal_l2_test_case.h>
#include <dal/dal_switch_test_case.h>
#include <dal/dal_stp_test_case.h>
#include <dal/dal_trap_test_case.h>
#include <dal/dal_port_test_case.h>
#include <dal/dal_vlan_test_case.h>
#include <dal/dal_rate_test_case.h>
#include <dal/dal_stat_test_case.h>
#include <dal/dal_led_test_case.h>
#include <dal/dal_l3_test_case.h>
#include <dal/dal_sec_test_case.h>
#include <dal/dal_oam_test_case.h>
#include <dal/dal_mpls_test_case.h>
#include <dal/dal_time_test_case.h>
#include <dal/dal_acl_test_case.h>
#include <dal/dal_mirror_test_case.h>
#include <sdk/sdk_test.h>
#include <rtk/init.h>




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
    UNIT_TEST_CASE(0, hal_api_halFindDevice_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(1, hal_api_halGetDriverId_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(2, hal_api_halGetChipId_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(3, hal_api_halIsPpBlockCheck_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    /* HAL API test case: driver.c */
    UNIT_TEST_CASE(10, hal_api_halFindDriver_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    /* HAL API test case: halctrl.c */
    UNIT_TEST_CASE(20, hal_api_halInit_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(21, hal_api_halCtrlInfoGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    /* HAL API test case: reg.c */
    UNIT_TEST_CASE(50, hal_api_regArrayFieldRead_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(51, hal_api_regArrayFieldWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(52, hal_api_regArrayRead_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(53, hal_api_regArrayWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(54, hal_api_regFieldGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(55, hal_api_regFieldSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(56, hal_api_regFieldRead_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(57, hal_api_regFieldWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(58, hal_api_regIdx2AddrGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(59, hal_api_regIdxMaxGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(60, hal_api_regRead_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(61, hal_api_regWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    /* HAL API test case: mem.c */
    UNIT_TEST_CASE(70, hal_api_tableFieldByteGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(71, hal_api_tableFieldByteSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(72, hal_api_tableFieldGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    //UNIT_TEST_CASE(73, hal_api_tableFieldMacGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    //UNIT_TEST_CASE(74, hal_api_tableFieldMacSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(75, hal_api_tableFieldSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(76, hal_api_tableRead_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(77, hal_api_tableSizeGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(78, hal_api_tableWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    /* HAL API test case: drv.c, not public APIs */
    /* HAL API test case: identify.c */
    UNIT_TEST_CASE(100, hal_api_phyIdentifyFind_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(101, hal_api_phyIdentifyOUICheck_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(102, hal_api_phyIdentifyPhyidGet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    /* HAL API test case: phy_8214f.c, not public APIs */
    /* HAL API test case: phy_8208.c, not public APIs */
    /* HAL API test case: miim.c */
    UNIT_TEST_CASE(130, hal_api_halMiimReadWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(131, hal_api_phyAutoNegoAbilityGetSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(132, hal_api_phyAutoNegoEnableGetSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(133, hal_api_phyDuplexGetSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(134, hal_api_phyEnableSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(135, hal_api_phyMediaGetSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(136, hal_api_phySpeedGetSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),

    /* HAL performance test case */
    UNIT_TEST_CASE(150, hal_api_regReadPerformance_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(151, hal_api_miimReadPerformance_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),

    /* HAL database test case */
    UNIT_TEST_CASE(200, hal_database_halctrl_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    UNIT_TEST_CASE(201, hal_database_regFieldDef_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    //UNIT_TEST_CASE(202, hal_database_regDefaultVal_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),
    //UNIT_TEST_CASE(203, hal_database_tableFieldDef_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),

    /* HAL mechanism test case */
    UNIT_TEST_CASE(300, hal_mechanism_probe_test, SDKTEST_GRP_ALL|SDKTEST_GRP_HAL),

    /* EEE test case */
    UNIT_TEST_CASE(2000, dal_eee_portEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_EEE),
    UNIT_TEST_CASE(2001, dal_eeep_portEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_EEE),
    /* Time test case */
    UNIT_TEST_CASE(3000, dal_time_refTime_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TIME),
    UNIT_TEST_CASE(3001, dal_time_refTimeAdjust_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TIME),
    UNIT_TEST_CASE(3002, dal_time_portPtpEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TIME),


    /* flowcontrol test case */
    UNIT_TEST_CASE(4002, dal_flowctrl_portPauseOnAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4003, dal_flowctrl_portPauseOnAllowedPageNum_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4004, dal_flowctrl_pauseOnAllowedPktNum_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4005, dal_flowctrl_igrSystemPauseThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4007, dal_flowctrl_igrSystemCongestThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4011, dal_flowctrl_egrSystemDropThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4012, dal_flowctrl_egrPortDropThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4014, dal_flowctrl_portEgrDropRefCongestEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4015, dal_flowctrl_pauseOnAllowedPktLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4016, dal_flowctrl_igrPauseThreshGroup_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4017, dal_flowctrl_portIgrPortThreshGroupSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4018, dal_flowctrl_igrCongestThreshGroup_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4019, dal_flowctrl_egrQueueDropThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4020, dal_flowctrl_egrCpuQueueDropThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),
    UNIT_TEST_CASE(4021, dal_flowctrl_egrPortQueueDropEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_FLOWCTRL),

    /*L2 test case */
    UNIT_TEST_CASE(5000, dal_l2_flushLinkDownPortAddrEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5002, dal_l2_portLimitLearningCnt_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5003, dal_l2_portLimitLearningCntAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5004, dal_l2_fidLimitLearningEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5008, dal_l2_aging_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5010, dal_l2_hashAlgo_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5011, dal_l2_vlanMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5013, dal_l2_portNewMacOp_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5015, dal_l2_addr_getNext_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5016, dal_l2_addr_all_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5017, dal_l2_addr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5018, dal_l2_addr_del_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5019, dal_l2_addr_set_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5023, dal_l2_ipmcMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5031, dal_l2_legalPortMoveAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5036, dal_l2_lookupMissFloodPortMask_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5037, dal_l2_lookupMissFloodPortMask_add_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5038, dal_l2_srcPortEgrFilterMask_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5039, dal_l2_srcPortEgrFilterMask_add_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5040, dal_l2_exceptionAddrAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5043, dal_l2_mcastAddr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5044, dal_l2_mcastAddr_del_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5045, dal_l2_mcastAddr_set_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5043, dal_l2_mcastAddr_all_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5046, dal_l2_mcastAddr_getNext_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5047, dal_l2_ipMcastAddr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5048, dal_l2_ipMcastAddr_del_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5049, dal_l2_ipMcastAddr_set_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5047, dal_l2_ipMcastAddr_all_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5050, dal_l2_ipMcastAddr_getNext_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5051, dal_l2_addr_test1, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5052, dal_l2_mcastAddr_test1, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5053, dal_l2_ipMcastAddr_test1, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5054, dal_l2_limitLearningCnt_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5055, dal_l2_limitLearningCntAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5056, dal_l2_fidLearningCntAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5057, dal_l2_portAgingEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5058, dal_l2_zeroSALearningEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5059, dal_l2_staticPortMoveAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5060, dal_l2_legalPortMoveFlushAddrEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5062, dal_l2_mcastFwdPortmask_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5063, dal_l2_cpuMacAddr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5064, dal_l2_ipMcastAddrChkEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5065, dal_l2_ipMcstFidVidCompareEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5066, dal_l2_mcastAddr_addByIndex_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5067, dal_l2_ipMcastAddr_addByIndex_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5068, dal_l2_ip6mcMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5069, dal_l2_ip6CareByte_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5070, dal_l2_ip6McastAddr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5071, dal_l2_ip6McastAddr_del_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
//    UNIT_TEST_CASE(5072, dal_l2_ip6McastAddr_set_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5070, dal_l2_ip6McastAddr_DIPSIP_all_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5071, dal_l2_ip6McastAddr_DIPFVID_all_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5073, dal_l2_ip6McastAddr_addByIndex_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5074, dal_l2_ip6McastAddr_getNext_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5075, dal_l2_portLookupMissAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5076, dal_l2_mcastFwdIndex_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5078, dal_l2_secureMacMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5079, dal_l2_portDynamicPortMoveForbidEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),
    UNIT_TEST_CASE(5080, dal_l2_dynamicPortMoveForbidAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L2),

    UNIT_TEST_CASE(6000, dal_l3_routeSwitchMacAddr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_L3),
    UNIT_TEST_CASE(7000, dal_mpls_ttlInherit_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MPLS),
    UNIT_TEST_CASE(7001, dal_mpls_encap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MPLS),
    UNIT_TEST_CASE(7002, dal_mpls_enable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MPLS),

    /*LED test case */
    UNIT_TEST_CASE(8001, dal_led_sysEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_LED),
    UNIT_TEST_CASE(8002, dal_led_portLedEntitySwCtrlEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_LED),
    UNIT_TEST_CASE(8003, dal_led_portLedEntitySwCtrlMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_LED),
    UNIT_TEST_CASE(8004, dal_led_sysMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_LED),

    /*OAM*/
    UNIT_TEST_CASE(9000, dal_oam_autoDyingGaspEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9001, dal_oam_dyingGaspWaitTime_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9002, dal_oam_loopbackMacSwapEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9003, dal_oam_portLoopbackMuxAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9004, dal_oam_cfmCcmPcp_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9005, dal_oam_cfmCcmCfi_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9006, dal_oam_cfmCcmTpid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9007, dal_oam_cfmCcmInstLifetime_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9008, dal_oam_cfmCcmMepid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9009, dal_oam_cfmCcmIntervalField_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9010, dal_oam_cfmCcmMdl_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9011, dal_oam_cfmCcmInstTagStatus_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9012, dal_oam_cfmCcmInstVid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9013, dal_oam_cfmCcmInstMaid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9014, dal_oam_cfmCcmInstTxStatus_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9015, dal_oam_cfmCcmInstInterval_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9016, dal_oam_cfmCcmRxInstVid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9017, dal_oam_cfmCcmTxInstPort_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9018, dal_oam_cfmCcmRxInstPort_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9019, dal_oam_cfmPortEthDmEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9020, dal_oam_cfmCcmInstAliveTime_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),
    UNIT_TEST_CASE(9021, dal_oam_dyingGaspSend_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OAM),

    /*Port*/
    UNIT_TEST_CASE(11001, dal_port_isolation_test, SDKTEST_GRP_ALL),
    UNIT_TEST_CASE(11002, dal_port_isolation_add_test, SDKTEST_GRP_ALL),
    UNIT_TEST_CASE(11003, dal_port_backpressureEnable_test, SDKTEST_GRP_ALL),
    UNIT_TEST_CASE(11004, dal_port_adminEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11005, dal_port_linkMon_swScanPorts_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11006, dal_port_vlanBasedIsolationEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11007, dal_port_vlanBasedIsolation_vlanSource_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11008, dal_port_rxEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11009, dal_port_specialCongest_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11010, dal_port_link_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11011, dal_port_speedDuplex_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11012, dal_port_flowctrl_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11013, dal_port_cpuPortId_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11014, dal_port_phyAutoNegoEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11015, dal_port_phyAutoNegoAbility_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11016, dal_port_phyMasterSlave_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11017, dal_port_phyForceModeAbility_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11018, dal_port_phyComboPortMedia_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    UNIT_TEST_CASE(11019, dal_port_txEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT),
    //UNIT_TEST_CASE(11033, dal_port_linkMon_test, SDKTEST_GRP_ALL|SDKTEST_GRP_PORT), //sdk3 will support

     /* Qos Test */
    UNIT_TEST_CASE(12001, dal_qos_priSelGroup_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12002, dal_qos_portPriSelGroup_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12003, dal_qos_portPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12004, dal_qos_portInnerPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12005, dal_qos_portOuterPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12006, dal_qos_dscpPriRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12007, dal_qos_1pPriRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12008, dal_qos_queueNum_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12009, dal_qos_1pRemark_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12010, dal_qos_1pRemarkEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12011, dal_qos_out1pRemarkEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12012, dal_qos_dscpRemarkEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12013, dal_qos_dscpRemark_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12014, dal_qos_schedulingAlgorithm_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12015, dal_qos_schedulingQueue_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12016, dal_qos_congAvoidAlgo_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12017, dal_qos_congAvoidSysThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12018, dal_qos_dpSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12019, dal_qos_portDEISrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12020, dal_qos_deiDpRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12021, dal_qos_dscpDpRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12022, dal_qos_outer1pPriRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12023, dal_qos_priMap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12024, dal_qos_1pDfltPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12025, dal_qos_1pRemarkSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12026, dal_qos_outer1pRemark_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12027, dal_qos_outer1pRemarkSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12028, dal_qos_portOuter1pDfltPriSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12029, dal_qos_dscp2DscpRemark_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12030, dal_qos_dscpRemarkSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12031, dal_qos_deiRemark_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12032, dal_qos_deiRemarkEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12033, dal_qos_portDeiRemarkTagSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12034, dal_qos_congAvoidSysDropProbability_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12035, dal_qos_congAvoidGlobalQueueThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12036, dal_qos_congAvoidGlobalQueueDropProbability_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12037, dal_qos_portAvbStreamReservationClassEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12038, dal_qos_avbStreamReservationConfig_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12039, dal_qos_pkt2CpuPriRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12040, dal_qos_rspanPriRemap_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12041, dal_qos_1pDfltPriSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12042, dal_qos_portOuter1pRemarkSrcSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),
    UNIT_TEST_CASE(12043, dal_qos_outer1pDfltPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_QOS),

    /* DAL RATE test case */
    UNIT_TEST_CASE(13000, dal_rate_portEgrBwCtrlEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13001, dal_rate_portEgrBwCtrlRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13002, dal_rate_portEgrQueueBwCtrlEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13003, dal_rate_portEgrQueueBwCtrlRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13004, dal_rate_portIgrBwCtrlEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13006, dal_rate_portIgrBwCtrlRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13007, dal_rate_portIgrBwFlowctrlEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13008, dal_rate_igrBandwidthCtrlIncludeIfg_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13009, dal_rate_egrBandwidthCtrlIncludeIfg_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13010, dal_rate_portStormCtrlBurstSize_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13011, dal_rate_portStormCtrlExceed_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13012, dal_rate_stormControlIncludeIfg_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13013, dal_rate_portStormCtrlRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13014, dal_rate_igrBandwidthCtrlBypass_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13015, dal_rate_igrBandwidthLowThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13016, dal_rate_portIgrBandwidthHighThresh_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13017, dal_rate_portStormCtrlProtoRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13018, dal_rate_stormControlProtoExceed_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13019, dal_rate_stormControlBypass_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13020, dal_rate_portStormCtrlTypeSel_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    UNIT_TEST_CASE(13021, dal_rate_portIgrBwCtrlRate_10G_test, SDKTEST_GRP_ALL|SDKTEST_GRP_RATE),
    /* DAL SEC test case */
    UNIT_TEST_CASE(14000, dal_sec_portAttackPreventEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    UNIT_TEST_CASE(14001, dal_sec_attackPreventAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    UNIT_TEST_CASE(14002, dal_sec_minIPv6FragLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    UNIT_TEST_CASE(14003, dal_sec_maxPingLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    UNIT_TEST_CASE(14004, dal_sec_minTCPHdrLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    UNIT_TEST_CASE(14005, dal_sec_smurfNetmaskLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    UNIT_TEST_CASE(14006, dal_sec_portAttackPrevent_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SEC),
    /*ACL*/
    UNIT_TEST_CASE(15000, dal_acl_partition_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15001, dal_acl_blockPwrEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15002, dal_acl_blockLookupEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15003, dal_acl_ruleEntryFieldSize_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15004, dal_acl_ruleValidate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15005, dal_acl_blockResultMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15006, dal_acl_blockGroupEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15007, dal_acl_statPktCnt_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15008, dal_acl_statByteCnt_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15009, dal_acl_statClear_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15010, dal_acl_rangeCheckL4Port_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15011, dal_acl_rangeCheckVid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15012, dal_acl_rangeCheckIp_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15013, dal_acl_rangeCheckSrcPort_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15014, dal_acl_rangeCheckDstPort_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15015, dal_acl_rangeCheckPacketLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15016, dal_acl_fieldSelector_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    //UNIT_TEST_CASE(15017, dal_acl_portFieldSelector_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15018, dal_acl_meterMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15019, dal_acl_meterIncludeIfg_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15020, dal_acl_meterBurstSize_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15021, dal_acl_meterExceed_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15022, dal_acl_meterEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15023, dal_acl_ruleEntryFieldCheck_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15024, dal_acl_ruleEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15025, dal_acl_ruleEntryFieldGetSet_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15026, dal_acl_ruleEntryFieldReadWrite_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15027, dal_acl_ruleOperation_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15028, dal_acl_ruleAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15029, dal_acl_templateSelector_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    UNIT_TEST_CASE(15030, dal_acl_userTemplate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_ACL),
    /*STP*/
    UNIT_TEST_CASE(16000, dal_stp_mstpInstance_create_test, SDKTEST_GRP_ALL|SDKTEST_GRP_STP),
    UNIT_TEST_CASE(16001, dal_stp_mstpState_test, SDKTEST_GRP_ALL|SDKTEST_GRP_STP),
    UNIT_TEST_CASE(16002, dal_stp_mstpState_test1, SDKTEST_GRP_ALL|SDKTEST_GRP_STP),

    /*Switch*/
    UNIT_TEST_CASE(19001, dal_switch_chksumFailAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19002, dal_switch_recalcCRCEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19003, dal_switch_mgmtMacAddr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19004, dal_switch_IPv4Addr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19005, dal_switch_maxPktLenLinkSpeed_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19006, dal_switch_maxPktLenTagLenCntIncEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19007, dal_switch_snapMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19008, dal_switch_cpuMaxPktLen_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19009, dal_switch_pppoePassthrough_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19010, dal_switch_deviceInfo_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),
    UNIT_TEST_CASE(19011, dal_switch_pkt2CpuTypeFormat_test, SDKTEST_GRP_ALL|SDKTEST_GRP_SWITCH),

    /*Trap*/
    UNIT_TEST_CASE(20006, dal_trap_rmaAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20010, dal_trap_userDefineRma_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20011, dal_trap_userDefineRmaAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20015, dal_trap_mgmtFrameAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20016, dal_trap_mgmtFramePri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20022, dal_trap_portMgmtFrameAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20033, dal_trap_cfmFrameTrapPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20038, dal_trap_rmaGroupAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20040, dal_trap_rmaLearningEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20041, dal_trap_rmaGroupLearningEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20042, dal_trap_bypassStp_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20043, dal_trap_bypassVlan_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20044, dal_trap_userDefineRmaEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20046, dal_trap_userDefineRmaLearningEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20048, dal_trap_mgmtFrameLearningEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20052, dal_trap_pktWithCFIAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20053, dal_trap_pktWithOuterCFIAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20054, dal_trap_pktWithCFIPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20057, dal_trap_oamPDUAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20058, dal_trap_oamPDUPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20061, dal_trap_cfmUnknownFrameAct_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20062, dal_trap_cfmLoopbackLinkTraceAct_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20063, dal_trap_cfmCcmAct_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20064, dal_trap_cfmEthDmAct_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20065, dal_trap_portOamLoopbackParAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20066, dal_trap_routeExceptionAction_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20067, dal_trap_routeExceptionPri_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),
    UNIT_TEST_CASE(20068, dal_trap_mgmtFrameMgmtVlanEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRAP),

    /*trunk test case*/
    //UNIT_TEST_CASE(21001, dal_trunk_port_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRUNK),
    UNIT_TEST_CASE(21007, dal_trunk_distributionAlgorithmParam_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRUNK),
    UNIT_TEST_CASE(21008, dal_trunk_distributionAlgorithmShift_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRUNK),
    UNIT_TEST_CASE(21009, dal_trunk_trafficSeparate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRUNK),
    UNIT_TEST_CASE(21010, dal_trunk_distributionAlgorithmBind_test, SDKTEST_GRP_ALL|SDKTEST_GRP_TRUNK),

   /* DAL VLAN test case */
    UNIT_TEST_CASE(22000, dal_vlan_table_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22002, dal_vlan_stg_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22004, dal_vlan_port_modify_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22005, dal_vlan_portInnerAcceptFrameType_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22008, dal_vlan_portEgrFilterEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22011, dal_vlan_portEgrInnerTpid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22016, dal_vlan_portEgrOuterTpid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22019, dal_vlan_portEgrTagKeepEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22020, dal_vlan_portIgrExtraTagEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22024, dal_vlan_mcastLeakyEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22027, dal_vlan_portIgrInnerTpid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),//
    UNIT_TEST_CASE(22028, dal_vlan_portIgrOuterTpid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22029, dal_vlan_portIgrTagKeepEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22030, dal_vlan_portOuterAcceptFrameType_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22032, dal_vlan_portOuterPvid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22033, dal_vlan_portProtoVlan_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),//
    UNIT_TEST_CASE(22034, dal_vlan_portInnerPvid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22036, dal_vlan_protoGroup_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22038, dal_vlan_l2UcastLookupMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22039, dal_vlan_l2McastLookupMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22040, dal_vlan_profileIdx_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22041, dal_vlan_profile_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22042, dal_vlan_portIgrFilter_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22044, dal_vlan_portInnerPvidMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22045, dal_vlan_portOuterPvidMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22046, dal_vlan_innerTpidEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22047, dal_vlan_outerTpidEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22048, dal_vlan_extraTpidEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22049, dal_vlan_portEgrInnerTagSts_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22050, dal_vlan_portEgrOuterTagSts_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22051, dal_vlan_igrVlanCnvtBlkMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22052, dal_vlan_egrVlanCnvtDblTagEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22053, dal_vlan_egrVlanCnvtVidSource_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22054, dal_vlan_portVlanAggrEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22055, dal_vlan_leakyStpFilter_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22056, dal_vlan_except_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22057, dal_vlan_portIgrCnvtDfltAct_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22058, dal_vlan_portIgrTagKeepType_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22059, dal_vlan_portEgrTagKeepType_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22060, dal_vlan_portVlanAggrVidSource_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22061, dal_vlan_portVlanAggrPriTagVidSource_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22062, dal_vlan_portEgrVlanCnvtVidSource_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22063, dal_vlan_portEgrVlanCnvtVidTarget_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22064, dal_vlan_portEgrVlanCnvtLookupMissAct_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22065, dal_vlan_macBasedVlan_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22066, dal_vlan_igrVlanCnvtEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    //UNIT_TEST_CASE(22067, dal_vlan_egrVlanCnvtEntry_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22068, dal_vlan_igrVlanCnvtEntry_delAll_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    //UNIT_TEST_CASE(22069, dal_vlan_egrVlanCnvtEntry_delAll_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),
    UNIT_TEST_CASE(22070, dal_vlan_egrVlanCnvtRangeCheckVid_test, SDKTEST_GRP_ALL|SDKTEST_GRP_VLAN),

#if (!defined(CONFIG_SDK_MODEL_MODE) && !defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE))
    /* NIC Test Case */
    UNIT_TEST_CASE(25000, nic_case2_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25001, nic_case3_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25002, nic_case4_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25003, nic_case5_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25004, nic_case6_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25005, nic_case7_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25006, nic_case8_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25007, nic_case9_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25008, nic_case10_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25009, nic_case11_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25010, nic_case12_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25011, nic_case13_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
    UNIT_TEST_CASE(25050, nic_pausePktPerformance_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),

    /* OSAL Test Case */
    UNIT_TEST_CASE(26000, osal_memory_cache_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OSAL),
    UNIT_TEST_CASE(26001, osal_isr_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OSAL),
    UNIT_TEST_CASE(26002, osal_time_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OSAL),
    UNIT_TEST_CASE(26003, osal_thread_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OSAL),
    UNIT_TEST_CASE(26004, osal_mutex_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OSAL),
    UNIT_TEST_CASE(26005, osal_sem_test, SDKTEST_GRP_ALL|SDKTEST_GRP_OSAL),
#endif

#if 0
    UNIT_TEST_CASE(25060, nic_8380_case0_test, SDKTEST_GRP_ALL|SDKTEST_GRP_NIC),
#endif
    /* Statistic Test Case */
    UNIT_TEST_CASE(27000, dal_stat_readPerformance_test, SDKTEST_GRP_ALL|SDKTEST_GRP_STATS),
    //UNIT_TEST_CASE(27001, dal_stat_readPerformance2_test, SDKTEST_GRP_ALL|SDKTEST_GRP_STATS),
    UNIT_TEST_CASE(27002, dal_stat_tagLenCntIncEnable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_STATS),
    UNIT_TEST_CASE(27003, dal_stat_enable_test, SDKTEST_GRP_ALL|SDKTEST_GRP_STATS),

    /* Mirror Test Case */
    UNIT_TEST_CASE(28000, dal_mirror_group_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28001, dal_mirror_rspanIgrMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28002, dal_mirror_rspanEgrMode_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28003, dal_mirror_rspanTag_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28004, dal_mirror_sflowMirrorSampleRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28005, dal_mirror_sflowPortIgrSampleRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28006, dal_mirror_sflowPortEgrSampleRate_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),
    UNIT_TEST_CASE(28007, dal_mirror_sflowSampleCtrl_test, SDKTEST_GRP_ALL|SDKTEST_GRP_MIRROR),

    /* Final case, DO NOT remove it */
    UNIT_TEST_CASE(100000, NULL, SDKTEST_GRP_ALL),
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
    int32 noSupportCase = 0;
    int32 failCase = 0;
    int32 failCaseIdx[sizeof(unitTestCase) / sizeof(unitTestCase[0])];
    uint32 groupmask = SDKTEST_GRP_ALL;

    lower = 0;
    upper = unitTestCase[sizeof(unitTestCase) / sizeof(unitTestCase[0]) - 1].no;

    if (!osal_strcmp((const char*)pStr, "all"))
        groupmask = SDKTEST_GRP_ALL;
    else if (!osal_strcmp((const char*)pStr, "port"))
        groupmask = SDKTEST_GRP_PORT;
    else if (!osal_strcmp((const char*)pStr, "hal"))
        groupmask = SDKTEST_GRP_HAL;
    else if (!osal_strcmp((const char*)pStr, "eee"))
        groupmask = SDKTEST_GRP_EEE;
    else if (!osal_strcmp((const char*)pStr, "flowctrl"))
        groupmask = SDKTEST_GRP_FLOWCTRL;
    else if (!osal_strcmp((const char*)pStr, "l2"))
        groupmask = SDKTEST_GRP_L2;
    else if (!osal_strcmp((const char*)pStr, "l3"))
        groupmask = SDKTEST_GRP_L3;
    else if (!osal_strcmp((const char*)pStr, "oam"))
        groupmask = SDKTEST_GRP_OAM;
    else if (!osal_strcmp((const char*)pStr, "qos"))
        groupmask = SDKTEST_GRP_QOS;
    else if (!osal_strcmp((const char*)pStr, "rate"))
        groupmask = SDKTEST_GRP_RATE;
    else if (!osal_strcmp((const char*)pStr, "sec"))
        groupmask = SDKTEST_GRP_SEC;
    else if (!osal_strcmp((const char*)pStr, "stp"))
        groupmask = SDKTEST_GRP_STP;
    else if (!osal_strcmp((const char*)pStr, "switch"))
        groupmask = SDKTEST_GRP_SWITCH;
    else if (!osal_strcmp((const char*)pStr, "trap"))
        groupmask = SDKTEST_GRP_TRAP;
    else if (!osal_strcmp((const char*)pStr, "trunk"))
        groupmask = SDKTEST_GRP_TRUNK;
    else if (!osal_strcmp((const char*)pStr, "vlan"))
        groupmask = SDKTEST_GRP_VLAN;
    else if (!strcmp((const char*)pStr, "nic"))
        groupmask = SDKTEST_GRP_NIC;
    else if (!strcmp((const char*)pStr, "osal"))
        groupmask = SDKTEST_GRP_OSAL;
    else if (!strcmp((const char*)pStr, "stats"))
        groupmask = SDKTEST_GRP_STATS;
    else if (!strcmp((const char*)pStr, "mirror"))
        groupmask = SDKTEST_GRP_MIRROR;
    else if (!strcmp((const char*)pStr, "mpls"))
        groupmask = SDKTEST_GRP_MPLS;
    else if (!strcmp((const char*)pStr, "time"))
        groupmask = SDKTEST_GRP_TIME;
    else if (!strcmp((const char*)pStr, "led"))
        groupmask = SDKTEST_GRP_LED;
    else if (!strcmp((const char*)pStr, "acl"))
        groupmask = SDKTEST_GRP_ACL;

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

        ret = pCase->fp(pCase->no, unit);

        if (ret == RT_ERR_OK)
        {
            osal_printf(" ok!\n\n");
            okCase++;
        }
        else if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
        {
            osal_printf(" no supported!\n\n");
            noSupportCase++;
        }
        else
        {
            osal_printf("\033[31;43m fail!! \033[m\n\n");
            failCaseIdx[failCase] = i;
            failCase++;
        }

    }

    osal_printf(">>Total Case: %d,  Pass: %d,  NoSupport: %d,  Fail: %d\n\n", totalCase, okCase, noSupportCase, failCase);
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
    int32 noSupportCase = 0;
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

        ret = pCase->fp(pCase->no, unit);

        if (ret == RT_ERR_OK)
        {
            osal_printf(" ok!\n\n");
            okCase++;
        }
        else if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
        {
            osal_printf(" no supported!\n\n");
            noSupportCase++;
        }
        else
        {
            osal_printf("\033[31;43m fail!! \033[m\n\n");
            failCaseIdx[failCase] = i;
            failCase++;
        }

    }

    osal_printf(">>Total Case: %d,  Pass: %d,  NoSupport: %d,  Fail: %d\n\n", totalCase, okCase, noSupportCase, failCase);
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
