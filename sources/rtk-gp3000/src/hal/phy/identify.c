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
 * Purpose : PHY identify service APIs in the SDK.
 *
 * Feature : PHY identify service APIs
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_autoconf.h>
#include <osal/lib.h>
#include <hal/common/halctrl.h>
#include <hal/common/miim.h>
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/phy/identify.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_8218b.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static int32 _phy_identify_default(rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_int_default(rtk_port_t port, uint32 model_id, uint32 rev_id);


/* supported external PHY chip lists */
static rt_phyctrl_t supported_phys[] =
{
     {_phy_identify_default, PHY_MODEL_ID_RTL8212_EXT, PHY_REV_NO_C, &phy_8218b_drv_int_ge, PHY_AFLAG_NULL},
     {_phy_identify_default, PHY_MODEL_ID_RTL8201_EXT, PHY_REV_NO_E, &phy_8218b_drv_int_ge, PHY_AFLAG_NULL},
     {_phy_identify_default, PHY_MODEL_ID_RTL8201_EXT, PHY_REV_NO_F, &phy_8218b_drv_int_ge, PHY_AFLAG_NULL},
     {_phy_identify_default, PHY_MODEL_ID_RTL8201_EXT, PHY_REV_NO_G, &phy_8218b_drv_int_ge, PHY_AFLAG_NULL},

}; /* end of supported_phys */

/* supported internal PHY chip lists */
static rt_phyctrl_t supported_int_phys[] =
{
    {_phy_identify_int_default, PHY_MODEL_ID_RTL8218B_INT, PHY_REV_NO_A, &phy_8218b_drv_int_ge, PHY_AFLAG_NULL},
};

/*
 * Function Declaration
 */

/* Static Function Body */

/* Function Name:
 *      _phy_identify_default
 * Description:
 *      Identify the port is match input PHY information or not?
 * Input:
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - is match the PHY information
 *      RT_ERR_FAILED - is not match the PHY information
 * Note:
 *      None
 */
static int32
_phy_identify_default(rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;

    if (RT_ERR_OK != phy_identify_phyid_get(port, &real_model_id, &real_rev_id))
    {
        return RT_ERR_FAILED;
    }
  
   
    if (RT_ERR_OK != phy_identify_OUI_check(port))
    {
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
        return RT_ERR_OK;

    return RT_ERR_FAILED;
}/* end of _phy_identify_default */

/* Function Name:
 *      _phy_identify_int_default
 * Description:
 *      Identify the port is match input PHY information or not?
 * Input:
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - is match the PHY information
 *      RT_ERR_FAILED - is not match the PHY information
 * Note:
 *      None
 */
static int32
_phy_identify_int_default(rtk_port_t port, uint32 model_id, uint32 rev_id)
{
#if defined(FORCE_PROBE_APOLLO) || defined(FORCE_PROBE_APOLLO_REV_B)
    if(model_id || rev_id)
    {
    }

    switch(port)
    {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
            return RT_ERR_OK;
            break;
        default:
            break;
    }

    return RT_ERR_FAILED;
#elif defined(FORCE_PROBE_APOLLOMP) || defined(FORCE_PROBE_APOLLOMP_REV_B)
    if(model_id || rev_id)
    {
    }

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            return RT_ERR_OK;
            break;
        default:
            break;
    }

    return RT_ERR_FAILED;
#elif defined(FORCE_PROBE_RTL9601B)
    if(model_id || rev_id)
    {
    }

    switch(port)
    {
        case 0:
            return RT_ERR_OK;
            break;
        default:
            break;
    }

    return RT_ERR_FAILED;
#elif defined(FORCE_PROBE_RTL9602C) || defined(FORCE_PROBE_RTL9602C_REV_B)
    if(model_id || rev_id)
    {
    }

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            return RT_ERR_OK;
            break;
        default:
            break;
    }

    return RT_ERR_FAILED;
#elif defined(FORCE_PROBE_RTL9607C) || defined(FORCE_PROBE_RTL9607C_REV_B)
    if(model_id || rev_id)
    {
    }

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            return RT_ERR_OK;
            break;
        default:
            break;
    }

    return RT_ERR_FAILED;	
#elif defined(FORCE_PROBE_RTL9603CVD)
    if(model_id || rev_id)
    {
    }

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            return RT_ERR_OK;
            break;
        default:
            break;
    }

    return RT_ERR_FAILED;	
