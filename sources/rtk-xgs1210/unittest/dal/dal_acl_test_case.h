/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74758 $
 * $Date: 2016-12-28 16:08:11 +0800 (Wed, 28 Dec 2016) $
 *
 * Purpose : Definition of DAL test APIs in the SDK
 *
 * Feature : DAL test APIs
 *
 */

#ifndef __DAL_ACL_TEST_CASE_H__
#define __DAL_ACL_TEST_CASE_H__

#include <common/rt_type.h>

extern int32 dal_acl_partition_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_blockPwrEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_blockLookupEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleEntryFieldSize_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleValidate_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_blockResultMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_blockGroupEnable_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_statPktCnt_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_statByteCnt_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_statClear_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_rangeCheckL4Port_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_rangeCheckVid_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_rangeCheckIp_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_rangeCheckSrcPort_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_rangeCheckDstPort_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_rangeCheckPacketLen_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_fieldSelector_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_portFieldSelector_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_meterMode_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_meterIncludeIfg_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_meterBurstSize_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_meterExceed_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_meterEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleEntryFieldCheck_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleEntry_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleEntryFieldGetSet_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleEntryFieldReadWrite_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleOperation_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_ruleAction_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_templateSelector_test(uint32 caseNo, uint32 unit);
extern int32 dal_acl_userTemplate_test(uint32 caseNo, uint32 unit);

#endif/*__DAL_ACL_TEST_CASE_H__*/
