#ifndef __AAL_L2_TE_H__
#define __AAL_L2_TE_H__

#include "ca_types.h"

#define AAL_L2_TE_TBL_OP_TIMEOUT      0xfff

#define CA_L2_TE_LOG(/*CODEGEN_IGNORE_TAG*/ args, ...) ca_printf(args, ##__VA_ARGS__)

#define CA_AAL_L2_TE_POL_PM_FLOW_OFFSET 0
#define CA_AAL_L2_TE_POL_PM_AGR_FLOW_OFFSET 256
#define CA_AAL_L2_TE_POL_PM_SRC_PORT_OFFSET 384
#define CA_AAL_L2_TE_POL_PM_PKT_TYPE_OFFSET 400

#define CA_AAL_L2_TE_POL_MAX_RATE_M 0xffff
#define CA_AAL_L2_TE_POL_MAX_RATE_K 0x3ff   /* 999 */
#define CA_AAL_L2_TE_POL_MAX_CBS    0xfff
#define CA_AAL_L2_TE_POL_MAX_PBS    0xfff

#define CA_AAL_L2_TE_SHP_MAX_RATE_M     0xffff
#define CA_AAL_L2_TE_SHP_MAX_RATE_K     0x3ff /* 999 */
#define CA_AAL_L2_TE_SHP_MAX_RATE_K_VOQ 0x3ff /* 499 */
#define CA_AAL_L2_TE_SHP_MAX_BS     0xfff

#define CA_AAL_WRED_MAX_PROFILE_SEL_ID 0x1f
#define CA_AAL_WRED_MAX_PROFILE_ID     0x1f
#define CA_AAL_WRED_MAX_PROFILE_NUMS   0x20
/*#define CA_AAL_WRED_MAX_PROFILE_NUMS   (CA_AAL_WRED_MAX_PROFILE_ID + 1)*/

#define CA_AAL_WRED_MAX_DROP_PROBABILTY 16

/* TE global configurations */
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd       :25;
        ca_uint32_t wred_ecn_ena :1;
        ca_uint32_t pol_ecn_ena  :1; 
        ca_uint32_t mark_ecn_ena :1; 
        ca_uint32_t wred_ena     :1; 
        ca_uint32_t tail_drop_ena:1; 
        ca_uint32_t shaper_ena   :1; 
        ca_uint32_t policer_ena  :1; 
#else 
        ca_uint32_t policer_ena  :1; 
        ca_uint32_t shaper_ena   :1; 
        ca_uint32_t tail_drop_ena:1; 
        ca_uint32_t wred_ena     :1; 
        ca_uint32_t mark_ecn_ena :1; 
        ca_uint32_t pol_ecn_ena  :1; 
        ca_uint32_t wred_ecn_ena :1;
        ca_uint32_t f_rsvd       :25;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_glb_cfg_msk_t;

typedef struct 
{
    ca_boolean_t policer_ena;
    ca_boolean_t shaper_ena;
    ca_boolean_t tail_drop_ena;
    ca_boolean_t wred_ena;
    ca_boolean_t mark_ecn_ena;
    ca_boolean_t pol_ecn_ena;
    ca_boolean_t wred_ecn_ena;
}aal_l2_te_glb_cfg_t;

ca_status_t aal_l2_te_glb_cfg_set
(
    CA_IN ca_device_id_t             device_id,
    CA_IN aal_l2_te_glb_cfg_msk_t msk,
    CA_IN aal_l2_te_glb_cfg_t     *cfg
);

ca_status_t aal_l2_te_glb_cfg_get
(
    CA_IN  ca_device_id_t         device_id,
    CA_OUT aal_l2_te_glb_cfg_t *cfg
);

/* TE global status */
typedef struct
{
    ca_uint8_t qmio_enq_ovrn;
    ca_uint8_t qmio_deq_ovrn;
    ca_uint8_t qmio_enp_ovrn;
    ca_uint8_t qmio_dep_ovrn;
    ca_uint8_t qmio_shp_ovrn;
    ca_uint8_t te_qm_nrdy;
    ca_uint8_t pol_init_done;
    ca_uint8_t shp_init_done;
    ca_uint8_t cb_init_done;
    ca_uint8_t wred_init_done;
    ca_uint8_t feio_sm;
    ca_uint8_t fe_te_req;
    ca_uint8_t te_fe_ack;
    ca_uint8_t qmio_idle;
    ca_uint8_t init_done;
}aal_l2_te_status_t;

ca_status_t aal_l2_te_status_get
(
    CA_IN  ca_device_id_t        device_id,
    CA_OUT aal_l2_te_status_t *status
);


