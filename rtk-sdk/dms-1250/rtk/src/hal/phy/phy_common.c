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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/miim.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/mac/miim_common_drv.h>


/*
 * Symbol Definition
 */


/*
 * Function Declaration
 */

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
int32 phy_common_unavail(void)
{
    return RT_ERR_PORT_NOT_SUPPORTED;
} /* end of phy_common_unavail */

/* Function Name:
 *      phy_common_autoNegoEnable_get
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
int32
phy_common_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    if (phyData0 & AutoNegotiationEnable_MASK)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return ret;
} /* end of phy_common_autoNegoEnable_get */

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
int32
phy_common_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(AutoNegotiationEnable_MASK | RestartAutoNegotiation_MASK);
    phyData0 = phyData0 | ((enable << AutoNegotiationEnable_OFFSET) | (1 << RestartAutoNegotiation_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_common_autoNegoEnable_set */


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
int32
phy_common_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pDuplex = (phyData0 & DuplexMode_MASK) >> DuplexMode_OFFSET;

    return ret;
} /* end of phy_common_duplex_get */

/* Function Name:
 *      phy_common_duplex_set
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
int32
phy_common_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(DuplexMode_MASK);
    phyData0 = phyData0 | (duplex << DuplexMode_OFFSET);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_common_duplex_set */

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
int32
phy_common_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return ret;

    if (ENABLED == enable)
    {
        phyData &= ~(PowerDown_MASK);
    }
    else
    {
        phyData &= ~(PowerDown_MASK);
        phyData |= (1 << PowerDown_OFFSET);
    }

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_common_enable_set */

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
phy_common_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *enable)
{
    int32   ret;
    uint32  phyData;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData & PowerDown_MASK) >> PowerDown_OFFSET)
    {
       *enable = DISABLED;
    }
    else
    {
       *enable = ENABLED;
    }

    return RT_ERR_OK;
} /* end of phy_common_enable_set */

int32 phy_common_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;
    uint32  A, B, C, D, E, F, G, H, I, i;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 1, &phyData)) != RT_ERR_OK)
        return ret;
    A = (phyData >> 8) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 6, &phyData)) != RT_ERR_OK)
        return ret;
    B = (phyData >> 0) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyData)) != RT_ERR_OK)
        return ret;
    C = (phyData >> 12) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyData)) != RT_ERR_OK)
        return ret;
    D = (phyData >> 6) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyData)) != RT_ERR_OK)
        return ret;
    E = (phyData >> 13) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 5, &phyData)) != RT_ERR_OK)
        return ret;
    F = (phyData >> 7) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 5, &phyData)) != RT_ERR_OK)
        return ret;
    G = (phyData >> 5) & 1;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 9, &phyData)) != RT_ERR_OK)
        return ret;
    H = (phyData >> 9) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 10, &phyData)) != RT_ERR_OK)
        return ret;
    H &= (phyData >> 11) & 1;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 4, &phyData)) != RT_ERR_OK)
        return ret;
    I = (phyData >> 8) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 5, &phyData)) != RT_ERR_OK)
        return ret;
    I &= (phyData >> 8) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 4, &phyData)) != RT_ERR_OK)
        return ret;
    i = (phyData >> 7) & 1;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 5, &phyData)) != RT_ERR_OK)
        return ret;
    i &= (phyData >> 7) & 1;
    I = I | i;

    if ((A & B & C & H) | (A & ~C & D & ~E))
        *pSpeed = PORT_SPEED_1000M; /* 1000M */
    else if ((B & ~C & ~D & E)|(B & C & ~H & I)|(~B & ~C & ~D & E)|(~B & C & F & ~G))
        *pSpeed = PORT_SPEED_100M;  /* 100M */
    else
        *pSpeed = PORT_SPEED_10M;   /* 10M */

    return RT_ERR_OK;
}

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
int32
phy_common_speedStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    rtk_port_duplex_t duplex;

    return phy_common_speedDuplexStatusResReg_get(unit, port, pSpeed, &duplex);
}


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
int32
phy_common_speedDuplexStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, spd;

    *pSpeed = PORT_SPEED_10M;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 26, &phyData)) != RT_ERR_OK)
        return ret;

    /* [10:9,5:4] */
    spd = ((phyData & (0x3 << 9)) >> (9 - 2)) | ((phyData & (0x3 << 4)) >> 4);
    switch (spd)
    {
      case 0x0:
        *pSpeed = PORT_SPEED_10M;
        break;
      case 0x1:
        *pSpeed = PORT_SPEED_100M;
        break;
      case 0x2:
        *pSpeed = PORT_SPEED_1000M;
        break;
      case 0x3:
        *pSpeed = PORT_SPEED_500M;
        break;
      case 0x4:
        *pSpeed = PORT_SPEED_10G;
        break;
      case 0x5:
        *pSpeed = PORT_SPEED_2_5G;
        break;
      case 0x6:
        *pSpeed = PORT_SPEED_5G;
        break;
      default:
        *pSpeed = PORT_SPEED_10M;
        break;
    }

    *pDuplex = (((phyData & (0x1 << 3)) >> 3) == 1) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;

    return RT_ERR_OK;
}


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
int32
phy_common_reg_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 phy_reg, uint32 *pData)
{
    return hal_miim_read(unit, port, page, phy_reg, pData);
}

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
int32
phy_common_reg_set(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data)
{
    return hal_miim_write(unit, port, page, phy_reg, data);
}

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
int32
phy_common_reg_park_get(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData)
{
    return hal_miim_park_read(unit, port, page, parkPage, phy_reg, pData);
}


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
int32
phy_common_reg_park_set(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    return hal_miim_park_write(unit, port, page, parkPage, phy_reg, data);
}


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
int32
phy_common_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    return hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);
}

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
int32
phy_common_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    return hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
}

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
int32
phy_common_reg_mmd_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    return hal_miim_mmd_portmask_write(unit, portmask, mmdAddr, mmdReg, data);
}


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
int32
phy_common_reg_extParkPage_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    return hal_miim_extParkPage_read(unit, port, mainPage, extPage, parkPage, phy_reg, pData);
}

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
int32
phy_common_reg_extParkPage_set(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    return hal_miim_extParkPage_write(unit, port, mainPage, extPage, parkPage, phy_reg, data);
}

