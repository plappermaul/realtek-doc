

#ifndef __AAL_FDB_H__
#define __AAL_FDB_H__

#include "ca_types.h"
#include "aal_port.h"
#include "aal_arb.h"

extern ca_int32_t aal_fdb_debug;
#define L2_ASSERT_RET(x/*CODEGEN_IGNORE_TAG*/,y)  do{ if (!(x))  { ca_printf("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__); return (y); } }while(0)

#define L2_DEBUG(arg/*CODEGEN_IGNORE_TAG*/ ,...) if (aal_fdb_debug) ca_printf("%s:%d "arg, __FUNCTION__,__LINE__,##__VA_ARGS__)
//#define cvt_mac(x) x[0], x[1], x[2], x[3], x[4], x[5]

#define FDB_REG_WRITE(reg/*CODEGEN_IGNORE_TAG*/, data)    CA_NE_REG_WRITE((data), (reg))

/* FDB tables indirect access timeout value */
#define CA_AAL_FDB_TBL_OP_TIMEOUT       0xFFF

#define CA_AAL_TBL_FDB_NAME/*CODEGEN_IGNORE_TAG*/              "l2fdb"
#define CA_AAL_TBL_FDB_SVL_NAME/*CODEGEN_IGNORE_TAG*/          "l2fdb_svl"
#define CA_AAL_TBL_MAC_FILTER_NAME/*CODEGEN_IGNORE_TAG*/       "l2mac_filter"

#define CA_AAL_FDB_MUTEX_NAME/*CODEGEN_IGNORE_TAG*/            "fdb_mutex"

#define CA_AAL_FDB_ETH_PORT_NUM/*CODEGEN_IGNORE_TAG*/          8 /* CA_PORT_MAX_NUM_ETHERNET */

#define CA_AAL_FDB_PORT_CHECK(port_id)/*CODEGEN_IGNORE_TAG*/ if ((port_id) < CA_PORT_ID_NI0 || (port_id) > CA_PORT_ID_NI7){ return CA_E_PARAM; }


#define CA_AAL_FDB_MAC_FILTER_DFT_ID/*CODEGEN_IGNORE_TAG*/   (AAL_L2_CLS_MAC_FILTER_ENTRY_NUMBER * 2)
#define CA_AAL_FDB_MAC_FILTER_RULE/*CODEGEN_IGNORE_TAG*/    (CA_AAL_FDB_MAC_FILTER_DFT_ID - 2)        /* base 1, total 6 items */

#define CA_AAL_RULE_ID_PARTNER(rule_id)/*CODEGEN_IGNORE_TAG*/     (((rule_id)%2 ? (rule_id) + 1 : (rule_id) - 1))  /* rule_id 1 base */

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
#define CA_AAL_PORT_OFFSET                                        3 
#define CA_AAL_CLS_ENTRY_ID_START(port_id)/*CODEGEN_IGNORE_TAG*/  ((port_id - CA_AAL_PORT_OFFSET)*__INGRESS_CLS_ENTRY_NUM_PER_PORT)
#else
#define CA_AAL_CLS_ENTRY_ID_START(port_id)/*CODEGEN_IGNORE_TAG*/  ((port_id)*__INGRESS_CLS_ENTRY_NUM_PER_PORT)
#endif
#define CLS_ENTRY_ID(port_id/*CODEGEN_IGNORE_TAG*/, rule_id)      (CA_AAL_CLS_ENTRY_ID_START(port_id) + (AAL_L2_CLS_RESERVE_ENTRY_NUMBER + (((rule_id) - 1)/2)))  /* 6 ~ 9 --> 0 ~ 7 */
#define CLS_IDX_IN_ENTRY(rule_id)/*CODEGEN_IGNORE_TAG*/           (((rule_id) - 1)%2)

#define CA_AAL_FDB_DEF_AGING_OUT_TIME   300
#define CA_AAL_FDB_DEF_AGING_START_VAL  255

#if 0//yocto
#else//sd1 uboot for 98f mp test
#define CA_AAL_FDB_UNK_VLAN_NOT_LEARN   0
#define CA_AAL_FDB_UNK_VLAN_BE_LEARN    1
#endif//sd1 uboot for 98f mp test

#define FDB_AGING_TIME(x)/*CODEGEN_IGNORE_TAG*/   (x)//(l2_aging_time * (x) / CA_AAL_FDB_DEF_AGING_START_VAL)

#define CA_AAL_FDB_LRN_MODE_SW          3   /* sw learn and enable the learn confirmation */
#define CA_AAL_FDB_LRN_MODE_HW          1   /* hw learn and enable the learn confirmation */

/* Defines of the L2 Hash methods */
#define CA_AAL_FDB_HASH_METHOD_XOR      0
#define CA_AAL_FDB_HASH_METHOD_CRC32    1

/* Defines of FDB table access result */
#define CA_AAL_FDB_SCAN_RETURN_LAST     0x800000
#define CA_AAL_FDB_SCAN_RETURN_END      0x800001

#define CA_AAL_L2E_CMD_MODE             1
#define CA_AAL_L2E_INDIRECT_MODE        0


/* hash read result */
#define CA_AAL_FDB_ACCESS_HIT           0x5  /* hash read/append/delete, scan, flush */
#define CA_AAL_FDB_ACCESS_MISS          0xf  /* hash read/delete, scan, flush */
#define CA_AAL_FDB_ACCESS_MAC_LIMIT     0xb  /* entry updated; However port MAC limit check failure detected */

