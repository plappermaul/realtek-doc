/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention 
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/sec.h>
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_sec.h>
#include <osal/time.h>

#include <cortina-api/include/classifier.h>
#include <cortina-api/include/rate.h>

/*
 * Symbol Definition
 */
#define __CLS_TCP_FLAGS_BIT_SYN                 (1<<1)
#define __CLS_TCP_FLAGS_BIT_FIN                 (1<<0)

#define SEC_ICMP_FLOW_ID_BASE    128
#define SEC_SYN_FLOW_ID_BASE     SEC_ICMP_FLOW_ID_BASE+32
#define SEC_FIN_FLOW_ID_BASE     SEC_SYN_FLOW_ID_BASE+32

/*
 * Data Declaration
 */
static uint32 sec_init = {INIT_NOT_COMPLETED}; 

typedef struct {
    rtk_enable_t enable;
    ca_uint32_t synfin_deny_index;
    ca_uint32_t icmp_flood_index;
    ca_uint32_t syn_flood_index;
    ca_uint32_t fin_flood_index;
} sec_atkPrvt_cfg_t;

sec_atkPrvt_cfg_t atkPrvt[RTK_MAX_NUM_OF_PORTS];

rtk_action_t SYNFIN_DENY_ACTION = ACTION_FORWARD;
rtk_action_t ICMP_FLOOD_ACTION = ACTION_FORWARD;
rtk_action_t SYN_FLOOD_ACTION = ACTION_FORWARD;
rtk_action_t FIN_FLOOD_ACTION = ACTION_FORWARD;

uint32 ICMP_FLOOD_TH = 0;
uint32 SYN_FLOOD_TH = 0;
uint32 FIN_FLOOD_TH = 0;

uint32 synfin_prio = 4;
uint32 icmp_flood_prio = 4;
uint32 syn_flood_prio = 4;
uint32 fin_flood_prio = 4;


EXPORT_SYMBOL(synfin_prio);
EXPORT_SYMBOL(icmp_flood_prio);
EXPORT_SYMBOL(syn_flood_prio);
EXPORT_SYMBOL(fin_flood_prio);

/*
 * Function Declaration
 */

int32 _dal_luna_g3_synfin_deny_classification_add(rtk_port_t port,rtk_action_t rtk_action,ca_uint32_t *pIndex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    //for RTK_RG_DOS_SYNFIN_DENY 
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));


    key.src_port = port_id;
    key_mask.src_port = 1;

    key.l4.l4_valid = 1;
    key.l4.tcp_flags = __CLS_TCP_FLAGS_BIT_SYN | __CLS_TCP_FLAGS_BIT_FIN;
    key_mask.l4 = 1;
