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
 * aal_mcast.h: Hardware Abstraction Layer for Multicast to access hardware regsiters
 *
 */
#ifndef __AAL_MCAST_H__
#define __AAL_MCAST_H__

#include "ca_types.h"
#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif

#define MCE_INDX_TBL_ENTRY_MAX		256
#define ARB_FIB_TBL_ENTRY_MAX		64
#define IM3E_MLDDA_ENTRY_MAX		256
#define IM3E_MLDSA_ENTRY_MAX		64
#define L2_PE_MAC_SA_LAN_IDX		1

typedef struct {
	ca_uint64_t mc_vec		: 64;
} __attribute__((packed)) ni_mce_indx_tbl_entry_t;

typedef struct {
	ca_mac_addr_t mac_da;
	ca_uint32_t mac_cmd		: 1;
	ca_uint32_t dei			: 1;
	ca_uint32_t dei_cmd		: 1;
	ca_uint32_t dot1p		: 3;
	ca_uint32_t dot1p_cmd		: 2;
	ca_uint32_t sc_ind		: 1;
	ca_uint32_t vid			: 12;
	ca_uint32_t vlan_cmd		: 3;
	ca_uint32_t vlan_fltr_en	: 1;
	ca_uint32_t mcgid		: 10;
	ca_uint32_t mcgid_en		: 1;
	ca_uint32_t pol_id		: 9;
	ca_uint32_t pol_grp_id		: 3;
	ca_uint32_t pol_en		: 1;
	ca_uint32_t cos			: 3;
	ca_uint32_t ldpid		: 6;
	ca_uint32_t mac_sa_sel		: 1;
	ca_uint32_t mac_sa_cmd		: 1;
	ca_uint32_t permit_spid_en	: 1;
	ca_uint32_t wan_dst		: 1;
	ca_uint32_t cos_cmd		: 1;
	ca_uint32_t rsvd		: 17;
} __attribute__((packed)) l2fe_arb_fib_tbl_entry_t;

typedef struct {
	ca_uint32_t	entry_valid	: 1;
	ca_uint32_t	mc_group_id	: 10;
	ca_uint32_t	v3_ind		: 1;
	ca_uint32_t	connection_id	: 8;
	ca_uint32_t	ipv6_da_0	: 32;
	ca_uint32_t	ipv6_da_1	: 32;
	ca_uint32_t	ipv6_da_2	: 32;
	ca_uint32_t	ipv6_da_3	: 32;
	ca_uint32_t	ipv6_flg	: 1;
	ca_uint32_t	vid		: 12;
	ca_uint32_t	vlan_sc_ind	: 1;
	ca_uint32_t	reserved	: 30;
} __attribute__((packed)) im3e_mldda_entry_t;

typedef struct {
	ca_uint32_t	entry_valid	: 1;
	ca_uint32_t	mc_group_id	: 10;
	ca_uint32_t	ipv6_sa_0	: 32;
	ca_uint32_t	ipv6_sa_1	: 32;
	ca_uint32_t	ipv6_sa_2	: 32;
	ca_uint32_t	ipv6_sa_3	: 32;
	ca_uint32_t	connection_id	: 8;
	ca_uint32_t	reserved	: 13;
} __attribute__((packed)) im3e_mldsa_entry_t;

typedef struct {
	ca_mac_addr_t mac_addr;
	ca_uint32_t rsvd		: 16;
} __attribute__((packed)) l2fe_pe_my_mac_tbl_entry_t;

typedef enum {
	MC_L2 = 0,
	MC_L3 = 1,
	MC_OTHER = 2
} mcgid_type_t;

typedef struct repl_key_s {
	ca_uint16_t ldpid;
	ca_uint16_t vlan_cmd;
	ca_uint16_t vlan;
	ca_uint16_t mac_da_cmd;
	ca_mac_addr_t mac_da;
	ca_uint16_t mac_sa_cmd;
} repl_key_t;

int aal_mce_indx_init(void);
void aal_mce_indx_exit(void);

