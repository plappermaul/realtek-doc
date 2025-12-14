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
 * aal_lpm.h: Hardware Abstraction Layer for LPM to access hardware regsiters
 *
 */
#ifndef __AAL_LPM_H__
#define __AAL_LPM_H__

#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif
#include "aal_table.h"
#include "aal_common.h"

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define LPM_TBL_ENTRY_MAX	512	/* 2^9 */
#else
#define LPM_TBL_ENTRY_MAX	1024	/* 2^10 */
#endif

enum {
	LPM_PROFILE_0 = 0,
	LPM_PROFILE_1,
	LPM_PROFILE_2,
	LPM_PROFILE_3,

	LPM_PROFILE_MAX,		/* HW limits 4 profiles */
};

/* HW bitmap of LPM Misc Status register */
#define LPM_MISC_STS_IPV4_MISS		0x1	/* IPv4 LPM search is on, but search fails */
#define LPM_MISC_STS_IPV6_MISS		0x2	/* IPv6 LPM search is on, but search fails */
#define LPM_MISC_STS_ILL		0x4	/* LPM search is on, but HDR_I.L3_VLD is 0 */
#define LPM_MISC_STS_BYPASS		0x8	/* HDR_I bypasses LPM search */

/***************************************************************************/

/* IPv4 and IPv6 share same C struct */
typedef struct lpm_action_ctrl_s {
	ca_uint32_t	default_idx		: 12;	// MDATA_L[11:0]
	ca_uint32_t	default_ctrl		: 4;	// t4ctrl, choose NextHop profile
	ca_uint32_t	default_dpid		: 6;	// LDPID
	ca_uint32_t	default_dpid_vld	: 1;	// valid bit of default_dpid
	ca_uint32_t	reserved_0		: 1;	// unused
	ca_uint32_t	default_stg		: 2;	// bit 1: update HDR_I.stage3_ctrl
							// bit 0: update HDR_I.stage2_ctrl
	ca_uint32_t	default_stg_ud		: 2;	// bit 1: enable/disable bit 1 of default_stg
							// bit 0: enable/disable bit 0 of default_stg
	ca_uint32_t	reserved_1		: 4;	// unused
} __attribute__((packed)) lpm_action_ctrl_t;

/***************************************************************************/

/* same as the attr in lpm_tbl_entry_t, 2 bits */
enum {
	ATTR_IPV4 = 0,
	ATTR_IPV6_LEN_0_32,	/* prefix len 0~32 bits */
	ATTR_IPV6_LEN_33_64,	/* prefix len 33~64 bits, use 2 entries */
	ATTR_IPV6_LEN_65_128,	/* prefix len 65~128 bits, use 4 entries */
};

typedef struct lpm_tbl_entry_s {
	ca_uint64_t	data		: 32;	// IP DA, host order
	ca_uint64_t	mask		: 6;	// mask bit length of data
	ca_uint64_t	attr		: 2;
	ca_uint64_t	rst_idx		: 12;	// MDATA_L[11:0]
	ca_uint64_t	rst_ctrl	: 1;	// choose NextHop profile which is assigned in default_t4ctrl
	ca_uint64_t	valid		: 1;

	/* NOT set to register; SW only */
	ca_uint64_t	profile		: 2;	// LPM Profile#, t3_ctrl.

	ca_uint64_t	reserved	: 8;
} __attribute__((packed)) lpm_tbl_entry_t;

/***************************************************************************/

int aal_lpm_default_t4ctrl_set(ca_ip_afi_t afi, unsigned int t4ctrl_0, unsigned int t4ctrl_1);
int aal_lpm_default_t4ctrl_get(ca_ip_afi_t afi, unsigned int *t4ctrl_0, unsigned int *t4ctrl_1);
int aal_lpm_default_action_set(ca_ip_afi_t afi, lpm_action_ctrl_t *val);
int aal_lpm_default_action_get(ca_ip_afi_t afi, lpm_action_ctrl_t *val);

int aal_lpm_first_row_set(ca_ip_afi_t afi, unsigned int profile, unsigned int row);
int aal_lpm_first_row_get(ca_ip_afi_t afi, unsigned int profile, unsigned int *row);
int aal_lpm_last_row_set(ca_ip_afi_t afi, unsigned int profile, unsigned int row);
int aal_lpm_last_row_get(ca_ip_afi_t afi, unsigned int profile, unsigned int *row);

bool aal_lpm_hit_status_get(unsigned int idx);
uint32_t aal_lpm_misc_status_get(void);

int __aal_lpm_tbl_init(void);
void __aal_lpm_tbl_exit(void);

int aal_lpm_init(void);

#endif /* __AAL_LPM_H__ */

