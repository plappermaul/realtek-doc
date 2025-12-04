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
 * Purpose : Definition those public Trap APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Packets trap to CPU setting.
 *            2) RMA (Reserved MAC address).
 *
 */

#include <dal/apollomp/raw/apollomp_raw_trap.h>

/*
 * Symbol Definition
 */
#define     RAW_APOLLOMP_RMA_ADDR_PREFIX_LEN         5

/*
 * Data Declaration
 */
#if 0
/* prefix of reserve multicast address */
const static uint8 raw_rma_prefix[RAW_APOLLOMP_RMA_ADDR_PREFIX_LEN] = {0x01, 0x80, 0xC2, 0x00, 0x00};
const static uint8 raw_rma_cisco_prefix[RAW_APOLLOMP_RMA_ADDR_PREFIX_LEN] = {0x01, 0x00, 0x0C, 0xCC, 0xCC};

/* Function Name:
 *      apollomp_raw_trap_rmaPriority_set
 * Description:
 *      Set RMA trap priority assignment for frames be trapped to CPU port.
 * Input:
 *      priority        - The trap priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_rmaPriority_set(uint32 priority)
{
    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK((priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_field_write(APOLLOMP_RMA_CFGr, APOLLOMP_RMA_TRAP_PRIORITYf, &priority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_trap_rmaPriority_get
 * Description:
 *      Get RMA trap priority assignment for frames be trapped to CPU port.
 * Input:
 *      None
 * Output:
 *      pPriority        - The trap priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_rmaPriority_get(uint32 *pPriority)
{
    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);
    if ((ret = reg_field_read(APOLLOMP_RMA_CFGr, APOLLOMP_RMA_TRAP_PRIORITYf, pPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_trap_rmaAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 *      action          - RMA action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_RMA_ADDR
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_rmaAction_set(uint32  rmaTail, rtk_action_t action)
{
    int32 ret;
    uint32 regAddr;
	apollomp_raw_rmaAction_t rmaAct;

    /* parameter check */
    RT_PARAM_CHK(rmaTail > APOLLOMP_RMA_L2TAILNUM_MAX, RT_ERR_RMA_ADDR);
    RT_PARAM_CHK(action >= ACTION_END, RT_ERR_OUT_OF_RANGE);

	switch(action)
	{
	 	case ACTION_FORWARD:
	 		rmaAct = APOLLOMP_RMA_ACT_FORWARD;
			break;
	 	case ACTION_TRAP2CPU:
	 		rmaAct = APOLLOMP_RMA_ACT_TRAP;
			break;
	 	case ACTION_DROP:
	 		rmaAct = APOLLOMP_RMA_ACT_DROP;
			break;
	 	case ACTION_FORWARD_EXCLUDE_CPU:
	 		rmaAct = APOLLOMP_RMA_ACT_FORWARD_EXCLUDE_CPU;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	switch(rmaTail)
	{
		case 0x00:
			regAddr = APOLLOMP_RMA_CTRL00r;
			break;
		case 0x01:
			regAddr = APOLLOMP_RMA_CTRL01r;
			break;
		case 0x02:
			regAddr = APOLLOMP_RMA_CTRL02r;
			break;
		case 0x03:
			regAddr = APOLLOMP_RMA_CTRL03r;
			break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x09:
		case 0x0A:
		case 0x0B:
		case 0x0C:
		case 0x0F:
			regAddr = APOLLOMP_RMA_CTRL04r;
			break;
		case 0x08:
			regAddr = APOLLOMP_RMA_CTRL08r;
			break;
		case 0x0D:
			regAddr = APOLLOMP_RMA_CTRL0Dr;
			break;
		case 0x0E:
			regAddr = APOLLOMP_RMA_CTRL0Er;
			break;
		case 0x10:
			regAddr = APOLLOMP_RMA_CTRL10r;
			break;
		case 0x11:
			regAddr = APOLLOMP_RMA_CTRL11r;
			break;
		case 0x12:
			regAddr = APOLLOMP_RMA_CTRL12r;
			break;
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x19:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
			regAddr = APOLLOMP_RMA_CTRL13r;
			break;
		case 0x18:
			regAddr = APOLLOMP_RMA_CTRL18r;
			break;
		case 0x1A:
			regAddr = APOLLOMP_RMA_CTRL1Ar;
			break;
		case 0x20:
			regAddr = APOLLOMP_RMA_CTRL20r;
			break;
		case 0x21:
			regAddr = APOLLOMP_RMA_CTRL21r;
			break;
		case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            regAddr = APOLLOMP_RMA_CTRL22r;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    /* set entry from CHIP*/
    if ((ret = reg_field_write(regAddr, APOLLOMP_OPERATIONf, &rmaAct)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaAction_set */


/* Function Name:
 *      apollomp_raw_trap_rmaAction_get
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 * Output:
 *      pAction       - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_RMA_ADDR
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_rmaAction_get(uint32 rmaTail, rtk_action_t* pAction)
{
    int32 ret;
    uint32 regAddr;
	apollomp_raw_rmaAction_t rmaAct;

    /* parameter check */
    RT_PARAM_CHK((pAction == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(rmaTail > APOLLOMP_RMA_L2TAILNUM_MAX, RT_ERR_RMA_ADDR);


	switch(rmaTail)
	{
		case 0x00:
			regAddr = APOLLOMP_RMA_CTRL00r;
			break;
		case 0x01:
			regAddr = APOLLOMP_RMA_CTRL01r;
			break;
		case 0x02:
			regAddr = APOLLOMP_RMA_CTRL02r;
			break;
		case 0x03:
			regAddr = APOLLOMP_RMA_CTRL03r;
			break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x09:
		case 0x0A:
		case 0x0B:
		case 0x0C:
		case 0x0F:
			regAddr = APOLLOMP_RMA_CTRL04r;
			break;
		case 0x08:
			regAddr = APOLLOMP_RMA_CTRL08r;
			break;
		case 0x0D:
			regAddr = APOLLOMP_RMA_CTRL0Dr;
			break;
		case 0x0E:
			regAddr = APOLLOMP_RMA_CTRL0Er;
			break;
		case 0x10:
			regAddr = APOLLOMP_RMA_CTRL10r;
			break;
		case 0x11:
			regAddr = APOLLOMP_RMA_CTRL11r;
			break;
		case 0x12:
			regAddr = APOLLOMP_RMA_CTRL12r;
			break;
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x19:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
			regAddr = APOLLOMP_RMA_CTRL13r;
			break;
		case 0x18:
			regAddr = APOLLOMP_RMA_CTRL18r;
			break;
		case 0x1A:
			regAddr = APOLLOMP_RMA_CTRL1Ar;
			break;
		case 0x20:
			regAddr = APOLLOMP_RMA_CTRL20r;
			break;
		case 0x21:
			regAddr = APOLLOMP_RMA_CTRL21r;
			break;
		case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
			regAddr = APOLLOMP_RMA_CTRL22r;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}


    if ((ret = reg_field_read(regAddr, APOLLOMP_OPERATIONf, &rmaAct)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

	switch(rmaAct)
	{
	 	case APOLLOMP_RMA_ACT_FORWARD:
	 		*pAction = ACTION_FORWARD;
			break;
	 	case APOLLOMP_RMA_ACT_TRAP:
	 		*pAction = ACTION_TRAP2CPU;
			break;
	 	case APOLLOMP_RMA_ACT_DROP:
	 		*pAction = ACTION_DROP;
			break;
	 	case APOLLOMP_RMA_ACT_FORWARD_EXCLUDE_CPU:
	 		*pAction = ACTION_FORWARD_EXCLUDE_CPU;
			break;
		default:
            return RT_ERR_FAILED;
	}

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaAction_get */

#endif
/* Function Name:
 *      apollomp_raw_trap_rmaFeature_set
 * Description:
 *      Set feature status of reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 *      type      		- feature type
 *      enable      	- enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_RMA_ADDR
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_rmaFeature_set(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t enable)
{
    int32 ret;
    uint32 regAddr;
    uint32 field;
    /* parameter check */
    RT_PARAM_CHK(rmaTail > APOLLOMP_RMA_L2TAILNUM_MAX, RT_ERR_RMA_ADDR);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type >= APOLLOMP_RMA_FUN_END, RT_ERR_OUT_OF_RANGE);
    switch (type)
    {
        case APOLLOMP_RMA_FUN_VLAN_LEAKY:
            field = APOLLOMP_VLAN_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_PISO_LEAKY:
            field = APOLLOMP_PORTISO_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_BYPASS_STORM:
            field = APOLLOMP_DISCARD_STORM_FILTERf;
            break;
        case APOLLOMP_RMA_FUN_KEEP_CTGA_FMT:
            field = APOLLOMP_KEEP_FORMATf;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	switch(rmaTail)
	{
		case 0x00:
			regAddr = APOLLOMP_RMA_CTRL00r;
			break;
		case 0x01:
			regAddr = APOLLOMP_RMA_CTRL01r;
			break;
		case 0x02:
			regAddr = APOLLOMP_RMA_CTRL02r;
			break;
		case 0x03:
			regAddr = APOLLOMP_RMA_CTRL03r;
			break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x09:
		case 0x0A:
		case 0x0B:
		case 0x0C:
		case 0x0F:
			regAddr = APOLLOMP_RMA_CTRL04r;
			break;
		case 0x08:
			regAddr = APOLLOMP_RMA_CTRL08r;
			break;
		case 0x0D:
			regAddr = APOLLOMP_RMA_CTRL0Dr;
			break;
		case 0x0E:
			regAddr = APOLLOMP_RMA_CTRL0Er;
			break;
		case 0x10:
			regAddr = APOLLOMP_RMA_CTRL10r;
			break;
		case 0x11:
			regAddr = APOLLOMP_RMA_CTRL11r;
			break;
		case 0x12:
			regAddr = APOLLOMP_RMA_CTRL12r;
			break;
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x19:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
			regAddr = APOLLOMP_RMA_CTRL13r;
			break;
		case 0x18:
			regAddr = APOLLOMP_RMA_CTRL18r;
			break;
		case 0x1A:
			regAddr = APOLLOMP_RMA_CTRL1Ar;
			break;
		case 0x20:
			regAddr = APOLLOMP_RMA_CTRL20r;
			break;
		case 0x21:
			regAddr = APOLLOMP_RMA_CTRL21r;
			break;
	    case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
			regAddr = APOLLOMP_RMA_CTRL22r;
			break;
		default:
            return RT_ERR_FAILED;
	}

    if ((ret = reg_field_write(regAddr, field, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaFeature_set */

/* Function Name:
 *      apollomp_raw_trap_rmaFeature_get
 * Description:
 *      Get feature status of reserved multicast address(RMA) frame
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 *      type      		- feature type
 * Output:
 *      pEnable      	- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_RMA_ADDR
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_rmaFeature_get(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 regAddr;
    uint32 field;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(rmaTail > APOLLOMP_RMA_L2TAILNUM_MAX, RT_ERR_RMA_ADDR);
    RT_PARAM_CHK(type >= APOLLOMP_RMA_FUN_END, RT_ERR_OUT_OF_RANGE);

    switch (type)
    {
        case APOLLOMP_RMA_FUN_VLAN_LEAKY:
            field = APOLLOMP_VLAN_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_PISO_LEAKY:
            field = APOLLOMP_PORTISO_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_BYPASS_STORM:
            field = APOLLOMP_DISCARD_STORM_FILTERf;
            break;
        case APOLLOMP_RMA_FUN_KEEP_CTGA_FMT:
            field = APOLLOMP_KEEP_FORMATf;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	switch(rmaTail)
	{
		case 0x00:
			regAddr = APOLLOMP_RMA_CTRL00r;
			break;
		case 0x01:
			regAddr = APOLLOMP_RMA_CTRL01r;
			break;
		case 0x02:
			regAddr = APOLLOMP_RMA_CTRL02r;
			break;
		case 0x03:
			regAddr = APOLLOMP_RMA_CTRL03r;
			break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x09:
		case 0x0A:
		case 0x0B:
		case 0x0C:
		case 0x0F:
			regAddr = APOLLOMP_RMA_CTRL04r;
			break;
		case 0x08:
			regAddr = APOLLOMP_RMA_CTRL08r;
			break;
		case 0x0D:
			regAddr = APOLLOMP_RMA_CTRL0Dr;
			break;
		case 0x0E:
			regAddr = APOLLOMP_RMA_CTRL0Er;
			break;
		case 0x10:
			regAddr = APOLLOMP_RMA_CTRL10r;
			break;
		case 0x11:
			regAddr = APOLLOMP_RMA_CTRL11r;
			break;
		case 0x12:
			regAddr = APOLLOMP_RMA_CTRL12r;
			break;
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x19:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
			regAddr = APOLLOMP_RMA_CTRL13r;
			break;
		case 0x18:
			regAddr = APOLLOMP_RMA_CTRL18r;
			break;
		case 0x1A:
			regAddr = APOLLOMP_RMA_CTRL1Ar;
			break;
		case 0x20:
			regAddr = APOLLOMP_RMA_CTRL20r;
			break;
		case 0x21:
			regAddr = APOLLOMP_RMA_CTRL21r;
			break;
		case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
			regAddr = APOLLOMP_RMA_CTRL22r;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    if ((ret = reg_field_read(regAddr, field, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaFeature_get */

/* Function Name:
 *      apollomp_raw_trap_rmaCiscoAction_set
 * Description:
 *      Set action of Cisco reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 *      action         - RMA action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      Support RMA 01-00-0C-CC-CC-CC and 01-00-0C-CC-CC-CD
 */
int32 apollomp_raw_trap_rmaCiscoAction_set(uint32  rmaTail, rtk_action_t action)
{
    int32 ret;
    uint32 regAddr;
	apollomp_raw_rmaAction_t rmaAct;

    /* parameter check */

    switch(action)
	{
	 	case ACTION_FORWARD:
	 		rmaAct = APOLLOMP_RMA_ACT_FORWARD;
			break;
	 	case ACTION_TRAP2CPU:
	 		rmaAct = APOLLOMP_RMA_ACT_TRAP;
			break;
	 	case ACTION_DROP:
	 		rmaAct = APOLLOMP_RMA_ACT_DROP;
			break;
	 	case ACTION_FORWARD_EXCLUDE_CPU:
	 		rmaAct = APOLLOMP_RMA_ACT_FORWARD_EXCLUDE_CPU;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	switch(rmaTail)
	{
		case 0xCC:
			regAddr = APOLLOMP_RMA_CTRL_CDPr;
			break;
		case 0xCD:
			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    /* set entry from CHIP*/
    if ((ret = reg_field_write(regAddr, APOLLOMP_OPERATIONf, &rmaAct)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaCiscoAction_set */

/* Function Name:
 *      apollomp_raw_trap_rmaCiscoAction_get
 * Description:
 *      Get action of Cisco reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 * Output:
 *      pAction        - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_RMA_ADDR
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      Support RMA 01-00-0C-CC-CC-CC and 01-00-0C-CC-CC-CD
 */
int32 apollomp_raw_trap_rmaCiscoAction_get(uint32  rmaTail, rtk_action_t *pAction)
{
    int32 ret;
    uint32 regAddr;
	apollomp_raw_rmaAction_t rmaAct;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

	switch(rmaTail)
	{
		case 0xCC:
			regAddr = APOLLOMP_RMA_CTRL_CDPr;
			break;
		case 0xCD:
			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    /* set entry from CHIP*/
    if ((ret = reg_field_read(regAddr, APOLLOMP_OPERATIONf, &rmaAct)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

	switch(rmaAct)
	{
	 	case APOLLOMP_RMA_ACT_FORWARD:
	 		*pAction = ACTION_FORWARD;
			break;
	 	case APOLLOMP_RMA_ACT_TRAP:
	 		*pAction = ACTION_TRAP2CPU;
			break;
	 	case APOLLOMP_RMA_ACT_DROP:
	 		*pAction = ACTION_DROP;
			break;
	 	case APOLLOMP_RMA_ACT_FORWARD_EXCLUDE_CPU:
	 		*pAction = ACTION_FORWARD_EXCLUDE_CPU;
			break;
		default:
            return RT_ERR_FAILED;
	}

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaCiscoAction_get */

#if 0
/* Function Name:
 *      apollomp_raw_trap_rmaCiscoFeature_set
 * Description:
 *      Set feature status of Cisco reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 *      type      		- feature type
 *      enable      	- enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_INPUT
 *      RT_ERR_RMA_ADDR
 * Note:
 *      Support RMA 01-00-0C-CC-CC-CC and 01-00-0C-CC-CC-CD
 */
int32 apollomp_raw_trap_rmaCiscoFeature_set(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t enable)
{
    int32 ret;
    uint32 regAddr;
    uint32 field;

    /* parameter check */
    RT_PARAM_CHK( ((rmaTail != 0xCC) && (rmaTail != 0xCD)), RT_ERR_RMA_ADDR);
	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_RMA_FUN_END <= type), RT_ERR_INPUT);
    switch (type)
    {
        case APOLLOMP_RMA_FUN_VLAN_LEAKY:
            field = APOLLOMP_VLAN_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_PISO_LEAKY:
            field = APOLLOMP_PORTISO_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_BYPASS_STORM:
            field = APOLLOMP_DISCARD_STORM_FILTERf;
            break;
        case APOLLOMP_RMA_FUN_KEEP_CTGA_FMT:
            field = APOLLOMP_KEEP_FORMATf;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	switch(rmaTail)
	{
		case 0xCC:
			regAddr = APOLLOMP_RMA_CTRL_CDPr;
			break;
		case 0xCD:
			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    if ((ret = reg_field_write(regAddr, field, (uint32 *)&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaCiscoFeature_set */


/* Function Name:
 *      apollomp_raw_trap_rmaCiscoFeature_get
 * Description:
 *      Get feature status of Cisco reserved multicast address(RMA) frame.
 * Input:
 *      rmaTail       - Tail number of reserved multicast address.
 *      type      		- feature type
 * Output:
 *      pEnable      	- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_RMA_ADDR
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      Support RMA 01-00-0C-CC-CC-CC and 01-00-0C-CC-CC-CD
 */
int32 apollomp_raw_trap_rmaCiscoFeature_get(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 regAddr;
    uint32 field;

    /* parameter check */
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK( ((rmaTail != 0xCC) && (rmaTail != 0xCD)), RT_ERR_RMA_ADDR);
    RT_PARAM_CHK((APOLLOMP_RMA_FUN_END <= type), RT_ERR_INPUT);
    switch (type)
    {
        case APOLLOMP_RMA_FUN_VLAN_LEAKY:
            field = APOLLOMP_VLAN_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_PISO_LEAKY:
            field = APOLLOMP_PORTISO_LEAKYf;
            break;
        case APOLLOMP_RMA_FUN_BYPASS_STORM:
            field = APOLLOMP_DISCARD_STORM_FILTERf;
            break;
        case APOLLOMP_RMA_FUN_KEEP_CTGA_FMT:
            field = APOLLOMP_KEEP_FORMATf;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	switch(rmaTail)
	{
		case 0xCC:
			regAddr = APOLLOMP_RMA_CTRL_CDPr;
			break;
		case 0xCD:
			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
			break;
		default:
            return RT_ERR_FAILED;
	}

    if ((ret = reg_field_read(regAddr, field, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_rmaCiscoFeature_get */
/* Function Name:
 *      apollomp_raw_trap_igmpAction_set
 * Description:
 *      Set IGMP/MLD protocol action
 * Input:
 *      port            - port ID
 *      type            - IGMP/MLD protocol type
 *      action          - Action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpAction_set(rtk_port_t port, apollomp_raw_igmpmld_type_t type, rtk_action_t action)
{
    apollomp_raw_igmpAction_t igmpAct;
    uint32 field;
    int32 ret;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_RAW_TYPE_END <= type), RT_ERR_INPUT);
    switch(type)
    {
        case APOLLOMP_RAW_TYPE_IGMPV1:
            field = APOLLOMP_IGMPV1_OPf;
            break;
        case APOLLOMP_RAW_TYPE_IGMPV2:
            field = APOLLOMP_IGMPV2_OPf;
            break;
        case APOLLOMP_RAW_TYPE_IGMPV3:
            field = APOLLOMP_IGMPV3_OPf;
            break;
        case APOLLOMP_RAW_TYPE_MLDV1:
            field = APOLLOMP_MLDV1_OPf;
            break;
        case APOLLOMP_RAW_TYPE_MLDV2:
            field = APOLLOMP_MLDV2_OPf;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    switch(action)
	{
	 	case ACTION_FORWARD:
	 		igmpAct = APOLLOMP_RAW_IGMP_ACT_FORWARD;
			break;
	 	case ACTION_TRAP2CPU:
	 		igmpAct = APOLLOMP_RAW_IGMP_ACT_TRAP;
			break;
	 	case ACTION_DROP:
	 		igmpAct = APOLLOMP_RAW_IGMP_ACT_DROP;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, field, (uint32 *)&igmpAct)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trap_igmpAction_set */

/* Function Name:
 *      apollomp_raw_trap_igmpAction_get
 * Description:
 *      Get IGMP/MLD protocol action
 * Input:
 *      port            - port ID
 *      type            - IGMP/MLD protocol type
 * Output:
 *      pAction         - Action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpAction_get(rtk_port_t port, apollomp_raw_igmpmld_type_t type, rtk_action_t *pAction)
{
    apollomp_raw_igmpAction_t igmpAct;
    uint32 field;
    int32 ret;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type > APOLLOMP_RAW_TYPE_MLDV2), RT_ERR_INPUT);
    RT_PARAM_CHK((pAction == NULL), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case APOLLOMP_RAW_TYPE_IGMPV1:
            field = APOLLOMP_IGMPV1_OPf;
            break;
        case APOLLOMP_RAW_TYPE_IGMPV2:
            field = APOLLOMP_IGMPV2_OPf;
            break;
        case APOLLOMP_RAW_TYPE_IGMPV3:
            field = APOLLOMP_IGMPV3_OPf;
            break;
        case APOLLOMP_RAW_TYPE_MLDV1:
            field = APOLLOMP_MLDV1_OPf;
            break;
        case APOLLOMP_RAW_TYPE_MLDV2:
            field = APOLLOMP_MLDV2_OPf;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_read(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, field, (uint32 *)&igmpAct)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    switch(igmpAct)
	{
	 	case APOLLOMP_RAW_IGMP_ACT_FORWARD:
	 		*pAction = ACTION_FORWARD;
			break;
	 	case APOLLOMP_RAW_IGMP_ACT_TRAP:
	 		*pAction = ACTION_TRAP2CPU;
			break;
	 	case APOLLOMP_RAW_IGMP_ACT_DROP:
	 		*pAction = ACTION_DROP;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_trap_igmpIsoLeaky_set
 * Description:
 *      Set IGMP/MLD Port Isolation Leaky
 * Input:
 *      state           - The state of IGMP/MLD port isolation leaky
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpIsoLeaky_set(rtk_enable_t state)
{
    int32 ret;

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_PISO_LEAKYf, (uint32 *)&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_trap_igmpIsoLeaky_get
 * Description:
 *      Get IGMP/MLD Port Isolation Leaky
 * Input:
 *      None.
 * Output:
 *      pState          - The state of IGMP/MLD port isolation leaky
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpIsoLeaky_get(rtk_enable_t *pState)
{
    int32 ret;
    uint32 raw_state;

    /* parameter check */
    RT_PARAM_CHK((pState == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_PISO_LEAKYf, &raw_state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pState = (rtk_enable_t)raw_state;
    return RT_ERR_OK;
}
#endif
/* Function Name:
 *      apollomp_raw_trap_igmpVLANLeaky_set
 * Description:
 *      Set IGMP/MLD VLAN Leaky
 * Input:
 *      state           - The state of IGMP/MLD VLAN leaky
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpVLANLeaky_set(rtk_enable_t state)
{
    int32 ret;

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_VLAN_LEAKYf, (uint32 *)&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_trap_igmpVLANLeaky_get
 * Description:
 *      Get IGMP/MLD VLAN Leaky
 * Input:
 *      None.
 * Output:
 *      pState          - The state of IGMP/MLD VLAN leaky
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpVLANLeaky_get(rtk_enable_t *pState)
{
    int32 ret;
    uint32 raw_state;

    /* parameter check */
    RT_PARAM_CHK((pState == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_VLAN_LEAKYf, &raw_state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pState = (rtk_enable_t)raw_state;
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_trap_igmpBypassStrom_set
 * Description:
 *      Set IGMP/MLD Bypass strom control
 * Input:
 *      state           - The state of IGMP/MLD Bypass strom control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpBypassStrom_set(rtk_enable_t state)
{
    int32 ret;

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_DISC_STORM_FILTERf, (uint32 *)&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_trap_igmpBypassStrom_get
 * Description:
 *      Get IGMP/MLD Bypass strom control
 * Input:
 *      None.
 * Output:
 *      pState          - The state of IGMP/MLD Bypass strom control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpBypassStrom_get(rtk_enable_t *pState)
{
    int32 ret;
    uint32 raw_state;

    /* parameter check */
    RT_PARAM_CHK((pState == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_DISC_STORM_FILTERf, &raw_state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pState = (rtk_enable_t)raw_state;
    return RT_ERR_OK;
}
#if 0
/* Function Name:
 *      apollomp_raw_trap_igmpChechsumError_set
 * Description:
 *      Set IGMP/MLD Checksum Error action
 * Input:
 *      action           - The Action of IGMP/MLD checksum error packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpChechsumError_set(rtk_action_t action)
{
    int32 ret;
    apollomp_raw_igmpChecksumErrAction_t raw_act;

    /* parameter check */
    switch(action)
	{
	 	case ACTION_FORWARD:
	 		raw_act = APOLLOMP_RAW_CHECKSUM_ERR_FORWARD;
			break;
	 	case ACTION_TRAP2CPU:
	 		raw_act = APOLLOMP_RAW_CHECKSUM_ERR_TRAP;
			break;
	 	case ACTION_DROP:
	 		raw_act = APOLLOMP_RAW_CHECKSUM_ERR_DROP;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    if ((ret = reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_CKS_ERR_OPf, (uint32 *)&raw_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_trap_igmpChechsumError_get
 * Description:
 *      Get IGMP/MLD Checksum Error action
 * Input:
 *      None.
 * Output:
 *      pAction         - The Action of IGMP/MLD checksum error packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None.
 */
int32 apollomp_raw_trap_igmpChechsumError_get(rtk_action_t *pAction)
{
    int32 ret;
    apollomp_raw_igmpChecksumErrAction_t raw_act;

    /* parameter check */
    RT_PARAM_CHK((pAction == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_CKS_ERR_OPf, (uint32 *)&raw_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    switch(raw_act)
	{
	 	case APOLLOMP_RAW_CHECKSUM_ERR_FORWARD:
	 		*pAction = ACTION_FORWARD;
			break;
	 	case APOLLOMP_RAW_CHECKSUM_ERR_TRAP:
	 		*pAction = ACTION_TRAP2CPU;
			break;
	 	case APOLLOMP_RAW_CHECKSUM_ERR_DROP:
	 		*pAction = ACTION_DROP;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    return RT_ERR_OK;
}
#endif
