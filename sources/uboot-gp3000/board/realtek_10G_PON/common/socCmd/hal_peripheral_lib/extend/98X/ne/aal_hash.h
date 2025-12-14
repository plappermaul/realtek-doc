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
 * aal_hash.h: Hardware Abstraction Layer for Hash Engine to access hardware regsiters
 *
 */
#ifndef __AAL_HASH_H__
#define __AAL_HASH_H__

#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif
#include "aal_table.h"
#include "aal_common.h"
#include "aal_l3fe.h"

/***************************************************************************
 * Hash misc
 ***************************************************************************/

enum hash_ha_width {
	HASH_HA_WIDTH_64 = 0,	/* 64-bit (8-byte)*/
	HASH_HA_WIDTH_128,
	HASH_HA_WIDTH_192,		/* (3*64-bit) */
	HASH_HA_WIDTH_256,
	HASH_HA_WIDTH_384,		/* (3*128-bit) */
	HASH_HA_WIDTH_512,
	HASH_HA_WIDTH_768,		/* (3*256-bit) */
	HASH_HA_WIDTH_1024,
};

enum hash_ht_size {
	HASH_HT_SIZE_512 = 1 << 9,
	HASH_HT_SIZE_1K = 1 << 10,
	HASH_HT_SIZE_2K = 2 * HASH_HT_SIZE_1K,
	HASH_HT_SIZE_4K = 4 * HASH_HT_SIZE_1K,
	HASH_HT_SIZE_8K = 8 * HASH_HT_SIZE_1K,
	HASH_HT_SIZE_16K = 16 * HASH_HT_SIZE_1K,
	HASH_HT_SIZE_32K = 32 * HASH_HT_SIZE_1K,
#ifndef CONFIG_ARCH_CORTINA_G3LITE
	HASH_HT_SIZE_64K = 64 * HASH_HT_SIZE_1K,
#endif /* CONFIG_ARCH_CORTINA_G3LITE */
};

enum hash_hb_size {
	HASH_HB_SIZE_4 = 2, /* slot[1:0]. use 2 bits */
	HASH_HB_SIZE_8,
	HASH_HB_SIZE_16,
	HASH_HB_SIZE_32,
};

enum hash_ct_size {
	HASH_CT_DISABLE = 0,
	HASH_CT_SIZE_256,
	HASH_CT_SIZE_512,
	HASH_CT_SIZE_1K,
#ifndef CONFIG_ARCH_CORTINA_G3LITE
	HASH_CT_SIZE_2K,
#endif /* CONFIG_ARCH_CORTINA_G3LITE */
};

int aal_hash_mem_init(void);
int aal_hash_hash_table_size_set(u32 ht_size, u8 hb_size, u8 ha_width);
int aal_hash_init(void);

/***************************************************************************
 * Hash Profile
 ***************************************************************************/

#define NUM_TUPLE	8	/* how many tuples in a profile */

typedef enum {
	HASH_PROFILE_0 = 0,
	HASH_PROFILE_1,
	HASH_PROFILE_2,
	HASH_PROFILE_3,
	HASH_PROFILE_4,
	HASH_PROFILE_5,
	HASH_PROFILE_6,
	HASH_PROFILE_7,
	HASH_PROFILE_8,
	HASH_PROFILE_9,
	HASH_PROFILE_10,
	HASH_PROFILE_11,
	HASH_PROFILE_12,
	HASH_PROFILE_13,
	HASH_PROFILE_14,
	HASH_PROFILE_15,	// bypass Hash engine
} aal_hash_profile_id_e;

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
		void	*type0_empty_miss;	// hash action for Type-0 empty miss, related action group mask is type0_act_grp_mask_dft
		int	type0_empty_miss_len;	// length of type0_empty_miss in bytes

		unsigned int	type0_act_grp_mask_age_miss;	// hash action group mask for hash action for Type-0 age miss (type0_age_miss)
		void	*type0_age_miss;	// hash action for Type-0 age miss, related action group mask is type0_act_grp_mask_dft
		int	type0_age_miss_len;	// length of type0_age_miss in bytes

		unsigned int	type1_act_grp_mask_empty_miss;	// hash action group mask for hash action for Type-1 empty miss (type1_empty_miss)
		void	*type1_empty_miss;	// hash action for Type-1 empty miss, related action group mask is type1_act_grp_mask_dft
		int	type1_empty_miss_len;	// length of type1_empty_miss in bytes

		unsigned int	type1_act_grp_mask_age_miss;	// hash action group mask for hash action for Type-1 age miss (type1_age_miss)
		void	*type1_age_miss;	// hash action for Type-1 age miss, related action group mask is type1_act_grp_mask_dft
		int	type1_age_miss_len;	// length of type1_age_miss in bytes
	} miss;

	/* hash mask for hash hit
	 * For Type-0 hash hit, related action group mask is type0_act_grp_mask_hit.
	 * For Type-1 hash hit, related action group mask is type1_act_grp_mask_hit.
	 */
	struct {
		aal_hash_tuple_type_e		tuple_type;
		aal_hash_mask_t			*hash_mask;
		unsigned int refcnt;
		ca_uint32_t tuple_wrd;
	} hit[NUM_TUPLE];
} aal_hash_profile_t;

