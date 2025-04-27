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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_stat.h>
#include <rtk/default.h>
#include <rtk/stat.h>

/*
 * Symbol Definition
 */
#if 0
typedef struct counter_info_s
{
    uint32  reg_index;
    uint32  field_index;
    uint32  word_count;
} counter_info_t;
#endif

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
static int32 _dal_mango_stat_init_config(uint32 unit);


/* Module Name : STAT */

/* Function Name:
 *      dal_mango_statMapper_init
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
dal_mango_statMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stat_init = dal_mango_stat_init;
    pMapper->stat_global_reset = dal_mango_stat_global_reset;
    pMapper->stat_port_reset = dal_mango_stat_port_reset;
    pMapper->stat_global_get = dal_mango_stat_global_get;
    pMapper->stat_global_getAll = dal_mango_stat_global_getAll;
    pMapper->stat_port_get = dal_mango_stat_port_get;
    pMapper->stat_port_getAll = dal_mango_stat_port_getAll;
    pMapper->stat_tagLenCntIncEnable_get = dal_mango_stat_tagLenCntIncEnable_get;
    pMapper->stat_tagLenCntIncEnable_set = dal_mango_stat_tagLenCntIncEnable_set;
    pMapper->stat_stackHdrLenCntIncEnable_get = dal_mango_stat_stackHdrLenCntIncEnable_get;
    pMapper->stat_stackHdrLenCntIncEnable_set = dal_mango_stat_stackHdrLenCntIncEnable_set;
    pMapper->stat_flexibleCntRange_get = dal_mango_stat_flexibleCntRange_get;
    pMapper->stat_flexibleCntRange_set = dal_mango_stat_flexibleCntRange_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_stat_init
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
dal_mango_stat_init(uint32 unit)
{
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

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if (( ret = _dal_mango_stat_init_config(unit)) != RT_ERR_OK)
        {
            stat_init[unit] = INIT_NOT_COMPLETED;
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_mango_stat_init */

/* Function Name:
 *      dal_mango_stat_global_reset
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
dal_mango_stat_global_reset(uint32 unit)
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
    if ((ret = reg_field_write(unit, MANGO_STAT_RSTr, MANGO_RST_GLB_MIBf, &value)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, MANGO_STAT_RSTr, MANGO_RST_GLB_MIBf, &value)) == RT_ERR_OK)
            && (0 == value))
        {
            break;
        }
    }
    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_stat_global_reset */

/* Function Name:
 *      dal_mango_stat_port_reset
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
dal_mango_stat_port_reset(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  loop;
    uint32  value, flag = 0x1;
    rtk_portmask_t  resetPortmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&resetPortmask, 0, sizeof(resetPortmask));
    RTK_PORTMASK_PORT_SET(resetPortmask, port);

    STAT_SEM_LOCK(unit);

    /* set entry to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_STAT_PORT_RSTr, MANGO_RST_PORT_MIBf, &port)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_STAT_PORT_RSTr, MANGO_RST_PORT_FLAGf, &flag)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    for (loop = 0; loop < 0xFFFFFFFF; loop++)
    {
        if (((ret = reg_field_read(unit, MANGO_STAT_PORT_RSTr, MANGO_RST_PORT_FLAGf, &value)) == RT_ERR_OK) && (0 == value))
        {
            break;
        }
    }
    STAT_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_stat_port_reset*/

/* Function Name:
 *      dal_mango_stat_global_get
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
dal_mango_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr)
{
    int32 ret;
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
            if ((ret = reg_field_read(unit, MANGO_STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDSr,
                            MANGO_DOT1DTPLEARNEDENTRYDISCARDSf, &value)) != RT_ERR_OK)
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

    STAT_SEM_UNLOCK(unit);

    *pCntr = value;

    return RT_ERR_OK;
} /*dal_mango_stat_global_get  */

