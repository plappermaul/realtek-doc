#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_port_test_case.h>
#include <rtk/port.h>
#include <common/unittest_util.h>


int32 dal_port_adminEnable_test(uint32 caseNo)
{
    rtk_port_t portW;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if(rtk_port_adminEnable_set(HAL_GET_MAX_PORT() + 1, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_adminEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_adminEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    HAL_SCAN_ALL_PORT(portW)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
        {
            if(rtk_port_adminEnable_set(portW, stateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d port:%d\n",__FUNCTION__,__LINE__,portW);
                return RT_ERR_FAILED;
            }

            if(rtk_port_adminEnable_get(portW, &stateR) != RT_ERR_OK)
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

int32 dal_port_cpuPortId_test(uint32 caseNo)
{
    rtk_port_t portR;

    /*null pointer*/
    if(rtk_port_cpuPortId_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */

    if(rtk_port_cpuPortId_get(&portR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(portR != HAL_GET_CPU_PORT())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


int32 dal_port_enhancedFid_test(uint32 caseNo)
{
    rtk_port_t portW;
    rtk_efid_t efidW;
    rtk_efid_t efidR;


    if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }


    /*error input check*/
    /*out of range*/
    if(rtk_port_enhancedFid_set(HAL_GET_MAX_PORT() + 1, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_enhancedFid_set(0, HAL_ENHANCED_FID_MAX() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_enhancedFid_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* Get/set */
    for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    {
        for(efidW = 0; efidW <= HAL_ENHANCED_FID_MAX(); efidW++)
        {
            if(rtk_port_enhancedFid_set(portW, efidW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_port_enhancedFid_get(portW, &efidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(efidW != efidR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }


    return RT_ERR_OK;
}

int32 dal_port_isolation_test(uint32 caseNo)
{
    uint32 portW;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_portmask_t extPortmaskW;
    rtk_portmask_t extPortmaskR;

    RTK_PORTMASK_RESET(portmaskW);
    RTK_PORTMASK_RESET(extPortmaskW);
    if(rtk_port_isolation_set(0, &portmaskW, &extPortmaskW) == RT_ERR_FEATURE_NOT_SUPPORTED)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_port_isolation_set(HAL_GET_MAX_PORT() + 1, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = 0xFF;
    extPortmaskW.bits[0] = 0xFF;
    if(rtk_port_isolation_set(0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolation_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    HAL_SCAN_ALL_PORT(portW)
    {
        for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            for(extPortmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(extPortmaskW); extPortmaskW.bits[0]++)
            {
                if(!RTK_PORTMASK_IS_PORT_SET(extPortmaskW, HAL_GET_EXT_CPU_PORT()))
                {
                    if(rtk_port_isolation_set(0, &portmaskW, &extPortmaskW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(rtk_port_isolation_get(0, &portmaskR, &extPortmaskR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(extPortmaskW, extPortmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_isolationExt_test(uint32 caseNo)
{
    uint32 portW;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_portmask_t extPortmaskW;
    rtk_portmask_t extPortmaskR;


    if(UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    RTK_PORTMASK_RESET(portmaskW);
    RTK_PORTMASK_RESET(extPortmaskW);
    if(rtk_port_isolationExt_set(0, &portmaskW, &extPortmaskW) == RT_ERR_FEATURE_NOT_SUPPORTED)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_port_isolationExt_set(HAL_GET_MAX_EXT_PORT() + 1, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = 0xFF;
    extPortmaskW.bits[0] = 0xFF;
    if(rtk_port_isolationExt_set(0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolationExt_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_EXT_PORT_EX_CPU(portW)
    {
        for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            for(extPortmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(extPortmaskW); extPortmaskW.bits[0]++)
            {
                if(!RTK_PORTMASK_IS_PORT_SET(extPortmaskW, HAL_GET_EXT_CPU_PORT()))
                {
                    if(rtk_port_isolationExt_set(0, &portmaskW, &extPortmaskW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(rtk_port_isolationExt_get(0, &portmaskR, &extPortmaskR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(extPortmaskW, extPortmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_isolationL34_test(uint32 caseNo)
{
    uint32 portW;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_portmask_t extPortmaskW;
    rtk_portmask_t extPortmaskR;

    RTK_PORTMASK_RESET(portmaskW);
    RTK_PORTMASK_RESET(extPortmaskW);


    if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    if(rtk_port_isolationL34_set(0, &portmaskW, &extPortmaskW) == RT_ERR_FEATURE_NOT_SUPPORTED)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_port_isolationL34_set(HAL_GET_MAX_PORT() + 1, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = 0xFF;
    extPortmaskW.bits[0] = 0xFF;
    if(rtk_port_isolationL34_set(0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolationL34_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    {
        for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            for(extPortmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(extPortmaskW); extPortmaskW.bits[0]++)
            {
                if(!RTK_PORTMASK_IS_PORT_SET(extPortmaskW, HAL_GET_EXT_CPU_PORT()))
                {
                    if(rtk_port_isolationL34_set(0, &portmaskW, &extPortmaskW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(rtk_port_isolationL34_get(0, &portmaskR, &extPortmaskR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(extPortmaskW, extPortmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_isolationExtL34_test(uint32 caseNo)
{
    uint32 portW;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_portmask_t extPortmaskW;
    rtk_portmask_t extPortmaskR;


    if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    RTK_PORTMASK_RESET(portmaskW);
    RTK_PORTMASK_RESET(extPortmaskW);
    if(rtk_port_isolationExtL34_set(0, &portmaskW, &extPortmaskW) == RT_ERR_FEATURE_NOT_SUPPORTED)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_port_isolationExtL34_set(HAL_GET_MAX_EXT_PORT() + 1, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = 0xFF;
    extPortmaskW.bits[0] = 0xFF;
    if(rtk_port_isolationExtL34_set(0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolationExtL34_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_EXT_PORT_EX_CPU(portW)
    {
        for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            for(extPortmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(extPortmaskW); extPortmaskW.bits[0]++)
            {
                if(!RTK_PORTMASK_IS_PORT_SET(extPortmaskW, HAL_GET_EXT_CPU_PORT()))
                {
                    if(rtk_port_isolationExtL34_set(0, &portmaskW, &extPortmaskW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(rtk_port_isolationExtL34_get(0, &portmaskR, &extPortmaskR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(extPortmaskW, extPortmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_isolationEntry_test(uint32 caseNo)
{
    uint32 portW;
    rtk_port_isoConfig_t modeW;
    rtk_port_isoConfig_t modeR;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_portmask_t extPortmaskW;
    rtk_portmask_t extPortmaskR;



    if(UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    RTK_PORTMASK_RESET(portmaskW);
    RTK_PORTMASK_RESET(extPortmaskW);
    if(rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, 0, &portmaskW, &extPortmaskW) == RT_ERR_FEATURE_NOT_SUPPORTED)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    
    /*check mod range*/
    /*only apollomp care mode field*/
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)
    {
        if(rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_END, 0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    
    /*port out of range*/
    if(rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, HAL_GET_MAX_PORT() + 1, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*port mask out of range*/
    portmaskW.bits[0] =  1<<(HAL_GET_MAX_PORT()+1);
    extPortmaskW.bits[0] = 0x0;
    if(rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, 0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*ext port mask out of range*/
    portmaskW.bits[0] =  0x0;
    extPortmaskW.bits[0] = 1<<(HAL_GET_MAX_EXT_PORT()+1);
    if(rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, 0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    

    /*null pointer*/
    if(rtk_port_isolationEntry_get(RTK_PORT_ISO_CFG_0, 0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(modeW = RTK_PORT_ISO_CFG_0; modeW < RTK_PORT_ISO_CFG_END; modeW++)
    {
        if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
        {
            if(modeW != RTK_PORT_ISO_CFG_0)
                break;        
        }
        
        //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
        HAL_SCAN_ALL_PORT(portW)
        {
            for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
            {
                for(extPortmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(extPortmaskW); extPortmaskW.bits[0]++)
                {
                    if(portmaskW.bits[0]!=0x0)
                        continue;
                        
                    if(rtk_port_isolationEntry_set(modeW, portW, &portmaskW, &extPortmaskW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(rtk_port_isolationEntry_get(modeW, portW, &portmaskR, &extPortmaskR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(extPortmaskW, extPortmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_port_isolationEntryExt_test(uint32 caseNo)
{
    uint32 portW;
    rtk_port_isoConfig_t modeW;
    rtk_port_isoConfig_t modeR;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_portmask_t extPortmaskW;
    rtk_portmask_t extPortmaskR;


    RTK_PORTMASK_RESET(portmaskW);
    RTK_PORTMASK_RESET(extPortmaskW);
    
    
    
    if(rtk_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_0, 0, &portmaskW, &extPortmaskW) == RT_ERR_FEATURE_NOT_SUPPORTED)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    /*only apollomp care mode field*/
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)
    {
        if(rtk_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_END, 0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*port mask out of range*/
    portmaskW.bits[0] =  1<<(HAL_GET_MAX_PORT()+1);
    extPortmaskW.bits[0] = 0x0;
    if(rtk_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_0, 0, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*ext port mask out of range*/
    portmaskW.bits[0] =  0x0;
    extPortmaskW.bits[0] = 1<<(HAL_GET_MAX_EXT_PORT()+1);    
    if(rtk_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_0, HAL_GET_MAX_EXT_PORT() + 1, &portmaskW, &extPortmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolationEntryExt_get(RTK_PORT_ISO_CFG_0, 0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(modeW = RTK_PORT_ISO_CFG_0; modeW < RTK_PORT_ISO_CFG_END; modeW++)
    {
        if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
        {
            if(modeW != RTK_PORT_ISO_CFG_0)
                break;        
        }

        HAL_SCAN_ALL_EXT_PORT_EX_CPU(portW)
        {
            for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
            {
                if(0x0 != portmaskW.bits[0])
                    continue;

                for(extPortmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(extPortmaskW); extPortmaskW.bits[0]++)
                {
                    if(rtk_port_isolationEntryExt_set(modeW, portW, &portmaskW, &extPortmaskW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(rtk_port_isolationEntryExt_get(modeW, portW, &portmaskR, &extPortmaskR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(RTK_PORTMASK_COMPARE(extPortmaskW, extPortmaskR) != 0)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_isolationCtagPktConfig_test(uint32 caseNo)
{

    rtk_port_isoConfig_t modeW;
    rtk_port_isoConfig_t modeR;

        
    if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_port_isolationCtagPktConfig_set(RTK_PORT_ISO_CFG_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolationCtagPktConfig_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(modeW = RTK_PORT_ISO_CFG_0; modeW < RTK_PORT_ISO_CFG_END; modeW++)
    {
        if(rtk_port_isolationCtagPktConfig_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_port_isolationCtagPktConfig_get(&modeR) != RT_ERR_OK)
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

int32 dal_port_isolationL34PktConfig_test(uint32 caseNo)
{

    rtk_port_isoConfig_t modeW;
    rtk_port_isoConfig_t modeR;

    if(UNITTEST_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_port_isolationL34PktConfig_set(RTK_PORT_ISO_CFG_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_isolationL34PktConfig_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(modeW = RTK_PORT_ISO_CFG_0; modeW < RTK_PORT_ISO_CFG_END; modeW++)
    {
        if(rtk_port_isolationL34PktConfig_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_port_isolationL34PktConfig_get(&modeR) != RT_ERR_OK)
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


int32 dal_port_linkStatus_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_port_linkStatus_t status;
    rtk_port_speed_t speed;
    rtk_port_duplex_t duplex;
    uint32 txfc;
    uint32 rxfc;

    /*error input check*/
    /*out of range*/
    port = HAL_GET_MAX_PORT() + 1;
    if(rtk_port_link_get(port, &status) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_speedDuplex_get(port, &speed, &duplex) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_flowctrl_get(port, &txfc, &rxfc) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_link_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_speedDuplex_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_flowctrl_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        if(rtk_port_link_get(port, &status) != RT_ERR_OK)
        {
            osal_printf("\n %s %d port:%d\n",__FUNCTION__,__LINE__,port);
            return RT_ERR_FAILED;
        }

        if(rtk_port_speedDuplex_get(port, &speed, &duplex) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_port_flowctrl_get(port, &txfc, &rxfc) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_macLocalLoopbackEnable_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if(rtk_port_macLocalLoopbackEnable_set(HAL_GET_MAX_PORT() + 1, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_macLocalLoopbackEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_macLocalLoopbackEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
        {
            if(rtk_port_macLocalLoopbackEnable_set(port, stateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_port_macLocalLoopbackEnable_get(port, &stateR) != RT_ERR_OK)
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

int32 dal_port_specialCongest_test(uint32 caseNo)
{
    rtk_port_t port;
    uint32 secW;
    uint32 secR;

    /*error input check*/
    /*out of range*/
    if(rtk_port_specialCongest_set(HAL_GET_MAX_PORT() + 1, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_specialCongest_set(0, HAL_MAX_SPECIAL_CONGEST_SEC() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_specialCongest_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(secW = 0; secW <= HAL_MAX_SPECIAL_CONGEST_SEC(); secW++)
        {
            if(rtk_port_specialCongest_set(port, secW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_port_specialCongest_get(port, &secR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(secW != secR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_port_specialCongestStatus_test(uint32 caseNo)
{
    rtk_port_t port;
    uint32 statusR;
    /*error input check*/
    /*out of range*/
    if(rtk_port_specialCongestStatus_clear(HAL_GET_MAX_PORT() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_port_specialCongestStatus_get(HAL_GET_MAX_PORT() + 1, &statusR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_port_specialCongestStatus_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        if(rtk_port_specialCongestStatus_get(port, &statusR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_port_specialCongestStatus_clear(port) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}