#else
    uint32 real_model_id, real_rev_id;

    if (RT_ERR_OK != phy_identify_phyid_get(port, &real_model_id, &real_rev_id))
    {
        return RT_ERR_FAILED;
    }

    if (RT_ERR_OK != phy_identify_OUI_check(port))
    {
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
        return RT_ERR_OK;

    return RT_ERR_FAILED;
#endif
} /* end of _phy_identify_int_default */

/* Public Function Body */

/* Function Name:
 *      phy_identify_OUI_check
 * Description:
 *      Identify the OUI is the realtek OUI or not?
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - Realtek OUI
 *      RT_ERR_FAILED - not Realtek OUI
 * Note:
 *      None
 */
int32
phy_identify_OUI_check(rtk_port_t port)
{
    uint32  data0, data1;
    uint32  page;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get())), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);

    page = pHalCtrl->pDev_info->pCapacityInfo->miim_page_id_min;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(port, page, PHY_IDENTIFIER_1_REG, &data0)) != RT_ERR_OK)
    {
        RT_DBG(LOG_TRACE, MOD_HAL, "fMdrv_miim_read(port %d) PHY_IDENTIFIER_1_REG failed!!", port);
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(port, page, PHY_IDENTIFIER_2_REG, &data1)) != RT_ERR_OK)
    {
        RT_DBG(LOG_TRACE, MOD_HAL, "fMdrv_miim_read(port %d) PHY_IDENTIFIER_2_REG failed!!", port);
        return ret;
    }

    if ((data0 != PHY_IDENT_OUI_03_18) ||
        ((data1 >> OUI_19_24_OFFSET) != PHY_IDENT_OUI_19_24))
    {
        RT_DBG(LOG_TRACE, MOD_HAL, "Compare OUI data0 failed(port %d): data0= 0x%x; PHY_IDENT_OUI_03_18 = 0x%x!!", port, data0, PHY_IDENT_OUI_03_18);
        RT_DBG(LOG_TRACE, MOD_HAL, "Compare OUI data1 failed(port %d): data1.b[24:19]= 0x%x; PHY_IDENT_OUI_19_24 = 0x%x!!", port, (data1 >> OUI_19_24_OFFSET), PHY_IDENT_OUI_19_24);
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of phy_identify_OUI_check */


/* Function Name:
 *      phy_identify_find
 * Description:
 *      Find this kind of PHY control structure from the phy supported list.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_find(rtk_port_t port)
{
    int32  size = 0, i;
    uint32 real_model_id, real_rev_id;

    size = sizeof(supported_phys) / sizeof(rt_phyctrl_t);

    if (RT_ERR_OK != phy_identify_phyid_get(port, &real_model_id, &real_rev_id))
    {
        return NULL;
    }


    for (i = size - 1; i >= 0; i--)
    {
        if ((supported_phys[i].chk_func)(port, supported_phys[i].phy_model_id, supported_phys[i].phy_rev_id) == RT_ERR_OK)
        {
            return (&supported_phys[i]);
        }
    }
    
    return (&supported_phys[0]);

} /* end of phy_identify_find */


/* Function Name:
 *      phy_identify_phyid_get
 * Description:
 *      Get this phy model id and its revision id from chip.
 * Input:
 *      port      - port id
 * Output:
 *      pModel_id - pointer buffer of phy model id
 *      pRev_id   - pointer buffer of phy revision id
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_identify_phyid_get(
    rtk_port_t  port,
    uint32      *pModel_id,
    uint32      *pRev_id)
{
    uint32  data;
    uint32  page;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get())), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);
    page = pHalCtrl->pDev_info->pCapacityInfo->miim_page_id_min;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(port, page, PHY_IDENTIFIER_2_REG, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* model id is bit[9:4]; rev id is bit[3:0] */
    *pModel_id = (data & ModelNumber_MASK) >> ModelNumber_OFFSET;
    *pRev_id = (data & RevisionNumber_MASK) >> RevisionNumber_OFFSET;

    return ret;
} /* end of phy_identify_phyid_get */


/* Function Name:
 *      phy_identify_int_find
 * Description:
 *      Find this kind of PHY control structure from the internal phy supported list.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_int_find(rtk_port_t port)
{
    int32  size = 0, i;

    size = sizeof(supported_int_phys) / sizeof(rt_phyctrl_t);

    for (i = size - 1; i >= 0; i--)
    {
        if ((supported_int_phys[i].chk_func)(port, supported_int_phys[i].phy_model_id, supported_int_phys[i].phy_rev_id) == RT_ERR_OK)
        {
            return (&supported_int_phys[i]);
        }
    }

    /* Return a default PHY control */
    return &supported_int_phys[0];
} /* end of phy_identify_int_find */
