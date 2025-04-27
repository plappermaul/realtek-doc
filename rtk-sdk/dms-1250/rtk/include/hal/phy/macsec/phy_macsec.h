/*
 * Copyright (C) 2022 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PHY MACSec Driver APIs.
 *
 * Feature : PHY MACSec Driver APIs
 *
 */
#ifndef __HAL_PHY_PHY_MACSEC_H__
#define __HAL_PHY_PHY_MACSEC_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <rtk/phy.h>

/*
 * Symbol Definition
 */
#define PHY_MACSEC_DEV_EGRESS   0
#define PHY_MACSEC_DEV_INGRESS  1

#define BIT_0_IN32   0x00000001U
#define BIT_1_IN32   0x00000002U
#define BIT_2_IN32   0x00000004U
#define BIT_3_IN32   0x00000008U
#define BIT_4_IN32   0x00000010U
#define BIT_5_IN32   0x00000020U
#define BIT_6_IN32   0x00000040U
#define BIT_7_IN32   0x00000080U
#define BIT_8_IN32   0x00000100U
#define BIT_9_IN32   0x00000200U
#define BIT_10_IN32  0x00000400U
#define BIT_11_IN32  0x00000800U
#define BIT_12_IN32  0x00001000U
#define BIT_13_IN32  0x00002000U
#define BIT_14_IN32  0x00004000U
#define BIT_15_IN32  0x00008000U
#define BIT_16_IN32  0x00010000U
#define BIT_17_IN32  0x00020000U
#define BIT_18_IN32  0x00040000U
#define BIT_19_IN32  0x00080000U
#define BIT_20_IN32  0x00100000U
#define BIT_21_IN32  0x00200000U
#define BIT_22_IN32  0x00400000U
#define BIT_23_IN32  0x00800000U
#define BIT_24_IN32  0x01000000U
#define BIT_25_IN32  0x02000000U
#define BIT_26_IN32  0x04000000U
#define BIT_27_IN32  0x08000000U
#define BIT_28_IN32  0x10000000U
#define BIT_29_IN32  0x20000000U
#define BIT_30_IN32  0x40000000U
#define BIT_31_IN32  0x80000000U

#define MASK_1_BITS     (BIT_1_IN32 - 1)
#define MASK_2_BITS     (BIT_2_IN32 - 1)
#define MASK_3_BITS     (BIT_3_IN32 - 1)
#define MASK_4_BITS     (BIT_4_IN32 - 1)
#define MASK_5_BITS     (BIT_5_IN32 - 1)
#define MASK_6_BITS     (BIT_6_IN32 - 1)
#define MASK_7_BITS     (BIT_7_IN32 - 1)
#define MASK_8_BITS     (BIT_8_IN32 - 1)
#define MASK_9_BITS     (BIT_9_IN32 - 1)
#define MASK_10_BITS    (BIT_10_IN32 - 1)
#define MASK_11_BITS    (BIT_11_IN32 - 1)
#define MASK_12_BITS    (BIT_12_IN32 - 1)
#define MASK_13_BITS    (BIT_13_IN32 - 1)
#define MASK_14_BITS    (BIT_14_IN32 - 1)
#define MASK_15_BITS    (BIT_15_IN32 - 1)
#define MASK_16_BITS    (BIT_16_IN32 - 1)
#define MASK_17_BITS    (BIT_17_IN32 - 1)
#define MASK_18_BITS    (BIT_18_IN32 - 1)
#define MASK_19_BITS    (BIT_19_IN32 - 1)
#define MASK_20_BITS    (BIT_20_IN32 - 1)
#define MASK_21_BITS    (BIT_21_IN32 - 1)
#define MASK_22_BITS    (BIT_22_IN32 - 1)
#define MASK_23_BITS    (BIT_23_IN32 - 1)
#define MASK_24_BITS    (BIT_24_IN32 - 1)
#define MASK_25_BITS    (BIT_25_IN32 - 1)
#define MASK_26_BITS    (BIT_26_IN32 - 1)
#define MASK_27_BITS    (BIT_27_IN32 - 1)
#define MASK_28_BITS    (BIT_28_IN32 - 1)
#define MASK_29_BITS    (BIT_29_IN32 - 1)
#define MASK_30_BITS    (BIT_30_IN32 - 1)
#define MASK_31_BITS    (BIT_31_IN32 - 1)