/* hash append result */
#define CA_AAL_FDB_APPEND_SUCCESS       0x5
#define CA_AAL_FDB_APPEND_FAIL_FULL     0xa
#define CA_AAL_FDB_APPEND_FAIL_MAC_LMT  0xb
#if 0
/* hash delete result */
#define CA_AAL_FDB_DELETE_SUCCESS       0x5
#define CA_AAL_FDB_DELETE_MISS          0xf


/* scan result */
#define CA_AAL_FDB_SCAN_HIT             0x5
#define CA_AAL_FDB_SCAN_MISS            0xf

/* flush result */
#define CA_AAL_FDB_FLUSH_HIT            0x5
#define CA_AAL_FDB_FLUSH_MISS           0xf
#endif

#define CA_AAL_L2E_LMT_ACCESS           0x40 /* Address [6]: '1', lmt access; '0', cnt access */

#define CA_AAL_L2E_CNT_INC              0x100
#define CA_AAL_L2E_CNT_DEC              0x200

#define CA_AAL_L3E_CNT_LMT_SUCCESS      1
#define CA_AAL_L3E_CNT_LMT_FAIL         0

#define CA_AAL_MAX_MAC_AGING_TIME       500000

#define AAL_ILPB_UNKWN_POL_NUM          8
#define AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM  15
#define CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE  15

#define INVALID_MC_GROUP_ID  0xFFFFFFFF

/* operation codes of the FDB command table */
typedef enum {
    __FDB_TBL_OPCODE_INIT     = 0,
    __FDB_TBL_OPCODE_SCAN     = 1,
    __FDB_TBL_OPCODE_READ     = 0x4,
    __FDB_TBL_OPCODE_FLUSH    = 0x42,
    __FDB_TBL_OPCODE_FLUSH_OLDEST  = 0x62,      /* only remove the oldest entry, bit5=1 */
    __FDB_TBL_OPCODE_APPEND   = 0x45,
    __FDB_TBL_OPCODE_APPEND_REPLACE   = 0x65,   /* replace oldest entry if hash entry full, bit5=1 */
    __FDB_TBL_OPCODE_DELETE   = 0x85,


    __FDB_TBL_OPCODE_RESV0    = 0x106,

    __FDB_TBL_OPCODE_CNT_INIT = 0x107,
    __FDB_TBL_OPCODE_CNT_INC  = 0x108,
    __FDB_TBL_OPCODE_CNT_DEC  = 0x109,

    __FDB_TBL_OPCODE_CNT_WR   = 0x10A,
    __FDB_TBL_OPCODE_CNT_RD   = 0x10B,

    __FDB_TBL_OPCODE_LMT_WR   = 0x10C,
    __FDB_TBL_OPCODE_LMT_RD   = 0x10D,
    __FDB_TBL_OPCODE_LMT_INIT = 0x10E,
    __FDB_TBL_OPCODE_LMT_INC  = 0x10F,
    __FDB_TBL_OPCODE_LMT_DEC  = 0x110,

} __fdb_tbl_opcode_t;

/* FDB Scan modes */
typedef enum {
    CA_AAL_FDB_SCAN_MODE_NORMAL,
    CA_AAL_FDB_SCAN_MODE_TMP,
    CA_AAL_FDB_SCAN_MODE_STATIC,
    CA_AAL_FDB_SCAN_MODE_DYNAMIC,
    CA_AAL_FDB_SCAN_MODE_LPID,
    CA_AAL_FDB_SCAN_MODE_MAX
} cs_aal_fdb_scan_mode_t;

/* FDB Scan rule */
typedef struct {
    cs_aal_fdb_scan_mode_t  mode;
    ca_uint16_t               param;
} cs_aal_fdb_scan_rule_t;

typedef struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t resv0             : 12,
              start_address     : 12,
              dynamic_tmp       : 1,
              dynamic_formal    : 1,
              static_flag       : 1,
              in_st_mv_flag     : 1,
              in_refresh_flag   : 1,
              op                : 3;
#else/* CS_LITTLE_ENDIAN */
    ca_uint32_t op                : 3,
              in_refresh_flag   : 1,
              in_st_mv_flag     : 1,
              static_flag       : 1,
              dynamic_formal    : 1,
              dynamic_tmp       : 1,
              start_address     : 12,
              resv0             : 12;
#endif
} __l2fe_l2e_fdb_scan_t;

typedef struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t resv1           : 15,
              ret_address       : 13,
              status            : 4;
#else/* CS_LITTLE_ENDIAN */
    ca_uint32_t status          : 4,
              ret_address       : 13,
              resv1             : 15;
#endif
} __l2fe_l2e_fdb_scan_return_t;


typedef struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t resv0             : 12,
              start_address     : 12,
              update_target     : 2,
              resv1             : 3,
              op                : 3;
#else/* CS_LITTLE_ENDIAN */
    ca_uint32_t op                : 3,
              resv1             : 3,
              update_target     : 2,
              start_address     : 12,
              resv0             : 12;
#endif
} __l2fe_l2e_fdb_update_t;

/* software learn update tmp to formal */
typedef struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t resv1             : 10,
              port_id           : 6,
              ret_address       : 12,
              resv0             : 4;
#else/* CS_LITTLE_ENDIAN */
    ca_uint32_t resv0             : 4,
              ret_address       : 12,
              port_id           : 6,
              resv1             : 10;
#endif
} __l2fe_l2e_fdb_scan_return_2_t;


