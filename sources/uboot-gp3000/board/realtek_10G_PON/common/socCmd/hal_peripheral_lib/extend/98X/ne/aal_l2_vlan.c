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

#if 0//yocto
#include <generated/ca_ne_autoconf.h>
#else//sd1 u-boot
#include "ca_ne_autoconf.h"
#endif
#include "aal_l2_vlan.h"
#include "aal_arb.h"
#include "aal_common.h"
#include "osal_common.h"
#include "aal_port.h"
#include "aal_mcast.h"
#include "scfg.h"


#define __AAL_L2_VLAN_MAX_FIB_INDEX         127
#define __AAL_L2_VLAN_LOGIC_PORT_NUM        64
/*#define REG_READ(REG_ADDR, VALUE)   do{\
                                        VALUE=\
                                    }while(0)*/


#define G3_APB0_ADDRESS_BASE 0xf4300000

APB0_t *g_g3_dev_ptr = (APB0_t *)G3_APB0_ADDRESS_BASE;

#define AAL_L2FE_VE_REG_ADDR(reg) (volatile)L2FE_VE_##reg
#define AAL_L2FE_MEMSHIP_MAX    15

#if 0//yocto
#define __L2_VLAN_LOCK()    \
    do{ \
        __l2_vlan_lock();\
    }while(0)

#define __L2_VLAN_UNLOCK()    \
    do{ \
        __l2_vlan_unlock();\
    }while(0)
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_uint g_l2_vlan_lock;
ca_uint8_t __g_l2_vlan_port_map[__AAL_L2_VLAN_LOGIC_PORT_NUM] = {
                                           0,1,2,3,4,5,6,7,0,0,0,0,0,0,0,0,
                                           8,9,0,10,11,12,13,14,15,15,0,0,0,0,0,0,
                                           7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                           7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
                                                               };
ca_uint8_t g_l2_vlan_unknown_port_fib_map[__AAL_L2_VLAN_LOGIC_PORT_NUM] = {
                                            0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                            0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                            0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                            0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
static ca_uint64_t __g_unkown_vlan_mc_fib_bmp = 0;

ca_uint32_t g_vlan_flooding_domain_1[CFG_ID_FLOODING_DOMAIN_LEN];
ca_uint32_t g_vlan_flooding_domain_2[CFG_ID_FLOODING_DOMAIN_LEN];
ca_uint32_t g_vlan_mcgid_0 = 0;
ca_uint32_t g_vlan_mcgid_1 = 0;

#if 0//yocto
void  __l2_vlan_lock(void)
{
    ca_spin_lock(g_l2_vlan_lock);
}

void  __l2_vlan_unlock(void)
{
    ca_spin_unlock(g_l2_vlan_lock);
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

static ca_status_t
__aal_l2_wan_svlan_act_cfg_set (ca_device_id_t device_id,
				ca_uint8 fib_id,
				aal_l2_vlan_fib_mask_t mask,
				aal_l2_vlan_fib_t * cfg)
{
  L2FE_VE_WAN_SVL_FIB_DATA3_t svl_map_data3;
  L2FE_VE_WAN_SVL_FIB_DATA2_t svl_map_data2;
  L2FE_VE_WAN_SVL_FIB_DATA1_t svl_map_data1;
  L2FE_VE_WAN_SVL_FIB_DATA0_t svl_map_data0;

  if(0 == device_id){
      READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_SVL_FIB_ACCESS);
      svl_map_data3.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA3));
      svl_map_data2.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA2));
      svl_map_data1.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA1));
      svl_map_data0.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA0));
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_SVL_FIB_ACCESS);
      svl_map_data3.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA3);
      svl_map_data2.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA2);
      svl_map_data1.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA1);
      svl_map_data0.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA0);
#endif
  }

  if (mask.s.inner_vid_lookup_ena)
    {
        svl_map_data3.bf.wan_svl_fib_do_inner_vid_lookup =
        cfg->inner_vid_lookup_ena ? 1 : 0;
    }
  if (mask.s.dpid_field_vld)
    {
      svl_map_data2.bf.wan_svl_fib_ldpid_vld = cfg->dpid_field_vld ? 1 : 0;
    }
  if (mask.s.dpid)
    {
      svl_map_data2.bf.wan_svl_fib_ldpid = cfg->dpid ;
    }
  if (mask.s.vlan_id)
    {
      svl_map_data3.bf.wan_svl_fib_vid = (cfg->vlan_id >> 4) & 0xff;
      svl_map_data2.bf.wan_svl_fib_vid = cfg->vlan_id & 0xf;
    }
  /* if(mask.s.is_share_vlan            ){
     svl_map_data2.bf.wan_svl_fib_si_ind              = cfg->is_share_vlan ? 1:0          ;
     } */
  if (mask.s.is_svlan)
    {
      svl_map_data2.bf.wan_svl_fib_sc_ind = cfg->is_svlan ? 1 : 0;
    }
  if (mask.s.top_vlan_cmd)
    {
      svl_map_data2.bf.wan_svl_fib_top_vlan_cmd = cfg->top_vlan_cmd;
    }
  if (mask.s.top_vlan_id_sel)
    {
      svl_map_data2.bf.wan_svl_fib_topvid_usel2 = cfg->top_vlan_id_sel;
    }
  if (mask.s.top_vlan_cmd_field_vld)
    {
      svl_map_data2.bf.wan_svl_fib_top_cmd_valid =
	cfg->top_vlan_cmd_field_vld ? 1 : 0;
    }
  if (mask.s.inner_vlan_cmd)
    {
      svl_map_data2.bf.wan_svl_fib_inner_vlan_cmd = cfg->inner_vlan_cmd;
    }
  if (mask.s.inner_vlan_id_sel)
    {
      svl_map_data2.bf.wan_svl_fib_innervid_usel2 = cfg->inner_vlan_id_sel;
    }
  if (mask.s.inner_vlan_cmd_field_vld)
    {
      svl_map_data2.bf.wan_svl_fib_inner_cmd_valid =
	cfg->inner_vlan_cmd_field_vld ? 1 : 0;
    }
   if(mask.s.stp_stat                 ){
     svl_map_data2.bf.wan_svl_fib_stp_mode            = cfg->stp_stat                ;
     }
  if (mask.s.is_mc_vlan)
    {
      svl_map_data2.bf.wan_svl_fib_mc_vlan_ind = cfg->is_mc_vlan ? 1 : 0;
    }
  // if(mask.s.key_is_share_vlan        ){
  //      svl_map_data2.bf.wan_svl_fib_key_si_ind          = cfg->key_is_share_vlan ? 1:0      ;
  //  }
  if (mask.s.dot1p)
    {
      svl_map_data2.bf.wan_svl_fib_802_1p = cfg->dot1p;
    }
  if (mask.s.dscp)
    {
      svl_map_data2.bf.wan_svl_fib_dscp = cfg->dscp ;
      //svl_map_data1.bf.wan_svl_fib_dscp = cfg->dscp & 0x07;
    }
  if (mask.s.cos)
    {
      svl_map_data1.bf.wan_svl_fib_cos = cfg->cos;
    }
  if (mask.s.dot1p_field_vld)
    {
      svl_map_data1.bf.wan_svl_fib_802_1p_valid =
	cfg->dot1p_field_vld ? 1 : 0;
    }
  if (mask.s.dscp_field_vld)
    {
      svl_map_data1.bf.wan_svl_fib_dscp_valid = cfg->dscp_field_vld ? 1 : 0;
    }
  if (mask.s.cos_field_vld)
    {
      svl_map_data1.bf.wan_svl_fib_cos_valid = cfg->cos_field_vld ? 1 : 0;
    }
  if (mask.s.flow_id)
    {
      svl_map_data1.bf.wan_svl_fib_flowid = cfg->flow_id;
    }
  if (mask.s.flow_field_vld)
    {
      svl_map_data1.bf.wan_svl_fib_flowid_valid = cfg->flow_field_vld ? 1 : 0;
    }
  if (mask.s.permit)
    {
      svl_map_data1.bf.wan_svl_fib_permit = cfg->permit ? 1 : 0;
    }
  if (mask.s.da_sa_swap_ena)
    {
      svl_map_data1.bf.wan_svl_fib_da_sa_swap = cfg->da_sa_swap_ena ? 1 : 0;
    }
  if (mask.s.vlan_group_id)
    {
      svl_map_data1.bf.wan_svl_fib_vl_group_id = cfg->vlan_group_id;
    }
  if (mask.s.memship_bm)
    {
      svl_map_data0.bf.wan_svl_fib_membership_bm = cfg->memship_bm;
    }
  if (mask.s.untag_bm)
    {
      svl_map_data0.bf.wan_svl_fib_egress_top_untag_bm = cfg->untag_bm;
    }

  if(0 == device_id){
      CA_NE_REG_WRITE (svl_map_data3.wrd, AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA3));
      CA_NE_REG_WRITE (svl_map_data2.wrd, AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA2));
      CA_NE_REG_WRITE (svl_map_data1.wrd, AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA1));
      CA_NE_REG_WRITE (svl_map_data0.wrd, AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA0));
      WRITE_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_SVL_FIB_ACCESS);
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_NE_REG_WRITE (svl_map_data3.wrd, L2FE_VE_WAN_SVL_FIB_DATA3);
      CA_8279_NE_REG_WRITE (svl_map_data2.wrd, L2FE_VE_WAN_SVL_FIB_DATA2);
      CA_8279_NE_REG_WRITE (svl_map_data1.wrd, L2FE_VE_WAN_SVL_FIB_DATA1);
      CA_8279_NE_REG_WRITE (svl_map_data0.wrd, L2FE_VE_WAN_SVL_FIB_DATA0);
      CA_8279_WRITE_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_SVL_FIB_ACCESS);
#endif
  }

  return CA_E_OK;
}

static ca_status_t
__aal_l2_wan_svlan_act_cfg_get (ca_device_id_t device_id,
				ca_uint8 fib_id, aal_l2_vlan_fib_t * cfg)
{
  L2FE_VE_WAN_SVL_FIB_DATA3_t svl_map_data3;
  L2FE_VE_WAN_SVL_FIB_DATA2_t svl_map_data2;
  L2FE_VE_WAN_SVL_FIB_DATA1_t svl_map_data1;
  L2FE_VE_WAN_SVL_FIB_DATA0_t svl_map_data0;

  if(0 == device_id){
      READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_SVL_FIB_ACCESS);
      svl_map_data3.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA3));
      svl_map_data2.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA2));
      svl_map_data1.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA1));
      svl_map_data0.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_FIB_DATA0));
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_SVL_FIB_ACCESS);
      svl_map_data3.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA3);
      svl_map_data2.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA2);
      svl_map_data1.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA1);
      svl_map_data0.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_FIB_DATA0);