/*
** shaper profile configuration 
*/
typedef enum
{
    CA_AAL_SHAPER_ADMIN_STATE_FORWARD = 0,  /* always forward packets */
    CA_AAL_SHAPER_ADMIN_STATE_DROP,         /* always drop packets    */
    CA_AAL_SHAPER_ADMIN_STATE_SHAPER,       /* normal shaper limit    */
    CA_AAL_SHAPER_ADMIN_STATE_TBC_WRITABLE,
    CA_AAL_SHAPER_STATE_END,
}aal_l2_te_shaper_admin_state_t;

typedef enum
{
    CA_AAL_SHAPER_MODE_BPS = 0,        /* byte(octet) mode */
    CA_AAL_SHAPER_MODE_PPS,            /* packet mode      */
    CA_AAL_SHAPER_MODE_END,
}aal_l2_te_shaper_counting_mode_t;

typedef enum
{
    CA_AAL_SHAPER_TBC_PKT_MODE_CLASS0 = 0,
    CA_AAL_SHAPER_TBC_PKT_MODE_CLASS1,
    CA_AAL_SHAPER_TBC_PKT_MODE_END,
}aal_l2_te_shaper_tbc_pkt_mode_t;

typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd    :25;
        ca_uint32_t pkt_mode  :1;
        ca_uint32_t tbc_count :1; 
        ca_uint32_t mode      :1; 
        ca_uint32_t state     :1; 
        ca_uint32_t bs        :1; 
        ca_uint32_t rate_m    :1; 
        ca_uint32_t rate_k    :1; 
#else 
        ca_uint32_t rate_k    :1; 
        ca_uint32_t rate_m    :1; 
        ca_uint32_t bs        :1; 
        ca_uint32_t state     :1; 
        ca_uint32_t mode      :1; 
        ca_uint32_t tbc_count :1; 
        ca_uint32_t pkt_mode  :1;
        ca_uint32_t f_rsvd    :25;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_shaper_tbc_cfg_msk_t;

typedef struct 
{
    ca_uint16_t rate_k;                        /* VoQ          : in kbps or pps, range 0 ~ 499 */
                                               /* port & group : in unit of 1Mbps or 1000 pps */
    ca_uint16_t rate_m;                        /* VoQ          : in unit of 500kbps or 500 pps */
                                               /* port & group : in unit of 1Mbps or 1000 pps  */
    ca_uint16_t bs;                            /* burst size, in unit of 256 bytes or 1 packet */
                                               /* bs should be greater than rate_m / 2 / 256   */
    aal_l2_te_shaper_admin_state_t state;      /* state                                        */
    aal_l2_te_shaper_counting_mode_t mode;     /* token counting mode, pps or bps              */
    ca_uint32_t tbc_count;                     /* token bucket count, MSB bit is sign bit, 1 : positive value, 0 : negative value */
    aal_l2_te_shaper_tbc_pkt_mode_t pkt_mode;  /* 0 : packet mode class 0, 1 : packet mode class 1 */
}aal_l2_te_shaper_tbc_cfg_t;


/*
** shaper SRP configurations
*/
typedef enum
{
    CA_AAL_SHAPER_SRP_CLASS_A = 0,
    CA_AAL_SHAPER_SRP_CLASS_B,
    CA_AAL_SHAPER_SRP_CLASS_END
}aal_l2_te_shaper_srp_class_t;

typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd     :29;
        ca_uint32_t class_type :1; 
        ca_uint32_t cos        :1; 
        ca_uint32_t ena        :1; 
#else 
        ca_uint32_t ena        :1; 
        ca_uint32_t cos        :1; 
        ca_uint32_t class_type :1; 
        ca_uint32_t f_rsvd     :29;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_shaper_srp_cfg_msk_t;

typedef struct
{
    ca_boolean_t ena;
    ca_uint16_t  cos;                     /* SRP COS, range 0 ~ 7                           */
    aal_l2_te_shaper_srp_class_t class_type; /* SRP class A or B                               */
}aal_l2_te_shaper_srp_cfg_t;

ca_status_t aal_l2_te_shaper_srp_bmp_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  bmp
);

ca_status_t aal_l2_te_shaper_srp_bmp_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *bmp
);

ca_status_t aal_l2_te_shaper_srp_cfg_set
(
    CA_IN ca_device_id_t                    device_id,
    CA_IN ca_uint8_t                     srp_id,
    CA_IN aal_l2_te_shaper_srp_cfg_msk_t msk,
    CA_IN aal_l2_te_shaper_srp_cfg_t     *cfg
);

ca_status_t aal_l2_te_shaper_srp_cfg_get
(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint8_t                 srp_id,
    CA_OUT aal_l2_te_shaper_srp_cfg_t *cfg
);

