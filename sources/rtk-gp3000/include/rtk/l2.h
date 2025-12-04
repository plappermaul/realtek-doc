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
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
 */

#ifndef __RTK_L2_H__
#define __RTK_L2_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */
/* Definition for flags in rtk_l2_ucastAddr_t */
#define RTK_L2_UCAST_FLAG_SA_BLOCK          0x00000001
#define RTK_L2_UCAST_FLAG_DA_BLOCK          0x00000002
#define RTK_L2_UCAST_FLAG_STATIC            0x00000004
#define RTK_L2_UCAST_FLAG_ARP_USED          0x00000008
#define RTK_L2_UCAST_FLAG_FWD_PRI           0x00000010
#define RTK_L2_UCAST_FLAG_LOOKUP_PRI        0x00000020
#define RTK_L2_UCAST_FLAG_IVL               0x00000040
#define RTK_L2_UCAST_FLAG_CTAG_IF			0x00000080

#define RTK_L2_UCAST_FLAG_ALL               ( RTK_L2_UCAST_FLAG_SA_BLOCK | \
                                            RTK_L2_UCAST_FLAG_DA_BLOCK   | \
                                            RTK_L2_UCAST_FLAG_STATIC     | \
                                            RTK_L2_UCAST_FLAG_ARP_USED   | \
                                            RTK_L2_UCAST_FLAG_FWD_PRI    | \
                                            RTK_L2_UCAST_FLAG_LOOKUP_PRI | \
                                            RTK_L2_UCAST_FLAG_IVL		 | \
                                            RTK_L2_UCAST_FLAG_CTAG_IF)

/* Definition for flags in rtk_l2_mcastAddr_t */
#define RTK_L2_MCAST_FLAG_FWD_PRI           0x00000001
#define RTK_L2_MCAST_FLAG_IVL               0x00000002

#define RTK_L2_MCAST_FLAG_ALL               (RTK_L2_MCAST_FLAG_FWD_PRI | RTK_L2_MCAST_FLAG_IVL)

/* Definition for flags in rtk_l2_ipmcastAddr_t */
#define RTK_L2_IPMCAST_FLAG_FWD_PRI         0x00000001
#define RTK_L2_IPMCAST_FLAG_DIP_ONLY        0x00000002
#define RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE 0x00000004 /* Only valid when RTK_L2_IPMCAST_FLAG_DIP_ONLY is set */
#define RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN   0x00000008 /* Only valid when RTK_L2_IPMCAST_FLAG_DIP_ONLY is set */
#define RTK_L2_IPMCAST_FLAG_STATIC          0x00000010
#define RTK_L2_IPMCAST_FLAG_IPV6            0x00000020
#define RTK_L2_IPMCAST_FLAG_SIP_FILTER      0x00000040
#define RTK_L2_IPMCAST_FLAG_IVL             0x00000080

#define RTK_L2_IPMCAST_FLAG_ALL             ( RTK_L2_IPMCAST_FLAG_FWD_PRI | \
                                            RTK_L2_IPMCAST_FLAG_DIP_ONLY | \
                                            RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE | \
                                            RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN | \
                                            RTK_L2_IPMCAST_FLAG_STATIC | \
                                            RTK_L2_IPMCAST_FLAG_IPV6 | \
                                            RTK_L2_IPMCAST_FLAG_SIP_FILTER | \
                                            RTK_L2_IPMCAST_FLAG_IVL)

#define RTK_L2_DEFAULT_AGING_TIME           (300 * 10) /* 300 seconds */

/* l2 limit learning count action */
typedef enum rtk_l2_limitLearnCntAction_e
{
    LIMIT_LEARN_CNT_ACTION_DROP = ACTION_DROP,
    LIMIT_LEARN_CNT_ACTION_FORWARD = ACTION_FORWARD,
    LIMIT_LEARN_CNT_ACTION_TO_CPU = ACTION_TRAP2CPU,
    LIMIT_LEARN_CNT_ACTION_COPY_CPU = ACTION_COPY2CPU,
    LIMIT_LEARN_CNT_ACTION_END
} rtk_l2_limitLearnCntAction_t;

/* l2 limit learning count action */
typedef enum rtk_l2_limitLearnEntryAction_e
{
    LIMIT_LEARN_ENTRY_ACTION_FORWARD = ACTION_FORWARD,
    LIMIT_LEARN_ENTRY_ACTION_TO_CPU = ACTION_TRAP2CPU,
    LIMIT_LEARN_ENTRY_ACTION_END = ACTION_END
} rtk_l2_limitLearnEntryAction_t;

