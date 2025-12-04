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
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_sec.h>
#include <osal/time.h>

#include <aal_port.h>
#include <aal_l3_ddos.h>
#include <aal_l2_ddos.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 sec_init = {INIT_NOT_COMPLETED}; 

/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      dal_rtl9607f_sec_init
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
dal_rtl9607f_sec_init(void)
{
    int32 ret;
    uint32  port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    sec_init = INIT_COMPLETED;

    /*disable attack prevent*/    
    HAL_SCAN_ALL_PORT(port)    
    {
        if ((ret = dal_rtl9607f_sec_portAttackPreventState_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            sec_init = INIT_NOT_COMPLETED;
            return ret;
        }    
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_init */

/* Function Name:
 *      dal_rtl9607f_sec_portAttackPreventState_get
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
dal_rtl9607f_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    aal_dos_fib_t dos_fib;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    memset(&dos_fib, 0, sizeof(aal_dos_fib_t));
    if(aal_dos_atk_get(0, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    *pEnable = ((dos_fib.dos_port_mask & (1 << port)) != 0)? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_portAttackPreventState_get */

/* Function Name:
 *      dal_rtl9607f_sec_portAttackPreventState_set
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
dal_rtl9607f_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable)
{
    aal_dos_fib_t dos_fib;
    aal_dos_fib_mask_t dos_fib_mask;
    aal_l2_dos_fib_t dos_fib_l2;
    aal_l2_dos_fib_mask_t dos_fib_mask_l2;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    memset(&dos_fib, 0, sizeof(aal_dos_fib_t));
    memset(&dos_fib_mask, 0, sizeof(aal_dos_fib_mask_t));

    if(aal_dos_atk_get(0, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    dos_fib_mask.s.dos_port_mask = 1;
    if(ENABLED == enable)
        dos_fib.dos_port_mask |= (1 << port);
    else
        dos_fib.dos_port_mask &= ~(1 << port);

    if(aal_dos_atk_set(0, dos_fib_mask, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    /*the same configuration as l3ddos */ 
    memset(&dos_fib_l2, 0, sizeof(aal_l2_dos_fib_t));
    memset(&dos_fib_mask_l2, 0, sizeof(aal_l2_dos_fib_mask_t));
    if(aal_l2_dos_atk_get(0, &dos_fib_l2) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    dos_fib_mask_l2.s.dos_port_mask = 1;
    if(ENABLED == enable)
        dos_fib_l2.dos_port_mask |= (1 << port);
    else
        dos_fib_l2.dos_port_mask &= ~(1 << port);

    if(aal_l2_dos_atk_set(0, dos_fib_mask_l2, &dos_fib_l2) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_portAttackPreventState_set */

/* Function Name:
 *      dal_rtl9607f_sec_attackPrevent_get
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
dal_rtl9607f_sec_attackPrevent_get(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            *pAction)
{
    ca_uint32_t dos_action = 0;
    aal_dos_fib_t dos_fib;
    aal_dos_type_id_t dos_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((ATTACK_TYPE_END <= attackType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    memset(&dos_fib, 0, sizeof(aal_dos_fib_t));
    if(aal_dos_atk_get(0, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    switch(attackType)
    {
        case  DAEQSA_DENY:
            dos_action = dos_fib.smac_eq_dmac;
            dos_type = AAL_DOS_ID_TYPE_SMAC_EQ_DMAC;
            break;
        case  LAND_DENY:
            dos_action = dos_fib.land_attacks;
            dos_type = AAL_DOS_ID_TYPE_LAND_ATTACKS;
            break;
        case  BLAT_DENY:
            dos_action = dos_fib.blat_attacks;
            dos_type = AAL_DOS_ID_TYPE_BLAT_ATTACKS;
            break;
        case  SYNFIN_DENY:
            dos_action = dos_fib.syn_fin_scan;
            dos_type = AAL_DOS_ID_TYPE_SYN_FIN_SCAN;
            break;
        case  XMA_DENY:
            dos_action = dos_fib.xma_scan;
            dos_type = AAL_DOS_ID_TYPE_XMA_SCAN;
            break;
        case  NULLSCAN_DENY:
            dos_action = dos_fib.null_scan;
            dos_type = AAL_DOS_ID_TYPE_NULL_SCAN;
            break;
        case  SYN_SPORTL1024_DENY:
            dos_action = dos_fib.syn1024;
            dos_type = AAL_DOS_ID_TYPE_SYN1024;
            break;
        case  TCPHDR_MIN_CHECK:
            dos_action = dos_fib.tcp_short_hdr;
            dos_type = AAL_DOS_ID_TYPE_TCP_SHORT_HDR;
            break;
        case  TCP_FRAG_OFF_MIN_CHECK:
            dos_action = dos_fib.tcp_frag_error;
            dos_type = AAL_DOS_ID_TYPE_TCP_FRAG_ERROR;
            break;
        case  ICMP_FRAG_PKTS_DENY:
            dos_action = dos_fib.icmp_fragment;
            dos_type = AAL_DOS_ID_TYPE_ICMP_FRAGMENT;
            break;
        case  POD_DENY:
            dos_action = dos_fib.ping_of_death;
            dos_type = AAL_DOS_ID_TYPE_PING_OF_DEATH;
            break;

        case  UDPDOMB_DENY:
            dos_action = dos_fib.udp_bomb;
            dos_type = AAL_DOS_ID_TYPE_UDP_BOMB;
            break;

        case  SYNWITHDATA_DENY:
            dos_action = dos_fib.syn_with_data;
            dos_type = AAL_DOS_ID_TYPE_SYN_WITH_DATA;
            break;
        case  SYNFLOOD_DENY:
            dos_action = dos_fib.syn_flood;
            dos_type = AAL_DOS_ID_TYPE_SYN_FLOOD;
            break;
        case  FINFLOOD_DENY:
            dos_action = dos_fib.fin_flood;
            dos_type = AAL_DOS_ID_TYPE_FIN_FLOOD;
            break;
        case  ICMPFLOOD_DENY:
            dos_action = dos_fib.icmp_flood;
            dos_type = AAL_DOS_ID_TYPE_ICMP_FLOOD;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;   
            break;    
    }

    if(dos_fib.dos_type_en & (1 << dos_type))
        *pAction = dos_action? ACTION_TRAP2CPU : ACTION_DROP;
    else
        *pAction = ACTION_FORWARD;
	
    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_attackPrevent_get */

/* Function Name:
 *      dal_rtl9607f_sec_attackPrevent_set
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
dal_rtl9607f_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action)
{
    aal_dos_fib_t dos_fib;
    aal_dos_type_id_t dos_type;
    aal_dos_fib_mask_t dos_fib_mask;

    aal_l2_dos_fib_t dos_fib_l2;
    aal_l2_dos_type_id_t dos_type_l2;
    aal_l2_dos_fib_mask_t dos_fib_mask_l2;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((ATTACK_TYPE_END <= attackType), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= action), RT_ERR_INPUT);

    switch(action)
    {
        case ACTION_TRAP2CPU:
            // No reserved policer for DDOS, not supported so far. If TRAP is required, need to coodinate with FC.
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
        case ACTION_DROP:
            break;
        case ACTION_FORWARD:
            break;
        default:
            return RT_ERR_INPUT;
            break;   
    }

    memset(&dos_fib, 0, sizeof(aal_dos_fib_t));
    memset(&dos_fib_mask, 0, sizeof(aal_dos_fib_mask_t));
    if(aal_dos_atk_get(0, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    memset(&dos_fib_l2, 0, sizeof(aal_l2_dos_fib_t));
    memset(&dos_fib_mask_l2, 0, sizeof(aal_l2_dos_fib_mask_t));
    if(aal_l2_dos_atk_get(0, &dos_fib_l2) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    /*the same configuration as l3ddos */ 
    switch(attackType)
    {
        case  DAEQSA_DENY:
            dos_type = AAL_DOS_ID_TYPE_SMAC_EQ_DMAC;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.smac_eq_dmac = 1;
                //dos_fib.smac_eq_dmac = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.smac_eq_dmac = 1;
                //dos_fib_l2.smac_eq_dmac = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  LAND_DENY:
            dos_type = AAL_DOS_ID_TYPE_LAND_ATTACKS;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.land_attacks = 1;
                //dos_fib.land_attacks = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.land_attacks = 1;
                //dos_fib_l2.land_attacks = (action == ACTION_TRAP2CPU)? 1 : 0;

            }
            break;
        case  BLAT_DENY:
            dos_type = AAL_DOS_ID_TYPE_BLAT_ATTACKS;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.blat_attacks = 1;
                //dos_fib.blat_attacks = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.blat_attacks = 1;
                //dos_fib_l2.blat_attacks = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  SYNFIN_DENY:
            dos_type = AAL_DOS_ID_TYPE_SYN_FIN_SCAN;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.syn_fin_scan = 1;
                //dos_fib.syn_fin_scan = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.syn_fin_scan = 1;
                //dos_fib_l2.syn_fin_scan = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  XMA_DENY:
            dos_type = AAL_DOS_ID_TYPE_XMA_SCAN;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.xma_scan = 1;
                //dos_fib.xma_scan = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.xma_scan = 1;
                //dos_fib_l2.xma_scan = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  NULLSCAN_DENY:
            dos_type = AAL_DOS_ID_TYPE_NULL_SCAN;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.null_scan = 1;
                //dos_fib.null_scan = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.null_scan = 1;
                //dos_fib_l2.null_scan = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  SYN_SPORTL1024_DENY:
            dos_type = AAL_DOS_ID_TYPE_SYN1024;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.syn1024 = 1;
                //dos_fib.syn1024 = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.syn1024 = 1;
                //dos_fib_l2.syn1024 = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  TCPHDR_MIN_CHECK:
            dos_type = AAL_DOS_ID_TYPE_TCP_SHORT_HDR;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.tcp_short_hdr = 1;
                //dos_fib.tcp_short_hdr = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.tcp_short_hdr = 1;
                //dos_fib_l2.tcp_short_hdr = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  TCP_FRAG_OFF_MIN_CHECK:
            dos_type = AAL_DOS_ID_TYPE_TCP_FRAG_ERROR;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.tcp_frag_error = 1;
                //dos_fib.tcp_frag_error = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.tcp_frag_error = 1;
                //dos_fib_l2.tcp_frag_error = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  ICMP_FRAG_PKTS_DENY:
            dos_type = AAL_DOS_ID_TYPE_ICMP_FRAGMENT;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.icmp_fragment = 1;
                //dos_fib.icmp_fragment = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.icmp_fragment = 1;
                //dos_fib_l2.icmp_fragment = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  POD_DENY:
            dos_type = AAL_DOS_ID_TYPE_PING_OF_DEATH;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.ping_of_death = 1;
                //dos_fib.ping_of_death = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.ping_of_death = 1;
                //dos_fib_l2.ping_of_death = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  UDPDOMB_DENY:
            dos_type = AAL_DOS_ID_TYPE_UDP_BOMB;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.udp_bomb = 1;
                //dos_fib.udp_bomb = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.udp_bomb = 1;
                //dos_fib_l2.udp_bomb = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;

        case  SYNWITHDATA_DENY:
            dos_type = AAL_DOS_ID_TYPE_SYN_WITH_DATA;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.syn_with_data = 1;
                //dos_fib.syn_with_data = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.syn_with_data = 1;
                //dos_fib_l2.syn_with_data = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  SYNFLOOD_DENY:
            dos_type = AAL_DOS_ID_TYPE_SYN_FLOOD;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.syn_flood = 1;
                //dos_fib.syn_flood = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.syn_flood = 1;
                //dos_fib_l2.syn_flood = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  FINFLOOD_DENY:
            dos_type = AAL_DOS_ID_TYPE_FIN_FLOOD;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.fin_flood = 1;
                //dos_fib.fin_flood = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.fin_flood = 1;
                //dos_fib_l2.fin_flood = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;
        case  ICMPFLOOD_DENY:
            dos_type = AAL_DOS_ID_TYPE_ICMP_FLOOD;
            if(action != ACTION_FORWARD)
            {
                dos_fib_mask.s.icmp_flood = 1;
                //dos_fib.icmp_flood = (action == ACTION_TRAP2CPU)? 1 : 0;
                dos_fib_mask_l2.s.icmp_flood = 1;
                //dos_fib_l2.icmp_flood = (action == ACTION_TRAP2CPU)? 1 : 0;
            }
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;   
            break;    
    }

    dos_fib_mask.s.dos_type_en = 1;
    if(action == ACTION_FORWARD)
        dos_fib.dos_type_en &= ~(1 << dos_type);
    else
        dos_fib.dos_type_en |= (1 << dos_type);

    if(aal_dos_atk_set(0, dos_fib_mask, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }


    
    dos_fib_mask_l2.s.dos_type_en = 1;
    if(action == ACTION_FORWARD)
        dos_fib_l2.dos_type_en &= ~(1 << dos_type);
    else
        dos_fib_l2.dos_type_en |= (1 << dos_type);

    if(aal_l2_dos_atk_set(0, dos_fib_mask_l2, &dos_fib_l2) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_attackPrevent_set */

/* Function Name:
 *      dal_rtl9607f_sec_attackFloodThresh_get
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
dal_rtl9607f_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh)
{
    aal_dos_fib_t dos_fib;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((SEC_FLOOD_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFloodThresh), RT_ERR_NULL_POINTER);

    memset(&dos_fib, 0, sizeof(aal_dos_fib_t));
    if(aal_dos_atk_get(0, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    switch(type)
    {
        case SEC_ICMPFLOOD:
            *pFloodThresh = dos_fib.icmp_flood_th;
            break;
        case SEC_SYNCFLOOD:
            *pFloodThresh = dos_fib.syn_flood_th;
            break;
        case SEC_FINFLOOD:
            *pFloodThresh = dos_fib.fin_flood_th;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_attackFloodThresh_get */
      
/* Function Name:
 *      dal_rtl9607f_sec_attackFloodThresh_set
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
dal_rtl9607f_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh)
{
    aal_dos_fib_t dos_fib;
    aal_dos_fib_mask_t dos_fib_mask;
    aal_l2_dos_fib_t dos_fib_l2;
    aal_l2_dos_fib_mask_t dos_fib_mask_l2;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((SEC_FLOOD_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607F_SEC_FLOOD_THRESHOLD_MAX < floodThresh), RT_ERR_INPUT);

    memset(&dos_fib_mask, 0, sizeof(aal_dos_fib_mask_t));
    memset(&dos_fib, 0, sizeof(aal_dos_fib_t));
    memset(&dos_fib_mask_l2, 0, sizeof(aal_l2_dos_fib_mask_t));
    memset(&dos_fib_l2, 0, sizeof(aal_l2_dos_fib_t));

    switch(type)
    {
        case SEC_ICMPFLOOD:
            dos_fib_mask.s.icmp_flood_th = 1;
            dos_fib.icmp_flood_th = floodThresh;
            dos_fib_mask_l2.s.icmp_flood_th = 1;
            dos_fib_l2.icmp_flood_th = floodThresh;
            break;
        case SEC_SYNCFLOOD:
            dos_fib_mask.s.syn_flood_th = 1;
            dos_fib.syn_flood_th = floodThresh;
            dos_fib_mask_l2.s.syn_flood_th = 1;
            dos_fib_l2.syn_flood_th = floodThresh;
            break;
        case SEC_FINFLOOD:
            dos_fib_mask.s.fin_flood_th = 1;
            dos_fib.fin_flood_th = floodThresh;
            dos_fib_mask_l2.s.fin_flood_th = 1;
            dos_fib_l2.fin_flood_th = floodThresh;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if(aal_dos_atk_set(0, dos_fib_mask, &dos_fib) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    if(aal_l2_dos_atk_set(0, dos_fib_mask_l2, &dos_fib_l2) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_attackFloodThresh_set */

/* Function Name:
 *      dal_rtl9607f_sec_attackFloodThreshUnit_get
 * Description:
 *      Get time unit of flood threshold, 1/2/4/8/16/32/64/128/512 ms.
 * Input:
 *      None
 * Output:
 *      pFloodThreshUnit - pointer to flood threshold time unit
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
dal_rtl9607f_sec_attackFloodThreshUnit_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThreshUnit)
{
    aal_dos_atk_flood_cnt_t flood_cnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((SEC_FLOOD_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFloodThreshUnit), RT_ERR_NULL_POINTER);

    memset(&flood_cnt, 0, sizeof(aal_dos_atk_flood_cnt_t));
    if(aal_dos_atk_flood_cnt_get(0, &flood_cnt) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }
    if(flood_cnt.cnt == DOS_ATK_FLOOD_UNIT_100MS)
        *pFloodThreshUnit = 100;
    else        
        *pFloodThreshUnit = (flood_cnt.cnt / DOS_ATK_FLOOD_UNIT_1MS);
        

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_attackFloodThreshUnit_get */

/* Function Name:
 *      dal_rtl9607f_sec_attackFloodThreshUnit_set
 * Description:
 *      Set time unit of flood threshold, 1/2/4/8/16/32/64/128/512 ms.
 * Input:
 *      floodThresh - flood threshold time unit
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
dal_rtl9607f_sec_attackFloodThreshUnit_set(rtk_sec_attackFloodType_t type, uint32 floodThreshUnit)
{

    aal_dos_atk_flood_cnt_mask_t flood_cnt_mask;
    aal_dos_atk_flood_cnt_t flood_cnt;
    aal_l2_dos_atk_flood_cnt_mask_t flood_cnt_mask_l2;
    aal_l2_dos_atk_flood_cnt_t flood_cnt_l2;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((SEC_FLOOD_END <= type), RT_ERR_INPUT);

    memset(&flood_cnt_mask, 0, sizeof(aal_dos_atk_flood_cnt_mask_t));
    memset(&flood_cnt, 0, sizeof(aal_dos_atk_flood_cnt_t));

    memset(&flood_cnt_mask_l2, 0, sizeof(aal_l2_dos_atk_flood_cnt_mask_t));
    memset(&flood_cnt_l2, 0, sizeof(aal_l2_dos_atk_flood_cnt_t));

    switch(floodThreshUnit)
    {
        case 1:
            flood_cnt.cnt= floodThreshUnit * DOS_ATK_FLOOD_UNIT_1MS;
            flood_cnt_l2.cnt= 2;
            break;
        case 2:
            flood_cnt.cnt= floodThreshUnit * DOS_ATK_FLOOD_UNIT_1MS;
            flood_cnt_l2.cnt= 5;
            break;
        case 3:
            flood_cnt.cnt= floodThreshUnit * DOS_ATK_FLOOD_UNIT_1MS;
            flood_cnt_l2.cnt= 7;
            break;
        case 4:
            flood_cnt.cnt= floodThreshUnit * DOS_ATK_FLOOD_UNIT_1MS;
            flood_cnt_l2.cnt= 10;
            break;
        case 5:
            flood_cnt.cnt= floodThreshUnit * DOS_ATK_FLOOD_UNIT_1MS;
            flood_cnt_l2.cnt= 12;
            break;
        case 6:
            flood_cnt.cnt= floodThreshUnit * DOS_ATK_FLOOD_UNIT_1MS;
            flood_cnt_l2.cnt= 15;
            break;
        case 100:
            flood_cnt.cnt= DOS_ATK_FLOOD_UNIT_100MS;
            flood_cnt_l2.cnt= DOS_ATK_L2_FLOOD_UNIT_100MS;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;            
    }

    /*time unit is 100ms for 77C*/
    flood_cnt_mask.s.cnt = 1;
     
    if(aal_dos_atk_flood_cnt_set(0, flood_cnt_mask, &flood_cnt) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }


    /*time unit is 100ms for 77C*/
    flood_cnt_mask_l2.s.cnt = 1;
    
    if(aal_l2_dos_atk_flood_cnt_set(0, flood_cnt_mask_l2, &flood_cnt_l2) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_sec_attackFloodThreshUnit_set */
