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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <soc/type.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#if defined(CONFIG_SDK_RTL8212B) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB)
#include <hal/phy/phy_rtl8214fb.h>
#endif
#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
#include <hal/phy/phy_rtl8218b.h>
#endif
#include <hal/mac/miim_common_drv.h>
#include <osal/time.h>
#include <hal/phy/phydef.h>
#include <hal/phy/identify.h>
#include <ioal/phy_reset.h>

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
#include <hal/phy/phy_rtl8295.h>
#endif
#if defined(CONFIG_SDK_PHY_CUST1)
#include <hal/phy/phy_cust1.h>
#endif
#if defined(CONFIG_SDK_RTL8218D)
#include <hal/phy/phy_rtl8218d.h>
#endif
#if defined(CONFIG_SDK_RTL8218E)
#include <hal/phy/phy_rtl8218e.h>
#endif


#define MIIM_NULL_PHY_DRV(pHalCtrl, port)            ((port >= RTK_MAX_PORT_PER_UNIT) || (pHalCtrl->pPhy_ctrl[port] == NULL) || (pHalCtrl->pPhy_ctrl[port]->pPhydrv == NULL))
#define MIIM_PHY_API_AVAILABLE(_function)            ((_function) != (void *)phy_common_unavail)

#define MIIM_PHY_CHIP_REV_ID_EMPTY          0xFFFF
uint16 gMiim_phyRevId[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
uint32 gMiim_phyRevIdInit = INIT_NOT_COMPLETED;

/*
 * Macro Definition
 */

/* MIIM semaphore handling */
#define MIIM_SEM_LOCK(unit)\
do {\
    hal_control_t   *pHalCtrl;  \
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)    \
        return RT_ERR_FAILED;   \
    if (osal_sem_mutex_take(pHalCtrl->miim_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "MIIM semaphore lock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)

#define MIIM_SEM_UNLOCK(unit)\
do {\
    hal_control_t   *pHalCtrl;  \
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)    \
        return RT_ERR_FAILED;   \
    if (osal_sem_mutex_give(pHalCtrl->miim_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "MIIM semaphore unlock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)

/*
 * Data Declaration
 */

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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      1. media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 */
int32
phy_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_media_get(unit, port, pMedia));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_media_get */


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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      1. media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 */
int32
phy_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, media=%d", unit, port, media);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);


    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_media_set(unit, port, media));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_media_set */


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
int32
phy_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_autoNegoEnable_get */

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
int32
phy_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoEnable_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_autoNegoEnable_set */

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
int32
phy_autoNegoAbilityLocal_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoAbilityLocal_get(unit, port, pAbility);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoAbility_get(unit, port, pAbility));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, pAbility=0x%x", unit, port, pAbility);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_autoNegoAbility_set(unit, port, pAbility));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_duplex_get(unit, port, pDuplex));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_duplex_get */

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
int32
phy_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, duplex=%d", unit, port, duplex);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_duplex_set(unit, port, duplex));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_duplex_set */

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
int32
phy_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_speed_get(unit, port, pSpeed));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_speed_get */


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
int32
phy_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, speed=%d", unit, port, speed);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_speed_set(unit, port, speed));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_speed_set */

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
int32
phy_speedStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_speedStatus_get(unit, port, pSpeed));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_enable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_enable_set */

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
int32
phy_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_enable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_rtct_start(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_rtct_start(unit, port));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_rtct_start */

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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_rtctResult_get(unit, port, pRtctResult));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_rtctResult_get */

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
int32
phy_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_greenEnable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_greenEnable_get */

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
int32
phy_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_greenEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_greenEnable_set */

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
int32
phy_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_eeeEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_eeeEnable_get */

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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_eeeEnable_set(unit, port, enable);

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_eeeEnable_set */

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
int32
phy_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_crossOverMode_get(unit, port, pMode));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_crossOverMode_get */

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
int32
phy_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, mode);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_crossOverMode_set(unit, port, mode);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_crossOverMode_set */

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
int32
phy_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_crossOverStatus_get(unit, port, pStatus));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_crossOverStatus_get */


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
int32
phy_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiber_media_get(unit, port, pMedia));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiber_media_get */


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
int32
phy_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, media=%d", unit, port, media);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiber_media_set(unit, port, media));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiber_media_set */

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
int32
phy_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_linkDownPowerSavingEnable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_linkDownPowerSavingEnable_get */

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
int32
phy_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_linkDownPowerSavingEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_linkDownPowerSavingEnable_set */

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
int32
phy_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_broadcastEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_broadcastEnable_set */

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
int32
phy_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, broadcastID=%d", unit, port, broadcastID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_broadcastID_set(unit, port, broadcastID);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_broadcastID_set */

/* Function Name:
 *      phy_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
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
int32
phy_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_gigaLiteEnable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_gigaLiteEnable_get */

/* Function Name:
 *      phy_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
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
int32
phy_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_gigaLiteEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_gigaLiteEnable_set */

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
int32
phy_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_eeepEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_eeepEnable_get */

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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_eeepEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_eeepEnable_set */


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
int32
phy_patch_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_patch_set(unit, port);
    MIIM_SEM_UNLOCK(unit);

    if (RT_ERR_OK != ret)
        return ret;

    return ret;
} /* end of phy_patch_set */

/* Function Name:
 *      _phy_serdes_linkdown_chk
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
 * Note:
 *      None
 */