/* Function Name:
 *      phy_common_reg_extParkPage_portmask_set
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
int32
phy_common_reg_extParkPage_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data)
{
    return hal_miim_extParkPage_portmask_write(unit, portmask, mainPage, extPage, parkPage, phy_reg, data);
}

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
int32
phy_common_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    int32 ret;
    uint32 data;

    if ((ret = hal_miim_read(unit, port, 0, 9, &data)) != RT_ERR_OK)
        return ret;

    data = (data >> 11) & 0x3;
    switch(data)
    {
        case 0x2:
            *pMasterSlaveCfg = PORT_SLAVE_MODE;
            break;
        case 0x3:
            *pMasterSlaveCfg = PORT_MASTER_MODE;
            break;
        default:
            *pMasterSlaveCfg = PORT_AUTO_MODE;
            break;
    }

    if ((ret = hal_miim_read(unit, port, 0, 10, &data)) != RT_ERR_OK)
        return ret;
    data = (data >> 14) & 0x1;
    switch(data)
    {
        case 0x0:
            *pMasterSlaveActual = PORT_SLAVE_MODE;
            break;
        case 0x1:
            *pMasterSlaveActual = PORT_MASTER_MODE;
            break;
        default:
            *pMasterSlaveActual = PORT_SLAVE_MODE;
            break;
     }

    return ret;
}/* end of phy_common_masterSlave_get */

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
int32
phy_common_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    int32 ret;
    uint32 data;

    if ((ret = hal_miim_read(unit, port, 0, 9, &data)) != RT_ERR_OK)
        return ret;

    data &= ~ (0x3 << 11);
    switch(masterSlave)
    {
        case PORT_AUTO_MODE:
            data |= 0x0 << 11;
            break;
        case PORT_SLAVE_MODE:
            data |= 0x2 << 11;
            break;
        case PORT_MASTER_MODE:
            data |= 0x3 << 11;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_write(unit, port, 0, 9, data)) != RT_ERR_OK)
        return ret;

    return ret;
}/* end of phy_common_masterSlave_set */



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
int32
phy_common_masterSlaveMmd_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    int32 ret;
    uint32 data;

    if ((ret = hal_miim_read(unit, port, 0, 9, &data)) != RT_ERR_OK)
        return ret;

    data = (data >> 11) & 0x3;
    switch(data)
    {
        case 0x2:
            *pMasterSlaveCfg = PORT_SLAVE_MODE;
            break;
        case 0x3:
            *pMasterSlaveCfg = PORT_MASTER_MODE;
            break;
        default:
            *pMasterSlaveCfg = PORT_AUTO_MODE;
            break;
    }

    if ((ret = hal_miim_read(unit, port, 0, 10, &data)) != RT_ERR_OK)
        return ret;
    data = (data >> 14) & 0x1;
    switch(data)
    {
        case 0x0:
            *pMasterSlaveActual = PORT_SLAVE_MODE;
            break;
        case 0x1:
            *pMasterSlaveActual = PORT_MASTER_MODE;
            break;
        default:
            *pMasterSlaveActual = PORT_SLAVE_MODE;
            break;
     }

    return ret;
}/* end of phy_common_masterSlaveMmd_get */

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
    rtk_port_masterSlave_t   masterSlave)
{
    int32 ret;
    uint32 data;

    if ((ret = hal_miim_mmd_read(unit, port, 7, 32, &data)) != RT_ERR_OK)
        return ret;

    data &= ~ (0x3 << 14);
    switch(masterSlave)
    {
        case PORT_AUTO_MODE:
            data |= 0x0 << 14;
            break;
        case PORT_SLAVE_MODE:
            data |= 0x2 << 14;
            break;
        case PORT_MASTER_MODE:
            data |= 0x3 << 14;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_mmd_write(unit, port, 7, 32, data)) != RT_ERR_OK)
        return ret;

    return ret;
}/* end of phy_common_masterSlaveMmd_set */


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
int32
phy_common_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32       ret;
    uint32      data;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    if(Loopback_MASK & data)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
}

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
int32
phy_common_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret;
    uint32      data;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
         return ret;
    }

    if (enable == ENABLED)
        data |= Loopback_MASK;
    else
        data &= ~Loopback_MASK;

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

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
int32
phy_common_reset_set(uint32 unit, rtk_port_t port)
{
    int32       ret;
    uint32      data;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
         return ret;
    }

    data |= Reset_MASK;

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

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
int32
phy_common_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32       ret;
    uint32      data;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &data)) != RT_ERR_OK)
    {
         return ret;
    }

    *pStatus = ((data & LinkStatus_MASK) >> LinkStatus_OFFSET);

    return RT_ERR_OK;
} /* end of phy_common_linkStatus_get */

