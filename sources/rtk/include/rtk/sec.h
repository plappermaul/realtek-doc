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
 * Purpose : Definition those public security APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Attack prevention
 */

#ifndef __RTK_SEC_H__
#define __RTK_SEC_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
/* types of ipMacBind */
typedef enum rtk_sec_ipMacBindPktType_e
{
    RTK_SEC_IPMACBIND_PKTTYPE_NONE = 0,
    RTK_SEC_IPMACBIND_PKTTYPE_IP,
    RTK_SEC_IPMACBIND_PKTTYPE_ARP,
    RTK_SEC_IPMACBIND_PKTTYPE_END
} rtk_sec_ipMacBindPktType_t;

/* flags of ipMacBindEntry */
typedef enum rtk_sec_ipMacBindFlag_e
{
    RTK_SEC_IPMACBIND_FLAG_NONE             = 0x00000000,
    RTK_SEC_IPMACBIND_FLAG_BIND_VLAN        = 0x00000001,
    RTK_SEC_IPMACBIND_FLAG_BIND_PORT        = 0x00000002,
    RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK       = 0x00000004,       /* Trunk port */
} rtk_sec_ipMacBindFlag_t;

/* ip-mac bind entry */
typedef struct rtk_sec_ipMacBindEntry_s
{
    uint32  flags;              /* Configuration flags */

    /* IP-MAC binding */
    rtk_ip_addr_t   ipAddr;     /* IPv4 Address */
    rtk_mac_t       macAddr;    /* MAC address */

    /* port binding */
    union
    {
        rtk_port_t      port;       /* available when flag RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK is NOT set */
        rtk_trunk_t     trunkId;    /* available when flag RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK is set */
    };

    /* VLAN binding */
    rtk_vlan_t      vid;
} rtk_sec_ipMacBindEntry_t;


/*
 * Data Declaration
 */
/* Type of attack prevention */
typedef enum rtk_sec_attackType_e
{
    DAEQSA_DENY = 0,
    LAND_DENY,
    UDPBLAT_DENY,
    TCPBLAT_DENY,
    POD_DENY,
    IPV6_MIN_FRAG_SIZE_CHECK,
    ICMP_FRAG_PKTS_DENY,
    ICMPV4_PING_MAX_CHECK,
    ICMPV6_PING_MAX_CHECK,
    SMURF_DENY,
    TCPHDR_MIN_CHECK,
    SYN_SPORTL1024_DENY,
    NULLSCAN_DENY,
    XMA_DENY,
    SYNFIN_DENY,
    SYNRST_DENY,
    TCP_FRAG_OFF_MIN_CHECK,     /* Applicable to 8380, 8390, 9300, 9310 */
    ARP_INVALID,                            /* Applicable to 8380, 8390, 9300, 9310 */
    GRATUITOUS_ARP,                     /* Applicable to 8380, 8390 */
    IPV4_INVALID_LEN,                   /* Applicable to 9300 */
    IPV4_INVALID_HDR,                   /* Applicable to 9310 */
    ATTACK_TYPE_END
} rtk_sec_attackType_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      rtk_sec_init
 * Description:
 *      Initialize security module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize security module before calling any sec APIs.
 * Changes:
 *      None
 */
extern int32
rtk_sec_init(uint32 unit);

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */

/* Function Name:
 *      rtk_sec_portAttackPrevent_get
 * Description:
 *      Get action for each kind of attack on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      attack_type - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported type of attack is as following:
 *          - rtk_sec_attackType_t \ Chip:  8390    8380    9300    9310
 *          - GRATUITOUS_ARP                O       O       X       X
 *          - ARP_INVALID                   O       O       O       O
 *      (2) Supported action is as following:
 *          - rtk_action_t \ Chip:          8390    8380    9300    9310
 *          - ACTION_FORWARD                O       O       O       O
 *          - ACTION_DROP                   O       O       O       O
 *          - ACTION_TRAP2CPU               O       O       O       O
 *          - ACTION_COPY                   O       O       X       X
 *      (3) The action - ACTION_COPY is for GRATUITOUS_ARP only.
 * Changes:
 *      None
 */
extern int32
rtk_sec_portAttackPrevent_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction);

