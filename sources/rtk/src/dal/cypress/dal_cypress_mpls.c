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
 * Purpose : Definition those public MPLS routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *              1) MPLS
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_mpls.h>
#include <dal/dal_mgmt.h>
#include <rtk/mpls.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32       mpls_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t mpls_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Declaration
 */
#define MPLS_SEM_LOCK(unit)                                                         \
do {                                                                                \
    if (osal_sem_mutex_take(mpls_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)    \
    {                                                                               \
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_MPLS),"semaphore lock failed"); \
        return RT_ERR_SEM_LOCK_FAILED;                                              \
    }                                                                               \
} while(0)

#define MPLS_SEM_UNLOCK(unit)                                                           \
do {                                                                                    \
    if (osal_sem_mutex_give(mpls_sem[unit]) != RT_ERR_OK)                               \
    {                                                                                   \
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_MPLS),"semaphore unlock failed"); \
        return RT_ERR_SEM_UNLOCK_FAILED;                                                \
    }                                                                                   \
} while(0)

/*
 * Function Declaration
 */

 /* Function Name:
 *      dal_cypress_mplsMapper_init
 * Description:
 *      Hook mpls module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook mpls module before calling any mpls APIs.
 */
int32
dal_cypress_mplsMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mpls_init              = dal_cypress_mpls_init;
    pMapper->mpls_ttlInherit_get    = dal_cypress_mpls_ttlInherit_get;
    pMapper->mpls_ttlInherit_set    = dal_cypress_mpls_ttlInherit_set;
    pMapper->mpls_encap_get         = dal_cypress_mpls_encap_get;
    pMapper->mpls_encap_set         = dal_cypress_mpls_encap_set;
    pMapper->mpls_enable_get        = dal_cypress_mpls_enable_get;
    pMapper->mpls_enable_set        = dal_cypress_mpls_enable_set;

    return RT_ERR_OK;
}
int32 dal_cypress_mpls_ttlInherit_set(uint32 unit, rtk_mpls_ttlInherit_t inherit);


/* Module Name : MPLS */

/* Function Name:
 *      dal_cypress_mpls_init
 * Description:
 *      Initialize mpls module of the specified device.
 * Input:
 *      unit    - Device number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_cypress_mpls_init(uint32 unit)
{
    if (INIT_COMPLETED == mpls_init[unit])
        return RT_ERR_OK;

    RT_INIT_REENTRY_CHK(mpls_init[unit]);
    mpls_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    mpls_sem[unit] = osal_sem_mutex_create();
    if (0 == mpls_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_MPLS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    mpls_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_init */

/* Function Name:
 *      dal_cypress_mpls_ttlInherit_get
 * Description:
 *      Get MPLS TTL inherit properties
 * Input:
 *      unit    - Device number
 * Output:
 *      inherit - MPLS TTL inherit information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_mpls_ttlInherit_get(uint32 unit, rtk_mpls_ttlInherit_t *inherit)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d", unit);

    RT_PARAM_CHK((NULL == inherit), RT_ERR_NULL_POINTER);

    MPLS_SEM_LOCK(unit);
    ret = reg_field_read(unit, CYPRESS_MPLS_CTRLr, CYPRESS_TTL_PROCf, (uint32 *)&val);
    if (ret != RT_ERR_OK)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }
    MPLS_SEM_UNLOCK(unit);

    if (0 == val)
        *inherit = RTK_MPLS_TTL_INHERIT_UNIFORM;
    else if (1 == val)
        *inherit = RTK_MPLS_TTL_INHERIT_PIPE;

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_ttlInherit_get */

/* Function Name:
 *      dal_cypress_mpls_ttlInherit_set
 * Description:
 *      Set MPLS TTL inherit properties
 * Input:
 *      unit    - Device number
 *      inherit - MPLS TTL inherit information
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
dal_cypress_mpls_ttlInherit_set(uint32 unit, rtk_mpls_ttlInherit_t inherit)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d", unit);

    RT_PARAM_CHK((RTK_MPLS_TTL_INHERIT_MAX <= inherit), RT_ERR_INPUT);

    if (RTK_MPLS_TTL_INHERIT_UNIFORM == inherit)
        val = 0;
    else if (RTK_MPLS_TTL_INHERIT_PIPE == inherit)
        val = 1;

    MPLS_SEM_LOCK(unit);
    ret = reg_field_write(unit, CYPRESS_MPLS_CTRLr, CYPRESS_TTL_PROCf, (uint32 *)&val);
    if (ret != RT_ERR_OK)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }
    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_ttlInherit_set */

