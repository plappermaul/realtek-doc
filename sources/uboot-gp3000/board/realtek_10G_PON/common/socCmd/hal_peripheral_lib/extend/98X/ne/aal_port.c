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
 * aal_ni.c: Hardware Abstraction Layer for NI driver to access hardware regsiters
 *
 */

#include <generated/ca_ne_autoconf.h>
#include "scfg.h"
#include "aal_port.h"
#include <soc/cortina/registers.h>
#include "osal_common.h"
#include "aal_common.h"
#include "aal_arb.h"
#include "aal_mcast.h"
#include "aal_l3_l2lookup.h"
#include "aal_common.h"

ca_uint __aal_port_lock;
#if 0//yocto
ca_uint8_t wan_port_id = AAL_LPORT_ETH_NI7; // wan port id will be inited by ca_load_init() in ni_drv.

#define __AAL_PORT_LOCK(dev)           ca_spin_lock(__aal_port_lock)
#define __AAL_PORT_UNLOCK(dev)         ca_spin_unlock(__aal_port_lock)
#else//sd1 uboot for 98f
#endif

#define L2LKUP_CHECK(lspid)	(lspid == AAL_LPORT_L3_LAN ? TRUE: (lspid == AAL_LPORT_L3_WAN ? TRUE: FALSE))
#define L2LKUP_IDX(ldpid)	(ldpid == AAL_LPORT_L3_LAN ? 0: (ldpid == AAL_LPORT_L3_WAN ? 1: (ldpid == AAL_LPORT_ETH_WAN ? 2: 3)))
#define L2LKUP_LDPID(lspid)	(lspid == AAL_LPORT_L3_LAN ? AAL_LPORT_L3_WAN: AAL_LPORT_L3_LAN)
ca_uint8_t g_aal_l2vlan_outer_is_s[CFG_ID_L2FE_VLAN_IS_S_LEN] = {
                                                    1,1,1,1,1,1,1,1,
                                                    1,1,1,1,1,1,1,1,
                                                    1,1,1,1,1,1,1,1,
                                                    1,1,1,1,1,1,1,1
};
ca_uint8_t g_aal_l2vlan_inner_is_s[CFG_ID_L2FE_VLAN_IS_S_LEN] = {
                                                    1,1,1,1,1,1,1,1,
                                                    1,1,1,1,1,1,1,1,
                                                    1,1,1,1,1,1,1,1,
                                                    1,1,1,1,1,1,1,1
};

/* physical port to logical port mapping table */
ca_status_t aal_port_ippb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t index,
				CA_IN aal_ippb_cfg_msk_t msk,
				CA_IN aal_ippb_cfg_t * cfg)
{
	L2FE_PP_IPPB_ACCESS_t oper;
	L2FE_PP_IPPB_DATA_t data;

	AAL_LPID_CHECK(index);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&oper, 0, sizeof(L2FE_PP_IPPB_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PP_IPPB_DATA_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, index, L2FE_PP_IPPB_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PP_IPPB_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, index, L2FE_PP_IPPB_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PP_IPPB_DATA);
    }
#endif
	//if(msk.s.wan_ind)
	//{
	//data.bf.wan_ind = cfg->wan_ind > 0;
	//}

	if (msk.s.lspid) {
		data.bf.lspid = cfg->lspid;
	}

    if (dev == 0){
    	CA_L2FE_REG_WRITE(data.wrd, L2FE_PP_IPPB_DATA);
    	WRITE_INDIRCT_REG(dev, index, L2FE_PP_IPPB_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_NE_REG_WRITE(data.wrd, L2FE_PP_IPPB_DATA);
    	CA_8279_WRITE_INDIRCT_REG(dev, index, L2FE_PP_IPPB_ACCESS);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_ippb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t index,
				CA_OUT aal_ippb_cfg_t * cfg)
{
	L2FE_PP_IPPB_ACCESS_t oper;
	L2FE_PP_IPPB_DATA_t data;

	AAL_LPID_CHECK(index);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

	memset((void *)&oper, 0, sizeof(L2FE_PP_IPPB_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PP_IPPB_DATA_t));

	memset(cfg, 0, sizeof(aal_ippb_cfg_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, index, L2FE_PP_IPPB_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PP_IPPB_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, index, L2FE_PP_IPPB_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PP_IPPB_DATA);
    }
#endif
	//cfg->wan_ind = data.bf.wan_ind;
	cfg->lspid = data.bf.lspid;

	return CA_E_OK;
}

/* ingress logical port behavior configuration */
ca_status_t aal_port_ilpb_cfg_set(
    CA_IN ca_device_id_t dev,
    CA_IN ca_uint32_t lpid,
    CA_IN aal_ilpb_cfg_msk_t msk,
    CA_IN aal_ilpb_cfg_t * cfg
)
{
	L2FE_PP_ILPB_ACCESS_t oper;
	L2FE_PP_ILPB_DATA4_t data4;
	L2FE_PP_ILPB_DATA3_t data3;
	L2FE_PP_ILPB_DATA2_t data2;
	L2FE_PP_ILPB_DATA1_t data1;
	L2FE_PP_ILPB_DATA0_t data0;
	l3fe_glb_ilpb_t l2lkup_ilpb;
	ca_uint16_t set_l2lkup, tbl_idx = 3;

	AAL_LPID_CHECK(lpid);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

	memset((void *)&oper, 0, sizeof(L2FE_PP_ILPB_ACCESS_t));
	memset((void *)&data4, 0, sizeof(L2FE_PP_ILPB_DATA4_t));
	memset((void *)&data3, 0, sizeof(L2FE_PP_ILPB_DATA3_t));
	memset((void *)&data2, 0, sizeof(L2FE_PP_ILPB_DATA2_t));
	memset((void *)&data1, 0, sizeof(L2FE_PP_ILPB_DATA1_t));
	memset((void *)&data0, 0, sizeof(L2FE_PP_ILPB_DATA0_t));

	memset((void *)&l2lkup_ilpb, 0, sizeof(l3fe_glb_ilpb_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid, L2FE_PP_ILPB_ACCESS);
    	data4.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA4);
    	data3.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA3);
    	data2.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA2);
    	data1.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA1);
    	data0.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA0);
    }
	/* TODO. other mask cfg need to add */

	if (msk.s.wan_ind) {
		data4.bf.ilpb_wan_ind = cfg->wan_ind;
	}

	if (msk.s.default_vlanid) {
		data3.bf.ilpb_default_vlanid = cfg->default_vlanid;
	}

    if (msk.s.sc_ind) {
		data3.bf.ilpb_sc_ind = cfg->sc_ind;
	}

    if (msk.s.unkwn_pol_idx) {
		data3.bf.ilpb_unkwn_pol_idx = cfg->unkwn_pol_idx;
	}

	if (msk.s.default_untag_cmd) {
		data2.bf.ilpb_default_untag_cmd = cfg->default_untag_cmd;
	}

	if (msk.s.drop_stag_packet) {
		data2.bf.ilpb_drop_stag_packet = cfg->drop_stag_packet > 0;
	}

	if (msk.s.drop_ctag_packet) {
		data2.bf.ilpb_drop_ctag_packet = cfg->drop_ctag_packet > 0;
	}

	if (msk.s.drop_untagged_packet) {
		data2.bf.ilpb_drop_untagged_packet = cfg->drop_untagged_packet > 0;
	}
	if(msk.s.drop_pritagged_packet)
	{
	    data2.bf.ilpb_drop_prytagged_packet = cfg->drop_pritagged_packet > 0;
	}

	if (msk.s.drop_doubletagged_packet) {
		data2.bf.ilpb_drop_doubletagged_packet = cfg->drop_doubletagged_packet > 0;
	}

    if (msk.s.drop_singletagged_packet) {
		data2.bf.ilpb_drop_singletagged_packet = cfg->drop_singletagged_packet > 0;
	}

	if (msk.s.igr_cls_lookup_en) {
		data1.bf.ilpb_igr_cls_lookup_en = cfg->igr_cls_lookup_en;
	}
	if (msk.s.cls_start) {
		data1.bf.ilpb_cls_start = cfg->cls_start;
	}

	if (msk.s.cls_length) {
		data1.bf.ilpb_cls_length = cfg->cls_length;
	}

    if (msk.s.stp_mode) {
		data1.bf.ilpb_stp_mode = cfg->stp_mode;
	}

	if (msk.s.station_move_en) {
		data1.bf.ilpb_station_move_en = cfg->station_move_en > 0;
	}

	if (msk.s.sa_move_dis_fwd) {
		data1.bf.ilpb_sa_move_dis_fwd = cfg->sa_move_dis_fwd > 0;
	}

    if (msk.s.loop_enable) {
		data1.bf.ilpb_loop_enable = cfg->loop_enable > 0;
	}

	if (msk.s.default_802_1p) {
		data0.bf.ilpb_default_802_1p = cfg->default_802_1p;
	}

	if (msk.s.default_dscp) {
		data0.bf.ilpb_default_dscp = cfg->default_dscp;
	}

	if (msk.s.default_cos) {
		data0.bf.ilpb_default_cos = cfg->default_cos;
	}

	if (msk.s.cos_mode_sel) {
		data0.bf.ilpb_cos_mode_sel = cfg->cos_mode_sel;
	}

	if (msk.s.cos_control_bm) {
		data0.bf.ilpb_cos_control_bm = cfg->cos_control_bm;
	}

	if (msk.s.dscp_mode_sel) {
		data0.bf.ilpb_dscp_mode_sel = cfg->dscp_mode_sel;
	}

	if (msk.s.dscp_mark_control_bm) {
		data0.bf.ilpb_dscp_mark_control_bm = cfg->dscp_mark_control_bm;
	}

	if (msk.s.top_802_1p_mode_sel) {
		data1.bf.ilpb_top_802_1p_mode_sel = cfg->top_802_1p_mode_sel;
	}

    if (msk.s.change_1p_if_pop) {
		data1.bf.ilpb_change_1p_if_pop = cfg->change_1p_if_pop;
	}

	if (msk.s.inner_802_1p_mode_sel) {
		data0.bf.ilpb_inner_802_1p_mode_sel =
		    cfg->inner_802_1p_mode_sel;
	}

	if (msk.s.dot1p_mark_control_bm) {
		data0.bf.ilpb_802_1p_mark_control_bm =
		    (cfg->dot1p_mark_control_bm & 3 );
        data1.bf.ilpb_802_1p_mark_control_bm =
		    ((cfg->dot1p_mark_control_bm >> 2) & 1 );
	}

    if(msk.s.top_s_tpid_enable){
        data2.bf.ilpb_top_s_tpid_enable    = (cfg->top_s_tpid_enable > 0) ;
    }

    if(msk.s.top_c_tpid_enable){
        data2.bf.ilpb_top_c_tpid_enable    = (cfg->top_c_tpid_enable > 0) ;
    }

    if(msk.s.inner_s_tpid_enable){
        data2.bf.ilpb_inner_s_tpid_enable    = (cfg->inner_s_tpid_enable > 0) ;
    }

    if(msk.s.inner_c_tpid_enable){
        data2.bf.ilpb_inner_c_tpid_enable    = (cfg->inner_c_tpid_enable > 0) ;
    }

    if(msk.s.c_tpid_match){
        data2.bf.ilpb_c_tpid_match    = (cfg->c_tpid_match) ;
    }

    if(msk.s.s_tpid_match){
        data2.bf.ilpb_s_tpid_match    = (cfg->s_tpid_match) ;
    }

    if(msk.s.other_tpid_match){
        data2.bf.ilpb_other_tpid_match    = (cfg->other_tpid_match) ;
    }

