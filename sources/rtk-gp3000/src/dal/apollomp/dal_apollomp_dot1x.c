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
 * Purpose : Definition of 802.1x API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Unauth packet handling
 *           (2) 802.1X port-based NAC
 *           (3) 802.1X MAC-based NAC
 *           (4) 802.1X parameter
 *           (5) Parameter for trapped packets
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/dot1x.h>
#include <dal/apollomp/dal_apollomp_dot1x.h>
#include <dal/apollomp/dal_apollomp_vlan.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32    apollomp_dot1x_init = {INIT_NOT_COMPLETED};

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_dot1x_init
 * Description:
 *      Initial the dot1x module of the specified device..
 * Input:
 *      none
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize dot1x module before calling any dot1x APIs.
 */
int32
dal_apollomp_dot1x_init(void)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);


    apollomp_dot1x_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_init */

/* Function Name:
 *      dal_apollomp_dot1x_unauthPacketOper_get
 * Description:
 *      Get the configuration of unauthorized behavior for both 802.1x port and mac based network access control on specified port.
 * Input:
 *      port           - port id
 * Output:
 *      pUnauth_action - The action of how to handle unauthorized packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      Forwarding action for unauth packet is as following
 *      - DOT1X_ACTION_DROP
 *      - DOT1X_ACTION_TRAP2CPU
 *      - DOT1X_ACTION_TO_GUEST_VLAN
 */
