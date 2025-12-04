/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Test QoS API
 *
 * Feature : SEC API test 
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/sec.h>
#include <dal/dal_sec_test_case.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>


#if 1
#ifdef CONFIG_SDK_APOLLO 
    #include <hal/chipdef/apollo/apollo_def.h>
#endif
#ifdef CONFIG_SDK_APOLLOMP 
    #include <hal/chipdef/apollomp/apollomp_def.h>
#endif
#endif


#define SEC_RANDOM_RUN_TIMES        20

/*
 * Function Declaration
 */


/*
 * Function Declaration
 */


int32 dal_sec_portAttackPreventState_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port;
    uint32 getState,setState;
    
    port = HAL_GET_MAX_PORT();
    setState= ENABLED;  
    
    rtk_sec_init();
    
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_sec_portAttackPreventState_set(port,setState)) 
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_sec_portAttackPreventState_get(port,&getState)))
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setState= ENABLED;  
    if( rtk_sec_portAttackPreventState_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_sec_portAttackPreventState_get(port,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    setState= RTK_ENABLE_END;  
    if( rtk_sec_portAttackPreventState_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    /*2. null pointer*/
    if( rtk_sec_portAttackPreventState_get(port,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }      
    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (setState = 0; setState < RTK_ENABLE_END; setState++)
        {
            /*set*/
            if( rtk_sec_portAttackPreventState_set(port,setState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d port:%d setState:%d\n",__FUNCTION__,__LINE__,port,setState);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_sec_portAttackPreventState_get(port,&getState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(setState != getState)
            {
            #if 1    
                osal_printf("\n port:%d\n",port);
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

int32 dal_sec_attackPrevent_test(uint32 caseNo)
{  
    int32  ret;
    rtk_sec_attackType_t    attackType;
    rtk_action_t            setAction,getAction;
    rtk_action_t   actionList[] = {ACTION_TRAP2CPU,ACTION_DROP,ACTION_FORWARD,ACTION_END};         
    int32 actIdx;

    rtk_sec_init();
    
    attackType = DAEQSA_DENY;
    setAction = ACTION_TRAP2CPU;  
    
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_sec_attackPrevent_set(attackType,setAction)) 
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_sec_attackPrevent_get(attackType,&getAction)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*attack type out of range*/
    attackType = ATTACK_TYPE_END;
    setAction = ACTION_TRAP2CPU;  
    if( rtk_sec_attackPrevent_set(attackType,setAction) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_sec_attackPrevent_get(attackType,&getAction) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*action out of range*/
    attackType = DAEQSA_DENY;
    setAction = ACTION_END;  
    if( rtk_sec_attackPrevent_set(attackType,setAction) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }     
    /*2. null pointer*/
    if( rtk_sec_attackPrevent_get(attackType,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }     
    
    /* get/set test*/
    for (attackType = 0; attackType < ATTACK_TYPE_END; attackType++)
    {
        for (actIdx = 0;; actIdx++)
        {
            setAction = actionList[actIdx];
            if(ACTION_END==setAction)
                break;
            /*set*/
            if( rtk_sec_attackPrevent_set(attackType,setAction) != RT_ERR_OK)
            {
                osal_printf("\n %s %d attackType:%d setAction:%d\n",__FUNCTION__,__LINE__,attackType,setAction);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_sec_attackPrevent_get(attackType,&getAction) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(setAction != getAction)
            {
            #if 1    
                osal_printf("\n attackType:%d\n",attackType);
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setAction);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getAction);

            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            }
        }
    }    
    
    
    return RT_ERR_OK;        
}

int32 dal_sec_attackFloodThresh_test(uint32 caseNo)
{  
    int32  ret;
    uint32 getThresh,setThresh;
    rtk_sec_attackFloodType_t type;
    uint32 maxThresh;
    int32 randIdx;
 
    rtk_sec_init();
        
    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            maxThresh = APOLLO_SEC_FLOOD_THRESHOLD_MAX;
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            maxThresh = APOLLOMP_SEC_FLOOD_THRESHOLD_MAX;
            break;
#endif
        default:
            maxThresh = 0xFF;
            break;
    }
    
    
    type = SEC_ICMPFLOOD;
    setThresh = maxThresh;  
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_sec_attackFloodThresh_set(type,getThresh)) 
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_sec_attackFloodThresh_get(type,&getThresh)))
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*1. out of range*/
        /*type out of range*/
    type = SEC_FLOOD_END;
    setThresh = maxThresh; 
    if( rtk_sec_attackFloodThresh_set(type,setThresh) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_sec_attackFloodThresh_get(type,&getThresh) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
        /*threshold index out of range*/
    type = SEC_FLOOD_END;
    setThresh = maxThresh+1;  
    if( rtk_sec_attackFloodThresh_set(type,setThresh) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    for (type = 0; type < SEC_FLOOD_END; type++)
    {
        for (randIdx = 0; randIdx <= SEC_RANDOM_RUN_TIMES; randIdx++) 
        {
            setThresh = ut_rand()%maxThresh;
            /*set*/
            if( rtk_sec_attackFloodThresh_set(type,setThresh) != RT_ERR_OK)
            {
                osal_printf("\n %s %d type:%d setThresh:%d\n",__FUNCTION__,__LINE__,type,setThresh);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_sec_attackFloodThresh_get(type,&getThresh) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(setThresh != getThresh)
            {
            #if 1    
                osal_printf("\n type:%d\n",type);
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setThresh);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getThresh);

            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            }
        }
    }    
       
    return RT_ERR_OK;        
}