/* Function Name:
*      phy_common_copperPeerAutoNegoAbility_get
* Description:
*      Get ability from link partner advertisement auto-negotiation of the specific port
*      (For Copper port)
* Input:
*      unit - unit id
*      port - port id
* Output:
*      pAbility - pointer to PHY auto-negotiation ability
* Return:
*      RT_ERR_OK     - OK
*      RT_ERR_FAILED - invalid parameter
* Note:
*      None
*/
int32
phy_common_copperPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32              phyData5, phyData10;
    int32               ret;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_LINKPARTNER_REG, &phyData5)) != RT_ERR_OK)
        goto ERR;

    /* Check Asymmetric Pause Ability */
    if((phyData5 & AsymmetricPause_R5_MASK))
        pAbility->AsyFC = ENABLED;
    else
        pAbility->AsyFC = DISABLED;

    /* Check Pause Ability */
    if((phyData5 & Pause_R5_MASK))
        pAbility->FC = ENABLED;
    else
        pAbility->FC = DISABLED;

    /* Check 100-TX Full Duplex */
    if((phyData5 & _100Base_TX_FD_R5_MASK))
        pAbility->Full_100 = ENABLED;
    else
        pAbility->Full_100 = DISABLED;

    /* Check 100-TX Half Duplex */
    if((phyData5 & _100Base_TX_R5_MASK))
        pAbility->Half_100 = ENABLED;
    else
        pAbility->Half_100 = DISABLED;

    /* Check 10-T Full Duplex */
    if((phyData5 & _10Base_T_FD_R5_MASK))
        pAbility->Full_10 = ENABLED;
    else
        pAbility->Full_10 = DISABLED;

    /* Check 10-T Half Duplex */
    if((phyData5 & _10Base_T_R5_MASK))
        pAbility->Half_10 = ENABLED;
    else
        pAbility->Half_10 = DISABLED;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_STATUS_REG, &phyData10)) != RT_ERR_OK)
        goto ERR;

    /* Check 1000-T Full Duplex */
    if((phyData10 & _1000Base_TLinkPartnerFullDuplex_MASK))
        pAbility->Full_1000 = ENABLED;
    else
        pAbility->Full_1000 = DISABLED;

    /* Check 1000-T Half Duplex */
    if((phyData10 & _1000Base_TLinkPartnerHalfDuplex_MASK))
        pAbility->Half_1000 = ENABLED;
    else
        pAbility->Half_1000 = DISABLED;

    return RT_ERR_OK;

ERR:
    return ret;

} /* end of phy_common_copperPeerAutoNegoAbility_get */

