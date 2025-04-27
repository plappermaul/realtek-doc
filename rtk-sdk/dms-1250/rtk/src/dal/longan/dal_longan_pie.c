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
 *            1) Block/Template
 *            2) Field Selector
 *            3) Range Check
 *            4) Meter
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
#include <dal/longan/dal_longan_pie.h>
#include <rtk/default.h>
#include <rtk/pie.h>

/*
 * Symbol Definition
 */
#define DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD 12
#define DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK      16
#define DAL_LONGAN_MAX_TEMPLATE_PER_BLOCK    2

/*
 * Data Declaration
 */
static uint32               pie_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         pie_sem[RTK_MAX_NUM_OF_UNIT];

static rtk_pie_phase_t      pie_blk_type[DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK];
static uint32               pie_blk_template[DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK][DAL_LONGAN_MAX_TEMPLATE_PER_BLOCK];


typedef struct dal_longan_pie_templateField_s
{
    rtk_pie_templateFieldType_t     field_type;     /* field type in logical */
    uint32                          physical_id;    /* physical field ID in ASIC */
    uint32                          valid_location; /* valid field location, 0 means no limitation */
} dal_longan_pie_templateField_t;

uint32 template_epie_list[] = {TMPLTE_FIELD_DLP, TMPLTE_FIELD_SRC_FWD_VID, TMPLTE_FIELD_END};

rtk_pie_template_t dal_longan_pie_templates[] =
{
    /* predefined templates */
    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_SMAC0, TMPLTE_FIELD_SMAC1, TMPLTE_FIELD_SMAC2,
        TMPLTE_FIELD_VLAN, TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_DSAP_SSAP,
        TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_SPM0,
        TMPLTE_FIELD_SPM1}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0,
        TMPLTE_FIELD_DIP1, TMPLTE_FIELD_IP_TOS_PROTO, TMPLTE_FIELD_TCP_INFO,
        TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT, TMPLTE_FIELD_VLAN,
        TMPLTE_FIELD_RANGE_CHK, TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1}},

    {{TMPLTE_FIELD_DMAC0, TMPLTE_FIELD_DMAC1, TMPLTE_FIELD_DMAC2,
        TMPLTE_FIELD_VLAN, TMPLTE_FIELD_ETHERTYPE, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_DIP0,
        TMPLTE_FIELD_DIP1, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT}},

    {{TMPLTE_FIELD_DIP0, TMPLTE_FIELD_DIP1, TMPLTE_FIELD_DIP2,
        TMPLTE_FIELD_DIP3, TMPLTE_FIELD_DIP4, TMPLTE_FIELD_DIP5,
        TMPLTE_FIELD_DIP6, TMPLTE_FIELD_DIP7, TMPLTE_FIELD_IP_TOS_PROTO,
        TMPLTE_FIELD_TCP_INFO, TMPLTE_FIELD_L4_SPORT, TMPLTE_FIELD_L4_DPORT}},

    {{TMPLTE_FIELD_SIP0, TMPLTE_FIELD_SIP1, TMPLTE_FIELD_SIP2,
        TMPLTE_FIELD_SIP3, TMPLTE_FIELD_SIP4,
        TMPLTE_FIELD_SIP5, TMPLTE_FIELD_SIP6,
        TMPLTE_FIELD_SIP7, TMPLTE_FIELD_VLAN,
        TMPLTE_FIELD_RANGE_CHK, TMPLTE_FIELD_SPM0, TMPLTE_FIELD_SPM1}},

    /* configurable templates */
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
    {{TMPLTE_FIELD_NONE}},
};

