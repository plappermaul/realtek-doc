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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
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

#ifndef __DAL_RTL9602C_DOT1X_H__
#define __DAL_RTL9602C_DOT1X_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef enum dal_rtl9602c_dot1x_unAuthAct_e
{
    DAL_RTL9602C_DOT1X_UNAUTH_DROP = 0,
    DAL_RTL9602C_DOT1X_UNAUTH_TRAP,
    DAL_RTL9602C_DOT1X_UNAUTH_END
}dal_rtl9602c_dot1x_unAuthAct_t;



/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9602c_dot1x_init
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
extern int32
dal_rtl9602c_dot1x_init(void);

/* Function Name:
 *      dal_rtl9602c_dot1x_unauthPacketOper_get
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
extern int32
dal_rtl9602c_dot1x_unauthPacketOper_get(rtk_port_t port, rtk_action_t *pUnauthAction);

/* Function Name:
 *      dal_rtl9602c_dot1x_unauthPacketOper_set
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
extern int32
dal_rtl9602c_dot1x_unauthPacketOper_set(rtk_port_t port, rtk_action_t unauthAction);

/* Function Name:
 *      dal_rtl9602c_dot1x_portBasedEnable_get
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
extern int32
dal_rtl9602c_dot1x_portBasedEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9602c_dot1x_portBasedEnable_set
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
extern int32
dal_rtl9602c_dot1x_portBasedEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9602c_dot1x_portBasedAuthStatus_get
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
extern int32
dal_rtl9602c_dot1x_portBasedAuthStatus_get(rtk_port_t port, rtk_dot1x_auth_status_t *pPort_auth);

/* Function Name:
 *      dal_rtl9602c_dot1x_portBasedAuthStatus_set
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
extern int32
dal_rtl9602c_dot1x_portBasedAuthStatus_set(rtk_port_t port, rtk_dot1x_auth_status_t port_auth);

/* Function Name:
 *      dal_rtl9602c_dot1x_portBasedDirection_get
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
extern int32
dal_rtl9602c_dot1x_portBasedDirection_get(rtk_port_t port, rtk_dot1x_direction_t *pPort_direction);

/* Function Name:
 *      dal_rtl9602c_dot1x_portBasedDirection_set
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
extern int32
dal_rtl9602c_dot1x_portBasedDirection_set(rtk_port_t port, rtk_dot1x_direction_t port_direction);

/* Module Name    : Dot1x                */
/* Sub-module Name: 802.1x MAC-based NAC */
/* Function Name:
 *      dal_rtl9602c_dot1x_macBasedEnable_get
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
extern int32
dal_rtl9602c_dot1x_macBasedEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9602c_dot1x_macBasedEnable_set
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
extern int32
dal_rtl9602c_dot1x_macBasedEnable_set(rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      dal_rtl9602c_dot1x_macBasedDirection_get
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
extern int32
dal_rtl9602c_dot1x_macBasedDirection_get(rtk_dot1x_direction_t *pMac_direction);

/* Function Name:
 *      dal_rtl9602c_dot1x_macBasedDirection_set
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
extern int32
dal_rtl9602c_dot1x_macBasedDirection_set(rtk_dot1x_direction_t mac_direction);


/* Module Name    : Dot1x                         */
/* Sub-module Name: Parameter for trapped packets */

/* Function Name:
 *      dal_rtl9602c_dot1x_trapPri_get
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
extern int32
dal_rtl9602c_dot1x_trapPri_get(rtk_pri_t *pPriority);

/* Function Name:
 *      dal_rtl9602c_dot1x_trapPri_set
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
extern int32
dal_rtl9602c_dot1x_trapPri_set(rtk_pri_t priority);

#endif /* __DAL_RTL9602C_DOT1X_H__ */

