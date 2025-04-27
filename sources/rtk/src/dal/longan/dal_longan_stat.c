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
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/longan/dal_longan_stat.h>
#include <rtk/default.h>
#include <rtk/stat.h>

/*
 * Symbol Definition
 */

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
static int32 _dal_longan_stat_init_config(uint32 unit);


/* Module Name : STAT */

/* Function Name:
 *      dal_longan_statMapper_init
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
dal_longan_statMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stat_init = dal_longan_stat_init;
    pMapper->stat_global_reset = dal_longan_stat_global_reset;
    pMapper->stat_port_reset = dal_longan_stat_port_reset;
    pMapper->stat_global_get = dal_longan_stat_global_get;
    pMapper->stat_global_getAll = dal_longan_stat_global_getAll;
    pMapper->stat_port_get = dal_longan_stat_port_get;
    pMapper->stat_port_getAll = dal_longan_stat_port_getAll;
    pMapper->stat_tagLenCntIncEnable_get = dal_longan_stat_tagLenCntIncEnable_get;
    pMapper->stat_tagLenCntIncEnable_set = dal_longan_stat_tagLenCntIncEnable_set;
    pMapper->stat_stackHdrLenCntIncEnable_get = dal_longan_stat_stackHdrLenCntIncEnable_get;
    pMapper->stat_stackHdrLenCntIncEnable_set = dal_longan_stat_stackHdrLenCntIncEnable_set;
    pMapper->stat_flexibleCntRange_get = dal_longan_stat_flexibleCntRange_get;
    pMapper->stat_flexibleCntRange_set = dal_longan_stat_flexibleCntRange_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_stat_init
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
dal_longan_stat_init(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;

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


    if (( ret = _dal_longan_stat_init_config(unit)) != RT_ERR_OK)
    {
        stat_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_longan_stat_init */

/* Function Name:
 *      dal_longan_stat_global_reset
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
dal_longan_stat_global_reset(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;
    uint32  loop;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    value = 1;

    STAT_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_STAT_RSTr, LONGAN_RST_GLB_MIBf, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, LONGAN_STAT_RSTr, LONGAN_RST_GLB_MIBf, &value)) == RT_ERR_OK)
            && (0 == value))
        {
            break;
        }
    }
    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_stat_global_reset */

/* Function Name:
 *      dal_longan_stat_port_reset
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
dal_longan_stat_port_reset(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_read(unit, LONGAN_STAT_PORT_RSTr, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, LONGAN_STAT_PORT_RSTr, LONGAN_RST_PORT_FLAGf, &flag, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, LONGAN_STAT_PORT_RSTr, LONGAN_RST_PORT_MIBf, &port, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_STAT_PORT_RSTr, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, LONGAN_STAT_PORT_RSTr, LONGAN_RST_PORT_FLAGf, &value)) == RT_ERR_OK) && (0 == value))
        {
            break;
        }
    }

    /*reset egress queue counters */
    if ((ret = reg_read(unit, LONGAN_STAT_PORT_PRVTE_E_Q_RSTr, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, LONGAN_STAT_PORT_PRVTE_E_Q_RSTr, LONGAN_RST_FLAGf, &flag, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, LONGAN_STAT_PORT_PRVTE_E_Q_RSTr, LONGAN_RST_PORT_IDXf, &port, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_STAT_PORT_PRVTE_E_Q_RSTr, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, LONGAN_STAT_PORT_PRVTE_E_Q_RSTr, LONGAN_RST_FLAGf, &value)) == RT_ERR_OK) && (0 == value))
        {
            break;
        }
    }

    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_stat_port_reset*/

/* Function Name:
 *      dal_longan_stat_global_get
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
dal_longan_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    STAT_SEM_LOCK(unit);

    switch (cntr_idx)
    {
        case DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX:
            if ((ret = reg_field_read(unit, LONGAN_STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDSr,
                            LONGAN_DOT1DTPLEARNEDENTRYDISCARDSf, &value)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            break;
        default:
            STAT_SEM_UNLOCK(unit);
            return RT_ERR_STAT_INVALID_GLOBAL_CNTR;
    }

    *pCntr = value;

    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /*dal_longan_stat_global_get  */

/* Function Name:
 *      dal_longan_stat_global_getAll
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
dal_longan_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs)
{
    uint64  counter;
    int32   ret = RT_ERR_FAILED;

    rtk_stat_global_type_t counter_idx;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGlobal_cntrs), RT_ERR_NULL_POINTER);

    for (counter_idx = DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX; counter_idx < MIB_GLOBAL_CNTR_END; counter_idx++)
    {
        ret = dal_longan_stat_global_get(unit, counter_idx, &counter);
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
}/* end of dal_longan_stat_global_getAll */