int32
_phy_serdes_linkdown_chk(uint32 unit, rtk_port_t port, uint32 * status)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    switch (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
    {
      #ifdef CONFIG_SDK_RTL8218D
        case RT_PHYDRV_RTL8218D:
            phy_8218d_serdes_linkdown_chk(unit, port, status);
        break;
      #endif
      #ifdef CONFIG_SDK_RTL8214FC
        case RT_PHYDRV_RTL8214FC:
            phy_8214fc_serdes_linkdown_chk(unit, port, status);
        break;
      #endif
      #ifdef CONFIG_SDK_RTL8218E
        case RT_PHYDRV_RTL8218E:
            phy_8218e_serdes_linkdown_chk(unit, port, status);
        break;
      #endif
      default:
        break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_chk_rst_status
 * Description:
 *      Check the PHY had been reset or NOT.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pPhy_rst_flag   - 1 : PHY had been reset
 *      pPort_rst_flag  - 1 : port had been reset
 *      pRst_portmask   - reset port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED    - PHY doesn't support this feature
 * Note:
 *      None
 */
int32
_phy_chk_rst_status(uint32 unit, rtk_port_t port, uint32 *pPhy_rst_flag, uint32 *pPort_rst_flag, rtk_portmask_t *pRst_portmask)
{
    int32           ret = RT_ERR_OK;
    rtk_enable_t    pollingSts = DISABLED;
    hal_control_t   *pHalCtrl;
#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218D) || defined(CONFIG_SDK_RTL8214C) || defined(CONFIG_SDK_RTL8218E)
    uint32  phy_data0, phy_data1;
#endif
#if defined(CONFIG_SDK_RTL8218D) || defined(CONFIG_SDK_RTL8218E)
    uint32 phy_data2;
#endif

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    /* Back up MAC polling status and disable polling */
    if ((ret = hal_miim_pollingWa_lock(unit, port, &pollingSts)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret);
        return ret;
    }

    *pPhy_rst_flag = 0x0;
    *pPort_rst_flag = 0x0;
    osal_memset(pRst_portmask, 0, sizeof(rtk_portmask_t));

    switch (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
    {
    #ifdef CONFIG_SDK_RTL8218B
        case RT_PHYDRV_RTL8218B:
    #endif /* CONFIG_SDK_RTL8218B */
    #ifdef CONFIG_SDK_RTL8218FB
        case RT_PHYDRV_RTL8218FB_MP:
    #endif /* CONFIG_SDK_RTL8218FB */
    #if defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8218B)
            hal_miim_write(unit, port, 0x0, 27, 0x801e);
            hal_miim_read(unit, port, 0x0, 28, &phy_data0);
            /*Double check*/
            hal_miim_read(unit, port, 0x0, 28, &phy_data1);
            if ((phy_data0 != 0x1) && (phy_data1 != 0x1))
            {
                *pPhy_rst_flag = 0x1;   /* Had been reset */
            }
            else
            {
                *pPhy_rst_flag = 0x0;
            }
            break;
    #endif /* defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8218B) */

    #ifdef CONFIG_SDK_RTL8218D
        case RT_PHYDRV_RTL8218D:
        {
            uint32      blk = 0, data_tmp;
            uint32      i, j, port_tmp;

            /* initial the data as expected value to prevent the read fail condition triggers reset.
             * the access fail reset condition shall trigger from top of the esd check process.
             */
            phy_data0 = 0x8218;
            phy_data1 = 0x8218;
            phy_data2 = 0;
            /* Set default value for reset checking in waMon */
            hal_miim_read(unit, port, 0, 30, &blk);
            hal_miim_write(unit, port, 0, 30, 8);
            hal_miim_read(unit, port, 0x327, 16, &phy_data0);
            hal_miim_read(unit, port, 0x327, 16, &phy_data1);
            hal_miim_read(unit, port, 0x346, 17, &phy_data2);
            hal_miim_write(unit, port, 0, 30, blk);
            phy_data2 = (phy_data2 >>8) & 0xff;
            if ((phy_data2 != 0) || ((phy_data0 != 0x8218) && (phy_data1 != 0x8218)))
            {
                *pPhy_rst_flag = 0x1;   /* Had been reset */
                RT_LOG(LOG_WARNING, (MOD_PHY), "port %u data0=%x data1=%x data2=%x\n", port, phy_data0, phy_data1, phy_data2);
                /* related reg information */
                if (0 != phy_data2)
                {
                    for (i = 0; i < 8; i++)
                    {
                        port_tmp = port + i;
                        hal_miim_read(unit, port_tmp, 0, 30, &blk);
                        hal_miim_write(unit, port_tmp, 0, 30, 0);
                        data_tmp = 0;
                        for (j = 0; j < 2; j++)
                        {
                            ret = hal_miim_read(unit, port_tmp, 0xa40, 0, &data_tmp);
                            RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa40,0=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        }
                        for (j = 0; j < 20; j++)
                        {
                            ret = hal_miim_read(unit, port_tmp, 0xa60, 16, &data_tmp);
                            RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa60,16=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                            ret = hal_miim_read(unit, port_tmp, 0xa70, 20, &data_tmp);
                            RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa70,20=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        }
                        ret = hal_miim_read(unit, port_tmp, 0xb90, 20, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xb90,20=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        hal_miim_write(unit, port_tmp, 0, 30, blk);
                    }
                }
                else
                {
                    port_tmp = port;
                    hal_miim_read(unit, port, 0, 30, &blk);
                    hal_miim_write(unit, port, 0, 30, 8);
                    ret = hal_miim_read(unit, port, 0x361, 16, &data_tmp);
                    RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0x361,16=%x (rv=%x)\n", port, data_tmp, ret);
                    ret = hal_miim_read(unit, port, 0x361, 17, &data_tmp);
                    RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0x361,17=%x (rv=%x)\n", port, data_tmp, ret);
                    hal_miim_write(unit, port, 0, 30, blk);
                    for (i = 0; i < 8; i++)
                    {
                        port_tmp = port + i;
                        hal_miim_read(unit, port_tmp, 0, 30, &blk);
                        hal_miim_write(unit, port_tmp, 0, 30, 0);
                        ret = hal_miim_read(unit, port_tmp, 0xbc1, 18, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port_tmp %u 0xbc1,18=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xbc1, 19, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xbc1,19=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xbc1, 20, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xbc1,20=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xa42, 22, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa42,22=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_write(unit, port_tmp, 0xa43, 27, 0x809a);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa43,27 write 0x809a (rv=%x)\n", port_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xa43, 28, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa43,28=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        hal_miim_write(unit, port_tmp, 0, 30, blk);
                    }
                }
            }
            else
            {
                *pPhy_rst_flag = 0x0;
            }

            break;
        }
    #endif /* CONFIG_SDK_RTL8218D */

    #ifdef CONFIG_SDK_RTL8214FC
        case RT_PHYDRV_RTL8214FC_MP:
        {
            uint32      blk;

            /* Set default value for reset checking in waMon */
            hal_miim_read(unit, port, 0, 30, &blk);
            hal_miim_write(unit, port, 0, 30, 8);
            hal_miim_read(unit, port, 0x268, 16, &phy_data0);
            hal_miim_read(unit, port, 0x268, 16, &phy_data1);
            hal_miim_write(unit, port, 0, 30, blk);
            phy_data0 &= 0xF000;
            phy_data1 &= 0xF000;

            if ((phy_data0 != 0xA000) && (phy_data1 != 0xA000))
            {
                *pPhy_rst_flag = 0x1;   /* Had been reset */
            }
            else
            {
                *pPhy_rst_flag = 0x0;
            }

            break;
        }
    #endif /* CONFIG_SDK_RTL8214FC */

    #if defined(CONFIG_SDK_RTL8214C)
        case RT_PHYDRV_RTL8214C:
            hal_miim_write(unit, port, 0, 29, 0x1322);
            hal_miim_read(unit, port, 0, 30, &phy_data0);
            phy_data0 = (phy_data0>>4) & 0x1;

            /* Double check */
            hal_miim_write(unit,  port, 0,  29, 0x1322);
            hal_miim_read(unit,  port,  0,  30, &phy_data1);
            phy_data1 = (phy_data1>>4) & 0x1;

            if ((phy_data0 != 0x1) && (phy_data1 != 0x1))
            {
                *pPhy_rst_flag = 0x1;   /* Had been reset */
            }
            else
            {
                *pPhy_rst_flag = 0x0;
            }
            break;
    #endif /* CONFIG_SDK_RTL8214C */

    #ifdef CONFIG_SDK_RTL8218E
        case RT_PHYDRV_RTL8218E:
        {
            uint32  blk = 0;
            uint32  phyPort = 0;
            int32   i;

            hal_miim_read(unit, port, 0, 30, &blk);
            hal_miim_write(unit, port, 0, 30, 0x8);
            hal_miim_read(unit, port, 0x346, 17, &phy_data2);
            hal_miim_write(unit, port, 0, 30, blk);
            phy_data2 = (phy_data2 >> 8) & 0xff;
            if (phy_data2 != 0)
            {
                *pPort_rst_flag = 0x1;
                for (i = 0; i < HWP_PHY_BASE_PHY_MAX(unit, port); i++)
                {
                    if (phy_data2 & (1 << i))
                    {
                        phyPort = port + i;
                        RTK_PORTMASK_PORT_SET(*pRst_portmask, phyPort);
                    }
                }
            }

            if (0 == *pPort_rst_flag)
            {
                for (i = 0; i < HWP_PHY_BASE_PHY_MAX(unit, port); i++)
                {
                    phyPort = port + i;
                    hal_miim_read(unit, phyPort, 0, 30, &blk);
                    hal_miim_write(unit, phyPort, 0, 30, 0);
                    hal_miim_read(unit, phyPort, 0xa43, 27, &phy_data2);
                    hal_miim_write(unit, phyPort, 0xa43, 27, 0x8211);
                    hal_miim_read(unit, phyPort, 0xa43, 28, &phy_data0);
                    hal_miim_write(unit, phyPort, 0xa43, 27, 0x8213);
                    hal_miim_read(unit, phyPort, 0xa43, 28, &phy_data1);
                    hal_miim_write(unit, phyPort, 0xa43, 27, phy_data2);
                    hal_miim_read(unit, phyPort, 0xa43, 28, &phy_data2);
                    hal_miim_write(unit, phyPort, 0, 30, blk);
                    if ((phy_data0 != 0x0800) || (phy_data1 != 0x0FFF))
                    {
                        *pPort_rst_flag = 0x1;
                        RTK_PORTMASK_PORT_SET(*pRst_portmask, phyPort);
                    }
                }
            }

            if (0 == *pPhy_rst_flag)
            {
                hal_miim_read(unit, port, 0, 30, &blk);
                hal_miim_write(unit, port, 0, 30, 0x8);
                hal_miim_read(unit, port, PHY_8218E_DUMMY_REG_PAGE, PHY_8218E_DUMMY_REG_REG, &phy_data2);
                hal_miim_write(unit, port, 0, 30, blk);
                if (0 == phy_data2)
                {
                    *pPhy_rst_flag = 0x1;      /* Had been reset */
                }
            }

            break;
        }
    #endif /* CONFIG_SDK_RTL8218E */

        default:
            break;
    }

    /* Restore MAC polling status */
    if ((ret = hal_miim_pollingWa_unlock(unit, port, pollingSts)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret);
        return ret;
    }

    return RT_ERR_OK;
} /* end of _phy_chk_rst_status */

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
 *      RT_ERR_CHIP_NOT_SUPPORTED    - PHY doesn't support this feature
 * Note:
 *      None
 */
int32
phy_chk_rst_status(uint32 unit, rtk_port_t port, uint32 * status)
{
    int32           ret = RT_ERR_OK;
    rtk_enable_t    pollingSts = DISABLED;
    hal_control_t   *pHalCtrl;
#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218D) || defined(CONFIG_SDK_RTL8214C)
    uint32  phy_data0, phy_data1;
#endif
#if defined(CONFIG_SDK_RTL8218D) || defined(CONFIG_SDK_RTL8218E)
    uint32 phy_data2;
#endif

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    /* Back up MAC polling status and disable polling */
    if ((ret = hal_miim_pollingWa_lock(unit, port, &pollingSts)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret);
        return ret;
    }

    switch (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
    {
      #ifdef CONFIG_SDK_RTL8218B
        case RT_PHYDRV_RTL8218B:
      #endif    /* CONFIG_SDK_RTL8218B */
      #ifdef CONFIG_SDK_RTL8218FB
        case RT_PHYDRV_RTL8218FB_MP:
      #endif  /* CONFIG_SDK_RTL8218FB */
      #if defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8218B)
        hal_miim_write(unit,  port, 0x0,  27, 0x801e);
        hal_miim_read(unit,  port,  0x0,  28, &phy_data0);
        /*Double check*/
        hal_miim_read(unit,  port,  0x0,  28, &phy_data1);
        if((0x1 != phy_data0) && (0x1 != phy_data1))
        {
            *status = 0x1; /*Had been reset*/
        }
        else
        {
            *status = 0x0;
        }
        break;
    #endif  /* defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8218B) */
      #ifdef CONFIG_SDK_RTL8218D
        case RT_PHYDRV_RTL8218D:
        {
            uint32          blk = 0, data_tmp;
            uint32          i, j, port_tmp;

            /* initial the data as expected value to prevent the read fail condition triggers reset.
             * the access fail reset condition shall trigger from top of the esd check process.
             */
            phy_data0 = 0x8218;
            phy_data1 = 0x8218;
            phy_data2 = 0;
            /* Set default value for reset checking in waMon */
            hal_miim_read(unit, port, 0, 30, &blk);
            hal_miim_write(unit, port, 0, 30, 8);
            hal_miim_read(unit, port, 0x327, 16, &phy_data0);
            hal_miim_read(unit, port, 0x327, 16, &phy_data1);
            hal_miim_read(unit, port, 0x346, 17, &phy_data2);
            hal_miim_write(unit, port, 0, 30, blk);
            phy_data2 = (phy_data2 >>8) & 0xff;
            if((0 != phy_data2) ||((0x8218 != phy_data0) && (0x8218 != phy_data1)))
            {
                *status = 0x1; /*Had been reset*/
                RT_LOG(LOG_WARNING, (MOD_PHY), "port %u data0=%x data1=%x data2=%x\n", port, phy_data0, phy_data1, phy_data2);
                /* related reg information */
                if (0 != phy_data2)
                {
                    for (i = 0; i < 8; i++)
                    {
                        port_tmp = port + i;
                        hal_miim_read(unit, port_tmp, 0, 30, &blk);
                        hal_miim_write(unit, port_tmp, 0, 30, 0);
                        data_tmp = 0;
                        for (j = 0; j < 2; j++)
                        {
                            ret = hal_miim_read(unit, port_tmp, 0xa40, 0, &data_tmp);
                            RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa40,0=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        }
                        for (j = 0; j < 20; j++)
                        {
                            ret = hal_miim_read(unit, port_tmp, 0xa60, 16, &data_tmp);
                            RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa60,16=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                            ret = hal_miim_read(unit, port_tmp, 0xa70, 20, &data_tmp);
                            RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa70,20=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        }
                        ret = hal_miim_read(unit, port_tmp, 0xb90, 20, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xb90,20=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        hal_miim_write(unit, port_tmp, 0, 30, blk);
                    }
                }
                else
                {
                    port_tmp = port;
                    hal_miim_read(unit, port, 0, 30, &blk);
                    hal_miim_write(unit, port, 0, 30, 8);
                    ret = hal_miim_read(unit, port, 0x361, 16, &data_tmp);
                    RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0x361,16=%x (rv=%x)\n", port, data_tmp, ret);
                    ret = hal_miim_read(unit, port, 0x361, 17, &data_tmp);
                    RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0x361,17=%x (rv=%x)\n", port, data_tmp, ret);
                    hal_miim_write(unit, port, 0, 30, blk);
                    for (i = 0; i < 8; i++)
                    {
                        port_tmp = port + i;
                        hal_miim_read(unit, port_tmp, 0, 30, &blk);
                        hal_miim_write(unit, port_tmp, 0, 30, 0);
                        ret = hal_miim_read(unit, port_tmp, 0xbc1, 18, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port_tmp %u 0xbc1,18=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xbc1, 19, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xbc1,19=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xbc1, 20, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xbc1,20=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xa42, 22, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa42,22=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        ret = hal_miim_write(unit, port_tmp, 0xa43, 27, 0x809a);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa43,27 write 0x809a (rv=%x)\n", port_tmp, ret);
                        ret = hal_miim_read(unit, port_tmp, 0xa43, 28, &data_tmp);
                        RT_LOG(LOG_WARNING, (MOD_PHY), "port %u 0xa43,28=%x (rv=%x)\n", port_tmp, data_tmp, ret);
                        hal_miim_write(unit, port_tmp, 0, 30, blk);
                    }
                }
            }
            else
            {
                *status = 0x0;
            }
        }
            break;
      #endif    /* CONFIG_SDK_RTL8218D */
      #ifdef CONFIG_SDK_RTL8214FC
        case RT_PHYDRV_RTL8214FC_MP:
        {
            uint32          blk;

            /* Set default value for reset checking in waMon */
            hal_miim_read(unit, port, 0, 30, &blk);
            hal_miim_write(unit, port, 0, 30, 8);
            hal_miim_read(unit, port, 0x268, 16, &phy_data0);
            hal_miim_read(unit, port, 0x268, 16, &phy_data1);
            hal_miim_write(unit, port, 0, 30, blk);
            phy_data0 &= 0xF000;
            phy_data1 &= 0xF000;

            if((0xA000 != phy_data0) && (0xA000 != phy_data1))
            {
                *status = 0x1; /*Had been reset*/
            }
            else
            {
                *status = 0x0;
            }
        }
            break;
      #endif  /* CONFIG_SDK_RTL8214FC */
#if defined(CONFIG_SDK_RTL8214C)
    case RT_PHYDRV_RTL8214C:
        hal_miim_write(unit,  port, 0,  29, 0x1322);
        hal_miim_read(unit,  port,  0,  30, &phy_data0);
        phy_data0 = (phy_data0>>4) & 0x1;

        /*Double check*/
        hal_miim_write(unit,  port, 0,  29, 0x1322);
        hal_miim_read(unit,  port,  0,  30, &phy_data1);
        phy_data1 = (phy_data1>>4) & 0x1;

        if((0x1 != phy_data0) && (0x1 != phy_data1))
        {
            *status = 0x1; /*Had been reset*/
        }
        else
        {
            *status = 0x0;
        }
        break;
#endif
    #ifdef CONFIG_SDK_RTL8218E
        /* Check if a fatal err occurrs, output *status: 0x1(err), 0x0(ok) */
        case RT_PHYDRV_RTL8218E:
        {
            uint32  blk = 0;

            /* Set default value for reset checking in waMon */
            hal_miim_read(unit, port, 0, 30, &blk);
            hal_miim_write(unit, port, 0, 30, 0x8);
            hal_miim_read(unit, port, 0x346, 17, &phy_data2);
            hal_miim_write(unit, port, 0, 30, blk);
            phy_data2 = (phy_data2 >> 8) & 0xff;
            if (phy_data2 != 0)
                *status = 0x1; /*Had been reset*/
            else
                *status = 0x0;

            if (0 == *status)
            {
                hal_miim_read(unit, port, 0, 30, &blk);
                hal_miim_write(unit, port, 0, 30, 0x8);
                hal_miim_read(unit, port, PHY_8218E_DUMMY_REG_PAGE, PHY_8218E_DUMMY_REG_REG, &phy_data2);
                hal_miim_write(unit, port, 0, 30, blk);
                if (0 == phy_data2)
                    *status = 0x1;  /*Had been reset*/
                else
                    *status = 0x0;
            }
            break;
        }
    #endif

        default:
            break;
    }

    /* Restore MAC polling status */
    if ((ret = hal_miim_pollingWa_unlock(unit, port, pollingSts)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret);
        return ret;
    }

    return RT_ERR_OK;
} /* end of phy_patch_set */

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
int32
phy_downSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_downSpeedEnable_get(unit, port, pEnable));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_downSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret =  (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_downSpeedEnable_set(unit, port, enable));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_downSpeedStatus_get(unit, port, pDownSpeedStatus);

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_downSpeedStatus_get */

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
int32
phy_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberDownSpeedEnable_get(unit, port, pEnable));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret,ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberDownSpeedEnable_set(unit, port, enable));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      phy_drv_chk
 * Description:
 *      Check the driver is used for which PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      drv_type : Chip version
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_drv_chk(uint32 unit, rtk_port_t port, int32 * drv_type)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    *drv_type = pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index;

    return RT_ERR_OK;
} /* end of phy_drv_chk */

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
int32
phy_fiberNwayForceLinkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_fiberNwayForceLinkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpSwitchMacAddr_get(unit, port, pSwitchMacAddr));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpSwitchMacAddr_set(unit, port, pSwitchMacAddr));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTime_get(unit, port, pTimeStamp));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTime_set(unit, port, timeStamp, exec));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeAdjust_set(unit, port, sign, timeStamp, exec));
    MIIM_SEM_UNLOCK(unit);

    return ret;

}

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
int32
phy_ptpRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTimeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeEnable_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeFreq_get(unit, port, pFreq));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTimeFreq_set(uint32 unit, rtk_port_t port, uint32 freq)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeFreq_set(unit, port, freq));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpEnable_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, type=%d, tpid_idx=%d", unit, port, type, tpid_idx);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpIgrTpid_get(unit, port, type, tpid_idx, pTpid));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, type=%d, tpid_idx=%d", unit, port, type, tpid_idx);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpIgrTpid_set(unit, port, type, tpid_idx, tpid));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpSwitchMacRange_get(uint32 unit, rtk_port_t port, uint32 *pRange)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpSwitchMacRange_get(unit, port, pRange));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpSwitchMacRange_set(uint32 unit, rtk_port_t port, uint32 range)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpSwitchMacRange_set(unit, port, range));
    MIIM_SEM_UNLOCK(unit);

    return ret;

}

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
int32
phy_ptpInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIsPortIntr, uint32 *pIntrSts)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpInterruptStatus_get(unit, port, pIsPortIntr, pIntrSts));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpInterruptEnable_get(unit, port, pEnable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpInterruptEnable_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRxTimestamp_get(uint32 unit, rtk_port_t port, rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRxTimestamp_get(unit, port, identifier, pTimeStamp));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpTxTimestamp_get(uint32 unit, rtk_port_t port, rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret =(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpTxTimestamp_get(unit, port, identifier, pTimeStamp));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    hal_control_t   *pHalCtrl;
    int32            ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpOper_get(unit, port, pOperCfg);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    hal_control_t   *pHalCtrl;
    int32            ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpOper_set(unit, port, pOperCfg);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime)
{
    hal_control_t   *pHalCtrl;
    int32            ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpLatchTime_get(unit, port, pLatchTime);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTimeFreqCfg_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeFreqCfg_get(unit, port, pFreqCfg, pFreqCur);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpRefTimeFreqCfg_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, freq=%u, apply=%u", unit, port, freq, apply);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpRefTimeFreqCfg_set(unit, port, freq, apply);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      phy_ptpTxInterruptStatus_get
 * Description:
 *      Get the TX timestamp FIFO non-empty interrupt status of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *
 * Output:
 *      pIntrSts    - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_ptpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpTxInterruptStatus_get(unit, port, pIntrSts);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpTxTimestampFifo_get(unit, port, pTimeEntry);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptp1PPSOutput_get(unit, port, pPulseWidth, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptp1PPSOutput_set(unit, port, pulseWidth, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpClockOutput_get(unit, port, pClkOutput);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpClockOutput_set(unit, port, pClkOutput);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpOutputSigSel_get(unit, port, pOutSigSel);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, outSigSel=%d", unit, port, outSigSel);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpOutputSigSel_set(unit, port, outSigSel);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpTransEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpTransEnable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpTransEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpTransEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpLinkDelay_get(unit, port, pLinkDelay);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ptpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, linkDelay=.%09u", unit, port, linkDelay);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ptpLinkDelay_set(unit, port, linkDelay);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_reg_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 phy_reg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;



    MIIM_SEM_LOCK(unit);
    if (MIIM_NULL_PHY_DRV(pHalCtrl, port) || (!MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_get)))
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, page, phy_reg, pData);
    }
    else
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_get(unit, port, page, phy_reg, pData));
    }
    MIIM_SEM_UNLOCK(unit);

    return ret;

}

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
int32
phy_reg_set(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    if (MIIM_NULL_PHY_DRV(pHalCtrl, port) || (!MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set)))
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, page, phy_reg, data);
    }
    else
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, page, phy_reg, data));
    }
    MIIM_SEM_UNLOCK(unit);

    return ret;

}

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
int32
phy_reg_park_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    if (MIIM_NULL_PHY_DRV(pHalCtrl, port) || (!MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_park_get)))
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_park_read(unit, port, page, parkPage, phy_reg, pData);
    }
    else
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_park_get(unit, port, page, parkPage, phy_reg, pData));
    }
    MIIM_SEM_UNLOCK(unit);

    return ret;

}

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
int32
phy_reg_park_set(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    if (MIIM_NULL_PHY_DRV(pHalCtrl, port) || (!MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_park_set)))
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_park_write(unit, port, page, parkPage, phy_reg, data);
    }
    else
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_park_set(unit, port, page, parkPage, phy_reg, data));
    }
    MIIM_SEM_UNLOCK(unit);

    return  ret;

}


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
int32
phy_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_NULL_PHY_DRV(pHalCtrl, port) || (!MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_mmd_get)))
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);
    }
    else
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_mmd_get(unit, port, mmdAddr, mmdReg, pData));
    }
    MIIM_SEM_UNLOCK(unit);


    return ret;
}

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
int32
phy_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_NULL_PHY_DRV(pHalCtrl, port) || (!MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_mmd_set)))
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
    }
    else
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_mmd_set(unit, port, mmdAddr, mmdReg, data));
    }
    MIIM_SEM_UNLOCK(unit);


    return ret;

}

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
int32
phy_reg_mmd_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    int32   ret;

    MIIM_SEM_LOCK(unit);
    ret = hal_miim_mmd_portmask_write(unit, portmask, mmdAddr, mmdReg, data);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}
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
int32
phy_reg_extParkPage_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_extParkPage_get(unit, port, mainPage, extPage, parkPage, phy_reg, pData));
    MIIM_SEM_UNLOCK(unit);

    return ret;

}

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
int32
phy_reg_extParkPage_set(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_extParkPage_set(unit, port, mainPage, extPage, parkPage, phy_reg, data));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_reg_extParkPage_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data)
{
    int32   ret;

    MIIM_SEM_LOCK(unit);
    ret = hal_miim_extParkPage_portmask_write(unit, portmask, mainPage, extPage, parkPage, phy_reg, data);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_reg_portmask_set(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data)
{
    int32   ret;

    MIIM_SEM_LOCK(unit);
    ret = hal_miim_portmask_write(unit, portmask, page, phy_reg, data);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_reg_broadcast_set(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret;

    MIIM_SEM_LOCK(unit);
    ret = hal_miim_broadcast_write(unit, page, phy_reg, data);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    int32 ret;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_masterSlave_get(unit, port, pMasterSlaveCfg, pMasterSlaveActual));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}/* end of phy_masterSlave_get */

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
int32
phy_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    int32 ret;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, masterSlave=%d", unit, port, masterSlave);

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_masterSlave_set(unit, port, masterSlave));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}/* end of phy_masterSlave_set */


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
int32
phy_loopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_loopback_get(unit, port, pEnable));

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_loopbackEnable_get */

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
int32
phy_loopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_loopback_set(unit, port, enable));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_loopBackEnable_set */