/*
 * Data Declaration
 */
#define MACSEC_REG_OFFS               4

#define PHY_MACSEC_HW_SA_ID(sc_id, an)       (sc_id * 4 + an)
#define PHY_MACSEC_HW_FLOW_ID(sc_id)         (sc_id * 4)
#define PHY_MACSEC_HW_SA_TO_AN(sa_id)        (sa_id % 4)

#define PHY_MACSEC_MAX_SA_EN_SIZE     24 //32-bit words
#define PHY_MACSEC_MAX_SA_IN_SIZE     20
#define PHY_MACSEC_MAX_SA_SIZE        PHY_MACSEC_MAX_SA_EN_SIZE

#define MACSEC_XFORM_REC_BASE         (0x0000)
#define MACSEC_XFORM_REC_SIZE(dir)   ((RTK_MACSEC_DIR_EGRESS == dir) ? PHY_MACSEC_MAX_SA_EN_SIZE : PHY_MACSEC_MAX_SA_IN_SIZE )

#define MACSEC_REG_XFORM_REC(n, dir)        (MACSEC_XFORM_REC_BASE + MACSEC_XFORM_REC_SIZE(dir) * \
                                             MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_XFORM_REC_OFFS(n, dir, off) (MACSEC_REG_XFORM_REC(n, dir) + off * MACSEC_REG_OFFS)


#define MACSEC_REG_SAM_MAC_SA_MATCH_LO(n) (0x4000 + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_MAC_SA_MATCH_HI(n) (0x4004 + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_MAC_DA_MATCH_LO(n) (0x4008 + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_MAC_DA_MATCH_HI(n) (0x400C + 16 * MACSEC_REG_OFFS * (n % 128))

#define MACSEC_REG_SAM_MISC_MATCH(n)      (0x4010 + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_SCI_MATCH_LO(n)    (0x4014 + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_SCI_MATCH_HI(n)    (0x4018 + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_MASK(n)            (0x401C + 16 * MACSEC_REG_OFFS * (n % 128))
#define MACSEC_REG_SAM_EXT_MATCH(n)       (0x4020 + 16 * MACSEC_REG_OFFS * (n % 128))

#define MACSEC_REG_SAM_ENTRY_ENABLE(n)    (0x6000 + (n * MACSEC_REG_OFFS))
#define MACSEC_REG_SAM_ENTRY_TOGGLE(n)    (0x6040 + (n * MACSEC_REG_OFFS))
#define MACSEC_REG_SAM_ENTRY_SET(n)       (0x6080 + (n * MACSEC_REG_OFFS))
#define MACSEC_REG_SAM_ENTRY_CLEAR(n)     (0x60C0 + (n * MACSEC_REG_OFFS))
#define MACSEC_REG_SAM_ENTRY_ENABLE_CTRL  (0x6100)
#define MACSEC_REG_SAM_IN_FLIGHT          (0x6104)

#define MACSEC_REG_SAM_FLOW_CTRL(n)   (0x7000 + MACSEC_REG_OFFS * (n % 128))


#define MACSEC_REG_SAM_IN_FLIGHT      (0x6104)
#define MACSEC_REG_COUNT_CTRL         (0xC810)
#define MACSEC_REG_IG_CC_CONTROL      (0xE840)
#define MACSEC_REG_COUNT_SECFAIL1     (0xF124)
#define MACSEC_REG_CTX_CTRL           (0xF408)
#define MACSEC_REG_CTX_UPD_CTRL       (0xF430)
#define MACSEC_REG_SAM_CP_TAG         (0x7900)
#define MACSEC_REG_SAM_NM_PARAMS      (0x7940)
#define MACSEC_REG_SAM_NM_FLOW_NCP        (0x7944)
#define MACSEC_REG_SAM_NM_FLOW_CP         (0x7948)

