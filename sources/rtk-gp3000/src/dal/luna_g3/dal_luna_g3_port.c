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
 * $Revision: 42067 $
 * $Date: 2013-08-15 14:30:04 +0800 (星期四, 15 八月 2013) $
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 *           (2) RTCT
 */

/*
 * Include Files
 */

#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_port.h>
#include <osal/time.h>

#include <scfg.h>
#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
 
static uint32 port_init = INIT_NOT_COMPLETED;

static ca_uint8_t eth_mdio_addr[CA_PORT_MAX_NUM_ETHERNET+1];

/*
 * Macro Declaration
 */

/* Function Name:
 *      dal_luna_g3_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_luna_g3_port_init(void)
{
    ca_status_t ret=CA_E_OK;
    ca_device_config_tlv_t scfg_tlv;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    port_init = INIT_COMPLETED;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PHY_BASE_ADDR;
    scfg_tlv.len = CFG_ID_PHY_BASE_ADDR_LEN;

    if((ret = ca_device_config_tlv_get(0, &scfg_tlv)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        port_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }

    memcpy(eth_mdio_addr,scfg_tlv.value, CFG_ID_PHY_BASE_ADDR_LEN);

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_init */


/* Function Name:
 *      dal_luna_g3_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
dal_luna_g3_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_link_status_t status;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = ca_eth_port_link_status_get(0,port_id,&status)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pStatus = ((status == CA_PORT_LINK_UP) ? PORT_LINKUP : PORT_LINKDOWN);
    return RT_ERR_OK;
} /* end of dal_luna_g3_port_link_get */

/* Function Name:
 *      dal_luna_g3_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      (1) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (2) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
dal_luna_g3_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_speed_t speed;
    ca_eth_port_duplex_t duplex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    if((ret = ca_eth_port_speed_get(0,port_id,&speed)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_duplex_get(0,port_id,&duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    switch(speed)
    {
        case CA_PORT_SPEED_10M:
            *pSpeed=PORT_SPEED_10M;
            break;
        case CA_PORT_SPEED_100M:
            *pSpeed=PORT_SPEED_100M;
            break;
        case CA_PORT_SPEED_1G:
            *pSpeed=PORT_SPEED_1000M;
            break;
        case CA_PORT_SPEED_2_5G:
            *pSpeed=PORT_SPEED_2G5;
            break;
        case CA_PORT_SPEED_10G:
            *pSpeed=PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    *pDuplex = ((CA_PORT_DUPLEX_FULL == duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);
    return RT_ERR_OK;
} /* end of dal_luna_g3_port_speedDuplex_get */

/* Function Name:
 *      dal_luna_g3_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
dal_luna_g3_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t pfc_enable;
    ca_boolean_t pause_rx;
    ca_boolean_t pause_tx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

    if((ret = ca_eth_port_pause_get(0,port_id,&pfc_enable,&pause_rx,&pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    *pTxStatus=pause_tx;
    *pRxStatus=pause_rx;

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_flowctrl_get */

/* Function Name:
 *      dal_luna_g3_port_phyAutoNegoEnable_get
 * Description:
 *      Get PHY ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t ca_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = ca_eth_port_autoneg_get(0,port_id,&ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pEnable = (ca_enable==1)? ENABLED:DISABLED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_phyAutoNegoEnable_get */

/* Function Name:
 *      dal_luna_g3_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - input parameter out of range
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely swtich the mode without calling ability setting API again
 */
