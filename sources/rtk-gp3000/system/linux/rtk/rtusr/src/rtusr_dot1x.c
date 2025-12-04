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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_dot1x_init
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
rtk_dot1x_init(void)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));

    SETSOCKOPT(RTDRV_DOT1X_INIT, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_init */

/* Function Name:
 *      rtk_dot1x_unauthPacketOper_get
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
rtk_dot1x_unauthPacketOper_get(rtk_port_t port, rtk_action_t *pUnauthAction)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUnauthAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_DOT1X_UNAUTHPACKETOPER_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pUnauthAction, &dot1x_cfg.unauthAction, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_unauthPacketOper_get */

/* Function Name:
 *      rtk_dot1x_unauthPacketOper_set
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
rtk_dot1x_unauthPacketOper_set(rtk_port_t port, rtk_action_t unauthAction)
{
    rtdrv_dot1xCfg_t dot1x_cfg;


    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&dot1x_cfg.unauthAction, &unauthAction, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_DOT1X_UNAUTHPACKETOPER_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_unauthPacketOper_set */

/* Function Name:
 *      rtk_dot1x_portBasedEnable_get
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
rtk_dot1x_portBasedEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_DOT1X_PORTBASEDENABLE_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pEnable, &dot1x_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_portBasedEnable_get */

/* Function Name:
 *      rtk_dot1x_portBasedEnable_set
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
rtk_dot1x_portBasedEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));

    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&dot1x_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_DOT1X_PORTBASEDENABLE_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_portBasedEnable_set */

/* Function Name:
 *      rtk_dot1x_portBasedAuthStatus_get
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
rtk_dot1x_portBasedAuthStatus_get(rtk_port_t port, rtk_dot1x_auth_status_t *pPort_auth)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort_auth), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_DOT1X_PORTBASEDAUTHSTATUS_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pPort_auth, &dot1x_cfg.port_auth, sizeof(rtk_dot1x_auth_status_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_portBasedAuthStatus_get */

/* Function Name:
 *      rtk_dot1x_portBasedAuthStatus_set
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
rtk_dot1x_portBasedAuthStatus_set(rtk_port_t port, rtk_dot1x_auth_status_t port_auth)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));

    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&dot1x_cfg.port_auth, &port_auth, sizeof(rtk_dot1x_auth_status_t));
    SETSOCKOPT(RTDRV_DOT1X_PORTBASEDAUTHSTATUS_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_portBasedAuthStatus_set */

/* Function Name:
 *      rtk_dot1x_portBasedDirection_get
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
rtk_dot1x_portBasedDirection_get(rtk_port_t port, rtk_dot1x_direction_t *pPort_direction)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort_direction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_DOT1X_PORTBASEDDIRECTION_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pPort_direction, &dot1x_cfg.port_direction, sizeof(rtk_dot1x_direction_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_portBasedDirection_get */

/* Function Name:
 *      rtk_dot1x_portBasedDirection_set
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
rtk_dot1x_portBasedDirection_set(rtk_port_t port, rtk_dot1x_direction_t port_direction)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));

    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&dot1x_cfg.port_direction, &port_direction, sizeof(rtk_dot1x_direction_t));
    SETSOCKOPT(RTDRV_DOT1X_PORTBASEDDIRECTION_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_portBasedDirection_set */

/* Module Name    : Dot1x                */
/* Sub-module Name: 802.1x MAC-based NAC */
/* Function Name:
 *      rtk_dot1x_macBasedEnable_get
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
rtk_dot1x_macBasedEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_DOT1X_MACBASEDENABLE_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pEnable, &dot1x_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_macBasedEnable_get */

/* Function Name:
 *      rtk_dot1x_macBasedEnable_set
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
rtk_dot1x_macBasedEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));
    osal_memcpy(&dot1x_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&dot1x_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_DOT1X_MACBASEDENABLE_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_macBasedEnable_set */


/* Function Name:
 *      rtk_dot1x_macBasedDirection_get
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
rtk_dot1x_macBasedDirection_get(rtk_dot1x_direction_t *pMac_direction)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac_direction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_DOT1X_MACBASEDDIRECTION_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pMac_direction, &dot1x_cfg.mac_direction, sizeof(rtk_dot1x_direction_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_macBasedDirection_get */

/* Function Name:
 *      rtk_dot1x_macBasedDirection_set
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
rtk_dot1x_macBasedDirection_set(rtk_dot1x_direction_t mac_direction)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));

    osal_memcpy(&dot1x_cfg.mac_direction, &mac_direction, sizeof(rtk_dot1x_direction_t));
    SETSOCKOPT(RTDRV_DOT1X_MACBASEDDIRECTION_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_macBasedDirection_set */

/* Module Name    : Dot1x            */
/* Sub-module Name: 802.1X parameter */

/* Function Name:
 *      rtk_dot1x_portGuestVlan_get
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
rtk_dot1x_guestVlan_get( rtk_vlan_t *pGuest_vlan)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGuest_vlan), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_DOT1X_GUESTVLAN_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pGuest_vlan, &dot1x_cfg.guest_vlan, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_guestVlan_get */

/* Function Name:
 *      rtk_dot1x_guestVlan_set
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
rtk_dot1x_guestVlan_set(rtk_vlan_t guest_vlan)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));
    osal_memcpy(&dot1x_cfg.guest_vlan, &guest_vlan, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_DOT1X_GUESTVLAN_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_guestVlan_set */

/* Function Name:
 *      rtk_dot1x_guestVlanBehavior_get
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
rtk_dot1x_guestVlanBehavior_get(rtk_dot1x_guestVlanBehavior_t *pBehavior)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pBehavior), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_DOT1X_GUESTVLANBEHAVIOR_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pBehavior, &dot1x_cfg.behavior, sizeof(rtk_dot1x_guestVlanBehavior_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_guestVlanBehavior_get */

/* Function Name:
 *      rtk_dot1x_guestVlanBehavior_set
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
rtk_dot1x_guestVlanBehavior_set(rtk_dot1x_guestVlanBehavior_t behavior)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));
    osal_memcpy(&dot1x_cfg.behavior, &behavior, sizeof(rtk_dot1x_guestVlanBehavior_t));
    SETSOCKOPT(RTDRV_DOT1X_GUESTVLANBEHAVIOR_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_guestVlanBehavior_set */


/* Module Name    : Dot1x                         */
/* Sub-module Name: Parameter for trapped packets */

/* Function Name:
 *      rtk_dot1x_trapPri_get
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
rtk_dot1x_trapPri_get(rtk_pri_t *pPriority)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_DOT1X_TRAPPRI_GET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);
    osal_memcpy(pPriority, &dot1x_cfg.priority, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_dot1x_trapPri_get */

/* Function Name:
 *      rtk_dot1x_trapPri_set
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
rtk_dot1x_trapPri_set(rtk_pri_t priority)
{
    rtdrv_dot1xCfg_t dot1x_cfg;

    /* function body */
    osal_memset(&dot1x_cfg,0x0,sizeof(rtdrv_dot1xCfg_t));

    osal_memcpy(&dot1x_cfg.priority, &priority, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_DOT1X_TRAPPRI_SET, &dot1x_cfg, rtdrv_dot1xCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_dot1x_trapPri_set */