#define MACSEC_REG_MISC_CONTROL       (0x797C)

// Mask last byte received of MAC source address
#define MACSEC_SA_MATCH_MASK_MAC_SA_0         BIT_0_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_1         BIT_1_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_2         BIT_2_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_3         BIT_3_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_4         BIT_4_IN32
// Mask first byte received of MAC source address
#define MACSEC_SA_MATCH_MASK_MAC_SA_5         BIT_5_IN32

#define MACSEC_SA_MATCH_MASK_MAC_SA_FULL      (BIT_0_IN32 | BIT_1_IN32 | BIT_2_IN32 | BIT_3_IN32 | BIT_4_IN32 | BIT_5_IN32)

// Mask last byte received of MAC destination address
#define MACSEC_SA_MATCH_MASK_MAC_DA_0         BIT_6_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_1         BIT_7_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_2         BIT_8_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_3         BIT_9_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_4         BIT_10_IN32
// Mask first byte received of MAC destination address
#define MACSEC_SA_MATCH_MASK_MAC_DA_5         BIT_11_IN32

#define MACSEC_SA_MATCH_MASK_MAC_DA_FULL      (BIT_6_IN32 | BIT_7_IN32 | BIT_8_IN32 | BIT_9_IN32 | BIT_10_IN32 | BIT_11_IN32)

#define MACSEC_SA_MATCH_MASK_MAC_ETYPE        BIT_12_IN32
#define MACSEC_SA_MATCH_MASK_VLAN_VALID       BIT_13_IN32
#define MACSEC_SA_MATCH_MASK_QINQ_FOUND       BIT_14_IN32
#define MACSEC_SA_MATCH_MASK_STAG_VALID       BIT_15_IN32
#define MACSEC_SA_MATCH_MASK_QTAG_VALID       BIT_16_IN32
#define MACSEC_SA_MATCH_MASK_VLAN_UP          BIT_17_IN32
#define MACSEC_SA_MATCH_MASK_VLAN_ID          BIT_18_IN32
#define MACSEC_SA_MATCH_MASK_SRC_PORT         BIT_19_IN32
#define MACSEC_SA_MATCH_MASK_CTRL_PKT         BIT_20_IN32

// For ingress only
#define MACSEC_SA_MATCH_MASK_MACSEC_SCI       BIT_23_IN32
#define MACSEC_SA_MATCH_MASK_MACSEC_TCI_AN_SC (BIT_24_IN32 | BIT_25_IN32 | BIT_29_IN32)

#define MACSEC_SAB_CW0_MACSEC_EG32 0x9241e066
#define MACSEC_SAB_CW0_MACSEC_IG32 0xd241e06f
#define MACSEC_SAB_CW0_MACSEC_EG64 0xa241e066
#define MACSEC_SAB_CW0_MACSEC_IG64 0xe241a0ef
#define MACSEC_SAB_CW0_AES128      0x000a0000
#define MACSEC_SAB_CW0_AES256      0x000e0000


#define RTK_MACSEC_PORT_COMMON          0
#define RTK_MACSEC_PORT_RESERVED        1
#define RTK_MACSEC_PORT_CONTROLLED      2
#define RTK_MACSEC_PORT_UNCONTROLLED    3


typedef struct phy_macsec_sa_info_s
{
    uint8   ssci[4];
} phy_macsec_sa_info_t;