#if 0//yocto
    if(msk.s.ing_fltr_ena){
        data0.bf.ilpb_ing_fltr_ena  = (cfg->ing_fltr_ena>0);
    }
#else//sd1 uboot for 98f MP
        data0.bf.ilpb_ing_fltr_ena = 0;
#endif

    if(msk.s.vlan_cmd_sel_bm){
        data2.bf.ilpb_vlan_cmd_sel_bm = cfg->vlan_cmd_sel_bm;
    }
    if(msk.s.default_vlanid){
        data3.bf.ilpb_default_vlanid = cfg->default_vlanid;
    }

    if(msk.s.default_top_vlan_cmd){
         data2.bf.ilpb_default_top_vlan_cmd   = cfg->default_top_vlan_cmd;
    }

    if(msk.s.default_untag_cmd){
         data2.bf.ilpb_default_untag_cmd   = cfg->default_untag_cmd;
    }

    if(msk.s.inner_vlan_cmd){
         data2.bf.ilpb_inner_vlan_cmd   = cfg->inner_vlan_cmd;
    }

    if(msk.s.inner_vid_sel_bm){
         data2.bf.ilpb_inner_vid_sel_bm   = cfg->inner_vid_sel_bm;
    }

    if (msk.s.spcl_pkt_idx) {
        data1.bf.ilpb_spcl_pkt_idx = cfg->spcl_pkt_idx;
    }

    if(msk.s.flowid_sel_bm){
        data1.bf.ilpb_flowid_sel_bm = cfg->flowid_sel_bm;
    }

    if (dev == 0){
    	CA_L2FE_REG_WRITE(data4.wrd, L2FE_PP_ILPB_DATA4);
    	CA_L2FE_REG_WRITE(data3.wrd, L2FE_PP_ILPB_DATA3);
    	CA_L2FE_REG_WRITE(data2.wrd, L2FE_PP_ILPB_DATA2);
    	CA_L2FE_REG_WRITE(data1.wrd, L2FE_PP_ILPB_DATA1);
    	CA_L2FE_REG_WRITE(data0.wrd, L2FE_PP_ILPB_DATA0);
    	WRITE_INDIRCT_REG(dev, lpid, L2FE_PP_ILPB_ACCESS);
    }

	/* Set L2Lookup table */
	/*
	   LDPID 0x18   => L3_ILPB.LSPID = 0x19
	   LDPID 0x19   => L3_ILPB.LSPID = 0x18
	   LDPID 0x7    => L3_ILPB.LSPID = 0x19
	   LDPID others => L3_ILPB.LSPID = 0x18
	 */
	set_l2lkup = L2LKUP_CHECK(lpid);

	if (set_l2lkup == TRUE) {
		tbl_idx = L2LKUP_IDX(L2LKUP_LDPID(lpid));
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU

#if 0//yocto
		aal_l3fe_glb_ilpb_get(0, tbl_idx, &l2lkup_ilpb);
#else//sd1 uboot for 98f
#endif

#endif

		if (msk.s.wan_ind) {
			l2lkup_ilpb.wan_ind = cfg->wan_ind;
		}
		l2lkup_ilpb.lspid = lpid;
		l2lkup_ilpb.l2lrn_en = TRUE;
		if (msk.s.default_vlanid) {
			l2lkup_ilpb.dft_vlan_id = cfg->default_vlanid;
		}
		if (msk.s.sc_ind) {
			l2lkup_ilpb.sc_ind = cfg->sc_ind;
		}
		if (msk.s.vlan_cmd_sel_bm) {
			l2lkup_ilpb.top_vlan_cmd_sel_bm = cfg->vlan_cmd_sel_bm;
		}
		if (msk.s.default_top_vlan_cmd) {
			l2lkup_ilpb.dft_top_vlan_cmd = cfg->default_top_vlan_cmd;
		}
		if (msk.s.default_untag_cmd) {
			l2lkup_ilpb.dft_untag_cmd = cfg->default_untag_cmd;
		}
		if (msk.s.inner_vid_sel_bm) {
			l2lkup_ilpb.inner_vlan_cmd_sel_bm = cfg->inner_vid_sel_bm;
		}
		if (msk.s.inner_vlan_cmd) {
			l2lkup_ilpb.dft_inner_vlan_cmd = cfg->inner_vlan_cmd;
		}
		if (msk.s.default_noninnertag_cmd) {
			l2lkup_ilpb.dft_noinnertag_cmd = cfg->default_noninnertag_cmd;
		}
		if (msk.s.drop_stag_packet) {
			l2lkup_ilpb.drop_stag_pkt = cfg->drop_stag_packet;
		}
		if (msk.s.drop_ctag_packet) {
			l2lkup_ilpb.drop_ctag_pkt = cfg->drop_ctag_packet;
		}
		if (msk.s.drop_untagged_packet) {
			l2lkup_ilpb.drop_untag_pkt = cfg->drop_untagged_packet;
		}
		if (msk.s.drop_pritagged_packet) {
			l2lkup_ilpb.drop_pritag_pkt = cfg->drop_pritagged_packet;
		}
		if (msk.s.drop_doubletagged_packet) {
			l2lkup_ilpb.drop_dbltag_pkt = cfg->drop_doubletagged_packet;
		}
		if (msk.s.s_tpid_match) {
			l2lkup_ilpb.s_tpid_match = cfg->s_tpid_match;
		}
		if (msk.s.c_tpid_match) {
			l2lkup_ilpb.c_tpid_match = cfg->c_tpid_match;
		}
		if (msk.s.top_s_tpid_enable) {
			l2lkup_ilpb.top_s_tpid_en = cfg->top_s_tpid_enable;
		}
		if (msk.s.top_c_tpid_enable) {
			l2lkup_ilpb.top_c_tpid_en = cfg->top_c_tpid_enable;
		}
		if (msk.s.inner_c_tpid_enable) {
			l2lkup_ilpb.inner_c_tpid_en = cfg->inner_c_tpid_enable;
		}
		if (msk.s.ing_fltr_ena) {
			l2lkup_ilpb.ingr_known_vlan_only = cfg->ing_fltr_ena;
		}

#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU

#if 0//yocto
		aal_l3fe_glb_ilpb_set(0, tbl_idx, &l2lkup_ilpb);
		if (lpid == AAL_LPORT_L3_LAN)
			aal_l3fe_glb_ilpb_set(0, L2LKUP_IDX(AAL_LPORT_ETH_WAN), &l2lkup_ilpb);
		if (lpid == AAL_LPORT_L3_WAN)
			aal_l3fe_glb_ilpb_set(0, 3, &l2lkup_ilpb);
#else//sd1 uboot for 98f
#endif

#endif
	}

	return CA_E_OK;
}