int32
dal_luna_g3_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t ca_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    
    ca_enable = (enable==ENABLED)? 1:0;

    if((ret = ca_eth_port_autoneg_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_luna_g3_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_phyAutoNegoAbility_get(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    memset(&ability,0,sizeof(ca_eth_port_ability_t));

    if((ret = ca_eth_port_phy_advert_get(0,port_id,&ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    pAbility->Half_10=ability.half_10;
    pAbility->Full_10=ability.full_10;
    pAbility->Half_100=ability.half_100;
    pAbility->Full_100=ability.full_100;
    pAbility->Full_1000=ability.full_1g;
    pAbility->Full_10000=ability.full_10g;
    pAbility->FC=ability.flow_ctrl;
    pAbility->AsyFC=ability.asym_flow_ctrl;

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_luna_g3_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
int32
dal_luna_g3_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    ability.half_10=pAbility->Half_10;
    ability.full_10=pAbility->Full_10;
    ability.half_100=pAbility->Half_100;
    ability.full_100=pAbility->Full_100;
    ability.full_1g=pAbility->Full_1000;
    ability.full_10g=pAbility->Full_10000;
    ability.flow_ctrl=pAbility->FC;
    ability.asym_flow_ctrl=pAbility->AsyFC;

    if((ret = ca_eth_port_phy_advert_set(0,port_id,ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_luna_g3_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_luna_g3_port_phyForceModeAbility_get(
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_speed_t speed;
    ca_eth_port_duplex_t duplex;
    ca_boolean_t pfc_enable;
    ca_boolean_t pause_rx;
    ca_boolean_t pause_tx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFlowControl), RT_ERR_NULL_POINTER);

    if((ret = ca_eth_port_speed_get(0,port_id,&speed)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_duplex_get(0,port_id,&duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_pause_get(0,port_id,&pfc_enable,&pause_rx,&pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    switch(speed)
    {
        case CA_PORT_SPEED_10M:
            *pSpeed=PORT_SPEED_10M;
            break;
        case CA_PORT_SPEED_100M:
            *pSpeed=PORT_SPEED_100M;
            break;
        case CA_PORT_SPEED_1G:
            *pSpeed=PORT_SPEED_1000M;
            break;
        case CA_PORT_SPEED_2_5G:
            *pSpeed=PORT_SPEED_2G5;
            break;
        case CA_PORT_SPEED_10G:
            *pSpeed=PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    *pDuplex = ((CA_PORT_DUPLEX_FULL == duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);

    *pFlowControl = (pfc_enable==1)? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_luna_g3_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      port        - port id
 *      speed       - port speed
 *      duplex      - port duplex mode
 *      flowControl - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      (1) You can set these abilities no matter which mode PHY stays currently.
 *          However, these abilities only take effect when the PHY is in Force mode
 *
 *      (2) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (3) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
dal_luna_g3_port_phyForceModeAbility_set(
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_speed_t ca_speed;
    ca_eth_port_duplex_t ca_duplex;
    ca_boolean_t ca_pfc_enable;
    ca_boolean_t ca_pause_rx;
    ca_boolean_t ca_pause_tx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_DUPLEX_END <= duplex), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= flowControl), RT_ERR_INPUT);

    switch(speed)
    {
        case PORT_SPEED_10M:
            ca_speed=CA_PORT_SPEED_10M;
            break;
        case PORT_SPEED_100M:
            ca_speed=CA_PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000M:
            ca_speed=CA_PORT_SPEED_1G;
            break;
        case PORT_SPEED_2G5:
            ca_speed=CA_PORT_SPEED_2_5G;
            break;
        case PORT_SPEED_10G:
            ca_speed=CA_PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_speed_set(0,port_id,ca_speed)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    ca_duplex = ((PORT_FULL_DUPLEX == duplex) ? CA_PORT_DUPLEX_FULL : CA_PORT_DUPLEX_HALF);

    if((ret = ca_eth_port_duplex_set(0,port_id,ca_duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_pause_get(0,port_id,&ca_pfc_enable,&ca_pause_rx,&ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    ca_pfc_enable = (flowControl==ENABLED)? 1:0;

    if((ret = ca_eth_port_pause_set(0,port_id,ca_pfc_enable,ca_pause_rx,ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_luna_g3_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port
 * Input:
 *      port  - port id
 *      page  - page id
 *      reg   - reg id
 * Output:
 *      pData - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */

int32
dal_luna_g3_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    ca_status_t ret=CA_E_OK;
    ca_uint8_t st_code;
    ca_uint8_t reg_dev_addr;
    ca_uint16_t addr;
    ca_uint16_t data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    st_code = 0; //clause 22
    reg_dev_addr = reg;
    addr = 0;
    if((ret = cap_mdio_read(0,st_code,eth_mdio_addr[port],reg_dev_addr,addr,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pData = data;

    return ret;
} /* end of dal_luna_g3_port_phyReg_get */

/* Function Name:
 *      dal_luna_g3_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port
 * Input:
 *      port - port id
 *      page - page id
 *      reg  - reg id
 *      data - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
int32
dal_luna_g3_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    ca_status_t ret=CA_E_OK;
    ca_uint8_t st_code;
    ca_uint8_t reg_dev_addr;
    ca_uint16_t addr;
    ca_uint16_t ca_data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((data > HAL_MIIM_DATA_MAX()), RT_ERR_INPUT);

    st_code = 0; //clause 22
    reg_dev_addr = reg;
    addr = 0;
    ca_data = data;
    if((ret = cap_mdio_write(0,st_code,eth_mdio_addr[port],reg_dev_addr,addr,ca_data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_luna_g3_port_phyReg_set */

/* Function Name:
 *      dal_luna_g3_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    
    if((ret = ca_port_enable_get(0,port_id,&enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(enable = 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_adminEnable_get */

/* Function Name:
 *      dal_luna_g3_port_adminEnable_set
 * Description:
 *      Set port admin status of the specific port
 * Input:
 *      port    - port id
 *      enable  - port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
dal_luna_g3_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t ca_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable == ENABLED)
        ca_enable = 1;
    else
        ca_enable = 0;

    if((ret = ca_port_enable_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
} /* end of dal_luna_g3_port_adminEnable_set */

/* Function Name:
 *      dal_luna_g3_port_macForceAbility_get
 * Description:
 *      Get MAC forece ability
 * Input:
 *      port - the ports for get ability
 *      pMacAbility - mac ability value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_speed_t speed;
    ca_eth_port_duplex_t duplex;
    ca_eth_port_link_status_t status;
    ca_boolean_t pfc_enable;
    ca_boolean_t pause_rx;
    ca_boolean_t pause_tx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    memset(pMacAbility,0,sizeof(rtk_port_macAbility_t));
    
    if((ret = ca_eth_port_speed_get(0,port_id,&speed)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_duplex_get(0,port_id,&duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_link_status_get(0,port_id,&status)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_pause_get(0,port_id,&pfc_enable,&pause_rx,&pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    switch(speed)
    {
        case CA_PORT_SPEED_10M:
            pMacAbility->speed=PORT_SPEED_10M;
            break;
        case CA_PORT_SPEED_100M:
            pMacAbility->speed=PORT_SPEED_100M;
            break;
        case CA_PORT_SPEED_1G:
            pMacAbility->speed=PORT_SPEED_1000M;
            break;
        case CA_PORT_SPEED_2_5G:
            pMacAbility->speed=PORT_SPEED_2G5;
            break;
        case CA_PORT_SPEED_10G:
            pMacAbility->speed=PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pMacAbility->duplex = ((CA_PORT_DUPLEX_FULL == duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);
    pMacAbility->linkStatus = ((status == CA_PORT_LINK_UP) ? PORT_LINKUP : PORT_LINKDOWN);
    pMacAbility->txFc = (pause_tx==1)? ENABLED : DISABLED;
    pMacAbility->rxFc = (pause_rx==1)? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_macForceAbility_get */

/* Function Name:
 *      dal_luna_g3_port_macForceAbility_set
 * Description:
 *      Set MAC forece ability
 * Input:
 *      port - the ports for set ability
 *      macAbility - mac ability value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_speed_t ca_speed;
    ca_eth_port_duplex_t ca_duplex;
    ca_boolean_t ca_pfc_enable;
    ca_boolean_t ca_pause_rx;
    ca_boolean_t ca_pause_tx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_SPEED_END <= macAbility.speed), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_DUPLEX_END <= macAbility.duplex), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.rxFc), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.txFc), RT_ERR_INPUT);

    switch(macAbility.speed)
    {
        case PORT_SPEED_10M:
            ca_speed=CA_PORT_SPEED_10M;
            break;
        case PORT_SPEED_100M:
            ca_speed=CA_PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000M:
            ca_speed=CA_PORT_SPEED_1G;
            break;
        case PORT_SPEED_2G5:
            ca_speed=CA_PORT_SPEED_2_5G;
            break;
        case PORT_SPEED_10G: 
            ca_speed=CA_PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_speed_set(0,port_id,ca_speed)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    ca_duplex = ((PORT_FULL_DUPLEX == macAbility.duplex) ? CA_PORT_DUPLEX_FULL : CA_PORT_DUPLEX_HALF);

    if((ret = ca_eth_port_duplex_set(0,port_id,ca_duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_pause_get(0,port_id,&ca_pfc_enable,&ca_pause_rx,&ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    ca_pause_rx = (macAbility.rxFc==ENABLED)? 1:0;
    ca_pause_tx = (macAbility.txFc==ENABLED)? 1:0;

    if((ret = ca_eth_port_pause_set(0,port_id,ca_pfc_enable,ca_pause_rx,ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_macForceAbility_set */

/* Function Name:
 *      dal_luna_g3_port_macForceAbilityState_get
 * Description:
 *      Get MAC forece ability state
 * Input:
 *      port - the ports for get ability
 *      pState - mac ability state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_macForceAbilityState_get(rtk_port_t port,rtk_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t ca_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if((ret = ca_eth_port_autoneg_get(0,port_id,&ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pState = (ca_enable==1)? DISABLED:ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_luna_g3_port_macForceAbilityState_set
 * Description:
 *      Set MAC forece ability state
 * Input:
 *      port - the ports for set ability
 *      state - mac ability state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_luna_g3_port_macForceAbilityState_set(rtk_port_t port,rtk_enable_t state)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t ca_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    ca_enable = (state==ENABLED)? 0:1;

    if((ret = ca_eth_port_autoneg_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_luna_g3_port_isolationEntry_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_luna_g3_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint8_t port_count;
    ca_port_id_t dst_ports[RTK_MAX_NUM_OF_PORTS];
    uint8 i;
    rtk_portmask_t blockPortMask;

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);
    
    if((ret = ca_port_isolation_get(0,port_id,&port_count,dst_ports)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(&blockPortMask,0,sizeof(rtk_portmask_t));

    for(i=0;i<port_count;i++)
    {
        RT_PARAM_CHK(!HAL_IS_PORT_EXIST(PORT_ID(dst_ports[i])), RT_ERR_PORT_ID);

        blockPortMask.bits[0] |= 1<<PORT_ID(dst_ports[i]);
    }

    HAL_GET_ALL_PORTMASK(*pPortmask);

    for(i=0;i<32;i++)
    {
        if(blockPortMask.bits[0]&(1<<i))
        {
            pPortmask->bits[0] = pPortmask->bits[0]&~(1<<i);
        }
    }

    memset(pExtPortmask,0,sizeof(rtk_portmask_t));

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_isolationEntry_get */

/* Function Name:
 *      dal_luna_g3_port_isolationEntry_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_luna_g3_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint8_t port_count;
    ca_port_id_t dst_ports[RTK_MAX_NUM_OF_PORTS];
    uint8 i;
    rtk_portmask_t blockPortMask;

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    HAL_GET_ALL_PORTMASK(blockPortMask);

    port_count = 0;
    for(i=0;i<32;i++)
    {
        if(pPortmask->bits[0]&(1<<i))
        {
            blockPortMask.bits[0] = blockPortMask.bits[0]&~(1<<i);
        }
    }

    port_count = 0;
    for(i=0;i<32;i++)
    {
        if(blockPortMask.bits[0]&(1<<i))
        {
            dst_ports[port_count++] = RTK2CA_PORT_ID(i);
        }
    }

    if((ret = ca_port_isolation_set(0,port_id,port_count,dst_ports)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_port_isolationEntry_set */