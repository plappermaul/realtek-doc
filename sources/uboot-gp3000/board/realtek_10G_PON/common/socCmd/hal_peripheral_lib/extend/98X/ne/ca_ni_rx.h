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
#ifndef __CA_NI_RX_H__
#define __CA_NI_RX_H__

#if 0//yocto
#include <linux/netdevice.h>
#else//sd1 uboot for 98f
#endif

typedef union {
	u32 bits32[2];
	struct bit_header_xr {
		u32 rsv1				: 32;	/* bits 0:31 */
		u32 next_link				: 12;	/* bits 32:43 */
		u32 bytes_valid				: 4;	/* bits 34:47 */
		u32 rsv2				: 12;	/* bits 48:59 */
		u32 invalid_pkt_len			: 1;	/* bits 60 */
		u32 error_flag				: 1;	/* bits 61 */
		u32 hdr_a				: 1;	/* bits 62 */
		u32 ownership				: 1;	/* bits 63 */
	} bits;
} HEADER_XR_T;

typedef union {
	struct bit_header_e
	{
		__u64 pkt_size				: 14;	/*  0 ~ 13 */
		__u64 fwd_type				: 4;	/* 14 ~ 17 */
		__u64 fwd_pkt_info			: 4;	/* 18 ~ 21 */
		__u64 mc_grp_id				: 9;	/* 22 ~ 30 */
		__u64 cos				: 3;	/* 31 ~ 33 */
		__u64 mark				: 1;	/* 34      */
		__u64 acl_replaced_voq_valid		: 1;	/* 35      */
		__u64 cpu_header			: 1;	/* 36      */
		__u64 cpu_ptp_flag			: 1;	/* 38      */
		__u64 spare				: 4;	/* 39 ~ 42 */
		__u64 lspid				: 4;	/* 43 ~ 46 */
		__u64 time_stamped_pkt			: 1;	/* 47      */
		__u64 resevred				: 13;	/* 51 ~ 63 */
		__u64 linux_buffer_number		: 3;	/* 48 ~ 50 */
	} bits;
	__u32 bits32[2];
}  HEADER_E_T;

typedef union
{
	__u64 bits64;
	struct bit_cpu0
	{
		__u64 cpuhdr_voq			: 7;	/*  0 ~ 6  */
		__u64 acl_vld_dvoq_cpu			: 1;	/*  7      */
		__u64 flags				: 13;	/*  8 ~ 20 */
		__u64 pspid				: 4;	/* 21 ~ 24 */
		__u64 hash_result_idx			: 13;	/* 25 ~ 37 */
		__u64 dst_voq				: 8;	/* 38 ~ 45 */
		__u64 acl_id				: 6;	/* 46 ~ 51 */
		__u64 l2l3_flags			: 4;	/* 52 ~ 55 */
		__u64 lpm_result_idx			: 6;	/* 56 ~ 61 */
		__u64 fwd_valids			: 2;	/* 62 ~ 63 */
	} bits;
	__u32 bits32[2];
}  CPU_HEADER0_T;

typedef union
{
	__u64 bits64;
	struct bit_cpu1
	{
		__u64 sw_action				: 20;	/*  0 ~ 19 */
		__u64 input_l3_start			: 8;	/* 20 ~ 27 */
		__u64 ipv4_csum_err			: 1;	/* 28      */
		__u64 l4_csum_err			: 1;	/* 29      */
		__u64 cpucopy_valid			: 1;	/* 30      */
		__u64 spare				: 1;	/* 31      */
		__u64 superhash				: 16;	/* 32 ~ 47 */
		__u64 superhash_vld			: 1;	/* 48      */
		__u64 svidx				: 6;	/* 49 ~ 54 */
		__u64 class_match			: 1;	/* 55      */
		__u64 input_l4_start			: 8;	/* 56 ~ 63 */
	} bits;
	__u32 bits32[2];
}  CPU_HEADER1_T;


typedef union {
	u32 bits32;
	struct bit_cpu_epp_fifo_cmd {
		u32 eqid	                        : 4;    /* bits 0 ~ 3 */
		u32 sop		                        : 1;    /* bits 4 */
		u32 l4_csum_err	                        : 1;    /* bits 5 */
		u32 eop		                        : 1;    /* bits 6 */
		u32 phy_addr                            : 25;   /* bits 7 ~ 31 */
	} bits;
} CPU_EPP_FIFO_CMD_T;

typedef struct refill_pyh_addr_info {
	u32 phy_addr;
	u8  eqid;
} refill_pyh_addr_info_t;


int ca_ni_rx_poll(int cpu_port, struct napi_struct *napi, int budget, int mgmt_mode);
int ca_ni_mgmt_recv(int instance, struct napi_struct *napi, int budget);

#endif /* __CA_NI_RX_H__ */

