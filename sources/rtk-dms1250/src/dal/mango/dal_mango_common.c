
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
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature :  Parameter settings for the system-wise view
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <dal/mango/dal_mango_common.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               common_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         common_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* trap semaphore handling */
#define COMMON_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(common_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_COMMON), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define COMMON_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(common_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_COMMON), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_common_init
 * Description:
 *      Initialize common module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_mango_common_init(uint32 unit)
{
#if 0
    uint32  val;
    int32   ret;
#endif

    RT_INIT_REENTRY_CHK(common_init[unit]);
    common_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    common_sem[unit] = osal_sem_mutex_create();
    if (0 == common_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_COMMON), "semaphore create failed");
        return RT_ERR_FAILED;
    }

#if 0
    val = 1;
    //RT_ERR_CHK(reg_field_write(unit, MANGO_MEM_IGR_INITr, MANGO_LINK_LIST_INITf, &val), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_MEM_ENCAP_INITr, MANGO_MEM_INITf, &val), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_MEM_EGR_INITr, MANGO_MEM_INITf, &val), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_MEM_MIB_INITr, MANGO_MEM_RSTf, &val), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_MEM_ACL_INITr, MANGO_MEM_INITf, &val), ret);

    val = 0xFFFFFFFF;
    RT_ERR_CHK(reg_write(unit, MANGO_MEM_ALE_INIT_0r, &val), ret);
    val = 0x7F;
    RT_ERR_CHK(reg_write(unit, MANGO_MEM_ALE_INIT_1r, &val), ret);
    val = 0x7FF;
    RT_ERR_CHK(reg_write(unit, MANGO_MEM_ALE_INIT_2r, &val), ret);
#endif

    /* set init flag to complete init */
    common_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_cypress_common_init */