ca_status_t aal_port_ilpb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_OUT aal_ilpb_cfg_t * cfg)
{
	L2FE_PP_ILPB_ACCESS_t oper;
	L2FE_PP_ILPB_DATA4_t data4;
	L2FE_PP_ILPB_DATA3_t data3;
	L2FE_PP_ILPB_DATA2_t data2;
	L2FE_PP_ILPB_DATA1_t data1;
	L2FE_PP_ILPB_DATA0_t data0;

	AAL_LPID_CHECK(lpid);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

	memset((void *)&oper, 0, sizeof(L2FE_PP_ILPB_ACCESS_t));
	memset((void *)&data4, 0, sizeof(L2FE_PP_ILPB_DATA4_t));
	memset((void *)&data3, 0, sizeof(L2FE_PP_ILPB_DATA3_t));
	memset((void *)&data2, 0, sizeof(L2FE_PP_ILPB_DATA2_t));
	memset((void *)&data1, 0, sizeof(L2FE_PP_ILPB_DATA1_t));
	memset((void *)&data0, 0, sizeof(L2FE_PP_ILPB_DATA0_t));

	memset(cfg, 0, sizeof(aal_ilpb_cfg_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid, L2FE_PP_ILPB_ACCESS);
    	data4.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA4);
    	data3.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA3);
    	data2.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA2);
    	data1.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA1);
    	data0.wrd = CA_L2FE_REG_READ(L2FE_PP_ILPB_DATA0);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid, L2FE_PP_ILPB_ACCESS);
    	data4.wrd = CA_8279_NE_REG_READ(L2FE_PP_ILPB_DATA4);
    	data3.wrd = CA_8279_NE_REG_READ(L2FE_PP_ILPB_DATA3);
    	data2.wrd = CA_8279_NE_REG_READ(L2FE_PP_ILPB_DATA2);
    	data1.wrd = CA_8279_NE_REG_READ(L2FE_PP_ILPB_DATA1);
    	data0.wrd = CA_8279_NE_REG_READ(L2FE_PP_ILPB_DATA0);
    }
#endif
	/* TODO. other mask cfg need to add */
	cfg->wan_ind = data4.bf.ilpb_wan_ind;
	cfg->default_untag_cmd = data2.bf.ilpb_default_untag_cmd;
	cfg->default_vlanid = data3.bf.ilpb_default_vlanid;
    cfg->sc_ind = data3.bf.ilpb_sc_ind;
    cfg->unkwn_pol_idx = data3.bf.ilpb_unkwn_pol_idx;

	cfg->drop_stag_packet = data2.bf.ilpb_drop_stag_packet;
	cfg->drop_ctag_packet = data2.bf.ilpb_drop_ctag_packet;
	cfg->drop_untagged_packet = data2.bf.ilpb_drop_untagged_packet;
	cfg->drop_pritagged_packet = data2.bf.ilpb_drop_prytagged_packet;
	cfg->drop_doubletagged_packet = data2.bf.ilpb_drop_doubletagged_packet;
    cfg->drop_singletagged_packet = data2.bf.ilpb_drop_singletagged_packet;
    cfg->top_s_tpid_enable      =  data2.bf.ilpb_top_s_tpid_enable ;
    cfg->top_c_tpid_enable      =  data2.bf.ilpb_top_c_tpid_enable ;
    cfg->inner_s_tpid_enable    =  data2.bf.ilpb_inner_s_tpid_enable ;
    cfg->inner_c_tpid_enable    =  data2.bf.ilpb_inner_c_tpid_enable ;
    cfg->s_tpid_match           =  data2.bf.ilpb_s_tpid_match ;
    cfg->c_tpid_match           =  data2.bf.ilpb_c_tpid_match ;
    cfg->other_tpid_match       =  data2.bf.ilpb_other_tpid_match;
	cfg->stp_mode           = data1.bf.ilpb_stp_mode;
	cfg->station_move_en    = data1.bf.ilpb_station_move_en;
	cfg->sa_move_dis_fwd    = data1.bf.ilpb_sa_move_dis_fwd;
    cfg->cls_start          = data1.bf.ilpb_cls_start;
    cfg->cls_length         = data1.bf.ilpb_cls_length;
    cfg->igr_cls_lookup_en  = data1.bf.ilpb_igr_cls_lookup_en;
    cfg->loop_enable        = data1.bf.ilpb_loop_enable;

	cfg->default_802_1p = data0.bf.ilpb_default_802_1p;
	cfg->default_vlanid = data3.bf.ilpb_default_vlanid;
	cfg->default_untag_cmd = data2.bf.ilpb_default_untag_cmd;
	cfg->default_cos = data0.bf.ilpb_default_cos;
	cfg->default_dscp = data0.bf.ilpb_default_dscp;
	cfg->cos_mode_sel = data0.bf.ilpb_cos_mode_sel;
	cfg->dscp_mode_sel = data0.bf.ilpb_dscp_mode_sel;
	cfg->top_802_1p_mode_sel = data1.bf.ilpb_top_802_1p_mode_sel;
    cfg->change_1p_if_pop = data1.bf.ilpb_change_1p_if_pop;
	cfg->inner_802_1p_mode_sel = data0.bf.ilpb_inner_802_1p_mode_sel;
    cfg->ing_fltr_ena  = data0.bf.ilpb_ing_fltr_ena;
    cfg->vlan_cmd_sel_bm = data2.bf.ilpb_vlan_cmd_sel_bm;
    cfg->default_vlanid = data3.bf.ilpb_default_vlanid;
    cfg->default_top_vlan_cmd   = data2.bf.ilpb_default_top_vlan_cmd;
    cfg->default_untag_cmd   = data2.bf.ilpb_default_untag_cmd;
    cfg->inner_vlan_cmd  = data2.bf.ilpb_inner_vlan_cmd ;
    cfg->inner_vid_sel_bm   = data2.bf.ilpb_inner_vid_sel_bm;
    cfg->spcl_pkt_idx = data1.bf.ilpb_spcl_pkt_idx;
    cfg->flowid_sel_bm = data1.bf.ilpb_flowid_sel_bm;
    cfg->cos_control_bm = data0.bf.ilpb_cos_control_bm;
    cfg->dot1p_mark_control_bm = (data1.bf.ilpb_802_1p_mark_control_bm << 2) | data0.bf.ilpb_802_1p_mark_control_bm;
    cfg->dscp_mark_control_bm = data0.bf.ilpb_dscp_mark_control_bm;

	return CA_E_OK;
}

