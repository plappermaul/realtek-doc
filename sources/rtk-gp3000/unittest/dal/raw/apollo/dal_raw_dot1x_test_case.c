#include <osal/lib.h>
#include <ioal/mem32.h>
#include <dal/apollo/raw/apollo_raw_dot1x.h>
#include <dal/raw/apollo/dal_raw_dot1x_test_case.h>


int32 dal_dot1x_raw_test(uint32 caseNo)
{  

    int32 ret;
    rtk_port_t port;
    rtk_enable_t enable;
    uint32 index;
    uint32 priority;
    uint32 rData;
    uint32 regName;
    uint32 regData;
    uint32 dataMask;
    uint32 checkData;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_dot1x_guestVidx_set(APOLLO_CVIDXNO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_dot1x_trapPri_set((APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_dot1x_guestVlanOpdir_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    if( apollo_raw_dot1x_macBaseVlanOpdir_set(RAW_DOT1X_DIRECT_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( apollo_raw_dot1x_macBasedEnable_set(APOLLO_PORTNO,DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_dot1x_macBasedEnable_set(0,RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_dot1x_portBasedEnable_set(APOLLO_PORTNO,DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_dot1x_portBasedEnable_set(0,RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_dot1x_portAuthState_set(APOLLO_PORTNO,RAW_DOT1X_AUTHORIZED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_dot1x_portAuthState_set(0,RAW_DOT1X_AUTH_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_dot1x_portBaseVlanOpdir_set(APOLLO_PORTNO,RAW_DOT1X_DIRECT_BOTH) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_dot1x_portBaseVlanOpdir_set(0,RAW_DOT1X_DIRECT_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_dot1x_unauthAct_set(APOLLO_PORTNO,RAW_DOT1X_UNAUTH_DROP) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_dot1x_unauthAct_set(0,RAW_DOT1X_UNAUTH_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        
    /* get/set test*/
    for(index = 0; index < APOLLO_PRI_TO_QUEUE_TBL_SIZE ; index++)
    {
        /*set*/
        if( apollo_raw_dot1x_guestVidx_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*get*/
        if( apollo_raw_dot1x_guestVidx_get(&rData) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*compare*/
        if(index != rData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }    

    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        /*set*/
        if( apollo_raw_dot1x_trapPri_set(priority) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*get*/
        if( apollo_raw_dot1x_trapPri_get(&rData) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*compare*/
        if(priority != rData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(enable = DISABLED; enable < RTK_ENABLE_END ; enable++)
    {
        /*set*/
        if( apollo_raw_dot1x_guestVlanOpdir_set(enable) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*get*/
        if( apollo_raw_dot1x_guestVlanOpdir_get(&rData) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*compare*/
        if(enable != rData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(index = RAW_DOT1X_DIRECT_BOTH; index < RAW_DOT1X_DIRECT_END ; index++)
    {
        /*set*/
        if( apollo_raw_dot1x_macBaseVlanOpdir_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*get*/
        if( apollo_raw_dot1x_macBaseVlanOpdir_get(&rData) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        /*compare*/
        if(index != rData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(enable = DISABLED; enable < RTK_ENABLE_END ; enable++)
        {
            /*set*/
            if( apollo_raw_dot1x_macBasedEnable_set(port,enable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_dot1x_macBasedEnable_get(port,&rData) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*compare*/
            if(enable != rData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }    
    }    
    

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(enable = DISABLED; enable < RTK_ENABLE_END ; enable++)
        {
            /*set*/
            if( apollo_raw_dot1x_portBasedEnable_set(port,enable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_dot1x_portBasedEnable_get(port,&rData) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*compare*/
            if(enable != rData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }    
    }


    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = RAW_DOT1X_AUTHORIZED; index < RAW_DOT1X_AUTH_END ; index++)
        {
            /*set*/
            if( apollo_raw_dot1x_portAuthState_set(port,index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_dot1x_portAuthState_get(port,&rData) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*compare*/
            if(index != rData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }    
    }

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = RAW_DOT1X_DIRECT_BOTH; index < RAW_DOT1X_DIRECT_END ; index++)
        {
            /*set*/
            if( apollo_raw_dot1x_portBaseVlanOpdir_set(port,index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_dot1x_portBaseVlanOpdir_get(port,&rData) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*compare*/
            if(index != rData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }    
    }
    

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = RAW_DOT1X_UNAUTH_DROP; index < RAW_DOT1X_UNAUTH_END ; index++)
        {
            /*set*/
            if( apollo_raw_dot1x_unauthAct_set(port,index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_dot1x_unauthAct_get(port,&rData) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*compare*/
            if(index != rData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }    
    }
    /*null pointer*/
    if(apollo_raw_dot1x_guestVidx_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(apollo_raw_dot1x_trapPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(apollo_raw_dot1x_guestVlanOpdir_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(apollo_raw_dot1x_macBaseVlanOpdir_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(apollo_raw_dot1x_macBasedEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(apollo_raw_dot1x_portBasedEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(apollo_raw_dot1x_portAuthState_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if(apollo_raw_dot1x_portBaseVlanOpdir_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if(apollo_raw_dot1x_unauthAct_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    /*set value checking with register address checking*/
    
    /*test for apollo_raw_dot1x_guestVidx_set*/
    /*test for apollo_raw_dot1x_guestVlanOpdir_set*/
    /*test for apollo_raw_dot1x_macBaseVlanOpdir_set*/
    /*clear register to 0*/
    regName = DOT1X_CFG_1r;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);
   
    /*set register by raw driver*/
    /*set*/
    apollo_raw_dot1x_guestVlanOpdir_set(ENABLED);
    apollo_raw_dot1x_macBaseVlanOpdir_set(RAW_DOT1X_DIRECT_IN);
    apollo_raw_dot1x_guestVidx_set(0x12);

    /*get register data*/
    dataMask = 0x7F;
    checkData =0x72;          
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }
    
    
    /*test for apollo_raw_dot1x_trapPri_set*/
    /*clear register to 0*/
    regName = DOT1X_CFG_0r;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);
   
    /*set register by raw driver*/
    /*set*/
    apollo_raw_dot1x_trapPri_set(7);

    /*get register data*/
    dataMask = 0x7;
    checkData =0x7;          
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }

    /*test for apollo_raw_dot1x_portBasedEnable_set*/
    /*clear register to 0*/
    regName = DOT1X_P_CTRLr;
    for(port = 0; port < APOLLO_PORTNO ; port++)
        ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4*port,0x0);
   
    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_dot1x_portBasedEnable_set(port,ENABLED);

    /*get register data*/
    dataMask = 0x20;
    checkData =0x20;          
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4*port,&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    } 

    /*test for apollo_raw_dot1x_macBasedEnable_get*/
    /*clear register to 0*/
    regName = DOT1X_P_CTRLr;
    for(port = 0; port < APOLLO_PORTNO ; port++)
        ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4*port,0x0);
   
    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_dot1x_macBasedEnable_set(port,ENABLED);

    /*get register data*/
    dataMask = 0x10;
    checkData =0x10;          
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4*port,&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }    
    
    /*test for apollo_raw_dot1x_portAuthState_set*/
    /*clear register to 0*/
    regName = DOT1X_P_CTRLr;
    for(port = 0; port < APOLLO_PORTNO ; port++)
        ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4*port,0x0);
   
    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_dot1x_portAuthState_set(port,RAW_DOT1X_UNAUTHORIZED);

    /*get register data*/
    dataMask = 0x8;
    checkData =0x8;          
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4*port,&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }    

    /*test for apollo_raw_dot1x_portBaseVlanOpdir_set*/
    /*clear register to 0*/
    regName = DOT1X_P_CTRLr;
    for(port = 0; port < APOLLO_PORTNO ; port++)
        ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4*port,0x0);
   
    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_dot1x_portBaseVlanOpdir_set(port,RAW_DOT1X_DIRECT_IN);

    /*get register data*/
    dataMask = 0x4;
    checkData =0x4;          
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4*port,&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }    

    /*test for apollo_raw_dot1x_unauthAct_set*/
    /*clear register to 0*/
    regName = DOT1X_P_CTRLr;
    for(port = 0; port < APOLLO_PORTNO ; port++)
        ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4*port,0x0);
   
    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_dot1x_unauthAct_set(port,RAW_DOT1X_UNAUTH_GVLAN);

    /*get register data*/
    dataMask = 0x3;
    checkData =0x2;          
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4*port,&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }    
    return RT_ERR_OK;
}    
