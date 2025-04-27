/*
 * Copyright (C) 2009-2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : PHY 826XB/8261N HW patch APIs.
 *
 * Feature : PHY 826XB/8261N HW patch APIs
 *
 */


#ifndef __HAL_PHY_PHY_RTL826XB_PATCH_H__
#define __HAL_PHY_PHY_RTL826XB_PATCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/* Function Name:
 *      phy_rtl826xb_patch
 * Description:
 *      apply patch data to PHY
 * Input:
 *      unit       - unit id
 *      baseport   - base port id on the PHY chip
 *      portOffset - the index offset base on baseport for the port to patch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 *      RT_ERR_ABORT
 * Note:
 *      None
 */
extern int32 phy_rtl826xb_patch(uint32 unit, uint8 baseport, uint8 portOffset);
extern int32 phy_rtl826xb_broadcast_patch(uint32 unit, uint8 port, uint8 portOffset);

#endif /* __HAL_PHY_PHY_RTL826XB_PATCH_H__ */