ca_status_t aal_port_dpb_ctrl_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint8_t ldpid_pattern)
{
	L2FE_PP_DPB_CTRL_t cfg;

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&cfg, 0, sizeof(L2FE_PP_DPB_CTRL_t));
	cfg.bf.ldpid_pattern = ldpid_pattern;

    if (dev == 0){
    	CA_L2FE_REG_WRITE(cfg.wrd, L2FE_PP_DPB_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_NE_REG_WRITE(cfg.wrd, L2FE_PP_DPB_CTRL);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_dpb_ctrl_get(CA_IN ca_device_id_t dev,
				CA_OUT ca_uint8_t * ldpid_pattern)
{
	L2FE_PP_DPB_CTRL_t cfg;

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&cfg, 0, sizeof(L2FE_PP_DPB_CTRL_t));
    if (dev == 0){
    	cfg.wrd = CA_L2FE_REG_READ(L2FE_PP_DPB_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	cfg.wrd = CA_8279_NE_REG_READ(L2FE_PP_DPB_CTRL);
    }
#endif
	*ldpid_pattern = cfg.bf.ldpid_pattern;

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

/* destination port behavior configuration */
ca_status_t aal_port_dpb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid_cos,
				CA_IN aal_dpb_cfg_msk_t msk,
				CA_IN aal_dpb_cfg_t * cfg)
{
	L2FE_PP_DPB_ACCESS_t oper;
	L2FE_PP_DPB_DATA_t data;

	AAL_LPID_CHECK(lpid_cos);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&oper, 0, sizeof(L2FE_PP_DPB_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PP_DPB_DATA_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid_cos, L2FE_PP_DPB_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PP_DPB_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid_cos, L2FE_PP_DPB_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PP_DPB_DATA);
    }
#endif
	if (msk.s.bypass) {
		data.bf.bypass = cfg->bypass > 0;
	}

	if (msk.s.ldpid) {
		data.bf.ldpid = cfg->ldpid;
	}

	if (msk.s.cos) {
		data.bf.cos = cfg->cos;
	}

    if (dev == 0){
    	CA_L2FE_REG_WRITE(data.wrd, L2FE_PP_DPB_DATA);
    	WRITE_INDIRCT_REG(dev, lpid_cos, L2FE_PP_DPB_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_NE_REG_WRITE(data.wrd, L2FE_PP_DPB_DATA);
    	CA_8279_WRITE_INDIRCT_REG(dev, lpid_cos, L2FE_PP_DPB_ACCESS);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_dpb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid_cos,
				CA_OUT aal_dpb_cfg_t * cfg)
{
	L2FE_PP_DPB_ACCESS_t oper;
	L2FE_PP_DPB_DATA_t data;

	AAL_LPID_CHECK(lpid_cos);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

	memset((void *)&oper, 0, sizeof(L2FE_PP_DPB_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PP_DPB_DATA_t));

	memset(cfg, 0, sizeof(aal_dpb_cfg_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid_cos, L2FE_PP_DPB_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PP_DPB_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid_cos, L2FE_PP_DPB_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PP_DPB_DATA);
    }
#endif
	cfg->bypass = data.bf.bypass;
	cfg->ldpid = data.bf.ldpid;
	cfg->cos = data.bf.cos;

	return CA_E_OK;
}

/* egress logical port behavior configuration */
ca_status_t aal_port_elpb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_IN aal_elpb_cfg_msk_t msk,
				CA_IN aal_elpb_cfg_t * cfg)
{
	L2FE_PLC_ELPB_ACCESS_t oper;
	L2FE_PLC_ELPB_DATA_t data;
	l3fe_glb_elpb_t l2lkup_elpb;

	AAL_LPID_CHECK(lpid);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&oper, 0, sizeof(L2FE_PLC_ELPB_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PLC_ELPB_DATA_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid, L2FE_PLC_ELPB_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PLC_ELPB_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid, L2FE_PLC_ELPB_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PLC_ELPB_DATA);
    }
#endif
	if (msk.s.egr_cls_lookup_en) {
		data.bf.elpb_egr_cls_lookup_en = cfg->egr_cls_lookup_en > 0;
	}

	if (msk.s.egr_cls_start) {
		data.bf.elpb_egr_cls_start = cfg->egr_cls_start;
	}

	if (msk.s.egr_cls_length) {
		data.bf.elpb_egr_cls_length = cfg->egr_cls_length;
	}

	if (msk.s.egr_dest_wan) {
		data.bf.elpb_egr_dest_wan = cfg->egr_dest_wan;
	}

	if (msk.s.egr_ve_srch_en) {
		data.bf.elpb_egr_ve_srch_en = cfg->egr_ve_srch_en > 0;
	}

	if (msk.s.egr_port_stp_status) {
		data.bf.elpb_egr_port_stp_status = cfg->egr_port_stp_status > 0;
	}

	if (msk.s.egr_vlan_aware_mode) {
		data.bf.elpb_egr_vlan_aware_mode = cfg->egr_vlan_aware_mode > 0;
	}
    if(msk.s.egr_cls_flow_id_ena){
        data.bf.elpb_egr_cls_flowid_sel = (cfg->egr_cls_flow_id_ena > 0);
    }
    if(msk.s.egr_cls_cos_ena){
        data.bf.elpb_egr_cls_cos_sel = (cfg->egr_cls_cos_ena > 0);
    }

    if (dev == 0){
    	CA_L2FE_REG_WRITE(data.wrd, L2FE_PLC_ELPB_DATA);
    	WRITE_INDIRCT_REG(dev, lpid, L2FE_PLC_ELPB_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_NE_REG_WRITE(data.wrd, L2FE_PLC_ELPB_DATA);
    	CA_8279_WRITE_INDIRCT_REG(dev, lpid, L2FE_PLC_ELPB_ACCESS);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	/* Set L2Lookup ELPB */

	if (msk.s.egr_vlan_aware_mode) {
		memset((void *)&l2lkup_elpb, 0, sizeof(l3fe_glb_elpb_t));
		/* egr_vlan_aware_mode - 1: keep vlan, 0: pop all
		 * l2lkup_elpb - 1: pop all, 0: keep vlan
		 */
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU

#if 0//yocto
		aal_l3fe_glb_elpb_get(0, &l2lkup_elpb);
		if (cfg->egr_vlan_aware_mode == 0)
			l2lkup_elpb.ldpid_vec |= 1ll << lpid;
		else
			l2lkup_elpb.ldpid_vec &= ~(1ll << lpid);
		aal_l3fe_glb_elpb_set(0, &l2lkup_elpb);
#else//sd1 uboot for 98f
#endif

#endif
	}

	return CA_E_OK;
}

ca_status_t aal_port_elpb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_OUT aal_elpb_cfg_t * cfg)
{
	L2FE_PLC_ELPB_ACCESS_t oper;
	L2FE_PLC_ELPB_DATA_t data;

	AAL_LPID_CHECK(lpid);
	AAL_ASSERT_RET(cfg, CA_E_PARAM);

	memset((void *)&oper, 0, sizeof(L2FE_PLC_ELPB_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PLC_ELPB_DATA_t));

	memset(cfg, 0, sizeof(aal_elpb_cfg_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid, L2FE_PLC_ELPB_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PLC_ELPB_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid, L2FE_PLC_ELPB_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PLC_ELPB_DATA);
    }
#endif
	cfg->egr_cls_lookup_en = data.bf.elpb_egr_cls_lookup_en;
	cfg->egr_cls_start = data.bf.elpb_egr_cls_start;
	cfg->egr_cls_length = data.bf.elpb_egr_cls_length;
	cfg->egr_dest_wan = data.bf.elpb_egr_dest_wan;
	cfg->egr_ve_srch_en = data.bf.elpb_egr_ve_srch_en;
	cfg->egr_port_stp_status = data.bf.elpb_egr_port_stp_status;
	cfg->egr_vlan_aware_mode = data.bf.elpb_egr_vlan_aware_mode;
    cfg->egr_cls_cos_ena             = data.bf.elpb_egr_cls_cos_sel;
    cfg->egr_cls_flow_id_ena         = data.bf.elpb_egr_cls_flowid_sel;

	return CA_E_OK;
}

ca_status_t aal_port_mmshp_check_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_IN ca_uint64_t bmp)
{
	L2FE_PLE_PORT_MMSHP_CHK_ACCESS_t oper;
	L2FE_PLE_PORT_MMSHP_CHK_DATA1_t data1;
	L2FE_PLE_PORT_MMSHP_CHK_DATA0_t data0;

	AAL_LPID_CHECK(lpid);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

    //ca_printf("mmshp_check port %d, bmp 0x%x\r\n", lpid, bmp);
	memset((void *)&oper, 0, sizeof(L2FE_PLE_PORT_MMSHP_CHK_ACCESS_t));
	memset((void *)&data1, 0, sizeof(L2FE_PLE_PORT_MMSHP_CHK_DATA1_t));
	memset((void *)&data0, 0, sizeof(L2FE_PLE_PORT_MMSHP_CHK_DATA0_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
    	data1.wrd = CA_L2FE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA1);
    	data0.wrd = CA_L2FE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA0);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
    	data1.wrd = CA_8279_NE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA1);
    	data0.wrd = CA_8279_NE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA0);
    }