/* New unicast MAC address learning mode */
typedef enum rtk_l2_newMacLrnMode_e
{
    HARDWARE_LEARNING = 0,
    SOFTWARE_LEARNING,
    NOT_LEARNING,
    LEARNING_MODE_END,
} rtk_l2_newMacLrnMode_t;

/* L2 IP multicast switch mode */
typedef enum rtk_l2_ipmcMode_e
{
    LOOKUP_ON_MAC_AND_VID_FID = 0,
    LOOKUP_ON_DIP_AND_SIP,
    LOOKUP_ON_DIP_AND_VID,
    LOOKUP_ON_DIP,
    LOOKUP_ON_DIP_AND_CVID,
    LOOKUP_ON_DIP_AND_VID_FID,
    IPMC_MODE_END
} rtk_l2_ipmcMode_t;

/* L2 IP multicast VLAN switch mode */
typedef enum rtk_l2_ipmcVlanMode_e
{
    LOOKUP_VLAN_BY_IVL_SVL = 0,
    LOOKUP_VLAN_FORCE_NO_VLAN,
    LOOKUP_VLAN_FORCE_VLAN,
    IPMC_VLAN_MODE_END
} rtk_l2_ipmcVlanMode_t;

/* L2 IP multicast hash operation */
typedef enum rtk_l2_ipmcHashOp_e
{
    HASH_DIP_AND_SIP = 0,
    HASH_DIP_ONLY,
    HASH_END
} rtk_l2_ipmcHashOp_t;

/* Type of lookup miss */
typedef enum rtk_l2_lookupMissType_e
{
    DLF_TYPE_IPMC = 0,
    DLF_TYPE_UCAST,
    DLF_TYPE_BCAST,
    DLF_TYPE_MCAST,
    DLF_TYPE_IP6MC,
    DLF_TYPE_RSVIPMC,
    DLF_TYPE_RSVIP6MC,
    DLF_TYPE_ICMP6MC,
    DLF_TYPE_DHCP6MC,
    DLF_TYPE_END
} rtk_l2_lookupMissType_t;


/*l2 address read method*/
typedef enum rtk_l2_readMethod_e
{
	LUT_READ_METHOD_MAC = 0,
	LUT_READ_METHOD_ADDRESS,
	LUT_READ_METHOD_NEXT_ADDRESS,
	LUT_READ_METHOD_NEXT_L2UC,
	LUT_READ_METHOD_NEXT_L2MC,
	LUT_READ_METHOD_NEXT_L3MC,
	LUT_READ_METHOD_NEXT_L2L3MC,
	LUT_READ_METHOD_NEXT_L2UCSPA,
	LUT_READ_METHOD_END,
	
}rtk_l2_readMethod_t;

/* Type of IP6 Multilcast Reserved Address*/
typedef enum rtk_l2_ip6McReservedAddr_e
{
    LUT_IP6MCADDR_RSVDOC_FF01 = 0,
    LUT_IP6MCADDR_RSVDOC_FF02,
    LUT_IP6MCADDR_RSVDOC_FF05,
    LUT_IP6MCADDR_RSVDOC_FF08,
    LUT_IP6MCADDR_RSVDOC_FF0E,
    LUT_IP6MCADDR_SOILCITED_NOTE,
    LUT_IP6MCADDR_END
} rtk_l2_ip6McReservedAddr_t;

/* Hash types */
typedef enum rtk_l2_hashType_e
{
    LUT_HASHTYPE_L2_UCSVL = 0,
    LUT_HASHTYPE_L2_UCIVL,
    LUT_HASHTYPE_L2_MCSVL,
    LUT_HASHTYPE_L2_MCIVL,
    LUT_HASHTYPE_L2_MC,
    LUT_HASHTYPE_L3_SIPGIP,
    LUT_HASHTYPE_L3_GIPFID,
    LUT_HASHTYPE_L3_GIPVID,
    LUT_HASHTYPE_L3_GIP,
    LUT_HASHTYPE_L3_GIPFIDSIPFILTER,
    LUT_HASHTYPE_L3_GIPVIDSIPFILTER,
    LUT_HASHTYPE_L3_DIP6,
    LUT_HASHTYPE_END
} rtk_l2_hashType_t;

