#ifndef __AAL_ARB_H__
#define __AAL_ARB_H__

#include "ca_types.h"

#define AAL_ARB_MC_FIB_MAX        63
#define AAL_ARB_FLOOD_POL_ID_NUM  16
#define AAL_ARB_LSPID_POL_MAX      8

typedef enum {
    AAL_ARB_MC_MAC_CMD_NOP  = 0,
    AAL_ARB_MC_MAC_CMD_SWAP = 1,
    AAL_ARB_MC_MAC_CMD_MAX

} aal_arb_mc_mac_cmd_t;

typedef enum {
    AAL_ARB_PKT_FWD_TYPE_MC_UC,
    AAL_ARB_PKT_FWD_TYPE_UUC,
    AAL_ARB_PKT_FWD_TYPE_UMC,
    AAL_ARB_PKT_FWD_TYPE_BC,
    AAL_ARB_PKT_FWD_TYPE_MAX
}aal_arb_pkt_fwd_type_t;

typedef enum {
    AAL_ARB_MC_VLAN_CMD_NOP = 0,
    AAL_ARB_MC_VLAN_CMD_PUSH,
    AAL_ARB_MC_VLAN_CMD_POP,
    AAL_ARB_MC_VLAN_CMD_SWAP,
    AAL_ARB_MC_VLAN_CMD_POP_ALL,
    AAL_ARB_MC_VLAN_CMD_MAX

} aal_arb_mc_vlan_cmd_t;

typedef enum {
    AAL_ARB_MC_DEI_CMD_KEEP_OLD  = 0,
    AAL_ARB_MC_DEI_CMD_USE_TABLE = 1,
    AAL_ARB_MC_DEI_CMD_MAX

} aal_arb_mc_dei_cmd_t;

typedef enum {
    AAL_ARB_MC_DOT1P_CMD_KEEP_OLD  = 0,
    AAL_ARB_MC_DOT1P_CMD_USE_THIS_TABLE = 1,
    AAL_ARB_MC_DOT1P_CMD_USE_EGRESS_TABLE = 2,
    AAL_ARB_MC_DOT1P_CMD_NO_IP_USE_EGRESS_TABLE = 3,
    AAL_ARB_MC_DOT1P_CMD_MAX

} aal_arb_mc_dot1p_cmd_t;


typedef enum {/*AUTO-CLI gen ignore */
    AAL_PLE_NONKWN_TYPE_BC,
    AAL_PLE_NONKWN_TYPE_UUC,
    AAL_PLE_NONKWN_TYPE_UL2MC,
    AAL_PLE_NONKWN_TYPE_UL3MC,
    AAL_PLE_NONKWN_TYPE_MAX
}aal_ple_nonkwn_type_t;


typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1               : 24 ;
    ca_uint32_t dst_wan_dync_buf_sel : 1;
    ca_uint32_t keep_o_lspid_en_for_mc : 1 ;
    ca_uint32_t use_hdr_a_dbuf_en    :  1 ;
    ca_uint32_t dbuf_dpid            :  1 ;
    ca_uint32_t mc_idx_copy_en       :  1 ;
    ca_uint32_t ip_ecn_en            :  1 ;
    ca_uint32_t dbuf_sel             :  1 ;
    ca_uint32_t drop_redir_keep_old  :  1 ;
#else
    ca_uint32_t drop_redir_keep_old  :  1 ;
    ca_uint32_t dbuf_sel             :  1 ;
    ca_uint32_t ip_ecn_en            :  1 ;
    ca_uint32_t mc_idx_copy_en       :  1 ;
    ca_uint32_t dbuf_dpid            :  1 ;
    ca_uint32_t use_hdr_a_dbuf_en    :  1 ;
    ca_uint32_t keep_o_lspid_en_for_mc : 1 ;
    ca_uint32_t dst_wan_dync_buf_sel : 1;
    ca_uint32_t rsrvd1               : 24 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_arb_ctrl_mask_t;