#endif
	data1.bf.ldpid_bm = (bmp >> 32) & 0x00000000ffffffff;
	data0.bf.ldpid_bm = bmp & 0x00000000ffffffff;

    if (dev == 0){
    	CA_L2FE_REG_WRITE(data1.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA1);
    	CA_L2FE_REG_WRITE(data0.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA0);
    	WRITE_INDIRCT_REG(dev, lpid, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_NE_REG_WRITE(data1.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA1);
    	CA_8279_NE_REG_WRITE(data0.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA0);
    	CA_8279_WRITE_INDIRCT_REG(dev, lpid, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_mmshp_check_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_OUT ca_uint64_t * bmp)
{
	L2FE_PLE_PORT_MMSHP_CHK_ACCESS_t oper;
	L2FE_PLE_PORT_MMSHP_CHK_DATA1_t data1;
	L2FE_PLE_PORT_MMSHP_CHK_DATA0_t data0;

	AAL_LPID_CHECK(lpid);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&oper, 0, sizeof(L2FE_PLE_PORT_MMSHP_CHK_ACCESS_t));
	memset((void *)&data1, 0, sizeof(L2FE_PLE_PORT_MMSHP_CHK_DATA1_t));
	memset((void *)&data0, 0, sizeof(L2FE_PLE_PORT_MMSHP_CHK_DATA0_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, lpid, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
    	data1.wrd = CA_L2FE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA1);
    	data0.wrd = CA_L2FE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA0);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, lpid, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
    	data1.wrd = CA_8279_NE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA1);
    	data0.wrd = CA_8279_NE_REG_READ(L2FE_PLE_PORT_MMSHP_CHK_DATA0);
    }
#endif
	*bmp = (((ca_uint64_t) (data1.bf.ldpid_bm) << 32) & 0xffffffff00000000) | (((ca_uint64_t) (data0.bf.ldpid_bm) & 0x00000000ffffffff));

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_trunk_group_hash_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_IN aal_trunk_hash_key_t key)
{
	L2FE_PLE_TRUNK_GRP_CTRL_0_t cfg;

	AAL_TRUNK_CHECK(group_id);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&cfg, 0, sizeof(L2FE_PLE_TRUNK_GRP_CTRL_0_t));
	if (key.field_sel_cos == 1) {
		cfg.bf.trunk_hash_field_sel_cos = 1;
	}

	if (key.field_sel_l4_dp == 1) {
		cfg.bf.trunk_hash_field_sel_l4_dp = 1;
	}

	if (key.field_sel_l4_sp == 1) {
		cfg.bf.trunk_hash_field_sel_l4_sp = 1;
	}

	if (key.field_sel_ip_da == 1) {
		cfg.bf.trunk_hash_field_sel_ip_da = 1;
	}

	if (key.field_sel_ip_sa == 1) {
		cfg.bf.trunk_hash_field_sel_ip_sa = 1;
	}

	if (key.field_sel_tx_top_vid == 1) {
		cfg.bf.trunk_hash_field_sel_tx_top_vid = 1;
	}

	if (key.field_sel_mac_da == 1) {
		cfg.bf.trunk_hash_field_sel_mac_da = 1;
	}

	if (key.field_sel_mac_sa == 1) {
		cfg.bf.trunk_hash_field_sel_mac_sa = 1;
	}
    if (dev == 0){
    	if (group_id == 1) {
        	CA_L2FE_REG_WRITE(cfg.wrd, L2FE_PLE_TRUNK_GRP_CTRL_1);
    	} else {
        	CA_L2FE_REG_WRITE(cfg.wrd, L2FE_PLE_TRUNK_GRP_CTRL_0);
    	}
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	if (group_id == 1) {
        	CA_8279_NE_REG_WRITE(cfg.wrd, L2FE_PLE_TRUNK_GRP_CTRL_1);
    	} else {
        	CA_8279_NE_REG_WRITE(cfg.wrd, L2FE_PLE_TRUNK_GRP_CTRL_0);
    	}
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_trunk_group_hash_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_OUT aal_trunk_hash_key_t * key)
{
	L2FE_PLE_TRUNK_GRP_CTRL_0_t cfg;

	AAL_TRUNK_CHECK(group_id);
	AAL_ASSERT_RET(key, CA_E_PARAM);

    if (dev == 0){
    	if (group_id == 1) {
        	cfg.wrd = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_GRP_CTRL_1);
    	} else {
        	cfg.wrd = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_GRP_CTRL_0);
    	}
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	if (group_id == 1) {
        	cfg.wrd = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_GRP_CTRL_1);
    	} else {
        	cfg.wrd = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_GRP_CTRL_0);
    	}
    }
#endif
	memset(key, 0, sizeof(aal_trunk_hash_key_t));

	if (cfg.bf.trunk_hash_field_sel_cos == 1) {
		key->field_sel_cos = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_l4_dp == 1) {
		key->field_sel_l4_dp = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_l4_sp == 1) {
		key->field_sel_l4_sp = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_ip_da == 1) {
		key->field_sel_ip_da = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_ip_sa == 1) {
		key->field_sel_ip_sa = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_tx_top_vid == 1) {
		key->field_sel_tx_top_vid = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_mac_da == 1) {
		key->field_sel_mac_da = 1;
	}

	if (cfg.bf.trunk_hash_field_sel_mac_sa == 1) {
		key->field_sel_mac_sa = 1;
	}

	return CA_E_OK;
}

ca_status_t aal_port_trunk_group_member_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_IN ca_uint64_t bmp)
{
	ca_uint32_t addr0 = 0, addr1 = 0;
	L2FE_PLE_TRUNK_GRP_0_MEMBER_0_t cfg0;
	L2FE_PLE_TRUNK_GRP_0_MEMBER_1_t cfg1;

	AAL_TRUNK_CHECK(group_id);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&cfg0, 0, sizeof(L2FE_PLE_TRUNK_GRP_0_MEMBER_0_t));
	memset((void *)&cfg1, 0, sizeof(L2FE_PLE_TRUNK_GRP_0_MEMBER_1_t));

	cfg0.wrd = (ca_uint32_t) (bmp & 0xFFFFFFFF);
	cfg1.wrd = (ca_uint32_t) (bmp >> 32 & 0xFFFFFFFF);

    if (dev == 0){
    	if (group_id == 1) {
        	CA_L2FE_REG_WRITE(cfg0.wrd, L2FE_PLE_TRUNK_GRP_1_MEMBER_0);
        	CA_L2FE_REG_WRITE(cfg1.wrd, L2FE_PLE_TRUNK_GRP_1_MEMBER_1);

    	} else {
        	CA_L2FE_REG_WRITE(cfg0.wrd, L2FE_PLE_TRUNK_GRP_0_MEMBER_0);
        	CA_L2FE_REG_WRITE(cfg1.wrd, L2FE_PLE_TRUNK_GRP_0_MEMBER_1);
    	}
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	if (group_id == 1) {
        	CA_8279_NE_REG_WRITE(cfg0.wrd, L2FE_PLE_TRUNK_GRP_1_MEMBER_0);
        	CA_8279_NE_REG_WRITE(cfg1.wrd, L2FE_PLE_TRUNK_GRP_1_MEMBER_1);

    	} else {
        	CA_8279_NE_REG_WRITE(cfg0.wrd, L2FE_PLE_TRUNK_GRP_0_MEMBER_0);
        	CA_8279_NE_REG_WRITE(cfg1.wrd, L2FE_PLE_TRUNK_GRP_0_MEMBER_1);
    	}
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_trunk_group_member_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_OUT ca_uint64_t * bmp)
{
	ca_uint32_t cfg0, cfg1;

	AAL_TRUNK_CHECK(group_id);
	AAL_ASSERT_RET(bmp, CA_E_PARAM);

    if (dev == 0){
    	if (group_id == 1) {
    		cfg0 = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_GRP_1_MEMBER_0);
    		cfg1 = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_GRP_1_MEMBER_1);
    	} else {
    		cfg0 = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_GRP_0_MEMBER_0);
    		cfg1 = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_GRP_0_MEMBER_1);
    	}
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	if (group_id == 1) {
    		cfg0 = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_GRP_1_MEMBER_0);
    		cfg1 = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_GRP_1_MEMBER_1);
    	} else {
    		cfg0 = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_GRP_0_MEMBER_0);
    		cfg1 = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_GRP_0_MEMBER_1);
    	}
    }
