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
 * $Revision: 79393 $
 * $Date: 2017-06-05 13:37:38 +0800 (Mon, 05 Jun 2017) $
 *
 * Purpose : Definition of TRAP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration for traping packet to CPU
 *           (2) RMA
 *           (3) User defined RMA
 *           (4) System-wise management frame
 *           (5) System-wise user defined management frame
 *           (6) Per port user defined management frame
 *           (7) Packet with special flag or option
 *           (8) CFM and OAM packet
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

/* Module Name    : Trap                                    */
/* Sub-module Name: Configuration for traping packet to CPU */
/* Function Name:
 *      rt_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rt_trap_init(void)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    SETSOCKOPT(RTDRV_RT_TRAP_INIT, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_init */

/* Function Name:
 *      rt_trap_portIgmpMldCtrlPktAction_get
 * Description:
 *      Get the configuration about MLD control packets Action
 * Input:
 *      port        - The ingress port ID.
 *      igmpMldType - IGMP/MLD protocol type;
 * Output:
 *      pAction     - Action of IGMP/MLD control packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portIgmpMldCtrlPktAction_get(rt_port_t port, rt_trap_igmpMld_type_t igmpMldType, rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.igmpMldType, &igmpMldType, sizeof(rt_trap_igmpMld_type_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTIGMPMLDCTRLPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portIgmpMldCtrlPktAction_get */

/* Function Name:
 *      rt_trap_portIgmpMldCtrlPktAction_set
 * Description:
 *      Set the configuration about MLD control packets Action
 * Input:
 *      port        - The ingress port ID.
 *      igmpMldType - IGMP/MLD protocol type;
 *      action      - Action of IGMP/MLD control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portIgmpMldCtrlPktAction_set(rt_port_t port, rt_trap_igmpMld_type_t igmpMldType, rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.igmpMldType, &igmpMldType, sizeof(rt_trap_igmpMld_type_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTIGMPMLDCTRLPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portIgmpMldCtrlPktAction_set */

/* Module Name    : Trap       */
/* Sub-module Name: OAM packet */

/* Function Name:
 *      rt_trap_oamPduAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU on specified port.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_oamPduAction_get(rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TRAP_OAMPDUACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_oamPduAction_get */

/* Function Name:
 *      rt_trap_oamPduAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU on specified port.
 * Input:
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_oamPduAction_set(rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_OAMPDUACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_oamPduAction_set */

/* Function Name:
 *      rt_trap_omciAction_get
 * Description:
 *      Get forwarding action of OMCI.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of OMCI
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_omciAction_get(rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TRAP_OMCIACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_omciAction_get */

/* Function Name:
 *      rt_trap_omciAction_set
 * Description:
 *      Set forwarding action of OMCI.
 * Input:
 *      action - forwarding action of OMCI
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_omciAction_set(rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_OMCIACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_omciAction_set */

/* Function Name:
 *      rt_trap_portDhcpPktAction_get
 * Description:
 *      Get the configuration about DHCP packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of DHCP packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portDhcpPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTDHCPPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portDhcpPktAction_get */

/* Function Name:
 *      rt_trap_portDhcpPktAction_set
 * Description:
 *      Set the configuration about DHCP packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of DHCP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portDhcpPktAction_set(rt_port_t port,  rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTDHCPPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portDhcpPktAction_set */


/* Function Name:
 *      rt_trap_portPppoePktAction_get
 * Description:
 *      Get the configuration about PPPoE packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of PPPoE packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portPppoePktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTPPPOEPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portPppoePktAction_get */

/* Function Name:
 *      rt_trap_portPppoePktAction_set
 * Description:
 *      Set the configuration about PPPoE packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of PPPoE control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portPppoePktAction_set(rt_port_t port,  rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTPPPOEPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portPppoePktAction_set */


/* Function Name:
 *      rt_trap_portStpPktAction_get
 * Description:
 *      Get the configuration about STP packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of STP packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portStpPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTSTPPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portStpPktAction_get */

/* Function Name:
 *      rt_trap_portStpPktAction_set
 * Description:
 *      Set the configuration about STP packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of STP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portStpPktAction_set(rt_port_t port,  rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTSTPPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portStpPktAction_set */


/* Function Name:
 *      rt_trap_portHostPktAction_get
 * Description:
 *      Get the configuration about host packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of host packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portHostPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTHOSTPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portHostPktAction_get */

/* Function Name:
 *      rt_trap_portHostPktAction_set
 * Description:
 *      Set the configuration about host packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of host control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portHostPktAction_set(rt_port_t port,  rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTHOSTPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portHostPktAction_set */


/* Function Name:
 *      rt_trap_portEthertypePktAction_get
 * Description:
 *      Get the configuration about special ethertype packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of special ethertype packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portEthertypePktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTETHERTYPEPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portEthertypePktAction_get */

