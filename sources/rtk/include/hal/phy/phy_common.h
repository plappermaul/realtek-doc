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
 * Purpose : PHY Common Driver APIs.
 *
 * Feature : PHY Common Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_COMMON_H__
#define __HAL_PHY_PHY_COMMON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */
#define RTCT_ENABLE                 (0)
#define RTCT_CH_A                   (4)
#define RTCT_CH_B                   (5)
#define RTCT_CH_C                   (6)
#define RTCT_CH_D                   (7)
#define RTCT_DONE                   (15)

#define RTCT_BASE_ADDR              (0x802a)
#define RTCT_LEN_ADDR_A             (RTCT_BASE_ADDR + 0x2)
#define RTCT_LEN_ADDR_B             (RTCT_BASE_ADDR + 0x6)
#define RTCT_LEN_ADDR_C             (RTCT_BASE_ADDR + 0xa)
#define RTCT_LEN_ADDR_D             (RTCT_BASE_ADDR + 0xe)
#define RTCT_STATUS_ADDR_A          (RTCT_BASE_ADDR + 0x0)
#define RTCT_STATUS_ADDR_B          (RTCT_BASE_ADDR + 0x4)
#define RTCT_STATUS_ADDR_C          (RTCT_BASE_ADDR + 0x8)
#define RTCT_STATUS_ADDR_D          (RTCT_BASE_ADDR + 0xc)

#define RTCT_STATUS_NORMAL          (5)
#define RTCT_STATUS_SHORT           (4)
#define RTCT_STATUS_OPEN            (3)
#define RTCT_STATUS_MISSMATCH_SHORT (2)
#define RTCT_STATUS_MISSMATCH_OPEN  (1)
#define RTCT_STATUS_LINE_DRIVER     (0)

#define GIGA_LITE_CTRL_REG          0xa42

//#define PHY_C22_MMD_PAGE            0
#define PHY_C22_MMD_PAGE            0x0A41
#define PHY_C22_MMD_DEV_REG         13
#define PHY_C22_MMD_ADD_REG         14

/* MDIO Manageable Device(MDD) address*/
#define PHY_MMD_PMAPMD              1
#define PHY_MMD_PCS                 3
#define PHY_MMD_AN                  7
#define PHY_MMD_VEND1               30   /* Vendor specific 1 */
#define PHY_MMD_VEND2               31   /* Vendor specific 2 */

#define BIT_0       0x0001
#define BIT_1       0x0002
#define BIT_2       0x0004
#define BIT_3       0x0008
#define BIT_4       0x0010
#define BIT_5       0x0020
#define BIT_6       0x0040
#define BIT_7       0x0080
#define BIT_8       0x0100
#define BIT_9       0x0200
#define BIT_10      0x0400
#define BIT_11      0x0800
#define BIT_12      0x1000
#define BIT_13      0x2000
#define BIT_14      0x4000
#define BIT_15      0x8000

/* PHY SerDes eye-monitor handler */
typedef int32 (*phy_sds_eyeMon_hdlr_t)(int32 xAxis, int32 yAxis, uint32 frameNum, void *pDb, uint32 val);


/* Function Name:
 *      phy_common_unavail
 * Description:
 *      Return chip not support
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_PORT_NOT_SUPPORTED   - functions not supported by this port
 * Note:
 *      None
 */
extern int32
phy_common_unavail(void);

/* Function Name:
 *      phy_common_AutoNegoEnable_get
 * Description:
 *      Get auto-negotiation enable status of the specific port
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
phy_common_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      phy_common_autoNegoEnable_set
 * Description:
 *      Set auto-negotiation enable status of the specific port
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
phy_common_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_common_duplex_get
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
phy_common_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex);

/* Function Name:
 *      phy_common_duplex_get
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
phy_common_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex);

/* Function Name:
 *      phy_common_enable_set
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
phy_common_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
/* Function Name:
 *      phy_common_enable_get
 * Description:
 *      Get PHY interface status of the specific port
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
int32
phy_common_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *enable);

/* Function Name:
 *      phy_common_speed_get
 * Description:
 *      Get PHY negotiated speed of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      pSpeed        - PHY negotiated speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_common_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed);

/* Function Name:
 *      phy_common_speedStatusResReg_get
 * Description:
 *      Get PHY operational link speed status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      Only PHY that supports proprietary resolution register can use this driver!
 */