dal_longan_pie_templateField_t dal_longan_template_field_list[] =
{
    {   /* field type       */      TMPLTE_FIELD_NONE,
        /* physical field ID*/      0x3f,/* invalid phsic id */
        /* valid field location */  0,
    },
    {   /* field type       */      TMPLTE_FIELD_SPM0,
        /* physical field ID*/      0,
        /* valid field location */  (1 << 10)
    },
    {   /* field type       */      TMPLTE_FIELD_SPM1,
        /* physical field ID*/      1,
        /* valid field location */  (1 << 11)
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC0,
        /* physical field ID*/      2,
        /* valid field location */  ((1 << 0) | (1 << 3) | (1 << 6) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC1,
        /* physical field ID*/      3,
        /* valid field location */  ((1 << 1) | (1 << 4) | (1 << 7) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_DMAC2,
        /* physical field ID*/      4,
        /* valid field location */  ((1 << 2) | (1 << 5) | (1 << 8) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC0,
        /* physical field ID*/      5,
        /* valid field location */  ((1 << 0) | (1 << 3) | (1 << 6) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC1,
        /* physical field ID*/      6,
        /* valid field location */  ((1 << 1) | (1 << 4) | (1 << 7) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_SMAC2,
        /* physical field ID*/      7,
        /* valid field location */  ((1 << 2) | (1 << 5) | (1 << 8) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_ETHERTYPE,
        /* physical field ID*/      8,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_OTAG,
        /* physical field ID*/      9,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_ITAG,
        /* physical field ID*/      10,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SIP0,
        /* physical field ID*/      11,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_SIP1,
        /* physical field ID*/      12,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP0,
        /* physical field ID*/      13,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_DIP1,
        /* physical field ID*/      14,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_IP_TOS_PROTO,
        /* physical field ID*/      15,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_L4_SPORT,
        /* physical field ID*/      16,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10))
    },
    {   /* field type       */      TMPLTE_FIELD_L4_DPORT,
        /* physical field ID*/      17,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11))
    },
    {   /* field type       */      TMPLTE_FIELD_L34_HEADER,
        /* physical field ID*/      18,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_TCP_INFO,
        /* physical field ID*/      19,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_VALID,
        /* physical field ID*/      20,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_0,
        /* physical field ID*/      21,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_1,
        /* physical field ID*/      22,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_2,
        /* physical field ID*/      23,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_3,
        /* physical field ID*/      24,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_4,
        /* physical field ID*/      25,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_5,
        /* physical field ID*/      26,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_6,
        /* physical field ID*/      27,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_7,
        /* physical field ID*/      28,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_8,
        /* physical field ID*/      29,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_9,
        /* physical field ID*/      30,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_10,
        /* physical field ID*/      31,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_FIELD_SELECTOR_11,
        /* physical field ID*/      32,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP2,
        /* physical field ID*/      27,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP3,
        /* physical field ID*/      28,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP4,
        /* physical field ID*/      29,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP5,
        /* physical field ID*/      30,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP6,
        /* physical field ID*/      31,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_SIP7,
        /* physical field ID*/      32,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP2,
        /* physical field ID*/      33,
        /* valid field location */  (1 << 2)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP3,
        /* physical field ID*/      34,
        /* valid field location */  (1 << 3)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP4,
        /* physical field ID*/      35,
        /* valid field location */  (1 << 4)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP5,
        /* physical field ID*/      36,
        /* valid field location */  (1 << 5)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP6,
        /* physical field ID*/      37,
        /* valid field location */  (1 << 6)
    },
    {   /* field type       */      TMPLTE_FIELD_DIP7,
        /* physical field ID*/      38,
        /* valid field location */  (1 << 7)
    },
    {   /* field type       */      TMPLTE_FIELD_PKT_INFO,
        /* physical field ID*/      39,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_FLOW_LABEL,
        /* physical field ID*/      40,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_DSAP_SSAP,
        /* physical field ID*/      41,
        /* valid field location */  ((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8))
    },
    {   /* field type       */      TMPLTE_FIELD_SNAP_OUI,
        /* physical field ID*/      42,
        /* valid field location */  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9))
    },
    {   /* field type       */      TMPLTE_FIELD_FWD_VID,
        /* physical field ID*/      43,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_RANGE_CHK,
        /* physical field ID*/      44,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_VLAN_GMSK,
        /* physical field ID*/      45,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_DLP,
        /* physical field ID*/      46,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_META_DATA,
        /* physical field ID*/      47,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SRC_FWD_VID,
        /* physical field ID*/      48,
        /* valid field location */  0
    },
    {   /* field type       */      TMPLTE_FIELD_SLP,
        /* physical field ID*/      49,
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

static int32
_dal_longan_pie_log2PhyTmplteField(uint32 unit, uint32 field_idx, rtk_pie_templateFieldType_t log_field_id, uint32 *phy_field_id);
static int32 _dal_longan_pie_init_config(uint32 unit);


int32 dal_longan_pie_index_chk(uint32 _u, rtk_pie_phase_t _t, uint32 _idx)
{
    rtk_pie_phase_t _phase;
    RT_PARAM_CHK((_idx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(_u)), RT_ERR_ENTRY_INDEX);
    dal_longan_pie_phase_get(_u, _idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(_u), &_phase);

    if (_t != _phase)
        return RT_ERR_PIE_PHASE;
    return RT_ERR_OK;
}

/*translate user view block index to physical block index*/
int32
dal_longan_pie_blockIdx_trans(uint32 unit, rtk_pie_phase_t type, uint32 blk_idx, uint32 *phy_blk_idx)
{
    uint32  i;

    PIE_SEM_LOCK(unit);
    for (i = 0;  i < DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK; i++)
    {
        if(type == pie_blk_type[i])
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
}

/*translate user view entry index to physical entry index*/
int32
dal_longan_pie_entryIdx_trans(uint32 unit, rtk_pie_phase_t type, uint32 entry_idx, uint32 *phy_entry_idx)
{
    uint32  i, blk_idx;

    blk_idx = entry_idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    PIE_SEM_LOCK(unit);
    for (i = 0;  i < DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK; i++)
    {
        if(type == pie_blk_type[i])
        {
            if(0 != blk_idx)
            {
                blk_idx--;
                continue;
            }
            else
            {
                *phy_entry_idx = i * HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) +
                        entry_idx % HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
                PIE_SEM_UNLOCK(unit);
                return RT_ERR_OK;
            }
        }
    }
    PIE_SEM_UNLOCK(unit);
    return RT_ERR_PIE_BLOCK_INDEX;
}


/* translate entry index from logical(phase) to physical */
int32 dal_longan_pie_index_to_physical(uint32 _u, rtk_pie_phase_t _type, uint32 _logicIdx, uint32 *_phyIdx)
{
    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK((_logicIdx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(_u)), RT_ERR_ENTRY_INDEX);
    ret = dal_longan_pie_entryIdx_trans(_u, _type, _logicIdx, _phyIdx);
    if(RT_ERR_OK != ret)
        return ret;
    ret = dal_longan_pie_index_chk(_u, _type, *_phyIdx);
    if(RT_ERR_OK != ret)
        return ret;
    return ret;
}

int32 dal_longan_pie_physical_index_to_logic(uint32 unit, uint32 phyIdx, rtk_pie_phase_t *phase, uint32 *logicIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 i;
    uint32 phy_blk_idx = 0;
    uint32 logic_blk_idx = 0;
    RT_PARAM_CHK((phyIdx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)), RT_ERR_ENTRY_INDEX);

    phy_blk_idx = phyIdx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);

    PIE_SEM_LOCK(unit);
    *phase = pie_blk_type[phy_blk_idx];
    for (i = 0;  i < DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK; i++)
    {
        if(*phase == pie_blk_type[i])
        {
            if(i == phy_blk_idx)
            {
                break;
            }
            else
            {
                logic_blk_idx++;
            }
        }
    }
    *logicIdx = logic_blk_idx * HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit) +
            phyIdx % HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit);
    PIE_SEM_UNLOCK(unit);

    return ret;
}

