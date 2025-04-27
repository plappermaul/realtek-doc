/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public PIE APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Ingress PIE
 *            2) Egress PIE
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_pie.h>
#include <rtk/default.h>

/*
 * Symbol Definition
 */
#define DAL_MANGO_MAX_NUM_OF_PIE_BLOCK          32
#define DAL_MANGO_MAX_TEMPLATE_OF_BLK           2
#define DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD     14
#define DAL_MANGO_MAX_HITINDICATION_BLK_ENTRY   32
#define DAL_MANGO_MAX_BLK_RTL9311E_DIV          2
#define DAL_MANGO_BURST_BYTE_MODE_UNIT          128

/*
 * Data Declaration
 */
static uint32           pie_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t     pie_sem[RTK_MAX_NUM_OF_UNIT];

static rtk_pie_phase_t  pie_blk_type[DAL_MANGO_MAX_NUM_OF_PIE_BLOCK];
static uint32           pie_blk_template[DAL_MANGO_MAX_NUM_OF_PIE_BLOCK][DAL_MANGO_MAX_TEMPLATE_OF_BLK];

typedef struct dal_mango_pie_templateField_s
{
    rtk_pie_templateFieldType_t     field_type;     /* field type in logical */
    uint32                          physical_id;    /* physical field ID in ASIC */
    uint32                          valid_location; /* valid field location, 0 means no limitation */
} dal_mango_pie_templateField_t;

rtk_pie_template_t dal_mango_pie_template[] =
{
    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_SMAC0, TMPLTE_FIELD_SMAC1, TMPLTE_FIELD_SMAC2,
        TMPLTE_FIELD_VLAN, TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_DSAP_SSAP,
        TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1,
        TMPLTE_FIELD_SPM2, TMPLTE_FIELD_SPM3}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0,
        TMPLTE_FIELD_DIP1, TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_TCP_INFO,
        TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_VLAN,
        TMPLTE_FIELD_RANGE_CHK, TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1,
        TMPLTE_FIELD_SPM2, TMPLTE_FIELD_SPM3}},

    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_VLAN, TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0,
        TMPLTE_FIELD_DIP1, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT,
        TMPLTE_FIELD_META_DATA, TMPLTE_FIELD_SLP}},

    {{TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1, TMPLTE_FIELD_DIP2,
        TMPLTE_FIELD_DIP3, TMPLTE_FIELD_DIP4, TMPLTE_FIELD_DIP5,
        TMPLTE_FIELD_DIP6, TMPLTE_FIELD_DIP7, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_TCP_INFO, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT,
        TMPLTE_FIELD_RANGE_CHK, TMPLTE_FIELD_SLP}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_SIP2,
        TMPLTE_FIELD_SIP3, TMPLTE_FIELD_SIP4, TMPLTE_FIELD_SIP5,
        TMPLTE_FIELD_SIP6, TMPLTE_FIELD_SIP7, TMPLTE_FIELD_META_DATA,
        TMPLTE_FIELD_VLAN, TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1,
        TMPLTE_FIELD_SPM2, TMPLTE_FIELD_SPM3}},

    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
};

dal_mango_pie_templateField_t dal_mango_template_common_field_list[] =
{
    {   /* field type       */      TMPLTE_FIELD_NONE,
        /* physical field ID*/      0,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC0,
        /* physical field ID*/      9,
        /* valid field location */  ((1 << 0) | (1 << 3) | (1 << 6) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC1,
        /* physical field ID*/      10,
        /* valid field location */  ((1 << 1) | (1 << 4) | (1 << 7) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC2,
        /* physical field ID*/      11,
        /* valid field location */  ((1 << 2) | (1 << 5) | (1 << 8) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC0,
        /* physical field ID*/      12,
        /* valid field location */  ((1 << 0) | (1 << 3) | (1 << 6) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC1,
        /* physical field ID*/      13,
        /* valid field location */  ((1 << 1) | (1 << 4) | (1 << 7) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC2,
        /* physical field ID*/      14,
        /* valid field location */  ((1 << 2) | (1 << 5) | (1 << 8) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_ETHERTYPE,
        /* physical field ID*/      15,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_OTAG,
        /* physical field ID*/      16,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ITAG,
        /* physical field ID*/      17,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SIP0,
        /* physical field ID*/      18,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10) | (1 << 12))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP1,
        /* physical field ID*/      19,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP0,
        /* physical field ID*/      20,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10) | (1 << 12))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP1,
        /* physical field ID*/      21,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13))
    },
    {   /* field type       */      TMPLTE_FIELD_IP_TOS_PROTO,
        /* physical field ID*/      22,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L4_SPORT,
        /* physical field ID*/      23,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10) | (1 << 12))
    },
    {   /* field type       */      TMPLTE_FIELD_L4_DPORT,
        /* physical field ID*/      24,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13))
    },
    {   /* field type       */      TMPLTE_FIELD_L34_HEADER,
        /* physical field ID*/      25,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_TCP_INFO,
        /* physical field ID*/      26,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP2,
        /* physical field ID*/      34,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP3,
        /* physical field ID*/      35,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP4,
        /* physical field ID*/      36,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP5,
        /* physical field ID*/      37,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP6,
        /* physical field ID*/      38,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP7,
        /* physical field ID*/      39,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_FLOW_LABEL,
        /* physical field ID*/      49,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_FWD_VID,
        /* physical field ID*/      52,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_RANGE_CHK,
        /* physical field ID*/      53,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SLP,
        /* physical field ID*/      55,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DLP,
        /* physical field ID*/      56,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_META_DATA,
        /* physical field ID*/      57,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIRST_MPLS1,
        /* physical field ID*/      60,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10) | (1 << 12))
    },
    {   /* field type       */      TMPLTE_FIELD_FIRST_MPLS2,
        /* physical field ID*/      61,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13))
    },
    {   /* field type       */      TMPLTE_FIELD_DPM3,
        /* physical field ID*/      8,
        /* valid field location */  (1 << 13)
    },
    {   /* field type       */      TMPLTE_FIELD_END,
        /* physical field ID*/      0xFF,
        /* valid field location */  0
    },
};

dal_mango_pie_templateField_t dal_mango_template_vi_field_list[] =
{
    {   /* field type       */      TMPLTE_FIELD_SPM0,
        /* physical field ID*/      1,
        /* valid field location */  (1 << 10)
    },
    {   /* field type       */      TMPLTE_FIELD_SPM1,
        /* physical field ID*/      2,
        /* valid field location */  (1 << 11)
    },
    {   /* field type       */      TMPLTE_FIELD_SPM2,
        /* physical field ID*/      3,
        /* valid field location */  (1 << 12)
    },
    {   /* field type       */      TMPLTE_FIELD_SPM3,
        /* physical field ID*/      4,
        /* valid field location */  (1 << 13)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* physical field ID*/      27,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* physical field ID*/      28,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* physical field ID*/      29,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* physical field ID*/      30,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* physical field ID*/      31,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* physical field ID*/      32,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* physical field ID*/      33,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* physical field ID*/      34,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* physical field ID*/      35,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* physical field ID*/      36,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* physical field ID*/      37,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* physical field ID*/      38,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* physical field ID*/      39,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_12,
        /* physical field ID*/      40,
        /* valid field location */  (1 << 8)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_13,
        /* physical field ID*/      41,
        /* valid field location */  (1 << 9)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP2,
        /* physical field ID*/      42,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP3,
        /* physical field ID*/      43,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP4,
        /* physical field ID*/      44,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP5,
        /* physical field ID*/      45,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP6,
        /* physical field ID*/      46,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP7,
        /* physical field ID*/      47,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_PKT_INFO,
        /* physical field ID*/      48,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DSAP_SSAP,
        /* physical field ID*/      50,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_SNAP_OUI,
        /* physical field ID*/      51,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_VLAN_GMSK,
        /* physical field ID*/      54,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_TTID,
        /* physical field ID*/      58,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10) | (1 << 12))
    },
    {   /* field type       */      TMPLTE_FIELD_END,
        /* physical field ID*/      0xFF,
        /* valid field location */  0
    },
};

dal_mango_pie_templateField_t dal_mango_template_i_field_list[] =
{
    {   /* field type       */      TMPLTE_FIELD_TSID,
        /* physical field ID*/      59,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13))
    },
    {   /* field type       */      TMPLTE_FIELD_END,
        /* physical field ID*/      0xFF,
        /* valid field location */  0
    },
};