#endif
	*bmp = (ca_uint64_t) cfg1 << 32;
	*bmp |= (ca_uint64_t) cfg0;

	return CA_E_OK;
}

ca_status_t aal_port_trunk_dest_map_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t trunk_idx,
				CA_IN ca_uint32_t ldpid)
{
	L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS_t oper;
	L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA_t data;

	AAL_LPID_CHECK(ldpid);
	if (trunk_idx > 32 * AAL_PORT_NUM_TRUNK) {
		return CA_E_PARAM;
	}

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

	memset((void *)&oper, 0, sizeof(L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA_t));

    if (dev == 0){
    	READ_INDIRCT_REG(dev, trunk_idx, L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA);

    	data.bf.ldpid = ldpid;
    	CA_L2FE_REG_WRITE(data.wrd, L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA);
    	WRITE_INDIRCT_REG(dev, trunk_idx, L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, trunk_idx, L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA);

    	data.bf.ldpid = ldpid;
    	CA_8279_NE_REG_WRITE(data.wrd, L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA);
    	CA_8279_WRITE_INDIRCT_REG(dev, trunk_idx, L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

	return CA_E_OK;
}

ca_status_t aal_port_trunk_dest_map_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t trunk_idx,
				CA_OUT ca_uint32_t * ldpid)
{
	L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS_t oper;
	L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA_t data;

	AAL_ASSERT_RET(ldpid, CA_E_PARAM);

	if (trunk_idx > 32 * AAL_PORT_NUM_TRUNK) {
		return CA_E_PARAM;
	}

	memset((void *)&oper, 0, sizeof(L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS_t));
	memset((void *)&data, 0, sizeof(L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA_t));
	*ldpid = 0;

    if (dev == 0){
    	READ_INDIRCT_REG(dev, trunk_idx, L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS);
    	data.wrd = CA_L2FE_REG_READ(L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
    	CA_8279_READ_INDIRCT_REG(dev, trunk_idx, L2FE_PLE_TRUNK_DEST_MAP_TBL_ACCESS);
    	data.wrd = CA_8279_NE_REG_READ(L2FE_PLE_TRUNK_DEST_MAP_TBL_DATA);
    }
#endif
    *ldpid = data.bf.ldpid;

	return CA_E_OK;
}

ca_status_t aal_port_arb_ldpid_pdpid_map_set(
    CA_IN ca_device_id_t dev,
    CA_IN ca_uint32_t  my_mac_flag,
    CA_IN ca_uint32_t  dbuf_flag,
    CA_IN ca_uint32_t ldpid,
    CA_IN ca_uint32_t pdpid
)
{
    ca_uint32_t temp_ind = 0;
    AAL_LPID_CHECK(ldpid);
    if(pdpid >15){
        return CA_E_PARAM;
    }

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif
    if (dev == 0){
        CA_L2FE_REG_WRITE(pdpid, L2FE_ARB_PDPID_MAP_TBL_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
        CA_8279_NE_REG_WRITE(pdpid, L2FE_ARB_PDPID_MAP_TBL_DATA);
    }
#endif

    temp_ind = (((my_mac_flag > 0) << 7) | ((dbuf_flag > 0) << 6) |ldpid);

    if (dev == 0){
        WRITE_INDIRCT_REG(dev, temp_ind, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
        CA_8279_WRITE_INDIRCT_REG(dev, temp_ind, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

    return CA_E_OK;
}

ca_status_t aal_port_arb_ldpid_pdpid_map_get(
    CA_IN ca_device_id_t dev,
    CA_IN ca_uint32_t  my_mac_flag,
    CA_IN ca_uint32_t  dbuf_flag,
    CA_IN ca_uint32_t ldpid,
    CA_OUT ca_uint32_t *pdpid
)
{
    ca_uint32_t temp_ind = 0;

    AAL_LPID_CHECK(ldpid);

	AAL_ASSERT_RET(pdpid, CA_E_PARAM);

#if 0//yocto
	__AAL_PORT_LOCK(dev);
#else//sd1 uboot for 98f
#endif

    temp_ind = (((my_mac_flag > 0) << 7) | ((dbuf_flag > 0) << 6) |ldpid);
    if (dev == 0){
        READ_INDIRCT_REG(dev, temp_ind, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
        *pdpid = CA_L2FE_REG_READ(L2FE_ARB_PDPID_MAP_TBL_DATA);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
        CA_8279_READ_INDIRCT_REG(dev, temp_ind, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
        *pdpid = CA_8279_NE_REG_READ(L2FE_ARB_PDPID_MAP_TBL_DATA);
    }
#endif

#if 0//yocto
	__AAL_PORT_UNLOCK(dev);
#else//sd1 uboot for 98f
#endif

    return CA_E_OK;

}

void __aal_port_change_1p_if_pop_en(ca_device_id_t dev, ca_uint8_t enable)
{
    L2FE_GLB_CTRL_t cfg;

    if (dev == 0){
        cfg.wrd = CA_L2FE_REG_READ(L2FE_GLB_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
        cfg.wrd = CA_8279_NE_REG_READ(L2FE_GLB_CTRL);
    }
#endif

    cfg.bf.change_ip_if_pop_en = enable > 0;

    if (dev == 0){
        CA_L2FE_REG_WRITE(cfg.wrd, L2FE_GLB_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (dev == 1){
        CA_8279_NE_REG_WRITE(cfg.wrd, L2FE_GLB_CTRL);
    }
#endif

}

void __aal_port_pipeline_clk_num_set(ca_uint32_t clk_num)
{
#ifdef CONFIG_ARCH_CORTINA_G3LITE
    L2FE_GLB_CTRL_t fe_glb_ctrl;
    NI_HV_GLB_RXMUX_CTRL_CFG_t ni_glb_nimux_ctrl;

    fe_glb_ctrl.wrd = CA_L2FE_REG_READ(L2FE_GLB_CTRL);
    fe_glb_ctrl.bf.pipeline_clk_num = clk_num;
    CA_L2FE_REG_WRITE(fe_glb_ctrl.wrd, L2FE_GLB_CTRL);
    ni_glb_nimux_ctrl.wrd = CA_NE_REG_READ(NI_HV_GLB_RXMUX_CTRL_CFG);
    ni_glb_nimux_ctrl.bf.next_sop_delay = clk_num;
    CA_NE_REG_WRITE(ni_glb_nimux_ctrl.wrd, NI_HV_GLB_RXMUX_CTRL_CFG);
#endif
}

ca_status_t aal_port_init(CA_IN ca_device_id_t dev)
{
    ca_uint32_t i = 0;
    ca_uint32_t lpid = 0;
    aal_ippb_cfg_msk_t ippb_msk;
    aal_ippb_cfg_t ippb_cfg;
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t igr_cfg;
    aal_elpb_cfg_msk_t egr_msk;
    aal_elpb_cfg_t egr_cfg;
    ca_uint64_t bmp = 0;
    ca_uint32_t my_mac = 0;
    /*ca_uint32_t flooding_domain_1[CFG_ID_FLOODING_DOMAIN_LEN];
	ca_uint32_t flooding_domain_2[CFG_ID_FLOODING_DOMAIN_LEN];
	ca_uint32_t mcgid_0, mcgid_1;
	aal_ple_dft_fwd_ctrl_mask_t fwd_msk;
	aal_ple_dft_fwd_ctrl_t fwd_cfg;*/
    ca_uint8_t wan_port_type = 0;

#if 0//yocto
    ca_spin_lock_init(&__aal_port_lock, SPINLOCK_BH);
#else//sd1 uboot for 98f
#endif
   // memset(g_aal_l2vlan_outer_is_s, 0, CFG_ID_L2FE_VLAN_IS_S_LEN);
    //memset(g_aal_l2vlan_inner_is_s, 0, CFG_ID_L2FE_VLAN_IS_S_LEN);

 	/* init IPPB table, ppid to lpid */
    memset(&ippb_msk, 0, sizeof(aal_ippb_cfg_msk_t));
    memset(&ippb_cfg, 0, sizeof(aal_ippb_cfg_t));
    ippb_msk.s.lspid = 1;
    for (i = 0; i < AAL_PORT_NUM_LOGICAL; i++) {
        ippb_cfg.lspid = i;
        aal_port_ippb_cfg_set(dev, i, ippb_msk, &ippb_cfg);
    }

	/* STP mode configuration: forward and learning
	   Station move enable
	   Port isolation disable  */
    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));
    igr_cfg.stp_mode = AAL_STP_STATE_FWD_AND_LEARN;
    igr_msk.s.station_move_en = 1;
    igr_cfg.station_move_en = 1;
    for(i = 0; i < AAL_PORT_NUM_LOGICAL; i++){
        igr_msk.s.stp_mode = 1;
        aal_port_ilpb_cfg_set(dev, i, igr_msk, &igr_cfg);
        bmp = ~(1<<i);  /* disable port isolation */
        aal_port_mmshp_check_set(dev, i, bmp);
    }

    /* set AAL_LPORT_L3_WAN/AAL_LPORT_L3_LAN to AAL_STP_STATE_FWD_NO_LEARN */
    igr_cfg.stp_mode = AAL_STP_STATE_FWD_NO_LEARN;
    igr_msk.s.stp_mode = 1;
    aal_port_ilpb_cfg_set(dev, AAL_LPORT_L3_WAN, igr_msk, &igr_cfg);
    aal_port_ilpb_cfg_set(dev, AAL_LPORT_L3_LAN, igr_msk, &igr_cfg);

#if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    aal_port_ilpb_cfg_set(dev, AAL_LPORT_ETH_WAN, igr_msk, &igr_cfg);
#endif

    memset(&egr_msk, 0, sizeof(aal_elpb_cfg_msk_t));
    memset(&egr_cfg, 0, sizeof(aal_elpb_cfg_t));
    egr_msk.s.egr_port_stp_status = 1;
    egr_cfg.egr_port_stp_status = 1;
    egr_msk.s.egr_vlan_aware_mode = 1;
    egr_cfg.egr_vlan_aware_mode = TRUE;
    egr_msk.s.egr_ve_srch_en = 1;
    egr_cfg.egr_ve_srch_en = TRUE;
    egr_msk.s.egr_dest_wan = 1;
    egr_msk.s.egr_cls_cos_ena = 1;
    egr_msk.s.egr_cls_flow_id_ena = 1;
    egr_cfg.egr_cls_cos_ena = TRUE;
    egr_cfg.egr_cls_flow_id_ena = TRUE;

#if 0//yocto
    aal_scfg_read(CFG_ID_WAN_PORT_TYPE_IS_L3,1,&wan_port_type);

    if (CA_E_OK != scfg_read(0, CFG_ID_WAN_PORT_ID, 1,&wan_port_id)) {
        printf("SCFG read %s failed!!!\n", CFG_ID_WAN_PORT_ID);
    }
#else//sd1 uboot for 98f
    wan_port_type = 1;

    wan_port_id = 0x03;
#endif

    for(i = 0; i < AAL_PORT_NUM_LOGICAL; i++){

        if(AAL_LPORT_L3_WAN == i){
            if(wan_port_type == 1){
                egr_cfg.egr_ve_srch_en = FALSE;
            }
        }else{
             egr_cfg.egr_ve_srch_en = TRUE;
        }
        if(i == wan_port_id || (i >= AAL_PORT_LLID_TYPE_START)){
            egr_cfg.egr_dest_wan = TRUE;
        }else{
            egr_cfg.egr_dest_wan = FALSE;
        }

        aal_port_elpb_cfg_set(dev, i, egr_msk, &egr_cfg);
    }

    do
    {
        /* egress destination WAN of L3_LAN port should be 1, other ports uses default value 0 */
        aal_elpb_cfg_msk_t elpb_msk;
        aal_elpb_cfg_t elpb_cfg;

        memset(&elpb_msk, 0, sizeof(elpb_msk));
        memset(&elpb_cfg, 0, sizeof(elpb_cfg));

        elpb_msk.s.egr_dest_wan = 1;
        elpb_cfg.egr_dest_wan   = 1;

        (void)aal_port_elpb_cfg_set(dev, AAL_LPORT_L3_LAN, elpb_msk, &elpb_cfg);
    } while(0);

#if 0//yocto
    if (CA_E_OK != scfg_read(0, CFG_ID_PORT_L2VLAN_OUTER_IS_S,
        CFG_ID_L2FE_VLAN_IS_S_LEN,g_aal_l2vlan_outer_is_s)) {
        printf("SCFG read %s failed!!!\n", CFG_ID_PORT_L2VLAN_OUTER_IS_S);
    }

    if (CA_E_OK != scfg_read(0, CFG_ID_PORT_L2VLAN_INNER_IS_S,
        CFG_ID_L2FE_VLAN_IS_S_LEN,g_aal_l2vlan_inner_is_s)) {
        printf("SCFG read %s failed!!!\n", CFG_ID_PORT_L2VLAN_INNER_IS_S);
    }
#else//sd1 uboot for 98f
    int j;
    for(j=0; j<CFG_ID_L2FE_VLAN_IS_S_LEN; j++){
        g_aal_l2vlan_outer_is_s[j] = 1;
        g_aal_l2vlan_inner_is_s[j] = 1;
    }
#endif

    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

    igr_msk.s.top_s_tpid_enable = 1;
    igr_msk.s.inner_s_tpid_enable = 1;
    igr_msk.s.top_c_tpid_enable = 1;
    igr_msk.s.inner_c_tpid_enable = 1;
    igr_msk.s.c_tpid_match = 1;
    igr_msk.s.s_tpid_match = 1;
    igr_msk.s.other_tpid_match = 1;
    igr_msk.s.vlan_cmd_sel_bm = 1;
    igr_msk.s.dot1p_mark_control_bm = 1;
    igr_msk.s.top_802_1p_mode_sel = 1;
    igr_msk.s.inner_802_1p_mode_sel = 1;
    igr_msk.s.change_1p_if_pop = 1;
    igr_msk.s.sa_move_dis_fwd     = 1;
    igr_msk.s.flowid_sel_bm       = 1;
    igr_msk.s.dscp_mode_sel = 1;
    igr_msk.s.inner_vid_sel_bm = 1;
    igr_msk.s.ing_fltr_ena = 1;

    /* set 0x8100 as default comparing tpid */
    igr_cfg.c_tpid_match = (1 << AAL_VLAN_TPID_INDEX_8100);
    igr_cfg.s_tpid_match = (1 << AAL_VLAN_TPID_INDEX_8100);
    igr_cfg.other_tpid_match = (1 << AAL_VLAN_TPID_INDEX_8100);

    igr_cfg.vlan_cmd_sel_bm = 0xf;
    igr_cfg.dot1p_mark_control_bm = 0x7;
    igr_cfg.top_802_1p_mode_sel = 0;
    igr_cfg.inner_802_1p_mode_sel = 2;
    igr_cfg.change_1p_if_pop = 1;
    igr_cfg.sa_move_dis_fwd     = FALSE;
    igr_cfg.flowid_sel_bm       = 7;
    igr_cfg.dscp_mode_sel       = 2; /* from Rx DSCP Map */
    igr_cfg.inner_vid_sel_bm = 0xf;


    for(i = 0; i < AAL_PORT_NUM_LOGICAL; i++){
        if(i == AAL_LPORT_MC){ // for UUC packets
            igr_cfg.top_s_tpid_enable = 1;
            igr_cfg.top_c_tpid_enable = 1;
            igr_cfg.inner_s_tpid_enable = 1;
            igr_cfg.inner_c_tpid_enable = 1;

            igr_cfg.c_tpid_match = 0xf;
            igr_cfg.s_tpid_match = 0xf;
            igr_cfg.other_tpid_match = 0xf;
            igr_cfg.ing_fltr_ena = TRUE;
        }else if(i<32){
            igr_cfg.top_s_tpid_enable = g_aal_l2vlan_outer_is_s[i];
            igr_cfg.top_c_tpid_enable = (g_aal_l2vlan_outer_is_s[i] == 0);
            igr_cfg.inner_s_tpid_enable = g_aal_l2vlan_inner_is_s[i];
            igr_cfg.inner_c_tpid_enable = (g_aal_l2vlan_inner_is_s[i] == 0);
            igr_cfg.ing_fltr_ena = TRUE;

        }else{
            igr_cfg.top_s_tpid_enable = 1;
            igr_cfg.top_c_tpid_enable = 0;
            igr_cfg.inner_s_tpid_enable = 1;
            igr_cfg.inner_c_tpid_enable = 0;
            igr_cfg.ing_fltr_ena = TRUE;
  #if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
            igr_msk.s.wan_ind = 1;
            igr_cfg.wan_ind = TRUE;
  #endif
        }

        aal_port_ilpb_cfg_set(dev, i, igr_msk, &igr_cfg);
    }

    do
    {   /* fixed bug 48568 */
        /* source WAN indication of L3_WAN port should be 1, other ports uses default value 0 */
        aal_ilpb_cfg_msk_t ilpb_msk;
        aal_ilpb_cfg_t ilpb_cfg;

        memset(&ilpb_msk, 0, sizeof(ilpb_msk));
        memset(&ilpb_cfg, 0, sizeof(ilpb_cfg));

        ilpb_msk.s.wan_ind = 1;
        ilpb_cfg.wan_ind   = 1;

        (void)aal_port_ilpb_cfg_set(dev, AAL_LPORT_L3_WAN, ilpb_msk, &ilpb_cfg);
    } while(0);

    __aal_port_change_1p_if_pop_en(dev, 1);

    /* the classification rules allocation for 4 UNI */
    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&egr_msk, 0, sizeof(aal_elpb_cfg_msk_t));
    memset(&egr_cfg, 0, sizeof(aal_elpb_cfg_t));

    igr_msk.s.igr_cls_lookup_en = 1;
    igr_msk.s.cls_start = 1;
    igr_msk.s.cls_length = 1;

    igr_cfg.igr_cls_lookup_en = 1;
    igr_cfg.cls_length = AAL_ILPB_CLS_LENGTH;

    egr_msk.s.egr_cls_lookup_en = 1;
    egr_msk.s.egr_cls_start = 1;
    egr_msk.s.egr_cls_length = 1;
    egr_msk.s.egr_cls_flow_id_ena = 1;
    egr_msk.s.egr_cls_cos_ena = 1;

    egr_cfg.egr_cls_lookup_en = 1;
    egr_cfg.egr_cls_length = AAL_ELPB_CLS_LENGTH;
    egr_cfg.egr_cls_flow_id_ena = TRUE;
    egr_cfg.egr_cls_cos_ena = TRUE;

    for (lpid = AAL_PORT_START_FOR_CLS; lpid < AAL_PORT_START_FOR_CLS + AAL_PORT_NUM_FOR_CLS; lpid++) {
        igr_cfg.cls_start =
            AAL_ILPB_CLS_START + (lpid-AAL_PORT_START_FOR_CLS) * AAL_ILPB_CLS_LENGTH;
        aal_port_ilpb_cfg_set(dev, lpid, igr_msk, &igr_cfg);
        if(lpid == AAL_LPORT_ETH_WAN){
            aal_port_ilpb_cfg_set(dev, AAL_LPORT_L3_WAN, igr_msk, &igr_cfg);
        }
        egr_cfg.egr_cls_start =
            AAL_ELPB_CLS_START + (lpid -AAL_PORT_START_FOR_CLS)  * AAL_ELPB_CLS_LENGTH;
        aal_port_elpb_cfg_set(dev, lpid, egr_msk, &egr_cfg);
    }

    for(lpid = 0x10; lpid <= 0x17; lpid++){
        igr_cfg.cls_start = AAL_PORT_START_FOR_CLS_CPU + (lpid-0x10)*AAL_ILPB_CLS_LENGTH_CPU;
        igr_cfg.cls_length = AAL_ILPB_CLS_LENGTH_CPU;
        aal_port_ilpb_cfg_set(dev, lpid, igr_msk, &igr_cfg);
    }


    /* ARB LDPID mapping */
    for(my_mac = 0; my_mac <= 1; my_mac++)
    {
        for (lpid = AAL_LPORT_ETH_NI0; lpid <= AAL_LPORT_ETH_NI7; lpid++) {
            aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, lpid, lpid);
        }
        for (lpid = AAL_LPORT_DEEPQ_0; lpid <= AAL_LPORT_DEEPQ_7; lpid++) {
            aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 1, lpid, AAL_PPORT_QM);
        }
        for (lpid = AAL_LPORT_9QUEUE_NI0; lpid <= AAL_LPORT_9QUEUE_NI7; lpid++) {
            aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, lpid, AAL_PPORT_OAM);
        }
        for (lpid = AAL_LPORT_CPU_0; lpid <= AAL_LPORT_CPU_7; lpid++) {
            aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, lpid, AAL_PPORT_CPU);
        }
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_L3_WAN, AAL_PPORT_L3_WAN);
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_L3_LAN, AAL_PPORT_L3_LAN);
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_OAM, AAL_PPORT_OAM);
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_MC, AAL_PPORT_MC);
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_CPU_Q, AAL_PPORT_CPU);
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_SPCL, AAL_PPORT_BLACKHOLE);
        aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, AAL_LPORT_BLACKHOLE, AAL_PPORT_BLACKHOLE);