#endif
  }

  cfg->inner_vid_lookup_ena =
    svl_map_data3.bf.wan_svl_fib_do_inner_vid_lookup;
  cfg->dpid_field_vld = svl_map_data2.bf.wan_svl_fib_ldpid_vld;
  cfg->dpid = svl_map_data2.bf.wan_svl_fib_ldpid;
  cfg->vlan_id =
    (svl_map_data3.bf.wan_svl_fib_vid << 4) | (svl_map_data2.bf.
						wan_svl_fib_vid);
  /*cfg->is_share_vlan             = svl_map_data2.bf.wan_svl_fib_si_ind                 ; */
  cfg->is_svlan = svl_map_data2.bf.wan_svl_fib_sc_ind;
  cfg->top_vlan_cmd = svl_map_data2.bf.wan_svl_fib_top_vlan_cmd;
  cfg->top_vlan_id_sel = svl_map_data2.bf.wan_svl_fib_topvid_usel2;
  cfg->top_vlan_cmd_field_vld = svl_map_data2.bf.wan_svl_fib_top_cmd_valid;
  cfg->inner_vlan_cmd = svl_map_data2.bf.wan_svl_fib_inner_vlan_cmd;
  cfg->inner_vlan_id_sel = svl_map_data2.bf.wan_svl_fib_innervid_usel2;
  cfg->inner_vlan_cmd_field_vld =
    svl_map_data2.bf.wan_svl_fib_inner_cmd_valid;
  cfg->stp_stat                  = svl_map_data2.bf.wan_svl_fib_stp_mode               ;
  cfg->is_mc_vlan = svl_map_data2.bf.wan_svl_fib_mc_vlan_ind;
  //cfg->key_is_share_vlan         = svl_map_data2.bf.wan_svl_fib_key_si_ind             ;
  cfg->dot1p = svl_map_data2.bf.wan_svl_fib_802_1p;
  cfg->dscp =
    (svl_map_data2.bf.wan_svl_fib_dscp << 3);
  cfg->cos = svl_map_data1.bf.wan_svl_fib_cos;
  cfg->dot1p_field_vld = svl_map_data1.bf.wan_svl_fib_802_1p_valid;
  cfg->dscp_field_vld = svl_map_data1.bf.wan_svl_fib_dscp_valid;
  cfg->cos_field_vld = svl_map_data1.bf.wan_svl_fib_cos_valid;
  cfg->flow_id = svl_map_data1.bf.wan_svl_fib_flowid;
  cfg->flow_field_vld = svl_map_data1.bf.wan_svl_fib_flowid_valid;
  cfg->permit = svl_map_data1.bf.wan_svl_fib_permit;
  cfg->da_sa_swap_ena = svl_map_data1.bf.wan_svl_fib_da_sa_swap;
  cfg->vlan_group_id = svl_map_data1.bf.wan_svl_fib_vl_group_id;
  cfg->memship_bm = svl_map_data0.bf.wan_svl_fib_membership_bm;
  cfg->untag_bm = svl_map_data0.bf.wan_svl_fib_egress_top_untag_bm;

  return CA_E_OK;
}

static ca_status_t
__aal_l2_wan_cvlan_act_cfg_set (ca_device_id_t device_id,
				ca_uint8 fib_id,
				aal_l2_vlan_fib_mask_t mask,
				aal_l2_vlan_fib_t * cfg)
{
  L2FE_VE_WAN_CVL_FIB_DATA3_t cvl_map_data3;
  L2FE_VE_WAN_CVL_FIB_DATA2_t cvl_map_data2;
  L2FE_VE_WAN_CVL_FIB_DATA1_t cvl_map_data1;
  L2FE_VE_WAN_CVL_FIB_DATA0_t cvl_map_data0;

  if(0 == device_id){
      READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_CVL_FIB_ACCESS);
      cvl_map_data3.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA3));
      cvl_map_data2.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA2));
      cvl_map_data1.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA1));
      cvl_map_data0.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA0));
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_CVL_FIB_ACCESS);
      cvl_map_data3.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA3);
      cvl_map_data2.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA2);
      cvl_map_data1.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA1);
      cvl_map_data0.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA0);
#endif
  }

  if (mask.s.inner_vid_lookup_ena)
    {
      cvl_map_data3.bf.wan_cvl_fib_reserved = cfg->inner_vid_lookup_ena;
    }
  if (mask.s.dpid_field_vld)
    {
      cvl_map_data2.bf.wan_cvl_fib_ldpid_vld = cfg->dpid_field_vld ? 1 : 0;
    }
  if (mask.s.dpid)
    {
      cvl_map_data2.bf.wan_cvl_fib_ldpid = cfg->dpid;
    }
  if (mask.s.vlan_id)
    {
      cvl_map_data3.bf.wan_cvl_fib_vid = (cfg->vlan_id >> 4) & 0xff;
      cvl_map_data2.bf.wan_cvl_fib_vid = cfg->vlan_id & 0xf;
    }
  /* if(mask.s.is_share_vlan            ){
     cvl_map_data2.bf.wan_cvl_fib_si_ind              = cfg->is_share_vlan  ? 1:0         ;
     } */
  if (mask.s.is_svlan)
    {
      cvl_map_data2.bf.wan_cvl_fib_sc_ind = cfg->is_svlan ? 1 : 0;
    }
  if (mask.s.top_vlan_cmd)
    {
      cvl_map_data2.bf.wan_cvl_fib_top_vlan_cmd = cfg->top_vlan_cmd;
    }
  if (mask.s.top_vlan_id_sel)
    {
      cvl_map_data2.bf.wan_cvl_fib_topvid_usel2 = cfg->top_vlan_id_sel;
    }
  if (mask.s.top_vlan_cmd_field_vld)
    {
      cvl_map_data2.bf.wan_cvl_fib_top_cmd_valid =
	cfg->top_vlan_cmd_field_vld ? 1 : 0;
    }
  if (mask.s.inner_vlan_cmd)
    {
      cvl_map_data2.bf.wan_cvl_fib_inner_vlan_cmd = cfg->inner_vlan_cmd;
    }
  if (mask.s.inner_vlan_id_sel)
    {
      cvl_map_data2.bf.wan_cvl_fib_innervid_usel2 = cfg->inner_vlan_id_sel;
    }
  if (mask.s.inner_vlan_cmd_field_vld)
    {
      cvl_map_data2.bf.wan_cvl_fib_inner_cmd_valid =
	cfg->inner_vlan_cmd_field_vld ? 1 : 0;
    }
   if(mask.s.stp_stat                 ){
     cvl_map_data2.bf.wan_cvl_fib_stp_mode            = cfg->stp_stat                ;
     }
  if (mask.s.is_mc_vlan)
    {
      cvl_map_data2.bf.wan_cvl_fib_mc_vlan_ind = cfg->is_mc_vlan ? 1 : 0;
    }
  // if(mask.s.key_is_share_vlan        ){
  //     cvl_map_data2.bf.wan_cvl_fib_key_si_ind          = cfg->key_is_share_vlan  ? 1:0     ;
  //  }
  if (mask.s.dot1p)
    {
      cvl_map_data2.bf.wan_cvl_fib_802_1p = cfg->dot1p;
    }
  if (mask.s.dscp)
    {
      cvl_map_data2.bf.wan_cvl_fib_dscp = cfg->dscp;
    }
  if (mask.s.cos)
    {
      cvl_map_data1.bf.wan_cvl_fib_cos = cfg->cos;
    }
  if (mask.s.dot1p_field_vld)
    {
      cvl_map_data1.bf.wan_cvl_fib_802_1p_valid =
	cfg->dot1p_field_vld ? 1 : 0;
    }
  if (mask.s.dscp_field_vld)
    {
      cvl_map_data1.bf.wan_cvl_fib_dscp_valid = cfg->dscp_field_vld ? 1 : 0;
    }
  if (mask.s.cos_field_vld)
    {
      cvl_map_data1.bf.wan_cvl_fib_cos_valid = cfg->cos_field_vld ? 1 : 0;
    }
  if (mask.s.flow_id)
    {
      cvl_map_data1.bf.wan_cvl_fib_flowid = cfg->flow_id;
    }
  if (mask.s.flow_field_vld)
    {
      cvl_map_data1.bf.wan_cvl_fib_flowid_valid = cfg->flow_field_vld ? 1 : 0;
    }
  if (mask.s.permit)
    {
      cvl_map_data1.bf.wan_cvl_fib_permit = cfg->permit ? 1 : 0;
    }
  if (mask.s.da_sa_swap_ena)
    {
      cvl_map_data1.bf.wan_cvl_fib_da_sa_swap = cfg->da_sa_swap_ena ? 1 : 0;
    }
  if (mask.s.vlan_group_id)
    {
      cvl_map_data1.bf.wan_cvl_fib_vl_group_id = cfg->vlan_group_id;
    }
  if (mask.s.memship_bm)
    {
      cvl_map_data0.bf.wan_cvl_fib_membership_bm = cfg->memship_bm;
    }
  if (mask.s.untag_bm)
    {
      cvl_map_data0.bf.wan_cvl_fib_egress_top_untag_bm = cfg->untag_bm;
    }

  if(0 == device_id){
      CA_NE_REG_WRITE (cvl_map_data3.wrd, AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA3));
      CA_NE_REG_WRITE (cvl_map_data2.wrd, AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA2));
      CA_NE_REG_WRITE (cvl_map_data1.wrd, AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA1));
      CA_NE_REG_WRITE (cvl_map_data0.wrd, AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA0));
      WRITE_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_CVL_FIB_ACCESS);
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_NE_REG_WRITE (cvl_map_data3.wrd, L2FE_VE_WAN_CVL_FIB_DATA3);
      CA_8279_NE_REG_WRITE (cvl_map_data2.wrd, L2FE_VE_WAN_CVL_FIB_DATA2);
      CA_8279_NE_REG_WRITE (cvl_map_data1.wrd, L2FE_VE_WAN_CVL_FIB_DATA1);
      CA_8279_NE_REG_WRITE (cvl_map_data0.wrd, L2FE_VE_WAN_CVL_FIB_DATA0);
      CA_8279_WRITE_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_CVL_FIB_ACCESS);
#endif
  }
  return CA_E_OK;
}

static ca_status_t
__aal_l2_wan_cvlan_act_cfg_get (ca_device_id_t device_id,
				ca_uint8 fib_id, aal_l2_vlan_fib_t * cfg)
{
  L2FE_VE_WAN_CVL_FIB_DATA3_t cvl_map_data3;
  L2FE_VE_WAN_CVL_FIB_DATA2_t cvl_map_data2;
  L2FE_VE_WAN_CVL_FIB_DATA1_t cvl_map_data1;
  L2FE_VE_WAN_CVL_FIB_DATA0_t cvl_map_data0;

  if(0 == device_id){
      READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_CVL_FIB_ACCESS);
      cvl_map_data3.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA3));
      cvl_map_data2.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA2));
      cvl_map_data1.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA1));
      cvl_map_data0.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_FIB_DATA0));
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_WAN_CVL_FIB_ACCESS);
      cvl_map_data3.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA3);
      cvl_map_data2.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA2);
      cvl_map_data1.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA1);
      cvl_map_data0.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_FIB_DATA0);
#endif
  }

  cfg->inner_vid_lookup_ena = cvl_map_data3.bf.wan_cvl_fib_reserved;
  cfg->dpid_field_vld = cvl_map_data2.bf.wan_cvl_fib_ldpid_vld;
  cfg->dpid = cvl_map_data2.bf.wan_cvl_fib_ldpid;
  cfg->vlan_id =
    (cvl_map_data3.bf.wan_cvl_fib_vid << 4) | (cvl_map_data2.bf.
						wan_cvl_fib_vid);
  /* cfg->is_share_vlan             =  cvl_map_data2.bf.wan_cvl_fib_si_ind                 ;         */
  cfg->is_svlan = cvl_map_data2.bf.wan_cvl_fib_sc_ind;
  cfg->top_vlan_cmd = cvl_map_data2.bf.wan_cvl_fib_top_vlan_cmd;
  cfg->top_vlan_id_sel = cvl_map_data2.bf.wan_cvl_fib_topvid_usel2;
  cfg->top_vlan_cmd_field_vld = cvl_map_data2.bf.wan_cvl_fib_top_cmd_valid;
  cfg->inner_vlan_cmd = cvl_map_data2.bf.wan_cvl_fib_inner_vlan_cmd;
  cfg->inner_vlan_id_sel = cvl_map_data2.bf.wan_cvl_fib_innervid_usel2;
  cfg->inner_vlan_cmd_field_vld =
    cvl_map_data2.bf.wan_cvl_fib_inner_cmd_valid;
  cfg->stp_stat                  =  cvl_map_data2.bf.wan_cvl_fib_stp_mode               ;
  cfg->is_mc_vlan = cvl_map_data2.bf.wan_cvl_fib_mc_vlan_ind;
  //cfg->key_is_share_vlan         =  cvl_map_data2.bf.wan_cvl_fib_key_si_ind             ;
  cfg->dot1p = cvl_map_data2.bf.wan_cvl_fib_802_1p;
  cfg->dscp =
    (cvl_map_data2.bf.wan_cvl_fib_dscp);
  cfg->cos = cvl_map_data1.bf.wan_cvl_fib_cos;
  cfg->dot1p_field_vld = cvl_map_data1.bf.wan_cvl_fib_802_1p_valid;
  cfg->dscp_field_vld = cvl_map_data1.bf.wan_cvl_fib_dscp_valid;
  cfg->cos_field_vld = cvl_map_data1.bf.wan_cvl_fib_cos_valid;
  cfg->flow_id = cvl_map_data1.bf.wan_cvl_fib_flowid;
  cfg->flow_field_vld = cvl_map_data1.bf.wan_cvl_fib_flowid_valid;
  cfg->permit = cvl_map_data1.bf.wan_cvl_fib_permit;
  cfg->da_sa_swap_ena = cvl_map_data1.bf.wan_cvl_fib_da_sa_swap;
  cfg->vlan_group_id = cvl_map_data1.bf.wan_cvl_fib_vl_group_id;
  cfg->memship_bm = cvl_map_data0.bf.wan_cvl_fib_membership_bm;
  cfg->untag_bm = cvl_map_data0.bf.wan_cvl_fib_egress_top_untag_bm;
  return CA_E_OK;
}

