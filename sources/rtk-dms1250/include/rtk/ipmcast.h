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
 * Purpose : Definition of L3 IP Multicast API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) L3 Multicast Address
 *           (2) L3 Multicast control
 *
 */

#ifndef __RTK_IPMCAST_H__
#define __RTK_IPMCAST_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <rtk/mcast.h>

/*
 * Symbol Definition
 */

/* Definition for flags in rtk_ipmc_flag_t */
typedef enum rtk_ipmc_flag_e
{
    RTK_IPMC_FLAG_NONE                  = 0x00000000,   /* none */
    RTK_IPMC_FLAG_REPLACE               = 0x00000001,   /* replace the existing entry with the new info */
    RTK_IPMC_FLAG_DISABLE               = 0x00000002,   /* invalid the entry */
    RTK_IPMC_FLAG_IPV6                  = 0x00000004,   /* IPv6-specific (belong to IPv6 related entry) */
    RTK_IPMC_FLAG_SIP                   = 0x00000008,   /* when set, means (S, G) entry; otherwise (*, G) */
    RTK_IPMC_FLAG_SRC_INTF_ID           = 0x00000010,   /* for IP multicast, use interface as key (S,G,INTF) instead of VID (S,G,VID). */
    RTK_IPMC_FLAG_RPF_EN                = 0x00000020,   /* RPF enabled or not */
    RTK_IPMC_FLAG_QOS_ASSIGN            = 0x00000040,   /* assign a new internal priority */
    RTK_IPMC_FLAG_HIT                   = 0x00000080,   /* hit-specific (hit bit is set) */
    RTK_IPMC_FLAG_HIT_CLEAR             = 0x00000100,   /* action - clear the hit bit if it's set */
    RTK_IPMC_FLAG_READ_HIT_IGNORE       = 0x00000400,   /* when get ipmc addr entry, ignore entry hit flag, only for 9300 */
} rtk_ipmc_flag_t;

/* L3 IPMCAST drop/trap action */
typedef enum rtk_ipmc_act_e
{
    RTK_IPMC_ACT_FORWARD = 0,
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_COPY2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    RTK_IPMC_ACT_COPY2MASTERCPU,
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_END
} rtk_ipmc_act_t;

/* L3 IPMCAST RPF fail action */
typedef enum rtk_ipmc_rpfFailAct_e
{
    RTK_IPMC_RPF_FAIL_ACT_TRAP = 0,
    RTK_IPMC_RPF_FAIL_ACT_DROP,
    RTK_IPMC_RPF_FAIL_ACT_COPY,
    RTK_IPMC_RPF_FAIL_ACT_ASSERT_CHK,
    RTK_IPMC_RPF_FAIL_ACT_END
} rtk_ipmc_rpfFailAct_t;

/* IPMCAST CPU target */
typedef enum rtk_ipmc_cpuTarget_e
{
    RTK_IPMC_CPUTARGET_LOCAL = 0,
    RTK_IPMC_CPUTARGET_MASTER,
    RTK_IPMC_CPUTARGET_END,
} rtk_ipmc_cpuTarget_t;

/* IPMCAST global control type (GCT) */
typedef enum rtk_ipmc_globalCtrlType_e
{
    RTK_IPMC_GCT_NONE = 0,                              /* Dummy */

    /* IP multicast routing */
    RTK_IPMC_GCT_LU_HASH_WT_INTF_EN,                    /* rtk_enable_t */

    /* IPv4 multicast */
    RTK_IPMC_GCT_IPMC_GLB_EN,                           /* rtk_enable_t */
    RTK_IPMC_GCT_IPMC_BAD_SIP_ACT,                      /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT,                     /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT,                /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IPMC_HDR_OPT_ACT,                      /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN,                 /* rtk_enable_t */
    RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT,                     /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT,                     /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET,                /* rtk_ipmc_cpuTarget_t */
#ifdef CONFIG_SDK_RTL9300
    RTK_IPMC_GCT_IPMC_LU_MIS_ACT,                       /* rtk_ipmc_act_t */
#endif

    /* IPv6 multicast */
    RTK_IPMC_GCT_IP6MC_GLB_EN,                          /* rtk_enable_t */
    RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT,                     /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT,                    /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT,               /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_HBH_ACT,                         /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT,                     /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT,                   /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN,                /* rtk_enable_t */
    RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT,                    /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT,                     /* rtk_ipmc_act_t */
    RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET,               /* rtk_ipmc_cpuTarget_t */
#ifdef CONFIG_SDK_RTL9300
    RTK_IPMC_GCT_IP6MC_LU_MIS_ACT,                      /* rtk_ipmc_act_t */
#endif

    RTK_IPMC_GCT_END                                    /* end of enum */
} rtk_ipmc_globalCtrlType_t;

