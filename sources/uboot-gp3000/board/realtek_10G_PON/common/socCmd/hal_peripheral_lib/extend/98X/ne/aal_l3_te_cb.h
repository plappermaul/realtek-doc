#ifndef __AAL_L3_TE_CB_H__
#define __AAL_L3_TE_CB_H__

#include "ca_types.h"

typedef struct{
    /*ca_boolean_t scan_cycle_err;*/
    ca_boolean_t port_freecnt_ovfl;
    ca_boolean_t port_freecnt_udfl;
    ca_boolean_t voq_pcnt_ovfl;
    ca_boolean_t voq_pcnt_udfl;    
    ca_boolean_t voq_bufcnt_ovfl;
    ca_boolean_t voq_bufcnt_udfl;
}aal_l3_te_cb_intr_t;

typedef union{                              
    struct{                                 
        /*ca_uint32_t scan_cycle_err    : 1;  */
        ca_uint32_t port_freecnt_ovfl : 1;  
        ca_uint32_t port_freecnt_udfl : 1;        
        ca_uint32_t voq_pcnt_ovfl   : 1;  
        ca_uint32_t voq_pcnt_udfl   : 1;  
        ca_uint32_t voq_bufcnt_ovfl   : 1;  
        ca_uint32_t voq_bufcnt_udfl   : 1;  
    }s;                                     
    ca_uint32_t u32;                        
}aal_l3_te_cb_intr_mask_t; 

typedef struct{
    ca_uint8_t voq_of_pkt_err;
    ca_uint8_t voq_of_buf_err;
}aal_l3_te_cb_err_info_t;

typedef struct{
    ca_boolean_t scan_ena;/*Scan enable to update WRED buffer usage*/
    ca_uint16_t  scan_cycle_unit;
    ca_uint16_t  scan_cycle_per_run;/* Scan cycles per run to update WRED buffer usage,It is equal to (scan_cycle_per_run + 1) * 1024	cycles in core clock */
}aal_l3_te_cb_ctrl_t;

typedef union{
    struct{
        ca_uint32_t scan_ena            : 1;
        ca_uint32_t scan_cycle_unit     : 1;
        ca_uint32_t scan_cycle_per_run  : 1;
    }s;
    ca_uint32_t u32;
}aal_l3_te_cb_ctrl_mask_t;

typedef struct{
    ca_uint16_t global_drop_pool_bmp;
    ca_uint16_t spid_drop_bmp;/*bit 0 DMA, bit 1 WAN*/
    ca_uint16_t dpid_drop_bmp;
    ca_uint32_t voq0_31_drop_bmp;
    ca_uint32_t voq32_63_drop_bmp;
    ca_uint32_t voq64_95_drop_bmp;
    ca_uint32_t voq96_127_drop_bmp;
    ca_uint8_t  wan_src_pri_drop_cnt_bmp;
}aal_l3_te_cb_td_sts_t;


typedef struct{
    ca_uint32_t voq0_31_bmp;
    ca_uint32_t voq32_63_bmp;
    ca_uint32_t voq64_95_bmp;
    ca_uint32_t voq96_127_bmp;
}aal_l3_te_cb_no_prvt_buf_sts_t;

typedef struct{
    ca_uint16_t threshold;
}aal_l3_te_cb_eq_threshold_t;



typedef struct{
    ca_uint16_t threshold_hi;
    ca_uint16_t threshold_lo;
}aal_l3_te_cb_threshold_t;

typedef union{
    struct{
        ca_uint32_t prvt             : 1;          
        ca_uint32_t threshold_hi     : 1;          
        ca_uint32_t threshold_lo     : 1;          
        ca_uint32_t grp_threshold_hi : 1;          
        ca_uint32_t grp_threshold_lo : 1;          
    }s;
    ca_uint32_t u32;
}aal_l3_te_cb_comb_threshold_mask_t;


typedef struct{
    ca_uint16_t prvt;
    ca_uint16_t threshold_hi;
    ca_uint16_t threshold_lo;
    ca_uint16_t grp_threshold_hi;
    ca_uint16_t grp_threshold_lo;
}aal_l3_te_cb_comb_threshold_t;

typedef struct{
    ca_boolean_t    cnt1_msb;
    ca_uint16_t     cnt1;
    ca_boolean_t    cnt0_msb;
    ca_uint16_t     cnt0;
}aal_l3_te_cb_free_buf_cnt_t;