ca_status_t aal_l2_te_shaper_ipq_set
(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint8_t  ipg_id,
    CA_IN ca_uint16_t ipg
);

ca_status_t aal_l2_te_shaper_ipq_get
(
    CA_IN  ca_device_id_t device_id,
    CA_IN  ca_uint8_t  ipg_id,
    CA_OUT ca_uint16_t *ipg
);

ca_status_t aal_l2_te_shaper_port_ipq_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint8_t   ipg_id
);

ca_status_t aal_l2_te_shaper_port_ipq_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint8_t   *ipg_id
);

ca_status_t aal_l2_te_shaper_voq_tbc_set
(
    CA_IN ca_device_id_t                    device_id,
    CA_IN ca_uint32_t                    voq_id,
    CA_IN aal_l2_te_shaper_tbc_cfg_msk_t msk,
    CA_IN aal_l2_te_shaper_tbc_cfg_t     *cfg
);

ca_status_t aal_l2_te_shaper_voq_tbc_get
(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                voq_id,
    CA_OUT aal_l2_te_shaper_tbc_cfg_t *cfg
);

ca_status_t aal_l2_te_shaper_port_tbc_set
(
    CA_IN ca_device_id_t                    device_id,
    CA_IN ca_port_id_t                   port_id,
    CA_IN aal_l2_te_shaper_tbc_cfg_msk_t msk,
    CA_IN aal_l2_te_shaper_tbc_cfg_t     *cfg
);

ca_status_t aal_l2_te_shaper_port_tbc_get
(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_port_id_t               port_id,
    CA_OUT aal_l2_te_shaper_tbc_cfg_t *cfg
);

ca_status_t aal_l2_te_shaper_group_tbc_set
(
    CA_IN ca_device_id_t                    device_id,
    CA_IN ca_uint32_t                    group_id,
    CA_IN aal_l2_te_shaper_tbc_cfg_msk_t msk,
    CA_IN aal_l2_te_shaper_tbc_cfg_t     *cfg
);

ca_status_t aal_l2_te_shaper_group_tbc_get
(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint32_t                 group_id,
    CA_OUT aal_l2_te_shaper_tbc_cfg_t *cfg
);


#ifdef __L2_TE_POL
#endif

/* 
** packet type used by L2 TM engine 
*/
typedef enum
{
    CA_AAL_POLICER_PKT_TYPE_UC = 0,
    CA_AAL_POLICER_PKT_TYPE_MC,        
    CA_AAL_POLICER_PKT_TYPE_BC,
    CA_AAL_POLICER_PKT_TYPE_UUC,
    CA_AAL_POLICER_PKT_TYPE_UMC,
    CA_AAL_POLICER_PKT_TYPE_UBC,
    CA_AAL_POLICER_PKT_TYPE_RSV1,
    CA_AAL_POLICER_PKT_TYPE_RSV2,
    CA_AAL_POLICER_PKT_TYPE_END = 16,
}aal_l2_te_policer_pkt_type_t;

/*
** agr flow mapping type
*/
typedef enum
{
    CA_AAL_POLICER_GRP_MAP_TYPE_A = 0,
    CA_AAL_POLICER_GRP_MAP_TYPE_B,
    CA_AAL_POLICER_GRP_MAP_TYPE_C,
    CA_AAL_POLICER_GRP_MAP_TYPE_D,
    CA_AAL_POLICER_GRP_MAP_TYPE_END = 4, /* CA_AAL_POLICER_GRP_MAP_TYPE_D , */
}aal_l2_te_policer_grp_map_type_t;


/*
** policing profile configuration 
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd : 26;
        ca_uint32_t pbs    : 1; 
        ca_uint32_t pir_m  : 1; 
        ca_uint32_t pir_k  : 1; 
        ca_uint32_t cbs    : 1; 
        ca_uint32_t cir_m  : 1; 
        ca_uint32_t cir_k  : 1; 
#else 
        ca_uint32_t cir_k  : 1; 
        ca_uint32_t cir_m  : 1; 
        ca_uint32_t cbs    : 1; 
        ca_uint32_t pir_k  : 1; 
        ca_uint32_t pir_m  : 1; 
        ca_uint32_t pbs    : 1; 
        ca_uint32_t f_rsvd : 26;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_policer_profile_msk_t;

typedef struct 
{
    ca_uint16_t cir_k;   /* rate in kbps or pps, range 0 ~ 999 */
    ca_uint16_t cir_m;   /* rate in mbps or kpps               */
    ca_uint16_t cbs;     /* committed burst size, in unit of 256 bytes or 1 packet */
    ca_uint16_t pir_k;   /* rate in kbps or pps, range 0 ~ 999 */
    ca_uint16_t pir_m;   /* rate in mbps or kpps               */
    ca_uint16_t pbs;     /* peak burse size, in unit of 256 bytes or 1 packet */
}aal_l2_te_policer_profile_t;