/* Function Name:
*      phy_common_fiberPeerAutoNegoAbility_get
* Description:
*      Get ability from link partner advertisement auto-negotiation of the specific port
*      (For Fiber port)
* Input:
*      unit - unit id
*      port - port id
* Output:
*      pAbility - pointer to PHY auto-negotiation ability
* Return:
*      RT_ERR_OK     - OK
*      RT_ERR_FAILED - invalid parameter
* Note:
*      None
*/
int32
phy_common_fiberPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32              phyData5;
    int32               ret;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_LINKPARTNER_REG, &phyData5)) != RT_ERR_OK)
        goto ERR;

    /* Check Asymmetric Pause Ability */
    if((phyData5 & _1000BaseX_AsymmetricPause_R5_MASK))
        pAbility->AsyFC = ENABLED;
    else
        pAbility->AsyFC = DISABLED;

    /* Check Pause Ability */
    if((phyData5 & _1000BaseX_Pause_R5_MASK))
        pAbility->FC = ENABLED;
    else
        pAbility->FC = DISABLED;

    /* Check 1000-X Full Duplex */
    if((phyData5 & _1000BaseX_FullDuplex_R5_MASK))
        pAbility->Full_1000 = ENABLED;
    else
        pAbility->Full_1000 = DISABLED;

    /* Check 1000-X Half Duplex */
    if((phyData5 & _1000BaseX_HalfDuplex_R5_MASK))
        pAbility->Half_1000 = ENABLED;
    else
        pAbility->Half_1000 = DISABLED;


    /* 1000-X doesn't define the 100 and 10 ability */
    pAbility->Full_100 = DISABLED;
    pAbility->Half_100 = DISABLED;
    pAbility->Full_10 = DISABLED;
    pAbility->Half_10 = DISABLED;

    return RT_ERR_OK;

ERR:
    return ret;

} /* end of phy_common_fiberPeerAutoNegoAbility_get */


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
    uint32 endBit, uint32 startBit, uint32 *data)
{
    uint32  configVal, len, mask;
    int32   ret;

    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    if (endBit < startBit)
        return RT_ERR_INPUT;

    RT_ERR_CHK(hal_miim_read(unit, port, page, reg, &configVal), ret);

    len = endBit - startBit + 1;

    if (16 == len)
        *data = configVal;
    else
    {
        mask = (1 << len) - 1;
        *data = (configVal >> startBit) & mask;
    }

    return RT_ERR_OK;
}   /* end of phy_field_read */

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
    uint32 endBit, uint32 startBit, uint32 data)
{
    uint32  configVal, len, mask;
    int32   ret;

    len = endBit - startBit + 1;

    if (endBit < startBit)
        return RT_ERR_INPUT;

    if (16 == len)
        configVal = data;
    else
    {
        mask = (1 << len) - 1;

        RT_ERR_CHK(hal_miim_read(unit, port, page, reg, &configVal), ret);

        configVal &= ~(mask << startBit);
        configVal |= (data << startBit);
    }

    RT_ERR_CHK(hal_miim_write(unit, port, page, reg, configVal), ret);

    return ret;
}   /* end of phy_field_write */

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
int32
phy_common_eyeMonPixel_get(int32 xAxis, int32 yAxis, uint32 frameNum, void *pDb, uint32 val)
{
    uint32  *eyeData;

    eyeData = (uint32 *)pDb;

    eyeData[RTK_EYE_MON_DATA_POS(frameNum, xAxis, yAxis)] = val;

    return RT_ERR_OK;
}

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
int32
phy_common_eyeMonInfo_update(int32 xAxis, int32 yAxis, uint32 frameNum, void *pDb, uint32 val)
{
    rtk_bitmap_t    *eye;
    uint32          i, uy, dy, chkVal;

    eye = (rtk_bitmap_t *)pDb + (xAxis * BITMAP_ARRAY_CNT(RTK_EYE_MON_ARXIS_Y_MAX));

    uy = RTK_EYE_MON_Y_MAX + yAxis;
    dy = RTK_EYE_MON_Y_MAX - yAxis - 1;

    for (i = 0; i < 7; ++i)
    {
        chkVal = (val >> (i << 1)) & 0x3;
        if (chkVal & 0x2)
        {
            if (chkVal & 0x1)
            {
                BITMAP_SET(eye, dy);
            }
            else
            {
                BITMAP_SET(eye, uy);
            }
        }
    }

    return RT_ERR_OK;
}

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
phy_common_general_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    /* temporary support C45 only */
    return hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);
}

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
phy_common_general_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    /* temporary support C45 only */
    return hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
}

/* Function Name:
 *      phy_common_c45_autoNegoEnable_get
 * Description:
 *      Get the auto-negotiation state of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable - pointer to output the auto-negotiation state
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_common_c45_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_12) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_common_c45_autoNegoEnable_set
 * Description:
 *      Set the auto-negotiation state of the specific port and restart auto-negotiation for enabled state.
 * Input:
 *      unit - unit id
 *      port - port id
 *      enable - auto-negotiation state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_common_c45_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_12 | BIT_9));
    phyData |= (enable == ENABLED) ? (BIT_12 | BIT_9) : (0);

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 0, phyData);
    return ret;
}

/* Function Name:
 *      phy_common_c45_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto-negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto-negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1000 is vendor specific in C45.
 */
