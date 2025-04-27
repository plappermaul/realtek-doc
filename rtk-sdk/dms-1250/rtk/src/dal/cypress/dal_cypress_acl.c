/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Ingress ACL
 *            2) Egress ACL
 *            3) Field Selector
 *            4) Range Check
 *            5) Meter
 *            6) Counter
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_acl.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/acl.h>

/*
 * Symbol Definition
 */
#define DAL_CYPRESS_MAX_NUM_OF_FIXED_FIELD      3
#define DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD   12
#define DAL_CYPRESS_DATA_BITS                   8
#define DAL_CYPRESS_FILED_BITS                  16
#define DAL_CYPRESS_BUFFER_UNIT_LENGTH_BITS     32

#define DAL_CYPRESS_ACL_GLB_HIT_GRP_NUM         3
#define DAL_CYPRESS_ACL_GLB_HIT_ENTRY_PER_BIT   32
#define DAL_CYPRESS_MAX_HITINDICATION_BLK_ENTRY 18



#define DAL_CYPRESS_MAX_INFO_IDX                \
    (DAL_CYPRESS_BUFFER_UNIT_LENGTH_BITS / DAL_CYPRESS_DATA_BITS)

#define DAL_CYPRESS_DATA_WIDTH_GET(_data_len)                   \
    (((_data_len) + DAL_CYPRESS_DATA_BITS - 1) / DAL_CYPRESS_DATA_BITS)

#define DAL_CYPRESS_GET_BYTE_IDX(_offset)                       \
    ((_offset) / DAL_CYPRESS_DATA_BITS)

#define DAL_CYPRESS_GET_INFO_IDX(_size, _offset)                \
    (DAL_CYPRESS_DATA_WIDTH_GET((_size)) - DAL_CYPRESS_DATA_WIDTH_GET((_offset)))

#define DAL_CYPRESS_GET_INFO_OFFSET(_max, _idx)                       \
    ((((_idx) - ((_max) % DAL_CYPRESS_MAX_INFO_IDX)) % DAL_CYPRESS_MAX_INFO_IDX) * DAL_CYPRESS_DATA_BITS)

/*
 * Data Declaration
 */
static uint32               acl_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         acl_sem[RTK_MAX_NUM_OF_UNIT];

typedef struct dal_cypress_acl_entryTable_s
{
    uint16   data_field[DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD];
    uint8    data_fixed[DAL_CYPRESS_MAX_NUM_OF_FIXED_FIELD];
    uint16   care_field[DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD];
    uint8    care_fixed[DAL_CYPRESS_MAX_NUM_OF_FIXED_FIELD];
} dal_cypress_acl_entryTable_t;

typedef struct dal_cypress_acl_fieldLocation_s
{
    uint32   template_field_type;
    uint32   field_offset;
    uint32   field_length;
    uint32   data_offset;
} dal_cypress_acl_fieldLocation_t;

typedef struct dal_cypress_acl_fixField_s
{
    uint32                          data_field;     /* data field in chip view */
    uint32                          mask_field;     /* mask field in chip view */
    uint32                          position;       /* position in fix data */
    rtk_acl_fieldType_t             type;           /* field type in user view */
    dal_cypress_acl_fieldLocation_t *pField;
} dal_cypress_acl_fixField_t;

typedef struct dal_cypress_acl_entryField_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          field_number;   /* locate in how many fields */
    dal_cypress_acl_fieldLocation_t *pField;
} dal_cypress_acl_entryField_t;

typedef struct dal_cypress_acl_templateField_s
{
    rtk_acl_templateFieldType_t     field_type;     /* field type in logical */
    uint32                          physical_id;    /* physical field ID in ASIC */
    uint32                          valid_location; /* valid field location, 0 means no limitation */
} dal_cypress_acl_templateField_t;

typedef struct dal_cypress_acl_entryFieldInfo_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          field_number;   /* locate in how many fields */
} dal_cypress_acl_entryFieldInfo_t;

dal_cypress_acl_entryFieldInfo_t dal_cypress_acl_field_info_list[] =
{
    {   /* field name    */           USER_FIELD_IP_FLAG,
        /* field number  */           1,
    },
    {   /* field name    */           USER_FIELD_IGMP_TYPE,
        /* field number  */           1,
    },
    {   /* field name    */           USER_FIELD_TCP_FLAG,
        /* field number  */           1,
    },
    {   /* field name    */           USER_FIELD_ICMP_CODE,
        /* field number  */           1,
    },
    {   /* field name    */           USER_FIELD_ICMP_TYPE,
        /* field number  */           1,
    },
};

static uint32 template_iacl_data_field[DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD] = {
    CYPRESS_IACL_FIELD_0tf, CYPRESS_IACL_FIELD_1tf, CYPRESS_IACL_FIELD_2tf,
    CYPRESS_IACL_FIELD_3tf, CYPRESS_IACL_FIELD_4tf, CYPRESS_IACL_FIELD_5tf,
    CYPRESS_IACL_FIELD_6tf, CYPRESS_IACL_FIELD_7tf, CYPRESS_IACL_FIELD_8tf,
    CYPRESS_IACL_FIELD_9tf, CYPRESS_IACL_FIELD_10tf, CYPRESS_IACL_FIELD_11tf};

static uint32 template_iacl_mask_field[DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD] = {
    CYPRESS_IACL_BMSK_FIELD_0tf, CYPRESS_IACL_BMSK_FIELD_1tf, CYPRESS_IACL_BMSK_FIELD_2tf,
    CYPRESS_IACL_BMSK_FIELD_3tf, CYPRESS_IACL_BMSK_FIELD_4tf, CYPRESS_IACL_BMSK_FIELD_5tf,
    CYPRESS_IACL_BMSK_FIELD_6tf, CYPRESS_IACL_BMSK_FIELD_7tf, CYPRESS_IACL_BMSK_FIELD_8tf,
    CYPRESS_IACL_BMSK_FIELD_9tf, CYPRESS_IACL_BMSK_FIELD_10tf, CYPRESS_IACL_BMSK_FIELD_11tf};

static uint32 template_eacl_data_field[DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD] = {
    CYPRESS_EACL_FIELD_0tf, CYPRESS_EACL_FIELD_1tf, CYPRESS_EACL_FIELD_2tf,
    CYPRESS_EACL_FIELD_3tf, CYPRESS_EACL_FIELD_4tf, CYPRESS_EACL_FIELD_5tf,
    CYPRESS_EACL_FIELD_6tf, CYPRESS_EACL_FIELD_7tf, CYPRESS_EACL_FIELD_8tf,
    CYPRESS_EACL_FIELD_9tf, CYPRESS_EACL_FIELD_10tf, CYPRESS_EACL_FIELD_11tf};

static uint32 template_eacl_mask_field[DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD] = {
    CYPRESS_EACL_BMSK_FIELD_0tf, CYPRESS_EACL_BMSK_FIELD_1tf, CYPRESS_EACL_BMSK_FIELD_2tf,
    CYPRESS_EACL_BMSK_FIELD_3tf, CYPRESS_EACL_BMSK_FIELD_4tf, CYPRESS_EACL_BMSK_FIELD_5tf,
    CYPRESS_EACL_BMSK_FIELD_6tf, CYPRESS_EACL_BMSK_FIELD_7tf, CYPRESS_EACL_BMSK_FIELD_8tf,
    CYPRESS_EACL_BMSK_FIELD_9tf, CYPRESS_EACL_BMSK_FIELD_10tf, CYPRESS_EACL_BMSK_FIELD_11tf};

uint32 template_eacl_list[] = {TMPLTE_FIELD_DPMMASK, TMPLTE_FIELD_DPM0,
    TMPLTE_FIELD_DPM1, TMPLTE_FIELD_DPM2, TMPLTE_FIELD_DPM3,
    TMPLTE_FIELD_L2DPM0, TMPLTE_FIELD_L2DPM1, TMPLTE_FIELD_L2DPM2,
    TMPLTE_FIELD_L2DPM3, TMPLTE_FIELD_IVLAN, TMPLTE_FIELD_OVLAN,
    TMPLTE_FIELD_FWD_VID, TMPLTE_FIELD_END};

rtk_acl_template_t dal_cypress_acl_fixedTemplate[] =
{
    {{TMPLTE_FIELD_SPMMASK, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_OTAG,
        TMPLTE_FIELD_SMAC0, TMPLTE_FIELD_SMAC1, TMPLTE_FIELD_SMAC2,
        TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_VID_RANG0}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0,
        TMPLTE_FIELD_DIP1, TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_L4_SPORT,
        TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_ICMP_IGMP, TMPLTE_FIELD_ITAG,
        TMPLTE_FIELD_OTAG, TMPLTE_FIELD_FIELD_SELECTOR_0,
        TMPLTE_FIELD_FIELD_SELECTOR_1}},

    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_ITAG, TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_SIP0,
        TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1}},

    {{TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1, TMPLTE_FIELD_DIP2,
        TMPLTE_FIELD_DIP3, TMPLTE_FIELD_DIP4, TMPLTE_FIELD_DIP5,
        TMPLTE_FIELD_DIP6, TMPLTE_FIELD_DIP7, TMPLTE_FIELD_L4_DPORT,
        TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_ICMP_IGMP,
        TMPLTE_FIELD_IP_TOS_PROTO}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_FIELD_SELECTOR_6,
        TMPLTE_FIELD_FIELD_SELECTOR_7, TMPLTE_FIELD_FIELD_SELECTOR_8,
        TMPLTE_FIELD_FIELD_SELECTOR_9, TMPLTE_FIELD_FIELD_SELECTOR_10,
        TMPLTE_FIELD_FIELD_SELECTOR_11, TMPLTE_FIELD_ITAG,
        TMPLTE_FIELD_SMAC0, TMPLTE_FIELD_SMAC1, TMPLTE_FIELD_SMAC2}},
};