/*
 * Data Declaration
 */
/* L3 IPMCAST ADDR action */
typedef enum rtk_ipmc_addr_act_e
{
    RTK_IPMC_ADDR_ACT_FORWARD = 0,
    RTK_IPMC_ADDR_ACT_DROP,
    RTK_IPMC_ADDR_ACT_TRAP2CPU,
    RTK_IPMC_ADDR_ACT_COPY2CPU,
    RTK_IPMC_ADDR_ACT_END
} rtk_ipmc_addr_act_t;

/* L3 multicast address */
typedef struct rtk_ipmc_addr_s
{
    rtk_ipmc_flag_t         ipmc_flags;     /* RTK_IPMC_FLAG_xxx flags */
    rtk_vrf_id_t            vrf_id;         /* VRF ID */

    union
    {
        rtk_ipv6_addr_t     ipv6;
        rtk_ip_addr_t       ipv4;
    } src_ip_addr;                          /* Source IP address */

    union
    {
        rtk_ipv6_addr_t     ipv6;
        rtk_ip_addr_t       ipv4;
    } dst_ip_addr;                          /* Group/Destination IP address */

    union
    {
        rtk_vlan_t          vlan_id;        /* VLAN ID */
        rtk_intf_id_t       intf_id;        /* Intface ID, used with flag - RTK_IPMC_FLAG_SRC_INTF_ID */
    } src_intf;                             /* Source/Incoming Interface (VLAN or INTF) */

    rtk_mcast_group_t       group;          /* multicast group ID */

    rtk_ipmc_addr_act_t     l2_fwd_act;         /* forwarding action of L2 bridging */
    rtk_enable_t            l3_en;              /* L3 ability control */
    rtk_ipmc_addr_act_t     l3_fwd_act;         /* forwarding action of L3 routing */
    rtk_ipmc_rpfFailAct_t   l3_rpf_fail_act;    /* forwarding action of L3 RPF failed packets */

    uint32                  ttl_min;    /* TTL minimum, copy the routed packet to CPU
                                         * if its TTL is less than the value. */
    uint32                  mtu_max;    /* MTU maximum, copy the routed packet to CPU
                                         * if its L3 payload length is more than the value. */
    rtk_pri_t               qos_pri;    /* available when RTK_IPMC_FLAG_QOS_ASSIGN set */

    uint32                  hw_entry_idx;       /* hardware entry index */
} rtk_ipmc_addr_t;


typedef struct rtk_ipmc_statKey_s
{
    uint32                  flags;      /* RTK_IPMC_FLAG_xxx */
    rtk_vrf_id_t            vrf_id;     /* VRF ID */

    union
    {
        rtk_ipv6_addr_t     ipv6;
        rtk_ip_addr_t       ipv4;
    } src_ip_addr;

    union
    {
        rtk_ipv6_addr_t     ipv6;
        rtk_ip_addr_t       ipv4;
    } dst_ip_addr;

    union
    {
        rtk_vlan_t          vlan_id;
        rtk_intf_id_t       intf_id;
    } src_intf;
} rtk_ipmc_statKey_t;

typedef enum rtk_ipmc_statMontFlag_e
{
    RTK_IPMCAST_STAT_MONT_FLAG_BYTE             = 0x00000001,
    RTK_IPMCAST_STAT_MONT_FLAG_PACKET           = 0x00000002,
    RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET  = (RTK_IPMCAST_STAT_MONT_FLAG_BYTE | RTK_IPMCAST_STAT_MONT_FLAG_PACKET),
    RTK_IPMCAST_STAT_MONT_FLAG_END
} rtk_ipmc_statMontFlag_t;

/* Statistic monitor */
typedef struct rtk_ipmc_statMont_s
{
    rtk_ipmc_statMontFlag_t  mont_flags;
    rtk_ipmc_statKey_t       key;
} rtk_ipmc_statMont_t;

/* Statistic counter */
typedef struct rtk_ipmc_statCntr_s
{
    uint64          rxByte;
    uint64          rxPacket;
} rtk_ipmc_statCntr_t;


/*
 * Function Declaration
 */

/* Module Name : L3 Multicast */