//    key_mask.l4_mask.l4_valid = 1;
    key_mask.l4_mask.tcp_flags = __CLS_TCP_FLAGS_BIT_SYN | __CLS_TCP_FLAGS_BIT_FIN;

    if(rtk_action == ACTION_DROP)
    {
        action.forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if(rtk_action == ACTION_TRAP2CPU)
    {
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = RTK2CA_PORT_ID(HAL_GET_CPU_PORT());
    }
    else
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_classifier_rule_add(0,synfin_prio,&key,&key_mask,&action,pIndex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_synfin_deny_classification_delete(ca_uint32_t index)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

int32 _dal_luna_g3_icmp_flood_classification_add(rtk_port_t port,rtk_action_t rtk_action,uint32 floodThresh,ca_uint32_t *pIndex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 0; /* True - PPS mode, false - BPS mode */
    policer.cir = floodThresh;
    policer.cbs = (policer.cir/1000 + 511) >> 9;
    policer.cbs = (policer.cbs == 0) ? 1: policer.cbs;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/
    policer.pir = floodThresh;
    policer.pbs = (policer.pir/1000 + 511) >> 9;
    policer.pbs = (policer.pbs == 0) ? 1: policer.pbs;

    if((ret = ca_flow_policer_set(0,SEC_ICMP_FLOW_ID_BASE,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    key.ip.ip_valid = 1;
    key.ip.ip_version = 4; //IPv4
    key.ip.ip_protocol = 1; //ICMP
    key_mask.ip = 1;
    key_mask.ip_mask.ip_valid = 1;
    key_mask.ip_mask.ip_version = 1;
    key_mask.ip_mask.ip_protocol = 1;

    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = SEC_ICMP_FLOW_ID_BASE;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,icmp_flood_prio,&key,&key_mask,&action,pIndex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_icmp_flood_classification_delete(ca_uint32_t index)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

int32 _dal_luna_g3_syn_flood_classification_add(rtk_port_t port,rtk_action_t rtk_action,uint32 floodThresh,ca_uint32_t *pIndex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 0; /* True - PPS mode, false - BPS mode */
    policer.cir = floodThresh;
    policer.cbs = (policer.cir/1000 + 511) >> 9;
    policer.cbs = (policer.cbs == 0) ? 1: policer.cbs;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/
    policer.pir = floodThresh;
    policer.pbs = (policer.pir/1000 + 511) >> 9;
    policer.pbs = (policer.pbs == 0) ? 1: policer.pbs;

    if((ret = ca_flow_policer_set(0,SEC_SYN_FLOW_ID_BASE,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    key.l4.l4_valid = 1;
    key.l4.tcp_flags = __CLS_TCP_FLAGS_BIT_SYN;
    key_mask.l4 = 1;
//    key_mask.l4_mask.l4_valid = 1;
    key_mask.l4_mask.tcp_flags = __CLS_TCP_FLAGS_BIT_SYN ;
    
    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = SEC_SYN_FLOW_ID_BASE;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,syn_flood_prio,&key,&key_mask,&action,pIndex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_syn_flood_classification_delete(ca_uint32_t index)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

int32 _dal_luna_g3_fin_flood_classification_add(rtk_port_t port,rtk_action_t rtk_action,uint32 floodThresh,ca_uint32_t *pIndex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 0; /* True - PPS mode, false - BPS mode */
    policer.cir = floodThresh;
    policer.cbs = (policer.cir/1000 + 511) >> 9;
    policer.cbs = (policer.cbs == 0) ? 1: policer.cbs;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/
    policer.pir = floodThresh;
    policer.pbs = (policer.pir/1000 + 511) >> 9;
    policer.pbs = (policer.pbs == 0) ? 1: policer.pbs;

    if((ret = ca_flow_policer_set(0,SEC_FIN_FLOW_ID_BASE,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    key.l4.l4_valid = 1;
    key.l4.tcp_flags = __CLS_TCP_FLAGS_BIT_FIN;
    key_mask.l4 = 1;
//    key_mask.l4_mask.l4_valid = 1;
    key_mask.l4_mask.tcp_flags = __CLS_TCP_FLAGS_BIT_FIN ;
    
    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = SEC_FIN_FLOW_ID_BASE;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,fin_flood_prio,&key,&key_mask,&action,pIndex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_fin_flood_classification_delete(ca_uint32_t index)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

/* Module Name : Security */

/* Function Name:
 *      dal_luna_g3_sec_init
 * Description:
 *      Initialize security module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize security module before calling any sec APIs.
 */
int32
dal_luna_g3_sec_init(void)
{
    uint32  port;

    sec_init = INIT_COMPLETED;

    memset(atkPrvt,0,sizeof(sec_atkPrvt_cfg_t)*RTK_MAX_NUM_OF_PORTS);

    /*disable attack prevent*/    
    HAL_SCAN_ALL_PORT(port)
    {
        atkPrvt[port].enable = DISABLED;
        atkPrvt[port].synfin_deny_index = 0;
    }

    SYNFIN_DENY_ACTION = ACTION_FORWARD;
    ICMP_FLOOD_ACTION = ACTION_FORWARD;
    SYN_FLOOD_ACTION = ACTION_FORWARD;
    FIN_FLOOD_ACTION = ACTION_FORWARD;

    ICMP_FLOOD_TH = 0;
    SYN_FLOOD_TH = 0;
    FIN_FLOOD_TH = 0;

    return RT_ERR_OK;
} /* end of dal_luna_g3_sec_init */

/* Function Name:
 *      dal_luna_g3_sec_portAttackPreventState_get
 * Description:
 *      Per port get attack prevention confi state
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status attack prevention
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status attack prevention:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_luna_g3_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = atkPrvt[port].enable;

    return RT_ERR_OK;
} /* end of dal_luna_g3_sec_portAttackPreventState_get */

/* Function Name:
 *      dal_luna_g3_sec_portAttackPreventState_set
 * Description:
 *      Per port set attack prevention confi state
 * Input:
 *      port   - port id.
 *      enable - status attack prevention
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status attack prevention:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_luna_g3_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret=RT_ERR_OK;
    ca_uint32_t index;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((atkPrvt[port].enable == DISABLED)&&(enable == ENABLED)) //DISABLE -> ENABLE
    {
        if(SYNFIN_DENY_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_synfin_deny_classification_add(port,SYNFIN_DENY_ACTION,&index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].synfin_deny_index = index;
        }

        if(ICMP_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_icmp_flood_classification_add(port,ICMP_FLOOD_ACTION,ICMP_FLOOD_TH,&index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].icmp_flood_index = index;
        }

        if(SYN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_syn_flood_classification_add(port,SYN_FLOOD_ACTION,SYN_FLOOD_TH,&index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].syn_flood_index = index;
        }

        if(FIN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_fin_flood_classification_add(port,FIN_FLOOD_ACTION,FIN_FLOOD_TH,&index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].fin_flood_index = index;
        }
    }
    else if((atkPrvt[port].enable == ENABLED)&&(enable == DISABLED)) //ENABLE -> DISABLE
    {
        if(SYNFIN_DENY_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_synfin_deny_classification_delete(atkPrvt[port].synfin_deny_index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].synfin_deny_index = 0;
        }

        if(ICMP_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_icmp_flood_classification_delete(atkPrvt[port].icmp_flood_index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].icmp_flood_index = 0;
        }

        if(SYN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_syn_flood_classification_delete(atkPrvt[port].syn_flood_index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].syn_flood_index = 0;
        }

        if(FIN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_luna_g3_fin_flood_classification_delete(atkPrvt[port].fin_flood_index)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
            atkPrvt[port].fin_flood_index = 0;
        }
    }

    atkPrvt[port].enable = enable;

    return RT_ERR_OK;
} /* end of dal_luna_g3_sec_portAttackPreventState_set */

/* Function Name:
 *      dal_luna_g3_sec_attackPrevent_get
 * Description:
 *      Get action for each kind of attack on specified port.
 * Input:
 *      attackType - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action is as following:
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
dal_luna_g3_sec_attackPrevent_get(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            *pAction)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((ATTACK_TYPE_END <= attackType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(attackType)
    {
        case SYNFIN_DENY:
            *pAction = SYNFIN_DENY_ACTION;
            break;
        case ICMPFLOOD_DENY:
            *pAction = ICMP_FLOOD_ACTION;
            break;
        case SYNFLOOD_DENY:
            *pAction = SYN_FLOOD_ACTION;
            break;
        case FINFLOOD_DENY:
            *pAction = FIN_FLOOD_ACTION;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;   
            break;    
    }
    
    return RT_ERR_OK;
} /* end of dal_luna_g3_sec_attackPrevent_get */

/* Function Name:
 *      dal_luna_g3_sec_attackPrevent_set
 * Description:
 *      Set action for each kind of attack.
 * Input:
 *      attack_type - type of attack
 *      action      - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      Action is as following:
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
dal_luna_g3_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action)
{
    int32 ret=RT_ERR_OK;
    uint32  port;
    ca_uint32_t index;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((ATTACK_TYPE_END <= attackType), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= action), RT_ERR_INPUT);

    switch(attackType)
    {
        case SYNFIN_DENY:
            if((SYNFIN_DENY_ACTION == ACTION_FORWARD) && (action != ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_synfin_deny_classification_add(port,action,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].synfin_deny_index = index;
                    }
                }
            }
            else if((SYNFIN_DENY_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_synfin_deny_classification_delete(atkPrvt[port].synfin_deny_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].synfin_deny_index = 0;
                    }
                }
            }
            else if((SYNFIN_DENY_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (SYNFIN_DENY_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_synfin_deny_classification_delete(atkPrvt[port].synfin_deny_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].synfin_deny_index = 0;

                        if((ret = _dal_luna_g3_synfin_deny_classification_add(port,action,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].synfin_deny_index = index;
                    }
                }
            }

            SYNFIN_DENY_ACTION = action;
            break;
        case ICMPFLOOD_DENY:
            if(action == ACTION_TRAP2CPU)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return RT_ERR_INPUT;
            }
            if((ICMP_FLOOD_ACTION == ACTION_FORWARD) && (action != ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_icmp_flood_classification_add(port,action,ICMP_FLOOD_TH,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].icmp_flood_index = index;
                    }
                }
            }
            else if((ICMP_FLOOD_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_icmp_flood_classification_delete(atkPrvt[port].icmp_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].icmp_flood_index = 0;
                    }
                }
            }
            else if((ICMP_FLOOD_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (ICMP_FLOOD_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_icmp_flood_classification_delete(atkPrvt[port].icmp_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].icmp_flood_index = 0;

                        if((ret = _dal_luna_g3_icmp_flood_classification_add(port,action,ICMP_FLOOD_TH,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].icmp_flood_index = index;
                    }
                }
            }

            ICMP_FLOOD_ACTION = action;
            break;
        case SYNFLOOD_DENY:
            if(action == ACTION_TRAP2CPU)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return RT_ERR_INPUT;
            }
            if((SYN_FLOOD_ACTION == ACTION_FORWARD) && (action != ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_syn_flood_classification_add(port,action,SYN_FLOOD_TH,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].syn_flood_index = index;
                    }
                }
            }
            else if((SYN_FLOOD_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_syn_flood_classification_delete(atkPrvt[port].syn_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].syn_flood_index = 0;
                    }
                }
            }
            else if((SYN_FLOOD_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (SYN_FLOOD_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_syn_flood_classification_delete(atkPrvt[port].syn_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].syn_flood_index = 0;

                        if((ret = _dal_luna_g3_syn_flood_classification_add(port,action,SYN_FLOOD_TH,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].syn_flood_index = index;
                    }
                }
            }

            SYN_FLOOD_ACTION = action;
            break;
        case FINFLOOD_DENY:
            if(action == ACTION_TRAP2CPU)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return RT_ERR_INPUT;
            }
            if((FIN_FLOOD_ACTION == ACTION_FORWARD) && (action != ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_fin_flood_classification_add(port,action,FIN_FLOOD_TH,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].fin_flood_index = index;
                    }
                }
            }
            else if((FIN_FLOOD_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_fin_flood_classification_delete(atkPrvt[port].fin_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].fin_flood_index = 0;
                    }
                }
            }
            else if((FIN_FLOOD_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (FIN_FLOOD_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_fin_flood_classification_delete(atkPrvt[port].fin_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].fin_flood_index = 0;

                        if((ret = _dal_luna_g3_fin_flood_classification_add(port,action,FIN_FLOOD_TH,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].fin_flood_index = index;
                    }
                }
            }

            FIN_FLOOD_ACTION = action;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;    
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_sec_attackPrevent_set */

/* Function Name:
 *      dal_luna_g3_sec_attackFloodThresh_get
 * Description:
 *      Get flood threshold, rate unit 1 kbps.
 * Input:
 *      None
 * Output:
 *      pFloodThresh - pointer to flood threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
dal_luna_g3_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((SEC_FLOOD_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFloodThresh), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case SEC_ICMPFLOOD:
            *pFloodThresh = ICMP_FLOOD_TH;
            break;
        case SEC_SYNCFLOOD:
            *pFloodThresh = SYN_FLOOD_TH;
            break;
        case SEC_FINFLOOD:
            *pFloodThresh = FIN_FLOOD_TH;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    return RT_ERR_FAILED;
} /* end of dal_luna_g3_sec_attackFloodThresh_get */
      
/* Function Name:
 *      dal_luna_g3_sec_attackFloodThresh_set
 * Description:
 *      Set  flood threshold, rate unit 1 kbps.
 * Input:
 *      floodThresh - flood threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
dal_luna_g3_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh)
{
    int32 ret=RT_ERR_OK;
    uint32  port;
    ca_uint32_t index;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((SEC_FLOOD_END <= type), RT_ERR_INPUT);

    switch(type)
    {
        case SEC_ICMPFLOOD:
            if((ICMP_FLOOD_ACTION != ACTION_FORWARD) && (ICMP_FLOOD_TH != floodThresh))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_icmp_flood_classification_delete(atkPrvt[port].icmp_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].icmp_flood_index = 0;

                        if((ret = _dal_luna_g3_icmp_flood_classification_add(port,ICMP_FLOOD_ACTION,floodThresh,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].icmp_flood_index = index;
                    }
                }
            }

            ICMP_FLOOD_TH = floodThresh;
            break;
        case SEC_SYNCFLOOD:
            if((SYN_FLOOD_ACTION != ACTION_FORWARD) && (SYN_FLOOD_TH != floodThresh))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_syn_flood_classification_delete(atkPrvt[port].syn_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].syn_flood_index = 0;

                        if((ret = _dal_luna_g3_syn_flood_classification_add(port,SYN_FLOOD_ACTION,floodThresh,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].syn_flood_index = index;
                    }
                }
            }

            SYN_FLOOD_TH = floodThresh;
            break;
        case SEC_FINFLOOD:
            if((FIN_FLOOD_ACTION != ACTION_FORWARD) && (FIN_FLOOD_TH != floodThresh))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_luna_g3_fin_flood_classification_delete(atkPrvt[port].fin_flood_index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].fin_flood_index = 0;

                        if((ret = _dal_luna_g3_fin_flood_classification_add(port,FIN_FLOOD_ACTION,floodThresh,&index)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                        atkPrvt[port].fin_flood_index = index;
                    }
                }
            }

            FIN_FLOOD_TH = floodThresh;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_sec_attackFloodThresh_set */
