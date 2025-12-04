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
 * aal_hashlite_iros.h: HashLite Engine AAL API exported for iROS
 *
 * NOTE: NEVER include this file in this driver. This is for iROS only.
 */
#ifndef __AAL_HASHLITE_IROS_H__
#define __AAL_HASHLITE_IROS_H__

#include "ca_types.h"
#include "aal_common_iros.h"

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

ca_status_t ca_aal_hashlite_hash_add (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	aal_hash_key_t				*hash_key,
	CA_IN	ca_uint32_t				hash_mask_idx,
	CA_IN	aal_hash_action_t			*hash_action,
	CA_IN	ca_uint32_t				actgrp_mask,
	CA_IN	aal_hashlite_aging_e			aging,
	CA_OUT	ca_uint32_t				*rslt_hash_idx );

ca_status_t ca_aal_hashlite_hash_delete (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	ca_uint32_t				hash_idx );

ca_status_t ca_aal_hashlite_hashmask_add (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	aal_hash_mask_t				*hash_mask,
	CA_OUT	ca_uint32_t				*rslt_hash_mask_idx );

ca_status_t ca_aal_hashlite_hashmask_delete (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	ca_uint32_t				hash_mask_idx );

ca_status_t ca_aal_hashlite_aging_timer_set (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	ca_uint32_t				time );

ca_status_t ca_aal_hashlite_aging_timer_get (
	CA_IN	ca_device_id_t				device_id,
	CA_OUT	ca_uint32_t				*time );

ca_status_t ca_aal_hashlite_granularity_get (
	CA_IN	ca_device_id_t				device_id,
	CA_OUT	ca_uint32_t				*granularity );

ca_status_t ca_aal_hashlite_age_set (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	ca_uint32_t				hash_idx,
	CA_IN	aal_hashlite_aging_e			age );

ca_status_t ca_aal_hashlite_age_get (
	CA_IN	ca_device_id_t				device_id,
	CA_IN	ca_uint32_t				hash_idx,
	CA_OUT	aal_hashlite_aging_e			*age,
	CA_OUT	ca_uint32_t				*time );

#endif /* __AAL_HASHLITE_IROS_H__ */

