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
 * Purpose : MIIM service APIs in the SDK.
 *
 * Feature : MIIM service APIs
 *
 */

#ifndef __HAL_COMMON_MIIM_H__
#define __HAL_COMMON_MIIM_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/diag.h>
#include <hal/chipdef/chipdef.h>
#include <hal/phy/phydef.h>

/*
 * Function Declaration
 */

/* Function Name:
 *      phy_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 */
extern int32
phy_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

/* Function Name:
 *      phy_media_set
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 */
extern int32
phy_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media);

/* Function Name:
 *      phy_autoNegoEnable_get
 * Description:
 *      Get auto negotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_autoNegoEnable_set
 * Description:
 *      Set auto negotiation enable status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_autoNegoAbilityLocal_get
 * Description:
 *      Get complete abilities for auto negotiation of the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 *
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_autoNegoAbilityLocal_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_autoNegoAbility_get
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_autoNegoAbility_set
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_duplex_get
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex);


/* Function Name:
 *      phy_duplex_set
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex);

/* Function Name:
 *      phy_speed_get
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);


/* Function Name:
 *      phy_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed);

/* Function Name:
 *      phy_speedStatus_get
 * Description:
 *      Get PHY operational link speed status. The status is valid only when link is up.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_speedStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_enable_set
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_enable_get
 * Description:
 *      Get PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      pEnable       - pointer to admin configuration of PHY interface
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);

/* Function Name:
 *      phy_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_rtct_start(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_greenEnable_get
 * Description:
 *      Get the status of green feature of the specific port in the specific unit
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_greenEnable_set
 * Description:
 *      Set the status of green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
extern int32
phy_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode);

/* Function Name:
 *      phy_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
extern int32
phy_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode);

/* Function Name:
 *      phy_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pStatus - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
extern int32
phy_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);

/* Function Name:
 *      phy_fiber_media_get
 * Description:
 *      Get PHY fiber media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_100
 *      - PORT_FIBER_MEDIA_AUTO
 */
extern int32
phy_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia);

/* Function Name:
 *      phy_fiber_media_set
 * Description:
 *      Get PHY fiber media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_100
 *      - PORT_FIBER_MEDIA_AUTO
 */
extern int32
phy_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media);

/* Function Name:
 *      phy_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      broadcastID   - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID);

/* Function Name:
 *      phy_gigaLiteEnable_get
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
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_eeepEnable_get
 * Description:
 *      Get enable status of EEEP function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEEP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_eeepEnable_set
 * Description:
 *      Set enable status of EEEP function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      phy_patch_set
 * Description:
 *      Patch the PHY.
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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_patch_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_chk_rst_status
 * Description:
 *      Check the PHY had been reset or NOT.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      status - 1 : Had been reset
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_chk_rst_status(uint32 unit, rtk_port_t port, uint32 * status);

/* Function Name:
 *      phy_downSpeedEnable_get
 * Description:
 *      Get down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_downSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      phy_downSpeedEnable_set
 * Description:
 *      Set down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_downSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_downSpeedStatus_get
 * Description:
 *      Get the status of down speed status of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pDownSpeedStatus - pointer to status of down speed.
 *                         TRUE: link is up due to down speed; FALSE: down speed is not performed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus);

/* Function Name:
 *      phy_fiberDownSpeedEnable_get
 * Description:
 *      Get fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_fiberDownSpeedEnable_get
 * Description:
 *      Set fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_downSpeedEnable_get
 * Description:
 *      Get down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_downSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      phy_downSpeedEnable_set
 * Description:
 *      Set down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_downSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_drv_chk
 * Description:
 *      Check the driver is used for which PHY..
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      drv_type : Chip version
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED    - PHY doesn't support this feature
 * Note:
 *      None
 */
extern int32
phy_drv_chk(uint32 unit, rtk_port_t port, int32 * drv_type);

