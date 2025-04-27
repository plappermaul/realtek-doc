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
 *            2) Field Selector
 *            3) Range Check
 *            4) Meter
 *            5) Counter
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
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_acl.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/acl.h>


/*
 * Symbol Definition
 */
#define DAL_MAPLE_MAX_NUM_OF_FIXED_FIELD        3
#define DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD     12
#define DAL_MAPLE_DATA_BITS                   8
#define DAL_MAPLE_BUFFER_UNIT_LENGTH_BITS       32
#define DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD  5

#define DAL_MAPLE_MAX_INFO_IDX                \
    (DAL_MAPLE_BUFFER_UNIT_LENGTH_BITS / DAL_MAPLE_DATA_BITS)

#define DAL_MAPLE_DATA_WIDTH_GET(_data_len)                   \
    (((_data_len) + DAL_MAPLE_DATA_BITS - 1) / DAL_MAPLE_DATA_BITS)

#define DAL_MAPLE_GET_BYTE_IDX(_offset)                       \
    ((_offset) / DAL_MAPLE_DATA_BITS)

#define DAL_MAPLE_GET_INFO_IDX(_size, _offset)                \
    (DAL_MAPLE_DATA_WIDTH_GET((_size)) - DAL_MAPLE_DATA_WIDTH_GET((_offset)))

#define DAL_MAPLE_GET_INFO_OFFSET(_max, _idx)                       \
    ((((_idx) - ((_max) % DAL_MAPLE_MAX_INFO_IDX)) % DAL_MAPLE_MAX_INFO_IDX) * DAL_MAPLE_DATA_BITS)


/*
 * Data Declaration
 */
static uint32               acl_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         acl_sem[RTK_MAX_NUM_OF_UNIT];

typedef struct dal_maple_acl_entryTable_s
{
    uint16   data_field[DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD];
    uint8    data_fixed[DAL_MAPLE_MAX_NUM_OF_FIXED_FIELD];
    uint16   care_field[DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD];
    uint8    care_fixed[DAL_MAPLE_MAX_NUM_OF_FIXED_FIELD];
} dal_maple_acl_entryTable_t;

typedef struct dal_maple_acl_fieldLocation_s
{
    uint32   template_field_type;
    uint32   field_offset;
    uint32   field_length;
    uint32   data_offset;
} dal_maple_acl_fieldLocation_t;

typedef struct dal_maple_acl_fixField_s
{
    uint32                          data_field;     /* data field in chip view */
    uint32                          mask_field;     /* mask field in chip view */
    uint32                          position;       /* position in fix data */
    rtk_acl_fieldType_t             type;           /* field type in user view */
    dal_maple_acl_fieldLocation_t   *pField;
} dal_maple_acl_fixField_t;

typedef struct dal_maple_acl_entryField_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          field_number;   /* locate in how many fields */
    dal_maple_acl_fieldLocation_t *pField;
} dal_maple_acl_entryField_t;

typedef struct dal_maple_acl_entryFieldInfo_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          field_number;   /* locate in how many fields */
} dal_maple_acl_entryFieldInfo_t;

dal_maple_acl_entryFieldInfo_t dal_maple_acl_field_info_list[] =
{
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

rtk_acl_template_t dal_maple_acl_fixedTemplate_new[] =
{
    {{TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1, TMPLTE_FIELD_OTAG, TMPLTE_FIELD_SMAC0,TMPLTE_FIELD_SMAC1,
      TMPLTE_FIELD_SMAC2, TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2, TMPLTE_FIELD_ETHERTYPE,
      TMPLTE_FIELD_ITAG, TMPLTE_FIELD_RANGE_CHK}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1,TMPLTE_FIELD_IP_TOS_PROTO,
      TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_ICMP_IGMP, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_RANGE_CHK,
      TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1}},

    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_ETHERTYPE,
      TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1,
      TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1}},

    {{TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1, TMPLTE_FIELD_DIP2, TMPLTE_FIELD_DIP3, TMPLTE_FIELD_DIP4,
      TMPLTE_FIELD_DIP5, TMPLTE_FIELD_DIP6, TMPLTE_FIELD_DIP7, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_L4_SPORT,
      TMPLTE_FIELD_ICMP_IGMP, TMPLTE_FIELD_IP_TOS_PROTO}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_SIP2, TMPLTE_FIELD_SIP3, TMPLTE_FIELD_SIP4,
      TMPLTE_FIELD_SIP5, TMPLTE_FIELD_SIP6, TMPLTE_FIELD_SIP7, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_RANGE_CHK,
      TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1}},
};

rtk_acl_template_t dal_maple_acl_fixedTemplate[] =
{
    {{TMPLTE_FIELD_SPMMASK, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_OTAG, TMPLTE_FIELD_SMAC0,TMPLTE_FIELD_SMAC1,
      TMPLTE_FIELD_SMAC2, TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2, TMPLTE_FIELD_ETHERTYPE,
      TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_RANGE_CHK}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1,TMPLTE_FIELD_IP_TOS_PROTO,
      TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_ICMP_IGMP, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_OTAG,
      TMPLTE_FIELD_FIELD_SELECTOR_0, TMPLTE_FIELD_FIELD_SELECTOR_1}},

    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_ETHERTYPE,
      TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1,
      TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1}},

    {{TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1, TMPLTE_FIELD_DIP2, TMPLTE_FIELD_DIP3, TMPLTE_FIELD_DIP4,
      TMPLTE_FIELD_DIP5, TMPLTE_FIELD_DIP6, TMPLTE_FIELD_DIP7, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_L4_SPORT,
      TMPLTE_FIELD_ICMP_IGMP, TMPLTE_FIELD_IP_TOS_PROTO}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_SIP2, TMPLTE_FIELD_SIP3, TMPLTE_FIELD_SIP4,
      TMPLTE_FIELD_SIP5, TMPLTE_FIELD_SIP6, TMPLTE_FIELD_SIP7, TMPLTE_FIELD_ITAG, TMPLTE_FIELD_SMAC0,
      TMPLTE_FIELD_SMAC1, TMPLTE_FIELD_SMAC2}},
};


dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SPMM_0_1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SPN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 5,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_MGNT_VLAN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SWITCHMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP_NONZEROOFFSET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_L4_PROTO[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 3,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_L23_PROTO[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_OUTER_UNTAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_INNER_UNTAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ITAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_OTAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FRAME_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_TEMPLATE_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_DATYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM1,
        /* offset address */         0xd,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_PPPOE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM1,
        /* offset address */         0xf,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SPMM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPMMASK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM1,
        /* offset address */         0x0,
        /* length */                 13,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_SPM0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_DMAC[] =
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

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SMAC[] =
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

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ETHERTYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ETHERTYPE,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ARPOPCODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_OTAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_DEI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_OTAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ITAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_CFI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ITAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP4_SIP[] =
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

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP4_DIP[] =
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

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_SIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SIP7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x70,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x60,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x50,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x40,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x30,
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP2,
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

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_DIP[] =
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

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP4TOS_IP6TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IPDSCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0xA,
        /* length */                 6,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP4PROTO_IP6NH[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP4_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xe,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP4_TTL_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_L4_SRC_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_L4_DST_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_MOB_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_AUTH_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_DEST_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_FRAG_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_ROUTING_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP6_HOP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IGMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IGMP_GROUPIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x0,
        /* length */                 4,
        /* data offset */            0x18
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ICMP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_ICMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_TCP_ECN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_TCP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_TCP_NONZEROSEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_TELNET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SSH[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_HTTP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_HTTPS[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_SNMP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_UNKNOWN_L7[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ICMP_IGMP,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_VID_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_PORT_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_IP_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_RANGE,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0,
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FLOW_LABEL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FLOW_LABEL,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_IP_RANGE,
        /* offset address */         0xa,
        /* length */                 4,
        /* data offset */            0x10
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_LEN_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FLDSEL_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FIELD_SELECTOR_VALID_MSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x0,
        /* length */                 4,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RSPAN_TAGGED[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x5,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_EXTRA_TAGGED[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x4,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_L2_CRC_ERROR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x5,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_DROPPED[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RRCPHLNULLMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RRCPOP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x8,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RRCPREPLY[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RTKPP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xb,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RRCPVER[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_RRCPKEY[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xf,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FIELD_SELECTOR0[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FIELD_SELECTOR1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FIELD_SELECTOR2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FIELD_SELECTOR3[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_maple_acl_fieldLocation_t DAL_MAPLE_FIELD_FWD_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};

dal_maple_acl_fixField_t dal_maple_acl_igrFixField_list[] =
{
    {   /* data field       */  MAPLE_IACL_TIDtf,
        /* mask field name  */  MAPLE_IACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_MAPLE_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  MAPLE_IACL_FRAME_TYPE_L2tf,
        /* mask field name  */  MAPLE_IACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_MAPLE_FIELD_FRAME_TYPE
    },
    {   /* data field       */  MAPLE_IACL_ITAG_EXISTtf,
        /* mask field name  */  MAPLE_IACL_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_MAPLE_FIELD_ITAG_EXIST
    },
    {   /* data field       */  MAPLE_IACL_OTAG_EXISTtf,
        /* mask field name  */  MAPLE_IACL_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_MAPLE_FIELD_OTAG_EXIST
    },
    {   /* data field       */  MAPLE_IACL_ITAG_FMTtf,
        /* mask field name  */  MAPLE_IACL_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_MAPLE_FIELD_INNER_UNTAG
    },
    {   /* data field       */  MAPLE_IACL_OTAG_FMTtf,
        /* mask field name  */  MAPLE_IACL_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_MAPLE_FIELD_OUTER_UNTAG
    },
    {   /* data field       */  MAPLE_IACL_FRAME_TYPEtf,
        /* mask field name  */  MAPLE_IACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_MAPLE_FIELD_L23_PROTO
    },
    {   /* data field       */  MAPLE_IACL_FRAME_TYPE_L4tf,
        /* mask field name  */  MAPLE_IACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_MAPLE_FIELD_L4_PROTO
    },
    {   /* data field       */  MAPLE_IACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  MAPLE_IACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_MAPLE_FIELD_IP_NONZEROOFFSET
    },
    {   /* data field       */  MAPLE_IACL_DMAC_HIT_SWtf,
        /* mask field name  */  MAPLE_IACL_BMSK_DMAC_HIT_SWtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_SWITCHMAC,
        /* field pointer    */  DAL_MAPLE_FIELD_SWITCHMAC
    },
    {   /* data field       */  MAPLE_IACL_MGNT_VLANtf,
        /* mask field name  */  MAPLE_IACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_MAPLE_FIELD_MGNT_VLAN
    },
    {   /* data field       */  MAPLE_IACL_SPNtf,
        /* mask field name  */  MAPLE_IACL_BMSK_SPNtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_SPN,
        /* field pointer    */  DAL_MAPLE_FIELD_SPN
    },
    {   /* data field       */  MAPLE_IACL_SPMMASK_FIXtf,
        /* mask field name  */  MAPLE_IACL_BMSK_SPMMASK_FIXtf,
        /* position         */  22,
        /* field name       */  USER_FIELD_SPMM_0_1,
        /* field pointer    */  DAL_MAPLE_FIELD_SPMM_0_1
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
}; /* dal_maple_acl_igrFixField_list */

dal_maple_acl_entryField_t dal_maple_acl_field_list[] =
{
    {   /* field name    */           USER_FIELD_SPMM_0_1,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_SPMM_0_1
    },
    {   /* field name    */           USER_FIELD_SPN,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_SPN
    },
    {   /* field name    */           USER_FIELD_MGNT_VLAN,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_MGNT_VLAN
    },
    {   /* field name    */           USER_FIELD_SWITCHMAC,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_SWITCHMAC
    },
    {   /* field name    */           USER_FIELD_IP_NONZEROOFFSET,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP_NONZEROOFFSET
    },
    {   /* field name    */           USER_FIELD_L4_PROTO,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_L4_PROTO
    },
    {   /* field name    */           USER_FIELD_FRAME_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_L23_PROTO
    },
    {   /* field name    */           USER_FIELD_OTAG_FMT,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_OUTER_UNTAG
    },
    {   /* field name    */           USER_FIELD_ITAG_FMT,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_INNER_UNTAG
    },
    {   /* field name    */           USER_FIELD_OTAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_OTAG_EXIST
    },
    {   /* field name    */           USER_FIELD_ITAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ITAG_EXIST
    },
    {   /* field name    */           USER_FIELD_FRAME_TYPE_L2,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FRAME_TYPE
    },
    {   /* field name    */           USER_FIELD_TEMPLATE_ID,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_TEMPLATE_ID
    },
    {   /* field name    */           USER_FIELD_SPMM,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_SPMM
    },
    {   /* field name    */           USER_FIELD_SPM,
        /* field number  */           2,
        /* field pointer */           DAL_MAPLE_FIELD_SPM
    },
    {   /* field name    */           USER_FIELD_DATYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_DATYPE
    },
    {   /* field name    */           USER_FIELD_PPPOE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_PPPOE
    },
    {   /* field name    */           USER_FIELD_DMAC,
        /* field number  */           3,
        /* field pointer */           DAL_MAPLE_FIELD_DMAC
    },
    {   /* field name    */           USER_FIELD_SMAC,
        /* field number  */           3,
        /* field pointer */           DAL_MAPLE_FIELD_SMAC
    },
    {   /* field name    */           USER_FIELD_ETHERTYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ETHERTYPE
    },
    {   /* field name    */           USER_FIELD_ARPOPCODE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ARPOPCODE
    },
    {   /* field name    */           USER_FIELD_OTAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_OTAG_PRI
    },
    {   /* field name    */           USER_FIELD_DEI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_DEI_VALUE
    },
    {   /* field name    */           USER_FIELD_OTAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_OTAG_VID
    },
    {   /* field name    */           USER_FIELD_ITAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ITAG_PRI
    },
    {   /* field name    */           USER_FIELD_CFI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_CFI_VALUE
    },
    {   /* field name    */           USER_FIELD_ITAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ITAG_VID
    },
    {   /* field name    */           USER_FIELD_IP4_SIP,
        /* field number  */           2,
        /* field pointer */           DAL_MAPLE_FIELD_IP4_SIP
    },
    {   /* field name    */           USER_FIELD_IP4_DIP,
        /* field number  */           2,
        /* field pointer */           DAL_MAPLE_FIELD_IP4_DIP
    },
    {   /* field name    */           USER_FIELD_IP6_SIP,
        /* field number  */           8,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_SIP
    },
    {   /* field name    */           USER_FIELD_IP6_DIP,
        /* field number  */           8,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_DIP
    },
    {   /* field name    */           USER_FIELD_IP4TOS_IP6TC,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP4TOS_IP6TC
    },
    {   /* field name    */           USER_FIELD_IP_DSCP,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IPDSCP
    },
    {   /* field name    */           USER_FIELD_IP4PROTO_IP6NH,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP4PROTO_IP6NH
    },
    {   /* field name    */           USER_FIELD_IP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP4_FLAG
    },
    {   /* field name    */           USER_FIELD_IP4_TTL_IP6_HOPLIMIT,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP4_TTL_TYPE
    },
    {   /* field name    */           USER_FIELD_L4_SRC_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_L4_SRC_PORT
    },
    {   /* field name    */           USER_FIELD_L4_DST_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_L4_DST_PORT
    },
    {   /* field name    */           USER_FIELD_IP6_MOB_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_MOB_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_AUTH_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_AUTH_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_DEST_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_DEST_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_FRAG_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_FRAG_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_ROUTING_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_ROUTING_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_HOP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP6_HOP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IGMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IGMP_TYPE
    },
    {   /* field name    */           USER_FIELD_IGMP_GROUPIP,
        /* field number  */           3,
        /* field pointer */           DAL_MAPLE_FIELD_IGMP_GROUPIP
    },
    {   /* field name    */           USER_FIELD_TCP_ECN,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_TCP_ECN
    },
    {   /* field name    */           USER_FIELD_TCP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_TCP_FLAG
    },
    {   /* field name    */           USER_FIELD_TCP_NONZEROSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_TCP_NONZEROSEQ
    },
    {   /* field name    */           USER_FIELD_ICMP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ICMP_CODE
    },
    {   /* field name    */           USER_FIELD_ICMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_ICMP_TYPE
    },
    {   /* field name    */           USER_FIELD_TELNET,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_TELNET
    },
    {   /* field name    */           USER_FIELD_SSH,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_SSH
    },
    {   /* field name    */           USER_FIELD_HTTP,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_HTTP
    },
    {   /* field name    */           USER_FIELD_HTTPS,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_HTTPS
    },
    {   /* field name    */           USER_FIELD_SNMP,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_SNMP
    },
    {   /* field name    */           USER_FIELD_UNKNOWN_L7,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_UNKNOWN_L7
    },
    {   /* field name    */           USER_FIELD_VID_RANGE0,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_VID_RANGE
    },
    {   /* field name    */           USER_FIELD_PORT_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_PORT_RANGE
    },
    {   /* field name    */           USER_FIELD_IP_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_IP_RANGE
    },
    {   /* field name    */           USER_FIELD_FLOW_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MAPLE_FIELD_FLOW_LABEL
    },
    {   /* field name    */           USER_FIELD_LEN_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_LEN_RANGE
    },
    {   /* field name    */           USER_FIELD_FLDSEL_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FLDSEL_RANGE
    },
    {   /* field name    */           USER_FIELD_RRCPKEY,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_RRCPKEY
    },
    {   /* field name    */           USER_FIELD_RRCPVER,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_RRCPVER
    },
    {   /* field name    */           USER_FIELD_RTKPP,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_RTKPP
    },
    {   /* field name    */           USER_FIELD_RRCPREPLY,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_RRCPREPLY
    },
    {   /* field name    */           USER_FIELD_RRCPOP,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_RRCPOP
    },
    {   /* field name    */           USER_FIELD_RRCPHLNULLMAC,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_RRCPHLNULLMAC
    },
    {   /* field name    */           USER_FIELD_DROPPED,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_DROPPED
    },
    {   /* field name    */           USER_FIELD_L2_CRC_ERROR,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_L2_CRC_ERROR
    },
    {   /* field name    */           USER_FIELD_ETAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_EXTRA_TAGGED
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR_VALID_MSK,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FIELD_SELECTOR_VALID_MSK
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR0,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FIELD_SELECTOR0
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR1,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FIELD_SELECTOR1
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR2,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FIELD_SELECTOR2
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR3,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FIELD_SELECTOR3
    },
    {   /* field name    */           USER_FIELD_FWD_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MAPLE_FIELD_FWD_VID
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};

dal_maple_acl_templateField_t dal_maple_template_field_list[] =
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
        /* valid field location */  ((1<<0) | (1<<4) | (1<<8) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_SPM1,
        /* physical field ID*/      2,
        /* valid field location */  ((1<<1) | (1<<5) | (1<<9) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_RANGE_CHK,
        /* physical field ID*/      3,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC0,
        /* physical field ID*/      4,
        /* valid field location */  ((1<<0) | (1<<3) | (1<<6) | (1<<9))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC1,
        /* physical field ID*/      5,
        /* valid field location */  ((1<<1) | (1<<4) | (1<<7) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC2,
        /* physical field ID*/      6,
        /* valid field location */  ((1<<2) | (1<<5) | (1<<8) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC0,
        /* physical field ID*/      7,
        /* valid field location */  ((1<<0) | (1<<3) | (1<<6) | (1<<9))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC1,
        /* physical field ID*/      8,
        /* valid field location */  ((1<<1) | (1<<4) | (1<<7) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC2,
        /* physical field ID*/      9,
        /* valid field location */  ((1<<2) | (1<<5) | (1<<8) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_ETHERTYPE,
        /* physical field ID*/      10,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_OTAG,
        /* physical field ID*/      11,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ITAG,
        /* physical field ID*/      12,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SIP0,
        /* physical field ID*/      13,
        /* valid field location */  ((1<<0) | (1<<2) | (1<<4) | (1<<6) | (1<<8) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP1,
        /* physical field ID*/      14,
        /* valid field location */  ((1<<1) | (1<<3) | (1<<5) | (1<<7) | (1<<9) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP0,
        /* physical field ID*/      15,
        /* valid field location */  ((1<<0) | (1<<2) | (1<<4) | (1<<6) | (1<<8) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP1,
        /* physical field ID*/      16,
        /* valid field location */  ((1<<1) | (1<<3) | (1<<5) | (1<<7) | (1<<9) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_IP_TOS_PROTO,
        /* physical field ID*/      17,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L34_HEADER,
        /* physical field ID*/      18,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L4_SPORT,
        /* physical field ID*/      19,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L4_DPORT,
        /* physical field ID*/      20,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ICMP_IGMP,
        /* physical field ID*/      21,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_IP_RANGE,
        /* physical field ID*/      22,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* physical field ID*/      23,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* physical field ID*/      24,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* physical field ID*/      25,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* physical field ID*/      26,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* physical field ID*/      27,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SIP2,
        /* physical field ID*/      28,
        /* valid field location */  ((1<<2) | (1<<6))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP3,
        /* physical field ID*/      29,
        /* valid field location */  ((1<<3) | (1<<7))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP4,
        /* physical field ID*/      30,
        /* valid field location */  ((1<<0) | (1<<4) | (1<<8))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP5,
        /* physical field ID*/      31,
        /* valid field location */  ((1<<1) | (1<<5) | (1<<9))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP6,
        /* physical field ID*/      32,
        /* valid field location */  ((1<<2) | (1<<6) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP7,
        /* physical field ID*/      33,
        /* valid field location */  ((1<<3) | (1<<7) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP2,
        /* physical field ID*/      34,
        /* valid field location */  ((1<<2) | (1<<6))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP3,
        /* physical field ID*/      35,
        /* valid field location */  ((1<<3) | (1<<7))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP4,
        /* physical field ID*/      36,
        /* valid field location */  ((1<<0) | (1<<4) | (1<<8))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP5,
        /* physical field ID*/      37,
        /* valid field location */  ((1<<1) | (1<<5) | (1<<9))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP6,
        /* physical field ID*/      38,
        /* valid field location */  ((1<<2) | (1<<6) | (1<<10))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP7,
        /* physical field ID*/      39,
        /* valid field location */  ((1<<3) | (1<<7) | (1<<11))
    },
    {   /* field type       */      TMPLTE_FIELD_FWD_VID,
        /* physical field ID*/      40,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FLOW_LABEL,
        /* physical field ID*/      41,
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

#define ACL_SEM_LOCK2(unit, ret)    \
do {\
    if (osal_sem_mutex_take(acl_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_ACL), "semaphore lock failed");\
        ret = RT_ERR_SEM_LOCK_FAILED;\
        goto fail_ret; \
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

#define DAL_MAPLE_ACL_INDEX_CHK(UINT,TYPE,INDEX)    \
do {\
    RT_PARAM_CHK((INDEX >= HAL_MAX_NUM_OF_PIE_FILTER_ID(UINT)), RT_ERR_ENTRY_INDEX);\
    if (TYPE == ACL_PHASE_IGR_ACL && (INDEX >= HAL_MAX_NUM_OF_PIE_BLOCK(UINT)*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT)))\
        return RT_ERR_ENTRY_INDEX;\
} while(0)

#define DAL_MAPLE_ACL_ENTRYNUM(UINT,TYPE,ENTRYNUM)    \
do {\
        ENTRYNUM = HAL_MAX_NUM_OF_PIE_BLOCK(UINT)*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT);\
} while(0)

#define DAL_MAPLE_ACL_FIELD_TYPE_CHK(UINT,TYPE)    \
do {\
    uint32  i;\
    RT_PARAM_CHK((TYPE >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);\
    for (i = 0; dal_maple_acl_field_list[i].type != USER_FIELD_END; i++)\
    {\
        if (TYPE == dal_maple_acl_field_list[i].type)\
            break;\
    }\
    if (dal_maple_acl_field_list[i].type == USER_FIELD_END)\
        return RT_ERR_ACL_FIELD_TYPE;\
} while(0)

#define DAL_MAPLE_ACL_BLOCK_PWR_CHK(UINT,ENTRY_INDEX)    \
do {\
    uint32 blk_idx;\
    rtk_enable_t enable;\
    blk_idx = ENTRY_INDEX/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(UINT);\
    dal_maple_acl_blockPwrEnable_get(UINT, blk_idx, &enable);\
    if (DISABLED == enable)\
        return RT_ERR_ACL_BLOCK_POWER;\
} while(0)

/*
 * Function Declaration
 */
static int32 _dal_maple_acl_phy2logTmplteField(uint32 unit, uint32 phy_field_id, rtk_acl_templateFieldType_t *log_field_id);
static int32 _dal_maple_acl_log2PhyTmplteField(uint32 unit, uint32 field_idx, rtk_acl_templateFieldType_t log_field_id, uint32 *phy_field_id);
static int32 _dal_maple_acl_rule_del(uint32 unit, rtk_acl_clear_t *pClrIdx);
static int32 _dal_maple_acl_rule_move(uint32 unit, rtk_acl_move_t *pData);
static int32 _dal_maple_acl_igrRuleValidate_get(uint32 unit, rtk_acl_id_t entry_idx, uint32 *pValid);
static int32 _dal_maple_acl_igrRuleValidate_set(uint32 unit, rtk_acl_id_t entry_idx, uint32 valid);
static int32 _dal_maple_acl_igrRuleEntry_read(uint32 unit, rtk_acl_id_t entry_idx, uint8 *pEntry_buffer);
static int32 _dal_maple_acl_igrRuleEntry_write(uint32 unit, rtk_acl_id_t entry_idx, uint8 *pEntry_buffer);
static int32
_dal_maple_acl_igrRuleEntryField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);
static int32
_dal_maple_acl_igrRuleEntryField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);
static int32
_dal_maple_acl_igrRuleOperation_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);
static int32
_dal_maple_acl_igrRuleOperation_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);
static int32
_dal_maple_acl_igrRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction);
static int32
_dal_maple_acl_igrRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction);
static int32 _dal_maple_acl_igrRule_del(uint32 unit, rtk_acl_clear_t *pClrIdx);
static int32 _dal_maple_acl_igrRule_move(uint32 unit, rtk_acl_move_t *pData);

static void
_dal_maple_acl_buf2Field_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint16 *data16, uint16 *mask16,
    uint8 *pData, uint8 *pMask);

static void
_dal_maple_acl_field2Buf_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint32 data, uint32 mask,
    uint8 *pData, uint8 *pMask);

/* Function Name:
 *      dal_maple_aclMapper_init
 * Description:
 *      Hook acl module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook acl module before calling any acl APIs.
 */
int32
dal_maple_aclMapper_init(dal_mapper_t *pMapper)
{
    pMapper->acl_init = dal_maple_acl_init;
    pMapper->acl_portLookupEnable_get = dal_maple_acl_portLookupEnable_get;
    pMapper->acl_portLookupEnable_set = dal_maple_acl_portLookupEnable_set;
    pMapper->acl_lookupMissAct_get = dal_maple_acl_lookupMissAct_get;
    pMapper->acl_lookupMissAct_set = dal_maple_acl_lookupMissAct_set;
    pMapper->acl_blockPwrEnable_get = dal_maple_acl_blockPwrEnable_get;
    pMapper->acl_blockPwrEnable_set = dal_maple_acl_blockPwrEnable_set;
    pMapper->pie_blockLookupEnable_get = dal_maple_acl_blockLookupEnable_get;
    pMapper->pie_blockLookupEnable_set = dal_maple_acl_blockLookupEnable_set;
    pMapper->acl_ruleEntryFieldSize_get = dal_maple_acl_ruleEntryFieldSize_get;
    pMapper->acl_ruleEntrySize_get = dal_maple_acl_ruleEntrySize_get;
    pMapper->acl_ruleValidate_get = dal_maple_acl_ruleValidate_get;
    pMapper->acl_ruleValidate_set = dal_maple_acl_ruleValidate_set;
    pMapper->acl_ruleEntry_read = dal_maple_acl_ruleEntry_read;
    pMapper->acl_ruleEntry_write = dal_maple_acl_ruleEntry_write;
    pMapper->acl_ruleEntryField_get = dal_maple_acl_ruleEntryField_get;
    pMapper->acl_ruleEntryField_set = dal_maple_acl_ruleEntryField_set;
    pMapper->acl_ruleEntryField_read = dal_maple_acl_ruleEntryField_read;
    pMapper->acl_ruleEntryField_write = dal_maple_acl_ruleEntryField_write;
    pMapper->acl_ruleEntryField_check = dal_maple_acl_ruleEntryField_check;
    pMapper->acl_ruleEntryField_validate = dal_maple_acl_ruleEntryField_validate;
    pMapper->acl_ruleOperation_get = dal_maple_acl_ruleOperation_get;
    pMapper->acl_ruleOperation_set = dal_maple_acl_ruleOperation_set;
    pMapper->acl_ruleAction_get = dal_maple_acl_ruleAction_get;
    pMapper->acl_ruleAction_set = dal_maple_acl_ruleAction_set;
    pMapper->acl_rule_del = dal_maple_acl_rule_del;
    pMapper->acl_rule_move = dal_maple_acl_rule_move;
    pMapper->acl_templateSelector_get = dal_maple_acl_templateSelector_get;
    pMapper->acl_templateSelector_set = dal_maple_acl_templateSelector_set;
    pMapper->pie_template_get = dal_maple_acl_template_get;
    pMapper->pie_template_set = dal_maple_acl_template_set;
    pMapper->pie_templateField_check = dal_maple_acl_templateField_check;
    pMapper->acl_templateId_get = dal_maple_acl_templateId_get;
    pMapper->acl_blockGroupEnable_get = dal_maple_acl_blockGroupEnable_get;
    pMapper->acl_blockGroupEnable_set = dal_maple_acl_blockGroupEnable_set;
    pMapper->acl_statPktCnt_get = dal_maple_acl_statPktCnt_get;
    pMapper->acl_statPktCnt_clear = dal_maple_acl_statPktCnt_clear;
    pMapper->acl_statByteCnt_get = dal_maple_acl_statByteCnt_get;
    pMapper->acl_statByteCnt_clear = dal_maple_acl_statByteCnt_clear;
    pMapper->acl_stat_clearAll = dal_maple_acl_stat_clearAll;
    pMapper->acl_rangeCheckL4Port_get = dal_maple_acl_rangeCheckL4Port_get;
    pMapper->acl_rangeCheckL4Port_set = dal_maple_acl_rangeCheckL4Port_set;
    pMapper->acl_rangeCheckVid_get = dal_maple_acl_rangeCheckVid_get;
    pMapper->acl_rangeCheckVid_set = dal_maple_acl_rangeCheckVid_set;
    pMapper->pie_rangeCheckIp_get = dal_maple_acl_rangeCheckIp_get;
    pMapper->pie_rangeCheckIp_set = dal_maple_acl_rangeCheckIp_set;
    pMapper->acl_rangeCheckSrcPort_get = dal_maple_acl_rangeCheckSrcPort_get;
    pMapper->acl_rangeCheckSrcPort_set = dal_maple_acl_rangeCheckSrcPort_set;
    pMapper->acl_rangeCheckPacketLen_get = dal_maple_acl_rangeCheckPacketLen_get;
    pMapper->acl_rangeCheckPacketLen_set = dal_maple_acl_rangeCheckPacketLen_set;
    pMapper->acl_rangeCheckFieldSelector_get = dal_maple_acl_rangeCheckFieldSelector_get;
    pMapper->acl_rangeCheckFieldSelector_set = dal_maple_acl_rangeCheckFieldSelector_set;
    pMapper->pie_fieldSelector_get = dal_maple_acl_fieldSelector_get;
    pMapper->pie_fieldSelector_set = dal_maple_acl_fieldSelector_set;
    pMapper->acl_meterMode_get = dal_maple_acl_meterMode_get;
    pMapper->acl_meterMode_set = dal_maple_acl_meterMode_set;
    pMapper->pie_meterIncludeIfg_get = dal_maple_acl_meterIncludeIfg_get;
    pMapper->pie_meterIncludeIfg_set = dal_maple_acl_meterIncludeIfg_set;
    pMapper->acl_meterBurstSize_get = dal_maple_acl_meterBurstSize_get;
    pMapper->acl_meterBurstSize_set = dal_maple_acl_meterBurstSize_set;
    pMapper->pie_meterExceed_get = dal_maple_acl_meterExceed_get;
    pMapper->pie_meterEntry_get = dal_maple_acl_meterEntry_get;
    pMapper->pie_meterEntry_set = dal_maple_acl_meterEntry_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_acl_init
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
dal_maple_acl_init(uint32 unit)
{
    int32   ret;
    uint32  value, i, enable;

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

    ret = RT_ERR_OK;
    value = 0x0;

    ACL_SEM_LOCK(unit);
    /*enable per-port acl lookup*/
    enable = ENABLED;
    for(i = 0; i <= 28;  i++)
    {
        if ((ret = reg_array_field_write(unit,
                            MAPLE_ACL_PORT_LOOKUP_CTRLr,
                            i,
                            REG_ARRAY_INDEX_NONE,
                            MAPLE_LOOKUP_ENf,
                            &enable)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    /*enable all acl block power*/
    for (i = 0; i <= 11 ; i++)
    {
        if ((ret = reg_array_field_write(unit,
                            MAPLE_ACL_BLK_PWR_CTRLr,
                            REG_ARRAY_INDEX_NONE,
                            i,
                            MAPLE_PWR_ENf,
                            &enable)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    /*meter: include IPG*/
    value = 0x1;
    if ((ret = reg_field_write(unit, MAPLE_METER_GLB_CTRLr, MAPLE_INCL_PREIFGf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /*routing can skip source port filter, enable internal RW first*/
    value = 0x0;
    if ((ret = reg_read(unit, MAPLE_INT_RW_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    value |= 0x3;
    if ((ret = reg_write(unit, MAPLE_INT_RW_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    value = 0x0;
    if ((ret = reg_read(unit, MAPLE_DMY_REG27r, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    value |= 0x1;
    if ((ret = reg_write(unit, MAPLE_DMY_REG27r, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    value = 0x0;
    if ((ret = reg_read(unit, MAPLE_INT_RW_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    value &= 0xfffffffc;
    if ((ret = reg_write(unit, MAPLE_INT_RW_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);


    return RT_ERR_OK;
} /* end of dal_maple_acl_init */

/* Function Name:
 *      dal_maple_acl_blockPwrEnable_get
 * Description:
 *      Get the acl block power state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 * Output:
 *      pEnable   - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - block index is out of range
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The rule data are cleared if the block power is disabled.
 */
int32
dal_maple_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_ACL_BLK_PWR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_PWR_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pEnable=%d", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_acl_blockPwrEnable_get */

/* Function Name:
 *      dal_maple_acl_blockPwrEnable_set
 * Description:
 *      Set the acl block power state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      enable    - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - block index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      The rule data are cleared if the block power is disabled.
 */
int32
dal_maple_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_BLK_PWR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_PWR_ENf,
                        &enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_blockPwrEnable_set */

/* Function Name:
 *      dal_maple_acl_blockLookupEnable_get
 * Description:
 *      Get the acl block lookup state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 * Output:
 *      pEnable   - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - block index is out of range
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      1) The rule data are kept regardless of the lookup status.
 *      2) The lookup result is always false if the lookup state is disabled.
 */
int32
dal_maple_acl_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_ACL_BLK_LOOKUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_LOOKUP_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pEnable=%d", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_acl_blockLookupEnable_get */

/* Function Name:
 *      dal_maple_acl_blockLookupEnable_set
 * Description:
 *      Set the acl block lookup state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      enable    - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - block index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      1) The rule data are kept regardless of the lookup status.
 *      2) The lookup result is always false if the lookup state is disabled.
 */
int32
dal_maple_acl_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_BLK_LOOKUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_LOOKUP_ENf,
                        &enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_blockLookupEnable_set */

/* Function Name:
 *      dal_maple_acl_ruleEntryFieldSize_get
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
dal_maple_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size)
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
    for (i = 0; dal_maple_acl_igrFixField_list[i].type != USER_FIELD_END; i++)
    {
        if (type == dal_maple_acl_igrFixField_list[i].type)
        {
            index = i;
            break;
        }
    }

    if (dal_maple_acl_igrFixField_list[i].type != USER_FIELD_END)
    {
        /* Get field size */
        *pField_size = dal_maple_acl_igrFixField_list[index].pField[0].field_length;
    }
    else
    {
        uint32  field_number;

        /* Get chip specific field type from database */
        for (i = 0; dal_maple_acl_field_list[i].type != USER_FIELD_END; i++)
        {
            if (type == dal_maple_acl_field_list[i].type)
            {
                index = i;
                break;
            }
        }

        RT_PARAM_CHK((dal_maple_acl_field_list[i].type == USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

        field_number = dal_maple_acl_field_list[index].field_number;
        for (i = 0; i < (sizeof(dal_maple_acl_field_info_list)/sizeof(dal_maple_acl_entryFieldInfo_t)); ++i)
        {
            if (type == dal_maple_acl_field_info_list[i].type)
            {
                field_number = dal_maple_acl_field_info_list[i].field_number;
                break;
            }
        }

        /* Get field size */
        *pField_size = 0;
        for (i = 0; i < field_number; i++)
        {
            *pField_size += dal_maple_acl_field_list[index].pField[i].field_length;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pField_size=%d", *pField_size);
    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntryFieldSize_get */

/* Function Name:
 *      dal_maple_acl_ruleEntrySize_get
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
dal_maple_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size)
{
    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_size), RT_ERR_NULL_POINTER);

    /* dal_maple_acl_entryTable_t is padding to 4-byte alignment, so the sizeof() returns 56.
    *pEntry_size = sizeof(dal_maple_acl_entryTable_t);*/
    *pEntry_size = 54;

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntrySize_get */

/* Function Name:
 *      dal_maple_acl_ruleValidate_get
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 * Output:
 *      valid     - pointer buffer of valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);

    if ((ret = _dal_maple_acl_igrRuleValidate_get(unit, entry_idx, pValid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pValid=%d", *pValid);

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleValidate_get */

/* Function Name:
 *      dal_maple_acl_ruleValidate_set
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
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ACL_PHASE   - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
int32
dal_maple_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);

    if ((ret = _dal_maple_acl_igrRuleValidate_set(unit, entry_idx, valid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleValidate_set */

/* Function Name:
 *      dal_maple_acl_ruleEntry_read
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
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",\
        unit, phase, entry_idx);

    if ((ret = _dal_maple_acl_igrRuleEntry_read(unit, entry_idx, pEntry_buffer)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntry_read */

/* Function Name:
 *      dal_maple_acl_ruleEntry_write
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
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, pEntry_buffer=%x",\
        unit, phase, entry_idx, pEntry_buffer);

    if ((ret = _dal_maple_acl_igrRuleEntry_write(unit, entry_idx, pEntry_buffer)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntry_write */

static int32
_dal_maple_acl_igrRuleEntryBufField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint32              *buf,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;
    uint32  fixed_field = TRUE;
    uint32  block_idx;
    uint32  field_idx;
    uint32  field_number;
    uint32  field_data = 0;
    uint32  field_mask = 0;
    uint32  data_value;
    uint32  mask_value;
    uint32  field_match = FALSE;
    uint32  dal_field_type;
    rtk_acl_template_t      tmplte;
    rtk_acl_templateIdx_t   tmplte_idx;

    uint32  is_tcp_flag = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;
    uint32  is_icmp_code_type = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;

    /* handle fixed field type */
    switch (type)
    {
        case USER_FIELD_SPMM_0_1:
            field_data = MAPLE_IACL_SPMMASK_FIXtf;
            field_mask = MAPLE_IACL_BMSK_SPMMASK_FIXtf;
            break;
        case USER_FIELD_SPN:
            field_data = MAPLE_IACL_SPNtf;
            field_mask = MAPLE_IACL_BMSK_SPNtf;
            break;
        case USER_FIELD_MGNT_VLAN:
            field_data = MAPLE_IACL_MGNT_VLANtf;
            field_mask = MAPLE_IACL_BMSK_MGNT_VLANtf;
            break;
        case USER_FIELD_SWITCHMAC:
            field_data = MAPLE_IACL_DMAC_HIT_SWtf;
            field_mask = MAPLE_IACL_BMSK_DMAC_HIT_SWtf;
            break;
        case USER_FIELD_IP_NONZEROOFFSET:
            field_data = MAPLE_IACL_NOT_FIRST_FRAGtf;
            field_mask = MAPLE_IACL_BMSK_NOT_FIRST_FRAGtf;
            break;
        case USER_FIELD_L4_PROTO:
            field_data = MAPLE_IACL_FRAME_TYPE_L4tf;
            field_mask = MAPLE_IACL_BMSK_FRAME_TYPE_L4tf;
            break;
        case USER_FIELD_FRAME_TYPE:
            field_data = MAPLE_IACL_FRAME_TYPEtf;
            field_mask = MAPLE_IACL_BMSK_FRAME_TYPEtf;
            break;
        case USER_FIELD_OTAG_FMT:
            field_data = MAPLE_IACL_OTAG_FMTtf;
            field_mask = MAPLE_IACL_BMSK_OTAG_FMTtf;
            break;
        case USER_FIELD_ITAG_FMT:
            field_data = MAPLE_IACL_ITAG_FMTtf;
            field_mask = MAPLE_IACL_BMSK_ITAG_FMTtf;
            break;
        case USER_FIELD_OTAG_EXIST:
            field_data = MAPLE_IACL_OTAG_EXISTtf;
            field_mask = MAPLE_IACL_BMSK_OTAG_EXISTtf;
            break;
        case USER_FIELD_ITAG_EXIST:
            field_data = MAPLE_IACL_ITAG_EXISTtf;
            field_mask = MAPLE_IACL_BMSK_ITAG_EXISTtf;
            break;
        case USER_FIELD_FRAME_TYPE_L2:
            field_data = MAPLE_IACL_FRAME_TYPE_L2tf;
            field_mask = MAPLE_IACL_BMSK_FRAME_TYPE_L2tf;
            break;
        case USER_FIELD_TEMPLATE_ID:
            field_data = MAPLE_IACL_TIDtf;
            field_mask = MAPLE_IACL_BMSK_TIDtf;
            break;
        default:
            field_data = MAPLE_IACL_TIDtf;
            field_mask = MAPLE_IACL_BMSK_TIDtf;
            fixed_field = FALSE;
            break;
    }

    if ((ret = table_field_get(unit, MAPLE_IACLt, field_data, &data_value,  buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_IACLt, field_mask, &mask_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (TRUE == fixed_field)
    {
        switch (type)
        {
            case USER_FIELD_SPMM_0_1:
            case USER_FIELD_SPN:
            case USER_FIELD_MGNT_VLAN:
            case USER_FIELD_SWITCHMAC:
            case USER_FIELD_IP_NONZEROOFFSET:
            case USER_FIELD_L4_PROTO:
            case USER_FIELD_FRAME_TYPE:
            case USER_FIELD_OTAG_FMT:
            case USER_FIELD_ITAG_FMT:
            case USER_FIELD_OTAG_EXIST:
            case USER_FIELD_ITAG_EXIST:
            case USER_FIELD_FRAME_TYPE_L2:
            case USER_FIELD_TEMPLATE_ID:
                *pData = data_value;
                *pMask = mask_value;
                break;
            default:
                *pData = data_value;
                *pMask = mask_value;
                break;
        }

        return ret;
    }

    /* caculate entry in which block */
    block_idx = entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    /* find out the binding template of the block */
    dal_maple_acl_templateSelector_get(unit, block_idx, &tmplte_idx);
    /* get template fields */
    osal_memset(&tmplte, 0, sizeof(rtk_acl_template_t));
    dal_maple_acl_template_get(unit, tmplte_idx.template_id[data_value], &tmplte);

    /* translate field from RTK superset view to DAL view */
    for (field_idx = 0; dal_maple_acl_field_list[field_idx].type != USER_FIELD_END; field_idx++)
    {
        if (type == dal_maple_acl_field_list[field_idx].type)
            break;
    }

    dal_field_type = field_idx;

    /* search template to check all field types */
    field_match = FALSE;

    for (field_idx = 0; field_idx < DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        if (dal_maple_acl_field_list[field_idx].type == USER_FIELD_TCP_FLAG)
        {
            if (dal_maple_acl_field_list[dal_field_type].pField[0].template_field_type == TMPLTE_FIELD_L34_HEADER)
                is_tcp_flag = field_idx;
            if (dal_maple_acl_field_list[dal_field_type].pField[0].template_field_type == TMPLTE_FIELD_ICMP_IGMP)
                is_tcp_flag = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;
        }
        if ((dal_maple_acl_field_list[field_idx].type == USER_FIELD_ICMP_CODE) ||
            (dal_maple_acl_field_list[field_idx].type == USER_FIELD_ICMP_TYPE)||
            (dal_maple_acl_field_list[field_idx].type == USER_FIELD_IGMP_TYPE))
        {
            if (dal_maple_acl_field_list[dal_field_type].pField[0].template_field_type == TMPLTE_FIELD_L4_SPORT)
                is_icmp_code_type = field_idx;
            if (dal_maple_acl_field_list[dal_field_type].pField[0].template_field_type == TMPLTE_FIELD_ICMP_IGMP)
                is_icmp_code_type = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;
        }
    }

    for (field_idx = 0; field_idx < DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++)
    {
        for (field_number = 0; field_number < dal_maple_acl_field_list[dal_field_type].field_number; field_number++)
        {   /* check whether the user field type is pull in template, partial match is also allowed */
            if ((dal_maple_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) \
                || (is_tcp_flag == field_idx) \
                || (is_icmp_code_type == field_idx))
            {
                field_match = TRUE;
                switch (field_idx)
                {
                    case 0:
                        field_data = MAPLE_IACL_FIELD_0tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_0tf;
                        break;
                    case 1:
                        field_data = MAPLE_IACL_FIELD_1tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_1tf;
                        break;
                    case 2:
                        field_data = MAPLE_IACL_FIELD_2tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_2tf;
                        break;
                    case 3:
                        field_data = MAPLE_IACL_FIELD_3tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_3tf;
                        break;
                    case 4:
                        field_data = MAPLE_IACL_FIELD_4tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_4tf;
                        break;
                    case 5:
                        field_data = MAPLE_IACL_FIELD_5tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_5tf;
                        break;
                    case 6:
                        field_data = MAPLE_IACL_FIELD_6tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_6tf;
                        break;
                    case 7:
                        field_data = MAPLE_IACL_FIELD_7tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_7tf;
                        break;
                    case 8:
                        field_data = MAPLE_IACL_FIELD_8tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_8tf;
                        break;
                    case 9:
                        field_data = MAPLE_IACL_FIELD_9tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_9tf;
                        break;
                    case 10:
                        field_data = MAPLE_IACL_FIELD_10tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_10tf;
                        break;
                    case 11:
                        field_data = MAPLE_IACL_FIELD_11tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_11tf;
                        break;
                    default:
                        break;
                }

                /* data */
                if ((ret = table_field_get(unit, MAPLE_IACLt, field_data, &data_value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                /* mask */
                if ((ret = table_field_get(unit, MAPLE_IACLt, field_mask, &mask_value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                /* find out corresponding data field from field data */
                _dal_maple_acl_field2Buf_get(unit, dal_field_type, type, field_number, data_value, mask_value, pData, pMask);

            } /* if (dal_maple_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) */
        } /* for (field_number = 0; field_number < dal_maple_acl_field_list[dal_field_type].field_number; field_number++) */
    } /* for (field_idx = 0; field_idx < DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++) */

    /* can't find then return */
    if ( field_match != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pData=%x, pMask=%x", *pData, *pMask);

#if defined(CONFIG_SDK_ENDIAN_LITTLE)
    {
        uint32  index;
        uint8   temp=0, *pData_temp=NULL, *pMask_temp=NULL;
        uint32  total_length = 0;

        dal_maple_acl_ruleEntryFieldSize_get(unit, type, &total_length);

        for (index = 0; index <= (total_length / DAL_MAPLE_BUFFER_UNIT_LENGTH_BITS); index++)
        {
            pData_temp = pData + (index * (DAL_MAPLE_BUFFER_UNIT_LENGTH_BITS / 8));
            temp = *pData_temp;
            *pData_temp = *(pData_temp + 3);
            *(pData_temp + 3) = temp;
            temp = *(pData_temp+1);
            *(pData_temp + 1) = *(pData_temp + 2);
            *(pData_temp + 2) = temp;

            pMask_temp = pMask + (index * (DAL_MAPLE_BUFFER_UNIT_LENGTH_BITS / 8));
            temp = *pMask_temp;
            *pMask_temp = *(pMask_temp + 3);
            *(pMask_temp + 3) = temp;
            temp = *(pMask_temp + 1);
            *(pMask_temp + 1) = *(pMask_temp + 2);
            *(pMask_temp + 2) = temp;
        }
    }
#endif

    return RT_ERR_OK;
}   /* end of _dal_maple_acl_igrRuleEntryBufField_read */

static int32
_dal_maple_acl_igrRuleEntryBufField_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint32              *buf,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret;
    uint32  value;
    uint32  block_idx;
    uint32  field_idx;
    uint32  field_number;
    uint32  field_data = 0;
    uint32  field_mask = 0;
    uint16  data_value_16;
    uint16  mask_value_16;
    uint32  data_value;
    uint32  mask_value;
    uint32  field_match = FALSE;
    uint32  dal_field_type;
    rtk_acl_template_t      tmplte;
    rtk_acl_templateIdx_t   tmplte_idx;

    uint32  is_tcp_flag = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;
    uint32  is_icmp_code_type = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;

    switch (type)
    {
        case USER_FIELD_SPMM_0_1:
            field_data = MAPLE_IACL_SPMMASK_FIXtf;
            field_mask = MAPLE_IACL_BMSK_SPMMASK_FIXtf;
            break;
        case USER_FIELD_SPN:
            field_data = MAPLE_IACL_SPNtf;
            field_mask = MAPLE_IACL_BMSK_SPNtf;
            break;
        case USER_FIELD_MGNT_VLAN:
            field_data = MAPLE_IACL_MGNT_VLANtf;
            field_mask = MAPLE_IACL_BMSK_MGNT_VLANtf;
            break;
        case USER_FIELD_SWITCHMAC:
            field_data = MAPLE_IACL_DMAC_HIT_SWtf;
            field_mask = MAPLE_IACL_BMSK_DMAC_HIT_SWtf;
            break;
        case USER_FIELD_IP_NONZEROOFFSET:
            field_data = MAPLE_IACL_NOT_FIRST_FRAGtf;
            field_mask = MAPLE_IACL_BMSK_NOT_FIRST_FRAGtf;
            break;
        case USER_FIELD_L4_PROTO:
            field_data = MAPLE_IACL_FRAME_TYPE_L4tf;
            field_mask = MAPLE_IACL_BMSK_FRAME_TYPE_L4tf;
            break;
        case USER_FIELD_FRAME_TYPE:
            field_data = MAPLE_IACL_FRAME_TYPEtf;
            field_mask = MAPLE_IACL_BMSK_FRAME_TYPEtf;
            break;
        case USER_FIELD_OTAG_FMT:
            field_data = MAPLE_IACL_OTAG_FMTtf;
            field_mask = MAPLE_IACL_BMSK_OTAG_FMTtf;
            break;
        case USER_FIELD_ITAG_FMT:
            field_data = MAPLE_IACL_ITAG_FMTtf;
            field_mask = MAPLE_IACL_BMSK_ITAG_FMTtf;
            break;
        case USER_FIELD_OTAG_EXIST:
        case USER_FIELD_OTAG_VID:
            field_data = MAPLE_IACL_OTAG_EXISTtf;
            field_mask = MAPLE_IACL_BMSK_OTAG_EXISTtf;
            break;
        case USER_FIELD_ITAG_EXIST:
        case USER_FIELD_ITAG_VID:
            field_data = MAPLE_IACL_ITAG_EXISTtf;
            field_mask = MAPLE_IACL_BMSK_ITAG_EXISTtf;
            break;
        case USER_FIELD_FRAME_TYPE_L2:
            field_data = MAPLE_IACL_FRAME_TYPE_L2tf;
            field_mask = MAPLE_IACL_BMSK_FRAME_TYPE_L2tf;
            break;
        case USER_FIELD_TEMPLATE_ID:
            field_data = MAPLE_IACL_TIDtf;
            field_mask = MAPLE_IACL_BMSK_TIDtf;
            break;
        default:
            field_data = MAPLE_IACL_TIDtf;
            field_mask = MAPLE_IACL_BMSK_TIDtf;
            break;
    }
    if ((ret = table_field_get(unit, MAPLE_IACLt, field_data, &data_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_IACLt, field_mask, &mask_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* handle fixed field type */
    switch (type)
    {
        case USER_FIELD_SPMM_0_1:
        case USER_FIELD_SPN:
        case USER_FIELD_MGNT_VLAN:
        case USER_FIELD_SWITCHMAC:
        case USER_FIELD_IP_NONZEROOFFSET:
        case USER_FIELD_L4_PROTO:
        case USER_FIELD_FRAME_TYPE:
        case USER_FIELD_OTAG_FMT:
        case USER_FIELD_ITAG_FMT:
        case USER_FIELD_OTAG_EXIST:
        case USER_FIELD_ITAG_EXIST:
        case USER_FIELD_FRAME_TYPE_L2:
        case USER_FIELD_TEMPLATE_ID:
            data_value = *pData;
            mask_value = *pMask;
            if (USER_FIELD_TEMPLATE_ID == type)
            {
                mask_value = 0x3;   /*force to be valid*/
            }
            if ((ret = table_field_set(unit, MAPLE_IACLt, field_data, &data_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MAPLE_IACLt, field_mask, &mask_value, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if (USER_FIELD_ITAG_VID == type || USER_FIELD_OTAG_VID == type)
                break;

            return ret;
        default:
            break;
    }

    /* caculate entry in which block */
    block_idx = entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    /* find out the binding template of the block */
    dal_maple_acl_templateSelector_get(unit, block_idx, &tmplte_idx);
    /* get field 'template ID' to know the template that the entry maps to */
    if ((ret = table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_TIDtf, &data_value, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* get template fields */
    osal_memset(&tmplte, 0, sizeof(rtk_acl_template_t));
    dal_maple_acl_template_get(unit, tmplte_idx.template_id[data_value], &tmplte);

    /* translate field from RTK superset view to DAL view */
    for (field_idx = 0; dal_maple_acl_field_list[field_idx].type != USER_FIELD_END; field_idx++)
    {
        if (type == dal_maple_acl_field_list[field_idx].type)
        {
            break;
        }
    }
    dal_field_type = field_idx;
    /* search template to check all field types */
    field_match = FALSE;

    for (field_idx = 0; field_idx < DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        if (type == USER_FIELD_TCP_FLAG)
        {
            if(tmplte.field[field_idx] == TMPLTE_FIELD_L34_HEADER)
                is_tcp_flag = field_idx;
            if(tmplte.field[field_idx] == TMPLTE_FIELD_ICMP_IGMP)
                is_tcp_flag = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;
        }
        if ((type == USER_FIELD_ICMP_CODE) || (type == USER_FIELD_ICMP_TYPE) || (type == USER_FIELD_IGMP_TYPE))
        {
            if (tmplte.field[field_idx] == TMPLTE_FIELD_L4_SPORT)
                is_icmp_code_type = field_idx;
            if (tmplte.field[field_idx] == TMPLTE_FIELD_ICMP_IGMP)
                is_icmp_code_type = DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD + 1;
        }
    }

    for (field_idx = 0; field_idx < DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++)
    {
        for (field_number = 0; field_number < dal_maple_acl_field_list[dal_field_type].field_number; field_number++)
        {   /* check whether the user field type is pulled in template, partial match is allowed */
            if ((dal_maple_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) \
                || (is_tcp_flag == field_idx) \
                || (is_icmp_code_type == field_idx))
            {
                uint32  field_offset, field_length;

                field_match = TRUE;
                field_offset = dal_maple_acl_field_list[dal_field_type].pField[field_number].field_offset;
                field_length = dal_maple_acl_field_list[dal_field_type].pField[field_number].field_length;

                _dal_maple_acl_buf2Field_get(unit, dal_field_type, type,
                    field_number, &data_value_16, &mask_value_16, pData, pMask);

                switch (field_idx)
                {
                    case 0:
                        field_data = MAPLE_IACL_FIELD_0tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_0tf;
                        break;
                    case 1:
                        field_data = MAPLE_IACL_FIELD_1tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_1tf;
                        break;
                    case 2:
                        field_data = MAPLE_IACL_FIELD_2tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_2tf;
                        break;
                    case 3:
                        field_data = MAPLE_IACL_FIELD_3tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_3tf;
                        break;
                    case 4:
                        field_data = MAPLE_IACL_FIELD_4tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_4tf;
                        break;
                    case 5:
                        field_data = MAPLE_IACL_FIELD_5tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_5tf;
                        break;
                    case 6:
                        field_data = MAPLE_IACL_FIELD_6tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_6tf;
                        break;
                    case 7:
                        field_data = MAPLE_IACL_FIELD_7tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_7tf;
                        break;
                    case 8:
                        field_data = MAPLE_IACL_FIELD_8tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_8tf;
                        break;
                    case 9:
                        field_data = MAPLE_IACL_FIELD_9tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_9tf;
                        break;
                    case 10:
                        field_data = MAPLE_IACL_FIELD_10tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_10tf;
                        break;
                    case 11:
                        field_data = MAPLE_IACL_FIELD_11tf;
                        field_mask = MAPLE_IACL_BMSK_FIELD_11tf;
                        break;
                }
                /* data */
                if ((ret = table_field_get(unit, MAPLE_IACLt, field_data, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                value &= ~(((1 << field_length)-1) << field_offset);
                value |= data_value_16;

                if ((ret = table_field_set(unit, MAPLE_IACLt, field_data, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, MAPLE_IACLt, field_mask, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                value &= ~(((1 << field_length)-1) << field_offset);
                value |= mask_value_16;

                if ((ret = table_field_set(unit, MAPLE_IACLt, field_mask, &value, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
            } /* if (dal_maple_acl_field_list[dal_field_type].pField[field_number].template_field_type == tmplte.field[field_idx]) */
        } /* for (field_number = 0; field_number < dal_maple_acl_field_list[dal_field_type].field_number; field_number++) */
    } /* for (field_idx = 0; field_idx < DAL_MAPLE_MAX_NUM_OF_TEMPLATE_FIELD; field_idx ++) */
    /* no matched filed in template */
    if (field_match != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    return RT_ERR_OK;
}   /* end of _dal_maple_acl_igrRuleEntryBufField_write */


/* Function Name:
 *      dal_maple_acl_ruleEntryField_get
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
dal_maple_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    dal_maple_acl_entryTable_t  tableBuf;
    uint8                       *ptr;
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);

    osal_memset(&tableBuf, 0, sizeof(dal_maple_acl_entryTable_t));
    ptr = (uint8 *)&tableBuf;
    osal_memcpy(ptr, pEntry_buffer, 24);
    osal_memcpy(&ptr[25], &pEntry_buffer[24], 27);
    osal_memcpy(&ptr[53], &pEntry_buffer[51], 3);

    if ((ret = _dal_maple_acl_igrRuleEntryBufField_read(unit, entry_idx,
            type, (uint32 *)&tableBuf, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntryField_get */

/* Function Name:
 *      dal_maple_acl_ruleEntryField_set
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
dal_maple_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    dal_maple_acl_entryTable_t  tableBuf;
    uint8                       *ptr;
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);

    osal_memset(&tableBuf, 0, sizeof(dal_maple_acl_entryTable_t));
    ptr = (uint8 *)&tableBuf;
    osal_memcpy(ptr, pEntry_buffer, 24);
    osal_memcpy(&ptr[25], &pEntry_buffer[24], 27);
    osal_memcpy(&ptr[53], &pEntry_buffer[51], 3);

    if ((ret = _dal_maple_acl_igrRuleEntryBufField_write(unit, entry_idx,
        type, (uint32 *)&tableBuf, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memcpy(pEntry_buffer, ptr, 24);
    osal_memcpy(&pEntry_buffer[24], &ptr[25], 27);
    osal_memcpy(&pEntry_buffer[51], &ptr[53], 3);

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntryField_set */

/* Function Name:
 *      dal_maple_acl_ruleEntryField_read
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
dal_maple_acl_ruleEntryField_read(
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
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);

    if ((ret = _dal_maple_acl_igrRuleEntryField_read(unit, entry_idx, type, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntryField_read */

/* Function Name:
 *      dal_maple_acl_ruleEntryField_write
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
dal_maple_acl_ruleEntryField_write(
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
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRuleEntryField_write(unit, entry_idx, type, pData, pMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleEntryField_write */

/* Function Name:
 *      dal_maple_acl_ruleEntryField_check
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
dal_maple_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, type=%d",\
            unit, phase, type);

    if (phase != ACL_PHASE_IGR_ACL)
        return RT_ERR_ACL_PHASE;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);

    return RT_ERR_OK;
}   /* end of dal_maple_acl_ruleEntryField_check */

/* Function Name:
 *      dal_maple_acl_ruleEntryField_validate
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
dal_maple_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type)
{
    acl_igrRule_entry_t      entry;
    int32                   ret;
    uint8                   data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8                   mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,entry_idx=%d,type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

    /* function body */
    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_maple_acl_igrRuleEntryBufField_read(unit,
            entry_idx, type, (uint32 *)&entry, data, mask)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_maple_acl_ruleEntryField_validate */

/* Function Name:
 *      dal_maple_acl_ruleOperation_get
 * Description:
 *      Get ACL rule operation.
 * Input:
 *      unit       - unit id
 *      phase      - ACL lookup phase
 *      entry_idx  - ACL entry index
 * Output:
 *      pOperation - operation configuration
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
dal_maple_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRuleOperation_get(unit, entry_idx, pOperation)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleOperation_get */

/* Function Name:
 *      dal_maple_acl_ruleOperation_set
 * Description:
 *      Set ACL rule operation.
 * Input:
 *      unit       - unit id
 *      phase      - ACL lookup phase
 *      entry_idx  - ACL entry index
 *      pOperation - operation configuration
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
 *      None.
 */
int32
dal_maple_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRuleOperation_set(unit, entry_idx, pOperation)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleOperation_set */

/* Function Name:
 *      dal_maple_acl_ruleAction_get
 * Description:
 *      Get the ACL rule action configuration.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 * Output:
 *      pAction   - action mask and data configuration
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
dal_maple_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRuleAction_get(unit, entry_idx, &pAction->igr_acl)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleAction_get */

/* Function Name:
 *      dal_maple_acl_ruleAction_set
 * Description:
 *      Set the ACL rule action configuration.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      pAction   - action mask and data configuration
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
dal_maple_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{

    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRuleAction_set(unit, entry_idx, &pAction->igr_acl)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_ruleAction_set */

/* Function Name:
 *      dal_maple_acl_rule_del
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Entry fields, operations and actions are all cleared.
 */
int32
dal_maple_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRule_del(unit, pClrIdx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_rule_del */

/* Function Name:
 *      dal_maple_acl_rule_move
 * Description:
 *      Move the specified ACL rules.
 * Input:
 *      unit  - unit id
 *      phase - ACL lookup phase
 *      pData - movement info
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
 *      1) Entry fields, operations and actions are all moved.
 *      2) The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      3) (move_from + length) and (move_to + length) must <= the number of ACL rule
 */
int32
dal_maple_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData)
{
    int32   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((phase != ACL_PHASE_IGR_ACL), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if (phase == ACL_PHASE_IGR_ACL)
    {
        if ((ret = _dal_maple_acl_igrRule_move(unit, pData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_rule_move */

/* Function Name:
 *      dal_maple_acl_templateSelector_get
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
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_templateSelector_get(
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
                        MAPLE_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_BLK_TMPLTE1f,
                        &pTemplate_idx->template_id[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_BLK_TMPLTE2f,
                        &pTemplate_idx->template_id[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        MAPLE_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_BLK_TMPLTE3f,
                        &pTemplate_idx->template_id[2])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "template1_idx=%d, template2_idx=%d, template3_idx=%d",\
        pTemplate_idx->template_id[0], pTemplate_idx->template_id[1], pTemplate_idx->template_id[2]);

    return RT_ERR_OK;
} /* end of dal_maple_acl_templateSelector_get */

/* Function Name:
 *      dal_maple_acl_templateSelector_set
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
 *      RT_ERR_NOT_INIT                  - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX           - invalid block index
 *      RT_ERR_PIE_TEMPLATE_INDEX        - invalid template index
 *      RT_ERR_ACL_TEMPLATE_INCOMPATIBLE - try to map a ACL block to an incompatible template
 *      RT_ERR_INPUT                     - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx)
{
    int32   ret;
    uint32  i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, template0_idx=%d, template1_idx=%d, template2_idx=%d", \
        unit, block_idx, template_idx.template_id[0], template_idx.template_id[1], template_idx.template_id[2]);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);

    for (i = 0; i < RTK_MAX_NUM_OF_ACL_BLOCK_TEMPLATE; i++)
    {
        RT_PARAM_CHK((template_idx.template_id[i] >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    }

    ACL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_BLK_TMPLTE1f,
                        &template_idx.template_id[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_BLK_TMPLTE2f,
                        &template_idx.template_id[1])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        MAPLE_BLK_TMPLTE3f,
                        &template_idx.template_id[2])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_templateSelector_set */

/* Function Name:
 *      dal_maple_acl_template_get
 * Description:
 *      Get the template content of specific template index.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 * Output:
 *      pTemplate   - template content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX - invalid template index
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_template_get(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate)
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
        *pTemplate = dal_maple_acl_fixedTemplate_new[template_id];
        return RT_ERR_OK;
    }

    /* User defined template then get value from chip */
    for (field_idx = 0; field_idx < RTK_MAX_NUM_OF_ACL_TEMPLATE_FIELD; field_idx++)
    {
        ACL_SEM_LOCK(unit);
        if ((ret = reg_array_field_read(unit, MAPLE_ACL_TMPLTE_CTRLr, template_id, field_idx, MAPLE_TMPLTE_FIELDf, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        ACL_SEM_UNLOCK(unit);

        if ((ret = _dal_maple_acl_phy2logTmplteField(unit, value, &pTemplate->field[field_idx])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_template_get */

/* Function Name:
 *      dal_maple_acl_template_set
 * Description:
 *      Set the template content of specific template index.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 *      pTemplate   - template content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX - invalid template index
 *      RT_ERR_ACL_FIELD_TYPE     - invalid ACL field type
 *      RT_ERR_PIE_FIELD_LOCATION - invalid field location
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_template_set(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate)
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
    for (field_idx = 0; field_idx < RTK_MAX_NUM_OF_ACL_TEMPLATE_FIELD; field_idx++)
    {
        if ((ret = _dal_maple_acl_log2PhyTmplteField(unit,
                                field_idx,
                                pTemplate->field[field_idx],
                                &physical_tmplte.field[field_idx])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    /* set value to CHIP */
    for (field_idx = 0; field_idx < RTK_MAX_NUM_OF_ACL_TEMPLATE_FIELD; field_idx++)
    {
        value = physical_tmplte.field[field_idx];

        ACL_SEM_LOCK(unit);
        if ((ret = reg_array_field_write(unit, MAPLE_ACL_TMPLTE_CTRLr, template_id, field_idx, MAPLE_TMPLTE_FIELDf, &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        ACL_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_template_set */

/* Function Name:
 *      dal_maple_acl_templateField_check
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
dal_maple_acl_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_acl_templateFieldType_t type)
{
    uint32  i;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    if (phase != PIE_PHASE_VACL)
        return RT_ERR_ACL_PHASE;

    /* Check field type */
    for (i = 0; dal_maple_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (type == dal_maple_template_field_list[i].field_type)
            break;
    }

    if (dal_maple_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_ACL_FIELD_TYPE;
        return ret;
    }

    return RT_ERR_OK;
}    /* end of dal_maple_acl_templateField_check */

/* Function Name:
 *      dal_maple_acl_templateId_get
 * Description:
 *      Get physic template Id from entry idx.
 * Input:
 *      unit         - unit id
 *      phase        - ACL lookup phase
 *      entry_idx    - ACL entry index
 * Output:
 *      pTemplate_id - pointer to template id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
int32
dal_maple_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id)
{
    int32     ret;
    uint32    block_idx;
    rtk_acl_templateIdx_t   template_idx;
    uint8     field_data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8     field_mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTemplate_id), RT_ERR_NULL_POINTER);

    /* function body */
    /* get entry field template id */
    if ((ret = dal_maple_acl_ruleEntryField_read(unit, phase, entry_idx, \
            USER_FIELD_TEMPLATE_ID, field_data, field_mask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    /* caculate entry in which block */
    block_idx = entry_idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    /* find out the binding template of the block */
    if ((ret = dal_maple_acl_templateSelector_get(unit, block_idx, &template_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    *pTemplate_id = template_idx.template_id[field_data[0]];

    return RT_ERR_OK;
}   /* end of dal_maple_acl_templateId_get */

/* Function Name:
 *      dal_maple_acl_blockGroupEnable_get
 * Description:
 *      Set the block aggregation.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      aggr_type - aggregator type
 * Output:
 *      pEnable   - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - invalid block index
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      1) If multiple physical blocks are aggregated to a logical block,
 *         it only outputs a single hit result and the hit result will be
 *         the entry with lowest index.
 *      2) Aggregate ingress ACL block with egress ACL block is forbidden.
 *      3) For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      4) For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      5) For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      6) For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      7) If multiple aggregator types are applied to the same block index, then
 *         the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *         ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 */
int32
dal_maple_acl_blockGroupEnable_get(
    uint32                unit,
    uint32                block_idx,
    rtk_acl_blockGroup_t  aggr_type,
    rtk_enable_t          *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, aggr_type=%d", unit, block_idx, aggr_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)) - 1), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(aggr_type >= ACL_BLOCK_GROUP_END, RT_ERR_INPUT);

    if (aggr_type != ACL_BLOCK_GROUP_1)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "parameter aggr_type error");
        return RT_ERR_INPUT;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                                     MAPLE_ACL_BLK_GROUP_CTRLr,
                                     REG_ARRAY_INDEX_NONE,
                                     block_idx,
                                     MAPLE_ENABLEf,
                                     &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    *pEnable = value;
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_blockGroupEnable_get */

/* Function Name:
 *      dal_maple_acl_blockGroupEnable_set
 * Description:
 *      Set the block aggregation.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      aggr_type - aggregator type
 *      enable    - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX - invalid block index
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      1) If multiple physical blocks are aggregated to a logical block,
 *         it only outputs a single hit result and the hit result will be
 *         the entry with lowest index.
 *      2) Aggregate ingress ACL block with egress ACL block is forbidden.
 *      3) For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      4) For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      5) For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      6) For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      7) If multiple aggregator types are applied to the same block index, then
 *         the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *         ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 */
int32
dal_maple_acl_blockGroupEnable_set(
    uint32                unit,
    uint32                block_idx,
    rtk_acl_blockGroup_t  aggr_type,
    rtk_enable_t          enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, aggr_type=%d, enable=%d",\
        unit, block_idx, aggr_type, enable);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)) - 1), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(aggr_type >= ACL_BLOCK_GROUP_END, RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    if (aggr_type != ACL_BLOCK_GROUP_1)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "parameter aggr_type error");
        return RT_ERR_INPUT;
    }

    ACL_SEM_LOCK(unit);
    value = enable;
    if ((ret = reg_array_field_write(unit,
                                     MAPLE_ACL_BLK_GROUP_CTRLr,
                                     REG_ARRAY_INDEX_NONE,
                                     block_idx,
                                     MAPLE_ENABLEf,
                                     &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_blockGroupEnable_set */

/* Function Name:
 *      dal_maple_acl_statPktCnt_get
 * Description:
 *      Get packet-based statistic counter of the log id.
 * Input:
 *      unit     - unit id
 *      log_id   - log id
 * Output:
 *      pPkt_cnt - pointer buffer of packet count
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
dal_maple_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt)
{
    int32   ret;
    uint32  cnt[2];
    log_entry_t entry;
    uint32 MSB;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= (HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pPkt_cnt), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    if (log_id % 2 == 1)
       MSB = 1;
    else
       MSB = 0;
    log_id = log_id/2;

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_LOGt, MAPLE_LOG_CNTRtf, cnt, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* get the packet counter from a 64-bit byte counter */
    if (1 == MSB)
       *pPkt_cnt = cnt[1];
    else
       *pPkt_cnt = cnt[0];

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pPkt_cnt=%d", *pPkt_cnt);

    return RT_ERR_OK;
} /* end of dal_maple_acl_statPktCnt_get */

/* Function Name:
 *      dal_maple_acl_statPktCnt_clear
 * Description:
 *      Set packet-based statistic counter of the log id.
 * Input:
 *      unit   - unit id
 *      log_id - log id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
int32
dal_maple_acl_statPktCnt_clear(uint32 unit, uint32 log_id)
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

    /* form the index in ASIC view: Log (Write): ADDR [11:0]={Reserved[11:9],  CLRH[8:8], CLRL[7:7] , Index [6:0] } */
    log_id = log_id/2;
    if (msb_word == TRUE)
        log_id |= (1<<8);
    else
        log_id |= (1<<7);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_statPktCnt_clear */

/* Function Name:
 *      dal_maple_acl_statByteCnt_get
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
dal_maple_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt)
{
    int32   ret;
    uint32  cnt[2];
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pByte_cnt), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_LOGt, MAPLE_LOG_CNTRtf, cnt, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    *pByte_cnt = (uint64)cnt[0] | (uint64)cnt[1] << 32;

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pByte_cnt=%ull", *pByte_cnt);
    return RT_ERR_OK;
} /* end of dal_maple_acl_statByteCnt_get */

/* Function Name:
 *      dal_maple_acl_statByteCnt_clear
 * Description:
 *      Reset byte-based statistic counter of the log id.
 * Input:
 *      unit     - unit id
 *      log_id   - log id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
int32
dal_maple_acl_statByteCnt_clear(uint32 unit, uint32 log_id)
{
    int32   ret;
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)), RT_ERR_ENTRY_INDEX);

    osal_memset(&entry, 0x0, sizeof(log_entry_t));/*no used for clear*/

    /* form the index in ASIC view: Log (Write): ADDR [11:0]={Reserved[11:9],  CLRH[8:8], CLRL[7:7] , Index [6:0] } */
    log_id |= (0x3<<7);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_statByteCnt_clear */

/* Function Name:
 *      dal_maple_acl_stat_clearAll
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
dal_maple_acl_stat_clearAll(uint32 unit)
{
    uint32 ret;
    uint32 i, entry_num;

    entry_num = HAL_MAX_NUM_OF_PIE_COUNTER(unit);

    for (i = 0; i < entry_num; i++)
    {
        if ((ret = dal_maple_acl_statByteCnt_clear(unit, i)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_stat_clearAll */

/* Function Name:
 *      dal_maple_acl_rangeCheckL4Port_get
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
dal_maple_acl_rangeCheckL4Port_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
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
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (value)
    {
        case 2:
            pData->l4port_dir = RNGCHK_L4PORT_DIRECTION_SRC;
            break;
        case 3:
            pData->l4port_dir = RNGCHK_L4PORT_DIRECTION_DST;
            break;
        case 4:
            pData->l4port_dir = RNGCHK_L4PORT_DIRECTION_SRC_DST;
            break;
        default:
            ACL_SEM_UNLOCK(unit);
            return RT_ERR_RANGE_CHECK_TYPE;
    }

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckL4Port_get */

/* Function Name:
 *      dal_maple_acl_rangeCheckL4Port_set
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
dal_maple_acl_rangeCheckL4Port_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, l4port_dir=%d",\
        unit, index, pData->upper_bound, pData->lower_bound, pData->l4port_dir);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->l4port_dir >= RNGCHK_L4PORT_DIRECTION_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->upper_bound > 65535), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > 65535), RT_ERR_INPUT);

    switch (pData->l4port_dir)
    {
        case RNGCHK_L4PORT_DIRECTION_SRC:
            value = 2;
            break;
        case RNGCHK_L4PORT_DIRECTION_DST:
            value = 3;
            break;
        case RNGCHK_L4PORT_DIRECTION_SRC_DST:
            value = 4;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "invalid L4 port type");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckL4Port_set */

/* Function Name:
 *      dal_maple_acl_rangeCheckVid_get
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
dal_maple_acl_rangeCheckVid_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
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
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (value)
    {
        case 0:
            pData->vid_type = RNGCHK_VID_TYPE_INNER;
            break;
        case 1:
            pData->vid_type = RNGCHK_VID_TYPE_OUTER;
            break;
        default:
            ACL_SEM_UNLOCK(unit);
            return RT_ERR_RANGE_CHECK_TYPE;
    }

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckVid_get */

/* Function Name:
 *      dal_maple_acl_rangeCheckVid_set
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
dal_maple_acl_rangeCheckVid_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, vid_type=%d",\
        unit, index, pData->vid_upper_bound, pData->vid_lower_bound, pData->vid_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
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
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckVid_set */

/* Function Name:
 *      dal_maple_acl_rangeCheckIp_get
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
 *      1) For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *         index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *         be used together in order to filter a full IPv6 address.
 *      2) For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *         Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 */
int32
dal_maple_acl_rangeCheckIp_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_ip_t *pData)
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
                        MAPLE_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_IP_UPPERf,
                        &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_IP_LOWERf,
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
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckIp_get */

/* Function Name:
 *      dal_maple_acl_rangeCheckIp_set
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
 *      1) For IPv6 range check, index 0/4 represents IP6[31:0], index 1/5 represents IP6[63:32],
 *         index 2/6 represents IP6[95:64], index 3/7 represents IP6[127:96]. Index 0~3/4~7 must
 *         be used together in order to filter a full IPv6 address.
 *      2) For IPv6 suffix range check, index 0/2/4/6 represents IP6[31:0], index 1/3/5/7 represents IP6[63:32].
 *         Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 */
int32
dal_maple_acl_rangeCheckIp_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_ip_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, ip_type=%d",\
        unit, index, pData->ip_upper_bound, pData->ip_lower_bound, pData->ip_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
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
                        MAPLE_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_IP_UPPERf,
                        &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_IP_LOWERf,
                        &pData->ip_lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckIp_set */

/* Function Name:
 *      dal_maple_acl_rangeCheckSrcPort_get
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
dal_maple_acl_rangeCheckSrcPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT(unit)), RT_ERR_ENTRY_INDEX);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_SPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_SPM_0f,
                        &pData->port_mask.bits[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckSrcPort_get */

/* Function Name:
 *      dal_maple_acl_rangeCheckSrcPort_set
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
dal_maple_acl_rangeCheckSrcPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, port_mask0=%x",\
        unit, index, pData->port_mask.bits[0]);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RT_ERR_OK != hwp_portmaskValid_Chk(unit, &(pData->port_mask))), RT_ERR_PORT_MASK);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_SPM_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_SPM_0f,
                        &pData->port_mask.bits[0])) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckSrcPort_set */

/* Function Name:
 *      dal_maple_acl_rangeCheckPacketLen_get
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
dal_maple_acl_rangeCheckPacketLen_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /*type is not packet length*/
    if(0x5 != value)
    {
        ACL_SEM_UNLOCK(unit);
        return RT_ERR_RANGE_CHECK_TYPE;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckPacketLen_get */

/* Function Name:
 *      dal_maple_acl_rangeCheckPacketLen_set
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
dal_maple_acl_rangeCheckPacketLen_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    int32   ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d",\
        unit, index, pData->upper_bound, pData->lower_bound);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->lower_bound > 0xFFFF), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->upper_bound > 0xFFFF), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    value = 5;
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckPacketLen_set */

/* Function Name:
 *      dal_maple_acl_rangeCheckFieldSelector_get
 * Description:
 *      Get the configuration of field selector range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of field selector
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
dal_maple_acl_rangeCheckFieldSelector_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (value)
    {
        case 6:
            pData->fieldSelector_type = RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR0;
            break;
        case 7:
            pData->fieldSelector_type = RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR1;
            break;
        default:
            ACL_SEM_UNLOCK(unit);
            return RT_ERR_RANGE_CHECK_TYPE;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckFieldSelector_get */

/* Function Name:
 *      dal_maple_acl_rangeCheckFieldSelector_set
 * Description:
 *      Set the configuration of field selector  range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of field selector
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
dal_maple_acl_rangeCheckFieldSelector_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, vid_type=%d",\
        unit, index, pData->upper_bound, pData->lower_bound, pData->fieldSelector_type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->fieldSelector_type >= RNGCHK_FIELDSELECTOR_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > 0xFFFF), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->upper_bound > 0xFFFF), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);

    switch (pData->fieldSelector_type)
    {
        case RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR0:
            value = 6;
            break;
        case RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR1:
            value = 7;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "invalid field selector type");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_UPPERf,
                        &pData->upper_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        MAPLE_RNG_CHK_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        MAPLE_LOWERf,
                        &pData->lower_bound)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_rangeCheckFieldSelector_set */

/* Function Name:
 *      dal_maple_acl_fieldSelector_get
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
dal_maple_acl_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_acl_fieldSelector_data_t *pFs)
{
    int32   ret;
    uint32  value = 0;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, fs_idx=%d, pFs.start=%d, pFs.offset=%d", \
        unit, fs_idx, pFs->start, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);

    for (idx = 0; idx <= 28; idx++)
    {
        if(!HWP_PORT_EXIST(unit, idx))
            continue;

        /* get value from CHIP */
        if ((ret = reg_array_field_read(unit,
                            MAPLE_PARSER_FIELD_SELTOR_CTRLr,
                            idx,
                            fs_idx,
                            MAPLE_FMTf,
                            &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_array_field_read(unit,
                            MAPLE_PARSER_FIELD_SELTOR_CTRLr,
                            idx,
                            fs_idx,
                            MAPLE_OFFSETf,
                            &pFs->offset)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        break;
    }
    ACL_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            pFs->start = FS_START_POS_RAW;
            break;
        case 1:
            pFs->start = FS_START_POS_L2;
            break;
        case 2:
            pFs->start = FS_START_POS_L3;
            break;
        case 3:
            pFs->start = FS_START_POS_L4;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_maple_acl_fieldSelector_get */

/* Function Name:
 *      dal_maple_acl_fieldSelector_set
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
dal_maple_acl_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_acl_fieldSelector_data_t *pFs)
{
    int32   ret;
    uint32  value;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, fs_idx=%d, pFs.start=%d, pFs.offset=%d", \
        unit, fs_idx, pFs->start, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pFs->start >= FS_START_POS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pFs->offset >= 256), RT_ERR_INPUT);

    switch (pFs->start)
    {
        case FS_START_POS_RAW:
            value = 0;
            break;
        case FS_START_POS_L2:
            value = 1;
            break;
        case FS_START_POS_L3:
            value = 2;
            break;
        case FS_START_POS_L4:
            value = 3;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "invalid start position");
            return RT_ERR_INPUT;
    }

    ACL_SEM_LOCK(unit);
    for (idx = 0; idx <= 28; idx++)
    {
        if(!HWP_PORT_EXIST(unit, idx))
            continue;

        /* set value to CHIP */
        if ((ret = reg_array_field_write(unit,
                            MAPLE_PARSER_FIELD_SELTOR_CTRLr,
                            idx,
                            fs_idx,
                            MAPLE_FMTf,
                            &value)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                            MAPLE_PARSER_FIELD_SELTOR_CTRLr,
                            idx,
                            fs_idx,
                            MAPLE_OFFSETf,
                            &pFs->offset)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_fieldSelector_set */

/* Function Name:
 *      dal_maple_acl_meterMode_get
 * Description:
 *      Get the meter mode of a specific meter block.
 * Input:
 *      unit       - unit id
 *      blockIdx   - meter block ID
 * Output:
 *      pMeterMode - meter mode:byte based or packet based
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_meterMode_get(
    uint32              unit,
    uint32              entryIdx,
    rtk_acl_meterMode_t *pMeterMode)
{
    int32   ret;
    uint32  meterMode;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entryIdx=%d", unit, entryIdx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((entryIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMeterMode), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[entryIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          entryIdx,
                          MAPLE_TYPEf,
                          &meterMode)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (meterMode == 0)
        *pMeterMode = METER_MODE_BYTE;
    else
        *pMeterMode = METER_MODE_PACKET;

    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "*pMeterMode=%d", *pMeterMode);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meterMode_get */

/* Function Name:
 *      dal_maple_acl_meterMode_set
 * Description:
 *      Set the meter mode.
 * Input:
 *      unit      - unit id
 *      blockIdx  - meter block ID
 *      meterMode - meter mode (byte based or packet based)
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
dal_maple_acl_meterMode_set(
    uint32              unit,
    uint32              entryIdx,
    rtk_acl_meterMode_t meterMode)
{
    int32   ret;
    uint32  meterModeVal;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entryIdx=%d meterMode=%d", unit, entryIdx, meterMode);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((entryIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((meterMode >= METER_MODE_END), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    /* program value to CHIP*/
    if (meterMode == METER_MODE_BYTE)
        meterModeVal = 0;
    else
        meterModeVal = 1;
    if ((ret = reg_array_field_write(unit,
                          meterEntryReg[entryIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          entryIdx,
                          MAPLE_TYPEf,
                          &meterModeVal)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meterMode_set */

/* Function Name:
 *      dal_maple_acl_meterIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for meter.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_METER_GLB_CTRLr, MAPLE_INCL_PREIFGf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

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
} /* end of dal_maple_acl_meterIncludeIfg_get */

/* Function Name:
 *      dal_maple_acl_meterIncludeIfg_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_acl_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32   ret;
    uint32  value;

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
    if ((ret = reg_field_write(unit, MAPLE_METER_GLB_CTRLr, MAPLE_INCL_PREIFGf, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_meterIncludeIfg_set */

/* Function Name:
 *      dal_maple_acl_meterBurstSize_get
 * Description:
 *      Get the meter burst sizes of a specific meter mode.
 * Input:
 *      unit       - unit id
 *      meterMode  - meter mode
 * Output:
 *      pBurstSize - pointer to burst sizes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_meterBurstSize_get(
    uint32                      unit,
    rtk_acl_meterMode_t         meterMode,
    rtk_acl_meterBurstSize_t    *pBurstSize)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterMode=%d", unit, meterMode);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterMode >= METER_MODE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBurstSize), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if (METER_MODE_BYTE == meterMode)
    {
        if ((ret = reg_field_read(unit,
                              MAPLE_METER_BYTE_LB_THR_CTRLr,
                              MAPLE_BYTE_LB_THR0f,
                              &pBurstSize->slb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_read(unit,
                              MAPLE_METER_BYTE_LB_THR_CTRLr,
                              MAPLE_BYTE_LB_THR1f,
                              &pBurstSize->slb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_read(unit,
                              MAPLE_METER_PKT_LB_THR_CTRLr,
                              MAPLE_PKT_LB_THR0f,
                              &pBurstSize->slb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_read(unit,
                              MAPLE_METER_PKT_LB_THR_CTRLr,
                              MAPLE_PKT_LB_THR1f,
                              &pBurstSize->slb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meterBurstSize_get */

/* Function Name:
 *      dal_maple_acl_meterBurstSize_set
 * Description:
 *      Set the meter burst sizes of a specific meter mode.
 * Input:
 *      unit       - unit id
 *      meterMode  - meter mode (byte based or packet based)
 *      pBurstSize - pointer to burst sizes
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_meterBurstSize_set(
    uint32                      unit,
    rtk_acl_meterMode_t         meterMode,
    rtk_acl_meterBurstSize_t    *pBurstSize)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterMode >= METER_MODE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->slb_lb0bs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pBurstSize->slb_lb1bs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBurstSize), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if (METER_MODE_BYTE == meterMode)
    {
        if ((ret = reg_field_write(unit,
                              MAPLE_METER_BYTE_LB_THR_CTRLr,
                              MAPLE_BYTE_LB_THR0f,
                              &pBurstSize->slb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_write(unit,
                              MAPLE_METER_BYTE_LB_THR_CTRLr,
                              MAPLE_BYTE_LB_THR1f,
                              &pBurstSize->slb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_write(unit,
                              MAPLE_METER_PKT_LB_THR_CTRLr,
                              MAPLE_PKT_LB_THR0f,
                              &pBurstSize->slb_lb0bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = reg_field_write(unit,
                              MAPLE_METER_PKT_LB_THR_CTRLr,
                              MAPLE_PKT_LB_THR1f,
                              &pBurstSize->slb_lb1bs)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meterBurstSize_set */

/* Function Name:
 *      dal_maple_acl_meterExceed_get
 * Description:
 *      Get the meter exceed flag of a meter entry.
 * Input:
 *      unit      - unit id
 *      meterIdx  - meter entry index
 * Output:
 *      pIsExceed - pointer to exceed flag
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
dal_maple_acl_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed)
{
    int32 ret;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_EXCEED_FLAGf,
                          pIsExceed)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (TRUE == *pIsExceed)
    {
        /* reset the flag */
        if ((ret = reg_array_field_write(unit,
                              meterEntryReg[meterIdx/32],
                              REG_ARRAY_INDEX_NONE,
                              meterIdx,
                              MAPLE_EXCEED_FLAGf,
                              pIsExceed)) != RT_ERR_OK)
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "*pIsExceed=%d", *pIsExceed);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meterExceed_get */

/* Function Name:
 *      dal_maple_acl_meterEntry_get
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
dal_maple_acl_meterEntry_get(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_acl_meterEntry_t   *pMeterEntry)
{
    int32           ret;
    meter_entry_t   meter_entry;
    uint32          val;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_ENABLEf,
                          &pMeterEntry->enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_THRGRPf,
                          &pMeterEntry->thr_grp)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_RATEf,
                          &pMeterEntry->rate)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_DROPf,
                          &val)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if(0x1 == val)
    {
        pMeterEntry->rate = 0x0;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_meterEntry_get */

/* Function Name:
 *      dal_maple_acl_meterEntry_set
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
dal_maple_acl_meterEntry_set(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_acl_meterEntry_t    *pMeterEntry)
{

    int32           ret;
    meter_entry_t   meter_entry;
    uint32          val;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_ENABLEf,
                          &pMeterEntry->enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_THRGRPf,
                          &pMeterEntry->thr_grp)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_RATEf,
                          &pMeterEntry->rate)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if(0x0 == pMeterEntry->rate)
    {
        val = 0x1;
    }
    else
    {
        val = 0x0;
    }
    if ((ret = reg_array_field_write(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_DROPf,
                          &val)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meterEntry_set */

/*
 * Transfer physical template field ID to logical template field ID
 */
static int32
_dal_maple_acl_phy2logTmplteField(uint32 unit, uint32 phy_field_id, rtk_acl_templateFieldType_t *log_field_id)
{
    int32   ret = RT_ERR_OK;
    uint32  i;

    /* Check field type */
    for (i = 0; dal_maple_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (phy_field_id == dal_maple_template_field_list[i].physical_id)
            break;
    }

    if (dal_maple_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_ACL_FIELD_TYPE;
        return ret;
    }

    /* Get the logical field ID */
    *log_field_id = dal_maple_template_field_list[i].field_type;

    return ret;
} /* end of _dal_maple_acl_phy2logTmplteField */

/*
 * Validate fields and transfer logical field ID to physical field ID
 */
static int32
_dal_maple_acl_log2PhyTmplteField(uint32 unit, uint32 field_idx, rtk_acl_templateFieldType_t log_field_id, uint32 *phy_field_id)
{
    int32   ret = RT_ERR_OK;
    uint32  i;

    /* Check arguments */
    RT_PARAM_CHK((field_idx >= RTK_MAX_NUM_OF_ACL_TEMPLATE_FIELD), RT_ERR_ENTRY_INDEX);

    /* Check field type */
    for (i = 0; dal_maple_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (log_field_id == dal_maple_template_field_list[i].field_type)
            break;
    }

    if (dal_maple_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_ACL_FIELD_TYPE;
        return ret;
    }

    /* Check field location */
    if ((dal_maple_template_field_list[i].valid_location != 0) &&
        ((dal_maple_template_field_list[i].valid_location & (1<<field_idx)) == 0))
        ret = RT_ERR_PIE_FIELD_LOCATION;

    /* Get the physical field ID */
    *phy_field_id = dal_maple_template_field_list[i].physical_id;

    return ret;
} /* end of _dal_maple_acl_log2PhyTmplteField */

static int32
_dal_maple_acl_rule_del(uint32 unit, rtk_acl_clear_t *pClrIdx)
{
    int32   ret;
    uint32  value = 0, field_data;
    uint32  start_blk;
    uint32  end_blk;
    uint32  block_idx;
    uint8   *org_block_lookup_state=NULL;
    rtk_enable_t lookup_state;

    /* get start and end blocks */
    start_blk = pClrIdx->start_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    end_blk = pClrIdx->end_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    if ((ret = reg_field_set(unit, MAPLE_ACL_CLR_CTRLr, MAPLE_CLR_FROMf, &pClrIdx->start_idx, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MAPLE_ACL_CLR_CTRLr, MAPLE_CLR_TOf, &pClrIdx->end_idx, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /* disable lookup of corresponding blocks to avoid incautious hit */
    org_block_lookup_state = osal_alloc(HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit));
    if (NULL==org_block_lookup_state)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(org_block_lookup_state, 0x0, HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit));
    for (block_idx = start_blk; block_idx <= end_blk; block_idx++)
    {
        dal_maple_acl_blockLookupEnable_get(unit, block_idx, &lookup_state);

        if (ENABLED == lookup_state)
        {
            org_block_lookup_state[block_idx] = ENABLED;
            dal_maple_acl_blockLookupEnable_set(unit, block_idx, DISABLED);
        }
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, MAPLE_ACL_CLR_CTRLr, MAPLE_CLRf, &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        osal_free(org_block_lookup_state);
        return ret;
    }

    ACL_SEM_LOCK2(unit, ret);

    if ((ret = reg_write(unit, MAPLE_ACL_CLR_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        osal_free(org_block_lookup_state);
        return ret;
    }

    /* wait until clear action is completed */
    do {
        reg_field_read(unit, MAPLE_ACL_CLR_CTRLr, MAPLE_CLRf, &value);
        if (value == 0)
            break;
    } while(1);

    ACL_SEM_UNLOCK(unit);

    /* restore the lookup state for the blocks */
    for (block_idx = start_blk; block_idx <= end_blk; block_idx++)
    {
        if (ENABLED == org_block_lookup_state[block_idx])
            dal_maple_acl_blockLookupEnable_set(unit, block_idx, ENABLED);
    }

    osal_free(org_block_lookup_state);
    return RT_ERR_OK;

  fail_ret:
    if (org_block_lookup_state != NULL)
    {
        osal_free(org_block_lookup_state);
    }
    return ret;
} /* end of _dal_maple_acl_rule_del */

static int32
_dal_maple_acl_rule_move(uint32 unit, rtk_acl_move_t *pData)
{
    int32   ret;
    uint32  value = 0, field_data;
    uint32  start_blk;
    uint32  end_blk;
    uint32  block_idx;
    uint8   *org_block_lookup_state=NULL;
    rtk_enable_t lookup_state;

    /* get start and end blocks */
    start_blk = pData->move_from/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    end_blk = pData->move_to/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    if ((ret = reg_field_set(unit, MAPLE_ACL_MV_CTRLr, MAPLE_MV_FROMf, &pData->move_from, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MAPLE_ACL_MV_CTRLr, MAPLE_MV_TOf, &pData->move_to, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_ACL_MV_LEN_CTRLr, MAPLE_MV_LENf, &pData->length)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* disable lookup of corresponding blocks to avoid incautious hit */
    org_block_lookup_state = osal_alloc(HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit));
    if (NULL==org_block_lookup_state)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(org_block_lookup_state, 0x0, HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit));
    for (block_idx = start_blk; block_idx <= end_blk; block_idx++)
    {
        dal_maple_acl_blockLookupEnable_get(unit, block_idx, &lookup_state);

        if (ENABLED == lookup_state)
        {
            org_block_lookup_state[block_idx] = ENABLED;
            dal_maple_acl_blockLookupEnable_set(unit, block_idx, DISABLED);
        }
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, MAPLE_ACL_MV_CTRLr, MAPLE_MVf, &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        osal_free(org_block_lookup_state);
        return ret;
    }

    ACL_SEM_LOCK2(unit, ret);

    if ((ret = reg_write(unit, MAPLE_ACL_MV_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        osal_free(org_block_lookup_state);
        return ret;
    }

    /* wait until move action is completed */
    do {
        reg_field_read(unit, MAPLE_ACL_MV_CTRLr, MAPLE_MVf, &value);
        if (value == 0)
            break;
    } while(1);

    ACL_SEM_UNLOCK(unit);

    /* restore the lookup state for the blocks */
    for (block_idx = start_blk; block_idx <= end_blk; block_idx++)
    {
        if (ENABLED == org_block_lookup_state[block_idx])
            dal_maple_acl_blockLookupEnable_set(unit, block_idx, ENABLED);
    }

    osal_free(org_block_lookup_state);
    return RT_ERR_OK;

  fail_ret:
    if (org_block_lookup_state != NULL)
    {
        osal_free(org_block_lookup_state);
    }
    return ret;
} /* end of _dal_maple_acl_rule_move */

static int32
_dal_maple_acl_igrRuleValidate_get(
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
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_VALIDtf, pValid, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleValidate_get */

static int32
_dal_maple_acl_igrRuleValidate_set(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    int32   ret;
    acl_igrRule_entry_t igr_entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((valid != 0) && (valid != 1), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_VALIDtf, &valid, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_write(unit, MAPLE_IACLt, entry_idx, (uint32 *) &igr_entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleValidate_set */

static int32
_dal_maple_acl_igrRuleEntry_read(
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
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_maple_acl_ruleEntrySize_get(unit, ACL_PHASE_IGR_ACL, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(pEntry_buffer, &entry, 24);
    osal_memcpy(pEntry_buffer + 24, (uint8 *)&entry + 25 , 27);
    osal_memcpy(pEntry_buffer + 51, (uint8 *)&entry + 53 , (size - 24 - 27));

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleEntry_read */

static int32
_dal_maple_acl_igrRuleEntry_write(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    acl_igrRule_entry_t entry;
    uint8               *ptr;
    int32               ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pEntry_buffer=%x",\
        unit, entry_idx, pEntry_buffer);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    ptr = (uint8 *)&entry;
    osal_memcpy(ptr, pEntry_buffer, 24);
    osal_memcpy(&ptr[25], &pEntry_buffer[24], 27);
    osal_memcpy(&ptr[53], &pEntry_buffer[51], 3);

    ACL_SEM_LOCK(unit);

    if ((ret = table_write(unit, MAPLE_IACLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleEntry_write */

static void
_dal_maple_acl_field2Buf_get(uint32 unit, uint32 fieldType,
    rtk_acl_fieldType_t type, uint32 fieldNumber,
    uint32 data, uint32 mask,
    uint8 *pData, uint8 *pMask)
{
    uint32  field_offset, data_offset, field_length;
    uint32  *pTmp_data;
    uint32  *pTmp_mask;
    uint32  field_size, tmp_offset;
    uint32  buf_field_offset, buf_byte_idx_num, mask_len, i;
    uint32  chip_field_offset;
    uint32  buf_field_bit_start, buf_field_bit_end, buf_field_bit_max;
    uint32  buf_field_byte_idx_start, buf_field_byte_idx_end;
    uint32  tmp_data_offset;
    uint32  data_field;

    field_offset = dal_maple_acl_field_list[fieldType].pField[fieldNumber].field_offset;
    field_length = dal_maple_acl_field_list[fieldType].pField[fieldNumber].field_length;
    data_offset = dal_maple_acl_field_list[fieldType].pField[fieldNumber].data_offset;

    dal_maple_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_MAPLE_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_MAPLE_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_MAPLE_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_MAPLE_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_MAPLE_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_MAPLE_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_MAPLE_MAX_INFO_IDX);
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
}   /* end of _dal_maple_acl_field2Buf_get */


static void
_dal_maple_acl_buf2Field_get(uint32 unit, uint32 fieldType,
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
#if defined(CONFIG_SDK_ENDIAN_LITTLE)
    uint32 ednData=0;
    uint32 ednMask=0;
#endif

    *data16 = *mask16 = 0;

    field_offset = dal_maple_acl_field_list[fieldType].pField[fieldNumber].field_offset;
    field_length = dal_maple_acl_field_list[fieldType].pField[fieldNumber].field_length;
    data_offset = dal_maple_acl_field_list[fieldType].pField[fieldNumber].data_offset;

    dal_maple_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_MAPLE_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_MAPLE_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_MAPLE_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_MAPLE_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_MAPLE_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_MAPLE_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_MAPLE_MAX_INFO_IDX);
        pTmp_data = (uint32 *)pData + tmp_offset;
        pTmp_mask = (uint32 *)pMask + tmp_offset;
    #if defined(CONFIG_SDK_ENDIAN_LITTLE)
        ednData = *pTmp_data;
        ednData = (((ednData & 0xff) << 24) | ((ednData & 0xff00) << 8) | ((ednData & 0xff0000) >> 8) | ((ednData & 0xff000000) >> 24));
        pTmp_data = &ednData;
        ednMask = *pTmp_mask;
        ednMask = (((ednMask & 0xff) << 24) | ((ednMask & 0xff00) << 8) | ((ednMask & 0xff0000) >> 8) | ((ednMask & 0xff000000) >> 24));
        pTmp_mask = &ednMask;
    #endif

        *data16 |= ((*pTmp_data >> buf_field_offset) & ((1 << mask_len)-1)) << chip_field_offset;
        *mask16 |= ((*pTmp_mask >> buf_field_offset) & ((1 << mask_len)-1)) << chip_field_offset;

        chip_field_offset += mask_len;

        buf_field_bit_start = buf_field_bit_end + 1;

    }
}   /* end of _dal_maple_acl_buf2Field_get */


static int32
_dal_maple_acl_igrRuleEntryField_read(
    uint32              unit,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    acl_igrRule_entry_t entry;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, type=%d", unit, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_maple_acl_igrRuleEntryBufField_read(unit, entry_idx,
            type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleEntryField_read */

static int32
_dal_maple_acl_igrRuleEntryField_write(
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
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_FIELD_TYPE_CHK(unit, type);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_maple_acl_igrRuleEntryBufField_write(unit, entry_idx,
            type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

   ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleEntryField_write */

static int32
_dal_maple_acl_igrRuleOperation_get(
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
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_NOTtf,\
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AND1tf,\
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AND2tf,\
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleOperation_get */

static int32
_dal_maple_acl_igrRuleOperation_set(
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
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pOperation->aggr_1 == ENABLED) && (entry_idx%2 != 0)), RT_ERR_ENTRY_INDEX);
    if (pOperation->aggr_2 == ENABLED &&
        (entry_idx%2 != 0 || (entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 != 0))
    {
        return RT_ERR_ENTRY_INDEX;
    }
    if (pOperation->aggr_2 == ENABLED &&
        (entry_idx >= 6*HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)))
    {
        return RT_ERR_ACL_AGGR_2;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_NOTtf,\
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AND1tf,\
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AND2tf,\
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRuleOperation_set */

static int32
_dal_maple_acl_igrRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction)
{
    int32    ret;
    uint32   value;
    uint32   action_info_field;
    uint32   act;
    uint32   info;
    acl_igrRule_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, action_idx=%d", unit, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    info = 0;
    osal_memset(pAction, 0x0, sizeof(rtk_acl_igrAction_t));

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    action_info_field = 0;

    /* drop action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_DROPtf,\
        &pAction->drop_data, (uint32 *) &entry), errHandle, ret);

    /* Forwarding action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_FWD_SELtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->fwd_en)
    {
        RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
            &value, (uint32 *) &entry), errHandle, ret);
        act = (value >> 13) & 0x7;
        switch (act)
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
            case 7:
                pAction->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_VLAN_LEAKY;
        }

        info = value & 0x1fff;
        switch (act)
        {
            case 2:
            case 4:
                pAction->fwd_data.info.copy_redirect_port.fwd_port_id = info & 0x1f;
                pAction->fwd_data.info.copy_redirect_port.cpu_tag = (info >> 9) & 0x1;
                pAction->fwd_data.info.copy_redirect_port.skip_igrStpDrop = (info >> 10) & 0x1;
                pAction->fwd_data.info.copy_redirect_port.skip_storm_igrVlan = (info >> 11) & 0x1;
                pAction->fwd_data.info.copy_redirect_port.force = (info >> 12) & 0x1;
                break;
            case 3:
            case 5:
                pAction->fwd_data.info.copy_redirect_portMsk.fwd_idx = info & 0x1ff;
                pAction->fwd_data.info.copy_redirect_portMsk.cpu_tag = (info >> 9) & 0x1;
                pAction->fwd_data.info.copy_redirect_portMsk.skip_igrStpDrop = (info >> 10) & 0x1;
                pAction->fwd_data.info.copy_redirect_portMsk.skip_storm_igrVlan = (info >> 11) & 0x1;
                pAction->fwd_data.info.copy_redirect_portMsk.force = (info >> 12) & 0x1;
                break;
            case 6:
                pAction->fwd_data.info.route.idx = info;
                break;
        }

        action_info_field++;
    }

    /*OVID action*/
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_OVID_SELtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->outer_vlan_assign_en)
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = (value >> 12) & 0x3;
        switch (act)
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
            case 3:
                pAction->outer_vlan_data.vid_assign_type = ACL_IGR_ACTION_OVLAN_ASSIGN_PORT_BASED_OUTER_VID;
                break;
        }
        info = value & 0xfff;
        pAction->outer_vlan_data.vid_value = info;
        pAction->outer_vlan_data.vid_shift_sel = 0;
        action_info_field++;
    }

    /*IVID action*/
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_IVID_SELtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->inner_vlan_assign_en)
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = (value >> 12) & 0x3;
        switch (act)
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
            case 3:
                pAction->inner_vlan_data.vid_assign_type = ACL_IGR_ACTION_IVLAN_ASSIGN_PORT_BASED_INNER_VID;
                break;
        }
        info = value & 0xfff;
        pAction->inner_vlan_data.vid_value = info;
        pAction->inner_vlan_data.vid_shift_sel = 0;
        action_info_field++;
    }

    /*Filter action*/
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_FLT_SELtf,\
        &pAction->filter_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->filter_en)
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = (value >> 9) & 0x1;
        switch (act)
        {
            case 0:
                pAction->filter_data.flt_act = ACL_IGR_ACTION_FLT_SINGLE_PORT;
                break;
            case 1:
                pAction->filter_data.flt_act = ACL_IGR_ACTION_FLT_MULTIPLE_PORTS;
                break;
        }
        if (0 == act)
            info = value & 0x1f;
        if (1 == act)
            info = value & 0x1ff;
        pAction->filter_data.flt_info = info;
        action_info_field++;
    }

    /*Log action*/
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_LOG_SELtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->stat_en)
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = (value >> 8) & 0x1;
        switch (act)
        {
            case 0:
                pAction->stat_data.stat_type= STAT_TYPE_PACKET_BASED_32BIT;
                break;
            case 1:
                pAction->stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
                break;
        }
        if (0 == act)
        {
            info = (((value >> 1) & 0x7f) * 2) + (value & 0x1);
        }
        if (1 == act)
            info = (value >> 1) & 0x7f;
        pAction->stat_data.stat_idx = info;
        action_info_field++;
    }

    /*Remark action*/
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_RMK_SELtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->rmk_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = (value >> 6) & 0x7;
        switch (act)
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
            case 6:
               pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_KEEP_INNER_USER_PRI;
               break;
            case 7:
               pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_KEEP_OUTER_USER_PRI;
               break;
        }
        if ((0 == act) || (1 == act) || (3 == act))
        {
            info = value & 0x7;
        }
        if (2 == act)
            info = value & 0x3f;
        pAction->rmk_data.rmk_info = info;
        action_info_field++;
    }

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_METER_SELtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->meter_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        pAction->meter_data.meter_idx = value & 0xff;
        action_info_field++;
    }

    /* Egress Tag Status action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_TAGST_SELtf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->tag_sts_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        switch (value & 0x3)
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
        }
        switch ((value >>2) & 0x3)
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
        }
        action_info_field++;
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_MIR_SELtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->mirror_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = (value >> 2) & 0x1;
        if (0 == act)
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_ORIGINAL;
        if (1 == act)
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_MODIFIED;
        pAction->mirror_data.mirror_set_idx = value & 0x3;
        action_info_field++;
    }

    /* Normal Priority action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_NORPRI_SELtf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->pri_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        pAction->pri_data.pri = value & 0x7;
        action_info_field++;
    }

    /*CPU Priority action*/
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_CPUPRI_SELtf,\
        &pAction->cpu_pri_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->cpu_pri_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        pAction->cpu_pri_data.pri = value & 0x7;
        action_info_field++;
    }

    /* Otpid action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_OTPID_SELtf,\
        &pAction->otpid_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->otpid_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        pAction->otpid_data.tpid_idx = value & 0x3;
        action_info_field++;
    }

    /* Itpid action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_ITPID_SELtf,\
        &pAction->itpid_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->itpid_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        pAction->itpid_data.tpid_idx = value & 0x3;
        action_info_field++;
    }

    /*Shaper action */
    RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_SHAPER_SELtf,\
        &pAction->shaper_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->shaper_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_get(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        act = value & 0x3;
        switch (act)
        {
            case 0:
                pAction->shaper_data.shaper_act = ACL_IGR_ACTION_SHAPER_Q0;
                break;
            case 1:
                pAction->shaper_data.shaper_act = ACL_IGR_ACTION_SHAPER_Q1;
                break;
            case 2:
                pAction->shaper_data.shaper_act = ACL_IGR_ACTION_SHAPER_Q2;
                break;
            case 3:
                pAction->shaper_data.shaper_act = ACL_IGR_ACTION_SHAPER_Q3;
                break;
        }
        action_info_field++;
    }

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
} /* end of _dal_maple_acl_igrRuleAction_get */

static int32
_dal_maple_acl_igrRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_igrAction_t     *pAction)
{
    int32    ret;
    uint32   value;
    uint32   type;
    uint32   action_info_field;
    acl_igrRule_entry_t entry;
    rtk_vlan_t          vid_value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, entry_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, entry_idx);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    type = 0xf;

    /* input value range check */
    if ((pAction->fwd_data.fwd_type >= ACL_IGR_ACTION_FWD_END) ||
        (((pAction->fwd_data.fwd_type == ACL_IGR_ACTION_FWD_COPY_TO_PORTMASK) ||
          (pAction->fwd_data.fwd_type == ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTMASK)) &&
         (pAction->fwd_data.info.copy_redirect_portMsk.fwd_idx >= HAL_MAX_NUM_OF_MCAST_ENTRY(unit))) ||
         (pAction->filter_data.flt_act >= ACL_IGR_ACTION_FLT_END) ||
         ((pAction->filter_data.flt_act == ACL_IGR_ACTION_FLT_MULTIPLE_PORTS) &&
         (pAction->filter_data.flt_info >= HAL_MAX_NUM_OF_MCAST_ENTRY(unit))) ||
        (pAction->stat_data.stat_type >= STAT_TYPE_END) ||
        ((pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT) &&
         (pAction->stat_data.stat_idx >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)) ||
        ((pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT) &&
         (pAction->stat_data.stat_idx >= HAL_MAX_NUM_OF_PIE_COUNTER(unit))) ||
        (pAction->rmk_data.rmk_act >= ACL_ACTION_REMARK_END) ||
        ((pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit)) &&
        (pAction->mirror_data.mirror_type >= ACL_ACTION_MIRROR_END)) ||
        (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit)) ||
        (pAction->inner_vlan_data.vid_assign_type >= ACL_IGR_ACTION_IVLAN_ASSIGN_END) ||
        (pAction->inner_vlan_data.vid_shift_sel >= 2) ||
        (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX) ||
        (pAction->outer_vlan_data.vid_assign_type >= ACL_IGR_ACTION_OVLAN_ASSIGN_END) ||
        (pAction->outer_vlan_data.vid_shift_sel >= 2) ||
        (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX) ||
        (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit)) ||
        (pAction->cpu_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit)) ||
        (pAction->shaper_data.shaper_act >= ACL_IGR_ACTION_SHAPER_END) ||
        (pAction->tag_sts_data.itag_sts >= ACL_ACTION_TAG_STS_END) ||
        (pAction->tag_sts_data.otag_sts >= ACL_ACTION_TAG_STS_END))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "parameter error");
        return RT_ERR_INPUT;
    }

    action_info_field = 0;
    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* drop action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_DROPtf,\
        &pAction->drop_data, (uint32 *) &entry), errHandle, ret);

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_FWD_SELtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->fwd_en)
    {
        value = 0x0;
        switch (pAction->fwd_data.fwd_type)
        {
            case ACL_IGR_ACTION_FWD_PERMIT:
                type = 0;
                break;
            case ACL_IGR_ACTION_FWD_DROP:
                type = 1;
                break;
            case ACL_IGR_ACTION_FWD_COPY_TO_PORTID:
                type = 2;
                break;
            case ACL_IGR_ACTION_FWD_COPY_TO_PORTMASK:
                type = 3;
                break;
            case ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID:
                type = 4;
                break;
            case ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTMASK:
                type = 5;
                break;
            case ACL_IGR_ACTION_FWD_UNICAST_ROUTING:
                type = 6;
                break;
            case ACL_IGR_ACTION_FWD_VLAN_LEAKY:
                type = 7;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
                return RT_ERR_INPUT;
        }
        if ((2 == type) || (4 == type))
        {
            value = ((pAction->fwd_data.info.copy_redirect_port.fwd_port_id & 0x1f) << 0) |
                        ((pAction->fwd_data.info.copy_redirect_port.cpu_tag & 0x1) << 9) |
                        ((pAction->fwd_data.info.copy_redirect_port.skip_igrStpDrop & 0x1) << 10) |
                        ((pAction->fwd_data.info.copy_redirect_port.skip_storm_igrVlan & 0x1) << 11) |
                        ((pAction->fwd_data.info.copy_redirect_port.force & 0x1) << 12);
        }
        if ((3 == type) || (5 == type))
        {
            value = ((pAction->fwd_data.info.copy_redirect_portMsk.fwd_idx & 0x1ff) << 0) |
                        ((pAction->fwd_data.info.copy_redirect_portMsk.cpu_tag & 0x1) << 9) |
                        ((pAction->fwd_data.info.copy_redirect_portMsk.skip_igrStpDrop & 0x1) << 10) |
                        ((pAction->fwd_data.info.copy_redirect_portMsk.skip_storm_igrVlan & 0x1) << 11) |
                        ((pAction->fwd_data.info.copy_redirect_portMsk.force & 0x1) << 12);
        }
        if (6 == type)
        {
            value = pAction->fwd_data.info.route.idx & 0x1fff;
        }
        value |= ((type  & 0x7) << 13);
        RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
            &value, (uint32 *) &entry), errHandle, ret);
        action_info_field++;
    }

    /* Ovid action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_OVID_SELtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->outer_vlan_assign_en)
    {
        value = 0x0;
        switch (pAction->outer_vlan_data.vid_assign_type)
        {
            case ACL_IGR_ACTION_OVLAN_ASSIGN_NEW_VID:
                type = 0;
                break;
            case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_VID:
                type = 1;
                break;
            case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                type = 2;
                break;
            case ACL_IGR_ACTION_OVLAN_ASSIGN_PORT_BASED_OUTER_VID:
                type = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        if (1 == pAction->inner_vlan_data.vid_shift_sel)
            vid_value = 4096 - pAction->outer_vlan_data.vid_value;
        else
            vid_value = pAction->outer_vlan_data.vid_value;

        value = ((type & 0x3) << 12) |
                    ((vid_value & 0xfff) << 0);
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Ivid action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_IVID_SELtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->inner_vlan_assign_en)
    {
        value = 0x0;
        switch (pAction->inner_vlan_data.vid_assign_type)
        {
            case ACL_IGR_ACTION_IVLAN_ASSIGN_NEW_VID:
                type = 0;
                break;
            case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_VID:
                type = 1;
                break;
            case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                type = 2;
                break;
            case ACL_IGR_ACTION_IVLAN_ASSIGN_PORT_BASED_INNER_VID:
                type = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        if (1 == pAction->inner_vlan_data.vid_shift_sel)
            vid_value = 4096 - pAction->inner_vlan_data.vid_value;
        else
            vid_value = pAction->inner_vlan_data.vid_value;

        value = ((type & 0x3) << 12) |
                    ((vid_value & 0xfff) << 0);
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Filter  action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_FLT_SELtf,\
        &pAction->filter_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->filter_en)
    {
        value = 0x0;
        switch (pAction->filter_data.flt_act)
        {
            case ACL_IGR_ACTION_FLT_SINGLE_PORT:
                type = 0;
                break;
            case ACL_IGR_ACTION_FLT_MULTIPLE_PORTS:
                type = 1;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "filter_data.flt_act error");
                return RT_ERR_INPUT;
        }
        if (0 == type)
            value = (pAction->filter_data.flt_info & 0x1f) << 0;
        if (1 == type)
            value = (pAction->filter_data.flt_info & 0x1ff) << 0;
        value |= ((pAction->filter_data.flt_act & 0x1) << 9);
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_LOG_SELtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    if (1 == pAction->stat_en)
    {
        value = 0x0;
        if (pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT)
        {
            type = 0;
        }
        else if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
        {
            type = 1;
        }
        if (0 == type)
        {
            value = (((pAction->stat_data.stat_idx >> 1) & 0x7f) << 1);
            value |= ((pAction->stat_data.stat_idx % 2) & 0x1);
        }
        else if (1 == type)
        {
            value = ((pAction->stat_data.stat_idx) << 1) | (0x1 << 8);
        }
        else
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "stat_data.stat_type error");
            return RT_ERR_INPUT;
        }
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_RMK_SELtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->rmk_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = 0x0;
        switch (pAction->rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_INNER_USER_PRI:
                type = 0;
                break;
            case ACL_ACTION_REMARK_OUTER_USER_PRI:
                type = 1;
                break;
            case ACL_ACTION_REMARK_DSCP:
                type = 2;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                type = 3;
                break;
            case ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI:
                type = 4;
                break;
            case ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI:
                type = 5;
                break;
            case ACL_ACTION_REMARK_KEEP_INNER_USER_PRI:
                type = 6;
                break;
            case ACL_ACTION_REMARK_KEEP_OUTER_USER_PRI:
                type = 7;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "rmk_data.rmk_act error");
                return RT_ERR_INPUT;
        }
        if ((0 == type) || (1 == type) || (3 == type))
        {
            value = pAction->rmk_data.rmk_info & 0x7;
        }
        if (2 == type)
        {
            value = pAction->rmk_data.rmk_info & 0x3f;
        }
        value |= (type << 6);
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_METER_SELtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->meter_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = pAction->meter_data.meter_idx & 0xff;
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Egress tag status action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_TAGST_SELtf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->tag_sts_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        switch (pAction->tag_sts_data.itag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                type = 0;
                break;
            case ACL_ACTION_TAG_STS_TAG:
                type = 1;
                break;
            case ACL_ACTION_TAG_STS_KEEP_CONTENT:
                type = 2;
                break;
            case ACL_ACTION_TAG_STS_NOP:
                type = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.itag_sts error");
                return RT_ERR_INPUT;
        }
        value = type;
        switch (pAction->tag_sts_data.otag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                type = 0;
                break;
            case ACL_ACTION_TAG_STS_TAG:
                type = 1;
                break;
            case ACL_ACTION_TAG_STS_KEEP_CONTENT:
                type = 2;
                break;
            case ACL_ACTION_TAG_STS_NOP:
                type = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.itag_sts error");
                return RT_ERR_INPUT;
        }
        value |= (type << 2);
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_MIR_SELtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->mirror_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = 0x0;
        switch (pAction->mirror_data.mirror_type)
        {
            case ACL_ACTION_MIRROR_ORIGINAL:
                type = 0;
                break;
            case ACL_ACTION_MIRROR_MODIFIED:
                type = 1;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "mirror_data.mirror_type error");
                return RT_ERR_INPUT;
        }
        value = (type << 2) | (pAction->mirror_data.mirror_set_idx & 0x3);
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Normal Priority action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_NORPRI_SELtf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->pri_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = pAction->pri_data.pri & 0x7;
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* CPU Priority action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_CPUPRI_SELtf,\
        &pAction->cpu_pri_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->cpu_pri_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = pAction->cpu_pri_data.pri & 0x7;
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* OTPID action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_OTPID_SELtf,
        &pAction->otpid_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->otpid_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = pAction->otpid_data.tpid_idx & 0x3;
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* ITPID action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_ITPID_SELtf,
        &pAction->itpid_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->itpid_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = pAction->itpid_data.tpid_idx & 0x3;
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* Shaper action */
    RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_SHAPER_SELtf,
        &pAction->shaper_en, (uint32 *) &entry), errHandle, ret);
    if ((1 == pAction->shaper_en) && (action_info_field < DAL_MAPLE_MAX_NUM_OF_ACTION_INFO_FIELD))
    {
        value = 0x0;
        switch (pAction->shaper_data.shaper_act)
        {
            case ACL_IGR_ACTION_SHAPER_Q0:
                type = 0;
                break;
            case ACL_IGR_ACTION_SHAPER_Q1:
                type = 1;
                break;
            case ACL_IGR_ACTION_SHAPER_Q2:
                type = 2;
                break;
            case ACL_IGR_ACTION_SHAPER_Q3:
                type = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "shaper_data.shaper_act error");
                return RT_ERR_INPUT;
        }
        value = type;
        switch (action_info_field)
        {
            case 0:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF0tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 1:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF1tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 2:
                 RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF2tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 3:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF3tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
            case 4:
                RT_ERR_HDL(table_field_set(unit, MAPLE_IACLt, MAPLE_IACL_AIF4tf,\
                    &value, (uint32 *) &entry), errHandle, ret);
                break;
        }
        action_info_field++;
    }

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, MAPLE_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
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
} /* end of _dal_maple_acl_igrRuleAction_set */

static int32
_dal_maple_acl_igrRule_del(uint32 unit, rtk_acl_clear_t *pClrIdx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, start_idx=%d, end_idx=%d",\
        unit, pClrIdx->start_idx, pClrIdx->end_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pClrIdx->start_idx);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pClrIdx->end_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, pClrIdx->start_idx);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, pClrIdx->end_idx);

    if ((ret = _dal_maple_acl_rule_del(unit, pClrIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRule_del */

static int32
_dal_maple_acl_igrRule_move(uint32 unit, rtk_acl_move_t *pData)
{
    int32   ret;
    uint32  entry_num;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, move_from=%d, move_to=%d, length=%d",\
        unit, pData->move_from, pData->move_to, pData->length);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pData->move_from);
    DAL_MAPLE_ACL_INDEX_CHK(unit, ACL_PHASE_IGR_ACL, pData->move_to);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, pData->move_from);
    DAL_MAPLE_ACL_BLOCK_PWR_CHK(unit, pData->move_to);
    DAL_MAPLE_ACL_ENTRYNUM(unit, ACL_PHASE_IGR_ACL, entry_num);
    if ((pData->move_from + pData->length) > entry_num ||
        (pData->move_to + pData->length) > entry_num)
        return RT_ERR_ENTRY_INDEX;

    if ((ret = _dal_maple_acl_rule_move(unit, pData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_acl_igrRule_move */

/* Function Name:
 *      dal_maple_acl_lookupMissAct_get
 * Description:
 *      Get the acl lookup miss action of the specific port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAct - pointer to lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_lookupMissAct_get(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t *pAct)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pAct, RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_ACL_PORT_LOOKUP_CTRLr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        MAPLE_DEFACTf,
                        pAct)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_lookupMissAct_get */

/* Function Name:
 *      dal_maple_acl_lookupMissAct_set
 * Description:
 *      Set the acl lookup miss action of the specific port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      act  - lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_acl_lookupMissAct_set(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t act)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(act >= ACL_LOOKUPMISS_ACTION_END, RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_PORT_LOOKUP_CTRLr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        MAPLE_DEFACTf,
                        &act)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_lookupMissAct_set */

/* Function Name:
 *      dal_maple_acl_portLookupEnable_get
 * Description:
 *      Get the acl lookup state of the specific port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable - pointer to lookup state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_acl_portLookupEnable_get(uint32 unit, rtk_port_t port, uint32 *pEnable)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        MAPLE_ACL_PORT_LOOKUP_CTRLr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        MAPLE_LOOKUP_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_portLookupEnable_get */

/* Function Name:
 *      dal_maple_acl_portLookupEnable_set
 * Description:
 *      Set the acl lookup state of the specific port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - lookup state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_acl_portLookupEnable_set(uint32 unit, rtk_port_t port, uint32 enable)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        MAPLE_ACL_PORT_LOOKUP_CTRLr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        MAPLE_LOOKUP_ENf,
                        &enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_portLookupEnable_set */

/* Function Name:
 *      dal_maple_acl_statByteCnt_set
 * Description:
 *      Set byte-based statistic counter of the log id.
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
dal_maple_acl_statByteCnt_set(uint32 unit, uint32 log_id, uint64 *pByte_cnt)
{
    int32   ret;
    uint32  cnt[2];
    log_entry_t entry;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= HAL_MAX_NUM_OF_PIE_COUNTER(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pByte_cnt), RT_ERR_NULL_POINTER);

    osal_memset(&entry, 0, sizeof(log_entry_t));

    cnt[0] = *pByte_cnt &0xffffffff;
    cnt[1] = (*pByte_cnt >> 32) & 0xffffffff;

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = table_field_set(unit, MAPLE_LOGt, MAPLE_LOG_CNTRtf, cnt, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = table_write(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_statByteCnt_set */

/* Function Name:
 *      dal_maple_acl_statPktCnt_set
 * Description:
 *      Set packet-based statistic counter of the log id.
 * Input:
 *      unit     - unit id
 *      log_id   - log id
 * Output:
 *      pPkt_cnt - pointer buffer of packet count
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
dal_maple_acl_statPktCnt_set(uint32 unit, uint32 log_id, uint32 *pPkt_cnt)
{
    int32   ret;
    uint32  cnt[2];
    log_entry_t entry;
    uint32  MSB;
    uint64  counterVal = 0;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((log_id >= (HAL_MAX_NUM_OF_PIE_COUNTER(unit)*2)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pPkt_cnt), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    if (log_id % 2 == 1)
       MSB = 1;
    else
       MSB = 0;
    log_id = log_id/2;

    dal_maple_acl_statByteCnt_get(unit, log_id, &counterVal);

    if (1 == MSB)
    {
        cnt[0] = counterVal & 0xffffffff;
        cnt[1] = *pPkt_cnt;
    }
    else
    {
        cnt[0] = *pPkt_cnt;
        cnt[1] = (counterVal >> 32) & 0xffffffff;
    }

    osal_memset(&entry, 0, sizeof(log_entry_t));

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = table_field_set(unit, MAPLE_LOGt, MAPLE_LOG_CNTRtf, cnt, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = table_write(unit, MAPLE_LOGt, log_id, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_acl_statPktCnt_set */

/* Function Name:
 *      dal_maple_acl_meter_tokenCounter_get
 * Description:
 *      Get acl token counter of the specified meter id.
 * Input:
 *      unit     - unit id
 *      meterIdx - meter id
 * Output:
 *      pCntr    - pointer buffer of token counter
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
dal_maple_acl_meter_tokenCounter_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pCntr)
{
    int32           ret;
    meter_entry_t   meter_entry;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_COUNTERf,
                          pCntr)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meter_tokenCounter_get */

/* Function Name:
 *      dal_maple_acl_meter_tokenCounter_set
 * Description:
 *      Set acl token counter of the specified meter id.
 * Input:
 *      unit     - unit id
 *      meterIdx - meter id
 * Output:
 *      pCntr    - pointer buffer of token counter
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
dal_maple_acl_meter_tokenCounter_set(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pCntr)
{

    int32           ret;
    meter_entry_t   meter_entry;
    rtk_maple_reg_list_t meterEntryReg[] =
    {
        MAPLE_METER_ENTRY_CTRL0r,
        MAPLE_METER_ENTRY_CTRL1r,
        MAPLE_METER_ENTRY_CTRL2r,
        MAPLE_METER_ENTRY_CTRL3r,
        MAPLE_METER_ENTRY_CTRL4r,
        MAPLE_METER_ENTRY_CTRL5r,
        MAPLE_METER_ENTRY_CTRL6r,
        MAPLE_METER_ENTRY_CTRL7r
    };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          meterEntryReg[meterIdx/32],
                          REG_ARRAY_INDEX_NONE,
                          meterIdx,
                          MAPLE_COUNTERf,
                          pCntr)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_acl_meter_tokenCounter_set */
