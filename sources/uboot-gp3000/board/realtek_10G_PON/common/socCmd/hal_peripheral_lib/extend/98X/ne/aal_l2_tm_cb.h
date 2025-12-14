#ifndef __AAL_L2_TM_CB_H__
#define __AAL_L2_TM_CB_H__
#include "ca_types.h"

#define __HAVE_VOQ_BUF_LEN
#ifdef __HAVE_VOQ_BUF_LEN

typedef struct
{
    ca_uint32_t ref_counter;
    ca_uint32_t size;           /* voq buffer size */
}aal_l2_tm_voq_buff_profile_t;

ca_boolean aal_l2_tm_voq_buff_profile_lookup
(/*CODEGEN_IGNORE_TAG*/ 
    CA_IN  ca_uint32_t buff_size,
    CA_OUT ca_uint32_t *index
);
ca_boolean aal_l2_tm_voq_buff_profile_allocate
(/*CODEGEN_IGNORE_TAG*/
    CA_OUT ca_uint32_t *index
);

ca_status_t aal_l2_tm_voq_buff_profile_set
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_uint32_t index,
    CA_IN ca_uint32_t buff_size
);

ca_status_t aal_l2_tm_voq_buff_profile_get
(/*CODEGEN_IGNORE_TAG*/
    CA_IN  ca_uint32_t index,
    CA_OUT ca_uint32_t *buff_size
);
ca_uint32_t aal_l2_tm_voq_buff_profile_ref_counter_get
(/*CODEGEN_IGNORE_TAG*/
    CA_OUT ca_uint32_t index
);

void aal_l2_tm_voq_buff_profile_ref_inc
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_uint32_t index
);

void aal_l2_tm_voq_buff_profile_ref_dec
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_uint32_t index
);

void aal_l2_tm_voq_buff_profile_sel_set
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_port_id_t port,
    CA_IN ca_uint32_t  queue,
    CA_IN ca_uint32    index
);

ca_uint8_t aal_l2_tm_voq_buff_profile_sel_get
(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_port_id_t port, 
    CA_IN ca_uint32_t  queue
);

ca_status_t aal_l2_tm_cb_voq_buff_debug_get
(
    CA_IN ca_device_id_t device_id
);

#endif


typedef struct{
    /*ca_boolean_t scan_cycle_err;*/
    ca_boolean_t port_freecnt_ovfl;
    ca_boolean_t port_freecnt_udfl;
    ca_boolean_t voq_bufcnt_ovfl;
    ca_boolean_t voq_bufcnt_udfl;
}aal_l2_tm_cb_intr_t;

typedef union{                              
    struct{                                 
        /*ca_uint32_t scan_cycle_err    : 1; */ 
        ca_uint32_t port_freecnt_ovfl : 1;  
        ca_uint32_t port_freecnt_udfl : 1;  
        ca_uint32_t voq_bufcnt_ovfl   : 1;  
        ca_uint32_t voq_bufcnt_udfl   : 1;  
    }s;                                     
    ca_uint32_t u32;                        
}aal_l2_tm_cb_intr_mask_t;                

typedef struct{
    ca_boolean_t scan_ena;/*Scan enable to update WRED buffer usage*/
    ca_uint16_t  scan_cycle_unit;
    ca_uint16_t  scan_cycle_per_run;/* Scan cycles per run to update WRED buffer usage,It is equal to (scan_cycle_per_run + 1) * 1024	cycles in core clock */
}aal_l2_tm_cb_ctrl_t;

typedef union{
    struct{
        ca_uint32_t scan_ena            : 1;
        ca_uint32_t scan_cycle_unit     : 1;
        ca_uint32_t scan_cycle_per_run  : 1;
    }s;
    ca_uint32_t u32;
}aal_l2_tm_cb_ctrl_mask_t;

typedef struct{
    ca_uint16_t global_drop_pri_bmp;
    ca_uint16_t spid_drop_bmp;
    ca_uint16_t dpid_drop_bmp;
    ca_uint32_t voq0_31_drop_bmp;
    ca_uint32_t voq32_63_drop_bmp;
    ca_uint32_t voq64_95_drop_bmp;
    ca_uint32_t voq96_127_drop_bmp;
    ca_uint8_t   src_pri_drop_cnt0_bmp;
    ca_uint8_t   src_pri_drop_cnt1_bmp;
}aal_l2_tm_cb_td_sts_t;