typedef enum rtk_l2_hashDataFlag_e
{
    LUT_HASHFLAG_MAC = (1<<0),
    LUT_HASHFLAG_FID = (1<<1),
    LUT_HASHFLAG_EFID = (1<<2),
    LUT_HASHFLAG_VID = (1<<3),
    LUT_HASHFLAG_GIP = (1<<4),
    LUT_HASHFLAG_SIP = (1<<5),
    LUT_HASHFLAG_DIP6 = (1<<6),
    LUT_HASHFLAG_SIP_FILTER_EN = (1<<7),
    LUT_HASHFLAG_SIP_FILTER_IDX = (1<<8),
} rtk_l2_hashDataFlag_t;


/* l2 address table - unicast data structure */
typedef struct rtk_l2_ucastAddr_s
{
    rtk_vlan_t  vid;
    rtk_mac_t   mac;
    uint32      fid;
    uint32      efid;
    rtk_port_t  port;
    uint32      ext_port;
    uint32      flags;          /* Refer to RTK_L2_UCAST_FLAG_XXX */
    uint32      age;
    uint32      priority;
    uint8       auth;
    uint32      index;
	rtk_vlan_t  ctag_vid;
}rtk_l2_ucastAddr_t;

/* l2 address table - multicast data structure */
typedef struct rtk_l2_mcastAddr_s
{
    uint16          vid;
    rtk_mac_t       mac;
    uint32          fid;
    rtk_portmask_t  portmask;
    rtk_portmask_t  ext_portmask; /* Support in RTL9600 Series, RTL9601B, RTl9602C */
    uint32          ext_portmask_idx; /* Support in RTL9607C */
    uint32          flags;          /* Refer to RTK_L2_MCAST_FLAG_XXX */
    uint32          priority;
    uint32          index;
}rtk_l2_mcastAddr_t;

/* l2 address table - ip multicast data structure */
typedef struct rtk_l2_ipMcastAddr_s
{
    ipaddr_t        dip;
    ipaddr_t        sip;
    rtk_ipv6_addr_t dip6;
    uint16          vid;
    uint32          fid;
    uint32          sip_index;
    rtk_portmask_t  portmask;
    rtk_portmask_t  ext_portmask; /* Support in RTL9600 Series, RTL9601B, RTl9602C */
    uint32          ext_portmask_idx; /* Support in RTL9607C */
    uint32          l3_trans_index;
    uint32          l3_mcr_index;
    uint32          flags;          /* Refer to RTK_L2_IPMCAST_FLAG_XXX */
    uint32          priority;
    uint32          index;
}rtk_l2_ipMcastAddr_t;

typedef enum rtk_lut_entry_type_e
{
    RTK_LUT_L2UC = 0,
    RTK_LUT_L2MC,
    RTK_LUT_L3MC,
    RTK_LUT_L3V6MC,
    RTK_LUT_END
} rtk_lut_entry_type_t;

/* l2 address table - for all type */
typedef struct rtk_l2_addr_table_s
{
    rtk_lut_entry_type_t entryType;
	rtk_l2_readMethod_t  method;
    union
    {
        rtk_l2_ucastAddr_t      l2UcEntry;
        rtk_l2_mcastAddr_t      l2McEntry;
        rtk_l2_ipMcastAddr_t    ipmcEntry;
    }entry;
}rtk_l2_addr_table_t;

typedef struct rtk_l2_flushCfg_s
{
    rtk_enable_t    flushByVid;
    rtk_vlan_t      vid;
    rtk_enable_t    flushByPort;
    rtk_port_t      port;
    rtk_enable_t    flushByFid;
    rtk_port_t      fid;
    rtk_enable_t    flushStaticAddr;
    rtk_enable_t    flushDynamicAddr;
    rtk_enable_t    flushAddrOnAllPorts; /* this is used when flushByVid */
} rtk_l2_flushCfg_t;

typedef struct rtk_l2_hashData_s
{
    uint32          dataFlags;
    uint32          fid;
    uint32          efid;
    uint32          vid;
    rtk_mac_t       mac;
    rtk_ip_addr_t   gip;
    rtk_ip_addr_t   sip;
    rtk_ipv6_addr_t dip6;
    rtk_enable_t    sip_filter_en;
    uint16          sip_filter_idx;
} rtk_l2_hashData_t;

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      rtk_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
extern int32
rtk_l2_init(void);

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
extern int32
rtk_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration.
 * Input:
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
extern int32
rtk_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable);



