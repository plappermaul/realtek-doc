/*
 * Copyright (C) 2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PHY 826XB/8261N Driver APIs.
 *
 * Feature : PHY 826XB/8261N Driver APIs.
 *
 */
#ifndef __HAL_PHY_PHY_RTL826XB_H__
#define __HAL_PHY_PHY_RTL826XB_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>

/*
 * Symbol Definition
 */
//#define PHY_826X_CONSTRUCT_BROADCAST  1

#define PHY_826XB_LED_MODE_L_10G              (1 << 0)
#define PHY_826XB_LED_MODE_L_10G_LITE         (1 << 1)
#define PHY_826XB_LED_MODE_L_5G               (1 << 2)
#define PHY_826XB_LED_MODE_L_5G_LITE          (1 << 3)
#define PHY_826XB_LED_MODE_L_2P5G             (1 << 4)
#define PHY_826XB_LED_MODE_L_2P5G_LITE        (1 << 5)
#define PHY_826XB_LED_MODE_L_1G               (1 << 6)
#define PHY_826XB_LED_MODE_L_500M             (1 << 7)
#define PHY_826XB_LED_MODE_L_100M             (1 << 8)
#define PHY_826XB_LED_MODE_L_10G_FLASH        (1 << 9)
#define PHY_826XB_LED_MODE_L_10G_LITE_FLASH   (1 << 10)
#define PHY_826XB_LED_MODE_L_5G_FLASH         (1 << 11)
#define PHY_826XB_LED_MODE_L_5G_LITE_FLASH    (1 << 12)
#define PHY_826XB_LED_MODE_L_2P5G_FLASH       (1 << 13)
#define PHY_826XB_LED_MODE_L_2P5G_LITE_FLASH  (1 << 14)
#define PHY_826XB_LED_MODE_L_1G_FLASH         (1 << 15)

#define PHY_826XB_LED_MODE_H_500M_FLASH       (1 << 0)
#define PHY_826XB_LED_MODE_H_100M_FLASH       (1 << 1)
#define PHY_826XB_LED_MODE_H_RX_ACT           (1 << 2)
#define PHY_826XB_LED_MODE_H_TX_ACT           (1 << 3)
#define PHY_826XB_LED_MODE_H_LITE_FLASH       (1 << 4)
#define PHY_826XB_LED_MODE_H_LITE             (1 << 5)
#define PHY_826XB_LED_MODE_H_DUPLEX           (1 << 6)
#define PHY_826XB_LED_MODE_H_MASTER           (1 << 7)
#define PHY_826XB_LED_MODE_H_TRAINING         (1 << 8)
#define PHY_826XB_LED_MODE_H_LINK_EN          (1 << 9)

/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8261B_info;
extern rt_phyInfo_t phy_8264B_info;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_826XBdrv_mapperInit
 * Description:
 *      Initialize PHY 826XB driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_826XBdrv_mapperInit(rt_phydrv_t *pPhydrv);

/* Function Name:
 *      phy_826xb_init
 * Description:
 *      Initialize PHY.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_826xb_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_826xb_ctrl_set
 * Description:
 *      Set PHY specific settings.
 * Input:
 *      unit      - unit id
 *      port      - port id
  *     ctrl_type - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
extern int32
phy_826xb_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value);

/* Function Name:
 *      phy_826xb_ctrl_get
 * Description:
 *      Get PHY specific settings.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
extern int32
phy_826xb_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue);

extern int32
_phy_826xb_serdes_reg_read(uint32 unit, rtk_port_t port, uint32 page, uint32 reg, uint32 *pData);

extern int32
_phy_826xb_serdes_reg_write(uint32 unit, rtk_port_t port, uint32 page, uint32 reg, uint32 data);

extern int32
phy_826xb_eeePhymodeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
phy_826xb_eeePhymodeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

#endif /* __HAL_PHY_PHY_RTL826XB_H__ */