static ca_status_t
__aal_l2_lan_vlan_act_cfg_set (ca_device_id_t device_id,
			       ca_uint8 fib_id,
			       aal_l2_vlan_fib_mask_t mask,
			       aal_l2_vlan_fib_t * cfg)
{
  L2FE_VE_LAN_VL_FIB_DATA3_t vl_map_data3;
  L2FE_VE_LAN_VL_FIB_DATA2_t vl_map_data2;
  L2FE_VE_LAN_VL_FIB_DATA1_t vl_map_data1;
  L2FE_VE_LAN_VL_FIB_DATA0_t vl_map_data0;

  if(0 == device_id){
      READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_LAN_VL_FIB_ACCESS);
      vl_map_data3.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA3));
      vl_map_data2.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA2));
      vl_map_data1.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA1));
      vl_map_data0.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA0));
  }

  if (mask.s.inner_vid_lookup_ena)
    {
        vl_map_data3.bf.lan_vl_fib_reserved = cfg->inner_vid_lookup_ena    ;
    }
  if (mask.s.dpid_field_vld)
    {
      vl_map_data2.bf.lan_vl_fib_ldpid_vld = cfg->dpid_field_vld ? 1 : 0;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) vl_map_data2.bf.lan_vl_fib_ldpid_vld: %#x\n", __func__, __LINE__, vl_map_data2.bf.lan_vl_fib_ldpid_vld);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  if (mask.s.dpid)
    {
      vl_map_data2.bf.lan_vl_fib_ldpid = cfg->dpid;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("%s(%d) vl_map_data2.bf.lan_vl_fib_ldpid: %#x\n", __func__, __LINE__, vl_map_data2.bf.lan_vl_fib_ldpid);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  if (mask.s.vlan_id)
    {
      vl_map_data3.bf.lan_vl_fib_vid = (cfg->vlan_id >> 6) & 0x3f;
      vl_map_data2.bf.lan_vl_fib_vid = cfg->vlan_id & 0x3f;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) cfg->vlan_id: %d\n", __func__, __LINE__, cfg->vlan_id);
    printf("%s(%d) vl_map_data3.bf.lan_vl_fib_vid: %#x\n", __func__, __LINE__, vl_map_data3.bf.lan_vl_fib_vid);
    printf("%s(%d) vl_map_data2.bf.lan_vl_fib_vid: %#x\n", __func__, __LINE__, vl_map_data2.bf.lan_vl_fib_vid);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  /* if(mask.s.is_share_vlan            ){
     vl_map_data2.bf.lan_vl_fib_si_ind              = cfg->is_share_vlan ? 1:0          ;
     }     */
  if (mask.s.is_svlan)
    {
      vl_map_data2.bf.lan_vl_fib_sc_ind = cfg->is_svlan ? 1 : 0;
    }
  if (mask.s.top_vlan_cmd)
    {
      vl_map_data2.bf.lan_vl_fib_top_vlan_cmd = cfg->top_vlan_cmd;
    }
  if (mask.s.top_vlan_id_sel)
    {
      // vl_map_data2.bf.lan_vl_fib_topvid_usel2        = cfg->top_vlan_id_sel         ;
    }
  if (mask.s.top_vlan_cmd_field_vld)
    {
      vl_map_data2.bf.lan_vl_fib_top_cmd_valid =
	cfg->top_vlan_cmd_field_vld ? 1 : 0;
    }
  if (mask.s.inner_vlan_cmd)
    {
      vl_map_data2.bf.lan_vl_fib_inner_vlan_cmd = cfg->inner_vlan_cmd;
    }
  /*
     if(mask.s.inner_vlan_id_sel        ){
     vl_map_data2.bf.lan_vl_fib_innervid_usel2      = cfg->inner_vlan_id_sel       ;
     }
   */
  if (mask.s.inner_vlan_cmd_field_vld)
    {
      vl_map_data2.bf.lan_vl_fib_inner_cmd_valid =
	cfg->inner_vlan_cmd_field_vld ? 1 : 0;
    }
    if(mask.s.stp_stat                 ){
      vl_map_data2.bf.lan_vl_fib_stp_mode            = cfg->stp_stat                ;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) vl_map_data2.bf.lan_vl_fib_stp_mode: %#x\n", __func__, __LINE__, vl_map_data2.bf.lan_vl_fib_stp_mode);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  if (mask.s.is_mc_vlan)
    {
      vl_map_data2.bf.lan_vl_fib_mc_vlan_ind = cfg->is_mc_vlan ? 1 : 0;
    }
  //if(mask.s.key_is_share_vlan        ){
  //    vl_map_data2.bf.lan_vl_fib_key_si_ind          = cfg->key_is_share_vlan  ? 1:0     ;
  // }
  if (mask.s.dot1p)
    {
      vl_map_data2.bf.lan_vl_fib_802_1p = cfg->dot1p;
    }
  if (mask.s.dscp)
    {
      vl_map_data2.bf.lan_vl_fib_dscp = cfg->dscp ;
    }
  if (mask.s.cos)
    {
      vl_map_data1.bf.lan_vl_fib_cos = cfg->cos;
    }
  if (mask.s.dot1p_field_vld)
    {
      vl_map_data1.bf.lan_vl_fib_802_1p_valid = cfg->dot1p_field_vld ? 1 : 0;
    }
  if (mask.s.dscp_field_vld)
    {
      vl_map_data1.bf.lan_vl_fib_dscp_valid = cfg->dscp_field_vld ? 1 : 0;
    }
  if (mask.s.cos_field_vld)
    {
      vl_map_data1.bf.lan_vl_fib_cos_valid = cfg->cos_field_vld ? 1 : 0;
    }
  if (mask.s.flow_id)
    {
      vl_map_data1.bf.lan_vl_fib_flowid = cfg->flow_id;
    }
  if (mask.s.flow_field_vld)
    {
      vl_map_data1.bf.lan_vl_fib_flowid_valid = cfg->flow_field_vld ? 1 : 0;
    }

  if (mask.s.permit)
    {
      vl_map_data1.bf.lan_vl_fib_permit = cfg->permit ? 1 : 0;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) vl_map_data1.bf.lan_vl_fib_permit: %#x\n", __func__, __LINE__, vl_map_data1.bf.lan_vl_fib_permit);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  if (mask.s.da_sa_swap_ena)
    {
      vl_map_data1.bf.lan_vl_fib_da_sa_swap = cfg->da_sa_swap_ena ? 1 : 0;
    }

  if (mask.s.vlan_group_id)
    {
      vl_map_data1.bf.lan_vl_fib_vl_group_id = cfg->vlan_group_id & 0x3ff;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) vl_map_data1.bf.lan_vl_fib_vl_group_id: %#x\n", __func__, __LINE__, vl_map_data1.bf.lan_vl_fib_vl_group_id);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  if (mask.s.memship_bm)
    {
      vl_map_data0.bf.lan_vl_fib_membership_bm = cfg->memship_bm;
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) vl_map_data0.bf.lan_vl_fib_membership_bm: %#x\n", __func__, __LINE__, vl_map_data0.bf.lan_vl_fib_membership_bm);
    printf("\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */

  if (mask.s.untag_bm)
    {
      vl_map_data0.bf.lan_vl_fib_egress_top_untag_bm = cfg->untag_bm;
    }

  if(0 == device_id){
      CA_NE_REG_WRITE (vl_map_data3.wrd, AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA3));
      CA_NE_REG_WRITE (vl_map_data2.wrd, AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA2));
      CA_NE_REG_WRITE (vl_map_data1.wrd, AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA1));
      CA_NE_REG_WRITE (vl_map_data0.wrd, AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA0));
      WRITE_INDIRCT_REG (device_id, fib_id, L2FE_VE_LAN_VL_FIB_ACCESS);
  }

  return CA_E_OK;

}

static ca_status_t
__aal_l2_lan_vlan_act_cfg_get (ca_device_id_t device_id,
			       ca_uint8 fib_id, aal_l2_vlan_fib_t * cfg)
{
  L2FE_VE_LAN_VL_FIB_DATA3_t vl_map_data3;
  L2FE_VE_LAN_VL_FIB_DATA2_t vl_map_data2;
  L2FE_VE_LAN_VL_FIB_DATA1_t vl_map_data1;
  L2FE_VE_LAN_VL_FIB_DATA0_t vl_map_data0;

  if(0 == device_id){
      READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_LAN_VL_FIB_ACCESS);
      vl_map_data3.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA3));
      vl_map_data2.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA2));
      vl_map_data1.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA1));
      vl_map_data0.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_FIB_DATA0));
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_READ_INDIRCT_REG (device_id, fib_id, L2FE_VE_LAN_VL_FIB_ACCESS);
      vl_map_data3.wrd = CA_8279_NE_REG_READ (L2FE_VE_LAN_VL_FIB_DATA3);
      vl_map_data2.wrd = CA_8279_NE_REG_READ (L2FE_VE_LAN_VL_FIB_DATA2);
      vl_map_data1.wrd = CA_8279_NE_REG_READ (L2FE_VE_LAN_VL_FIB_DATA1);
      vl_map_data0.wrd = CA_8279_NE_REG_READ (L2FE_VE_LAN_VL_FIB_DATA0);
#endif
  }
  //cfg->inner_vid_lookup_ena      =  vl_map_data3.bf.lan_vl_fib_do_inner_vid_lookup    ;
  cfg->dpid_field_vld = vl_map_data2.bf.lan_vl_fib_ldpid_vld;
  cfg->dpid = vl_map_data2.bf.lan_vl_fib_ldpid;
  cfg->vlan_id =
    (vl_map_data3.bf.lan_vl_fib_vid << 6) | (vl_map_data2.bf.lan_vl_fib_vid);
  /*cfg->is_share_vlan             =  vl_map_data2.bf.lan_vl_fib_si_ind                 ;     */
  cfg->is_svlan = vl_map_data2.bf.lan_vl_fib_sc_ind;
  cfg->top_vlan_cmd = vl_map_data2.bf.lan_vl_fib_top_vlan_cmd;
  //cfg->top_vlan_id_sel           =  vl_map_data2.bf.lan_vl_fib_topvid_usel2           ;
  cfg->top_vlan_cmd_field_vld = vl_map_data2.bf.lan_vl_fib_top_cmd_valid;
  cfg->inner_vlan_cmd = vl_map_data2.bf.lan_vl_fib_inner_vlan_cmd;
  //cfg->inner_vlan_id_sel         =  vl_map_data2.bf.lan_vl_fib_innervid_usel2         ;
  cfg->inner_vlan_cmd_field_vld = vl_map_data2.bf.lan_vl_fib_inner_cmd_valid;
  cfg->stp_stat                  =  vl_map_data2.bf.lan_vl_fib_stp_mode               ;
  cfg->is_mc_vlan = vl_map_data2.bf.lan_vl_fib_mc_vlan_ind;
  //cfg->key_is_share_vlan         =  vl_map_data2.bf.lan_vl_fib_key_si_ind             ;
  cfg->dot1p = vl_map_data2.bf.lan_vl_fib_802_1p;
  cfg->dscp =
    (vl_map_data2.bf.lan_vl_fib_dscp);
  cfg->cos = vl_map_data1.bf.lan_vl_fib_cos;
  cfg->dot1p_field_vld = vl_map_data1.bf.lan_vl_fib_802_1p_valid;
  cfg->dscp_field_vld = vl_map_data1.bf.lan_vl_fib_dscp_valid;
  cfg->cos_field_vld = vl_map_data1.bf.lan_vl_fib_cos_valid;
  cfg->flow_id = vl_map_data1.bf.lan_vl_fib_flowid;
  cfg->flow_field_vld = vl_map_data1.bf.lan_vl_fib_flowid_valid;
  cfg->permit = vl_map_data1.bf.lan_vl_fib_permit;
  cfg->da_sa_swap_ena = vl_map_data1.bf.lan_vl_fib_da_sa_swap;
  cfg->vlan_group_id = vl_map_data1.bf.lan_vl_fib_vl_group_id;
  cfg->memship_bm = vl_map_data0.bf.lan_vl_fib_membership_bm;
  cfg->untag_bm = vl_map_data0.bf.lan_vl_fib_egress_top_untag_bm;

  return CA_E_OK;
}