/* FDB entry structure */
typedef struct {
    ca_uint8_t    valid;              /* Entry valid flag */

    ca_uint8_t    learn_flag;         /* VLAN learning flag. 0: IVL, 1: SVL (VID should be zero) */
    ca_uint8_t    vid_flag;           /* VLAN ID flag */

    ca_uint8_t    permit_sa;          /* Drop or permit the packet when SA hits this entry
                                       0: DROP, 1: PERMIT */
    ca_uint8_t    permit_da;          /* Drop or permit the packet when DA hits this entry
                                       0: DROP, 1: PERMIT */

    ca_uint8_t    mc_flag;            /* Multicast flag */
    ca_uint8_t    static_flag;
    ca_uint8_t    port_flag;          /* Port flag when mc_flag is ZERO
                                       (not used, when mc_flag is 1)
                                       0x0 Destination port is physical port ID
                                       0x1 Destination port is subscriber ID */

    ca_uint16_t   aging;              /* Aging status for dynamic entries */
    ca_uint8_t    ieee8021p;
    ca_uint8_t    rsvd1;
    ca_uint16_t   vid;
    ca_mac_addr_t    mac;
    ca_uint8_t    rsvd2;

    /* rzhou, t4 not used any more */
    ca_uint8_t    zone;               /* Logical FDB zone in isolation mode (the whole FDB will be
                                       divided into four zones and each will have 4K MAC entries
                                       under isolation mode) */
    ca_uint16_t   sub_id;
    ca_uint8_t    port_id;            /* Versatile destination port index:
                                       When mc_flag is 1
                                       0x000~0x3ff Multicast FIB ID(refer to olt_mcast_fib_set())
                                       When mc_flag is 0 and port_flag is 0
                                       0x000~0x00f Physical port ID
                                       When mc_flag is 0 and port_flag is 1
                                       0x000~0x1ff Subscriber ID */

    ca_uint16_t   lrnt_vid;           /* Learned VLAN ID */
    ca_uint16_t   lrnt_vid_flag;      /* Learned VLAN ID Flag, 0: IVL, 1: SVL */

    ca_uint8_t    aged;               /* Aged flag, 0: not aged, 1: aged */

    ca_uint8_t    tmp_flag;           /* Temp entry flag, used for multiple core FDB sync, 0: normal, 1: temp entry */
    ca_uint8_t    share_vlan_flag;    /* Shared VLAN flag, used for multiple core FDB sync, 0: not shared VLAN, 1: shared VLAN */

    ca_uint8_t    rsvd5;
} cs_aal_fdb_entry_t;

typedef struct {
    ca_boolean_t  key_sc_ind;
    ca_boolean_t  mc_flag;
    ca_boolean_t  sa_permit;
    ca_boolean_t  da_permit;
    ca_boolean_t  static_flag;
    ca_boolean_t  in_refresh_flag;
    ca_boolean_t  in_st_mv_flag;
    ca_boolean_t  tmp_flag;

    ca_uint16_t   key_vid_high;
    ca_uint16_t   key_vid_low;

    ca_uint16_t   lrned_vid_high;
    ca_uint16_t   lrned_vid_low;

    ca_uint8_t    dot1p_high;
    ca_uint8_t    dot1p_low;

    ca_uint8_t    aging_status_high;
    ca_uint8_t    aging_status_low;

    ca_uint16_t   mcgid_high;
    ca_uint16_t   mcgid_low;

    ca_uint8_t    lpid_high;
    ca_uint8_t    lpid_low;

} aal_fdb_flush_cfg_t;

/* Scan/Flush matching configuration table */
typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t        rsvd                                    : 20;
        ca_uint32_t        sa_permit                               : 1;
        ca_uint32_t        da_permit                               : 1;
        ca_uint32_t        static_flag                             : 1;
        ca_uint32_t        in_refresh_flag                         : 1;
        ca_uint32_t        in_st_mv_flag                           : 1;
        ca_uint32_t        tmp_flag                                : 1;
        ca_uint32_t        key_dot1p                               : 1;
        ca_uint32_t        key_vid                                 : 1;
        ca_uint32_t        lrned_vid                               : 1;
        ca_uint32_t        aging_status                            : 1;
        ca_uint32_t        mcgid                                   : 1;
        ca_uint32_t        lpid                                    : 1;
#else
        ca_uint32_t        lpid                                    : 1;
        ca_uint32_t        mcgid                                   : 1;
        ca_uint32_t        aging_status                            : 1;
        ca_uint32_t        lrned_vid                               : 1;
        ca_uint32_t        key_vid                                 : 1;
        ca_uint32_t        key_dot1p                               : 1;
        ca_uint32_t        tmp_flag                                : 1;
        ca_uint32_t        in_st_mv_flag                           : 1;
        ca_uint32_t        in_refresh_flag                         : 1;
        ca_uint32_t        static_flag                             : 1;
        ca_uint32_t        da_permit                               : 1;
        ca_uint32_t        sa_permit                               : 1;
        ca_uint32_t        mc_flag                                 : 1;
        ca_uint32_t        key_sc_ind                              : 1;
        ca_uint32_t        invalid_entry                           : 1;
        ca_uint32_t        rsvd                                    : 17;
