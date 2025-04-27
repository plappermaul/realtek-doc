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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : PHY 8218E Driver APIs.
 *
 * Feature : PHY 8218E Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL8218E_H__
#define __HAL_PHY_PHY_RTL8218E_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8218E                   8

#define PHY_8218E_REV_ID_OFFSET             0
#define PHY_8218E_REV_ID_MASK               (0x7F << PHY_8218E_REV_ID_OFFSET)
#define PHY_8218E_CHIP_ID_MAP_OFFSET        7
#define PHY_8218E_CHIP_ID_MAP_MASK          (0x7 << PHY_8218E_CHIP_ID_MAP_OFFSET)

#define PHY_8218E_DUMMY_REG_PAGE            0x036E
#define PHY_8218E_DUMMY_REG_REG             16
#define PHY_8218E_DUMMY_REG_VALUE           (0x8888)

#define PHY_RTL8218E_VER_C                  2
#define PHY_RTL8218E_VER_D                  3


/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8218E_info;


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_8218Edrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8218E driver.
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
phy_8218Edrv_ge_mapperInit(rt_phydrv_t *pPhydrv);


/* Function Name:
 *      phy_8218e_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pChip_rev_id    - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8218e_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *pChip_rev_id);

/* Function Name:
 *      phy_8218e_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS only support in mode 4
 */
extern int32
phy_8218e_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);


/* Function Name:
 *      phy_8218e_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218e_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      phy_8218e_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC interface SerDes mode
 * Input:
 *      unit   - unit id
 *      port   - PHY's base mac id
 * Output:
 *      pMode   - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218e_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode);

/* Function Name:
 *      phy_8218e_macIntfSerdes_reset
 * Description:
 *      Reset PHY's MAC (system) interface
 * Input:
 *      unit        - unit id
 *      port        - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218e_macIntfSerdes_reset(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8218e_serdes_linkdown_chk
 * Description:
 *      check phy serdes linkdown status
 * Input:
 *      unit        - unit id
 *      port        - base mac id
 * Output:
 *      pStatus     - pointer to serdes linkdown status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218e_serdes_linkdown_chk(uint32 unit, rtk_port_t port, uint32 *pStatus);


/* Function Name:
 *      phy_8218e_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 * Output:
 *      pEyeParam - eye parameter.
 *                  impedance is not supported.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
extern int32
phy_8218e_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

/* Function Name:
 *      phy_8218e_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 *      pEyeParam - eye parameter.
 *                  impedance is not supported.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
extern int32
phy_8218e_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);


#endif /* __HAL_PHY_PHY_RTL8218E_H__ */

