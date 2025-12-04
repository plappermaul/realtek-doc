/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Test QoS API
 *
 * Feature : QoS API test 
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/qos.h>
#include <dal/dal_qos_test_case.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>

/*
 * Function Declaration
 */


/*
 * Function Declaration
 */

int32 dal_qos_priSelGroup_test(uint32 caseNo)
{  
    int32  ret;
    rtk_qos_priSelWeight_t   wWeight;
    rtk_qos_priSelWeight_t   rWeight;
    uint32 weightVal,priSelGrpIdx;
    
    memset(&wWeight,0x0,sizeof(rtk_qos_priSelWeight_t));
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_priSelGroup_set(0, &wWeight)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_priSelGroup_get(0, &rWeight)))
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group out of range*/
    memset(&wWeight,0x0,sizeof(rtk_qos_priSelWeight_t));
    if( rtk_qos_priSelGroup_set((HAL_PRI_SEL_GROUP_INDEX_MAX()+1), &wWeight) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*weight out of range*/
    wWeight.weight_of_portBased  = weightVal; 
    wWeight.weight_of_dot1q      = (HAL_PRI_SEL_WEIGHT_MAX()+1);
    wWeight.weight_of_dscp       = (HAL_PRI_SEL_WEIGHT_MAX()+1); 
    wWeight.weight_of_acl        = (HAL_PRI_SEL_WEIGHT_MAX()+1);
    wWeight.weight_of_lutFwd     = (HAL_PRI_SEL_WEIGHT_MAX()+1); 
    wWeight.weight_of_saBaed     = (HAL_PRI_SEL_WEIGHT_MAX()+1);
    wWeight.weight_of_vlanBased  = (HAL_PRI_SEL_WEIGHT_MAX()+1);
    wWeight.weight_of_svlanBased = (HAL_PRI_SEL_WEIGHT_MAX()+1);
    wWeight.weight_of_l4Based    = (HAL_PRI_SEL_WEIGHT_MAX()+1);
    
    if( rtk_qos_priSelGroup_set(0, &wWeight) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

     
    /*2. null pointer*/
    if( rtk_qos_priSelGroup_set(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_qos_priSelGroup_set(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    /* get/set test*/
    for (priSelGrpIdx = 0; priSelGrpIdx < HAL_PRI_SEL_GROUP_INDEX_MAX(); priSelGrpIdx++)
    {
        for (weightVal = 0; weightVal <= HAL_PRI_SEL_WEIGHT_MAX(); weightVal++)
        {
            wWeight.weight_of_portBased  = weightVal; 
            wWeight.weight_of_dot1q      = (weightVal+1)%HAL_PRI_SEL_WEIGHT_MAX();
            wWeight.weight_of_dscp       = (weightVal+2)%HAL_PRI_SEL_WEIGHT_MAX(); 
            wWeight.weight_of_acl        = (weightVal+3)%HAL_PRI_SEL_WEIGHT_MAX();
            wWeight.weight_of_svlanBased = (weightVal+7)%HAL_PRI_SEL_WEIGHT_MAX(); 

            switch(UNITTEST_UTIL_CHIP_TYPE)
            {
                case RTL9602C_CHIP_ID:    
                    wWeight.weight_of_lutFwd     = 0; 
                    wWeight.weight_of_saBaed     = 0; 
                    wWeight.weight_of_vlanBased  = 0; 
                    wWeight.weight_of_l4Based    = (weightVal+8)%HAL_PRI_SEL_WEIGHT_MAX();  
                    break;
                case APOLLOMP_CHIP_ID:    
                    wWeight.weight_of_lutFwd     = (weightVal+4)%HAL_PRI_SEL_WEIGHT_MAX(); 
                    wWeight.weight_of_saBaed     = (weightVal+5)%HAL_PRI_SEL_WEIGHT_MAX(); 
                    wWeight.weight_of_vlanBased  = (weightVal+6)%HAL_PRI_SEL_WEIGHT_MAX(); 
                    wWeight.weight_of_l4Based    = (weightVal+8)%HAL_PRI_SEL_WEIGHT_MAX(); 
                    break;                
                default:
                    wWeight.weight_of_lutFwd     = 0; 
                    wWeight.weight_of_saBaed     = 0; 
                    wWeight.weight_of_vlanBased  = 0; 
                    wWeight.weight_of_l4Based    = 0; 
                    break;
                
            }

            /*set*/
            if( rtk_qos_priSelGroup_set(priSelGrpIdx, &wWeight) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_priSelGroup_get(priSelGrpIdx, &rWeight) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
    
            /*compare*/
            if(priSelGrpIdx == 0)
            {
                if(memcmp(&wWeight,&rWeight,sizeof(rtk_qos_priSelWeight_t))!=0)
                {
                #if 1    
                    osal_printf("\n[write]\n");
                    osal_printf("\nport   : %d\n",wWeight.weight_of_portBased);
                    osal_printf("\ndot1q  : %d\n",wWeight.weight_of_dot1q);
                    osal_printf("\ndscp   : %d\n",wWeight.weight_of_dscp);
                    osal_printf("\nacl    : %d\n",wWeight.weight_of_acl);
                    osal_printf("\nlutFwd : %d\n",wWeight.weight_of_lutFwd);
                    osal_printf("\nsaBaed : %d\n",wWeight.weight_of_saBaed);
                    osal_printf("\nvlan   : %d\n",wWeight.weight_of_vlanBased);
                    osal_printf("\nsvlan  : %d\n",wWeight.weight_of_svlanBased);
                    osal_printf("\nl4     : %d\n",wWeight.weight_of_l4Based);
    
                    osal_printf("\n[read]\n");
                    osal_printf("\nport   : %d\n",rWeight.weight_of_portBased);
                    osal_printf("\ndot1q  : %d\n",rWeight.weight_of_dot1q);
                    osal_printf("\ndscp   : %d\n",rWeight.weight_of_dscp);
                    osal_printf("\nacl    : %d\n",rWeight.weight_of_acl);
                    osal_printf("\nlutFwd : %d\n",rWeight.weight_of_lutFwd);
                    osal_printf("\nsaBaed : %d\n",rWeight.weight_of_saBaed);
                    osal_printf("\nvlan   : %d\n",rWeight.weight_of_vlanBased);
                    osal_printf("\nsvlan  : %d\n",rWeight.weight_of_svlanBased);
                    osal_printf("\nl4     : %d\n",rWeight.weight_of_l4Based);
                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            } 
            else
            {
                if(wWeight.weight_of_dscp != rWeight.weight_of_dscp)
                {
                    osal_printf("\n[write]\n");
                    osal_printf("\ndscp   : %d\n",wWeight.weight_of_dscp);
                    
                    osal_printf("\n[read]\n");
                    osal_printf("\ndscp   : %d\n",rWeight.weight_of_dscp);

                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__); 
                    return RT_ERR_FAILED;   
                }    
            }
        }
    }
        
    return RT_ERR_OK;        
}



int32 dal_qos_portPri_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port,getPri,setPri;
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_portPri_get(0, &getPri)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_portPri_set(0, 1)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    setPri = 0;
    if( rtk_qos_portPri_set((HAL_GET_MAX_PORT()+1), setPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_portPri_get((HAL_GET_MAX_PORT()+1), &getPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
        /*priority out of range*/
    setPri = HAL_INTERNAL_PRIORITY_MAX()+1;
    if(rtk_qos_portPri_set(HAL_GET_MAX_PORT(), setPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_portPri_get(HAL_GET_MAX_PORT(), NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (setPri = 0; setPri <= HAL_INTERNAL_PRIORITY_MAX(); setPri++)
        {
            /*set*/
            if( rtk_qos_portPri_set(port, setPri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_portPri_get(port, &getPri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(setPri != getPri)
            {
            #if 1    
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setPri);

                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getPri);
            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            } 
        }
    }
        
    return RT_ERR_OK;        
}


int32 dal_qos_dscpPriRemapGroup_test(uint32 caseNo)
{  
    int32  ret;
    uint32 dscp,setIntPri,getIntPri;
    uint32 groupIdx,dp,getDp;
    
    
    groupIdx= HAL_PRIORITY_REMAP_GROUP_IDX_MAX();
    dscp = 0;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = 0;
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscpPriRemapGroup_set(groupIdx,dscp,setIntPri,dp)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscpPriRemapGroup_get(groupIdx,dscp,&getIntPri,&dp)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group index out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX()+1; 
    dscp = 0;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_dscpPriRemapGroup_set(groupIdx,dscp,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_dscpPriRemapGroup_get(groupIdx,dscp,&getIntPri,&dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


        /*dscp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    dscp = RTK_VALUE_OF_DSCP_MAX+1;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_dscpPriRemapGroup_set(groupIdx,dscp,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_dscpPriRemapGroup_get(groupIdx,dscp,&getIntPri,&dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*internal priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    dscp = RTK_VALUE_OF_DSCP_MAX;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX()+1;
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_dscpPriRemapGroup_set(groupIdx,dscp,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
        /*dp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    dscp = RTK_VALUE_OF_DSCP_MAX;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX()+1;
    if( rtk_qos_dscpPriRemapGroup_set(groupIdx,dscp,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_dscpPriRemapGroup_get(groupIdx,dscp,NULL,&dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    if( rtk_qos_dscpPriRemapGroup_get(groupIdx,dscp,&getIntPri,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    
    dp = 0;
    /* get/set test*/
    for (groupIdx = 0; groupIdx <= HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); groupIdx++)
    {
        for (dscp = 0; dscp <= RTK_VALUE_OF_DSCP_MAX; dscp++)
        {
            for (setIntPri = 0; setIntPri <= HAL_INTERNAL_PRIORITY_MAX(); setIntPri++)
            {
                /*set*/
                if( rtk_qos_dscpPriRemapGroup_set(groupIdx,dscp,setIntPri,dp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d groupIdx:%d dscp:%d setIntPri:%d dp:%d\n",__FUNCTION__,__LINE__,groupIdx,dscp,setIntPri,dp);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( rtk_qos_dscpPriRemapGroup_get(groupIdx,dscp,&getIntPri,&getDp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }

                /*compare*/
                if(setIntPri != getIntPri || dp != getDp)
                {
                #if 1    
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setIntPri);
                    osal_printf("\n %d\n",dp);
    
                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getIntPri);
                    osal_printf("\n %d\n",getDp);

                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            }            
        }
    }
        
    return RT_ERR_OK;        
}



int32 dal_qos_1pPriRemapGroup_test(uint32 caseNo)
{  
    int32  ret;
    uint32 dot1qPri,setIntPri,getIntPri;
    uint32 groupIdx,dp,getDp;
    
    
    groupIdx= HAL_PRIORITY_REMAP_GROUP_IDX_MAX();
    dot1qPri = (RTK_MAX_NUM_OF_PRIORITY -1);
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = 0;
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_1pPriRemapGroup_set(groupIdx,dot1qPri,setIntPri,dp)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_1pPriRemapGroup_get(groupIdx,dot1qPri,&getIntPri,&dp)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group index out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX()+1; 
    dot1qPri = 0;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_1pPriRemapGroup_set(groupIdx,dot1qPri,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_1pPriRemapGroup_get(groupIdx,dot1qPri,&getIntPri,&dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


        /*1p priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    dot1qPri = RTK_MAX_NUM_OF_PRIORITY;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_1pPriRemapGroup_set(groupIdx,dot1qPri,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_1pPriRemapGroup_get(groupIdx,dot1qPri,&getIntPri,&dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*internal priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    dot1qPri = 0;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX()+1;
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_1pPriRemapGroup_set(groupIdx,dot1qPri,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
        /*dp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    dot1qPri = 0;
    setIntPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX()+1;
    if( rtk_qos_1pPriRemapGroup_set(groupIdx,dot1qPri,setIntPri,dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_1pPriRemapGroup_get(groupIdx,dot1qPri,NULL,&dp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    if( rtk_qos_1pPriRemapGroup_get(groupIdx,dot1qPri,&getIntPri,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    
    dp = 0;
    /* get/set test*/
    for (groupIdx = 0; groupIdx <= HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); groupIdx++)
    {
        for (dot1qPri = 0; dot1qPri < RTK_MAX_NUM_OF_PRIORITY; dot1qPri++)
        {
            for (setIntPri = 0; setIntPri <= HAL_INTERNAL_PRIORITY_MAX(); setIntPri++)
            {
                /*set*/
                if( rtk_qos_1pPriRemapGroup_set(groupIdx,dot1qPri,setIntPri,dp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d groupIdx:%d dot1qPri:%d setIntPri:%d dp:%d\n",__FUNCTION__,__LINE__,groupIdx,dot1qPri,setIntPri,dp);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( rtk_qos_1pPriRemapGroup_get(groupIdx,dot1qPri,&getIntPri,&getDp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }

                /*compare*/
                if(setIntPri != getIntPri || dp != getDp)
                {
                #if 1    
                    osal_printf("\n groupIdx:%d dot1qPri:%d \n",groupIdx,dot1qPri);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setIntPri);
                    osal_printf("\n %d\n",dp);
    
                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getIntPri);
                    osal_printf("\n %d\n",getDp);

                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            }            
        }
    }
        
    return RT_ERR_OK;        
}




int32 dal_qos_priMap_test(uint32 caseNo)
{  
    int32  ret;
    rtk_qos_pri2queue_t setPri2qid;
    rtk_qos_pri2queue_t getPri2qid;
    uint32  groupIdx,intPri,queueMap;
    
    groupIdx = HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX();    
    memset(&setPri2qid,0x0,sizeof(rtk_qos_pri2queue_t));
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_priMap_set(groupIdx,&setPri2qid)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_priMap_get(groupIdx,&getPri2qid)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group index out of range*/
    groupIdx = HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX()+1;    
    memset(&setPri2qid,0x0,sizeof(rtk_qos_pri2queue_t));

    if( rtk_qos_priMap_set(groupIdx,&setPri2qid) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_priMap_get(groupIdx,&getPri2qid) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


        /*queue mapping out of range*/
    groupIdx = HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX(); 
    for(intPri = 0; intPri < RTK_MAX_NUM_OF_PRIORITY ; intPri++)
        setPri2qid.pri2queue[intPri] = HAL_MAX_NUM_OF_QUEUE();
    if( rtk_qos_priMap_set(groupIdx,&setPri2qid) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

     
    /*2. null pointer*/
    groupIdx = HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX(); 
    if( rtk_qos_priMap_set(groupIdx,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }  
    if( rtk_qos_priMap_get(groupIdx,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }  
    

    /* get/set test*/
    for (groupIdx = 0; groupIdx <= HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX(); groupIdx++)
    {
        for (queueMap = 0; queueMap < HAL_MAX_NUM_OF_QUEUE(); queueMap++)
        {
            for(intPri = 0; intPri < RTK_MAX_NUM_OF_PRIORITY ; intPri++)
                setPri2qid.pri2queue[intPri] = queueMap;

            /*set*/
            if( rtk_qos_priMap_set(groupIdx,&setPri2qid) != RT_ERR_OK)
            {
                osal_printf("\n %s %d groupIdx:%d \n",__FUNCTION__,__LINE__,groupIdx);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_priMap_set(groupIdx,&getPri2qid) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(memcmp(&getPri2qid,&getPri2qid,sizeof(&getPri2qid))!=0)
            {
                osal_printf("\n %s %d groupIdx:%d \n",__FUNCTION__,__LINE__,groupIdx);
                return RT_ERR_FAILED;
            }
        }
    }
        
    return RT_ERR_OK;        
}



int32 dal_qos_portPriMap_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port,getIdx,setIdx;
    
    port = HAL_GET_MAX_PORT();
    setIdx= HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX();  
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_portPriMap_set(port,setIdx)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_portPriMap_get(port,&getIdx)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setIdx= HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX();  
    if( rtk_qos_portPriMap_set(port,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_portPriMap_get(port,&getIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*group index out of range*/
    port = HAL_GET_MAX_PORT();
    setIdx= HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX()+1;  
    if( rtk_qos_portPriMap_set(port,setIdx) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_portPriMap_get(port,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   

    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (setIdx = 0; setIdx <= HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX(); setIdx++)
        {
            /*set*/
            if( rtk_qos_portPriMap_set(port,setIdx) != RT_ERR_OK)
            {
                osal_printf("\n %s %d port:%d setIdx:%d\n",__FUNCTION__,__LINE__,port,setIdx);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_portPriMap_get(port,&getIdx) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(setIdx != getIdx)
            {
            #if 1    
                osal_printf("\n port:%d\n",port);
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
        
    return RT_ERR_OK;        
}




int32 dal_qos_1pRemarkEnable_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port;
    uint32 getState,setState;
    
    port = HAL_GET_MAX_PORT();
    setState= ENABLED;  
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_1pRemarkEnable_set(port,setState)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_1pRemarkEnable_get(port,&getState)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setState= ENABLED;  
    if( rtk_qos_1pRemarkEnable_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_1pRemarkEnable_get(port,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    setState= RTK_ENABLE_END;  
    if( rtk_qos_1pRemarkEnable_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_1pRemarkEnable_get(port,NULL) == RT_ERR_OK)
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
            if( rtk_qos_1pRemarkEnable_set(port,setState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d port:%d setState:%d\n",__FUNCTION__,__LINE__,port,setState);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_1pRemarkEnable_get(port,&getState) != RT_ERR_OK)
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




int32 dal_qos_1pRemarkGroup_test(uint32 caseNo)
{  
    int32  ret;
    uint32 intPri,set1pPri,get1pPri;
    uint32 groupIdx,dp,getDp;
    
    
    groupIdx= HAL_PRIORITY_REMARK_GROUP_IDX_MAX();
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    set1pPri = (RTK_MAX_NUM_OF_PRIORITY -1);
    dp = 0;
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_1pRemarkGroup_set(groupIdx,intPri,dp,set1pPri)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_1pRemarkGroup_get(groupIdx,intPri,dp,&get1pPri)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group index out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX()+1; 
    set1pPri = (RTK_MAX_NUM_OF_PRIORITY -1);
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_1pRemarkGroup_set(groupIdx,intPri,dp,set1pPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_1pRemarkGroup_get(groupIdx,intPri,dp,&get1pPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*1p priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    set1pPri = RTK_MAX_NUM_OF_PRIORITY;
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_1pRemarkGroup_set(groupIdx,intPri,dp,set1pPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*internal priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    set1pPri = (RTK_MAX_NUM_OF_PRIORITY -1);
    intPri = HAL_INTERNAL_PRIORITY_MAX()+1;
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_1pRemarkGroup_set(groupIdx,intPri,dp,set1pPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_1pRemarkGroup_get(groupIdx,intPri,dp,&get1pPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
        /*dp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    set1pPri = RTK_MAX_NUM_OF_PRIORITY-1;
    dp = HAL_DROP_PRECEDENCE_MAX()+1;
    if( rtk_qos_1pRemarkGroup_set(groupIdx,intPri,dp,set1pPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_1pRemarkGroup_get(groupIdx,intPri,dp,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    dp = 0;
    /* get/set test*/
    for (groupIdx = 0; groupIdx <= HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); groupIdx++)
    {
        for (intPri = 0; intPri <= HAL_INTERNAL_PRIORITY_MAX(); intPri++)
        {
            for (set1pPri = 0; set1pPri < RTK_MAX_NUM_OF_PRIORITY; set1pPri++)
            {
                /*set*/
                if( rtk_qos_1pRemarkGroup_set(groupIdx,intPri,dp,set1pPri) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d groupIdx:%d intPri:%d dp:%d dp:%d\n",__FUNCTION__,__LINE__,groupIdx,intPri,dp,set1pPri);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( rtk_qos_1pRemarkGroup_get(groupIdx,intPri,dp,&get1pPri) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }

                /*compare*/
                if(get1pPri != set1pPri)
                {
                #if 1    
                    osal_printf("\n groupIdx:%d intPri:%d\n",groupIdx,intPri);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",set1pPri);
    
                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",get1pPri);

                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            }            
        }
    }
        
    return RT_ERR_OK;        
}




int32 dal_qos_dscpRemarkEnable_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port;
    uint32 getState,setState;
    
    port = HAL_GET_MAX_PORT();
    setState= ENABLED;  
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscpRemarkEnable_set(port,setState)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscpRemarkEnable_get(port,&getState)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setState= ENABLED;  
    if( rtk_qos_dscpRemarkEnable_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_dscpRemarkEnable_get(port,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    setState= RTK_ENABLE_END;  
    if( rtk_qos_dscpRemarkEnable_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_dscpRemarkEnable_get(port,NULL) == RT_ERR_OK)
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
            if( rtk_qos_dscpRemarkEnable_set(port,setState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d port:%d setState:%d\n",__FUNCTION__,__LINE__,port,setState);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_dscpRemarkEnable_get(port,&getState) != RT_ERR_OK)
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


int32 dal_qos_dscpRemarkGroup_test(uint32 caseNo)
{  
    int32  ret;
    uint32 intPri,setDscp,getDscp;
    uint32 groupIdx,dp;
    
    
    groupIdx= HAL_PRIORITY_REMARK_GROUP_IDX_MAX();
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscpRemarkGroup_set(groupIdx,intPri,dp,setDscp)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscpRemarkGroup_get(groupIdx,intPri,dp,&getDscp)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group index out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX()+1; 
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_dscpRemarkGroup_set(groupIdx,intPri,dp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_dscpRemarkGroup_get(groupIdx,intPri,dp,&getDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*internal priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    intPri = HAL_INTERNAL_PRIORITY_MAX()+1;
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_dscpRemarkGroup_set(groupIdx,intPri,dp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*dscp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    setDscp = RTK_VALUE_OF_DSCP_MAX+1;
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    dp = HAL_DROP_PRECEDENCE_MAX();
    if( rtk_qos_dscpRemarkGroup_set(groupIdx,intPri,dp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
   
        /*dp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    intPri = HAL_INTERNAL_PRIORITY_MAX();
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    dp = HAL_DROP_PRECEDENCE_MAX()+1;
    if( rtk_qos_dscpRemarkGroup_set(groupIdx,intPri,dp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_dscpRemarkGroup_get(groupIdx,intPri,dp,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    dp = 0;
    /* get/set test*/
    for (groupIdx = 0; groupIdx <= HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); groupIdx++)
    {
        for (intPri = 0; intPri <= HAL_INTERNAL_PRIORITY_MAX(); intPri++)
        {
            for (setDscp = 0; setDscp <= RTK_VALUE_OF_DSCP_MAX; setDscp++)
            {
                /*set*/
                if( rtk_qos_dscpRemarkGroup_set(groupIdx,intPri,dp,setDscp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d groupIdx:%d intPri:%d dp:%d dp:%d\n",__FUNCTION__,__LINE__,groupIdx,intPri,dp,setDscp);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( rtk_qos_dscpRemarkGroup_get(groupIdx,intPri,dp,&getDscp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }

                /*compare*/
                if(getDscp != setDscp)
                {
                #if 1    
                    osal_printf("\n groupIdx:%d intPri:%d\n",groupIdx,intPri);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setDscp);
    
                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getDscp);

                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            }            
        }
    }
        
    return RT_ERR_OK;        
}





int32 dal_qos_portDscpRemarkSrcSel_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port;
    rtk_qos_dscpRmkSrc_t setState,getState;    
    
    port = HAL_GET_MAX_PORT();
    setState= DSCP_RMK_SRC_INT_PRI;  
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_portDscpRemarkSrcSel_set(port,setState)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_portDscpRemarkSrcSel_get(port,&getState)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    setState= ENABLED;  
    if( rtk_qos_portDscpRemarkSrcSel_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_portDscpRemarkSrcSel_get(port,&getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*state out of range*/
    port = HAL_GET_MAX_PORT();
    setState= DSCP_RMK_SRC_END;  
    if( rtk_qos_portDscpRemarkSrcSel_set(port,setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
     
    /*2. null pointer*/
    if( rtk_qos_portDscpRemarkSrcSel_get(port,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   

    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        for (setState = 0; setState < DSCP_RMK_SRC_END; setState++)
        {
            
            if(DSCP_RMK_SRC_USER_PRI==setState)
            {
                if(UNITTEST_UTIL_CHIP_TYPE != RTL9601B_CHIP_ID || UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)    
                    continue;
            }
            
            /*set*/
            if( rtk_qos_portDscpRemarkSrcSel_set(port,setState) != RT_ERR_OK)
            {
                osal_printf("\n %s %d port:%d setState:%d\n",__FUNCTION__,__LINE__,port,setState);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_portDscpRemarkSrcSel_get(port,&getState) != RT_ERR_OK)
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


int32 dal_qos_dscp2DscpRemarkGroup_test(uint32 caseNo)
{  
    int32  ret;
    uint32 oriDscp,setDscp,getDscp;
    uint32 groupIdx,dp;
    
    
    groupIdx= HAL_PRIORITY_REMARK_GROUP_IDX_MAX();
    oriDscp = RTK_VALUE_OF_DSCP_MAX;
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscp2DscpRemarkGroup_set(groupIdx,oriDscp,setDscp)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_dscp2DscpRemarkGroup_get(groupIdx,oriDscp,&getDscp)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*group index out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX()+1; 
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    oriDscp = RTK_VALUE_OF_DSCP_MAX;
    if( rtk_qos_dscp2DscpRemarkGroup_set(groupIdx,oriDscp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_dscp2DscpRemarkGroup_get(groupIdx,oriDscp,&getDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*original dscp priority out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    oriDscp = RTK_VALUE_OF_DSCP_MAX+1;
    setDscp = RTK_VALUE_OF_DSCP_MAX;
    if( rtk_qos_dscp2DscpRemarkGroup_set(groupIdx,oriDscp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*dscp out of range*/
    groupIdx = HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); 
    setDscp = RTK_VALUE_OF_DSCP_MAX+1;
    oriDscp = RTK_VALUE_OF_DSCP_MAX;
    if( rtk_qos_dscp2DscpRemarkGroup_set(groupIdx,oriDscp,setDscp) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
   
     
    /*2. null pointer*/
    if( rtk_qos_dscp2DscpRemarkGroup_get(groupIdx,oriDscp,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    dp = 0;
    /* get/set test*/
    for (groupIdx = 0; groupIdx <= HAL_PRIORITY_REMAP_GROUP_IDX_MAX(); groupIdx++)
    {
        for (oriDscp = 0; oriDscp <= RTK_VALUE_OF_DSCP_MAX; oriDscp++)
        {
            for (setDscp = 0; setDscp <= RTK_VALUE_OF_DSCP_MAX; setDscp++)
            {
                /*set*/
                if( rtk_qos_dscp2DscpRemarkGroup_set(groupIdx,oriDscp,setDscp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d groupIdx:%d oriDscp:%d \n",__FUNCTION__,__LINE__,groupIdx,oriDscp);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( rtk_qos_dscp2DscpRemarkGroup_get(groupIdx,oriDscp,&getDscp) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }

                /*compare*/
                if(getDscp != setDscp)
                {
                #if 1    
                    osal_printf("\n groupIdx:%d oriDscp:%d\n",groupIdx,oriDscp);
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setDscp);
    
                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getDscp);

                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            }            
        }
    }
        
    return RT_ERR_OK;        
}




int32 dal_qos_fwd2CpuPriRemap_test(uint32 caseNo)
{  
    int32  ret;
    uint32 oriPri,setFwdPri,getFwdPri;
    
    oriPri = HAL_INTERNAL_PRIORITY_MAX();
    setFwdPri = (RTK_MAX_NUM_OF_PRIORITY -1);
    
    /*skip not support feature*/
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_fwd2CpuPriRemap_set(oriPri,setFwdPri)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_fwd2CpuPriRemap_get(oriPri,&getFwdPri)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/

        /*original priority priority out of range*/
    oriPri = HAL_INTERNAL_PRIORITY_MAX()+1;
    setFwdPri = (RTK_MAX_NUM_OF_PRIORITY -1);
    if( rtk_qos_fwd2CpuPriRemap_set(oriPri,setFwdPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_fwd2CpuPriRemap_get(oriPri,&getFwdPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*forward priority out of range*/
    oriPri = HAL_INTERNAL_PRIORITY_MAX();
    setFwdPri = (RTK_MAX_NUM_OF_PRIORITY );
    if( rtk_qos_fwd2CpuPriRemap_set(oriPri,setFwdPri) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
   
     
    /*2. null pointer*/
    if(  rtk_qos_fwd2CpuPriRemap_get(oriPri,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }   
    /* get/set test*/
    for (oriPri = 0; oriPri <= HAL_INTERNAL_PRIORITY_MAX(); oriPri++)
    {
        for (setFwdPri = 0; setFwdPri < RTK_MAX_NUM_OF_PRIORITY; setFwdPri++)
        {
            /*set*/
            if( rtk_qos_fwd2CpuPriRemap_set(oriPri,setFwdPri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d oriPri:%d \n",__FUNCTION__,__LINE__,oriPri);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if(  rtk_qos_fwd2CpuPriRemap_get(oriPri,&getFwdPri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(getFwdPri != setFwdPri)
            {
            #if 1    
                osal_printf("\noriPri:%d\n",oriPri);
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setFwdPri);
    
                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getFwdPri);

            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            }
        }            
    }    
    return RT_ERR_OK;        
}



int32 dal_qos_schedulingQueue_test(uint32 caseNo)
{  
    int32  ret;
    uint32 port;
    uint32 queue,index;
    rtk_qos_queue_weights_t  getWeight,setWeight;
    
    /*skip not support feature*/
    port = HAL_GET_MAX_PORT();
    memset(&setWeight,0x0,sizeof(rtk_qos_queue_weights_t));
    if ((RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_schedulingQueue_get(port, &setWeight)) 
        || (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_qos_schedulingQueue_get(port, &getWeight)))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
        /*port out of range*/
    port = HAL_GET_MAX_PORT()+1;
    memset(&setWeight,0x0,sizeof(rtk_qos_queue_weights_t));
    if( rtk_qos_schedulingQueue_set(port, &setWeight) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_qos_schedulingQueue_get(port, &getWeight) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
        /*weight out of range*/
        /*Queue 0 outof range*/
    port = HAL_GET_MAX_PORT()+1;
    memset(&setWeight,0x0,sizeof(rtk_qos_queue_weights_t));
    setWeight.weights[0] = HAL_QUEUE0_WEIGHT_MAX()+1;
    if(rtk_qos_schedulingQueue_set(port, &setWeight) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*Queue 1~6 outof range*/
    port = HAL_GET_MAX_PORT()+1;
    memset(&setWeight,0x0,sizeof(rtk_qos_queue_weights_t));
    setWeight.weights[1] = HAL_QUEUE_WEIGHT_MAX()+1;
    if(rtk_qos_schedulingQueue_set(port, &setWeight) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }     
    /*2. null pointer*/
    if( rtk_qos_schedulingQueue_set(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_qos_schedulingQueue_get(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    /* get/set test*/
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_GET_PON_PORT()==port)
            continue;
        for (index = 1; index <= 64; index++)
        {
            for (queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
            {
                if(queue==0)
                    setWeight.weights[queue] = (index * 1024)-1;     
                else
                    setWeight.weights[queue] = (index * 2)-1;     
            }
            /*set*/
            if( rtk_qos_schedulingQueue_set(port, &setWeight) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_qos_schedulingQueue_get(port, &getWeight) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            /*compare*/
            if(memcmp(&setWeight,&getWeight,sizeof(&getWeight))!=0)
            {
            #if 1    
                osal_printf("\n[write] port:%d\n",port);
                for (queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
                    osal_printf("\n [%d]:%d\n",queue,setWeight.weights[queue]);

                osal_printf("\n[read]\n");
                for (queue = 0; queue < RTK_MAX_NUM_OF_QUEUE; queue++)
                    osal_printf("\n [%d]:%d\n",queue,getWeight.weights[queue]);
            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            } 
        }
    }
        
    return RT_ERR_OK;        
}
