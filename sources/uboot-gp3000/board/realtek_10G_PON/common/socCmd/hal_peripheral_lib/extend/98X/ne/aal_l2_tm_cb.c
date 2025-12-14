/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_l2_tm_cb.c: Hardware Abstraction Layer for L2 TE CB driver to access hardware regsiters
 *
 */

#if 0//yocto
#include <generated/ca_ne_autoconf.h>
#else//sd1 uboot
#include "ca_ne_autoconf.h"
#endif
#include "aal_l2_tm_cb.h"
#include "aal_l2_tm.h"
#include "osal_common.h"
#include "aal_common.h"
#include "scfg.h"


#define __AAL_L2_TM_CB_PORT_NUM                 16
#define __AAL_L2_TM_CB_VOQ_NUM                  128
#define __AAL_L2_TM_CB_PORT_SEL_MAX             3
#define __AAL_L2_TM_CB_BUF_NUM                  4096
#define __AAL_L2_TM_CB_THRESHOLD_DISABLE        0x7FFF
#define __AAL_L2_TM_CB_GLB_THRESHOLD_HIGH       1228
#define __AAL_L2_TM_CB_GLB_THRESHOLD_LOW        409
#define __AAL_L2_TM_CB_GLB_PRI_THRSH01234_HIGH  461
#define __AAL_L2_TM_CB_GLB_PRI_THRSH01234_LOW   256
#define __AAL_L2_TM_CB_GLB_PRI_THRSH567_HIGH    461
#define __AAL_L2_TM_CB_GLB_PRI_THRSH567_LOW     256
#define __AAL_L2_TM_CB_PORT_HIGH_THRESHOLD      87
#define __AAL_L2_TM_CB_PORT_LOW_THRESHOLD       29
#define __AAL_L2_TM_CB_VOQ_HIGH_THRESHOLD       33
#define __AAL_L2_TM_CB_VOQ_LOW_THRESHOLD        18
#define __AAL_L2_TM_CB_VOQ_HIGH_THRESHOLD_567   33
#define __AAL_L2_TM_CB_VOQ_LOW_THRESHOLD_567    18
#define __AAL_L2_TM_CB_SRC_PORT_HIGH_THRESHOLD  3668
#define __AAL_L2_TM_CB_SRC_PORT_LOW_THRESHOLD   2044
#define __AAL_L2_TM_CB_SRC_PRI04_HIGH_THRESHOLD 462
#define __AAL_L2_TM_CB_SRC_PRI04_LOW_THRESHOLD  252
#define __AAL_L2_TM_CB_SRC_PRI57_HIGH_THRESHOLD 462
#define __AAL_L2_TM_CB_SRC_PRI57_LOW_THRESHOLD  252
#define __AAL_L2_TM_CB_PORT_FREE_BUF_CNT        3600
#define __AAL_L2_TM_CB_PRI_NUM                  8
#define __AAL_L2_TM_CB_VOQ_PRF_NUM              8
#define __AAL_L2_TM_CB_DEEP_SCH_EQ0_THRD        2252
#define __AAL_L2_TM_CB_DEEP_SCH_PORT_THRD_HI    87
#define __AAL_L2_TM_CB_DEEP_SCH_PORT_THRD_LO    29
#define __AAL_L2_TM_CB_DEEP_SCH_VOQ0_4_THRD_HI  33
#define __AAL_L2_TM_CB_DEEP_SCH_VOQ0_4_THRD_LO  18
#define __AAL_L2_TM_CB_DEEP_SCH_VOQ5_7_THRD_HI  33
#define __AAL_L2_TM_CB_DEEP_SCH_VOQ5_7_THRD_LO  18


ca_uint32_t l2te_glb_thrsh_thl[2] = {0,0};
ca_uint32_t l2te_glb_thrsh_thh[2] = {0,0};

ca_uint32_t l2te_glb_pri_thrsh_thl[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ca_uint32_t l2te_glb_pri_thrsh_thh[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


ca_uint32_t l2te_port_thrsh_profile_thl[2][4] = {0,0,0,0,0,0,0,0};
ca_uint32_t l2te_port_thrsh_profile_thh[2][4] = {0,0,0,0,0,0,0,0};

ca_uint32_t l2te_voq_thrsh_profile_thl[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ca_uint32_t l2te_voq_thrsh_profile_thh[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

ca_uint32_t l2te_src_port_thrsh_profile_thl[2][4] = {0,0,0,0,0,0,0,0};

ca_uint32_t l2te_src_port_thrsh_profile_thh[2][4] = {0,0,0,0,0,0,0,0};

ca_uint32_t l2te_src0_pri_thrsh_profile_thl[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ca_uint32_t l2te_src1_pri_thrsh_profile_thl[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ca_uint32_t l2te_src0_pri_thrsh_profile_thh[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ca_uint32_t l2te_src1_pri_thrsh_profile_thh[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

ca_uint32_t l2te_dqsch_eq_profile_thrsh0_thl[2] = {0,0};
ca_uint32_t l2te_dqsch_eq_profile_thrsh0_thh[2] = {0,0};

ca_uint32_t l2te_dqsch_port_thrsh_profile_thl[2][4]  = {0,0,0,0,0,0,0,0};
ca_uint32_t l2te_dqsch_port_thrsh_profile_thh[2][4]  = {0,0,0,0,0,0,0,0};

ca_uint32_t l2te_dqsch_voq_thrsh_profile_thl[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ca_uint32_t l2te_dqsch_voq_thrsh_profile_thh[2][8] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

ca_uint32_t l2te_port_free_buf_cnt[2][16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

#ifdef __HAVE_VOQ_BUF_LEN

aal_l2_tm_voq_buff_profile_t g_aal_l2_tm_voq_buff_profile[__AAL_L2_TM_CB_VOQ_PRF_NUM];
ca_uint8_t g_aal_l2_tm_voq_buff_profile_id[CA_AAL_MAX_PORT_ID + 1][8];
#endif


ca_status_t  aal_l2_tm_cb_intr_ena_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l2_tm_cb_intr_mask_t  mask  ,
    CA_IN aal_l2_tm_cb_intr_t       *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_INTE_t intr_en;

    if (cfg == NULL){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        intr_en.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_INTE);
    }
    if(mask.s.port_freecnt_ovfl){
        intr_en.bf.port_freecnt_ovflE = cfg->port_freecnt_ovfl;
    }
    if(mask.s.port_freecnt_udfl){
        intr_en.bf.port_freecnt_udflE = cfg->port_freecnt_udfl;
    }
    if(mask.s.voq_bufcnt_ovfl){
        intr_en.bf.voq_bufcnt_ovflE = cfg->voq_bufcnt_ovfl;
    }
    if(mask.s.voq_bufcnt_udfl){
        intr_en.bf.voq_bufcnt_udflE = cfg->voq_bufcnt_udfl;
    }
   /* if(mask.s.scan_cycle_err){
        intr_en.bf.scan_cycle_errE = cfg->scan_cycle_err;
    }*/

    if(0 == device_id){
        CA_NE_REG_WRITE(intr_en.wrd, L2TM_L2TE_CB_INTE);
    }

    return ret;
}

ca_status_t  aal_l2_tm_cb_intr_ena_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l2_tm_cb_intr_t       *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_INTE_t intr_en;

    if (cfg == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        intr_en.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_INTE);
    }

    cfg->port_freecnt_ovfl = intr_en.bf.port_freecnt_ovflE;
    cfg->port_freecnt_udfl = intr_en.bf.port_freecnt_udflE;
    cfg->voq_bufcnt_ovfl   = intr_en.bf.voq_bufcnt_ovflE  ;
    cfg->voq_bufcnt_udfl   = intr_en.bf.voq_bufcnt_udflE  ;
   // cfg->scan_cycle_err    = intr_en.bf.scan_cycle_errE   ;

    return ret;
}

ca_status_t  aal_l2_tm_cb_intr_status_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l2_tm_cb_intr_mask_t  mask  ,
    CA_IN aal_l2_tm_cb_intr_t       *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_INT_t intr_status;

    if (cfg == NULL){
        return CA_E_PARAM;
    }

    intr_status.wrd = 0;

    if(mask.s.port_freecnt_ovfl){
        intr_status.bf.port_freecnt_ovfl = cfg->port_freecnt_ovfl;
    }
    if(mask.s.port_freecnt_udfl){
        intr_status.bf.port_freecnt_udfl = cfg->port_freecnt_udfl;
    }
    if(mask.s.voq_bufcnt_ovfl){
        intr_status.bf.voq_bufcnt_ovfl = cfg->voq_bufcnt_ovfl;
    }
    if(mask.s.voq_bufcnt_udfl){
        intr_status.bf.voq_bufcnt_udfl = cfg->voq_bufcnt_udfl;
    }
   /* if(mask.s.scan_cycle_err){
        intr_status.bf.scan_cycle_err = cfg->scan_cycle_err;
    }*/
    if(0 == device_id){
        CA_NE_REG_WRITE(intr_status.wrd, L2TM_L2TE_CB_INT);
    }

    return ret;
}

ca_status_t  aal_l2_tm_cb_intr_status_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l2_tm_cb_intr_t       *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_INT_t intr_status;

    if (cfg == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        intr_status.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_INT);
    }

    cfg->port_freecnt_ovfl = intr_status.bf.port_freecnt_ovfl;
    cfg->port_freecnt_udfl = intr_status.bf.port_freecnt_udfl;
    cfg->voq_bufcnt_ovfl   = intr_status.bf.voq_bufcnt_ovfl  ;
    cfg->voq_bufcnt_udfl   = intr_status.bf.voq_bufcnt_udfl  ;
   // cfg->scan_cycle_err    = intr_status.bf.scan_cycle_err   ;

    return ret;
}

ca_status_t aal_l2_tm_cb_err_info_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT ca_uint16_t               *err_info
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ERR_INFO_t temp_err_info;

    if (err_info == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        temp_err_info.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_ERR_INFO);
    }

    *err_info = (ca_uint16_t)(temp_err_info.bf.voq_of_buferr);

    return ret;
}

ca_status_t aal_l2_tm_cb_ctrl_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN aal_l2_tm_cb_ctrl_mask_t    mask,
    CA_IN aal_l2_tm_cb_ctrl_t         *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_CTRL_t  te_cb_ctrl;

    if (cfg == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        te_cb_ctrl.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_CTRL);
    }

    if(mask.s.scan_cycle_per_run){
        te_cb_ctrl.bf.scan_cycle_per_run = cfg->scan_cycle_per_run;
    }

    if(mask.s.scan_ena){
        te_cb_ctrl.bf.scan_enable = cfg->scan_ena;
    }
    if(mask.s.scan_cycle_unit){
        te_cb_ctrl.bf.scan_cycle_unit  = cfg->scan_cycle_unit;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE(te_cb_ctrl.wrd, L2TM_L2TE_CB_CTRL);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_CTRL;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}


