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
 * aal_l3_pe.h: Hardware Abstraction Layer for Egress L3 If table to access hardware regsiters
 *
 */
#ifndef __AAL_L3_PE_H__
#define __AAL_L3_PE_H__

#include "ca_types.h"
#include "aal_table.h"

/***************************************************************************
 * PPPoE encapsulation
 ***************************************************************************/

int aal_l3pe_pppoe_cfg_set(unsigned char code, unsigned char ver_type);
int aal_l3pe_pppoe_cfg_get(unsigned char *code, unsigned char *ver_type);
int aal_l3pe_pppoe_ipv4_prot_set(unsigned short prot);
int aal_l3pe_pppoe_ipv4_prot_get(unsigned short *prot);
int aal_l3pe_pppoe_ipv6_prot_set(unsigned short prot);
int aal_l3pe_pppoe_ipv6_prot_get(unsigned short *prot);

/***************************************************************************
 * PE configuration
 ***************************************************************************/

int aal_l3pe_dsl_tbl_clear(void);
int aal_l3pe_6rd_tbl_clear(void);

int aal_l3pe_pe_stomp_crc_set(bool mode);
int aal_l3pe_pe_stomp_crc_get(bool *mode);

int aal_l3pe_pe_bypass_crc_set(bool mode);
int aal_l3pe_pe_bypass_crc_get(bool *mode);

int aal_l3pe_pe_ignore_tqm_fe_rdy_set(bool mode);
int aal_l3pe_pe_ignore_tqm_fe_rdy_get(bool *mode);

/***************************************************************************
 * PE Port Egress Behavior
 ***************************************************************************/

typedef struct l3pe_port_egress_behavior_s {
	ca_uint32_t	ldpid 			: 6;
	ca_uint32_t	reserved0		: 19;
	ca_uint32_t	bypass_l2_edit		: 1;
	ca_uint32_t	bypass_l3_edit		: 1;
	ca_uint32_t	bypass_l4_edit		: 1;
	ca_uint32_t	bypass_l3_encap		: 1;
	ca_uint32_t	bypass_l3_decap		: 1;
	ca_uint32_t	reserved1		: 1;
	ca_uint32_t	bypass_padding		: 1;

	ca_uint32_t	mtu_check		: 1;
	ca_uint32_t	redirect_ldpid		: 6;
	ca_uint32_t	redirect_cos		: 3;
	ca_uint32_t	reserved2		: 2;
	ca_uint32_t	cpu_cmd			: 3;
	ca_uint32_t	reserved3		: 17;

	ca_uint32_t	reserved4;
} __attribute__((packed)) l3pe_port_egress_behavior_t;

int aal_l3pe_port_egress_behavior_set(ca_device_id_t device_id, unsigned int idx, l3pe_port_egress_behavior_t *entry);
int aal_l3pe_port_egress_behavior_get(ca_device_id_t device_id, unsigned int idx, l3pe_port_egress_behavior_t *entry);

/***************************************************************************
 * MTU
 ***************************************************************************/

int aal_l3pe_mtu_size_set(unsigned int idx, unsigned int mtu);
int aal_l3pe_mtu_size_get(unsigned int idx, unsigned int *mtu);

int aal_l3pe_mtu_size_add(/*CODEGEN_IGNORE_TAG*/unsigned int mtu, unsigned int *idx);
int aal_l3pe_mtu_size_del(/*CODEGEN_IGNORE_TAG*/unsigned int idx);
int aal_l3pe_mtu_size_lookup(/*CODEGEN_IGNORE_TAG*/unsigned int mtu, unsigned int *idx);


/***************************************************************************
 * Mirror
 ***************************************************************************/

int aal_l3pe_mirror_set(unsigned int ldpid, unsigned int cos, unsigned int fifo_high_thold, unsigned int fifo_low_thold);
int aal_l3pe_mirror_len_set(unsigned int len);

/***************************************************************************
 * Config - 6RD and DS-Lite
 ***************************************************************************/

int aal_l3pe_6rd_df_set(bool df);
int aal_l3pe_6rd_df_get(bool *df);
int aal_l3pe_6rd_encap_nh_set(unsigned char nh);
int aal_l3pe_6rd_encap_nh_get(unsigned char *nh);
int aal_l3pe_dsl_encap_nh_set(unsigned char nh);
int aal_l3pe_dsl_encap_nh_get(unsigned char *nh);

/***************************************************************************
 * EtherType for Encapsulation - 6RD and DS-Lite
 ***************************************************************************/

int aal_l3pe_6rd_encap_ethertype_set(uint16_t ethertype);
int aal_l3pe_6rd_encap_ethertype_get(uint16_t *ethertype);
int aal_l3pe_dsl_encap_ethertype_set(uint16_t ethertype);
int aal_l3pe_dsl_encap_ethertype_get(uint16_t *ethertype);

