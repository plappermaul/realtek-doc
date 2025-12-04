#ifndef __AAL_L2_QM_H__
#define __AAL_L2_QM_H__

#include "ca_types.h"

#define CA_AAL_MAX_VOQ_ID 0x7f                    /* 128 VoQs         */

#define CA_AAL_L2_BM_MAX_GLOBAL_BUFF_SIZE 0x7fff  /* max 32K          */
#define CA_AAL_L2_BM_MAX_VOQ_BUFF_SIZE    0x3ffff /* 256KB, 4K * 64 bytes VoQ */
#define CA_AAL_L2_QM_MAX_PORT_PRVT_PROFILE_ID 0x7 /* up to 8 profiles */
#define CA_AAL_L2_QM_PORT_PRVT_BUFF_NUM 29        /* 29 * 64, store a max size packet */

/*
** interrupt enable for QM
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd                :19;
        ca_uint32_t deq_ff_ovrflo_en      :1; 
        ca_uint32_t deq_ff_undrflo_en     :1; 
        ca_uint32_t enq_ff_ovrflo_en      :1; 
        ca_uint32_t enq_ff_undrflo_en     :1; 
        ca_uint32_t eq1_stack_err_en      :1; 
        ca_uint32_t eq0_stack_err_en      :1; 
        ca_uint32_t wr_race_err_en        :1; 
        ca_uint32_t bid_out_of_range_en   :1; 
        ca_uint32_t no_buff_drop_en       :1; 
        ca_uint32_t voqinfo_ecc_err_en    :1; 
        ca_uint32_t voqinfo_ecc_correctederr_en  :1; 
        ca_uint32_t freeq_ecc_err_en    :1; 
        ca_uint32_t freeq_ecc_correctederr_en    :1; 
#else 
        ca_uint32_t freeq_ecc_correctederr_en    :1; 
        ca_uint32_t freeq_ecc_err_en      :1; 
        ca_uint32_t voqinfo_ecc_correctederr_en  :1; 
        ca_uint32_t voqinfo_ecc_err_en    :1; 
        ca_uint32_t no_buff_drop_en       :1; 
        ca_uint32_t bid_out_of_range_en   :1; 
        ca_uint32_t wr_race_err_en        :1; 
        ca_uint32_t eq0_stack_err_en      :1; 
        ca_uint32_t eq1_stack_err_en      :1; 
        ca_uint32_t enq_ff_undrflo_en     :1; 
        ca_uint32_t enq_ff_ovrflo_en      :1; 
        ca_uint32_t deq_ff_undrflo_en     :1; 
        ca_uint32_t deq_ff_ovrflo_en      :1; 
        ca_uint32_t f_rsvd                :19;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_qm_intr_ena_msk_t;


typedef struct 
{
    ca_boolean_t freeq_ecc_correctederr_en;    
    ca_boolean_t freeq_ecc_err_en;    
    ca_boolean_t voqinfo_ecc_correctederr_en;  
    ca_boolean_t voqinfo_ecc_err_en;  
    ca_boolean_t no_buff_drop_en;       
    ca_boolean_t bid_out_of_range_en;
    ca_boolean_t wr_race_err_en;
    ca_boolean_t eq0_stack_err_en;
    ca_boolean_t eq1_stack_err_en;
    ca_boolean_t enq_ff_undrflo_en;
    ca_boolean_t enq_ff_ovrflo_en;
    ca_boolean_t deq_ff_undrflo_en;
    ca_boolean_t deq_ff_ovrflo_en;
}aal_l2_qm_intr_ena_t;


/*
** empty queue stack configurations
*/
typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd          :29;
        ca_uint32_t high_threshold  :1; 
        ca_uint32_t mid_threshold   :1; 
        ca_uint32_t low_threshold   :1; 
#else 
        ca_uint32_t low_threshold   :1; 
        ca_uint32_t mid_threshold   :1; 
        ca_uint32_t high_threshold  :1; 
        ca_uint32_t f_rsvd          :29;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_qm_eq_stack_cfg_msk_t;

typedef struct 
{
    ca_uint32_t low_threshold;
    ca_uint32_t mid_threshold;
    ca_uint32_t high_threshold;  
}aal_l2_qm_eq_stack_cfg_t;


