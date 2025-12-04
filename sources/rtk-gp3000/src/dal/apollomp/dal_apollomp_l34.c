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
#include <dal/apollomp/dal_apollomp.h>

#include <rtk/l34.h>
#include <dal/apollomp/dal_apollomp_l34.h>
#include <dal/apollomp/raw/apollomp_raw.h>
#include <dal/apollomp/raw/apollomp_raw_l34.h>
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
int32 dal_apollomp_l34_hsb_set(apollomp_l34_hsb_param_t *hsb);
int32 dal_apollomp_l34_hsb_get(apollomp_l34_hsb_param_t *hsb);
int32 dal_apollomp_l34_hsa_set(apollomp_l34_hsa_param_t *hsa);
int32 dal_apollomp_l34_hsa_get(apollomp_l34_hsa_param_t *hsa);
int32 dal_apollomp_l34_hsabCtrMode_set(apollomp_l34_hsab_mode_t mode);

/*
    Number of entries allocate for Nextp hop routing entry.
    asic_val  entry number
    --------  ------------
    0          1
    1          2
    2          4
    3          8
    4         16
 */

static unsigned int _dal_apollomp_l34_log2(unsigned int n)
{
    return n < 2 ? 0 : 1 + _dal_apollomp_l34_log2(n/2);
}


static uint8
_dal_apollomp_l34_routingTable_nhNum_to_asicVal(uint8 nh_num)
{
    return _dal_apollomp_l34_log2(nh_num);
}

static int8
_dal_apollomp_l34_routingTable_asicVal_to_nhNum(uint8 asic_val)
{
    uint8 ret=1;
    int i;
    for(i=1;i<asic_val;i++)
    {
        ret=ret*2;
    }


    return ret;
}