typedef union{   
    struct{
        ca_uint32_t     cnt1_msb :1;
        ca_uint32_t     cnt1     :1;
        ca_uint32_t     cnt0_msb :1;
        ca_uint32_t     cnt0     :1;
    }s;
    ca_uint32_t   u32;
}aal_l3_te_cb_free_buf_cnt_mask_t;


typedef struct{
    ca_uint16_t eq_prof0_threshold0;  
    ca_uint16_t eq_prof0_threshold1;  
    ca_uint16_t eq_prof1_threshold0;  
    ca_uint16_t eq_prof1_threshold1;  
    ca_uint16_t eq_prof2_threshold0;  
    ca_uint16_t eq_prof2_threshold1;  
    ca_uint16_t eq_prof3_threshold0;  
    ca_uint16_t eq_prof3_threshold1;  
    ca_uint16_t eq_prof4_threshold0;  
    ca_uint16_t eq_prof4_threshold1;  
    ca_uint16_t eq_prof5_threshold0;  
    ca_uint16_t eq_prof5_threshold1;  
    ca_uint16_t eq_prof6_threshold0;  
    ca_uint16_t eq_prof6_threshold1;  
    ca_uint16_t eq_prof7_threshold0;  
    ca_uint16_t eq_prof7_threshold1;  
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
    ca_uint16_t voq_grp_threshold_profile0_hi;        
    ca_uint16_t voq_grp_threshold_profile0_lo;        
    ca_uint16_t voq_grp_threshold_profile1_hi;        
    ca_uint16_t voq_grp_threshold_profile1_lo;        
    ca_uint16_t voq_grp_threshold_profile2_hi;        
    ca_uint16_t voq_grp_threshold_profile2_lo;        
    ca_uint16_t voq_grp_threshold_profile3_hi;        
    ca_uint16_t voq_grp_threshold_profile3_lo;        
    ca_uint16_t voq_grp_threshold_profile4_hi;        
    ca_uint16_t voq_grp_threshold_profile4_lo;        
    ca_uint16_t voq_grp_threshold_profile5_hi;        
    ca_uint16_t voq_grp_threshold_profile5_lo;        
    ca_uint16_t voq_grp_threshold_profile6_hi;        
    ca_uint16_t voq_grp_threshold_profile6_lo;        
    ca_uint16_t voq_grp_threshold_profile7_hi;        
    ca_uint16_t voq_grp_threshold_profile7_lo;      
    ca_uint16_t voq_private_buf_profile0;
    ca_uint16_t voq_private_buf_profile1;
    ca_uint16_t voq_private_buf_profile2;
    ca_uint16_t voq_private_buf_profile3;
    ca_uint16_t voq_private_buf_profile4;
    ca_uint16_t voq_private_buf_profile5;
    ca_uint16_t voq_private_buf_profile6;
    ca_uint16_t voq_private_buf_profile7;
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
    ca_uint16_t src_pri_threshold_hi_0;                 
    ca_uint16_t src_pri_threshold_lo_0;                 
    ca_uint16_t src_pri_threshold_hi_1;                 
    ca_uint16_t src_pri_threshold_lo_1;                 
    ca_uint16_t src_pri_threshold_hi_2;                 
    ca_uint16_t src_pri_threshold_lo_2;                 
    ca_uint16_t src_pri_threshold_hi_3;      
    ca_uint16_t src_pri_threshold_lo_3;      
    ca_uint16_t src_pri_threshold_hi_4;   
    ca_uint16_t src_pri_threshold_lo_4;   
    ca_uint16_t src_pri_threshold_hi_5;   
    ca_uint16_t src_pri_threshold_lo_5;   
    ca_uint16_t src_pri_threshold_hi_6;   
    ca_uint16_t src_pri_threshold_lo_6;   
    ca_uint16_t src_pri_threshold_hi_7;   
    ca_uint16_t src_pri_threshold_lo_7;   
    ca_uint16_t port00_free_buf_cnt0;        
    ca_uint16_t port01_free_buf_cnt0;        
    ca_uint16_t port02_free_buf_cnt0;        
    ca_uint16_t port03_free_buf_cnt0;        
    ca_uint16_t port04_free_buf_cnt0;        
    ca_uint16_t port05_free_buf_cnt0;        
    ca_uint16_t port06_free_buf_cnt0;        
    ca_uint16_t port07_free_buf_cnt0;        
    ca_uint16_t port08_free_buf_cnt0;        
    ca_uint16_t port09_free_buf_cnt0;        
    ca_uint16_t port10_free_buf_cnt0;        
    ca_uint16_t port11_free_buf_cnt0;        
    ca_uint16_t port12_free_buf_cnt0;        
    ca_uint16_t port13_free_buf_cnt0;        
    ca_uint16_t port14_free_buf_cnt0;        
    ca_uint16_t port15_free_buf_cnt0;        
    ca_uint16_t port00_free_buf_cnt1;        
    ca_uint16_t port01_free_buf_cnt1;        
    ca_uint16_t port02_free_buf_cnt1;        
    ca_uint16_t port03_free_buf_cnt1;        
    ca_uint16_t port04_free_buf_cnt1;        
    ca_uint16_t port05_free_buf_cnt1;        
    ca_uint16_t port06_free_buf_cnt1;        
    ca_uint16_t port07_free_buf_cnt1;        
    ca_uint16_t port08_free_buf_cnt1;        
    ca_uint16_t port09_free_buf_cnt1;        
    ca_uint16_t port10_free_buf_cnt1;        
    ca_uint16_t port11_free_buf_cnt1;        
    ca_uint16_t port12_free_buf_cnt1;        
    ca_uint16_t port13_free_buf_cnt1;        
    ca_uint16_t port14_free_buf_cnt1;        
    ca_uint16_t port15_free_buf_cnt1;
    ca_uint16_t deep_q_port_threshold_profile0;
    ca_uint16_t deep_q_port_threshold_profile1;
    ca_uint16_t deep_q_port_threshold_profile2;
    ca_uint16_t deep_q_port_threshold_profile3;                                  
    ca_uint16_t deep_q_voq_threshold_profile0;
    ca_uint16_t deep_q_voq_threshold_profile1;
    ca_uint16_t deep_q_voq_threshold_profile2;
    ca_uint16_t deep_q_voq_threshold_profile3;
    ca_uint16_t deep_q_voq_threshold_profile4;
    ca_uint16_t deep_q_voq_threshold_profile5;
    ca_uint16_t deep_q_voq_threshold_profile6;
    ca_uint16_t deep_q_voq_threshold_profile7; 
}aal_l3_te_cb_all_threshold_t;