/*
** QM VoQ infomation
*/
typedef struct
{
    ca_uint8_t  buff_len;
    ca_uint8_t  eqid;
    ca_uint8_t  eop;
    ca_uint8_t  sop;
    ca_uint8_t  cpu_flag;
    ca_uint8_t  prvt_flag;
    ca_uint16_t head_ptr;
    ca_uint16_t tail_ptr;
    ca_uint8_t  valid;
    ca_uint8_t  ecc;
}aal_l2_qm_voq_info_t;

/*
** QM free Q infomation
*/

typedef struct
{
    ca_uint8_t  buff_len;   /* valid buffer length */
    ca_uint8_t  eqid;       /* empty queue ID      */
    ca_uint8_t  eop;        /* end of packet       */
    ca_uint8_t  sop;        /* start of packet     */
    ca_uint8_t  cpu_flag;   /* CPU packet flag     */
    ca_uint8_t  prvt_flag;  /* private buffer flag */
    ca_uint16_t next_ptr;   /* next pointer        */
    ca_uint8_t  ecc;        /* ECC field           */
}aal_l2_qm_free_q_info_t;


/*
** global buffer configurations
*/

typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd             : 28;
        ca_uint32_t fe_bp_ena          : 1; 
        ca_uint32_t non_cong_threshold : 1; 
        ca_uint32_t drop_ena           : 1; 
        ca_uint32_t no_drop_threshold  : 1; 
#else 
        ca_uint32_t no_drop_threshold  : 1; 
        ca_uint32_t drop_ena           : 1; 
        ca_uint32_t non_cong_threshold : 1; 
        ca_uint32_t fe_bp_ena          : 1; 
        ca_uint32_t f_rsvd             : 28;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_qm_buff_cfg_msk_t;


typedef struct 
{
    ca_uint16_t  no_drop_threshold;    /* indicates no-drop threshold counted to number of WEIGHTed global free buffers,
                                          the number of WEIGHTed buffers for no-drop packet is this value minus 1.             */
    ca_boolean_t drop_ena;             /* enable drop or not when buffer is unavailable in En-queue processing. 
                                            TRUE  : drop in case of out of buffer 
                                            FALSE : halt traffic until next buffer available                                   */
    ca_uint16_t  non_cong_threshold;   /* indciatse non-congestion threshold counted to number of WEIGHTed global free buffer,
                                          the number of WEIGHTed buffers for non-congestion packet is this vlaue minus 1       */
    ca_boolean_t fe_bp_ena;            /* enable FE backpressure in case free buffer runs low 
                                            TRUE  : backpressure to FE when number of free buffer is less than or equal to 16  
                                            FALSE : drop when out of buffer                                                    */
}aal_l2_qm_buff_cfg_t;



/*
** empty queue configurations
*/

typedef union
{
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t f_rsvd        : 29;
        ca_uint32_t prvt_buff_num : 1;
        ca_uint32_t buff_num      : 1; 
        ca_uint32_t eq_ena        : 1; 
#else 
        ca_uint32_t eq_ena        : 1; 
        ca_uint32_t buff_num      : 1; 
        ca_uint32_t prvt_buff_num : 1;
        ca_uint32_t f_rsvd        : 29;
#endif 
    }s;
    ca_uint32_t        u32;
}aal_l2_qm_eq_cfg_msk_t;


typedef struct 
{
    ca_boolean_t eq_ena;            /* enable or disable this empty queue                      */
    ca_uint32_t  buff_num;          /* 1k buffer for EQ, 0 means 8k-1 for eq0 and 4k-1 for eq1 */
    ca_uint32_t  prvt_buff_num;     /* private buffer numbers                                  */
}aal_l2_qm_eq_cfg_t;

typedef struct
{
    ca_boolean_t unfill_done;
    ca_boolean_t idle;
    ca_boolean_t empty;
    ca_boolean_t full;
    ca_uint8_t   cnts;
}aal_l2_qm_eq_stack_status_t;


typedef struct
{
    ca_boolean_t en_q_fifo_empty;   /* buffer De-Q FIFO empty from Link List                                  */
    ca_boolean_t de_q_fifo_empty;   /* buffer En-Q FIFO empty to Link List                                    */
    ca_uint16_t  plm_non_empty;     /* bit-map of the non-empty status in packet link manager per source port */
}aal_l2_qm_status_t;


