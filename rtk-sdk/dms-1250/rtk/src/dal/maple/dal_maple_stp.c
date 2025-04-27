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
 * Purpose : Definition those public STP APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spanning tree (1D, 1w and 1s)
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
#include <dal/maple/dal_maple_stp.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/stp.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               stp_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         stp_sem[RTK_MAX_NUM_OF_UNIT];

static rtk_bitmap_t         *pMsti_valid[RTK_MAX_NUM_OF_UNIT];
const static uint16 mstpState_fieldidx[] = {MAPLE_MSTI_STATE_PORT0tf, MAPLE_MSTI_STATE_PORT1tf,
                                            MAPLE_MSTI_STATE_PORT2tf, MAPLE_MSTI_STATE_PORT3tf,
                                            MAPLE_MSTI_STATE_PORT4tf, MAPLE_MSTI_STATE_PORT5tf,
                                            MAPLE_MSTI_STATE_PORT6tf, MAPLE_MSTI_STATE_PORT7tf,
                                            MAPLE_MSTI_STATE_PORT8tf, MAPLE_MSTI_STATE_PORT9tf,
                                            MAPLE_MSTI_STATE_PORT10tf, MAPLE_MSTI_STATE_PORT11tf,
                                            MAPLE_MSTI_STATE_PORT12tf, MAPLE_MSTI_STATE_PORT13tf,
                                            MAPLE_MSTI_STATE_PORT14tf, MAPLE_MSTI_STATE_PORT15tf,
                                            MAPLE_MSTI_STATE_PORT16tf, MAPLE_MSTI_STATE_PORT17tf,
                                            MAPLE_MSTI_STATE_PORT18tf, MAPLE_MSTI_STATE_PORT19tf,
                                            MAPLE_MSTI_STATE_PORT20tf, MAPLE_MSTI_STATE_PORT21tf,
                                            MAPLE_MSTI_STATE_PORT22tf, MAPLE_MSTI_STATE_PORT23tf,
                                            MAPLE_MSTI_STATE_PORT24tf, MAPLE_MSTI_STATE_PORT25tf,
                                            MAPLE_MSTI_STATE_PORT26tf, MAPLE_MSTI_STATE_PORT27tf,
                                            MAPLE_MSTI_STATE_PORT28tf};

/*
 * Macro Definition
 */
/* stp semaphore handling */
#define STP_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(stp_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_STP),"semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define STP_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(stp_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_STP),"semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define MSTI_VALID_IS_SET(unit, msti)   BITMAP_IS_SET(pMsti_valid[unit], msti)
#define MSTI_VALID_IS_CLEAR(unit, msti) BITMAP_IS_CLEAR(pMsti_valid[unit], msti)
#define MSTI_VALID_SET(unit, msti)      BITMAP_SET(pMsti_valid[unit], msti)
#define MSTI_VALID_CLEAR(unit, msti)    BITMAP_CLEAR(pMsti_valid[unit], msti)

/*
 * Function Declaration
 */
static int32 _dal_maple_stp_init_config(uint32 unit);

/* Module Name : STP */

/* Function Name:
 *      dal_maple_stpMapper_init
 * Description:
 *      Hook stp module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook stp module before calling any stp APIs.
 */
int32
dal_maple_stpMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stp_init = dal_maple_stp_init;
    pMapper->stp_mstpInstance_create = dal_maple_stp_mstpInstance_create;
    pMapper->stp_mstpInstance_destroy = dal_maple_stp_mstpInstance_destroy;
    pMapper->stp_isMstpInstanceExist_get = dal_maple_stp_isMstpInstanceExist_get;
    pMapper->stp_mstpState_get = dal_maple_stp_mstpState_get;
    pMapper->stp_mstpState_set = dal_maple_stp_mstpState_set;
    pMapper->stp_mstpInstanceMode_get = dal_maple_stp_mstpInstanceMode_get;
    pMapper->stp_mstpInstanceMode_set = dal_maple_stp_mstpInstanceMode_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_stp_init
 * Description:
 *      Initialize stp module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stp module before calling any stp APIs.
 */
