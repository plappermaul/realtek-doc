/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Test LED API
 *
 * Feature : LED API test 
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/led.h>
#include <dal/dal_led_test_case.h>
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


/*
 * Function Declaration
 */


/*
 * Function Declaration
 */

int32 dal_led_operation_test(uint32 testcase)
{  
    int32  ret;
    rtk_led_operation_t   getMode,setMode;
    
    rtk_led_init();
        
    setMode = LED_OP_SERIAL;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_led_operation_set(setMode)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_led_operation_get(&getMode)))
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*1. mode of range*/
        /*port out of range*/
    setMode = LED_OP_END;
    if( rtk_led_operation_set(setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*2. null pointer*/
    if( rtk_led_operation_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    for (setMode = 0; setMode < RTK_ENABLE_END; setMode++)
    {
        /*set*/
        if( rtk_led_operation_set(setMode) != RT_ERR_OK)
        {
            osal_printf("\n %s %d setMode:%d\n",__FUNCTION__,__LINE__,setMode);
            return RT_ERR_FAILED;       
        }
        /*get*/
        if( rtk_led_operation_get(&getMode) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        /*compare*/
        if(getMode != setMode)
        {
        #if 1    
            osal_printf("\n[write]\n");
            osal_printf("\n %d\n",getMode);

            osal_printf("\n[read]\n");
            osal_printf("\n %d\n",getMode);

        #endif                  
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            
            return RT_ERR_FAILED;
        }
    }    
             
    return RT_ERR_OK;        
}

int32 dal_led_serialMode_test(uint32 testcase)
{  
    int32  ret;
    rtk_led_active_t   getMode,setMode;

    rtk_led_init();
    
    setMode = LED_OP_SERIAL;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_led_serialMode_set(setMode)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_led_serialMode_get(&getMode)))
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*1. mode of range*/
        /*port out of range*/
    setMode = LED_ACTIVE_END;
    if( rtk_led_serialMode_set(setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*2. null pointer*/
    if( rtk_led_serialMode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*get set test*/
    for (setMode = 0; setMode < LED_ACTIVE_END; setMode++)
    {
        /*set*/
        if( rtk_led_serialMode_set(setMode) != RT_ERR_OK)
        {
            osal_printf("\n %s %d setMode:%d\n",__FUNCTION__,__LINE__,setMode);
            return RT_ERR_FAILED;       
        }
        /*get*/
        if( rtk_led_serialMode_get(&getMode) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        /*compare*/
        if(getMode != setMode)
        {
        #if 1    
            osal_printf("\n[write]\n");
            osal_printf("\n %d\n",setMode);

            osal_printf("\n[read]\n");
            osal_printf("\n %d\n",getMode);

        #endif                  
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            
            return RT_ERR_FAILED;
        }
    }    
             
    return RT_ERR_OK;     
}

int32 dal_led_blinkRate_test(uint32 testcase)
{  
    int32  ret;
    rtk_led_blinkGroup_t group;
    rtk_led_blink_rate_t setRate,getRate;

    rtk_led_init();

    
    group = LED_BLINK_GROUP_PORT;
    setRate = LED_BLINKRATE_32MS;
    
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_led_blinkRate_set(group,setRate)) 
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_led_blinkRate_get(group,&getRate)))
    {
        return RT_ERR_OK;
    }    

    /*error input check*/
    /*1. out of range*/
        /*group out of range*/
    group = LED_BLINK_GROUP_END;
    setRate = LED_BLINKRATE_32MS;
    if( rtk_led_blinkRate_set(group,setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_led_blinkRate_get(group,&getRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*rate out of range*/
    group = LED_BLINK_GROUP_PORT;
    setRate = LED_BLINKRATE_END;
    if( rtk_led_blinkRate_set(group,setRate) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*2. null pointer*/
    if( rtk_led_blinkRate_get(group,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*get set test*/
    for (group = 0; group < LED_BLINK_GROUP_END; group++)
    {
        if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID && UNITTEST_UTIL_CHIP_TYPE != RTL9601B_CHIP_ID && UNITTEST_UTIL_CHIP_TYPE != RTL9602C_CHIP_ID)
        {
            continue;
        }

        for (setRate = 0; setRate < LED_BLINKRATE_END; setRate++)
        {
            /*set*/
            if( rtk_led_blinkRate_set(group,setRate) != RT_ERR_OK)
            {
                osal_printf("\n %s %d setRate:%d\n",__FUNCTION__,__LINE__,setRate);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_led_blinkRate_get(group,&getRate) != RT_ERR_OK)
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

int32 dal_led_config_test(uint32 testcase)
{  
    int32  ret;
    rtk_led_config_t    getMode,setMode;
    uint32  ledIdx,testCfgIdx;
    uint32  maxLedIdx;
    rtk_led_type_t  ledType,ledGetType;
    uint32 *testCfg;
    uint32 portLedItem[]={LED_CONFIG_FORCE_MODE,
                         LED_CONFIG_SPD1000,
                         LED_CONFIG_SPD100,
                         LED_CONFIG_SPD10,
                         LED_CONFIG_DUP,
                         LED_CONFIG_SPD1000ACT,
                         LED_CONFIG_SPD100ACT,
                         LED_CONFIG_SPD10ACT,
                         LED_CONFIG_RX_ACT,
                         LED_CONFIG_TX_ACT,
                         LED_CONFIG_COL,
                         LED_CONFIG_END};
    uint32 ponLedItem[]={LED_CONFIG_PON_LINK,
                         LED_CONFIG_END};
    uint32 socLedItem[]={LED_CONFIG_SOC_LINK_ACK,
                         LED_CONFIG_END};


    rtk_led_init();


        
    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            maxLedIdx = APOLLO_LED_INDEX_MAX;
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            maxLedIdx = APOLLOMP_LED_INDEX_MAX;
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            maxLedIdx = RTL9601B_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            maxLedIdx = RTL9602C_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            maxLedIdx = RTL9607C_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            maxLedIdx = RTL9603CVD_LED_INDEX_MAX;
            break;
#endif

        default:
            maxLedIdx = 31;
            break;
    }
    
    ledIdx = 0;
    ledType = LED_TYPE_UTP0;
    memset(&setMode,0x0,sizeof(rtk_led_config_t));
    /*skip not support feature*/
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_led_config_set(ledIdx,ledType,&setMode)) 
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_led_config_get(ledIdx,&ledGetType,&getMode)))
    {
        return RT_ERR_OK;
    }    
    
    /*error input check*/
    /*1. out of range*/
        /*index out of range*/
    ledIdx = maxLedIdx+1;
    ledType = LED_TYPE_UTP0;
    if( rtk_led_config_set(ledIdx,ledType,&setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_led_config_get(ledIdx,&ledGetType,&getMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*type out of range*/
    ledIdx = 0;
    ledType = LED_TYPE_END;
    if( rtk_led_config_set(ledIdx,ledType,&setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    
        /*config parameter*/
    setMode.ledEnable[LED_CONFIG_COL] = RTK_ENABLE_END;
    ledIdx = 0;
    ledType = LED_TYPE_UTP0;
    if( rtk_led_config_set(ledIdx,ledType,&setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*2. null pointer*/    
    if( rtk_led_config_set(ledIdx,ledType,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 
    if( rtk_led_config_get(ledIdx,&ledGetType,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 
    if( rtk_led_config_get(ledIdx,NULL,&getMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 

    /*get set test*/
    for (ledIdx = 0; ledIdx <= maxLedIdx; ledIdx++)
    {
        for (ledType = LED_TYPE_UTP0; ledType < LED_TYPE_END ; ledType++)
        {            
            switch(ledType)
            {
                case LED_TYPE_UTP0:
                case LED_TYPE_UTP1:
                case LED_TYPE_UTP2:
                case LED_TYPE_UTP3:
                case LED_TYPE_UTP4:
                case LED_TYPE_FIBER:
                    testCfg = portLedItem;
                    break;
                case LED_TYPE_USB0:
                case LED_TYPE_USB1:
                case LED_TYPE_SATA:
                    testCfg = socLedItem;
                    break;
                case LED_TYPE_PON:
                    testCfg = ponLedItem;
                    break;
                case LED_TYPE_UTP5:
                default:
                    continue;
                    break;
            }
            
            
            for(testCfgIdx=0 ; ;testCfgIdx++)
            {
                if(LED_CONFIG_END==testCfg[testCfgIdx])    
                    break;
                
                memset(&setMode,0x0,sizeof(rtk_led_config_t));
                memset(&getMode,0x0,sizeof(rtk_led_config_t));
    
                setMode.ledEnable[testCfg[testCfgIdx]] = ENABLED;
                
                if(LED_TYPE_FIBER == ledType )
                {
                    if(LED_CONFIG_SPD10ACT == testCfg[testCfgIdx] ||
                       LED_CONFIG_SPD10 == testCfg[testCfgIdx] ||
                       LED_CONFIG_COL == testCfg[testCfgIdx] ||
                       LED_CONFIG_DUP == testCfg[testCfgIdx])    
                        continue;
                }
                    
                /*set*/
                if( rtk_led_config_set(ledIdx,ledType,&setMode) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d ledType:%d testCfgIdx:%d\n",__FUNCTION__,__LINE__,ledType,testCfgIdx);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( rtk_led_config_get(ledIdx,&ledGetType,&getMode) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
        
                /*compare*/
                if(ledType != ledGetType || (memcmp(&setMode,&getMode,sizeof(rtk_led_config_t)!=0)))
                {
                #if 1    
                    osal_printf("\nledIdx: %d  testCfgIdx:%d",ledIdx,testCfgIdx);

                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",ledType);
        
                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",ledGetType);
        
                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }                
            }
                
        }    
    }
    
    
    return RT_ERR_OK;                       
}

int32 dal_led_modeForce_test(uint32 testcase)
{  
    int32  ret;
    rtk_led_force_mode_t    getMode,setMode;
    uint32  ledIdx;
    uint32  maxLedIdx;

    rtk_led_init();

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            maxLedIdx = APOLLO_LED_INDEX_MAX;
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            maxLedIdx = APOLLOMP_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            maxLedIdx = RTL9601B_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            maxLedIdx = RTL9602C_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            maxLedIdx = RTL9607C_LED_INDEX_MAX;
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            maxLedIdx = RTL9603CVD_LED_INDEX_MAX;
            break;
#endif

        default:
            maxLedIdx = 31;
            break;
    }


    
    setMode = LED_FORCE_BLINK;
    ledIdx = 0;
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_led_modeForce_set(ledIdx,setMode)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_led_modeForce_get(ledIdx,&getMode)))
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*1. out of range*/
        /*mode out of range*/
    setMode = LED_FORCE_END;
    if( rtk_led_modeForce_set(ledIdx,setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*index out of range*/
    setMode = LED_FORCE_BLINK;
    ledIdx = maxLedIdx+1;
    if( rtk_led_modeForce_set(ledIdx,setMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_led_modeForce_get(ledIdx,&getMode) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*2. null pointer*/
    if( rtk_led_modeForce_get(ledIdx,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*get set test*/
    for (ledIdx = 0; ledIdx <= maxLedIdx; ledIdx++)
    {
        for (setMode = 0; setMode < LED_FORCE_END; setMode++)
        {
            /*set*/
            if( rtk_led_modeForce_set(ledIdx,setMode) != RT_ERR_OK)
            {
                osal_printf("\n %s %d setMode:%d  ledIdx:%d\n",__FUNCTION__,__LINE__,setMode,ledIdx);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_led_modeForce_get(ledIdx,&getMode) != RT_ERR_OK)
            {
                osal_printf("\n %s %d ledIdx:%d\n",__FUNCTION__,__LINE__,ledIdx);
                return RT_ERR_FAILED;       
            }
    
            /*compare*/
            if(getMode != setMode)
            {
            #if 1    
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setMode);
    
                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getMode);
    
            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            }
        }    
    }
                 
    return RT_ERR_OK;        
}