typedef struct{
    ca_uint16_t threshold_hi;
    ca_uint16_t threshold_lo;
}aal_l2_tm_cb_threshold_t;

typedef union{
    struct{
        ca_uint32_t threshold_hi     : 1;          
        ca_uint32_t threshold_lo     : 1;          
        ca_uint32_t grp_threshold_hi : 1;          
        ca_uint32_t grp_threshold_lo : 1;          
    }s;
    ca_uint32_t u32;
}aal_l2_tm_cb_comb_threshold_mask_t;


typedef struct{
    ca_uint16_t threshold_hi;
    ca_uint16_t threshold_lo;
    ca_uint16_t grp_threshold_hi;
    ca_uint16_t grp_threshold_lo;
}aal_l2_tm_cb_comb_threshold_t;

typedef struct{
    ca_boolean_t    cnt1_msb;
    ca_uint16_t     cnt1;
    ca_boolean_t    cnt0_msb;
    ca_uint16_t     cnt0;
}aal_l2_tm_cb_free_buf_cnt_t;

typedef union{
    struct{                    
        ca_uint32_t    src1_pri_en    : 1 ;    
        ca_uint32_t    src1_pri_nspid : 1 ;
        ca_uint32_t    src0_pri_en    : 1 ;    
        ca_uint32_t    src0_pri_nspid : 1 ;
    }s;
    ca_uint32_t u32;
}aal_l2_tm_cb_src_ctrl_mask_t;          



typedef struct{
    ca_boolean_t    src1_pri_en;
    ca_uint8_t      src1_pri_nspid;
    ca_boolean_t    src0_pri_en;
    ca_uint8_t      src0_pri_nspid;
}aal_l2_tm_cb_src_ctrl_t;

