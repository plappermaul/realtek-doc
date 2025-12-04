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
 * Purpose : L34 Driver test case
 *
 * Feature : test L34 API
 *
 */

#ifndef __DAL_QOS_TEST_CASE_H__
#define __DAL_QOS_TEST_CASE_H__


int32 dal_qos_priSelGroup_test(uint32 testcase);
int32 dal_qos_portPri_test(uint32 caseNo);
int32 dal_qos_dscpPriRemapGroup_test(uint32 caseNo);
int32 dal_qos_1pPriRemapGroup_test(uint32 caseNo);
int32 dal_qos_priMap_test(uint32 caseNo);
int32 dal_qos_portPriMap_test(uint32 caseNo);
int32 dal_qos_1pRemarkEnable_test(uint32 caseNo);
int32 dal_qos_1pRemarkGroup_test(uint32 caseNo);
int32 dal_qos_dscpRemarkEnable_test(uint32 caseNo);
int32 dal_qos_dscpRemarkGroup_test(uint32 caseNo);
int32 dal_qos_portDscpRemarkSrcSel_test(uint32 caseNo);
int32 dal_qos_dscp2DscpRemarkGroup_test(uint32 caseNo);
int32 dal_qos_fwd2CpuPriRemap_test(uint32 caseNo);
int32 dal_qos_schedulingQueue_test(uint32 caseNo);

#endif /* __DAL_QOS_TEST_CASE_H__ */

