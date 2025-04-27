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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_acl.h>
#include <dal/mango/dal_mango_pie.h>
#include <dal/mango/dal_mango_stack.h>
#include <rtk/default.h>
#include <rtk/acl.h>
#include <rtk/pie.h>

/*
 * Symbol Definition
 */
#define DAL_MANGO_BYTE_OF_FIXED_FIELD           2
#define DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD     14
#define DAL_MANGO_BYTE_OF_PER_TEMPLATE_FIELD    2
#define DAL_MANGO_BYTE_OF_TEMPLATE_FIELD        (DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD * DAL_MANGO_BYTE_OF_PER_TEMPLATE_FIELD)
#define DAL_MANGO_MAX_HITINDICATION_BLK_ENTRY   32

#define DAL_MANGO_LOOPBACK_PORT                 58
#define DAL_MANGO_ACL_CNTR_OPER_MAX             20

#define DAL_MANGO_ACL_GLB_HIT_GRP_NUM           4
#define DAL_MANGO_ACL_GLB_HIT_ENTRY_PER_BIT     32

#define DAL_MANGO_PHASE_ACL_TO_PIE(_aclPhase, _piePhase)                    \
do {                                                                        \
    switch (_aclPhase)                                                      \
    {                                                                       \
        case ACL_PHASE_VACL:                                             \
            _piePhase = PIE_PHASE_VACL;                                  \
            break;                                                          \
        case ACL_PHASE_IACL:                                             \
            _piePhase = PIE_PHASE_IACL;                                  \
            break;                                                          \
        case ACL_PHASE_EACL:                                            \
            _piePhase = PIE_PHASE_EACL;                                 \
            break;                                                          \
        default:                                                            \
            return RT_ERR_ACL_PHASE;                                        \
    }                                                                       \
} while(0)

#define DAL_MANGO_PHASE_PIE_TO_ACL(_piePhase, _aclPhase)                    \
do {                                                                        \
    switch (_piePhase)                                                      \
    {                                                                       \
        case PIE_PHASE_VACL:                                             \
            _aclPhase = ACL_PHASE_VACL;                                  \
            break;                                                          \
        case PIE_PHASE_IACL:                                             \
            _aclPhase = ACL_PHASE_IACL;                                  \
            break;                                                          \
        case PIE_PHASE_EACL:                                            \
            _aclPhase = ACL_PHASE_EACL;                                 \
            break;                                                          \
        default:                                                            \
            return RT_ERR_PIE_PHASE;                                        \
    }                                                                       \
} while(0)

#define DAL_MANGO_ACL_INDEX_TO_PHYSICAL(_unit, _phase, _logicIdx, _phyIdx)  \
do {                                                                        \
    rtk_pie_phase_t _pphase;                                                \
    int32           _ret;                                                   \
    if (PIE_ENTRY_IS_PHYSICAL_TYPE(_logicIdx))                              \
    {                                                                       \
        _phyIdx = PIE_ENTRY_PHYSICAL_CLEAR(_logicIdx);                      \
        RT_PARAM_CHK((_phyIdx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(_unit)), RT_ERR_ENTRY_INDEX);\
    }                                                                       \
    else                                                                    \
    {                                                                       \
        RT_PARAM_CHK((_logicIdx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(_unit)), RT_ERR_ENTRY_INDEX);\
        switch (_phase)                                                     \
        {                                                                   \
            case ACL_PHASE_VACL:                                             \
                _pphase = PIE_PHASE_VACL;                                    \
                break;                                                          \
            case ACL_PHASE_IACL:                                             \
                _pphase = PIE_PHASE_IACL;                                    \
                break;                                                          \
            case ACL_PHASE_EACL:                                            \
                _pphase = PIE_PHASE_EACL;                                   \
                break;                                                          \
            default:                                                            \
                return RT_ERR_ACL_PHASE;                                        \
        }                                                                   \
        RT_ERR_CHK(_dal_mango_pie_entryIdx_trans(_unit, _pphase, _logicIdx, &_phyIdx), _ret);\
    }                                                                       \
} while(0)

#define DAL_MANGO_ACL_ENTRY_HIT_GRP_NUM(_unit)   \
        (HAL_MAX_NUM_OF_PIE_FILTER_ID(_unit) / 32)


#define ACL_BLK_BIT_OFST    7   // 128 entries per block
#define ACL_BLK_IDX_MSK     (0x1F << ACL_BLK_BIT_OFST)   // 32 blocks
#define ACL_ENTRY_IDX_MSK   ((1 << ACL_BLK_BIT_OFST) - 1)

#define ACL_MV_DBG_FLAG     0
#define ACL_DEL_DBG_FLAG    0

#define ACL_MV_DBG(_fmt, args...) \
do { \
    if (ACL_MV_DBG_FLAG) {osal_printf(_fmt, args);} \
    else {RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), _fmt, args);} \
} while(0)

#define ACL_DEL_DBG(_fmt, args...) \
do { \
    if (ACL_DEL_DBG_FLAG) {osal_printf(_fmt, args);} \
    else {RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), _fmt, args);} \
} while(0)

#define ACL_ERR_CHK_MSG_ARGS(_op, _ret, _fmt, args...) \
do { \
    if ((_ret = (_op)) != RT_ERR_OK) \
    { \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), _fmt, args); \
        return _ret;\
    } \
} while(0)


/*
 * Data Declaration
 */
static uint32               acl_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         acl_sem[RTK_MAX_NUM_OF_UNIT];
uint32                      *pACL_TBL = NULL;

typedef struct dal_mango_acl_entryTable_s
{
    uint8   data_field[DAL_MANGO_BYTE_OF_TEMPLATE_FIELD];
    uint8   data_fixed[DAL_MANGO_BYTE_OF_FIXED_FIELD];
    uint8   care_field[DAL_MANGO_BYTE_OF_TEMPLATE_FIELD];
    uint8   care_fixed[DAL_MANGO_BYTE_OF_FIXED_FIELD];
} dal_mango_acl_entryTable_t;

typedef struct dal_mango_acl_fieldLocation_s
{
    uint32   template_field_type;
    uint32   field_offset;
    uint32   field_length;
    uint32   data_offset;
} dal_mango_acl_fieldLocation_t;

typedef struct dal_mango_acl_fixField_s
{
    uint32                          data_field;     /* data field in chip view */
    uint32                          mask_field;     /* mask field in chip view */
    uint32                          position;       /* position in fix data */
    rtk_acl_fieldType_t             type;           /* field type in user view */
    dal_mango_acl_fieldLocation_t   *pField;
} dal_mango_acl_fixField_t;

typedef struct dal_mango_acl_entryField_s
{
    rtk_acl_fieldType_t             type;           /* field type in user view */
    uint32                          fieldNumber;   /* locate in how many fields */
    dal_mango_acl_fieldLocation_t   *pField;
} dal_mango_acl_entryField_t;

typedef struct dal_mango_acl_entryFieldInfo_s
{
    rtk_acl_fieldType_t             type;       /* field type in user view */
    uint32                          fieldLen;   /* field length */
} dal_mango_acl_entryFieldInfo_t;

typedef struct dal_mango_phaseInfo_s
{
    dal_mango_acl_fixField_t    *fixFieldList;
    uint32                      table;
    uint32                      *tmplteDataFieldList, *tmplteMaskFieldList;
} dal_mango_phaseInfo_t;

typedef struct dal_mango_aggInfo_s
{
    rtk_enable_t    valid;
    rtk_enable_t    agg2Sts;
    rtk_enable_t    agg1Sts;
    /*
        For AGG2 mode, the baseEntryIdx is AGG2 base entry index.
        For NON-AGG2 mode and AGG1 mode, the baseEntryIdx is AGG1 base entry index.
        For NON-AGG2 mode and NON-AGG1 mode, the baseEntryIdx is current entry index.
    */
    uint32  baseEntryIdx;
    /*
        For AGG2 mode and AGG1 mode of AGG2 base, the agg1PartnerEntryIdx is AGG1 partner entry index of AGG2 base.
        For AGG2 mode and NON-AGG1 mode of AGG2 base, the agg1PartnerEntryIdx is unused (MAX entry id).
        For NON-AGG2 mode and AGG1 mode, the agg1PartnerEntryIdx is AGG1 partner entry index.
        For NON-AGG2 mode and NON-AGG1 mode, the agg1PartnerEntryIdx is unused (MAX entry id).
    */
    uint32  agg1PartnerEntryIdx;
    /*
        If AGG2 mode, the agg2PartnerEntryIdx is AGG2 partner entry index
        else the agg2PartnerEntryIdx is unused (MAX entry id)
    */
    uint32  agg2PartnerEntryIdx;
    /*
        When NON-AGG2 mode or AGG2 mode but AGG2 partner NON-AGG1 mode,
        the agg2PartnerAgg1PartnerEntryIdx is unused (MAX entry id).
    */
    uint32  agg2PartnerAgg1PartnerEntryIdx;
} dal_mango_acl_aggInfo_t;

typedef struct dal_mango_aggMoveInfo_s
{
    rtk_bitmap_t    *pEntryMovedInfo;
    rtk_acl_phase_t phase;
    rtk_enable_t    agg2Sts;
    uint32          srcStartIdx;
    uint32          srcEndIdx;
    uint32          srcIdx;
    uint32          dstIdx;
} dal_mango_acl_aggMoveInfo_t;

typedef struct dal_mango_acl_physicalMove_info_s
{
    uint32  logicSrcBlkEntryOfst;
    uint32  logicDstBlkEntryOfst;
    uint32  logicBlkEntryNum;
    uint32  logicBlkOfst;
    uint32  entryWidth;
    uint32  srcBlkEntryBase;
    uint32  dstBlkEntryBase;
    int32   lenIdx;
} dal_mango_acl_physicalMove_info_t;

static dal_mango_acl_entryFieldInfo_t dal_mango_acl_specialField_list[] =
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
    {   /* field name    */           USER_FIELD_VRFID,
        /* field length  */           8,
    },
};

static uint32 template_vacl_data_field[DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD] = {
    MANGO_VACL_FIELD_0tf, MANGO_VACL_FIELD_1tf, MANGO_VACL_FIELD_2tf,
    MANGO_VACL_FIELD_3tf, MANGO_VACL_FIELD_4tf, MANGO_VACL_FIELD_5tf,
    MANGO_VACL_FIELD_6tf, MANGO_VACL_FIELD_7tf, MANGO_VACL_FIELD_8tf,
    MANGO_VACL_FIELD_9tf, MANGO_VACL_FIELD_10tf, MANGO_VACL_FIELD_11tf,
    MANGO_VACL_FIELD_12tf, MANGO_VACL_FIELD_13tf};

static uint32 template_vacl_mask_field[DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD] = {
    MANGO_VACL_BMSK_FIELD_0tf, MANGO_VACL_BMSK_FIELD_1tf, MANGO_VACL_BMSK_FIELD_2tf,
    MANGO_VACL_BMSK_FIELD_3tf, MANGO_VACL_BMSK_FIELD_4tf, MANGO_VACL_BMSK_FIELD_5tf,
    MANGO_VACL_BMSK_FIELD_6tf, MANGO_VACL_BMSK_FIELD_7tf, MANGO_VACL_BMSK_FIELD_8tf,
    MANGO_VACL_BMSK_FIELD_9tf, MANGO_VACL_BMSK_FIELD_10tf, MANGO_VACL_BMSK_FIELD_11tf,
    MANGO_VACL_BMSK_FIELD_12tf, MANGO_VACL_BMSK_FIELD_13tf};

static uint32 template_iacl_data_field[DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD] = {
    MANGO_IACL_FIELD_0tf, MANGO_IACL_FIELD_1tf, MANGO_IACL_FIELD_2tf,
    MANGO_IACL_FIELD_3tf, MANGO_IACL_FIELD_4tf, MANGO_IACL_FIELD_5tf,
    MANGO_IACL_FIELD_6tf, MANGO_IACL_FIELD_7tf, MANGO_IACL_FIELD_8tf,
    MANGO_IACL_FIELD_9tf, MANGO_IACL_FIELD_10tf, MANGO_IACL_FIELD_11tf,
    MANGO_IACL_FIELD_12tf, MANGO_IACL_FIELD_13tf};

static uint32 template_iacl_mask_field[DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD] = {
    MANGO_IACL_BMSK_FIELD_0tf, MANGO_IACL_BMSK_FIELD_1tf, MANGO_IACL_BMSK_FIELD_2tf,
    MANGO_IACL_BMSK_FIELD_3tf, MANGO_IACL_BMSK_FIELD_4tf, MANGO_IACL_BMSK_FIELD_5tf,
    MANGO_IACL_BMSK_FIELD_6tf, MANGO_IACL_BMSK_FIELD_7tf, MANGO_IACL_BMSK_FIELD_8tf,
    MANGO_IACL_BMSK_FIELD_9tf, MANGO_IACL_BMSK_FIELD_10tf, MANGO_IACL_BMSK_FIELD_11tf,
    MANGO_IACL_BMSK_FIELD_12tf, MANGO_IACL_BMSK_FIELD_13tf};

static uint32 template_eacl_data_field[DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD] = {
    MANGO_EACL_FIELD_0tf, MANGO_EACL_FIELD_1tf, MANGO_EACL_FIELD_2tf,
    MANGO_EACL_FIELD_3tf, MANGO_EACL_FIELD_4tf, MANGO_EACL_FIELD_5tf,
    MANGO_EACL_FIELD_6tf, MANGO_EACL_FIELD_7tf, MANGO_EACL_FIELD_8tf,
    MANGO_EACL_FIELD_9tf, MANGO_EACL_FIELD_10tf, MANGO_EACL_FIELD_11tf,
    MANGO_EACL_FIELD_12tf, MANGO_EACL_FIELD_13tf};

static uint32 template_eacl_mask_field[DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD] = {
    MANGO_EACL_BMSK_FIELD_0tf, MANGO_EACL_BMSK_FIELD_1tf, MANGO_EACL_BMSK_FIELD_2tf,
    MANGO_EACL_BMSK_FIELD_3tf, MANGO_EACL_BMSK_FIELD_4tf, MANGO_EACL_BMSK_FIELD_5tf,
    MANGO_EACL_BMSK_FIELD_6tf, MANGO_EACL_BMSK_FIELD_7tf, MANGO_EACL_BMSK_FIELD_8tf,
    MANGO_EACL_BMSK_FIELD_9tf, MANGO_EACL_BMSK_FIELD_10tf, MANGO_EACL_BMSK_FIELD_11tf,
    MANGO_EACL_BMSK_FIELD_12tf, MANGO_EACL_BMSK_FIELD_13tf};

dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TEMPLATE_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGR_NML_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FRAME_TYPE_L2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ITAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OTAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ITAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OTAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FRAME_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FRAME_TYPE_L4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP_NONZERO_OFFSET[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DEV_DMAC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_MGNT_VLAN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_VRFID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0x9,
        /* length */                 6,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0x0,
        /* length */                 9,
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DPM[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x0,
        /* length */                 9,
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DMAC[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SMAC[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SENDER_ADDR[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TARGET_ADDR[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ETHERTYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ETHERTYPE,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ETHER_AUX[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OTAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DEI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OTAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ITAG_PRI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xd,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CFI_VALUE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ITAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ETAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ARPOPCODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};

dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP4_SIP[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP4_DIP[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_SIP[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_DIP[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP4TOS_IP6TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IPDSCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0xA,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP4PROTO_IP6NH[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_L4_SRC_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_L4_DST_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_UNSEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xf,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_UNREP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_NONEXT_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_MOB_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_ESP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_AUTH_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_DEST_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_FRAG_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_ROUTING_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP6_HOP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x3,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP_FRAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP4_TTL_IP6_HOPLIMIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGMP_MAX_RESP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0xe,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TCP_ECN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TCP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TCP_NONZERO_SEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ICMP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ICMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR_VALID_MSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x0,
        /* length */                 14,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_L2_CRC_ERROR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP4_HDR_ERR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0xf,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IPMC_ROUT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IPUC_ROUT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR0[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR3[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR5[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR6[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR7[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR8[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR9[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR10[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR11[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR12[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_12,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FIELD_SELECTOR13[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_13,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_URPF_CHK_FAIL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_PORT_MV[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGR_VLAN_DROP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_STP_DROP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SPP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0xa,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DATYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x8,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IP_SUBNET_BASED_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_MAC_BASED_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x1,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IVC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_VACL_DROP_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_VACL_COPY_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_VACL_REDIRECT_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x5,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_ATTACK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x4,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SMAC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x3,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SA_NH_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TSID,
        /* offset address */         0xD,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SA_STATIC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TSID,
        /* offset address */         0xC,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DIP_HOST_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DIP_PREFIX_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x1,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SIP_HOST_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TSID,
        /* offset address */         0xE,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DMAC_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_IGR_FIELD_CONTENT_TOO_DEEP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CONTENT_TOO_DEEP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FWD_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FWD_VID,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_VLAN_GRPMSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_VLAN_GMSK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_METADATA[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_LB_PKT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SPM3,
        /* offset address */         0xF,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_LB_TIMES[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0xC,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_PCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0xD,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_DEI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0xC,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_IGR_ECID_BASE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_TAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0xE,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_ECID_GRP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0xC,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_ECID_BASE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_IGR_ECID_EXT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_1BR_ECID_EXT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_FLOW_LABEL[] =
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DSAP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SSAP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_KBIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_MBIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0xb,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_WBIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0xa,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_LBIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_FBIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_TBIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_RID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x2,
        /* length */                 5,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_HDR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x1,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_TT_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_GRE_KEY[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_GTP_TEID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SNAP_OUI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
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
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_80211_WEP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0xe,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_80211_MF[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0xd,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_TS_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_EGR_BSSID_IDX[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x0,
        /* length */                 11,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_L4_HDR[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_SLP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGR_DEV[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x6,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGR_TRK_PRESENT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGR_TRK_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x0,
        /* length */                 7,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DPP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x9,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_DLP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_EGR_DEV[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x6,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_EGR_TRK_PRESENT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_EGR_TRK_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
        /* length */                 7,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_80211_FRAME_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0xd,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_CAPWAP_IGR_BSSID_IDX[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
        /* length */                 11,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TT_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TT_IDX[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0x0,
        /* length */                 9,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TS_HIT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TSID,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_TS_IDX[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TSID,
        /* offset address */         0x0,
        /* length */                 9,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_APP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0xB,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OMPLS_LABEL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x0,
        /* length */                 4,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OMPLS_EXP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x4,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OMPLS_LABEL_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0xE,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_OMPLS_BOS[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IMPLS_LABEL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x0,
        /* length */                 4,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IMPLS_EXP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x4,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IMPLS_LABEL_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0xF,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IMPLS_BOS[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_VXLAN_VNI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x10
    },
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_acl_fieldLocation_t DAL_MANGO_FIELD_IGR_INTF[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0x0,
        /* length */                 10,
        /* data offset */            0x0
    },
};

dal_mango_acl_fixField_t dal_mango_acl_vFixField_list[] =
{
    {   /* data field       */  MANGO_VACL_TIDtf,
        /* mask field name  */  MANGO_VACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_MANGO_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  MANGO_VACL_IGR_NML_PORTtf,
        /* mask field name  */  MANGO_VACL_BMSK_IGR_NML_PORTtf,
        /* position         */  1,
        /* field name       */  USER_FIELD_IGR_NML_PORT,
        /* field pointer    */  DAL_MANGO_FIELD_IGR_NML_PORT
    },
    {   /* data field       */  MANGO_VACL_FRAME_TYPE_L2tf,
        /* mask field name  */  MANGO_VACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  MANGO_VACL_ITAG_EXISTtf,
        /* mask field name  */  MANGO_VACL_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_MANGO_FIELD_ITAG_EXIST
    },
    {   /* data field       */  MANGO_VACL_OTAG_EXISTtf,
        /* mask field name  */  MANGO_VACL_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_MANGO_FIELD_OTAG_EXIST
    },
    {   /* data field       */  MANGO_VACL_ITAG_FMTtf,
        /* mask field name  */  MANGO_VACL_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_MANGO_FIELD_ITAG_FMT
    },
    {   /* data field       */  MANGO_VACL_OTAG_FMTtf,
        /* mask field name  */  MANGO_VACL_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_MANGO_FIELD_OTAG_FMT
    },
    {   /* data field       */  MANGO_VACL_FRAME_TYPEtf,
        /* mask field name  */  MANGO_VACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE
    },
    {   /* data field       */  MANGO_VACL_FRAME_TYPE_L4tf,
        /* mask field name  */  MANGO_VACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  MANGO_VACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  MANGO_VACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_MANGO_FIELD_IP_NONZERO_OFFSET
    },
    {   /* data field       */  MANGO_VACL_CONTENT_TOO_DEEPtf,
        /* mask field name  */  MANGO_VACL_BMSK_CONTENT_TOO_DEEPtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_CONTENT_TOO_DEEP,
        /* field pointer    */  DAL_MANGO_IGR_FIELD_CONTENT_TOO_DEEP
    },
    {   /* data field       */  MANGO_VACL_MGNT_VLANtf,
        /* mask field name  */  MANGO_VACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_MANGO_FIELD_MGNT_VLAN
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_mango_acl_vFixField_list */

dal_mango_acl_fixField_t dal_mango_acl_iFixField_list[] =
{
    {   /* data field       */  MANGO_IACL_TIDtf,
        /* mask field name  */  MANGO_IACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_MANGO_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  MANGO_IACL_IGR_NML_PORTtf,
        /* mask field name  */  MANGO_IACL_BMSK_IGR_NML_PORTtf,
        /* position         */  1,
        /* field name       */  USER_FIELD_IGR_NML_PORT,
        /* field pointer    */  DAL_MANGO_FIELD_IGR_NML_PORT
    },
    {   /* data field       */  MANGO_IACL_FRAME_TYPE_L2tf,
        /* mask field name  */  MANGO_IACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  MANGO_IACL_ITAG_EXISTtf,
        /* mask field name  */  MANGO_IACL_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_MANGO_FIELD_ITAG_EXIST
    },
    {   /* data field       */  MANGO_IACL_OTAG_EXISTtf,
        /* mask field name  */  MANGO_IACL_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_MANGO_FIELD_OTAG_EXIST
    },
    {   /* data field       */  MANGO_IACL_ITAG_FMTtf,
        /* mask field name  */  MANGO_IACL_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_MANGO_FIELD_ITAG_FMT
    },
    {   /* data field       */  MANGO_IACL_OTAG_FMTtf,
        /* mask field name  */  MANGO_IACL_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_MANGO_FIELD_OTAG_FMT
    },
    {   /* data field       */  MANGO_IACL_FRAME_TYPEtf,
        /* mask field name  */  MANGO_IACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE
    },
    {   /* data field       */  MANGO_IACL_FRAME_TYPE_L4tf,
        /* mask field name  */  MANGO_IACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  MANGO_IACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  MANGO_IACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_MANGO_FIELD_IP_NONZERO_OFFSET
    },
    {   /* data field       */  MANGO_IACL_DEVMACtf,
        /* mask field name  */  MANGO_IACL_BMSK_DEVMACtf,
        /* position         */  14,
        /* field name       */  USER_FIELD_DEV_DMAC,
        /* field pointer    */  DAL_MANGO_FIELD_DEV_DMAC
    },
    {   /* data field       */  MANGO_IACL_MGNT_VLANtf,
        /* mask field name  */  MANGO_IACL_BMSK_MGNT_VLANtf,
        /* position         */  15,
        /* field name       */  USER_FIELD_MGNT_VLAN,
        /* field pointer    */  DAL_MANGO_FIELD_MGNT_VLAN
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_mango_acl_iFixField_list */

dal_mango_acl_fixField_t dal_mango_acl_eFixField_list[] =
{
    {   /* data field       */  MANGO_EACL_TIDtf,
        /* mask field name  */  MANGO_EACL_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  USER_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_MANGO_FIELD_TEMPLATE_ID
    },
    {   /* data field       */  MANGO_EACL_EGR_NML_PORTtf,
        /* mask field name  */  MANGO_EACL_BMSK_EGR_NML_PORTtf,
        /* position         */  1,
        /* field name       */  USER_FIELD_EGR_NML_PORT,
        /* field pointer    */  DAL_MANGO_FIELD_IGR_NML_PORT    /* same as IGR */
    },
    {   /* data field       */  MANGO_EACL_FRAME_TYPE_L2tf,
        /* mask field name  */  MANGO_EACL_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  USER_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  MANGO_EACL_ITAG_EXISTtf,
        /* mask field name  */  MANGO_EACL_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  USER_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_MANGO_FIELD_ITAG_EXIST
    },
    {   /* data field       */  MANGO_EACL_OTAG_EXISTtf,
        /* mask field name  */  MANGO_EACL_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  USER_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_MANGO_FIELD_OTAG_EXIST
    },
    {   /* data field       */  MANGO_EACL_ITAG_FMTtf,
        /* mask field name  */  MANGO_EACL_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  USER_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_MANGO_FIELD_ITAG_FMT
    },
    {   /* data field       */  MANGO_EACL_OTAG_FMTtf,
        /* mask field name  */  MANGO_EACL_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  USER_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_MANGO_FIELD_OTAG_FMT
    },
    {   /* data field       */  MANGO_EACL_FRAME_TYPEtf,
        /* mask field name  */  MANGO_EACL_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  USER_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE
    },
    {   /* data field       */  MANGO_EACL_FRAME_TYPE_L4tf,
        /* mask field name  */  MANGO_EACL_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  USER_FIELD_L4_PROTO,
        /* field pointer    */  DAL_MANGO_FIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  MANGO_EACL_NOT_FIRST_FRAGtf,
        /* mask field name  */  MANGO_EACL_BMSK_NOT_FIRST_FRAGtf,
        /* position         */  13,
        /* field name       */  USER_FIELD_IP_NONZEROOFFSET,
        /* field pointer    */  DAL_MANGO_FIELD_IP_NONZERO_OFFSET
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  USER_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_mango_acl_eFixField_list */

dal_mango_acl_entryField_t dal_mango_acl_common_field_list[] =
{
    {   /* field name    */           USER_FIELD_DMAC,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_FIELD_DMAC
    },
    {   /* field name    */           USER_FIELD_SMAC,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_FIELD_SMAC
    },
    {   /* field name    */           USER_FIELD_SENDER_ADDR,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_FIELD_SENDER_ADDR
    },
    {   /* field name    */           USER_FIELD_TARGET_ADDR,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_FIELD_TARGET_ADDR
    },
    {   /* field name    */           USER_FIELD_ETHERTYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ETHERTYPE
    },
    {   /* field name    */           USER_FIELD_ETHER_AUX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ETHER_AUX
    },
    {   /* field name    */           USER_FIELD_OTAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_OTAG_PRI
    },
    {   /* field name    */           USER_FIELD_DEI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DEI_VALUE
    },
    {   /* field name    */           USER_FIELD_OTAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_OTAG_VID
    },
    {   /* field name    */           USER_FIELD_ITAG_PRI,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ITAG_PRI
    },
    {   /* field name    */           USER_FIELD_CFI_VALUE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CFI_VALUE
    },
    {   /* field name    */           USER_FIELD_ITAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ITAG_VID
    },
    {   /* field name    */           USER_FIELD_IP4_SIP,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_IP4_SIP
    },
    {   /* field name    */           USER_FIELD_IP4_DIP,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_IP4_DIP
    },
    {   /* field name    */           USER_FIELD_IP6_SIP,
        /* field number  */           14,
        /* field pointer */           DAL_MANGO_FIELD_IP6_SIP
    },
    {   /* field name    */           USER_FIELD_IP6_DIP,
        /* field number  */           8,
        /* field pointer */           DAL_MANGO_FIELD_IP6_DIP
    },
    {   /* field name    */           USER_FIELD_IP4TOS_IP6TC,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP4TOS_IP6TC
    },
    {   /* field name    */           USER_FIELD_IP_DSCP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IPDSCP
    },
    {   /* field name    */           USER_FIELD_IP4PROTO_IP6NH,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP4PROTO_IP6NH
    },
    {   /* field name    */           USER_FIELD_ARPOPCODE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ARPOPCODE
    },
    {   /* field name    */           USER_FIELD_L4_SRC_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_L4_SRC_PORT
    },
    {   /* field name    */           USER_FIELD_L4_DST_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_L4_DST_PORT
    },
    {   /* field name    */           USER_FIELD_IGMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGMP_TYPE
    },
    {   /* field name    */           USER_FIELD_IGMP_MAX_RESP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGMP_MAX_RESP_CODE
    },
    {   /* field name    */           USER_FIELD_ICMP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ICMP_CODE
    },
    {   /* field name    */           USER_FIELD_ICMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ICMP_TYPE
    },
    {   /* field name    */           USER_FIELD_L4_HDR,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_L4_HDR
    },
    {   /* field name    */           USER_FIELD_IP6_HDR_UNSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_UNSEQ
    },
    {   /* field name    */           USER_FIELD_IP6_HDR_UNREP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_UNREP
    },
    {   /* field name    */           USER_FIELD_IP6_NONEXT_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_NONEXT_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_MOB_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_MOB_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_ESP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_ESP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_AUTH_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_AUTH_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_DEST_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_DEST_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_FRAG_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_FRAG_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_ROUTING_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_ROUTING_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP6_HOP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP6_HOP_HDR_EXIST
    },
    {   /* field name    */           USER_FIELD_IP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP_FLAG
    },
    {   /* field name    */           USER_FIELD_IP_FRAG,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP_FRAG
    },
    {   /* field name    */           USER_FIELD_IP4_TTL_IP6_HOPLIMIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP4_TTL_IP6_HOPLIMIT
    },
    {   /* field name    */           USER_FIELD_DP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DP
    },
    {   /* field name    */           USER_FIELD_ETAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ETAG_EXIST
    },
    {   /* field name    */           USER_FIELD_FLOW_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FLOW_LABEL
    },
    {   /* field name    */           USER_FIELD_TCP_NONZEROSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TCP_NONZERO_SEQ
    },
    {   /* field name    */           USER_FIELD_TCP_ECN,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TCP_ECN
    },
    {   /* field name    */           USER_FIELD_TCP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TCP_FLAG
    },
    {   /* field name    */           USER_FIELD_CONTENT_TOO_DEEP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CONTENT_TOO_DEEP
    },
    {   /* field name    */           USER_FIELD_FWD_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FWD_VID
    },
    {   /* field name    */           USER_FIELD_VID_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_RANGE
    },
    {   /* field name    */           USER_FIELD_L4_PORT_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_RANGE
    },
    {   /* field name    */           USER_FIELD_LEN_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_RANGE
    },
    {   /* field name    */           USER_FIELD_SLP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SLP
    },
    {   /* field name    */           USER_FIELD_IGR_DEV_ID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGR_DEV
    },
    {   /* field name    */           USER_FIELD_IGR_TRK_PRESENT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGR_TRK_PRESENT
    },
    {   /* field name    */           USER_FIELD_IGR_TRK_ID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGR_TRK_ID
    },
    {   /* field name    */           USER_FIELD_META_DATA,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_METADATA
    },
    {   /* field name    */           USER_FIELD_LB_TIMES,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_LB_TIMES
    },
    {   /* field name    */           USER_FIELD_VXLAN_VNI,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_VXLAN_VNI
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};  /* dal_mango_acl_common_field_list */

dal_mango_acl_entryField_t dal_mango_acl_vi_field_list[] =
{
    {   /* field name    */           USER_FIELD_SPM,
        /* field number  */           4,
        /* field pointer */           DAL_MANGO_FIELD_SPM
    },
    {   /* field name    */           USER_FIELD_VRFID,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_VRFID
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR_VALID_MSK,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR_VALID_MSK
    },
    {   /* field name    */           USER_FIELD_L2_CRC_ERROR,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_L2_CRC_ERROR
    },
    {   /* field name    */           USER_FIELD_IP4_HDR_ERR,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP4_HDR_ERR
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR0,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR0
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR1,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR1
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR2,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR2
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR3,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR3
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR4,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR4
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR5,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR5
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR6,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR6
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR7,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR7
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR8,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR8
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR9,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR9
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR10,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR10
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR11,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR11
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR12,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR12
    },
    {   /* field name    */           USER_FIELD_FIELD_SELECTOR13,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_FIELD_SELECTOR13
    },
    {   /* field name    */           USER_FIELD_SPP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SPP
    },
    {   /* field name    */           USER_FIELD_DATYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DATYPE
    },
    {   /* field name    */           USER_FIELD_DSAP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DSAP
    },
    {   /* field name    */           USER_FIELD_SSAP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SSAP
    },
    {   /* field name    */           USER_FIELD_CAPWAP_KBIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_KBIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_MBIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_MBIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_WBIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_WBIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_LBIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_LBIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_FBIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_FBIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_TBIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_TBIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_RID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_RID
    },
    {   /* field name    */           USER_FIELD_CAPWAP_HDR,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_HDR
    },
    {   /* field name    */           USER_FIELD_CAPWAP_TT_HIT,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_TT_HIT
    },
    {   /* field name    */           USER_FIELD_GRE_KEY,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_GRE_KEY
    },
    {   /* field name    */           USER_FIELD_GTP_TEID,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_GTP_TEID
    },
    {   /* field name    */           USER_FIELD_APP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_APP_TYPE
    },
    {   /* field name    */           USER_FIELD_OMPLS_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_OMPLS_LABEL
    },
    {   /* field name    */           USER_FIELD_OMPLS_EXP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_OMPLS_EXP
    },
    {   /* field name    */           USER_FIELD_OMPLS_LABEL_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_OMPLS_LABEL_EXIST
    },
    {   /* field name    */           USER_FIELD_OMPLS_BOS,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_OMPLS_BOS
    },
    {   /* field name    */           USER_FIELD_IMPLS_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_IMPLS_LABEL
    },
    {   /* field name    */           USER_FIELD_IMPLS_EXP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IMPLS_EXP
    },
    {   /* field name    */           USER_FIELD_IMPLS_LABEL_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IMPLS_LABEL_EXIST
    },
    {   /* field name    */           USER_FIELD_IMPLS_BOS,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IMPLS_BOS
    },
    {   /* field name    */           USER_FIELD_SNAP_OUI,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_FIELD_SNAP_OUI
    },
    {   /* field name    */           USER_FIELD_CAPWAP_80211_WEP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_80211_WEP
    },
    {   /* field name    */           USER_FIELD_CAPWAP_80211_MF,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_80211_MF
    },
    {   /* field name    */           USER_FIELD_CAPWAP_TS_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_TS_HIT
    },
    {   /* field name    */           USER_FIELD_CAPWAP_EGR_BSSID_IDX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_EGR_BSSID_IDX
    },
    {   /* field name    */           USER_FIELD_VLAN_GRPMSK,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_VLAN_GRPMSK
    },
    {   /* field name    */           USER_FIELD_CAPWAP_80211_FRAME_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_80211_FRAME_TYPE
    },
    {   /* field name    */           USER_FIELD_CAPWAP_IGR_BSSID_IDX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_CAPWAP_IGR_BSSID_IDX
    },
    {   /* field name    */           USER_FIELD_TT_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TT_HIT
    },
    {   /* field name    */           USER_FIELD_TT_IDX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TT_IDX
    },
    {   /* field name    */           USER_FIELD_IP_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP_RANGE
    },
    {   /* field name    */           USER_FIELD_IGR_INTF_IDX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGR_INTF
    },
    {   /* field name    */           USER_FIELD_LB_PKT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_LB_PKT
    },
    {   /* field name    */           USER_FIELD_1BR_PCP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_PCP
    },
    {   /* field name    */           USER_FIELD_1BR_DEI,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_DEI
    },
    {   /* field name    */           USER_FIELD_1BR_IGR_ECID_BASE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_IGR_ECID_BASE
    },
    {   /* field name    */           USER_FIELD_1BR_TAG_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_TAG_EXIST
    },
    {   /* field name    */           USER_FIELD_1BR_ECID_GRP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_ECID_GRP
    },
    {   /* field name    */           USER_FIELD_1BR_ECID_BASE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_ECID_BASE
    },
    {   /* field name    */           USER_FIELD_1BR_IGR_ECID_EXT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_IGR_ECID_EXT
    },
    {   /* field name    */           USER_FIELD_1BR_ECID_EXT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_1BR_ECID_EXT
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};  /* dal_mango_acl_vi_field_list */

dal_mango_acl_entryField_t dal_mango_acl_v_field_list[] =
{
    {   /* field name    */           USER_FIELD_IP_SUBNET_BASED_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IP_SUBNET_BASED_HIT
    },
    {   /* field name    */           USER_FIELD_MAC_BASED_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_MAC_BASED_HIT
    },
    {   /* field name    */           USER_FIELD_IVC_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IVC_HIT
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};  /* dal_mango_acl_v_field_list */

dal_mango_acl_entryField_t dal_mango_acl_i_field_list[] =
{
    {   /* field name    */           USER_FIELD_VACL_DROP_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_VACL_DROP_HIT
    },
    {   /* field name    */           USER_FIELD_VACL_COPY_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_VACL_COPY_HIT
    },
    {   /* field name    */           USER_FIELD_VACL_REDIRECT_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_VACL_REDIRECT_HIT
    },
    {   /* field name    */           USER_FIELD_ATTACK,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_ATTACK
    },
    {   /* field name    */           USER_FIELD_SA_LUT_RESULT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SMAC_HIT
    },
    {   /* field name    */           USER_FIELD_SA_NH_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SA_NH_HIT
    },
    {   /* field name    */           USER_FIELD_SA_STATIC_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SA_STATIC_HIT
    },
    {   /* field name    */           USER_FIELD_DIP_HOST_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DIP_HOST_HIT
    },
    {   /* field name    */           USER_FIELD_DIP_PREFIX_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DIP_PREFIX_HIT
    },
    {   /* field name    */           USER_FIELD_SIP_HOST_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_SIP_HOST_HIT
    },
    {   /* field name    */           USER_FIELD_DA_LUT_RESULT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DMAC_HIT
    },
    {   /* field name    */           USER_FIELD_IPUC_ROUT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IPUC_ROUT
    },
    {   /* field name    */           USER_FIELD_IPMC_ROUT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IPMC_ROUT
    },
    {   /* field name    */           USER_FIELD_URPF_CHK_FAIL,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_URPF_CHK_FAIL
    },
    {   /* field name    */           USER_FIELD_PORT_MV,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_PORT_MV
    },
    {   /* field name    */           USER_FIELD_IGR_VLAN_DROP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_IGR_VLAN_DROP
    },
    {   /* field name    */           USER_FIELD_STP_DROP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_STP_DROP
    },
    {   /* field name    */           USER_FIELD_TS_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TS_HIT
    },
    {   /* field name    */           USER_FIELD_TS_IDX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TS_IDX
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};  /* dal_mango_acl_i_field_list */

dal_mango_acl_entryField_t dal_mango_acl_e_field_list[] =
{
    {   /* field name    */           USER_FIELD_DPM,
        /* field number  */           4,
        /* field pointer */           DAL_MANGO_FIELD_DPM
    },
    {   /* field name    */           USER_FIELD_DPP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DPP
    },
    {   /* field name    */           USER_FIELD_DLP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_DLP
    },
    {   /* field name    */           USER_FIELD_EGR_DEV_ID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_EGR_DEV
    },
    {   /* field name    */           USER_FIELD_EGR_TRK_PRESENT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_EGR_TRK_PRESENT
    },
    {   /* field name    */           USER_FIELD_EGR_TRK_ID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_EGR_TRK_ID
    },
    {   /* field name    */           USER_FIELD_TS_HIT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TS_HIT
    },
    {   /* field name    */           USER_FIELD_TS_IDX,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_FIELD_TS_IDX
    },
    {   /* field name    */           USER_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};  /* dal_mango_acl_e_field_list */

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

#define ACL_TBL_READ(_u, _t, _idx, _p)  _dal_mango_acl_tbl_read(_u, _t, _idx, _p)
#define ACL_TBL_WRITE(_u, _t, _idx, _p) _dal_mango_acl_tbl_write(_u, _t, _idx, _p)
/*
 * Function Declaration
 */
static int32
_dal_mango_acl_entryAggType_get(uint32 unit, uint32 entryIdx,
    dal_mango_acl_aggInfo_t *pAggInfo);

static int32
_dal_mango_acl_aggEntry_del(uint32 unit, rtk_acl_phase_t phase, dal_mango_acl_aggInfo_t *pAggInfo);


static int32
_dal_mango_acl_tbl_read(uint32 unit, uint32 tbl, uint32 idx, uint32 *pEntry)
{
    uint32  *pShadowEntry, entryByteNum;

    RT_PARAM_CHK((!pACL_TBL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_FILTER_ID(unit) <= idx), RT_ERR_OUT_OF_RANGE);

    entryByteNum = sizeof(acl_entry_t);
    pShadowEntry = pACL_TBL + ((entryByteNum / sizeof(uint32)) * idx);

    osal_memcpy(pEntry, pShadowEntry, entryByteNum);
    //RT_ERR_CHK(table_read(unit, tbl, idx, entry), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_tbl_read */

static int32
_dal_mango_acl_tbl_write(uint32 unit, uint32 tbl, uint32 idx, uint32 *pEntry)
{
    uint32  *pShadowEntry, entryByteNum;
    int32   ret;

    RT_PARAM_CHK((!pACL_TBL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_FILTER_ID(unit) <= idx), RT_ERR_OUT_OF_RANGE);

    entryByteNum = sizeof(acl_entry_t);
    pShadowEntry = pACL_TBL + ((entryByteNum / sizeof(uint32)) * idx);

    RT_ERR_CHK(table_write(unit, tbl, idx, pEntry), ret);
    osal_memcpy(pShadowEntry, pEntry, entryByteNum);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_tbl_write */

static int32 _dal_mango_acl_phaseInfo_get(rtk_acl_phase_t phase,
    dal_mango_phaseInfo_t *phaseInfo)
{
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == phaseInfo), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            phaseInfo->table = MANGO_VACLt;
            phaseInfo->fixFieldList = dal_mango_acl_vFixField_list;
            phaseInfo->tmplteDataFieldList = template_vacl_data_field;
            phaseInfo->tmplteMaskFieldList = template_vacl_mask_field;
            break;
        case ACL_PHASE_IACL:
            phaseInfo->table = MANGO_IACLt;
            phaseInfo->fixFieldList = dal_mango_acl_iFixField_list;
            phaseInfo->tmplteDataFieldList = template_iacl_data_field;
            phaseInfo->tmplteMaskFieldList = template_iacl_mask_field;
            break;
        case ACL_PHASE_EACL:
            phaseInfo->table = MANGO_EACLt;
            phaseInfo->fixFieldList = dal_mango_acl_eFixField_list;
            phaseInfo->tmplteDataFieldList = template_eacl_data_field;
            phaseInfo->tmplteMaskFieldList = template_eacl_mask_field;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "Inavlid phase %d", phase);
            return RT_ERR_ACL_PHASE;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_phaseInfo_get */

static int32 _dal_mango_acl_fieldInfo_get(rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, dal_mango_acl_entryField_t **fieldList,
    uint32 *fieldListIdx)
{
    uint32  idx;

    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == fieldListIdx), RT_ERR_NULL_POINTER);

    for (idx = 0; dal_mango_acl_common_field_list[idx].type != USER_FIELD_END; ++idx)
    {
        if (type == dal_mango_acl_common_field_list[idx].type)
        {
            *fieldList = dal_mango_acl_common_field_list;
            *fieldListIdx = idx;
            return RT_ERR_OK;
        }
    }

    if (ACL_PHASE_VACL == phase || ACL_PHASE_IACL == phase)
    {
        for (idx = 0; dal_mango_acl_vi_field_list[idx].type != USER_FIELD_END; ++idx)
        {
            if (type == dal_mango_acl_vi_field_list[idx].type)
            {
                *fieldList = dal_mango_acl_vi_field_list;
                *fieldListIdx = idx;
                return RT_ERR_OK;
            }
        }
    }

    if (ACL_PHASE_VACL == phase)
    {
        for (idx = 0; dal_mango_acl_v_field_list[idx].type != USER_FIELD_END; ++idx)
        {
            if (type == dal_mango_acl_v_field_list[idx].type)
            {
                *fieldList = dal_mango_acl_v_field_list;
                *fieldListIdx = idx;
                return RT_ERR_OK;
            }
        }
    }

    if (ACL_PHASE_IACL == phase)
    {
        for (idx = 0; dal_mango_acl_i_field_list[idx].type != USER_FIELD_END; ++idx)
        {
            if (type == dal_mango_acl_i_field_list[idx].type)
            {
                *fieldList = dal_mango_acl_i_field_list;
                *fieldListIdx = idx;
                return RT_ERR_OK;
            }
        }
    }

    if (ACL_PHASE_EACL == phase)
    {
        for (idx = 0; dal_mango_acl_e_field_list[idx].type != USER_FIELD_END; ++idx)
        {
            if (type == dal_mango_acl_e_field_list[idx].type)
            {
                *fieldList = dal_mango_acl_e_field_list;
                *fieldListIdx = idx;
                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_ACL_FIELD_TYPE;
}   /* end of _dal_mango_acl_fieldInfo_get */

static int32 _dal_mango_acl_fieldType_chk(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type)
{
    dal_mango_acl_fixField_t        *fixList = NULL;
    dal_mango_acl_entryField_t      *fieldList = NULL;
    uint32                          idx;
    int32                           ret;

    switch (phase)
    {
        case ACL_PHASE_VACL:
            fixList = dal_mango_acl_vFixField_list;
            break;
        case ACL_PHASE_IACL:
            fixList = dal_mango_acl_iFixField_list;
            break;
        case ACL_PHASE_EACL:
            fixList = dal_mango_acl_eFixField_list;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    for (idx = 0; fixList[idx].type != USER_FIELD_END; ++idx)
    {
        if (type == fixList[idx].type)
            return RT_ERR_OK;
    }

    if ((ret = _dal_mango_acl_fieldInfo_get(phase, type, &fieldList, &idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_fieldType_chk */




/* Function Name:
 *      dal_mango_acl_ruleEntryFieldSize_get
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
dal_mango_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type,
    uint32 *pField_size)
{
    uint32  i, j;
    uint32  fieldNumber;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d type=%d", unit, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);
    RT_PARAM_CHK((NULL == pField_size), RT_ERR_NULL_POINTER);

    /* check special field type */
    for (i = 0; i < (sizeof(dal_mango_acl_specialField_list)/sizeof(dal_mango_acl_entryFieldInfo_t)); ++i)
    {
        if (type == dal_mango_acl_specialField_list[i].type)
        {
            *pField_size = dal_mango_acl_specialField_list[i].fieldLen;
            return RT_ERR_OK;
        }
    }

    /* Get chip fixed field type */
    for (i = 0; dal_mango_acl_iFixField_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_iFixField_list[i].type)
        {
            *pField_size = dal_mango_acl_iFixField_list[i].pField[0].field_length;
            return RT_ERR_OK;
        }
    }

    for (i = 0; dal_mango_acl_eFixField_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_eFixField_list[i].type)
        {
            *pField_size = dal_mango_acl_eFixField_list[i].pField[0].field_length;
            return RT_ERR_OK;
        }
    }

    /* Get chip specific field type from database */
    for (i = 0; dal_mango_acl_common_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_common_field_list[i].type)
        {
            fieldNumber = dal_mango_acl_common_field_list[i].fieldNumber;
            *pField_size = 0;
            for (j = 0; j < fieldNumber; ++j)
            {
                *pField_size += dal_mango_acl_common_field_list[i].pField[j].field_length;
            }

            return RT_ERR_OK;
        }
    }

    for (i = 0; dal_mango_acl_vi_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_vi_field_list[i].type)
        {
            fieldNumber = dal_mango_acl_vi_field_list[i].fieldNumber;
            *pField_size = 0;
            for (j = 0; j < fieldNumber; ++j)
            {
                *pField_size += dal_mango_acl_vi_field_list[i].pField[j].field_length;
            }

            return RT_ERR_OK;
        }
    }

    for (i = 0; dal_mango_acl_v_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_v_field_list[i].type)
        {
            fieldNumber = dal_mango_acl_v_field_list[i].fieldNumber;
            *pField_size = 0;
            for (j = 0; j < fieldNumber; ++j)
            {
                *pField_size += dal_mango_acl_v_field_list[i].pField[j].field_length;
            }

            return RT_ERR_OK;
        }
    }

    for (i = 0; dal_mango_acl_i_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_i_field_list[i].type)
        {
            fieldNumber = dal_mango_acl_i_field_list[i].fieldNumber;
            *pField_size = 0;
            for (j = 0; j < fieldNumber; ++j)
            {
                *pField_size += dal_mango_acl_i_field_list[i].pField[j].field_length;
            }

            return RT_ERR_OK;
        }
    }

    for (i = 0; dal_mango_acl_e_field_list[i].type != USER_FIELD_END; ++i)
    {
        if (type == dal_mango_acl_e_field_list[i].type)
        {
            fieldNumber = dal_mango_acl_e_field_list[i].fieldNumber;
            *pField_size = 0;
            for (j = 0; j < fieldNumber; ++j)
            {
                *pField_size += dal_mango_acl_e_field_list[i].pField[j].field_length;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_ACL_FIELD_TYPE;
}   /* end of dal_mango_acl_ruleEntryFieldSize_get */

/* Function Name:
 *      dal_mango_acl_ruleEntrySize_get
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
dal_mango_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase,
    uint32 *pEntry_size)
{
    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_size), RT_ERR_NULL_POINTER);

    *pEntry_size = (DAL_MANGO_BYTE_OF_TEMPLATE_FIELD +
            DAL_MANGO_BYTE_OF_FIXED_FIELD) * 2;

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntrySize_get */

/* Function Name:
 *      dal_mango_acl_ruleValidate_get
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
dal_mango_acl_ruleValidate_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pValid)
{
    acl_entry_t entry;
    uint32      phyEntryId;
    uint32      table, field;
    int32       ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            table = MANGO_VACLt;
            field = MANGO_VACL_VALIDtf;
            break;
        case ACL_PHASE_IACL:
            table = MANGO_IACLt;
            field = MANGO_IACL_VALIDtf;
            break;
        case ACL_PHASE_EACL:
            table = MANGO_EACLt;
            field = MANGO_EACL_VALIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, table, phyEntryId, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, table, field, pValid, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pValid=%d", *pValid);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleValidate_get */

/* Function Name:
 *      dal_mango_acl_ruleValidate_set
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
dal_mango_acl_ruleValidate_set(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 valid)
{
    acl_entry_t entry;
    uint32      phyEntryId;
    uint32      table, field;
    int32       ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((valid != 0) && (valid != 1), RT_ERR_INPUT);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            table = MANGO_VACLt;
            field = MANGO_VACL_VALIDtf;
            break;
        case ACL_PHASE_IACL:
            table = MANGO_IACLt;
            field = MANGO_IACL_VALIDtf;
            break;
        case ACL_PHASE_EACL:
            table = MANGO_EACLt;
            field = MANGO_EACL_VALIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, table, phyEntryId, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, table, field, &valid, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if ((ret = ACL_TBL_WRITE(unit, table, phyEntryId, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleValidate_set */

/* Function Name:
 *      dal_mango_acl_ruleEntry_read
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
dal_mango_acl_ruleEntry_read(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint8 *pEntry_buffer)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    uint32                  size;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",
            unit, phase, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId,
            (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_mango_acl_ruleEntrySize_get(unit, phase, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(pEntry_buffer, &entry, size);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntry_read */

/* Function Name:
 *      dal_mango_acl_ruleEntry_write
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
dal_mango_acl_ruleEntry_write(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint8 *pEntry_buffer)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    uint32                  size;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, pEntry_buffer=%x",
            unit, phase, entry_idx, pEntry_buffer);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = dal_mango_acl_ruleEntrySize_get(unit, phase, &size)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }

    osal_memcpy(&entry, pEntry_buffer, size);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_WRITE(unit, phaseInfo.table, phyEntryId, (uint32 *)&entry)) != RT_ERR_OK)
    {
         ACL_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
         return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntry_write */

/* Function Name:
 *      _dal_mango_acl_field2Buf_get
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
_dal_mango_acl_field2Buf_get(uint32 unit, dal_mango_acl_fieldLocation_t *field,
    rtk_acl_fieldType_t type, uint32 data, uint32 mask,
    uint8 *pData, uint8 *pMask)
{
    uint32  field_offset, field_length; /* the type data in chip field offset and length */
    uint32  field_size;                 /* the total size of type data */
    uint32  data_offset;                /* the chip field data will push into buffer offset */
    uint32  buf_byte_idx_num;           /* total bytes of type data */
    uint32  mask_len, i;
    uint32  chip_field_offset;          /* get the chip field data offset (per byte) */
    uint32  buf_field_bit_start, buf_field_bit_end;
    uint32  buf_field_bit_max;          /* max bits of buffer in the type of the field */
    uint32  buf_field_byte_idx_start;   /* start byte index of buffer (per byte) */
    uint32  buf_field_byte_idx_end;     /* end byte index of buffer (per byte) */
    uint32  data_field;
    uint8   *pTmp_data, *pTmp_mask;
    uint32  tmp_offset;

    field_offset = field->field_offset;
    field_length = field->field_length;
    data_offset = field->data_offset;

    dal_mango_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_MANGO_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_MANGO_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_MANGO_GET_BYTE_IDX(buf_field_bit_max);
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_bit_end = ((i + 1) * DAL_MANGO_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = (buf_byte_idx_num - i - 1);
        pTmp_data = pData + tmp_offset;
        pTmp_mask = pMask + tmp_offset;

        data_field = ((1 << mask_len) - 1);
        *pTmp_data &= ~data_field;
        *pTmp_mask &= ~data_field;

        *pTmp_data |= ((data >> chip_field_offset) & ((1 << mask_len)-1));
        *pTmp_mask |= ((mask >> chip_field_offset) & ((1 << mask_len)-1));

        chip_field_offset += mask_len;

        buf_field_bit_start = buf_field_bit_end + 1;
    }
}   /* end of _dal_mango_acl_field2Buf_get */

static int32
_dal_mango_acl_ruleEntryBufField_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_fieldType_t type, uint32 *buf,
    uint8 *pData, uint8 *pMask)
{
    rtk_acl_templateIdx_t           tmplteIdx;
    rtk_pie_template_t              tmplte;
    dal_mango_phaseInfo_t           phaseInfo;
    dal_mango_acl_entryField_t      *fieldList = NULL;
    dal_mango_acl_fieldLocation_t   *fieldLocation;
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

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            field = MANGO_VACL_TIDtf;
            break;
        case ACL_PHASE_IACL:
            field = MANGO_IACL_TIDtf;
            break;
        case ACL_PHASE_EACL:
            field = MANGO_EACL_TIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* handle fixed field type */
    for (i = 0; phaseInfo.fixFieldList[i].type != USER_FIELD_END; ++i)
    {
        if (type == phaseInfo.fixFieldList[i].type)
        {
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

            *pData = data;
            *pMask = mask;

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    blkIdx = entryIdx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    /* find out the binding template of the block */
    osal_memset(&tmplteIdx, 0, sizeof(rtk_acl_templateIdx_t));
    dal_mango_acl_templateSelector_get(unit, blkIdx, &tmplteIdx);
    /* get template fields */
    if ((ret = table_field_get(unit, phaseInfo.table, field, &data, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_pie_template_t));
    val = tmplteIdx.template_id[data];
    DAL_MANGO_PHASE_ACL_TO_PIE(phase, pphase);
    dal_mango_pie_phaseTemplate_get(unit, pphase, val, &tmplte);

    /* translate field from RTK superset view to DAL view */
    if ((ret = _dal_mango_acl_fieldInfo_get(phase, type, &fieldList,
            &dalFieldType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (!fieldList)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    /* search template to check all field types */
    fieldMatch = FALSE;
    for (fieldNum = 0; fieldNum < fieldList[dalFieldType].fieldNumber; ++fieldNum)
    {
        for (fieldIdx = 0; fieldIdx < DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
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
                _dal_mango_acl_field2Buf_get(unit, fieldLocation, type,
                        data, mask, pData, pMask);
            }   /* if (field->template_field_type == tmplte.field[fieldIdx]) */
        }   /* for (fieldIdx = 0; fieldIdx < DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx) */
    }   /* for (fieldNum = 0; fieldNum < fieldList[dalFieldType].fieldNumber; ++fieldNum) */

    /* can't find then return */
    if (fieldMatch != TRUE)
    {
        return RT_ERR_ACL_FIELD_TYPE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pData=%x, pMask=%x", *pData, *pMask);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_ruleEntryBufField_get */

static void
_dal_mango_acl_buf2Field_get(uint32 unit, dal_mango_acl_fieldLocation_t *field,
    rtk_acl_fieldType_t type, uint16 *data16, uint16 *mask16,
    uint8 *pData, uint8 *pMask)
{
    uint32  field_offset, data_offset, field_length;
    uint32  field_size;
    uint32  buf_byte_idx_num;   /* num of byte idx */
    uint32  mask_len, i;
    uint32  chip_field_offset;
    uint32  buf_field_bit_start, buf_field_bit_end, buf_field_bit_max;
    uint32  buf_field_byte_idx_start, buf_field_byte_idx_end;
    uint32  tmp_offset;
    uint8   *pTmp_data, *pTmp_mask;

    *data16 = *mask16 = 0;

    field_offset = field->field_offset;
    field_length = field->field_length;
    data_offset = field->data_offset;

    dal_mango_acl_ruleEntryFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_MANGO_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_MANGO_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_MANGO_GET_BYTE_IDX(buf_field_bit_max);
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_bit_end = ((i + 1) * DAL_MANGO_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = (buf_byte_idx_num - i - 1);
        pTmp_data = pData + tmp_offset;
        pTmp_mask = pMask + tmp_offset;

        *data16 |= (*pTmp_data & ((1 << mask_len)-1)) << chip_field_offset;
        *mask16 |= (*pTmp_mask & ((1 << mask_len)-1)) << chip_field_offset;

        chip_field_offset += mask_len;

        buf_field_bit_start = buf_field_bit_end + 1;
    }
}   /* end of _dal_mango_acl_buf2Field_get */

static int32
_dal_mango_acl_ruleEntryBufField_set(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_fieldType_t type, uint32 *buf,
    uint8 *pData, uint8 *pMask)
{
    rtk_acl_templateIdx_t           tmplteIdx;
    rtk_pie_template_t              tmplte;
    dal_mango_phaseInfo_t           phaseInfo;
    dal_mango_acl_entryField_t      *fieldList = NULL;
    dal_mango_acl_fieldLocation_t   *fieldLocation;
    uint32                          field;
    uint32                          val;
    uint32                          blkIdx, fieldIdx, fieldNum;
    uint32                          fieldData = 0, fieldMask = 0;
    uint32                          data, mask;
    uint32                          fieldMatch = FALSE;
    uint32                          dalFieldType;
    uint32                          i;
    int32                           ret;
    uint16                          data16, mask16;
    rtk_pie_phase_t                 pphase;

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            field = MANGO_VACL_TIDtf;
            break;
        case ACL_PHASE_IACL:
            field = MANGO_IACL_TIDtf;
            break;
        case ACL_PHASE_EACL:
            field = MANGO_EACL_TIDtf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    /* handle fixed field type */
    for (i = 0; phaseInfo.fixFieldList[i].type != USER_FIELD_END; ++i)
    {
        if (type == phaseInfo.fixFieldList[i].type)
        {
            data = *pData;
            mask = *pMask;

            fieldData = phaseInfo.fixFieldList[i].data_field;
            fieldMask = phaseInfo.fixFieldList[i].mask_field;

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
    dal_mango_acl_templateSelector_get(unit, blkIdx, &tmplteIdx);
    /* get field 'template ID' to know the template that the entry maps to */
    if ((ret = table_field_get(unit, phaseInfo.table, field, &data, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_pie_template_t));
    val = tmplteIdx.template_id[data];
    DAL_MANGO_PHASE_ACL_TO_PIE(phase, pphase);
    dal_mango_pie_phaseTemplate_get(unit, pphase, val, &tmplte);

    /* translate field from RTK superset view to DAL view */
    if ((ret = _dal_mango_acl_fieldInfo_get(phase, type, &fieldList,
            &dalFieldType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (!fieldList)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return RT_ERR_ACL_FIELD_TYPE;
    }

    /* search template to check all field types */
    fieldMatch = FALSE;
    for (fieldNum = 0; fieldNum < fieldList[dalFieldType].fieldNumber; ++fieldNum)
    {
        for (fieldIdx = 0; fieldIdx < DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
        {
            fieldLocation = &fieldList[dalFieldType].pField[fieldNum];

            /* check whether the user field type is pulled in template, partial match is allowed */
            if (fieldLocation->template_field_type == tmplte.field[fieldIdx])
            {
                uint32  fieldOfst, fieldLen;

                fieldMatch = TRUE;

                fieldOfst = fieldLocation->field_offset;
                fieldLen = fieldLocation->field_length;

                _dal_mango_acl_buf2Field_get(unit, fieldLocation, type,
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
        }   /* for (fieldIdx = 0; fieldIdx < DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx) */
    }   /* for (fieldNum = 0; fieldNum < fieldList[dal_field_type].fieldNumber; ++fieldNum) */

    /* no matched filed in template */
    if (fieldMatch != TRUE)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return RT_ERR_ACL_FIELD_TYPE;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_ruleEntryBufField_set */

/* Function Name:
 *      dal_mango_acl_ruleEntryField_get
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
dal_mango_acl_ruleEntryField_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint8 *pEntry_buffer, rtk_acl_fieldType_t type,
    uint8 *pData, uint8 *pMask)
{
    uint32  phyEntryId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    if ((ret = _dal_mango_acl_ruleEntryBufField_get(unit, phase, phyEntryId,
            type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntryField_get */

/* Function Name:
 *      dal_mango_acl_ruleEntryField_set
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
dal_mango_acl_ruleEntryField_set(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint8 *pEntry_buffer, rtk_acl_fieldType_t type,
    uint8 *pData, uint8 *pMask)
{
    uint32  phyEntryId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pEntry_buffer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    if ((ret = _dal_mango_acl_ruleEntryBufField_set(unit, phase, phyEntryId,
            type, (uint32 *)pEntry_buffer, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntryField_set */

/* Function Name:
 *      dal_mango_acl_ruleEntryField_read
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
dal_mango_acl_ruleEntryField_read(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type,
    uint8 *pData, uint8 *pMask)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_mango_acl_ruleEntryBufField_get(unit, phase,
            phyEntryId, type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "type:%u", type);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntryField_read */

/* Function Name:
 *      dal_mango_acl_ruleEntryField_write
 * Description:
 *      Write the field data to specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 *      pData     - field data
 *      pMask     - field mask
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
dal_mango_acl_ruleEntryField_write(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type,
    uint8 *pData, uint8 *pMask)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%u, type=%d",
            unit, phase, entry_idx, type);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_mango_acl_ruleEntryBufField_set(unit, phase,
            phyEntryId, type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "type:%u", type);
        return ret;
    }

    ACL_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = ACL_TBL_WRITE(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntryField_write */

/* Function Name:
 *      dal_mango_acl_ruleEntryField_check
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
dal_mango_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, type=%d",
            unit, phase, type);

    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    if ((ret = _dal_mango_acl_fieldType_chk(unit, phase, type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntryField_check */

/* Function Name:
 *      dal_mango_acl_ruleOperation_get
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
dal_mango_acl_ruleOperation_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_operation_t *pOperation)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    uint32                  fieldNot, fieldAnd1 = 0, fieldAnd2;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",
            unit, phase, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            fieldNot = MANGO_VACL_NOTtf;
            fieldAnd1 = MANGO_VACL_AND1tf;
            fieldAnd2 = MANGO_VACL_AND2tf;
            break;
        case ACL_PHASE_IACL:
            fieldNot = MANGO_IACL_NOTtf;
            fieldAnd1 = MANGO_IACL_AND1tf;
            fieldAnd2 = MANGO_IACL_AND2tf;
            break;
        case ACL_PHASE_EACL:
            fieldNot = MANGO_EACL_NOTtf;
            fieldAnd2 = MANGO_EACL_AND2tf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    ACL_SEM_LOCK(unit);

    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
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

    if (ACL_PHASE_EACL != phase)
    {
        if ((ret = table_field_get(unit, phaseInfo.table, fieldAnd1,
                &pOperation->aggr_1, (uint32 *)&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    if ((ret = table_field_get(unit, phaseInfo.table, fieldAnd2,
            &pOperation->aggr_2, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleOperation_get */

/* Function Name:
 *      dal_mango_acl_ruleOperation_set
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
dal_mango_acl_ruleOperation_set(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_operation_t *pOperation)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    uint32                  fieldNot, fieldAnd1 = 0, fieldAnd2;
    int32                   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",
            unit, phase, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pOperation->aggr_1 == ENABLED) &&
            (entry_idx % 2 != 0)), RT_ERR_ENTRY_INDEX);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    if (pOperation->aggr_2 == ENABLED)
    {
        if ((phyEntryId / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) % 2 != 0 ||
                (phyEntryId % 2) != 0)
            return RT_ERR_ENTRY_INDEX;
    }

    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            fieldNot = MANGO_VACL_NOTtf;
            fieldAnd1 = MANGO_VACL_AND1tf;
            fieldAnd2 = MANGO_VACL_AND2tf;
            break;
        case ACL_PHASE_IACL:
            fieldNot = MANGO_IACL_NOTtf;
            fieldAnd1 = MANGO_IACL_AND1tf;
            fieldAnd2 = MANGO_IACL_AND2tf;
            break;
        case ACL_PHASE_EACL:
            fieldNot = MANGO_EACL_NOTtf;
            fieldAnd2 = MANGO_EACL_AND2tf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, phaseInfo.table, fieldNot,
            &pOperation->reverse, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    if (ACL_PHASE_EACL != phase)
    {
        if ((ret = table_field_set(unit, phaseInfo.table, fieldAnd1,
                &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    if ((ret = table_field_set(unit, phaseInfo.table, fieldAnd2,
            &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);

    if ((ret = ACL_TBL_WRITE(unit, phaseInfo.table, phyEntryId,
            (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleOperation_set */

static int32
_dal_mango_acl_vRuleAction_get(uint32 unit, rtk_acl_id_t entry_idx,
    rtk_acl_vAct_t *pAction)
{
    acl_entry_t     entry;
    uint32          value, info;
    int32           ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, action_idx=%d", unit, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    osal_memset(pAction, 0x0, sizeof(rtk_acl_vAct_t));

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, MANGO_VACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_DROPtf,
            &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_DROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_YDROPtf,
            &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_YDROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_RDROPtf,
            &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_RDROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_FWDtf,
            &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_FWD_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_FWD_PORT_INFOtf,
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
            {
                if (DAL_MANGO_LOOPBACK_PORT == pAction->fwd_data.fwd_info)
                {
                    pAction->fwd_data.fwd_type = ACL_ACTION_FWD_LOOPBACK;
                }
                else
                    pAction->fwd_data.devID = (info >> 6) & 0xF;
            }

            break;
        case 5:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTMASK;
            pAction->fwd_data.fwd_info = info;
            break;
        case 6:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_UNICAST_ROUTING;
            pAction->fwd_data.fwd_info = info & 0x1FFF;
            switch (info >> 13)
            {
                case 0:
                    break;
                case 1:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_ECMP;
                    break;
                case 2:
                    switch (info & 0x3)
                    {
                        case 0:
                            pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_DROP;
                            break;
                        case 1:
                            pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU;
                            break;
                        case 2:
                            pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 7:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_DFLT_UNICAST_ROUTING;
            pAction->fwd_data.fwd_info = info & 0x1FFF;
            switch (info >> 13)
            {
                case 0:
                    break;
                case 1:
                    pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_ECMP;
                    break;
                case 2:
                    switch (info & 0x3)
                    {
                        case 0:
                            pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_DROP;
                            break;
                        case 1:
                            pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU;
                            break;
                        case 2:
                            pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
    }

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_FWD_CPU_PKT_FMTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    if (0 == value)
        pAction->fwd_data.fwd_cpu_fmt = ORIGINAL_PACKET;
    else
        pAction->fwd_data.fwd_cpu_fmt = MODIFIED_PACKET;

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_FWD_SA_LRNtf,
            &value, (uint32 *) &entry), errHandle, ret);
    if (1 == value)
        pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_SA_NOT_LEARN;

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_FWD_SELtf,
            &value, (uint32 *) &entry), errHandle, ret);
    if (1 == value)
        pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_OVERWRITE_DROP;

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_LOGtf,
            &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_MIRtf,
            &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_MIR_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_ORIGINAL;
            RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_MIR_IDXtf,
                    &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_CANCEL;
            break;
    }

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_METERtf,
            &pAction->meter_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_METER_IDXtf,
            &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_IVLANtf,
            &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_IVLAN_VID_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_IVLAN_VIDtf,
            &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_OVLANtf,
        &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_OVLAN_VID_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_OVLAN_VIDtf,
            &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    /* Ingress Inner Priority action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_IPRI_RMKtf,
            &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_IPRI_ACTtf,
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_IPRI_DATAtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_OPRI_RMKtf,
        &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_OPRI_ACTtf,
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_OPRI_DATAtf,
                    &pAction->outer_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_COPY_FROM_INNER;
            break;
        case 2:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_KEEP;
            break;
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_TAGSTStf,
            &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ITAGSTStf,
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

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_OTAGSTStf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_PRIOtf,
            &pAction->pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_INTERNAL_PRIOtf,
            &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* Bypass action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_BYPASStf,
            &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_BYPASS_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    pAction->bypass_data.ibc_sc = (value & 0x1);
    pAction->bypass_data.igr_stp = (value & 0x2) >> 0x1;
    pAction->bypass_data.igr_vlan = (value & 0x4) >> 0x2;

    /* Meta data action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_METADATAtf,
            &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);

    if (HAL_MAX_NUM_OF_METADATA(unit) < pAction->meta_data.data ||
            HAL_MAX_NUM_OF_METADATA(unit) < pAction->meta_data.mask)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
        return RT_ERR_INPUT;
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_METADATAtf,
            &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_METADATA_MSKtf,
            &pAction->meta_data.mask, (uint32 *) &entry), errHandle, ret);

    /* QID action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_QIDtf,
            &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_QID_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    if (0 == value)
        pAction->cpu_qid.act = ACL_ACTION_QID_IGR;
    else
        pAction->cpu_qid.act = ACL_ACTION_QID_CPU;

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_QIDtf,
            &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);

    /* Green Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_REMARKtf,
            &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_REMARK_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_REMARK_DATAtf,
            &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Yellow Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_YREMARKtf,
            &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_YREMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_YREMARK_DATAtf,
            &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Red Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_RREMARKtf,
        &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_RREMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_RREMARK_DATAtf,
            &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_INVT_IP_RSV_FLAGtf,
            &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "pAction=%x", *pAction);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
} /* end of dal_mango_acl_igrRuleAction_get */

static int32
_dal_mango_acl_iRuleAction_get( uint32 unit, rtk_acl_id_t entry_idx,
    rtk_acl_iAct_t *pAction)
{
    acl_entry_t     entry;
    uint32          value, info;
    int32           ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x",
            unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, MANGO_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_DROPtf,
            &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_DROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_YDROPtf,
            &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_YDROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_RDROPtf,
            &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_RDROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_FWDtf,
            &pAction->fwd_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_FWD_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_FWD_PORT_INFOtf,
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
            {
                if (DAL_MANGO_LOOPBACK_PORT == pAction->fwd_data.fwd_info)
                {
                    pAction->fwd_data.fwd_type = ACL_ACTION_FWD_LOOPBACK;
                }
                else
                    pAction->fwd_data.devID = (info >> 6) & 0xF;
            }

            break;
        case 5:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTMASK;
            pAction->fwd_data.fwd_info = info;
            break;
        case 6:
            pAction->fwd_data.fwd_type = ACL_ACTION_FWD_FILTERING;
            pAction->fwd_data.fwd_info = info;
            break;
    }

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_FWD_CPU_PKT_FMTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    if (0 == value)
        pAction->fwd_data.fwd_cpu_fmt = ORIGINAL_PACKET;
    else
        pAction->fwd_data.fwd_cpu_fmt = MODIFIED_PACKET;

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_FWD_SELtf,
            &value, (uint32 *) &entry), errHandle, ret);
    if (1 == value)
        pAction->fwd_data.flags |= RTK_ACL_FWD_FLAG_OVERWRITE_DROP;

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_LOGtf,
            &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    /* Mirror action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_MIRtf,
            &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_MIR_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_ORIGINAL;
            RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_MIR_IDXtf,
                    &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_MODIFIED;
            RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_MIR_IDXtf,
                    &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
            break;
        case 2:
            pAction->mirror_data.mirror_type = ACL_ACTION_MIRROR_CANCEL;
            break;
    }

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_METERtf,
            &pAction->meter_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_METER_IDXtf,
            &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress I-VID/TPID assignment action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_IVLANtf,
            &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_IVLAN_VID_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_IVLAN_VIDtf,
            &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_IVLAN_TPID_ACTtf,
            &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_IVLAN_TPID_IDXtf,
            &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_OVLANtf,
            &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OVLAN_VID_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OVLAN_VIDtf,
            &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OVLAN_TPID_ACTtf,
            &pAction->outer_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OVLAN_TPID_IDXtf,
            &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress Inner Priority action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_IPRItf,
            &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_IPRI_ACTtf,
        &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_IPRI_DATAtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_OPRItf,
            &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OPRI_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OPRI_DATAtf,
                    &pAction->outer_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_COPY_FROM_INNER;
            break;
        case 2:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_KEEP;
            break;
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_TAGSTStf,
            &pAction->tag_sts_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ITAGSTStf,
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

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_OTAGSTStf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_PRIOtf,
            &pAction->pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_INTERNAL_PRIOtf,
            &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);

    /* Bypass action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_BYPASStf,
            &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_BYPASS_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    pAction->bypass_data.ibc_sc = (value & 0x1);
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_BYPASS_ACT1tf,
            &value, (uint32 *) &entry), errHandle, ret);
    pAction->bypass_data.egr_vlan = (value & 0x1);

    /* Meta data action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_METADATAtf,
            &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_METADATAtf,
            &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_METADATA_MSKtf,
            &pAction->meta_data.mask, (uint32 *) &entry), errHandle, ret);

    /* QID action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_CPUQIDtf,
            &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_CPUQIDtf,
            &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);

    /* Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_REMARKtf,
            &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_REMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_REMARK_DATAtf,
            &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Yellow Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_YREMARKtf,
            &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_YREMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_YREMARK_DATAtf,
            &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Red Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_RREMARKtf,
            &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_RREMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_RREMARK_DATAtf,
            &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_INVT_IP_RSV_FLAGtf,
            &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}   /* end of _dal_mango_acl_iRuleAction_get */

static int32
_dal_mango_acl_eRuleAction_get(uint32 unit, rtk_acl_id_t entry_idx,
    rtk_acl_eAct_t *pAction)
{
    acl_entry_t     entry;
    uint32          value;
    int32           ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x",
            unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, MANGO_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_DROPtf,
            &pAction->green_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_DROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_YDROPtf,
            &pAction->yellow_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_YDROP_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_RDROPtf,
            &pAction->red_drop_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_RDROP_ACTtf,
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

    /* Log action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_LOGtf,
            &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    /* Meter action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_METERtf,
            &pAction->meter_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_METER_IDXtf,
            &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress I-VID/TPID assignment action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_IVLANtf,
            &pAction->inner_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_IVLAN_VID_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_IVLAN_VIDtf,
            &pAction->inner_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_IVLAN_TPID_ACTtf,
            &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_IVLAN_TPID_IDXtf,
            &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress O-VID assignment action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_OVLANtf,
            &pAction->outer_vlan_assign_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_OVLAN_VID_ACTtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_OVLAN_VIDtf,
            &pAction->outer_vlan_data.vid_value, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_OVLAN_TPID_ACTtf,
            &pAction->outer_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_OVLAN_TPID_IDXtf,
            &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);

    /* Egress Inner Priority action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_IPRItf,
            &pAction->inner_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_IPRI_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_IPRI_DATAtf,
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
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_OPRItf,
            &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_OPRI_ACTtf,
            &value, (uint32 *) &entry), errHandle, ret);
    switch (value)
    {
        case 0:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_OPRI_DATAtf,
                    &pAction->outer_pri_data.pri, (uint32 *) &entry), errHandle, ret);
            break;
        case 1:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_COPY_FROM_INNER;
            break;
        case 2:
            pAction->outer_pri_data.act = ACL_ACTION_INNER_PRI_KEEP;
            break;
    }

    /* Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_REMARKtf,
            &pAction->rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_REMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_REMARK_DATAtf,
            &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Yellow Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_YREMARKtf,
            &pAction->yellow_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_YREMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_YREMARK_DATAtf,
            &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

    /*Red Remark action */
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_RREMARKtf,
            &pAction->red_rmk_en, (uint32 *) &entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_RREMARK_ACTtf,
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
    }
    RT_ERR_HDL(table_field_get(unit, MANGO_EACLt, MANGO_EACL_RREMARK_DATAtf,
            &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}   /* end of _dal_mango_acl_eRuleAction_get */

/* Function Name:
 *      dal_mango_acl_ruleAction_get
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
dal_mango_acl_ruleAction_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_action_t *pAction)
{
    uint32  phyEntryId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",
            unit, phase, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    osal_memset(pAction, 0, sizeof(rtk_acl_action_t));

    switch (phase)
    {
        case ACL_PHASE_VACL:
            ret = _dal_mango_acl_vRuleAction_get(unit, phyEntryId, &pAction->vact);
            break;
        case ACL_PHASE_IACL:
            ret = _dal_mango_acl_iRuleAction_get(unit, phyEntryId, &pAction->iact);
            break;
        case ACL_PHASE_EACL:
            ret = _dal_mango_acl_eRuleAction_get(unit, phyEntryId, &pAction->eact);
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleAction_get */

static int32
_dal_mango_acl_vRuleAction_set(uint32 unit, rtk_acl_id_t entry_idx,
    rtk_acl_vAct_t *pAction)
{
    acl_entry_t     entry;
    uint32          value, info = 0;
    uint32          multicast_tableSize;
    uint32          fwd_unit;
    int32           ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x",
            unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    osal_memset(&entry, 0, sizeof(acl_entry_t));
    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, MANGO_VACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_DROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_DROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_YDROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_YDROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* red drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_RDROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_RDROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_FWDtf,
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
                        RT_ERR_CHK(dal_mango_stack_devId_get(unit, &fwd_unit), ret);
                    }
                    else
                    {
                        fwd_unit = pAction->fwd_data.devID;
                    }

                    info |= ((fwd_unit & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                value = 3;

                if ((ret = table_size_get(unit, MANGO_MC_PMSKt,
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
            case ACL_ACTION_FWD_LOOPBACK:
                value = 4;
                info = DAL_MANGO_LOOPBACK_PORT;
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
                        RT_ERR_CHK(dal_mango_stack_devId_get(unit, &fwd_unit), ret);
                    }
                    else
                    {
                        fwd_unit = pAction->fwd_data.devID;
                    }

                    info |= ((pAction->fwd_data.devID & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                value = 5;

                if ((ret = table_size_get(unit, MANGO_MC_PMSKt,
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

                info = pAction->fwd_data.fwd_info & 0x1FFF;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_ECMP)
                    info |= (1 << 13);
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_DROP)
                {
                    info = (2 << 13) | 0;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU)
                {
                    info = (2 << 13) | 1;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU)
                {
                    info = (2 << 13) | 2;
                }
                break;
            case ACL_ACTION_FWD_DFLT_UNICAST_ROUTING:
                value = 7;

                info = pAction->fwd_data.fwd_info & 0x1FFF;
                if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_ECMP)
                    info |= (1 << 13);
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_DROP)
                {
                    info = (2 << 13) | 0;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU)
                {
                    info = (2 << 13) | 1;
                }
                else if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU)
                {
                    info = (2 << 13) | 2;
                }
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
                return RT_ERR_INPUT;
        }   /* end of switch (pAction->fwd_data.fwd_type) */
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_FWD_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_FWD_PORT_INFOtf,
                &info, (uint32 *) &entry), errHandle, ret);

        switch (pAction->fwd_data.fwd_cpu_fmt)
        {
            case ORIGINAL_PACKET:
                value = 0;
                break;
            case MODIFIED_PACKET:
                value = 1;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_FWD_CPU_PKT_FMTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_SA_NOT_LEARN)
            value = 1;
        else
            value = 0;
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_FWD_SA_LRNtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_OVERWRITE_DROP)
            value = 1;
        else
            value = 0;
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_FWD_SELtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_LOGtf,
            &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_MIRtf,
            &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->mirror_en)
    {
        if (pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_MIR_IDXtf,
                &pAction->mirror_data.mirror_set_idx, (uint32 *) &entry), errHandle, ret);
        switch (pAction->mirror_data.mirror_type)
        {
            case ACL_ACTION_MIRROR_ORIGINAL:
                value = 0;
                break;
            case ACL_ACTION_MIRROR_CANCEL:
                value = 1;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_MIR_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_METERtf,
            &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meter_en)
    {
        if (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_METER_IDXtf,
                &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress I-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_IVLANtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_IVLAN_VID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        value = pAction->inner_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_IVLAN_VIDtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress O-VID assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_OVLANtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_OVLAN_VID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        value = pAction->outer_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_OVLAN_VIDtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_IPRI_RMKtf,
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
                RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_IPRI_DATAtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_IPRI_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Outer PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_OPRI_RMKtf,
            &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_pri_en)
    {
        switch (pAction->outer_pri_data.act)
        {
            case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                if (pAction->outer_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->outer_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_OPRI_DATAtf,
                        &value, (uint32 *) &entry), errHandle, ret);

                value = 0;

                break;
            case ACL_ACTION_INNER_PRI_COPY_FROM_INNER:
                value = 1;
                break;
            case ACL_ACTION_INNER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_OPRI_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_TAGSTStf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ITAGSTStf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_OTAGSTStf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Priority action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_PRIOtf,
            &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->pri_en)
    {
        if (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_INTERNAL_PRIOtf,
                &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);
    }

    /* Bypass action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_BYPASStf,
            &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->bypass_en)
    {
        value = (pAction->bypass_data.ibc_sc & 0x1) |
                ((pAction->bypass_data.igr_stp & 0x1) << 0x1) |
                ((pAction->bypass_data.igr_vlan & 0x1) << 0x2);
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_BYPASS_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meta data action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_METADATAtf,
            &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meta_data_en)
    {
        if (HAL_MAX_NUM_OF_METADATA(unit) < pAction->meta_data.data ||
                HAL_MAX_NUM_OF_METADATA(unit) < pAction->meta_data.mask)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_METADATAtf,
                &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_METADATA_MSKtf,
                &pAction->meta_data.mask, (uint32 *) &entry), errHandle, ret);
    }

    /* QID action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_QIDtf,
            &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->cpu_qid_en)
    {
        switch (pAction->cpu_qid.act)
        {
            case ACL_ACTION_QID_IGR:
                if (HAL_MAX_NUM_OF_IGR_QUEUE(unit) <= pAction->cpu_qid.qid)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 0;
                break;
            case ACL_ACTION_QID_CPU:
                if (HAL_MAX_NUM_OF_CPU_QUEUE(unit) <= pAction->cpu_qid.qid)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 1;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_QID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_QIDtf,
                &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);
    }

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_REMARKtf,
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
                if (pAction->rmk_data.rmk_info > 2)
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = 3;
                break;
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_REMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_REMARK_DATAtf,
                &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->rmk_en) */

    /* Yellow Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_YREMARKtf,
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
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "act %d",
                        pAction->yellow_rmk_data.rmk_act);
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_YREMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_YREMARK_DATAtf,
                &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->yellow_rmk_en) */

    /*Red Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_RREMARKtf,
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
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_RREMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_RREMARK_DATAtf,
                &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->red_rmk_en) */

    RT_ERR_HDL(table_field_set(unit, MANGO_VACLt, MANGO_VACL_ACT_MSK_INVT_IP_RSV_FLAGtf,
            &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_WRITE(unit, MANGO_VACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
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
} /* end of dal_mango_acl_igrRuleAction_set */

static int32
_dal_mango_acl_iRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_iAct_t     *pAction)
{
    acl_entry_t     entry;
    int32           ret;
    uint32          value, info = 0;
    uint32          multicast_tableSize;
    uint32          fwd_unit;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    osal_memset(&entry, 0, sizeof(acl_entry_t));
    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, MANGO_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "entry_idx %x", entry_idx);
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_DROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_DROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_YDROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_YDROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* red drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_RDROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_RDROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Forwarding action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_FWDtf,
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
                        RT_ERR_CHK(dal_mango_stack_devId_get(unit, &fwd_unit), ret);
                    }
                    else
                    {
                        fwd_unit = pAction->fwd_data.devID;
                    }

                    info |= ((fwd_unit & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                value = 3;

                if ((ret = table_size_get(unit, MANGO_MC_PMSKt,
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
            case ACL_ACTION_FWD_LOOPBACK:
                value = 4;
                info = DAL_MANGO_LOOPBACK_PORT;
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
                        RT_ERR_CHK(dal_mango_stack_devId_get(unit, &fwd_unit), ret);
                    }
                    else
                    {
                        fwd_unit = pAction->fwd_data.devID;
                    }
                    info |= ((fwd_unit & 0xF) << 6);
                }

                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                value = 5;

                if ((ret = table_size_get(unit, MANGO_MC_PMSKt,
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
            case ACL_ACTION_FWD_FILTERING:
                value = 6;
                info = pAction->fwd_data.fwd_info;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "fwd_data.fwd_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_FWD_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_FWD_PORT_INFOtf,
                &info, (uint32 *) &entry), errHandle, ret);

        switch (pAction->fwd_data.fwd_cpu_fmt)
        {
            case ORIGINAL_PACKET:
                value = 0;
                break;
            case MODIFIED_PACKET:
                value = 1;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_FWD_CPU_PKT_FMTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->fwd_data.flags & RTK_ACL_FWD_FLAG_OVERWRITE_DROP)
            value = 1;
        else
            value = 0;
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_FWD_SELtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->fwd_en) */

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_LOGtf,
            &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    /* Mirror action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_MIRtf,
            &pAction->mirror_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->mirror_en)
    {
        if (pAction->mirror_data.mirror_set_idx >= HAL_MAX_NUM_OF_MIRROR(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_MIR_IDXtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_MIR_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_METERtf,
            &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meter_en)
    {
        if (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_METER_IDXtf,
                &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress IVLAN assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_IVLANtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_IVLAN_VID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        value = pAction->inner_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_IVLAN_VIDtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (ENABLED == pAction->inner_vlan_data.tpid_assign)
        {
            if (HAL_TPID_ENTRY_IDX_MAX(unit) < pAction->inner_vlan_data.tpid_idx)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
            }
            RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_IVLAN_TPID_IDXtf,
                    &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);
            value = 1;
        }
        else
            value = 0;

        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_IVLAN_TPID_ACTtf,
                &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress OVLAN assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_OVLANtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OVLAN_VID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        value = pAction->outer_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OVLAN_VIDtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (ENABLED == pAction->outer_vlan_data.tpid_assign)
        {
            if (HAL_TPID_ENTRY_IDX_MAX(unit) < pAction->outer_vlan_data.tpid_idx)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
            }
            RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OVLAN_TPID_IDXtf,
                    &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);
            value = 1;
        }
        else
            value = 0;

        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OVLAN_TPID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_IPRItf,
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
                RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_IPRI_DATAtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_IPRI_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Outer PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_OPRItf,
            &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_pri_en)
    {
        switch (pAction->outer_pri_data.act)
        {
            case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                if (pAction->outer_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->outer_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OPRI_DATAtf,
                        &value, (uint32 *) &entry), errHandle, ret);

                value = 0;
                break;
            case ACL_ACTION_INNER_PRI_COPY_FROM_INNER:
                value = 1;
                break;
            case ACL_ACTION_INNER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OPRI_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner/Outer Tag Status action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_TAGSTStf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ITAGSTStf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_OTAGSTStf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Priority action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_PRIOtf,
            &pAction->pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->pri_en)
    {
        if (pAction->pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_INTERNAL_PRIOtf,
                &pAction->pri_data.pri, (uint32 *) &entry), errHandle, ret);
    }

    /* Bypass action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_BYPASStf,
            &pAction->bypass_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->bypass_en)
    {
        value = (pAction->bypass_data.ibc_sc & 0x1);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_BYPASS_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        value = (pAction->bypass_data.egr_vlan & 0x1);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_BYPASS_ACT1tf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Meta data action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_METADATAtf,
            &pAction->meta_data_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meta_data_en)
    {
        if (HAL_MAX_NUM_OF_METADATA(unit) < pAction->meta_data.data ||
                HAL_MAX_NUM_OF_METADATA(unit) < pAction->meta_data.mask)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_METADATAtf,
                &pAction->meta_data.data, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_METADATA_MSKtf,
                &pAction->meta_data.mask, (uint32 *) &entry), errHandle, ret);
    }

    /* QID action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_CPUQIDtf,
            &pAction->cpu_qid_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->cpu_qid_en)
    {
        if (HAL_MAX_NUM_OF_CPU_QUEUE(unit) <= pAction->cpu_qid.qid)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_CPUQIDtf,
            &pAction->cpu_qid.qid, (uint32 *) &entry), errHandle, ret);
    }

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_REMARKtf,
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
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_REMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_REMARK_DATAtf,
                &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->rmk_en) */

    /* Yellow Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_YREMARKtf,
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
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_YREMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_YREMARK_DATAtf,
                &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->yellow_rmk_en) */

    /*Red Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_RREMARKtf,
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
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_RREMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_RREMARK_DATAtf,
                &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->red_rmk_en) */

    RT_ERR_HDL(table_field_set(unit, MANGO_IACLt, MANGO_IACL_ACT_MSK_INVT_IP_RSV_FLAGtf,
            &pAction->invert_en, (uint32 *) &entry), errHandle, ret);

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_WRITE(unit, MANGO_IACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}   /* end of _dal_mango_acl_iRuleAction_set */

static int32
_dal_mango_acl_eRuleAction_set(
    uint32                  unit,
    rtk_acl_id_t            entry_idx,
    rtk_acl_eAct_t     *pAction)
{
    acl_entry_t     entry;
    int32           ret;
    uint32          value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, entry_idx=%d, pAction=%x", unit, entry_idx, pAction);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, MANGO_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    /* green drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_DROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_DROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* yellow drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_YDROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_YDROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* red drop action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_RDROPtf,
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
                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_RDROP_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Log action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_LOGtf,
            &pAction->stat_en, (uint32 *) &entry), errHandle, ret);

    /* Meter action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_METERtf,
            &pAction->meter_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->meter_en)
    {
        if (pAction->meter_data.meter_idx >= HAL_MAX_NUM_OF_METERING(unit))
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_METER_IDXtf,
                &pAction->meter_data.meter_idx, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress IVLAN assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_IVLANtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_IVLAN_VID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->inner_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }

        value = pAction->inner_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_IVLAN_VIDtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (ENABLED == pAction->inner_vlan_data.tpid_assign)
        {
            if (HAL_TPID_ENTRY_IDX_MAX(unit) < pAction->inner_vlan_data.tpid_idx)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
            }
            RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_IVLAN_TPID_IDXtf,
                    &pAction->inner_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);
            value = 1;
        }
        else
            value = 0;

        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_IVLAN_TPID_ACTtf,
                &pAction->inner_vlan_data.tpid_assign, (uint32 *) &entry), errHandle, ret);
    }

    /* Ingress OVLAN assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_OVLANtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_OVLAN_VID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (pAction->outer_vlan_data.vid_value > RTK_VLAN_ID_MAX)
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
            return RT_ERR_INPUT;
        }
        value = pAction->outer_vlan_data.vid_value;
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_OVLAN_VIDtf,
                &value, (uint32 *) &entry), errHandle, ret);

        if (ENABLED == pAction->outer_vlan_data.tpid_assign)
        {
            if (HAL_TPID_ENTRY_IDX_MAX(unit) < pAction->outer_vlan_data.tpid_idx)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
            }
            RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_OVLAN_TPID_IDXtf,
                    &pAction->outer_vlan_data.tpid_idx, (uint32 *) &entry), errHandle, ret);
            value = 1;
        }
        else
            value = 0;

        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_OVLAN_TPID_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Inner PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_IPRItf,
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
                RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_IPRI_DATAtf,
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
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_IPRI_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Outer PRI assignment action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_OPRItf,
            &pAction->outer_pri_en, (uint32 *) &entry), errHandle, ret);
    if (ENABLED == pAction->outer_pri_en)
    {
        switch (pAction->outer_pri_data.act)
        {
            case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                if (pAction->outer_pri_data.pri > HAL_INTERNAL_PRIORITY_MAX(unit))
                {
                    RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                    return RT_ERR_INPUT;
                }
                value = pAction->outer_pri_data.pri;
                RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_OPRI_DATAtf,
                        &value, (uint32 *) &entry), errHandle, ret);

                value = 0;
                break;
            case ACL_ACTION_INNER_PRI_COPY_FROM_INNER:
                value = 1;
                break;
            case ACL_ACTION_INNER_PRI_KEEP:
                value = 2;
                break;
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "outer_vlan_data.vid_assign_type error");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_OPRI_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
    }

    /* Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_REMARKtf,
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
            default:
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_REMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_REMARK_DATAtf,
                &pAction->rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->rmk_en) */

    /* Yellow Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_YREMARKtf,
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
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_YREMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_YREMARK_DATAtf,
                &pAction->yellow_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->yellow_rmk_en) */

    /*Red Remark action */
    RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_ACT_MSK_RREMARKtf,
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
            default:
                RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_ACL), "");
                return RT_ERR_INPUT;
        }
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_RREMARK_ACTtf,
                &value, (uint32 *) &entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, MANGO_EACLt, MANGO_EACL_RREMARK_DATAtf,
                &pAction->red_rmk_data.rmk_info, (uint32 *) &entry), errHandle, ret);
    }   /* end of if (ENABLED == pAction->red_rmk_en) */

    /* set entry to chip */
    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_WRITE(unit, MANGO_EACLt, entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

errHandle:
    RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    return ret;
}   /* end of _dal_mango_acl_eRuleAction_set */

/* Function Name:
 *      dal_mango_acl_ruleAction_set
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
dal_mango_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    uint32  phyEntryId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d",
            unit, phase, entry_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            ret = _dal_mango_acl_vRuleAction_set(unit, phyEntryId, &pAction->vact);
            break;
        case ACL_PHASE_IACL:
            ret = _dal_mango_acl_iRuleAction_set(unit, phyEntryId, &pAction->iact);
            break;
        case ACL_PHASE_EACL:
            ret = _dal_mango_acl_eRuleAction_set(unit, phyEntryId, &pAction->eact);
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_acl_portPhaseLookupEnable_get
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
dal_mango_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable)
{
    uint32  field, val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d, phase=%d",
            unit, port, phase);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case ACL_PHASE_VACL:
        case ACL_PHASE_IACL:
            field = MANGO_IGR_LUTf;
            break;
        case ACL_PHASE_EACL:
            field = MANGO_EGR_LUTf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MANGO_ACL_PORT_LOOKUP_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, &val)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_mango_acl_portPhaseLookupEnable_get */

/* Function Name:
 *      dal_mango_acl_portPhaseLookupEnable_set
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
dal_mango_acl_portPhaseLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable)
{
    uint32  field, val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, port=%d, phase=%d, enable=%d",
            unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (phase)
    {
        case ACL_PHASE_VACL:
        case ACL_PHASE_IACL:
            field = MANGO_IGR_LUTf;
            break;
        case ACL_PHASE_EACL:
            field = MANGO_EGR_LUTf;
            break;
        default:
            return RT_ERR_ACL_PHASE;
    }

    if (ENABLED == enable)
        val = 1;
    else
        val = 0;

    ACL_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MANGO_ACL_PORT_LOOKUP_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, &val)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_acl_portPhaseLookupEnable_set */

/* Function Name:
 *      dal_mango_acl_templateSelector_get
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
 *      RT_ERR_ACL_BLOCK_INDEX  - invalid block index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_acl_templateSelector_get(uint32 unit, uint32 block_idx, rtk_acl_templateIdx_t *pTemplate_idx)
{
    int32           ret;
    rtk_pie_phase_t phase;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d", unit, block_idx);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_ACL_BLOCK_INDEX);
    RT_PARAM_CHK((NULL == pTemplate_idx), RT_ERR_NULL_POINTER);

    if ((ret = dal_mango_pie_phase_get(unit, block_idx, &phase)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case PIE_PHASE_VACL:
        case PIE_PHASE_IACL:
        case PIE_PHASE_EACL:
            break;
        default:
            return RT_ERR_ACL_BLOCK_INDEX;
    }

    if ((ret = _dal_mango_pie_templateSelector_get(unit, block_idx, &pTemplate_idx->template_id[0],\
        &pTemplate_idx->template_id[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "template1_idx=%d, template2_idx=%d",\
        pTemplate_idx->template_id[0], pTemplate_idx->template_id[1]);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_templateSelector_get */

/* Function Name:
 *      dal_mango_acl_templateSelector_set
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
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_ACL_BLOCK_INDEX      - invalid block index
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_acl_templateSelector_set(uint32 unit, uint32 block_idx, rtk_acl_templateIdx_t template_idx)
{
    int32           ret;
    uint32          i;
    rtk_pie_phase_t phase;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, block_idx=%d, template1_idx=%d, template2_idx=%d", \
        unit, block_idx, template_idx.template_id[0], template_idx.template_id[1]);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_ACL_BLOCK_INDEX);
    for (i = 0; i < HAL_MAX_NUM_OF_PIE_BLOCK_TEMPLATESELECTOR(unit); ++i)
    {
        RT_PARAM_CHK((template_idx.template_id[i] >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    }

    if ((ret = dal_mango_pie_phase_get(unit, block_idx, &phase)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    switch (phase)
    {
        case PIE_PHASE_VACL:
        case PIE_PHASE_IACL:
        case PIE_PHASE_EACL:
            break;
        default:
            return RT_ERR_ACL_BLOCK_INDEX;
    }

    if ((ret = _dal_mango_pie_templateSelector_set(unit, block_idx, template_idx.template_id[0],\
        template_idx.template_id[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_templateSelector_set */

/* Function Name:
 *      dal_mango_acl_ruleHitIndication_get
 * Description:
 *      Get the PIE rule hit indication.
 * Input:
 *      unit        - unit id
 *      phase       - PIE lookup phase
 *      entry_idx   - logic entry index
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
dal_mango_acl_ruleHitIndication_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_pie_id_t entry_idx, uint32 reset, uint32 *pIsHit)
{
    uint32  phyEntryId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d,reset=%d",
            unit, phase, entry_idx, reset);

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    if ((ret = _dal_mango_pie_entryHitSts_get(unit, phyEntryId, reset, pIsHit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pIsHit=%d", *pIsHit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleHitIndication_get */

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
dal_mango_acl_ruleHitIndicationMask_get(uint32 unit, rtk_acl_phase_t phase,
    uint32 reset, rtk_acl_hitMask_t *pHitMask)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phaseBlkCnt = 0;


    // global hit has 4 register
    // per global hit register has 32 bits
    // per bits is 32 entries

    uint32  blk_max = DAL_MANGO_MAX_HITINDICATION_BLK_ENTRY;          // total 32 blocks
    uint32  blk_entry_max = DAL_MANGO_MAX_HITINDICATION_BLK_ENTRY * DAL_MANGO_ACL_GLB_HIT_GRP_NUM;    // per block has 128 entries
    // every bit of global hit register represents 32 entries
    uint32  glb_hit_entry_num_per_bit = DAL_MANGO_ACL_GLB_HIT_ENTRY_PER_BIT;
    // every block has 4 bits in global reigster (128/32=4)
    uint32  glb_hit_bit_num_per_blk = blk_entry_max / glb_hit_entry_num_per_bit;
    // every register address has 32 bits. Then one address has 8 block hit info (32/4=8)
    uint32  glb_hit_blk_num_per_addr = ((32 * glb_hit_entry_num_per_bit) / blk_entry_max);
    // global hit register has 4 addresses (32/8=4)
    uint32  glb_hit_array_max = blk_max / glb_hit_blk_num_per_addr;
    // every register address has 32 bits. So every entry hit register address has 32 entries.
    uint32  entry_hit_num_per_addr = DAL_MANGO_ACL_GLB_HIT_ENTRY_PER_BIT;
    uint32  glb_reg_read_idx = glb_hit_array_max;   // record read global register array index
    uint32  glb_hit_addr = 0;   // get global register address by block index
    uint32  blk_entry_hit_addr_base = 0;
    uint32  blk_entry_ofst = 0;
    uint32  entryOfst = 0;
    uint32  logical_entry_idx = 0;

    uint32  blk = 0, j = 0, k = 0;
    rtk_pie_phase_t pieBlkPhase = ACL_PHASE_END;
    rtk_acl_phase_t blkPhase = ACL_PHASE_END;
    rtk_bitmap_t glb_hit_sts = 0;
    rtk_bitmap_t glb_blk_hit_sts = 0;
    rtk_bitmap_t blk_entry_hit_sts = 0;


    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pHitMask), RT_ERR_NULL_POINTER);
    if (phase != ACL_PHASE_VACL && phase != ACL_PHASE_IACL && phase != ACL_PHASE_EACL)
        return RT_ERR_ACL_PHASE;

    BITMAP_RESET(pHitMask->bits, BITMAP_ARRAY_CNT(RTK_MAX_NUM_OF_ACL_ENTRY));

    ACL_SEM_LOCK(unit);


    for (blk = 0; blk < blk_max; ++blk)
    {
        if ( (ret = dal_mango_pie_phase_get(unit, blk, &pieBlkPhase)) != RT_ERR_OK )
        {
            ACL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        DAL_MANGO_PHASE_PIE_TO_ACL(pieBlkPhase, blkPhase);

        if ( blkPhase == phase )
        {
            // get global register address by block index
            glb_hit_addr = blk / glb_hit_blk_num_per_addr;
            // check global register address if read
            if (glb_reg_read_idx != glb_hit_addr)
            {
                /* get global hit */
                if ((ret = reg_array_field_read(unit,
                                MANGO_PIE_GLB_HIT_INDICATIONr,
                                REG_ARRAY_INDEX_NONE,
                                glb_hit_addr,
                                MANGO_GLB_INDICATIONf,
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
                                        MANGO_PIE_RULE_HIT_INDICATIONr,
                                        REG_ARRAY_INDEX_NONE,
                                        blk_entry_hit_addr_base+j,
                                        MANGO_RULE_INDICATIONf,
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
                                            MANGO_PIE_RULE_HIT_INDICATIONr,
                                            REG_ARRAY_INDEX_NONE,
                                            blk_entry_hit_addr_base+j,
                                            MANGO_RULE_INDICATIONf,
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

    }

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;

}   /* end of dal_mango_acl_ruleHitIndicationMask_get */


static int32
_dal_mango_acl_tblEntry_del(uint32 unit, uint32 entryIdx)
{
    acl_entry_t     aclEntry;
    log_entry_t     cntrEntry;
    uint32          hitSts;
    int32           ret;

    ACL_DEL_DBG("%s entryIdx %u\n", __func__, entryIdx);

    osal_memset(&aclEntry, 0, sizeof(acl_entry_t));
    osal_memset(&cntrEntry, 0, sizeof(log_entry_t));

    RT_ERR_CHK(ACL_TBL_WRITE(unit, MANGO_VACLt, entryIdx, (uint32 *)&aclEntry), ret);
    RT_ERR_CHK(table_write(unit, MANGO_FLOW_CNTRt, entryIdx, (uint32 *) &cntrEntry), ret);
    RT_ERR_CHK(_dal_mango_pie_entryHitSts_get(unit, entryIdx, TRUE, (uint32 *)&hitSts), ret);

    return ret;
}   /* end of _dal_mango_acl_tblEntry_del */

static int32
_dal_mango_acl_entry_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx,
    rtk_bitmap_t *pDeletedInfo)
{
    dal_mango_acl_aggInfo_t     entryAggInfo;
    uint32                      entryIdx, phyEntryIdx, startIdx, endIdx;
    int32                       ret;

    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDeletedInfo), RT_ERR_NULL_POINTER);

    ACL_DEL_DBG("%s startIdx %u endIdx %u\n", __func__, pClrIdx->start_idx, pClrIdx->end_idx);

    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_PIE_CLEAR_INDEX);

    /* entry index is same physical/logic type */
    if (PIE_ENTRY_IS_PHYSICAL_TYPE(pClrIdx->start_idx) !=
            PIE_ENTRY_IS_PHYSICAL_TYPE(pClrIdx->end_idx))
    {
        return RT_ERR_ENTRY_INDEX;
    }

    startIdx    = pClrIdx->start_idx;
    endIdx      = pClrIdx->end_idx;

    BITMAP_RESET(pDeletedInfo, BITMAP_ARRAY_CNT(HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)));

    /* get physical entry index */
    for (entryIdx = startIdx; entryIdx <= endIdx; ++entryIdx)
    {
        DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entryIdx, phyEntryIdx);

        if (BITMAP_IS_SET(pDeletedInfo, phyEntryIdx))
        {
            ACL_DEL_DBG("Skip entry %d has deleted\n", phyEntryIdx);
            continue;
        }

        RT_ERR_CHK(_dal_mango_acl_entryAggType_get(unit, phyEntryIdx, &entryAggInfo), ret);

        ACL_DEL_DBG("agg2Sts %u\n", entryAggInfo.agg2Sts);
        ACL_DEL_DBG("agg1Sts %u\n", entryAggInfo.agg1Sts);
        ACL_DEL_DBG("baseEntryIdx %u\n", entryAggInfo.baseEntryIdx);
        ACL_DEL_DBG("agg1PartnerEntryIdx %u\n", entryAggInfo.agg1PartnerEntryIdx);
        ACL_DEL_DBG("agg2PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerEntryIdx);
        ACL_DEL_DBG("agg2PartnerAgg1PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerAgg1PartnerEntryIdx);

        RT_ERR_CHK(_dal_mango_acl_aggEntry_del(unit, phase, &entryAggInfo), ret);

        BITMAP_SET(pDeletedInfo, entryAggInfo.baseEntryIdx);

        if ((entryAggInfo.agg2Sts || entryAggInfo.agg1Sts) && (ACL_PHASE_EACL != phase))
        {
            BITMAP_SET(pDeletedInfo, entryAggInfo.agg1PartnerEntryIdx);
        }

        if (entryAggInfo.agg2Sts)
        {
            /* Delete Agg2PartnerAgg1Partner entry */
            if (ACL_PHASE_EACL != phase)
            {
                BITMAP_SET(pDeletedInfo, entryAggInfo.agg2PartnerAgg1PartnerEntryIdx);
            }
            BITMAP_SET(pDeletedInfo, entryAggInfo.agg2PartnerEntryIdx);
        }
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_entry_del */

/* Function Name:
 *      dal_mango_acl_rule_del
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
dal_mango_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx)
{
    rtk_bitmap_t    *pDeletedInfo;
    int32           ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_PIE_CLEAR_INDEX);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,start_idx=%d,end_idx=%d",
            unit, phase, pClrIdx->start_idx, pClrIdx->end_idx);

    ACL_SEM_LOCK(unit);

    pDeletedInfo = osal_alloc(BITMAP_ARRAY_SIZE(HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)));
    if (!pDeletedInfo)
    {
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_DAL|MOD_ACL), "alloc memory fail");
        ACL_SEM_UNLOCK(unit);
        return RT_ERR_MEM_ALLOC;
    }

    RT_ERR_HDL(_dal_mango_acl_entry_del(unit, phase, pClrIdx, pDeletedInfo), ERR, ret);

ERR:
    osal_free(pDeletedInfo);

    ACL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_acl_rule_del */

static int32
_dal_mango_acl_tbl_entry_move(uint32 unit, rtk_acl_phase_t phase,
    uint32 fromIdx, uint32 toIdx, rtk_bitmap_t *pEntryMovedInfo)
{
    acl_entry_t         entry;
    int32               ret;

    ACL_MV_DBG("%s from %u to %u\n", __func__, fromIdx, toIdx);

    ACL_ERR_CHK_MSG_ARGS(ACL_TBL_READ(unit, MANGO_VACLt, fromIdx, (uint32 *)&entry),
            ret, "Table read %d fail", fromIdx);

    ACL_ERR_CHK_MSG_ARGS(ACL_TBL_WRITE(unit, MANGO_VACLt, toIdx, (uint32 *)&entry),
            ret, "Table write %d fail", toIdx);

    BITMAP_SET(pEntryMovedInfo, fromIdx);

    /*
        For AND2 case. AGG2 mode: [0, 1, 128, 129]
        And entry 127 ~ 2 is no config => not AND2.
        Config: move from 0 to 2 len 130
        1. move 129 to 131 => [0, 1, 128, 129] to [2, 3, 130, 131] => mark 0, 1, 128, 129 is moved.
        2. move entry 3 => get agg info is new status (X)
    */
    BITMAP_SET(pEntryMovedInfo, toIdx);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_tbl_entry_move */

/*
    Aggregation: E(N), E(N+1), E(N+128), E(N+129)
    agg2Base is                 E(N)
    agg1Base is                 E(N) and/or (E+128)
    agg2BaseAgg1Partner is      E(N+1)
    agg2PartnerAgg1Partner is   E(N+129)
*/
static int32
_dal_mango_acl_entryAggType_get(uint32 unit, uint32 entryIdx,
    dal_mango_acl_aggInfo_t *pAggInfo)
{
    acl_entry_t     entry;
    uint32          entryBlkIdx, agg2BaseBlkIdx, agg2PartnerBlkIdx;
    uint32          baseEntryOfst;
    uint32          baseEntryIdx, agg2PartnerEntryIdx;
    int32           ret;

    RT_PARAM_CHK((NULL == pAggInfo), RT_ERR_NULL_POINTER);

    /* Init status */
    pAggInfo->valid             = ENABLED;
    pAggInfo->agg2Sts           = DISABLED;
    pAggInfo->agg1Sts           = DISABLED;
    pAggInfo->baseEntryIdx      = entryIdx;

    /* find AND2 operator block index */
    entryBlkIdx     = entryIdx >> ACL_BLK_BIT_OFST;
    agg2BaseBlkIdx  = entryBlkIdx - (entryBlkIdx % 2);

    /* find entry offset of block */
    baseEntryOfst = (entryIdx & ACL_ENTRY_IDX_MSK) - (entryIdx % 2);

    baseEntryIdx = (agg2BaseBlkIdx << ACL_BLK_BIT_OFST) | baseEntryOfst;

    agg2PartnerBlkIdx   = agg2BaseBlkIdx + 1;
    agg2PartnerEntryIdx = (agg2PartnerBlkIdx << ACL_BLK_BIT_OFST) + baseEntryOfst;

    RT_ERR_CHK(ACL_TBL_READ(unit, MANGO_VACLt, baseEntryIdx, (uint32 *)&entry), ret);

    RT_ERR_CHK(table_field_get(unit, MANGO_VACLt, MANGO_VACL_AND2tf,
            &pAggInfo->agg2Sts, (uint32 *)&entry), ret);

    if (ENABLED == pAggInfo->agg2Sts)
    {
        pAggInfo->baseEntryIdx          = baseEntryIdx;
        pAggInfo->agg2PartnerEntryIdx   = agg2PartnerEntryIdx;

        /* When AGG2 is enabled, move entry is quadruple mode */
        RT_ERR_CHK(table_field_get(unit, MANGO_VACLt, MANGO_VACL_AND2tf,
                &pAggInfo->agg1Sts, (uint32 *)&entry), ret);

        pAggInfo->agg1PartnerEntryIdx               = baseEntryIdx + 1;
        pAggInfo->agg2PartnerAgg1PartnerEntryIdx    = agg2PartnerEntryIdx + 1;
    }
    else
    {
        /* Get AGG1 status of E(N) or E(N+128) */
        if (agg2BaseBlkIdx == entryBlkIdx)
        {
            pAggInfo->baseEntryIdx = baseEntryIdx;
        }
        else
        {
            RT_ERR_CHK(ACL_TBL_READ(unit, MANGO_VACLt, agg2PartnerEntryIdx, (uint32 *)&entry), ret);
            pAggInfo->baseEntryIdx = agg2PartnerEntryIdx;
        }

        RT_ERR_CHK(table_field_get(unit, MANGO_VACLt, MANGO_VACL_AND1tf,
                &pAggInfo->agg1Sts, (uint32 *)&entry), ret);

        /* AGG1 mode */
        if (ENABLED == pAggInfo->agg1Sts)
        {
            pAggInfo->agg1PartnerEntryIdx = pAggInfo->baseEntryIdx + 1;
        }
        /* Single mode */
        else
        {
            pAggInfo->baseEntryIdx = entryIdx;
        }
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_entryAggType_get */

/*
    Aggregation: E(N), E(N+1), E(N+128), E(N+129)
    agg2Base is                 E(N)
    agg1Base is                 E(N) and/or (E+128)
    agg2BaseAgg1Partner is      E(N+1)
    agg2PartnerAgg1Partner is   E(N+129)

    Input entryIdx maybe is E(N+129) when agg2 mode move to down.
    Or entryIdx is E(N+1) when agg1 mode move to down.
    Or entryIdx is E(N) when move to up.
*/
static int32
_dal_mango_acl_mvEntryAggType_get(uint32 unit, dal_mango_acl_aggMoveInfo_t *pAggMvInfo,
    dal_mango_acl_aggInfo_t *pAggInfo)
{
    uint32          entryIdx;
    int32           ret;

    RT_PARAM_CHK((NULL == pAggMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAggInfo), RT_ERR_NULL_POINTER);

    entryIdx = pAggMvInfo->srcIdx;

    RT_ERR_CHK(_dal_mango_acl_entryAggType_get(unit, entryIdx, pAggInfo), ret);

    if (pAggMvInfo->agg2Sts != pAggInfo->agg2Sts)
    {
        ACL_MV_DBG("entry %u agg2Sts mis-match %d %d\n",
                pAggMvInfo->srcIdx, pAggMvInfo->agg2Sts, pAggInfo->agg2Sts);

        pAggInfo->valid = DISABLED;
        return RT_ERR_OK;
    }

    /*
        1. AGG2 base is NOT in range (before src start).
        2. AGG2 partner is NOT in range (after src end).
        3. AGG2 and AGG1 partner is NOT in range (after src end).
        4. AGG1 partner is NOT in range (after src end).
    */
    if ((pAggInfo->baseEntryIdx < pAggMvInfo->srcStartIdx) ||
            ((ENABLED == pAggInfo->agg2Sts) &&
             (pAggInfo->agg2PartnerEntryIdx > pAggMvInfo->srcEndIdx)) ||
            ((ENABLED == pAggInfo->agg2Sts) && (ENABLED == pAggInfo->agg1Sts) &&
             (pAggInfo->agg2PartnerAgg1PartnerEntryIdx > pAggMvInfo->srcEndIdx)) ||
            ((ENABLED == pAggInfo->agg1Sts) &&
             (pAggInfo->agg1PartnerEntryIdx > pAggMvInfo->srcEndIdx)))
    {
        pAggInfo->valid = DISABLED;
        ACL_MV_DBG("%s agg2Sts %d agg1Sts %d start %u end %u entry %u (%u %u %u %u)\n",
                __func__, pAggInfo->agg2Sts, pAggInfo->agg1Sts, pAggMvInfo->srcStartIdx,
                pAggMvInfo->srcEndIdx, pAggMvInfo->srcIdx, pAggInfo->baseEntryIdx,
                pAggInfo->agg1PartnerEntryIdx, pAggInfo->agg2PartnerEntryIdx,
                pAggInfo->agg2PartnerAgg1PartnerEntryIdx);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_mvEntryAggType_get */

static int32
_dal_mango_acl_aggEntry_del(uint32 unit, rtk_acl_phase_t phase, dal_mango_acl_aggInfo_t *pAggInfo)
{
    int32   ret;

    RT_PARAM_CHK((NULL == pAggInfo), RT_ERR_NULL_POINTER);

    /* Delete Agg2BaseAgg1Partner entry at least */
    if ((pAggInfo->agg2Sts || pAggInfo->agg1Sts) && (ACL_PHASE_EACL != phase))
    {
        RT_ERR_CHK(_dal_mango_acl_tblEntry_del(unit, pAggInfo->agg1PartnerEntryIdx), ret);
    }

    if (pAggInfo->agg2Sts)
    {
        /* Delete Agg2PartnerAgg1Partner entry */
        if (ACL_PHASE_EACL != phase)
        {
            RT_ERR_CHK(_dal_mango_acl_tblEntry_del(unit, pAggInfo->agg2PartnerAgg1PartnerEntryIdx), ret);
        }

        /* !!! Delete Agg2Partner entry after Agg2PartnerAgg1Partner !!!*/
        RT_ERR_CHK(_dal_mango_acl_tblEntry_del(unit, pAggInfo->agg2PartnerEntryIdx), ret);

    }

    /* !!! Delete Agg2 base entry at least !!! */
    RT_ERR_CHK(_dal_mango_acl_tblEntry_del(unit, pAggInfo->baseEntryIdx), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_aggEntry_del */

static int32
_dal_mango_acl_aggEntry_move(uint32 unit, dal_mango_acl_aggMoveInfo_t *pAggMvInfo)
{
    dal_mango_acl_aggInfo_t     entryAggInfo;
    acl_entry_t             aclEntry;
    log_entry_t             cntrEntry;
    uint32                  oriAgg2BaseValid, newAgg2BaseValid;
    uint32                  mvDstIdx;
    int32                   mvOfst;
    int32                   ret;

    RT_PARAM_CHK((NULL == pAggMvInfo), RT_ERR_NULL_POINTER);

    ACL_MV_DBG("%s src %u dst %u agg2Sts %d\n", __func__,
            pAggMvInfo->srcIdx, pAggMvInfo->dstIdx, pAggMvInfo->agg2Sts);

    RT_ERR_CHK(_dal_mango_acl_mvEntryAggType_get(unit, pAggMvInfo, &entryAggInfo), ret);

    ACL_MV_DBG("agg2Sts %u\n", entryAggInfo.agg2Sts);
    ACL_MV_DBG("agg1Sts %u\n", entryAggInfo.agg1Sts);
    ACL_MV_DBG("baseEntryIdx %u\n", entryAggInfo.baseEntryIdx);
    ACL_MV_DBG("agg1PartnerEntryIdx %u\n", entryAggInfo.agg1PartnerEntryIdx);
    ACL_MV_DBG("agg2PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerEntryIdx);
    ACL_MV_DBG("agg2PartnerAgg1PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerAgg1PartnerEntryIdx);

    if (DISABLED == entryAggInfo.valid)
    {
        ACL_MV_DBG("entry %u invalid\n", pAggMvInfo->srcIdx);
        return RT_ERR_OK;
    }

    mvOfst = pAggMvInfo->dstIdx - pAggMvInfo->srcIdx;

    /* single entry */
    if (DISABLED == entryAggInfo.agg2Sts && DISABLED == entryAggInfo.agg1Sts)
    {
        mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

        ACL_MV_DBG("Single src %u dst %u\n", entryAggInfo.baseEntryIdx, mvDstIdx);

        RT_ERR_CHK(_dal_mango_acl_tbl_entry_move(unit, pAggMvInfo->phase,
                entryAggInfo.baseEntryIdx, mvDstIdx, pAggMvInfo->pEntryMovedInfo), ret);
    }
    /* aggregation entry */
    else //if (entryAggInfo.agg2Sts || entryAggInfo.agg1Sts)
    {
        /* AGG2 base entry, invalid Agg2 base entry at first */
        mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

        ACL_MV_DBG("Agg2 src %u dst %u\n", entryAggInfo.baseEntryIdx, mvDstIdx);

        ACL_ERR_CHK_MSG_ARGS(ACL_TBL_READ(unit, MANGO_VACLt, entryAggInfo.baseEntryIdx,
                (uint32 *)&aclEntry), ret, "Table read %u fail", entryAggInfo.baseEntryIdx);

        RT_ERR_CHK(table_field_get(unit, MANGO_VACLt, MANGO_VACL_VALIDtf, &oriAgg2BaseValid,
                (uint32 *)&aclEntry), ret);

        newAgg2BaseValid = 0;
        RT_ERR_CHK(table_field_set(unit, MANGO_VACLt, MANGO_VACL_VALIDtf, &newAgg2BaseValid,
                (uint32 *)&aclEntry), ret);

        ACL_ERR_CHK_MSG_ARGS(ACL_TBL_WRITE(unit, MANGO_VACLt, mvDstIdx,
                (uint32 *)&aclEntry), ret, "Table write %u fail", mvDstIdx);

        BITMAP_SET(pAggMvInfo->pEntryMovedInfo, entryAggInfo.baseEntryIdx);
        BITMAP_SET(pAggMvInfo->pEntryMovedInfo, mvDstIdx);

        /* Agg2BaseAgg1Partner entry */
        if (entryAggInfo.agg1Sts)
        {
            mvDstIdx = entryAggInfo.agg1PartnerEntryIdx + mvOfst;

            ACL_MV_DBG("Agg2 base Agg1 src %u dst %u\n",
                    entryAggInfo.agg1PartnerEntryIdx, mvDstIdx);

            RT_ERR_CHK(_dal_mango_acl_tbl_entry_move(unit, pAggMvInfo->phase,
                    entryAggInfo.agg1PartnerEntryIdx, mvDstIdx, pAggMvInfo->pEntryMovedInfo), ret);
        }
    }

    if (entryAggInfo.agg2Sts)
    {
        /* Agg2Partner entry */
        mvDstIdx = entryAggInfo.agg2PartnerEntryIdx + mvOfst;

        ACL_MV_DBG("Agg2 partner src %u dst %u\n", entryAggInfo.agg2PartnerEntryIdx, mvDstIdx);

        RT_ERR_CHK(_dal_mango_acl_tbl_entry_move(unit, pAggMvInfo->phase,
                entryAggInfo.agg2PartnerEntryIdx, mvDstIdx, pAggMvInfo->pEntryMovedInfo), ret);

        /* Agg2PartnerAgg1Partner entry */
        if (entryAggInfo.agg1Sts)
        {
            mvDstIdx = entryAggInfo.agg2PartnerAgg1PartnerEntryIdx + mvOfst;

            ACL_MV_DBG("Agg2 partner src %u dst %u\n",
                    entryAggInfo.agg2PartnerAgg1PartnerEntryIdx, mvDstIdx);

            RT_ERR_CHK(_dal_mango_acl_tbl_entry_move(unit, pAggMvInfo->phase,
                    entryAggInfo.agg2PartnerAgg1PartnerEntryIdx, mvDstIdx,
                    pAggMvInfo->pEntryMovedInfo), ret);
        }
    }

    /* Validate Agg2 entry at least */
    if ((entryAggInfo.agg2Sts || entryAggInfo.agg1Sts) && (newAgg2BaseValid != oriAgg2BaseValid))
    {
        mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

        ACL_MV_DBG("Enable Agg base entry %u valid\n", mvDstIdx);

        ACL_ERR_CHK_MSG_ARGS(ACL_TBL_READ(unit, MANGO_VACLt, mvDstIdx,
                (uint32 *)&aclEntry), ret, "Table read %u fail", mvDstIdx);

        RT_ERR_CHK(table_field_set(unit, MANGO_VACLt, MANGO_VACL_VALIDtf, &oriAgg2BaseValid,
                (uint32 *)&aclEntry), ret);

        ACL_ERR_CHK_MSG_ARGS(ACL_TBL_WRITE(unit, MANGO_VACLt, mvDstIdx,
                (uint32 *)&aclEntry), ret, "Table write %u fail", mvDstIdx);
    }

    /* counter move */
    mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

    ACL_MV_DBG("Move base entry %u counter to %u\n", entryAggInfo.baseEntryIdx, mvDstIdx);

    ACL_ERR_CHK_MSG_ARGS(table_read(unit, MANGO_FLOW_CNTRt, entryAggInfo.baseEntryIdx,
            (uint32 *)&cntrEntry), ret, "Cntr entry %u read fail", entryAggInfo.baseEntryIdx);

    ACL_ERR_CHK_MSG_ARGS(table_write(unit, MANGO_FLOW_CNTRt, mvDstIdx,
            (uint32 *)&cntrEntry), ret, "Cntr entry %u write fail", mvDstIdx);

    /* delete original entry */
    RT_ERR_CHK(_dal_mango_acl_aggEntry_del(unit, pAggMvInfo->phase, &entryAggInfo), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_aggEntry_move */

static int32
_dal_mango_acl_ruleAggSts_get(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pMvInfo,
    rtk_enable_t *pAgg1Sts, rtk_enable_t *pAgg2Sts)
{
    acl_entry_t     aclEntry;
    uint32          srcIdx;
    int32           ret;

    RT_PARAM_CHK((NULL == pMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAgg1Sts), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAgg2Sts), RT_ERR_NULL_POINTER);

    *pAgg1Sts = DISABLED;
    *pAgg2Sts = DISABLED;

    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, pMvInfo->move_from, srcIdx);

    /* find AND1 & AND2 operator block index */
    ACL_ERR_CHK_MSG_ARGS(ACL_TBL_READ(unit, MANGO_VACLt, srcIdx,
            (uint32 *)&aclEntry), ret, "Table read %u fail", srcIdx);
    RT_ERR_CHK(table_field_get(unit, MANGO_VACLt, MANGO_VACL_AND1tf, pAgg1Sts, (uint32 *)&aclEntry), ret);
    RT_ERR_CHK(table_field_get(unit, MANGO_VACLt, MANGO_VACL_AND2tf, pAgg2Sts, (uint32 *)&aclEntry), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_ruleAggSts_get */

static int32
_dal_mango_acl_physicalEntry_move(uint32 unit, rtk_acl_phase_t phase,
    dal_mango_acl_aggMoveInfo_t *pAggMvInfo,
    const dal_mango_acl_physicalMove_info_t * const pPhyMvInfo)
{
    uint32  logicSrcOfstIdx, logicDstOfstIdx;
    uint32  mvSrcIdx, mvDstIdx;
    uint32  srcIdx, dstIdx;
    int32   ret;

    RT_PARAM_CHK((NULL == pAggMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPhyMvInfo), RT_ERR_NULL_POINTER);

    ACL_MV_DBG("lenIdx %d\n", pPhyMvInfo->lenIdx);
    logicSrcOfstIdx = (pPhyMvInfo->lenIdx + pPhyMvInfo->logicSrcBlkEntryOfst);
    logicDstOfstIdx = (pPhyMvInfo->lenIdx + pPhyMvInfo->logicDstBlkEntryOfst);

    mvSrcIdx = (((logicSrcOfstIdx / pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->logicBlkOfst) +
            ((logicSrcOfstIdx % pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->entryWidth) + pPhyMvInfo->srcBlkEntryBase);

    mvDstIdx = (((logicDstOfstIdx / pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->logicBlkOfst) +
            ((logicDstOfstIdx % pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->entryWidth) + pPhyMvInfo->dstBlkEntryBase);

    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, mvSrcIdx, srcIdx);
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, mvDstIdx, dstIdx);
    ACL_MV_DBG("mvSrcIdx %u mvDstIdx %u srcIdx %u dstIdx %u\n", mvSrcIdx, mvDstIdx, srcIdx, dstIdx);

    if (BITMAP_IS_CLEAR(pAggMvInfo->pEntryMovedInfo, srcIdx))
    {
        pAggMvInfo->srcIdx = srcIdx;
        pAggMvInfo->dstIdx = dstIdx;
        RT_ERR_CHK(_dal_mango_acl_aggEntry_move(unit, pAggMvInfo), ret);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_physicalEntry_move */

static int32
_dal_mango_acl_entry_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pMvInfo,
    rtk_bitmap_t *pEntryMovedInfo)
{
    rtk_enable_t            agg1Sts, agg2Sts;
    dal_mango_acl_aggMoveInfo_t aggMvInfo;
    uint32                  convertLen, dualEntryLen, blkEntryNum;
    uint32                  dstIdx;
    uint32                  srcBlkEntryOfst, dstBlkEntryOfst;
    uint32                  blkWidth;
    int32                   lenIdx;
    int32                   ret;
    dal_mango_acl_physicalMove_info_t   phyMvInfo;

    RT_PARAM_CHK((NULL == pMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntryMovedInfo), RT_ERR_NULL_POINTER);

    ACL_MV_DBG("%s from %#x to %#x len %u\n", __func__,
            pMvInfo->move_from, pMvInfo->move_to, pMvInfo->length);

    /* entry index from and to are same physical/logic type */
    if (PIE_ENTRY_IS_PHYSICAL_TYPE(pMvInfo->move_from) !=
            PIE_ENTRY_IS_PHYSICAL_TYPE(pMvInfo->move_to))
    {
        return RT_ERR_ENTRY_INDEX;
    }

    RT_ERR_CHK(_dal_mango_acl_ruleAggSts_get(unit, phase, pMvInfo, &agg1Sts, &agg2Sts), ret);

    blkEntryNum  = HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    if (ENABLED == agg2Sts)
    {
        if (((pMvInfo->move_from % 2) != 0) || ((pMvInfo->move_to % 2) != 0))
        {
            return RT_ERR_ENTRY_INDEX;
        }

        /* N = user length * 2;
           physical length: [(N / 128) * 256] + [(N % 128) + 128] */
        dualEntryLen = (pMvInfo->length << 1);
        convertLen   = (((dualEntryLen / blkEntryNum) * (blkEntryNum << 1)) +
                ((dualEntryLen % blkEntryNum) + blkEntryNum));

        phyMvInfo.entryWidth  = 2;
        blkWidth    = 2;
    }
    else if (ENABLED == agg1Sts)
    {
        if (((pMvInfo->move_from % 2) != 0) || ((pMvInfo->move_to % 2) != 0))
        {
            return RT_ERR_ENTRY_INDEX;
        }

        convertLen = (pMvInfo->length << 1);

        phyMvInfo.entryWidth  = 2;
        blkWidth    = 1;
    }
    else
    {
        convertLen = pMvInfo->length;

        phyMvInfo.entryWidth  = 1;
        blkWidth    = 1;
    }

    /* check start (from/to) and end (from/to) index is valid */
    /* check end from & to index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, (pMvInfo->move_from + convertLen - 1), aggMvInfo.srcEndIdx);
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, (pMvInfo->move_to + convertLen - 1), dstIdx);
    /* check start from & to index. */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, pMvInfo->move_from, aggMvInfo.srcStartIdx);
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, pMvInfo->move_to, dstIdx);

    ACL_MV_DBG("%s move from %u (end %u) to %u convertLen %u agg1Sts %u agg2Sts %u\n", __func__,
            aggMvInfo.srcStartIdx, aggMvInfo.srcEndIdx, dstIdx,
            convertLen, agg1Sts, agg2Sts);

    BITMAP_RESET(pEntryMovedInfo, BITMAP_ARRAY_CNT(HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)));

    aggMvInfo.pEntryMovedInfo = pEntryMovedInfo;
    aggMvInfo.phase           = phase;
    aggMvInfo.agg2Sts         = agg2Sts;

    srcBlkEntryOfst = pMvInfo->move_from % blkEntryNum;
    dstBlkEntryOfst = pMvInfo->move_to % blkEntryNum;

    phyMvInfo.srcBlkEntryBase       = pMvInfo->move_from - srcBlkEntryOfst;
    phyMvInfo.dstBlkEntryBase       = pMvInfo->move_to - dstBlkEntryOfst;
    phyMvInfo.logicBlkEntryNum      = blkEntryNum / phyMvInfo.entryWidth;
    phyMvInfo.logicBlkOfst          = blkEntryNum * blkWidth;
    phyMvInfo.logicSrcBlkEntryOfst  = srcBlkEntryOfst / phyMvInfo.entryWidth;
    phyMvInfo.logicDstBlkEntryOfst  = dstBlkEntryOfst / phyMvInfo.entryWidth;

    ACL_MV_DBG("entryWidth: %u ", phyMvInfo.entryWidth);
    ACL_MV_DBG("blkWidth: %u ", blkWidth);
    ACL_MV_DBG("srcBlkEntryOfst: %u ", srcBlkEntryOfst);
    ACL_MV_DBG("dstBlkEntryOfst: %u ", dstBlkEntryOfst);
    ACL_MV_DBG("srcBlkEntryBase: %u ", phyMvInfo.srcBlkEntryBase);
    ACL_MV_DBG("dstBlkEntryBase: %u ", phyMvInfo.dstBlkEntryBase);
    ACL_MV_DBG("logicBlkEntryNum: %u ", phyMvInfo.logicBlkEntryNum);
    ACL_MV_DBG("logicBlkOfst: %u ", phyMvInfo.logicBlkEntryNum);
    ACL_MV_DBG("logicSrcBlkEntryOfst: %u ", phyMvInfo.logicSrcBlkEntryOfst);
    ACL_MV_DBG("logicDstBlkEntryOfst: %u ", phyMvInfo.logicDstBlkEntryOfst);

    /* move down */
    if (pMvInfo->move_from < pMvInfo->move_to)
    {
        for (lenIdx = (pMvInfo->length - 1); lenIdx >= 0; --lenIdx)
        {
            phyMvInfo.lenIdx = lenIdx;
            RT_ERR_CHK(_dal_mango_acl_physicalEntry_move(unit, phase, &aggMvInfo, &phyMvInfo), ret);
        }
    }
    /* move up */
    else
    {
        for (lenIdx = 0; lenIdx < pMvInfo->length; ++lenIdx)
        {
            phyMvInfo.lenIdx = lenIdx;
            RT_ERR_CHK(_dal_mango_acl_physicalEntry_move(unit, phase, &aggMvInfo, &phyMvInfo), ret);
        }
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_entry_move */

/* Function Name:
 *      dal_mango_acl_rule_move
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
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      If one of move entries is AGG2 mode,
 *      all move entries MUST AGG2 mode and not AGG2 mode entry won't move.
 *
 *      length is user view entry number.
 *      AGG1 entry length 1 will move 2 entries.
 *      AGG2 entry length 1 will move 4 entries.
 */
int32
dal_mango_acl_rule_move(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_move_t *pData)
{
    rtk_bitmap_t    *physicalEntryMoved;
    int32           ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,move_from=%d,move_to=%d,length=%d",
            unit, phase, pData->move_from, pData->move_to, pData->length);

    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((pData->move_from == pData->move_to), RT_ERR_INPUT);
    RT_PARAM_CHK((0 == pData->length), RT_ERR_INPUT);

    ACL_SEM_LOCK(unit);

    physicalEntryMoved = osal_alloc(BITMAP_ARRAY_SIZE(HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)));
    if (!physicalEntryMoved)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_DAL|MOD_ACL), "alloc memory fail");
        return RT_ERR_MEM_ALLOC;
    }

    if ((ret = _dal_mango_acl_entry_move(unit, phase, pData, physicalEntryMoved)) != RT_ERR_OK)
    {
        osal_free(physicalEntryMoved);
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "entry move fail");
        return ret;
    }

    osal_free(physicalEntryMoved);

    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_rule_move */

void _swap64(uint64 *a, uint64 *b)
{
    uint64 temp = *b;
    *b = *a;
    *a = temp;
}

void _max_heapify64(uint64 *array, int start, int end)
{
    int32 dad = start;
    int32 son = dad * 2 + 1;

    while (son <= end) {
        if (son + 1 <= end && array[son] < array[son + 1])
            son++;
        if (array[dad] > array[son])
            return;
        else
        {
            _swap64(&array[dad], &array[son]);
            dad = son;
            son = dad * 2 + 1;
        }
    }
}

static void _heap_sort64(uint64 *array, int32 len)
{
    int i;

    // build max heap
    for (i = len / 2 - 1; i >= 0; i--)
        _max_heapify64(array, i, len - 1);

    for (i = len - 1; i > 0; i--)
    {
        _swap64(&array[0], &array[i]);
        _max_heapify64(array, 0, i - 1);
    }
}   /* end of _heap_sort */

uint32 _dal_mango_acl_cntr_read(uint32 unit, rtk_acl_phase_t phase,
    uint32 phyEntryIdx, rtk_acl_statMode_t mode, uint64 *pCnt)
{
    log_entry_t entry[DAL_MANGO_ACL_CNTR_OPER_MAX];
    uint32      operIdx;
    uint32      hitSts;
    uint32      field_H, field_L;
    uint32      cntr_H[DAL_MANGO_ACL_CNTR_OPER_MAX], cntr_L[DAL_MANGO_ACL_CNTR_OPER_MAX];
    uint64      cntr[DAL_MANGO_ACL_CNTR_OPER_MAX];
    int32       ret;

    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((mode >= STAT_MODE_END), RT_ERR_TYPE);
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    switch (mode)
    {
        case STAT_MODE_BYTE:
            field_H = MANGO_FLOW_CNTR_BYTE_CNTR_Htf;
            field_L = MANGO_FLOW_CNTR_BYTE_CNTR_Ltf;
            break;
        case STAT_MODE_PACKET:
            field_H = MANGO_FLOW_CNTR_PKT_CNTR_Htf;
            field_L = MANGO_FLOW_CNTR_PKT_CNTR_Ltf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RT_ERR_CHK(_dal_mango_pie_entryHitSts_get(unit, phyEntryIdx, TRUE, &hitSts), ret);

    RT_ERR_CHK(table_read(unit, MANGO_FLOW_CNTRt, phyEntryIdx, (uint32 *)&entry[0]), ret);

    RT_ERR_CHK(_dal_mango_pie_entryHitSts_get(unit, phyEntryIdx, TRUE, &hitSts), ret);

    RT_ERR_CHK(table_read(unit, MANGO_FLOW_CNTRt, phyEntryIdx, (uint32 *)&entry[1]), ret);

    if (FALSE == hitSts)
    {
        RT_ERR_CHK(table_field_get(unit, MANGO_FLOW_CNTRt, field_H, (uint32 *)&cntr_H[0],
                (uint32 *)&entry[0]), ret);

        RT_ERR_CHK(table_field_get(unit, MANGO_FLOW_CNTRt, field_L, (uint32 *)&cntr_L[0],
                (uint32 *)&entry[0]), ret);

        RT_ERR_CHK(table_field_get(unit, MANGO_FLOW_CNTRt, field_H, (uint32 *)&cntr_H[1],
                (uint32 *)&entry[1]), ret);

        RT_ERR_CHK(table_field_get(unit, MANGO_FLOW_CNTRt, field_L, (uint32 *)&cntr_L[1],
                (uint32 *)&entry[1]), ret);

        if (cntr_H[0] == cntr_H[1] && cntr_L[0] == cntr_L[1])
        {
            *pCnt = (((uint64)cntr_H[0]) << 32) | ((uint64)cntr_L[0]);
            return RT_ERR_OK;
        }
    }

    for (operIdx = 0; operIdx < DAL_MANGO_ACL_CNTR_OPER_MAX; ++operIdx)
    {
        RT_ERR_CHK(table_read(unit, MANGO_FLOW_CNTRt, phyEntryIdx, (uint32 *)&entry[operIdx]), ret);
    }

    for (operIdx = 0; operIdx < DAL_MANGO_ACL_CNTR_OPER_MAX; ++operIdx)
    {
        RT_ERR_CHK(table_field_get(unit, MANGO_FLOW_CNTRt, field_H, (uint32 *)&cntr_H[operIdx],
                (uint32 *)&entry[operIdx]), ret);

        RT_ERR_CHK(table_field_get(unit, MANGO_FLOW_CNTRt, field_L, (uint32 *)&cntr_L[operIdx],
                (uint32 *)&entry[operIdx]), ret);

        cntr[operIdx] = (((uint64)cntr_H[operIdx]) << 32) | ((uint64)cntr_L[operIdx]);
    }

    _heap_sort64(cntr, DAL_MANGO_ACL_CNTR_OPER_MAX);

    *pCnt = cntr[(DAL_MANGO_ACL_CNTR_OPER_MAX >> 1)];

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_cntr_read */

/* Function Name:
 *      dal_mango_acl_statCnt_get
 * Description:
 *      Get packet-based or byte-based statistic counter of the log id.
 * Input:
 *      unit        - unit id
 *      phase       - PIE lookup phase
 *      entry_idx   - logic entry index
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
dal_mango_acl_statCnt_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_pie_id_t entryIdx, rtk_acl_statMode_t mode, uint64 *pCnt)
{
    uint32      phyEntryId;
    int32       ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((mode >= STAT_MODE_END), RT_ERR_TYPE);
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entryIdx, phyEntryId);

    ACL_SEM_LOCK(unit);

    ret = _dal_mango_acl_cntr_read(unit, phase, phyEntryId, mode, pCnt);
    if (ret != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_ACL|MOD_DAL), "pCnt=%llu", *pCnt);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_statCnt_get */

/* Function Name:
 *      dal_mango_acl_statCnt_clear
 * Description:
 *      Clear statistic counter of the log id.
 * Input:
 *      unit        - unit id
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
dal_mango_acl_statCnt_clear(uint32 unit, rtk_acl_phase_t phase,
    rtk_pie_id_t entryIdx, rtk_acl_statMode_t mode)
{
    log_entry_t             entry;
    uint32                  phyEntryId;
    uint32                  val = 0;
    int32                   ret;

    /* Check init state */
    RT_INIT_CHK(acl_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((mode >= STAT_MODE_END), RT_ERR_TYPE);

    osal_memset(&entry, 0, sizeof(log_entry_t));
    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entryIdx, phyEntryId);

    if (STAT_MODE_BYTE != mode)
    {
        /* ADDR[13:0]={PKTCNTR[0:0], BYTECNTR[0:0], Index [11:0]} */
        entryIdx |= (1<<13);

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf,\
                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf,\
                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    if (STAT_MODE_PACKET != mode)
    {
        /* ADDR[13:0]={PKTCNTR[0:0], BYTECNTR[0:0], Index [11:0]} */
        entryIdx |= (1<<12);

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf,\
                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf,\
                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }

    val = 0;//OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT;

    if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf,\
                &val, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    ACL_SEM_LOCK(unit);
    if ((ret = table_write(unit, MANGO_FLOW_CNTRt, phyEntryId, (uint32 *) &entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_statCnt_clear */

/* Function Name:
 *      dal_mango_acl_limitLoopbackTimes_get
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
dal_mango_acl_limitLoopbackTimes_get(uint32 unit, uint32 *pLb_times)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLb_times), RT_ERR_NULL_POINTER);

    /* function body */
    ACL_SEM_LOCK(unit);
    if((ret = reg_field_read(unit, MANGO_ACL_CTRLr, MANGO_LB_LIMITf,
            pLb_times) != RT_ERR_OK))
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_limitLoopbackTimes_get */

/* Function Name:
 *      dal_mango_acl_limitLoopbackTimes_set
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
dal_mango_acl_limitLoopbackTimes_set(uint32 unit, uint32 lb_times)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,lb_times=%d", unit, lb_times);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((8 <= lb_times), RT_ERR_INPUT);

    /* function body */
    ACL_SEM_LOCK(unit);
    if((ret = reg_field_write(unit, MANGO_ACL_CTRLr, MANGO_LB_LIMITf,
            &lb_times) != RT_ERR_OK))
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_acl_limitLoopbackTimes_set */

/* Function Name:
 *      dal_mango_acl_ruleEntryField_validate
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
dal_mango_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type)
{
    dal_mango_phaseInfo_t   phaseInfo;
    acl_entry_t             entry;
    uint32                  phyEntryId;
    int32                   ret;
    uint8                   data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8                   mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,entry_idx=%d,type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_ACL_PHASE);
    RT_PARAM_CHK((type >= USER_FIELD_END), RT_ERR_ACL_FIELD_TYPE);

    /* function body */
    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* translate to physical index */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);

    ACL_SEM_LOCK(unit);
    if ((ret = ACL_TBL_READ(unit, phaseInfo.table, phyEntryId, (uint32 *)&entry)) != RT_ERR_OK)
    {
        ACL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }
    ACL_SEM_UNLOCK(unit);

    if ((ret = _dal_mango_acl_ruleEntryBufField_get(unit, phase,
            phyEntryId, type, (uint32 *)&entry, data, mask)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_acl_ruleEntryField_validate */

/* Function Name:
 *      dal_mango_acl_templateId_get
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
dal_mango_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id)
{
    rtk_acl_templateIdx_t   template_idx;
    uint32                  phyEntryId;
    uint32                  block_idx;
    int32                   ret;
    uint8                   field_data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8                   field_mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTemplate_id), RT_ERR_NULL_POINTER);

    /* function body */
    /* get entry field template id */
    if ((ret = dal_mango_acl_ruleEntryField_read(unit, phase, entry_idx, \
            USER_FIELD_TEMPLATE_ID, field_data, field_mask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    /* caculate entry in which block */
    DAL_MANGO_ACL_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phyEntryId);
    block_idx = phyEntryId / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    osal_memset(&template_idx, 0, sizeof(rtk_acl_templateIdx_t));
    if ((ret = dal_mango_acl_templateSelector_get(unit, block_idx, &template_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    *pTemplate_id = template_idx.template_id[field_data[0]];

    return RT_ERR_OK;
}   /* end of dal_mango_acl_templateId_get */

/* Function Name:
 *      _dal_mango_acl_init_config
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
_dal_mango_acl_init_config(uint32 unit)
{
    return RT_ERR_OK;
} /* end of _dal_mango_acl_init_config */

/* Function Name:
 *      dal_mango_aclMapper_init
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
dal_mango_aclMapper_init(dal_mapper_t *pMapper)
{
    pMapper->acl_init = dal_mango_acl_init;
    pMapper->acl_ruleEntryFieldSize_get = dal_mango_acl_ruleEntryFieldSize_get;
    pMapper->acl_ruleEntrySize_get = dal_mango_acl_ruleEntrySize_get;
    pMapper->acl_ruleValidate_get = dal_mango_acl_ruleValidate_get;
    pMapper->acl_ruleValidate_set = dal_mango_acl_ruleValidate_set;
    pMapper->acl_ruleEntry_read = dal_mango_acl_ruleEntry_read;
    pMapper->acl_ruleEntry_write = dal_mango_acl_ruleEntry_write;
    pMapper->acl_ruleEntryField_get = dal_mango_acl_ruleEntryField_get;
    pMapper->acl_ruleEntryField_set = dal_mango_acl_ruleEntryField_set;
    pMapper->acl_ruleEntryField_read = dal_mango_acl_ruleEntryField_read;
    pMapper->acl_ruleEntryField_write = dal_mango_acl_ruleEntryField_write;
    pMapper->acl_ruleEntryField_check = dal_mango_acl_ruleEntryField_check;
    pMapper->acl_ruleOperation_get = dal_mango_acl_ruleOperation_get;
    pMapper->acl_ruleOperation_set = dal_mango_acl_ruleOperation_set;
    pMapper->acl_ruleAction_get = dal_mango_acl_ruleAction_get;
    pMapper->acl_ruleAction_set = dal_mango_acl_ruleAction_set;
    pMapper->acl_limitLoopbackTimes_get = dal_mango_acl_limitLoopbackTimes_get;
    pMapper->acl_limitLoopbackTimes_set = dal_mango_acl_limitLoopbackTimes_set;
    pMapper->acl_portPhaseLookupEnable_get = dal_mango_acl_portPhaseLookupEnable_get;
    pMapper->acl_portPhaseLookupEnable_set = dal_mango_acl_portPhaseLookupEnable_set;
    pMapper->acl_templateSelector_get = dal_mango_acl_templateSelector_get;
    pMapper->acl_templateSelector_set = dal_mango_acl_templateSelector_set;
    pMapper->acl_statCnt_get = dal_mango_acl_statCnt_get;
    pMapper->acl_statCnt_clear = dal_mango_acl_statCnt_clear;
    pMapper->acl_ruleHitIndication_get = dal_mango_acl_ruleHitIndication_get;
    pMapper->acl_ruleHitIndicationMask_get = dal_mango_acl_ruleHitIndicationMask_get;
    pMapper->acl_rule_del = dal_mango_acl_rule_del;
    pMapper->acl_rule_move = dal_mango_acl_rule_move;
    pMapper->acl_ruleEntryField_validate = dal_mango_acl_ruleEntryField_validate;
    pMapper->acl_fieldUsr2Template_get = dal_mango_acl_fieldUsr2Template_get;
    pMapper->acl_templateId_get = dal_mango_acl_templateId_get;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_acl_init
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
dal_mango_acl_init(uint32 unit)
{
    uint32  tblShadowSize;
    int32   ret;

    RT_INIT_REENTRY_CHK(acl_init[unit]);
    acl_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    acl_sem[unit] = osal_sem_mutex_create();
    if (0 == acl_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_ACL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    tblShadowSize = sizeof(acl_entry_t) * HAL_MAX_NUM_OF_PIE_FILTER_ID(unit);
    pACL_TBL = osal_alloc(tblShadowSize);
    if (!pACL_TBL)
    {
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_DAL|MOD_ACL), "alloc memory fail");
        return RT_ERR_MEM_ALLOC;
    }

    osal_memset(pACL_TBL, 0, tblShadowSize);

    acl_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((ret = _dal_mango_acl_init_config(unit)) != RT_ERR_OK)
        {
            osal_free(pACL_TBL);
            acl_init[unit] = INIT_NOT_COMPLETED;
            RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_mango_acl_init */

/* Function Name:
 *      dal_mango_acl_fieldUsr2Template_get
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
dal_mango_acl_fieldUsr2Template_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, rtk_acl_fieldUsr2Template_t *info)
{
    dal_mango_phaseInfo_t           phaseInfo;
    dal_mango_acl_entryField_t      *fieldList = NULL;
    dal_mango_acl_fieldLocation_t   *fieldLocation;
    uint32                          dalFieldType, tmplte_num;
    uint32                          fieldNum;
    int32                           ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_ACL), "unit=%d,phase=%d,type=%d", unit, phase, type);

    /* check Init status */
    RT_INIT_CHK(acl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACL_PHASE_END <= phase), RT_ERR_INPUT);
    RT_PARAM_CHK((USER_FIELD_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(info, 0, sizeof(rtk_acl_fieldUsr2Template_t));

    /* translate field from RTK superset view to DAL view */
    /* Fix field */
    if ((ret = _dal_mango_acl_phaseInfo_get(phase, &phaseInfo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    for (fieldNum = 0; phaseInfo.fixFieldList[fieldNum].type != USER_FIELD_END; ++fieldNum)
    {
        if (type == phaseInfo.fixFieldList[fieldNum].type)
        {
            info->fields[0] = TMPLTE_FIELD_FIX;
            return RT_ERR_OK;
        }
    }

    /* configable field */
    if ((ret = _dal_mango_acl_fieldInfo_get(phase, type, &fieldList,
            &dalFieldType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
        return ret;
    }

    tmplte_num = fieldList[dalFieldType].fieldNumber;
    if (tmplte_num >= RTK_ACL_USR2TMPLTE_MAX)
        return RT_ERR_FAILED;

    for (fieldNum = 0; fieldNum < tmplte_num; ++fieldNum)
    {
        fieldLocation = &fieldList[dalFieldType].pField[fieldNum];
        info->fields[fieldNum] = fieldLocation->template_field_type;
    }

    return ret;
}   /* end of dal_mango_acl_fieldUsr2Template_get */

