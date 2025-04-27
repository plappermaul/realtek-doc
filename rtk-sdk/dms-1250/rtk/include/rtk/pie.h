/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public PIE APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Template/Block
 *            2) Field Selector
 *            3) Range Check
 *            4) Meter
 *
 */

#ifndef __RTK_PIE_H__
#define __RTK_PIE_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/port.h>
#include <rtk/vlan.h>

/*
 * Symbol Definition
 */
#define RTK_MAX_NUM_OF_PIE_TEMPLATE_FIELD   16
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
#define RTK_MAX_NUM_OF_PIE_BLOCK_TEMPLATE   3
#else   /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
#define RTK_MAX_NUM_OF_PIE_BLOCK_TEMPLATE   2
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
//#define RTK_MAX_SIZE_OF_PIE_USER_FIELD      28  /* maximum user field size among chips*/
//#define RTK_MAX_SIZE_OF_PIE_ENTRY           56  /* maximum entry size among chips*/

#define PIE_ENTRY_PHYSICAL_OFST             31
#define PIE_ENTRY_PHYSICAL_FLAG             (1 << PIE_ENTRY_PHYSICAL_OFST)
#define PIE_ENTRY_PHYSICAL_TYPE(_idx)       ((_idx) | PIE_ENTRY_PHYSICAL_FLAG)
#define PIE_ENTRY_IS_PHYSICAL_TYPE(_idx)    (((_idx) >> PIE_ENTRY_PHYSICAL_OFST) & 0x1)
#define PIE_ENTRY_PHYSICAL_CLEAR(_idx)      ((_idx) & (~ PIE_ENTRY_PHYSICAL_FLAG))

/*
 * Data Declaration
 */
typedef enum rtk_pie_phase_e
{
    PIE_PHASE_DIS,			/* Applicable to 9310 */
    PIE_PHASE_VACL,			/* Applicable to 9300/9310 */
    PIE_PHASE_IACL,			/* Applicable to 9300/9310 */
    PIE_PHASE_EACL,			/* Applicable to 9310 */
    PIE_PHASE_IGR_FLOW_TABLE_0,	/* Applicable to 9310 */
    PIE_PHASE_IGR_FLOW_TABLE_3,	/* Applicable to 9310 */
    PIE_PHASE_EGR_FLOW_TABLE_0,	/* Applicable to 9310 */
    PIE_PHASE_END
} rtk_pie_phase_t;

typedef enum rtk_pie_meterType_e
{
    METER_TYPE_INVALID = 0,
    /* Applicable to 8380 */
    METER_TYPE_SLB,
    /* Applicable to 8390/9300/9310 */
    METER_TYPE_DLB,
    /* Applicable to 8390/9300/9310 */
    METER_TYPE_SRTCM,
    /* Applicable to 8390/9300/9310 */
    METER_TYPE_TRTCM,
    METER_TYPE_END
} rtk_pie_meterType_t;

typedef enum rtk_pie_meterMode_e
{
    METER_MODE_BYTE = 0,
    METER_MODE_PACKET,
    METER_MODE_END
} rtk_pie_meterMode_t;

typedef struct rtk_pie_meterEntry_s
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_pie_meterType_t type;   /* 0x0: invalid, 0x1:SLB, 0x2: DLB; 0x3: srTCM; 0x4: trTCM. */
    uint8   color_aware;        /* Meter is color aware. This bit takes effect when meter is srTCM or trTCM.*/
    uint32  lb0_rate;           /* Leaky bucket 0 rate (DLB) or Committed Information Rate (srTCM or trTCM)*/
    uint32  lb1_rate;           /* Leaky bucket 1 rate ( DLB) or Peak Information Rate (trTCM), this rate is not taken used for srTCM*/
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32  lb0_bs;             /* Leaky bucket 0 burst size */
    uint32  lb1_bs;             /* Leaky bucket 1 burst size */
    rtk_pie_meterMode_t mode;
#endif  /* defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */
#endif  /* defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */
#if defined(CONFIG_SDK_RTL8380)
    uint32 enable;              /* enable state. Should set this bit each time changing the parameters of this entry */
    uint32 rate;                /* 18-bit wide rate setting in units of pps, or 16-bit wide rate setting in units of 16Kbps */
    uint32 thr_grp;             /* select threshold group. 0: group 0, 1: group 1 */