/*
** statistics for policing 
*/
typedef struct
{
    ca_uint32_t committed_tb_nums;  /* committed token bucket numbers */
    ca_uint32_t peak_tb_nums;       /* peak token bucket numbers      */
}aal_l2_te_policer_stats_t;


/*
** token bucket related configruations
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd              :27;
        ca_uint32_t last_update_cycle   :1; 
        ca_uint32_t cycle_cnt_lmt       :1; 
        ca_uint32_t update_cnt_lmt      :1; 
        ca_uint32_t sub_token_ena       :1; 
        ca_uint32_t addition_token_ena  :1; 
#else 
        ca_uint32_t addition_token_ena  :1; 
        ca_uint32_t sub_token_ena       :1; 
        ca_uint32_t update_cnt_lmt      :1; 
        ca_uint32_t cycle_cnt_lmt       :1; 
        ca_uint32_t last_update_cycle   :1; 
        ca_uint32_t f_rsvd              :27;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_token_bucket_cfg_msk_t;


typedef struct 
{
    ca_uint8_t  addition_token_ena; /* 1 : enable the addition of the token buckets, 0 : disable the addition of the token buckets        */
    ca_uint8_t  sub_token_ena;      /* 1 : enable the Subtraction of the token buckets, 0 : disable the subtraction of the token buckets  */
    ca_uint8_t  update_cnt_lmt;     /* The limit of update counter; it equals to the number of clock minus 1 to update a token bucket     */
    ca_uint16_t cycle_cnt_lmt;      /* The limit of update counter; it equals to the number of clocks minus 1 to update all token buckets */
    ca_uint16_t last_update_cycle;  /* The cycle of last entry update, the fomular is (number of buckets) * (update_cnt_lmt + 1) - 1      */
}aal_l2_te_token_bucket_cfg_t;


/*
**
*/
typedef enum
{
    CA_AAL_POLICER_TYPE_NONE = 0,   /* disable                                                                 */
    CA_AAL_POLICER_TYPE_RFC2698,    /* two rate three color marker                                             */
    CA_AAL_POLICER_TYPE_RFC2697,    /* single rate three color marker                                          */
    CA_AAL_POLICER_TYPE_RFC4115,    /* differentiated two rate three color marker                              */
    CA_AAL_POLICER_TYPE_RFC4115_EX, /* rfc 4115 with token coupling, CIR overflow token are added to EIR token */
}aal_l2_te_policer_type_t;

typedef enum
{
    CA_AAL_POLICER_UPDATE_MODE_BYTE = 0,   /* byte(octet) mode  */
    CA_AAL_POLICER_UPDATE_MODE_PKT,        /* packet based mode */
}aal_l2_te_policer_update_mode_t;

typedef enum
{
    CA_AAL_POLICER_PKT_LEN_INGRESS = 0,
    CA_AAL_POLICER_PKT_LEN_EGRESS,
}aal_l2_te_policer_pkt_len_set_t;

typedef enum
{
    CA_AAL_POLICER_RESULT_UPDATE_MODE_NESTED_LEVEL_COLOR = 0,   /* use policer result color and nested level result color */
    CA_AAL_POLICER_RESULT_UPDATE_MODE_COLOR,                    /* use policer result color */
}aal_l2_te_policer_result_update_mode_t;


/*
** policing related configurations 
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd             :23;
        ca_uint32_t nest_level         :1; 
        ca_uint32_t result_update_mode :1; 
        ca_uint32_t bypass_red         :1; 
        ca_uint32_t bypass_yellow      :1; 
        ca_uint32_t ipg                :1; 
        ca_uint32_t pkt_len_sel        :1; 
        ca_uint32_t pol_update_mode    :1; 
        ca_uint32_t color_blind        :1; 
        ca_uint32_t type               :1; 
#else 
        ca_uint32_t type               :1; 
        ca_uint32_t color_blind        :1; 
        ca_uint32_t pol_update_mode    :1; 
        ca_uint32_t pkt_len_sel        :1; 
        ca_uint32_t ipg                :1; 
        ca_uint32_t bypass_yellow      :1; 
        ca_uint32_t bypass_red         :1; 
        ca_uint32_t result_update_mode :1; 
        ca_uint32_t nest_level         :1; 
        ca_uint32_t f_rsvd             :23;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_policer_cfg_msk_t;


typedef struct 
{
    aal_l2_te_policer_type_t         type;
    ca_boolean_t                     color_blind;     /* TRUE : color blind; FALSE : color aware */
    aal_l2_te_policer_update_mode_t  pol_update_mode;
    aal_l2_te_policer_pkt_len_set_t  pkt_len_sel;
    ca_uint16_t                      ipg;             /* ipg value added into packet length, 0 ~ 31 */
    ca_boolean_t                     bypass_yellow;   /* TRUE : disable policer color yellow; FALSE : enable policer color yellow */
    ca_boolean_t                     bypass_red;      /* TRUE : disable policer color red;    FALSE : enable policer color red    */
    aal_l2_te_policer_result_update_mode_t result_update_mode;
    ca_uint8_t                       nest_level;      /* 0 ~ 3 */
}aal_l2_te_policer_cfg_t;


