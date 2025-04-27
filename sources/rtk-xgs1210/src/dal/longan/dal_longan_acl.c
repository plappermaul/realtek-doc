/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision: 96152 $
 * $Date: 2019-04-03 11:00:59 +0800 (Wed, 03 Apr 2019) $
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Ingress ACL
 *            2) Egress ACL
 *            3) Counter
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
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/longan/dal_longan_acl.h>
#include <dal/longan/dal_longan_pie.h>
#include <dal/longan/dal_longan_l3.h>
#include <rtk/default.h>
#include <rtk/acl.h>
#include <rtk/pie.h>

/*
 * Symbol Definition
 */
#define DAL_LONGAN_MAX_NUM_OF_FIXED_FIELD      3
#define DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD   12
#define DAL_LONGAN_BYTE_OF_PER_TEMPLATE_FIELD  2
#define DAL_LONGAN_BYTE_OF_TEMPLATE_FIELD      (DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD * DAL_LONGAN_BYTE_OF_PER_TEMPLATE_FIELD)
#define DAL_LONGAN_DATA_BITS                   8
#define DAL_LONGAN_FILED_BITS                  16
#define DAL_LONGAN_BUFFER_UNIT_LENGTH_BITS     32

#define DAL_LONGAN_MAX_INFO_IDX                \
    (DAL_LONGAN_BUFFER_UNIT_LENGTH_BITS / DAL_LONGAN_DATA_BITS)

#define DAL_LONGAN_DATA_WIDTH_GET(_data_len)                   \
    (((_data_len) + DAL_LONGAN_DATA_BITS - 1) / DAL_LONGAN_DATA_BITS)

#define DAL_LONGAN_GET_BYTE_IDX(_offset)                       \
    ((_offset) / DAL_LONGAN_DATA_BITS)

#define DAL_LONGAN_GET_INFO_IDX(_size, _offset)                \
    (DAL_LONGAN_DATA_WIDTH_GET((_size)) - DAL_LONGAN_DATA_WIDTH_GET((_offset)))

#define DAL_LONGAN_GET_INFO_OFFSET(_max, _idx)                       \
    ((((_idx) - ((_max) % DAL_LONGAN_MAX_INFO_IDX)) % DAL_LONGAN_MAX_INFO_IDX) * DAL_LONGAN_DATA_BITS)

/*
 * Data Declaration
 */
static uint32               acl_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         acl_sem[RTK_MAX_NUM_OF_UNIT];

typedef struct dal_longan_acl_entryTable_s
{
    uint16   data_field[DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD];
    uint8    data_fixed[DAL_LONGAN_MAX_NUM_OF_FIXED_FIELD];
    uint16   care_field[DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD];
    uint8    care_fixed[DAL_LONGAN_MAX_NUM_OF_FIXED_FIELD];
} dal_longan_acl_entryTable_t;

typedef struct dal_longan_acl_fieldLocation_s
{
    uint32   template_field_type;
    uint32   field_offset;
    uint32   field_length;
    uint32   data_offset;
} dal_longan_acl_fieldLocation_t;

typedef struct dal_longan_acl_fixField_s
{
    uint32                          data_field;     /* data field in chip view */
    uint32                          mask_field;     /* mask field in chip view */
    uint32                          position;       /* position in fix data */
    rtk_acl_fieldType_t             type;           /* field type in user view */
    dal_longan_acl_fieldLocation_t *pField;
} dal_longan_acl_fixField_t;

typedef struct dal_longan_acl_entryField_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          field_number;   /* locate in how many fields */
    dal_longan_acl_fieldLocation_t *pField;
} dal_longan_acl_entryField_t;

typedef struct dal_longan_acl_entryFieldInfo_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          fieldLen;       /* field length */
} dal_longan_acl_entryFieldInfo_t;

typedef struct dal_longan_phaseInfo_s
{
    dal_longan_acl_fixField_t    *fixFieldList;
    uint32                      table;
    uint32                      *tmplteDataFieldList, *tmplteMaskFieldList;
} dal_longan_phaseInfo_t;

static dal_longan_acl_entryFieldInfo_t dal_longan_acl_specialField_list[] =
{
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR6,
        /* field length  */           16,
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR7,
        /* field length  */           16,
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR8,
        /* field length  */           16,
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR9,
        /* field length  */           16,
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR10,
        /* field length  */           16,
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR11,
        /* field length  */           16,
    },
    {   /* field name    */           USER_FIELD_IP6_SIP,
        /* field length  */           128,
    },
};

static uint32 template_vacl_data_field[DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD] = {
    LONGAN_VACL_FIELD_0tf, LONGAN_VACL_FIELD_1tf, LONGAN_VACL_FIELD_2tf,
    LONGAN_VACL_FIELD_3tf, LONGAN_VACL_FIELD_4tf, LONGAN_VACL_FIELD_5tf,
    LONGAN_VACL_FIELD_6tf, LONGAN_VACL_FIELD_7tf, LONGAN_VACL_FIELD_8tf,
    LONGAN_VACL_FIELD_9tf, LONGAN_VACL_FIELD_10tf, LONGAN_VACL_FIELD_11tf};

static uint32 template_vacl_mask_field[DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD] = {
    LONGAN_VACL_BMSK_FIELD_0tf, LONGAN_VACL_BMSK_FIELD_1tf, LONGAN_VACL_BMSK_FIELD_2tf,
    LONGAN_VACL_BMSK_FIELD_3tf, LONGAN_VACL_BMSK_FIELD_4tf, LONGAN_VACL_BMSK_FIELD_5tf,
    LONGAN_VACL_BMSK_FIELD_6tf, LONGAN_VACL_BMSK_FIELD_7tf, LONGAN_VACL_BMSK_FIELD_8tf,
    LONGAN_VACL_BMSK_FIELD_9tf, LONGAN_VACL_BMSK_FIELD_10tf, LONGAN_VACL_BMSK_FIELD_11tf};

static uint32 template_iacl_data_field[DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD] = {
    LONGAN_IACL_FIELD_0tf, LONGAN_IACL_FIELD_1tf, LONGAN_IACL_FIELD_2tf,
    LONGAN_IACL_FIELD_3tf, LONGAN_IACL_FIELD_4tf, LONGAN_IACL_FIELD_5tf,
    LONGAN_IACL_FIELD_6tf, LONGAN_IACL_FIELD_7tf, LONGAN_IACL_FIELD_8tf,
    LONGAN_IACL_FIELD_9tf, LONGAN_IACL_FIELD_10tf, LONGAN_IACL_FIELD_11tf};

static uint32 template_iacl_mask_field[DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD] = {
    LONGAN_IACL_BMSK_FIELD_0tf, LONGAN_IACL_BMSK_FIELD_1tf, LONGAN_IACL_BMSK_FIELD_2tf,
    LONGAN_IACL_BMSK_FIELD_3tf, LONGAN_IACL_BMSK_FIELD_4tf, LONGAN_IACL_BMSK_FIELD_5tf,
    LONGAN_IACL_BMSK_FIELD_6tf, LONGAN_IACL_BMSK_FIELD_7tf, LONGAN_IACL_BMSK_FIELD_8tf,
    LONGAN_IACL_BMSK_FIELD_9tf, LONGAN_IACL_BMSK_FIELD_10tf, LONGAN_IACL_BMSK_FIELD_11tf};

uint32 vacl_only_field_type[] = {USER_FIELD_IP_SUBNET_BASED_HIT, USER_FIELD_MAC_BASED_HIT,
    USER_FIELD_IVC_HIT, USER_FIELD_END};

uint32 iacl_only_field_type[] = {USER_FIELD_IPUC_ROUT, USER_FIELD_IPMC_ROUT,
    USER_FIELD_VACL_DROP_HIT,USER_FIELD_VACL_COPY_HIT,USER_FIELD_VACL_REDIRECT_HIT, USER_FIELD_ATTACK,
    USER_FIELD_SA_LUT_RESULT, USER_FIELD_DIP_HOST_HIT,USER_FIELD_DIP_PREFIX_HIT, USER_FIELD_DA_LUT_RESULT,
    USER_FIELD_URPF_CHK_FAIL, USER_FIELD_PORT_MV, USER_FIELD_VLAN_GRPMSK, USER_FIELD_IGR_VLAN_DROP,
    USER_FIELD_STP_DROP, USER_FIELD_DLP, USER_FIELD_SRC_FWD_VID, USER_FIELD_END};

dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TEMPLATE_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGR_NML_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FRAME_TYPE_L2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ITAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_OTAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ITAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_OTAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FRAME_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FRAME_TYPE_L4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP_NONZERO_OFFSET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_IGR_FIELD_CONTENT_TOO_DEEP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DEV_DMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_MGNT_VLAN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SPN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 5,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TRK_PRESENT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TRK_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_STACTING_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SPM[] =
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DMAC[] =
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SMAC[] =
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SENDER_ADDR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20,
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TARGET_ADDR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DIP2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x20,
    },
    {
        /* template field type */    TMPLTE_FIELD_FLOW_LABEL,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10,
    },
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ETHERTYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ETHERTYPE,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_OTAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DEI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_OTAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ITAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_CFI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ITAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ETAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ARPOPCODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP4_SIP[] =
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP4_DIP[] =
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_SIP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SIP7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x70,
    },
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_11,
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
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_10,
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
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_9,
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
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_8,
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
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_7,
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_DIP[] =
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
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP4TOS_IP6TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP_DSCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0xA,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP4PROTO_IP6NH[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_AFTER_ETHERTYPE_BYTE_0_1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_L4_SRC_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_L4_DST_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_MOB_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_ESP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_AUTH_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_DEST_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_FRAG_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_ROUTING_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP6_HOP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x3,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP_FRAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP4_TTL_IP6_HOPLIMIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGMP_MAX_RESP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0xe,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TCP_ECN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TCP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_TCP_NONZERO_SEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ICMP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ICMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_PORT_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_VLAN_GMSK,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_LEN_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_L3_LEN_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0,
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR_VALID_MSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_L2_CRC_ERROR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP4_HDR_ERR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IPMC_ROUT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IPUC_ROUT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xf,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR0[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR3[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR5[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR6[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR7[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR8[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR9[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR10[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FIELD_SELECTOR11[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_SIP7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_URPF_CHK_FAIL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_PORT_MV[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGR_VLAN_DROP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_STP_DROP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SPP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0xa,
        /* length */                 5,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DATYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x8,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IP_SUBNET_BASED_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_MAC_BASED_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x1,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IVC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_VACL_DROP_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_VACL_COPY_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_VACL_REDIRECT_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x5,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_ATTACK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x4,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SMAC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x3,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DIP_HOST_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DIP_PREFIX_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x1,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DMAC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_CONTENT_TOO_DEEP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FWD_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_VLAN_GRPMSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_VLAN_GMSK,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_META_DATA[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_LB_TIMES[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0x8,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_LB_PKT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SRC_FWD_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SRC_FWD_VID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_FLOW_LABEL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x9,
        /* length */                 4,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_FLOW_LABEL,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DSAP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SSAP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SNAP_OUI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_L4_HDR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_SLP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGR_DEV_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x6,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGR_TRK_PRESENT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_IGR_TRK_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_DLP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_EGR_DEV_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0xc,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_EGR_TRK_PRESENT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_longan_acl_fieldLocation_t DAL_LONGAN_FIELD_EGR_TRK_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};

dal_longan_acl_fixField_t dal_longan_acl_vFixField_list[] =
{
    {   /* data field       */  LONGAN_VACL_TIDtf,
        /* mask field name  */  LONGAN_VACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_LONGAN_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  LONGAN_VACL_IGR_NML_PORTtf,
        /* mask field name  */  LONGAN_VACL_BMSK_IGR_NML_PORTtf,
        /* position         */  1,
        /* field name       */  USER_FIELD_IGR_NML_PORT,
        /* field pointer    */  DAL_LONGAN_FIELD_IGR_NML_PORT
    },
    {   /* data field       */  LONGAN_VACL_FRAME_TYPE_L2tf,
        /* mask field name  */  LONGAN_VACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_LONGAN_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  LONGAN_VACL_INNER_TAG_PRITAGtf,
        /* mask field name  */  LONGAN_VACL_BMSK_INNER_TAG_PRITAGtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_LONGAN_FIELD_ITAG_EXIST
    },
    {   /* data field       */  LONGAN_VACL_OUTER_TAG_PRITAGtf,
        /* mask field name  */  LONGAN_VACL_BMSK_OUTER_TAG_PRITAGtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_LONGAN_FIELD_OTAG_EXIST
    },
    {   /* data field       */  LONGAN_VACL_INNER_UNTAG_PRITAGtf,
        /* mask field name  */  LONGAN_VACL_BMSK_INNER_UNTAG_PRITAGtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_LONGAN_FIELD_ITAG_FMT
    },
    {   /* data field       */  LONGAN_VACL_OUTER_UNTAG_PRITAGtf,
        /* mask field name  */  LONGAN_VACL_BMSK_OUTER_UNTAG_PRITAGtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_LONGAN_FIELD_OTAG_FMT
    },
    {   /* data field       */  LONGAN_VACL_FRAME_TYPEtf,
        /* mask field name  */  LONGAN_VACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_LONGAN_FIELD_FRAME_TYPE
    },
    {   /* data field       */  LONGAN_VACL_FRAME_TYPE_L4tf,
        /* mask field name  */  LONGAN_VACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_LONGAN_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  LONGAN_VACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  LONGAN_VACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_LONGAN_FIELD_IP_NONZERO_OFFSET
    },
    {   /* data field       */  LONGAN_VACL_CONTENT_TOO_DEEPtf,
        /* mask field name  */  LONGAN_VACL_BMSK_CONTENT_TOO_DEEPtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_CONTENT_TOO_DEEP,
        /* field pointer    */  DAL_LONGAN_IGR_FIELD_CONTENT_TOO_DEEP
    },
    {   /* data field       */  LONGAN_VACL_MGNT_VLANtf,
        /* mask field name  */  LONGAN_VACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_LONGAN_FIELD_MGNT_VLAN
    },
    {   /* data field       */  LONGAN_VACL_SLPtf,
        /* mask field name  */  LONGAN_VACL_BMSK_SLPtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_SPN,
        /* field pointer    */  DAL_LONGAN_FIELD_SPN
    },
    {   /* data field       */  LONGAN_VACL_SLPtf,
        /* mask field name  */  LONGAN_VACL_BMSK_SLPtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_TRK_PRESENT,
        /* field pointer    */  DAL_LONGAN_FIELD_TRK_PRESENT
    },
    {   /* data field       */  LONGAN_VACL_SLPtf,
        /* mask field name  */  LONGAN_VACL_BMSK_SLPtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_TRK_ID,
        /* field pointer    */  DAL_LONGAN_FIELD_TRK_ID
    },
    {   /* data field       */  LONGAN_VACL_STACKING_PORTtf,
        /* mask field name  */  LONGAN_VACL_BMSK_STACKING_PORTtf,
        /* position         */  23,
        /* field name       */  USER_FIELD_STACKING_PORT,
        /* field pointer    */  DAL_LONGAN_FIELD_STACTING_PORT
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_longan_acl_vFixField_list */

dal_longan_acl_fixField_t dal_longan_acl_iFixField_list[] =
{
    {   /* data field       */  LONGAN_IACL_TIDtf,
        /* mask field name  */  LONGAN_IACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_LONGAN_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  LONGAN_IACL_IGR_NML_PORTtf,
        /* mask field name  */  LONGAN_IACL_BMSK_IGR_NML_PORTtf,
        /* position         */  1,
        /* field name       */  USER_FIELD_IGR_NML_PORT,
        /* field pointer    */  DAL_LONGAN_FIELD_IGR_NML_PORT
    },
    {   /* data field       */  LONGAN_IACL_FRAME_TYPE_L2tf,
        /* mask field name  */  LONGAN_IACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_LONGAN_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  LONGAN_IACL_INNER_TAG_PRITAGtf,
        /* mask field name  */  LONGAN_IACL_BMSK_INNER_TAG_PRITAGtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_LONGAN_FIELD_ITAG_EXIST
    },
    {   /* data field       */  LONGAN_IACL_OUTER_TAG_PRITAGtf,
        /* mask field name  */  LONGAN_IACL_BMSK_OUTER_TAG_PRITAGtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_LONGAN_FIELD_OTAG_EXIST
    },
    {   /* data field       */  LONGAN_IACL_INNER_UNTAG_PRITAGtf,
        /* mask field name  */  LONGAN_IACL_BMSK_INNER_UNTAG_PRITAGtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_LONGAN_FIELD_ITAG_FMT
    },
    {   /* data field       */  LONGAN_IACL_OUTER_UNTAG_PRITAGtf,
        /* mask field name  */  LONGAN_IACL_BMSK_OUTER_UNTAG_PRITAGtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_LONGAN_FIELD_OTAG_FMT
    },
    {   /* data field       */  LONGAN_IACL_FRAME_TYPEtf,
        /* mask field name  */  LONGAN_IACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_LONGAN_FIELD_FRAME_TYPE
    },
    {   /* data field       */  LONGAN_IACL_FRAME_TYPE_L4tf,
        /* mask field name  */  LONGAN_IACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_LONGAN_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  LONGAN_IACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  LONGAN_IACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_LONGAN_FIELD_IP_NONZERO_OFFSET
    },
    {   /* data field       */  LONGAN_IACL_DEV_DMACtf,
        /* mask field name  */  LONGAN_IACL_BMSK_DEV_DMACtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_DEV_DMAC,
        /* field pointer    */  DAL_LONGAN_FIELD_DEV_DMAC
    },
    {   /* data field       */  LONGAN_IACL_MGNT_VLANtf,
        /* mask field name  */  LONGAN_IACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_LONGAN_FIELD_MGNT_VLAN
    },
    {   /* data field       */  LONGAN_IACL_SLPtf,
        /* mask field name  */  LONGAN_IACL_BMSK_SLPtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_SPN,
        /* field pointer    */  DAL_LONGAN_FIELD_SPN
    },
    {   /* data field       */  LONGAN_IACL_SLPtf,
        /* mask field name  */  LONGAN_IACL_BMSK_SLPtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_TRK_PRESENT,
        /* field pointer    */  DAL_LONGAN_FIELD_TRK_PRESENT
    },
    {   /* data field       */  LONGAN_IACL_SLPtf,
        /* mask field name  */  LONGAN_IACL_BMSK_SLPtf,
        /* position         */  16,
        /* field name       */  USER_FIELD_TRK_ID,
        /* field pointer    */  DAL_LONGAN_FIELD_TRK_ID
    },
    {   /* data field       */  LONGAN_IACL_STACKING_PORTtf,
        /* mask field name  */  LONGAN_VACL_BMSK_STACKING_PORTtf,
        /* position         */  23,
        /* field name       */  USER_FIELD_STACKING_PORT,
        /* field pointer    */  DAL_LONGAN_FIELD_STACTING_PORT
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_longan_acl_iFixField_list */

dal_longan_acl_entryField_t dal_longan_acl_field_list[] =
{
    {   /* field name    */           USER_FIELD_SPM,
        /* field number  */           2,
        /* field pointer */           DAL_LONGAN_FIELD_SPM
    },
    {   /* field name    */           USER_FIELD_DMAC,
        /* field number  */           3,
        /* field pointer */           DAL_LONGAN_FIELD_DMAC
    },
    {   /* field name    */           USER_FIELD_SMAC,
        /* field number  */           3,
        /* field pointer */           DAL_LONGAN_FIELD_SMAC
    },
    {   /* field name    */           USER_FIELD_ETHERTYPE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ETHERTYPE
    },
    {   /* field name    */           USER_FIELD_ETHER_AUX,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_AFTER_ETHERTYPE_BYTE_0_1
    },
    {   /* field name    */           USER_FIELD_OTAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_OTAG_PRI
    },
    {   /* field name    */           USER_FIELD_DEI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DEI_VALUE
    },
    {   /* field name    */           USER_FIELD_OTAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_OTAG_VID
    },
    {   /* field name    */           USER_FIELD_ITAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ITAG_PRI
    },
    {   /* field name    */           USER_FIELD_CFI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_CFI_VALUE
    },
    {   /* field name    */           USER_FIELD_ITAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ITAG_VID
    },
    {   /* field name    */           USER_FIELD_ETAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ETAG_EXIST
    },
    {   /* field name    */           USER_FIELD_ARPOPCODE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ARPOPCODE
    },
    {   /* field name    */           USER_FIELD_IP4_SIP,
        /* field number  */           2,
        /* field pointer */           DAL_LONGAN_FIELD_IP4_SIP
    },
    {   /* field name    */           USER_FIELD_IP4_DIP,
        /* field number  */           2,
        /* field pointer */           DAL_LONGAN_FIELD_IP4_DIP
    },
    {   /* field name    */           USER_FIELD_IP6_SIP,
        /* field number  */           14,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_SIP
    },
    {   /* field name    */           USER_FIELD_IP6_DIP,
        /* field number  */           8,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_DIP
    },
    {   /* field name    */           USER_FIELD_IP4TOS_IP6TC,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP4TOS_IP6TC
    },
    {   /* field name    */           USER_FIELD_IP_DSCP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP_DSCP
    },
    {   /* field name    */           USER_FIELD_IP4PROTO_IP6NH,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP4PROTO_IP6NH
    },
    {   /* field name    */           USER_FIELD_IP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP_FLAG
    },
    {   /* field name    */           USER_FIELD_IP4_TTL_IP6_HOPLIMIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP4_TTL_IP6_HOPLIMIT
    },
    {   /* field name    */           USER_FIELD_L4_SRC_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_L4_SRC_PORT
    },
    {   /* field name    */           USER_FIELD_L4_DST_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_L4_DST_PORT
    },
    {   /* field name    */           USER_FIELD_IP6_MOB_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_MOB_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_ESP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_ESP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_AUTH_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_AUTH_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_DEST_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_DEST_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_FRAG_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_FRAG_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_ROUTING_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_ROUTING_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_HOP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP6_HOP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IGMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IGMP_TYPE
    },
    {   /* field name    */           USER_FIELD_IGMP_MAX_RESP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IGMP_MAX_RESP_CODE
    },
    {   /* field name    */           USER_FIELD_TCP_ECN,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_TCP_ECN
    },
    {   /* field name    */           USER_FIELD_TCP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_TCP_FLAG
    },
    {   /* field name    */           USER_FIELD_TCP_NONZEROSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_TCP_NONZERO_SEQ
    },
    {   /* field name    */           USER_FIELD_ICMP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ICMP_CODE
    },
    {   /* field name    */           USER_FIELD_ICMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ICMP_TYPE
    },
    {   /* field name    */           USER_FIELD_VID_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_RANGE
    },
    {   /* field name    */           USER_FIELD_L4_PORT_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_RANGE
    },
    {   /* field name    */           USER_FIELD_LEN_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_RANGE
    },
    {   /* field name    */           USER_FIELD_IP_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP_RANGE
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR_VALID_MSK,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR_VALID_MSK
    },
    {   /* field name    */           USER_FIELD_L2_CRC_ERROR,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_L2_CRC_ERROR
    },
    {   /* field name    */           USER_FIELD_IP4_HDR_ERR,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP4_HDR_ERR
    },
    {   /* field name    */           USER_FIELD_IPMC_ROUT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IPMC_ROUT
    },
    {   /* field name    */           USER_FIELD_IPUC_ROUT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IPUC_ROUT
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR0,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR0
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR1,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR1
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR2,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR2
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR3,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR3
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR4,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR4
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR5,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR5
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR6,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR6
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR7,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR7
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR8,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR8
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR9,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR9
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR10,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR10
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR11,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FIELD_SELECTOR11
    },
    {   /* field name    */           USER_FIELD_MAC_BASED_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_MAC_BASED_HIT
    },
    {   /* field name    */           USER_FIELD_IP_SUBNET_BASED_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP_SUBNET_BASED_HIT
    },
    {   /* field name    */           USER_FIELD_IVC_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IVC_HIT
    },
    {   /* field name    */           USER_FIELD_SA_LUT_RESULT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_SMAC_HIT
    },
    {   /* field name    */           USER_FIELD_DA_LUT_RESULT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DMAC_HIT
    },
    {   /* field name    */           USER_FIELD_DIP_HOST_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DIP_HOST_HIT
    },
    {   /* field name    */           USER_FIELD_DIP_PREFIX_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DIP_PREFIX_HIT
    },
    {   /* field name    */           USER_FIELD_URPF_CHK_FAIL,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_URPF_CHK_FAIL
    },
    {   /* field name    */           USER_FIELD_PORT_MV,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_PORT_MV
    },
    {   /* field name    */           USER_FIELD_IGR_VLAN_DROP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IGR_VLAN_DROP
    },
    {   /* field name    */           USER_FIELD_STP_DROP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_STP_DROP
    },
    {   /* field name    */           USER_FIELD_VACL_DROP_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_VACL_DROP_HIT
    },
    {   /* field name    */           USER_FIELD_VACL_COPY_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_VACL_COPY_HIT
    },
    {   /* field name    */           USER_FIELD_VACL_REDIRECT_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_VACL_REDIRECT_HIT
    },
    {   /* field name    */           USER_FIELD_ATTACK,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_ATTACK
    },
    {   /* field name    */           USER_FIELD_DP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DP
    },
    {   /* field name    */           USER_FIELD_FWD_VID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_FWD_VID
    },
    {   /* field name    */           USER_FIELD_SRC_FWD_VID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_SRC_FWD_VID
    },
    {   /* field name    */           USER_FIELD_META_DATA,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_META_DATA
    },
    {   /* field name    */           USER_FIELD_LB_TIMES,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_LB_TIMES
    },
    {   /* field name    */           USER_FIELD_LB_PKT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_LB_PKT
    },
    {   /* field name    */           USER_FIELD_VLAN_GRPMSK,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_VLAN_GRPMSK
    },
    {   /* field name    */           USER_FIELD_CONTENT_TOO_DEEP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_CONTENT_TOO_DEEP
    },
    {   /* field name    */           USER_FIELD_SPP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_SPP
    },
    {   /* field name    */           USER_FIELD_SENDER_ADDR,
        /* field number  */           3,
        /* field pointer */           DAL_LONGAN_FIELD_SENDER_ADDR
    },
    {   /* field name    */           USER_FIELD_TARGET_ADDR,
        /* field number  */           3,
        /* field pointer */           DAL_LONGAN_FIELD_TARGET_ADDR
    },
    {   /* field name    */           USER_FIELD_FLOW_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_LONGAN_FIELD_FLOW_LABEL
    },
    {   /* field name    */           USER_FIELD_DATYPE,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DATYPE
    },
    {   /* field name    */           USER_FIELD_DSAP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DSAP
    },
    {   /* field name    */           USER_FIELD_SSAP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_SSAP
    },
    {   /* field name    */           USER_FIELD_SNAP_OUI,
        /* field number  */           2,
        /* field pointer */           DAL_LONGAN_FIELD_SNAP_OUI
    },
    {   /* field name    */           USER_FIELD_IP_FRAG,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IP_FRAG
    },
    {   /* field name    */           USER_FIELD_L4_HDR,
        /* field number  */           2,
        /* field pointer */           DAL_LONGAN_FIELD_L4_HDR
    },
    {   /* field name    */           USER_FIELD_SLP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_SLP
    },
    {   /* field name    */           USER_FIELD_IGR_DEV_ID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IGR_DEV_ID
    },
    {   /* field name    */           USER_FIELD_IGR_TRK_PRESENT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IGR_TRK_PRESENT
    },
    {   /* field name    */           USER_FIELD_IGR_TRK_ID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_IGR_TRK_ID
    },
    {   /* field name    */           USER_FIELD_DLP,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_DLP
    },
    {   /* field name    */           USER_FIELD_EGR_DEV_ID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_EGR_DEV_ID
    },
    {   /* field name    */           USER_FIELD_EGR_TRK_PRESENT,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_EGR_TRK_PRESENT
    },
    {   /* field name    */           USER_FIELD_EGR_TRK_ID,
        /* field number  */           1,
        /* field pointer */           DAL_LONGAN_FIELD_EGR_TRK_ID
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
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

#define DAL_LONGAN_PHASE_ACL_TO_PIE(_aclPhase, _piePhase)                   \
do {                                                                        \
    switch (_aclPhase)                                                      \
    {                                                                       \
        case ACL_PHASE_VACL:                                             \
            _piePhase = PIE_PHASE_VACL;                                  \
            break;                                                          \
        case ACL_PHASE_IACL:                                             \
            _piePhase = PIE_PHASE_IACL;                                  \
            break;                                                          \
        default:                                                            \
            return RT_ERR_ACL_PHASE;                                        \
    }                                                                       \
} while(0)

/*
 * Function Declaration
 */
static int32 _dal_longan_acl_index_to_physical(uint32 _unit, rtk_acl_phase_t _phase, uint32 _logicIdx, uint32 *_phyIdx)
{
    int32 ret = RT_ERR_OK;
    rtk_pie_phase_t _pphase;

    if (PIE_ENTRY_IS_PHYSICAL_TYPE(_logicIdx))
    {
        *_phyIdx = _logicIdx & ~(1 << 31);
        if(*_phyIdx>=HAL_MAX_NUM_OF_PIE_FILTER_ID(_unit))
            return RT_ERR_ENTRY_INDEX;
    }
    else
    {
        switch (_phase)
        {
            case ACL_PHASE_VACL:
                _pphase = PIE_PHASE_VACL;
                break;
            case ACL_PHASE_IACL:
                _pphase = PIE_PHASE_IACL;
                break;
            default:
                return RT_ERR_ACL_PHASE;
        }
        ret = dal_longan_pie_index_to_physical(_unit, _pphase, _logicIdx, _phyIdx);
    }

    return ret;
}

static int32 _dal_longan_acl_phaseInfo_get(rtk_acl_phase_t phase,
    dal_longan_phaseInfo_t *phaseInfo)
{
    RT_PARAM_CHK((NULL == phaseInfo), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            phaseInfo->table = LONGAN_VACLt;
            phaseInfo->fixFieldList = dal_longan_acl_vFixField_list;
            phaseInfo->tmplteDataFieldList = template_vacl_data_field;
            phaseInfo->tmplteMaskFieldList = template_vacl_mask_field;
            break;
        case ACL_PHASE_IACL:
            phaseInfo->table = LONGAN_IACLt;
            phaseInfo->fixFieldList = dal_longan_acl_iFixField_list;
            phaseInfo->tmplteDataFieldList = template_iacl_data_field;
            phaseInfo->tmplteMaskFieldList = template_iacl_mask_field;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    return RT_ERR_OK;
}   /* end of _dal_longan_acl_phaseInfo_get */

static int32 _dal_longan_acl_fieldType_chk(uint32 unit, rtk_acl_phase_t phase, rtk_acl_fieldType_t type);

/* Function Name:
 *      dal_longan_acl_ruleEntryFieldSize_get
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
dal_longan_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size)
{
    uint32  i;
    uint32  index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d type=%d", unit, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);
    RT_PARAM_CHK((NULL == pField_size), RT_ERR_NULL_POINTER);

    /* check special field type */
    for (i = 0; i < (sizeof(dal_longan_acl_specialField_list)/sizeof(dal_longan_acl_entryFieldInfo_t)); ++i)
    {
        if (type == dal_longan_acl_specialField_list[i].type)
        {
            *pField_size = dal_longan_acl_specialField_list[i].fieldLen;
            return RT_ERR_OK;
        }
    }

    for (i = 0; dal_longan_acl_iFixField_list[i].type != USER_FIELD_END; i++)
    {
        if (type == dal_longan_acl_iFixField_list[i].type)
        {
            index = i;
            break;
        }
    }

    if (dal_longan_acl_iFixField_list[i].type != USER_FIELD_END)
    {
        /* Get field size */
        *pField_size = dal_longan_acl_iFixField_list[index].pField[0].field_length;
    }
    else
    {
        uint32  field_number;

        /* Get chip specific field type from database */
        for (i = 0; dal_longan_acl_field_list[i].type != USER_FIELD_END; i++)
        {
            if (type == dal_longan_acl_field_list[i].type)
            {
                index = i;
                break;
            }
        }

        RT_PARAM_CHK((dal_longan_acl_field_list[i].type == USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

        field_number = dal_longan_acl_field_list[index].field_number;

        /* Get field size */
        *pField_size = 0;
        for (i = 0; i < field_number; i++)
        {
            *pField_size += dal_longan_acl_field_list[index].pField[i].field_length;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pField_size=%d", *pField_size);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleEntrySize_get
 * Description:
 *      Get the rule entry size of ACL.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 * Output:
 *      pEntry_size - rule entry size of ACL entry
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
dal_longan_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size)
{
    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_size), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    *pEntry_size = (DAL_LONGAN_BYTE_OF_TEMPLATE_FIELD +
            DAL_LONGAN_MAX_NUM_OF_FIXED_FIELD) * 2;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleValidate_get
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
dal_longan_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    acl_entry_t entry;
    uint32      table, field;
    int32       ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            table = LONGAN_VACLt;
            field = LONGAN_VACL_VALIDtf;
            break;
        case ACL_PHASE_IACL:
            table = LONGAN_IACLt;
            field = LONGAN_IACL_VALIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, table, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, table, field, pValid, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pValid=%d", *pValid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleValidate_set
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
dal_longan_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    acl_entry_t entry;
    uint32      table, field;
    int32       ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((valid != 0) && (valid != 1), RT_ERR_INPUT);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            table = LONGAN_VACLt;
            field = LONGAN_VACL_VALIDtf;
            break;
        case ACL_PHASE_IACL:
            table = LONGAN_IACLt;
            field = LONGAN_IACL_VALIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, table, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, table, field, &valid, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_write(unit, table, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleEntry_read
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
dal_longan_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    dal_longan_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  size;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",
            unit, phase, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, entry_idx,(uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_longan_acl_ruleEntrySize_get(unit, phase, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(pEntry_buffer, &entry, size);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleEntry_write
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
dal_longan_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    dal_longan_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  size;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, pEntry_buffer=%x",
            unit, phase, entry_idx, pEntry_buffer);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, entry_idx,(uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_longan_acl_ruleEntrySize_get(unit, phase, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(&entry, pEntry_buffer, size);

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, phaseInfo.table, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

static int32 _dal_longan_acl_fieldInfo_get(rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, dal_longan_acl_entryField_t **fieldList,
    uint32 *fieldListIdx)
{
    uint32  idx;

    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == fieldList), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == fieldListIdx), RT_ERR_NULL_POINTER);

    if (ACL_PHASE_VACL == phase)
    {
        for (idx = 0; iacl_only_field_type[idx] != USER_FIELD_END; ++idx)
        {
            if (type == iacl_only_field_type[idx])
            {
                return RT_ERR_ACL_FIELD_TYPE;
            }
        }
    }

    if (ACL_PHASE_IACL == phase)
    {
        for (idx = 0; vacl_only_field_type[idx] != USER_FIELD_END; ++idx)
        {
            if (type == vacl_only_field_type[idx])
            {
                return RT_ERR_ACL_FIELD_TYPE;
            }
        }
    }

    for (idx = 0; dal_longan_acl_field_list[idx].type != USER_FIELD_END;
            ++idx)
    {
        if (type == dal_longan_acl_field_list[idx].type)
        {
            *fieldList = dal_longan_acl_field_list;
            *fieldListIdx = idx;
            return RT_ERR_OK;
        }
    }
    *fieldList = NULL;
    return RT_ERR_ACL_FIELD_TYPE;
}   /* end of _dal_longan_acl_fieldInfo_get */

static void
_dal_longan_acl_field2Buf_get(uint32 unit, dal_longan_acl_fieldLocation_t *field,
    rtk_acl_fieldType_t type, uint32 data, uint32 mask,
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

    field_offset = field->field_offset;
    field_length = field->field_length;
    data_offset = field->data_offset;

    dal_longan_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_LONGAN_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_LONGAN_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_LONGAN_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_LONGAN_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_LONGAN_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_LONGAN_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_LONGAN_MAX_INFO_IDX);
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
}   /* end of _dal_longan_acl_field2Buf_get */

static int32
_dal_longan_acl_ruleEntryBufField_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_fieldType_t type, uint32 *buf,
    uint8 *pData, uint8 *pMask)
{
    rtk_acl_templateIdx_t           tmplteIdx;
    rtk_pie_template_t              tmplte;
    rtk_pie_templateVlanSel_t       vlanSel;
    dal_longan_phaseInfo_t          phaseInfo;
    dal_longan_acl_entryField_t     *fieldList = dal_longan_acl_field_list;
    dal_longan_acl_fieldLocation_t  *fieldLocation;
    uint32                          field;
    uint32                          val;
    uint32                          blkIdx, fieldIdx, fieldNum;
    uint32                          fieldData = 0, fieldMask = 0;
    uint32                          data, mask;
    uint32                          fieldMatch = FALSE;
    uint32                          dalFieldType;
    uint32                          i;
    int32                           ret;
    rtk_pie_phase_t                 pphase;

    osal_memset(pData, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(pMask, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            field = LONGAN_VACL_TIDtf;
            break;
        case ACL_PHASE_IACL:
            field = LONGAN_IACL_TIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* handle fixed field type */
    for (i = 0; phaseInfo.fixFieldList[i].type != USER_FIELD_END; ++i)
    {
        if (type == phaseInfo.fixFieldList[i].type)
        {
            fieldLocation = &phaseInfo.fixFieldList[i].pField[0];

            fieldData = phaseInfo.fixFieldList[i].data_field;
            fieldMask = phaseInfo.fixFieldList[i].mask_field;

            if ((ret = table_field_get(unit, phaseInfo.table, fieldData,
                    &data, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_get(unit, phaseInfo.table, fieldMask,
                    &mask, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if (fieldData == LONGAN_VACL_SLPtf || fieldData == LONGAN_IACL_SLPtf)
            {
                /* find out corresponding buf data from field data */
                _dal_longan_acl_field2Buf_get(unit, fieldLocation, type,
                        data, mask, pData, pMask);
            }
            else
            {
                *pData = data;
                *pMask = mask;
            }

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    blkIdx = entryIdx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    /* find out the binding template of the block */
    osal_memset(&tmplteIdx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_longan_acl_templateSelector_get(unit, blkIdx, &tmplteIdx);
    /* get template fields */
    if ((ret = table_field_get(unit, phaseInfo.table, field, &data, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_pie_template_t));
    val = tmplteIdx.template_id[data];
    dal_longan_pie_template_get(unit, val, &tmplte);

    switch (val)
    {
        case 0 ... 2:
        case 4:
            DAL_LONGAN_PHASE_ACL_TO_PIE(phase, pphase);

            if ((ret = dal_longan_pie_templateVlanSel_get(unit, pphase, val,
                    &vlanSel)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            for (fieldIdx = 0; fieldIdx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
            {
                if (TMPLTE_FIELD_VLAN == tmplte.field[fieldIdx])
                {
                    switch (vlanSel)
                    {
                        case TMPLTE_VLAN_SEL_INNER:
                            tmplte.field[fieldIdx] = TMPLTE_FIELD_ITAG;
                            break;
                        case TMPLTE_VLAN_SEL_OUTER:
                        case TMPLTE_VLAN_SEL_FWD:
                            tmplte.field[fieldIdx] = TMPLTE_FIELD_OTAG;
                            break;
                        default:
                            return RT_ERR_INPUT;
                    }
                }
            }
            break;
        default:
            break;
    }

    /* translate field from RTK superset view to DAL view */
    if ((ret = _dal_longan_acl_fieldInfo_get(phase, type, &fieldList,
            &dalFieldType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* search template to check all field types */
    fieldMatch = FALSE;
    for (fieldNum = 0; fieldNum < fieldList[dalFieldType].field_number; ++fieldNum)
    {
        for (fieldIdx = 0; fieldIdx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
        {
            fieldLocation = &fieldList[dalFieldType].pField[fieldNum];

            /* check whether the user field type is pull in template, partial match is also allowed */
            if (fieldLocation->template_field_type == tmplte.field[fieldIdx])
            {
                fieldMatch = TRUE;

                fieldData = phaseInfo.tmplteDataFieldList[fieldIdx];
                fieldMask = phaseInfo.tmplteMaskFieldList[fieldIdx];

                /* data */
                if ((ret = table_field_get(unit, phaseInfo.table,
                        fieldData, &data, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, phaseInfo.table,
                        fieldMask, &mask, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* find out corresponding data field from field data */
                _dal_longan_acl_field2Buf_get(unit, fieldLocation, type,
                        data, mask, pData, pMask);
            }   /* if (field->template_field_type == tmplte.field[fieldIdx]) */
        }   /* for (fieldIdx = 0; fieldIdx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx) */
    }   /* for (fieldNum = 0; fieldNum < fieldList[dalFieldType].field_number; ++fieldNum) */

    /* can't find then return */
    if (fieldMatch != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pData=%x, pMask=%x", *pData, *pMask);
#if defined(CONFIG_SDK_ENDIAN_LITTLE)
    {
        uint32  index;
        uint8   temp=0, *pData_temp=NULL, *pMask_temp=NULL;
        uint32  total_length = 0;
        dal_longan_acl_ruleEntryFieldSize_get(unit, type, &total_length);
        for (index = 0; index <= (total_length / DAL_LONGAN_BUFFER_UNIT_LENGTH_BITS); index++)
        {
            pData_temp = pData + (index * (DAL_LONGAN_BUFFER_UNIT_LENGTH_BITS / 8));
            temp = *pData_temp;
            *pData_temp = *(pData_temp + 3);
            *(pData_temp + 3) = temp;
            temp = *(pData_temp+1);
            *(pData_temp + 1) = *(pData_temp + 2);
            *(pData_temp + 2) = temp;
            pMask_temp = pMask + (index * (DAL_LONGAN_BUFFER_UNIT_LENGTH_BITS / 8));
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
}   /* end of _dal_longan_acl_ruleEntryBufField_get */

/* Function Name:
 *      dal_longan_acl_ruleEntryField_get
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
dal_longan_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret = RT_ERR_FAILED;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if ((ret = _dal_longan_acl_ruleEntryBufField_get(unit, phase, entry_idx,
            type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static void
_dal_longan_acl_buf2Field_get(uint32 unit, dal_longan_acl_fieldLocation_t *field,
    rtk_acl_fieldType_t type, uint16 *data16, uint16 *mask16,
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

    field_offset = field->field_offset;
    field_length = field->field_length;
    data_offset = field->data_offset;

    dal_longan_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_LONGAN_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_LONGAN_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_LONGAN_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_LONGAN_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_LONGAN_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_LONGAN_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_LONGAN_MAX_INFO_IDX);
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
}   /* end of _dal_longan_acl_buf2Field_get */

static int32
_dal_longan_acl_ruleEntryBufField_set(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_fieldType_t type, uint32 *buf,
    uint8 *pData, uint8 *pMask)
{
    rtk_acl_templateIdx_t           tmplteIdx;
    rtk_pie_template_t              tmplte;
    rtk_pie_templateVlanSel_t       vlanSel;
    dal_longan_phaseInfo_t          phaseInfo;
    dal_longan_acl_entryField_t     *fieldList = dal_longan_acl_field_list;
    dal_longan_acl_fieldLocation_t  *fieldLocation;
    uint32                          field;
    uint32                          val;
    uint32                          blkIdx, fieldIdx, fieldNum;
    uint32                          fieldData = 0, fieldMask = 0;
    uint32                          data, mask;
    uint32                          fieldOfst, fieldLen;
    uint32                          fieldMatch = FALSE;
    uint32                          dalFieldType;
    uint32                          i;
    int32                           ret;
    uint16                          data16, mask16;
    rtk_pie_phase_t                 pphase;

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            field = LONGAN_VACL_TIDtf;
            break;
        case ACL_PHASE_IACL:
            field = LONGAN_IACL_TIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* handle fixed field type */
    for (i = 0; phaseInfo.fixFieldList[i].type != USER_FIELD_END; ++i)
    {
        if (type == phaseInfo.fixFieldList[i].type)
        {
            fieldLocation = &phaseInfo.fixFieldList[i].pField[0];

            fieldData = phaseInfo.fixFieldList[i].data_field;
            fieldMask = phaseInfo.fixFieldList[i].mask_field;

            if (fieldData == LONGAN_VACL_SLPtf || fieldData == LONGAN_IACL_SLPtf)
            {
                fieldOfst = fieldLocation->field_offset;
                fieldLen = fieldLocation->field_length;

                _dal_longan_acl_buf2Field_get(unit, fieldLocation, type,
                        &data16, &mask16, pData, pMask);

                if ((ret = table_field_get(unit, phaseInfo.table, fieldData,
                                &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                val &= ~(((1 << fieldLen)-1) << fieldOfst);
                data = val | data16;

                if ((ret = table_field_get(unit, phaseInfo.table, fieldMask,
                                &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                val &= ~(((1 << fieldLen)-1) << fieldOfst);
                mask = val | mask16;
            }
            else
            {
                data = *pData;
                mask = *pMask;
            }

            if ((ret = table_field_set(unit, phaseInfo.table, fieldData,
                    &data, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            if ((ret = table_field_set(unit, phaseInfo.table, fieldMask,
                    &mask, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    blkIdx = entryIdx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    /* find out the binding template of the block */
    osal_memset(&tmplteIdx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_longan_acl_templateSelector_get(unit, blkIdx, &tmplteIdx);
    /* get field 'template ID' to know the template that the entry maps to */
    if ((ret = table_field_get(unit, phaseInfo.table, field, &data, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_pie_template_t));
    val = tmplteIdx.template_id[data];
    dal_longan_pie_template_get(unit, val, &tmplte);

    switch (val)
    {
        case 0 ... 2:
        case 4:
            DAL_LONGAN_PHASE_ACL_TO_PIE(phase, pphase);

            if ((ret = dal_longan_pie_templateVlanSel_get(unit, pphase, val,
                    &vlanSel)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return ret;
            }
            for (fieldIdx = 0; fieldIdx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
            {
                if (TMPLTE_FIELD_VLAN == tmplte.field[fieldIdx])
                {
                    switch (vlanSel)
                    {
                        case TMPLTE_VLAN_SEL_INNER:
                            tmplte.field[fieldIdx] = TMPLTE_FIELD_ITAG;
                            break;
                        case TMPLTE_VLAN_SEL_OUTER:
                        case TMPLTE_VLAN_SEL_FWD:
                            tmplte.field[fieldIdx] = TMPLTE_FIELD_OTAG;
                            break;
                        default:
                            return RT_ERR_INPUT;
                    }
                }
            }
            break;
        default:
            break;
    }

    /* translate field from RTK superset view to DAL view */
    if ((ret = _dal_longan_acl_fieldInfo_get(phase, type, &fieldList,
            &dalFieldType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* search template to check all field types */
    fieldMatch = FALSE;
    for (fieldNum = 0; fieldNum < fieldList[dalFieldType].field_number; ++fieldNum)
    {
        for (fieldIdx = 0; fieldIdx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
        {
            fieldLocation = &fieldList[dalFieldType].pField[fieldNum];

            /* check whether the user field type is pulled in template, partial match is allowed */
            if (fieldLocation->template_field_type == tmplte.field[fieldIdx])
            {
                fieldMatch = TRUE;

                fieldOfst = fieldLocation->field_offset;
                fieldLen = fieldLocation->field_length;

                _dal_longan_acl_buf2Field_get(unit, fieldLocation, type,
                        &data16, &mask16, pData, pMask);

                fieldData = phaseInfo.tmplteDataFieldList[fieldIdx];
                fieldMask = phaseInfo.tmplteMaskFieldList[fieldIdx];

                /* data */
                if ((ret = table_field_get(unit, phaseInfo.table, fieldData, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                val &= ~(((1 << fieldLen)-1) << fieldOfst);
                val |= data16;

                if ((ret = table_field_set(unit, phaseInfo.table, fieldData, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, phaseInfo.table, fieldMask, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                val &= ~(((1 << fieldLen)-1) << fieldOfst);
                val |= mask16;

                if ((ret = table_field_set(unit, phaseInfo.table, fieldMask, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
            }   /* if (field->template_field_type == tmplte.field[fieldIdx]) */
        }   /* for (fieldIdx = 0; fieldIdx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx) */
    }   /* for (fieldNum = 0; fieldNum < fieldList[dal_field_type].field_number; ++fieldNum) */

    /* no matched filed in template */
    if (fieldMatch != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    return RT_ERR_OK;
}   /* end of _dal_longan_acl_ruleEntryBufField_set */

/* Function Name:
 *      dal_longan_acl_ruleEntryField_set
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
dal_longan_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if ((ret = _dal_longan_acl_ruleEntryBufField_set(unit, phase, entry_idx,
            type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleEntryField_read
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
dal_longan_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    dal_longan_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &phyEntryId)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_longan_acl_ruleEntryBufField_get(unit, phase,
            phyEntryId, type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "type:%u", type);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleEntryField_write
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
dal_longan_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    dal_longan_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &phyEntryId)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_longan_acl_ruleEntryBufField_set(unit, phase,
            phyEntryId, type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "type:%u", type);
        return ret;
    }

    ACL_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleEntryField_check
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
dal_longan_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, type=%d",\
            unit, phase, type);

    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    if ((ret = _dal_longan_acl_fieldType_chk(unit, phase, type)) != RT_ERR_OK)
    {
        //RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_longan_acl_ruleEntryField_check */

/* Function Name:
 *      dal_longan_acl_ruleOperation_get
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
dal_longan_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    dal_longan_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  fieldNot, fieldAnd1, fieldAnd2 = 0;
    int32                   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            fieldNot = LONGAN_VACL_NOTtf;
            fieldAnd1 = LONGAN_VACL_AND1tf;
            fieldAnd2 = LONGAN_VACL_AND2tf;
            break;
        case ACL_PHASE_IACL:
            fieldNot = LONGAN_IACL_NOTtf;
            fieldAnd1 = LONGAN_IACL_AND1tf;
            fieldAnd2 = LONGAN_IACL_AND2tf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, entry_idx,
            (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, phaseInfo.table, fieldNot,
            &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, phaseInfo.table, fieldAnd1,
            &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, phaseInfo.table, fieldAnd2,
            &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_ruleOperation_set
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
dal_longan_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    dal_longan_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  fieldNot, fieldAnd1, fieldAnd2 = 0;
    int32                   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            fieldNot = LONGAN_VACL_NOTtf;
            fieldAnd1 = LONGAN_VACL_AND1tf;
            fieldAnd2 = LONGAN_VACL_AND2tf;
            break;
        case ACL_PHASE_IACL:
            fieldNot = LONGAN_IACL_NOTtf;
            fieldAnd1 = LONGAN_IACL_AND1tf;
            fieldAnd2 = LONGAN_IACL_AND2tf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, entry_idx,
            (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, phaseInfo.table, fieldNot,
        &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, phaseInfo.table, fieldAnd1,
        &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, phaseInfo.table, fieldAnd2,
        &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, phaseInfo.table, entry_idx,
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
_dal_longan_acl_vRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_vAct_t     *pAction)
{
    int32           ret;
    uint32          value, info;
    acl_entry_t   entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, action_idx=%d", unit, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    if ((ret = dal_longan_pie_index_chk(unit, ACL_PHASE_VACL, entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    osal_memset(pAction, 0x0, sizeof(rtk_acl_vAct_t));

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_VACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_DROPtf,\
        &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_DROP_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->green_drop_data = ACL_ACTION_COLOR_DROP_PERMIT;
            break;
        case 1:
            pAction->green_drop_data = ACL_ACTION_COLOR_DROP_DROP;
            break;
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_YELLOW_DROPtf,\
        &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_YELLOW_DROP_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->yellow_drop_data = ACL_ACTION_COLOR_DROP_PERMIT;
            break;
        case 1:
            pAction->yellow_drop_data = ACL_ACTION_COLOR_DROP_DROP;
            break;
    }

    /* red drop action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_RED_DROPtf,\
        &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_RED_DROP_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->red_drop_data = ACL_ACTION_COLOR_DROP_PERMIT;
            break;
        case 1:
            pAction->red_drop_data = ACL_ACTION_COLOR_DROP_DROP;
            break;
    }

    /* Forwarding action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_FWD_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_FWD_PORT_INFOtf,\
            &info, (uint32 *) &entry), errHandle, ret);

    switch (value)
    {
        case 0:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_PERMIT;
            break;
        case 1:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_DROP;
            break;
        case 2:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            pAction->fwd_data.fwd_info = info & 0x3F;

            if (0 != (info >> 10))
                pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            else
                pAction->fwd_data.devID = (info >> 6) & 0xF;
            break;
        case 3:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTMASK;
            pAction->fwd_data.fwd_info = info;
            break;
        case 4:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            pAction->fwd_data.fwd_info = info & 0x3F;

            if (0 != (info >> 10))
                pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            else
                pAction->fwd_data.devID = (info >> 6) & 0xF;
            break;
        case 5:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTMASK;
            pAction->fwd_data.fwd_info = info;
            break;
        case 6:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_UNICAST_ROUTING;
            pAction->fwd_data.fwd_info = info & 0x7FF;
            switch(info)
            {
                case DAL_LONGAN_NULLINTF_DROP_NEXTHOP_IDX:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_DROP;
                    break;
                case DAL_LONGAN_NULLINTF_TRAP2CPU_NEXTHOP_IDX:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU;
                    break;
                case DAL_LONGAN_NULLINTF_TRAP2MASTER_NEXTHOP_IDX:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU;
                    break;
                default:
                    break;
            }
            break;
        case 7:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_DFLT_UNICAST_ROUTING;
            pAction->fwd_data.fwd_info = info & 0x7FF;
            switch(info)
            {
                case DAL_LONGAN_NULLINTF_DROP_NEXTHOP_IDX:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_DROP;
                    break;
                case DAL_LONGAN_NULLINTF_TRAP2CPU_NEXTHOP_IDX:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU;
                    break;
                case DAL_LONGAN_NULLINTF_TRAP2MASTER_NEXTHOP_IDX:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU;
                    break;
                default:
                    break;
            }
            break;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_FWD_CPU_PKT_FMTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    if (0 == value)
        pAction->fwd_data.fwd_cpu_fmt = ORIGINAL_PACKET;
    else if (1 == value)
        pAction->fwd_data.fwd_cpu_fmt = MODIFIED_PACKET;
    else
        pAction->fwd_data.fwd_cpu_fmt = PKT_FORMAT_NOP;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_FWD_SA_LRNtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    if (1 == value)
        pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_SA_NOT_LEARN;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_FWD_SELtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    if (1 == value)
        pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_OVERWRITE_DROP;

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_LOG_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (value == 0)
    {
        pAction->stat_data.stat_type = STAT_TYPE_PACKET_BASED_32BIT;
    }
    else
    {
        pAction->stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_MIRROR_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_ORIGINAL;
            RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_MIRROR_INDEXtf,\
                    &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_CANCEL;
            break;
    }

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_METER_INDEXtf,\
        &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_IVLANtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_VIDtf,\
        &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_OVLANtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_VIDtf,\
        &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    /* Ingress Inner Priority action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_IPRItf,\
        &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_PRI_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_PRItf,\
                    &pAction->inner_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_COPY_FROM_OUTER;
            break;
        case 2:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_KEEP;
            break;
    }

    /* Ingress Outer Priority action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_OPRItf,\
        &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_PRI_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_pri_data.act = ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_PRItf,\
                    &pAction->outer_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->outer_pri_data.act = ACL_ACTION_OUTER_PRI_COPY_FROM_INNER;
            break;
        case 2:
            pAction->outer_pri_data.act = ACL_ACTION_OUTER_PRI_KEEP;
            break;
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_TAG_STATUStf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_TAG_STATUS_INNERtf,\
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
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_KEEP_FORMAT;
            break;
        case 3:
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_NOP;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_TAG_STATUS_OUTERtf,\
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
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_KEEP_FORMAT;
            break;
        case 3:
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_NOP;
            break;
    }

    /* Priority action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_PRIORITYtf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_INTERNAL_PRItf,\
        &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* Bypass action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_BYPASStf,\
        &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_BYPASS_DATAtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    pAction->bypass_data.ibc_sc = (value & 0x1);
    pAction->bypass_data.igr_stp = (value & 0x2) >> 0x1;
    pAction->bypass_data.igr_vlan = (value & 0x4) >> 0x2;

    /* Meta data action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_METADATAtf,\
        &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_META_DATAtf,\
        &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);

    /* QID action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_QIDtf,\
        &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_QID_DATAtf,\
        &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);

    /* Remark action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_REMARKtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_REMARK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 1:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 2:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            break;
        case 3:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_REMARK_VALUEtf,\
        &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Yellow Remark action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_YELLOW_REMARKtf,\
        &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_YELLOW_REMARK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 1:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 2:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            break;
        case 3:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_YELLOW_REMARK_VALUEtf,\
        &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Red Remark action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_RED_REMARKtf,\
        &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_RED_REMARK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 1:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 2:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            break;
        case 3:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_RED_REMARK_VALUEtf,\
        &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /* Ip-rsvd flag invert action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_INVERT_IP_RSVD_FLAGtf,\
        &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pAction=%x", *pAction);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
} /* end of dal_longan_acl_igrRuleAction_get */

static int32
_dal_longan_acl_iRuleAction_get(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_iAct_t     *pAction)
{
    int32           ret;
    uint32          value, info;
    acl_entry_t   entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    osal_memset(pAction, 0x0, sizeof(rtk_acl_iAct_t));

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_DROPtf,\
        &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_DROP_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->green_drop_data = ACL_ACTION_COLOR_DROP_PERMIT;
            break;
        case 1:
            pAction->green_drop_data = ACL_ACTION_COLOR_DROP_DROP;
            break;
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_YELLOW_DROPtf,\
        &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_YELLOW_DROP_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->yellow_drop_data = ACL_ACTION_COLOR_DROP_PERMIT;
            break;
        case 1:
            pAction->yellow_drop_data = ACL_ACTION_COLOR_DROP_DROP;
            break;
    }

    /* red drop action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_RED_DROPtf,\
        &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_RED_DROP_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->red_drop_data = ACL_ACTION_COLOR_DROP_PERMIT;
            break;
        case 1:
            pAction->red_drop_data = ACL_ACTION_COLOR_DROP_DROP;
            break;
    }

    /* Forwarding action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_FWD_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_FWD_PORT_INFOtf,\
            &info, (uint32 *) &entry), errHandle, ret);

    switch (value)
    {
        case 0:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_PERMIT;
            break;
        case 1:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_DROP;
            break;
        case 2:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            pAction->fwd_data.fwd_info = info & 0x3F;

            if (0 != (info >> 10))
                pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            else
                pAction->fwd_data.devID = (info >> 6) & 0xF;
            break;
        case 3:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTMASK;
            pAction->fwd_data.fwd_info = info;
            break;
        case 4:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            pAction->fwd_data.fwd_info = info & 0x3F;

            if (0 != (info >> 10))
                pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            else
                pAction->fwd_data.devID = (info >> 6) & 0xF;
            break;
        case 5:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTMASK;
            pAction->fwd_data.fwd_info = info;
            break;
        case 6:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_FILTERING;
            break;
        case 7:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_LOOPBACK;
            break;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_FWD_CPU_PKT_FMTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    if (0 == value)
        pAction->fwd_data.fwd_cpu_fmt = ORIGINAL_PACKET;
    else if (1 == value)
        pAction->fwd_data.fwd_cpu_fmt = MODIFIED_PACKET;
    else
        pAction->fwd_data.fwd_cpu_fmt = PKT_FORMAT_NOP;

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_FWD_SELtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (1 == value)
        pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_OVERWRITE_DROP;

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_LOG_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    if (value == 0)
    {
        pAction->stat_data.stat_type = STAT_TYPE_PACKET_BASED_32BIT;
    }
    else
    {
        pAction->stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_MIRROR_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_ORIGINAL;
            RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_MIRROR_INDEXtf,\
                    &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_MODIFIED;
            RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_MIRROR_INDEXtf,\
                    &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
            break;
        case 2:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_CANCEL;
            break;
    }

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_METER_INDEXtf,\
        &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress I-VID/TPID assignment action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_IVLANtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID;
            break;
        case 3:
            pAction->inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NOP;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_VIDtf,\
        &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_TPID_ACTtf,\
        &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_TPID_IDXtf,\
        &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_OVLANtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_VID_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
            break;
        case 1:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
            break;
        case 2:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID;
            break;
        case 3:
            pAction->outer_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NOP;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_VIDtf,\
        &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_TPID_ACTtf,\
        &pAction->outer_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_TPID_IDXtf,\
        &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress Inner Priority action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_IPRItf,\
        &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_PRI_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_PRItf,\
                    &pAction->inner_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_COPY_FROM_OUTER;
            break;
        case 2:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_KEEP;
            break;
    }

    /* Egress Outer Priority action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_OPRItf,\
        &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_PRI_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_pri_data.act = ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_PRItf,\
                    &pAction->outer_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->outer_pri_data.act = ACL_ACTION_OUTER_PRI_COPY_FROM_INNER;
            break;
        case 2:
            pAction->outer_pri_data.act = ACL_ACTION_OUTER_PRI_KEEP;
            break;
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_TAG_STATUStf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_TAG_STATUS_INNERtf,\
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
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_KEEP_FORMAT;
            break;
        case 3:
            pAction->tag_sts_data.itag_sts = ACL_ACTION_TAG_STS_NOP;
            break;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_TAG_STATUS_OUTERtf,\
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
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_KEEP_FORMAT;
            break;
        case 3:
            pAction->tag_sts_data.otag_sts = ACL_ACTION_TAG_STS_NOP;
            break;
    }

    /* Priority action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_PRIORITYtf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_PRI_DATAtf,\
        &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* Bypass action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_BYPASStf,\
        &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_BYPASS_DATAtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    pAction->bypass_data.ibc_sc = (value & 0x1);

    /* Meta data action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_METADATAtf,\
        &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_META_DATAtf,\
        &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);

    /* QID action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_CPUQIDtf,\
        &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_QID_DATAtf,\
        &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);

    /* Remark action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_REMARKtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_REMARK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 1:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 2:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            break;
        case 3:
            pAction->rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_REMARK_VALUEtf,\
        &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Yellow Remark action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_YELLOW_REMARKtf,\
        &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_YELLOW_REMARK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 1:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 2:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            break;
        case 3:
            pAction->yellow_rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_YELLOW_REMARK_VALUEtf,\
        &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Red Remark action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_RED_REMARKtf,\
        &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_RED_REMARK_ACTtf,\
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            break;
        case 1:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            break;
        case 2:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            break;
        case 3:
            pAction->red_rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            break;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_RED_REMARK_VALUEtf,\
        &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /* Ip-rsvd flag invert action */
    RT_ERR_HDL(table_field_get(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_INVERT_IP_RSVD_FLAGtf,\
        &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      dal_longan_acl_ruleAction_get
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
dal_longan_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    int32   ret = RT_ERR_FAILED;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    if (phase == ACL_PHASE_VACL)
    {
        if ((ret = _dal_longan_acl_vRuleAction_get(unit, entry_idx, &pAction->vact)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_longan_acl_iRuleAction_get(unit, entry_idx, &pAction->iact)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_longan_acl_vRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_vAct_t     *pAction)
{
    int32               ret;
    uint32              value, info = 0;
    uint32              fwd_devID;
    uint32              multicast_tableSize;
    acl_entry_t       entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    if ((ret = dal_longan_pie_index_chk(unit, ACL_PHASE_VACL, entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_VACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_DROPtf,\
        &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->green_drop_en)
    {
        switch (pAction->green_drop_data)
        {
            case ACL_ACTION_COLOR_DROP_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_COLOR_DROP_DROP:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_DROP_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_YELLOW_DROPtf,\
        &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->yellow_drop_en)
    {
        switch (pAction->yellow_drop_data)
        {
            case ACL_ACTION_COLOR_DROP_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_COLOR_DROP_DROP:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_YELLOW_DROP_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* red drop action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_RED_DROPtf,\
        &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->red_drop_en)
    {
        switch (pAction->red_drop_data)
        {
            case ACL_ACTION_COLOR_DROP_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_COLOR_DROP_DROP:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_RED_DROP_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->fwd_en)
    {
        switch (pAction->fwd_data.fwd_type)
        {
            case ACL_ACTION_FWD_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_FWD_DROP:
                value = 1;
                break;
            case ACL_ACTION_FWD_COPY_TO_PORTID:
                value = 2;

                info = pAction->fwd_data.fwd_info & 0x3F;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                {
                    if (info > HAL_MAX_NUM_OF_TRUNK(unit))
                    {
                        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                        return RT_ERR_INPUT;
                    }
                    info |= (1 << 10);
                }
                else
                {
                    if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_LOCAL_DEV)
                    {
                        fwd_devID = HAL_UNIT_TO_DEV_ID(HWP_MY_UNIT_ID());
                    }
                    else
                    {
                        fwd_devID = pAction->fwd_data.devID;
                    }

                    info |= ((fwd_devID & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                value = 3;

                if ((ret = table_size_get(unit, LONGAN_MC_PORTMASKt,
                        &multicast_tableSize)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                if (pAction->fwd_data.fwd_info >= multicast_tableSize)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                info = pAction->fwd_data.fwd_info;
                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                value = 4;

                info = pAction->fwd_data.fwd_info & 0x3F;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                {
                    if (info > HAL_MAX_NUM_OF_TRUNK(unit))
                    {
                        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                        return RT_ERR_INPUT;
                    }
                    info |= (1 << 10);
                }
                else
                {
                    if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_LOCAL_DEV)
                    {
                        fwd_devID = HAL_UNIT_TO_DEV_ID(HWP_MY_UNIT_ID());
                    }
                    else
                    {
                        fwd_devID = pAction->fwd_data.devID;
                    }

                    info |= ((fwd_devID & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                value = 5;

                if ((ret = table_size_get(unit, LONGAN_MC_PORTMASKt,
                        &multicast_tableSize)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                if (pAction->fwd_data.fwd_info >= multicast_tableSize)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                info = pAction->fwd_data.fwd_info;
                break;
            case ACL_ACTION_FWD_UNICAST_ROUTING:
                value = 6;
                info = pAction->fwd_data.fwd_info & 0x7FF;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_DROP)
                {
                    info = DAL_LONGAN_NULLINTF_DROP_NEXTHOP_IDX;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU)
                {
                    info = DAL_LONGAN_NULLINTF_TRAP2CPU_NEXTHOP_IDX;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU)
                {
                    info = DAL_LONGAN_NULLINTF_TRAP2MASTER_NEXTHOP_IDX;
                }
                break;
            case ACL_ACTION_FWD_DFLT_UNICAST_ROUTING:
                value = 7;
                info = pAction->fwd_data.fwd_info & 0x7FF;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_DROP)
                {
                    info = DAL_LONGAN_NULLINTF_DROP_NEXTHOP_IDX;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU)
                {
                    info = DAL_LONGAN_NULLINTF_TRAP2CPU_NEXTHOP_IDX;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU)
                {
                    info = DAL_LONGAN_NULLINTF_TRAP2MASTER_NEXTHOP_IDX;
                }                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_FWD_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_FWD_PORT_INFOtf,\
            &info, (uint32 *) &entry), errHandle, ret);

        switch (pAction->fwd_data.fwd_cpu_fmt)
        {
            case ORIGINAL_PACKET:
                value = 0;
                break;
            case MODIFIED_PACKET:
                value = 1;
                break;
            case PKT_FORMAT_NOP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_FWD_CPU_PKT_FMTtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_SA_NOT_LEARN)
            value = 1;
        else
            value = 0;
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_FWD_SA_LRNtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_OVERWRITE_DROP)
            value = 1;
        else
            value = 0;
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_FWD_SELtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->stat_en)
    {
        if (pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT)
        {
            value = 0;
        }
        else if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
        {
            value = 1;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_LOG_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_MIRROR_INDEXtf,\
        &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->mirror_en)
    {
        if (pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        switch (pAction->mirror_data.mirror_type)
        {
            case ACL_ACTION_MIRROR_ORIGINAL:
                value = 0;
                break;
            case ACL_ACTION_MIRROR_CANCEL:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_MIRROR_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meter_en)
    {
        if (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_METER_INDEXtf,\
            &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_IVLANtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->inner_vlan_assign_en)
    {
        switch (pAction->inner_vlan_data.vid_assign_type)
        {
            case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                value = 0;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                value = 1;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_VID_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        value = pAction->inner_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_VIDtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_OVLANtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_vlan_assign_en)
    {
        switch (pAction->outer_vlan_data.vid_assign_type)
        {
            case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                value = 0;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                value = 1;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_VID_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        value = pAction->outer_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_VIDtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_IPRItf,\
        &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->inner_pri_en)
    {
        switch (pAction->inner_pri_data.act)
        {
            case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                if (pAction->inner_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->inner_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_PRItf,\
                        &value, (uint32 *) &entry), errHandle, ret);
                value = 0;
                break;
            case ACL_ACTION_INNER_PRI_COPY_FROM_OUTER:
                value = 1;
                break;
            case ACL_ACTION_INNER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_IVLAN_PRI_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Outer PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_OPRItf,\
        &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_pri_en)
    {
        switch (pAction->outer_pri_data.act)
        {
            case ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI:
                if (pAction->outer_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->outer_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_PRItf,\
                        &value, (uint32 *) &entry), errHandle, ret);
                value = 0;
                break;
            case ACL_ACTION_OUTER_PRI_COPY_FROM_INNER:
                value = 1;
                break;
            case ACL_ACTION_OUTER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_OVLAN_PRI_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_TAG_STATUStf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->tag_sts_en)
    {
        switch (pAction->tag_sts_data.itag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                value = 0;
                break;
            case ACL_ACTION_TAG_STS_TAG:
                value = 1;
                break;
            case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                value = 2;
                break;
            case ACL_ACTION_TAG_STS_NOP:
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.itag_sts error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_TAG_STATUS_INNERtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        switch (pAction->tag_sts_data.otag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                value = 0;
                break;
            case ACL_ACTION_TAG_STS_TAG:
                value = 1;
                break;
            case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                value = 2;
                break;
            case ACL_ACTION_TAG_STS_NOP:
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.otag_sts error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_TAG_STATUS_OUTERtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Priority action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_PRIORITYtf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->pri_en)
    {
        if (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_INTERNAL_PRItf,\
            &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);
    }

    /* Bypass action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_BYPASStf,\
        &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->bypass_en)
    {
        value = (pAction->bypass_data.ibc_sc & 0x1) |
            ((pAction->bypass_data.igr_stp & 0x1) << 0x1) |
            ((pAction->bypass_data.igr_vlan & 0x1) << 0x2);
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_BYPASS_DATAtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meta data action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_METADATAtf,\
        &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meta_data_en)
    {
        if (HAL_MAX_NUM_OF_METADATA(unit) <= pAction->meta_data.data)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_META_DATAtf,\
            &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);
    }

    /* QID action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_QIDtf,\
        &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->cpu_qid_en)
    {
        if (HAL_MAX_NUM_OF_CPU_QUEUE(unit) <= pAction->cpu_qid.qid)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_QID_DATAtf,\
            &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);
    }

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_REMARKtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->rmk_en)
    {
        switch (pAction->rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_DSCP:
                if (pAction->rmk_data.rmk_info > RTK_VALUE_OF_DSCP_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                if (pAction->rmk_data.rmk_info > RTK_VALUE_OF_IP_PRECEDENCE_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            case ACL_ACTION_REMARK_TOS:
                if (pAction->rmk_data.rmk_info > RTK_VALUE_OF_TOS_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 2;
                break;
            case ACL_ACTION_REMARK_EAV:
                if (pAction->rmk_data.rmk_info >= 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_REMARK_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_REMARK_VALUEtf,\
            &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }

    /* Yellow Remark action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_YELLOW_REMARKtf,\
        &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->yellow_rmk_en)
    {
        switch (pAction->yellow_rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_DSCP:
                if (pAction->yellow_rmk_data.rmk_info > RTK_VALUE_OF_DSCP_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                if (pAction->yellow_rmk_data.rmk_info > RTK_VALUE_OF_IP_PRECEDENCE_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            case ACL_ACTION_REMARK_TOS:
                if (pAction->yellow_rmk_data.rmk_info > RTK_VALUE_OF_TOS_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 2;
                break;
            case ACL_ACTION_REMARK_EAV:
                if (pAction->yellow_rmk_data.rmk_info >= 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_YELLOW_REMARK_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_YELLOW_REMARK_VALUEtf,\
            &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }

    /*Red Remark action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_RED_REMARKtf,\
        &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->red_rmk_en)
    {
        switch (pAction->red_rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_DSCP:
                if (pAction->red_rmk_data.rmk_info > RTK_VALUE_OF_DSCP_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                if (pAction->red_rmk_data.rmk_info > RTK_VALUE_OF_IP_PRECEDENCE_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            case ACL_ACTION_REMARK_TOS:
                if (pAction->red_rmk_data.rmk_info > RTK_VALUE_OF_TOS_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 2;
                break;
            case ACL_ACTION_REMARK_EAV:
                if (pAction->red_rmk_data.rmk_info >= 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_RED_REMARK_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_RED_REMARK_VALUEtf,\
            &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }

    RT_ERR_HDL(table_field_set(unit, LONGAN_VACLt, LONGAN_VACL_ACT_MSK_INVERT_IP_RSVD_FLAGtf,\
        &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, LONGAN_VACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
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
} /* end of dal_longan_acl_igrRuleAction_set */

static int32
_dal_longan_acl_iRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_iAct_t     *pAction)
{
    int32               ret;
    uint32              value, info = 0;
    acl_entry_t       entry;
    uint32              multicast_tableSize;
    uint32              fwd_devID;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    if ((ret = dal_longan_pie_index_chk(unit, ACL_PHASE_IACL, entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

   /* green drop action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_DROPtf,\
        &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->green_drop_en)
    {
        switch (pAction->green_drop_data)
        {
            case ACL_ACTION_COLOR_DROP_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_COLOR_DROP_DROP:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_DROP_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_YELLOW_DROPtf,\
        &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->yellow_drop_en)
    {
        switch (pAction->yellow_drop_data)
        {
            case ACL_ACTION_COLOR_DROP_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_COLOR_DROP_DROP:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_YELLOW_DROP_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* red drop action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_RED_DROPtf,\
        &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->red_drop_en)
    {
        switch (pAction->red_drop_data)
        {
            case ACL_ACTION_COLOR_DROP_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_COLOR_DROP_DROP:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_RED_DROP_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_FWDtf,\
        &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->fwd_en)
    {
        switch (pAction->fwd_data.fwd_type)
        {
            case ACL_ACTION_FWD_PERMIT:
                value = 0;
                break;
            case ACL_ACTION_FWD_DROP:
                value = 1;
                break;
            case ACL_ACTION_FWD_COPY_TO_PORTID:
                value = 2;

                info = pAction->fwd_data.fwd_info & 0x3F;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                    info |= (1 << 10);
                else
                {
                    if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_LOCAL_DEV)
                    {
                        fwd_devID = HAL_UNIT_TO_DEV_ID(HWP_MY_UNIT_ID());
                    }
                    else
                    {
                        fwd_devID = pAction->fwd_data.devID;
                    }

                    info |= ((fwd_devID & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                value = 3;

                if ((ret = table_size_get(unit, LONGAN_MC_PORTMASKt,
                        &multicast_tableSize)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                if (pAction->fwd_data.fwd_info >= multicast_tableSize)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                info = pAction->fwd_data.fwd_info;

                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                value = 4;

                info = pAction->fwd_data.fwd_info & 0x3F;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                    info |= (1 << 10);
                else
                {
                    if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_LOCAL_DEV)
                    {
                        fwd_devID = HAL_UNIT_TO_DEV_ID(HWP_MY_UNIT_ID());
                    }
                    else
                    {
                        fwd_devID = pAction->fwd_data.devID;
                    }
                    info |= ((fwd_devID & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                value = 5;

                if ((ret = table_size_get(unit, LONGAN_MC_PORTMASKt,
                        &multicast_tableSize)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                if (pAction->fwd_data.fwd_info >= multicast_tableSize)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }
                info = pAction->fwd_data.fwd_info;

                break;
            case ACL_ACTION_FWD_FILTERING:
                value = 6;
                info = pAction->fwd_data.fwd_info;
                break;
            case ACL_ACTION_FWD_LOOPBACK:
                value = 7;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_FWD_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_FWD_PORT_INFOtf,\
            &info, (uint32 *) &entry), errHandle, ret);

        switch (pAction->fwd_data.fwd_cpu_fmt)
        {
            case ORIGINAL_PACKET:
                value = 0;
                break;
            case MODIFIED_PACKET:
                value = 1;
                break;
            case PKT_FORMAT_NOP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_FWD_CPU_PKT_FMTtf,\
            &pAction->fwd_data.fwd_cpu_fmt, (uint32 *) &entry), errHandle, ret);

        if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_OVERWRITE_DROP)
            value = 1;
        else
            value = 0;
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_FWD_SELtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_LOGtf,\
        &pAction->stat_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->stat_en)
    {
        if (pAction->stat_data.stat_type == STAT_TYPE_PACKET_BASED_32BIT)
        {
            value = 0;
        }
        else if (pAction->stat_data.stat_type == STAT_TYPE_BYTE_BASED_64BIT)
        {
            value = 1;
        }
        else
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_LOG_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_MIRRORtf,\
        &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->mirror_en)
    {
        if (pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_MIRROR_INDEXtf,\
            &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);

        switch (pAction->mirror_data.mirror_type)
        {
            case ACL_ACTION_MIRROR_ORIGINAL:
                value = 0;
                break;
            case ACL_ACTION_MIRROR_MODIFIED:
                value = 1;
                break;
            case ACL_ACTION_MIRROR_CANCEL:
                value = 2;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_MIRROR_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_METERtf,\
        &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meter_en)
    {
        if (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_METER_INDEXtf,\
            &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_IVLANtf,\
        &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->inner_vlan_assign_en)
    {
        switch (pAction->inner_vlan_data.vid_assign_type)
        {
            case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                value = 0;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                value = 1;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                value = 2;
                break;
            case ACL_ACTION_VLAN_ASSIGN_NOP:
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_VID_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        value = pAction->inner_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_VIDtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (ENABLED == pAction->inner_vlan_data.tpid_assign)
        {
            if (HAL_MAX_NUM_OF_CVLAN_TPID(unit) <= pAction->inner_vlan_data.tpid_idx)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
            }

            RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_TPID_IDXtf,
                    &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);
            value = 1;
        }
        else
        {
            value = 0;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_TPID_ACTtf,\
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_OVLANtf,\
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_vlan_assign_en)
    {
        switch (pAction->outer_vlan_data.vid_assign_type)
        {
            case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                value = 0;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                value = 1;
                break;
            case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                value = 2;
                break;
            case ACL_ACTION_VLAN_ASSIGN_NOP:
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_VID_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        value = pAction->outer_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_VIDtf,\
            &value, (uint32 *) &entry), errHandle, ret);


        if (ENABLED == pAction->outer_vlan_data.tpid_assign)
        {
            if (HAL_MAX_NUM_OF_SVLAN_TPID(unit) <= pAction->outer_vlan_data.tpid_idx)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
            }
            RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_TPID_IDXtf,\
                    &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);
            value = 1;
        }
        else
            value = 0;

        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_TPID_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_IPRItf,\
        &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->inner_pri_en)
    {
        switch (pAction->inner_pri_data.act)
        {
            case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                if (pAction->inner_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->inner_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_PRItf,\
                        &value, (uint32 *) &entry), errHandle, ret);

                value = 0;
                break;
            case ACL_ACTION_INNER_PRI_COPY_FROM_OUTER:
                value = 1;
                break;
            case ACL_ACTION_INNER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "inner_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_IVLAN_PRI_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Outer PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_OPRItf,\
        &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_pri_en)
    {
        switch (pAction->outer_pri_data.act)
        {
            case ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI:
                if (pAction->outer_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->outer_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_PRItf,\
                        &value, (uint32 *) &entry), errHandle, ret);
                value = 0;
                break;
            case ACL_ACTION_OUTER_PRI_COPY_FROM_INNER:
                value = 1;
                break;
            case ACL_ACTION_OUTER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_OVLAN_PRI_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_TAG_STATUStf,\
        &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->tag_sts_en)
    {
        switch (pAction->tag_sts_data.itag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                value = 0;
                break;
            case ACL_ACTION_TAG_STS_TAG:
                value = 1;
                break;
            case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                value = 2;
                break;
            case ACL_ACTION_TAG_STS_NOP:
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.itag_sts error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_TAG_STATUS_INNERtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        switch (pAction->tag_sts_data.otag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                value = 0;
                break;
            case ACL_ACTION_TAG_STS_TAG:
                value = 1;
                break;
            case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                value = 2;
                break;
            case ACL_ACTION_TAG_STS_NOP:
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "tag_sts_data.otag_sts error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_TAG_STATUS_OUTERtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Priority action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_PRIORITYtf,\
        &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->pri_en)
    {
        if (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_PRI_DATAtf,\
            &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);
    }

    /* Bypass action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_BYPASStf,\
        &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->bypass_en)
    {
        value = (pAction->bypass_data.ibc_sc & 0x1);
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_BYPASS_DATAtf,\
            &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meta data action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_METADATAtf,\
        &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meta_data_en)
    {
        if (HAL_MAX_NUM_OF_METADATA(unit) <= pAction->meta_data.data)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_META_DATAtf,\
            &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);
    }

    /* QID action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_CPUQIDtf,\
        &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->cpu_qid_en)
    {
        if (HAL_MAX_NUM_OF_CPU_QUEUE(unit) <= pAction->cpu_qid.qid)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_QID_DATAtf,\
            &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);
    }

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_REMARKtf,\
        &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->rmk_en)
    {
        switch (pAction->rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_DSCP:
                if (pAction->rmk_data.rmk_info > RTK_VALUE_OF_DSCP_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                if (pAction->rmk_data.rmk_info > RTK_VALUE_OF_IP_PRECEDENCE_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            case ACL_ACTION_REMARK_TOS:
                if (pAction->rmk_data.rmk_info > RTK_VALUE_OF_TOS_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 2;
                break;
            case ACL_ACTION_REMARK_EAV:
                if (pAction->rmk_data.rmk_info >= 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                break;
                //return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_REMARK_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_REMARK_VALUEtf,\
            &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }

    /* Yellow Remark action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_YELLOW_REMARKtf,\
        &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->yellow_rmk_en)
    {
        switch (pAction->yellow_rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_DSCP:
                if (pAction->yellow_rmk_data.rmk_info > RTK_VALUE_OF_DSCP_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                if (pAction->yellow_rmk_data.rmk_info > RTK_VALUE_OF_IP_PRECEDENCE_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            case ACL_ACTION_REMARK_TOS:
                if (pAction->yellow_rmk_data.rmk_info > RTK_VALUE_OF_TOS_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 2;
                break;
            case ACL_ACTION_REMARK_EAV:
                if (pAction->yellow_rmk_data.rmk_info >= 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_YELLOW_REMARK_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_YELLOW_REMARK_VALUEtf,\
            &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }

    /*Red Remark action */
    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_RED_REMARKtf,\
        &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->red_rmk_en)
    {
        switch (pAction->red_rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_DSCP:
                if (pAction->red_rmk_data.rmk_info > RTK_VALUE_OF_DSCP_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                if (pAction->red_rmk_data.rmk_info > RTK_VALUE_OF_IP_PRECEDENCE_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            case ACL_ACTION_REMARK_TOS:
                if (pAction->red_rmk_data.rmk_info > RTK_VALUE_OF_TOS_MAX)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 2;
                break;
            case ACL_ACTION_REMARK_EAV:
                if (pAction->red_rmk_data.rmk_info >= 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_RED_REMARK_ACTtf,\
            &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_RED_REMARK_VALUEtf,\
            &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }

    RT_ERR_HDL(table_field_set(unit, LONGAN_IACLt, LONGAN_IACL_ACT_MSK_INVERT_IP_RSVD_FLAGtf,\
        &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, LONGAN_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
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

/* Function Name:
 *      dal_longan_acl_ruleAction_set
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
dal_longan_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    int32   ret = RT_ERR_FAILED;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    if (phase == ACL_PHASE_VACL)
    {
        if ((ret = _dal_longan_acl_vRuleAction_set(unit, entry_idx, &pAction->vact)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _dal_longan_acl_iRuleAction_set(unit, entry_idx, &pAction->iact)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_longan_acl_fieldType_chk(uint32 unit, rtk_acl_phase_t phase, rtk_acl_fieldType_t type)
{
    uint32  i;
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);
    if(ACL_PHASE_VACL == phase)
    {
        for (i = 0; dal_longan_acl_vFixField_list[i].type != USER_FIELD_END; ++i)
        {
            if (type == dal_longan_acl_vFixField_list[i].type)
                break;
        }
        if (dal_longan_acl_vFixField_list[i].type != USER_FIELD_END)
            return RT_ERR_OK;
    }
    else
    {
        for (i = 0; dal_longan_acl_iFixField_list[i].type != USER_FIELD_END; ++i)
        {
            if (type == dal_longan_acl_iFixField_list[i].type)
                break;
        }
        if (dal_longan_acl_iFixField_list[i].type != USER_FIELD_END)
            return RT_ERR_OK;
    }
    for (i = 0; dal_longan_acl_field_list[i].type != USER_FIELD_END; i++)
    {
        if (type == dal_longan_acl_field_list[i].type)
            break;
    }
    if (dal_longan_acl_field_list[i].type == USER_FIELD_END)
        return RT_ERR_ACL_FIELD_TYPE;
    if(ACL_PHASE_VACL == phase)
    {
        for (i = 0; iacl_only_field_type[i] != USER_FIELD_END; ++i)
        {
            if (type == iacl_only_field_type[i])
                break;
        }
        if (iacl_only_field_type[i] != USER_FIELD_END)
            return RT_ERR_ACL_FIELD_TYPE;
    }
    else
    {
        for (i = 0; vacl_only_field_type[i] != USER_FIELD_END; ++i)
        {
            if (type == vacl_only_field_type[i])
                break;
        }
        if (vacl_only_field_type[i] != USER_FIELD_END)
            return RT_ERR_ACL_FIELD_TYPE;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_loopBackEnable_set
 * Description:
 *      Set loopback state.
 * Input:
 *      unit    - unit id
 *      enable  - loopback state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_longan_acl_loopBackEnable_get(uint32 unit, uint32 *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    ACL_SEM_LOCK(unit);
    if((ret = reg_field_read(unit,LONGAN_ACL_CTRLr,LONGAN_LB_ENf,pEnable) != RT_ERR_OK))
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_loopBackEnable_get */

/* Function Name:
 *      dal_longan_acl_loopBackEnable_set
 * Description:
 *      Set loopback state.
 * Input:
 *      unit    - unit id
 *      enable  - loopback state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_longan_acl_loopBackEnable_set(uint32 unit, uint32 enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    ACL_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit,LONGAN_ACL_CTRLr,LONGAN_LB_ENf,&enable) != RT_ERR_OK))
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_loopBackEnable_set */

/* Function Name:
 *      dal_longan_acl_limitLoopbackTimes_get
 * Description:
 *      Get the loopback maximum times.
 * Input:
 *      unit    - unit id
 *      pLb_times  - pointer to loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_longan_acl_limitLoopbackTimes_get(uint32 unit, uint32 *pLb_times)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLb_times), RT_ERR_NULL_POINTER);

    /* function body */
    ACL_SEM_LOCK(unit);
    if((ret = reg_field_read(unit,LONGAN_ACL_CTRLr,LONGAN_LB_LIMITf,pLb_times) != RT_ERR_OK))
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_limitLoopbackTimes_get */


/* Function Name:
 *      dal_longan_acl_limitLoopbackTimes_set
 * Description:
 *      Set the loopback maximum times.
 * Input:
 *      unit    - unit id
 *      lb_times  - loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_longan_acl_limitLoopbackTimes_set(uint32 unit, uint32 lb_times)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,lb_times=%d", unit, lb_times);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((8 <= lb_times), RT_ERR_INPUT);

    /* function body */
    ACL_SEM_LOCK(unit);
    if((ret = reg_field_write(unit,LONGAN_ACL_CTRLr,LONGAN_LB_LIMITf,&lb_times) != RT_ERR_OK))
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_limitLoopbackTimes_set */

/* Function Name:
 *      dal_longan_acl_portPhaseLookupEnable_get
 * Description:
 *      Get the acl phase lookup state of the specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phase   - ACL lookup phase
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
dal_longan_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        LONGAN_ACL_PORT_LOOKUP_CTRLr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        LONGAN_LOOKUP_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_longan_acl_portPhaseLookupEnable_get */

/* Function Name:
 *      dal_longan_acl_portPhaseLookupEnable_set
 * Description:
 *      Set the acl phase lookup state of the specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phase   - ACL lookup phase
 *      enable  - lookup state
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
dal_longan_acl_portPhaseLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        LONGAN_ACL_PORT_LOOKUP_CTRLr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        LONGAN_LOOKUP_ENf,
                        &enable)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_longan_acl_portPhaseLookupEnable_set */

/* Function Name:
 *      dal_longan_acl_templateSelector_get
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
dal_longan_acl_templateSelector_get(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   *pTemplate_idx)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d", unit, block_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((NULL == pTemplate_idx), RT_ERR_NULL_POINTER);

    /* get value from SW database */
    if ((ret = dal_longan_pie_templateSelector_get(unit, block_idx,\
                    &pTemplate_idx->template_id[0],
                    &pTemplate_idx->template_id[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "template1_idx=%d, template2_idx=%d",\
        pTemplate_idx->template_id[0], pTemplate_idx->template_id[1]);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_templateSelector_get */

/* Function Name:
 *      dal_longan_acl_templateSelector_set
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
 *      RT_ERR_PIE_TEMPLATE_INCOMPATIBLE    - try to map a PIE block to an incompatible template
 *      RT_ERR_INPUT                        - invalid input parameter
 * Note:
 *      block_idx 0-17, template_idx 0-7
 */
int32
dal_longan_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx)
{
    int32   ret = RT_ERR_FAILED;
    uint32  i;

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

    /* set value to CHIP */
    if ((ret = dal_longan_pie_templateSelector_set(unit, block_idx,\
                    template_idx.template_id[0],
                    template_idx.template_id[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_longan_acl_templateSelector_set */

/* Function Name:
 *      dal_longan_acl_ruleHitIndication_get
 * Description:
 *      Get the PIE rule hit indication.
 * Input:
 *      unit        - unit id
 *      phase       - PIE lookup phase
 *      entry_idx   - PIE entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PIE_PHASE    - invalid PIE phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_longan_acl_ruleHitIndication_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_pie_id_t entry_idx, uint32 reset, uint32 *pIsHit)
{
    int32   ret = RT_ERR_FAILED;
    uint32  grpIdx, offset, grpData;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_PIE_PHASE;

    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &entry_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    grpIdx = entry_idx / 32;
    offset = entry_idx % 32;

    ACL_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit,
                          LONGAN_PIE_RULE_HIT_INDICATIONr,
                          REG_ARRAY_INDEX_NONE,
                          grpIdx,
                          LONGAN_RULE_INDICATIONf,
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
                              LONGAN_PIE_RULE_HIT_INDICATIONr,
                              REG_ARRAY_INDEX_NONE,
                              grpIdx,
                              LONGAN_RULE_INDICATIONf,
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
}   /* end of dal_longan_acl_ruleHitIndication_get */

static int32 _dal_longan_acl_rule_del(uint32 unit, rtk_acl_clear_t *pClrIdx)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value = 0, field_data;

    if ((ret = reg_field_set(unit, LONGAN_PIE_CLR_CTRLr, LONGAN_CLR_FROMf, &pClrIdx->start_idx, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, LONGAN_PIE_CLR_CTRLr, LONGAN_CLR_TOf, &pClrIdx->end_idx, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, LONGAN_PIE_CLR_CTRLr, LONGAN_CLRf, &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        //osal_free(org_block_lookup_state);
        return ret;
    }

    ACL_SEM_LOCK(unit);

    if ((ret = reg_write(unit, LONGAN_PIE_CLR_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        //osal_free(org_block_lookup_state);
        return ret;
    }

    /* wait until clear action is completed */
    do {
        reg_field_read(unit, LONGAN_PIE_CLR_CTRLr, LONGAN_CLRf, &value);
        if (value == 0)
            break;
    } while(1);

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}   /* end of _dal_longan_acl_rule_del */

/* Function Name:
 *      dal_longan_acl_rule_del
 * Description:
 *      Delete the specified PIE rules.
 * Input:
 *      unit    - unit id
 *      phase   - PIE lookup phase
 *      pClrIdx - rule index to clear
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PIE_PHASE       - invalid PIE phase
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_PIE_CLEAR_INDEX - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      Entry fields, operations and actions are all cleared.
 */
int32
dal_longan_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx)
{
    rtk_acl_clear_t phyClr;
    uint32          idx;
    int32           ret, i, hit;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_PIE_PHASE;

    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_PIE_CLEAR_INDEX);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, start_idx=%d, end_idx=%d",\
            unit, pClrIdx->start_idx, pClrIdx->end_idx);

    /*check entry phase correct before del entry */
    /* start */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, pClrIdx->start_idx, &phyClr.start_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    /* end */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, pClrIdx->end_idx, &phyClr.end_idx)) != RT_ERR_OK)
    {
        return ret;
    }

    if (PIE_ENTRY_IS_PHYSICAL_TYPE(pClrIdx->start_idx))
    {
        if ((ret = _dal_longan_acl_rule_del(unit, &phyClr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }

        for (i = phyClr.start_idx; i <= phyClr.end_idx; ++i)
        {
            if ((ret = dal_longan_acl_ruleHitIndication_get(unit, phase,i, TRUE, (uint32 *)&hit)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                return ret;
            }
        }
        return RT_ERR_OK;
    }

    /* per physical block del entry */
    i = (pClrIdx->start_idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
            HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    idx = pClrIdx->start_idx;

    do
    {
        /* start */
        if ((ret = _dal_longan_acl_index_to_physical(unit, phase, idx, &phyClr.start_idx)) != RT_ERR_OK)
        {
            return ret;
        }


        /* end */
        i += HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

        if (i > pClrIdx->end_idx)
            idx = pClrIdx->end_idx;
        else
            idx = i - 1;

        if ((ret = _dal_longan_acl_index_to_physical(unit, phase, idx, &phyClr.end_idx)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = _dal_longan_acl_rule_del(unit, &phyClr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        idx = i;
    } while (i < pClrIdx->end_idx);

    /* clear hit indication */
    for (i = pClrIdx->start_idx; i <= pClrIdx->end_idx; ++i)
    {
        if ((ret = dal_longan_acl_ruleHitIndication_get(unit, phase, i, TRUE, (uint32 *)&hit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_longan_acl_rule_del */

static int32 _dal_longan_acl_rule_move(uint32 unit, rtk_acl_move_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value = 0, field_data;

    if ((ret = reg_field_set(unit, LONGAN_PIE_MV_CTRLr, LONGAN_MV_FROMf, &pData->move_from, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, LONGAN_PIE_MV_CTRLr, LONGAN_MV_TOf, &pData->move_to, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, LONGAN_PIE_MV_LEN_CTRLr, LONGAN_MV_LENf, &pData->length)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    field_data = 1;
    if ((ret = reg_field_set(unit, LONGAN_PIE_MV_CTRLr, LONGAN_MVf, &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        //osal_free(org_block_lookup_state);
        return ret;
    }

    ACL_SEM_LOCK(unit);

    if ((ret = reg_write(unit, LONGAN_PIE_MV_CTRLr, &value)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        //osal_free(org_block_lookup_state);
        return ret;
    }

    /* wait until move action is completed */
    do {
        reg_field_read(unit, LONGAN_PIE_MV_CTRLr, LONGAN_MVf, &value);
        if (value == 0)
            break;
    } while(1);

    ACL_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_rule_move
 * Description:
 *      Move the specified PIE rules.
 * Input:
 *      unit    - unit id
 *      phase   - PIE lookup phase
 *      pData   - movement info
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PIE_PHASE    - invalid PIE phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *   case A) from 248 to 100 length 80
 *      1) src 248 ~ 255, dst 100 ~ 107
 *      2) src 256 ~ 327
 *          2.1) src 256 ~ 275, dst 108 ~ 127
 *          2.2) src 276 ~ 327, dst 128 ~ 179
 *
 *        0        128      256      384
 *        +--------+--------+--------+------
 *    src                 |1|  2  |
 *    dst    |1|2.1|2.2|
 *        +--------+--------+--------+------
 *        0        128      256      384
 *   ===========================================
 *   case B) from 200 to 125 length 10
 *      1) src 200 ~ 210
 *          1.1) src 200 ~ 202, dst 125 ~ 127
 *          1.2) src 203 ~ 209, dst 128 ~ 134
 *
 *        0        128      256      384
 *        +--------+--------+--------+------
 *    src                |1|
 *    dst      |1.1|1.2|
 *        +--------+--------+--------+------
 *        0        128      256      384
 *   ===========================================
 *   case C) from 100 to 248 length 80
 *      1) src 128 ~ 179, dst 276 ~ 327
 *      2) src 100 ~ 127
 *          2.1) src 108 ~ 127, dst 256 ~ 275
 *          2.2) src 100 ~ 107, dst 248 ~ 255
 *
 *        0        128      256      384
 *        +--------+--------+--------+------
 *    src      | 2 |1|
 *    dst               |2.2|2.1|1|
 *        +--------+--------+--------+------
 *        0        128      256      384
 *   ===========================================
 *   case D) from 118 to 239 length 20
 *      1) src 128 ~ 137
 *          1.1) src 135 ~ 137, dst 256 ~ 258
 *          1.2) src 128 ~ 134, dst 249 ~ 255
 *      2) src 118 ~ 127, dst 239 ~ 248
 *
 *        0        128      256      384
 *        +--------+--------+--------+------
 *    src      | 2 |1|
 *    dst                 |2|1.2|1.1|
 *        +--------+--------+--------+------
 *        0        128      256      384
 *   ===========================================
 */
int32
dal_longan_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData)
{
    rtk_acl_move_t  phyMov;
    uint32         validPhyEntry;
    int32           srcLen, srcBlkStart, srcBlkEnd,srcBlkLen, srcMvLen;
    int32           dstBlkStart, dstBlkEnd,dstBlkLen;
    uint32         phy_length;
    int32           ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_PIE_PHASE;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,move_from=%d,move_to=%d,length=%d",
            unit, phase, pData->move_from, pData->move_to, pData->length);

    osal_memset(&phyMov, 0, sizeof(rtk_acl_move_t));
    phyMov.length = pData->length;
    /*check entry phase correct before move entry */
    /* from */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, pData->move_from, &phyMov.move_from)) != RT_ERR_OK)
    {
        return ret;
    }
    /* from + length*/
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, pData->move_from + pData->length - 1, &validPhyEntry)) != RT_ERR_OK)
    {
        return ret;
    }
    /* to */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, pData->move_to, &phyMov.move_to)) != RT_ERR_OK)
    {
        return ret;
    }

    /* to + length*/
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, pData->move_to + pData->length - 1, &validPhyEntry)) != RT_ERR_OK)
    {
        return ret;
    }

    if (PIE_ENTRY_IS_PHYSICAL_TYPE(pData->move_from))
    {
        if ((ret = _dal_longan_acl_rule_move(unit, &phyMov)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
        return RT_ERR_OK;
    }

    /* per source physical block move to each destination physical block */
    if (pData->move_from > pData->move_to)
    {
        srcBlkStart = pData->move_from;
        srcLen = pData->length;
        dstBlkStart = pData->move_to;

        do
        {
            srcBlkEnd = (srcBlkStart / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                    HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) +
                    HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) - 1;

            srcBlkLen = srcBlkEnd - srcBlkStart + 1;
            if (srcBlkLen > srcLen)
                srcMvLen = srcLen;
            else
                srcMvLen = srcBlkLen;

            do {
                dstBlkEnd = (dstBlkStart / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                        HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) +
                        HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) - 1;
                dstBlkLen = dstBlkEnd - dstBlkStart + 1;

                if (dstBlkLen > srcMvLen)
                    phy_length = srcMvLen;
                else
                    phy_length = dstBlkLen;

                phyMov.length = phy_length;

                if ((ret = _dal_longan_acl_index_to_physical(unit, phase, srcBlkStart, &phyMov.move_from)) != RT_ERR_OK)
                {
                    return ret;
                }
                if ((ret = _dal_longan_acl_index_to_physical(unit, phase, dstBlkStart, &phyMov.move_to)) != RT_ERR_OK)
                {
                    return ret;
                }

                if ((ret = _dal_longan_acl_rule_move(unit, &phyMov)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                dstBlkStart += phy_length;
                srcBlkStart += phy_length;
                srcMvLen -= phy_length;
                srcLen -= phy_length;
            } while (srcMvLen > 0);
        } while (srcLen > 0);
    }
    else if (pData->move_from < pData->move_to)
    {
        srcBlkEnd = pData->move_from + pData->length - 1;
        srcLen = pData->length;
        dstBlkEnd = pData->move_to + pData->length - 1;

        do
        {
            srcBlkStart = (srcBlkEnd / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                    HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

            if (srcBlkStart < pData->move_from)
                srcBlkStart = pData->move_from;

            srcBlkLen = srcBlkEnd - srcBlkStart + 1;
            if (srcBlkLen > srcLen)
                srcMvLen = srcLen;
            else
                srcMvLen = srcBlkLen;

            do {
                dstBlkStart = (dstBlkEnd / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                        HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
                if (dstBlkStart < pData->move_to)
                    dstBlkStart = pData->move_to;

                dstBlkLen = dstBlkEnd - dstBlkStart + 1;

                if (dstBlkLen > srcMvLen)
                {
                    phy_length = srcMvLen;
                }
                else
                {
                    phy_length = dstBlkLen;
                }

                srcBlkStart = srcBlkEnd - phy_length + 1;
                dstBlkStart = dstBlkEnd - phy_length + 1;
                phyMov.length = phy_length;

                if ((ret = _dal_longan_acl_index_to_physical(unit, phase, srcBlkStart, &phyMov.move_from)) != RT_ERR_OK)
                {
                    return ret;
                }
                if ((ret = _dal_longan_acl_index_to_physical(unit, phase, dstBlkStart, &phyMov.move_to)) != RT_ERR_OK)
                {
                    return ret;
                }

                if ((ret = _dal_longan_acl_rule_move(unit, &phyMov)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                    return ret;
                }

                dstBlkEnd -= phy_length;
                srcBlkEnd -= phy_length;
                srcMvLen -= phy_length;
                srcLen -= phy_length;
            } while (srcMvLen > 0);
        } while (srcLen > 0);
    }

    return RT_ERR_OK;
}   /* end of dal_longan_acl_rule_move */

/* Function Name:
 *      dal_longan_acl_statCnt_get
 * Description:
 *      Get packet-based or byte-based statistic counter of the log id.
 * Input:
 *      unit        - unit id
 *      phase       - PIE lookup phase
 *      entryidx    - logic entry index
 *      mode        - statistic counter mode
 * Output:
 *      pCnt - pointer buffer of count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_acl_statCnt_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_pie_id_t entryIdx, rtk_acl_statMode_t mode, uint64 *pCnt)
{
    log_entry_t entry;
    int32       ret;
    uint32      val[2];

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((mode >= STAT_MODE_END), RT_ERR_TYPE);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entryIdx, &entryIdx)) != RT_ERR_OK)
    {
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_LOGt, entryIdx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    if ((ret = table_field_get(unit, LONGAN_LOGt, LONGAN_LOG_COUNTERtf, val,
            (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    *pCnt = val[0] | (uint64)val[1] << 32;

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pCnt=%d", *pCnt);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_statCnt_get */

/* Function Name:
 *      dal_longan_acl_statCnt_clear
 * Description:
 *      Clear statistic counter of the log id.
 * Input:
 *      unit    - unit id
 *      phase       - PIE lookup phase
 *      entryidx    - logic entry index
 *      mode        - statistic counter mode
 * Output:
 *      None
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
dal_longan_acl_statCnt_clear(uint32 unit, rtk_acl_phase_t phase,
    rtk_pie_id_t entryIdx, rtk_acl_statMode_t mode)
{
    log_entry_t entry;
    int32       ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((mode >= STAT_MODE_END), RT_ERR_TYPE);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;

    /* translate to physical index */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entryIdx, &entryIdx)) != RT_ERR_OK)
    {
        return ret;
    }

    osal_memset(&entry, 0, sizeof(log_entry_t));

    /* Log (Write = Clear) */
    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, LONGAN_LOGt, entryIdx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_acl_statCnt_clear */

/* Function Name:
 *      dal_longan_acl_ruleEntryField_validate
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
dal_longan_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type)
{
    dal_longan_phaseInfo_t  phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    int32                   ret;
    uint8                   data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8                   mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,entry_idx=%d,type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL)
        return RT_ERR_ACL_PHASE;
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

    /* function body */
    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &phyEntryId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_read(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_longan_acl_ruleEntryBufField_get(unit, phase,
            phyEntryId, type, (uint32 *)&entry, data, mask)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_longan_acl_ruleEntryField_validate */

/* Function Name:
 *      dal_longan_acl_fieldUsr2Template_get
 * Description:
 *      Get template field ID from user field ID.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      type        - user field ID
 * Output:
 *      info        - template field ID list
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
int32
dal_longan_acl_fieldUsr2Template_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, rtk_acl_fieldUsr2Template_t *info)
{
    dal_longan_phaseInfo_t         phaseInfo;
    dal_longan_acl_entryField_t    *fieldList = NULL;
    dal_longan_acl_fieldLocation_t *fieldLocation;
    uint32                         dalFieldType;
    uint32                         field_num;
    uint32                         field_idx;
    int32                          ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, type=%d", unit, phase, type);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_INPUT);
    RT_PARAM_CHK((USER_FIELD_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(info, 0, sizeof(rtk_acl_fieldUsr2Template_t));

    /* translate field from RTK superset view to DAL view */

    /* fixed field */
    if ((ret = _dal_longan_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    for (field_idx = 0; phaseInfo.fixFieldList[field_idx].type != USER_FIELD_END; ++field_idx)
    {
        if (type == phaseInfo.fixFieldList[field_idx].type)
        {
            info->fields[0] = TMPLTE_FIELD_FIX;
            return RT_ERR_OK;
        }
    }

    /* configurable field */
    if ((ret = _dal_longan_acl_fieldInfo_get(phase, type, &fieldList,
            &dalFieldType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    field_num = fieldList[dalFieldType].field_number;
    if (field_num >= RTK_ACL_USR2TMPLTE_MAX)
        return RT_ERR_FAILED;

    for (field_idx = 0; field_idx < field_num; ++field_idx)
    {
        fieldLocation = &fieldList[dalFieldType].pField[field_idx];
        info->fields[field_idx] = fieldLocation->template_field_type;
    }

    return RT_ERR_OK;
}   /* end of dal_longan_acl_fieldUsr2Template_get */

/* Function Name:
 *      dal_longan_acl_templateId_get
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
dal_longan_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id)
{
    int32     ret;
    uint32    phy_entry_idx;
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
    if ((ret = dal_longan_acl_ruleEntryField_read(unit, phase, entry_idx, \
            USER_FIELD_TEMPLATE_ID, field_data, field_mask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    /* caculate entry in which block */
    if ((ret = _dal_longan_acl_index_to_physical(unit, phase, entry_idx, &phy_entry_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    block_idx = phy_entry_idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    osal_memset(&template_idx, 0, sizeof(rtk_acl_templateIdx_t));
    if ((ret = dal_longan_acl_templateSelector_get(unit, block_idx, &template_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    *pTemplate_id = template_idx.template_id[field_data[0]];

    return RT_ERR_OK;
}   /* end of dal_longan_acl_templateId_get */

/* Function Name:
 *      _dal_longan_acl_init_config
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
_dal_longan_acl_init_config(uint32 unit)
{
    return RT_ERR_OK;
} /* end of _dal_longan_acl_init_config */

/* Function Name:
 *      dal_longan_aclMapper_init
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
dal_longan_aclMapper_init(dal_mapper_t *pMapper)
{
    pMapper->acl_init = dal_longan_acl_init;
    pMapper->acl_ruleEntryFieldSize_get = dal_longan_acl_ruleEntryFieldSize_get;
    pMapper->acl_ruleEntrySize_get = dal_longan_acl_ruleEntrySize_get;
    pMapper->acl_ruleValidate_get = dal_longan_acl_ruleValidate_get;
    pMapper->acl_ruleValidate_set = dal_longan_acl_ruleValidate_set;
    pMapper->acl_ruleEntry_read = dal_longan_acl_ruleEntry_read;
    pMapper->acl_ruleEntry_write = dal_longan_acl_ruleEntry_write;
    pMapper->acl_ruleEntryField_get = dal_longan_acl_ruleEntryField_get;
    pMapper->acl_ruleEntryField_set = dal_longan_acl_ruleEntryField_set;
    pMapper->acl_ruleEntryField_read = dal_longan_acl_ruleEntryField_read;
    pMapper->acl_ruleEntryField_write = dal_longan_acl_ruleEntryField_write;
    pMapper->acl_ruleEntryField_check = dal_longan_acl_ruleEntryField_check;
    pMapper->acl_ruleOperation_get = dal_longan_acl_ruleOperation_get;
    pMapper->acl_ruleOperation_set = dal_longan_acl_ruleOperation_set;
    pMapper->acl_ruleAction_get = dal_longan_acl_ruleAction_get;
    pMapper->acl_ruleAction_set = dal_longan_acl_ruleAction_set;
    pMapper->acl_loopBackEnable_get = dal_longan_acl_loopBackEnable_get;
    pMapper->acl_loopBackEnable_set = dal_longan_acl_loopBackEnable_set;
    pMapper->acl_limitLoopbackTimes_get = dal_longan_acl_limitLoopbackTimes_get;
    pMapper->acl_limitLoopbackTimes_set = dal_longan_acl_limitLoopbackTimes_set;
    pMapper->acl_portPhaseLookupEnable_get = dal_longan_acl_portPhaseLookupEnable_get;
    pMapper->acl_portPhaseLookupEnable_set = dal_longan_acl_portPhaseLookupEnable_set;
    pMapper->acl_templateSelector_get = dal_longan_acl_templateSelector_get;
    pMapper->acl_templateSelector_set = dal_longan_acl_templateSelector_set;
    pMapper->acl_statCnt_get = dal_longan_acl_statCnt_get;
    pMapper->acl_statCnt_clear = dal_longan_acl_statCnt_clear;
    pMapper->acl_ruleHitIndication_get = dal_longan_acl_ruleHitIndication_get;
    pMapper->acl_rule_del = dal_longan_acl_rule_del;
    pMapper->acl_rule_move = dal_longan_acl_rule_move;
    pMapper->acl_ruleEntryField_validate = dal_longan_acl_ruleEntryField_validate;
    pMapper->acl_fieldUsr2Template_get = dal_longan_acl_fieldUsr2Template_get;
    pMapper->acl_templateId_get = dal_longan_acl_templateId_get;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_acl_init
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
dal_longan_acl_init(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;

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

    if ((ret = _dal_longan_acl_init_config(unit)) != RT_ERR_OK)
    {
        acl_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_longan_acl_init */