#endif  /* defined(CONFIG_SDK_RTL8380) */
} rtk_pie_meterEntry_t;

typedef enum rtk_pie_meterDpSel_e
{
    METER_DP_SEL_DEEPEST_COLOR = 0,     /* select deepest color */
    METER_DP_SEL_LOWEST_IDX,            /* select the color from lowest entry index */
    METER_DP_SEL_END
} rtk_pie_meterDpSel_t;

typedef enum rtk_pie_meterTrtcmType_e
{
    METER_TRTCM_TYPE_ORIGINAL,  /* RFC-2698 */
    METER_TRTCM_TYPE_MODIFIED,  /* RFC-4115 */
    METER_TRTCM_TYPE_END
} rtk_pie_meterTrtcmType_t;

typedef enum rtk_pie_arpMacSel_e
{
    ARP_MAC_SEL_L2 = 0,         /* select SMAC/DMAC field filter L2 SMAC/DMAC for ARP packet */
    ARP_MAC_SEL_ARP,            /* select SMAC/DMAC field filter ARP sender/target hardeware address for ARP packet */
    ARP_MAC_SEL_END
} rtk_pie_arpMacSel_t;

typedef enum rtk_pie_intfSel_e
{
    PIE_INTF_SEL_L3 = 0,    /* select interface field filter L3 interface */
    PIE_INTF_SEL_TUNNEL,    /* select interface field filter tunnel interface */
    PIE_INTF_SEL_END
} rtk_pie_intfSel_t;