typedef struct {
    ca_boolean_t keep_o_lspid_en_for_mc; /* Control whether keep original lspid for packet from MC port. */
    ca_boolean_t use_hdr_a_dbuf_en; /* treat the HDR_A carried deepQ flag as deepQ search enable*/
    ca_boolean_t mc_idx_copy_en; /* whether keep the mc_idx or not, while redirect the packet to L3FE packet replicated in MC engine.*/
    ca_boolean_t ip_ecn_en; /*  IP ECN function enable */
    ca_boolean_t dbuf_sel; /* flow based deep buffer table indexed by voqid or flowid */
    ca_boolean_t drop_redir_keep_old; /* The dropped redirection packet preform the packet edit or not */
    ca_uint32_t  dbuf_dpid; /* If the final resolved TM_DPID matching with value, the packet will be delivered to Deep Queue in QM */
    ca_boolean_t dst_wan_dync_buf_sel;
} aal_arb_ctrl_t;


typedef volatile union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1          : 30 ;
    ca_uint32_t bitmap_dst_wan  :  1 ;
    ca_uint32_t bitmap_dst_lan  :  1 ;
#else
    ca_uint32_t bitmap_dst_lan  :  1 ;
    ca_uint32_t bitmap_dst_wan  :  1 ;
    ca_uint32_t rsrvd1          : 30 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_arb_per_cos_dont_mark_ctrl_mask_t;

typedef struct {
    ca_uint8_t  bitmap_dst_lan; /* Destined to WAN trafic DSCP mark down disable per cos */
    ca_uint8_t  bitmap_dst_wan; /* Destined to LAN trafic DSCP mark down disable per cos */
} aal_arb_per_cos_dont_mark_ctrl_t;


typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1          : 27 ;
    ca_uint32_t ldpid           :  1 ;
    ca_uint32_t cos             :  1 ;
    ca_uint32_t cos_vld         :  1 ;
    ca_uint32_t wan_dst         :  1 ;
    ca_uint32_t en              :  1 ;

#else
    ca_uint32_t en              :  1 ;
    ca_uint32_t wan_dst         :  1 ;
    ca_uint32_t cos_vld         :  1 ;
    ca_uint32_t cos             :  1 ;
    ca_uint32_t ldpid           :  1 ;
    ca_uint32_t rsrvd1          : 27 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_arb_redir_config_mask_t;

typedef struct {
    ca_boolean_t en;      /* redirection enable control */
    ca_boolean_t wan_dst; /* indicate the packet redirection destination is WAN or LAN */
    ca_boolean_t cos_vld; /* use the table cos as the redirection cos or keep original */
    ca_uint8_t   cos;     /* redirection cos */
    ca_uint16_t  ldpid;   /* when redirect is enabled, current packet redirected to this ldpid */
} aal_arb_redir_config_t;


typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1         : 27 ;
    ca_uint32_t lspid           : 1 ;
    ca_uint32_t lspid_vld       : 1 ;
    ca_uint32_t ldpid           : 1 ;
    ca_uint32_t ldpid_vld       : 1 ;
    ca_uint32_t dbuf_flg        : 1 ;

#else
    ca_uint32_t dbuf_flg        : 1 ;
    ca_uint32_t ldpid_vld       : 1 ;
    ca_uint32_t ldpid           : 1 ;
    ca_uint32_t lspid_vld       : 1 ;
    ca_uint32_t lspid           : 1 ;
    ca_uint32_t rsrvd1         : 27 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_arb_port_dbuf_config_mask_t;


typedef struct {
    ca_uint32_t dbuf_flg;
    ca_uint32_t ldpid_vld;
    ca_uint32_t ldpid;
    ca_uint32_t lspid_vld;
    ca_uint32_t lspid;
} aal_arb_port_dbuf_config_t;


typedef union  {
    ca_uint32_t u32;
    struct {
        ca_uint32_t keep_o_lspid   : 1;
        ca_uint32_t pol_en         : 1;
        ca_uint32_t cos_cmd        : 1;
        ca_uint32_t cos            : 1;
        ca_uint32_t ldpid          : 1;
        ca_uint32_t mac_sa_sel     : 1;
        ca_uint32_t mac_sa_cmd     : 1;
        ca_uint32_t permit_spid_en : 1;
        ca_uint32_t wan_dst        : 1;
        ca_uint32_t vid            : 1;
        ca_uint32_t vlan_cmd       : 1;
        ca_uint32_t vlan_fltr_en   : 1;
        ca_uint32_t mcgid          : 1;
        ca_uint32_t mcgid_en       : 1;
        ca_uint32_t pol_id         : 1;
        ca_uint32_t pol_grp_id     : 1;
        ca_uint32_t dei            : 1;
        ca_uint32_t dei_cmd        : 1;
        ca_uint32_t dot1p          : 1;
        ca_uint32_t dot1p_cmd      : 1;
        ca_uint32_t sc_ind         : 1;
        ca_uint32_t mac_cmd        : 1;
        ca_uint32_t mac_da         : 1;
        ca_uint32_t rsvd           : 9;
    }s;
} aal_arb_mc_fib_mask_t;

