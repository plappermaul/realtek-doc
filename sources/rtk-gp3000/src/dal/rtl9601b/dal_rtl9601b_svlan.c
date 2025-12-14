/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
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
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1ad, SVLAN [VLAN Stacking]
 *
 */




/*
 * Include Files
 */
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_svlan.h>
#include <dal/rtl9601b/dal_rtl9601b_vlan.h>

/*
 * Symbol Definition
 */

#define DAL_RTL9601B_SVLAN_SET(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {apollomp_svid_valid[vid>>3] |= (1 << (vid&7));}\
} while (0);\

#define DAL_RTL9601B_SVLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {apollomp_svid_valid[vid >> 3] &= (~(1 << (vid&7)));}\
} while (0);\

#define DAL_RTL9601B_SVLAN_IS_SET(vid) \
    (((vid) <= RTK_VLAN_ID_MAX)?((apollomp_svid_valid[vid >> 3] >> (vid&7)) & 1): 0)


#define DAL_RTL9601B_SVLAN_ENTRY_SET(index) \
do {\
    if ((index) <= RTK_VLAN_ID_MAX) {apollomp_svidx_valid[index>>3] |= (1 << (index&7));}\
} while (0);\

#define DAL_RTL9601B_SVLAN_ENTRY_CLEAR(index) \
do {\
    if ((index) <= RTK_VLAN_ID_MAX) {apollomp_svidx_valid[index>> 3] &= (~(1 << (index&7)));}\
} while (0);\

#define DAL_RTL9601B_SVLAN_ENTRY_IS_SET(index) \
    (((index) <= RTK_VLAN_ID_MAX)?((apollomp_svidx_valid[index >> 3] >> (index&7)) & 1): 0)





/*
 * Data Declaration
 */