/* Function Name:
 *      dal_mango_stat_global_getAll
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
dal_mango_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs)
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
        ret = dal_mango_stat_global_get(unit, counter_idx, &counter);
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
}/* end of dal_mango_stat_global_getAll */


/* Function Name:
 *      dal_mango_stat_port_get
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
dal_mango_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr)
{
    int32   ret;
    uint32  cntr_H, cntr_L, cntr_Tx, cntr_Rx, cntr;
    mib_portCntr_entry_t portCntr;
    mib_portPrvteCntr_entry_t portPrvteCntr;
    mib_portPrvteEQCntr_entry_t portPrvteEQCntr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, port=%d, cntr_idx=%d", unit, port, cntr_idx);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(cntr_idx >= MIB_PORT_CNTR_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    osal_memset(&portCntr, 0, sizeof(mib_portCntr_entry_t));
    osal_memset(&portPrvteCntr, 0, sizeof(mib_portPrvteCntr_entry_t));
    osal_memset(&portPrvteEQCntr, 0, sizeof(mib_portPrvteEQCntr_entry_t));

    STAT_SEM_LOCK(unit);

    if ((ret = table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, port, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    if ((ret = table_read(unit, MANGO_STAT_PORT_PRVTE_CNTRt, port, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

     if ((ret = table_read(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt, port, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    switch (cntr_idx)
    {
        /*Section1==MANGO_STAT_PORT_MIB_CNTRr*/
        case IF_IN_OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINOCTETS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINOCTETS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINOCTETS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_OUT_OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_IN_UCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_UCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_IN_MULTICAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_MULTICAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_IN_BROADCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_IN_BROADCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_UCAST_PKTS_CNT_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case IF_HC_OUT_UCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTMULTICASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_OUT_MULTICAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTMULTICASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTMULTICASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTBROADCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case IF_HC_OUT_BROADCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTBROADCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTBROADCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);
            break;

        case IF_OUT_DISCARDS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFOUTDISCARDStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT1DTPPORTINDISCARDStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3STATSSINGLECOLLISIONFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3STATSMULTIPLECOLLISIONFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3STATSDEFERREDTRANSMISSIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_LATE_COLLISIONS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3STATSLATECOLLISIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3STATSEXCESSIVECOLLISIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_STATS_SYMBOL_ERRORS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3STATSSYMBOLERRORStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3CONTROLINUNKNOWNOPCODEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case DOT3_IN_PAUSE_FRAMES_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3INPAUSEFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case DOT3_OUT_PAUSE_FRAMES_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_DOT3OUTPAUSEFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_DROP_EVENTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_ETHERSTATSDROPEVENTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_BROADCAST_PKTS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSBROADCASTPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Ltf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSBROADCASTPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_MULTICAST_PKTS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSMULTICASTPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Ltf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSMULTICASTPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSCRCALIGNERRORStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSCRCALIGNERRORStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSCRCALIGNERRORStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSCRCALIGNERRORStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_UNDER_SIZE_PKTS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSUNDERSIZEPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSUNDERSIZEPKTStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSUNDERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSUNDERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_OVERSIZE_PKTS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSOVERSIZEPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSOVERSIZEPKTStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;
        case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSOVERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;
        case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSOVERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_FRAGMENTS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSFRAGMENTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSFRAGMENTStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case TX_ETHER_STATS_FRAGMENTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSFRAGMENTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_FRAGMENTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSFRAGMENTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_JABBERS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSJABBERStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSJABBERStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case TX_ETHER_STATS_JABBERS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSJABBERStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_JABBERS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSJABBERStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_COLLISIONS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSCOLLISIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_64OCTETS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS64OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS64OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS64OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS64OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_65TO127OCTETS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS65TO127OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS65TO127OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS65TO127OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS65TO127OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_128TO255OCTETS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS128TO255OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS128TO255OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS128TO255OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS128TO255OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_256TO511OCTETS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS256TO511OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS256TO511OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS256TO511OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS256TO511OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_512TO1023OCTETS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX: /* TX+RX */
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = cntr_Rx + cntr_Tx;
            break;

        case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                    MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSUNDERSIZEDROPPKTSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTS1519TOMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTS1519TOMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_TX_PKTS_OVERMAXOCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSOVERMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case ETHER_STATS_RX_PKTS_OVERMAXOCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSOVERMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_LENGTH_FIELD_ERROR_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_LENGTHFIELDERRORRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_FALSE_CARRIER_TIMES_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_FALSECARRIERTIMESRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_UNDER_SIZE_OCTETS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_UNDERSIZEOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_FRAMING_ERRORS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_FRAMINGERRORSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_MAC_IPGSHORTDROP_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RXMACIPGSHORTDROPRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case RX_MAC_DISCARDS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_RXMACDISCARDSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_0_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE0OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_1_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE1OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_2_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE2OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_3_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE3OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_4_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE4OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_5_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE5OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_6_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE6OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_7_OUT_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
                    MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE7OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_0_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE0DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_1_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE1DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_2_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE2DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_3_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE3DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_4_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE4DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_5_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE5DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_6_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE6DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
            *pCntr = (uint64)cntr;
            break;

        case TX_QUEUE_7_DROP_PKTS_INDEX:
            if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
                    MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE7DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
            {
                STAT_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
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
} /* end of dal_mango_stat_port_get */