/* SST-75: <START> */
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
int32
phy_construct_reg_park_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData)
{
    int32           ret;

    MIIM_SEM_LOCK(unit);
    ret = hal_miim_park_read(unit, port, page, parkPage, phy_reg, pData);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_construct_reg_park_set(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 data)
{
    int32           ret;

    MIIM_SEM_LOCK(unit);
    ret = hal_miim_park_write(unit, port, page, parkPage, phy_reg, data);
    MIIM_SEM_UNLOCK(unit);

    return  ret;
}
/* SST-75: <END> */

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
int32
phy_fiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberTxDis_set(unit, port, enable));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberTxDis_set */

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
int32
phy_fiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberTxDisPin_set(unit, port, data));
    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberTxDisPin_set */


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
int32
phy_fiberRx_check(uint32 unit, rtk_port_t port, uint32 * pStatus)
{

    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret =  (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberRx_check(unit, port, pStatus));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberRx_check */

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
int32
phy_fiberRx_reset(uint32 unit, rtk_port_t port)
{

    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret =  (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberRx_reset(unit, port));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberRx_reset */

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
int32
phy_serdesFiberRx_check(uint32 unit, rtk_port_t port, uint32 * pStatus)
{

    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret =  (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_serdesFiberRx_check(unit, port, pStatus));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_serdesFiberRx_check */

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
int32
phy_serdesFiberRx_reset(uint32 unit, rtk_port_t port)
{

    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable polling*/
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret =  (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_serdesFiberRx_reset(unit, port));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_serdesFiberRx_reset */

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
phy_fiberLinkUp_handler1(uint32 unit, rtk_port_t port)
{

    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status and disable MAC polling */
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret =  (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberLinkUp_handler1(unit, port));

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberLinkUp_handler1 */


int32 phy_serdes_rst(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    switch (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
    {
      #ifdef CONFIG_SDK_RTL8218D
      case RT_PHYDRV_RTL8218D:
        phy_8218d_macIntfSerdes_reset(unit, port);
        break;
      #endif

      #ifdef CONFIG_SDK_RTL8214QF
      case RT_PHYDRV_RTL8214QF:
        phy_8214qf_macIntfSerdes_reset(unit, port);
        break;
      #endif

      #ifdef CONFIG_SDK_RTL8224QF
      case RT_PHYDRV_RTL8224QF:
        phy_8214qf_macIntfSerdes_reset(unit, port);
        break;
      #endif

      #ifdef CONFIG_SDK_RTL8295R
      case RT_PHYDRV_RTL8295R:
        phy_8295r_macIntfSerdes_reset(unit, port);
        break;
      #endif

      #ifdef CONFIG_SDK_RTL8214FC
      case RT_PHYDRV_RTL8214FC:
        phy_8214fc_serdes_reset(unit, port);
        break;
      #endif

      #ifdef CONFIG_SDK_RTL8218E
      case RT_PHYDRV_RTL8218E:
        phy_8218e_macIntfSerdes_reset(unit, port);
        break;
      #endif

      default:
        break;
    }

    MIIM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of phy_serdes_rst */


static int32 phy_rst_to_default(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_OK;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218D)
    if((pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218B) ||
            (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB_MP) ||
            (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC_MP) ||
            (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218D))
    {
        if((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x8)) != RT_ERR_OK)
            return ret;

        if((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x262, 16, 0x1)) != RT_ERR_OK)
            return ret;

        if((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x0)) != RT_ERR_OK)
            return ret;
    }
#endif

#if defined(CONFIG_SDK_RTL8214C)
    if(pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214C)
    {
        if((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 29, 0x1322)) != RT_ERR_OK)
            return ret;

        if((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x1)) != RT_ERR_OK)
            return ret;
    }
#endif

#if defined(CONFIG_SDK_RTL8218E)
    /* Reset phy */
    if (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218E)
    {
        if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x8)) != RT_ERR_OK)
            return ret;

        if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x262, 16, 0x1)) != RT_ERR_OK)
            return ret;

        if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x0)) != RT_ERR_OK)
            return ret;
    }