int32
phy_common_c45_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 16, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->Half_10  = (phyData & BIT_5) ? (1) : (0);
    pAbility->Full_10  = (phyData & BIT_6) ? (1) : (0);
    pAbility->Half_100 = (phyData & BIT_7) ? (1) : (0);
    pAbility->Full_100 = (phyData & BIT_8) ? (1) : (0);
    pAbility->FC       = (phyData & BIT_10) ? (1) : (0);
    pAbility->AsyFC    = (phyData & BIT_11) ? (1) : (0);

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->adv_2_5G      = (phyData & BIT_7) ? (1) : (0);
    pAbility->adv_5G        = (phyData & BIT_8) ? (1) : (0);
    pAbility->adv_10GBase_T = (phyData & BIT_12) ? (1) : (0);

    return ret;
}

/* Function Name:
 *      phy_common_c45_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto-negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto-negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1000 is vendor specific in C45.
 */
int32
phy_common_c45_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 16, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_5 | BIT_6 | BIT_7 | BIT_8 | BIT_10 | BIT_11));
    phyData |= (pAbility->Half_10) ? (BIT_5) : (0);
    phyData |= (pAbility->Full_10) ? (BIT_6) : (0);
    phyData |= (pAbility->Half_100) ? (BIT_7) : (0);
    phyData |= (pAbility->Full_100) ? (BIT_8) : (0);
    phyData |= (pAbility->FC) ? (BIT_10) : (0);
    phyData |= (pAbility->AsyFC) ? (BIT_11) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 16, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_7 | BIT_8 | BIT_12));
    phyData |= (pAbility->adv_2_5G) ? (BIT_7) : (0);
    phyData |= (pAbility->adv_5G) ? (BIT_8) : (0);
    phyData |= (pAbility->adv_10GBase_T) ? (BIT_12) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_12) /*AN is enabled*/
    {
        phyData |= BIT_9; /*AN restart*/
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 0, phyData)) != RT_ERR_OK)
            return ret;
    }

    return ret;
}

/* Function Name:
*      phy_common_c45_copperPeerAutoNegoAbility_get
* Description:
*      Get ability from link partner advertisement auto-negotiation of the specific port
*      (For Copper port)
* Input:
*      unit - unit id
*      port - port id
* Output:
*      pAbility - pointer to PHY auto-negotiation ability
* Return:
*      RT_ERR_OK     - OK
*      RT_ERR_FAILED - invalid parameter
* Note:
*      1000 is vendor specific in C45.
*/
int32
phy_common_c45_copperPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 19, &phyData)) != RT_ERR_OK)
        return ret;
    pAbility->Half_10  = (phyData & BIT_5) ? (1) : (0);
    pAbility->Full_10  = (phyData & BIT_6) ? (1) : (0);
    pAbility->Half_100 = (phyData & BIT_7) ? (1) : (0);
    pAbility->Full_100 = (phyData & BIT_8) ? (1) : (0);
    pAbility->FC       = (phyData & BIT_10) ? (1) : (0);
    pAbility->AsyFC    = (phyData & BIT_11) ? (1) : (0);

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 33, &phyData)) != RT_ERR_OK)
        return ret;
    pAbility->adv_2_5G      = (phyData & BIT_5) ? (1) : (0);
    pAbility->adv_5G        = (phyData & BIT_6) ? (1) : (0);
    pAbility->adv_10GBase_T = (phyData & BIT_11) ? (1) : (0);

    return ret;
}

