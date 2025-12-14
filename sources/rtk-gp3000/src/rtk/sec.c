/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
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
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention 
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/sec.h> 
#include <dal/dal_mgmt.h> 
/*
 * Symbol Definition
 */
/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      rtk_sec_init
 * Description:
 *      Initialize security module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize security module before calling any sec APIs.
 */
int32
rtk_sec_init(void)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_init)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = RT_MAPPER->sec_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_init */

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */


/* Function Name:
 *      rtk_sec_portAttackPreventState_get
 * Description:
 *      Per port get attack prevention confi state
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status attack prevention
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status attack prevention:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_portAttackPreventState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_portAttackPreventState_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_portAttackPreventState_get */

/* Function Name:
 *      rtk_sec_portAttackPreventState_set
 * Description:
 *      Per port set attack prevention confi state
 * Input:
 *      port   - port id.
 *      enable - status attack prevention
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status attack prevention:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_portAttackPreventState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_portAttackPreventState_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_portAttackPreventState_set */



/* Function Name:
 *      rtk_sec_attackPrevent_get
 * Description:
 *      Get action for each kind of attack on specified port.
 * Input:
 *      attackType - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action is as following:
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
rtk_sec_attackPrevent_get(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            *pAction)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_attackPrevent_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackPrevent_get( attackType, pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_attackPrevent_get */

/* Function Name:
 *      rtk_sec_attackPrevent_set
 * Description:
 *      Set action for each kind of attack.
 * Input:
 *      attack_type - type of attack
 *      action      - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      Action is as following:
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
rtk_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_attackPrevent_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackPrevent_set( attackType, action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_attackPrevent_set */


/* Function Name:
 *      rtk_sec_attackFloodThresh_get
 * Description:
 *      Get flood threshold.
 * Input:
 *      None
 * Output:
 *      pFloodThresh - pointer to flood threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_attackFloodThresh_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThresh_get( type, pFloodThresh);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_attackFloodThresh_get */

/* Function Name:
 *      rtk_sec_attackFloodThresh_set
 * Description:
 *      Set flood threshold.
 * Input:
 *      floodThresh - flood threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh)
{
    int32   ret;
    if (NULL == RT_MAPPER->sec_attackFloodThresh_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThresh_set( type, floodThresh);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_sec_attackFloodThresh_set */

/* Function Name:
 *      rtk_sec_attackFloodThreshUnit_get
 * Description:
 *      Get time unit of flood threshold, 1/256/512 ms.
 * Input:
 *      None
 * Output:
 *      pFloodThreshUnit - pointer to flood threshold time unit
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThreshUnit_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThreshUnit)
{
    int32   ret;

    if (NULL == RT_MAPPER->sec_attackFloodThreshUnit_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThreshUnit_get(type, pFloodThreshUnit);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_sec_attackFloodThreshUnit_get */

/* Function Name:
 *      rtk_sec_attackFloodThreshUnit_set
 * Description:
 *      Set time unit of flood threshold, 1/256/512 ms.
 * Input:
 *      floodThresh - flood threshold time unit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThreshUnit_set(rtk_sec_attackFloodType_t type, uint32 floodThreshUnit)
{
    int32   ret;

    if (NULL == RT_MAPPER->sec_attackFloodThreshUnit_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThreshUnit_set(type, floodThreshUnit);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_sec_attackFloodThreshUnit_set */