rtk_acl_template_t dal_cypress_acl_fixedTemplate_6290[] =
{
    {{TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1, TMPLTE_FIELD_ITAG,
        TMPLTE_FIELD_SMAC0, TMPLTE_FIELD_SMAC1, TMPLTE_FIELD_SMAC2,
        TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_SPM2, TMPLTE_FIELD_SPM3}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0,
        TMPLTE_FIELD_DIP1, TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_L4_SPORT,
        TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_ICMP_IGMP, TMPLTE_FIELD_SPM0,
        TMPLTE_FIELD_SPM1, TMPLTE_FIELD_SPM2, TMPLTE_FIELD_SPM3}},

    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_ITAG, TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_SIP0,
        TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1}},

    {{TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1, TMPLTE_FIELD_DIP2,
        TMPLTE_FIELD_DIP3, TMPLTE_FIELD_DIP4, TMPLTE_FIELD_DIP5,
        TMPLTE_FIELD_DIP6, TMPLTE_FIELD_DIP7, TMPLTE_FIELD_L4_DPORT,
        TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_ICMP_IGMP,
        TMPLTE_FIELD_IP_TOS_PROTO}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_FIELD_SELECTOR_6,
        TMPLTE_FIELD_FIELD_SELECTOR_7, TMPLTE_FIELD_FIELD_SELECTOR_8,
        TMPLTE_FIELD_FIELD_SELECTOR_9, TMPLTE_FIELD_FIELD_SELECTOR_10,
        TMPLTE_FIELD_FIELD_SELECTOR_11, TMPLTE_FIELD_SPM0,
        TMPLTE_FIELD_SPM1, TMPLTE_FIELD_SPM2, TMPLTE_FIELD_SPM3}},
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_TEMPLATE_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FRAME_TYPE_L2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ITAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OTAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ITAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OTAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FRAME_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FRAME_TYPE_L4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_IP_NONZERO_OFFSET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_SWITCHMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_MGNT_VLAN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SPN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SPMMASK_FIX[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SPMM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPMMASK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0x0,
        /* length */                 5,
        /* data offset */            0x30
    },
    {
        /* template field type */    TMPLTE_FIELD_SPM2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20
    },
    {
        /* template field type */    TMPLTE_FIELD_SPM1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_SPM0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DMAC2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20,
    },
    {
        /* template field type */    TMPLTE_FIELD_DMAC1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_DMAC0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SMAC2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20,
    },
    {
        /* template field type */    TMPLTE_FIELD_SMAC1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_SMAC0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ETHERTYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ETHERTYPE,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OTAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DEI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OTAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ITAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_CFI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ITAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ETAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ARPOPCODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OMPLS_LABEL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OILABEL,
        /* offset address */         0xc,
        /* length */                 4,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_OLABEL,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OMPLS_EXP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OILABEL,
        /* offset address */         0x9,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OMPLS_LABEL_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OILABEL,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IMPLS_LABEL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OILABEL,
        /* offset address */         0x4,
        /* length */                 4,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_ILABEL,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IMPLS_EXP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OILABEL,
        /* offset address */         0x1,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IMPLS_LABEL_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OILABEL,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP4_SIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SIP1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP4_DIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DIP1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_SIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x70,
    },
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x60,
    },
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x50,
    },
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x40,
    },
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x30,
    },
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_DIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DIP7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x70,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x60,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x50,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x40,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x30,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_DIP0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP4TOS_IP6TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IPDSCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0xA,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP4PROTO_IP6NH[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_FLAG_OFFSET,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xe,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP_FRAGMENT_OFFSET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_FLAG_OFFSET,
        /* offset address */         0x0,
        /* length */                 13,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP4_TTL_IP6_HOPLIMIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_L4_SRC_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_L4_DST_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_ESP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_AUTH_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_DEST_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_FRAG_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_ROUTING_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP6_HOP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IGMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_TCP_ECN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_TCP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_TCP_NONZERO_SEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ICMP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ICMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_TELNET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SSH[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_HTTP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_HTTPS[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SNMP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_UNKNOWN_L7[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_VID_RANGE0[] =
{
    {
        /* template field type */    TMPLTE_FIELD_VID_RANG0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_VID_RANGE1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_VID_RANG1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_PORT_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_L4PORT_RANG,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_PORT_RANGE_6290[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0x5,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_L4PORT_RANG,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP_RANGE_6290[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_LEN_RANG,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_LEN_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0x5,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_LEN_RANGE_6290[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_LEN_RANG,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR_VALID_MSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_L2_CRC_ERROR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IP4_CHKSUM_ERROR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR0[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR3[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR5[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR6[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR7[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR8[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR9[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR10[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FIELD_SELECTOR11[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x0,
        /* length */                 5,
        /* data offset */            0x30
    },
    {
        /* template field type */    TMPLTE_FIELD_DPM2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20
    },
    {
        /* template field type */    TMPLTE_FIELD_DPM1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_DPM0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DPMM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPMMASK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DPN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0xa,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_UCAST_DA[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_MCAST_DA[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_SA_LUT_RESULT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DA_LUT_RESULT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_NONZERO_DPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x5,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_L2_DPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L2DPM3,
        /* offset address */         0x0,
        /* length */                 5,
        /* data offset */            0x30
    },
    {
        /* template field type */    TMPLTE_FIELD_L2DPM2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20
    },
    {
        /* template field type */    TMPLTE_FIELD_L2DPM1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_L2DPM0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_L2_DPN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L2DPM3,
        /* offset address */         0xa,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_ATTACK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L2DPM3,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_DP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L2DPM3,
        /* offset address */         0x5,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_INT_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IVLAN,
        /* offset address */         0xc,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IVID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IVLAN,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_OVID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OVLAN,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_FWD_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IGR_ACL_DROP_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xf,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IGR_ACL_COPY_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IGR_ACL_REDIRECT_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fieldLocation_t DAL_CYPRESS_FIELD_IGR_ACL_ROUTING_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_cypress_acl_fixField_t dal_cypress_acl_igrFixField_list[] =
{
    {   /* data field       */  CYPRESS_IACL_TIDtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_CYPRESS_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  CYPRESS_IACL_FRAME_TYPE_L2tf,
        /* mask field name  */  CYPRESS_IACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_CYPRESS_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  CYPRESS_IACL_ITAG_EXISTtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_CYPRESS_FIELD_ITAG_EXIST
    },
    {   /* data field       */  CYPRESS_IACL_OTAG_EXISTtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_CYPRESS_FIELD_OTAG_EXIST
    },
    {   /* data field       */  CYPRESS_IACL_ITAG_FMTtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_CYPRESS_FIELD_ITAG_FMT
    },
    {   /* data field       */  CYPRESS_IACL_OTAG_FMTtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_CYPRESS_FIELD_OTAG_FMT
    },
    {   /* data field       */  CYPRESS_IACL_FRAME_TYPEtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_CYPRESS_FIELD_FRAME_TYPE
    },
    {   /* data field       */  CYPRESS_IACL_FRAME_TYPE_L4tf,
        /* mask field name  */  CYPRESS_IACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_CYPRESS_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  CYPRESS_IACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_CYPRESS_IP_NONZERO_OFFSET
    },
    {   /* data field       */  CYPRESS_IACL_SWITCHMACtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_SWITCHMACtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_SWITCHMAC,
        /* field pointer    */  DAL_CYPRESS_SWITCHMAC
    },
    {   /* data field       */  CYPRESS_IACL_MGNT_VLANtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_CYPRESS_MGNT_VLAN
    },
    {   /* data field       */  CYPRESS_IACL_SPNtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_SPNtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_SPN,
        /* field pointer    */  DAL_CYPRESS_FIELD_SPN
    },
    {   /* data field       */  CYPRESS_IACL_SPMMASK_FIXtf,
        /* mask field name  */  CYPRESS_IACL_BMSK_SPMMASK_FIXtf,
        /* position         */  22,
        /* field name       */  USER_FIELD_SPMM_0_1,
        /* field pointer    */  DAL_CYPRESS_FIELD_SPMMASK_FIX
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_cypress_acl_igrFixField_list */

dal_cypress_acl_fixField_t dal_cypress_acl_egrFixField_list[] =
{
    {   /* data field       */  CYPRESS_EACL_TIDtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_CYPRESS_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  CYPRESS_EACL_FRAME_TYPE_L2tf,
        /* mask field name  */  CYPRESS_EACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_CYPRESS_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  CYPRESS_EACL_ITAG_EXISTtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_CYPRESS_FIELD_ITAG_EXIST
    },
    {   /* data field       */  CYPRESS_EACL_OTAG_EXISTtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_CYPRESS_FIELD_OTAG_EXIST
    },
    {   /* data field       */  CYPRESS_EACL_ITAG_FMTtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_CYPRESS_FIELD_ITAG_FMT
    },
    {   /* data field       */  CYPRESS_EACL_OTAG_FMTtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_CYPRESS_FIELD_OTAG_FMT
    },
    {   /* data field       */  CYPRESS_EACL_FRAME_TYPEtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_CYPRESS_FIELD_FRAME_TYPE
    },
    {   /* data field       */  CYPRESS_EACL_FRAME_TYPE_L4tf,
        /* mask field name  */  CYPRESS_EACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_CYPRESS_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  CYPRESS_EACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_CYPRESS_IP_NONZERO_OFFSET
    },
    {   /* data field       */  CYPRESS_EACL_SWITCHMACtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_SWITCHMACtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_SWITCHMAC,
        /* field pointer    */  DAL_CYPRESS_SWITCHMAC
    },
    {   /* data field       */  CYPRESS_EACL_MGNT_VLANtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_CYPRESS_MGNT_VLAN
    },
    {   /* data field       */  CYPRESS_EACL_SPNtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_SPNtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_SPN,
        /* field pointer    */  DAL_CYPRESS_FIELD_SPN
    },
    {   /* data field       */  CYPRESS_EACL_SPMMASK_FIXtf,
        /* mask field name  */  CYPRESS_EACL_BMSK_SPMMASK_FIXtf,
        /* position         */  22,
        /* field name       */  USER_FIELD_SPMM_0_1,
        /* field pointer    */  DAL_CYPRESS_FIELD_SPMMASK_FIX
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_cypress_acl_egrFixField_list */

dal_cypress_acl_entryField_t dal_cypress_acl_field_list_6290[] =
{
    {   /* field name    */           USER_FIELD_PORT_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_PORT_RANGE_6290
    },
    {   /* field name    */           USER_FIELD_IP_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP_RANGE_6290
    },
    {   /* field name    */           USER_FIELD_LEN_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_LEN_RANGE_6290
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};

dal_cypress_acl_entryField_t dal_cypress_acl_field_list[] =
{
    {   /* field name    */           USER_FIELD_SPMM,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_SPMM
    },
    {   /* field name    */           USER_FIELD_SPM,
        /* field number  */           4,
        /* field pointer */           DAL_CYPRESS_FIELD_SPM
    },
    {   /* field name    */           USER_FIELD_DMAC,
        /* field number  */           3,
        /* field pointer */           DAL_CYPRESS_FIELD_DMAC
    },
    {   /* field name    */           USER_FIELD_SMAC,
        /* field number  */           3,
        /* field pointer */           DAL_CYPRESS_FIELD_SMAC
    },
    {   /* field name    */           USER_FIELD_ETHERTYPE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_ETHERTYPE
    },
    {   /* field name    */           USER_FIELD_OTAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_OTAG_PRI
    },
    {   /* field name    */           USER_FIELD_DEI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_DEI_VALUE
    },
    {   /* field name    */           USER_FIELD_OTAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_OTAG_VID
    },
    {   /* field name    */           USER_FIELD_ITAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_ITAG_PRI
    },
    {   /* field name    */           USER_FIELD_CFI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_CFI_VALUE
    },
    {   /* field name    */           USER_FIELD_ITAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_ITAG_VID
    },
    {   /* field name    */           USER_FIELD_ETAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_ETAG_EXIST
    },
    {   /* field name    */           USER_FIELD_ARPOPCODE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_ARPOPCODE
    },
    {   /* field name    */           USER_FIELD_OMPLS_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_OMPLS_LABEL
    },
    {   /* field name    */           USER_FIELD_OMPLS_EXP,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_OMPLS_EXP
    },
    {   /* field name    */           USER_FIELD_OMPLS_LABEL_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_OMPLS_LABEL_EXIST
    },
    {   /* field name    */           USER_FIELD_IMPLS_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_IMPLS_LABEL
    },
    {   /* field name    */           USER_FIELD_IMPLS_EXP,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IMPLS_EXP
    },
    {   /* field name    */           USER_FIELD_IMPLS_LABEL_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IMPLS_LABEL_EXIST
    },
    {   /* field name    */           USER_FIELD_IP4_SIP,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_IP4_SIP
    },
    {   /* field name    */           USER_FIELD_IP4_DIP,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_IP4_DIP
    },
    {   /* field name    */           USER_FIELD_IP6_SIP,
        /* field number  */           8,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_SIP
    },
    {   /* field name    */           USER_FIELD_IP6_DIP,
        /* field number  */           8,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_DIP
    },
    {   /* field name    */           USER_FIELD_IP4TOS_IP6TC,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP4TOS_IP6TC
    },
    {   /* field name    */           USER_FIELD_IP_DSCP,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IPDSCP
    },
    {   /* field name    */           USER_FIELD_IP4PROTO_IP6NH,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP4PROTO_IP6NH
    },
    {   /* field name    */           USER_FIELD_IP_FLAG,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_IP_FLAG
    },
    {   /* field name    */           USER_FIELD_IP_FRAGMENT_OFFSET,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP_FRAGMENT_OFFSET
    },
    {   /* field name    */           USER_FIELD_IP4_TTL_IP6_HOPLIMIT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP4_TTL_IP6_HOPLIMIT
    },
    {   /* field name    */           USER_FIELD_L4_SRC_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_L4_SRC_PORT
    },
    {   /* field name    */           USER_FIELD_L4_DST_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_L4_DST_PORT
    },
    {   /* field name    */           USER_FIELD_IP6_ESP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_ESP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_AUTH_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_AUTH_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_DEST_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_DEST_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_FRAG_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_FRAG_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_ROUTING_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_ROUTING_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_HOP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP6_HOP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IGMP_TYPE,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_IGMP_TYPE
    },
    {   /* field name    */           USER_FIELD_TCP_ECN,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_TCP_ECN
    },
    {   /* field name    */           USER_FIELD_TCP_FLAG,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_TCP_FLAG
    },
    {   /* field name    */           USER_FIELD_TCP_NONZEROSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_TCP_NONZERO_SEQ
    },
    {   /* field name    */           USER_FIELD_ICMP_CODE,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_ICMP_CODE
    },
    {   /* field name    */           USER_FIELD_ICMP_TYPE,
        /* field number  */           2,
        /* field pointer */           DAL_CYPRESS_FIELD_ICMP_TYPE
    },
    {   /* field name    */           USER_FIELD_TELNET,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_TELNET
    },
    {   /* field name    */           USER_FIELD_SSH,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_SSH
    },
    {   /* field name    */           USER_FIELD_HTTP,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_HTTP
    },
    {   /* field name    */           USER_FIELD_HTTPS,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_HTTPS
    },
    {   /* field name    */           USER_FIELD_SNMP,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_SNMP
    },
    {   /* field name    */           USER_FIELD_UNKNOWN_L7,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_UNKNOWN_L7
    },
    {   /* field name    */           USER_FIELD_VID_RANGE0,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_VID_RANGE0
    },
    {   /* field name    */           USER_FIELD_VID_RANGE1,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_VID_RANGE1
    },
    {   /* field name    */           USER_FIELD_PORT_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_PORT_RANGE
    },
    {   /* field name    */           USER_FIELD_IP_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP_RANGE
    },
    {   /* field name    */           USER_FIELD_LEN_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_LEN_RANGE
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR_VALID_MSK,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR_VALID_MSK
    },
    {   /* field name    */           USER_FIELD_L2_CRC_ERROR,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_L2_CRC_ERROR
    },
    {   /* field name    */           USER_FIELD_IP4_CHKSUM_ERROR,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IP4_CHKSUM_ERROR
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR0,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR0
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR1,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR1
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR2,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR2
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR3,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR3
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR4,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR4
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR5,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR5
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR6,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR6
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR7,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR7
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR8,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR8
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR9,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR9
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR10,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR10
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR11,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FIELD_SELECTOR11
    },
    {   /* field name    */           USER_FIELD_DPM,
        /* field number  */           4,
        /* field pointer */           DAL_CYPRESS_FIELD_DPM
    },
    {   /* field name    */           USER_FIELD_DPMM,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_DPMM
    },
    {   /* field name    */           USER_FIELD_DPN,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_DPN
    },
    {   /* field name    */           USER_FIELD_UCAST_DA,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_UCAST_DA
    },
    {   /* field name    */           USER_FIELD_MCAST_DA,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_MCAST_DA
    },
    {   /* field name    */           USER_FIELD_SA_LUT_RESULT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_SA_LUT_RESULT
    },
    {   /* field name    */           USER_FIELD_DA_LUT_RESULT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_DA_LUT_RESULT
    },
    {   /* field name    */           USER_FIELD_NONZERO_DPM,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_NONZERO_DPM
    },
    {   /* field name    */           USER_FIELD_L2_DPM,
        /* field number  */           4,
        /* field pointer */           DAL_CYPRESS_FIELD_L2_DPM
    },
    {   /* field name    */           USER_FIELD_L2_DPN,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_L2_DPN
    },
    {   /* field name    */           USER_FIELD_ATTACK,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_ATTACK
    },
    {   /* field name    */           USER_FIELD_DP,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_DP
    },
    {   /* field name    */           USER_FIELD_INT_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_INT_PRI
    },
    {   /* field name    */           USER_FIELD_IVID,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IVID
    },
    {   /* field name    */           USER_FIELD_OVID,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_OVID
    },
    {   /* field name    */           USER_FIELD_FWD_VID,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_FWD_VID
    },
    {   /* field name    */           USER_FIELD_IGR_ACL_DROP_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IGR_ACL_DROP_HIT
    },
    {   /* field name    */           USER_FIELD_IGR_ACL_COPY_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IGR_ACL_COPY_HIT
    },
    {   /* field name    */           USER_FIELD_IGR_ACL_REDIRECT_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IGR_ACL_REDIRECT_HIT
    },
    {   /* field name    */           USER_FIELD_IGR_ACL_ROUTING_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_CYPRESS_FIELD_IGR_ACL_ROUTING_HIT
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};

dal_cypress_acl_templateField_t dal_cypress_template_field_list[] =
{
    {   /* field type       */      TMPLTE_FIELD_NONE,
        /* physical field ID*/      0x3f,/* invalid phsic id */
        /* valid field location */  0,
    },
    {   /* field type           */  TMPLTE_FIELD_SPMMASK,
        /* physical field ID    */  0,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SPM0,
        /* physical field ID*/      1,
        /* valid field location */  ((1 << 0) | (1 << 4) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_SPM1,
        /* physical field ID*/      2,
        /* valid field location */  ((1 << 1) | (1 << 5) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_SPM2,
        /* physical field ID*/      3,
        /* valid field location */  ((1 << 2) | (1 << 6) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_SPM3,
        /* physical field ID*/      4,
        /* valid field location */  ((1 << 3) | (1 << 7) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC0,
        /* physical field ID*/      5,
        /* valid field location */  ((1 << 0) | (1 << 3) | (1 << 6) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC1,
        /* physical field ID*/      6,
        /* valid field location */  ((1 << 1) | (1 << 4) | (1 << 7) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC2,
        /* physical field ID*/      7,
        /* valid field location */  ((1 << 2) | (1 << 5) | (1 << 8) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC0,
        /* physical field ID*/      8,
        /* valid field location */  ((1 << 0) | (1 << 3) | (1 << 6) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC1,
        /* physical field ID*/      9,
        /* valid field location */  ((1 << 1) | (1 << 4) | (1 << 7) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC2,
        /* physical field ID*/      10,
        /* valid field location */  ((1 << 2) | (1 << 5) | (1 << 8) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_ETHERTYPE,
        /* physical field ID*/      11,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_OTAG,
        /* physical field ID*/      13,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ITAG,
        /* physical field ID*/      14,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SIP0,
        /* physical field ID*/      15,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP1,
        /* physical field ID*/      16,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP0,
        /* physical field ID*/      17,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP1,
        /* physical field ID*/      18,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_IP_TOS_PROTO,
        /* physical field ID*/      19,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_IP_FLAG_OFFSET,
        /* physical field ID*/      20,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L4_SPORT,
        /* physical field ID*/      21,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L4_DPORT,
        /* physical field ID*/      22,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L34_HEADER,
        /* physical field ID*/      23,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ICMP_IGMP,
        /* physical field ID*/      24,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_VID_RANG0,
        /* physical field ID*/      25,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_VID_RANG1,
        /* physical field ID*/      26,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_IP_L4PORT_RANG,
        /* physical field ID*/      27,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_IP_LEN_RANG,
        /* physical field ID*/      27,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* physical field ID*/      28,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* physical field ID*/      29,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* physical field ID*/      30,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* physical field ID*/      31,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* physical field ID*/      32,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* physical field ID*/      33,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* physical field ID*/      34,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* physical field ID*/      35,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP2,
        /* physical field ID*/      35,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* physical field ID*/      36,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP3,
        /* physical field ID*/      36,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* physical field ID*/      37,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP4,
        /* physical field ID*/      37,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* physical field ID*/      38,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP5,
        /* physical field ID*/      38,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* physical field ID*/      39,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP6,
        /* physical field ID*/      39,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* physical field ID*/      40,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP7,
        /* physical field ID*/      40,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_OLABEL,
        /* physical field ID*/      41,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ILABEL,
        /* physical field ID*/      42,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_OILABEL,
        /* physical field ID*/      43,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DPMMASK,
        /* physical field ID*/      44,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DPM0,
        /* physical field ID*/      45,
        /* valid field location */  ((1 << 0) | (1 << 4) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_DPM1,
        /* physical field ID*/      46,
        /* valid field location */  ((1 << 1) | (1 << 5) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_DPM2,
        /* physical field ID*/      47,
        /* valid field location */  ((1 << 2) | (1 << 6) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_DPM3,
        /* physical field ID*/      48,
        /* valid field location */  ((1 << 3) | (1 << 7) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_L2DPM0,
        /* physical field ID*/      49,
        /* valid field location */  ((1 << 0) | (1 << 4) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_L2DPM1,
        /* physical field ID*/      50,
        /* valid field location */  ((1 << 1) | (1 << 5) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_L2DPM2,
        /* physical field ID*/      51,
        /* valid field location */  ((1 << 2) | (1 << 6) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_L2DPM3,
        /* physical field ID*/      52,
        /* valid field location */  ((1 << 3) | (1 << 7) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_IVLAN,
        /* physical field ID*/      53,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_OVLAN,
        /* physical field ID*/      54,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FWD_VID,
        /* physical field ID*/      55,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DIP2,
        /* physical field ID*/      56,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DIP3,
        /* physical field ID*/      57,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DIP4,
        /* physical field ID*/      58,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DIP5,
        /* physical field ID*/      59,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DIP6,
        /* physical field ID*/      60,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DIP7,
        /* physical field ID*/      61,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_END,
        /* physical field ID*/      0xFF,
        /* valid field location */  0
    },
};

/*
 * Macro Declaration
 */

/* semaphore handling */
#define ACL_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(acl_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_ACL), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define ACL_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(acl_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_ACL), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define DAL_CYPRESS_ACL_INDEX_CHK(UINT,TYPE,INDEX)    \
do {\
    uint32  partition;\
    RT_PARAM_CHK((INDEX >= HAL_MAX_NUM_OF_PIE_FILTER_ID(UINT)), RT_ERR_ENTRY_INDEX);\
    dal_cypress_acl_partition_get(UINT, &partition);\
    if (TYPE == ACL_PHASE_IGR_ACL && (INDEX >= partition*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT)))\
        return RT_ERR_ENTRY_INDEX;\
    if (TYPE == ACL_PHASE_EGR_ACL && (INDEX >= (HAL_MAX_NUM_OF_PIE_BLOCK(UINT)-partition)*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT)))\
        return RT_ERR_ENTRY_INDEX;\
} while(0)

#define DAL_CYPRESS_ACL_ENTRYNUM(UINT,TYPE,ENTRYNUM)    \
do {\
    uint32  partition;\
    dal_cypress_acl_partition_get(UINT, &partition);\
    if (TYPE == ACL_PHASE_IGR_ACL)\
        ENTRYNUM = partition*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT);\
    if (TYPE == ACL_PHASE_EGR_ACL)\
        ENTRYNUM = (HAL_MAX_NUM_OF_PIE_BLOCK(UINT)-partition)*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT);\
} while(0)

#define DAL_CYPRESS_ACL_FIELD_TYPE_CHK(UINT,TYPE)                                   \
do {                                                                                \
    uint32  i;                                                                      \
    RT_PARAM_CHK((TYPE >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);                  \
    for (i = 0; dal_cypress_acl_igrFixField_list[i].type != USER_FIELD_END; ++i)    \
    {                                                                               \
        if (TYPE == dal_cypress_acl_igrFixField_list[i].type)                       \
            break;                                                                  \
    }                                                                               \
    if (dal_cypress_acl_igrFixField_list[i].type != USER_FIELD_END)                 \
        break;                                                                      \
    for (i = 0; dal_cypress_acl_field_list[i].type != USER_FIELD_END; i++)          \
    {                                                                               \
        if (TYPE == dal_cypress_acl_field_list[i].type)                             \
            break;                                                                  \
    }                                                                               \
    if (dal_cypress_acl_field_list[i].type == USER_FIELD_END)                       \
        return RT_ERR_ACL_FIELD_TYPE;                                               \
} while(0)

/* translate entry index from logical to physical for egress ACL */
#define DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(UINT,LOG_INDEX,PHY_INDEX)    \
do {\
    uint32  partition;\
    RT_PARAM_CHK((LOG_INDEX >= HAL_MAX_NUM_OF_PIE_FILTER_ID(UINT)), RT_ERR_ENTRY_INDEX);\
    dal_cypress_acl_partition_get(UINT, &partition);\
    PHY_INDEX = LOG_INDEX+(partition*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT));\
} while(0)

#define DAL_CYPRESS_ACL_BLOCK_PWR_CHK(UINT,ENTRY_INDEX)    \
do {\
    uint32 blk_idx;\
    rtk_enable_t enable;\
    blk_idx = ENTRY_INDEX/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT);\
    dal_cypress_acl_blockPwrEnable_get(UINT, blk_idx, &enable);\
    if (DISABLED == enable)\
        return RT_ERR_ACL_BLOCK_POWER;\
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_validate
 * Description:
 *      Check the field if validate for entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type)
{
    acl_entry_t             buf;
    int32                   ret;
    uint8                   data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8                   mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,entry_idx=%d,type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

    if ((ret = dal_cypress_acl_ruleEntryField_get(unit, phase, entry_idx,
            (uint8 *)&buf, type, data, mask)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_acl_ruleEntryField_validate */

 /* Function Name:
 *      dal_cypress_aclMapper_init
 * Description:
 *      Hook acl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook acl module before calling any acl APIs.
 */
int32
dal_cypress_aclMapper_init(dal_mapper_t *pMapper)
{

    pMapper->acl_init = dal_cypress_acl_init;
    pMapper->acl_partition_get = dal_cypress_acl_partition_get;
    pMapper->acl_partition_set = dal_cypress_acl_partition_set;
    pMapper->acl_templateFieldIntentVlanTag_get = dal_cypress_acl_templateFieldIntentVlanTag_get;
    pMapper->acl_templateFieldIntentVlanTag_set = dal_cypress_acl_templateFieldIntentVlanTag_set;
    pMapper->acl_rangeCheckDstPort_get = dal_cypress_acl_rangeCheckDstPort_get;
    pMapper->acl_rangeCheckDstPort_set = dal_cypress_acl_rangeCheckDstPort_set;
    pMapper->acl_blockResultMode_get = dal_cypress_acl_blockResultMode_get;
    pMapper->acl_blockResultMode_set = dal_cypress_acl_blockResultMode_set;
    pMapper->acl_ruleHitIndication_get = dal_cypress_acl_ruleHitIndication_get;
    pMapper->acl_ruleHitIndicationMask_get = dal_cypress_acl_ruleHitIndicationMask_get;
    pMapper->acl_blockPwrEnable_get = dal_cypress_acl_blockPwrEnable_get;
    pMapper->acl_blockPwrEnable_set = dal_cypress_acl_blockPwrEnable_set;
    pMapper->pie_blockLookupEnable_get = dal_cypress_acl_blockLookupEnable_get;
    pMapper->pie_blockLookupEnable_set = dal_cypress_acl_blockLookupEnable_set;
    pMapper->acl_ruleEntryFieldSize_get = dal_cypress_acl_ruleEntryFieldSize_get;
    pMapper->acl_ruleEntrySize_get = dal_cypress_acl_ruleEntrySize_get;
    pMapper->acl_ruleValidate_get = dal_cypress_acl_ruleValidate_get;
    pMapper->acl_ruleValidate_set = dal_cypress_acl_ruleValidate_set;
    pMapper->acl_ruleEntry_read = dal_cypress_acl_ruleEntry_read;
    pMapper->acl_ruleEntry_write = dal_cypress_acl_ruleEntry_write;
    pMapper->acl_ruleEntryField_get = dal_cypress_acl_ruleEntryField_get;
    pMapper->acl_ruleEntryField_set = dal_cypress_acl_ruleEntryField_set;
    pMapper->acl_ruleEntryField_read = dal_cypress_acl_ruleEntryField_read;
    pMapper->acl_ruleEntryField_write = dal_cypress_acl_ruleEntryField_write;
    pMapper->acl_ruleEntryField_check = dal_cypress_acl_ruleEntryField_check;
    pMapper->acl_ruleOperation_get = dal_cypress_acl_ruleOperation_get;
    pMapper->acl_ruleOperation_set = dal_cypress_acl_ruleOperation_set;
    pMapper->acl_ruleAction_get = dal_cypress_acl_ruleAction_get;
    pMapper->acl_ruleAction_set = dal_cypress_acl_ruleAction_set;
    pMapper->acl_rule_del = dal_cypress_acl_rule_del;
    pMapper->acl_rule_move = dal_cypress_acl_rule_move;
    pMapper->acl_templateSelector_get = dal_cypress_acl_templateSelector_get;
    pMapper->acl_templateSelector_set = dal_cypress_acl_templateSelector_set;
    pMapper->pie_template_get = dal_cypress_acl_template_get;
    pMapper->pie_template_set = dal_cypress_acl_template_set;
    pMapper->pie_templateField_check = dal_cypress_acl_templateField_check;
    pMapper->acl_blockGroupEnable_get = dal_cypress_acl_blockGroupEnable_get;
    pMapper->acl_blockGroupEnable_set = dal_cypress_acl_blockGroupEnable_set;
    pMapper->acl_statPktCnt_get = dal_cypress_acl_statPktCnt_get;
    pMapper->acl_statPktCnt_clear = dal_cypress_acl_statPktCnt_clear;
    pMapper->acl_statByteCnt_get = dal_cypress_acl_statByteCnt_get;
    pMapper->acl_statByteCnt_clear = dal_cypress_acl_statByteCnt_clear;
    pMapper->acl_stat_clearAll = dal_cypress_acl_stat_clearAll;
    pMapper->acl_rangeCheckL4Port_get = dal_cypress_acl_rangeCheckL4Port_get;
    pMapper->acl_rangeCheckL4Port_set = dal_cypress_acl_rangeCheckL4Port_set;
    pMapper->acl_rangeCheckVid_get = dal_cypress_acl_rangeCheckVid_get;
    pMapper->acl_rangeCheckVid_set = dal_cypress_acl_rangeCheckVid_set;
    pMapper->pie_rangeCheckIp_get = dal_cypress_acl_rangeCheckIp_get;
    pMapper->pie_rangeCheckIp_set = dal_cypress_acl_rangeCheckIp_set;
    pMapper->acl_rangeCheckSrcPort_get = dal_cypress_acl_rangeCheckSrcPort_get;
    pMapper->acl_rangeCheckSrcPort_set = dal_cypress_acl_rangeCheckSrcPort_set;
    pMapper->acl_rangeCheckPacketLen_get = dal_cypress_acl_rangeCheckPacketLen_get;
    pMapper->acl_rangeCheckPacketLen_set = dal_cypress_acl_rangeCheckPacketLen_set;
    pMapper->pie_fieldSelector_get = dal_cypress_acl_fieldSelector_get;
    pMapper->pie_fieldSelector_set = dal_cypress_acl_fieldSelector_set;
    pMapper->acl_meterMode_get = dal_cypress_acl_meterMode_get;
    pMapper->acl_meterMode_set = dal_cypress_acl_meterMode_set;
    pMapper->pie_meterIncludeIfg_get = dal_cypress_acl_meterIncludeIfg_get;
    pMapper->pie_meterIncludeIfg_set = dal_cypress_acl_meterIncludeIfg_set;
    pMapper->acl_meterBurstSize_get = dal_cypress_acl_meterBurstSize_get;
    pMapper->acl_meterBurstSize_set = dal_cypress_acl_meterBurstSize_set;
    pMapper->pie_meterExceed_get = dal_cypress_acl_meterExceed_get;
    pMapper->pie_meterExceedAggregation_get = dal_cypress_acl_meterExceedAggregation_get;
    pMapper->pie_meterEntry_get = dal_cypress_acl_meterEntry_get;
    pMapper->pie_meterEntry_set = dal_cypress_acl_meterEntry_set;
    pMapper->acl_ruleEntryField_validate = dal_cypress_acl_ruleEntryField_validate;

    return RT_ERR_OK;
}
static int32
_dal_cypress_acl_phy2logTmplteField(uint32 unit, uint32 phy_field_id, rtk_acl_templateFieldType_t *log_field_id);
static int32
_dal_cypress_acl_log2PhyTmplteField(uint32 unit, uint32 field_idx, rtk_acl_templateFieldType_t log_field_id, uint32 *phy_field_id);
static int32 _dal_cypress_acl_rule_del(uint32 unit, rtk_acl_clear_t *pClrIdx);
static int32 _dal_cypress_acl_rule_move(uint32 unit, rtk_acl_move_t *pData);
static int32
_dal_cypress_acl_igrRuleValidate_get(uint32 unit, rtk_acl_id_t entry_idx, uint32 *pValid);
static int32
_dal_cypress_acl_igrRuleValidate_set(uint32 unit, rtk_acl_id_t entry_idx, uint32 valid);
static int32
_dal_cypress_acl_egrRuleValidate_get(uint32 unit, rtk_acl_id_t entry_idx, uint32 *pValid);
static int32
_dal_cypress_acl_egrRuleValidate_set(uint32 unit, rtk_acl_id_t entry_idx, uint32 valid);
static int32
_dal_cypress_acl_igrRuleEntry_read(uint32 unit, rtk_acl_id_t entry_idx, uint8 *pEntry_buffer);
static int32
_dal_cypress_acl_igrRuleEntry_write(uint32 unit, rtk_acl_id_t entry_idx, uint8 *pEntry_buffer);
static int32
_dal_cypress_acl_egrRuleEntry_read(uint32 unit, rtk_acl_id_t entry_idx, uint8 *pEntry_buffer);
static int32
_dal_cypress_acl_egrRuleEntry_write(uint32 unit, rtk_acl_id_t entry_idx, uint8 *pEntry_buffer);

static int32
_dal_cypress_acl_igrRuleEntryField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);
static int32
_dal_cypress_acl_igrRuleEntryField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);
static int32
_dal_cypress_acl_egrRuleEntryField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);
static int32
_dal_cypress_acl_egrRuleEntryField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);
static int32
_dal_cypress_acl_igrRuleOperation_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);
static int32
_dal_cypress_acl_igrRuleOperation_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);
static int32
_dal_cypress_acl_egrRuleOperation_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);
static int32
_dal_cypress_acl_egrRuleOperation_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);
static int32
_dal_cypress_acl_igrRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction);
static int32
_dal_cypress_acl_igrRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction);
static int32
_dal_cypress_acl_egrRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_egrAction_t     *pAction);
static int32
_dal_cypress_acl_egrRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_egrAction_t     *pAction);
static int32 _dal_cypress_acl_igrRule_del(uint32 unit, rtk_acl_clear_t *pClrIdx);
static int32 _dal_cypress_acl_egrRule_del(uint32 unit, rtk_acl_clear_t *pClrIdx);
static int32 _dal_cypress_acl_igrRule_move(uint32 unit, rtk_acl_move_t *pData);
static int32 _dal_cypress_acl_egrRule_move(uint32 unit, rtk_acl_move_t *pData);
static int32 _dal_cypress_acl_init_config(uint32 unit);

static void
_dal_cypress_acl_buf2Field_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint16 *data16, uint16 *mask16,
    uint8 *pData, uint8 *pMask);

static void
_dal_cypress_acl_field2Buf_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint32 data, uint32 mask,
    uint8 *pData, uint8 *pMask);

/* Function Name:
 *      dal_cypress_acl_init
 * Description:
 *      Initialize ACL module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize ACL module before calling any ACL APIs.
 */
int32
dal_cypress_acl_init(uint32 unit)
{
    int32   ret;
    int32   i;
    uint32  j;
    rtk_acl_fieldType_t old_type, new_type;

    RT_INIT_REENTRY_CHK(acl_init[unit]);
    acl_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    acl_sem[unit] = osal_sem_mutex_create();
    if (0 == acl_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    acl_init[unit] = INIT_COMPLETED;

    if ((ret = _dal_cypress_acl_init_config(unit)) != RT_ERR_OK)
    {
        acl_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    for (i = 0; dal_cypress_acl_field_list[i].type != USER_FIELD_END; ++i)
    {
        old_type = dal_cypress_acl_field_list[i].type;
        for (j = 0; dal_cypress_acl_field_list_6290[j].type != USER_FIELD_END; ++j)
        {
            new_type = dal_cypress_acl_field_list_6290[j].type;
            if (new_type == old_type)
            {
                dal_cypress_acl_field_list[i].pField =
                    dal_cypress_acl_field_list_6290[j].pField;
                break;
            }
        }
    }

    if (RTL8396M_CHIP_ID == HWP_CHIP_ID(unit))
    {
        for (i = 0; i < HAL_MAX_NUM_OF_METER_BLOCK(unit); ++i)
        {
            j = 1;
            if ((reg_array_field_write(unit, CYPRESS_METER_RATE_MODE_CTRLr,
                    REG_ARRAY_INDEX_NONE, i, CYPRESS_RATE_MODEf, &j)) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "Init meter %d rate mode fail", i);
            }

            j = METER_LB_BPS_TICK_250M_10G;
            if ((reg_array_field_write(unit, CYPRESS_METER_LB_TICK_TKN_CTRLr,
                    REG_ARRAY_INDEX_NONE, i, CYPRESS_TICK_PERIODf, &j)) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "Init meter %d tick fail", i);
            }

            j = METER_LB_BPS_TOKEN_250M_10G;
            if ((reg_array_field_write(unit, CYPRESS_METER_LB_TICK_TKN_CTRLr,
                    REG_ARRAY_INDEX_NONE, i, CYPRESS_TKNf, &j)) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "Init meter %d token fail", i);
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_cypress_acl_init */

/* Function Name:
 *      dal_cypress_acl_partition_get
 * Description:
 *      Get the acl partition configuration from the specified device.
 * Input:
 *      unit       - unit id
 * Output:
 *      pPartition - pointer buffer of partition value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_partition_get(uint32 unit, uint32 *pPartition)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pPartition, RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, CYPRESS_ACL_CTRLr, CYPRESS_CUTLINEf, pPartition)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pPartition=%d", *pPartition);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_partition_set
 * Description:
 *      Set the acl partition configuration to the specified device.
 * Input:
 *      unit        - unit id
 *      partition   - partition value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OUT_OF_RANGE   - partition value is out of range
 * Note:
 *      None
 */
int32
dal_cypress_acl_partition_set(uint32 unit, uint32 partition)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "unit=%d, partition=%d", unit, partition);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(partition > HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_OUT_OF_RANGE);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, CYPRESS_ACL_CTRLr, CYPRESS_CUTLINEf, &partition)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockPwrEnable_get
 * Description:
 *      Get the acl block power state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - block index is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The rule data are cleared if the block power is disabled.
 */
int32
dal_cypress_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_PS_ACL_PWR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_ACL_PWR_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockPwrEnable_set
 * Description:
 *      Set the acl block power state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      enable      - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - block index is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      The rule data are cleared if the block power is disabled.
 */
int32
dal_cypress_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_PS_ACL_PWR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_ACL_PWR_ENf,
                        &enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if (enable == ENABLED)
    {
        rtk_acl_clear_t clrIdx;

        osal_time_mdelay(10);

        osal_memset(&clrIdx, 0, sizeof(rtk_acl_clear_t));
        clrIdx.start_idx = block_idx * HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
        clrIdx.end_idx = clrIdx.start_idx + HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) - 1;

        if ((ret = _dal_cypress_acl_rule_del(unit, &clrIdx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "clear fail");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockLookupEnable_get
 * Description:
 *      Get the acl block lookup state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - block index is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      1.The rule data are kept regardless of the lookup status.
 *      2.The lookup result is always false if the lookup state is disabled.
 */
int32
dal_cypress_acl_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    ACL_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                        CYPRESS_ACL_BLK_LOOKUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_LUT_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockLookupEnable_set
 * Description:
 *      Set the acl block lookup state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      enable      - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - block index is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      1.The rule data are kept regardless of the lookup status.
 *      2.The lookup result is always false if the lookup state is disabled.
 */
int32
dal_cypress_acl_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_ACL_BLK_LOOKUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_LUT_ENf,
                        &enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntryFieldSize_get
 * Description:
 *      Get the field size of ACL entry.
 * Input:
 *      unit        - unit id
 *      type        - type of entry field
 * Output:
 *      pField_size - field size of ACL entry.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The unit of size is bit.
 */
int32
dal_cypress_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size)
{
    uint32  i;
    uint32  index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d type=%d", unit, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);
    RT_PARAM_CHK((NULL == pField_size), RT_ERR_NULL_POINTER);

    /* Get chip fixed field type */
    for (i = 0; dal_cypress_acl_egrFixField_list[i].type != USER_FIELD_END; i++)
    {
        if (type == dal_cypress_acl_egrFixField_list[i].type)
        {
            index = i;
            break;
        }
    }

    if (dal_cypress_acl_egrFixField_list[i].type != USER_FIELD_END)
    {
        /* Get field size */
        *pField_size = dal_cypress_acl_egrFixField_list[index].pField[0].field_length;
    }
    else
    {
        uint32  field_number;

        /* Get chip specific field type from database */
        for (i = 0; dal_cypress_acl_field_list[i].type != USER_FIELD_END; i++)
        {
            if (type == dal_cypress_acl_field_list[i].type)
            {
                index = i;
                break;
            }
        }

        RT_PARAM_CHK((dal_cypress_acl_field_list[i].type == USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

        field_number = dal_cypress_acl_field_list[index].field_number;
        for (i = 0; i < (sizeof(dal_cypress_acl_field_info_list)/sizeof(dal_cypress_acl_entryFieldInfo_t)); ++i)
        {
            if (type == dal_cypress_acl_field_info_list[i].type)
            {
                field_number = dal_cypress_acl_field_info_list[i].field_number;
                break;
            }
        }

        /* Get field size */
        *pField_size = 0;
        for (i = 0; i < field_number; i++)
        {
            *pField_size += dal_cypress_acl_field_list[index].pField[i].field_length;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pField_size=%d", *pField_size);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntrySize_get
 * Description:
 *      Get the rule entry size of ACL.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 * Output:
 *      pEntry_size - rule entry size of ingress ACL
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit of size is byte.
 */
int32
dal_cypress_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size)
{
    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_size), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    *pEntry_size = 54;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleValidate_get
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 * Output:
 *      valid     - valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleValidate_get(unit, entry_idx, pValid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleValidate_get(unit, entry_idx, pValid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pValid=%d", *pValid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleValidate_set
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 * Output:
 *      valid     - valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 * Note:
 *      None
 */
int32
dal_cypress_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleValidate_set(unit, entry_idx, valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleValidate_set(unit, entry_idx, valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntry_read
 * Description:
 *      Read the entry data from specified ACL entry.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - entry index
 * Output:
 *      pEntry_buffer - data buffer of ACL entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",\
        unit, phase, entry_idx);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleEntry_read(unit, entry_idx, pEntry_buffer)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleEntry_read(unit, entry_idx, pEntry_buffer)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntry_write
 * Description:
 *      Write the entry data to specified ACL entry.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - entry index
 *      pEntry_buffer - data buffer of ACL entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, pEntry_buffer=%x",\
        unit, phase, entry_idx, pEntry_buffer);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleEntry_write(unit, entry_idx, pEntry_buffer)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleEntry_write(unit, entry_idx, pEntry_buffer)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleEntryBufField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint32              *buf,
    uint8               *pData,
    uint8               *pMask)
{
    int32                   ret;
    uint32                  block_idx;
    uint32                  field_idx;
    uint32                  field_number;
    uint32                  field_data = 0;
    uint32                  field_mask = 0;
    uint32                  data_value;
    uint32                  mask_value;
    uint32                  field_match = FALSE;
    uint32                  dal_field_type;
    uint32                  i;
    rtk_acl_template_t      tmplte;
    rtk_acl_templateIdx_t   tmplte_idx;

    osal_memset(pData, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(pMask, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    /* handle fixed field type */
    for (i = 0; dal_cypress_acl_igrFixField_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_cypress_acl_igrFixField_list[i].type)
        {
            field_data = dal_cypress_acl_igrFixField_list[i].data_field;
            field_mask = dal_cypress_acl_igrFixField_list[i].mask_field;

            if ((ret = table_field_get(unit, CYPRESS_IACLt, field_data,
                    &data_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_get(unit, CYPRESS_IACLt, field_mask,
                    &mask_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            *pData = data_value;
            *pMask = mask_value;

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    block_idx = entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    /* find out the binding template of the block */
    osal_memset(&tmplte_idx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_cypress_acl_templateSelector_get(unit, block_idx, &tmplte_idx);
    /* get template fields */
    if ((ret = table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_TIDtf,
                    &data_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_acl_template_t));
    dal_cypress_acl_template_get(unit, tmplte_idx.template_id[data_value], &tmplte);

    /*
     * For 6290 pre-defined template 0, field 2 can be used as inner or outer tag
     * according to ACL_CTRL.TEMLTE_IOTAG_SEL
     */
    if (0 == tmplte_idx.template_id[data_value])
    {
        if (USER_FIELD_OTAG_PRI == type)
            type = USER_FIELD_ITAG_PRI;
        if (USER_FIELD_DEI_VALUE == type)
            type = USER_FIELD_CFI_VALUE;
        if (USER_FIELD_OTAG_VID == type)
            type = USER_FIELD_ITAG_VID;
    }

    /* translate field from RTK superset view to DAL view */
    for (field_idx = 0; dal_cypress_acl_field_list[field_idx].type != USER_FIELD_END; field_idx++)
    {
        if (type == dal_cypress_acl_field_list[field_idx].type)
            break;
    }

    dal_field_type = field_idx;

    /* search template to check all field types */
    field_match = FALSE;
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++)
        {   /* check whether the user field type is pull in template, partial match is also allowed */
            if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx])
            {
                field_match = TRUE;

                field_data = template_iacl_data_field[field_idx];
                field_mask = template_iacl_mask_field[field_idx];

                /* data */
                if ((ret = table_field_get(unit, CYPRESS_IACLt, field_data, &data_value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, CYPRESS_IACLt, field_mask, &mask_value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* find out corresponding data field from field data */
                _dal_cypress_acl_field2Buf_get(unit, dal_field_type, type, field_number, data_value, mask_value, pData, pMask);
            }/* if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) */
        }/* for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++) */
    } /* for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++) */

    /* can't find then return */
    if ( field_match != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pData=%x, pMask=%x", *pData, *pMask);

    return RT_ERR_OK;
}   /* end of _dal_cypress_acl_igrRuleEntryBufField_read */

static int32
_dal_cypress_acl_igrRuleEntryBufField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint32              *buf,
    uint8               *pData,
    uint8               *pMask)
{
    rtk_acl_template_t      tmplte;
    rtk_acl_templateIdx_t   tmplte_idx;
    uint32                  value;
    uint32                  block_idx;
    uint32                  field_idx;
    uint32                  field_number;
    uint32                  field_data = 0;
    uint32                  field_mask = 0;
    uint16                  data_value_16;
    uint16                  mask_value_16;
    uint32                  data_value;
    uint32                  mask_value;
    uint32                  field_match = FALSE;
    uint32                  dal_field_type;
    uint32                  i;
    int32                   ret;

    /* handle fixed field type */
    for (i = 0; dal_cypress_acl_igrFixField_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_cypress_acl_igrFixField_list[i].type)
        {
            data_value = *pData;
            mask_value = *pMask;

            if(USER_FIELD_TEMPLATE_ID == type)
            {
                uint32 field_length;

                field_length = dal_cypress_acl_igrFixField_list[i].pField->field_length;
                mask_value = ((1 << field_length) - 1);
            }

            field_data = dal_cypress_acl_igrFixField_list[i].data_field;
            field_mask = dal_cypress_acl_igrFixField_list[i].mask_field;

            if ((ret = table_field_set(unit, CYPRESS_IACLt, field_data,
                    &data_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_set(unit, CYPRESS_IACLt, field_mask,
                    &mask_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    block_idx = entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    /* find out the binding template of the block */
    osal_memset(&tmplte_idx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_cypress_acl_templateSelector_get(unit, block_idx, &tmplte_idx);
    /* get field 'template ID' to know the template that the entry maps to */
    if ((ret = table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_TIDtf,
                    &data_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_acl_template_t));
    dal_cypress_acl_template_get(unit, tmplte_idx.template_id[data_value], &tmplte);

    /*
     * For 6290 pre-defined template 0, field 2 can be used as inner or outer tag
     * according to ACL_CTRL.TEMLTE_IOTAG_SEL
     */
    if (0 == tmplte_idx.template_id[data_value])
    {
        if (USER_FIELD_OTAG_PRI == type)
            type = USER_FIELD_ITAG_PRI;
        if (USER_FIELD_DEI_VALUE == type)
            type = USER_FIELD_CFI_VALUE;
        if (USER_FIELD_OTAG_VID == type)
            type = USER_FIELD_ITAG_VID;
    }

    /* translate field from RTK superset view to DAL view */
    for (field_idx = 0; dal_cypress_acl_field_list[field_idx].type != USER_FIELD_END; field_idx++)
    {
        if (type == dal_cypress_acl_field_list[field_idx].type)
            break;
    }

    dal_field_type = field_idx;

    /* search template to check all field types */
    field_match = FALSE;
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++)
        {
            /* check whether the user field type is pulled in template, partial match is allowed */
            if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx])
            {
                uint32  field_offset, field_length;

                field_match = TRUE;

                field_offset = dal_cypress_acl_field_list[dal_field_type].pField[field_number].field_offset;
                field_length = dal_cypress_acl_field_list[dal_field_type].pField[field_number].field_length;

                _dal_cypress_acl_buf2Field_get(unit, dal_field_type, type,
                    field_number, &data_value_16, &mask_value_16, pData, pMask);

                field_data = template_iacl_data_field[field_idx];
                field_mask = template_iacl_mask_field[field_idx];

                /* data */
                if ((ret = table_field_get(unit, CYPRESS_IACLt, field_data, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                value &= ~(((1 << field_length)-1) << field_offset);
                value |= data_value_16;

                if ((ret = table_field_set(unit, CYPRESS_IACLt, field_data, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, CYPRESS_IACLt, field_mask, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                value &= ~(((1 << field_length)-1) << field_offset);
                value |= mask_value_16;

                if ((ret = table_field_set(unit, CYPRESS_IACLt, field_mask, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
            }/* if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) */
        } /* for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++) */
    } /* for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++) */

    /* no matched filed in template */
    if (field_match != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    return RT_ERR_OK;
}   /* end of _dal_cypress_acl_igrRuleEntryBufField_write */

static int32
_dal_cypress_acl_egrRuleEntryBufField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint32              *buf,
    uint8               *pData,
    uint8               *pMask)
{
    rtk_acl_template_t      tmplte;
    rtk_acl_templateIdx_t   tmplte_idx;
    uint32                  block_idx;
    uint32                  field_idx;
    uint32                  field_number;
    uint32                  field_data = 0;
    uint32                  field_mask = 0;
    uint32                  data_value;
    uint32                  mask_value;
    uint32                  field_match = FALSE;
    uint32                  dal_field_type;
    uint32                  i;
    int32                   ret;

    osal_memset(pData, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(pMask, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    /* handle fixed field type */
    for (i = 0; dal_cypress_acl_egrFixField_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_cypress_acl_egrFixField_list[i].type)
        {
            field_data = dal_cypress_acl_egrFixField_list[i].data_field;
            field_mask = dal_cypress_acl_egrFixField_list[i].mask_field;

            if ((ret = table_field_get(unit, CYPRESS_EACLt, field_data,
                    &data_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_get(unit, CYPRESS_EACLt, field_mask,
                    &mask_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            *pData = data_value;
            *pMask = mask_value;

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    block_idx = entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    /* find out the binding template of the block */
    osal_memset(&tmplte_idx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_cypress_acl_templateSelector_get(unit, block_idx, &tmplte_idx);
    /* get template fields */
    if ((ret = table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_TIDtf,
                    &data_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_acl_template_t));
    dal_cypress_acl_template_get(unit, tmplte_idx.template_id[data_value], &tmplte);

    /*
     * For 6290 pre-defined template 0, field 2 can be used as inner or outer tag
     * according to ACL_CTRL.TEMLTE_IOTAG_SEL
     */
    if (0 == tmplte_idx.template_id[data_value])
    {
        if (USER_FIELD_OTAG_PRI == type)
            type = USER_FIELD_ITAG_PRI;
        if (USER_FIELD_DEI_VALUE == type)
            type = USER_FIELD_CFI_VALUE;
        if (USER_FIELD_OTAG_VID == type)
            type = USER_FIELD_ITAG_VID;
    }

    /* translate field from RTK superset view to DAL view */
    for (field_idx = 0; dal_cypress_acl_field_list[field_idx].type != USER_FIELD_END; field_idx++)
    {
        if (type == dal_cypress_acl_field_list[field_idx].type)
            break;
    }

    dal_field_type = field_idx;

    /* search template to check all field types */
    field_match = FALSE;
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++)
    {
        for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++)
        {   /* check whether the user field type is pull in template, partial match is also allowed */
            if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx])
            {
                field_match = TRUE;

                field_data = template_eacl_data_field[field_idx];
                field_mask = template_eacl_mask_field[field_idx];

                /* data */
                if ((ret = table_field_get(unit, CYPRESS_EACLt, field_data, &data_value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, CYPRESS_EACLt, field_mask, &mask_value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* find out corresponding data field from field data */
                _dal_cypress_acl_field2Buf_get(unit, dal_field_type, type, field_number, data_value, mask_value, pData, pMask);
            }/* if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) */
        }/* for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++) */
    } /* for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++) */

    /* can't find then return */
    if ( field_match != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pData=%x, pMask=%x", *pData, *pMask);

    return RT_ERR_OK;
}   /* end of _dal_cypress_acl_egrRuleEntryBufField_read */

static int32
_dal_cypress_acl_egrRuleEntryBufField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint32              *buf,
    uint8               *pData,
    uint8               *pMask)
{
    rtk_acl_template_t      tmplte;
    rtk_acl_templateIdx_t   tmplte_idx;
    uint32                  value;
    uint32                  block_idx;
    uint32                  field_idx;
    uint32                  field_number;
    uint32                  field_data = 0;
    uint32                  field_mask = 0;
    uint16                  data_value_16;
    uint16                  mask_value_16;
    uint32                  data_value;
    uint32                  mask_value;
    uint32                  field_match = FALSE;
    uint32                  dal_field_type;
    uint32                  i;
    int32                   ret;

    /* handle fixed field type */
    for (i = 0; dal_cypress_acl_egrFixField_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_cypress_acl_egrFixField_list[i].type)
        {
            data_value = *pData;
            mask_value = *pMask;

            if(USER_FIELD_TEMPLATE_ID == type)
            {
                uint32 field_length;

                field_length = dal_cypress_acl_egrFixField_list[i].pField->field_length;
                mask_value = ((1 << field_length) - 1);
            }

            field_data = dal_cypress_acl_egrFixField_list[i].data_field;
            field_mask = dal_cypress_acl_egrFixField_list[i].mask_field;

            if ((ret = table_field_set(unit, CYPRESS_EACLt, field_data,
                    &data_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_set(unit, CYPRESS_EACLt, field_mask,
                    &mask_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            ACL_SEM_LOCK(unit);
            if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, buf)) != RT_ERR_OK)
            {
                ACL_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }
            ACL_SEM_UNLOCK(unit);

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    block_idx = entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    /* find out the binding template of the block */
    osal_memset(&tmplte_idx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_cypress_acl_templateSelector_get(unit, block_idx, &tmplte_idx);
    /* get field 'template ID' to know the template that the entry maps to */
    if ((ret = table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_TIDtf,
            &data_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_acl_template_t));
    dal_cypress_acl_template_get(unit, tmplte_idx.template_id[data_value], &tmplte);

    /*
     * For 6290 pre-defined template 0, field 2 can be used as inner or outer tag
     * according to ACL_CTRL.TEMLTE_IOTAG_SEL
     */
    if (0 == tmplte_idx.template_id[data_value])
    {
        if (USER_FIELD_OTAG_PRI == type)
            type = USER_FIELD_ITAG_PRI;
        if (USER_FIELD_DEI_VALUE == type)
            type = USER_FIELD_CFI_VALUE;
        if (USER_FIELD_OTAG_VID == type)
            type = USER_FIELD_ITAG_VID;
    }

    /* translate field from RTK superset view to DAL view */
    for (field_idx = 0; dal_cypress_acl_field_list[field_idx].type != USER_FIELD_END; field_idx++)
    {
        if (type == dal_cypress_acl_field_list[field_idx].type)
            break;
    }

    dal_field_type = field_idx;

    /* search template to check all field types */
    field_match = FALSE;
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++)
        {   /* check whether the user field type is pulled in template, partial match is allowed */
            if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx])
            {
                uint32  field_offset, field_length;

                field_match = TRUE;

                field_offset = dal_cypress_acl_field_list[dal_field_type].pField[field_number].field_offset;
                field_length = dal_cypress_acl_field_list[dal_field_type].pField[field_number].field_length;

                _dal_cypress_acl_buf2Field_get(unit, dal_field_type, type,
                    field_number, &data_value_16, &mask_value_16, pData, pMask);

                field_data = template_eacl_data_field[field_idx];
                field_mask = template_eacl_mask_field[field_idx];

                /* data */
                if ((ret = table_field_get(unit, CYPRESS_EACLt, field_data, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                value &= ~(((1 << field_length)-1) << field_offset);
                value |= data_value_16;
                if ((ret = table_field_set(unit, CYPRESS_EACLt, field_data, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, CYPRESS_EACLt, field_mask, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                value &= ~(((1 << field_length)-1) << field_offset);
                value |= mask_value_16;
                if ((ret = table_field_set(unit, CYPRESS_EACLt, field_mask, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
            }/* if (dal_cypress_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) */
        } /* for (field_number = 0; field_number < dal_cypress_acl_field_list[dal_field_type].field_number; field_number++) */
    } /* for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++) */

    /* no matched filed in template */
    if (field_match != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    return RT_ERR_OK;
}   /* end of _dal_cypress_acl_egrRuleEntryBufField_write */

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_get
 * Description:
 *      Get the field data from specified ACL entry buffer.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - ACL entry index
 *      pEntry_buffer - data buffer of ACL entry
 *      type          - field type
 * Output:
 *      pData         - field data
 *      pMask         - field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The API reads the field data/mask from the entry buffer. Use rtk_acl_ruleEntry_read to
 *      read the rule data to the entry buffer.
 */
int32
dal_cypress_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
        if (RT_ERR_OK != (ret = dal_cypress_acl_ruleEntryField_check(unit, phase, type)))
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = _dal_cypress_acl_igrRuleEntryBufField_read(unit, entry_idx,
                type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
        /* translate to physical index */
        DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

        if ((ret = _dal_cypress_acl_egrRuleEntryBufField_read(unit, entry_idx,
                type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_set
 * Description:
 *      Set the field data to specified ACL entry buffer.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - ACL entry index
 *      pEntry_buffer - data buffer of ACL entry
 *      type          - field type
 *      pData         - field data
 *      pMask         - field mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The API writes the field data/mask to the entry buffer. After the fields are configured,
 *      use rtk_acl_ruleEntry_write to write the entry buffer to ASIC at once.
 */
int32
dal_cypress_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
        if (RT_ERR_OK != (ret = dal_cypress_acl_ruleEntryField_check(unit, phase, type)))
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = _dal_cypress_acl_igrRuleEntryBufField_write(unit, entry_idx,
            type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
        /* translate to physical index */
        DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

        if ((ret = _dal_cypress_acl_egrRuleEntryBufField_write(unit, entry_idx,
            type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_read
 * Description:
 *      Read the field data from specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 * Output:
 *      pData     - field data
 *      pMask     - field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if (RT_ERR_OK != (ret = dal_cypress_acl_ruleEntryField_check(unit, phase, type)))
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = _dal_cypress_acl_igrRuleEntryField_read(unit, entry_idx, type, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleEntryField_read(unit, entry_idx, type, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_write
 * Description:
 *      Write the field data to specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 * Output:
 *      pData     - field data
 *      pMask     - field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if (RT_ERR_OK != (ret = dal_cypress_acl_ruleEntryField_check(unit, phase, type)))
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = _dal_cypress_acl_igrRuleEntryField_write(unit, entry_idx, type, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleEntryField_write(unit, entry_idx, type, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_check
 * Description:
 *      Check whether the specified field type is supported on the chip.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      type        - field type to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ACL_PHASE        - invalid ACL phase
 *      RT_ERR_ACL_FIELD_TYPE   - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type)
{
    uint32  i, j, k;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, type=%d",\
            unit, phase, type);

    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);

    if (ACL_PHASE_EGR_ACL == phase)
        return RT_ERR_OK;

    for (i = 0; dal_cypress_acl_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (dal_cypress_acl_field_list[i].type == type)
        {
            for (j = 0; j < dal_cypress_acl_field_list[i].field_number; ++j)
            {
                for (k = 0; template_eacl_list[k] != TMPLTE_FIELD_END; ++k)
                {
                    if (template_eacl_list[k] ==
                            dal_cypress_acl_field_list[i].pField[j].template_field_type)
                        return RT_ERR_ACL_FIELD_TYPE;
                }
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_acl_ruleEntryField_check */

/* Function Name:
 *      dal_cypress_acl_ruleOperation_get
 * Description:
 *      Get ACL rule operation.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 * Output:
 *      pOperation  - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) For reverse operation, valid index is N where N = 0,1,2...
 *      (2) For aggr_1 operation, index must be 2N where N = 0,1,2...
 *      (3) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,2...
 *      (4) For aggregating 4 rules, both aggr_1 and aggr_2 must be enabled.
 */
int32
dal_cypress_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleOperation_get(unit, entry_idx, pOperation)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleOperation_get(unit, entry_idx, pOperation)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleOperation_set
 * Description:
 *      Set ACL rule operation.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 *      pOperation  - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) For reverse operation, valid index is N where N = 0,1,2...
 *      (2) For aggr_1 operation, index must be 2N where N = 0,1,2...
 *      (3) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,2...
 *      (4) For aggregating 4 rules, both aggr_1 and aggr_2 must be enabled.
 */
int32
dal_cypress_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleOperation_set(unit, entry_idx, pOperation)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleOperation_set(unit, entry_idx, pOperation)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleAction_get
 * Description:
 *      Get the ACL rule action configuration.
 * Input:
 *      unit       - unit id
 *      phase      - ACL lookup phase
 *      entry_idx  - ACL entry index
 * Output:
 *      pAction    - action mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleAction_get(unit, entry_idx, &pAction->igr_acl)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleAction_get(unit, entry_idx, &pAction->egr_acl)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleAction_set
 * Description:
 *      Set the ACL rule action configuration.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 *      pAction     - action mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRuleAction_set(unit, entry_idx, &pAction->igr_acl)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRuleAction_set(unit, entry_idx, &pAction->egr_acl)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_ruleHitIndication_get
 * Description:
 *      Get the ACL rule hit indication.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleHitIndication_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              reset,
    uint32              *pIsHit)
{
    int32   ret;
    uint32  grpIdx, offset, grpData;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;
    if (phase == ACL_PHASE_IGR_ACL)
        DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    else
        DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);

    if (phase == ACL_PHASE_EGR_ACL)
    {
        /* translate to physical index */
        DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);
    }

    grpIdx = entry_idx / 32;
    offset = entry_idx % 32;

    ACL_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_ACL_RULE_HIT_INDICATIONr,
                          REG_ARRAY_INDEX_NONE,
                          grpIdx,
                          CYPRESS_RULE_INDICATIONf,
                          &grpData)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((grpData & (1<<offset)) != 0)
        *pIsHit = 1;
    else
        *pIsHit = 0;

    if(TRUE == reset && *pIsHit == 1)
    {
        /* reset the hit bit */
        grpData &= (1<<offset);
        if ((ret = reg_array_field_write(unit,
                              CYPRESS_ACL_RULE_HIT_INDICATIONr,
                              REG_ARRAY_INDEX_NONE,
                              grpIdx,
                              CYPRESS_RULE_INDICATIONf,
                              &grpData)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pIsHit=%d", *pIsHit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_acl_ruleHitIndicationMask_get
 * Description:
 *      Get the ACL rule hit indication bitmask of the specified phase.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      reset       - reset the hit status
 * Output:
 *      pHitMask    - pointer to hit status bitmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_ruleHitIndicationMask_get(uint32 unit, rtk_acl_phase_t phase,
    uint32 reset, rtk_acl_hitMask_t *pHitMask)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phaseBlkCnt = 0;
    
    
    // global hit has 4 register
    // per global hit register has 32 bits
    // per bits is 32 entries

    uint32  blk_max = DAL_CYPRESS_MAX_HITINDICATION_BLK_ENTRY;          // total 32 blocks
    uint32  blk_entry_max = DAL_CYPRESS_MAX_HITINDICATION_BLK_ENTRY * DAL_CYPRESS_ACL_GLB_HIT_GRP_NUM;    // per block has 128 entries
    // every bit of global hit register represents 32 entries
    uint32  glb_hit_entry_num_per_bit = DAL_CYPRESS_ACL_GLB_HIT_ENTRY_PER_BIT;
    // every block has 4 bits in global reigster (128/32=4)
    uint32  glb_hit_bit_num_per_blk = blk_entry_max / glb_hit_entry_num_per_bit;
    // every register address has 32 bits. Then one address has 8 block hit info (32/4=8)
    uint32  glb_hit_blk_num_per_addr = ((32 * glb_hit_entry_num_per_bit) / blk_entry_max);
    // global hit register has 4 addresses (32/8=4)
    uint32  glb_hit_array_max = blk_max / glb_hit_blk_num_per_addr;
    // every register address has 32 bits. So every entry hit register address has 32 entries.
    uint32  entry_hit_num_per_addr = DAL_CYPRESS_ACL_GLB_HIT_ENTRY_PER_BIT;
    uint32  glb_reg_read_idx = glb_hit_array_max;   // record read global register array index
    uint32  glb_hit_addr = 0;   // get global register address by block index
    uint32  blk_entry_hit_addr_base = 0;
    uint32  blk_entry_ofst = 0;
    uint32  entryOfst = 0;
    uint32  logical_entry_idx = 0;
    uint32  partition = 0;
    
    uint32  blk = 0, j = 0, k = 0;
    uint32  startBlk = 0, endBlk = 0;
    rtk_bitmap_t glb_hit_sts = 0;
    rtk_bitmap_t glb_blk_hit_sts = 0;
    rtk_bitmap_t blk_entry_hit_sts = 0;
    
    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);
    
    /* Check arguments */
    RT_PARAM_CHK((NULL == pHitMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;      
    
    if ( (ret = dal_cypress_acl_partition_get(unit, &partition)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (phase == ACL_PHASE_IGR_ACL)
    {
        startBlk = 0;
        endBlk = partition;
    }
    else
    {
        startBlk = partition;
        endBlk = blk_max;
    }

    ACL_SEM_LOCK(unit);


    for (blk = startBlk; blk < endBlk; ++blk)
    {

        // get global register address by block index
        glb_hit_addr = blk / glb_hit_blk_num_per_addr;
        // check global register address if read
        if (glb_reg_read_idx != glb_hit_addr)
        {
            /* get global hit */
            if ((ret = reg_array_field_read(unit,
                            CYPRESS_ACL_GLB_HIT_INDICATIONr,
                            REG_ARRAY_INDEX_NONE,
                            glb_hit_addr,
                            CYPRESS_GLB_INDICATIONf,
                            &glb_hit_sts)) != RT_ERR_OK)
            {
                ACL_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }            
            
            glb_reg_read_idx = glb_hit_addr;
        }
    
        // get global hit status about the block (4 bits)
        glb_blk_hit_sts = (glb_hit_sts >> ((blk % glb_hit_blk_num_per_addr) * glb_hit_bit_num_per_blk) & ((1 << glb_hit_bit_num_per_blk) - 1));
        // if block's global hit status represents HIT, check the entry hit status
        if (glb_blk_hit_sts != 0)
        {
            //physical_entry_base = blk * blk_entry_max;
            // register has 32 bits. get entry hit register address.
            // every block has 4 entry hit addresses (128/32=4)
            blk_entry_hit_addr_base = blk * (blk_entry_max / entry_hit_num_per_addr);
            // check every block's global hit status
            for (j=0; j<glb_hit_bit_num_per_blk; ++j)
            {
                if ((glb_blk_hit_sts >> j & 0x1) == 1)
                {
                    // block has 4 bits global hit status
                    // every bit represents 32 entries in the block
                    blk_entry_ofst = entry_hit_num_per_addr * j;
                    // read the specific block entry hit address
                    if ((ret = reg_array_field_read(unit,
                                    CYPRESS_ACL_RULE_HIT_INDICATIONr,
                                    REG_ARRAY_INDEX_NONE,
                                    blk_entry_hit_addr_base+j,
                                    CYPRESS_RULE_INDICATIONf,
                                    &blk_entry_hit_sts)) != RT_ERR_OK)
                    {
                        ACL_SEM_UNLOCK(unit);
                        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                        return ret;
                    }                                          

                    // check every 32 entries hit status of block
                    for (k=0; k<entry_hit_num_per_addr; ++k)
                    {
                        // get entry index of the block
                        entryOfst = blk_entry_ofst + k;
                        //physical_entry_idx = physical_entry_base + entryOfst;

                        if ((blk_entry_hit_sts >> k) & 0x1)
                        {
                            logical_entry_idx = (phaseBlkCnt * blk_entry_max) + entryOfst;
                            BITMAP_SET(pHitMask->bits, logical_entry_idx);
                        }
                    }

                    if (TRUE == reset)
                    {
                        if ((ret = reg_array_field_write(unit,
                                        CYPRESS_ACL_RULE_HIT_INDICATIONr,
                                        REG_ARRAY_INDEX_NONE,
                                        blk_entry_hit_addr_base+j,
                                        CYPRESS_RULE_INDICATIONf,
                                        &blk_entry_hit_sts)) != RT_ERR_OK)
                        {
                            ACL_SEM_UNLOCK(unit);
                            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                            return ret;
                        }                
                    }

                }
            }
        }
        phaseBlkCnt++;

    }

    ACL_SEM_UNLOCK(unit);
    
    return RT_ERR_OK;

}   /* end of dal_mango_acl_ruleHitIndicationMask_get */


/* Function Name:
 *      dal_cypress_acl_rule_del
 * Description:
 *      Delete the specified ACL rules.
 * Input:
 *      unit    - unit id
 *      phase   - ACL lookup phase
 *      pClrIdx - rule index to clear
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_ACL_PHASE       - invalid ACL phase
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ACL_CLEAR_INDEX - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      Entry fields, operations and actions are all cleared.
 */
int32
dal_cypress_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx)
{
    int32   ret, i, hit;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_ACL_CLEAR_INDEX);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRule_del(unit, pClrIdx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRule_del(unit, pClrIdx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    for (i = pClrIdx->start_idx; i <= pClrIdx->end_idx; ++i)
    {
        if ((ret = dal_cypress_acl_ruleHitIndication_get(unit, phase, i, TRUE, (uint32 *)&hit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rule_move
 * Description:
 *      Move the specified ACL rules.
 * Input:
 *      unit    - unit id
 *      phase   - ACL lookup phase
 *      pData   - movement info
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      1.Entry fields, operations and actions are all moved.
 *      2.The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      3.(move_from + length) and (move_to + length) must <= the number of ACL rule
 */
int32
dal_cypress_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_cypress_acl_igrRule_move(unit, pData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_cypress_acl_egrRule_move(unit, pData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_templateSelector_get
 * Description:
 *      Get the mapping template of specific block.
 * Input:
 *      unit          - unit id
 *      block_idx     - block index
 * Output:
 *      pTemplate_idx - template index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - invalid block index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      block_idx 0-17
 */
int32
dal_cypress_acl_templateSelector_get(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   *pTemplate_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d", unit, block_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((NULL == pTemplate_idx), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_BLK_TMPLTE1f,
                        &pTemplate_idx->template_id[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        CYPRESS_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_BLK_TMPLTE2f,
                        &pTemplate_idx->template_id[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "template1_idx=%d, template2_idx=%d",\
        pTemplate_idx->template_id[0], pTemplate_idx->template_id[1]);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_templateSelector_set
 * Description:
 *      Set the mapping template of specific block.
 * Input:
 *      unit         - unit id
 *      block_idx    - block index
 *      template_idx - template index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX              - invalid block index
 *      RT_ERR_PIE_TEMPLATE_INDEX           - invalid template index
 *      RT_ERR_ACL_TEMPLATE_INCOMPATIBLE    - try to map a ACL block to an incompatible template
 *      RT_ERR_INPUT                        - invalid input parameter
 * Note:
 *      block_idx 0-17, template_idx 0-7
 */
int32
dal_cypress_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx)
{
    int32   ret;
    uint32  i;
    uint32  partition;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, template1_idx=%d, template2_idx=%d", \
        unit, block_idx, template_idx.template_id[0], template_idx.template_id[1]);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    for (i=0; i<HAL_MAX_NUM_OF_PIE_BLOCK_TEMPLATESELECTOR(unit); i++)
    {
        RT_PARAM_CHK((template_idx.template_id[i] >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    }

    dal_cypress_acl_partition_get(unit, &partition);

    ACL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_BLK_TMPLTE1f,
                        &template_idx.template_id[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_BLK_TMPLTE2f,
                        &template_idx.template_id[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_template_get
 * Description:
 *      Get the template content of specific template index.
 * Input:
 *      unit         - unit id
 *      template_id  - template ID
 * Output:
 *      pTemplate    - template content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_template_get(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate)
{
    int32   ret;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, template_id=%d", unit, template_id);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((template_id >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);

    /* Fixed template */
    if ((template_id >= HAL_PIE_USER_TEMPLATE_ID_MIN(unit) &&
        template_id <= HAL_PIE_USER_TEMPLATE_ID_MAX(unit)) == FALSE)
    {
        *pTemplate = dal_cypress_acl_fixedTemplate_6290[template_id];
        return RT_ERR_OK;
    }

    /* User defined template then get value from chip */
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        ACL_SEM_LOCK(unit);
        if ((ret = reg_array_field_read(unit, CYPRESS_ACL_TMPLTE_CTRLr, template_id, field_idx, CYPRESS_TMPLTE_FIELDf, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        ACL_SEM_UNLOCK(unit);

        if ((ret = _dal_cypress_acl_phy2logTmplteField(unit, value, &pTemplate->field[field_idx])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_template_set
 * Description:
 *      Set the template content of specific template index.
 * Input:
 *      unit         - unit id
 *      template_id  - template ID
 *      pTemplate    - template content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_template_set(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate)
{
    int32   ret;
    uint32  field_idx;
    uint32  value;
    rtk_acl_template_t physical_tmplte;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, template_id=%d", unit, template_id);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((template_id < HAL_PIE_USER_TEMPLATE_ID_MIN(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((template_id > HAL_PIE_USER_TEMPLATE_ID_MAX(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);

    /* Check fields */
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        if ((ret = _dal_cypress_acl_log2PhyTmplteField(unit,
                                field_idx,
                                pTemplate->field[field_idx],
                                &physical_tmplte.field[field_idx])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    /* set value to CHIP */
    for (field_idx = 0; field_idx < DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        value = physical_tmplte.field[field_idx];

        ACL_SEM_LOCK(unit);
        if ((ret = reg_array_field_write(unit, CYPRESS_ACL_TMPLTE_CTRLr, template_id, field_idx, CYPRESS_TMPLTE_FIELDf, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        ACL_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_templateField_check
 * Description:
 *      Check whether the specified template field type is supported on the chip.
 * Input:
 *      unit  - unit id
 *      phase - PIE lookup phase
 *      type  - template field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_ACL_PHASE                    - invalid ACL phase
 *      RT_ERR_ACL_FIELD_TYPE               - invalid ACL field type
 * Note:
 *      None
 */
int32
dal_cypress_acl_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_acl_templateFieldType_t type)
{
    uint32  i, k;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    if (phase != PIE_PHASE_VACL && phase != PIE_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if (TMPLTE_FIELD_IP_L4PORT_RANG == type)
        return RT_ERR_ACL_FIELD_TYPE;

    /* Check field type */
    for (i = 0; dal_cypress_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (type == dal_cypress_template_field_list[i].field_type)
            break;
    }

    if (dal_cypress_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_ACL_FIELD_TYPE;
        return ret;
    }

    if (PIE_PHASE_IACL == phase)
        return RT_ERR_OK;

    for (k = 0; template_eacl_list[k] != TMPLTE_FIELD_END; ++k)
    {
        if (template_eacl_list[k] == type)
            return RT_ERR_ACL_FIELD_TYPE;
    }

    return RT_ERR_OK;
}    /* end of dal_cypress_acl_templateField_check */

/* Function Name:
 *      dal_cypress_acl_blockResultMode_get
 * Description:
 *      Get the acl block result mode.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pMode       - block result mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - invalid block index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      If a packet hit multiple rules and the mode is configured to ACL_BLOCK_RESULT_SINGLE, then
 *      the hit result will be the rule with the lowest index.
 */
int32
dal_cypress_acl_blockResultMode_get(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t *pMode)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d", unit, block_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_ACL_BLK_RESULT_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_BLK_RESULT_MULTIf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pMode = ACL_BLOCK_RESULT_SINGLE;
            break;
        case 1:
            *pMode = ACL_BLOCK_RESULT_MULTIPLE;
            break;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pMode=%d", *pMode);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockResultMode_set
 * Description:
 *      Set the acl block result mode.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      mode        - block result mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - invalid block index
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      If a packet hit multiple rules and the mode is configured to ACL_BLOCK_RESULT_SINGLE, then
 *      the hit result will be the rule with the lowest index.
 */
int32
dal_cypress_acl_blockResultMode_set(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t mode)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, mode=%d", unit, block_idx, mode);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((mode >= ACL_BLOCK_RESULT_END), RT_ERR_INPUT);

    switch (mode)
    {
        case ACL_BLOCK_RESULT_SINGLE:
            value = 0;
            break;
        case ACL_BLOCK_RESULT_MULTIPLE:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "block mode error");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_ACL_BLK_RESULT_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        CYPRESS_BLK_RESULT_MULTIf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockGroupEnable_get
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      group_type  - grouping type
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - invalid block index
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      1.If multiple physical blocks are grouped to a logical block,
 *        it only outputs a single hit result and the hit result will be
 *        the entry with lowest index.
 *      2.Group ingress ACL block with egress ACL block is forbidden.
 *      3.For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      4.For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      5.For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      6.For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      7.If multiple grouping types are applied to the same block index, then
 *        the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *        ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 */
int32
dal_cypress_acl_blockGroupEnable_get(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               *pEnable)
{
    int32   ret;
    uint32  value;
    uint32  partition;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, group_type=%d", unit, block_idx, group_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(group_type >= ACL_BLOCK_GROUP_END, RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_2) && ((block_idx % 2) != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_4) && (((block_idx % 4) != 0) || (block_idx + 3 >= HAL_MAX_NUM_OF_PIE_BLOCK(unit))), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_8) && (((block_idx % 8) != 0) || (block_idx + 7 >= HAL_MAX_NUM_OF_PIE_BLOCK(unit))), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_ALL) && (block_idx != 0), RT_ERR_INPUT);
    if ((group_type != ACL_BLOCK_GROUP_2) &&
        (group_type != ACL_BLOCK_GROUP_4) &&
        (group_type != ACL_BLOCK_GROUP_8) &&
        (group_type != ACL_BLOCK_GROUP_ALL))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "parameter aggr_type error");
        return RT_ERR_INPUT;
    }

    /* Aggregate ingress ACL block with egress ACL block is forbidden */
    dal_cypress_acl_partition_get(unit, &partition);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_2) &&
                 ((block_idx < partition) && ((block_idx+1) >= partition)), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_4) &&
                 ((block_idx < partition) && ((block_idx+3) >= partition)), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_8) &&
                 ((block_idx < partition) && ((block_idx+7) >= partition)), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);

    if (group_type == ACL_BLOCK_GROUP_2)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_2f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        value &= (1 << (block_idx/2));
        *pEnable = (value >> (block_idx/2));
    }
    else if (group_type == ACL_BLOCK_GROUP_4)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_4f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        value &= (1 << (block_idx/4));
        *pEnable = (value >> (block_idx/4));
    }
    else if (group_type == ACL_BLOCK_GROUP_8)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_8f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        value &= (1 << (block_idx/8));
        *pEnable = (value >> (block_idx/8));
    }
    else if (group_type == ACL_BLOCK_GROUP_ALL)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_ALLf, pEnable)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_blockGroupEnable_set
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      group_type  - grouping type
 *      enable      - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - invalid block index
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      1.If multiple physical blocks are grouped to a logical block,
 *        it only outputs a single hit result and the hit result will be
 *        the entry with lowest index.
 *      2.Group ingress ACL block with egress ACL block is forbidden.
 *      3.For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      4.For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      5.For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      6.For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      7.If multiple grouping types are applied to the same block index, then
 *        the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *        ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 */
int32
dal_cypress_acl_blockGroupEnable_set(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               enable)
{
    int32   ret;
    uint32  value;
    uint32  partition;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, aggr_type=%d, enable=%d",\
        unit, block_idx, group_type, enable);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(group_type >= ACL_BLOCK_GROUP_END, RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_2) && ((block_idx % 2) != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_4) && (((block_idx % 4) != 0) || (block_idx + 3 >= HAL_MAX_NUM_OF_PIE_BLOCK(unit))), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_8) && (((block_idx % 8) != 0) || (block_idx + 7 >= HAL_MAX_NUM_OF_PIE_BLOCK(unit))), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_ALL) && (block_idx != 0), RT_ERR_INPUT);
    if ((group_type != ACL_BLOCK_GROUP_2) &&
        (group_type != ACL_BLOCK_GROUP_4) &&
        (group_type != ACL_BLOCK_GROUP_8) &&
        (group_type != ACL_BLOCK_GROUP_ALL))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "parameter aggr_type error");
        return RT_ERR_INPUT;
    }

    /* Aggregate ingress ACL block with egress ACL block is forbidden */
    dal_cypress_acl_partition_get(unit, &partition);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_2) &&
                 ((block_idx < partition) && ((block_idx+1) >= partition)), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_4) &&
                 ((block_idx < partition) && ((block_idx+3) >= partition)), RT_ERR_INPUT);
    RT_PARAM_CHK((group_type == ACL_BLOCK_GROUP_8) &&
                 ((block_idx < partition) && ((block_idx+7) >= partition)), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);

    if (group_type == ACL_BLOCK_GROUP_2)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_2f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        value &= ~(1 << (block_idx/2));
        value |= (enable << (block_idx/2));
        if ((ret = reg_field_write(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_2f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }
    else if (group_type == ACL_BLOCK_GROUP_4)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_4f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        value &= ~(1 << (block_idx/4));
        value |= (enable << (block_idx/4));
        if ((ret = reg_field_write(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_4f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }
    else if (group_type == ACL_BLOCK_GROUP_8)
    {
        if ((ret = reg_field_read(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_8f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        value &= ~(1 << (block_idx/8));
        value |= (enable << (block_idx/8));
        if ((ret = reg_field_write(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_8f, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }
    else if (group_type == ACL_BLOCK_GROUP_ALL)
    {
        if ((ret = reg_field_write(unit, CYPRESS_ACL_BLK_GROUP_CTRLr, CYPRESS_BLK_GROUP_ALLf, &enable)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_statPktCnt_get
 * Description:
 *      Get packet-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 * Output:
 *      pPkt_cnt  - pointer buffer of packet count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt)
{
    int32   ret;
    uint32  cnt[2];
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pPkt_cnt), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((log_id >= (HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)), RT_ERR_ENTRY_INDEX);

    /* translate to physical index */
    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_LOGt, (log_id/2), (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_LOGt, CYPRESS_LOG_CNTRtf, cnt, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* get the packet counter from a 64-bit byte counter */
    if (log_id%2 == 1)
        *pPkt_cnt = cnt[1];
    else
        *pPkt_cnt = cnt[0];

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pPkt_cnt=%d", *pPkt_cnt);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_statPktCnt_clear
 * Description:
 *      Set packet-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 *      pkt_cnt   - packet count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      None
 */
int32
dal_cypress_acl_statPktCnt_clear(uint32 unit, uint32 log_id)
{
    int32   ret;
    uint8   msb_word;
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= (HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)), RT_ERR_ENTRY_INDEX);

    osal_memset(&entry, 0x0, sizeof(log_entry_t));/*no used for clear*/

    /* find MSB or LSB word to clear */
    if (log_id%2 == 1)
       msb_word = TRUE;
    else
       msb_word = FALSE;

    /* form the index in ASIC view: Log (Write): ADDR [11:0]={ CLRH, CLRL , Index [9:0] } */
    log_id = log_id/2;
    if (msb_word == TRUE)
        log_id |= (1<<11);
    else
        log_id |= (1<<10);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_statByteCnt_get
 * Description:
 *      Get byte-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 * Output:
 *      pByte_cnt - byte count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt)
{
    int32   ret;
    uint32  cnt[2];
    uint32  *p;
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pByte_cnt), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((log_id >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)), RT_ERR_ENTRY_INDEX);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_LOGt, CYPRESS_LOG_CNTRtf, cnt, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    p = (uint32 *)pByte_cnt;
    p[0] = cnt[1];
    p[1] = cnt[0];

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pByte_cnt=%ull", *pByte_cnt);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_statByteCnt_clear
 * Description:
 *      Reset byte-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 *      byte_cnt  - byte count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      None
 */
int32
dal_cypress_acl_statByteCnt_clear(uint32 unit, uint32 log_id)
{
    int32   ret;
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)), RT_ERR_ENTRY_INDEX);

    osal_memset(&entry, 0x0, sizeof(log_entry_t));/*no used for clear*/

    /* form the index in ASIC view: Log (Write): ADDR [11:0]={ CLRH, CLRL , Index [9:0] } */
    log_id |= (0x3<<10);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_stat_clearAll
 * Description:
 *      Clear all statistic counter for the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      All the packet counters and byte counters are cleared.
 */
int32
dal_cypress_acl_stat_clearAll(uint32 unit)
{
    uint32 ret;
    uint32 i, entry_num;

    entry_num = HAL_MAX_NUM_OF_PIE_COUNTER(unit);

    for (i = 0; i < entry_num; i++)
    {
        if ((ret = dal_cypress_acl_statByteCnt_clear(unit, i)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckL4Port_get
 * Description:
 *      Get the configuration of L4 port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of L4 port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckL4Port_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_L4PORT_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_L4PORT_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_L4PORT_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_L4PORT_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_L4PORT_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_L4PORT_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            pData->l4port_dir = RNGCHK_L4PORT_DIRECTION_SRC;
            break;
        case 1:
            pData->l4port_dir = RNGCHK_L4PORT_DIRECTION_DST;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckL4Port_set
 * Description:
 *      Set the configuration of L4 port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of L4 port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckL4Port_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, reverse=%d, l4port_dir=%d",\
        unit, index, pData->upper_bound, pData->lower_bound, pData->reverse, pData->l4port_dir);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pData->l4port_dir >= RNGCHK_L4PORT_DIRECTION_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->upper_bound > 65535), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > 65535), RT_ERR_INPUT);

    switch (pData->l4port_dir)
    {
        case RNGCHK_L4PORT_DIRECTION_SRC:
            value = 0;
            break;
        case RNGCHK_L4PORT_DIRECTION_DST:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "invalid L4 port type");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_L4PORT_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_L4PORT_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_L4PORT_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_L4PORT_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_L4PORT_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_L4PORT_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckVid_get
 * Description:
 *      Get the configuration of VID range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of VID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckVid_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_VID_UPPERf,
                        &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_VID_LOWERf,
                        &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

     switch (value)
    {
        case 0:
            pData->vid_type = RNGCHK_VID_TYPE_INNER;
            break;
        case 1:
            pData->vid_type = RNGCHK_VID_TYPE_OUTER;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckVid_set
 * Description:
 *      Set the configuration of VID range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckVid_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, reverse=%d, vid_type=%d",\
        unit, index, pData->vid_upper_bound, pData->vid_lower_bound, pData->reverse, pData->vid_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pData->vid_type >= RNGCHK_VID_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > pData->vid_upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_upper_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);

    switch (pData->vid_type)
    {
        case RNGCHK_VID_TYPE_INNER:
            value = 0;
            break;
        case RNGCHK_VID_TYPE_OUTER:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "invalid VID type");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_VID_UPPERf,
                        &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_VID_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_VID_LOWERf,
                        &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckIp_get
 * Description:
 *      Get the configuration of IP range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of IP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1.For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *        index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *        be used together in order to filter a full IPv6 address.
 *      2.For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *        Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 */
int32
dal_cypress_acl_rangeCheckIp_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_ip_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_IP_UPPERf,
                        &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_IP_LOWERf,
                        &pData->ip_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

     switch (value)
    {
        case 0:
            pData->ip_type = RNGCHK_IP_TYPE_IPV4_SRC;
            break;
        case 1:
            pData->ip_type = RNGCHK_IP_TYPE_IPV4_DST;
            break;
        case 2:
            pData->ip_type = RNGCHK_IP_TYPE_IPV6_SRC;
            break;
        case 3:
            pData->ip_type = RNGCHK_IP_TYPE_IPV6_DST;
            break;
        case 4:
            pData->ip_type = RNGCHK_IP_TYPE_IPV6_SRC_SUFFIX;
            break;
        case 5:
            pData->ip_type = RNGCHK_IP_TYPE_IPV6_DST_SUFFIX;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckIp_set
 * Description:
 *      Set the configuration of IP range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of IP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1.For IPv6 range check, index 0/4 represents IP6[31:0], index 1/5 represents IP6[63:32],
 *        index 2/6 represents IP6[95:64], index 3/7 represents IP6[127:96]. Index 0~3/4~7 must
 *        be used together in order to filter a full IPv6 address.
 *      2.For IPv6 suffix range check, index 0/2/4/6 represents IP6[31:0], index 1/3/5/7 represents IP6[63:32].
 *        Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 */
int32
dal_cypress_acl_rangeCheckIp_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_ip_t *pData)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%x, lower_bound=%x, reverse=%d, ip_type=%d",\
        unit, index, pData->ip_upper_bound, pData->ip_lower_bound, pData->reverse, pData->ip_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pData->ip_type >= RNGCHK_IP_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->ip_lower_bound > pData->ip_upper_bound), RT_ERR_INPUT);

    switch (pData->ip_type)
    {
        case RNGCHK_IP_TYPE_IPV4_SRC:
            value = 0;
            break;
        case RNGCHK_IP_TYPE_IPV4_DST:
            value = 1;
            break;
        case RNGCHK_IP_TYPE_IPV6_SRC:
            value = 2;
            break;
        case RNGCHK_IP_TYPE_IPV6_DST:
            value = 3;
            break;
        case RNGCHK_IP_TYPE_IPV6_SRC_SUFFIX:
            value = 4;
            break;
        case RNGCHK_IP_TYPE_IPV6_DST_SUFFIX:
            value = 5;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "invalid IP type");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_IP_UPPERf,
                        &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_IP_LOWERf,
                        &pData->ip_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckSrcPort_get
 * Description:
 *      Get the configuration of source port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of source port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckSrcPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_SPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_SPM_0f,
                        &pData->port_mask.bits[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_SPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_SPM_1f,
                        &pData->port_mask.bits[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckSrcPort_set
 * Description:
 *      Set the configuration of source port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of source port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckSrcPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    int32   ret;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, pmsk0=0x%x, pmsk1=0x%x",\
        unit, index, pData->port_mask.bits[0], pData->port_mask.bits[1]);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RT_ERR_OK != hwp_portmaskValid_Chk(unit, &(pData->port_mask))), RT_ERR_PORT_MASK);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_SPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_SPM_0f,
                        &pData->port_mask.bits[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_SPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_SPM_1f,
                        &pData->port_mask.bits[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckDstPort_get
 * Description:
 *      Get the configuration of destination port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of destination port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckDstPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_DSTPORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_DPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_DPM_0f,
                        &pData->port_mask.bits[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_DPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_DPM_1f,
                        &pData->port_mask.bits[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckDstPort_set
 * Description:
 *      Set the configuration of destination port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of destination port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_rangeCheckDstPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    int32   ret;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, pmsk0=0x%x, pmsk1=0x%x",\
        unit, index,
        RTK_PORTMASK_WORD_GET(pData->port_mask, 0),
        RTK_PORTMASK_WORD_GET(pData->port_mask, 1));

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_DSTPORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RT_ERR_OK != hwp_portmaskValid_Chk(unit, &(pData->port_mask))), RT_ERR_PORT_MASK);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_DPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_DPM_0f,
                        &pData->port_mask.bits[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_DPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_DPM_1f,
                        &pData->port_mask.bits[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckPacketLen_get
 * Description:
 *      Get the configuration of packet length range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet length includes CRC(4Byte)
 */
int32
dal_cypress_acl_rangeCheckPacketLen_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_PKT_LEN_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_PKT_LEN_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_PKTLEN_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_RNG_CHK_PKT_LEN_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_PKTLEN_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_rangeCheckPacketLen_set
 * Description:
 *      Set the configuration of packet length range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Packet length includes CRC(4Byte)
 */
int32
dal_cypress_acl_rangeCheckPacketLen_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    int32   ret;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, lower_bound=%d, reverse=%d",\
        unit, index, pData->upper_bound, pData->lower_bound, pData->reverse);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->upper_bound > 0x3FFF), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > 0x3FFF), RT_ERR_INPUT);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_PKT_LEN_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_REVERSEf,
                        &pData->reverse)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_PKT_LEN_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_PKTLEN_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_RNG_CHK_PKT_LEN_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        CYPRESS_PKTLEN_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_fieldSelector_get
 * Description:
 *      Get the configuration of field selector.
 * Input:
 *      unit   - unit id
 *      fs_idx - field selector index
 * Output:
 *      pFs    - configuration of field selector.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      For 8390, only the first 180B(since DA) of packet can be inspected by field selector.
 *      Thus, (start position + offest) must less than 164B(since DA) in order to grab a
 *      complete 16-bit user define field.
 */
int32
dal_cypress_acl_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_acl_fieldSelector_data_t *pFs)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, fs_idx=%d, pFs.start=%d, pFs.offset=%d", \
        unit, fs_idx, pFs->start, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);

    ACL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        CYPRESS_FMTf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        CYPRESS_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        CYPRESS_OFFSETf,
                        &pFs->offset)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            pFs->start = FS_START_POS_RAW;
            break;
        case 1:
            pFs->start = FS_START_POS_LLC;
            break;
        case 2:
            pFs->start = FS_START_POS_L3;
            break;
        case 3:
            pFs->start = FS_START_POS_ARP;
            break;
        case 4:
            pFs->start = FS_START_POS_IPV4;
            break;
        case 5:
            pFs->start = FS_START_POS_IPV6;
            break;
        case 6:
            pFs->start = FS_START_POS_IP;
            break;
        case 7:
            pFs->start = FS_START_POS_L4;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_fieldSelector_set
 * Description:
 *      Set the configuration of field selector.
 * Input:
 *      unit   - unit id
 *      fs_idx - field selector index
 *      pFs    - configuration of field selector.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      For 8390, only the first 180B(since DA) of packet can be inspected by field selector.
 *      Thus, (start position + offest) must less than 164B(since DA) in order to grab a
 *      complete 16-bit user define field.
 */
int32
dal_cypress_acl_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_acl_fieldSelector_data_t *pFs)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, fs_idx=%d, pFs.start=%d, pFs.offset=%d", \
        unit, fs_idx, pFs->start, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pFs->start >= FS_START_POS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pFs->offset >= 179), RT_ERR_INPUT);/*unable to grab complete 16-bit*/

    switch (pFs->start)
    {
        case FS_START_POS_RAW:
            value = 0;
            break;
        case FS_START_POS_LLC:
            value = 1;
            break;
        case FS_START_POS_L3:
            value = 2;
            break;
        case FS_START_POS_ARP:
            value = 3;
            break;
        case FS_START_POS_IPV4:
            value = 4;
            break;
        case FS_START_POS_IPV6:
            value = 5;
            break;
        case FS_START_POS_IP:
            value = 6;
            break;
        case FS_START_POS_L4:
            value = 7;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "invalid start position");
            return RT_ERR_INPUT;
    }

    ACL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        CYPRESS_FMTf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        CYPRESS_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        CYPRESS_OFFSETf,
                        &pFs->offset)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterMode_get
 * Description:
 *      Get the meter mode of a specific meter block.
 * Input:
 *      unit        - unit id
 *      blockIdx    - meter block ID
 * Output:
 *      pMeterMode  - meter mode:byte based or packet based
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterMode_get(
    uint32  unit,
    uint32  blockIdx,
    rtk_acl_meterMode_t *pMeterMode)
{
    int32 ret;
    uint32 meterMode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, blockIdx=%d", unit, blockIdx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((blockIdx >= HAL_MAX_NUM_OF_METER_BLOCK(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMeterMode), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_METER_MODE_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_METER_MODEf,
                          &meterMode)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if(meterMode == 0)
        *pMeterMode = METER_MODE_BYTE;
    else
        *pMeterMode = METER_MODE_PACKET;

    ACL_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "*pMeterMode=%d", *pMeterMode);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterMode_set
 * Description:
 *      Set the meter mode.
 * Input:
 *      unit        - unit id
 *      blockIdx    - meter block ID
 *      meterMode   - meter mode (byte based or packet based)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterMode_set(
    uint32  unit,
    uint32  blockIdx,
    rtk_acl_meterMode_t meterMode)
{
    int32 ret;
    uint32 meterModeVal;
    uint32 tickPeriod;
    uint32 tokenLen;
    uint32 rateMode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, blockIdx=%d meterMode=%d", unit, blockIdx, meterMode);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((blockIdx >= HAL_MAX_NUM_OF_METER_BLOCK(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((meterMode >= METER_MODE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    /* program value to CHIP*/
    if(meterMode == METER_MODE_BYTE)
        meterModeVal = 0;
    else
        meterModeVal = 1;
    if ((ret = reg_array_field_write(unit,
                          CYPRESS_METER_MODE_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_METER_MODEf,
                          &meterModeVal)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (HWP_8350_FAMILY_ID(unit)) /* 50MHz */
    {
        if (METER_MODE_BYTE == meterMode)
        {
            tickPeriod = METER_LB_BPS_TICK_50M;
            tokenLen = METER_LB_BPS_TOKEN_50M;
            rateMode = 0;
        }
        else
        {
            tickPeriod = METER_LB_PPS_TICK_50M;
            tokenLen = METER_LB_PPS_TOKEN_50M;
            rateMode = 1;
        }
    }
    else /* 250MHz */
    {
        if (METER_MODE_BYTE == meterMode)
        {
            if (RTL8396M_CHIP_ID == HWP_CHIP_ID(unit))
            {
                tickPeriod = METER_LB_BPS_TICK_250M_10G;
                tokenLen = METER_LB_BPS_TOKEN_250M_10G;
                rateMode = 1;
            }
            else
            {
                tickPeriod = METER_LB_BPS_TICK_250M;
                tokenLen = METER_LB_BPS_TOKEN_250M;
                rateMode = 0;
            }
        }
        else
        {
            tickPeriod = METER_LB_PPS_TICK_250M;
            tokenLen = METER_LB_PPS_TOKEN_250M;
            rateMode = 1;
        }
    }

    if ((ret = reg_array_field_write(unit,
                          CYPRESS_METER_RATE_MODE_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_RATE_MODEf,
                          &rateMode)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    dal_cypress_acl_meterTokenRefill_set(unit, blockIdx, tickPeriod, tokenLen);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterTokenRefill_get
 * Description:
 *      Get token refill T/B value of meter block.
 * Input:
 *      unit         - unit id
 *      blockIdx     - block id
 * Output:
 *      pTickPeriod  - pointer to tick period (unit:clock)
 *      pTokenLen    - pointer to token length (unit: byte or packet depends on meter mode)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterTokenRefill_get(
    uint32  unit,
    uint32  blockIdx,
    uint32  *pTickPeriod,
    uint32  *pTokenLen)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, blockIdx=%d", unit, blockIdx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTickPeriod), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTokenLen), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_METER_LB_TICK_TKN_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_TICK_PERIODf,
                          pTickPeriod)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          CYPRESS_METER_LB_TICK_TKN_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_TKNf,
                          pTokenLen)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "*pTickPeriod=%d, *pTokenLen", *pTickPeriod, *pTokenLen);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterTokenRefill_get
 * Description:
 *      Set token refill T/B value of meter block.
 * Input:
 *      unit         - unit id
 *      blockIdx     - block id
 *      pTickPeriod  - tick period (unit:clock)
 *      pTokenLen    - token length (unit: byte or packet depends on meter mode)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterTokenRefill_set(
    uint32  unit,
    uint32  blockIdx,
    uint32  tickPeriod,
    uint32  tokenLen)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, blockIdx=%d tickPeriod=%d tokenLen=%d", unit, blockIdx, tickPeriod, tokenLen);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */


    ACL_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          CYPRESS_METER_LB_TICK_TKN_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_TICK_PERIODf,
                          &tickPeriod)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          CYPRESS_METER_LB_TICK_TKN_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          blockIdx,
                          CYPRESS_TKNf,
                          &tokenLen)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }


    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for meter.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, CYPRESS_METER_GLB_CTRLr, CYPRESS_INCL_PREIFGf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "val:%d", value);

    /* translate to chip value */
    switch (value)
    {
        case 0:
            *pIfg_include = DISABLED;
            break;
        case 1:
            *pIfg_include = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterIncludeIfg_set
 * Description:
 *      Set enable status of includes IFG for meter.
 * Input:
 *      unit        - unit id
 *      ifg_include - enable status of includes IFG
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, ifg_include=%d", unit, ifg_include);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ifg_include >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip value */
    switch (ifg_include)
    {
        case DISABLED:
            value = 0;
            break;
        case ENABLED:
            value = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    ACL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, CYPRESS_METER_GLB_CTRLr, CYPRESS_INCL_PREIFGf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterBurstSize_get
 * Description:
 *      Get the meter burst sizes of a specific meter mode.
 * Input:
 *      unit        - unit id
 *      meterMode   - meter mode
 * Output:
 *      pBurstSize  - pointer to burst sizes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterBurstSize_get(
    uint32              unit,
    rtk_acl_meterMode_t meterMode,
    rtk_acl_meterBurstSize_t  *pBurstSize)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterMode=%d", unit, meterMode);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurstSize), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((meterMode >= METER_MODE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if(METER_MODE_BYTE == meterMode)
    {
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_BYTE_DLB_LB_THR_CTRLr,
                              CYPRESS_BYTE_DLB_LB0_THRf,
                              &pBurstSize->dlb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_BYTE_DLB_LB_THR_CTRLr,
                              CYPRESS_BYTE_DLB_LB1_THRf,
                              &pBurstSize->dlb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_BYTE_SRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_SRTCM_LB0_THRf,
                              &pBurstSize->srtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_BYTE_SRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_SRTCM_LB1_THRf,
                              &pBurstSize->srtcm_ebs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_BYTE_TRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_TRTCM_LB0_THRf,
                              &pBurstSize->trtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_BYTE_TRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_TRTCM_LB1_THRf,
                              &pBurstSize->trtcm_pbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_PKT_DLB_LB_THR_CTRLr,
                              CYPRESS_PKT_DLB_LB0_THRf,
                              &pBurstSize->dlb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_PKT_DLB_LB_THR_CTRLr,
                              CYPRESS_PKT_DLB_LB1_THRf,
                              &pBurstSize->dlb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_PKT_SRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_SRTCM_LB0_THRf,
                              &pBurstSize->srtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_PKT_SRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_SRTCM_LB1_THRf,
                              &pBurstSize->srtcm_ebs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_PKT_TRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_TRTCM_LB0_THRf,
                              &pBurstSize->trtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_read(unit,
                              CYPRESS_METER_PKT_TRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_TRTCM_LB1_THRf,
                              &pBurstSize->trtcm_pbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    ACL_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pBurstSize->dlb_lb0bs=%d, pBurstSize->dlb_lb1bs=%d, \
        pBurstSize->srtcm_cbs=%d, pBurstSize->srtcm_ebs=%d, pBurstSize->trtcm_cbs=%d, \
        pBurstSize->trtcm_pbs=%d", pBurstSize->dlb_lb0bs, pBurstSize->dlb_lb1bs, pBurstSize->srtcm_cbs,
        pBurstSize->srtcm_ebs, pBurstSize->trtcm_cbs, pBurstSize->trtcm_pbs);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterBurstSize_set
 * Description:
 *      Set the meter burst sizes of a specific meter mode.
 * Input:
 *      unit        - unit id
 *      meterMode   - meter mode (byte based or packet based)
 *      pBurstSize  - pointer to burst sizes
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      In meterMode = METER_MODE_BYTE, the (minimum, maximum) of pBurstSize->dlb_lb0bs and
 *      pBurstSize->dlb_lb1bs setting range is (17, 65535).
 */
int32
dal_cypress_acl_meterBurstSize_set(
    uint32              unit,
    rtk_acl_meterMode_t meterMode,
    rtk_acl_meterBurstSize_t  *pBurstSize)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pBurstSize), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterMode=%d, pBurstSize->dlb_lb0bs=%d, \
        pBurstSize->dlb_lb1bs=%d, pBurstSize->srtcm_cbs=%d, pBurstSize->srtcm_ebs=%d, \
        pBurstSize->trtcm_cbs=%d, pBurstSize->trtcm_pbs=%d", unit, meterMode,
        pBurstSize->dlb_lb0bs, pBurstSize->dlb_lb1bs, pBurstSize->srtcm_cbs,
        pBurstSize->srtcm_ebs, pBurstSize->trtcm_cbs, pBurstSize->trtcm_pbs);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterMode >= METER_MODE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->dlb_lb0bs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->dlb_lb1bs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->srtcm_cbs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->srtcm_ebs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->trtcm_cbs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->trtcm_pbs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);

    if(METER_MODE_BYTE == meterMode)
    {
        RT_PARAM_CHK((pBurstSize->dlb_lb0bs < HAL_BURST_SIZE_OF_ACL_METER_MIN(unit)), RT_ERR_INPUT);
        RT_PARAM_CHK((pBurstSize->dlb_lb1bs < HAL_BURST_SIZE_OF_ACL_METER_MIN(unit)), RT_ERR_INPUT);
    }

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if(METER_MODE_BYTE == meterMode)
    {
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_BYTE_DLB_LB_THR_CTRLr,
                              CYPRESS_BYTE_DLB_LB0_THRf,
                              &pBurstSize->dlb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_BYTE_DLB_LB_THR_CTRLr,
                              CYPRESS_BYTE_DLB_LB1_THRf,
                              &pBurstSize->dlb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_BYTE_SRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_SRTCM_LB0_THRf,
                              &pBurstSize->srtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_BYTE_SRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_SRTCM_LB1_THRf,
                              &pBurstSize->srtcm_ebs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_BYTE_TRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_TRTCM_LB0_THRf,
                              &pBurstSize->trtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_BYTE_TRTCM_LB_THR_CTRLr,
                              CYPRESS_BYTE_TRTCM_LB1_THRf,
                              &pBurstSize->trtcm_pbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_PKT_DLB_LB_THR_CTRLr,
                              CYPRESS_PKT_DLB_LB0_THRf,
                              &pBurstSize->dlb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_PKT_DLB_LB_THR_CTRLr,
                              CYPRESS_PKT_DLB_LB1_THRf,
                              &pBurstSize->dlb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_PKT_SRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_SRTCM_LB0_THRf,
                              &pBurstSize->srtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_PKT_SRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_SRTCM_LB1_THRf,
                              &pBurstSize->srtcm_ebs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_PKT_TRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_TRTCM_LB0_THRf,
                              &pBurstSize->trtcm_cbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_field_write(unit,
                              CYPRESS_METER_PKT_TRTCM_LB_THR_CTRLr,
                              CYPRESS_PKT_TRTCM_LB1_THRf,
                              &pBurstSize->trtcm_pbs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterExceed_get
 * Description:
 *      Get the meter exceed flag of a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 * Output:
 *      pIsExceed   - pointer to exceed flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX         - invalid entry index
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed)
{
    int32 ret;
    uint32 blockIdx = 0, entryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    blockIdx = meterIdx / 32;
    entryIdx = meterIdx % 32;

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          CYPRESS_METER_LB_EXCEED_STSr,
                          blockIdx,
                          entryIdx,
                          CYPRESS_LB_EXCEEDf,
                          pIsExceed)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if(TRUE == *pIsExceed)
    {
        /* reset the flag */
        if ((ret = reg_array_field_write1toClear(unit,
                              CYPRESS_METER_LB_EXCEED_STSr,
                              blockIdx,
                              entryIdx,
                              CYPRESS_LB_EXCEEDf)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "*pIsExceed=%d", *pIsExceed);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterExceedAggregation_get
 * Description:
 *      Get the meter exceed flag mask of meter entry exceed aggregated result every 16 entries.
 * Input:
 *      unit      - unit id
 * Output:
 *      pExceedMask - pointer to aggregated exceed flag mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pExceedMask), RT_ERR_NULL_POINTER);


    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_field_read(unit,
                          CYPRESS_METER_LB_GLB_EXCEED_STSr,
                          CYPRESS_LB_GLB_EXCEEDf,
                          pExceedMask)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "*pExceedMask=%d", *pExceedMask);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterEntry_get
 * Description:
 *      Get the content of a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 * Output:
 *      pMeterEntry - pointer to a meter entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_meterEntry_get(
    uint32          unit,
    uint32          meterIdx,
    rtk_acl_meterEntry_t   *pMeterEntry)
{
    int32           ret;
    uint32          value;
    uint32          table_index;
    meter_entry_t   meter_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    /*translate meter index to table index*/
    table_index = meterIdx;

    ACL_SEM_LOCK(unit);
    /* get entry from chip */
    if ((ret = table_read(unit, CYPRESS_METERt, table_index, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* get TYPE */
    if ((ret = table_field_get( unit, CYPRESS_METERt, CYPRESS_METER_TYPEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    switch (value)
    {
        case 0:
            pMeterEntry->type = METER_TYPE_INVALID;
            break;
        case 1:
            pMeterEntry->type = METER_TYPE_DLB;
            break;
        case 2:
            pMeterEntry->type = METER_TYPE_SRTCM;
            break;
        case 3:
            pMeterEntry->type = METER_TYPE_TRTCM;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), " ");
            return RT_ERR_FAILED;
    }

    /* get COLORAWARE */
    if ((ret = table_field_get( unit, CYPRESS_METERt, CYPRESS_METER_COLOR_AWAREtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    pMeterEntry->color_aware = value;


    /* get leaky bucket 0 rate */
    if ((ret = table_field_get( unit, CYPRESS_METERt, CYPRESS_METER_LB0_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    pMeterEntry->lb0_rate = value;

    /* get leaky bucket 1 rate */
    if ((ret = table_field_get( unit, CYPRESS_METERt, CYPRESS_METER_LB1_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    pMeterEntry->lb1_rate = value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pMeterEntry->type=%d, pMeterEntry->color_aware=%d, pMeterEntry->lb0_rate=%d, pMeterEntry->lb1_rate=%d",
        pMeterEntry->type, pMeterEntry->color_aware, pMeterEntry->lb0_rate, pMeterEntry->lb1_rate);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_meterEntry_set
 * Description:
 *      Set a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 *      pMeterEntry - pointer to meter entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_acl_meterEntry_set(
    uint32          unit,
    uint32          meterIdx,
    rtk_acl_meterEntry_t   *pMeterEntry)
{
    int32           ret;
    uint32          value;
    uint32          table_index;
    meter_entry_t   meter_entry;

    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d pMeterEntry->type=%d, \
        pMeterEntry->color_aware=%d, pMeterEntry->lb0_rate=%d, pMeterEntry->lb1_rate=%d",
        unit, meterIdx, pMeterEntry->type, pMeterEntry->color_aware, pMeterEntry->lb0_rate,
        pMeterEntry->lb1_rate);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);

    if (RTL8396M_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK((pMeterEntry->lb0_rate > HAL_RATE_OF_10G_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
        RT_PARAM_CHK((pMeterEntry->lb1_rate > HAL_RATE_OF_10G_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    }
    else
    {
        RT_PARAM_CHK((pMeterEntry->lb0_rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
        RT_PARAM_CHK((pMeterEntry->lb1_rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    }

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    /*translate policer index to table index*/
    table_index = meterIdx;

    /* set TYPE */
    switch (pMeterEntry->type)
    {
        case METER_TYPE_INVALID:
            value = 0;
            break;
        case METER_TYPE_DLB:
            value = 1;
            break;
        case METER_TYPE_SRTCM:
            value = 2;
            break;
        case METER_TYPE_TRTCM:
            value = 3;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), " ");
            return RT_ERR_FAILED;
    }
    if ((ret = table_field_set( unit, CYPRESS_METERt, CYPRESS_METER_TYPEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* set COLORAWARE */
    value = pMeterEntry->color_aware;
    if ((ret = table_field_set( unit, CYPRESS_METERt, CYPRESS_METER_COLOR_AWAREtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* set leaky bucket 0 rate */
    value = pMeterEntry->lb0_rate;
    if ((ret = table_field_set( unit, CYPRESS_METERt, CYPRESS_METER_LB0_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* set leaky bucket 1 rate */
    value = pMeterEntry->lb1_rate;
    if ((ret = table_field_set( unit, CYPRESS_METERt, CYPRESS_METER_LB1_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, CYPRESS_METERt, table_index, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/*
 * Transfer physical template field ID to logical template field ID
 */
static int32 _dal_cypress_acl_phy2logTmplteField(uint32 unit, uint32 phy_field_id, rtk_acl_templateFieldType_t *log_field_id)
{
    int32   ret = RT_ERR_OK;
    uint32  i;

    /* Check field type */
    for (i = 0; dal_cypress_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (TMPLTE_FIELD_IP_L4PORT_RANG == dal_cypress_template_field_list[i].field_type)
            continue;

        if (phy_field_id == dal_cypress_template_field_list[i].physical_id)
            break;
    }

    if (dal_cypress_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_ACL_FIELD_TYPE;
        return ret;
    }

    /* Get the logical field ID */
    *log_field_id = dal_cypress_template_field_list[i].field_type;

    return ret;
}

/*
 * Validate fields and transfer logical field ID to physical field ID
 */
static int32 _dal_cypress_acl_log2PhyTmplteField(uint32 unit, uint32 field_idx, rtk_acl_templateFieldType_t log_field_id, uint32 *phy_field_id)
{
    int32   ret = RT_ERR_OK;
    uint32  i;

    RT_PARAM_CHK((field_idx >= DAL_CYPRESS_MAX_NUM_OF_TEMPLATE_FIELD), RT_ERR_ENTRY_INDEX);

    if (TMPLTE_FIELD_IP_L4PORT_RANG == log_field_id)
        return RT_ERR_ACL_FIELD_TYPE;

    /* Check field type */
    for (i = 0; dal_cypress_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (log_field_id == dal_cypress_template_field_list[i].field_type)
            break;
    }

    if (dal_cypress_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_ACL_FIELD_TYPE;
        return ret;
    }

    /* Check field location */
    if ((dal_cypress_template_field_list[i].valid_location != 0) &&
        ((dal_cypress_template_field_list[i].valid_location & (1<<field_idx)) == 0))
        ret = RT_ERR_PIE_FIELD_LOCATION;

    /* Get the physical field ID */
    *phy_field_id = dal_cypress_template_field_list[i].physical_id;

    return ret;
}

static int32 _dal_cypress_acl_rule_del(uint32 unit, rtk_acl_clear_t *pClrIdx)
{
    int32   ret;
    uint32  value = 0, field_data;

    if ((ret = reg_field_set(unit, CYPRESS_ACL_CLR_CTRLr, CYPRESS_CLR_FROMf, &pClrIdx->start_idx, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, CYPRESS_ACL_CLR_CTRLr, CYPRESS_CLR_TOf, &pClrIdx->end_idx, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, CYPRESS_ACL_CLR_CTRLr, CYPRESS_CLRf, &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);

    if ((ret = reg_write(unit, CYPRESS_ACL_CLR_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /* wait until clear action is completed */
    do {
        reg_field_read(unit, CYPRESS_ACL_CLR_CTRLr, CYPRESS_CLRf, &value);
        if (value == 0)
            break;
    } while(1);

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32 _dal_cypress_acl_rule_move(uint32 unit, rtk_acl_move_t *pData)
{
    int32   ret;
    uint32  value = 0, field_data;

    if ((ret = reg_field_set(unit, CYPRESS_ACL_MV_CTRLr, CYPRESS_MV_FROMf, &pData->move_from, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, CYPRESS_ACL_MV_CTRLr, CYPRESS_MV_TOf, &pData->move_to, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, CYPRESS_ACL_MV_LEN_CTRLr, CYPRESS_MV_LENf, &pData->length)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    field_data = 1;
    if ((ret = reg_field_set(unit, CYPRESS_ACL_MV_CTRLr, CYPRESS_MVf, &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = reg_write(unit, CYPRESS_ACL_MV_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /* wait until move action is completed */
    do {
        reg_field_read(unit, CYPRESS_ACL_MV_CTRLr, CYPRESS_MVf, &value);
        if (value == 0)
            break;
    } while(1);

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleValidate_get(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    int32   ret;
    acl_igrRule_entry_t igr_entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_VALIDtf, pValid, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleValidate_set(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    int32   ret;
    acl_igrRule_entry_t igr_entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((valid != 0) && (valid != 1), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_VALIDtf, &valid, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleValidate_get(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    int32   ret;
    acl_egrRule_entry_t egr_entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &egr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_VALIDtf, pValid, (uint32 *) &egr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleValidate_set(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    int32   ret;
    acl_egrRule_entry_t egr_entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((valid != 0) && (valid != 1), RT_ERR_INPUT);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &egr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_VALIDtf, &valid, (uint32 *) &egr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &egr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleEntry_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    acl_igrRule_entry_t entry;
    uint32              size;
    int32               ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_cypress_acl_ruleEntrySize_get(unit, ACL_PHASE_IGR_ACL, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(pEntry_buffer, &entry, size);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleEntry_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    acl_igrRule_entry_t entry;
    uint32              size;
    int32               ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pEntry_buffer=%x",\
        unit, entry_idx, pEntry_buffer);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_cypress_acl_ruleEntrySize_get(unit, ACL_PHASE_IGR_ACL, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(&entry, pEntry_buffer, size);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_IACLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleEntry_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    acl_egrRule_entry_t entry;
    uint32              size;
    int32               ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_cypress_acl_ruleEntrySize_get(unit, ACL_PHASE_EGR_ACL, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(pEntry_buffer, &entry, size);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleEntry_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    acl_igrRule_entry_t entry;
    uint32              size;
    int32               ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, (uint32 *)pEntry_buffer)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_cypress_acl_ruleEntrySize_get(unit, ACL_PHASE_EGR_ACL, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(&entry, pEntry_buffer, size);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_cypress_acl_field2Buf_get
 * Description:
 *      Translate field info to data array.
 * Input:
 *      table   -   pointer to dal structure format
 * Output:
 *      buffer  -   pointer to chip format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      <A> Calu the field (sub bits) byte offset of the type
 *      <B> The data of type will push into LSB of BYTE base, not WORD base.
 *
 *      Ex. OMPLS = 0x12345
 *      1. OLABEL = outer label [15:0]
 *          field_size:20, field_offset:0 field_length:16, data_offset:0
 *          buf_byte_idx_num:3, buf_field_bit_max:15,
 *          buf_field_byte_idx_start:0, buf_field_byte_idx_end:1
 *          tmp_data_offset:0, buf_field_bit_start:0, chip_field_offset:0
 *
 *          a) buf_field_offset:8,  chip_field_offset:0, buf_field_bit_start:0
 *             pTmp_data = 0x4500
 *          b) buf_field_offset:16, chip_field_offset:8, buf_field_bit_start:8
 *             pTmp_data = 0x234500
 *      2. OILABEL = outer label [19:0]
 *          field_size:20, field_offset:12 field_length:4, data_offset:16
 *          buf_byte_idx_num:3, buf_field_bit_max:19, buf_field_byte_idx_start:2, buf_field_byte_idx_end:2
 *          tmp_data_offset:0, buf_field_bit_start:16, chip_field_offset:12
 *
 *          a) buf_field_offset:24, chip_field_offset:12, buf_field_bit_start:16
 *             pTmp_data = 0x1234500
 */
static void
_dal_cypress_acl_field2Buf_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint32 data, uint32 mask,
    uint8 *pData, uint8 *pMask)
{
    uint32  field_offset, field_length; /* the type data in chip field offset and length */
    uint32  field_size;                 /* the total size of type data */
    uint32  data_offset;                /* the chip field data will push into buffer offset */
    uint32  *pTmp_data;
    uint32  *pTmp_mask;
    uint32  tmp_offset;                 /* some field is not 8 bits alignment */
    uint32  buf_field_offset;           /* push the chip data to buffer offset (per byte) */
    uint32  buf_byte_idx_num;           /* total bytes of type data */
    uint32  mask_len, i;
    uint32  chip_field_offset;          /* get the chip field data offset (per byte) */
    uint32  buf_field_bit_start, buf_field_bit_end;
    uint32  buf_field_bit_max;          /* max bits of buffer in the type of the field */
    uint32  buf_field_byte_idx_start;   /* start byte index of buffer (per byte) */
    uint32  buf_field_byte_idx_end;     /* end byte index of buffer (per byte) */
    uint32  tmp_data_offset;
    uint32  data_field;

    field_offset = dal_cypress_acl_field_list[fieldType].pField[fieldNumber].field_offset;
    field_length = dal_cypress_acl_field_list[fieldType].pField[fieldNumber].field_length;
    data_offset = dal_cypress_acl_field_list[fieldType].pField[fieldNumber].data_offset;

    dal_cypress_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_CYPRESS_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_CYPRESS_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_CYPRESS_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_CYPRESS_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_CYPRESS_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_CYPRESS_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_CYPRESS_MAX_INFO_IDX);
        pTmp_data = (uint32 *)pData + tmp_offset;
        pTmp_mask = (uint32 *)pMask + tmp_offset;

        data_field = ((1 << mask_len) - 1) << buf_field_offset;
        *pTmp_data &= ~data_field;
        *pTmp_mask &= ~data_field;

        *pTmp_data |= ((data >> chip_field_offset) & ((1 << mask_len)-1)) << buf_field_offset;
        *pTmp_mask |= ((mask >> chip_field_offset) & ((1 << mask_len)-1)) << buf_field_offset;
        chip_field_offset += mask_len;

        buf_field_bit_start = buf_field_bit_end + 1;
    }
}   /* end of _dal_cypress_acl_field2Buf_get */

static void
_dal_cypress_acl_buf2Field_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint16 *data16, uint16 *mask16,
    uint8 *pData, uint8 *pMask)
{
    uint32  field_offset, data_offset, field_length;
    uint32  *pTmp_data;
    uint32  *pTmp_mask;
    uint32  field_size, tmp_offset;
    uint32  buf_byte_idx_num;   /* num of byte idx */
    uint32  buf_field_offset, mask_len, i;
    uint32  chip_field_offset;
    uint32  buf_field_bit_start, buf_field_bit_end, buf_field_bit_max;
    uint32  buf_field_byte_idx_start, buf_field_byte_idx_end;
    uint32  tmp_data_offset;

    *data16 = *mask16 = 0;

    field_offset = dal_cypress_acl_field_list[fieldType].pField[fieldNumber].field_offset;
    field_length = dal_cypress_acl_field_list[fieldType].pField[fieldNumber].field_length;
    data_offset = dal_cypress_acl_field_list[fieldType].pField[fieldNumber].data_offset;

    dal_cypress_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_CYPRESS_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_CYPRESS_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_CYPRESS_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_CYPRESS_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_CYPRESS_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_CYPRESS_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_CYPRESS_MAX_INFO_IDX);
        pTmp_data = (uint32 *)pData + tmp_offset;
        pTmp_mask = (uint32 *)pMask + tmp_offset;

        *data16 |= ((*pTmp_data >> buf_field_offset) & ((1 << mask_len)-1)) << chip_field_offset;
        *mask16 |= ((*pTmp_mask >> buf_field_offset) & ((1 << mask_len)-1)) << chip_field_offset;

        chip_field_offset += mask_len;

        buf_field_bit_start = buf_field_bit_end + 1;
    }
}   /* end of _dal_cypress_acl_buf2Field_get */

static int32
_dal_cypress_acl_igrRuleEntryField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    acl_igrRule_entry_t     entry;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, type=%d", unit, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_cypress_acl_igrRuleEntryBufField_read(unit, entry_idx,
            type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleEntryField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;
    acl_igrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, type=%d, pData=%x, pMask=%x", unit, entry_idx, type, pData, pMask);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_cypress_acl_igrRuleEntryBufField_write(unit, entry_idx,
            type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_IACLt, entry_idx,
            (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleEntryField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    acl_egrRule_entry_t entry;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, type=%d", unit, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_cypress_acl_egrRuleEntryBufField_read(unit, entry_idx,
            type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleEntryField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    acl_egrRule_entry_t entry;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, type=%d, pData=%x, pMask=%x",
            unit, entry_idx, type, pData, pMask);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_cypress_acl_egrRuleEntryBufField_write(unit, entry_idx,
        type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleOperation_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;
    acl_igrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_NOTtf,\
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_AND1tf,\
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_AND2tf,\
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleOperation_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;
    acl_igrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pOperation=%x", unit, entry_idx, pOperation);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pOperation->aggr_1 == ENABLED) && (entry_idx%2 != 0)), RT_ERR_ENTRY_INDEX);
    if (pOperation->aggr_2 == ENABLED &&
        (entry_idx%2 != 0 || (entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 != 0))
    {
        return RT_ERR_ENTRY_INDEX;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_NOTtf,\
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_AND1tf,\
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_AND2tf,\
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleOperation_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;
    acl_igrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_NOTtf,\
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_AND1tf,\
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_AND2tf,\
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRuleOperation_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;
    acl_egrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pOperation=%x", unit, entry_idx, pOperation);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pOperation->aggr_1 == ENABLED) && (entry_idx%2 != 0)), RT_ERR_ENTRY_INDEX);
    if (pOperation->aggr_2 == ENABLED &&
        (entry_idx%2 != 0 || (entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 != 0))
    {
        return RT_ERR_ENTRY_INDEX;
    }

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_NOTtf,\
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_AND1tf,\
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_AND2tf,\
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction)
{
    int32           ret;
    uint32          value;
    acl_igrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, action_idx=%d", unit, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    osal_memset(pAction, 0x0, sizeof(rtk_acl_igrAction_t));

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* Forwarding action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_FWD_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_PERMIT;
            break;
        case 1:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_DROP;
            break;
        case 2:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_COPY_TO_PORTID;
            break;
        case 3:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_COPY_TO_PORTMASK;
            break;
        case 4:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID;
            break;
        case 5:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTMASK;
            break;
        case 6:
            pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_UNICAST_ROUTING;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_FWD_PORT_INFOtf,\
        &pAction->fwd_data.fwd_info, (uint32 *) &entry), errHandle, ret);

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_LOG_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (value == 0)
    {
        pAction->stat_data.stat_type = STAT_TYPE_PACKET_BASED_32BIT;
    }
    else
    {
        pAction->stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
    }
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_LOG_IDXtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    /* translate physical index to logical index */
    pAction->stat_data.stat_idx = value;
    if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        pAction->stat_data.stat_idx = ( value >> 1);
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_MIR_IDXtf,\
        &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_METER_IDXtf,\
        &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_IVIDtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_IVLAN_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_vlan_data.vid_assign_type = ACL_IGR_ACTION_IVLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->inner_vlan_data.vid_assign_type = ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->inner_vlan_data.vid_assign_type = ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID;
            break;
    }
    pAction->inner_vlan_data.vid_shift_sel = 0;
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_IVLAN_DATAtf,\
        &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_OVIDtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_OVLAN_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_vlan_data.vid_assign_type = ACL_IGR_ACTION_OVLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->outer_vlan_data.vid_assign_type = ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->outer_vlan_data.vid_assign_type = ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID;
            break;
    }
    pAction->outer_vlan_data.vid_shift_sel = 0;
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_OVLAN_DATAtf,\
        &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    /* Priority action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_PRItf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_INT_PRItf,\
        &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* MPLS action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_MPLStf,\
        &pAction->mpls_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_MPLS_ACTtf,\
        &pAction->mpls_data.mpls_act, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_MPLS_LIB_IDXtf,\
        &pAction->mpls_data.mpls_idx, (uint32 *) &entry), errHandle, ret);
    if (pAction->mpls_data.mpls_act == 1)
    {
        /* translate physical index to logical index */
        pAction->mpls_data.mpls_idx = (pAction->mpls_data.mpls_idx >> 1);
    }

    /* Bypass action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_BYPASStf,\
        &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_BYPASS_IBC_SCtf,\
        &pAction->bypass_data.ibc_sc, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_BYPASS_IGR_STPtf,\
        &pAction->bypass_data.igr_stp, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_IACLt, CYPRESS_IACL_BYPASS_ALLtf,\
        &pAction->bypass_data.all, (uint32 *) &entry), errHandle, ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pAction=%x", *pAction);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
} /* end of dal_cypress_acl_igrRuleAction_get */

static int32
_dal_cypress_acl_igrRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction)
{
    int32               ret;
    uint32              value;
    uint32              l2_tableSize;
    acl_igrRule_entry_t entry;
    rtk_vlan_t          vid_value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = table_size_get(unit, CYPRESS_L2_UCt, &l2_tableSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* input value range check */
    if ((pAction->fwd_data.fwd_type >= ACL_IGR_ACTION_FWD_END) ||
        (((pAction->fwd_data.fwd_type == ACL_IGR_ACTION_FWD_COPY_TO_PORTMASK) ||
          (pAction->fwd_data.fwd_type == ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTMASK) ||
          (pAction->fwd_data.fwd_type == ACL_IGR_ACTION_FWD_UNICAST_ROUTING)) &&
         (pAction->fwd_data.fwd_info >= l2_tableSize)) ||
        (pAction->stat_data.stat_type >= STAT_TYPE_END) ||
        ((pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT) &&
         (pAction->stat_data.stat_idx >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)) ||
        ((pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT) &&
         (pAction->stat_data.stat_idx >= HAL_MAX_NUM_OF_PIE_COUNTER(unit))) ||
        (pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit)) ||
        (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit)) ||
        (pAction->inner_vlan_data.vid_assign_type >= ACL_IGR_ACTION_IVLAN_ASSIGN_END) ||
        (pAction->inner_vlan_data.vid_shift_sel >= 2) ||
        (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX) ||
        (pAction->outer_vlan_data.vid_assign_type >= ACL_IGR_ACTION_OVLAN_ASSIGN_END) ||
        (pAction->outer_vlan_data.vid_shift_sel >= 2) ||
        (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX) ||
        (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit)) ||
        (pAction->mpls_data.mpls_idx == 0 && pAction->mpls_data.mpls_idx >= HAL_MAX_NUM_OF_MPLS_LIB(unit)*2) ||
        (pAction->mpls_data.mpls_idx == 1 && pAction->mpls_data.mpls_idx >= HAL_MAX_NUM_OF_MPLS_LIB(unit)))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "parameter error");
        return RT_ERR_INPUT;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);

    switch (pAction->fwd_data.fwd_type)
    {
        case ACL_IGR_ACTION_FWD_PERMIT:
            value = 0;
            break;
        case ACL_IGR_ACTION_FWD_DROP:
            value = 1;
            break;
        case ACL_IGR_ACTION_FWD_COPY_TO_PORTID:
            value = 2;
            break;
        case ACL_IGR_ACTION_FWD_COPY_TO_PORTMASK:
            value = 3;
            break;
        case ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID:
            value = 4;
            break;
        case ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTMASK:
            value = 5;
            break;
        case ACL_IGR_ACTION_FWD_UNICAST_ROUTING:
            value = 6;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_FWD_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_FWD_PORT_INFOtf,\
        &pAction->fwd_data.fwd_info, (uint32 *) &entry), errHandle, ret);

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    if (pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT)
    {
        value = 0;
    }
    else if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        value = 1;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_LOG_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    /* translate logical index to physical index */
    value = pAction->stat_data.stat_idx;
    if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        value = (pAction->stat_data.stat_idx << 1);
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_LOG_IDXtf,\
        &value, (uint32 *) &entry), errHandle, ret);

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_MIR_IDXtf,\
        &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_METER_IDXtf,\
        &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_IVIDtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    switch (pAction->inner_vlan_data.vid_assign_type)
    {
        case ACL_IGR_ACTION_IVLAN_ASSIGN_NEW_VID:
            value = 0;
            break;
        case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_VID:
            value = 1;
            break;
        case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
            value = 2;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_IVLAN_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->inner_vlan_data.vid_shift_sel)
        vid_value = 4096 - pAction->inner_vlan_data.vid_value;
    else
        vid_value = pAction->inner_vlan_data.vid_value;
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_IVLAN_DATAtf,\
        &vid_value, (uint32 *) &entry), errHandle, ret);

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_OVIDtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    switch (pAction->outer_vlan_data.vid_assign_type)
    {
        case ACL_IGR_ACTION_OVLAN_ASSIGN_NEW_VID:
            value = 0;
            break;
        case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_VID:
            value = 1;
            break;
        case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID:
            value = 2;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_OVLAN_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->inner_vlan_data.vid_shift_sel)
        vid_value = 4096 - pAction->outer_vlan_data.vid_value;
    else
        vid_value = pAction->outer_vlan_data.vid_value;
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_OVLAN_DATAtf,\
        &vid_value, (uint32 *) &entry), errHandle, ret);

    /* Priority action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_PRItf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_INT_PRItf,\
        &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* MPLS action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_MPLStf,\
        &pAction->mpls_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_MPLS_ACTtf,\
        &pAction->mpls_data.mpls_act, (uint32 *) &entry), errHandle, ret);
    if (pAction->mpls_data.mpls_act == 1)
    {
        /* translate logical index to physical index */
        value = (pAction->mpls_data.mpls_idx << 1);
    }
    else
    {
        value = pAction->mpls_data.mpls_idx;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_MPLS_LIB_IDXtf,\
        &value, (uint32 *) &entry), errHandle, ret);

    /* Bypass action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_ACT_MSK_BYPASStf,\
        &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_BYPASS_IBC_SCtf,\
        &pAction->bypass_data.ibc_sc, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_BYPASS_IGR_STPtf,\
        &pAction->bypass_data.igr_stp, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_IACLt, CYPRESS_IACL_BYPASS_ALLtf,\
        &pAction->bypass_data.all, (uint32 *) &entry), errHandle, ret);

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
} /* end of dal_cypress_acl_igrRuleAction_set */

static int32
_dal_cypress_acl_egrRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_egrAction_t     *pAction)
{
    int32           ret;
    uint32          value;
    acl_egrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* Forwarding action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_FWD_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_PERMIT;
            break;
        case 1:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_DROP;
            break;
        case 2:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_COPY_TO_PORTID;
            break;
        case 3:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_COPY_TO_PORTMASK;
            break;
        case 4:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTID;
            break;
        case 5:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTMASK;
            break;
        case 6:
            pAction->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_FILTERING;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_FWD_PORT_INFOtf,\
        &pAction->fwd_data.fwd_info, (uint32 *) &entry), errHandle, ret);

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_LOG_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (value == 0)
    {
        pAction->stat_data.stat_type = STAT_TYPE_PACKET_BASED_32BIT;
    }
    else if (value == 1)
    {
        pAction->stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
    }
    /* translate physical index to logical index */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_LOG_IDXtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    pAction->stat_data.stat_idx = value;
    if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        pAction->stat_data.stat_idx = (value >> 1);
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_MIR_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_ORIGINAL;
            break;
        case 1:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_MODIFIED;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_MIR_IDXtf,\
        &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_METER_IDXtf,\
        &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress I-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_IVIDtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_vlan_data.vid_assign_type = ACL_EGR_ACTION_IVLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->inner_vlan_data.vid_assign_type = ACL_EGR_ACTION_IVLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->inner_vlan_data.vid_assign_type = ACL_EGR_ACTION_IVLAN_ASSIGN_COPY_FROM_OUTER_VID;
            break;
        case 3:
            pAction->inner_vlan_data.vid_assign_type = ACL_EGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
            return RT_ERR_FAILED;
    }
    pAction->inner_vlan_data.vid_shift_sel = 0;
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_VIDtf,\
        &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_TPID_ACTtf,\
        &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_TPID_IDXtf,\
        &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_OVIDtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_vlan_data.vid_assign_type = ACL_EGR_ACTION_OVLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->outer_vlan_data.vid_assign_type = ACL_EGR_ACTION_OVLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->outer_vlan_data.vid_assign_type = ACL_EGR_ACTION_OVLAN_ASSIGN_COPY_FROM_INNER_VID;
            break;
        case 3:
            pAction->outer_vlan_data.vid_assign_type = ACL_EGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
            return RT_ERR_FAILED;
    }
    pAction->outer_vlan_data.vid_shift_sel = 0;
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_VIDtf,\
        &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_TPID_ACTtf,\
        &pAction->outer_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_TPID_IDXtf,\
        &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Priority action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_PRItf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_PRI_ACTtf,\
        &pAction->pri_data.pri_act, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_INT_PRItf,\
        &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* Remark action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_QOS_REMARKtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_RMK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_INNER_USER_PRI;
            break;
        case 1:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_OUTER_USER_PRI;
            break;
        case 2:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 3:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 4:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI;
            break;
        case 5:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "rmk_data.rmk_act error");
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_RMK_VALtf,\
        &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /* Tag action */
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_TAG_STStf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_TAG_STS_INNERtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_UNTAG;
            break;
        case 1:
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_TAG;
            break;
        case 2:
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_KEEP_CONTENT;
            break;
        case 3:
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_NOP;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "tag_sts_data.itag_sts error");
            return RT_ERR_FAILED;
    }

    RT_ERR_HDL(table_field_get(unit, CYPRESS_EACLt, CYPRESS_EACL_TAG_STS_OUTERtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_UNTAG;
            break;
        case 1:
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_TAG;
            break;
        case 2:
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_KEEP_CONTENT;
            break;
        case 3:
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_NOP;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "tag_sts_data.otag_sts error");
            return RT_ERR_FAILED;
    }

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}

static int32
_dal_cypress_acl_egrRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_egrAction_t     *pAction)
{
    int32               ret;
    uint32              value;
    acl_egrRule_entry_t entry;
    rtk_vlan_t          vid_value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, entry_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* input value range check */
    if ((pAction->fwd_data.fwd_type >= ACL_EGR_ACTION_FWD_END) ||
        (((pAction->fwd_data.fwd_type == ACL_EGR_ACTION_FWD_COPY_TO_PORTMASK) ||
          (pAction->fwd_data.fwd_type == ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTMASK) ||
          (pAction->fwd_data.fwd_type == ACL_EGR_ACTION_FWD_FILTERING)) &&
         (pAction->fwd_data.fwd_info >= HAL_MAX_NUM_OF_MCAST_ENTRY(unit))) ||
        (pAction->stat_data.stat_type >= STAT_TYPE_END) ||
        ((pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT) &&
         (pAction->stat_data.stat_idx >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)) ||
        ((pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT) &&
         (pAction->stat_data.stat_idx >= HAL_MAX_NUM_OF_PIE_COUNTER(unit))) ||
        (pAction->mirror_data.mirror_type >= ACL_ACTION_MIRROR_END) ||
        (pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit)) ||
        (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit)) ||
        (pAction->inner_vlan_data.vid_assign_type >= ACL_EGR_ACTION_IVLAN_ASSIGN_END) ||
        (pAction->inner_vlan_data.vid_shift_sel >= 2) ||
        (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX) ||
        (pAction->inner_vlan_data.tpid_assign >= RTK_ENABLE_END) ||
        (pAction->inner_vlan_data.tpid_idx >= HAL_MAX_NUM_OF_CVLAN_TPID(unit)) ||
        (pAction->outer_vlan_data.vid_assign_type >= ACL_EGR_ACTION_OVLAN_ASSIGN_END) ||
        (pAction->outer_vlan_data.vid_shift_sel >= 2) ||
        (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX) ||
        (pAction->outer_vlan_data.tpid_assign >= RTK_ENABLE_END) ||
        (pAction->outer_vlan_data.tpid_idx >= HAL_MAX_NUM_OF_SVLAN_TPID(unit)) ||
        (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit)) ||
        (pAction->rmk_data.rmk_act >= ACL_ACTION_REMARK_END) ||
        (((pAction->rmk_data.rmk_act == ACL_ACTION_REMARK_INNER_USER_PRI) ||
          (pAction->rmk_data.rmk_act == ACL_ACTION_REMARK_OUTER_USER_PRI) ||
          (pAction->rmk_data.rmk_act == ACL_ACTION_REMARK_IP_PRECEDENCE)) &&
         (pAction->rmk_data.rmk_info > HAL_INTERNAL_PRIORITY_MAX(unit))) ||
        ((pAction->rmk_data.rmk_act == ACL_ACTION_REMARK_DSCP) &&
         (pAction->rmk_data.rmk_info >= 64)) ||
        (pAction->tag_sts_data.itag_sts >= ACL_ACTION_TAG_STS_END) ||
        (pAction->tag_sts_data.otag_sts >= ACL_ACTION_TAG_STS_END))
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "parameter error");
        return RT_ERR_FAILED;
    }

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, entry_idx, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);

    switch (pAction->fwd_data.fwd_type)
    {
        case ACL_EGR_ACTION_FWD_PERMIT:
            value = 0;
            break;
        case ACL_EGR_ACTION_FWD_DROP:
            value = 1;
            break;
        case ACL_EGR_ACTION_FWD_COPY_TO_PORTID:
            value = 2;
            break;
        case ACL_EGR_ACTION_FWD_COPY_TO_PORTMASK:
            value = 3;
            break;
        case ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTID:
            value = 4;
            break;
        case ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTMASK:
            value = 5;
            break;
        case ACL_EGR_ACTION_FWD_FILTERING:
            value = 6;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_FWD_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_FWD_PORT_INFOtf,\
        &pAction->fwd_data.fwd_info, (uint32 *) &entry), errHandle, ret);

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    if (pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT)
    {
        value = 0;
    }
    else if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        value = 1;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_LOG_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    /* translate logical index to physical index */
    value = pAction->stat_data.stat_idx;
    if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        value = (pAction->stat_data.stat_idx << 1);
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_LOG_IDXtf,\
        &value, (uint32 *) &entry), errHandle, ret);

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    switch (pAction->mirror_data.mirror_type)
    {
        case ACL_ACTION_MIRROR_ORIGINAL:
            value = 0;
            break;
        case ACL_ACTION_MIRROR_MODIFIED:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "mirror_data.mirror_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_MIR_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_MIR_IDXtf,\
        &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_METER_IDXtf,\
        &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress I-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_IVIDtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    switch (pAction->inner_vlan_data.vid_assign_type)
    {
        case ACL_EGR_ACTION_IVLAN_ASSIGN_NEW_VID:
            value = 0;
            break;
        case ACL_EGR_ACTION_IVLAN_ASSIGN_SHIFT_VID:
            value = 1;
            break;
        case ACL_EGR_ACTION_IVLAN_ASSIGN_COPY_FROM_OUTER_VID:
            value = 2;
            break;
        case ACL_EGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
            value = 3;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->inner_vlan_data.vid_shift_sel)
        vid_value = 4096 - pAction->inner_vlan_data.vid_value;
    else
        vid_value = pAction->inner_vlan_data.vid_value;
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_VIDtf,\
        &vid_value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_TPID_ACTtf,\
        &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_IVLAN_TPID_IDXtf,\
        &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress O-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_OVIDtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    switch (pAction->outer_vlan_data.vid_assign_type)
    {
        case ACL_EGR_ACTION_OVLAN_ASSIGN_NEW_VID:
            value = 0;
            break;
        case ACL_EGR_ACTION_OVLAN_ASSIGN_SHIFT_VID:
            value = 1;
            break;
        case ACL_EGR_ACTION_OVLAN_ASSIGN_COPY_FROM_INNER_VID:
            value = 2;
            break;
        case ACL_EGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID:
            value = 3;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->outer_vlan_data.vid_shift_sel)
        vid_value = 4096 - pAction->outer_vlan_data.vid_value;
    else
        vid_value = pAction->outer_vlan_data.vid_value;
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_VIDtf,\
        &vid_value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_TPID_ACTtf,\
        &pAction->outer_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_OVLAN_TPID_IDXtf,\
        &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Priority action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_PRItf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_PRI_ACTtf,\
        &pAction->pri_data.pri_act, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_INT_PRItf,\
        &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_QOS_REMARKtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);
    switch (pAction->rmk_data.rmk_act)
    {
        case ACL_ACTION_REMARK_INNER_USER_PRI:
            value = 0;
            break;
        case ACL_ACTION_REMARK_OUTER_USER_PRI:
            value = 1;
            break;
        case ACL_ACTION_REMARK_DSCP:
            value = 2;
            break;
        case ACL_ACTION_REMARK_IP_PRECEDENCE:
            value = 3;
            break;
        case ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI:
            value = 4;
            break;
        case ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI:
            value = 5;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "mk_data.rmk_act error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_RMK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_RMK_VALtf,\
        &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /* Tag action */
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_ACT_MSK_TAG_STStf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);
    switch (pAction->tag_sts_data.itag_sts)
    {
        case ACL_ACTION_TAG_STS_UNTAG:
            value = 0;
            break;
        case ACL_ACTION_TAG_STS_TAG:
            value = 1;
            break;
        case ACL_ACTION_TAG_STS_KEEP_CONTENT:
            value = 2;
            break;
        case ACL_ACTION_TAG_STS_NOP:
            value = 3;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.itag_sts error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_TAG_STS_INNERtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (pAction->tag_sts_data.otag_sts)
    {
        case ACL_ACTION_TAG_STS_UNTAG:
            value = 0;
            break;
        case ACL_ACTION_TAG_STS_TAG:
            value = 1;
            break;
        case ACL_ACTION_TAG_STS_KEEP_CONTENT:
            value = 2;
            break;
        case ACL_ACTION_TAG_STS_NOP:
            value = 3;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.otag_sts error");
            return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_set(unit, CYPRESS_EACLt, CYPRESS_EACL_TAG_STS_OUTERtf,\
        &value, (uint32 *) &entry), errHandle, ret);

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, CYPRESS_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}

static int32
_dal_cypress_acl_igrRule_del(uint32 unit, rtk_acl_clear_t *pClrIdx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, start_idx=%d, end_idx=%d",\
        unit, pClrIdx->start_idx, pClrIdx->end_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pClrIdx->start_idx);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pClrIdx->end_idx);
    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_ACL_CLEAR_INDEX);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pClrIdx->start_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pClrIdx->end_idx);

    if ((ret = _dal_cypress_acl_rule_del(unit, pClrIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRule_del(uint32 unit, rtk_acl_clear_t *pClrIdx)
{
    int32   ret;
    rtk_acl_clear_t phy_clrIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, start_idx=%d, end_idx=%d",\
        unit, pClrIdx->start_idx, pClrIdx->end_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, pClrIdx->start_idx);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, pClrIdx->end_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pClrIdx->start_idx);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pClrIdx->end_idx);
    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_ACL_CLEAR_INDEX);

    /* translate to physical index */
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, pClrIdx->start_idx, phy_clrIdx.start_idx);
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, pClrIdx->end_idx, phy_clrIdx.end_idx);
    if ((ret = _dal_cypress_acl_rule_del(unit, &phy_clrIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_igrRule_move(uint32 unit, rtk_acl_move_t *pData)
{
    int32   ret;
    uint32  entry_num;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, move_from=%d, move_to=%d, length=%d",\
        unit, pData->move_from, pData->move_to, pData->length);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pData->move_from);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pData->move_to);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pData->move_from);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pData->move_to);
    DAL_CYPRESS_ACL_ENTRYNUM(unit, ACL_PHASE_IGR_ACL, entry_num);
    if ((pData->move_from + pData->length) > entry_num ||
        (pData->move_to + pData->length) > entry_num)
        return RT_ERR_ENTRY_INDEX;

    if ((ret = _dal_cypress_acl_rule_move(unit, pData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_cypress_acl_egrRule_move(uint32 unit, rtk_acl_move_t *pData)
{
    int32   ret;
    uint32  entry_num;
    rtk_acl_move_t phy_data;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, move_from=%d, move_to=%d, length=%d",\
        unit, pData->move_from, pData->move_to, pData->length);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, pData->move_from);
    DAL_CYPRESS_ACL_INDEX_CHK(unit, ACL_PHASE_EGR_ACL, pData->move_to);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pData->move_from);
    DAL_CYPRESS_ACL_BLOCK_PWR_CHK(unit, pData->move_to);
    DAL_CYPRESS_ACL_ENTRYNUM(unit, ACL_PHASE_EGR_ACL, entry_num);
    if ((pData->move_from + pData->length) > entry_num ||
        (pData->move_to + pData->length) > entry_num)
        return RT_ERR_ENTRY_INDEX;

    phy_data.length = pData->length;
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, pData->move_from, phy_data.move_from);
    DAL_CYPRESS_ACL_INDEX_TO_PHYSICAL(unit, pData->move_to, phy_data.move_to);
    if ((ret = _dal_cypress_acl_rule_move(unit, &phy_data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_acl_fieldSupported_check
 * Description:
 *      Check field type is supported.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      type        - field type to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ACL_PHASE        - invalid ACL phase
 *      RT_ERR_ACL_FIELD_TYPE   - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_acl_fieldSupported_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type)
{
    uint32  i, j, k;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, type=%d",\
            unit, phase, type);

    if (phase != ACL_PHASE_IGR_ACL && phase != ACL_PHASE_EGR_ACL)
        return RT_ERR_ACL_PHASE;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    DAL_CYPRESS_ACL_FIELD_TYPE_CHK(unit, type);

    if (ACL_PHASE_EGR_ACL == phase)
        return RT_ERR_OK;

    for (i = 0; dal_cypress_acl_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (dal_cypress_acl_field_list[i].type == type)
        {
            for (j = 0; j < dal_cypress_acl_field_list[i].field_number; ++j)
            {
                for (k = 0; template_eacl_list[k] != TMPLTE_FIELD_END; ++k)
                {
                    if (template_eacl_list[k] ==
                            dal_cypress_acl_field_list[i].pField[j].template_field_type)
                        return RT_ERR_ACL_TEMPLATE_INCOMPATIBLE;
                }
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_acl_fieldSupported_check */

/* Function Name:
 *      _dal_cypress_acl_init_config
 * Description:
 *      Initialize default configuration for the ACL module of the specified device..
 * Input:
 *      unit                - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
static int32
_dal_cypress_acl_init_config(uint32 unit)
{
    int32   ret;
    uint32  blockIdx;
    rtk_acl_meterMode_t meterMode;
    rtk_acl_meterBurstSize_t burstSize;

    if ((ret = dal_cypress_acl_meterIncludeIfg_set(unit, RTK_DEFAULT_METER_IFG_INCLUDE_STATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    for (blockIdx = 0; blockIdx < HAL_MAX_NUM_OF_METER_BLOCK(unit); blockIdx++)
    {
        if ((ret = dal_cypress_acl_meterMode_get(unit, blockIdx, &meterMode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = dal_cypress_acl_meterMode_set(unit, blockIdx, meterMode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    /* Policer High Threshold */
    burstSize.dlb_lb0bs = METER_LB_BPS_HIGH_THRESH;
    burstSize.dlb_lb1bs = METER_LB_BPS_HIGH_THRESH;
    burstSize.srtcm_cbs = METER_LB_BPS_HIGH_THRESH;
    burstSize.srtcm_ebs = METER_LB_BPS_HIGH_THRESH;
    burstSize.trtcm_cbs = METER_LB_BPS_HIGH_THRESH;
    burstSize.trtcm_pbs = METER_LB_BPS_HIGH_THRESH;
    if ((ret = dal_cypress_acl_meterBurstSize_set(unit, METER_MODE_BYTE, &burstSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    burstSize.dlb_lb0bs = METER_LB_PPS_HIGH_THRESH;
    burstSize.dlb_lb1bs = METER_LB_PPS_HIGH_THRESH;
    burstSize.srtcm_cbs = METER_LB_PPS_HIGH_THRESH;
    burstSize.srtcm_ebs = METER_LB_PPS_HIGH_THRESH;
    burstSize.trtcm_cbs = METER_LB_PPS_HIGH_THRESH;
    burstSize.trtcm_pbs = METER_LB_PPS_HIGH_THRESH;
    if ((ret = dal_cypress_acl_meterBurstSize_set(unit, METER_MODE_PACKET, &burstSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_acl_init_config */

/* Function Name:
 *      dal_cypress_acl_templateFieldIntentVlanTag_get
 * Description:
 *      Get the acl template field VLAN tag status
 * Input:
 *      unit     - unit id
 * Output:
 *      tagType  - template field VLAN tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only work on template 0.
 */
int32
dal_cypress_acl_templateFieldIntentVlanTag_get(uint32 unit,
    rtk_vlan_tagType_t *tagType)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == tagType), RT_ERR_NULL_POINTER);

    /* function body */
    ACL_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_ACL_CTRLr, CYPRESS_TMPLTE_IOTAG_SELf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    if (0 == value)
        *tagType = VLAN_TAG_TYPE_INNER;
    else
        *tagType = VLAN_TAG_TYPE_OUTER;

    return RT_ERR_OK;
}   /* end of dal_cypress_acl_templateFieldIntentVlanTag_get */

/* Function Name:
 *      dal_cypress_acl_templateFieldIntentVlanTag_set
 * Description:
 *      Set the acl template field VLAN tag status
 * Input:
 *      unit     - unit id
 *      tagType  - template field VLAN tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      Only work on template 0.
 */
int32
dal_cypress_acl_templateFieldIntentVlanTag_set(uint32 unit,
    rtk_vlan_tagType_t tagType)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,tagType=%d",unit, tagType);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((VLAN_TAG_TYPE_INNER != tagType) && (VLAN_TAG_TYPE_OUTER  != tagType), RT_ERR_INPUT);

    /* function body */
    if (VLAN_TAG_TYPE_INNER == tagType)
        value = 0;
    else
        value = 1;

    ACL_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_ACL_CTRLr, CYPRESS_TMPLTE_IOTAG_SELf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_acl_templateFieldIntentVlanTag_set */