/*
** policing counter
*/

typedef struct
{
    ca_uint32_t total_pkt;
    ca_uint32_t red_pkt;        /* drop   */
    ca_uint32_t yellow_pkt;     /* mark   */
    ca_uint32_t green_pkt;      /* unmark */
    ca_uint64_t total_bytes;
    ca_uint64_t red_bytes;
    ca_uint64_t yellow_bytes;
    ca_uint64_t green_bytes;
}aal_l2_te_pm_policer_t;

typedef struct
{
    ca_uint32_t total_pkt;          /* ingress packet excluding policer drop */
    ca_uint32_t td_drop_pkt;        /* TD drop          */
    ca_uint32_t wred_mark_pkt;      /* WRED mark drop   */
    ca_uint32_t wred_unmark_pkt;    /* WRED unmark drop */
    ca_uint64_t total_bytes;
    ca_uint64_t td_drop_bytes;
    ca_uint64_t wred_mark_bytes;
    ca_uint64_t wred_unmark_bytes;
}aal_l2_te_pm_egress_t;


ca_status_t aal_l2_te_policer_pkt_type_cfg_set
(
    CA_IN ca_device_id_t                  device_id,
    CA_IN aal_l2_te_policer_pkt_type_t pkt_type,
    CA_IN aal_l2_te_policer_cfg_msk_t  msk,
    CA_IN aal_l2_te_policer_cfg_t      *cfg
);

ca_status_t aal_l2_te_policer_pkt_type_cfg_get
(
    CA_IN  ca_device_id_t                  device_id,
    CA_IN  aal_l2_te_policer_pkt_type_t pkt_type,
    CA_OUT aal_l2_te_policer_cfg_t      *cfg
);

ca_status_t aal_l2_te_token_bucket_cfg_get
(
    CA_IN  ca_device_id_t                  device_id,
    CA_OUT aal_l2_te_token_bucket_cfg_t *cfg
);

ca_status_t aal_l2_te_policer_pkt_type_profile_set
(
    CA_IN ca_device_id_t                     device_id,
    CA_IN aal_l2_te_policer_pkt_type_t    pkt_type,
    CA_IN aal_l2_te_policer_profile_msk_t msk,
    CA_IN aal_l2_te_policer_profile_t     *cfg
);

ca_status_t aal_l2_te_policer_pkt_type_profile_get
(
    CA_IN  ca_device_id_t                  device_id,
    CA_IN  aal_l2_te_policer_pkt_type_t pkt_type,
    CA_OUT aal_l2_te_policer_profile_t  *cfg
);

ca_status_t aal_l2_te_policer_pkt_type_stats_get
(
    CA_IN  ca_device_id_t                  device_id,
    CA_IN  aal_l2_te_policer_pkt_type_t pkt_type,
    CA_OUT aal_l2_te_policer_stats_t    *stats
);


ca_status_t aal_l2_te_policer_port_cfg_set
(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_port_id_t                port_id,
    CA_IN aal_l2_te_policer_cfg_msk_t msk,
    CA_IN aal_l2_te_policer_cfg_t     *cfg
);

ca_status_t aal_l2_te_policer_port_cfg_get(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_port_id_t            port_id,
    CA_OUT aal_l2_te_policer_cfg_t *cfg
);

ca_status_t aal_l2_te_policer_port_profile_set
(
    CA_IN ca_device_id_t                     device_id,
    CA_IN ca_port_id_t                    port_id,
    CA_IN aal_l2_te_policer_profile_msk_t msk,
    CA_IN aal_l2_te_policer_profile_t     *cfg
);

ca_status_t aal_l2_te_policer_port_profile_get
(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_port_id_t                port_id,
    CA_OUT aal_l2_te_policer_profile_t *cfg
);

ca_status_t aal_l2_te_policer_port_stats_get
(
    CA_IN  ca_device_id_t               device_id,
    CA_IN  ca_port_id_t              port_id,
    CA_OUT aal_l2_te_policer_stats_t *stats
);