ca_status_t
aal_l2_vlan_action_cfg_set (CA_IN ca_device_id_t device_id,
			    CA_IN aal_l2_vlan_type_t vlan_type,
			    CA_IN ca_uint8 fib_id,
			    CA_IN aal_l2_vlan_fib_mask_t mask,
			    CA_IN aal_l2_vlan_fib_t * cfg)
{
  ca_status_t ret = CA_E_OK;

  if (vlan_type > AAL_L2_VLAN_TYPE_MAX)
    {
      return CA_E_PARAM;
    }
  if (fib_id > __AAL_L2_VLAN_MAX_FIB_INDEX)
    {
      return CA_E_PARAM;
    }
  if (NULL == cfg)
    {
      return CA_E_PARAM;
    }

  switch (vlan_type)
    {
    case AAL_L2_VLAN_TYPE_WAN_SVLAN:
      ret = __aal_l2_wan_svlan_act_cfg_set (device_id, fib_id, mask, cfg);
      break;
    case AAL_L2_VLAN_TYPE_WAN_CVLAN:
      ret = __aal_l2_wan_cvlan_act_cfg_set (device_id, fib_id, mask, cfg);
      break;
    case AAL_L2_VLAN_TYPE_LAN_VLAN:
      ret = __aal_l2_lan_vlan_act_cfg_set (device_id, fib_id, mask, cfg);
      break;
    default:
      ret = CA_E_PARAM;
      goto end;
    }

end:

  return ret;
}

ca_status_t
aal_l2_vlan_action_cfg_get (CA_IN ca_device_id_t device_id,
			    CA_IN aal_l2_vlan_type_t vlan_type,
			    CA_IN ca_uint8 fib_id,
			    CA_OUT aal_l2_vlan_fib_t * cfg)
{
  ca_status_t ret = CA_E_OK;

  if (vlan_type > AAL_L2_VLAN_TYPE_MAX)
    {
      return CA_E_PARAM;
    }
  if (fib_id > __AAL_L2_VLAN_MAX_FIB_INDEX)
    {
      return CA_E_PARAM;
    }
  if (NULL == cfg)
    {
      return CA_E_PARAM;
    }

#if 0//yocto
  __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  switch (vlan_type)
    {
    case AAL_L2_VLAN_TYPE_WAN_SVLAN:
      ret = __aal_l2_wan_svlan_act_cfg_get (device_id, fib_id, cfg);
      break;
    case AAL_L2_VLAN_TYPE_WAN_CVLAN:
      ret = __aal_l2_wan_cvlan_act_cfg_get (device_id, fib_id, cfg);
      break;
    case AAL_L2_VLAN_TYPE_LAN_VLAN:
      ret = __aal_l2_lan_vlan_act_cfg_get (device_id, fib_id, cfg);
      break;
    default:
      ret = CA_E_PARAM;
      goto end;
    }

end:

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    return ret;
}

ca_status_t
aal_l2_vlan_fib_map_set (CA_IN ca_device_id_t device_id,
			 CA_IN aal_l2_vlan_type_t vlan_type,
			 CA_IN ca_uint16_t vlan_id,
			 CA_IN aal_l2_vlan_fib_map_mask_t mask,
			 CA_IN aal_l2_vlan_fib_map_t * cfg)
{
  ca_status_t ret = CA_E_OK;
  L2FE_VE_WAN_SVL_IDX_DATA_t vl_index;
  if (vlan_type > AAL_L2_VLAN_TYPE_MAX)
    {
      return CA_E_PARAM;
    }
  if (cfg == NULL)
    {
      return CA_E_PARAM;
    }
  if (vlan_id > 4095)
    {
      return CA_E_PARAM;
    }
  if (cfg->fib_id > __AAL_L2_VLAN_MAX_FIB_INDEX)
    {
      return CA_E_PARAM;
    }

  vl_index.wrd = 0;

#if 0//yocto
  __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  switch (vlan_type)
    {
    case AAL_L2_VLAN_TYPE_WAN_SVLAN:
      if(0 == device_id){
          READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_SVL_IDX_ACCESS);
          vl_index.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_IDX_DATA));
      }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
          CA_8279_READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_SVL_IDX_ACCESS);
          vl_index.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_IDX_DATA);
#endif
      }

      if (mask.s.fib_id)
	{
	  vl_index.bf.wan_svl_idx_fib_idx = cfg->fib_id;
	}
      if (mask.s.valid)
	{
	  vl_index.bf.wan_svl_idx_entry_vld = cfg->valid;
	}
      if(0 == device_id){
          CA_NE_REG_WRITE (vl_index.wrd, AAL_L2FE_VE_REG_ADDR (WAN_SVL_IDX_DATA));
          WRITE_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_SVL_IDX_ACCESS);
      }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
          CA_8279_NE_REG_WRITE (vl_index.wrd, L2FE_VE_WAN_SVL_IDX_DATA);
          CA_8279_WRITE_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_SVL_IDX_ACCESS);
#endif
      }
      break;
    case AAL_L2_VLAN_TYPE_WAN_CVLAN:
      if(0 == device_id){
          READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_CVL_IDX_ACCESS);
          vl_index.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_IDX_DATA));
      }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
          CA_8279_READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_CVL_IDX_ACCESS);
          vl_index.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_IDX_DATA);
#endif
      }

      if (mask.s.fib_id)
	{
	  vl_index.bf.wan_svl_idx_fib_idx = cfg->fib_id;
	}
      if (mask.s.valid)
	{
	  vl_index.bf.wan_svl_idx_entry_vld = cfg->valid;
	}
      if(0 == device_id){
          CA_NE_REG_WRITE (vl_index.wrd, AAL_L2FE_VE_REG_ADDR (WAN_CVL_IDX_DATA));
          WRITE_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_CVL_IDX_ACCESS);
      }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
          CA_8279_NE_REG_WRITE (vl_index.wrd, L2FE_VE_WAN_CVL_IDX_DATA);
          CA_8279_WRITE_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_CVL_IDX_ACCESS);
#endif
      }
      break;
    case AAL_L2_VLAN_TYPE_LAN_VLAN:
      if(0 == device_id){
          READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_LAN_VL_IDX_ACCESS);
          vl_index.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_IDX_DATA));
      }

      if (mask.s.fib_id)
	{
	  vl_index.bf.wan_svl_idx_fib_idx = cfg->fib_id;
	}
      if (mask.s.valid)
	{
	  vl_index.bf.wan_svl_idx_entry_vld = cfg->valid;
	}

      if(0 == device_id){
          CA_NE_REG_WRITE (vl_index.wrd, AAL_L2FE_VE_REG_ADDR (LAN_VL_IDX_DATA));
          WRITE_INDIRCT_REG (device_id, vlan_id, L2FE_VE_LAN_VL_IDX_ACCESS);
      }
      break;
    default:
      ret = CA_E_PARAM;
      goto end;
    }
end:

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  return ret;

}

ca_status_t
aal_l2_vlan_fib_map_get (CA_IN ca_device_id_t device_id,
			 CA_IN aal_l2_vlan_type_t vlan_type,
			 CA_IN ca_uint16_t vlan_id,
			 CA_OUT aal_l2_vlan_fib_map_t * cfg)
{
  ca_status_t ret = CA_E_OK;

  L2FE_VE_WAN_SVL_IDX_DATA_t vl_index;

  if (vlan_type > AAL_L2_VLAN_TYPE_MAX)
    {
      return CA_E_PARAM;
    }
  if (cfg == NULL)
    {
      return CA_E_PARAM;
    }
  if (vlan_id > 4095)
    {
      return CA_E_PARAM;
    }
  vl_index.wrd = 0;

#if 0//yocto
  __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  switch (vlan_type)
    {
    case AAL_L2_VLAN_TYPE_WAN_SVLAN:
      if(0 == device_id){
          READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_SVL_IDX_ACCESS);
          vl_index.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_SVL_IDX_DATA));
      }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
          CA_8279_READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_SVL_IDX_ACCESS);
          vl_index.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_SVL_IDX_DATA);
#endif
      }
      break;
    case AAL_L2_VLAN_TYPE_WAN_CVLAN:
      if(0 == device_id){
          READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_CVL_IDX_ACCESS);
          vl_index.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (WAN_CVL_IDX_DATA));
      }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
          CA_8279_READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_WAN_CVL_IDX_ACCESS);
          vl_index.wrd = CA_8279_NE_REG_READ (L2FE_VE_WAN_CVL_IDX_DATA);
#endif
      }
      break;
    case AAL_L2_VLAN_TYPE_LAN_VLAN:
      if(0 == device_id){
          READ_INDIRCT_REG (device_id, vlan_id, L2FE_VE_LAN_VL_IDX_ACCESS);
          vl_index.wrd = CA_NE_REG_READ (AAL_L2FE_VE_REG_ADDR (LAN_VL_IDX_DATA));
      }
      break;
    default:
      ret = CA_E_PARAM;
      goto end;
    }
  cfg->fib_id = vl_index.bf.wan_svl_idx_fib_idx;
  cfg->valid = vl_index.bf.wan_svl_idx_entry_vld;
end:

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  return ret;

}

ca_status_t
aal_l2_vlan_default_cfg_set (CA_IN ca_device_id_t device_id,
			     CA_IN aal_l2_vlan_default_cfg_mask_t mask,
			     CA_IN aal_l2_vlan_default_cfg_t * cfg)
{
  ca_status_t ret = CA_E_OK;
  if (cfg == NULL)
    {
      return CA_E_PARAM;
    }

#if 0//yocto
  __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  if(0 == device_id){

      if (mask.s.vlan_mode)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLC_VLAN_MODE_CTRL, l2e_glb_vlan_mode,
    		       cfg->vlan_mode);
        }

      if (mask.s.cmp_tpid_svlan)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PP_TPID_CMP_S, tpid, cfg->cmp_tpid_svlan);
        }

      if (mask.s.cmp_tpid_cvlan)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PP_TPID_CMP_C, tpid, cfg->cmp_tpid_cvlan);
        }

      if (mask.s.cmp_tpid_other)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PP_TPID_CMP_O, tpid, cfg->cmp_tpid_other);
        }

      if (mask.s.tx_tpid0)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PE_CFG_VLAN_TPID_A, tpid, cfg->tx_tpid0);
        }

      if (mask.s.tx_tpid1)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PE_CFG_VLAN_TPID_B, tpid, cfg->tx_tpid1);
        }

      if (mask.s.tx_tpid2)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PE_CFG_VLAN_TPID_C, tpid, cfg->tx_tpid2);
        }

      if (mask.s.tx_tpid3)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PE_CFG_VLAN_TPID_D, tpid, cfg->tx_tpid3);
        }

      if (mask.s.wan_memship_check_ena)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_CTL_REG, txvlan_membshp_chk_en_wan,
    		       cfg->wan_memship_check_ena);
        }

      if (mask.s.lan_memship_check_ena)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_CTL_REG, txvlan_membshp_chk_en_lan,
    		       cfg->lan_memship_check_ena);
        }

      if (mask.s.rx_unknown_vlan_memship_check_bmp)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_RX_UNKWN_VLAN_DFT, memshp_bm,
    		       cfg->rx_unknown_vlan_memship_check_bmp);
        }

      if (mask.s.rx_unknown_vlan_grp_id)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_RX_UNKWN_VLAN_DFT, vl_grp_id,
    		       cfg->rx_unknown_vlan_grp_id);
        }

      if (mask.s.tx_unknown_vlan_memship_check_bmp)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_TX_UNKWN_VLAN_DFT_2, memshp_bm,
    		       cfg->tx_unknown_vlan_memship_check_bmp);
        }

      if (mask.s.tx_unknown_vlan_untag_bmp)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_TX_UNKWN_VLAN_DFT_2, untag_bm,
    		       cfg->tx_unknown_vlan_untag_bmp);
        }

      if (mask.s.tx_unknown_vlan_stp_mode_forward_ena)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLE_TX_UNKWN_VLAN_DFT_1, stp_mode,
    		       cfg->tx_unknown_vlan_stp_mode_forward_ena);
        }

      if(mask.s.unknown_vlan_fwd_ena)
        {
            REG_L2FE_FIELD_WRITE (L2FE_PLC_DPID_FWD_CTRL, unkvlan_fwd,
                 cfg->unknown_vlan_fwd_ena);
        }

      if(mask.s.vlan_4095_fwd_ena)
        {
          REG_L2FE_FIELD_WRITE (L2FE_PLC_DPID_FWD_CTRL, vid_fff_fwd,
               cfg->vlan_4095_fwd_ena);
        }