typedef struct{
    ca_uint16_t glb_threshold_hi;
    ca_uint16_t glb_threshold_lo;
    ca_uint16_t port_threshold_profile0_hi; 
    ca_uint16_t port_threshold_profile0_lo; 
    ca_uint16_t port_threshold_profile1_hi; 
    ca_uint16_t port_threshold_profile1_lo; 
    ca_uint16_t port_threshold_profile2_hi; 
    ca_uint16_t port_threshold_profile2_lo; 
    ca_uint16_t port_threshold_profile3_hi; 
    ca_uint16_t port_threshold_profile3_lo; 
    ca_uint16_t port_grp_threshold_profile0_hi;
    ca_uint16_t port_grp_threshold_profile0_lo;
    ca_uint16_t port_grp_threshold_profile1_hi;
    ca_uint16_t port_grp_threshold_profile1_lo;    
    ca_uint16_t port_grp_threshold_profile2_hi;
    ca_uint16_t port_grp_threshold_profile2_lo;
    ca_uint16_t port_grp_threshold_profile3_hi;
    ca_uint16_t port_grp_threshold_profile3_lo;
    ca_uint32_t port_profile_sel;
    ca_uint16_t voq_threshold_profile0_hi;
    ca_uint16_t voq_threshold_profile0_lo;
    ca_uint16_t voq_threshold_profile1_hi;
    ca_uint16_t voq_threshold_profile1_lo;
    ca_uint16_t voq_threshold_profile2_hi;
    ca_uint16_t voq_threshold_profile2_lo;
    ca_uint16_t voq_threshold_profile3_hi;
    ca_uint16_t voq_threshold_profile3_lo;
    ca_uint16_t voq_threshold_profile4_hi;
    ca_uint16_t voq_threshold_profile4_lo;
    ca_uint16_t voq_threshold_profile5_hi;
    ca_uint16_t voq_threshold_profile5_lo;
    ca_uint16_t voq_threshold_profile6_hi;
    ca_uint16_t voq_threshold_profile6_lo;
    ca_uint16_t voq_threshold_profile7_hi;
    ca_uint16_t voq_threshold_profile7_lo; 
    ca_uint16_t voq_grp_threshold_hi_0;
    ca_uint16_t voq_grp_threshold_lo_0;
    ca_uint16_t voq_grp_threshold_hi_1;
    ca_uint16_t voq_grp_threshold_lo_1;
    ca_uint16_t voq_grp_threshold_hi_2;
    ca_uint16_t voq_grp_threshold_lo_2;
    ca_uint16_t voq_grp_threshold_hi_3;
    ca_uint16_t voq_grp_threshold_lo_3;
    ca_uint16_t voq_grp_threshold_hi_4;
    ca_uint16_t voq_grp_threshold_lo_4;
    ca_uint16_t voq_grp_threshold_hi_5;
    ca_uint16_t voq_grp_threshold_lo_5;
    ca_uint16_t voq_grp_threshold_hi_6;
    ca_uint16_t voq_grp_threshold_lo_6;
    ca_uint16_t voq_grp_threshold_hi_7;
    ca_uint16_t voq_grp_threshold_lo_7;
    ca_uint16_t glb_pri_threshold_hi_0;
    ca_uint16_t glb_pri_threshold_lo_0;
    ca_uint16_t glb_pri_threshold_hi_1;
    ca_uint16_t glb_pri_threshold_lo_1;
    ca_uint16_t glb_pri_threshold_hi_2;
    ca_uint16_t glb_pri_threshold_lo_2;
    ca_uint16_t glb_pri_threshold_hi_3;
    ca_uint16_t glb_pri_threshold_lo_3;
    ca_uint16_t glb_pri_threshold_hi_4;
    ca_uint16_t glb_pri_threshold_lo_4;
    ca_uint16_t glb_pri_threshold_hi_5;
    ca_uint16_t glb_pri_threshold_lo_5;
    ca_uint16_t glb_pri_threshold_hi_6;
    ca_uint16_t glb_pri_threshold_lo_6;
    ca_uint16_t glb_pri_threshold_hi_7;
    ca_uint16_t glb_pri_threshold_lo_7;   
    ca_uint16_t src_port_threshold_profile0_hi;             
    ca_uint16_t src_port_threshold_profile0_lo;  
    ca_uint16_t src_port_threshold_profile1_hi;  
    ca_uint16_t src_port_threshold_profile1_lo;  
    ca_uint16_t src_port_threshold_profile2_hi;  
    ca_uint16_t src_port_threshold_profile2_lo;  
    ca_uint16_t src_port_threshold_profile3_hi;  
    ca_uint16_t src_port_threshold_profile3_lo;  
    ca_uint32_t src_port_sel;
    ca_uint16_t src0_pri_threshold_hi_0;
    ca_uint16_t src0_pri_threshold_lo_0;
    ca_uint16_t src0_pri_threshold_hi_1;
    ca_uint16_t src0_pri_threshold_lo_1;
    ca_uint16_t src0_pri_threshold_hi_2;
    ca_uint16_t src0_pri_threshold_lo_2;
    ca_uint16_t src0_pri_threshold_hi_3;
    ca_uint16_t src0_pri_threshold_lo_3;
    ca_uint16_t src0_pri_threshold_hi_4;
    ca_uint16_t src0_pri_threshold_lo_4;
    ca_uint16_t src0_pri_threshold_hi_5;
    ca_uint16_t src0_pri_threshold_lo_5;
    ca_uint16_t src0_pri_threshold_hi_6;
    ca_uint16_t src0_pri_threshold_lo_6;
    ca_uint16_t src0_pri_threshold_hi_7;
    ca_uint16_t src0_pri_threshold_lo_7;
    ca_uint16_t src1_pri_threshold_hi_0;
    ca_uint16_t src1_pri_threshold_lo_0;
    ca_uint16_t src1_pri_threshold_hi_1;
    ca_uint16_t src1_pri_threshold_lo_1;
    ca_uint16_t src1_pri_threshold_hi_2;
    ca_uint16_t src1_pri_threshold_lo_2;
    ca_uint16_t src1_pri_threshold_hi_3;
    ca_uint16_t src1_pri_threshold_lo_3;
    ca_uint16_t src1_pri_threshold_hi_4;
    ca_uint16_t src1_pri_threshold_lo_4;
    ca_uint16_t src1_pri_threshold_hi_5;
    ca_uint16_t src1_pri_threshold_lo_5;
    ca_uint16_t src1_pri_threshold_hi_6;
    ca_uint16_t src1_pri_threshold_lo_6;
    ca_uint16_t src1_pri_threshold_hi_7;
    ca_uint16_t src1_pri_threshold_lo_7;
    ca_uint32_t src_ctrl;
    ca_uint32_t port00_free_buffer_cnt; 
    ca_uint32_t port01_free_buffer_cnt; 
    ca_uint32_t port02_free_buffer_cnt; 
    ca_uint32_t port03_free_buffer_cnt; 
    ca_uint32_t port04_free_buffer_cnt; 
    ca_uint32_t port05_free_buffer_cnt; 
    ca_uint32_t port06_free_buffer_cnt; 
    ca_uint32_t port07_free_buffer_cnt; 
    ca_uint32_t port08_free_buffer_cnt; 
    ca_uint32_t port09_free_buffer_cnt; 
    ca_uint32_t port10_free_buffer_cnt; 
    ca_uint32_t port11_free_buffer_cnt; 
    ca_uint32_t port12_free_buffer_cnt; 
    ca_uint32_t port13_free_buffer_cnt; 
    ca_uint32_t port14_free_buffer_cnt; 
    ca_uint32_t port15_free_buffer_cnt;
    ca_uint32_t dqsch_eq0_threshold;
    ca_uint32_t dqsch_port_profile0;
    ca_uint32_t dqsch_port_profile1;
    ca_uint32_t dqsch_port_profile2;
    ca_uint32_t dqsch_port_profile3;  
    ca_uint32_t dqsch_port_profile_sel;     
    ca_uint32_t dqsch_voq_profile0;
    ca_uint32_t dqsch_voq_profile1;
    ca_uint32_t dqsch_voq_profile2;
    ca_uint32_t dqsch_voq_profile3;
    ca_uint32_t dqsch_voq_profile4;
    ca_uint32_t dqsch_voq_profile5;
    ca_uint32_t dqsch_voq_profile6;
    ca_uint32_t dqsch_voq_profile7;

}aal_l2_tm_cb_all_threshold_t;