extern int32
phy_common_speedStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_common_speedDuplexStatusResReg_get
 * Description:
 *      Get PHY operational link speed-duplex status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 *      pDuplex - pointer to PHY operational link duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      Only PHY that supports proprietary resolution register can use this driver!
 */
extern int32
phy_common_speedDuplexStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);

/* Function Name:
 *      phy_common_reg_get
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
phy_common_reg_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 phy_reg, uint32 *pData);

/* Function Name:
 *      phy_common_reg_set
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
phy_common_reg_set(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      phy_common_reg_park_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
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
phy_common_reg_park_get(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData);


/* Function Name:
 *      phy_common_reg_park_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
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
phy_common_reg_park_set(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 data);

/* Function Name:
 *      phy_common_reg_mmd_get
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
phy_common_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData);

/* Function Name:
 *      phy_common_reg_mmd_set
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
phy_common_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data);

/* Function Name:
 *      phy_common_reg_mmd_portmask_set
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
phy_common_reg_mmd_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data);


/* Function Name:
 *      phy_common_reg_extParkPage_get
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
phy_common_reg_extParkPage_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData);


/* Function Name:
 *      phy_common_reg_extParkPage_set
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
phy_common_reg_extParkPage_set(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      phy_common_extParkPage_portmask_set
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
phy_common_reg_extParkPage_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data);

/* Function Name:
 *      phy_common_masterSlave_get
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
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
extern int32
phy_common_masterSlave_get(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual);

/* Function Name:
 *      phy_common_masterSlave_set
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
 * Note:
 *      None
 */
extern int32
phy_common_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave);


/* Function Name:
 *      phy_common_masterSlaveMmd_get
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
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
extern int32
phy_common_masterSlaveMmd_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual);

/* Function Name:
 *      phy_common_reset_set
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
 * Note:
 *      None
 */
extern int32
phy_common_reset_set(uint32 unit, rtk_port_t port);

/* Function Name:
*      phy_common_linkStatus_get
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
* Note:
*      The Link Status bit (Status Register 1.2) has LL (Latching Low) attribute
*      for link failure. Please refer IEEE 802.3 for detailed.
*/
extern int32
phy_common_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

/* Function Name:
*      phy_common_copperPeerAutoNegoAbility_get
* Description:
*      Get ability from link partner advertisement auto negotiation of the specific port
*      (For Copper port)
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
phy_common_copperPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
*      phy_common_fiberPeerAutoNegoAbility_get
* Description:
*      Get ability from link partner advertisement auto negotiation of the specific port
*      (For Fiber port)
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
phy_common_fiberPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);


/* Function Name:
 *      phy_common_masterSlaveMmd_set
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
 * Note:
 *      None
 */
int32
phy_common_masterSlaveMmd_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave);

