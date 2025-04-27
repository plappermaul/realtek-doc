/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public statistic APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) statistic counter reset
 *           2) statistic counter get
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
#include <ioal/mem32.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_stat.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/stat.h>

/*
 * Symbol Definition
 */
typedef struct counter_info_s
{
    uint32  reg_index;
    uint32  field_index;
    uint32  word_count;
} counter_info_t;

/*
 * Data Declaration
 */
static uint32               stat_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         stat_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* stat semaphore handling */
#define STAT_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(stat_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_STAT),"semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define STAT_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(stat_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_STAT), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
static int32 _dal_maple_stat_init_config(uint32 unit);


/* Module Name : STAT */

/* Function Name:
 *      dal_maple_statMapper_init
 * Description:
 *      Hook stat module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook stat module before calling any stat APIs.
 */
int32
dal_maple_statMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stat_init = dal_maple_stat_init;
    pMapper->stat_enable_get = dal_maple_stat_enable_get;
    pMapper->stat_enable_set = dal_maple_stat_enable_set;
    pMapper->stat_global_reset = dal_maple_stat_global_reset;
    pMapper->stat_port_reset = dal_maple_stat_port_reset;
    pMapper->stat_global_get = dal_maple_stat_global_get;
    pMapper->stat_global_getAll = dal_maple_stat_global_getAll;
    pMapper->stat_port_get = dal_maple_stat_port_get;
    pMapper->stat_port_getAll = dal_maple_stat_port_getAll;
    pMapper->stat_tagLenCntIncEnable_get = dal_maple_stat_tagLenCntIncEnable_get;
    pMapper->stat_tagLenCntIncEnable_set = dal_maple_stat_tagLenCntIncEnable_set;
    pMapper->stat_tagLenCntIncEnable_get = dal_maple_stat_tagLenCntIncEnable_get;
    pMapper->stat_tagLenCntIncEnable_set = dal_maple_stat_tagLenCntIncEnable_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stat module before calling any vlan APIs.
 */