ca_status_t  aal_l3_te_cb_intr_ena_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l3_te_cb_intr_mask_t  mask  ,
    CA_IN aal_l3_te_cb_intr_t       *cfg
);

ca_status_t  aal_l3_te_cb_intr_ena_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l3_te_cb_intr_t       *cfg
);

ca_status_t  aal_l3_te_cb_intr_status_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l3_te_cb_intr_mask_t  mask  ,
    CA_IN aal_l3_te_cb_intr_t       *cfg
);

ca_status_t  aal_l3_te_cb_intr_status_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l3_te_cb_intr_t       *cfg
);

ca_status_t aal_l3_te_cb_err_info_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l3_te_cb_err_info_t    *err_info
);

ca_status_t aal_l3_te_cb_ctrl_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l3_te_cb_ctrl_mask_t    mask,
    CA_IN aal_l3_te_cb_ctrl_t         *cfg
);

ca_status_t aal_l3_te_cb_ctrl_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l3_te_cb_ctrl_t         *cfg
);

ca_status_t aal_l3_te_cb_td_sts_get(
    CA_IN ca_device_id_t   device_id,
    CA_OUT aal_l3_te_cb_td_sts_t *sts
);

ca_status_t aal_l3_te_cb_no_prvt_buf_sts_get(    
    CA_IN ca_device_id_t                       device_id,
    CA_OUT aal_l3_te_cb_no_prvt_buf_sts_t   *sts
);
ca_status_t aal_l3_te_cb_pool_profile_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint8_t                  profile_id,
    CA_IN aal_l3_te_cb_eq_threshold_t    *cfg
);

ca_status_t aal_l3_te_cb_pool_profile_get(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint8_t                  profile_id,
    CA_OUT aal_l3_te_cb_eq_threshold_t    *cfg
);

ca_status_t aal_l3_te_cb_port_threshold_profile_set(
    CA_IN ca_device_id_t                           device_id,
    CA_IN ca_uint8_t                            profile_id,
    CA_IN aal_l3_te_cb_comb_threshold_mask_t    mask,
    CA_IN aal_l3_te_cb_comb_threshold_t         *cfg
);

ca_status_t aal_l3_te_cb_port_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l3_te_cb_comb_threshold_t  *cfg
);