#endif

    return ret;
} /* end of phy_rst_to_default */

static int32
_phy_rst_to_default_perPort(uint32 unit, rtk_port_t port, rtk_portmask_t *rst_portmask)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl = NULL;
#if defined(CONFIG_SDK_RTL8218E)
    uint32          phyPort = 0;
    uint32          data = 0;
#endif

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

#if defined(CONFIG_SDK_RTL8218E)
    if (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218E)
    {
        RTK_PORTMASK_SCAN(*rst_portmask, phyPort)
        {
            /* top reg page 0x262 register 16 bit[11:4] PHY7-PHY0 */
            data |= REG32_FIELD_SET(data, (1 << (phyPort - port)), 4, (0xFF << 4));
        }

        if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x8)) != RT_ERR_OK)
            return ret;

        if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x262, 16, data)) != RT_ERR_OK)
            return ret;

        if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_set(unit, port, 0x0, 30, 0x0)) != RT_ERR_OK)
            return ret;
    }
 #endif

     return ret;
 } /* end of _phy_rst_to_default_perPort */

static int32
_phy_enable(uint32 unit, rtk_port_t port)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl;
    uint32          phyPort = 0;
    int32           i;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    for (i = 0; i < HWP_PHY_BASE_PHY_MAX(unit, port); i++)
    {
        phyPort = port + i;
        if ((ret = pHalCtrl->pPhy_ctrl[phyPort]->pPhydrv->fPhydrv_enable_set(unit, phyPort, ENABLED)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of _phy_enable */

static int32
_phy_enable_perPort(uint32 unit, rtk_port_t port, rtk_portmask_t *rst_portmask)
{
    int32           ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl;
    uint32          phyPort = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RTK_PORTMASK_SCAN(*rst_portmask, phyPort)
    {
        if ((ret = pHalCtrl->pPhy_ctrl[phyPort]->pPhydrv->fPhydrv_enable_set(unit, phyPort, ENABLED)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of _dal_phy_enable_perPort */

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
int32
phy_cableESD_recover(uint32 unit, rtk_port_t port, uint32 *pPhy_rst_flag, uint32 *pPort_rst_flag, rtk_portmask_t *pRst_portmask)
{
    hal_control_t   *pHalCtrl;
    uint32          phy_data;
    uint32          phy_access_flag;
    uint32          phy_rst_flag;
    uint32          port_rst_flag;
    rtk_portmask_t  rst_portmask;
    int32           ret = RT_ERR_OK;
    rtk_portmask_t  poll_restore_portmask;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    *pPhy_rst_flag = 0x0;

    /*Check PHY can be accessed or not yet?*/
    phy_access_flag = 0;
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reg_get(unit, port, 0, 2, &phy_data);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_EVENT, (MOD_HAL|MOD_PORT), "Port:%d phy access failed!\n", port);
    }
    else
    {
        if (phy_data == 0x1c)
            phy_access_flag = 0x1;
    }

    phy_rst_flag = 0x0;
    port_rst_flag = 0x0;
    /* PHY can be accessed */
    if (0x1 == phy_access_flag)
    {
        /* Check whether External PHY has been reseted or not */
        if (_phy_chk_rst_status(unit, port, &phy_rst_flag, &port_rst_flag, &rst_portmask) == RT_ERR_OK)
        {
            *pPhy_rst_flag = phy_rst_flag;
            *pPort_rst_flag = port_rst_flag;
            *pRst_portmask = rst_portmask;

            /* This Phy really has been reseted */
            if ((phy_rst_flag != 0x0) || (port_rst_flag != 0x0))
            {
                RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "[PHY-WATCHDOG] --->Cable ESD--->Port:%d found Reset!\n", port);

                hal_miim_pollingPauseAllPort_set(unit, &poll_restore_portmask);

                /* Reset PHY again */
                if (port_rst_flag)
                    ret = _phy_rst_to_default_perPort(unit, port, &rst_portmask);
                else
                    ret = phy_rst_to_default(unit, port);

                if (ret != RT_ERR_OK)
                {
                    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "Port:%d failed!\n", port);
                    hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);
                    MIIM_SEM_UNLOCK(unit);
                    return ret;
                }

                /* Delay until PHY reset done */
                osal_time_usleep(100 * 1000);   /* delay 100 ms */

                /* Re-patch PHY */
                if (port_rst_flag)
                    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_patch_perPort_set(unit, port, &rst_portmask);
                else
                    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_patch_set(unit, port);

                if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
                {
                    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "Port:%d failed!\n", port);
                    hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);
                    MIIM_SEM_UNLOCK(unit);
                    return ret;
                }

                hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);
            }
        }
    }
