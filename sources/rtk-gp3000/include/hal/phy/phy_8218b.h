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
 * Purpose : PHY 8218 Driver APIs.
 *
 * Feature : PHY 8218 Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_8218B_H__
#define __HAL_PHY_PHY_8218B_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern rt_phydrv_t phy_8218b_drv_int_ge;


/* Function Name:
 *      phy_8218b_init
 * Description:
 *      Initialize PHY 8218B.
 * Input:
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
phy_8218b_init(rtk_port_t port);

/* Function Name:
 *      phy_8218b_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
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
phy_8218b_autoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
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
phy_8218b_autoNegoEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
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
phy_8218b_autoNegoAbility_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218b_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
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
phy_8218b_autoNegoAbility_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218b_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
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
phy_8218b_duplex_get(rtk_port_t port, uint32 *pDuplex);

/* Function Name:
 *      phy_8218b_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
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
phy_8218b_duplex_set(rtk_port_t port, uint32 duplex);

/* Function Name:
 *      phy_8218b_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
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
phy_8218b_speed_get(rtk_port_t port, uint32 *pSpeed);

/* Function Name:
 *      phy_8218b_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8218b_speed_set(rtk_port_t port, uint32 speed);

/* Function Name:
 *      phy_8218b_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
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
phy_8218b_enable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_greenEnable_get
 * Description:
 *      Get the status of green feature of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of linkdown green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218 is supported the per-port green feature.
 */
extern int32
phy_8218b_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_greenEnable_set
 * Description:
 *      Set the status of green feature of the specific port
 * Input:
 *      port   - port id
 *      enable - status of linkdown green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218 is supported the per-port green feature.
 */
extern int32
phy_8218b_greenEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_eeeEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_eeeEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
extern int32
phy_8218b_crossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode);

/* Function Name:
 *      phy_8218b_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
extern int32
phy_8218b_crossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode);

#endif /* __HAL_PHY_PHY_8218B_H__ */
