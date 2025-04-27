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
 * Purpose : MAC 8390 internal PHY Driver APIs.
 *
 * Feature : MAC 8390 internal PHY Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL8390_H__
#define __HAL_PHY_PHY_RTL8390_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/dal_waMon.h>

/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8390SDS     (1)
#define PORT_NUM_IN_8390SDS10G  (1)

#define PHY_8390_SERDES_SET(_reg, _sb, _eb, _val) phy_rtl8390_sds_field_write(unit, _reg, _sb, _eb, _val)

/*
 * Data Declaration
 */
extern rt_phydrv_t phy_8390_serdes_ge;
extern rt_phyInfo_t phy_8390sds_info;
extern rt_phyInfo_t phy_8390sds10G_info;

/* Function Name:
 *      phy_8390_serdes_ge_mapperInit
 * Description:
 *      Initialize PHY 8390 serdes driver.
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
phy_8390_serdes_ge_mapperInit(rt_phydrv_t *pPhydrv);

/* Function Name:
 *      phy_8390_init
 * Description:
 *      Initialize 8390 MAC internal serdes PHY.
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
phy_8390_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8390_media_get
 * Description:
 *      Get 8390 serdes PHY media type.
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
phy_8390_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

#if !defined(__BOOTLOADER__)
/* Function Name:
 *      phy_8390_gigaLiteEnable_get
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
 *      1. The RTL8390 is not supported the per-port Giga Lite feature.
 */
extern int32
phy_8390_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);
#endif /* !defined(__BOOTLOADER__) */

/* Function Name:
 *      phy_8390_speed_get
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
phy_8390_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_8390_speed_set
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
phy_8390_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed);

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
phy_8390_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex);

/* Function Name:
 *      phy_8390_duplex_set
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
phy_8390_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex);

/* Function Name:
 *      phy_8390_enable_set
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
phy_8390_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8390_autoNegoEnable_get
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
phy_8390_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8390_autoNegoEnable_set
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
phy_8390_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8390_autoNegoAbility_get
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
phy_8390_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8390_autoNegoAbility_set
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
phy_8390_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8390_serdesFiberRx_check
 * Description:
 *      Handle serdes fiber 1G & 10G
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
phy_8390_serdesFiberRx_check(uint32 unit, uint32 port, uint32 *pStatus);

void phy_8390_serdes_10g_leq_init (uint32 unit, rtk_port_t port);
int32 phy_8390_10gSds_restart(uint32 unit, rtk_port_t port);
int32 phy_8390_10g_init(uint32 unit, rtk_port_t port);

int32
phy_8390_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *media);

int32
phy_8390_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

int32
phy_8390_10gTxFifo_check(uint32 unit, uint32 port);

int32
phy_8390_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts);

/* Function Name:
 *      phy_8390_masterSlave_get
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
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8390_masterSlave_get(uint32 unit, rtk_port_t port,
    rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual);


/* Function Name:
 *      phy_8390_masterSlave_set
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
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8390_masterSlave_set(uint32 unit, rtk_port_t port,
    rtk_port_masterSlave_t masterSlave);

/* Function Name:
 *      phy_8390_serdes_loopback_get
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
int32
phy_8390_serdes_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8380_serdes_loopback_set
 * Description:
 *      Set serdes Loopback status of the specific port
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
int32
phy_8390_serdes_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8390_sds_get
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
int32
phy_8390_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      phy_8390_sds_set
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
int32
phy_8390_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

void
phy_rtl8390_sds_field_write(uint32 unit, uint32 reg, uint32 endBit,
    uint32 startBit, uint32 val);

#endif /* __HAL_PHY_PHY_RTL8390_H__ */