#if CONFIG_98F_UBOOT_NE_DBG
            printf("mask.s.vlan_mode: %#x\n", mask.s.vlan_mode);
            printf("mask.s.cmp_tpid_svlan: %#x\n", mask.s.cmp_tpid_svlan);
            printf("mask.s.cmp_tpid_cvlan: %#x\n", mask.s.cmp_tpid_cvlan);
            printf("mask.s.cmp_tpid_other: %#x\n", mask.s.cmp_tpid_other);
            printf("mask.s.tx_tpid0: %#x\n", mask.s.tx_tpid0);
            printf("mask.s.tx_tpid1: %#x\n", mask.s.tx_tpid1);
            printf("mask.s.tx_tpid2: %#x\n", mask.s.tx_tpid2);
            printf("mask.s.tx_tpid3: %#x\n", mask.s.tx_tpid3);
            printf("mask.s.wan_memship_check_ena: %#x\n", mask.s.wan_memship_check_ena);
            printf("mask.s.lan_memship_check_ena: %#x\n", mask.s.lan_memship_check_ena);
            printf("mask.s.rx_unknown_vlan_memship_check_bmp: %#x\n", mask.s.rx_unknown_vlan_memship_check_bmp);
            printf("mask.s.rx_unknown_vlan_grp_id: %#x\n", mask.s.rx_unknown_vlan_grp_id);
            printf("mask.s.tx_unknown_vlan_memship_check_bmp: %#x\n", mask.s.tx_unknown_vlan_memship_check_bmp);
            printf("mask.s.tx_unknown_vlan_untag_bmp: %#x\n", mask.s.tx_unknown_vlan_untag_bmp);
            printf("mask.s.tx_unknown_vlan_stp_mode_forward_ena: %#x\n", mask.s.tx_unknown_vlan_stp_mode_forward_ena);
            printf("mask.s.unknown_vlan_fwd_ena: %#x\n", mask.s.unknown_vlan_fwd_ena);
            printf("mask.s.vlan_4095_fwd_ena: %#x\n", mask.s.vlan_4095_fwd_ena);
#endif
      }

#if 0//yocto
  __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  return ret;
}

ca_status_t
aal_l2_vlan_default_cfg_get (CA_IN ca_device_id_t device_id,
			     CA_OUT aal_l2_vlan_default_cfg_t * cfg)
{
  ca_status_t ret = CA_E_OK;
  if (cfg == NULL)
    {
      return CA_E_PARAM;
    }

#if 0//yocto
  __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  if(0 == device_id){
      REG_L2FE_FIELD_READ (L2FE_PLC_VLAN_MODE_CTRL, l2e_glb_vlan_mode,
    		  cfg->vlan_mode);
      REG_L2FE_FIELD_READ (L2FE_PP_TPID_CMP_S, tpid, cfg->cmp_tpid_svlan);
      REG_L2FE_FIELD_READ (L2FE_PP_TPID_CMP_C, tpid, cfg->cmp_tpid_cvlan);
      REG_L2FE_FIELD_READ (L2FE_PP_TPID_CMP_O, tpid, cfg->cmp_tpid_other);
      REG_L2FE_FIELD_READ (L2FE_PE_CFG_VLAN_TPID_A, tpid, cfg->tx_tpid0);
      REG_L2FE_FIELD_READ (L2FE_PE_CFG_VLAN_TPID_B, tpid, cfg->tx_tpid1);
      REG_L2FE_FIELD_READ (L2FE_PE_CFG_VLAN_TPID_C, tpid, cfg->tx_tpid2);
      REG_L2FE_FIELD_READ (L2FE_PE_CFG_VLAN_TPID_D, tpid, cfg->tx_tpid3);
      REG_L2FE_FIELD_READ (L2FE_PLE_CTL_REG, txvlan_membshp_chk_en_wan,
    		  cfg->wan_memship_check_ena);
      REG_L2FE_FIELD_READ (L2FE_PLE_CTL_REG, txvlan_membshp_chk_en_lan,
    		  cfg->lan_memship_check_ena);
      REG_L2FE_FIELD_READ (L2FE_PLE_RX_UNKWN_VLAN_DFT, memshp_bm,
    		  cfg->rx_unknown_vlan_memship_check_bmp);
      REG_L2FE_FIELD_READ (L2FE_PLE_RX_UNKWN_VLAN_DFT, vl_grp_id,
    		  cfg->rx_unknown_vlan_grp_id);
      REG_L2FE_FIELD_READ (L2FE_PLE_TX_UNKWN_VLAN_DFT_2, memshp_bm,
    		  cfg->tx_unknown_vlan_memship_check_bmp);
      REG_L2FE_FIELD_READ (L2FE_PLE_TX_UNKWN_VLAN_DFT_2, untag_bm,
    		  cfg->tx_unknown_vlan_untag_bmp);
      REG_L2FE_FIELD_READ (L2FE_PLE_TX_UNKWN_VLAN_DFT_1, stp_mode,
    		  cfg->tx_unknown_vlan_stp_mode_forward_ena);

      REG_L2FE_FIELD_READ (L2FE_PLC_DPID_FWD_CTRL, unkvlan_fwd,
           cfg->unknown_vlan_fwd_ena);
      REG_L2FE_FIELD_READ (L2FE_PLC_DPID_FWD_CTRL, vid_fff_fwd,
           cfg->vlan_4095_fwd_ena);
  }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLC_VLAN_MODE_CTRL, l2e_glb_vlan_mode,
    		  cfg->vlan_mode);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PP_TPID_CMP_S, tpid, cfg->cmp_tpid_svlan);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PP_TPID_CMP_C, tpid, cfg->cmp_tpid_cvlan);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PP_TPID_CMP_O, tpid, cfg->cmp_tpid_other);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PE_CFG_VLAN_TPID_A, tpid, cfg->tx_tpid0);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PE_CFG_VLAN_TPID_B, tpid, cfg->tx_tpid1);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PE_CFG_VLAN_TPID_C, tpid, cfg->tx_tpid2);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PE_CFG_VLAN_TPID_D, tpid, cfg->tx_tpid3);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_CTL_REG, txvlan_membshp_chk_en_wan,
    		  cfg->wan_memship_check_ena);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_CTL_REG, txvlan_membshp_chk_en_lan,
    		  cfg->lan_memship_check_ena);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_RX_UNKWN_VLAN_DFT, memshp_bm,
    		  cfg->rx_unknown_vlan_memship_check_bmp);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_RX_UNKWN_VLAN_DFT, vl_grp_id,
    		  cfg->rx_unknown_vlan_grp_id);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_TX_UNKWN_VLAN_DFT_2, memshp_bm,
    		  cfg->tx_unknown_vlan_memship_check_bmp);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_TX_UNKWN_VLAN_DFT_2, untag_bm,
    		  cfg->tx_unknown_vlan_untag_bmp);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLE_TX_UNKWN_VLAN_DFT_1, stp_mode,
    		  cfg->tx_unknown_vlan_stp_mode_forward_ena);

      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLC_DPID_FWD_CTRL, unkvlan_fwd,
           cfg->unknown_vlan_fwd_ena);
      CA_8279_REG_L2FE_FIELD_READ (SATURN_L2FE_PLC_DPID_FWD_CTRL, vid_fff_fwd,
           cfg->vlan_4095_fwd_ena);
#endif
  }

#if 0//yocto
  __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

  return ret;
}

ca_status_t aal_l2_vlan_port_memship_map_set(CA_IN ca_device_id_t device_id,CA_IN ca_uint32_t logic_port, CA_IN ca_uint32_t mapped_port)
{
    if(mapped_port > AAL_L2FE_MEMSHIP_MAX){
        return CA_E_PARAM;
    }

	AAL_LPID_CHECK(logic_port);

#if 0//yocto
    __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if(0 == device_id){
        CA_NE_REG_WRITE(mapped_port, L2FE_PLE_MMSHP_CHK_ID_MAP_DATA);
        WRITE_INDIRCT_REG (device_id, logic_port, L2FE_PLE_MMSHP_CHK_ID_MAP_ACCESS);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(mapped_port, L2FE_PLE_MMSHP_CHK_ID_MAP_DATA);
        CA_8279_WRITE_INDIRCT_REG (device_id, logic_port, L2FE_PLE_MMSHP_CHK_ID_MAP_ACCESS);
#endif
    }

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    return CA_E_OK;
}

ca_status_t aal_l2_vlan_port_memship_map_get(CA_IN ca_device_id_t device_id,CA_IN ca_uint32_t logic_port, CA_OUT ca_uint32_t *mapped_port)
{
    if(mapped_port == NULL){
        return CA_E_PARAM;
    }

	AAL_LPID_CHECK(logic_port);

#if 0//yocto
    __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if(0 == device_id){
        READ_INDIRCT_REG (device_id, logic_port, L2FE_PLE_MMSHP_CHK_ID_MAP_ACCESS);
        *mapped_port = CA_NE_REG_READ(L2FE_PLE_MMSHP_CHK_ID_MAP_DATA);
    }else if( 1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG (device_id, logic_port, L2FE_PLE_MMSHP_CHK_ID_MAP_ACCESS);
        *mapped_port = CA_8279_NE_REG_READ(L2FE_PLE_MMSHP_CHK_ID_MAP_DATA);
#endif
    }

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    return CA_E_OK;
}


