/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2015
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) OpenFlow
 *
 */


/*
 * Include Files
 */
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_pie.h>
#include <dal/mango/dal_mango_openflow.h>
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_tunnel.h>
#include <dal/mango/dal_mango_mpls.h>
#include <rtk/default.h>
#include <rtk/openflow.h>


/*
 * Symbol Definition
 */
#define DAL_MANGO_OF_FIXED_FIELD_SIZE       (2)     /* fix field size in byte */
#define DAL_MANGO_OF_TMP_FIELD_SIZE         (2)     /* fix field size in byte */
#define DAL_MANGO_OF_TMP_FIELD_NUM          (14)    /* the number of template field */
#define DAL_MANGO_OF_TMP_FIELD_TOTAL_SIZE   (DAL_MANGO_OF_TMP_FIELD_NUM*DAL_MANGO_OF_TMP_FIELD_SIZE)
#define DAL_MANGO_OF_SET_FIELD_ID_MAX       (4)     /* max. set filed ID */

typedef struct dal_mango_of_fieldLocation_s
{
    uint32   template_field_type;
    uint32   field_offset;
    uint32   field_length;
    uint32   data_offset;
} dal_mango_of_fieldLocation_t;

typedef struct dal_mango_of_fixField_s
{
    uint32                          data_field; /* data field in chip view */
    uint32                          mask_field; /* mask field in chip view */
    uint32                          position;   /* position in fix field */
    rtk_of_matchfieldType_t         type;       /* match field type in user view */
    dal_mango_of_fieldLocation_t    *pField;
} dal_mango_of_fixField_t;

typedef struct dal_mango_of_entryField_s
{
    rtk_of_matchfieldType_t         type;       /* field type in user view */
    uint32                          fieldNumber;/* locate in how many fields */
    dal_mango_of_fieldLocation_t    *pField;
    uint32                          valid_phase; /* valid phase */
} dal_mango_of_entryField_t;

typedef struct dal_mango_of_setFieldList_s
{
    rtk_of_setFieldType_t type[OF_SET_FIELD_TYPE_END];
} dal_mango_of_setFieldList_t;

typedef struct dal_mango_of_aggInfo_s
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
} dal_mango_of_aggInfo_t;

typedef struct dal_mango_of_aggMoveInfo_s
{
    rtk_bitmap_t    *pEntryMovedInfo;
    rtk_of_flowtable_phase_t  phase;
    rtk_enable_t    agg2Sts;
    uint32          srcStartIdx;
    uint32          srcEndIdx;
    uint32          srcIdx;
    uint32          dstIdx;
} dal_mango_of_aggMoveInfo_t;

typedef struct dal_mango_of_physicalMove_info_s
{
    uint32  logicSrcBlkEntryOfst;
    uint32  logicDstBlkEntryOfst;
    uint32  logicBlkEntryNum;
    uint32  logicBlkOfst;
    uint32  entryWidth;
    uint32  srcBlkEntryBase;
    uint32  dstBlkEntryBase;
    int32   lenIdx;
} dal_mango_of_physicalMove_info_t;

/*
 * Data Declaration
 */
static uint32       of_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t of_sem[RTK_MAX_NUM_OF_UNIT];

static uint32       l2Sram_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32       l2Cam_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32       l3Sram_size[RTK_MAX_NUM_OF_UNIT] = {0};
uint32              *pOF_TBL = NULL;

static dal_mango_of_drvDb_t     _ofDb[RTK_MAX_NUM_OF_UNIT];

dal_mango_of_setFieldList_t dal_mango_of_setField_list[FT_PHASE_END][DAL_MANGO_OF_SET_FIELD_ID_MAX+1] =
{
    {/* FT_PHASE_IGR_FT_0 */
        {{/* Field_ID_0 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_SA,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_IP_TTL,
            OF_SET_FIELD_TYPE_IP_FLAG_RSVD,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_1 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_DA,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_IP_TTL,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_2 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_ID,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_IP_TTL,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_3 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_IP4_SIP,
            OF_SET_FIELD_TYPE_IP4_DIP,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_4 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_L4_SPORT,
            OF_SET_FIELD_TYPE_L4_DPORT,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
    },
    {/* FT_PHASE_IGR_FT_1 */
        {{/* Field_ID_0 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_ID,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_1 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_2 */
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_3 */
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_4 */
            OF_SET_FIELD_TYPE_END
        }},
    },
    {/* FT_PHASE_IGR_FT_2 */
        {{/* Field_ID_0 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_SA,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_1 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_DA,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_2 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_ID,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_3 */
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_4 */
            OF_SET_FIELD_TYPE_END
        }},
    },
    {/* FT_PHASE_IGR_FT_3 */
        {{/* Field_ID_0 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_SA,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_IP_TTL,
            OF_SET_FIELD_TYPE_IP_FLAG_RSVD,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_1 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_DA,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_IP_TTL,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_2 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_ID,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_IP_TTL,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_3 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_IP4_SIP,
            OF_SET_FIELD_TYPE_IP4_DIP,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_4 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_L4_SPORT,
            OF_SET_FIELD_TYPE_L4_DPORT,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_MPLS_LABEL,
            OF_SET_FIELD_TYPE_MPLS_TC,
            OF_SET_FIELD_TYPE_MPLS_TTL,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
    },
    {/* FT_PHASE_EGR_FT_0 */
        {{/* Field_ID_0 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_PRI,
            OF_SET_FIELD_TYPE_IP_DSCP,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_1 */
            OF_SET_FIELD_TYPE_NONE,
            OF_SET_FIELD_TYPE_VLAN_ID,
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_2 */
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_3 */
            OF_SET_FIELD_TYPE_END
        }},
        {{/* Field_ID_4 */
            OF_SET_FIELD_TYPE_END
        }},
    },
};

dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_TEMPLATE_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FRAME_TYPE_L2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ITAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OTAG_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ITAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OTAG_FMT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FRAME_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FRAME_TYPE_L4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_END,
        /* offset address */         0x0,
        /* length */                 3,
        /* data offset */            0x0
    },
};

dal_mango_of_fixField_t dal_mango_of_fixField_list[] =
{
    {   /* data field       */  MANGO_FT_IGR_TIDtf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_TIDtf,
        /* position         */  0,
        /* field name       */  MATCH_FIELD_TEMPLATE_ID,
        /* field pointer    */  DAL_MANGO_MFIELD_TEMPLATE_ID
    },
    {   /* data field       */  MANGO_FT_IGR_FRAME_TYPE_L2tf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_FRAME_TYPE_L2tf,
        /* position         */  2,
        /* field name       */  MATCH_FIELD_FRAME_TYPE_L2,
        /* field pointer    */  DAL_MANGO_MFIELD_FRAME_TYPE_L2
    },
    {   /* data field       */  MANGO_FT_IGR_ITAG_EXISTtf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_ITAG_EXISTtf,
        /* position         */  4,
        /* field name       */  MATCH_FIELD_ITAG_EXIST,
        /* field pointer    */  DAL_MANGO_MFIELD_ITAG_EXIST
    },
    {   /* data field       */  MANGO_FT_IGR_OTAG_EXISTtf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_OTAG_EXISTtf,
        /* position         */  5,
        /* field name       */  MATCH_FIELD_OTAG_EXIST,
        /* field pointer    */  DAL_MANGO_MFIELD_OTAG_EXIST
    },
    {   /* data field       */  MANGO_FT_IGR_ITAG_FMTtf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_ITAG_FMTtf,
        /* position         */  6,
        /* field name       */  MATCH_FIELD_ITAG_FMT,
        /* field pointer    */  DAL_MANGO_MFIELD_ITAG_FMT
    },
    {   /* data field       */  MANGO_FT_IGR_OTAG_FMTtf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_OTAG_FMTtf,
        /* position         */  7,
        /* field name       */  MATCH_FIELD_OTAG_FMT,
        /* field pointer    */  DAL_MANGO_MFIELD_OTAG_FMT
    },
    {   /* data field       */  MANGO_FT_IGR_FRAME_TYPEtf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_FRAME_TYPEtf,
        /* position         */  8,
        /* field name       */  MATCH_FIELD_FRAME_TYPE,
        /* field pointer    */  DAL_MANGO_MFIELD_FRAME_TYPE
    },
    {   /* data field       */  MANGO_FT_IGR_FRAME_TYPE_L4tf,
        /* mask field name  */  MANGO_FT_IGR_BMSK_FRAME_TYPE_L4tf,
        /* position         */  10,
        /* field name       */  MATCH_FIELD_L4_PROTO,
        /* field pointer    */  DAL_MANGO_MFIELD_FRAME_TYPE_L4
    },
    {   /* data field       */  0,
        /* mask field name  */  0,
        /* position         */  0,
        /* field name       */  MATCH_FIELD_END,
        /* field pointer    */  NULL
    },
};  /* dal_mango_of_fixField_list */

dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_DROP_PRECEDENCE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0xe,
        /* length */                 2,
        /* data offset */            0x0
    },
};

dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_LB_TIME[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0xC,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OUTPUT_ACTION[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0xC,
        /* length */                 4,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OUTPUT_DATA[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DLP,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_METADATA[] =
{
    {
        /* template field type */    TMPLTE_FIELD_META_DATA,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_TRUNK_PRESENT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};

dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_TRUNK_ID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SLP,
        /* offset address */         0x0,
        /* length */                 7,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IN_PHY_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_PKT_INFO,
        /* offset address */         0xA,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IN_PHY_PORTMASK[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OUT_PHY_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DPM3,
        /* offset address */         0x9,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OUT_PHY_PORTMASK[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ETH_DST[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ETH_SRC[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OTAG_PCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xD,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OTAG_DEI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0xC,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OTAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_OTAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ITAG_PCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xD,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ITAG_CFI[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0xc,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ITAG_VID[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ITAG,
        /* offset address */         0x0,
        /* length */                 12,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ETHER_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_ETHERTYPE,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ARP_SHA[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ARP_THA[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ARP_OP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OMPLS_LABEL[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OMPLS_TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x4,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OMPLS_LABEL_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0xE,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_OMPLS_BOS[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IMPLS_LABEL[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IMPLS_TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x4,
        /* length */                 3,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IMPLS_LABEL_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TTID,
        /* offset address */         0xF,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IMPLS_BOS[] =
{
    {
        /* template field type */    TMPLTE_FIELD_SNAP_OUI,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_DSAP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_SSAP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_DSAP_SSAP,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_SNAP_OUI[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP4_SRC[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP4_DST[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_SRC[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_DST[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IPV6_FLABEL[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP4TOS_IP6TC[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_ECN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x8,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_DSCP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0xA,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_PROTO[] =
{
    {
        /* template field type */    TMPLTE_FIELD_IP_TOS_PROTO,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x3,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_FRAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x2,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_TTL[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x0,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_HDR_UNSEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xE,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_HDR_UNREP[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xD,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_NONEXT_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xC,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_MOB_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xB,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_ESP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0xA,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_AUTH_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x9,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_DEST_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_FRAG_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x7,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_ROUTING_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x6,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP6_HOP_HDR_EXIST[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L34_HEADER,
        /* offset address */         0x5,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IGMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IGMP_MAX_RESP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ICMP_CODE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_ICMP_TYPE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_L4_HDR[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_L4_SRC_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_SPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_L4_DST_PORT[] =
{
    {
        /* template field type */    TMPLTE_FIELD_L4_DPORT,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_TCP_ECN[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x6,
        /* length */                 2,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_TCP_FLAG[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x0,
        /* length */                 6,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_TCP_NONZERO_SEQ[] =
{
    {
        /* template field type */    TMPLTE_FIELD_TCP_INFO,
        /* offset address */         0x8,
        /* length */                 1,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_RANGE_CHK,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_IP_RANGE[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIRST_MPLS2,
        /* offset address */         0x8,
        /* length */                 8,
        /* data offset */            0x0,
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR_VALID_MSK[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* offset address */         0x0,
        /* length */                 14,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR0[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR1[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR2[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR3[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR4[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR5[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR6[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR7[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR8[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR9[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR10[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR11[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR12[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_12,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_FIELD_SELECTOR13[] =
{
    {
        /* template field type */    TMPLTE_FIELD_FIELD_SELECTOR_13,
        /* offset address */         0x0,
        /* length */                 16,
        /* data offset */            0x0
    },
};
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_GRE_KEY[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_VXLAN_VNI[] =
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
dal_mango_of_fieldLocation_t DAL_MANGO_MFIELD_GTP_TEID[] =
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

dal_mango_of_entryField_t dal_mango_of_field_list[] =
{
    {   /* field name    */           MATCH_FIELD_DROP_PRECEDENCE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_DROP_PRECEDENCE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_LOOPBACK_TIME,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_LB_TIME,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_OUTPUT_ACTION,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OUTPUT_ACTION,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_OUTPUT_DATA,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OUTPUT_DATA,
        /* valid phase   */           (1 << FT_PHASE_EGR_FT_0)
    },
    {   /* field name    */           MATCH_FIELD_METADATA,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_METADATA,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_TRUNK_PRESENT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_TRUNK_PRESENT,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_TRUNK_ID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_TRUNK_ID,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IN_PHY_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IN_PHY_PORT,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IN_PHY_PORTMASK,
        /* field number  */           4,
        /* field pointer */           DAL_MANGO_MFIELD_IN_PHY_PORTMASK,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_OUT_PHY_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OUT_PHY_PORT,
        /* valid phase   */           (1 << FT_PHASE_EGR_FT_0)
    },
    {   /* field name    */           MATCH_FIELD_OUT_PHY_PORTMASK,
        /* field number  */           4,
        /* field pointer */           DAL_MANGO_MFIELD_OUT_PHY_PORTMASK,
        /* valid phase   */           (1 << FT_PHASE_EGR_FT_0)
    },
    {   /* field name    */           MATCH_FIELD_ETH_DST,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_MFIELD_ETH_DST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ETH_SRC,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_MFIELD_ETH_SRC,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_OTAG_PCP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OTAG_PCP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_OTAG_DEI,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OTAG_DEI,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_OTAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OTAG_VID,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ITAG_PCP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ITAG_PCP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ITAG_CFI,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ITAG_CFI,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ITAG_VID,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ITAG_VID,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ETH_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ETHER_TYPE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ARP_SHA,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_MFIELD_ARP_SHA,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ARP_THA,
        /* field number  */           3,
        /* field pointer */           DAL_MANGO_MFIELD_ARP_THA,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ARP_OP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ARP_OP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_OMPLS_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_OMPLS_LABEL,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_OMPLS_TC,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OMPLS_TC,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_OMPLS_LABEL_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OMPLS_LABEL_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_OMPLS_BOS,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_OMPLS_BOS,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_IMPLS_LABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_IMPLS_LABEL,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_IMPLS_TC,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IMPLS_TC,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_IMPLS_LABEL_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IMPLS_LABEL_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_IMPLS_BOS,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IMPLS_BOS,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_DSAP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_DSAP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_SSAP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_SSAP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_SNAP_OUI,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_SNAP_OUI,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_IPV4_SRC,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_IP4_SRC,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IPV4_DST,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_IP4_DST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IPV6_SRC,
        /* field number  */           8,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_SRC,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IPV6_DST,
        /* field number  */           8,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_DST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IPV6_FLABEL,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_IPV6_FLABEL,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP4TOS_IP6TC,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP4TOS_IP6TC,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_ECN,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_ECN,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_DSCP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_DSCP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_PROTO,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_PROTO,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_FLAG,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_FRAG,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_FRAG,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_TTL,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_TTL,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_HDR_UNSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_HDR_UNSEQ,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_HDR_UNREP,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_HDR_UNREP,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_NONEXT_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_NONEXT_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_MOB_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_MOB_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_ESP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_ESP_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_AUTH_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_AUTH_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_DEST_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_DEST_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_FRAG_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_FRAG_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_ROUTING_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_ROUTING_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP6_HOP_HDR_EXIST,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP6_HOP_HDR_EXIST,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IGMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IGMP_TYPE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IGMP_MAX_RESP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IGMP_MAX_RESP_CODE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ICMP_CODE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ICMP_CODE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_ICMP_TYPE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_ICMP_TYPE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_L4_HDR,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_L4_HDR,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_L4_SRC_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_L4_SRC_PORT,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_L4_DST_PORT,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_L4_DST_PORT,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_TCP_ECN,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_TCP_ECN,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_TCP_FLAG,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_TCP_FLAG,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_TCP_NONZEROSEQ,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_TCP_NONZERO_SEQ,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_VID_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_RANGE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_L4_PORT_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_RANGE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_IP_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_IP_RANGE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_LEN_RANGE,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_RANGE,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3) | (1 << FT_PHASE_EGR_FT_0))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR_VALID_MSK,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR_VALID_MSK,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR0,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR0,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR1,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR1,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR2,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR2,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR3,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR3,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR4,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR4,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR5,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR5,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR6,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR6,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR7,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR7,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR8,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR8,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR9,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR9,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR10,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR10,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR11,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR11,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR12,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR12,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_FIELD_SELECTOR13,
        /* field number  */           1,
        /* field pointer */           DAL_MANGO_MFIELD_FIELD_SELECTOR13,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_GRE_KEY,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_GRE_KEY,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_VXLAN_VNI,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_VXLAN_VNI,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_GTP_TEID,
        /* field number  */           2,
        /* field pointer */           DAL_MANGO_MFIELD_GTP_TEID,
        /* valid phase   */           ((1 << FT_PHASE_IGR_FT_0) | (1 << FT_PHASE_IGR_FT_3))
    },
    {   /* field name    */           MATCH_FIELD_END,
        /* field number  */           0,
        /* field pointer */           NULL
    },
};  /* dal_mango_of_field_list */

static uint32 template_igr_flowtbl_data_field[DAL_MANGO_OF_TMP_FIELD_NUM] = {
    MANGO_FT_IGR_FIELD_0tf, MANGO_FT_IGR_FIELD_1tf,
    MANGO_FT_IGR_FIELD_2tf, MANGO_FT_IGR_FIELD_3tf,
    MANGO_FT_IGR_FIELD_4tf, MANGO_FT_IGR_FIELD_5tf,
    MANGO_FT_IGR_FIELD_6tf, MANGO_FT_IGR_FIELD_7tf,
    MANGO_FT_IGR_FIELD_8tf, MANGO_FT_IGR_FIELD_9tf,
    MANGO_FT_IGR_FIELD_10tf, MANGO_FT_IGR_FIELD_11tf,
    MANGO_FT_IGR_FIELD_12tf, MANGO_FT_IGR_FIELD_13tf};

static uint32 template_igr_flowtbl_mask_field[DAL_MANGO_OF_TMP_FIELD_NUM] = {
    MANGO_FT_IGR_BMSK_FIELD_0tf, MANGO_FT_IGR_BMSK_FIELD_1tf,
    MANGO_FT_IGR_BMSK_FIELD_2tf, MANGO_FT_IGR_BMSK_FIELD_3tf,
    MANGO_FT_IGR_BMSK_FIELD_4tf, MANGO_FT_IGR_BMSK_FIELD_5tf,
    MANGO_FT_IGR_BMSK_FIELD_6tf, MANGO_FT_IGR_BMSK_FIELD_7tf,
    MANGO_FT_IGR_BMSK_FIELD_8tf, MANGO_FT_IGR_BMSK_FIELD_9tf,
    MANGO_FT_IGR_BMSK_FIELD_10tf, MANGO_FT_IGR_BMSK_FIELD_11tf,
    MANGO_FT_IGR_BMSK_FIELD_12tf, MANGO_FT_IGR_BMSK_FIELD_13tf};

static uint32 template_egr_flowtbl_data_field[DAL_MANGO_OF_TMP_FIELD_NUM] = {
    MANGO_FT_EGR_FIELD_0tf, MANGO_FT_EGR_FIELD_1tf,
    MANGO_FT_EGR_FIELD_2tf, MANGO_FT_EGR_FIELD_3tf,
    MANGO_FT_EGR_FIELD_4tf, MANGO_FT_EGR_FIELD_5tf,
    MANGO_FT_EGR_FIELD_6tf, MANGO_FT_EGR_FIELD_7tf,
    MANGO_FT_EGR_FIELD_8tf, MANGO_FT_EGR_FIELD_9tf,
    MANGO_FT_EGR_FIELD_10tf, MANGO_FT_EGR_FIELD_11tf,
    MANGO_FT_EGR_FIELD_12tf, MANGO_FT_EGR_FIELD_13tf};

static uint32 template_egr_flowtbl_mask_field[DAL_MANGO_OF_TMP_FIELD_NUM] = {
    MANGO_FT_EGR_BMSK_FIELD_0tf, MANGO_FT_EGR_BMSK_FIELD_1tf,
    MANGO_FT_EGR_BMSK_FIELD_2tf, MANGO_FT_EGR_BMSK_FIELD_3tf,
    MANGO_FT_EGR_BMSK_FIELD_4tf, MANGO_FT_EGR_BMSK_FIELD_5tf,
    MANGO_FT_EGR_BMSK_FIELD_6tf, MANGO_FT_EGR_BMSK_FIELD_7tf,
    MANGO_FT_EGR_BMSK_FIELD_8tf, MANGO_FT_EGR_BMSK_FIELD_9tf,
    MANGO_FT_EGR_BMSK_FIELD_10tf, MANGO_FT_EGR_BMSK_FIELD_11tf,
    MANGO_FT_EGR_BMSK_FIELD_12tf, MANGO_FT_EGR_BMSK_FIELD_13tf};

/*
 * Macro Declaration
 */
/* semaphore handling */
#define OF_SEM_LOCK(unit)                                                          \
do {                                                                               \
    if (osal_sem_mutex_take(of_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)     \
    {                                                                               \
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_OPENFLOW),"semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;                                              \
    }                                                                               \
} while(0)

#define OF_SEM_UNLOCK(unit)                                                         \
do {                                                                                \
    if (osal_sem_mutex_give(of_sem[unit]) != RT_ERR_OK)                             \
    {                                                                               \
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_OPENFLOW),"semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;                                             \
    }                                                                                \
} while(0)

#define DAL_MANGO_PHASE_OF_TO_PIE(_ofPhase, _piePhase)                      \
do {                                                                        \
    switch (_ofPhase)                                                       \
    {                                                                       \
        case FT_PHASE_IGR_FT_0:                                             \
            _piePhase = PIE_PHASE_IGR_FLOW_TABLE_0;                         \
            break;                                                          \
        case FT_PHASE_IGR_FT_3:                                             \
            _piePhase = PIE_PHASE_IGR_FLOW_TABLE_3;                         \
            break;                                                          \
        case FT_PHASE_EGR_FT_0:                                             \
            _piePhase = PIE_PHASE_EGR_FLOW_TABLE_0;                         \
            break;                                                          \
        default:                                                            \
            return RT_ERR_OF_FT_PHASE;                                      \
    }                                                                       \
} while(0)

#define DAL_MANGO_OF_BLKINDEX_TO_PHYSICAL(_unit, _phase, _logicIdx, _phyIdx)\
do {                                                                        \
    rtk_pie_phase_t _pphase;                                                \
    int32           _ret;                                                   \
    if (PIE_ENTRY_IS_PHYSICAL_TYPE(_logicIdx))                              \
    {                                                                       \
        _phyIdx = PIE_ENTRY_PHYSICAL_CLEAR(_logicIdx);                      \
        RT_PARAM_CHK((_phyIdx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_INPUT);\
    }                                                                       \
    else                                                                    \
    {                                                                       \
        RT_PARAM_CHK((_logicIdx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit)), RT_ERR_INPUT);\
        switch (_phase)                                                     \
        {                                                                   \
            case FT_PHASE_IGR_FT_0:                                         \
                _pphase = PIE_PHASE_IGR_FLOW_TABLE_0;                       \
                break;                                                      \
            case FT_PHASE_IGR_FT_3:                                         \
                _pphase = PIE_PHASE_IGR_FLOW_TABLE_3;                       \
                break;                                                      \
            case FT_PHASE_EGR_FT_0:                                         \
                _pphase = PIE_PHASE_EGR_FLOW_TABLE_0;                       \
                break;                                                      \
            default:                                                        \
                return RT_ERR_OF_FT_PHASE;                                  \
        }                                                                   \
        RT_ERR_CHK(_dal_mango_pie_blockIdx_trans(_unit, _pphase, _logicIdx, &_phyIdx), _ret);\
    }                                                                       \
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "phy_block_idx=%d",_phyIdx);  \
} while(0)

#define DAL_MANGO_OF_INDEX_TO_PHYSICAL(_unit, _phase, _logicIdx, _phyIdx)   \
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
            case FT_PHASE_IGR_FT_0:                                         \
                _pphase = PIE_PHASE_IGR_FLOW_TABLE_0;                       \
                break;                                                      \
            case FT_PHASE_IGR_FT_3:                                         \
                _pphase = PIE_PHASE_IGR_FLOW_TABLE_3;                       \
                break;                                                      \
            case FT_PHASE_EGR_FT_0:                                         \
                _pphase = PIE_PHASE_EGR_FLOW_TABLE_0;                       \
                break;                                                      \
            default:                                                        \
                return RT_ERR_OF_FT_PHASE;                                  \
        }                                                                   \
        RT_ERR_CHK(_dal_mango_pie_entryIdx_trans(_unit, _pphase, _logicIdx, &_phyIdx), _ret);\
    }                                                                       \
} while(0)

#define DAL_MANGO_PHASE_FLOWTBL_TO_PIE(_ftPhase, _piePhase)                 \
do {                                                                        \
    switch (_ftPhase)                                                       \
    {                                                                       \
        case FT_PHASE_IGR_FT_0:                                             \
            _piePhase = PIE_PHASE_IGR_FLOW_TABLE_0;                         \
            break;                                                          \
        case FT_PHASE_IGR_FT_3:                                             \
            _piePhase = PIE_PHASE_IGR_FLOW_TABLE_3;                         \
            break;                                                          \
        case FT_PHASE_EGR_FT_0:                                             \
            _piePhase = PIE_PHASE_EGR_FLOW_TABLE_0;                         \
            break;                                                          \
        default:                                                            \
            return RT_ERR_OF_FT_PHASE;                                      \
    }                                                                       \
} while(0)

#define DAL_MANGO_IPV6_U32_TO_U8(_in, _out)                                 \
do {                                                                        \
    _out[0]  = (_in[0] & 0xFF000000) >> 24;                                 \
    _out[1]  = (_in[0] & 0x00FF0000) >> 16;                                 \
    _out[2]  = (_in[0] & 0x0000FF00) >> 8;                                  \
    _out[3]  = (_in[0] & 0x000000FF) >> 0;                                  \
    _out[4]  = (_in[1] & 0xFF000000) >> 24;                                 \
    _out[5]  = (_in[1] & 0x00FF0000) >> 16;                                 \
    _out[6]  = (_in[1] & 0x0000FF00) >> 8;                                  \
    _out[7]  = (_in[1] & 0x000000FF) >> 0;                                  \
    _out[8]  = (_in[2] & 0xFF000000) >> 24;                                 \
    _out[9]  = (_in[2] & 0x00FF0000) >> 16;                                 \
    _out[10] = (_in[2] & 0x0000FF00) >> 8;                                  \
    _out[11] = (_in[2] & 0x000000FF) >> 0;                                  \
    _out[12] = (_in[3] & 0xFF000000) >> 24;                                 \
    _out[13] = (_in[3] & 0x00FF0000) >> 16;                                 \
    _out[14] = (_in[3] & 0x0000FF00) >> 8;                                  \
    _out[15] = (_in[3] & 0x000000FF) >> 0;                                  \
} while(0)

/* check whether the entry index is valid corresponding to the returned/add entry type */
#define DAL_MANGO_OF_L3CAM_IDX_CHK(entry, index)                                        \
do {                                                                                    \
    if (entry.ip_ver == MANGO_OF_L3_IPVER_IP4 && (index % 2) != 0)                        \
        return RT_ERR_ENTRY_INDEX;                                                  \
                                                                                        \
    if (entry.ip_ver == MANGO_OF_L3_IPVER_IP6 &&                                          \
        (entry.type == MANGO_OF_L3ENTRY_TYPE_SIP || entry.type == MANGO_OF_L3ENTRY_TYPE_DIP) && \
        (index % 3) != 0)                                                           \
        return RT_ERR_ENTRY_INDEX;                                                  \
                                                                                        \
    if (entry.ip_ver == MANGO_OF_L3_IPVER_IP6 && entry.type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP && (index % 6) != 0) \
        return RT_ERR_ENTRY_INDEX;                                                  \
} while(0)


#define OF_BLK_BIT_OFST    7   // 128 entries per block
#define OF_BLK_IDX_MSK     (0x1F << OF_BLK_BIT_OFST)   // 32 blocks
#define OF_ENTRY_IDX_MSK   ((1 << OF_BLK_BIT_OFST) - 1)

#define OF_MV_DBG_FLAG     0
#define OF_DEL_DBG_FLAG    0

#define OF_MV_DBG(_fmt, args...) \
do { \
    if (OF_MV_DBG_FLAG) {osal_printf(_fmt, args);} \
    else {RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), _fmt, args);} \
} while(0)

#define OF_DEL_DBG(_fmt, args...) \
do { \
    if (OF_DEL_DBG_FLAG) {osal_printf(_fmt, args);} \
    else {RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), _fmt, args);} \
} while(0)

#define OF_TBL_READ(_u, _t, _idx, _p)  _dal_mango_of_tbl_read(_u, _t, _idx, _p)
#define OF_TBL_WRITE(_u, _t, _idx, _p) _dal_mango_of_tbl_write(_u, _t, _idx, _p)


/*
 * Function Declaration
 */

static int32
_dal_mango_of_l3HashFlowEntry_get(uint32 unit, uint32 entry_idx, dal_mango_of_l3HashEntry_t *pEntry);
int32
_dal_mango_of_dmacEntry_get(uint32 unit, uint32 idx, dal_mango_of_dmacEntry_t *pEntry);
static int32
_dal_mango_of_l2PortMaskEntry_set(uint32 unit, rtk_portmask_t *portmask, uint32 *pIndex);

static int32
_dal_mango_of_entry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx,
    rtk_bitmap_t *pDeletedInfo);

static int32
_dal_mango_of_tbl_read(uint32 unit, uint32 tbl, uint32 idx, uint32 *pEntry)
{
    uint32  *pShadowEntry, entryByteNum;

    RT_PARAM_CHK((!pOF_TBL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_FILTER_ID(unit) <= idx), RT_ERR_OUT_OF_RANGE);

    entryByteNum = sizeof(of_igr_entry_t);
    pShadowEntry = pOF_TBL + ((entryByteNum / sizeof(uint32)) * idx);

    osal_memcpy(pEntry, pShadowEntry, entryByteNum);
    //RT_ERR_CHK(table_read(unit, tbl, idx, entry), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_of_tbl_read */

static int32
_dal_mango_of_tbl_write(uint32 unit, uint32 tbl, uint32 idx, uint32 *pEntry)
{
    uint32  *pShadowEntry, entryByteNum;
    int32   ret;

    RT_PARAM_CHK((!pOF_TBL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_FILTER_ID(unit) <= idx), RT_ERR_OUT_OF_RANGE);

    entryByteNum = sizeof(of_igr_entry_t);
    pShadowEntry = pOF_TBL + ((entryByteNum / sizeof(uint32)) * idx);

    RT_ERR_CHK(table_write(unit, tbl, idx, pEntry), ret);
    osal_memcpy(pShadowEntry, pEntry, entryByteNum);

    return RT_ERR_OK;
}   /* end of _dal_mango_of_tbl_write */

static int32
_dal_mango_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    uint32  i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,phase=%d,field_id=%d,type=%d", unit, phase, field_id, type);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((OF_SET_FIELD_TYPE_END <= type), RT_ERR_OF_SET_FIELD_TYPE);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            if(field_id > 4)
                return RT_ERR_OF_SET_FIELD_ID;
            break;
        case FT_PHASE_IGR_FT_1:
            if(field_id > 1)
                return RT_ERR_OF_SET_FIELD_ID;
            break;
        case FT_PHASE_IGR_FT_2:
            if(field_id > 2)
                return RT_ERR_OF_SET_FIELD_ID;
            break;
        case FT_PHASE_IGR_FT_3:
            if(field_id > 4)
                return RT_ERR_OF_SET_FIELD_ID;
            break;
        case FT_PHASE_EGR_FT_0:
            if(field_id > 1)
                return RT_ERR_OF_SET_FIELD_ID;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* search supported set field type according to specified phase and filed ID */
    for (i = 0; dal_mango_of_setField_list[phase][field_id].type[i] != OF_SET_FIELD_TYPE_END; ++i)
    {
        if (type == dal_mango_of_setField_list[phase][field_id].type[i])
            return RT_ERR_OK;
    }

    return RT_ERR_OF_SET_FIELD_TYPE;
}   /* end of dal_mango_of_flowEntrySetField_check */

/* Function Name:
 *      _dal_mango_of_field2Buf_get
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
_dal_mango_of_field2Buf_get(
    uint32 unit,
    dal_mango_of_fieldLocation_t *field,
    rtk_of_matchfieldType_t type,
    uint32 data,
    uint32 mask,
    uint8 *pData,
    uint8 *pMask)
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

    dal_mango_of_flowMatchFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_MANGO_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_MANGO_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_MANGO_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_MANGO_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_MANGO_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_MANGO_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_MANGO_MAX_INFO_IDX);
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
}   /* end of _dal_mango_acl_field2Buf_get */

static void
_dal_mango_of_buf2Field_get(
    uint32 unit,
    dal_mango_of_fieldLocation_t *field,
    rtk_of_matchfieldType_t type,
    uint16 *data16,
    uint16 *mask16,
    uint8  *pData,
    uint8  *pMask)
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

    field_offset = field->field_offset;
    field_length = field->field_length;
    data_offset = field->data_offset;

    dal_mango_of_flowMatchFieldSize_get(unit, type, &field_size);

    /* form the field data in chip view from user input */
    buf_byte_idx_num = DAL_MANGO_DATA_WIDTH_GET(field_size);
    buf_field_bit_max = data_offset + field_length - 1;
    buf_field_byte_idx_start = DAL_MANGO_GET_BYTE_IDX(data_offset);
    buf_field_byte_idx_end = DAL_MANGO_GET_BYTE_IDX(buf_field_bit_max);
    tmp_data_offset = data_offset % DAL_MANGO_DATA_BITS;
    buf_field_bit_start = data_offset;
    chip_field_offset = field_offset;

    for (i = buf_field_byte_idx_start; i <= buf_field_byte_idx_end; ++i)
    {
        buf_field_offset = DAL_MANGO_GET_INFO_OFFSET(buf_byte_idx_num, i) + tmp_data_offset;
        tmp_data_offset = 0;
        buf_field_bit_end = ((i + 1) * DAL_MANGO_DATA_BITS) - 1;
        if (buf_field_bit_end > buf_field_bit_max)
            buf_field_bit_end = buf_field_bit_max;

        mask_len = buf_field_bit_end - buf_field_bit_start + 1;

        tmp_offset = ((buf_byte_idx_num - i - 1) / DAL_MANGO_MAX_INFO_IDX);
        pTmp_data = (uint32 *)pData + tmp_offset;
        pTmp_mask = (uint32 *)pMask + tmp_offset;

        *data16 |= ((*pTmp_data >> buf_field_offset) & ((1 << mask_len)-1)) << chip_field_offset;
        *mask16 |= ((*pTmp_mask >> buf_field_offset) & ((1 << mask_len)-1)) << chip_field_offset;

        chip_field_offset += mask_len;

        buf_field_bit_start = buf_field_bit_end + 1;
    }
}   /* end of _dal_mango_of_buf2Field_get */

static int32
_dal_mango_of_ruleEntryBufField_get(
    uint32 unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entryIdx,
    rtk_of_matchfieldType_t  type,
    uint32 *buf,
    uint8  *pData,
    uint8  *pMask)
{
    rtk_of_ftTemplateIdx_t          tmplteIdx;
    rtk_pie_template_t              tmplte;
    dal_mango_of_entryField_t       *fieldList = NULL;
    dal_mango_of_fieldLocation_t    *fieldLocation = NULL;
    uint32                          *tmplteDataFieldList = NULL;
    uint32                          *tmplteMaskFieldList = NULL;
    uint32                          table, field;
    uint32                          i, val;
    uint32                          blkIdx, fieldIdx, fieldNum;
    uint32                          fieldData = 0, fieldMask = 0;
    uint32                          data, mask;
    uint32                          fieldMatch = FALSE;
    uint32                          dalTypeIdx;
    rtk_pie_phase_t                 pphase;
    int32                           ret;


    osal_memset(pData, 0, RTK_OF_MATCH_FIELD_MAX);
    osal_memset(pMask, 0, RTK_OF_MATCH_FIELD_MAX);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            field = MANGO_FT_IGR_TIDtf;
            tmplteDataFieldList = template_igr_flowtbl_data_field;
            tmplteMaskFieldList = template_igr_flowtbl_mask_field;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            field = MANGO_FT_EGR_TIDtf;
            tmplteDataFieldList = template_egr_flowtbl_data_field;
            tmplteMaskFieldList = template_egr_flowtbl_mask_field;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* handle fixed field type */
    for (i = 0; dal_mango_of_fixField_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_fixField_list[i].type)
        {
            fieldData = dal_mango_of_fixField_list[i].data_field;
            fieldMask = dal_mango_of_fixField_list[i].mask_field;

            if ((ret = table_field_get(unit, table, fieldData, &data, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, table, fieldMask, &mask, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
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
    _dal_mango_pie_templateSelector_get(unit, blkIdx, &tmplteIdx.template_id[0], &tmplteIdx.template_id[1]);
    /* get template fields */
    if ((ret = table_field_get(unit, table, field, &data, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_pie_template_t));
    val = tmplteIdx.template_id[data];
    DAL_MANGO_PHASE_FLOWTBL_TO_PIE(phase, pphase);
    dal_mango_pie_phaseTemplate_get(unit, pphase, val, &tmplte);

    /* translate field from RTK superset view to DAL view */
    for (i = 0; dal_mango_of_field_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_field_list[i].type)
        {
            if(dal_mango_of_field_list[i].valid_phase & (1 << phase))
            {
                dalTypeIdx = i;
                fieldList = dal_mango_of_field_list;
                break;
            }
        }
    }

    if (fieldList == NULL)
    {
        return RT_ERR_OF_FIELD_TYPE;
    }

    /* search template to check all field types */
    fieldMatch = FALSE;
    for (fieldNum = 0; fieldNum < fieldList[dalTypeIdx].fieldNumber; ++fieldNum)
    {
        for (fieldIdx = 0; fieldIdx < DAL_MANGO_OF_TMP_FIELD_NUM; ++fieldIdx)
        {
            fieldLocation = &fieldList[dalTypeIdx].pField[fieldNum];

            /* check whether the user field type is pull in template, partial match is also allowed */
            if (fieldLocation->template_field_type == tmplte.field[fieldIdx])
            {
                fieldMatch = TRUE;

                fieldData = tmplteDataFieldList[fieldIdx];
                fieldMask = tmplteMaskFieldList[fieldIdx];

                /* data */
                if ((ret = table_field_get(unit, table,
                        fieldData, &data, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, table,
                        fieldMask, &mask, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                /* find out corresponding data field from field data */
                _dal_mango_of_field2Buf_get(unit, fieldLocation, type, data, mask, pData, pMask);
            }
        }
    }

    /* can't find then return */
    if (fieldMatch != TRUE)
    {
        return RT_ERR_OF_FIELD_TYPE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pData=%x, pMask=%x", *pData, *pMask);

    return RT_ERR_OK;
}   /* end of _dal_mango_acl_ruleEntryBufField_get */

static int32
_dal_mango_of_ruleEntryBufField_set(
    uint32 unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entryIdx,
    rtk_of_matchfieldType_t  type,
    uint32 *buf,
    uint8  *pData,
    uint8  *pMask)
{
    rtk_of_ftTemplateIdx_t          tmplteIdx;
    rtk_pie_template_t              tmplte;
    dal_mango_of_entryField_t       *fieldList = NULL;
    dal_mango_of_fieldLocation_t    *fieldLocation = NULL;
    uint32                          *tmplteDataFieldList = NULL;
    uint32                          *tmplteMaskFieldList = NULL;
    uint32                          table, field;
    uint32                          i, val;
    uint32                          blkIdx, fieldIdx, fieldNum;
    uint32                          fieldData = 0, fieldMask = 0;
    uint32                          data, mask;
    uint16                          data16 = 0, mask16 = 0;
    uint32                          fieldMatch = FALSE;
    uint32                          dalTypeIdx;
    rtk_pie_phase_t                 pphase;
    int32                           ret;

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            field = MANGO_FT_IGR_TIDtf;
            tmplteDataFieldList = template_igr_flowtbl_data_field;
            tmplteMaskFieldList = template_igr_flowtbl_mask_field;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            field = MANGO_FT_EGR_TIDtf;
            tmplteDataFieldList = template_egr_flowtbl_data_field;
            tmplteMaskFieldList = template_egr_flowtbl_mask_field;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* handle fixed field type */
    for (i = 0; dal_mango_of_fixField_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_fixField_list[i].type)
        {
            data = *pData;
            mask = *pMask;

            fieldData = dal_mango_of_fixField_list[i].data_field;
            fieldMask = dal_mango_of_fixField_list[i].mask_field;

            if ((ret = table_field_set(unit, table, fieldData, &data, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, table, fieldMask, &mask, buf)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    /* caculate entry in which block */
    blkIdx = entryIdx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    /* find out the binding template of the block */
    _dal_mango_pie_templateSelector_get(unit, blkIdx, &tmplteIdx.template_id[0], &tmplteIdx.template_id[1]);
    /* get field 'template ID' to know the template that the entry maps to */
    if ((ret = table_field_get(unit, table, field, &data, buf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    osal_memset(&tmplte, 0, sizeof(rtk_pie_template_t));
    val = tmplteIdx.template_id[data];
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "Fix field tid: %d, template ID: %d", data, val);
    DAL_MANGO_PHASE_FLOWTBL_TO_PIE(phase, pphase);
    dal_mango_pie_phaseTemplate_get(unit, pphase, val, &tmplte);

    /* translate field from RTK superset view to DAL view */
    for (i = 0; dal_mango_of_field_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_field_list[i].type)
        {
            if(dal_mango_of_field_list[i].valid_phase & (1 << phase))
            {
                dalTypeIdx = i;
                fieldList = dal_mango_of_field_list;
                break;
            }
        }
    }

    if (fieldList == NULL)
    {
        return RT_ERR_OF_FIELD_TYPE;
    }

    /* search template to check all field types */
    fieldMatch = FALSE;
    for (fieldNum = 0; fieldNum < fieldList[dalTypeIdx].fieldNumber; ++fieldNum)
    {
        for (fieldIdx = 0; fieldIdx < DAL_MANGO_OF_TMP_FIELD_NUM; ++fieldIdx)
        {
            fieldLocation = &fieldList[dalTypeIdx].pField[fieldNum];

            /* check whether the user field type is pulled in template, partial match is allowed */
            if (fieldLocation->template_field_type == tmplte.field[fieldIdx])
            {
                uint32  fieldOfst, fieldLen;

                fieldMatch = TRUE;

                fieldOfst = fieldLocation->field_offset;
                fieldLen = fieldLocation->field_length;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pData: 0x%X%X, pMask: 0x%X%X", pData[0], pData[1], pMask[0], pMask[1]);
                _dal_mango_of_buf2Field_get(unit, fieldLocation, type, &data16, &mask16, pData, pMask);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "data16: 0x%X, mask16: 0x%X", data16, mask16);
                fieldData = tmplteDataFieldList[fieldIdx];
                fieldMask = tmplteMaskFieldList[fieldIdx];

                /* data */
                if ((ret = table_field_get(unit, table, fieldData, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
                val &= ~(((1 << fieldLen)-1) << fieldOfst);
                val |= data16;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "data val: 0x%X", val);

                if ((ret = table_field_set(unit, table, fieldData, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                /* mask */
                if ((ret = table_field_get(unit, table, fieldMask, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
                val &= ~(((1 << fieldLen)-1) << fieldOfst);
                val |= mask16;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "mask val: 0x%X", val);

                if ((ret = table_field_set(unit, table, fieldMask, &val, buf)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
        }
    }

    /* no matched filed in template */
    if (fieldMatch != TRUE)
    {
        return RT_ERR_OF_FIELD_TYPE;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_of_ruleEntryBufField_set */

static int32
_dal_mango_of_igrFTInstruct_get(uint32 unit, rtk_of_flow_id_t entry_idx, dal_mango_of_igrFtIns_t *pIns)
{
    int32  ret;
    uint32 tblName;
    of_igr_entry_t insEntry;


    osal_memset((void *)&insEntry, 0x00, sizeof(of_igr_entry_t));
    tblName = MANGO_FT_IGRt;

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, tblName, entry_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_INS_GOTO_TBLtf, &pIns->gotoTbl_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_INS_WRITE_METADATAtf, &pIns->writeMd_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_INS_WRITE_ACTtf, &pIns->writeAct_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_INS_CLR_ACTtf, &pIns->clearAct_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_INS_METERtf, &pIns->meter_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_METER_IDXtf, &pIns->meterIdx,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_METER_YELLOW_ACTtf, &pIns->yellow_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_METER_RED_ACTtf, &pIns->red_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_COPY_TTL_INWARDtf, &pIns->cp_ttl_inward,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_POP_VLANtf, &pIns->pop_vlan,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_POP_MPLStf, &pIns->pop_mpls,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_POP_MPLS_TYPEtf, &pIns->pop_mpls_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_PUSH_MPLStf, &pIns->push_mpls,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_PUSH_MPLS_MODEtf, &pIns->push_mpls_mode,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_MPLS_VPN_TYPEtf, &pIns->push_mpls_vpnType,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_MPLS_LIB_IDXtf, &pIns->push_mpls_libIdx,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_MPLS_ETHTYPEtf,\
                                &pIns->push_mpls_ethType, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_PUSH_VLANtf,\
                                &pIns->push_vlan, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_VLAN_ETHTYPEtf,\
                                &pIns->etherType_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_COPY_TTL_OUTWARDtf, &pIns->cp_ttl_outward,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_DEC_MPLS_TTLtf, &pIns->dec_mpls_ttl,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_DEC_IP_TTLtf, &pIns->dec_ip_ttl,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD0_TYPEtf, &pIns->field0_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD0_DATAtf, \
                                &pIns->field0_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD1_TYPEtf, &pIns->field1_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD1_DATAtf, \
                                &pIns->field1_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD2_TYPEtf, &pIns->field2_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD2_DATAtf, \
                                &pIns->field2_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD3_TYPEtf, &pIns->field3_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD3_DATAtf, \
                                &pIns->field3_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD4_TYPEtf, &pIns->field4_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD4_DATAtf, \
                                &pIns->field4_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_Qtf,\
                                &pIns->set_queue, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_SET_Q_DATAtf,\
                                &pIns->qid, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_GROUPtf,\
                                &pIns->group, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_GROUP_IDXtf,\
                                &pIns->gid, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_OUTPUT_TYPEtf, &pIns->output_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_WA_OUTPUT_DATAtf, &pIns->output_data,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_METADATAtf, &pIns->metadata,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_METADATA_MSKtf, &pIns->metadata_mask,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_GOTO_TBL_ACTtf, &pIns->tbl_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_GOTO_TBL_IDtf, &pIns->tbl_id,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_IGR_GOTO_TBL_LB_TIMEtf, &pIns->tbl_lb_time,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_mango_of_igrFTInstruct_get */

static int32
_dal_mango_of_igrFTInstruct_set(uint32 unit, rtk_of_flow_id_t entry_idx, dal_mango_of_igrFtIns_t *pIns)
{
    int32  ret;
    uint32 tblName;
    of_igr_entry_t insEntry;


    osal_memset((void *)&insEntry, 0x00, sizeof(of_igr_entry_t));
    tblName = MANGO_FT_IGRt;

    /* read entry for preserving the portion other than instruction */
    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, tblName, entry_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_INS_GOTO_TBLtf, &pIns->gotoTbl_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_INS_WRITE_METADATAtf, &pIns->writeMd_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_INS_WRITE_ACTtf, &pIns->writeAct_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_INS_CLR_ACTtf, &pIns->clearAct_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_INS_METERtf, &pIns->meter_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_METER_IDXtf, &pIns->meterIdx,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_METER_YELLOW_ACTtf, &pIns->yellow_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_METER_RED_ACTtf, &pIns->red_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_COPY_TTL_INWARDtf, &pIns->cp_ttl_inward,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_POP_VLANtf, &pIns->pop_vlan,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_POP_MPLStf, &pIns->pop_mpls,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_POP_MPLS_TYPEtf, &pIns->pop_mpls_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_PUSH_MPLStf, &pIns->push_mpls,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_PUSH_MPLS_MODEtf, &pIns->push_mpls_mode,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_MPLS_VPN_TYPEtf, &pIns->push_mpls_vpnType,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_MPLS_LIB_IDXtf, &pIns->push_mpls_libIdx,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_MPLS_ETHTYPEtf,\
                                &pIns->push_mpls_ethType, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_PUSH_VLANtf,\
                                &pIns->push_vlan, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_VLAN_ETHTYPEtf,\
                                &pIns->etherType_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_COPY_TTL_OUTWARDtf, &pIns->cp_ttl_outward,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_DEC_MPLS_TTLtf, &pIns->dec_mpls_ttl,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_DEC_IP_TTLtf, &pIns->dec_ip_ttl,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD0_TYPEtf, &pIns->field0_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD0_DATAtf, \
                                &pIns->field0_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD1_TYPEtf, &pIns->field1_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD1_DATAtf, \
                                &pIns->field1_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD2_TYPEtf, &pIns->field2_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD2_DATAtf, \
                                &pIns->field2_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD3_TYPEtf, &pIns->field3_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD3_DATAtf, \
                                &pIns->field3_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD4_TYPEtf, &pIns->field4_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_FIELD4_DATAtf, \
                                &pIns->field4_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_Qtf,\
                                &pIns->set_queue, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_SET_Q_DATAtf,\
                                &pIns->qid, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_GROUPtf,\
                                &pIns->group, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_GROUP_IDXtf,\
                                &pIns->gid, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_OUTPUT_TYPEtf, &pIns->output_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_WA_OUTPUT_DATAtf, &pIns->output_data,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_METADATAtf, &pIns->metadata,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_METADATA_MSKtf, &pIns->metadata_mask,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_GOTO_TBL_ACTtf, &pIns->tbl_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_GOTO_TBL_IDtf, &pIns->tbl_id,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_IGR_GOTO_TBL_LB_TIMEtf, &pIns->tbl_lb_time,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_WRITE(unit, tblName, entry_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_mango_of_igrFTInstruct_set */

/* convert rtk_of_igrFT0Ins_t to dal_mango_of_igrFtIns_t */
static int32
_dal_mango_of_igrFTRtk2Dal_convert(uint32 unit, dal_mango_of_igrFtIns_t *pDest, rtk_of_igrFT0Ins_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(dal_mango_of_igrFtIns_t));

    pDest->meter_en = pSrc->meter_en;
    pDest->clearAct_en = pSrc->clearAct_en;
    pDest->writeAct_en = pSrc->writeAct_en;
    pDest->writeMd_en = pSrc->writeMetadata_en;
    pDest->gotoTbl_en = pSrc->gotoTbl_en;

    pDest->meterIdx = pSrc->meter_data.meter_id;
    switch (pSrc->meter_data.yellow)
    {
        case OF_METER_TYPE_DROP:
            pDest->yellow_act = MANGO_OF_METER_ACT_DROP;
            break;
        case OF_METER_TYPE_DSCP_REMARK:
            pDest->yellow_act = MANGO_OF_METER_ACT_DSCP_REMARK;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (pSrc->meter_data.red)
    {
        case OF_METER_TYPE_DROP:
            pDest->red_act = MANGO_OF_METER_ACT_DROP;
            break;
        case OF_METER_TYPE_DSCP_REMARK:
            pDest->red_act = MANGO_OF_METER_ACT_DSCP_REMARK;
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->cp_ttl_inward = pSrc->wa_data.cp_ttl_inward;

    pDest->pop_vlan = pSrc->wa_data.pop_vlan;

    pDest->pop_mpls = pSrc->wa_data.pop_mpls;
    switch (pSrc->wa_data.pop_mpls_type)
    {
        case OF_POP_MPLS_TYPE_OUTERMOST_LABEL:
            pDest->pop_mpls_type = MANGO_OF_POP_MPLS_TYPE_OUTERMOST;
            break;
        case OF_POP_MPLS_TYPE_DOUBLE_LABEL:
            pDest->pop_mpls_type = MANGO_OF_POP_MPLS_TYPE_DOUBLE;
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->push_mpls = pSrc->wa_data.push_mpls;
    switch (pSrc->wa_data.push_mpls_data.push_mode)
    {
        case OF_PUSH_MPLS_MODE_NORMAL:
            pDest->push_mpls_mode = MANGO_OF_PUSH_MPLS_MODE_NORMAL;
            break;
        case OF_PUSH_MPLS_MODE_ENCAPTBL:
            pDest->push_mpls_mode = MANGO_OF_PUSH_MPLS_MODE_ENCAPTBL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (pSrc->wa_data.push_mpls_data.vpn_type)
    {
        case OF_PUSH_MPLS_VPN_TYPE_L2:
            pDest->push_mpls_vpnType = MANGO_OF_PUSH_MPLS_VPN_TYPE_L2;
            break;
        case OF_PUSH_MPLS_VPN_TYPE_L3:
            pDest->push_mpls_vpnType = MANGO_OF_PUSH_MPLS_VPN_TYPE_L3;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->push_mpls_libIdx = pSrc->wa_data.push_mpls_data.mpls_encap_idx;
    pDest->push_mpls_ethType = pSrc->wa_data.push_mpls_data.etherType;

    pDest->push_vlan = pSrc->wa_data.push_vlan;
    pDest->etherType_idx = pSrc->wa_data.push_vlan_data.etherType_idx;

    pDest->cp_ttl_outward = pSrc->wa_data.cp_ttl_outward;

    pDest->dec_mpls_ttl= pSrc->wa_data.dec_mpls_ttl;

    pDest->dec_ip_ttl= pSrc->wa_data.dec_ip_ttl;

    switch (pSrc->wa_data.set_field_0_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE0_NONE:
            pDest->field0_type = MANGO_OF_SF_TYPE0_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE0_SA:
            pDest->field0_type = MANGO_OF_SF_TYPE0_SA;
            break;
        case OF_IGR_FT0_SF_TYPE0_VLAN_PRI:
            pDest->field0_type = MANGO_OF_SF_TYPE0_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE0_MPLS_TC:
            pDest->field0_type = MANGO_OF_SF_TYPE0_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE0_MPLS_TTL:
            pDest->field0_type = MANGO_OF_SF_TYPE0_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE0_IP_DSCP:
            pDest->field0_type = MANGO_OF_SF_TYPE0_IP_DSCP;
            break;
        case OF_IGR_FT0_SF_TYPE0_IP_TTL:
            pDest->field0_type = MANGO_OF_SF_TYPE0_IP_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE0_IP_FLAG_RSVD:
            pDest->field0_type = MANGO_OF_SF_TYPE0_IP_FLAG_RSVD;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field0_data = pSrc->wa_data.set_field_0_data.field_data;

    switch (pSrc->wa_data.set_field_1_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE1_NONE:
            pDest->field1_type = MANGO_OF_SF_TYPE1_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE1_DA:
            pDest->field1_type = MANGO_OF_SF_TYPE1_DA;
            break;
        case OF_IGR_FT0_SF_TYPE1_VLAN_PRI:
            pDest->field1_type = MANGO_OF_SF_TYPE1_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE1_MPLS_LABEL:
            pDest->field1_type = MANGO_OF_SF_TYPE1_MPLS_LABEL;
            break;
        case OF_IGR_FT0_SF_TYPE1_MPLS_TC:
            pDest->field1_type = MANGO_OF_SF_TYPE1_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE1_MPLS_TTL:
            pDest->field1_type = MANGO_OF_SF_TYPE1_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE1_IP_DSCP:
            pDest->field1_type = MANGO_OF_SF_TYPE1_IP_DSCP;
            break;
        case OF_IGR_FT0_SF_TYPE1_IP_TTL:
            pDest->field1_type = MANGO_OF_SF_TYPE1_IP_TTL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field1_data = pSrc->wa_data.set_field_1_data.field_data;

    switch (pSrc->wa_data.set_field_2_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE2_NONE:
            pDest->field2_type = MANGO_OF_SF_TYPE2_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE2_VLAN_ID:
            pDest->field2_type = MANGO_OF_SF_TYPE2_VLAN_ID;
            break;
        case OF_IGR_FT0_SF_TYPE2_VLAN_PRI:
            pDest->field2_type = MANGO_OF_SF_TYPE2_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE2_MPLS_LABEL:
            pDest->field2_type = MANGO_OF_SF_TYPE2_MPLS_LABEL;
            break;
        case OF_IGR_FT0_SF_TYPE2_MPLS_TC:
            pDest->field2_type = MANGO_OF_SF_TYPE2_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE2_MPLS_TTL:
            pDest->field2_type = MANGO_OF_SF_TYPE2_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE2_IP_DSCP:
            pDest->field2_type = MANGO_OF_SF_TYPE2_IP_DSCP;
            break;
        case OF_IGR_FT0_SF_TYPE2_IP_TTL:
            pDest->field2_type = MANGO_OF_SF_TYPE2_IP_TTL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field2_data = pSrc->wa_data.set_field_2_data.field_data;

    switch (pSrc->wa_data.set_field_3_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE3_NONE:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE3_SIP:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_SIP;
            break;
        case OF_IGR_FT0_SF_TYPE3_DIP:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_DIP;
            break;
        case OF_IGR_FT0_SF_TYPE3_VLAN_PRI:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE3_MPLS_LABEL:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_MPLS_LABEL;
            break;
        case OF_IGR_FT0_SF_TYPE3_MPLS_TC:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE3_MPLS_TTL:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE3_IP_DSCP:
            pDest->field3_type = MANGO_OF_IGR_SF_TYPE3_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field3_data = pSrc->wa_data.set_field_3_data.field_data;

    switch (pSrc->wa_data.set_field_4_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE4_NONE:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE4_L4_SPORT:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_SPORT;
            break;
        case OF_IGR_FT0_SF_TYPE4_L4_DPORT:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_DPORT;
            break;
        case OF_IGR_FT0_SF_TYPE4_VLAN_PRI:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE4_MPLS_LABEL:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_MPLS_LABEL;
            break;
        case OF_IGR_FT0_SF_TYPE4_MPLS_TC:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE4_MPLS_TTL:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE4_IP_DSCP:
            pDest->field4_type = MANGO_OF_IGR_SF_TYPE4_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field4_data = pSrc->wa_data.set_field_4_data.field_data;

    pDest->set_queue = pSrc->wa_data.set_queue;
    pDest->qid = pSrc->wa_data.qid;

    pDest->group = pSrc->wa_data.group;
    if (pSrc->wa_data.gid >= HAL_OF_MAX_NUM_OF_GRP_ENTRY(unit))
        return RT_ERR_INPUT;
    pDest->gid = pSrc->wa_data.gid;

    switch (pSrc->wa_data.output_data.type)
    {
        case OF_OUTPUT_TYPE_NONE:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_DISABLE;
                pDest->output_data = 0;
            break;
        case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
                pDest->output_data = (pSrc->wa_data.output_data.devID << 6) | (pSrc->wa_data.output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_PHY_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT;
                pDest->output_data = (pSrc->wa_data.output_data.devID << 6) | (pSrc->wa_data.output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
                pDest->output_data = pSrc->wa_data.output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_TRK_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT;
                pDest->output_data = pSrc->wa_data.output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT;
            break;
        case OF_OUTPUT_TYPE_IN_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_IN_PORT;
            break;
        case OF_OUTPUT_TYPE_FLOOD:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_FLOOD;
            break;
        case OF_OUTPUT_TYPE_LB:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_LB;
            break;
        case OF_OUTPUT_TYPE_TUNNEL:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_TUNNEL;
                /* convert interface to tunnel index */
                if (_dal_mango_tunnel_intf2tunnelIdx(unit, &pDest->output_data, pSrc->wa_data.output_data.intf) != RT_ERR_OK)
                    return RT_ERR_INPUT;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "intf=0x%x,tunnel_idx=%d", pSrc->wa_data.output_data.intf, pDest->output_data);
            break;
        case OF_OUTPUT_TYPE_FAILOVER:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_FAILOVER;
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->metadata = pSrc->wm_data.data;
    pDest->metadata_mask = pSrc->wm_data.mask;

    switch (pSrc->gt_data.type)
    {
        case OF_GOTOTBL_TYPE_NORMAL:
            pDest->tbl_act = MANGO_OF_GOTOTBL_NORMAL;
            if (pSrc->gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->tbl_id = pSrc->gt_data.tbl_id;
            break;
        case OF_GOTOTBL_TYPE_APPLY_AND_LB:
            pDest->tbl_act = MANGO_OF_GOTOTBL_APPLY_AND_LB;
            if (pSrc->gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->tbl_id = pSrc->gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->tbl_lb_time = pSrc->gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        case OF_GOTOTBL_TYPE_LB:
            pDest->tbl_act = MANGO_OF_GOTOTBL_LB;
            if (pSrc->gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->tbl_id = pSrc->gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->tbl_lb_time = pSrc->gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* convert dal_mango_of_igrFtIns_t to rtk_of_igrFT0Ins_t */
static int32
_dal_mango_of_igrFTDal2Rtk_convert(uint32 unit, rtk_of_igrFT0Ins_t *pDest, dal_mango_of_igrFtIns_t *pSrc)
{
    uint32 ret;
    dal_mango_l3_intfEntry_t l3_intf_entry;
    dal_mango_of_dmacEntry_t dmac_entry;
    rtk_mpls_encap_t         mpls_entry;

    osal_memset(pDest, 0x00, sizeof(rtk_of_igrFT0Ins_t));

    pDest->meter_en = pSrc->meter_en;
    pDest->clearAct_en = pSrc->clearAct_en;
    pDest->writeAct_en = pSrc->writeAct_en;
    pDest->writeMetadata_en = pSrc->writeMd_en;
    pDest->gotoTbl_en = pSrc->gotoTbl_en;

    pDest->meter_data.meter_id = pSrc->meterIdx;
    switch (pSrc->yellow_act)
    {
        case MANGO_OF_METER_ACT_DROP:
            pDest->meter_data.yellow = OF_METER_TYPE_DROP;
            break;
        case MANGO_OF_METER_ACT_DSCP_REMARK:
            pDest->meter_data.yellow = OF_METER_TYPE_DSCP_REMARK;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (pSrc->red_act)
    {
        case MANGO_OF_METER_ACT_DROP:
            pDest->meter_data.red = OF_METER_TYPE_DROP;
            break;
        case MANGO_OF_METER_ACT_DSCP_REMARK:
            pDest->meter_data.red = OF_METER_TYPE_DSCP_REMARK;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.cp_ttl_inward = pSrc->cp_ttl_inward;

    pDest->wa_data.pop_vlan = pSrc->pop_vlan;

    pDest->wa_data.pop_mpls = pSrc->pop_mpls;
    switch (pSrc->pop_mpls_type)
    {
        case MANGO_OF_POP_MPLS_TYPE_OUTERMOST:
            pDest->wa_data.pop_mpls_type = OF_POP_MPLS_TYPE_OUTERMOST_LABEL;
            break;
        case MANGO_OF_POP_MPLS_TYPE_DOUBLE:
            pDest->wa_data.pop_mpls_type = OF_POP_MPLS_TYPE_DOUBLE_LABEL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.push_mpls = pSrc->push_mpls;
    switch (pSrc->push_mpls_mode)
    {
        case MANGO_OF_PUSH_MPLS_MODE_NORMAL:
            pDest->wa_data.push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_NORMAL;
            break;
        case MANGO_OF_PUSH_MPLS_MODE_ENCAPTBL:
            pDest->wa_data.push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_ENCAPTBL;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (pSrc->push_mpls_vpnType)
    {
        case MANGO_OF_PUSH_MPLS_VPN_TYPE_L2:
            pDest->wa_data.push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L2;
            break;
        case MANGO_OF_PUSH_MPLS_VPN_TYPE_L3:
            pDest->wa_data.push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L3;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->wa_data.push_mpls_data.mpls_encap_idx = pSrc->push_mpls_libIdx;
    pDest->wa_data.push_mpls_data.etherType = pSrc->push_mpls_ethType;

    pDest->wa_data.push_vlan = pSrc->push_vlan;
    pDest->wa_data.push_vlan_data.etherType_idx = pSrc->etherType_idx;

    pDest->wa_data.cp_ttl_outward = pSrc->cp_ttl_outward;

    pDest->wa_data.dec_mpls_ttl= pSrc->dec_mpls_ttl;

    pDest->wa_data.dec_ip_ttl= pSrc->dec_ip_ttl;

    pDest->wa_data.set_field_0_data.field_data = pSrc->field0_data;
    switch (pSrc->field0_type)
    {
        case MANGO_OF_SF_TYPE0_NONE:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_NONE;
            break;
        case MANGO_OF_SF_TYPE0_SA:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_SA;
            /* retrieve smac from egress L3 interface table */
            if ((ret = _dal_mango_l3_intfEntry_get(unit, pSrc->field0_data, &l3_intf_entry, \
                            DAL_MANGO_L3_API_FLAG_NONE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            pDest->wa_data.set_field_0_data.mac = l3_intf_entry.egrIntf.smac_addr;
            break;
        case MANGO_OF_SF_TYPE0_VLAN_PRI:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_VLAN_PRI;
            break;
        case MANGO_OF_SF_TYPE0_MPLS_TC:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_MPLS_TC;
            break;
        case MANGO_OF_SF_TYPE0_MPLS_TTL:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_MPLS_TTL;
            break;
        case MANGO_OF_SF_TYPE0_IP_DSCP:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_DSCP;
            break;
        case MANGO_OF_SF_TYPE0_IP_TTL:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_TTL;
            break;
        case MANGO_OF_SF_TYPE0_IP_FLAG_RSVD:
            pDest->wa_data.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_FLAG_RSVD;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.set_field_1_data.field_data = pSrc->field1_data;
    switch (pSrc->field1_type)
    {
        case MANGO_OF_SF_TYPE1_NONE:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_NONE;
            break;
        case MANGO_OF_SF_TYPE1_DA:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_DA;
            /* retrieve dmac from OpenFlow DMAC table */
            if ((ret = _dal_mango_of_dmacEntry_get(unit, pSrc->field1_data, &dmac_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            osal_memcpy(&pDest->wa_data.set_field_1_data.mac, &dmac_entry.mac, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_SF_TYPE1_VLAN_PRI:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_VLAN_PRI;
            break;
        case MANGO_OF_SF_TYPE1_MPLS_LABEL:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_LABEL;
            /* retrieve MPLS label from MPLS Encap. table */
            if ((ret = dal_mango_mpls_encap_get(unit, pSrc->field1_data, &mpls_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            pDest->wa_data.set_field_1_data.field_data = mpls_entry.label;
            break;
        case MANGO_OF_SF_TYPE1_MPLS_TC:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_TC;
            break;
        case MANGO_OF_SF_TYPE1_MPLS_TTL:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_TTL;
            break;
        case MANGO_OF_SF_TYPE1_IP_DSCP:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_IP_DSCP;
            break;
        case MANGO_OF_SF_TYPE1_IP_TTL:
            pDest->wa_data.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_IP_TTL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.set_field_2_data.field_data = pSrc->field2_data;
    switch (pSrc->field2_type)
    {
        case MANGO_OF_SF_TYPE2_NONE:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_NONE;
            break;
        case MANGO_OF_SF_TYPE2_VLAN_ID:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_VLAN_ID;
            break;
        case MANGO_OF_SF_TYPE2_VLAN_PRI:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_VLAN_PRI;
            break;
        case MANGO_OF_SF_TYPE2_MPLS_LABEL:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_LABEL;
            /* retrieve MPLS label from MPLS Encap. table */
            if ((ret = dal_mango_mpls_encap_get(unit, pSrc->field2_data, &mpls_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            pDest->wa_data.set_field_2_data.field_data = mpls_entry.label;
            break;
        case MANGO_OF_SF_TYPE2_MPLS_TC:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_TC;
            break;
        case MANGO_OF_SF_TYPE2_MPLS_TTL:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_TTL;
            break;
        case MANGO_OF_SF_TYPE2_IP_DSCP:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_IP_DSCP;
            break;
        case MANGO_OF_SF_TYPE2_IP_TTL:
            pDest->wa_data.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_IP_TTL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.set_field_3_data.field_data = pSrc->field3_data;
    switch (pSrc->field3_type)
    {
        case MANGO_OF_IGR_SF_TYPE3_NONE:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_NONE;
            break;
        case MANGO_OF_IGR_SF_TYPE3_SIP:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_SIP;
            pDest->wa_data.set_field_3_data.ip = pSrc->field3_data;
            break;
        case MANGO_OF_IGR_SF_TYPE3_DIP:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_DIP;
            pDest->wa_data.set_field_3_data.ip = pSrc->field3_data;
            break;
        case MANGO_OF_IGR_SF_TYPE3_VLAN_PRI:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_VLAN_PRI;
            break;
        case MANGO_OF_IGR_SF_TYPE3_MPLS_LABEL:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_MPLS_LABEL;
            /* retrieve MPLS label from MPLS Encap. table */
            if ((ret = dal_mango_mpls_encap_get(unit, pSrc->field3_data, &mpls_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            pDest->wa_data.set_field_3_data.field_data = mpls_entry.label;
            break;
        case MANGO_OF_IGR_SF_TYPE3_MPLS_TC:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_MPLS_TC;
            break;
        case MANGO_OF_IGR_SF_TYPE3_MPLS_TTL:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_MPLS_TTL;
            break;
        case MANGO_OF_IGR_SF_TYPE3_IP_DSCP:
            pDest->wa_data.set_field_3_data.field_type = OF_IGR_FT0_SF_TYPE3_IP_DSCP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.set_field_4_data.field_data = pSrc->field4_data;
    switch (pSrc->field4_type)
    {
        case MANGO_OF_IGR_SF_TYPE4_NONE:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_NONE;
            break;
        case MANGO_OF_IGR_SF_TYPE4_SPORT:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_L4_SPORT;
            break;
        case MANGO_OF_IGR_SF_TYPE4_DPORT:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_L4_DPORT;
            break;
        case MANGO_OF_IGR_SF_TYPE4_VLAN_PRI:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_VLAN_PRI;
            break;
        case MANGO_OF_IGR_SF_TYPE4_MPLS_LABEL:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_MPLS_LABEL;
            /* retrieve MPLS label from MPLS Encap. table */
            if ((ret = dal_mango_mpls_encap_get(unit, pSrc->field4_data, &mpls_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            pDest->wa_data.set_field_4_data.field_data = mpls_entry.label;
            break;
        case MANGO_OF_IGR_SF_TYPE4_MPLS_TC:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_MPLS_TC;
            break;
        case MANGO_OF_IGR_SF_TYPE4_MPLS_TTL:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_MPLS_TTL;
            break;
        case MANGO_OF_IGR_SF_TYPE4_IP_DSCP:
            pDest->wa_data.set_field_4_data.field_type = OF_IGR_FT0_SF_TYPE4_IP_DSCP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.set_queue = pSrc->set_queue;
    pDest->wa_data.qid = pSrc->qid;

    pDest->wa_data.group = pSrc->group;
    pDest->wa_data.gid = pSrc->gid;

    switch (pSrc->output_type)
    {
        case MANGO_OF_OUTPUT_TYPE_DISABLE:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_NONE;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
                pDest->wa_data.output_data.devID = (pSrc->output_data >> 6) & 0xF;
                pDest->wa_data.output_data.port = pSrc->output_data & 0x3F;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
                pDest->wa_data.output_data.devID = (pSrc->output_data >> 6) & 0xF;
                pDest->wa_data.output_data.port = pSrc->output_data & 0x3F;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
                pDest->wa_data.output_data.trunk = pSrc->output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
                pDest->wa_data.output_data.trunk = pSrc->output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
                /* convert portmask index to portmask */
                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->output_data, \
                                &pDest->wa_data.output_data.portmask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            break;
        case MANGO_OF_OUTPUT_TYPE_IN_PORT:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
            break;
        case MANGO_OF_OUTPUT_TYPE_FLOOD:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
            break;
        case MANGO_OF_OUTPUT_TYPE_LB:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
            break;
        case MANGO_OF_OUTPUT_TYPE_TUNNEL:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
                /* convert tunnel index to interface */
                _dal_mango_tunnel_tunnelIdx2intf(unit, &pDest->wa_data.output_data.intf, pSrc->output_data);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "tunnel_idx=%d,intf=0x%x", pSrc->output_data, pDest->wa_data.output_data.intf);
            break;
        case MANGO_OF_OUTPUT_TYPE_FAILOVER:
                pDest->wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
                /* convert portmask index to portmask */
                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->output_data, \
                                &pDest->wa_data.output_data.portmask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wm_data.data = pSrc->metadata;
    pDest->wm_data.mask = pSrc->metadata_mask;

    switch (pSrc->tbl_act)
    {
        case MANGO_OF_GOTOTBL_NORMAL:
            pDest->gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
            break;
        case MANGO_OF_GOTOTBL_APPLY_AND_LB:
            pDest->gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
            break;
        case MANGO_OF_GOTOTBL_LB:
            pDest->gt_data.type = OF_GOTOTBL_TYPE_LB;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->gt_data.tbl_id = pSrc->tbl_id + (pSrc->tbl_lb_time * HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit));

    return RT_ERR_OK;
}

static int32
_dal_mango_of_egrFTInstruct_get(uint32 unit, rtk_of_flow_id_t entry_idx, dal_mango_of_egrFtIns_t *pIns)
{
    int32  ret;
    uint32 tblName;
    of_igr_entry_t insEntry;


    osal_memset((void *)&insEntry, 0x00, sizeof(of_igr_entry_t));
    tblName = MANGO_FT_EGRt;

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, tblName, entry_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_INS_WRITE_ACTtf, &pIns->writeAct_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_INS_METERtf, &pIns->meter_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_METER_IDXtf, &pIns->meterIdx,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_METER_YELLOW_ACTtf, &pIns->yellow_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_METER_RED_ACTtf, &pIns->red_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_POP_VLANtf, &pIns->pop_vlan,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_PUSH_VLANtf,\
                                &pIns->push_vlan, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_VLAN_ETHTYPEtf,\
                                &pIns->etherType_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD0_TYPEtf, &pIns->field0_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD0_DATAtf, \
                                &pIns->field0_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD1_TYPEtf, &pIns->field1_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD1_DATAtf, \
                                &pIns->field1_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_EGR_WA_OUTPUT_TYPEtf, &pIns->drop,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_mango_of_egrFTInstruct_get */

static int32
_dal_mango_of_egrFTInstruct_set(uint32 unit, rtk_of_flow_id_t entry_idx, dal_mango_of_egrFtIns_t *pIns)
{
    int32  ret;
    uint32 tblName;
    of_igr_entry_t insEntry;


    osal_memset((void *)&insEntry, 0x00, sizeof(of_igr_entry_t));
    tblName = MANGO_FT_EGRt;

    /* read entry for preserving the portion other than instruction */
    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, tblName, entry_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_INS_WRITE_ACTtf, &pIns->writeAct_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_INS_METERtf, &pIns->meter_en,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_METER_IDXtf, &pIns->meterIdx,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_METER_YELLOW_ACTtf, &pIns->yellow_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_METER_RED_ACTtf, &pIns->red_act,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_POP_VLANtf, &pIns->pop_vlan,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_PUSH_VLANtf,\
                                &pIns->push_vlan, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_VLAN_ETHTYPEtf,\
                                &pIns->etherType_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD0_TYPEtf, &pIns->field0_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD0_DATAtf, \
                                &pIns->field0_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD1_TYPEtf, &pIns->field1_type,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_SET_FIELD1_DATAtf, \
                                &pIns->field1_data, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_EGR_WA_OUTPUT_TYPEtf, &pIns->drop,\
                                (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_WRITE(unit, tblName, entry_idx, (uint32 *)&insEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_mango_of_egrFTInstruct_set */

/* convert rtk_of_egrFTIns_t to dal_mango_of_egrFtIns_t */
static int32
_dal_mango_of_egrFTRtk2Dal_convert(uint32 unit, dal_mango_of_egrFtIns_t *pDest, rtk_of_egrFTIns_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(dal_mango_of_egrFtIns_t));

    pDest->meter_en = pSrc->meter_en;
    pDest->writeAct_en = pSrc->writeAct_en;

    pDest->meterIdx = pSrc->meter_data.meter_id;
    switch (pSrc->meter_data.yellow)
    {
        case OF_METER_TYPE_DROP:
            pDest->yellow_act = MANGO_OF_METER_ACT_DROP;
            break;
        case OF_METER_TYPE_DSCP_REMARK:
            pDest->yellow_act = MANGO_OF_METER_ACT_DSCP_REMARK;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (pSrc->meter_data.red)
    {
        case OF_METER_TYPE_DROP:
            pDest->red_act = MANGO_OF_METER_ACT_DROP;
            break;
        case OF_METER_TYPE_DSCP_REMARK:
            pDest->red_act = MANGO_OF_METER_ACT_DSCP_REMARK;
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->pop_vlan = pSrc->wa_data.pop_vlan;
    pDest->push_vlan = pSrc->wa_data.push_vlan;
    pDest->etherType_idx = pSrc->wa_data.push_vlan_data.etherType_idx;

    switch (pSrc->wa_data.set_field_0_data.field_type)
    {
        case OF_EGR_FT_SF_TYPE0_NONE:
            pDest->field0_type = MANGO_OF_EGR_SFTYPE0_NONE;
            break;
        case OF_EGR_FT_SF_TYPE0_VLAN_PRI:
            pDest->field0_type = MANGO_OF_EGR_SFTYPE0_VLAN_PRI;
            break;
        case OF_EGR_FT_SF_TYPE0_IP_DSCP:
            pDest->field0_type = MANGO_OF_EGR_SFTYPE0_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field0_data = pSrc->wa_data.set_field_0_data.field_data;

    switch (pSrc->wa_data.set_field_1_data.field_type)
    {
        case OF_EGR_FT_SF_TYPE1_NONE:
            pDest->field1_type = MANGO_OF_EGR_SFTYPE1_NONE;
            break;
        case OF_EGR_FT_SF_TYPE1_VLAN_ID:
            pDest->field1_type = MANGO_OF_EGR_SFTYPE1_VLAN_ID;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field1_data = pSrc->wa_data.set_field_1_data.field_data;

    pDest->drop = pSrc->wa_data.drop;

    return RT_ERR_OK;
}

/* convert dal_mango_of_egrFtIns_t to rtk_of_egrFTIns_t */
static int32
_dal_mango_of_egrFTDal2Rtk_convert(uint32 unit, rtk_of_egrFTIns_t *pDest, dal_mango_of_egrFtIns_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(rtk_of_egrFTIns_t));

    pDest->meter_en = pSrc->meter_en;
    pDest->writeAct_en = pSrc->writeAct_en;

    pDest->meter_data.meter_id = pSrc->meterIdx;
    switch (pSrc->yellow_act)
    {
        case MANGO_OF_METER_ACT_DROP:
            pDest->meter_data.yellow = OF_METER_TYPE_DROP;
            break;
        case MANGO_OF_METER_ACT_DSCP_REMARK:
            pDest->meter_data.yellow = OF_METER_TYPE_DSCP_REMARK;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (pSrc->red_act)
    {
        case MANGO_OF_METER_ACT_DROP:
            pDest->meter_data.red = OF_METER_TYPE_DROP;
            break;
        case MANGO_OF_METER_ACT_DSCP_REMARK:
            pDest->meter_data.red = OF_METER_TYPE_DSCP_REMARK;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->wa_data.pop_vlan = pSrc->pop_vlan;
    pDest->wa_data.push_vlan = pSrc->push_vlan;
    pDest->wa_data.push_vlan_data.etherType_idx = pSrc->etherType_idx;

    switch (pSrc->field0_type)
    {
        case MANGO_OF_EGR_SFTYPE0_NONE:
            pDest->wa_data.set_field_0_data.field_type = OF_EGR_FT_SF_TYPE0_NONE;
            break;
        case MANGO_OF_EGR_SFTYPE0_VLAN_PRI:
            pDest->wa_data.set_field_0_data.field_type = OF_EGR_FT_SF_TYPE0_VLAN_PRI;
            break;
        case MANGO_OF_EGR_SFTYPE0_IP_DSCP:
            pDest->wa_data.set_field_0_data.field_type = OF_EGR_FT_SF_TYPE0_IP_DSCP;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->wa_data.set_field_0_data.field_data = pSrc->field0_data;

    switch (pSrc->field1_type)
    {
        case MANGO_OF_EGR_SFTYPE1_NONE:
            pDest->wa_data.set_field_1_data.field_type = OF_EGR_FT_SF_TYPE1_NONE;
            break;
        case MANGO_OF_EGR_SFTYPE1_VLAN_ID:
            pDest->wa_data.set_field_1_data.field_type = OF_EGR_FT_SF_TYPE1_VLAN_ID;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->wa_data.set_field_1_data.field_data = pSrc->field1_data;

    pDest->wa_data.drop = pSrc->drop;

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l2_bucket_idx_get(uint32 unit, dal_mango_of_l2Entry_t *pEntry, dal_mango_of_bucketIdx_t *idx)
{
    uint32  hashSeed_23_0  = 0;
    uint32  hashSeed_47_24 = 0;
    uint32  hashSeed_71_48 = 0;
    uint32  hashSeed_95_72 = 0;
    uint32  hashSeed_107_96 = 0;
    uint32  tmp_23_0  = 0;
    uint32  tmp_47_24 = 0;
    uint32  tmp_71_48 = 0;
    uint32  algo0_idx = 0, algo1_idx = 0;
    uint32  block0_algo, block1_algo;

    switch (pEntry->type)
    {
        case MANGO_OF_L2ENTRY_TYPE_SA:
        case MANGO_OF_L2ENTRY_TYPE_DA:
            hashSeed_23_0  = ((pEntry->matchfd.vidMac.mac0.octet[3] << 16) |
                              (pEntry->matchfd.vidMac.mac0.octet[4] << 8)  |
                              (pEntry->matchfd.vidMac.mac0.octet[5]));
            hashSeed_47_24 = ((pEntry->matchfd.vidMac.mac0.octet[0] << 16) |
                              (pEntry->matchfd.vidMac.mac0.octet[1] << 8) |
                              (pEntry->matchfd.vidMac.mac0.octet[2]));
            hashSeed_71_48 = pEntry->matchfd.vidMac.vid;
            break;
        case MANGO_OF_L2ENTRY_TYPE_SA_DA:
            hashSeed_23_0  = ((pEntry->matchfd.vidMac.mac0.octet[3] << 16) |
                              (pEntry->matchfd.vidMac.mac0.octet[4] << 8)  |
                              (pEntry->matchfd.vidMac.mac0.octet[5]));
            hashSeed_47_24 = ((pEntry->matchfd.vidMac.mac0.octet[0] << 16) |
                              (pEntry->matchfd.vidMac.mac0.octet[1] << 8) |
                              (pEntry->matchfd.vidMac.mac0.octet[2]));
            hashSeed_71_48 = ((pEntry->matchfd.vidMac.mac1.octet[3] << 16) |
                              (pEntry->matchfd.vidMac.mac1.octet[4] << 8)  |
                              (pEntry->matchfd.vidMac.mac1.octet[5]));
            hashSeed_95_72 = ((pEntry->matchfd.vidMac.mac1.octet[0] << 16) |
                              (pEntry->matchfd.vidMac.mac1.octet[1] << 8) |
                              (pEntry->matchfd.vidMac.mac1.octet[2]));

            break;
        case MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE:
            hashSeed_23_0  = (pEntry->matchfd.fiveTp.sip & 0xFFFFFF);
            hashSeed_47_24 = (((pEntry->matchfd.fiveTp.sip & 0xFF000000) >> 24) |
                              ((pEntry->matchfd.fiveTp.dip & 0xFFFF) << 8));
            hashSeed_71_48 = (((pEntry->matchfd.fiveTp.dip & 0xFFFF0000) >> 16) |
                              ((pEntry->matchfd.fiveTp.l4_sport & 0xFF) << 16));
            hashSeed_95_72 = (((pEntry->matchfd.fiveTp.l4_sport & 0xFF00) >> 8) |
                              (pEntry->matchfd.fiveTp.l4_dport << 8));
            if (pEntry->matchfd.fiveTp.type == 0)
                hashSeed_107_96 = (pEntry->matchfd.fiveTp.fifthData.ipproto & 0xFF);
            else
                hashSeed_107_96 = (pEntry->matchfd.fiveTp.fifthData.vid & 0xFFF);
            break;
        default:
            return RT_ERR_INPUT;
    }

    dal_mango_of_l2FlowTblHashAlgo_get(unit, 0, &block0_algo);
    dal_mango_of_l2FlowTblHashAlgo_get(unit, 1, &block1_algo);

    if (block0_algo == 0 || block1_algo == 0)
    {
        /* Algo 0 */
        tmp_71_48 = (hashSeed_71_48 & 0xFFF000) |( (hashSeed_71_48 >> 3) & 0x1FF) | ((hashSeed_71_48 << 9) & 0xE00) ;

        algo0_idx = ((hashSeed_107_96)                ^
                     ((hashSeed_95_72 >> 12) & 0xFFF)  ^
                     (hashSeed_95_72 & 0xFFF)         ^
                     ((tmp_71_48 >> 12) & 0xFFF)      ^
                     (tmp_71_48 & 0xFFF)              ^
                     ((hashSeed_47_24 >> 12) & 0xFFF) ^
                     (hashSeed_47_24 & 0xFFF)         ^
                     ((hashSeed_23_0 >> 12) & 0xFFF)  ^
                     (hashSeed_23_0 & 0xFFF));
    }

    if (block0_algo == 1 || block1_algo == 1)
    {
        /* Algo 1 */
        tmp_47_24 = ((hashSeed_47_24 >> 6)  & 0x3F000) | ((hashSeed_47_24 << 6)  & 0xFC0000) |  (hashSeed_47_24  & 0xFFF);
        tmp_23_0  =  ((hashSeed_23_0 >> 6)  & 0x3F000) | ((hashSeed_23_0 << 6)  & 0xFC0000)  | ((hashSeed_23_0 >> 9) & 0x7) | ((hashSeed_23_0 << 3) & 0xFF8);

        algo1_idx = (((hashSeed_107_96)               ^
                     ((hashSeed_95_72 >> 12) & 0xFFF)  ^
                     (hashSeed_95_72 & 0xFFF)         ^
                     ((hashSeed_71_48 >> 12) & 0xFFF) ^
                     (hashSeed_71_48 & 0xFFF)         ^
                     ((tmp_47_24 >> 12) & 0xFFF)      ^
                     (tmp_47_24 & 0xFFF)              ^
                     ((tmp_23_0 >> 12) & 0xFFF)       ^
                     (tmp_23_0 & 0xFFF)));
    }

    if (block0_algo == 0)
        idx->bucket0_hash_idx = algo0_idx;
    else
        idx->bucket0_hash_idx = algo1_idx;

    if (block1_algo == 0)
        idx->bucket1_hash_idx = algo0_idx;
    else
        idx->bucket1_hash_idx = algo1_idx;

    /* convert bucket hash index to table access index */
    idx->bucket0_idx = idx->bucket0_hash_idx*HAL_L2_HASHWIDTH(unit);
    idx->bucket1_idx = (l2Sram_size[unit]/2) + (idx->bucket1_hash_idx * HAL_L2_HASHWIDTH(unit));

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "bucket0_hash_idx=%d, bucket1_hash_idx=%d,bucket0_idx=%d, bucket1_idx=%d",\
        idx->bucket0_hash_idx, idx->bucket1_hash_idx,idx->bucket0_idx, idx->bucket1_idx);

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3_hash0_ret(uint32 unit, dal_mango_of_l3HashEntry_t *pEntry)
{
    uint32  hashIdx = 0;
    uint32  hashRow[26] = {0};
    rtk_ip_addr_t   sip = 0, dip = 0;
    rtk_ipv6_addr_t sip6 , dip6;

    osal_memset(&sip6, 0, sizeof(rtk_ipv6_addr_t));
    osal_memset(&dip6, 0, sizeof(rtk_ipv6_addr_t));

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP)
            sip = pEntry->ip0.ipv4;

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
            dip = pEntry->ip0.ipv4;

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        {
            sip = pEntry->ip0.ipv4;
            dip = pEntry->ip1.ipv4;
        }
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "sip=0x%X, dip=0x%X", sip, dip);
    }

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP)
            osal_memcpy(&sip6, &pEntry->ip0.ipv6, sizeof(rtk_ipv6_addr_t));

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
            osal_memcpy(&dip6, &pEntry->ip0.ipv6, sizeof(rtk_ipv6_addr_t));

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        {
            osal_memcpy(&sip6, &pEntry->ip0.ipv6, sizeof(rtk_ipv6_addr_t));
            osal_memcpy(&dip6, &pEntry->ip1.ipv6, sizeof(rtk_ipv6_addr_t));
        }
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "sip6=0x%X, dip6=0x%X", sip6, dip6);
    }

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        hashRow[0] = (EXTRACT_BITS_BY_LEN(sip6.octet[0], 0, 7) << 3) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[1], 5, 3) << 0);
        hashRow[1] = (EXTRACT_BITS_BY_LEN(sip6.octet[1], 0, 5) << 5) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[2], 3, 5) << 0);
        hashRow[2] = (EXTRACT_BITS_BY_LEN(sip6.octet[2], 0, 3) << 7) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[3], 1, 7) << 0);
        hashRow[3] = (EXTRACT_BITS_BY_LEN(sip6.octet[3], 0, 1) << 9) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[4], 0, 8) << 1) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[5], 7, 1) << 0);
        hashRow[4] = (EXTRACT_BITS_BY_LEN(sip6.octet[5], 0, 7) << 3) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[6], 5, 3) << 0);
        hashRow[5] = (EXTRACT_BITS_BY_LEN(sip6.octet[6], 0, 5) << 5) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[7], 3, 5) << 0);
        hashRow[6] = (EXTRACT_BITS_BY_LEN(sip6.octet[7], 0, 3) << 7) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[8], 1, 7) << 0);
        hashRow[7] = (EXTRACT_BITS_BY_LEN(sip6.octet[8], 0, 1) << 9) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[9], 0, 8) << 1) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[10], 7, 1) << 0);
        hashRow[8] = (EXTRACT_BITS_BY_LEN(sip6.octet[10], 0, 7) << 3) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[11], 5, 3) << 0);
        hashRow[9] = (EXTRACT_BITS_BY_LEN(sip6.octet[11], 0, 5) << 5) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[12], 3, 5) << 0);
        hashRow[10] = (EXTRACT_BITS_BY_LEN(sip6.octet[12], 0, 3) << 7) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[13], 1, 7) << 0);
        hashRow[11] = (EXTRACT_BITS_BY_LEN(sip6.octet[13], 0, 1) << 9) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[14], 0, 8) << 1) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[15], 7, 1) << 0);
        hashRow[12] = (EXTRACT_BITS_BY_LEN(sip6.octet[15], 0, 7) << 3) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[0],  7, 1) << 0);

        hashRow[13] = (EXTRACT_BITS_BY_LEN(dip6.octet[0], 0, 8) << 0);
        hashRow[14] = (EXTRACT_BITS_BY_LEN(dip6.octet[1], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[2], 6, 2) << 0);
        hashRow[15] = (EXTRACT_BITS_BY_LEN(dip6.octet[2], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[3], 4, 4) << 0);
        hashRow[16] = (EXTRACT_BITS_BY_LEN(dip6.octet[3], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[4], 2, 6) << 0);
        hashRow[17] = (EXTRACT_BITS_BY_LEN(dip6.octet[4], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[5], 0, 8) << 0);
        hashRow[18] = (EXTRACT_BITS_BY_LEN(dip6.octet[6], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[7], 6, 2) << 0);
        hashRow[19] = (EXTRACT_BITS_BY_LEN(dip6.octet[7], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[8], 4, 4) << 0);
        hashRow[20] = (EXTRACT_BITS_BY_LEN(dip6.octet[8], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[9], 2, 6) << 0);
        hashRow[21] = (EXTRACT_BITS_BY_LEN(dip6.octet[9], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[10], 0, 8) << 0);
        hashRow[22] = (EXTRACT_BITS_BY_LEN(dip6.octet[11], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[12], 6, 2) << 0);
        hashRow[23] = (EXTRACT_BITS_BY_LEN(dip6.octet[12], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[13], 4, 4) << 0);
        hashRow[24] = (EXTRACT_BITS_BY_LEN(dip6.octet[13], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[14], 2, 6) << 0);
        hashRow[25] = (EXTRACT_BITS_BY_LEN(dip6.octet[14], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[15], 0, 8) << 0);
    }
    else
    {
        hashRow[9]  = (EXTRACT_BITS_BY_LEN(sip, 27,  5) << 0);
        hashRow[10] = (EXTRACT_BITS_BY_LEN(sip, 17, 10) << 0);
        hashRow[11] = (EXTRACT_BITS_BY_LEN(sip,  7, 10) << 0);
        hashRow[12] = (EXTRACT_BITS_BY_LEN(sip, 0,  7) << 3);

        hashRow[22] = (EXTRACT_BITS_BY_LEN(dip, 30,  2) << 0);
        hashRow[23] = (EXTRACT_BITS_BY_LEN(dip, 20, 10) << 0);
        hashRow[24] = (EXTRACT_BITS_BY_LEN(dip, 10, 10) << 0);
        hashRow[25] = (EXTRACT_BITS_BY_LEN(dip,  0, 10) << 0);
    }

    hashIdx = \
                hashRow[0]  ^ hashRow[1]  ^ hashRow[2]  ^ hashRow[3]  ^ hashRow[4]  ^ \
                hashRow[5]  ^ hashRow[6]  ^ hashRow[7]  ^ hashRow[8]  ^ hashRow[9]  ^ \
                hashRow[10] ^ hashRow[11] ^ hashRow[12] ^ hashRow[13] ^ hashRow[14] ^ \
                hashRow[15] ^ hashRow[16] ^ hashRow[17] ^ hashRow[18] ^ hashRow[19] ^ \
                hashRow[20] ^ hashRow[21] ^ hashRow[22] ^ hashRow[23] ^ hashRow[24] ^ \
                hashRow[25];

    return hashIdx;
}

static int32
_dal_mango_of_l3_hash1_ret(uint32 unit, dal_mango_of_l3HashEntry_t *pEntry)
{
    uint32  hashIdx = 0;
    uint32  hashRow[28] = {0};
    uint32  sum;
    rtk_ip_addr_t   sip = 0, dip = 0;
    rtk_ipv6_addr_t sip6 , dip6;

    osal_memset(&sip6, 0, sizeof(rtk_ipv6_addr_t));
    osal_memset(&dip6, 0, sizeof(rtk_ipv6_addr_t));

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP)
            sip = pEntry->ip0.ipv4;

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
            dip = pEntry->ip0.ipv4;

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        {
            sip = pEntry->ip0.ipv4;
            dip = pEntry->ip1.ipv4;
        }
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "sip=0x%X, dip=0x%X", sip, dip);
    }

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP)
            osal_memcpy(&sip6, &pEntry->ip0.ipv6, sizeof(rtk_ipv6_addr_t));

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
            osal_memcpy(&dip6, &pEntry->ip0.ipv6, sizeof(rtk_ipv6_addr_t));

        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        {
            osal_memcpy(&sip6, &pEntry->ip0.ipv6, sizeof(rtk_ipv6_addr_t));
            osal_memcpy(&dip6, &pEntry->ip1.ipv6, sizeof(rtk_ipv6_addr_t));
        }
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "sip6=0x%X, dip6=0x%X", sip6, dip6);
    }

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        hashRow[0] = (EXTRACT_BITS_BY_LEN(sip6.octet[12], 6, 2) << 0);
        hashRow[1] = (EXTRACT_BITS_BY_LEN(sip6.octet[12], 0, 6) << 4) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[13], 4, 4) << 0);
        hashRow[2] = (EXTRACT_BITS_BY_LEN(sip6.octet[13], 0, 4) << 6) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[14], 2, 6) << 0);
        hashRow[3] = (EXTRACT_BITS_BY_LEN(sip6.octet[14], 0, 2) << 8) | \
                     (EXTRACT_BITS_BY_LEN(sip6.octet[15], 0, 8) << 0);

        hashRow[4] = (EXTRACT_BITS_BY_LEN(dip6.octet[12], 6, 2) << 0);
        hashRow[5] = (EXTRACT_BITS_BY_LEN(dip6.octet[12], 0, 6) << 4) | \
                     (EXTRACT_BITS_BY_LEN(dip6.octet[13], 4, 4) << 0);
        hashRow[6] = (EXTRACT_BITS_BY_LEN(dip6.octet[13], 0, 4) << 6) | \
                     (EXTRACT_BITS_BY_LEN(dip6.octet[14], 2, 6) << 0);
        hashRow[7] = (EXTRACT_BITS_BY_LEN(dip6.octet[14], 0, 2) << 8) | \
                     (EXTRACT_BITS_BY_LEN(dip6.octet[15], 0, 8) << 0);

        hashRow[8] = (EXTRACT_BITS_BY_LEN(sip6.octet[0], 2, 6) << 0);
        hashRow[9] = (EXTRACT_BITS_BY_LEN(sip6.octet[0], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[1], 0, 8) << 0);
        hashRow[10] = (EXTRACT_BITS_BY_LEN(sip6.octet[2], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[3], 6, 2) << 0);
        hashRow[11] = (EXTRACT_BITS_BY_LEN(sip6.octet[3], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[4], 4, 4) << 0);
        hashRow[12] = (EXTRACT_BITS_BY_LEN(sip6.octet[4], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[5], 2, 6) << 0);
        hashRow[13] = (EXTRACT_BITS_BY_LEN(sip6.octet[5], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[6], 0, 8) << 0);
        hashRow[14] = (EXTRACT_BITS_BY_LEN(sip6.octet[7], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[8], 6, 2) << 0);
        hashRow[15] = (EXTRACT_BITS_BY_LEN(sip6.octet[8], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[9], 4, 4) << 0);
        hashRow[16] = (EXTRACT_BITS_BY_LEN(sip6.octet[9], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[10], 2, 6) << 0);
        hashRow[17] = (EXTRACT_BITS_BY_LEN(sip6.octet[10], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(sip6.octet[11], 0, 8) << 0);

        hashRow[18] = (EXTRACT_BITS_BY_LEN(dip6.octet[0], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[1], 6, 2) << 0);
        hashRow[19] = (EXTRACT_BITS_BY_LEN(dip6.octet[1], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[2], 4, 4) << 0);
        hashRow[20] = (EXTRACT_BITS_BY_LEN(dip6.octet[2], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[3], 2, 6) << 0);
        hashRow[21] = (EXTRACT_BITS_BY_LEN(dip6.octet[3], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[4], 0, 8) << 0);
        hashRow[22] = (EXTRACT_BITS_BY_LEN(dip6.octet[5], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[6], 6, 2) << 0);
        hashRow[23] = (EXTRACT_BITS_BY_LEN(dip6.octet[6], 0, 6) << 4) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[7], 4, 4) << 0);
        hashRow[24] = (EXTRACT_BITS_BY_LEN(dip6.octet[7], 0, 4) << 6) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[8], 2, 6) << 0);
        hashRow[25] = (EXTRACT_BITS_BY_LEN(dip6.octet[8], 0, 2) << 8) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[9], 0, 8) << 0);
        hashRow[26] = (EXTRACT_BITS_BY_LEN(dip6.octet[10], 0, 8) << 2) | \
                      (EXTRACT_BITS_BY_LEN(dip6.octet[11], 6, 2) << 0);
        hashRow[27] = (EXTRACT_BITS_BY_LEN(dip6.octet[11], 0, 6) << 4);
    }
    else
    {
        hashRow[0] = (EXTRACT_BITS_BY_LEN(sip, 30,  2) << 0);
        hashRow[1] = (EXTRACT_BITS_BY_LEN(sip, 20, 10) << 0);
        hashRow[2] = (EXTRACT_BITS_BY_LEN(sip, 10, 10) << 0);
        hashRow[3] = (EXTRACT_BITS_BY_LEN(sip,  0, 10) << 0);

        hashRow[4] = (EXTRACT_BITS_BY_LEN(dip, 30,  2) << 0);
        hashRow[5] = (EXTRACT_BITS_BY_LEN(dip, 20, 10) << 0);
        hashRow[6] = (EXTRACT_BITS_BY_LEN(dip, 10, 10) << 0);
        hashRow[7] = (EXTRACT_BITS_BY_LEN(dip,  0, 10) << 0);
    }

    sum = hashRow[0] + hashRow[1];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[2];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[3];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[4];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[5];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[6];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[7];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);

    hashIdx = sum ^ hashRow[8]  ^ hashRow[9]  ^ \
              hashRow[10] ^ hashRow[11] ^ hashRow[12] ^ hashRow[13] ^ hashRow[14] ^ \
              hashRow[15] ^ hashRow[16] ^ hashRow[17] ^ hashRow[18] ^ hashRow[19] ^ \
              hashRow[20] ^ hashRow[21] ^ hashRow[22] ^ hashRow[23] ^ hashRow[24] ^ \
              hashRow[25] ^ hashRow[26] ^ hashRow[27];

    return hashIdx;
}

static int32
_dal_mango_of_l3_bucket_idx_get(uint32 unit, dal_mango_of_l3HashEntry_t *pEntry, dal_mango_of_bucketIdx_t *idx)
{
    uint32  algo0_idx = 0, algo1_idx = 0;
    uint32  block0_algo, block1_algo;

    dal_mango_of_l3HashFlowTblHashAlgo_get(unit, 0, &block0_algo);
    dal_mango_of_l3HashFlowTblHashAlgo_get(unit, 1, &block1_algo);

    if (block0_algo == 0 || block1_algo == 0)
    {
        algo0_idx = _dal_mango_of_l3_hash0_ret(unit, pEntry);
    }

    if (block0_algo == 1 || block1_algo == 1)
    {
        algo1_idx = _dal_mango_of_l3_hash1_ret(unit, pEntry);
    }

    if (block0_algo == 0)
        idx->bucket0_hash_idx = algo0_idx;
    else
        idx->bucket0_hash_idx = algo1_idx;

    if (block1_algo == 0)
        idx->bucket1_hash_idx = algo0_idx;
    else
        idx->bucket1_hash_idx = algo1_idx;

    /* convert bucket index to table access index */
    idx->bucket0_idx = idx->bucket0_hash_idx*DAL_MANGO_L3_HOST_TBL_WIDTH;
    idx->bucket1_idx = (l3Sram_size[unit]/2) + (idx->bucket1_hash_idx * DAL_MANGO_L3_HOST_TBL_WIDTH);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "bucket0_hash_idx=%d, bucket1_hash_idx=%d,bucket0_idx=%d, bucket1_idx=%d",\
        idx->bucket0_hash_idx, idx->bucket1_hash_idx,idx->bucket0_idx, idx->bucket1_idx);

    return RT_ERR_OK;
}

#if 0// replaced by _dal_mango_l3_hostEntry_alloc
static int32
_dal_mango_of_search_vacant_entries(uint32 unit, uint32 index, dal_mango_of_l3HashEntry_t *pEntry)
{
    int32  ret;
    uint32 i, used_entry_num = 0;
    dal_mango_of_l3HashEntry_t tmp_entry;

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        used_entry_num = 2;
    else if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6 &&
            (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP))
        used_entry_num = 3;
    else if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6 && pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        used_entry_num = 6;

    for (i=0; i < used_entry_num; i++)
    {
        if ((ret = _dal_mango_of_l3HashFlowEntry_get(unit, index+i, &tmp_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (tmp_entry.valid)
            return FALSE;
    }

    return TRUE;
}
#endif

/* allocate a L3 interface entry to store the SA */
static int32
_dal_mango_of_l3IntfEgr_set(uint32 unit, rtk_mac_t mac, uint32 *pIndex)
{
    int32 ret;
    dal_mango_l3_intfEgrEntry_t intf_entry;

    if ((ret = _dal_mango_l3_intfEntry_alloc(unit, pIndex, RTK_L3_FLAG_NONE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    osal_memset(&intf_entry, 0x00, sizeof(dal_mango_l3_intfEgrEntry_t));
    intf_entry.smac_addr = mac;
    if ((ret = _dal_mango_l3_intfEgrEntry_set(unit, *pIndex, &intf_entry, RTK_L3_FLAG_NONE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
_dal_mango_of_dmacEntry_dump(uint32 unit)
{
    uint32 entryIdx, numOfEntry = 0;
    dal_mango_of_dmacEntry_t entry;
    rtk_mac_t zeroMac;

    osal_memset(&zeroMac, 0x00, sizeof(rtk_mac_t));

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_OPENFLOW), "===== SHADOW =====");
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_OPENFLOW), "Total DMAC entry in Shadow: %u\n", _ofDb[unit].dmac_tbl.used_count);
    for (entryIdx=0; entryIdx<DAL_MANGO_OF_DMAC_ENTRY_MAX; entryIdx++)
    {
        if (BITMAP_IS_SET(_ofDb[unit].dmac_tbl.used, entryIdx))
        {
            RT_LOG(LOG_INFO, (MOD_DAL|MOD_OPENFLOW), "Index %u is used\n", entryIdx);
        }
    }

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_OPENFLOW), "\n===== HARDWARE =====");
    for (entryIdx=0; entryIdx<DAL_MANGO_OF_DMAC_ENTRY_MAX; entryIdx++)
    {
        _dal_mango_of_dmacEntry_get(unit, entryIdx, &entry);
        if (osal_memcmp(&entry.mac, &zeroMac, sizeof(rtk_mac_t)) != 0)
        {
            RT_LOG(LOG_INFO, (MOD_DAL|MOD_OPENFLOW), "Index %u: DMAC=%02X:%02X:%02X:%02X:%02X:%02X",\
                entryIdx,entry.mac.octet[0],entry.mac.octet[1],entry.mac.octet[2],entry.mac.octet[3],entry.mac.octet[4],entry.mac.octet[5]);
            numOfEntry++;
        }
    }
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_OPENFLOW), "Total DMAC entry in Hardware: %u\n", numOfEntry);

    return RT_ERR_OK;
}

int32
_dal_mango_of_dmacEntry_get(uint32 unit, uint32 idx, dal_mango_of_dmacEntry_t *pEntry)
{
    int32 ret;
    of_dmac_entry_t  entry;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_OF_DMACt, idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_mac_get(unit, MANGO_OF_DMACt, MANGO_OF_DMAC_MACtf, (uint8 *)&pEntry->mac.octet[0],\
                                    (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "index=%u,dmac=%02X:%02X:%02X:%02X:%02X:%02X",\
        idx,pEntry->mac.octet[0],pEntry->mac.octet[1],pEntry->mac.octet[2],pEntry->mac.octet[3],pEntry->mac.octet[4],pEntry->mac.octet[5]);

    return RT_ERR_OK;
}

int32
_dal_mango_of_dmacEntry_set(uint32 unit, uint32 idx, dal_mango_of_dmacEntry_t *pEntry)
{
    int32 ret;
    of_dmac_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "index=%u,dmac=%02X:%02X:%02X:%02X:%02X:%02X",\
        idx,pEntry->mac.octet[0],pEntry->mac.octet[1],pEntry->mac.octet[2],pEntry->mac.octet[3],pEntry->mac.octet[4],pEntry->mac.octet[5]);

    if ((ret = table_field_mac_set(unit, MANGO_OF_DMACt, MANGO_OF_DMAC_MACtf, (uint8 *)&pEntry->mac.octet[0],\
                                    (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, MANGO_OF_DMACt, idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

int32
_dal_mango_of_dmacEntry_alloc(uint32 unit, uint32 *pIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    OF_SEM_LOCK(unit);

    /* pre-check */
    if (_ofDb[unit].dmac_tbl.used_count >= DAL_MANGO_OF_DMAC_ENTRY_MAX)
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

    /* search an empty entry */
    for (entryIdx=0; entryIdx<DAL_MANGO_OF_DMAC_ENTRY_MAX; entryIdx++)
    {

        if (BITMAP_IS_CLEAR(_ofDb[unit].dmac_tbl.used, entryIdx))
        {
            BITMAP_SET(_ofDb[unit].dmac_tbl.used, entryIdx);
            _ofDb[unit].dmac_tbl.used_count += 1;
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "allocated Idx=%u, used_count=%u", entryIdx, _ofDb[unit].dmac_tbl.used_count);
            *pIdx = entryIdx;
            goto errOk;
        }
    }

    ret = RT_ERR_TBL_FULL;

errTblFull:
errOk:
    OF_SEM_UNLOCK(unit);
    return ret;
}

int32
_dal_mango_of_dmacEntry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;
    uint32 isFreed = FALSE;
    dal_mango_of_dmacEntry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,index=%u", unit, idx);

    OF_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_ofDb[unit].dmac_tbl.used, idx))
    {
        BITMAP_CLEAR(_ofDb[unit].dmac_tbl.used, idx);
        _ofDb[unit].dmac_tbl.used_count -= 1;
        isFreed = TRUE;
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "free Idx=%u, used_count=%u", idx, _ofDb[unit].dmac_tbl.used_count);
    }
    else
        ret = RT_ERR_ENTRY_NOTFOUND;

    OF_SEM_UNLOCK(unit);

    if (isFreed)
    {
        osal_memset(&entry, 0x00, sizeof(dal_mango_of_dmacEntry_t));
        if ((ret = _dal_mango_of_dmacEntry_set(unit, idx, &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            OF_SEM_UNLOCK(unit);
            return ret;
        }
    }

    return ret;
}

/* allocate a OpenFlow DMAC entry to store the SA and return the allocated index */
static int32
_dal_mango_of_dmac_set(uint32 unit, rtk_mac_t *pMac, uint32 *pIndex)
{
    int32 ret;
    dal_mango_of_dmacEntry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "dmac=%02X:%02X:%02X:%02X:%02X:%02X",\
        pMac->octet[0],pMac->octet[1],pMac->octet[2],pMac->octet[3],pMac->octet[4],pMac->octet[5]);

    if ((ret = _dal_mango_of_dmacEntry_alloc(unit, pIndex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    osal_memset(&entry, 0x00, sizeof(dal_mango_of_dmacEntry_t));
    osal_memcpy(&entry.mac, pMac, sizeof(rtk_mac_t));

    if ((ret = _dal_mango_of_dmacEntry_set(unit, *pIndex, &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /*_dal_mango_of_dmacEntry_dump(unit);*/

    return RT_ERR_OK;
}

static int32
_dal_mango_of_flowEntryResource_alloc(uint32 unit, dal_mango_of_igrFtIns_t *pIgrIns, rtk_of_igrFT0Ins_t *pIgrInsRtk)
{
    int32  ret;
    uint32 return_idx;
    rtk_mpls_encap_t mpls_encap_entry;

    /* portmask check */
    if (pIgrIns->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pIgrIns->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pIgrInsRtk->wa_data.output_data.portmask), RT_ERR_PORT_MASK);
    }

    osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
    mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;/*to pass para. check*/

    /* allocate a L3 interface entry to store the SA */
    if (pIgrIns->field0_type == MANGO_OF_SF_TYPE0_SA)
    {
        if ((ret = _dal_mango_of_l3IntfEgr_set(unit, pIgrInsRtk->wa_data.set_field_0_data.mac,\
                    &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->field0_data = return_idx;
    }

    /* add a OpenFlow DMAC entry to store the DA */
    if (pIgrIns->field1_type == MANGO_OF_SF_TYPE1_DA)
    {
        if ((ret = _dal_mango_of_dmac_set(unit, &pIgrInsRtk->wa_data.set_field_1_data.mac, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->field1_data = return_idx;
    }

    /* add a MPLS encap. entry to store the MPLS label */
    if (pIgrIns->field1_type == MANGO_OF_SF_TYPE1_MPLS_LABEL)
    {
        osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
        mpls_encap_entry.label = pIgrInsRtk->wa_data.set_field_1_data.field_data;
        mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;
        if ((ret = dal_mango_mpls_encap_create(unit, &mpls_encap_entry, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->field1_data = return_idx;
    }

    /* add a MPLS encap. entry to store the MPLS label */
    if (pIgrIns->field2_type == MANGO_OF_SF_TYPE2_MPLS_LABEL)
    {
        osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
        mpls_encap_entry.label = pIgrInsRtk->wa_data.set_field_2_data.field_data;
        mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;
        if ((ret = dal_mango_mpls_encap_create(unit, &mpls_encap_entry, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->field2_data = return_idx;
    }

    /* add a MPLS encap. entry to store the MPLS label */
    if (pIgrIns->field3_type == MANGO_OF_IGR_SF_TYPE3_MPLS_LABEL)
    {
        osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
        mpls_encap_entry.label = pIgrInsRtk->wa_data.set_field_3_data.field_data;
        mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;
        if ((ret = dal_mango_mpls_encap_create(unit, &mpls_encap_entry, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->field3_data = return_idx;
    }

    /* add a MPLS encap. entry to store the MPLS label */
    if (pIgrIns->field4_type == MANGO_OF_IGR_SF_TYPE4_MPLS_LABEL)
    {
        osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
        mpls_encap_entry.label = pIgrInsRtk->wa_data.set_field_4_data.field_data;
        mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;
        if ((ret = dal_mango_mpls_encap_create(unit, &mpls_encap_entry, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->field4_data = return_idx;
    }

    /* allocate a portmask entry to store the egress ports */
    if (pIgrIns->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pIgrIns->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        if ((ret = _dal_mango_of_l2PortMaskEntry_set(unit, &pIgrInsRtk->wa_data.output_data.portmask, \
                    &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIgrIns->output_data = return_idx;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_flowEntryResource_free(uint32 unit, dal_mango_of_igrFtIns_t *pIgrIns)
{
    int32  ret;

    /* free L3 interface entry */
    if (pIgrIns->field0_type == MANGO_OF_SF_TYPE0_SA)
    {
        if ((ret = _dal_mango_l3_intfEntry_free(unit, pIgrIns->field0_data, RTK_L3_FLAG_NONE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* free OpenFlow DMAC entry */
    if (pIgrIns->field1_type == MANGO_OF_SF_TYPE1_DA)
    {
        if ((ret = _dal_mango_of_dmacEntry_free(unit, pIgrIns->field1_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* delete MPLS encap. entry */
    if (pIgrIns->field1_type == MANGO_OF_SF_TYPE1_MPLS_LABEL)
    {
        if ((ret = dal_mango_mpls_encap_destroy(unit, pIgrIns->field1_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if (pIgrIns->field2_type == MANGO_OF_SF_TYPE2_MPLS_LABEL)
    {
        if ((ret = dal_mango_mpls_encap_destroy(unit, pIgrIns->field2_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if (pIgrIns->field3_type == MANGO_OF_IGR_SF_TYPE3_MPLS_LABEL)
    {
        if ((ret = dal_mango_mpls_encap_destroy(unit, pIgrIns->field3_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if (pIgrIns->field4_type == MANGO_OF_IGR_SF_TYPE4_MPLS_LABEL)
    {
        if ((ret = dal_mango_mpls_encap_destroy(unit, pIgrIns->field4_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* free portmask entry */
    if (pIgrIns->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pIgrIns->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, pIgrIns->output_data)) != RT_ERR_OK)
        {
           RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Reverse VID by hash index */
static int32
_dal_mango_of_l2FlowEntryVid_reverse(uint32 unit, uint32 idx, dal_mango_of_l2Entry_t *pEntry)
{
    uint32 ret, bucket_hash_idx = 0, block_id = 0, block_algo = 0;
    dal_mango_of_bucketIdx_t bkt_idx = {0};

    block_id = (idx >= (l2Sram_size[unit]/2)) ? 1 : 0;
    dal_mango_of_l2FlowTblHashAlgo_get(unit, block_id, &block_algo);

    if (block_id == 0)
        bucket_hash_idx = idx/HAL_L2_HASHWIDTH(unit);
    else
        bucket_hash_idx = (idx - (l2Sram_size[unit]/2))/HAL_L2_HASHWIDTH(unit);

    if (block_algo == 0)
        pEntry->matchfd.vidMac.vid = ((bucket_hash_idx & 0x1ff) << 3) | ((bucket_hash_idx & 0xfff) >> 9);
    else
        pEntry->matchfd.vidMac.vid = bucket_hash_idx;

    /* get hash index(s) of SRAM */
    if ((ret = _dal_mango_of_l2_bucket_idx_get(unit, pEntry, &bkt_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (block_algo == 0)
    {
        if (block_id == 0)
            pEntry->matchfd.vidMac.vid = ((bkt_idx.bucket0_hash_idx & 0x1ff) << 3) | ((bkt_idx.bucket0_hash_idx & 0xfff) >> 9);
        else
            pEntry->matchfd.vidMac.vid = ((bkt_idx.bucket1_hash_idx & 0x1ff) << 3) | ((bkt_idx.bucket1_hash_idx & 0xfff) >> 9);
    }
    else
    {
        if (block_id == 0)
            pEntry->matchfd.vidMac.vid = bkt_idx.bucket0_hash_idx;
        else
            pEntry->matchfd.vidMac.vid = bkt_idx.bucket1_hash_idx;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "block_id=%d,block_algo=%d,pEntry->vid=%d",block_id,block_algo,pEntry->matchfd.vidMac.vid);

    return RT_ERR_OK;
}

/* allocate a portmask entry to store the egress ports */
static int32
_dal_mango_of_l2PortMaskEntry_set(uint32 unit, rtk_portmask_t *portmask, uint32 *pIndex)
{
    int32 ret;
    int32 allocIdx = -1;

    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, portmask), RT_ERR_PORT_MASK);

    if ((ret = _dal_mango_l2_mcastFwdIndex_alloc(unit, &allocIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_set(unit, allocIdx, portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    *pIndex = (uint32)allocIdx;
    return RT_ERR_OK;
}

static int32
_dal_mango_of_l2FlowEntry_get(
    uint32                      unit,
    dal_mango_of_memType_t      memType,
    uint32                      entry_idx,
    dal_mango_of_l2Entry_t      *pEntry)
{
    int32           ret;
    uint32          tblName;
    uint32          ipproto_7_1, ipproto_0, vid_11_5, vid_4_0;
    of_l2_entry_t   l2_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, memType=%d, entry_idx=%d", unit, memType, entry_idx);

    switch (memType)
    {
        case MANGO_OF_MEMTYPE_SRAM:
            tblName = MANGO_FT_L2_HASH_FMT0_0t;
            break;
        case MANGO_OF_MEMTYPE_CAM:
            tblName = MANGO_FT_L2_CAM_FMT0_0t;
            break;
        default:
            return RT_ERR_FAILED;
    }

    osal_memset((void *)&l2_entry, 0x00, sizeof(of_l2_entry_t));

    /* get entry N for first part */
    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_VALIDtf, &pEntry->valid,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* invalid entry */
    if (!pEntry->valid)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pEntry->valid == FALSE");
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_FMTtf, &pEntry->fmt,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* specified index is a traditional L2 entry */
    if (pEntry->fmt == 0)
        return RT_ERR_OK;

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_TYPEtf, &pEntry->type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (pEntry->type == MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE)
    {
        if (memType == MANGO_OF_MEMTYPE_SRAM)
        {
            if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_SIPtf, &pEntry->matchfd.fiveTp.sip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_L4SPORTtf, &pEntry->matchfd.fiveTp.l4_sport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (pEntry->matchfd.fiveTp.type == 0)
            {
                if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_IPPROTO_7_1tf, &ipproto_7_1,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
            else
            {
                if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT2_0t, MANGO_FT_L2_HASH_FMT2_0_VID_11_5tf, &vid_11_5,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
        }
        else/* MANGO_OF_MEMTYPE_CAM */
        {
            if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_SIPtf, &pEntry->matchfd.fiveTp.sip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_L4SPORTtf, &pEntry->matchfd.fiveTp.l4_sport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (pEntry->matchfd.fiveTp.type == 0)
            {
                if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_IPPROTOtf, &pEntry->matchfd.fiveTp.fifthData.ipproto,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
            else
            {
                if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT2_0t, MANGO_FT_L2_CAM_FMT2_0_VIDtf, &pEntry->matchfd.fiveTp.fifthData.vid,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
        }
    }
    else
    {
        if ((ret = table_field_mac_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_MACtf, (uint8 *)&pEntry->matchfd.vidMac.mac0.octet[0], \
                                        (uint32 *)&l2_entry) != RT_ERR_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (memType == MANGO_OF_MEMTYPE_SRAM)
        {
            if ((pEntry->type == MANGO_OF_L2ENTRY_TYPE_SA) ||(pEntry->type == MANGO_OF_L2ENTRY_TYPE_DA))
            {
                /* ASIC doesn't return the VID for SRAM, calculate VID by driver. */
                if ((ret = _dal_mango_of_l2FlowEntryVid_reverse(unit, entry_idx, pEntry) != RT_ERR_OK))
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }

            if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_METADATA_CMPtf, &pEntry->matchfd.vidMac.md_cmp,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_METADATA_KEYtf, &pEntry->matchfd.vidMac.metadata,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
        else/* MANGO_OF_MEMTYPE_CAM */
        {
            if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_CAM_FMT0_0_VIDtf, &pEntry->matchfd.vidMac.vid,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
    }

    /* Instrution Part */
    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_GOTO_TBLtf, &pEntry->ins.gotoTbl_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_WRITE_METADATAtf, &pEntry->ins.writeMd_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_WRITE_ACTtf, &pEntry->ins.writeAct_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_CLR_ACTtf, &pEntry->ins.clearAct_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_VLAN_ETHTYPEtf,\
                                &pEntry->ins.etherType_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD0_TYPEtf, &pEntry->ins.field0_type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD0_DATAtf, \
                                &pEntry->ins.field0_data, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD1_TYPEtf, &pEntry->ins.field1_type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD1_DATAtf, \
                                &pEntry->ins.field1_data, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_PUSH_VLANtf,\
                                &pEntry->ins.push_vlan, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_Qtf,\
                                &pEntry->ins.set_queue, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_Q_DATAtf,\
                                &pEntry->ins.qid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_GOTO_TBL_ACTtf, &pEntry->ins.tbl_act,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_GOTO_TBL_IDtf, &pEntry->ins.tbl_id,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_GOTO_TBL_LB_TIMEtf, &pEntry->ins.tbl_lb_time,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (memType == MANGO_OF_MEMTYPE_SRAM)
    {
        if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_HIT_STStf, &pEntry->hit,\
                                    (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_CAM_FMT0_0_HIT_STStf, &pEntry->hit,\
                                    (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    switch (memType)
    {
        case MANGO_OF_MEMTYPE_SRAM:
            tblName = MANGO_FT_L2_HASH_FMT0_1t;
            break;
        case MANGO_OF_MEMTYPE_CAM:
            tblName = MANGO_FT_L2_CAM_FMT0_1t;
            break;
        default:
            return RT_ERR_FAILED;
    }

    /* get entry N+1 for second part */
    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+1, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if (pEntry->type == MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE)
    {
        if (memType == MANGO_OF_MEMTYPE_SRAM)
        {
            if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_DIPtf, &pEntry->matchfd.fiveTp.dip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_L4DPORTtf, &pEntry->matchfd.fiveTp.l4_dport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (pEntry->matchfd.fiveTp.type == 0)
            {
                if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_IPPROTO_0tf, &ipproto_0,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                pEntry->matchfd.fiveTp.fifthData.ipproto = ((ipproto_7_1 << 1) | ipproto_0);
            }
            else
            {
                if ((ret = table_field_get(unit, MANGO_FT_L2_HASH_FMT2_1t, MANGO_FT_L2_HASH_FMT2_1_VID_4_0tf, &vid_4_0,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                pEntry->matchfd.fiveTp.fifthData.vid = ((vid_11_5 << 5) | vid_4_0);
            }
        }
        else/* MANGO_OF_MEMTYPE_CAM */
        {
            if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_1t, MANGO_FT_L2_CAM_FMT1_1_DIPtf, &pEntry->matchfd.fiveTp.dip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_1t, MANGO_FT_L2_CAM_FMT1_1_L4DPORTtf, &pEntry->matchfd.fiveTp.l4_dport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_get(unit, MANGO_FT_L2_CAM_FMT1_1t, MANGO_FT_L2_CAM_FMT1_1_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
    }
    else if (pEntry->type == MANGO_OF_L2ENTRY_TYPE_SA_DA)
    {
        /* get DMAC for entry type SMAC+DMAC */
        if ((ret = table_field_mac_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_MACtf, (uint8 *)&pEntry->matchfd.vidMac.mac1.octet[0], \
                                        (uint32 *)&l2_entry) != RT_ERR_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_WA_OUTPUT_TYPEtf, &pEntry->ins.output_type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_WA_OUTPUT_DATAtf, &pEntry->ins.output_data,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_METADATAtf, &pEntry->ins.metadata,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_METADATA_MSKtf, &pEntry->ins.metadata_mask,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}/* end of _dal_mango_of_l2FlowEntry_get */

static int32
_dal_mango_of_l2FlowEntry_set(
    uint32                      unit,
    dal_mango_of_memType_t      memType,
    uint32                      entry_idx,
    dal_mango_of_l2Entry_t      *pEntry)
{
    int32           ret;
    uint32          val;
    uint32          tblName;
    uint32          ipproto_7_1, ipproto_0, vid_11_5, vid_4_0, zero;
    of_l2_entry_t   l2_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, memType=%d, entry_idx=%d", unit, memType, entry_idx);

    switch (memType)
    {
        case MANGO_OF_MEMTYPE_SRAM:
            tblName = MANGO_FT_L2_HASH_FMT0_0t;
            break;
        case MANGO_OF_MEMTYPE_CAM:
            tblName = MANGO_FT_L2_CAM_FMT0_0t;
            break;
        default:
            return RT_ERR_FAILED;
    }

    osal_memset((void *)&l2_entry, 0x0, sizeof(of_l2_entry_t));

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_VALIDtf, &pEntry->valid,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    val = 1;
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_FMTtf, &val, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_TYPEtf, &pEntry->type, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (pEntry->type == MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE)
    {
        if (memType == MANGO_OF_MEMTYPE_SRAM)
        {
            if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_SIPtf, &pEntry->matchfd.fiveTp.sip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_L4SPORTtf, &pEntry->matchfd.fiveTp.l4_sport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (pEntry->matchfd.fiveTp.type == 0)
            {
                ipproto_7_1 = (pEntry->matchfd.fiveTp.fifthData.ipproto >> 1) & 0x7F;
                if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_IPPROTO_7_1tf, &ipproto_7_1,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
            else
            {
                vid_11_5 = (pEntry->matchfd.fiveTp.fifthData.vid >> 5) & 0x7F;
                if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT2_0t, MANGO_FT_L2_HASH_FMT2_0_VID_11_5tf, &vid_11_5,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
        }
        else/* MANGO_OF_MEMTYPE_CAM */
        {
            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_SIPtf, &pEntry->matchfd.fiveTp.sip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_L4SPORTtf, &pEntry->matchfd.fiveTp.l4_sport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (pEntry->matchfd.fiveTp.type == 0)
            {
                if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_0t, MANGO_FT_L2_CAM_FMT1_0_IPPROTOtf, &pEntry->matchfd.fiveTp.fifthData.ipproto,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
            else
            {
                if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT2_0t, MANGO_FT_L2_CAM_FMT2_0_VIDtf, &pEntry->matchfd.fiveTp.fifthData.vid,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
        }
    }
    else
    {
        if ((ret = table_field_mac_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_MACtf, (uint8 *)&pEntry->matchfd.vidMac.mac0.octet[0], \
                                        (uint32 *)&l2_entry) != RT_ERR_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_VIDtf, &pEntry->matchfd.vidMac.vid,\
                                    (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (memType == MANGO_OF_MEMTYPE_SRAM)
        {
            if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_METADATA_CMPtf, &pEntry->matchfd.vidMac.md_cmp,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_METADATA_KEYtf, &pEntry->matchfd.vidMac.metadata,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
    }

    /* Instrution Part */
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_GOTO_TBLtf, &pEntry->ins.gotoTbl_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_WRITE_METADATAtf, &pEntry->ins.writeMd_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_WRITE_ACTtf, &pEntry->ins.writeAct_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_INS_CLR_ACTtf, &pEntry->ins.clearAct_en,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_VLAN_ETHTYPEtf,\
                                &pEntry->ins.etherType_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD0_TYPEtf, &pEntry->ins.field0_type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD0_DATAtf, \
                                &pEntry->ins.field0_data, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD1_TYPEtf, &pEntry->ins.field1_type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_FIELD1_DATAtf, \
                                &pEntry->ins.field1_data, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_PUSH_VLANtf,\
                                &pEntry->ins.push_vlan, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_Qtf,\
                                &pEntry->ins.set_queue, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_WA_SET_Q_DATAtf,\
                                &pEntry->ins.qid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_GOTO_TBL_ACTtf, &pEntry->ins.tbl_act,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_GOTO_TBL_IDtf, &pEntry->ins.tbl_id,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_GOTO_TBL_LB_TIMEtf, &pEntry->ins.tbl_lb_time,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (memType == MANGO_OF_MEMTYPE_SRAM)
    {
        if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_0_HIT_STStf, &pEntry->hit,\
                                    (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_CAM_FMT0_0_HIT_STStf, &pEntry->hit,\
                                    (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* write entry N for first part */
    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* prepare entry N+1 data */
    switch (memType)
    {
        case MANGO_OF_MEMTYPE_SRAM:
            tblName = MANGO_FT_L2_HASH_FMT0_1t;
            break;
        case MANGO_OF_MEMTYPE_CAM:
            tblName = MANGO_FT_L2_CAM_FMT0_1t;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if (pEntry->type == MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE)
    {
        if (memType == MANGO_OF_MEMTYPE_SRAM)
        {
            if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_DIPtf, &pEntry->matchfd.fiveTp.dip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_L4DPORTtf, &pEntry->matchfd.fiveTp.l4_dport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (pEntry->matchfd.fiveTp.type == 0)
            {
                /* clean FT_L2_HASH_FMT1_1 bit[62:56] because [61:56] must be 0(RESERVED) */
                ipproto_7_1 = 0;
                if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_0t, MANGO_FT_L2_HASH_FMT1_0_IPPROTO_7_1tf, &ipproto_7_1,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                ipproto_0 = (pEntry->matchfd.fiveTp.fifthData.ipproto & 0x1);
                if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT1_1t, MANGO_FT_L2_HASH_FMT1_1_IPPROTO_0tf, &ipproto_0,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
            else
            {
                vid_4_0 = (pEntry->matchfd.fiveTp.fifthData.vid & 0x1F);
                if ((ret = table_field_set(unit, MANGO_FT_L2_HASH_FMT2_1t, MANGO_FT_L2_HASH_FMT2_1_VID_4_0tf, &vid_4_0,\
                                            (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }
        }
        else/* MANGO_OF_MEMTYPE_CAM */
        {
            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_1t, MANGO_FT_L2_CAM_FMT1_1_DIPtf, &pEntry->matchfd.fiveTp.dip,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_1t, MANGO_FT_L2_CAM_FMT1_1_L4DPORTtf, &pEntry->matchfd.fiveTp.l4_dport,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT1_1t, MANGO_FT_L2_CAM_FMT1_1_TUPLE_TYPEtf, &pEntry->matchfd.fiveTp.type,\
                                        (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            /* leverage MANGO_FT_L2_CAM_FMT2_0t to clean FT_L2_CAM_FMT1_1 bit[74:56]=(RESERVED) */
            zero = 0;
            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT2_0t, MANGO_FT_L2_CAM_FMT2_0_VIDtf, &zero, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            if ((ret = table_field_set(unit, MANGO_FT_L2_CAM_FMT2_0t, MANGO_FT_L2_CAM_FMT2_0_HIT_STStf, &zero, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
    }
    else if (pEntry->type == MANGO_OF_L2ENTRY_TYPE_SA_DA)
    {
        /* set DMAC for entry type SMAC+DMAC */
        if ((ret = table_field_mac_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_MACtf, (uint8 *)&pEntry->matchfd.vidMac.mac1.octet[0], \
                                        (uint32 *)&l2_entry) != RT_ERR_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_WA_OUTPUT_TYPEtf, &pEntry->ins.output_type,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_WA_OUTPUT_DATAtf, &pEntry->ins.output_data,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_METADATAtf, &pEntry->ins.metadata,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L2_HASH_FMT0_1_METADATA_MSKtf, &pEntry->ins.metadata_mask,\
                                (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* write entry N+1 for second part */
    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+1, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of _dal_mango_of_l2FlowEntry_set */

/* convert rtk_of_l2FlowEntry_t to dal_mango_of_l2Entry_t */
static int32
_dal_mango_of_l2EntryRtk2Dal_convert(uint32 unit, dal_mango_of_l2Entry_t *pDest, rtk_of_l2FlowEntry_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(dal_mango_of_l2Entry_t));

    pDest->valid = TRUE;
    pDest->fmt = 1;

    switch (pSrc->type)
    {
        case OF_L2_FLOW_ENTRY_TYPE_SA:
            pDest->type = MANGO_OF_L2ENTRY_TYPE_SA;
            pDest->matchfd.vidMac.vid = 0;
            break;
        case OF_L2_FLOW_ENTRY_TYPE_VID_SA:
            pDest->type = MANGO_OF_L2ENTRY_TYPE_SA;
            pDest->matchfd.vidMac.vid = pSrc->matchfield.vidMac.vid;
            break;
        case OF_L2_FLOW_ENTRY_TYPE_DA:
            pDest->type = MANGO_OF_L2ENTRY_TYPE_DA;
            pDest->matchfd.vidMac.vid = 0;
            break;
        case OF_L2_FLOW_ENTRY_TYPE_VID_DA:
            pDest->type = MANGO_OF_L2ENTRY_TYPE_DA;
            pDest->matchfd.vidMac.vid = pSrc->matchfield.vidMac.vid;
            break;
        case OF_L2_FLOW_ENTRY_TYPE_SA_DA:
            pDest->type = MANGO_OF_L2ENTRY_TYPE_SA_DA;
            pDest->matchfd.vidMac.vid = 0;
            break;
        case OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO:
        case OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID:
            pDest->type = MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (pSrc->type)
    {
        case OF_L2_FLOW_ENTRY_TYPE_SA:
        case OF_L2_FLOW_ENTRY_TYPE_VID_SA:
            osal_memcpy(&pDest->matchfd.vidMac.mac0, &pSrc->matchfield.vidMac.smac, sizeof(rtk_mac_t));
            break;
        case OF_L2_FLOW_ENTRY_TYPE_DA:
        case OF_L2_FLOW_ENTRY_TYPE_VID_DA:
            osal_memcpy(&pDest->matchfd.vidMac.mac0, &pSrc->matchfield.vidMac.dmac, sizeof(rtk_mac_t));
            break;
        case OF_L2_FLOW_ENTRY_TYPE_SA_DA:
            osal_memcpy(&pDest->matchfd.vidMac.mac0, &pSrc->matchfield.vidMac.smac, sizeof(rtk_mac_t));
            osal_memcpy(&pDest->matchfd.vidMac.mac1, &pSrc->matchfield.vidMac.dmac, sizeof(rtk_mac_t));
            break;
        case OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO:
        case OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID:
            osal_memcpy(&pDest->matchfd.fiveTp.sip, &pSrc->matchfield.fiveTp.sip, sizeof(rtk_ip_addr_t));
            osal_memcpy(&pDest->matchfd.fiveTp.dip, &pSrc->matchfield.fiveTp.dip, sizeof(rtk_ip_addr_t));
            pDest->matchfd.fiveTp.l4_sport = pSrc->matchfield.fiveTp.l4_sport;
            pDest->matchfd.fiveTp.l4_dport = pSrc->matchfield.fiveTp.l4_dport;
            if (pSrc->type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO)
            {
                pDest->matchfd.fiveTp.type = 0;
                pDest->matchfd.fiveTp.fifthData.ipproto = pSrc->matchfield.fiveTp.fifthData.ipproto;
            }
            else
            {
                pDest->matchfd.fiveTp.type = 1;
                pDest->matchfd.fiveTp.fifthData.vid = pSrc->matchfield.fiveTp.fifthData.vid;
            }
            break;
       default:
            return RT_ERR_INPUT;
    }

    if (pSrc->flags & RTK_OF_FLAG_FLOWENTRY_MD_CMP)
        pDest->matchfd.vidMac.md_cmp = TRUE;
    pDest->matchfd.vidMac.metadata = (pSrc->matchfield.vidMac.metadata & 0x3F);

    pDest->ins.clearAct_en = pSrc->ins.clearAct_en;
    pDest->ins.writeAct_en = pSrc->ins.writeAct_en;
    pDest->ins.writeMd_en = pSrc->ins.writeMetadata_en;
    pDest->ins.gotoTbl_en = pSrc->ins.gotoTbl_en;
    pDest->ins.push_vlan = pSrc->ins.wa_data.push_vlan;
    pDest->ins.etherType_idx = pSrc->ins.wa_data.push_vlan_data.etherType_idx;
    switch (pSrc->ins.wa_data.set_field_0_data.field_type)
    {
        case OF_IGR_FT_SF_TYPE0_NONE:
            pDest->ins.field0_type = MANGO_OF_L2_FIELD_TYPE0_NONE;
            break;
        case OF_IGR_FT_SF_TYPE0_VLAN_ID:
            pDest->ins.field0_type = MANGO_OF_L2_FIELD_TYPE0_VLAN_ID;
            break;
        case OF_IGR_FT_SF_TYPE0_IP_DSCP:
            pDest->ins.field0_type = MANGO_OF_L2_FIELD_TYPE0_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field0_data = pSrc->ins.wa_data.set_field_0_data.field_data;

    switch (pSrc->ins.wa_data.set_field_1_data.field_type)
    {
        case OF_IGR_FT_SF_TYPE1_NONE:
            pDest->ins.field1_type = MANGO_OF_L2_FIELD_TYPE1_NONE;
            break;
        case OF_IGR_FT_SF_TYPE1_VLAN_PRI:
            pDest->ins.field1_type = MANGO_OF_L2_FIELD_TYPE1_VLAN_PRI;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field1_data = pSrc->ins.wa_data.set_field_1_data.field_data;

    pDest->ins.set_queue = pSrc->ins.wa_data.set_queue;
    pDest->ins.qid = pSrc->ins.wa_data.qid;

    switch (pSrc->ins.wa_data.output_data.type)
    {
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_DISABLE;
            pDest->ins.output_data = 0;
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_PHY_PORT;
            pDest->ins.output_data = (pSrc->ins.wa_data.output_data.port & 0x3F);
            pDest->ins.output_data = pDest->ins.output_data | ((pSrc->ins.wa_data.output_data.devID & 0xF) << 6);
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_TRK_PORT;
            pDest->ins.output_data = pSrc->ins.wa_data.output_data.trunk;
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT;
            /* portmask translate to portmask index is handled outside */
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_IN_PORT;
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_FLOOD;
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_LB:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_LB;
            break;
        case OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL:
            pDest->ins.output_type = MANGO_OF_L2_OUTPUT_TYPE_TUNNEL;
            /* convert interface to tunnel index */
            if (_dal_mango_tunnel_intf2tunnelIdx(unit, &pDest->ins.output_data, pSrc->ins.wa_data.output_data.intf) != RT_ERR_OK)
                    return RT_ERR_INPUT;
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "intf=0x%x,tunnel_idx=%d", pSrc->ins.wa_data.output_data.intf, pDest->ins.output_data);
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->ins.metadata = pSrc->ins.wm_data.data;
    pDest->ins.metadata_mask = pSrc->ins.wm_data.mask;

    switch (pSrc->ins.gt_data.type)
    {
        case OF_GOTOTBL_TYPE_NORMAL:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_NORMAL;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id;
            break;
        case OF_GOTOTBL_TYPE_APPLY_AND_LB:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_APPLY_AND_LB;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->ins.tbl_lb_time = pSrc->ins.gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        case OF_GOTOTBL_TYPE_LB:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_LB;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->ins.tbl_lb_time = pSrc->ins.gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* convert dal_mango_of_l2Entry_t to rtk_of_l2FlowEntry_t */
static int32
_dal_mango_of_l2EntryDal2Rtk_convert(uint32 unit, rtk_of_l2FlowEntry_t *pDest, dal_mango_of_l2Entry_t *pSrc)
{
    int32   ret;

    osal_memset(pDest, 0x00, sizeof(rtk_of_l2FlowEntry_t));

    switch (pSrc->type)
    {
        case MANGO_OF_L2ENTRY_TYPE_SA:
            if (pSrc->matchfd.vidMac.vid == 0)
                pDest->type = OF_L2_FLOW_ENTRY_TYPE_SA;
            else
                pDest->type = OF_L2_FLOW_ENTRY_TYPE_VID_SA;
            break;
        case MANGO_OF_L2ENTRY_TYPE_DA:
            if (pSrc->matchfd.vidMac.vid == 0)
                pDest->type = OF_L2_FLOW_ENTRY_TYPE_DA;
            else
                pDest->type = OF_L2_FLOW_ENTRY_TYPE_VID_DA;
            break;
        case MANGO_OF_L2ENTRY_TYPE_SA_DA:
            pDest->type = OF_L2_FLOW_ENTRY_TYPE_SA_DA;
            break;
        case MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE:
            if (pSrc->matchfd.fiveTp.type == 0)
                pDest->type = OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO;
            else
                pDest->type = OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->matchfield.vidMac.vid = pSrc->matchfd.vidMac.vid;

    switch (pSrc->type)
    {
        case MANGO_OF_L2ENTRY_TYPE_SA:
            osal_memcpy(&pDest->matchfield.vidMac.smac, &pSrc->matchfd.vidMac.mac0, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_L2ENTRY_TYPE_DA:
            osal_memcpy(&pDest->matchfield.vidMac.dmac, &pSrc->matchfd.vidMac.mac0, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_L2ENTRY_TYPE_SA_DA:
            osal_memcpy(&pDest->matchfield.vidMac.smac, &pSrc->matchfd.vidMac.mac0, sizeof(rtk_mac_t));
            osal_memcpy(&pDest->matchfield.vidMac.dmac, &pSrc->matchfd.vidMac.mac1, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE:
            osal_memcpy(&pDest->matchfield.fiveTp.sip, &pSrc->matchfd.fiveTp.sip, sizeof(rtk_ip_addr_t));
            osal_memcpy(&pDest->matchfield.fiveTp.dip, &pSrc->matchfd.fiveTp.dip, sizeof(rtk_ip_addr_t));
            pDest->matchfield.fiveTp.l4_sport = pSrc->matchfd.fiveTp.l4_sport;
            pDest->matchfield.fiveTp.l4_dport = pSrc->matchfd.fiveTp.l4_dport;
            if (pSrc->matchfd.fiveTp.type == 0)
                pDest->matchfield.fiveTp.fifthData.ipproto = pSrc->matchfd.fiveTp.fifthData.ipproto;
            else
                pDest->matchfield.fiveTp.fifthData.vid = pSrc->matchfd.fiveTp.fifthData.vid;
            break;
       default:
            return RT_ERR_FAILED;
    }

    if (pSrc->matchfd.vidMac.md_cmp == TRUE)
        pDest->flags |= RTK_OF_FLAG_FLOWENTRY_MD_CMP;
    pDest->matchfield.vidMac.metadata = pSrc->matchfd.vidMac.metadata;

    pDest->ins.clearAct_en = pSrc->ins.clearAct_en;
    pDest->ins.writeAct_en = pSrc->ins.writeAct_en;
    pDest->ins.writeMetadata_en = pSrc->ins.writeMd_en;
    pDest->ins.gotoTbl_en = pSrc->ins.gotoTbl_en;

    pDest->ins.wa_data.push_vlan = pSrc->ins.push_vlan;
    pDest->ins.wa_data.push_vlan_data.etherType_idx = pSrc->ins.etherType_idx;
    switch (pSrc->ins.field0_type)
    {
        case MANGO_OF_L2_FIELD_TYPE0_NONE:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT_SF_TYPE0_NONE;
            break;
        case MANGO_OF_L2_FIELD_TYPE0_VLAN_ID:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT_SF_TYPE0_VLAN_ID;
            break;
        case MANGO_OF_L2_FIELD_TYPE0_IP_DSCP:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT_SF_TYPE0_IP_DSCP;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.wa_data.set_field_0_data.field_data = pSrc->ins.field0_data;

    switch (pSrc->ins.field1_type)
    {
        case MANGO_OF_L2_FIELD_TYPE1_NONE:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT_SF_TYPE1_NONE;
            break;
        case MANGO_OF_L2_FIELD_TYPE1_VLAN_PRI:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT_SF_TYPE1_VLAN_PRI;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.wa_data.set_field_1_data.field_data = pSrc->ins.field1_data;

    pDest->ins.wa_data.set_queue = pSrc->ins.set_queue;
    pDest->ins.wa_data.qid = pSrc->ins.qid;

    switch (pSrc->ins.output_type)
    {
        case MANGO_OF_L2_OUTPUT_TYPE_DISABLE:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE;
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_PHY_PORT:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT;
            pDest->ins.wa_data.output_data.port = pSrc->ins.output_data & 0x3F;
            pDest->ins.wa_data.output_data.devID = (pSrc->ins.output_data >> 6) & 0xF;
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_TRK_PORT:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT;
            pDest->ins.wa_data.output_data.trunk = pSrc->ins.output_data;
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT;
            /* convert portmask index to portmask */
            if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->ins.output_data, \
                            &pDest->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_IN_PORT:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT;
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_FLOOD:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD;
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_LB:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_LB;
            break;
        case MANGO_OF_L2_OUTPUT_TYPE_TUNNEL:
            pDest->ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL;
            /* convert tunnel index to interface */
            _dal_mango_tunnel_tunnelIdx2intf(unit, &pDest->ins.wa_data.output_data.intf, pSrc->ins.output_data);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "tunnel_idx=%d,intf=0x%x", pSrc->ins.output_data, pDest->ins.wa_data.output_data.intf);
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->ins.wm_data.data = pSrc->ins.metadata;
    pDest->ins.wm_data.mask = pSrc->ins.metadata_mask;
    switch (pSrc->ins.tbl_act)
    {
        case OF_GOTOTBL_TYPE_NORMAL:
            pDest->ins.gt_data.type = MANGO_OF_GOTOTBL_NORMAL;
            break;
        case OF_GOTOTBL_TYPE_APPLY_AND_LB:
            pDest->ins.gt_data.type = MANGO_OF_GOTOTBL_APPLY_AND_LB;
            break;
        case OF_GOTOTBL_TYPE_LB:
            pDest->ins.gt_data.type = MANGO_OF_GOTOTBL_LB;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.gt_data.tbl_id = pSrc->ins.tbl_id + (pSrc->ins.tbl_lb_time * HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit));

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l2FlowEntry_cmp(uint32 unit, dal_mango_of_l2Entry_t *pSrc, dal_mango_of_l2Entry_t *pDest)
{
    if (pSrc->type != pDest->type)
        return RT_ERR_FAILED;

    if (pSrc->matchfd.vidMac.vid != pDest->matchfd.vidMac.vid)
        return RT_ERR_FAILED;

    if (osal_memcmp(&pSrc->matchfd.vidMac.mac0, &pDest->matchfd.vidMac.mac0, sizeof(rtk_mac_t)))
        return RT_ERR_FAILED;

    if (osal_memcmp(&pSrc->matchfd.vidMac.mac1, &pDest->matchfd.vidMac.mac1, sizeof(rtk_mac_t)))
        return RT_ERR_FAILED;

    if (pSrc->matchfd.vidMac.md_cmp != pDest->matchfd.vidMac.md_cmp)
        return RT_ERR_FAILED;

    if (pSrc->matchfd.vidMac.metadata != pDest->matchfd.vidMac.metadata)
        return RT_ERR_FAILED;

    if (pSrc->matchfd.fiveTp.type != pDest->matchfd.fiveTp.type)
        return RT_ERR_FAILED;

    if (osal_memcmp(&pSrc->matchfd.fiveTp.fifthData, &pDest->matchfd.fiveTp.fifthData, sizeof(uint32)))
        return RT_ERR_FAILED;

    if (osal_memcmp(&pSrc->matchfd.fiveTp.sip, &pDest->matchfd.fiveTp.sip, sizeof(rtk_ip_addr_t)))
        return RT_ERR_FAILED;

    if (osal_memcmp(&pSrc->matchfd.fiveTp.dip, &pDest->matchfd.fiveTp.dip, sizeof(rtk_ip_addr_t)))
        return RT_ERR_FAILED;

    if (pSrc->matchfd.fiveTp.l4_sport != pDest->matchfd.fiveTp.l4_sport)
        return RT_ERR_FAILED;

    if (pSrc->matchfd.fiveTp.l4_dport != pDest->matchfd.fiveTp.l4_dport)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l2FlowEntry_getByMethod(
    uint32                      unit,
    dal_mango_of_l2Entry_t      *pEntry,
    dal_mango_of_getMethod_t    method,
    dal_mango_of_getResult_t    *pResult)
{
    int32  ret;
    uint32 i, entry_idx, hash_way;
    rtk_enable_t camState;
    dal_mango_of_bucketIdx_t bkt_idx = {0};
    dal_mango_of_l2Entry_t l2_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, method=%d", unit, method);

    pResult->isFree = FALSE;
    pResult->isExist = FALSE;

    /* get hash index(s) of SRAM */
    if ((ret = _dal_mango_of_l2_bucket_idx_get(unit, pEntry, &bkt_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    hash_way = HAL_L2_HASHDEPTH(unit);

    /* Search bucket 0 */
    entry_idx = bkt_idx.bucket0_idx;
    for (i = 0; i < hash_way; i++)
    {
        if ((i%2) == 1)
            continue;

        entry_idx += i;
        osal_memset((void *)&l2_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));
        if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_SRAM, entry_idx, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (((method == MANGO_OF_GETMETHOD_FREE) ||
            (method == MANGO_OF_GETMETHOD_EXIST_FIRST)) &&
            (l2_entry.valid == FALSE))
        {
            if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_SRAM, entry_idx+1, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (l2_entry.valid == FALSE)
            {
                pResult->memType = MANGO_OF_MEMTYPE_SRAM;
                pResult->isFree = TRUE;
                pResult->idx = entry_idx;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "free index=%d", pResult->idx);

                /* for type MANGO_OF_GETMETHOD_EXIST_FIRST, must keep searching existed entry */
                if (method == MANGO_OF_GETMETHOD_FREE)
                    return RT_ERR_OK;
            }
        }

        if (((method == MANGO_OF_GETMETHOD_EXIST) ||
            (method == MANGO_OF_GETMETHOD_EXIST_FIRST)) &&
            (l2_entry.valid && l2_entry.fmt == 1))
        {
            if (RT_ERR_OK == _dal_mango_of_l2FlowEntry_cmp(unit, pEntry, &l2_entry))
            {
                pResult->memType = MANGO_OF_MEMTYPE_SRAM;
                pResult->isExist = TRUE;
                pResult->idx = entry_idx;
                pEntry->ins = l2_entry.ins;
                pEntry->hit = l2_entry.hit;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "exist index=%d", pResult->idx);
                return RT_ERR_OK;
            }
        }
    }

    /* Search bucket 1 */
    pResult->memType = MANGO_OF_MEMTYPE_SRAM;
    entry_idx = bkt_idx.bucket1_idx;
    for (i = 0; i < hash_way; i++)
    {
        if ((i%2) == 1)
            continue;

        entry_idx += i;
        osal_memset((void *)&l2_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));
        if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_SRAM, entry_idx, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((pResult->isFree == FALSE) &&
            (((method == MANGO_OF_GETMETHOD_FREE) || (method == MANGO_OF_GETMETHOD_EXIST_FIRST)) && (l2_entry.valid == FALSE)))
        {
            if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_SRAM, entry_idx+1, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (l2_entry.valid == FALSE)
            {
                pResult->memType = MANGO_OF_MEMTYPE_SRAM;
                pResult->isFree = TRUE;
                pResult->idx = entry_idx;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "free index=%d", pResult->idx);

                if (method == MANGO_OF_GETMETHOD_FREE)
                    return RT_ERR_OK;
            }
        }

        if (((method == MANGO_OF_GETMETHOD_EXIST) ||
            (method == MANGO_OF_GETMETHOD_EXIST_FIRST)) &&
            (l2_entry.valid && l2_entry.fmt == 1))
        {
            if (RT_ERR_OK == _dal_mango_of_l2FlowEntry_cmp(unit, pEntry, &l2_entry))
            {
                pResult->memType = MANGO_OF_MEMTYPE_SRAM;
                pResult->isExist = TRUE;
                pResult->idx = entry_idx;
                pEntry->ins = l2_entry.ins;
                pEntry->hit = l2_entry.hit;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "exist index=%d", pResult->idx);
                return RT_ERR_OK;
            }
        }
    }

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &camState))!=RT_ERR_OK)
        return ret;

    if ((camState == DISABLED) || (pEntry->matchfd.vidMac.md_cmp == TRUE))/* CAM doesn't support metadata */
        return RT_ERR_OK;

    /* Search CAM. CAM 0 index=0,2,4..., CAM 1 index=1,3,5... */
    for (entry_idx = 0; entry_idx < l2Cam_size[unit]; entry_idx+=2)
    {
        osal_memset((void *)&l2_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));
        if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_CAM, entry_idx, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((pResult->isFree == FALSE) &&
            (((method == MANGO_OF_GETMETHOD_FREE) ||(method == MANGO_OF_GETMETHOD_EXIST_FIRST)) && (l2_entry.valid == FALSE)))
        {
            if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_CAM, entry_idx+1, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (l2_entry.valid == FALSE)
            {
                pResult->memType = MANGO_OF_MEMTYPE_CAM;
                pResult->isFree = TRUE;
                pResult->idx = entry_idx;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "free index=%d", pResult->idx);
                if (method == MANGO_OF_GETMETHOD_FREE)
                    return RT_ERR_OK;
            }
        }

        if (((method == MANGO_OF_GETMETHOD_EXIST) ||
            (method == MANGO_OF_GETMETHOD_EXIST_FIRST)) &&
            (l2_entry.valid && l2_entry.fmt == 1))
        {
            if (RT_ERR_OK == _dal_mango_of_l2FlowEntry_cmp(unit, pEntry, &l2_entry))
            {
                pResult->memType = MANGO_OF_MEMTYPE_CAM;
                pResult->isExist = TRUE;
                pResult->idx = entry_idx;
                pEntry->ins = l2_entry.ins;
                pEntry->hit = l2_entry.hit;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "exist index=%d", pResult->idx);
                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3EntryResource_alloc(
    uint32 unit,
    dal_mango_of_l3Ins_t *pIns,
    rtk_mac_t smac,
    rtk_mac_t dmac,
    rtk_portmask_t *pEgrPortmsk)
{
    int32  ret;
    uint32 return_idx;

    if (pIns->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pIns->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pEgrPortmsk), RT_ERR_PORT_MASK);
    }

    /* allocate a L3 interface entry to store the SA */
    if (pIns->field0_type == MANGO_OF_L3_FIELD_TYPE0_SA)
    {
        if ((ret = _dal_mango_of_l3IntfEgr_set(unit, smac, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIns->field0_data = return_idx;
    }

    /* add a OpenFlow DAMC entry to store the DA */
    if (pIns->field1_type == MANGO_OF_L3_FIELD_TYPE1_DA)
    {
        if ((ret = _dal_mango_of_dmac_set(unit, &dmac, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIns->field1_data = return_idx;
    }

    /* allocate a portmask entry to store the egress ports */
    if (pIns->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pIns->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        if ((ret = _dal_mango_of_l2PortMaskEntry_set(unit, pEgrPortmsk,\
                    &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pIns->output_data = return_idx;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3EntryResource_free(uint32 unit, dal_mango_of_l3Ins_t *pIns)
{
    int32  ret;

    /* free L3 interface entry which stored the SA */
    if (pIns->field0_type == MANGO_OF_L3_FIELD_TYPE0_SA)
    {
        if ((ret = _dal_mango_l3_intfEntry_free(unit, pIns->field0_data, RTK_L3_FLAG_NONE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* free OpenFlow DMAC entry which stored the DA */
    if (pIns->field1_type == MANGO_OF_L3_FIELD_TYPE1_DA)
    {
        if ((ret = _dal_mango_of_dmacEntry_free(unit, pIns->field1_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* free portmask entry which stored the egress ports */
    if (pIns->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pIns->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, pIns->output_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, dal_mango_of_l3CamEntry_t *pEntry)
{
    int32  ret;
    uint32 tblName;
    uint32 ip0_31_0, ip1_31_0, ip0_31_0_msk, ip1_31_0_msk;
    uint32 ip0_79_32[2], ip0_127_80[2], ip1_79_32[2], ip1_127_80[2];
    uint32 ip0_79_32_msk[2], ip0_127_80_msk[2], ip1_79_32_msk[2], ip1_127_80_msk[2];
    uint32 ip6_u32[4], ip6_u32_msk[4];
    of_l3_cam_entry_t l3_entry;
    uint32 fmt_1_2[9] = {0};

    osal_memset((void *)pEntry, 0x00, sizeof(dal_mango_of_l3CamEntry_t));
    osal_memset((void *)&l3_entry, 0x00, sizeof(of_l3_cam_entry_t));
    tblName = MANGO_FT_L3_TCAM_0t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_VALIDtf, &pEntry->valid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* invalid entry */
    if (!pEntry->valid)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pEntry->valid == FALSE");
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_FMTtf, &pEntry->fmt,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* specified index is a traditional L3 entry.
     * Should not happen because caller already offset the traditional entries.
     */
    if (pEntry->fmt == 0)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pEntry->fmt == 0");
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_IP_VERtf, &pEntry->ip_ver,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_TYPEtf, &pEntry->type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_IPtf, &ip0_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_IPtf, &ip0_31_0_msk,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_METADATA_KEYtf, &pEntry->metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_METADATAtf, &pEntry->metadata_msk,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* Instrution Part */
    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_INS_GOTO_TBLtf, &pEntry->ins.gotoTbl_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_INS_WRITE_METADATAtf, &pEntry->ins.writeMd_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_INS_WRITE_ACTtf, &pEntry->ins.writeAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_INS_CLR_ACTtf, &pEntry->ins.clearAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_PUSH_VLANtf,\
                                &pEntry->ins.push_vlan, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_VLAN_ETHTYPEtf,\
                                &pEntry->ins.etherType_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_DEC_IP_TTLtf, &pEntry->ins.dec_ip_ttl,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD0_TYPEtf, &pEntry->ins.field0_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD0_DATAtf, \
                                &pEntry->ins.field0_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD1_TYPEtf, &pEntry->ins.field1_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD1_DATAtf, \
                                &pEntry->ins.field1_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD2_TYPEtf, &pEntry->ins.field2_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD2_DATAtf, \
                                &pEntry->ins.field2_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_GOTO_TBL_LB_TIMEtf, \
                                &pEntry->ins.tbl_lb_time, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_Qtf,\
                                &pEntry->ins.set_queue, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_HIT_STStf,\
                                &pEntry->hit, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }


    /* entry N+1 */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        tblName = MANGO_FT_L3_TCAM_1t;
    else
        tblName = MANGO_FT_L3_TCAM_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+1, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_1_IPtf, &ip1_31_0,\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_1_BMSK_IPtf, &ip1_31_0_msk,\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* table MANGO_FT_L3_TCAM_1t format is different from MANGO_FT_L3_TCAM_2t.
         * MANGO_FT_L3_TCAM_1t has two more fields: MANGO_FT_L3_TCAM_1_METADATA_KEYtf and MANGO_FT_L3_TCAM_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_TCAM_1_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_TCAM_1_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_TCAM_1_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_TCAM_1_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_TCAM_1_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_TCAM_1_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_TCAM_1_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_TCAM_1_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_TCAM_1_WA_SET_Q_DATAtf;
    }
    else
    {
        if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip0_79_32[0],\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip0_79_32_msk[0],\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* table MANGO_FT_L3_TCAM_1t format is different from MANGO_FT_L3_TCAM_2t.
         * MANGO_FT_L3_TCAM_1t has two more fields: MANGO_FT_L3_TCAM_1_METADATA_KEYtf and MANGO_FT_L3_TCAM_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_TCAM_2_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_TCAM_2_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_TCAM_2_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_TCAM_2_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_TCAM_2_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_TCAM_2_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_TCAM_2_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_TCAM_2_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_TCAM_2_WA_SET_Q_DATAtf;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[0], &pEntry->ins.group,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[1], &pEntry->ins.gid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[2], &pEntry->ins.output_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[3], &pEntry->ins.output_data,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[4], &pEntry->ins.metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[5], &pEntry->ins.metadata_mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[6], &pEntry->ins.tbl_act,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[7], &pEntry->ins.tbl_id,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[8],\
                                &pEntry->ins.qid, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* IPv4 entry */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        switch (pEntry->type)
        {
            case MANGO_OF_L3ENTRY_TYPE_SIP:
            case MANGO_OF_L3ENTRY_TYPE_DIP:
                 pEntry->ip0.ipv4 = ip0_31_0;
                 pEntry->ip0_msk.ipv4 = ip0_31_0_msk;
                 break;
            case MANGO_OF_L3ENTRY_TYPE_SIP_DIP:
                 pEntry->ip0.ipv4 = ip0_31_0;
                 pEntry->ip0_msk.ipv4 = ip0_31_0_msk;
                 pEntry->ip1.ipv4 = ip1_31_0;
                 pEntry->ip1_msk.ipv4 = ip1_31_0_msk;
                 break;
            default:
                 break;
        }

        return RT_ERR_OK;
    }

    /* entry N+2 */
    tblName = MANGO_FT_L3_TCAM_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+2, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip0_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip0_127_80_msk[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    ip6_u32[0] = ((ip0_127_80[1] & 0x0000FFFF) << 16) | ((ip0_127_80[0] & 0xFFFF0000) >> 16);
    ip6_u32[1] = ((ip0_127_80[0] & 0x0000FFFF) << 16) | (ip0_79_32[1] & 0x0000FFFF);
    ip6_u32[2] = ip0_79_32[0];
    ip6_u32[3] = ip0_31_0;
    ip6_u32_msk[0] = ((ip0_127_80_msk[1] & 0x0000FFFF) << 16) | ((ip0_127_80_msk[0] & 0xFFFF0000) >> 16);
    ip6_u32_msk[1] = ((ip0_127_80_msk[0] & 0x0000FFFF) << 16) | (ip0_79_32_msk[1] & 0x0000FFFF);
    ip6_u32_msk[2] = ip0_79_32_msk[0];
    ip6_u32_msk[3] = ip0_31_0_msk;

    DAL_MANGO_IPV6_U32_TO_U8(ip6_u32, pEntry->ip0.ipv6.octet);
    DAL_MANGO_IPV6_U32_TO_U8(ip6_u32_msk, pEntry->ip0_msk.ipv6.octet);

    /* IPv6 SIP or DIP entry */
    if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
        return RT_ERR_OK;

    /* entry N+3 */
    tblName = MANGO_FT_L3_TCAM_0t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+3, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_IPtf, &ip1_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_IPtf, &ip1_31_0_msk,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* entry N+4 */
    tblName = MANGO_FT_L3_TCAM_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+4, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip1_79_32[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip1_79_32_msk[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* entry N+5 */
    tblName = MANGO_FT_L3_TCAM_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+5, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip1_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip1_127_80_msk[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    ip6_u32[0] = ((ip1_127_80[1] & 0x0000FFFF) << 16) | ((ip1_127_80[0] & 0xFFFF0000) >> 16);
    ip6_u32[1] = ((ip1_127_80[0] & 0x0000FFFF) << 16) | (ip1_79_32[1] & 0x0000FFFF);
    ip6_u32[2] = ip1_79_32[0];
    ip6_u32[3] = ip1_31_0;
    ip6_u32_msk[0] = ((ip1_127_80_msk[1] & 0x0000FFFF) << 16) | ((ip1_127_80_msk[0] & 0xFFFF0000) >> 16);
    ip6_u32_msk[1] = ((ip1_127_80_msk[0] & 0x0000FFFF) << 16) | (ip1_79_32_msk[1] & 0x0000FFFF);
    ip6_u32_msk[2] = ip1_79_32_msk[0];
    ip6_u32_msk[3] = ip1_31_0_msk;

    DAL_MANGO_IPV6_U32_TO_U8(ip6_u32, pEntry->ip1.ipv6.octet);
    DAL_MANGO_IPV6_U32_TO_U8(ip6_u32_msk, pEntry->ip1_msk.ipv6.octet);

#if 0/* use macro DAL_MANGO_IPV6_U32_TO_U8 instead */
    pEntry->ip1.ipv6.octet[0]  = (ip1_127_80[1] & 0x0000FF00) >> 8;
    pEntry->ip1.ipv6.octet[1]  = (ip1_127_80[1] & 0x000000FF) >> 0;
    pEntry->ip1.ipv6.octet[2]  = (ip1_127_80[0] & 0xFF000000) >> 24;
    pEntry->ip1.ipv6.octet[3]  = (ip1_127_80[0] & 0x00FF0000) >> 16;
    pEntry->ip1.ipv6.octet[4]  = (ip1_127_80[0] & 0x0000FF00) >> 8;
    pEntry->ip1.ipv6.octet[5]  = (ip1_127_80[0] & 0x000000FF) >> 0;
    pEntry->ip1.ipv6.octet[6]  = (ip1_79_32[1] & 0x0000FF00) >> 8;
    pEntry->ip1.ipv6.octet[7]  = (ip1_79_32[1] & 0x000000FF) >> 0;
    pEntry->ip1.ipv6.octet[8]  = (ip1_79_32[0] & 0xFF000000) >> 24;
    pEntry->ip1.ipv6.octet[9]  = (ip1_79_32[0] & 0x00FF0000) >> 16;
    pEntry->ip1.ipv6.octet[10] = (ip1_79_32[0] & 0x0000FF00) >> 8;
    pEntry->ip1.ipv6.octet[11] = (ip1_79_32[0] & 0x000000FF) >> 0;
    pEntry->ip1.ipv6.octet[12] = (ip1_31_0 & 0xFF000000) >> 24;
    pEntry->ip1.ipv6.octet[13] = (ip1_31_0 & 0x00FF0000) >> 16;
    pEntry->ip1.ipv6.octet[14] = (ip1_31_0 & 0x0000FF00) >> 8;
    pEntry->ip1.ipv6.octet[15] = (ip1_31_0 & 0x000000FF) >> 0;
#endif

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3CamFlowEntry_set(uint32 unit, uint32 entry_idx, dal_mango_of_l3CamEntry_t *pEntry)
{
    int32  ret;
    uint32 tblName;
    uint32 ip0_31_0, ip1_31_0, ip0_31_0_msk, ip1_31_0_msk;
    uint32 ip0_79_32[2], ip0_127_80[2], ip1_79_32[2], ip1_127_80[2];
    uint32 ip0_79_32_msk[2], ip0_127_80_msk[2], ip1_79_32_msk[2], ip1_127_80_msk[2];
    of_l3_cam_entry_t l3_entry;
    uint32 mask = 1, rsvd = 0;
    uint32 fmt_1_2[9] = {0};

    osal_memset((void *)&l3_entry, 0x00, sizeof(of_l3_cam_entry_t));
    tblName = MANGO_FT_L3_TCAM_0t;

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_VALIDtf, &pEntry->valid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_FMTtf, &pEntry->fmt,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_FMTtf, &mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_IP_VERtf, &pEntry->ip_ver,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_IP_VERtf, &mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_TYPEtf, &pEntry->type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    mask = 3;
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_TYPEtf, &mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        ip0_31_0 = pEntry->ip0.ipv4;
        ip0_31_0_msk = pEntry->ip0_msk.ipv4;
    }
    else
    {
        ip0_31_0 =     ((pEntry->ip0.ipv6.octet[12] << 24) |
                        (pEntry->ip0.ipv6.octet[13] << 16) |
                        (pEntry->ip0.ipv6.octet[14] << 8)  |
                         pEntry->ip0.ipv6.octet[15]);
        ip0_31_0_msk = ((pEntry->ip0_msk.ipv6.octet[12] << 24) |
                        (pEntry->ip0_msk.ipv6.octet[13] << 16) |
                        (pEntry->ip0_msk.ipv6.octet[14] << 8)  |
                         pEntry->ip0_msk.ipv6.octet[15]);
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_IPtf, &ip0_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_IPtf, &ip0_31_0_msk,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_METADATA_KEYtf, &pEntry->metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_METADATAtf, &pEntry->metadata_msk,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* Instrution Part */
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_INS_GOTO_TBLtf, &pEntry->ins.gotoTbl_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_INS_WRITE_METADATAtf, &pEntry->ins.writeMd_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_INS_WRITE_ACTtf, &pEntry->ins.writeAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_INS_CLR_ACTtf, &pEntry->ins.clearAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_PUSH_VLANtf,\
                                &pEntry->ins.push_vlan, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_VLAN_ETHTYPEtf,\
                                &pEntry->ins.etherType_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_DEC_IP_TTLtf, &pEntry->ins.dec_ip_ttl,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD0_TYPEtf, &pEntry->ins.field0_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD0_DATAtf, \
                                &pEntry->ins.field0_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD1_TYPEtf, &pEntry->ins.field1_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD1_DATAtf, \
                                &pEntry->ins.field1_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD2_TYPEtf, &pEntry->ins.field2_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_FIELD2_DATAtf, \
                                &pEntry->ins.field2_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_GOTO_TBL_LB_TIMEtf, \
                                &pEntry->ins.tbl_lb_time, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_WA_SET_Qtf,\
                                &pEntry->ins.set_queue, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* pEntry->hit = 0 means clear hit bit, pEntry->hit = 1 means keep */
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_HIT_STStf,\
                                &pEntry->hit, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);


    /* entry N+1 */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        tblName = MANGO_FT_L3_TCAM_1t;
    else
        tblName = MANGO_FT_L3_TCAM_2t;

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        {
            ip1_31_0 = pEntry->ip1.ipv4;
            ip1_31_0_msk = pEntry->ip1_msk.ipv4;

            if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_1_IPtf, &ip1_31_0,\
                                        (uint32 *)&l3_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_1_BMSK_IPtf, &ip1_31_0_msk,\
                                        (uint32 *)&l3_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }

        /* table MANGO_FT_L3_TCAM_1t format is different from MANGO_FT_L3_TCAM_2t.
         * MANGO_FT_L3_TCAM_1t has two more fields: MANGO_FT_L3_TCAM_1_METADATA_KEYtf and MANGO_FT_L3_TCAM_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_TCAM_1_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_TCAM_1_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_TCAM_1_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_TCAM_1_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_TCAM_1_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_TCAM_1_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_TCAM_1_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_TCAM_1_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_TCAM_1_WA_SET_Q_DATAtf;
    }
    else if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        ip0_79_32[1]     = (pEntry->ip0.ipv6.octet[6] << 8) | pEntry->ip0.ipv6.octet[7];
        ip0_79_32[0]     = ((pEntry->ip0.ipv6.octet[8] << 24) |
                            (pEntry->ip0.ipv6.octet[9] << 16) |
                            (pEntry->ip0.ipv6.octet[10] << 8) |
                             pEntry->ip0.ipv6.octet[11]);
        ip0_79_32_msk[1] = (pEntry->ip0_msk.ipv6.octet[6] << 8) | pEntry->ip0_msk.ipv6.octet[7];
        ip0_79_32_msk[0] = ((pEntry->ip0_msk.ipv6.octet[8] << 24) |
                            (pEntry->ip0_msk.ipv6.octet[9] << 16) |
                            (pEntry->ip0_msk.ipv6.octet[10] << 8) |
                             pEntry->ip0_msk.ipv6.octet[11]);

        if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip0_79_32[0],\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip0_79_32_msk[0],\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* table MANGO_FT_L3_TCAM_1t format is different from MANGO_FT_L3_TCAM_2t.
         * MANGO_FT_L3_TCAM_1t has two more fields: MANGO_FT_L3_TCAM_1_METADATA_KEYtf and MANGO_FT_L3_TCAM_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_TCAM_2_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_TCAM_2_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_TCAM_2_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_TCAM_2_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_TCAM_2_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_TCAM_2_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_TCAM_2_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_TCAM_2_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_TCAM_2_WA_SET_Q_DATAtf;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[0], &pEntry->ins.group,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[1], &pEntry->ins.gid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[2], &pEntry->ins.output_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[3], &pEntry->ins.output_data,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[4], &pEntry->ins.metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[5], &pEntry->ins.metadata_mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[6], &pEntry->ins.tbl_act,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[7], &pEntry->ins.tbl_id,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[8],\
                                &pEntry->ins.qid, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+1, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* IPv4 entry */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        return RT_ERR_OK;

    /* entry N+2 */
    tblName = MANGO_FT_L3_TCAM_2t;

    ip0_127_80[1]     = (pEntry->ip0.ipv6.octet[0] << 8) | pEntry->ip0.ipv6.octet[1];
    ip0_127_80[0]     = ((pEntry->ip0.ipv6.octet[2] << 24) |
                        (pEntry->ip0.ipv6.octet[3] << 16) |
                        (pEntry->ip0.ipv6.octet[4] << 8) |
                         pEntry->ip0.ipv6.octet[5]);
    ip0_127_80_msk[1] = (pEntry->ip0_msk.ipv6.octet[0] << 8) | pEntry->ip0_msk.ipv6.octet[1];
    ip0_127_80_msk[0] = ((pEntry->ip0_msk.ipv6.octet[2] << 24) |
                        (pEntry->ip0_msk.ipv6.octet[3] << 16) |
                        (pEntry->ip0_msk.ipv6.octet[4] << 8) |
                         pEntry->ip0_msk.ipv6.octet[5]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip0_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip0_127_80_msk[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+2, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* IPv6 SIP or DIP entry */
    if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
        return RT_ERR_OK;

    /* entry N+3 */
    tblName = MANGO_FT_L3_TCAM_0t;
    ip1_31_0 =     ((pEntry->ip1.ipv6.octet[12] << 24) |
                    (pEntry->ip1.ipv6.octet[13] << 16) |
                    (pEntry->ip1.ipv6.octet[14] << 8) |
                     pEntry->ip1.ipv6.octet[15]);
    ip1_31_0_msk = ((pEntry->ip1_msk.ipv6.octet[12] << 24) |
                    (pEntry->ip1_msk.ipv6.octet[13] << 16) |
                    (pEntry->ip1_msk.ipv6.octet[14] << 8) |
                     pEntry->ip1_msk.ipv6.octet[15]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_IPtf, &ip1_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_IPtf, &ip1_31_0_msk,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_0_BMSK_METADATAtf, &rsvd,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+3, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* entry N+4 */
    tblName = MANGO_FT_L3_TCAM_2t;
    ip1_79_32[1]     = (pEntry->ip1.ipv6.octet[6] << 8) | pEntry->ip1.ipv6.octet[7];
    ip1_79_32[0]     = ((pEntry->ip1.ipv6.octet[8] << 24) |
                        (pEntry->ip1.ipv6.octet[9] << 16) |
                        (pEntry->ip1.ipv6.octet[10] << 8) |
                         pEntry->ip1.ipv6.octet[11]);
    ip1_79_32_msk[1] = (pEntry->ip1_msk.ipv6.octet[6] << 8) | pEntry->ip1_msk.ipv6.octet[7];
    ip1_79_32_msk[0] = ((pEntry->ip1_msk.ipv6.octet[8] << 24) |
                        (pEntry->ip1_msk.ipv6.octet[9] << 16) |
                        (pEntry->ip1_msk.ipv6.octet[10] << 8) |
                         pEntry->ip1_msk.ipv6.octet[11]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip1_79_32[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip1_79_32_msk[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+4, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* entry N+5 */
    tblName = MANGO_FT_L3_TCAM_2t;
    ip1_127_80[1]     = (pEntry->ip1.ipv6.octet[0] << 8) | pEntry->ip1.ipv6.octet[1];
    ip1_127_80[0]     = ((pEntry->ip1.ipv6.octet[2] << 24) |
                        (pEntry->ip1.ipv6.octet[3] << 16)  |
                        (pEntry->ip1.ipv6.octet[4] << 8)   |
                         pEntry->ip1.ipv6.octet[5]);
    ip1_127_80_msk[1] = (pEntry->ip1_msk.ipv6.octet[0] << 8) | pEntry->ip1_msk.ipv6.octet[1];
    ip1_127_80_msk[0] = ((pEntry->ip1_msk.ipv6.octet[2] << 24) |
                        (pEntry->ip1_msk.ipv6.octet[3] << 16)  |
                        (pEntry->ip1_msk.ipv6.octet[4] << 8)   |
                         pEntry->ip1_msk.ipv6.octet[5]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_IPtf, &ip1_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_TCAM_2_BMSK_IPtf, &ip1_127_80_msk[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+5, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* convert rtk_of_l3CamFlowEntry_t to dal_mango_of_l3CamEntry_t */
static int32
_dal_mango_of_l3CamEntryRtk2Dal_convert(uint32 unit, dal_mango_of_l3CamEntry_t *pDest, rtk_of_l3CamFlowEntry_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(dal_mango_of_l3CamEntry_t));

    pDest->valid = 1;
    pDest->fmt = 1;

    switch (pSrc->type)
    {
        case OF_L3_FLOW_ENTRY_TYPE_IP4_SIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP4;
             pDest->ip0.ipv4 = pSrc->sip.ipv4;
             pDest->ip0_msk.ipv4 = pSrc->sip_msk.ipv4;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP4_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP4;
             pDest->ip0.ipv4 = pSrc->dip.ipv4;
             pDest->ip0_msk.ipv4 = pSrc->dip_msk.ipv4;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP4;
             pDest->ip0.ipv4 = pSrc->sip.ipv4;
             pDest->ip0_msk.ipv4 = pSrc->sip_msk.ipv4;
             pDest->ip1.ipv4 = pSrc->dip.ipv4;
             pDest->ip1_msk.ipv4 = pSrc->dip_msk.ipv4;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP6_SIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP6;
             pDest->ip0.ipv6 = pSrc->sip.ipv6;
             pDest->ip0_msk.ipv6 = pSrc->sip_msk.ipv6;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP6_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP6;
             pDest->ip0.ipv6 = pSrc->dip.ipv6;
             pDest->ip0_msk.ipv6 = pSrc->dip_msk.ipv6;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP6;
             pDest->ip0.ipv6 = pSrc->sip.ipv6;
             pDest->ip0_msk.ipv6 = pSrc->sip_msk.ipv6;
             pDest->ip1.ipv6 = pSrc->dip.ipv6;
             pDest->ip1_msk.ipv6 = pSrc->dip_msk.ipv6;
             break;
        default:
             return RT_ERR_INPUT;
    }

    pDest->metadata = pSrc->metadata;
    pDest->metadata_msk = pSrc->metadata_msk;

    pDest->ins.clearAct_en = pSrc->ins.clearAct_en;
    pDest->ins.writeAct_en = pSrc->ins.writeAct_en;
    pDest->ins.writeMd_en = pSrc->ins.writeMetadata_en;
    pDest->ins.gotoTbl_en = pSrc->ins.gotoTbl_en;

    pDest->ins.push_vlan = pSrc->ins.wa_data.push_vlan;
    pDest->ins.etherType_idx = pSrc->ins.wa_data.push_vlan_data.etherType_idx;
    pDest->ins.dec_ip_ttl= pSrc->ins.wa_data.dec_ip_ttl;

    switch (pSrc->ins.wa_data.set_field_0_data.field_type)
    {
        case OF_IGR_FT2_ACT_SF_TYPE0_NONE:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_NONE;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE0_SA:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_SA;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_VLAN_PRI;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field0_data = pSrc->ins.wa_data.set_field_0_data.field_data;

    switch (pSrc->ins.wa_data.set_field_1_data.field_type)
    {
        case OF_IGR_FT2_ACT_SF_TYPE1_NONE:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_NONE;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE1_DA:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_DA;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_VLAN_PRI;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field1_data = pSrc->ins.wa_data.set_field_1_data.field_data;

    switch (pSrc->ins.wa_data.set_field_2_data.field_type)
    {
        case OF_IGR_FT2_ACT_SF_TYPE2_NONE:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_NONE;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_VLAN_ID;

            break;
        case OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_VLAN_PRI;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field2_data = pSrc->ins.wa_data.set_field_2_data.field_data;

    pDest->ins.set_queue = pSrc->ins.wa_data.set_queue;
    pDest->ins.qid = pSrc->ins.wa_data.qid;
    pDest->ins.group = pSrc->ins.wa_data.group;
    pDest->ins.gid = pSrc->ins.wa_data.gid;

    switch (pSrc->ins.wa_data.output_data.type)
    {
        case OF_OUTPUT_TYPE_NONE:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_DISABLE;
                pDest->ins.output_data = 0;
            break;
        case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
                pDest->ins.output_data = (pSrc->ins.wa_data.output_data.devID << 6) |
                                         (pSrc->ins.wa_data.output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_PHY_PORT:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT;
                pDest->ins.output_data = (pSrc->ins.wa_data.output_data.devID << 6) |
                                         (pSrc->ins.wa_data.output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
                pDest->ins.output_data = pSrc->ins.wa_data.output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_TRK_PORT:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT;
                pDest->ins.output_data = pSrc->ins.wa_data.output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT;
                /* portmask translate to portmask index is handled outside */
            break;
        case OF_OUTPUT_TYPE_IN_PORT:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_IN_PORT;
            break;
        case OF_OUTPUT_TYPE_FLOOD:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_FLOOD;
            break;
        case OF_OUTPUT_TYPE_LB:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_LB;
            break;
        case OF_OUTPUT_TYPE_TUNNEL:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_TUNNEL;
                /* convert interface to tunnel index */
                if (_dal_mango_tunnel_intf2tunnelIdx(unit, &pDest->ins.output_data, pSrc->ins.wa_data.output_data.intf) != RT_ERR_OK)
                    return RT_ERR_INPUT;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "intf=0x%x,tunnel_idx=%d", pSrc->ins.wa_data.output_data.intf, pDest->ins.output_data);
            break;
        case OF_OUTPUT_TYPE_FAILOVER:
                pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_FAILOVER;
                /* portmask translate to portmask index is handled outside */
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->ins.metadata = pSrc->ins.wm_data.data;
    pDest->ins.metadata_mask = pSrc->ins.wm_data.mask;
    switch (pSrc->ins.gt_data.type)
    {
        case OF_GOTOTBL_TYPE_NORMAL:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_NORMAL;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id;
            break;
        case OF_GOTOTBL_TYPE_APPLY_AND_LB:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_APPLY_AND_LB;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->ins.tbl_lb_time = pSrc->ins.gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        case OF_GOTOTBL_TYPE_LB:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_LB;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->ins.tbl_lb_time = pSrc->ins.gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* convert dal_mango_of_l3CamEntry_t to rtk_of_l3CamFlowEntry_t */
static int32
_dal_mango_of_l3CamEntryDal2Rtk_convert(uint32 unit, rtk_of_l3CamFlowEntry_t *pDest, dal_mango_of_l3CamEntry_t *pSrc)
{
    int32   ret;
    dal_mango_l3_intfEntry_t l3_intf_entry;
    dal_mango_of_dmacEntry_t dmac_entry;

    osal_memset(pDest, 0x00, sizeof(rtk_of_l3CamFlowEntry_t));
    osal_memset(&l3_intf_entry, 0x00, sizeof(dal_mango_l3_intfEntry_t));

    switch (pSrc->type)
    {
        case MANGO_OF_L3ENTRY_TYPE_SIP:
             if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP;
                 pDest->sip.ipv4 = pSrc->ip0.ipv4;
                 pDest->sip_msk.ipv4 = pSrc->ip0_msk.ipv4;
             }
             else
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP;
                 pDest->sip.ipv6 = pSrc->ip0.ipv6;
                 pDest->sip_msk.ipv6 = pSrc->ip0_msk.ipv6;
             }
             break;
        case MANGO_OF_L3ENTRY_TYPE_DIP:
             if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP4_DIP;
                 pDest->dip.ipv4 = pSrc->ip0.ipv4;
                 pDest->dip_msk.ipv4 = pSrc->ip0_msk.ipv4;
             }
             else
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP6_DIP;
                 pDest->dip.ipv6 = pSrc->ip0.ipv6;
                 pDest->dip_msk.ipv6 = pSrc->ip0_msk.ipv6;
             }
             break;
        case MANGO_OF_L3ENTRY_TYPE_SIP_DIP:
             if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP;
                 pDest->sip.ipv4 = pSrc->ip0.ipv4;
                 pDest->sip_msk.ipv4 = pSrc->ip0_msk.ipv4;
                 pDest->dip.ipv4 = pSrc->ip1.ipv4;
                 pDest->dip_msk.ipv4 = pSrc->ip1_msk.ipv4;
             }
             else
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP;
                 pDest->sip.ipv6 = pSrc->ip0.ipv6;
                 pDest->sip_msk.ipv6 = pSrc->ip0_msk.ipv6;
                 pDest->dip.ipv6 = pSrc->ip1.ipv6;
                 pDest->dip_msk.ipv6 = pSrc->ip1_msk.ipv6;
             }
             break;
        default:
             return RT_ERR_FAILED;
    }

    pDest->metadata = pSrc->metadata;
    pDest->metadata_msk = pSrc->metadata_msk;

    pDest->ins.clearAct_en = pSrc->ins.clearAct_en;
    pDest->ins.writeAct_en = pSrc->ins.writeAct_en;
    pDest->ins.writeMetadata_en = pSrc->ins.writeMd_en;
    pDest->ins.gotoTbl_en = pSrc->ins.gotoTbl_en;

    pDest->ins.wa_data.push_vlan = pSrc->ins.push_vlan;
    pDest->ins.wa_data.push_vlan_data.etherType_idx = pSrc->ins.etherType_idx;
    pDest->ins.wa_data.dec_ip_ttl= pSrc->ins.dec_ip_ttl;

    switch (pSrc->ins.field0_type)
    {
        case MANGO_OF_L3_FIELD_TYPE0_NONE:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
            break;
        case MANGO_OF_L3_FIELD_TYPE0_SA:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
            /* retrieve smac from egress L3 interface table */
            if ((ret = _dal_mango_l3_intfEntry_get(unit, pSrc->ins.field0_data, &l3_intf_entry, \
                            DAL_MANGO_L3_API_FLAG_NONE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            pDest->ins.wa_data.set_field_0_data.mac = l3_intf_entry.egrIntf.smac_addr;
            break;
        case MANGO_OF_L3_FIELD_TYPE0_VLAN_PRI:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI;
            pDest->ins.wa_data.set_field_0_data.field_data = pSrc->ins.field0_data;
            break;
        case MANGO_OF_L3_FIELD_TYPE0_IP_DSCP:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP;
            pDest->ins.wa_data.set_field_0_data.field_data = pSrc->ins.field0_data;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch (pSrc->ins.field1_type)
    {
        case MANGO_OF_L3_FIELD_TYPE1_NONE:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
            break;
        case MANGO_OF_L3_FIELD_TYPE1_DA:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
            /* retrieve dmac from OpenFlow DMAC table */
            if ((ret = _dal_mango_of_dmacEntry_get(unit, pSrc->ins.field1_data, &dmac_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            osal_memcpy(&pDest->ins.wa_data.set_field_1_data.mac, &dmac_entry.mac, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_L3_FIELD_TYPE1_VLAN_PRI:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI;
            pDest->ins.wa_data.set_field_1_data.field_data = pSrc->ins.field1_data;
            break;
        case MANGO_OF_L3_FIELD_TYPE1_IP_DSCP:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP;
            pDest->ins.wa_data.set_field_1_data.field_data = pSrc->ins.field1_data;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch (pSrc->ins.field2_type)
    {
        case MANGO_OF_L3_FIELD_TYPE2_NONE:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
            break;
        case MANGO_OF_L3_FIELD_TYPE2_VLAN_ID:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID;

            break;
        case MANGO_OF_L3_FIELD_TYPE2_VLAN_PRI:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI;
            break;
        case MANGO_OF_L3_FIELD_TYPE2_IP_DSCP:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.wa_data.set_field_2_data.field_data = pSrc->ins.field2_data;

    pDest->ins.wa_data.set_queue = pSrc->ins.set_queue;
    pDest->ins.wa_data.qid = pSrc->ins.qid;
    pDest->ins.wa_data.group = pSrc->ins.group;
    pDest->ins.wa_data.gid = pSrc->ins.gid;

    switch (pSrc->ins.output_type)
    {
        case MANGO_OF_OUTPUT_TYPE_DISABLE:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_NONE;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
            pDest->ins.wa_data.output_data.port = pSrc->ins.output_data & 0x3F;
            pDest->ins.wa_data.output_data.devID = (pSrc->ins.output_data >> 6) & 0xF;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
            pDest->ins.wa_data.output_data.port = pSrc->ins.output_data & 0x3F;
            pDest->ins.wa_data.output_data.devID = (pSrc->ins.output_data >> 6) & 0xF;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
            pDest->ins.wa_data.output_data.trunk = pSrc->ins.output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
            pDest->ins.wa_data.output_data.trunk = pSrc->ins.output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
            /* convert portmask index to portmask */
            if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->ins.output_data, \
                            &pDest->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case MANGO_OF_OUTPUT_TYPE_IN_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
            break;
        case MANGO_OF_OUTPUT_TYPE_FLOOD:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
            break;
        case MANGO_OF_OUTPUT_TYPE_LB:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
            break;
        case MANGO_OF_OUTPUT_TYPE_TUNNEL:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
            /* convert tunnel index to interface */
            _dal_mango_tunnel_tunnelIdx2intf(unit, &pDest->ins.wa_data.output_data.intf, pSrc->ins.output_data);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "tunnel_idx=%d,intf=0x%x", pSrc->ins.output_data, pDest->ins.wa_data.output_data.intf);
            break;
        case MANGO_OF_OUTPUT_TYPE_FAILOVER:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
            /* convert portmask index to portmask */
            if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->ins.output_data, \
                            &pDest->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->ins.wm_data.data = pSrc->ins.metadata;
    pDest->ins.wm_data.mask = pSrc->ins.metadata_mask;
    switch (pSrc->ins.tbl_act)
    {
        case MANGO_OF_GOTOTBL_NORMAL:
            pDest->ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
            break;
        case MANGO_OF_GOTOTBL_APPLY_AND_LB:
            pDest->ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
            break;
        case MANGO_OF_GOTOTBL_LB:
            pDest->ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.gt_data.tbl_id = pSrc->ins.tbl_id + (pSrc->ins.tbl_lb_time * HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit));

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3HashEntry_cmp(uint32 unit, dal_mango_of_l3HashEntry_t *pSrc, dal_mango_of_l3HashEntry_t *pDest)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    if (pSrc->type != pDest->type)
    {
        RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->type=%d,pDest->type=%d",pSrc->type,pDest->type);
        return RT_ERR_FAILED;
    }

    if (pSrc->ip_ver != pDest->ip_ver)
    {
        RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip_ver=%d,pDest->ip_ver=%d",pSrc->ip_ver,pDest->ip_ver);
        return RT_ERR_FAILED;
    }
#if 0
    if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if (memcmp(&pSrc->ip0.ipv4, &pDest->ip0.ipv4, sizeof(rtk_ip_addr_t)))
        {
            RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip0.ipv4=0x%X,pDest->ip0.ipv4=0x%X",pSrc->ip0.ipv4,pDest->ip0.ipv4);
            return RT_ERR_FAILED;
        }

        if ((pSrc->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP) &&
            memcmp(&pSrc->ip1.ipv4, &pDest->ip1.ipv4, sizeof(rtk_ip_addr_t)))
        {
            RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip1.ipv4=0x%X,pDest->ip1.ipv4=0x%X",pSrc->ip1.ipv4,pDest->ip1.ipv4);
            return RT_ERR_FAILED;
        }
    }

    if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        if (memcmp(&pSrc->ip0.ipv6, &pDest->ip0.ipv6, sizeof(rtk_ipv6_addr_t)))
        {
            RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip0.ipv6=0x%X,pDest->ip0.ipv6=0x%X",pSrc->ip0.ipv6,pDest->ip0.ipv6);
            return RT_ERR_FAILED;
        }

        if ((pSrc->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP) &&
            memcmp(&pSrc->ip1.ipv6, &pDest->ip1.ipv6, sizeof(rtk_ipv6_addr_t)))
        {
            RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip1.ipv=0x%X,pDest->ip1.ipv=0x%X",pSrc->ip1.ipv6,pDest->ip1.ipv6);
            return RT_ERR_FAILED;
        }
    }
#endif
    if (memcmp(&pSrc->ip0, &pDest->ip0, sizeof(rtk_ipv6_addr_t)) != 0)
    {
        RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip0=0x%X,pDest->ip0=0x%X",pSrc->ip0,pDest->ip0);
        return RT_ERR_FAILED;
    }

    if (memcmp(&pSrc->ip1, &pDest->ip1, sizeof(rtk_ipv6_addr_t)) != 0)
    {
        RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->ip1=0x%X,pDest->ip1=0x%X",pSrc->ip1,pDest->ip1);
        return RT_ERR_FAILED;
    }

    if (pSrc->md_cmp != pDest->md_cmp)
    {
        RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->md_cmp=%d,pDest->md_cmp=%d",pSrc->md_cmp,pDest->md_cmp);
        return RT_ERR_FAILED;
    }

    if (pSrc->metadata != pDest->metadata)
    {
        RT_LOG(LOG_TRACE, (MOD_DAL|MOD_OPENFLOW), "pSrc->metadata=0x%X,pDest->metadata=0x%X",pSrc->metadata,pDest->metadata);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3HashEntry_getByMethod(
    uint32                      unit,
    dal_mango_of_l3HashEntry_t  *pEntry,
    dal_mango_of_getMethod_t    method,
    dal_mango_of_getResult_t    *pResult)
{
    int32  ret;
    uint32 i, entry_idx, hash_way, block_id;
    dal_mango_of_bucketIdx_t    bkt_idx;
    dal_mango_of_l3HashEntry_t  raw_entry;
    dal_mango_l3_hostAlloc_t    hostAlloc;

    pResult->isFree = FALSE;
    pResult->isExist = FALSE;
    osal_memset((void *)&bkt_idx, 0x00, sizeof(dal_mango_of_bucketIdx_t));
    osal_memset((void *)&raw_entry, 0x00, sizeof(dal_mango_of_l3HashEntry_t));
    osal_memset((void *)&hostAlloc, 0x00, sizeof(dal_mango_l3_hostAlloc_t));

    /* get hash index(s) of SRAM */
    if ((ret = _dal_mango_of_l3_bucket_idx_get(unit, pEntry, &bkt_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (method == MANGO_OF_GETMETHOD_FREE)
    {
        hostAlloc.hashIdx.idx_of_tbl[0] = bkt_idx.bucket0_hash_idx;
        hostAlloc.hashIdx.idx_of_tbl[1] = bkt_idx.bucket1_hash_idx;

        if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
            hostAlloc.width = 2;

        if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6) &&
            (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP))
            hostAlloc.width = 3;

        if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6) &&
            (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP))
            hostAlloc.width = 6;

        if ((ret = _dal_mango_l3_hostEntry_alloc(unit, &hostAlloc, &entry_idx, DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW)) == RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "_dal_mango_l3_hostEntry_alloc entry_idx=%d", entry_idx);
            pResult->isFree = TRUE;
            pResult->idx = entry_idx;
            return RT_ERR_OK;
        }
    }

    if (method == MANGO_OF_GETMETHOD_EXIST)
    {
        hash_way = DAL_MANGO_L3_HOST_TBL_WIDTH;

        for (block_id = 0; block_id < 2; block_id++)
        {
            if (block_id == 0)
                entry_idx = bkt_idx.bucket0_idx;/* Search bucket 0 */
            else
                entry_idx = bkt_idx.bucket1_idx;/* Search bucket 1 */

            for (i = 0; i < hash_way; i++)
            {
                /* an IPv4 entry occuipes two physical index */
                if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4) && ((i%2) != 0))
                    continue;

                /* an IPv6 SIP or DIP entry occuipes three physical index */
                if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6) &&
                    (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP) &&
                    ((i%3) != 0))
                    continue;

                /* an IPv6 SIP+DIP entry occuipes six physical index */
                if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6) &&
                    (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP) &&
                    (i != 0))
                    break;

                ret = _dal_mango_of_l3HashFlowEntry_get(unit, entry_idx+i, &raw_entry);

                if (ret == RT_ERR_ENTRY_INDEX)
                    continue;/* do next search */

                if (ret != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }

                if ((raw_entry.valid == TRUE) &&
                    (raw_entry.fmt == 1) &&
                    (RT_ERR_OK == _dal_mango_of_l3HashEntry_cmp(unit, pEntry, &raw_entry)))
                {
                    pResult->isExist = TRUE;
                    pResult->idx = entry_idx+i;
                    pEntry->ins = raw_entry.ins;
                    pEntry->hit = raw_entry.hit;
                    return RT_ERR_OK;
                }
            }
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3HashFlowEntry_get(uint32 unit, uint32 entry_idx, dal_mango_of_l3HashEntry_t *pEntry)
{
    int32  ret;
    uint32 tblName;
    uint32 ip0_31_0, ip1_31_0;
    uint32 ip0_79_32[2], ip0_127_80[2], ip1_79_32[2], ip1_127_80[2];
    uint32 ip6_u32[4];
    of_l3_hash_entry_t l3_entry;
    uint32 fmt_1_2[9] = {0};

    osal_memset((void *)pEntry, 0x00, sizeof(dal_mango_of_l3HashEntry_t));
    osal_memset((void *)&l3_entry, 0x00, sizeof(of_l3_hash_entry_t));
    tblName = MANGO_FT_L3_HASH_0t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_VALIDtf, &pEntry->valid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* invalid entry */
    if (!pEntry->valid)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pEntry->valid == FALSE");
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_FMTtf, &pEntry->fmt,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* specified index is a traditional L3 entry.
     * Should not happen because caller already offset the traditional entries.
     */
    if (pEntry->fmt == 0)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pEntry->fmt == 0");
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_IP_VERtf, &pEntry->ip_ver,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_TYPEtf, &pEntry->type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* IPv4 entry index must be 0/2/4 */
    if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4) && ((entry_idx%2) != 0))
        return RT_ERR_OK;

    /* IPv6 SIP or DIP entry index must be 0/3 */
    if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6) &&
        (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP) &&
        ((entry_idx%3) != 0))
        return RT_ERR_OK;

    /* IPv6 SIP+DIP entry index must be 0 */
    if ((pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6) &&
        (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP) &&
        ((entry_idx%6) != 0))
        return RT_ERR_OK;

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_IPtf, &ip0_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_MD_CMPtf, &pEntry->md_cmp,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_METADATA_KEYtf, &pEntry->metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* Instrution Part */
    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_INS_GOTO_TBLtf, &pEntry->ins.gotoTbl_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_INS_WRITE_METADATAtf, &pEntry->ins.writeMd_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_INS_WRITE_ACTtf, &pEntry->ins.writeAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_INS_CLR_ACTtf, &pEntry->ins.clearAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_PUSH_VLANtf,\
                                &pEntry->ins.push_vlan, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_VLAN_ETHTYPEtf,\
                                &pEntry->ins.etherType_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_DEC_IP_TTLtf, &pEntry->ins.dec_ip_ttl,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD0_TYPEtf, &pEntry->ins.field0_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD0_DATAtf, \
                                &pEntry->ins.field0_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD1_TYPEtf, &pEntry->ins.field1_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD1_DATAtf, \
                                &pEntry->ins.field1_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD2_TYPEtf, &pEntry->ins.field2_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD2_DATAtf, \
                                &pEntry->ins.field2_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_GOTO_TBL_LB_TIMEtf, \
                                &pEntry->ins.tbl_lb_time, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_Qtf,\
                                &pEntry->ins.set_queue, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_HIT_STStf,\
                                &pEntry->hit, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }


    /* entry N+1 */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        tblName = MANGO_FT_L3_HASH_1t;
    else
        tblName = MANGO_FT_L3_HASH_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+1, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_1_IPtf, &ip1_31_0,\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* table MANGO_FT_L3_HASH_1t format is different from MANGO_FT_L3_HASH_2t.
         * MANGO_FT_L3_HASH_1t has two more fields: MANGO_FT_L3_HASH_1_METADATA_KEYtf and MANGO_FT_L3_HASH_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_HASH_1_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_HASH_1_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_HASH_1_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_HASH_1_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_HASH_1_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_HASH_1_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_HASH_1_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_HASH_1_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_HASH_1_WA_SET_Q_DATAtf;
    }
    else
    {
        if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip0_79_32[0],\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* table MANGO_FT_L3_HASH_1t format is different from MANGO_FT_L3_HASH_2t.
         * MANGO_FT_L3_HASH_1t has two more fields: MANGO_FT_L3_HASH_1_METADATA_KEYtf and MANGO_FT_L3_HASH_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_HASH_2_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_HASH_2_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_HASH_2_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_HASH_2_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_HASH_2_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_HASH_2_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_HASH_2_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_HASH_2_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_HASH_2_WA_SET_Q_DATAtf;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[0], &pEntry->ins.group,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[1], &pEntry->ins.gid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[2], &pEntry->ins.output_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[3], &pEntry->ins.output_data,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[4], &pEntry->ins.metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[5], &pEntry->ins.metadata_mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[6], &pEntry->ins.tbl_act,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[7], &pEntry->ins.tbl_id,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, fmt_1_2[8],\
                                &pEntry->ins.qid, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* IPv4 entry */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        switch (pEntry->type)
        {
            case MANGO_OF_L3ENTRY_TYPE_SIP:
            case MANGO_OF_L3ENTRY_TYPE_DIP:
                 pEntry->ip0.ipv4 = ip0_31_0;
                 break;
            case MANGO_OF_L3ENTRY_TYPE_SIP_DIP:
                 pEntry->ip0.ipv4 = ip0_31_0;
                 pEntry->ip1.ipv4 = ip1_31_0;
                 break;
            default:
                 break;
        }

        return RT_ERR_OK;
    }

    /* entry N+2 */
    tblName = MANGO_FT_L3_HASH_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+2, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip0_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    ip6_u32[0] = ((ip0_127_80[1] & 0x0000FFFF) << 16) | ((ip0_127_80[0] & 0xFFFF0000) >> 16);
    ip6_u32[1] = ((ip0_127_80[0] & 0x0000FFFF) << 16) | (ip0_79_32[1] & 0x0000FFFF);
    ip6_u32[2] = ip0_79_32[0];
    ip6_u32[3] = ip0_31_0;
    DAL_MANGO_IPV6_U32_TO_U8(ip6_u32, pEntry->ip0.ipv6.octet);

    /* IPv6 SIP or DIP entry */
    if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
        return RT_ERR_OK;

    /* entry N+3 */
    tblName = MANGO_FT_L3_HASH_0t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+3, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_0_IPtf, &ip1_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* entry N+4 */
    tblName = MANGO_FT_L3_HASH_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+4, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip1_79_32[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* entry N+5 */
    tblName = MANGO_FT_L3_HASH_2t;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx+5, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip1_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    ip6_u32[0] = ((ip1_127_80[1] & 0x0000FFFF) << 16) | ((ip1_127_80[0] & 0xFFFF0000) >> 16);
    ip6_u32[1] = ((ip1_127_80[0] & 0x0000FFFF) << 16) | (ip1_79_32[1] & 0x0000FFFF);
    ip6_u32[2] = ip1_79_32[0];
    ip6_u32[3] = ip1_31_0;
    DAL_MANGO_IPV6_U32_TO_U8(ip6_u32, pEntry->ip1.ipv6.octet);

    return RT_ERR_OK;
}

static int32
_dal_mango_of_l3HashFlowEntry_set(uint32 unit, uint32 entry_idx, dal_mango_of_l3HashEntry_t *pEntry)
{
    int32  ret;
    uint32 tblName;
    uint32 ip0_31_0, ip1_31_0;
    uint32 ip0_79_32[2], ip0_127_80[2], ip1_79_32[2], ip1_127_80[2];
    of_l3_hash_entry_t l3_entry;
    uint32 rsvd = 0;
    uint32 fmt_1_2[9] = {0};

    osal_memset((void *)&l3_entry, 0x00, sizeof(of_l3_hash_entry_t));
    tblName = MANGO_FT_L3_HASH_0t;

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_VALIDtf, &pEntry->valid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_FMTtf, &pEntry->fmt,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_IP_VERtf, &pEntry->ip_ver,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_TYPEtf, &pEntry->type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        ip0_31_0 = pEntry->ip0.ipv4;
    }
    else
    {
        ip0_31_0 =     ((pEntry->ip0.ipv6.octet[12] << 24) |
                        (pEntry->ip0.ipv6.octet[13] << 16) |
                        (pEntry->ip0.ipv6.octet[14] << 8)  |
                         pEntry->ip0.ipv6.octet[15]);
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_IPtf, &ip0_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_MD_CMPtf, &pEntry->md_cmp,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_METADATA_KEYtf, &pEntry->metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* Instrution Part */
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_INS_GOTO_TBLtf, &pEntry->ins.gotoTbl_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_INS_WRITE_METADATAtf, &pEntry->ins.writeMd_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_INS_WRITE_ACTtf, &pEntry->ins.writeAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_INS_CLR_ACTtf, &pEntry->ins.clearAct_en,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_PUSH_VLANtf,\
                                &pEntry->ins.push_vlan, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_VLAN_ETHTYPEtf,\
                                &pEntry->ins.etherType_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_DEC_IP_TTLtf, &pEntry->ins.dec_ip_ttl,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD0_TYPEtf, &pEntry->ins.field0_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD0_DATAtf, \
                                &pEntry->ins.field0_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD1_TYPEtf, &pEntry->ins.field1_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD1_DATAtf, \
                                &pEntry->ins.field1_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD2_TYPEtf, &pEntry->ins.field2_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_FIELD2_DATAtf, \
                                &pEntry->ins.field2_data, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_GOTO_TBL_LB_TIMEtf, \
                                &pEntry->ins.tbl_lb_time, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_WA_SET_Qtf,\
                                &pEntry->ins.set_queue, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* pEntry->hit = 0 means clear hit bit, pEntry->hit = 1 means keep */
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_HIT_STStf,\
                                &pEntry->hit, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);


    /* entry N+1 */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        tblName = MANGO_FT_L3_HASH_1t;
    else
        tblName = MANGO_FT_L3_HASH_2t;

    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
    {
        if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
        {
            ip1_31_0 = pEntry->ip1.ipv4;

            if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_1_IPtf, &ip1_31_0,\
                                        (uint32 *)&l3_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_1_MD_CMPtf, &rsvd,\
                                        (uint32 *)&l3_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_1_METADATA_KEYtf, &rsvd,\
                                        (uint32 *)&l3_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }

        /* table MANGO_FT_L3_HASH_1t format is different from MANGO_FT_L3_HASH_2t.
         * MANGO_FT_L3_HASH_1t has two more fields: MANGO_FT_L3_HASH_1_METADATA_KEYtf and MANGO_FT_L3_HASH_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_HASH_1_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_HASH_1_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_HASH_1_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_HASH_1_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_HASH_1_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_HASH_1_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_HASH_1_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_HASH_1_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_HASH_1_WA_SET_Q_DATAtf;
    }
    else if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP6)
    {
        ip0_79_32[1]     = (pEntry->ip0.ipv6.octet[6] << 8) | pEntry->ip0.ipv6.octet[7];
        ip0_79_32[0]     = ((pEntry->ip0.ipv6.octet[8] << 24) |
                            (pEntry->ip0.ipv6.octet[9] << 16) |
                            (pEntry->ip0.ipv6.octet[10] << 8) |
                             pEntry->ip0.ipv6.octet[11]);

        if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip0_79_32[0],\
                                    (uint32 *)&l3_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* table MANGO_FT_L3_HASH_1t format is different from MANGO_FT_L3_HASH_2t.
         * MANGO_FT_L3_HASH_1t has two more fields: MANGO_FT_L3_HASH_1_METADATA_KEYtf and MANGO_FT_L3_HASH_1_BMSK_METADATAtf */
        fmt_1_2[0] = MANGO_FT_L3_HASH_2_WA_GROUPtf;
        fmt_1_2[1] = MANGO_FT_L3_HASH_2_WA_GROUP_IDXtf;
        fmt_1_2[2] = MANGO_FT_L3_HASH_2_WA_OUTPUT_TYPEtf;
        fmt_1_2[3] = MANGO_FT_L3_HASH_2_WA_OUTPUT_DATAtf;
        fmt_1_2[4] = MANGO_FT_L3_HASH_2_METADATAtf;
        fmt_1_2[5] = MANGO_FT_L3_HASH_2_METADATA_MSKtf;
        fmt_1_2[6] = MANGO_FT_L3_HASH_2_GOTO_TBL_ACTtf;
        fmt_1_2[7] = MANGO_FT_L3_HASH_2_GOTO_TBL_IDtf;
        fmt_1_2[8] = MANGO_FT_L3_HASH_2_WA_SET_Q_DATAtf;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[0], &pEntry->ins.group,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[1], &pEntry->ins.gid,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[2], &pEntry->ins.output_type,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[3], &pEntry->ins.output_data,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[4], &pEntry->ins.metadata,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[5], &pEntry->ins.metadata_mask,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[6], &pEntry->ins.tbl_act,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[7], &pEntry->ins.tbl_id,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, fmt_1_2[8],\
                                &pEntry->ins.qid, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+1, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* IPv4 entry */
    if (pEntry->ip_ver == MANGO_OF_L3_IPVER_IP4)
        return RT_ERR_OK;

    /* entry N+2 */
    tblName = MANGO_FT_L3_HASH_2t;

    ip0_127_80[1]     = (pEntry->ip0.ipv6.octet[0] << 8) | pEntry->ip0.ipv6.octet[1];
    ip0_127_80[0]     = ((pEntry->ip0.ipv6.octet[2] << 24) |
                        (pEntry->ip0.ipv6.octet[3] << 16) |
                        (pEntry->ip0.ipv6.octet[4] << 8) |
                         pEntry->ip0.ipv6.octet[5]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip0_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+2, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* IPv6 SIP or DIP entry */
    if (pEntry->type == MANGO_OF_L3ENTRY_TYPE_SIP || pEntry->type == MANGO_OF_L3ENTRY_TYPE_DIP)
        return RT_ERR_OK;

    /* entry N+3 */
    tblName = MANGO_FT_L3_HASH_0t;
    ip1_31_0 =     ((pEntry->ip1.ipv6.octet[12] << 24) |
                    (pEntry->ip1.ipv6.octet[13] << 16) |
                    (pEntry->ip1.ipv6.octet[14] << 8) |
                     pEntry->ip1.ipv6.octet[15]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_IPtf, &ip1_31_0,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_MD_CMPtf, &rsvd,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_0_METADATA_KEYtf, &rsvd,\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+3, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* entry N+4 */
    tblName = MANGO_FT_L3_HASH_2t;
    ip1_79_32[1]     = (pEntry->ip1.ipv6.octet[6] << 8) | pEntry->ip1.ipv6.octet[7];
    ip1_79_32[0]     = ((pEntry->ip1.ipv6.octet[8] << 24) |
                        (pEntry->ip1.ipv6.octet[9] << 16) |
                        (pEntry->ip1.ipv6.octet[10] << 8) |
                         pEntry->ip1.ipv6.octet[11]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip1_79_32[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+4, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    /* entry N+5 */
    tblName = MANGO_FT_L3_HASH_2t;
    ip1_127_80[1]     = (pEntry->ip1.ipv6.octet[0] << 8) | pEntry->ip1.ipv6.octet[1];
    ip1_127_80[0]     = ((pEntry->ip1.ipv6.octet[2] << 24) |
                        (pEntry->ip1.ipv6.octet[3] << 16)  |
                        (pEntry->ip1.ipv6.octet[4] << 8)   |
                         pEntry->ip1.ipv6.octet[5]);

    if ((ret = table_field_set(unit, tblName, MANGO_FT_L3_HASH_2_IPtf, &ip1_127_80[0],\
                                (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx+5, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* convert rtk_of_l3HashFlowEntry_t to dal_mango_of_l3HashEntry_t */
static int32
_dal_mango_of_l3HashEntryRtk2Dal_convert(uint32 unit, dal_mango_of_l3HashEntry_t *pDest, rtk_of_l3HashFlowEntry_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(dal_mango_of_l3HashEntry_t));

    pDest->valid = 1;
    pDest->fmt = 1;

    switch (pSrc->type)
    {
        case OF_L3_FLOW_ENTRY_TYPE_IP4_SIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP4;
             pDest->ip0.ipv4 = pSrc->sip.ipv4;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP4_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP4;
             pDest->ip0.ipv4 = pSrc->dip.ipv4;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP4;
             pDest->ip0.ipv4 = pSrc->sip.ipv4;
             pDest->ip1.ipv4 = pSrc->dip.ipv4;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP6_SIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP6;
             pDest->ip0.ipv6 = pSrc->sip.ipv6;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP6_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP6;
             pDest->ip0.ipv6 = pSrc->dip.ipv6;
             break;
        case OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP:
             pDest->type = MANGO_OF_L3ENTRY_TYPE_SIP_DIP;
             pDest->ip_ver = MANGO_OF_L3_IPVER_IP6;
             pDest->ip0.ipv6 = pSrc->sip.ipv6;
             pDest->ip1.ipv6 = pSrc->dip.ipv6;
             break;
        default:
             return RT_ERR_INPUT;
    }

    if (pSrc->flags & RTK_OF_FLAG_FLOWENTRY_MD_CMP)
        pDest->md_cmp = ENABLED;
    pDest->metadata = pSrc->metadata;

    pDest->ins.clearAct_en = pSrc->ins.clearAct_en;
    pDest->ins.writeAct_en = pSrc->ins.writeAct_en;
    pDest->ins.writeMd_en = pSrc->ins.writeMetadata_en;
    pDest->ins.gotoTbl_en = pSrc->ins.gotoTbl_en;

    pDest->ins.push_vlan = pSrc->ins.wa_data.push_vlan;
    pDest->ins.etherType_idx = pSrc->ins.wa_data.push_vlan_data.etherType_idx;
    pDest->ins.dec_ip_ttl= pSrc->ins.wa_data.dec_ip_ttl;

    switch (pSrc->ins.wa_data.set_field_0_data.field_type)
    {
        case OF_IGR_FT2_ACT_SF_TYPE0_NONE:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_NONE;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE0_SA:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_SA;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_VLAN_PRI;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP:
            pDest->ins.field0_type = MANGO_OF_L3_FIELD_TYPE0_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field0_data = pSrc->ins.wa_data.set_field_0_data.field_data;

    switch (pSrc->ins.wa_data.set_field_1_data.field_type)
    {
        case OF_IGR_FT2_ACT_SF_TYPE1_NONE:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_NONE;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE1_DA:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_DA;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_VLAN_PRI;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP:
            pDest->ins.field1_type = MANGO_OF_L3_FIELD_TYPE1_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field1_data = pSrc->ins.wa_data.set_field_1_data.field_data;

    switch (pSrc->ins.wa_data.set_field_2_data.field_type)
    {
        case OF_IGR_FT2_ACT_SF_TYPE2_NONE:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_NONE;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_VLAN_ID;

            break;
        case OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_VLAN_PRI;
            break;
        case OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP:
            pDest->ins.field2_type = MANGO_OF_L3_FIELD_TYPE2_IP_DSCP;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->ins.field2_data = pSrc->ins.wa_data.set_field_2_data.field_data;

    pDest->ins.set_queue = pSrc->ins.wa_data.set_queue;
    pDest->ins.qid = pSrc->ins.wa_data.qid;
    pDest->ins.group = pSrc->ins.wa_data.group;
    pDest->ins.gid = pSrc->ins.wa_data.gid;

    switch (pSrc->ins.wa_data.output_data.type)
    {
        case OF_OUTPUT_TYPE_NONE:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_DISABLE;
            pDest->ins.output_data = 0;
            break;
        case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
            pDest->ins.output_data = (pSrc->ins.wa_data.output_data.devID << 6) |
                                     (pSrc->ins.wa_data.output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_PHY_PORT:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT;
            pDest->ins.output_data = (pSrc->ins.wa_data.output_data.devID << 6) |
                                     (pSrc->ins.wa_data.output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
            pDest->ins.output_data = pSrc->ins.wa_data.output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_TRK_PORT:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT;
            pDest->ins.output_data = pSrc->ins.wa_data.output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT;
            /* portmask translate to portmask index is handled outside */
            break;
        case OF_OUTPUT_TYPE_IN_PORT:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_IN_PORT;
            break;
        case OF_OUTPUT_TYPE_FLOOD:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_FLOOD;
            break;
        case OF_OUTPUT_TYPE_LB:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_LB;
            break;
        case OF_OUTPUT_TYPE_TUNNEL:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_TUNNEL;
            /* convert interface to tunnel index */
            if (_dal_mango_tunnel_intf2tunnelIdx(unit, &pDest->ins.output_data, pSrc->ins.wa_data.output_data.intf) != RT_ERR_OK)
                return RT_ERR_INPUT;
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "intf=0x%x,tunnel_idx=%d", pSrc->ins.wa_data.output_data.intf, pDest->ins.output_data);
            break;
        case OF_OUTPUT_TYPE_FAILOVER:
            pDest->ins.output_type = MANGO_OF_OUTPUT_TYPE_FAILOVER;
            /* portmask translate to portmask index is handled outside */
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->ins.metadata = pSrc->ins.wm_data.data;
    pDest->ins.metadata_mask = pSrc->ins.wm_data.mask;

    switch (pSrc->ins.gt_data.type)
    {
        case OF_GOTOTBL_TYPE_NORMAL:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_NORMAL;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id;
            break;
        case OF_GOTOTBL_TYPE_APPLY_AND_LB:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_APPLY_AND_LB;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->ins.tbl_lb_time = pSrc->ins.gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        case OF_GOTOTBL_TYPE_LB:
            pDest->ins.tbl_act = MANGO_OF_GOTOTBL_LB;
            if (pSrc->ins.gt_data.tbl_id >= HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit))
                return RT_ERR_INPUT;
            pDest->ins.tbl_id = pSrc->ins.gt_data.tbl_id % HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            pDest->ins.tbl_lb_time = pSrc->ins.gt_data.tbl_id / HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* convert dal_mango_of_l3HashEntry_t to rtk_of_l3HashFlowEntry_t */
static int32
_dal_mango_of_l3HashEntryDal2Rtk_convert(uint32 unit, rtk_of_l3HashFlowEntry_t *pDest, dal_mango_of_l3HashEntry_t *pSrc)
{
    int32   ret;
    dal_mango_l3_intfEntry_t l3_intf_entry;
    dal_mango_of_dmacEntry_t dmac_entry;

    osal_memset(pDest, 0x00, sizeof(rtk_of_l3HashFlowEntry_t));
    osal_memset(&l3_intf_entry, 0x00, sizeof(dal_mango_l3_intfEntry_t));

    switch (pSrc->type)
    {
        case MANGO_OF_L3ENTRY_TYPE_SIP:
             if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP;
                 pDest->sip.ipv4 = pSrc->ip0.ipv4;
             }
             else
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP;
                 pDest->sip.ipv6 = pSrc->ip0.ipv6;
             }
             break;
        case MANGO_OF_L3ENTRY_TYPE_DIP:
             if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP4_DIP;
                 pDest->dip.ipv4 = pSrc->ip0.ipv4;
             }
             else
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP6_DIP;
                 pDest->dip.ipv6 = pSrc->ip0.ipv6;
             }
             break;
        case MANGO_OF_L3ENTRY_TYPE_SIP_DIP:
             if (pSrc->ip_ver == MANGO_OF_L3_IPVER_IP4)
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP;
                 pDest->sip.ipv4 = pSrc->ip0.ipv4;
                 pDest->dip.ipv4 = pSrc->ip1.ipv4;
             }
             else
             {
                 pDest->type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP;
                 pDest->sip.ipv6 = pSrc->ip0.ipv6;
                 pDest->dip.ipv6 = pSrc->ip1.ipv6;
             }
             break;
        default:
             return RT_ERR_FAILED;
    }

    if (pSrc->md_cmp == ENABLED)
        pDest->flags |= RTK_OF_FLAG_FLOWENTRY_MD_CMP;
    pDest->metadata = pSrc->metadata;

    pDest->ins.clearAct_en = pSrc->ins.clearAct_en;
    pDest->ins.writeAct_en = pSrc->ins.writeAct_en;
    pDest->ins.writeMetadata_en = pSrc->ins.writeMd_en;
    pDest->ins.gotoTbl_en = pSrc->ins.gotoTbl_en;

    pDest->ins.wa_data.push_vlan = pSrc->ins.push_vlan;
    pDest->ins.wa_data.push_vlan_data.etherType_idx = pSrc->ins.etherType_idx;
    pDest->ins.wa_data.dec_ip_ttl= pSrc->ins.dec_ip_ttl;

    switch (pSrc->ins.field0_type)
    {
        case MANGO_OF_L3_FIELD_TYPE0_NONE:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
            break;
        case MANGO_OF_L3_FIELD_TYPE0_SA:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
            /* retrieve smac from egress L3 interface table */
            if ((ret = _dal_mango_l3_intfEntry_get(unit, pSrc->ins.field0_data, &l3_intf_entry, \
                            DAL_MANGO_L3_API_FLAG_NONE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            pDest->ins.wa_data.set_field_0_data.mac = l3_intf_entry.egrIntf.smac_addr;
            break;
        case MANGO_OF_L3_FIELD_TYPE0_VLAN_PRI:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI;
            pDest->ins.wa_data.set_field_0_data.field_data = pSrc->ins.field0_data;
            break;
        case MANGO_OF_L3_FIELD_TYPE0_IP_DSCP:
            pDest->ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP;
            pDest->ins.wa_data.set_field_0_data.field_data = pSrc->ins.field0_data;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch (pSrc->ins.field1_type)
    {
        case MANGO_OF_L3_FIELD_TYPE1_NONE:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
            break;
        case MANGO_OF_L3_FIELD_TYPE1_DA:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
            /* retrieve dmac from OpenFlow DMAC table */
            if ((ret = _dal_mango_of_dmacEntry_get(unit, pSrc->ins.field1_data, &dmac_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            osal_memcpy(&pDest->ins.wa_data.set_field_1_data.mac, &dmac_entry.mac, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_L3_FIELD_TYPE1_VLAN_PRI:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI;
            pDest->ins.wa_data.set_field_1_data.field_data = pSrc->ins.field1_data;
            break;
        case MANGO_OF_L3_FIELD_TYPE1_IP_DSCP:
            pDest->ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP;
            pDest->ins.wa_data.set_field_1_data.field_data = pSrc->ins.field1_data;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch (pSrc->ins.field2_type)
    {
        case MANGO_OF_L3_FIELD_TYPE2_NONE:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
            break;
        case MANGO_OF_L3_FIELD_TYPE2_VLAN_ID:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID;

            break;
        case MANGO_OF_L3_FIELD_TYPE2_VLAN_PRI:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI;
            break;
        case MANGO_OF_L3_FIELD_TYPE2_IP_DSCP:
            pDest->ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.wa_data.set_field_2_data.field_data = pSrc->ins.field2_data;

    pDest->ins.wa_data.set_queue = pSrc->ins.set_queue;
    pDest->ins.wa_data.qid = pSrc->ins.qid;
    pDest->ins.wa_data.group = pSrc->ins.group;
    pDest->ins.wa_data.gid = pSrc->ins.gid;

    switch (pSrc->ins.output_type)
    {
        case MANGO_OF_OUTPUT_TYPE_DISABLE:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_NONE;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
            pDest->ins.wa_data.output_data.port = pSrc->ins.output_data & 0x3F;
            pDest->ins.wa_data.output_data.devID = (pSrc->ins.output_data >> 6) & 0xF;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
            pDest->ins.wa_data.output_data.port = pSrc->ins.output_data & 0x3F;
            pDest->ins.wa_data.output_data.devID = (pSrc->ins.output_data >> 6) & 0xF;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
                pDest->ins.wa_data.output_data.trunk = pSrc->ins.output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
            pDest->ins.wa_data.output_data.trunk = pSrc->ins.output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
            /* convert portmask index to portmask */
            if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->ins.output_data, \
                            &pDest->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case MANGO_OF_OUTPUT_TYPE_IN_PORT:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
            break;
        case MANGO_OF_OUTPUT_TYPE_FLOOD:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
            break;
        case MANGO_OF_OUTPUT_TYPE_LB:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
            break;
        case MANGO_OF_OUTPUT_TYPE_TUNNEL:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
            /* convert tunnel index to interface */
            _dal_mango_tunnel_tunnelIdx2intf(unit, &pDest->ins.wa_data.output_data.intf, pSrc->ins.output_data);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "tunnel_idx=%d,intf=0x%x", pSrc->ins.output_data, pDest->ins.wa_data.output_data.intf);
            break;
        case MANGO_OF_OUTPUT_TYPE_FAILOVER:
            pDest->ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
            /* convert portmask index to portmask */
            if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->ins.output_data, \
                            &pDest->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->ins.wm_data.data = pSrc->ins.metadata;
    pDest->ins.wm_data.mask = pSrc->ins.metadata_mask;
    switch (pSrc->ins.tbl_act)
    {
        case MANGO_OF_GOTOTBL_NORMAL:
            pDest->ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
            break;
        case MANGO_OF_GOTOTBL_APPLY_AND_LB:
            pDest->ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
            break;
        case MANGO_OF_GOTOTBL_LB:
            pDest->ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->ins.gt_data.tbl_id = pSrc->ins.tbl_id + (pSrc->ins.tbl_lb_time * HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit));

    return RT_ERR_OK;
}

static int32
_dal_mango_of_actBucketResource_alloc(uint32 unit, dal_mango_of_actBucket_t *pRawEntry, rtk_of_actionBucket_t *pRtkEntry)
{
    int32  ret;
    uint32 return_idx;
    rtk_mpls_encap_t mpls_encap_entry;

    if (pRawEntry->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pRawEntry->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pRtkEntry->output_data.portmask), RT_ERR_PORT_MASK);
    }

    /* allocate a L3 interface entry to store the SA */
    if (pRawEntry->field0_type == MANGO_OF_SF_TYPE0_SA)
    {
        if ((ret = _dal_mango_of_l3IntfEgr_set(unit, pRtkEntry->set_field_0_data.mac, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pRawEntry->field0_data = return_idx;
    }

    /* add a OpenFlow DAMC entry to store the DA */
    if (pRawEntry->field1_type == MANGO_OF_SF_TYPE1_DA)
    {
        if ((ret = _dal_mango_of_dmac_set(unit, &pRtkEntry->set_field_1_data.mac, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pRawEntry->field1_data = return_idx;
    }

    /* add a MPLS encap. entry to store the MPLS label */
    if (pRawEntry->field1_type == MANGO_OF_SF_TYPE1_MPLS_LABEL)
    {
        osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
        mpls_encap_entry.label = pRtkEntry->set_field_1_data.field_data;
        mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;
        if ((ret = dal_mango_mpls_encap_create(unit, &mpls_encap_entry, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pRawEntry->field1_data = return_idx;
    }

    /* add a MPLS encap. entry to store the MPLS label */
    if (pRawEntry->field2_type == MANGO_OF_SF_TYPE2_MPLS_LABEL)
    {
        osal_memset(&mpls_encap_entry, 0x00, sizeof(rtk_mpls_encap_t));
        mpls_encap_entry.label = pRtkEntry->set_field_2_data.field_data;
        mpls_encap_entry.tcAct = RTK_MPLS_TC_ASSIGN;
        if ((ret = dal_mango_mpls_encap_create(unit, &mpls_encap_entry, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pRawEntry->field2_data = return_idx;
    }

    /* allocate a portmask entry to store the egress ports */
    if (pRawEntry->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pRawEntry->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        if ((ret = _dal_mango_of_l2PortMaskEntry_set(unit, &pRtkEntry->output_data.portmask, &return_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        pRawEntry->output_data = return_idx;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_actBucketResource_free(uint32 unit, dal_mango_of_actBucket_t *pEntry)
{
    int32  ret;

    /* free L3 interface entry */
    if (pEntry->field0_type == MANGO_OF_SF_TYPE0_SA)
    {
        if ((ret = _dal_mango_l3_intfEntry_free(unit, pEntry->field0_data, RTK_L3_FLAG_NONE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* free OpenFlow DMAC entry */
    if (pEntry->field1_type == MANGO_OF_SF_TYPE1_DA)
    {
        if ((ret = _dal_mango_of_dmacEntry_free(unit, pEntry->field1_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* delete MPLS encap. entry */
    if (pEntry->field1_type == MANGO_OF_SF_TYPE1_MPLS_LABEL)
    {
        if ((ret = dal_mango_mpls_encap_destroy(unit, pEntry->field1_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* delete MPLS encap. entry */
    if (pEntry->field2_type == MANGO_OF_SF_TYPE2_MPLS_LABEL)
    {
        if ((ret = dal_mango_mpls_encap_destroy(unit, pEntry->field2_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* free portmask entry which stored the egress ports */
    if (pEntry->output_type == MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT ||
        pEntry->output_type == MANGO_OF_OUTPUT_TYPE_FAILOVER)
    {
        if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, pEntry->output_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_actBucket_get(uint32 unit, uint32 entry_idx, dal_mango_of_actBucket_t *pEntry)
{
    int32  ret;
    uint32 tblName;
    of_bucket_entry_t bucket_entry;

    osal_memset((void *)pEntry, 0x00, sizeof(dal_mango_of_actBucket_t));
    osal_memset((void *)&bucket_entry, 0x00, sizeof(of_bucket_entry_t));
    tblName = MANGO_ACTION_BUCKETt;

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, tblName, entry_idx, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_COPY_TTL_INWARDtf, &pEntry->cp_ttl_inward,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_POP_VLANtf, &pEntry->pop_vlan,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_POP_MPLStf, &pEntry->pop_mpls,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_POP_MPLS_TYPEtf, &pEntry->pop_mpls_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_PUSH_MPLStf, &pEntry->push_mpls,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_PUSH_MPLS_MODEtf, &pEntry->push_mpls_mode,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_MPLS_VPN_TYPEtf, &pEntry->push_mpls_vpnType,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_MPLS_LIB_IDXtf, &pEntry->push_mpls_libIdx,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_MPLS_ETHTYPEtf,\
                                &pEntry->push_mpls_ethType, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_PUSH_VLANtf,\
                                &pEntry->push_vlan, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_VLAN_ETHTYPEtf,\
                                &pEntry->vlan_ethType_idx, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_COPY_TTL_OUTWARDtf, &pEntry->cp_ttl_outward,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_DEC_MPLS_TTLtf, &pEntry->dec_mpls_ttl,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_DEC_IP_TTLtf, &pEntry->dec_ip_ttl,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD0_TYPEtf, &pEntry->field0_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD0_DATAtf, \
                                &pEntry->field0_data, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD1_TYPEtf, &pEntry->field1_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD1_DATAtf, \
                                &pEntry->field1_data, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD2_TYPEtf, &pEntry->field2_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD2_DATAtf, \
                                &pEntry->field2_data, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_Qtf,\
                                &pEntry->set_queue, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_Q_DATAtf,\
                                &pEntry->qid, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_OUTPUT_TYPEtf, &pEntry->output_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_get(unit, tblName, MANGO_ACTION_BUCKET_WA_OUTPUT_DATAtf, &pEntry->output_data,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_of_actBucket_set(uint32 unit, uint32 entry_idx, dal_mango_of_actBucket_t *pEntry)
{
    int32  ret;
    uint32 tblName;
    of_bucket_entry_t bucket_entry;


    osal_memset((void *)&bucket_entry, 0x00, sizeof(of_bucket_entry_t));
    tblName = MANGO_ACTION_BUCKETt;

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_COPY_TTL_INWARDtf, &pEntry->cp_ttl_inward,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_POP_VLANtf, &pEntry->pop_vlan,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_POP_MPLStf, &pEntry->pop_mpls,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_POP_MPLS_TYPEtf, &pEntry->pop_mpls_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_PUSH_MPLStf, &pEntry->push_mpls,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_PUSH_MPLS_MODEtf, &pEntry->push_mpls_mode,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_MPLS_VPN_TYPEtf, &pEntry->push_mpls_vpnType,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_MPLS_LIB_IDXtf, &pEntry->push_mpls_libIdx,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_MPLS_ETHTYPEtf,\
                                &pEntry->push_mpls_ethType, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_PUSH_VLANtf,\
                                &pEntry->push_vlan, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_VLAN_ETHTYPEtf,\
                                &pEntry->vlan_ethType_idx, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_COPY_TTL_OUTWARDtf, &pEntry->cp_ttl_outward,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_DEC_MPLS_TTLtf, &pEntry->dec_mpls_ttl,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_DEC_IP_TTLtf, &pEntry->dec_ip_ttl,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD0_TYPEtf, &pEntry->field0_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD0_DATAtf, \
                                &pEntry->field0_data, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD1_TYPEtf, &pEntry->field1_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD1_DATAtf, \
                                &pEntry->field1_data, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD2_TYPEtf, &pEntry->field2_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_FIELD2_DATAtf, \
                                &pEntry->field2_data, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_Qtf,\
                                &pEntry->set_queue, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_SET_Q_DATAtf,\
                                &pEntry->qid, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_OUTPUT_TYPEtf, &pEntry->output_type,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, tblName, MANGO_ACTION_BUCKET_WA_OUTPUT_DATAtf, &pEntry->output_data,\
                                (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, tblName, entry_idx, (uint32 *)&bucket_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* convert rtk_of_actionBucket_t to dal_mango_of_actBucket_t */
static int32
_dal_mango_of_actBucketRtk2Dal_convert(uint32 unit, dal_mango_of_actBucket_t *pDest, rtk_of_actionBucket_t *pSrc)
{
    osal_memset(pDest, 0x00, sizeof(dal_mango_of_actBucket_t));

    pDest->cp_ttl_inward = pSrc->cp_ttl_inward;

    pDest->pop_vlan = pSrc->pop_vlan;

    pDest->pop_mpls = pSrc->pop_mpls;
    switch (pSrc->pop_mpls_type)
    {
        case OF_POP_MPLS_TYPE_OUTERMOST_LABEL:
            pDest->pop_mpls_type = MANGO_OF_POP_MPLS_TYPE_OUTERMOST;
            break;
        case OF_POP_MPLS_TYPE_DOUBLE_LABEL:
            pDest->pop_mpls_type = MANGO_OF_POP_MPLS_TYPE_DOUBLE;
            break;
        default:
            return RT_ERR_INPUT;
    }

    pDest->push_mpls = pSrc->push_mpls;
    switch (pSrc->push_mpls_data.push_mode)
    {
        case OF_PUSH_MPLS_MODE_NORMAL:
            pDest->push_mpls_mode = MANGO_OF_PUSH_MPLS_MODE_NORMAL;
            break;
        case OF_PUSH_MPLS_MODE_ENCAPTBL:
            pDest->push_mpls_mode = MANGO_OF_PUSH_MPLS_MODE_ENCAPTBL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (pSrc->push_mpls_data.vpn_type)
    {
        case OF_PUSH_MPLS_VPN_TYPE_L2:
            pDest->push_mpls_vpnType = MANGO_OF_PUSH_MPLS_VPN_TYPE_L2;
            break;
        case OF_PUSH_MPLS_VPN_TYPE_L3:
            pDest->push_mpls_vpnType = MANGO_OF_PUSH_MPLS_VPN_TYPE_L3;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->push_mpls_libIdx = pSrc->push_mpls_data.mpls_encap_idx;
    pDest->push_mpls_ethType = pSrc->push_mpls_data.etherType;

    pDest->push_vlan = pSrc->push_vlan;
    pDest->vlan_ethType_idx = pSrc->push_vlan_data.etherType_idx;

    pDest->cp_ttl_outward = pSrc->cp_ttl_outward;

    pDest->dec_mpls_ttl= pSrc->dec_mpls_ttl;

    pDest->dec_ip_ttl= pSrc->dec_ip_ttl;

    switch (pSrc->set_field_0_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE0_NONE:
            pDest->field0_type = MANGO_OF_SF_TYPE0_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE0_SA:
            pDest->field0_type = MANGO_OF_SF_TYPE0_SA;
            break;
        case OF_IGR_FT0_SF_TYPE0_VLAN_PRI:
            pDest->field0_type = MANGO_OF_SF_TYPE0_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE0_MPLS_TC:
            pDest->field0_type = MANGO_OF_SF_TYPE0_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE0_MPLS_TTL:
            pDest->field0_type = MANGO_OF_SF_TYPE0_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE0_IP_DSCP:
            pDest->field0_type = MANGO_OF_SF_TYPE0_IP_DSCP;
            break;
        case OF_IGR_FT0_SF_TYPE0_IP_TTL:
            pDest->field0_type = MANGO_OF_SF_TYPE0_IP_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE0_IP_FLAG_RSVD:
            pDest->field0_type = MANGO_OF_SF_TYPE0_IP_FLAG_RSVD;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field0_data = pSrc->set_field_0_data.field_data;

    switch (pSrc->set_field_1_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE1_NONE:
            pDest->field1_type = MANGO_OF_SF_TYPE1_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE1_DA:
            pDest->field1_type = MANGO_OF_SF_TYPE1_DA;
            break;
        case OF_IGR_FT0_SF_TYPE1_VLAN_PRI:
            pDest->field1_type = MANGO_OF_SF_TYPE1_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE1_MPLS_LABEL:
            pDest->field1_type = MANGO_OF_SF_TYPE1_MPLS_LABEL;
            break;
        case OF_IGR_FT0_SF_TYPE1_MPLS_TC:
            pDest->field1_type = MANGO_OF_SF_TYPE1_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE1_MPLS_TTL:
            pDest->field1_type = MANGO_OF_SF_TYPE1_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE1_IP_DSCP:
            pDest->field1_type = MANGO_OF_SF_TYPE1_IP_DSCP;
            break;
        case OF_IGR_FT0_SF_TYPE1_IP_TTL:
            pDest->field1_type = MANGO_OF_SF_TYPE1_IP_TTL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field1_data = pSrc->set_field_1_data.field_data;

    switch (pSrc->set_field_2_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE2_NONE:
            pDest->field2_type = MANGO_OF_SF_TYPE2_NONE;
            break;
        case OF_IGR_FT0_SF_TYPE2_VLAN_ID:
            pDest->field2_type = MANGO_OF_SF_TYPE2_VLAN_ID;
            break;
        case OF_IGR_FT0_SF_TYPE2_VLAN_PRI:
            pDest->field2_type = MANGO_OF_SF_TYPE2_VLAN_PRI;
            break;
        case OF_IGR_FT0_SF_TYPE2_MPLS_LABEL:
            pDest->field2_type = MANGO_OF_SF_TYPE2_MPLS_LABEL;
            break;
        case OF_IGR_FT0_SF_TYPE2_MPLS_TC:
            pDest->field2_type = MANGO_OF_SF_TYPE2_MPLS_TC;
            break;
        case OF_IGR_FT0_SF_TYPE2_MPLS_TTL:
            pDest->field2_type = MANGO_OF_SF_TYPE2_MPLS_TTL;
            break;
        case OF_IGR_FT0_SF_TYPE2_IP_DSCP:
            pDest->field2_type = MANGO_OF_SF_TYPE2_IP_DSCP;
            break;
        case OF_IGR_FT0_SF_TYPE2_IP_TTL:
            pDest->field2_type = MANGO_OF_SF_TYPE2_IP_TTL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    pDest->field2_data = pSrc->set_field_2_data.field_data;

    pDest->set_queue = pSrc->set_queue;
    pDest->qid = pSrc->qid;

    switch (pSrc->output_data.type)
    {
        case OF_OUTPUT_TYPE_NONE:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_DISABLE;
                pDest->output_data = 0;
            break;
        case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
                pDest->output_data = (pSrc->output_data.devID << 6) | (pSrc->output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_PHY_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_PHY_PORT;
                pDest->output_data = (pSrc->output_data.devID << 6) | (pSrc->output_data.port & 0x3F);
            break;
        case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
                pDest->output_data = pSrc->output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_TRK_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_TRK_PORT;
                pDest->output_data = pSrc->output_data.trunk;
            break;
        case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT;
            break;
        case OF_OUTPUT_TYPE_IN_PORT:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_IN_PORT;
            break;
        case OF_OUTPUT_TYPE_FLOOD:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_FLOOD;
            break;
        case OF_OUTPUT_TYPE_LB:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_LB;
            break;
        case OF_OUTPUT_TYPE_TUNNEL:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_TUNNEL;
                /* convert interface to tunnel index */
                if (_dal_mango_tunnel_intf2tunnelIdx(unit, &pDest->output_data, pSrc->output_data.intf) != RT_ERR_OK)
                    return RT_ERR_INPUT;
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "intf=0x%x,tunnel_idx=%d", pSrc->output_data.intf, pDest->output_data);
            break;
        case OF_OUTPUT_TYPE_FAILOVER:
                pDest->output_type = MANGO_OF_OUTPUT_TYPE_FAILOVER;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* convert dal_mango_of_actBucket_t to rtk_of_actionBucket_t */
static int32
_dal_mango_of_actBucketDal2Rtk_convert(uint32 unit, rtk_of_actionBucket_t *pDest, dal_mango_of_actBucket_t *pSrc)
{
    int32   ret;
    dal_mango_l3_intfEntry_t l3_intf_entry;
    dal_mango_of_dmacEntry_t dmac_entry;
    rtk_mpls_encap_t         mpls_entry;

    osal_memset(pDest, 0x00, sizeof(rtk_of_actionBucket_t));

    pDest->cp_ttl_inward = pSrc->cp_ttl_inward;

    pDest->pop_vlan = pSrc->pop_vlan;

    pDest->pop_mpls = pSrc->pop_mpls;
    switch (pSrc->pop_mpls_type)
    {
        case MANGO_OF_POP_MPLS_TYPE_OUTERMOST:
            pDest->pop_mpls_type = OF_POP_MPLS_TYPE_OUTERMOST_LABEL;
            break;
        case MANGO_OF_POP_MPLS_TYPE_DOUBLE:
            pDest->pop_mpls_type = OF_POP_MPLS_TYPE_DOUBLE_LABEL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->push_mpls = pSrc->push_mpls;
    switch (pSrc->push_mpls_mode)
    {
        case MANGO_OF_PUSH_MPLS_MODE_NORMAL:
            pDest->push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_NORMAL;
            break;
        case MANGO_OF_PUSH_MPLS_MODE_ENCAPTBL:
            pDest->push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_ENCAPTBL;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (pSrc->push_mpls_vpnType)
    {
        case MANGO_OF_PUSH_MPLS_VPN_TYPE_L2:
            pDest->push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L2;
            break;
        case MANGO_OF_PUSH_MPLS_VPN_TYPE_L3:
            pDest->push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L3;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pDest->push_mpls_data.mpls_encap_idx = pSrc->push_mpls_libIdx;
    pDest->push_mpls_data.etherType = pSrc->push_mpls_ethType;

    pDest->push_vlan = pSrc->push_vlan;
    pDest->push_vlan_data.etherType_idx = pSrc->vlan_ethType_idx;

    pDest->cp_ttl_outward = pSrc->cp_ttl_outward;

    pDest->dec_mpls_ttl= pSrc->dec_mpls_ttl;

    pDest->dec_ip_ttl= pSrc->dec_ip_ttl;

    pDest->set_field_0_data.field_data = pSrc->field0_data;
    switch (pSrc->field0_type)
    {
        case MANGO_OF_SF_TYPE0_NONE:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_NONE;
            break;
        case MANGO_OF_SF_TYPE0_SA:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_SA;
            /* retrieve smac from egress L3 interface table */
            if ((ret = _dal_mango_l3_intfEntry_get(unit, pSrc->field0_data, &l3_intf_entry, \
                            DAL_MANGO_L3_API_FLAG_NONE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            pDest->set_field_0_data.mac = l3_intf_entry.egrIntf.smac_addr;
            break;
        case MANGO_OF_SF_TYPE0_VLAN_PRI:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_VLAN_PRI;
            break;
        case MANGO_OF_SF_TYPE0_MPLS_TC:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_MPLS_TC;
            break;
        case MANGO_OF_SF_TYPE0_MPLS_TTL:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_MPLS_TTL;
            break;
        case MANGO_OF_SF_TYPE0_IP_DSCP:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_DSCP;
            break;
        case MANGO_OF_SF_TYPE0_IP_TTL:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_TTL;
            break;
        case MANGO_OF_SF_TYPE0_IP_FLAG_RSVD:
            pDest->set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_FLAG_RSVD;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->set_field_1_data.field_data = pSrc->field1_data;
    switch (pSrc->field1_type)
    {
        case MANGO_OF_SF_TYPE1_NONE:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_NONE;
            break;
        case MANGO_OF_SF_TYPE1_DA:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_DA;
            /* retrieve dmac from OpenFlow DMAC table */
            if ((ret = _dal_mango_of_dmacEntry_get(unit, pSrc->field1_data, &dmac_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            osal_memcpy(&pDest->set_field_1_data.mac, &dmac_entry.mac, sizeof(rtk_mac_t));
            break;
        case MANGO_OF_SF_TYPE1_VLAN_PRI:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_VLAN_PRI;
            break;
        case MANGO_OF_SF_TYPE1_MPLS_LABEL:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_LABEL;
            /* retrieve MPLS label from MPLS Encap. table */
            if ((ret = dal_mango_mpls_encap_get(unit, pSrc->field1_data, &mpls_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            pDest->set_field_1_data.field_data = mpls_entry.label;
            break;
        case MANGO_OF_SF_TYPE1_MPLS_TC:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_TC;
            break;
        case MANGO_OF_SF_TYPE1_MPLS_TTL:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_TTL;
            break;
        case MANGO_OF_SF_TYPE1_IP_DSCP:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_IP_DSCP;
            break;
        case MANGO_OF_SF_TYPE1_IP_TTL:
            pDest->set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_IP_TTL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->set_field_2_data.field_data = pSrc->field2_data;
    switch (pSrc->field2_type)
    {
        case MANGO_OF_SF_TYPE2_NONE:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_NONE;
            break;
        case MANGO_OF_SF_TYPE2_VLAN_ID:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_VLAN_ID;
            break;
        case MANGO_OF_SF_TYPE2_VLAN_PRI:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_VLAN_PRI;
            break;
        case MANGO_OF_SF_TYPE2_MPLS_LABEL:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_LABEL;
            /* retrieve MPLS label from MPLS Encap. table */
            if ((ret = dal_mango_mpls_encap_get(unit, pSrc->field2_data, &mpls_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            pDest->set_field_2_data.field_data = mpls_entry.label;
            break;
        case MANGO_OF_SF_TYPE2_MPLS_TC:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_TC;
            break;
        case MANGO_OF_SF_TYPE2_MPLS_TTL:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_TTL;
            break;
        case MANGO_OF_SF_TYPE2_IP_DSCP:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_IP_DSCP;
            break;
        case MANGO_OF_SF_TYPE2_IP_TTL:
            pDest->set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_IP_TTL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pDest->set_queue = pSrc->set_queue;
    pDest->qid = pSrc->qid;

    switch (pSrc->output_type)
    {
        case MANGO_OF_OUTPUT_TYPE_DISABLE:
                pDest->output_data.type = DISABLED;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
                pDest->output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
                pDest->output_data.devID = (pSrc->output_data >> 6) & 0xF;
                pDest->output_data.port = pSrc->output_data & 0x3F;
            break;
        case MANGO_OF_OUTPUT_TYPE_PHY_PORT:
                pDest->output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
                pDest->output_data.devID = (pSrc->output_data >> 6) & 0xF;
                pDest->output_data.port = pSrc->output_data & 0x3F;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                pDest->output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
                pDest->output_data.trunk = pSrc->output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_TRK_PORT:
                pDest->output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
                pDest->output_data.trunk = pSrc->output_data;
            break;
        case MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT:
                pDest->output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
                /* convert portmask index to portmask */
                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->output_data, \
                                &pDest->output_data.portmask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            break;
        case MANGO_OF_OUTPUT_TYPE_IN_PORT:
                pDest->output_data.type = OF_OUTPUT_TYPE_IN_PORT;
            break;
        case MANGO_OF_OUTPUT_TYPE_FLOOD:
                pDest->output_data.type = OF_OUTPUT_TYPE_FLOOD;
            break;
        case MANGO_OF_OUTPUT_TYPE_LB:
                pDest->output_data.type = OF_OUTPUT_TYPE_LB;
            break;
        case MANGO_OF_OUTPUT_TYPE_TUNNEL:
                pDest->output_data.type = OF_OUTPUT_TYPE_TUNNEL;
            /* convert tunnel index to interface */
            _dal_mango_tunnel_tunnelIdx2intf(unit, &pDest->output_data.intf, pSrc->output_data);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "tunnel_idx=%d,intf=0x%x", pSrc->output_data, pDest->output_data.intf);
            break;
        case MANGO_OF_OUTPUT_TYPE_FAILOVER:
                pDest->output_data.type = OF_OUTPUT_TYPE_FAILOVER;
                /* convert portmask index to portmask */
                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pSrc->output_data, \
                                &pDest->output_data.portmask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_ofMapper_init
 * Description:
 *      Hook of module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook of module before calling any of APIs.
 */
int32
dal_mango_ofMapper_init(dal_mapper_t *pMapper)
{
    pMapper->of_init = dal_mango_of_init;
    pMapper->of_classifier_get = dal_mango_of_classifier_get;
    pMapper->of_classifier_set = dal_mango_of_classifier_set;
    pMapper->of_flowMatchFieldSize_get = dal_mango_of_flowMatchFieldSize_get;
    pMapper->of_flowEntrySize_get = dal_mango_of_flowEntrySize_get;
    pMapper->of_flowEntryValidate_get = dal_mango_of_flowEntryValidate_get;
    pMapper->of_flowEntryValidate_set = dal_mango_of_flowEntryValidate_set;
    pMapper->of_flowEntryFieldList_get = dal_mango_of_flowEntryFieldList_get;
    pMapper->of_flowEntryField_check = dal_mango_of_flowEntryField_check;
    pMapper->of_flowEntryField_read = dal_mango_of_flowEntryField_read;
    pMapper->of_flowEntryField_write = dal_mango_of_flowEntryField_write;
    pMapper->of_flowEntryOperation_get = dal_mango_of_flowEntryOperation_get;
    pMapper->of_flowEntryOperation_set = dal_mango_of_flowEntryOperation_set;
    pMapper->of_flowEntryInstruction_get = dal_mango_of_flowEntryInstruction_get;
    pMapper->of_flowEntryInstruction_set = dal_mango_of_flowEntryInstruction_set;
    pMapper->of_flowEntrySetField_check = dal_mango_of_flowEntrySetField_check;
    pMapper->of_flowEntryHitSts_get = dal_mango_of_flowEntryHitSts_get;
    pMapper->of_flowEntry_del = dal_mango_of_flowEntry_del;
    pMapper->of_flowEntry_move = dal_mango_of_flowEntry_move;
    pMapper->of_ftTemplateSelector_get = dal_mango_of_ftTemplateSelector_get;
    pMapper->of_ftTemplateSelector_set = dal_mango_of_ftTemplateSelector_set;
    pMapper->of_flowCntMode_get = dal_mango_of_flowCntMode_get;
    pMapper->of_flowCntMode_set = dal_mango_of_flowCntMode_set;
    pMapper->of_flowCnt_get = dal_mango_of_flowCnt_get;
    pMapper->of_flowCnt_clear = dal_mango_of_flowCnt_clear;
    pMapper->of_flowCntThresh_get = dal_mango_of_flowCntThresh_get;
    pMapper->of_flowCntThresh_set = dal_mango_of_flowCntThresh_set;
    pMapper->of_ttlExcpt_get = dal_mango_of_ttlExcpt_get;
    pMapper->of_ttlExcpt_set = dal_mango_of_ttlExcpt_set;
    pMapper->of_maxLoopback_get = dal_mango_of_maxLoopback_get;
    pMapper->of_maxLoopback_set = dal_mango_of_maxLoopback_set;
    pMapper->of_l2FlowTblMatchField_get = dal_mango_of_l2FlowTblMatchField_get;
    pMapper->of_l2FlowTblMatchField_set = dal_mango_of_l2FlowTblMatchField_set;
    pMapper->of_l2FlowEntrySetField_check = dal_mango_of_l2FlowEntrySetField_check;
    pMapper->of_l2FlowEntry_get = dal_mango_of_l2FlowEntry_get;
    pMapper->of_l2FlowEntryNextValid_get = dal_mango_of_l2FlowEntryNextValid_get;
    pMapper->of_l2FlowEntry_add = dal_mango_of_l2FlowEntry_add;
    pMapper->of_l2FlowEntry_del = dal_mango_of_l2FlowEntry_del;
    pMapper->of_l2FlowEntry_delAll = dal_mango_of_l2FlowEntry_delAll;
    pMapper->of_l2FlowEntryHitSts_get = dal_mango_of_l2FlowEntryHitSts_get;
    pMapper->of_l2FlowTblHashAlgo_get = dal_mango_of_l2FlowTblHashAlgo_get;
    pMapper->of_l2FlowTblHashAlgo_set = dal_mango_of_l2FlowTblHashAlgo_set;
    pMapper->of_l3FlowTblPri_get = dal_mango_of_l3FlowTblPri_get;
    pMapper->of_l3FlowTblPri_set = dal_mango_of_l3FlowTblPri_set;
    pMapper->of_l3CamFlowTblMatchField_get = dal_mango_of_l3CamFlowTblMatchField_get;
    pMapper->of_l3CamFlowTblMatchField_set = dal_mango_of_l3CamFlowTblMatchField_set;
    pMapper->of_l3HashFlowTblMatchField_get = dal_mango_of_l3HashFlowTblMatchField_get;
    pMapper->of_l3HashFlowTblMatchField_set = dal_mango_of_l3HashFlowTblMatchField_set;
    pMapper->of_l3HashFlowTblHashAlgo_get = dal_mango_of_l3HashFlowTblHashAlgo_get;
    pMapper->of_l3HashFlowTblHashAlgo_set = dal_mango_of_l3HashFlowTblHashAlgo_set;
    pMapper->of_l3FlowEntrySetField_check = dal_mango_of_l3FlowEntrySetField_check;
    pMapper->of_l3CamFlowEntry_get = dal_mango_of_l3CamFlowEntry_get;
    pMapper->of_l3CamFlowEntry_add = dal_mango_of_l3CamFlowEntry_add;
    pMapper->of_l3CamFlowEntry_del = dal_mango_of_l3CamFlowEntry_del;
    pMapper->of_l3CamFlowEntry_move = dal_mango_of_l3CamFlowEntry_move;
    pMapper->of_l3CamflowEntryHitSts_get = dal_mango_of_l3CamflowEntryHitSts_get;
    pMapper->of_l3HashFlowEntry_get = dal_mango_of_l3HashFlowEntry_get;
    pMapper->of_l3HashFlowEntryNextValid_get = dal_mango_of_l3HashFlowEntryNextValid_get;
    pMapper->of_l3HashFlowEntry_add = dal_mango_of_l3HashFlowEntry_add;
    pMapper->of_l3HashFlowEntry_del = dal_mango_of_l3HashFlowEntry_del;
    pMapper->of_l3HashFlowEntry_delAll = dal_mango_of_l3HashFlowEntry_delAll;
    pMapper->of_l3HashflowEntryHitSts_get = dal_mango_of_l3HashflowEntryHitSts_get;
    pMapper->of_groupEntry_get = dal_mango_of_groupEntry_get;
    pMapper->of_groupEntry_set = dal_mango_of_groupEntry_set;
    pMapper->of_groupTblHashPara_get = dal_mango_of_groupTblHashPara_get;
    pMapper->of_groupTblHashPara_set = dal_mango_of_groupTblHashPara_set;
    pMapper->of_actionBucket_get = dal_mango_of_actionBucket_get;
    pMapper->of_actionBucket_set = dal_mango_of_actionBucket_set;
    pMapper->of_trapTarget_get = dal_mango_of_trapTarget_get;
    pMapper->of_trapTarget_set = dal_mango_of_trapTarget_set;
    pMapper->of_tblMissAction_get = dal_mango_of_tblMissAction_get;
    pMapper->of_tblMissAction_set = dal_mango_of_tblMissAction_set;
    pMapper->of_flowTblCnt_get = dal_mango_of_flowTblCnt_get;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_of_init
 * Description:
 *      Initialize OpenFlow module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize OpenFlow module before calling any OpenFlow APIs.
 */
int32
dal_mango_of_init(uint32 unit)
{
    uint32  tblShadowSize;
    int32   ret;

    RT_INIT_REENTRY_CHK(of_init[unit]);
    of_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    of_sem[unit] = osal_sem_mutex_create();
    if (0 == of_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_OPENFLOW), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    of_init[unit] = INIT_COMPLETED;

    osal_memset(&_ofDb[unit], 0x00, sizeof(dal_mango_of_drvDb_t));

    if ((ret = table_size_get(unit, MANGO_FT_L2_HASH_FMT0_0t, &l2Sram_size[unit])) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_size_get(unit, MANGO_FT_L2_CAM_FMT0_0t, &l2Cam_size[unit])) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_size_get(unit, MANGO_FT_L3_HASH_0t, &l3Sram_size[unit])) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    tblShadowSize = sizeof(of_igr_entry_t) * HAL_MAX_NUM_OF_PIE_FILTER_ID(unit);
    pOF_TBL = osal_alloc(tblShadowSize);
    if (!pOF_TBL)
    {
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_DAL|MOD_OPENFLOW), "alloc memory fail");
        return RT_ERR_MEM_ALLOC;
    }

    osal_memset(pOF_TBL, 0, tblShadowSize);

    return RT_ERR_OK;
}   /* end of dal_mango_of_init */

/* Function Name:
 *      dal_mango_of_classifier_get
 * Description:
 *      Get OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 * Output:
 *      pData 	- classifier data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_INPUT    	- invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 */
int32
dal_mango_of_classifier_get(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t *pData)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,type=%d,port_vlan=%d", unit, type, pData->port);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_CLASSIFIER_TYPE_END <= type), RT_ERR_INPUT);
    /*RT_PARAM_CHK((OF_CLASSIFIER_TYPE_PORT == type) && !HWP_ETHER_PORT(unit, pData->port), RT_ERR_INPUT);*/
    RT_PARAM_CHK(((OF_CLASSIFIER_TYPE_VLAN == type) || (OF_CLASSIFIER_TYPE_VLAN_AND_PORT == type)) &&
                 (pData->vlan > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    /* function body */
    OF_SEM_LOCK(unit);

    switch (type)
    {
        case OF_CLASSIFIER_TYPE_PORT:
            if ((ret = reg_array_field_read(unit, MANGO_OF_PORT_CTRLr, pData->port, REG_ARRAY_INDEX_NONE,\
                MANGO_ENf, &pData->enable)) != RT_ERR_OK)
            {
                OF_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case OF_CLASSIFIER_TYPE_VLAN:
            if ((ret = reg_array_field_read(unit, MANGO_OF_VLAN_ENr, REG_ARRAY_INDEX_NONE, pData->vlan,\
                MANGO_ENf, &pData->enable)) != RT_ERR_OK)
            {
                OF_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case OF_CLASSIFIER_TYPE_VLAN_AND_PORT:
            if ((ret = reg_array_field_read(unit, MANGO_OF_VLAN_AND_PORT_ENr, REG_ARRAY_INDEX_NONE, pData->vlan,\
                MANGO_ENf, &pData->enable)) != RT_ERR_OK)
            {
                OF_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_classifier_get */

/* Function Name:
 *      dal_mango_of_classifier_set
 * Description:
 *      Set OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 *      data 	- classifier data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 */
int32
dal_mango_of_classifier_set(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t data)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,type=%d,data=%d", unit, type, data);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((OF_CLASSIFIER_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((data.enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    /*RT_PARAM_CHK((OF_CLASSIFIER_TYPE_PORT == type) && !HWP_PORT_ETH(unit, data.port), RT_ERR_INPUT);*/
    RT_PARAM_CHK(((OF_CLASSIFIER_TYPE_VLAN == type) || (OF_CLASSIFIER_TYPE_VLAN_AND_PORT == type)) &&
                 (data.vlan > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    /* function body */
    OF_SEM_LOCK(unit);

    switch (type)
    {
        case OF_CLASSIFIER_TYPE_PORT:
            if ((ret = reg_array_field_write(unit, MANGO_OF_PORT_CTRLr, data.port, REG_ARRAY_INDEX_NONE,\
                MANGO_ENf, &data.enable)) != RT_ERR_OK)
            {
                OF_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case OF_CLASSIFIER_TYPE_VLAN:
            if ((ret = reg_array_field_write(unit, MANGO_OF_VLAN_ENr, REG_ARRAY_INDEX_NONE, data.vlan,\
                MANGO_ENf, &data.enable)) != RT_ERR_OK)
            {
                OF_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case OF_CLASSIFIER_TYPE_VLAN_AND_PORT:
            if ((ret = reg_array_field_write(unit, MANGO_OF_VLAN_AND_PORT_ENr, REG_ARRAY_INDEX_NONE, data.vlan,\
                MANGO_ENf, &data.enable)) != RT_ERR_OK)
            {
                OF_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_classifier_set */

/* Function Name:
 *      dal_mango_of_flowMatchFieldSize_get
 * Description:
 *      Get the match field size of flow entry.
 * Input:
 *      unit        - unit id
 *      type        - type of entry field
 * Output:
 *      pField_size - match field size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The unit of size is bit.
 */
int32
dal_mango_of_flowMatchFieldSize_get(uint32 unit, rtk_of_matchfieldType_t type, uint32 *pField_size)
{
    uint32  i, j, fieldNumber;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((MATCH_FIELD_END <= type), RT_ERR_OF_FIELD_TYPE);
    RT_PARAM_CHK((NULL == pField_size), RT_ERR_NULL_POINTER);

    /* search fixed field type */
    for (i = 0; dal_mango_of_fixField_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_fixField_list[i].type)
        {
            *pField_size = dal_mango_of_fixField_list[i].pField[0].field_length;
            return RT_ERR_OK;
        }
    }

    /* search non-fixed field type */
    for (i = 0; dal_mango_of_field_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_field_list[i].type)
        {
            fieldNumber = dal_mango_of_field_list[i].fieldNumber;
            *pField_size = 0;
            for (j = 0; j < fieldNumber; ++j)
            {
                *pField_size += dal_mango_of_field_list[i].pField[j].field_length;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_OF_FIELD_TYPE;
}   /* end of dal_mango_of_flowMatchFieldSize_get */

/* Function Name:
 *      dal_mango_of_flowEntrySize_get
 * Description:
 *      Get the flow entry size
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 * Output:
 *      pEntry_size - flow entry size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The unit of size is byte.
 */
int32
dal_mango_of_flowEntrySize_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 *pEntry_size)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,phase=%d", unit, phase);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pEntry_size), RT_ERR_NULL_POINTER);

    *pEntry_size = (DAL_MANGO_OF_TMP_FIELD_TOTAL_SIZE + DAL_MANGO_OF_FIXED_FIELD_SIZE) * 2;/* multiply 2 for bitmask */

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntrySize_get */

/* Function Name:
 *      dal_mango_of_flowEntryValidate_get
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pValid    - pointer buffer of valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryValidate_get(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		*pValid)
{
    int32       ret;
    uint32      table, field;
    uint32      phy_entry_idx;
    of_igr_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);

    /* function body */
    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            field = MANGO_FT_IGR_VALIDtf;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            field = MANGO_FT_EGR_VALIDtf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, table, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, table, field, pValid, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pValid=%d", *pValid);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryValidate_get */

/* Function Name:
 *      dal_mango_of_flowEntryValidate_set
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      valid     - valid state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    	- The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryValidate_set(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		valid)
{
    int32       ret;
    uint32      table, field;
    uint32      phy_entry_idx;
    of_igr_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, valid=%d", unit, phase, entry_idx, valid);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((valid != 0) && (valid != 1), RT_ERR_INPUT);

    /* function body */
    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            field = MANGO_FT_IGR_VALIDtf;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            field = MANGO_FT_EGR_VALIDtf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    OF_SEM_LOCK(unit);

    if ((ret = OF_TBL_READ(unit, table, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, table, field, &valid, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = OF_TBL_WRITE(unit, table, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryValidate_set */

/* Function Name:
 *      dal_mango_of_flowEntryFieldList_get
 * Description:
 *      Get supported match field list of the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OF_FT_PHASE    - invalid Flow Table phase
 * Note:
 *      None
 */
int32
dal_mango_of_flowEntryFieldList_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldList_t  *list)
{
    uint32  i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,phase=%d", unit, phase);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);

    osal_memset((void *)list, 0x00, sizeof(rtk_of_matchfieldList_t));

    if (phase == FT_PHASE_IGR_FT_1)
    {
        BITMAP_SET(list->field_bmp, MATCH_FIELD_OTAG_VID);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_ETH_SRC);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_ETH_DST);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_IP_PROTO);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_IPV4_SRC);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_IPV4_DST);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_L4_SRC_PORT);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_L4_DST_PORT);
    }
    else if (phase == FT_PHASE_IGR_FT_2)
    {
        BITMAP_SET(list->field_bmp, MATCH_FIELD_METADATA);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_IPV4_SRC);
        BITMAP_SET(list->field_bmp, MATCH_FIELD_IPV4_DST);
    }
    else
    {
        /* set fixed field type to list */
        for (i = 0; dal_mango_of_fixField_list[i].type != MATCH_FIELD_END; ++i)
        {
            BITMAP_SET(list->field_bmp, dal_mango_of_fixField_list[i].type);
        }

        /* set non-fixed field type to list */
        for (i = 0; dal_mango_of_field_list[i].type != MATCH_FIELD_END; ++i)
        {
            if(dal_mango_of_field_list[i].valid_phase & (1 << phase))
            {
                BITMAP_SET(list->field_bmp, dal_mango_of_field_list[i].type);
            }
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "list.field_bmp[0]=0x%X", list->field_bmp[0]);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "list.field_bmp[1]=0x%X", list->field_bmp[1]);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "list.field_bmp[2]=0x%X", list->field_bmp[2]);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryFieldList_get */

/* Function Name:
 *      dal_mango_of_flowEntryField_check
 * Description:
 *      Check whether the match field type is supported on the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OF_FT_PHASE    - invalid Flow Table phase
 *      RT_ERR_OF_FIELD_TYPE  - invalid match field type
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldType_t  type)
{
    uint32  i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,phase=%d,type=%d", unit, phase, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((MATCH_FIELD_END <= type), RT_ERR_OF_FIELD_TYPE);

    /* search fixed field type */
    for (i = 0; dal_mango_of_fixField_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_fixField_list[i].type)
            return RT_ERR_OK;
    }

    /* search non-fixed field type */
    for (i = 0; dal_mango_of_field_list[i].type != MATCH_FIELD_END; ++i)
    {
        if (type == dal_mango_of_field_list[i].type)
        {
            if(dal_mango_of_field_list[i].valid_phase & (1 << phase))
                return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}   /* end of dal_mango_of_flowEntryField_check */

/* Function Name:
 *      dal_mango_of_flowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified phase and field ID.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,phase=%d,field_id=%d,type=%d", unit, phase, field_id, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    if ((ret = _dal_mango_of_flowEntrySetField_check(unit, phase, field_id, type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntrySetField_check */

/* Function Name:
 *      dal_mango_of_flowEntryField_read
 * Description:
 *      Read match field data from specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 * Output:
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE		- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryField_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask)
{
    int32       ret;
    uint32      table, phy_entry_idx;
    of_igr_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((MATCH_FIELD_END <= type), RT_ERR_OF_FIELD_TYPE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, table, phy_entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = _dal_mango_of_ruleEntryBufField_get(unit, phase,
            phy_entry_idx, type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryField_read */

/* Function Name:
 *      dal_mango_of_flowEntryField_write
 * Description:
 *      Write match field data to specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryField_write(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask)
{
    int32       ret;
    uint32      table, phy_entry_idx;
    of_igr_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((MATCH_FIELD_END <= type), RT_ERR_OF_FIELD_TYPE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, table, phy_entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = _dal_mango_of_ruleEntryBufField_set(unit, phase,
            phy_entry_idx, type, (uint32 *)&entry, pData, pMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_WRITE(unit, table, phy_entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryField_write */

/* Function Name:
 *      dal_mango_of_flowEntryOperation_get
 * Description:
 *      Get flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 * Output:
 *      pOperation - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 *      (6) Egress flow table 0 doesn't support aggr_1 operation.
 */
int32
dal_mango_of_flowEntryOperation_get(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation)
{
    int32       ret;
    uint32      table, fieldReverse, fieldAggr1 = 0, fieldAggr2, phy_entry_idx;
    of_igr_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table        = MANGO_FT_IGRt;
            fieldReverse = MANGO_FT_IGR_REVERSEtf;
            fieldAggr1   = MANGO_FT_IGR_AGGR_1tf;
            fieldAggr2   = MANGO_FT_IGR_AGGR_2tf;
            break;
        case FT_PHASE_EGR_FT_0:
            table        = MANGO_FT_EGRt;
            fieldReverse = MANGO_FT_EGR_REVERSEtf;
            fieldAggr2   = MANGO_FT_EGR_AGGR_2tf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, table, phy_entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, table, fieldReverse, &pOperation->reverse, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (FT_PHASE_EGR_FT_0 != phase)
    {
        if ((ret = table_field_get(unit, table, fieldAggr1, &pOperation->aggr_1, (uint32 *)&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if ((ret = table_field_get(unit, table, fieldAggr2, &pOperation->aggr_2, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryOperation_get */

/* Function Name:
 *      dal_mango_of_flowEntryOperation_set
 * Description:
 *      Set flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 *      pOperation - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 *      (6) Egress flow table 0 doesn't support aggr_1 operation.
 */
int32
dal_mango_of_flowEntryOperation_set(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation)
{
    int32       ret;
    uint32      table, fieldReverse, fieldAggr1 = 0, fieldAggr2, phy_entry_idx;
    of_igr_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pOperation), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pOperation->reverse), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pOperation->aggr_1), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pOperation->aggr_2), RT_ERR_INPUT);
    RT_PARAM_CHK((pOperation->aggr_1 == ENABLED) && (FT_PHASE_EGR_FT_0 == phase), RT_ERR_INPUT);
    RT_PARAM_CHK(((pOperation->aggr_1 == ENABLED) && (entry_idx % 2 != 0)), RT_ERR_ENTRY_INDEX);

    if (pOperation->aggr_2 == ENABLED &&
        (entry_idx%2 != 0 || (entry_idx/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 != 0))
    {
        return RT_ERR_ENTRY_INDEX;
    }

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table        = MANGO_FT_IGRt;
            fieldReverse = MANGO_FT_IGR_REVERSEtf;
            fieldAggr1   = MANGO_FT_IGR_AGGR_1tf;
            fieldAggr2   = MANGO_FT_IGR_AGGR_2tf;
            break;
        case FT_PHASE_EGR_FT_0:
            table        = MANGO_FT_EGRt;
            fieldReverse = MANGO_FT_EGR_REVERSEtf;
            fieldAggr2   = MANGO_FT_EGR_AGGR_2tf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_READ(unit, table, phy_entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_set(unit, table, fieldReverse, &pOperation->reverse, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (FT_PHASE_EGR_FT_0 != phase)
    {
        if ((ret = table_field_set(unit, table, fieldAggr1, &pOperation->aggr_1, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    if ((ret = table_field_set(unit, table, fieldAggr2, &pOperation->aggr_2, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = OF_TBL_WRITE(unit, table, phy_entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryOperation_set */

/* Function Name:
 *      dal_mango_of_flowEntryInstruction_get
 * Description:
 *      Get the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 * Output:
 *      pAction   - instruction mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryInstruction_get(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData)
{
    int32   ret;
    uint32  phy_entry_idx;
    dal_mango_of_igrFtIns_t igrIns;
    dal_mango_of_egrFtIns_t egrIns;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,	phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            ret = _dal_mango_of_igrFTInstruct_get(unit, phy_entry_idx, &igrIns);
            break;
        case FT_PHASE_EGR_FT_0:
            ret = _dal_mango_of_egrFTInstruct_get(unit, phy_entry_idx, &egrIns);
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            if ((ret = _dal_mango_of_igrFTDal2Rtk_convert(unit, &pData->igrFt_0, &igrIns)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case FT_PHASE_IGR_FT_3:
            if ((ret = _dal_mango_of_igrFTDal2Rtk_convert(unit, &pData->igrFt_3, &igrIns)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        case FT_PHASE_EGR_FT_0:
            if ((ret = _dal_mango_of_egrFTDal2Rtk_convert(unit, &pData->egrFt_0, &egrIns)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryInstruction_get */

/* Function Name:
 *      dal_mango_of_flowEntryInstruction_set
 * Description:
 *      Set the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      pData     - instruction mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_flowEntryInstruction_set(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData)
{
    int32   ret;
    uint32  phy_entry_idx;
    dal_mango_of_igrFtIns_t igrIns, orgIgrIns;
    dal_mango_of_egrFtIns_t egrIns;
    rtk_of_igrFT0Ins_t *pIgrInsRtk = NULL;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,	phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    /* convert RTK to DAL structure */
    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            if ((ret = _dal_mango_of_igrFTRtk2Dal_convert(unit, &igrIns, &pData->igrFt_0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            pIgrInsRtk = &pData->igrFt_0;
            break;
        case FT_PHASE_IGR_FT_3:
            if ((ret = _dal_mango_of_igrFTRtk2Dal_convert(unit, &igrIns, &pData->igrFt_3)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            pIgrInsRtk = &pData->igrFt_3;
            break;
        case FT_PHASE_EGR_FT_0:
            if ((ret = _dal_mango_of_egrFTRtk2Dal_convert(unit, &egrIns, &pData->egrFt_0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    if (phase == FT_PHASE_IGR_FT_0 || phase == FT_PHASE_IGR_FT_3)
    {
        ret = _dal_mango_of_igrFTInstruct_get(unit, phy_entry_idx, &orgIgrIns);

        /* free allocated resources for the set entry */
        if ((ret = _dal_mango_of_flowEntryResource_free(unit, &orgIgrIns)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* allocated new resources for the set entry */
        if ((ret = _dal_mango_of_flowEntryResource_alloc(unit, &igrIns, pIgrInsRtk)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* set the raw data to the chip */
    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            ret = _dal_mango_of_igrFTInstruct_set(unit, phy_entry_idx, &igrIns);
            break;
        case FT_PHASE_IGR_FT_3:
            ret = _dal_mango_of_igrFTInstruct_set(unit, phy_entry_idx, &igrIns);
            break;
        case FT_PHASE_EGR_FT_0:
            ret = _dal_mango_of_egrFTInstruct_set(unit, phy_entry_idx, &egrIns);
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryInstruction_set */

/* Function Name:
 *      dal_mango_of_flowEntryHitSts_get
 * Description:
 *      Get the flow entry hit status.
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The hit status can be cleared by configuring parameter 'reset' to (1)
 */
int32
dal_mango_of_flowEntryHitSts_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint32                   reset,
    uint32                   *pIsHit)
{
    int32   ret;
    uint32  phy_entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, reset=%d", unit, phase, entry_idx, reset);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((2 <= reset), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    if ((ret = _dal_mango_pie_entryHitSts_get(unit, phy_entry_idx, reset, pIsHit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pIsHit=%d", *pIsHit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryHitSts_get */

static int32
_dal_mango_of_tblEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 entryIdx)
{
    of_igr_entry_t  ofEntry;
    log_entry_t     cntrEntry;
    uint32          table, cntEntryIdx;
    uint32          hitSts;
    int32           ret;

    OF_DEL_DBG("%s entryIdx %u\n", __func__, entryIdx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    osal_memset(&ofEntry, 0, sizeof(of_igr_entry_t));
    osal_memset(&cntrEntry, 0, sizeof(log_entry_t));

    RT_ERR_CHK(OF_TBL_WRITE(unit, table, entryIdx, (uint32 *)&ofEntry), ret);
    cntEntryIdx = entryIdx | (0x3 << 12);
    RT_ERR_CHK(table_write(unit, MANGO_FLOW_CNTRt, cntEntryIdx, (uint32 *) &cntrEntry), ret);
    RT_ERR_CHK(_dal_mango_pie_entryHitSts_get(unit, entryIdx, TRUE, (uint32 *)&hitSts), ret);

    return ret;
}   /* end of _dal_mango_of_tblEntry_del */

/* Function Name:
 *      dal_mango_of_flowEntry_del
 * Description:
 *      Delete the specified flow entries.
 * Input:
 *      unit    - unit id
 *      phase   - Flow Table phase
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OF_FT_PHASE     - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_OF_CLEAR_INDEX  - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all cleared.
 */
int32
dal_mango_of_flowEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx)
{
    int32 ret;
    uint32 entry_idx, phy_startIdx, phy_endIdx;
    dal_mango_of_igrFtIns_t igrIns;
    rtk_bitmap_t    *pDeletedInfo;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d", unit, phase);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pClrIdx->start_idx > pClrIdx->end_idx, RT_ERR_INPUT);

    /* free allocated resources first */
    if (phase == FT_PHASE_IGR_FT_0 || phase == FT_PHASE_IGR_FT_3)
    {
        /* translate to physical index */
        DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, pClrIdx->start_idx, phy_startIdx);
        DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, pClrIdx->end_idx, phy_endIdx);

        for (entry_idx=phy_startIdx ;entry_idx<=phy_endIdx ; entry_idx++)
        {
            if ((ret = _dal_mango_of_igrFTInstruct_get(unit, entry_idx, &igrIns)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = _dal_mango_of_flowEntryResource_free(unit, &igrIns)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
    }

    /* delelte entry from HW */
    OF_SEM_LOCK(unit);

    pDeletedInfo = osal_alloc(BITMAP_ARRAY_SIZE(HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)));
    if (!pDeletedInfo)
    {
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_DAL|MOD_OPENFLOW), "alloc memory fail");
        OF_SEM_UNLOCK(unit);
        return RT_ERR_MEM_ALLOC;
    }

    RT_ERR_HDL(_dal_mango_of_entry_del(unit, phase, pClrIdx, pDeletedInfo), ERR, ret);

ERR:
    osal_free(pDeletedInfo);

    OF_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_of_flowEntry_del */

static int32
_dal_mango_of_tbl_entry_move(uint32 unit, rtk_of_flowtable_phase_t phase,
    uint32 fromIdx, uint32 toIdx, rtk_bitmap_t *pEntryMovedInfo)
{
    of_igr_entry_t      entry;
    uint32              table;
    int32               ret;

    OF_MV_DBG("%s from %u to %u\n", __func__, fromIdx, toIdx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table        = MANGO_FT_IGRt;
            break;
        case FT_PHASE_EGR_FT_0:
            table        = MANGO_FT_EGRt;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    RT_ERR_CHK(OF_TBL_READ(unit, table, fromIdx, (uint32 *)&entry), ret);
    RT_ERR_CHK(OF_TBL_WRITE(unit, table, toIdx, (uint32 *)&entry), ret);

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
}   /* end of _dal_mango_of_tbl_entry_move */

/*
    Aggregation: E(N), E(N+1), E(N+128), E(N+129)
    agg2Base is                 E(N)
    agg1Base is                 E(N) and/or (E+128)
    agg2BaseAgg1Partner is      E(N+1)
    agg2PartnerAgg1Partner is   E(N+129)
*/
static int32
_dal_mango_of_entryAggType_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 entryIdx,
    dal_mango_of_aggInfo_t *pAggInfo)
{
    of_igr_entry_t  entry;
    uint32          entryBlkIdx, agg2BaseBlkIdx, agg2PartnerBlkIdx;
    uint32          baseEntryOfst;
    uint32          baseEntryIdx, agg2PartnerEntryIdx;
    uint32          table, fieldAggr1 = 0, fieldAggr2;
    int32           ret;

    RT_PARAM_CHK((NULL == pAggInfo), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table        = MANGO_FT_IGRt;
            fieldAggr1   = MANGO_FT_IGR_AGGR_1tf;
            fieldAggr2   = MANGO_FT_IGR_AGGR_2tf;
            break;
        case FT_PHASE_EGR_FT_0:
            table        = MANGO_FT_EGRt;
            fieldAggr2   = MANGO_FT_EGR_AGGR_2tf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* Init status */
    pAggInfo->valid             = ENABLED;
    pAggInfo->agg2Sts           = DISABLED;
    pAggInfo->agg1Sts           = DISABLED;
    pAggInfo->baseEntryIdx      = entryIdx;

    /* find AND2 operator block index */
    entryBlkIdx     = entryIdx >> OF_BLK_BIT_OFST;
    agg2BaseBlkIdx  = entryBlkIdx - (entryBlkIdx % 2);

    /* find entry offset of block */
    baseEntryOfst = (entryIdx & OF_ENTRY_IDX_MSK) - (entryIdx % 2);

    baseEntryIdx = (agg2BaseBlkIdx << OF_BLK_BIT_OFST) | baseEntryOfst;

    agg2PartnerBlkIdx   = agg2BaseBlkIdx + 1;
    agg2PartnerEntryIdx = (agg2PartnerBlkIdx << OF_BLK_BIT_OFST) + baseEntryOfst;

    RT_ERR_CHK(OF_TBL_READ(unit, table, baseEntryIdx, (uint32 *)&entry), ret);

    RT_ERR_CHK(table_field_get(unit, table, fieldAggr2,
            &pAggInfo->agg2Sts, (uint32 *)&entry), ret);

    if (ENABLED == pAggInfo->agg2Sts)
    {
        pAggInfo->baseEntryIdx          = baseEntryIdx;
        pAggInfo->agg2PartnerEntryIdx   = agg2PartnerEntryIdx;

        /* When AGG2 is enabled, move entry is quadruple mode */
        if (FT_PHASE_EGR_FT_0 != phase)
        {
            RT_ERR_CHK(table_field_get(unit, table, fieldAggr2,
                    &pAggInfo->agg1Sts, (uint32 *)&entry), ret);
        }
        else
        {
            pAggInfo->agg1Sts = DISABLED;
        }

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
            RT_ERR_CHK(OF_TBL_READ(unit, table, agg2PartnerEntryIdx, (uint32 *)&entry), ret);
            pAggInfo->baseEntryIdx = agg2PartnerEntryIdx;
        }

        if (FT_PHASE_EGR_FT_0 != phase)
        {
            RT_ERR_CHK(table_field_get(unit, table, fieldAggr1,
                    &pAggInfo->agg1Sts, (uint32 *)&entry), ret);
        }
        else
        {
            pAggInfo->agg1Sts = DISABLED;
        }

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
}   /* end of _dal_mango_of_entryAggType_get */

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
_dal_mango_of_mvEntryAggType_get(uint32 unit, rtk_of_flowtable_phase_t phase, dal_mango_of_aggMoveInfo_t *pAggMvInfo,
    dal_mango_of_aggInfo_t *pAggInfo)
{
    uint32          entryIdx;
    int32           ret;

    RT_PARAM_CHK((NULL == pAggMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAggInfo), RT_ERR_NULL_POINTER);

    entryIdx = pAggMvInfo->srcIdx;

    RT_ERR_CHK(_dal_mango_of_entryAggType_get(unit, phase, entryIdx, pAggInfo), ret);

    if (pAggMvInfo->agg2Sts != pAggInfo->agg2Sts)
    {
        OF_MV_DBG("entry %u agg2Sts mis-match %d %d\n",
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
        OF_MV_DBG("%s agg2Sts %d agg1Sts %d start %u end %u entry %u (%u %u %u %u)\n",
                __func__, pAggInfo->agg2Sts, pAggInfo->agg1Sts, pAggMvInfo->srcStartIdx,
                pAggMvInfo->srcEndIdx, pAggMvInfo->srcIdx, pAggInfo->baseEntryIdx,
                pAggInfo->agg1PartnerEntryIdx, pAggInfo->agg2PartnerEntryIdx,
                pAggInfo->agg2PartnerAgg1PartnerEntryIdx);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_of_mvEntryAggType_get */

static int32
_dal_mango_of_aggEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, dal_mango_of_aggInfo_t *pAggInfo)
{
    int32   ret;

    RT_PARAM_CHK((NULL == pAggInfo), RT_ERR_NULL_POINTER);

    /* Delete Agg2BaseAgg1Partner entry at least */
    if ((pAggInfo->agg2Sts || pAggInfo->agg1Sts) && (FT_PHASE_EGR_FT_0 != phase))
    {
        RT_ERR_CHK(_dal_mango_of_tblEntry_del(unit, phase, pAggInfo->agg1PartnerEntryIdx), ret);
    }

    if (pAggInfo->agg2Sts)
    {
        /* Delete Agg2PartnerAgg1Partner entry */
        if (FT_PHASE_EGR_FT_0 != phase)
        {
            RT_ERR_CHK(_dal_mango_of_tblEntry_del(unit, phase, pAggInfo->agg2PartnerAgg1PartnerEntryIdx), ret);
        }

        /* !!! Delete Agg2Partner entry after Agg2PartnerAgg1Partner !!!*/
        RT_ERR_CHK(_dal_mango_of_tblEntry_del(unit, phase, pAggInfo->agg2PartnerEntryIdx), ret);
    }

    /* !!! Delete Agg2 base entry at least !!! */
    RT_ERR_CHK(_dal_mango_of_tblEntry_del(unit, phase, pAggInfo->baseEntryIdx), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_of_aggEntry_del */

static int32
_dal_mango_of_aggEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, dal_mango_of_aggMoveInfo_t *pAggMvInfo)
{
    dal_mango_of_aggInfo_t  entryAggInfo;
    of_igr_entry_t          ofEntry;
    log_entry_t             cntrEntry;
    uint32                  oriAgg2BaseValid, newAgg2BaseValid;
    uint32                  mvDstIdx;
    uint32                  table, field;
    int32                   mvOfst;
    int32                   ret;

    RT_PARAM_CHK((NULL == pAggMvInfo), RT_ERR_NULL_POINTER);

    OF_MV_DBG("%s src %u dst %u agg2Sts %d\n", __func__,
            pAggMvInfo->srcIdx, pAggMvInfo->dstIdx, pAggMvInfo->agg2Sts);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table = MANGO_FT_IGRt;
            field = MANGO_FT_IGR_VALIDtf;
            break;
        case FT_PHASE_EGR_FT_0:
            table = MANGO_FT_EGRt;
            field = MANGO_FT_EGR_VALIDtf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    RT_ERR_CHK(_dal_mango_of_mvEntryAggType_get(unit, phase, pAggMvInfo, &entryAggInfo), ret);

    OF_MV_DBG("agg2Sts %u\n", entryAggInfo.agg2Sts);
    OF_MV_DBG("agg1Sts %u\n", entryAggInfo.agg1Sts);
    OF_MV_DBG("baseEntryIdx %u\n", entryAggInfo.baseEntryIdx);
    OF_MV_DBG("agg1PartnerEntryIdx %u\n", entryAggInfo.agg1PartnerEntryIdx);
    OF_MV_DBG("agg2PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerEntryIdx);
    OF_MV_DBG("agg2PartnerAgg1PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerAgg1PartnerEntryIdx);

    if (DISABLED == entryAggInfo.valid)
    {
        OF_MV_DBG("entry %u invalid\n", pAggMvInfo->srcIdx);
        return RT_ERR_OK;
    }

    mvOfst = pAggMvInfo->dstIdx - pAggMvInfo->srcIdx;

    /* single entry */
    if (DISABLED == entryAggInfo.agg2Sts && DISABLED == entryAggInfo.agg1Sts)
    {
        mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

        OF_MV_DBG("Single src %u dst %u\n", entryAggInfo.baseEntryIdx, mvDstIdx);

        RT_ERR_CHK(_dal_mango_of_tbl_entry_move(unit, pAggMvInfo->phase,
                entryAggInfo.baseEntryIdx, mvDstIdx, pAggMvInfo->pEntryMovedInfo), ret);
    }
    /* aggregation entry */
    else //if (entryAggInfo.agg2Sts || entryAggInfo.agg1Sts)
    {
        /* AGG2 base entry, invalid Agg2 base entry at first */
        mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

        OF_MV_DBG("Agg2 src %u dst %u\n", entryAggInfo.baseEntryIdx, mvDstIdx);

        RT_ERR_CHK(OF_TBL_READ(unit, table, entryAggInfo.baseEntryIdx,
                (uint32 *)&ofEntry), ret);

        RT_ERR_CHK(table_field_get(unit, table, field, &oriAgg2BaseValid,
                (uint32 *)&ofEntry), ret);

        newAgg2BaseValid = 0;
        RT_ERR_CHK(table_field_set(unit, table, field, &newAgg2BaseValid,
                (uint32 *)&ofEntry), ret);

        RT_ERR_CHK(OF_TBL_WRITE(unit, table, mvDstIdx,
                (uint32 *)&ofEntry), ret);

        BITMAP_SET(pAggMvInfo->pEntryMovedInfo, entryAggInfo.baseEntryIdx);
        BITMAP_SET(pAggMvInfo->pEntryMovedInfo, mvDstIdx);

        /* Agg2BaseAgg1Partner entry */
        if (entryAggInfo.agg1Sts)
        {
            mvDstIdx = entryAggInfo.agg1PartnerEntryIdx + mvOfst;

            OF_MV_DBG("Agg2 base Agg1 src %u dst %u\n",
                    entryAggInfo.agg1PartnerEntryIdx, mvDstIdx);

            RT_ERR_CHK(_dal_mango_of_tbl_entry_move(unit, pAggMvInfo->phase,
                    entryAggInfo.agg1PartnerEntryIdx, mvDstIdx, pAggMvInfo->pEntryMovedInfo), ret);
        }
    }

    if (entryAggInfo.agg2Sts)
    {
        /* Agg2Partner entry */
        mvDstIdx = entryAggInfo.agg2PartnerEntryIdx + mvOfst;

        OF_MV_DBG("Agg2 partner src %u dst %u\n", entryAggInfo.agg2PartnerEntryIdx, mvDstIdx);

        RT_ERR_CHK(_dal_mango_of_tbl_entry_move(unit, pAggMvInfo->phase,
                entryAggInfo.agg2PartnerEntryIdx, mvDstIdx, pAggMvInfo->pEntryMovedInfo), ret);

        /* Agg2PartnerAgg1Partner entry */
        if (entryAggInfo.agg1Sts)
        {
            mvDstIdx = entryAggInfo.agg2PartnerAgg1PartnerEntryIdx + mvOfst;

            OF_MV_DBG("Agg2 partner src %u dst %u\n",
                    entryAggInfo.agg2PartnerAgg1PartnerEntryIdx, mvDstIdx);

            RT_ERR_CHK(_dal_mango_of_tbl_entry_move(unit, pAggMvInfo->phase,
                    entryAggInfo.agg2PartnerAgg1PartnerEntryIdx, mvDstIdx,
                    pAggMvInfo->pEntryMovedInfo), ret);
        }
    }

    /* Validate Agg2 entry at least */
    if ((entryAggInfo.agg2Sts || entryAggInfo.agg1Sts) && (newAgg2BaseValid != oriAgg2BaseValid))
    {
        mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

        OF_MV_DBG("Enable Agg base entry %u valid\n", mvDstIdx);

        RT_ERR_CHK(OF_TBL_READ(unit, table, mvDstIdx,
                (uint32 *)&ofEntry), ret);

        RT_ERR_CHK(table_field_set(unit, table, field, &oriAgg2BaseValid,
                (uint32 *)&ofEntry), ret);

        RT_ERR_CHK(OF_TBL_WRITE(unit, table, mvDstIdx,
                (uint32 *)&ofEntry), ret);
    }

    /* counter move */
    mvDstIdx = entryAggInfo.baseEntryIdx + mvOfst;

    OF_MV_DBG("Move base entry %u counter to %u\n", entryAggInfo.baseEntryIdx, mvDstIdx);

    RT_ERR_CHK(table_read(unit, MANGO_FLOW_CNTRt, entryAggInfo.baseEntryIdx,
            (uint32 *)&cntrEntry), ret);

    mvDstIdx |= (0x3 << 12);
    RT_ERR_CHK(table_write(unit, MANGO_FLOW_CNTRt, mvDstIdx,
            (uint32 *)&cntrEntry), ret);

    /* delete original entry */
    RT_ERR_CHK(_dal_mango_of_aggEntry_del(unit, pAggMvInfo->phase, &entryAggInfo), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_of_aggEntry_move */

static int32
_dal_mango_of_ruleAggSts_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pMvInfo,
    rtk_enable_t *pAgg1Sts, rtk_enable_t *pAgg2Sts)
{
    of_igr_entry_t  ofEntry;
    uint32          srcIdx;
    int32           ret;
    uint32          table, fieldAggr1 = 0, fieldAggr2;

    RT_PARAM_CHK((NULL == pMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAgg1Sts), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAgg2Sts), RT_ERR_NULL_POINTER);

    *pAgg1Sts = DISABLED;
    *pAgg2Sts = DISABLED;

    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, pMvInfo->move_from, srcIdx);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
        case FT_PHASE_IGR_FT_3:
            table        = MANGO_FT_IGRt;
            fieldAggr1   = MANGO_FT_IGR_AGGR_1tf;
            fieldAggr2   = MANGO_FT_IGR_AGGR_2tf;
            break;
        case FT_PHASE_EGR_FT_0:
            table        = MANGO_FT_EGRt;
            fieldAggr2   = MANGO_FT_EGR_AGGR_2tf;
            break;
        default:
            return RT_ERR_OF_FT_PHASE;
    }

    /* find AND1 & AND2 operator block index */
    RT_ERR_CHK(OF_TBL_READ(unit, table, srcIdx, (uint32 *)&ofEntry), ret);

    if (FT_PHASE_EGR_FT_0 != phase)
    {
        RT_ERR_CHK(table_field_get(unit, table, fieldAggr1, pAgg1Sts, (uint32 *)&ofEntry), ret);
    }

    RT_ERR_CHK(table_field_get(unit, table, fieldAggr2, pAgg2Sts, (uint32 *)&ofEntry), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_of_ruleAggSts_get */

static int32
_dal_mango_of_entry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx,
    rtk_bitmap_t *pDeletedInfo)
{
    dal_mango_of_aggInfo_t      entryAggInfo;
    uint32                      entryIdx, phyEntryIdx, startIdx, endIdx;
    int32                       ret;

    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDeletedInfo), RT_ERR_NULL_POINTER);

    OF_DEL_DBG("%s startIdx %u endIdx %u\n", __func__, pClrIdx->start_idx, pClrIdx->end_idx);

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
        DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entryIdx, phyEntryIdx);

        if (BITMAP_IS_SET(pDeletedInfo, phyEntryIdx))
        {
            OF_DEL_DBG("Skip entry %d has deleted\n", phyEntryIdx);
            continue;
        }

        RT_ERR_CHK(_dal_mango_of_entryAggType_get(unit, phase, phyEntryIdx, &entryAggInfo), ret);

        OF_DEL_DBG("agg2Sts %u\n", entryAggInfo.agg2Sts);
        OF_DEL_DBG("agg1Sts %u\n", entryAggInfo.agg1Sts);
        OF_DEL_DBG("baseEntryIdx %u\n", entryAggInfo.baseEntryIdx);
        OF_DEL_DBG("agg1PartnerEntryIdx %u\n", entryAggInfo.agg1PartnerEntryIdx);
        OF_DEL_DBG("agg2PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerEntryIdx);
        OF_DEL_DBG("agg2PartnerAgg1PartnerEntryIdx %u\n", entryAggInfo.agg2PartnerAgg1PartnerEntryIdx);

        RT_ERR_CHK(_dal_mango_of_aggEntry_del(unit, phase, &entryAggInfo), ret);

        BITMAP_SET(pDeletedInfo, entryAggInfo.baseEntryIdx);

        if ((entryAggInfo.agg2Sts || entryAggInfo.agg1Sts) && (FT_PHASE_EGR_FT_0 != phase))
        {
            BITMAP_SET(pDeletedInfo, entryAggInfo.agg1PartnerEntryIdx);
        }

        if (entryAggInfo.agg2Sts)
        {
            /* Delete Agg2PartnerAgg1Partner entry */
            if (FT_PHASE_EGR_FT_0 != phase)
            {
                BITMAP_SET(pDeletedInfo, entryAggInfo.agg2PartnerAgg1PartnerEntryIdx);
            }
            BITMAP_SET(pDeletedInfo, entryAggInfo.agg2PartnerEntryIdx);
        }
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_of_entry_del */

static int32
_dal_mango_of_physicalEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase,
    dal_mango_of_aggMoveInfo_t *pAggMvInfo,
    const dal_mango_of_physicalMove_info_t * const pPhyMvInfo)
{
    uint32  logicSrcOfstIdx, logicDstOfstIdx;
    uint32  mvSrcIdx, mvDstIdx;
    uint32  srcIdx, dstIdx;
    int32   ret;

    RT_PARAM_CHK((NULL == pAggMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPhyMvInfo), RT_ERR_NULL_POINTER);

    OF_MV_DBG("lenIdx %d\n", pPhyMvInfo->lenIdx);
    logicSrcOfstIdx = (pPhyMvInfo->lenIdx + pPhyMvInfo->logicSrcBlkEntryOfst);
    logicDstOfstIdx = (pPhyMvInfo->lenIdx + pPhyMvInfo->logicDstBlkEntryOfst);

    mvSrcIdx = (((logicSrcOfstIdx / pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->logicBlkOfst) +
            ((logicSrcOfstIdx % pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->entryWidth) + pPhyMvInfo->srcBlkEntryBase);

    mvDstIdx = (((logicDstOfstIdx / pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->logicBlkOfst) +
            ((logicDstOfstIdx % pPhyMvInfo->logicBlkEntryNum) * pPhyMvInfo->entryWidth) + pPhyMvInfo->dstBlkEntryBase);

    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, mvSrcIdx, srcIdx);
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, mvDstIdx, dstIdx);
    OF_MV_DBG("mvSrcIdx %u mvDstIdx %u srcIdx %u dstIdx %u\n", mvSrcIdx, mvDstIdx, srcIdx, dstIdx);

    if (BITMAP_IS_CLEAR(pAggMvInfo->pEntryMovedInfo, srcIdx))
    {
        pAggMvInfo->srcIdx = srcIdx;
        pAggMvInfo->dstIdx = dstIdx;
        RT_ERR_CHK(_dal_mango_of_aggEntry_move(unit, phase, pAggMvInfo), ret);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_of_physicalEntry_move */

static int32
_dal_mango_of_entry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pMvInfo,
    rtk_bitmap_t *pEntryMovedInfo)
{
    rtk_enable_t            agg1Sts, agg2Sts;
    dal_mango_of_aggMoveInfo_t aggMvInfo;
    uint32                  convertLen, dualEntryLen, blkEntryNum;
    uint32                  dstIdx;
    uint32                  srcBlkEntryOfst, dstBlkEntryOfst;
    uint32                  blkWidth;
    int32                   lenIdx;
    int32                   ret;
    dal_mango_of_physicalMove_info_t   phyMvInfo;

    RT_PARAM_CHK((NULL == pMvInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntryMovedInfo), RT_ERR_NULL_POINTER);

    OF_MV_DBG("%s from %#x to %#x len %u\n", __func__,
            pMvInfo->move_from, pMvInfo->move_to, pMvInfo->length);

    /* entry index from and to are same physical/logic type */
    if (PIE_ENTRY_IS_PHYSICAL_TYPE(pMvInfo->move_from) !=
            PIE_ENTRY_IS_PHYSICAL_TYPE(pMvInfo->move_to))
    {
        return RT_ERR_ENTRY_INDEX;
    }

    RT_ERR_CHK(_dal_mango_of_ruleAggSts_get(unit, phase, pMvInfo, &agg1Sts, &agg2Sts), ret);

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
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, (pMvInfo->move_from + convertLen - 1), aggMvInfo.srcEndIdx);
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, (pMvInfo->move_to + convertLen - 1), dstIdx);
    /* check start from & to index. */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, pMvInfo->move_from, aggMvInfo.srcStartIdx);
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, pMvInfo->move_to, dstIdx);

    OF_MV_DBG("%s move from %u (end %u) to %u convertLen %u agg1Sts %u agg2Sts %u\n", __func__,
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

    OF_MV_DBG("entryWidth: %u ", phyMvInfo.entryWidth);
    OF_MV_DBG("blkWidth: %u ", blkWidth);
    OF_MV_DBG("srcBlkEntryOfst: %u ", srcBlkEntryOfst);
    OF_MV_DBG("dstBlkEntryOfst: %u ", dstBlkEntryOfst);
    OF_MV_DBG("srcBlkEntryBase: %u ", phyMvInfo.srcBlkEntryBase);
    OF_MV_DBG("dstBlkEntryBase: %u ", phyMvInfo.dstBlkEntryBase);
    OF_MV_DBG("logicBlkEntryNum: %u ", phyMvInfo.logicBlkEntryNum);
    OF_MV_DBG("logicBlkOfst: %u ", phyMvInfo.logicBlkEntryNum);
    OF_MV_DBG("logicSrcBlkEntryOfst: %u ", phyMvInfo.logicSrcBlkEntryOfst);
    OF_MV_DBG("logicDstBlkEntryOfst: %u ", phyMvInfo.logicDstBlkEntryOfst);

    /* move down */
    if (pMvInfo->move_from < pMvInfo->move_to)
    {
        for (lenIdx = (pMvInfo->length - 1); lenIdx >= 0; --lenIdx)
        {
            phyMvInfo.lenIdx = lenIdx;
            RT_ERR_CHK(_dal_mango_of_physicalEntry_move(unit, phase, &aggMvInfo, &phyMvInfo), ret);
        }
    }
    /* move up */
    else
    {
        for (lenIdx = 0; lenIdx < pMvInfo->length; ++lenIdx)
        {
            phyMvInfo.lenIdx = lenIdx;
            RT_ERR_CHK(_dal_mango_of_physicalEntry_move(unit, phase, &aggMvInfo, &phyMvInfo), ret);
        }
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_of_entry_move */

/* Function Name:
 *      dal_mango_of_flowEntry_move
 * Description:
 *      Move the specified flow entries.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      pData - movement info
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all moved.
 *      (3) The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      (4) (move_from + length) and (move_to + length) must <= the number of flow entries for the specified phase.
 */
int32
dal_mango_of_flowEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pData)
{
    rtk_bitmap_t    *physicalEntryMoved;
    int32   ret;
    rtk_pie_phase_t pphase;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d", unit, phase);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->move_from == pData->move_to), RT_ERR_INPUT);
    RT_PARAM_CHK((0 == pData->length), RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d,phase=%d,move_from=%d,move_to=%d,length=%d",
            unit, phase, pData->move_from, pData->move_to, pData->length);

    DAL_MANGO_PHASE_OF_TO_PIE(phase, pphase);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, pphase=%d", unit, pphase);

    OF_SEM_LOCK(unit);

    physicalEntryMoved = osal_alloc(BITMAP_ARRAY_SIZE(HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)));
    if (!physicalEntryMoved)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_DAL|MOD_OPENFLOW), "alloc memory fail");
        return RT_ERR_MEM_ALLOC;
    }


    if ((ret = _dal_mango_of_entry_move(unit, phase, pData, physicalEntryMoved)) != RT_ERR_OK)
    {
        osal_free(physicalEntryMoved);
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "entry move fail");
        return ret;
    }

    osal_free(physicalEntryMoved);
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntry_move */

/* Function Name:
 *      dal_mango_of_ftTemplateSelector_get
 * Description:
 *      Get the mapping template of specific Flow Table block.
 * Input:
 *      unit          - unit id
 *      phase         - Flow Table phase
 *      block_idx     - block index
 * Output:
 *      pTemplate_idx - template index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_OF_BLOCK_PHASE   - The block is not belonged to Flow Table
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_ftTemplateSelector_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   *pTemplate_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, block_idx=%d", unit, phase, block_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pTemplate_idx), RT_ERR_NULL_POINTER);

    /* translate to physical block index */
    DAL_MANGO_OF_BLKINDEX_TO_PHYSICAL(unit, phase, block_idx, block_idx);

    if ((ret = _dal_mango_pie_templateSelector_get(unit, block_idx, &pTemplate_idx->template_id[0],\
        &pTemplate_idx->template_id[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "template1_idx=%d, template2_idx=%d",\
        pTemplate_idx->template_id[0], pTemplate_idx->template_id[1]);

    return RT_ERR_OK;
}   /* end of dal_mango_of_ftTemplateSelector_get */

/* Function Name:
 *      dal_mango_of_ftTemplateSelector_set
 * Description:
 *      Set the mapping template of specific Flow Table block.
 * Input:
 *      unit         - unit id
 *      phase        - Flow Table phase
 *      block_idx    - block index
 *      template_idx - template index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_OF_BLOCK_PHASE       - The block is not belonged to Flow Table
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
int32
dal_mango_of_ftTemplateSelector_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   template_idx)
{
    int32           ret;
    uint32          i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, block_idx=%d, template1_idx=%d, template2_idx=%d", \
        unit, phase, block_idx, template_idx.template_id[0], template_idx.template_id[1]);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    for (i = 0; i < HAL_MAX_NUM_OF_PIE_BLOCK_TEMPLATESELECTOR(unit); ++i)
    {
        RT_PARAM_CHK((template_idx.template_id[i] >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    }

    /* translate to physical block index */
    DAL_MANGO_OF_BLKINDEX_TO_PHYSICAL(unit, phase, block_idx, block_idx);

    if ((ret = _dal_mango_pie_templateSelector_set(unit, block_idx, template_idx.template_id[0],\
        template_idx.template_id[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_ftTemplateSelector_set */

/* Function Name:
 *      dal_mango_of_flowCntMode_get
 * Description:
 *      Get counter mode for specific flow counters.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pMode     - counter mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 */
int32
dal_mango_of_flowCntMode_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     *pMode)
{
    int32       ret;
    log_entry_t entry;
    uint32      val = 0, phy_entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    osal_memset((void *)&entry, 0x00, sizeof(log_entry_t));

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf, \
        (uint32 *)&val, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    switch (val)
    {
        case 0:
            *pMode = OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT;
            break;
        case 1:
            *pMode = OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH;
            break;
        case 2:
            *pMode = OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH;
            break;
        case 3:
            *pMode = OF_FLOW_CNTMODE_DISABLE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pMode=%d",*pMode);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowCntMode_get */

/* Function Name:
 *      dal_mango_of_flowCntMode_set
 * Description:
 *      Set counter mode for specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      mode      - counter mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 */
int32
dal_mango_of_flowCntMode_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     mode)
{
    int32       ret;
    log_entry_t entry;
    uint32      val = 0, zero = 0, phy_entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, mode=%d", unit, phase, entry_idx, mode);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((OF_FLOW_CNTMODE_END <= mode), RT_ERR_INPUT);

    switch (mode)
    {
        case OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT:
            val = 0;
            break;
        case OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH:
            val = 1;
            break;
        case OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH:
            val = 2;
            break;
        case OF_FLOW_CNTMODE_DISABLE:
            val = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    osal_memset((void *)&entry, 0x00, sizeof(log_entry_t));

    if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf, \
        &val, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf, \
        &zero, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf, \
        &zero, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf, \
        &zero, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf, \
        &zero, (uint32 *) &entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* ADDR[13:0]={PKTCNTR[0:0], BYTECNTR[0:0], Index [11:0]} */
    phy_entry_idx |= (1<<12) | (1<<13);

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowCntMode_set */

/* Function Name:
 *      dal_mango_of_flowCnt_get
 * Description:
 *      Get packet counter or byte counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      pCnt      - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 */
int32
dal_mango_of_flowCnt_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type,
    uint64                   *pCnt)
{
    int32                   ret;
    log_entry_t             entry;
    rtk_of_flowCntMode_t    mode;
    uint32                  cntr_H, cntr_L;
    uint32                  phy_entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((OF_FLOW_CNT_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf, \
        (uint32 *)&mode, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if((mode == OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH) && (type != OF_FLOW_CNT_TYPE_PACKET))
        return RT_ERR_INPUT;

    if((mode == OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH) && (type != OF_FLOW_CNT_TYPE_BYTE))
        return RT_ERR_INPUT;

    if(type == OF_FLOW_CNT_TYPE_BYTE)
    {
        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf, \
            &cntr_H, (uint32 *)&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf, \
            &cntr_L, (uint32 *)&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else if(type == OF_FLOW_CNT_TYPE_PACKET)
    {
        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf, \
            &cntr_H, (uint32 *)&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf, \
            &cntr_L, (uint32 *)&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else
        return RT_ERR_INPUT;

    *pCnt = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pCnt=%llu",*pCnt);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowCnt_get */

/* Function Name:
 *      dal_mango_of_flowCnt_clear
 * Description:
 *      Clear counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_OF_FT_PHASE - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 *      RT_ERR_INPUT       - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 */
int32
dal_mango_of_flowCnt_clear(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type)
{
    int32                   ret;
    uint32                  val = 0, phy_entry_idx;
    log_entry_t             entry;
    rtk_of_flowCntMode_t    mode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, type=%d", unit, phase, entry_idx, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((OF_FLOW_CNT_TYPE_END <= type), RT_ERR_INPUT);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf, \
                                (uint32 *)&mode, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if((mode == OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH) && (type != OF_FLOW_CNT_TYPE_PACKET))
        return RT_ERR_INPUT;

    if((mode == OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH) && (type != OF_FLOW_CNT_TYPE_BYTE))
        return RT_ERR_INPUT;

    if(type == OF_FLOW_CNT_TYPE_BYTE)
    {
        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf,\
                                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf,\
                                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* ADDR[13:0]={PKTCNTR[0:0], BYTECNTR[0:0], Index [11:0]} */
        phy_entry_idx |= (1<<12);
    }
    else if(type == OF_FLOW_CNT_TYPE_PACKET)
    {
        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf,\
                                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf,\
                                    &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* ADDR[13:0]={PKTCNTR[0:0], BYTECNTR[0:0], Index [11:0]} */
        phy_entry_idx |= (1<<13);
    }
    else
        return RT_ERR_INPUT;

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowCnt_clear */

/* Function Name:
 *      dal_mango_of_flowCntThresh_get
 * Description:
 *      Get packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 * Output:
 *      pThreshold - pointer buffer of interrupt trigger threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 */
int32
dal_mango_of_flowCntThresh_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   *pThreshold)
{
    int32                   ret;
    log_entry_t             entry;
    rtk_of_flowCntMode_t    mode;
    uint32                  val, th_H, th_L, phy_entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d", unit, phase, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf, \
                                (uint32 *)&val, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    switch (val)
    {
        case 0:
            mode = OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT;
            break;
        case 1:
            mode = OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH;
            break;
        case 2:
            mode = OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if (mode != OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH &&
        mode != OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH)
        return RT_ERR_INPUT;/* incompatible counter mode */

    if (mode == OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH)
    {
        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf, \
            &th_L, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf, \
            &th_H, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else if (mode == OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH)
    {
        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf, \
            &th_L, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf, \
            &th_H, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    *pThreshold = (((uint64)th_H) << 32) | ((uint64)th_L);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowCntThresh_get */

/* Function Name:
 *      dal_mango_of_flowCntThresh_set
 * Description:
 *      Set packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 *      threshold  - interrupt trigger threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 */
int32
dal_mango_of_flowCntThresh_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   threshold)
{
    int32                   ret;
    uint32                  val = 0;
    log_entry_t             entry;
    rtk_of_flowCntMode_t    mode;
    uint32                  th_H, th_L, phy_entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, entry_idx=%d, threshold=%llu", unit, phase, entry_idx, threshold);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((FT_PHASE_IGR_FT_1 == phase || FT_PHASE_IGR_FT_2 == phase), RT_ERR_OF_FT_PHASE);

    /* translate to physical index */
    DAL_MANGO_OF_INDEX_TO_PHYSICAL(unit, phase, entry_idx, phy_entry_idx);

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_CNTR_MODEtf, \
                                (uint32 *)&val, (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    switch (val)
    {
        case 0:
            mode = OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT;
            break;
        case 1:
            mode = OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH;
            break;
        case 2:
            mode = OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if (mode != OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH &&
        mode != OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH)
        return RT_ERR_INPUT;/* incompatible counter mode */

    th_H = (uint32)(threshold >> 32);
    th_L = (uint32)(threshold & 0xFFFFFFFF);

    if (mode == OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH)
    {
        RT_PARAM_CHK((HAL_OF_PKT_CNTR_TH_MAX(unit) < threshold), RT_ERR_OUT_OF_RANGE);

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf, \
            &th_L, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf, \
            &th_H, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* clear the counter when change threshold */
        val = 0;
        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf, \
            &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf, \
            &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else if (mode == OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH)
    {
        RT_PARAM_CHK((HAL_OF_BYTE_CNTR_TH_MAX(unit) < threshold), RT_ERR_OUT_OF_RANGE);

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Ltf, \
            &th_L, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_PKT_CNTR_Htf, \
            &th_H, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* clear the counter when change threshold */
        val = 0;
        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Ltf, \
            &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_FLOW_CNTRt, MANGO_FLOW_CNTR_BYTE_CNTR_Htf, \
            &val, (uint32 *) &entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* ADDR[13:0]={PKTCNTR[0:0], BYTECNTR[0:0], Index [11:0]} */
    phy_entry_idx |= (1<<12) | (1<<13);

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, MANGO_FLOW_CNTRt, phy_entry_idx, (uint32 *) &entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowCntThresh_set */

/* Function Name:
 *      dal_mango_of_ttlExcpt_get
 * Description:
 *      Get action for invalid IP/MPLS TTL.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_FORWARD.
 */
int32
dal_mango_of_ttlExcpt_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_EXCPT_CTRLr, MANGO_TTL_ACTf, &val) ) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pAction = ACTION_DROP;
            break;
        case 1:
            *pAction = ACTION_FORWARD;
            break;
        default:
            break;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pAction=%d",*pAction);

    return RT_ERR_OK;
}   /* end of dal_mango_of_ttlExcpt_get */

/* Function Name:
 *      dal_mango_of_ttlExcpt_set
 * Description:
 *      Set action for invalid IP/MPLS TTL.
 * Input:
 *      unit   - unit id
 *      action - action to take
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_FORWARD.
 */
int32
dal_mango_of_ttlExcpt_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action != ACTION_DROP) && (action != ACTION_FORWARD), RT_ERR_INPUT);

    switch (action)
    {
        case ACTION_DROP:
            val = 0;
            break;
        case ACTION_FORWARD:
            val = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_EXCPT_CTRLr, MANGO_TTL_ACTf, &val) ) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_ttlExcpt_set */

/* Function Name:
 *      dal_mango_of_maxLoopback_get
 * Description:
 *      Get maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 * Output:
 *      pTimes - pointer buffer of loopback times
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 */
int32
dal_mango_of_maxLoopback_get(uint32 unit, uint32 *pTimes)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimes), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_ACT_CTRLr, MANGO_LB_LIMITf, pTimes)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pTimes=%d",*pTimes);

    return RT_ERR_OK;
}   /* end of dal_mango_of_maxLoopback_get */

/* Function Name:
 *      dal_mango_of_maxLoopback_set
 * Description:
 *      Set maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 *      times  - loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 */
int32
dal_mango_of_maxLoopback_set(uint32 unit, uint32 times)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, times=%d", unit, times);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_OF_LOOPBACK_MAX(unit) < times), RT_ERR_INPUT);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_ACT_CTRLr, MANGO_LB_LIMITf, &times)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_maxLoopback_set */

/* Function Name:
 *      dal_mango_of_l2FlowTblMatchField_get
 * Description:
 *      Get match field of L2 flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L2 flow table can lookup twice for a certain packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 */
int32
dal_mango_of_l2FlowTblMatchField_get(uint32 unit, rtk_of_l2FlowTblMatchField_t *pField)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_L2_FLOW_TBL_CTRLr, MANGO_LU_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pField = OF_L2_FT_MATCH_FIELD_VID_SA_SA;
            break;
        case 1:
            *pField = OF_L2_FT_MATCH_FIELD_VID_SA_VID_DA;
            break;
        case 2:
            *pField = OF_L2_FT_MATCH_FIELD_VID_SA_DA;
            break;
        case 3:
            *pField = OF_L2_FT_MATCH_FIELD_SA_VID_DA;
            break;
        case 4:
            *pField = OF_L2_FT_MATCH_FIELD_SA_DA;
            break;
        case 5:
            *pField = OF_L2_FT_MATCH_FIELD_VID_DA_DA;
            break;
        case 6:
            *pField = OF_L2_FT_MATCH_FIELD_DA_SA_VID_SA;
            break;
        case 7:
            *pField = OF_L2_FT_MATCH_FIELD_DA_SA_SA;
            break;
        case 8:
            *pField = OF_L2_FT_MATCH_FIELD_DA_SA_VID_DA;
            break;
        case 9:
            *pField = OF_L2_FT_MATCH_FIELD_DA_SA_DA;
            break;
        case 10:
            *pField = OF_L2_FT_MATCH_FIELD_FIVE_TUPLE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pField=%d", *pField);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowTblMatchField_get */

/* Function Name:
 *      dal_mango_of_l2FlowTblMatchField_set
 * Description:
 *      Set match field of L2 flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L2 flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 */
int32
dal_mango_of_l2FlowTblMatchField_set(uint32 unit, rtk_of_l2FlowTblMatchField_t field)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, field=%d", unit, field);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((OF_L2_FT_MATCH_FIELD_END <= field), RT_ERR_INPUT);

    switch (field)
    {
        case OF_L2_FT_MATCH_FIELD_VID_SA_SA:
            val = 0;
            break;
        case OF_L2_FT_MATCH_FIELD_VID_SA_VID_DA:
            val = 1;
            break;
        case OF_L2_FT_MATCH_FIELD_VID_SA_DA:
            val = 2;
            break;
        case OF_L2_FT_MATCH_FIELD_SA_VID_DA:
            val = 3;
            break;
        case OF_L2_FT_MATCH_FIELD_SA_DA:
            val = 4;
            break;
        case OF_L2_FT_MATCH_FIELD_VID_DA_DA:
            val = 5;
            break;
        case OF_L2_FT_MATCH_FIELD_DA_SA_VID_SA:
            val = 6;
            break;
        case OF_L2_FT_MATCH_FIELD_DA_SA_SA:
            val = 7;
            break;
        case OF_L2_FT_MATCH_FIELD_DA_SA_VID_DA:
            val = 8;
            break;
        case OF_L2_FT_MATCH_FIELD_DA_SA_DA:
            val = 9;
            break;
        case OF_L2_FT_MATCH_FIELD_FIVE_TUPLE:
            val = 10;
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_L2_FLOW_TBL_CTRLr, MANGO_LU_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowTblMatchField_set */

/* Function Name:
 *      dal_mango_of_l2FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Note:
 *      None
 */
int32
dal_mango_of_l2FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    int32                    ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, field_id=%d, type=%d", unit, field_id, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    if ((ret = _dal_mango_of_flowEntrySetField_check(unit, FT_PHASE_IGR_FT_1, field_id, type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowEntrySetField_check */

/* Function Name:
 *      dal_mango_of_l2FlowEntry_get
 * Description:
 *      Get L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L2 flow entry is 6 bit width.
 */
int32
dal_mango_of_l2FlowEntry_get(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    int32   ret;
    dal_mango_of_l2Entry_t l2_entry;
    dal_mango_of_getResult_t result;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_L2_FLOW_ENTRY_TYPE_END <= pEntry->type), RT_ERR_INPUT);

    if ((ret = _dal_mango_of_l2EntryRtk2Dal_convert(unit, &l2_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l2FlowEntry_getByMethod(unit, &l2_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (result.isExist == FALSE)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = _dal_mango_of_l2EntryDal2Rtk_convert(unit, pEntry, &l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* return the physical entry index */
    pEntry->ret_idx = result.idx;

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowEntry_get */

/* Function Name:
 *      dal_mango_of_l2FlowEntryNextValid_get
 * Description:
 *      Get next valid L2 flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l2 flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND   - no next valid entry found
 * Note:
 *      (1) Input -1 for getting the first valid entry of l2 flow table.
 *      (2) The pScan_idx is both the input and output argument.
 *      (3) The pScan_idx must be multiple of 2.
 */
int32
dal_mango_of_l2FlowEntryNextValid_get(
    uint32               unit,
    int32                *pScan_idx,
    rtk_of_l2FlowEntry_t *pEntry)
{
    int32  ret;
    uint32 i = 0, physical_camIdx = 0, found = FALSE;
    rtk_enable_t camState;
    dal_mango_of_l2Entry_t l2_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, *pScan_idx = %d", unit, *pScan_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScan_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((*pScan_idx != -1) && (*pScan_idx%2) == 1), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &camState))!=RT_ERR_OK)
        return ret;

    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 2;

    if (((*pScan_idx >= l2Sram_size[unit]) && (camState == DISABLED)) ||
        (*pScan_idx >= (l2Sram_size[unit] + l2Cam_size[unit])))
        return RT_ERR_OUT_OF_RANGE;

    if (*pScan_idx < l2Sram_size[unit])/* pScan_idx within SRAM */
    {
        for (i = *pScan_idx; i < l2Sram_size[unit]; i=i+2)
        {
            osal_memset((void *)&l2_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));

            if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_SRAM, i, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (l2_entry.valid && l2_entry.fmt == 1)
            {
                found = TRUE;
                goto exit;
            }
        }
    }

    /* no entry found in SRAM or pScan_idx within CAM */
    if ((camState == ENABLED) && ((i == l2Sram_size[unit]) || (*pScan_idx >= l2Sram_size[unit])))
    {
        /* convert logical to physical index */
        physical_camIdx = (i == l2Sram_size[unit]) ? 0 : (*pScan_idx - l2Sram_size[unit]);
        for (i = physical_camIdx; i < l2Cam_size[unit]; i=i+2)
        {
            if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_CAM, i, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (l2_entry.valid && l2_entry.fmt == 1)
            {
                found = TRUE;
                i += l2Sram_size[unit];/* convert physical to logical index */
                goto exit;
            }
        }
    }

exit:
    if (found == TRUE)
    {
        *pScan_idx = i;

        if ((ret = _dal_mango_of_l2EntryDal2Rtk_convert(unit, pEntry, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        return RT_ERR_OK;
    }
    else
        return RT_ERR_ENTRY_NOTFOUND;
}   /* end of dal_mango_of_l2FlowEntryNextValid_get */

/* Function Name:
 *      dal_mango_of_l2FlowEntry_add
 * Description:
 *      Add a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_EXIST  - entry existed
 *      RT_ERR_TBL_FULL     - The table is full
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 *      (3) The metadata in L2 flow entry is 6 bit width.
 */
int32
dal_mango_of_l2FlowEntry_add(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  portmaskIdx;
    dal_mango_of_getResult_t result;
    dal_mango_of_l2Entry_t   l2_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, type=%d", unit, pEntry->type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_L2_FLOW_ENTRY_TYPE_END <= pEntry->type), RT_ERR_INPUT);

    if ((ret = _dal_mango_of_l2EntryRtk2Dal_convert(unit, &l2_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* prevent race condition with L2 module */
    _dal_mango_l2_semLock(unit);

    /* search existed or free */
    if ((ret = _dal_mango_of_l2FlowEntry_getByMethod(unit, &l2_entry, MANGO_OF_GETMETHOD_EXIST_FIRST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        goto exit;
    }

    if ((result.isExist == TRUE) && ((pEntry->flags & RTK_OF_FLAG_FLOWENTRY_REPLACE) == 0))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto exit;
    }

    if ((result.isExist == FALSE) && (result.isFree == FALSE))
    {
        ret = RT_ERR_TBL_FULL;
        goto exit;
    }

    /* entry existed and output type is multi-ports which occupied a portmask entry, so free it. */
    if (result.isExist == TRUE && l2_entry.ins.output_type == MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT)
    {
        if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, l2_entry.ins.output_data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            goto exit;
        }
    }

    /* translate again because l2_entry may be overwrote by _dal_mango_of_l2FlowEntry_getByMethod */
    if ((ret = _dal_mango_of_l2EntryRtk2Dal_convert(unit, &l2_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        goto exit;
    }

    /* allocate a portmask entry to store the egress ports */
    if (l2_entry.ins.output_type == MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT)
    {
        if ((ret = _dal_mango_of_l2PortMaskEntry_set(unit, &pEntry->ins.wa_data.output_data.portmask,\
                    &portmaskIdx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            goto exit;
        }

        l2_entry.ins.output_data = portmaskIdx;
    }

    if ((ret = _dal_mango_of_l2FlowEntry_set(unit, result.memType, result.idx, &l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        _dal_mango_l2_mcastFwdIndex_free(unit, portmaskIdx);
        goto exit;
    }

exit:
    _dal_mango_l2_semUnlock(unit);
    return ret;
}   /* end of dal_mango_of_l2FlowEntry_add */

/* Function Name:
 *      dal_mango_of_l2FlowEntry_del
 * Description:
 *      Delete a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      None
 */
int32
dal_mango_of_l2FlowEntry_del(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    int32   ret;
    dal_mango_of_getResult_t result;
    dal_mango_of_l2Entry_t l2_entry, null_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_L2_FLOW_ENTRY_TYPE_END <= pEntry->type), RT_ERR_INPUT);

    if ((ret = _dal_mango_of_l2EntryRtk2Dal_convert(unit, &l2_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l2FlowEntry_getByMethod(unit, &l2_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (result.isExist)
    {
        osal_memset((void *)&null_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));

        if ((ret = _dal_mango_of_l2FlowEntry_set(unit, result.memType, result.idx, &null_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        /* output type is multi-ports which occupied a portmask entry, so free it. */
        if (l2_entry.ins.output_type == MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT)
        {
            if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, l2_entry.ins.output_data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }
        }
    }
    else
        return RT_ERR_ENTRY_NOTFOUND;

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowEntry_del */

/* Function Name:
 *      dal_mango_of_l2FlowEntry_delAll
 * Description:
 *      Delete all L2 flow entry.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_of_l2FlowEntry_delAll(uint32 unit)
{
    int32   ret;
    uint32  i;
    rtk_enable_t camState;
    dal_mango_of_l2Entry_t  l2_entry, null_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    for (i = 0; i < l2Sram_size[unit]; i++)
    {
        osal_memset((void *)&l2_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));

        if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_SRAM, i, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (l2_entry.valid && l2_entry.fmt == 1)
        {
            osal_memset((void *)&null_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));

            if ((ret = _dal_mango_of_l2FlowEntry_set(unit, MANGO_OF_MEMTYPE_SRAM, i, &null_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            /* output type is multi-ports which occupied a portmask entry, so free it. */
            if (l2_entry.ins.output_type == MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT)
            {
                if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, l2_entry.ins.output_data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }

            ++i;/* a L2 flow entry occupies two physical entries */
        }
    }

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &camState))!=RT_ERR_OK)
        return ret;

    if (camState == DISABLED)
        return RT_ERR_OK;

    for (i = 0; i < l2Cam_size[unit]; i++)
    {
        osal_memset((void *)&l2_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));

        if ((ret = _dal_mango_of_l2FlowEntry_get(unit, MANGO_OF_MEMTYPE_CAM, i, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (l2_entry.valid && l2_entry.fmt == 1)
        {
            osal_memset((void *)&null_entry, 0x00, sizeof(dal_mango_of_l2Entry_t));

            if ((ret = _dal_mango_of_l2FlowEntry_set(unit, MANGO_OF_MEMTYPE_CAM, i, &null_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            /* output type is multi-ports which occupied a portmask entry, so free it. */
            if (l2_entry.ins.output_type == MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT)
            {
                if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, l2_entry.ins.output_data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                    return ret;
                }
            }

            ++i;/* a L2 flow entry occupies two physical entries */
        }
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowEntry_delAll */

/* Function Name:
 *      dal_mango_of_l2FlowEntryHitSts_get
 * Description:
 *      Get the L2 flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 */
int32
dal_mango_of_l2FlowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l2FlowEntry_t     *pEntry,
    uint32                   reset,
    uint32                   *pIsHit)
{
    int32   ret;
    dal_mango_of_l2Entry_t l2_entry;
    dal_mango_of_getResult_t result;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, reset=%d", unit, reset);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_L2_FLOW_ENTRY_TYPE_END <= pEntry->type), RT_ERR_INPUT);

    if ((ret = _dal_mango_of_l2EntryRtk2Dal_convert(unit, &l2_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l2FlowEntry_getByMethod(unit, &l2_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (result.isExist == FALSE)
        return RT_ERR_ENTRY_NOTFOUND;

    *pIsHit = l2_entry.hit;

    if (reset && l2_entry.hit)
    {
        l2_entry.hit = 1;/* 1 means clear the hit bit */
        if ((ret = _dal_mango_of_l2FlowEntry_set(unit, result.memType, result.idx, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "*pIsHit=%d", *pIsHit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowEntryHitSts_get */

/* Function Name:
 *      dal_mango_of_l2FlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 */
int32
dal_mango_of_l2FlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo)
{
    int32   ret;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, block=%d", unit, block);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((2 <= block), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    if(block == 0)
        field = MANGO_BLK0_ALGOf;
    else
        field = MANGO_BLK1_ALGOf;

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_L2_FLOW_TBL_CTRLr, field, pAlgo)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pAlgo=%d", *pAlgo);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowTblHashAlgo_get */

/* Function Name:
 *      dal_mango_of_l2FlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 */
int32
dal_mango_of_l2FlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo)
{
    int32   ret;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, block=%d, algo=%d", unit, block, algo);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((2 <= block), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <= algo), RT_ERR_INPUT);

    if(block == 0)
        field = MANGO_BLK0_ALGOf;
    else
        field = MANGO_BLK1_ALGOf;

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_L2_FLOW_TBL_CTRLr, field, &algo)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l2FlowTblHashAlgo_set */

/* Function Name:
 *      dal_mango_of_l3FlowTblPri_get
 * Description:
 *      Get precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 * Output:
 *      pTable  - pointer to target table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 */
int32
dal_mango_of_l3FlowTblPri_get(uint32 unit, rtk_of_l3FlowTblList_t *pTable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, MANGO_LU_PRI_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pTable = OF_L3_FLOW_TBL_LIST_HASH;
            break;
        case 1:
            *pTable = OF_L3_FLOW_TBL_LIST_CAM;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pTable=%d", *pTable);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3FlowTblPri_get */

/* Function Name:
 *      dal_mango_of_l3FlowTblPri_set
 * Description:
 *      Set precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 *      table   - target table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 */
int32
dal_mango_of_l3FlowTblPri_set(uint32 unit, rtk_of_l3FlowTblList_t table)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, table=%d", unit, table);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((OF_L3_FLOW_TBL_LIST_END <= table), RT_ERR_INPUT);

    switch (table)
    {
        case OF_L3_FLOW_TBL_LIST_HASH:
            val = 0;
            break;
        case OF_L3_FLOW_TBL_LIST_CAM:
            val = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, MANGO_LU_PRI_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3FlowTblPri_set */

/* Function Name:
 *      dal_mango_of_l3CamFlowTblMatchField_get
 * Description:
 *      Get match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
int32
dal_mango_of_l3CamFlowTblMatchField_get(uint32 unit, rtk_of_l3CamFlowTblMatchField_t *pField)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, MANGO_CAM_LU_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pField = OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_MD_DIP;
            break;
        case 1:
            *pField = OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_SIP;
            break;
        case 2:
            *pField = OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_DIP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pField=%d", *pField);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamFlowTblMatchField_get */

/* Function Name:
 *      dal_mango_of_l3CamFlowTblMatchField_set
 * Description:
 *      Set match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
int32
dal_mango_of_l3CamFlowTblMatchField_set(uint32 unit, rtk_of_l3CamFlowTblMatchField_t field)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, field=%d", unit, field);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((OF_L3_CAM_FT_MATCH_FIELD_END <= field), RT_ERR_INPUT);

    switch (field)
    {
        case OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_MD_DIP:
            val = 0;
            break;
        case OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_SIP:
            val = 1;
            break;
        case OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_DIP:
            val = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, MANGO_CAM_LU_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamFlowTblMatchField_set */

/* Function Name:
 *      dal_mango_of_l3HashFlowTblMatchField_get
 * Description:
 *      Get match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
int32
dal_mango_of_l3HashFlowTblMatchField_get(uint32 unit, rtk_of_l3HashFlowTblMatchField_t *pField)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, MANGO_HASH_LU_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pField = OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP;
            break;
        case 1:
            *pField = OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_SIP;
            break;
        case 2:
            *pField = OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_DIP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pField=%d", *pField);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowTblMatchField_get */

/* Function Name:
 *      dal_mango_of_l3HashFlowTblMatchField_set
 * Description:
 *      Set match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
int32
dal_mango_of_l3HashFlowTblMatchField_set(uint32 unit, rtk_of_l3HashFlowTblMatchField_t field)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, field=%d", unit, field);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((OF_L3_HASH_FT_MATCH_FIELD_END <= field), RT_ERR_INPUT);

    switch (field)
    {
        case OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP:
            val = 0;
            break;
        case OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_SIP:
            val = 1;
            break;
        case OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_DIP:
            val = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, MANGO_HASH_LU_SELf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowTblMatchField_set */

/* Function Name:
 *      dal_mango_of_l3HashFlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 */
int32
dal_mango_of_l3HashFlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo)
{
    int32   ret;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, block=%d", unit, block);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((2 <= block), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    if(block == 0)
        field = MANGO_BLK0_ALGOf;
    else
        field = MANGO_BLK1_ALGOf;

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, field, pAlgo)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pAlgo=%d", *pAlgo);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowTblHashAlgo_get */

/* Function Name:
 *      dal_mango_of_l3HashFlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 */
int32
dal_mango_of_l3HashFlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo)
{
    int32   ret;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, block=%d, algo=%d", unit, block, algo);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((2 <= block), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <= algo), RT_ERR_INPUT);

    if(block == 0)
        field = MANGO_BLK0_ALGOf;
    else
        field = MANGO_BLK1_ALGOf;

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_L3_FLOW_TBL_CTRLr, field, &algo)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowTblHashAlgo_set */

/* Function Name:
 *      dal_mango_of_l3FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Note:
 *      None
 */
int32
dal_mango_of_l3FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, field_id=%d, type=%d", unit, field_id, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    if ((ret = _dal_mango_of_flowEntrySetField_check(unit, FT_PHASE_IGR_FT_2, field_id, type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3FlowEntrySetField_check */

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_get
 * Description:
 *      Get a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      To get an IPv4 SIP/DIP entry, valid index is 2N.
 *      To get an IPv4 SIP+DIP entry, valid index is 2N.
 *      To get an IPv6 SIP/DIP entry, valid index is 3N.
 *      To get an IPv6 SIP+DIP entry, valid index is 6N.
 */
int32
dal_mango_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry)
{
    int32   ret;
    dal_mango_of_l3CamEntry_t dal_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    entry_idx += RTK_DEFAULT_L3_OPENFLOW_CUTLINE;

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_L3_ROUTE(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((entry_idx % 2) != 0 && (entry_idx % 3) != 0 && (entry_idx % 6) != 0), RT_ERR_ENTRY_INDEX);

    if ((ret = _dal_mango_of_l3CamFlowEntry_get(unit, entry_idx, &dal_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (!dal_entry.valid)
        return RT_ERR_ENTRY_NOTFOUND;

    DAL_MANGO_OF_L3CAM_IDX_CHK(dal_entry, entry_idx);

    if ((ret = _dal_mango_of_l3CamEntryDal2Rtk_convert(unit, pEntry, &dal_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamFlowEntry_get */

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_add
 * Description:
 *      Add a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VALID_ENTRY_EXIST- Valid entry is existed
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 *      (4) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 */
int32
dal_mango_of_l3CamFlowEntry_add(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry)
{
    int32   ret;
    uint32  i, used_entry_num = 0;
    dal_mango_of_l3CamEntry_t raw_entry, tmp_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    entry_idx += RTK_DEFAULT_L3_OPENFLOW_CUTLINE;

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_L3_ROUTE(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((entry_idx % 2) != 0 && (entry_idx % 3) != 0 && (entry_idx % 6) != 0), RT_ERR_ENTRY_INDEX);

    if ((ret = _dal_mango_of_l3CamEntryRtk2Dal_convert(unit, &raw_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    DAL_MANGO_OF_L3CAM_IDX_CHK(raw_entry, entry_idx);

    if ((pEntry->flags & RTK_OF_FLAG_FLOWENTRY_REPLACE) == 0)
    {
        /* check whether the entry index(s) which are going to be used are all vacant */
        if (raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP4)
            used_entry_num = 2;
        else if (raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP6 &&
                (raw_entry.type == MANGO_OF_L3ENTRY_TYPE_SIP || raw_entry.type == MANGO_OF_L3ENTRY_TYPE_DIP))
            used_entry_num = 3;
        else if (raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP6 && raw_entry.type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP)
            used_entry_num = 6;

        for (i=0; i < used_entry_num; i++)
        {
            if ((ret = _dal_mango_of_l3CamFlowEntry_get(unit, entry_idx+i, &tmp_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if (tmp_entry.valid)
                return RT_ERR_VALID_ENTRY_EXIST;

        }
    }

    if ((ret = _dal_mango_of_l3EntryResource_alloc(unit, &raw_entry.ins, pEntry->ins.wa_data.set_field_0_data.mac,\
                pEntry->ins.wa_data.set_field_1_data.mac, &pEntry->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l3CamFlowEntry_set(unit, entry_idx, &raw_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamFlowEntry_add */

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_del
 * Description:
 *      Delete the specified L3 CAM-based flow entry.
 * Input:
 *      unit      - unit id
 *      entry_idx - physical entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      To delete an IPv4 SIP/DIP entry, valid index is 2N.
 *      To delete an IPv4 SIP+DIP entry, valid index is 2N.
 *      To delete an IPv6 SIP/DIP entry, valid index is 3N.
 *      To delete an IPv6 SIP+DIP entry, valid index is 6N.
 */
int32
dal_mango_of_l3CamFlowEntry_del(uint32 unit, uint32 entry_idx)
{
    int32   ret;
    dal_mango_of_l3CamEntry_t raw_entry, null_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    entry_idx += RTK_DEFAULT_L3_OPENFLOW_CUTLINE;

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_L3_ROUTE(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK(((entry_idx % 2) != 0 && (entry_idx % 3) != 0 && (entry_idx % 6) != 0), RT_ERR_ENTRY_INDEX);

    if ((ret = _dal_mango_of_l3CamFlowEntry_get(unit, entry_idx, &raw_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (!raw_entry.valid)
        return RT_ERR_OK;

    DAL_MANGO_OF_L3CAM_IDX_CHK(raw_entry, entry_idx);

    osal_memset((void *)&null_entry, 0x00, sizeof(dal_mango_of_l3CamEntry_t));
    if ((ret = _dal_mango_of_l3CamFlowEntry_set(unit, entry_idx, &null_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l3EntryResource_free(unit, &raw_entry.ins)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamFlowEntry_del */

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_move
 * Description:
 *      Move the specified L3 CAM-based flow entries.
 * Input:
 *      unit    - unit id
 *      pData   - movement info
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *		In addition to entry data, hit status is also moved.
 */
int32
dal_mango_of_l3CamFlowEntry_move(uint32 unit, rtk_of_flowMove_t *pData)
{
    int32   ret;
    uint32  field_data, value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pData->move_from += RTK_DEFAULT_L3_OPENFLOW_CUTLINE;
    pData->move_to += RTK_DEFAULT_L3_OPENFLOW_CUTLINE;
    RT_PARAM_CHK(pData->move_from == pData->move_to, RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_L3_ROUTE(unit) <= (pData->move_from + pData->length)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_L3_ROUTE(unit) <= (pData->move_to + pData->length)), RT_ERR_ENTRY_INDEX);

    if (pData->move_from > pData->move_to)
    {
        pData->move_from = DAL_MANGO_OF_ENTRY_IDX_TO_ADDR(pData->move_from);
        pData->move_to = DAL_MANGO_OF_ENTRY_IDX_TO_ADDR(pData->move_to);
    }
    else
    {
        pData->move_from = DAL_MANGO_OF_ENTRY_IDX_TO_ADDR(pData->move_from + pData->length - 1);
        pData->move_to = DAL_MANGO_OF_ENTRY_IDX_TO_ADDR(pData->move_to + pData->length - 1);
    }

    if ((ret = reg_field_set(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_FROMf,
            &pData->move_from, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_TOf,
            &pData->move_to, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_CMDf,
            &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf,
            &field_data, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_L3_ENTRY_MV_PARAMr, MANGO_LENf,
            &pData->length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_write(unit, MANGO_L3_ENTRY_MV_CTRLr, &value)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* wait until move action is completed */
    do {
        reg_field_read(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, &value);
        if (value == 0)
            break;
    } while(1);
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamFlowEntry_move */

/* Function Name:
 *      dal_mango_of_l3CamflowEntryHitSts_get
 * Description:
 *      Get the L3 Cam-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      (1) The hit status can be cleared by configuring parameter 'reset' to 1.
 *      (2) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.

 */
int32
dal_mango_of_l3CamflowEntryHitSts_get(
    uint32    unit,
    uint32    entry_idx,
    uint32    reset,
    uint32    *pIsHit)
{
    int32   ret;
    dal_mango_of_l3CamEntry_t dal_entry;

    entry_idx += RTK_DEFAULT_L3_OPENFLOW_CUTLINE;

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_L3_ROUTE(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((entry_idx % 2) != 0 && (entry_idx % 3) != 0 && (entry_idx % 6) != 0), RT_ERR_ENTRY_INDEX);

    if ((ret = _dal_mango_of_l3CamFlowEntry_get(unit, entry_idx, &dal_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (dal_entry.valid == FALSE)
        return RT_ERR_ENTRY_NOTFOUND;

    DAL_MANGO_OF_L3CAM_IDX_CHK(dal_entry, entry_idx);

    *pIsHit = dal_entry.hit;

    if (reset && dal_entry.hit)
    {
        dal_entry.hit = 0;/* 0 means clear the hit bit */
        if ((ret = _dal_mango_of_l3CamFlowEntry_set(unit, entry_idx, &dal_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3CamflowEntryHitSts_get */

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_get
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L3 flow entry is 12 bit width.
 */
int32
dal_mango_of_l3HashFlowEntry_get(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    int32   ret;
    dal_mango_of_l3HashEntry_t raw_entry;
    dal_mango_of_getResult_t result;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_of_l3HashEntryRtk2Dal_convert(unit, &raw_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l3HashEntry_getByMethod(unit, &raw_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (result.isExist == FALSE)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = _dal_mango_of_l3HashEntryDal2Rtk_convert(unit, pEntry, &raw_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* return the physical entry index */
    pEntry->ret_idx = result.idx;

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowEntry_get */

/* Function Name:
 *      dal_mango_of_l3HashFlowEntryNextValid_get
 * Description:
 *      Get next valid L3 Hash-based flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l3 hash-based flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Note:
 *      (1) Input -1 for getting the first valid entry of l3 hash-based flow table.
 *      (2) The pScan_idx is both the input and output argument.
 */
int32
dal_mango_of_l3HashFlowEntryNextValid_get(
    uint32                   unit,
    int32                    *pScan_idx,
    rtk_of_l3HashFlowEntry_t *pEntry)
{
    int32   ret;
    uint32  i;
    dal_mango_of_l3HashEntry_t raw_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScan_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((*pScan_idx != -1) && (*pScan_idx >= l3Sram_size[unit])), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    if (*pScan_idx >= l3Sram_size[unit])
        return RT_ERR_OUT_OF_RANGE;

    for (i = *pScan_idx; i < l3Sram_size[unit]; i++)
    {
        if ((ret = _dal_mango_of_l3HashFlowEntry_get(unit, i, &raw_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (raw_entry.valid && raw_entry.fmt == 1)
        {
            /* an IPv4 entry occuipes two physical index */
            if ((raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP4) && ((i%2) != 0))
                continue;

            /* an IPv6 SIP or DIP entry occuipes three physical index */
            if ((raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP6) &&
                (raw_entry.type == MANGO_OF_L3ENTRY_TYPE_SIP || raw_entry.type == MANGO_OF_L3ENTRY_TYPE_DIP) &&
                ((i%3) != 0))
                continue;

            /* an IPv6 SIP+DIP entry occuipes six physical index */
            if ((raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP6) &&
                (raw_entry.type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP) &&
                ((i%6) != 0))
                continue;

            *pScan_idx = i;

            if ((ret = _dal_mango_of_l3HashEntryDal2Rtk_convert(unit, pEntry, &raw_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_ENTRY_NOTFOUND;
}   /* end of dal_mango_of_l3HashFlowEntryNextValid_get */

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_add
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_EXIST  - entry existed
 *      RT_ERR_TBL_FULL     - The table is full
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries.
 *          An IPv4 SIP+DIP entry occupies two physical entries.
 *          An IPv6 SIP/DIP entry occupies three physical entries.
 *          An IPv6 SIP+DIP entry occupies six physical entries.
 *      (2) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *          The metadata in L3 flow entry is 12 bit width.
 *      (3) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 */
int32
dal_mango_of_l3HashFlowEntry_add(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    int32   ret;
    dal_mango_of_getResult_t result;
    dal_mango_of_l3HashEntry_t raw_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_of_l3HashEntryRtk2Dal_convert(unit, &raw_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* search existed */
    if ((ret = _dal_mango_of_l3HashEntry_getByMethod(unit, &raw_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((result.isExist == TRUE) && ((pEntry->flags & RTK_OF_FLAG_FLOWENTRY_REPLACE) == 0))
        return RT_ERR_ENTRY_EXIST;

    if ((result.isExist == TRUE) && (pEntry->flags & RTK_OF_FLAG_FLOWENTRY_REPLACE))
    {
        if ((ret = _dal_mango_of_l3EntryResource_free(unit, &raw_entry.ins)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    /* translate again because l3_entry may be overwrote by _dal_mango_of_l3HashFlowEntry_getByMethod */
    if ((ret = _dal_mango_of_l3HashEntryRtk2Dal_convert(unit, &raw_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* search free */
    if (result.isExist == FALSE)
    {
        if ((ret = _dal_mango_of_l3HashEntry_getByMethod(unit, &raw_entry, MANGO_OF_GETMETHOD_FREE, &result)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (result.isFree == FALSE)
            return RT_ERR_TBL_FULL;
    }

    if ((ret = _dal_mango_of_l3EntryResource_alloc(unit, &raw_entry.ins, pEntry->ins.wa_data.set_field_0_data.mac,\
                pEntry->ins.wa_data.set_field_1_data.mac, &pEntry->ins.wa_data.output_data.portmask)) != RT_ERR_OK)
    {
        /* free the allocated L3 shadow */
        if ((ret = _dal_mango_l3_hostEntry_free(unit, result.idx, DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l3HashFlowEntry_set(unit, result.idx, &raw_entry)) != RT_ERR_OK)
    {
        /* free the allocated L3 shadow */
        if ((ret = _dal_mango_l3_hostEntry_free(unit, result.idx, DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowEntry_add */

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_del
 * Description:
 *      Delete a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      None
 */
int32
dal_mango_of_l3HashFlowEntry_del(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    int32 ret;
    dal_mango_of_getResult_t result;
    dal_mango_of_l3HashEntry_t raw_entry, null_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_L3_FLOW_ENTRY_TYPE_END <= pEntry->type), RT_ERR_INPUT);

    if ((ret = _dal_mango_of_l3HashEntryRtk2Dal_convert(unit, &raw_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l3HashEntry_getByMethod(unit, &raw_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (result.isExist)
    {
        osal_memset((void *)&null_entry, 0x00, sizeof(dal_mango_of_l3HashEntry_t));

        if ((ret = _dal_mango_of_l3HashFlowEntry_set(unit, result.idx, &null_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = _dal_mango_of_l3EntryResource_free(unit, &raw_entry.ins)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if ((ret = _dal_mango_l3_hostEntry_free(unit, result.idx, DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }
    else
        return RT_ERR_ENTRY_NOTFOUND;

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowEntry_del */

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_delAll
 * Description:
 *      Delete all L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_of_l3HashFlowEntry_delAll(uint32 unit)
{
    int32   ret;
    uint32  i;
    dal_mango_of_l3HashEntry_t raw_entry, null_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    osal_memset((void *)&null_entry, 0x00, sizeof(dal_mango_of_l3HashEntry_t));

    for (i = 0; i < l3Sram_size[unit]; i++)
    {
        osal_memset((void *)&raw_entry, 0x00, sizeof(dal_mango_of_l3HashEntry_t));

        if ((ret = _dal_mango_of_l3HashFlowEntry_get(unit, i, &raw_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }

        if (raw_entry.valid && raw_entry.fmt == 1)
        {
            if ((ret = _dal_mango_of_l3HashFlowEntry_set(unit, i, &null_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            if ((ret = _dal_mango_of_l3EntryResource_free(unit, &raw_entry.ins)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            /* free the allocated L3 shadow */
            if ((ret = _dal_mango_l3_hostEntry_free(unit, i, DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
                return ret;
            }

            /* an IPv4 entry occuipes two physical index */
            if (raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP4)
            {
                i+=1;
                continue;
            }

            /* an IPv6 SIP or DIP entry occuipes three physical index */
            if ((raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP6) &&
                (raw_entry.type == MANGO_OF_L3ENTRY_TYPE_SIP || raw_entry.type == MANGO_OF_L3ENTRY_TYPE_DIP))
            {
                i+=2;
                continue;
            }

            /* an IPv6 SIP+DIP entry occuipes six physical index */
            if ((raw_entry.ip_ver == MANGO_OF_L3_IPVER_IP6) &&
                (raw_entry.type == MANGO_OF_L3ENTRY_TYPE_SIP_DIP))
            {
                i+=5;
                continue;
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashFlowEntry_delAll */

/* Function Name:
 *      dal_mango_of_l3HashflowEntryHitSts_get
 * Description:
 *      Get the L3 Hash-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 */
int32
dal_mango_of_l3HashflowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l3HashFlowEntry_t *pEntry,
    uint32                   reset,
    uint32                   *pIsHit)
{
    int32 ret;
    dal_mango_of_getResult_t result;
    dal_mango_of_l3HashEntry_t raw_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, reset=%d", unit, reset);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_of_l3HashEntryRtk2Dal_convert(unit, &raw_entry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_l3HashEntry_getByMethod(unit, &raw_entry, MANGO_OF_GETMETHOD_EXIST, &result)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if (raw_entry.valid == FALSE)
        return RT_ERR_ENTRY_NOTFOUND;

    *pIsHit = raw_entry.hit;

    if (reset && raw_entry.hit)
    {
        raw_entry.hit = 0;/* 0 means clear the hit bit */
        if ((ret = _dal_mango_of_l3HashFlowEntry_set(unit, result.idx, &raw_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_l3HashflowEntryHitSts_get */

/* Function Name:
 *      dal_mango_of_groupEntry_get
 * Description:
 *      Get a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type OF_GROUP_TYPE_INDIRECT, 'bucket_num' is not used.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.
 */
int32
dal_mango_of_groupEntry_get(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry)
{
    int32           ret;
    uint32          val = 0;
    of_grp_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_GRP_ENTRY(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = table_read(unit, MANGO_GROUP_TBLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = table_field_get(unit, MANGO_GROUP_TBLt, MANGO_GROUP_TBL_TYPEtf, &val,\
                                (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    switch (val)
    {
        case 0:
            pEntry->type = OF_GROUP_TYPE_ALL;
            break;
        case 1:
            pEntry->type = OF_GROUP_TYPE_SELECT;
            break;
        default:
            break;
    }

    if ((ret = table_field_get(unit, MANGO_GROUP_TBLt, MANGO_GROUP_TBL_BUCKET_NUMtf, &val,\
                                (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    pEntry->bucket_num = val + 1;/*driver is 1-based*/

    if ((ret = table_field_get(unit, MANGO_GROUP_TBLt, MANGO_GROUP_TBL_BUCKET_IDtf, &pEntry->bucket_id,\
                                (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_groupEntry_get */

/* Function Name:
 *      dal_mango_of_groupEntry_set
 * Description:
 *      Set a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type INDIRECT, use OF_GROUP_TYPE_ALL with bucket_num=1.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.  Refer to rtk_of_actionBucket_set.
 */
int32
dal_mango_of_groupEntry_set(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry)
{
    int32           ret;
    uint32          val = 0;
    of_grp_entry_t  entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_GRP_ENTRY(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((OF_GROUP_TYPE_END <= pEntry->type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_GRP_ENTRY_BUCKET(unit) < pEntry->bucket_num) || (pEntry->bucket_num == 0), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_ACTION_BUCKET(unit) <= pEntry->bucket_id), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_ACTION_BUCKET(unit) <= (pEntry->bucket_id + pEntry->bucket_num)), RT_ERR_INPUT);

    switch (pEntry->type)
    {
        case OF_GROUP_TYPE_ALL:
            val = 0;
            break;
        case OF_GROUP_TYPE_SELECT:
            val = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    osal_memset(&entry, 0x00, sizeof(of_grp_entry_t));

    if ((ret = table_field_set(unit, MANGO_GROUP_TBLt, MANGO_GROUP_TBL_TYPEtf, &val,\
                                (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    val = pEntry->bucket_num - 1;/*ASIC is 0-based*/
    if ((ret = table_field_set(unit, MANGO_GROUP_TBLt, MANGO_GROUP_TBL_BUCKET_NUMtf, &val,\
                                (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_GROUP_TBLt, MANGO_GROUP_TBL_BUCKET_IDtf, &pEntry->bucket_id,\
                                (uint32 *)&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = table_write(unit, MANGO_GROUP_TBLt, entry_idx, (uint32 *)&entry)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_groupEntry_set */

/* Function Name:
 *      dal_mango_of_groupTblHashPara_get
 * Description:
 *      Get parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 * Output:
 *      para    - pointer buffer of hash parameters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Each parameter can be included/excluded separately.
 */
int32
dal_mango_of_groupTblHashPara_get(uint32 unit, rtk_of_groupTblHashPara_t *para)
{
    int32   ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == para), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_read(unit, MANGO_OF_GRP_HASH_CTRLr, &val) ) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_IGR_PORT_INCf, &para->igr_port,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_SMAC_INCf, &para->smac,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_DMAC_INCf, &para->dmac,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_SIP_INCf, &para->sip,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_DIP_INCf, &para->dip,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_L4_SPORT_INCf, &para->l4_sport,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_L4_DPORT_INCf, &para->l4_dport,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_groupTblHashPara_get */

/* Function Name:
 *      dal_mango_of_groupTblHashPara_set
 * Description:
 *      Set parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 *      para    - pointer buffer of hash parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Each parameter can be included/excluded separately.
 */
int32
dal_mango_of_groupTblHashPara_set(uint32 unit, rtk_of_groupTblHashPara_t *para)
{
    int32   ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == para), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_IGR_PORT_INCf, &para->igr_port,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_SMAC_INCf, &para->smac,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_DMAC_INCf, &para->dmac,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_SIP_INCf, &para->sip,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_DIP_INCf, &para->dip,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_L4_SPORT_INCf, &para->l4_sport,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_OF_GRP_HASH_CTRLr, MANGO_L4_DPORT_INCf, &para->l4_dport,\
                &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_write(unit, MANGO_OF_GRP_HASH_CTRLr, &val) ) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_groupTblHashPara_set */

/* Function Name:
 *      dal_mango_of_actionBucket_get
 * Description:
 *      Get a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 */
int32
dal_mango_of_actionBucket_get(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry)
{
    int32   ret;
    dal_mango_of_actBucket_t dal_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_ACTION_BUCKET(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    if ((ret = _dal_mango_of_actBucket_get(unit, entry_idx, &dal_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_actBucketDal2Rtk_convert(unit, pEntry, &dal_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_actionBucket_get */

/* Function Name:
 *      dal_mango_of_actionBucket_set
 * Description:
 *      Set a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 */
int32
dal_mango_of_actionBucket_set(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry)
{
    int32   ret;
    dal_mango_of_actBucket_t rawEntry, orgRawEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_OF_MAX_NUM_OF_ACTION_BUCKET(unit) <= entry_idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* convert RTK to DAL structure */
    if ((ret = _dal_mango_of_actBucketRtk2Dal_convert(unit, &rawEntry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* get existed entry to free the allocated resources */
    if ((ret = _dal_mango_of_actBucket_get(unit, entry_idx, &orgRawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_actBucketResource_free(unit, &orgRawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    /* allocated new resources */
    if ((ret = _dal_mango_of_actBucketResource_alloc(unit, &rawEntry, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    if ((ret = _dal_mango_of_actBucket_set(unit, entry_idx, &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_actionBucket_set */

/* Function Name:
 *      dal_mango_of_trapTarget_get
 * Description:
 *      Get target device for trap packet.
 * Input:
 *      unit	- unit id
 * Output:
 *      pTarget - pointer to target device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 */
int32
dal_mango_of_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_OF_ACT_CTRLr, MANGO_TRAP_TARGETf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pTarget = RTK_TRAP_LOCAL;
            break;
        case 1:
            *pTarget = RTK_TRAP_MASTER;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pTarget=%d", *pTarget);

    return RT_ERR_OK;
}   /* end of dal_mango_of_trapTarget_get */

/* Function Name:
 *      dal_mango_of_trapTarget_set
 * Description:
 *      Set target device for trap packet.
 * Input:
 *      unit	- unit id
 *      target 	- target device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 */
int32
dal_mango_of_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, target=%d", unit, target);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_TRAP_END <= target), RT_ERR_INPUT);

    switch (target)
    {
        case RTK_TRAP_LOCAL:
            val = 0;
            break;
        case RTK_TRAP_MASTER:
            val = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    OF_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_OF_ACT_CTRLr, MANGO_TRAP_TARGETf, &val)) != RT_ERR_OK)
    {
        OF_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
        return ret;
    }
    OF_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_trapTarget_set */

/* Function Name:
 *      dal_mango_of_tblMissAction_get
 * Description:
 *      Get table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 * Output:
 *      pAct    - pointer to table miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *		None
 */
int32
dal_mango_of_tblMissAction_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t *pAct)
{
    int32   ret;
    uint32  val = 0, index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d", unit, phase);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            index = 0;
            break;
        case FT_PHASE_IGR_FT_1:
            index = 1;
            break;
        case FT_PHASE_IGR_FT_2:
            index = 2;
            break;
        case FT_PHASE_IGR_FT_3:
            index = 3;
            break;
        case FT_PHASE_EGR_FT_0:
            break;
        default:
            return RT_ERR_INPUT;
    }

    if (phase == FT_PHASE_EGR_FT_0)
    {
        OF_SEM_LOCK(unit);
        if ((ret = reg_field_read(unit, MANGO_OF_EGR_TBL_MISr, MANGO_ACTf, &val)) != RT_ERR_OK)
        {
            OF_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
        OF_SEM_UNLOCK(unit);

        switch (val)
        {
            case 0:
                *pAct = OF_TBLMISS_ACT_EXEC_ACTION_SET;
                break;
            case 1:
                *pAct = OF_TBLMISS_ACT_DROP;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        OF_SEM_LOCK(unit);
        if ((ret = reg_array_field_read(unit, MANGO_OF_IGR_TBL_MISr, REG_ARRAY_INDEX_NONE, index,\
            MANGO_ACTf, &val)) != RT_ERR_OK)
        {
            OF_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
        OF_SEM_UNLOCK(unit);

        switch (val)
        {
            case 0:
                *pAct = OF_TBLMISS_ACT_DROP;
                break;
            case 1:
                *pAct = OF_TBLMISS_ACT_TRAP;
                break;
            case 2:
                *pAct = OF_TBLMISS_ACT_FORWARD_NEXT_TBL;
                break;
            case 3:
                *pAct = OF_TBLMISS_ACT_EXEC_ACTION_SET;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pAct=%d", *pAct);

    return RT_ERR_OK;
}   /* end of dal_mango_of_tblMissAction_get */

/* Function Name:
 *      dal_mango_of_tblMissAction_set
 * Description:
 *      Set table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 *      act     - table miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *		(1) OF_TBLMISS_ACT_FORWARD_NEXT_TBL isn't applicable for the last ingress flow table.
 *		(2) OF_TBLMISS_ACT_TRAP and OF_TBLMISS_ACT_FORWARD_NEXT_TBL aren't applicable for the egress flow table.
 */
int32
dal_mango_of_tblMissAction_set(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t act)
{
    int32   ret;
    uint32  val = 0, index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, act=%d", unit, phase, act);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((OF_TBLMISS_ACT_END <= act), RT_ERR_INPUT);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            index = 0;
            break;
        case FT_PHASE_IGR_FT_1:
            index = 1;
            break;
        case FT_PHASE_IGR_FT_2:
            index = 2;
            break;
        case FT_PHASE_IGR_FT_3:
            index = 3;
            if(act == OF_TBLMISS_ACT_FORWARD_NEXT_TBL)
                return RT_ERR_INPUT;
            break;
        case FT_PHASE_EGR_FT_0:
            if(act == OF_TBLMISS_ACT_TRAP || act == OF_TBLMISS_ACT_FORWARD_NEXT_TBL)
                return RT_ERR_INPUT;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if (phase == FT_PHASE_EGR_FT_0)
    {
        switch (act)
        {
            case OF_TBLMISS_ACT_EXEC_ACTION_SET:
                val = 0;
                break;
            case OF_TBLMISS_ACT_DROP:
                val = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }

        OF_SEM_LOCK(unit);
        if ((ret = reg_field_write(unit, MANGO_OF_EGR_TBL_MISr, MANGO_ACTf, &val)) != RT_ERR_OK)
        {
            OF_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
        OF_SEM_UNLOCK(unit);
    }
    else
    {
        switch (act)
        {
            case OF_TBLMISS_ACT_DROP:
                val = 0;
                break;
            case OF_TBLMISS_ACT_TRAP:
                val = 1;
                break;
            case OF_TBLMISS_ACT_FORWARD_NEXT_TBL:
                val = 2;
                break;
            case OF_TBLMISS_ACT_EXEC_ACTION_SET:
                val = 3;
                break;
            default:
                return RT_ERR_INPUT;
        }

        OF_SEM_LOCK(unit);
        if ((ret = reg_array_field_write(unit, MANGO_OF_IGR_TBL_MISr, REG_ARRAY_INDEX_NONE, index,\
            MANGO_ACTf, &val)) != RT_ERR_OK)
        {
            OF_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
        OF_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}   /* end of dal_mango_of_tblMissAction_set */

/* Function Name:
 *      dal_mango_of_flowTblCnt_get
 * Description:
 *      Get lookup and matched packet counters of flow tables.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      type     - counter type
 * Output:
 *      pCnt     - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The counter is cleared after reading.
 */
int32
dal_mango_of_flowTblCnt_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowTblCntType_t type, uint32 *pCnt)
{
    int32   ret;
    uint32  field, index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "unit=%d, phase=%d, type=%d", unit, phase, type);

    /* check Init status */
    RT_INIT_CHK(of_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((FT_PHASE_END <= phase), RT_ERR_OF_FT_PHASE);
    RT_PARAM_CHK((OF_FLOW_TBL_CNT_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case FT_PHASE_IGR_FT_0:
            index = 0;
            break;
        case FT_PHASE_IGR_FT_1:
            index = 1;
            break;
        case FT_PHASE_IGR_FT_2:
            index = 2;
            break;
        case FT_PHASE_IGR_FT_3:
            index = 3;
            break;
        case FT_PHASE_EGR_FT_0:
            break;
        default:
            return RT_ERR_INPUT;
    }

    if (type == OF_FLOW_TBL_CNT_TYPE_LOOKUP)
        field = MANGO_LOOKUPf;
    else if (type == OF_FLOW_TBL_CNT_TYPE_MATCH)
        field = MANGO_MATCHf;
    else
        return RT_ERR_INPUT;

    if (phase == FT_PHASE_EGR_FT_0)
    {
        OF_SEM_LOCK(unit);
        if ((ret = reg_field_read(unit, MANGO_OF_EGR_TBL_CNTRr, field, pCnt)) != RT_ERR_OK)
        {
            OF_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
        OF_SEM_UNLOCK(unit);
    }
    else
    {
        OF_SEM_LOCK(unit);
        if ((ret = reg_array_field_read(unit, MANGO_OF_IGR_TBL_CNTRr, REG_ARRAY_INDEX_NONE, index,\
                                        field, pCnt)) != RT_ERR_OK)
        {
            OF_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OPENFLOW), "");
            return ret;
        }
        OF_SEM_UNLOCK(unit);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OPENFLOW), "pCnt=%d", *pCnt);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowTblCnt_get */

