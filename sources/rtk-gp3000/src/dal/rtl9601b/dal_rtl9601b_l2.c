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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <osal/time.h>
#include <rtk/port.h>
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_l2.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

static uint32    l2_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */

/* Function Name:
 *		rtl9601b_raw_l2_lookUpTb_set
 * Description:
 *		Set filtering database entry
 * Input:
 *		pL2Table	- L2 table entry writing to 256+32 filtering database
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_OUT_OF_RANGE
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lookUpTb_set(rtl9601b_lut_table_t *pL2Table)
{
   int32 ret = RT_ERR_FAILED;
   rtk_enable_t state;

	RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pL2Table->table_type > RTL9601B_RAW_LUT_ENTRY_TYPE_END, RT_ERR_OUT_OF_RANGE);

	if ((ret = rtl9601b_raw_l2_camEnable_get(&state)) != RT_ERR_OK)
	{
		RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
		return RT_ERR_FAILED;
	}

	if(ENABLED == state)
		RT_PARAM_CHK(pL2Table->address >= (RTL9601B_LUT_4WAY_NO + RTL9601B_LUT_CAM_NO), RT_ERR_INPUT);
	else
		RT_PARAM_CHK(pL2Table->address >= RTL9601B_LUT_4WAY_NO, RT_ERR_INPUT);

#ifndef CONFIG_SDK_ASICDRV_TEST
	RT_PARAM_CHK(RTL9601B_RAW_LUT_READ_METHOD_ADDRESS == pL2Table->method, RT_ERR_INPUT);
#endif
	/*commna part check*/
	RT_PARAM_CHK(pL2Table->l3lookup > 1, RT_ERR_OUT_OF_RANGE);

	/*L2 check*/
	RT_PARAM_CHK(pL2Table->vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);

	/*l2 uc check*/
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(pL2Table->spa), RT_ERR_PORT_ID);
	RT_PARAM_CHK(pL2Table->age > RTL9601B_RAW_LUT_AGEMAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pL2Table->ctag_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pL2Table->ctag_if > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pL2Table->is_static > 1, RT_ERR_OUT_OF_RANGE);


	/*----L3----*/
	/*RT_PARAM_CHK(pL2Table->gip > APOLLO_RTL9601B_RAW_LUT_GIPMAX, RT_ERR_OUT_OF_RANGE);*/

	/* ---L3 MC DSL---*/
	/*RT_PARAM_CHK(pL2Table->sip_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);*/

	/*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
	RT_PARAM_CHK(pL2Table->mbr > RTL9601B_PORTMASK, RT_ERR_OUT_OF_RANGE);

	/*static entry will assign age to none-ZERO*/
	if(pL2Table->is_static && pL2Table->table_type == RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC)
		pL2Table->age = 1;

	/*write to l2 table*/
	if ((ret = table_write(RTL9601B_L2_UCt, pL2Table->address, (uint32 *)pL2Table)) != RT_ERR_OK)
	{
		RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
		return RT_ERR_FAILED;
	}
	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_lookUpTb_get
 * Description:
 *		Get filtering database entry
 * Input:
 *		method		-  Lut access method
 * Output:
 *		pL2Table	- L2 table entry writing to 2K+64 filtering database
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_INPUT
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lookUpTb_get(rtl9601b_lut_table_t *pL2Table)
{
	int32 ret = RT_ERR_FAILED;
	rtk_enable_t state;

	RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pL2Table->method > RTL9601B_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

	if ((ret = rtl9601b_raw_l2_camEnable_get(&state)) != RT_ERR_OK)
	{
		RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
		return RT_ERR_FAILED;
	}

	if(ENABLED == state)
		RT_PARAM_CHK(pL2Table->address >= (RTL9601B_LUT_4WAY_NO + RTL9601B_LUT_CAM_NO), RT_ERR_INPUT);
	else
		RT_PARAM_CHK(pL2Table->address >= RTL9601B_LUT_4WAY_NO, RT_ERR_INPUT);

	if ((ret = table_read(RTL9601B_L2_UCt, pL2Table->address, (uint32 *)pL2Table)) != RT_ERR_OK)
	{
		RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
		return RT_ERR_FAILED;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_flushEn_set
 * Description:
 *		Set per port force flush setting
 * Input:
 *		port		- port id
 *		enable		- enable status
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_INPUT
 *		RT_ERR_PORT_ID
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_flushEn_set(rtk_port_t port, rtk_enable_t enabled)
{
	int32 ret;
	rtl9601b_raw_l2_flushStatus_t status;
	uint32 cnt=0;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

	if ((ret = reg_array_field_write(RTL9601B_L2_TBL_FLUSH_ENr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &enabled)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	/* wait the flush status for non-busy */
	if ((ret = reg_field_read(RTL9601B_L2_TBL_FLUSH_CTRLr, RTL9601B_FLUSH_STATUSf ,&status)) != RT_ERR_OK )
	{
			RT_ERR(ret, (MOD_L2|MOD_DAL), "");
			return ret;
	}
	while (status != RTL9601B_RAW_FLUSH_STATUS_NONBUSY)
	{
		if(cnt++ > 0xFFFF)
			return RT_ERR_NOT_FINISH;

		osal_time_mdelay(10);

		if ((ret = reg_field_read(RTL9601B_L2_TBL_FLUSH_CTRLr, RTL9601B_FLUSH_STATUSf ,&status)) != RT_ERR_OK )
		{
				RT_ERR(ret, (MOD_L2|MOD_DAL), "");
				return ret;
		}
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_flushEn_get
 * Description:
 *		Get per port force flush status
 * Input:
 *		port			- port id
 * Output:
 *		pEnable 		- enable status
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_flushEn_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
	int32 ret;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK(NULL == pEnabled, RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_L2_TBL_FLUSH_ENr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, pEnabled)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}



/* Function Name:
 *		rtl9601b_raw_l2_unkn_saCtl_set
 * Description:
 *		Set drop/trap packet if SA is unknown
 * Input:
 *		port	- Port index (0~6)
 *		action	- 0b00: normal fowarding
 *					   0b01: drop packet & disable learning
 *					   0b10: trap to CPU
 *					   0b11: copy to CPU
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unkn_saCtl_set(rtk_port_t port,rtk_action_t action)
{
	int32 ret;
	rtl9601b_raw_l2_saCtlAct_t rawAction;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

	switch (action)
	{
		case ACTION_FORWARD:
			rawAction = RTL9601B_RAW_LUT_SACTL_ACT_FORWARD;
			break;
		case ACTION_DROP:
			rawAction = RTL9601B_RAW_LUT_SACTL_ACT_DROP;
			break;
		case ACTION_TRAP2CPU:
			rawAction = RTL9601B_RAW_LUT_SACTL_ACT_TRAP;
			break;
		case ACTION_COPY2CPU:
			rawAction = RTL9601B_RAW_LUT_SACTL_ACT_COPY;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	if ((ret = reg_array_field_write(RTL9601B_LUT_UNKN_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &rawAction)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_unkn_saCtl_get
 * Description:
 *		Get drop/trap packet if SA is unknown
 * Input:
 *		port	- Port index (0~6)
 * Output:
 *		pAction 	- 0b00: normal fowarding
 *					   0b01: drop packet & disable learning
 *					   0b10: trap to CPU
 *					   0b11: copy to CPU
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unkn_saCtl_get(rtk_port_t port, rtk_action_t* pAction)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t rawAction;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_LUT_UNKN_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &rawAction)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch (rawAction)
	{
		case RTL9601B_RAW_LUT_SACTL_ACT_FORWARD:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_LUT_SACTL_ACT_DROP:
			*pAction = ACTION_DROP;
			break;
		case RTL9601B_RAW_LUT_SACTL_ACT_TRAP:
			*pAction = ACTION_TRAP2CPU;
			break;
		case RTL9601B_RAW_LUT_SACTL_ACT_COPY:
			*pAction = ACTION_COPY2CPU;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_unkn_ucDaCtl_set
 * Description:
 *		Set drop/trap packet if unicast DA is unknown
 * Input:
 *		port	- Port index (0~6)
 *		action	- 0b00: normal flooding
 *					   0b01: drop packet, exclude IGMP/MLD packets
*					   0b10: trap to CPU, exclude IGMP/MLD packets
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unkn_ucDaCtl_set(rtk_port_t port,rtk_action_t action)
{
	int32 ret;
	rtl9601b_raw_l2_saCtlAct_t rawAction;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

	switch(action)
	{
		case ACTION_FORWARD:
			rawAction = RTL9601B_RAW_L2_COMMON_ACT_FORWARD;
			break;
		case ACTION_DROP:
			rawAction = RTL9601B_RAW_L2_COMMON_ACT_DROP;
			break;
		case ACTION_TRAP2CPU:
			rawAction = RTL9601B_RAW_L2_COMMON_ACT_TRAP;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	if ((ret = reg_array_field_write(RTL9601B_LUT_UNKN_UC_DA_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &action)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_unkn_ucDaCtl_get
 * Description:
 *		Get drop/trap packet if unicast DA is unknown
 * Input:
 *		port	- Port index (0~6)
 * Output:
 *		 action 	- 0b00: normal flooding
 *					   0b01: drop packet, exclude IGMP/MLD packets
*					   0b10: trap to CPU, exclude IGMP/MLD packets
 *
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unkn_ucDaCtl_get(rtk_port_t port, rtk_action_t* pAction)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t rawAction;
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
	if ((ret = reg_array_field_read(RTL9601B_LUT_UNKN_UC_DA_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &rawAction)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch (rawAction)
	{
		case RTL9601B_RAW_L2_COMMON_ACT_FORWARD:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_DROP:
			*pAction = ACTION_DROP;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_TRAP:
			*pAction = ACTION_TRAP2CPU;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_l2_learnOverAct_set
 * Description:
 *		Set auto learn over limit number action
 * Input:
 *		port	- Port index (0~6)
 *		action	- Learn over action 0:normal, 1:drop 2:trap
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_learnOverAct_set(rtk_port_t port, rtk_action_t action)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t rawAction;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

	switch(action)
	{
		case ACTION_FORWARD:
			rawAction=RTL9601B_RAW_L2_COMMON_ACT_FORWARD;
			break;
		case ACTION_DROP:
			rawAction=RTL9601B_RAW_L2_COMMON_ACT_DROP;
			break;
		case ACTION_TRAP2CPU:
			rawAction=RTL9601B_RAW_L2_COMMON_ACT_TRAP;
			break;
		case ACTION_COPY2CPU:
			rawAction=RTL9601B_RAW_L2_COMMON_ACT_COPY2CPU;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	if ((ret = reg_array_field_write(RTL9601B_LUT_LEARN_OVER_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &rawAction))!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_learnOverAct_get
 * Description:
 *		Get auto learn over limit number action
 * Input:
 *		port	- Port index (0~6)
 * Output:
 *		pAction 	- Learn over action 0:normal, 1:drop 2:trap
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_learnOverAct_get(rtk_port_t port, rtk_action_t* pAction)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t rawAction;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_LUT_LEARN_OVER_CTRLr, port,REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &rawAction)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch(rawAction)
	{
		case RTL9601B_RAW_L2_COMMON_ACT_FORWARD:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_DROP:
			*pAction = ACTION_DROP;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_TRAP:
			*pAction = ACTION_TRAP2CPU;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_COPY2CPU:
			*pAction = ACTION_COPY2CPU;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_l2_agingEnable_set
 * Description:
 *		Set L2 LUT aging per port setting
 * Input:
 *		port		- port id
 *		enable		- enable status
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_agingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	int32 ret;
	uint32 tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

	if (enable == ENABLED)
		tmpData = ENABLED;
	else
		tmpData = DISABLED;

	if ((ret = reg_array_field_write(RTL9601B_LUT_AGEOUT_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_AGEOUT_OUTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;

}
/* Function Name:
 *		rtl9601b_raw_l2_agingEnable_get
 * Description:
 *		Get L2 LUT aging per port setting
 * Input:
 *		port		- port id
 *
 * Output:
 *		pEnable 	- enable status
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_agingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	int32 ret;
	uint32 tmpData;

	RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	if ((ret = reg_array_field_read(RTL9601B_LUT_AGEOUT_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_AGEOUT_OUTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	if (tmpData == ENABLED)
		*pEnable = ENABLED;
	else
		*pEnable = DISABLED;

	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_l2_lrnLimitNo_set
 * Description:
 *		Set L2 learning limitation entry number. ASIC supports 2K LUT entries and L2 learning limitation is
 *		 for auto-learning L2 entry number. Set register to value 0xFFF for non-limitation.
 * Input:
 *		port	- Physical port number (0~6)
 *		num 	 - set limitation entry number
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_OUT_OF_RANGE
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lrnLimitNo_set(rtk_port_t port, uint32 num)
{
	int32 ret = RT_ERR_FAILED;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK(num > RTL9601B_LUT_TBL_NO, RT_ERR_OUT_OF_RANGE);

	if ((ret = reg_array_field_write(RTL9601B_LUT_LRN_LIMITNOr, port,REG_ARRAY_INDEX_NONE, RTL9601B_NUMf, &num)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_lrnLimitNo_get
 * Description:
 *		Get L2 learning limitation entry number. ASIC supports 2K LUT entries and L2 learning limitation is
 *		 for auto-learning L2 entry number. Set register to value 0xFFF for non-limitation.
  * Input:
  * 	 port	- Physical port number (0~6)
 *
 * Output:
 *		pNum	  - get limitation entry number
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lrnLimitNo_get(rtk_port_t port, uint32 *pNum)
{
	int32 ret = RT_ERR_FAILED;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK(pNum == NULL, RT_ERR_NULL_POINTER);

	if (( ret = reg_array_field_read(RTL9601B_LUT_LRN_LIMITNOr, port, REG_ARRAY_INDEX_NONE, RTL9601B_NUMf, pNum)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_lrnCnt_get
 * Description:
 *		Get    Number of SA learned
 * Input:
  * 	 port	- Physical port number (0~6)
  * Output:
 *		pNum	  - get Number of SA learned on Port
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lrnCnt_get(rtk_port_t port, uint32 *pNum)
{
	int32 ret = RT_ERR_FAILED;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK(pNum == NULL, RT_ERR_NULL_POINTER);

	if((ret = reg_array_field_read(RTL9601B_L2_LRN_CNTr, port, REG_ARRAY_INDEX_NONE, RTL9601B_L2_LRN_CNTf, pNum)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}


/* Function Name:
 *		rtl9601b_raw_l2_lrnOverSts_set
 * Description:
 *		Set L2 per-port learning over event to clear register
 * Input:
 *		port	- Physical port number (0~6)
 *		value	   - set 1 to clear
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_PORT_ID
 *		RT_ERR_OUT_OF_RANGE
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lrnOverSts_set(rtk_port_t port, uint32 value)
{
	int32 ret = RT_ERR_FAILED;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK(value > 1, RT_ERR_OUT_OF_RANGE);

	if (( ret = reg_array_field_write(RTL9601B_L2_LRN_OVER_STSr, port, REG_ARRAY_INDEX_NONE, RTL9601B_LRN_OVER_INDf, &value)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_lrnOverSts_get
 * Description:
 *		Get  L2 per-port learning over event
 * Input:
*	   port 	- Physical port number (0~6)
 *
 * Output:
 *		pValue		- L2 learning over ever occurs

 * Return:
 *		RT_ERR_OK			- Success
 *		RT_ERR_PORT_ID		- Invalid port id
 *		RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_lrnOverSts_get(rtk_port_t port, uint32 *pValue)
{
	int32 ret = RT_ERR_FAILED;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK(pValue == NULL, RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_L2_LRN_OVER_STSr, port, REG_ARRAY_INDEX_NONE, RTL9601B_LRN_OVER_INDf, pValue)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_unkn_l2Mc_set
 * Description:
 *		Set unknow L2 multicast frame behavior
 * Input:
 *		port	- Port index (0~6)
 *		action	- Learn over action
 *					   0b00:normal
 *					   0b01:drop
 *					   0b10:trap to CPU
 *					   0b11:drop packet exclude RMA
 *
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- Success
 *		RT_ERR_NOT_ALLOWED	- Invalid learn over action
 *		RT_ERR_PORT_ID			- Invalid port id
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_l2Mc_set(rtk_port_t port, rtk_action_t action)
{
	int32 ret;
	rtl9601b_raw_l2_unknMcAct_t tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	tmpData = RTL9601B_RAW_L2_UNKNMC_ACT_END;
	switch (action)
	{
		case ACTION_FORWARD:
			tmpData = RTL9601B_RAW_L2_UNKNMC_ACT_FORWARD;
			break;
		case ACTION_DROP:
			tmpData = RTL9601B_RAW_L2_UNKNMC_ACT_DROP;
			break;
		case ACTION_TRAP2CPU:
			tmpData = RTL9601B_RAW_L2_UNKNMC_ACT_TRAP;
			break;
		case ACTION_DROP_EXCLUDE_RMA:
			tmpData = RTL9601B_RAW_L2_UNKNMC_ACT_EXCLUDE_RMA;
			break;
		default:
			tmpData = RTL9601B_RAW_L2_UNKNMC_ACT_END;

	}
	RT_PARAM_CHK(tmpData >= RTL9601B_RAW_L2_UNKNMC_ACT_END, RT_ERR_NOT_ALLOWED);
	if ((ret = reg_array_field_write(RTL9601B_UNKN_L2_MCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_unkn_l2Mc_get
 * Description:
 *		Get unknow L2 multicast frame behavior
 * Input:
 *		port	- Port index (0~6)
  *
 * Output:
 *		action	- Learn over action
 *					   0b00:normal
 *					   0b01:drop
 *					   0b10:trap to CPU
 *					   0b11:drop packet exclude RMA
 * Return:
 *		RT_ERR_OK				- Success
 *		RT_ERR_NULL_POINTER   - Null pointer
 *		RT_ERR_PORT_ID			- Invalid port id
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_l2Mc_get(rtk_port_t port, rtk_action_t* pAction)
{
	int32 ret;
	rtl9601b_raw_l2_unknMcAct_t tmpData;
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
	if ((ret = reg_array_field_read(RTL9601B_UNKN_L2_MCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	switch (tmpData)
	{

		case RTL9601B_RAW_L2_UNKNMC_ACT_FORWARD:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_L2_UNKNMC_ACT_DROP:
			*pAction = ACTION_DROP;
			break;
		case RTL9601B_RAW_L2_UNKNMC_ACT_TRAP:
			*pAction = ACTION_TRAP2CPU;
			break;
		case RTL9601B_RAW_L2_UNKNMC_ACT_EXCLUDE_RMA:
			*pAction = ACTION_DROP_EXCLUDE_RMA;
			 break;
		default:
			return RT_ERR_NOT_ALLOWED;
	}
	return RT_ERR_OK;

}
/* Function Name:
 *		rtl9601b_raw_unkn_ip4Mc_set
 * Description:
 *		Set unknow IPv4 multicast frame behavior
 * Input:
 *		port	- Port index (0~6)
 *		action	- Learn over action
 *					   0b00:normal flooding
 *					   0b01:drop packet, exclude IP 224.0.0.x and IGMP packets
 *					   0b10:trap to CPU, exclude IP 224.0.0.x and IGMP packets
 *
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK				- Success
 *		RT_ERR_PORT_ID			- Invalid port id
 *		RT_ERR_NOT_ALLOWED	- Invalid learn over action
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_ip4Mc_set(rtk_port_t port, rtk_action_t action)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	tmpData = RTL9601B_RAW_L2_COMMON_ACT_END;
	switch (action)
	{
		case ACTION_FORWARD:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_FORWARD;
			break;
		case ACTION_DROP:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_DROP;
			break;
		case ACTION_TRAP2CPU:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_TRAP;
			break;
		default:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_END;
	}

	RT_PARAM_CHK(tmpData >= RTL9601B_RAW_L2_COMMON_ACT_END, RT_ERR_NOT_ALLOWED);
	if ((ret = reg_array_field_write(RTL9601B_UNKN_IP4_MCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_unkn_ip4Mc_get
 * Description:
 *		Get unknow IPv4 multicast frame behavior
 * Input:
 *		port	 - Port index (0~6)
  *
 * Output:

 *		action	- Learn over action
   *					 0b00:normal flooding
   *					 0b01:drop packet, exclude IP 224.0.0.x and IGMP packets
   *					 0b10:trap to CPU, exclude IP 224.0.0.x and IGMP packets
 * Return:
 *		RT_ERR_OK				- Success
 *		RT_ERR_PORT_ID			- Invalid port id
 *		RT_ERR_NULL_POINTER   -Null pointer
 *		RT_ERR_NOT_ALLOWED	- Invalid learn over action
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_ip4Mc_get(rtk_port_t port, rtk_action_t* pAction)
{
	int32 ret = RT_ERR_FAILED;
	rtl9601b_raw_l2_commonAct_t tmpData;
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
	if ((ret = reg_array_field_read(RTL9601B_UNKN_IP4_MCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	switch (tmpData)
	{

		case RTL9601B_RAW_L2_COMMON_ACT_FORWARD:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_DROP:
			*pAction = ACTION_DROP;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_TRAP:
			*pAction = ACTION_TRAP2CPU;
			break;
		default:
			return RT_ERR_NOT_ALLOWED;
	}
	return RT_ERR_OK;

}
/* Function Name:
 *		rtl9601b_raw_unkn_ip6Mc_set
 * Description:
 *		Set unknow IPv6 multicast frame behavior
 * Input:
 *		port	- Port index (0~6)
 *		action	- Learn over action
 *					   0b00:normal flooding
 *					   0b01:drop packet, exclude IP [FFXX::/8] and MLD packets
 *					   0b10:trap to CPU, exclude IP [FFXX::/8] and MLD packets
 *
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK				- Success
 *		RT_ERR_PORT_ID			- Invalid port id
 *		RT_ERR_NOT_ALLOWED	- Invalid learn over action
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_ip6Mc_set(rtk_port_t port, rtk_action_t action)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	tmpData = RTL9601B_RAW_L2_COMMON_ACT_END;
	switch (action)
	{
		case ACTION_FORWARD:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_FORWARD;
			break;
		case ACTION_DROP:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_DROP;
			break;
		case ACTION_TRAP2CPU:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_TRAP;
			break;
		default:
			tmpData = RTL9601B_RAW_L2_COMMON_ACT_END;

	}
	RT_PARAM_CHK(tmpData >= RTL9601B_RAW_L2_COMMON_ACT_END, RT_ERR_NOT_ALLOWED);
	if ((ret=reg_array_field_write(RTL9601B_UNKN_IP6_MCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_unkn_ip6Mc_get
 * Description:
 *		Get unknow IPv6 multicast frame behavior
 * Input:
  * 	 port	- Port index (0~6)
 *
 * Output:
  * 	 action 	- Learn over action
 *					   0b00:normal flooding
 *					   0b01:drop packet, exclude IP [FFXX::/8] and MLD packets
 *					   0b10:trap to CPU, exclude IP [FFXX::/8] and MLD packets
 * Return:
 *		RT_ERR_OK			- Success
 *		RT_ERR_NULL_POINTER 	 -Null pointer
 *		RT_ERR_PORT_ID			- Invalid port id
 *		RT_ERR_NOT_ALLOWED	- Invalid learn over action
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_ip6Mc_get(rtk_port_t port, rtk_action_t* pAction)
{
	int32 ret;
	rtl9601b_raw_l2_commonAct_t tmpData;
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
	if (( ret = reg_array_field_read(RTL9601B_UNKN_IP6_MCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ACTf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}
	switch (tmpData)
	{
		case RTL9601B_RAW_L2_COMMON_ACT_FORWARD:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_DROP:
			*pAction = ACTION_DROP;
			break;
		case RTL9601B_RAW_L2_COMMON_ACT_TRAP:
			*pAction = ACTION_TRAP2CPU;
			break;
		default:
			return RT_ERR_NOT_ALLOWED;
	}
	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_unkn_mcPri_set
 * Description:
 *		Set Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Input:
 *		priority - Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- Success
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *		None
 */
int32 rtl9601b_raw_unkn_mcPri_set(uint32 priority)
{
	int32 ret;

	RT_PARAM_CHK((priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_OUT_OF_RANGE);

	if ((ret = reg_field_write(RTL9601B_UNKN_MC_CFGr, RTL9601B_UNKN_MC_PRIf, &priority)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_unkn_mcPri_get
 * Description:
 *		Get Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Input:
 *		 None
 * Output:
 *		  pPriority - Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Return:
 *		RT_ERR_OK			- Success
 *		RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *		None
 */

int32 rtl9601b_raw_unkn_mcPri_get(uint32* pPriority)
{
	int32 ret;

	RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

	if ((ret = reg_field_read(RTL9601B_UNKN_MC_CFGr, RTL9601B_UNKN_MC_PRIf, pPriority)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_bcFlood_set
 * Description:
 *		Set egress port status for broadcast(ff-ff-ff-ff-ff-ff) flooding packets
 * Input:
 *		port		   - Physical port number (0~6)
 *		enabled 		- 0b0: drop; 0b1: normal flooding
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_bcFlood_set(rtk_port_t port, rtk_enable_t enabled)
{
	int32 ret = RT_ERR_FAILED;
	rtl9601b_raw_l2_flood_enable_t tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

	if ( enabled == ENABLED)
		tmpData = RTL9601B_RAW_LUT_FLOOD_FLOODING;
	else
		tmpData = RTL9601B_RAW_LUT_FLOOD_DROP;

	if ((ret = reg_array_field_write(RTL9601B_LUT_BC_FLOODr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_bcFlood_get
 * Description:
 *		Get egress port status for broadcast(ff-ff-ff-ff-ff-ff) flooding packets
 * Input:
 *		port		   - Physical port number (0~6)
 *
 * Output:
 *		pEnabled		- 0b0: drop; 0b1: normal flooding
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_bcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
	int32 ret = RT_ERR_FAILED;
	rtl9601b_raw_l2_flood_enable_t tmpData;

	RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

	if (( ret = reg_array_field_read(RTL9601B_LUT_BC_FLOODr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch(tmpData)
	{
		case RTL9601B_RAW_LUT_FLOOD_FLOODING:
			*pEnabled = ENABLED;
			break;
		case RTL9601B_RAW_LUT_FLOOD_DROP:
			*pEnabled = DISABLED;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;

}
/* Function Name:
 *		rtl9601b_raw_l2_unknUcFlood_set
 * Description:
 *		Set egress port status for unknown multicast flooding
 * Input:
 *		port		   - Physical port number (0~6)
 *		enabled 		- 0b0: drop; 0b1: normal flooding
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unknUcFlood_set(rtk_port_t port, rtk_enable_t enabled)
{
	int32 ret;
	rtl9601b_raw_l2_flood_enable_t tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

	if ( enabled == ENABLED )
		tmpData = RTL9601B_RAW_LUT_FLOOD_FLOODING;
	else
		tmpData = RTL9601B_RAW_LUT_FLOOD_DROP;

	if (( ret = reg_array_field_write(RTL9601B_LUT_UNKN_UC_FLOODr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_unknUcFlood_get
 * Description:
 *		Get egress port status for unknown multicast flooding
 * Input:
 *		port		   - Physical port number (0~6)
  * Output:
 *		pEnabled		- 0b0: drop; 0b1: normal flooding
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unknUcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
	int32 ret;
	rtl9601b_raw_l2_flood_enable_t tmpData;

	RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

	if ((ret = reg_array_field_read(RTL9601B_LUT_UNKN_UC_FLOODr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch(tmpData)
	{
		case RTL9601B_RAW_LUT_FLOOD_FLOODING:
			*pEnabled = ENABLED;
			break;
		case RTL9601B_RAW_LUT_FLOOD_DROP:
			*pEnabled = DISABLED;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_unknMcFlood_set
 * Description:
 *		Set egress port mask for unknown unicast flooding packets
 * Input:
 *		port		   - Physical port number (0~6)
 *		enabled 		- 0b0: drop; 0b1: normal flooding
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_PORT_ID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unknMcFlood_set(rtk_port_t port, rtk_enable_t enabled)
{
	int32 ret = RT_ERR_FAILED;
	rtl9601b_raw_l2_flood_enable_t tmpData;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

	if (enabled == ENABLED)
		tmpData = RTL9601B_RAW_LUT_FLOOD_FLOODING;
	else
		tmpData = RTL9601B_RAW_LUT_FLOOD_DROP;

	if ((ret = reg_array_field_write(RTL9601B_LUT_UNKN_MC_FLOODr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_l2_unknMcFlood_get
 * Description:
 *		Get egress port mask for unknown unicast flooding packets
 * Input:
 *		port		   - Physical port number (0~6)
 * Output:
  * 	 pEnabled		- 0b0: drop; 0b1: normal flooding
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unknMcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
	int32 ret;
	rtl9601b_raw_l2_flood_enable_t tmpData;

	RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	if ((ret = reg_array_field_read(RTL9601B_LUT_UNKN_MC_FLOODr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch(tmpData)
	{
		case RTL9601B_RAW_LUT_FLOOD_FLOODING:
			*pEnabled = ENABLED;
			break;
		case RTL9601B_RAW_LUT_FLOOD_DROP:
			*pEnabled = DISABLED;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;
}


/* Function Name:
 *		rtl9601b_raw_l2_ipmcAction_set
 * Description:
 *		Set per port multicast data action
 * Input:
 *		port			- Physical port number (0~6)
 *		action			- Allow / Drop
 * Output:
 *		None.
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_PORT_ID
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_ipmcAction_set(rtk_port_t port, rtk_action_t action)
{
	int32					ret;
	rtl9601b_raw_l2_McAllow_t raw_mc_act;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

	switch(action)
	{
		case ACTION_FORWARD:
			raw_mc_act = RTL9601B_RAW_L2_MC_ALLOW;
			break;
		case ACTION_DROP:
			raw_mc_act = RTL9601B_RAW_L2_MC_DROP;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	if ((ret = reg_array_field_write(RTL9601B_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ALLOW_MC_DATAf, (uint32 *)&raw_mc_act)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_ipmcAction_get
 * Description:
 *		Get per port multicast data action
 * Input:
 *		port			- Physical port number (0~6)
 * Output:
 *		pAction 		- Allow / Drop.
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_PORT_ID
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_ipmcAction_get(rtk_port_t port, rtk_action_t *pAction)
{
	int32					ret;
	rtl9601b_raw_l2_McAllow_t raw_mc_act;

	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ALLOW_MC_DATAf, (uint32 *)&raw_mc_act)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		return ret;
	}

	switch(raw_mc_act)
	{
		case RTL9601B_RAW_L2_MC_ALLOW:
			*pAction = ACTION_FORWARD;
			break;
		case RTL9601B_RAW_L2_MC_DROP:
			*pAction = ACTION_DROP;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;
}


/* Function Name:
 *		rtl9601b_raw_l2_camEnable_set
 * Description:
 *		Set cam usage status
 * Input:
 *		enable		- enable status
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_camEnable_set(rtk_enable_t enable)
{
	int32 ret;
	uint32 tmpData;

	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

	if (enable == ENABLED)
		tmpData = DISABLED;
	else
		tmpData = ENABLED;

	if ((ret = reg_field_write(RTL9601B_LUT_CFGr, RTL9601B_BCAM_DISf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
} /* end of rtl9601b_raw_l2_camEnable_set */

/* Function Name:
 *		rtl9601b_raw_l2_camEnable_get
 * Description:
 *		Get cam usage status
 * Input:
 *		None
 * Output:
 *		pEnable 		- enable status
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_camEnable_get(rtk_enable_t *pEnable)
{
	int32 ret;
	uint32 tmpData;

	if ((ret = reg_field_read(RTL9601B_LUT_CFGr, RTL9601B_BCAM_DISf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	if (tmpData == ENABLED)
		*pEnable = DISABLED;
	else
		*pEnable = ENABLED;

	return RT_ERR_OK;
} /* end of rtl9601b_raw_l2_camEnable_get */


/* Function Name:
 *		rtl9601b_raw_l2_agingTime_set
 * Description:
 *		Set L2 aging time
 * Input:
 *		agingTime		- L2 aging time, unit 0.1sec
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_OUT_OF_RANGE
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_agingTime_set(uint32 agingTime)
{
	int32 ret;

	RT_PARAM_CHK((agingTime > RTL9601B_RAW_LUT_AGESPEEDMAX), RT_ERR_OUT_OF_RANGE);

	if ((ret = reg_field_write(RTL9601B_LUT_CFGr, RTL9601B_AGE_SPDf, &agingTime)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
} /* end of rtl9601b_raw_l2_agingTime_set */

/* Function Name:
 *		rtl9601b_raw_l2_agingTime_get
 * Description:
 *		Get L2 aging time
 * Input:
 *		None
 * Output:
 *		pAgingTime		- L2 aging time, unit 0.1sec
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_agingTime_get(uint32 *pAgingTime)
{
	int32 ret;

	RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

	if ((ret = reg_field_read(RTL9601B_LUT_CFGr, RTL9601B_AGE_SPDf, pAgingTime)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
} /* end of rtl9601b_raw_l2_agingTime_get */



/* Function Name:
 *		rtl9601b_raw_l2_ipmcHashType_set
 * Description:
 *		Set IPv4 multicast lookup hash method
 * Input:
 *		type		- hash method
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_ipmcHashType_set(rtl9601b_raw_l2_ipMcHashType_t type)
{
	int32 ret;

	RT_PARAM_CHK((RTL9601B_RAW_LUT_IPMCHASH_TYPE_END <= type), RT_ERR_CHIP_NOT_SUPPORTED);

	if ((ret = reg_field_write(RTL9601B_LUT_CFGr, RTL9601B_LUT_IPMC_HASHf, &type)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
} /* end of rtl9601b_raw_l2_ipmcHashType_set */

/* Function Name:
 *		rtl9601b_raw_l2_ipmcHashType_get
 * Description:
 *		Get IPv4 multicast lookup hash method
 * Input:
 *		None
 * Output:
 *		pType		- hash method
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_ipmcHashType_get(rtl9601b_raw_l2_ipMcHashType_t *pType)
{
	int32 ret;

	RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

	if ((ret = reg_field_read(RTL9601B_LUT_CFGr, RTL9601B_LUT_IPMC_HASHf, pType)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
} /* end of rtl9601b_raw_l2_ipmcHashType_set */


/* Function Name:
 *		rtl9601b_raw_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *		Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *		enable		- enable status
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
	int32 ret;

	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

	if ((ret = reg_field_write(RTL9601B_LUT_CFGr, RTL9601B_LINKDOWN_AGEOUTf, &enable)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *		Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *		None
 * Output:
 *		pEnable 		- enable status
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
	int32 ret;

	RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

	if ((ret = reg_field_read(RTL9601B_LUT_CFGr, RTL9601B_LINKDOWN_AGEOUTf, pEnable)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_unknReservedMcFlood_set
 * Description:
 *		Set unknown Reserved IP multicast address packet flooding state
 * Input:
 *		state	- ENABLED  : Always flooding, DISABLED : Treat as normal unknown IP multicast packets
 * Output:
 *		Nnoe.
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unknReservedMcFlood_set(rtk_enable_t state)
{
	int32 ret;

	RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

	if ((ret = reg_field_write(RTL9601B_UNKN_MC_CFGr, RTL9601B_UNKN_MC_RES_FLOODf, &state)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_l2_unknReservedMcFlood_get
 * Description:
 *		get unknown Reserved IP multicast address packet flooding state
 * Input:
 *		None.
 * Output:
 *		pState	- ENABLED  : Always flooding, DISABLED : Treat as normal unknown IP multicast packets
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NULL_POINTER
 * Note:
 *		None
 */
int32 rtl9601b_raw_l2_unknReservedMcFlood_get(rtk_enable_t *pState)
{
	int32 ret;

	RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

	if ((ret = reg_field_read(RTL9601B_UNKN_MC_CFGr, RTL9601B_UNKN_MC_RES_FLOODf, pState)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
		return ret;
	}

	return RT_ERR_OK;
}

/*end of raw API*/

/*
 * Function Declaration
 */
static int32
_dal_rtl9601b_rawtoL2uc(rtk_l2_ucastAddr_t *pL2Data, rtl9601b_lut_table_t *pLut_entry)
{
    pL2Data->vid            = pLut_entry->vid;
    pL2Data->mac.octet[0]   = pLut_entry->mac.octet[0];
    pL2Data->mac.octet[1]   = pLut_entry->mac.octet[1];
    pL2Data->mac.octet[2]   = pLut_entry->mac.octet[2];
    pL2Data->mac.octet[3]   = pLut_entry->mac.octet[3];
    pL2Data->mac.octet[4]   = pLut_entry->mac.octet[4];
    pL2Data->mac.octet[5]   = pLut_entry->mac.octet[5];
    pL2Data->port           = pLut_entry->spa;
    pL2Data->age            = pLut_entry->age;
    pL2Data->index          = pLut_entry->address;
	pL2Data->ctag_vid		= pLut_entry->ctag_vid;

    if(ENABLED == pLut_entry->ivl_svl)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_IVL;

	if(ENABLED == pLut_entry->ctag_if)
		pL2Data->flags |= RTK_L2_UCAST_FLAG_CTAG_IF;

	if(ENABLED == pLut_entry->is_static)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_STATIC;


    return RT_ERR_OK;
}


static int32
_dal_rtl9601b_l2uctoRaw(rtl9601b_lut_table_t *pLut_entry, rtk_l2_ucastAddr_t *pL2Data)
{
    pLut_entry->vid    = pL2Data->vid;
    pLut_entry->mac.octet[0]= pL2Data->mac.octet[0];
    pLut_entry->mac.octet[1]= pL2Data->mac.octet[1];
    pLut_entry->mac.octet[2]= pL2Data->mac.octet[2];
    pLut_entry->mac.octet[3]= pL2Data->mac.octet[3];
    pLut_entry->mac.octet[4]= pL2Data->mac.octet[4];
    pLut_entry->mac.octet[5]= pL2Data->mac.octet[5];
    pLut_entry->spa         = pL2Data->port;
    pLut_entry->age         = pL2Data->age;
    pLut_entry->address     = pL2Data->index;
	pLut_entry->ctag_vid 	= pL2Data->ctag_vid;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_IVL)
        pLut_entry->ivl_svl = ENABLED;

	if (pL2Data->flags & RTK_L2_UCAST_FLAG_CTAG_IF)
        pLut_entry->ctag_if = ENABLED;

	if (pL2Data->flags & RTK_L2_UCAST_FLAG_STATIC)
        pLut_entry->is_static= ENABLED;

    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_rawtoL2mc(rtk_l2_mcastAddr_t *pL2Data, rtl9601b_lut_table_t *pLut_entry)
{

    pL2Data->vid            = pLut_entry->vid;
    pL2Data->mac.octet[0]   = pLut_entry->mac.octet[0];
    pL2Data->mac.octet[1]   = pLut_entry->mac.octet[1];
    pL2Data->mac.octet[2]   = pLut_entry->mac.octet[2];
    pL2Data->mac.octet[3]   = pLut_entry->mac.octet[3];
    pL2Data->mac.octet[4]   = pLut_entry->mac.octet[4];
    pL2Data->mac.octet[5]   = pLut_entry->mac.octet[5];
    pL2Data->index          = pLut_entry->address;
    pL2Data->portmask.bits[0] = pLut_entry->mbr;

    if(ENABLED == pLut_entry->ivl_svl)
    {
        pL2Data->vid   = pLut_entry->vid;
		pL2Data->flags |= RTK_L2_MCAST_FLAG_IVL;
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_l2mctoRaw(rtl9601b_lut_table_t *pLut_entry, rtk_l2_mcastAddr_t *pL2Data)
{
    pLut_entry->mac.octet[0] = pL2Data->mac.octet[0];
    pLut_entry->mac.octet[1] = pL2Data->mac.octet[1];
    pLut_entry->mac.octet[2] = pL2Data->mac.octet[2];
    pLut_entry->mac.octet[3] = pL2Data->mac.octet[3];
    pLut_entry->mac.octet[4] = pL2Data->mac.octet[4];
    pLut_entry->mac.octet[5] = pL2Data->mac.octet[5];
    pLut_entry->address      = pL2Data->index;
    pLut_entry->mbr          = pL2Data->portmask.bits[0];


    if (pL2Data->flags & RTK_L2_MCAST_FLAG_IVL)
    {
        pLut_entry->vid = pL2Data->vid;
        pLut_entry->ivl_svl  = ENABLED;
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_rawtoIpmc(rtk_l2_ipMcastAddr_t *pL2Data, rtl9601b_lut_table_t *pLut_entry)
{
    int32 ret;
    rtk_l2_ipmcMode_t   mode;

    if((ret = dal_rtl9601b_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    pL2Data->dip            = (pLut_entry->gip | 0xE0000000);

    pL2Data->vid = pLut_entry->sip_vid;

    pL2Data->portmask.bits[0]     = pLut_entry->mbr;
    pL2Data->index                = pLut_entry->address;

    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_IpmctoRaw(rtl9601b_lut_table_t *pLut_entry, rtk_l2_ipMcastAddr_t *pL2Data)
{
    int32 ret;
    rtk_l2_ipmcMode_t   mode;

    if((ret = dal_rtl9601b_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    pLut_entry->gip =   pL2Data->dip;

    if(!(pL2Data->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY))
        pLut_entry->sip_vid = pL2Data->vid;

    if(pL2Data->flags & RTK_L2_IPMCAST_FLAG_STATIC)
        pLut_entry->is_static = 1;


    pLut_entry->mbr      = pL2Data->portmask.bits[0];
    pLut_entry->address  = pL2Data->index;

    return RT_ERR_OK;
}


static int32
_dal_rtl9601b_lutReadMethodtoRaw(rtl9601b_raw_l2_readMethod_t *raw, rtk_l2_readMethod_t cfg)
{
   	switch(cfg){
	case LUT_READ_METHOD_MAC:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_MAC;
	break;
	case LUT_READ_METHOD_ADDRESS:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_ADDRESS;
	break;
	case LUT_READ_METHOD_NEXT_ADDRESS:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_NEXT_ADDRESS;
	break;
	case LUT_READ_METHOD_NEXT_L2UC:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UC;
	break;
	case LUT_READ_METHOD_NEXT_L2MC:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2MC;
	break;
	case LUT_READ_METHOD_NEXT_L3MC:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L3MC;
	break;
	case LUT_READ_METHOD_NEXT_L2L3MC:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2L3MC;
	break;
	case LUT_READ_METHOD_NEXT_L2UCSPA:
		*raw = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UCSPA;
	break;
	default:
	break;
	}
    return RT_ERR_OK;
}


static int32
_dal_rtl9601b_lutRawtoReadMethod(rtk_l2_readMethod_t *cfg,rtl9601b_raw_l2_readMethod_t raw)
{

	switch(raw){
	case RTL9601B_RAW_LUT_READ_METHOD_MAC:
		*cfg = LUT_READ_METHOD_MAC;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_ADDRESS:
		*cfg = LUT_READ_METHOD_ADDRESS;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
		*cfg = LUT_READ_METHOD_NEXT_ADDRESS;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UC:
		*cfg = LUT_READ_METHOD_NEXT_L2UC;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2MC:
		*cfg = LUT_READ_METHOD_NEXT_L2MC;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L3MC:
		*cfg = LUT_READ_METHOD_NEXT_L3MC;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
		*cfg = LUT_READ_METHOD_NEXT_L2L3MC;
	break;
	case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
		*cfg = LUT_READ_METHOD_NEXT_L2UCSPA;
	break;
	default:
	break;
	}
    return RT_ERR_OK;
}

static int _dal_rtl9601b_l2MacHash(rtk_mac_t mac, uint32 *pHashValue)
{
    unsigned char index[6];

    index[0] = 
        ((mac.octet[5] & (1<<0))>>0) ^
        ((mac.octet[5] & (1<<6))>>6) ^
        ((mac.octet[4] & (1<<4))>>4) ^
        ((mac.octet[3] & (1<<2))>>2) ^
        ((mac.octet[2] & (1<<0))>>0) ^
        ((mac.octet[2] & (1<<6))>>6) ^
        ((mac.octet[1] & (1<<4))>>4) ^
        ((mac.octet[0] & (1<<2))>>2);
    index[1] = 
        ((mac.octet[5] & (1<<1))>>1) ^
        ((mac.octet[5] & (1<<7))>>7) ^
        ((mac.octet[4] & (1<<5))>>5) ^
        ((mac.octet[3] & (1<<3))>>3) ^
        ((mac.octet[2] & (1<<1))>>1) ^
        ((mac.octet[2] & (1<<7))>>7) ^
        ((mac.octet[1] & (1<<5))>>5) ^
        ((mac.octet[0] & (1<<3))>>3);
    index[2] = 
        ((mac.octet[5] & (1<<2))>>2) ^
        ((mac.octet[4] & (1<<0))>>0) ^
        ((mac.octet[4] & (1<<6))>>6) ^
        ((mac.octet[3] & (1<<4))>>4) ^
        ((mac.octet[2] & (1<<2))>>2) ^
        ((mac.octet[1] & (1<<0))>>0) ^
        ((mac.octet[1] & (1<<6))>>6) ^
        ((mac.octet[0] & (1<<4))>>4);
    index[3] = 
        ((mac.octet[5] & (1<<3))>>3) ^
        ((mac.octet[4] & (1<<1))>>1) ^
        ((mac.octet[4] & (1<<7))>>7) ^
        ((mac.octet[3] & (1<<5))>>5) ^
        ((mac.octet[2] & (1<<3))>>3) ^
        ((mac.octet[1] & (1<<1))>>1) ^
        ((mac.octet[1] & (1<<7))>>7) ^
        ((mac.octet[0] & (1<<5))>>5);
    index[4] = 
        ((mac.octet[5] & (1<<4))>>4) ^
        ((mac.octet[4] & (1<<2))>>2) ^
        ((mac.octet[3] & (1<<0))>>0) ^
        ((mac.octet[3] & (1<<6))>>6) ^
        ((mac.octet[2] & (1<<4))>>4) ^
        ((mac.octet[1] & (1<<2))>>2) ^
        ((mac.octet[0] & (1<<0))>>0) ^
        ((mac.octet[0] & (1<<6))>>6);
    index[5] = 
        ((mac.octet[5] & (1<<5))>>5) ^
        ((mac.octet[4] & (1<<3))>>3) ^
        ((mac.octet[3] & (1<<1))>>1) ^
        ((mac.octet[3] & (1<<7))>>7) ^
        ((mac.octet[2] & (1<<5))>>5) ^
        ((mac.octet[1] & (1<<3))>>3) ^
        ((mac.octet[0] & (1<<1))>>1) ^
        ((mac.octet[0] & (1<<7))>>7);

    *pHashValue = 
        (index[5] << 5) | (index[4] << 4) | (index[3] << 3) | (index[2] << 2) |
        (index[1] << 1) | (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_rtl9601b_l2MacIvlHash(rtk_mac_t mac, uint32 vid, uint32 *pHashValue)
{
    unsigned char index[6];

    index[0] = 
        ((mac.octet[5] & (1<<0))>>0) ^
        ((mac.octet[5] & (1<<6))>>6) ^
        ((mac.octet[4] & (1<<4))>>4) ^
        ((mac.octet[3] & (1<<2))>>2) ^
        ((mac.octet[2] & (1<<0))>>0) ^
        ((mac.octet[2] & (1<<6))>>6) ^
        ((mac.octet[1] & (1<<4))>>4) ^
        ((mac.octet[0] & (1<<2))>>2) ^
        ((vid & (1<<0))>>0) ^
        ((vid & (1<<6))>>6);
    index[1] = 
        ((mac.octet[5] & (1<<1))>>1) ^
        ((mac.octet[5] & (1<<7))>>7) ^
        ((mac.octet[4] & (1<<5))>>5) ^
        ((mac.octet[3] & (1<<3))>>3) ^
        ((mac.octet[2] & (1<<1))>>1) ^
        ((mac.octet[2] & (1<<7))>>7) ^
        ((mac.octet[1] & (1<<5))>>5) ^
        ((mac.octet[0] & (1<<3))>>3) ^
        ((vid & (1<<1))>>1) ^
        ((vid & (1<<7))>>7);
    index[2] = 
        ((mac.octet[5] & (1<<2))>>2) ^
        ((mac.octet[4] & (1<<0))>>0) ^
        ((mac.octet[4] & (1<<6))>>6) ^
        ((mac.octet[3] & (1<<4))>>4) ^
        ((mac.octet[2] & (1<<2))>>2) ^
        ((mac.octet[1] & (1<<0))>>0) ^
        ((mac.octet[1] & (1<<6))>>6) ^
        ((mac.octet[0] & (1<<4))>>4) ^
        ((vid & (1<<2))>>2) ^
        ((vid & (1<<8))>>8);
    index[3] = 
        ((mac.octet[5] & (1<<3))>>3) ^
        ((mac.octet[4] & (1<<1))>>1) ^
        ((mac.octet[4] & (1<<7))>>7) ^
        ((mac.octet[3] & (1<<5))>>5) ^
        ((mac.octet[2] & (1<<3))>>3) ^
        ((mac.octet[1] & (1<<1))>>1) ^
        ((mac.octet[1] & (1<<7))>>7) ^
        ((mac.octet[0] & (1<<5))>>5) ^
        ((vid & (1<<3))>>3) ^
        ((vid & (1<<9))>>9);
    index[4] = 
        ((mac.octet[5] & (1<<4))>>4) ^
        ((mac.octet[4] & (1<<2))>>2) ^
        ((mac.octet[3] & (1<<0))>>0) ^
        ((mac.octet[3] & (1<<6))>>6) ^
        ((mac.octet[2] & (1<<4))>>4) ^
        ((mac.octet[1] & (1<<2))>>2) ^
        ((mac.octet[0] & (1<<0))>>0) ^
        ((mac.octet[0] & (1<<6))>>6) ^
        ((vid & (1<<4))>>4) ^
        ((vid & (1<<10))>>10);
    index[5] = 
        ((mac.octet[5] & (1<<5))>>5) ^
        ((mac.octet[4] & (1<<3))>>3) ^
        ((mac.octet[3] & (1<<1))>>1) ^
        ((mac.octet[3] & (1<<7))>>7) ^
        ((mac.octet[2] & (1<<5))>>5) ^
        ((mac.octet[1] & (1<<3))>>3) ^
        ((mac.octet[0] & (1<<1))>>1) ^
        ((mac.octet[0] & (1<<7))>>7) ^
        ((vid & (1<<5))>>5) ^
        ((vid & (1<<11))>>11);

    *pHashValue = 
        (index[5] << 5) | (index[4] << 4) | (index[3] << 3) | (index[2] << 2) |
        (index[1] << 1) | (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_rtl9601b_l3GipHash(rtk_ip_addr_t gip, uint32 *pHashValue)
{
    unsigned char index[6];
    unsigned char gip_addr[4] = { ((unsigned char *)(&gip))[3], ((unsigned char *)(&gip))[2], ((unsigned char *)(&gip))[1], ((unsigned char *)(&gip))[0] };

    index[0] = 
        ((gip_addr[3] & (1<<0))>>0) ^
        ((gip_addr[3] & (1<<6))>>6) ^
        ((gip_addr[2] & (1<<4))>>4) ^
        ((gip_addr[1] & (1<<2))>>2) ^
        ((gip_addr[0] & (1<<0))>>0);
    index[1] = 
        ((gip_addr[3] & (1<<1))>>1) ^
        ((gip_addr[3] & (1<<7))>>7) ^
        ((gip_addr[2] & (1<<5))>>5) ^
        ((gip_addr[1] & (1<<3))>>3) ^
        ((gip_addr[0] & (1<<1))>>1);
    index[2] = 
        ((gip_addr[3] & (1<<2))>>2) ^
        ((gip_addr[2] & (1<<0))>>0) ^
        ((gip_addr[2] & (1<<6))>>6) ^
        ((gip_addr[1] & (1<<4))>>4) ^
        ((gip_addr[0] & (1<<2))>>2);
    index[3] = 
        ((gip_addr[3] & (1<<3))>>3) ^
        ((gip_addr[2] & (1<<1))>>1) ^
        ((gip_addr[2] & (1<<7))>>7) ^
        ((gip_addr[1] & (1<<5))>>5) ^
        ((gip_addr[0] & (1<<3))>>3);
    index[4] = 
        ((gip_addr[3] & (1<<4))>>4) ^
        ((gip_addr[2] & (1<<2))>>2) ^
        ((gip_addr[1] & (1<<0))>>0) ^
        ((gip_addr[1] & (1<<6))>>6);
    index[5] = 
        ((gip_addr[3] & (1<<5))>>5) ^
        ((gip_addr[2] & (1<<3))>>3) ^
        ((gip_addr[1] & (1<<1))>>1) ^
        ((gip_addr[1] & (1<<7))>>7);

    *pHashValue = 
        (index[5] << 5) | (index[4] << 4) | (index[3] << 3) | (index[2] << 2) |
        (index[1] << 1) | (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_rtl9601b_l3GipVidHash(rtk_ip_addr_t gip, uint32 vid, uint32 *pHashValue)
{
    unsigned char index[6];
    unsigned char gip_addr[4] = { ((unsigned char *)(&gip))[3], ((unsigned char *)(&gip))[2], ((unsigned char *)(&gip))[1], ((unsigned char *)(&gip))[0] };

    index[0] = 
        ((gip_addr[3] & (1<<0))>>0) ^
        ((gip_addr[3] & (1<<6))>>6) ^
        ((gip_addr[2] & (1<<4))>>4) ^
        ((gip_addr[1] & (1<<2))>>2) ^
        ((gip_addr[0] & (1<<0))>>0) ^
        ((vid & (1<<0))>>0) ^
        ((vid & (1<<6))>>6);
    index[1] = 
        ((gip_addr[3] & (1<<1))>>1) ^
        ((gip_addr[3] & (1<<7))>>7) ^
        ((gip_addr[2] & (1<<5))>>5) ^
        ((gip_addr[1] & (1<<3))>>3) ^
        ((gip_addr[0] & (1<<1))>>1) ^
        ((vid & (1<<1))>>1) ^
        ((vid & (1<<7))>>7);
    index[2] = 
        ((gip_addr[3] & (1<<2))>>2) ^
        ((gip_addr[2] & (1<<0))>>0) ^
        ((gip_addr[2] & (1<<6))>>6) ^
        ((gip_addr[1] & (1<<4))>>4) ^
        ((gip_addr[0] & (1<<2))>>2) ^
        ((vid & (1<<2))>>2) ^
        ((vid & (1<<8))>>8);
    index[3] = 
        ((gip_addr[3] & (1<<3))>>3) ^
        ((gip_addr[2] & (1<<1))>>1) ^
        ((gip_addr[2] & (1<<7))>>7) ^
        ((gip_addr[1] & (1<<5))>>5) ^
        ((gip_addr[0] & (1<<3))>>3) ^
        ((vid & (1<<3))>>3) ^
        ((vid & (1<<9))>>9);
    index[4] = 
        ((gip_addr[3] & (1<<4))>>4) ^
        ((gip_addr[2] & (1<<2))>>2) ^
        ((gip_addr[1] & (1<<0))>>0) ^
        ((gip_addr[1] & (1<<6))>>6) ^
        ((vid & (1<<4))>>4) ^
        ((vid & (1<<10))>>10);
    index[5] = 
        ((gip_addr[3] & (1<<5))>>5) ^
        ((gip_addr[2] & (1<<3))>>3) ^
        ((gip_addr[1] & (1<<1))>>1) ^
        ((gip_addr[1] & (1<<7))>>7) ^
        ((vid & (1<<5))>>5) ^
        ((vid & (1<<11))>>11);

    *pHashValue = 
        (index[5] << 5) | (index[4] << 4) | (index[3] << 3) | (index[2] << 2) |
        (index[1] << 1) | (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_rtl9601b_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
dal_rtl9601b_l2_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_portmask_t all_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    l2_init = INIT_COMPLETED;

    if((ret = dal_rtl9601b_l2_flushLinkDownPortAddrEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_table_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl9601b_l2_portLimitLearningCnt_set(port, HAL_L2_LEARN_LIMIT_CNT_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9601b_l2_portLimitLearningCntAction_set(port, LIMIT_LEARN_CNT_ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9601b_l2_portAgingEnable_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9601b_l2_newMacOp_set(port, HARDWARE_LEARNING, ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

    }

    if((ret = dal_rtl9601b_l2_aging_set(RTK_L2_DEFAULT_AGING_TIME)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_lookupMissAction_set(DLF_TYPE_IPMC, ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_lookupMissAction_set(DLF_TYPE_MCAST, ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_lookupMissAction_set(DLF_TYPE_UCAST, ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_GET_ALL_PORTMASK(all_portmask);
    if((ret = dal_rtl9601b_l2_lookupMissFloodPortMask_set(DLF_TYPE_IPMC, &all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST, &all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST, &all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_l2_ipmcMode_set(LOOKUP_ON_MAC_AND_VID_FID)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = rtl9601b_raw_l2_unknReservedMcFlood_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }


    if((ret = dal_rtl9601b_l2_srcPortEgrFilterMask_set(&all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_init */

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      dal_rtl9601b_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9601b_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = rtl9601b_raw_l2_flushLinkDownPortAddrEnable_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_flushLinkDownPortAddrEnable_get */


/* Function Name:
 *      dal_rtl9601b_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration.
 * Input:
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9601b_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = rtl9601b_raw_l2_flushLinkDownPortAddrEnable_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_flushLinkDownPortAddrEnable_set */

/* Function Name:
 *      dal_rtl9601b_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_ucastAddr_flush(rtk_l2_flushCfg_t *pConfig)
{
    int32   ret = RT_ERR_OK;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(pConfig->port), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushAddrOnAllPorts >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* Error Configuration Check: must specify port(s). */
    if( (DISABLED == pConfig->flushByPort) && (DISABLED == pConfig->flushAddrOnAllPorts) )
        return RT_ERR_INPUT;


    /* Start Flushing */
    if(ENABLED == pConfig->flushByPort)
    {
        if((ret = rtl9601b_raw_l2_flushEn_set(pConfig->port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        }
    }
    else if(ENABLED == pConfig->flushAddrOnAllPorts)
    {
        HAL_SCAN_ALL_PORT(port)
        {
            if((ret = rtl9601b_raw_l2_flushEn_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                break;
            }
        }
    }
    return ret;
} /* end of dal_rtl9601b_l2_ucastAddr_flush */

/* Function Name:
 *      dal_rtl9601b_l2_table_clear
 * Description:
 *      Clear entire L2 table.
 *      All the entries (static and dynamic) (L2 and L3) will be deleted.
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_table_clear(void)
{
    int32   ret;
    uint32  index = 0;
    rtk_enable_t state;
    rtl9601b_lut_table_t lut_entry;

    if ((ret = rtl9601b_raw_l2_camEnable_get(&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    do
    {
        osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
        lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_NEXT_ADDRESS;
        lut_entry.address = index;

        if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        if(0 == lut_entry.lookup_hit)
        {
            /* table empty */
            break;
        }

        if(lut_entry.address < index)
        {
            /* all table clear and hit new-learn entry */
            break;
        }

        /* Delete this entry */
        lut_entry.is_static = DISABLED;
        if(lut_entry.l3lookup == 0)
        {
            /* L2 */
            if((lut_entry.mac.octet[0] & 0x01) == 0)
            {
                /* Unicast */
                lut_entry.age        = 0;
            }
            else
            {
                /* Multicast */
                lut_entry.mbr = 0x00;
            }
        }
        else
        {
            /* L3 */
            lut_entry.l3lookup = ENABLED;
        }

        if ((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        /* Assign new index */
        index = lut_entry.address + 1;

        if(ENABLED == state)
        {
            if(index >= (RTL9601B_LUT_4WAY_NO + RTL9601B_LUT_CAM_NO))
                break;
        }
        else
        {
            if(index >= RTL9601B_LUT_4WAY_NO)
                break;
        }
    }
    while(1);

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_l2_table_clear */

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      dal_rtl9601b_l2_portLimitLearningOverStatus_get
 * Description:
 *      Get the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_rtl9601b_l2_portLimitLearningOverStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9601B_L2_LRN_OVER_STSr, (uint32)port, REG_ARRAY_INDEX_NONE, RTL9601B_LRN_OVER_INDf, pStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9601b_l2_portLimitLearningOverStatus_clear
 * Description:
 *      Clear the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
dal_rtl9601b_l2_portLimitLearningOverStatus_clear(rtk_port_t port)
{
    int32   ret;
    uint32 regData = 1;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if((ret = reg_array_field_write(RTL9601B_L2_LRN_OVER_STSr, (uint32)port, REG_ARRAY_INDEX_NONE, RTL9601B_LRN_OVER_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9601b_l2_portLearningCnt_get
 * Description:
 *      Get the mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt  - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_rtl9601b_l2_portLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    if((ret = rtl9601b_raw_l2_lrnCnt_get(port, pMacCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portLearningCnt_get */



/* Function Name:
 *      dal_rtl9601b_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
dal_rtl9601b_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    if((ret = rtl9601b_raw_l2_lrnLimitNo_get(port, pMacCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portLimitLearningCnt_get */


/* Function Name:
 *      dal_rtl9601b_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
int32
dal_rtl9601b_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX() < macCnt), RT_ERR_INPUT);

    if((ret = rtl9601b_raw_l2_lrnLimitNo_set(port, macCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portLimitLearningCnt_set */


/* Function Name:
 *      dal_rtl9601b_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      port    - port id
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
dal_rtl9601b_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    int32   ret;
    rtk_action_t act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    if((ret = rtl9601b_raw_l2_learnOverAct_get(port, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    switch(act)
    {
        case ACTION_FORWARD:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_FORWARD;
            break;
        case ACTION_DROP:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_DROP;
            break;
        case ACTION_TRAP2CPU:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_TO_CPU;
            break;
        case ACTION_COPY2CPU:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_COPY_CPU;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      dal_rtl9601b_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      port   - port id
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
dal_rtl9601b_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t learningAction)
{

    int32   ret;
    rtk_action_t act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LIMIT_LEARN_CNT_ACTION_END <= learningAction), RT_ERR_INPUT);

    switch(learningAction)
    {
        case LIMIT_LEARN_CNT_ACTION_FORWARD:
            act = ACTION_FORWARD;
            break;
        case LIMIT_LEARN_CNT_ACTION_DROP:
            act = ACTION_DROP;
            break;
        case LIMIT_LEARN_CNT_ACTION_TO_CPU:
            act = ACTION_TRAP2CPU;
            break;
        case LIMIT_LEARN_CNT_ACTION_COPY_CPU:
            act = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    if((ret = rtl9601b_raw_l2_learnOverAct_set(port, act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portLimitLearningCntAction_set */



/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */

/* Function Name:
 *      dal_rtl9601b_l2_aging_get
 * Description:
 *      Get the dynamic address aging time.
 * Input:
 *      None
 * Output:
 *      pAgingTime - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (0.1sec)
 */
int32
dal_rtl9601b_l2_aging_get(uint32 *pAgingTime)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

    if((ret = rtl9601b_raw_l2_agingTime_get(pAgingTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_aging_get */


/* Function Name:
 *      dal_rtl9601b_l2_aging_set
 * Description:
 *      Set the dynamic address aging time.
 * Input:
 *      agingTime - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) RTL8329/RTL8389 aging time is not configurable.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 */
int32
dal_rtl9601b_l2_aging_set(uint32 agingTime)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L2_AGING_TIME_MAX() < agingTime), RT_ERR_INPUT);

    if((ret = rtl9601b_raw_l2_agingTime_set(agingTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_aging_set */


/* Function Name:
 *      dal_rtl9601b_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out setting of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of Age
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = rtl9601b_raw_l2_agingEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portAgingEnable_get */

/* Function Name:
 *      dal_rtl9601b_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      port    - port id
 *      enable  - enable status of Age
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = rtl9601b_raw_l2_agingEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_portAgingEnable_set */

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      dal_rtl9601b_l2_lookupMissAction_get
 * Description:
 *      Get forwarding action when destination address lookup miss.
 * Input:
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
int32
dal_rtl9601b_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case DLF_TYPE_IPMC:
            if((ret = rtl9601b_raw_unkn_ip4Mc_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_IP6MC:
            if((ret = rtl9601b_raw_unkn_ip6Mc_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if((ret = rtl9601b_raw_l2_unkn_ucDaCtl_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_MCAST:
            if((ret = rtl9601b_raw_unkn_l2Mc_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_lookupMissAction_get */

/* Function Name:
 *      dal_rtl9601b_l2_lookupMissAction_set
 * Description:
 *      Set forwarding action when destination address lookup miss.
 * Input:
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
int32
dal_rtl9601b_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32   ret;
    rtk_action_t raw_act;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= action), RT_ERR_INPUT);

    switch (action)
    {
        case ACTION_FORWARD:
        case ACTION_DROP:
        case ACTION_TRAP2CPU:
            raw_act = action;
            break;
        case ACTION_DROP_EXCLUDE_RMA:
            if(DLF_TYPE_MCAST != type)
                return RT_ERR_CHIP_NOT_SUPPORTED;
            else
                raw_act = action;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch(type)
    {
        case DLF_TYPE_IPMC:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_unkn_ip4Mc_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_IP6MC:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_unkn_ip6Mc_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_UCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_l2_unkn_ucDaCtl_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_MCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_unkn_l2Mc_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_lookupMissAction_set */

/* Function Name:
 *      dal_rtl9601b_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
dal_rtl9601b_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case DLF_TYPE_IPMC:
            if((ret = rtl9601b_raw_unkn_ip4Mc_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_IP6MC:
            if((ret = rtl9601b_raw_unkn_ip6Mc_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if((ret = rtl9601b_raw_l2_unkn_ucDaCtl_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_MCAST:
            if((ret = rtl9601b_raw_unkn_l2Mc_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9601b_l2_portLookupMissAction_get */

/* Function Name:
 *      dal_rtl9601b_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
dal_rtl9601b_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32   ret;
    rtk_action_t raw_act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    switch (action)
    {
        case ACTION_FORWARD:
        case ACTION_DROP:
        case ACTION_TRAP2CPU:
            raw_act = action;
            break;
        case ACTION_DROP_EXCLUDE_RMA:
            if(type != DLF_TYPE_MCAST)
                return RT_ERR_CHIP_NOT_SUPPORTED;
            else
                raw_act = action;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    switch(type)
    {
        case DLF_TYPE_IPMC:
            if((ret = rtl9601b_raw_unkn_ip4Mc_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_IP6MC:
            if((ret = rtl9601b_raw_unkn_ip6Mc_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if((ret = rtl9601b_raw_l2_unkn_ucDaCtl_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_MCAST:
            if((ret = rtl9601b_raw_unkn_l2Mc_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9601b_l2_portLookupMissAction_set */

/* Function Name:
 *      dal_rtl9601b_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type   - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
dal_rtl9601b_l2_lookupMissFloodPortMask_get(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    rtk_port_t port;
    rtk_enable_t state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlood_portmask), RT_ERR_NULL_POINTER);

    /* Clear Portmask */
    RTK_PORTMASK_RESET((*pFlood_portmask));

    switch(type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_l2_unknMcFlood_get(port, &state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                if(ENABLED == state)
                    RTK_PORTMASK_PORT_SET((*pFlood_portmask), port);
            }
            break;
        case DLF_TYPE_UCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_l2_unknUcFlood_get(port, &state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                if(ENABLED == state)
                    RTK_PORTMASK_PORT_SET((*pFlood_portmask), port);
            }
            break;
        case DLF_TYPE_BCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = rtl9601b_raw_l2_bcFlood_get(port, &state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                if(ENABLED == state)
                    RTK_PORTMASK_PORT_SET((*pFlood_portmask), port);
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_lookupMissFloodPortMask_get */

/* Function Name:
 *      dal_rtl9601b_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
dal_rtl9601b_l2_lookupMissFloodPortMask_set(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    rtk_port_t port;
    rtk_enable_t state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pFlood_portmask)), RT_ERR_PORT_MASK);

    switch(type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                state = (RTK_PORTMASK_IS_PORT_SET((*pFlood_portmask), port)) ? ENABLED : DISABLED;

                if((ret = rtl9601b_raw_l2_unknMcFlood_set(port, state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_UCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                state = (RTK_PORTMASK_IS_PORT_SET((*pFlood_portmask), port)) ? ENABLED : DISABLED;

                if((ret = rtl9601b_raw_l2_unknUcFlood_set(port, state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_BCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                state = (RTK_PORTMASK_IS_PORT_SET((*pFlood_portmask), port)) ? ENABLED : DISABLED;

                if((ret = rtl9601b_raw_l2_bcFlood_set(port, state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_lookupMissFloodPortMask_set */

/* Function Name:
 *      dal_rtl9601b_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
dal_rtl9601b_l2_lookupMissFloodPortMask_add(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(flood_port), RT_ERR_PORT_ID);

    switch (type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            if ((ret = rtl9601b_raw_l2_unknMcFlood_set(flood_port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if ((ret = rtl9601b_raw_l2_unknUcFlood_set(flood_port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_BCAST:
            if ((ret = rtl9601b_raw_l2_bcFlood_set(flood_port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_lookupMissFloodPortMask_add */

/* Function Name:
 *      dal_rtl9601b_l2_lookupMissFloodPortMask_del
 * Description:
 *      Del one port member in flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration..
 */
int32
dal_rtl9601b_l2_lookupMissFloodPortMask_del(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(flood_port), RT_ERR_PORT_ID);


    switch (type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            if ((ret = rtl9601b_raw_l2_unknMcFlood_set(flood_port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if ((ret = rtl9601b_raw_l2_unknUcFlood_set(flood_port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_BCAST:
            if ((ret = rtl9601b_raw_l2_bcFlood_set(flood_port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_lookupMissFloodPortMask_del */

/* Module Name    : L2      */
/* Sub-module Name: Unicast */
/* Function Name:
 *      dal_rtl9601b_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
dal_rtl9601b_l2_newMacOp_get(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLrnMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    if ((ret = rtl9601b_raw_l2_unkn_saCtl_get(port, pFwdAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pLrnMode = HARDWARE_LEARNING;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_newMacOp_get */

/* Function Name:
 *      dal_rtl9601b_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
dal_rtl9601b_l2_newMacOp_set(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LEARNING_MODE_END <= lrnMode), RT_ERR_INPUT);
    RT_PARAM_CHK((HARDWARE_LEARNING != lrnMode), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= fwdAction), RT_ERR_INPUT);

    if ((ret = rtl9601b_raw_l2_unkn_saCtl_set(port, fwdAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_newMacOp_set */


/* Module Name    : L2              */
/* Sub-module Name: Get next entry */

/* Function Name:
 *      dal_rtl9601b_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2UcastData   - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
dal_rtl9601b_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2UcastData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((*pScanIdx >= HAL_L2_LEARN_LIMIT_CNT_MAX()), RT_ERR_L2_INDEXTABLE_INDEX);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UC;
    lut_entry.address = *pScanIdx;
    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2UcastData, 0x00, sizeof(rtk_l2_ucastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoL2uc(pL2UcastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_nextValidAddr_get */


/* Function Name:
 *      dal_rtl9601b_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2UcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
dal_rtl9601b_l2_nextValidAddrOnPort_get(
    rtk_port_t          port,
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2UcastData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((*pScanIdx >= HAL_L2_LEARN_LIMIT_CNT_MAX()), RT_ERR_L2_INDEXTABLE_INDEX);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UCSPA;
    lut_entry.address = *pScanIdx;
    lut_entry.spa = (uint32)port;
    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2UcastData, 0x00, sizeof(rtk_l2_ucastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoL2uc(pL2UcastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_nextValidAddrOnPort_get */


/* Function Name:
 *      dal_rtl9601b_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pL2McastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          reply next valid L2 multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
dal_rtl9601b_l2_nextValidMcastAddr_get(
    int32               *pScanIdx,
    rtk_l2_mcastAddr_t  *pL2McastData)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2McastData), RT_ERR_NULL_POINTER);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2MC;
    lut_entry.address = *pScanIdx;
    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2McastData, 0x00, sizeof(rtk_l2_mcastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoL2mc(pL2McastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_nextValidMcastAddr_get */

/* Function Name:
 *      dal_rtl9601b_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pIpMcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and multicast entry and
 *          reply next valid L2 ip multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
dal_rtl9601b_l2_nextValidIpMcastAddr_get(
    int32                   *pScanIdx,
    rtk_l2_ipMcastAddr_t    *pIpMcastData)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpMcastData), RT_ERR_NULL_POINTER);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_NEXT_L3MC;
    lut_entry.address = *pScanIdx;
    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pIpMcastData, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoIpmc(pIpMcastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_nextValidIpMcastAddr_get */



/* Function Name:
 *      dal_rtl9601b_l2_nextValidEntry_get
 * Description:
 *      Get LUT next valid entry.
 * Input:
 *      pScanIdx - Index field in the structure.
 * Output:
 *      pL2Entry - entry content
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get next valid LUT entry.
 */
int32
dal_rtl9601b_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2Entry), RT_ERR_NULL_POINTER);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    _dal_rtl9601b_lutReadMethodtoRaw(&lut_entry.method,pL2Entry->method);
    lut_entry.address = *pScanIdx;
    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

	if(0 == lut_entry.lookup_hit)
	{
        return RT_ERR_L2_ENTRY_NOTFOUND;
	}
    osal_memset(pL2Entry, 0x00, sizeof(rtk_l2_addr_table_t));

	if(pL2Entry->entryType == RTK_LUT_END)
	{
		pL2Entry->entryType = RTK_LUT_L2UC;
        if ((ret = _dal_rtl9601b_rawtoL2uc(&(pL2Entry->entry.l2UcEntry), &lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
		pL2Entry->entryType = RTK_LUT_L2MC;
        if ((ret = _dal_rtl9601b_rawtoL2mc(&(pL2Entry->entry.l2McEntry), &lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
		pL2Entry->entryType = RTK_LUT_L3MC;
        if ((ret = _dal_rtl9601b_rawtoIpmc(&(pL2Entry->entry.ipmcEntry), &lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
	}
    else if(lut_entry.l3lookup == 0)
    {
        if((lut_entry.mac.octet[0] & 0x01) == 0)
        {
            pL2Entry->entryType = RTK_LUT_L2UC;
            if ((ret = _dal_rtl9601b_rawtoL2uc(&(pL2Entry->entry.l2UcEntry), &lut_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            pL2Entry->entryType = RTK_LUT_L2MC;
            if ((ret = _dal_rtl9601b_rawtoL2mc(&(pL2Entry->entry.l2McEntry), &lut_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
        }
    }
    else
    {
        pL2Entry->entryType = RTK_LUT_L3MC;
        if ((ret = _dal_rtl9601b_rawtoIpmc(&(pL2Entry->entry.ipmcEntry), &lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
    }

    *pScanIdx = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_nextValidEntry_get */



/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      dal_rtl9601b_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
dal_rtl9601b_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(pL2Addr->port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK((pL2Addr->age > HAL_L2_ENTRY_AGING_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((pL2Addr->ctag_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x00, sizeof(rtl9601b_lut_table_t));

    if ((ret = _dal_rtl9601b_l2uctoRaw(&lut_entry, pL2Addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC;
    lut_entry.valid = ENABLED;
    if ((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_FAILED;

    pL2Addr->index = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_addr_add */

/* Function Name:
 *      dal_rtl9601b_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
dal_rtl9601b_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    if(pL2Addr->flags & RTK_L2_UCAST_FLAG_IVL){

        RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    }

    osal_memset(&lut_entry, 0x00, sizeof(rtl9601b_lut_table_t));

    if ((ret = _dal_rtl9601b_l2uctoRaw(&lut_entry, pL2Addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC;
    lut_entry.is_static  = DISABLED;
    lut_entry.age        = 0;
    lut_entry.valid      = DISABLED;
    if ((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_addr_del */


/* Function Name:
 *      dal_rtl9601b_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
dal_rtl9601b_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK((pL2Addr->efid > HAL_ENHANCED_FID_MAX()), RT_ERR_L2_EFID);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    if(pL2Addr->flags & RTK_L2_UCAST_FLAG_IVL) {
        RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    }

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if ((ret = _dal_rtl9601b_l2uctoRaw(&lut_entry, pL2Addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_MAC;
    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC;
    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoL2uc(pL2Addr, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_addr_get */


/* Function Name:
 *      dal_rtl9601b_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry.
 * Input:
 *      includeStatic - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_addr_delAll(uint32 includeStatic)
{
    int32   ret;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((includeStatic!=TRUE), RT_ERR_CHIP_NOT_SUPPORTED);

    /* Start Flushing */
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = rtl9601b_raw_l2_flushEn_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_addr_delAll */

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      dal_rtl9601b_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
int32
dal_rtl9601b_l2_mcastAddr_add(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pMcastAddr->mac.octet[0] & 0x01) != 1), RT_ERR_MAC);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(pMcastAddr->portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pMcastAddr->flags > RTK_L2_MCAST_FLAG_ALL), RT_ERR_INPUT);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if((ret = _dal_rtl9601b_l2mctoRaw(&lut_entry, pMcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
    lut_entry.is_static = ENABLED;
    lut_entry.valid = ENABLED;
    if((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_FAILED;

    pMcastAddr->index = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_mcastAddr_add */

/* Function Name:
 *      dal_rtl9601b_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_mcastAddr_del(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pMcastAddr->mac.octet[0] & 0x01) != 1), RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->flags > RTK_L2_MCAST_FLAG_ALL), RT_ERR_INPUT);

	if(pMcastAddr->flags & RTK_L2_MCAST_FLAG_IVL){
        RT_PARAM_CHK((pMcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    }

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if((ret = _dal_rtl9601b_l2mctoRaw(&lut_entry, pMcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
    lut_entry.mbr = 0x00;
    lut_entry.is_static = DISABLED;
    lut_entry.valid = DISABLED;
    if((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_mcastAddr_del */

/* Function Name:
 *      dal_rtl9601b_l2_mcastAddr_get
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_mcastAddr_get(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pMcastAddr->mac.octet[0] & 0x01) != 1), RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->flags > RTK_L2_MCAST_FLAG_ALL), RT_ERR_INPUT);
    if(pMcastAddr->flags & RTK_L2_MCAST_FLAG_IVL){
        RT_PARAM_CHK((pMcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
	}

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if ((ret = _dal_rtl9601b_l2mctoRaw(&lut_entry, pMcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
    lut_entry.method = RTL9601B_RAW_LUT_READ_METHOD_MAC;

    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pMcastAddr, 0x00, sizeof(rtk_l2_mcastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoL2mc(pMcastAddr, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_mcastAddr_get */


/* Module Name    : L2           */
/* Sub-module Name: IP multicast */


/* Function Name:
 *      dal_rtl9601b_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DPI_AND_VID
 */
int32
dal_rtl9601b_l2_ipmcMode_get(rtk_l2_ipmcMode_t *pMode)
{
    int32   ret;
    rtl9601b_raw_l2_ipMcHashType_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = rtl9601b_raw_l2_ipmcHashType_get(&raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    switch(raw_mode)
    {
        case RTL9601B_RAW_LUT_IPMCHASH_TYPE_DMACFID:
            *pMode = LOOKUP_ON_MAC_AND_VID_FID;
            break;

        case RTL9601B_RAW_LUT_IPMCHASH_TYPE_DIP:
            *pMode = LOOKUP_ON_DIP;
            break;

        case RTL9601B_RAW_LUT_IPMCHASH_TYPE_GIPVID:
            *pMode = LOOKUP_ON_DIP_AND_VID;
            break;
		case RTL9601B_RAW_LUT_IPMCHASH_TYPE_GIPCVID:
			*pMode = LOOKUP_ON_DIP_AND_CVID;
			break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_ipmcMode_get */

/* Function Name:
 *      dal_rtl9601b_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DPI_AND_VID
 */
int32
dal_rtl9601b_l2_ipmcMode_set(rtk_l2_ipmcMode_t mode)
{
    int32   ret;
    rtl9601b_raw_l2_ipMcHashType_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((IPMC_MODE_END <= mode), RT_ERR_INPUT);

    switch(mode)
    {
        case LOOKUP_ON_MAC_AND_VID_FID:
            raw_mode = RTL9601B_RAW_LUT_IPMCHASH_TYPE_DMACFID;
            break;
        case LOOKUP_ON_DIP:
            raw_mode = RTL9601B_RAW_LUT_IPMCHASH_TYPE_DIP;
            break;
        case LOOKUP_ON_DIP_AND_VID:
            raw_mode = RTL9601B_RAW_LUT_IPMCHASH_TYPE_GIPVID;
            break;
		case LOOKUP_ON_DIP_AND_CVID:
			raw_mode = RTL9601B_RAW_LUT_IPMCHASH_TYPE_GIPCVID;
			break;
        default:
            return RT_ERR_CHIP_NOT_FOUND;
            break;
    }

    if ((ret = rtl9601b_raw_l2_ipmcHashType_set(raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_ipmcMode_set */



/* Function Name:
 *      dal_rtl9601b_l2_portIpmcAction_get
 * Description:
 *      Get the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 * Output:
 *      pAction     - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_portIpmcAction_get(rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = rtl9601b_raw_l2_ipmcAction_get(port, pAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_l2_portIpmcAction_get */

/* Function Name:
 *      dal_rtl9601b_l2_portIpmcAction_set
 * Description:
 *      Set the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 *      action      - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_portIpmcAction_set(rtk_port_t port, rtk_action_t action)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2), "port=%d,action=%d",port, action);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(( (ACTION_FORWARD != action) && (ACTION_DROP != action) ), RT_ERR_INPUT);

    /* function body */
    if((ret = rtl9601b_raw_l2_ipmcAction_set(port, action)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_l2_portIpmcAction_set */

/* Function Name:
 *      dal_rtl9601b_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_IPV4_ADDRESS  - Invalid IPv4 address
 *      RT_ERR_VLAN_VID      - invalid vlan id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
dal_rtl9601b_l2_ipMcastAddr_add(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;
    rtk_l2_ipmcMode_t   mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(( (pIpmcastAddr->dip > 0xEFFFFFFF) || (pIpmcastAddr->dip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(pIpmcastAddr->portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pIpmcastAddr->flags > RTK_L2_IPMCAST_FLAG_ALL), RT_ERR_PORT_MASK);

    if((ret = dal_rtl9601b_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(LOOKUP_ON_DIP_AND_VID == mode)
        RT_PARAM_CHK((pIpmcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if ((ret = _dal_rtl9601b_IpmctoRaw(&lut_entry, pIpmcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    lut_entry.l3lookup = ENABLED;
    lut_entry.is_static = ENABLED;
    lut_entry.valid = ENABLED;

    if ((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_FAILED;

    pIpmcastAddr->index = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_ipMcastAddr_add */

/* Function Name:
 *      dal_rtl9601b_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      pIpmcastAddr  - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      (1) In vlan unaware mode (SVL), the vid will be ignore, suggest to
 *          input vid=0 in vlan unaware mode.
 *      (2) In vlan aware mode (IVL), the vid will be care.
 */
int32
dal_rtl9601b_l2_ipMcastAddr_del(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;
    rtk_l2_ipmcMode_t   mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(( (pIpmcastAddr->dip > 0xEFFFFFFF) || (pIpmcastAddr->dip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK((pIpmcastAddr->flags > RTK_L2_IPMCAST_FLAG_ALL), RT_ERR_PORT_MASK);

    if((ret = dal_rtl9601b_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(LOOKUP_ON_DIP_AND_VID == mode)
        RT_PARAM_CHK((pIpmcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if ((ret = _dal_rtl9601b_IpmctoRaw(&lut_entry, pIpmcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    lut_entry.is_static= DISABLED;
    lut_entry.l3lookup = ENABLED;
    lut_entry.mbr = 0x0;
    lut_entry.valid = DISABLED;

    if ((ret = rtl9601b_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_ipMcastAddr_del */

/* Function Name:
 *      dal_rtl9601b_l2_ipMcastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
dal_rtl9601b_l2_ipMcastAddr_get(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    rtl9601b_lut_table_t lut_entry;
    rtk_l2_ipmcMode_t   mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(( (pIpmcastAddr->dip > 0xEFFFFFFF) || (pIpmcastAddr->dip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK((pIpmcastAddr->flags > RTK_L2_IPMCAST_FLAG_ALL), RT_ERR_PORT_MASK);

    if((ret = dal_rtl9601b_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(LOOKUP_ON_DIP_AND_VID == mode)
        RT_PARAM_CHK((pIpmcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x0, sizeof(rtl9601b_lut_table_t));
    if ((ret = _dal_rtl9601b_IpmctoRaw(&lut_entry, pIpmcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    lut_entry.l3lookup = ENABLED;

    if ((ret = rtl9601b_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pIpmcastAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    if ((ret = _dal_rtl9601b_rawtoIpmc(pIpmcastAddr, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_ipMcastAddr_get */


/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */


/* Function Name:
 *      dal_rtl9601b_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
dal_rtl9601b_l2_srcPortEgrFilterMask_get(rtk_portmask_t *pFilter_portmask)
{
    int32   ret;
    rtk_port_t port;
    uint32 permit;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter_portmask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET((*pFilter_portmask));

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = reg_array_field_read(RTL9601B_L2_SRC_PORT_PERMITr, (uint32)port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &permit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }

        if(DISABLED == permit)
            RTK_PORTMASK_PORT_SET((*pFilter_portmask), port);
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_srcPortEgrFilterMask_get */

/* Function Name:
 *      dal_rtl9601b_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
dal_rtl9601b_l2_srcPortEgrFilterMask_set(rtk_portmask_t *pFilter_portmask)
{
    int32   ret;
    rtk_port_t port;
    uint32 permit;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pFilter_portmask), RT_ERR_INPUT);

    HAL_SCAN_ALL_PORT(port)
    {
        if(RTK_PORTMASK_IS_PORT_SET((*pFilter_portmask), port))
            permit = DISABLED;
        else
            permit = ENABLED;

        if ((ret = reg_array_field_write(RTL9601B_L2_SRC_PORT_PERMITr, (uint32)port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &permit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_srcPortEgrFilterMask_set */



/* Function Name:
 *      dal_rtl9601b_l2_camState_set
 * Description:
 *      Set LUT cam state
 * Input:
 *      camState - enable or disable cam state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_rtl9601b_l2_camState_set(rtk_enable_t camState)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    if ((ret = rtl9601b_raw_l2_camEnable_set(camState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_srcPortEgrFilterMask_set */


/* Function Name:
 *      dal_rtl9601b_l2_camState_get
 * Description:
 *      Get LUT cam state
 * Input:
 *      pCamState - status of cam state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_rtl9601b_l2_camState_get(rtk_enable_t *pCamState)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    if ((ret = rtl9601b_raw_l2_camEnable_get(pCamState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_srcPortEgrFilterMask_set */

/* Function Name:
 *      dal_rtl9601b_l2_hashValue_get
 * Description:
 *      Calculate the hash value for specific hash type
 * Input:
 *      type        - hash type
 *      hashData    - input data to be hashed
 * Output:
 *      pHashValue  - hash result of specified type with data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
dal_rtl9601b_l2_hashValue_get(rtk_l2_hashType_t type, rtk_l2_hashData_t hashData, uint32 *pHashValue)
{
	int32	ret;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

	/* Software only API, don't check Init status */

	/* parameter check */
	RT_PARAM_CHK((type >= LUT_HASHTYPE_END), RT_ERR_INPUT);
	RT_PARAM_CHK((NULL == pHashValue), RT_ERR_NULL_POINTER);

	switch(type)
	{
        case LUT_HASHTYPE_L2_UCSVL:
        case LUT_HASHTYPE_L2_MCSVL:
        case LUT_HASHTYPE_L2_MC:
            RT_PARAM_CHK(!RTL9601B_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_MAC), RT_ERR_INPUT);
            if((ret = _dal_rtl9601b_l2MacHash(hashData.mac, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L2_UCIVL:
        case LUT_HASHTYPE_L2_MCIVL:
            RT_PARAM_CHK(!RTL9601B_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_MAC | LUT_HASHFLAG_VID), RT_ERR_INPUT);
            if((ret = _dal_rtl9601b_l2MacIvlHash(hashData.mac, hashData.vid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L3_GIPFID:
        case LUT_HASHTYPE_L3_GIP:
            RT_PARAM_CHK(!RTL9601B_HASH_FLAG_CHECK(hashData.dataFlags, 
                LUT_HASHFLAG_GIP), RT_ERR_INPUT);
            if((ret = _dal_rtl9601b_l3GipHash(hashData.gip, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L3_GIPVID:
            RT_PARAM_CHK(!RTL9601B_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_GIP | LUT_HASHFLAG_VID), RT_ERR_INPUT);
            if((ret = _dal_rtl9601b_l3GipVidHash(hashData.gip, hashData.vid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;
} /* end of dal_rtl9601b_l2_hashValue_get */