/* Function Name:
 *      dal_mango_stat_port_getAll
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
dal_mango_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    int32   ret;
    uint32  cntr_H, cntr_L, cntr_Tx, cntr_Rx, cntr;
    mib_portCntr_entry_t portCntr;
    mib_portPrvteCntr_entry_t portPrvteCntr;
    mib_portPrvteEQCntr_entry_t portPrvteEQCntr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPort_cntrs), RT_ERR_NULL_POINTER);

    osal_memset(&portCntr, 0, sizeof(mib_portCntr_entry_t));
    osal_memset(&portPrvteCntr, 0, sizeof(mib_portPrvteCntr_entry_t));
    osal_memset(&portPrvteEQCntr, 0, sizeof(mib_portPrvteEQCntr_entry_t));

    STAT_SEM_LOCK(unit);

    if ((ret = table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, port, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    if ((ret = table_read(unit, MANGO_STAT_PORT_PRVTE_CNTRt, port, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    if ((ret = table_read(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt, port, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    /* Get each counter */
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                 MANGO_STAT_PORT_MIB_CNTR_IFINOCTETS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
         STAT_SEM_UNLOCK(unit);
         RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
         return ret;
    }
    pPort_cntrs->ifInOctets = (uint64)cntr;        
         
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINOCTETS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINOCTETS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCInOctets = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);            

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifInUcastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCInUcastPkts = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifInMulticastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCInMulticastPkts = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifInBroadcastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCInBroadcastPkts = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifOutOctets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCOutOctets = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
                MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifOutUcastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCOutUcastPkts = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTMULTICASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifOutMulticastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTMULTICASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTMULTICASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCOutMulticastPkts = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTBROADCASTPKTS_Ltf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifOutBrocastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTBROADCASTPKTS_Htf, &cntr_H, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTBROADCASTPKTS_Ltf, &cntr_L, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifHCOutBrocastPkts = (((uint64)cntr_H) << 32) | ((uint64)cntr_L);

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFOUTDISCARDStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->ifOutDiscards = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3STATSSINGLECOLLISIONFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3StatsSingleCollisionFrames = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3STATSMULTIPLECOLLISIONFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3StatsMultipleCollisionFrames = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3STATSDEFERREDTRANSMISSIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3StatsDeferredTransmissions = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3STATSLATECOLLISIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3StatsLateCollisions = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3STATSEXCESSIVECOLLISIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3StatsExcessiveCollisions = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3STATSSYMBOLERRORStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3StatsSymbolErrors = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3CONTROLINUNKNOWNOPCODEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3ControlInUnknownOpcodes = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3INPAUSEFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3InPauseFrames = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT3OUTPAUSEFRAMEStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot3OutPauseFrames = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_DOT1DTPPORTINDISCARDStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->dot1dTpPortInDiscards = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_ETHERSTATSDROPEVENTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsDropEvents = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSBROADCASTPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFINBROADCASTPKTS_Ltf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsBroadcastPkts = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSBROADCASTPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxBroadcastPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSMULTICASTPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_IFINMULTICASTPKTS_Ltf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsMulticastPkts = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSMULTICASTPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxMulticastPkts = (uint64)cntr;
    
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSCRCALIGNERRORStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSCRCALIGNERRORStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsCRCAlignErrors = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSCRCALIGNERRORStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txEtherStatsCRCAlignErrors = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSCRCALIGNERRORStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsCRCAlignErrors = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSUNDERSIZEPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSUNDERSIZEPKTStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsUndersizePkts = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSUNDERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxUndersizePkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSUNDERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxUndersizePkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSOVERSIZEPKTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSOVERSIZEPKTStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsOversizePkts = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSOVERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxOversizePkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSOVERSIZEPKTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxOversizePkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSFRAGMENTStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSFRAGMENTStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsFragments = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSFRAGMENTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txEtherStatsFragments = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSFRAGMENTStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsFragments = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSJABBERStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSJABBERStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsJabbers = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSJABBERStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txEtherStatsJabbers = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSJABBERStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsJabbers = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSCOLLISIONStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsCollisions = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS64OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS64OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsPkts64Octets = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS64OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts64Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS64OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts64Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS65TO127OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS65TO127OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsPkts65to127Octets = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS65TO127OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts65to127Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS65TO127OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts65to127Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS128TO255OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS128TO255OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsPkts128to255Octets = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS128TO255OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts128to255Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS128TO255OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts128to255Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS256TO511OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS256TO511OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsPkts256to511Octets = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS256TO511OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts256to511Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS256TO511OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts256to511Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsPkts512to1023Octets = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts512to1023Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS512TO1023OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts512to1023Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr_Tx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr_Rx, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsPkts1024to1518Octets = cntr_Rx + cntr_Tx;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_TX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts1024to1518Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt,
            MANGO_STAT_PORT_MIB_CNTR_RX_ETHERSTATSPKTS1024TO1518OCTETStf, &cntr, (uint32 *) &portCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts1024to1518Octets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSUNDERSIZEDROPPKTSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxUndersizeDropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTS1519TOMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPkts1519toMaxOctets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTS1519TOMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPkts1519toMaxOctets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSOVERMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsTxPktsOverMaxOctets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSOVERMAXOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->etherStatsRxPktsOverMaxOctets = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txEtherStatsPktsFlexibleOctetsSet1RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsPktsFlexibleOctetsSet1RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txetherStatsPktsFlexibleOctetsCRCSet1RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET1RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsPktsFlexibleOctetsCRCSet1RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txEtherStatsPktsFlexibleOctetsSet0RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsPktsFlexibleOctetsSet0RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_TX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->txetherStatsPktsFlexibleOctetsCRCSet0RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RX_ETHERSTATSPKTSFLEXIBLEOCTETSCRCSET0RTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxEtherStatsPktsFlexibleOctetsCRCSet0RT = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_LENGTHFIELDERRORRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxLengthFieldError = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_FALSECARRIERTIMESRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxFalseCarrierTimes = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_UNDERSIZEOCTETSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxUnderSizeOctets = (uint64)cntr;
    
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_FRAMINGERRORSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxFramingErrors = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RXMACIPGSHORTDROPRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxMacIPGShortDrop = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_RXMACDISCARDSRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->rxMacDiscards = (uint64)cntr;
    
    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE0OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue0OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE1OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue1OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE2OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue2OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE3OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue3OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE4OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue4OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE5OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue5OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE6OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue6OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_CNTRt,
            MANGO_STAT_PORT_PRVTE_CNTR_EGRQUEUE7OUTPKTRTtf, &cntr, (uint32 *) &portPrvteCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue7OutPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE0DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue0DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE1DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue1DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE2DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue2DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE3DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue3DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE4DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue4DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE5DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue5DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE6DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue6DropPkts = (uint64)cntr;

    if ((ret = table_field_get(unit, MANGO_STAT_PORT_PRVTE_E_Q_CNTRt,
            MANGO_STAT_PORT_PRVTE_E_Q_CNTR_EGRQUEUE7DROPPKTRTtf, &cntr, (uint32 *) &portPrvteEQCntr)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    pPort_cntrs->egrQueue7DropPkts = (uint64)cntr;
    
    STAT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_mango_stat_port_getAll */


/* Function Name:
 *      dal_mango_stat_tagLenCntIncEnable_get
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
dal_mango_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable)
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
        field = MANGO_RX_CNT_TAGf;
    else
        field = MANGO_TX_CNT_TAGf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_STAT_CTRLr, field, &val)) != RT_ERR_OK)
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
} /* end of dal_mango_stat_tagLenCntIncEnable_get */

/* Function Name:
 *      dal_mango_stat_tagLenCntIncEnable_set
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
dal_mango_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable)
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
        field = MANGO_RX_CNT_TAGf;
    else
        field = MANGO_TX_CNT_TAGf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, MANGO_STAT_CTRLr, field, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_stat_stackHdrLenCntIncEnable_get
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
dal_mango_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable)
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
        field = MANGO_RX_STACK_CNT_HDRf;
    else
        field = MANGO_TX_STACK_CNT_HDRf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_STAT_CTRLr, field, &val)) != RT_ERR_OK)
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
 *      dal_mango_stat_stackHdrLenCntIncEnable_set
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
dal_mango_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable)
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
        field = MANGO_RX_STACK_CNT_HDRf;
    else
        field = MANGO_TX_STACK_CNT_HDRf;

    STAT_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, MANGO_STAT_CTRLr, field, &val)) != RT_ERR_OK)
    {
        STAT_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    STAT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_stat_flexibleCntRange_get
 * Description:
 *      Get the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 * Output:
 *      pRange      - pointer buffer of the boundary value
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
dal_mango_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    int32   ret;
    uint32  reg, field_min, field_max;

    /* check Init status */
    RT_INIT_CHK(stat_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STAT), "unit=%d, idx=%d", unit, idx);

    /* parameter check */
    RT_PARAM_CHK((pRange == NULL), RT_ERR_INPUT);

    switch (idx)
    {
        case 0:
            reg = MANGO_STAT_CNT_SET0_CTRLr;
            field_min = MANGO_CNT_SET0_LEN_MINf;
            field_max = MANGO_CNT_SET0_LEN_MAXf;
            break;
        case 1:
            reg = MANGO_STAT_CNT_SET1_CTRLr;
            field_min = MANGO_CNT_SET1_LEN_MINf;
            field_max = MANGO_CNT_SET1_LEN_MAXf;
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
}

/* Function Name:
 *      rtk_mango_stat_flexibleCntRange_set
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
dal_mango_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    int32   ret;
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
            reg = MANGO_STAT_CNT_SET0_CTRLr;
            field_min = MANGO_CNT_SET0_LEN_MINf;
            field_max = MANGO_CNT_SET0_LEN_MAXf;
            break;
        case 1:
            reg = MANGO_STAT_CNT_SET1_CTRLr;
            field_min = MANGO_CNT_SET1_LEN_MINf;
            field_max = MANGO_CNT_SET1_LEN_MAXf;
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

    return RT_ERR_OK;
}


/* Function Name:
 *      _dal_mango_stat_init_config
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
_dal_mango_stat_init_config(uint32 unit)
{
    return RT_ERR_OK;
} /* end of _dal_mango_stat_init_config */