ca_status_t aal_l2_te_policer_flow_cfg_set
(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_flow_id_t                flow_id,
    CA_IN aal_l2_te_policer_cfg_msk_t msk,
    CA_IN aal_l2_te_policer_cfg_t     *cfg
);

ca_status_t aal_l2_te_policer_flow_cfg_get
(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_flow_id_t            flow_id,
    CA_OUT aal_l2_te_policer_cfg_t *cfg
);

ca_status_t aal_l2_te_policer_flow_profile_set
(
    CA_IN ca_device_id_t                     device_id,
    CA_IN ca_flow_id_t                    flow_id,
    CA_IN aal_l2_te_policer_profile_msk_t msk,
    CA_IN aal_l2_te_policer_profile_t     *cfg
);

ca_status_t aal_l2_te_policer_flow_profile_get
(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_flow_id_t                flow_id,
    CA_OUT aal_l2_te_policer_profile_t *cfg
);

ca_status_t aal_l2_te_policer_flow_stats_get
(
    CA_IN  ca_device_id_t               device_id,
    CA_IN  ca_flow_id_t              flow_id,
    CA_OUT aal_l2_te_policer_stats_t *stats
);

/*
ca_status_t aal_l2_te_policer_flow_grp_map_set
(
    ca_device_id_t                    device_id,
    ca_uint32_t                    agr_flow_id,
    aal_l2_te_policer_grp_map_type_t grp_map_type,
    ca_uint32_t                    bmp
);

ca_status_t aal_l2_te_policer_flow_grp_map_get
(
    ca_device_id_t                    device_id,
    ca_uint32_t                    agr_flow_id,
    aal_l2_te_policer_grp_map_type_t grp_map_type,
    ca_uint32_t                    *bmp
);
*/

ca_status_t aal_l2_te_policer_agr_flow_cfg_set
(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint32_t                 agr_flow_id,
    CA_IN aal_l2_te_policer_cfg_msk_t msk,
    CA_IN aal_l2_te_policer_cfg_t     *cfg
);

ca_status_t aal_l2_te_policer_agr_flow_cfg_get
(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_uint32_t             agr_flow_id,
    CA_OUT aal_l2_te_policer_cfg_t *cfg
);

ca_status_t aal_l2_te_policer_agr_flow_profile_set
(
    CA_IN ca_device_id_t                     device_id,
    CA_IN ca_uint32_t                     agr_flow_id,
    CA_IN aal_l2_te_policer_profile_msk_t msk,
    CA_IN aal_l2_te_policer_profile_t     *cfg
);

ca_status_t aal_l2_te_policer_agr_flow_profile_get
(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_uint32_t                 agr_flow_id,
    CA_OUT aal_l2_te_policer_profile_t *cfg
);

ca_status_t aal_l2_te_policer_agr_flow_stats_get
(
    CA_IN  ca_device_id_t               device_id,
    CA_IN  ca_uint32_t               agr_flow_id,
    CA_OUT aal_l2_te_policer_stats_t *stats
);

#ifdef __L2_TE_PM
#endif

typedef struct
{
    ca_uint32_t total_cnt;
    ca_uint32_t pre_mark_cnt;         /* rx pre mark packets      */
    ca_uint32_t policer_drop_cnt;     /* policer drop packets     */
    ca_uint32_t policer_mark_cnt;     /* tx policer mark packets  */
    ca_uint32_t tail_drop_cnt;        /* tail drop packets        */
    ca_uint32_t wred_yellow_drop_cnt; /* wred yellow drop packets */
    ca_uint32_t wred_green_drop_cnt;  /* wred green drop packets  */
    ca_uint32_t mark_cnt;             /* tx mark packets          */
    ca_uint32_t bypass_cnt;           /* rx bypass packets        */
    ca_uint32_t bypass_flow_cnt;      /* rx bypass flow policer packets */
    ca_uint32_t ce_cnt;               /* tx ce packet counter     */
}aal_l2_te_pm_stats_t;

ca_status_t aal_l2_te_pm_stats_get
(
    CA_IN  ca_device_id_t          device_id, 
    CA_OUT aal_l2_te_pm_stats_t *stats
);

ca_status_t aal_l2_te_pm_enable_set
(
    ca_device_id_t device_id,
    ca_boolean_t enable
);

ca_status_t aal_l2_te_pm_enable_get
(
    ca_device_id_t device_id,
    ca_boolean_t *enable
);

ca_status_t aal_l2_te_pm_read_clr_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_boolean_t read_clr
);

ca_status_t aal_l2_te_pm_read_clr_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_OUT ca_boolean_t *read_clr
);

