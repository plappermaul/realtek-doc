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
 * $Date: 2013-08-15 14:30:04 +0800 (?Ÿæ??? 15 ?«æ? 2013) $
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
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_ca8277b_port.h>
#include <osal/time.h>

#include <scfg.h>
#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>

#include <aal_ni_l2.h>
#include <aal_port.h>
#include <aal_phy.h>

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
 *      dal_ca8277b_port_init
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
dal_ca8277b_port_init(void)
{
    ca_status_t ret=CA_E_OK;
    ca_device_config_tlv_t scfg_tlv;
    rtk_port_t port;
    rtk_port_phy_ability_t  ability;

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

    HAL_SCAN_ALL_PORT(port)
    {   
        if(HAL_IS_PON_PORT(port))
            continue;

        if(HAL_IS_SERDES_PORT(port))
            continue;

        if(HAL_IS_GE_PORT(port))
        {
            ability.Half_10=1;
            ability.Full_10=1;
            ability.Half_100=1;
            ability.Full_100=1;
            ability.Half_1000=1;
            ability.Full_1000=1;
            ability.Full_2500=0;
            ability.Full_10000=0;
            ability.FC=0;
            ability.AsyFC=0;
            if((ret = dal_ca8277b_port_phyAutoNegoAbility_set(port,&ability))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                port_init = INIT_NOT_COMPLETED;
                return RT_ERR_FAILED;
            }
        }
        else if(HAL_IS_FE_PORT(port))
        {
            ability.Half_10=1;
            ability.Full_10=1;
            ability.Half_100=1;
            ability.Full_100=1;
            ability.Half_1000=0;
            ability.Full_1000=0;
            ability.Full_2500=0;
            ability.Full_10000=0;
            ability.FC=0;
            ability.AsyFC=0;
            if((ret = dal_ca8277b_port_phyAutoNegoAbility_set(port,&ability))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                port_init = INIT_NOT_COMPLETED;
                return RT_ERR_FAILED;
            }
        }
    }
    
#if defined(CONFIG_COMMON_RT_API)
    /* Init port define for RT API */
    RT_PORT0 = 0;
    RT_PORT1 = 1;
    RT_PORT2 = 2;
    RT_PORT3 = 3;
    RT_PORT4 = 4;

    RT_PORT_PON = 7;

    RT_PORT_CPU0 = 0x10;
    RT_PORT_CPU1 = 0x11;
    RT_PORT_CPU2 = 0x12;
    RT_PORT_CPU3 = 0x13;
    RT_PORT_CPU4 = 0x14;
    RT_PORT_CPU5 = 0x15;
    RT_PORT_CPU6 = 0x16;
    RT_PORT_CPU7 = 0x17;
    RT_PORT_MAX = 0x18;
#endif

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_init */


/* Function Name:
 *      dal_ca8277b_port_link_get
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
dal_ca8277b_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_link_status_t status;
    ca_boolean_t ps=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if(HAL_IS_PON_PORT(port))
    {
        *pStatus = PORT_LINKUP;
    }
    else if(port>=CA_PORT_ID_CPU0 && port<=CA_PORT_ID_CPU7)
    {
        *pStatus = PORT_LINKUP;
    }
    else
    {
        if((ret = aal_port_physical_port_enable_get(0,port,&ps)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(ps == 1)
        {
            if((ret = ca_eth_port_link_status_get(0,port_id,&status)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            *pStatus = ((status == CA_PORT_LINK_UP) ? PORT_LINKUP : PORT_LINKDOWN);
        }
        else
        {
            *pStatus = PORT_LINKDOWN;
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_link_get */

/* Function Name:
 *      dal_ca8277b_port_speedDuplex_get
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
dal_ca8277b_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_link_status_t status;
    ca_eth_port_speed_t speed;
    ca_eth_port_duplex_t duplex;
    ca_boolean_t ps=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    if(HAL_IS_PON_PORT(port))
    {
        *pSpeed = PORT_SPEED_10G;
        *pDuplex = PORT_FULL_DUPLEX;
        return RT_ERR_OK;
    }
    else if(port>=CA_PORT_ID_CPU0 && port<=CA_PORT_ID_CPU7)
    {
        *pSpeed = PORT_SPEED_1000M;
        *pDuplex = PORT_FULL_DUPLEX;
        return RT_ERR_OK;
    }

    if((ret = ca_eth_port_link_status_get(0,port_id,&status)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(status == CA_PORT_LINK_UP)
    {
        if((ret = ca_eth_port_phy_speed_get(0,port_id,&speed)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if((ret = ca_eth_port_phy_duplex_get(0,port_id,&duplex)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }   
    }
    else
    {
        if((ret = aal_port_physical_port_enable_get(0,port,&ps)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(ps == 1)
        {
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
        }
        else
        {
            *pSpeed = PORT_SPEED_1000M;
            *pDuplex = PORT_FULL_DUPLEX;
            return RT_ERR_OK;
        }
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
} /* end of dal_ca8277b_port_speedDuplex_get */

/* Function Name:
 *      dal_ca8277b_port_flowctrl_get
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
dal_ca8277b_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t pfc_enable=0;
    ca_boolean_t pause_rx=0;
    ca_boolean_t pause_tx=0;
    ca_boolean_t ps=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

    if(HAL_IS_PON_PORT(port))
    {
        *pTxStatus=0;
        *pRxStatus=0;

        return RT_ERR_OK;
    }
    else if(port>=CA_PORT_ID_CPU0 && port<=CA_PORT_ID_CPU7)
    {
        *pTxStatus=0;
        *pRxStatus=0;

        return RT_ERR_OK;
    }

    if((ret = aal_port_physical_port_enable_get(0,port,&ps)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    if(ps == 1)
    {
        if((ret = ca_eth_port_pause_get(0,port_id,&pfc_enable,&pause_rx,&pause_tx)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        *pTxStatus=pause_tx;
        *pRxStatus=pause_rx;
    }
    else
    {
        *pTxStatus=0;
        *pRxStatus=0;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_flowctrl_get */

/* Function Name:
 *      dal_ca8277b_port_phyAutoNegoEnable_get
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
dal_ca8277b_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t ca_enable;
    ca_boolean_t ps=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(HAL_IS_PON_PORT(port))
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    if((ret = aal_port_physical_port_enable_get(0,port,&ps)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(ps == 1)
    {
        if((ret = ca_eth_port_autoneg_get(0,port_id,&ca_enable)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        *pEnable = (ca_enable==1)? ENABLED:DISABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyAutoNegoEnable_get */

/* Function Name:
 *      dal_ca8277b_port_phyAutoNegoEnable_set
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
dal_ca8277b_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
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
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    if(HAL_IS_SERDES_PORT(port))
    {
        return RT_ERR_OK;
    }

    ca_enable = (enable==ENABLED)? 1:0;

    if((ret = ca_eth_port_autoneg_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_eth_port_phy_autoneg_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_ca8277b_port_phyAutoNegoAbility_get
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
dal_ca8277b_port_phyAutoNegoAbility_get(
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

    if(HAL_IS_PON_PORT(port))
    {
        pAbility->Half_10=0;
        pAbility->Full_10=0;
        pAbility->Half_100=0;
        pAbility->Full_100=0;
        pAbility->Full_1000=0;
        pAbility->Full_10000=1;
        pAbility->FC=0;
        pAbility->AsyFC=0;
        return RT_ERR_OK;
    }
    else if(HAL_IS_SERDES_PORT(port))
    {
        pAbility->Half_10=0;
        pAbility->Full_10=0;
        pAbility->Half_100=0;
        pAbility->Full_100=0;
        pAbility->Full_1000=0;
        pAbility->Full_10000=1;
        pAbility->FC=0;
        pAbility->AsyFC=0;
        return RT_ERR_OK;
    }
    else if((ret = ca_eth_port_phy_advert_get(0,port_id,&ability)) != CA_E_OK)
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
} /* end of dal_ca8277b_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_ca8277b_port_phyAutoNegoAbility_set
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
dal_ca8277b_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_eth_port_ability_t ability;
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
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    if(HAL_IS_SERDES_PORT(port))
    {
        return RT_ERR_OK;
    }

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

    if((ret = ca_eth_port_pause_get(0,port_id,&ca_pfc_enable,&ca_pause_rx,&ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    ca_pause_tx = pAbility->FC;
    ca_pause_rx = pAbility->FC;

    if((ret = ca_eth_port_pause_set(0,port_id,ca_pfc_enable,ca_pause_rx,ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_eth_phy_auto_neg_restart(0,eth_mdio_addr[port])) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_ca8277b_port_phyForceModeAbility_get
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
dal_ca8277b_port_phyForceModeAbility_get(
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

    if(HAL_IS_PON_PORT(port))
    {
        *pSpeed=PORT_SPEED_10G;
        *pDuplex = PORT_FULL_DUPLEX;
        *pFlowControl = DISABLED;
        return RT_ERR_OK;
    }

    if(HAL_IS_SERDES_PORT(port))
    {
        *pSpeed=PORT_SPEED_1000M;
        *pDuplex = PORT_FULL_DUPLEX;
        *pFlowControl = DISABLED;
        return RT_ERR_OK;
    }

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

    *pFlowControl = ((pause_tx==1) || (pause_rx==1))? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_ca8277b_port_phyForceModeAbility_set
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
dal_ca8277b_port_phyForceModeAbility_set(
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
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    if(HAL_IS_SERDES_PORT(port))
    {
        return RT_ERR_OK;
    }

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

    ca_pause_tx = (flowControl==ENABLED)? 1:0;
    ca_pause_rx = (flowControl==ENABLED)? 1:0;

    if((ret = ca_eth_port_pause_set(0,port_id,ca_pfc_enable,ca_pause_rx,ca_pause_tx)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_ca8277b_port_phyReg_get
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
dal_ca8277b_port_phyReg_get(
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
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    st_code = 0; //clause 22
    reg_dev_addr = reg;
    addr = 0;
    /* Write page first */
    if((ret = cap_mdio_write(0,st_code,eth_mdio_addr[port],31,addr,page)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    if((ret = cap_mdio_read(0,st_code,eth_mdio_addr[port],reg_dev_addr,addr,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pData = data;

    return ret;
} /* end of dal_ca8277b_port_phyReg_get */

/* Function Name:
 *      dal_ca8277b_port_phyReg_set
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
dal_ca8277b_port_phyReg_set(
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
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    st_code = 0; //clause 22
    reg_dev_addr = reg;
    addr = 0;
    ca_data = data;
    /* Write page first */
    if((ret = cap_mdio_write(0,st_code,eth_mdio_addr[port],31,addr,page)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    if((ret = cap_mdio_write(0,st_code,eth_mdio_addr[port],reg_dev_addr,addr,ca_data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_ca8277b_port_phyReg_set */

/* Function Name:
 *      dal_ca8277b_port_adminEnable_get
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
dal_ca8277b_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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

    if(!HAL_IS_PON_PORT(port) && HAL_IS_SERDES_PORT(port))
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    if(port>=AAL_NI_PORT_GE0 && port<=AAL_NI_PORT_GE3)
    {
        aal_ni_eth_mac_config_t eth_mac_config;

        memset(&eth_mac_config, 0, sizeof(aal_ni_eth_mac_config_t));

        if((ret = aal_ni_eth_port_mac_get(0, port, &eth_mac_config)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        enable = eth_mac_config.tx_en & eth_mac_config.rx_en;

        if(enable == 1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;
    }
    else
    {
        if((ret = ca_port_enable_get(0,port_id,&enable)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(enable == 1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_adminEnable_get */

/* Function Name:
 *      dal_ca8277b_port_adminEnable_set
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
dal_ca8277b_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
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

    if(!HAL_IS_PON_PORT(port) && HAL_IS_SERDES_PORT(port))
    {
        return RT_ERR_OK;
    }

    if(enable == ENABLED)
        ca_enable = 1;
    else
        ca_enable = 0;
    
    if(port>=AAL_NI_PORT_GE0 && port<=AAL_NI_PORT_GE3)
    {
        aal_ni_eth_mac_config_mask_t eth_mac_mask;
        aal_ni_eth_mac_config_t eth_mac_config;

        memset(&eth_mac_mask, 0, sizeof(aal_ni_eth_mac_config_mask_t));
        memset(&eth_mac_config, 0, sizeof(aal_ni_eth_mac_config_t));

        eth_mac_mask.s.mac_tx_rst = 1;
        eth_mac_mask.s.tx_en      = 1;
        eth_mac_mask.s.mac_rx_rst = 1;
        eth_mac_mask.s.rx_en      = 1;

        eth_mac_config.mac_tx_rst = !ca_enable;
        eth_mac_config.tx_en      = ca_enable;
        eth_mac_config.mac_rx_rst = !ca_enable;
        eth_mac_config.rx_en      = ca_enable;

        if((ret = aal_ni_eth_port_mac_set(0, port, eth_mac_mask, &eth_mac_config)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    if((ret = ca_port_enable_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_adminEnable_set */

/* Function Name:
 *      dal_ca8277b_port_phyPowerDown_get
 * Description:
 *      Get PHY power down state of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to power down enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_ca8277b_port_phyPowerDown_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret=RT_ERR_OK;
    ca_port_id_t port_id;
    uint32 data;
    ca_boolean_t ps=0;
    ca_boolean_t power_up=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

//    if(HAL_IS_SERDES_PORT(port))
//    {
//        
//        return RT_ERR_OK;
//    }

    if(port>=AAL_NI_PORT_GE0 && port<=AAL_NI_PORT_GE3)
    {
        if((ret = dal_ca8277b_port_phyReg_get(port,0,0,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(data&(1<<11))
        {
            *pEnable = ENABLED;
        }
        else
        {
            *pEnable = DISABLED;
        }
    }
    else
    {
        if((ret = aal_port_physical_port_enable_get(0,port,&ps)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(ps == 1)
        {
            if((ret = aal_eth_phy_power_get(0,eth_mdio_addr[port],&power_up)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            if(power_up==1)
            {
                *pEnable = DISABLED;
            }
            else
            {
                *pEnable = ENABLED;
            }
        }
        else
        {
            if(!HAL_IS_SERDES_PORT(port))
                return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyPowerDown_get */

/* Function Name:
 *      dal_ca8277b_port_phyPowerDown_set
 * Description:
 *      Set PHY power down state of the specified port.
 * Input:
 *      port   - port id
 *      enable - PHY power down enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      none
 */
int32
dal_ca8277b_port_phyPowerDown_set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret=CA_E_OK;
    ca_port_id_t port_id;
    uint32 data;
    ca_boolean_t ps=0;
    ca_boolean_t power_up=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

//    if(HAL_IS_SERDES_PORT(port))
//    {
//        
//        return RT_ERR_OK;
//    }

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    if(port>=AAL_NI_PORT_GE0 && port<=AAL_NI_PORT_GE3)
    {
        if((ret = dal_ca8277b_port_phyReg_get(port,0,0,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(enable == ENABLED)
        {
            data |= (1<<11);

            if((ret = dal_ca8277b_port_adminEnable_set(port,DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }
        else
        {
            data &= ~(1<<11);

            if((ret = dal_ca8277b_port_adminEnable_set(port,ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }

        if((ret = dal_ca8277b_port_phyReg_set(port,0,0,data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        
    }
    else
    {
        if((ret = aal_port_physical_port_enable_get(0,port,&ps)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(ps == 1)
        {
            power_up =(enable == ENABLED)?0:1;
            if((ret = aal_eth_phy_power_set(0,eth_mdio_addr[port],power_up)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }
        else
        {
            if(!HAL_IS_SERDES_PORT(port))
                return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_port_phyPowerDown_set */

/* Function Name:
 *      dal_ca8277b_port_macForceAbility_get
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
dal_ca8277b_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
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

    if(HAL_IS_PON_PORT(port))
    {
        pMacAbility->speed=PORT_SPEED_10G;
        pMacAbility->duplex = PORT_FULL_DUPLEX;
        pMacAbility->linkStatus = PORT_LINKUP;
        pMacAbility->txFc = DISABLED;
        pMacAbility->rxFc = DISABLED;
        return RT_ERR_OK;
    }
    else if(HAL_IS_SERDES_PORT(port))
    {
        pMacAbility->speed=PORT_SPEED_1000M;
        pMacAbility->duplex = PORT_FULL_DUPLEX;
        pMacAbility->linkStatus = PORT_LINKDOWN;
        pMacAbility->txFc = DISABLED;
        pMacAbility->rxFc = DISABLED;
        return RT_ERR_OK;
    }

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
} /* end of dal_ca8277b_port_macForceAbility_get */

/* Function Name:
 *      dal_ca8277b_port_macForceAbility_set
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
dal_ca8277b_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
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
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    if(HAL_IS_SERDES_PORT(port))
    {
        
        return RT_ERR_OK;
    }

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
} /* end of dal_ca8277b_port_macForceAbility_set */

/* Function Name:
 *      dal_ca8277b_port_macForceAbilityState_get
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
dal_ca8277b_port_macForceAbilityState_get(rtk_port_t port,rtk_enable_t *pState)
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

    if(HAL_IS_PON_PORT(port))
    {
        *pState = ENABLED;
        return RT_ERR_OK;
    }
    else if(HAL_IS_SERDES_PORT(port))
    {
        *pState = DISABLED;
        return RT_ERR_OK;
    }

    if((ret = ca_eth_port_autoneg_get(0,port_id,&ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pState = (ca_enable==1)? DISABLED:ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8277b_port_macForceAbilityState_set
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
dal_ca8277b_port_macForceAbilityState_set(rtk_port_t port,rtk_enable_t state)
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
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_CHIP_NOT_SUPPORTED);

    if(HAL_IS_SERDES_PORT(port))
    {

        return RT_ERR_OK;
    }

    ca_enable = (state==ENABLED)? 0:1;

    if((ret = ca_eth_port_autoneg_set(0,port_id,ca_enable)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8277b_port_isolationEntry_get
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
dal_ca8277b_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
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
} /* end of dal_ca8277b_port_isolationEntry_get */

/* Function Name:
 *      dal_ca8277b_port_isolationEntry_set
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
dal_ca8277b_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
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
} /* end of dal_ca8277b_port_isolationEntry_set */


#if defined(CONFIG_SDK_KERNEL_LINUX)
static int _ca8277b_lan_sds0_mode_set(uint32 mode)
{
    int ret;

    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_MAC:
        ret = aal_port_serdes_cfg_set(0, 4, AAL_PORT_SDS_MODE_SGMII_1G);
        break;
    case LAN_SDS_MODE_HSGMII_MAC:
        ret = aal_port_serdes_cfg_set(0, 4, AAL_PORT_SDS_MODE_HISGMII_2G5);
        break;
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if(ret)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif

static uint8 lan_sds_mode=LAN_SDS_MODE_END;

/* Function Name:
 *      dal_ca8277b_port_serdesMode_set
 * Description:
 *      Set Serdes Mode
 * Input:
 *      num   - serdes number
 *      cfg   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_ca8277b_port_serdesMode_set(uint8 num, uint8 cfg)
{
    int32 ret;
    uint32 chip, rev, subtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d,cfg=%d",num, cfg);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((1 <=num), RT_ERR_INPUT);
    RT_PARAM_CHK((LAN_SDS_MODE_END <=cfg), RT_ERR_INPUT);

    /* function body */
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if((ret = _ca8277b_lan_sds0_mode_set(cfg)) != RT_ERR_OK)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
#endif
    lan_sds_mode = cfg;

    return RT_ERR_OK;
}   /* end of dal_ca8277b_port_serdesMode_set */



/* Function Name:
 *      dal_ca8277b_port_serdesMode_get
 * Description:
 *      Get Serdes Mode
 * Input:
 *      num   - serdes number
 * Output:
 *      cfg   - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_ca8277b_port_serdesMode_get(uint8 num, uint8 *cfg)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d",num);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((2 <=num), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == cfg), RT_ERR_NULL_POINTER);

    /* function body */
    *cfg = lan_sds_mode;
    
    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_serdesMode_get */