ca_status_t aal_l2_tm_cb_ctrl_get(
    CA_IN ca_device_id_t                 device_id,
    CA_OUT aal_l2_tm_cb_ctrl_t         *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_CTRL_t  te_cb_ctrl;

    if (cfg == NULL){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        te_cb_ctrl.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_CTRL);
    }

    cfg->scan_cycle_per_run = te_cb_ctrl.bf.scan_cycle_per_run;
    cfg->scan_ena = te_cb_ctrl.bf.scan_enable;
    cfg->scan_cycle_unit  = te_cb_ctrl.bf.scan_cycle_unit;

    return ret;
}


ca_status_t aal_l2_tm_cb_td_sts_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_tm_cb_td_sts_t *sts
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_GLOB_TD_STS_t  glb_td_sts;
    L2TM_L2TE_CB_PORT_TD_STS_t  port_td_sts;
    L2TM_L2TE_CB_VOQ_TD_STS0_t  voq_td_sts[L2TM_L2TE_CB_VOQ_TD_STS0_COUNT];
    L2TM_L2TE_CB_SRC_PRI_TD_STS_t src_pri_td_sts;
    ca_uint8_t  i;

    if (sts == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        glb_td_sts.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_GLOB_TD_STS);
        port_td_sts.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_TD_STS);
    }

    for(i= 0; i< L2TM_L2TE_CB_VOQ_TD_STS0_COUNT; i++){
        if(0 == device_id){
            voq_td_sts[i].wrd = CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_TD_STS0 + i*L2TM_L2TE_CB_VOQ_TD_STS0_STRIDE);
        }
    }
    if(0 == device_id){
        src_pri_td_sts.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_PRI_TD_STS);
    }

    sts->global_drop_pri_bmp    = glb_td_sts.bf.pfc;
    sts->spid_drop_bmp          = port_td_sts.bf.src_port;
    sts->dpid_drop_bmp          = port_td_sts.bf.port;
    sts->voq0_31_drop_bmp       = voq_td_sts[0].wrd;
    sts->voq32_63_drop_bmp      = voq_td_sts[1].wrd;
    sts->voq64_95_drop_bmp      = voq_td_sts[2].wrd;
    sts->voq96_127_drop_bmp     = voq_td_sts[3].wrd;
    sts->src_pri_drop_cnt0_bmp  = src_pri_td_sts.bf.src_pri & 0xff;
    sts->src_pri_drop_cnt1_bmp  = (src_pri_td_sts.bf.src_pri >> 8) & 0xff;
    return ret;
}

ca_status_t aal_l2_tm_cb_port_threshold_profile_set(
    CA_IN ca_device_id_t                       device_id,
    CA_IN ca_uint8_t                        profile_id,
    CA_IN aal_l2_tm_cb_comb_threshold_mask_t    mask,
    CA_IN aal_l2_tm_cb_comb_threshold_t     *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_PORT_THRSH_PROFILE0_t      port_thred;
    L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_t   grp_thred;

    if(profile_id >= L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_COUNT){
        return CA_E_PARAM;
    }

    if(NULL == cfg){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        port_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
        grp_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
    }

    if(mask.s.threshold_lo){
        port_thred.bf.lth = cfg->threshold_lo;
    }
    if(mask.s.threshold_hi){
        port_thred.bf.hth = cfg->threshold_hi;
    }
    if(mask.s.grp_threshold_lo){
        grp_thred.bf.grplth = cfg->grp_threshold_lo;
    }
    if(mask.s.grp_threshold_hi){
        grp_thred.bf.grphth = cfg->grp_threshold_hi;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE(port_thred.wrd,L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
        CA_NE_REG_WRITE(grp_thred.wrd,L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));

    regAddr = L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}


ca_status_t aal_l2_tm_cb_port_threshold_profile_get(
    CA_IN ca_device_id_t                       device_id,
    CA_IN ca_uint8_t                        profile_id,
    CA_OUT aal_l2_tm_cb_comb_threshold_t    *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_PORT_THRSH_PROFILE0_t      port_thred;
    L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_t   grp_thred;

    if(profile_id >= L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_COUNT){
        return CA_E_PARAM;
    }

    if(NULL == cfg){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        port_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
        grp_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
    }

    cfg->threshold_lo     = port_thred.bf.lth  ;
    cfg->threshold_hi     = port_thred.bf.hth  ;
    cfg->grp_threshold_lo = grp_thred.bf.grplth;
    cfg->grp_threshold_hi = grp_thred.bf.grphth;

    return ret;
}

ca_status_t aal_l2_tm_cb_port_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_PORT_THRSH_SELECT_t thd_sel;

    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) || (profile_id >= L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        thd_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_SELECT);
    }

    thd_sel.wrd &= ~(__AAL_L2_TM_CB_PORT_SEL_MAX << (port_id*2));

    thd_sel.wrd |= (profile_id << (port_id*2));

    if(0 == device_id){
        CA_NE_REG_WRITE(thd_sel.wrd, L2TM_L2TE_CB_PORT_THRSH_SELECT);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_PORT_THRSH_SELECT_t thd_sel;

    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) || (profile_id == NULL)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        thd_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_SELECT);
    }

    *profile_id = (thd_sel.wrd >> (port_id*2))&__AAL_L2_TM_CB_PORT_SEL_MAX;

    return ret;
}

ca_status_t aal_l2_tm_cb_voq_threshold_profile_set(
    CA_IN ca_device_id_t                           device_id,
    CA_IN ca_uint8_t                            profile_id,
    CA_IN aal_l2_tm_cb_comb_threshold_mask_t    mask,
    CA_IN aal_l2_tm_cb_comb_threshold_t         *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1_t voq_prf1;
    L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0_t voq_prf0;

    if((profile_id >= 8) || (NULL == cfg)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,profile_id, L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_ACCESS);
        voq_prf1.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1);
        voq_prf0.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0);
    }

    if(mask.s.threshold_lo){
        voq_prf0.bf.lth = cfg->threshold_lo;
    }
    if(mask.s.threshold_hi){
        voq_prf0.bf.hth = cfg->threshold_hi;
    }
    if(mask.s.grp_threshold_lo){
        voq_prf1.bf.grplth = ((cfg->grp_threshold_lo >> 2)&0x1FFF);
        voq_prf0.bf.grplth = (cfg->grp_threshold_lo & 0x03);
    }
    if(mask.s.grp_threshold_hi){
        voq_prf1.bf.grphth = cfg->grp_threshold_hi;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE(voq_prf1.wrd,L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1);
        CA_NE_REG_WRITE(voq_prf0.wrd,L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0);
        WRITE_INDIRCT_REG(device_id,profile_id, L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_ACCESS);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    READ_INDIRCT_REG(device_id,profile_id, L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_ACCESS);
    ca_printf("%s(%d)\tprofile_id: %#x\n", __func__, __LINE__, profile_id);

    ca_uint32 regAddr = L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));

    regAddr = L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}


ca_status_t aal_l2_tm_cb_voq_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_comb_threshold_t  *cfg
)
{
    L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1_t voq_prf1;
    L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0_t voq_prf0;

    if((profile_id >= 8) || (NULL == cfg)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,profile_id, L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_ACCESS);
        voq_prf1.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1);
        voq_prf0.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0);
    }

    cfg->threshold_lo = voq_prf0.bf.lth;
    cfg->threshold_hi = voq_prf0.bf.hth;
    cfg->grp_threshold_lo = ((voq_prf1.bf.grplth <<2) | voq_prf0.bf.grplth);
    cfg->grp_threshold_hi = voq_prf1.bf.grphth;

    return CA_E_OK;
}

ca_status_t aal_l2_tm_cb_voq_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_IN ca_uint8_t                profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) ||(profile_id >= 8)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE((ca_uint32_t)profile_id,L2TM_L2TE_CB_VOQ_THRSH_SELECT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id,voq_id,L2TM_L2TE_CB_VOQ_THRSH_SELECT_MEM_ACCESS);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    READ_INDIRCT_REG(device_id,profile_id, L2TM_L2TE_CB_VOQ_THRSH_SELECT_MEM_ACCESS);
    ca_printf("%s(%d)\tvoq_id: %#x\n", __func__, __LINE__, voq_id);

    ca_uint32 regAddr = L2TM_L2TE_CB_VOQ_THRSH_SELECT_MEM_DATA;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}


ca_status_t aal_l2_tm_cb_voq_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_OUT ca_uint8_t               *profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) ||(profile_id == NULL)){
         return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,voq_id,L2TM_L2TE_CB_VOQ_THRSH_SELECT_MEM_ACCESS);
        *profile_id = (ca_uint8_t)CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_THRSH_SELECT_MEM_DATA);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_port_free_buf_cnt_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN aal_l2_tm_cb_free_buf_cnt_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA_t port_free_buf_cnt;

    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) || (NULL == cfg)){
        return CA_E_PARAM;
    }

    port_free_buf_cnt.wrd = 0;
    port_free_buf_cnt.bf.cnt0_msb   = cfg->cnt0_msb;
    port_free_buf_cnt.bf.cnt0       = cfg->cnt0;
    port_free_buf_cnt.bf.cnt1_msb   = cfg->cnt1_msb;
    port_free_buf_cnt.bf.cnt1       = cfg->cnt1;

    if(0 == device_id){
        CA_NE_REG_WRITE(port_free_buf_cnt.wrd, L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id, port_id, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_port_free_buf_cnt_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT aal_l2_tm_cb_free_buf_cnt_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA_t port_free_buf_cnt;

    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) || (NULL == cfg)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, port_id, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        port_free_buf_cnt.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
    }

    cfg->cnt0_msb  = port_free_buf_cnt.bf.cnt0_msb  ;
    cfg->cnt0      = port_free_buf_cnt.bf.cnt0      ;
    cfg->cnt1_msb  = port_free_buf_cnt.bf.cnt1_msb  ;
    cfg->cnt1      = port_free_buf_cnt.bf.cnt1      ;

    return ret;
}


ca_status_t aal_l2_tm_cb_voq_buf_cnt_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN aal_l2_tm_cb_free_buf_cnt_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_VOQ_BUFCNT_MEM_DATA_t voq_buf_cnt;

    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) || (NULL == cfg)){
        return CA_E_PARAM;
    }

    voq_buf_cnt.wrd = 0;
    voq_buf_cnt.bf.cnt0_rsvd   = cfg->cnt0_msb;
    voq_buf_cnt.bf.cnt0       = cfg->cnt0;
    voq_buf_cnt.bf.cnt1_rsvd   = cfg->cnt1_msb;
    voq_buf_cnt.bf.cnt1       = cfg->cnt1;

    if(0 == device_id){
        CA_NE_REG_WRITE(voq_buf_cnt.wrd, L2TM_L2TE_CB_VOQ_BUFCNT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id, voq_id, L2TM_L2TE_CB_VOQ_BUFCNT_MEM_ACCESS);
    }

    return ret;

}


