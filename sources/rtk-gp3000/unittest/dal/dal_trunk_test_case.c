#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_trunk_test_case.h>
#include <rtk/trunk.h>
#include <common/unittest_util.h>


int32 dal_trunk_Port_test(uint32 caseNo)
{
    int32 ret;
    uint32 tgidW;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;

    /*error input check*/
    /*out of range*/
    if(rtk_trunk_port_set(HAL_TRUNK_GROUP_ID_MAX() + 1, &portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = 0xFF;
    if(rtk_trunk_port_set(0, &portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_trunk_port_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_trunk_portQueueEmpty_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(tgidW = 0; tgidW <= HAL_TRUNK_GROUP_ID_MAX(); tgidW++)
    {
        for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            if( !RTK_PORTMASK_IS_PORT_SET(portmaskW, HAL_GET_CPU_PORT()) &&
                !RTK_PORTMASK_IS_PORT_SET(portmaskW, HAL_GET_PON_PORT()) &&
                !RTK_PORTMASK_IS_PORT_SET(portmaskW, HAL_GET_RGMII_PORT()) )
            {
                if(rtk_trunk_port_set(tgidW, &portmaskW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( (ret = rtk_trunk_port_get(tgidW, &portmaskR)) != RT_ERR_OK)
                {
                    osal_printf("\n %d", ret);
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    if(rtk_trunk_portQueueEmpty_get(&portmaskR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_trunk_distributionAlgorithm_test(uint32 caseNo)
{
    uint32 tgidW;
    uint32 algomaskW;
    uint32 algomaskR;

    /*error input check*/
    /*out of range*/
    algomaskW = TRUNK_DISTRIBUTION_ALGO_SPA_BIT;
    if(rtk_trunk_distributionAlgorithm_set(HAL_TRUNK_GROUP_ID_MAX() + 1, algomaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    algomaskW = TRUNK_DISTRIBUTION_ALGO_MASKALL + 1;
    if(rtk_trunk_distributionAlgorithm_set(0, algomaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_trunk_distributionAlgorithm_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(tgidW = 0; tgidW <= HAL_TRUNK_GROUP_ID_MAX(); tgidW++)
    {
        for(algomaskW = 0x00; algomaskW <= TRUNK_DISTRIBUTION_ALGO_MASKALL; algomaskW++)
        {
            if(rtk_trunk_distributionAlgorithm_set(tgidW, algomaskW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_trunk_distributionAlgorithm_get(tgidW, &algomaskR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(algomaskW != algomaskR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }


    return RT_ERR_OK;
}

int32 dal_trunk_hashMappingTable_test(uint32 caseNo)
{
    uint32 tgidW;
    rtk_trunk_hashVal2Port_t hashW;
    rtk_trunk_hashVal2Port_t hashR;

    /*error input check*/
    /*out of range*/
    osal_memset(&hashW, 0x00, sizeof(rtk_trunk_hashVal2Port_t));
    if(rtk_trunk_hashMappingTable_set(HAL_TRUNK_GROUP_ID_MAX() + 1, &hashW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    hashW.value[0] = 4;
    if(rtk_trunk_hashMappingTable_set(0, &hashW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_trunk_hashMappingTable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(tgidW = 0; tgidW <= HAL_TRUNK_GROUP_ID_MAX(); tgidW++)
    {
        for(hashW.value[0] = 0; hashW.value[0] <= 3; hashW.value[0]++)
        {
            hashW.value[1] = hashW.value[0];
            hashW.value[2] = hashW.value[0];
            hashW.value[3] = hashW.value[0];
            hashW.value[4] = hashW.value[0];
            hashW.value[5] = hashW.value[0];
            hashW.value[6] = hashW.value[0];
            hashW.value[7] = hashW.value[0];
            hashW.value[8] = hashW.value[0];
            hashW.value[9] = hashW.value[0];
            hashW.value[10] = hashW.value[0];
            hashW.value[11] = hashW.value[0];
            hashW.value[12] = hashW.value[0];
            hashW.value[13] = hashW.value[0];
            hashW.value[14] = hashW.value[0];
            hashW.value[15] = hashW.value[0];

            if(rtk_trunk_hashMappingTable_set(0, &hashW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_trunk_hashMappingTable_get(0, &hashR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(osal_memcmp(&hashW, &hashR, sizeof(rtk_trunk_hashVal2Port_t)) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trunk_mode_test(uint32 caseNo)
{
    rtk_trunk_mode_t modeW;
    rtk_trunk_mode_t modeR;

    /*error input check*/
    /*out of range*/
    if(rtk_trunk_mode_set(TRUNK_MODE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_trunk_mode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(modeW = TRUNK_MODE_NORMAL; modeW < TRUNK_MODE_END; modeW++)
    {
        if(rtk_trunk_mode_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_trunk_mode_get(&modeR) != RT_ERR_OK)
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

    return RT_ERR_OK;
}

int32 dal_trunk_trafficPause_test(uint32 caseNo)
{
    uint32 tgidW;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if(rtk_trunk_trafficPause_set(HAL_TRUNK_GROUP_ID_MAX() + 1, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_trunk_trafficPause_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_trunk_trafficPause_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(tgidW = 0; tgidW <= HAL_TRUNK_GROUP_ID_MAX(); tgidW++)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
        {
            if(rtk_trunk_trafficPause_set(tgidW, stateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_trunk_trafficPause_get(tgidW, &stateR) != RT_ERR_OK)
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
    }

    return RT_ERR_OK;
}

int32 dal_trunk_separateType_test(uint32 caseNo)
{
    uint32 tgidW;
    rtk_trunk_separateType_t stateW;
    rtk_trunk_separateType_t stateR;

    /*error input check*/
    /*out of range*/
    if(rtk_trunk_trafficSeparate_set(HAL_TRUNK_GROUP_ID_MAX() + 1, SEPARATE_NONE) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_trunk_trafficSeparate_set(0, SEPARATE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_trunk_trafficSeparate_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(tgidW = 0; tgidW <= HAL_TRUNK_GROUP_ID_MAX(); tgidW++)
    {
        for(stateW = SEPARATE_NONE; stateW < SEPARATE_END; stateW++)
        {
            if(rtk_trunk_trafficSeparate_set(tgidW, stateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_trunk_trafficSeparate_get(tgidW, &stateR) != RT_ERR_OK)
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
    }

    return RT_ERR_OK;
}

