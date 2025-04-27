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
 * Purpose : Definition those public TRUNK APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Trunk
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
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_trunk.h>
#include <dal/maple/dal_maple_port.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/trunk.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               trunk_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         trunk_sem[RTK_MAX_NUM_OF_UNIT];
static rtk_portmask_t       *pTrunkMemberSet[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* trunk semaphore handling */
#define TRUNK_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(trunk_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_TRUNK), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define TRUNK_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(trunk_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_TRUNK), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
static int32 _dal_maple_trunk_init_config(uint32 unit);

/* Function Name:
 *      dal_maple_trunkMapper_init
 * Description:
 *      Hook trunk module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook trunk module before calling any trunk APIs.
 */
int32
dal_maple_trunkMapper_init(dal_mapper_t *pMapper)
{
    pMapper->trunk_init = dal_maple_trunk_init;
    pMapper->trunk_distributionAlgorithmBind_get = dal_maple_trunk_distributionAlgorithmBind_get;
    pMapper->trunk_distributionAlgorithmBind_set = dal_maple_trunk_distributionAlgorithmBind_set;
    pMapper->trunk_distributionAlgorithmParam_get = dal_maple_trunk_distributionAlgorithmParam_get;
    pMapper->trunk_distributionAlgorithmParam_set = dal_maple_trunk_distributionAlgorithmParam_set;
    pMapper->trunk_distributionAlgorithmShift_get = dal_maple_trunk_distributionAlgorithmShift_get;
    pMapper->trunk_distributionAlgorithmShift_set = dal_maple_trunk_distributionAlgorithmShift_set;
    pMapper->trunk_port_get = dal_maple_trunk_port_get;
    pMapper->trunk_port_set = dal_maple_trunk_port_set;
    pMapper->trunk_trafficSeparate_get = dal_maple_trunk_trafficSeparate_get;
    pMapper->trunk_trafficSeparate_set = dal_maple_trunk_trafficSeparate_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trunk_init
 * Description:
 *      Initialize trunk module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize trunk module before calling any trunk APIs.
 */
int32
dal_maple_trunk_init(uint32 unit)
{
    int32   ret;

    RT_INIT_REENTRY_CHK(trunk_init[unit]);
    trunk_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    trunk_sem[unit] = osal_sem_mutex_create();
    if (0 == trunk_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_TRUNK), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory for trunk database for this unit */
    pTrunkMemberSet[unit] = (rtk_portmask_t *)osal_alloc(HAL_MAX_NUM_OF_TRUNK(unit)*sizeof(rtk_portmask_t));
    if (NULL == pTrunkMemberSet[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_TRUNK|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    /* reset trunk member */
    osal_memset(pTrunkMemberSet[unit], 0, HAL_MAX_NUM_OF_TRUNK(unit)*sizeof(rtk_portmask_t));

    /* set init flag to complete init */
    trunk_init[unit] = INIT_COMPLETED;

    /* initialize default configuration */
    if ((ret = _dal_maple_trunk_init_config(unit)) != RT_ERR_OK)
    {
        trunk_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pTrunkMemberSet[unit]);
        pTrunkMemberSet[unit] = NULL;
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "init default configuration failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_trunk_init */

/* Function Name:
 *      dal_maple_trunk_distributionAlgorithmBind_get
 * Description:
 *      Get the distribution algorithm ID binded for a trunk group from the specified device.
 * Input:
 *      unit      - unit id
 *      trk_gid   - trunk group id
 * Output:
 *      pAlgo_idx - pointer buffer of the distribution algorithm ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trunk_distributionAlgorithmBind_get(uint32 unit, uint32 trk_gid, uint32 *pAlgo_idx)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, trk_gid=%d",
           unit, trk_gid);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pAlgo_idx), RT_ERR_NULL_POINTER);

    TRUNK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_IDX_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          trk_gid,
                          MAPLE_HASH_MSK_IDXf,
                          pAlgo_idx)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "pAlgo_idx=%x",
           *pAlgo_idx);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_distributionAlgorithmBind_get */

/* Function Name:
 *      dal_maple_trunk_distributionAlgorithmBind_set
 * Description:
 *      Set the distribution algorithm ID binded for a trunk group from the specified device.
 * Input:
 *      unit     - unit id
 *      trk_gid  - trunk group id
 *      algo_idx - index the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 *      RT_ERR_LA_ALGO_ID  - invalid trunk algorithm id
 * Note:
 *      None
 */
int32
dal_maple_trunk_distributionAlgorithmBind_set(uint32 unit, uint32 trk_gid, uint32 algo_idx)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, trk_gid=%d, algo_idx=%d",
           unit, trk_gid, algo_idx);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK(algo_idx >= HAL_MAX_NUM_OF_TRUNK_ALGO(unit), RT_ERR_LA_ALGO_ID);

    TRUNK_SEM_LOCK(unit);

    /* set to CHIP*/
    val = algo_idx;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_IDX_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          trk_gid,
                          MAPLE_HASH_MSK_IDXf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_distributionAlgorithmBind_set */

/* Function Name:
 *      dal_maple_trunk_distributionAlgorithmParam_get
 * Description:
 *      Get the distribution algorithm by algorithm ID from the specified device.
 * Input:
 *      unit          - unit id
 *      algo_idx      - algorithm index
 * Output:
 *      pAlgo_bitmask - pointer buffer of bitmask of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_ALGO_ID   - invalid trunk algorithm id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trunk_distributionAlgorithmParam_get(uint32 unit, uint32 algo_idx, uint32 *pAlgo_bitmask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, algo_idx=%d",
           unit, algo_idx);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(algo_idx >= HAL_MAX_NUM_OF_TRUNK_ALGO(unit), RT_ERR_LA_ALGO_ID);
    RT_PARAM_CHK((NULL == pAlgo_bitmask), RT_ERR_NULL_POINTER);

    TRUNK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_HASH_MSKf,
                          pAlgo_bitmask)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "pAlgo_bitmask=%x",
           *pAlgo_bitmask);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_distributionAlgorithmParam_get */

/* Function Name:
 *      dal_maple_trunk_distributionAlgorithmParam_set
 * Description:
 *      Set the distribution algorithm by algorithm ID from the specified device.
 * Input:
 *      unit         - unit id
 *      algo_idx     - algorithm index
 *      algo_bitmask - bitmask of the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_LA_ALGO_ID  - invalid trunk algorithm id
 *      RT_ERR_LA_HASHMASK - invalid hash mask
 * Note:
 *      None
 */
int32
dal_maple_trunk_distributionAlgorithmParam_set(uint32 unit, uint32 algo_idx, uint32 algo_bitmask)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, algo_idx=%d, algo_bitmask=%d",
           unit, algo_idx, algo_bitmask);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(algo_idx >= HAL_MAX_NUM_OF_TRUNK_ALGO(unit), RT_ERR_LA_ALGO_ID);
    RT_PARAM_CHK((algo_bitmask > TRUNK_DISTRIBUTION_ALGO_MASKALL), RT_ERR_LA_HASHMASK);

    TRUNK_SEM_LOCK(unit);

    /* get entry from CHIP*/
    val = algo_bitmask;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_HASH_MSKf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_distributionAlgorithmParam_set */

/* Function Name:
 *      dal_maple_trunk_distributionAlgorithmShift_get
 * Description:
 *      Get the shift bits of distribution algorithm parameters from the specified device.
 * Input:
 *      unit     - unit id
 *      algo_idx - algorithm index
 * Output:
 *      pShift   - pointer buffer of shift bits of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_ALGO_ID   - invalid trunk algorithm id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trunk_distributionAlgorithmShift_get(uint32 unit, uint32 algo_idx, rtk_trunk_distAlgoShift_t *pShift)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, algo_idx=%d", unit, algo_idx);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(algo_idx >= HAL_MAX_NUM_OF_TRUNK_ALGO(unit), RT_ERR_LA_ALGO_ID);
    RT_PARAM_CHK((NULL == pShift), RT_ERR_NULL_POINTER);

    TRUNK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_DPORT_SHIFT_BITSf,
                          &pShift->dport_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SPORT_SHIFT_BITSf,
                          &pShift->sport_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_DIP_SHIFT_BITSf,
                          &pShift->dip_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SIP_SHIFT_BITSf,
                          &pShift->sip_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_DMAC_SHIFT_BITSf,
                          &pShift->dmac_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SMAC_SHIFT_BITSf,
                          &pShift->smac_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SPA_SHIFT_BITSf,
                          &pShift->spa_shift)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_distributionAlgorithmShift_get */

/* Function Name:
 *      dal_maple_trunk_distributionAlgorithmShift_set
 * Description:
 *      Set the shift bits of distribution algorithm parameters from the specified device.
 * Input:
 *      unit         - unit id
 *      algo_idx     - algorithm index
 *      pShift       - shift bits of the distribution algorithm parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_LA_ALGO_ID    - invalid trunk algorithm id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_LA_ALGO_SHIFT - invalid trunk algorithm shift
 * Note:
 *      None
 */
int32
dal_maple_trunk_distributionAlgorithmShift_set(uint32 unit, uint32 algo_idx, rtk_trunk_distAlgoShift_t *pShift)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(algo_idx >= HAL_MAX_NUM_OF_TRUNK_ALGO(unit), RT_ERR_LA_ALGO_ID);
    RT_PARAM_CHK((NULL == pShift), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pShift->dport_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);
    RT_PARAM_CHK(pShift->sport_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);
    RT_PARAM_CHK(pShift->dip_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);
    RT_PARAM_CHK(pShift->sip_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);
    RT_PARAM_CHK(pShift->dmac_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);
    RT_PARAM_CHK(pShift->smac_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);
    RT_PARAM_CHK(pShift->spa_shift > HAL_TRUNK_ALGO_SHIFT_MAX(unit), RT_ERR_LA_ALGO_SHIFT);

    TRUNK_SEM_LOCK(unit);
    /* set to CHIP*/
    val = pShift->dport_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_DPORT_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    val = pShift->sport_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SPORT_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    val = pShift->dip_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_DIP_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    val = pShift->sip_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SIP_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    val = pShift->dmac_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_DMAC_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    val = pShift->smac_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SMAC_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    val = pShift->spa_shift;
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MAPLE_SPA_SHIFT_BITSf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_distributionAlgorithmShift_set */

/* Function Name:
 *      dal_maple_trunk_trafficSeparate_get
 * Description:
 *      Get the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      unit          - unit id
 *      trk_gid       - trunk group id
 * Output:
 *      pSeparateType - pointer buffer of bitmask of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trunk_trafficSeparate_get(uint32 unit, uint32 trk_gid, rtk_trunk_separateType_t *pSeparateType)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, trk_gid=%d", unit, trk_gid);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pSeparateType), RT_ERR_NULL_POINTER);

    TRUNK_SEM_LOCK(unit);
    /* get entry from CHIP */
    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_SEP_TRAFFIC_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          trk_gid,
                          MAPLE_SEP_TRAFFICf,
                          &value)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    switch (value)
    {   /*Convert value from register*/
        case 0:
            *pSeparateType = SEPARATE_NONE;
            break;
        case 1:
            *pSeparateType = SEPARATE_KNOWN_MULTI;
            break;
        case 2:
            *pSeparateType = SEPARATE_FLOOD;
            break;
        case 3:
            *pSeparateType = SEPARATE_KNOWN_MULTI_AND_FLOOD;
            break;
        default:
           TRUNK_SEM_UNLOCK(unit);
           return RT_ERR_FAILED;
    }

    TRUNK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_trafficSeparate_get */

/* Function Name:
 *      dal_maple_trunk_trafficSeparate_set
 * Description:
 *      Set the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      unit     - unit id
 *      trk_gid  - trunk group id
 *      separate - traffic separation setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 *      RT_ERR_INPUT       - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_trunk_trafficSeparate_set(uint32 unit, uint32 trk_gid, rtk_trunk_separateType_t separateType)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK(separateType >= SEPARATE_END, RT_ERR_INPUT);

    TRUNK_SEM_LOCK(unit);

     /*Convert value from register*/
    switch (separateType)
    {
        case SEPARATE_NONE:
            val = 0;
            break;
        case SEPARATE_KNOWN_MULTI:
            val = 1;
            break;
        case SEPARATE_FLOOD:
            val = 2;
            break;
        case SEPARATE_KNOWN_MULTI_AND_FLOOD:
            val = 3;
            break;
        default:
           TRUNK_SEM_UNLOCK(unit);
           return RT_ERR_INPUT;
    }

    /* set to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_SEP_TRAFFIC_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          trk_gid,
                          MAPLE_SEP_TRAFFICf,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_trafficSeparate_set */

/* Function Name:
 *      dal_maple_trunk_port_get
 * Description:
 *      Get the members of the trunk id from the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid                - trunk group id
 * Output:
 *      pTrunk_member_portmask - pointer buffer of trunk member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trunk_port_get(uint32 unit, uint32 trk_gid, rtk_portmask_t *pTrunk_member_portmask)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, trk_gid=%d", unit, trk_gid);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pTrunk_member_portmask), RT_ERR_NULL_POINTER);

    osal_memset(pTrunk_member_portmask, 0, sizeof(rtk_portmask_t));

    TRUNK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MAPLE_TRK_MBR_CTRr,
                          REG_ARRAY_INDEX_NONE,
                          trk_gid,
                          MAPLE_TRK_MBR_0f,
                          &pTrunk_member_portmask->bits[0])) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "pTrunk_member_portmask=0x%x", pTrunk_member_portmask->bits[0]);

    return RT_ERR_OK;
} /* end of dal_maple_trunk_port_get */

/* Function Name:
 *      dal_maple_trunk_port_set
 * Description:
 *      Set the members of the trunk id to the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid                - trunk group id
 *      pTrunk_member_portmask - trunk member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_LA_TRUNK_ID       - invalid trunk ID
 *      RT_ERR_NULL_POINTER      - null pointer
 *      RT_ERR_LA_PORTMASK       - error port mask
 *      RT_ERR_LA_PORTNUM_NORMAL - it can only aggregate at most eight ports when 802.1ad normal mode
 *      RT_ERR_LA_MEMBER_OVERLAP - the specified port mask is overlapped with other group
 * Note:
 *      1) support 8 trunk members in each group.
 */
int32
dal_maple_trunk_port_set(uint32 unit, uint32 trk_gid, rtk_portmask_t *pTrunk_member_portmask)
{
    uint32  grp_num, val;
    uint32  num_of_port;
    int32   ret;
    int32   port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "unit=%d, trk_gid=%d", unit, trk_gid);

    /* check Init status */
    RT_INIT_CHK(trunk_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pTrunk_member_portmask), RT_ERR_NULL_POINTER);

    if(-1 != RTK_PORTMASK_GET_FIRST_PORT(*pTrunk_member_portmask) && -1 != RTK_PORTMASK_GET_LAST_PORT(*pTrunk_member_portmask))
    {
            for (port = RTK_PORTMASK_GET_FIRST_PORT(*pTrunk_member_portmask); port <= RTK_PORTMASK_GET_LAST_PORT(*pTrunk_member_portmask); port++)
            {
                RT_PARAM_CHK((RTK_PORTMASK_IS_PORT_SET(*pTrunk_member_portmask, port) && !HWP_PORT_EXIST(unit, port)), RT_ERR_LA_PORTMASK);
            }
    }

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "pTrunk_member_portmask=0x%x", pTrunk_member_portmask->bits[0]);

    /* check number of trunk member */
    RT_PARAM_CHK(((num_of_port = RTK_PORTMASK_GET_PORT_COUNT(*pTrunk_member_portmask)) > HAL_MAX_NUM_OF_TRUNKMEMBER(unit)), RT_ERR_LA_PORTNUM_NORMAL);

    /* check whether new member set is overlap with other trunk */
    for (grp_num = 0; grp_num < HAL_MAX_NUM_OF_TRUNK(unit); grp_num++)
    {
        if (grp_num != trk_gid)
        {
            RT_PARAM_CHK((pTrunk_member_portmask->bits[0] & pTrunkMemberSet[unit][grp_num].bits[0])
                        , RT_ERR_LA_MEMBER_OVERLAP);
        }
    }

    /*FIXED ME*/
    /*Check full duplex*/
    /*Check whether CPU PORT*/

    TRUNK_SEM_LOCK(unit);
    /* get entry from CHIP */
    val = pTrunk_member_portmask->bits[0];
    if ((ret = reg_array_field_write(unit,
                          MAPLE_TRK_MBR_CTRr,
                          REG_ARRAY_INDEX_NONE,
                          trk_gid,
                          MAPLE_TRK_MBR_0f,
                          &val)) != RT_ERR_OK)
    {
        TRUNK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    TRUNK_SEM_UNLOCK(unit);

    RTK_PORTMASK_ASSIGN(pTrunkMemberSet[unit][trk_gid], *pTrunk_member_portmask);
    return RT_ERR_OK;
} /* end of dal_maple_trunk_port_set */

/* Function Name:
 *      _dal_maple_trunk_init_config
 * Description:
 *      Initialize default configuration for trunk module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize trunk module before calling this API
 */
static int32
_dal_maple_trunk_init_config(uint32 unit)
{
#if 0

    int32   ret;
    uint32  tgid, algo_idx;
    rtk_portmask_t portmask;

    osal_memset(&portmask, 0 , sizeof(portmask));
    portmask.bits[0] = RTK_DEFAULT_TRUNK_MEMBER_PORTMASK;

    for (tgid = 0; tgid < HAL_MAX_NUM_OF_TRUNK(unit); tgid++)
    {
        if ((ret = dal_maple_trunk_port_set(unit, tgid, &portmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
            return ret;
        }

        if ((ret = dal_maple_trunk_distributionAlgorithmBind_set(unit, tgid, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
            return ret;
        }
    }

    for (algo_idx = 0; algo_idx < 2; algo_idx++)
    {
        if ((ret = dal_maple_trunk_distributionAlgorithmParam_set(unit, algo_idx, RTK_DEFAULT_TRUNK_DISTRIBUTION_ALGORITHM)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
            return ret;
        }
    }
#endif
    return RT_ERR_OK;
} /* end of _dal_maple_trunk_init_config */
