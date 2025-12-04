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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (週五, 03 五月 2013) $
 *
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1ad, SVLAN [VLAN Stacking]
 *
 */




/*
 * Include Files
 */
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_svlan.h>
#include <dal/rtl9607c/dal_rtl9607c_vlan.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32   rtl9607c_svlan_init = {INIT_NOT_COMPLETED};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : SVLAN */

/* Function Name:
 *      dal_rtl9607c_svlan_init
 * Description:
 *      Initialize svlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize svlan module before calling any svlan APIs.
 */
int32
dal_rtl9607c_svlan_init(void)
{
    int32   ret;
    uint32  index;
    uint32  val;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    rtl9607c_svlan_init = INIT_COMPLETED;

    val = 0;
    for(index = 0; index < RTL9607C_SVLAN_SP2C_NO; index++)
    {
        if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, index, RTL9607C_VALIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_rtl9607c_svlan_servicePort_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9607c_svlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if ((ret = dal_rtl9607c_svlan_deiKeepState_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9607c_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9607c_svlan_untagAction_set(SVLAN_ACTION_DROP, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9607c_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9607c_svlan_tpidEnable_set(HAL_MAX_NUM_OF_SVLAN_TPID()-1, DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9607c_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9607c_svlan_svlanFunctionEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9607c_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_init */

/* Function Name:
 *      dal_rtl9607c_svlan_create
 * Description:
 *      Create the svlan.
 * Input:
 *      svid - svlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_EXIST          - SVLAN entry is exist
 *      RT_ERR_SVLAN_TABLE_FULL
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_create(rtk_vlan_t svid)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_rtl9607c_vlan_create(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_create */


/* Function Name:
 *      dal_rtl9607c_svlan_destroy
 * Description:
 *      Destroy the svlan.
 * Input:
 *      svid - svlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_destroy(rtk_vlan_t svid)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_rtl9607c_vlan_destroy(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_destroy */

/* Function Name:
 *      dal_rtl9607c_svlan_portSvid_get
 * Description:
 *      Get port default svlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pSvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_SVLAN_INVALID
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_portSvid_get(rtk_port_t port, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  svid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_SVLAN_P_SVIDr, port , REG_ARRAY_INDEX_NONE, RTL9607C_SVIDf, &svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pSvid = svid;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_portSvid_get */


/* Function Name:
 *      dal_rtl9607c_svlan_portSvid_set
 * Description:
 *      Set port default svlan id.
 * Input:
 *      port - port id
 *      svid - port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_portSvid_set(rtk_port_t port, rtk_vlan_t svid)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_rtl9607c_vlan_existCheck(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    /*Set Port-based SVLAN*/
    if ((ret = reg_array_field_write(RTL9607C_SVLAN_P_SVIDr, port , REG_ARRAY_INDEX_NONE, RTL9607C_SVIDf, &svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_portSvid_set */

/* Function Name:
 *      dal_rtl9607c_svlan_servicePort_get
 * Description:
 *      Get service ports from the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - status of service port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_servicePort_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_SVLAN_UPLINK_PMSKr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_servicePort_get */


/* Function Name:
 *      dal_rtl9607c_svlan_servicePort_set
 * Description:
 *      Set service ports to the specified device.
 * Input:
 *      port       - port id
 *      enable     - status of service port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_servicePort_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(RTL9607C_SVLAN_UPLINK_PMSKr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_servicePort_set */

/* Function Name:
 *      dal_rtl9607c_svlan_memberPort_set
 * Description:
 *      Replace the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
int32
dal_rtl9607c_svlan_memberPort_set(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    if((ret = dal_rtl9607c_vlan_existCheck(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if((ret = dal_rtl9607c_vlan_port_set(svid, pSvlanPortmask, pSvlanUntagPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_memberPort_set */



/* Function Name:
 *      dal_rtl9607c_svlan_memberPort_get
 * Description:
 *      Get the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
int32
dal_rtl9607c_svlan_memberPort_get(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    if((ret = dal_rtl9607c_vlan_existCheck(svid)) != RT_ERR_OK)
    {
        //RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if((ret = dal_rtl9607c_vlan_port_get(svid, pSvlanPortmask, pSvlanUntagPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_memberPort_get */


/* Function Name:
 *      dal_rtl9607c_svlan_tpidEntry_get
 * Description:
 *      Get the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 * Output:
 *      pSvlanTagId - pointer buffer of svlan TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9607c_svlan_tpidEntry_get(uint32 svlanIndex, uint32 *pSvlanTagId)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pSvlanTagId), RT_ERR_NULL_POINTER);

    if(svlanIndex == 0)
    {
        if ((ret = reg_field_read(RTL9607C_SVLAN_CFGr, RTL9607C_VS_TPIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_read(RTL9607C_SVLAN_CFGr, RTL9607C_VS_TPID2f, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    *pSvlanTagId = val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_tpidEntry_get */


/* Function Name:
 *      dal_rtl9607c_svlan_tpidEntry_set
 * Description:
 *      Set the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 *      svlan_tag_id - svlan TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9607c_svlan_tpidEntry_set(uint32 svlanIndex, uint32 svlan_tag_id)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ETHERTYPE_MAX < svlan_tag_id), RT_ERR_SVLAN_ETHER_TYPE);

    if(svlanIndex == 0)
    {
        if ((ret = reg_field_write(RTL9607C_SVLAN_CFGr, RTL9607C_VS_TPIDf, &svlan_tag_id)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_write(RTL9607C_SVLAN_CFGr, RTL9607C_VS_TPID2f, &svlan_tag_id)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_tpidEntry_set */


/* Function Name:
 *      dal_rtl9607c_svlan_priorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 * Input:
 *      ref - reference selection parameter.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
int32
dal_rtl9607c_svlan_priorityRef_set(rtk_svlan_pri_ref_t ref)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    switch(ref)
    {
        case SVLAN_PRISEL_INTERNAL_PRI:
            val = DAL_RTL9607C_SVLAN_PRISEL_INTERNAL_PRI;

            break;
        case SVLAN_PRISEL_1QTAG_PRI:
            val = DAL_RTL9607C_SVLAN_PRISEL_1QTAG_PRI;

            break;

        case SVLAN_PRISEL_PBPRI:
            val = DAL_RTL9607C_SVLAN_PRISEL_PBPRI;

            break;
        case SVLAN_PRISEL_OUTERTAG:
            val = DAL_RTL9607C_SVLAN_PRISEL_OUTER_TAG;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_write(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_SPRISELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_priorityRef_set */

/* Function Name:
 *      dal_rtl9607c_svlan_priorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 * Input:
 *      None
 * Output:
 *      pRef - reference selection parameter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
int32
dal_rtl9607c_svlan_priorityRef_get(rtk_svlan_pri_ref_t *pRef)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRef), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_SPRISELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(val)
    {
        case DAL_RTL9607C_SVLAN_PRISEL_INTERNAL_PRI:
            *pRef = SVLAN_PRISEL_INTERNAL_PRI;

            break;
        case DAL_RTL9607C_SVLAN_PRISEL_1QTAG_PRI:
            *pRef = SVLAN_PRISEL_1QTAG_PRI;

            break;
        case DAL_RTL9607C_SVLAN_PRISEL_PBPRI:
            *pRef = SVLAN_PRISEL_PBPRI;

            break;
        case DAL_RTL9607C_SVLAN_PRISEL_OUTER_TAG:
            *pRef = SVLAN_PRISEL_OUTERTAG;
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_priorityRef_get */

/* Function Name:
 *      dal_rtl9607c_svlan_sp2c_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      cvid        - VLAN ID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
int32
dal_rtl9607c_svlan_sp2c_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t cvid)
{
    int32   ret;
    uint32  val;
    uint32  svidCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  valid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < cvid), RT_ERR_VLAN_VID);

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &svidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidCheck == svid &&  port == dstPort)
            {
                val = (uint32)cvid;
                if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_EGRVIDf, &val)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == DISABLED)
        {
            val = (uint32)svid;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            val = (uint32)cvid;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_EGRVIDf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            val = (uint32)dstPort;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            valid = ENABLED;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_SVLAN_TABLE_FULL;
} /* end of dal_rtl9607c_svlan_sp2c_add */


/* Function Name:
 *      dal_rtl9607c_svlan_sp2c_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pCvid - VLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
int32
dal_rtl9607c_svlan_sp2c_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t *pCvid)
{
    int32   ret;
    uint32  val;
    uint32  svidCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  valid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCvid), RT_ERR_NULL_POINTER);

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &svidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidCheck == svid &&  port == dstPort)
            {
                if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_EGRVIDf, &val)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                *pCvid = (rtk_vlan_t)val;

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_rtl9607c_svlan_sp2c_get */

/* Function Name:
 *      dal_rtl9607c_svlan_sp2c_del
 * Description:
 *      Delete system SP2C configuration
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can delete SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
int32
dal_rtl9607c_svlan_sp2c_del(rtk_vlan_t svid, rtk_port_t dstPort)
{
    int32   ret;
    uint32  svidCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  valid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &svidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidCheck == svid &&  port == dstPort)
            {
                valid = DISABLED;
                if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_rtl9607c_svlan_sp2c_del */

/* Function Name:
 *      dal_rtl9607c_svlan_sp2cPriority_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      priority    - Priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned Priority if the output port is the assigned destination port.
 */
int32
dal_rtl9607c_svlan_sp2cPriority_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_pri_t priority)
{
    int32   ret;
    uint32  val;
    uint32  svidCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  valid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_1P_PRIORITY);

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &svidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidCheck == svid &&  port == dstPort)
            {
                val = (uint32)priority;
                if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_PRIORITYf, &val)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == DISABLED)
        {
            val = (uint32)svid;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            val = (uint32)priority;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_PRIORITYf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            val = (uint32)dstPort;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            valid = ENABLED;
            if ((ret = reg_array_field_write(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_SVLAN_TABLE_FULL;
} /* end of dal_rtl9607c_svlan_sp2cPriority_add */


/* Function Name:
 *      dal_rtl9607c_svlan_sp2cPriority_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pPriority   - Priority
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration.
 */
int32
dal_rtl9607c_svlan_sp2cPriority_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  val;
    uint32  svidCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  valid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    for(sp2cIndex = 0; sp2cIndex < RTL9607C_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_IGRVIDf, &svidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidCheck == svid &&  port == dstPort)
            {
                if ((ret = reg_array_field_read(RTL9607C_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, RTL9607C_PRIORITYf, &val)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                *pPriority = (rtk_vlan_t)val;

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_rtl9607c_svlan_sp2cPriority_get */


/* Function Name:
 *      dal_rtl9607c_svlan_untagAction_set
 * Description:
 *      Configure Action of downstream UnStag packet
 * Input:
 *      action  - Action for UnStag
 *      svid    - The SVID assigned to UnStag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also supported by this API. The parameter of svid is
 *      only referenced when the action is set to UNTAG_ASSIGN
 */
int32
dal_rtl9607c_svlan_untagAction_set(rtk_svlan_action_t action, rtk_vlan_t svid)
{
    int32   ret;
    uint32  untagAction;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    if (svid) { }

    /* parameter check */
    switch(action)
    {
        case SVLAN_ACTION_DROP:

            untagAction = DAL_RTL9607C_SVLAN_UNTAG_DROP;
            break;
        case SVLAN_ACTION_TRAP:

            untagAction = DAL_RTL9607C_SVLAN_UNTAG_TRAP;
            break;

        case SVLAN_ACTION_PSVID:

            untagAction = DAL_RTL9607C_SVLAN_UNTAG_PBVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    if ((ret = reg_field_write(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_UNTAGf, &untagAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_untagAction_set */


/* Function Name:
 *      dal_rtl9607c_svlan_untagAction_get
 * Description:
 *      Get Action of downstream UnStag packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for UnStag
 *      pSvid    - The SVID assigned to UnStag packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNTAG_ASSIGN
 */
int32
dal_rtl9607c_svlan_untagAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  untagAction;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_UNTAGf, &untagAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(untagAction)
    {
        case DAL_RTL9607C_SVLAN_UNTAG_DROP:

            *pAction = SVLAN_ACTION_DROP;
            break;
        case DAL_RTL9607C_SVLAN_UNTAG_TRAP:

            *pAction = SVLAN_ACTION_TRAP;
            break;
        case DAL_RTL9607C_SVLAN_UNTAG_PBVID:

            *pAction = SVLAN_ACTION_PSVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_untagAction_get */

/* Function Name:
 *      dal_rtl9607c_svlan_trapPri_get
 * Description:
 *      Get svlan trap priority
 * Input:
 *      None
 * Output:
 *      pPriority - priority for trap packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_trapPri_get(rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_PRIf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pPriority = (rtk_pri_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_trapPri_get */


/* Function Name:
 *      dal_rtl9607c_svlan_trapPri_set
 * Description:
 *      Set svlan trap priority
 * Input:
 *      priority - priority for trap packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_trapPri_set(rtk_pri_t priority)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_INT_PRIORITY);

    val = (uint32)priority;

    if ((ret = reg_field_write(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_PRIf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_trapPri_set */

/* Function Name:
 *      dal_rtl9607c_svlan_deiKeepState_get
 * Description:
 *      Get svlan dei keep state
 * Input:
 *      None
 * Output:
 *      pEnable - state of keep dei
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_deiKeepState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_DEI_KEEPf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_deiKeepState_get */

/* Function Name:
 *      dal_rtl9607c_svlan_deiKeepState_set
 * Description:
 *      Set svlan dei keep state
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9607c_svlan_deiKeepState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_field_write(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_DEI_KEEPf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_deiKeepState_set */

/* Function Name:
 *      dal_rtl9607c_svlan_svlanFunctionEnable_get
 * Description:
 *      Get the SVLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of svlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9607c_svlan_svlanFunctionEnable_get(rtk_enable_t *pEnable)
{

    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_FILTERINGf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_svlanFunctionEnable_get */


/* Function Name:
 *      dal_rtl9607c_svlan_svlanFunctionEnable_set
 * Description:
 *      Set the SVLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of svlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9607c_svlan_svlanFunctionEnable_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_field_write(RTL9607C_SVLAN_CTRLr, RTL9607C_VS_FILTERINGf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_svlanFunctionEnable_set */

/* Function Name:
 *      dal_rtl9607c_svlan_tpidEnable_get
 * Description:
 *      Get the svlan TPID enable status.
 * Input:
 *      svlanIndex  - index of tpid entry
 * Output:
 *      pEnable - pointer to svlan tpid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9607c_svlan_tpidEnable_get(uint32 svlanIndex, rtk_enable_t *pEnable)
{

    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(svlanIndex == 0)
    {
        *pEnable = ENABLED;
    }
    else
    {
        if ((ret = reg_field_read(RTL9607C_SVLAN_CFGr, RTL9607C_VS_TPID2_ENf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        *pEnable = (rtk_enable_t)val;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_tpidEnable_get */


/* Function Name:
 *      dal_rtl9607c_svlan_tpidEnable_set
 * Description:
 *      Set the svlan TPID enable status.
 * Input:
 *      svlanIndex  - index of tpid entry
 *      enable - svlan tpid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 */
int32
dal_rtl9607c_svlan_tpidEnable_set(uint32 svlanIndex, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(svlanIndex == 0)
    {
        if(enable == DISABLED)
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    else
    {
        val = (uint32)enable;
        if ((ret = reg_field_write(RTL9607C_SVLAN_CFGr, RTL9607C_VS_TPID2_ENf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_tpidEnable_set */

/* Function Name:
 *      dal_rtl9607c_svlan_extPortPvid_get
 * Description:
 *      Get extension port default svlan id.
 * Input:
 *      extPort - Extension port id
 * Output:
 *      pPvid - pointer buffer of port default svlan id
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
dal_rtl9607c_svlan_extPortPvid_get(uint32 extPort, uint32 *pPvid)
{
    int32   ret;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_SVLAN_EXT_SVIDr, extPort, REG_ARRAY_INDEX_NONE, RTL9607C_SVIDf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pPvid = tmpVal;
    return RT_ERR_OK;
} /* end of dal_rtl9607c_vlan_extPortPvid_get */


/* Function Name:
 *      dal_rtl9607c_svlan_extPortPvid_set
 * Description:
 *      Set extension port default svlan id.
 * Input:
 *      extPort - extension port id
 *      pvid    - extension port default svlan id
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
dal_rtl9607c_svlan_extPortPvid_set(uint32 extPort, uint32 pvid)
{
    int32   ret;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_VLAN_ID_MAX <= pvid), RT_ERR_INPUT);
    RT_PARAM_CHK(DAL_RTL9607C_SVLAN_VID_NOT_ACCEPT(pvid), RT_ERR_VLAN_VID);

    if((ret = dal_rtl9607c_vlan_existCheck(pvid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    tmpVal = pvid;

    if ((ret = reg_array_field_write(RTL9607C_SVLAN_EXT_SVIDr, extPort, REG_ARRAY_INDEX_NONE, RTL9607C_SVIDf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_svlan_extPortPvid_set */