#ifdef CONFIG_SDK_RTL8218D
    else
    {
        if (RT_PHYDRV_RTL8218D == pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
        {
            hal_miim_pollingPauseAllPort_set(unit, &poll_restore_portmask);
            ret = ioal_phy_reset(unit, port);
            if (ret != RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "Port:%d failed!\n", port);
                hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);
                MIIM_SEM_UNLOCK(unit);
                return ret;
            }

            /*Delay until PHY reset done*/
            osal_time_usleep(100 * 1000); /* delay 100mS */

            /*Do the PHY Re-Patch*/
            ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_patch_set(unit,  port);
            if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "Port:%d failed!\n", port);
                hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);
                MIIM_SEM_UNLOCK(unit);
                return ret;
            }

            hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);
            *pPhy_rst_flag = 0x2;
        }
    }
#endif

    if ((phy_rst_flag != 0x0) || (port_rst_flag != 0x0))
    {
        /* Enable PHY */
        if (port_rst_flag)
            ret = _phy_enable_perPort(unit, port, &rst_portmask);
        else
            ret = _phy_enable(unit, port);

        if (ret != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "Port:%d failed!\n", port);
            MIIM_SEM_UNLOCK(unit);
            return ret;
        }
    }

    MIIM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

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
phy_serdes_linkdown_check(uint32 unit, rtk_port_t port, uint32 * pStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret, ret_polling;
    rtk_enable_t    pollSts;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* back up MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_lock(unit, port, &pollSts)) != RT_ERR_OK)
    {
        MIIM_SEM_UNLOCK(unit);
        RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u disable MAC polling fail (0x%x)", port, ret_polling);
        return ret_polling;
    }

    ret = _phy_serdes_linkdown_chk(unit, port, pStatus);

    /* Restore MAC polling status*/
    if ((ret_polling = hal_miim_pollingWa_unlock(unit, port, pollSts)) != RT_ERR_OK)
    {
      MIIM_SEM_UNLOCK(unit);
      RT_ERR(ret_polling, (MOD_HAL|MOD_PORT), "port %u enable MAC polling fail (0x%x)", port, ret_polling);
      return ret_polling;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      phy_construct_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      phyType       - phy_type_t
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
phy_construct_enable_set(uint32 phyType, uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    rt_phyctrl_t    *pPhyCtrl;
    rt_phydrv_t     *pPhyDrv = NULL;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (pHalCtrl->pPhy_ctrl[port] != NULL)
    {
        pPhyDrv = pHalCtrl->pPhy_ctrl[port]->pPhydrv;

    }
    else
    {
        if ((pPhyCtrl = phy_identify_driver_find_by_phyType((phy_type_t)phyType)) != NULL)
            pPhyDrv = pPhyCtrl->pPhydrv;
    }

    if (pPhyDrv == NULL)
    {
        return RT_ERR_FAILED;
    }

    MIIM_SEM_LOCK(unit);
    ret = pPhyDrv->fPhydrv_enable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberRxEnable_get(unit, port, pEnable);

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberRxEnable_get */

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
int32
phy_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberRxEnable_set(unit, port, enable);

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_fiberRxEnable_set */


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
int32
phy_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia)
{
    hal_control_t   *pHalCtrl;
    int32           ret;


    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_10gMedia_get(unit, port, pMedia);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u, media=%u", unit, port, media);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_10gMedia_set(unit, port, media);

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_10gMedia_set */


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
int32
phy_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCfg)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_polar_get(unit, port, pPolarCfg);
    MIIM_SEM_UNLOCK(unit);

    return ret;

}


