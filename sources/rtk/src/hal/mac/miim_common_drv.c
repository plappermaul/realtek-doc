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
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#if defined(CONFIG_SDK_RTL8212B) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB)
#include <hal/phy/phy_rtl8214fb.h>
#endif
#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
#include <hal/phy/phy_rtl8218b.h>
#endif
#include <hal/mac/drv/drv.h>

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
hal_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    int32   ret = RT_ERR_FAILED;
    uint32  access_page;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x", unit, port, page, phy_reg);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_read)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pPhy_ctrl[port])
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, page, phy_reg, pData);
        return ret;
    }

    access_page = page;
    ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, access_page, phy_reg, pData);

#if !defined(__MODEL_USER__)
#if defined(CONFIG_SDK_RTL8212B) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB)
    if ((PHY_MODEL_ID_RTL8214B == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9) &&
        ((access_page == PHY_PAGE_0) || (access_page>=7 && access_page<=15)))
    {
        uint32  auto_1000f_shadow = 0;

        if (((*pData)&0x0200) == 0)
        {
            if ((ret = phy_8214fb_auto_1000f_get(unit, port, &auto_1000f_shadow)) != RT_ERR_OK)
            {
                RT_LOG(LOG_WARNING, MOD_HAL, "phy_8214fb_auto_1000f_get(unit=%d, port=%d) failed", unit, port);
                return ret;
            }
            (*pData) |= (auto_1000f_shadow<<9);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
    if ((PHY_MODEL_ID_RTL8218B_EXT == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9))
    {
        uint32  auto_1000f_shadow = 0;

        if (((*pData)&0x0200) == 0)
        {
            if ((ret = phy_8218b_auto_1000f_get(unit, port, &auto_1000f_shadow)) != RT_ERR_OK)
            {
                RT_LOG(LOG_WARNING, MOD_HAL, "phy_8218b_auto_1000f_get(unit=%d, port=%d) failed", unit, port);
                return ret;
            }
            (*pData) |= (auto_1000f_shadow<<9);
        }
    }
#endif
#endif

    /* E0007313 */
    if (((PHY_MODEL_ID_RTL8218B_EXT == pHalCtrl->pPhy_ctrl[port]->phy_model_id)) && (access_page != HAL_MIIM_FIX_PAGE))
    {
        MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0);
    }
    /* End of E0007313 */
    return ret;
} /* end of hal_miim_read */


/* Function Name:
 *      hal_miim_write
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
hal_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;
    uint32  access_page;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_write)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pPhy_ctrl[port])
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, page, phy_reg, data);
        return ret;
    }

    access_page = page;

#if !defined(__MODEL_USER__)
#if defined(CONFIG_SDK_RTL8212B) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB)
    if ((PHY_MODEL_ID_RTL8214B == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9) &&
        ((access_page == PHY_PAGE_0) || (access_page>=7 && access_page<=15)))
    {
        uint32  auto_1000f_shadow = 0;

        auto_1000f_shadow = (data >> 9) & 0x1;
        if ((ret = phy_8214fb_auto_1000f_set(unit, port, auto_1000f_shadow)) != RT_ERR_OK)
        {
            RT_LOG(LOG_WARNING, MOD_HAL, "phy_8214fb_auto_1000f_set(unit=%d, port=%d) failed", unit, port);
            return ret;
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
    if ((PHY_MODEL_ID_RTL8218B_EXT == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9))
    {
        uint32  auto_1000f_shadow = 0;

        auto_1000f_shadow = (data >> 9) & 0x1;
        if ((ret = phy_8218b_auto_1000f_set(unit, port, auto_1000f_shadow)) != RT_ERR_OK)
        {
            RT_LOG(LOG_WARNING, MOD_HAL, "phy_8218b_auto_1000f_set(unit=%d, port=%d) failed", unit, port);
            return ret;
        }
    }
#endif
#endif

    ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, access_page, phy_reg, data);

    /* E0007313 */
    if (((PHY_MODEL_ID_RTL8218B_EXT == pHalCtrl->pPhy_ctrl[port]->phy_model_id)) && (access_page != HAL_MIIM_FIX_PAGE))
    {
        MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, 0, 31, 0);
    }
    /* End of E0007313 */
    return ret;
} /* end of hal_miim_write */