/* Function Name:
 *      rtk_sec_portAttackPrevent_set
 * Description:
 *      Set action for each kind of attack on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      attack_type - type of attack
 *      action      - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported type of attack is as following:
 *          - rtk_sec_attackType_t \ Chip:  8390    8380    9300    9310
 *          - GRATUITOUS_ARP                O       O       X       X
 *          - ARP_INVALID                   O       O       O       O
 *      (2) Supported action is as following:
 *          - rtk_action_t \ Chip:          8390    8380    9300    9310
 *          - ACTION_FORWARD                O       O       O       O
 *          - ACTION_DROP                   O       O       O       O
 *          - ACTION_TRAP2CPU               O       O       O       O
 *          - ACTION_COPY                   O       O       X       X
 *      (3) ACTION_COPY is only supported by GRATUITOUS_ARP in 8380.
 * Changes:
 *      None
 */
extern int32
rtk_sec_portAttackPrevent_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action);

/* Function Name:
 *      rtk_sec_portAttackPreventEnable_get
 * Description:
 *      Get the attack prevention status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the status of the attack prevention
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Attack Prevention function is per-port enabled, use 'rtk_sec_attackPreventAction_set' to
 *      configure the action for each attack prevention type.
 * Changes:
 *      None
 */
extern int32
rtk_sec_portAttackPreventEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_sec_portAttackPreventEnable_set
 * Description:
 *      Set the attack prevention status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - pointer to the status of the attack prevention
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Attack Prevention function is per-port enabled, use 'rtk_sec_attackPreventAction_set' to
 *      configure the action for each attack prevention type.
 * Changes:
 *      None
 */
extern int32
rtk_sec_portAttackPreventEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_sec_attackPreventAction_get
 * Description:
 *      Get action for each kind of attack.
 * Input:
 *      unit        - unit id
 *      attack_type - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Type of attack is as following:
 *      - IPV4_INVALID_LEN
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMA_DENY
 *      - NULLSCAN_DENY
 *      - SYN_SPORTL1024_DENY
 *      - TCPHDR_MIN_CHECK
 *      - SMURF_DENY
 *      - ICMPV6_PING_MAX_CHECK
 *      - ICMPV4_PING_MAX_CHECK
 *      - ICMP_FRAG_PKTS_DENY
 *      - IPV6_MIN_FRAG_SIZE_CHECK
 *      - POD_DENY
 *      - TCPBLAT_DENY
 *      - UDPBLAT_DENY
 *      - LAND_DENY
 *      - DAEQSA_DENY
 *      - IPV4_INVALID_HDR
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      None
 */
extern int32
rtk_sec_attackPreventAction_get(
    uint32                      unit,
    rtk_sec_attackType_t        attack_type,
    rtk_action_t                *pAction);

/* Function Name:
 *      rtk_sec_attackPreventAction_set
 * Description:
 *      Set action for each kind of attack.
 * Input:
 *      unit        - unit id
 *      attack_type - type of attack
 *      action      - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Type of attack is as following:
 *      - IPV4_INVALID_LEN
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMA_DENY
 *      - NULLSCAN_DENY
 *      - SYN_SPORTL1024_DENY
 *      - TCPHDR_MIN_CHECK
 *      - SMURF_DENY
 *      - ICMPV6_PING_MAX_CHECK
 *      - ICMPV4_PING_MAX_CHECK
 *      - ICMP_FRAG_PKTS_DENY
 *      - IPV6_MIN_FRAG_SIZE_CHECK
 *      - POD_DENY
 *      - TCPBLAT_DENY
 *      - UDPBLAT_DENY
 *      - LAND_DENY
 *      - DAEQSA_DENY
 *      - IPV4_INVALID_HDR
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      None
 */
extern int32
rtk_sec_attackPreventAction_set(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action);