#define MACSEC_SA_IS_USED(unit, port, dir, sa_id)     BITMAP_IS_SET(phy_macsec_info[unit]->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_IS_USED(unit, port, dir, sc_id)     BITMAP_IS_SET(phy_macsec_info[unit]->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_IS_CLEAR(unit, port, dir, sa_id)    BITMAP_IS_CLEAR(phy_macsec_info[unit]->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id)    BITMAP_IS_CLEAR(phy_macsec_info[unit]->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_SET_USED(unit, port, dir, sa_id)    BITMAP_SET(phy_macsec_info[unit]->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_SET_USED(unit, port, dir, sc_id)    BITMAP_SET(phy_macsec_info[unit]->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_UNSET_USED(unit, port, dir, sa_id)  BITMAP_CLEAR(phy_macsec_info[unit]->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_UNSET_USED(unit, port, dir, sc_id)  BITMAP_CLEAR(phy_macsec_info[unit]->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_MAX(unit, port)  phy_macsec_info[unit]->port[port]->max_sa_num
#define MACSEC_SC_MAX(unit, port)  (phy_macsec_info[unit]->port[port]->max_sa_num/4)
#define MACSEC_SC_CS(unit, port, dir, sc_id)   phy_macsec_info[unit]->port[port]->cipher_suite[dir][sc_id]
#define MACSEC_SC_MATCH(unit, port, dir, sc_id)   phy_macsec_info[unit]->port[port]->flow_match[dir][sc_id]

#define MACSEC_SA_SSCI(unit, port, sa_id) phy_macsec_info[unit]->port[port]->sa_info[sa_id].ssci

typedef struct phy_macsec_port_info_s
{
    uint16               sa_gen_seq;
    uint32               max_sa_num;
    rtk_macsec_cipher_t  cipher_suite[RTK_MACSEC_DIR_END][RTK_MAX_MACSEC_SC_PER_PORT];
    uint32               flow_match[RTK_MACSEC_DIR_END][RTK_MAX_MACSEC_SC_PER_PORT];
    rtk_bitmap_t         sc_used[RTK_MACSEC_DIR_END][BITMAP_ARRAY_CNT(RTK_MAX_MACSEC_SC_PER_PORT)];
    rtk_bitmap_t         sa_used[RTK_MACSEC_DIR_END][BITMAP_ARRAY_CNT(RTK_MAX_MACSEC_SA_PER_PORT)];
    phy_macsec_sa_info_t sa_info[RTK_MAX_MACSEC_SA_PER_PORT];
} phy_macsec_port_info_t;

typedef struct phy_macsec_info_s
{
    phy_macsec_port_info_t *port[RTK_MAX_PORT_PER_UNIT];
} phy_macsec_info_t;

typedef struct phy_macsec_flow_action_e_s
{
    // 1 - enable frame protection,
    // 0 - bypass frame through device
    uint8 protect_frame;

    // 1 - SA is in use, packets classified for it can be transformed
    // 0 - SA not in use, packets classified for it can not be transformed
    uint8 sa_in_use;

    // 1 - inserts explicit SCI in the packet,
    // 0 - use implicit SCI (not transferred)
    uint8 include_sci;

    // 1 - enable ES bit in the generated SecTAG
    // 0 - disable ES bit in the generated SecTAG
    uint8 use_es;

    // 1 - enable SCB bit in the generated SecTAG
    // 0 - disable SCB bit in the generated SecTAG
    uint8 use_scb;

    // Number of VLAN tags to bypass for egress processing.
    // Valid values: 0, 1 and 2.
    // This feature is only available on HW4.1 and possibly later versions.
    uint8 tag_bypass_size;

    // 1 - Does not update sa_in_use flag
    // 0 - Update sa_in_use flag
    uint8 sa_index_update_by_hw;

    // The number of bytes (in the range of 0-127) that are authenticated but
    // not encrypted following the SecTAG in the encrypted packet. Values
    // 65-127 are reserved in HW < 4.0 and should not be used there.
    uint8 confidentiality_offset;

    // 1 - enable confidentiality protection
    // 0 - disable confidentiality protection
    uint8 conf_protect;

} phy_macsec_flow_action_e_t;

