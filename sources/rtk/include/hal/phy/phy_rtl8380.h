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
 * Purpose : MAC 8380 internal PHY Driver APIs.
 *
 * Feature : MAC 8380 internal PHY Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL8380_H__
#define __HAL_PHY_PHY_RTL8380_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8380SDS   (1)

/*
 * Data Declaration
 */
extern rt_phydrv_t phy_8380_serdes_ge;
extern rt_phyInfo_t phy_8380sds_info;

/* Function Name:
 *      phy_8380_serdes_ge_mapperInit
 * Description:
 *      Initialize PHY 8380 serdes driver.
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
phy_8380_serdes_ge_mapperInit(rt_phydrv_t *pPhydrv);
/* Function Name:
 *      phy_8380_serdes_ge_mapperInit
 * Description:
 *      Initialize PHY 8380 serdes driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8380_serdes_ge_mapperInit(rt_phydrv_t *pPhydrv);

/* Function Name:
 *      phy_8380_init
 * Description:
 *      Initialize 8380 MAC internal serdes PHY.
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
phy_8380_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8380_media_get
 * Description:
 *      Get 8380 serdes PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. media type is PORT_MEDIA_FIBER
 */
extern int32
phy_8380_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

#if !defined(__BOOTLOADER__)
/* Function Name:
 *      phy_8380_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8380 is not supported the per-port Giga Lite feature.
 */
extern int32
phy_8380_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);
#endif /* !defined(__BOOTLOADER__) */

/* Function Name:
 *      phy_8380_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_8380_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8380_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed);

/* Function Name:
 *      phy_8390_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex);

/* Function Name:
 *      phy_8380_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex);

/* Function Name:
 *      phy_8380_enable_set
 * Description:
 *      Set interface status of the specific port
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
phy_8380_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8380_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable -   auto negotiation status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8380_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
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
phy_8380_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8380_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8380_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);


/* Function Name:
 *      phy_8380_fiberOAMLoopBack_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_fiberOAMLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      phy_8380_serdesFiberRx_check
 * Description:
 *      Handle serdes fiber 1G
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
extern int32
phy_8380_serdesFiberRx_check(uint32 unit, uint32 port, uint32 *pStatus);

/* Function Name:
 *      phy_8380_serdesFiberRx_reset
 * Description:
 *      Handle serdes fiber 1G
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
extern int32
phy_8380_serdesFiberRx_reset(uint32 unit, uint32 port);

/* Function Name:
 *      phy_8380_serdes_loopback_get
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable           -loopback mode status;
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8380_serdes_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8380_serdes_loopback_set
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8380_serdes_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8380_sds_get
 * Description:
 *      Get PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8380_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      phy_8380_sds_set
 * Description:
 *      Set PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8380_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      phy_8380_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8380_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      phy_8380_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8380_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8380_polar_set
 * Description:
 *      Configure serdes port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8380_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl);

/* Function Name:
 *      phy_8380_polar_get
 * Description:
 *      Get serdes port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8380_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl);

#endif /* __HAL_PHY_PHY_RTL8380_H__ */