ca_status_t aal_l2_tm_cb_voq_buf_cnt_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT aal_l2_tm_cb_free_buf_cnt_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_VOQ_BUFCNT_MEM_DATA_t voq_buf_cnt;

    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) || (NULL == cfg)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, voq_id, L2TM_L2TE_CB_VOQ_BUFCNT_MEM_ACCESS);
        voq_buf_cnt.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_BUFCNT_MEM_DATA);
    }

    cfg->cnt0_msb   = voq_buf_cnt.bf.cnt0_rsvd;
    cfg->cnt0   = voq_buf_cnt.bf.cnt0    ;
    cfg->cnt1_msb   = voq_buf_cnt.bf.cnt1_rsvd;
    cfg->cnt1   = voq_buf_cnt.bf.cnt1    ;

    return ret;
}



ca_status_t aal_l2_tm_cb_voq_max_buf_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_IN ca_uint16_t                   cfg
)
{
    ca_status_t   ret = CA_E_OK;

    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) || (cfg >= __AAL_L2_TM_CB_BUF_NUM)){
         return CA_E_PARAM;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE((ca_uint32_t)cfg, L2TM_L2TE_CB_VOQ_MAX_BUFCNT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id, voq_id, L2TM_L2TE_CB_VOQ_MAX_BUFCNT_MEM_ACCESS);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_voq_max_buf_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    voq_id,
    CA_OUT ca_uint16_t                  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) || (cfg == NULL)){
         return CA_E_PARAM;
    }
    if(0 == device_id){
        READ_INDIRCT_REG(device_id, voq_id, L2TM_L2TE_CB_VOQ_MAX_BUFCNT_MEM_ACCESS);
        *cfg = (ca_uint16_t)CA_NE_REG_READ(L2TM_L2TE_CB_VOQ_MAX_BUFCNT_MEM_DATA);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_global_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_GLB_THRSH_t   glb_thrsh;

    if(NULL == cfg){
        return CA_E_PARAM;
    }
    glb_thrsh.wrd = 0;
    glb_thrsh.bf.hth = cfg->threshold_hi;
    glb_thrsh.bf.lth = cfg->threshold_lo;

    if(0 == device_id){
        CA_NE_REG_WRITE(glb_thrsh.wrd, L2TM_L2TE_CB_GLB_THRSH);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_GLB_THRSH;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}

ca_status_t aal_l2_tm_cb_global_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_GLB_THRSH_t   glb_thrsh;

    if(NULL == cfg){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        glb_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_GLB_THRSH);
    }

    cfg->threshold_hi = glb_thrsh.bf.hth;
    cfg->threshold_lo = glb_thrsh.bf.lth;

    return ret;
}

ca_status_t aal_l2_tm_cb_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_GLB_PRI_THRSH0_t glb_pri_thrsh;
    if((pri >= L2TM_L2TE_CB_GLB_PRI_THRSH0_COUNT) || (cfg == NULL)){
        return CA_E_PARAM;
    }
    glb_pri_thrsh.wrd = 0;
    glb_pri_thrsh.bf.lth = cfg->threshold_lo;
    glb_pri_thrsh.bf.hth = cfg->threshold_hi;

    if(0 == device_id){
        CA_NE_REG_WRITE(glb_pri_thrsh.wrd, L2TM_L2TE_CB_GLB_PRI_THRSH0 + L2TM_L2TE_CB_GLB_PRI_THRSH0_STRIDE*pri);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_GLB_PRI_THRSH0 + L2TM_L2TE_CB_GLB_PRI_THRSH0_STRIDE*pri;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}


ca_status_t aal_l2_tm_cb_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_GLB_PRI_THRSH0_t glb_pri_thrsh;
    if((pri >= L2TM_L2TE_CB_GLB_PRI_THRSH0_COUNT) || (cfg == NULL)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        glb_pri_thrsh.wrd = CA_NE_REG_READ( L2TM_L2TE_CB_GLB_PRI_THRSH0 + L2TM_L2TE_CB_GLB_PRI_THRSH0_STRIDE*pri);
    }

    cfg->threshold_lo = glb_pri_thrsh.bf.lth;
    cfg->threshold_hi = glb_pri_thrsh.bf.hth;

    return ret;
}

ca_status_t aal_l2_tm_cb_src_ctrl_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN aal_l2_tm_cb_src_ctrl_mask_t mask,
    CA_IN aal_l2_tm_cb_src_ctrl_t   *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC_CTRL_t src_ctrl;

    if(cfg == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        src_ctrl.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_CTRL);
    }

    if(mask.s.src1_pri_en){
        src_ctrl.bf.src1_pri_en = cfg->src1_pri_en;
    }
    if(mask.s.src1_pri_nspid){
        src_ctrl.bf.src1_pri_nspid = cfg->src1_pri_nspid;
    }
    if(mask.s.src0_pri_en){
        src_ctrl.bf.src0_pri_en = cfg->src0_pri_en;
    }
    if(mask.s.src0_pri_nspid){
        src_ctrl.bf.src0_pri_nspid = cfg->src0_pri_nspid;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE(src_ctrl.wrd, L2TM_L2TE_CB_SRC_CTRL);
    }

    return ret;
}

ca_status_t aal_l2_tm_cb_src_ctrl_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_tm_cb_src_ctrl_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC_CTRL_t src_ctrl;

    if(cfg == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        src_ctrl.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_CTRL);
    }

    cfg->src1_pri_en = src_ctrl.bf.src1_pri_en;
    cfg->src0_pri_en = src_ctrl.bf.src0_pri_en;
    cfg->src1_pri_nspid = src_ctrl.bf.src1_pri_nspid;
    cfg->src0_pri_nspid = src_ctrl.bf.src0_pri_nspid;

    return ret;
}

ca_status_t aal_l2_tm_cb_src_port_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_t src_port_thrsh;
    if((NULL == cfg)||(profile_id >=  L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }

    src_port_thrsh.wrd = 0;
    src_port_thrsh.bf.hth = cfg->threshold_hi;
    src_port_thrsh.bf.lth = cfg->threshold_lo;
    if(0 == device_id){
        CA_NE_REG_WRITE(src_port_thrsh.wrd,L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_STRIDE);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_STRIDE;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}

ca_status_t aal_l2_tm_cb_src_port_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
     ca_status_t   ret = CA_E_OK;

     L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_t src_port_thrsh;
     if((NULL == cfg)||(profile_id >=  L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_COUNT)){
         return CA_E_PARAM;
     }
    if(0 == device_id){
        src_port_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_STRIDE);
    }

    cfg->threshold_hi = src_port_thrsh.bf.hth;
    cfg->threshold_lo = src_port_thrsh.bf.lth;

    return ret;
}

ca_status_t aal_l2_tm_cb_src_port_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC_PORT_THRSH_SELECT_t src_port_thrsh_sel;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) ||(profile_id>=L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }

    src_port_thrsh_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_PORT_THRSH_SELECT);
    src_port_thrsh_sel.wrd &= ~(__AAL_L2_TM_CB_PORT_SEL_MAX << (port_id*2));
    src_port_thrsh_sel.wrd |= (profile_id << (port_id*2));
    if(0 == device_id){
        CA_NE_REG_WRITE(src_port_thrsh_sel.wrd, L2TM_L2TE_CB_SRC_PORT_THRSH_SELECT);
    }

    return ret;
}

ca_status_t aal_l2_tm_cb_src_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC_PORT_THRSH_SELECT_t src_port_thrsh_sel;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) ||(profile_id == NULL)){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        src_port_thrsh_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_PORT_THRSH_SELECT);
    }

    *profile_id = (src_port_thrsh_sel.wrd >> (port_id*2))&__AAL_L2_TM_CB_PORT_SEL_MAX;

    return ret;
}

ca_status_t aal_l2_tm_cb_src0_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC0_PRI_THRSH0_t src_pri_thrsh;

    if((NULL == cfg) || (profile_id >= L2TM_L2TE_CB_SRC0_PRI_THRSH0_COUNT)){
        return CA_E_PARAM;
    }

    src_pri_thrsh.wrd = 0;
    src_pri_thrsh.bf.hth = cfg->threshold_hi;
    src_pri_thrsh.bf.lth = cfg->threshold_lo;

    if(0 == device_id){
        CA_NE_REG_WRITE(src_pri_thrsh.wrd,L2TM_L2TE_CB_SRC0_PRI_THRSH0 + profile_id*L2TM_L2TE_CB_SRC0_PRI_THRSH0_STRIDE);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_SRC0_PRI_THRSH0 + profile_id*L2TM_L2TE_CB_SRC0_PRI_THRSH0_STRIDE;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}

ca_status_t aal_l2_tm_cb_src0_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC0_PRI_THRSH0_t src_pri_thrsh;

    if((NULL == cfg) || (profile_id >= L2TM_L2TE_CB_SRC0_PRI_THRSH0_COUNT)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        src_pri_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC0_PRI_THRSH0 + profile_id*L2TM_L2TE_CB_SRC0_PRI_THRSH0_STRIDE);
    }

    cfg->threshold_hi = src_pri_thrsh.bf.hth;
    cfg->threshold_lo = src_pri_thrsh.bf.lth;

    return ret;
}

ca_status_t aal_l2_tm_cb_src1_pri_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC1_PRI_THRSH0_t src_pri_thrsh;

    if((NULL == cfg) || (profile_id >= L2TM_L2TE_CB_SRC1_PRI_THRSH0_COUNT)){
        return CA_E_PARAM;
    }

    src_pri_thrsh.wrd = 0;
    src_pri_thrsh.bf.hth = cfg->threshold_hi;
    src_pri_thrsh.bf.lth = cfg->threshold_lo;

    if(0 == device_id){
        CA_NE_REG_WRITE(src_pri_thrsh.wrd,L2TM_L2TE_CB_SRC1_PRI_THRSH0 + profile_id*L2TM_L2TE_CB_SRC1_PRI_THRSH0_STRIDE);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_uint32 regAddr = L2TM_L2TE_CB_SRC1_PRI_THRSH0 + profile_id*L2TM_L2TE_CB_SRC1_PRI_THRSH0_STRIDE;
    ca_printf("%s(%d)\tCA_NE_REG_READ(%#x): %#x\n", __func__, __LINE__, regAddr, CA_NE_REG_READ(regAddr));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}

ca_status_t aal_l2_tm_cb_src1_pri_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_SRC1_PRI_THRSH0_t src_pri_thrsh;

    if((NULL == cfg) || (profile_id >= L2TM_L2TE_CB_SRC1_PRI_THRSH0_COUNT)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        src_pri_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_SRC1_PRI_THRSH0 + profile_id*L2TM_L2TE_CB_SRC1_PRI_THRSH0_STRIDE);
    }

    cfg->threshold_hi = src_pri_thrsh.bf.hth;
    cfg->threshold_lo = src_pri_thrsh.bf.lth;

    return ret;
}