typedef struct {
	ca_uint8_t	dpid			: 6;
	ca_uint8_t	dpid_vld		: 1;
	ca_uint8_t	bypass_next		: 1;
} __attribute__((packed)) aal_hash_dbl_chk_fail_ctrl_t;

int aal_hash_profile_set(unsigned int profile_id, aal_hash_profile_t *profile);

int aal_hash_profile_tuple_add(
	ca_uint32_t profile_id,
	aal_hash_tuple_type_e tuple_type,
	ca_uint8_t priority,
	ca_uint8_t mask_id,
	ca_uint8_t *tuple_idx);

int aal_hash_profile_tuple_del(
	ca_uint32_t profile_id,
	ca_uint8_t tuple_idx);

/***************************************************************************
 * Hash Aging
 ***************************************************************************/

/* The size of register AGING_GRANULARITY is 30 bits; its max value is 0x3FFFFFFF.
 * It decreases one every 4096 clocks.
 * When the register reaches zero, the age of all hash entries deceases one.
 *
 * The valid range of age for each hash entry is 0 to 7.
 * 7 means static entry, no aging. 0 means aged out.
 *
 * Therefore, the max aging clocks are (0x3FFFFFFF * 4096 * 6).
 */

typedef enum {
	HASH_AGING_0 = 0, /* the entry is invalid */
	HASH_AGING_1,
	HASH_AGING_2,
	HASH_AGING_3,
	HASH_AGING_4,
	HASH_AGING_5,
	HASH_AGING_6, /* 1~6: the entry is valid and might be aged-out. */
	HASH_AGING_7, /* the entry is valid, and is a static entry that will not be aged-out. */

	HASH_AGING_INVALID = HASH_AGING_0,
	HASH_AGING_START = HASH_AGING_6,
	HASH_AGING_STATIC = HASH_AGING_7,
} aal_hash_aging_e;

#define HASH_AGING_UNIT		4096		/* clocks; unit of granularity */
#define HASH_INIT_AGE_TIME	1500		/* seconds */

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
} __attribute__((packed)) aal_hash_age_t;

typedef volatile struct {
	aal_hash_age_t age0;
	aal_hash_age_t age1;
	aal_hash_age_t age2;
	aal_hash_age_t age3;
} __attribute__((packed)) aal_hash_age_tbl_t;

/*************************************************************************
 * overflow hash value
 *************************************************************************/

typedef volatile struct {
	ca_uint32_t	crc32		: 32;
	ca_uint32_t	crc16		: 16;
	ca_uint32_t	profile		: 4;
	ca_uint32_t	reserved	: 12;
} __attribute__((packed)) aal_overflow_hash_t;

int aal_hash_aging_timer_set(unsigned int time);
int aal_hash_aging_timer_get(unsigned int *time);
int aal_hash_current_granularity_get(unsigned int *granularity);
int aal_hash_age_set(aal_table_e table_id, unsigned int idx, unsigned int age);
int aal_hash_age_get(aal_table_e table_id, unsigned int idx, unsigned int *age, unsigned int *time);

/***************************************************************************
 * AAL table, L3FE_FE_L3E_HS_MASK_MEM_ACCESS for Mask Table
 ***************************************************************************/

#define HASH_MASK_TBL_ENTRY_MAX	64

extern unsigned int hash_mask_nat_idx;
extern unsigned int hash_mask_multicast_idx;

int _hash_mask_tbl_init(void);
void _hash_mask_tbl_exit(void);
int aal_mask_tbl_entry_add(aal_hash_mask_t *entry, unsigned int *rslt_idx);
int aal_mask_tbl_entry_del(unsigned int rslt_idx);

