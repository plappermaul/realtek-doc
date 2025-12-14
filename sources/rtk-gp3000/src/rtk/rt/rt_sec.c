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
 * $Revision: 85849 $
 * $Date: 2018-01-30 10:05:23 +0800 (Tue, 30 Jan 2018) $
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
#include <common/rt_error.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_sec.h>

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
 *      rt_sec_init
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
rt_sec_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->sec_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_sec_init */

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */


/* Function Name:
 *      rt_sec_portAttackPreventState_get
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
rt_sec_portAttackPreventState_get(rt_port_t port, rt_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->sec_portAttackPreventState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_portAttackPreventState_get(port, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_sec_portAttackPreventState_get */

/* Function Name:
 *      rt_sec_portAttackPreventState_set
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
rt_sec_portAttackPreventState_set(rt_port_t port, rt_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->sec_portAttackPreventState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_portAttackPreventState_set(port, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_sec_portAttackPreventState_set */



/* Function Name:
 *      rt_sec_attackPrevent_get
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
rt_sec_attackPrevent_get(
    rt_sec_attackType_t    attackType,
    rt_action_t            *pAction)
{
    int32   ret;
    rtk_action_t action;

    /* function body */
    if (NULL == RT_MAPPER->sec_attackPrevent_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackPrevent_get(attackType, &action);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pAction = RT_ACTION_FORWARD;
                break;
            case ACTION_DROP:
                *pAction = RT_ACTION_DROP;
                break;
            case ACTION_TRAP2CPU:
                *pAction = RT_ACTION_TRAP2CPU;
                break;
            case ACTION_COPY2CPU:
                *pAction = RT_ACTION_COPY2CPU;
                break;
            case ACTION_TO_GUESTVLAN:
                *pAction = RT_ACTION_TO_GUESTVLAN;
                break;
            case ACTION_FLOOD_IN_VLAN:
                *pAction = RT_ACTION_FLOOD_IN_VLAN;
                break;
            case ACTION_FLOOD_IN_ALL_PORT:
                *pAction = RT_ACTION_FLOOD_IN_ALL_PORT;
                break;
            case ACTION_FLOOD_IN_ROUTER_PORTS:
                *pAction = RT_ACTION_FLOOD_IN_ROUTER_PORTS;
                break;
            case ACTION_FORWARD_EXCLUDE_CPU:
                *pAction = RT_ACTION_FORWARD_EXCLUDE_CPU;
                break;
            case ACTION_DROP_EXCLUDE_RMA:
                *pAction = RT_ACTION_DROP_EXCLUDE_RMA;
                break;
            case ACTION_FOLLOW_FB:
                *pAction = RT_ACTION_FOLLOW_FB;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    return ret;
}   /* end of rt_sec_attackPrevent_get */

/* Function Name:
 *      rt_sec_attackPrevent_set
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
rt_sec_attackPrevent_set(
    rt_sec_attackType_t    attackType,
    rt_action_t            action)
{
    int32   ret;
    rtk_action_t rtk_action;

    /* function body */
    if (NULL == RT_MAPPER->sec_attackPrevent_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(action)
    {
        case RT_ACTION_FORWARD:
            rtk_action = ACTION_FORWARD;
            break;
        case RT_ACTION_DROP:
            rtk_action = ACTION_DROP;
            break;
        case RT_ACTION_TRAP2CPU:
            rtk_action = ACTION_TRAP2CPU;
            break;
        case RT_ACTION_COPY2CPU:
            rtk_action = ACTION_COPY2CPU;
            break;
        case RT_ACTION_TO_GUESTVLAN:
            rtk_action = ACTION_TO_GUESTVLAN;
            break;
        case RT_ACTION_FLOOD_IN_VLAN:
            rtk_action = ACTION_FLOOD_IN_VLAN;
            break;
        case RT_ACTION_FLOOD_IN_ALL_PORT:
            rtk_action = ACTION_FLOOD_IN_ALL_PORT;
            break;
        case RT_ACTION_FLOOD_IN_ROUTER_PORTS:
            rtk_action = ACTION_FLOOD_IN_ROUTER_PORTS;
            break;
        case RT_ACTION_FORWARD_EXCLUDE_CPU:
            rtk_action = ACTION_FORWARD_EXCLUDE_CPU;
            break;
        case RT_ACTION_DROP_EXCLUDE_RMA:
            rtk_action = ACTION_DROP_EXCLUDE_RMA;
            break;
        case RT_ACTION_FOLLOW_FB:
            rtk_action = ACTION_FOLLOW_FB;
            break;
        default:
            return RT_ERR_INPUT;
    }
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackPrevent_set(attackType, rtk_action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_sec_attackPrevent_set */


/* Function Name:
 *      rt_sec_attackFloodThresh_get
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
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rt_sec_attackFloodThresh_get(rt_sec_attackFloodType_t type, uint32 *pFloodThresh)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->sec_attackFloodThresh_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThresh_get(type, pFloodThresh);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_sec_attackFloodThresh_get */

/* Function Name:
 *      rt_sec_attackFloodThresh_set
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
rt_sec_attackFloodThresh_set(rt_sec_attackFloodType_t type, uint32 floodThresh)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->sec_attackFloodThresh_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThresh_set(type, floodThresh);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_sec_attackFloodThresh_set */

/* Function Name:
 *      rt_sec_attackFloodThreshUnit_get
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
rt_sec_attackFloodThreshUnit_get(rt_sec_attackFloodType_t type, uint32 *pFloodThreshUnit)
{
    int32   ret=RT_ERR_OK;
#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->sec_attackFloodThreshUnit_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThreshUnit_get(type, pFloodThreshUnit);
    RTK_API_UNLOCK();
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    if(type >= RT_SEC_FLOOD_END)
        return RT_ERR_INPUT;
    *pFloodThreshUnit = 1;
    ret = RT_ERR_OK;
#endif
#endif
    return ret;
}   /* end of rt_sec_attackFloodThreshUnit_get */

/* Function Name:
 *      rt_sec_attackFloodThreshUnit_set
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
rt_sec_attackFloodThreshUnit_set(rt_sec_attackFloodType_t type, uint32 floodThreshUnit)
{
    int32   ret=RT_ERR_OK;
#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->sec_attackFloodThreshUnit_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->sec_attackFloodThreshUnit_set(type, floodThreshUnit);
    RTK_API_UNLOCK();
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    if(type >= RT_SEC_FLOOD_END)
        return RT_ERR_INPUT;
    if(floodThreshUnit != 1)
        return RT_ERR_INPUT;
    ret = RT_ERR_OK;
#endif
#endif
    return ret;
}   /* end of rt_sec_attackFloodThreshUnit_set */
