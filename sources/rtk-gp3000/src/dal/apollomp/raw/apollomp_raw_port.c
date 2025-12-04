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
 *
 * Purpose : switch asic-level Port API
 * Feature : Port related functions
 *
 */

#include <dal/apollomp/raw/apollomp_raw_port.h>

/* Function Name:
 *      apollomp_raw_port_queueEmpty_get
 * Description:
 *      Get queue empty port mask;
 * Input:
 *      None.
 * Output:
 *      pEmpty_mask   	    - Empty port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_DSL_VC
 * Note:
 */
int32 apollomp_raw_port_queueEmpty_get(rtk_portmask_t *pEmpty_mask)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pEmpty_mask), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_P_QUEUE_EMPTYr, APOLLOMP_EMPTYf, &(pEmpty_mask->bits[0]))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_port_queueEmpty_get */

/* Function Name:
 *      apollomp_raw_port_phyReg_set
 * Description:
 *      Set PHY register
 * Input:
 *      port            - Port ID
 *      reg             - PHY Register
 *      data            - Data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 */
int32 apollomp_raw_port_phyReg_set(rtk_port_t port, uint32 reg, uint32 data)
{
    int32 ret;
    uint32 regdata;

    RT_PARAM_CHK(HAL_IS_PHY_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((data > HAL_MIIM_DATA_MAX()), RT_ERR_INPUT);

    /* Data */
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_WDr, APOLLOMP_WR_DATf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    /* register */
    regdata = ((port << 16) | reg);
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_ADRf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    /* Write Action */
    regdata = APOLLOMP_RAW_PORT_PHY_WRITE_ACT;
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_WRENf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    /* Command */
    regdata = APOLLOMP_RAW_PORT_PHY_CMD;
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_CMD_ENf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_port_phyReg_set */

/* Function Name:
 *      apollomp_raw_port_phyReg_get
 * Description:
 *      Get PHY register
 * Input:
 *      port            - Port ID
 *      reg             - PHY Register
 * Output:
 *      pData           - Data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_port_phyReg_get(rtk_port_t port, uint32 reg, uint32 *pData)
{
    int32 ret;
    uint32 regdata;
    uint32 retry = 0;

    RT_PARAM_CHK(HAL_IS_PHY_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* register */
    regdata = ((port << 16) | reg);
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_ADRf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    /* Read Action */
    regdata = APOLLOMP_RAW_PORT_PHY_READ_ACT;
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_WRENf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    /* Command */
    regdata = APOLLOMP_RAW_PORT_PHY_CMD;
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_CMD_ENf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    do
    {
        /* Polling until Command is done */
        if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_BUSYf, &regdata)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
    } while ( (++retry < 1000) && (APOLLOMP_RAW_PORT_PHY_CMD_BUSY == regdata) );

    if(APOLLOMP_RAW_PORT_PHY_CMD_BUSY == regdata)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_RD_DATf, &regdata)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    *pData = regdata;
    return RT_ERR_OK;
} /* end of apollomp_raw_port_phyReg_get */


/* Function Name:
 *      apollomp_raw_port_ForceAbility_set
 * Description:
 *      Set port forced ability
 * Input:
 *      port	    - port id
 *      pAbility   	- port ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 * Note:
 */
int32 apollomp_raw_port_ForceAbility_set(rtk_port_t port, apollomp_raw_port_ability_t *pAbility)
{
    int32 ret;

    RT_PARAM_CHK((pAbility==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((PORT_SPEED_500M <= pAbility->speed), RT_ERR_INPUT);
	RT_PARAM_CHK((PORT_DUPLEX_END <= pAbility->duplex), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->linkFib1g), RT_ERR_INPUT);
	RT_PARAM_CHK((PORT_LINKSTATUS_END <= pAbility->linkStatus), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->txFc), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->rxFc), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->nwayAbility), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->masterMod), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->nwayFault), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->lpi_100m), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pAbility->lpi_giga), RT_ERR_INPUT);

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_SPEED_ABLTYf, &pAbility->speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_DUPLEX_ABLTYf, &pAbility->duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_FIB1G_ABLTYf, &pAbility->linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LINK_ABLTYf, &pAbility->linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RXPAUSE_ABLTYf, &pAbility->rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_TXPAUSE_ABLTYf, &pAbility->txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_NWAY_ABLTYf, &pAbility->nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MST_MOD_ABLTYf, &pAbility->masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MST_FAULT_ABLTYf, &pAbility->nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LPI_100_ABLTYf, &pAbility->lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LPI_1000_ABLTYf, &pAbility->lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	return RT_ERR_OK;
} /* end of apollomp_raw_port_ForceAbility_set */

/* Function Name:
 *      apollomp_raw_port_ForceAbility_get
 * Description:
 *      Set port forced ability
 * Input:
 *      port	    - port id
 * Output:
 *      pAbility   	- port ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 * Note:
 */