ca_status_t aal_mask_tbl_entry_add_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	aal_hash_mask_t *entry,
CA_OUT	ca_uint32_t *rslt_idx);

ca_status_t aal_mask_tbl_entry_del_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t rslt_idx);

/***************************************************************************
 * AAL table: Hash Table
 ***************************************************************************/

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define HASH_HASH_TBL_ENTRY_MAX		32768
#define HASH_HASH_CACHE_ENTRY_MAX		1024
#else
#define HASH_HASH_TBL_ENTRY_MAX		65536
#define HASH_HASH_CACHE_ENTRY_MAX		2048
#endif /* CONFIG_ARCH_CORTINA_G3LITE */

int _hash_tbl_init(void);
void _hash_tbl_exit(void);

int _cache_hash_table_init(void);
void _cache_hash_table_exit(void);

int _cache_hash_action_table_init(void);
void _cache_hash_action_table_exit(void);

int _hash_age_table_init(void);
void _hash_age_table_exit(void);

/***************************************************************************
 * AAL table, L3FE_FE_L3E_HS_OVERFLOW_HASH_ACCESS for Overflow Table
 ***************************************************************************/

#define HASH_OVERFLOW_TBL_ENTRY_MAX	64

int _hash_overflow_tbl_init(void);
void _hash_overflow_tbl_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_IND_X for Hash Action
 ***************************************************************************/

#define HASH_HASH_ACTION_ENTRY_MAX	HASH_HASH_TBL_ENTRY_MAX
#define HASH_HASH_IDX_OVERFLOW_FLAG	0x10000		/* indicate the entry is in overflow table */
#define HASH_HASH_IDX_MASK			0xFFFF		/* mask for hash index for hash table and overflow table */


typedef struct hash_hash_action_entry_s {
	/* No matter CFG.HA_WIDTH is, 1024 bits are always available. */
	ca_uint64_t	reserved_0;
	ca_uint64_t	reserved_1;
	ca_uint64_t	reserved_2;
	ca_uint64_t	reserved_3;
	ca_uint64_t	reserved_4;
	ca_uint64_t	reserved_5;
	ca_uint64_t	reserved_6;
	ca_uint64_t	reserved_7;
} __attribute__((packed))hash_hash_action_entry_t;

int _hash_action_init(void);
void _hash_action_exit(void);

/***************************************************************************
 * AAL table: Overflow Action
 ***************************************************************************/

#define HASH_OVERFLOW_ACTION_ENTRY_MAX	64

typedef struct hash_overflow_action_entry_s {
	ca_uint64_t	reserved_0;
	ca_uint64_t	reserved_1;
	ca_uint64_t	reserved_2;
	ca_uint64_t	reserved_3;
	ca_uint64_t    reserved_4;
	ca_uint64_t    reserved_5;
	ca_uint64_t    reserved_6;
	ca_uint64_t    reserved_7;
} __attribute__((packed)) hash_overflow_action_entry_t;

int _hash_overflow_action_init(void);
void _hash_overflow_action_exit(void);

/***************************************************************************
 * AAL table, FE_L3E_HS_IND_X for Default Action
 ***************************************************************************/

#define HASH_DEFAULT_ACTION_ENTRY_MAX	32

typedef struct hash_default_action_entry_s {
	ca_uint64_t	reserved_0;
	ca_uint64_t	reserved_1;
	ca_uint64_t	reserved_2;
	ca_uint64_t	reserved_3;
	ca_uint64_t 	reserved_4;
	ca_uint64_t 	reserved_5;
	ca_uint64_t 	reserved_6;
	ca_uint64_t 	reserved_7;
} __attribute__((packed)) hash_default_action_entry_t;

int _hash_default_action_init(void);
void _hash_default_action_exit(void);

/***************************************************************************
 * Hash Action
 ***************************************************************************/
typedef struct hash_action_normal_s {
	HASH_ACTION_GROUP_01;
	HASH_ACTION_GROUP_02;
	HASH_ACTION_GROUP_03;
	HASH_ACTION_GROUP_04;
	HASH_ACTION_GROUP_05;
	HASH_ACTION_GROUP_06;
	HASH_ACTION_GROUP_18;
}__attribute__((packed)) hash_action_normal_t;

typedef struct hash_action_to_lan_s {
	HASH_ACTION_GROUP_01;
	HASH_ACTION_GROUP_02;
	HASH_ACTION_GROUP_03;
	HASH_ACTION_GROUP_04;
	HASH_ACTION_GROUP_05;
	HASH_ACTION_GROUP_08;
	HASH_ACTION_GROUP_09;
	HASH_ACTION_GROUP_17;
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
}__attribute__((packed)) hash_action_to_lan_t;

