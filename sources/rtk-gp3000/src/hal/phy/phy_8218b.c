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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/common/halctrl.h>
#include <hal/common/miim.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_8218b.h>
#include <hal/phy/phy_common.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
rt_phydrv_t phy_8218b_drv_int_ge =
{
    RT_PHYDRV_8218B_INT_GE,
    phy_8218b_init,
    (int32 (*)(rtk_port_t, rtk_port_media_t *))phy_common_unavail,
    (int32 (*)(rtk_port_t, rtk_port_media_t))phy_common_unavail,
    phy_8218b_autoNegoEnable_get,
    phy_8218b_autoNegoEnable_set,
    phy_8218b_autoNegoAbility_get,
    phy_8218b_autoNegoAbility_set,
    phy_8218b_duplex_get,
    phy_8218b_duplex_set,
    phy_8218b_speed_get,
    phy_8218b_speed_set,
    phy_8218b_enable_set,
    (int32 (*)(rtk_port_t, rtk_rtctResult_t *))phy_common_unavail,
    (int32 (*)(rtk_port_t))phy_common_unavail,
    phy_8218b_greenEnable_get,
    phy_8218b_greenEnable_set,
    phy_8218b_eeeEnable_get,
    phy_8218b_eeeEnable_set,
    phy_8218b_crossOverMode_get,
    phy_8218b_crossOverMode_set,
    (int32 (*)(rtk_port_t, rtk_enable_t))phy_common_unavail,
    (int32 (*)(rtk_port_t, uint32))phy_common_unavail,
}; /* end of phy_8218drv_ge */

/*
 * Function Declaration
 */

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
int32
phy_8218b_init(rtk_port_t port)
{
    if(port)
    {
    }
    return RT_ERR_OK;
} /* end of phy_8218b_init */

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
int32
phy_8218b_autoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
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
int32
phy_8218b_autoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(AutoNegotiationEnable_MASK | RestartAutoNegotiation_MASK);
    phyData0 = phyData0 | ((enable << AutoNegotiationEnable_OFFSET) | (1 << RestartAutoNegotiation_OFFSET));

    if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_common_autoNegoEnable_set */

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
int32
phy_8218b_autoNegoAbility_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_8218b_autoNegoEnable_get(port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
    pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
    pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;

    return ret;
} /* end of phy_8218b_autoNegoAbility_get */

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
int32
phy_8218b_autoNegoAbility_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_8218b_autoNegoEnable_get(port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
            | (pAbility->FC << Pause_R4_OFFSET)
            | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 &
            ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
    phyData4 = phyData4
            | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
            | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
            | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
            | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

    phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
    phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
               | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

    if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
        return ret;


    if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8218b_autoNegoAbility_set */


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
int32
phy_8218b_duplex_get(rtk_port_t port, uint32 *pDuplex)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pDuplex = (phyData0 & DuplexMode_MASK) >> DuplexMode_OFFSET;

    return ret;
} /* end of phy_common_duplex_get */

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
int32
phy_8218b_duplex_set(rtk_port_t port, uint32 duplex)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(DuplexMode_MASK);
    phyData0 = phyData0 | (duplex << DuplexMode_OFFSET);

    if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_common_duplex_set */

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
int32
phy_8218b_speed_get(rtk_port_t port, uint32 *pSpeed)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
              | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

    return ret;
} /* end of phy_8218b_speed_get */

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
int32
phy_8218b_speed_set(rtk_port_t port, uint32 speed)
{
    int32   ret;
    uint32  phyData0;

    RT_PARAM_CHK(speed == PORT_SPEED_1000M, RT_ERR_CHIP_NOT_SUPPORTED);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218b_speed_set */

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
int32
phy_8218b_enable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData;

    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phyData &= ~(PowerDown_MASK);

        if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = hal_miim_read(port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phyData |= (1 << PowerDown_OFFSET);

        /* E0006794: Configuration for LPI mode to set power-down bit */
        if (phyData & AutoNegotiationEnable_MASK)
            phyData |= (1 << RestartAutoNegotiation_OFFSET);
        /* E0006794 */

        if ((ret = hal_miim_write(port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }
    return ret;
} /* end of phy_8218b_enable_set */

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
int32
phy_8218b_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    /* TBD */
    if(port || pEnable)
    {
    }

    return RT_ERR_OK;
} /* end of phy_8218b_greenEnable_get */

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
int32
phy_8218b_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    /* TBD */
    if(port || enable)
    {
    }

    return RT_ERR_OK;
} /* end of phy_8218b_greenEnable_set */

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
int32
phy_8218b_eeeEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    /* TBD */
    if(port || pEnable)
    {
    }
    return RT_ERR_OK;
} /* end of phy_8218b_eeeEnable_get */

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
int32
phy_8218b_eeeEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    /* TBD */
    if(port || enable)
    {
    }

    return RT_ERR_OK;
} /* end of phy_8218b_eeeEnable_set */

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
int32
phy_8218b_crossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    /* TBD */
    if(port || pMode)
    {
    }
    return RT_ERR_OK;
} /* end of phy_8218b_crossOverMode_get */

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
int32
phy_8218b_crossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    /* TBD */
    if(port || mode)
    {
    }
    return RT_ERR_OK;
} /* end of phy_8218b_crossOverMode_set */