/* Function Name:
 *      phy_common_c45_masterSlave_get
 * Description:
 *      Get master/slave setting and status of the specific port
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
int32
phy_common_c45_masterSlave_get(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_15) //MASTER-SLAVE manual config enable
    {
        if (phyData & BIT_14)
            *pMasterSlaveCfg = PORT_MASTER_MODE;
        else
            *pMasterSlaveCfg = PORT_SLAVE_MODE;
    }
    else
        *pMasterSlaveCfg = PORT_AUTO_MODE;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 33, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_14)
        *pMasterSlaveActual = PORT_MASTER_MODE;
    else
        *pMasterSlaveActual = PORT_SLAVE_MODE;

    return ret;
}

/* Function Name:
 *      phy_common_c45_masterSlave_set
 * Description:
 *      Set master/slave mode of the specific port
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
phy_common_c45_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_15 | BIT_14));

    switch (masterSlave)
    {
    case PORT_AUTO_MODE:
        break;
    case PORT_SLAVE_MODE:
        phyData |= BIT_15;
        break;
    case PORT_MASTER_MODE:
        phyData |= (BIT_15 | BIT_14);
        break;
    default:
        return RT_ERR_INPUT;
    }

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
        return ret;

    //AN restart
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;
    if (phyData & BIT_12)
    {
        phyData |= BIT_9;
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 0, phyData)) != RT_ERR_OK)
            return ret;
    }

    return ret;
}

/* Function Name:
 *      phy_common_c45_speed_get
 * Description:
 *      Get speed of the specific port
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
int32
phy_common_c45_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret;
    uint32  phyData = 0;
    uint8   speed_l = 0;
    uint8   speed_h = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    speed_l = (((phyData & BIT_6) ? 1 : 0) << 1) | ((phyData & BIT_13) ? 1 : 0);
    speed_h = (phyData & (BIT_5 | BIT_4 | BIT_3 | BIT_2)) >> 2;

    switch (speed_l)
    {
        case 0b00:
            *pSpeed = PORT_SPEED_10M;
            break;
        case 0b01:
            *pSpeed = PORT_SPEED_100M;
            break;
        case 0b10:
            *pSpeed = PORT_SPEED_1000M;
            break;
        case 0b11:
            switch (speed_h)
            {
                case 0b0000:
                    *pSpeed = PORT_SPEED_10G;
                    break;
                case 0b0111:
                    *pSpeed = PORT_SPEED_5G;
                    break;
                case 0b0110:
                    *pSpeed = PORT_SPEED_2_5G;
                    break;
                default:
                    *pSpeed = PORT_SPEED_10M;
                    break;
            }
            break;
        default:
            *pSpeed = PORT_SPEED_10M;
            break;
    }

    return ret;
}

/* Function Name:
 *      phy_common_c45_speed_set
 * Description:
 *      Set speed of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed rtk_port_speed_t
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
int32
phy_common_c45_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_13 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2));

    switch (speed)
    {
        case PORT_SPEED_10M:
            break;
        case PORT_SPEED_100M:
            phyData |= BIT_13;
            break;
        case PORT_SPEED_1000M:
            phyData |= BIT_6;
            break;
        case PORT_SPEED_10G:
            phyData |= (BIT_13 | BIT_6);
            break;
        case PORT_SPEED_2_5G:
            phyData |= (BIT_13 | BIT_6 | BIT_4 | BIT_3);
            break;
        case PORT_SPEED_5G:
            phyData |= (BIT_13 | BIT_6 | BIT_4 | BIT_3 | BIT_2);
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 0, phyData);
    return ret;
}

/* Function Name:
 *      phy_common_c45_enable_set
 * Description:
 *      Set PHY interface state enable/disable of the specific port
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
phy_common_c45_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~BIT_11);
    phyData |= (enable == ENABLED) ? (0) : (BIT_11);

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 0, phyData);
    return ret;
}

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
int32
phy_common_c45_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_11) ? (DISABLED) : (ENABLED);
    return ret;
}

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
int32
phy_common_c45_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PCS, 1, &phyData)) != RT_ERR_OK)
        return ret;

    *pStatus = (phyData & BIT_2) ? PORT_LINKUP : PORT_LINKDOWN;

    return ret;
}

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
int32
phy_common_c45_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PCS, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_14) ? ENABLED : DISABLED;

    return ret;
}

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
int32
phy_common_c45_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PCS, 0, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (enable == ENABLED) ? (phyData | BIT_14) : (phyData & ~BIT_14);

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PCS, 0, phyData);
    return ret;
}

/* Function Name:
 *      phy_common_c45_reset_set
 * Description:
 *      Set PHY Reset bit in standard register(1.0.15).
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
int32
phy_common_c45_reset_set(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    phyData |= BIT_15;

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 0, phyData);
    return ret;
}

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
int32
phy_common_c45_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData = 0;
    rtk_enable_t ena0 = 0;
    rtk_enable_t ena1 = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 60, &phyData)) != RT_ERR_OK)
        return ret;
    ena0 = ((phyData & (BIT_1 | BIT_2 | BIT_3)) == (BIT_1 | BIT_2 | BIT_3)) ? ENABLED : DISABLED;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 62, &phyData)) != RT_ERR_OK)
        return ret;
    ena1 = ((phyData & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1)) ? ENABLED : DISABLED;

    if (ena0 == ena1)
        *pEnable = ena0;
    else
        *pEnable = DISABLED;
    return ret;
}

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
int32
phy_common_c45_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData = 0;
    rtk_enable_t an;

    if ((ret = phy_common_c45_autoNegoEnable_get(unit, port, &an)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 60, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (enable == ENABLED) ? (phyData | (BIT_1 | BIT_2 | BIT_3)) : (phyData & ~(BIT_1 | BIT_2 | BIT_3));

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 60, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 62, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (enable == ENABLED) ? (phyData | (BIT_0 | BIT_1)) : (phyData & ~(BIT_0 | BIT_1));

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 62, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;
    an = (phyData & BIT_12) ? ENABLED : DISABLED;
    if (ENABLED == an)
    {
        phyData |= BIT_9;
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 0, phyData)) != RT_ERR_OK)
            return ret;
    }

    return ret;
}

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
int32
phy_common_c45_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 130, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1))
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
    else *pStatus = PORT_CROSSOVER_STATUS_MDIX;

    return ret;
}

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
int32
phy_common_c45_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    int32   ret = RT_ERR_FAILED;
    uint32  A, B, C, D, E, F, G, H, I, J, K, L, M, N, P, Q, R, S, T, temp;
    uint32  phyData = 0, phyData1 = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 1, &phyData)) != RT_ERR_OK)
        return ret;
    A = (phyData & BIT_0)? 1 : 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 0, &phyData)) != RT_ERR_OK)
            return ret;
    B = (phyData & BIT_12)? 1 : 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 1, 0, &phyData)) != RT_ERR_OK)
            return ret;
    C = (phyData & BIT_6)? 1 : 0;
    D = (phyData & BIT_13)? 1 : 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 19, &phyData)) != RT_ERR_OK)
        return ret;
    E = (phyData & BIT_7)? 1 : 0;
    F = (phyData & BIT_5)? 1 : 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 31, 41984, &phyData)) != RT_ERR_OK)
        return ret;
    G = (phyData & BIT_8)? 1 : 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 32, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 33, &phyData1)) != RT_ERR_OK)
        return ret;
    temp = (phyData & BIT_12)? 1 : 0;
    H = (phyData1 & BIT_11)? 1 : 0;
    H = H & temp;

    temp = (phyData & BIT_8)? 1 : 0;
    I = (phyData1 & BIT_6)? 1 : 0;
    I = I & temp;

    temp = (phyData & BIT_7)? 1 : 0;
    J = (phyData1 & BIT_5)? 1 : 0;
    J = J & temp;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 31, 42002, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, 31, 42004, &phyData1)) != RT_ERR_OK)
        return ret;
    temp = (phyData & BIT_9)? 1 : 0;
    K = (phyData1 & BIT_11)? 1 : 0;
    K = K & temp;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 16, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 19, &phyData1)) != RT_ERR_OK)
        return ret;

    temp = (phyData & BIT_8)? 1 : 0;
    L = (phyData1 & BIT_8)? 1 : 0;
    L = L & temp;

    temp = (phyData & BIT_7)? 1 : 0;
    M = (phyData1 & BIT_7)? 1 : 0;
    M = M & temp;

    temp = (phyData & BIT_6)? 1 : 0;
    N = (phyData1 & BIT_6)? 1 : 0;
    N = N & temp;

    temp = (phyData & BIT_5)? 1 : 0;
    P = (phyData1 & BIT_5)? 1 : 0;
    P = P & temp;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 1, 0, &phyData)) != RT_ERR_OK)
            return ret;
    Q = (phyData & BIT_5)? 1 : 0;
    R = (phyData & BIT_4)? 1 : 0;
    S = (phyData & BIT_3)? 1 : 0;
    T = (phyData & BIT_2)? 1 : 0;

    if ((A & B & H) | (~B & C & D & ~Q & ~R & ~S & ~T))
    {
        *pSpeed = PORT_SPEED_10G;
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else if ((A & B & ~H & I) | (~B & C & D & ~Q & R & S & T))
    {
        *pSpeed = PORT_SPEED_5G;
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else if ((A & B & ~H & ~I & J) | (~B & C & D & ~Q & R & S & ~T))
    {
        *pSpeed = PORT_SPEED_2_5G;
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else if ((A & B & ~H &~I & ~J & K) | (~B & C & ~D & G))
    {
        *pSpeed = PORT_SPEED_1000M;
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else if ((~B & ~C & D & ~G) | (~A & B & E) | (A & B & ~H & ~I & ~J & ~K & ~L & M))
    {
        *pSpeed = PORT_SPEED_100M;
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else if ((~B & ~C & ~D & G) | (A & B & ~H & ~I & ~J & ~K & ~L & ~M & N))
    {
        *pSpeed = PORT_SPEED_100M;
        *pDuplex = PORT_HALF_DUPLEX;
    }
    else if ((A & B & ~H & ~I & ~J & ~K & ~L & ~M & ~N & P) | (~B & ~C & ~D & ~G)| (~A & B & ~E & F))
    {
        *pSpeed = PORT_SPEED_10M;
        *pDuplex = PORT_FULL_DUPLEX;
    }
    else
    {
        *pSpeed = PORT_SPEED_10M;
        *pDuplex = PORT_HALF_DUPLEX;
    }

    return RT_ERR_OK;
}

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
int32
phy_common_c45_speedDuplexStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, spd;

    *pSpeed = PORT_SPEED_10M;
    *pDuplex = PORT_HALF_DUPLEX;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA434, &phyData)) != RT_ERR_OK)
        return ret;

    /* [10:9,5:4] */
    spd = ((phyData & (0x3 << 9)) >> (9 - 2)) | ((phyData & (0x3 << 4)) >> 4);
    switch (spd)
    {
        case 0x0:
          *pSpeed = PORT_SPEED_10M;
          break;
        case 0x1:
          *pSpeed = PORT_SPEED_100M;
          break;
        case 0x2:
          *pSpeed = PORT_SPEED_1000M;
          break;
        case 0x3:
          *pSpeed = PORT_SPEED_500M;
          break;
        case 0x4:
          *pSpeed = PORT_SPEED_10G;
          break;
        case 0x5:
          *pSpeed = PORT_SPEED_2_5G;
          break;
        case 0x6:
          *pSpeed = PORT_SPEED_5G;
          break;
        case 0x7:
          *pSpeed = PORT_SPEED_2_5G_LITE;
          break;
        case 0x8:
          *pSpeed = PORT_SPEED_5G_LITE;
          break;
        case 0x9:
          *pSpeed = PORT_SPEED_10G_LITE;
          break;
        default:
          *pSpeed = PORT_SPEED_10M;
          break;
    }

    *pDuplex = (phyData & 0x8)? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_common_c45_speedStatusResReg_get
 * Description:
 *      Get operational link speed status from proprietary register
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
int32
phy_common_c45_speedStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret = RT_ERR_FAILED;
    rtk_port_duplex_t duplex;
    *pSpeed = PORT_SPEED_10M;
    ret = phy_common_c45_speedDuplexStatusResReg_get(unit, port, pSpeed, &duplex);

    return ret;
}

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
 *      1. disable ALPDS (fPhydrv_linkDownPowerSavingEnable_set)
 *      2. set speed (fPhydrv_speed_set)
 *      3. disable auto-negotiation (fPhydrv_autoNegoAbility_set)
 */
