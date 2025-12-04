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
#ifndef __CA_AAL_H__
#define __CA_AAL_H__

#include <asm/io.h>
#include "ca_types.h"
#include "osal_cmn.h"
#include "aal_init.h"

/* FE Table Enumeration Definition */
typedef enum {
	/* 0 */
	AAL_TABLE_LPM,
	AAL_TABLE_HASHLITE_MASK_TBL,
	AAL_TABLE_HASHLITE_HASH_TBL,
	AAL_TABLE_HASHLITE_OVERFLOW_TBL,
	AAL_TABLE_HASHLITE_HASH_ACTION,

	/* 5 */
	AAL_TABLE_HASHLITE_OVERFLOW_ACTION,
	AAL_TABLE_HASHLITE_DEFAULT_ACTION,
	AAL_TABLE_HASH_MASK_TBL,
	AAL_TABLE_HASH_HASH_TBL,
	AAL_TABLE_HASH_OVERFLOW_TBL,

	/* 10 */
	AAL_TABLE_HASH_HASH_ACTION,
	AAL_TABLE_HASH_OVERFLOW_ACTION,
	AAL_TABLE_HASH_DEFAULT_ACTION,
	AAL_TABLE_FE_L3E_IF_TBL,
	AAL_TABLE_MCE_INDX_TBL,

	/* 15 */
	AAL_TABLE_ARB_FIB_TBL,
	AAL_TABLE_IM3E_MLDDA,
	AAL_TABLE_IM3E_MLDSA,
	AAL_TABLE_L3_CLS_KEY,
	AAL_TABLE_L3_CLS_FIB,

	/* 20 */
	AAL_TABLE_L3_CAM_DPORT,
	AAL_TABLE_L3_CAM_SPORT,
	AAL_TABLE_L3_CAM_ETHERTYPE,
	AAL_TABLE_L3_CAM_MAC_DA,
	AAL_TABLE_L3_CAM_HTTP_MSGS,

	/* 25 */
	AAL_TABLE_L3_CAM_LLC,
	AAL_TABLE_L3_CAM_PKT_LEN,
	AAL_TABLE_L3_CAM_PPP_CODE,
	AAL_TABLE_L3_CAM_PPP_PROT,
	AAL_TABLE_L3_CAM_SPCL_HDR,

	/* 30 */
	AAL_TABLE_L3_CAM_IP_ADDR,
	AAL_TABLE_L3PE_SIXRD_TBL,
	AAL_TABLE_L3PE_DSL_TBL,
	AAL_TABLE_CACHE_HASH_TBL,
	AAL_TABLE_CACHE_HASH_ACTION_TBL,

	/* 35 */
	AAL_TABLE_HASH_AGE_TBL,
	AAL_TABLE_SA_ID,

	__AAL_TABLE_8279_START__,		/* The enum for HGU_8279 must be after this line */
	AAL_TABLE_8279_HASHLITE_MASK_TBL,
	AAL_TABLE_8279_HASHLITE_HASH_TBL,

	/* 40 */
	AAL_TABLE_8279_HASHLITE_OVERFLOW_TBL,
	AAL_TABLE_8279_HASHLITE_HASH_ACTION,
	AAL_TABLE_8279_HASHLITE_OVERFLOW_ACTION,
	AAL_TABLE_8279_HASHLITE_DEFAULT_ACTION,
	AAL_TABLE_8279_L3_CLS_KEY,

	/* 45 */
	AAL_TABLE_8279_L3_CLS_FIB,
	AAL_TABLE_8279_FE_L3E_IF_TBL,
	AAL_TABLE_8279_L3_CAM_DPORT,
	AAL_TABLE_8279_L3_CAM_SPORT,
	AAL_TABLE_8279_L3_CAM_ETHERTYPE,

	/* 50 */
	AAL_TABLE_8279_L3_CAM_MAC_DA,
	AAL_TABLE_8279_L3_CAM_HTTP_MSGS,
	AAL_TABLE_8279_L3_CAM_LLC,
	AAL_TABLE_8279_L3_CAM_PKT_LEN,
	AAL_TABLE_8279_L3_CAM_PPP_CODE,

	/* 55 */
	AAL_TABLE_8279_L3_CAM_PPP_PROT,
	AAL_TABLE_8279_L3_CAM_SPCL_HDR,
	AAL_TABLE_8279_L3_CAM_IP_ADDR,

	AAL_TABLE_MAX,
} aal_table_e;

char *aal_table_name_get(aal_table_e table_id);

void aal_array_reverse_copy(unsigned char *dst, unsigned char *src, unsigned int length);
void aal_array_nonaligned_reverse_copy(unsigned char *table_entry, unsigned int bit_offset, unsigned char *src, unsigned int bytes);

int aal_entry_add(aal_table_e table_id, void *entry, unsigned int *rslt_idx);
int aal_entry_add_by_idx(aal_table_e table_id, void *entry, unsigned int idx);
int aal_entry_del(aal_table_e table_id, unsigned int idx);
int aal_entry_set(aal_table_e table_id, unsigned int idx, void *entry);
int aal_entry_get(aal_table_e table_id, void *entry, unsigned int *rslt_idx);
int aal_entry_get_by_idx(aal_table_e table_id, unsigned int idx, void *entry);
int aal_entry_print(aal_table_e table_id, unsigned int start_idx, unsigned int count);

int aal_entry_local_data_set(aal_table_e table_id, unsigned int idx, unsigned long local_data);
int aal_entry_local_data_get(aal_table_e table_id, unsigned int idx, unsigned long *local_data);

int aal_table_flush(aal_table_e table_id);
int aal_table_get_avail_count(aal_table_e table_id);
int aal_table_get_used_count(aal_table_e table_id);
int aal_table_get_max_count(aal_table_e table_id);

#endif /* __CA_AAL_H__ */