int32
_dal_longan_stat_port_egrQueueDropCnt_get(uint32 unit, rtk_port_t port, uint32 qId, uint32 *pCntr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 reg;
    uint32 cntr;

    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    /*port0~23  8 queue*/
    /*port24~27  12 queue*/
    /*port28    32 queue*/
    if (port < 24)
    {
        RT_PARAM_CHK(qId >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_STAT_INVALID_PORT_CNTR);
        reg = LONGAN_STAT_PORT_E_DROP_CNTR0r;
    }
    else if (port < 28)
    {
        RT_PARAM_CHK(qId >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_STAT_INVALID_PORT_CNTR);
        reg = LONGAN_STAT_PORT_E_DROP_CNTR1r;
    }
    else if (28 == port)
    {
        RT_PARAM_CHK(qId >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_STAT_INVALID_PORT_CNTR);
        reg = LONGAN_STAT_PORT_E_DROP_CNTR2r;
    }
    else
        return RT_ERR_PORT_ID;

    if ((ret = reg_array_field_read(unit, reg, port, qId, LONGAN_EGRQUEUEDROPPKTRTf, &cntr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    *pCntr = cntr;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_stat_port_get
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
dal_longan_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr)
{
    int32   ret = RT_ERR_FAILED;
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

    STAT_SEM_LOCK(unit);

    switch (cntr_idx)
    {
        /*Section1==LONGAN_STAT_PORT_MIB_CNTRr*/
        case IF_IN_OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINOCTETS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINOCTETS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINOCTETS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTOCTETS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_OUT_OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTOCTETS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTOCTETS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_IN_UCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINUCASTPKTS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_UCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINUCASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINUCASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_IN_MULTICAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINMULTICASTPKTS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_MULTICAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINMULTICASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINMULTICASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

     case IF_IN_BROADCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINBROADCASTPKTS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_BROADCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINBROADCASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINBROADCASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

     case IF_OUT_UCAST_PKTS_CNT_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTUCASTPKTS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_OUT_UCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTUCASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTUCASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

     case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTMULTICASTPKTS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

     case IF_HC_OUT_MULTICAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTMULTICASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTMULTICASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

     case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTBROADCASTPKTS_Lf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

     case IF_HC_OUT_BROADCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTBROADCASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTBROADCASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_DISCARDS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFOUTDISCARDSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT1DTPPORTINDISCARDSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3STATSSINGLECOLLISIONFRAMESf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3STATSMULTIPLECOLLISIONFRAMESf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3STATSDEFERREDTRANSMISSIONSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_LATE_COLLISIONS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3STATSLATECOLLISIONSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3STATSEXCESSIVECOLLISIONSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_SYMBOL_ERRORS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3STATSSYMBOLERRORSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3CONTROLINUNKNOWNOPCODESf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_IN_PAUSE_FRAMES_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3INPAUSEFRAMESf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_OUT_PAUSE_FRAMES_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_DOT3OUTPAUSEFRAMESf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_DROP_EVENTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_ETHERSTATSDROPEVENTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_BROADCAST_PKTS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINBROADCASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINBROADCASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSBROADCASTPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) + ((uint64)cntr_L) + ((uint64)cntr_Tx) ;
            break;

        case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSBROADCASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_MULTICAST_PKTS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINMULTICASTPKTS_Lf, &cntr_L)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_IFINMULTICASTPKTS_Hf, &cntr_H)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSMULTICASTPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) + ((uint64)cntr_L) + ((uint64)cntr_Tx) ;
            break;

        case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSMULTICASTPKTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSCRCALIGNERRORSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }

             if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSCRCALIGNERRORSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Tx + (uint64)cntr_Rx;
            break;

        case TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSCRCALIGNERRORSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSCRCALIGNERRORSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_UNDER_SIZE_PKTS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSUNDERSIZEPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSUNDERSIZEPKTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSUNDERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSUNDERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_OVERSIZE_PKTS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSOVERSIZEPKTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSOVERSIZEPKTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSOVERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSOVERSIZEPKTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_FRAGMENTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSFRAGMENTSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSFRAGMENTSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case TX_ETHER_STATS_FRAGMENTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSFRAGMENTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_FRAGMENTS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSFRAGMENTSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_JABBERS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSJABBERSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSJABBERSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case TX_ETHER_STATS_JABBERS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSJABBERSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_JABBERS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSJABBERSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_COLLISIONS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSCOLLISIONSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_64OCTETS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS64OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS64OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS64OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS64OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_65TO127OCTETS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS65TO127OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS65TO127OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS65TO127OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS65TO127OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_128TO255OCTETS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS128TO255OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS128TO255OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS128TO255OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS128TO255OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_256TO511OCTETS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS256TO511OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS256TO511OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS256TO511OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS256TO511OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_512TO1023OCTETS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS512TO1023OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX: /* TX+RX */
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr_Tx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr_Rx)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr_Rx + (uint64)cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_MIB_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS1024TO1518OCTETSf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        /*Section2=LONGAN_STAT_PORT_PRVTE_CNTRr*/
        case ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSUNDERSIZEDROPPKTSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTS1519TOMAXOCTETSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTS1519TOMAXOCTETSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_TX_PKTS_OVERMAXOCTETS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTSOVERMAXOCTETSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_OVERMAXOCTETS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTSOVERMAXOCTETSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTSFLEXIBLEOCTETSSET1RTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTSFLEXIBLEOCTETSSET1RTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

       case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET1RTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET1RTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTSFLEXIBLEOCTETSSET0RTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTSFLEXIBLEOCTETSSET0RTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

       case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_TX_ETHERSTATSPKTSFLEXIBLEOCTETSCRSET0CRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RX_ETHERSTATSPKTSFLEXIBLEOCTETSCRSET0CRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_LENGTH_FIELD_ERROR_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_LENGTHFIELDERRORRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_FALSE_CARRIER_TIMES_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_FALSECARRIERTIMESRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_UNDER_SIZE_OCTETS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_UNDERSIZEOCTETSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_FRAMING_ERRORS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_FRAMINGERRORSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_PARSER_ERROR_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_PARSERERRORSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

         case RX_MAC_IPGSHORTDROP_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RXMACIPGSHORTDROPRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_MAC_DISCARDS_INDEX:
            reg = LONGAN_STAT_PORT_PRVTE_CNTRr;
            if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,
                    LONGAN_RXMACDISCARDSRTf, &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_0_DROP_PKTS_INDEX:
        case TX_QUEUE_1_DROP_PKTS_INDEX:
        case TX_QUEUE_2_DROP_PKTS_INDEX:
        case TX_QUEUE_3_DROP_PKTS_INDEX:
        case TX_QUEUE_4_DROP_PKTS_INDEX:
        case TX_QUEUE_5_DROP_PKTS_INDEX:
        case TX_QUEUE_6_DROP_PKTS_INDEX:
        case TX_QUEUE_7_DROP_PKTS_INDEX:
        case TX_QUEUE_8_DROP_PKTS_INDEX:
        case TX_QUEUE_9_DROP_PKTS_INDEX:
        case TX_QUEUE_10_DROP_PKTS_INDEX:
        case TX_QUEUE_11_DROP_PKTS_INDEX:
        case TX_QUEUE_12_DROP_PKTS_INDEX:
        case TX_QUEUE_13_DROP_PKTS_INDEX:
        case TX_QUEUE_14_DROP_PKTS_INDEX:
        case TX_QUEUE_15_DROP_PKTS_INDEX:
        case TX_QUEUE_16_DROP_PKTS_INDEX:
        case TX_QUEUE_17_DROP_PKTS_INDEX:
        case TX_QUEUE_18_DROP_PKTS_INDEX:
        case TX_QUEUE_19_DROP_PKTS_INDEX:
        case TX_QUEUE_20_DROP_PKTS_INDEX:
        case TX_QUEUE_21_DROP_PKTS_INDEX:
        case TX_QUEUE_22_DROP_PKTS_INDEX:
        case TX_QUEUE_23_DROP_PKTS_INDEX:
        case TX_QUEUE_24_DROP_PKTS_INDEX:
        case TX_QUEUE_25_DROP_PKTS_INDEX:
        case TX_QUEUE_26_DROP_PKTS_INDEX:
        case TX_QUEUE_27_DROP_PKTS_INDEX:
        case TX_QUEUE_28_DROP_PKTS_INDEX:
        case TX_QUEUE_29_DROP_PKTS_INDEX:
        case TX_QUEUE_30_DROP_PKTS_INDEX:
        case TX_QUEUE_31_DROP_PKTS_INDEX:
            if ((ret = _dal_longan_stat_port_egrQueueDropCnt_get(unit, port,    \
                            (uint32)(cntr_idx-TX_QUEUE_0_DROP_PKTS_INDEX), &cntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        default:
            STAT_SEM_UNLOCK(unit);
            return RT_ERR_STAT_INVALID_PORT_CNTR;
    }

    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_stat_port_get */

/* Function Name:
 *      dal_longan_stat_port_getAll
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
dal_longan_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    int32   ret = RT_ERR_FAILED;
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

        ret = dal_longan_stat_port_get(unit, port, counter_idx, &counter);
        if (RT_ERR_STAT_INVALID_PORT_CNTR == ret)
            continue;
        if (RT_ERR_OK != ret)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }

        switch (counter_idx)
        {
            case IF_IN_OCTETS_INDEX:
                pPort_cntrs->ifInOctets = counter;
                break;
            case IF_HC_IN_OCTETS_INDEX:
                pPort_cntrs->ifHCInOctets = counter;
                break;

            case IF_IN_UCAST_PKTS_INDEX:
                pPort_cntrs->ifInUcastPkts = counter;
                break;
            case IF_HC_IN_UCAST_PKTS_INDEX:
                pPort_cntrs->ifHCInUcastPkts = counter;
                break;

            case IF_IN_MULTICAST_PKTS_INDEX:
                pPort_cntrs->ifInMulticastPkts = counter;
                break;
            case IF_HC_IN_MULTICAST_PKTS_INDEX:
                pPort_cntrs->ifHCInMulticastPkts = counter;
                break;

            case IF_IN_BROADCAST_PKTS_INDEX:
                pPort_cntrs->ifInBroadcastPkts = counter;
                break;
            case IF_HC_IN_BROADCAST_PKTS_INDEX:
                pPort_cntrs->ifHCInBroadcastPkts = counter;
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
            case IF_HC_OUT_UCAST_PKTS_INDEX:
                pPort_cntrs->ifHCOutUcastPkts = counter;
                break;

            case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
                pPort_cntrs->ifOutMulticastPkts = counter;
                break;
            case IF_HC_OUT_MULTICAST_PKTS_INDEX:
                pPort_cntrs->ifHCOutMulticastPkts = counter;
                break;

            case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
                pPort_cntrs->ifOutBrocastPkts = counter;
                break;
            case IF_HC_OUT_BROADCAST_PKTS_INDEX:
                pPort_cntrs->ifHCOutBrocastPkts = counter;
                break;

            case IF_OUT_DISCARDS_INDEX:
                pPort_cntrs->ifOutDiscards = counter;
                break;

            case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
                pPort_cntrs->dot1dTpPortInDiscards = counter;
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
            case TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
                pPort_cntrs->txEtherStatsCRCAlignErrors = counter;
                break;
            case RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
                pPort_cntrs->rxEtherStatsCRCAlignErrors = counter;
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
            case TX_ETHER_STATS_FRAGMENTS_INDEX:
                pPort_cntrs->txEtherStatsFragments = counter;
                break;
            case RX_ETHER_STATS_FRAGMENTS_INDEX:
                pPort_cntrs->rxEtherStatsFragments = counter;
                break;

            case ETHER_STATS_JABBERS_INDEX:
                pPort_cntrs->etherStatsJabbers = counter;
                break;
            case TX_ETHER_STATS_JABBERS_INDEX:
                pPort_cntrs->txEtherStatsJabbers = counter;
                break;
            case RX_ETHER_STATS_JABBERS_INDEX:
                pPort_cntrs->rxEtherStatsJabbers = counter;
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

            case ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX:
                pPort_cntrs->etherStatsRxUndersizeDropPkts = counter;
                break;

            case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
                pPort_cntrs->etherStatsTxPkts1519toMaxOctets = counter;
                break;
            case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
                pPort_cntrs->etherStatsRxPkts1519toMaxOctets = counter;
                break;

            case ETHER_STATS_TX_PKTS_OVERMAXOCTETS_INDEX:
                pPort_cntrs->etherStatsTxPktsOverMaxOctets = counter;
                break;
            case ETHER_STATS_RX_PKTS_OVERMAXOCTETS_INDEX:
                pPort_cntrs->etherStatsRxPktsOverMaxOctets = counter;
                break;

            case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX:
                pPort_cntrs->txEtherStatsPktsFlexibleOctetsSet1RT = counter;
                break;

            case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX:
                pPort_cntrs->rxEtherStatsPktsFlexibleOctetsSet1RT = counter;
                break;

            case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX:
                pPort_cntrs->txetherStatsPktsFlexibleOctetsCRCSet1RT = counter;
                break;

            case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX:
                pPort_cntrs->rxEtherStatsPktsFlexibleOctetsCRCSet1RT = counter;
                break;

            case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX:
                pPort_cntrs->txEtherStatsPktsFlexibleOctetsSet0RT = counter;
                break;

            case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX:
                pPort_cntrs->rxEtherStatsPktsFlexibleOctetsSet0RT = counter;
                break;

            case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX:
                pPort_cntrs->txetherStatsPktsFlexibleOctetsCRCSet0RT = counter;
                break;

            case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX:
                pPort_cntrs->rxEtherStatsPktsFlexibleOctetsCRCSet0RT = counter;
                break;

            case RX_LENGTH_FIELD_ERROR_INDEX:
                pPort_cntrs->rxLengthFieldError = counter;
                break;

            case RX_FALSE_CARRIER_TIMES_INDEX:
                pPort_cntrs->rxFalseCarrierTimes = counter;
                break;

            case RX_UNDER_SIZE_OCTETS_INDEX:
                pPort_cntrs->rxUnderSizeOctets = counter;
                break;

            case RX_FRAMING_ERRORS_INDEX:
                pPort_cntrs->rxFramingErrors = counter;
                break;

            case RX_PARSER_ERROR_INDEX:
                pPort_cntrs->parserErrors = counter;
                break;

            case RX_MAC_IPGSHORTDROP_INDEX:
                pPort_cntrs->rxMacIPGShortDrop = counter;
                break;

            case RX_MAC_DISCARDS_INDEX:
                pPort_cntrs->rxMacDiscards = counter;
                break;

            case TX_QUEUE_0_DROP_PKTS_INDEX:
            case TX_QUEUE_1_DROP_PKTS_INDEX:
            case TX_QUEUE_2_DROP_PKTS_INDEX:
            case TX_QUEUE_3_DROP_PKTS_INDEX:
            case TX_QUEUE_4_DROP_PKTS_INDEX:
            case TX_QUEUE_5_DROP_PKTS_INDEX:
            case TX_QUEUE_6_DROP_PKTS_INDEX:
            case TX_QUEUE_7_DROP_PKTS_INDEX:
            case TX_QUEUE_8_DROP_PKTS_INDEX:
            case TX_QUEUE_9_DROP_PKTS_INDEX:
            case TX_QUEUE_10_DROP_PKTS_INDEX:
            case TX_QUEUE_11_DROP_PKTS_INDEX:
            case TX_QUEUE_12_DROP_PKTS_INDEX:
            case TX_QUEUE_13_DROP_PKTS_INDEX:
            case TX_QUEUE_14_DROP_PKTS_INDEX:
            case TX_QUEUE_15_DROP_PKTS_INDEX:
            case TX_QUEUE_16_DROP_PKTS_INDEX:
            case TX_QUEUE_17_DROP_PKTS_INDEX:
            case TX_QUEUE_18_DROP_PKTS_INDEX:
            case TX_QUEUE_19_DROP_PKTS_INDEX:
            case TX_QUEUE_20_DROP_PKTS_INDEX:
            case TX_QUEUE_21_DROP_PKTS_INDEX:
            case TX_QUEUE_22_DROP_PKTS_INDEX:
            case TX_QUEUE_23_DROP_PKTS_INDEX:
            case TX_QUEUE_24_DROP_PKTS_INDEX:
            case TX_QUEUE_25_DROP_PKTS_INDEX:
            case TX_QUEUE_26_DROP_PKTS_INDEX:
            case TX_QUEUE_27_DROP_PKTS_INDEX:
            case TX_QUEUE_28_DROP_PKTS_INDEX:
            case TX_QUEUE_29_DROP_PKTS_INDEX:
            case TX_QUEUE_30_DROP_PKTS_INDEX:
            case TX_QUEUE_31_DROP_PKTS_INDEX:
                pPort_cntrs->egrQueueDropPkts[counter_idx-TX_QUEUE_0_DROP_PKTS_INDEX] = counter;
                break;

            default:
                break;
        }
    }

    return RT_ERR_OK;
} /* end of dal_longan_stat_port_getAll */

/* Function Name:
 *      dal_longan_stat_tagLenCntIncEnable_get
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
dal_longan_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field, val;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, tagCnt_type=%d"
            , unit, tagCnt_type);

    /* parameter check */
    RT_PARAM_CHK((tagCnt_type >= TAG_CNT_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (TAG_CNT_TYPE_RX == tagCnt_type)
        field = LONGAN_RX_CNT_TAGf;
    else
        field = LONGAN_TX_CNT_TAGf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_STAT_CTRLr, field, &val)) != RT_ERR_OK)
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
} /* end of dal_longan_stat_tagLenCntIncEnable_get */

