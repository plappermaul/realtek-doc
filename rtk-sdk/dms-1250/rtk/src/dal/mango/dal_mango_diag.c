/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 92924 $
 * $Date: 2019-03-09 11:02:00 +0800 (週六, 9 三月 2019) $
 *
 * Purpose : None
 *
 * Feature : None
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
#include <osal/thread.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>

#include <rtk/port.h>
#include <rtk/default.h>

#include <rtk/diag.h>
#include <dal/mango/dal_mango_diag.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               diag_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         diag_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* vlan semaphore handling */
#define DIAG_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(diag_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_DIAG), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define DIAG_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(diag_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_DIAG), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


/* Function Name:
 *      dal_longan_diagMapper_init
 * Description:
 *      Hook diag module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook diag module before calling any diag APIs.
 */
int32
dal_mango_diagMapper_init(dal_mapper_t *pMapper)
{
    pMapper->diag_init = dal_mango_diag_init;
    pMapper->diag_table_read = dal_mango_diag_table_read;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_diag_init
 * Description:
 *      Initialize diagnostic module of the specified device.
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
dal_mango_diag_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(diag_init[unit]);
    diag_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    diag_sem[unit] = osal_sem_mutex_create();
    if (0 == diag_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    diag_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}/* end of dal_cypress_port_init */


/* Function Name:
 *      dal_mango_diag_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * 	  pData - pointer buffer for read back table entry
 *      pRev_vaild - used to sure the revbits is vaild.
 * 	  pRevbits - pointer buffer for read reverse bits which are not contain in entry.
 * Output:
 * 	  pData - pointer buffer for read back table entry
 *      pRev_vaild - used to sure the revbits is vaild.
 * 	  pRevbits - pointer buffer for read reverse bits which are not contain in entry.
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 * Note:
 *      1. Basically, this is a transparent API for table read.
 *      2. For L2 hash table, this API will converse the hiding 12-bits,
 *          and provide to upper layer by pRevbits parameter.
 *      3. addr format :
 *          From RTK and realchip view : hash_key[13:2]location[1:0]
 */
int32
dal_mango_diag_table_read(uint32    unit, uint32 table, uint32 addr, uint32 *pData, uint32 *pRev_vaild, uint32 *pRevbits)
{
    int32 ret = RT_ERR_OK;

       /* check Init status */
    RT_INIT_CHK(diag_init[unit]);

    DIAG_SEM_LOCK(unit);

    *pRev_vaild = 0;
    ret = table_read(unit, table, addr, pData);

    DIAG_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_longan_diag_table_read */

