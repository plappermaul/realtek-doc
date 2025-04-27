
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
#include <osal/time.h>
#include <osal/lib.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <dal/cypress/dal_cypress_common.h>
#include <dal/cypress/dal_cypress_oam.h>
#include <dal/cypress/dal_cypress_trap.h>

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
 *      dal_cypress_common_init
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
dal_cypress_common_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(common_init[unit]);
    common_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    common_sem[unit] = osal_sem_mutex_create();
    if (0 == common_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_COMMON), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    common_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_cypress_common_init */

/* Function Name:
 *      dal_cypress_common_portEgrQueueEmpty_start
 * Description:
 *      Empty egress queues of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 * Note:
 *      After using the API, dal_cypress_common_portEgrQueueEmpty_end must
 *      be called to free semaphore.
 */
int32
dal_cypress_common_portEgrQueueEmpty_start(
    uint32          unit,
    rtk_port_t      port,
    uint32          *pState,
    uint32          *pRate,
    rtk_trap_oam_action_t *pTrap_action,
    rtk_action_t    *pAction)
{
    sched_entry_t schedEntry;
    out_q_entry_t outQ;
    uint32 unlimit_rate = 0xFFFFF, value, cntr;
    int32 ret;
    osal_usecs_t usec_start, usec_current;

    COMMON_SEM_LOCK(unit);

    /* Get current OAM state and configuration */
    if ((ret = reg_field_read(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf, pState)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = dal_cypress_trap_portOamLoopbackParAction_get(unit, port, pTrap_action)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = dal_cypress_oam_portLoopbackMuxAction_get(unit, port, pAction)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }

    /* Disable OAM to blocking traffic */
    value = ENABLED;
    if ((ret = reg_field_write(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf, &value)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf, &value)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }

    if ((ret = dal_cypress_trap_portOamLoopbackParAction_set(unit, port, TRAP_OAM_ACTION_FORWARD)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = dal_cypress_oam_portLoopbackMuxAction_set(unit, port, ACTION_DROP)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }

    /* Get current rate value and set unlimited rate to rsest token of egress leaky bucket */
    osal_memset(&schedEntry, 0, sizeof(sched_entry_t));
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &schedEntry)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = table_field_get(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf,
                    pRate, (uint32 *) &schedEntry)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf,
                    &unlimit_rate, (uint32 *) &schedEntry)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &schedEntry)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }

    /* Check egress used-page-count of the port */
    cntr = 0;
    osal_time_usecs_get(&usec_start);
    do {
        osal_memset(&outQ, 0, sizeof(outQ));
        if ((ret = table_read(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
        {
            COMMON_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
            return ret;
        }
        if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_PE_USED_PAGE_CNTtf,
                        &cntr, (uint32 *) &outQ)) != RT_ERR_OK)
        {
            COMMON_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
            return ret;
        }
        osal_time_usecs_get(&usec_current);
    } while ((cntr != 0) && ((usec_current - usec_start) < 350000));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_common_portEgrQueueEmpty_end
 * Description:
 *      Restore egress queues of specified port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 * Note:
 *      Before using the API, dal_cypress_common_portEgrQueueEmpty_start must
 *      be called to lock semaphore.
 */
int32
dal_cypress_common_portEgrQueueEmpty_end(
    uint32          unit,
    rtk_port_t      port,
    rtk_q_empty_t   type,
    uint32          state,
    uint32          rate,
    rtk_trap_oam_action_t trap_action,
    rtk_action_t    action)
{
    int32 ret;
    sched_entry_t schedEntry;

    /* Restore OAM configuration to the port */
    if ((ret = reg_field_write(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf, &state)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = dal_cypress_trap_portOamLoopbackParAction_set(unit, port, trap_action)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }
    if ((ret = dal_cypress_oam_portLoopbackMuxAction_set(unit, port, action)) != RT_ERR_OK)
    {
        COMMON_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_COMMON), "");
        return ret;
    }

    /* Restore egress rate-limit configuration to the port */
    if (Q_EMPTY_SCHED_ALGO == type)
    {
        if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &schedEntry)) != RT_ERR_OK)
        {
            COMMON_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        if ((ret = table_field_set(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf,
                        &rate, (uint32 *) &schedEntry)) != RT_ERR_OK)
        {
            COMMON_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &schedEntry)) != RT_ERR_OK)
        {
            COMMON_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    COMMON_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

