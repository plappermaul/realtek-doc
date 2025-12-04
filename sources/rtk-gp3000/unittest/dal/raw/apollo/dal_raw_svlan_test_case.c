#include <osal/lib.h>
#include <ioal/mem32.h>
#include <dal/apollo/raw/apollo_raw_svlan.h>
#include <dal/raw/apollo/dal_raw_svlan_test_case.h>


int32 dal_svlan_raw_test(uint32 caseNo)
{  
    int32 ret;
    rtk_port_t port;
    rtk_enable_t enableR;
    rtk_enable_t enableW;
    apollo_raw_svidx_t svidxW;
    apollo_raw_svidx_t svidxR;
    rtk_svlan_lookupType_t lookTypeR;
    rtk_svlan_lookupType_t lookTypeW;
    apollo_raw_ethertype_t svlanTpidR;
    apollo_raw_ethertype_t svlanTpidW;
    rtk_pri_t priorityR;
    rtk_pri_t priorityW;
    rtk_svlan_priSel_t priSelmodeR;
    rtk_svlan_priSel_t priSelmodeW;
    rtk_svlan_action_t actionR;
    rtk_svlan_action_t actionW;
    
    
    /*error input check*/
    /*out of range*/
    if( apollo_raw_svlan_portUplinkEnable_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_portUplinkEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_portUplinkEnable_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_port1tonVlanEnable_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_port1tonVlanEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_port1tonVlanEnable_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_portSvlan_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_portSvlan_set(0, APOLLO_SVIDXMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_portSvlan_get(APOLLO_PORTNO, &svidxR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_lookupType_set(RAW_SVLAN_LOOKUP_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_tpid_set(0x10000) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_cfiKeepEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_trapPri_set(8) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_egrPriSel_set(SVLAN_PRISEL_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_untagAction_set(SVLAN_ACTION_SVLAN_AND_KEEP) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_unmatchAction_set(SVLAN_ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_untagSvidx_set(APOLLO_SVIDXMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_unmatchSvidx_set(APOLLO_SVIDXMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    
    /* get/set test*/
   
    for(port=0; port<7 ; port++)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
            if( apollo_raw_svlan_portUplinkEnable_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_svlan_portUplinkEnable_get(port, &enableR) != RT_ERR_OK)
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

    for(port=0; port<=APOLLO_PORTIDMAX; port++)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
            if( apollo_raw_svlan_port1tonVlanEnable_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_svlan_port1tonVlanEnable_get(port, &enableR) != RT_ERR_OK)
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

    for(port=0; port<=APOLLO_PORTIDMAX ; port++)
    {
        for(svidxW = 0; svidxW <= APOLLO_SVIDXMAX; svidxW += 4)
        {
            if( apollo_raw_svlan_portSvlan_set(port, svidxW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_svlan_portSvlan_get(port, &svidxR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        
            if(svidxW != svidxR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }   
        }
    }

    for(lookTypeW=0; lookTypeW<SVLAN_LOOKUP_END ; lookTypeW++)
    {
        if( apollo_raw_svlan_lookupType_set(lookTypeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_svlan_lookupType_get(&lookTypeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(lookTypeR != lookTypeW)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }   

    }

    for(svlanTpidW=0; svlanTpidW<0x10000 ; svlanTpidW+=0x1FFF)
    {
        if( apollo_raw_svlan_tpid_set(svlanTpidW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if( apollo_raw_svlan_tpid_get(&svlanTpidR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        if(svlanTpidR != svlanTpidW)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }   

    }

    for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
    {
        if( apollo_raw_svlan_cfiKeepEnable_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_svlan_cfiKeepEnable_get(&enableR) != RT_ERR_OK)
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

    for(priorityW = 0; priorityW <= 7; priorityW ++)
    {
        if( apollo_raw_svlan_trapPri_set(priorityW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        if( apollo_raw_svlan_trapPri_get(&priorityR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(priorityW != priorityR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }        
    }

    for(priSelmodeW = 0; priSelmodeW < SVLAN_PRISEL_END; priSelmodeW ++)
    {
        if( apollo_raw_svlan_egrPriSel_set(priSelmodeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        if( apollo_raw_svlan_egrPriSel_get(&priSelmodeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(priSelmodeW != priSelmodeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }        
    }

    for(actionW = 0; actionW <= RAW_SVLAN_ACTION_SVLAN; actionW ++)
    {
        if( apollo_raw_svlan_untagAction_set(actionW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        if( apollo_raw_svlan_untagAction_get(&actionR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(actionW != actionR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }        
    }

    for(actionW = 0; actionW < RAW_SVLAN_ACTION_END; actionW ++)
    {
        if( apollo_raw_svlan_unmatchAction_set(actionW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        if( apollo_raw_svlan_unmatchAction_get(&actionR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(actionW != actionR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }        
    }

    for(svidxW = 0; svidxW <= APOLLO_SVIDXMAX; svidxW += 4)
    {
        if( apollo_raw_svlan_untagSvidx_set(svidxW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_svlan_untagSvidx_get(&svidxR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(svidxW != svidxR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }   
    }

    for(svidxW = 0; svidxW <= APOLLO_SVIDXMAX; svidxW += 4)
    {
        if( apollo_raw_svlan_unmatchSvidx_set(svidxW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_svlan_unmatchSvidx_get(&svidxR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(svidxW != svidxR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }   
    }


    /*null pointer*/
    if( apollo_raw_svlan_portUplinkEnable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_port1tonVlanEnable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_portSvlan_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_lookupType_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_tpid_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_cfiKeepEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_svlan_trapPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_svlan_egrPriSel_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_svlan_untagAction_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_svlan_unmatchAction_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_svlan_untagSvidx_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    if( apollo_raw_svlan_unmatchSvidx_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

#if 0

    /*set value checking with register address checking*/
    ioal_mem32_write(HAL_GET_REG_ADDR(SVLAN_UPLINK_PMSKr),0x0);

    for(port=0; port<=APOLLO_PORTIDMAX ; port++)
        apollo_raw_svlan_portUplinkEnable_set(port,port&1);        

    ioal_mem32_read(HAL_GET_REG_ADDR(SVLAN_UPLINK_PMSKr),&regData);
    

    /*error input check*/
    /*out of range*/
    if( apollo_raw_svlan_portUplinkEnable_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_portUplinkEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_portUplinkEnable_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_port1tonVlanEnable_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_port1tonVlanEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_port1tonVlanEnable_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_portSvlan_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_portSvlan_set(0, APOLLO_SVIDXMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_portSvlan_get(APOLLO_PORTNO, &svidxR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_lookupType_set(RAW_SVLAN_LOOKUP_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_tpid_set(0x10000) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_cfiKeepEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_trapPri_set(8) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_egrPriSel_set(SVLAN_PRISEL_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_untagAction_set(SVLAN_ACTION_SVLAN_AND_KEEP) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( apollo_raw_svlan_unmatchAction_set(SVLAN_ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_untagSvidx_set(APOLLO_SVIDXMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_unmatchSvidx_set(APOLLO_SVIDXMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


#endif
    
    return RT_ERR_OK;
}    

int32 dal_svlan_raw_mbrCfg_test(uint32 caseNo)
{  
    int32 ret;

    apollo_raw_svlan_mbrCfg_t mbrCfgR;
    apollo_raw_svlan_mbrCfg_t mbrCfgW;

    /*error input check*/
    /*out of range*/
    osal_memset(&mbrCfgW,0x0, sizeof(apollo_raw_svlan_mbrCfg_t));
    mbrCfgW.idx = APOLLO_SVIDXMAX + 1;
    if( apollo_raw_svlan_mrbCfg_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    mbrCfgR.idx = APOLLO_SVIDXMAX + 1;
    if( apollo_raw_svlan_mrbCfg_get(&mbrCfgR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0, sizeof(apollo_raw_svlan_mbrCfg_t));
    mbrCfgW.spri = 8;
    if( apollo_raw_svlan_mrbCfg_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0, sizeof(apollo_raw_svlan_mbrCfg_t));
    mbrCfgW.fid_msti = 16;
    if( apollo_raw_svlan_mrbCfg_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    osal_memset(&mbrCfgW,0x0, sizeof(apollo_raw_svlan_mbrCfg_t));
    mbrCfgW.efid = 8;
    if( apollo_raw_svlan_mrbCfg_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /* get/set test*/
    for(mbrCfgW.idx = 0; mbrCfgW.idx <= APOLLO_SVIDXMAX ; mbrCfgW.idx += APOLLO_SVIDXMAX)
    {
        for(mbrCfgW.svid = 0; mbrCfgW.svid <= 0xFFF ; mbrCfgW.svid += 0x3FF)
        {
            for(mbrCfgW.mbr.bits[0] = 0, mbrCfgW.untagset.bits[0] = 0; mbrCfgW.mbr.bits[0] <= 0x3F; mbrCfgW.mbr.bits[0] += 0x1F, mbrCfgW.untagset.bits[0] += 0x1F)
            {
                for(mbrCfgW.spri = 0; mbrCfgW.spri <= 7 ; mbrCfgW.spri +=2)
                {
                    for(mbrCfgW.fid_msti = 0; mbrCfgW.fid_msti <= 15 ; mbrCfgW.fid_msti +=7)
                    {
                        for(mbrCfgW.efid = 0; mbrCfgW.efid <= 7 ; mbrCfgW.efid +=3)
                        {
                            for(mbrCfgW.efid_en = 0, mbrCfgW.fid_en = !mbrCfgW.efid_en; mbrCfgW.efid_en <= 1 ; mbrCfgW.efid_en ++, mbrCfgW.fid_en = !mbrCfgW.efid_en)
                            {
                                if( apollo_raw_svlan_mrbCfg_set(&mbrCfgW) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                mbrCfgR.idx = mbrCfgW.idx;
                                if( apollo_raw_svlan_mrbCfg_get(&mbrCfgR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }
                            
                                if(osal_memcmp(&mbrCfgR,&mbrCfgW, sizeof(apollo_raw_svlan_mbrCfg_t)))
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }
                            }
                        }
                    }
                }      
            }
        }
    }    

    /*null pointer*/
    if( apollo_raw_svlan_mrbCfg_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_mrbCfg_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}    

int32 dal_svlan_raw_mc2sCfg_test(uint32 caseNo)
{  
    int32 ret;

    apollo_raw_svlan_mc2sCfg_t mc2sCfgW;
    apollo_raw_svlan_mc2sCfg_t mc2sCfgR;
    
    /*error input check*/
    /*out of range*/
    osal_memset(&mc2sCfgW,0x0, sizeof(apollo_raw_svlan_mc2sCfg_t));
    mc2sCfgW.idx = APOLLO_SVLAN_MC2S_INDEX_MAX + 1;
    if( apollo_raw_svlan_mc2sCfg_set(&mc2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    mc2sCfgR.idx = APOLLO_SVLAN_MC2S_INDEX_MAX + 1;
    if( apollo_raw_svlan_mc2sCfg_get(&mc2sCfgR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mc2sCfgW,0x0, sizeof(apollo_raw_svlan_mc2sCfg_t));
    mc2sCfgW.svidx = APOLLO_SVIDXMAX + 1;
    if( apollo_raw_svlan_mc2sCfg_set(&mc2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mc2sCfgW,0x0, sizeof(apollo_raw_svlan_mc2sCfg_t));
    mc2sCfgW.format = RAW_SVLAN_MC2S_FMT_END;
    if( apollo_raw_svlan_mc2sCfg_set(&mc2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /* get/set test*/
    for(mc2sCfgW.idx = 0; mc2sCfgW.idx < APOLLO_SVLAN_MC2S_INDEX_MAX ; mc2sCfgW.idx +=3)
    {
        for(mc2sCfgW.svidx = 0; mc2sCfgW.svidx <= APOLLO_SVIDXMAX ; mc2sCfgW.svidx += 0x1F)
        {
            for(mc2sCfgW.format = 0; mc2sCfgW.format < RAW_SVLAN_MC2S_FMT_END; mc2sCfgW.format++)
            {
                for(mc2sCfgW.data = 0, mc2sCfgW.mask = mc2sCfgW.data; mc2sCfgW.data <= 0xFFFF ; mc2sCfgW.data +=0x3FFF, mc2sCfgW.mask = mc2sCfgW.data)
                {
                    if( apollo_raw_svlan_mc2sCfg_set(&mc2sCfgW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    mc2sCfgR.idx = mc2sCfgW.idx;
                    if( apollo_raw_svlan_mc2sCfg_get(&mc2sCfgR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                
                    if(osal_memcmp(&mc2sCfgR,&mc2sCfgW, sizeof(apollo_raw_svlan_mc2sCfg_t)))
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }      
            }
        }
    }    

    /*null pointer*/
    if( apollo_raw_svlan_mc2sCfg_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_mc2sCfg_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}  

int32 dal_svlan_raw_c2sCfg_test(uint32 caseNo)
{  
    int32 ret;
    apollo_raw_svlan_c2sCfg_t c2sCfgR;
    apollo_raw_svlan_c2sCfg_t c2sCfgW;


    /*error input check*/
    /*out of range*/
    osal_memset(&c2sCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    c2sCfgW.idx = APOLLO_SVLAN_C2S_INDEX_MAX + 1;    
    if( apollo_raw_svlan_c2sCfg_set(&c2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    c2sCfgR.idx = APOLLO_SVLAN_C2S_INDEX_MAX + 1;    
    if( apollo_raw_svlan_c2sCfg_get(&c2sCfgR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    osal_memset(&c2sCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    c2sCfgW.svidx = APOLLO_SVIDXMAX + 1;   
    if( apollo_raw_svlan_c2sCfg_set(&c2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&c2sCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    c2sCfgW.svidx = APOLLO_SVIDXMAX + 1;   
    if( apollo_raw_svlan_c2sCfg_set(&c2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&c2sCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    c2sCfgW.evid = APOLLO_EVIDMAX + 1;   
    if( apollo_raw_svlan_c2sCfg_set(&c2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&c2sCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    c2sCfgW.pmsk.bits[0]= APOLLO_PMSKMAX | (1<<APOLLO_PORTNO);   
    if( apollo_raw_svlan_c2sCfg_set(&c2sCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&c2sCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    osal_memset(&c2sCfgR,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    /* get/set test*/
    for(c2sCfgW.idx = 0; c2sCfgW.idx < APOLLO_SVLAN_C2S_INDEX_MAX ; c2sCfgW.idx +=0x1F)
    {
        for(c2sCfgW.svidx = 0; c2sCfgW.svidx <= APOLLO_SVIDXMAX ; c2sCfgW.svidx += 0xF)
        {
            for(c2sCfgW.evid = 0; c2sCfgW.evid <= 0x1FFF; c2sCfgW.evid += 0x7FF)
            {
                for(c2sCfgW.pmsk.bits[0] = 0; c2sCfgW.pmsk.bits[0] <= 0x3F ;c2sCfgW.pmsk.bits[0] +=0xF)
                {
                    if( apollo_raw_svlan_c2sCfg_set(&c2sCfgW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    c2sCfgR.idx = c2sCfgW.idx;
                    if( apollo_raw_svlan_c2sCfg_get(&c2sCfgR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                
                    if(osal_memcmp(&c2sCfgR,&c2sCfgW, sizeof(apollo_raw_svlan_c2sCfg_t)))
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }      
            }
        }
    }    


    /*null pointer*/
    if( apollo_raw_svlan_c2sCfg_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_c2sCfg_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}  

int32 dal_svlan_raw_sp2cCfg_test(uint32 caseNo)
{  
    int32 ret;
    apollo_raw_svlan_sp2cCfg_t sp2cCfgR;
    apollo_raw_svlan_sp2cCfg_t sp2cCfgW;

    /*error input check*/
    /*out of range*/
    osal_memset(&sp2cCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    sp2cCfgW.idx = APOLLO_SVLAN_SP2C_INDEX_MAX + 1;    
    if( apollo_raw_svlan_sp2cCfg_set(&sp2cCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    sp2cCfgR.idx = APOLLO_SVLAN_SP2C_INDEX_MAX + 1;    
    if( apollo_raw_svlan_sp2cCfg_get(&sp2cCfgR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    osal_memset(&sp2cCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    sp2cCfgW.svidx = APOLLO_SVIDXMAX + 1;    
    if( apollo_raw_svlan_sp2cCfg_set(&sp2cCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&sp2cCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    sp2cCfgW.vid = 4096;    
    if( apollo_raw_svlan_sp2cCfg_set(&sp2cCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&sp2cCfgW,0x0, sizeof(apollo_raw_svlan_c2sCfg_t));
    sp2cCfgW.port = APOLLO_PORTNO;    
    if( apollo_raw_svlan_sp2cCfg_set(&sp2cCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /* get/set test*/
    for(sp2cCfgW.idx = 0; sp2cCfgW.idx < APOLLO_SVLAN_SP2C_INDEX_MAX ; sp2cCfgW.idx +=0x1F)
    {
        for(sp2cCfgW.svidx = 0; sp2cCfgW.svidx <= APOLLO_SVIDXMAX ; sp2cCfgW.svidx += 0xF)
        {
            for(sp2cCfgW.vid = 0; sp2cCfgW.vid <= 4095; sp2cCfgW.vid += 0x3FF)
            {
                for(sp2cCfgW.port = 0, sp2cCfgW.valid = 0; sp2cCfgW.port <= 6 ;sp2cCfgW.port ++, sp2cCfgW.valid = (sp2cCfgW.port&1))
                {
                    if( apollo_raw_svlan_sp2cCfg_set(&sp2cCfgW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    sp2cCfgR.idx = sp2cCfgW.idx;
                    if( apollo_raw_svlan_sp2cCfg_get(&sp2cCfgR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                
                    if((sp2cCfgR.idx != sp2cCfgW.idx) || 
                        (sp2cCfgR.svidx != sp2cCfgW.svidx) || 
                        (sp2cCfgR.vid != sp2cCfgW.vid) || 
                        (sp2cCfgR.port != sp2cCfgW.port) || 
                        (sp2cCfgR.valid != sp2cCfgW.valid))
                    {
                    #if 0    
                        osal_printf("%d %d %d %d %d - %d %d %d %d %d\n",
                                      sp2cCfgR.idx,
                                      sp2cCfgR.svidx,
                                      sp2cCfgR.vid,
                                      sp2cCfgR.port,
                                      sp2cCfgR.valid,
                                      sp2cCfgW.idx,
                                      sp2cCfgW.svidx,
                                      sp2cCfgW.vid,
                                      sp2cCfgW.port,
                                      sp2cCfgW.valid);
                    #endif                  
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        
                        return RT_ERR_FAILED;
                    }
                }      
            }
        }
    }    


    /*null pointer*/
    if( apollo_raw_svlan_sp2cCfg_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( apollo_raw_svlan_sp2cCfg_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}  