/* Function Name:
 *      dal_cypress_mpls_encap_get
 * Description:
 *      Get MPLS encapsulation properties
 * Input:
 *      unit    - Device number
 *      lib_idx - the index of MPLS table
 * Output:
 *      info    - MPLS encapsulation information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_cypress_mpls_encap_get(uint32 unit, uint32 lib_idx,
    rtk_mpls_encap_t *info)
{
    uint32              phyical_idx, label_idx = 0;
    int32               ret;
    mpls_lib_entry_t    lib_entry;

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d %d", unit, lib_idx);

    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_MPLS_LABEL_OPER_MAX <= info->oper), RT_ERR_INPUT);

    phyical_idx = lib_idx;

    if (RTK_MPLS_LABEL_OPER_SINGLE == info->oper)
    {
        phyical_idx = (lib_idx >> 1);
        label_idx = (lib_idx & 0x1);
    }

    RT_PARAM_CHK((HAL_MAX_NUM_OF_MPLS_LIB(unit) <= phyical_idx), RT_ERR_INPUT);

    MPLS_SEM_LOCK(unit);
    ret = table_read(unit, CYPRESS_MPLS_LIBt, phyical_idx,
            (uint32 *) &lib_entry);
    if (ret != RT_ERR_OK)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }
    MPLS_SEM_UNLOCK(unit);

    if (0 == label_idx || RTK_MPLS_LABEL_OPER_DOUBEL == info->oper)
    {
        /* inner label */
        RT_ERR_CHK(table_field_get(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_I_LABELtf, &info->label0,  \
                (uint32 *) &lib_entry), ret);

        /* inner exp */
        RT_ERR_CHK(table_field_get(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_I_EXPtf, (uint32 *)&info->exp0,      \
                (uint32 *) &lib_entry), ret);

        /* inner TTL */
        RT_ERR_CHK(table_field_get(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_I_TTLtf, (uint32 *)&info->ttl0,      \
                (uint32 *) &lib_entry), ret);
    }   /* if (0 == label_idx) */

    if (1 == label_idx || RTK_MPLS_LABEL_OPER_DOUBEL == info->oper)
    {
        /* outer label */
        RT_ERR_CHK(table_field_get(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_O_LABELtf, &info->label1,  \
                (uint32 *) &lib_entry), ret);

        /* outer exp */
        RT_ERR_CHK(table_field_get(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_O_EXPtf, (uint32 *)&info->exp1,      \
                (uint32 *) &lib_entry), ret);

        /* outer TTL */
        RT_ERR_CHK(table_field_get(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_O_TTLtf, (uint32 *)&info->ttl1,      \
                (uint32 *) &lib_entry), ret);

        if (RTK_MPLS_LABEL_OPER_SINGLE == info->oper)
        {
            info->label0 = info->label1;
            info->exp0 = info->exp1;
            info->ttl0 = info->ttl1;
        }
    }   /* if (1 == label_idx) */

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_encap_get */