#if 0//defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
        for (lpid = AAL_LPORT_CPU_MQ_0; lpid <= AAL_LPORT_CPU_MQ_31; lpid++) {
            aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, lpid, AAL_PPORT_NI7);
        }
#else
        for (lpid = AAL_LPORT_CPU_MQ_0; lpid <= AAL_LPORT_CPU_MQ_31; lpid++) {
            aal_port_arb_ldpid_pdpid_map_set(dev, my_mac, 0, lpid, AAL_PPORT_CPU);
        }
#endif
    }


#ifdef CONFIG_ARCH_CORTINA_G3LITE
     __aal_port_pipeline_clk_num_set(0xC);
#endif

	return CA_E_OK;
}

#if 0//yocto
#else//sd1 uboot for 98f mp test
ca_status_t ca_ilpb_loopback_set(
    CA_IN ca_port_id_t port_id, 
    CA_IN ca_boolean_t en_loopback
)
{
#if 0
printf("%s\tport_id: %#x\ten_loopback: %#x\n", __func__, \
    port_id, en_loopback);
#endif

    ca_device_id_t dev = 0;
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t igr_cfg;

    ca_status_t ret = CA_E_OK;

    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

    /* enable loopback for specific port (lspid==ldpid will not be dropped by l2fe) */
    igr_cfg.loop_enable = en_loopback;
    igr_msk.s.loop_enable = 1;
    ret = aal_port_ilpb_cfg_set(dev, port_id, igr_msk, &igr_cfg);

    return ret;
}

