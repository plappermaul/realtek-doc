/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Test Rate API
 *
 * Feature : RateAPI test
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/rate.h>
#include <dal/dal_rate_test_case.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>


#if 1
#ifdef CONFIG_SDK_APOLLO
    #include <hal/chipdef/apollo/apollo_def.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
    #include <hal/chipdef/apollomp/apollomp_def.h>
#endif

#ifdef CONFIG_SDK_RTL9601B
    #include <hal/chipdef/rtl9601b/rtl9601b_def.h>
#endif
#ifdef CONFIG_SDK_RTL9602C
    #include <hal/chipdef/rtl9602c/rtl9602c_def.h>
#endif
#ifdef CONFIG_SDK_RTL9607C
#include <hal/chipdef/rtl9607c/rtl9607c_def.h>
#endif

#ifdef CONFIG_SDK_RTL9603CVD
#include <hal/chipdef/rtl9603cvd/rtl9603cvd_def.h>
#endif



#endif


#define RATE_RANDOM_RUN_TIMES        80


/*
 * Function Declaration
 */


/*
 * Function Declaration
 */

int32 dal_rate_portIgrBandwidthCtrlRate_test(uint32 testcase)
{
    int32  ret;
    uint32 getRate,setRate;
    uint32 port;
    uint32 maxRate;
    int32 randIdx;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            maxRate = APOLLO_RATE_MAX;
            break;
#endif

#ifdef CONFIG_SDK_APOLLOMP            
        case APOLLOMP_CHIP_ID:
            maxRate = APOLLOMP_RATE_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9601B            
        case RTL9601B_CHIP_ID:
            maxRate = RTL9601B_RATE_MAX;
            break;
#endif

            
#ifdef CONFIG_SDK_RTL9602C            
        case RTL9602C_CHIP_ID:
            maxRate = RTL9602C_INGRESS_RATE_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C            
        case RTL9607C_CHIP_ID:
            maxRate = RTL9607C_INGRESS_RATE_MAX;
            break;
#endif


#ifdef CONFIG_SDK_RTL9603CVD            
        case RTL9603CVD_CHIP_ID:
            maxRate = RTL9603CVD_INGRESS_RATE_MAX;
            break;
#endif
        default:
            maxRate = (0x7FFFF * 8);
            break;
    }

    port = HAL_GET_MAX_PORT();
    setRate =maxRate;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_portIgrBandwidthCtrlRate_get(port, &getRate))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_portIgrBandwidthCtrlRate_set(port, setRate)))
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);  
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setRate =maxRate;
    if( rtk_rate_portIgrBandwidthCtrlRate_set(port, setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_portIgrBandwidthCtrlRate_get(port, &getRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*rate out of range*/
    port = HAL_GET_MAX_PORT();
    setRate =maxRate+1;
    if( rtk_rate_portIgrBandwidthCtrlRate_set(port, setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_portIgrBandwidthCtrlRate_get(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (randIdx = 0; randIdx < RATE_RANDOM_RUN_TIMES; randIdx++)
        {
            setRate = ((ut_rand()%maxRate) & 0xFFFFFFF8);
            /*set*/
            if( rtk_rate_portIgrBandwidthCtrlRate_set(port, setRate) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( rtk_rate_portIgrBandwidthCtrlRate_get(port, &getRate) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /*compare*/
            if(setRate != getRate)
            {
            #if 1
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setRate);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getRate);
            #endif
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_rate_portIgrBandwidthCtrlIncludeIfg_test(uint32 testcase)
{
    uint32 getState,setState;
    uint32 port;


    port = HAL_GET_MAX_PORT();
    setState = ENABLED;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(port, &getState))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port, setState)))
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);  
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setState = ENABLED;
    if( rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port, setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(port, &getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    setState =RTK_ENABLE_END;
    if( rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port, setState ) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (setState = 0; setState < RTK_ENABLE_END; setState++)
        {
            /*set*/
            if( rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port, setState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(port, &getState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /*compare*/
            if(setState != getState)
            {
            #if 1
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setState);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getState);
            #endif
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_rate_portEgrBandwidthCtrlRate_test(uint32 testcase)
{
    int32  ret;
    uint32 getRate,setRate;
    uint32 port;
    uint32 maxRate;
    int32 randIdx;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            maxRate = APOLLO_RATE_MAX;
            break;
#endif

#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            maxRate = APOLLOMP_RATE_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            maxRate = RTL9601B_RATE_MAX;
            break;
#endif
            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            maxRate = RTL9602C_EGRESS_RATE_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            maxRate = RTL9607C_EGRESS_RATE_MAX;
            break;
#endif            

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            maxRate = RTL9603CVD_EGRESS_RATE_MAX;
            break;
#endif

        default:
            maxRate = 0x7FFFF * 8;
            break;
    }

    port = HAL_GET_MAX_PORT();
    setRate =maxRate;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_portEgrBandwidthCtrlRate_get(port, &getRate))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_portEgrBandwidthCtrlRate_set(port, setRate)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setRate =maxRate;
    if( rtk_rate_portEgrBandwidthCtrlRate_set(port, setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_portEgrBandwidthCtrlRate_get(port, &getRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*rate out of range*/
    port = HAL_GET_MAX_PORT();
    setRate =maxRate+1;
    if( rtk_rate_portEgrBandwidthCtrlRate_set(port, setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_portEgrBandwidthCtrlRate_get(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (randIdx = 0; randIdx < RATE_RANDOM_RUN_TIMES; randIdx++)
        {
            setRate = (ut_rand()%maxRate) & 0xFFFFFFF8;
            /*set*/
            if( rtk_rate_portEgrBandwidthCtrlRate_set(port, setRate) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( rtk_rate_portEgrBandwidthCtrlRate_get(port, &getRate) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /*compare*/
            if(setRate != getRate)
            {
            #if 1
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setRate);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getRate);
            #endif
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_rate_egrBandwidthCtrlIncludeIfg_test(uint32 testcase)
{
    int32  ret;
    uint32 getState,setState;


    setState = ENABLED;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_egrBandwidthCtrlIncludeIfg_get(&getState))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_egrBandwidthCtrlIncludeIfg_set(setState)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    setState = RTK_ENABLE_END;
    if( (ret = rtk_rate_egrBandwidthCtrlIncludeIfg_set(setState)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d ret:%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_egrBandwidthCtrlIncludeIfg_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get/set test*/
    for (setState = 0; setState < RTK_ENABLE_END; setState++)
    {
        /*set*/
        if( rtk_rate_egrBandwidthCtrlIncludeIfg_set(setState) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        /*get*/
        if( rtk_rate_egrBandwidthCtrlIncludeIfg_get(&getState) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        /*compare*/
        if(setState != getState)
        {
        #if 1
            osal_printf("\n[write]\n");
            osal_printf("\n %d\n",setState);

            osal_printf("\n[read]\n");
            osal_printf("\n %d\n",getState);
        #endif
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

            return RT_ERR_FAILED;
        }
    }


    return RT_ERR_OK;
}

int32 dal_rate_egrQueueBwCtrlEnable_test(uint32 testcase)
{
    int32  ret;
    uint32 port;
    uint32 queue;
    uint32 getState,setState;

    port = HAL_GET_MAX_PORT();
    queue = RTK_MAX_NUM_OF_QUEUE-1;
    setState= ENABLED;

    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_egrQueueBwCtrlEnable_set(port,queue,setState))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_egrQueueBwCtrlEnable_get(port,queue,&getState)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    queue = RTK_MAX_NUM_OF_QUEUE-1;
    setState= ENABLED;
    if( rtk_rate_egrQueueBwCtrlEnable_set(port,queue,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_egrQueueBwCtrlEnable_get(port,queue,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*queue out of range*/
    port = HAL_GET_MAX_PORT();
    queue = RTK_MAX_NUM_OF_QUEUE;
    setState= ENABLED;
    if( rtk_rate_egrQueueBwCtrlEnable_set(port,queue,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_egrQueueBwCtrlEnable_get(port,queue,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    queue = RTK_MAX_NUM_OF_QUEUE-1;
    setState= RTK_ENABLE_END;
    if( rtk_rate_egrQueueBwCtrlEnable_set(port,queue,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_egrQueueBwCtrlEnable_get(port,queue,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_GET_PON_PORT()==port)
            continue;

        for (queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
        {
            for (setState = 0; setState < RTK_ENABLE_END; setState++)
            {
                /*set*/
                if( rtk_rate_egrQueueBwCtrlEnable_set(port,queue,setState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d port:%d setState:%d\n",__FUNCTION__,__LINE__,port,setState);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( rtk_rate_egrQueueBwCtrlEnable_get(port,queue,&getState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                /*compare*/
                if(setState != getState)
                {
                #if 1
                    osal_printf("\n port:%d queue:%d\n",port,queue);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setState);

                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getState);

                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_rate_egrQueueBwCtrlMeterIdx_test(uint32 testcase)
{
    int32  ret;
    uint32 port;
    uint32 queue;
    uint32 getIdx,setIdx;

    port = HAL_GET_MAX_PORT();
    queue = RTK_MAX_NUM_OF_QUEUE-1;
    setIdx= HAL_MAX_NUM_OF_METERING()-1;

    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_rate_egrQueueBwCtrlMeterIdx_set(port,queue,setIdx))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&getIdx)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    queue = RTK_MAX_NUM_OF_QUEUE-1;
    setIdx= HAL_MAX_NUM_OF_METERING()-1;
    if( rtk_rate_egrQueueBwCtrlMeterIdx_set(port,queue,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&getIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*queue out of range*/
    port = HAL_GET_MAX_PORT();
    queue = RTK_MAX_NUM_OF_QUEUE;
    setIdx= HAL_MAX_NUM_OF_METERING()-1;
    if( rtk_rate_egrQueueBwCtrlMeterIdx_set(port,queue,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&getIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*meter index out of range*/
    port = HAL_GET_MAX_PORT();
    queue = RTK_MAX_NUM_OF_QUEUE-1;
    setIdx= HAL_MAX_NUM_OF_METERING();
    if( rtk_rate_egrQueueBwCtrlMeterIdx_set(port,queue,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_GET_PON_PORT()==port)
            continue;
        for (queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
        {
            uint32 meterBase;
            
            switch(UNITTEST_UTIL_CHIP_TYPE)
            {
                case APOLLO_CHIP_ID:
                    meterBase = (port%4)*8;
                    break;
                case APOLLOMP_CHIP_ID:
                    meterBase = (port%4)*8;
                    break;
                case RTL9601B_CHIP_ID:
                    meterBase = 0;
                    break;
                case RTL9602C_CHIP_ID:
                    meterBase = (port%2)*8;
                    break;
                case RTL9607C_CHIP_ID:
                    meterBase = (port%6)*8;
                    break;
                case RTL9603CVD_CHIP_ID:
                    meterBase = (port%6)*8;
                    break;
                default:
                    meterBase = 0;
                    break;
            }
            for (setIdx = meterBase; setIdx < RTK_MAX_NUM_OF_QUEUE; setIdx++)
            {
                /*set*/
                if( rtk_rate_egrQueueBwCtrlMeterIdx_set(port,queue,setIdx) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d port:%d setIdx:%d\n",__FUNCTION__,__LINE__,port,setIdx);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&getIdx) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                /*compare*/
                if(setIdx != getIdx)
                {
                #if 1
                    osal_printf("\n port:%d queue:%d\n",port,queue);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setIdx);

                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getIdx);

                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}


static int32 _dal_rate_stormEnbleEntry_set(rtk_rate_storm_group_t group, rtk_rate_storm_group_ctrl_t  *stormCtrl)
{
    int32  ret;


    switch(group)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            stormCtrl->unknown_unicast_enable = ENABLED;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            stormCtrl->unknown_multicast_enable = ENABLED;
            break;
        case STORM_GROUP_MULTICAST:
            stormCtrl->multicast_enable = ENABLED;
            break;
        case STORM_GROUP_BROADCAST:
            stormCtrl->broadcast_enable = ENABLED;
            break;
        case STORM_GROUP_DHCP:
            stormCtrl->dhcp_enable = ENABLED;
            break;
        case STORM_GROUP_ARP:
            stormCtrl->arp_enable = ENABLED;
            break;
        case STORM_GROUP_IGMP_MLD:
            stormCtrl->igmp_mld_enable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_rate_stormControlMeterIdx_test(uint32 testcase)
{
    int32  ret;
    rtk_rate_storm_group_ctrl_t  setStormEanble;
    uint32 getIdx,setIdx;
    uint32 port;
    rtk_rate_storm_group_t group;

    memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
    setStormEanble.unknown_unicast_enable = ENABLED;

    if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
    {
        if( rtk_rate_stormControlEnable_set(&setStormEanble) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_UNKNOWN_UNICAST;
    setIdx = HAL_MAX_NUM_OF_METERING()-1;
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_rate_stormControlMeterIdx_set(port,group,setIdx))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_rate_stormControlMeterIdx_get(port,group,&getIdx)))
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    group = STORM_GROUP_UNKNOWN_UNICAST;
    setIdx = HAL_MAX_NUM_OF_METERING()-1;
    if( rtk_rate_stormControlMeterIdx_set(port,group,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_stormControlMeterIdx_get(port,group,&getIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*group out of range*/
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_END;
    setIdx = HAL_MAX_NUM_OF_METERING()-1;
    if( rtk_rate_stormControlMeterIdx_set(port,group,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_stormControlMeterIdx_get(port,group,&getIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*index out of range*/
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_UNKNOWN_UNICAST;
    setIdx = HAL_MAX_NUM_OF_METERING();
    if( rtk_rate_stormControlMeterIdx_set(port,group,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
    {
        /*type global config not enable*/
        memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
        if(_dal_rate_stormEnbleEntry_set(STORM_GROUP_UNKNOWN_UNICAST,&setStormEanble)!=RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( (ret=rtk_rate_stormControlEnable_set(&setStormEanble)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d ret:%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        port = HAL_GET_MAX_PORT();
        group = STORM_GROUP_DHCP;
        setIdx = HAL_MAX_NUM_OF_METERING()-1;
        if( rtk_rate_stormControlMeterIdx_set(port,group,setIdx) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*2. null pointer*/
    memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
    setStormEanble.unknown_unicast_enable = ENABLED;


    if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
    {
        if( rtk_rate_stormControlEnable_set(&setStormEanble) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_UNKNOWN_UNICAST;
    if( rtk_rate_stormControlMeterIdx_get(port,group,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (group = 0; group < STORM_GROUP_END; group++)
        {
            memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));

#ifdef CONFIG_SDK_RTL9601B    
            if(group == STORM_GROUP_DHCP || group == STORM_GROUP_ARP || group == STORM_GROUP_IGMP_MLD)
                continue;
#endif            


            if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
            {
                if(_dal_rate_stormEnbleEntry_set(group,&setStormEanble)!=RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if( rtk_rate_stormControlEnable_set(&setStormEanble) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
            for (setIdx = 0; setIdx < HAL_MAX_NUM_OF_METERING(); setIdx++)
            {
                /*set*/
                if( (ret=rtk_rate_stormControlMeterIdx_set(port,group,setIdx)) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d port:%d setIdx:%d group:%d ret:%d\n",__FUNCTION__,__LINE__,port,setIdx,group,ret);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( rtk_rate_stormControlMeterIdx_get(port,group,&getIdx) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                /*compare*/
                if(setIdx != getIdx)
                {
                #if 1
                    osal_printf("\n port:%d group:%d\n",port,group);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setIdx);

                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getIdx);

                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_rate_stormControlPortEnable_test(uint32 testcase)
{
    int32  ret;
    rtk_rate_storm_group_ctrl_t  setStormEanble;
    uint32 getState,setState;
    uint32 port;
    rtk_rate_storm_group_t group;

    memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
    setStormEanble.unknown_unicast_enable = ENABLED;
    if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
    {
        if( rtk_rate_stormControlEnable_set(&setStormEanble) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_UNKNOWN_UNICAST;
    setState = ENABLED;
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_rate_stormControlPortEnable_set(port,group,setState))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_rate_stormControlPortEnable_get(port,group,&getState)))
    {
         osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    group = STORM_GROUP_UNKNOWN_UNICAST;
    setState = ENABLED;
    if( rtk_rate_stormControlPortEnable_set(port,group,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_stormControlPortEnable_get(port,group,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*group out of range*/
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_END;
    setState = ENABLED;
    if( rtk_rate_stormControlPortEnable_set(port,group,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_stormControlPortEnable_get(port,group,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_UNKNOWN_UNICAST;
    setState = RTK_ENABLE_END;
    if( rtk_rate_stormControlPortEnable_set(port,group,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        /*type global config not enable*/

    if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
    {
        memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
        if(_dal_rate_stormEnbleEntry_set(STORM_GROUP_UNKNOWN_UNICAST,&setStormEanble)!=RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( (ret=rtk_rate_stormControlEnable_set(&setStormEanble)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d ret:%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        port = HAL_GET_MAX_PORT();
        group = STORM_GROUP_DHCP;
        setState = RTK_ENABLE_END;
        if( rtk_rate_stormControlPortEnable_set(port,group,setState) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*2. null pointer*/
    if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
    {
        memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
        setStormEanble.unknown_unicast_enable = ENABLED;
    
        if( rtk_rate_stormControlEnable_set(&setStormEanble) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    port = HAL_GET_MAX_PORT();
    group = STORM_GROUP_UNKNOWN_UNICAST;
    if( rtk_rate_stormControlPortEnable_get(port,group,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (group = 0; group < STORM_GROUP_END; group++)
        {
#ifdef CONFIG_SDK_RTL9601B
            if(RTL9601B_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)    
            {
                if(group == STORM_GROUP_DHCP || group == STORM_GROUP_ARP || group == STORM_GROUP_IGMP_MLD)
                    continue;
            }
#endif            

            if(APOLLOMP_CHIP_ID==UNITTEST_UTIL_CHIP_TYPE)
            {
                memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
                if(_dal_rate_stormEnbleEntry_set(group,&setStormEanble)!=RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if( rtk_rate_stormControlEnable_set(&setStormEanble) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
            for (setState = 0; setState < RTK_ENABLE_END; setState++)
            {
                /*set*/
                if( (ret=rtk_rate_stormControlPortEnable_set(port,group,setState)) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d port:%d setState:%d group:%d ret:%d\n",__FUNCTION__,__LINE__,port,setState,group,ret);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( rtk_rate_stormControlPortEnable_get(port,group,&getState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                /*compare*/
                if(setState != getState)
                {
                #if 1
                    osal_printf("\n port:%d group:%d\n",port,group);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setState);

                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getState);

                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_rate_stormControlEnable_test(uint32 testcase)
{
    int32  ret;
    rtk_rate_storm_group_ctrl_t  getStormEanble;
    rtk_rate_storm_group_ctrl_t  setStormEanble;
    int32 randIdx;
    rtk_rate_storm_group_t group;
    int32 testNum;

    memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
    
    
    ret = rtk_rate_stormControlEnable_set(&setStormEanble);
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_rate_stormControlEnable_set(&setStormEanble))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_rate_stormControlEnable_get(&getStormEanble)))
    {
        return RT_ERR_OK;
    }
    osal_printf("\n %s %d ret:%d\n",__FUNCTION__,__LINE__,ret);
    /*error input check*/
    /*1. out of range*/
        /*enable state out of range*/
    setStormEanble.unknown_unicast_enable = RTK_ENABLE_END;
    if( rtk_rate_stormControlEnable_set(&setStormEanble) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*more than 4 types enabled*/
    getStormEanble.unknown_unicast_enable = ENABLED;
    getStormEanble.unknown_multicast_enable = ENABLED;
    getStormEanble.multicast_enable = ENABLED;
    getStormEanble.broadcast_enable = ENABLED;
    getStormEanble.dhcp_enable = ENABLED;
    getStormEanble.arp_enable = DISABLED;
    getStormEanble.igmp_mld_enable = DISABLED;
    if( rtk_rate_stormControlEnable_set(&setStormEanble) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_stormControlEnable_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_stormControlEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set/get test*/
    for (testNum = 0 ; testNum < 100 ; testNum++)
    {
        memset(&setStormEanble,0x0,sizeof(rtk_rate_storm_group_ctrl_t));
        for (randIdx = 0; randIdx < 4; randIdx++)
        {
            group = ut_rand()%STORM_GROUP_END;
#ifdef CONFIG_SDK_RTL9601B    
            if(group == STORM_GROUP_DHCP || group == STORM_GROUP_ARP || group == STORM_GROUP_IGMP_MLD)
                continue;
#endif            
            if(_dal_rate_stormEnbleEntry_set(group,&setStormEanble)!=RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*set*/
            if( (ret = rtk_rate_stormControlEnable_set(&setStormEanble)) != RT_ERR_OK)
            {
                osal_printf("\n testNum:%d\n",testNum);
                osal_printf("\n %s %d ret:%d\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }

            /*set*/
            if( rtk_rate_stormControlEnable_get(&getStormEanble) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /*compare*/
            if(memcmp(&getStormEanble,&setStormEanble,sizeof(rtk_rate_storm_group_ctrl_t))!=0)
            {
            #if 1
                osal_printf("\n[write]\n");
                osal_printf("\n unknown_unicast_enable:%d\n"  ,setStormEanble.unknown_unicast_enable);
                osal_printf("\n unknown_multicast_enable:%d\n",setStormEanble.unknown_multicast_enable);
                osal_printf("\n multicast_enable:%d\n"        ,setStormEanble.multicast_enable);
                osal_printf("\n broadcast_enable:%d\n"        ,setStormEanble.broadcast_enable);
                osal_printf("\n dhcp_enable:%d\n"             ,setStormEanble.dhcp_enable);
                osal_printf("\n arp_enable:%d\n"              ,setStormEanble.arp_enable);
                osal_printf("\n igmp_mld_enable:%d\n"         ,setStormEanble.igmp_mld_enable);



                osal_printf("\n[read]\n");
                osal_printf("\n unknown_unicast_enable:%d\n"  ,getStormEanble.unknown_unicast_enable);
                osal_printf("\n unknown_multicast_enable:%d\n",getStormEanble.unknown_multicast_enable);
                osal_printf("\n multicast_enable:%d\n"        ,getStormEanble.multicast_enable);
                osal_printf("\n broadcast_enable:%d\n"        ,getStormEanble.broadcast_enable);
                osal_printf("\n dhcp_enable:%d\n"             ,getStormEanble.dhcp_enable);
                osal_printf("\n arp_enable:%d\n"              ,getStormEanble.arp_enable);
                osal_printf("\n igmp_mld_enable:%d\n"         ,getStormEanble.igmp_mld_enable);
            #endif
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                return RT_ERR_FAILED;
            }


        }
    }
    return RT_ERR_OK;
}


int32 dal_rate_stormBypass_test(uint32 testcase)
{
    int32  ret;
    uint32 testType;
    uint32 testIndex;
    uint32 getState,setState;
    uint32 testTypeArray[]={
                            BYPASS_BRG_GROUP                      ,
                            BYPASS_FD_PAUSE                       ,
                            BYPASS_SP_MCAST                       ,
                            BYPASS_1X_PAE                         ,
                            BYPASS_UNDEF_BRG_04                   ,
                            BYPASS_UNDEF_BRG_05                   ,
                            BYPASS_UNDEF_BRG_06                   ,
                            BYPASS_UNDEF_BRG_07                   ,
                            BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS  ,
                            BYPASS_UNDEF_BRG_09                   ,
                            BYPASS_UNDEF_BRG_0A                   ,
                            BYPASS_UNDEF_BRG_0B                   ,
                            BYPASS_UNDEF_BRG_0C                   ,
                            BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS   ,
                            BYPASS_8021AB                         ,
                            BYPASS_UNDEF_BRG_0F                   ,
                            BYPASS_BRG_MNGEMENT                   ,
                            BYPASS_UNDEFINED_11                   ,
                            BYPASS_UNDEFINED_12                   ,
                            BYPASS_UNDEFINED_13                   ,
                            BYPASS_UNDEFINED_14                   ,
                            BYPASS_UNDEFINED_15                   ,
                            BYPASS_UNDEFINED_16                   ,
                            BYPASS_UNDEFINED_17                   ,
                            BYPASS_UNDEFINED_18                   ,
                            BYPASS_UNDEFINED_19                   ,
                            BYPASS_UNDEFINED_1A                   ,
                            BYPASS_UNDEFINED_1B                   ,
                            BYPASS_UNDEFINED_1C                   ,
                            BYPASS_UNDEFINED_1D                   ,
                            BYPASS_UNDEFINED_1E                   ,
                            BYPASS_UNDEFINED_1F                   ,
                            BYPASS_GMRP                           ,
                            BYPASS_GVRP                           ,
                            BYPASS_UNDEF_GARP_22                  ,
                            BYPASS_UNDEF_GARP_23                  ,
                            BYPASS_UNDEF_GARP_24                  ,
                            BYPASS_UNDEF_GARP_25                  ,
                            BYPASS_UNDEF_GARP_26                  ,
                            BYPASS_UNDEF_GARP_27                  ,
                            BYPASS_UNDEF_GARP_28                  ,
                            BYPASS_UNDEF_GARP_29                  ,
                            BYPASS_UNDEF_GARP_2A                  ,
                            BYPASS_UNDEF_GARP_2B                  ,
                            BYPASS_UNDEF_GARP_2C                  ,
                            BYPASS_UNDEF_GARP_2D                  ,
                            BYPASS_UNDEF_GARP_2E                  ,
                            BYPASS_UNDEF_GARP_2F                  ,
                            BYPASS_CSSTP                          ,
                            BYPASS_CDP                            ,
                            BYPASS_IGMP                           ,
                            BYPASS_END};


    testType = BYPASS_BRG_GROUP;
    setState = ENABLED;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_stormBypass_get(testType, &getState))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_stormBypass_set(testType, setState)))
    {
        return RT_ERR_OK;
    }


    /*error input check*/
    /*1. out of range*/
        /*type out of range*/
    testType = BYPASS_END;
    setState= ENABLED;
    if( rtk_rate_stormBypass_set(testType, setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_stormBypass_get(testType, &getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*state out of range*/
    testType = BYPASS_BRG_GROUP;
    setState= RTK_ENABLE_END;
    if( rtk_rate_stormBypass_set(testType, setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_stormBypass_get(testType, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    for (testIndex = 0 ; ; testIndex++)
    {
        testType = testTypeArray[testIndex];
        if(BYPASS_END == testType)
            break;
        for (setState = 0; setState < RTK_ENABLE_END; setState++)
        {
            /*set*/
            if( (ret=rtk_rate_stormBypass_set(testType, setState)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d setState:%d testType:%d ret:%d\n",__FUNCTION__,__LINE__,setState,testType,ret);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( rtk_rate_stormBypass_get(testType, &getState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /*compare*/
            if(setState != getState)
            {
            #if 1
                osal_printf("\n testType:%d\n",testType);
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setState);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getState);

            #endif
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                return RT_ERR_FAILED;
            }

        }
    }
    /*set/get test*/


    return RT_ERR_OK;
}


int32 dal_rate_shareMeter_test(uint32 testcase)
{
    int32  ret;
    uint32 getRate,setRate;
    uint32 getifgState,setifgState;
    uint32 index;
    uint32 maxRate;
    int32 randIdx;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            maxRate = APOLLO_RATE_MAX;
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            maxRate = APOLLOMP_RATE_MAX;
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            maxRate = RTL9602C_METER_RATE_MAX;
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            maxRate = RTL9601B_RATE_MAX;
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
			maxRate = RTL9607C_METER_RATE_MAX;
			break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
		case RTL9603CVD_CHIP_ID:
			maxRate = RTL9603CVD_METER_RATE_MAX;
			break;
#endif
        default:
            maxRate = 0x1FFFF*8;
            break;
    }

    index = HAL_MAX_NUM_OF_METERING()-1;
    setRate =maxRate;
    setifgState = ENABLED;
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_rate_shareMeter_get(index, &getRate,&getifgState))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_rate_shareMeter_set(index, setRate,setifgState)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*index out of range*/
    index = HAL_MAX_NUM_OF_METERING();
    setRate =maxRate;
    setifgState = ENABLED;
    if( rtk_rate_shareMeter_set(index, setRate,setifgState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_shareMeter_get(index, &getRate,&getifgState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*rate out of range*/
    index = HAL_MAX_NUM_OF_METERING()-1;
    setRate =maxRate+(1<<3);
    setifgState = ENABLED;
    if( rtk_rate_shareMeter_set(index, setRate,setifgState) == RT_ERR_OK)
    {
        osal_printf(
			"\n %s Line:%d: MeterIdx: %d Rate: %d IFG: %d\n",
			__FUNCTION__,
			__LINE__,
			index,
			setRate,
			setifgState);
        return RT_ERR_FAILED;
    }

        /*ifg state out of range*/
    index = HAL_MAX_NUM_OF_METERING()-1;
    setRate =maxRate;
    setifgState = RTK_ENABLE_END;
    if( rtk_rate_shareMeter_set(index, setRate,setifgState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_shareMeter_get(index, NULL,&getifgState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_shareMeter_get(index, &getRate,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get/set test*/
    for (index = 0; index < HAL_MAX_NUM_OF_METERING(); index++)
    {
        for (setifgState = 0; setifgState < RTK_ENABLE_END; setifgState++)
        {
            for (randIdx = 0; randIdx < RATE_RANDOM_RUN_TIMES; randIdx++)
            {
                setRate = (ut_rand()%maxRate) & 0xFFFFFFF8;
                /*set*/
                if( rtk_rate_shareMeter_set(index, setRate,setifgState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( rtk_rate_shareMeter_get(index, &getRate,&getifgState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                /*compare*/
                if((setRate>>3) != (getRate>>3))
                {
                #if 1
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setRate);

                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getRate);
                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_rate_shareMeterBucket_test(uint32 testcase)
{
    int32  ret;
    uint32 getRate,setRate;
    uint32 index;
    uint32 maxRate;
    int32 randIdx;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            maxRate = APOLLO_METER_BUCKETSIZE_MAX;
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            maxRate = APOLLOMP_METER_BUCKETSIZE_MAX;
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
			maxRate = RTL9607C_METER_BUCKETSIZE_MAX;
			break;
#endif
        default:
            maxRate = 0XFFFF;
            break;
    }

    index = HAL_MAX_NUM_OF_METERING()-1;
    setRate =maxRate;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_shareMeterBucket_get(index, &getRate))
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_rate_shareMeterBucket_set(index, setRate)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*index out of range*/
    index = HAL_MAX_NUM_OF_METERING();
    setRate =maxRate;
    if( rtk_rate_shareMeterBucket_set(index, setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_rate_shareMeterBucket_get(index, &getRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        /*rate out of range*/
    index = HAL_MAX_NUM_OF_METERING()-1;
    setRate =maxRate+1;
    if( rtk_rate_shareMeterBucket_set(index, setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_rate_shareMeterBucket_get(index, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get/set test*/
    for (index = 0; index < HAL_MAX_NUM_OF_METERING(); index++)
    {
        for (randIdx = 0; randIdx < RATE_RANDOM_RUN_TIMES; randIdx++)
        {
            setRate = ut_rand()%maxRate;
            /*set*/
            if((ret = rtk_rate_shareMeterBucket_set(index, setRate)) != RT_ERR_OK) {
                osal_printf(
					"\n %s L%d R0x%X: MeterIdx %d Rate 0x%X\n",
					__FUNCTION__,
					__LINE__,
					ret,
					index,
					setRate);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( rtk_rate_shareMeterBucket_get(index, &getRate) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /*compare*/
            if(setRate != getRate)
            {
            #if 1
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setRate);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getRate);
            #endif
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_rate_shareMeterExceed_test(uint32 testcase)
{
    int32  ret;

    return RT_ERR_OK;
}