/* Function Name:
 *      rtk_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
rtk_l2_ucastAddr_flush(rtk_l2_flushCfg_t *pConfig);

/* Function Name:
 *      rtk_l2_table_clear
 * Description:
 *      Clear entire L2 table.
 *      All the entries (static and dynamic) (L2 and L3) will be deleted.
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
rtk_l2_table_clear(void);


/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      rtk_l2_limitLearningOverStatus_get
 * Description:
 *      Get the system learning over status
 * Input:
 *      None.
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
rtk_l2_limitLearningOverStatus_get(uint32 *pStatus);

/* Function Name:
 *      rtk_l2_limitLearningOverStatus_clear
 * Description:
 *      Clear the system learning over status
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
extern int32
rtk_l2_limitLearningOverStatus_clear(void);

/* Function Name:
 *      rtk_l2_learningCnt_get
 * Description:
 *      Get the total mac learning counts of the whole specified device.
 * Input:
 *      None.
 * Output:
 *      pMacCnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
rtk_l2_learningCnt_get(uint32 *pMacCnt);

/* Function Name:
 *      rtk_l2_limitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
extern int32
rtk_l2_limitLearningCnt_get(uint32 *pMacCnt);

/* Function Name:
 *      rtk_l2_limitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the specified device.
 * Input:
 *      macCnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the system.
 */
extern int32
rtk_l2_limitLearningCnt_set(uint32 macCnt);

/* Function Name:
 *      rtk_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
extern int32
rtk_l2_limitLearningCntAction_get(rtk_l2_limitLearnCntAction_t *pLearningAction);

/* Function Name:
 *      rtk_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the specified device.
 * Input:
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
extern int32
rtk_l2_limitLearningCntAction_set(rtk_l2_limitLearnCntAction_t learningAction);

/* Function Name:
 *      rtk_l2_limitLearningEntryAction_get
 * Description:
 *      Get the action when over learning the same hash result entry.
 * Input:
 *      None
 * Output:
 *      pLearningAction - pointer buffer of action when over learning the same hash result entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_ENTRY_ACTION_FORWARD
 *          - LIMIT_LEARN_ENTRY_ACTION_TO_CPU
 */
extern int32
rtk_l2_limitLearningEntryAction_get(rtk_l2_limitLearnEntryAction_t *pLearningAction);

/* Function Name:
 *      rtk_l2_limitLearningEntryAction_set
 * Description:
 *      Set the action when over learning the same hash result entry.
 * Input:
 *      learningAction - action when over learning the same hash result entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_ENTRY_ACTION_FORWARD
 *          - LIMIT_LEARN_ENTRY_ACTION_TO_CPU
 */
extern int32
rtk_l2_limitLearningEntryAction_set(rtk_l2_limitLearnEntryAction_t learningAction);

/* Function Name:
 *      rtk_l2_portLimitLearningOverStatus_get
 * Description:
 *      Get the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
rtk_l2_portLimitLearningOverStatus_get(rtk_port_t port, uint32 *pStatus);

/* Function Name:
 *      rtk_l2_portLimitLearningOverStatus_clear
 * Description:
 *      Clear the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
extern int32
rtk_l2_portLimitLearningOverStatus_clear(rtk_port_t port);

/* Function Name:
 *      rtk_l2_limitLearningPortMask_get
 * Description:
 *      Get the port mask that indicate which port(s) are counted into
 *      system learning count
 * Input:
 *      None
 * Output:
 *      pPortmask - configure the port mask which counted into system counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
extern int32
rtk_l2_limitLearningPortMask_get(rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_l2_limitLearningPortMask_set
 * Description:
 *      Set the port mask that indicate which port(s) are counted into
 *      system learning count
 * Input:
 *      portmask - configure the port mask which counted into system counter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_MASK
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
extern int32
rtk_l2_limitLearningPortMask_set(rtk_portmask_t portmask);

/* Function Name:
 *      rtk_l2_portLearningCnt_get
 * Description:
 *      Get the mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt  - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
rtk_l2_portLearningCnt_get(rtk_port_t port, uint32 *pMacCnt);

/* Function Name:
 *      rtk_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
extern int32
rtk_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt);


/* Function Name:
 *      rtk_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
extern int32
rtk_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt);


/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      port    - port id
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
extern int32
rtk_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pLearningAction);


/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      port   - port id
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
extern int32
rtk_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t learningAction);



/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */

/* Function Name:
 *      rtk_l2_aging_get
 * Description:
 *      Get the dynamic address aging time.
 * Input:
 *      None
 * Output:
 *      pAgingTime - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (0.1sec)
 */
extern int32
rtk_l2_aging_get(uint32 *pAgingTime);


/* Function Name:
 *      rtk_l2_aging_set
 * Description:
 *      Set the dynamic address aging time.
 * Input:
 *      agingTime - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) RTL8329/RTL8389 aging time is not configurable.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 */
extern int32
rtk_l2_aging_set(uint32 agingTime);


/* Function Name:
 *      rtk_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out setting of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of Age
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      port    - port id
 *      enable  - enable status of Age
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
rtk_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      rtk_l2_lookupMissAction_get
 * Description:
 *      Get forwarding action when destination address lookup miss.
 * Input:
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
extern int32
rtk_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction);

/* Function Name:
 *      rtk_l2_lookupMissAction_set
 * Description:
 *      Set forwarding action when destination address lookup miss.
 * Input:
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU (only for DLF_TYPE_UCAST) 
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
extern int32
rtk_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action);

/* Function Name:
 *      rtk_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
extern int32
rtk_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction);

/* Function Name:
 *      rtk_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
extern int32
rtk_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action);

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type   - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
extern int32
rtk_l2_lookupMissFloodPortMask_get(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
extern int32
rtk_l2_lookupMissFloodPortMask_set(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration.
 */
extern int32
rtk_l2_lookupMissFloodPortMask_add(rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_del
 * Description:
 *      Del one port member in flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration..
 */
extern int32
rtk_l2_lookupMissFloodPortMask_del(rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

/* Module Name    : L2      */
/* Sub-module Name: Unicast */
/* Function Name:
 *      rtk_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
extern int32
rtk_l2_newMacOp_get(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction);

/* Function Name:
 *      rtk_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
extern int32
rtk_l2_newMacOp_set(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction);


/* Module Name    : L2              */
/* Sub-module Name: Get next entry */

/* Function Name:
 *      rtk_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx       - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2UcastData   - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
extern int32
rtk_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData);


/* Function Name:
 *      rtk_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2UcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
extern int32
rtk_l2_nextValidAddrOnPort_get(
    rtk_port_t          port,
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData);


/* Function Name:
 *      rtk_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pL2McastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          reply next valid L2 multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
extern int32
rtk_l2_nextValidMcastAddr_get(
    int32               *pScanIdx,
    rtk_l2_mcastAddr_t  *pL2McastData);

/* Function Name:
 *      rtk_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pIpMcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and multicast entry and
 *          reply next valid L2 ip multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
extern int32
rtk_l2_nextValidIpMcastAddr_get(
    int32                   *pScanIdx,
    rtk_l2_ipMcastAddr_t    *pIpMcastData);



/* Function Name:
 *      rtk_l2_nextValidEntry_get
 * Description:
 *      Get LUT next valid entry.
 * Input:
 *      pScanIdx - Index field in the structure.
 * Output:
 *      pL2Entry - entry content
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get next valid LUT entry.
 */
extern int32
rtk_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry);



/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      rtk_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
extern int32
rtk_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr);

/* Function Name:
 *      rtk_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
extern int32
rtk_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr);


/* Function Name:
 *      rtk_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
extern int32
rtk_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr);


/* Function Name:
 *      rtk_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry.
 * Input:
 *      includeStatic - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l2_addr_delAll(uint32 includeStatic);


/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      rtk_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
extern int32
rtk_l2_mcastAddr_add(rtk_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtk_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      None
 */