static int32
_dal_apollomp_l34_binding_parm_check(rtk_l34_bindType_t type,rtk_l34_bindAct_t action,uint32 *rawType,uint32 *rawAct)
{
    switch(type){
    case L34_BIND_UNMATCHED_L2L3:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L2L3;
        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L2L3_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L2L3_TRAP;
        }else
        if(action==L34_BIND_ACT_FORCE_L2BRIDGE)
        {
            *rawAct = APOLLOMP_UNMATCHED_L2L3_FORCE_L2Bridge;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_UNMATCHED_L2L34:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L2L34;
        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L2L34_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L2L34_TRAP;
        }else
        if(action==L34_BIND_ACT_FORCE_L2BRIDGE)
        {
            *rawAct = APOLLOMP_UNMATCHED_L2L34_FORCE_L2BRIDGE;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_UNMATCHED_L3L2:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L3L2;
        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L2_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L2_TRAP;
        }else
        if(action==L34_BIND_ACT_PERMIT_L2BRIDGE)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L2_PERMIT_L2BRIDGE;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_UNMATCHED_L3L34:
    {

        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L3L34;
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L34_TRAP;
        }else
        if(action==L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L34_FORCE_L4;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_UNMATCHED_L34L2:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L34L2;
        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L34L2_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L34L2_TRAP;
        }else
        if(action==L34_BIND_ACT_PERMIT_L2BRIDGE)
        {
            *rawAct = APOLLOMP_UNMATCHED_L34L2_PERMIT_L2BRIDGE;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_UNMATCHED_L34L3:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L34L3;
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L34L3_TRAP;
        }else
        if(action==L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4)
        {
            *rawAct = APOLLOMP_UNMATCHED_L34L3_FORCE_L3;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_UNMATCHED_L3L3:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L3L3;
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L3_TRAP;
        }else
        if(action==L34_BIND_ACT_FORCE_BINDL3)
        {
            *rawAct = APOLLOMP_UNMATCHED_L3L3_FORCE_L3;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_CUSTOMIZED_L2:
    {
        *rawType = APOLLOMP_L34_BIND_CUSTOMIZED_L2;

        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL2_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL2_TRAP;
        }else
        if(action==L34_BIND_ACT_PERMIT_L2BRIDGE)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL2_PERMIT_L2_BRIDGE;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_CUSTOMIZED_L3:
    {
        *rawType = APOLLOMP_L34_BIND_CUSTOMIZED_L3;
        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL3_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL3_TRAP;
        }else
        if(action==L34_BIND_ACT_FORCE_BINDL3)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL3_FORCE_L3;
        }else
        if(action==L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL3_FORCE_L4;
        }else
        {
            return RT_ERR_INPUT;
        }
    }
    break;
        case L34_BIND_CUSTOMIZED_L34:
    {
        *rawType = APOLLOMP_L34_BIND_CUSTOMIZED_L34;

        if(action==L34_BIND_ACT_DROP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL34_DROP;
        }else
        if(action==L34_BIND_ACT_TRAP)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL34_TRAP;
        }else
        if(action==L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL34_FORCE_L3;
        }else
        if(action==L34_BIND_ACT_NORMAL_LOOKUPL34)
        {
            *rawAct = APOLLOMP_UNMATCHED_CUSL34_NORMAL_L34;
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
_dal_apollomp_l34_bindingAct_raw2cfg(rtk_l34_bindType_t type,uint32 raw,rtk_l34_bindAct_t *pAct)
{
    switch(type){
    case L34_BIND_UNMATCHED_L2L3:
    {
        if(raw== APOLLOMP_UNMATCHED_L2L3_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L2L3_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L2L3_FORCE_L2Bridge)
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
        if(raw ==APOLLOMP_UNMATCHED_L2L34_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L2L34_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L2L34_FORCE_L2BRIDGE)
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
        if(raw==APOLLOMP_UNMATCHED_L3L2_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L3L2_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L3L2_PERMIT_L2BRIDGE)
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
        if(raw==APOLLOMP_UNMATCHED_L3L34_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L3L34_FORCE_L4)
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
        if(raw==APOLLOMP_UNMATCHED_L34L2_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L34L2_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L34L2_PERMIT_L2BRIDGE)
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
        if(raw==APOLLOMP_UNMATCHED_L34L3_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L34L3_FORCE_L3)
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
        if(raw==APOLLOMP_UNMATCHED_L3L3_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_L3L3_FORCE_L3)
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
        if(raw==APOLLOMP_UNMATCHED_CUSL2_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL2_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL2_PERMIT_L2_BRIDGE)
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
        if(raw==APOLLOMP_UNMATCHED_CUSL3_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL3_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL3_FORCE_L3)
        {
            *pAct=L34_BIND_ACT_FORCE_BINDL3;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL3_FORCE_L4)
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
        if(raw==APOLLOMP_UNMATCHED_CUSL34_DROP)
        {
            *pAct=L34_BIND_ACT_DROP;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL34_TRAP)
        {
            *pAct=L34_BIND_ACT_TRAP;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL34_FORCE_L3)
        {
            *pAct=L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
        }else
        if(raw==APOLLOMP_UNMATCHED_CUSL34_NORMAL_L34)
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
_dal_apollomp_l34_hsabMode_cfg2raw(rtk_l34_hsba_mode_t mode,apollomp_raw_l34_hsba_mode_t *pMode)
{
    switch(mode){
    case L34_HSBA_BOTH_LOG:
        *pMode = APOLLOMP_RAW_L34_HSBA_BOTH_LOG;
    break;
    case L34_HSBA_NO_LOG:
        *pMode = APOLLOMP_RAW_L34_HSBA_NO_LOG;
    break;
    case L34_HSBA_LOG_ALL:
        *pMode = APOLLOMP_RAW_L34_HSBA_LOG_ALL;
    break;
    case L34_HSBA_LOG_FIRST_DROP:
        *pMode = APOLLOMP_RAW_L34_HSBA_LOG_FIRST_DROP;
    break;
    case L34_HSBA_LOG_FIRST_PASS:
        *pMode = APOLLOMP_RAW_L34_HSBA_LOG_FIRST_PASS;
    break;
    case L34_HSBA_LOG_FIRST_TO_CPU:
        *pMode = APOLLOMP_RAW_L34_HSBA_LOG_FIRST_TO_CPU;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}

static int32
_dal_apollomp_l34_hsabMode_raw2cfg(apollomp_raw_l34_hsba_mode_t mode,rtk_l34_hsba_mode_t *pMode)
{
    switch(mode){
    case APOLLOMP_RAW_L34_HSBA_BOTH_LOG:
        *pMode = L34_HSBA_BOTH_LOG;
    break;
    case APOLLOMP_RAW_L34_HSBA_NO_LOG:
        *pMode = L34_HSBA_NO_LOG;
    break;
    case APOLLOMP_RAW_L34_HSBA_LOG_ALL:
        *pMode = L34_HSBA_LOG_ALL;
    break;
    case APOLLOMP_RAW_L34_HSBA_LOG_FIRST_DROP:
        *pMode = L34_HSBA_LOG_FIRST_DROP;
    break;
    case APOLLOMP_RAW_L34_HSBA_LOG_FIRST_PASS:
        *pMode = L34_HSBA_LOG_FIRST_PASS;
    break;
    case APOLLOMP_RAW_L34_HSBA_LOG_FIRST_TO_CPU:
        *pMode = L34_HSBA_LOG_FIRST_TO_CPU;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}



static int32
_dal_apollomp_l34_binding_type_check(rtk_l34_bindType_t type,uint32 *rawType)
{
    switch(type){
    case L34_BIND_UNMATCHED_L2L3:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L2L3;
    }
    break;
        case L34_BIND_UNMATCHED_L2L34:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L2L34;
    }
    break;
        case L34_BIND_UNMATCHED_L3L2:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L3L2;
    }
    break;
        case L34_BIND_UNMATCHED_L3L34:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L3L34;
    }
    break;
        case L34_BIND_UNMATCHED_L34L2:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L34L2;
    }
    break;
        case L34_BIND_UNMATCHED_L34L3:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L34L3;
    }
    break;
        case L34_BIND_UNMATCHED_L3L3:
    {
        *rawType = APOLLOMP_L34_BIND_UNMATCHED_L3L3;
    }
    break;
        case L34_BIND_CUSTOMIZED_L2:
    {
        *rawType = APOLLOMP_L34_BIND_CUSTOMIZED_L2;
    }
    break;
        case L34_BIND_CUSTOMIZED_L3:
    {
        *rawType = APOLLOMP_L34_BIND_CUSTOMIZED_L3;
    }
    break;
        case L34_BIND_CUSTOMIZED_L34:
    {
        *rawType = APOLLOMP_L34_BIND_CUSTOMIZED_L34;
    }
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}



static int32
l34_portMapNetif_get(rtk_l34_portType_t type,uint32 *map,uint32 index)
{

    uint32 size;
    uint8 value;
    int32 ret;
    int32 (*fun)(rtk_port_t,uint8*);

    switch(type){
    case L34_PORT_MAC:
        fun = apollomp_raw_l34_portIntfIdx_get;
        size = L34_MAC_PORT_MAX;
    break;
    case L34_PORT_EXTENSION:
        fun = apollomp_raw_l34_extPortIntfIdx_get;
        size = L34_EXT_PORT_MAX;
    break;
    case L34_PORT_DSLVC:
        fun = apollomp_raw_l34_vcPortIntfIdx_get;
        size = L34_DSLVC_PORT_MAX;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }

    if((ret =fun(index,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *map = value;
    return RT_ERR_OK;
}


static int32
l34_mode_l3_set(rtk_enable_t state)
{
    int32 ret;
    apollomp_raw_l34_mode_t l34mode;

    if((ret=apollomp_raw_l34_mode_get(&l34mode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    l34mode = state | (l34mode & 2);

    if((ret=apollomp_raw_l34_mode_set(l34mode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}

static int32
l34_mode_l4_set(rtk_enable_t state)
{
    int32 ret;
    apollomp_raw_l34_mode_t l34mode;

    if((ret=apollomp_raw_l34_mode_get(&l34mode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    l34mode = (state << 1) | (l34mode & 1);

    if((ret=apollomp_raw_l34_mode_set(l34mode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}


static int32
l34_lookupMode_cfg2raw(rtk_l34_lookupMode_t type,uint32 *raw)
{

    switch(type){
    case L34_LOOKUP_VLAN_BASE:
        *raw = APOLLOMP_RAW_L34_LIMBC_VLAN_BASED;
    break;
    case L34_LOOKUP_MAC_BASE:
        *raw = APOLLOMP_RAW_L34_LIMBC_MAC_BASED;
    break;
    case L34_LOOKUP_PORT_BASE:
        *raw = APOLLOMP_RAW_L34_LIMBC_PORT_BASED;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
l34_lookupMode_raw2cfg(uint32 raw, rtk_l34_lookupMode_t *mode)
{

    switch(raw){
    case APOLLOMP_RAW_L34_LIMBC_VLAN_BASED:
        *mode = L34_LOOKUP_VLAN_BASE;
    break;
    case APOLLOMP_RAW_L34_LIMBC_MAC_BASED:
        *mode = L34_LOOKUP_MAC_BASE;
    break;
    case APOLLOMP_RAW_L34_LIMBC_PORT_BASED:
        *mode = L34_LOOKUP_PORT_BASE;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
l34_wanroute_cfg2raw(rtk_l34_wanRouteMode_t type,uint32 *raw)
{
    switch(type){
    case L34_WANROUTE_FORWARD:
        *raw = L34_WAN_ROUTE_FWD;
    break;
    case L34_WANROUTE_FORWARD2CPU:
        *raw = L34_WAN_ROUTE_FWD_TO_CPU;
    break;
    case L34_WANROUTE_DROP:
        *raw = L34_WAN_ROUTE_DROP;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
l34_wanroute_raw2cfg(uint32 raw,rtk_l34_wanRouteMode_t *type)
{

    switch(raw){
    case L34_WAN_ROUTE_FWD:
        *type = L34_WANROUTE_FORWARD;
    break;
    case L34_WAN_ROUTE_FWD_TO_CPU:
        *type = L34_WANROUTE_FORWARD2CPU;
    break;
    case L34_WAN_ROUTE_DROP:
        *type = L34_WANROUTE_DROP;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }

    return RT_ERR_OK;
}

static  void
l34_l4Trf_cfg2raw(rtk_l34_l4_trf_t cfg,apollomp_raw_l34_l4_trf_t *table)
{
    switch(cfg){
    case RTK_L34_L4TRF_TABLE0:
        *table = APOLLOMP_RAW_L34_L4TRF_TABLE0;
    break;
    case RTK_L34_L4TRF_TABLE1:
        *table =APOLLOMP_RAW_L34_L4TRF_TABLE1;
    break;
    default:
    break;
    }
}

static  void
l34_arpTrf_cfg2raw(rtk_l34_arp_trf_t cfg,apollomp_raw_l34_arp_trf_t *table)
{
    switch(cfg){
    case RTK_L34_ARPTRF_TABLE0:
        *table = APOLLOMP_RAW_L34_ARPTRF_TABLE0;
    break;
    case RTK_L34_ARPTRF_TABLE1:
        *table =APOLLOMP_RAW_L34_ARPTRF_TABLE1;
    break;
    default:
    break;
    }
}

static  void
l34_l4Trf_raw2cfg(apollomp_raw_l34_l4_trf_t table,rtk_l34_l4_trf_t *cfg)
{
    switch(table){
    case APOLLOMP_RAW_L34_ARPTRF_TABLE0:
        *cfg = RTK_L34_L4TRF_TABLE0;
    break;
    case APOLLOMP_RAW_L34_ARPTRF_TABLE1:
        *cfg =RTK_L34_L4TRF_TABLE1;
    break;
    default:
    break;
    }
}

 static void
 l34_arpTrf_raw2cfg(apollomp_raw_l34_arp_trf_t table,rtk_l34_arp_trf_t *cfg)
{
    switch(table){
    case APOLLOMP_RAW_L34_ARPTRF_TABLE0:
        *cfg = RTK_L34_ARPTRF_TABLE0;
    break;
    case APOLLOMP_RAW_L34_ARPTRF_TABLE1:
        *cfg =RTK_L34_ARPTRF_TABLE1;
    break;
    default:
    break;
    }
}



/*
 * Function Declaration
 */
/* Function Name:
 *      dal_apollomp_l34_hsb_set
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
dal_apollomp_l34_hsb_set(apollomp_l34_hsb_param_t *hsb)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < APOLLOMP_L34_HSB_WORD; index++)
    {
        if ((ret = reg_write((APOLLOMP_HSB_DESC_W0r + index), (uint32 *)(&hsb->hsbWords[index])))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;

}/* end of dal_apollomp_l34_hsb_set */



/* Function Name:
 *      dal_apollomp_l34_hsb_get
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
dal_apollomp_l34_hsb_get(apollomp_l34_hsb_param_t *hsb)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < APOLLOMP_L34_HSB_WORD; index++)
    {
        if ((ret = reg_read((APOLLOMP_HSB_DESC_W0r + index), (uint32 *)(&hsb->hsbWords[index])))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;


}/* end of dal_apollomp_l34_hsb_get */





/* Function Name:
 *      dal_apollomp_l34_hsa_set
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
dal_apollomp_l34_hsa_set(apollomp_l34_hsa_param_t *hsa)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < APOLLOMP_L34_HSA_WORD; index++)
    {
        if ((ret = reg_write((APOLLOMP_HSA_DESC_W0r + index), (uint32 *)(&hsa->hsaWords[index])))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;

}/* end of dal_apollomp_l34_hsa_set */



/* Function Name:
 *      dal_apollomp_l34_hsa_get
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
dal_apollomp_l34_hsa_get(apollomp_l34_hsa_param_t *hsa)
{
    int32 ret;
    uint32 index;

    for(index = 0; index < APOLLOMP_L34_HSA_WORD; index++)
    {
        if ((ret = reg_read((APOLLOMP_HSA_DESC_W0r + index), (uint32 *)(&hsa->hsaWords[index])))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;


}/* end of dal_apollomp_l34_hsa_get */


/* Function Name:
 *      dal_apollomp_l34_hsabCtrMode_set
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
dal_apollomp_l34_hsabCtrMode_set(apollomp_l34_hsab_mode_t mode)
{
    int32 ret;
    if ((ret = reg_field_write(APOLLOMP_HSBA_CTRLr,APOLLOMP_TST_LOG_MDf,(uint32 *)&mode))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}/* end of dal_apollomp_l34_hsabCtrMode_set */





/* Function Name:
 *      dal_apollomp_l34_init
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
dal_apollomp_l34_init(void)
{
    int32 ret;
    int32 i;
    rtk_l34_globalStateType_t cfg;

    l34_init = INIT_COMPLETED;
    /*reset all table*/
    for(i=0;i<L34_TABLE_END;i++)
    {
        ret = dal_apollomp_l34_table_reset(i);
        if( (ret != RT_ERR_OK) && (ret != RT_ERR_INPUT))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            l34_init = INIT_NOT_COMPLETED ;
                return ret;
        }
    }
    /*enable l34 routing */
    cfg = L34_GLOBAL_L34_STATE;
    if((ret = dal_apollomp_l34_globalState_set(cfg, ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        l34_init = INIT_NOT_COMPLETED ;
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_l34_init */





/* Function Name:
 *      dal_apollomp_l34_netifTable_set
 * Description:
 *      Set netif entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
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
dal_apollomp_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    int32 ret;
    uint32 tempVal;

    apollomp_l34_netif_entry_t netif_entry;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < entry->vlan_id), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->enable_rounting != 0 && entry->enable_rounting != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->mtu >= 16384), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->mac_mask != 0 && entry->mac_mask != 4 && entry->mac_mask != 6 && entry->mac_mask != 7), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&netif_entry, 0x0, sizeof(netif_entry));


    tempVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_VALIDtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->mtu;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_MTUtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->enable_rounting;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_ENRTRtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->mac_mask;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_MACMASKtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_mac_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_GMACtf, (uint8 *)&entry->gateway_mac, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->vlan_id;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_VLANIDtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(APOLLOMP_NETIFt, idx, (uint32 *)&netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_l34_netifTable_set */



/* Function Name:
 *      dal_apollomp_l34_netifTable_get
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
dal_apollomp_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    int32 ret;
    apollomp_l34_netif_entry_t netif_entry;
    uint32 tmpVal;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&netif_entry, 0x0, sizeof(netif_entry));

    if ((ret = table_read(APOLLOMP_NETIFt, idx, (uint32 *)&netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_MTUtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->mtu = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_ENRTRtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->enable_rounting = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_MACMASKtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->mac_mask = tmpVal;

    if ((ret = table_field_mac_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_GMACtf, (uint8 *)&(entry->gateway_mac), (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_VLANIDtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->vlan_id = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_netifTable_get */


/* Function Name:
 *      dal_apollomp_l34_arpTable_set
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
dal_apollomp_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    int32 ret;
    apollomp_l34_arp_entry_t apollomp_arp_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX()<=entry->nhIdx), RT_ERR_ENTRY_INDEX);


    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&apollomp_arp_entry, 0x0, sizeof(apollomp_arp_entry));

    if ((ret = table_field_set(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_NXTHOPIDXtf, (uint32 *)&entry->nhIdx, (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_set(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_VALIDtf, (uint32 *)&entry->valid, (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_ARP_TABLEt, idx, (uint32 *)&apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_arpTable_set */



/* Function Name:
 *      dal_apollomp_l34_arpTable_get
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
dal_apollomp_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    int32 ret;
    apollomp_l34_arp_entry_t apollomp_arp_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&apollomp_arp_entry, 0x0, sizeof(apollomp_arp_entry));

    if ((ret = table_read(APOLLOMP_ARP_TABLEt, idx, (uint32 *)&apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_NXTHOPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;
    entry->index = idx;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_arpTable_get */

/* Function Name:
 *      dal_apollomp_l34_arpTable_del
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
dal_apollomp_l34_arpTable_del(uint32 idx)
{
    int32 ret;
    apollomp_l34_arp_entry_t apollomp_arp_entry;
    uint32 is_valid=0;
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&apollomp_arp_entry, 0x0, sizeof(apollomp_arp_entry));

    if ((ret = table_field_set(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_ARP_TABLEt, idx, (uint32 *)&apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_arpTable_del */



/* Function Name:
 *      dal_apollomp_l34_pppoeTable_set
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
dal_apollomp_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    int32 ret;
    apollomp_l34_pppoe_entry_t apollomp_pppoe_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->sessionID >= 0x10000), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&apollomp_pppoe_entry, 0x0, sizeof(apollomp_pppoe_entry));

    if ((ret = table_field_set(APOLLOMP_PPPOE_TABLEt, APOLLOMP_PPPOE_TABLE_SESIDtf, (uint32 *)&entry->sessionID, (uint32 *) &apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_PPPOE_TABLEt, idx, (uint32 *)&apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_pppoeTable_set */




/* Function Name:
 *      dal_apollomp_l34_pppoeTable_get
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
dal_apollomp_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    int32 ret;
    apollomp_l34_pppoe_entry_t apollomp_pppoe_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&apollomp_pppoe_entry, 0x0, sizeof(apollomp_pppoe_entry));

    if ((ret = table_read(APOLLOMP_PPPOE_TABLEt, idx, (uint32 *)&apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_PPPOE_TABLEt, APOLLOMP_PPPOE_TABLE_SESIDtf, (uint32 *)&(tmp_val), (uint32 *) &apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->sessionID = tmp_val;


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_pppoeTable_get */







/* Function Name:
 *      dal_apollomp_l34_routingTable_set
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
dal_apollomp_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    int32 ret;
    apollomp_l34_routing_entry_t routing_entry;
    rtk_apollomp_table_list_t table_type;
    uint32 tmpVal;
    uint32 nh_num_asic_val;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((31 < entry->ipMask), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_PROCESS_END <= entry->process), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->internal != 0 && entry->internal != 1), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->netifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((128 <= entry->arpStart), RT_ERR_INPUT);
    RT_PARAM_CHK((128 <= entry->arpEnd), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX() <= entry->nhStart), RT_ERR_INPUT);
    RT_PARAM_CHK((17 <= entry->nhNum), RT_ERR_INPUT);
    RT_PARAM_CHK((3 <= entry->nhAlgo), RT_ERR_INPUT);
    RT_PARAM_CHK((7 <= entry->ipDomain), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));


    /*general part*/
    tmpVal = entry->ipAddr;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipMask;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_MASKtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    tmpVal = entry->process;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    tmpVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    tmpVal = entry->internal;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_INTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->rt2waninf;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(entry->process == L34_PROCESS_CPU || entry->process == L34_PROCESS_DROP)
        table_type = APOLLOMP_L3_ROUTING_DROP_TRAPt;
    else if(entry->process == L34_PROCESS_ARP)
        table_type = APOLLOMP_L3_ROUTING_LOCAL_ROUTEt;
    else if(entry->process == L34_PROCESS_NH)
        table_type = APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt;
    else
        return RT_ERR_FAILED;

    /*process == arp*/
    if(table_type == APOLLOMP_L3_ROUTING_LOCAL_ROUTEt)
    {
        tmpVal = entry->netifIdx;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_DENTIFtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        tmpVal = entry->arpStart;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPSTAtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        tmpVal = entry->arpEnd;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPENDtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    /*process == nexthop*/
    else if(table_type == APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt)
    {
        tmpVal = entry->ipDomain;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPDOMAINtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        tmpVal = entry->nhAlgo;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHALGOtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        tmpVal = entry->nhNxt;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHNXTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        tmpVal = entry->nhStart;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_ADDR_STARTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        nh_num_asic_val = _dal_apollomp_l34_routingTable_nhNum_to_asicVal(entry->nhNum);

        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_NUMtf, (uint32 *)&nh_num_asic_val, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }


    }


    if ((ret = table_write(table_type, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_routingTable_set */

/* Function Name:
 *      dal_apollomp_l34_routingTable_get
 * Description:
 *      Get a routing table entry by idx.
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
dal_apollomp_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    int32 ret;
    apollomp_l34_routing_entry_t routing_entry;
    rtk_apollomp_table_list_t table_type;
    uint32 tmp_val;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    if ((ret = table_read(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_PROCESStf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->process = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    entry->ipAddr = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_MASKtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipMask = tmp_val;


    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_INTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->internal = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_RT2WANINFtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->rt2waninf = tmp_val;


    /*get entry process type*/
    if(entry->process == L34_PROCESS_CPU || entry->process == L34_PROCESS_DROP)
        table_type = APOLLOMP_L3_ROUTING_DROP_TRAPt;
    else if(entry->process == L34_PROCESS_ARP)
        table_type = APOLLOMP_L3_ROUTING_LOCAL_ROUTEt;
    else if(entry->process == L34_PROCESS_NH)
        table_type = APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt;
    else
        return RT_ERR_FAILED;


    /*process == arp*/
    if(table_type == APOLLOMP_L3_ROUTING_LOCAL_ROUTEt)
    {
        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_DENTIFtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->netifIdx = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPSTAtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->arpStart = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPENDtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->arpEnd = tmp_val;


    }
    /*process == nexthop*/
    else if(table_type == APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt)
    {
        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPDOMAINtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->ipDomain = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHALGOtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhAlgo = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHNXTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhNxt = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_ADDR_STARTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhStart = tmp_val;



        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_NUMtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhNum = _dal_apollomp_l34_routingTable_asicVal_to_nhNum(tmp_val);

    }
    return RT_ERR_OK;
} /* end of dal_apollomp_l34_routingTable_get */




/* Function Name:
 *      dal_apollomp_l34_routingTable_del
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
dal_apollomp_l34_routingTable_del(uint32 idx)
{
    int32 ret;
    apollomp_l34_routing_entry_t routing_entry;
    uint32 is_valid=0;

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_routingTable_del */


/* Function Name:
 *      dal_apollomp_l34_nexthopTable_set
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
dal_apollomp_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    int32 ret;
    apollomp_l34_nexthop_entry_t nh_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((2048 <= entry->nhIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->ifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= entry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_NH_END <= entry->type), RT_ERR_INPUT);


    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&nh_entry, 0x0, sizeof(nh_entry));


    tmp_val = (uint32)entry->nhIdx;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_NXTHOPIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->pppoeIdx;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_PPPIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->type;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_TYPEtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->ifIdx;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_IFIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->keepPppoe;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_KEEPPPPOPtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }



    if ((ret = table_write(APOLLOMP_NEXT_HOP_TABLEt, idx, (uint32 *)&nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_nexthopTable_set */



/* Function Name:
 *      dal_apollomp_l34_nexthopTable_get
 * Description:
 *      Get a nexthop entry by idx.
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
dal_apollomp_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    int32 ret;
    apollomp_l34_nexthop_entry_t nh_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&nh_entry, 0x0, sizeof(nh_entry));

    if ((ret = table_read(APOLLOMP_NEXT_HOP_TABLEt, idx, (uint32 *)&nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_NXTHOPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_PPPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->pppoeIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_IFIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ifIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->type = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_KEEPPPPOPtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->keepPppoe = tmp_val;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_nexthopTable_get */




/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_set
 * Description:
 *      Set external internal IP entry by idx.
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
dal_apollomp_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    int32 ret;
    apollomp_l34_extip_entry_t extip_entry;
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

    tmp_val = (uint32)entry->pri;
    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->prival;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIVALtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->nhIdx;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_NH_IDXtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->type;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_TYPEtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->valid;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_VALIDtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->extIpAddr;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_EXTIPtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->intIpAddr;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_INTIPtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_extIntIPTable_set */



/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_get
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
dal_apollomp_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    int32 ret;
    apollomp_l34_extip_entry_t extip_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_read(APOLLOMP_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIORITYtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->pri = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIVALtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->prival = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_NH_IDXtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->type = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_EXTIPtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extIpAddr = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_INTIPtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->intIpAddr = tmp_val;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_extIntIPTable_get */



/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_del
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
dal_apollomp_l34_extIntIPTable_del(uint32 idx)
{
    int32 ret;
    apollomp_l34_extip_entry_t extip_entry;
    uint32 is_valid=0;
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_extIntIPTable_del */




/* Function Name:
 *      dal_apollomp_l34_naptInboundTable_set
 * Description:
 *      Set Inband NAPT table by idx.
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
dal_apollomp_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptInband_entry;
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
        osal_memset(&naptInband_entry, 0x0, sizeof(naptInband_entry));
        if ((ret = table_read(APOLLOMP_NAPTR_TABLEt, idx, (uint32 *)&naptInband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        /*get valid field*/
        if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &naptInband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if(tmp_val == 1)
        {
            return RT_ERR_FAILED;
        }
    }


    osal_memset(&naptInband_entry, 0x0, sizeof(naptInband_entry));

    tmp_val = entry->priId;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRIORITYtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->priValid;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRI_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->valid;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->isTcp;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_TCPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->extPortLSB;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTPRT_LSBtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->extIpIdx;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTIP_IDXtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->remHash;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_REM_HASHtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->intPort;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = entry->intIp;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTIPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(APOLLOMP_NAPTR_TABLEt, idx, (uint32 *)&naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptInboundTable_set */






/* Function Name:
 *      dal_apollomp_l34_naptInboundTable_get
 * Description:
 *      Get Inband NAPT table by idx.
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
dal_apollomp_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptInband_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPTR_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&naptInband_entry, 0x0, sizeof(naptInband_entry));

    if ((ret = table_read(APOLLOMP_NAPTR_TABLEt, idx, (uint32 *)&naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRIORITYtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->priId = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRI_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->priValid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_TCPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->isTcp = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTPRT_LSBtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extPortLSB = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTIP_IDXtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extIpIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_REM_HASHtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->remHash= tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->intPort= tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTIPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->intIp = tmp_val;

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptInboundTable_get */


/* Function Name:
 *      dal_apollomp_l34_naptOutboundTable_set
 * Description:
 *      Set Outband NAPT table by idx.
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
dal_apollomp_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptOnband_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((forced != 0 && forced != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValid != 0 && entry->priValid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValue >= RTK_MAX_NUM_OF_PRIORITY), RT_ERR_PRIORITY);
    RT_PARAM_CHK((entry->hashIdx >=1024), RT_ERR_INPUT);



    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* check if the index is valid*/
    if(forced == 0)
    {
        osal_memset(&naptOnband_entry, 0x0, sizeof(naptOnband_entry));
        if ((ret = table_read(APOLLOMP_NAPT_TABLEt, idx, (uint32 *)&naptOnband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        /*get valid field from napt table*/
        if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if(tmp_val == 1)
        {
            return RT_ERR_FAILED;
        }
    }


    osal_memset(&naptOnband_entry, 0x0, sizeof(naptOnband_entry));

    tmp_val = (uint32)entry->valid;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->hashIdx;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_HASHIN_IDXtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->priValid;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRI_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->priValue;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_NAPT_TABLEt, idx, (uint32 *)&naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptOutboundTable_set */



/* Function Name:
 *      dal_apollomp_l34_naptOutboundTable_get
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
dal_apollomp_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptOnband_entry;
    uint32 tmp_val;
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&naptOnband_entry, 0x0, sizeof(naptOnband_entry));

    if ((ret = table_read(APOLLOMP_NAPT_TABLEt, idx, (uint32 *)&naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;


    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_HASHIN_IDXtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->hashIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRI_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->priValid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->priValue = tmp_val;

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptOutboundTable_get */



/* Function Name:
 *      dal_apollomp_l34_table_reset
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
dal_apollomp_l34_table_reset(rtk_l34_table_type_t type)
{
    int32 ret;
    uint32 reg_field;
    uint32 reset_status,busy,count;
    /*get reset table type*/
    switch(type)
    {
        case L34_ROUTING_TABLE:
            reg_field = APOLLOMP_RST_L3f;
            break;
        case L34_PPPOE_TABLE:
            reg_field = APOLLOMP_RST_PPf;
            break;
        case L34_NEXTHOP_TABLE:
            reg_field = APOLLOMP_RST_NHf;
            break;
        case L34_NETIF_TABLE:
            reg_field = APOLLOMP_RST_IFf;
            break;
        case L34_INTIP_TABLE:
            reg_field = APOLLOMP_RST_IPf;
            break;
        case L34_ARP_TABLE:
            reg_field = APOLLOMP_RST_ARPf;
            break;
        case L34_NAPTR_TABLE:
            reg_field = APOLLOMP_RST_NAPTRf;
            break;
        case L34_NAPT_TABLE:
            reg_field = APOLLOMP_RST_NAPTf;
            break;

        case L34_IPV6_ROUTING_TABLE:
            reg_field = APOLLOMP_RST_V6RTf;
            break;

        case L34_BINDING_TABLE:
            reg_field = APOLLOMP_RST_BDf;
            break;

        case L34_IPV6_NEIGHBOR_TABLE:
            reg_field = APOLLOMP_RST_NBf;
            break;

        case L34_WAN_TYPE_TABLE:
            reg_field = APOLLOMP_RST_WTf;
            break;

        default:
            return RT_ERR_INPUT;
    }
    /*get register*/
    if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CLRr,reg_field,&reset_status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if(reset_status == 0)
    {/*table is not perform reset, we reset it*/
        reset_status = 1;
        if ((ret = reg_field_write(APOLLOMP_NAT_TBL_ACCESS_CLRr,reg_field,&reset_status))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }

    /* Wait operation completed */
    count = 0;
    do
    {
        if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CLRr, reg_field, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
      count++;
    } while (busy && count<=10);

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_table_reset */







/* Function Name:
 *      dal_apollomp_l34_wanTypeTable_set
 * Description:
 *      Set WAN type entry by idx.
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
dal_apollomp_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    apollomp_l34_wan_type_entry_t wanType_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->nhIdx >= HAL_L34_NH_ENTRY_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->wanType >= L34_WAN_TYPE_END), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&wanType_entry, 0x0, sizeof(apollomp_l34_wan_type_entry_t));

    tmpVal = entry->nhIdx;
    if ((ret = table_field_set(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_NXHOPTBIDXtf, (uint32 *)&tmpVal, (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->wanType;
    if ((ret = table_field_set(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_WAN_TYPEtf, (uint32 *)&tmpVal, (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_WAN_TYPE_TABLEt, idx, (uint32 *)&wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_wanTypeTable_set */




/* Function Name:
 *      dal_apollomp_l34_wanTypeTable_get
 * Description:
 *      Get a WAN type entry by idx.
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
dal_apollomp_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *entry)
{
    int32 ret;
    apollomp_l34_wan_type_entry_t wanType_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&wanType_entry, 0x0, sizeof(apollomp_l34_wan_type_entry_t));

    if ((ret = table_read(APOLLOMP_WAN_TYPE_TABLEt, idx, (uint32 *)&wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_NXHOPTBIDXtf, (uint32 *)&(tmp_val), (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_WAN_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->wanType = tmp_val;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_wanTypeTable_get */



/* Function Name:
 *      dal_apollomp_l34_bindingTable_set
 * Description:
 *      Set binding table entry by idx.
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
dal_apollomp_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    apollomp_l34_binding_entry_t bindingEntry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_BINDING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->wanTypeIdx >= APOLLOMP_L34_WAN_TYPE_TABLE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->vidLan > RTK_VLAN_ID_MAX), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&bindingEntry, 0x0, sizeof(apollomp_l34_binding_entry_t));

    tmpVal = entry->wanTypeIdx;
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_WAN_TYPE_INDEXtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->vidLan;
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_VID_LANtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->portMask.bits[0];
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_PORT_MASKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->extPortMask.bits[0];
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_EXT_PMSKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->bindProto;
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_BIND_PTLtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if ((ret = table_write(APOLLOMP_BINDING_TABLEt, idx, (uint32 *)&bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_bindingTable_set */



/* Function Name:
 *      dal_apollomp_l34_bindingTable_get
 * Description:
 *      Get a binding table entry by idx.
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
dal_apollomp_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *entry)
{
    int32 ret;
    apollomp_l34_binding_entry_t bindingEntry;
    uint32 tmpVal;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_BINDING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&bindingEntry, 0x0, sizeof(apollomp_l34_binding_entry_t));

    if ((ret = table_read(APOLLOMP_BINDING_TABLEt, idx, (uint32 *)&bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_WAN_TYPE_INDEXtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->wanTypeIdx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_VID_LANtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->vidLan = tmpVal;

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_PORT_MASKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->portMask.bits[0] = tmpVal;

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_EXT_PMSKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extPortMask.bits[0] = tmpVal;


    /*binding protocal*/
    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_BIND_PTLtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->bindProto = tmpVal;




    return RT_ERR_OK;
} /* end of dal_apollomp_l34_bindingTable_get */


/* Function Name:
 *      dal_apollomp_l34_ipv6NeighborTable_set
 * Description:
 *      Set IPv6 neighbor table entry by idx.
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
dal_apollomp_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    uint64 tmpVal64;
    apollomp_l34_ipv6_neighbor_entry_t rawEntry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_neighbor_entry_t));

    tmpVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->l2Idx;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_L2_TABLE_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipv6RouteIdx;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_RT_MATCH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    memcpy(&tmpVal64,&(entry->ipv6Ifid),sizeof(uint64));

    if ((ret = table_field_byte_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_IP6IF_IDtf, (uint8 *)&tmpVal64,(uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_NEIGHBOR_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6NeighborTable_set */



/* Function Name:
 *      dal_apollomp_l34_ipv6NeighborTable_get
 * Description:
 *      Get a IPv6 neighbor table entry by idx.
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
dal_apollomp_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *entry)
{
    int32 ret;
    apollomp_l34_ipv6_neighbor_entry_t rawEntry;
    uint32 tmpVal = 0;
    uint64 tmpVal64 = 0;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_neighbor_entry_t));

    if ((ret = table_read(APOLLOMP_NEIGHBOR_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;


    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_L2_TABLE_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->l2Idx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_RT_MATCH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipv6RouteIdx = tmpVal;

    if ((ret = table_field_byte_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_IP6IF_IDtf, (uint8 *)&tmpVal64, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    memcpy(&(entry->ipv6Ifid),&tmpVal64,sizeof( uint64));

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6NeighborTable_get */



/* Function Name:
 *      dal_apollomp_l34_ipv6RoutingTable_set
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
dal_apollomp_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    apollomp_l34_ipv6_routing_entry_t rawEntry;
    rtk_ipv6_addr_t     ipv6Addr;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_routing_entry_t));

    tmpVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->type;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->nhOrIfidIdx;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_NEXTHOPtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipv6PrefixLen;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_PREFIX_LENtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->rt2waninf;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    osal_memcpy(&ipv6Addr,&entry->ipv6Addr,sizeof(rtk_ipv6_addr_t));
    if ((ret = table_field_byte_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_DIPtf, (uint8 *)&ipv6Addr, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_IPV6_ROUTING_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6RoutingTable_set */



/* Function Name:
 *      dal_apollomp_l34_ipv6RoutingTable_get
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
dal_apollomp_l34_ipv6RoutingTable_get(uint32 idx,rtk_ipv6Routing_entry_t *entry)
{
    int32 ret;
    apollomp_l34_ipv6_routing_entry_t rawEntry;
    uint32 tmpVal;
    rtk_ipv6_addr_t     ipv6Addr;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_routing_entry_t));

    if ((ret = table_read(APOLLOMP_IPV6_ROUTING_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->type = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_NEXTHOPtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhOrIfidIdx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_PREFIX_LENtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipv6PrefixLen = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->rt2waninf = tmpVal;

    if ((ret = table_field_byte_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_DIPtf, (uint8 *)&ipv6Addr, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    memcpy(&(entry->ipv6Addr),&ipv6Addr,sizeof(rtk_ipv6_addr_t));

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6RoutingTable_get */

/* Function Name:
 *      dal_apollomp_l34_ipmcTransTable_set
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
dal_apollomp_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    int32 ret;
    apollomp_raw_l34_ipmcTransEntry_t   rawEntry;

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_IPMC_TRANS_MAX < idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= pEntry->netifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= pEntry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX() <= pEntry->extipIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->sipTransEnable), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_PPPOE_ACT_END <= pEntry->pppoeAct), RT_ERR_INPUT);

    osal_memset(&rawEntry, 0x00, sizeof(apollomp_raw_l34_ipmcTransEntry_t));
    rawEntry.index          = idx;
    rawEntry.netifIdx       = pEntry->netifIdx;
    rawEntry.sipTransEnable = pEntry->sipTransEnable;
    rawEntry.extipIdx       = pEntry->extipIdx;
    rawEntry.pppoeAct      = pEntry->pppoeAct;
    rawEntry.pppoeIdx       = pEntry->pppoeIdx;

    if ((ret = apollomp_raw_l34_ipmcTransEntry_set(&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of dal_apollomp_l34_ipmcTransTable_set */


/* Function Name:
 *      dal_apollomp_l34_ipmcTransTable_get
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
dal_apollomp_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    int32 ret;
    apollomp_raw_l34_ipmcTransEntry_t   rawEntry;

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_IPMC_TRANS_MAX < idx), RT_ERR_ENTRY_INDEX);

    osal_memset(&rawEntry, 0x00, sizeof(apollomp_raw_l34_ipmcTransEntry_t));
    rawEntry.index = idx;
    if ((ret = apollomp_raw_l34_ipmcTransEntry_get(&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    pEntry->netifIdx        = rawEntry.netifIdx;
    pEntry->sipTransEnable  = rawEntry.sipTransEnable;
    pEntry->extipIdx        = rawEntry.extipIdx;
    pEntry->pppoeAct       = rawEntry.pppoeAct;
    pEntry->pppoeIdx        = rawEntry.pppoeIdx;
    return RT_ERR_OK;

} /* end of dal_apollomp_l34_ipmcTransTable_get */


/* Function Name:
 *      dal_apollomp_l34_bindingAction_set
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
dal_apollomp_l34_bindingAction_set(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t action)
{
    int32 ret;
    uint32 rawAct,rawType;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "bindType=%d,action=%d",bindType, action);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_BIND_TYPE_END <=bindType), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_BIND_ACT_END <=action), RT_ERR_INPUT);

    /* function body */
    if((ret = _dal_apollomp_l34_binding_parm_check(bindType,action,&rawType,&rawAct))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }
    /*set to binding act*/

    if((ret = apollomp_raw_l34_bindingAction_set(rawType, rawAct))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_bindingAction_set */


/* Function Name:
 *      dal_apollomp_l34_bindingAction_get
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
dal_apollomp_l34_bindingAction_get(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t *pAction)
{
    int32 ret;
    uint32 rawType,rawAct;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "bindType=%d",bindType);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_BIND_TYPE_END <=bindType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = _dal_apollomp_l34_binding_type_check(bindType,&rawType))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }
    /*get to binding act*/
    if((ret = apollomp_raw_l34_bindingAction_get(rawType,&rawAct))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }
    if((ret = _dal_apollomp_l34_bindingAct_raw2cfg(bindType,rawAct,pAction))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_bindingAction_get */


/* Function Name:
 *      dal_apollomp_l34_hsabMode_set
 * Description:
 *      Set L34 hsab mode
 * Input:
 *      hsabMode - L34 hsab
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
dal_apollomp_l34_hsabMode_set(rtk_l34_hsba_mode_t hsabMode)
{
    int32 ret;
    apollomp_raw_l34_hsba_mode_t mode;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "hsabMode=%d",hsabMode);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_HSBA_END <=hsabMode), RT_ERR_INPUT);

    _dal_apollomp_l34_hsabMode_cfg2raw(hsabMode,&mode);

    /* function body */
    if((ret = apollomp_raw_l34_hsba_mode_set(mode))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }


    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_hsabMode_set */


/* Function Name:
 *      dal_apollomp_l34_hsabMode_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      *pHsabMode - point of L34 hsab
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hsabMode_get(rtk_l34_hsba_mode_t *pHsabMode)
{
    int32 ret;
    apollomp_raw_l34_hsba_mode_t mode;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsabMode), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = apollomp_raw_l34_hsba_mode_get(&mode))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }
    _dal_apollomp_l34_hsabMode_raw2cfg(mode,pHsabMode);


    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_hsabMode_get */



/* Function Name:
 *      dal_apollomp_l34_hsaData_get
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
dal_apollomp_l34_hsaData_get(rtk_l34_hsa_t *pHsaData)
{
    int32 ret;
    apollomp_raw_l34_hsa_t entry;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsaData), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = apollomp_raw_l34_hsaData_get(&entry))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }

    pHsaData->action = entry.action;
    pHsaData->pppid_idx= entry.pppid_idx;
    pHsaData->pppoeKeep= entry.pppoeKeep;
    pHsaData->reason= entry.reason;
    pHsaData->bindVidTrans= entry.bindVidTrans;
    pHsaData->difid= entry.difid;
    pHsaData->dvid= entry.dvid;
    pHsaData->frag = entry.frag;
    pHsaData->interVlanIf = entry.interVlanIf;
    pHsaData->ip = entry.ip;
    pHsaData->l2trans = entry.l2trans;
    pHsaData->l34trans = entry.l34trans;
    pHsaData->l3_chksum = entry.l3_chksum;
    pHsaData->l4_chksum = entry.l4_chksum;
    pHsaData->l4_pri_sel = entry.l4_pri_sel;
    pHsaData->l4_pri_valid = entry.l4_pri_valid;
    pHsaData->nexthop_mac_idx = entry.nexthop_mac_idx;
    pHsaData->port = entry.port;

    switch (entry.pppoe_if)
    {
        case APOLLOMP_RAW_L34_HSA_PPPOE_INACT:
            pHsaData->pppoe_if = L34_HSA_PPPOE_INACT;
            break;
        case APOLLOMP_RAW_L34_HSA_PPPOE_TAGGING:
            pHsaData->pppoe_if = L34_HSA_PPPOE_TAGGING;
            break;
        case APOLLOMP_RAW_L34_HSA_PPPOE_REMOVE:
            pHsaData->pppoe_if = L34_HSA_PPPOE_REMOVE;
            break;
        case APOLLOMP_RAW_L34_HSA_PPPOE_MODIFY:
            pHsaData->pppoe_if = L34_HSA_PPPOE_MODIFY;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_hsaData_get */


/* Function Name:
 *      dal_apollomp_l34_hsbData_get
 * Description:
 *      Get L34 hsab mode
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
dal_apollomp_l34_hsbData_get(rtk_l34_hsb_t *pHsbData)
{
        int32 ret;
    apollomp_raw_l34_hsb_t entry;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsbData), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = apollomp_raw_l34_hsbData_get(&entry))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
            return ret;
    }

    pHsbData->cpu_direct_tx = entry.cpu_direct_tx;
    pHsbData->cvid= entry.cvid;
    pHsbData->cvlan_if= entry.cvlan_if;
    pHsbData->dip= entry.dip;
    pHsbData->dmac= entry.dmac;
    pHsbData->dport_l4chksum= entry.dport_l4chksum;
    pHsbData->ipfrag_s= entry.ipfrag_s;
    pHsbData->ipmf= entry.ipmf;
    pHsbData->isFromWan = entry.isFromWan;
    pHsbData->l2bridge = entry.l2bridge;
    pHsbData->l3_chksum_ok = entry.l3_chksum_ok;
    pHsbData->l4_chksum_ok = entry.l4_chksum_ok;
    pHsbData->len = entry.len;
    pHsbData->parse_fail = entry.parse_fail;
    pHsbData->pppoe_id = entry.pppoe_id;
    pHsbData->pppoe_if = entry.pppoe_if;
    memcpy(&(pHsbData->sip.ipv6_addr[12]),&entry.sip,4);
    pHsbData->sport_icmpid_chksum = entry.sport_icmpid_chksum;
    pHsbData->svlan_if = entry.svlan_if;
    pHsbData->tcp_flag = entry.tcp_flag;
    pHsbData->ttls = entry.ttls;
    pHsbData->udp_no_chksum = entry.udp_no_chksum;

    switch (entry.type)
    {
        case APOLLOMP_RAW_L34_HSB_ETHERNET:
            pHsbData->type = L34_HSB_ETHERNET;
            break;
        case APOLLOMP_RAW_L34_HSB_RESERVED_PPTP:
            pHsbData->type = L34_HSB_RESERVED_PPTP;
            break;
        case APOLLOMP_RAW_L34_HSB_IPV4:
            pHsbData->type = L34_HSB_IPV4;
            break;
        case APOLLOMP_RAW_L34_HSB_ICMP:
            pHsbData->type = L34_HSB_ICMP;
            break;
        case APOLLOMP_RAW_L34_HSB_IGMP:
            pHsbData->type = L34_HSB_IGMP;
            break;
        case APOLLOMP_RAW_L34_HSB_TCP:
            pHsbData->type = L34_HSB_TCP;
            break;
        case APOLLOMP_RAW_L34_HSB_UDP:
            pHsbData->type = L34_HSB_UDP;
            break;
        case APOLLOMP_RAW_L34_HSB_IPV6:
            pHsbData->type = L34_HSB_IPV6;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_hsbData_get */


/* Function Name:
 *      dal_apollomp_l34_portWanMap_set
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
dal_apollomp_l34_portWanMap_set(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t portWanMapEntry)
{
    int32 ret;
    rtk_action_t act;
    int32 (*fun)(rtk_port_t , uint32 , rtk_action_t );
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "portWanMapType=%d",portWanMapType);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK(( HAL_L34_NETIF_ENTRY_MAX()<=portWanMapEntry.wanIdx), RT_ERR_INPUT);

    switch(portWanMapEntry.act)
    {
        case L34_PORTMAP_ACT_DROP:
            act = ACTION_DROP;
            break;
        case L34_PORTMAP_ACT_PERMIT:
            act =ACTION_FORWARD;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    switch(portWanMapType)
    {
        case L34_PORTMAP_TYPE_PORT2WAN:
            fun = apollomp_raw_l34_portToWanAction_set;
            if((ret = fun(portWanMapEntry.port,portWanMapEntry.wanIdx,act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        case L34_PORTMAP_TYPE_EXT2WAN:
            fun = apollomp_raw_l34_extPortToWanAction_set;
            if((ret = fun(portWanMapEntry.port,portWanMapEntry.wanIdx,act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        case L34_PORTMAP_TYPE_WAN2EXT:
            fun = apollomp_raw_l34_wanToExtPortAction_set;
            if((ret = fun(portWanMapEntry.wanIdx,portWanMapEntry.port,act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        case L34_PORTMAP_TYPE_WAN2PORT:
            fun = apollomp_raw_l34_wanToPortAction_set;
            if((ret = fun(portWanMapEntry.wanIdx,portWanMapEntry.port,act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
        break;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_portWanMap_set */


/* Function Name:
 *      dal_apollomp_l34_portWanMap_get
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
dal_apollomp_l34_portWanMap_get(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t *pPortWanMapEntry)
{
    int32 ret;
    rtk_action_t act;
    int32 (*fun)(rtk_port_t , uint32 , rtk_action_t* );
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "portWanMapType=%d",portWanMapType);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortWanMapEntry), RT_ERR_NULL_POINTER);

    switch(portWanMapType)
    {
        case L34_PORTMAP_TYPE_PORT2WAN:
            fun = apollomp_raw_l34_portToWanAction_get;
            if((ret = fun(pPortWanMapEntry->port,pPortWanMapEntry->wanIdx,&act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        case L34_PORTMAP_TYPE_EXT2WAN:
            fun = apollomp_raw_l34_extPortToWanAction_get;
            if((ret = fun(pPortWanMapEntry->port,pPortWanMapEntry->wanIdx,&act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        case L34_PORTMAP_TYPE_WAN2EXT:
            fun = apollomp_raw_l34_wanToExtPortAction_get;
            if((ret = fun(pPortWanMapEntry->wanIdx,pPortWanMapEntry->port,&act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        case L34_PORTMAP_TYPE_WAN2PORT:
            fun = apollomp_raw_l34_wanToPortAction_get;
            if((ret = fun(pPortWanMapEntry->wanIdx,pPortWanMapEntry->port,&act))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
                    return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
        break;
    }


    switch(act)
    {
        case ACTION_DROP:
            pPortWanMapEntry->act = L34_PORTMAP_ACT_DROP;
            break;
        case ACTION_FORWARD:
            pPortWanMapEntry->act = L34_PORTMAP_ACT_PERMIT;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_portWanMap_get */


/* Sub-module Name: System configuration */

/* Function Name:
 *      dal_apollomp_l34_globalState_set
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
dal_apollomp_l34_globalState_set(rtk_l34_globalStateType_t stateType,rtk_enable_t state)
{

    int32 ret;
    uint32 wData;

    apollomp_raw_l34_pppoeKeepMode_t pppMode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "stateType=%d,state=%d",stateType, state);

    /* check Init status */
        RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_GLOBAL_STATE_END <=stateType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    switch(stateType){
    case L34_GLOBAL_L34_STATE:
        if((ret=apollomp_raw_l34_globalFunction_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_L3NAT_STATE:
        if((ret= l34_mode_l3_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_L4NAT_STATE:
        if((ret= l34_mode_l4_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_L3CHKSERRALLOW_STATE:
        if((ret=apollomp_raw_l34_L3chksumErrAllow_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_L4CHKSERRALLOW_STATE:
        if((ret=apollomp_raw_l34_L4chksumErrAllow_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_TTLMINUS_STATE:
        if((ret=apollomp_raw_l34_TtlOperationMode_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_NAT2LOG_STATE:
        if((ret=apollomp_raw_l34_natAttack2CPU_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_FRAG2CPU_STATE:
        if((ret=apollomp_raw_l34_FragPkt2Cpu_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_BIND_STATE:
        if((ret=apollomp_raw_l34_bindFunction_set(state))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_GLOBAL_PPPKEEP_STATE:
        pppMode = state ? APOLLOMP_RAW_L34_PPPOE_KEEP : APOLLOMP_RAW_L34_PPPOE_BY_ASIC;
        if((ret=apollomp_raw_l34_pppoeKeep_set(pppMode))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;


    case L34_GLOBAL_KEEP_ORG_STATE:
        if(ENABLED==state)
            wData = 1;
        else
            wData = 0;

        if ((ret = reg_field_write(APOLLOMP_L34_GLB_CFGr, APOLLOMP_CFG_CPU_ORG_OPf, (uint32 *)&wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    break;

    case L34_GLOBAL_ADV_PPPOE_MTU_CAL_STATE:
        if(ENABLED==state)
            wData = 1;
        else
            wData = 0;

        if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_EN_NEW_PP_MTUf, (uint32 *)&wData)) != RT_ERR_OK)
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

}   /* end of dal_apollomp_l34_globalState_set */


/* Function Name:
 *      dal_apollomp_l34_globalState_get
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
dal_apollomp_l34_globalState_get(rtk_l34_globalStateType_t stateType,rtk_enable_t *pState)
{

        int32 ret;
        uint32 rData;
    apollomp_raw_l34_mode_t l34mode;
    apollomp_raw_l34_pppoeKeepMode_t pppMode;

        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "stateType=%d",stateType);

        /* check Init status */
        RT_INIT_CHK(l34_init);

        /* parameter check */
        RT_PARAM_CHK((L34_GLOBAL_STATE_END <=stateType), RT_ERR_INPUT);
        RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    switch(stateType){
    case L34_GLOBAL_L34_STATE:
    if((ret=apollomp_raw_l34_globalFunction_get(pState)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_L3NAT_STATE:
    if((ret=apollomp_raw_l34_mode_get(&l34mode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    *pState = l34mode & 1;
    break;
    case L34_GLOBAL_L4NAT_STATE:
    if((ret=apollomp_raw_l34_mode_get(&l34mode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    *pState = (l34mode >> 1) & 1;
    break;
    case L34_GLOBAL_L3CHKSERRALLOW_STATE:
    if((ret=apollomp_raw_l34_L3chksumErrAllow_get(pState))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_L4CHKSERRALLOW_STATE:
    if((ret=apollomp_raw_l34_L4chksumErrAllow_get(pState))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_NAT2LOG_STATE:
    if((ret=apollomp_raw_l34_natAttack2CPU_get(pState))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_FRAG2CPU_STATE:
    if((ret=apollomp_raw_l34_FragPkt2Cpu_get(pState))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_TTLMINUS_STATE:
    if((ret=apollomp_raw_l34_TtlOperationMode_get(pState))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_BIND_STATE:
    if((ret=apollomp_raw_l34_bindFunction_get(pState))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    break;
    case L34_GLOBAL_PPPKEEP_STATE:
    if((ret=apollomp_raw_l34_pppoeKeep_get(&pppMode))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    *pState = (pppMode== APOLLOMP_RAW_L34_PPPOE_KEEP) ? ENABLED : DISABLED;
    break;


    case L34_GLOBAL_KEEP_ORG_STATE:
    if ((ret = reg_field_read(APOLLOMP_L34_GLB_CFGr, APOLLOMP_CFG_CPU_ORG_OPf, (uint32 *)&rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    if(rData==1)
        *pState = ENABLED;
    else
        *pState = DISABLED;
    break;

    case L34_GLOBAL_ADV_PPPOE_MTU_CAL_STATE:

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_EN_NEW_PP_MTUf, (uint32 *)&rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    if(rData==1)
        *pState = ENABLED;
    else
        *pState = DISABLED;
    break;

    default:
        return RT_ERR_INPUT;
    break;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_globalState_get */



/* Function Name:
 *      dal_apollomp_l34_lookupMode_set
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
dal_apollomp_l34_lookupMode_set(rtk_l34_lookupMode_t lookupMode)
{
        int32 ret;

    apollomp_raw_l34_limbc_t mode;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "lookupMode=%d",lookupMode);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_LOOKUP_MODE_END <=lookupMode), RT_ERR_INPUT);

    if((ret= l34_lookupMode_cfg2raw(lookupMode,&mode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = apollomp_raw_l34_LanIntfMDBC_set(mode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;

}   /* end of dal_apollomp_l34_lookupMode_set */


/* Function Name:
 *      dal_apollomp_l34_lookupMode_get
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
dal_apollomp_l34_lookupMode_get(rtk_l34_lookupMode_t *pLookupMode)
{
    int32 ret;
    apollomp_raw_l34_limbc_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLookupMode), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l34_LanIntfMDBC_get(&mode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = l34_lookupMode_raw2cfg(mode,pLookupMode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_lookupMode_get */



/* Function Name:
 *      dal_apollomp_l34_lookupPortMap_set
 * Description:
 *      configure l34 port base mapping
 * Input:
 *   portType   - port type, mac port/ext port/vc port
 *     portId   - port identity
 *   wanIdx - port based to wanIdx
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
dal_apollomp_l34_lookupPortMap_set(rtk_l34_portType_t portType, uint32 portId, uint32 wanIdx)
{
      int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "portType=%d,portId=%d,wanIdx=%d",portType, portId, wanIdx);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_PORT_TYPE_END <=portType), RT_ERR_INPUT);
    switch(portType){
    case L34_PORT_MAC:
        RT_PARAM_CHK((L34_MAC_PORT_MAX <=portId), RT_ERR_INPUT);
    break;
    case L34_PORT_EXTENSION:
        RT_PARAM_CHK((L34_EXT_PORT_MAX<=portId), RT_ERR_INPUT);
    break;
    case L34_PORT_DSLVC:
        RT_PARAM_CHK((L34_DSLVC_PORT_MAX <=portId), RT_ERR_INPUT);
    default:
    break;
    }
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <=wanIdx), RT_ERR_INPUT);

    /* function body */
    switch(portType){
    case L34_PORT_MAC:
        if((ret =apollomp_raw_l34_portIntfIdx_set(portId,wanIdx))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_PORT_EXTENSION:
        if((ret =apollomp_raw_l34_extPortIntfIdx_set(portId,wanIdx))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    case L34_PORT_DSLVC:
        if((ret =apollomp_raw_l34_vcPortIntfIdx_set(portId,wanIdx))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_lookupPortMap_set */


/* Function Name:
 *      dal_apollomp_l34_lookupPortMap_get
 * Description:
 *      configure l34 port base mapping
 * Input:
 *   portType   - port type, mac port/ext port/vc port
 *     portId   - port identity
 * Output:
 *   *pWanIdx   - port based to wanIdx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_lookupPortMap_get(rtk_l34_portType_t portType, uint32 portId, uint32 *pWanIdx)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "portType=%d,portId=%d",portType, portId);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_PORT_TYPE_END <=portType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pWanIdx), RT_ERR_NULL_POINTER);

      if((ret = l34_portMapNetif_get(portType,pWanIdx,portId)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_lookupPortMap_get */


/* Function Name:
 *      dal_apollomp_l34_wanRoutMode_set
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
dal_apollomp_l34_wanRoutMode_set(rtk_l34_wanRouteMode_t wanRouteMode)
{
        int32 ret;
    rtk_l34_wanroute_act_t act;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "wanRouteMode=%d",wanRouteMode);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((L34_WANROUTE_END <=wanRouteMode), RT_ERR_INPUT);

    /* function body */
    if((ret = l34_wanroute_cfg2raw(wanRouteMode,&act)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = apollomp_raw_l34_wanRouteAct_set(act)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_wanRoutMode_set */


/* Function Name:
 *      dal_apollomp_l34_wanRoutMode_get
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
dal_apollomp_l34_wanRoutMode_get(rtk_l34_wanRouteMode_t *pWanRouteMode)
{
    int32 ret;
    rtk_l34_wanroute_act_t act;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWanRouteMode), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l34_wanRouteAct_get(&act)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = l34_wanroute_raw2cfg(act,pWanRouteMode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_wanRoutMode_get */


/* Function Name:
 *      dal_apollomp_l34_arpTrfIndicator_get
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
dal_apollomp_l34_arpTrfIndicator_get(uint32 index, rtk_enable_t *pArpIndicator)
{
    int32 ret;
    int i;
    apollomp_raw_l34_arp_trf_t newTable,oldTable;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()+1<index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pArpIndicator), RT_ERR_NULL_POINTER);

    /*get current work table*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_get(&oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    newTable = !oldTable;
    /*clear new table first*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_Clear(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*swap*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_set(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get*/
    if(index==L34_TF_ARP_ALL){
        for(i=0;i<HAL_L34_ARP_ENTRY_MAX();i++){
            if((ret=apollomp_raw_l34_arpTrfTb_get( oldTable, i, &pArpIndicator[i]))!=RT_ERR_OK){
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
            }
        }
    }else{
        if((ret=apollomp_raw_l34_arpTrfTb_get( oldTable, index, pArpIndicator))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }

    /*clear old table*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_Clear(oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_arpTrfIndicator_get */


/* Function Name:
 *      dal_apollomp_l34_naptTrfIndicator_get
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
dal_apollomp_l34_naptTrfIndicator_get(uint32 index, rtk_enable_t *pNaptIndicator)
{

    int32 ret;
    int i;
    apollomp_raw_l34_l4_trf_t newTable,oldTable;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()+1<index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNaptIndicator), RT_ERR_NULL_POINTER);

    /*get current work table*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_get(&oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    newTable = !oldTable;
    /*clear new table first*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_Clear(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*swap*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_set(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get*/
    if(index == L34_TF_NAT_ALL){
        for(i=0;i<HAL_L34_NAPT_ENTRY_MAX();i++){
            if((ret=apollomp_raw_l34_l4TrfTb_get(oldTable,i,&pNaptIndicator[i]))!=RT_ERR_OK){
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
            }
        }

    }else{
        if((ret=apollomp_raw_l34_l4TrfTb_get(oldTable,index,pNaptIndicator))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }

    /*clear old table*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_Clear(oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_naptTrfIndicator_get */



/* Function Name:
 *      dal_apollomp_l34_pppTrfIndicator_get
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
dal_apollomp_l34_pppTrfIndicator_get(uint32 index, rtk_enable_t *pPppIndicator)
{
    int32 ret;
    uint32 pppInd;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPppIndicator), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l34_pppoeTrf_get(&pppInd))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    *pPppIndicator = (pppInd & (1 << index)) ? ENABLED: DISABLED;

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_pppTrfIndicator_get */


/* Function Name:
 *      dal_apollomp_l34_neighTrfIndicator_get
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
dal_apollomp_l34_neighTrfIndicator_get(uint32 index, rtk_enable_t *pNeighIndicator)
{
    int32 ret;
        uint32 tmpVal;
    uint32 tabeId,indexId;
    uint32 reg,field;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_IPV6_NEIGHBOR_ENTRY_MAX()+1 < index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNeighIndicator), RT_ERR_NULL_POINTER);

    tabeId  = index/32;
    indexId = index%32;

    switch(tabeId){
    case 0:
        reg     = APOLLOMP_NB_TRF0r;
        field = APOLLOMP_NBT0f;
    break;
    case 1:
        reg     = APOLLOMP_NB_TRF1r;
        field = APOLLOMP_NBT1f;
    break;
    case 2:
        reg     = APOLLOMP_NB_TRF2r;
        field = APOLLOMP_NBT2f;
    break;
    case 3:
        reg     = APOLLOMP_NB_TRF3r;
        field = APOLLOMP_NBT3f;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }

    if ((ret = reg_field_read(reg, field, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *pNeighIndicator = (tmpVal & (1<< indexId)) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_neighTrfIndicator_get */

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
dal_apollomp_l34_hsdState_set(rtk_enable_t hsdState)
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

    if((ret = reg_field_write(APOLLOMP_RSVD_ALE_HSAr, APOLLOMP_RSVD_MEMf, &pValue))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
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
dal_apollomp_l34_hsdState_get(rtk_enable_t *phsdState)
{
    int32 ret;
    uint32 pValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == phsdState), RT_ERR_NULL_POINTER);

    /* function body */
  if((ret = reg_field_read(APOLLOMP_RSVD_ALE_HSAr, APOLLOMP_RSVD_MEMf, &pValue))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if(pValue == 0x5f)
        *phsdState = ENABLED;
    else
        *phsdState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hsdState_get */


/* Function Name:
 *      dal_apollomp_l34_hwL4TrfWrkTbl_set
 * Description:
 *      Set HW working table id for L4 trf.
 * Input:
 *      dal_apollo_l34_l4_trf_t l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */

int32
dal_apollomp_l34_hwL4TrfWrkTbl_set(rtk_l34_l4_trf_t l4TrfTable)
{
    int32 ret;
    apollomp_raw_l34_l4_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "l4TrfTable=%d",l4TrfTable);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_L34_L4TRF_TABLE_END <=l4TrfTable), RT_ERR_INPUT);

    /* function body */
    l34_l4Trf_cfg2raw(l4TrfTable,&table);
    if((ret = apollomp_raw_l34_hwL4TrfWrkTbl_set(table))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hwL4TrfWrkTbl_set */


/* Function Name:
 *      dal_apollo_l34_hwL4TrfWrkTbl_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      dal_apollo_l34_l4_trf_t *pl4TrfTable
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hwL4TrfWrkTbl_get(rtk_l34_l4_trf_t *pl4TrfTable)
{

    int32 ret;
    apollomp_raw_l34_l4_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pl4TrfTable), RT_ERR_NULL_POINTER);

    /* function body */
   if((ret = apollomp_raw_l34_hwL4TrfWrkTbl_get(&table))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    l34_l4Trf_raw2cfg(table,pl4TrfTable);

    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hwL4TrfWrkTbl_get */

/* Function Name:
 *      dal_apollo_l34_l4TrfTb_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      l4TrfTable - table index
 *      l4EntryIndex - index of l4 table that went to get
 * Output:
 *      pIndicator - indicator for result of state
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_l4TrfTb_get(rtk_l34_l4_trf_t l4TrfTable,uint32 l4EntryIndex,rtk_enable_t *pIndicator)
{

    int32 ret;
    apollomp_raw_l34_l4_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "l4TrfTable=%d,l4EntryIndex=%d",l4TrfTable, l4EntryIndex);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_L34_L4TRF_TABLE_END <=l4TrfTable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=l4EntryIndex), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    l34_l4Trf_cfg2raw(l4TrfTable,&table);
   if((ret = apollomp_raw_l34_l4TrfTb_get(table,l4EntryIndex,pIndicator))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollo_l34_l4TrfTb_get */


/* Function Name:
 *      dal_apollo_l34_hwL4TrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hwL4TrfWrkTbl_Clear(rtk_l34_l4_trf_t l4TrfTable)
{
    int32 ret;
    apollomp_raw_l34_l4_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "l4TrfTable=%d",l4TrfTable);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_L34_L4TRF_TABLE_END <=l4TrfTable), RT_ERR_INPUT);

    /* function body */
   l34_l4Trf_cfg2raw(l4TrfTable,&table);
  if((ret = apollomp_raw_l34_hwL4TrfWrkTbl_Clear(table))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hwL4TrfWrkTbl_Clear */


/* Function Name:
 *      dal_apollo_l34_hwArpTrfWrkTbl_set
 * Description:
 *      Set HW working table id for ARP trf.
 * Input:
 *      dal_apollo_l34_arp_trf_t arpTrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hwArpTrfWrkTbl_set(rtk_l34_arp_trf_t arpTrfTable)
{

    int32 ret;
    apollomp_raw_l34_arp_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "arpTrfTable=%d",arpTrfTable);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_L34_ARPTRF_TABLE_END <=arpTrfTable), RT_ERR_INPUT);

  /* function body */
    l34_arpTrf_cfg2raw(arpTrfTable,&table);
    if((ret = apollomp_raw_l34_hwArpTrfWrkTbl_set(table))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hwArpTrfWrkTbl_set */


/* Function Name:
 *      dal_apollo_l34_hwArpTrfWrkTbl_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      None
 * Output:
 *      dal_apollo_l34_l4_trf_t *pArpTrfTable
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hwArpTrfWrkTbl_get(rtk_l34_arp_trf_t *pArpTrfTable)
{

    int32 ret;
    apollomp_raw_l34_arp_trf_t table;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpTrfTable), RT_ERR_NULL_POINTER);

    /* function body */
  if((ret = apollomp_raw_l34_hwArpTrfWrkTbl_get(&table))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    l34_arpTrf_raw2cfg(table,pArpTrfTable);
    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hwArpTrfWrkTbl_get */

/* Function Name:
 *      dal_apollo_l34_arpTrfTb_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      arpTrfTable - table index
 *      arpEntryIndex - index of l4 table that went to get
 * Output:
 *      pIndicator - indicator for result of state
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_arpTrfTb_get(rtk_l34_arp_trf_t arpTrfTable,uint32 arpEntryIndex,rtk_enable_t *pIndicator)
{
    int32 ret;
    apollomp_raw_l34_arp_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "arpTrfTable=%d,arpEntryIndex=%d",arpTrfTable, arpEntryIndex);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_L34_ARPTRF_TABLE_END <=arpTrfTable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=arpEntryIndex), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    l34_arpTrf_cfg2raw(arpTrfTable,&table);
   if((ret = apollomp_raw_l34_arpTrfTb_get(table,arpEntryIndex,pIndicator))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollo_l34_arpTrfTb_get */


/* Function Name:
 *      dal_apollo_l34_hwArpTrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hwArpTrfWrkTbl_Clear(rtk_l34_arp_trf_t arpTrfTable)
{
    int32 ret;
    apollomp_raw_l34_arp_trf_t table;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "arpTrfTable=%d",arpTrfTable);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_L34_ARPTRF_TABLE_END <=arpTrfTable), RT_ERR_INPUT);

    /* function body */
   l34_arpTrf_cfg2raw(arpTrfTable,&table);
   if((ret = apollomp_raw_l34_hwArpTrfWrkTbl_Clear(table))!=RT_ERR_OK)
    {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollo_l34_hwArpTrfWrkTbl_Clear */


/* Function Name:
 *      dal_apollomp_l34_naptTrfIndicator_get_all
 * Description:
 *      get  napt entry traffic indicator
 * Input:
 *      None
 * Output:
 *    *pNaptMaps    - point of traffic indicator for mask (64*32bit mask)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_naptTrfIndicator_get_all(uint32 *pNaptMaps)
{
    int32 ret,i;
    uint32 base,addr;
    apollomp_raw_l34_l4_trf_t newTable,oldTable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNaptMaps), RT_ERR_NULL_POINTER);

    /*get current work table*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_get(&oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    newTable = !oldTable;
    /*clear new table first*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_Clear(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*swap*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_set(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get*/
    if(oldTable == APOLLOMP_RAW_L34_L4TRF_TABLE0){
        base = HAL_GET_REG_ADDR(APOLLOMP_L4_TRF0r);
    }else{
        base = HAL_GET_REG_ADDR(APOLLOMP_L4_TRF1r);
    }
    /*get data, 64 word 32 bits traffic indicator*/
    for(i=0;i<(HAL_L34_NAPT_ENTRY_MAX()/32);i++){
        addr = base + i*4;
        if (ioal_mem32_read(addr, &pNaptMaps[i]) != RT_ERR_OK)
            {
                return RT_ERR_FAILED;
            }
    }

    /*clear old table*/
    if((ret=apollomp_raw_l34_hwL4TrfWrkTbl_Clear(oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }


    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_naptTrfIndicator_get_all */


/* Function Name:
 *      dal_apollomp_l34_arpTrfIndicator_get_all
 * Description:
 *      get  all arp entry traffic indicator
 * Input:
 *      None
 * Output:
 *    *pArpMaps - point of traffic indicator for arp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_arpTrfIndicator_get_all(uint32 *pArpMaps)
{
    int32 ret,i;
    uint32 base,addr;
    apollomp_raw_l34_arp_trf_t newTable,oldTable;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpMaps), RT_ERR_NULL_POINTER);

    /*get current work table*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_get(&oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    newTable = !oldTable;
    /*clear new table first*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_Clear(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*swap*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_set(newTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get*/
    if(oldTable == APOLLOMP_RAW_L34_ARPTRF_TABLE0){
        base = HAL_GET_REG_ADDR(APOLLOMP_ARP_TRF0r);
    }else{
        base = HAL_GET_REG_ADDR(APOLLOMP_ARP_TRF1r);
    }
    /*get data, 16 word 32 bits traffic indicator*/
    for(i=0;i<(HAL_L34_ARP_ENTRY_MAX()/32);i++){
        addr = base + i*4;
        if (ioal_mem32_read(addr, &pArpMaps[i]) != RT_ERR_OK)
            {
                return RT_ERR_FAILED;
            }
    }

    /*clear old table*/
    if((ret=apollomp_raw_l34_hwArpTrfWrkTbl_Clear(oldTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_arpTrfIndicator_get_all */

/* Function Name:
 *      dal_apollomp_l34_pppTrfIndicator_get_all
 * Description:
 *      get  ppp entry traffic indicator by index
 * Input:
 *      None
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
dal_apollomp_l34_pppTrfIndicator_get_all(rtk_l34_ppp_trf_all_t *pPppIndicator)
{
    int32 ret;
    uint32 pppInd;

    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPppIndicator), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l34_pppoeTrf_get(&pppInd))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L34), "");
        return ret;
    }
    osal_memcpy(&pPppIndicator->trf_state[0], &pppInd, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of dal_apollomp_l34_pppTrfIndicator_get */
