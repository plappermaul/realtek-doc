#include <osal/lib.h>
#include <ioal/mem32.h>
#include <dal/apollo/raw/apollo_raw_vlan.h>
#include <dal/raw/apollo/dal_raw_vlan_test_case.h>

int32 dal_vlan_raw_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t port;
    rtk_enable_t enable;
    uint32 index;
    uint32 priority;
    rtk_portmask_t portMsk;
    uint32 rData;
    uint32 rIndex;
    rtk_portmask_t rPortMsk;
    uint32 regName;
    uint32 regData;
    uint32 dataMask;
    uint32 checkData;

    /*error input check*/
    /*out of range*/
    /*set*/
    if( apollo_raw_vlan_portBasedVID_set(APOLLO_PORTNO, 0, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_portBasedVID_set(0, APOLLO_CVIDXNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_portBasedVID_set(0, 0, (APOLLO_PRIMAX+1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_portBasedVID_get(APOLLO_PORTNO, &rIndex, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*set*/
    if( apollo_raw_vlan_cfiKeepMode_set(RAW_CFI_KEEP_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_acceptFrameType_set(APOLLO_PORTNO, RAW_VLAN_ACCEPT_ALL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_acceptFrameType_set(0, RAW_VLAN_ACCEPT_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_acceptFrameType_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_igrFlterEnable_set(APOLLO_PORTNO, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_igrFlterEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_igrFlterEnable_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_egrTagMode_set(APOLLO_PORTNO, RAW_VLAN_EGR_TAG_MODE_ORI) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_egrTagMode_set(0, RAW_VLAN_EGR_TAG_MODE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_egrTagMode_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*set*/
    if( apollo_raw_vlan_vid0TagType_set(RAW_VLAN_TAG_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_vlan_vid4095TagType_set(RAW_VLAN_TAG_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_transparentEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_egrIgrFilterEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_portBasedFidEnable_set(APOLLO_PORTNO, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_portBasedFidEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_portBasedFidEnable_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*set*/
    if( apollo_raw_vlan_portBasedFid_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_portBasedFid_set(0,(APOLLO_FIDMAX + 1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_portBasedFid_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_portBasedPri_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_portBasedPri_set(0,(APOLLO_PRIMAX + 1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_portBasedPri_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_portToMbrCfgIdx_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_portToMbrCfgIdx_set(0, APOLLO_CVIDXNO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_portToMbrCfgIdx_get(APOLLO_PORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_vcPortToMbrCfgIdx_set(APOLLO_VCPORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_vcPortToMbrCfgIdx_set(0, APOLLO_CVIDXNO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_vcPortToMbrCfgIdx_get(APOLLO_VCPORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    if( apollo_raw_vlan_extPortToMbrCfgIdx_set(APOLLO_EXTPORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_vlan_extPortToMbrCfgIdx_set(0, APOLLO_CVIDXNO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_extPortToMbrCfgIdx_get(APOLLO_EXTPORTNO, &rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*set*/
    memset(&portMsk,0x0,sizeof(rtk_portmask_t));
    if( apollo_raw_vlan_egrKeepPmsk_set(APOLLO_PORTNO, portMsk) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    RTK_PORTMASK_PORT_SET(portMsk, APOLLO_PORTNO);
    if( apollo_raw_vlan_egrKeepPmsk_set(0, portMsk) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*get*/
    if( apollo_raw_vlan_egrKeepPmsk_get(APOLLO_PORTNO, &rPortMsk) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* get/set test*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = 0; index < APOLLO_CVIDXNO ; index++)
        {
            for(priority = 0; priority <= APOLLO_PRIMAX ; priority++)
            {
                /*set*/
                if( apollo_raw_vlan_portBasedVID_set(port, index, priority) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( apollo_raw_vlan_portBasedVID_get(port, &rIndex, &rData) != RT_ERR_OK)
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
                if(index != rIndex)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }
    /* get/set test*/
    for(index = RAW_CFI_ALWAYS_0; index < RAW_CFI_KEEP_TYPE_END ; index++)
    {
        /*set*/
        if( apollo_raw_vlan_cfiKeepMode_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        /*get*/
        if( apollo_raw_vlan_cfiKeepMode_get(&rData) != RT_ERR_OK)
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
        for(index = RAW_VLAN_ACCEPT_ALL; index < RAW_VLAN_ACCEPT_TYPE_END ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_acceptFrameType_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_acceptFrameType_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = DISABLED; index < RTK_ENABLE_END ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_igrFlterEnable_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_igrFlterEnable_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = RAW_VLAN_EGR_TAG_MODE_ORI; index < RAW_VLAN_EGR_TAG_MODE_END ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_egrTagMode_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_egrTagMode_get(port, &rIndex) != RT_ERR_OK)
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

    for(index = RAW_VLAN_UNTAG; index < RAW_VLAN_TAG_TYPE_END ; index++)
    {
        /*set*/
        if( apollo_raw_vlan_vid0TagType_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        /*get*/
        if( apollo_raw_vlan_vid0TagType_get(&rData) != RT_ERR_OK)
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

    for(index = RAW_VLAN_UNTAG; index < RAW_VLAN_TAG_TYPE_END ; index++)
    {
        /*set*/
        if( apollo_raw_vlan_vid4095TagType_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        /*get*/
        if( apollo_raw_vlan_vid4095TagType_get(&rData) != RT_ERR_OK)
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

    for(index = DISABLED; index < RTK_ENABLE_END ; index++)
    {
        /*set*/
        if( apollo_raw_vlan_transparentEnable_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        /*get*/
        if( apollo_raw_vlan_transparentEnable_get(&rIndex) != RT_ERR_OK)
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

    for(index = DISABLED; index < RTK_ENABLE_END ; index++)
    {
        /*set*/
        if( apollo_raw_vlan_egrIgrFilterEnable_set(index) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        /*get*/
        if( apollo_raw_vlan_egrIgrFilterEnable_get(&rIndex) != RT_ERR_OK)
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

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = DISABLED; index < RTK_ENABLE_END ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_portBasedFidEnable_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_portBasedFidEnable_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = 0; index <= APOLLO_FIDMAX ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_portBasedFid_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_portBasedFid_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = 0; index <= APOLLO_PRIMAX ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_portBasedPri_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_portBasedPri_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        for(index = 0; index <= APOLLO_CVIDXMAX ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_portToMbrCfgIdx_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_portToMbrCfgIdx_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port <= APOLLO_VCPORTNOMAX ; port++)
    {
        for(index = 0; index <= APOLLO_CVIDXMAX ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_vcPortToMbrCfgIdx_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_vcPortToMbrCfgIdx_get(port, &rIndex) != RT_ERR_OK)
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

    for(port = 0; port <= APOLLO_EXTPORTNOMAX ; port++)
    {
        for(index = 0; index <= APOLLO_CVIDXMAX ; index++)
        {
            /*set*/
            if( apollo_raw_vlan_extPortToMbrCfgIdx_set(port, index) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            if( apollo_raw_vlan_extPortToMbrCfgIdx_get(port, &rIndex) != RT_ERR_OK)
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


    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        memset(&portMsk,0x0,sizeof(rtk_portmask_t));
        for(index = 0; index < APOLLO_PORTNO ; index++)
        {
            RTK_PORTMASK_PORT_SET(portMsk, index);
            /*set*/
            if( apollo_raw_vlan_egrKeepPmsk_set(port, portMsk) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*get*/
            memset(&rPortMsk,0x0,sizeof(rtk_portmask_t));
            if( apollo_raw_vlan_egrKeepPmsk_get(port, &rPortMsk) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            /*compare*/
            if(RTK_PORTMASK_COMPARE(portMsk,rPortMsk) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /*null pointer*/
    if(apollo_raw_vlan_portBasedVID_get(0,&rData,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(apollo_raw_vlan_portBasedVID_get(0,NULL,&rData) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_cfiKeepMode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_acceptFrameType_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_igrFlterEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_egrTagMode_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_vid0TagType_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_vid4095TagType_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_transparentEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_egrIgrFilterEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_portBasedFidEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_portBasedFid_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_portBasedPri_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_portToMbrCfgIdx_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_vcPortToMbrCfgIdx_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_extPortToMbrCfgIdx_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(apollo_raw_vlan_egrKeepPmsk_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }




    /*set value checking with register address checking*/

    /*test for apollo_raw_vlan_portBasedVID_set*/
    /*clear register to 0*/
    regName = VLAN_PB_VIDXr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4,0x0);

    regName = VLAN_PB_PRIr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

        /*test for apollo_raw_vlan_egrIgrFilterEnable_set*/
    /*clear register to 0*/
    regName = VLAN_CTRLr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    apollo_raw_vlan_egrIgrFilterEnable_set(ENABLED);

    /*get register data*/
    dataMask = 0x1;
    checkData =0x1;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_transparentEnable_set*/
    /*clear register to 0*/
    regName = VLAN_CTRLr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    apollo_raw_vlan_transparentEnable_set(ENABLED);

    /*get register data*/
    dataMask = 0x2;
    checkData =0x2;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_portBasedVID_set*/
    /*clear register to 0*/
    regName = VLAN_CTRLr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    apollo_raw_vlan_cfiKeepMode_set(RAW_CFI_KEEP_INGRESS);

    /*get register data*/
    dataMask = 0x4;
    checkData =0x4;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }


    /*test for apollo_raw_vlan_vid0TagType_set*/
    /*clear register to 0*/
    regName = VLAN_CTRLr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    apollo_raw_vlan_vid0TagType_set(RAW_VLAN_TAG);

    /*get register data*/
    dataMask = 0x8;
    checkData =0x8;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_vid4095TagType_set*/
    /*clear register to 0*/
    regName = VLAN_CTRLr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    apollo_raw_vlan_vid4095TagType_set(RAW_VLAN_TAG);

    /*get register data*/
    dataMask = 0x10;
    checkData =0x10;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        index = port;
        priority = port;
        /*set*/
        apollo_raw_vlan_portBasedVID_set(port, index, priority);
    }

    regName = VLAN_PB_VIDXr;
    dataMask = 0x3FFFFFFF;
    checkData =0xA418820;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }
    dataMask = 0x1F;
    checkData =0x6;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4,&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }
    regName = VLAN_PB_PRIr;
    dataMask = 0x1FFFFF;
    checkData =0x1AC688;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_egrIgrFilterEnable_set*/
    /*clear register to 0*/
    regName = VLAN_PORT_ACCEPT_FRAME_TYPEr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    /*set*/
    apollo_raw_vlan_acceptFrameType_set(0, 0x0);
    apollo_raw_vlan_acceptFrameType_set(1, 0x1);
    apollo_raw_vlan_acceptFrameType_set(2, 0x2);
    apollo_raw_vlan_acceptFrameType_set(3, 0x3);
    apollo_raw_vlan_acceptFrameType_set(4, 0x3);
    apollo_raw_vlan_acceptFrameType_set(5, 0x2);
    apollo_raw_vlan_acceptFrameType_set(6, 0x1);

    /*get register data*/
    dataMask = 0x3FFF;
    checkData =0x1BE4;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_igrFlterEnable_set*/
    /*clear register to 0*/
    regName = VLAN_INGRESSr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    /*set*/
    apollo_raw_vlan_igrFlterEnable_set(0, ENABLED);
    apollo_raw_vlan_igrFlterEnable_set(1, ENABLED);
    apollo_raw_vlan_igrFlterEnable_set(2, DISABLED);
    apollo_raw_vlan_igrFlterEnable_set(3, ENABLED);
    apollo_raw_vlan_igrFlterEnable_set(4, DISABLED);
    apollo_raw_vlan_igrFlterEnable_set(5, DISABLED);
    apollo_raw_vlan_igrFlterEnable_set(6, ENABLED);

    /*get register data*/
    dataMask = 0x7F;
    checkData =0x4B;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }


    /*test for apollo_raw_vlan_egrTagMode_set*/
    /*clear register to 0*/
    regName = VLAN_EGRESS_TAGr;
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(regName)+HAL_GET_MACPP_INTERVAL()*port,0x0);

        /*set register by raw driver*/
        /*set*/
        apollo_raw_vlan_egrTagMode_set(port, RAW_VLAN_EGR_TAG_MODE_PRI_TAG);

        /*get register data*/
        dataMask = 0x3;
        checkData =0x2;
        ioal_mem32_read(HAL_GET_REG_ADDR(regName)+HAL_GET_MACPP_INTERVAL()*port,&regData);
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
            return RT_ERR_FAILED;
        }
    }

    /*test for apollo_raw_vlan_portBasedFidEnable_set*/
    /*clear register to 0*/
    regName = VLAN_PB_EFIDENr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    /*set*/
    apollo_raw_vlan_portBasedFidEnable_set(0, ENABLED);
    apollo_raw_vlan_portBasedFidEnable_set(1, ENABLED);
    apollo_raw_vlan_portBasedFidEnable_set(2, DISABLED);
    apollo_raw_vlan_portBasedFidEnable_set(3, ENABLED);
    apollo_raw_vlan_portBasedFidEnable_set(4, DISABLED);
    apollo_raw_vlan_portBasedFidEnable_set(5, DISABLED);
    apollo_raw_vlan_portBasedFidEnable_set(6, ENABLED);

    /*get register data*/
    dataMask = 0x7F;
    checkData =0x4B;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_portBasedFid_set*/
    /*clear register to 0*/
    regName = VLAN_PB_EFIDr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    /*set*/
    apollo_raw_vlan_portBasedFid_set(0, 0x1);
    apollo_raw_vlan_portBasedFid_set(1, 0x2);
    apollo_raw_vlan_portBasedFid_set(2, 0x3);
    apollo_raw_vlan_portBasedFid_set(3, 0x4);
    apollo_raw_vlan_portBasedFid_set(4, 0x5);
    apollo_raw_vlan_portBasedFid_set(5, 0x6);
    apollo_raw_vlan_portBasedFid_set(6, 0x7);

    /*get register data*/
    dataMask = 0xFFFFFFF;
    checkData =0x7654321;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }


    /*test for apollo_raw_vlan_portBasedPri_set*/
    /*clear register to 0*/
    regName = VLAN_PB_PRIr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        priority = port;
        apollo_raw_vlan_portBasedPri_set(port, priority);
    }

    /*get register data*/
    dataMask = 0x1FFFFF;
    checkData =0x1AC688;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }


    /*test for apollo_raw_vlan_portToMbrCfgIdx_set*/
    /*clear register to 0*/
    regName = VLAN_PB_VIDXr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4,0x0);

    /*set register by raw driver*/
    /*set*/
    for(port = 0; port < APOLLO_PORTNO ; port++)
    {
        index = port;
        apollo_raw_vlan_portToMbrCfgIdx_set(port, index);
    }

    /*get register data*/
    dataMask = 0x3FFFFFFF;
    checkData =0xA418820;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    dataMask = 0x1F;
    checkData =0x6;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4,&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_vcPortToMbrCfgIdx_set*/
    /*clear register to 0*/
    regName = VLAN_VC_VIDXr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4,0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(regName)+8,0x0);

    /*set register by raw driver*/
    /*set*/
    for(port = 0; port <= APOLLO_VCPORTNOMAX ; port++)
    {
        index = port % 6;
        apollo_raw_vlan_vcPortToMbrCfgIdx_set(port, index);
    }

    /*get register data*/
    dataMask = 0x3FFFFFFF;
    checkData =0xA418820;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }
    ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4,&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }
    dataMask = 0xFFFFF;
    checkData =0x18820;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName)+8,&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_vcPortToMbrCfgIdx_set*/
    /*clear register to 0*/
    regName = VLAN_EXT_VIDXr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);

    /*set register by raw driver*/
    /*set*/
    for(port = 0; port <= APOLLO_EXTPORTNOMAX ; port++)
        apollo_raw_vlan_extPortToMbrCfgIdx_set(port, port);

    /*get register data*/
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    /*test for apollo_raw_vlan_egrKeepPmsk_set*/
    /*clear register to 0*/
    regName = VLAN_EGRESS_KEEPr;
    ioal_mem32_write(HAL_GET_REG_ADDR(regName),0x0);
    ioal_mem32_write(HAL_GET_REG_ADDR(regName)+4,0x0);

    /*set register by raw driver*/
    /*set*/

    memset(&portMsk,0x0,sizeof(rtk_portmask_t));

    RTK_PORTMASK_PORT_SET(portMsk, 0);
    RTK_PORTMASK_PORT_SET(portMsk, 3);
    RTK_PORTMASK_PORT_SET(portMsk, 6);

    for(port = 0; port < APOLLO_PORTNO  ; port++)
        apollo_raw_vlan_egrKeepPmsk_set(port, portMsk);

    /*get register data*/
    dataMask = 0xFFFFFFF;
    checkData =0x93264C9;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName),&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }

    dataMask = 0x1FFFFF;
    checkData =0x1264C9;
    ioal_mem32_read(HAL_GET_REG_ADDR(regName)+4,&regData);
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d checkData 0x%x regData 0x%x\n",__FUNCTION__,__LINE__,checkData,regData);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}


int32 dal_vlan_raw_4kEntry_test(uint32 caseNo)
{
    apollo_raw_vlan4kentry_t wEntry;
    apollo_raw_vlan4kentry_t rEntry;
    uint32 cnt;

    /*error input check*/
    /*out of range*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.vid = APOLLO_VIDMAX + 1;
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.vid = APOLLO_VIDMAX + 1;
    if( apollo_raw_vlan_4kEntry_get(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.fid_msti = APOLLO_FIDMAX + 1;
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.meteridx = HAL_MAX_NUM_OF_METERING();
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.vbpri = APOLLO_PRIMAX + 1;
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*portmask check*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    RTK_PORTMASK_PORT_SET(wEntry.mbr, APOLLO_PORTNO);
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    RTK_PORTMASK_PORT_SET(wEntry.untag, APOLLO_PORTNO);
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    RTK_PORTMASK_PORT_SET(wEntry.dslMbr, APOLLO_VCPORTNO);
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    RTK_PORTMASK_PORT_SET(wEntry.exMbr, (APOLLO_EXTPORTNO+1));
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.vbpen = RTK_ENABLE_END;
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    wEntry.envlanpol = RTK_ENABLE_END;
    if( apollo_raw_vlan_4kEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*get parameter check*/
    osal_memset(&rEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    rEntry.vid = APOLLO_VIDMAX + 1;
    if( apollo_raw_vlan_4kEntry_get(&rEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
    cnt = 0;
    for(wEntry.vid = 0 ; wEntry.vid<=APOLLO_VIDMAX ; wEntry.vid += 128)
    {
        for(wEntry.fid_msti = 0 ; wEntry.fid_msti<=APOLLO_FIDMAX ; wEntry.fid_msti+=3)
        {
            for(wEntry.meteridx = 0 ; wEntry.meteridx < HAL_MAX_NUM_OF_METERING() ; wEntry.meteridx+=4)
            {
                /*prepare parameter*/
                if(cnt == 50)
                {
                    cnt = 0;
                    /*clear portmask*/
                    osal_memset(&(wEntry.mbr),0x0, sizeof(rtk_portmask_t));
                    osal_memset(&(wEntry.untag),0x0, sizeof(rtk_portmask_t));
                    osal_memset(&(wEntry.dslMbr),0x0, sizeof(rtk_portmask_t));
                    osal_memset(&(wEntry.exMbr),0x0, sizeof(rtk_portmask_t));
                }
                RTK_PORTMASK_PORT_SET(wEntry.untag, (cnt%APOLLO_PORTNO));
                RTK_PORTMASK_PORT_SET(wEntry.mbr, (cnt%APOLLO_PORTNO));
                RTK_PORTMASK_PORT_SET(wEntry.dslMbr, (cnt%APOLLO_VCPORTNO));
                RTK_PORTMASK_PORT_SET(wEntry.exMbr, (cnt%(APOLLO_EXTPORTNO+1)));

                wEntry.vbpri = (cnt%(APOLLO_PRIMAX+1));

                if(cnt%3 == 0)
                    wEntry.vbpen = DISABLED;
                else
                    wEntry.vbpen = ENABLED;

                if(cnt%2 == 0)
                    wEntry.vbpen = DISABLED;
                else
                    wEntry.vbpen = ENABLED;

                if( apollo_raw_vlan_4kEntry_set(&wEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    osal_printf("write:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x] [%4x]\n",
                                  wEntry.vid,
                                  wEntry.fid_msti,
                                  wEntry.envlanpol,
                                  wEntry.vbpen,
                                  wEntry.vbpri,
                                  wEntry.mbr.bits[0],
                                  wEntry.untag.bits[0],
                                  wEntry.dslMbr.bits[0],
                                  wEntry.exMbr.bits[0]
                                  );

                    return RT_ERR_FAILED;
                }

                osal_memset(&rEntry,0x0, sizeof(apollo_raw_vlan4kentry_t));
                rEntry.vid = wEntry.vid;
                if( apollo_raw_vlan_4kEntry_get(&rEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(memcmp(&wEntry,&rEntry,sizeof(apollo_raw_vlan4kentry_t))!=0)
                {
                #if 1
                    osal_printf("write:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x] [%4x]\n",
                                  wEntry.vid,
                                  wEntry.fid_msti,
                                  wEntry.envlanpol,
                                  wEntry.vbpen,
                                  wEntry.vbpri,
                                  wEntry.mbr.bits[0],
                                  wEntry.untag.bits[0],
                                  wEntry.dslMbr.bits[0],
                                  wEntry.exMbr.bits[0]
                                  );

                    osal_printf("read :%4d %2d %2d %2d %2d [%4x] [%4x] [%4x] [%4x]\n",
                                  rEntry.vid,
                                  rEntry.fid_msti,
                                  rEntry.envlanpol,
                                  rEntry.vbpen,
                                  rEntry.vbpri,
                                  rEntry.mbr.bits[0],
                                  rEntry.untag.bits[0],
                                  rEntry.dslMbr.bits[0],
                                  rEntry.exMbr.bits[0]
                                  );
                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }

                cnt++;
            }
        }
    }


    /*NULL pointer check*/
    if( apollo_raw_vlan_4kEntry_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_vlan_4kEntry_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}



int32 dal_vlan_raw_memberConfig_test(uint32 caseNo)
{
    apollo_raw_vlanconfig_t wEntry;
    apollo_raw_vlanconfig_t rEntry;
    uint32 cnt;

    /*error input check*/
    /*out of range*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.index = APOLLO_CVIDXMAX + 1;
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.index = APOLLO_CVIDXMAX + 1;
    if( apollo_raw_vlan_memberConfig_get(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.evid = APOLLO_EVIDMAX + 1;
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.vbpri = APOLLO_PRIMAX + 1;
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*portmask check*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    RTK_PORTMASK_PORT_SET(wEntry.mbr, APOLLO_PORTNO);
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    RTK_PORTMASK_PORT_SET(wEntry.dslMbr, APOLLO_VCPORTNO);
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    RTK_PORTMASK_PORT_SET(wEntry.exMbr, APOLLO_EXTPORTNO+1);
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.vbpen = RTK_ENABLE_END;
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.envlanpol = RTK_ENABLE_END;
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.fid_msti = APOLLO_FIDMAX + 1;
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    wEntry.meteridx = HAL_MAX_NUM_OF_METERING();
    if( apollo_raw_vlan_memberConfig_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
    cnt = 0;
    for(wEntry.index = 0 ; wEntry.index<=APOLLO_CVIDXMAX ; wEntry.index++)
    {
        for(wEntry.fid_msti = 0 ; wEntry.fid_msti<=APOLLO_FIDMAX ; wEntry.fid_msti+=2)
        {
            for(wEntry.meteridx = 0 ; wEntry.meteridx < HAL_MAX_NUM_OF_METERING(); wEntry.meteridx+=4)
            {
                /*prepare parameter*/
                if(cnt == 50)
                {
                    cnt = 0;

                    osal_memset(&(wEntry.mbr),0x0, sizeof(rtk_portmask_t));
                    osal_memset(&(wEntry.dslMbr),0x0, sizeof(rtk_portmask_t));
                    osal_memset(&(wEntry.exMbr),0x0, sizeof(rtk_portmask_t));
                }
                RTK_PORTMASK_PORT_SET(wEntry.mbr, (cnt%APOLLO_PORTNO));
                RTK_PORTMASK_PORT_SET(wEntry.dslMbr, (cnt%APOLLO_VCPORTNO));
                RTK_PORTMASK_PORT_SET(wEntry.exMbr, (cnt%(APOLLO_EXTPORTNO+1)));

                wEntry.vbpri = (cnt%(APOLLO_PRIMAX+1));

                if(cnt%3 == 0)
                    wEntry.vbpen = DISABLED;
                else
                    wEntry.vbpen = ENABLED;

                if(cnt%2 == 0)
                    wEntry.vbpen = DISABLED;
                else
                    wEntry.vbpen = ENABLED;

                if( apollo_raw_vlan_memberConfig_set(&wEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    osal_printf("write:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x]\n",
                                  wEntry.index,
                                  wEntry.fid_msti,
                                  wEntry.envlanpol,
                                  wEntry.vbpen,
                                  wEntry.vbpri,
                                  wEntry.mbr.bits[0],
                                  wEntry.dslMbr.bits[0],
                                  wEntry.exMbr.bits[0]
                                  );

                    return RT_ERR_FAILED;
                }

                osal_memset(&rEntry,0x0, sizeof(apollo_raw_vlanconfig_t));
                rEntry.index = wEntry.index;
                if( apollo_raw_vlan_memberConfig_get(&rEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(memcmp(&wEntry,&rEntry,sizeof(apollo_raw_vlanconfig_t))!=0)
                {
                #if 1
                    osal_printf("write:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x]\n",
                                  wEntry.index,
                                  wEntry.fid_msti,
                                  wEntry.envlanpol,
                                  wEntry.vbpen,
                                  wEntry.vbpri,
                                  wEntry.mbr.bits[0],
                                  wEntry.dslMbr.bits[0],
                                  wEntry.exMbr.bits[0]
                                  );

                    osal_printf("read:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x]\n",
                                  rEntry.index,
                                  rEntry.fid_msti,
                                  rEntry.envlanpol,
                                  rEntry.vbpen,
                                  rEntry.vbpri,
                                  rEntry.mbr.bits[0],
                                  rEntry.dslMbr.bits[0],
                                  rEntry.exMbr.bits[0]
                                  );
                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }

                #if 0
                    osal_printf("write:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x]\n",
                                  wEntry.index,
                                  wEntry.fid_msti,
                                  wEntry.envlanpol,
                                  wEntry.vbpen,
                                  wEntry.vbpri,
                                  wEntry.mbr.bits[0],
                                  wEntry.dslMbr.bits[0],
                                  wEntry.exMbr.bits[0]
                                  );

                    osal_printf("read:%4d %2d %2d %2d %2d [%4x] [%4x] [%4x]\n",
                                  rEntry.index,
                                  rEntry.fid_msti,
                                  rEntry.envlanpol,
                                  rEntry.vbpen,
                                  rEntry.vbpri,
                                  rEntry.mbr.bits[0],
                                  rEntry.dslMbr.bits[0],
                                  rEntry.exMbr.bits[0]
                                  );
                #endif

                cnt++;
            }
        }
    }


    /*NULL pointer check*/
    if( apollo_raw_vlan_memberConfig_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_vlan_memberConfig_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}




int32 dal_vlan_raw_protoAndPbCfg_test(uint32 caseNo)
{
    apollo_raw_protoValnEntryCfg_t wEntry;
    apollo_raw_protoValnEntryCfg_t rEntry;


    /*error input check*/
    /*out of range*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoValnEntryCfg_t));
    wEntry.index = HAL_PROTOCOL_VLAN_IDX_MAX() + 1;
    if( apollo_raw_vlanProtoAndPortBasedEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoValnEntryCfg_t));
    wEntry.index = HAL_PROTOCOL_VLAN_IDX_MAX()+1;
    if( apollo_raw_vlanProtoAndPortBasedEntry_get(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoValnEntryCfg_t));
    wEntry.frameType = RAW_PPVLAN_FRAME_TYPE_END;
    if( apollo_raw_vlanProtoAndPortBasedEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoValnEntryCfg_t));
    wEntry.etherType = 0x10000;
    if( apollo_raw_vlanProtoAndPortBasedEntry_set(&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoValnEntryCfg_t));
    for(wEntry.index = 0 ; wEntry.index <= HAL_PROTOCOL_VLAN_IDX_MAX() ; wEntry.index++)
    {
        for(wEntry.frameType = RAW_PPVLAN_FRAME_TYPE_ETHERNET ; wEntry.frameType<RAW_PPVLAN_FRAME_TYPE_END ; wEntry.frameType++)
        {
            for(wEntry.etherType = 0 ; wEntry.etherType<=0xFFFF ; wEntry.etherType+=0xFF)
            {
                if( apollo_raw_vlanProtoAndPortBasedEntry_set(&wEntry) != RT_ERR_OK)
                {
                    osal_printf("\n%d %d %d %d\n",wEntry.index,wEntry.frameType,wEntry.etherType,HAL_PROTOCOL_VLAN_IDX_MAX());

                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }

                osal_memset(&rEntry,0x0, sizeof(apollo_raw_protoValnEntryCfg_t));
                rEntry.index = wEntry.index;
                if( apollo_raw_vlanProtoAndPortBasedEntry_get(&rEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(memcmp(&wEntry,&rEntry,sizeof(apollo_raw_protoValnEntryCfg_t))!=0)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }




    /*NULL pointer check*/
    if( apollo_raw_vlanProtoAndPortBasedEntry_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_vlanProtoAndPortBasedEntry_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}





int32 dal_vlan_raw_portAndProtoBasedEntry_test(uint32 caseNo)
{
    apollo_raw_protoVlanCfg_t wEntry;
    apollo_raw_protoVlanCfg_t rEntry;
    uint32 port;

    /*error input check*/
    /*out of range*/
    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
    if( apollo_raw_vlan_protoAndPortBasedEntry_set(APOLLO_PORTNO,&wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
    wEntry.index = HAL_PROTOCOL_VLAN_IDX_MAX()+1;
    if( apollo_raw_vlan_portAndProtoBasedEntry_get(0, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
    wEntry.valid = 2;
    if( apollo_raw_vlan_protoAndPortBasedEntry_set(0, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
    wEntry.vlan_idx = (APOLLO_CVIDXMAX+1);
    if( apollo_raw_vlan_protoAndPortBasedEntry_set(0, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
    wEntry.priority = HAL_INTERNAL_PRIORITY_MAX()+1;
    if( apollo_raw_vlan_protoAndPortBasedEntry_set(0, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
    for(port = 0 ; port < APOLLO_PORTNO ; port++)
    {
        for(wEntry.index = 0 ; wEntry.index <= HAL_PROTOCOL_VLAN_IDX_MAX() ; wEntry.index++)
        {
            for(wEntry.vlan_idx = 0 ; wEntry.vlan_idx<= APOLLO_CVIDXMAX ; wEntry.vlan_idx+=2)
            {
                for(wEntry.priority = 0 ; wEntry.priority<=HAL_INTERNAL_PRIORITY_MAX() ; wEntry.priority+=2)
                {
                    if( apollo_raw_vlan_protoAndPortBasedEntry_set(port, &wEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n%d %d %d %d\n",wEntry.index,wEntry.valid,wEntry.vlan_idx,wEntry.priority);

                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                        return RT_ERR_FAILED;
                    }

                    osal_memset(&rEntry,0x0, sizeof(apollo_raw_protoVlanCfg_t));
                    rEntry.index = wEntry.index;
                    if( apollo_raw_vlan_portAndProtoBasedEntry_get(port, &rEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(memcmp(&wEntry,&rEntry,sizeof(apollo_raw_protoVlanCfg_t))!=0)
                    {
                        osal_printf("\nw:%d %d %d %d\n",wEntry.index,
                                                      wEntry.valid,
                                                      wEntry.vlan_idx,
                                                      wEntry.priority);

                        osal_printf("\nr:%d %d %d %d\n",rEntry.index,
                                                      rEntry.valid,
                                                      rEntry.vlan_idx,
                                                      rEntry.priority);

                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }



    /*NULL pointer check*/
    if( apollo_raw_vlan_portAndProtoBasedEntry_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_vlan_protoAndPortBasedEntry_set(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

