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
 * aal_hash_iros.h: Hash Engine AAL API exported for iROS
 *
 */
#ifndef __AAL_HASH_IROS_H__
#define __AAL_HASH_IROS_H__

#include "ca_types.h"
#include "aal_common_iros.h"

typedef enum {
        TUPLE_TYPE_UNUSED = -1,
        TUPLE_TYPE_FWD = 0,
        TUPLE_TYPE_QOS = 1,
} aal_hash_tuple_type_e;

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

ca_status_t aal_hash_add (
CA_IN	ca_device_id_t			device_id,
CA_IN	aal_hash_key_t		*hash_key,
CA_IN	aal_hash_action_t		*hash_action,
CA_IN	aal_hash_tuple_type_e	hash_type,
CA_IN	aal_hash_aging_e		hash_age,
CA_IN	ca_uint32_t			mask_id,
CA_IN	ca_uint32_t			profile_id,
CA_OUT	ca_uint32_t			*hash_idx);

ca_status_t aal_hash_crc_hw_offload (
CA_IN	ca_device_id_t	device_id,
CA_IN	L3FE_HDR_I_t	*hdr_i,
CA_IN	ca_uint32_t	mask_id,
CA_OUT	ca_uint32_t	*crc32,
CA_OUT	ca_uint16_t	*crc16);

ca_status_t aal_hash_delete (
CA_IN	ca_device_id_t		device_id,
CA_IN	ca_uint32_t		hash_idx);

ca_status_t aal_mask_tbl_entry_add_iros (
CA_IN	ca_device_id_t	device_id,
CA_IN	aal_hash_mask_t *entry,
CA_OUT	ca_uint32_t *rslt_idx);

ca_status_t aal_mask_tbl_entry_del_iros (
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t rslt_idx);

ca_status_t aal_hash_age_set_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t	table_id,
CA_IN	ca_uint32_t	hash_idx,
CA_IN	ca_uint32_t	age);

ca_status_t aal_hash_age_get_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t	table_id,
CA_IN	ca_uint32_t	hash_idx,
CA_OUT	ca_uint32_t	*age,
CA_OUT	ca_uint32_t	*time);

ca_status_t aal_hash_current_granularity_get_iros(
CA_IN	ca_device_id_t	device_id,
CA_OUT	ca_uint32_t	*granularity);

ca_status_t aal_hash_aging_timer_set_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t	time);

ca_status_t aal_hash_aging_timer_get_iros(
CA_IN	ca_device_id_t	device_id,
CA_OUT	ca_uint32_t	*time);

ca_status_t aal_hash_cache_invalidate_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t	hash_idx,
CA_IN	ca_uint32_t	hash_crc16);

ca_status_t aal_hash_cache_allocation_iros(
CA_IN	ca_device_id_t	device_id,
CA_IN	ca_uint32_t	hash_idx,
CA_IN	ca_uint32_t	hash_crc16,
CA_IN	ca_uint8_t	loc,
CA_IN	ca_uint8_t	age,
CA_IN	ca_uint8_t	priority);

ca_status_t aal_hash_cache_count_get(
CA_IN	ca_device_id_t	device_id,
CA_OUT	ca_uint32_t	*ut_cnt);

ca_status_t aal_64k_wan_to_lan_hash_add_unit_test(
CA_IN	ca_device_id_t	device_id);

ca_status_t aal_64k_lan_to_wan_hash_add_unit_test(
CA_IN	ca_device_id_t	device_id);

ca_status_t aal_64k_bidirection_hash_add_unit_test(
CA_IN	ca_device_id_t	device_id);

ca_status_t aal_64k_hash_del_unit_test(
CA_IN	ca_device_id_t	device_id);

#endif /* __AAL_HASH_IROS_H__ */