/* template field in user view */
typedef enum rtk_pie_templateFieldType_e
{
    TMPLTE_FIELD_NONE = 0,
    TMPLTE_FIELD_SPM0,                  /* SPM[15:0]   */
    TMPLTE_FIELD_SPM1,                  /* SPM[31:16]  */
    TMPLTE_FIELD_DMAC0,                 /* DMAC[15:0]  */
    TMPLTE_FIELD_DMAC1,                 /* DMAC[31:16] */
    TMPLTE_FIELD_DMAC2,                 /* DMAC[47:32] */
    TMPLTE_FIELD_SMAC0,                 /* SMAC[15:0]  */
    TMPLTE_FIELD_SMAC1,                 /* SMAC[31:16] */
    TMPLTE_FIELD_SMAC2,                 /* SMAC[47:32] */
    TMPLTE_FIELD_ETHERTYPE,
    TMPLTE_FIELD_OTAG,
    TMPLTE_FIELD_ITAG,
    TMPLTE_FIELD_SIP0,                  /* IP4 SIP[15:0]/IP6 SIP[15:0]   */
    TMPLTE_FIELD_SIP1,                  /* IP4 SIP[31:16]/IP6 SIP[31:16] */
    /*Applicable to 8390, 9300, 9310 */
    TMPLTE_FIELD_SIP2,                  /* IP6 SIP[47:32]  */
    /*Applicable to 8390, 9300, 9310 */
    TMPLTE_FIELD_SIP3,                  /* IP6 SIP[63:48]  */
    /*Applicable to 8390, 9300, 9310 */
    TMPLTE_FIELD_SIP4,                  /* IP6 SIP[79:64]  */
    /*Applicable to 8390, 9300, 9310 */
    TMPLTE_FIELD_SIP5,                  /* IP6 SIP[95:80]  */
    /*Applicable to 8390, 9300, 9310 */
    TMPLTE_FIELD_SIP6,                  /* IP6 SIP[111:96] */
    /*Applicable to 8390, 9300, 9310 */
    TMPLTE_FIELD_SIP7,                  /* IP6 SIP[127:112]*/
    TMPLTE_FIELD_DIP0,                  /* IP4 DIP[15:0]/IP6 DIP[15:0]   */
    TMPLTE_FIELD_DIP1,                  /* IP4 DIP[31:16]/IP6 DIP[31:16] */
    TMPLTE_FIELD_IP_TOS_PROTO,
    TMPLTE_FIELD_L4_SPORT,
    TMPLTE_FIELD_L4_DPORT,
    TMPLTE_FIELD_L34_HEADER,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_TCP_INFO,
    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
    TMPLTE_FIELD_FIELD_SELECTOR_0,
    TMPLTE_FIELD_FIELD_SELECTOR_1,
    TMPLTE_FIELD_FIELD_SELECTOR_2,
    TMPLTE_FIELD_FIELD_SELECTOR_3,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_4,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_5,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_6,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_7,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_8,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_9,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_10,
    /*Applicable to 8390, 9300, 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_11,
    /*Applicable to 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_12,
    /*Applicable to 9310*/
    TMPLTE_FIELD_FIELD_SELECTOR_13,
    TMPLTE_FIELD_DIP2,                  /* IP6 DIP[47:32]  */
    TMPLTE_FIELD_DIP3,                  /* IP6 DIP[63:48]  */
    TMPLTE_FIELD_DIP4,                  /* IP6 DIP[79:64]  */
    TMPLTE_FIELD_DIP5,                  /* IP6 DIP[95:80]  */
    TMPLTE_FIELD_DIP6,                  /* IP6 DIP[111:96] */
    TMPLTE_FIELD_DIP7,                  /* IP6 DIP[127:112]*/
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_PKT_INFO,
    /*Applicable to 8380, 9300, 9310*/
    TMPLTE_FIELD_FLOW_LABEL,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_DSAP_SSAP,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_SNAP_OUI,
    TMPLTE_FIELD_FWD_VID,
    /*Applicable to 8380, 9300, 9310*/
    TMPLTE_FIELD_RANGE_CHK,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_VLAN_GMSK,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_DLP,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_META_DATA,
    /*Applicable to 9300 */
    TMPLTE_FIELD_SRC_FWD_VID,
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_VLAN,              /* for fixed template, and per phase configure */
    /*Applicable to 9300, 9310*/
    TMPLTE_FIELD_SLP,
    /*Applicable to 9310*/
    TMPLTE_FIELD_TTID,
    /*Applicable to 9310*/
    TMPLTE_FIELD_TSID,
    /*Applicable to 9310*/
    TMPLTE_FIELD_FIRST_MPLS1,
    /*Applicable to 9310*/
    TMPLTE_FIELD_FIRST_MPLS2,
    /*Applicable to 8380/8390*/
    TMPLTE_FIELD_SPMMASK,
    /*Applicable to 8380/8390*/
    TMPLTE_FIELD_ICMP_IGMP,
    /*Applicable to 8380*/
    TMPLTE_FIELD_IP_RANGE,
    /*Applicable to 8390/9310 */
    TMPLTE_FIELD_SPM2,
    /*Applicable to 8390/9310 */
    TMPLTE_FIELD_SPM3,
    /*Applicable to 8390*/
    TMPLTE_FIELD_IP_FLAG_OFFSET,
    /*Applicable to 8390*/
    TMPLTE_FIELD_VID_RANG0,
    /*Applicable to 8390*/
    TMPLTE_FIELD_VID_RANG1,
    /*Applicable to 8390*/
    TMPLTE_FIELD_IP_L4PORT_RANG,
    /*Applicable to 8390*/
    TMPLTE_FIELD_IP_LEN_RANG,
    /*Applicable to 8390*/
    TMPLTE_FIELD_OLABEL,
    /*Applicable to 8390*/
    TMPLTE_FIELD_ILABEL,
    /*Applicable to 8390*/
    TMPLTE_FIELD_OILABEL,
    /*Applicable to 8390*/
    TMPLTE_FIELD_DPMMASK,
    /*Applicable to 8390/9310 */
    TMPLTE_FIELD_DPM0,
    /*Applicable to 8390/9310 */
    TMPLTE_FIELD_DPM1,
    /*Applicable to 8390/9310 */
    TMPLTE_FIELD_DPM2,
    /*Applicable to 8390/9310 */
    TMPLTE_FIELD_DPM3,
    /*Applicable to 8390*/
    TMPLTE_FIELD_L2DPM0,
    /*Applicable to 8390*/
    TMPLTE_FIELD_L2DPM1,
    /*Applicable to 8390*/
    TMPLTE_FIELD_L2DPM2,
    /*Applicable to 8390*/
    TMPLTE_FIELD_L2DPM3,
    /*Applicable to 8390*/
    TMPLTE_FIELD_IVLAN,
    /*Applicable to 8390*/
    TMPLTE_FIELD_OVLAN,
    TMPLTE_FIELD_FIX,
    TMPLTE_FIELD_END
} rtk_pie_templateFieldType_t;