#endif//sd1 uboot for 98f mp test

EXPORT_SYMBOL(aal_port_mmshp_check_set);
EXPORT_SYMBOL(aal_port_ilpb_cfg_set);
EXPORT_SYMBOL(aal_port_trunk_group_hash_get);
EXPORT_SYMBOL(aal_port_mmshp_check_get);
EXPORT_SYMBOL(aal_port_ilpb_cfg_get);
EXPORT_SYMBOL(aal_port_trunk_group_member_set);
EXPORT_SYMBOL(aal_port_elpb_cfg_get);
EXPORT_SYMBOL(aal_port_trunk_group_member_get);
EXPORT_SYMBOL(aal_port_ippb_cfg_set);
EXPORT_SYMBOL(aal_port_dpb_cfg_set);
EXPORT_SYMBOL(aal_port_arb_ldpid_pdpid_map_get);
EXPORT_SYMBOL(aal_port_init);
EXPORT_SYMBOL(aal_port_elpb_cfg_set);
EXPORT_SYMBOL(aal_port_dpb_ctrl_get);
EXPORT_SYMBOL(aal_port_ippb_cfg_get);
EXPORT_SYMBOL(aal_port_trunk_group_hash_set);
EXPORT_SYMBOL(aal_port_trunk_dest_map_set);
EXPORT_SYMBOL(aal_port_arb_ldpid_pdpid_map_set);
EXPORT_SYMBOL(aal_port_dpb_ctrl_set);
EXPORT_SYMBOL(aal_port_dpb_cfg_get);
EXPORT_SYMBOL(aal_port_trunk_dest_map_get);
