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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_switch.h>
#include <dal/cypress/dal_cypress_l2.h>
#include <dal/cypress/dal_cypress_l3.h>
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

/* Function Name:
 *      dal_cypress_l3Mapper_init
 * Description:
 *      Hook l3 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook l3 module before calling any l3 APIs.
 */
int32
dal_cypress_l3Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l3_init = dal_cypress_l3_init;
    pMapper->l3_routeEntry_get = dal_cypress_l3_routeEntry_get;
    pMapper->l3_routeEntry_set = dal_cypress_l3_routeEntry_set;
    pMapper->l3_routeSwitchMacAddr_get = dal_cypress_l3_routeSwitchMacAddr_get;
    pMapper->l3_routeSwitchMacAddr_set = dal_cypress_l3_routeSwitchMacAddr_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_l3_init
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
dal_cypress_l3_init(uint32 unit)
{
    uint32    i;
    rtk_mac_t cpuMac;

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

    /* set CPU MAC to the first switch MAC address entry for routing */
    dal_cypress_switch_mgmtMacAddr_get(unit, &cpuMac);
    dal_cypress_l3_routeSwitchMacAddr_set(unit, 0, &cpuMac);
    /* Set other entry to invalid MAC addr */
    cpuMac.octet[0] = 0xff;
    cpuMac.octet[1] = 0xff;
    cpuMac.octet[2] = 0xff;
    cpuMac.octet[3] = 0xff;
    cpuMac.octet[4] = 0xff;
    cpuMac.octet[5] = 0xfe;
    for (i = 1; i < HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit); i++)
        dal_cypress_l3_routeSwitchMacAddr_set(unit, i, &cpuMac);

    return RT_ERR_OK;
} /*end of dal_cypress_l3_init*/

/* Module Name    : Layer3 routing                */

/* Function Name:
 *      dal_cypress_l3_routeEntry_get
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
dal_cypress_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry)
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

    osal_memset(&route_entry, 0x00, sizeof(routing_entry_t));

    L3_SEM_LOCK(unit);

    if ((ret = table_read(unit, CYPRESS_ROUTINGt, index, (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_ROUTINGt, CYPRESS_ROUTING_GATEWAY_MACtf,
                            &mac_uint32[0], (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_ROUTINGt, CYPRESS_ROUTING_SW_MAC_IDXtf,
                            &pEntry->swMac_idx, (uint32 *) &route_entry)) != RT_ERR_OK)
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

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "swMac_idx=%d, pEntry->hostMac.=%x-%x-%x-%x-%x-%x", pEntry->swMac_idx,\
            pEntry->hostMac.octet[0], pEntry->hostMac.octet[1], pEntry->hostMac.octet[2],\
            pEntry->hostMac.octet[3], pEntry->hostMac.octet[4], pEntry->hostMac.octet[5]);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_l3_routeEntry_set
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
dal_cypress_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry)
{
    int32   ret;
    uint32  mac_uint32[2];
    routing_entry_t route_entry;

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pEntry->swMac_idx >= HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR(unit)), RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "unit=%d, index=%d, swMac_idx=%d, hostMac=%x-%x-%x-%x-%x-%x",\
        unit, index, pEntry->swMac_idx,\
        pEntry->hostMac.octet[0], pEntry->hostMac.octet[1], pEntry->hostMac.octet[2],\
        pEntry->hostMac.octet[3], pEntry->hostMac.octet[4], pEntry->hostMac.octet[5]);

    mac_uint32[1]  = ((uint32)pEntry->hostMac.octet[0]) << 8;
    mac_uint32[1] |= ((uint32)pEntry->hostMac.octet[1]);
    mac_uint32[0]  = ((uint32)pEntry->hostMac.octet[2]) << 24;
    mac_uint32[0] |= ((uint32)pEntry->hostMac.octet[3]) << 16;
    mac_uint32[0] |= ((uint32)pEntry->hostMac.octet[4]) << 8;
    mac_uint32[0] |= ((uint32)pEntry->hostMac.octet[5]);

    /* clear memory */
    osal_memset(&route_entry, 0x00, sizeof(routing_entry_t));

    L3_SEM_LOCK(unit);

    if ((ret = table_field_set(unit, CYPRESS_ROUTINGt, CYPRESS_ROUTING_GATEWAY_MACtf,
                            &mac_uint32[0], (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_ROUTINGt, CYPRESS_ROUTING_SW_MAC_IDXtf,
                            &pEntry->swMac_idx, (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_ROUTINGt, index, (uint32 *) &route_entry)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_l3_routeSwitchMacAddr_get
 * Description:
 *      Get rounting switch MAC address.
 * Input:
 *      unit    - unit id
 *      index   - index of switch MAC address
 * Output:
 *      pMac    - pointer to switch MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_l3_routeSwitchMacAddr_get(uint32 unit, uint32 index, rtk_mac_t *pMac)
{
    int32  ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    L3_SEM_LOCK(unit);

    if((ret = reg_array_field_read(unit, CYPRESS_ROUTING_SA_CTRLr, REG_ARRAY_INDEX_NONE,
                        index, CYPRESS_SA_31_0f, &val)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }
    pMac->octet[2] = (val >> 24) & 0xff;
    pMac->octet[3] = (val >> 16) & 0xff;
    pMac->octet[4] = (val >> 8) & 0xff;
    pMac->octet[5] = (val >> 0) & 0xff;

    if((ret = reg_array_field_read(unit, CYPRESS_ROUTING_SA_CTRLr, REG_ARRAY_INDEX_NONE,
                        index, CYPRESS_SA_47_32f, &val)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }
    pMac->octet[0] = (val >> 8) & 0xff;
    pMac->octet[1] = (val >> 0) & 0xff;

    L3_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    return RT_ERR_OK;
} /*end of dal_cypress_l3_routeSwitchMacAddr_get*/

/* Function Name:
 *      dal_cypress_l3_routeSwitchMacAddr_set
 * Description:
 *      Set rounting switch MAC address.
 * Input:
 *      unit    - unit id
 *      index   - index of switch MAC address
 *      pMac    - pointer to switch MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_l3_routeSwitchMacAddr_set(uint32 unit, uint32 index, rtk_mac_t *pMac)
{
    int32  ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), "unit=%d, index=%d, pMac=%x-%x-%x-%x-%x-%x", unit, index,
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    L3_SEM_LOCK(unit);

    val = (pMac->octet[2] << 24) | (pMac->octet[3] << 16) | (pMac->octet[4] << 8) | (pMac->octet[5] << 0);
    if((ret = reg_array_field_write(unit, CYPRESS_ROUTING_SA_CTRLr, REG_ARRAY_INDEX_NONE,
                        index, CYPRESS_SA_31_0f, &val)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    val = (pMac->octet[0] << 8) | (pMac->octet[1] << 0);
    if((ret = reg_array_field_write(unit, CYPRESS_ROUTING_SA_CTRLr, REG_ARRAY_INDEX_NONE,
                        index, CYPRESS_SA_47_32f, &val)) != RT_ERR_OK)
    {
        L3_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /*end of dal_cypress_l3_routeSwitchMacAddr_set*/