typedef struct rtk_pie_template_s
{
    rtk_pie_templateFieldType_t  field[RTK_MAX_NUM_OF_PIE_TEMPLATE_FIELD];
} rtk_pie_template_t;

/*
 * range_check
 */

typedef enum rtk_pie_rangeCheck_ip_type_e
{
    RNGCHK_IP_TYPE_IPV4_SRC = 0,   /* IPv4 source IP */
    RNGCHK_IP_TYPE_IPV4_DST,       /* IPv4 destination IP */
    RNGCHK_IP_TYPE_IPV6_SRC,       /* IPV6 source IP */
    RNGCHK_IP_TYPE_IPV6_DST,       /* IPv6 destination IP */
    RNGCHK_IP_TYPE_IPV6_SRC_SUFFIX,/* IPV6 source IP suffix[63:0] */
    RNGCHK_IP_TYPE_IPV6_DST_SUFFIX,/* IPv6 destination IP suffix[63:0] */
    RNGCHK_IP_TYPE_END
} rtk_pie_rangeCheck_ip_type_t;

typedef struct rtk_pie_rangeCheck_ip_s
{
    uint32  ip_lower_bound;/* IP address Lower bound */
    uint32  ip_upper_bound;/* IP address Upper bound */
#if defined(CONFIG_SDK_RTL8390)
    uint32  reverse;       /* reverse operation */
#endif
    rtk_pie_rangeCheck_ip_type_t   ip_type;
} rtk_pie_rangeCheck_ip_t;

typedef enum rtk_pie_rangeCheck_type_e
{
    RTK_RNGCHK_IVID,        /* inner VID */
    RTK_RNGCHK_OVID,        /* outer VID */
    RTK_RNGCHK_L4SPORT,     /* TCP/UDP source port */
    RTK_RNGCHK_L4DPORT,     /* TCP/UDP destination port */
    RTK_RNGCHK_L4PORT,      /* TCP/UDP source or destination port */
    RTK_RNGCHK_PKTLEN,      /* packet length include CRC (byte) */
    RTK_RNGCHK_L3LEN,       /* L3 length (byte) */
    RTK_RNGCHK_END
} rtk_pie_rangeCheck_type_t;

typedef struct rtk_pie_rangeCheck_s
{
    rtk_pie_rangeCheck_type_t   type;
    uint32                      upper_bound;
    uint32                      lower_bound;
} rtk_pie_rangeCheck_t;

/*
 * field_selector
 */
typedef enum rtk_pie_fieldSelector_startPosition_e
{
    FS_START_POS_RAW  = 0,      /* Raw packet(start after preamble,begin with DA) */
    /* Applicable to 8390, 9300, 9310 */
    FS_START_POS_LLC,           /* LLC packet(start with length "0000-05FF") */
    /* Applicable to 8380 */
    FS_START_POS_L2,            /* Start from Ethertype(EthernetII)/Length(RFC_1042/SNAP_OTHER),  VLAN tag is bypassed */
    FS_START_POS_L3,            /* L3 packet(start after EtherType field, VLAN tag is bypassed) */
    /* Applicable to 8390, 9300, 9310 */
    FS_START_POS_ARP,           /* ARP Packet(start from ARP/RARP header) */
    /* Applicable to 8390 */
    FS_START_POS_IPV4,          /* IPv4 Packet(start from IPv4 header) */
    /* Applicable to 8390 */
    FS_START_POS_IPV6,          /* IPv6 Packet(start from IPv6 header) */
    /* Applicable to 9300, 9310 */
    PIE_FS_START_POS_IP_HDR,    /* IPv4/IPv6 Packet(start from IPv4/IPv6 header) */
    /* Applicable to 8390, 9300, 9310 */
    FS_START_POS_IP,            /* IP payload(start from IP payload, also means start of layer 4 packet) */
    FS_START_POS_L4,            /* Start after TCP/UDP/ICMP/IGMP header. For 8390,ICMP/IGMP is not supported */
    FS_START_POS_END
} rtk_pie_fieldSelector_startPosition_t;