/* Function Name:
 *      hal_miim_park_read
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
hal_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    int32   ret = RT_ERR_FAILED;
    uint32  access_page;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, parkPage=0x%x, phy_reg=0x%x", unit, port, page, parkPage, phy_reg);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_park_read)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pPhy_ctrl[port])
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_park_read(unit, port, page, parkPage, phy_reg, pData);
        return ret;
    }

    access_page = page;

    ret = MACDRV(pHalCtrl)->fMdrv_miim_park_read(unit, port, access_page, parkPage, phy_reg, pData);

#if !defined(__MODEL_USER__)
#if defined(CONFIG_SDK_RTL8212B) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB)
    if ((PHY_MODEL_ID_RTL8214B == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9) &&
        ((access_page == PHY_PAGE_0) || (access_page>=7 && access_page<=15)))
    {
        uint32  auto_1000f_shadow = 0;

        if (((*pData)&0x0200) == 0)
        {
            if ((ret = phy_8214fb_auto_1000f_get(unit, port, &auto_1000f_shadow)) != RT_ERR_OK)
            {
                RT_LOG(LOG_WARNING, MOD_HAL, "phy_8214fb_auto_1000f_get(unit=%d, port=%d) failed", unit, port);
                return ret;
            }
            (*pData) |= (auto_1000f_shadow<<9);
        }
    }
#endif
#endif

#if !defined(__MODEL_USER__)
#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
    if ((PHY_MODEL_ID_RTL8218B_EXT == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9))
    {
        uint32  auto_1000f_shadow = 0;

        if (((*pData)&0x0200) == 0)
        {
            if ((ret = phy_8218b_auto_1000f_get(unit, port, &auto_1000f_shadow)) != RT_ERR_OK)
            {
                RT_LOG(LOG_WARNING, MOD_HAL, "phy_8218b_auto_1000f_get(unit=%d, port=%d) failed", unit, port);
                return ret;
            }
            (*pData) |= (auto_1000f_shadow<<9);
        }
    }
#endif
#endif

    return ret;
} /* end of hal_miim_read */


/* Function Name:
 *      hal_miim_park_write
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
hal_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;
    uint32  access_page;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, parkPage=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, parkPage, phy_reg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_park_write)
        return RT_ERR_FAILED;

    if (NULL == pHalCtrl->pPhy_ctrl[port])
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_park_write(unit, port, page, parkPage, phy_reg, data);
        return ret;
    }

    access_page = page;

#if !defined(__MODEL_USER__)
#if defined(CONFIG_SDK_RTL8212B) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB)
    if ((PHY_MODEL_ID_RTL8214B == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9) &&
        ((access_page == PHY_PAGE_0) || (access_page>=7 && access_page<=15)))
    {
        uint32  auto_1000f_shadow = 0;

        auto_1000f_shadow = (data >> 9) & 0x1;
        if ((ret = phy_8214fb_auto_1000f_set(unit, port, auto_1000f_shadow)) != RT_ERR_OK)
        {
            RT_LOG(LOG_WARNING, MOD_HAL, "phy_8214fb_auto_1000f_set(unit=%d, port=%d) failed", unit, port);
            return ret;
        }
    }
#endif
#endif

#if !defined(__MODEL_USER__)
#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
    if ((PHY_MODEL_ID_RTL8218B_EXT == pHalCtrl->pPhy_ctrl[port]->phy_model_id) &&
        (PHY_REV_NO_B == pHalCtrl->pPhy_ctrl[port]->phy_rev_id) && (phy_reg == 9))
    {
        uint32  auto_1000f_shadow = 0;

        auto_1000f_shadow = (data >> 9) & 0x1;
        if ((ret = phy_8218b_auto_1000f_set(unit, port, auto_1000f_shadow)) != RT_ERR_OK)
        {
            RT_LOG(LOG_WARNING, MOD_HAL, "phy_8218b_auto_1000f_set(unit=%d, port=%d) failed", unit, port);
            return ret;
        }
    }
#endif
#endif

    ret = MACDRV(pHalCtrl)->fMdrv_miim_park_write(unit, port, access_page, parkPage, phy_reg, data);

    return ret;
} /* end of hal_miim_write */