/* Function Name:
 *      dal_longan_stat_tagLenCntIncEnable_set
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
dal_longan_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
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
        field = LONGAN_RX_CNT_TAGf;
    else
        field = LONGAN_TX_CNT_TAGf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_STAT_CTRLr, field, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_longan_stat_tagLenCntIncEnable_set */


/* Function Name:
 *      dal_longan_stat_stackHdrLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 */
int32
dal_longan_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  field, val;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d", unit, type);

    /* parameter check */
    RT_PARAM_CHK((type >= STACK_HDR_CNT_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (STACK_HDR_CNT_TYPE_RX == type)
        field = LONGAN_RX_STACK_CNT_HDRf;
    else
        field = LONGAN_TX_STACK_CNT_HDRf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_STAT_CTRLr, field, &val)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_stat_stackHdrLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude stacking header length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 */
int32
dal_longan_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  field, val;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d, enable=%x", unit, type, enable);

    /* parameter check */
    RT_PARAM_CHK((type >= STACK_HDR_CNT_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else
        val = 1;

    if (STACK_HDR_CNT_TYPE_RX == type)
        field = LONGAN_RX_STACK_CNT_HDRf;
    else
        field = LONGAN_TX_STACK_CNT_HDRf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_STAT_CTRLr, field, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_longan_stat_flexibleCntRange_get
 * Description:
 *      Get the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 * Output:
 *      pRange     - pointer buffer of the boundary value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 */
int32
dal_longan_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg, field_min, field_max;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, idx=%d", unit, idx);

    /* parameter check */
    RT_PARAM_CHK((pRange == NULL), RT_ERR_INPUT);

    switch (idx)
    {
        case 0:
            reg = LONGAN_STAT_CNT_SET0_CTRLr;
            field_min = LONGAN_CNT_SET0_LEN_MINf;
            field_max = LONGAN_CNT_SET0_LEN_MAXf;
            break;
        case 1:
            reg = LONGAN_STAT_CNT_SET1_CTRLr;
            field_min = LONGAN_CNT_SET1_LEN_MINf;
            field_max = LONGAN_CNT_SET1_LEN_MAXf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, reg, field_min, &(pRange->len_min))) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, reg, field_max, &(pRange->len_max))) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "len_min=%d, len_max=%d",
           pRange->len_min, pRange->len_max);

    return RT_ERR_OK;

} /* end of dal_longan_stat_flexibleCntRange_get */