int32 apollomp_raw_port_ForceAbility_get(rtk_port_t port, apollomp_raw_port_ability_t *pAbility)
{
    int32 ret;

    RT_PARAM_CHK((pAbility==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_SPEED_ABLTYf, &pAbility->speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_DUPLEX_ABLTYf, &pAbility->duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_FIB1G_ABLTYf, &pAbility->linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LINK_ABLTYf, &pAbility->linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RXPAUSE_ABLTYf, &pAbility->rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_TXPAUSE_ABLTYf, &pAbility->txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_NWAY_ABLTYf, &pAbility->nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MST_MOD_ABLTYf, &pAbility->masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MST_FAULT_ABLTYf, &pAbility->nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LPI_100_ABLTYf, &pAbility->lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LPI_1000_ABLTYf, &pAbility->lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	return RT_ERR_OK;
} /* end of apollomp_raw_port_ForceAbility_set */

/* Function Name:
 *      apollomp_raw_port_ability_get
 * Description:
 *      Set port forced ability
 * Input:
 *      port	    - port id
 * Output:
 *      pAbility   	- port ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 * Note:
 */

int32 apollomp_raw_port_ability_get(rtk_port_t port, apollomp_raw_port_ability_t *pAbility)
{
    int32 ret;

    RT_PARAM_CHK((pAbility==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_SPDf, &pAbility->speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_DUPLEXf, &pAbility->duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_FIB1Gf, &pAbility->linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_STATUSf, &pAbility->linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_RX_FCf, &pAbility->rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_TX_FCf, &pAbility->txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_NWAY_ABLTYf, &pAbility->nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_MSTRf, &pAbility->masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_NWAY_FAULTf, &pAbility->nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LPI_100f, &pAbility->lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LPI_1000f, &pAbility->lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

	return RT_ERR_OK;
} /* end of apollomp_raw_port_ability_get */

/* Function Name:
 *      apollomp_raw_port_forceDmp_set
 * Description:
 *      Set forwarding force mode
 * Input:
 *      state     - enable/disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 */
int32 apollomp_raw_port_forceDmp_set(rtk_enable_t state)
{
    int32 ret = RT_ERR_FAILED;
	RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_field_write(APOLLOMP_EN_FORCE_P_DMPr, APOLLOMP_FORCE_MODEf,&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }

	return RT_ERR_OK;

} /* end of apollomp_raw_port_forceDmp_set */

/* Function Name:
 *      apollomp_raw_port_forceDmp_get
 * Description:
 *      Get forwarding force mode
 * Input:
 *      None
 * Output:
 *      pState 	- enable/disable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_port_forceDmp_get(rtk_enable_t *pState)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_EN_FORCE_P_DMPr, APOLLOMP_FORCE_MODEf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;

} /* end of apollomp_raw_port_forceDmp_get */

/* Function Name:
 *      apollomp_raw_port_forceDmpMask_set
 * Description:
 *      Set force mode port mask
 * Input:
 *      port      - port id
 *      mask   	- port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_DSL_VC
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 */
int32 apollomp_raw_port_forceDmpMask_set(rtk_port_t port, rtk_portmask_t mask)
{
    int32 ret;
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(mask), RT_ERR_PORT_MASK);
    if ((ret = reg_array_field_write(APOLLOMP_FORCE_P_DMPr, REG_ARRAY_INDEX_NONE, port, APOLLOMP_FORCE_PROT_MASKf, mask.bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;

} /* end of apollomp_raw_port_forceDmpMask_set */

/* Function Name:
 *      apollomp_raw_port_forceDmpMask_get
 * Description:
 *      Get force mode port mask
 * Input:
 *      port      - port id
 * Output:
 *      pMask   	- port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_port_forceDmpMask_get(rtk_port_t port, rtk_portmask_t  *pMask)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

     if ((ret = reg_array_field_read(APOLLOMP_FORCE_P_DMPr, REG_ARRAY_INDEX_NONE, port, APOLLOMP_FORCE_PROT_MASKf, pMask->bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;

} /* end of apollomp_raw_port_forceDmpMask_get */

/* Function Name:
 *      apollomp_raw_port_localPacket_set
 * Description:
 *      Set action of local packet action configuration of the specific port
 * Input:
 *      port        - port id
 *      action		- the port local packet action configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_port_localPacket_set(rtk_port_t port, rtk_action_t action)
{
    int32 ret;
    rtk_enable_t permit;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    switch(action)
    {
        case ACTION_FORWARD:
            permit = ENABLED;;

            break;
        case ACTION_DROP:
            permit = DISABLED;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_write(APOLLOMP_L2_SRC_PORT_PERMITr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &permit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of apollomp_raw_port_localPacket_set */


/* Function Name:
 *      apollomp_raw_port_localPacket_get
 * Description:
 *      Set action of local packet action configuration of the specific port
 * Input:
 *      port        - port id
 * Output:
 *      pAaction    - the port local packet action configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_port_localPacket_get(rtk_port_t port, rtk_action_t *pAction)
{
    int32 ret;
    rtk_enable_t permit;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_L2_SRC_PORT_PERMITr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &permit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(permit)
        *pAction = ACTION_FORWARD;
    else
        *pAction = ACTION_DROP;

    return RT_ERR_OK;
} /* end of apollomp_raw_port_localPacket_get */

/* Function Name:
 *      apollomp_raw_port_isoIpmcastLeaky_set
 * Description:
 *      Get IP mulicast vlan leaky
 * Input:
 *      port 		- port number
 *      enable 		- enable status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_port_isoIpmcastLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}/* end of apollomp_raw_port_isoIpmcastLeaky_set */

/* Function Name:
 *      apollomp_raw_port_isoIpmcastLeaky_get
 * Description:
 *      Get IP mulicast vlan leaky
 * Input:
 *      port 		- port number
 * Output:
 *      pEnable 	- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_port_isoIpmcastLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}/* end of apollomp_raw_port_isoIpmcastLeaky_get */




