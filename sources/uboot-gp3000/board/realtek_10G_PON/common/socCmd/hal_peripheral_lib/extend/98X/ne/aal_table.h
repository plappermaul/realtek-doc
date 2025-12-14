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
#ifndef __AAL_TABLE_H__
#define __AAL_TABLE_H__

#include <linux/types.h>
#if 0//yocto
#include <linux/gfp.h>
#include <linux/slab.h>
#else//sd1 uboot for 98f
#endif
#include <linux/spinlock.h>


#include "ca_aal.h"
#include "ca_types.h"

extern uint32_t ca_aal_debug;
extern uint32_t ca_aal_table_sw_cache;

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define SIZE_CHECK_ASSERT(e) enum { ASSERT_CONCAT(ASSERT_AT_LINE_, __LINE__) = 1/(!!(e)) }

/* AAL Return Code */
#define AAL_E_OK			(0)
#define AAL_E_TBLNOTEXIST		(-1)	/* Table does not exist */
#define AAL_E_TBLFULL			(-2)	/* Table is full */
#define AAL_E_ENTRYNOTFOUND		(-3)	/* Entry is not found */
#define AAL_E_ENTRY_USED		(-4)	/* Entry is used */
#define AAL_E_ENTRYNOTRSVD		(-5)	/* Entry is not reserved */
#define AAL_E_DUPLICATED		(-6)	/* Duplicated */
#define AAL_E_OPNOTSUPP			(-7)	/* Operation is not supported */
#define AAL_E_NULLPTR			(-8)	/* Accessing a NULL pointer */
#define AAL_E_OUTRANGE			(-9)	/* Out of range */
#define AAL_E_NOMEM			(-10)	/* No memory */
#define AAL_E_ACCESSINCOMP		(-11)	/* Access is incomplete */
#define AAL_E_INVALID			(-12)	/* Invalid */
#define AAL_E_EXIST			(-13)	/* Entry is existed */

typedef struct aal_table_op_s {
	/*
	 * the combined add API that will first find if a previous entry has
	 * been added to the table. If so, it will just increment the user count
	 * of the matching entry and return this index.  If not, it finds the
	 * first available index, set the entry value and increment the user
	 * count.  The implementation of this function should be an optimized
	 * function that combines (*entry_find), (*entry_refcnt_inc),
	 * (*entry_alloc), and (*entry_set).
	 */
	int (*entry_add) (void *entry, unsigned int *rslt_idx);

	int (*entry_add_by_idx) (void *entry, unsigned int idx);

	/* delete the entry by index. It checks the user count. */
	int (*entry_del) (unsigned int idx);

	/* Set the entry value to a given index in the table.  Must have
	 * valid OP assigned to (*convert_sw_to_hw_data) */
	int (*entry_set) (unsigned int idx, void *entry);

	/*
	 * search the first occurence of entry that matches the given one and
	 * return the index that points to it.
	 */
	int (*entry_get) (void *entry, unsigned int *rslt_idx);

	/*
	 * get the content of the entry of the given index and write it to
	 * the memory given by the user.
	 */
	int (*entry_get_by_idx) (unsigned int idx, void *entry);

	/* flush the whole table */
	int (*table_flush) (void);

	/* Get the remaining available entry count of this table */
	int (*get_avail_count) (void);

	/* print the whole table */
	void (*entry_print) (unsigned int start_idx, int count);
} aal_table_op_t;

typedef struct aal_entry_s {
	void			*data;		// entry content
	atomic_t		refcnt;		// reference count
	unsigned long		local_data;	// private data of this entry
} aal_entry_t;

typedef struct aal_table_s {
	aal_table_e		table_id;
	char			*table_name;
	unsigned int		max_entry;	// total number of entries
	unsigned int		used_entry;
	unsigned int		curr_ptr;
	unsigned int		entry_size;	// the size of each entry in bytes

	unsigned int		access_reg;	// the address of ACCESS register
	unsigned int		data_reg_num;	// the number of DATA registers

	const aal_table_op_t	op;
	spinlock_t		lock;
	aal_entry_t		*entry;		// array of all entries

	/* The successor can overwrite the following functions for
	 * direct access table or combined table.
	 * If the function pointer is null, default functions are used.
	 */
	int (*hw_entry_set) (struct aal_table_s *table, unsigned int idx);
	int (*hw_entry_get) (struct aal_table_s *table, unsigned int idx, void *data);
	int (*hw_entry_clear) (struct aal_table_s *table, unsigned int idx);
} aal_table_t;

/* Register order: from high word to low word or vice versa */
typedef enum {
	AAL_TABLE_DIR_HI_TO_LOW,
	AAL_TABLE_DIR_LOW_TO_HI
} aal_table_direction_t;

typedef struct aal_direct_table_s {
	unsigned int		max_entry;	// total number of entries
	unsigned int		first_reg;	// the address of the first register
	unsigned int		reg_num;	// the number of DATA registers
	unsigned int		reg_stride;	// reg stride may not be reg_num * 4
} aal_direct_table_t;

int hw_entry_clear(aal_table_t *table, unsigned int idx);
int hw_entry_set(aal_table_t *table, unsigned int idx);
int hw_entry_get(aal_table_t *table, unsigned int idx, void *data);
int hw_table_flush(aal_table_t *table);
aal_entry_t *sw_entry_get(aal_table_t *table, unsigned int idx);

/* General internal AAL functions
 * AAL modules could hook these functions if they doesn't need specific handler function.
 */
int generic_entry_add(aal_table_t *table, void *entry, unsigned int *rslt_idx);
int generic_entry_add_by_idx(aal_table_t *table, void *entry, unsigned int idx);
int generic_entry_del(aal_table_t *table, unsigned int idx);
int generic_entry_set(aal_table_t *table, unsigned int idx, void *entry);
int generic_entry_get(aal_table_t *table, void *entry, unsigned int *rslt_idx);
int generic_entry_get_by_idx(aal_table_t *table, unsigned int idx, void *entry);
int generic_table_flush(aal_table_t *table);
int generic_get_avail_count(aal_table_t *table);

/* Table management that controls the registration of each table */
int __aal_table_register(aal_table_e table_id, aal_table_t *table);
int __aal_table_unregister(aal_table_e table_id);
bool __aal_table_is_registered(aal_table_e table_id);
aal_table_t *__aal_table_get(aal_table_e table_id);

ca_status_t aal_direct_table_set(aal_direct_table_t *table, ca_uint16_t entry_idx, void *entry, aal_table_direction_t dir);
ca_status_t aal_direct_table_get(aal_direct_table_t *table, ca_uint16_t entry_idx, void *entry, aal_table_direction_t dir);

#endif /* __AAL_TABLE_H__ */
