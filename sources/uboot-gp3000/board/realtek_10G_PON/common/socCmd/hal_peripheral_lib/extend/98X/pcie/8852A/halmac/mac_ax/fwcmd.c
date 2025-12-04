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

#include "fwcmd.h"
#include "trx_desc.h"
#if MAC_AX_FEATURE_DBGPKG
#include "dbgpkg.h"
#endif

#define H2CB_CMD_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_CMD_SIZE		(H2C_CMD_LEN - FWCMD_HDR_LEN)
#define H2CB_CMD_QLEN		8

#define H2CB_DATA_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_DATA_SIZE		(H2C_DATA_LEN - FWCMD_HDR_LEN)
#define H2CB_DATA_QLEN		4

#define H2CB_LONG_DATA_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_LONG_DATA_SIZE	(H2C_LONG_DATA_LEN - FWCMD_HDR_LEN)
#define H2CB_LONG_DATA_QLEN	1

#define FWCMD_WQ_MAX_JOB_NUM	5

static struct h2c_buf_head h2cb_head[H2CB_CLASS_MAX];
static struct fwcmd_wkb_head fwcmd_wq_head;

static inline u32 h2cb_queue_len(struct h2c_buf_head *list)
{
	return list->qlen;
};

static inline void __h2cb_queue_head_init(struct h2c_buf_head *list)
{
	list->prev = (struct h2c_buf *)list;
	list->next = (struct h2c_buf *)list;
	list->qlen = 0;
	list->suspend = 0;
};

static inline void h2cb_queue_head_init(struct mac_ax_adapter *adapter,
					struct h2c_buf_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__h2cb_queue_head_init(list);
}

static inline void __h2cb_insert(struct h2c_buf *new_h2cb, struct h2c_buf *prev,
				 struct h2c_buf *next, struct h2c_buf_head *list)
{
	new_h2cb->next = next;
	new_h2cb->prev = prev;
	next->prev  = new_h2cb;
	prev->next = new_h2cb;
	list->qlen++;
}

static inline void __h2cb_queue_before(struct h2c_buf_head *list,
				       struct h2c_buf *next,
				       struct h2c_buf *new_h2cb)
{
	__h2cb_insert(new_h2cb, next->prev, next, list);
}

static inline void __h2cb_queue_tail(struct h2c_buf_head *list,
				     struct h2c_buf *new_h2cb)
{
	__h2cb_queue_before(list, (struct h2c_buf *)list, new_h2cb);
}

static inline void __h2cb_unlink(struct h2c_buf *h2cb,
				 struct h2c_buf_head *list)
{
	struct h2c_buf *next, *prev;

	list->qlen--;
	next = h2cb->next;
	prev = h2cb->prev;
	h2cb->prev = NULL;
	h2cb->next = NULL;
	next->prev = prev;
	prev->next = next;
}

static inline struct h2c_buf *h2cb_peek(struct h2c_buf_head *list)
{
	struct h2c_buf *h2cb = list->next;

	if (h2cb == (struct h2c_buf *)list)
		h2cb = NULL;
	return h2cb;
}

static inline u8 *h2cb_tail_pointer(const struct h2c_buf *h2cb)
{
	return h2cb->tail;
}

static inline struct h2c_buf *h2cb_dequeue(struct h2c_buf_head *list)
{
	struct h2c_buf *h2cb = h2cb_peek(list);

	if (h2cb)
		__h2cb_unlink(h2cb, list);
	return h2cb;
}

static u8 *__h2cb_alloc_buf_pool(struct mac_ax_adapter *adapter,
				 struct h2c_buf_head *list, u32 size, int num)
{
    u32 block_size = (size * num);
    u8 *ptr;

    ptr = (u8 *)PLTFM_MALLOC(block_size);
    list->pool = ptr;
    list->size = block_size;

    return ptr;
}

static struct h2c_buf *__h2cb_alloc(struct mac_ax_adapter *adapter,
				    enum h2c_buf_class buf_class,
				    u32 hdr_size, u8 *buf_ptr, int buf_size)
{
	struct h2c_buf *h2cb;

	_ASSERT_(!buf_ptr);

	h2cb = (struct h2c_buf *)PLTFM_MALLOC(sizeof(struct h2c_buf));
	if (!h2cb)
		return NULL;
	PLTFM_MEMSET(h2cb, 0, sizeof(struct h2c_buf));

	h2cb->_class_ = buf_class;
	h2cb->id = 0;
	h2cb->master = 0;
	h2cb->len = 0;
	h2cb->head = buf_ptr;
	h2cb->end = h2cb->head + buf_size;
	h2cb->data = h2cb->head + hdr_size;
	h2cb->tail = h2cb->data;
	h2cb->hdr_len = hdr_size;
	h2cb->flags |= H2CB_FLAGS_FREED;

	return h2cb;
}