typedef struct {
    ca_boolean_t           keep_o_lspid    ;
    ca_boolean_t           pol_en         ;
    ca_boolean_t           cos_cmd        ;
    ca_uint8_t             cos            ;
    ca_uint8_t             ldpid          ;
    ca_uint8_t             mac_sa_sel     ;
    aal_arb_mc_mac_cmd_t   mac_sa_cmd     ;
    ca_boolean_t           permit_spid_en ;
    ca_uint32_t            wan_dst        ;
    ca_uint32_t            vid            ;
    aal_arb_mc_vlan_cmd_t  vlan_cmd       ;
    ca_boolean_t           vlan_fltr_en   ;
    ca_uint32_t            mcgid          ;
    ca_boolean_t           mcgid_en       ;
    ca_uint32_t            pol_id         ;
    ca_uint32_t            pol_grp_id     ;
    ca_uint32_t            dei            ;
    aal_arb_mc_dei_cmd_t   dei_cmd        ;
    ca_uint32_t            dot1p          ;
    aal_arb_mc_dot1p_cmd_t dot1p_cmd      ;
    ca_uint32_t            sc_ind         ;
    aal_arb_mc_mac_cmd_t   mac_da_cmd     ;
    ca_uint8_t             mac_da[6]      ;

} aal_arb_mc_fib_t;
typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1               : 28 ;
    ca_uint32_t dft_fwd_mc_group_id_valid :  1 ;
    ca_uint32_t dft_fwd_redir_en     :  1 ;
    ca_uint32_t dft_fwd_mc_group_id  :  1 ;
    ca_uint32_t dft_fwd_deny         :  1 ;
#else
    ca_uint32_t dft_fwd_deny         :  1 ;
    ca_uint32_t dft_fwd_mc_group_id  :  1 ;
    ca_uint32_t dft_fwd_redir_en     :  1 ;
    ca_uint32_t dft_fwd_mc_group_id_valid :  1 ;
    ca_uint32_t rsrvd1               : 28 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_ple_dft_fwd_ctrl_mask_t;

typedef struct {
    ca_uint16_t  dft_fwd_mc_group_id; /* The MC_Group_ID for the traffic type of the port, used only when MC_Group_ID_Valid */
    ca_boolean_t dft_fwd_deny; /* drop or not */
    ca_boolean_t dft_fwd_redir_en; /* Control this unknown packet will be forwarded to a specify destination port or forwarded to MC Port for replication */
    ca_boolean_t dft_fwd_mc_group_id_valid; /* MC_Group_ID Valid indication */
} aal_ple_dft_fwd_ctrl_t;

typedef struct{
    ca_boolean_t ena;
}aal_arb_flow_voq_dq_ena_t;


ca_status_t aal_arb_init(/*AUTO-CLI gen ignore */
    ca_device_id_t     device_id
);

ca_status_t aal_arb_ctrl_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_arb_ctrl_mask_t  ctrl_mask,
    CA_IN aal_arb_ctrl_t      *ctrl
);

ca_status_t aal_arb_ctrl_get(
    CA_IN ca_device_id_t       device_id,
    CA_OUT aal_arb_ctrl_t  *ctrl
);

ca_status_t aal_arb_per_cos_dont_mark_ctrl_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_arb_per_cos_dont_mark_ctrl_mask_t  ctrl_mask,
    CA_IN aal_arb_per_cos_dont_mark_ctrl_t       *ctrl
);

ca_status_t aal_arb_per_cos_dont_mark_ctrl_get(
    CA_IN ca_device_id_t          device_id,
    CA_OUT aal_arb_per_cos_dont_mark_ctrl_t       *ctrl
);