void aal_l2_vlan_init(ca_device_id_t device_id)
{
    aal_l2_vlan_fib_mask_t l2_vlan_fib_mask;
    aal_l2_vlan_fib_t      l2_vlan_fib;
    aal_l2_vlan_default_cfg_mask_t def_cfg_msk;
    aal_l2_vlan_default_cfg_t def_cfg;
    aal_arb_mc_fib_mask_t     mc_fib_mask;
    aal_arb_mc_fib_t          mc_fib_cfg;
    ca_uint8_t             fib_id = 0;
    ca_int16_t             mc_fib_id = 0;
    ca_uint32_t            lpid = 0;
    aal_l2_vlan_insert_tag_num_t insert_tag_num;
    ca_uint32_t i = 0;

    aal_l2_vlan_port_tx_tpid_sel_mask_t             tx_tpid_sel_mask;
    aal_l2_vlan_port_tx_tpid_sel_t                  tx_tpid_sel;

    memset(&mc_fib_cfg, 0, sizeof(aal_arb_mc_fib_t));
    memset(&l2_vlan_fib, 0, sizeof(aal_l2_vlan_fib_t));

    l2_vlan_fib_mask.u32 = 0;
    l2_vlan_fib_mask.s.stp_stat = 1;
    l2_vlan_fib.stp_stat = AAL_L2_VLAN_STP_STAT_FORWARD_AND_LEARN;

#if 0//yocto
    if(CA_OSAL_OK != ca_spin_lock_init(&g_l2_vlan_lock, SPINLOCK_CMN)){
        ca_printf("%s: init spinlock fail\n", __func__);
        return CA_E_INIT;
    }
#else//sd1 uboot for 98f
#endif

    for(fib_id = 0; fib_id<128; fib_id++){

#if CONFIG_98F_UBOOT_NE_DBG
        printf("%s(%d)\tvlan fib init #%#x\n", __func__, __LINE__, fib_id);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, fib_id, l2_vlan_fib_mask, &l2_vlan_fib);
        aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_CVLAN, fib_id, l2_vlan_fib_mask, &l2_vlan_fib);
        aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, fib_id, l2_vlan_fib_mask, &l2_vlan_fib);
    }
    //__g_l2_vlan_port_map[AAL_LPORT_L3_WAN] = __g_l2_vlan_port_map[AAL_LPORT_ETH_WAN];
    for(lpid = 0; lpid<__AAL_L2_VLAN_LOGIC_PORT_NUM; lpid++){
        aal_l2_vlan_port_memship_map_set(device_id, lpid, __g_l2_vlan_port_map[lpid]);

        tx_tpid_sel_mask.u32 = 0;
        tx_tpid_sel_mask.s.svlan = 1;
        tx_tpid_sel.svlan = AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A;
        tx_tpid_sel_mask.s.cvlan = 1;
        tx_tpid_sel.cvlan = AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A;
        aal_l2_vlan_tx_tpid_sel_set(device_id, lpid, tx_tpid_sel_mask, &tx_tpid_sel);
    }

    insert_tag_num.tag_num = 2;
    for(lpid = 0; lpid<12; lpid++){
        aal_l2_vlan_insert_tag_num_set(device_id, lpid, &insert_tag_num);
    }

    memset(&def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    def_cfg_msk.u32 = 0;

    def_cfg_msk.s.vlan_mode = 1;
    def_cfg_msk.s.rx_unknown_vlan_memship_check_bmp = 1;
    def_cfg_msk.s.tx_unknown_vlan_memship_check_bmp = 1;
    def_cfg_msk.s.tx_unknown_vlan_stp_mode_forward_ena = 1;
    def_cfg_msk.s.wan_memship_check_ena = 1;
    def_cfg_msk.s.lan_memship_check_ena = 1;
    def_cfg_msk.s.unknown_vlan_fwd_ena = 1;
    def_cfg_msk.s.rx_unknown_vlan_grp_id = 1;
    def_cfg_msk.s.tx_tpid0 = 1;
    def_cfg_msk.s.tx_tpid1 = 1;
    def_cfg_msk.s.tx_tpid2 = 1;
    def_cfg_msk.s.tx_tpid3 = 1;

    def_cfg.vlan_mode = AAL_L2_VLAN_MODE_SVL;
    def_cfg.tx_unknown_vlan_stp_mode_forward_ena = 1;
    def_cfg.wan_memship_check_ena = TRUE;
    def_cfg.lan_memship_check_ena = TRUE;
    def_cfg.unknown_vlan_fwd_ena  = TRUE;
    def_cfg.tx_tpid0 = 0x8100;
    def_cfg.tx_tpid1 = 0x8100;
    def_cfg.tx_tpid2 = 0x8100;
    def_cfg.tx_tpid3 = 0x8100;


    /* Create and config default flooding domain for UUC/BC/UMC */
#if 0//yocto
    if (CA_E_OK != scfg_read(0, CFG_ID_FLOODING_DOMAIN_1,
        CFG_ID_FLOODING_DOMAIN_LEN * sizeof(ca_uint32_t),
        (ca_uint8_t *) g_vlan_flooding_domain_1)) {
        printk("SCFG read %s failed!!!\n", CFG_ID_FLOODING_DOMAIN_1);
    }
    if (CA_E_OK != scfg_read(0, CFG_ID_FLOODING_DOMAIN_2,
        CFG_ID_FLOODING_DOMAIN_LEN * sizeof(ca_uint32_t),
        (ca_uint8_t *) g_vlan_flooding_domain_2)) {
        printk("SCFG read %s failed!!!\n", CFG_ID_FLOODING_DOMAIN_2);
    }
#else//sd1 uboot for 98f

#if 0//yocto
    g_vlan_flooding_domain_1[0] = 0xFF;
    g_vlan_flooding_domain_1[1] = 0;//LAN port 0
    g_vlan_flooding_domain_1[2] = 1;//LAN port 1
    g_vlan_flooding_domain_1[3] = 2;//LAN port 2
    g_vlan_flooding_domain_1[4] = 4;//LAN port 4
    g_vlan_flooding_domain_1[5] = 0x11;
#else//sd1 uboot for 98f
    g_vlan_flooding_domain_1[0] = 0;//LAN port 0
    g_vlan_flooding_domain_1[1] = 1;//LAN port 1
    g_vlan_flooding_domain_1[2] = 2;//LAN port 2
    g_vlan_flooding_domain_1[3] = 3;//LAN port 3
    g_vlan_flooding_domain_1[4] = 4;//LAN port 4
    g_vlan_flooding_domain_1[5] = 0xFF;
#endif//sd1 uboot for 98f
    g_vlan_flooding_domain_1[6] = 0xFF;
    g_vlan_flooding_domain_1[7] = 0xFF;

#if 0//yocto
    g_vlan_flooding_domain_2[0] = 3;//WAN port 3
#else//sd1 uboot for 98f
    g_vlan_flooding_domain_2[0] = 0xFF;
#endif//sd1 uboot for 98f
    g_vlan_flooding_domain_2[1] = 0xFF;
    g_vlan_flooding_domain_2[2] = 0xFF;
    g_vlan_flooding_domain_2[3] = 0xFF;
    g_vlan_flooding_domain_2[4] = 0xFF;
    g_vlan_flooding_domain_2[5] = 0xFF;
    g_vlan_flooding_domain_2[6] = 0xFF;
    g_vlan_flooding_domain_2[7] = 0xFF;
#endif//sd1 uboot for 98f

    g_vlan_mcgid_0 = aal_flooding_group_create();
    g_vlan_mcgid_1 = aal_flooding_group_create();

#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) g_vlan_mcgid_0: %#x\n", __func__, __LINE__, g_vlan_mcgid_0);
    printf("%s(%d) g_vlan_mcgid_1: %#x\n", __func__, __LINE__, g_vlan_mcgid_1);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    def_cfg.rx_unknown_vlan_grp_id = g_vlan_mcgid_0;

    for (i = 0; i < CFG_ID_FLOODING_DOMAIN_LEN; i++){
        if (g_vlan_flooding_domain_1[i] != 0xFF){
            mc_fib_id = aal_flooding_fib_idx_alloc();

            g_l2_vlan_unknown_port_fib_map[g_vlan_flooding_domain_1[i]] = mc_fib_id;
            __g_unkown_vlan_mc_fib_bmp |= 1<<mc_fib_id;

            aal_flooding_mc_fib_add(g_vlan_mcgid_0,mc_fib_id);
            mc_fib_cfg.ldpid = g_vlan_flooding_domain_1[i];
            mc_fib_mask.s.ldpid = 1;
#ifdef MC_FIB_COS_CMD
            mc_fib_mask.s.cos_cmd = 1;
            mc_fib_cfg.cos_cmd = 1;
#endif

#if CONFIG_98F_UBOOT_NE_DBG
            printf("%s(%d) mc_fib_cfg.ldpid: %#x\n", __func__, __LINE__, mc_fib_cfg.ldpid);
            printf("%s(%d) g_vlan_flooding_domain_1[%d]: %#x\n", __func__, __LINE__, i, g_vlan_flooding_domain_1[i]);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

            aal_arb_mc_fib_set(device_id,(ca_uint32_t)mc_fib_id,mc_fib_mask,&mc_fib_cfg);
            def_cfg.tx_unknown_vlan_memship_check_bmp |= (1 << __g_l2_vlan_port_map[g_vlan_flooding_domain_1[i]]);
            def_cfg.rx_unknown_vlan_memship_check_bmp |= (1 << __g_l2_vlan_port_map[g_vlan_flooding_domain_1[i]]);

#if CONFIG_98F_UBOOT_NE_DBG
            printf("%s(%d) mc_fib_id: %#x\n", __func__, __LINE__, mc_fib_id);
            printf("%s(%d) g_l2_vlan_unknown_port_fib_map[g_vlan_flooding_domain_1[%d]: %d] = %#x\n",
                __func__, __LINE__,
                i,     g_vlan_flooding_domain_1[i],
                g_l2_vlan_unknown_port_fib_map[g_vlan_flooding_domain_1[i]]);
            printf("%s(%d) __g_unkown_vlan_mc_fib_bmp: %#x\n", __func__, __LINE__, __g_unkown_vlan_mc_fib_bmp);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }
    }
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s(%d) def_cfg.tx_unknown_vlan_memship_check_bmp: %#x\n", __func__, __LINE__, (unsigned int)def_cfg.tx_unknown_vlan_memship_check_bmp);
    printf("%s(%d) def_cfg.rx_unknown_vlan_memship_check_bmp: %#x\n", __func__, __LINE__, (unsigned int)def_cfg.rx_unknown_vlan_memship_check_bmp);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    def_cfg.tx_unknown_vlan_memship_check_bmp |= (1 << __g_l2_vlan_port_map[0x18]);
    def_cfg.rx_unknown_vlan_memship_check_bmp |= (1 << __g_l2_vlan_port_map[0x18]);

#if 0//yocto
#if 1//CONFIG_98F_UBOOT_NE_DBG
printf("%s(%d) def_cfg.tx_unknown_vlan_memship_check_bmp: %#x\n", __func__, __LINE__, (unsigned int)def_cfg.tx_unknown_vlan_memship_check_bmp);
printf("%s(%d) def_cfg.rx_unknown_vlan_memship_check_bmp: %#x\n", __func__, __LINE__, (unsigned int)def_cfg.rx_unknown_vlan_memship_check_bmp);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

	/*for (i = 0; i < CFG_ID_FLOODING_DOMAIN_LEN; i++){
		if (flooding_domain_2[i] != 0xFF)
			aal_flooding_port_add(mcgid_1, flooding_domain_2[i]);
	}*/
#else//sd1 uboot for 98f - do not add cpu ports
#endif

    aal_l2_vlan_default_cfg_set(device_id, def_cfg_msk, &def_cfg);

    l2_vlan_fib_mask.u32 = 0;
    l2_vlan_fib_mask.s.vlan_group_id = 1;
    l2_vlan_fib.vlan_group_id = g_vlan_mcgid_1;

    aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, 127, l2_vlan_fib_mask, &l2_vlan_fib);
    aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_CVLAN, 127, l2_vlan_fib_mask, &l2_vlan_fib);
    aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, 127, l2_vlan_fib_mask, &l2_vlan_fib);
    REG_L2FE_FIELD_WRITE(L2FE_PP_DEBUG_BUS, ni2fe_pkt_cnt_wrap_en, 1);
    REG_L2FE_FIELD_WRITE(L2FE_PE_DEBUG, fe2tm_pkt_cnt_wrap_en , 1);

}

