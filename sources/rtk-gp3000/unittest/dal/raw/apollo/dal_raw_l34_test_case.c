#include <osal/lib.h>
#include <dal/apollo/raw/apollo_raw_l34.h>
#include <rtk/l34.h>
#include <ioal/mem32.h>
#include <dal/raw/apollo/dal_raw_l34_test_case.h>



int32 dal_raw_l34_raw_test(uint32 caseNo)
{
    int32 ret;
    uint32 test_type;
    uint32 get_type;
    uint32 itemArray[4];
    uint32 i;
    uint32 port;
    uint8 test_index;
    uint8 get_index;
    /*error input check*/
    /*out of range*/
    if( apollo_raw_l34_LanIntfMDBC_set((RAW_L34_LIMBC_RESERVED)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_debugMonitor_set((RTK_ENABLE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_natAttack2CPU_set((RTK_ENABLE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_wanRouteAct_set(L34_WAN_ROUTE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_mode_set((RAW_L34_L3_MODE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_TtlOperationMode_set((RTK_ENABLE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_L3chksumErrAllow_set((RTK_ENABLE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_L4chksumErrAllow_set((RTK_ENABLE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_FragPkt2Cpu_set((RTK_ENABLE_END)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_portIntfIdx_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_portIntfIdx_set(0, L34_NETIF_TABLE_MAX_IDX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_portIntfIdx_get(APOLLO_PORTNO, &get_index) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_extPortIntfIdx_set(APOLLO_EXTPORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_extPortIntfIdx_set(0, L34_NETIF_TABLE_MAX_IDX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_extPortIntfIdx_get(APOLLO_EXTPORTNO, &get_index) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_vcPortIntfIdx_set(APOLLO_VCPORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_vcPortIntfIdx_set(0, L34_NETIF_TABLE_MAX_IDX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_vcPortIntfIdx_get(APOLLO_VCPORTNO, &get_index) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(test_type=RAW_L34_LIMBC_VLAN_BASED; test_type<RAW_L34_LIMBC_RESERVED ; test_type++)
    {
        if( apollo_raw_l34_LanIntfMDBC_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_LanIntfMDBC_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }


    for(test_type=DISABLED; test_type<RTK_ENABLE_END ; test_type++)
    {
        if( apollo_raw_l34_debugMonitor_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_debugMonitor_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(test_type=DISABLED; test_type<RTK_ENABLE_END ; test_type++)
    {
        if( apollo_raw_l34_natAttack2CPU_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_natAttack2CPU_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }


    itemArray[0] = L34_WAN_ROUTE_FWD;
    itemArray[1] = L34_WAN_ROUTE_FWD_TO_CPU;
    itemArray[2] = L34_WAN_ROUTE_DROP;
    for(i=0 ; i < 3 ; i++)
    {
        test_type=itemArray[i];
        if( apollo_raw_l34_wanRouteAct_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_wanRouteAct_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(test_type=RAW_L34_L3_DISABLE_L4_DISABLE; test_type<RAW_L34_L3_MODE_END ; test_type++)
    {
        if( apollo_raw_l34_mode_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_mode_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(test_type=DISABLED; test_type<RTK_ENABLE_END ; test_type++)
    {
        if( apollo_raw_l34_TtlOperationMode_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_TtlOperationMode_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(test_type=DISABLED; test_type<RTK_ENABLE_END ; test_type++)
    {
        if( apollo_raw_l34_L3chksumErrAllow_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_L3chksumErrAllow_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(test_type=DISABLED; test_type<RTK_ENABLE_END ; test_type++)
    {
        if( apollo_raw_l34_L4chksumErrAllow_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_L4chksumErrAllow_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(test_type=DISABLED; test_type<RTK_ENABLE_END ; test_type++)
    {
        if( apollo_raw_l34_FragPkt2Cpu_set((test_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l34_FragPkt2Cpu_get((&get_type)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(test_type != get_type)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(port = 0; port <= 5 ; port++)
    {
        for(test_index=0; test_index<L34_NETIF_TABLE_MAX_IDX ; test_index++)
        {
            if( apollo_raw_l34_portIntfIdx_set(port,test_index) != RT_ERR_OK)
            {
                osal_printf("\n port:%d index:%d\n",port,test_index);
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l34_portIntfIdx_get(port,&get_index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(test_index != get_index)
            {
                osal_printf("\n test_index %d get_index  %d\n",test_index,get_index);

                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    for(port = 0; port <= APOLLO_EXTPORTNOMAX ; port++)
    {
        for(test_index=0; test_index<L34_NETIF_TABLE_MAX_IDX ; test_index++)
        {
            if( apollo_raw_l34_extPortIntfIdx_set(port,test_index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l34_extPortIntfIdx_get(port,&get_index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(test_index != get_index)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    for(port = 0; port < APOLLO_VCPORTNO ; port++)
    {
        for(test_index=0; test_index<L34_NETIF_TABLE_MAX_IDX ; test_index++)
        {
            if( apollo_raw_l34_vcPortIntfIdx_set(port,test_index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l34_vcPortIntfIdx_get(port,&get_index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(test_index != get_index)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_l34_LanIntfMDBC_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_debugMonitor_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_natAttack2CPU_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_wanRouteAct_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_mode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_TtlOperationMode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_L3chksumErrAllow_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_L4chksumErrAllow_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_FragPkt2Cpu_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l34_portIntfIdx_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_extPortIntfIdx_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l34_vcPortIntfIdx_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