typedef struct hash_action_to_wan_s {
	HASH_ACTION_GROUP_01;
	HASH_ACTION_GROUP_02;
	HASH_ACTION_GROUP_03;
	HASH_ACTION_GROUP_05;
	HASH_ACTION_GROUP_06;
//marklin: add for e8C (+)
	HASH_ACTION_GROUP_09;
//marklin: add for e8C (-)
	HASH_ACTION_GROUP_17;
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
}__attribute__((packed)) hash_action_to_wan_t;

typedef struct hash_action_type1_cls_hash_s {
	HASH_ACTION_GROUP_08;
	HASH_ACTION_GROUP_09;
	HASH_ACTION_GROUP_11;
	HASH_ACTION_GROUP_12;
	HASH_ACTION_GROUP_13;
	HASH_ACTION_GROUP_14;
	HASH_ACTION_GROUP_15;
	HASH_ACTION_GROUP_16;
	HASH_ACTION_GROUP_17;
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
}__attribute__((packed)) hash_action_type1_cls_hash_t;

typedef struct hash_action_flow_s {
	HASH_ACTION_GROUP_01;
	HASH_ACTION_GROUP_02;
	HASH_ACTION_GROUP_03;
	HASH_ACTION_GROUP_05;
	HASH_ACTION_GROUP_07;
	HASH_ACTION_GROUP_08;
	HASH_ACTION_GROUP_09;
	HASH_ACTION_GROUP_10;
	HASH_ACTION_GROUP_11;
	HASH_ACTION_GROUP_12;
	HASH_ACTION_GROUP_13;
	HASH_ACTION_GROUP_14;
	HASH_ACTION_GROUP_15;
	HASH_ACTION_GROUP_16;
	HASH_ACTION_GROUP_17;
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
} __attribute__((packed)) hash_action_flow_t;

typedef struct hash_default_action_s {
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
}__attribute__((packed)) hash_default_action_t;


typedef struct aal_cache_hash_s {
	ca_uint32_t crc32		: 32;
	ca_uint32_t crc16		: 13 ; /* bits 12:0 */
	ca_uint32_t slot		:  5 ; /* bits 17:13 */
	ca_uint32_t location	:  1 ; /* bits 18:18 */
	ca_uint32_t pri		:  2 ; /* bits 20:19 */
	ca_uint32_t age		:  2 ; /* bits 22:21 */
	ca_uint32_t rsrvd1	:  9 ;
} __attribute__((packed)) aal_cache_hash_t;

ca_status_t aal_hash_add (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	aal_hash_key_t		*hash_key,
	CA_IN	aal_hash_action_t	*hash_action,
	CA_IN	aal_hash_tuple_type_e	hash_type,
	CA_IN	aal_hash_aging_e		hash_age,
	CA_IN	ca_uint32_t		mask_id,
	CA_IN	ca_uint32_t		profile_id,
	CA_OUT	ca_uint32_t		*hash_idx);

ca_status_t aal_hash_delete (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		hash_idx);

ca_status_t aal_default_action_set(
	CA_IN	ca_device_id_t	device_id,
	CA_IN	ca_uint32_t profile_id, 
	CA_IN	ca_uint32_t miss_to_cpu);

ca_status_t aal_hash_cache_count_get(
	CA_IN	ca_device_id_t	device_id,
	CA_OUT	ca_uint32_t	*ut_cnt);

ca_status_t aal_hash_crc_hw_offload (
	CA_IN	ca_device_id_t	device_id,
	CA_IN	L3FE_HDR_I_t	*hdr_i,
	CA_IN	ca_uint32_t	mask_id,
	CA_OUT	ca_uint32_t	*crc32,
	CA_OUT	ca_uint16_t	*crc16);

int aal_hash_cache_invalidate(unsigned int hash_idx, unsigned int hash_crc16);
int aal_hash_cache_allocation(
	unsigned int hash_idx,
	unsigned int hash_crc16,
	unsigned char loc,
	unsigned char age,
	unsigned char priority);

int aal_hashtbl_hash_key_set(unsigned int hash_idx, aal_hash_key_t *hash_key);
int aal_hashtbl_hash_key_get(unsigned int hash_idx, aal_hash_key_t **hash_key);

#endif /* __AAL_HASH_H__ */