int aal_arb_fib_init(void);
void aal_arb_fib_exit(void);

int aal_im3e_control_set(/*CODEGEN_IGNORE_TAG*/L2FE_IM3E_CTRL_t *ctrl);
int aal_im3e_control_get(/*CODEGEN_IGNORE_TAG*/L2FE_IM3E_CTRL_t *ctrl);

int aal_im3e_mldda_init(void);
void aal_im3e_mldda_exit(void);

int aal_im3e_mldsa_init(void);
void aal_im3e_mldsa_exit(void);

/* AAL helper functions */
ca_uint16_t aal_mc_fake_vid_get(/*CODEGEN_IGNORE_TAG*/ca_uint16_t wan_vid);
ca_uint16_t aal_mc_fake_vid_alloc(ca_uint16_t wan_vid);
void aal_mc_fake_vid_free(ca_uint16_t wan_vid);

ca_int16_t aal_mc_mcgid_alloc(void);
void aal_mc_mcgid_set_callback(ca_uint16_t mcgid, mcgid_type_t type, ca_int8_t *ptr);
ca_int8_t* aal_mc_mcgid_ptr_get(/*CODEGEN_IGNORE_TAG*/ca_uint16_t mcgid, mcgid_type_t type);

ca_int16_t aal_mc_fake_mcgid_alloc(void);
void aal_mc_fake_mcgid_set_callback(ca_uint16_t mcgid, mcgid_type_t type, ca_int8_t *ptr);
ca_int8_t* aal_mc_fake_mcgid_ptr_get(/*CODEGEN_IGNORE_TAG*/ca_uint16_t mcgid, mcgid_type_t type);

ca_status_t aal_mc_mcgid_free(ca_int16_t mcgid);
ca_status_t aal_mc_fake_mcgid_free(ca_int16_t mcgid);

ca_int16_t aal_mc_repl_bit_alloc(repl_key_t *key);
ca_int16_t aal_mc_repl_bit_get(/*CODEGEN_IGNORE_TAG*/repl_key_t *key);
ca_status_t aal_mc_repl_bit_free(ca_int16_t port_bit);

ca_int16_t aal_flooding_group_create(void);
ca_status_t aal_flooding_group_delete(
	ca_int16_t mcgid);
ca_status_t aal_flooding_port_add(
	ca_int16_t mcgid,
	ca_int16_t ldpid);
ca_status_t aal_flooding_port_delete(
	ca_int16_t mcgid,
	ca_int16_t ldpid);

ca_int16_t aal_flooding_fib_idx_alloc(void);
ca_status_t aal_flooding_fib_idx_free(ca_int16_t mc_fib_idx);
ca_status_t aal_flooding_mc_fib_add(ca_int16_t mcgid, ca_int16_t mc_fib_idx);
ca_status_t aal_flooding_mc_fib_delete(ca_int16_t mcgid, ca_int16_t mc_fib_idx);

ca_status_t aal_l2fe_pe_my_mac_set(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l2fe_pe_my_mac_tbl_entry_t *entry
);

/* AAL CLI debug functions */
ca_status_t aal_mc_ni_mce_indx_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT ni_mce_indx_tbl_entry_t *entry
);

ca_status_t aal_mc_ni_mce_indx_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN ni_mce_indx_tbl_entry_t *entry
);

ca_status_t aal_mc_l2fe_arb_fib_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT l2fe_arb_fib_tbl_entry_t *entry
);

ca_status_t aal_mc_l2fe_arb_fib_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l2fe_arb_fib_tbl_entry_t *entry
);

ca_status_t aal_mc_im3e_mldda_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT im3e_mldda_entry_t *entry
);

ca_status_t aal_mc_im3e_mldda_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN im3e_mldda_entry_t *entry
);

ca_status_t aal_mc_im3e_mldsa_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT im3e_mldsa_entry_t *entry
);

ca_status_t aal_mc_im3e_mldsa_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN im3e_mldsa_entry_t *entry
);

#endif /* __AAL_MCAST_H__ */
