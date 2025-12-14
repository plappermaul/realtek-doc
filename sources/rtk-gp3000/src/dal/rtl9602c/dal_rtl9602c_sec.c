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
#include <common/rt_error.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/sec.h>
#include <dal/rtl9602c/dal_rtl9602c_sec.h>
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32    sec_init = {INIT_NOT_COMPLETED}; 

/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      dal_rtl9602c_sec_init
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
dal_rtl9602c_sec_init(void)
{
    int32   ret;
    uint32  port;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    sec_init = INIT_COMPLETED;

    /*disable attack prevent*/    
    HAL_SCAN_ALL_PORT(port)    
    {
        if ((ret = dal_rtl9602c_sec_portAttackPreventState_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            sec_init = INIT_NOT_COMPLETED;
            return ret;
        }    
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_init */

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */


/* Function Name:
 *      dal_rtl9602c_sec_portAttackPreventState_get
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
dal_rtl9602c_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);
    
    
    if ((ret = reg_array_field_read(RTL9602C_DOS_ENr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    *pEnable = (rtk_enable_t)value;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_portAttackPreventState_get */

/* Function Name:
 *      dal_rtl9602c_sec_portAttackPreventState_set
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
dal_rtl9602c_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    value = (uint32)enable;
    if ((ret = reg_array_field_write(RTL9602C_DOS_ENr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_portAttackPreventState_set */



/* Function Name:
 *      dal_rtl9602c_sec_attackPrevent_get
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
dal_rtl9602c_sec_attackPrevent_get(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            *pAction)
{
    uint32  value;
    uint32  field,actField;    
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(attackType)
    {
		case DAEQSA_DENY:
			field    = RTL9602C_DOS_DAEQSAf;
			actField = RTL9602C_DOS_DAEQSA_ACTf;
			break;
		case LAND_DENY:
			field = RTL9602C_DOS_LANDATTACKSf;
			actField = RTL9602C_DOS_LANDATTACKS_ACTf;
			break;
		case BLAT_DENY:
			field = RTL9602C_DOS_BLATATTACKSf;
			actField = RTL9602C_DOS_BLATATTACKS_ACTf;
			break;
		case SYNFIN_DENY:
			field = RTL9602C_DOS_SYNFINSCANf;
			actField = RTL9602C_DOS_SYNFINSCAN_ACTf;
			break;
		case XMA_DENY:
			field = RTL9602C_DOS_XMASCANf;
			actField = RTL9602C_DOS_XMASCAN_ACTf;
			break;
		case NULLSCAN_DENY:
			field = RTL9602C_DOS_NULLSCANf;
			actField = RTL9602C_DOS_NULLSCAN_ACTf;
			break;
		case SYN_SPORTL1024_DENY:
			field = RTL9602C_DOS_SYN1024f;
			actField = RTL9602C_DOS_SYN1024_ACTf;
			break;
		case TCPHDR_MIN_CHECK:
			field = RTL9602C_DOS_TCPSHORTHDRf;
			actField = RTL9602C_DOS_TCPSHORTHDR_ACTf;
			break;
		case TCP_FRAG_OFF_MIN_CHECK:
			field = RTL9602C_DOS_TCPFRAGERRORf;
			actField = RTL9602C_DOS_TCPFRAGERROR_ACTf;
			break;
		case ICMP_FRAG_PKTS_DENY:
			field = RTL9602C_DOS_ICMPFRAGMENTf;
			actField = RTL9602C_DOS_ICMPFRAGMENT_ACTf;
			break;
		case POD_DENY:
			field = RTL9602C_DOS_PINGOFDEATHf;
			actField = RTL9602C_DOS_PINGOFDEATH_ACTf;
			break;
		case UDPDOMB_DENY:
			field = RTL9602C_DOS_UDPBOMBf;
			actField = RTL9602C_DOS_UDPBOMB_ACTf;
			break;
		case SYNWITHDATA_DENY:
			field = RTL9602C_DOS_SYNWITHDATAf;
			actField = RTL9602C_DOS_SYNWITHDATA_ACTf;
			break;
		case SYNFLOOD_DENY:
			field = RTL9602C_DOS_SYNFLOODf;
			actField = RTL9602C_DOS_SYNFLOOD_ACTf;
			break;
		case FINFLOOD_DENY:
			field = RTL9602C_DOS_FINFLOODf;
			actField = RTL9602C_DOS_FINFLOOD_ACTf;
			break;
		case ICMPFLOOD_DENY:
			field = RTL9602C_DOS_ICMPFLOODf;
			actField = RTL9602C_DOS_ICMPFLOOD_ACTf;
			break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;	
            break;    
    }

    if ((ret = reg_field_read(RTL9602C_DOS_CFGr,field, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SEC | MOD_DAL), "");
        return ret;
    }
    
    if(0 == value)
    {
        *pAction = ACTION_FORWARD;
        return  RT_ERR_OK;   
    }

    /*get action type*/
    if ((ret = reg_field_read(RTL9602C_DOS_CFGr,actField, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SEC | MOD_DAL), "");
        return ret;
    }
    
	switch(value)
	{
	 	case DAL_RTL9602C_DOS_ACT_DROP:
	 		*pAction = ACTION_DROP;
			break;
	 	case DAL_RTL9602C_DOS_ACT_TRAP:
	 		*pAction = ACTION_TRAP2CPU;
			break;

		default:
            return RT_ERR_FAILED;
	}       

    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_attackPrevent_get */

/* Function Name:
 *      dal_rtl9602c_sec_attackPrevent_set
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
dal_rtl9602c_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action)
{
    uint32  value,actVal;
    uint32  field,actField;  
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    switch(action)
    {
		case ACTION_TRAP2CPU:
		    actVal = DAL_RTL9602C_DOS_ACT_TRAP;
		    break;
		case ACTION_DROP:
		    actVal = DAL_RTL9602C_DOS_ACT_DROP;
		    break;
		case ACTION_FORWARD:
            break;
         default:
            return RT_ERR_INPUT;
            break;   
    }
    
    switch(attackType)
    {
		case DAEQSA_DENY:
			field    = RTL9602C_DOS_DAEQSAf;
			actField = RTL9602C_DOS_DAEQSA_ACTf;
			break;
		case LAND_DENY:
			field = RTL9602C_DOS_LANDATTACKSf;
			actField = RTL9602C_DOS_LANDATTACKS_ACTf;
			break;
		case BLAT_DENY:
			field = RTL9602C_DOS_BLATATTACKSf;
			actField = RTL9602C_DOS_BLATATTACKS_ACTf;
			break;
		case SYNFIN_DENY:
			field = RTL9602C_DOS_SYNFINSCANf;
			actField = RTL9602C_DOS_SYNFINSCAN_ACTf;
			break;
		case XMA_DENY:
			field = RTL9602C_DOS_XMASCANf;
			actField = RTL9602C_DOS_XMASCAN_ACTf;
			break;
		case NULLSCAN_DENY:
			field = RTL9602C_DOS_NULLSCANf;
			actField = RTL9602C_DOS_NULLSCAN_ACTf;
			break;
		case SYN_SPORTL1024_DENY:
			field = RTL9602C_DOS_SYN1024f;
			actField = RTL9602C_DOS_SYN1024_ACTf;
			break;
		case TCPHDR_MIN_CHECK:
			field = RTL9602C_DOS_TCPSHORTHDRf;
			actField = RTL9602C_DOS_TCPSHORTHDR_ACTf;
			break;
		case TCP_FRAG_OFF_MIN_CHECK:
			field = RTL9602C_DOS_TCPFRAGERRORf;
			actField = RTL9602C_DOS_TCPFRAGERROR_ACTf;
			break;
		case ICMP_FRAG_PKTS_DENY:
			field = RTL9602C_DOS_ICMPFRAGMENTf;
			actField = RTL9602C_DOS_ICMPFRAGMENT_ACTf;
			break;
		case POD_DENY:
			field = RTL9602C_DOS_PINGOFDEATHf;
			actField = RTL9602C_DOS_PINGOFDEATH_ACTf;
			break;
		case UDPDOMB_DENY:
			field = RTL9602C_DOS_UDPBOMBf;
			actField = RTL9602C_DOS_UDPBOMB_ACTf;
			break;
		case SYNWITHDATA_DENY:
			field = RTL9602C_DOS_SYNWITHDATAf;
			actField = RTL9602C_DOS_SYNWITHDATA_ACTf;
			break;
		case SYNFLOOD_DENY:
			field = RTL9602C_DOS_SYNFLOODf;
			actField = RTL9602C_DOS_SYNFLOOD_ACTf;
			break;
		case FINFLOOD_DENY:
			field = RTL9602C_DOS_FINFLOODf;
			actField = RTL9602C_DOS_FINFLOOD_ACTf;
			break;
		case ICMPFLOOD_DENY:
			field = RTL9602C_DOS_ICMPFLOODf;
			actField = RTL9602C_DOS_ICMPFLOOD_ACTf;
			break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;	
            break;    
    }
    
    if(ACTION_FORWARD == action)
    {
        value = 0;
        if ((ret = reg_field_write(RTL9602C_DOS_CFGr, field, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SEC | MOD_DAL), "");
            return ret;
        }    
        return RT_ERR_OK;
    }
    else
    {
        value = 1;
        if ((ret = reg_field_write(RTL9602C_DOS_CFGr, field, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SEC | MOD_DAL), "");
            return ret;
        }    
    }
    
    if ((ret = reg_field_write(RTL9602C_DOS_CFGr, actField, &actVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SEC | MOD_DAL), "");
        return ret;
    }    
    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_attackPrevent_set */


/* Function Name:
 *      dal_rtl9602c_sec_attackFloodThresh_get
 * Description:
 *      Get flood threshold, time unit 1ms.
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
dal_rtl9602c_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh)
{
    int32   ret;
    uint32  regName;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFloodThresh), RT_ERR_NULL_POINTER);
    switch(type)
    {
        case SEC_ICMPFLOOD:
            regName = RTL9602C_DOS_ICMPFLOOD_THr;
            break;        
        case SEC_SYNCFLOOD:
            regName = RTL9602C_DOS_SYNFLOOD_THr;
            break;        
        case SEC_FINFLOOD:       
            regName = RTL9602C_DOS_FINFLOOD_THr;
            break;        
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if ((ret = reg_field_read(regName, RTL9602C_THf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    *pFloodThresh = value;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_attackFloodThresh_get */

/* Function Name:
 *      dal_rtl9602c_sec_attackFloodThresh_set
 * Description:
 *      Set  flood threshold, time unit 1ms.
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
dal_rtl9602c_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh)
{
    int32   ret;
    uint32  regName;
    uint32  value;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SEC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(sec_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9602C_SEC_FLOOD_THRESHOLD_MAX < floodThresh), RT_ERR_INPUT);
    switch(type)
    {
        case SEC_ICMPFLOOD:
            regName = RTL9602C_DOS_ICMPFLOOD_THr;
            break;        
        case SEC_SYNCFLOOD:
            regName = RTL9602C_DOS_SYNFLOOD_THr;
            break;        
        case SEC_FINFLOOD:       
            regName = RTL9602C_DOS_FINFLOOD_THr;
            break;        
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }
    
    value = floodThresh;
    if ((ret = reg_field_write(regName, RTL9602C_THf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_sec_attackFloodThresh_set */