/* Function Name:
 *      hal_miim_parkBits_read
 * Description:
 *      Get value from the specified bits of the PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      msb     - most significant bit for reading the value
 *      lsb     - least significant bit for reading the value
 * Output:
  *      pVal   - pointer buffer of read [msb:lsb] value of the PHY register
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
hal_miim_parkBits_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      msb,
    uint32      lsb,
    uint32      *pVal)
{
    uint32      data = 0, mask;
    int32       ret;

    if ((ret = hal_miim_park_read(unit, port, page, parkPage, phy_reg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    mask = UINT32_BITS_MASK(msb, lsb);
    *pVal = REG32_FIELD_GET(data, lsb, mask);
    return RT_ERR_OK;
}


/* Function Name:
 *      hal_miim_parkBits_write
 * Description:
 *      Set value to the specified bits of the PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      msb     - most significant bit for writing the value
 *      lsb     - least significant bit for writing the value
 *      val    - value for write to [msb:lsb] of the PHY register
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
hal_miim_parkBits_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      msb,
    uint32      lsb,
    uint32      val)
{
    uint32      data = 0, mask;
    int32       ret;

    if ((ret = hal_miim_park_read(unit, port, page, parkPage, phy_reg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    mask = UINT32_BITS_MASK(msb, lsb);
    data = REG32_FIELD_SET(data, val, lsb, mask);
    ret = hal_miim_park_write(unit, port, page, parkPage, phy_reg, data);
    return ret;
}

/* Function Name:
 *      hal_miim_portmask_write
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
hal_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data)
{
    uint32  access_page;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask="PMSK_FMT", page=0x%x, phy_reg=0x%x, data=0x%x", unit, PMSK_ARG(portmask), page, phy_reg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_portmask_write)
        return RT_ERR_FAILED;

    access_page = page;  /* Normal case: access the page number */

    return (MACDRV(pHalCtrl)->fMdrv_miim_portmask_write(unit, portmask, access_page, phy_reg, data));
} /* end of hal_miim_portmask_write */

/* Function Name:
 *      hal_miim_broadcast_write
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
hal_miim_broadcast_write(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, reg=0x%x \
           data=0x%x", unit, page, phy_reg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_broadcast_write)
        return RT_ERR_FAILED;
    else
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_broadcast_write(unit, page, phy_reg, data);
        return ret;
    }
} /* end of hal_miim_extParkPage_write */

/* Function Name:
 *      hal_miim_extParkPage_read
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
hal_miim_extParkPage_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mainPage=0x%x, extPage=0x%x, parkPage=0x%x, reg=0x%x",
           unit, port, mainPage, extPage, parkPage, phy_reg);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_extParkPage_read)
        return RT_ERR_FAILED;
    else
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_extParkPage_read(unit, port, mainPage, extPage, parkPage, phy_reg, pData);
        return ret;
    }
} /* end of hal_miim_extParkPage_read */


/* Function Name:
 *      hal_miim_extParkPage_write
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
hal_miim_extParkPage_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mainPage=0x%x, extPage=0x%x, parkPage=0x%x, reg=0x%x \
           data=0x%x", unit, port, mainPage, extPage, parkPage, phy_reg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_extParkPage_write)
        return RT_ERR_FAILED;
    else
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_extParkPage_write(unit, port, mainPage, extPage, parkPage, phy_reg, data);
        return ret;
    }
} /* end of hal_miim_extParkPage_write */


/* Function Name:
 *      hal_miim_extParkPage_portmask_write
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
hal_miim_extParkPage_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask="PMSK_FMT", mainPage=0x%x, extPage=0x%x, parkPage=0x%x, reg=0x%x \
           data=0x%x", unit, PMSK_ARG(portmask), mainPage, extPage, parkPage, phy_reg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_extParkPage_portmask_write)
        return RT_ERR_FAILED;

    return (MACDRV(pHalCtrl)->fMdrv_miim_extParkPage_portmask_write(unit, portmask, mainPage, extPage, parkPage, phy_reg, data));
} /* end of hal_miim_extParkPage_portmask_write */

/* Function Name:
 *      hal_miim_mmd_read
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
hal_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x, reg=0x%x",
           unit, port, mmdAddr, mmdReg);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_mmd_read)
        return RT_ERR_FAILED;
    else
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);
        return ret;
    }
} /* end of hal_miim_read */


/* Function Name:
 *      hal_miim_mmd_write
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
hal_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, port, mmdAddr, mmdReg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_mmd_write)
        return RT_ERR_CHIP_NOT_SUPPORTED;
    else
    {
        ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
        return ret;
    }
} /* end of hal_miim_mmd_write */


/* Function Name:
 *      hal_miim_mmd_portmask_write
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
hal_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask="PMSK_FMT", mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, PMSK_ARG(portmask), mmdAddr, mmdReg, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_miim_mmd_portmask_write)
        return RT_ERR_FAILED;

    return (MACDRV(pHalCtrl)->fMdrv_miim_mmd_portmask_write(unit, portmask, mmdAddr, mmdReg, data));
} /* end of hal_miim_mmd_portmask_write */

/* Function Name:
 *      hal_miim_pollingEnable_get
 * Description:
 *      Get the mac polling PHY status of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pEnabled - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (!HWP_PORT_EXIST(unit, port))
        return RT_ERR_FAILED;

    return (MACDRV(pHalCtrl)->fMdrv_miim_pollingEnable_get(unit, port, pEnabled));
} /* end of hal_miim_pollingEnable_get */