#endif
    } bf;
    ca_uint32_t     wrd ;
} aal_fdb_cpu_cmd_word4_msk_t;
typedef volatile union {
    struct {
#   ifdef CS_BIG_ENDIAN
        ca_uint32_t rsvd                  : 2;
        ca_uint32_t sa_permit             : 1;
        ca_uint32_t da_permit             : 1;
        ca_uint32_t static_flag           : 1;
        ca_uint32_t in_refresh_flag       : 1;
        ca_uint32_t in_st_mv_flag         : 1;
        ca_uint32_t tmp_flag              : 1;
        ca_uint32_t key_vid_high          : 12;
        ca_uint32_t lrned_vid_high        : 12;
#   else /* CS_LITTLE_ENDIAN */
        ca_uint32_t lrned_vid_high        : 12;
        ca_uint32_t key_vid_high          : 12;
        ca_uint32_t tmp_flag              : 1;
        ca_uint32_t in_st_mv_flag         : 1;
        ca_uint32_t in_refresh_flag       : 1;
        ca_uint32_t static_flag           : 1;
        ca_uint32_t da_permit             : 1;
        ca_uint32_t sa_permit             : 1;
        ca_uint32_t mc_flag               : 1;
        ca_uint32_t key_sc_ind            : 1;
#   endif
    } bf ;
    ca_uint32_t     wrd ;
} aal_fdb_cpu_cmd_word3_t;

typedef volatile union {
    struct {
#   ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd1               :  8 ;
        ca_uint32_t key_vid_low          : 12 ;
        ca_uint32_t lrned_vid_low        : 12 ;
#   else /* CS_LITTLE_ENDIAN */
        ca_uint32_t lrned_vid_low        : 12 ;
        ca_uint32_t key_vid_low          : 12 ;
        ca_uint32_t rsrvd1               :  8 ;
#   endif
    } bf ;
    ca_uint32_t     wrd ;
} aal_fdb_cpu_cmd_word2_t;

typedef volatile union {
    struct {
#   ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd1                : 5;
        ca_uint32_t key_dot1p_high        : 3;
        ca_uint32_t aging_status_high     : 8;
        ca_uint32_t mcgid_high            : 10;
        ca_uint32_t lpid_high             : 6;
#   else /* CS_LITTLE_ENDIAN */
        ca_uint32_t lpid_high             : 6;
        ca_uint32_t mcgid_high            : 10;
        ca_uint32_t aging_status_high     : 8;
        ca_uint32_t key_dot1p_high        : 3;
        ca_uint32_t rsrvd1                : 5;
#   endif
    } bf ;
    ca_uint32_t     wrd ;
} aal_fdb_cpu_cmd_word1_t;

typedef volatile union {
    struct {
#   ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd1                : 5;
        ca_uint32_t key_dot1p_low         : 3;
        ca_uint32_t aging_status_low      : 8;
        ca_uint32_t mcgid_low             : 10;
        ca_uint32_t lpid_low              : 6;
#   else /* CS_LITTLE_ENDIAN */
        ca_uint32_t lpid_low              : 6;
        ca_uint32_t mcgid_low             : 10;
        ca_uint32_t aging_status_low      : 8;
        ca_uint32_t key_dot1p_low         : 3;
        ca_uint32_t rsrvd1                : 5;
#   endif
    } bf ;
    ca_uint32_t     wrd ;
} aal_fdb_cpu_cmd_word0_t;


/* when mc_flag is 0, use this structure */
typedef volatile union {
    struct {
#   ifdef CS_BIG_ENDIAN
        ca_uint32_t lrned_vid            :  9 ; /* bits 31:23 */
        ca_uint32_t lrned_sc_ind         :  1 ; /* bits 22:22 */
        ca_uint32_t sa_permit            :  1 ; /* bits 21:21 */
        ca_uint32_t da_permit            :  1 ; /* bits 20:20 */
        ca_uint32_t is_static            :  1 ; /* bits 19:19 */
        ca_uint32_t aging_status         :  8 ; /* bits 18:11 */
        ca_uint32_t mc_flag              :  1 ; /* bits 10:10 */
        ca_uint32_t entry_valid          :  1 ;
        ca_uint32_t tmp_flag             :  1 ;
        ca_uint32_t in_st_move_flag      :  1 ;
        ca_uint32_t in_refresh_flag      :  1 ;
        ca_uint32_t lpid                 :  6 ;
#   else /* CS_LITTLE_ENDIAN */
        ca_uint32_t lpid                 :  6 ;
        ca_uint32_t in_refresh_flag      :  1 ;
        ca_uint32_t in_st_move_flag      :  1 ;
        ca_uint32_t tmp_flag             :  1 ;
        ca_uint32_t entry_valid          :  1 ;
        ca_uint32_t mc_flag              :  1 ; /* bits 10:10 */
        ca_uint32_t aging_status         :  8 ; /* bits 18:11 */
        ca_uint32_t is_static            :  1 ; /* bits 19:19 */
        ca_uint32_t da_permit            :  1 ; /* bits 20:20 */
        ca_uint32_t sa_permit            :  1 ; /* bits 21:21 */
        ca_uint32_t lrned_sc_ind         :  1 ; /* bits 22:22 */
        ca_uint32_t lrned_vid            :  9 ; /* bits 31:23 */
#   endif
    } bf ;
    ca_uint32_t     wrd ;
} aal_fdb_data0_t;

typedef volatile union {
    struct {
#   ifdef CS_BIG_ENDIAN
        ca_uint32_t rsvd0                :  12 ;
        ca_uint32_t start_addr           :  12 ;
        ca_uint32_t dyn_tmp              :  1 ;
        ca_uint32_t dyn_formal           :  1 ;
        ca_uint32_t is_static            :  1 ;
        ca_uint32_t in_st_mv_flag        :  1 ;
        ca_uint32_t in_refresh_flag      :  1 ;
        ca_uint32_t rsvd1                :  3 ;
#   else /* CS_LITTLE_ENDIAN */
        ca_uint32_t rsvd1                :  3 ;
        ca_uint32_t in_refresh_flag      :  1 ;
        ca_uint32_t in_st_mv_flag        :  1 ;
        ca_uint32_t is_static            :  1 ;
        ca_uint32_t dyn_formal           :  1 ;
        ca_uint32_t dyn_tmp              :  1 ;
        ca_uint32_t start_addr           :  12 ;
        ca_uint32_t rsvd0                :  12 ;
#   endif
    } bf ;
    ca_uint32_t     wrd ;
} aal_fdb_scan_t;


