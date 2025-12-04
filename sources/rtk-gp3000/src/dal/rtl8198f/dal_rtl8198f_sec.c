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
#include <dal/rtl8198f/dal_rtl8198f.h>
#include <dal/rtl8198f/dal_rtl8198f_sec.h>
#include <osal/time.h>

#include <cortina-api/include/classifier.h>
#include <cortina-api/include/rate.h>

#include <aal/include/aal_port.h>

/*
 * Symbol Definition
 */
#define __CLS_TCP_FLAGS_BIT_SYN                 (1<<1)
#define __CLS_TCP_FLAGS_BIT_FIN                 (1<<0)

/*use block 224~255*/
#define SEC_ICMP_FLOW_ID_BASE    224
#define SEC_SYN_FLOW_ID_BASE     231
#define SEC_FIN_FLOW_ID_BASE     238

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

int32 _dal_rtl8198f_synfin_deny_classification_set(rtk_port_t port,rtk_action_t rtk_action,ca_uint32_t *pIndex,ca_uint8_t flag)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t priority=synfin_prio;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t get_priority;
    ca_classifier_key_t get_key;
    ca_classifier_key_mask_t get_key_mask;
    ca_classifier_action_t get_action;

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

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;
    }

    if(flag == 1) //Adding
    {
        if((ret = ca_classifier_rule_add(0,synfin_prio,&key,&key_mask,&action,pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else //Deleting
    {
        memset(&get_key,0,sizeof(ca_classifier_key_t));
        memset(&get_key_mask,0,sizeof(ca_classifier_key_mask_t));
        memset(&get_action,0,sizeof(ca_classifier_action_t));

        ret = ca_classifier_rule_get(0,*pIndex,&get_priority,&get_key,&get_key_mask,&get_action);
        if(ret == CA_E_NOT_FOUND)
        {
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&priority,&get_priority,sizeof(ca_uint32_t)))
        {
            printk("%s %d priority not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key,&get_key,sizeof(ca_classifier_key_t)))
        {
            printk("%s %d key not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key_mask,&get_key_mask,sizeof(ca_classifier_key_mask_t)))
        {
            printk("%s %d key_mask not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&action,&get_action,sizeof(ca_classifier_action_t)))
        {
            printk("%s %d action not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if((ret = ca_classifier_rule_delete(0,*pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        *pIndex = -1;
    }

    return RT_ERR_OK;
}

int32 _dal_rtl8198f_icmp_flood_classification_set(rtk_port_t port,rtk_action_t rtk_action,uint32 floodThresh,ca_uint32_t *pIndex,ca_uint8_t flag)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_uint32_t priority=icmp_flood_prio;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t get_priority;
    ca_classifier_key_t get_key;
    ca_classifier_key_mask_t get_key_mask;
    ca_classifier_action_t get_action;
    ca_uint16_t flow_id;

    port_id = RTK2CA_PORT_ID(port);

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 1; /* True - PPS mode, false - BPS mode */
    policer.cir = floodThresh*1000;
    policer.cbs = 40;
    policer.pir = floodThresh*1000;
    policer.pbs = 40;

    if(port < 4)
        flow_id = SEC_ICMP_FLOW_ID_BASE + port;
    else if(HAL_IS_PON_PORT(port))
        flow_id = SEC_ICMP_FLOW_ID_BASE + 4;
    else
        flow_id = SEC_ICMP_FLOW_ID_BASE + 5;

    if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    key.ip.ip_version = 4; //IPv4
    key.ip.ip_protocol = 1; //ICMP
    key_mask.ip = 1;
    key_mask.ip_mask.ip_version = 1;
    key_mask.ip_mask.ip_protocol = 1;

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
    }

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if(flag == 1) //Adding
    {
        if((ret = ca_classifier_rule_add(0,priority,&key,&key_mask,&action,pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        memset(&get_key,0,sizeof(ca_classifier_key_t));
        memset(&get_key_mask,0,sizeof(ca_classifier_key_mask_t));
        memset(&get_action,0,sizeof(ca_classifier_action_t));

        ret = ca_classifier_rule_get(0,*pIndex,&get_priority,&get_key,&get_key_mask,&get_action);
        if(ret == CA_E_NOT_FOUND)
        {
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&priority,&get_priority,sizeof(ca_uint32_t)))
        {
            printk("%s %d priority not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key,&get_key,sizeof(ca_classifier_key_t)))
        {
            printk("%s %d key not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key_mask,&get_key_mask,sizeof(ca_classifier_key_mask_t)))
        {
            printk("%s %d key_mask not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&action,&get_action,sizeof(ca_classifier_action_t)))
        {
            printk("%s %d action not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if((ret = ca_classifier_rule_delete(0,*pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        *pIndex = -1;
    }

    return RT_ERR_OK;
}

int32 _dal_rtl8198f_syn_flood_classification_set(rtk_port_t port,rtk_action_t rtk_action,uint32 floodThresh,ca_uint32_t *pIndex,ca_uint8_t flag)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_uint32_t priority=syn_flood_prio;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t get_priority;
    ca_classifier_key_t get_key;
    ca_classifier_key_mask_t get_key_mask;
    ca_classifier_action_t get_action;
    ca_uint16_t flow_id;

    port_id = RTK2CA_PORT_ID(port);

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 1; /* True - PPS mode, false - BPS mode */
    policer.cir = floodThresh*1000;
    policer.cbs = 40;
    policer.pir = floodThresh*1000;
    policer.pbs = 40;

    if(port < 4)
        flow_id = SEC_SYN_FLOW_ID_BASE + port;
    else if(HAL_IS_PON_PORT(port))
        flow_id = SEC_SYN_FLOW_ID_BASE + 4;
    else
        flow_id = SEC_SYN_FLOW_ID_BASE + 5;

    if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
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
    key_mask.l4_mask.tcp_flags = __CLS_TCP_FLAGS_BIT_SYN ;
    
    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
    }

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if(flag == 1) //Adding
    {
        if((ret = ca_classifier_rule_add(0,priority,&key,&key_mask,&action,pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else //Deleting
    {
        memset(&get_key,0,sizeof(ca_classifier_key_t));
        memset(&get_key_mask,0,sizeof(ca_classifier_key_mask_t));
        memset(&get_action,0,sizeof(ca_classifier_action_t));

        ret = ca_classifier_rule_get(0,*pIndex,&get_priority,&get_key,&get_key_mask,&get_action);
        if(ret == CA_E_NOT_FOUND)
        {
            *pIndex = -1;
            return RT_ERR_OK;
        }
//printk("priority=%d\n",priority);
//printk("key.src_port=%d\n",key.src_port);
//printk("key_mask.src_port=%d\n",key_mask.src_port);
//printk("key.l4.l4_valid=%d\n",key.l4.l4_valid);
//printk("key.l4.tcp_flags=%d\n",key.l4.tcp_flags);
//printk("key_mask.l4=%d\n",key_mask.l4);
//printk("key_mask.l4_mask.tcp_flags=%d\n",key_mask.l4_mask.tcp_flags);
//
//if(HAL_IS_PON_PORT(port))
//{
//    printk("key.key_handle.flow_id=%d\n",key.key_handle.flow_id);
//    printk("key.key_handle.gem_index=%d\n",key.key_handle.gem_index);
//    printk("key.key_handle.llid_cos_index=%d\n",key.key_handle.llid_cos_index);
//    printk("key_mask.key_handle=%d\n",key_mask.key_handle);
//
//    printk("action.forward=%d\n",action.forward);
//    printk("action.dest.port=%d\n",action.dest.port);
//}
//else
//{
//    printk("action.forward=%d\n",action.forward);
//    printk("action.dest.port=%d\n",action.dest.port);
//}
//
//printk("action.options.action_handle.flow_id=%d\n",action.options.action_handle.flow_id);
//printk("action.options.masks.action_handle=%d\n",action.options.masks.action_handle);
//
//printk("get_priority=%d\n",get_priority);
//printk("get_key.src_port=%d\n",get_key.src_port);
//printk("get_key_mask.src_port=%d\n",get_key_mask.src_port);
//printk("get_key.l4.l4_valid=%d\n",get_key.l4.l4_valid);
//printk("get_key.l4.tcp_flags=%d\n",get_key.l4.tcp_flags);
//printk("get_key_mask.l4=%d\n",get_key_mask.l4);
//printk("get_key_mask.l4_mask.tcp_flags=%d\n",get_key_mask.l4_mask.tcp_flags);
//
//if(HAL_IS_PON_PORT(port))
//{
//    printk("get_key.key_handle.flow_id=%d\n",get_key.key_handle.flow_id);
//    printk("get_key.key_handle.gem_index=%d\n",get_key.key_handle.gem_index);
//    printk("get_key.key_handle.llid_cos_index=%d\n",get_key.key_handle.llid_cos_index);
//    printk("get_key_mask.key_handle=%d\n",get_key_mask.key_handle);
//
//    printk("get_action.forward=%d\n",get_action.forward);
//    printk("get_action.dest.port=%d\n",get_action.dest.port);
//}
//else
//{
//    printk("get_action.forward=%d\n",get_action.forward);
//    printk("get_action.dest.port=%d\n",get_action.dest.port);
//}
//
//printk("get_action.options.action_handle.flow_id=%d\n",get_action.options.action_handle.flow_id);
//printk("get_action.options.masks.action_handle=%d\n",get_action.options.masks.action_handle);

        if(memcmp(&priority,&get_priority,sizeof(ca_uint32_t)))
        {
            printk("%s %d priority not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key,&get_key,sizeof(ca_classifier_key_t)))
        {
            printk("%s %d key not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key_mask,&get_key_mask,sizeof(ca_classifier_key_mask_t)))
        {
            printk("%s %d key_mask not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&action,&get_action,sizeof(ca_classifier_action_t)))
        {
            printk("%s %d action not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if((ret = ca_classifier_rule_delete(0,*pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        *pIndex = -1;
    }

    return RT_ERR_OK;
}

int32 _dal_rtl8198f_fin_flood_classification_set(rtk_port_t port,rtk_action_t rtk_action,uint32 floodThresh,ca_uint32_t *pIndex,ca_uint8_t flag)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_uint32_t priority=fin_flood_prio;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t get_priority;
    ca_classifier_key_t get_key;
    ca_classifier_key_mask_t get_key_mask;
    ca_classifier_action_t get_action;
    ca_uint16_t flow_id;

    port_id = RTK2CA_PORT_ID(port);

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 1; /* True - PPS mode, false - BPS mode */
    policer.cir = floodThresh*1000;
    policer.cbs = 40;
    policer.pir = floodThresh*1000;
    policer.pbs = 40;

    if(port < 4)
        flow_id = SEC_FIN_FLOW_ID_BASE + port;
    else if(HAL_IS_PON_PORT(port))
        flow_id = SEC_FIN_FLOW_ID_BASE + 4;
    else
        flow_id = SEC_FIN_FLOW_ID_BASE + 5;

    if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
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
    
    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
    }

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if(flag == 1) //Adding
    {
        if((ret = ca_classifier_rule_add(0,priority,&key,&key_mask,&action,pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else //Deleting
    {
        memset(&get_key,0,sizeof(ca_classifier_key_t));
        memset(&get_key_mask,0,sizeof(ca_classifier_key_mask_t));
        memset(&get_action,0,sizeof(ca_classifier_action_t));

        ret = ca_classifier_rule_get(0,*pIndex,&get_priority,&get_key,&get_key_mask,&get_action);
        if(ret == CA_E_NOT_FOUND)
        {
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&priority,&get_priority,sizeof(ca_uint32_t)))
        {
            printk("%s %d priority not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key,&get_key,sizeof(ca_classifier_key_t)))
        {
            printk("%s %d key not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&key_mask,&get_key_mask,sizeof(ca_classifier_key_mask_t)))
        {
            printk("%s %d key_mask not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if(memcmp(&action,&get_action,sizeof(ca_classifier_action_t)))
        {
            printk("%s %d action not match!!\n",__FUNCTION__,__LINE__);
            *pIndex = -1;
            return RT_ERR_OK;
        }

        if((ret = ca_classifier_rule_delete(0,*pIndex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        *pIndex = -1;
    }

    return RT_ERR_OK;
}

/* Module Name : Security */

/* Function Name:
 *      dal_rtl8198f_sec_init
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
dal_rtl8198f_sec_init(void)
{
    uint32  port;

    sec_init = INIT_COMPLETED;

    memset(atkPrvt,0,sizeof(sec_atkPrvt_cfg_t)*RTK_MAX_NUM_OF_PORTS);

    /*disable attack prevent*/    
    HAL_SCAN_ALL_PORT(port)
    {
        atkPrvt[port].enable = DISABLED;
        atkPrvt[port].synfin_deny_index = -1;
        atkPrvt[port].icmp_flood_index = -1;
        atkPrvt[port].syn_flood_index = -1;
        atkPrvt[port].fin_flood_index = -1;
    }

    SYNFIN_DENY_ACTION = ACTION_FORWARD;
    ICMP_FLOOD_ACTION = ACTION_FORWARD;
    SYN_FLOOD_ACTION = ACTION_FORWARD;
    FIN_FLOOD_ACTION = ACTION_FORWARD;

    ICMP_FLOOD_TH = 0;
    SYN_FLOOD_TH = 0;
    FIN_FLOOD_TH = 0;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_sec_init */

/* Function Name:
 *      dal_rtl8198f_sec_portAttackPreventState_get
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
dal_rtl8198f_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = atkPrvt[port].enable;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_sec_portAttackPreventState_get */

/* Function Name:
 *      dal_rtl8198f_sec_portAttackPreventState_set
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
dal_rtl8198f_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret=RT_ERR_OK;
    
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
            if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,SYNFIN_DENY_ACTION,&atkPrvt[port].synfin_deny_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }

            if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,SYNFIN_DENY_ACTION,&atkPrvt[port].synfin_deny_index,1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }

        if(ICMP_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,ICMP_FLOOD_ACTION,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }

            if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,ICMP_FLOOD_ACTION,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }

        if(SYN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_syn_flood_classification_set(port,SYN_FLOOD_ACTION,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }

            if((ret = _dal_rtl8198f_syn_flood_classification_set(port,SYN_FLOOD_ACTION,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }

        if(FIN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_fin_flood_classification_set(port,FIN_FLOOD_ACTION,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }

            if((ret = _dal_rtl8198f_fin_flood_classification_set(port,FIN_FLOOD_ACTION,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }
    }
    else if((atkPrvt[port].enable == ENABLED)&&(enable == DISABLED)) //ENABLE -> DISABLE
    {
        if(SYNFIN_DENY_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,SYNFIN_DENY_ACTION,&atkPrvt[port].synfin_deny_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }

        if(ICMP_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,ICMP_FLOOD_ACTION,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }

        if(SYN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_syn_flood_classification_set(port,SYN_FLOOD_ACTION,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }

        if(FIN_FLOOD_ACTION != ACTION_FORWARD)
        {
            if((ret = _dal_rtl8198f_fin_flood_classification_set(port,FIN_FLOOD_ACTION,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                return ret;
            }
        }
    }

    atkPrvt[port].enable = enable;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_sec_portAttackPreventState_set */

/* Function Name:
 *      dal_rtl8198f_sec_attackPrevent_get
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
dal_rtl8198f_sec_attackPrevent_get(
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
} /* end of dal_rtl8198f_sec_attackPrevent_get */

/* Function Name:
 *      dal_rtl8198f_sec_attackPrevent_set
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
dal_rtl8198f_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action)
{
    int32 ret=RT_ERR_OK;
    uint32  port;

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
                        if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,action,&atkPrvt[port].synfin_deny_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,action,&atkPrvt[port].synfin_deny_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((SYNFIN_DENY_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,action,&atkPrvt[port].synfin_deny_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((SYNFIN_DENY_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (SYNFIN_DENY_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,action,&atkPrvt[port].synfin_deny_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_synfin_deny_classification_set(port,action,&atkPrvt[port].synfin_deny_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,action,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,action,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((ICMP_FLOOD_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,action,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((ICMP_FLOOD_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (ICMP_FLOOD_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,action,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,action,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,action,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,action,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((SYN_FLOOD_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,action,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((SYN_FLOOD_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (SYN_FLOOD_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,action,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,action,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,action,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
            
                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,action,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((FIN_FLOOD_ACTION != ACTION_FORWARD) && (action == ACTION_FORWARD))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,action,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
                    }
                }
            }
            else if((FIN_FLOOD_ACTION != ACTION_FORWARD) && (action != ACTION_FORWARD) && (FIN_FLOOD_ACTION != action))
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    if(atkPrvt[port].enable == ENABLED)
                    {
                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,action,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,action,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
} /* end of dal_rtl8198f_sec_attackPrevent_set */

/* Function Name:
 *      dal_rtl8198f_sec_attackFloodThresh_get
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
dal_rtl8198f_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh)
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

    return RT_ERR_OK;
} /* end of dal_rtl8198f_sec_attackFloodThresh_get */
      
/* Function Name:
 *      dal_rtl8198f_sec_attackFloodThresh_set
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
dal_rtl8198f_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh)
{
    int32 ret=RT_ERR_OK;
    uint32  port;

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
                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,ICMP_FLOOD_ACTION,ICMP_FLOOD_TH,&atkPrvt[port].icmp_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_icmp_flood_classification_set(port,ICMP_FLOOD_ACTION,floodThresh,&atkPrvt[port].icmp_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,SYN_FLOOD_ACTION,SYN_FLOOD_TH,&atkPrvt[port].syn_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_syn_flood_classification_set(port,SYN_FLOOD_ACTION,floodThresh,&atkPrvt[port].syn_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,FIN_FLOOD_ACTION,FIN_FLOOD_TH,&atkPrvt[port].fin_flood_index,0)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }

                        if((ret = _dal_rtl8198f_fin_flood_classification_set(port,FIN_FLOOD_ACTION,floodThresh,&atkPrvt[port].fin_flood_index,1)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
                            return ret;
                        }
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
} /* end of dal_rtl8198f_sec_attackFloodThresh_set */