static u32 __h2cb_free(struct mac_ax_adapter *adapter,
		       enum h2c_buf_class buf_class)
{
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST)
		return MACNOITEM;

	if (!list_head->pool)
		return MACNPTR;

	if (!h2cb_queue_len(list_head))
		return 0;

	while ((h2cb = h2cb_dequeue(list_head)))
		PLTFM_FREE(h2cb, sizeof(struct h2c_buf));

	PLTFM_FREE(list_head->pool, list_head->size);
	list_head->pool = NULL;
	list_head->size = 0;
	PLTFM_MUTEX_DEINIT(&list_head->lock);

	return 0;
}

static u32 __h2cb_init(struct mac_ax_adapter *adapter,
		       enum h2c_buf_class buf_class, u32 num, u32 buf_size,
		       u32 hdr_size, u32 tailer_size)
{
	u32 i;
	u8 *ptr;
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	u32 real_size = buf_size + hdr_size + tailer_size;
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST)
		return MACNOITEM;

	if (h2cb_queue_len(list_head))
		return MACBUFSZ;

	h2cb_queue_head_init(adapter, list_head);

	ptr = __h2cb_alloc_buf_pool(adapter, list_head, real_size, num);
	if (!ptr)
		return MACNPTR;

	for (i = 0; i < num; i++) {
		h2cb = __h2cb_alloc(adapter,
				    buf_class, hdr_size, ptr, real_size);
		if (!h2cb)
			goto h2cb_fail;
		__h2cb_queue_tail(list_head, h2cb);
		ptr += real_size;
	}

	return 0;
h2cb_fail:
	__h2cb_free(adapter, buf_class);

	return MACBUFALLOC;
}

static inline u32 fwcmd_wkb_queue_len(struct fwcmd_wkb_head *list)
{
	return list->qlen;
};

static inline void __fwcmd_wkb_queue_head_init(struct fwcmd_wkb_head *list)
{
	list->prev = (struct h2c_buf *)list;
	list->next = (struct h2c_buf *)list;
	list->qlen = 0;
};

static inline void fwcmd_wkb_queue_head_init(struct mac_ax_adapter *adapter,
					     struct fwcmd_wkb_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__fwcmd_wkb_queue_head_init(list);
}

static u32 __fwcmd_wkb_init(struct mac_ax_adapter *adapter)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (fwcmd_wkb_queue_len(list_head))
		return MACBUFSZ;

	fwcmd_wkb_queue_head_init(adapter, list_head);

	return 0;
}

u32 h2cb_init(struct mac_ax_adapter *adapter)
{
	u32 ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_CMD, H2CB_CMD_QLEN,
			  H2CB_CMD_SIZE, H2CB_CMD_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_DATA, H2CB_DATA_QLEN,
			  H2CB_DATA_SIZE, H2CB_DATA_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_LONG_DATA, H2CB_LONG_DATA_QLEN,
			  H2CB_LONG_DATA_SIZE, H2CB_LONG_DATA_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __fwcmd_wkb_init(adapter);
	if (ret)
		return ret;

	return 0;
}

u32 h2cb_exit(struct mac_ax_adapter *adapter)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (fwcmd_wkb_queue_len(list_head))
		return MACBUFSZ;

	__h2cb_free(adapter, H2CB_CLASS_CMD);
	__h2cb_free(adapter, H2CB_CLASS_DATA);
	__h2cb_free(adapter, H2CB_CLASS_LONG_DATA);

	return 0;
}

struct h2c_buf *h2cb_alloc(struct mac_ax_adapter *adapter,
			   enum h2c_buf_class buf_class)
{
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST) {
		PLTFM_MSG_ERR("[ERR]unknown class\n");
		return NULL;
	}

	PLTFM_MUTEX_LOCK(&list_head->lock);

	h2cb = h2cb_dequeue(list_head);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]allocate h2cb, class : %d\n", buf_class);
		goto h2cb_fail;
	}

	if (!(h2cb->flags & H2CB_FLAGS_FREED)) {
		PLTFM_MSG_ERR("[ERR]not freed flag\n");
		PLTFM_FREE(h2cb, sizeof(struct h2c_buf));
		goto h2cb_fail;
	}

	h2cb->flags &= ~H2CB_FLAGS_FREED;
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return h2cb;
h2cb_fail:
	PLTFM_MUTEX_UNLOCK(&list_head->lock);
	return NULL;
}