typedef struct {
    ca_mac_addr_t mac;
    ca_uint16_t     vid;
    ca_uint8_t      dot1p;
} aal_tbl_fdb_key_t;

typedef struct {
    ca_int32_t port_id;
    ca_mac_addr_t mac;
    ca_uint32_t   vid;
    ca_uint32_t   mac_flag;
} aal_tbl_mac_filter_key_t;


typedef struct {
    /*ca_l2_mac_filter_entry_t filter_cfg; */
    ca_int32_t    port_id;
    ca_mac_addr_t mac;       /* MAC address for filter function */
    ca_uint32_t   vid;       /* Top VLAN tag, only used when filter mode is MAC+VLAN                */
    ca_uint32_t   mac_flag;  /* Indicates the filter is based on source or destination*/
                             /* {0(SA), 1(DA)}*/
    ca_uint32_t   mask;      /* Refer to CA_L2_MAC_FILTER_MASK_xxx */
    ca_boolean_t  drop_flag; /* Indciates whether to drop the packet. {FALSE(fordward), TRUE(drop)} */
    ca_uint8_t    rule_id;
    ca_uint32_t   entry_id;
} aal_tbl_mac_filter_entry_t;


typedef struct {
    ca_mac_addr_t    mac;
    ca_uint16_t   key_vid;
    ca_uint8_t    dot1p;
    
    ca_boolean_t  key_sc_ind;


    ca_boolean_t  sa_permit;
    ca_boolean_t  da_permit;

    ca_boolean_t  mc_flag;
    ca_boolean_t  static_flag;

    ca_uint8_t    aging_sts;

    ca_uint16_t   port_id;

    ca_uint16_t   lrnt_vid;
    ca_boolean_t  lrnt_sc_ind;

    /* the following not defined in G3 */
    ca_boolean_t  aged_flag;
    ca_boolean_t  valid;
    ca_boolean_t  tmp_flag;
} aal_fdb_entry_cfg_t;


typedef  union {
    
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd                :  10 ;
    ca_uint32_t loop_pkt_no_learn        : 1;
    ca_uint32_t non_std_sa_b40_is_one_learn: 1; 
    ca_uint32_t non_std_sa_all_one_learn : 1;
    ca_uint32_t non_std_sa_all_zero_learn: 1;
    ca_uint32_t igr_dropped_pkt_no_learn : 1;
    ca_uint32_t unk_ethtype_no_learn     : 1;
    ca_uint32_t unkvlan_learn            : 1; 
    ca_uint32_t cnt_to_1s            :  1;
    ca_uint32_t aging_out_time       :  1; 
    ca_uint32_t cpu_opt_mode         :  1 ; /* bits 31:31 */
    ca_uint32_t ovfl_entries_rcyc_en :  1 ; /* bits 29:29 */
    ca_uint32_t da_deny_dnt_lrn      :  1 ; /* bits 28:28 */
    ca_uint32_t hashing_mode         :  1 ; /* bits 27:26 */
    ca_uint32_t hash_collision_overwrite :  1 ; /* bits 25:25 */
    ca_uint32_t lrn_mode             :  1 ; /* bits 24:23 */
    ca_uint32_t lrn_dis              :  1 ; /* bits 22:22 */
    ca_uint32_t st_mv_window         :  1 ; /* bits 21:13 */
    ca_uint32_t st_move_en           :  1 ; /* bits 11:11 */
    ca_uint32_t aging_start_val      :  1 ; /* bits 10:3 */
    ca_uint32_t aging_mode           :  1 ; /* bits 2:2 */
    ca_uint32_t tmp_entry_aging_dis  :  1 ; /* bits 1:1 */
    ca_uint32_t aging_en             :  1 ; /* bits 0:0 */

#else 
    ca_uint32_t aging_en             :  1 ; /* bits 0:0 */
    ca_uint32_t tmp_entry_aging_dis  :  1 ; /* bits 1:1 */
    ca_uint32_t aging_mode           :  1 ; /* bits 2:2 */
    ca_uint32_t aging_start_val      :  1 ; /* bits 10:3 */
    ca_uint32_t st_move_en           :  1 ; /* bits 11:11 */
    ca_uint32_t st_mv_window         :  1 ; /* bits 21:13 */
    ca_uint32_t lrn_dis              :  1 ; /* bits 22:22 */
    ca_uint32_t lrn_mode             :  1 ; /* bits 24:23 */
    ca_uint32_t hash_collision_overwrite :  1 ; /* bits 25:25 */
    ca_uint32_t hashing_mode         :  1 ; /* bits 27:26 */
    ca_uint32_t da_deny_dnt_lrn      :  1 ; /* bits 28:28 */
    ca_uint32_t ovfl_entries_rcyc_en :  1 ; /* bits 29:29 */
    ca_uint32_t cpu_opt_mode         :  1 ; /* bits 31:31 */
    ca_uint32_t aging_out_time       :  1; 
    ca_uint32_t cnt_to_1s            :  1;
    ca_uint32_t unkvlan_learn            : 1; 
    ca_uint32_t unk_ethtype_no_learn     : 1;
    ca_uint32_t igr_dropped_pkt_no_learn : 1;
    ca_uint32_t non_std_sa_all_zero_learn: 1;
    ca_uint32_t non_std_sa_all_one_learn : 1;
    ca_uint32_t non_std_sa_b40_is_one_learn: 1; 
    ca_uint32_t loop_pkt_no_learn        : 1;
    ca_uint32_t rsrvd                :  10 ;

#endif
  } s;
  ca_uint32_t     wrd ;
} aal_fdb_ctrl_mask_t;