/***************************************************************************
 * EtherType for Decapsulation - 6RD and DS-Lite
 ***************************************************************************/

int aal_l3pe_6rd_decap_ethertype_set(uint16_t ethertype);
int aal_l3pe_6rd_decap_ethertype_get(uint16_t *ethertype);
int aal_l3pe_dsl_decap_ethertype_set(uint16_t ethertype);
int aal_l3pe_dsl_decap_ethertype_get(uint16_t *ethertype);

/***************************************************************************
 * 6RD
 ***************************************************************************/

#if defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define L3PE_SIXRD_TBL_ENTRY_MAX	8	/* 2^3 */
#else
#define L3PE_SIXRD_TBL_ENTRY_MAX	16	/* 2^4 */
#endif

typedef struct l3pe_sixrd_tbl_entry_s {
	ca_uint32_t	valid		: 1;
	ca_uint32_t	ingr_chk_en	: 1;
	ca_uint32_t	br_only		: 1;
	ca_uint32_t	ipsa_mtch	: 1;	/* Check if pkt comes from BR. MUST set ingr_chk_en to enable ipsa_mtch. */
	ca_uint32_t	id_end		: 16;
	ca_uint32_t	id_start	: 16;
	ca_uint32_t	ttl_en		: 1;
	ca_uint32_t	ttl		: 8;
	ca_uint32_t	tos_en		: 1;
	ca_uint32_t	tos		: 8;
	ca_uint32_t	br_v4_ip	: 32;
	ca_uint64_t	v6_prfx		: 64;	/* IPv6[127:64] */
	ca_uint32_t	v6_prfx_len	: 6;	/* Max is 63. v6_prfx_len + v4_msk_len must NOT exceed 64. */
	ca_uint32_t	v4_msk_len	: 5;	/* Max is 31. If system requires 32, set FIB.six_rd_ipda_from_v6. */
	ca_uint32_t	ce_v4_ip	: 32;
	ca_uint32_t	reserved	: 3;
} __attribute__((packed)) l3pe_sixrd_tbl_entry_t;

int aal_l3pe_sixrd_tbl_init(void);
void aal_l3pe_sixrd_tbl_exit(void);

/***************************************************************************
 * DS-Lite
 ***************************************************************************/

#if defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define L3PE_DSL_TBL_ENTRY_MAX	8	/* 2^3 */
#else
#define L3PE_DSL_TBL_ENTRY_MAX	16	/* 2^4 */
#endif

typedef struct l3pe_dsl_tbl_entry_s {
	ca_uint32_t	valid		: 1;
	ca_uint32_t	mc		: 1;	/* set 0 for Unicast only, set 1 for Multicast only */
	ca_uint32_t	flwlabel	: 20;
	ca_uint32_t	hoplimit_en	: 1;
	ca_uint32_t	hoplimit	: 8;
	ca_uint32_t	tc_en		: 1;
	ca_uint32_t	tc		: 8;

	/* If mc is 0, aftr_ip[127:0] is AFTR IP, b4_ip[127:0] is B4 IP.
	 * if mc is 1, aftr_ip[127:0] is {uPrefix64, uPrefix_msk64}, b4_ip[127:0] is {mPrefix64, mPrefix_msk64}.
	 */
	ca_uint32_t	aftr_ip0	: 32;
	ca_uint32_t	aftr_ip1	: 32;
	ca_uint32_t	aftr_ip2	: 32;
	ca_uint32_t	aftr_ip3	: 32;
	ca_uint32_t	b4_ip0		: 32;
	ca_uint32_t	b4_ip1		: 32;
	ca_uint32_t	b4_ip2		: 32;
	ca_uint32_t	b4_ip3		: 32;
	ca_uint32_t	reserved	: 24;
} __attribute__((packed)) l3pe_dsl_tbl_entry_t;

int aal_l3pe_dsl_tbl_init(void);
void aal_l3pe_dsl_tbl_exit(void);

/***************************************************************************
 * Counter
 ***************************************************************************/

/* all counter of DROP_SRC_XXX */
#define L3PE_CNTR_DROP_SRC_ENTRY_MAX	64
#define L3PE_CNTR_DROP_SRC_DATA_REG_NUM	2

/* all counter of RX */
#define L3PE_CNTR_RX_ENTRY_MAX		128
#define L3PE_CNTR_RX_DATA_REG_NUM	8

/* all coutner of TX */
#define L3PE_CNTR_TX_ENTRY_MAX		128
#define L3PE_CNTR_TX_DATA_REG_NUM	7