/* Function Name:
 *      phy_fiberNwayForceLinkEnable_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber nway force link
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberNwayForceLinkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_fiberNwayForceLinkEnable_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber nway force link
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberNwayForceLinkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_fiberOAMLoopBackEnable_set
 * Description:
 *      Set fiber port OAM Loopback featrue enable or not
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      phy_ptpSwitchMacAddr_get
 * Description:
 *      Get the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port,
    rtk_mac_t *pSwitchMacAddr);

/* Function Name:
 *      phy_ptpSwitchMacAddr_set
 * Description:
 *      Set the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port,
    rtk_mac_t *pSwitchMacAddr);

/* Function Name:
 *      phy_ptpRefTime_get
 * Description:
 *      Get the reference time of PHY of the specified port.
 * Input:
 *      unit       - unit id
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRefTime_get(uint32 unit, rtk_port_t port,
    rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      phy_ptpRefTime_set
 * Description:
 *      Set the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRefTime_set(uint32 unit, rtk_port_t port,
    rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      phy_ptpRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      port    - port id
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
extern int32
phy_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port,
    uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      phy_ptpRefTimeEnable_get
 * Description:
 *      Get the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_ptpRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port    - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRefTimeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_ptpRefTimeFreq_get
 * Description:
 *      Get the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pFreq  - pointer to reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
extern int32
phy_ptpRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq);

/* Function Name:
 *      phy_ptpRefTimeFreq_set
 * Description:
 *      Set the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      freq   - reference time frequency
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
extern int32
phy_ptpRefTimeFreq_set(uint32 unit, rtk_port_t port, uint32 freq);

/* Function Name:
 *      phy_ptpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_ptpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_ptpInterruptStatus_get
 * Description:
 *      Get PTP interrupt status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pIsPortIntr - port interrupt triggered status
 *      pIntrSts - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIsPortIntr, uint32 *pIntrSts);

/* Function Name:
 *      phy_ptpInterruptEnable_get
 * Description:
 *      Get PTP interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_ptpInterruptEnable_set
 * Description:
 *      Set PTP interrupt enable status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_ptpIgrTpid_get
 * Description:
 *      Get inner/outer TPID of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type  -  vlan type
 *      tpid_idx - TPID index
 * Output:
 *      pTpid   - TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid);

/* Function Name:
 *      phy_ptpIgrTpid_set
 * Description:
 *      Set inner/outer TPID of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type  -  vlan type
 *      tpid_idx - TPID index
 *      tpid   - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid);

/* Function Name:
 *      phy_ptpSwitchMacRange_get
 * Description:
 *      Get MAC address range of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pRange - pointer to MAC address range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpSwitchMacRange_get(uint32 unit, rtk_port_t port, uint32 *pRange);

/* Function Name:
 *      phy_ptpSwitchMacRange_set
 * Description:
 *      Set MAC address range of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pMac  -  pointer to MAC address
 *      range - MAC address range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpSwitchMacRange_set(uint32 unit, rtk_port_t port, uint32 range);

/* Function Name:
 *      phy_ptpRxTimestamp_get
 * Description:
 *      Get PTP Rx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      phy_ptpTxTimestamp_get
 * Description:
 *      Get PTP Tx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpTxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      phy_ptpOper_get
 * Description:
 *      Get the PTP time operation configuration of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_ptpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

/* Function Name:
 *      phy_ptpOper_set
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_ptpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

/* Function Name:
 *      phy_ptpLatchTime_get
 * Description:
 *      Get the PTP latched time of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_ptpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime);

/* Function Name:
 *      phy_ptpRefTimeFreqCfg_get
 * Description:
 *      Get the frequency of reference time of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pFreqCfg    - pointer to configured reference time frequency
 *      pFreqCur    - pointer to current reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRefTimeFreqCfg_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur);

/* Function Name:
 *      phy_ptpRefTimeFreqCfg_set
 * Description:
 *      Set the frequency of reference time of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      freq        - reference time frequency
 *      apply       - if the frequency is applied immediately
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpRefTimeFreqCfg_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply);

/* Function Name:
 *      phy_ptpTxInterruptStatus_get
 * Description:
 *      Get the TX timestamp FIFO non-empty interrupt status of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pIntrSts    - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts);

/* Function Name:
 *      phy_ptpTxTimestampFifo_get
 * Description:
 *      Get the top entry from PTP Tx timstamp FIFO on the dedicated port from the specified device. of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pTimeEntry  - pointer buffer of TIME timestamp entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry);

/* Function Name:
 *      phy_ptp1PPSOutput_get
 * Description:
 *      Get the 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPulseWidth - pointer to 1 PPS pulse width
 *      pEnable     - pointer to 1 PPS output enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_ptp1PPSOutput_set
 * Description:
 *      Set the 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pulseWidth  - pointer to 1 PPS pulse width
 *      enable      - enable 1 PPS output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable);

/* Function Name:
 *      phy_ptpClockOutput_get
 * Description:
 *      Get the clock output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pClkOutput  - pointer to clock output configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput);

/* Function Name:
 *      phy_ptpClockOutput_set
 * Description:
 *      Set the clock output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pClkOutput  - pointer to clock output configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput);

/* Function Name:
 *      phy_ptpOutputSigSel_get
 * Description:
 *      Get the output pin signal selection configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pOutSigSel  - pointer to output pin signal selection configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel);

/* Function Name:
 *      phy_ptpOutputSigSel_set
 * Description:
 *      Set the output pin signal selection configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      outSigSel   - output pin signal selection configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel);

/* Function Name:
 *      phy_ptpTransEnable_get
 * Description:
 *      Get the enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pEnable     - pointer to PTP transparent clock enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpTransEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_ptpTransEnable_set
 * Description:
 *      Set the enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      enable      - PTP transparent clock enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpTransEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_ptpLinkDelay_get
 * Description:
 *      Get the link delay for PTP p2p transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pLinkDelay  - pointer to link delay (unit: nsec)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay);

/* Function Name:
 *      phy_ptpLinkDelay_set
 * Description:
 *      Set the link delay for PTP p2p transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      linkDelay   - link delay (unit: nsec)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ptpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay);


/* Function Name:
 *      phy_reg_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_reg_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 phy_reg, uint32 *pData);

/* Function Name:
 *      phy_reg_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_reg_set(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data);


/* Function Name:
 *      phy_reg_park_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_park_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData);

/* Function Name:
 *      phy_reg_park_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_park_set(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 data);


/* Function Name:
 *      phy_reg_mmd_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData);

/* Function Name:
 *      phy_reg_mmd_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data);

/* Function Name:
 *      phy_reg_mmd_portmask_set
 * Description:
 *      Set PHY registers in those portmask.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mmdAddr  - mmd device address
 *      mmdReg   - mmd reg id
 *      data     - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_mmd_portmask_set(uint32 unit, rtk_portmask_t portmask, uint32 mmdAddr, uint32 mmdReg, uint32 data);


/* Function Name:
 *      phy_reg_extParkPage_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_extParkPage_get(uint32 unit, rtk_port_t port, uint32 mainPage, uint32 extPage, uint32 parkPage, uint32 phy_reg, uint32 *pData);

/* Function Name:
 *      phy_reg_extParkPage_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_extParkPage_set(uint32 unit, rtk_port_t port, uint32 mainPage, uint32 extPage, uint32 parkPage, uint32 phy_reg, uint32 data);

/* Function Name:
 *      phy_reg_extParkPage_portmask_set
 * Description:
 *      Set PHY registers in those portmask.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg  - PHY register
 *      data     - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_extParkPage_portmask_set(uint32 unit, rtk_portmask_t portmask, uint32 mainPage, uint32 extPage, uint32 parkPage, uint32 phy_reg, uint32 data);

/* Function Name:
 *      phy_reg_portmask_set
 * Description:
 *      Set PHY registers in those portmask.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_portmask_set(uint32 unit, rtk_portmask_t portmask, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      phy_reg_broadcast_set
 * Description:
 *      Set PHY registers with broadcast mechanism.
 * Input:
 *      unit    - unit id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_reg_broadcast_set(uint32 unit, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      phy_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
extern int32
phy_masterSlave_get(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual);


/* Function Name:
 *      phy_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave);

/* Function Name:
 *      phy_loopbackEnable_get
 * Description:
 *      Get Loopback featrue enable/disable
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pEnable  - ENABLED: Enable loopback;
 *                DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_loopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_loopBackEnable_set
 * Description:
 *      Set Loopback featrue enable/disable
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ENABLED: Enable loopback;
 *                DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_loopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_construct_reg_park_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_construct_reg_park_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData);

/* Function Name:
 *      phy_construct_reg_park_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_construct_reg_park_set(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 data);

extern int32
phy_construct_enable_set(uint32 phyType, uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_fiberTxDis_set
 * Description:
 *      Set PHY Tx disable signal featrue
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ENABLED: Enable Tx disable signal;
 *                DISABLED: Disable Tx disable signal.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_fiberTxDisPin_set
 * Description:
 *      Set PHY Tx disable signal GPO output
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      data   - GPO pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data);

/* Function Name:
 *      phy_fiberRx_check
 * Description:
 *      Check Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberRx_check(uint32 unit, rtk_port_t port, uint32 * pStatus);

/* Function Name:
 *      phy_fiberRx_reset
 * Description:
 *      Reset Fiber Rx part
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberRx_reset(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_serdesFiberRx_check
 * Description:
 *      Check SerDes Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_serdesFiberRx_check(uint32 unit, rtk_port_t port, uint32 * pStatus);

/* Function Name:
 *      phy_serdesFiberRx_reset
 * Description:
 *      Reset SerDes Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_serdesFiberRx_reset(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_fiberLinkUp_handler1
 * Description:
 *      PHY fiber linkup handler 1
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_fiberLinkUp_handler1(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_serdes_rst
 * Description:
 *      Reset PHY SerDes
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_serdes_rst(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_serdes_linkdown_check
 * Description:
 *      Check the PHY serdes had been linkdown or NOT.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      status - 1 : Had been linkdwon
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED    - PHY doesn't support this feature
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_serdes_linkdown_check(uint32 unit, rtk_port_t port, uint32 * pStatus);

/* Function Name:
 *      phy_cableESD_recover
 * Description:
 *      Cable ESD problem.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pPhy_rst_flag   - 1: PHY RST, 0: PHY Normal
 *      pPort_rst_flag  - 1: port RST, 0: port Normal
 *      pRst_portmask   - reset port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor for Cable ESD probem and patch it.
 *      Protect PHY page by port semaphore.
 */