ca_status_t  aal_l2_tm_cb_intr_ena_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l2_tm_cb_intr_mask_t  mask  ,
    CA_IN aal_l2_tm_cb_intr_t       *cfg
);

ca_status_t  aal_l2_tm_cb_intr_ena_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l2_tm_cb_intr_t       *cfg
);

ca_status_t  aal_l2_tm_cb_intr_status_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l2_tm_cb_intr_mask_t  mask  ,
    CA_IN aal_l2_tm_cb_intr_t       *cfg
);

ca_status_t  aal_l2_tm_cb_intr_status_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l2_tm_cb_intr_t       *cfg
);

ca_status_t aal_l2_tm_cb_err_info_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT ca_uint16_t               *err_info
);

ca_status_t aal_l2_tm_cb_ctrl_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l2_tm_cb_ctrl_mask_t    mask,
    CA_IN aal_l2_tm_cb_ctrl_t         *cfg
);

ca_status_t aal_l2_tm_cb_ctrl_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l2_tm_cb_ctrl_t         *cfg
);

ca_status_t aal_l2_tm_cb_td_sts_get(
    CA_IN ca_device_id_t   device_id,
    CA_OUT aal_l2_tm_cb_td_sts_t *sts
);

ca_status_t aal_l2_tm_cb_port_threshold_profile_set(
    CA_IN ca_device_id_t                           device_id,
    CA_IN ca_uint8_t                            profile_id,
    CA_IN aal_l2_tm_cb_comb_threshold_mask_t    mask,
    CA_IN aal_l2_tm_cb_comb_threshold_t         *cfg
);

ca_status_t aal_l2_tm_cb_port_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_comb_threshold_t  *cfg
);


ca_status_t aal_l2_tm_cb_port_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l2_tm_cb_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
);

ca_status_t aal_l2_tm_cb_voq_threshold_profile_set(
    CA_IN ca_device_id_t                           device_id,
    CA_IN ca_uint8_t                            profile_id,
    CA_IN aal_l2_tm_cb_comb_threshold_mask_t    mask,
    CA_IN aal_l2_tm_cb_comb_threshold_t         *cfg
);

ca_status_t aal_l2_tm_cb_voq_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_comb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_voq_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l2_tm_cb_voq_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_OUT ca_uint8_t               *profile_id
);

