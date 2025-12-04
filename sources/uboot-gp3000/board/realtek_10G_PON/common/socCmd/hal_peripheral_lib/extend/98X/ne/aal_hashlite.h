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
 * aal_hashlite.h: Hardware Abstraction Layer for Hash-Lite Engine to access hardware regsiters
 *
 */
#ifndef __AAL_HASHLITE_H__
#define __AAL_HASHLITE_H__

#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif
#include "aal_table.h"
#include "aal_common.h"
#include "aal_hashlite_def.h"
#include "aal_hashlite_init.h"

/***************************************************************************
 * HashLite misc
 ***************************************************************************/

#ifdef CONFIG_ARCH_CORTINA_G3LITE
enum hashlite_ht_size {
	HASHLITE_HT_SIZE_256 = 0,	/* 256 hash entries, action of a entry is 192 bits */
};
#else
enum hashlite_ht_size {
	HASHLITE_HT_SIZE_1K = 0,	/* 1K hash entries, action of a entry is 384 bits */
	HASHLITE_HT_SIZE_2K,		/* 2K hash entries, action of a entry is 192 bits */
};
#endif

int aal_hashlite_hash_table_size_set(unsigned int ht_size);
int aal_hashlite_mem_init(void);

/***************************************************************************
 * HashLite Profile
 ***************************************************************************/

#define NUM_TUPLE	8	/* how many tuples in a profile */

typedef enum {
	HASHLITE_PROFILE_0 = 0,
	HASHLITE_PROFILE_1,
	HASHLITE_PROFILE_2,
	HASHLITE_PROFILE_3,
	HASHLITE_PROFILE_4,
	HASHLITE_PROFILE_5,
	HASHLITE_PROFILE_6,
	HASHLITE_PROFILE_7,
	HASHLITE_PROFILE_8,
	HASHLITE_PROFILE_9,
	HASHLITE_PROFILE_10,
	HASHLITE_PROFILE_11,
	HASHLITE_PROFILE_12,
	HASHLITE_PROFILE_13,
	HASHLITE_PROFILE_14,	// bypass hash calculation & search, use hash action directly
	HASHLITE_PROFILE_15,	// bypass HashLite engine
} aal_hashlite_profile_id_e;

typedef struct {
	/* bitmask of action group */
	unsigned int	type0_act_grp_mask_hit; // hash action group mask, use when Type-0 action hit
	unsigned int	type1_act_grp_mask_hit; // hash action group mask, use when Type-1 action hit

	/* hash action when hash miss
	 *   empty miss: no matching hash value (CRC32)
	 *   age miss:   hash value (CRC32) matches but age is 0
	 */
	struct {
		unsigned int	type0_act_grp_mask_empty_miss;	// hash action group mask for hash action for Type-0 empty miss (type0_empty_miss)
		void		*type0_empty_miss;		// hash action for Type-0 empty miss
		int		type0_empty_miss_len;		// length of type0_empty_miss in bytes

		unsigned int	type0_act_grp_mask_age_miss;	// hash action group mask for hash action for Type-0 age miss (type0_age_miss)
		void		*type0_age_miss;		// hash action for Type-0 age miss
		int		type0_age_miss_len;		// length of type0_age_miss in bytes

		unsigned int	type1_act_grp_mask_empty_miss;	// hash action group mask for hash action for Type-1 empty miss (type1_empty_miss)
		void		*type1_empty_miss;		// hash action for Type-1 empty miss
		int		type1_empty_miss_len;		// length of type1_empty_miss in bytes

		unsigned int	type1_act_grp_mask_age_miss;	// hash action group mask for hash action for Type-1 age miss (type1_age_miss)
		void		*type1_age_miss;		// hash action for Type-1 age miss
		int		type1_age_miss_len;		// length of type1_age_miss in bytes
	} miss;

	/* hash mask for hash hit
	 * For Type-0 hash hit, related action group mask is type0_act_grp_mask_hit.
	 * For Type-1 hash hit, related action group mask is type1_act_grp_mask_hit.
	 */
	struct {
		aal_hash_tuple_type_e		tuple_type;
		aal_hash_mask_t			*hash_mask;
	} hit[NUM_TUPLE];
} aal_hashlite_profile_t;

int aal_hashlite_profile_set(ca_device_id_t device_id, unsigned int profile_id, aal_hashlite_profile_t *profile);
int aal_hashlite_profile_14_set(ca_device_id_t device_id, unsigned int act_grp_mask);

/***************************************************************************
 * HashLite Aging
 ***************************************************************************/

/* The size of register AGING_GRANULARITY is 30 bits; its max value is 0x3FFFFFFF.
 * It decreases one every 4096 clocks.
 * When the register reaches zero, the age of all hash entries deceases one.
 *
 * The valid range of age for each hash entry is 0 to 15.
 * 15 means static entry, no aging. 0 means aged out.
 *
 * Therefore, the max aging clocks are (0x3FFFFFFF * 4096 * 14).
 */

