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
 * aal_l3_if.h: Hardware Abstraction Layer for Egress L3 If table to access hardware regsiters
 *
 */
#ifndef __AAL_L3_CAM_H__
#define __AAL_L3_CAM_H__

#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif

#define L3_CAM_DPORT_TBL_ENTRY_MAX	16
#define L3_CAM_SPORT_TBL_ENTRY_MAX	16
#define L3_CAM_ETHERTYPE_TBL_ENTRY_MAX	63
#define L3_CAM_MAC_DA_TBL_ENTRY_MAX	7
#define L3_CAM_HTTP_MSGS_TBL_ENTRY_MAX	16
#define L3_CAM_LLC_TBL_ENTRY_MAX	3
#define L3_CAM_PKT_LEN_TBL_ENTRY_MAX	4
#define L3_CAM_PPP_CODE_TBL_ENTRY_MAX	15
#define L3_CAM_PPP_PROT_TBL_ENTRY_MAX	15
#define L3_CAM_SPCL_HDR_TBL_ENTRY_MAX	8
#if defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define L3_CAM_IP_ADDR_TBL_ENTRY_MAX	7
#else
#define L3_CAM_IP_ADDR_TBL_ENTRY_MAX	15
#endif

typedef struct l3_cam_dport_tbl_entry_s {
	ca_uint32_t dport_low	: 16;
	ca_uint32_t dport_hi	: 16;

	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 31;

	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
	ca_uint32_t reserved_3	: 32;
} __attribute__((packed)) l3_cam_dport_tbl_entry_t;

typedef struct l3_cam_sport_tbl_entry_s {
	ca_uint32_t sport_low	: 16;
	ca_uint32_t sport_hi	: 16;

	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 31;

	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
	ca_uint32_t reserved_3	: 32;
} __attribute__((packed)) l3_cam_sport_tbl_entry_t;

typedef struct l3_cam_ethertype_tbl_entry_s {
	ca_uint32_t ethertype	: 16;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 15;

	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
	ca_uint32_t reserved_3	: 32;
	ca_uint32_t reserved_4	: 32;
} __attribute__((packed)) l3_cam_ethertype_tbl_entry_t;

typedef struct l3_cam_mac_da_tbl_entry_s {
	ca_uint32_t mac_5	: 8;
	ca_uint32_t mac_4	: 8;
	ca_uint32_t mac_3	: 8;
	ca_uint32_t mac_2	: 8;
	ca_uint32_t mac_1	: 8;
	ca_uint32_t mac_0	: 8;

	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 15;

	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
	ca_uint32_t reserved_3	: 32;
} __attribute__((packed)) l3_cam_mac_da_tbl_entry_t;

typedef struct l3_cam_http_msgs_tbl_entry_s {
	ca_uint64_t http_msg	: 64;

	ca_uint32_t byte_en	: 8;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 23;

	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
} __attribute__((packed)) l3_cam_http_msgs_tbl_entry_t;

typedef struct l3_cam_llc_tbl_entry_s {
	ca_uint32_t llc		: 24;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 7;
	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
} __attribute__((packed)) l3_cam_llc_tbl_entry_t;

typedef struct l3_cam_pkt_len_tbl_entry_s {
	ca_uint32_t pkt_len_low	: 14;
	ca_uint32_t pkt_len_hi	: 14;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 3;
	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
} __attribute__((packed)) l3_cam_pkt_len_tbl_entry_t;

typedef struct l3_cam_ppp_code_tbl_entry_s {
	ca_uint32_t ppp_code	: 8;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 23;
	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
} __attribute__((packed)) l3_cam_ppp_code_tbl_entry_t;

typedef struct l3_cam_ppp_prot_tbl_entry_s {
	ca_uint32_t ppp_prot	: 16;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 15;
	ca_uint32_t reserved_1	: 32;
	ca_uint32_t reserved_2	: 32;
} __attribute__((packed)) l3_cam_ppp_prot_tbl_entry_t;

typedef struct l3_cam_spcl_hdr_tbl_entry_s {
	ca_uint32_t key		: 16;
	ca_uint32_t mask	: 16;
	ca_uint32_t ms_vld	: 1;
	ca_uint32_t type	: 3;
	ca_uint32_t vld		: 1;
	ca_uint32_t reserved_0	: 27;
	ca_uint32_t reserved_1	: 32;
} __attribute__((packed)) l3_cam_spcl_hdr_tbl_entry_t;

typedef struct l3_cam_ip_addr_tbl_entry_s {
	ca_uint32_t ip_addr_0	: 32;
	ca_uint32_t ip_addr_1	: 32;
	ca_uint32_t ip_addr_2	: 32;
	ca_uint32_t ip_addr_3	: 32;
	ca_uint32_t ip_mask	: 8;
	ca_uint32_t left_mask_flg : 1;
	ca_uint32_t ipv4_flg	: 1;
	ca_uint32_t ip_sa_vld	: 1;
	ca_uint32_t ip_da_vld	: 1;
	ca_uint32_t reserved_0	: 20;
} __attribute__((packed)) l3_cam_ip_addr_tbl_entry_t;

int aal_l3_cam_tbls_init(void);
void aal_l3_cam_tbls_exit(void);
int aal_l3_cam_mac_da_lookup(ca_uint8_t mac[6], unsigned int *idx);
ca_int32_t aal_l3fe_pp_top_tpid_get(/*CODEGEN_IGNORE_TAG*/ca_uint32_t tpid, ca_uint8_t *index);
ca_int32_t aal_l3fe_pp_inner_tpid_get(/*CODEGEN_IGNORE_TAG*/ca_uint32_t tpid, ca_uint8_t *index);
ca_int32_t aal_l3fe_pp_tpid_set(/*CODEGEN_IGNORE_TAG*/ca_uint32_t top_nums, ca_uint32_t *top_tpid, ca_uint32_t inner_nums, ca_uint32_t *inner_tpid);

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
ca_int32_t aal_l3fe_pp_tpid_saturn_set(/*CODEGEN_IGNORE_TAG*/ca_uint32_t top_nums, ca_uint32_t *top_tpid, ca_uint32_t inner_nums, ca_uint32_t *inner_tpid);
#endif

/* AAL CLI debug functions */
ca_status_t aal_l3_cam_dport_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_dport_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_dport_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_dport_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_sport_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_sport_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_sport_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_sport_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ethertype_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_ethertype_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ethertype_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_ethertype_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_mac_da_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_mac_da_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_mac_da_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_mac_da_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_http_msgs_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_http_msgs_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_http_msgs_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_http_msgs_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_llc_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_llc_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_llc_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_llc_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_pkt_len_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_pkt_len_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_pkt_len_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_pkt_len_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ppp_code_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_ppp_code_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ppp_code_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_ppp_code_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ppp_prot_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_ppp_prot_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ppp_prot_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_ppp_prot_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_spcl_hdr_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_spcl_hdr_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_spcl_hdr_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_spcl_hdr_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ip_addr_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT l3_cam_ip_addr_tbl_entry_t *entry
);

ca_status_t aal_l3_cam_ip_addr_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN l3_cam_ip_addr_tbl_entry_t *entry
);

int aal_cam_init(void);

#endif /* __AAL_L3_CAM_H__ */