ca_status_t aal_l2_tm_cb_all_threshold_get(
    CA_IN ca_device_id_t                   device_id,
    CA_OUT aal_l2_tm_cb_all_threshold_t *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_GLB_THRSH_t   glb_thrsh;
    L2TM_L2TE_CB_PORT_THRSH_PROFILE0_t      port_thred;
    L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_t   grp_thred;
    L2TM_L2TE_CB_PORT_THRSH_SELECT_t        port_sel;

    aal_l2_tm_cb_comb_threshold_t voq_profile;
    aal_l2_tm_cb_threshold_t      flow_control_profile;
    if(NULL == cfg){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        glb_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_GLB_THRSH);
        cfg->glb_threshold_hi = glb_thrsh.bf.hth;
        cfg->glb_threshold_lo = glb_thrsh.bf.lth;

        port_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_PROFILE0);
        cfg->port_threshold_profile0_hi = port_thred.bf.hth;
        cfg->port_threshold_profile0_lo = port_thred.bf.lth;

        port_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + 8);
        cfg->port_threshold_profile1_hi = port_thred.bf.hth;
        cfg->port_threshold_profile1_lo = port_thred.bf.lth;

        port_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + 16);
        cfg->port_threshold_profile2_hi = port_thred.bf.hth;
        cfg->port_threshold_profile2_lo = port_thred.bf.lth;

        port_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + 24);
        cfg->port_threshold_profile3_hi = port_thred.bf.hth;
        cfg->port_threshold_profile3_lo = port_thred.bf.lth;


        grp_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0);
        cfg->port_grp_threshold_profile0_hi = grp_thred.bf.grphth;
        cfg->port_grp_threshold_profile0_lo = grp_thred.bf.grplth;

        grp_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + 8);
        cfg->port_grp_threshold_profile1_hi = grp_thred.bf.grphth;
        cfg->port_grp_threshold_profile1_lo = grp_thred.bf.grplth;

        grp_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + 16);
        cfg->port_grp_threshold_profile2_hi = grp_thred.bf.grphth;
        cfg->port_grp_threshold_profile2_lo = grp_thred.bf.grplth;

        grp_thred.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0 + 24);
        cfg->port_grp_threshold_profile3_hi = grp_thred.bf.grphth;
        cfg->port_grp_threshold_profile3_lo = grp_thred.bf.grplth;

        port_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_THRSH_SELECT);
        cfg->port_profile_sel = port_sel.wrd;
    }

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,0,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_0 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_0 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile0_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile0_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,1,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_1 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_1 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile1_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile1_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,2,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_2 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_2 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile2_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile2_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,3,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_3 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_3 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile3_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile3_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,4,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_4 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_4 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile4_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile4_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,5,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_5 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_5 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile5_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile5_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,6,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_6 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_6 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile6_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile6_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_voq_threshold_profile_get(device_id,7,&voq_profile);
    if(ret != CA_E_OK){
        goto end;
    }
    cfg->voq_grp_threshold_hi_7 = voq_profile.grp_threshold_hi;
    cfg->voq_grp_threshold_lo_7 = voq_profile.grp_threshold_lo;
    cfg->voq_threshold_profile7_hi = voq_profile.threshold_hi;
    cfg->voq_threshold_profile7_lo = voq_profile.threshold_lo;

    ret = aal_l2_tm_cb_src_port_threshold_profile_get(device_id,0 , &flow_control_profile);
    cfg->src_port_threshold_profile0_hi = flow_control_profile.threshold_hi;
    cfg->src_port_threshold_profile0_lo = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src_port_threshold_profile_get(device_id,1 , &flow_control_profile);
    cfg->src_port_threshold_profile1_hi = flow_control_profile.threshold_hi;
    cfg->src_port_threshold_profile1_lo = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src_port_threshold_profile_get(device_id,2 , &flow_control_profile);
    cfg->src_port_threshold_profile2_hi = flow_control_profile.threshold_hi;
    cfg->src_port_threshold_profile2_lo = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src_port_threshold_profile_get(device_id,3 , &flow_control_profile);
    cfg->src_port_threshold_profile3_hi = flow_control_profile.threshold_hi;
    cfg->src_port_threshold_profile3_lo = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,0 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_0 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_0 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,1 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_1 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_1 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,2 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_2 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_2 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,3 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_3 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_3 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,4 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_4 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_4 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,5 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_5 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_5 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,6 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_6 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_6 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_pri_threshold_get(device_id,7 , &flow_control_profile);
    cfg->glb_pri_threshold_hi_7 = flow_control_profile.threshold_hi;
    cfg->glb_pri_threshold_lo_7 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,0 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_0 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_0 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,1 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_1 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_1 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,2 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_2 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_2 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,3 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_3 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_3 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,4 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_4 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_4 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,5 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_5 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_5 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,6 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_6 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_6 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src0_pri_threshold_get(device_id,7 , &flow_control_profile);
    cfg->src0_pri_threshold_hi_7 = flow_control_profile.threshold_hi;
    cfg->src0_pri_threshold_lo_7 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,0 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_0 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_0 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,1 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_1 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_1 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,2 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_2 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_2 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,3 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_3 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_3 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,4 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_4 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_4 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,5 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_5 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_5 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,6 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_6 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_6 = flow_control_profile.threshold_lo;

    ret = aal_l2_tm_cb_src1_pri_threshold_get(device_id,7 , &flow_control_profile);
    cfg->src1_pri_threshold_hi_7 = flow_control_profile.threshold_hi;
    cfg->src1_pri_threshold_lo_7 = flow_control_profile.threshold_lo;

    if(0 == device_id){
        cfg->src_ctrl = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_CTRL);

        READ_INDIRCT_REG(device_id, 0, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port00_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 1, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port01_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 2, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port02_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 3, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port03_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 4, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port04_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 5, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port05_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 6, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port06_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 7, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port07_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 8, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port08_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 9, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port09_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 10, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port10_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 11, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port11_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 12, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port12_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 13, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port13_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
        READ_INDIRCT_REG(device_id, 15, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
        cfg->port15_free_buffer_cnt = CA_NE_REG_READ(L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);

        cfg->dqsch_eq0_threshold = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_EQ_PROFILE_THRSH0);
        cfg->dqsch_port_profile0 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0);
        cfg->dqsch_port_profile1 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0 + 4);
        cfg->dqsch_port_profile2 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0 + 8);
        cfg->dqsch_port_profile3 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0 + 12);
        cfg->dqsch_port_profile_sel = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_SELECT);

        READ_INDIRCT_REG(device_id, 0, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile0 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 1, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile1 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 2, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile2 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 3, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile3 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 4, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile4 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 5, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile5 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 6, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile6 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);

        READ_INDIRCT_REG(device_id, 7, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        cfg->dqsch_voq_profile7 = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);
    }

end:
    return ret;
}

/*
ca_status_t aal_l2_tm_cb_src_pri_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_IN ca_uint8_t                profile_id
)

{
    ca_status_t   ret = CA_E_OK;


    return ret;
}

ca_status_t aal_l2_tm_cb_src_pri_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                pri,
    CA_OUT ca_uint8_t               *profile_id
)
{
    ca_status_t   ret = CA_E_OK;


    return ret;
}
*/

ca_status_t aal_l2_tm_cb_src_port_buf_cnt_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_IN ca_uint16_t                   cfg
)
{
    ca_status_t   ret = CA_E_OK;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM)||(cfg >= __AAL_L2_TM_CB_BUF_NUM)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE((ca_uint32_t)cfg,L2TM_L2TE_CB_SRC_PORT_BUFCNT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id, port_id, L2TM_L2TE_CB_SRC_PORT_BUFCNT_MEM_ACCESS);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_src_port_buf_cnt_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                    port_id,
    CA_OUT ca_uint16_t                  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM)||(cfg == NULL)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, port_id, L2TM_L2TE_CB_SRC_PORT_BUFCNT_MEM_ACCESS);
        *cfg = (ca_uint16_t)CA_NE_REG_READ(L2TM_L2TE_CB_SRC_PORT_BUFCNT_MEM_DATA);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_src_pri_buf_cnt_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_boolean_t                  indicate_ena,
    CA_IN ca_uint8_t                    pri_id,
    CA_IN ca_uint32_t                   cfg
)
{
    ca_status_t   ret = CA_E_OK;
    ca_uint32_t temp_value = 0;

    if((pri_id >= __AAL_L2_TM_CB_PRI_NUM) ||(cfg >= __AAL_L2_TM_CB_BUF_NUM)){
        return CA_E_PARAM;
    }

    temp_value = (ca_uint32_t)(((indicate_ena >0)<<3) | pri_id);

    if(0 == device_id){
        CA_NE_REG_WRITE(cfg,L2TM_L2TE_CB_SRC_PRI_BUFCNT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id,temp_value,L2TM_L2TE_CB_SRC_PRI_BUFCNT_MEM_ACCESS);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_src_pri_buf_cnt_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_boolean_t                  indicate_ena,
    CA_IN ca_uint8_t                    pri_id,
    CA_OUT ca_uint32_t                  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    ca_uint32_t temp_value = 0;

    if((pri_id >= __AAL_L2_TM_CB_PRI_NUM) ||(cfg == NULL)){
        return CA_E_PARAM;
    }

    temp_value = (ca_uint32_t)(((indicate_ena >0)<<3) | pri_id);

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,temp_value,L2TM_L2TE_CB_SRC_PRI_BUFCNT_MEM_ACCESS);
        *cfg = CA_NE_REG_READ(L2TM_L2TE_CB_SRC_PRI_BUFCNT_MEM_DATA);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_deep_q_sched_pool_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint32_t                   cfg

)
{
    ca_status_t   ret = CA_E_OK;

    if(0 == device_id){
        CA_NE_REG_WRITE(cfg,L2TM_L2TE_CB_DQSCH_EQ_PROFILE_THRSH0);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_deep_q_sched_pool_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT ca_uint32_t                  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    if(cfg == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        *cfg = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_EQ_PROFILE_THRSH0);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_deep_q_port_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_t dq_port_thrsh;
    if((NULL == cfg)||(profile_id >= L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }
    dq_port_thrsh.wrd = 0;
    dq_port_thrsh.bf.hth = cfg->threshold_hi;
    dq_port_thrsh.bf.lth = cfg->threshold_lo;

    if(0 == device_id){
        CA_NE_REG_WRITE(dq_port_thrsh.wrd,L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_STRIDE);
    }

    return ret;
}


ca_status_t aal_l2_tm_cb_deep_q_port_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_t dq_port_thrsh;
    if((NULL == cfg)||(profile_id >= L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        dq_port_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_STRIDE);
    }

    cfg->threshold_hi = dq_port_thrsh.bf.hth;
    cfg->threshold_lo = dq_port_thrsh.bf.lth;

    return ret;
}


ca_status_t aal_l2_tm_cb_deep_q_port_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t                profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_DQSCH_PORT_THRSH_SELECT_t src_port_thrsh_sel;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) ||(profile_id>=L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        src_port_thrsh_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_SELECT);
    }

    src_port_thrsh_sel.wrd &= ~(__AAL_L2_TM_CB_PORT_SEL_MAX << (port_id*2));
    src_port_thrsh_sel.wrd |= (profile_id << (port_id*2));

    if(0 == device_id){
        CA_NE_REG_WRITE(src_port_thrsh_sel.wrd, L2TM_L2TE_CB_DQSCH_PORT_THRSH_SELECT);
    }

    return ret;
}

