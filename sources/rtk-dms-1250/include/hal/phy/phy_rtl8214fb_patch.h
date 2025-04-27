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
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL8214FB_PATCH_H__
#define __HAL_PHY_PHY_RTL8214FB_PATCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/* Function Name:
 *      sub_phy_8214fb_patch_set
 * Description:
 *      Patch the PHY:8214fb.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
sub_phy_8214fb_patch_set(uint32 unit, rtk_port_t port);


#endif /* __HAL_PHY_PHY_RTL8214FB_PATCH_H__ */