typedef struct phy_macsec_flow_action_i_s
{
    // 1 - enable replay protection
    // 0 - disable replay protection
    uint8 replay_protect;

    // true - SA is in use, packets classified for it can be transformed
    // false - SA not in use, packets classified for it can not be transformed
    uint8 sa_in_use;

    // MACsec frame validation level
    rtk_macsec_validate_t validate_frames;

    // The number of bytes (in the range of 0-127) that are authenticated but
    // not encrypted following the SecTAG in the encrypted packet.
    uint8 confidentiality_offset;

} phy_macsec_flow_action_i_t;

typedef struct phy_macsec_flow_action_bd_s
{
    // 1 - enable statistics counting for the associated SA
    // 0 - disable statistics counting for the associated SA
    uint8 sa_in_use;
} phy_macsec_flow_action_bd_t;

typedef struct phy_macsec_flow_action_s
{
    uint32 sa_index;

    rtk_macsec_flow_type_t flow_type;
    union
    {
        phy_macsec_flow_action_e_t egress;
        phy_macsec_flow_action_i_t ingress;
        phy_macsec_flow_action_bd_t bypass_drop;
    } params;

    uint8 dest_port;

} phy_macsec_flow_action_t;



typedef struct
{
    // index for flow control rule entry
    uint32 flow_index;

    // Packet field values to match

    // MAC source address
    uint8 mac_sa[6];

    // MAC destination address
    uint8 mac_da[6];

    // EtherType
    uint16 etherType;

    // SCI, for ingress only
    uint8 sci[8];

    // Parsed VLAN ID compare value
    uint16 vlan_id;

    // Parsed VLAN valid flag compare value
    uint8 fVLANValid;

    // Parsed QinQ found flag compare value
    uint8 fQinQFound;

    // Parsed STAG valid flag compare value
    uint8 fSTagValid;

    // Parsed QTAG valid flag compare value
    uint8 fQTagFound;

    // Parsed VLAN User Priority compare value
    uint8 vlanUserPriority;

    // Packet is a control packet (as pre-decoded) compare value
    uint8 fControlPacket;

    // true - allow packets without a MACsec tag to match
    uint8 fUntagged;

    // true - allow packets with a standard and valid MACsec tag to match
    uint8 fTagged;

    // true - allow packets with an invalid MACsec tag to match
    uint8 fBadTag;

    // true - allow packets with a MACsec tag indicating KaY handling
    // to be done to match
    uint8 fKayTag;

    // Source port compare value
    uint8 sourcePort;

    // Priority of this entry for determining the actual transform used
    // on a match when multiple entries match, 0 = lowest, 15 = highest.
    // In case of identical priorities, the lowest numbered entry takes
    // precedence.
    uint8 matchPriority;

    // MACsec TCI/AN byte compare value, bits are individually masked for
    // comparing. The TCI bits are in bits [7:2] while the AN bits reside
    // in bits [1:0]. the macsec_TCI_AN field should only be set to
    // non-zero for an actual MACsec packet.
    uint8 macsec_TCI_AN;

    // Match mask for the SA flow rules, see MACSEC_SA_MATCH_MASK_*
    uint32 matchMask;

    // Parsed inner VLAN ID compare value
    uint16 vlanIdInner;

    // Parsed inner VLAN UP compare value
    uint8 vlanUpInner;

} phy_macsec_flow_match_t;

typedef struct {
    uint8 key_offs;
    uint8 hkey_offs;
    uint8 seq_offs;
    uint8 mask_offs;
    uint8 ctx_salt_offs;
    uint8 iv_offs;
    uint8 upd_ctrl_offs;
} phy_macsec_sa_offset_t;

#define RTK_PHY_MACSEC_SA_FLAG_XPN    0x00000001U    //Extended Packet Numbering

