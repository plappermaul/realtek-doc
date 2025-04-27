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
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8218b.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_construct.h>

/*
 * Symbol Definition
 */

/* Function Name:
 *      sub_phy_8218b_patch_set
 * Description:
 *      Patch the PHY:8218B.
 * Input:
 *      unit   - unit id
 *      port   - PHY base port id
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
sub_phy_8218b_patch_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_FAILED;
    int32   i;

    RT_PARAM_CHK(((port % PORT_NUM_IN_8218B) != 0), RT_ERR_PORT_ID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    /*Single image*/
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_FAMILY_ID(unit))
    {
        if((port % 8) == 0)
        {
            /*Re-Patch 8218b*/
            rtl8218b_rtl8380_config(unit, port);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8350)
    if (HWP_8390_50_FAMILY(unit))
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "Do the 8218B MP Re-Patch Operation!\n", port);
        rtl8218b_rtl8390_config(unit, port);
        for (i = 0; i < PORT_NUM_IN_8218B; ++i)
            pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_enable_set(unit, port + i, ENABLED);
    }
#endif

    for (i = 0; i < PORT_NUM_IN_8218B; ++i)
        pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_init(unit, port + i);

    ret = RT_ERR_OK;
    return ret;
} /* end of sub_phy_8218b_patch_set */

/* Function Name:
 *      sub_phy_8214fc_mp_patch_set
 * Description:
 *      Patch the PHY:8214FC MP.
 * Input:
 *      unit   - unit id
 *      port   - PHY base port id
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
sub_phy_8214fc_mp_patch_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_FAILED;
    int32   i;

    RT_PARAM_CHK(((port % PORT_NUM_IN_8214FC) != 0), RT_ERR_PORT_ID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

#if defined(CONFIG_SDK_RTL8380)
    if ((RTL8382M_CHIP_ID == HWP_CHIP_ID(unit)) || (RTL8380M_CHIP_ID == HWP_CHIP_ID(unit)))
    {
        if((port % 8) == 0)
        {
            /* Re-Patch 8214FC*/
            rtl8214fc_rtl8380_config(unit, port);
        }
    }
#endif


#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8350)
    if (HWP_8390_50_FAMILY(unit))
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "Do the 8214FC MP Re-Patch Operation!\n", port);
        rtl8214fc_rtl8390_config(unit, port);
        for (i = 0; i < PORT_NUM_IN_8214FC; ++i)
            pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_enable_set(unit, port + i, ENABLED);
    }
#endif
#if defined(CONFIG_SDK_RTL9300) && defined(CONFIG_SDK_RTL8214FC)
        if (HWP_9300_FAMILY_ID(unit))
        {
            if((port % 8) == 0)
            {
                /* Re-Patch 8214FC*/
                rtl8214fc_rtl9300_config(unit, port);
            }
        }
#endif

    for (i = 0; i < PORT_NUM_IN_8214FC; ++i)
    {
        pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_init(unit, port + i);
    }

    ret = RT_ERR_OK;
    return ret;

} /* end of sub_phy_8214fc_mp_patch_set */
#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      sub_phy_8218fb_mp_patch_set
 * Description:
 *      Patch the PHY:8218FB MP.
 * Input:
 *      unit   - unit id
 *      port   - PHY base port id
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
sub_phy_8218fb_mp_patch_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_FAILED;
    int32   i;

    RT_PARAM_CHK(((port % PORT_NUM_IN_8218FB) != 0), RT_ERR_PORT_ID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8350)
        if (HWP_8390_50_FAMILY(unit))
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "Do the 8218FB MP Re-Patch Operation!\n", port);
            rtl8218fb_rtl8390_config(unit, port);
            for (i = 0; i < PORT_NUM_IN_8218FB; ++i)
                pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_enable_set(unit, port + i, ENABLED);
        }
#endif

#if defined(CONFIG_SDK_RTL8380)
        if (HWP_8380_30_FAMILY(unit))
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "Do the 8218FB MP Re-Patch Operation!\n", port);
            rtl8218fb_rtl8380_config(unit, port);
        }
#endif


    for (i = 0; i < PORT_NUM_IN_8218FB; ++i)
        pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_init(unit, port + i);

    ret = RT_ERR_OK;
    return ret;

} /* end of sub_phy_8218fb_mp_patch_set */
#endif  /* CONFIG_SDK_RTL8218FB */