static uint32   rtl9601b_svlan_init = {INIT_NOT_COMPLETED};
#if 0
static uint8    rtl9601b_svid_valid[4096/8];
#endif
/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Function Name:
 *      dal_rtl9601b_svlan_init
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
dal_rtl9601b_svlan_init(void)
{
    int32   ret;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    rtl9601b_svlan_init = INIT_COMPLETED;

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_rtl9601b_svlan_servicePort_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9601b_svlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if ((ret = dal_rtl9601b_svlan_deiKeepState_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9601b_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9601b_svlan_untagAction_set(SVLAN_ACTION_PSVID, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9601b_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9601b_svlan_svlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9601b_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_init */

/* Function Name:
 *      dal_rtl9601b_svlan_create
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
dal_rtl9601b_svlan_create(rtk_vlan_t svid)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9601B_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);


    if((ret = dal_rtl9601b_vlan_create(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_create */


/* Function Name:
 *      dal_rtl9601b_svlan_destroy
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
dal_rtl9601b_svlan_destroy(rtk_vlan_t svid)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_RTL9601B_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_rtl9601b_vlan_destroy(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_destroy */

/* Function Name:
 *      dal_rtl9601b_svlan_portSvid_get
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
dal_rtl9601b_svlan_portSvid_get(rtk_port_t port, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  svid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9601B_SVLAN_P_SVIDr, port , REG_ARRAY_INDEX_NONE, RTL9601B_SVIDf, &svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pSvid = svid;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_portSvid_get */


/* Function Name:
 *      dal_rtl9601b_svlan_portSvid_set
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
dal_rtl9601b_svlan_portSvid_set(rtk_port_t port, rtk_vlan_t svid)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(DAL_RTL9601B_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_rtl9601b_vlan_existCheck(svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    /*Set Port-based SVLAN*/
    if ((ret = reg_array_field_write(RTL9601B_SVLAN_P_SVIDr, port , REG_ARRAY_INDEX_NONE, RTL9601B_SVIDf, &svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_portSvid_set */

/* Function Name:
 *      dal_rtl9601b_svlan_servicePort_get
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
dal_rtl9601b_svlan_servicePort_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9601B_SVLAN_UPLINK_PMSKr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_servicePort_get */


/* Function Name:
 *      dal_rtl9601b_svlan_servicePort_set
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
dal_rtl9601b_svlan_servicePort_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(RTL9601B_SVLAN_UPLINK_PMSKr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_servicePort_set */

/* Function Name:
 *      dal_rtl9601b_svlan_memberPort_set
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
dal_rtl9601b_svlan_memberPort_set(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    if((ret = dal_rtl9601b_vlan_port_set(svid, pSvlanPortmask, pSvlanUntagPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9601b_svlan_memberPort_set */


/* Function Name:
 *      dal_rtl9601b_svlan_memberPort_get
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
dal_rtl9601b_svlan_memberPort_get(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    if((ret = dal_rtl9601b_vlan_port_get(svid, pSvlanPortmask, pSvlanUntagPortmask)) != RT_ERR_OK)
    {
        if(RT_ERR_VLAN_ENTRY_NOT_FOUND == ret)
        {
            ret = RT_ERR_SVLAN_ENTRY_NOT_FOUND;
        }

        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_memberPort_get */


/* Function Name:
 *      dal_rtl9601b_svlan_tpidEntry_get
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
dal_rtl9601b_svlan_tpidEntry_get(uint32 svlanIndex, uint32 *pSvlanTagId)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pSvlanTagId), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_SVLAN_CFGr, RTL9601B_VS_TPIDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pSvlanTagId = val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_tpidEntry_get */


/* Function Name:
 *      dal_rtl9601b_svlan_tpidEntry_set
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
dal_rtl9601b_svlan_tpidEntry_set(uint32 svlanIndex, uint32 svlan_tag_id)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ETHERTYPE_MAX < svlan_tag_id), RT_ERR_SVLAN_ETHER_TYPE);

    if ((ret = reg_field_write(RTL9601B_SVLAN_CFGr, RTL9601B_VS_TPIDf, &svlan_tag_id)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_tpidEntry_set */


/* Function Name:
 *      dal_rtl9601b_svlan_priorityRef_set
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
dal_rtl9601b_svlan_priorityRef_set(rtk_svlan_pri_ref_t ref)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    switch(ref)
    {
        case SVLAN_PRISEL_INTERNAL_PRI:
            val = DAL_RTL9601B_SVLAN_PRISEL_INTERNAL_PRI;

            break;
        case SVLAN_PRISEL_1QTAG_PRI:
            val = DAL_RTL9601B_SVLAN_PRISEL_1QTAG_PRI;

            break;
        case SVLAN_PRISEL_PBPRI:
            val = DAL_RTL9601B_SVLAN_PRISEL_PBPRI;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_write(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_SPRISELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_priorityRef_set */

/* Function Name:
 *      dal_rtl9601b_svlan_priorityRef_get
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
dal_rtl9601b_svlan_priorityRef_get(rtk_svlan_pri_ref_t *pRef)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRef), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_SPRISELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(val)
    {
        case DAL_RTL9601B_SVLAN_PRISEL_INTERNAL_PRI:
            *pRef = SVLAN_PRISEL_INTERNAL_PRI;

            break;
        case DAL_RTL9601B_SVLAN_PRISEL_1QTAG_PRI:
            *pRef = SVLAN_PRISEL_1QTAG_PRI;

            break;
        case DAL_RTL9601B_SVLAN_PRISEL_PBPRI:
            *pRef = SVLAN_PRISEL_PBPRI;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_priorityRef_get */


/* Function Name:
 *      dal_rtl9601b_svlan_untagAction_set
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
dal_rtl9601b_svlan_untagAction_set(rtk_svlan_action_t action, rtk_vlan_t svid)
{
    int32   ret;
    uint32  untagAction;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    if(svid) { }

    /* parameter check */
    switch(action)
    {
        case SVLAN_ACTION_DROP:

            untagAction = DAL_RTL9601B_SVLAN_UNTAG_DROP;
            break;
        case SVLAN_ACTION_TRAP:

            untagAction = DAL_RTL9601B_SVLAN_UNTAG_TRAP;
            break;

        case SVLAN_ACTION_PSVID:

            untagAction = DAL_RTL9601B_SVLAN_UNTAG_ASSIGN;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_write(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_UNTAGf, &untagAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_untagAction_set */


/* Function Name:
 *      dal_rtl9601b_svlan_untagAction_get
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
dal_rtl9601b_svlan_untagAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  untagAction;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_UNTAGf, &untagAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(untagAction)
    {
        case DAL_RTL9601B_SVLAN_UNTAG_DROP:

            *pAction = SVLAN_ACTION_DROP;
            break;
        case DAL_RTL9601B_SVLAN_UNTAG_TRAP:

            *pAction = SVLAN_ACTION_TRAP;
            break;
        case DAL_RTL9601B_SVLAN_UNTAG_ASSIGN:

            *pAction = SVLAN_ACTION_PSVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_untagAction_get */


/* Function Name:
 *      dal_rtl9601b_svlan_deiKeepState_get
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
dal_rtl9601b_svlan_deiKeepState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_DEI_KEEPf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_deiKeepState_get */

/* Function Name:
 *      dal_rtl9601b_svlan_deiKeepState_set
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
dal_rtl9601b_svlan_deiKeepState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_field_write(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_DEI_KEEPf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_deiKeepState_set */

/* Function Name:
 *      dal_rtl9601b_svlan_svlanFunctionEnable_get
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
dal_rtl9601b_svlan_svlanFunctionEnable_get(rtk_enable_t *pEnable)
{

    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_FILTERINGf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_svlanFunctionEnable_get */


/* Function Name:
 *      dal_rtl9601b_svlan_svlanFunctionEnable_set
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
dal_rtl9601b_svlan_svlanFunctionEnable_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_field_write(RTL9601B_SVLAN_CTRLr, RTL9601B_VS_FILTERINGf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_svlan_svlanFunctionEnable_set */