typedef struct {
    ca_boolean_t aging_en;
    ca_boolean_t tmp_entry_aging_dis;
    ca_boolean_t st_move_en;
    ca_boolean_t lrn_dis;
    ca_boolean_t da_deny_dnt_lrn;
    ca_boolean_t hash_collision_overwrite;
    ca_boolean_t ovfl_entries_rcyc_en;
    ca_uint32_t  hashing_mode;
    ca_uint32_t  lrn_mode;
    ca_uint32_t  aging_mode;
    ca_uint32_t  aging_start_val;
    ca_uint32_t  st_mv_window;
    ca_uint32_t  cpu_opt_mode;
    ca_uint32_t  aging_out_time;
    ca_uint32_t  cnt_to_1s;
    ca_boolean_t unkvlan_learn;  
    ca_boolean_t unk_ethtype_no_learn;
    ca_boolean_t igr_dropped_pkt_no_learn;
    ca_boolean_t non_std_sa_all_zero_learn;
    ca_boolean_t non_std_sa_all_one_learn;
    ca_boolean_t non_std_sa_b40_is_one_learn; 
    ca_boolean_t loop_pkt_no_learn;
} aal_fdb_ctrl_t;

typedef struct {
    ca_boolean_t  over_flow_flag;
    ca_uint32_t   cached_entry_num;
}aal_fdb_learn_status_t;

typedef struct {
    ca_boolean_t  lf_overflow        ;
    ca_boolean_t  ple_hd_ff_overflow ;
    ca_boolean_t  port_mac_lmt_excd  ;
    ca_boolean_t  l2e_hash_collision ;
} aal_fdb_glb_intr_t;

typedef union {
  struct {
    ca_uint32_t l2_mac_sw_lrn_ldpid        :  1 ; 
    ca_uint32_t l2_mac_sw_lrn_cos          :  1 ; 
    ca_uint32_t l2_mac_sw_lrn_keep_old_wan :  1 ; 
    ca_uint32_t l2_mac_sw_lrn_keep_old_lan :  1 ; 
    ca_uint32_t sa_miss_fwd                :  1 ; 
    ca_uint32_t nonstd_sa_all_zero_fwd     :  1 ; 
    ca_uint32_t nonstd_sa_all_one_fwd      :  1 ; 
    ca_uint32_t nonstd_sa_b40_is_one_fwd   :  1 ; 
    ca_uint32_t sm_static_entry_fwd        :  1 ; 
    ca_uint32_t sm_tmp_entry_fwd           :  1 ; 
    ca_uint32_t sm_sm_entry_fwd            :  1 ; 
    ca_uint32_t sa_limit_excd_ldpid        :  1 ; 
    ca_uint32_t sa_limit_excd_cos          :  1 ; 
    ca_uint32_t sa_limit_excd_fwd_ctrl     :  1 ; 
    ca_uint32_t sa_limit_excd_keep_old_wan :  1 ; 
    ca_uint32_t sa_limit_excd_keep_old_lan :  1 ; 
    ca_uint32_t sa_hash_cllsn_ldpid        :  1 ; 
    ca_uint32_t sa_hash_cllsn_cos          :  1 ; 
    ca_uint32_t sa_hash_cllsn_fwd_ctrl     :  1 ; 
    ca_uint32_t sa_hash_cllsn_keep_old_wan :  1 ; 
    ca_uint32_t sa_hash_cllsn_keep_old_lan :  1 ; 
    ca_uint32_t swlrn_othr_pkt_fwd         :  1 ; 
    ca_uint32_t rsrvd1                     : 10 ;
  } bf ;
  ca_uint32_t     wrd ;
} lrn_fwd_ctrl_mask_t;


typedef enum{
    SA_LRN_FWD_CTRL_DROP,
    SA_LRN_FWD_CTRL_REDIRECT,
    SA_LRN_FWD_CTRL_FORWARD,
    SA_LRN_FWD_CTRL_MAX
}sa_lrn_fwd_ctrl_e;

typedef struct hask_cllsn_fwd_ctrl_s{
    sa_lrn_fwd_ctrl_e hash_cllsn_mode;
    ca_uint32_t       hash_cllsn_ldpid;
    sa_lrn_fwd_ctrl_e limit_excd_mode;
    ca_uint32_t       limit_excd_ldpid;
    ca_uint32_t       l2_mac_sw_lrn_ldpid;

    ca_uint8_t sa_limit_excd_cos; 
    ca_boolean_t sa_limit_excd_keep_old_wan; 
    ca_boolean_t sa_limit_excd_keep_old_lan; 

    ca_uint8_t sa_hash_cllsn_cos; 
    ca_boolean_t sa_hash_cllsn_keep_old_wan; 
    ca_boolean_t sa_hash_cllsn_keep_old_lan; 
    ca_uint32_t swlrn_othr_pkt_fwd; 

    ca_uint8_t l2_mac_sw_lrn_cos; 
    ca_boolean_t l2_mac_sw_lrn_keep_old_wan; 
    ca_boolean_t l2_mac_sw_lrn_keep_old_lan; 
    ca_boolean_t sa_miss_fwd; 
    ca_boolean_t nonstd_sa_all_zero_fwd; 
    ca_boolean_t nonstd_sa_all_one_fwd; 
    ca_boolean_t nonstd_sa_b40_is_one_fwd; 
    ca_boolean_t sm_static_entry_fwd; 
    ca_boolean_t sm_tmp_entry_fwd; 
    ca_boolean_t sm_sm_entry_fwd; 
    
}lrn_fwd_ctrl_t;