dal_mango_pie_templateField_t dal_mango_template_e_field_list[] =
{
    {   /* field type       */      TMPLTE_FIELD_DPM0,
        /* physical field ID*/      5,
        /* valid field location */  (1 << 10)
    },
    {   /* field type       */      TMPLTE_FIELD_DPM1,
        /* physical field ID*/      6,
        /* valid field location */  (1 << 11)
    },
    {   /* field type       */      TMPLTE_FIELD_DPM2,
        /* physical field ID*/      7,
        /* valid field location */  (1 << 12)
    },
    {   /* field type       */      TMPLTE_FIELD_TSID,
        /* physical field ID*/      59,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13))
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
#define PIE_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(pie_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_PIE), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define PIE_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(pie_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_PIE), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

/*
 * Transfer physical template field ID to logical template field ID
 */

/*
 * Validate fields and transfer logical field ID to physical field ID
 */
static int32
_dal_mango_pie_log2PhyTmplteField(uint32 unit, uint32 field_idx,
    rtk_pie_templateFieldType_t log_field_id, uint32 *phy_field_id)
{
    uint32  i;

    RT_PARAM_CHK((field_idx >= HAL_MAX_NUM_OF_PIE_TEMPLATE_FIELD(unit)), RT_ERR_ENTRY_INDEX);

    /* Check common field type */
    for (i = 0; dal_mango_template_common_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
    {
        if (log_field_id == dal_mango_template_common_field_list[i].field_type)
        {
            /* Check field location */
            if ((dal_mango_template_common_field_list[i].valid_location != 0) &&
                    ((dal_mango_template_common_field_list[i].valid_location &
                    (1 << field_idx)) == 0))
            {
                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "template field %d invalid location",
                        log_field_id);
                return RT_ERR_PIE_FIELD_LOCATION;
            }

            *phy_field_id = dal_mango_template_common_field_list[i].physical_id;
            return RT_ERR_OK;
        }
    }

    /* Check VACL & IACL field type */
    for (i = 0; dal_mango_template_vi_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
    {
        if (log_field_id == dal_mango_template_vi_field_list[i].field_type)
        {
            /* Check field location */
            if ((dal_mango_template_vi_field_list[i].valid_location != 0) &&
                    ((dal_mango_template_vi_field_list[i].valid_location &
                    (1 << field_idx)) == 0))
            {
                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "template field %d invalid location",
                        log_field_id);
                return RT_ERR_PIE_FIELD_LOCATION;
            }

            *phy_field_id = dal_mango_template_vi_field_list[i].physical_id;
            return RT_ERR_OK;
        }
    }

    /* Check IACL field type */
    for (i = 0; dal_mango_template_i_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
    {
        if (log_field_id == dal_mango_template_i_field_list[i].field_type)
        {
            /* Check field location */
            if ((dal_mango_template_i_field_list[i].valid_location != 0) &&
                    ((dal_mango_template_i_field_list[i].valid_location &
                    (1 << field_idx)) == 0))
            {
                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "template field %d invalid location",
                        log_field_id);
                return RT_ERR_PIE_FIELD_LOCATION;
            }

            *phy_field_id = dal_mango_template_i_field_list[i].physical_id;
            return RT_ERR_OK;
        }
    }

    /* Check EACL field type */
    for (i = 0; dal_mango_template_e_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
    {
        if (log_field_id == dal_mango_template_e_field_list[i].field_type)
        {
            /* Check field location */
            if ((dal_mango_template_e_field_list[i].valid_location != 0) &&
                    ((dal_mango_template_e_field_list[i].valid_location &
                    (1 << field_idx)) == 0))
            {
                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "template field %d invalid location",
                        log_field_id);
                return RT_ERR_PIE_FIELD_LOCATION;
            }

            *phy_field_id = dal_mango_template_e_field_list[i].physical_id;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_PIE_FIELD_TYPE;
}   /* end of _dal_mango_pie_log2PhyTmplteField */

/*
 * Get PIE entry hit status.
 */
int32
_dal_mango_pie_entryHitSts_get(
    uint32                   unit,
    rtk_pie_id_t             entry_idx,
    uint32                   reset,
    uint32                   *pIsHit)
{
    int32   ret;
    uint32  grpIdx, offset, grpData;

    grpIdx = entry_idx / 32;
    offset = entry_idx % 32;

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_PIE_RULE_HIT_INDICATIONr,
            REG_ARRAY_INDEX_NONE, grpIdx, MANGO_RULE_INDICATIONf,
            &grpData)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((grpData & (1<<offset)) != 0)
        *pIsHit = 1;
    else
        *pIsHit = 0;

    if(TRUE == reset && *pIsHit == 1)
    {
        /* reset the hit bit */
        grpData = (1<<offset);
        if ((ret = reg_array_field_write(unit, MANGO_PIE_RULE_HIT_INDICATIONr,
                REG_ARRAY_INDEX_NONE, grpIdx, MANGO_RULE_INDICATIONf,
                &grpData)) != RT_ERR_OK)
        {
            PIE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_of_flowEntryHitSts_get */

/*
 * Delete PIE entries by physical index..
 */
int32
_dal_mango_pie_del(
    uint32 unit,
    uint32 start_idx,
    uint32 end_idx)
{
    acl_entry_t entry;
    int32   ret;
    uint32  value = 0, field_data;

    PIE_SEM_LOCK(unit);

    if ((ret = table_read(unit, MANGO_VACLt, 0, (uint32 *)&entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_PIE_CLR_CTRLr, MANGO_CLR_FROMf,
            &start_idx, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_PIE_CLR_CTRLr, MANGO_CLR_TOf,
            &end_idx, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, MANGO_PIE_CLR_CTRLr, MANGO_CLRf,
            &field_data, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_write(unit, MANGO_PIE_CLR_CTRLr, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* wait until clear action is completed */
    do {
        reg_field_read(unit, MANGO_PIE_CLR_CTRLr, MANGO_CLRf, &value);
        if (value == 0)
            break;
    } while(1);

    PIE_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}   /* end of _dal_mango_pie_del */

/*
 * Delete PIE entries by PIE phase and logical index.
 */
int32
_dal_mango_pie_entry_del(
    uint32          unit,
    rtk_pie_phase_t phase,
    uint32          start_idx,
    uint32          end_idx)
{
    int32  ret;
    uint32 idx, i, hit;
    uint32 phy_start_idx, phy_end_idx;

    if (PIE_ENTRY_IS_PHYSICAL_TYPE(start_idx))
    {
        start_idx = PIE_ENTRY_PHYSICAL_CLEAR(start_idx);
        end_idx = PIE_ENTRY_PHYSICAL_CLEAR(end_idx);
        if ((ret = _dal_mango_pie_del(unit, start_idx, end_idx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }

        for (i = start_idx; i <= end_idx; ++i)
        {
            if ((ret = _dal_mango_pie_entryHitSts_get(unit, i, TRUE, (uint32 *)&hit)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                return ret;
            }
        }
    }
    else
    {
        DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, start_idx, phy_start_idx);
        DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, end_idx, phy_start_idx);

        /* per physical block del entry */
        i = (start_idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

        idx = start_idx;

        do
        {
            /* start */
            DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, idx, phy_start_idx);

            /* end */
            i += HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

            if (i > end_idx)
                idx = end_idx;
            else
                idx = i - 1;

            DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, idx, phy_end_idx);

            if ((ret = _dal_mango_pie_del(unit, phy_start_idx, phy_end_idx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                return ret;
            }

            idx = i;
        } while (i <= end_idx);

        /* clear hit indication */
        for (i = start_idx; i <= end_idx; ++i)
        {
            DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, i, idx);

            if ((ret = _dal_mango_pie_entryHitSts_get(unit, idx, TRUE, (uint32 *)&hit)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}

/*
 * Move PIE entries by physical index.
 */
int32
_dal_mango_pie_move(
    uint32  unit,
    uint32  length,
    uint32  move_from,
    uint32  move_to)
{
    acl_entry_t entry;
    int32   ret;
    uint32  value = 0, field_data;

    PIE_SEM_LOCK(unit);

    if ((ret = table_read(unit, MANGO_VACLt, 0, (uint32 *)&entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_PIE_MV_CTRLr, MANGO_MV_FROMf,
            &move_from, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_PIE_MV_CTRLr, MANGO_MV_TOf,
            &move_to, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_PIE_MV_LEN_CTRLr, MANGO_MV_LENf,
            &length)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    field_data = 1;
    if ((ret = reg_field_set(unit, MANGO_PIE_MV_CTRLr, MANGO_MVf,
            &field_data, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_write(unit, MANGO_PIE_MV_CTRLr, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* wait until move action is completed */
    do {
        reg_field_read(unit, MANGO_PIE_MV_CTRLr, MANGO_MVf, &value);
        if (value == 0)
            break;
    } while(1);
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of _dal_mango_pie_move */

/*
 * Move PIE entries by PIE phase and logical index.
 */
int32
_dal_mango_pie_entry_move(
    uint32          unit,
    rtk_pie_phase_t phase,
    uint32          length,
    uint32          move_from,
    uint32          move_to)
{
    int32 ret;
    uint32 srcLen, srcBlkStart, srcBlkEnd, srcBlkLen, srcMvLen;
    uint32 dstBlkStart, dstBlkEnd, dstBlkLen;
    uint32 phy_length, phy_move_from, phy_move_to;

    if (PIE_ENTRY_IS_PHYSICAL_TYPE(move_from))
    {
        move_from = PIE_ENTRY_PHYSICAL_CLEAR(move_from);
        move_to = PIE_ENTRY_PHYSICAL_CLEAR(move_to);
        if ((ret = _dal_mango_pie_move(unit, length, move_from, move_to)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
    }
    else
    {
        /* check move_from entries and move_to entries are valid */
        DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, move_from, phy_move_from);
        DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, move_from + length - 1, phy_move_from);
        DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, move_to, phy_move_from);
        DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, move_to + length - 1, phy_move_from);

        /* per source physical block move to each destination physical block */
        if (move_from > move_to)
        {
            srcBlkStart = move_from;
            srcLen = length;
            dstBlkStart = move_to;

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

                    DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, srcBlkStart, phy_move_from);
                    DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, dstBlkStart, phy_move_to);

                    if ((ret = _dal_mango_pie_move(unit, phy_length, phy_move_from, phy_move_to)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                        return ret;
                    }

                    dstBlkStart += phy_length;
                    srcBlkStart += phy_length;
                    srcMvLen -= phy_length;
                    srcLen -= phy_length;
                } while (srcMvLen > 0);
            } while (srcLen > 0);
        }
        else if (move_from < move_to)
        {
            srcBlkEnd = move_from + length - 1;
            srcLen = length;
            dstBlkEnd = move_to + length - 1;

            do
            {
                srcBlkStart = (srcBlkEnd / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                        HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
                if (srcBlkStart < move_from)
                    srcBlkStart = move_from;

                srcBlkLen = srcBlkEnd - srcBlkStart + 1;
                if (srcBlkLen > srcLen)
                    srcMvLen = srcLen;
                else
                    srcMvLen = srcBlkLen;

                do {
                    dstBlkStart = (dstBlkEnd / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) *
                            HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
                    if (dstBlkStart < move_to)
                        dstBlkStart = move_to;

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

                    DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, srcBlkStart, phy_move_from);
                    DAL_MANGO_PIE_INDEX_TO_PHYSICAL(unit, phase, dstBlkStart, phy_move_to);

                    if ((ret = _dal_mango_pie_move(unit, phy_length, phy_move_from, phy_move_to)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                        return ret;
                    }

                    dstBlkEnd -= phy_length;
                    srcBlkEnd -= phy_length;
                    srcMvLen -= phy_length;
                    srcLen -= phy_length;
                } while (srcMvLen > 0);
            } while (srcLen > 0);
        }
    }

    return RT_ERR_OK;
}

/*
 * Get the mapping template of specific PIE block.
 */
int32
_dal_mango_pie_templateSelector_get(
    uint32 unit,
    uint32 block_idx,
    uint32 *template0_id,
    uint32 *template1_id)
{
    PIE_SEM_LOCK(unit);

    *template0_id = pie_blk_template[block_idx][0];
    *template1_id = pie_blk_template[block_idx][1];

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/*
 * Set the mapping template of specific PIE block.
 */
int32
_dal_mango_pie_templateSelector_set(
    uint32 unit,
    uint32 block_idx,
    uint32 template0_id,
    uint32 template1_id)
{
    int32   ret;

    PIE_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_PIE_BLK_TMPLTE_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_BLK_TMPLTE1f,
            &template0_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    pie_blk_template[block_idx][0] = template0_id;

    if ((ret = reg_array_field_write(unit, MANGO_PIE_BLK_TMPLTE_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_BLK_TMPLTE2f,
            &template1_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    pie_blk_template[block_idx][1] = template1_id;

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_pie_phase_get
 * Description:
 *      Get the PIE block phase.
 * Input:
 *      unit       - unit id
 *      block_idx - block index
 * Output:
 *      pPhase - pointer buffer of phase value
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
dal_mango_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase)
{
    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pPhase, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK(block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV), RT_ERR_PIE_BLOCK_INDEX);
    }

    PIE_SEM_LOCK(unit);

    *pPhase = pie_blk_type[block_idx];

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_phase_get */

/* Function Name:
 *      dal_mango_pie_phase_set
 * Description:
 *      Set the PIE block phase configuration.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      phase       - phase value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 *      None
 */
int32
dal_mango_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((phase >= PIE_PHASE_END), RT_ERR_INPUT);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK(block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV), RT_ERR_PIE_BLOCK_INDEX);
    }

    switch (phase)
    {
        case PIE_PHASE_DIS:
            val = 6;
            break;
        case PIE_PHASE_VACL:
            val = 0;
            break;
        case PIE_PHASE_IACL:
            val = 1;
            break;
        case PIE_PHASE_EACL:
            val = 2;
            break;
        case PIE_PHASE_IGR_FLOW_TABLE_0:
            val = 3;
            break;
        case PIE_PHASE_IGR_FLOW_TABLE_3:
            val = 4;
            break;
        case PIE_PHASE_EGR_FLOW_TABLE_0:
            val = 5;
            break;
        default:
            return RT_ERR_INPUT;
    }

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MANGO_PIE_BLK_PHASE_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_PHASEf, &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    pie_blk_type[block_idx] = phase;

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_phase_set */

/*translate user view block index to physical block index*/
int32
_dal_mango_pie_blockIdx_trans(uint32 unit, rtk_pie_phase_t type,
    uint32 blk_idx, uint32 *phy_blk_idx)
{
    uint32  i;

    PIE_SEM_LOCK(unit);
    for (i = 0;  i < DAL_MANGO_MAX_NUM_OF_PIE_BLOCK; i++)
    {
        if (type == pie_blk_type[i])
        {
            if(0 != blk_idx)
            {
                blk_idx--;
                continue;
            }

            if(0 == blk_idx)
            {
                *phy_blk_idx = i;
                PIE_SEM_UNLOCK(unit);
                return RT_ERR_OK;
            }
        }
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_PIE_BLOCK_INDEX;
}   /* end of dal_mango_pie_blockIdx_trans */

/*translate user view entry index to physical entry index*/
int32
_dal_mango_pie_entryIdx_trans(uint32 unit, rtk_pie_phase_t type,
    uint32 entry_idx, uint32 *phy_entry_idx)
{
    uint32  i, blk_idx;

    blk_idx = entry_idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    PIE_SEM_LOCK(unit);
    for (i = 0;  i < DAL_MANGO_MAX_NUM_OF_PIE_BLOCK; ++i)
    {
        if (type == pie_blk_type[i])
        {
            if (0 != blk_idx)
            {
                blk_idx--;
                continue;
            }

            if (0 == blk_idx)
            {
                *phy_entry_idx = i * HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) +
                        entry_idx % HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "TCAM Physical Entry Index=%d", *phy_entry_idx);
                PIE_SEM_UNLOCK(unit);
                return RT_ERR_OK;
            }
        }
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_ENTRY_INDEX;
}   /* end of _dal_mango_pie_entryIdx_trans */

/* Function Name:
 *      dal_mango_pie_blockLookupEnable_get
 * Description:
 *      Get the PIE block lookup state.
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
dal_mango_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx,
    rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK(block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV), RT_ERR_PIE_BLOCK_INDEX);
    }

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_PIE_BLK_LOOKUP_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_LOOKUP_ENf, &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    if (0 == val)
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable = ENABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_blockLookupEnable_get */

/* Function Name:
 *      dal_mango_pie_blockLookupEnable_set
 * Description:
 *      Set the PIE block lookup state.
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
dal_mango_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx,
    rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK(block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV), RT_ERR_PIE_BLOCK_INDEX);
    }

    if (DISABLED == enable)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MANGO_PIE_BLK_LOOKUP_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_LOOKUP_ENf, &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_blockLookupEnable_set */

/* Function Name:
 *      dal_mango_pie_blockGrouping_get
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 * Output:
 *      group_id    - block group index
 *      logic_id    - block logic index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_ACL_BLOCK_INDEX - invalid block index
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 */
int32
dal_mango_pie_blockGrouping_get(uint32 unit, uint32 block_idx,
    uint32 *group_id, uint32 *logic_id)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, block_idx=%d", unit, block_idx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(NULL == group_id, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == logic_id, RT_ERR_NULL_POINTER);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK(block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV), RT_ERR_PIE_BLOCK_INDEX);
    }

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_PIE_BLK_GROUP_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_GRP_IDf,
            group_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PIE_BLK_GROUP_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_LOGIC_IDf,
            logic_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_blockGrouping_get */

/* Function Name:
 *      dal_mango_pie_blockGrouping_set
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 *      group_id   - block group index
 *      logic_id   - block logic index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_ACL_BLOCK_INDEX - invalid block index
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 */
int32
dal_mango_pie_blockGrouping_set( uint32 unit, uint32 block_idx,
    uint32 group_id, uint32 logic_id)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, block_idx=%d, grpId=%x, logId=%d",
            unit, block_idx, group_id, logic_id);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(group_id >= HAL_MAX_NUM_OF_PIE_BLOCK_GRP(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(logic_id >= HAL_MAX_NUM_OF_PIE_BLOCK_LOGIC(unit), RT_ERR_PIE_BLOCK_INDEX);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        RT_PARAM_CHK(block_idx >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV), RT_ERR_PIE_BLOCK_INDEX);
    }

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_PIE_BLK_GROUP_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_GRP_IDf,
            &group_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PIE_BLK_GROUP_CTRLr,
            REG_ARRAY_INDEX_NONE, block_idx, MANGO_LOGIC_IDf,
            &logic_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_blockGrouping_set */

/* Function Name:
 *      dal_mango_pie_template_get
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
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_pie_template_get(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, template_id=%d",
            unit, template_id);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((template_id >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);

    osal_memcpy(pTemplate, &dal_mango_pie_template[template_id], sizeof(rtk_pie_template_t));

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_template_get */

/* Function Name:
 *      dal_mango_pie_template_set
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
dal_mango_pie_template_set(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate)
{
    int32   ret;
    uint32  field_idx;
    uint32  value;
    rtk_pie_template_t physical_tmplte;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, template_id=%d", unit, template_id);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((template_id < HAL_PIE_USER_TEMPLATE_ID_MIN(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((template_id > HAL_PIE_USER_TEMPLATE_ID_MAX(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);

    /* Check fields */
    for (field_idx = 0; field_idx < HAL_MAX_NUM_OF_PIE_TEMPLATE_FIELD(unit); field_idx++)
    {
        if ((ret = _dal_mango_pie_log2PhyTmplteField(unit, field_idx,
                pTemplate->field[field_idx],
                &physical_tmplte.field[field_idx])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "tmplte id %u field idx %u field %d",
                    template_id, field_idx, pTemplate->field[field_idx]);
            return ret;
        }
    }

    PIE_SEM_LOCK(unit);

    osal_memcpy(&dal_mango_pie_template[template_id], pTemplate, sizeof(rtk_pie_template_t));

    PIE_SEM_UNLOCK(unit);

    /* set value to CHIP */
    for (field_idx = 0; field_idx < HAL_MAX_NUM_OF_PIE_TEMPLATE_FIELD(unit); field_idx++)
    {
        value = physical_tmplte.field[field_idx];

        PIE_SEM_LOCK(unit);
        if ((ret = reg_array_field_write(unit, MANGO_PIE_TMPLTE_CTRLr,
                template_id, field_idx, MANGO_TMPLTE_FIELDf,
                &value)) != RT_ERR_OK)
        {
            PIE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
        PIE_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_template_set */

/* Function Name:
 *      dal_mango_pie_phaseTemplate_get
 * Description:
 *      Get the template content of specific template index and phase.
 * Input:
 *      unit        - unit id
 *      templateId  - template ID
 * Output:
 *      pTemplate   - template content
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
dal_mango_pie_phaseTemplate_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 templateId, rtk_pie_template_t *pTemplate)
{
    rtk_pie_templateVlanSel_t   vlanSel;
    uint32                      fieldIdx;
    int32                       ret;

    ret = dal_mango_pie_template_get(unit, templateId, pTemplate);

    if ((PIE_PHASE_EACL == phase) || (PIE_PHASE_EGR_FLOW_TABLE_0 == phase))
    {
        switch (templateId)
        {
            case 0:
            case 1:
            case 4:
                pTemplate->field[10] = TMPLTE_FIELD_DPM0;
                pTemplate->field[11] = TMPLTE_FIELD_DPM1;
                pTemplate->field[12] = TMPLTE_FIELD_DPM2;
                pTemplate->field[13] = TMPLTE_FIELD_DPM3;
                break;
            case 2:
            case 3:
                pTemplate->field[13] = TMPLTE_FIELD_DLP;
                break;
        }
    }

    switch (templateId)
    {
        case 0 ... 2:
        case 4:
            if ((ret = dal_mango_pie_templateVlanSel_get(unit, phase, templateId,
                    &vlanSel)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                return ret;
            }

            for (fieldIdx = 0; fieldIdx < DAL_MANGO_MAX_NUM_OF_TEMPLATE_FIELD; ++fieldIdx)
            {
                if (TMPLTE_FIELD_VLAN == pTemplate->field[fieldIdx])
                {
                    switch (vlanSel)
                    {
                        case TMPLTE_VLAN_SEL_INNER:
                            pTemplate->field[fieldIdx] = TMPLTE_FIELD_ITAG;
                            break;
                        case TMPLTE_VLAN_SEL_OUTER:
                        case TMPLTE_VLAN_SEL_FWD:
                            pTemplate->field[fieldIdx] = TMPLTE_FIELD_OTAG;
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

    return ret;
}   /* end of dal_mango_pie_phaseTemplate_get */

/* Function Name:
 *      dal_mango_pie_templateField_check
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
 *      RT_ERR_PIE_PHASE                    - invalid PIE phase
 *      RT_ERR_PIE_FIELD_TYPE               - invalid PIE field type
 * Note:
 *      None
 */
int32
dal_mango_pie_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_pie_templateFieldType_t type)
{
    uint32  i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((phase >= PIE_PHASE_END), RT_ERR_INPUT);

    /* Check common field type */
    for (i = 0; dal_mango_template_common_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
    {
        if (type == dal_mango_template_common_field_list[i].field_type)
            return RT_ERR_OK;
    }

    /* check phase field type */
    switch (phase)
    {
        case PIE_PHASE_IACL:
            for (i = 0; dal_mango_template_i_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
            {
                if (type == dal_mango_template_i_field_list[i].field_type)
                    return RT_ERR_OK;
            }
            /* Don't break */
        case PIE_PHASE_VACL:
            for (i = 0; dal_mango_template_vi_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
            {
                if (type == dal_mango_template_vi_field_list[i].field_type)
                    return RT_ERR_OK;
            }
            break;
        case PIE_PHASE_EACL:
            for (i = 0; dal_mango_template_e_field_list[i].field_type != TMPLTE_FIELD_END; ++i)
            {
                if (type == dal_mango_template_e_field_list[i].field_type)
                    return RT_ERR_OK;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_PIE_FIELD_TYPE;
}    /* end of dal_mango_pie_templateField_check */

/* Function Name:
 *      dal_mango_pie_templateVlanSel_get
 * Description:
 *      Get the configuration of VLAN select in pre-defined template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      preTemplate_idx     - pre-defined template index
 * Output:
 *      pVlanSel - pointer to VLAN select in pre-defined template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 */
int32
dal_mango_pie_templateVlanSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 preTemplate_idx, rtk_pie_templateVlanSel_t *pVlanSel)
{
    int32 ret;
    uint32 value;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, phase=%d, idx=%d",
            unit, phase, preTemplate_idx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((phase >= PIE_PHASE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(((preTemplate_idx != 0) && (preTemplate_idx != 1) &&
            (preTemplate_idx != 2) &&(preTemplate_idx != 4)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanSel), RT_ERR_NULL_POINTER);

    switch (phase)
    {
        case PIE_PHASE_VACL:
        case PIE_PHASE_IGR_FLOW_TABLE_0:
            switch(preTemplate_idx)
            {
                case 0:
                    field = MANGO_V_TMPLTE0_IOTAG_SELf;
                    break;
                case 1:
                    field = MANGO_V_TMPLTE1_IOTAG_SELf;
                    break;
                case 2:
                    field = MANGO_V_TMPLTE2_IOTAG_SELf;
                    break;
                case 4:
                    field = MANGO_V_TMPLTE4_IOTAG_SELf;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        case PIE_PHASE_IACL:
        case PIE_PHASE_IGR_FLOW_TABLE_3:
            switch(preTemplate_idx)
            {
                case 0:
                    field = MANGO_I_TMPLTE0_IOTAG_SELf;
                    break;
                case 1:
                    field = MANGO_I_TMPLTE1_IOTAG_SELf;
                    break;
                case 2:
                    field = MANGO_I_TMPLTE2_IOTAG_SELf;
                    break;
                case 4:
                    field = MANGO_I_TMPLTE4_IOTAG_SELf;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        case PIE_PHASE_EACL:
        case PIE_PHASE_EGR_FLOW_TABLE_0:
            switch(preTemplate_idx)
            {
                case 0:
                    field = MANGO_E_TMPLTE0_IOTAG_SELf;
                    break;
                case 1:
                    field = MANGO_E_TMPLTE1_IOTAG_SELf;
                    break;
                case 2:
                    field = MANGO_E_TMPLTE2_IOTAG_SELf;
                    break;
                case 4:
                    field = MANGO_E_TMPLTE4_IOTAG_SELf;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_PIE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%d", value);

    /* translate to chip value */
    switch (value)
    {
        case 0:
            *pVlanSel = TMPLTE_VLAN_SEL_INNER;
            break;
        case 1:
            *pVlanSel = TMPLTE_VLAN_SEL_OUTER;
            break;
        case 2:
            *pVlanSel = TMPLTE_VLAN_SEL_FWD;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_templateVlanSel_get */

/* Function Name:
 *      dal_mango_pie_templateVlanSel_set
 * Description:
 *      set the configuration of VLAN select in pre-defined template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      preTemplate_idx     - pre-defined template index
 *      vlanSel - VLAN select in pre-defined template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 */
int32
dal_mango_pie_templateVlanSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 preTemplate_idx, rtk_pie_templateVlanSel_t vlanSel)
{
    int32 ret;
    uint32 value;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, phase=%d, idx=%d, vlanSel=%d",
            unit, phase, preTemplate_idx, vlanSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((phase >= PIE_PHASE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(((preTemplate_idx != 0) && (preTemplate_idx != 1) &&
            (preTemplate_idx != 2) &&(preTemplate_idx != 4)), RT_ERR_INPUT);
    RT_PARAM_CHK((vlanSel >= TMPLTE_VLAN_SEL_END), RT_ERR_INPUT);

    /* translate to chip value */
    switch (vlanSel)
    {
        case TMPLTE_VLAN_SEL_INNER:
            value = 0;
            break;
        case TMPLTE_VLAN_SEL_OUTER:
            value = 1;
            break;
        case TMPLTE_VLAN_SEL_FWD:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (phase)
    {
        case PIE_PHASE_VACL:
            switch(preTemplate_idx)
            {
                case 0:
                    field = MANGO_V_TMPLTE0_IOTAG_SELf;
                    break;
                case 1:
                    field = MANGO_V_TMPLTE1_IOTAG_SELf;
                    break;
                case 2:
                    field = MANGO_V_TMPLTE2_IOTAG_SELf;
                    break;
                case 4:
                    field = MANGO_V_TMPLTE4_IOTAG_SELf;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        case PIE_PHASE_IACL:
            switch(preTemplate_idx)
            {
                case 0:
                    field = MANGO_I_TMPLTE0_IOTAG_SELf;
                    break;
                case 1:
                    field = MANGO_I_TMPLTE1_IOTAG_SELf;
                    break;
                case 2:
                    field = MANGO_I_TMPLTE2_IOTAG_SELf;
                    break;
                case 4:
                    field = MANGO_I_TMPLTE4_IOTAG_SELf;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        case PIE_PHASE_EACL:
            switch(preTemplate_idx)
            {
                case 0:
                    field = MANGO_E_TMPLTE0_IOTAG_SELf;
                    break;
                case 1:
                    field = MANGO_E_TMPLTE1_IOTAG_SELf;
                    break;
                case 2:
                    field = MANGO_E_TMPLTE2_IOTAG_SELf;
                    break;
                case 4:
                    field = MANGO_E_TMPLTE4_IOTAG_SELf;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }

    PIE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_PIE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_templateVlanSel_set */

/* Function Name:
 *      dal_mango_pie_rangeCheckIp_get
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
dal_mango_pie_rangeCheckIp_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_ip_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MANGO_RNG_CHK_IP_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_TYPEf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_RNG_CHK_IP_RNGr,
            REG_ARRAY_INDEX_NONE, index, MANGO_IP_UPPERf,
            &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_RNG_CHK_IP_RNGr,
            REG_ARRAY_INDEX_NONE, index, MANGO_IP_LOWERf,
            &pData->ip_lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

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
}   /* end of dal_mango_pie_rangeCheckIp_get */

/* Function Name:
 *      dal_mango_pie_rangeCheckIp_set
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
dal_mango_pie_rangeCheckIp_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_ip_t *pData)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d, upper_bound=%d, upper_bound=%d, ip_type=%d",\
            unit, index, pData->ip_upper_bound, pData->ip_lower_bound, pData->ip_type);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

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
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_PIE), "invalid IP type");
            return RT_ERR_INPUT;
    }

    /* set value to CHIP */
    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MANGO_RNG_CHK_IP_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_TYPEf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_RNG_CHK_IP_RNGr,
            REG_ARRAY_INDEX_NONE, index, MANGO_IP_UPPERf,
            &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_RNG_CHK_IP_RNGr,
            REG_ARRAY_INDEX_NONE, index, MANGO_IP_LOWERf,
            &pData->ip_lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_rangeCheckIp_set */

/* Function Name:
 *      dal_mango_pie_rangeCheck_get
 * Description:
 *      Get the configuration of range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of range check
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
dal_mango_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_RANGE_CHECK(unit) <= index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MANGO_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_TYPEf, &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_UPPERf,
            &pData->upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_LOWERf,
            &pData->lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            pData->type = RTK_RNGCHK_IVID;
            break;
        case 1:
            pData->type = RTK_RNGCHK_OVID;
            break;
        case 2:
            pData->type = RTK_RNGCHK_L4SPORT;
            break;
        case 3:
            pData->type = RTK_RNGCHK_L4DPORT;
            break;
        case 4:
            pData->type = RTK_RNGCHK_L4PORT;
            break;
        case 5:
            pData->type = RTK_RNGCHK_PKTLEN;
            break;
        case 6:
            pData->type = RTK_RNGCHK_L3LEN;
            break;
        default:
            return RT_ERR_TYPE;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_rangeCheck_get */

/* Function Name:
 *      dal_mango_pie_rangeCheck_set
 * Description:
 *      Set the configuration of range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of range check
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "type=%d, upper_bound=%d, lower_bound=%d",
            pData->type, pData->upper_bound, pData->lower_bound);

    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->upper_bound > 0xFFFF), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->lower_bound > 0xFFFF), RT_ERR_INPUT);


    /* set value to CHIP */
    switch (pData->type)
    {
        case RTK_RNGCHK_IVID:
            RT_PARAM_CHK((pData->upper_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
            RT_PARAM_CHK((pData->lower_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
            val = 0;
            break;
        case RTK_RNGCHK_OVID:
            RT_PARAM_CHK((pData->upper_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
            RT_PARAM_CHK((pData->lower_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
            val = 1;
            break;
        case RTK_RNGCHK_L4SPORT:
            val = 2;
            break;
        case RTK_RNGCHK_L4DPORT:
            val = 3;
            break;
        case RTK_RNGCHK_L4PORT:
            val = 4;
            break;
        case RTK_RNGCHK_PKTLEN:
            val = 5;
            break;
        case RTK_RNGCHK_L3LEN:
            val = 6;
            break;
        default:
            return RT_ERR_TYPE;
    }

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_TYPEf, &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_UPPERf,
            &pData->upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MANGO_LOWERf,
            &pData->lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_rangeCheck_set */

/* Function Name:
 *      dal_mango_pie_fieldSelector_get
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
 *      None
 */
int32
dal_mango_pie_fieldSelector_get(uint32 unit, uint32 fs_idx,
    rtk_pie_fieldSelector_data_t *pFs)
{
    int32   ret;
    uint32  value, select;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, fs_idx=%d", \
            unit, fs_idx);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_PIE_FIELD_SELTOR_CTRLr,
            REG_ARRAY_INDEX_NONE, fs_idx, MANGO_FMTf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PIE_FIELD_SELTOR_CTRLr,
            REG_ARRAY_INDEX_NONE, fs_idx, MANGO_PAYLOAD_SELf, &select)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PIE_FIELD_SELTOR_CTRLr,
            REG_ARRAY_INDEX_NONE, fs_idx, MANGO_OFFSETf, &pFs->offset)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

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
            pFs->start = PIE_FS_START_POS_IP_HDR;
            break;
        case 5:
            pFs->start = FS_START_POS_IP;
            break;
        case 6:
            pFs->start = FS_START_POS_L4;
            break;
    }

    if (1 == select)
        pFs->payloadSel = PIE_FS_INNER;
    else
        pFs->payloadSel = PIE_FS_OUTER;

    return RT_ERR_OK;
}   /* end of dal_mango_pie_fieldSelector_get */

/* Function Name:
 *      dal_mango_pie_fieldSelector_set
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
 *      None
 */
int32
dal_mango_pie_fieldSelector_set(uint32 unit, uint32 fs_idx,
    rtk_pie_fieldSelector_data_t *pFs)
{
    int32   ret;
    uint32  value, select;

    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, fs_idx=%d, pFs.start=%d, select=%d,pFs.offset=%d",
        unit, fs_idx, pFs->start, pFs->payloadSel, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pFs->start >= FS_START_POS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pFs->payloadSel >= PIE_FS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pFs->offset > HAL_MAX_OFST_OF_FIELD_SELECTOR(unit)), RT_ERR_INPUT);/*unable to grab complete 16-bit*/

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
        case PIE_FS_START_POS_IP_HDR:
            value = 4;
            break;
        case FS_START_POS_IP:
            value = 5;
            break;
        case FS_START_POS_L4:
            value = 6;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_PIE), "invalid start position");
            return RT_ERR_INPUT;
    }

    if (PIE_FS_INNER == pFs->payloadSel)
        select = 1;
    else
        select = 0;

    PIE_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_PIE_FIELD_SELTOR_CTRLr,
            REG_ARRAY_INDEX_NONE, fs_idx, MANGO_FMTf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PIE_FIELD_SELTOR_CTRLr,
            REG_ARRAY_INDEX_NONE, fs_idx, MANGO_PAYLOAD_SELf, &select)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_PIE_FIELD_SELTOR_CTRLr,
            REG_ARRAY_INDEX_NONE, fs_idx, MANGO_OFFSETf,
            &pFs->offset)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_fieldSelector_set */

/* Function Name:
 *      dal_mango_pie_meterIncludeIfg_get
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
dal_mango_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32 ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    /* get val from CHIP */
    if ((ret = reg_field_read(unit, MANGO_METER_GLB_CTRLr, MANGO_INCL_PREIFGf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%d", val);

    /* translate to chip val */
    switch (val)
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
}   /* end of dal_mango_pie_meterIncludeIfg_get */

/* Function Name:
 *      dal_mango_pie_meterIncludeIfg_set
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
dal_mango_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32 ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, ifg_include=%d", unit, ifg_include);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ifg_include >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip val */
    switch (ifg_include)
    {
        case DISABLED:
            val = 0;
            break;
        case ENABLED:
            val = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    PIE_SEM_LOCK(unit);

    /* program val to CHIP */
    if ((ret = reg_field_write(unit, MANGO_METER_GLB_CTRLr, MANGO_INCL_PREIFGf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterIncludeIfg_set */

/* Function Name:
 *      dal_mango_pie_meterExceed_get
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
dal_mango_pie_meterExceed_get(uint32 unit, uint32 meterIdx, uint32 *pIsExceed)
{
    int32   ret;
    uint32  grpIdx, entryIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    grpIdx = meterIdx / 32;
    entryIdx = meterIdx % 32;

    PIE_SEM_LOCK(unit);
    /* get val from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_METER_LB_EXCEED_STSr,
            grpIdx, entryIdx, MANGO_LB_EXCEEDf, pIsExceed)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if(TRUE == *pIsExceed)
    {
        /* reset the flag */
        if ((ret = reg_array_field_write1toClear(unit,
                MANGO_METER_LB_EXCEED_STSr, grpIdx, entryIdx,
                MANGO_LB_EXCEEDf)) != RT_ERR_OK)
        {
            PIE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "*pIsExceed=%d", *pIsExceed);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterExceed_get */

/* Function Name:
 *      dal_mango_pie_meterExceedAggregation_get
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
dal_mango_pie_meterExceedAggregation_get(uint32 unit, uint32 *pExceedMask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pExceedMask), RT_ERR_NULL_POINTER);


    PIE_SEM_LOCK(unit);
    /* get val from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_METER_LB_GLB_EXCEED_STSr,
            MANGO_LB_EXCEEDf, pExceedMask)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "*pExceedMask=%d", *pExceedMask);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterExceedAggregation_get */

/* Function Name:
 *      dal_mango_pie_meterEntry_get
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
dal_mango_pie_meterEntry_get(uint32 unit, uint32 meterIdx,
    rtk_pie_meterEntry_t *pMeterEntry)
{
    int32           ret;
    uint32          val;
    uint32          table_index;
    meter_entry_t   meter_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    /*translate meter index to table index*/
    table_index = meterIdx;

    PIE_SEM_LOCK(unit);
    /* get entry from chip */
    if ((ret = table_read(unit, MANGO_METERt, table_index,
            (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    /* get TYPE */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_TYPEtf, &val,
            (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    switch (val)
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
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }

    /* Get meter mode */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_MODEtf, &val,
            (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    switch (val)
    {
        case 0:
            pMeterEntry->mode = METER_MODE_BYTE;
            break;
        case 1:
            pMeterEntry->mode = METER_MODE_PACKET;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }

    /* get COLORAWARE */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_COLOR_AWAREtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->color_aware = val;

    /* get leaky bucket 0 rate */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_LB0_RATEtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb0_rate = val;

    /* get leaky bucket 0 burst */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_LB0_BStf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb0_bs = val;

    /* get leaky bucket 1 rate */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_LB1_RATEtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb1_rate = val;

    /* get leaky bucket 1 burst */
    if ((ret = table_field_get(unit, MANGO_METERt, MANGO_METER_LB1_BStf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb1_bs = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "pMeterEntry->type=%d, pMeterEntry->color_aware=%d, pMeterEntry->lb0_rate=%d, pMeterEntry->lb1_rate=%d",
        pMeterEntry->type, pMeterEntry->color_aware, pMeterEntry->lb0_rate, pMeterEntry->lb1_rate);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterEntry_get */

/* Function Name:
 *      dal_mango_pie_meterEntry_set
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
dal_mango_pie_meterEntry_set(uint32 unit, uint32 meterIdx,
    rtk_pie_meterEntry_t *pMeterEntry)
{
    int32           ret;
    uint32          val;
    uint32          table_index;
    uint32          meterBpsTkn, minByteBurst;
    meter_entry_t   meter_entry;

    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, meterIdx=%d type=%d, \
        mode=%d, color_aware=%d, lb0_rate=%d, lb1_rate=%d",
        unit, meterIdx, pMeterEntry->type, pMeterEntry->mode,
        pMeterEntry->color_aware, pMeterEntry->lb0_rate,
        pMeterEntry->lb1_rate);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pMeterEntry->lb0_rate > HAL_RATE_OF_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pMeterEntry->lb1_rate > HAL_RATE_OF_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pMeterEntry->lb0_bs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pMeterEntry->lb1_bs > HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)), RT_ERR_INPUT);

    osal_memset(&meter_entry, 0, sizeof(meter_entry));

    /*translate policer index to table index*/
    table_index = meterIdx;

    /* set TYPE */
    switch (pMeterEntry->type)
    {
        case METER_TYPE_INVALID:
            val = 0;
            break;
        case METER_TYPE_DLB:
            val = 1;
            break;
        case METER_TYPE_SRTCM:
            val = 2;
            break;
        case METER_TYPE_TRTCM:
            val = 3;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }
    if ((ret = table_field_set(unit, MANGO_METERt, MANGO_METER_TYPEtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set mode */
    switch (pMeterEntry->mode)
    {
        case METER_MODE_BYTE:
            if (pMeterEntry->lb0_rate != 0 || pMeterEntry->lb1_rate != 0)
            {
                RT_ERR_CHK(reg_field_read(unit, MANGO_METER_BYTE_TB_CTRLr, MANGO_TKNf,
                        &meterBpsTkn), ret);

                minByteBurst = (meterBpsTkn + DAL_MANGO_BURST_BYTE_MODE_UNIT - 1) / DAL_MANGO_BURST_BYTE_MODE_UNIT;

                if (pMeterEntry->lb0_rate != 0 && pMeterEntry->lb0_bs < minByteBurst)
                {
                    RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), "Meter bucket 0 burst is less than %u", minByteBurst);
                    return RT_ERR_INPUT;
                }

                if (pMeterEntry->lb1_rate != 0 && pMeterEntry->lb1_bs < minByteBurst)
                {
                    RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), "Meter bucket 1 burst is less than %u", minByteBurst);
                    return RT_ERR_INPUT;
                }
            }
            val = 0;
            break;
        case METER_MODE_PACKET:
            val = 1;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }
    if ((ret = table_field_set(unit, MANGO_METERt, MANGO_METER_MODEtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set COLORAWARE */
    val = pMeterEntry->color_aware;
    if ((ret = table_field_set(unit, MANGO_METERt, MANGO_METER_COLOR_AWAREtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 0 rate */
    val = pMeterEntry->lb0_rate;
    if ((ret = table_field_set( unit, MANGO_METERt, MANGO_METER_LB0_RATEtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 0 burst */
    val = pMeterEntry->lb0_bs;
    if ((ret = table_field_set( unit, MANGO_METERt, MANGO_METER_LB0_BStf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 1 rate */
    val = pMeterEntry->lb1_rate;
    if ((ret = table_field_set( unit, MANGO_METERt, MANGO_METER_LB1_RATEtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 1 burst */
    val = pMeterEntry->lb1_bs;
    if ((ret = table_field_set( unit, MANGO_METERt, MANGO_METER_LB1_BStf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, MANGO_METERt, table_index,
            (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    /* reset meter token */
    val = 1;
    if ((ret = table_field_set( unit, MANGO_METERt, MANGO_METER_RSTtf,
            &val, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, MANGO_METERt, table_index,
            (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterEntry_set */

/* Function Name:
 *      dal_mango_pie_meterDpSel_get
 * Description:
 *      Get the configuration of DP select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pDpSel      - pointer to DP select
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
dal_mango_pie_meterDpSel_get(uint32 unit, rtk_pie_meterDpSel_t *pDpSel)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDpSel), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_PIE_MISCr, MANGO_DP_SELf,
            &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%d", value);

    /* translate to chip value */
    switch (value)
    {
        case 0:
            *pDpSel = METER_DP_SEL_DEEPEST_COLOR;
            break;
        case 1:
            *pDpSel = METER_DP_SEL_LOWEST_IDX;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterDpSel_get */

/* Function Name:
 *      dal_mango_pie_meterDpSel_set
 * Description:
 *      Set the configuration of DP select.
 * Input:
 *      unit        - unit id
 *      dpSel       - DP select
 * Output:
 *      None
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
dal_mango_pie_meterDpSel_set(uint32 unit, rtk_pie_meterDpSel_t dpSel)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, dpSel=%d", unit, dpSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((dpSel >= METER_DP_SEL_END), RT_ERR_INPUT);

    /* translate to chip value */
    switch (dpSel)
    {
        case METER_DP_SEL_DEEPEST_COLOR:
            value = 0;
            break;
        case METER_DP_SEL_LOWEST_IDX:
            value = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    PIE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_PIE_MISCr, MANGO_DP_SELf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterDpSel_set */

/* Function Name:
 *      dal_mango_pie_arpMacSel_get
 * Description:
 *      Get the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pArpMacSel  - pointer to ARP MAC select
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
dal_mango_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpMacSel), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_PIE_MISCr, MANGO_ARP_MAC_CTRLf,
            &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%d", value);

    /* translate to chip value */
    switch (value)
    {
        case 0:
            *pArpMacSel = ARP_MAC_SEL_L2;
            break;
        case 1:
            *pArpMacSel = ARP_MAC_SEL_ARP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_arpMacSel_get */

/* Function Name:
 *      dal_mango_pie_arpMacSel_set
 * Description:
 *      Set the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 *      arpMacSel   - ARP MAC select
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
dal_mango_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, arpMacSel=%d", unit, arpMacSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((arpMacSel >= ARP_MAC_SEL_END), RT_ERR_INPUT);

    /* translate to chip value */
    switch (arpMacSel)
    {
        case ARP_MAC_SEL_L2:
            value = 0;
            break;
        case ARP_MAC_SEL_ARP:
            value = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    PIE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_PIE_MISCr, MANGO_ARP_MAC_CTRLf,
            &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_arpMacSel_set */

/* Function Name:
 *      dal_mango_pie_intfSel_get
 * Description:
 *      Get the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 * Output:
 *      intfSel    - select inteface filter type
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
dal_mango_pie_intfSel_get(uint32 unit, rtk_pie_intfSel_t *intfSel)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == intfSel), RT_ERR_NULL_POINTER);

    /* function body */
    PIE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_PIE_MISCr, MANGO_INTF_SELf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%u", val);

    /* translate to chip value */
    switch (val)
    {
        case 0:
            *intfSel = PIE_INTF_SEL_L3;
            break;
        case 1:
            *intfSel = PIE_INTF_SEL_TUNNEL;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_intfSel_get */

/* Function Name:
 *      dal_mango_pie_intfSel_set
 * Description:
 *      Set the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 *      intfSel    - select inteface filter type
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
dal_mango_pie_intfSel_set(uint32 unit, rtk_pie_intfSel_t intfSel)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,intfSel=%d", unit, intfSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PIE_INTF_SEL_END <= intfSel), RT_ERR_INPUT);

    /* function body */
    /* translate to chip value */
    switch (intfSel)
    {
        case PIE_INTF_SEL_L3:
            val = 0;
            break;
        case PIE_INTF_SEL_TUNNEL:
            val = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    PIE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_PIE_MISCr, MANGO_INTF_SELf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_intfSel_set */

/* Function Name:
 *      dal_mango_pie_templateVlanFmtSel_get
 * Description:
 *      Get the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - pre-defined template index
 * Output:
 *      pVlanFmtSel         - pointer to VLAN format select in template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The API onlye support ingress PIE.
 */
int32
dal_mango_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,phase=%d,template_idx=%d", unit, phase, template_idx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PIE_PHASE_VACL != phase && PIE_PHASE_IGR_FLOW_TABLE_0 != phase), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_TEMPLATE(unit) <= template_idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanFmtSel), RT_ERR_NULL_POINTER);

    /* function body */
    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_PIE_TAG_CTRLr,
            REG_ARRAY_INDEX_NONE, template_idx, MANGO_V_TMPLTE_IOTAG_FMTf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    if (0 == val)
        *pVlanFmtSel = TMPLTE_VLAN_FMT_SEL_ORIGINAL;
    else
        *pVlanFmtSel = TMPLTE_VLAN_FMT_SEL_MODIFIED;

    return RT_ERR_OK;
}   /* end of dal_mango_pie_templateVlanFmtSel_get */

/* Function Name:
 *      dal_mango_pie_templateVlanFmtSel_set
 * Description:
 *      Set the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - pre-defined template index
 *      vlanFmtSel          - VLAN format select in template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The API onlye support ingress PIE.
 */
int32
dal_mango_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,phase=%d,template_idx=%d,vlanFmtSel=%d", unit, phase, template_idx, vlanFmtSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PIE_PHASE_VACL != phase && PIE_PHASE_IGR_FLOW_TABLE_0 != phase), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_TEMPLATE(unit) <= template_idx), RT_ERR_INPUT);
    RT_PARAM_CHK((TMPLTE_VLAN_FMT_SEL_END <= vlanFmtSel), RT_ERR_INPUT);

    /* function body */
    if (TMPLTE_VLAN_FMT_SEL_ORIGINAL == vlanFmtSel)
        val = 0;
    else
        val = 1;

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_PIE_TAG_CTRLr,
            REG_ARRAY_INDEX_NONE, template_idx, MANGO_V_TMPLTE_IOTAG_FMTf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_templateVlanFmtSel_set */

/* Function Name:
 *      dal_mango_pie_meterTrtcmType_get
 * Description:
 *      Get a meter TrTCM type.
 * Input:
 *      unit        - unit id
 * Output:
 *      type        - meter trTCM type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None.
 */
int32
dal_mango_pie_meterTrtcmType_get(uint32 unit, rtk_pie_meterTrtcmType_t *type)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == type), RT_ERR_NULL_POINTER);

    /* function body */
    PIE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_METER_GLB_CTRLr, MANGO_TRTCM_TYPEf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    /* translate to chip value */
    switch (val)
    {
        case 0:
            *type = METER_TRTCM_TYPE_ORIGINAL;
            break;
        case 1:
            *type = METER_TRTCM_TYPE_MODIFIED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterTrtcmType_get */

/* Function Name:
 *      dal_mango_pie_meterTrtcmType_set
 * Description:
 *      Set a meter TrTCM type.
 * Input:
 *      unit        - unit id
 *      type        - meter trTCM type
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None.
 */
int32
dal_mango_pie_meterTrtcmType_set(uint32 unit, rtk_pie_meterTrtcmType_t type)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((METER_TRTCM_TYPE_END <= type), RT_ERR_INPUT);

    /* function body */
    /* translate to chip value */
    switch (type)
    {
        case PIE_INTF_SEL_L3:
            val = 0;
            break;
        case PIE_INTF_SEL_TUNNEL:
            val = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    PIE_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_METER_GLB_CTRLr, MANGO_TRTCM_TYPEf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_meterTrtcmType_set */

/* Function Name:
 *      _dal_mango_pie_init_config
 * Description:
 *      Initialize default configuration for the PIE module of the specified device..
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
_dal_mango_pie_init_config(uint32 unit)
{
    uint32  i;
    int32   ret;

    for (i = 0; i < HAL_MAX_NUM_OF_PIE_BLOCK(unit); ++i)
    {
        if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit) &&
            (i >= (HAL_MAX_NUM_OF_PIE_BLOCK(unit)/DAL_MANGO_MAX_BLK_RTL9311E_DIV)))
        {
            continue;
        }

        if ((ret = dal_mango_pie_phase_set(unit, i, RTK_DEFAULT_PIE_PHASE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "blk:%d", i);
            return ret;
        }
    }

    if ((ret = dal_mango_pie_meterIncludeIfg_set(unit,
            RTK_DEFAULT_METER_IFG_INCLUDE_STATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_mango_pie_init_config */

/* Function Name:
 *      dal_mango_pieMapper_init
 * Description:
 *      Hook pie module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook pie module before calling any pie APIs.
 */
int32
dal_mango_pieMapper_init(dal_mapper_t *pMapper)
{
    pMapper->pie_init = dal_mango_pie_init;
    pMapper->pie_phase_get = dal_mango_pie_phase_get;
    pMapper->pie_phase_set = dal_mango_pie_phase_set;
    pMapper->pie_blockLookupEnable_get = dal_mango_pie_blockLookupEnable_get;
    pMapper->pie_blockLookupEnable_set = dal_mango_pie_blockLookupEnable_set;
    pMapper->pie_blockGrouping_get = dal_mango_pie_blockGrouping_get;
    pMapper->pie_blockGrouping_set = dal_mango_pie_blockGrouping_set;
    pMapper->pie_template_get = dal_mango_pie_template_get;
    pMapper->pie_template_set = dal_mango_pie_template_set;
    pMapper->pie_templateVlanSel_get = dal_mango_pie_templateVlanSel_get;
    pMapper->pie_templateVlanSel_set = dal_mango_pie_templateVlanSel_set;
    pMapper->pie_templateField_check = dal_mango_pie_templateField_check;
    pMapper->pie_rangeCheckIp_get = dal_mango_pie_rangeCheckIp_get;
    pMapper->pie_rangeCheckIp_set = dal_mango_pie_rangeCheckIp_set;
    pMapper->pie_rangeCheck_get = dal_mango_pie_rangeCheck_get;
    pMapper->pie_rangeCheck_set = dal_mango_pie_rangeCheck_set;
    pMapper->pie_fieldSelector_get = dal_mango_pie_fieldSelector_get;
    pMapper->pie_fieldSelector_set = dal_mango_pie_fieldSelector_set;
    pMapper->pie_meterIncludeIfg_get = dal_mango_pie_meterIncludeIfg_get;
    pMapper->pie_meterIncludeIfg_set = dal_mango_pie_meterIncludeIfg_set;
    pMapper->pie_meterExceed_get = dal_mango_pie_meterExceed_get;
    pMapper->pie_meterExceedAggregation_get = dal_mango_pie_meterExceedAggregation_get;
    pMapper->pie_meterEntry_get = dal_mango_pie_meterEntry_get;
    pMapper->pie_meterEntry_set = dal_mango_pie_meterEntry_set;
    pMapper->pie_meterDpSel_get = dal_mango_pie_meterDpSel_get;
    pMapper->pie_meterDpSel_set = dal_mango_pie_meterDpSel_set;
    pMapper->pie_arpMacSel_get = dal_mango_pie_arpMacSel_get;
    pMapper->pie_arpMacSel_set = dal_mango_pie_arpMacSel_set;
    pMapper->pie_intfSel_get = dal_mango_pie_intfSel_get;
    pMapper->pie_intfSel_set = dal_mango_pie_intfSel_set;
    pMapper->pie_templateVlanFmtSel_get = dal_mango_pie_templateVlanFmtSel_get;
    pMapper->pie_templateVlanFmtSel_set = dal_mango_pie_templateVlanFmtSel_set;
    pMapper->pie_meterTrtcmType_get = dal_mango_pie_meterTrtcmType_get;
    pMapper->pie_meterTrtcmType_set = dal_mango_pie_meterTrtcmType_set;
    pMapper->pie_filter1BR_get = dal_mango_pie_filter1BR_get;
    pMapper->pie_filter1BR_set = dal_mango_pie_filter1BR_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_pie_init
 * Description:
 *      Initialize PIE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize PIE module before calling any PIE APIs.
 */
int32
dal_mango_pie_init(uint32 unit)
{
    int32   ret;

    RT_INIT_REENTRY_CHK(pie_init[unit]);
    pie_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    pie_sem[unit] = osal_sem_mutex_create();
    if (0 == pie_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    pie_init[unit] = INIT_COMPLETED;

    osal_memset(&pie_blk_type, 0x0, sizeof(rtk_pie_phase_t) * DAL_MANGO_MAX_NUM_OF_PIE_BLOCK);
    osal_memset(&pie_blk_template, 0, (sizeof(uint32) * DAL_MANGO_MAX_NUM_OF_PIE_BLOCK * DAL_MANGO_MAX_TEMPLATE_OF_BLK));

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((ret = _dal_mango_pie_init_config(unit)) != RT_ERR_OK)
        {
            pie_init[unit] = INIT_NOT_COMPLETED;
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_mango_pie_init */

/* Function Name:
 *      dal_mango_pie_filter1BR_get
 * Description:
 *      Get filter 802.1BR status.
 * Input:
 *      unit                - unit id
 * Output:
 *      en    - filter 802.1BR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_pie_filter1BR_get(uint32 unit, rtk_enable_t *en)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == en), RT_ERR_NULL_POINTER);

    /* function body */
    PIE_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_PIE_MISC2r, MANGO_FILTER_8021BR_ENf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    switch (val)
    {
        case 0:
            *en = DISABLED;
            break;
        case 1:
            *en = ENABLED;
            break;
        default:
            PIE_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_filter1BR_get */

/* Function Name:
 *      dal_mango_pie_filter1BR_set
 * Description:
 *      Set filter 802.1BR status.
 * Input:
 *      unit  - unit id
 *      en    - filter 802.1BR status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_pie_filter1BR_set(uint32 unit, rtk_enable_t en)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,en=%d", unit, en);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= en), RT_ERR_INPUT);

    /* function body */
    PIE_SEM_LOCK(unit);

    switch (en)
    {
        case DISABLED:
            val = 0;
            break;
        case ENABLED:
            val = 1;
            break;
        default:
            PIE_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
    }

    if ((ret = reg_field_write(unit, MANGO_PIE_MISC2r, MANGO_FILTER_8021BR_ENf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_pie_filter1BR_set */

