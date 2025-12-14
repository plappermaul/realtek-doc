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
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : TRAP SDK test case
 *
 * Feature : Provide the APIs to access TRAP
 *
 */

#ifndef __DAL_TRAP_TEST_CASE_H__
#define __DAL_TRAP_TEST_CASE_H__


extern int32 dal_trap_igmpCtrlPkt2CpuEnable_test(uint32 caseNo);
extern int32 dal_trap_ipMcastPkt2CpuEnable_test(uint32 caseNo);
extern int32 dal_trap_l2McastPkt2CpuEnable_test(uint32 caseNo);
extern int32 dal_trap_mldCtrlPkt2CpuEnable_test(uint32 caseNo);
extern int32 dal_trap_oamPduAction_test(uint32 caseNo);
extern int32 dal_trap_oamPduPri_test(uint32 caseNo);
extern int32 dal_trap_reasonTrapToCpuPriority_test(uint32 caseNo);
extern int32 dal_trap_rmaAction_test(uint32 caseNo);
extern int32 dal_trap_rmaKeepCtagEnable_test(uint32 caseNo);
extern int32 dal_trap_rmaPortIsolationEnable_test(uint32 caseNo);
extern int32 dal_trap_rmaPri_test(uint32 caseNo);
extern int32 dal_trap_rmaStormControlEnable_test(uint32 caseNo);
extern int32 dal_trap_rmaVlanCheckEnable_test(uint32 caseNo);
extern int32 dal_trap_portIgmpMldCtrlPktAction_test(uint32 caseNo);
extern int32 dal_trap_uniTrapPriorityEnable_test(uint32 caseNo);
extern int32 dal_trap_uniTrapPriorityPriority_test(uint32 caseNo);

#endif /* __DAL_TRAP_TEST_CASE_H__ */