extern int32
phy_cableESD_recover(uint32 unit, rtk_port_t port, uint32 *pPhy_rst_flag, uint32 *pPort_rst_flag, rtk_portmask_t *pRst_portmask);

/* Function Name:
 *      phy_fiberRxEnable_get
 * Description:
 *      Get PHY Rx enable status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable  - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_fiberRxEnable_set
 * Description:
 *      Set PHY Rx enable status
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_10gMedia_get
 * Description:
 *      Get 10G port media of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pMedia  - pointer to the media type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia);

/* Function Name:
 *      phy_10gMedia_set
 * Description:
 *      Set 10G port media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

/* Function Name:
 *      phy_polar_get
 * Description:
 *      Get 10GE port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCfg);

/* Function Name:
 *      phy_polar_set
 * Description:
 *      Configure 10GE port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCfg);

/* Function Name:
 *      phy_id_get
 * Description:
 *      Get phy chip id and revision id
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_id     - chip ID by phy_type_t
 *      pChip_rev_id - chip revision ID.
 *                     If the chip version is larger than SDK known version,
 *                     for SDK forward comatibility, the ID will return the latest chip ID that SDK reconized!!
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_id_get(uint32 unit, rtk_port_t port, uint32 *pChip_id, uint16 *pChip_rev_id);

/* Function Name:
 *      phy_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      channel   - Channel A, B, C, or D
 *      allPhyPort - apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall use the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      None
 */