ca_status_t aal_ni_heada_get(CA_IN  ca_device_id_t device_id,
                                                    CA_IN  ca_uint32_t ca_ni_port,
                                                    CA_OUT aal_l2_fe_heada_t *heada)
{
    if(heada == NULL){
        return CA_E_PARAM;
    }

    if( (ca_ni_port < AAL_LPORT_ETH_NI0) || (ca_ni_port > AAL_LPORT_ETH_NI4) ){
        ca_printf("\tca_ni_port: %d is not a valid lspid, it should be 0~4.\n", ca_ni_port);
        return CA_E_PARAM;
    }

    NI_HV_GLB_RXMUX_CTRL_CFG_t ni_hv_glb_rxmux_ctrl_cfg;
    ni_hv_glb_rxmux_ctrl_cfg.wrd = CA_NE_REG_READ(NI_HV_GLB_RXMUX_CTRL_CFG);
    ni_hv_glb_rxmux_ctrl_cfg.bf.dbg_sel = ca_ni_port;
    CA_NE_REG_WRITE(ni_hv_glb_rxmux_ctrl_cfg.wrd, NI_HV_GLB_RXMUX_CTRL_CFG);

    ca_uint32 hdra_low, hdra_mid, hdra_high;
    hdra_high = NI_HV_GLB_RXMUX_LAST_HDR_A2;
    hdra_mid = NI_HV_GLB_RXMUX_LAST_HDR_A1;
    hdra_low = NI_HV_GLB_RXMUX_LAST_HDR_A0;

    if(0 == device_id){
        heada->heada_hi         = CA_NE_REG_READ(hdra_high);
        heada->heada_mid        = CA_NE_REG_READ(hdra_mid);
        heada->heada_low        = CA_NE_REG_READ(hdra_low);
        heada->cpu_flag         = (CA_NE_REG_READ(hdra_high)>>31)&1;
        heada->deep_q           = (CA_NE_REG_READ(hdra_high)>>30)&1;
        heada->policy_ena       = (CA_NE_REG_READ(hdra_high)>>16)&3;
        heada->policy_group_id  = (CA_NE_REG_READ(hdra_high)>>27)&7;
        heada->policy_id        = (CA_NE_REG_READ(hdra_high)>>18)&0x1ff;
        heada->marked           = (CA_NE_REG_READ(hdra_high)>>15)&1;
        heada->mirror           = (CA_NE_REG_READ(hdra_high)>>14)&1;
        heada->no_drop          = (CA_NE_REG_READ(hdra_high)>>13)&1;
        heada->rx_packet_type   = (CA_NE_REG_READ(hdra_high)>>11)&3;
        heada->drop_code        = (CA_NE_REG_READ(hdra_high)>>8)&7;
        heada->mc_group_id      = (CA_NE_REG_READ(hdra_high)>>0)&0xff;
        heada->header_type      = (CA_NE_REG_READ(hdra_mid)>>30)&3;
        heada->fe_bypass        = (CA_NE_REG_READ(hdra_mid)>>29)&1;
        heada->packet_size      = (CA_NE_REG_READ(hdra_mid)>>15)&0x3fff;
        heada->cos              = (CA_NE_REG_READ(hdra_mid) >> 0)&0x7;
        heada->logic_spid       = (CA_NE_REG_READ(hdra_mid)>>9) &0x3f;
        heada->logic_dpid       = (CA_NE_REG_READ(hdra_mid)>>3)&0x3f;
    }

    heada->heada_hi         = CA_NE_REG_READ(hdra_high);
    heada->heada_mid        = CA_NE_REG_READ(hdra_mid);
    heada->heada_low        = CA_NE_REG_READ(hdra_low);

    return CA_E_OK;
}

ca_status_t aal_l2_fe_pp_heada_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_heada_t *heada)
{
    if(heada == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        heada->heada_hi         = CA_NE_REG_READ(L2FE_PP_HEADER_A_HI);
        heada->heada_mid        = CA_NE_REG_READ(L2FE_PP_HEADER_A_MID);
        heada->heada_low        = CA_NE_REG_READ(L2FE_PP_HEADER_A_LOW);
        heada->cpu_flag         = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>31)&1;
        heada->deep_q           = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>30)&1;
        heada->policy_ena       = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>16)&3;
        heada->policy_group_id  = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>27)&7;
        heada->policy_id        = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>18)&0x1ff;
        heada->marked           = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>15)&1;
        heada->mirror           = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>14)&1;
        heada->no_drop          = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>13)&1;
        heada->rx_packet_type   = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>11)&3;
        heada->drop_code        = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>8)&7;
        heada->mc_group_id      = (CA_NE_REG_READ(L2FE_PP_HEADER_A_HI)>>0)&0xff;
        heada->header_type      = (CA_NE_REG_READ(L2FE_PP_HEADER_A_MID)>>30)&3;
        heada->fe_bypass        = (CA_NE_REG_READ(L2FE_PP_HEADER_A_MID)>>29)&1;
        heada->packet_size      = (CA_NE_REG_READ(L2FE_PP_HEADER_A_MID)>>15)&0x3fff;
        heada->cos              = (CA_NE_REG_READ(L2FE_PP_HEADER_A_MID) >> 0)&0x7;
        heada->logic_spid       = (CA_NE_REG_READ(L2FE_PP_HEADER_A_MID)>>9) &0x3f;
        heada->logic_dpid       = (CA_NE_REG_READ(L2FE_PP_HEADER_A_MID)>>3)&0x3f;
    }

    heada->heada_hi         = CA_NE_REG_READ(L2FE_PP_HEADER_A_HI);
    heada->heada_mid        = CA_NE_REG_READ(L2FE_PP_HEADER_A_MID);
    heada->heada_low        = CA_NE_REG_READ(L2FE_PP_HEADER_A_LOW);

    return CA_E_OK;
}


ca_status_t aal_l2_fe_pp_nierr_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_ni_status_t *err)
{
    if(err == NULL){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        err->sop_eop_mismatch   = CA_NE_REG_READ(L2FE_PP_NI_INTF_ERR_STTS_0);
        err->runt_l2_packet     = CA_NE_REG_READ(L2FE_PP_NI_INTF_ERR_STTS_1);
        err->sob_eob_mismatch   = CA_NE_REG_READ(L2FE_PP_NI_INTF_ERR_STTS_2);
        err->sop_eop_gap        = CA_NE_REG_READ(L2FE_PP_NI_INTF_ERR_STTS_3);
        err->pp_pingpong_overflow = CA_NE_REG_READ(L2FE_PP_PINGPONG_STS)&1;
        err->ready              = CA_NE_REG_READ(L2FE_PP_NI_INTF_STS)&1;
        err->drop_cnt           = CA_NE_REG_READ(L2FE_PP_NI_INTF_DROP_CNT)&0xffff;
        err->sop_cnt            = (CA_NE_REG_READ(L2FE_PP_NI_INTF_PKT_CNT)>>16)&0xffff;
        err->eop_cnt           = CA_NE_REG_READ(L2FE_PP_NI_INTF_PKT_CNT)&0xffff;
    }

    return CA_E_OK;
}

ca_status_t aal_l2_fe_pe_heada_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_heada_t *heada)
{
    if(heada == NULL){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        heada->heada_hi         = CA_NE_REG_READ(L2FE_PE_HEADER_A_HI);
        heada->heada_mid        = CA_NE_REG_READ(L2FE_PE_HEADER_A_MID);
        heada->heada_low        = CA_NE_REG_READ(L2FE_PE_HEADER_A_LOW);
        heada->cpu_flag         = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>31)&1;
        heada->deep_q           = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>30)&1;
        heada->policy_ena       = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>16)&3;
        heada->policy_group_id  = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>27)&7;
        heada->policy_id        = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>18)&0x1ff;
        heada->marked           = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>15)&1;
        heada->mirror           = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>14)&1;
        heada->no_drop          = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>13)&1;
        heada->rx_packet_type   = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>11)&3;
        heada->drop_code        = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>8)&7;
        heada->mc_group_id      = (CA_NE_REG_READ(L2FE_PE_HEADER_A_HI)>>0)&0xff;
        heada->header_type      = (CA_NE_REG_READ(L2FE_PE_HEADER_A_MID)>>30)&3;
        heada->fe_bypass        = (CA_NE_REG_READ(L2FE_PE_HEADER_A_MID)>>29)&1;
        heada->packet_size      = (CA_NE_REG_READ(L2FE_PE_HEADER_A_MID)>>15)&0x3fff;
        heada->cos              = (CA_NE_REG_READ(L2FE_PE_HEADER_A_MID) >> 0)&0x7;
        heada->logic_spid       = (CA_NE_REG_READ(L2FE_PE_HEADER_A_MID)>>9) &0x3f;
        heada->logic_dpid       = (CA_NE_REG_READ(L2FE_PE_HEADER_A_MID)>>3)&0x3f;
    }

    return CA_E_OK;
}


ca_status_t aal_l2_fe_drop_source_cnt_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_drop_source_t *drop_source_cnt)
{
        ca_uint8_t drop_source_id;
        ca_uint32_t *p = (ca_uint32_t*)drop_source_cnt;

        if(drop_source_cnt == NULL){
                return CA_E_PARAM;
        }

        if(0 == device_id){
                for(drop_source_id = 0; drop_source_id<32; drop_source_id++){
                        READ_INDIRCT_REG (device_id, drop_source_id, L2FE_PE_DROP_STTS_ACCESS);
                        *p =  CA_NE_REG_READ(L2FE_PE_DROP_STTS_DATA);
                        p++;
                }
        }

        if(0 == device_id){
                for(drop_source_id = 0; drop_source_id<32; drop_source_id++){
                        CA_NE_REG_WRITE(0,L2FE_PE_DROP_STTS_DATA);
                        WRITE_INDIRCT_REG (device_id, drop_source_id, L2FE_PE_DROP_STTS_ACCESS);
                }
        }

        return CA_E_OK;
}
ca_status_t aal_l2_pe2tm_pkt_cnt_set(CA_IN  ca_device_id_t device_id,
                                      CA_IN ca_pe2tm_cnt_mask_t mask,
                                      CA_IN ca_pe2tm_cnt_t *cnt)
{

    if(NULL == cnt){
        return CA_E_PARAM;
    }

    //temp_value = CA_NE_REG_READ(L2FE_PE_TM_INTERFACE_PKT_CNT);
    if(0 == device_id){
        CA_NE_REG_WRITE(0,L2FE_PE_TM_INTERFACE_PKT_CNT);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(0,L2FE_PE_TM_INTERFACE_PKT_CNT);
#endif
    }
    return CA_E_OK;
}


ca_status_t aal_l2_pe2tm_pkt_cnt_get(CA_IN  ca_device_id_t device_id,
                                      CA_OUT ca_pe2tm_cnt_t *cnt)
{
    ca_uint32_t temp_value = 0;

    if(NULL == cnt){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        temp_value = CA_NE_REG_READ(L2FE_PE_TM_INTERFACE_PKT_CNT);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        temp_value = CA_8279_NE_REG_READ(L2FE_PE_TM_INTERFACE_PKT_CNT);
#endif
    }
    cnt->tm2fe_ready = (temp_value >>31) &1;
    cnt->sop_cnt     = (temp_value>>16) &0x7fff;
    cnt->eop_cnt     = (temp_value>>0) &0x7fff;
    if(0 == device_id){
        CA_NE_REG_WRITE(0,L2FE_PE_TM_INTERFACE_PKT_CNT);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(0,L2FE_PE_TM_INTERFACE_PKT_CNT);
#endif
    }
    return CA_E_OK;
}



ca_status_t aal_l2_ni2pp_pkt_cnt_set(CA_IN  ca_device_id_t device_id,
                                      CA_IN ca_ni2pp_cnt_t *cnt)
{

    if(NULL == cnt){
        return CA_E_PARAM;
    }

    //temp_value = CA_NE_REG_READ(L2FE_PE_TM_INTERFACE_PKT_CNT);
    if(0 == device_id){
        CA_NE_REG_WRITE(0,L2FE_PP_NI_INTF_DROP_CNT);
        CA_NE_REG_WRITE(0,L2FE_PP_NI_INTF_PKT_CNT);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(0,L2FE_PP_NI_INTF_DROP_CNT);
        CA_8279_NE_REG_WRITE(0,L2FE_PP_NI_INTF_PKT_CNT);
#endif
    }
    return CA_E_OK;
}


