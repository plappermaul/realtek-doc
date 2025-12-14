#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_flowctrl.h>
#include <dal/raw/apollo/dal_raw_flowctrl_test_case.h>

int32 dal_flowctrl_raw_test(uint32 caseNo)
{
    rtk_enable_t enableW;
    rtk_enable_t enableR;
    uint32 threshold1R;
    uint32 threshold2R;
    uint32 countR;
    uint32 maxR;
    uint32 indexW;	
    uint32 indexR;
    uint32 threshold1[10] = {0,0xF,0xF0,0xFF,0xF00,0xFF0,0xFFF,0x1000,0x1800,0};
    uint32 threshold2[10] = {0,0x1800,0x1000,0xFFF,0xFF0,0xF00,0xFF,0xF0,0xF,0};	
    uint32 i, port,qid;
    apollo_raw_flowctrl_type_t typeW,typeR;
    apollo_raw_flowctrl_jumbo_size_t sizeW, sizeR;
    rtk_bmp_t maskW;
    rtk_bmp_t maskR;
	
    /*error input check*/
    /*out of range*/
    if( apollo_raw_flowctrl_type_set(RAW_FLOWCTRL_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboMode_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboSize_set(RAW_FLOWCTRL_JUMBO_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_dropAllThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_pauseAllThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_portFcoffHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFcoffLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
    if( apollo_raw_flowctrl_jumboGlobalHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboGlobalLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_portFcoffHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFcoffLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
    if( apollo_raw_flowctrl_jumboGlobalHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboGlobalLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEegressDropThreshold_set(APOLLO_QUEUENO,0)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEegressDropThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEegressDropThreshold_set(APOLLO_PORTNO,0)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEegressDropThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEegressGapThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEegressGapThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_egressDropEnable_set(APOLLO_PORTNO,0,0)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_egressDropEnable_set(0,APOLLO_QUEUENO,0)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_egressDropEnable_set(0,0,RTK_ENABLE_END)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_debugCtrl_set(APOLLO_PORTNO,0,maskW)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
    if( apollo_raw_flowctrl_debugCtrl_set(0,RTK_ENABLE_END,maskW)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    maskW.bits[0] = APOLLO_QUEUE_MASK +1;	
    if( apollo_raw_flowctrl_debugCtrl_set(0,0,maskW)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_clrMaxUsedPageCnt_set(RTK_ENABLE_END)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_clrTotalPktCnt_set(RTK_ENABLE_END)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_ponGlobalHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponGlobalHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponGlobalLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponGlobalLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortHighThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortHighThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortLowThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortLowThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropIndex_set(APOLLO_PON_QUEUE_NO ,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropIndex_set(0,APOLLO_PON_QUEUE_INDEX_NO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropThreshold_set(APOLLO_PON_QUEUE_INDEX_NO,0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropThreshold_set(0,APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_ponQueueEegressGapThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_ponUsedPageCtrl_set(APOLLO_PON_QUEUE_NO,0)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_ponUsedPageCtrl_set(0,RTK_ENABLE_END)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_txPrefet_set(APOLLO_FLOWCTRL_PREFET_THRESHOLD_MAX+1)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_lowQueueThreshold_set(APOLLO_FLOWCTRL_THRESHOLD_MAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    maskW.bits[0] = APOLLO_PORTMASK;
    if( apollo_raw_flowctrl_highQueueMask_set(APOLLO_PORTNO , maskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    maskW.bits[0] = APOLLO_QUEUE_MASK| 0x100;
    if( apollo_raw_flowctrl_highQueueMask_set(APOLLO_PORTIDMAX, maskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

   if( apollo_raw_flowctrl_type_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboMode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboSize_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_dropAllThreshold_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_pauseAllThreshold_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_portFcoffHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFcoffLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
    if( apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortLowThreshold_get(&threshold1R,NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFcoffLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_globalFconLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_portFcoffHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFcoffLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portFconLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
    if( apollo_raw_flowctrl_jumboGlobalHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboGlobalLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_jumboPortLowThreshold_get(NULL,&threshold1R)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEegressDropThreshold_get(APOLLO_QUEUENO,&threshold1R)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEegressDropThreshold_get(0,NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEegressDropThreshold_get(APOLLO_PORTNO,&threshold1R)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEegressDropThreshold_get(0,NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEegressGapThreshold_get(NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEegressGapThreshold_get(NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_egressDropEnable_get(APOLLO_PORTNO,0,&enableR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_egressDropEnable_get(0,APOLLO_QUEUENO,&enableR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_egressDropEnable_get(0,0,NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_totalPageCnt_get(NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
    if( apollo_raw_flowctrl_ponGlobalHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponGlobalHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponGlobalLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponGlobalLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortHighThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortHighThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortLowThreshold_get(NULL,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponPortLowThreshold_get(&threshold1R,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropIndex_get(APOLLO_PON_QUEUE_NO ,&indexR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropIndex_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropThreshold_get(APOLLO_PON_QUEUE_INDEX_NO,&threshold1R) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponQueueEegressDropThreshold_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_ponQueueEegressGapThreshold_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_txPrefet_get(NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_totalUsedPageCnt_get(&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_totalUsedPageCnt_get(NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_publicUsedPageCnt_get(&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_publicUsedPageCnt_get(NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portUsedPageCnt_get(0,&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portUsedPageCnt_get(0,NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portUsedPageCnt_get(APOLLO_PORTNO,&countR, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_portEgressUsedPageCnt_get(0,&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEgressUsedPageCnt_get(0,NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portEgressUsedPageCnt_get(APOLLO_PORTNO,&countR, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_publicFcoffUsedPageCnt_get(&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_publicFcoffUsedPageCnt_get(NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_publicJumboUsedPageCnt_get(&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_publicJumboUsedPageCnt_get(NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponUsedPage_get(0,&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponUsedPage_get(0,NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_ponUsedPage_get(APOLLO_PON_QUEUE_NO,&countR, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_queueEgressUsedPageCnt_get(0,0,&countR, NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEgressUsedPageCnt_get(0,0,NULL, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_queueEgressUsedPageCnt_get(APOLLO_PORTNO,0,&countR, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_queueEgressUsedPageCnt_get(0,APOLLO_QUEUENO,&countR, &maxR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_portPacketPageCnt_get(0,NULL)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_portPacketPageCnt_get(APOLLO_PORTNO,&countR)  == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }	
    if( apollo_raw_flowctrl_lowQueueThreshold_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_highQueueMask_get(APOLLO_PORTNO ,&maskR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_flowctrl_highQueueMask_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    /* get/set test*/  
    for (i = 0; i <= 9; i++)
    {
        if( apollo_raw_flowctrl_dropAllThreshold_set(threshold1[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_dropAllThreshold_get(&threshold1R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1[i] != threshold1R)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_pauseAllThreshold_set(threshold1[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_pauseAllThreshold_get(&threshold1R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1[i] != threshold1R)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_globalFcoffHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_globalFcoffHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_globalFcoffLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_globalFcoffLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_globalFconHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_globalFconHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_globalFconLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_globalFconLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }		
        if( apollo_raw_flowctrl_portFcoffHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_portFcoffHighThreshold_get(&threshold1R, &threshold2R) !=RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }		
        if( apollo_raw_flowctrl_portFcoffLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_portFcoffLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_portFconHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_portFconHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }		
        if( apollo_raw_flowctrl_portFconLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_portFconLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_jumboGlobalHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }		
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }	
        if( apollo_raw_flowctrl_jumboGlobalLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }	
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_jumboPortHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_jumboPortHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }	
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_jumboPortLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_jumboPortLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }	
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_ponGlobalHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_ponGlobalHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }	
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_ponGlobalLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_ponGlobalLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_ponPortHighThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_ponPortHighThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_ponPortLowThreshold_set(threshold1[i], threshold2[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_ponPortLowThreshold_get(&threshold1R, &threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if((threshold1[i] != threshold1R) || (threshold2[i] != threshold2R))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_queueEegressGapThreshold_set(threshold1[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_queueEegressGapThreshold_get(&threshold1R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1[i] != threshold1R) 
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_portEegressGapThreshold_set(threshold1[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_portEegressGapThreshold_get(&threshold1R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1[i] != threshold1R) 
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
        if( apollo_raw_flowctrl_ponQueueEegressGapThreshold_set(threshold1[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_ponQueueEegressGapThreshold_get(&threshold1R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1[i] != threshold1R) 
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
		
        if( apollo_raw_flowctrl_lowQueueThreshold_set(threshold1[i]) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_lowQueueThreshold_get(&threshold1R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1[i] != threshold1R)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
        if (9!=i)
        {
            threshold1[i]++;
            threshold2[i]++;			
        }
		
        for(port=0; port<APOLLO_PORTNO;port++ )
        {
            if( apollo_raw_flowctrl_portEegressDropThreshold_set(port,threshold1[i]) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_flowctrl_portEegressDropThreshold_get(port,&threshold1R) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(threshold1[i] != threshold1R)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }
            if (9!=i)
            {
                threshold1[i]++;
                threshold2[i]++;			
            }
			
        }

		
        for(qid=0; qid<APOLLO_QUEUENO;qid++ )
        {
            if( apollo_raw_flowctrl_queueEegressDropThreshold_set(qid,threshold1[i]) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_flowctrl_queueEegressDropThreshold_get(qid,&threshold1R) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(threshold1[i] != threshold1R)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }
            if (9!=i)
            {
                threshold1[i]++;
                threshold2[i]++;			
            }			
        }
		
        for(qid=0; qid<APOLLO_PON_QUEUE_INDEX_NO;qid++ )
        {			
            if( apollo_raw_flowctrl_ponQueueEegressDropThreshold_set(qid,threshold1[i]) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_flowctrl_ponQueueEegressDropThreshold_get(qid,&threshold1R) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(threshold1[i] != threshold1R)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }
            if (9!=i)
            {
                threshold1[i]++;
                threshold2[i]++;			
            }
			
        }
	  
    }

    for ( typeW = 0 ; typeW < RAW_FLOWCTRL_TYPE_END; typeW++)
    {
        if( apollo_raw_flowctrl_type_set(typeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_type_get(&typeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(typeW != typeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }   
    }

    for ( sizeW = 0 ; sizeW < RAW_FLOWCTRL_JUMBO_END; sizeW++)
    {
        if( apollo_raw_flowctrl_jumboSize_set(sizeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_jumboSize_get(&sizeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(sizeW != sizeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }   
    }

    for ( enableW = 0 ; enableW < RTK_ENABLE_END; enableW++)
    { 
        if( apollo_raw_flowctrl_jumboMode_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_jumboMode_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }   	
        for (port=0;port<APOLLO_PORTNO;port++)
        {
            for (qid=0;qid<APOLLO_QUEUENO;qid++)
            {
                if( apollo_raw_flowctrl_egressDropEnable_set(port,qid,enableW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                if( apollo_raw_flowctrl_egressDropEnable_get(port,qid,&enableR) != RT_ERR_OK)
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
        }
    }

    for (qid=0;qid<APOLLO_PON_QUEUE_NO;qid++)
    {
        for (indexW=0;indexW<APOLLO_PON_QUEUE_INDEX_NO;indexW++)
        {
            if( apollo_raw_flowctrl_ponQueueEegressDropIndex_set(qid,indexW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if( apollo_raw_flowctrl_ponQueueEegressDropIndex_get(qid,&indexR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            if(indexW != indexR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;   
            }            
        }		
    }

   
    for ( threshold1R= 0 ; threshold1R <= APOLLO_FLOWCTRL_PREFET_THRESHOLD_MAX; threshold1R++)
    {
        if( apollo_raw_flowctrl_txPrefet_set(threshold1R) != RT_ERR_OK)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
        }
        if( apollo_raw_flowctrl_txPrefet_get(&threshold2R) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(threshold1R != threshold2R)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;   
        }
    }

    for(port=0; port<APOLLO_PORTNO;port++ )
    {
        for (qid=0;qid<APOLLO_QUEUENO;qid++)
        {
            maskW.bits[0] = 1<<qid;
                if( apollo_raw_flowctrl_highQueueMask_set(port,maskW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                if( apollo_raw_flowctrl_highQueueMask_get(port,&maskR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                if(maskW.bits[0] != maskR.bits[0])
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;   
                }            		
        }
    }

	
    return RT_ERR_OK;
    
}