typedef enum {
	HASHLITE_AGING_0 = 0,
	HASHLITE_AGING_1,
	HASHLITE_AGING_2,
	HASHLITE_AGING_3,
	HASHLITE_AGING_4,
	HASHLITE_AGING_5,
	HASHLITE_AGING_6,
	HASHLITE_AGING_7,
	HASHLITE_AGING_8,
	HASHLITE_AGING_9,
	HASHLITE_AGING_10,
	HASHLITE_AGING_11,
	HASHLITE_AGING_12,
	HASHLITE_AGING_13,
	HASHLITE_AGING_14,
	HASHLITE_AGING_15,

	HASHLITE_AGING_INVALID = HASHLITE_AGING_0,
	HASHLITE_AGING_START = HASHLITE_AGING_14,
	HASHLITE_AGING_STATIC = HASHLITE_AGING_15,
} aal_hashlite_aging_e;

#define HASHLITE_AGING_UNIT		4096		/* clocks; unit of granularity */

typedef volatile union {
	struct {
		ca_uint32_t	slot_0 : 4;
		ca_uint32_t	slot_1 : 4;
		ca_uint32_t	slot_2 : 4;
		ca_uint32_t	slot_3 : 4;
		ca_uint32_t	slot_4 : 4;
		ca_uint32_t	slot_5 : 4;
		ca_uint32_t	slot_6 : 4;
		ca_uint32_t	slot_7 : 4;
	} bf;
	ca_uint32_t	wrd;
} __attribute__((packed)) aal_hashlite_age_t;

int aal_hashlite_aging_timer_set(unsigned int time);
int aal_hashlite_aging_timer_get(unsigned int *time);
int aal_hashlite_granularity_get(unsigned int *granularity);
int aal_hashlite_age_set(aal_table_e table_id, unsigned int idx, unsigned int age);
int aal_hashlite_age_get(aal_table_e table_id, unsigned int idx, unsigned int *age, unsigned int *time);

/***************************************************************************
 * AAL table, FE_L3E_HS_LIGHT_IND_X for Mask Table
 ***************************************************************************/

#define HASHLITE_MASK_TBL_ENTRY_MAX	32

int __hashlite_mask_tbl_init(void);
void __hashlite_mask_tbl_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_LIGHT_IND_X for Hash Table
 ***************************************************************************/

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define HASHLITE_HASH_TBL_ENTRY_MAX	256
#else
#define HASHLITE_HASH_TBL_ENTRY_MAX	2048
#endif

int __hashlite_hash_tbl_init(void);
void __hashlite_hash_tbl_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_LIGHT_IND_X for Overflow Table
 ***************************************************************************/

#define HASHLITE_OVERFLOW_TBL_ENTRY_MAX	32

int __hashlite_overflow_tbl_init(void);
void __hashlite_overflow_tbl_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_LIGHT_IND_X for Hash Action
 ***************************************************************************/

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define HASHLITE_HASH_ACTION_ENTRY_MAX	256
#else
#define HASHLITE_HASH_ACTION_ENTRY_MAX	2048
#endif

int __hashlite_hash_action_init(void);
void __hashlite_hash_action_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_LIGHT_IND_X for Overflow Action
 ***************************************************************************/

#define HASHLITE_OVERFLOW_ACTION_ENTRY_MAX	32

int __hashlite_overflow_action_init(void);
void __hashlite_overflow_action_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_LIGHT_IND_X for Default Action
 ***************************************************************************/

#define HASHLITE_DEFAULT_ACTION_ENTRY_MAX	16

int __hashlite_default_action_init(void);
void __hashlite_default_action_exit(void);

/***************************************************************************
 * HashLite wrap functions
 ***************************************************************************/

int aal_hashlite_hash_add(
	ca_device_id_t device_id,
	aal_hash_key_t *hash_key, aal_hash_mask_t *hash_mask,
	aal_hashlite_hash_action_entry_t *hash_action, unsigned int actgrp_mask,
	aal_hashlite_aging_e aging, unsigned int *rslt_hash_idx);
int aal_hashlite_hash_delete(ca_device_id_t device_id, unsigned int hash_idx);
int aal_hashlite_hash_key_set(ca_device_id_t device_id, unsigned int hash_idx, aal_hash_key_t *hash_key);
int aal_hashlite_hash_key_get(ca_device_id_t device_id, unsigned int hash_idx, aal_hash_key_t **hash_key);

/***************************************************************************
 * Hash-Lite Engine Initialization
 ***************************************************************************/

int aal_hashlite_init(void);

#endif /* __AAL_HASHLITE_H__ */