ca_status_t aal_l3_te_cb_port_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l3_te_cb_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
);

ca_status_t aal_l3_te_cb_voq_threshold_profile_set(
    CA_IN ca_device_id_t                           device_id,
    CA_IN ca_uint8_t                            profile_id,
    CA_IN aal_l3_te_cb_comb_threshold_mask_t    mask,
    CA_IN aal_l3_te_cb_comb_threshold_t         *cfg
);

ca_status_t aal_l3_te_cb_voq_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l3_te_cb_comb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_voq_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_IN ca_uint8_t                profile_id
);

ca_status_t aal_l3_te_cb_voq_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_OUT ca_uint8_t               *profile_id
);

ca_status_t aal_l3_te_cb_port_free_buf_cnt_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN aal_l3_te_cb_free_buf_cnt_mask_t mask,
    CA_IN aal_l3_te_cb_free_buf_cnt_t  *cfg
);

ca_status_t aal_l3_te_cb_port_free_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT aal_l3_te_cb_free_buf_cnt_t  *cfg
);

ca_status_t aal_l3_te_cb_voq_buf_cnt_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN aal_l3_te_cb_free_buf_cnt_t  *cfg
);

ca_status_t aal_l3_te_cb_voq_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT aal_l3_te_cb_free_buf_cnt_t  *cfg
);


ca_status_t aal_l3_te_cb_voq_max_buf_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN ca_uint16_t                   cfg
);

ca_status_t aal_l3_te_cb_voq_max_buf_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT ca_uint16_t                  *cfg
);


ca_status_t aal_l3_te_cb_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_IN aal_l3_te_cb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,    
    CA_IN ca_uint8_t                pri,
    CA_OUT aal_l3_te_cb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_src_ctrl_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_boolean_t              wan_src_pri
);

ca_status_t aal_l3_te_cb_src_ctrl_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT ca_boolean_t             *wan_src_pri
);

ca_status_t aal_l3_te_cb_src_port_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN aal_l3_te_cb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_src_port_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT aal_l3_te_cb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_src_port_buf_cnt_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN ca_uint16_t                   cfg
);

ca_status_t aal_l3_te_cb_src_port_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT ca_uint16_t                  *cfg
);

ca_status_t aal_l3_te_cb_src_pri_buf_cnt_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    pri_id,
    CA_IN ca_uint16_t                   cfg
);

ca_status_t aal_l3_te_cb_src_pri_buf_cnt_get(
    CA_IN ca_device_id_t                   device_id,    
    CA_IN ca_uint8_t                    pri_id,
    CA_OUT ca_uint16_t                  *cfg
);
ca_status_t aal_l3_te_cb_dqsch_port_thrsh_profile_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    profile_id,
    CA_IN aal_l3_te_cb_threshold_t        *cfg
);

ca_status_t aal_l3_te_cb_dqsch_port_thrsh_profile_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    profile_id,
    CA_OUT aal_l3_te_cb_threshold_t        *cfg
);
ca_status_t aal_l3_te_cb_dqsch_port_thrsh_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN ca_uint8_t                    profile_id
);

ca_status_t aal_l3_te_cb_dqsch_port_thrsh_profile_sel_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT ca_uint8_t                   *profile_id
);


ca_status_t aal_l3_te_cb_dqsch_voq_thrsh_profile_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    profile_id,
    CA_IN aal_l3_te_cb_threshold_t        *cfg
);

ca_status_t aal_l3_te_cb_dqsch_voq_thrsh_profile_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    profile_id,
    CA_OUT aal_l3_te_cb_threshold_t        *cfg
);
ca_status_t aal_l3_te_cb_dqsch_voq_thrsh_profile_sel_set(/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN ca_uint8_t                    profile_id
);

ca_status_t aal_l3_te_cb_dqsch_voq_thrsh_profile_sel_get(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT ca_uint8_t                   *profile_id
);

ca_status_t aal_l3_te_cb_all_threshold_get(
    CA_IN ca_device_id_t                   device_id,
    CA_OUT aal_l3_te_cb_all_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_src_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_IN aal_l3_te_cb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_src_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_OUT aal_l3_te_cb_threshold_t  *cfg
);

ca_status_t aal_l3_te_cb_voq_pcnt_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_OUT ca_uint16_t                *cfg
);

ca_status_t aal_l3_te_cb_init(CA_IN ca_device_id_t device_id);

#endif

