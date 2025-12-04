/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Define DS packet broadcaster
 *
 * Feature : Broadcast DS packet to lan interface
 */

#ifndef __DS_PKT_BCASTER_H__
#define __DS_PKT_BCASTER_H__


#include "common/type.h"


#define VLAN_TAG_PRI_MASK	(0xE000)
#define VLAN_TAG_PRI_SHIFT	(13)
#define VLAN_TAG_DEI_MASK	(0x1000)
#define VLAN_TAG_DEI_SHIFT	(12)
#define VLAN_TAG_VID_MASK	(0xFFF)
#define m_get_tci_pri(x)    ((x & VLAN_TAG_PRI_MASK) >> VLAN_TAG_PRI_SHIFT)
#define m_get_tci_dei(x)    ((x & VLAN_TAG_DEI_MASK) >> VLAN_TAG_DEI_SHIFT)
#define m_get_tci_vid(x)    (x & VLAN_TAG_VID_MASK)
#define m_set_tci_pri(x,v)  ((x & ~VLAN_TAG_PRI_MASK) |\
 								((v << VLAN_TAG_PRI_SHIFT) & VLAN_TAG_PRI_MASK))
#define m_set_tci_dei(x,v)  ((x & ~VLAN_TAG_DEI_MASK) |\
								((v << VLAN_TAG_DEI_SHIFT) & VLAN_TAG_DEI_MASK))
#define m_set_tci_vid(x,v)  ((x & ~VLAN_TAG_VID_MASK) | (v & VLAN_TAG_VID_MASK))

typedef struct
{
	uint8	tagPri;
	uint8	tagDei;
	uint16	tagVid;
	uint16	tagTpid;
} tagField_t;

typedef enum
{
	TAG_FILTER_MODE_NO_TAG,
	TAG_FILTER_MODE_TAG,
	TAG_FILTER_MODE_DONT_CARE,
} tagFilterMode_t;

typedef enum
{
	TAG_FILTER_FIELD_PRI			= (1 << 0),
	TAG_FILTER_FIELD_DEI			= (1 << 1),
	TAG_FILTER_FIELD_VID			= (1 << 2),
	TAG_FILTER_FIELD_TPID			= (1 << 3),
} tagFilterMask_t;

typedef struct
{
	tagFilterMode_t 	sTagMode;
	tagFilterMode_t		cTagMode;
	tagFilterMask_t		sTagMask;
	tagFilterMask_t		cTagMask;
	tagField_t			sTagField;
	tagField_t			cTagField;
} tagFilter_t;

typedef enum
{
	TAG_TREATMENT_MODE_NOP,
	TAG_TREATMENT_MODE_REMOVE,
	TAG_TREATMENT_MODE_INSERT,
	TAG_TREATMENT_MODE_MODIFY,
} tagTreatmentMode_t;

typedef enum
{
	TAG_TREATMENT_PRI_ACTION_NOP,
	TAG_TREATMENT_PRI_ACTION_INSERT,
	TAG_TREATMENT_PRI_ACTION_COPY_FROM_INNER,
	TAG_TREATMENT_PRI_ACTION_COPY_FROM_OUTER,
	TAG_TREATMENT_PRI_ACTION_DERIVE_FROM_DSCP
} tagTreatmentPriAction_t;

typedef enum
{
	TAG_TREATMENT_DEI_ACTION_NOP,
	TAG_TREATMENT_DEI_ACTION_INSERT,
	TAG_TREATMENT_DEI_ACTION_COPY_FROM_INNER,
	TAG_TREATMENT_DEI_ACTION_COPY_FROM_OUTER,
} tagTreatmentDeiAction_t;

typedef enum
{
	TAG_TREATMENT_VID_ACTION_NOP,
	TAG_TREATMENT_VID_ACTION_INSERT,
	TAG_TREATMENT_VID_ACTION_COPY_FROM_INNER,
	TAG_TREATMENT_VID_ACTION_COPY_FROM_OUTER,
} tagTreatmentVidAction_t;

typedef struct
{
	tagTreatmentPriAction_t		priAction;
	tagTreatmentDeiAction_t		deiAction;
	tagTreatmentVidAction_t		vidAction;
} tagTreatmentAction_t;

typedef struct
{
	tagTreatmentMode_t		sTagMode;
	tagTreatmentMode_t		cTagMode;
	tagTreatmentAction_t	sTagAction;
	tagTreatmentAction_t	cTagAction;
	tagField_t  			sTagField;
	tagField_t				cTagField;
} tagTreatment_t;

typedef struct
{
	tagFilter_t		tagFilter;
	tagTreatment_t	tagTreatment;
	uint32			dstPortMask;
} tagOperation_t;

typedef struct {
	struct list_head	list;
	uint32				idx;
	uint32				hit;
	tagOperation_t		data;
} tagOperation_entry_t;

extern int ds_pkt_bcaster_ds_bc_gem_flow_id_set(uint32 gemFlowId);
extern int ds_pkt_bcaster_ds_tag_operation_add(tagOperation_t *pData, uint32 idx);
extern int ds_pkt_bcaster_ds_tag_operation_del(uint32 idx);
extern int ds_pkt_bcaster_ds_tag_operation_flush(void);
extern int ds_pkt_bcaster_ds_tag_operation_dump(void);

#endif