/* Function Name:
 *      phy_polar_set
 * Description:
 *      Configure 10GE port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pPolarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_polar_set(unit, port, pPolarCtrl);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      phy_chipRevId_get
 * Description:
 *      Get chip revision ID
 *      This API does not handle SDK forward comatibility.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_rev_id - actual chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *pChip_rev_id)
{
    hal_control_t   *pHalCtrl;
    int32           ret;
    rtk_enable_t    pollSts;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    hal_miim_pollingWa_lock(unit, port, &pollSts);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_chipRevId_get(unit, port, pChip_rev_id);

    hal_miim_pollingWa_unlock(unit, port, pollSts);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      phy_id_get
 * Description:
 *      Get phy chip id and revision id.
 *      This API will handle forward comatibility.
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
int32
phy_id_get(uint32 unit, rtk_port_t port, phy_type_t *pChip_id, uint16 *pChip_rev_id)
{
    hal_control_t   *pHalCtrl;
    uint16          chipRevId;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;


    *pChip_id = pHalCtrl->pPhy_ctrl[port]->phyType;
    if (gMiim_phyRevIdInit != INIT_COMPLETED)
    {
        uint32      unit, port;
        for (unit = 0; unit < RTK_MAX_NUM_OF_UNIT; unit++)
        {
            for (port = 0; port < RTK_MAX_PORT_PER_UNIT; port++)
            {
                gMiim_phyRevId[unit][port] = MIIM_PHY_CHIP_REV_ID_EMPTY;
            }
        }
        gMiim_phyRevIdInit = INIT_COMPLETED;
    }

    if (gMiim_phyRevId[unit][port] == MIIM_PHY_CHIP_REV_ID_EMPTY)
    {
        if ((ret = phy_chipRevId_get(unit, port, &chipRevId)) != RT_ERR_OK)
        {
            return ret;
        }

        /* forward compatible */
        if (chipRevId > pHalCtrl->pPhy_ctrl[port]->drv_rev_id)
        {
            gMiim_phyRevId[unit][port] = pHalCtrl->pPhy_ctrl[port]->drv_rev_id;
        }
        else
        {
            gMiim_phyRevId[unit][port] = chipRevId;
        }

    }

    *pChip_rev_id = gMiim_phyRevId[unit][port];

    return RT_ERR_OK;
}


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
int32
phy_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u, mode=%u channel=%d flags=%x", unit, port, pTestMode->mode, pTestMode->channel, pTestMode->flags);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ieeeTestMode_set(unit, port, pTestMode);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_portEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum)
{
    hal_control_t   *pHalCtrl;
    int32           ret;
    rtk_portmask_t  poll_restore_portmask;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* pause all port's mac-polling for speeding up the eye-monitor process */
    hal_miim_pollingPauseAllPort_set(unit, &poll_restore_portmask);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_portEyeMonitor_start(unit, port, sdsId, frameNum);

    hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}   /* end of phy_portEyeMonitor_start */

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
int32
phy_eyeMonitorInfo_get(uint32 unit, rtk_port_t port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    hal_control_t   *pHalCtrl;
    int32           ret;
    rtk_portmask_t  poll_restore_portmask;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u, sds=%u", unit, port, sds);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    /* pause all port's mac-polling for speeding up the eye-monitor process */
    hal_miim_pollingPauseAllPort_set(unit, &poll_restore_portmask);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_portEyeMonitorInfo_get(unit, port, sds, frameNum, pInfo);

    hal_miim_pollingResumeAllPort_set(unit, &poll_restore_portmask);

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_eyeMonitorInfo_get */

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
int32
phy_portFlashImage_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_portimageFlash_load(unit, port, size, image);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}   /* end of phy_portEyeMonitor_start */

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
int32
_phy_swMacPollPhyStatus_get(uint32 unit, rtk_port_t port, rtk_port_swMacPollPhyStatus_t *pphyStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (!HWP_PHY_EXIST(unit, port))
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);

