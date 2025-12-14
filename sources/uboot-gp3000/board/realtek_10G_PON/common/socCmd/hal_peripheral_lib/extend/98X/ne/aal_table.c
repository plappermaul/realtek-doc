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
 * aal_table.c
 *
 * Generic resource management implementation for all HW Tables
 */

#include "ca_ne_autoconf.h"
#include <asm/io.h>
#include "ca_types.h"
#include "aal_table.h"
#include "aal_common.h"
#include "ca_mut.h"

#define TABLE_TRY_TIMEOUT	1000

static aal_table_t *ca_table_info[AAL_TABLE_MAX];

uint32_t ca_aal_debug = 0x0;		// AAL debug flag
uint32_t ca_aal_table_sw_cache = 0;	// Dump AAL table from cached software table or not

/*************************************************************************
 * internal AAL function for hardware register
 * This should be static because only general functions need them (?)
 *************************************************************************/

/* Clear the entry with given index and table */
int hw_entry_clear(aal_table_t *table, unsigned int idx)
{
	int i;
	uint32_t val = 0;
	ca_device_id_t device_id;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;
	if (idx >= table->max_entry) return AAL_E_OUTRANGE;

	device_id = (table->table_id < __AAL_TABLE_8279_START__) ? 0 : 1;

	/* write the value to the specific data registers */
	for (i = 0; i < table->data_reg_num ; i++)
		ca_ne_reg_write(device_id, 0, table->access_reg + ((table->data_reg_num - i) << 2));

	/* start the write access (prepare access register) */
	val |= 1 << 30;		/* set rbw to write */
	val |= 1 << 31;		/* set access to start */
	val |= idx;
	ca_ne_reg_write(device_id, val, table->access_reg);

	/* wait write complete */
	i = 0;
	do {
		val = ca_ne_reg_read(device_id, table->access_reg);
	} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
	if (i >= TABLE_TRY_TIMEOUT)
		return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

	return AAL_E_OK;
} /* hw_entry_clear */

/* Set the value to HW table entry with given index and table type */
int hw_entry_set(aal_table_t *table, unsigned int idx)
{
	int i;
	uint32_t val = 0;
	unsigned int *ptr;
	ca_device_id_t device_id;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;
	if (idx >= table->max_entry) return AAL_E_OUTRANGE;
	if (table->entry[idx].data == NULL) return AAL_E_NULLPTR;

	device_id = (table->table_id < __AAL_TABLE_8279_START__) ? 0 : 1;

	ptr = table->entry[idx].data;
	/* write the value to the specific data registers */
	for (i = 0; i < table->data_reg_num ; i++)
		ca_ne_reg_write(device_id, ptr[i], table->access_reg + ((table->data_reg_num - i) << 2));

	/* start the write access (prepare access register) */
	val |= 1 << 30;		/* set rbw to write */
	val |= 1 << 31;		/* set access to start */
	val |= idx;
	ca_ne_reg_write(device_id, val, table->access_reg);

	/* wait write complete */
	i = 0;
	do {
		val = ca_ne_reg_read(device_id, table->access_reg);
	} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
	if (i >= TABLE_TRY_TIMEOUT)
		return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

	return AAL_E_OK;
} /* hw_entry_set */

/* Get the value from HW table entry with given index and table type */
int hw_entry_get(aal_table_t *table, unsigned int idx, void *data)
{
	int i;
	uint32_t val = 0;
	unsigned int *ptr;
	ca_device_id_t device_id;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;
	if (idx >= table->max_entry) return AAL_E_OUTRANGE;
	if (data == NULL) return AAL_E_NULLPTR;

	device_id = (table->table_id < __AAL_TABLE_8279_START__) ? 0 : 1;

	ptr = data;

	/* start the write access (prepare access register) */
	val |= 0 << 30;		/* set rbw to read */
	val |= 1 << 31;		/* set access to start */
	val |= idx;
	ca_ne_reg_write(device_id, val, table->access_reg);

	/* wait write complete */
	i = 0;
	do {
		val = ca_ne_reg_read(device_id, table->access_reg);
	} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
	if (i >= TABLE_TRY_TIMEOUT)
		return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

	for (i = 0; i < table->data_reg_num; i++)
		ptr[i] = ca_ne_reg_read(device_id, table->access_reg + ((table->data_reg_num - i) << 2));

	return AAL_E_OK;
}