/* L3FE_PE_MIB_CFG.counter_wrap_around */
#define L3PE_MODE_DROP_SRC_PKT		BIT(0)
#define L3PE_MODE_DROP_SRC_BYTE		BIT(1)
#define L3PE_MODE_RX_BYTE		BIT(2)
#define L3PE_MODE_RX_UC_PKT		BIT(3)
#define L3PE_MODE_RX_NUC_PKT		BIT(4)
#define L3PE_MODE_RX_DROP_BYTE		BIT(5)
#define L3PE_MODE_RX_DROP_PKT		BIT(6)
#define L3PE_MODE_RX_ERR_PKT		BIT(7)
#define L3PE_MODE_TX_BYTE		BIT(8)
#define L3PE_MODE_TX_UC_PKT		BIT(9)
#define L3PE_MODE_TX_NUC_PKT		BIT(10)
#define L3PE_MODE_TX_DROP_BYTE		BIT(11)
#define L3PE_MODE_TX_DROP_PKT		BIT(12)
#define L3PE_MODE_MASK			(BIT_MASK(13) - 1)

/* counter mode for each bit in L3FE_PE_MIB_CFG.counter_wrap_around */
typedef enum {
	L3PE_CNTR_MODE_STOP = 0,	/* stop the counter when it overflows */
	L3PE_CNTR_MODE_WRAP = 1,	/* wrap around the counter when it overflows */
} l3pe_cntr_mode_e;

/* L3FE_PE_MIB_CFG.counter_clear */
#define L3PE_CLEAR_DROP_SRC_PKT		BIT(0)
#define L3PE_CLEAR_DROP_SRC_BYTE	BIT(1)
#define L3PE_CLEAR_RX			BIT(2)
#define L3PE_CLEAR_TX			BIT(3)
#define L3PE_CLEAR_MASK			(BIT_MASK(4) - 1)

typedef struct {
	ca_uint64_t	drop_src_pkt	: 64;
} __attribute__((packed)) l3pe_cntr_drop_src_pkt_t;

typedef struct {
	ca_uint64_t	drop_src_byte	: 64;
} __attribute__((packed)) l3pe_cntr_drop_src_byte_t;

typedef struct {
	ca_uint64_t	byte		: 64;	/* Rx byte */
	ca_uint64_t	uc_pkt		: 32;	/* Unicast pkt count */
	ca_uint64_t	nuc_pkt		: 32;	/* None-Unicast pkt count */
	ca_uint64_t	drop_byte	: 64;	/* Droped byte */
	ca_uint64_t	drop_pkt	: 32;	/* Dropped pkt count */
	ca_uint64_t	error_pkt	: 32;	/* Error pkt count */
} __attribute__((packed)) l3pe_cntr_rx_t;

typedef struct {
	ca_uint64_t	byte		: 64;	/* Tx byte */
	ca_uint64_t	uc_pkt		: 32;	/* Unicast pkt count */
	ca_uint64_t	nuc_pkt		: 32;	/* None-Unicast pkt count */
	ca_uint64_t	drop_byte	: 64;	/* Dropped byte */
	ca_uint64_t	drop_pkt	: 32;	/* Dropped pkt count */
} __attribute__((packed)) l3pe_cntr_tx_t;

int aal_l3pe_cntr_drop_src_pkt_set(ca_device_id_t device_id, unsigned int idx, ca_uint64 count);
int aal_l3pe_cntr_drop_src_pkt_get(ca_device_id_t device_id, unsigned int idx, ca_uint64 *count);

int aal_l3pe_cntr_drop_src_byte_set(ca_device_id_t device_id, unsigned int idx, ca_uint64 count);
int aal_l3pe_cntr_drop_src_byte_get(ca_device_id_t device_id, unsigned int idx, ca_uint64 *count);

int aal_l3pe_cntr_rx_set(ca_device_id_t device_id, unsigned int idx, l3pe_cntr_rx_t *data);
int aal_l3pe_cntr_rx_get(ca_device_id_t device_id, unsigned int idx, l3pe_cntr_rx_t *data);

int aal_l3pe_cntr_tx_set(ca_device_id_t device_id, unsigned int idx, l3pe_cntr_tx_t *data);
int aal_l3pe_cntr_tx_get(ca_device_id_t device_id, unsigned int idx, l3pe_cntr_tx_t *data);

int aal_l3pe_cntr_drop_src_pkt_clear(ca_device_id_t device_id);
int aal_l3pe_cntr_drop_src_byte_clear(ca_device_id_t device_id);
int aal_l3pe_cntr_rx_clear(ca_device_id_t device_id);
int aal_l3pe_cntr_tx_clear(ca_device_id_t device_id);

int aal_l3pe_cntr_cfg_set(ca_device_id_t device_id, ca_uint32_t cntr_mode);
int aal_l3pe_cntr_cfg_get(ca_device_id_t device_id, ca_uint32_t *cntr_mode);

/***************************************************************************
 * Initialization
 ***************************************************************************/

int aal_l3pe_init(void);

#endif /* __AAL_L3_PE_H__ */
