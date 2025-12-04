#ifndef __AAL_L2TM_H__
#define __AAL_L2TM_H__

#include "ca_types.h"

#define CA_AAL_MAX_NETWORK_PORT_ID 0x6    /* 7 ports  */
#define CA_AAL_MAX_PORT_ID         0xf    /* 16 ports  */
#define CA_AAL_MAX_FLOW_ID         0xff   /* 256 flows */
#define CA_AAL_MAX_AGR_FLOW_ID     0x7    /* up to 8 flow group, each group supports up to 32 flows */
#define CA_AAL_MAX_SRP_ID          0x3
#define CA_AAL_MAX_VOQ_ID          0x7f      /* 128 VoQs  */
#define CA_AAL_MAX_IPG_ID          0x3
#define CA_AAL_MAX_IPG             0x1f

#define CA_AAL_ES_PORT_NI0         0
#define CA_AAL_ES_PORT_NI1         1
#define CA_AAL_ES_PORT_NI2         2
#define CA_AAL_ES_PORT_NI3         3
#define CA_AAL_ES_PORT_NI4         4
#define CA_AAL_ES_PORT_NI5         5
#define CA_AAL_ES_PORT_NI6         6
#define CA_AAL_ES_PORT_L3QM        7
#define CA_AAL_ES_PORT_L3LAN       8
#define CA_AAL_ES_PORT_L3WAN       9
#define CA_AAL_ES_PORT_MC          10
#define CA_AAL_ES_MAX_PORT_ID      15

#define CA_AAL_TM_SPID_CPU 9

#define CA_AAL_L2TM_SPID_WAN_L3_L2 0xa
#define CA_AAL_L2TM_SPID_LAN_L3_L2 0xa
#define CA_AAL_L2TM_DPID_L3_WAN 0xa
#define CA_AAL_L2TM_DPID_L3_LAN 0xd


#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define __BM_BUF_BLOCK_SIZE 32    /* in unit of 32 byte buffer */
#else
#define __BM_BUF_BLOCK_SIZE 64    /* in unit of 64 byte buffer */
#endif

#define CA_TM_LOG(/*CODEGEN_IGNORE_TAG*/ args, ...) ca_printf(args, ##__VA_ARGS__)

ca_status_t aal_l2_tm_read_clr_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_boolean_t read_clr
);

ca_status_t aal_l2_tm_read_clr_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_OUT ca_boolean_t *read_clr
);

ca_status_t aal_l2_tm_es_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l2_tm_es_ena_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_OUT ca_boolean_t *ena
);

ca_status_t aal_l2_tm_es_port_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l2_tm_es_port_ena_get
(
    CA_IN ca_device_id_t   device_id, 
    CA_IN ca_port_id_t  port_id,
    CA_OUT ca_boolean_t *ena
);

ca_status_t aal_l2_tm_es_voq_status_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  voq_id,
    CA_OUT ca_boolean_t *empty
);

ca_status_t aal_l2_tm_es_voq_flush_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  voq_id
);

ca_status_t aal_l2_tm_es_voq_flush_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *voq_id
);

ca_status_t aal_l2_tm_es_voq_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  voq_id,
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l2_tm_es_voq_ena_get
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  voq_id,
    CA_OUT ca_boolean_t *ena
);


ca_status_t aal_l2_tm_es_weight_ratio_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint8_t   cos,
    CA_IN ca_uint32_t  weight
);

ca_status_t aal_l2_tm_es_weight_ratio_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint8_t   cos,
    CA_OUT ca_uint32_t  *weight
);

ca_status_t aal_l2_tm_es_port_weight_ratio_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  weight
);

ca_status_t aal_l2_tm_es_port_weight_ratio_get
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *weight
);


#ifdef __L2_TM_BM
#endif
#if 0
typedef struct
{
    ca_uint8_t  cpu_flag;
    ca_uint8_t  deep_q;
    ca_uint8_t  policy_group_id;
    ca_uint16_t policy_id;
    ca_uint8_t  policy_ena;
    ca_uint8_t  marked;
    ca_uint8_t  mirror;
    ca_uint8_t  no_drop;
    ca_uint8_t  rx_packet_type;
    ca_uint8_t  drop_code;
    ca_uint16_t mc_group_id;
    ca_uint8_t  header_type;
    ca_uint8_t  fe_bypass;
    ca_uint16_t packet_size;
    ca_uint8_t  logic_spid;
    ca_uint8_t  logic_dpid;
    ca_uint8_t  cos;   
    ca_mac_addr_t mac_da;
    ca_mac_addr_t mac_sa;
    ca_uint16_t   tpid_eth_type;
    ca_uint8_t    dot_1p;
    ca_uint8_t    cfi;
    ca_uint16_t   vlan_id;
}aal_l2_tm_bm_buff_pkt_t;
#endif

typedef struct
{
    ca_uint32_t rx_cnt;
    ca_uint32_t tx_cnt;
    ca_uint32_t sb_drop_cnt;        /* dropped due to side-band drop signal                                 */
    ca_uint32_t hdr_drop_cnt;       /* dropped due to backhole indicator in header                          */
    ca_uint32_t te_drop_cnt;        /* dropped due to traffic engine including policing, tail and WRED drop */
    ca_uint32_t err_drop_cnt;       /* dropped due to errors as follows
                                       (1) invalid VoQ in header
                                       (2) minimum packet size
                                       (3) maximum packet size
                                       (4) segment length error
                                       (5) packet length error
                                       (6) sequence error                                                   */
    ca_uint32_t no_buff_drop_cnt;   /* dropped due to no available buffer                                   */
}aal_l2_tm_bm_mib_t;