/* Function Name:
 *      phy_common_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pEnable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_common_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_common_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_common_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_field_read
 * Description:
 *      Read PHY register field configuration
 * Input:
 *      unit - unit id
 *      port - port id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - PHY register field Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
phy_field_read(uint32 unit, uint32 port, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data);

/* Function Name:
 *      phy_field_write
 * Description:
 *      Write PHY register field configuration
 * Input:
 *      unit - unit id
 *      port - port id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - PHY register field Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
phy_field_write(uint32 unit, uint32 port, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data);

/* Function Name:
 *      phy_common_eyeMonPixel_get
 * Description:
 *      PHY SerDes eye-monitor utility to set val to the database according to x,y,z
 * Input:
 *      xAxis - x position
 *      yAxis - y position
 *      frameNum - frame number
 * Output:
 *      pDb - 3 dimension uint32 array database. E.g. uint32 a[RTK_EYE_MON_FRAME_MAX][RTK_EYE_MON_X_MAX][RTK_EYE_MON_Y_MAX]
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_common_eyeMonPixel_get(int32 xAxis, int32 yAxis, uint32 frameNum, void *pDb, uint32 val);

/* Function Name:
 *      phy_common_eyeMonInfo_update
 * Description:
 *      PHY SerDes eye-monitor utility to set val to the database according to x,y,z
 * Input:
 *      xAxis - x position
 *      yAxis - y position
 *      frameNum - frame number
 * Output:
 *      pDb - 2 dimension rtk_bitmap_t array database. e.g. rtk_bitmap_t eye[RTK_EYE_MON_X_MAX][BITMAP_ARRAY_CNT(RTK_EYE_MON_ARXIS_Y_MAX)]
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_common_eyeMonInfo_update(int32 xAxis, int32 yAxis, uint32 frameNum, void *pDb, uint32 val);

/* Function Name:
 *      phy_common_general_reg_mmd_get
 * Description:
 *      Get PHY registers. Support access C45 reg by C22 format when connect to C22 MAC.
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
int32
phy_common_general_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData);

/* Function Name:
 *      phy_common_general_reg_mmd_set
 * Description:
 *      Set PHY registers. Support access C45 reg by C22 format when connect to C22 MAC.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_common_general_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data);

/* Function Name:
 *      phy_common_c45_autoNegoEnable_get
 * Description:
 *      Get auto-negotiation enable status of the specific port
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
phy_common_c45_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      phy_common_c45_autoNegoEnable_set
 * Description:
 *      Set auto-negotiation enable status of the specific port
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
phy_common_c45_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      phy_common_c45_autoNegoAbility_get
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
phy_common_c45_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);


/* Function Name:
 *      phy_common_c45_autoNegoAbility_set
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
phy_common_c45_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);


/* Function Name:
*      phy_common_c45_copperPeerAutoNegoAbility_get
* Description:
*      Get ability from link partner advertisement auto negotiation of the specific port
*      (For Copper port)
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
phy_common_c45_copperPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_common_c45_masterSlave_get
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
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
extern int32
phy_common_c45_masterSlave_get(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual);

/* Function Name:
 *      phy_common_c45_masterSlave_set
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
 * Note:
 *      None
 */
extern int32
phy_common_c45_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave);

/* Function Name:
 *      phy_common_c45_speed_get
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
phy_common_c45_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_common_c45_speed_set
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
phy_common_c45_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed);

/* Function Name:
 *      phy_common_c45_enable_set
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
phy_common_c45_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_common_c45_enable_get
 * Description:
 *      Get PHY interface state enable/disable of the specific port
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
phy_common_c45_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
*      phy_common_c45_linkStatus_get
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
* Note:
*      The Link Status bit (Status Register 1.2) has LL (Latching Low) attribute
*      for link failure. Please refer IEEE 802.3 for detailed.
*/
extern int32
phy_common_c45_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

/* Function Name:
 *      phy_common_c45_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pEnable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_common_c45_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_common_c45_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_common_c45_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_common_c45_reset_set
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
 * Note:
 *      None
 */
extern int32
phy_common_c45_reset_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_common_c45_eeeEnable_get
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
 * Note:
 *      None
 */
extern int32
phy_common_c45_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_common_c45_eeeEnable_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_common_c45_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_common_c45_crossOverStatus_get
 * Description:
 *      Get cross over(MDI/MDI-X) status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
extern int32
phy_common_c45_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);

/* Function Name:
 *      phy_common_c45_speedDuplexStatus_get
 * Description:
 *      Get PHY operational link speed-duplex status.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 *      pDuplex - pointer to PHY operational link duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      none
 */
extern int32
phy_common_c45_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);

/* Function Name:
 *      phy_common_c45_speedStatusResReg_get
 * Description:
 *      Get PHY operational link speed status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      Only PHY that supports proprietary resolution register can use this driver!
 */
extern int32
phy_common_c45_speedStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_common_c45_ieeeTestMode_set
 * Description:
 *      Set test mode for PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      Must configure following settings before set IEEE test mode:
 *      1. disable ALPDS
 *      2. set speed
 *      3. disable auto-negotiation
 */
extern int32
phy_common_c45_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);

/* Function Name:
 *      phy_common_c45_speedDuplexStatusResReg_get
 * Description:
 *      Get operational link speed-duplex status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      Only PHY that supports proprietary resolution register can use this driver!
 */
extern int32
phy_common_c45_speedDuplexStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);

/* Function Name:
 *      phy_common_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pAbility    - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_common_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_common_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pAbility    - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_common_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_common_speed_set
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
phy_common_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed);

/* Function Name:
 *      phy_common_c45_an_restart
 * Description:
 *      restart autonegotiation
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_common_c45_an_restart(uint32 unit, rtk_port_t port);

#endif /* __HAL_PHY_PHY_COMMON_H__ */
