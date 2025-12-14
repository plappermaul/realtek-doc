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
 * aal_ni_cputag.h: Hardware Abstraction Layer for NI CPU TAG
 *
 */
#ifndef __AAL_NI_CPUTAG_H__
#define __AAL_NI_CPUTAG_H__

typedef struct {
	ca_uint32_t port_id_0		: 8;
	ca_uint32_t port_id_1		: 8;
	ca_uint32_t port_id_2		: 8;
	ca_uint32_t port_id_3		: 8;
	ca_uint32_t port_id_4		: 8;
	ca_uint32_t port_id_5		: 8;
	ca_uint32_t port_id_6		: 8;
	ca_uint32_t reserved		: 8;
} __attribute__((packed)) ca_ni_glb_cpu_tag_port_id_t;

typedef struct {
	ca_uint32_t port_msk_0		: 8;
	ca_uint32_t port_msk_1		: 8;
	ca_uint32_t port_msk_2		: 8;
	ca_uint32_t port_msk_3		: 8;
	ca_uint32_t port_msk_4		: 8;
	ca_uint32_t port_msk_5		: 8;
	ca_uint32_t port_msk_6		: 8;
	ca_uint32_t reserved		: 8;
} __attribute__((packed)) ca_ni_glb_cpu_tag_port_mk_t;

typedef struct {
	ca_uint32_t cos			: 3;	/* COS */
	ca_uint32_t ldpid		: 6;	/* Logical Destination Port ID, Packet from MC Engine, this field carries MC_INDEX */
	ca_uint32_t lspid		: 6;	/* Original Logical Source Port ID */
	ca_uint32_t pkt_size		: 14;	/* Packet size */
	ca_uint32_t fe_bypass		: 1;	/* Bypass L2FE and L3FE processing */
	ca_uint32_t hdr_type		: 2;	/* 00: Generic* 01: MC HDR
						 * 01: MC HDR
						 * 10: CPU bound pkt
						 * 11: PTP ingress or egress pkt
						 */

	ca_uint32_t mcgid		: 8;	/* When packet type is 2'b10(CPU), redefined as L2 learn info */
	ca_uint32_t drop_code		: 3;	/* Drop event code, used by TE */
	ca_uint32_t rx_pkt_type		: 2;	/* 00:UC, 01:BC, 10:MC, 11:Unknown-UC
						   for l3fe->l2fe traffic, use uc(00) to disable l2 learning
						   uuc(11) to enable l2 learning */
	ca_uint32_t no_drop		: 1;	/* No-Drop flag for high priority packet */
	ca_uint32_t mirror		: 1;	/* Packet to be mirrored */
	ca_uint32_t mark		: 1;	/* Conjestion experienced in QM */
	ca_uint32_t pol_en		: 2;	/* 2'b00: Policer disable
						   2'b01: L2 Policer enable
						   2'b10: L3 Policer enable
						   2'b11: both L2,L3 Policer enable
						 */

	ca_uint32_t pol_id		: 9;	/* Used for Policing in TE/QTM */
	ca_uint32_t pol_grp_id		: 3;	/* Policer Group ID, group 0 is NULL */
	ca_uint32_t deep_q		: 1;	/* Packet target to DeepQ in QM */
	ca_uint32_t cpu_flg		: 1;	/* CPU header is included */
} __attribute__((packed)) ca_ni_cpu_tag_hdra_t;

typedef struct {
	ca_uint32_t donttouch		: 16;
	ca_uint32_t cpu_tag_rx_en	: 1;
	ca_uint32_t cpu_tag_tx_en	: 1;
	ca_uint32_t cpu_tag_cfg_sel	: 1;
	ca_uint32_t donttouch2		: 13;
} __attribute__((packed)) ca_ni_cpu_tag_cfg_t;

/* AAL CLI CPU TAG debug functions */
ca_status_t aal_ni_glb_cpu_tag_port_id_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t cpu_tag_idx,
	CA_OUT ca_ni_glb_cpu_tag_port_id_t *reg
);

ca_status_t aal_ni_glb_cpu_tag_port_id_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t cpu_tag_idx,
	CA_IN ca_ni_glb_cpu_tag_port_id_t *reg
);

ca_status_t aal_ni_glb_cpu_tag_port_msk_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t cpu_tag_idx,
	CA_OUT ca_ni_glb_cpu_tag_port_mk_t *reg
);

ca_status_t aal_ni_glb_cpu_tag_port_msk_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t cpu_tag_idx,
	CA_IN ca_ni_glb_cpu_tag_port_mk_t *reg
);

ca_status_t aal_ni_glb_cpu_tag_hdra_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t cpu_tag_idx,
	CA_IN ca_uint16_t hdra_idx,
	CA_OUT ca_ni_cpu_tag_hdra_t *entry
);

ca_status_t aal_ni_glb_cpu_tag_hdra_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t cpu_tag_idx,
	CA_IN ca_uint16_t hdra_idx,
	CA_OUT ca_ni_cpu_tag_hdra_t *entry
);

ca_status_t aal_ni_pt_cpu_tag_cfg_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t port_id,
	CA_OUT ca_ni_cpu_tag_cfg_t *entry
);

ca_status_t aal_ni_pt_cpu_tag_cfg_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t port_id,
	CA_IN ca_ni_cpu_tag_cfg_t *entry
);

ca_status_t aal_ni_pt_cpu_tag_dft_hdra_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t port_id,
	CA_OUT ca_ni_cpu_tag_hdra_t *entry
);

ca_status_t aal_ni_pt_cpu_tag_dft_hdra_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t port_id,
	CA_IN ca_ni_cpu_tag_hdra_t *entry
);
#endif