typedef enum rtk_pie_fieldSelector_payloadSel_e
{
    PIE_FS_INNER = 0,
    PIE_FS_OUTER,
    PIE_FS_END
} rtk_pie_fieldSelector_payloadSel_t;

typedef struct rtk_pie_fieldSelector_data_s
{
    rtk_pie_fieldSelector_startPosition_t   start;  /* starting position of field selector */
    /* Applicable to 9310 */
    /* Select inner or outer for tunnel packet */
    rtk_pie_fieldSelector_payloadSel_t      payloadSel;
    uint32                                  offset; /* offset in byte */
} rtk_pie_fieldSelector_data_t;

typedef enum rtk_pie_templateVlanSel_e
{
    TMPLTE_VLAN_SEL_INNER,
    TMPLTE_VLAN_SEL_OUTER,
    TMPLTE_VLAN_SEL_FWD,
    TMPLTE_VLAN_SEL_SRC_FWD,
    TMPLTE_VLAN_SEL_END
} rtk_pie_templateVlanSel_t;

typedef enum rtk_pie_templateVlanFmtSel_e
{
    TMPLTE_VLAN_FMT_SEL_ORIGINAL,
    TMPLTE_VLAN_FMT_SEL_MODIFIED,
    TMPLTE_VLAN_FMT_SEL_END
} rtk_pie_templateVlanFmtSel_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name     : PIE */

/* Function Name:
 *      rtk_pie_init
 * Description:
 *      Initialize PIE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      Must initialize PIE module before calling any PIE APIs.
 * Changes:
 *      None
 */
extern int32
rtk_pie_init(uint32 unit);

/* Function Name:
 *      rtk_pie_phase_get
 * Description:
 *      Get the pie block phase.
 * Input:
 *      unit       - unit id
 *      block_idx - block index
 * Output:
 *      pPhase - pointer buffer of phase value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase);

/* Function Name:
 *      rtk_pie_phase_set
 * Description:
 *      Set the pie block phase configuration.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      phase       - phase value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase);

/* Function Name:
 *      rtk_pie_blockLookupEnable_get
 * Description:
 *      Get the pie block lookup state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 * Output:
 *      pEnable   - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The rule data are kept regardless of the lookup status.
 *      (2) The lookup result is always false if the lookup state is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_blockLookupEnable_get)
 */
extern int32
rtk_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_pie_blockLookupEnable_set
 * Description:
 *      Set the pie block lookup state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      enable    - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The rule data are kept regardless of the lookup status.
 *      (2) The lookup result is always false if the lookup state is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_blockLookupEnable_set)
 */
extern int32
rtk_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable);

/* Function Name:
 *      rtk_pie_blockGrouping_get
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 * Output:
 *      group_id    - block group index
 *      logic_id    - block logic index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_blockGrouping_get(uint32 unit, uint32 block_idx, uint32 *group_id,
    uint32 *logic_id);

/* Function Name:
 *      rtk_pie_blockGrouping_set
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 *      group_id   - block group index
 *      logic_id   - block logic index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_blockGrouping_set(uint32 unit, uint32 block_idx, uint32 group_id,
    uint32 logic_id);

/* Function Name:
 *      rtk_pie_template_get
 * Description:
 *      Get the content of specific template.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 * Output:
 *      pTemplate   - template content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX - invalid template index
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      ID 0-4 are fixed template, ID 5-9 are user defined template.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_template_get)
 */
extern int32
rtk_pie_template_get(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate);

/* Function Name:
 *      rtk_pie_template_set
 * Description:
 *      Set the content of specific template.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 *      pTemplate   - template content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX - invalid template index
 *      RT_ERR_PIE_FIELD_TYPE     - invalid PIE field type
 *      RT_ERR_PIE_FIELD_LOCATION - invalid field location
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) ID 0-4 are fixed template, ID 5-9 are user defined template.
 *      (2) Configure fixed templates is prohibited.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_template_get)
 */