extern int32
phy_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);

/* Function Name:
 *      phy_portEyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes id or 0 for serdes port or the PHY has no serdes id.
 *      frameNum- frame number
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
phy_portEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum);

/* Function Name:
 *      phy_eyeMonitorInfo_get
 * Description:
 *      Get the status of eye monitor height and width of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sds     - SerDes id of the PHY
 *      frameNum- frame number
 * Output:
 *      pInfo - pointer to the information of eye monitor height and width
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_BUSYWAIT_TIMEOUT - Read information timeout
 * Note:
 *      None
 */
extern int32
phy_eyeMonitorInfo_get(uint32 unit, rtk_port_t port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo);

/* Function Name:
 *      phy_portFlashImage_load
 * Description:
 *      load image into flash
 * Input:
 *      unit - unit id
 *      port - port id
 *      size - image size
 *      image - image
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
phy_portFlashImage_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image);

/* Function Name:
 *      _phy_swMacPollPhyStatus_get
 * Description:
 *      Get PHY status
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pphyStatus  - PHY status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
_phy_swMacPollPhyStatus_get(uint32 unit, rtk_port_t port, rtk_port_swMacPollPhyStatus_t *pphyStatus);

/* Function Name:
 *      _phy_resolutionResult2Speed_get
 * Description:
 *      Get speed from PHY resolution status
 * Input:
 *      resl - resolution status
 * Output:
 *      None
 * Return:
 *      speed
 * Note:
 *      None
 */