int32
dal_apollomp_dot1x_unauthPacketOper_get(rtk_port_t port, rtk_action_t *pUnauthAction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pUnauthAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_UNAUTH_ACTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    switch(val)
    {
        case DAL_APOLLOMP_DOT1X_UNAUTH_DROP:

            *pUnauthAction = ACTION_DROP;
            break;
        case DAL_APOLLOMP_DOT1X_UNAUTH_TRAP:

            *pUnauthAction = ACTION_TRAP2CPU;
            break;
        case DAL_APOLLOMP_DOT1X_UNAUTH_GVLAN:

            *pUnauthAction = ACTION_TO_GUESTVLAN;
            break;
        case ACTION_TO_GUESTVLAN:

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_unauthPacketOper_get */

/* Function Name:
 *      dal_apollomp_dot1x_unauthPacketOper_set
 * Description:
 *      Set the configuration of unauthorized behavior for both 802.1x port and mac based network access control on specified port.
 * Input:
 *      port          - port id
 *      unauth_action - The action of how to handle unauthorized packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      Forwarding action for unauth packet is as following
 *      - DOT1X_ACTION_DROP
 *      - DOT1X_ACTION_TRAP2CPU
 *      - DOT1X_ACTION_TO_GUEST_VLAN
 */
int32
dal_apollomp_dot1x_unauthPacketOper_set(rtk_port_t port, rtk_action_t unauthAction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    switch(unauthAction)
    {
        case ACTION_DROP:

            val = DAL_APOLLOMP_DOT1X_UNAUTH_DROP;
            break;
        case ACTION_TRAP2CPU:

            val = DAL_APOLLOMP_DOT1X_UNAUTH_TRAP;
            break;
        case ACTION_TO_GUESTVLAN:

            val = DAL_APOLLOMP_DOT1X_UNAUTH_GVLAN;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    if ((ret = reg_array_field_write(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_UNAUTH_ACTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_unauthPacketOper_set */

/* Function Name:
 *      dal_apollomp_dot1x_portBasedEnable_get
 * Description:
 *      Get the status of 802.1x port-based network access control on a specific port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - The status of 802.1x port-based network access control.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) If a port is 802.1x port based network access control "enabled",
 *          it should be authenticated so packets from that port wont be dropped or trapped to CPU.
 *
 *      (2) The status of 802.1x port-based network access control is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_dot1x_portBasedEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PB_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_portBasedEnable_get */

/* Function Name:
 *      dal_apollomp_dot1x_portBasedEnable_set
 * Description:
 *      Set the status of 802.1x port-based network access control on a specific port
 * Input:
 *      port   - port id
 *      enable - The status of 802.1x port-based network access control.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 * Note:
 *      (1) If a port is 802.1x port based network access control "enabled",
 *          it should be authenticated so packets from that port wont be dropped or trapped to CPU.
 *
 *      (2) The status of 802.1x port-based network access control is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_dot1x_portBasedEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PB_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_portBasedEnable_set */

/* Function Name:
 *      dal_apollomp_dot1x_portBasedAuthStatus_get
 * Description:
 *      Get the authenticated status of 802.1x port-based network access control on a specific port.
 * Input:
 *      port       - port id
 * Output:
 *      pPort_auth - The status of 802.1x port-based network access controlx is authenticated
 *                   or unauthenticated.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The authenticated status of 802.1x port-based network access control is as following:
 *      - UNAUTH
 *      - AUTH
 */
int32
dal_apollomp_dot1x_portBasedAuthStatus_get(rtk_port_t port, rtk_dot1x_auth_status_t *pPort_auth)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPort_auth), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PB_AUTHf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pPort_auth = (rtk_dot1x_auth_status_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_portBasedAuthStatus_get */

/* Function Name:
 *      dal_apollomp_dot1x_portBasedAuthStatus_set
 * Description:
 *      Set the authenticated status of 802.1x port-based network access control on a specific
 *      port.
 * Input:
 *      port      - port id
 *      port_auth - The status of 802.1x port-based network access control is authenticated
 *                  or unauthenticated.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 * Note:
 *      The authenticated status of 802.1x port-based network access control is as following:
 *      - UNAUTH
 *      - AUTH
 */
int32
dal_apollomp_dot1x_portBasedAuthStatus_set(rtk_port_t port, rtk_dot1x_auth_status_t port_auth)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((AUTH_STATUS_END <= port_auth), RT_ERR_INPUT);

    val = (uint32)port_auth;
    if ((ret = reg_array_field_write(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PB_AUTHf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_portBasedAuthStatus_set */

/* Function Name:
 *      dal_apollomp_dot1x_portBasedDirection_get
 * Description:
 *      Get the operate controlled direction 802.1x port-based network access control on a specific
 *      port.
 * Input:
 *      port            - port id
 *
 * Output:
 *      pPort_direction - The controlled direction of 802.1x port-based network access control is BOTH
 *                        or IN.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 * Note:
 *      The operate controlled direction of 802.1x port-based network access control is as following:
 *      - BOTH
 *      - IN
 */
int32
dal_apollomp_dot1x_portBasedDirection_get(rtk_port_t port, rtk_dot1x_direction_t *pPort_direction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPort_direction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PB_DIRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pPort_direction = (rtk_dot1x_direction_t)val;
    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_portBasedDirection_get */

/* Function Name:
 *      dal_apollomp_dot1x_portBasedDirection_set
 * Description:
 *      Set the operate controlled direction 802.1x port-based network access control on a specific
 *      port.
 * Input:
 *      port           - port id
 *      port_direction - The controlled direction of 802.1x port-based network access control is BOTH
 *                        or IN.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 * Note:
 *    The operate controlled direction of 802.1x port-based network access control is as following:
 *    - BOTH
 *    - IN
 */
int32
dal_apollomp_dot1x_portBasedDirection_set(rtk_port_t port, rtk_dot1x_direction_t port_direction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((DIRECTION_END <= port_direction), RT_ERR_INPUT);

    val = (uint32)port_direction;
    if ((ret = reg_array_field_write(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PB_DIRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_portBasedDirection_set */

/* Module Name    : Dot1x                */
/* Sub-module Name: 802.1x MAC-based NAC */
/* Function Name:
 *      dal_apollomp_dot1x_macBasedEnable_get
 * Description:
 *      Get the status of 802.1x MAC-based network access control on a specific port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - The status of 802.1x MAC-based network access control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    (1) If a port is 802.1x MAC based network access control "enabled",
 *        the incoming packets should be authenticated so packets from that port wont be dropped
 *        or trapped to CPU.
 *
 *    (2) The status of 802.1x MAC-based network access control is as following:
 *        - DISABLED
 *        - ENABLED
 */
int32
dal_apollomp_dot1x_macBasedEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MAC_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;
    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_macBasedEnable_get */

/* Function Name:
 *      dal_apollomp_dot1x_macBasedEnable_set
 * Description:
 *      Set the status of 802.1x MAC-based network access control on a specific port.
 * Input:
 *      port   - port id
 *      enable - The status of 802.1x MAC-based network access control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 * Note:
 *      (1) If a port is 802.1x MAC based network access control "enabled",
 *          the incoming packets should be authenticated so packets from that port wont be dropped
 *          or trapped to CPU.
 *
 *      (2) The status of 802.1x MAC-based network access control is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_dot1x_macBasedEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_DOT1X_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MAC_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_macBasedEnable_set */


/* Function Name:
 *      dal_apollomp_dot1x_macBasedDirection_get
 * Description:
 *      Get the operate controlled direction 802.1x mac-based network access control on system.
 * Input:
 *      none
 * Output:
 *      pMac_direction - The controlled direction of 802.1x mac-based
 *                       network access control is BOTH or IN.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The operate controlled direction of 802.1x mac-based network access control is as following:
 *      - BOTH
 *      - IN
 */
int32
dal_apollomp_dot1x_macBasedDirection_get(rtk_dot1x_direction_t *pMac_direction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac_direction), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_DOT1X_CFG_1r, APOLLOMP_DOT1X_MAC_OPDIRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pMac_direction = (rtk_dot1x_direction_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_macBasedDirection_get */

/* Function Name:
 *      dal_apollomp_dot1x_macBasedDirection_set
 * Description:
 *      Set the operate controlled direction 802.1x mac-based network access control on system.
 * Input:
 *      mac_direction - The controlled direction of 802.1x mac-based
 *                      network access control is BOTH or IN.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The operate controlled direction of 802.1x mac-based network access control is as following:
 *      - BOTH
 *      - IN
 */
int32
dal_apollomp_dot1x_macBasedDirection_set(rtk_dot1x_direction_t mac_direction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((DIRECTION_END <= mac_direction), RT_ERR_INPUT);

    val = (uint32)mac_direction;
    if ((ret = reg_field_write(APOLLOMP_DOT1X_CFG_1r, APOLLOMP_DOT1X_MAC_OPDIRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_macBasedDirection_set */

/* Module Name    : Dot1x            */
/* Sub-module Name: 802.1X parameter */

/* Function Name:
 *      dal_apollomp_dot1x_portGuestVlan_get
 * Description:
 *      Get guest vlan on specified port.
 * Input:
 *      none
 * Output:
 *      pGuest_vlan - pointer to guest vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_dot1x_guestVlan_get( rtk_vlan_t *pGuest_vlan)
{
    int32   ret;
    uint32  vidx;
    uint32  vid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGuest_vlan), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_DOT1X_CFG_1r, APOLLOMP_DOT1X_GVIDXf, &vidx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr,REG_ARRAY_INDEX_NONE, vidx, APOLLOMP_EVIDf, &vid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pGuest_vlan = vid;

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_guestVlan_get */

/* Function Name:
 *      dal_apollomp_dot1x_guestVlan_set
 * Description:
 *      Set guest vlan
 * Input:
 *      guest_vlan - guest vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_VLAN_VID - invalid vlan id
 * Note:
 *      (1) Configure the port's forwarding guest vlan setting.
 *      (2) When the packet is unauthenticated and action is DOT1X_ACTION_TO_GUEST_VLAN,
 *          the packet will based on the guest vlan to forward.
 */
int32
dal_apollomp_dot1x_guestVlan_set(rtk_vlan_t guest_vlan)
{
    int32  ret;
    uint32 vidx;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < guest_vlan), RT_ERR_VLAN_VID);

    if ((ret = dal_apollomp_vlan_checkAndCreate(guest_vlan, &vidx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_DOT1X), "");
        return ret;
    }

    val = vidx;

    if ((ret = reg_field_write(APOLLOMP_DOT1X_CFG_1r, APOLLOMP_DOT1X_GVIDXf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_guestVlan_set */

/* Function Name:
 *      dal_apollomp_dot1x_guestVlanBehavior_get
 * Description:
 *      Get forwarding behavior for host in guest vlan.
 * Input:
 *      none
 * Output:
 *      pBehavior - pointer to Forwarding behavior
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding behavior is as following
 *      - DISALLOW_TO_AUTH_DA
 *      - ALLOW_TO_AUTH_DA
 */
int32
dal_apollomp_dot1x_guestVlanBehavior_get(rtk_dot1x_guestVlanBehavior_t *pBehavior)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBehavior), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_DOT1X_CFG_1r, APOLLOMP_DOT1X_GVOPDIRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pBehavior = (rtk_dot1x_guestVlanBehavior_t)val;
    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_guestVlanBehavior_get */

/* Function Name:
 *      dal_apollomp_dot1x_guestVlanBehavior_set
 * Description:
 *      Set forwarding behavior for host in guest vlan.
 * Input:
 *      behavior - Forwarding behavior
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding behavior is as following
 *      - DISALLOW_TO_AUTH_DA
 *      - ALLOW_TO_AUTH_DA
 */
int32
dal_apollomp_dot1x_guestVlanBehavior_set(rtk_dot1x_guestVlanBehavior_t behavior)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((GUEST_VLAN_BEHAVIOR_END <= behavior), RT_ERR_INPUT);

    val = (uint32)behavior;
    if ((ret = reg_field_write(APOLLOMP_DOT1X_CFG_1r, APOLLOMP_DOT1X_GVOPDIRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_guestVlanBehavior_set */


/* Module Name    : Dot1x                         */
/* Sub-module Name: Parameter for trapped packets */

/* Function Name:
 *      dal_apollomp_dot1x_trapPri_get
 * Description:
 *      Get priority of trapped dot1x packets.
 * Input:
 *      none
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Get the dot1x trap to cpu priority and valid range is 0-7.
 */
int32
dal_apollomp_dot1x_trapPri_get(rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_DOT1X_CFG_0r, APOLLOMP_DOT1X_PRIORTYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    *pPriority = (rtk_pri_t)val;
    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_trapPri_get */

/* Function Name:
 *      dal_apollomp_dot1x_trapPri_set
 * Description:
 *      Set priority of trapped dot1x packet.
 * Input:
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      (1) Config the dot1x trap to cpu priority and valid range is 0-7.
 */
int32
dal_apollomp_dot1x_trapPri_set(rtk_pri_t priority)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_DOT1X),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_dot1x_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_INT_PRIORITY);

    val = (uint32)priority;
    if ((ret = reg_field_write(APOLLOMP_DOT1X_CFG_0r, APOLLOMP_DOT1X_PRIORTYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_dot1x_trapPri_set */


