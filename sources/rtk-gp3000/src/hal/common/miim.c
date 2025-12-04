/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <hal/mac/drv.h>


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      hal_miim_read
 * Description:
 *      Get PHY registers.
 * Input:
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
int32
hal_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, page=0x%x, phy_reg=0x%x", port, page, phyReg);
    RT_PARAM_CHK(pData == NULL,RT_ERR_NULL_POINTER);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_read)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pPhy_ctrl[port])
    {
        return RT_ERR_FAILED;
    }

    ret = MACDRV(pHalCtrl)->fMdrv_miim_read(port, page, phyReg, pData);
    return ret;
} /* end of hal_miim_read */


/* Function Name:
 *      hal_miim_write
 * Description:
 *      Set PHY registers.
 * Input:
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
int32
hal_miim_write(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{

    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", port, page, phyReg, data);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_write)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pPhy_ctrl[port])
    {
        return RT_ERR_FAILED;
    }

    ret = MACDRV(pHalCtrl)->fMdrv_miim_write(port, page, phyReg, data);
    return ret;
} /* end of hal_miim_write */

/* Function Name:
 *      phy_autoNegoEnable_get
 * Description:
 *      Get auto negotiation enable status of the specific port
 * Input:
 *      port - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_autoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoEnable_get(port, pEnable));
} /* end of phy_autoNegoEnable_get */

/* Function Name:
 *      phy_autoNegoEnable_set
 * Description:
 *      Set auto negotiation enable status of the specific port
 * Input:
 *      port          - port id
 *      enable        - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_autoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, enable=%d", port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoEnable_set(port, enable));
} /* end of phy_autoNegoEnable_set */

/* Function Name:
 *      phy_autoNegoAbility_get
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
phy_autoNegoAbility_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoAbility_get(port, pAbility));
}

/* Function Name:
 *      phy_autoNegoAbility_set
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
phy_autoNegoAbility_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PORT_EXIST(port))
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    if(HAL_IS_FE_PORT(port))
    {
        pAbility->Half_1000     = DISABLED;
        pAbility->Full_1000     = DISABLED;

    }

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoAbility_set(port, pAbility));
}

/* Function Name:
 *      phy_duplex_get
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
phy_duplex_get(rtk_port_t port, uint32 *pDuplex)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_duplex_get(port, pDuplex));
} /* end of phy_duplex_get */

/* Function Name:
 *      phy_duplex_set
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
phy_duplex_set(rtk_port_t port, uint32 duplex)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, duplex=%d", port, duplex);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_duplex_set(port, duplex));
} /* end of phy_duplex_set */

/* Function Name:
 *      phy_speed_get
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
phy_speed_get(rtk_port_t port, uint32 *pSpeed)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_speed_get(port, pSpeed));
} /* end of phy_speed_get */


/* Function Name:
 *      phy_speed_set
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
 * Note:
 *      None
 */
int32
phy_speed_set(rtk_port_t port, uint32 speed)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, speed=%d", port, speed);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PORT_EXIST(port))
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    if(HAL_IS_FE_PORT(port) && ((speed == PORT_SPEED_1000M) || (speed == PORT_SPEED_500M)))
    {
        return RT_ERR_INPUT;
    }

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_speed_set(port, speed));
} /* end of phy_speed_set */

/* Function Name:
 *      phy_enable_set
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
phy_enable_set(rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, enable=%d", port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_enable_set(port, enable);
} /* end of phy_enable_set */

/* Function Name:
 *      phy_greenEnable_get
 * Description:
 *      Get the status of green feature of the specific port
 * Input:
 *      port - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_greenEnable_get(port, pEnable);
} /* end of phy_greenEnable_get */

/* Function Name:
 *      phy_greenEnable_set
 * Description:
 *      Set the status of green feature of the specific port
 * Input:
 *      port   - port id
 *      enable - status of green feature
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
phy_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, enable=%d", port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_greenEnable_set(port, enable);
} /* end of phy_greenEnable_set */

/* Function Name:
 *      phy_eeeEnable_get
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
phy_eeeEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_eeeEnable_get(port, pEnable));
} /* end of phy_eeeEnable_get */

/* Function Name:
 *      phy_eeeEnable_set
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
phy_eeeEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, enable=%d", port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_eeeEnable_set(port, enable);
} /* end of phy_eeeEnable_set */

/* Function Name:
 *      phy_crossOverMode_get
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
phy_crossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d", port);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_crossOverMode_get(port, pMode));
} /* end of phy_crossOverMode_get */

/* Function Name:
 *      phy_crossOverMode_set
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
phy_crossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "port=%d, enable=%d", port, mode);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (!HAL_IS_PHY_EXIST(port))
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_crossOverMode_set(port, mode);
} /* end of phy_crossOverMode_set */