extern int32
rtk_l2_mcastAddr_del(rtk_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtk_l2_mcastAddr_get
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
rtk_l2_mcastAddr_get(rtk_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtk_l2_extMemberConfig_get
 * Description:
 *      Get the extension port member config
 * Input:
 *      cfgIndex  - extension port member config index
 * Output:
 *      pPortmask - extension port mask for specific index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
extern int32
rtk_l2_extMemberConfig_get(uint32 cfgIndex, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_l2_extMemberConfig_set
 * Description:
 *      Set the extension port member config
 * Input:
 *      cfgIndex  - extension port member config index
 *      portmask  - extension port mask for specific index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
extern int32
rtk_l2_extMemberConfig_set(uint32 cfgIndex, rtk_portmask_t portmask);

/* Function Name:
 *      rtk_l2_vidUnmatchAction_get
 * Description:
 *      Get forwarding action when vid learning unmatch happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
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
extern int32
rtk_l2_vidUnmatchAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction);

/* Function Name:
 *      rtk_l2_vidUnmatchAction_set
 * Description:
 *      Set forwarding action when vid learning unmatch happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
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
extern int32
rtk_l2_vidUnmatchAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction);


/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      rtk_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
extern int32
rtk_l2_illegalPortMoveAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction);

/* Function Name:
 *      rtk_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
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
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
extern int32
rtk_l2_illegalPortMoveAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction);


/* Module Name    : L2           */
/* Sub-module Name: IP multicast */


/* Function Name:
 *      rtk_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP_AND_VID
 */
extern int32
rtk_l2_ipmcMode_get(rtk_l2_ipmcMode_t *pMode);

/* Function Name:
 *      rtk_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP_AND_VID
 */
extern int32
rtk_l2_ipmcMode_set(rtk_l2_ipmcMode_t mode);

/* Function Name:
 *      rtk_l2_ipmcVlanMode_get
 * Description:
 *      Get lookup VLAN mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup VLAN mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_VLAN_BY_IVL_SVL
 *      - LOOKUP_VLAN_FORCE_NO_VLAN
 *      - LOOKUP_VLAN_FORCE_VLAN
 */
extern int32
rtk_l2_ipmcVlanMode_get(rtk_l2_ipmcVlanMode_t *pMode);

/* Function Name:
 *      rtk_l2_ipmcVlanMode_set
 * Description:
 *      Set lookup VLAN mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup VLAN mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_VLAN_BY_IVL_SVL
 *      - LOOKUP_VLAN_FORCE_NO_VLAN
 *      - LOOKUP_VLAN_FORCE_VLAN
 */
extern int32
rtk_l2_ipmcVlanMode_set(rtk_l2_ipmcVlanMode_t mode);

/* Function Name:
 *      rtk_l2_ipv6mcMode_get
 * Description:
 *      Get lookup mode of layer2 ipv6 multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ipv6 multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ipv6 multicast switching is as following
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP
 */
extern int32
rtk_l2_ipv6mcMode_get(rtk_l2_ipmcMode_t *pMode);

/* Function Name:
 *      rtk_l2_ipv6mcMode_set
 * Description:
 *      Set lookup mode of layer2 ipv6 multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ipv6 multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ipv6 multicast switching is as following
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP
 */
extern int32
rtk_l2_ipv6mcMode_set(rtk_l2_ipmcMode_t mode);

/* Function Name:
 *      rtk_l2_ipmcGroupLookupMissHash_get
 * Description:
 *      Get Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      None
 * Output:
 *      pIpmcHash - pointer of Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
rtk_l2_ipmcGroupLookupMissHash_get(rtk_l2_ipmcHashOp_t *pIpmcHash);

/* Function Name:
 *      rtk_l2_ipmcGroupLookupMissHash_set
 * Description:
 *      Set Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      ipmcHash - Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
rtk_l2_ipmcGroupLookupMissHash_set(rtk_l2_ipmcHashOp_t ipmcHash);

/* Function Name:
 *      rtk_l2_ipmcGroup_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip         - Group IP
 *      pPortmask   - Group member port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroup_add(ipaddr_t gip, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_l2_ipmcGroupExtPortmask_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip             - Group IP
 *      pExtPortmask    - Group member extension port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroupExtPortmask_add(ipaddr_t gip, rtk_portmask_t *pExtPortmask);

/* Function Name:
 *      rtk_l2_ipmcGroupExtMemberIdx_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip             - Group IP
 *      index           - Group member extension port mask index configured by rtk_l2_extMemberConfig_set
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroupExtMemberIdx_add(ipaddr_t gip, uint32 index);

/* Function Name:
 *      rtk_l2_ipmcGroup_del
 * Description:
 *      Delete an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroup_del(ipaddr_t gip);

/* Function Name:
 *      rtk_l2_ipmcGroup_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      pPortmask   - Group member port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroup_get(ipaddr_t gip, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_l2_ipmcGroupExtPortmask_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip             - Group IP
 * Output:
 *      pExtPortmask    - Group member extension port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroupExtPortmask_get(ipaddr_t gip, rtk_portmask_t *pExtPortmask);

/* Function Name:
 *      rtk_l2_ipmcGroupExtMemberIdx_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip             - Group IP
 * Output:
 *      pIndex          - Group member extension port mask index configured by rtk_l2_extMemberConfig_set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcGroupExtMemberIdx_get(ipaddr_t gip, uint32 *pIndex);

/* Function Name:
 *      rtk_l2_ipmcSipFilter_set
 * Description:
 *      Set an entry to IPMC SIP filter Table by index
 * Input:
 *      index       - table index
 *      sip         - SIP to be filtered
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcSipFilter_set(uint32 index, ipaddr_t sip);

/* Function Name:
 *      rtk_l2_ipmcSipFilter_get
 * Description:
 *      Get an entry from IPMC SIP filter Table by index
 * Input:
 *      index       - table index
 * Output:
 *      pSip        - SIP buffer to store the retrieved data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      
 * Note:
 *      None
 */
extern int32
rtk_l2_ipmcSipFilter_get(uint32 index, ipaddr_t *pSip);

/* Function Name:
 *      rtk_l2_portIpmcAction_get
 * Description:
 *      Get the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 * Output:
 *      pAction     - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l2_portIpmcAction_get(rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      rtk_l2_portIpmcAction_set
 * Description:
 *      Set the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 *      action      - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 * Note:
 *      None
 */
extern int32
rtk_l2_portIpmcAction_set(rtk_port_t port, rtk_action_t action);

/* Function Name:
 *      rtk_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_IPV4_ADDRESS  - Invalid IPv4 address
 *      RT_ERR_VLAN_VID      - invalid vlan id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
rtk_l2_ipMcastAddr_add(rtk_l2_ipMcastAddr_t *pIpmcastAddr);

/* Function Name:
 *      rtk_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      pIpmcastAddr  - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      (1) In vlan unaware mode (SVL), the vid will be ignore, suggest to
 *          input vid=0 in vlan unaware mode.
 *      (2) In vlan aware mode (IVL), the vid will be care.
 */
extern int32
rtk_l2_ipMcastAddr_del(rtk_l2_ipMcastAddr_t *pIpmcastAddr);

/* Function Name:
 *      rtk_l2_ipMcastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
rtk_l2_ipMcastAddr_get(rtk_l2_ipMcastAddr_t *pIpmcastAddr);


/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */


/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
extern int32
rtk_l2_srcPortEgrFilterMask_get(rtk_portmask_t *pFilter_portmask);

/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
extern int32
rtk_l2_srcPortEgrFilterMask_set(rtk_portmask_t *pFilter_portmask);

/* Function Name:
 *      rtk_l2_extPortEgrFilterMask_get
 * Description:
 *      Get extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
extern int32
rtk_l2_extPortEgrFilterMask_get(rtk_portmask_t *pExt_portmask);

/* Function Name:
 *      rtk_l2_extPortEgrFilterMask_set
 * Description:
 *      Set extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
extern int32
rtk_l2_extPortEgrFilterMask_set(rtk_portmask_t *pExt_portmask);


/* Function Name:
 *      rtk_l2_camState_set
 * Description:
 *      Set LUT cam state 
 * Input:
 *      camState - enable or disable cam state 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
extern int32
rtk_l2_camState_set(rtk_enable_t camState);


/* Function Name:
 *      rtk_l2_camState_get
 * Description:
 *      Get LUT cam state 
 * Input:
 *      pCamState - status of cam state 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *       
 */
extern int32
rtk_l2_camState_get(rtk_enable_t *pCamState);

/* Function Name:
 *      rtk_l2_ip6mcReservedAddrEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      type    - ip6 mc reserved address type 
 *      enable  - treat as reserved state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l2_ip6mcReservedAddrEnable_set(rtk_l2_ip6McReservedAddr_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_l2_ip6mcReservedAddrEnable_get
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      type    - ip6 mc reserved address type 
 * Output:
 *      pEnable - treat as reserved addresss state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l2_ip6mcReservedAddrEnable_get(rtk_l2_ip6McReservedAddr_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_l2_hashValue_get
 * Description:
 *      Calculate the hash value for specific hash type
 * Input:
 *      type        - hash type
 *      hashData    - input data to be hashed
 * Output:
 *      pHashValue  - hash result of specified type with data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
extern int32
rtk_l2_hashValue_get(rtk_l2_hashType_t type, rtk_l2_hashData_t hashData, uint32 *pHashValue);
#endif /* __RTK_L2_H__ */