ca_status_t aal_l2_te_pm_cnt_mem_set
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t block_id,
    CA_IN ca_uint32_t sel_id,
    CA_IN ca_uint32_t sel_event
);

ca_status_t aal_l2_te_pm_cnt_mem_get
(/*CODEGEN_IGNORE_TAG*/
    CA_IN  ca_device_id_t device_id,
    CA_IN  ca_uint32_t block_id,
    CA_OUT ca_uint32_t *sel_id,
    CA_OUT ca_uint32_t *sel_event
);

ca_status_t aal_l2_te_pm_policer_pkt_type_get
(
    CA_IN  ca_device_id_t                  device_id,
    CA_IN  aal_l2_te_policer_pkt_type_t pkt_type,
    CA_OUT aal_l2_te_pm_policer_t       *pm
);

ca_status_t aal_l2_te_pm_policer_port_get
(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_port_id_t      port_id,
    CA_OUT aal_l2_te_pm_policer_t *pm
);

ca_status_t aal_l2_te_pm_policer_flow_get
(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_flow_id_t      flow_id,
    CA_OUT aal_l2_te_pm_policer_t *pm
);

ca_status_t aal_l2_te_pm_policer_agr_flow_get
(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_uint32_t       agr_flow_id,
    CA_OUT aal_l2_te_pm_policer_t *pm
);

ca_status_t aal_l2_te_pm_egress_voq_get
(
    ca_device_id_t           device_id,
    ca_uint32_t           voq_id,
    aal_l2_te_pm_egress_t *pm
);


#ifdef __TE_WRED
#endif
/*
** WRED configuration
*/


typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd        :30;
        ca_uint32_t global_weight :1; 
        ca_uint32_t fast_decay    :1; 
#else 
        ca_uint32_t fast_decay    :1; 
        ca_uint32_t global_weight :1; 
        ca_uint32_t f_rsvd        :30;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_wred_cfg_msk_t;

typedef struct 
{
    ca_boolean_t       fast_decay;      /* WRED average size update mode, 0 : normal, 1 : fast decay */
    ca_uint32_t        global_weight;   /* weight factor for average free global buffer size */
}aal_l2_te_wred_cfg_t;

/*
** WRED profile selection
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd        :24;
        ca_uint32_t profile_id_7  :1; 
        ca_uint32_t profile_id_6  :1; 
        ca_uint32_t profile_id_5  :1; 
        ca_uint32_t profile_id_4  :1; 
        ca_uint32_t profile_id_3  :1; 
        ca_uint32_t profile_id_2  :1; 
        ca_uint32_t profile_id_1  :1; 
        ca_uint32_t profile_id_0  :1; 
#else 
        ca_uint32_t profile_id_0  :1;
        ca_uint32_t profile_id_1  :1; 
        ca_uint32_t profile_id_2  :1; 
        ca_uint32_t profile_id_3  :1; 
        ca_uint32_t profile_id_4  :1; 
        ca_uint32_t profile_id_5  :1; 
        ca_uint32_t profile_id_6  :1; 
        ca_uint32_t profile_id_7  :1; 
        ca_uint32_t f_rsvd        :24;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_wred_profile_sel_msk_t;

typedef struct
{
    ca_uint16_t profile_id[8];  /* 8 means cos 0 to 7, each cos will have a profile individually */
}aal_l2_te_wred_profile_sel_t;


/*
** WRED profile
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd      :24;
        ca_uint32_t mark_idx    :1; 
        ca_uint32_t unmark_idx  :1; 
        ca_uint32_t mark_dp     :1;
        ca_uint32_t unmark_dp   :1;
#else 
        ca_uint32_t unmark_dp   :1;
        ca_uint32_t mark_dp     :1; 
        ca_uint32_t unmark_idx  :1; 
        ca_uint32_t mark_idx    :1; 
        ca_uint32_t f_rsvd      :24;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_te_wred_profile_msk_t;

typedef struct
{
    ca_uint32_t unmark_dp[CA_AAL_WRED_MAX_DROP_PROBABILTY];          /* 6 bits, drop probability */
    ca_uint32_t mark_dp[CA_AAL_WRED_MAX_DROP_PROBABILTY];            /* 6 bits, drop probability */
    ca_uint16_t unmark_idx;             /* 14 bits, each DP pointer range is 2^unmark_idx buffers. WRED drop range is 0~ 2^(unmark_idx+4) -1 free buffers */
    ca_uint16_t mark_idx;               /* 14 bits, each DP pointer range is 2^mark_idx buffers. WRED drop range is 0~ 2^(mark_idx+4) -1 free buffers */
}aal_l2_te_wred_profile_t;