int hw_table_flush(aal_table_t *table)
{
	int i, ret;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;

	for (i = 0; i < table->max_entry; i++) {
		ret = hw_entry_clear(table, i);
		if (ret != AAL_E_OK) return ret;
	}

	return AAL_E_OK;
} /* hw_table_flush */

/*************************************************************************
 * internal AAL function for software table
 * called/hooked by other AAL modules if they don't need specific ones
 *************************************************************************/

inline aal_entry_t *sw_entry_get(aal_table_t *table, unsigned int idx)
{
	if (table->entry == NULL) return NULL;
	if (table->entry[idx].data == NULL) return NULL;
	return &table->entry[idx];

} /* cs_table_get_entry */

/*************************************************************************
 * internal AAL function for general table entry operation
 * called/hooked by other AAL modules if they don't need specific ones
 *************************************************************************/

int generic_entry_add(aal_table_t *table, void *entry, unsigned int *rslt_idx)
{
	unsigned long flags;
	bool f_found = false;
	int ret;

	if ((entry == NULL) || (rslt_idx == NULL))
		return AAL_E_NULLPTR;

	if (table->op.entry_get == NULL)
		return AAL_E_OPNOTSUPP;

	ret = table->op.entry_get(entry, rslt_idx);
	if (ret == AAL_E_OK) {
		/* already existed */
		atomic_inc(&table->entry[*rslt_idx].refcnt);
		return AAL_E_OK;
	} else if (ret == AAL_E_ENTRYNOTFOUND) {

		if (table->max_entry == table->used_entry)
			return AAL_E_TBLFULL;

		spin_lock_irqsave(&table->lock, flags);

		*rslt_idx = 0;
		do {
			if (*rslt_idx == table->max_entry) {
				spin_unlock_irqrestore(&table->lock, flags);
				return AAL_E_TBLFULL;
			}

			/* search an empty entry */

			if (table->entry[*rslt_idx].data == NULL) {
				table->entry[*rslt_idx].data = ca_zalloc_trace(table->entry_size, GFP_ATOMIC, true);
				if (table->entry[*rslt_idx].data == NULL) {
					spin_unlock_irqrestore(&table->lock, flags);
					return AAL_E_NOMEM;
				}

				memcpy(table->entry[*rslt_idx].data, entry, table->entry_size);
				atomic_set(&table->entry[*rslt_idx].refcnt, 1);
				table->used_entry++;

				/* set to HW */
				if (table->hw_entry_set != NULL)
					ret = table->hw_entry_set(table, *rslt_idx);
				else
					ret = hw_entry_set(table, *rslt_idx);
				break;
			}

			(*rslt_idx)++;

		} while (f_found == false);

		spin_unlock_irqrestore(&table->lock, flags);
	}
	return ret;
} /* generic_entry_add */

int generic_entry_add_by_idx(aal_table_t *table, void *entry, unsigned int idx)
{
	unsigned long flags;
	int ret;
	void *tmp_entry;

	if (entry == NULL)
		return AAL_E_NULLPTR;

	if (idx >= table->max_entry)
		return AAL_E_OUTRANGE;

	if (table->op.entry_get_by_idx == NULL)
		return AAL_E_OPNOTSUPP;

	tmp_entry = ca_zalloc_trace(table->entry_size, GFP_ATOMIC, true);

	ret = table->op.entry_get_by_idx(idx, tmp_entry);
	if (ret == AAL_E_OK) {
		/* there is an entry at same index */

		if (memcmp(entry, tmp_entry, table->entry_size) == 0) {
			/* same entry, increase the reference count */
			atomic_inc(&table->entry[idx].refcnt);
			ca_free_trace(tmp_entry);
		} else {
			/* different entry, must keep original */
			ca_free_trace(tmp_entry);
			return AAL_E_ENTRY_USED;
		}

	} else {
		/* entry at index is not used */
		ca_free_trace(tmp_entry);

		spin_lock_irqsave(&table->lock, flags);

		table->entry[idx].data = ca_zalloc_trace(table->entry_size, GFP_ATOMIC, true);
		if (table->entry[idx].data == NULL) {
			spin_unlock_irqrestore(&table->lock, flags);
			return AAL_E_NOMEM;
		}

		memcpy(table->entry[idx].data, entry, table->entry_size);
		atomic_set(&table->entry[idx].refcnt, 1);
		table->used_entry++;

		/* set to HW */
		if (table->hw_entry_set != NULL)
			ret = table->hw_entry_set(table, idx);
		else
			ret = hw_entry_set(table, idx);

		spin_unlock_irqrestore(&table->lock, flags);

	}
	return AAL_E_OK;
} /* generic_entry_add_by_idx */

