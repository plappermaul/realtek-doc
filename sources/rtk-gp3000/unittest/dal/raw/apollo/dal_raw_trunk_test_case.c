#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_trunk.h>
#include <dal/raw/apollo/dal_raw_trunk_test_case.h>

int32 dal_raw_trunk_memberPort_test(uint32 caseNo)
{
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    uint32 mask;

    /*error input check*/
    /*out of range*/
    portmaskW.bits[0] = APOLLO_PORTMASK + 1;
    if( apollo_raw_trunk_memberPort_set(&portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = (0x01 << 2);
    if( apollo_raw_trunk_memberPort_set(&portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = (0x01 << 3);
    if( apollo_raw_trunk_memberPort_set(&portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = (0x01 << 6);
    if( apollo_raw_trunk_memberPort_set(&portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(mask = 0; mask <= APOLLO_PORTMASK; mask++)
    {
        if( ((mask & (0x01 << 2)) == 0) && ((mask & (0x01 << 3)) == 0) && ((mask & (0x01 << 6)) == 0) )
        {
            portmaskW.bits[0] = mask;
            if( apollo_raw_trunk_memberPort_set(&portmaskW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_trunk_memberPort_get(&portmaskR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(portmaskW.bits[0] != portmaskR.bits[0])
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }
    }

    /*null pointer*/
    if( apollo_raw_trunk_memberPort_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trunk_memberPort_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trunk_mode_test(uint32 caseNo)
{
    raw_trunk_mode_t modeW;
    raw_trunk_mode_t modeR;
    /*error input check*/
    /*out of range*/
    if( apollo_raw_trunk_mode_set(RAW_TRUNK_MODE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(modeW = RAW_TRUNK_USER_MODE; modeW < RAW_TRUNK_MODE_END; modeW++)
    {
        if( apollo_raw_trunk_mode_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trunk_mode_get(&modeR) != RT_ERR_OK)
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

    /*null pointer*/
    if( apollo_raw_trunk_mode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trunk_hashMapping_test(uint32 caseNo)
{
    raw_trunk_port_t portW;
    raw_trunk_port_t portR;
    uint32 hash;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trunk_hashMapping_set(RAW_TRUNK_HASH_VALUE_MAX + 1, RAW_TRUNK_PORT_FIRST) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trunk_hashMapping_set(0, RAW_TRUNK_PORT_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(hash = 0; hash <= RAW_TRUNK_HASH_VALUE_MAX; hash++)
    {
        for(portW = RAW_TRUNK_PORT_FIRST; portW < RAW_TRUNK_PORT_END; portW++)
        {
            if( apollo_raw_trunk_hashMapping_set(hash, portW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_trunk_hashMapping_get(hash, &portR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(portW != portR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_trunk_hashMapping_get(hash, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trunk_hashAlgorithm_test(uint32 caseNo)
{
    uint32 hashW;
    uint32 hashR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trunk_hashAlgorithm_set(RAW_TRUNK_HASH_ALL + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(hashW = 0; hashW <= RAW_TRUNK_HASH_ALL; hashW++)
    {
        if( apollo_raw_trunk_hashAlgorithm_set(hashW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trunk_hashAlgorithm_get(&hashR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(hashW != hashR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_trunk_hashAlgorithm_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trunk_flowControl_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trunk_flowControl_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( apollo_raw_trunk_flowControl_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trunk_flowControl_get(&stateR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_trunk_flowControl_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_trunk_flood_test(uint32 caseNo)
{
    raw_trunk_floodMode_t modeW;
    raw_trunk_floodMode_t modeR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_trunk_flood_set(RAW_TRUNK_FLOOD_MODE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(modeW = RAW_TRUNK_NORMAL_FLOOD; modeW < RAW_TRUNK_FLOOD_MODE_END; modeW++)
    {
        if( apollo_raw_trunk_flood_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_trunk_flood_get(&modeR) != RT_ERR_OK)
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

    /*null pointer*/
    if( apollo_raw_trunk_flood_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