ca_status_t aal_l2_ni2pp_pkt_cnt_get(CA_IN  ca_device_id_t device_id,
                                      CA_OUT ca_ni2pp_cnt_t *cnt)
{
    ca_uint32_t temp_value = 0;

    if(NULL == cnt){
        return CA_E_PARAM;
    }

    if(0 == device_id){
        cnt->drp_cnt = CA_NE_REG_READ(L2FE_PP_NI_INTF_DROP_CNT);
        temp_value = CA_NE_REG_READ(L2FE_PP_NI_INTF_PKT_CNT);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        cnt->drp_cnt = CA_8279_NE_REG_READ(L2FE_PP_NI_INTF_DROP_CNT);
        temp_value = CA_8279_NE_REG_READ(L2FE_PP_NI_INTF_PKT_CNT);
#endif
    }

    cnt->sop_cnt     = (temp_value>>16) &0x7fff;
    cnt->eop_cnt     = (temp_value>>0) &0x7fff;
    if(0 == device_id){
        CA_NE_REG_WRITE(0,L2FE_PP_NI_INTF_DROP_CNT);
        CA_NE_REG_WRITE(0,L2FE_PP_NI_INTF_PKT_CNT);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(0,L2FE_PP_NI_INTF_DROP_CNT);
        CA_8279_NE_REG_WRITE(0,L2FE_PP_NI_INTF_PKT_CNT);
#endif
    }
    return CA_E_OK;
}


ca_status_t aal_l2_vlan_tx_tpid_sel_set(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_IN aal_l2_vlan_port_tx_tpid_sel_mask_t mask,
                                        CA_IN aal_l2_vlan_port_tx_tpid_sel_t  *cfg)
{
    L2FE_PE_TPID_SEL_CONFIG_TBL_DATA_t tpid_sel;

    if(NULL == cfg){
        return CA_E_PARAM;
    }

    if(port_id>63){
        return CA_E_PARAM;
    }

#if 0//yocto
    __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,port_id, L2FE_PE_TPID_SEL_CONFIG_TBL_ACCESS);
        tpid_sel.wrd = CA_NE_REG_READ(L2FE_PE_TPID_SEL_CONFIG_TBL_DATA);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(device_id,port_id, L2FE_PE_TPID_SEL_CONFIG_TBL_ACCESS);
        tpid_sel.wrd = CA_8279_NE_REG_READ(L2FE_PE_TPID_SEL_CONFIG_TBL_DATA);
#endif
    }

    if(mask.s.cvlan){
        tpid_sel.bf.ctag = cfg->cvlan;
    }
    if(mask.s.svlan){
        tpid_sel.bf.stag = cfg->svlan;
    }
#ifdef CONFIG_ARCH_CORTINA_G3LITE
    if(mask.s.inner_cvlan){
        tpid_sel.bf.inner_ctag = cfg->inner_cvlan;
    }
    if(mask.s.inner_svlan){
        tpid_sel.bf.inner_stag = cfg->inner_svlan;
    }
#endif
    if(0 == device_id){
        CA_NE_REG_WRITE(tpid_sel.wrd,L2FE_PE_TPID_SEL_CONFIG_TBL_DATA);
        WRITE_INDIRCT_REG(device_id,port_id, L2FE_PE_TPID_SEL_CONFIG_TBL_ACCESS);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(tpid_sel.wrd,L2FE_PE_TPID_SEL_CONFIG_TBL_DATA);
        CA_8279_WRITE_INDIRCT_REG(device_id,port_id, L2FE_PE_TPID_SEL_CONFIG_TBL_ACCESS);
#endif
    }

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    return CA_E_OK;
}

ca_status_t aal_l2_vlan_tx_tpid_sel_get(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_OUT aal_l2_vlan_port_tx_tpid_sel_t  *cfg)
{
    L2FE_PE_TPID_SEL_CONFIG_TBL_DATA_t tpid_sel;

    if(NULL == cfg){
        return CA_E_PARAM;
    }

    if(port_id>63){
        return CA_E_PARAM;
    }

#if 0//yocto
    __L2_VLAN_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if(0 == device_id){
        READ_INDIRCT_REG(device_id,port_id, L2FE_PE_TPID_SEL_CONFIG_TBL_ACCESS);
        tpid_sel.wrd = CA_NE_REG_READ(L2FE_PE_TPID_SEL_CONFIG_TBL_DATA);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(device_id,port_id, L2FE_PE_TPID_SEL_CONFIG_TBL_ACCESS);
        tpid_sel.wrd = CA_8279_NE_REG_READ(L2FE_PE_TPID_SEL_CONFIG_TBL_DATA);
#endif
    }
    cfg->cvlan = tpid_sel.bf.ctag;
    cfg->svlan = tpid_sel.bf.stag;
#ifdef CONFIG_ARCH_CORTINA_G3LITE
    cfg->inner_cvlan = tpid_sel.bf.inner_ctag;
    cfg->inner_svlan = tpid_sel.bf.inner_stag;
#endif

#if 0//yocto
    __L2_VLAN_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    return CA_E_OK;
}

ca_status_t aal_l2_vlan_insert_tag_num_set(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_IN aal_l2_vlan_insert_tag_num_t *cfg)
{
    NI_HV_GLB_RX_SEG_VLAN_TAG_INSERT_CFG_t ni_vlan_insert;
    if(NULL == cfg){
        return CA_E_PARAM;
    }

    if(port_id>11){
        return CA_E_PARAM;
    }

    if( 0 == device_id){
        ni_vlan_insert.wrd = CA_NE_REG_READ(NI_HV_GLB_RX_SEG_VLAN_TAG_INSERT_CFG);
        ni_vlan_insert.wrd &= ~(3 << (port_id*2));
        ni_vlan_insert.wrd |= cfg->tag_num << (port_id*2);

        CA_NE_REG_WRITE(ni_vlan_insert.wrd,NI_HV_GLB_RX_SEG_VLAN_TAG_INSERT_CFG);
    }

    return CA_E_OK;
}

ca_status_t aal_l2_vlan_insert_tag_num_get(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_OUT aal_l2_vlan_insert_tag_num_t *cfg)
{
    NI_HV_GLB_RX_SEG_VLAN_TAG_INSERT_CFG_t ni_vlan_insert;
    if(NULL == cfg){
        return CA_E_PARAM;
    }

    if(port_id>11){
        return CA_E_PARAM;
    }
    if(0 == device_id){
        ni_vlan_insert.wrd = CA_NE_REG_READ(NI_HV_GLB_RX_SEG_VLAN_TAG_INSERT_CFG);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        ni_vlan_insert.wrd = CA_8279_NE_REG_READ(NI_HV_GLB_RX_SEG_VLAN_TAG_INSERT_CFG);
#endif
    }
    cfg->tag_num =  (ni_vlan_insert.wrd >> (port_id*2)) & 3;

    return CA_E_OK;
}

ca_status_t aal_l2_unknown_vlan_mc_fib_bmp_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT ca_uint64_t *bmp)
{
    if(bmp == NULL){
        return CA_E_PARAM;
    }
    *bmp = __g_unkown_vlan_mc_fib_bmp;
    return CA_E_OK;
}

#if defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
ca_status_t __xgpon_setup_vlan_traffic(
    ca_boolean_t is_add,
    ca_uint16_t vlan,
    ca_uint16_t lan_port)
{
    aal_l2_vlan_fib_mask_t     fib_mask;
    aal_l2_vlan_fib_t          fib;
    aal_l2_vlan_fib_map_mask_t map_mask;
    aal_l2_vlan_fib_map_t      map;
    ca_status_t                ret = CA_E_OK;
    ca_uint8_t                 fib_id = lan_port + 100;

    fib_mask.u32 = 0;
    map_mask.u32 = 0;

    memset(&fib, 0, sizeof(fib));
    memset(&map, 0, sizeof(map));


    fib_mask.s.vlan_id = 1;
    fib_mask.s.permit = 1;
    fib_mask.s.stp_stat = 1;
    fib_mask.s.dpid_field_vld = 1;
    fib_mask.s.dpid = 1;
    fib_mask.s.memship_bm = 1;
    fib_mask.s.is_svlan = 1;

    fib.vlan_id = is_add ? vlan : 0;
    fib.permit = 1;
    fib.stp_stat = 1;
    fib.dpid_field_vld = 1;
    fib.dpid = 4; /* The port 4 is used to connect Saturn to G3 */
    fib.memship_bm = 0xFF;
    fib.is_svlan = 1;

    ret = aal_l2_vlan_action_cfg_set (0,
                AAL_L2_VLAN_TYPE_LAN_VLAN,
                fib_id,
                fib_mask,
                &fib);

    map_mask.s.fib_id = 1;
    map_mask.s.valid  = 1;

    map.fib_id = fib_id;
    map.valid  = is_add;

    if (CA_E_OK == ret) {

        ret = aal_l2_vlan_fib_map_set(0,
                AAL_L2_VLAN_TYPE_LAN_VLAN,
                vlan,
                map_mask,
                &map);
    }

    if (CA_E_OK != ret) {
        ca_printf("setup upstream fail, return %d\r\n", ret);
        return ret;
    }

    fib_id = lan_port;
    fib.dpid = lan_port;

    ret = aal_l2_vlan_action_cfg_set (0,
                AAL_L2_VLAN_TYPE_WAN_SVLAN,
                fib_id,
                fib_mask,
                &fib);

    if (CA_E_OK == ret) {

        ret = aal_l2_vlan_fib_map_set(0,
                AAL_L2_VLAN_TYPE_WAN_SVLAN,
                vlan,
                map_mask,
                &map);
    }

    if (CA_E_OK != ret) {
        ca_printf("setup downstream fail, return %d\r\n", ret);
        return ret;
    }

    ca_printf("setup vlan %u lanport %u OK, UPS use fib %u, DNS use fib %u.\n",
        vlan, lan_port, lan_port + 100, lan_port);
    return ret;

}
#endif

ca_status_t aal_l2_vlan_debug_set(
                CA_IN ca_device_id_t device_id,
                CA_IN ca_boolean_t add_del,
                CA_IN ca_uint16_t vlan,
                CA_IN ca_uint16_t lan_port)
{
#if defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    __xgpon_setup_vlan_traffic(add_del, vlan, lan_port);
#endif
    return CA_E_OK;
}
ca_status_t aal_l2_vlan_debug_get(
                CA_IN ca_device_id_t device_id,
                CA_IN ca_boolean_t add_del,
                CA_IN ca_uint16_t vlan,
                CA_OUT ca_uint16_t *lan_port)
{
    // fake api for cli gen
    return CA_E_OK;
}


EXPORT_SYMBOL(aal_l2_unknown_vlan_mc_fib_bmp_get);
EXPORT_SYMBOL(aal_l2_fe_pp_nierr_get);
EXPORT_SYMBOL(aal_l2_ni2pp_pkt_cnt_set);
EXPORT_SYMBOL(aal_l2_vlan_fib_map_get);
EXPORT_SYMBOL(aal_l2_ni2pp_pkt_cnt_get);
EXPORT_SYMBOL(aal_l2_vlan_action_cfg_get);
EXPORT_SYMBOL(aal_l2_vlan_default_cfg_set);
EXPORT_SYMBOL(aal_l2_vlan_port_memship_map_get);
EXPORT_SYMBOL(aal_l2_vlan_action_cfg_set);
EXPORT_SYMBOL(aal_l2_vlan_fib_map_set);
EXPORT_SYMBOL(aal_l2_vlan_port_memship_map_set);
EXPORT_SYMBOL(aal_l2_fe_drop_source_cnt_get);
EXPORT_SYMBOL(aal_l2_vlan_tx_tpid_sel_set);
EXPORT_SYMBOL(aal_l2_vlan_default_cfg_get);
EXPORT_SYMBOL(aal_l2_vlan_tx_tpid_sel_get);
EXPORT_SYMBOL(aal_l2_fe_pp_heada_get);
EXPORT_SYMBOL(aal_l2_pe2tm_pkt_cnt_set);
EXPORT_SYMBOL(aal_l2_fe_pe_heada_get);
EXPORT_SYMBOL(aal_l2_pe2tm_pkt_cnt_get);
EXPORT_SYMBOL(aal_l2_vlan_insert_tag_num_set);
EXPORT_SYMBOL(aal_l2_vlan_insert_tag_num_get);
EXPORT_SYMBOL(aal_l2_vlan_debug_set);
EXPORT_SYMBOL(aal_l2_vlan_debug_get);
