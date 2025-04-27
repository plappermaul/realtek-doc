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

#ifndef __DAL_QOS_TEST_CASE_H__
#define __DAL_QOS_TEST_CASE_H__

#include <common/rt_type.h>


extern int32 dal_qos_priSelGroup_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portPriSelGroup_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portInnerPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portOuterPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dscpPriRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_1pPriRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_queueNum_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_1pRemarkEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_1pRemark_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_out1pRemarkEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dscpRemarkEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dscpRemark_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_schedulingAlgorithm_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_schedulingQueue_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_congAvoidAlgo_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_congAvoidSysThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dpSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portDEISrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_deiDpRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dscpDpRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_outer1pPriRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_priMap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_1pDfltPri_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_1pRemarkSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_outer1pRemark_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_outer1pRemarkSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portOuter1pDfltPriSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dscp2DscpRemark_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_dscpRemarkSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_deiRemark_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_deiRemarkEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portDeiRemarkTagSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_congAvoidSysDropProbability_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_congAvoidGlobalQueueThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_congAvoidGlobalQueueDropProbability_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portAvbStreamReservationClassEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_avbStreamReservationConfig_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_pkt2CpuPriRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_rspanPriRemap_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portPri2IgrQMapEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_1pDfltPriSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_portOuter1pRemarkSrcSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_qos_outer1pDfltPri_test(uint32 caseNo, uint32 unit);


#endif/*__DAL_QOS_TEST_CASE_H__*/