#if defined(CONFIG_SDK_RTL8295R)
    if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22))
    {
        ret = phy_8295r_swMacPollPhyStatus_get(unit, port, pphyStatus);
    }
    else
#endif
#if defined(CONFIG_SDK_PHY_CUST1)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_CUST1)
    {
        ret = phy_cust1_swMacPollPhyStatus_get(unit, port, pphyStatus);
    }
    else
#endif
    {
        ret = RT_ERR_PORT_NOT_SUPPORTED;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
rtk_port_speed_t
_phy_resolutionResult2Speed_get(uint32 resl)
{
    uint32              speed0, speed1;
    rtk_port_speed_t    speed;

    /* [10,9,5,4] */
    speed0 = REG32_FIELD_GET(resl, PHY_RESL_REG_SPEED0_OFFSET, PHY_RESL_REG_SPEED0_MASK);
    speed1 = REG32_FIELD_GET(resl, PHY_RESL_REG_SPEED1_OFFSET, PHY_RESL_REG_SPEED1_MASK);
    if ((speed1 == 0x0) && (speed0 == 0x0))
        speed = PORT_SPEED_10M;
    else if ((speed1 == 0x0) && (speed0 == 0x1))
        speed = PORT_SPEED_100M;
    else if ((speed1 == 0x0) && (speed0 == 0x2))
        speed = PORT_SPEED_1000M;
    else if ((speed1 == 0x0) && (speed0 == 0x3))
        speed = PORT_SPEED_500M;
    else if ((speed1 == 0x1) && (speed0 == 0x0))
        speed = PORT_SPEED_10G;
    else if ((speed1 == 0x1) && (speed0 == 0x1))
        speed = PORT_SPEED_2_5G;
    else if ((speed1 == 0x1) && (speed0 == 0x2))
        speed = PORT_SPEED_5G;
    else //unknwon
        speed = PORT_SPEED_10M;

    return speed;

}

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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sds_get(unit, port, sdsCfg);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}   /* end of phy_sds_get */

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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sds_set(unit, port, sdsCfg);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}   /* end of phy_sds_set */


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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_sdsRxCaliStatus_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus)
{
    int32   ret;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsRxCaliStatus_get(unit, port, sdsId, pStatus));

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_fiberUnidirEnable_set(unit, port, enable);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}   /* end of phy_fiberUnidirEnable_set */

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
 *      RT_ERR_PORT_NOT_SUPPORTED      - function is not supported by this port.
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_reset_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_reset_set(unit, port);

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_reset_set */

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
 *      RT_ERR_PORT_NOT_SUPPORTED      - function is not supported by this port.
 * Note:
 *      The Link Status bit (Status Register 1.2) has LL (Latching Low) attribute
 *      for link failure. Please refer IEEE 802.3 for detailed.
 */
