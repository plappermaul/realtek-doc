#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <common/unittest_util.h>
#include <rtk/trap.h>
#include <dal/dal_trap_test_case.h>

/* Define symbol used for test input */
uint8 mac_array[][6]= {
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x00},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x01},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x02},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x03},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x04},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x05},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x06},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x07},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x08},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x09},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0A},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0B},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0C},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0D},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0F},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x10},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x11},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x12},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x13},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x14},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x15},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x16},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x17},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x18},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x19},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x1A},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x1B},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x1C},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x1D},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x1E},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x1F},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x20},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x21},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x22},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x23},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x24},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x25},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x26},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x27},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x28},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x29},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x2A},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x2B},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x2C},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x2D},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x2E},
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x2F},
{0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCC},
{0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCD}

};

int32 dal_trap_igmpCtrlPkt2CpuEnable_test(uint32 caseNo)
{
    int32 ret;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_igmpCtrlPkt2CpuEnable_set(RTK_ENABLE_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_igmpCtrlPkt2CpuEnable_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( (ret = rtk_trap_igmpCtrlPkt2CpuEnable_set(stateW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_igmpCtrlPkt2CpuEnable_get(&stateR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}


int32 dal_trap_ipMcastPkt2CpuEnable_test(uint32 caseNo)
{
    int32 ret;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_ipMcastPkt2CpuEnable_set(RTK_ENABLE_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_ipMcastPkt2CpuEnable_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( (ret = rtk_trap_ipMcastPkt2CpuEnable_set(stateW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_ipMcastPkt2CpuEnable_get(&stateR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_l2McastPkt2CpuEnable_test(uint32 caseNo)
{
    int32 ret;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_l2McastPkt2CpuEnable_set(RTK_ENABLE_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_l2McastPkt2CpuEnable_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( (ret = rtk_trap_l2McastPkt2CpuEnable_set(stateW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_l2McastPkt2CpuEnable_get(&stateR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_mldCtrlPkt2CpuEnable_test(uint32 caseNo)
{
    int32 ret;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_mldCtrlPkt2CpuEnable_set(RTK_ENABLE_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_mldCtrlPkt2CpuEnable_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( (ret = rtk_trap_mldCtrlPkt2CpuEnable_set(stateW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_mldCtrlPkt2CpuEnable_get(&stateR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_oamPduAction_test(uint32 caseNo)
{
    int32 ret;
    rtk_action_t actW;
    rtk_action_t actR;

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
       osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
       return RT_ERR_OK;          
    }

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_oamPduAction_set(ACTION_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_oamPduAction_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(actW = ACTION_FORWARD; actW < ACTION_END; actW++)
    {
        if( (actW == ACTION_FORWARD) || (actW == ACTION_TRAP2CPU) )
        {
            if( (ret = rtk_trap_oamPduAction_set(actW)) != RT_ERR_OK )
            {
                osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }

            if( (ret = rtk_trap_oamPduAction_get(&actR)) != RT_ERR_OK )
            {
                osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }

            if(actW != actR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_oamPduPri_test(uint32 caseNo)
{
    int32 ret;
    rtk_pri_t priW;
    rtk_pri_t priR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_oamPduPri_set(HAL_INTERNAL_PRIORITY_MAX() + 1)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_oamPduPri_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
    {
        if( (ret = rtk_trap_oamPduPri_set(priW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_oamPduPri_get(&priR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(priW != priR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}


int32 dal_trap_reasonTrapToCpuPriority_test(uint32 caseNo)
{
    int32 ret;
    rtk_trap_reason_type_t type;
    rtk_pri_t priW;
    rtk_pri_t priR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_reasonTrapToCpuPriority_set(TRAP_REASON_END, 0)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    if( (ret = rtk_trap_reasonTrapToCpuPriority_set(TRAP_REASON_RMA, HAL_INTERNAL_PRIORITY_MAX() + 1)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_reasonTrapToCpuPriority_get(TRAP_REASON_IPV4IGMP, NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    if(UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
    {
        for(type = TRAP_REASON_RMA; type < TRAP_REASON_END; type++)
        {
            if( (type == TRAP_REASON_RMA) || (type == TRAP_REASON_MULTICASTDLF) || (type == TRAP_REASON_1XUNAUTH) )
            {
                for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
                {
                    if( (ret = rtk_trap_reasonTrapToCpuPriority_set(type, priW)) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                        return RT_ERR_FAILED;
                    }

                    if( (ret = rtk_trap_reasonTrapToCpuPriority_get(type, &priR)) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                        return RT_ERR_FAILED;
                    }

                    if(priW != priR)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)
    {
        for(type = TRAP_REASON_RMA; type < TRAP_REASON_END; type++)
        {
            if( (type == TRAP_REASON_RMA) || (type == TRAP_REASON_MULTICASTDLF) || (type == TRAP_REASON_1XUNAUTH) || (type == TRAP_REASON_IPV4IGMP) || (type == TRAP_REASON_IPV6MLD))
            {
                for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
                {
                    if( (ret = rtk_trap_reasonTrapToCpuPriority_set(type, priW)) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                        return RT_ERR_FAILED;
                    }

                    if( (ret = rtk_trap_reasonTrapToCpuPriority_get(type, &priR)) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                        return RT_ERR_FAILED;
                    }

                    if(priW != priR)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
    {
        for(type = TRAP_REASON_RMA; type < TRAP_REASON_END; type++)
        {
            if( (type == TRAP_REASON_RMA) || (type == TRAP_REASON_MULTICASTDLF) || (type == TRAP_REASON_1XUNAUTH) || (type == TRAP_REASON_IPV4IGMP) || (type == TRAP_REASON_IPV6MLD))
            {
                for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
                {
                    if( (ret = rtk_trap_reasonTrapToCpuPriority_set(type, priW)) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                        return RT_ERR_FAILED;
                    }

                    if( (ret = rtk_trap_reasonTrapToCpuPriority_get(type, &priR)) != RT_ERR_OK )
                    {
                        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                        return RT_ERR_FAILED;
                    }

                    if(priW != priR)
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


int32 dal_trap_rmaAction_test(uint32 caseNo)
{
    int32 ret;
    rtk_mac_t mac;
    uint32 index;
    rtk_trap_rma_action_t actW;
    rtk_trap_rma_action_t actR;

    /*error input check*/
    /*out of range*/
    mac.octet[0] = 0x00;
    if( (ret = rtk_trap_rmaAction_set(&mac, RMA_ACTION_FORWARD)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    mac.octet[0] = 0x01;
    mac.octet[1] = 0x80;
    mac.octet[2] = 0xC1;
    mac.octet[3] = 0x00;
    mac.octet[4] = 0x00;
    mac.octet[5] = 0x00;
    if( (ret = rtk_trap_rmaAction_set(&mac, RMA_ACTION_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_rmaAction_get(&mac, NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(index = 0; index < (sizeof(mac_array) / 6); index++)
    {
        for(actW = RMA_ACTION_FORWARD; actW < RMA_ACTION_END; actW++)
        {
            if( (actW == RMA_ACTION_FORWARD) || (actW == RMA_ACTION_DROP) || (actW == RMA_ACTION_TRAP2CPU) )
            {
                mac.octet[0] = mac_array[index][0];
                mac.octet[1] = mac_array[index][1];
                mac.octet[2] = mac_array[index][2];
                mac.octet[3] = mac_array[index][3];
                mac.octet[4] = mac_array[index][4];
                mac.octet[5] = mac_array[index][5];
                if( (ret = rtk_trap_rmaAction_set(&mac, actW)) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }

                if( (ret = rtk_trap_rmaAction_get(&mac, &actR)) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }

                if(actW != actR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}
int32 dal_trap_rmaPri_test(uint32 caseNo)
{
    int32 ret;
    rtk_pri_t priW;
    rtk_pri_t priR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_rmaPri_set(HAL_INTERNAL_PRIORITY_MAX() + 1)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_rmaPri_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
    {
        if( (ret = rtk_trap_rmaPri_set(priW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_rmaPri_get(&priR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(priW != priR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}

int32 dal_trap_portIgmpMldCtrlPktAction_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t port;
    rtk_trap_igmpMld_type_t typeW;
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_portIgmpMldCtrlPktAction_set(HAL_GET_MAX_PORT() + 1, IGMPMLD_TYPE_IGMPV1, ACTION_FORWARD)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    if( (ret = rtk_trap_portIgmpMldCtrlPktAction_set(0, IGMPMLD_TYPE_END, ACTION_FORWARD)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    if( (ret = rtk_trap_portIgmpMldCtrlPktAction_set(0, IGMPMLD_TYPE_IGMPV1, ACTION_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_portIgmpMldCtrlPktAction_get(0, IGMPMLD_TYPE_IGMPV1, NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(typeW = IGMPMLD_TYPE_IGMPV1; typeW < IGMPMLD_TYPE_END; typeW++)
        {
            for(actionW = ACTION_FORWARD; actionW <= ACTION_TRAP2CPU; actionW++)
            {
                if( (ret = rtk_trap_portIgmpMldCtrlPktAction_set(port, typeW, actionW)) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }

                if( (ret = rtk_trap_portIgmpMldCtrlPktAction_get(port, typeW, &actionR)) != RT_ERR_OK )
                {
                    osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_uniTrapPriorityEnable_test(uint32 caseNo)
{
    int32 ret;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_uniTrapPriorityEnable_set(RTK_ENABLE_END)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_uniTrapPriorityEnable_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( (ret = rtk_trap_uniTrapPriorityEnable_set(stateW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_uniTrapPriorityEnable_get(&stateR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_uniTrapPriorityPriority_test(uint32 caseNo)
{
    int32 ret;
    rtk_pri_t priW;
    rtk_pri_t priR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_trap_uniTrapPriorityPriority_set(HAL_INTERNAL_PRIORITY_MAX() + 1)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if( (ret = rtk_trap_uniTrapPriorityPriority_get(NULL)) == RT_ERR_OK )
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/Set */
    for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
    {
        if( (ret = rtk_trap_uniTrapPriorityPriority_set(priW)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if( (ret = rtk_trap_uniTrapPriorityPriority_get(&priR)) != RT_ERR_OK )
        {
            osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }

        if(priW != priR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}