void h2cb_free(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
{
	struct h2c_buf_head *list_head;

	if (h2cb->flags & H2CB_FLAGS_FREED) {
		PLTFM_MSG_ERR("[ERR]freed flag\n");
		return;
	}

	if (h2cb->_class_ >= H2CB_CLASS_LAST) {
		PLTFM_MSG_ERR("[ERR]unknown class\n");
		return;
	}

	list_head = &h2cb_head[h2cb->_class_];

	h2cb->len = 0;
	h2cb->data = h2cb->head + h2cb->hdr_len;
	h2cb->tail = h2cb->data;
	h2cb->flags |= H2CB_FLAGS_FREED;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	__h2cb_queue_tail(list_head, h2cb);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);
}

u8 *h2cb_push(struct h2c_buf *h2cb, u32 len)
{
	h2cb->data -= len;
	h2cb->len  += len;

	if (h2cb->data < h2cb->head)
		return NULL;

	return h2cb->data;
}

u8 *h2cb_pull(struct h2c_buf *h2cb, u32 len)
{
	h2cb->data += len;

	if (h2cb->data > h2cb->end)
		return NULL;

	if (h2cb->len < len)
		return NULL;

	h2cb->len -= len;

	return h2cb->data;
}

u8 *h2cb_put(struct h2c_buf *h2cb, u32 len)
{
	u8 *tmp = h2cb_tail_pointer(h2cb);

	h2cb->tail += len;
	h2cb->len += len;

	if (h2cb->tail > h2cb->end)
		return NULL;

	return tmp;
}

static inline void __fwcmd_wq_insert(struct h2c_buf *new_h2cb,
				     struct h2c_buf *prev, struct h2c_buf *next,
				     struct fwcmd_wkb_head *list)
{
	new_h2cb->next = next;
	new_h2cb->prev = prev;
	next->prev  = new_h2cb;
	prev->next = new_h2cb;
	list->qlen++;
}

static inline void __fwcmd_wq_before(struct fwcmd_wkb_head *list,
				     struct h2c_buf *next,
				     struct h2c_buf *new_h2cb)
{
	__fwcmd_wq_insert(new_h2cb, next->prev, next, list);
}

static inline void __fwcmd_wq_tail(struct fwcmd_wkb_head *list,
				   struct h2c_buf *new_h2cb)
{
	__fwcmd_wq_before(list, (struct h2c_buf *)list, new_h2cb);
}

u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u8 rack, u8 dack)
{
	struct fwcmd_hdr *hdr;

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, FWCMD_HDR_TYPE) |
				SET_WORD(cat, FWCMD_HDR_CAT) |
				SET_WORD(_class_, FWCMD_HDR_CLASS) |
				SET_WORD(func, FWCMD_HDR_FUNC));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->len, H2C_HDR_LEN) |
				(rack ? H2C_HDR_RACK_EN : 0) |
				(dack ? H2C_HDR_DACK_EN : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);

	return 0;
}

u32 h2c_pkt_build_txd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
{
	u8 *buf;
	u32 ret;
	u32 txd_len;
	struct mac_ax_txpkt_info info;

	info.type = MAC_AX_PKT_H2C;
	info.pktsize = h2cb->len;
	txd_len = mac_txdesc_len(adapter, &info);

	buf = h2cb_push(h2cb, txd_len);
	if (!buf)
		return MACNPTR;

	ret = mac_build_txdesc(adapter, &info, buf, txd_len);
	if (ret)
		return ret;

	return 0;
}

u32 fwcmd_wq_enqueue(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (list_head->qlen > FWCMD_WQ_MAX_JOB_NUM) {
		PLTFM_MSG_WARN("[WARN]fwcmd work queue full\n");
		return MACBUFALLOC;
	}

	/* worq queue doesn't need wd body */
	h2cb_pull(h2cb, WD_BODY_LEN);
	PLTFM_MUTEX_LOCK(&list_head->lock);
	__fwcmd_wq_tail(list_head, h2cb);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return 0;
}

static inline void __fwcmd_wq_unlink(struct h2c_buf *h2cb,
				     struct fwcmd_wkb_head *list)
{
	struct h2c_buf *next, *prev;