int32
phy_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_linkStatus_get(unit, port, pStatus);

    MIIM_SEM_UNLOCK(unit);

    return ret;

} /* end of phy_linkStatus_get */

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
int32
phy_peerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_peerAutoNegoAbility_get(unit, port, pAbility);

    MIIM_SEM_UNLOCK(unit);

    return ret;

} /* end of phy_peerAutoNegoAbility_get */

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
int32
phy_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pSerdesMode)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macIntfSerdesMode_get(unit, port, pSerdesMode);

    MIIM_SEM_UNLOCK(unit);

    return ret;
} /* end of phy_macIntfSerdesMode_get */

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
int32
phy_ledMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ledMode_set(unit, port, pLedMode);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ledCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ledCtrl_get(unit, port, pLedCtrl);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ledCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ledCtrl_set(unit, port, pLedCtrl);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_modelId_get(uint32 unit, rtk_port_t port, phy_type_t *pModel_id)
{
    *pModel_id = HWP_PHY_MODEL_BY_PORT(unit, port);
    return RT_ERR_OK;
}

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
int32
phy_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get(unit, port, pStatus);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsEyeParam_get(unit, port, sdsId, pEyeParam);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsEyeParam_set(unit, port, sdsId, pEyeParam);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_2pt5gLiteEnable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_2pt5gLiteEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_mdiLoopbackEnable_get(unit, port, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_mdiLoopbackEnable_set(unit, port, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_fiberRemoteFault_handle(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_remoteFault_handle(unit, port);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_intr_init(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, phyIntr=%d", unit, port, phyIntr);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_intr_init(unit, port, phyIntr);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, phyIntr=%d", unit, port, phyIntr);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_intrEnable_get(unit, port, phyIntr, pEnable);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, phyIntr=%d, enable=%d", unit, port, phyIntr, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_intrEnable_set(unit, port, phyIntr, enable);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, phyIntr=%d", unit, port, phyIntr);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_intrStatus_get(unit, port, phyIntr, pStatus);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_intrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, phyIntr=%d", unit, port, phyIntr);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_intrMask_get(unit, port, phyIntr, pMask);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_intrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, phyIntr=%d, mask=%d", unit, port, phyIntr, mask);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_intrMask_set(unit, port, phyIntr, mask);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_sdsTestMode_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_testMode_t testMode)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, sdsId=%d, testMode=%d", unit, port, sdsId, testMode);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsTestMode_set(unit, port, sdsId, testMode);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      phy_sdsTestModeCnt_get
 * Description:
 *      Get SerDes test mode test pattern error counter.
 * Input:
 *      unit        - unit id
 *      port        - port id
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
int32
phy_sdsTestModeCnt_get(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 *pCnt)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, sdsId=%d", unit, port, sdsId);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsTestModeCnt_get(unit, port, sdsId, pCnt);

    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_sdsLeq_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t *pManual_en, uint32 *pLeq_val)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, sdsId=%u", unit, port, sdsId);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsLeq_get(unit, port, sdsId, pManual_en, pLeq_val);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_sdsLeq_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t manual_en, uint32 leq_val)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, sdsId=%u enable=%d, leq=%u", unit, port, sdsId, manual_en, leq_val);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsLeq_set(unit, port, sdsId, manual_en, leq_val);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}


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
int32
phy_info_get(uint32 unit, rtk_port_t port, rt_phyInfo_t *pphyInfo)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%u, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);

    if (pHalCtrl->pPhy_ctrl[port]->pPhyInfo != NULL)
    {
        osal_memcpy(pphyInfo, pHalCtrl->pPhy_ctrl[port]->pPhyInfo, sizeof(rt_phyInfo_t));
        ret = RT_ERR_OK;
    }
    else
    {
        ret = RT_ERR_FAILED;
    }

    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d ctrl_type=%d", unit, port, ctrl_type);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ctrl_get(unit, port, ctrl_type, pValue);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, ctrl_type=%d value=%d", unit, port, ctrl_type, value);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ctrl_set(unit, port, ctrl_type, value);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_liteEnable_get(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t *pEnable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_liteEnable_get(unit, port, mode, pEnable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_liteEnable_set(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t enable)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, enable=%d", unit, port, enable);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_liteEnable_set(unit, port, mode, enable);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
int32
phy_dbgCounter_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint64 *pCnt)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_dbgCounter_get(unit, port, type, pCnt);
    MIIM_SEM_UNLOCK(unit);

    return ret;
}



extern rtk_port_phyReconfig_callback_t pPhyReconfig_cb;
extern uint32   waMon_init;
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
int32
phy_waMon_phyReconfig_register(rtk_port_phyReconfig_callback_t phyReconfig_callback)
{

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* check Init status */
    RT_INIT_CHK(waMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == phyReconfig_callback), RT_ERR_NULL_POINTER);

    pPhyReconfig_cb = phyReconfig_callback;

    return RT_ERR_OK;
} /* end of dal_waMon_phyReconfig_register */

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
int32
phy_waMon_phyReconfig_unregister(void)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* check Init status */
    RT_INIT_CHK(waMon_init);

    pPhyReconfig_cb = NULL;

    return RT_ERR_OK;
} /* end of dal_waMon_phyReconfig_unregister */

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
int32
phy_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MIIM_NULL_PHY_DRV(pHalCtrl, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_speedDuplexStatus_get(unit, port, pSpeed, pDuplex));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      miim_phy_macsec_reg_get
 * Description:
 *
 * Input:
 *      unit    - unit id
 *      port    -  port id
 *      type    - macsec reg type
 *      reg     - register address
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
miim_phy_macsec_reg_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_reg_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_reg_get(unit, port, dir, reg, pData));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      miim_phy_macsec_reg_set
 * Description:
 *
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - macsec reg type
 *      reg     - register address
 *      data    - value write to register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
miim_phy_macsec_reg_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t type, uint32 reg, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_reg_set))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_reg_set(unit, port, type, reg, data));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_port_cfg_set(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_port_cfg_set))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_port_cfg_set(unit, port, pPortcfg));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_port_cfg_get(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_port_cfg_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_port_cfg_get(unit, port, pPortcfg));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sc_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    rtk_macsec_sc_t *pSc, uint32 *pSc_id)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_create))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_create(unit, port, dir, pSc, pSc_id));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sc_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_t *pSc)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_get(unit, port, dir, sc_id, pSc));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sc_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_del))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_del(unit, port, dir, sc_id));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sc_status_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_status_t *pSc_status)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_status_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sc_status_get(unit, port, dir, sc_id, pSc_status));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sa_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_create))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_create(unit, port, dir, sc_id, an, pSa));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sa_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_get(unit, port, dir, sc_id, an, pSa));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sa_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_del))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_del(unit, port, dir, sc_id, an));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_sa_activate(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_activate))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_sa_activate(unit, port, dir, sc_id, an));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_rxsa_disable(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_rxsa_disable))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_rxsa_disable(unit, port, rxsc_id, an));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_txsa_disable(uint32 unit, rtk_port_t port, uint32 txsc_id)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_txsa_disable))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_txsa_disable(unit, port, txsc_id));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_stat_clear(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_clear))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_clear(unit, port));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_stat_port_get(uint32 unit, rtk_port_t port, rtk_macsec_stat_t stat,
    uint64 *pCnt)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_port_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_port_get(unit, port, stat, pCnt));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_stat_txsa_get(uint32 unit, rtk_port_t port, uint32 txsc_id,
    rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, uint64 *pCnt)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_txsa_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_txsa_get(unit, port, txsc_id, an, stat, pCnt));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_stat_rxsa_get(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, uint64 *pCnt)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_rxsa_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_stat_rxsa_get(unit, port, rxsc_id, an, stat, pCnt));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

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
int32
miim_phy_macsec_intr_status_get(uint32 unit, rtk_port_t port,
    rtk_macsec_intr_status_t *pIntr_status)
{
    hal_control_t   *pHalCtrl;
    int32           ret = RT_ERR_OK;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    /* direct access to MAC driver if there is no PHY driver */
    if (MIIM_PHY_API_AVAILABLE(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_intr_status_get))
    {
        ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_intr_status_get(unit, port, pIntr_status));
    }
    MIIM_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      phy_sdsReg_get
 * Description:
 *      Get PHY SerDes registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - SerDes page id
 *      sdsReg  - SerDes reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_sdsReg_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsReg_get(unit, port, sdsPage, sdsReg, pData));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}

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
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_sdsReg_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 data)
{
    hal_control_t   *pHalCtrl;
    int32           ret;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    MIIM_SEM_LOCK(unit);
    ret = (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_sdsReg_set(unit, port, sdsPage, sdsReg, data));
    MIIM_SEM_UNLOCK(unit);

    return ret;
}