int32
dal_maple_stp_init(uint32 unit)
{
    int32   ret;

    RT_INIT_REENTRY_CHK(stp_init[unit]);
    stp_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    stp_sem[unit] = osal_sem_mutex_create();
    if (0 == stp_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_STP), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* Allocate and initilize memory for STP valid database */
    pMsti_valid[unit] = (rtk_bitmap_t *)osal_alloc(BITMAP_ARRAY_SIZE(HAL_MAX_NUM_OF_MSTI(unit)));
    if (NULL == pMsti_valid[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_STP|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pMsti_valid[unit], 0, BITMAP_ARRAY_SIZE(HAL_MAX_NUM_OF_MSTI(unit)));

    /* set init flag to complete init */
    stp_init[unit] = INIT_COMPLETED;

    if ((ret = _dal_maple_stp_init_config(unit)) != RT_ERR_OK)
    {
        stp_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pMsti_valid[unit]);
        pMsti_valid[unit] = 0;
        RT_ERR(ret, (MOD_STP|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_stp_init */

/* Function Name:
 *      dal_maple_stp_mstpInstance_create
 * Description:
 *      Create one specified mstp instance of the specified device.
 * Input:
 *      unit - unit id
 *      msti - mstp instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_MSTI     - invalid msti
 *      RT_ERR_MSTI_EXIST - MSTI is already exist.
 * Note:
 *      The msti valid range is 0 .. HAL_MAX_NUM_OF_MSTI(unit)-1
 */
int32
dal_maple_stp_mstpInstance_create(uint32 unit, uint32 msti)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "unit=%d, msti=%d", unit, msti);

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(msti >= HAL_MAX_NUM_OF_MSTI(unit), RT_ERR_MSTI);
    RT_PARAM_CHK(MSTI_VALID_IS_SET(unit, msti), RT_ERR_MSTI_EXIST);

    STP_SEM_LOCK(unit);
    /* Set valid bit of MSTI */
    MSTI_VALID_SET(unit, msti);

    STP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_stp_mstpInstance_create */

/* Function Name:
 *      dal_maple_stp_mstpInstance_destroy
 * Description:
 *      Destroy one specified mstp instance from the specified device.
 * Input:
 *      unit - unit id
 *      msti - mstp instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_MSTI_NOT_EXIST - msti is not exist
 * Note:
 *      The msti valid range is 0 .. RTK_STP_INSTANCE_ID_MAX-1
 */
int32
dal_maple_stp_mstpInstance_destroy(uint32 unit, uint32 msti)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "unit=%d, msti=%d", unit, msti);

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(msti >= HAL_MAX_NUM_OF_MSTI(unit), RT_ERR_MSTI);
    RT_PARAM_CHK(MSTI_VALID_IS_CLEAR(unit, msti), RT_ERR_MSTI_NOT_EXIST);

    STP_SEM_LOCK(unit);
    /* clear valid bit of MSTI */
    MSTI_VALID_CLEAR(unit, msti);

    STP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_stp_mstpInstance_destroy */

/* Function Name:
 *      dal_maple_stp_isMstpInstanceExist_get
 * Description:
 *      Check one specified mstp instance is existing or not in the specified device.
 * Input:
 *      unit        - unit id
 *      msti        - mstp instance
 * Output:
 *      pMsti_exist - mstp instance exist or not?
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The pMsti_exist value as following:
 *      0: this mstp instance not exist
 *      1: this mstp instance exist
 */
int32
dal_maple_stp_isMstpInstanceExist_get(uint32 unit, uint32 msti, uint32 *pMsti_exist)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "unit=%d, msti=%d", unit, msti);

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((msti >= HAL_MAX_NUM_OF_MSTI(unit)), RT_ERR_MSTI);
    RT_PARAM_CHK((NULL == pMsti_exist), RT_ERR_NULL_POINTER);

    STP_SEM_LOCK(unit);
    /* clear valid bit of MSTI */
    *pMsti_exist = (MSTI_VALID_IS_SET(unit, msti))?1:0;

    STP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "pMsti_exist=%x", *pMsti_exist);

    return RT_ERR_OK;
} /* end of dal_maple_stp_isMstpInstanceExist_get */

/* Function Name:
 *      dal_maple_stp_mstpState_get
 * Description:
 *      Get port spanning tree state of the msti from the specified device.
 * Input:
 *      unit       - unit id
 *      msti       - multiple spanning tree instance
 *      port       - port id
 * Output:
 *      pStp_state - pointer buffer of spanning tree state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_MSTI_NOT_EXIST - MSTI is not exist
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      1) For single spanning tree mode, input CIST0 (msti=0).
 *      2) Spanning tree state as following
 *         - STP_STATE_DISABLED
 *         - STP_STATE_BLOCKING
 *         - STP_STATE_LEARNING
 *         - STP_STATE_FORWARDING
 */
int32
dal_maple_stp_mstpState_get(uint32 unit, uint32 msti, rtk_port_t port, rtk_stp_state_t *pStp_state)
{
    int32   ret;
    spt_entry_t pStpEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "unit=%d, msti=%d, port=%d",
           unit, msti, port);

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((msti >= HAL_MAX_NUM_OF_MSTI(unit)), RT_ERR_MSTI);
    RT_PARAM_CHK(MSTI_VALID_IS_CLEAR(unit, msti), RT_ERR_MSTI_NOT_EXIST);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStp_state), RT_ERR_NULL_POINTER);

    STP_SEM_LOCK(unit);

    osal_memset(&pStpEntry, 0, sizeof(spt_entry_t));

    if ((ret = table_read(unit, MAPLE_MSTIt, msti, (uint32 *) &pStpEntry)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_MSTIt, (uint32)mstpState_fieldidx[port],
                    pStp_state, (uint32 *) &pStpEntry)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    STP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "pStp_state=%x", *pStp_state);

    return RT_ERR_OK;
} /* end of dal_maple_stp_mstpState_get */