/*
 * Get the mapping template of specific PIE block.
 */
int32
dal_longan_pie_templateSelector_get(
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
dal_longan_pie_templateSelector_set(
    uint32 unit,
    uint32 block_idx,
    uint32 template0_id,
    uint32 template1_id)
{
    int32   ret;

    PIE_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PIE_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_BLK_TMPLTE1f,
                        &template0_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pie_blk_template[block_idx][0] = template0_id;

    if ((ret = reg_array_field_write(unit,
                        LONGAN_PIE_BLK_TMPLTE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_BLK_TMPLTE2f,
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
 *      dal_longan_pieMapper_init
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
dal_longan_pieMapper_init(dal_mapper_t *pMapper)
{
    pMapper->pie_init = dal_longan_pie_init;
    pMapper->pie_phase_get = dal_longan_pie_phase_get;
    pMapper->pie_phase_set = dal_longan_pie_phase_set;
    pMapper->pie_blockLookupEnable_get = dal_longan_pie_blockLookupEnable_get;
    pMapper->pie_blockLookupEnable_set = dal_longan_pie_blockLookupEnable_set;
    pMapper->pie_blockGrouping_get = dal_longan_pie_blockGrouping_get;
    pMapper->pie_blockGrouping_set = dal_longan_pie_blockGrouping_set;
    pMapper->pie_template_get = dal_longan_pie_template_get;
    pMapper->pie_template_set = dal_longan_pie_template_set;
    pMapper->pie_templateVlanSel_get = dal_longan_pie_templateVlanSel_get;
    pMapper->pie_templateVlanSel_set = dal_longan_pie_templateVlanSel_set;
    pMapper->pie_templateField_check = dal_longan_pie_templateField_check;
    pMapper->pie_rangeCheckIp_get = dal_longan_pie_rangeCheckIp_get;
    pMapper->pie_rangeCheckIp_set = dal_longan_pie_rangeCheckIp_set;
    pMapper->pie_rangeCheck_get = dal_longan_pie_rangeCheck_get;
    pMapper->pie_rangeCheck_set = dal_longan_pie_rangeCheck_set;
    pMapper->pie_fieldSelector_get = dal_longan_pie_fieldSelector_get;
    pMapper->pie_fieldSelector_set = dal_longan_pie_fieldSelector_set;
    pMapper->pie_meterIncludeIfg_get = dal_longan_pie_meterIncludeIfg_get;
    pMapper->pie_meterIncludeIfg_set = dal_longan_pie_meterIncludeIfg_set;
    pMapper->pie_meterExceed_get = dal_longan_pie_meterExceed_get;
    pMapper->pie_meterExceedAggregation_get = dal_longan_pie_meterExceedAggregation_get;
    pMapper->pie_meterEntry_get = dal_longan_pie_meterEntry_get;
    pMapper->pie_meterEntry_set = dal_longan_pie_meterEntry_set;
    pMapper->pie_meterDpSel_get = dal_longan_pie_meterDpSel_get;
    pMapper->pie_meterDpSel_set = dal_longan_pie_meterDpSel_set;
    pMapper->pie_arpMacSel_get = dal_longan_pie_arpMacSel_get;
    pMapper->pie_arpMacSel_set = dal_longan_pie_arpMacSel_set;
    pMapper->pie_templateVlanFmtSel_get = dal_longan_pie_templateVlanFmtSel_get;
    pMapper->pie_templateVlanFmtSel_set = dal_longan_pie_templateVlanFmtSel_set;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_init
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
dal_longan_pie_init(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;

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

    osal_memset(&pie_blk_type, 0x0, sizeof(dal_longan_pie_blkType_t) * DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK);
    osal_memset(&pie_blk_template, 0x0, sizeof(uint32) * DAL_LONGAN_MAX_NUM_OF_PIE_BLOCK * DAL_LONGAN_MAX_TEMPLATE_PER_BLOCK);

    if ((ret = _dal_longan_pie_init_config(unit)) != RT_ERR_OK)
    {
        pie_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_longan_pie_init */

/* Function Name:
 *      dal_longan_pie_phase_get
 * Description:
 *      Get the pie block phase.
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
 * Applicable:
 *      9300, 931x
 * Note:
 *      Each pie block can be configured to ingress or egress or post(only for 931x).
 */
int32
dal_longan_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase)
{
    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pPhase, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    PIE_SEM_LOCK(unit);

    *pPhase = pie_blk_type[block_idx];

    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "phase=%d", *pPhase);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_phase_set
 * Description:
 *      Set the pie block phase configuration.
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
 * Applicable:
 *      9300, 931x
 * Note:
 *      Each pie block can be configured to ingress or egress or post(only for 931x).
 */
int32
dal_longan_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((phase > PIE_PHASE_IACL), RT_ERR_INPUT);

    switch (phase)
    {
        case PIE_PHASE_VACL:
            val = 0;
            break;
        case PIE_PHASE_IACL:
            val = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PIE_BLK_PHASE_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_PHASEf,
                        &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pie_blk_type[block_idx] = phase;

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_blockLookupEnable_get
 * Description:
 *      Get the pie block lookup state.
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
dal_longan_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                        LONGAN_PIE_BLK_LOOKUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_LOOKUP_ENf,
                        pEnable)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_blockLookupEnable_set
 * Description:
 *      Set the pie block lookup state.
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
dal_longan_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PIE_BLK_LOOKUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_LOOKUP_ENf,
                        &enable)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_blockGrouping_get
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
dal_longan_pie_blockGrouping_get(uint32 unit, uint32 block_idx,
    uint32 *group_id, uint32 *logic_id)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, block_idx=%d", unit, block_idx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(NULL == group_id, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == logic_id, RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        LONGAN_PIE_BLK_GROUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_GRP_IDf,
                        group_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        LONGAN_PIE_BLK_GROUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_LOGIC_IDf,
                        logic_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_pie_blockGrouping_get */

/* Function Name:
 *      dal_longan_pie_blockGrouping_set
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
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 */
int32
dal_longan_pie_blockGrouping_set(uint32 unit, uint32 block_idx,
    uint32 group_id, uint32 logic_id)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, block_idx=%d", unit, block_idx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(block_idx >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(group_id >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);
    RT_PARAM_CHK(logic_id >= HAL_MAX_NUM_OF_PIE_BLOCK(unit), RT_ERR_PIE_BLOCK_INDEX);

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PIE_BLK_GROUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_GRP_IDf,
                        &group_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PIE_BLK_GROUP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        block_idx,
                        LONGAN_LOGIC_IDf,
                        &logic_id)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_pie_blockGrouping_set */

/* Function Name:
 *      dal_longan_pie_template_get
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
dal_longan_pie_template_get(uint32 unit, uint32 template_id, rtk_pie_template_t *pTemplate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, template_id=%d", unit, template_id);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((template_id >= HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)), RT_ERR_PIE_TEMPLATE_INDEX);
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);

    /* get value from Sw database */
    PIE_SEM_LOCK(unit);
    osal_memcpy(pTemplate, &dal_longan_pie_templates[template_id], sizeof(rtk_pie_template_t));
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_template_set
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
dal_longan_pie_template_set(uint32 unit, uint32 template_id, rtk_pie_template_t *pTemplate)
{
    int32   ret = RT_ERR_FAILED;
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
    for (field_idx = 0; field_idx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        if ((ret = _dal_longan_pie_log2PhyTmplteField(unit,
                                field_idx,
                                pTemplate->field[field_idx],
                                &physical_tmplte.field[field_idx])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
    }

    /* set value to CHIP */
    for (field_idx = 0; field_idx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
    {
        value = physical_tmplte.field[field_idx];

        PIE_SEM_LOCK(unit);
        if ((ret = reg_array_field_write(unit, LONGAN_PIE_TMPLTE_CTRLr, template_id, field_idx, LONGAN_TMPLTE_FIELDf, &value)) != RT_ERR_OK)
        {
            PIE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
        PIE_SEM_UNLOCK(unit);
    }

    /* set value to Sw database */
    PIE_SEM_LOCK(unit);
    osal_memcpy(&dal_longan_pie_templates[template_id], pTemplate, sizeof(rtk_pie_template_t));
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_templateField_check
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
dal_longan_pie_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_pie_templateFieldType_t type)
{
    uint32  i, k;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    if (phase != PIE_PHASE_VACL && phase != PIE_PHASE_IACL)
        return RT_ERR_PIE_PHASE;

    /* Check field type */
    for (i = 0; dal_longan_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (type == dal_longan_template_field_list[i].field_type)
            break;
    }

    if (dal_longan_template_field_list[i].field_type == TMPLTE_FIELD_END)
    {
        ret = RT_ERR_PIE_FIELD_TYPE;
        return ret;
    }

    if (PIE_PHASE_IACL == phase)
        return RT_ERR_OK;

    for (k = 0; template_epie_list[k] != TMPLTE_FIELD_END; ++k)
    {
        if (template_epie_list[k] == type)
            return RT_ERR_PIE_FIELD_TYPE;
    }

    return RT_ERR_OK;
}    /* end of dal_longan_pie_templateField_check */

/* Function Name:
 *      dal_longan_pie_rangeCheckIp_get
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
dal_longan_pie_rangeCheckIp_get(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    PIE_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                        LONGAN_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        LONGAN_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        LONGAN_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        LONGAN_IP_UPPERf,
                        &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        LONGAN_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        LONGAN_IP_LOWERf,
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
}

/* Function Name:
 *      dal_longan_pie_rangeCheckIp_set
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
dal_longan_pie_rangeCheckIp_set(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_write(unit,
                        LONGAN_RNG_CHK_IP_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        LONGAN_TYPEf,
                        &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        LONGAN_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        LONGAN_IP_UPPERf,
                        &pData->ip_upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        LONGAN_RNG_CHK_IP_RNGr,
                        REG_ARRAY_INDEX_NONE,
                        index,
                        LONGAN_IP_LOWERf,
                        &pData->ip_lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_rangeCheck_get
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
dal_longan_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* get value from CHIP */
    PIE_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, LONGAN_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, LONGAN_TYPEf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

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

    if ((ret = reg_array_field_read(unit, LONGAN_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, LONGAN_UPPERf,
            &pData->upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, LONGAN_LOWERf,
            &pData->lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_pie_rangeCheck_get */

/* Function Name:
 *      dal_longan_pie_rangeCheck_set
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
dal_longan_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, index=%d", unit, index);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "upper_bound=%d, upper_bound=%d",
            pData->upper_bound, pData->lower_bound);

    RT_PARAM_CHK((pData->lower_bound > pData->upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->type >= RTK_RNGCHK_END), RT_ERR_INPUT);

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

    if ((ret = reg_array_field_write(unit, LONGAN_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, LONGAN_TYPEf,
            &val)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, LONGAN_UPPERf,
            &pData->upper_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_RNG_CHK_CTRLr,
            REG_ARRAY_INDEX_NONE, index, LONGAN_LOWERf,
            &pData->lower_bound)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_pie_rangeCheck_set */

/* Function Name:
 *      dal_longan_pie_fieldSelector_get
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
dal_longan_pie_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, fs_idx=%d, pFs.start=%d, pFs.offset=%d", \
        unit, fs_idx, pFs->start, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit,
                        LONGAN_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        LONGAN_FMTf,
                        &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                        LONGAN_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        LONGAN_OFFSETf,
                        &pFs->offset)) != RT_ERR_OK)
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

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_fieldSelector_set
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
dal_longan_pie_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_PARAM_CHK((NULL == pFs), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, fs_idx=%d, pFs.start=%d, pFs.offset=%d", \
        unit, fs_idx, pFs->start, pFs->offset);

    /* Check init state */
    RT_INIT_CHK(pie_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK((fs_idx >= HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((pFs->start >= FS_START_POS_END), RT_ERR_INPUT);
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

    PIE_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        LONGAN_FMTf,
                        &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                        LONGAN_PARSER_FIELD_SELTOR_CTRLr,
                        REG_ARRAY_INDEX_NONE,
                        fs_idx,
                        LONGAN_OFFSETf,
                        &pFs->offset)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_meterIncludeIfg_get
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
dal_longan_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_METER_GLB_CTRLr, LONGAN_INC_IFGf, &value)) != RT_ERR_OK)
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
 *      dal_longan_pie_meterIncludeIfg_set
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
dal_longan_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, ifg_include=%d", unit, ifg_include);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

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

    PIE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, LONGAN_METER_GLB_CTRLr, LONGAN_INC_IFGf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_meterExceed_get
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
dal_longan_pie_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed)
{
    int32 ret = RT_ERR_FAILED;
    uint32 blockIdx = 0, entryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, meterIdx=%d", unit, meterIdx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((meterIdx >= HAL_MAX_NUM_OF_METERING(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    blockIdx = meterIdx / 16;
    entryIdx = meterIdx % 16;

    PIE_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_METER_LB_EXCEED_STSr,
                          blockIdx,
                          entryIdx,
                          LONGAN_LB_EXCEEDf,
                          pIsExceed)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    if(TRUE == *pIsExceed)
    {
        /* reset the flag */
        if ((ret = reg_array_field_write1toClear(unit,
                              LONGAN_METER_LB_EXCEED_STSr,
                              blockIdx,
                              entryIdx,
                              LONGAN_LB_EXCEEDf)) != RT_ERR_OK)
        {
            PIE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
            return ret;
        }
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "*pIsExceed=%d", *pIsExceed);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_meterExceedAggregation_get
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
dal_longan_pie_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pExceedMask), RT_ERR_NULL_POINTER);


    PIE_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_field_read(unit,
                          LONGAN_METER_LB_GLB_EXCEED_STSr,
                          LONGAN_LB_EXCEEDf,
                          pExceedMask)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "*pExceedMask=%d", *pExceedMask);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_meterEntry_get
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
dal_longan_pie_meterEntry_get(
    uint32          unit,
    uint32          meterIdx,
    rtk_pie_meterEntry_t   *pMeterEntry)
{
    int32           ret;
    uint32          value;
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
    if ((ret = table_read(unit, LONGAN_METERt, table_index, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    /* get TYPE */
    if ((ret = table_field_get( unit, LONGAN_METERt, LONGAN_METER_TYPEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
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
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }

    /* Get meter mode */
    if ((ret = table_field_get(unit, LONGAN_METERt, LONGAN_METER_MODEtf, &value,
            (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    switch (value)
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
    if ((ret = table_field_get( unit, LONGAN_METERt, LONGAN_METER_COLOR_AWAREtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->color_aware = value;


    /* get leaky bucket 0 rate */
    if ((ret = table_field_get( unit, LONGAN_METERt, LONGAN_METER_LB0_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb0_rate = value;

    /* get leaky bucket 0 burst size */
    if ((ret = table_field_get( unit, LONGAN_METERt, LONGAN_METER_LB0_BStf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb0_bs = value;

    /* get leaky bucket 1 rate */
    if ((ret = table_field_get( unit, LONGAN_METERt, LONGAN_METER_LB1_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb1_rate = value;

    /* get leaky bucket 1 burst size */
    if ((ret = table_field_get( unit, LONGAN_METERt, LONGAN_METER_LB1_BStf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    pMeterEntry->lb1_bs = value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "pMeterEntry->type=%d, pMeterEntry->color_aware=%d, pMeterEntry->lb0_rate=%d, pMeterEntry->lb1_rate=%d",
        pMeterEntry->type, pMeterEntry->color_aware, pMeterEntry->lb0_rate, pMeterEntry->lb1_rate);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_meterEntry_set
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
dal_longan_pie_meterEntry_set(
    uint32          unit,
    uint32          meterIdx,
    rtk_pie_meterEntry_t   *pMeterEntry)
{
    int32           ret;
    uint32          value;
    uint32          table_index;
    meter_entry_t   meter_entry;

    RT_PARAM_CHK((NULL == pMeterEntry), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, meterIdx=%d pMeterEntry->type=%d, \
        pMeterEntry->color_aware=%d, pMeterEntry->lb0_rate=%d, pMeterEntry->lb1_rate=%d",
        unit, meterIdx, pMeterEntry->type, pMeterEntry->color_aware, pMeterEntry->lb0_rate,
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
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_TYPEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set mode */
    switch (pMeterEntry->mode)
    {
        case METER_MODE_BYTE:
            value = 0;
            break;
        case METER_MODE_PACKET:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PIE), " ");
            return RT_ERR_FAILED;
    }
    if ((ret = table_field_set(unit, LONGAN_METERt, LONGAN_METER_MODEtf,
            &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set COLORAWARE */
    value = pMeterEntry->color_aware;
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_COLOR_AWAREtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 0 rate */
    value = pMeterEntry->lb0_rate;
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_LB0_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 0 burst size */
    value = pMeterEntry->lb0_bs;
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_LB0_BStf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 1 rate */
    value = pMeterEntry->lb1_rate;
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_LB1_RATEtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    /* set leaky bucket 1 burst size */
    value = pMeterEntry->lb1_bs;
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_LB1_BStf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, LONGAN_METERt, table_index, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    /*reset leaky bucket*/
    value = 0x1;
    if ((ret = table_field_set( unit, LONGAN_METERt, LONGAN_METER_RSTtf, &value, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_LOCK(unit);
    /* set entry to chip */
    if ((ret = table_write(unit, LONGAN_METERt, table_index, (uint32 *) &meter_entry)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_meterDpSel_get
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
 * Applicable:
 *      9300, 931x
 * Note:
 *      None
 */
int32
dal_longan_pie_meterDpSel_get(
    uint32                  unit,
    rtk_pie_meterDpSel_t    *pDpSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDpSel), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_PIE_MISCr, LONGAN_DP_SELf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_pie_meterDpSel_set
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
 * Applicable:
 *      9300, 931x
 * Note:
 *      None
 */
int32
dal_longan_pie_meterDpSel_set(
    uint32                  unit,
    rtk_pie_meterDpSel_t    dpSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, ifg_include=%d", unit, dpSel);

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
    if ((ret = reg_field_write(unit, LONGAN_PIE_MISCr, LONGAN_DP_SELf, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_pie_templateVlanSel_get
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
dal_longan_pie_templateVlanSel_get(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   *pVlanSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((phase > PIE_PHASE_IACL), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanSel), RT_ERR_NULL_POINTER);

    if(PIE_PHASE_VACL == phase)
    {
        switch(preTemplate_idx)
        {
            case 0:
                field = LONGAN_V_TMPLTE0_IOTAG_SELf;
                break;
            case 1:
                field = LONGAN_V_TMPLTE1_IOTAG_SELf;
                break;
            case 2:
                field = LONGAN_V_TMPLTE2_IOTAG_SELf;
                break;
            case 4:
                field = LONGAN_V_TMPLTE4_IOTAG_SELf;
                break;
        default:
            return RT_ERR_FAILED;
        }
    }
    else{
        switch(preTemplate_idx)
        {
            case 0:
                field = LONGAN_I_TMPLTE0_IOTAG_SELf;
                break;
            case 1:
                field = LONGAN_I_TMPLTE1_IOTAG_SELf;
                break;
            case 2:
                field = LONGAN_I_TMPLTE2_IOTAG_SELf;
                break;
            case 4:
                field = LONGAN_I_TMPLTE4_IOTAG_SELf;
                break;
        default:
            return RT_ERR_FAILED;
        }
    }

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_PIE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%d", value);

    if((PIE_PHASE_VACL == phase) && (value > 2))
        return RT_ERR_FAILED;

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
        case 3:
            *pVlanSel = TMPLTE_VLAN_SEL_SRC_FWD;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      dal_longan_pie_templateVlanSel_set
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
dal_longan_pie_templateVlanSel_set(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   vlanSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d, vlanSel=%d", unit, vlanSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((phase > PIE_PHASE_IACL), RT_ERR_INPUT);
    RT_PARAM_CHK((vlanSel >= TMPLTE_VLAN_SEL_END), RT_ERR_INPUT);

    if((PIE_PHASE_VACL == phase) && (vlanSel > TMPLTE_VLAN_SEL_FWD))
        return RT_ERR_FAILED;

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
        case TMPLTE_VLAN_SEL_SRC_FWD:
            value = 3;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if(PIE_PHASE_VACL == phase)
    {
        switch(preTemplate_idx)
        {
            case 0:
                field = LONGAN_V_TMPLTE0_IOTAG_SELf;
                break;
            case 1:
                field = LONGAN_V_TMPLTE1_IOTAG_SELf;
                break;
            case 2:
                field = LONGAN_V_TMPLTE2_IOTAG_SELf;
                break;
            case 4:
                field = LONGAN_V_TMPLTE4_IOTAG_SELf;
                break;
        default:
            return RT_ERR_FAILED;
        }
    }
    else{
        switch(preTemplate_idx)
        {
            case 0:
                field = LONGAN_I_TMPLTE0_IOTAG_SELf;
                break;
            case 1:
                field = LONGAN_I_TMPLTE1_IOTAG_SELf;
                break;
            case 2:
                field = LONGAN_I_TMPLTE2_IOTAG_SELf;
                break;
            case 4:
                field = LONGAN_I_TMPLTE4_IOTAG_SELf;
                break;
        default:
            return RT_ERR_FAILED;
        }
    }

    PIE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, LONGAN_PIE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/*
 * Validate fields and transfer logical field ID to physical field ID
 */
static int32 _dal_longan_pie_log2PhyTmplteField(uint32 unit, uint32 field_idx, rtk_pie_templateFieldType_t log_field_id, uint32 *phy_field_id)
{
    uint32  i;

    RT_PARAM_CHK((field_idx >= DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD), RT_ERR_ENTRY_INDEX);

    /* Check field type */
    for (i = 0; dal_longan_template_field_list[i].field_type != TMPLTE_FIELD_END; i++)
    {
        if (log_field_id == dal_longan_template_field_list[i].field_type)
        {
            /* Check field location */
            if ((dal_longan_template_field_list[i].valid_location != 0) &&
                ((dal_longan_template_field_list[i].valid_location & (1<<field_idx)) == 0))
            {
                return RT_ERR_PIE_FIELD_LOCATION;

            }
            /* Get the physical field ID */
            *phy_field_id = dal_longan_template_field_list[i].physical_id;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_PIE_FIELD_TYPE;
}

/* Function Name:
 *      dal_longan_pie_arpMacSel_get
 * Description:
 *      Get the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pArpMacSel  - pointer to ARP MAC select
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
dal_longan_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpMacSel), RT_ERR_NULL_POINTER);

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_PIE_MISCr, LONGAN_ARP_MAC_CTRLf,
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
}   /* end of dal_longan_pie_arpMacSel_get */

/* Function Name:
 *      dal_longan_pie_arpMacSel_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel)
{
    int32 ret = RT_ERR_FAILED;
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
    if ((ret = reg_field_write(unit, LONGAN_PIE_MISCr, LONGAN_ARP_MAC_CTRLf,
            &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_pie_arpMacSel_set */

/* Function Name:
 *      dal_longan_pie_templateVlanFmtSel_get
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
dal_longan_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32  value;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,phase=%d,template_idx=%d", unit, phase, template_idx);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PIE_PHASE_VACL != phase), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_TEMPLATE(unit) <= template_idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanFmtSel), RT_ERR_NULL_POINTER);

    /* function body */
    switch(template_idx)
    {
        case 0:
            field = LONGAN_V_TMPLTE0_IOTAG_FMTf;
            break;
        case 1:
            field = LONGAN_V_TMPLTE1_IOTAG_FMTf;
            break;
        case 2:
            field = LONGAN_V_TMPLTE2_IOTAG_FMTf;
            break;
        case 4:
            field = LONGAN_V_TMPLTE4_IOTAG_FMTf;
            break;
        case 5:
            field = LONGAN_V_TMPLTE5_IOTAG_FMTf;
            break;
        case 6:
            field = LONGAN_V_TMPLTE6_IOTAG_FMTf;
            break;
        case 7:
            field = LONGAN_V_TMPLTE7_IOTAG_FMTf;
            break;
        case 8:
            field = LONGAN_V_TMPLTE8_IOTAG_FMTf;
            break;
        case 9:
            field = LONGAN_V_TMPLTE9_IOTAG_FMTf;
            break;
    default:
        return RT_ERR_FAILED;
    }

    PIE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_PIE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }
    PIE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "val:%d", value);

    if (0 == value)
        *pVlanFmtSel = TMPLTE_VLAN_FMT_SEL_ORIGINAL;
    else
        *pVlanFmtSel = TMPLTE_VLAN_FMT_SEL_MODIFIED;

    return RT_ERR_OK;
}   /* end of dal_mango_pie_templateVlanFmtSel_get */

/* Function Name:
 *      dal_longan_pie_templateVlanFmtSel_set
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
dal_longan_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel)
{
    int32 ret = RT_ERR_FAILED;
    uint32 field;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PIE), "unit=%d,phase=%d,template_idx=%d,vlanFmtSel=%d", unit, phase, template_idx, vlanFmtSel);

    /* check Init status */
    RT_INIT_CHK(pie_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PIE_PHASE_VACL != phase), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PIE_TEMPLATE(unit) <= template_idx), RT_ERR_INPUT);
    RT_PARAM_CHK((TMPLTE_VLAN_FMT_SEL_END <= vlanFmtSel), RT_ERR_INPUT);

    /* function body */
    switch(template_idx)
    {
        case 0:
            field = LONGAN_V_TMPLTE0_IOTAG_FMTf;
            break;
        case 1:
            field = LONGAN_V_TMPLTE1_IOTAG_FMTf;
            break;
        case 2:
            field = LONGAN_V_TMPLTE2_IOTAG_FMTf;
            break;
        case 4:
            field = LONGAN_V_TMPLTE4_IOTAG_FMTf;
            break;
        case 5:
            field = LONGAN_V_TMPLTE5_IOTAG_FMTf;
            break;
        case 6:
            field = LONGAN_V_TMPLTE6_IOTAG_FMTf;
            break;
        case 7:
            field = LONGAN_V_TMPLTE7_IOTAG_FMTf;
            break;
        case 8:
            field = LONGAN_V_TMPLTE8_IOTAG_FMTf;
            break;
        case 9:
            field = LONGAN_V_TMPLTE9_IOTAG_FMTf;
            break;
    default:
        return RT_ERR_FAILED;
    }


    if (TMPLTE_VLAN_FMT_SEL_ORIGINAL == vlanFmtSel)
        value = 0;
    else
        value = 1;

    PIE_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_PIE_CTRLr, field, &value)) != RT_ERR_OK)
    {
        PIE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    PIE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_pie_templateVlanFmtSel_set */
/* Function Name:
 *      _dal_longan_pie_init_config
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
_dal_longan_pie_init_config(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;
    uint32  i,template_id, field_idx, value;

    if ((ret = dal_longan_pie_meterIncludeIfg_set(unit, RTK_DEFAULT_METER_IFG_INCLUDE_STATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
        return ret;
    }

    for (i = 0; i < HAL_MAX_NUM_OF_PIE_BLOCK(unit); ++i)
    {
        if ((ret = dal_longan_pie_phase_set(unit, i, PIE_PHASE_VACL)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PIE), "blk:%d", i);
            return ret;
        }
    }
    /*init all template field type as invalid type*/
    /* set value to CHIP */
    for(template_id = HAL_PIE_USER_TEMPLATE_ID_MIN(unit); template_id <= HAL_PIE_USER_TEMPLATE_ID_MAX(unit); template_id++)
    {
        for (field_idx = 0; field_idx < DAL_LONGAN_MAX_NUM_OF_TEMPLATE_FIELD; field_idx++)
        {
            value = 0x3f;

            PIE_SEM_LOCK(unit);
            if ((ret = reg_array_field_write(unit, LONGAN_PIE_TMPLTE_CTRLr, template_id, field_idx, LONGAN_TMPLTE_FIELDf, &value)) != RT_ERR_OK)
            {
                PIE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_PIE), "");
                return ret;
            }
            PIE_SEM_UNLOCK(unit);
        }
    }

    return RT_ERR_OK;
} /* end of _dal_longan_pie_init_config */