typedef struct arb_non_known_pol_s{ 
    //ca_uint8_t lspid_pol_idx;
    //ca_uint8_t pkt_fwd_type;
    ca_uint8_t profile_id;
}arb_non_known_pol_t;

typedef struct l2te_pol_profile_s{ 
    ca_uint32_t rate;
    ca_uint16_t pir_k;   /* rate in kbps or pps, range 0 ~ 999 */
    ca_uint16_t pir_m;   /* rate in mbps or kpps               */
    ca_uint16_t p_bs;     /* peak burse size, in unit of 256 bytes or 1 packet */
}l2te_pol_profile_t;


typedef struct aal_port_flooding_s{
    ca_uint8_t lspid_policer[AAL_PORT_NUM_LOGICAL];  /* range from 0~AAL_ILPB_UNKWN_POL_NUM */
    arb_non_known_pol_t lspid_type_prof[AAL_ILPB_UNKWN_POL_NUM][AAL_ARB_PKT_FWD_TYPE_MAX];
    l2te_pol_profile_t profile[AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM]; /* l2te support up to 15 */
}aal_port_flooding_t;


/* AAL public APIs */

ca_status_t aal_fdb_debug_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t value1,
    CA_IN ca_int32_t value2
);

ca_status_t aal_fdb_debug_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  value1,
    CA_OUT ca_int32_t *value2
);

ca_status_t aal_fdb_lrn_fwd_ctrl_set(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  lrn_fwd_ctrl_mask_t fwd_ctrl_mask,
    CA_IN  lrn_fwd_ctrl_t    *fwd_ctrl
);

ca_status_t aal_fdb_lrn_fwd_ctrl_get(
    CA_IN  ca_device_id_t       device_id,
    CA_OUT lrn_fwd_ctrl_t    *fwd_ctrl
);


ca_status_t aal_fdb_sw_learning(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
);

ca_status_t aal_fdb_mac_limit_exceed(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              lpid,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
);

ca_status_t aal_fdb_hash_collision(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
);

ca_status_t aal_fdb_cpu_pkt_lrn_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_boolean_t flag
);
ca_status_t aal_fdb_cpu_pkt_lrn_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT ca_boolean_t *flag
);

ca_status_t aal_fdb_limit_excd_fwd_ctrl_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT ca_uint32_t *lrn_ctrl
);

/*
ca_status_t aal_fdb_hash_cllsn_update(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
);
*/
ca_status_t aal_fdb_tbl_del_all(void);

ca_status_t aal_fdb_entry_get(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_mac_addr_t          mac,
    CA_IN ca_uint16_t            vid,
    CA_OUT aal_fdb_entry_cfg_t  *fdb_entry
);

ca_status_t aal_fdb_entry_add(
    CA_IN ca_device_id_t           device_id,
    CA_IN aal_fdb_entry_cfg_t  *fdb
);

ca_status_t aal_fdb_entry_del(
    CA_IN ca_device_id_t           device_id,
    CA_IN aal_fdb_entry_cfg_t  *fdb
);

ca_status_t aal_fdb_addr_aged_entry_get(
    CA_IN ca_device_id_t       device_id,
    CA_IN aal_fdb_entry_cfg_t  *fdb
);

ca_status_t aal_fdb_cmp_cfg_load(aal_fdb_flush_cfg_t *cfg);

ca_status_t aal_fdb_scan_flush_field_set (
    CA_IN ca_device_id_t device_id,
    CA_IN aal_fdb_cpu_cmd_word4_msk_t mask,
    CA_IN aal_fdb_flush_cfg_t *cfg
);
ca_status_t aal_fdb_scan_flush_field_get (
    CA_IN ca_device_id_t device_id,
    CA_IN aal_fdb_flush_cfg_t *cfg
);

ca_status_t aal_fdb_scan_flush_field_init(CA_IN ca_device_id_t device_id);

ca_status_t aal_fdb_entry_flush(
    CA_IN ca_device_id_t                  device_id
);

ca_status_t aal_fdb_entry_scan(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             start_addr,
    CA_OUT ca_uint32_t            *hit_addr,
    CA_OUT aal_fdb_entry_cfg_t    *fdb_entry,
    CA_OUT ca_uint32_t            *cmd_return
);

ca_status_t aal_fdb_port_limit_init(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);

ca_status_t aal_fdb_port_limit_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id,  /* lpid */
    CA_IN ca_uint32_t   addr_limit
);

ca_status_t aal_fdb_port_limit_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id,
    CA_OUT ca_uint32_t  *addr_limit
);

ca_status_t aal_fdb_port_limit_inc(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);

ca_status_t aal_fdb_port_limit_dec(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);

ca_status_t aal_fdb_port_cnt_init(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);

ca_status_t aal_fdb_port_cnt_set(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32_t        port_id,
    CA_IN ca_uint32_t       addr_cnt
);

ca_status_t aal_fdb_port_cnt_get(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32_t        port_id,
    CA_OUT ca_uint32_t      *addr_cnt
);

ca_status_t aal_fdb_port_cnt_inc(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32_t        port_id
);

ca_status_t aal_fdb_port_cnt_dec(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32_t        port_id
);