/* Function Name:
 *      dal_maple_stp_mstpState_set
 * Description:
 *      Set port spanning tree state of the msti to the specified device.
 * Input:
 *      unit      - unit id
 *      msti      - multiple spanning tree instance
 *      port      - port id
 *      stp_state - spanning tree state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_MSTI_NOT_EXIST - MSTI is not exist
 *      RT_ERR_MSTP_STATE     - invalid spanning tree status
 * Note:
 *      1) For single spanning tree mode, input CIST0 (msti=0).
 *      2) Spanning tree state as following
 *         - STP_STATE_DISABLED
 *         - STP_STATE_BLOCKING
 *         - STP_STATE_LEARNING
 *         - STP_STATE_FORWARDING
 */
int32
dal_maple_stp_mstpState_set(uint32 unit, uint32 msti, rtk_port_t port, rtk_stp_state_t stp_state)
{
    int32   ret;
    uint32  val = stp_state;
    spt_entry_t pStpEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "unit=%d, msti=%d, port=%d, stp_state=%d",
           unit, msti, port, stp_state);

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(HWP_IS_CPU_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((msti >= HAL_MAX_NUM_OF_MSTI(unit)), RT_ERR_MSTI);
    RT_PARAM_CHK(MSTI_VALID_IS_CLEAR(unit, msti), RT_ERR_MSTI_NOT_EXIST);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((stp_state >= STP_STATE_END), RT_ERR_MSTP_STATE);

    STP_SEM_LOCK(unit);

    osal_memset(&pStpEntry, 0, sizeof(spt_entry_t));

    if ((ret = table_read(unit, MAPLE_MSTIt, msti, (uint32 *) &pStpEntry)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MAPLE_MSTIt, (uint32)mstpState_fieldidx[port],
                    &val, (uint32 *) &pStpEntry)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    if ((ret = table_write(unit, MAPLE_MSTIt, msti, (uint32 *) &pStpEntry)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    STP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_stp_mstpState_set */

/* Function Name:
 *      dal_maple_stp_mstpInstanceMode_get
 * Description:
 *      Get mstp instance source
 * Input:
 *      unit        - unit id
 * Output:
 *      pMsti_mode  - mstp instance mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      MSTI is either from VLAN table lookup or always be 0.
 */
int32
dal_maple_stp_mstpInstanceMode_get(uint32 unit, rtk_stp_mstiMode_t *pMsti_mode)
{
    int32   ret;
    uint32  val = 0;

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsti_mode), RT_ERR_NULL_POINTER);

    STP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MAPLE_VLAN_STP_CTRLr, MAPLE_MSTI_MODEf, &val)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }
    STP_SEM_UNLOCK(unit);

    if (val == 0)
        *pMsti_mode = STP_MSTI_MODE_NORMAL;
    else if (val == 1)
        *pMsti_mode = STP_MSTI_MODE_CIST;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_stp_mstpInstanceMode_set
 * Description:
 *      Set mstp instance source
 * Input:
 *      unit        - unit id
 *      msti_mode   - mstp instance mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      MSTI is either from VLAN table lookup or always be 0.
 */
int32
dal_maple_stp_mstpInstanceMode_set(uint32 unit, rtk_stp_mstiMode_t msti_mode)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STP), "unit=%d, msti_mode=%d", unit, msti_mode);

    /* check Init status */
    RT_INIT_CHK(stp_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((msti_mode != STP_MSTI_MODE_NORMAL) && (msti_mode != STP_MSTI_MODE_CIST)), RT_ERR_INPUT);

    if (msti_mode == STP_MSTI_MODE_NORMAL)
        val = 0;
    else if (msti_mode == STP_MSTI_MODE_CIST)
        val = 1;

    STP_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_STP_CTRLr, MAPLE_MSTI_MODEf, &val)) != RT_ERR_OK)
    {
        STP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }
    STP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_maple_stp_init_config
 * Description:
 *      Initialize default configuration for stp module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stp module before calling this API
 */
static int32
_dal_maple_stp_init_config(uint32 unit)
{
    int32   ret;
    uint32  port;

    if ((ret = dal_maple_stp_mstpInstance_create(unit, RTK_DEFAULT_MSTI)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STP|MOD_DAL), "");
        return ret;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_maple_stp_mstpState_set(unit, RTK_DEFAULT_MSTI, port, RTK_DEFAULT_STP_PORT_STATE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_STP|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_maple_stp_init_config */