extern int32
rtk_pie_template_set(uint32 unit, uint32 template_id, rtk_pie_template_t *pTemplate);

/* Function Name:
 *      rtk_pie_templateVlanSel_get
 * Description:
 *      Get the configuration of VLAN select in pre-defined template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      preTemplate_idx     - pre-defined template index
 * Output:
 *      pVlanSel - pointer to VLAN select in pre-defined template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_templateVlanSel_get(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   *pVlanSel);

/* Function Name:
 *      rtk_pie_templateVlanSel_set
 * Description:
 *      set the configuration of VLAN select in pre-defined template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      preTemplate_idx     - pre-defined template index
 *      vlanSel - VLAN select in pre-defined template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_templateVlanSel_set(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   vlanSel);

/* Function Name:
 *      rtk_pie_templateField_check
 * Description:
 *      Check whether the specified template field type is supported on the chip.
 * Input:
 *      unit  - unit id
 *      phase - PIE lookup phase
 *      type  - template field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_PIE_PHASE      - invalid PIE phase
 *      RT_ERR_PIE_FIELD_TYPE - invalid PIE field type
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_templateField_check)
 */
extern int32
rtk_pie_templateField_check(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    rtk_pie_templateFieldType_t type);

/* Sub-module Name :
 *  Range Check
 */

/* Function Name:
 *      rtk_pie_rangeCheckIp_get
 * Description:
 *      Get the configuration of IP range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of IP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *          index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *          be used together in order to filter a full IPv6 address.
 *      (2) For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *          Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 *      (3) If conditions for (1)/(2) is not satisfied, each range check entry should compare independently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_rangeCheckIp_get)
 */
extern int32
rtk_pie_rangeCheckIp_get(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData);

/* Function Name:
 *      rtk_pie_rangeCheckIp_set
 * Description:
 *      Set the configuration of IP range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of IP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *          index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *          be used together in order to filter a full IPv6 address.
 *      (2) For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *          Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 *      (3) If conditions for (1)/(2) is not satisfied, each range check entry should compare independently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_rangeCheckIp_set)
 */
extern int32
rtk_pie_rangeCheckIp_set(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData);

/* Function Name:
 *      rtk_pie_rangeCheck_get
 * Description:
 *      Get the configuration of range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of range check
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Function Name:
 *      rtk_pie_rangeCheck_set
 * Description:
 *      Set the configuration of range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of range check
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
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
rtk_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Sub-module Name :
 *  Field Selector
 */

/* Function Name:
 *      rtk_pie_fieldSelector_get
 * Description:
 *      Get the configuration of field selector.
 * Input:
 *      unit   - unit id
 *      fs_idx - field selector index
 * Output:
 *      pFs    - configuration of field selector.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If offset is longer than the actual packet length or packet type is not matched, the value retrieved by
 *      field selector should be 0x0, also field selector valid mask(pie field type) should be 0x0.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_fieldSelector_get)
 */
extern int32
rtk_pie_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

/* Function Name:
 *      rtk_pie_fieldSelector_set
 * Description:
 *      Set the configuration of field selector.
 * Input:
 *      unit   - unit id
 *      fs_idx - field selector index
 *      pFs    - configuration of field selector.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If offset is longer than the actual packet length or packet type is not matched, the value retrieved by
 *      field selector should be 0x0, also field selector valid mask(pie field type) should be 0x0.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_fieldSelector_set)
 */
extern int32
rtk_pie_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

/* Function Name:
 *      rtk_pie_meterIncludeIfg_get
 * Description:
 *      Get IFG including status for packet length calculation of meter module.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
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
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterIncludeIfg_get)
 */
extern int32
rtk_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      rtk_pie_meterIncludeIfg_set
 * Description:
 *      Set IFG including status for packet length calculation of meter module.
 * Input:
 *      unit        - unit id
 *      ifg_include - enable status of includes IFG
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterIncludeIfg_get)
 */
extern int32
rtk_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      rtk_pie_meterExceedAggregation_get
 * Description:
 *      Get the meter exceed flag mask of meter entry exceed aggregated result every 16 entries.
 * Input:
 *      unit      - unit id
 * Output:
 *      pExceedMask - pointer to aggregated exceed flag mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterExceedAggregation_get)
 */