ca_status_t aal_l2_qm_init(CA_IN ca_device_id_t device_id); /*CODEGEN_IGNORE_TAG*/ 

ca_status_t aal_l2_qm_intr_ena_set
(
    CA_IN ca_device_id_t              device_id,
    CA_IN aal_l2_qm_intr_ena_msk_t msk,
    CA_IN aal_l2_qm_intr_ena_t     *cfg
);

ca_status_t aal_l2_qm_intr_ena_get
(
    CA_IN  ca_device_id_t          device_id,
    CA_OUT aal_l2_qm_intr_ena_t *cfg
);

ca_status_t aal_l2_qm_eq_stack_cfg_set
(
    ca_device_id_t                  device_id,
    ca_uint8_t                   eq_id,
    aal_l2_qm_eq_stack_cfg_msk_t msk,
    aal_l2_qm_eq_stack_cfg_t     *cfg
);

ca_status_t aal_l2_qm_eq_stack_cfg_get
(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint8_t               eq_id,
    CA_OUT aal_l2_qm_eq_stack_cfg_t *cfg
);

ca_status_t aal_l2_qm_buff_cfg_set
(
    CA_IN ca_device_id_t              device_id,
    CA_IN aal_l2_qm_buff_cfg_msk_t msk,
    CA_IN aal_l2_qm_buff_cfg_t     *cfg
);


ca_status_t aal_l2_qm_buff_cfg_get
(
    CA_IN  ca_device_id_t          device_id,
    CA_OUT aal_l2_qm_buff_cfg_t *cfg
);

ca_status_t aal_l2_qm_eq_cfg_set
(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_uint8_t             eq_id,
    CA_IN aal_l2_qm_eq_cfg_msk_t msk,
    CA_IN aal_l2_qm_eq_cfg_t     *cfg
);

ca_status_t aal_l2_qm_eq_cfg_get
(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_uint8_t         eq_id,
    CA_OUT aal_l2_qm_eq_cfg_t *cfg
);

ca_status_t aal_l2_qm_eq_free_cnt_get
(
    CA_IN  ca_device_id_t device_id,
    CA_IN  ca_uint8_t  eq_id,
    CA_OUT ca_uint32_t *free_cnt
);

ca_status_t aal_l2_qm_eq_glb_free_cnt_get
(
    CA_IN  ca_device_id_t device_id,
    CA_OUT ca_uint32_t *free_cnt
);

ca_status_t aal_l2_qm_port_prvt_sel_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  profile_id
);

ca_status_t aal_l2_qm_port_prvt_sel_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *profile_id
);

ca_status_t aal_l2_qm_port_prvt_profile_set
(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t profile_id,
    CA_IN ca_uint32_t buff_nums
);

ca_status_t aal_l2_qm_port_prvt_profile_get
(
    CA_IN  ca_device_id_t device_id,
    CA_IN  ca_uint32_t profile_id,
    CA_OUT ca_uint32_t *buff_nums
);

ca_status_t aal_l2_qm_port_prvt_buff_status_get
(
    CA_IN  ca_device_id_t device_id,
    CA_OUT ca_uint32_t *status
);

ca_status_t aal_l2_qm_port_prvt_buff_cnt_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *cnt
);


ca_status_t aal_l2_qm_eq_hdr_ptr_get
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_uint8_t   eq_id,
    CA_OUT ca_uint16_t *hdr_ptr
);

ca_status_t aal_l2_qm_eq_stack_status_get
(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_uint8_t                  eq,
    CA_OUT aal_l2_qm_eq_stack_status_t *status
);


ca_status_t aal_l2_qm_eq_cos_buf_cnt_get
(
    CA_IN  ca_device_id_t device_id,
    CA_IN  ca_uint8_t  cos,
    CA_OUT ca_uint32_t *buf_cnt
);

ca_status_t aal_l2_qm_status_get
(
    CA_IN  ca_device_id_t        device_id,
    CA_OUT aal_l2_qm_status_t *status
);

ca_status_t aal_l2_qm_voq_info_get
(
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_uint32_t          voq_id,
    CA_OUT aal_l2_qm_voq_info_t *voq_info
);

ca_status_t aal_l2_qm_freeq_get
(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_uint32_t             buff_id,
    CA_OUT aal_l2_qm_free_q_info_t *info
);



#endif