/* Function Name:
 *      rtk_longan_stat_flexibleCntRange_set
 * Description:
 *      Set the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 *      pRange      - the pointer of the boundary value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 */
int32
dal_longan_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg, field_min, field_max;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, idx=%d, len_min=%d, len_max=%d",
            unit, idx, pRange->len_min, pRange->len_max);

    /* parameter check */
    RT_PARAM_CHK((pRange == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pRange->len_min > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pRange->len_max > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_INPUT);

    switch (idx)
    {
        case 0:
            reg = LONGAN_STAT_CNT_SET0_CTRLr;
            field_min = LONGAN_CNT_SET0_LEN_MINf;
            field_max = LONGAN_CNT_SET0_LEN_MAXf;
            break;
        case 1:
            reg = LONGAN_STAT_CNT_SET1_CTRLr;
            field_min = LONGAN_CNT_SET1_LEN_MINf;
            field_max = LONGAN_CNT_SET1_LEN_MAXf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    STAT_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, reg, field_min, &(pRange->len_min))) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, reg, field_max, &(pRange->len_max))) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "len_min=%d, len_max=%d",
           pRange->len_min, pRange->len_max);

    return RT_ERR_OK;

} /* end of dal_longan_stat_flexibleCntRange_set */


/* Function Name:
 *      _dal_longan_stat_init_config
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
_dal_longan_stat_init_config(uint32 unit)
{
    return RT_ERR_OK;
} /* end of _dal_longan_stat_init_config */


