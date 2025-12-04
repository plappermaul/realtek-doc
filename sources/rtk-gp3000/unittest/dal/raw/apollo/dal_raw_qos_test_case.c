#include <osal/lib.h>
#include <ioal/mem32.h>
#include <dal/apollo/raw/apollo_raw_qos.h>
#include <dal/raw/apollo/dal_raw_qos_test_case.h>


int32 dal_qos_raw_test(uint32 caseNo)
{  
    int32 ret;
    uint32 max_weight;
    rtk_port_t port;
    rtk_enable_t enable;
    uint32 index;
    uint32 qid;
    uint32 priority;
    uint32 rePri;
    uint32 dscp;
    uint32 newDscp;
    uint32 weight;
    apollo_raw_qos_priType_t type;  
    apollo_raw_dscp_source_t source;
    apollo_raw_queue_type_t queueType;
    apollo_raw_queue_type_t rQueueType;
    
    uint32 rQid;
    uint32 rPriority;
    uint32 rIndex;
    uint32 rWeight;
    uint32 rDscp;
    rtk_enable_t rEnable;
    apollo_raw_dscp_source_t rSource;
    uint32 indexOffset;
    uint32 regData;
    uint32 wData;
    uint32 dataMask;
    uint32 checkData;
    
        
    /*error input check*/
    /*out of range*/
    if( apollo_raw_qos_priToQidMappingTable_set(APOLLO_PRI_TO_QUEUE_TBL_SIZE, 0, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_priToQidMappingTable_set(0, (APOLLO_PRIMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_priToQidMappingTable_set(0, 0, APOLLO_QUEUENO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_qos_portQidMapIdx_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_portQidMapIdx_set(0, APOLLO_PRI_TO_QUEUE_TBL_SIZE) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_qos_priRemapToCpu_set((APOLLO_PRIMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_priRemapToCpu_set(0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 

    if( apollo_raw_qos_1QPriRemap_set((APOLLO_PRIMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_1QPriRemap_set(0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }  

    if( apollo_raw_qos_dscpPriRemap_set((APOLLO_DSCPMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_dscpPriRemap_set(0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_portBasePri_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_portBasePri_set(0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_priDecisionWeight_set(RAW_QOS_PRI_TYPE_END, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_priDecisionWeight_set(RAW_QOS_PRI_TYPE_SVLAN, (APOLLO_PRI_WEIGHT_MAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_1qRemarkAbility_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_1qRemarkAbility_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    if( apollo_raw_qos_1qRemarkPri_set((APOLLO_PRIMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_1qRemarkPri_set(0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_qos_dscpRemarkDscp_set((APOLLO_DSCPMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_dscpRemarkDscp_set(0, (APOLLO_DSCPMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_qos_interPriRemarkDscp_set((APOLLO_PRIMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_interPriRemarkDscp_set(0, (APOLLO_DSCPMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    if( apollo_raw_qos_dscpRemarkSource_set(APOLLO_PORTNO, RAW_QOS_DSCP_SOURCE_INTR_PRI) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_dscpRemarkSource_set(0, RAW_QOS_DSCP_SOURCE_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_wfqWeight_set(0, 0, 65536) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_wfqWeight_set(0, 1, 128) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_wfqWeight_set(APOLLO_PORTNO, 0, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( apollo_raw_qos_wfqWeight_set(0, APOLLO_QUEUENO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_qos_aprEnable_set(APOLLO_PORTNO, 0, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_aprEnable_set(0, APOLLO_QUEUENO, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_aprEnable_set(0, 0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_qos_aprMeter_set(APOLLO_PORTNO, 0, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_aprMeter_set(0, APOLLO_QUEUENO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_aprMeter_set(0, 0, 8) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_queueType_set(APOLLO_PORTNO, 0, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_queueType_set(0, APOLLO_QUEUENO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_queueType_set(0, 0, RAW_QOS_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_ethernetAvEnable_set(APOLLO_PORTNO, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_ethernetAvEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_ethernetAvRemapPri_set((APOLLO_PRIMAX+1), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_ethernetAvRemapPri_set(0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 
    
    /* get/set test*/
    for(index = 0; index < APOLLO_PRI_TO_QUEUE_TBL_SIZE ; index++)
    {
        for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
        {
            for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
            {
                /*set*/
                if( apollo_raw_qos_priToQidMappingTable_set(index, priority, qid) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( apollo_raw_qos_priToQidMappingTable_get(index, priority, &rQid) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*compare*/
                if(qid != rQid)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }                
            }
        }        
    }    

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = 0; index < APOLLO_PRI_TO_QUEUE_TBL_SIZE ; index++)
        {    
            /*set*/
            if( apollo_raw_qos_portQidMapIdx_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_portQidMapIdx_get(port, &rIndex) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }                
            /*compare*/
            if(index != rIndex)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            } 
        }
    }

    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        for(rePri = 0; rePri <= APOLLO_PRIMAX ; rePri++)
        {
            /*set*/
            if( apollo_raw_qos_priRemapToCpu_set(priority, rePri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_priRemapToCpu_get(priority, &rPriority) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rPriority != rePri)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }    
    
    for(dscp = 0; dscp <= APOLLO_DSCPMAX ; dscp++)
    {
        for(rePri = 0; rePri <= APOLLO_PRIMAX ; rePri++)
        {
            /*set*/
            if( apollo_raw_qos_dscpPriRemap_set(dscp, rePri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_dscpPriRemap_get(dscp, &rPriority) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rPriority != rePri)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }        


    
    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        for(rePri = 0; rePri <= APOLLO_PRIMAX ; rePri++)
        {
            /*set*/
            if( apollo_raw_qos_1QPriRemap_set(priority, rePri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_1QPriRemap_get(priority, &rPriority) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rPriority != rePri)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }


    for(type = RAW_QOS_PRI_TYPE_SVLAN; type < RAW_QOS_PRI_TYPE_END ; type++)
    {
        for(weight = 0; weight <= APOLLO_PRI_WEIGHT_MAX ; weight++)
        {
            /*set*/
            if( apollo_raw_qos_priDecisionWeight_set(type, weight) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_priDecisionWeight_get(type, &rWeight) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(weight != rWeight)
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
            if( apollo_raw_qos_1qRemarkAbility_set(port, enable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_1qRemarkAbility_get(port, &rEnable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }                
            /*compare*/
            if(enable != rEnable)
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
            if( apollo_raw_qos_dscpRemarkAbility_set(port, enable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_dscpRemarkAbility_get(port, &rEnable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }                
            /*compare*/
            if(enable != rEnable)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            } 
        }
    }    

    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        for(rePri = 0; rePri <= APOLLO_PRIMAX ; rePri++)
        {
            /*set*/
            if( apollo_raw_qos_1qRemarkPri_set(priority, rePri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_1qRemarkPri_get(priority, &rPriority) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rPriority != rePri)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }

    for(dscp = 0; dscp <= APOLLO_DSCPMAX ; dscp+=3)
    {
        for(newDscp = 0; newDscp <= APOLLO_DSCPMAX ; newDscp+=4)
        {
            /*set*/
            if( apollo_raw_qos_dscpRemarkDscp_set(dscp, newDscp) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_dscpRemarkDscp_get(dscp, &rDscp) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rDscp != newDscp)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }        

    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        for(newDscp = 0; newDscp <= APOLLO_DSCPMAX ; newDscp+=4)
        {
            /*set*/
            if( apollo_raw_qos_interPriRemarkDscp_set(priority, newDscp) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_interPriRemarkDscp_get(priority, &rDscp) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rDscp != newDscp)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }        

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(source = 0; source < RAW_QOS_DSCP_SOURCE_TYPE_END ; source++)
        {    
            /*set*/
            if( apollo_raw_qos_dscpRemarkSource_set(port, source) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_dscpRemarkSource_get(port, &rSource) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }                
            /*compare*/
            if(source != rSource)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            } 
        }
    }

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
        {
            if(qid == 0)
            {
                indexOffset = 1024;
                max_weight = 65535;
            }
            else
            {
                indexOffset = 16;
                max_weight = 127;
            }
            for(weight = 0; weight <= max_weight ; weight+=indexOffset)
            {
                /*set*/
                if( apollo_raw_qos_wfqWeight_set(port, qid, weight) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( apollo_raw_qos_wfqWeight_get(port, qid, &rWeight) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*compare*/
                if(weight != rWeight)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }                
            }
        }        
    }

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
        {
            for(enable = DISABLED ; enable < RTK_ENABLE_END ; enable++)
            {
                /*set*/
                if( apollo_raw_qos_aprEnable_set(port, qid, enable) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( apollo_raw_qos_aprEnable_get(port, qid, &rEnable) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*compare*/
                if(enable != rEnable)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }                
            }
        }        
    }

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
        {
            for(index = 0 ; index < 8 ; index++)
            {
                /*set*/
                if( apollo_raw_qos_aprMeter_set(port, qid, index) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( apollo_raw_qos_aprMeter_get(port, qid, &rIndex) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*compare*/
                if(index != rIndex)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }                
            }
        }        
    }


    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
        {
            for(queueType = 0 ; queueType < RAW_QOS_TYPE_END ; queueType++)
            {
                /*set*/
                if( apollo_raw_qos_queueType_set(port, qid, queueType) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*get*/
                if( apollo_raw_qos_queueType_get(port, qid, &rQueueType) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                /*compare*/
                if(queueType != rQueueType)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }                
            }
        }        
    }

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(enable = DISABLED; enable < RTK_ENABLE_END ; enable++)
        {    
            /*set*/
            if( apollo_raw_qos_ethernetAvEnable_set(port, enable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_ethernetAvEnable_get(port, &rEnable) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }                
            /*compare*/
            if(enable != rEnable)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            } 
        }
    }

    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        for(rePri = 0; rePri <= APOLLO_PRIMAX ; rePri++)
        {
            /*set*/
            if( apollo_raw_qos_ethernetAvRemapPri_set(priority, rePri) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( apollo_raw_qos_ethernetAvRemapPri_get(priority, &rPriority) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }            
            /*compare*/
            if(rPriority != rePri)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }                     
        }
    }

    
    /*null pointer*/
    if(apollo_raw_qos_priToQidMappingTable_get(0, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_portQidMapIdx_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_priRemapToCpu_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_1QPriRemap_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }       

    if(apollo_raw_qos_1QPriRemap_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }       

    if(apollo_raw_qos_portBasePri_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }  

    if(apollo_raw_qos_priDecisionWeight_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_1qRemarkAbility_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_dscpRemarkAbility_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(apollo_raw_qos_1qRemarkPri_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(apollo_raw_qos_dscpRemarkDscp_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    if(apollo_raw_qos_interPriRemarkDscp_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    if(apollo_raw_qos_dscpRemarkSource_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(apollo_raw_qos_wfqWeight_get(0, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_aprEnable_get(0, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(apollo_raw_qos_aprMeter_get(0, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(apollo_raw_qos_queueType_get(0, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    if(apollo_raw_qos_ethernetAvEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    if(apollo_raw_qos_ethernetAvRemapPri_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    


    /*register access test*/
    /*test for apollo_raw_qos_priToQidMappingTable_set*/
    /*clear register to 0*/
    for(index =0 ; index<= 3 ; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(QOS_INTPRI_TO_QIDr)+4*index,0x0);

    /*set register by raw driver*/
    for(index = 0; index < APOLLO_PRI_TO_QUEUE_TBL_SIZE ; index++)
    {
        for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
        {
            apollo_raw_qos_priToQidMappingTable_set(index,priority,priority);    
        }
    }
    /*get register */    
    for(index =0 ; index<= 3 ; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(QOS_INTPRI_TO_QIDr)+4*index,&regData);
        
        dataMask = 0x00FFFFFF;
        checkData =0xFAC688;
    
        /*mask out reserve bits*/
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;         
        }
    }                

    /*test for apollo_raw_qos_portQidMapIdx_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(QOS_PORT_QMAP_CTRLr),0x0);
    /*set register by raw driver*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        index = port%4;
        apollo_raw_qos_portQidMapIdx_set(port,index);    
    } 
    ioal_mem32_read(HAL_GET_REG_ADDR(QOS_PORT_QMAP_CTRLr),&regData);
        
    dataMask = 0x3FFF;
    checkData =0x24E4;    
    /*mask out reserve bits*/
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }    

    /*test for apollo_raw_qos_priRemapToCpu_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(QOS_PRI_REMAP_IN_CPUr),0x0);
    
    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        rePri = APOLLO_PRIMAX - priority;
        /*set*/
        apollo_raw_qos_priRemapToCpu_set(priority, rePri);
    }
    ioal_mem32_read(HAL_GET_REG_ADDR(QOS_PRI_REMAP_IN_CPUr),&regData);
        
    dataMask = 0xFFFFFF;
    checkData =0x53977;    
    /*mask out reserve bits*/
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }
       
    /*test for apollo_raw_qos_dscpPriRemap_set*/
    /*clear register to 0*/
    for(index =0 ; index<= 6 ; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(QOS_DSCP_REMAPr)+4*index,0x0);

    /*set register by raw driver*/
    /*
            word (1)
            dscp 0 --> 0
            dscp 1 --> 1
            dscp 2 --> 2
            dscp 3 --> 3
            dscp 4 --> 4
            dscp 5 --> 5
            dscp 6 --> 6
            dscp 7 --> 7
            dscp 8 --> 0
            dscp 9 --> 1

            word (2)
            dscp 10 --> 0
            dscp 11 --> 1
            dscp 12 --> 2
            dscp 13 --> 3
            dscp 14 --> 4
            dscp 15 --> 5
            dscp 16 --> 6
            dscp 17 --> 7
            dscp 18 --> 0
            dscp 19 --> 1
               ...  repeat ...
                                                    
            00 1000 1111 1010 1100 0110 1000 1000
                8    F     A   C    6    8    8 
    */
    for(dscp = 0; dscp <= APOLLO_DSCPMAX ; dscp++)
    {
        rePri = (dscp%10)%8; 
        apollo_raw_qos_dscpPriRemap_set(dscp,rePri);
    }
    /*get register */
    for(index =0 ; index<= 6 ; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(QOS_DSCP_REMAPr)+4*index,&regData);
        
        if(index == 6)
        {
            dataMask = 0xFFF;
            checkData = 0x688;
        }
        else
        {
            dataMask = 0x3FFFFFFF;
            checkData = 0x8FAC688;            
        }
    
        /*mask out reserve bits*/
        regData = regData & dataMask; 

        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;         
        }
    }

    /*test for apollo_raw_qos_1QPriRemap_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(QOS_1Q_PRI_REMAPr),0x0);
    
    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        rePri = APOLLO_PRIMAX - priority;
        /*set*/
        apollo_raw_qos_1QPriRemap_set(priority, rePri);
    }
    ioal_mem32_read(HAL_GET_REG_ADDR(QOS_1Q_PRI_REMAPr),&regData);
        
    dataMask = 0xFFFFFF;
    checkData =0x53977;    
    /*mask out reserve bits*/
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }
    
    /*test for apollo_raw_qos_priDecisionWeight_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(PRI_SEL_TBL_CTRLr),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(PRI_SEL_TBL_CTRL2r),0x0);
   
    for(type = RAW_QOS_PRI_TYPE_SVLAN,weight=1; type < RAW_QOS_PRI_TYPE_END ; type++,weight++)
        apollo_raw_qos_priDecisionWeight_set(type, weight);

    ioal_mem32_read(HAL_GET_REG_ADDR(PRI_SEL_TBL_CTRLr),&regData);
        
    dataMask = 0xFFFFFFFF;
    checkData =0x12345678;    
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    } 

    ioal_mem32_read(HAL_GET_REG_ADDR(PRI_SEL_TBL_CTRL2r),&regData);
        
    dataMask = 0xF;
    checkData =0x9;    
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }

    /*test for apollo_raw_qos_1qRemarkAbility_set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        /*clear register to 0*/
        ioal_mem32_write(HAL_GET_REG_ADDR(RMK_DOT1Q_RMK_EN_CTRLr)+HAL_GET_MACPP_INTERVAL()*port,0x0);
        /*set*/
        apollo_raw_qos_1qRemarkAbility_set(port, ENABLED);

        /*get register */
        ioal_mem32_read(HAL_GET_REG_ADDR(RMK_DOT1Q_RMK_EN_CTRLr)+HAL_GET_MACPP_INTERVAL()*port,&regData);
        dataMask = 0x1;
        checkData =0x1;    
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }        
    }

    /*test for apollo_raw_qos_dscpRemarkAbility_set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        /*clear register to 0*/
        ioal_mem32_write(HAL_GET_REG_ADDR(RMK_DSCP_RMK_EN_CTRLr)+4*port,0x0);
        /*set*/
        apollo_raw_qos_dscpRemarkAbility_set(port, ENABLED);

        /*get register */
        ioal_mem32_read(HAL_GET_REG_ADDR(RMK_DSCP_RMK_EN_CTRLr)+4*port,&regData);
        dataMask = 0x1;
        checkData =0x1;    
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x port:%d  addr:0x%x\n",
                __FUNCTION__,
                __LINE__,
                checkData,
                regData,
                port,HAL_GET_REG_ADDR(RMK_DSCP_RMK_EN_CTRLr)+HAL_GET_MACPP_INTERVAL()*port);
            return RT_ERR_FAILED;         
        }        
    }    
    
    /*test for apollo_raw_qos_1qRemarkPri_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(RMK_1Q_CTRLr),0x0);
    
    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        rePri = APOLLO_PRIMAX - priority;
        /*set*/
        apollo_raw_qos_1qRemarkPri_set(priority, rePri);
    }
    ioal_mem32_read(HAL_GET_REG_ADDR(RMK_1Q_CTRLr),&regData);
        
    dataMask = 0xFFFFFF;
    checkData =0x53977;    
    /*mask out reserve bits*/
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }    
    
    /*test for apollo_raw_qos_dscpRemarkDscp_set*/
    /*clear register to 0*/    
    for(index =0 ; index< 13 ; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(RMK_DSCP_CTRLr)+4*index,0x0);

    
    for(dscp = 0; dscp <= APOLLO_DSCPMAX ; dscp++)
    {
        newDscp = (dscp%5)+1;
        apollo_raw_qos_dscpRemarkDscp_set(dscp, newDscp);
    }    
    
    for(index =0 ; index< 13 ; index++)
    {
        if(index == 12)
        {
            dataMask = 0xFFFFFF;
            checkData =0x103081;
        }
        else
        {
            dataMask = 0x3FFFFFFF;
            checkData = 0x5103081;            
        }
        
        ioal_mem32_read(HAL_GET_REG_ADDR(RMK_DSCP_CTRLr)+4*index,&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x addr:0x%x\n",
                __FUNCTION__,
                __LINE__,
                checkData,
                regData,
                HAL_GET_REG_ADDR(RMK_DSCP_CTRLr)+4*index);
            return RT_ERR_FAILED;         
        } 
    }


    /*test for apollo_raw_qos_interPriRemarkDscp_set*/
    /*clear register to 0*/        
    ioal_mem32_write(HAL_GET_REG_ADDR(RMK_DSCP_INT_PRI_CTRLr),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(RMK_DSCP_INT_PRI_CTRLr)+4,0x0);
    
    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
        apollo_raw_qos_interPriRemarkDscp_set(priority, priority+1);
        

    dataMask = 0x3FFFFFFF;
    checkData = 0x5103081;            
    
    ioal_mem32_read(HAL_GET_REG_ADDR(RMK_DSCP_INT_PRI_CTRLr),&regData);
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }         
    dataMask = 0x3FFFF;
    checkData = 0x081C6;            
    regData = regData & dataMask; 
    ioal_mem32_read(HAL_GET_REG_ADDR(RMK_DSCP_INT_PRI_CTRLr)+4,&regData);
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }         
       
    
    
    /*test for apollo_raw_qos_dscpRemarkSource_set*/
    /*clear register to 0*/    
    ioal_mem32_write(HAL_GET_REG_ADDR(RMK_P_DSCP_SELr),0x0);
    
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_qos_dscpRemarkSource_set(port,RAW_QOS_DSCP_SOURCE_ORI_DSCP);    
    
    dataMask = 0x7F;
    checkData = 0x7F;            
    regData = regData & dataMask; 
    ioal_mem32_read(HAL_GET_REG_ADDR(RMK_P_DSCP_SELr),&regData);
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }    
    
   
    /*test for apollo_raw_qos_wfqWeight_set*/
    /*WFQ_P02_CFG0r WFQ_P46_CFG0r test*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_P02_CFG0r),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_P02_CFG0r)+4,0x0);

    ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_P46_CFG0r),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_P46_CFG0r)+4,0x0);


    /*set register by raw driver*/
    apollo_raw_qos_wfqWeight_set(0, 0, 0x1234);
    apollo_raw_qos_wfqWeight_set(1, 0, 0x5678);
    apollo_raw_qos_wfqWeight_set(2, 0, 0xABCD);
    apollo_raw_qos_wfqWeight_set(4, 0, 0x1234);
    apollo_raw_qos_wfqWeight_set(5, 0, 0x5678);
    apollo_raw_qos_wfqWeight_set(6, 0, 0xABCD);


    /*get register */
    ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_P02_CFG0r),&regData);
    if(regData != 0x56781234)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }

    ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_P02_CFG0r)+4,&regData);
    regData = regData & 0xFFFF;
    if(regData != 0xABCD)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }

    ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_P46_CFG0r),&regData);
    if(regData != 0x56781234)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }

    ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_P46_CFG0r)+4,&regData);
    regData = regData & 0xFFFF;
    if(regData != 0xABCD)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;         
    }

    /*WFQ_P02_CFG1_7r WFQ_P46_CFG1_7r test*/
    /*clear register to 0*/
    for(index =0 ; index<= 5 ; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_P02_CFG1_7r)+4*index,0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_P46_CFG1_7r)+4*index,0x0);
    }
    /*set register by raw driver*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        apollo_raw_qos_wfqWeight_set(port, 1, 0x1);
        apollo_raw_qos_wfqWeight_set(port, 2, 0x10);
        apollo_raw_qos_wfqWeight_set(port, 3, 0x20);
        apollo_raw_qos_wfqWeight_set(port, 4, 0x40);
        apollo_raw_qos_wfqWeight_set(port, 5, 0x1);
        apollo_raw_qos_wfqWeight_set(port, 6, 0x10);
        apollo_raw_qos_wfqWeight_set(port, 7, 0x20);
    }    

    for(index =0 ; index<= 5 ; index++)
    {
        if(index%2 == 0)
        {
            dataMask = 0x0FFFFFFF;
            checkData = 0x8080801;
        }
        else
        {
            dataMask = 0x1FFFFF;
            checkData = 0x80801;            
        }

        ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_P02_CFG1_7r)+4*index,&regData);
        /*mask out reserve bits*/
        regData = regData & dataMask; 

        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;         
        }        
        
        ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_P46_CFG1_7r)+4*index,&regData);
        /*mask out reserve bits*/
        regData = regData & dataMask; 

        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;         
        }
    }


    
    /*test for apollo_raw_qos_aprEnable_set*/
    /*clear register to 0*/
    for(index =0 ; index<= 2 ; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(APR_EN_P02_CFGr)+4*index,0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(APR_EN_P46_CFGr)+4*index,0x0);
    }

    /*set register by raw driver*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
            apollo_raw_qos_aprEnable_set(port, qid, ENABLED);
    }

    dataMask = 0xFF;
    checkData = 0xFF;            
    
    for(index =0 ; index<= 2 ; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(APR_EN_P02_CFGr),&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }

        ioal_mem32_read(HAL_GET_REG_ADDR(APR_EN_P46_CFGr),&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }        


    /*test for apollo_raw_qos_aprMeter_set*/
    /*clear register to 0*/
    for(index =0 ; index<= 2 ; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(APR_METER_P02_CFGr)+4*index,0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(APR_METER_P46_CFGr)+4*index,0x0);
    }    
    
    /*set register by raw driver*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
        {
            index = APOLLO_QUEUENO - 1 - qid;
            apollo_raw_qos_aprMeter_set(port, qid, index);
        }
    }    

    dataMask = 0xFFFFFF;
    checkData =0x53977; 

    for(index =0 ; index<= 2 ; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(APR_METER_P02_CFGr),&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
        ioal_mem32_read(HAL_GET_REG_ADDR(APR_METER_P46_CFGr),&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }     

    /*test for apollo_raw_qos_queueType_set*/
    /*clear register to 0*/
    for(index =0 ; index<= 2 ; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_TYPE_P02_CFGr)+4*index,0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(WFQ_TYPE_P46_CFGr)+4*index,0x0);
    }

    /*set register by raw driver*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        if(3 == port)
            continue;
        for(qid = 0; qid < APOLLO_QUEUENO ; qid++)
            apollo_raw_qos_queueType_set(port, qid, RAW_QOS_QUEUE_WFQ);
    }

    dataMask = 0xFF;
    checkData = 0xFF;            
    
    for(index =0 ; index<= 2 ; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_TYPE_P02_CFGr),&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }

        ioal_mem32_read(HAL_GET_REG_ADDR(WFQ_TYPE_P46_CFGr),&regData);
        regData = regData & dataMask; 
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;         
        }
    }

    /*test for apollo_raw_qos_ethernetAvEnable_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(AVB_PORT_ENr),0x0);
    /*set register by raw driver*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
        apollo_raw_qos_ethernetAvEnable_set(port, ENABLED);

    dataMask = 0x7F;
    checkData = 0x7F;            
    ioal_mem32_read(HAL_GET_REG_ADDR(AVB_PORT_ENr),&regData);
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }    

    /*test for apollo_raw_qos_ethernetAvEnable_set*/
    /*clear register to 0*/
    ioal_mem32_write(HAL_GET_REG_ADDR(AVB_PRI_REMAPr),0x0);
    /*set register by raw driver*/
    for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
    {
        rePri = APOLLO_PRIMAX - priority;
        apollo_raw_qos_ethernetAvRemapPri_set(priority, rePri);
    }
    dataMask = 0xFFFFFF;
    checkData =0x53977;           
    ioal_mem32_read(HAL_GET_REG_ADDR(AVB_PRI_REMAPr),&regData);
    regData = regData & dataMask; 
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;         
    }
    return RT_ERR_OK;
}    

