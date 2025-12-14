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
 * Purpose : LED Driver test case
 *
 * Feature : test LED API
 *
 */

#ifndef __DAL_LED_TEST_CASE_H__
#define __DAL_LED_TEST_CASE_H__

int32 dal_led_operation_test(uint32 testcase);
int32 dal_led_serialMode_test(uint32 testcase);
int32 dal_led_blinkRate_test(uint32 testcase);
int32 dal_led_config_test(uint32 testcase);
int32 dal_led_modeForce_test(uint32 testcase);

#endif /* __DAL_LED_TEST_CASE_H__ */

