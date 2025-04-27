/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of DAL test APIs in the SDK
 *
 * Feature : DAL test APIs
 *
 */

#ifndef __DAL_PORT_TEST_CASE_H__
#define __DAL_PORT_TEST_CASE_H__

#include <common/rt_type.h>

#if 0
extern int32 dal_port_phyAutoNegoEnable_test(uint32 caseNo, uint32 unit);
#endif

extern int32 dal_port_isolation_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_isolation_add_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_backpressureEnable_test(uint32 caseNo, uint32 unit);
#if 0
extern int32 dal_port_adminEnable_test(uint32 caseNo, uint32 unit);
#endif
extern int32 dal_port_adminEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_linkMon_swScanPorts_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_vlanBasedIsolationEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_vlanBasedIsolation_vlanSource_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_rxEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_specialCongest_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_link_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_speedDuplex_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_flowctrl_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_cpuPortId_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_phyAutoNegoEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_phyAutoNegoAbility_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_phyMasterSlave_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_phyForceModeAbility_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_phyComboPortMedia_test(uint32 caseNo, uint32 unit);
extern int32 dal_port_txEnable_test(uint32 caseNo, uint32 unit);



#endif/*__DAL_PORT_TEST_CASE_H__*/