/* Function Name:
 *      rtk_sec_minIPv6FragLen_get
 * Description:
 *      Get minimum length of IPv6 fragments.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of IPv6 fragments
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_minIPv6FragLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      rtk_sec_minIPv6FragLen_set
 * Description:
 *      Set minimum length of IPv6 fragments.
 * Input:
 *      unit   - unit id
 *      length - minimum length of IPv6 fragments
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_minIPv6FragLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      rtk_sec_maxPingLen_get
 * Description:
 *      Get maximum length of ICMP packet.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pLength - pointer to maximum length of ICMP packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_maxPingLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      rtk_sec_maxPingLen_set
 * Description:
 *      Set maximum length of ICMP packet.
 * Input:
 *      unit   - unit id
 *      length - maximum length of ICMP packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_maxPingLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      rtk_sec_minTCPHdrLen_get
 * Description:
 *      Get minimum length of TCP header.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of TCP header
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_minTCPHdrLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      rtk_sec_minTCPHdrLen_set
 * Description:
 *      Set minimum length of TCP header.
 * Input:
 *      unit   - unit id
 *      length - minimum length of TCP header
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_minTCPHdrLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      rtk_sec_smurfNetmaskLen_get
 * Description:
 *      Get netmask length for preventing SMURF attack.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to netmask length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_smurfNetmaskLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      rtk_sec_smurfNetmaskLen_set
 * Description:
 *      Set netmask length for preventing SMURF attack.
 * Input:
 *      unit   - unit id
 *      length - netmask length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_sec_smurfNetmaskLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      rtk_sec_trapTarget_get
 * Description:
 *      Get information of attack trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of attack trap packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      rtk_sec_trapTarget_set
 * Description:
 *      Set information of attack trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of attack trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_trapTarget_set(uint32 unit, rtk_trapTarget_t target);


/* Function Name:
 *      rtk_sec_ipMacBindAction_get
 * Description:
 *      Get forwarding actions of various results of IP-MAC bind packets.
 * Input:
 *      unit         - unit id
 * Output:
 *      pLumisAct    - pointer to the forwarding action of IP-MAC bind look-up-miss packets
 *      pMatchAct    - pointer to the forwarding action of IP-MAC bind match packets
 *      pMismatchAct - pointer to the forwarding action of IP-MAC bind mismatch packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_ipMacBindAction_get(uint32 unit, rtk_action_t *pLumisAct, rtk_action_t *pMatchAct, rtk_action_t *pMismatchAct);

/* Function Name:
 *      rtk_sec_ipMacBindAction_set
 * Description:
 *      Set forwarding actions of various results of IP-MAC bind packets.
 * Input:
 *      unit        - unit id
 *      lumisAct    - forwarding action of IP-MAC bind look-up-miss packets
 *      matchAct    - forwarding action of IP-MAC bind match packets
 *      mismatchAct - forwarding action of IP-MAC bind mismatch packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_ipMacBindAction_set(uint32 unit, rtk_action_t lumisAct, rtk_action_t matchAct, rtk_action_t mismatchAct);

/* Function Name:
 *      rtk_sec_portIpMacBindEnable_get
 * Description:
 *      Get enable status of IP-MAC bind of the specified packet type.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - packet type
 * Output:
 *      pEnable - pointer to the enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_portIpMacBindEnable_get(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_sec_portIpMacBindEnable_set
 * Description:
 *      Set enable status of IP-MAC bind of the specified packet type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - packet type
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_portIpMacBindEnable_set(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_sec_ipMacBindEntry_add
 * Description:
 *      Add an IP-MAC bind entry.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_EXIST  - input entry has already been added
 *      RT_ERR_TBL_FULL     - table is full
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_ipMacBindEntry_add(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry);

/* Function Name:
 *      rtk_sec_ipMacBindEntry_del
 * Description:
 *      Delete an IP-MAC bind entry.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - input entry is not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_ipMacBindEntry_del(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry);

/* Function Name:
 *      rtk_sec_ipMacBindEntry_getNext
 * Description:
 *      Get the next valid IP-MAC bind entry (based on the base index)
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of IP-MAC table
 *      (2) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_ipMacBindEntry_getNext(uint32 unit, int32 *pBase, rtk_sec_ipMacBindEntry_t *pEntry);

/* Function Name:
 *      rtk_sec_attackPreventHit_get
 * Description:
 *      Get hit indication for each kind of attack.
 * Input:
 *      unit            - unit id
 *      attack_type     - type of attack
 * Output:
 *      pHit            - pointer to hit indication for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Type of attack is as following:
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMA_DENY
 *      - XMA_DENY
 *      - SYN_SPORTL1024_DENY
 *      - TCPHDR_MIN_CHECK
 *      - SMURF_DENY
 *      - ICMPV6_PING_MAX_CHECK
 *      - ICMPV4_PING_MAX_CHECK
 *      - ICMP_FRAG_PKTS_DENY
 *      - IPV6_MIN_FRAG_SIZE_CHECK
 *      - POD_DENY
 *      - TCPBLAT_DENY
 *      - UDPBLAT_DENY
 *      - LAND_DENY
 *      - DAEQSA_DENY
 *      - IPV4_INVALID_HDR
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_sec_attackPreventHit_get(uint32 unit,
    rtk_sec_attackType_t attack_type, uint32 *pHit);

#endif /* __RTK_SEC_H__ */