int32
dal_maple_stat_init(uint32 unit)
{
    rtk_enable_t enable;
    int32   ret;

    RT_INIT_REENTRY_CHK(stat_init[unit]);
    stat_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    stat_sem[unit] = osal_sem_mutex_create();
    if (0 == stat_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, MOD_STP, "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    stat_init[unit] = INIT_COMPLETED;


    if (( ret = _dal_maple_stat_init_config(unit)) != RT_ERR_OK)
    {
        stat_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }


    enable = ENABLED;
    if (( ret = dal_maple_stat_enable_set(unit, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "Init MIB Counters enabled failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_stat_init */

/* Function Name:
 *      dal_maple_stat_enable_get
 * Description:
 *      Get the status to check whether MIB counters are enabled or not.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of enable or disable MIB counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_stat_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_STAT_CTRLr, MAPLE_ALL_CNTR_ENf, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_maple_stat_enable_get */

/* Function Name:
 *      dal_maple_stat_enable_set
 * Description:
 *      Set the status to enable or disable MIB counters
 * Input:
 *      unit   - unit id
 *      enable - enable or disable MIB counters
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
dal_maple_stat_enable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else
        val = 1;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_STAT_CTRLr, MAPLE_ALL_CNTR_ENf, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_stat_enable_set */

/* Function Name:
 *      dal_maple_stat_global_reset
 * Description:
 *      Reset the global counters in the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_maple_stat_global_reset(uint32 unit)
{
    int32   ret;
    uint32  loop;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    value = 1;

    STAT_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_STAT_RSTr, MAPLE_RST_GLOBAL_MIBf, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, MAPLE_STAT_RSTr, MAPLE_RST_GLOBAL_MIBf, &value)) == RT_ERR_OK)
            && (0 == value))
        {
            break;
        }
    }
    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_stat_global_reset */

/* Function Name:
 *      dal_maple_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_maple_stat_port_reset(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  loop;
    uint32  value = 0, flag;
    rtk_portmask_t  resetPortmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&resetPortmask, 0, sizeof(resetPortmask));
    RTK_PORTMASK_PORT_SET(resetPortmask, port);
    flag = 0x1;

    STAT_SEM_LOCK(unit);

    /* set entry to CHIP*/
    if ((ret = reg_read(unit, MAPLE_STAT_PORT_RSTr, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MAPLE_STAT_PORT_RSTr, MAPLE_RST_PORT_FLAGf, &flag, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MAPLE_STAT_PORT_RSTr, MAPLE_RST_PORT_MIBf, &port, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_write(unit, MAPLE_STAT_PORT_RSTr, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, MAPLE_STAT_PORT_RSTr, MAPLE_RST_PORT_FLAGf, &value)) == RT_ERR_OK) && (0 == value))
        {
            break;
        }
    }
    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_stat_port_reset*/

/* Function Name:
 *      dal_maple_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      unit     - unit id
 *      cntr_idx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
dal_maple_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    switch (cntr_idx)
    {
        case DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX:
            if ((ret = reg_field_read(unit, MAPLE_STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDSr,
                            MAPLE_DOT1DTPLEARNEDENTRYDISCARDSf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_STAT_INVALID_GLOBAL_CNTR;
    }

    *pCntr = value;

    return RT_ERR_OK;
} /*dal_maple_stat_global_get  */

/* Function Name:
 *      dal_maple_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pGlobal_cntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
dal_maple_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs)
{
    uint64  counter;
    int32   ret;

    rtk_stat_global_type_t counter_idx;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGlobal_cntrs), RT_ERR_NULL_POINTER);

    for (counter_idx = DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX; counter_idx < MIB_GLOBAL_CNTR_END; counter_idx++)
    {
        ret = dal_maple_stat_global_get(unit, counter_idx, &counter);
        if (RT_ERR_STAT_INVALID_GLOBAL_CNTR == ret)
            continue;
        if (RT_ERR_OK != ret)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }

        switch (counter_idx)
        {
            case DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX:
                pGlobal_cntrs->dot1dTpLearnedEntryDiscards = counter;
                break;
            default:
                break;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "dot1dTpLearnedEntryDiscards=%ul", pGlobal_cntrs->dot1dTpLearnedEntryDiscards);

    return RT_ERR_OK;
}/* end of dal_maple_stat_global_getAll */

/* Function Name:
 *      dal_maple_stat_port_get
 * Description:
 *      Get one specified port counter in the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      cntr_idx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE           - input parameter out of range
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Note:
 *      None
 */
int32
dal_maple_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr)
{
    int32   ret;
    uint32  reg;
    uint32  cntr_H, cntr_L, cntr_Tx, cntr_Rx;
    uint32  cntr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, port=%d, cntr_idx=%d", unit, port, cntr_idx);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(cntr_idx >= MIB_PORT_CNTR_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    reg = MAPLE_STAT_PORT_STANDARD_MIBr;
    switch (cntr_idx)
    {
        case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT1DTPPORTINDISCARDSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_IN_OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINOCTETS_Lf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINOCTETS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_HC_IN_OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINOCTETS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINOCTETS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;
        case IF_IN_UCAST_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINUCASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_IN_MULTICAST_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINMULTICASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_IN_BROADCAST_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINBROADCASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_OUT_OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTOCTETS_Lf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTOCTETS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_HC_OUT_OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTOCTETS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTOCTETS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;
        case IF_OUT_UCAST_PKTS_CNT_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTUCASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTMULTICASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTBROADCASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_OUT_DISCARDS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFOUTDISCARDSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3STATSSINGLECOLLISIONFRAMESf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3STATSMULTIPLECOLLISIONFRAMESf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3STATSDEFERREDTRANSMISSIONSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_LATE_COLLISIONS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3STATSLATECOLLISIONSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3STATSEXCESSIVECOLLISIONSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_SYMBOL_ERRORS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3STATSSYMBOLERRORSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3CONTROLINUNKNOWNOPCODESf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_IN_PAUSE_FRAMES_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3INPAUSEFRAMESf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_OUT_PAUSE_FRAMES_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DOT3OUTPAUSEFRAMESf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_DROP_EVENTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_ETHERSTATSDROPEVENTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_BROADCAST_PKTS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINBROADCASTPKTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSBROADCASTPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSBROADCASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_MULTICAST_PKTS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IFINMULTICASTPKTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSMULTICASTPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSMULTICASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_ETHERSTATSCRCALIGNERRORSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_UNDER_SIZE_PKTS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSUNDERSIZEPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSUNDERSIZEPKTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSUNDERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSUNDERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSUNDERSIZEDROPPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_OVERSIZE_PKTS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSOVERSIZEPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSOVERSIZEPKTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSOVERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSOVERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_FRAGMENTS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_ETHERSTATSFRAGMENTSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_JABBERS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_ETHERSTATSJABBERSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_COLLISIONS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_ETHERSTATSCOLLISIONSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_64OCTETS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS64OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS64OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS64OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS64OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_65TO127OCTETS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS65TO127OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS65TO127OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS65TO127OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS65TO127OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;


        case ETHER_STATS_PKTS_128TO255OCTETS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS128TO255OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS128TO255OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS128TO255OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS128TO255OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_256TO511OCTETS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS256TO511OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS256TO511OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS256TO511OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS256TO511OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_PKTS_512TO1023OCTETS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX: /* TX+RX */
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_TX_ETHERSTATSPKTS1519TOMAXOCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RX_ETHERSTATSPKTS1519TOMAXOCTETSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case RX_MAC_DISCARDS_INDEX:
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_RXMACDISCARDSf, &cntr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        default:
            return RT_ERR_STAT_INVALID_PORT_CNTR;
    }

    return RT_ERR_OK;
} /* end of dal_maple_stat_port_get */

/* Function Name:
 *      dal_maple_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPort_cntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Note:
 *      None
 */
int32
dal_maple_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    int32   ret;
    uint64  counter;
    rtk_stat_port_type_t    counter_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPort_cntrs), RT_ERR_NULL_POINTER);

    for (counter_idx = IF_IN_OCTETS_INDEX; counter_idx < MIB_PORT_CNTR_END; counter_idx++)
    {
        counter = 0;

        ret = dal_maple_stat_port_get(unit, port, counter_idx, &counter);
        if (RT_ERR_STAT_INVALID_PORT_CNTR == ret)
            continue;
        if (RT_ERR_OK != ret)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }

        switch (counter_idx)
        {
            case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
                pPort_cntrs->dot1dTpPortInDiscards = counter;
                break;
            case IF_IN_OCTETS_INDEX:
                pPort_cntrs->ifInOctets = counter;
                break;
            case IF_HC_IN_OCTETS_INDEX:
                pPort_cntrs->ifHCInOctets = counter;
                break;
            case IF_IN_UCAST_PKTS_INDEX:
                pPort_cntrs->ifInUcastPkts = counter;
                break;
            case IF_IN_MULTICAST_PKTS_INDEX:
                pPort_cntrs->ifInMulticastPkts = counter;
                break;
            case IF_IN_BROADCAST_PKTS_INDEX:
                pPort_cntrs->ifInBroadcastPkts = counter;
                break;
            case IF_OUT_OCTETS_INDEX:
                pPort_cntrs->ifOutOctets = counter;
                break;
            case IF_HC_OUT_OCTETS_INDEX:
                pPort_cntrs->ifHCOutOctets = counter;
                break;
            case IF_OUT_UCAST_PKTS_CNT_INDEX:
                pPort_cntrs->ifOutUcastPkts = counter;
                break;
            case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
                pPort_cntrs->ifOutMulticastPkts = counter;
                break;
            case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
                pPort_cntrs->ifOutBrocastPkts = counter;
                break;
            case IF_OUT_DISCARDS_INDEX:
                pPort_cntrs->ifOutDiscards = counter;
                break;
            case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
                pPort_cntrs->dot3StatsSingleCollisionFrames = counter;
                break;
            case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
                pPort_cntrs->dot3StatsMultipleCollisionFrames = counter;
                break;
            case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
                pPort_cntrs->dot3StatsDeferredTransmissions = counter;
                break;
            case DOT3_STATS_LATE_COLLISIONS_INDEX:
                pPort_cntrs->dot3StatsLateCollisions = counter;
                break;
            case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
                pPort_cntrs->dot3StatsExcessiveCollisions = counter;
                break;
            case DOT3_STATS_SYMBOL_ERRORS_INDEX:
                pPort_cntrs->dot3StatsSymbolErrors = counter;
                break;
            case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
                pPort_cntrs->dot3ControlInUnknownOpcodes = counter;
                break;
            case DOT3_IN_PAUSE_FRAMES_INDEX:
                pPort_cntrs->dot3InPauseFrames = counter;
                break;
            case DOT3_OUT_PAUSE_FRAMES_INDEX:
                pPort_cntrs->dot3OutPauseFrames = counter;
                break;
            case ETHER_STATS_DROP_EVENTS_INDEX:
                pPort_cntrs->etherStatsDropEvents = counter;
                break;
            case ETHER_STATS_BROADCAST_PKTS_INDEX:
                pPort_cntrs->etherStatsBroadcastPkts = counter;
                break;
            case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
                pPort_cntrs->etherStatsTxBroadcastPkts = counter;
                break;
            case ETHER_STATS_MULTICAST_PKTS_INDEX:
                pPort_cntrs->etherStatsMulticastPkts = counter;
                break;
            case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
                pPort_cntrs->etherStatsTxMulticastPkts = counter;
                break;
            case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
                pPort_cntrs->etherStatsCRCAlignErrors = counter;
                break;
            case ETHER_STATS_UNDER_SIZE_PKTS_INDEX:
                pPort_cntrs->etherStatsUndersizePkts = counter;
                break;
            case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
                pPort_cntrs->etherStatsTxUndersizePkts = counter;
                break;
            case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
                pPort_cntrs->etherStatsRxUndersizePkts = counter;
                break;
            case ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX:
                pPort_cntrs->etherStatsRxUndersizeDropPkts = counter;
                break;
            case ETHER_STATS_OVERSIZE_PKTS_INDEX:
                pPort_cntrs->etherStatsOversizePkts = counter;
                break;
            case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
                pPort_cntrs->etherStatsTxOversizePkts = counter;
                break;
            case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
                pPort_cntrs->etherStatsRxOversizePkts = counter;
                break;
            case ETHER_STATS_FRAGMENTS_INDEX:
                pPort_cntrs->etherStatsFragments = counter;
                break;
            case ETHER_STATS_JABBERS_INDEX:
                pPort_cntrs->etherStatsJabbers = counter;
                break;
            case ETHER_STATS_COLLISIONS_INDEX:
                pPort_cntrs->etherStatsCollisions = counter;
                break;
            case ETHER_STATS_PKTS_64OCTETS_INDEX:
                pPort_cntrs->etherStatsPkts64Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts64Octets = counter;
                break;
            case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts64Octets = counter;
                break;
            case ETHER_STATS_PKTS_65TO127OCTETS_INDEX:
                pPort_cntrs->etherStatsPkts65to127Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts65to127Octets = counter;
                break;
            case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts65to127Octets = counter;
                break;
            case ETHER_STATS_PKTS_128TO255OCTETS_INDEX:
                pPort_cntrs->etherStatsPkts128to255Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts128to255Octets = counter;
                break;
            case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts128to255Octets = counter;
                break;
            case ETHER_STATS_PKTS_256TO511OCTETS_INDEX:
                pPort_cntrs->etherStatsPkts256to511Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts256to511Octets = counter;
                break;
            case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts256to511Octets = counter;
                break;
            case ETHER_STATS_PKTS_512TO1023OCTETS_INDEX:
                pPort_cntrs->etherStatsPkts512to1023Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts512to1023Octets = counter;
                break;
            case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts512to1023Octets = counter;
                break;
            case ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX:
                pPort_cntrs->etherStatsPkts1024to1518Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts1024to1518Octets = counter;
                break;
            case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts1024to1518Octets = counter;
                break;
            case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts1519toMaxOctets = counter;
                break;
            case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts1519toMaxOctets = counter;
                break;
            case RX_MAC_DISCARDS_INDEX:
                pPort_cntrs->rxMacDiscards = counter;
                break;
            default:
                break;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_stat_port_getAll */

/* Function Name:
 *      dal_maple_stat_tagLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
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
dal_maple_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  field, val;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, tagCnt_type=%d"
            , unit, tagCnt_type);

    /* parameter check */
    RT_PARAM_CHK((tagCnt_type >= TAG_CNT_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (TAG_CNT_TYPE_RX == tagCnt_type)
        field = MAPLE_RX_CNT_TAGf;
    else
        field = MAPLE_TX_CNT_TAGf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_STAT_CTRLr, field, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_maple_stat_tagLenCntIncEnable_get */

/* Function Name:
 *      dal_maple_stat_tagLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
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
dal_maple_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable)
{
    int32   ret;
    uint32  field, val;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, tagCnt_type=%d, enable=%x"
            , unit, tagCnt_type, enable);

    /* parameter check */
    RT_PARAM_CHK((tagCnt_type >= TAG_CNT_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else
        val = 1;

    if (TAG_CNT_TYPE_RX == tagCnt_type)
        field = MAPLE_RX_CNT_TAGf;
    else
        field = MAPLE_TX_CNT_TAGf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_STAT_CTRLr, field, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_stat_tagLenCntIncEnable_set */



/* Function Name:
 *      _dal_maple_stat_init_config
 * Description:
 *      Initialize default configuration for  the statistic module of the specified device..
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
_dal_maple_stat_init_config(uint32 unit)
{
    int32   ret;
    uint32  val;

    val = 0x1;

    /* set debug counter always enable*/
    if ((ret = reg_field_write(unit, MAPLE_STAT_CTRLr, MAPLE_DBG_CNTR_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_stat_init_config */


