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
 * Purpose : GMac Driver Alarm Management
 *
 * Feature : GMac Driver Alarm Management
 *
 */

#ifndef __GPON_ALM_H__
#define __GPON_ALM_H__

int32 gpon_alarm_event(gpon_dev_obj_t* obj, rtk_gpon_alarm_type_t alarm, int32 status);

int32 gpon_alarm_status_get(gpon_dev_obj_t* obj, rtk_gpon_alarm_type_t alarm, int32 *pStatus);

void gpon_alarm_status_check(gpon_dev_obj_t* obj);

void gpon_alarm_status_reset(gpon_dev_obj_t* obj);

#endif  /* __GPON_ALM_H__ */