/* Function Name:
 *      hal_miim_pollingEnable_set
 * Description:
 *      Set the mac polling PHY status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enabled - mac polling PHY status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enabled)
{
    hal_control_t   *pHalCtrl;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (!HWP_PORT_EXIST(unit, port))
        return RT_ERR_FAILED;

    return (MACDRV(pHalCtrl)->fMdrv_miim_pollingEnable_set(unit, port, enabled));
} /* end of hal_miim_pollingEnable_set */


/* Function Name:
 *      hal_miim_pollingPauseAllPort_set
 * Description:
 *      Set the mac polling PHY status to disable
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pRestore_portmask - backup the polling enabled ports for Resume API to restore the polling state
 * Return:
 *      RT_ERR_OK     - OK
 * Note:
 *      None
 */
int32
hal_miim_pollingPauseAllPort_set(
    uint32          unit,
    rtk_portmask_t  *pRestore_portmask)
{
    uint32          port;
    rtk_enable_t    enable;
    int32           ret;

    RTK_PORTMASK_RESET(*pRestore_portmask);

    HWP_ETHER_PORT_TRAVS(unit, port)
    {
        if ((ret = hal_miim_pollingEnable_get(unit, port, &enable)) != RT_ERR_OK)
        {
            continue;
        }

        if (enable == ENABLED)
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d polling pause", unit, port);
            RTK_PORTMASK_PORT_SET(*pRestore_portmask, port);
            hal_miim_pollingEnable_set(unit, port, DISABLED);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      hal_miim_pollingResumeAllPort_set
 * Description:
 *      Set the mac polling PHY status to enable according to the pRestore_portmask
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pRestore_portmask - Restore the polling state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 * Note:
 *      None
 */
int32
hal_miim_pollingResumeAllPort_set(
    uint32          unit,
    rtk_portmask_t  *pRestore_portmask)
{
    uint32          port;

    HWP_ETHER_PORT_TRAVS(unit, port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(*pRestore_portmask, port))
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d polling resume", unit, port);
            hal_miim_pollingEnable_set(unit, port, ENABLED);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      hal_miim_pollingWa_lock
 * Description:
 *      Get mac polling PHY enable status of the specified port and disable mac polling PHY.
 *      This API is used for workaround PHY page being changed by polling mechanism problem.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pRestore_state - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_miim_pollingWa_lock(uint32          unit,
                        rtk_port_t      port,
                        rtk_enable_t    *pRestore_state)
{
    int32       ret;

    if ((ret = hal_miim_pollingEnable_get(unit, port, pRestore_state)) != RT_ERR_OK)
        return ret;

    /* 9300/9310 using "park" machanism to avoid PHY page messing up problem. so it does not need to disable polling. */
    if (HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
    {
        return RT_ERR_OK;
    }

    return hal_miim_pollingEnable_set(unit, port, DISABLED);
}


/* Function Name:
 *      hal_miim_pollingWa_unlock
 * Description:
 *      Restore mac polling PHY status of the specified port.
 *      This API is used for workaround PHY page being changed by polling mechanism problem.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      restore_state - Restoring mac polling PHY status which is obtained from hal_miim_pollingWa_lock()
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_miim_pollingWa_unlock(uint32          unit,
                          rtk_port_t      port,
                          rtk_enable_t    restore_state)
{
    /* 9300/9310 using "park" machanism to avoid PHY page messing up problem. so it does not need to disable polling. */
    if (HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
    {
        return RT_ERR_OK;
    }

    return hal_miim_pollingEnable_set(unit, port, restore_state);
}



/* Function Name:
 *      hal_miim_portSmiMdxProto_get
 * Description:
 *      Configure MDC/MDIO protocol for the port interface
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      *pProto - protocol as Clause 22 or Clause 45
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
int32
hal_miim_portSmiMdxProto_get(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto)
{
    hal_control_t   *pHalCtrl = NULL;

    //RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%u", unit, port);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MACDRV(pHalCtrl)->fMdrv_miim_portSmiMdxProto_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return (MACDRV(pHalCtrl)->fMdrv_miim_portSmiMdxProto_get(unit, port, pProto));
}

/* Function Name:
 *      hal_miim_portSmiMdxProto_set
 * Description:
 *      Configure MDC/MDIO protocol for the port interface
 * Input:
 *      unit - unit id
 *      port - port id
 *      proto  - protocol as Clause 22 or Clause 45
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
int32
hal_miim_portSmiMdxProto_set(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t proto)
{
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%u proto %u", unit, port, proto);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (MACDRV(pHalCtrl)->fMdrv_miim_portSmiMdxProto_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return (MACDRV(pHalCtrl)->fMdrv_miim_portSmiMdxProto_set(unit, port, proto));
}

