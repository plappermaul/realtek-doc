#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_mirror_test_case.h>
#include <rtk/mirror.h>
#include <common/unittest_util.h>


int32 dal_mirrorPort_test(uint32 caseNo)
{
    rtk_port_t portW;
    rtk_port_t portR;
    rtk_portmask_t rxPortmaskW;
    rtk_portmask_t rxPortmaskR;
    rtk_portmask_t txPortmaskW;
    rtk_portmask_t txPortmaskR;
    int ret;


    if( rtk_mirror_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*error input check*/
    /*out of range*/
    
    /*port range*/
    RTK_PORTMASK_RESET(rxPortmaskW);
    RTK_PORTMASK_RESET(txPortmaskW);
    if( rtk_mirror_portBased_set(HAL_GET_MAX_PORT() + 1, &rxPortmaskW, &txPortmaskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*rxPortmaskW port mask*/
    rxPortmaskW.bits[0] = 1<<(HAL_GET_MAX_PORT()+1);
    txPortmaskW.bits[0] = 0x0;
    if( rtk_mirror_portBased_set(0, &rxPortmaskW, &txPortmaskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    /*txPortmaskW port mask*/
    rxPortmaskW.bits[0] = 0x0;
    txPortmaskW.bits[0] = 1<<(HAL_GET_MAX_PORT()+1);
    if( rtk_mirror_portBased_set(0, &rxPortmaskW, &txPortmaskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_mirror_portBased_get(NULL, &rxPortmaskW, &txPortmaskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_mirror_portBased_get(&portR, NULL, &txPortmaskR) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_mirror_portBased_get(&portR, &rxPortmaskR, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(portW)
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    {
        for(rxPortmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(rxPortmaskW); rxPortmaskW.bits[0]++)
        {
            for(txPortmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(txPortmaskW); txPortmaskW.bits[0]++)
            {
                if( (RTK_PORTMASK_GET_PORT_COUNT(rxPortmaskW) == 0) ||
                    (RTK_PORTMASK_GET_PORT_COUNT(txPortmaskW) == 0) ||
                    (RTK_PORTMASK_COMPARE(rxPortmaskW, txPortmaskW) == 0) )
                {
                    if( (!RTK_PORTMASK_IS_PORT_SET(rxPortmaskW, portW)) && (!RTK_PORTMASK_IS_PORT_SET(txPortmaskW, portW)) )
                    {
                        if( (ret=rtk_mirror_portBased_set(portW, &rxPortmaskW, &txPortmaskW)) != RT_ERR_OK )
                        {
                            osal_printf("\n %s %d ret:0x%x  rxPortmaskW:0x%x txPortmaskW:0x%x\n",__FUNCTION__,__LINE__,ret,rxPortmaskW.bits[0],txPortmaskW.bits[0]);
                            return RT_ERR_FAILED;
                        }

                        if( rtk_mirror_portBased_get(&portR, &rxPortmaskR, &txPortmaskR) != RT_ERR_OK )
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if(portR != portW)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if(RTK_PORTMASK_COMPARE(rxPortmaskW, rxPortmaskR) != 0)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if(RTK_PORTMASK_COMPARE(txPortmaskW, txPortmaskR) != 0)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }
                    }
                }
            }
        }
    }


    return RT_ERR_OK;
}

int32 dal_mirrorIsolation_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;


    if( rtk_mirror_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*error input check*/
    /*out of range*/
    if( rtk_mirror_portIso_set(RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_mirror_portIso_get(NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( rtk_mirror_portIso_set(stateW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( rtk_mirror_portIso_get(&stateR) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( stateW != stateR )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}