ca_status_t aal_arb_redir_ldpid_config_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_uint16_t          ldpid,
    CA_IN aal_arb_redir_config_mask_t mask,
    CA_IN aal_arb_redir_config_t *cfg
);

ca_status_t aal_arb_redir_ldpid_config_get(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_uint16_t          ldpid,
    CA_OUT aal_arb_redir_config_t *cfg
);

ca_status_t aal_arb_redir_drop_src_config_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_uint16_t          drop_src,
    CA_IN aal_arb_redir_config_mask_t mask,
    CA_IN aal_arb_redir_config_t *cfg
);

ca_status_t aal_arb_redir_drop_src_config_get(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_uint16_t          drop_src,
    CA_OUT aal_arb_redir_config_t *cfg
);

ca_status_t aal_arb_non_known_pol_map_set(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_uint16_t            lspid_pol_idx,
    CA_IN aal_arb_pkt_fwd_type_t pkt_type,
    CA_IN ca_uint16_t            flooding_pol_id
);

ca_status_t aal_arb_non_known_pol_map_get(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint16_t             lspid_pol_idx,
    CA_IN aal_arb_pkt_fwd_type_t  pkt_type,
    CA_OUT ca_uint16_t           *flooding_pol_id
);

ca_status_t aal_arb_dscp_te_mark_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_boolean_t         ipv6_type,
    CA_IN ca_boolean_t         wan_dst_ind,
    CA_IN ca_uint8_t           tx_dscp,
    CA_IN ca_uint8_t           mark_value
);

ca_status_t aal_arb_dscp_te_mark_get(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_boolean_t         ipv6_type,
    CA_IN ca_boolean_t         wan_dst_ind,
    CA_IN ca_uint8_t           tx_dscp,
    CA_OUT ca_uint8_t         *mark_value
);

ca_status_t aal_arb_port_dbuf_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_uint8_t           dbuf_id,
    CA_IN aal_arb_port_dbuf_config_mask_t mask,
    CA_IN aal_arb_port_dbuf_config_t *cfg
);

ca_status_t aal_arb_port_dbuf_get(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_uint8_t           dbuf_id,
    CA_OUT aal_arb_port_dbuf_config_t *cfg
);

ca_status_t aal_arb_pdpid_map_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_boolean_t         my_mac,
    CA_IN ca_boolean_t         dbuf_flag,
    CA_IN ca_uint8_t           ldpid,
    CA_IN ca_uint8_t           pdpid
);

ca_status_t aal_arb_pdpid_map_get(
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_boolean_t         my_mac,
    CA_IN ca_boolean_t         dbuf_flag,
    CA_IN ca_uint8_t           ldpid,
    CA_OUT ca_uint8_t         *pdpid

);

ca_status_t aal_arb_mc_fib_set(
    CA_IN  ca_device_id_t           device_id,
    CA_IN  ca_uint32_t           fib_idx,
    CA_IN  aal_arb_mc_fib_mask_t mask,
    CA_IN  aal_arb_mc_fib_t      *cfg
);

ca_status_t aal_arb_mc_fib_get(
    CA_IN  ca_device_id_t         device_id,
    CA_IN  ca_uint32_t         fib_idx,
    CA_OUT aal_arb_mc_fib_t   *cfg
);

ca_status_t aal_ple_dft_fwd_set(/*AUTO-CLI gen ignore */
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint16_t                 lspid,
    CA_IN aal_ple_nonkwn_type_t       type,
    CA_IN aal_ple_dft_fwd_ctrl_mask_t mask,
    CA_IN aal_ple_dft_fwd_ctrl_t     *cfg
);

ca_status_t aal_ple_dft_fwd_get(/*AUTO-CLI gen ignore */
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_uint16_t              lspid,
    CA_IN aal_ple_nonkwn_type_t    type,
    CA_OUT aal_ple_dft_fwd_ctrl_t *cfg
);

ca_status_t aal_arb_flow_voq_dbuf_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint16_t                   flow_id,
    CA_OUT aal_arb_flow_voq_dq_ena_t     *ena
);

ca_status_t aal_arb_flow_voq_dbuf_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint16_t                   flow_id,
    CA_IN aal_arb_flow_voq_dq_ena_t     *ena
);


#endif

