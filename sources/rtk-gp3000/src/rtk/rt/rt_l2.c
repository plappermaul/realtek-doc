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
 * $Revision: 81643 $
 * $Date: 2017-08-24 13:28:53 +0800 (Thu, 24 Aug 2017) $
 *
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *           (12) Age time
 *           (13) IVL/SVL mode
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_l2.h>
#include <rtk/rt/rt_port.h>

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#include <rt/include/rt_ext_mapper.h>
#endif

#ifdef CONFIG_EXTERNAL_SWITCH
#include <dal/dal_ext_switch_mapper.h>
#endif
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

extern rt_port_uniType_t g_port_uniType[]; /*only for UNI port 0~3?*/

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
static rt_ext_mapper_t* RT_EXT_MAPPER = NULL;
#endif

/*
 * Macro Declaration
 */
#define RT_L2_S_TO_100MS(_sec)  (_sec*10)
#define RT_L2_100MS_TO_S(_100ms)  (_100ms/10)


static int32 _rt_l2_age_calculate(uint32 ori,uint32 *age);

/*
 * Function Declaration
 */

static int32 _rt_l2_age_calculate(uint32 ori,uint32 *age)
{
    int32   ret = RT_ERR_OK;
    uint32 agingTimeUnit = 1;

    if (NULL == RT_MAPPER->l2_aging_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_aging_get(&agingTimeUnit);
    RTK_API_UNLOCK();
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    *age = (ori*agingTimeUnit/10)/7;
#else
    *age = ori*agingTimeUnit/255;    
#endif

    return ret;
}


/* Function Name:
 *      rt_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
rt_l2_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l2_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_init */

/* Function Name:
 *      rt_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
rt_l2_portLimitLearningCnt_get(rt_port_t port, int32 *pMacCnt)
{
    int32   ret = RT_ERR_OK;

    if(!HAL_IS_GE_PORT(port) && !HAL_IS_FE_PORT(port) && !HAL_IS_SERDES_PORT(port) && !HAL_IS_XE_PORT(port))
        return RT_ERR_PORT_ID;

    if(pMacCnt == NULL)
        return RT_ERR_NULL_POINTER;

    /* function body */
    if (NULL == RT_MAPPER->l2_portLimitLearningCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(g_port_uniType[port] == RT_PORT_UNI_TYPE_PPTP)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_portLimitLearningCnt_get(port, pMacCnt);
        RTK_API_UNLOCK();
        if(*pMacCnt==2112)
        {
            *pMacCnt = -1;
        }
#else
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_portLimitLearningCnt_get(port, pMacCnt);
        RTK_API_UNLOCK();
        if(*pMacCnt==8191)
        {
            *pMacCnt = -1;
        }

#endif
    }
    else
    {
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        if(NULL == RT_EXT_MAPPER)
        {
            RT_EXT_MAPPER = rt_ext_mapper_get();
            if(RT_EXT_MAPPER == NULL)
                return RT_ERR_DRIVER_NOT_FOUND;
        }

        if(RT_EXT_MAPPER->internal_macAddr_learning_limit_get == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;

        RTK_API_LOCK();
        ret = RT_EXT_MAPPER->internal_macAddr_learning_limit_get(port, pMacCnt);
        RTK_API_UNLOCK();
#endif
    }
    return ret;
}   /* end of rt_l2_portLimitLearningCnt_get */


/* Function Name:
 *      rt_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
int32
rt_l2_portLimitLearningCnt_set(rt_port_t port, int32 macCnt)
{
    int32   ret = RT_ERR_OK;

    if(!HAL_IS_GE_PORT(port) && !HAL_IS_FE_PORT(port) && !HAL_IS_SERDES_PORT(port) && !HAL_IS_XE_PORT(port))
        return RT_ERR_PORT_ID;

    /* function body */
    if (NULL == RT_MAPPER->l2_portLimitLearningCnt_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(g_port_uniType[port] == RT_PORT_UNI_TYPE_PPTP)
    {
        if(macCnt == -1)
        {
            macCnt = HAL_L2_LEARN_LIMIT_CNT_MAX();
        }
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_portLimitLearningCnt_set(port, macCnt);
        RTK_API_UNLOCK();
    }
    else
    {
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        if(NULL == RT_EXT_MAPPER)
        {
            RT_EXT_MAPPER = rt_ext_mapper_get();
            if(RT_EXT_MAPPER == NULL)
                return RT_ERR_DRIVER_NOT_FOUND;
        }

        if(RT_EXT_MAPPER->internal_macAddr_learning_limit_set == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;

        RTK_API_LOCK();
        ret = RT_EXT_MAPPER->internal_macAddr_learning_limit_set(port, macCnt);
        RTK_API_UNLOCK();
#endif
    }

    return ret;
}   /* end of rt_l2_portLimitLearningCnt_set */

/* Function Name:
 *      rt_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when exceed the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pLearnCntAc - mac learning exceed action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action support forward/drop/trap
 */
int32
rt_l2_portLimitLearningCntAction_get(rt_port_t port, rt_l2_limitLearnCntAction_t *pLearnCntAct)
{
    int32   ret = RT_ERR_OK;

    if(g_port_uniType[port] == RT_PORT_UNI_TYPE_PPTP)
    {
        /* function body */
        if (NULL == RT_MAPPER->l2_portLimitLearningCntAction_get)
            return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_portLimitLearningCntAction_get(port, (rtk_l2_limitLearnCntAction_t *)pLearnCntAct);
        RTK_API_UNLOCK();
    }
    else
    {
        /* To Do for VEIP port */
        return RT_ERR_DRIVER_NOT_FOUND;
    }
    return ret;
}   /* end of rt_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      rt_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when exceed the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      learnCntAc - mac learning exceed action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_PORT_ID             - invalid port id
 * Note:
 *      Action support forward/drop/trap
 */
int32
rt_l2_portLimitLearningCntAction_set(rt_port_t port, rt_l2_limitLearnCntAction_t learnCntAct)
{
    int32   ret = RT_ERR_OK;

    if(g_port_uniType[port] == RT_PORT_UNI_TYPE_PPTP)
    {
        /* function body */
        if (NULL == RT_MAPPER->l2_portLimitLearningCntAction_set)
            return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_portLimitLearningCntAction_set(port, (rtk_l2_limitLearnCntAction_t)learnCntAct);
        RTK_API_UNLOCK();
    }
    else
    {
        /* To do for VEIP port */
        return RT_ERR_DRIVER_NOT_FOUND;
    }
    return ret;
}   /* end of rt_l2_portLimitLearningCntAction_set */


/* Function Name:
 *      rt_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
rt_l2_newMacOp_get(rt_port_t port,rt_action_t *pFwdAction)
{
    int32   ret;
    rtk_l2_newMacLrnMode_t mode;
    rtk_action_t action;

    if(!HAL_IS_GE_PORT(port) && !HAL_IS_FE_PORT(port) && !HAL_IS_SERDES_PORT(port))
        return RT_ERR_PORT_ID;

    if(pFwdAction == NULL)
        return RT_ERR_NULL_POINTER;

    if (NULL == RT_MAPPER->l2_newMacOp_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_newMacOp_get(port, &mode, &action);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pFwdAction = RT_ACTION_FORWARD;
                break;
            case ACTION_DROP:
                *pFwdAction = RT_ACTION_DROP;
                break;
            case ACTION_TRAP2CPU:
                *pFwdAction = RT_ACTION_TRAP2CPU;
                break;
            case ACTION_COPY2CPU:
                *pFwdAction = RT_ACTION_COPY2CPU;
                break;
            case ACTION_TO_GUESTVLAN:
                *pFwdAction = RT_ACTION_TO_GUESTVLAN;
                break;
            case ACTION_FLOOD_IN_VLAN:
                *pFwdAction = RT_ACTION_FLOOD_IN_VLAN;
                break;
            case ACTION_FLOOD_IN_ALL_PORT:
                *pFwdAction = RT_ACTION_FLOOD_IN_ALL_PORT;
                break;
            case ACTION_FLOOD_IN_ROUTER_PORTS:
                *pFwdAction = RT_ACTION_FLOOD_IN_ROUTER_PORTS;
                break;
            case ACTION_FORWARD_EXCLUDE_CPU:
                *pFwdAction = RT_ACTION_FORWARD_EXCLUDE_CPU;
                break;
            case ACTION_DROP_EXCLUDE_RMA:
                *pFwdAction = RT_ACTION_DROP_EXCLUDE_RMA;
                break;
            case ACTION_FOLLOW_FB:
                *pFwdAction = RT_ACTION_FOLLOW_FB;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    return ret;
}

/* Function Name:
 *      rt_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
rt_l2_newMacOp_set(rt_port_t port,rt_action_t fwdAction)
{
    int32   ret;
    rtk_action_t rtk_action;
    
    if(!HAL_IS_GE_PORT(port) && !HAL_IS_FE_PORT(port) && !HAL_IS_SERDES_PORT(port))
        return RT_ERR_PORT_ID;

    if(fwdAction != ACTION_FORWARD && fwdAction != ACTION_DROP)
        return RT_ERR_INPUT;

    if (NULL == RT_MAPPER->l2_newMacOp_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(fwdAction)
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
    ret = RT_MAPPER->l2_newMacOp_set(port, HARDWARE_LEARNING, rtk_action);
    RTK_API_UNLOCK();

    return ret;
}

/* Function Name:
 *      rt_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_FULL       - entry is full 
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
rt_l2_addr_add(rt_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    rtk_l2_ucastAddr_t l2Addr;
    uint32 agingTimeUnit = 1;   
    
    if(pL2Addr == NULL)
        return RT_ERR_NULL_POINTER;

    if(!HAL_IS_GE_PORT(pL2Addr->port) && !HAL_IS_FE_PORT(pL2Addr->port) && !HAL_IS_SERDES_PORT(pL2Addr->port))
        return RT_ERR_PORT_ID;

    memset(&l2Addr,0x00,sizeof(rtk_l2_ucastAddr_t));

    /* function body */
    if (NULL == RT_MAPPER->l2_addr_add)
        return RT_ERR_DRIVER_NOT_FOUND;

    l2Addr.fid = 0;
    l2Addr.efid = 0;
    l2Addr.flags = 0;
    l2Addr.port = pL2Addr->port;
    l2Addr.vid = pL2Addr->vid;

    memcpy(l2Addr.mac.octet,pL2Addr->mac.octet,ETHER_ADDR_LEN);

    if(pL2Addr->staticFlag)
        l2Addr.flags|= RTK_L2_UCAST_FLAG_STATIC;
    else
    {
        if (NULL == RT_MAPPER->l2_aging_get)
            return RT_ERR_DRIVER_NOT_FOUND;
        
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_aging_get(&agingTimeUnit);
        RTK_API_UNLOCK();
        
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        l2Addr.age = ((pL2Addr->age > agingTimeUnit/10 || pL2Addr->age == 0) ? 7 : pL2Addr->age*10*7/agingTimeUnit);
#else
        l2Addr.age = ((pL2Addr->age > agingTimeUnit || pL2Addr->age == 0) ? 255 : pL2Addr->age*255/agingTimeUnit);
#endif

    }
    
    if(pL2Addr->filterFlag)
        l2Addr.flags|= RTK_L2_UCAST_FLAG_SA_BLOCK;
   

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_add(&l2Addr);
    RTK_API_UNLOCK();

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if(RT_EXT_MAPPER->internal_l2_addr_add == NULL)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_EXT_MAPPER->internal_l2_addr_add(pL2Addr);
    RTK_API_UNLOCK();
#endif
    
    return ret;
}

/* Function Name:
 *      rt_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
rt_l2_addr_del(rt_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    rtk_l2_ucastAddr_t l2Addr;

    if(pL2Addr == NULL)
        return RT_ERR_NULL_POINTER;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if(RT_EXT_MAPPER->internal_l2_addr_del == NULL)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_EXT_MAPPER->internal_l2_addr_del(pL2Addr);
    RTK_API_UNLOCK();

    if(RT_ERR_OK == ret)
        return RT_ERR_OK;
#endif

    memset(&l2Addr,0x00,sizeof(rtk_l2_ucastAddr_t));    

    if (NULL == RT_MAPPER->l2_addr_del)
        return RT_ERR_DRIVER_NOT_FOUND;

    l2Addr.fid = 0;
    l2Addr.efid = 0;
    l2Addr.vid = pL2Addr->vid;
    memcpy(l2Addr.mac.octet,pL2Addr->mac.octet,ETHER_ADDR_LEN);

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_del(&l2Addr);
    RTK_API_UNLOCK();

    return ret;
}

/* Function Name:
 *      rt_l2_addr_get
 * Description:
 *      Get L2 entry based on MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
rt_l2_addr_get(rt_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    rtk_l2_ucastAddr_t l2Addr;
    
    if(pL2Addr == NULL)
        return RT_ERR_NULL_POINTER;

    memset(&l2Addr,0x00,sizeof(rtk_l2_ucastAddr_t));    

    if (NULL == RT_MAPPER->l2_addr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    l2Addr.fid = 0;
    l2Addr.efid = 0;
    l2Addr.vid = pL2Addr->vid;
    memcpy(l2Addr.mac.octet,pL2Addr->mac.octet,ETHER_ADDR_LEN);

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_get(&l2Addr);
    RTK_API_UNLOCK();
    if(ret != RT_ERR_OK)
        return ret;

    ret = _rt_l2_age_calculate(l2Addr.age,&pL2Addr->age);
    if(ret != RT_ERR_OK)
        return ret;

    pL2Addr->port = l2Addr.port;
    pL2Addr->vid = l2Addr.vid;
    pL2Addr->staticFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_STATIC)?ENABLED:DISABLED;
    pL2Addr->filterFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)?ENABLED:DISABLED;
    
    return ret;
}

/* Function Name:
 *      rt_l2_mcastAddr_add
 * Description:
 *      Add L2 mcast entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_FULL       - entry is full 
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
rt_l2_mcastAddr_add(rt_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    rtk_l2_mcastAddr_t l2McastAddr;

    if(pMcastAddr == NULL)
        return RT_ERR_NULL_POINTER;

    /* function body */
    if (NULL == RT_MAPPER->l2_mcastAddr_add)
        return RT_ERR_DRIVER_NOT_FOUND;

    memset(&l2McastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    memcpy(l2McastAddr.mac.octet,pMcastAddr->mac.octet,ETHER_ADDR_LEN);
    l2McastAddr.portmask.bits[0] = pMcastAddr->group;
    l2McastAddr.vid = pMcastAddr->vid;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_mcastAddr_add(&l2McastAddr);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_mcast_addr_add */

/* Function Name:
 *      rt_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                - OK
 *      RT_ERR_FAILED            - Failed
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
rt_l2_mcastAddr_del(rt_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    rtk_l2_mcastAddr_t l2McastAddr;

    if(pMcastAddr == NULL)
        return RT_ERR_NULL_POINTER;

    /* function body */
    if (NULL == RT_MAPPER->l2_mcastAddr_del)
        return RT_ERR_DRIVER_NOT_FOUND;

    memset(&l2McastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    memcpy(l2McastAddr.mac.octet,pMcastAddr->mac.octet,ETHER_ADDR_LEN);
    l2McastAddr.portmask.bits[0] = pMcastAddr->group;
    l2McastAddr.vid = pMcastAddr->vid;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_mcastAddr_del(&l2McastAddr);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_mcast_addr_del */

/* Function Name:
 *      rt_l2_mcastAddr_get
 * Description:
 *      Get L2 mcast entry based on MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK                - OK
 *      RT_ERR_FAILED            - Failed
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the multicast mac address existed in LUT, it will return the port and vid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
rt_l2_mcastAddr_get(rt_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    rtk_l2_mcastAddr_t l2McastAddr;

    if(pMcastAddr == NULL)
        return RT_ERR_NULL_POINTER;

    /* function body */
    if (NULL == RT_MAPPER->l2_mcastAddr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    memset(&l2McastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    memcpy(l2McastAddr.mac.octet,pMcastAddr->mac.octet,ETHER_ADDR_LEN);
    l2McastAddr.vid = pMcastAddr->vid;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_mcastAddr_get(&l2McastAddr);
    RTK_API_UNLOCK();

    if(ret != RT_ERR_OK)
        return ret;
    
    pMcastAddr->group = l2McastAddr.portmask.bits[0];

    return ret;
}   /* end of rt_l2_mcast_addr_get */

/* Function Name:
 *      rt_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx       - currently scan index of l2 table to get next.
 * Output:
 *      pL2Addr        - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rt_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rt_l2_ucastAddr_t   *pL2Addr)
{
    int32   ret;
    rtk_l2_ucastAddr_t l2Addr;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    int32 tmSwScanIdx = *pScanIdx;
    int32 tmHwScanIdx = *pScanIdx;
    rt_l2_ucastAddr_t swL2Addr;
    rt_enable_t swHitFlag = DISABLED;
    rt_enable_t hwHitFlag = DISABLED;
    rt_enable_t useSwFlag = DISABLED;

    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if(RT_EXT_MAPPER->internal_l2_addr_get == NULL)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->l2_nextValidAddr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    //Getting software address
    RTK_API_LOCK();
    ret = RT_EXT_MAPPER->internal_l2_addr_get(&tmSwScanIdx, &swL2Addr);
    RTK_API_UNLOCK();
    if(ret == 0)
        swHitFlag = ENABLED;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidAddr_get(&tmHwScanIdx, &l2Addr);
    RTK_API_UNLOCK();
    if(ret == RT_ERR_OK)
        hwHitFlag = ENABLED;

    if(swHitFlag == ENABLED && hwHitFlag == ENABLED)
    {
        if(tmHwScanIdx < tmSwScanIdx)
            useSwFlag = DISABLED;
        else
            useSwFlag = ENABLED;
    }
    else if(swHitFlag == ENABLED)
    {
        useSwFlag = ENABLED;
    }
    else if(hwHitFlag == ENABLED)
    {
        useSwFlag = DISABLED;
    }
    else
    {
        return RT_ERR_L2_ENTRY_NOTFOUND;
    }

    if(useSwFlag == ENABLED)
    {
        *pScanIdx = tmSwScanIdx;
        memcpy(pL2Addr->mac.octet,swL2Addr.mac.octet,ETHER_ADDR_LEN);
        pL2Addr->port = swL2Addr.port;
        pL2Addr->age = swL2Addr.age;
        pL2Addr->staticFlag = swL2Addr.staticFlag;
        pL2Addr->filterFlag = swL2Addr.filterFlag;
        pL2Addr->vid = swL2Addr.vid;
    }
    else
    {
        *pScanIdx = tmHwScanIdx;
        ret = _rt_l2_age_calculate(l2Addr.age,&pL2Addr->age);
        if(ret != RT_ERR_OK)
            return ret;

        memcpy(pL2Addr->mac.octet,l2Addr.mac.octet,ETHER_ADDR_LEN);
        pL2Addr->port = l2Addr.port;
        pL2Addr->staticFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_STATIC)?ENABLED:DISABLED;
        pL2Addr->filterFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)?ENABLED:DISABLED;
        pL2Addr->vid = l2Addr.vid;
    }
#else

    if (NULL == RT_MAPPER->l2_nextValidAddr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidAddr_get(pScanIdx, &l2Addr);
    RTK_API_UNLOCK();
    if(ret != RT_ERR_OK)
        return ret;

    ret = _rt_l2_age_calculate(l2Addr.age,&pL2Addr->age);
    if(ret != RT_ERR_OK)
        return ret;

    memcpy(pL2Addr->mac.octet,l2Addr.mac.octet,ETHER_ADDR_LEN);
    pL2Addr->port = l2Addr.port;
    pL2Addr->staticFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_STATIC)?ENABLED:DISABLED;
    pL2Addr->filterFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)?ENABLED:DISABLED;
    pL2Addr->vid = l2Addr.vid;
#endif

    return ret;
} /* end of rt_l2_nextValidAddr_get */

/* Function Name:
 *      rt_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      port          - speific port
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2Addr       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rt_l2_nextValidAddrOnPort_get(
    rt_port_t           port,
    int32               *pScanIdx,
    rt_l2_ucastAddr_t   *pL2Addr)
{
    int32   ret;
    rtk_l2_ucastAddr_t l2Addr;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    int32 tmSwScanIdx = *pScanIdx;
    int32 tmHwScanIdx = *pScanIdx;
    rt_l2_ucastAddr_t swL2Addr;
    rt_enable_t swHitFlag = DISABLED;
    rt_enable_t hwHitFlag = DISABLED;
    rt_enable_t useSwFlag = DISABLED;

    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if(RT_EXT_MAPPER->internal_l2_addr_get == NULL)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->l2_nextValidAddr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    //Getting software address
    while(1)
    {
        RTK_API_LOCK();
        ret = RT_EXT_MAPPER->internal_l2_addr_get(&tmSwScanIdx, &swL2Addr);
        RTK_API_UNLOCK();

        if(ret != 0)
            break;
        else
        {
 //	printk("tmSwScanIdx=%d swL2Addr.port=%d\n",tmSwScanIdx,swL2Addr.port);
            if(swL2Addr.port != port)
            {
                tmSwScanIdx++;
                continue;
            }
            break;
        }
    }

    if(ret == 0)
        swHitFlag = ENABLED;
#if defined(CONFIG_EXTERNAL_SWITCH)
    if(IS_EXT_SWITCH_PORT(port))
    {
        ret = RT_ERR_L2_ENTRY_NOTFOUND;
    }
    else
    {
        RTK_API_LOCK();
        ret = RT_MAPPER->l2_nextValidAddrOnPort_get(port, &tmHwScanIdx, &l2Addr);
        RTK_API_UNLOCK();
        if(ret == RT_ERR_OK)
            hwHitFlag = ENABLED;
    }	
#else
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidAddrOnPort_get(port, &tmHwScanIdx, &l2Addr);
    RTK_API_UNLOCK();
    if(ret == RT_ERR_OK)
        hwHitFlag = ENABLED;
#endif
   //printk("hwHitFlag=%d tmHwScanIdx=%d swHitFlag=%d tmSwScanIdx=%d\n"
        //,hwHitFlag,tmHwScanIdx,swHitFlag,tmSwScanIdx);

    if(swHitFlag == ENABLED && hwHitFlag == ENABLED)
    {
        if(tmHwScanIdx < tmSwScanIdx)
            useSwFlag = DISABLED;
        else
            useSwFlag = ENABLED;
    }
    else if(swHitFlag == ENABLED)
    {
        useSwFlag = ENABLED;
    }
    else if(hwHitFlag == ENABLED)
    {
        useSwFlag = DISABLED;
    }
    else
    {
        return RT_ERR_L2_ENTRY_NOTFOUND;
    }

    if(useSwFlag == ENABLED)
    {
        *pScanIdx = tmSwScanIdx;
        memcpy(pL2Addr->mac.octet,swL2Addr.mac.octet,ETHER_ADDR_LEN);
        pL2Addr->port = swL2Addr.port;
        pL2Addr->age = swL2Addr.age;
        pL2Addr->staticFlag = swL2Addr.staticFlag;
        pL2Addr->filterFlag = swL2Addr.filterFlag;
        pL2Addr->vid = swL2Addr.vid;
    }
    else
    {
        *pScanIdx = tmHwScanIdx;
        ret = _rt_l2_age_calculate(l2Addr.age,&pL2Addr->age);
        if(ret != RT_ERR_OK)
            return ret;

        memcpy(pL2Addr->mac.octet,l2Addr.mac.octet,ETHER_ADDR_LEN);
        pL2Addr->port = l2Addr.port;
        pL2Addr->staticFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_STATIC)?ENABLED:DISABLED;
        pL2Addr->filterFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)?ENABLED:DISABLED;
        pL2Addr->vid = l2Addr.vid;
    }
#else
    /* function body */
    if (NULL == RT_MAPPER->l2_nextValidAddrOnPort_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidAddrOnPort_get(port, pScanIdx, &l2Addr);
    RTK_API_UNLOCK();
    if(ret != RT_ERR_OK)
        return ret;

    ret = _rt_l2_age_calculate(l2Addr.age,&pL2Addr->age);
    if(ret != RT_ERR_OK)
        return ret;

    memcpy(pL2Addr->mac.octet,l2Addr.mac.octet,ETHER_ADDR_LEN);
    pL2Addr->port = l2Addr.port;
    pL2Addr->staticFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_STATIC)?ENABLED:DISABLED;
    pL2Addr->filterFlag = (l2Addr.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)?ENABLED:DISABLED;
    pL2Addr->vid = l2Addr.vid;
#endif
    return RT_ERR_OK;
} /* end of rt_l2_nextValidAddrOnPort_get */

/* Function Name:
 *      rt_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rt_l2_illegalPortMoveAction_get(rt_port_t port,rt_action_t *pFwdAction)
{
    int32   ret;
    rtk_action_t action;

    if(pFwdAction == NULL)
        return RT_ERR_NULL_POINTER;
   
    /* function body */
    if (NULL == RT_MAPPER->l2_illegalPortMoveAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_illegalPortMoveAction_get( port, &action);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pFwdAction = RT_ACTION_FORWARD;
                break;
            case ACTION_DROP:
                *pFwdAction = RT_ACTION_DROP;
                break;
            case ACTION_TRAP2CPU:
                *pFwdAction = RT_ACTION_TRAP2CPU;
                break;
            case ACTION_COPY2CPU:
                *pFwdAction = RT_ACTION_COPY2CPU;
                break;
            case ACTION_TO_GUESTVLAN:
                *pFwdAction = RT_ACTION_TO_GUESTVLAN;
                break;
            case ACTION_FLOOD_IN_VLAN:
                *pFwdAction = RT_ACTION_FLOOD_IN_VLAN;
                break;
            case ACTION_FLOOD_IN_ALL_PORT:
                *pFwdAction = RT_ACTION_FLOOD_IN_ALL_PORT;
                break;
            case ACTION_FLOOD_IN_ROUTER_PORTS:
                *pFwdAction = RT_ACTION_FLOOD_IN_ROUTER_PORTS;
                break;
            case ACTION_FORWARD_EXCLUDE_CPU:
                *pFwdAction = RT_ACTION_FORWARD_EXCLUDE_CPU;
                break;
            case ACTION_DROP_EXCLUDE_RMA:
                *pFwdAction = RT_ACTION_DROP_EXCLUDE_RMA;
                break;
            case ACTION_FOLLOW_FB:
                *pFwdAction = RT_ACTION_FOLLOW_FB;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    
    return ret;
} /* end of rt_l2_illegalPortMoveAction_get */

/* Function Name:
 *      rt_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rt_l2_illegalPortMoveAction_set(rt_port_t port,rt_action_t fwdAction)
{
    int32   ret;
    rtk_action_t rtk_action;
    
    /* function body */
    if (NULL == RT_MAPPER->l2_illegalPortMoveAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(fwdAction)
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
    ret = RT_MAPPER->l2_illegalPortMoveAction_set( port, rtk_action);
    RTK_API_UNLOCK();
    
    return ret;
} /* end of rt_l2_illegalPortMoveAction_set */


/* Function Name:
 *      rt_l2_ageTime_get
 * Description:
 *      Get L2 age time
 * Input:
 *      None
 * Output:
 *      pAgeTime  - age time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */

int32
rt_l2_ageTime_get(uint32 *pAgeTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l2_aging_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_aging_get(pAgeTime);
    RTK_API_UNLOCK();

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    *pAgeTime = RT_L2_100MS_TO_S(*pAgeTime);
#endif
    
    return ret;

} /* end of rt_l2_ageTime_get */


/* Function Name:
 *      rt_l2_ageTime_set
 * Description:
 *      Set L2 age time.
 * Input:
 *      ageTime  -ageTime
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      
 */
int32
rt_l2_ageTime_set(uint32 ageTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l2_aging_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    ageTime = RT_L2_S_TO_100MS(ageTime);
#endif

    RTK_API_LOCK();
    ret = RT_MAPPER->l2_aging_set(ageTime);
    RTK_API_UNLOCK();
    
    return ret;

} /* end of rt_l2_ageTime_set */

/* Function Name:
 *      rt_l2_ivlSvl_get
 * Description:
 *      Get L2 IVL SVL mode
 * Input:
 *      None
 * Output:
 *      pIvlEn  - IVL enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */
int32
rt_l2_ivlSvl_get(rt_enable_t *pIvlEn)
{

    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_l2_ivlSvl_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_l2_ivlSvl_get(pIvlEn);
    RTK_API_UNLOCK();

    return ret;

}


/* Function Name:
 *      rt_l2_ivlSvl_set
 * Description:
 *      Set L2 IVL SVL mode
 * Input:
 *      ivlEn  - IVL enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_ivlSvl_set(rt_enable_t ivlEn)
{

    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_l2_ivlSvl_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_l2_ivlSvl_set(ivlEn);
    RTK_API_UNLOCK();

    return ret;

}

/* Function Name:
 *      rt_l2_fwdGroupId_set
 * Description:
 *      Set l2 forward group action mask
 * Input:
 *      idx   - forward group index
 *      mask  - forward group mask
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupId_set(uint8 idx, uint64 mask)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_l2_fwdGroupId_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_l2_fwdGroupId_set(idx, mask);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_fwdGroupId_set */

/* Function Name:
 *      rt_l2_fwdGroupId_get
 * Description:
 *      Get l2 forward group action mask
 * Input:
 *      idx   - forward group index
 * Output:
 *      pMask  - forward group mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupId_get(uint8 idx, uint64 *pMask)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_l2_fwdGroupId_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_l2_fwdGroupId_get(idx, pMask);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_fwdGroupId_get */

/* Function Name:
 *      rt_l2_fwdGroupAction_set
 * Description:
 *      Set forward group action
 * Input:
 *      idx     - forward group action index
 *      portAct - port forward action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupAction_set(uint8 idx, rt_l2_portAction_t portAct)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_l2_fwdGroupAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_l2_fwdGroupAction_set(idx, portAct);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_fwdGroupAction_set */

/* Function Name:
 *      rt_l2_fwdGroupAction_get
 * Description:
 *      Get forward group action
 * Input:
 *      idx     - forward group action index
 * Output:
 *      pPortAct - port forward action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupAction_get(uint8 idx, rt_l2_portAction_t *pPortAct)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_l2_fwdGroupAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_l2_fwdGroupAction_get(idx, pPortAct);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_l2_fwdGroupAction_get */