/* Function Name:
 *      dal_cypress_mpls_encap_set
 * Description:
 *      Set MPLS encapsulation properties
 * Input:
 *      unit    - Device number
 *      info    - MPLS encapsulation information
 *      lib_idx - the index of MPLS table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_mpls_encap_set(uint32 unit, uint32 lib_idx,
    rtk_mpls_encap_t *info)
{
    uint32              phyical_idx, label_idx = 0;
    int32               ret;
    mpls_lib_entry_t    lib_entry;

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d %d", unit, lib_idx);

    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_MPLS_LABEL_OPER_MAX <= info->oper), RT_ERR_INPUT);

    phyical_idx = lib_idx;

    if (RTK_MPLS_LABEL_OPER_SINGLE == info->oper)
    {
        phyical_idx = (lib_idx >> 1);
        label_idx = (lib_idx & 0x1);
    }

    RT_PARAM_CHK((HAL_MAX_NUM_OF_MPLS_LIB(unit) <= phyical_idx), RT_ERR_INPUT);

    osal_memset(&lib_entry, 0, sizeof(lib_entry));

    /* When config inner & outer, it needn't read old data */
    if (RTK_MPLS_LABEL_OPER_SINGLE == info->oper)
    {
        MPLS_SEM_LOCK(unit);
        ret = table_read(unit, CYPRESS_MPLS_LIBt, phyical_idx,
                (uint32 *) &lib_entry);
        if (ret != RT_ERR_OK) {
            MPLS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
            return ret;
        }
        MPLS_SEM_UNLOCK(unit);

        info->label1 = info->label0;
        info->exp1 = info->exp0;
        info->ttl1 = info->ttl0;
    }

    if (0 == label_idx || RTK_MPLS_LABEL_OPER_DOUBEL == info->oper)
    {
        /* inner label */
        RT_ERR_CHK(table_field_set(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_I_LABELtf, &info->label0,  \
                (uint32 *) &lib_entry), ret);

        /* inner exp */
        RT_PARAM_CHK((RTK_MPLS_EXP_MAX < info->exp0), RT_ERR_INPUT);
        RT_ERR_CHK(table_field_set(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_I_EXPtf, (uint32 *)&info->exp0,      \
                (uint32 *) &lib_entry), ret);

        /* inner TTL */
        RT_PARAM_CHK((RTK_MPLS_TTL_MAX < info->ttl0), RT_ERR_INPUT);
        RT_ERR_CHK(table_field_set(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_I_TTLtf, (uint32 *)&info->ttl0,      \
                (uint32 *) &lib_entry), ret);
    }   /* if (0 == label_idx) */

    if (1 == label_idx || RTK_MPLS_LABEL_OPER_DOUBEL == info->oper)
    {
        /* outer label */
        RT_ERR_CHK(table_field_set(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_O_LABELtf, &info->label1,  \
                (uint32 *) &lib_entry), ret);

        /* outer exp */
        RT_PARAM_CHK((RTK_MPLS_EXP_MAX < info->exp1), RT_ERR_INPUT);
        RT_ERR_CHK(table_field_set(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_O_EXPtf, (uint32 *)&info->exp1,      \
                (uint32 *) &lib_entry), ret);

        /* outer TTL */
        RT_PARAM_CHK((RTK_MPLS_TTL_MAX < info->ttl1), RT_ERR_INPUT);
        RT_ERR_CHK(table_field_set(unit, CYPRESS_MPLS_LIBt, \
                CYPRESS_MPLS_LIB_O_TTLtf, (uint32 *)&info->ttl1,      \
                (uint32 *) &lib_entry), ret);
    }   /* if (1 == label_idx) */

    MPLS_SEM_LOCK(unit);
    ret = table_write(unit, CYPRESS_MPLS_LIBt, phyical_idx,
            (uint32 *) &lib_entry);
    if (ret != RT_ERR_OK)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }
    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_encap_set */

/* Function Name:
 *      dal_cypress_mpls_enable_get
 * Description:
 *      Get MPLS state
 * Input:
 *      unit    - Device number
 * Output:
 *      pEnable - MPLS state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_mpls_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d", unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    MPLS_SEM_LOCK(unit);

    ret = reg_field_read(unit, CYPRESS_MPLS_CTRLr, CYPRESS_MPLS_ENf, (uint32 *)&val);
    if (ret != RT_ERR_OK)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }

    MPLS_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else if (1 == val)
        *pEnable = ENABLED;

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_enable_get */

/* Function Name:
 *      dal_cypress_mpls_enable_set
 * Description:
 *      Set MPLS state
 * Input:
 *      unit    - Device number
 *      enable  - state of MPLS
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
dal_cypress_mpls_enable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d, enable=%d", unit, enable);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else
        val = 1;

    MPLS_SEM_LOCK(unit);

    ret = reg_field_write(unit, CYPRESS_MPLS_CTRLr, CYPRESS_MPLS_ENf, (uint32 *)&val);
    if (ret != RT_ERR_OK)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_cypress_mpls_enable_set */