extern rtk_port_speed_t
_phy_resolutionResult2Speed_get(uint32 resl);

/* Function Name:
 *      phy_sds_get
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
phy_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      phy_sds_set
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
phy_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);


/* Function Name:
 *      phy_sdsRxCaliStatus_get
 * Description:
 *      Get PHY SerDes rx-calibration status
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      rtk_port_phySdsRxCaliStatus_t   - rx-calibration status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_OUT_OF_RANGE - invalid serdes id
 * Applicable:
 *      8295r
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
phy_sdsRxCaliStatus_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus);

/* Function Name:
 *      phy_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable         - enable status of fiber unidirection
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
phy_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_reset_set
 * Description:
 *      Set PHY standard register Reset bit (0.15).
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_reset_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_linkStatus_get
 * Description:
 *      Get PHY link status from standard register (1.2).
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      The Link Status bit (Status Register 1.2) has LL (Latching Low) attribute
 *      for link failure. Please refer IEEE 802.3 for detailed.
 */
extern int32
phy_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

/* Function Name:
 *      phy_peerAutoNegoAbility_get
 * Description:
 *      Get ability from link partner advertisement auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_peerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side interface serdes mode
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pSerdesMode  - PHY serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pSerdesMode);

/* Function Name:
 *      phy_ledMode_set
 * Description:
 *      Configure LED mode to PHY. PHY will send out data to LED panel for port status indication
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pLedMode - LED mode select.
 *                 led_id is used to specify which LED to config.
 *                 mdi is PHY port ID of each PHY (starting from 0). It config the LED to display which port's status.
 *                 led_ind_status_sel can select multiple indicators by OR with mutiple RTK_PHY_LED_IND_STATUS_SEL_xxx macros.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ledMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode);

/* Function Name:
 *      phy_ledCtrl_get
 * Description:
 *      Get configuration of LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pLedCtrl - LED control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ledCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

/* Function Name:
 *      phy_ledCtrl_set
 * Description:
 *      Configure LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pLedCtrl - LED control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ledCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

/* Function Name:
 *      phy_modelId_get
 * Description:
 *      Get phy model id.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pModel_id   - model ID indicates by phy_type_t.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_modelId_get(uint32 unit, rtk_port_t port, phy_type_t *pModel_id);

/* Function Name:
*      phy_macIntfSerdesLinkStatus_get
* Description:
*      Get PHY's MAC side interface serdes link status
* Input:
*      unit    - unit ID
*      port    - Base mac ID of the PHY
* Output:
*      pStatus - link status of the SerDes
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_UNIT_ID - invalid unit id
*      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
* Note:
*      None
*/
extern int32
phy_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus);