typedef struct
{
    uint32 context_id;  //keep 0 for create
    rtk_macsec_dir_t direction;
    uint32 flow_index; //the flow entry index apply to this SA
    uint32 flags;  // a bitmap of RTK_PHY_MACSEC_SA_FLAG_*

    uint8 an;      // 2-bit AN inserted in SecTAG (egress).
    uint8 sci[8];  // 8-byte SCI.([0:5] = MAC address, [6:7] = port index)

    uint8 key[RTK_MACSEC_MAX_KEY_LEN];  // MACsec Key.
    uint32 key_bytes; // Size of the MACsec key in bytes (16 for AES128, 32 for AES256).

    uint8 salt[12];  // 12-byte salt (64-bit sequence numbers).
    uint8 ssci[4];  // 4-byte SSCI value (64-bit sequence numbers).

    uint32 seq; // sequence number.
    uint32 seq_h; // High part of sequence number (64-bit sequence numbers)
    uint32 replay_window; // Size of the replay window, 0 for strict ordering (ingress).

    /* update ctrl */
    uint32 next_sa_index; // SA index of the next chained SA (egress).
    uint8 sa_expired_irq; // 1 if SA expired IRQ is to be generated.
    uint8 next_sa_valid; // SA Index field is a valid SA.
    uint8 update_en;  // Set to true if the SA must be updated.
} phy_macsec_sa_params_t;


typedef void (*phy_macsec_aes_cb)(
        const uint8 * const In_p,
        uint8 * const Out_p,
        const uint8 * const Key_p,
        const unsigned int KeyByteCount);

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_macsec_init
 * Description:
 *      PHY MACSec driver initialize
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_macsec_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_macsec_port_cfg_set
 * Description:
 *      Set per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      pPortcfg - pointer to macsec port configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_port_cfg_set(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      phy_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_port_cfg_get(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      phy_macsec_sc_create
 * Description:
 *      Create a MACsec Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      pSc      - pointer to macsec sc configuration structure
 * Output:
 *      pSc_id   - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sc_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    rtk_macsec_sc_t *pSc, uint32 *pSc_id);

/* Function Name:
 *      phy_macsec_sc_get
 * Description:
 *      Get configuration info for a created Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_index - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sc_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_t *pSc);

/* Function Name:
 *      phy_macsec_sc_del
 * Description:
 *      Delete a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sc_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id);

/* Function Name:
 *      phy_macsec_sc_status_get
 * Description:
 *      Get hardware status for a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_status - pointer to macsec SC status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sc_status_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_status_t *pSc_status);

/* Function Name:
 *      phy_macsec_sa_create
 * Description:
 *      Create a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 *      pSa      - pointer to macsec SA configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sa_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);

/* Function Name:
 *      phy_macsec_sa_get
 * Description:
 *      Get configuration info for a Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      pSa      - pointer to macsec SA configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sa_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);

/* Function Name:
 *      phy_macsec_sa_del
 * Description:
 *      Delete a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_sa_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      phy_macsec_sa_activate
 * Description:
 *      Activate a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - Secure Channel id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      For egress, this function will change running SA.
 */
extern int32
phy_macsec_sa_activate(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      phy_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association (inUse = 0)
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_rxsa_disable(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an);

/* Function Name:
 *      phy_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association (inUse = 0)
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_txsa_disable(uint32 unit, rtk_port_t port, uint32 txsc_id);

/* Function Name:
 *      phy_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_stat_clear(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_macsec_stat_port_get
 * Description:
 *      get per-port statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern int32
phy_macsec_stat_port_get(uint32 unit, rtk_port_t port, rtk_macsec_stat_t stat,
    uint64 *pCnt);

/* Function Name:
 *      phy_macsec_stat_txsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern int32
phy_macsec_stat_txsa_get(uint32 unit, rtk_port_t port, uint32 txsc_id,
    rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, uint64 *pCnt);

/* Function Name:
 *      phy_macsec_stat_rxsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern int32
phy_macsec_stat_rxsa_get(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, uint64 *pCnt);

/* Function Name:
 *      phy_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_macsec_intr_status_get(uint32 unit, rtk_port_t port,
    rtk_macsec_intr_status_t *pIntr_status);

#endif /* __HAL_PHY_PHY_MACSEC_H__ */

