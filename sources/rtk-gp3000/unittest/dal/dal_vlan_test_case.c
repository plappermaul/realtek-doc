#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_vlan_test_case.h>
#include <rtk/vlan.h>
#include <common/unittest_util.h>

static uint32 vid_arr[2] = { 1, RTK_VLAN_ID_MAX-1};

int32 dal_vlan_test(uint32 caseNo)
{
    uint32 dataR;
    uint32 dataW;
    rtk_enable_t enableR;
    rtk_enable_t enableW;
    rtk_vlan_t vidR;
    rtk_vlan_t vidW;
    rtk_port_t port;
    int32 ret;

    rtk_portmask_t pmsk;
    rtk_portmask_t extpmsk;
    rtk_portmask_t memPmskW;
    rtk_portmask_t untagPmskW;
    rtk_portmask_t memPmskR;
    rtk_portmask_t untagPmskR;

    HAL_GET_ALL_PORTMASK(pmsk);

    if( rtk_vlan_init() != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*must initial for test*/
    /* Bound Cehck */
    if( rtk_vlan_create(RTK_EXT_VLAN_ID_MAX + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroy(RTK_EXT_VLAN_ID_MAX + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroyAll(RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Create / Destroy / DestroyAll */
    if( rtk_vlan_create(1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroy(1) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroy(1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_create(1) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_create(2) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_create(3) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroyAll(DISABLED) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroy(1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroy(2) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_destroy(3) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* VLAN configuration test*/
    memPmskW.bits[0]    = 0x00;
    untagPmskW.bits[0]  = 0x00;
    if( rtk_vlan_port_set(1, &memPmskW, &untagPmskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_create(1) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_port_set(1, &memPmskW, &untagPmskW) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Boundary Test */
    memPmskW.bits[0]    = 0x00;
    untagPmskW.bits[0]  = pmsk.bits[0] + 1;
    if( rtk_vlan_port_set(1, &memPmskW, &untagPmskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memPmskW.bits[0]    = pmsk.bits[0] + 1;
    untagPmskW.bits[0]  = 0x00;
    if( rtk_vlan_port_set(1, &memPmskW, &untagPmskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Set/Get test */
    for(memPmskW.bits[0] = 0; memPmskW.bits[0] <= pmsk.bits[0]; memPmskW.bits[0]++)
    {
        for(untagPmskW.bits[0] = 0; untagPmskW.bits[0] <= pmsk.bits[0]; untagPmskW.bits[0]++)
        {
            if( rtk_vlan_port_set(1, &memPmskW, &untagPmskW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_port_get(1, &memPmskR, &untagPmskR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(RTK_PORTMASK_COMPARE(memPmskW, memPmskR) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(RTK_PORTMASK_COMPARE(untagPmskW, untagPmskR) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* PVID */
    if( rtk_vlan_portPvid_set(HAL_GET_MAX_PORT() + 1, 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portPvid_set(1, RTK_EXT_VLAN_ID_MAX + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Create VLAN for test */
    for(vidW = 2; vidW <= 10; vidW++)
    {
        if( rtk_vlan_create(vidW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        for(vidW = 1; vidW <= 10; vidW++)
        {
            if( rtk_vlan_portPvid_set(port, vidW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_portPvid_get(port, &vidR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(vidW != vidR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* EXT port GET/SET*/
    HAL_GET_ALL_EXT_PORTMASK(extpmsk);
    RTK_PORTMASK_PORT_CLEAR(extpmsk, HAL_GET_EXT_CPU_PORT());

    memPmskW.bits[0]    = extpmsk.bits[0] << 2;
    if( rtk_vlan_extPort_set(1, &memPmskW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Set/Get test */
    for(memPmskW.bits[0] = 0; memPmskW.bits[0] <= extpmsk.bits[0]; memPmskW.bits[0]++)
    {
        ret = rtk_vlan_extPort_set(1, &memPmskW);
                
        if( ret != RT_ERR_OK && ret != RT_ERR_DRIVER_NOT_FOUND)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ret = rtk_vlan_extPort_get(1, &memPmskR);

        if( ret != RT_ERR_OK && ret != RT_ERR_DRIVER_NOT_FOUND)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        if(ret == RT_ERR_OK)
        {
            if(RTK_PORTMASK_COMPARE(memPmskW, memPmskR) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* EXT port PVID */
    if( rtk_vlan_extPortPvid_set(HAL_GET_MAX_EXT_PORT() + 1, 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_extPortPvid_set(1, RTK_EXT_VLAN_ID_MAX + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    HAL_SCAN_ALL_EXT_PORT_EX_CPU(port)
    {
        if( rtk_vlan_extPortPvid_set(port, 0) == RT_ERR_DRIVER_NOT_FOUND )
            break;
        for(vidW = 1; vidW <= 10; vidW++)
        {
            if( rtk_vlan_extPortPvid_set(port, vidW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_extPortPvid_get(port, &vidR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(vidW != vidR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_Fid_test(uint32 caseNo)
{
    rtk_fid_t fidW;
    rtk_fid_t fidR;
    rtk_fidMode_t modeW;
    rtk_fidMode_t modeR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_fid_set(RTK_EXT_VLAN_ID_MAX + 1, 0) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_fid_set(1, HAL_VLAN_FID_MAX() + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_fidMode_set(RTK_EXT_VLAN_ID_MAX + 1, 0) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_fidMode_set(1, VLAN_FID_MODE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_fid_get(1, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_fidMode_get(1, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(fidW = 0; fidW <= HAL_VLAN_FID_MAX(); fidW++)
    {
        if( rtk_vlan_fid_set(1, fidW) == RT_ERR_DRIVER_NOT_FOUND )
            break;    
        
        if( rtk_vlan_fid_set(1, fidW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( rtk_vlan_fid_get(1, &fidR) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(fidW != fidR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(modeW = VLAN_FID_IVL; modeW < VLAN_FID_MODE_END; modeW++)
    {
        if( rtk_vlan_fidMode_set(1, modeW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( rtk_vlan_fidMode_get(1, &modeR) != RT_ERR_OK )
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

int32 dal_vlan_portFid_test(uint32 caseNo)
{
    uint32 portW;
    rtk_fid_t fidW;
    rtk_fid_t fidR;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_portFid_set(RTK_EXT_VLAN_ID_MAX + 1, DISABLED, 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portFid_set(1, RTK_ENABLE_END, 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portFid_set(1, DISABLED, HAL_VLAN_FID_MAX() + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portFid_get(1, DISABLED, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    HAL_SCAN_ALL_PORT(portW)
    {
        if( rtk_vlan_portFid_set(portW, DISABLED, 0) == RT_ERR_DRIVER_NOT_FOUND )
            break;
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
        {
            for(fidW = 0; fidW <= HAL_VLAN_FID_MAX(); fidW++)
            {
                if( rtk_vlan_portFid_set(portW, stateW, fidW) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( rtk_vlan_portFid_get(portW, &stateR, &fidR) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(fidW != fidR)
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
    }

    return RT_ERR_OK;
}

int32 dal_vlan_portPriority_test(uint32 caseNo)
{
    uint32 portW;
    rtk_pri_t priW;
    rtk_pri_t priR;

    if( rtk_vlan_portPriority_set(0, 0) == RT_ERR_DRIVER_NOT_FOUND )
    {
        /* unsupport function, return OK */
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_portPriority_set(HAL_GET_MAX_PORT() + 1, 0) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portPriority_set(1, HAL_INTERNAL_PRIORITY_MAX() + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portPriority_get(1, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    HAL_SCAN_ALL_PORT(portW)
    {
        for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
        {
            if( rtk_vlan_portPriority_set(portW, priW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_portPriority_get(portW, &priR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(priW != priR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_accpetFrameType_test(uint32 caseNo)
{
    uint32 portW;
    rtk_vlan_acceptFrameType_t typeW;
    rtk_vlan_acceptFrameType_t typeR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_portAcceptFrameType_set(HAL_GET_MAX_PORT() + 1, ACCEPT_FRAME_TYPE_ALL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portAcceptFrameType_set(0, ACCEPT_FRAME_TYPE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portAcceptFrameType_get(0, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    HAL_SCAN_ALL_PORT(portW)
    {
        for(typeW = ACCEPT_FRAME_TYPE_ALL; typeW < ACCEPT_FRAME_TYPE_END; typeW++)
        {
            if( rtk_vlan_portAcceptFrameType_set(portW, typeW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_portAcceptFrameType_get(portW, &typeR) != RT_ERR_OK )
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
    }

    return RT_ERR_OK;
}

int32 dal_vlan_portEgrTagKeepType_test(uint32 caseNo)
{
    uint32 portW;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    rtk_vlan_tagKeepType_t typeW;
    rtk_vlan_tagKeepType_t typeR;


    if( rtk_vlan_portEgrTagKeepType_set(0, &portmaskW, TAG_KEEP_TYPE_CONTENT) == RT_ERR_DRIVER_NOT_FOUND )
    {
        /* unsupport function, return OK */
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    RTK_PORTMASK_PORT_SET(portmaskW, 1);
    if( rtk_vlan_portEgrTagKeepType_set(HAL_GET_MAX_PORT() + 1, &portmaskW, TAG_KEEP_TYPE_CONTENT) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portmaskW.bits[0] = (0x01 << (HAL_GET_MAX_PORT() + 1));
    if( rtk_vlan_portEgrTagKeepType_set(0, &portmaskW, TAG_KEEP_TYPE_CONTENT) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    RTK_PORTMASK_PORT_SET(portmaskW, 1);
    if( rtk_vlan_portEgrTagKeepType_set(0, &portmaskW, TAG_KEEP_TYPE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portEgrTagKeepType_get(0, NULL, &typeR) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portEgrTagKeepType_get(0, &portmaskR, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(portW = 0; portW <= HAL_GET_MAX_PORT(); portW++)
    HAL_SCAN_ALL_PORT(portW)
    {
        for(portmaskW.bits[0] = 0x01; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            for(typeW = TAG_KEEP_TYPE_CONTENT; typeW < TAG_KEEP_TYPE_END; typeW++)
            {
                if( rtk_vlan_portEgrTagKeepType_set(portW, &portmaskW, typeW) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( rtk_vlan_portEgrTagKeepType_get(portW, &portmaskR, &typeR) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
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
        }
    }

    return RT_ERR_OK;
}


int32 dal_vlan_portIgrFilterEnable_test(uint32 caseNo)
{
    uint32 portW;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_portIgrFilterEnable_set(HAL_GET_MAX_PORT() + 1, DISABLED) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portIgrFilterEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portIgrFilterEnable_get(0, NULL) == RT_ERR_OK )
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
            if( rtk_vlan_portIgrFilterEnable_set(portW, stateW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_portIgrFilterEnable_get(portW, &stateR) != RT_ERR_OK )
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

int32 dal_vlan_misc_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_cfiKeepEnable_set(RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_cfiKeepEnable_get(NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( rtk_vlan_cfiKeepEnable_set(stateW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( rtk_vlan_cfiKeepEnable_get(&stateR) != RT_ERR_OK )
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

    return RT_ERR_OK;
}

int32 dal_vlan_leaky_test(uint32 caseNo)
{
    int32 ret;
    rtk_leaky_type_t typeW;
    rtk_enable_t stateW;
    rtk_enable_t stateR;


    if( rtk_vlan_leaky_set(LEAKY_END, DISABLED) == RT_ERR_DRIVER_NOT_FOUND )
        return RT_ERR_OK;    
        
    /*error input check*/
    /*out of range*/
    if( rtk_vlan_leaky_set(LEAKY_END, DISABLED) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_leaky_set(LEAKY_BRG_GROUP, RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_leaky_get(LEAKY_BRG_GROUP, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(typeW = LEAKY_BRG_GROUP; typeW < LEAKY_END; typeW++)
    {
        if(typeW != LEAKY_IPMULTICAST)
        {
            for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
            {
                if( (ret = rtk_vlan_leaky_set(typeW, stateW)) != RT_ERR_OK )
                {
                    osal_printf("\n %d %d (0x%X)", typeW, stateW, ret);
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( rtk_vlan_leaky_get(typeW, &stateR) != RT_ERR_OK )
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
    }

    return RT_ERR_OK;
}

int32 dal_vlan_portLeaky_test(uint32 caseNo)
{
    int32 ret;
    rtk_leaky_type_t typeW;
    rtk_enable_t stateW;
    rtk_enable_t stateR;
    rtk_port_t port;


    if( rtk_vlan_portLeaky_set(0, LEAKY_BRG_GROUP, DISABLED) == RT_ERR_DRIVER_NOT_FOUND )
    {
        return RT_ERR_OK;
    }
    /*error input check*/
    /*out of range*/
    if( rtk_vlan_portLeaky_set(HAL_GET_MAX_PORT() + 1, LEAKY_BRG_GROUP, DISABLED) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portLeaky_set(0, LEAKY_END, DISABLED) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portLeaky_set(0, LEAKY_BRG_GROUP, RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portLeaky_get(0, LEAKY_BRG_GROUP, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(port = 0; port <= HAL_GET_MAX_PORT(); port++)
    HAL_SCAN_ALL_PORT(port)
    {
        for(typeW = LEAKY_BRG_GROUP; typeW < LEAKY_END; typeW++)
        {
            if(typeW == LEAKY_IPMULTICAST)
            {
                for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
                {
                    if( (ret = rtk_vlan_portLeaky_set(port, typeW, stateW)) != RT_ERR_OK )
                    {
                        osal_printf("\n %d %d (0x%X)", typeW, stateW, ret);
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if( rtk_vlan_portLeaky_get(port, typeW, &stateR) != RT_ERR_OK )
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
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_keepType_test(uint32 caseNo)
{
    int32 ret;
    rtk_vlan_keep_type_t typeW;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_keepType_set(KEEP_END, DISABLED) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_keepType_set(KEEP_BRG_GROUP, RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_keepType_get(KEEP_BRG_GROUP, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(typeW = KEEP_BRG_GROUP; typeW < KEEP_END; typeW++)
    {
        if( (typeW != KEEP_IPMULTICAST) && (typeW != KEEP_IGMP) )
        {
            for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
            {
                if( (ret = rtk_vlan_keepType_set(typeW, stateW)) != RT_ERR_OK )
                {
                    osal_printf("\n %d %d (0x%X)", typeW, stateW, ret);
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( rtk_vlan_keepType_get(typeW, &stateR) != RT_ERR_OK )
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
    }

    return RT_ERR_OK;
}

int32 dal_vlan_portProtoVlan_test(uint32 caseNo)
{
    uint32 port;
    uint32 index;
    uint32      valid;
    rtk_vlan_t  vid;
    rtk_pri_t   pri;
    uint32      dei;
    rtk_vlan_protoVlanCfg_t cfgW;
    rtk_vlan_protoVlanCfg_t cfgR;

    /*error input check*/
    /*out of range*/
    osal_memset(&cfgW, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    cfgW.vid = 1;
    if( rtk_vlan_portProtoVlan_set(HAL_GET_MAX_PORT() + 1, 0, &cfgW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_portProtoVlan_set(0, HAL_PROTOCOL_VLAN_IDX_MAX() + 1, &cfgW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfgW, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    cfgW.valid = 2;
    cfgW.vid = 1;
    if( rtk_vlan_portProtoVlan_set(0, 0, &cfgW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfgW, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    cfgW.valid = 1;
    cfgW.vid = RTK_EXT_VLAN_ID_MAX + 1;
    if( rtk_vlan_portProtoVlan_set(0, 0, &cfgW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfgW, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    cfgW.valid = 1;
    cfgW.vid = 1;
    cfgW.pri = HAL_INTERNAL_PRIORITY_MAX() + 1;
    if( rtk_vlan_portProtoVlan_set(0, 0, &cfgW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfgW, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    cfgW.valid = 1;
    cfgW.vid = 1;
    cfgW.pri = HAL_INTERNAL_PRIORITY_MAX();
    cfgW.dei = 2;
    if( rtk_vlan_portProtoVlan_set(0, 0, &cfgW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_portProtoVlan_get(0, 0, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(port = 0; port <= HAL_GET_MAX_PORT(); port ++)
    HAL_SCAN_ALL_PORT(port)
    {
        for(index = 0; index <= HAL_PROTOCOL_VLAN_IDX_MAX(); index++)
        {
             for(valid = 0; valid <= 1; valid++)
             {
                 for(pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(); pri++)
                 {
                     for(dei = 0; dei <= 0; dei++)
                     {
                         osal_memset(&cfgW, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
                         cfgW.valid = valid;
                         cfgW.vid = 1;
                         cfgW.pri = pri;
                         cfgW.dei = dei;

                         if( rtk_vlan_portProtoVlan_set(port, index, &cfgW) != RT_ERR_OK )
                         {
                             osal_printf(" %d %d %d %d %d %d", port, index, cfgW.valid, cfgW.vid, cfgW.pri, cfgW.dei);
                             osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                             return RT_ERR_FAILED;
                         }

                         if( rtk_vlan_portProtoVlan_get(port, index, &cfgR) != RT_ERR_OK )
                         {
                             osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                             return RT_ERR_FAILED;
                         }

                         if(osal_memcmp(&cfgW, &cfgR, sizeof(rtk_vlan_protoVlanCfg_t)) != 0)
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

int32 dal_vlan_priority_test(uint32 caseNo)
{
    uint32 vidx;
    rtk_pri_t priW;
    rtk_pri_t priR;

    
    if( rtk_vlan_priority_set(0, 0) == RT_ERR_DRIVER_NOT_FOUND )
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;    
    }    
    /*error input check*/
    /*out of range*/
    if( rtk_vlan_priority_set(RTK_EXT_VLAN_ID_MAX + 1, 0) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_priority_set(1, HAL_INTERNAL_PRIORITY_MAX() + 1) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_priority_get(1, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    if( rtk_vlan_create(5000) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(vidx = 0; vidx <= 1; vidx++)
    {
        for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
        {
            if( rtk_vlan_priority_set(vid_arr[vidx], priW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_priority_get(vid_arr[vidx], &priR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(priW != priR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    if( rtk_vlan_destroy(5000) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_vlan_priorityEnable_test(uint32 caseNo)
{
    uint32 vidx;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    
    if( rtk_vlan_priorityEnable_set(1, DISABLED) == RT_ERR_DRIVER_NOT_FOUND )
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_priorityEnable_set(RTK_EXT_VLAN_ID_MAX + 1, DISABLED) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_priorityEnable_set(1, RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_priorityEnable_get(1, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    if( rtk_vlan_create(5000) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(vidx = 0; vidx <= 1; vidx++)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
        {
            if( rtk_vlan_priorityEnable_set(vid_arr[vidx], stateW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_priorityEnable_get(vid_arr[vidx], &stateR) != RT_ERR_OK )
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

    if( rtk_vlan_destroy(5000) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_vlan_protoGroup_test(uint32 caseNo)
{
    uint32 index;
    rtk_vlan_protoVlan_frameType_t frametype;
    uint32 framevalue;
    rtk_vlan_protoGroup_t groupW;
    rtk_vlan_protoGroup_t groupR;

    /*error input check*/
    /*out of range*/
    osal_memset(&groupW, 0x00, sizeof(rtk_vlan_protoGroup_t));
    groupW.frametype = FRAME_TYPE_ETHERNET;
    groupW.framevalue = 0x1234;
    if( rtk_vlan_protoGroup_set(HAL_PROTOCOL_VLAN_IDX_MAX() + 1, &groupW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&groupW, 0x00, sizeof(rtk_vlan_protoGroup_t));
    groupW.frametype = FRAME_TYPE_END;
    groupW.framevalue = 0x1234;
    if( rtk_vlan_protoGroup_set(0, &groupW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&groupW, 0x00, sizeof(rtk_vlan_protoGroup_t));
    groupW.frametype = FRAME_TYPE_SNAP8021H;
    groupW.framevalue = 0x1234;
    if( rtk_vlan_protoGroup_set(0, &groupW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&groupW, 0x00, sizeof(rtk_vlan_protoGroup_t));
    groupW.frametype = FRAME_TYPE_SNAPOTHER;
    groupW.framevalue = 0x1234;
    if( rtk_vlan_protoGroup_set(0, &groupW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&groupW, 0x00, sizeof(rtk_vlan_protoGroup_t));
    groupW.frametype = FRAME_TYPE_ETHERNET;
    groupW.framevalue = 0x12345678;
    if( rtk_vlan_protoGroup_set(0, &groupW) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_protoGroup_get(0, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(index = 0; index <= HAL_PROTOCOL_VLAN_IDX_MAX(); index++)
    {
        for(frametype = FRAME_TYPE_ETHERNET; frametype < FRAME_TYPE_END; frametype++)
        {
            if( (frametype != FRAME_TYPE_SNAP8021H) && (frametype != FRAME_TYPE_SNAPOTHER) )
            {
                for(framevalue = 0x1234; framevalue <= 0xFFFF; framevalue+= 0x1234)
                {
                    groupW.frametype = frametype;
                    groupW.framevalue = framevalue;

                    if( rtk_vlan_protoGroup_set(index, &groupW) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if( rtk_vlan_protoGroup_get(index, &groupR) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if(osal_memcmp(&groupW, &groupR, sizeof(rtk_vlan_protoGroup_t)) != 0)
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

int32 dal_vlan_reservedVidAction_test(uint32 caseNo)
{
    rtk_vlan_resVidAction_t vid0_actW;
    rtk_vlan_resVidAction_t vid0_actR;
    rtk_vlan_resVidAction_t vid4095_actW;
    rtk_vlan_resVidAction_t vid4095_actR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_reservedVidAction_set(RESVID_ACTION_UNTAG, RESVID_ACTION_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_reservedVidAction_set(RESVID_ACTION_END, RESVID_ACTION_UNTAG) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_reservedVidAction_get(&vid0_actR, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_reservedVidAction_get(NULL, &vid4095_actR) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(vid0_actW = RESVID_ACTION_UNTAG; vid0_actW < RESVID_ACTION_END; vid0_actW++)
    {
        for(vid4095_actW = RESVID_ACTION_UNTAG; vid4095_actW < RESVID_ACTION_END; vid4095_actW++)
        {
            if( rtk_vlan_reservedVidAction_set(vid0_actW, vid4095_actW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_reservedVidAction_get(&vid0_actR, &vid4095_actR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(vid0_actW != vid0_actR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(vid4095_actW != vid4095_actR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_stg_test(uint32 caseNo)
{
    uint32 vidx;
    rtk_stg_t stgW;
    rtk_stg_t stgR;

    if( rtk_vlan_stg_set(0, 0) == RT_ERR_DRIVER_NOT_FOUND )
    {
        return RT_ERR_OK;    
    }
    
    /*error input check*/
    /*out of range*/
    if( rtk_vlan_stg_set(RTK_EXT_VLAN_ID_MAX + 1, 0) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_stg_set(1, HAL_MAX_NUM_OF_MSTI()) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_stg_get(1, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    if( rtk_vlan_create(RTK_VLAN_ID_MAX-1) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(vidx = 0; vidx <= 1; vidx++)
    {
        for(stgW = 0; stgW < HAL_MAX_NUM_OF_MSTI(); stgW++)
        {
            if( rtk_vlan_stg_set(vid_arr[vidx], stgW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_stg_get(vid_arr[vidx], &stgR) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(stgW != stgR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    if( rtk_vlan_destroy(RTK_VLAN_ID_MAX-1) != RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_vlan_tagMode_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_vlan_tagMode_t modeW;
    rtk_vlan_tagMode_t modeR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_tagMode_set(HAL_GET_MAX_PORT() + 1, VLAN_TAG_MODE_ORIGINAL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( rtk_vlan_tagMode_set(0, VLAN_TAG_MODE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_tagMode_get(0, NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    //for(port = 0; port <= HAL_GET_MAX_PORT(); port++)
    HAL_SCAN_ALL_PORT(port)
    {
        for(modeW = VLAN_TAG_MODE_ORIGINAL; modeW < VLAN_TAG_MODE_END; modeW++)
        {
            if( rtk_vlan_tagMode_set(port, modeW) != RT_ERR_OK )
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( rtk_vlan_tagMode_get(port, &modeR) != RT_ERR_OK )
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

    return RT_ERR_OK;
}

int32 dal_vlan_transparent_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    if( rtk_vlan_transparentEnable_set(DISABLED) == RT_ERR_DRIVER_NOT_FOUND )
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);   
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_transparentEnable_set(RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_transparentEnable_get(NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( rtk_vlan_transparentEnable_set(stateW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( rtk_vlan_transparentEnable_get(&stateR) != RT_ERR_OK )
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

    return RT_ERR_OK;
}

int32 dal_vlan_vlanFunc_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( rtk_vlan_vlanFunctionEnable_set(RTK_ENABLE_END) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( rtk_vlan_vlanFunctionEnable_get(NULL) == RT_ERR_OK )
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( rtk_vlan_vlanFunctionEnable_set(stateW) != RT_ERR_OK )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( rtk_vlan_vlanFunctionEnable_get(&stateR) != RT_ERR_OK )
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

    return RT_ERR_OK;
}