int generic_entry_del(aal_table_t *table, unsigned int idx)
{
	aal_entry_t *p_entry;
	unsigned long flags;
	int ret;

	if (idx >= table->max_entry)
		return AAL_E_OUTRANGE;

	p_entry = sw_entry_get(table, idx);
	if (p_entry == NULL)
		return AAL_E_NULLPTR;

	spin_lock_irqsave(&table->lock, flags);

	if (atomic_dec_and_test(&p_entry->refcnt)) {
		if (table->hw_entry_clear != NULL)
			ret = table->hw_entry_clear(table, idx);
		else
			ret = hw_entry_clear(table, idx);

		ca_free_trace(p_entry->data);
		p_entry->data = NULL;
		table->used_entry--;

		if (ret != AAL_E_OK) {
			spin_unlock_irqrestore(&table->lock, flags);
			return ret;
		}
	}

	spin_unlock_irqrestore(&table->lock, flags);

	return AAL_E_OK;
} /* generic_entry_del */

int generic_entry_set(aal_table_t *table, unsigned int idx, void *entry)
{
	unsigned long flags;
	int ret;

	if (entry == NULL)
		return AAL_E_NULLPTR;
	if (idx >= table->max_entry)
		return AAL_E_OUTRANGE;
	if (table->entry[idx].data == NULL)
		return AAL_E_NULLPTR;

	spin_lock_irqsave(&table->lock, flags);

	memcpy(table->entry[idx].data, entry, table->entry_size);

	/* set to HW */
	if (table->hw_entry_set != NULL)
		ret = table->hw_entry_set(table, idx);
	else
		ret = hw_entry_set(table, idx);

	if (ret != AAL_E_OK) {
		spin_unlock_irqrestore(&table->lock, flags);
		return ret;
	}

	spin_unlock_irqrestore(&table->lock, flags);

	return ret;
} /* generic_entry_set */

int generic_entry_get(aal_table_t *table, void *entry, unsigned int *rslt_idx)
{
	aal_entry_t *p_entry;
	unsigned int i, remaining_used;

	if ((entry == NULL) || (rslt_idx == NULL))
		return AAL_E_NULLPTR;

	remaining_used = table->used_entry;

	for (i = 0; i < table->max_entry; i++) {
		if (remaining_used == 0) return AAL_E_ENTRYNOTFOUND;

		p_entry = sw_entry_get(table, i);
		if (p_entry != NULL) {
			remaining_used--;

			if (!memcmp(p_entry->data, entry, table->entry_size)) {
				*rslt_idx = i;
				return AAL_E_OK;
			}
		}
	}

	return AAL_E_ENTRYNOTFOUND;
} /* generic_entry_get */

int generic_entry_get_by_idx(aal_table_t *table, unsigned int idx, void *entry)
{
	unsigned long flags;
	aal_entry_t *p_entry;

	if (entry == NULL)
		return AAL_E_NOMEM;
	if (idx >= table->max_entry)
		return AAL_E_OUTRANGE;

	p_entry = sw_entry_get(table, idx);
	if (p_entry == NULL)
		return AAL_E_NULLPTR;

	spin_lock_irqsave(&table->lock, flags);
	memcpy(entry, p_entry->data, table->entry_size);
	spin_unlock_irqrestore(&table->lock, flags);

	return AAL_E_OK;
} /* generic_entry_get_by_idx */