/* Function Name:
 *      phy_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pEyeParam - eye parameter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

/* Function Name:
 *      phy_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pEyeParam - eye parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);


/* Function Name:
 *      phy_2pt5gLiteEnable_get
 * Description:
 *      Get the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_2pt5gLiteEnable_set
 * Description:
 *      Set the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_mdiLoopbackEnable_get
 * Description:
 *      Enable port MDI loopback for connecting with RJ45 loopback connector
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of MDI loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_mdiLoopbackEnable_set
 * Description:
 *      Enable port MDI loopback for connecting with RJ45 loopback connector
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of MDI loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      phy_intr_init
 * Description:
 *      Initialize the type of PHY interrupt function of the specified PHY chip.
 * Input:
 *      unit    - unit id
 *      port    - base mac ID number of the PHY
 *      phyIntr - PHY interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_intr_init(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr);

/* Function Name:
 *      phy_intrEnable_get
 * Description:
 *      Get the type of PHY interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 * Output:
 *      pEnable - pointer to status of interrupt enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_intrEnable_set
 * Description:
 *      Set the type of PHY interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable);

/* Function Name:
 *      phy_intrStatus_get
 * Description:
 *      Get specified PHY interrupt status.
 * Input:
 *      unit    - unit id
 *      port    - port id or base mac ID number of the PHY
 *      phyIntr - PHY interrupt type
 * Output:
 *      pStatus - interrupt triggered status for specified PHY interrupt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus);

/* Function Name:
 *      phy_intrMask_get
 * Description:
 *      Get PHY interrupt mask status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 * Output:
 *      pMask   - pointer to status of PHY interrupt mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_intrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask);

/* Function Name:
 *      phy_intrMask_set
 * Description:
 *      Set PHY interrupt mask status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      mask    - mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_intrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask);


/* Function Name:
 *      phy_sdsTestMode_set
 * Description:
 *      Set SerDes test mode.
 * Input:
 *      unit        - unit id
 *      port        - base mac ID number of the PHY
 *      sdsId       - SerDes id
 *      testMode    - test mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 * Note:
 *      None
 */
extern int32
phy_sdsTestMode_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_testMode_t testMode);

/* Function Name:
 *      phy_sdsTestModeCnt_get
 * Description:
 *      Get SerDes test mode test pattern error counter.
 * Input:
 *      unit        - unit id
 *      port        - base mac ID number of the PHY
 *      sdsId       - SerDes id
 * Output:
 *      pCnt        - test pattern error counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 * Note:
 *      The test pattern error counter register is read-clear.
 */
extern int32
phy_sdsTestModeCnt_get(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 *pCnt);