ca_status_t aal_l2_tm_cb_deep_q_port_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t               *profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_DQSCH_PORT_THRSH_SELECT_t src_port_thrsh_sel;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) ||(profile_id == NULL)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        src_port_thrsh_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_PORT_THRSH_SELECT);
    }

    *profile_id = (src_port_thrsh_sel.wrd >> (port_id*2))&__AAL_L2_TM_CB_PORT_SEL_MAX;

    return ret;
}

ca_status_t aal_l2_tm_cb_deep_q_voq_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA_t dqsch_voq_thrsh;
    if((profile_id >=  __AAL_L2_TM_CB_VOQ_PRF_NUM)||(NULL == cfg)){
        return CA_E_PARAM;
    }
    dqsch_voq_thrsh.wrd = 0;

    dqsch_voq_thrsh.bf.lth = cfg->threshold_lo;
    dqsch_voq_thrsh.bf.hth = cfg->threshold_hi;

    if(0 == device_id){
        CA_NE_REG_WRITE(dqsch_voq_thrsh.wrd, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);
        WRITE_INDIRCT_REG(device_id,profile_id,L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
    }

    return ret;
}

ca_status_t aal_l2_tm_cb_deep_q_voq_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT aal_l2_tm_cb_threshold_t  *cfg
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA_t dqsch_voq_thrsh;
    if((profile_id >=  __AAL_L2_TM_CB_VOQ_PRF_NUM)||(NULL == cfg)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,profile_id,L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
        dqsch_voq_thrsh.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);
    }

    cfg->threshold_hi = dqsch_voq_thrsh.bf.hth;
    cfg->threshold_lo = dqsch_voq_thrsh.bf.lth;

    return ret;
}

ca_status_t aal_l2_tm_cb_deep_q_voq_profile_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_IN ca_uint8_t                profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) ||(profile_id >= __AAL_L2_TM_CB_VOQ_PRF_NUM)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE((ca_uint32_t)profile_id, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_SELECT_MEM_DATA);
        WRITE_INDIRCT_REG(device_id,voq_id,L2TM_L2TE_CB_DQSCH_VOQ_THRSH_SELECT_MEM_ACCESS);
    }

    return ret;
}

ca_status_t aal_l2_tm_cb_deep_q_voq_profile_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                voq_id,
    CA_OUT ca_uint8_t               *profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    if((voq_id >= __AAL_L2_TM_CB_VOQ_NUM) ||(profile_id == NULL)){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,voq_id,L2TM_L2TE_CB_DQSCH_VOQ_THRSH_SELECT_MEM_ACCESS);
        *profile_id = (ca_uint8_t)CA_NE_REG_READ(L2TM_L2TE_CB_DQSCH_VOQ_THRSH_SELECT_MEM_DATA);
    }

    return ret;
}

void aal_l2_tm_cb_init_startup_config(ca_device_id_t device_id)
{
        if(0 == device_id){
#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_GLB_THRSH_THL, 4, &l2te_glb_thrsh_thl[0]);
                aal_scfg_read(CFG_ID_L2TE_GLB_THRSH_THH, 4, &l2te_glb_thrsh_thh[0]);
#else//sd1 uboot
                l2te_glb_thrsh_thh[device_id] = 0xE10;
                l2te_glb_thrsh_thl[device_id] = 0x20;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH0_THL, 4, &l2te_glb_pri_thrsh_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH1_THL, 4, &l2te_glb_pri_thrsh_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH2_THL, 4, &l2te_glb_pri_thrsh_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH3_THL, 4, &l2te_glb_pri_thrsh_thl[0][3]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH4_THL, 4, &l2te_glb_pri_thrsh_thl[0][4]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH5_THL, 4, &l2te_glb_pri_thrsh_thl[0][5]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH6_THL, 4, &l2te_glb_pri_thrsh_thl[0][6]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH7_THL, 4, &l2te_glb_pri_thrsh_thl[0][7]);
#else//sd1 uboot
                l2te_glb_pri_thrsh_thl[device_id][0] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][1] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][2] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][3] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][4] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][5] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][6] = 0x10;
                l2te_glb_pri_thrsh_thl[device_id][7] = 0x10;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH0_THH, 4, &l2te_glb_pri_thrsh_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH1_THH, 4, &l2te_glb_pri_thrsh_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH2_THH, 4, &l2te_glb_pri_thrsh_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH3_THH, 4, &l2te_glb_pri_thrsh_thh[0][3]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH4_THH, 4, &l2te_glb_pri_thrsh_thh[0][4]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH5_THH, 4, &l2te_glb_pri_thrsh_thh[0][5]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH6_THH, 4, &l2te_glb_pri_thrsh_thh[0][6]);
                aal_scfg_read(CFG_ID_L2TE_GLB_PRI_THRSH7_THH, 4, &l2te_glb_pri_thrsh_thh[0][7]);
#else//sd1 uboot
                l2te_glb_pri_thrsh_thh[device_id][0] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][1] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][2] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][3] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][4] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][5] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][6] = 0x20;
                l2te_glb_pri_thrsh_thh[device_id][7] = 0x20;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_0_THL, 4, &l2te_port_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_1_THL, 4, &l2te_port_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_2_THL, 4, &l2te_port_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_3_THL, 4, &l2te_port_thrsh_profile_thl[0][3]);
#else//sd1 uboot
                l2te_port_thrsh_profile_thl[device_id][0] = 0x10;
                l2te_port_thrsh_profile_thl[device_id][1] = 0x10;
                l2te_port_thrsh_profile_thl[device_id][2] = 0x3FFF;
                l2te_port_thrsh_profile_thl[device_id][3] = 0x3FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_0_THH, 4, &l2te_port_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_1_THH, 4, &l2te_port_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_2_THH, 4, &l2te_port_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_PORT_THRSH_PROFILE_3_THH, 4, &l2te_port_thrsh_profile_thh[0][3]);
#else//sd1 uboot
                l2te_port_thrsh_profile_thh[device_id][0] = 0xD00;
                l2te_port_thrsh_profile_thh[device_id][1] = 0xD00;
                l2te_port_thrsh_profile_thh[device_id][2] = 0x7FFF;
                l2te_port_thrsh_profile_thh[device_id][3] = 0x7FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_0_THL	, 4, &l2te_voq_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_1_THL	, 4, &l2te_voq_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_2_THL	, 4, &l2te_voq_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_3_THL	, 4, &l2te_voq_thrsh_profile_thl[0][3]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_4_THL	, 4, &l2te_voq_thrsh_profile_thl[0][4]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_5_THL	, 4, &l2te_voq_thrsh_profile_thl[0][5]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_6_THL	, 4, &l2te_voq_thrsh_profile_thl[0][6]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_7_THL	, 4, &l2te_voq_thrsh_profile_thl[0][7]);
#else//sd1 uboot
                l2te_voq_thrsh_profile_thl[device_id][0] = 0x80;
                l2te_voq_thrsh_profile_thl[device_id][1] = 0x40;
                l2te_voq_thrsh_profile_thl[device_id][2] = 0x7FFF;
                l2te_voq_thrsh_profile_thl[device_id][3] = 0x7FFF;
                l2te_voq_thrsh_profile_thl[device_id][4] = 0x7FFF;
                l2te_voq_thrsh_profile_thl[device_id][5] = 0x7FFF;
                l2te_voq_thrsh_profile_thl[device_id][6] = 0x7FFF;
                l2te_voq_thrsh_profile_thl[device_id][7] = 0x7FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_0_THH	, 4, &l2te_voq_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_1_THH	, 4, &l2te_voq_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_2_THH	, 4, &l2te_voq_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_3_THH	, 4, &l2te_voq_thrsh_profile_thh[0][3]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_4_THH	, 4, &l2te_voq_thrsh_profile_thh[0][4]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_5_THH	, 4, &l2te_voq_thrsh_profile_thh[0][5]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_6_THH	, 4, &l2te_voq_thrsh_profile_thh[0][6]);
                aal_scfg_read(CFG_ID_L2TE_VOQ_THRSH_PROFILE_7_THH	, 4, &l2te_voq_thrsh_profile_thh[0][7]);
#else//sd1 uboot
                l2te_voq_thrsh_profile_thh[device_id][0] = 0x200;
                l2te_voq_thrsh_profile_thh[device_id][1] = 0x80;
                l2te_voq_thrsh_profile_thh[device_id][2] = 0x7FFF;
                l2te_voq_thrsh_profile_thh[device_id][3] = 0x7FFF;
                l2te_voq_thrsh_profile_thh[device_id][4] = 0x7FFF;
                l2te_voq_thrsh_profile_thh[device_id][5] = 0x7FFF;
                l2te_voq_thrsh_profile_thh[device_id][6] = 0x7FFF;
                l2te_voq_thrsh_profile_thh[device_id][7] = 0x7FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_0_THL , 4, &l2te_src_port_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_1_THL , 4, &l2te_src_port_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_2_THL , 4, &l2te_src_port_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_3_THL , 4, &l2te_src_port_thrsh_profile_thl[0][3]);
#else//sd1 uboot
                l2te_src_port_thrsh_profile_thl[device_id][0] = 0x100;
                l2te_src_port_thrsh_profile_thl[device_id][1] = 0x100;
                l2te_src_port_thrsh_profile_thl[device_id][2] = 0x100;
                l2te_src_port_thrsh_profile_thl[device_id][3] = 0x100;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_0_THH , 4, &l2te_src_port_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_1_THH , 4, &l2te_src_port_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_2_THH , 4, &l2te_src_port_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_SRC_PORT_THRSH_PROFILE_3_THH , 4, &l2te_src_port_thrsh_profile_thh[0][3]);
#else//sd1 uboot
                l2te_src_port_thrsh_profile_thh[device_id][0] = 0xC00;
                l2te_src_port_thrsh_profile_thh[device_id][1] = 0xC00;
                l2te_src_port_thrsh_profile_thh[device_id][2] = 0xC00;
                l2te_src_port_thrsh_profile_thh[device_id][3] = 0xC00;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS0_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS1_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS2_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS3_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][3]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS4_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][4]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS5_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][5]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS6_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][6]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS7_THL , 4, &l2te_src0_pri_thrsh_profile_thl[0][7]);
