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

#ifndef __DAL_FLOWCTRL_TEST_CASE_H__
#define __DAL_FLOWCTRL_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_flowctrl_portPauseOnAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_portPauseOnAllowedPageNum_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_pauseOnAllowedPktNum_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_igrSystemPauseThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_igrSystemCongestThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_egrSystemDropThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_egrPortDropThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_portEgrDropRefCongestEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_pauseOnAllowedPktLen_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_igrPauseThreshGroup_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_portIgrPortThreshGroupSel_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_igrCongestThreshGroup_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_egrQueueDropThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_egrCpuQueueDropThresh_test(uint32 caseNo, uint32 unit);
extern int32 dal_flowctrl_egrPortQueueDropEnable_test(uint32 caseNo, uint32 unit);


#endif/*__DAL_FLOWCTRL_TEST_CASE_H__*/

