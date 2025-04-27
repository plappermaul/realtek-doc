/*
 * Copyright (C) 2009-2022 Realtek Semiconductor Corp.
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
 * Purpose : PHY 8224 HW patch APIs.
 *
 * Feature : PHY 8224 HW patch APIs
 *
 */


#ifndef __HAL_PHY_PHY_RTL8224_PATCH_H__
#define __HAL_PHY_PHY_RTL8224_PATCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/* Function Name:
 *      phy_rtl8224_sdsReg_get
 * Description:
 *      Get SerDes Register
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      sdsPage     - the SerDes page
 *      sdsReg      - the SerDes register
 * Output:
 *      pData       - return register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 *      RT_ERR_ABORT
 * Note:
 *      None
 */
extern int32
phy_rtl8224_sdsReg_get(uint32 unit, rtk_port_t baseport, uint32 sdsPage, uint32 sdsReg, uint32 *pData);

/* Function Name:
 *      phy_rtl8224_sdsReg_set
 * Description:
 *      Set SerDes Register
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      sdsPage     - the SerDes page
 *      sdsReg      - the SerDes register
 *      wData       - the write data
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

extern int32
phy_rtl8224_sdsReg_set(uint32 unit, rtk_port_t baseport, uint32 sdsPage, uint32 sdsReg, uint32 wData);


/* Function Name:
 *      phy_rtl8224_patch
 * Description:
 *      Apply patch data to PHY
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      portOffset  - the index offset base on baseport for the port to patch
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
extern int32
phy_rtl8224_patch(uint32 unit, uint8 baseport, uint8 portOffset);

/* Function Name:
 *      phy_rtl8224_broadcast_patch
 * Description:
 *      Apply patch data to PHY
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      portOffset  - the index offset base on baseport for the port to patch
 *      perChip     - 1 for per-chip mode, 0 for per-bus mode
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
extern int32
phy_rtl8224_broadcast_patch(uint32 unit, uint8 port, uint8 portOffset, uint8 perChip);


#endif /* __HAL_PHY_PHY_RTL8224_PATCH_H__ */