#else//sd1 uboot
                l2te_src0_pri_thrsh_profile_thl[device_id][0] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][1] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][2] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][3] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][4] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][5] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][6] = 0x110;
                l2te_src0_pri_thrsh_profile_thl[device_id][7] = 0x110;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS0_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS1_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS2_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS3_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][3]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS4_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][4]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS5_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][5]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS6_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][6]);
                aal_scfg_read(CFG_ID_L2TE_SRC0_PRI_THRSH_PROFILE_CoS7_THH , 4, &l2te_src0_pri_thrsh_profile_thh[0][7]);
#else//sd1 uboot
                l2te_src0_pri_thrsh_profile_thh[device_id][0] = 0x280;
                l2te_src0_pri_thrsh_profile_thh[device_id][1] = 0x180;
                l2te_src0_pri_thrsh_profile_thh[device_id][2] = 0x180;
                l2te_src0_pri_thrsh_profile_thh[device_id][3] = 0x180;
                l2te_src0_pri_thrsh_profile_thh[device_id][4] = 0x180;
                l2te_src0_pri_thrsh_profile_thh[device_id][5] = 0x180;
                l2te_src0_pri_thrsh_profile_thh[device_id][6] = 0x180;
                l2te_src0_pri_thrsh_profile_thh[device_id][7] = 0x180;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS0_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS1_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS2_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS3_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][3]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS4_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][4]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS5_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][5]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS6_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][6]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS7_THL , 4, &l2te_src1_pri_thrsh_profile_thl[0][7]);
#else//sd1 uboot
                l2te_src1_pri_thrsh_profile_thl[device_id][0] = 0x110;
                l2te_src1_pri_thrsh_profile_thl[device_id][1] = 0x60;
                l2te_src1_pri_thrsh_profile_thl[device_id][2] = 0x60;
                l2te_src1_pri_thrsh_profile_thl[device_id][3] = 0x60;
                l2te_src1_pri_thrsh_profile_thl[device_id][4] = 0x60;
                l2te_src1_pri_thrsh_profile_thl[device_id][5] = 0x60;
                l2te_src1_pri_thrsh_profile_thl[device_id][6] = 0x60;
                l2te_src1_pri_thrsh_profile_thl[device_id][7] = 0x60;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS0_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS1_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS2_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS3_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][3]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS4_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][4]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS5_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][5]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS6_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][6]);
                aal_scfg_read(CFG_ID_L2TE_SRC1_PRI_THRSH_PROFILE_CoS7_THH , 4, &l2te_src1_pri_thrsh_profile_thh[0][7]);
#else//sd1 uboot
                l2te_src1_pri_thrsh_profile_thh[device_id][0] = 0x280;
                l2te_src1_pri_thrsh_profile_thh[device_id][1] = 0x180;
                l2te_src1_pri_thrsh_profile_thh[device_id][2] = 0x180;
                l2te_src1_pri_thrsh_profile_thh[device_id][3] = 0x180;
                l2te_src1_pri_thrsh_profile_thh[device_id][4] = 0x180;
                l2te_src1_pri_thrsh_profile_thh[device_id][5] = 0x180;
                l2te_src1_pri_thrsh_profile_thh[device_id][6] = 0x180;
                l2te_src1_pri_thrsh_profile_thh[device_id][7] = 0x180;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_DQSCH_EQ_PROFILE_THRSH0_THL , 4, &l2te_dqsch_eq_profile_thrsh0_thl[0]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_EQ_PROFILE_THRSH0_THH , 4, &l2te_dqsch_eq_profile_thrsh0_thh[0]);
#else//sd1 uboot
                l2te_dqsch_eq_profile_thrsh0_thl[device_id] = 0x7FFF;
                l2te_dqsch_eq_profile_thrsh0_thh[device_id] = 0x7FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_0_THL , 4, &l2te_dqsch_port_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_1_THL , 4, &l2te_dqsch_port_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_2_THL , 4, &l2te_dqsch_port_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_3_THL , 4, &l2te_dqsch_port_thrsh_profile_thl[0][3]);
#else//sd1 uboot
                l2te_dqsch_port_thrsh_profile_thl[device_id][0] = 0x10;
                l2te_dqsch_port_thrsh_profile_thl[device_id][1] = 0x10;
                l2te_dqsch_port_thrsh_profile_thl[device_id][2] = 0x3FFF;
                l2te_dqsch_port_thrsh_profile_thl[device_id][3] = 0x3FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_0_THH , 4, &l2te_dqsch_port_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_1_THH , 4, &l2te_dqsch_port_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_2_THH , 4, &l2te_dqsch_port_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_PORT_THRSH_PROFILE_3_THH , 4, &l2te_dqsch_port_thrsh_profile_thh[0][3]);
#else//sd1 uboot
                l2te_dqsch_port_thrsh_profile_thh[device_id][0] = 0xE20;
                l2te_dqsch_port_thrsh_profile_thh[device_id][1] = 0xE20;
                l2te_dqsch_port_thrsh_profile_thh[device_id][2] = 0x3FFF;
                l2te_dqsch_port_thrsh_profile_thh[device_id][3] = 0x3FFF;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_0_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][0]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_1_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][1]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_2_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][2]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_3_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][3]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_4_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][4]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_5_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][5]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_6_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][6]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_7_THL  , 4, &l2te_dqsch_voq_thrsh_profile_thl[0][7]);
#else//sd1 uboot
                l2te_dqsch_voq_thrsh_profile_thl[device_id][0] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][1] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][2] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][3] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][4] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][5] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][6] = 18;
                l2te_dqsch_voq_thrsh_profile_thl[device_id][7] = 18;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_0_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][0]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_1_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][1]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_2_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][2]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_3_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][3]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_4_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][4]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_5_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][5]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_6_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][6]);
                aal_scfg_read(CFG_ID_L2TE_DQSCH_VOQ_THRSH_PROFILE_7_THH  , 4, &l2te_dqsch_voq_thrsh_profile_thh[0][7]);
#else//sd1 uboot
                l2te_dqsch_voq_thrsh_profile_thh[device_id][0] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][1] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][2] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][3] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][4] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][5] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][6] = 33;
                l2te_dqsch_voq_thrsh_profile_thh[device_id][7] = 33;
#endif

#if 0//yocto
                aal_scfg_read(CFG_ID_L2TE_PORT00_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][0 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT01_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][1 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT02_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][2 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT03_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][3 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT04_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][4 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT05_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][5 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT06_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][6 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT07_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][7 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT08_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][8 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT09_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][9 ]);
                aal_scfg_read(CFG_ID_L2TE_PORT10_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][10]);
                aal_scfg_read(CFG_ID_L2TE_PORT11_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][11]);
                aal_scfg_read(CFG_ID_L2TE_PORT12_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][12]);
                aal_scfg_read(CFG_ID_L2TE_PORT13_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][13]);
                aal_scfg_read(CFG_ID_L2TE_PORT14_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][14]);
                aal_scfg_read(CFG_ID_L2TE_PORT15_FREE_BUF_CNT  , 4, &l2te_port_free_buf_cnt[0][15]);
#else//sd1 uboot
                l2te_port_free_buf_cnt[device_id][0 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][1 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][2 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][3 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][4 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][5 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][6 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][7 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][8 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][9 ] = 0xE30;
                l2te_port_free_buf_cnt[device_id][10] = 0xE30;
                l2te_port_free_buf_cnt[device_id][11] = 0xE30;
                l2te_port_free_buf_cnt[device_id][12] = 0xE30;
                l2te_port_free_buf_cnt[device_id][13] = 0xE30;
                l2te_port_free_buf_cnt[device_id][14] = 0xE30;
                l2te_port_free_buf_cnt[device_id][15] = 0xE30;
#endif
        }

}

#ifdef __HAVE_VOQ_BUF_LEN
ca_boolean aal_l2_tm_voq_buff_profile_lookup
(
    ca_uint32_t buff_size,
    ca_uint32_t *index
)
{
    ca_uint32_t i = 0;

    if (!index)
        return FALSE;

    for (i=0; i<__AAL_L2_TM_CB_VOQ_PRF_NUM; i++)
    {
        if (g_aal_l2_tm_voq_buff_profile[i].ref_counter == 0)
            continue;

        if (g_aal_l2_tm_voq_buff_profile[i].size == buff_size)
        {
            *index = i;     /* hit, this profile entry can be reused */
            return TRUE;
        }
    }

    return FALSE;

}

ca_boolean aal_l2_tm_voq_buff_profile_allocate(ca_uint32_t *index)
{
    ca_uint32_t i = 0;

    if (!index)
        return FALSE;

    for (i=0; i<__AAL_L2_TM_CB_VOQ_PRF_NUM; i++)
    {
        if (g_aal_l2_tm_voq_buff_profile[i].ref_counter == 0)
        {
            *index = i;

            return TRUE;
        }
    }

    return FALSE;
}

ca_status_t aal_l2_tm_voq_buff_profile_set
(
    ca_uint32_t index,
    ca_uint32_t buff_size
)
{
    if (index > __AAL_L2_TM_CB_VOQ_PRF_NUM)
        return CA_E_PARAM;

    if (!buff_size)
        return CA_E_PARAM;

    g_aal_l2_tm_voq_buff_profile[index].size = buff_size;

    return CA_E_OK;
}

ca_status_t aal_l2_tm_voq_buff_profile_get
(
    ca_uint32_t index,
    ca_uint32_t *buff_size
)
{
    if (index > __AAL_L2_TM_CB_VOQ_PRF_NUM)
        return CA_E_PARAM;

    if (!buff_size)
        return CA_E_PARAM;

     *buff_size = g_aal_l2_tm_voq_buff_profile[index].size;

    return CA_E_OK;
}


ca_uint32_t aal_l2_tm_voq_buff_profile_ref_counter_get(ca_uint32_t index)
{
    return g_aal_l2_tm_voq_buff_profile[index].ref_counter;
}

void aal_l2_tm_voq_buff_profile_ref_inc(ca_uint32_t index)
{
    g_aal_l2_tm_voq_buff_profile[index].ref_counter++;
}

void aal_l2_tm_voq_buff_profile_ref_dec(ca_uint32_t index)
{
    if (g_aal_l2_tm_voq_buff_profile[index].ref_counter)
        g_aal_l2_tm_voq_buff_profile[index].ref_counter--;
}

void aal_l2_tm_voq_buff_profile_sel_set
(
    ca_port_id_t port,
    ca_uint32_t  queue,
    ca_uint32    index
)
{
    if (port <= CA_AAL_MAX_PORT_ID && queue <= 7)
        g_aal_l2_tm_voq_buff_profile_id[port][queue] = index;
}

ca_uint8_t aal_l2_tm_voq_buff_profile_sel_get
(
    ca_port_id_t port,
    ca_uint32_t  queue
)
{
    if (port <= CA_AAL_MAX_PORT_ID && queue <= 7)
        return g_aal_l2_tm_voq_buff_profile_id[port][queue];
    else
        return 0xff;
}