typedef struct
{
    ca_uint32_t ref_counter;
    ca_uint32_t marked_dp[CA_AAL_WRED_MAX_DROP_PROBABILTY];
    ca_uint32_t unmarked_dp[CA_AAL_WRED_MAX_DROP_PROBABILTY];
}aal_l2_te_wred_info_t;




ca_status_t aal_l2_te_wred_cfg_set
(
    CA_IN ca_device_id_t              device_id,
    CA_IN aal_l2_te_wred_cfg_msk_t msk,
    CA_IN aal_l2_te_wred_cfg_t     *cfg
);

ca_status_t aal_l2_te_wred_cfg_get
(
    CA_IN  ca_device_id_t          device_id,
    CA_OUT aal_l2_te_wred_cfg_t *cfg
);

ca_status_t aal_l2_te_wred_global_free_mem_size_set
(
    ca_device_id_t  device_id,
    ca_uint32_t  pool_id,
    ca_uint32_t  size
);

ca_status_t aal_l2_te_wred_global_free_mem_size_get
(
    ca_device_id_t  device_id,
    ca_uint32_t  pool_id,
    ca_uint32_t  *size
);

ca_status_t aal_l2_te_wred_voq_buff_size_set
(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t voq_id,
    CA_IN ca_uint32_t buff_size
);

ca_status_t aal_l2_te_wred_voq_buff_size_get
(
    CA_IN  ca_device_id_t device_id,
    CA_IN  ca_uint32_t voq_id,
    CA_OUT ca_uint32_t *buff_size
);

ca_status_t aal_l2_te_wred_profile_sel_set
(
    CA_IN ca_device_id_t                      device_id,
    CA_IN ca_uint32_t                      id,
    CA_IN aal_l2_te_wred_profile_sel_msk_t msk,
    CA_IN aal_l2_te_wred_profile_sel_t     *sel
);

ca_status_t aal_l2_te_wred_profile_sel_get
(
    CA_IN  ca_device_id_t                  device_id,
    CA_IN  ca_uint32_t                  id,
    CA_OUT aal_l2_te_wred_profile_sel_t *sel
);

ca_status_t aal_l2_te_wred_profile_set
(
    CA_IN ca_device_id_t                  device_id,
    CA_IN ca_uint32_t                  id,
    CA_IN aal_l2_te_wred_profile_msk_t msk,
    CA_IN aal_l2_te_wred_profile_t     *profile
);

ca_status_t aal_l2_te_wred_profile_get
(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              id,
    CA_OUT aal_l2_te_wred_profile_t *profile
);



ca_boolean aal_l2_te_wred_profile_info_allocate
(/*CODEGEN_IGNORE_TAG*/ 
    CA_OUT ca_uint32_t *index
); 

ca_boolean aal_l2_te_wred_info_lookup
(/*CODEGEN_IGNORE_TAG*/ 
    CA_IN  ca_uint32_t *marked_dp,
    CA_IN  ca_uint32_t *unmarked_dp,
    CA_OUT ca_uint32_t *index
); 

ca_boolean aal_l2_te_wred_info_empty
(/*CODEGEN_IGNORE_TAG*/ 
void
);

ca_status_t aal_l2_te_wred_profile_info_set
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_uint32_t index, 
    CA_IN ca_uint32_t *marked_dp, 
    CA_IN ca_uint32_t *unmarked_dp
);

ca_status_t aal_l2_te_wred_profile_info_get
(/*CODEGEN_IGNORE_TAG*/
    CA_IN  ca_uint32_t index,
    CA_OUT ca_uint32_t *marked_dp,
    CA_OUT ca_uint32_t *unmarked_dp
);

ca_uint32_t aal_l2_te_wred_profile_info_ref_counter_get
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_uint32_t index
);

void aal_l2_te_wred_profile_info_ref_inc
(/*CODEGEN_IGNORE_TAG*/ 
    CA_IN ca_uint32_t index
);

void aal_l2_te_wred_profile_info_ref_dec
(/*CODEGEN_IGNORE_TAG*/ 
    CA_IN ca_uint32_t index
);

void aal_l2_te_wred_sel_info_set
(/*CODEGEN_IGNORE_TAG*/ 
    CA_IN ca_port_id_t port,
    CA_IN ca_uint32_t  queue, 
    CA_IN ca_uint32    index
);

ca_uint8_t aal_l2_te_wred_sel_info_get
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_port_id_t port, 
    CA_IN ca_uint32_t  queue
);

ca_status_t aal_l2_te_wred_debug_get(ca_device_id_t device_id);


ca_status_t aal_l2_te_init
(  /*CODEGEN_IGNORE_TAG*/ 
    CA_IN ca_device_id_t device_id
);


#endif