ca_status_t aal_l2_tm_cb_port_free_buf_cnt_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN aal_l2_tm_cb_free_buf_cnt_t  *cfg
);

ca_status_t aal_l2_tm_cb_port_free_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT aal_l2_tm_cb_free_buf_cnt_t  *cfg
);

ca_status_t aal_l2_tm_cb_voq_buf_cnt_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN aal_l2_tm_cb_free_buf_cnt_t  *cfg
);

ca_status_t aal_l2_tm_cb_voq_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT aal_l2_tm_cb_free_buf_cnt_t  *cfg
);


ca_status_t aal_l2_tm_cb_voq_max_buf_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN ca_uint16_t                   cfg
);

ca_status_t aal_l2_tm_cb_voq_max_buf_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT ca_uint16_t                  *cfg
);

ca_status_t aal_l2_tm_cb_global_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_global_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,    
    CA_IN ca_uint8_t                pri,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_src_ctrl_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN aal_l2_tm_cb_src_ctrl_mask_t mask,
    CA_IN aal_l2_tm_cb_src_ctrl_t   *cfg
);

ca_status_t aal_l2_tm_cb_src_ctrl_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_tm_cb_src_ctrl_t  *cfg
);

ca_status_t aal_l2_tm_cb_src_port_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_src_port_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_src_port_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l2_tm_cb_src_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
);

ca_status_t aal_l2_tm_cb_src0_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_src0_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_src1_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_src1_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

/*
ca_status_t aal_l2_tm_cb_src_pri_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l2_tm_cb_src_pri_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_OUT ca_uint8_t               *profile_id
);
*/
ca_status_t aal_l2_tm_cb_src_port_buf_cnt_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN ca_uint16_t                   cfg
);

ca_status_t aal_l2_tm_cb_src_port_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT ca_uint16_t                  *cfg
);

ca_status_t aal_l2_tm_cb_src_pri_buf_cnt_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_boolean_t                  indicate_ena,
    CA_IN ca_uint8_t                    pri_id,
    CA_IN ca_uint32_t                   cfg
);

ca_status_t aal_l2_tm_cb_src_pri_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,    
    CA_IN ca_boolean_t                  indicate_ena,
    CA_IN ca_uint8_t                    pri_id,
    CA_OUT ca_uint32_t                  *cfg
);

ca_status_t aal_l2_tm_cb_deep_q_sched_pool_threshold_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,    
    CA_IN ca_uint32_t                   cfg

);

ca_status_t aal_l2_tm_cb_deep_q_sched_pool_threshold_get(
    CA_IN ca_device_id_t                   device_id,    
    CA_OUT ca_uint32_t                  *cfg
);

ca_status_t aal_l2_tm_cb_deep_q_port_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_deep_q_port_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_deep_q_port_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l2_tm_cb_deep_q_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
);

ca_status_t aal_l2_tm_cb_deep_q_voq_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_deep_q_voq_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
);

ca_status_t aal_l2_tm_cb_deep_q_voq_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l2_tm_cb_deep_q_voq_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_OUT ca_uint8_t               *profile_id
);
ca_status_t aal_l2_tm_cb_all_threshold_get(
    CA_IN ca_device_id_t                   device_id,
    CA_OUT aal_l2_tm_cb_all_threshold_t *cfg
);
/*
ca_status_t aal_l2_tm_cb_abr_td_drop_status_port_bmp_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT ca_uint16_t              *bmp
);

ca_status_t aal_l2_tm_cm_abr_global_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint16_t               threshold
);

ca_status_t aal_l2_tm_cm_abr_global_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT ca_uint16_t              *threshold
);

ca_status_t aal_l2_tm_cm_abr_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN ca_uint16_t               threshold
);

ca_status_t aal_l2_tm_cm_abr_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT ca_uint16_t              *threshold
);

ca_status_t aal_l2_tm_cm_abr_threshold_profile_port_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t               profile_id
);

ca_status_t aal_l2_tm_cm_abr_threshold_profile_port_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t              *profile_id
);
*/

ca_status_t aal_l2_tm_cb_init(ca_device_id_t device_id);

#endif /* __AAL_L2_TM_CB_H__ */

