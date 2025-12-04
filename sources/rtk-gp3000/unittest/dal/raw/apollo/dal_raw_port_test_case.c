#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_port.h>
#include <dal/apollo/raw/apollo_raw_congest.h>
#include <ioal/mem32.h>
#include <dal/raw/apollo/dal_raw_port_test_case.h>


int32 dal_port_raw_forceDmp_test(uint32 caseNo)
{

    rtk_enable_t stateR;
    rtk_enable_t stateW;
    rtk_portmask_t maskW;
    rtk_portmask_t maskR;
    uint32 portIdx;
    uint32 maskShift;
    uint32 regData;
    uint32 dataMask;
    uint32 checkData;
    /*error input check*/
    /*out of range*/
    if( apollo_raw_port_forceDmp_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    maskW.bits[0] = APOLLO_PORTMASK;
    if( apollo_raw_port_forceDmpMask_set(APOLLO_PORTIDMAX +1 , maskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    maskW.bits[0] = APOLLO_PORTMASK | 0x80;
    if( apollo_raw_port_forceDmpMask_set(APOLLO_PORTIDMAX, maskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for (stateW = 0; stateW < RTK_ENABLE_END; stateW++)
    {
        if( apollo_raw_port_forceDmp_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_port_forceDmp_get(&stateR) != RT_ERR_OK)
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

    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {
        maskW.bits[0] = APOLLO_PORTIDMAX;
        if( apollo_raw_port_forceDmpMask_set(portIdx, maskW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_port_forceDmpMask_get(portIdx, &maskR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(maskR.bits[0] != maskW.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        maskW.bits[0] = 0;
        if( apollo_raw_port_forceDmpMask_set(portIdx, maskW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_port_forceDmpMask_get(portIdx, &maskR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(maskR.bits[0] != maskW.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        for (maskShift = 0; maskShift < APOLLO_PORTNO; maskShift++)
        {
            maskW.bits[0] = 1<<maskShift;
            if( apollo_raw_port_forceDmpMask_set(portIdx, maskW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_port_forceDmpMask_get(portIdx, &maskR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(maskR.bits[0] != maskW.bits[0])
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }
    }

    /*null pointer*/
    if( apollo_raw_port_forceDmp_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_port_forceDmpMask_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*register access test*/
    ioal_mem32_write(HAL_GET_REG_ADDR(EN_FORCE_P_DMPr), 0x0);
    apollo_raw_port_forceDmp_set(ENABLED);
    ioal_mem32_read(HAL_GET_REG_ADDR(EN_FORCE_P_DMPr),&regData);
    dataMask = 0x1;
    checkData = 0x1;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if (regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {

        ioal_mem32_write(HAL_GET_REG_ADDR(FORCE_P_DMPr) + 4*(portIdx>>2), 0x0);
        maskW.bits[0] = APOLLO_PMSKMAX;
        apollo_raw_port_forceDmpMask_set(portIdx, maskW);
        ioal_mem32_read(HAL_GET_REG_ADDR(FORCE_P_DMPr) + 4*(portIdx>>2),&regData);
        dataMask = 0x7f;
        checkData = 0x7f<<(7*(portIdx&0x3));
        /*mask out reserve bits*/
        regData = regData & dataMask<<(7*(portIdx&0x3));
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    return RT_ERR_OK;

}
int32 dal_port_raw_congest_test(uint32 caseNo)
{
    uint32 occur;
    uint32 sustTmrW;
    uint32 sustTmrR;
    uint32 cgstTmr;
    uint32 portIdx;
    uint32 regData;
    uint32 wData;
    uint32 dataMask;
    uint32 checkData;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_sc_cgstInd_reset(APOLLO_PORTIDMAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_sc_cgstInd_get(APOLLO_PORTIDMAX +1, &occur) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    sustTmrW = 0;
    if( apollo_raw_sc_sustTmr_set(APOLLO_PORTIDMAX +1, sustTmrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_sc_sustTmr_get(APOLLO_PORTIDMAX +1, &sustTmrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_sc_cgstTmr_get(APOLLO_PORTIDMAX +1, &cgstTmr) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {
        for (sustTmrW = 0; sustTmrW <= APOLLO_RAW_SC_SUSTEN_TIMER_MAX; sustTmrW++)
        {
            if( apollo_raw_sc_sustTmr_set(portIdx, sustTmrW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_sc_sustTmr_get(portIdx, &sustTmrR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(sustTmrR != sustTmrW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }
    }

    /*null pointer*/
    if( apollo_raw_sc_cgstInd_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_sc_sustTmr_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_sc_cgstTmr_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*register access test*/
    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(SC_P_CTRL_0r) + 0x400*portIdx, 0x0);
        apollo_raw_sc_sustTmr_set(portIdx, APOLLO_RAW_SC_SUSTEN_TIMER_MAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(SC_P_CTRL_0r) + 0x400*portIdx,&regData);
        dataMask = 0xf;
        checkData = 0xf;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}