int32
phy_common_c45_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    int32   ret;
    uint32  phyData = 0;

    //1G_TEST_MODE_NONE
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, (BIT_11 | BIT_10 | BIT_9))) != RT_ERR_OK)
        return ret;

    //10G_TEST_MODE_NONE
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, BIT_10)) != RT_ERR_OK)
        return ret;

    switch (pTestMode->mode)
    {
        case RTK_PORT_PHY_1G_TEST_MODE_NONE:
            break;
        case RTK_PORT_PHY_1G_TEST_MODE1:
            phyData = (BIT_13 | BIT_11 | BIT_10 | BIT_9);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, phyData);
            break;
        case RTK_PORT_PHY_1G_TEST_MODE2:
            phyData = (BIT_14 | BIT_11 | BIT_10 | BIT_9);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, phyData);
            break;
        case RTK_PORT_PHY_1G_TEST_MODE3:
            phyData = (BIT_14 | BIT_13 | BIT_11 | BIT_10 | BIT_9);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, phyData);
            break;
        case RTK_PORT_PHY_1G_TEST_MODE4:
            phyData = (BIT_15 | BIT_11 | BIT_10 | BIT_9);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, phyData);
            break;

        case RTK_PORT_PHY_10G_TEST_MODE_NONE:
            break;
        case RTK_PORT_PHY_10G_TEST_MODE1:
            phyData = (BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE2:
            phyData = (BIT_14 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE3:
            phyData = (BIT_14 | BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE1:
            phyData = (BIT_15 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE2:
            phyData = (BIT_15 | BIT_11);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE3:
            phyData = (BIT_15 | BIT_12);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE4:
            phyData = (BIT_15 | BIT_12 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE5:
            phyData = (BIT_15 | BIT_12 | BIT_11);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE5:
            phyData = (BIT_15 | BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE6:
            phyData = (BIT_15 | BIT_14 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE7:
            phyData = (BIT_15 | BIT_14 | BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }

    return ret;
}


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
int32
phy_common_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData4 = 0;
    uint32          phyData9 = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4), ret);

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;
    pAbility->Full_100 = (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100 = (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10 = (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10 = (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9), ret);
    pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
    pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;

    return ret;
}

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
int32
phy_common_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData0 = 0;
    uint32          phyData4 = 0;
    uint32          phyData9 = 0;
    rtk_enable_t    enable = DISABLED;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4), ret);

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
            | (pAbility->FC << Pause_R4_OFFSET)
            | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

    phyData4 = phyData4 &
            ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
    phyData4 = phyData4
            | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
            | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
            | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
            | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9), ret);

    phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
    phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
               | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9), ret);

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0), ret);
        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);
        RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0), ret);
    }

    return ret;
}

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
int32
phy_common_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData0 = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0), ret);

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0), ret);

    return ret;
}

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
int32
phy_common_c45_an_restart(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_12) /*AN is enabled*/
    {
        phyData |= BIT_9; /*AN restart*/
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 0, phyData)) != RT_ERR_OK)
            return ret;
    }
    return ret;
}
