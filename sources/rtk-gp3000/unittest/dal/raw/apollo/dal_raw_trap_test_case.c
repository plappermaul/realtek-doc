#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_trap.h>
#include <dal/raw/apollo/dal_raw_trap_test_case.h>

int32 dal_raw_trap_igmp_test(uint32 caseNo)
{
    rtk_port_t port;
    apollo_raw_igmpmld_type_t type;
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trap_igmpAction_set(APOLLO_PORTIDMAX + 1, RAW_TYPE_IGMPV1, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_END, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_COPY2CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_FLOOD_IN_VLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_FLOOD_IN_ALL_PORT) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_FLOOD_IN_ROUTER_PORTS) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_FORWARD_EXCLUDE_CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpAction_set(0, RAW_TYPE_IGMPV1, ACTION_DROP_EXCLUDE_RMA) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        for(type = RAW_TYPE_IGMPV1; type < RAW_TYPE_END; type++)
        {
            for(actionW = ACTION_FORWARD; actionW <= ACTION_TRAP2CPU; actionW++)
            {
                if( apollo_raw_trap_igmpAction_set(port, type, actionW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( apollo_raw_trap_igmpAction_get(port, type, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionR != actionW)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_trap_igmpAction_get(0, RAW_TYPE_IGMPV1, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trap_igmp_BypassStorm_test(uint32 caseNo)
{
    rtk_enable_t enableW;
    rtk_enable_t enableR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trap_igmpBypassStrom_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW++)
    {
        if( apollo_raw_trap_igmpBypassStrom_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trap_igmpBypassStrom_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_trap_igmpBypassStrom_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trap_igmp_ChecksumError_test(uint32 caseNo)
{
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trap_igmpChechsumError_set(ACTION_COPY2CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpChechsumError_set(ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpChechsumError_set(ACTION_FLOOD_IN_VLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpChechsumError_set(ACTION_FLOOD_IN_ALL_PORT) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpChechsumError_set(ACTION_FLOOD_IN_ROUTER_PORTS) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpChechsumError_set(ACTION_FORWARD_EXCLUDE_CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_igmpChechsumError_set(ACTION_DROP_EXCLUDE_RMA) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(actionW = ACTION_FORWARD; actionW <= ACTION_TRAP2CPU; actionW++)
    {
        if( apollo_raw_trap_igmpChechsumError_set(actionW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trap_igmpChechsumError_get(&actionR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(actionR != actionW)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_trap_igmpChechsumError_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trap_igmp_IsolationLeaky_test(uint32 caseNo)
{
    rtk_enable_t enableW;
    rtk_enable_t enableR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trap_igmpIsoLeaky_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW++)
    {
        if( apollo_raw_trap_igmpIsoLeaky_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trap_igmpIsoLeaky_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_trap_igmpIsoLeaky_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trap_igmp_vlanLeaky_test(uint32 caseNo)
{
    rtk_enable_t enableW;
    rtk_enable_t enableR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trap_igmpVLANLeaky_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW++)
    {
        if( apollo_raw_trap_igmpVLANLeaky_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trap_igmpVLANLeaky_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_trap_igmpVLANLeaky_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

