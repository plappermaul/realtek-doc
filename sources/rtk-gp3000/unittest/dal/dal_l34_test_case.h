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

#ifndef __DAL_L34_TEST_CASE_H__
#define __DAL_L34_TEST_CASE_H__


int32 dal_l34_netifTable_test(uint32 testcase);
int32 dal_l34_arpTable_test(uint32 testcase);
int32 dal_l34_pppoeTable_test(uint32 testcase);
int32 dal_l34_routingTable_test(uint32 testcase);
int32 dal_l34_nexthopTable_test(uint32 testcase);
int32 dal_l34_extIntIPTable_test(uint32 testcase);
int32 dal_l34_naptInboundTable_test(uint32 testcase);
int32 dal_l34_naptOutboundTable_test(uint32 testcase);

#endif /* __DAL_L34_TEST_CASE_H__ */
