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
#include <linux/module.h>
#include <linux/kernel.h>

#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <osal/print.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/mac_debug.h>
#include <hal/chipdef/chip.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/driver.h>
#include <rtk/init.h>
#include <rtk/eee.h>
#include <rtk/flowctrl.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/mirror.h>
#include <rtk/oam.h>
#include <rtk/pie.h>
#include <rtk/acl.h>
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
#include <rtk/led.h>
#include <rtk/mpls.h>
#include <rtk/diag.h>

#if defined(CONFIG_SDK_DRIVER_TEST_MODULE) || defined(CONFIG_SDK_DRIVER_TEST)
#include <hal/common/miim.h>
#include <hal/phy/identify.h>
#include <hal/mac/mem.h>
#endif

#if defined(CONFIG_SDK_RTL8390)
#include <common/util/rt_bitop.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/mac/mac_probe.h>
#include <dal/dal_mgmt.h>
#include <dal/cypress/dal_cypress_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL8380)
#include <common/util/rt_bitop.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/mac/mac_probe.h>
#include <dal/maple/dal_maple_l2.h>
#include <dal/maple/dal_maple_acl.h>
#include <dal/dal_mgmt.h>
#include <dal/maple/dal_maple_mapper.h>
#include <dal/maple/dal_maple_qos.h>
#include <dal/maple/dal_maple_time.h>
#endif

#if defined(CONFIG_SDK_RTL9300)
#include <common/util/rt_bitop.h>
#include <hal/mac/mac_probe.h>
#include <dal/dal_mgmt.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <dal/longan/dal_longan_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL9310)
#include <common/util/rt_bitop.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/mac/mac_probe.h>
#include <dal/dal_mgmt.h>
#include <dal/mango/dal_mango_mapper.h>
#include <hal/mac/drv/drv_rtl9310.h>
#endif


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
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
//EXPORT_SYMBOL(dal_mgmt_init);
//EXPORT_SYMBOL(dal_mgmt_initDevice);
//EXPORT_SYMBOL(reg_read);
//EXPORT_SYMBOL(reg_write);
EXPORT_SYMBOL(reg_field_get);
EXPORT_SYMBOL(reg_field_set);
//EXPORT_SYMBOL(mac_probe);
EXPORT_SYMBOL(reg_array_read);
EXPORT_SYMBOL(reg_array_write);
//EXPORT_SYMBOL(mac_init);
EXPORT_SYMBOL(rt_bitop_numberOfSetBits);
//EXPORT_SYMBOL(rt_bitop_findFirstBitInAaray);
#endif

#if defined(CONFIG_SDK_RTL8390)
EXPORT_SYMBOL(dal_cypress_init);
EXPORT_SYMBOL(rtk_cypress_table_list);
#endif

#if defined(CONFIG_SDK_RTL9300)
EXPORT_SYMBOL(rtk_longan_reg_list);
EXPORT_SYMBOL(rtk_longan_table_list);
#endif

#if defined(CONFIG_SDK_RTL9310)
EXPORT_SYMBOL(rtk_mango_table_list);
EXPORT_SYMBOL(rtk_portmask2bitPortmask_get);
EXPORT_SYMBOL(drv_rtl9310_regAddrField_write);
#endif

#if defined (CONFIG_SDK_RTL8380)
//EXPORT_SYMBOL(reg_field_read);
//EXPORT_SYMBOL(reg_field_write);
//EXPORT_SYMBOL(rtk_qos_portPri2IgrQMapEnable_get);
//EXPORT_SYMBOL(rtk_qos_rspanPriRemap_get);
EXPORT_SYMBOL(rtk_maple_table_list);
//EXPORT_SYMBOL(rtk_switch_snapMode_get);
EXPORT_SYMBOL(dal_maple_acl_lookupMissAct_get);
EXPORT_SYMBOL(dal_maple_acl_statPktCnt_set);
EXPORT_SYMBOL(dal_maple_l2_getL2EntryfromHash);
EXPORT_SYMBOL(dal_maple_acl_statByteCnt_set);
//EXPORT_SYMBOL(rtk_vlan_portVlanAggrVidSource_get);
//EXPORT_SYMBOL(rtk_trap_rmaGroupAction_get);
EXPORT_SYMBOL(dal_maple_acl_portLookupEnable_get);
//EXPORT_SYMBOL(rtk_trap_rmaGroupAction_set);
EXPORT_SYMBOL(dal_maple_acl_statPktCnt_get);
EXPORT_SYMBOL(dal_maple_init);
//EXPORT_SYMBOL(rtk_qos_portPri2IgrQMap_get);
EXPORT_SYMBOL(dal_maple_acl_ruleEntry_read);
//EXPORT_SYMBOL(rtk_l2_legalPortMoveFlushAddrEnable_get);
//EXPORT_SYMBOL(rtk_l2_secureMacMode_get);
//EXPORT_SYMBOL(dal_maple_acl_rngChkCtrl_get);
//EXPORT_SYMBOL(rtk_vlan_portVlanAggrPriTagVidSource_get);
//EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidSource_set);
//EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidSource_get);
//EXPORT_SYMBOL(rtk_vlan_portVlanAggrVidSource_set);
//EXPORT_SYMBOL(rtk_trap_rmaGroupLearningEnable_get);
EXPORT_SYMBOL(dal_maple_acl_statByteCnt_get);
EXPORT_SYMBOL(rtk_port_cpuPortId_set);
//EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidTarget_set);
//EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtVidTarget_get);
//EXPORT_SYMBOL(rtk_vlan_portVlanAggrPriTagVidSource_set);
//EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtLookupMissAct_set);
//EXPORT_SYMBOL(rtk_vlan_portEgrVlanCnvtLookupMissAct_get);
//EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepType_set);
//EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepType_get);
//EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepType_set);
//EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepType_get);
//EXPORT_SYMBOL(reg_array_field_read);