/* Function Name:
 *      rt_trap_portEthertypePktAction_set
 * Description:
 *      Set the configuration about special ethertype packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of special ethertype control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portEthertypePktAction_set(rt_port_t port,  rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTETHERTYPEPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portEthertypePktAction_set */


/* Function Name:
 *      rt_trap_portArpPktAction_get
 * Description:
 *      Get the configuration about ARP packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of ARP packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_portArpPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTARPPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portArpPktAction_get */

/* Function Name:
 *      rt_trap_portArpPktAction_set
 * Description:
 *      Set the configuration about ARP packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of ARP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_portArpPktAction_set(rt_port_t port,  rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTARPPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portArpPktAction_set */

/* Function Name:
 *      rt_trap_portLoopDetectPktAction_get
 * Description:
 *      Get the configuration about loop detect(0xfffa) packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of ARP packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - pAction is NULL pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_portLoopDetectPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTLOOPDETECTPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portLoopDetectPktAction_get */

/* Function Name:
 *      rt_trap_portLoopDetectPktAction_set
 * Description:
 *      Set the configuration about loop detect(0xfffa) packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of ARP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_portLoopDetectPktAction_set(rt_port_t port, rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    memset(&rt_trap_cfg,0,sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTLOOPDETECTPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portLoopDetectPktAction_set */

/* Function Name:
 *      rt_trap_portDot1xPktAction_get
 * Description:
 *      Get the configuration about Dot1x(0x888e) packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of ARP packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - pAction is NULL pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_portDot1xPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PORTDOT1XPKTACTION_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pAction, &rt_trap_cfg.action, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_trap_portDot1xPktAction_get */

/* Function Name:
 *      rt_trap_portDot1xPktAction_set
 * Description:
 *      Set the configuration about Dot1x(0x888e) packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of ARP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rt_trap_portDot1xPktAction_set(rt_port_t port, rt_action_t action)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    memset(&rt_trap_cfg,0,sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.action, &action, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PORTDOT1XPKTACTION_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_portDot1xPktAction_set */


/* Function Name:
 *      rt_trap_pattern_get
 * Description:
 *      Get the configuration about ARP packets Action
 * Input:
 *      type        - Pattern type.
 * Output:
 *      pattern     - Trap pattern data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_pattern_get(rt_trap_pattern_type_t type, rt_trap_pattern_t *pattern)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pattern), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_trap_cfg.type, &type, sizeof(rt_trap_pattern_type_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PATTERN_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pattern, &rt_trap_cfg.attern, sizeof(rt_trap_pattern_t));

    return RT_ERR_OK;
}   /* end of rt_trap_pattern_get */


/* Function Name:
 *      rt_trap_portArpPktAction_set
 * Description:
 *      Set the configuration about ARP packets Action
 * Input:
 *      type        - Pattern type.
 *      pattern     - Trap pattern data
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_pattern_set(rt_trap_pattern_type_t type, rt_trap_pattern_t pattern)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    osal_memset(&rt_trap_cfg, 0x0, sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.type, &type, sizeof(rt_trap_pattern_type_t));
    osal_memcpy(&rt_trap_cfg.pattern, &pattern, sizeof(rt_trap_pattern_t));
    SETSOCKOPT(RTDRV_RT_TRAP_PATTERN_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_pattern_set */

/* Function Name:
 *      rt_trap_priority_get
 * Description:
 *      Get the priority for specify protocol
 * Input:
 *      port        - The ingress port ID.
 *      type        - Protocol type.
 * Output:
 *      priority    - Trap priority
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid protocol type
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 *
 */
int32
rt_trap_priority_get(rt_port_t port, rt_trap_protocol_t type, uint8 *pPri)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_trap_cfg,0,sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.type, &type, sizeof(rt_trap_protocol_t));
    GETSOCKOPT(RTDRV_RT_TRAP_PRIORITY_GET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);
    osal_memcpy(pPri, &rt_trap_cfg.pri, sizeof(uint8));

    return RT_ERR_OK;
}   /* end of rt_trap_priority_get */

/* Function Name:
 *      rt_trap_priority_set
 * Description:
 *      Set the priority for specify protocol
 * Input:
 *      port        - The ingress port ID.
 *      type        - Protocol type.
 *      priority    - Trap priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid protocol type
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 *
 */
int32
rt_trap_priority_set(rt_port_t port, rt_trap_protocol_t type, uint8 pri)
{
    rtdrv_rt_trapCfg_t rt_trap_cfg;

    /* function body */
    memset(&rt_trap_cfg,0,sizeof(rtdrv_rt_trapCfg_t));
    osal_memcpy(&rt_trap_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_trap_cfg.type, &type, sizeof(rt_trap_protocol_t));
    osal_memcpy(&rt_trap_cfg.pri, &pri, sizeof(uint8));
    SETSOCKOPT(RTDRV_RT_TRAP_PRIORITY_SET, &rt_trap_cfg, rtdrv_rt_trapCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_trap_priority_set */

