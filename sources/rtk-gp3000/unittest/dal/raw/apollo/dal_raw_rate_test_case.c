#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_qos.h>
#include <dal/raw/apollo/dal_raw_rate_test_case.h>

int32 dal_rate_raw_test(uint32 caseNo)
{
    uint32 port;
    uint32 rateW;
    uint32 rateR;
    rtk_enable_t enableW;
    rtk_enable_t enableR;
    uint32 thresholdW;
    uint32 thresholdR;
    uint32 threshold2W;
    uint32 threshold2R;
    apollo_raw_ifg_include_t modeW;
    apollo_raw_ifg_include_t modeR;	

	
    /*error input check*/
    /*out of range*/
    if( apollo_raw_qos_bwCtrlEgrRate_set(APOLLO_PORTNO, APOLLO_QOS_GRANULARTY_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlEgrRate_set(APOLLO_PORTIDMAX, APOLLO_QOS_GRANULARTY_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateBypass_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRate_set(APOLLO_PORTNO, APOLLO_QOS_GRANULARTY_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRate_set(APOLLO_PORTIDMAX, APOLLO_QOS_GRANULARTY_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateCalIncIfg_set(APOLLO_PORTNO , 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateCalIncIfg_set(APOLLO_PORTIDMAX , RAW_QOS_IFG_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_qos_bwCtrlIgrRateFc_set(APOLLO_PORTIDMAX, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateFc_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrBound_set(APOLLO_QOS_INBW_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrBound_set(0,APOLLO_QOS_INBW_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	

    if( apollo_raw_qos_bwCtrlEgrRate_get(APOLLO_PORTNO, &rateR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlEgrRate_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_qos_bwCtrlIgrRateBypass_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRate_get(APOLLO_PORTNO, &rateR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRate_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateCalIncIfg_get(APOLLO_PORTNO , &modeR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateCalIncIfg_get(APOLLO_PORTIDMAX , NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_qos_bwCtrlIgrRateFc_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrRateFc_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrBound_get(&thresholdR,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_qos_bwCtrlIgrBound_get(NULL,&thresholdR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	

    /* get/set test*/  
    for (port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        for ( rateW = 1; rateW < APOLLO_QOS_GRANULARTY_MAX; rateW = (rateW<<1))
        {
            if( apollo_raw_qos_bwCtrlEgrRate_set(port, rateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_qos_bwCtrlEgrRate_get(port, &rateR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(rateW != rateR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }			
        }
        for ( rateW = 1; rateW < APOLLO_QOS_GRANULARTY_MAX; rateW = (rateW<<1))
        {
            if( apollo_raw_qos_bwCtrlIgrRate_set(port, rateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_qos_bwCtrlIgrRate_get(port, &rateR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(rateW != rateR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }			
        }
        for ( enableW = 0 ; enableW < RTK_ENABLE_END; enableW++)
        {
            if( apollo_raw_qos_bwCtrlIgrRateFc_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_qos_bwCtrlIgrRateFc_get(port, &enableR) != RT_ERR_OK)
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

        for ( modeW = 0 ; modeW < RAW_QOS_IFG_TYPE_END; modeW++)
        {
            if( apollo_raw_qos_bwCtrlIgrRateCalIncIfg_set(port, modeW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_qos_bwCtrlIgrRateCalIncIfg_get(port, &modeR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(modeW != modeR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }
        }	  
    }

    for ( thresholdW = 0 ; thresholdW < APOLLO_QOS_INBW_THRESHOLD_MAX; thresholdW++)
    {
        threshold2W = APOLLO_QOS_INBW_THRESHOLD_MAX - thresholdW;
        if( apollo_raw_qos_bwCtrlIgrBound_set(thresholdW, threshold2W) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_qos_bwCtrlIgrBound_get(&thresholdR, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if ((thresholdW != thresholdR) || (threshold2W != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
    }
        
    for ( enableW = 0 ; enableW < RTK_ENABLE_END; enableW++)
    {
        if( apollo_raw_qos_bwCtrlIgrRateBypass_set(enableW) != RT_ERR_OK)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
        }
        if( apollo_raw_qos_bwCtrlIgrRateBypass_get(&enableR) != RT_ERR_OK)
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
	
    return RT_ERR_OK;
    
}

