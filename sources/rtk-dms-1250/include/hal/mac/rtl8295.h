/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : PHY 8295 Driver APIs.
 *
 * Feature : PHY 8295 Driver APIs
 *
 */
#ifndef __HAL_RTL8295_H__
#define __HAL_RTL8295_H__

/*
 * Include Files
 */


/*
 * Symbol Definition
 */
#define PHY_8295_MODE_A1                0x1
#define PHY_8295_MODE_B0                0x4
#define PHY_8295_MODE_C3                0xB
#define PHY_8295_MODE_C5                0xD

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
extern int32
hal_rtl8295_reg_read(uint32 unit, rtk_port_t mdxMacId, uint32 reg_addr, uint32 *pData);

extern int32
hal_rtl8295_reg_write(uint32 unit, rtk_port_t mdxMacId, uint32 reg_addr, uint32 data);

extern int32
hal_rtl8295_sds_read(uint32 unit, rtk_port_t mdxMacId, uint32 sds_idx, uint32 page, uint32 reg, uint32 *pData);

extern int32
hal_rtl8295_sds_write(uint32 unit, rtk_port_t mdxMacId, uint32 sds_idx, uint32 page, uint32 reg, uint32 data);

extern int32
hal_rtl8295_mdxMacId_get(uint32 unit, rtk_port_t port, uint32 *pMdxMacId);

#endif /* __HAL_RTL8295_H__ */