typedef struct
{
    ca_uint8_t fe_tm_valid;
    ca_uint8_t tm_fe_ready;
    ca_uint8_t ni_tm_valid;
    ca_uint8_t tm_ni_ready;
    ca_uint8_t tm_ni_valid;
    ca_uint8_t deep_q_if_fifo_empty;
    ca_uint8_t deep_q_pkt_fifo_empty;
    ca_uint8_t fe_if_fifo_empty;
    ca_uint8_t pkt_fifo_empty;
    ca_uint8_t wr_data_fifo_empty;
    ca_uint8_t en_q_buff_fifo_empty;
    ca_uint8_t rd_data_fifo_empty;
    ca_uint8_t de_q_buff_fifo_empty;
    ca_uint8_t buff_recycling_fifo_empty;
}aal_l2_tm_bm_status_t;

typedef struct
{
    ca_uint8  cos;
    ca_uint8  ldpid;             /* logical DPID */
    ca_uint8  lspid;             /* logical SPID */
    ca_uint16 pkt_size;          /* packet size within 2K ranges (non-jumbo packet size), which includes HRD_CPU and does not include HDR_A length */
    ca_uint8  fe_bypass;         /* bypass L2FE and L3FE processing or not */
    ca_uint8  hdr_type;          /* 0 : generic UC HDR, 1 : MC HDR, 2 : CPU bound packet, MCGID includes L2 learning info  */
    ca_uint8  mc_group_id;       /* MC ground id, redfined as L2 learning info in case hdr_type is 2 */
    ca_uint8  drop_code;         /* drop event code, used by TE */
    ca_uint8  rx_pkt_type;       /* 0 : UC, 1 : BC, 2 : MC, 3 : UUC */
    ca_uint8  no_drop;           /* no drop flag for high priority packet */
    ca_uint8  mirror;            /* packet to be mirrored*/
    ca_uint8  mark;              /* conjession experienced in QM */
    ca_uint8  policer_en;        /* 0 : both disabled, 3 : both enabled, 1 : l2 enabled, 2 : l3 enabled */
    ca_uint16 policing_id;       /* used for policing in TE/QM/TM */
    ca_uint8  policing_group_id; /* 0 is null */
    ca_uint8  deep_q;            /* targed to deep Q in QM */
    ca_uint8  cpu_flag;          /* HDR CPU to be included for this packet */ 
    ca_mac_addr_t mac_da;
    ca_mac_addr_t mac_sa;
    ca_uint16_t eth_type;
    ca_uint16 vlan_tag;
}aal_l2_tm_bm_buff_pkt_t;

/* BM global configuration */
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd        :26;
        ca_uint32_t rx_ena        :1; 
        ca_uint32_t min_pkt_len   :1; 
        ca_uint32_t sb_drop_ena   :1; 
        ca_uint32_t rx_drop_ena   :1;
        ca_uint32_t rx_drop_port  :1; 
        ca_uint32_t rx_drop_cos   :1; 
#else 
        ca_uint32_t rx_drop_cos   :1; 
        ca_uint32_t rx_drop_port  :1; 
        ca_uint32_t rx_drop_ena   :1;
        ca_uint32_t sb_drop_ena   :1; 
        ca_uint32_t min_pkt_len   :1; 
        ca_uint32_t rx_ena        :1; 
        ca_uint32_t f_rsvd        :26;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_tm_bm_cfg_msk_t;


typedef struct
{
    ca_boolean_t rx_drop_ena;     /* ingress drop or not                   */
    ca_uint8_t   rx_drop_cos;     /* drop packets with certain CoS          */
    ca_uint8_t   rx_drop_port;    /* drop packet from certain port          */
    ca_boolean_t sb_drop_ena;     /* side-band drop enable or not           */
    ca_uint8_t   min_pkt_len;     /* allowed minimum packet length in HDR-A */
    ca_boolean_t rx_ena;          /* receiving packets or not               */
}aal_l2_tm_bm_cfg_t;


ca_status_t aal_l2_tm_bm_buff_dump_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint16_t  buff_id
);

ca_status_t aal_l2_tm_bm_stats_get
(
    CA_IN  ca_device_id_t        device_id,
    CA_OUT aal_l2_tm_bm_mib_t *mib
);

ca_status_t aal_l2_tm_bm_status_get
(
    CA_IN  ca_device_id_t           device_id,
    CA_OUT aal_l2_tm_bm_status_t *status
);

ca_status_t aal_l2_tm_bm_buff_get
(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_uint16_t       buff_id,
    CA_OUT aal_l2_tm_bm_buff_pkt_t *buf_pkt
);

ca_status_t aal_l2_tm_bm_cfg_set
(
    CA_IN ca_device_id_t      device_id,
    CA_IN aal_l2_tm_bm_cfg_msk_t msk,
    CA_IN aal_l2_tm_bm_cfg_t     *cfg
);

ca_status_t aal_l2_tm_bm_cfg_get
(
    CA_IN ca_device_id_t   device_id,
    CA_OUT aal_l2_tm_bm_cfg_t *cfg
);

ca_status_t aal_l2_tm_init(ca_device_id_t device_id);

#endif