/* Function Name:
 *      rtk_ipmc_init
 * Description:
 *      Initialize L3 IP multicast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300, 9310
 * Note:
 *      Must initialize L3 IP multicast module before calling any L3 multicast APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_init(uint32 unit);

/* Function Name:
 *      rtk_ipmc_addr_t_init
 * Description:
 *      Initialize data structure of rtk_ipmc_addr_t
 * Input:
 *      pAddr - pointer to rtk_ipmc_addr_t
 * Output:
 *      pAddr - pointer to rtk_ipmc_addr_t((initialized))
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
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
rtk_ipmc_addr_t_init(rtk_ipmc_addr_t *pAddr);

/* Function Name:
 *      rtk_ipmc_addr_add
 * Description:
 *      Add a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found (when try to replace)
 *      RT_ERR_IPMC_ADDR      - invalid multicast ip address
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, src_ip_addr(if RTK_IPMC_FLAG_SIP is set) and dst_ip_addr (either ipv4 or ipv6),
 *          and group (if RTK_IPMC_FLAG_NULL_INTF isn't set).
 *          and vlan_id or intf_id for 9310(only vlan_id for 9300)
 *      (2) Applicable flags:
 *          RTK_IPMC_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_IPMC_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                          (path ID will not be referred)
 *          RTK_IPMC_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_IPMC_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_IPMC_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_IPMC_FLAG_SIP             - entry is dip + sip
 *          RTK_IPMC_FLAG_SRC_INTF_ID     - only for 9310
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pAddr);

/* Function Name:
 *      rtk_ipmc_addr_find
 * Description:
 *      Find a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pMcastAddras input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          and vlan_id or intf_id for 9310(only vlan_id for 9300)
 *      (2) Applicable flags:
 *          RTK_IPMC_FLAG_HIT_CLEAR     - clear the hit bit if it's set
 *          RTK_IPMC_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *          RTK_IPMC_FLAG_SRC_INTF_ID   - only for 9310
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t *pAddr);

/* Function Name:
 *      rtk_ipmc_addr_del
 * Description:
 *      Delete a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pAddr input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          and vlan_id or intf_id for 9310(only vlan_id for 9300)
 *      (2) Applicable flags:
 *          RTK_IPMC_FLAG_SRC_INTF_ID   - only for 9310
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pAddr);

/* Function Name:
 *      rtk_ipmc_addr_delAll
 * Description:
 *      Delete all L3 multicast address
 * Input:
 *      unit - unit id
 *      flag - configure flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_IPMC_FLAG_IPV6   - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flag);

/* Function Name:
 *      rtk_ipmc_nextValidAddr_get
 * Description:
 *      Get next L3 multicast address with matched flags
 * Input:
 *      unit  - unit id
 *      flags - flags
 *      pBase - start index
 * Output:
 *      pAddr - pointer to L3 multicast address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_IPMC_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *      (2) Use base index -1 to indicate to search from the beginging of ipmcast entry table
 *      (3) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_nextValidAddr_get(uint32 unit, uint32 flags, int32 *pBase, rtk_ipmc_addr_t *pAddr);

/* Function Name:
 *      rtk_ipmc_statMont_create
 * Description:
 *      Create a statistic monitor session
 * Input:
 *      unit      - unit id
 *      pStatMont - statistic monitor information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont);

/* Function Name:
 *      rtk_ipmc_statMont_destroy
 * Description:
 *      Destroy a statistic monitor session
 * Input:
 *      unit      - unit id
 *      pStatMont - statistic monitor information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont);

/* Function Name:
 *      rtk_ipmc_statCntr_reset
 * Description:
 *      Reset (clear) counter(s)
 * Input:
 *      unit - unit id
 *      pKey - key for search the statistic session
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pKey);

/* Function Name:
 *      rtk_ipmc_statCntr_get
 * Description:
 *      Get counter and parameter of L3 counter
 * Input:
 *      unit  - unit id
 *      pKey  - key for search the statistic session
 * Output:
 *      pCntr - Statistic counter vlanue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pKey, rtk_ipmc_statCntr_t *pCntr);

/* Function Name:
 *      rtk_ipmc_globalCtrl_set
 * Description:
 *      Set status of specified L3 multicast control
 * Input:
 *      unit - unit id
 *      type - L3 multicast control type
 *      arg  - control stauts
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      rtk_ipmc_globalCtrl_get
 * Description:
 *      Get status of specified L3 multicast control
 * Input:
 *      unit - unit id
 *      type - L3 multicast control type
 * Output:
 *      pArg - control stauts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg);

#endif /* __RTK_IPMCAST_H__ */

