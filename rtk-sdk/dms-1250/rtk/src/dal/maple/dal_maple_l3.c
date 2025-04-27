/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public L3 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) L3 routing
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
#include <dal/maple/dal_maple_l2.h>
#include <dal/maple/dal_maple_l3.h>
#include <dal/dal_mgmt.h>
#include <rtk/l2.h>
#include <rtk/l3.h>

/*
 * Data Declaration
 */
static uint32               l3_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         l3_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* vlan semaphore handling */
#define L3_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(l3_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define L3_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l3_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_maple_l3Mapper_init
 * Description:
 *      Hook l3 module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook l3 module before calling any l3 APIs.
 */
int32
dal_maple_l3Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l3_init = dal_maple_l3_init;
    pMapper->l3_routeEntry_get = dal_maple_l3_routeEntry_get;
    pMapper->l3_routeEntry_set = dal_maple_l3_routeEntry_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_l3_init
 * Description:
 *      Initialize L3 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
dal_maple_l3_init(uint32 unit)
{
    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(l3_init[unit]);
    l3_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    l3_sem[unit] = osal_sem_mutex_create();
    if (0 == l3_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_L3|MOD_DAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    l3_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_maple_l3_init */

/* Module Name : Layer3 routing */

/* Function Name:
 *      dal_maple_l3_routeEntry_get
 * Description:
 *      Get L3 rounting entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 * Output:
 *      pEntry - L3 route entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The route entry is indexed by L2 NextHop entry.
 *      Valid index range:
 *      - 0~2047 in 8390
 *      - 0~511 in 8380
 */
int32
dal_maple_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry)
{
    int32   ret;
    uint32  mac_uint32[2];
    routing_entry_t route_entry;

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "unit=%d, index=%d", unit, index);
    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    L3_SEM_LOCK(unit);

    if ((ret = table_read(unit, MAPLE_ROUTINGt, index, (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MAPLE_ROUTINGt, MAPLE_ROUTING_GATEWAY_MACtf,
                    &mac_uint32[0], (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    pEntry->hostMac.octet[0] = (uint8)(mac_uint32[1] >> 8);
    pEntry->hostMac.octet[1] = (uint8)(mac_uint32[1]);
    pEntry->hostMac.octet[2] = (uint8)(mac_uint32[0] >> 24);
    pEntry->hostMac.octet[3] = (uint8)(mac_uint32[0] >> 16);
    pEntry->hostMac.octet[4] = (uint8)(mac_uint32[0] >> 8);
    pEntry->hostMac.octet[5] = (uint8)(mac_uint32[0]);

    L3_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "pEntry->hostMac =%x-%x-%x-%x-%x-%x", \
            pEntry->hostMac.octet[0], pEntry->hostMac.octet[1], pEntry->hostMac.octet[2],\
            pEntry->hostMac.octet[3], pEntry->hostMac.octet[4], pEntry->hostMac.octet[5]);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_l3_routeEntry_set
 * Description:
 *      Set L3 rounting entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 *      pEntry - L3 route entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry)
{
    int32   ret;
    uint32  mac_uint32[2];
    routing_entry_t route_entry;

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "unit=%d, index=%d, hostMac=%x-%x-%x-%x-%x-%x",\
        unit, \
        pEntry->hostMac.octet[0], pEntry->hostMac.octet[1], pEntry->hostMac.octet[2],\
        pEntry->hostMac.octet[3], pEntry->hostMac.octet[4], pEntry->hostMac.octet[5]);

    mac_uint32[1]  = ((uint32)pEntry->hostMac.octet[0]) << 8;
    mac_uint32[1] |= ((uint32)pEntry->hostMac.octet[1]);
    mac_uint32[0]  = ((uint32)pEntry->hostMac.octet[2]) << 24;
    mac_uint32[0] |= ((uint32)pEntry->hostMac.octet[3]) << 16;
    mac_uint32[0] |= ((uint32)pEntry->hostMac.octet[4]) << 8;
    mac_uint32[0] |= ((uint32)pEntry->hostMac.octet[5]);

    osal_memset(&route_entry, 0, sizeof(routing_entry_t));

    L3_SEM_LOCK(unit);

    if ((ret = table_field_set(unit, MAPLE_ROUTINGt, MAPLE_ROUTING_GATEWAY_MACtf,
                            &mac_uint32[0], (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_write(unit, MAPLE_ROUTINGt, index, (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