//EXPORT_SYMBOL(rtk_qos_portPri2IgrQMapEnable_set);
//EXPORT_SYMBOL(rtk_l2_legalPortMoveFlushAddrEnable_set);
//EXPORT_SYMBOL(rtk_qos_rspanPriRemap_set);
//EXPORT_SYMBOL(reg_array_field_write);
//EXPORT_SYMBOL(rtk_switch_snapMode_set);
//EXPORT_SYMBOL(rtk_qos_portPri2IgrQMap_set);
//EXPORT_SYMBOL(rtk_l2_secureMacMode_set);
//EXPORT_SYMBOL(rtk_l2_ipMcstFidVidCompareEnable_set);


/*vv-Add for RTL8380 Module code test by Daniel*/
EXPORT_SYMBOL(dal_maple_l2_portMacLimitEnable_set);
EXPORT_SYMBOL(dal_maple_l2_fidMacLimitEnable_get);
EXPORT_SYMBOL(dal_maple_l2_macLimitEnable_get);
EXPORT_SYMBOL(dal_maple_l2_fidMacLimitEnable_set);
EXPORT_SYMBOL(dal_maple_l2_portMacLimitEnable_get);
EXPORT_SYMBOL(dal_maple_l2_macLimitEnable_set);

/*time*/
EXPORT_SYMBOL(dal_maple_time_refTimeForceEnable_get);
EXPORT_SYMBOL(dal_maple_time_refTimeForceEnable_set);
EXPORT_SYMBOL(dal_maple_time_itagTpid_get);
EXPORT_SYMBOL(dal_maple_time_itagTpid_set);
EXPORT_SYMBOL(dal_maple_time_otagTpid_get);
EXPORT_SYMBOL(dal_maple_time_otagTpid_set);
EXPORT_SYMBOL(dal_maple_time_mac_get);
EXPORT_SYMBOL(dal_maple_time_mac_set);
EXPORT_SYMBOL(dal_maple_time_offLoadEnable_get);
EXPORT_SYMBOL(dal_maple_time_offLoadEnable_set);
EXPORT_SYMBOL(dal_maple_time_offLoadSaveTsEnable_get);
EXPORT_SYMBOL(dal_maple_time_offLoadSaveTsEnable_set);
EXPORT_SYMBOL(dal_maple_time_portIntFlag_get);
EXPORT_SYMBOL(dal_maple_time_rxIntEnable_get);
EXPORT_SYMBOL(dal_maple_time_rxIntEnable_set);
EXPORT_SYMBOL(dal_maple_time_txIntEnable_get);
EXPORT_SYMBOL(dal_maple_time_txIntEnable_set);
EXPORT_SYMBOL(dal_maple_time_portRxIntFlag_get);
EXPORT_SYMBOL(dal_maple_time_portRxIntFlag_clear);
EXPORT_SYMBOL(dal_maple_time_portTxIntFlag_get);
EXPORT_SYMBOL(dal_maple_time_portTxIntFlag_clear);


EXPORT_SYMBOL(dal_maple_template_field_list);


/*^^-Add for RTL8380 Module code test by Daniel*/

#endif /*end of CONFIG_SDK_RTL8380*/


#if defined(__MODEL_USER__)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
 EXPORT_SYMBOL(reg_model_write_register);
 #endif
#endif

