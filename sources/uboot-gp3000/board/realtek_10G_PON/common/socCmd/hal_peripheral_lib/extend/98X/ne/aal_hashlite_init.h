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
 * aal_hashlite_init.h: HashLite Engine Initialization
 *
 */
#ifndef __AAL_HASHLITE_INIT_H__
#define __AAL_HASHLITE_INIT_H__

#include "aal_common.h"
#include "aal_hashlite.h"
#include "aal_hashlite_def.h"

/***************************************************************************
 * all Hash Masks
 ***************************************************************************/

extern aal_hash_mask_t HM_L3_GATEWAY;		/* The gateway that is routed to. */
extern aal_hash_mask_t HM_L3_NEIGHBOR;		/* Used by ARP, ICMPv6 Neighbor Discovery. */
extern aal_hash_mask_t HM_PON_DS;		/* PON downstream traffic */

/***************************************************************************
 * all Hash Actions
 ***************************************************************************/

#define HL_ACTGRP_L3_GENERIC	(ACTION_GROUP_08 | ACTION_GROUP_11 | ACTION_GROUP_13 | ACTION_GROUP_18 | ACTION_GROUP_19)

typedef struct {
	HASH_ACTION_GROUP_08;
	HASH_ACTION_GROUP_11;
	HASH_ACTION_GROUP_13;
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
} __attribute__((packed)) hl_act_l3_generic_t;

#define HL_ACTGRP_PON_DS	(ACTION_GROUP_09 | ACTION_GROUP_18)

typedef struct {
	HASH_ACTION_GROUP_09;
	HASH_ACTION_GROUP_18;
} __attribute__((packed)) hl_act_pon_ds_t;

int hashlite_action_dump(unsigned int act_grp_mask, aal_hashlite_hash_action_entry_t *act_entry);

/***************************************************************************
 * function prototype
 ***************************************************************************/

int hashlite_hashmask_init(void);
int hashlite_profile_init(void);

#endif /* __AAL_HASHLITE_INIT_H__ */