ca_status_t aal_fdb_learn_status_get(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              port_id,
    CA_OUT aal_fdb_learn_status_t  *status);

/*
ca_status_t aal_fdb_entry_2_l2_addr(
    CA_OUT ca_l2_addr_entry_t *addr,
    CA_IN aal_fdb_entry_cfg_t *fdb
);
*/
ca_status_t aal_fdb_mac_filter_default_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_int32_t port_id,
    CA_IN ca_boolean_t drop_flag
);

ca_status_t aal_fdb_mac_filter_default_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_int32_t port_id,
    CA_OUT ca_boolean_t *drop_flag
);

ca_status_t aal_fdb_mac_filter_add (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_int32_t             port_id,
    CA_IN aal_tbl_mac_filter_entry_t *entry
);

ca_status_t aal_fdb_mac_filter_delete (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_int32_t             port_id,
    CA_IN aal_tbl_mac_filter_entry_t *entry
);

ca_status_t aal_fdb_mac_filter_delete_all (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_int32_t port_id
);

aal_tbl_mac_filter_entry_t *aal_fdb_mac_filter_getfirst(void);

aal_tbl_mac_filter_entry_t *aal_fdb_mac_filter_getnext(void);



ca_status_t aal_fdb_init(
    ca_device_id_t     device_id
);

ca_status_t aal_fdb_mac_filter_init(
    ca_device_id_t     device_id
);

extern ca_port_id_t ca_port_id_get(ca_uint32_t  lpid);

ca_status_t aal_fdb_ctrl_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_fdb_ctrl_mask_t  ctrl_mask,
    CA_IN aal_fdb_ctrl_t       *ctrl
);

ca_status_t aal_fdb_ctrl_get(
    CA_IN ca_device_id_t       device_id,
    CA_OUT aal_fdb_ctrl_t  *ctrl
);

ca_status_t aal_fdb_glb_intr_status_get(
    CA_IN  ca_device_id_t         device_id,
    CA_OUT aal_fdb_glb_intr_t *status
);

ca_status_t aal_fdb_glb_intr_enable_get(
    CA_IN  ca_device_id_t         device_id,
    CA_OUT aal_fdb_glb_intr_t *config
);

ca_status_t aal_fdb_glb_intr_enable_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_fdb_glb_intr_t  *config
);

#if 1
/* aal cli debug purpose */

ca_status_t aal_fdb_dbg_add_get(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             port_id,
    CA_IN ca_uint32_t             static_flag
);

ca_status_t aal_fdb_entry_addr_get(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_mac_addr_t          mac,
    CA_IN ca_uint16_t            key_vid
);


ca_status_t aal_fdb_entry_addr_set (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_mac_addr_t        mac,
    CA_IN ca_uint16_t          key_vid,
    CA_IN aal_fdb_entry_cfg_t *fdb_entry
);

ca_status_t aal_fdb_addr_delete_get(
    CA_IN ca_device_id_t             device_id,
    CA_IN aal_fdb_entry_cfg_t *fdb_entry
);

ca_status_t aal_fdb_table_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             flag
);

ca_status_t aal_fdb_addr_delete_all(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t flag
);

ca_status_t aal_fdb_addr_delete_all_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t flag
);

ca_status_t aal_fdb_addr_traverse_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             static_flag
);

ca_status_t aal_fdb_addr_delete_by_port_get (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_port_id_t          port_id,
    CA_IN ca_uint32_t           flag
);

ca_status_t aal_fdb_addr_iterate_loop_count_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             loop_count
);

ca_status_t aal_fdb_addr_iterate_entry_cnt_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             entry_count
);

ca_status_t aal_fdb_mac_filter_add_get (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_int32_t             lpid,
    CA_IN ca_int32_t             mac_id
);


ca_status_t aal_fdb_mac_filter_delete_get (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_int32_t             lpid,
    CA_IN ca_int32_t             mac_id
);


ca_status_t aal_fdb_mac_filter_delete_all_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_int32_t lpid
);


ca_status_t aal_fdb_mac_filter_traverse_get (
    CA_IN ca_device_id_t             device_id
);


ca_status_t aal_fdb_port_limit_init_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);


ca_status_t aal_fdb_port_limit_inc_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);


ca_status_t aal_fdb_port_limit_dec_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);



ca_status_t aal_fdb_port_cnt_init_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t    port_id
);


ca_status_t aal_fdb_port_cnt_inc_get(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32_t        port_id
);


ca_status_t aal_fdb_port_cnt_dec_get(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32_t        port_id
);

ca_status_t aal_fdb_ilpb_unknw_pol_idx_map_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  lspid,
    CA_IN ca_uint8_t  pkt_type,
    CA_IN ca_uint32_t rate
);

ca_status_t aal_fdb_ilpb_unknw_pol_idx_map_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  lspid,
    CA_IN ca_uint8_t pkt_type,
    CA_IN ca_uint32_t *rate
);


ca_status_t aal_fdb_ilpb_unknw_pol_idx_cb_get(
    CA_IN ca_device_id_t device_id
);

//+Andy
ca_status_t aal_fdb_sram_rw_test(
	CA_IN ca_device_id_t 	device_id,
	CA_IN ca_uint32 	reg_value
);

#endif

ca_status_t aal_fdb_disable_table_aging(
        CA_IN ca_device_id_t device_id,
        CA_IN bool enable
);

ca_status_t aal_fdb_enable_table_indirect_access(
        CA_IN ca_device_id_t device_id,
        CA_IN bool enable
);


#endif /* End of __AAL_FDB_H__ */

