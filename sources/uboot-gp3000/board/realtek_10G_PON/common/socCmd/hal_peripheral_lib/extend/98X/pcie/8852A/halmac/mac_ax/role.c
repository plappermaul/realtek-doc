/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#include "role.h"

static struct mac_role_tbl_head role_tbl_head;

static inline u32 role_queue_len(struct mac_role_tbl_head *list)
{
	return list->qlen;
};

static inline void __role_queue_head_init(struct mac_role_tbl_head *list)
{
	list->prev = (struct mac_role_tbl *)list;
	list->next = (struct mac_role_tbl *)list;
	list->qlen = 0;
};

static inline void role_queue_head_init(struct mac_ax_adapter *adapter,
					struct mac_role_tbl_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__role_queue_head_init(list);
}

static inline void __role_insert(struct mac_role_tbl *new_role,
				 struct mac_role_tbl *prev,
				 struct mac_role_tbl *next,
				 struct mac_role_tbl_head *list)
{
	new_role->next = next;
	new_role->prev = prev;
	next->prev  = new_role;
	prev->next = new_role;
	list->qlen++;
}

static inline void __role_queue_before(struct mac_role_tbl_head *list,
				       struct mac_role_tbl *next,
				       struct mac_role_tbl *new_role)
{
	__role_insert(new_role, next->prev, next, list);
}

static inline void __role_queue_tail(struct mac_role_tbl_head *list,
				     struct mac_role_tbl *new_role)
{
	__role_queue_before(list, (struct mac_role_tbl *)list, new_role);
}

static struct mac_role_tbl *__role_alloc(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl *role;

	role = (struct mac_role_tbl *)PLTFM_MALLOC(sizeof(struct mac_role_tbl));
	if (!role)
		return NULL;

	role->macid = 0;
	role->wmm = 0;

	return role;
}

static inline void __role_unlink(struct mac_role_tbl *role,
				 struct mac_role_tbl_head *list)
{
	struct mac_role_tbl *next, *prev;

	list->qlen--;
	next = role->next;
	prev = role->prev;
	role->prev = NULL;
	role->next = NULL;
	next->prev = prev;
	prev->next = next;
}

static inline struct mac_role_tbl *role_peek(struct mac_role_tbl_head *list)
{
	struct mac_role_tbl *role = list->next;

	if (role == (struct mac_role_tbl *)list)
		role = NULL;
	return role;
}

static inline struct mac_role_tbl *role_dequeue(struct mac_role_tbl_head *list)
{
	struct mac_role_tbl *role = role_peek(list);

	if (role)
		__role_unlink(role, list);
	return role;
}

static u32 __role_free(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl_head *list_head = &role_tbl_head;
	struct mac_role_tbl *role;

	if (!role_queue_len(list_head))
		return 0;

	while ((role = role_dequeue(list_head)))
		PLTFM_FREE(role, sizeof(struct mac_role_tbl));

	PLTFM_MUTEX_DEINIT(&list_head->lock);

	return 0;
}

u32 role_tbl_init(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl_head *list_head = &role_tbl_head;

	if (role_queue_len(list_head))
		return MACBUFSZ;

	role_queue_head_init(adapter, list_head);

	return 0;
}

u32 role_tbl_exit(struct mac_ax_adapter *adapter)
{
	__role_free(adapter);

	return 0;
}

u32 mac_add_role(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = &role_tbl_head;

	if (mac_role_srch(adapter, info->macid)) {
		PLTFM_MSG_ERR("[ERR]:existing macid %d\n", info->macid);
		return MACSAMACID;
	}

	if (role_queue_len(list_head) > adapter->hw_info->macid_num) {
		PLTFM_MSG_ERR("[ERR]:full macid %d\n", info->macid);
		return MACMACIDFL;
	}

	role = __role_alloc(adapter);
	if (!role)
		goto role_fail;

	role->macid = info->macid;
	if (!info->band)
		role->wmm = !info->wmm ? MAC_AX_WMM0_SEL : MAC_AX_WMM1_SEL;
	else
		role->wmm = !info->wmm ? MAC_AX_WMM2_SEL : MAC_AX_WMM3_SEL;

	/* Add address CAM configuration later... */

	__role_queue_tail(list_head, role);

	return 0;
role_fail:
	__role_free(adapter);

	return MACBUFALLOC;
}

u32 mac_remove_role(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = &role_tbl_head;

	role = mac_role_srch(adapter, macid);
	if (!role) {
		PLTFM_MSG_ERR("[ERR]macid in role table: %d", macid);
		return MACNOITEM;
	}

	__role_unlink(role, list_head);

	PLTFM_FREE(role, sizeof(struct mac_role_tbl));

	return 0;
}

struct mac_role_tbl *mac_role_srch(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = &role_tbl_head;

	role = list_head->next;

	for (; role != (struct mac_role_tbl *)list_head; role = role->next) {
		if (macid == role->macid)
			return role;
	}

	return 0;
}