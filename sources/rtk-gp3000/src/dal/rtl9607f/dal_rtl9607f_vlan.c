/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 46639 $
 * $Date: 2014-02-26 16:48:44 +0800 (週三, 26 二月 2014) $
 *
 * Purpose : Definition of VLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Vlan table configure and modification
 *           (2) Accept frame type
 *           (3) Vlan ingress/egress filter
 *           (4) Port based and protocol based vlan
 *           (5) TPID configuration
 *           (6) Ingress tag handling
 *           (7) Tag format handling
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_vlan.h>
#include <osal/time.h>
#include <scfg.h>

#include <cortina-api/include/vlan.h>
#include <cortina-api/include/port.h>

#include <aal_l2_vlan.h>

/*
 * Symbol Definition
 */

static uint32   vlan_init = INIT_NOT_COMPLETED;

static rtk_enable_t VLAN_FUNC_ENABLE = ENABLED;

static uint32   vlan_valid[(RTK_VLAN_ID_MAX + 1) / 32];

#define DAL_RTL9607F_VLAN_SET(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {vlan_valid[vid >> 5] |= (1 << (vid&31));}\
} while (0);\

#define DAL_RTL9607F_VLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {vlan_valid[vid >> 5] &= (~(1 << (vid&31)));}\
} while (0);\

#define DAL_RTL9607F_VLAN_IS_SET(vid) \
    (((vid) <= RTK_VLAN_ID_MAX)?((vlan_valid[vid >> 5] >> (vid&31)) & 1): 0)


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9607f_vlan_init
 * Description:
 *      Initialize vlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
int32
dal_rtl9607f_vlan_init(void)
{
    int32 ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    vlan_init = INIT_COMPLETED;

    if((ret = dal_rtl9607f_vlan_vlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Create Default VLAN */
    if((ret = dal_rtl9607f_vlan_destroyAll(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_init */

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      dal_rtl9607f_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Note:
 */
int32
dal_rtl9607f_vlan_create(rtk_vlan_t vid)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_create */

/* Function Name:
 *      dal_rtl9607f_vlan_destroy
 * Description:
 *      Destroy the vlan.
 * Input:
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
dal_rtl9607f_vlan_destroy(rtk_vlan_t vid)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    
    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_destroy */


/* Function Name:
 *      dal_rtl9607f_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan.
 * Input:
 *      restore_default_vlan - keep and restore default vlan id or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
dal_rtl9607f_vlan_destroyAll(uint32 restoreDefaultVlan)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= restoreDefaultVlan), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_destroyAll */

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      dal_rtl9607f_vlan_portPvid_get
 * Description:
 *      Get port default vlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607f_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_portPvid_get */


/* Function Name:
 *      dal_rtl9607f_vlan_portPvid_set
 * Description:
 *      Set port default vlan id.
 * Input:
 *      port - port id
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_rtl9607f_vlan_portPvid_set(rtk_port_t port, uint32 pvid)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_portPvid_set */

/* Function Name:
 *      dal_rtl9607f_vlan_port_get
 * Description:
 *      Get the vlan members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607f_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMemberPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntagPortmask), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_port_get */


/* Function Name:
 *      dal_rtl9607f_vlan_port_set
 * Description:
 *      Replace the vlan members.
 * Input:
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 */
int32
dal_rtl9607f_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMember_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntag_portmask), RT_ERR_NULL_POINTER);
    
    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_port_set */

/* Function Name:
 *      dal_rtl9607f_vlan_vlanFunctionEnable_get
 * Description:
 *      Get the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9607f_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_vlanFunctionEnable_get */


/* Function Name:
 *      dal_rtl9607f_vlan_vlanFunctionEnable_set
 * Description:
 *      Set the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9607f_vlan_vlanFunctionEnable_set(rtk_enable_t enable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    VLAN_FUNC_ENABLE = enable;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_vlanFunctionEnable_set */

/* Function Name:
 *      dal_rtl9607f_vlan_fidMode_get
 * Description:
 *      Get the filter id mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pMode - pointer buffer of filter id mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_rtl9607f_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_fidMode_get */

/* Function Name:
 *      dal_rtl9607f_vlan_fidMode_set
 * Description:
 *      Set the filter id mode of the vlan.
 * Input:
 *      vid   - vlan id
 *      mode  - filter id mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_rtl9607f_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_FID_MODE_END <= mode), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_vlan_fidMode_set */
