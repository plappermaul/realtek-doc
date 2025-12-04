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
 * aal_fbm.h: Hardware Abstraction Layer for FBM to access hardware regsiters
 *
 */
#ifndef __AAL_FBM_H__
#define __AAL_FBM_H__

#include "ca_types.h"
#include "osal_cmn.h"

//#define FBM_REG_READ(offset)		CA_REG_READ(offset)
//#define FBM_REG_WRITE(val, offset)	CA_REG_WRITE(val, offset)

#define FBM_MAX_SEG_NUM		4
#define FBM_MAX_MSET_SIZE	0x1000

/* Global Control */
#define FBM_RELEASE_POOLID	0
#define FBM_RELEASE_SEGMNT	1
#define FBM_RELEASE_MODE	FBM_RELEASE_POOLID
#define FBM_ECC_EN		1
#define FBM_CNT_BURST_MODE	1	/* 64 bytes */
#define FBM_EXS_BURST_MODE	1	/* 16 entries */
#define FBM_CNT_BURST_SIZE	(32 << FBM_CNT_BURST_MODE)
#define FBM_EXS_BURST_SIZE	(8 << FBM_EXS_BURST_MODE)

/* Per Pool Control */
#define FBM_STACK_LOW_THRESHOLD		16
#define FBM_STACK_HIGH_THRESHOLD	48

typedef enum {
	QM_PRF0_Q0 = 0,
	QM_PRF0_Q1 = 1,
	QM_PRF1_Q0 = 2,
	QM_PRF1_Q1 = 1,
	QM_PRF2_Q0 = 6,
	QM_PRF2_Q1 = 7
} aal_fbm_pool_id_t;

typedef enum {
	FBM_CPU_ARM0 = 0,
	FBM_CPU_ARM1 = 1,
	FBM_CPU_PE0 = 2,
	FBM_CPU_PE1 = 3
} aal_fbm_cpu_id_t;

enum seg_size_e {
	FBM_SEG_64K,
	FBM_SEG_256K,
	FBM_SEG_1M,
	FBM_SEG_4M,
	FBM_SEG_16M
};

enum mset_mode_e{
	FBM_MSET_DISABLE,
	FBM_MSET_PAT0,
	FBM_MSET_PAT1,
	FBM_MSET_ZERO
};

struct aal_fbm_seg_config_s {
	ca_boolean_t		seg_enable;
	enum seg_size_e		seg_size_mode;
	ca_uint32_t		seg_bar;
};

struct aal_fbm_pool_config_s {
	ca_boolean_t		cache_invalidate;
	enum mset_mode_e	memset_mode;
	ca_uint16_t		mset_cainv_size; /* 64~4KB, should be multiple of FBM_CNT_BURST_SIZE */
#if FBM_RELEASE_MODE
	struct aal_fbm_seg_config_s seg[FBM_MAX_SEG_NUM];
#endif
};

ca_uint32_t aal_fbm_init(void);
ca_uint32_t aal_fbm_pool_init(aal_fbm_pool_id_t id, struct aal_fbm_pool_config_s *pool_config);
ca_uint32_t aal_fbm_buf_push(aal_fbm_cpu_id_t cpu, aal_fbm_pool_id_t id, ca_uint32_t addr);
ca_uint32_t aal_fbm_buf_pop(aal_fbm_cpu_id_t cpu, aal_fbm_pool_id_t id, ca_uint32_t *addr);
#endif /* __AAL_FBM_H__ */