/* Function Name:
 *      phy_sdsLeq_get
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sdsId   - SerDes of the PHY
 * Output:
 *      pManual_en - pointer to manual LEQ config satus
 *      pLeq_val   - pointer to current LEQ value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_sdsLeq_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t *pManual_en, uint32 *pLeq_val);

/* Function Name:
 *      phy_sdsLeq_set
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sdsId   - SerDes of the PHY
 *      manual_en - ENABLED: LEQ in manual-mode; DISABLED: LEQ is auto-mode.
 *      leq_val - Fixed LEQ value when manual_en is set to ENABLED;
 *                this field is not used in driver when manual_en set to DISABLED, just keep it set to 0.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_sdsLeq_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t manual_en, uint32 leq_val);

/* Function Name:
 *      phy_fiberRemoteFault_handle
 * Description:
 *      Handle fiber remote fault
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_fiberRemoteFault_handle(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_ctrl_get
 * Description:
 *      Get Port/PHY specific settings
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue);

/* Function Name:
 *      phy_ctrl_set
 * Description:
 *      Set the statue of Control of the specific port in the specific unit
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value);

/* Function Name:
 *      phy_info_get
 * Description:
 *      Get PHY capibility information
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pphyInfo - PHY information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_info_get(uint32 unit, rtk_port_t port, rt_phyInfo_t *pphyInfo);

/* Function Name:
 *      phy_liteEnable_get
 * Description:
 *      Get the status of Lite setting
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      mode    - Lite speed mode
 * Output:
 *      pEnable - pointer to status of Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_liteEnable_get(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_liteEnable_set
 * Description:
 *      Set the status of Lite setting
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      mode    - Lite speed mode
 *      enable  - status of Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_liteEnable_set(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t enable);

/* Function Name:
 *      phy_dbgCounter_get
 * Description:
 *      Get the status of debug counter
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - counter type
 * Output:
 *      pCnt - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_dbgCounter_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint64 *pCnt);


/* Function Name:
 *      phy_waMon_phyReconfig_register
 * Description:
 *      Register callback function for PHY need to reconfigure notification
 * Input:
 *      phyReconfig_callback    - callback function for reconfigure notification
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
phy_waMon_phyReconfig_register(rtk_port_phyReconfig_callback_t phyReconfig_callback);

/* Function Name:
 *      phy_waMon_phyReconfig_unregister
 * Description:
 *      UnRegister callback function for PHY need to reconfigure notification
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32
phy_waMon_phyReconfig_unregister(void);

/* Function Name:
 *      phy_speedDuplexStatus_get
 * Description:
 *      Get PHY operational link speed-duplex status. The status is valid only when link is up.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed-duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
extern int32
phy_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);


/* Function Name:
 *      miim_phy_macsec_reg_set
 * Description:
 *
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      dir     - macsec reg type
 *      reg     - register address
 *      data    - value write to register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
miim_phy_macsec_reg_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 data);

/* Function Name:
 *      miim_phy_macsec_reg_get
 * Description:
 *
 * Input:
 *      unit    - unit id
 *      port    -  port id
 *      dir     - macsec reg type
 *      reg     - register address
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
miim_phy_macsec_reg_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 *pData);


/* Function Name:
 *      miim_phy_macsec_port_cfg_set
 * Description:
 *      Set per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      pPortcfg - pointer to macsec port configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_port_cfg_set(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      miim_phy_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_port_cfg_get(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      miim_phy_macsec_sc_create
 * Description:
 *      Create a MACsec Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      pSc      - pointer to macsec sc configuration structure
 * Output:
 *      pSc_id   - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sc_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    rtk_macsec_sc_t *pSc, uint32 *pSc_id);

/* Function Name:
 *      miim_phy_macsec_sc_get
 * Description:
 *      Get configuration info for a created Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_index - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sc_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_t *pSc);

/* Function Name:
 *      miim_phy_macsec_sc_del
 * Description:
 *      Delete a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sc_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id);

/* Function Name:
 *      miim_phy_macsec_sc_status_get
 * Description:
 *      Get hardware status for a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_status - pointer to macsec SC status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sc_status_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_status_t *pSc_status);

/* Function Name:
 *      miim_phy_macsec_sa_create
 * Description:
 *      Create a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 *      pSa      - pointer to macsec SA configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sa_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);

/* Function Name:
 *      miim_phy_macsec_sa_get
 * Description:
 *      Get configuration info for a Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      pSa      - pointer to macsec SA configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sa_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);

/* Function Name:
 *      miim_phy_macsec_sa_del
 * Description:
 *      Delete a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_sa_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      miim_phy_macsec_sa_activate
 * Description:
 *      Activate a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - Secure Channel id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      For egress, this function will change running SA.
 */
extern int32
miim_phy_macsec_sa_activate(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      miim_phy_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_rxsa_disable(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an);

/* Function Name:
 *      miim_phy_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_txsa_disable(uint32 unit, rtk_port_t port, uint32 txsc_id);

/* Function Name:
 *      miim_phy_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_stat_clear(uint32 unit, rtk_port_t port);

/* Function Name:
 *      miim_phy_macsec_stat_port_get
 * Description:
 *      get per-port statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern int32
miim_phy_macsec_stat_port_get(uint32 unit, rtk_port_t port, rtk_macsec_stat_t stat,
    uint64 *pCnt);

/* Function Name:
 *      miim_phy_macsec_stat_txsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern int32
miim_phy_macsec_stat_txsa_get(uint32 unit, rtk_port_t port, uint32 txsc_id,
    rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, uint64 *pCnt);

/* Function Name:
 *      miim_phy_macsec_stat_rxsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern int32
miim_phy_macsec_stat_rxsa_get(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, uint64 *pCnt);

/* Function Name:
 *      miim_phy_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
miim_phy_macsec_intr_status_get(uint32 unit, rtk_port_t port,
    rtk_macsec_intr_status_t *pIntr_status);

/* Function Name:
 *      phy_sdsReg_get
 * Description:
 *      Get PHY SerDes registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - SerDes page id
 *      sdsReg  - SerDes reg id
 * Note:
 *      None
 */
extern int32
phy_sdsReg_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData);

/* Function Name:
 *      phy_sdsReg_set
 * Description:
 *      Set PHY SerDes registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - SerDes page id
 *      sdsReg  - SerDes reg id
 *      data    - write data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_sdsReg_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 data);

#endif  /* __HAL_COMMON_MIIM_H__ */

