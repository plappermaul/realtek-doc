/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public security APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Attack prevention
 *
 */

#ifndef __DAL_MANGO_SEC_H__
#define __DAL_MANGO_SEC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/sec.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Function Name:
 *      dal_mango_secMapper_init
 * Description:
 *      Hook sec module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook sec module before calling any sec APIs.
 */
extern int32
dal_mango_secMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_sec_init
 * Description:
 *      Initialize security module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize security module before calling any sec APIs.
 */
extern int32
dal_mango_sec_init(uint32 unit);

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */

/* Function Name:
 *      dal_mango_sec_portAttackPrevent_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of attack is as following:
 *      - ARP_INVALID
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_mango_sec_portAttackPrevent_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction);

/* Function Name:
 *      dal_mango_sec_portAttackPrevent_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Type of attack is as following:
 *      - ARP_INVALID
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_mango_sec_portAttackPrevent_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action);

/* Function Name:
 *      dal_mango_sec_portAttackPreventEnable_get
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sec_portAttackPreventEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_sec_portAttackPreventEnable_set
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_sec_portAttackPreventEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_sec_attackPreventAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of attack is as following:
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
 */
extern int32
dal_mango_sec_attackPreventAction_get(
    uint32                      unit,
    rtk_sec_attackType_t        attack_type,
    rtk_action_t                *pAction);

/* Function Name:
 *      dal_mango_sec_attackPreventAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Type of attack is as following:
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
 */
extern int32
dal_mango_sec_attackPreventAction_set(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action);

/* Function Name:
 *      dal_mango_sec_minIPv6FragLen_get
 * Description:
 *      Get minimum length of IPv6 fragments.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of IPv6 fragments
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sec_minIPv6FragLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      dal_mango_sec_minIPv6FragLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_mango_sec_minIPv6FragLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      dal_mango_sec_maxPingLen_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sec_maxPingLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      dal_mango_sec_maxPingLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_mango_sec_maxPingLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      dal_mango_sec_minTCPHdrLen_get
 * Description:
 *      Get minimum length of TCP header.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of TCP header
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sec_minTCPHdrLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      dal_mango_sec_minTCPHdrLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_mango_sec_minTCPHdrLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      dal_mango_sec_smurfNetmaskLen_get
 * Description:
 *      Get netmask length for preventing SMURF attack.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to netmask length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sec_smurfNetmaskLen_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      dal_mango_sec_smurfNetmaskLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_mango_sec_smurfNetmaskLen_set(uint32 unit, uint32 length);

/* Function Name:
 *      dal_mango_sec_trapTarget_get
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
 * Note:
 */
extern int32
dal_mango_sec_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      dal_mango_sec_trapTarget_set
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
 * Note:
 */
extern int32
dal_mango_sec_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      dal_mango_sec_ipMacBindAction_get
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
 * Note:
 */
extern int32
dal_mango_sec_ipMacBindAction_get(uint32 unit, rtk_action_t *pLumisAct, rtk_action_t *pMatchAct, rtk_action_t *pMismatchAct);

/* Function Name:
 *      dal_mango_sec_ipMacBindAction_get
 * Description:
 *      Get forwarding actions of various results of IP-MAC bind packets.
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
 * Note:
 */
extern int32
dal_mango_sec_ipMacBindAction_set(uint32 unit, rtk_action_t lumisAct, rtk_action_t matchAct, rtk_action_t mismatchAct);

/* Function Name:
 *      dal_mango_sec_portIpMacBindEnable_get
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
 * Note:
 */
extern int32
dal_mango_sec_portIpMacBindEnable_get(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_sec_portIpMacBindEnable_set
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
 * Note:
 */
extern int32
dal_mango_sec_portIpMacBindEnable_set(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_sec_ipMacBindEntry_add
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
 * Note:
 */
extern int32
dal_mango_sec_ipMacBindEntry_add(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry);

/* Function Name:
 *      dal_mango_sec_ipMacBindEntry_del
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
 * Note:
 */
extern int32
dal_mango_sec_ipMacBindEntry_del(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry);

/* Function Name:
 *      dal_mango_sec_ipMacBindEntry_getNext
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
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of IP-MAC table
 *      (2) If the returned index is -1, means not found the next valid entry
 */
extern int32
dal_mango_sec_ipMacBindEntry_getNext(uint32 unit, int32 *pBase, rtk_sec_ipMacBindEntry_t *pEntry);

/* Function Name:
 *      dal_mango_sec_attackPreventHit_get
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
 */
extern int32
dal_mango_sec_attackPreventHit_get(uint32 unit,
    rtk_sec_attackType_t attack_type, uint32 *pHit);

#endif /* __DAL_MANGO_SEC_H__ */