	list->qlen--;
	next = h2cb->next;
	prev = h2cb->prev;
	h2cb->prev = NULL;
	h2cb->next = NULL;
	next->prev = prev;
	prev->next = next;
}

struct h2c_buf *fwcmd_wq_dequeue(struct mac_ax_adapter *adapter, u32 id)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;
	struct h2c_buf *h2cb;
	u32 hdr0;
	u16 type = GET_FWCMD_TYPE(id);
	u16 cat = GET_FWCMD_CAT(id);
	u16 _class_ = GET_FWCMD_CLASS(id);
	u16 func = GET_FWCMD_FUNC(id);

	PLTFM_MUTEX_LOCK(&list_head->lock);

	for (h2cb = list_head->next; h2cb->next != list_head->next;
	     h2cb = h2cb->next) {
		hdr0 = ((struct fwcmd_hdr *)h2cb->data)->hdr0;
		hdr0 = le32_to_cpu(hdr0);
		if (type == GET_FIELD(hdr0, FWCMD_HDR_TYPE) &&
		    cat == GET_FIELD(hdr0, FWCMD_HDR_CAT) &&
		    _class_ == GET_FIELD(hdr0, FWCMD_HDR_CLASS) &&
		    func == GET_FIELD(hdr0, FWCMD_HDR_FUNC)) {
			__fwcmd_wq_unlink(h2cb, list_head);
			PLTFM_MUTEX_UNLOCK(&list_head->lock);
			return h2cb;
		}
	}

	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	PLTFM_MSG_ERR("[ERR]cannot find wq item: %X\n", id);

	return NULL;
}

u32 fwcmd_wq_idle(struct mac_ax_adapter *adapter, u32 id)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;
	struct h2c_buf *h2cb;

	PLTFM_MUTEX_LOCK(&list_head->lock);

	for (h2cb = list_head->next; h2cb->next != list_head->next;
	     h2cb = h2cb->next) {
		if (h2cb->id == id) {
			PLTFM_MUTEX_UNLOCK(&list_head->lock);
			return MACWQBUSY;
		}
	}

	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return 0;
}

#if MAC_AX_FEATURE_DBGPKG
static struct c2h_proc_class c2h_proc_sys[] = {
	{FWCMD_C2H_CL_CMD_PATH, c2h_sys_cmd_path},
	{FWCMD_C2H_CL_NULL, NULL},
};
#else
static struct c2h_proc_class c2h_proc_sys[] = {
	{FWCMD_C2H_CL_NULL, NULL},
};
#endif

static struct c2h_proc_class c2h_proc_mac[] = {
	{FWCMD_C2H_CL_NULL, NULL},
};

static struct c2h_proc_class c2h_proc_phy[] = {
	{FWCMD_C2H_CL_NULL, NULL},
};

static inline struct c2h_proc_class *c2h_proc_sel(u8 cat)
{
	struct c2h_proc_class *proc;

	switch (cat) {
	case FWCMD_C2H_CAT_SYS:
		proc = c2h_proc_sys;
		break;
	case FWCMD_C2H_CAT_MAC:
		proc = c2h_proc_mac;
		break;
	case FWCMD_C2H_CAT_PHY:
		proc = c2h_proc_phy;
		break;
	default:
		proc = NULL;
		break;
	}

	return proc;
}

u32 mac_process_c2h(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	u8 _class_;
	struct c2h_proc_class *proc;
	struct fwcmd_hdr *hdr;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len) = NULL;

	hdr = (struct fwcmd_hdr *)(buf + RXD_SHORT_LEN);

	if (GET_FIELD(hdr->hdr0, FWCMD_HDR_TYPE) != FWCMD_TYPE_C2H) {
		PLTFM_MSG_ERR("[ERR]wrong fwcmd type: %X\n", hdr->hdr0);
		return MACNOITEM;
	}

	proc = c2h_proc_sel(GET_FIELD(hdr->hdr0, FWCMD_HDR_CAT));
	if (!proc) {
		PLTFM_MSG_ERR("[ERR]wrong fwcmd cat: %X\n", hdr->hdr0);
		return MACNOITEM;
	}

	_class_ = GET_FIELD(hdr->hdr0, FWCMD_HDR_CLASS);

	for (; proc->id != FWCMD_C2H_CL_NULL; proc++) {
		if (_class_ == proc->id) {
			handler = proc->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null class handler id: %X", proc->id);
		return MACNOITEM;
	}

	return handler(adapter, buf, len);
}