ca_status_t aal_l2_tm_cb_voq_buff_debug_get(ca_device_id_t device_id)
{
    ca_uint32_t i = 0, j = 0;

    printk("============= S/W VoQ buffer profile table ============= \n");

    for (i=0; i<__AAL_L2_TM_CB_VOQ_PRF_NUM; i++)
    {
        printk("index %02d : ref_counter(%d)\n", i, g_aal_l2_tm_voq_buff_profile[i].ref_counter);
    }

    printk("\n");

    printk("===== S/W VoQ buffer selection table ===== \n");
    printk("          Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7\n");
    printk("          -- -- -- -- -- -- -- --\n");

    for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
    {
        printk("port %02d : ", i);
        for (j=0; j<8; j++)
        {
            printk("%02d ", g_aal_l2_tm_voq_buff_profile_id[i][j]);
        }
        printk("\n");
    }

    return CA_E_OK;
}


#endif



ca_status_t aal_l2_tm_cb_init(ca_device_id_t device_id)
{
    aal_l2_tm_cb_free_buf_cnt_t port_free_buf_cnt;
    aal_l2_tm_cb_comb_threshold_t port_threshold_profile;
    aal_l2_tm_cb_comb_threshold_t voq_threshold_profile;
    aal_l2_tm_cb_comb_threshold_mask_t port_threshold_profile_mask;
    aal_l2_tm_cb_comb_threshold_mask_t voq_threshold_profile_mask;
    aal_l2_tm_cb_threshold_t glb_thsh;
    aal_l2_tm_cb_threshold_t glb_pri_thsh;
    aal_l2_tm_cb_threshold_t src_port_thsh;
    aal_l2_tm_cb_threshold_t src_pri_thsh;
    aal_l2_tm_cb_threshold_t dq_pri_thsh;
    aal_l2_tm_cb_threshold_t dq_port_thsh;
    ca_uint8_t voq = 0;

    ca_uint8 port_id, profile_id, pri;
    ca_status_t ret = CA_E_OK;
    memset(&port_free_buf_cnt, 0, sizeof(aal_l2_tm_cb_free_buf_cnt_t));
    memset(&port_threshold_profile, 0, sizeof(aal_l2_tm_cb_comb_threshold_t));
    memset(&voq_threshold_profile, 0, sizeof(aal_l2_tm_cb_comb_threshold_t));
    memset(&glb_thsh, 0, sizeof(aal_l2_tm_cb_threshold_t));
    memset(&glb_pri_thsh, 0, sizeof(aal_l2_tm_cb_threshold_t));
    memset(&src_port_thsh, 0, sizeof(aal_l2_tm_cb_threshold_t));
    memset(&src_pri_thsh, 0, sizeof(aal_l2_tm_cb_threshold_t));
    memset(&dq_pri_thsh, 0, sizeof(aal_l2_tm_cb_threshold_t));
    memset(&dq_port_thsh, 0, sizeof(aal_l2_tm_cb_threshold_t));

    port_threshold_profile_mask.u32 = 0;
    voq_threshold_profile_mask.u32 = 0 ;

    aal_l2_tm_cb_init_startup_config(device_id);

    for(pri = 0; pri < __AAL_L2_TM_CB_PRI_NUM; pri++){
        src_pri_thsh.threshold_hi = l2te_src0_pri_thrsh_profile_thh[device_id][pri];
        src_pri_thsh.threshold_lo = l2te_src0_pri_thrsh_profile_thl[device_id][pri];
        ret = aal_l2_tm_cb_src0_pri_threshold_set(device_id,pri,&src_pri_thsh);
        if( ret != CA_E_OK){
            return ret;
        }

        src_pri_thsh.threshold_hi = l2te_src1_pri_thrsh_profile_thh[device_id][pri];
        src_pri_thsh.threshold_lo = l2te_src1_pri_thrsh_profile_thl[device_id][pri];
        ret = aal_l2_tm_cb_src1_pri_threshold_set(device_id,pri,&src_pri_thsh);
        if( ret != CA_E_OK){
            return ret;
        }
    }
    for(profile_id = 0; profile_id < 4; profile_id++){
        src_port_thsh.threshold_hi =  l2te_src_port_thrsh_profile_thh[device_id][profile_id];
        src_port_thsh.threshold_lo =  l2te_src_port_thrsh_profile_thl[device_id][profile_id];
        ret = aal_l2_tm_cb_src_port_threshold_profile_set(device_id,profile_id , &src_port_thsh);
        if( ret != CA_E_OK){
            return ret;
        }
    }

    for(pri = 0; pri<__AAL_L2_TM_CB_PRI_NUM; pri++){
        glb_pri_thsh.threshold_hi = l2te_glb_pri_thrsh_thh[device_id][pri];
        glb_pri_thsh.threshold_lo = l2te_glb_pri_thrsh_thl[device_id][pri];
        ret = aal_l2_tm_cb_pri_threshold_set(device_id, pri, &glb_pri_thsh);
        if( ret != CA_E_OK){
            return ret;
        }
    }

    glb_thsh.threshold_hi = l2te_glb_thrsh_thh[device_id];
    glb_thsh.threshold_lo = l2te_glb_thrsh_thl[device_id];

    ret = aal_l2_tm_cb_global_threshold_set(device_id,&glb_thsh);
    if( ret != CA_E_OK){
        return ret;
    }

    port_free_buf_cnt.cnt1_msb = 1;
    port_free_buf_cnt.cnt0_msb = 1;

    for(port_id = 0; port_id<__AAL_L2_TM_CB_PORT_NUM; port_id ++){
 #ifdef CONFIG_ARCH_CORTINA_G3LITE
        port_free_buf_cnt.cnt0 = l2te_port_free_buf_cnt[device_id][port_id];
 #else
        port_free_buf_cnt.cnt1 = l2te_port_free_buf_cnt[device_id][port_id];
 #endif
        ret = aal_l2_tm_cb_port_free_buf_cnt_set(device_id, port_id, &port_free_buf_cnt);
        if( ret != CA_E_OK){
            return ret;
        }
    }

    port_threshold_profile.grp_threshold_hi = __AAL_L2_TM_CB_THRESHOLD_DISABLE;
    port_threshold_profile.grp_threshold_lo = __AAL_L2_TM_CB_THRESHOLD_DISABLE;

    port_threshold_profile_mask.s.grp_threshold_hi = 1;
    port_threshold_profile_mask.s.grp_threshold_lo = 1;
    port_threshold_profile_mask.s.threshold_hi = 1;
    port_threshold_profile_mask.s.threshold_lo = 1;

    for(profile_id = 0; profile_id<L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_COUNT; profile_id++){

        port_threshold_profile.threshold_hi = l2te_port_thrsh_profile_thh[device_id][profile_id];
        port_threshold_profile.threshold_lo = l2te_port_thrsh_profile_thl[device_id][profile_id];
        ret = aal_l2_tm_cb_port_threshold_profile_set(device_id,profile_id,port_threshold_profile_mask,&port_threshold_profile);

        if( ret != CA_E_OK){
            return ret;
        }
    }

    #ifndef __HAVE_VOQ_BUF_LEN
    voq_threshold_profile.grp_threshold_hi = __AAL_L2_TM_CB_THRESHOLD_DISABLE;
    voq_threshold_profile.grp_threshold_lo = __AAL_L2_TM_CB_THRESHOLD_DISABLE;
    voq_threshold_profile_mask.s.grp_threshold_hi = 1;
    voq_threshold_profile_mask.s.grp_threshold_lo = 1;
    voq_threshold_profile_mask.s.threshold_hi = 1;
    voq_threshold_profile_mask.s.threshold_lo = 1;

    for(profile_id = 0; profile_id<8; profile_id++){
        voq_threshold_profile.threshold_hi = l2te_voq_thrsh_profile_thh[device_id][profile_id];
        voq_threshold_profile.threshold_lo = l2te_voq_thrsh_profile_thl[device_id][profile_id];
        ret = aal_l2_tm_cb_voq_threshold_profile_set(device_id,profile_id,voq_threshold_profile_mask,&voq_threshold_profile);
        if( ret != CA_E_OK){
            return ret;
        }
    }

    for(voq = 0; voq<__AAL_L2_TM_CB_VOQ_NUM; voq++){
        if((voq&7) == 0){
            ret = aal_l2_tm_cb_voq_profile_sel_set(device_id,voq,0);
            if( ret != CA_E_OK){
                return ret;
            }
        }else{
            ret = aal_l2_tm_cb_voq_profile_sel_set(device_id,voq,1);
            if( ret != CA_E_OK){
                return ret;
            }
        }
    }
    #endif

    for(profile_id = 0; profile_id<4; profile_id++){
        dq_port_thsh.threshold_hi = l2te_dqsch_port_thrsh_profile_thh[device_id][profile_id];
        dq_port_thsh.threshold_lo = l2te_dqsch_port_thrsh_profile_thl[device_id][profile_id];
        ret = aal_l2_tm_cb_deep_q_port_threshold_profile_set(device_id,profile_id,&dq_port_thsh);
        if( ret != CA_E_OK){
            return ret;
        }
    }

    for(profile_id = 0; profile_id<8; profile_id++){
        dq_pri_thsh.threshold_hi = l2te_dqsch_voq_thrsh_profile_thh[device_id][profile_id];
        dq_pri_thsh.threshold_lo = l2te_dqsch_voq_thrsh_profile_thl[device_id][profile_id];

        ret = aal_l2_tm_cb_deep_q_voq_threshold_profile_set(device_id,profile_id,&dq_pri_thsh);
        if( ret != CA_E_OK){
            return ret;
        }
    }

    ret = aal_l2_tm_cb_deep_q_sched_pool_threshold_set(device_id,l2te_dqsch_eq_profile_thrsh0_thl[device_id]);
    if( ret != CA_E_OK){
        return ret;
    }


#ifdef __HAVE_VOQ_BUF_LEN
    /* reset VoQ buffer size software table */
    memset((void *)&g_aal_l2_tm_voq_buff_profile[0], 0x0, sizeof(g_aal_l2_tm_voq_buff_profile));
    memset((void *)&g_aal_l2_tm_voq_buff_profile_id[0][0], 0x0, sizeof(g_aal_l2_tm_voq_buff_profile_id));

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("L2_TM_CB : initialize VoQ based buffer profile & selection table ... ");
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    do
    {
        ca_uint32_t index = 0, i = 0, j = 0;

        voq_threshold_profile.grp_threshold_hi = __AAL_L2_TM_CB_THRESHOLD_DISABLE;
        voq_threshold_profile.grp_threshold_lo = __AAL_L2_TM_CB_THRESHOLD_DISABLE;
        voq_threshold_profile_mask.s.grp_threshold_hi = 1;
        voq_threshold_profile_mask.s.grp_threshold_lo = 1;
        voq_threshold_profile_mask.s.threshold_hi = 1;
        voq_threshold_profile_mask.s.threshold_lo = 1;

        /* update VoQ buffer threshold table */
        for (i=0; i<__AAL_L2_TM_CB_VOQ_PRF_NUM; i++)
        {
            voq_threshold_profile.threshold_hi = l2te_voq_thrsh_profile_thh[device_id][i]; /* initial value is from startup configuration */
            voq_threshold_profile.threshold_lo = l2te_voq_thrsh_profile_thl[device_id][i];
            if (aal_l2_tm_cb_voq_threshold_profile_set(device_id, i, voq_threshold_profile_mask, &voq_threshold_profile) != CA_E_OK)
                return CA_E_ERROR;
        }

        /* setup VoQ profile and selection table */
        index = 0;
        if (aal_l2_tm_voq_buff_profile_allocate(&index))
        {
            /* VoQ 0 for all ports use profile 0 by default */

            aal_l2_tm_voq_buff_profile_set(index, l2te_voq_thrsh_profile_thh[device_id][0] * __BM_BUF_BLOCK_SIZE);

            for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
            {
                for (j=0; j<1; j++)
                {
                    /* write VoQ based buffer threshold entry */

                    aal_l2_tm_voq_buff_profile_ref_inc(index); /* reference counter ++ */

                    /* update software selection table */
                    aal_l2_tm_voq_buff_profile_sel_set(i, j, index);

                    /* update hardware selection table */
                    voq = i * 8 + j;
                    (void)aal_l2_tm_cb_voq_profile_sel_set(device_id, voq, index);
                }
            }

#if CONFIG_98F_UBOOT_NE_DBG
            ca_printf("%s(%d) done.\n", __func__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }
        else
        {
            ca_printf("failed to allocate profile for VoQ 0.\n");
        }

        index = 0;
        if (aal_l2_tm_voq_buff_profile_allocate(&index))
        {
            /* VoQ 1-7 for all ports use profile 1 by default */

            aal_l2_tm_voq_buff_profile_set(index, l2te_voq_thrsh_profile_thh[device_id][1] * __BM_BUF_BLOCK_SIZE);

            for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
            {
                for (j=1; j<8; j++)
                {
                    /* write VoQ based buffer threshold entry */

                    aal_l2_tm_voq_buff_profile_ref_inc(index); /* reference counter ++ */

                    /* update software selection table */
                    aal_l2_tm_voq_buff_profile_sel_set(i, j, index);

                    /* update hardware selection table */
                    voq = i * 8 + j;
                    (void)aal_l2_tm_cb_voq_profile_sel_set(device_id, voq, index);
                }
            }

#if CONFIG_98F_UBOOT_NE_DBG
            ca_printf("%s(%d) done.\n", __func__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }
        else
        {
            ca_printf("failed to allocate profile for VoQ 1-7.\n");
        }
    }while(0);
#endif /* __HAVE_VOQ_BUF_LEN */


    /* enable SCAN to update WRED usage buffer */
    do 
    {
        aal_l2_tm_cb_ctrl_t cb_cfg;
        aal_l2_tm_cb_ctrl_mask_t cb_msk;

        cb_msk.u32 = 0;
        memset((void *)&cb_cfg, 0, sizeof(cb_cfg));

        cb_msk.s.scan_ena = 1;
        cb_cfg.scan_ena   = TRUE;

        (void)aal_l2_tm_cb_ctrl_set(device_id, cb_msk, &cb_cfg);
    }while(0);

#if CONFIG_98F_UBOOT_NE_DBG
    printf("==== end of %s(%d) ====\n", __func__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return ret;
}





/*
ca_status_t aal_l2_tm_cb_abr_td_drop_status_port_bmp_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT ca_uint16_t              *bmp
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ABR_TD_STS0_t abr_td_sta;

    if(bmp == NULL){
        return CA_E_PARAM;
    }
    *bmp = (ca_uint16_t)CA_NE_REG_READ(L2TM_L2TE_CB_ABR_TD_STS0);

    return ret;
}

ca_status_t aal_l2_tm_cm_abr_global_threshold_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint16_t               threshold
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ABR_THRSH_GLB_t abr_glb_thr;

    if(threshold >= __AAL_L2_TM_CB_BUF_NUM){
        return CA_E_PARAM;
    }
    abr_glb_thr.wrd = 0;
    abr_glb_thr.bf.th = (ca_uint32_t)threshold;

    CA_NE_REG_WRITE(abr_glb_thr.wrd,L2TM_L2TE_CB_ABR_THRSH_GLB);

    return ret;
}

ca_status_t aal_l2_tm_cm_abr_global_threshold_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT ca_uint16_t              *threshold
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ABR_THRSH_GLB_t abr_glb_thr;

    if(threshold == NULL){
        return CA_E_PARAM;
    }
    abr_glb_thr.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_ABR_THRSH_GLB);
    *threshold = (ca_uint16_t)abr_glb_thr.bf.th;

    return ret;
}

ca_status_t aal_l2_tm_cm_abr_threshold_profile_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_IN ca_uint16_t               threshold
)
{
    ca_status_t   ret = CA_E_OK;

    L2TM_L2TE_CB_ABR_THRSH_PROFILE0_t abr_thd_prf;
    if((threshold >= __AAL_L2_TM_CB_BUF_NUM) ||(profile_id >= L2TM_L2TE_CB_ABR_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }
    abr_thd_prf.wrd = 0;
    abr_thd_prf.bf.th = (ca_uint32_t)threshold;

    CA_NE_REG_WRITE(abr_thd_prf.wrd,L2TM_L2TE_CB_ABR_THRSH_PROFILE0 +profile_id*L2TM_L2TE_CB_ABR_THRSH_PROFILE0_STRIDE);

    return ret;
}

ca_status_t aal_l2_tm_cm_abr_threshold_profile_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                profile_id,
    CA_OUT ca_uint16_t              *threshold
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ABR_THRSH_PROFILE0_t abr_glb_thr;

     if((threshold == NULL)||(profile_id >= L2TM_L2TE_CB_ABR_THRSH_PROFILE0_COUNT)){
         return CA_E_PARAM;
     }
     abr_glb_thr.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_ABR_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_ABR_THRSH_PROFILE0_STRIDE);
     *threshold = (ca_uint16_t)abr_glb_thr.bf.th;

     return ret;


    return ret;
}

ca_status_t aal_l2_tm_cm_abr_threshold_profile_port_sel_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_IN ca_uint8_t               profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ABR_THRSH_SELECT_t abr_thrsh_sel;
    if((port_id >= __AAL_L2_TM_CB_PORT_NUM)||(profile_id >= L2TM_L2TE_CB_ABR_THRSH_PROFILE0_COUNT)){
        return CA_E_PARAM;
    }

    abr_thrsh_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_ABR_THRSH_SELECT);
    abr_thrsh_sel.wrd &= ~(__AAL_L2_TM_CB_PORT_SEL_MAX << (port_id*2));
    abr_thrsh_sel.wrd |= (profile_id << (port_id*2));

    CA_NE_REG_WRITE(abr_thrsh_sel.wrd, L2TM_L2TE_CB_ABR_THRSH_SELECT);

    return ret;
}

ca_status_t aal_l2_tm_cm_abr_threshold_profile_port_sel_get(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_uint8_t                port_id,
    CA_OUT ca_uint8_t              *profile_id
)
{
    ca_status_t   ret = CA_E_OK;
    L2TM_L2TE_CB_ABR_THRSH_SELECT_t abr_thrsh_sel;

    if((port_id >= __AAL_L2_TM_CB_PORT_NUM) ||(profile_id == NULL)){
        return CA_E_PARAM;
    }

    abr_thrsh_sel.wrd = CA_NE_REG_READ(L2TM_L2TE_CB_ABR_THRSH_SELECT);
    *profile_id = (abr_thrsh_sel.wrd >> (port_id*2))&__AAL_L2_TM_CB_PORT_SEL_MAX;

    return ret;
}
*/

EXPORT_SYMBOL(aal_l2_tm_cb_src_pri_buf_cnt_get);
EXPORT_SYMBOL(aal_l2_tm_cb_src1_pri_threshold_get);
EXPORT_SYMBOL(aal_l2_tm_cb_pri_threshold_set);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_port_threshold_profile_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src0_pri_threshold_get);
EXPORT_SYMBOL(aal_l2_tm_cb_src_pri_buf_cnt_set);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_buf_cnt_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src_ctrl_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src_port_threshold_profile_get);
EXPORT_SYMBOL(aal_l2_tm_cb_src_port_buf_cnt_set);
EXPORT_SYMBOL(aal_l2_tm_cb_intr_status_get);
EXPORT_SYMBOL(aal_l2_tm_cb_ctrl_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_voq_threshold_profile_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_sched_pool_threshold_get);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_profile_sel_get);
EXPORT_SYMBOL(aal_l2_tm_cb_src_port_threshold_profile_set);
EXPORT_SYMBOL(aal_l2_tm_cb_port_profile_sel_set);
EXPORT_SYMBOL(aal_l2_tm_cb_td_sts_get);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_buf_cnt_get);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_max_buf_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_port_profile_sel_get);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_threshold_profile_set);
EXPORT_SYMBOL(aal_l2_tm_cb_global_threshold_get);
EXPORT_SYMBOL(aal_l2_tm_cb_src_port_profile_sel_get);
EXPORT_SYMBOL(aal_l2_tm_cb_pri_threshold_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_port_profile_sel_set);
EXPORT_SYMBOL(aal_l2_tm_cb_intr_ena_get);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_max_buf_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src1_pri_threshold_set);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_voq_threshold_profile_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src_port_profile_sel_set);
EXPORT_SYMBOL(aal_l2_tm_cb_intr_ena_set);
EXPORT_SYMBOL(aal_l2_tm_cb_intr_status_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src_ctrl_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_port_threshold_profile_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_voq_profile_sel_get);
EXPORT_SYMBOL(aal_l2_tm_cb_global_threshold_set);
EXPORT_SYMBOL(aal_l2_tm_cb_port_threshold_profile_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_sched_pool_threshold_set);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_profile_sel_set);
EXPORT_SYMBOL(aal_l2_tm_cb_port_threshold_profile_set);
EXPORT_SYMBOL(aal_l2_tm_cb_voq_threshold_profile_get);
EXPORT_SYMBOL(aal_l2_tm_cb_deep_q_voq_profile_sel_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src0_pri_threshold_set);
EXPORT_SYMBOL(aal_l2_tm_cb_port_free_buf_cnt_set);
EXPORT_SYMBOL(aal_l2_tm_cb_port_free_buf_cnt_get);
EXPORT_SYMBOL(aal_l2_tm_cb_port_profile_sel_get);
EXPORT_SYMBOL(aal_l2_tm_cb_ctrl_set);
EXPORT_SYMBOL(aal_l2_tm_cb_src_port_buf_cnt_get);
EXPORT_SYMBOL(aal_l2_tm_cb_err_info_get);
EXPORT_SYMBOL(aal_l2_tm_cb_all_threshold_get);
#ifdef __HAVE_VOQ_BUF_LEN
EXPORT_SYMBOL(aal_l2_tm_cb_voq_buff_debug_get);
#endif