extern int32
rtk_pie_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask);

/* Function Name:
 *      rtk_pie_meterExceed_get
 * Description:
 *      Get the meter exceed flag of a meter entry.
 * Input:
 *      unit      - unit id
 *      meterIdx  - meter entry index
 * Output:
 *      pIsExceed - pointer to exceed flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterExceed_get)
 */
extern int32
rtk_pie_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed);

/* Function Name:
 *      rtk_pie_meterEntry_get
 * Description:
 *      Get the content of a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 * Output:
 *      pMeterEntry - pointer to a meter entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterEntry_get)
 */
extern int32
rtk_pie_meterEntry_get(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_pie_meterEntry_t    *pMeterEntry);

/* Function Name:
 *      rtk_pie_meterEntry_set
 * Description:
 *      Set a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 *      pMeterEntry - pointer to meter entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterEntry_set)
 */
extern int32
rtk_pie_meterEntry_set(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_pie_meterEntry_t    *pMeterEntry);

/* Function Name:
 *      rtk_pie_meterDpSel_get
 * Description:
 *      Get the configuration of DP select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pDpSel      - pointer to DP select
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_meterDpSel_get(
    uint32                  unit,
    rtk_pie_meterDpSel_t    *pDpSel);

/* Function Name:
 *      rtk_pie_meterDpSel_set
 * Description:
 *      Set the configuration of DP select.
 * Input:
 *      unit        - unit id
 *      dpSel       - DP select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_meterDpSel_set(
    uint32                  unit,
    rtk_pie_meterDpSel_t    dpSel);

/* Function Name:
 *      rtk_pie_arpMacSel_get
 * Description:
 *      Get the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pArpMacSel  - pointer to ARP MAC select
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel);

/* Function Name:
 *      rtk_pie_arpMacSel_set
 * Description:
 *      Set the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 *      arpMacSel   - ARP MAC select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel);

/* Function Name:
 *      rtk_pie_intfSel_get
 * Description:
 *      Get the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 * Output:
 *      intfSel    - select inteface filter type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_intfSel_get(uint32 unit, rtk_pie_intfSel_t *intfSel);

/* Function Name:
 *      rtk_pie_intfSel_set
 * Description:
 *      Set the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 *      intfSel    - select inteface filter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_intfSel_set(uint32 unit, rtk_pie_intfSel_t intfSel);

/* Function Name:
 *      rtk_pie_templateVlanFmtSel_get
 * Description:
 *      Get the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - template index
 * Output:
 *      pVlanFmtSel         - pointer to VLAN format select in template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The API onlye support ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel);

/* Function Name:
 *      rtk_pie_templateVlanFmtSel_set
 * Description:
 *      Set the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - template index
 *      vlanFmtSel          - VLAN format select in template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The API onlye support ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel);

/* Function Name:
 *      rtk_pie_meterTrtcmType_get
 * Description:
 *      Get a meter trTCM type.
 * Input:
 *      unit        - unit id
 * Output:
 *      type        - meter TrTCM type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_pie_meterTrtcmType_get(uint32 unit, rtk_pie_meterTrtcmType_t *type);

/* Function Name:
 *      rtk_pie_meterTrtcmType_set
 * Description:
 *      Set a meter trTCM type.
 * Input:
 *      unit        - unit id
 *      type        - meter TrTCM type
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_pie_meterTrtcmType_set(uint32 unit, rtk_pie_meterTrtcmType_t type);

/* Function Name:
 *      rtk_pie_filter1BR_get
 * Description:
 *      Get filter 802.1BR status.
 * Input:
 *      unit                - unit id
 * Output:
 *      en    - filter 802.1BR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_pie_filter1BR_get(uint32 unit, rtk_enable_t *en);

/* Function Name:
 *      rtk_pie_filter1BR_set
 * Description:
 *      Set filter 802.1BR status.
 * Input:
 *      unit  - unit id
 *      en    - filter 802.1BR status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_pie_filter1BR_set(uint32 unit, rtk_enable_t en);

#endif /* __RTK_PIE_H__ */

