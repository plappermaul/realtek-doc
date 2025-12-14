 /*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 */

/*
 * Include Files
 */
#include <dal/rtl9602c/dal_rtl9602c.h>

#include <rtk/l34.h>
#include <dal/rtl9602c/dal_rtl9602c_l34.h>
#include <ioal/mem32.h>

/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */
static uint32               l34_init = {INIT_NOT_COMPLETED};


/*
 * Macro Definition
 */
int32 dal_rtl9602c_l34_hsb_set(rtl9602c_l34_hsb_param_t *hsb);
int32 dal_rtl9602c_l34_hsb_get(rtl9602c_l34_hsb_param_t *hsb);
int32 dal_rtl9602c_l34_hsa_set(rtl9602c_l34_hsa_param_t *hsa);
int32 dal_rtl9602c_l34_hsa_get(rtl9602c_l34_hsa_param_t *hsa);
int32 dal_rtl9602c_l34_hsabCtrMode_set(rtl9602c_l34_hsba_mode_t mode);


static int32
_dal_rtl9602c_l34_bindingAct_cfg2raw(rtk_l34_bindType_t type,rtk_l34_bindAct_t action,uint32 *rawfield,uint32 *rawAct)
{
    switch(type){
        case L34_BIND_UNMATCHED_L2L3:
        {
            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L2L3f;
            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_L2L3_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L2L3_TRAP;
            }else
            if(action==L34_BIND_ACT_FORCE_L2BRIDGE)
            {
                *rawAct = RTL9602C_UNMATCHED_L2L3_FORCE_L2Bridge;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L2L34:
        {
            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L2L34f;
            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_L2L34_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L2L34_TRAP;
            }else
            if(action==L34_BIND_ACT_FORCE_L2BRIDGE)
            {
                *rawAct = RTL9602C_UNMATCHED_L2L34_FORCE_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L3L2:
        {
            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L3L2f;
            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L2_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L2_TRAP;
            }else
            if(action==L34_BIND_ACT_PERMIT_L2BRIDGE)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L2_PERMIT_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L3L34:
        {

            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L3L34f;
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L34_TRAP;
            }else
            if(action==L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L34_FORCE_L4;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L34L2:
        {
            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L34L2f;
            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_L34L2_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L34L2_TRAP;
            }else
            if(action==L34_BIND_ACT_PERMIT_L2BRIDGE)
            {
                *rawAct = RTL9602C_UNMATCHED_L34L2_PERMIT_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L34L3:
        {
            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L34L3f;
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L34L3_TRAP;
            }else
            if(action==L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4)
            {
                *rawAct = RTL9602C_UNMATCHED_L34L3_FORCE_L3;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L3L3:
        {
            *rawfield = RTL9602C_WAN_BINDING_UNMATCHED_L3L3f;
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L3_TRAP;
            }else
            if(action==L34_BIND_ACT_FORCE_BINDL3)
            {
                *rawAct = RTL9602C_UNMATCHED_L3L3_FORCE_L3;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_CUSTOMIZED_L2:
        {
            *rawfield = RTL9602C_WAN_BINDING_CUSTOMIZED_L2f;

            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL2_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL2_TRAP;
            }else
            if(action==L34_BIND_ACT_PERMIT_L2BRIDGE)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL2_PERMIT_L2_BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_CUSTOMIZED_L3:
        {
            *rawfield = RTL9602C_WAN_BINDING_CUSTOMIZED_L3f;
            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL3_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL3_TRAP;
            }else
            if(action==L34_BIND_ACT_FORCE_BINDL3)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL3_FORCE_L3;
            }else
            if(action==L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL3_FORCE_L4;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_CUSTOMIZED_L34:
        {
            *rawfield = RTL9602C_WAN_BINDING_CUSTOMIZED_L34f;

            if(action==L34_BIND_ACT_DROP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL34_DROP;
            }else
            if(action==L34_BIND_ACT_TRAP)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL34_TRAP;
            }else
            if(action==L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL34_FORCE_L3;
            }else
            if(action==L34_BIND_ACT_NORMAL_LOOKUPL34)
            {
                *rawAct = RTL9602C_UNMATCHED_CUSL34_NORMAL_L34;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        default:
            return RT_ERR_INPUT;
        break;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_l34_bindingAct_raw2cfg(rtk_l34_bindType_t type,uint32 raw,rtk_l34_bindAct_t *pAct)
{
    switch(type){
        case L34_BIND_UNMATCHED_L2L3:
        {
            if(raw== RTL9602C_UNMATCHED_L2L3_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_L2L3_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L2L3_FORCE_L2Bridge)
            {
                *pAct=L34_BIND_ACT_FORCE_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L2L34:
        {
            if(raw ==RTL9602C_UNMATCHED_L2L34_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_L2L34_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L2L34_FORCE_L2BRIDGE)
            {
                *pAct=L34_BIND_ACT_FORCE_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L3L2:
        {
            if(raw==RTL9602C_UNMATCHED_L3L2_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_L3L2_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L3L2_PERMIT_L2BRIDGE)
            {
                *pAct=L34_BIND_ACT_PERMIT_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L3L34:
        {
            if(raw==RTL9602C_UNMATCHED_L3L34_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L3L34_FORCE_L4)
            {
                *pAct=L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L34L2:
        {
            if(raw==RTL9602C_UNMATCHED_L34L2_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_L34L2_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L34L2_PERMIT_L2BRIDGE)
            {
                *pAct=L34_BIND_ACT_PERMIT_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L34L3:
        {
            if(raw==RTL9602C_UNMATCHED_L34L3_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L34L3_FORCE_L3)
            {
                *pAct=L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_UNMATCHED_L3L3:
        {
            if(raw==RTL9602C_UNMATCHED_L3L3_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_L3L3_FORCE_L3)
            {
                *pAct=L34_BIND_ACT_FORCE_BINDL3;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_CUSTOMIZED_L2:
        {
            if(raw==RTL9602C_UNMATCHED_CUSL2_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL2_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL2_PERMIT_L2_BRIDGE)
            {
                *pAct=L34_BIND_ACT_PERMIT_L2BRIDGE;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_CUSTOMIZED_L3:
        {
            if(raw==RTL9602C_UNMATCHED_CUSL3_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL3_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL3_FORCE_L3)
            {
                *pAct=L34_BIND_ACT_FORCE_BINDL3;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL3_FORCE_L4)
            {
                *pAct=L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
        case L34_BIND_CUSTOMIZED_L34:
        {
            if(raw==RTL9602C_UNMATCHED_CUSL34_DROP)
            {
                *pAct=L34_BIND_ACT_DROP;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL34_TRAP)
            {
                *pAct=L34_BIND_ACT_TRAP;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL34_FORCE_L3)
            {
                *pAct=L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
            }else
            if(raw==RTL9602C_UNMATCHED_CUSL34_NORMAL_L34)
            {
                *pAct=L34_BIND_ACT_NORMAL_LOOKUPL34;
            }else
            {
                return RT_ERR_INPUT;
            }
        }
        break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_l34_binding_type_field_get(rtk_l34_bindType_t type,uint32 *rawField)
{
    switch(type){
        case L34_BIND_UNMATCHED_L2L3:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L2L3f;
        }
        break;
            case L34_BIND_UNMATCHED_L2L34:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L2L34f;
        }
        break;
            case L34_BIND_UNMATCHED_L3L2:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L3L2f;
        }
        break;
            case L34_BIND_UNMATCHED_L3L34:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L3L34f;
        }
        break;
            case L34_BIND_UNMATCHED_L34L2:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L34L2f;
        }
        break;
            case L34_BIND_UNMATCHED_L34L3:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L34L3f;
        }
        break;
            case L34_BIND_UNMATCHED_L3L3:
        {
            *rawField = RTL9602C_WAN_BINDING_UNMATCHED_L3L3f;
        }
        break;
            case L34_BIND_CUSTOMIZED_L2:
        {
            *rawField = RTL9602C_WAN_BINDING_CUSTOMIZED_L2f;
        }
        break;
            case L34_BIND_CUSTOMIZED_L3:
        {
            *rawField = RTL9602C_WAN_BINDING_CUSTOMIZED_L3f;
        }
        break;
            case L34_BIND_CUSTOMIZED_L34:
        {
            *rawField = RTL9602C_WAN_BINDING_CUSTOMIZED_L34f;
        }
        break;
        default:
            return RT_ERR_INPUT;
        break;
    }
    return RT_ERR_OK;
}


static int32
_dal_rtl9602c_l34_hsbaMode_cfg2raw(rtk_l34_hsba_mode_t mode, rtl9602c_l34_hsba_mode_t *pMode)
{
    switch(mode){
    case L34_HSBA_BOTH_LOG:
        *pMode = RTL9602C_L34_HSBA_TEST_MODE;
    break;
    case L34_HSBA_NO_LOG:
        *pMode = RTL9602C_L34_HSBA_NO_LOG;
    break;
    case L34_HSBA_LOG_ALL:
        *pMode = RTL9602C_L34_HSBA_LOG_ALL;
    break;
    case L34_HSBA_LOG_FIRST_DROP:
        *pMode = RTL9602C_L34_HSBA_LOG_FIRST_DROP;
    break;
    case L34_HSBA_LOG_FIRST_PASS:
        *pMode = RTL9602C_L34_HSBA_LOG_FIRST_PASS;
    break;
    case L34_HSBA_LOG_FIRST_TO_CPU:
        *pMode = RTL9602C_L34_HSBA_LOG_FIRST_TO_CPU;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_l34_hsbaMode_raw2cfg(rtl9602c_l34_hsba_mode_t mode,rtk_l34_hsba_mode_t *pMode)
{
    switch(mode){
    case RTL9602C_L34_HSBA_TEST_MODE:
        *pMode = L34_HSBA_BOTH_LOG;
    break;
    case RTL9602C_L34_HSBA_NO_LOG:
        *pMode = L34_HSBA_NO_LOG;
    break;
    case RTL9602C_L34_HSBA_LOG_ALL:
        *pMode = L34_HSBA_LOG_ALL;
    break;
    case RTL9602C_L34_HSBA_LOG_FIRST_DROP:
        *pMode = L34_HSBA_LOG_FIRST_DROP;
    break;
    case RTL9602C_L34_HSBA_LOG_FIRST_PASS:
        *pMode = L34_HSBA_LOG_FIRST_PASS;
    break;
    case RTL9602C_L34_HSBA_LOG_FIRST_TO_CPU:
        *pMode = L34_HSBA_LOG_FIRST_TO_CPU;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      _dal_rtl9602c_l34_mode_set
 * Description:
 *      Set L34 mode.
 * Input:
 *      rtl9602c_l34_mode_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
static int32 _dal_rtl9602c_l34_mode_set(rtl9602c_l34_mode_t mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode > RTL9602C_L34_L3_ENABLE_L4_ENABLE), RT_ERR_INPUT);

    tmp_val = mode;

    if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_NATMODEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      _dal_rtl9602c_l34_mode_get
 * Description:
 *      Get L34 mode.
 * Input:
 *      None
 * Output:
 *      rtl9602c_l34_mode_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
static int32 _dal_rtl9602c_l34_mode_get(rtl9602c_l34_mode_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_NATMODEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    *mode = tmp_val;

    return RT_ERR_OK;

}


static int32
_dal_rtl9602c_l34_lookupMode_cfg2raw(rtk_l34_lookupMode_t type,uint32 *raw)
{

    switch(type){
    case L34_LOOKUP_VLAN_BASE:
        *raw = RTL9602C_L34_LIMBC_VLAN_BASED;
    break;
    case L34_LOOKUP_MAC_BASE:
        *raw = RTL9602C_L34_LIMBC_MAC_BASED;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_l34_lookupMode_raw2cfg(uint32 raw, rtk_l34_lookupMode_t *mode)
{

    switch(raw){
    case RTL9602C_L34_LIMBC_VLAN_BASED:
        *mode = L34_LOOKUP_VLAN_BASE;
    break;
    case RTL9602C_L34_LIMBC_MAC_BASED:
        *mode = L34_LOOKUP_MAC_BASE;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_l34_flowRouteType_cfg2raw(rtk_l34_flowRoute_type_t process, uint32 *raw)
{

    switch(process){
    case L34_FLOWRT_TYPE_CPU:
        *raw = RTL9602C_L34_FLOWRT_TYPE_CPU;
    break;
    case L34_FLOWRT_TYPE_DROP:
        *raw = RTL9602C_L34_FLOWRT_TYPE_DROP;
    break;
    case L34_FLOWRT_TYPE_LOCAL:
        *raw = RTL9602C_L34_FLOWRT_TYPE_LOCAL;
    break;
    case L34_FLOWRT_TYPE_GLOBAL:
        *raw = RTL9602C_L34_FLOWRT_TYPE_GLOBAL;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_l34_flowRouteType_raw2cfg(uint32 raw, rtk_l34_flowRoute_type_t *process)
{

    switch(raw){
    case RTL9602C_L34_FLOWRT_TYPE_CPU:
        *process = L34_FLOWRT_TYPE_CPU;
    break;
    case RTL9602C_L34_FLOWRT_TYPE_DROP:
        *process = L34_FLOWRT_TYPE_DROP;
    break;
    case RTL9602C_L34_FLOWRT_TYPE_LOCAL:
        *process = L34_FLOWRT_TYPE_LOCAL;
    break;
    case RTL9602C_L34_FLOWRT_TYPE_GLOBAL:
        *process = L34_FLOWRT_TYPE_GLOBAL;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9602c_l34_hsb_set
 * Description:
 *      set hsb value.
 * Input:
 *      hsb : hsb content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsb_set(rtl9602c_l34_hsb_param_t *hsb)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < RTL9602C_L34_HSB_WORD; index++)
    {
        if ((ret = reg_write((RTL9602C_HSB_DESC_W0r + index), (uint32 *)(&hsb->hsbWords[index])))!=RT_ERR_OK)
        {
            RT_ERR(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;

}/* end of dal_rtl9602c_l34_hsb_set */



/* Function Name:
 *      dal_rtl9602c_l34_hsb_get
 * Description:
 *      get data from hsb.
 * Input:
 *      None
 * Output:
 *      hsb : hsb content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsb_get(rtl9602c_l34_hsb_param_t *hsb)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < RTL9602C_L34_HSB_WORD; index++)
    {
        if ((ret = reg_read((RTL9602C_HSB_DESC_W0r + index), (uint32 *)(&hsb->hsbWords[index])))!=RT_ERR_OK)
        {
            RT_ERR(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;


}/* end of dal_rtl9602c_l34_hsb_get */





/* Function Name:
 *      dal_rtl9602c_l34_hsa_set
 * Description:
 *      set hsa value.
 * Input:
 *      hsa : hsa content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsa_set(rtl9602c_l34_hsa_param_t *hsa)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < RTL9602C_L34_HSA_WORD; index++)
    {
        if ((ret = reg_write((RTL9602C_HSA_DESC_W0r + index), (uint32 *)(&hsa->hsaWords[index])))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
    }
    return RT_ERR_OK;

}/* end of dal_rtl9602c_l34_hsa_set */



/* Function Name:
 *      dal_rtl9602c_l34_hsa_get
 * Description:
 *      get data from hsa.
 * Input:
 *      None
 * Output:
 *      hsb : hsa content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsa_get(rtl9602c_l34_hsa_param_t *hsa)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < RTL9602C_L34_HSA_WORD; index++)
    {
        if ((ret = reg_read((RTL9602C_HSA_DESC_W0r + index), (uint32 *)(&hsa->hsaWords[index])))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
    }
    return RT_ERR_OK;


}/* end of dal_rtl9602c_l34_hsa_get */


/* Function Name:
 *      dal_rtl9602c_l34_hsabCtrMode_set
 * Description:
 *      Set L34 HSAB log mode.
 * Input:
 *      None
 * Output:
 *      mode : L34 HSAB log mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsabCtrMode_set(rtl9602c_l34_hsba_mode_t mode)
{
    int32 ret;
    if ((ret = reg_field_write(RTL9602C_HSBA_CTRLr,RTL9602C_TST_LOG_MDf,(uint32 *)&mode))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    return RT_ERR_OK;
}/* end of dal_rtl9602c_l34_hsabCtrMode_set */





/* Function Name:
 *      dal_rtl9602c_l34_init
 * Description:
 *      Initialize l34 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l34 module before calling any l34 APIs.
 */
int32
dal_rtl9602c_l34_init(void)
{
    int32 ret;
    int32 i;
    rtk_l34_globalStateType_t cfg;

    l34_init = INIT_COMPLETED;

    /*reset all table*/
    for(i=0;i<L34_TABLE_END;i++)
    {
        ret = dal_rtl9602c_l34_table_reset(i);
        if( (ret != RT_ERR_OK) && (ret != RT_ERR_INPUT))
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            l34_init = INIT_NOT_COMPLETED ;
                return ret;
        }
    }
    /*enable l34 routing */
    cfg = L34_GLOBAL_L34_STATE;
    if((ret = dal_rtl9602c_l34_globalState_set(cfg, ENABLED))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        l34_init = INIT_NOT_COMPLETED ;
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_init */





/* Function Name:
 *      dal_rtl9602c_l34_netifTable_set
 * Description:
 *      Set netif entry by idx.
 * Input:
 *      idx, *entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    int32 ret;
    uint32 tempVal;
    rtl9602c_l34_netif_entry_t netif_entry;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < entry->vlan_id), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->enable_rounting != 0 && entry->enable_rounting != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->mtu >= 16384), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->mac_mask != 0 && entry->mac_mask != 4 && entry->mac_mask != 6 && entry->mac_mask != 7), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->dslite_state != 0 && entry->dslite_state != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9602C_L34_DSLITE_INF_TABLE_MAX <= entry->dslite_idx), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&netif_entry, 0x0, sizeof(netif_entry));

    tempVal = entry->valid;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_VALIDtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->mtu;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_MTUtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->enable_rounting;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_ENRTRtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->mac_mask;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_MACMASKtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_mac_set(RTL9602C_NETIFt, RTL9602C_NETIF_GMACtf, (uint8 *)&entry->gateway_mac, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->vlan_id;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_VLANIDtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->dslite_state;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_DSLITEtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->dslite_idx;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_DSTB_IDXtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->isL34;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_L34tf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->ipAddr;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_IPtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->isCtagIf;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_CTAG_IFtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tempVal = entry->isIpv6;
    if ((ret = table_field_set(RTL9602C_NETIFt, RTL9602C_NETIF_IPV6tf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(RTL9602C_NETIFt, idx, (uint32 *)&netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_netifTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_netifTable_get
 * Description:
 *      Get a arp entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_netif_entry_t netif_entry;
    uint32 tmpVal;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&netif_entry, 0x0, sizeof(netif_entry));

    if ((ret = table_read(RTL9602C_NETIFt, idx, (uint32 *)&netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_MTUtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->mtu = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_ENRTRtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->enable_rounting = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_MACMASKtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->mac_mask = tmpVal;

    if ((ret = table_field_mac_get(RTL9602C_NETIFt, RTL9602C_NETIF_GMACtf, (uint8 *)&(entry->gateway_mac), (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_VLANIDtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->vlan_id = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_DSLITEtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->dslite_state = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_L34tf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->isL34= tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_IPtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->ipAddr = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_CTAG_IFtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->isCtagIf = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_DSTB_IDXtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->dslite_idx = tmpVal;

    if ((ret = table_field_get(RTL9602C_NETIFt, RTL9602C_NETIF_IPV6tf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->isIpv6 = tmpVal;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_netifTable_get */


/* Function Name:
 *      dal_rtl9602c_l34_arpTable_set
 * Description:
 *      Set ARP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_arp_entry_t rtl9602c_arp_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX()<=entry->nhIdx), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rtl9602c_arp_entry, 0x0, sizeof(rtl9602c_arp_entry));

    if ((ret = table_field_set(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_NXTHOPIDXtf, (uint32 *)&entry->nhIdx, (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_set(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_VALIDtf, (uint32 *)&entry->valid, (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_set(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_IPtf, (uint32 *)&entry->ipAddr, (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_ARP_CAM_TABLEt, idx, (uint32 *)&rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_arpTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_arpTable_get
 * Description:
 *      Get a arp entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_arp_entry_t rtl9602c_arp_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&rtl9602c_arp_entry, 0x0, sizeof(rtl9602c_arp_entry));

    if ((ret = table_read(RTL9602C_ARP_CAM_TABLEt, idx, (uint32 *)&rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_NXTHOPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_IPtf, (uint32 *)&(tmp_val), (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->ipAddr = tmp_val;

    entry->index = idx;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_arpTable_get */

/* Function Name:
 *      dal_rtl9602c_l34_arpTable_del
 * Description:
 *      delete a arp entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_arpTable_del(uint32 idx)
{
    int32 ret;
    rtl9602c_l34_arp_entry_t rtl9602c_arp_entry;
    uint32 is_valid=0;
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rtl9602c_arp_entry, 0x0, sizeof(rtl9602c_arp_entry));

    if ((ret = table_field_set(RTL9602C_ARP_CAM_TABLEt, RTL9602C_ARP_CAM_TABLE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_ARP_CAM_TABLEt, idx, (uint32 *)&rtl9602c_arp_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_rtl9602c_l34_arpTable_del */



/* Function Name:
 *      dal_rtl9602c_l34_pppoeTable_set
 * Description:
 *      Set ARP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_pppoe_entry_t rtl9602c_pppoe_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->sessionID >= 0x10000), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rtl9602c_pppoe_entry, 0x0, sizeof(rtl9602c_pppoe_entry));

    if ((ret = table_field_set(RTL9602C_PPPOE_TABLEt, RTL9602C_PPPOE_TABLE_SESIDtf, (uint32 *)&entry->sessionID, (uint32 *) &rtl9602c_pppoe_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_PPPOE_TABLEt, idx, (uint32 *)&rtl9602c_pppoe_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_pppoeTable_set */




/* Function Name:
 *      dal_rtl9602c_l34_pppoeTable_get
 * Description:
 *      Get a PPPOE entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_pppoe_entry_t rtl9602c_pppoe_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rtl9602c_pppoe_entry, 0x0, sizeof(rtl9602c_pppoe_entry));

    if ((ret = table_read(RTL9602C_PPPOE_TABLEt, idx, (uint32 *)&rtl9602c_pppoe_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_PPPOE_TABLEt, RTL9602C_PPPOE_TABLE_SESIDtf, (uint32 *)&(tmp_val), (uint32 *) &rtl9602c_pppoe_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->sessionID = tmp_val;


    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_pppoeTable_get */


/* Function Name:
 *      dal_rtl9602c_l34_routingTable_set
 * Description:
 *      Set Routing table by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_routing_entry_t routing_entry;
    rtk_rtl9602c_table_list_t table_type;
    uint32 tmpVal;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((31 < entry->ipMask), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_PROCESS_END <= entry->process), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->internal != 0 && entry->internal != 1), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->netifIdx), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX() <= entry->nhNxt), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    /*general part*/
    tmpVal = entry->ipAddr;
    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_IPtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipMask;
    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_MASKtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->process;
    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->valid;
    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->internal;
    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_INTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->rt2waninf;
    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if(entry->process == L34_PROCESS_CPU || entry->process == L34_PROCESS_DROP)
        table_type = RTL9602C_L3_ROUTING_DROP_TRAPt;
    else if(entry->process == L34_PROCESS_ARP)
        table_type = RTL9602C_L3_ROUTING_LOCAL_ROUTEt;
    else if(entry->process == L34_PROCESS_NH)
        table_type = RTL9602C_L3_ROUTING_GLOBAL_ROUTEt;
    else
        return RT_ERR_FAILED;

    /*process == arp*/
    if(table_type == RTL9602C_L3_ROUTING_LOCAL_ROUTEt)
    {
        tmpVal = entry->netifIdx;
        if ((ret = table_field_set(table_type, RTL9602C_L3_ROUTING_LOCAL_ROUTE_DENTIFtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
    }
    /*process == nexthop*/
    else if(table_type == RTL9602C_L3_ROUTING_GLOBAL_ROUTEt)
    {
        tmpVal = entry->nhNxt;
        if ((ret = table_field_set(table_type, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_NH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
    }

    if ((ret = table_write(table_type, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_routingTable_set */

/* Function Name:
 *      dal_rtl9602c_l34_routingTable_get
 * Description:
 *      Get a routing table entry by idx.
 * Input:
 *      idx
 * Output:
 *      *entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_routing_entry_t routing_entry;
    rtk_rtl9602c_table_list_t table_type;
    uint32 tmp_val;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    if ((ret = table_read(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_PROCESStf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->process = tmp_val;

    if ((ret = table_field_get(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_IPtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    entry->ipAddr = tmp_val;

    if ((ret = table_field_get(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_MASKtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->ipMask = tmp_val;


    if ((ret = table_field_get(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_INTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->internal = tmp_val;

    if ((ret = table_field_get(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_RT2WANINFtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->rt2waninf = tmp_val;


    /*get entry process type*/
    if(entry->process == L34_PROCESS_CPU || entry->process == L34_PROCESS_DROP)
        table_type = RTL9602C_L3_ROUTING_DROP_TRAPt;
    else if(entry->process == L34_PROCESS_ARP)
        table_type = RTL9602C_L3_ROUTING_LOCAL_ROUTEt;
    else if(entry->process == L34_PROCESS_NH)
        table_type = RTL9602C_L3_ROUTING_GLOBAL_ROUTEt;
    else
        return RT_ERR_FAILED;


    /*process == arp*/
    if(table_type == RTL9602C_L3_ROUTING_LOCAL_ROUTEt)
    {
        if ((ret = table_field_get(table_type, RTL9602C_L3_ROUTING_LOCAL_ROUTE_DENTIFtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        entry->netifIdx = tmp_val;
    }
    /*process == nexthop*/
    else if(table_type == RTL9602C_L3_ROUTING_GLOBAL_ROUTEt)
    {
        if ((ret = table_field_get(table_type, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_NH_IDXtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        entry->nhNxt = tmp_val;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_routingTable_get */




/* Function Name:
 *      dal_rtl9602c_l34_routingTable_del
 * Description:
 *      delete a routing entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_routingTable_del(uint32 idx)
{
    int32 ret;
    rtl9602c_l34_routing_entry_t routing_entry;
    uint32 is_valid=0;

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    if ((ret = table_field_set(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, RTL9602C_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_L3_ROUTING_GLOBAL_ROUTEt, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_rtl9602c_l34_routingTable_del */


/* Function Name:
 *      dal_rtl9602c_l34_nexthopTable_set
 * Description:
 *      Set nexthop entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_nexthop_entry_t nh_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX() <= entry->nhIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->ifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= entry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_NH_END <= entry->type), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_NH_PPPOE_END <= entry->keepPppoe), RT_ERR_INPUT);


    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&nh_entry, 0x0, sizeof(nh_entry));

    tmp_val = (uint32)entry->nhIdx;
    if ((ret = table_field_set(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_NXTHOPIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->pppoeIdx;
    if ((ret = table_field_set(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_PPPIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->type;
    if ((ret = table_field_set(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_TYPEtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->ifIdx;
    if ((ret = table_field_set(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_IFIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->keepPppoe;
    if ((ret = table_field_set(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_KEEPPPPOPtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_NEXT_HOP_TABLEt, idx, (uint32 *)&nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_rtl9602c_l34_nexthopTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_nexthopTable_get
 * Description:
 *      Get a nexthop entry by idx.
 * Input:
 *      idx
 * Output:
 *      *entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_nexthop_entry_t nh_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&nh_entry, 0x0, sizeof(nh_entry));

    if ((ret = table_read(RTL9602C_NEXT_HOP_TABLEt, idx, (uint32 *)&nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_NXTHOPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_PPPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->pppoeIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_IFIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->ifIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->type = tmp_val;

    if ((ret = table_field_get(RTL9602C_NEXT_HOP_TABLEt, RTL9602C_NEXT_HOP_TABLE_KEEPPPPOPtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->keepPppoe = tmp_val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_nexthopTable_get */




/* Function Name:
 *      dal_rtl9602c_l34_extIntIPTable_set
 * Description:
 *      Set external internal IP entry by idx.
 * Input:
 *      idx, *entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_extip_entry_t extip_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=entry->nhIdx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->prival != 0 && entry->prival != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->type >= L34_EXTIP_TYPE_END ), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->pri >= RTK_MAX_NUM_OF_PRIORITY ), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    tmp_val = (uint32)entry->pri;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->prival;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_PRIVALtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->nhIdx;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_NH_IDXtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->type;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_TYPEtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->valid;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_VALIDtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->extIpAddr;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_EXTIPtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->intIpAddr;
    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_INTIPtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_extIntIPTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_extIntIPTable_get
 * Description:
 *      Get a external internal IP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_extip_entry_t extip_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_read(RTL9602C_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_PRIORITYtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->pri = tmp_val;

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_PRIVALtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->prival = tmp_val;

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_NH_IDXtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->type = tmp_val;

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_EXTIPtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->extIpAddr = tmp_val;

    if ((ret = table_field_get(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_INTIPtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->intIpAddr = tmp_val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_extIntIPTable_get */



/* Function Name:
 *      dal_rtl9602c_l34_extIntIPTable_del
 * Description:
 *      delete a external internal IP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_extIntIPTable_del(uint32 idx)
{
    int32 ret;
    rtl9602c_l34_extip_entry_t extip_entry;
    uint32 is_valid=0;

    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_field_set(RTL9602C_EXTERNAL_IP_TABLEt, RTL9602C_EXTERNAL_IP_TABLE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_rtl9602c_l34_extIntIPTable_del */




/* Function Name:
 *      dal_rtl9602c_l34_naptInboundTable_set
 * Description:
 *      Set Inbound NAPT table by idx.
 * Input:
 *      None
 * Output:
 *      forced : 0:do not add this entry if collection  1:farce overwrite this entry
 *      idx    : entry index
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_napt_inbound_entry_t naptInbound_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPTR_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((forced != 0 && forced != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->isTcp != 0 && entry->isTcp != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid >=4), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValid != 0 && entry->priValid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priId >= RTK_MAX_NUM_OF_PRIORITY), RT_ERR_PRIORITY);
    RT_PARAM_CHK((entry->extIpIdx >= HAL_L34_EXTIP_ENTRY_MAX()), RT_ERR_INPUT);


    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* check if the index is valid*/
    if(forced == 0)
    {
        osal_memset(&naptInbound_entry, 0x0, sizeof(naptInbound_entry));
        if ((ret = table_read(RTL9602C_NAPTR_TABLEt, idx, (uint32 *)&naptInbound_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        /*get valid field*/
        if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        if(tmp_val == 1)
        {
            return RT_ERR_FAILED;
        }
    }


    osal_memset(&naptInbound_entry, 0x0, sizeof(naptInbound_entry));

    tmp_val = entry->priId;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_PRIORITYtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->priValid;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_PRI_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->valid;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->isTcp;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_TCPtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (entry->extPortHSB << 8 | entry->extPortLSB) ;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_EXTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->extIpIdx;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_EXTIP_IDXtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->remHash;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_REM_HASHtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->intPort;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_INTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = entry->intIp;
    if ((ret = table_field_set(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_INTIPtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(RTL9602C_NAPTR_TABLEt, idx, (uint32 *)&naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_rtl9602c_l34_naptInboundTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_naptInboundTable_get
 * Description:
 *      Get Inbound NAPT table by idx.
 * Input:
 *      idx    : entry index
 * Output:
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_napt_inbound_entry_t naptInbound_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPTR_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&naptInbound_entry, 0x0, sizeof(naptInbound_entry));

    if ((ret = table_read(RTL9602C_NAPTR_TABLEt, idx, (uint32 *)&naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_PRIORITYtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->priId = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_PRI_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->priValid = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_TCPtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->isTcp = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_EXTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->extPortLSB = tmp_val & 0xFF;
    entry->extPortHSB = (tmp_val >> 8) & 0xFF;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_EXTIP_IDXtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->extIpIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_REM_HASHtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->remHash= tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_INTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->intPort= tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPTR_TABLEt, RTL9602C_NAPTR_TABLE_INTIPtf, (uint32 *)&tmp_val, (uint32 *) &naptInbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->intIp = tmp_val;

    return RT_ERR_OK;
}/* end of dal_rtl9602c_l34_naptInboundTable_get */


/* Function Name:
 *      dal_rtl9602c_l34_naptOutboundTable_set
 * Description:
 *      Set Outbound NAPT table by idx.
 * Input:
 *      forced : 0:do not add this entry if collection  1:farce overwrite this entry
 *      idx    : entry index
 *      entry  : entry content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_napt_outbound_entry_t naptOntbound_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((forced != 0 && forced != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValid != 0 && entry->priValid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValue >= RTK_MAX_NUM_OF_PRIORITY), RT_ERR_PRIORITY);
    RT_PARAM_CHK((entry->hashIdx >= 4096), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* check if the index is valid*/
    if(forced == 0)
    {
        osal_memset(&naptOntbound_entry, 0x0, sizeof(naptOntbound_entry));
        if ((ret = table_read(RTL9602C_NAPT_TABLEt, idx, (uint32 *)&naptOntbound_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        /*get valid field from napt table*/
        if ((ret = table_field_get(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        if(tmp_val == 1)
        {
            return RT_ERR_FAILED;
        }
    }


    osal_memset(&naptOntbound_entry, 0x0, sizeof(naptOntbound_entry));

    tmp_val = (uint32)entry->valid;
    if ((ret = table_field_set(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_VALIDtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->hashIdx;
    if ((ret = table_field_set(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_HASHIN_IDXtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->priValid;
    if ((ret = table_field_set(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_PRI_VALIDtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->priValue;
    if ((ret = table_field_set(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_NAPT_TABLEt, idx, (uint32 *)&naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_rtl9602c_l34_naptOutboundTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_naptOutboundTable_get
 * Description:
 *      Set Outband NAPT table by idx.
 * Input:
 *      None
 * Output:
 *      idx    : entry index
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_napt_outbound_entry_t naptOntbound_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&naptOntbound_entry, 0x0, sizeof(naptOntbound_entry));

    if ((ret = table_read(RTL9602C_NAPT_TABLEt, idx, (uint32 *)&naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_VALIDtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;


    if ((ret = table_field_get(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_HASHIN_IDXtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->hashIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_PRI_VALIDtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->priValid = tmp_val;

    if ((ret = table_field_get(RTL9602C_NAPT_TABLEt, RTL9602C_NAPT_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &naptOntbound_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->priValue = tmp_val;

    return RT_ERR_OK;
}/* end of dal_rtl9602c_l34_naptOutboundTable_get */



/* Function Name:
 *      dal_rtl9602c_l34_table_reset
 * Description:
 *      Reset specific L34 table.
 * Input:
 *      None
 * Output:
 *      type : L34 table type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_table_reset(rtk_l34_table_type_t type)
{
    int32 ret;
    uint32 reg_field;
    uint32 reset_status,busy,count;
    /*get reset table type*/
    switch(type)
    {
        case L34_ROUTING_TABLE:
            reg_field = RTL9602C_RST_L3f;
            break;
        case L34_PPPOE_TABLE:
            reg_field = RTL9602C_RST_PPf;
            break;
        case L34_NEXTHOP_TABLE:
            reg_field = RTL9602C_RST_NHf;
            break;
        case L34_NETIF_TABLE:
            reg_field = RTL9602C_RST_IFf;
            break;
        case L34_INTIP_TABLE:
            reg_field = RTL9602C_RST_IPf;
            break;
        case L34_ARP_TABLE:
            reg_field = RTL9602C_RST_ARPCAMf;
            break;
        case L34_NAPTR_TABLE:
            reg_field = RTL9602C_RST_NAPTRf;
            break;
        case L34_NAPT_TABLE:
            reg_field = RTL9602C_RST_NAPTf;
            break;

        case L34_IPV6_ROUTING_TABLE:
            reg_field = RTL9602C_RST_V6RTf;
            break;

        case L34_BINDING_TABLE:
            reg_field = RTL9602C_RST_BDf;
            break;

        case L34_IPV6_NEIGHBOR_TABLE:
            reg_field = RTL9602C_RST_NBf;
            break;

        case L34_WAN_TYPE_TABLE:
            reg_field = RTL9602C_RST_WTf;
            break;

        case L34_FLOW_ROUTING_TABLE:
            reg_field = RTL9602C_RST_FTf;
            break;

        default:
            return RT_ERR_INPUT;
    }
    /*get register*/
    if ((ret = reg_field_read(RTL9602C_NAT_TBL_ACCESS_CLRr,reg_field,&reset_status))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    if(reset_status == 0)
    {/*table is not perform reset, we reset it*/
        reset_status = 1;
        if ((ret = reg_field_write(RTL9602C_NAT_TBL_ACCESS_CLRr,reg_field,&reset_status))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
    }

    /* Wait operation completed */
    count = 0;
    do
    {
        if ((ret = reg_field_read(RTL9602C_NAT_TBL_ACCESS_CLRr, reg_field, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
      count++;
    } while (busy && count<=100);

    return RT_ERR_OK;
}/* end of dal_rtl9602c_l34_table_reset */



/* Function Name:
 *      dal_rtl9602c_l34_wanTypeTable_set
 * Description:
 *      Set WAN type entry by idx.
 * Input:
 *      idx, *entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    rtl9602c_l34_wan_type_entry_t wanType_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->nhIdx >= HAL_L34_NH_ENTRY_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->wanType >= L34_WAN_TYPE_END), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&wanType_entry, 0x0, sizeof(rtl9602c_l34_wan_type_entry_t));

    tmpVal = entry->nhIdx;
    if ((ret = table_field_set(RTL9602C_WAN_TYPE_TABLEt, RTL9602C_WAN_TYPE_TABLE_NXHOPTBIDXtf, (uint32 *)&tmpVal, (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->wanType;
    if ((ret = table_field_set(RTL9602C_WAN_TYPE_TABLEt, RTL9602C_WAN_TYPE_TABLE_WAN_TYPEtf, (uint32 *)&tmpVal, (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_WAN_TYPE_TABLEt, idx, (uint32 *)&wanType_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_wanTypeTable_set */


/* Function Name:
 *      dal_rtl9602c_l34_wanTypeTable_get
 * Description:
 *      Get a WAN type entry by idx.
 * Input:
 *      idx
 * Output:
 *      *entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_wan_type_entry_t wanType_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&wanType_entry, 0x0, sizeof(rtl9602c_l34_wan_type_entry_t));

    if ((ret = table_read(RTL9602C_WAN_TYPE_TABLEt, idx, (uint32 *)&wanType_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_WAN_TYPE_TABLEt, RTL9602C_WAN_TYPE_TABLE_NXHOPTBIDXtf, (uint32 *)&(tmp_val), (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(RTL9602C_WAN_TYPE_TABLEt, RTL9602C_WAN_TYPE_TABLE_WAN_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->wanType = tmp_val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_wanTypeTable_get */



/* Function Name:
 *      dal_rtl9602c_l34_bindingTable_set
 * Description:
 *      Set binding table entry by idx.
 * Input:
 *      idx, *entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    rtl9602c_l34_binding_entry_t bindingEntry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_BINDING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->wanTypeIdx >= RTL9602C_L34_WAN_TYPE_TABLE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->vidLan > RTK_VLAN_ID_MAX), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&bindingEntry, 0x0, sizeof(rtl9602c_l34_binding_entry_t));

    tmpVal = entry->wanTypeIdx;
    if ((ret = table_field_set(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_WAN_TYPE_INDEXtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->vidLan;
    if ((ret = table_field_set(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_VID_LANtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->portMask.bits[0];
    if ((ret = table_field_set(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_PORT_MASKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->extPortMask.bits[0];
    if ((ret = table_field_set(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_EXT_PMSKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->bindProto;
    if ((ret = table_field_set(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_BIND_PTLtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_BINDING_TABLEt, idx, (uint32 *)&bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_bindingTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_bindingTable_get
 * Description:
 *      Get a binding table entry by idx.
 * Input:
 *      idx
 * Output:
 *      *entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_binding_entry_t bindingEntry;
    uint32 tmpVal;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_BINDING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&bindingEntry, 0x0, sizeof(rtl9602c_l34_binding_entry_t));

    if ((ret = table_read(RTL9602C_BINDING_TABLEt, idx, (uint32 *)&bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_WAN_TYPE_INDEXtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->wanTypeIdx = tmpVal;

    if ((ret = table_field_get(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_VID_LANtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->vidLan = tmpVal;

    if ((ret = table_field_get(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_PORT_MASKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->portMask.bits[0] = tmpVal;

    if ((ret = table_field_get(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_EXT_PMSKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->extPortMask.bits[0] = tmpVal;


    /*binding protocal*/
    if ((ret = table_field_get(RTL9602C_BINDING_TABLEt, RTL9602C_BINDING_TABLE_BIND_PTLtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->bindProto = tmpVal;




    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_bindingTable_get */


/* Function Name:
 *      dal_rtl9602c_l34_ipv6NeighborTable_set
 * Description:
 *      Set IPv6 neighbor table entry by idx.
 * Input:
 *      idx, *entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */


int32
dal_rtl9602c_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    uint64 tmpVal64;
    rtl9602c_l34_ipv6_neighbor_entry_t rawEntry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX() <= entry->l2Idx), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(rtl9602c_l34_ipv6_neighbor_entry_t));

    tmpVal = entry->valid;
    if ((ret = table_field_set(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->l2Idx;
    if ((ret = table_field_set(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_L2_TABLE_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipv6RouteIdx;
    if ((ret = table_field_set(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_RT_MATCH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    memcpy(&tmpVal64,&(entry->ipv6Ifid),sizeof(uint64));

    if ((ret = table_field_byte_set(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_IP6IF_IDtf, (uint8 *)&tmpVal64,(uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_NEIGHBOR_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_ipv6NeighborTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_ipv6NeighborTable_get
 * Description:
 *      Get a IPv6 neighbor table entry by idx.
 * Input:
 *      idx
 * Output:
 *      *entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */


int32
dal_rtl9602c_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_ipv6_neighbor_entry_t rawEntry;
    uint32 tmpVal = 0;
    uint64 tmpVal64 = 0;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(rtl9602c_l34_ipv6_neighbor_entry_t));

    if ((ret = table_read(RTL9602C_NEIGHBOR_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;


    if ((ret = table_field_get(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_L2_TABLE_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->l2Idx = tmpVal;

    if ((ret = table_field_get(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_RT_MATCH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->ipv6RouteIdx = tmpVal;

    if ((ret = table_field_byte_get(RTL9602C_NEIGHBOR_TABLEt, RTL9602C_NEIGHBOR_TABLE_IP6IF_IDtf, (uint8 *)&tmpVal64, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    memcpy(&(entry->ipv6Ifid),&tmpVal64,sizeof( uint64));

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_ipv6NeighborTable_get */



/* Function Name:
 *      dal_rtl9602c_l34_ipv6RoutingTable_set
 * Description:
 *      Set a IPv6 routing entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    rtl9602c_l34_ipv6_routing_entry_t rawEntry;
    rtk_ipv6_addr_t     ipv6Addr;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(rtl9602c_l34_ipv6_routing_entry_t));

    tmpVal = entry->valid;
    if ((ret = table_field_set(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->type;
    if ((ret = table_field_set(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->nhOrIfidIdx;
    if ((ret = table_field_set(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_NEXTHOPtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipv6PrefixLen;
    if ((ret = table_field_set(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_IP6_PREFIX_LENtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmpVal = entry->rt2waninf;
    if ((ret = table_field_set(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&ipv6Addr,&entry->ipv6Addr,sizeof(rtk_ipv6_addr_t));
    if ((ret = table_field_byte_set(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_IP6_DIPtf, (uint8 *)&ipv6Addr, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(RTL9602C_IPV6_ROUTING_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_ipv6RoutingTable_set */



/* Function Name:
 *      dal_rtl9602c_l34_ipv6RoutingTable_get
 * Description:
 *      Get a IPv6 routing entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_ipv6RoutingTable_get(uint32 idx,rtk_ipv6Routing_entry_t *entry)
{
    int32 ret;
    rtl9602c_l34_ipv6_routing_entry_t rawEntry;
    uint32 tmpVal;
    rtk_ipv6_addr_t     ipv6Addr;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(rtl9602c_l34_ipv6_routing_entry_t));

    if ((ret = table_read(RTL9602C_IPV6_ROUTING_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }


    if ((ret = table_field_get(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;

    if ((ret = table_field_get(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->type = tmpVal;

    if ((ret = table_field_get(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_NEXTHOPtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->nhOrIfidIdx = tmpVal;

    if ((ret = table_field_get(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_IP6_PREFIX_LENtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->ipv6PrefixLen = tmpVal;

    if ((ret = table_field_get(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    entry->rt2waninf = tmpVal;

    if ((ret = table_field_byte_get(RTL9602C_IPV6_ROUTING_TABLEt, RTL9602C_IPV6_ROUTING_TABLE_IP6_DIPtf, (uint8 *)&ipv6Addr, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    memcpy(&(entry->ipv6Addr),&ipv6Addr,sizeof(rtk_ipv6_addr_t));

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_ipv6RoutingTable_get */

/* Function Name:
 *      dal_rtl9602c_l34_ipmcTransTable_set
 * Description:
 * Input:
 *      idx     - index
 *      pEntry  - IPMC Translation entry
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    int32 ret;
    uint32 tmpVal;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_IPMC_TRANS_MAX < idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= pEntry->netifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= pEntry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX() <= pEntry->extipIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->sipTransEnable), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_PPPOE_ACT_END <= pEntry->pppoeAct), RT_ERR_INPUT);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pEntry->untagMbr)), RT_ERR_PORT_MASK);

    tmpVal = pEntry->netifIdx;
    if ((ret = reg_array_field_write(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_NETIF_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = pEntry->sipTransEnable;
    if ((ret = reg_array_field_write(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_EN_SIP_TRANSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = pEntry->extipIdx;
    if ((ret = reg_array_field_write(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_EXT_IP_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    switch(pEntry->pppoeAct)
    {
        case L34_PPPOE_ACT_NO:
            tmpVal = RTL9602C_L34_IPMC_PPPOE_ACT_NO;
        break;
        case L34_PPPOE_ACT_KEEP:
            tmpVal = RTL9602C_L34_IPMC_PPPOE_ACT_KEEP;
        break;
        case L34_PPPOE_ACT_MODIFY:
            tmpVal = RTL9602C_L34_IPMC_PPPOE_ACT_MODIFY;
        break;
        case L34_PPPOE_ACT_REMOVE:
            tmpVal = RTL9602C_L34_IPMC_PPPOE_ACT_REMOVE;
        break;
        default:
            return  RT_ERR_INPUT;
            break;
    }
    if ((ret = reg_array_field_write(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_PPPOE_ACTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = pEntry->pppoeIdx;
    if ((ret = reg_array_field_write(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_PPPOE_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = pEntry->untagMbr.bits[0];
    if ((ret = reg_array_field_write(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_UNTAGMBRf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9602c_l34_ipmcTransTable_set */


/* Function Name:
 *      dal_rtl9602c_l34_ipmcTransTable_get
 * Description:
 * Input:
 *      idx     - index
 * Output:
 *      pEntry  - IPMC Translation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    int32 ret;
    uint32 tmpVal;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_IPMC_TRANS_MAX < idx), RT_ERR_ENTRY_INDEX);

    if ((ret = reg_array_field_read(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_NETIF_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    pEntry->netifIdx = tmpVal;


    if ((ret = reg_array_field_read(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_EN_SIP_TRANSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    pEntry->sipTransEnable = tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_EXT_IP_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    pEntry->extipIdx = tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_PPPOE_ACTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    switch(tmpVal)
    {
        case RTL9602C_L34_IPMC_PPPOE_ACT_NO:
            pEntry->pppoeAct = L34_PPPOE_ACT_NO;
        break;
        case RTL9602C_L34_IPMC_PPPOE_ACT_KEEP:
            pEntry->pppoeAct = L34_PPPOE_ACT_KEEP;
        break;
        case RTL9602C_L34_IPMC_PPPOE_ACT_MODIFY:
            pEntry->pppoeAct = L34_PPPOE_ACT_MODIFY;
        break;
        case RTL9602C_L34_IPMC_PPPOE_ACT_REMOVE:
            pEntry->pppoeAct = L34_PPPOE_ACT_REMOVE;
        break;
        default:
            return  RT_ERR_INPUT;
        break;
    }

    if ((ret = reg_array_field_read(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_PPPOE_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    pEntry->pppoeIdx = tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_UNTAGMBRf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    pEntry->untagMbr.bits[0] = tmpVal;

    return RT_ERR_OK;

} /* end of dal_rtl9602c_l34_ipmcTransTable_get */

/* Function Name:
 *      dal_rtl9602c_l34_bindingAction_set
 * Description:
 *     Set binding action
 * Input:
 *      bindType - binding type
 *      action - binding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_bindingAction_set(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t action)
{
    int32 ret;
    uint32 rawAct,rawField, tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "bindType=%d,action=%d",bindType, action);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_BIND_TYPE_END <=bindType), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_BIND_ACT_END <=action), RT_ERR_INPUT);

    /* function body */
    if((ret = _dal_rtl9602c_l34_bindingAct_cfg2raw(bindType,action,&rawField,&rawAct))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }
    /*set to binding act*/
    tmpVal = rawAct;
    if ((ret = reg_field_write(RTL9602C_BD_CFGr, rawField, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_bindingAction_set */


/* Function Name:
 *      dal_rtl9602c_l34_bindingAction_get
 * Description:
 *      Get binding action
 * Input:
 *      bindType - binding type
 * Output:
 *      *pAction - binding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_bindingAction_get(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t *pAction)
{
    int32 ret;
    uint32 rawField,rawAct,tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "bindType=%d",bindType);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_BIND_TYPE_END <=bindType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = _dal_rtl9602c_l34_binding_type_field_get(bindType,&rawField))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }
    /*get to binding act*/
    if ((ret = reg_field_read(RTL9602C_BD_CFGr, rawField, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    rawAct = tmpVal;

    if((ret = _dal_rtl9602c_l34_bindingAct_raw2cfg(bindType,rawAct,pAction))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_bindingAction_get */

/* Function Name:
 *      dal_rtl9602c_l34_portWanMap_set
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      portWanMapType: port wan mapping type
 *    portWanMapEntry: port wan mapping entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_portWanMap_set(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t portWanMapEntry)
{
    int32 ret;
    uint32 writeVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "dal_rtl9602c_l34_portWanMap_set");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_PORTMAP_TYPE_END <=portWanMapType), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_PORTMAP_ACT_END <=portWanMapEntry.act), RT_ERR_INPUT);
    RT_PARAM_CHK(( HAL_L34_NETIF_ENTRY_MAX()<=portWanMapEntry.wanIdx), RT_ERR_INPUT);

    switch(portWanMapEntry.act){
    case L34_PORTMAP_ACT_DROP:
        writeVal = 0;
        break;
    case L34_PORTMAP_ACT_PERMIT:
        writeVal = 1;
        break;
    default:
        return RT_ERR_INPUT;
        break;
    }

    switch(portWanMapType){
    case L34_PORTMAP_TYPE_PORT2WAN:
        RT_PARAM_CHK((HAL_GET_MAX_PORT()< portWanMapEntry.port),RT_ERR_INPUT);
        if ((ret = reg_array_field_write(RTL9602C_L34_PORT_TO_WANr, portWanMapEntry.port, portWanMapEntry.wanIdx, RTL9602C_PORT_TO_WAN_PERMITf, &writeVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        break;

    case L34_PORTMAP_TYPE_WAN2PORT:
        RT_PARAM_CHK((HAL_GET_MAX_PORT()< portWanMapEntry.port),RT_ERR_INPUT);
        if ((ret = reg_array_field_write(RTL9602C_L34_WAN_TO_PORTr, portWanMapEntry.port, portWanMapEntry.wanIdx, RTL9602C_WAN_TO_PORT_PERMITf, &writeVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        break;
    default:
        return RT_ERR_INPUT;
        break;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_portWanMap_set */


/* Function Name:
 *      dal_rtl9602c_l34_portWanMap_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      portWanMapType: port wan mapping type
 * Output:
 *    pPortWanMapEntry: point of port wan mapping entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_portWanMap_get(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t *pPortWanMapEntry)
{
    int32 ret;
    uint32 readVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "portWanMapType=%d",portWanMapType);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_PORTMAP_TYPE_END <=portWanMapType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortWanMapEntry), RT_ERR_NULL_POINTER);

    switch(portWanMapType){
    case L34_PORTMAP_TYPE_PORT2WAN:
        RT_PARAM_CHK((HAL_GET_MAX_PORT()< pPortWanMapEntry->port),RT_ERR_INPUT);
        if ((ret = reg_array_field_read(RTL9602C_L34_PORT_TO_WANr, pPortWanMapEntry->port, pPortWanMapEntry->wanIdx, RTL9602C_PORT_TO_WAN_PERMITf, &readVal)) != RT_ERR_OK)
        {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
        }
        break;
    case L34_PORTMAP_TYPE_WAN2PORT:
        RT_PARAM_CHK((HAL_GET_MAX_PORT()< pPortWanMapEntry->port),RT_ERR_INPUT);
        if ((ret = reg_array_field_read(RTL9602C_L34_WAN_TO_PORTr, pPortWanMapEntry->port, pPortWanMapEntry->wanIdx, RTL9602C_WAN_TO_PORT_PERMITf, &readVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        break;
    default:
        return RT_ERR_INPUT;
        break;
    }


    switch(readVal){
    case 0:
        pPortWanMapEntry->act = L34_PORTMAP_ACT_DROP;
        break;
    case 1:
        pPortWanMapEntry->act = L34_PORTMAP_ACT_PERMIT;
        break;
    default:
        return RT_ERR_INPUT;
        break;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_portWanMap_get */


/* Function Name:
 *      dal_rtl9602c_l34_hsbaMode_set
 * Description:
 *      Set L34 hsba mode
 * Input:
 *      hsbaMode - L34 hsba
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsbaMode_set(rtk_l34_hsba_mode_t hsbaMode)
{
    int32 ret;
    rtl9602c_l34_hsba_mode_t mode;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "hsbaMode=%d",hsbaMode);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_HSBA_END <=hsbaMode), RT_ERR_INPUT);

    RT_ERR_CHK(_dal_rtl9602c_l34_hsbaMode_cfg2raw(hsbaMode,&mode), ret);

    /* function body */
    if ((ret = reg_field_write(RTL9602C_HSBA_CTRLr, RTL9602C_TST_LOG_MDf, (uint32 *)&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9602c_l34_hsbaMode_get
 * Description:
 *      Get L34 hsba mode
 * Input:
 *      None
 * Output:
 *      *pHsbaMode - point of L34 hsab
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsbaMode_get(rtk_l34_hsba_mode_t *pHsbaMode)
{
    int32 ret;
    uint32 tmp_val;
    rtl9602c_l34_hsba_mode_t mode;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsbaMode), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_HSBA_CTRLr, RTL9602C_TST_LOG_MDf, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    mode = (rtl9602c_l34_hsba_mode_t)tmp_val;

    _dal_rtl9602c_l34_hsbaMode_raw2cfg(mode,pHsbaMode);


    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_hsabMode_get */



/* Function Name:
 *      dal_rtl9602c_l34_hsaData_get
 * Description:
 *      Get L34 hsa data
 * Input:
 *      None
 * Output:
 *      pHsaData - point of hsa data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsaData_get(rtk_l34_hsa_t *pHsaData)
{
    int32 ret;
    int32 index;
    uint32 tmp_val,*tmp_val_ptr;
    ipaddr_t tmp_ip;
    rtl9602c_l34_hsa_entry_t hsba_entry,tmp_hsba_entry;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsaData), RT_ERR_NULL_POINTER);

    /* function body */

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &tmp_hsba_entry;
    for(index=0 ; index<(sizeof(rtl9602c_l34_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_read((RTL9602C_HSA_DESC_W0r + index), tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    /* prepare data for reading */
    for(index=0 ; index< (sizeof(rtl9602c_l34_hsa_entry_t)/4) ; index++)
    {
        hsba_entry.entry_data[sizeof(rtl9602c_l34_hsa_entry_t)/4 - 1 - index] = tmp_hsba_entry.entry_data[index];
    }

    /*get field data from hsba buffer*/
    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_DSOUT_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->dslite_Idx= tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_DSOUTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->dslite_valid = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_IS_POLICYtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->is_policy = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_PPPOE_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->pppoeKeep = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_BIND_VIDTRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->bindVidTrans = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_INTERNAL_VLAN_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->interVlanIf = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_REASONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->reason = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_L3L4TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->l34trans = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_L2TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->l2trans = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_ACTIONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->action = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_L4HP_Vtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->l4_pri_valid = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_L4HPtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->l4_pri_sel = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_FRAGtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->frag = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_DIFIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->difid = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->pppoe_if = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_PPPID_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->pppid_idx = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_NEXTHOP_MAC_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->nexthop_mac_idx = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_L4_CHECKSUM_OFFSETtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->l4_chksum = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_L3_CHECKSUM_OFFSETtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->l3_chksum = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_DVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->dvid = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_PORTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->port = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSAt, RTL9602C_L34_HSA_IPtf, (uint32 *)&tmp_ip, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsaData->ip = tmp_ip ;


    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_hsaData_get */


/* Function Name:
 *      dal_rtl9602c_l34_hsbData_get
 * Description:
 *      Get L34 hsb data
 * Input:
 *      None
 * Output:
 *      pHsaData - point of hsa data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsbData_get(rtk_l34_hsb_t *pHsbData)
{
    int32 ret, index;
    uint32 tmp_val,*tmp_val_ptr;
    ipaddr_t tmp_ip;
    rtk_ipv6_addr_t  tmpIpv6Addr;
    rtl9602c_l34_hsb_entry_t hsba_entry,tmp_hsba_entry;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsbData), RT_ERR_NULL_POINTER);

    /* function body */
    /*read data from register*/
    tmp_val_ptr = (uint32 *) &tmp_hsba_entry;
    for(index=0 ; index<(sizeof(rtl9602c_l34_hsb_entry_t)/4) ; index++)
    {
        if ((ret = reg_read((RTL9602C_HSB_DESC_W0r + index), tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    /*Prepare the data for reading*/
    for(index=0 ; index< (sizeof(rtl9602c_l34_hsb_entry_t)/4) ; index++)
    {
        hsba_entry.entry_data[sizeof(rtl9602c_l34_hsb_entry_t)/4 - 1 - index] = tmp_hsba_entry.entry_data[index];
    }

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_DMAC_Ttf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->da_traslate = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_CFI_1tf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->cfi_1 = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SA_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->sa_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SA_IDX_VLDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->sa_idx_valid = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_IP6_TYPE_EXTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->ip6_type_ext = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_POLICY_NH_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->policy_nh_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_IS_POLICY_ROUTEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->is_policy_route = tmp_val;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_IS_FROM_WAN_PORTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->isFromWan = tmp_val ;


    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_L2BRIDGEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->l2bridge = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_IPFRAGOFStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->ipfrag_s = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_IPMFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->ipmf = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_L4CSOKtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->l4_chksum_ok = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_L3CSOKtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->l3_chksum_ok = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_CPU_DIRECT_TXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->cpu_direct_tx = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_UDP_NOCStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->udp_no_chksum = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_PARS_FAILtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->parse_fail = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->pppoe_if = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SVLAN_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->svlan_if = tmp_val ;


    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_TTLSTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->ttls = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->type = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_TCP_FLAG_PROTOCOL_PARSING_FAILURE_REASONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->tcp_flag = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_CVLAN_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->cvlan_if = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->spa = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_CVLANIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->cvid = tmp_val ;


    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->len = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_DPORT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->dport_l4chksum = tmp_val ;

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_PPPOE_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->pppoe_id = tmp_val ;


    if ((ret = table_field_byte_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_DIPtf, (uint8 *)&tmpIpv6Addr, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    memcpy(&(pHsbData->dip),&tmpIpv6Addr,sizeof(rtk_ipv6_addr_t));

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SIPtf, (uint32 *)&tmp_ip, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    memcpy(&(pHsbData->sip.ipv6_addr[12]),&tmp_ip,4);

    if ((ret = table_field_byte_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SIP_127_32tf, (uint8 *)&tmpIpv6Addr, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    memcpy(&(pHsbData->sip),&tmpIpv6Addr,sizeof(rtk_ipv6_addr_t)-4);

    if ((ret = table_field_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_SPORT_ICMPID_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    pHsbData->sport_icmpid_chksum = tmp_val ;

    if ((ret = table_field_mac_get(RTL9602C_L34_HSBt, RTL9602C_L34_HSB_DMACtf, (uint8 *)&pHsbData->dmac, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_hsbData_get */


/* Sub-module Name: System configuration */

/* Function Name:
 *      dal_rtl9602c_l34_globalState_set
 * Description:
 *      get l34 global status
 * Input:
 *    stateType -status type
 *    state     - status of state type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_globalState_set(rtk_l34_globalStateType_t stateType,rtk_enable_t state)
{

    int32 ret;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "stateType=%d,state=%d",stateType, state);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_GLOBAL_STATE_END <=stateType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    switch(stateType){
    case L34_GLOBAL_L34_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_L34_GLB_CFGr, RTL9602C_L34_GLOBAL_CFGf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_L3NAT_STATE:
    {
        rtl9602c_l34_mode_t l34mode;

        if((ret=_dal_rtl9602c_l34_mode_get(&l34mode))!=RT_ERR_OK){
            RT_ERR(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        l34mode = state | (l34mode & 2);

        if((ret=_dal_rtl9602c_l34_mode_set(l34mode))!=RT_ERR_OK){
            RT_ERR(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    case L34_GLOBAL_L4NAT_STATE:
    {
        rtl9602c_l34_mode_t l34mode;

        if((ret=_dal_rtl9602c_l34_mode_get(&l34mode))!=RT_ERR_OK){
            RT_ERR(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        l34mode = (state << 1) | (l34mode & 1);

        if((ret=_dal_rtl9602c_l34_mode_set(l34mode))!=RT_ERR_OK){
            RT_ERR(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    case L34_GLOBAL_L3CHKSERRALLOW_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_L3CHKSERRALLOWf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_L4CHKSERRALLOW_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_L4CHKSERRALLOWf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_TTLMINUS_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_TTL_1ENABLEf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }

    break;
    case L34_GLOBAL_NAT2LOG_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_ENNATT2LOGf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_FRAG2CPU_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_FRAGMENT2CPUf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_BIND_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_V6_BD_CTLr, RTL9602C_PB_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_V4FLOW_RT_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_V4FLRT_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_V6FLOW_RT_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_V6FLRT_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_IPMC_TTLMINUS_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_L34_GLB_CFGr, RTL9602C_IP_MCST_TTL_1f, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_SIP_ARP_TRF_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_CF_SIP_ARP_TRF_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    case L34_GLOBAL_DIP_ARP_TRF_STATE:
        tmpVal = (state == DISABLED) ? 0 : 1;
        if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_CF_DIP_ARP_TRF_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }

    return RT_ERR_OK;

}   /* end of dal_rtl9602c_l34_globalState_set */


/* Function Name:
 *      dal_rtl9602c_l34_globalState_get
 * Description:
 *      set l34 global status
 * Input:
 *    stateType -status type
 * Output:
 *    pState        - status of state type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_globalState_get(rtk_l34_globalStateType_t stateType,rtk_enable_t *pState)
{

    int32 ret;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "stateType=%d",stateType);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_GLOBAL_STATE_END <=stateType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    switch(stateType){
    case L34_GLOBAL_L34_STATE:
        if ((ret = reg_field_read(RTL9602C_L34_GLB_CFGr, RTL9602C_L34_GLOBAL_CFGf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_L3NAT_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_NATMODEf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = tmpVal & 1;
    break;
    case L34_GLOBAL_L4NAT_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_NATMODEf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal >> 1) & 1;
    break;
    case L34_GLOBAL_L3CHKSERRALLOW_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_L3CHKSERRALLOWf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_L4CHKSERRALLOW_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_L4CHKSERRALLOWf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_NAT2LOG_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_ENNATT2LOGf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_FRAG2CPU_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_FRAGMENT2CPUf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_TTLMINUS_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_TTL_1ENABLEf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_BIND_STATE:
        if ((ret = reg_field_read(RTL9602C_V6_BD_CTLr, RTL9602C_PB_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_V4FLOW_RT_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_V4FLRT_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_V6FLOW_RT_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_V6FLRT_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_IPMC_TTLMINUS_STATE:
        if ((ret = reg_field_read(RTL9602C_L34_GLB_CFGr, RTL9602C_IP_MCST_TTL_1f, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_SIP_ARP_TRF_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_CF_SIP_ARP_TRF_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    case L34_GLOBAL_DIP_ARP_TRF_STATE:
        if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_CF_DIP_ARP_TRF_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        *pState = (tmpVal == 0) ? DISABLED : ENABLED;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_globalState_get */



/* Function Name:
 *      dal_rtl9602c_l34_lookupMode_set
 * Description:
 *      configure l34 lookup mode selection
 * Input:
 *    lookupMode    - mode of l34 lookup method
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_lookupMode_set(rtk_l34_lookupMode_t lookupMode)
{
    int32 ret;
    uint32 tmp_val;
    rtl9602c_l34_limbc_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "lookupMode=%d",lookupMode);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_LOOKUP_MODE_END <=lookupMode), RT_ERR_INPUT);

    if((ret= _dal_rtl9602c_l34_lookupMode_cfg2raw(lookupMode,&mode)) != RT_ERR_OK){
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }

    tmp_val = mode;
    if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_LIMDBCf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}   /* end of dal_rtl9602c_l34_lookupMode_set */


/* Function Name:
 *      dal_rtl9602c_l34_lookupMode_get
 * Description:
 *      get l34 lookup mode selection
 * Input:
 *      None
 * Output:
 *    pLookupMode   - point of mode of l34 lookup method
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_lookupMode_get(rtk_l34_lookupMode_t *pLookupMode)
{
    int32 ret;
    uint32 tmp_val;
    rtl9602c_l34_limbc_t mode;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLookupMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_LIMDBCf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    mode = tmp_val;

    if((ret = _dal_rtl9602c_l34_lookupMode_raw2cfg(mode,pLookupMode)) != RT_ERR_OK){
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_lookupMode_get */


/* Function Name:
 *      dal_rtl9602c_l34_wanRoutMode_set
 * Description:
 *      set wan route mode
 * Input:
 *    wanRouteMode  - mode of wan routed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_wanRoutMode_set(rtk_l34_wanRouteMode_t wanRouteMode)
{
    int32 ret;
    rtl9602c_rtk_l34_wanroute_act_t raw_act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "wanRouteMode=%d",wanRouteMode);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_WANROUTE_END <=wanRouteMode), RT_ERR_INPUT);

    /* function body */
    switch(wanRouteMode)
    {
        case L34_WANROUTE_FORWARD:
            raw_act = RTL9602C_L34_WAN_ROUTE_FWD;
        break;
        case L34_WANROUTE_FORWARD2CPU:
            raw_act = RTL9602C_L34_WAN_ROUTE_FWD_TO_CPU;
        break;
        case L34_WANROUTE_DROP:
            raw_act = RTL9602C_L34_WAN_ROUTE_DROP;
        break;
        default:
            return  RT_ERR_INPUT;
        break;
    }

    if ((ret = reg_field_write(RTL9602C_SWTCR0r, RTL9602C_WANROUTEMODEf, (uint32 *)&raw_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_wanRoutMode_set */


/* Function Name:
 *      dal_rtl9602c_l34_wanRoutMode_get
 * Description:
 *      get  wan route mode
 * Input:
 *      None
 * Output:
 *    *pWanRouteMode    - point of mode of wan routed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_wanRoutMode_get(rtk_l34_wanRouteMode_t *pWanRouteMode)
{
    int32 ret;
    rtl9602c_rtk_l34_wanroute_act_t raw_act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWanRouteMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_SWTCR0r, RTL9602C_WANROUTEMODEf, (uint32 *)&raw_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    switch(raw_act)
    {
        case RTL9602C_L34_WAN_ROUTE_FWD:
            *pWanRouteMode = L34_WANROUTE_FORWARD;
            break;

        case RTL9602C_L34_WAN_ROUTE_FWD_TO_CPU:
            *pWanRouteMode = L34_WANROUTE_FORWARD2CPU;
            break;

        case RTL9602C_L34_WAN_ROUTE_DROP:
            *pWanRouteMode = L34_WANROUTE_DROP ;
            break;

        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_wanRoutMode_get */


/* Function Name:
 *      dal_rtl9602c_l34_arpTrfIndicator_get
 * Description:
 *      get  arp entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 *    *pArpIndicator    - point of traffic indicator for arp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_arpTrfIndicator_get(uint32 index, rtk_enable_t *pArpIndicator)
{
    int32 ret;
    int tableId, indexId;
    uint32 tmp_val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pArpIndicator), RT_ERR_NULL_POINTER);

    tableId = index / 32;
    indexId = index % 32;
    if ((ret = reg_read((RTL9602C_ARP_TRF0r + tableId), &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *pArpIndicator = (tmp_val & (1<< indexId)) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_arpTrfIndicator_get */


/* Function Name:
 *      dal_rtl9602c_l34_naptTrfIndicator_get
 * Description:
 *      get  napt entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 *    *pNaptIndicator   - point of traffic indicator for arp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_naptTrfIndicator_get(uint32 index, rtk_enable_t *pNaptIndicator)
{
    int32 ret;
    uint32 tmp_val;
    int tableId, indexId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNaptIndicator), RT_ERR_NULL_POINTER);


    tableId = index / 32;
    indexId = index % 32;

    if((ret = reg_array_field_read(RTL9602C_L4_TRFr, REG_ARRAY_INDEX_NONE, tableId, RTL9602C_TRFf, &tmp_val)) != RT_ERR_OK){
             RT_ERR(ret, (MOD_L34|MOD_DAL), "");
             return ret;
    }
    *pNaptIndicator = (tmp_val & (1<< indexId)) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_naptTrfIndicator_get */



/* Function Name:
 *      dal_rtl9602c_l34_pppTrfIndicator_get
 * Description:
 *      get  ppp entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 *    *pPppIndicator    - point of traffic indicator for pppoe table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_pppTrfIndicator_get(uint32 index, rtk_enable_t *pPppIndicator)
{
    int32 ret;
    uint8 pppInd;
    uint32 tmp_val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPppIndicator), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_PP_AGEr, RTL9602C_PPPOE_TRF_BMPf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    pppInd = (uint8)tmp_val;

    *pPppIndicator = (pppInd & (1 << index)) ? ENABLED: DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_pppTrfIndicator_get */

/* Function Name:
 *      dal_rtl9602c_l34_neighTrfIndicator_get
 * Description:
 *      get  ipv6 neighbor entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 *    *pNeighIndicator  - point of traffic indicator for neighbor
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_neighTrfIndicator_get(uint32 index, rtk_enable_t *pNeighIndicator)
{
    int32 ret;
    uint32 tmpVal;
    uint32 tableId,indexId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_IPV6_NEIGHBOR_ENTRY_MAX() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNeighIndicator), RT_ERR_NULL_POINTER);

    tableId  = index/32;
    indexId = index%32;
    if ((ret = reg_read((RTL9602C_NB_TRF0r + tableId), &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *pNeighIndicator = (tmpVal & (1<< indexId)) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_neighTrfIndicator_get */

/* Function Name:
 *      dal_apollo_l34_hsdState_set
 * Description:
 *      Set L34 hsd state
 * Input:
 *      hsdState - L34 hsd state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsdState_set(rtk_enable_t hsdState)
{
    int32 ret;
    uint32 pValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "hsdState=%d",hsdState);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= hsdState), RT_ERR_INPUT);

    /* function body */
    if(hsdState == ENABLED)
        pValue = 0xf5;
    else
        pValue = 0;

    if((ret = reg_field_write(RTL9602C_RSVD_ALE_HSAr, RTL9602C_RSVD_MEMf, &pValue))!=RT_ERR_OK)
    {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hsdState_set */


/* Function Name:
 *      dal_apollo_l34_hsdState_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      phsdState - point of hsd state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_hsdState_get(rtk_enable_t *phsdState)
{
    int32 ret;
    uint32 pValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == phsdState), RT_ERR_NULL_POINTER);

    /* function body */
  if((ret = reg_field_read(RTL9602C_RSVD_ALE_HSAr, RTL9602C_RSVD_MEMf, &pValue))!=RT_ERR_OK)
    {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }

    if(pValue == 0x5f)
        *phsdState = ENABLED;
    else
        *phsdState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hsdState_get */


/* Function Name:
 *      dal_rtl9602c_l34_ip6mcRoutingTransIdx_set
 * Description:
 *      Set per-port IPv6 Multicast routing translation table index
 * Input:
 *      idx          - table index
 *      port         - port id
 *      ipmcTransIdx - ip multicast translation table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_ip6mcRoutingTransIdx_set(uint32 idx, rtk_port_t port, uint32 ipmcTransIdx)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    RT_PARAM_CHK((ipmcTransIdx > RTL9602C_IPTRANS_IDX_MAX), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((idx > RTL9602C_L3MCR_IDX_MAX), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_write(RTL9602C_L3MCR_TBLr, idx, port, RTL9602C_INDEXf, &ipmcTransIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_ip6mcRouting_set */


/* Function Name:
 *      dal_rtl9602c_l34_ip6mcRoutingTransIdx_get
 * Description:
 *      Get per-port IPv6 Multicast routing translation table index
 * Input:
 *      idx          - table index
 *      port         - port id
 * Output:
 *      pIpmcTransIdx - ip multicast translation table index to be get
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_ip6mcRoutingTransIdx_get(uint32 idx, rtk_port_t port, uint32 *pIpmcTransIdx)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    RT_PARAM_CHK((NULL == pIpmcTransIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((idx > RTL9602C_L3MCR_IDX_MAX), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(RTL9602C_L3MCR_TBLr, idx, port, RTL9602C_INDEXf, pIpmcTransIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_l34_ip6mcRouting_get */

/* Function Name:
 *      dal_rtl9602c_l34_flowRouteTable_set
 * Description:
 *      Set Flow Routing Table.
 * Input:
 *      entry  : entry content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_flowRouteTable_set(rtk_l34_flowRoute_entry_t *pEntry)
{
    int32 ret;
    uint32 tmp_val;
    uint32 idx;
    rtl9602c_l34_flow_route_entry_t flow_entry;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_FLOW_ROUTE_TABLE_MAX <= pEntry->index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((1 < pEntry->isIpv4), RT_ERR_OUT_OF_RANGE);
    if(pEntry->isIpv4 == 0) /*If IPv6 entry, index must be even number*/
    {
        RT_PARAM_CHK((1 ==(pEntry->index%2)), RT_ERR_ENTRY_INDEX);
    }
    RT_PARAM_CHK((RTL9602C_L34_NH_TABLE_MAX <= pEntry->nexthopIdx), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((1 < pEntry->valid), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((1 < pEntry->isTcp), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((1 < pEntry->pri_valid), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((7 < pEntry->priority), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((L34_FLOWRT_TYPE_END <= pEntry->process), RT_ERR_OUT_OF_RANGE);

    osal_memset(&flow_entry, 0x0, sizeof(rtl9602c_l34_flow_route_entry_t));

    /*IPv4*/
    if(pEntry->isIpv4 == 1)
    {
        tmp_val = (uint32)pEntry->isIpv4;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_IPV4tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->valid;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_VALIDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->sport;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_SPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->dport;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_DPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->isTcp;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_TCPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->nexthopIdx;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_NEXTHOPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        _dal_rtl9602c_l34_flowRouteType_cfg2raw(pEntry->process, &tmp_val);
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_PROCESStf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->sip.ipv4;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_SIPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->dip.ipv4;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_DIPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->pri_valid;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_PRI_VLDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->priority;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_PRItf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        idx = pEntry->index;
        if ((ret = table_write(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, idx, (uint32 *)&flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
    }
    else /*IPv6*/
    {
        rtk_ipv6_addr_t     dipIpv6, sipIpv6;
        uint32 i;

        tmp_val = (uint32)pEntry->isIpv4;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_IPV4tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->valid;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_VALIDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->sport;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_SPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->dport;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_DPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->isTcp;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_TCPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->nexthopIdx;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_NEXTHOPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        _dal_rtl9602c_l34_flowRouteType_cfg2raw(pEntry->process, &tmp_val);
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_PROCESStf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->pri_valid;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_PRI_VLDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->priority;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_PRItf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        osal_memcpy(&sipIpv6,&pEntry->sip.ipv6, sizeof(rtk_ipv6_addr_t));
        if ((ret = table_field_byte_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_SIP_31_0tf, (uint8 *)&sipIpv6.ipv6_addr[12], (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = ((uint32)(sipIpv6.ipv6_addr[9] & 0x3F) << 16 ) | ((uint32)sipIpv6.ipv6_addr[10] << 8 ) |  (uint32)sipIpv6.ipv6_addr[11] ;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_SIP53_32tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        osal_memcpy(&dipIpv6,&pEntry->dip.ipv6, sizeof(rtk_ipv6_addr_t));
        if ((ret = table_field_byte_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_DIP_31_0tf, (uint8 *)&dipIpv6.ipv6_addr[12], (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = ((uint32)(dipIpv6.ipv6_addr[9] & 0x3F) << 16 ) | ((uint32)dipIpv6.ipv6_addr[10] << 8 ) |  (uint32)dipIpv6.ipv6_addr[11] ;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_DIP53_32tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        idx = pEntry->index;
        if ((ret = table_write(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, idx, (uint32 *)&flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        osal_memset(&flow_entry, 0x0, sizeof(rtl9602c_l34_flow_route_entry_t));

        /*Set IPv6 flow_EXT table*/
        tmp_val = 0;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_IPV4tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        tmp_val = (uint32)pEntry->valid;
        if ((ret = table_field_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_VALIDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        for(i = 9; i > 0; i--)
        {
            sipIpv6.ipv6_addr[i] = (uint8)(sipIpv6.ipv6_addr[i] >> 6) + (uint8)(sipIpv6.ipv6_addr[i-1] << 2);
        }
        sipIpv6.ipv6_addr[0] = (uint8)(sipIpv6.ipv6_addr[0] >> 6);

        if ((ret = table_field_byte_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_SIP127_54tf, (uint8 *)&sipIpv6, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        for(i = 9; i > 0; i--)
        {
            dipIpv6.ipv6_addr[i] = (uint8)(dipIpv6.ipv6_addr[i] >> 6) + (uint8)(dipIpv6.ipv6_addr[i-1] << 2);
        }
        dipIpv6.ipv6_addr[0] = (uint8)(dipIpv6.ipv6_addr[0] >> 6);

        if ((ret = table_field_byte_set(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_DIP127_54tf, (uint8 *)&dipIpv6, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        idx = pEntry->index+1;
        if ((ret = table_write(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, idx, (uint32 *)&flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_l34_flowRouteTable_get
 * Description:
 *      Get Flow Routing Table.
 * Input:
 *      None
 * Output:
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_flowRouteTable_get(rtk_l34_flowRoute_entry_t *pEntry)
{
    int32 ret;
    uint32 tmp_val;
    uint32 idx;
    rtl9602c_l34_flow_route_entry_t flow_entry;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&flow_entry, 0x0, sizeof(rtl9602c_l34_flow_route_entry_t));

    idx = pEntry->index;
    if ((ret = table_read(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, idx, (uint32 *)&flow_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_IPV4tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->isIpv4 = tmp_val;

    /*IPv4*/
    if(pEntry->isIpv4 == 1)
    {
        osal_memset(&flow_entry, 0x0, sizeof(rtl9602c_l34_flow_route_entry_t));

        if ((ret = table_read(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, idx, (uint32 *)&flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_VALIDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->valid = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_SPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->sport = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_DPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->dport = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_TCPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->isTcp = tmp_val;

        tmp_val = (uint32)pEntry->nexthopIdx;
        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_NEXTHOPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->nexthopIdx = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_PROCESStf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        _dal_rtl9602c_l34_flowRouteType_raw2cfg(tmp_val, &pEntry->process);

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_SIPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->sip.ipv4 = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_DIPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->dip.ipv4 = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_PRI_VLDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->pri_valid = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV4t, RTL9602C_FLOW_ROUTING_TABLE_IPV4_PRItf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->priority = tmp_val;
    }
    else /*IPv6*/
    {
        rtk_ipv6_addr_t     ipv6Addr;
        uint32 i;

        /*If IPv6 entry, index must be even number*/
        if(1 ==(pEntry->index%2))
            return RT_ERR_OK;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_VALIDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->valid = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_SPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->sport = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_DPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->dport = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_TCPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->isTcp = tmp_val;

        tmp_val = (uint32)pEntry->nexthopIdx;
        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_NEXTHOPtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->nexthopIdx = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_PROCESStf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        _dal_rtl9602c_l34_flowRouteType_raw2cfg(tmp_val, &pEntry->process);

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_PRI_VLDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->pri_valid = tmp_val;

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_PRItf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->priority = tmp_val;

        if ((ret = table_field_byte_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_SIP_31_0tf, (uint8 *)&ipv6Addr.ipv6_addr[12], (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        osal_memcpy(&pEntry->sip.ipv6.ipv6_addr[12], &ipv6Addr.ipv6_addr[12], 4);

        if ((ret = table_field_byte_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_SIP53_32tf, (uint8 *)&ipv6Addr.ipv6_addr[9], (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        osal_memcpy(&pEntry->sip.ipv6.ipv6_addr[9], &ipv6Addr.ipv6_addr[9], 3);

        if ((ret = table_field_byte_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_DIP_31_0tf, (uint8 *)&ipv6Addr.ipv6_addr[12], (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        osal_memcpy(&pEntry->dip.ipv6.ipv6_addr[12], &ipv6Addr.ipv6_addr[12], 4);

        if ((ret = table_field_byte_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6t, RTL9602C_FLOW_ROUTING_TABLE_IPV6_DIP53_32tf, (uint8 *)&ipv6Addr.ipv6_addr[9], (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        osal_memcpy(&pEntry->dip.ipv6.ipv6_addr[9], &ipv6Addr.ipv6_addr[9], 3);


        idx = pEntry->index+1;
        if ((ret = table_read(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, idx, (uint32 *)&flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_IPV4tf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        if ((ret = table_field_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_VALIDtf, &tmp_val, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->valid = pEntry->valid & tmp_val;

        if ((ret = table_field_byte_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_SIP127_54tf, (uint8 *)&ipv6Addr, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }

        pEntry->sip.ipv6.ipv6_addr[9] = ( pEntry->sip.ipv6.ipv6_addr[9] & 0x3F ) + ((uint8)(ipv6Addr.ipv6_addr[9] << 6) & 0xC0 );
        for(i = 0; i < 9; i++)
        {
            pEntry->sip.ipv6.ipv6_addr[i] = (uint8)((ipv6Addr.ipv6_addr[i] << 6) & 0xC0) +  (uint8)(ipv6Addr.ipv6_addr[i+1] >> 2);
        }

        if ((ret = table_field_byte_get(RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt, RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXT_DIP127_54tf, (uint8 *)&ipv6Addr, (uint32 *) &flow_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return RT_ERR_FAILED;
        }
        pEntry->dip.ipv6.ipv6_addr[9] = ( pEntry->dip.ipv6.ipv6_addr[9] & 0x3F ) + ((uint8)(ipv6Addr.ipv6_addr[9] << 6) & 0xC0 );
        for(i = 0; i < 9; i++)
        {
            pEntry->dip.ipv6.ipv6_addr[i] = (uint8)((ipv6Addr.ipv6_addr[i] << 6) & 0xC0) +  (uint8)(ipv6Addr.ipv6_addr[i+1] >> 2);
        }

    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9602c_l34_flowTrfIndicator_get
 * Description:
 *      get flow routing traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 *    *pFlowIndicator   - point of traffic indicator for flow routing traffic
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_flowTrfIndicator_get(uint32 index, rtk_enable_t *pFlowIndicator)
{
    int32 ret;
    int tableId, indexId;
    uint32 tmp_val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9602C_L34_FLOW_ROUTE_TABLE_MAX <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlowIndicator), RT_ERR_NULL_POINTER);

    tableId = index / 32;
    indexId = index % 32;
    if ((ret = reg_read((RTL9602C_FT_TRF0r + tableId), &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *pFlowIndicator = (tmp_val & (1<< indexId)) ? ENABLED : DISABLED;


    return RT_ERR_OK;
}   /* end of dal_rtl9602c_l34_flowTrfIndicator_get */


/* Function Name:
 *      dal_rtl9602c_l34_dsliteInfTable_set
 * Description:
 *      Set Ds-Lite interface table entry by idx.
 * Input:
 *      idx    : entry index
 *      entry  : entry content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_dsliteInfTable_set(rtk_l34_dsliteInf_entry_t *pEntry)
{
    int32 ret;
    uint32 pValue[10];
    uint32 tmp_field_val;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_DSLITE_INF_TABLE_MAX <= pEntry->index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((256 <= pEntry->hopLimit), RT_ERR_INPUT);
    RT_PARAM_CHK((0xFFFFF < pEntry->flowLabel), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_L34_DSLITE_TC_OPT_END <= pEntry->tcOpt), RT_ERR_INPUT);
    RT_PARAM_CHK((256 <= pEntry->tc), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->valid), RT_ERR_INPUT);


    if ((ret = reg_array_read(RTL9602C_DSLITE_INF_TBLr, REG_ARRAY_INDEX_NONE, pEntry->index, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = pEntry->hopLimit;
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_HOPLIMITf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = pEntry->flowLabel;
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_FLOWLABELf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = pEntry->tcOpt;
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_TC_OPTIONf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = pEntry->tc;
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_TCf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = pEntry->valid;
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_VALIDf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipAftr.ipv6_addr[0] << 24 ) | ( pEntry->ipAftr.ipv6_addr[1] << 16 ) | ( pEntry->ipAftr.ipv6_addr[2] << 8 ) | ( pEntry->ipAftr.ipv6_addr[3]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_3f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipAftr.ipv6_addr[4] << 24 ) | ( pEntry->ipAftr.ipv6_addr[5] << 16 ) | ( pEntry->ipAftr.ipv6_addr[6] << 8 ) | ( pEntry->ipAftr.ipv6_addr[7]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_2f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipAftr.ipv6_addr[8] << 24 ) | ( pEntry->ipAftr.ipv6_addr[9] << 16 ) | ( pEntry->ipAftr.ipv6_addr[10] << 8 ) | ( pEntry->ipAftr.ipv6_addr[11]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipAftr.ipv6_addr[12] << 24 ) | ( pEntry->ipAftr.ipv6_addr[13] << 16 ) | ( pEntry->ipAftr.ipv6_addr[14] << 8 ) | ( pEntry->ipAftr.ipv6_addr[15]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipB4.ipv6_addr[0] << 24 ) | ( pEntry->ipB4.ipv6_addr[1] << 16 ) | ( pEntry->ipB4.ipv6_addr[2] << 8 ) | ( pEntry->ipB4.ipv6_addr[3]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_3f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipB4.ipv6_addr[4] << 24 ) | ( pEntry->ipB4.ipv6_addr[5] << 16 ) | ( pEntry->ipB4.ipv6_addr[6] << 8 ) | ( pEntry->ipB4.ipv6_addr[7]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_2f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

        tmp_field_val = ( pEntry->ipB4.ipv6_addr[8] << 24 ) | ( pEntry->ipB4.ipv6_addr[9] << 16 ) | ( pEntry->ipB4.ipv6_addr[10] << 8 ) | ( pEntry->ipB4.ipv6_addr[11]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipB4.ipv6_addr[12] << 24 ) | ( pEntry->ipB4.ipv6_addr[13] << 16 ) | ( pEntry->ipB4.ipv6_addr[14] << 8 ) | ( pEntry->ipB4.ipv6_addr[15]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }


    if ((ret = reg_array_write(RTL9602C_DSLITE_INF_TBLr, REG_ARRAY_INDEX_NONE, pEntry->index, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;


}

/* Function Name:
 *      dal_rtl9602c_l34_dsliteInfTable_get
 * Description:
 *      Get Ds-Lite interface table entry by idx.
 * Input:
 *      idx    : entry index
 * Output:
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_dsliteInfTable_get(rtk_l34_dsliteInf_entry_t *pEntry)
{
    int32 ret;
    uint32 pValue[10];
    uint32 tmp_field_val;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_DSLITE_INF_TABLE_MAX <= pEntry->index), RT_ERR_ENTRY_INDEX);


    if ((ret = reg_array_read(RTL9602C_DSLITE_INF_TBLr, REG_ARRAY_INDEX_NONE, pEntry->index, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_HOPLIMITf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->hopLimit = tmp_field_val;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_FLOWLABELf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->flowLabel = tmp_field_val;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_TC_OPTIONf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->tcOpt = tmp_field_val;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_TCf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->tc = tmp_field_val;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_VALIDf, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->valid = tmp_field_val;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_3f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipAftr.ipv6_addr[0] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipAftr.ipv6_addr[1] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipAftr.ipv6_addr[2] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipAftr.ipv6_addr[3] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_2f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipAftr.ipv6_addr[4] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipAftr.ipv6_addr[5] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipAftr.ipv6_addr[6] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipAftr.ipv6_addr[7] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipAftr.ipv6_addr[8] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipAftr.ipv6_addr[9] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipAftr.ipv6_addr[10] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipAftr.ipv6_addr[11] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_AFTR_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipAftr.ipv6_addr[12] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipAftr.ipv6_addr[13] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipAftr.ipv6_addr[14] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipAftr.ipv6_addr[15] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_3f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipB4.ipv6_addr[0] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipB4.ipv6_addr[1] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipB4.ipv6_addr[2] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipB4.ipv6_addr[3] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_2f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipB4.ipv6_addr[4] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipB4.ipv6_addr[5] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipB4.ipv6_addr[6] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipB4.ipv6_addr[7] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipB4.ipv6_addr[8] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipB4.ipv6_addr[9] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipB4.ipv6_addr[10] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipB4.ipv6_addr[11] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_INF_TBLr, RTL9602C_IP_B4_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipB4.ipv6_addr[12] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipB4.ipv6_addr[13] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipB4.ipv6_addr[14] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipB4.ipv6_addr[15] = tmp_field_val & 0xFF;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9602c_l34_dsliteMcTable_set
 * Description:
 *      Set Ds-Lite multicast table entry by idx.
 * Input:
 *      idx    : entry index
 *      entry  : entry content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_dsliteMcTable_set(rtk_l34_dsliteMc_entry_t *pEntry)
{
    int32 ret;
    uint32 pValue[10];
    uint32 tmp_field_val;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_DSLITE_MCAST_TABLE_MAX <= pEntry->index), RT_ERR_ENTRY_INDEX);


    if ((ret = reg_array_read(RTL9602C_DSLITE_MC_TBLr, REG_ARRAY_INDEX_NONE, pEntry->index, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipUPrefix64Mask.ipv6_addr[0] << 24 ) | ( pEntry->ipUPrefix64Mask.ipv6_addr[1] << 16 ) | ( pEntry->ipUPrefix64Mask.ipv6_addr[2] << 8 ) | ( pEntry->ipUPrefix64Mask.ipv6_addr[3]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_MASK_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipUPrefix64Mask.ipv6_addr[4] << 24 ) | ( pEntry->ipUPrefix64Mask.ipv6_addr[5] << 16 ) | ( pEntry->ipUPrefix64Mask.ipv6_addr[6] << 8 ) | ( pEntry->ipUPrefix64Mask.ipv6_addr[7]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_MASK_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipUPrefix64.ipv6_addr[0] << 24 ) | ( pEntry->ipUPrefix64.ipv6_addr[1] << 16 ) | ( pEntry->ipUPrefix64.ipv6_addr[2] << 8 ) | ( pEntry->ipUPrefix64.ipv6_addr[3]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipUPrefix64.ipv6_addr[4] << 24 ) | ( pEntry->ipUPrefix64.ipv6_addr[5] << 16 ) | ( pEntry->ipUPrefix64.ipv6_addr[6] << 8 ) | ( pEntry->ipUPrefix64.ipv6_addr[7]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipMPrefix64Mask.ipv6_addr[0] << 24 ) | ( pEntry->ipMPrefix64Mask.ipv6_addr[1] << 16 ) | ( pEntry->ipMPrefix64Mask.ipv6_addr[2] << 8 ) | ( pEntry->ipMPrefix64Mask.ipv6_addr[3]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_MASK_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipMPrefix64Mask.ipv6_addr[4] << 24 ) | ( pEntry->ipMPrefix64Mask.ipv6_addr[5] << 16 ) | ( pEntry->ipMPrefix64Mask.ipv6_addr[6] << 8 ) | ( pEntry->ipMPrefix64Mask.ipv6_addr[7]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_MASK_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipMPrefix64.ipv6_addr[0] << 24 ) | ( pEntry->ipMPrefix64.ipv6_addr[1] << 16 ) | ( pEntry->ipMPrefix64.ipv6_addr[2] << 8 ) | ( pEntry->ipMPrefix64.ipv6_addr[3]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    tmp_field_val = ( pEntry->ipMPrefix64.ipv6_addr[4] << 24 ) | ( pEntry->ipMPrefix64.ipv6_addr[5] << 16 ) | ( pEntry->ipMPrefix64.ipv6_addr[6] << 8 ) | ( pEntry->ipMPrefix64.ipv6_addr[7]);
    if ((ret = reg_field_set(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = reg_array_write(RTL9602C_DSLITE_MC_TBLr, REG_ARRAY_INDEX_NONE, pEntry->index, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;


}

/* Function Name:
 *      dal_rtl9602c_l34_dsliteInfTable_get
 * Description:
 *      Get Ds-Lite interface entry by idx.
 * Input:
 *      idx    : entry index
 * Output:
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_dsliteMcTable_get(rtk_l34_dsliteMc_entry_t *pEntry)
{
    int32 ret;
    uint32 pValue[10];
    uint32 tmp_field_val;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTL9602C_L34_DSLITE_MCAST_TABLE_MAX <= pEntry->index), RT_ERR_ENTRY_INDEX);


    if ((ret = reg_array_read(RTL9602C_DSLITE_MC_TBLr, REG_ARRAY_INDEX_NONE, pEntry->index, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_MASK_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipUPrefix64Mask.ipv6_addr[0] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipUPrefix64Mask.ipv6_addr[1] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipUPrefix64Mask.ipv6_addr[2] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipUPrefix64Mask.ipv6_addr[3] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_MASK_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipUPrefix64Mask.ipv6_addr[4] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipUPrefix64Mask.ipv6_addr[5] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipUPrefix64Mask.ipv6_addr[6] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipUPrefix64Mask.ipv6_addr[7] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipUPrefix64.ipv6_addr[0] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipUPrefix64.ipv6_addr[1] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipUPrefix64.ipv6_addr[2] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipUPrefix64.ipv6_addr[3] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_UPREFIX64_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipUPrefix64.ipv6_addr[4] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipUPrefix64.ipv6_addr[5] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipUPrefix64.ipv6_addr[6] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipUPrefix64.ipv6_addr[7] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_MASK_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipMPrefix64Mask.ipv6_addr[0] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipMPrefix64Mask.ipv6_addr[1] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipMPrefix64Mask.ipv6_addr[2] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipMPrefix64Mask.ipv6_addr[3] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_MASK_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipMPrefix64Mask.ipv6_addr[4] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipMPrefix64Mask.ipv6_addr[5] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipMPrefix64Mask.ipv6_addr[6] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipMPrefix64Mask.ipv6_addr[7] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_1f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipMPrefix64.ipv6_addr[0] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipMPrefix64.ipv6_addr[1] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipMPrefix64.ipv6_addr[2] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipMPrefix64.ipv6_addr[3] = tmp_field_val & 0xFF;

    if ((ret = reg_field_get(RTL9602C_DSLITE_MC_TBLr, RTL9602C_IP_MPREFIX64_0f, &tmp_field_val, pValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_FAILED;
    }
    pEntry->ipMPrefix64.ipv6_addr[4] = (tmp_field_val >> 24) & 0xFF;
    pEntry->ipMPrefix64.ipv6_addr[5] = (tmp_field_val >> 16) & 0xFF;
    pEntry->ipMPrefix64.ipv6_addr[6] = (tmp_field_val >> 8) & 0xFF;
    pEntry->ipMPrefix64.ipv6_addr[7] = tmp_field_val & 0xFF;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_l34_dsliteControl_set
 * Description:
 *      Set DS-Lite Control
 * Input:
 *      ctrlType - control type
 *      act - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_dsliteControl_set(rtk_l34_dsliteCtrlType_t ctrlType, uint32 act)
{
    uint32 tmp_val;
    int32  ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "stateType=%d, action=%d", ctrlType, act);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_DSLITE_CTRL_END <= ctrlType), RT_ERR_INPUT);

    switch(ctrlType){
    case L34_DSLITE_CTRL_DSLITE_STATE:
        RT_PARAM_CHK((RTK_ENABLE_END <= act), RT_ERR_INPUT);

        tmp_val = act;
        if ((ret = reg_field_write(RTL9602C_DSLITE_CTRLr, RTL9602C_ENf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        break;
    case L34_DSLITE_CTRL_MC_PREFIX_UNMATCH:
        switch(act)
        {
            case RTK_L34_DSLITE_UNMATCH_ACT_DROP:
                tmp_val = 0;
            break;
            case RTK_L34_DSLITE_UNMATCH_ACT_TRAP:
                tmp_val = 1;
            break;
            default:
                return RT_ERR_INPUT;
            break;
        }
        if ((ret = reg_field_write(RTL9602C_DSLITE_CTRLr, RTL9602C_MC_PREFIX_UNMATCHf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        break;
    case L34_DSLITE_CTRL_DS_UNMATCH_ACT:
        switch(act)
        {
            case RTK_L34_DSLITE_UNMATCH_ACT_DROP:
                tmp_val = 0;
                break;
            case RTK_L34_DSLITE_UNMATCH_ACT_TRAP:
                tmp_val = 1;
                break;
            default:
                return RT_ERR_INPUT;
                break;
        }
        if ((ret = reg_field_write(RTL9602C_DSLITE_CTRLr, RTL9602C_DSLITE_UNMATCH_ACTf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        break;
    case L34_DSLITE_CTRL_IP6_NH_ACTION:
        switch(act)
        {
            case RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_0:
                tmp_val = 0;
                break;
            case RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_240:
                tmp_val = 1;
                break;
            default:
                return RT_ERR_INPUT;
                break;
        }
        if ((ret = reg_field_write(RTL9602C_DSLITE_CTRLr, RTL9602C_IP6_NH_ACTIONf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        break;
    default:
        return RT_ERR_INPUT;
        break;

    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_l34_dsliteControl_get
 * Description:
 *      Get DS-Lite Control
 * Input:
 *      ctrlType - control type
 * Output:
 *      pAct - action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_dsliteControl_get(rtk_l34_dsliteCtrlType_t ctrlType, uint32 *pAct)
{
    uint32 tmp_val;
    int32  ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "stateType=%d", ctrlType);
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_DSLITE_CTRL_END <= ctrlType), RT_ERR_INPUT);

    switch(ctrlType){
    case L34_DSLITE_CTRL_DSLITE_STATE:
        if ((ret = reg_field_read(RTL9602C_DSLITE_CTRLr, RTL9602C_ENf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        *pAct = (rtk_enable_t) tmp_val;
        break;
    case L34_DSLITE_CTRL_MC_PREFIX_UNMATCH:
        if ((ret = reg_field_read(RTL9602C_DSLITE_CTRLr, RTL9602C_MC_PREFIX_UNMATCHf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        switch(tmp_val)
        {
            case 0:
                *pAct = RTK_L34_DSLITE_UNMATCH_ACT_DROP;
                break;
            case 1:
                *pAct = RTK_L34_DSLITE_UNMATCH_ACT_TRAP;
                break;
            default:
                return RT_ERR_INPUT;
                break;
        }
        break;
    case L34_DSLITE_CTRL_DS_UNMATCH_ACT:
        if ((ret = reg_field_read(RTL9602C_DSLITE_CTRLr, RTL9602C_DSLITE_UNMATCH_ACTf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        switch(tmp_val)
        {
            case 0:
                *pAct = RTK_L34_DSLITE_UNMATCH_ACT_DROP;
                break;
            case 1:
                *pAct = RTK_L34_DSLITE_UNMATCH_ACT_TRAP;
                break;
            default:
                return RT_ERR_INPUT;
                break;
        }
        break;
    case L34_DSLITE_CTRL_IP6_NH_ACTION:
        if ((ret = reg_field_read(RTL9602C_DSLITE_CTRLr, RTL9602C_IP6_NH_ACTIONf, &tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        switch(tmp_val)
        {
            case 0:
                *pAct = RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_0;
                break;
            case 1:
                *pAct = RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_240;
                break;
            default:
                return RT_ERR_INPUT;
                break;
        }
        break;
    default:
        return RT_ERR_INPUT;
        break;

    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_l34_mib_get
 * Description:
 *      Get per L34 interface counters
 * Input:
 *      ifIndex    - L34 interface index
 * Output:
 *      counters   - counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_mib_get(rtk_l34_mib_t *pL34Cnt)
{
    int32    ret;
    uint32   tmpVal;
    uint32   idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL34Cnt), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= pL34Cnt->ifIndex), RT_ERR_ENTRY_INDEX);

    idx = pL34Cnt->ifIndex;

    if ((ret = reg_array_field_read(RTL9602C_STAT_L34_MIBr, REG_ARRAY_INDEX_NONE, idx, RTL9602C_OUTL34IFOCTETS_Hf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    pL34Cnt->ifOutOctets = tmpVal;
    pL34Cnt->ifOutOctets = ( pL34Cnt->ifOutOctets << 32 );

    if ((ret = reg_array_field_read(RTL9602C_STAT_L34_MIBr, REG_ARRAY_INDEX_NONE, idx,  RTL9602C_OUTL34IFOCTETS_Lf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    pL34Cnt->ifOutOctets = pL34Cnt->ifOutOctets + tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_STAT_L34_MIBr, REG_ARRAY_INDEX_NONE, idx,  RTL9602C_OUTL34IFPKTSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    pL34Cnt->ifOutUcstPkts = tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_STAT_L34_MIBr, REG_ARRAY_INDEX_NONE, idx,  RTL9602C_INL34IFOCTETS_Hf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    pL34Cnt->ifInOctets = tmpVal;
    pL34Cnt->ifInOctets = (pL34Cnt->ifInOctets << 32);

    if ((ret = reg_array_field_read(RTL9602C_STAT_L34_MIBr, REG_ARRAY_INDEX_NONE, idx,  RTL9602C_INL34IFOCTETS_Lf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    pL34Cnt->ifInOctets = pL34Cnt->ifInOctets + tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_STAT_L34_MIBr, REG_ARRAY_INDEX_NONE, idx,  RTL9602C_INL34IFPKTSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    pL34Cnt->ifInUcstPkts = tmpVal;


    return RT_ERR_OK;
} /* end of dal_rtl9602c_stat_l34_mib_get */


/* Function Name:
 *      dal_rtl9602c_l34_mib_reset
 * Description:
 *      Reset per L34 interface counters
 * Input:
 *      ifIndex    - L34 interface index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_mib_reset(uint32 ifIndex)
{
    int32   ret, retry;
    uint32  tmpVal;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= ifIndex), RT_ERR_ENTRY_INDEX);

    /*Reset L34 interface counters*/

    /*Check if ASIC is still reseting MIB or not*/
    if ((ret = reg_field_read(RTL9602C_STAT_RSTr, RTL9602C_RST_STATf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    if (tmpVal != 0)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    tmpVal = 1;
    if ((ret = reg_array_field_write(RTL9602C_STAT_L34_RSTr, REG_ARRAY_INDEX_NONE, ifIndex, RTL9602C_ENf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }

    tmpVal = 1;
    if ((ret = reg_field_write(RTL9602C_STAT_RST_CFGr, RTL9602C_RST_CMDf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }

    /*Waiting until reset finish*/
    retry = 0;
    do
    {
       if ((ret = reg_field_read(RTL9602C_STAT_RSTr, RTL9602C_RST_STATf, &tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L34), "");
            return ret;
        }
        retry++;
    }while((1 == tmpVal) && (retry < 1000));

    if(retry >= 1000)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_l34_lutLookupMiss_set
 * Description:
 *      set L34 MAC table lookup miss action
 * Input:
 *    lutMissAct    - L34 MAC table lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_lutLookupMiss_set(rtk_l34_lutMissAct_t lutMissAct)
{
    int32   ret;
    uint32  tmpVal;

    RT_PARAM_CHK((lutMissAct >= L34_LUTMISS_ACT_END), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    tmpVal = (lutMissAct == L34_LUTMISS_ACT_DROP) ? 0 : 1;
    if ((ret = reg_field_write(RTL9602C_L34_GLB_CFGr, RTL9602C_L34_L2_LOOKUP_MISS_ACTf, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_l34_lutLookupMiss_get
 * Description:
 *      Get L34 MAC table lookup miss action
 * Input:
 *      None
 * Output:
 *      lutMissAct  - L34 MAC table lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_l34_lutLookupMiss_get(rtk_l34_lutMissAct_t *pLutMissAct)
{
    int32   ret;
    uint32  tmpVal;

    RT_PARAM_CHK((pLutMissAct==NULL), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    if ((ret = reg_field_read(RTL9602C_L34_GLB_CFGr, RTL9602C_L34_L2_LOOKUP_MISS_ACTf, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *pLutMissAct = (tmpVal == 0) ? L34_LUTMISS_ACT_DROP : L34_LUTMISS_ACT_TRAP;

    return RT_ERR_OK;
}
