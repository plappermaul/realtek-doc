#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_port.h>
#include <dal/apollo/raw/apollo_raw_l2.h>
#include <ioal/mem32.h>
#include <dal/raw/apollo/dal_raw_isolation_test_case.h>

int32 dal_l2_raw_efid_test(uint32 caseNo)
{

    uint32 portIdx;
    uint32 efidW;
    uint32 efidR;
    uint32 regData;
    uint32 dataMask;
    uint32 checkData;
    /*error input check*/
    /*out of range*/
    if( apollo_raw_l2_efid_set(APOLLO_PORTIDMAX+1, APOLLO_EFIDMAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l2_efid_set(APOLLO_PORTIDMAX, APOLLO_EFIDMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_l2_efid_get(APOLLO_PORTIDMAX+1, &efidR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {
        for (efidW = 0; efidW <= APOLLO_EFIDMAX; efidW++)
        {
            if( apollo_raw_l2_efid_set(portIdx, efidW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l2_efid_get(portIdx, &efidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(efidR != efidW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }
    }

    /*null pointer*/
    if( apollo_raw_l2_efid_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*register access test*/
    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {
        efidW =0x5;
        ioal_mem32_write(HAL_GET_REG_ADDR(L2_EFIDr), 0x0);
        apollo_raw_l2_efid_set(portIdx, efidW);
        ioal_mem32_read(HAL_GET_REG_ADDR((L2_EFIDr)), &regData);
        dataMask = 0x7;
        checkData = 0x5;
        regData = (regData>>(portIdx * 3)) & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    return RT_ERR_OK;





}
int32 dal_isolation_raw_test(uint32 caseNo)
{
    rtk_port_t portIdx;
    uint32 vidxR;
    uint32 vidxW;
    uint32 vcIdx;
    uint32 regData;
    uint32 dataMask;
    uint32 checkData;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_port_isolation_set(APOLLO_PORTIDMAX, APOLLO_CVIDXMAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_port_isolation_set(APOLLO_PORTNO, APOLLO_CVIDXMAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_port_isolationL34_set(APOLLO_PORTIDMAX, APOLLO_CVIDXMAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_port_isolationL34_set(APOLLO_PORTNO, APOLLO_CVIDXMAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_extport_isolation_set(APOLLO_PORTIDMAX, APOLLO_CVIDXMAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_extport_isolation_set(APOLLO_PORTNO, APOLLO_CVIDXMAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_extport_isolationL34_set(APOLLO_PORTIDMAX, APOLLO_CVIDXMAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_extport_isolationL34_set(APOLLO_PORTNO, APOLLO_CVIDXMAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_dslvc_isolation_set(APOLLO_DSLVCMAX, APOLLO_CVIDXMAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_dslvc_isolation_set(APOLLO_DSLVCMAX + 1, APOLLO_CVIDXMAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* get/set test*/
    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {

        for (vidxW = 0; vidxW <= APOLLO_CVIDXMAX; vidxW++)
        {
            if( apollo_raw_port_isolation_set(portIdx, vidxW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_port_isolation_get(portIdx, &vidxR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(vidxW != vidxR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_port_isolationL34_set(portIdx, vidxW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_port_isolationL34_get(portIdx, &vidxR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(vidxW != vidxR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }

    }

    for (portIdx = 0; portIdx <= APOLLO_EXTPORTNOMAX; portIdx++)
    {

        for (vidxW = 0; vidxW <= APOLLO_CVIDXMAX; vidxW++)
        {

            if( apollo_raw_extport_isolation_set(portIdx, vidxW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_extport_isolation_get(portIdx, &vidxR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(vidxW != vidxR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_extport_isolationL34_set(portIdx, vidxW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_extport_isolationL34_get(portIdx, &vidxR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(vidxW != vidxR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }
    }

    for (vcIdx = 0; vcIdx <= APOLLO_DSLVCMAX ; vcIdx++)
    {

        for (vidxW = 0; vidxW <= APOLLO_CVIDXMAX; vidxW++)
        {
            if( apollo_raw_dslvc_isolation_set(vcIdx, vidxW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_dslvc_isolation_get(vcIdx, &vidxR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(vidxW != vidxR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }

     }

    /*null pointer*/

    if( apollo_raw_port_isolation_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_port_isolationL34_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_extport_isolation_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_extport_isolationL34_get(APOLLO_PORTIDMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_dslvc_isolation_get(APOLLO_DSLVCMAX, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /*register access test*/


    for (portIdx = 0; portIdx < APOLLO_PORTNO; portIdx++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(PISO_P_CTRLr) + (portIdx>=0x6?4:0), 0x0);
        apollo_raw_port_isolation_set(portIdx, APOLLO_CVIDXMAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(PISO_P_CTRLr) + (portIdx>=0x6?4:0),&regData);
        dataMask = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        checkData = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(PISO_P_L34_CTRLr) + (portIdx>=0x6?4:0), 0x0);
        apollo_raw_port_isolationL34_set(portIdx, APOLLO_CVIDXMAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(PISO_P_L34_CTRLr) +  (portIdx>=0x6?4:0),&regData);
        dataMask = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        checkData = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for (portIdx = 0; portIdx <= APOLLO_EXTPORTNOMAX; portIdx++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(PISO_EXT_CTRLr) + (portIdx>=0x6?4:0), 0x0);
        apollo_raw_extport_isolation_set(portIdx, APOLLO_CVIDXMAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(PISO_EXT_CTRLr) + (portIdx>=0x6?4:0),&regData);
        dataMask = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        checkData = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(PISO_EXT_L34_CTRLr) + (portIdx>=0x6?4:0), 0x0);
        apollo_raw_extport_isolationL34_set(portIdx, APOLLO_CVIDXMAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(PISO_EXT_L34_CTRLr) + (portIdx>=0x6?4:0),&regData);
        dataMask = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        checkData = APOLLO_CVIDXMAX<<((portIdx%6)*5);
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for (vcIdx = 0; vcIdx <= APOLLO_DSLVCMAX ; vcIdx++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(PISO_VC_CTRLr) + 4*(vcIdx/6), 0x0);
        apollo_raw_dslvc_isolation_set(vcIdx, APOLLO_CVIDXMAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(PISO_VC_CTRLr) + 4*(vcIdx/6),&regData);
        dataMask = APOLLO_CVIDXMAX<<((vcIdx%6)*5);
        checkData = APOLLO_CVIDXMAX<<((vcIdx%6)*5);
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