int generic_table_flush(aal_table_t *table)
{
	int i;
	unsigned long flags;

	if (table->op.entry_del == NULL)
		return AAL_E_OPNOTSUPP;

	for (i = 0; i < table->max_entry; i++)
		table->op.entry_del(i);

	spin_lock_irqsave(&table->lock, flags);
	table->used_entry = 0;
	table->curr_ptr = 0;
	// hw_table_flush(table); // TBD: Test the performance on real machine then decide to mark or not.
	spin_unlock_irqrestore(&table->lock, flags);

	return AAL_E_OK;
} /* generic_table_flush */

int generic_get_avail_count(aal_table_t *table)
{
	return (table->max_entry - table->used_entry);
} /* generic_get_avail_count */

/*************************************************************************
 * internal AAL function for table operation
 *************************************************************************/

int __aal_table_register(aal_table_e table_id, aal_table_t *table)
{
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        if (table_id >= AAL_TABLE_MAX){
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
                return AAL_E_OUTRANGE;
        }

        if (ca_table_info[table_id] != NULL){
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
                return AAL_E_DUPLICATED;
        }

        table->entry = ca_zalloc_trace(sizeof(aal_entry_t) * table->max_entry, GFP_ATOMIC, true);
        if (table->entry == NULL){
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
                return AAL_E_NOMEM;
        }

        ca_table_info[table_id] = table;

        return AAL_E_OK;
}

int __aal_table_unregister(aal_table_e table_id)
{
	aal_table_t *table;

	if (table_id >= AAL_TABLE_MAX)
		return AAL_E_OUTRANGE;

	table = __aal_table_get(table_id);
	if (table == NULL)
		return AAL_E_TBLNOTEXIST;

	if (table->op.table_flush != NULL)
		table->op.table_flush();

	ca_free_trace(table->entry);

	ca_table_info[table_id] = NULL;

	return AAL_E_OK;
}

bool __aal_table_is_registered(aal_table_e table_id)
{
	if (table_id >= AAL_TABLE_MAX)
		return false;

	if (ca_table_info[table_id] == NULL)
		return false;

	return true;
}

aal_table_t *__aal_table_get(aal_table_e table_id)
{
	if (table_id >= AAL_TABLE_MAX)
		return NULL;

	return ca_table_info[table_id];
}

ca_status_t aal_direct_table_get(aal_direct_table_t *table, ca_uint16_t entry_idx, void *entry, aal_table_direction_t dir)
{
	ca_uint32_t i;
	ca_uint32_t *ptr;

	if (table == NULL || entry == NULL) return AAL_E_NULLPTR;
	if (entry_idx >= table->max_entry) return AAL_E_OUTRANGE;

	ptr = entry;

	for (i = 0; i < table->reg_num; i++){
		if (dir == AAL_TABLE_DIR_HI_TO_LOW)
			ptr[i] = ca_ne_reg_read(0, table->first_reg + table->reg_stride * entry_idx + (table->reg_num - i - 1) * sizeof(ca_uint32_t));
		else
			ptr[i] = ca_ne_reg_read(0, table->first_reg + table->reg_stride * entry_idx + i * sizeof(ca_uint32_t));
	}

	return AAL_E_OK;
}

ca_status_t aal_direct_table_set(aal_direct_table_t *table, ca_uint16_t entry_idx, void *entry, aal_table_direction_t dir)
{
	ca_uint32_t i;
	ca_uint32_t *ptr;

	if (table == NULL || entry == NULL) return AAL_E_NULLPTR;
	if (entry_idx >= table->max_entry) return AAL_E_OUTRANGE;

	ptr = entry;

	for (i = 0; i < table->reg_num; i++){
		if (dir == AAL_TABLE_DIR_HI_TO_LOW)
			ca_ne_reg_write(0, ptr[i], table->first_reg + table->reg_stride * entry_idx + (table->reg_num - i - 1) * sizeof(ca_uint32_t));
		else
			ca_ne_reg_write(0, ptr[i], table->first_reg + table->reg_stride * entry_idx + i * sizeof(ca_uint32_t));
	}

	return AAL_E_OK;
}

