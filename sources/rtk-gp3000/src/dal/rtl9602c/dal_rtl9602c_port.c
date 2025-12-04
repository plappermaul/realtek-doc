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
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/port.h>
#include <dal/rtl9602c/dal_rtl9602c_port.h>
#include <dal/rtl9602c/dal_rtl9602c_ponmac.h>
#include <osal/lib.h>
#include <hal/common/miim.h>

/*
 * Symbol Definition
 */

typedef struct dal_rtl9602c_phy_info_s {
    uint8   force_mode_speed[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_duplex[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_flowControl[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_pause[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_asy_pause[RTK_MAX_NUM_OF_PORTS];
} dal_rtl9602c_phy_info_t;

/*
 * Data Declaration
 */
static uint32    port_init = INIT_NOT_COMPLETED;

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
static uint32 adminPhyConfig = DISABLED;
#endif

static dal_rtl9602c_phy_info_t   phy_info;

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9602c_port_init
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
dal_rtl9602c_port_init(void)
{
    int32 ret;
    rtk_port_t port;
    rtk_portmask_t allPortmask;
    rtk_portmask_t allExtPortmask;
    rtk_port_phy_ability_t phy_ability;
    rtk_port_macAbility_t macAbility;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    port_init = INIT_COMPLETED;

    osal_memset(&phy_info, 0x00, sizeof(dal_rtl9602c_phy_info_t));

    HAL_GET_ALL_PORTMASK(allPortmask);
    HAL_GET_ALL_EXT_PORTMASK(allExtPortmask);

    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_PHY_EXIST(port))
        {
            phy_ability.Half_10     = ENABLED;
            phy_ability.Full_10     = ENABLED;
            phy_ability.Half_100    = ENABLED;
            phy_ability.Full_100    = ENABLED;
            phy_ability.Half_1000   = ENABLED;
            phy_ability.Full_1000   = ENABLED;
            phy_ability.FC          = ENABLED;
            phy_ability.AsyFC       = ENABLED;

            if((ret = dal_rtl9602c_port_phyAutoNegoAbility_set(port, &phy_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_rtl9602c_port_phyAutoNegoEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_rtl9602c_port_greenEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_rtl9602c_port_phyCrossOverMode_set(port, PORT_CROSSOVER_MODE_AUTO)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl9602c_port_macRemoteLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9602c_port_macLocalLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9602c_port_specialCongest_set(port, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /* Force CPU port */
        if(port == HAL_GET_CPU_PORT())
        {
            osal_memset(&macAbility, 0x00, sizeof(rtk_port_macAbility_t));
            macAbility.speed           = PORT_SPEED_1000M;
            macAbility.duplex          = PORT_FULL_DUPLEX;
            macAbility.linkFib1g       = DISABLED;
            macAbility.linkStatus      = PORT_LINKUP;
            macAbility.txFc            = DISABLED;
            macAbility.rxFc            = DISABLED;
            macAbility.nwayAbility     = DISABLED;
            macAbility.masterMod       = DISABLED;
            macAbility.nwayFault       = DISABLED;
            macAbility.lpi_100m        = DISABLED;
            macAbility.lpi_giga        = DISABLED;
            if((ret = dal_rtl9602c_port_macForceAbility_set(port, macAbility)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            data = 0xFFF;

            if((ret = reg_array_write(RTL9602C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        else
        {
            if((ret = dal_rtl9602c_port_adminEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

#if 1
{
    uint32 value;
    /* Temporary patch for MP chip due to PHY interrupt error */
    dal_rtl9602c_port_phyReg_set(1, 0xb84, 18, 0xfffb);
    dal_rtl9602c_port_phyReg_get(1, 0xb90, 19, &value);
    dal_rtl9602c_port_phyReg_get(1, 0xb90, 19, &value);
}
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      dal_rtl9602c_port_link_get
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
dal_rtl9602c_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_P_LINK_STATUSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    *pStatus = ((data == 1) ? PORT_LINKUP : PORT_LINKDOWN);
    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_link_get */

/* Function Name:
 *      dal_rtl9602c_port_speedDuplex_get
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
dal_rtl9602c_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32  ret;
    uint32 speed;
    uint32 duplex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_P_LINK_SPDf, &speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_P_DUPLEXf, &duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    switch(speed)
    {
        case 0:
            *pSpeed = PORT_SPEED_10M;
            break;
        case 1:
            *pSpeed = PORT_SPEED_100M;
            break;
        case 2:
            *pSpeed = PORT_SPEED_1000M;
            break;
        case 3:
            *pSpeed = PORT_SPEED_500M;
            break;

        default:
            return RT_ERR_FAILED;
            break;
    }

    *pDuplex = ((1 == duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);
    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_speedDuplex_get */

/* Function Name:
 *      dal_rtl9602c_port_flowctrl_get
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
dal_rtl9602c_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_P_TX_FCf, pTxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_P_RX_FCf, pRxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_flowctrl_get */

/* Function Name:
 *      dal_rtl9602c_port_phyAutoNegoEnable_get
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
dal_rtl9602c_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = phy_autoNegoEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyAutoNegoEnable_get */

/* Function Name:
 *      dal_rtl9602c_port_phyAutoNegoEnable_set
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
dal_rtl9602c_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32  ret;
    rtk_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        ability.FC = phy_info.auto_mode_pause[port];
        ability.AsyFC = phy_info.auto_mode_asy_pause[port];

        if ((ret = phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    if ((ret = phy_autoNegoEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (DISABLED == enable)
    {
        if ((ret = phy_duplex_set(port, phy_info.force_mode_duplex[port])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_speed_set(port, phy_info.force_mode_speed[port])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        ability.FC = phy_info.force_mode_flowControl[port];
        ability.AsyFC = phy_info.force_mode_flowControl[port];

        if ((ret = phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_rtl9602c_port_phyAutoNegoAbility_get
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
dal_rtl9602c_port_phyAutoNegoAbility_get(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32  ret;
    rtk_enable_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if ((ret = dal_rtl9602c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    if ((ret = phy_autoNegoAbility_get(port, pAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if (DISABLED == enable)
    {
        /* Force mode currently, return database status */
        pAbility->FC    = phy_info.auto_mode_pause[port];
        pAbility->AsyFC = phy_info.auto_mode_asy_pause[port];
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_rtl9602c_port_phyAutoNegoAbility_set
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
dal_rtl9602c_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32   ret;
    uint32  value;
    rtk_enable_t enable;
    rtk_ponmac_mode_t ponMode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if ((ret = dal_rtl9602c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "%s(port=%d) failed!!", __FUNCTION__, port);
        return ret;
    }

    if (DISABLED == enable)
    {
        /* PHY is in force mode currently, keep user configuration and set Force ability */
        phy_info.auto_mode_pause[port] = pAbility->FC;
        phy_info.auto_mode_asy_pause[port] = pAbility->AsyFC;
        pAbility->FC = phy_info.force_mode_flowControl[port];
        pAbility->AsyFC = phy_info.force_mode_flowControl[port];
    }

    if ((ret = phy_autoNegoAbility_set(port, pAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        /* PHY is in auto-nego mode, store user configuration to database */
        phy_info.auto_mode_pause[port] = pAbility->FC;
        phy_info.auto_mode_asy_pause[port] = pAbility->AsyFC;
    }

    ret = dal_rtl9602c_ponmac_mode_get(&ponMode);
    /* Perform fiber flow control only for FIBER modes */
    if((RT_ERR_OK == ret) &&
       (PONMAC_MODE_GPON != ponMode) &&
       (PONMAC_MODE_EPON != ponMode) &&
       (HAL_IS_PON_PORT(port)))
    {
        /* for PON port, it is possible that the actual application is fiber
         * Configure fiber register too
         */
        /* Force select giga registers */
        value = 1;
        if ((ret = reg_field_write(RTL9602C_WSDS_DIG_02r, RTL9602C_FRC_REG4_ENf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        value = 0;
        if ((ret = reg_field_write(RTL9602C_WSDS_DIG_02r, RTL9602C_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Configure pause field */
        if ((ret = reg_field_read(RTL9602C_FIB_REG4r, RTL9602C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        if(1 == pAbility->FC)
        {
            /* Turn on pause by set bit [7] */
            value |= 0x80;
        }
        else
        {
            /* Turn on pause by clear bit [7] */
            value &= ~(0x80);
        }
        if(1 == pAbility->AsyFC)
        {
            /* Turn on pause by set bit [8] */
            value |= 0x100;
        }
        else
        {
            /* Turn on pause by clear bit [8] */
            value &= ~(0x100);
        }
        if ((ret = reg_field_write(RTL9602C_FIB_REG4r, RTL9602C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Force select 100M registers */
        value = 1;
        if ((ret = reg_field_write(RTL9602C_WSDS_DIG_02r, RTL9602C_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Configure pause field */
        if ((ret = reg_field_read(RTL9602C_FIB_REG4r, RTL9602C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        if(1 == pAbility->FC)
        {
            /* Turn on pause by set bit [7] */
            value |= 0x80;
        }
        else
        {
            /* Turn on pause by clear bit [7] */
            value &= ~(0x80);
        }
        if(1 == pAbility->AsyFC)
        {
            /* Turn on pause by set bit [8] */
            value |= 0x100;
        }
        else
        {
            /* Turn on pause by clear bit [8] */
            value &= ~(0x100);
        }

        if ((ret = reg_field_write(RTL9602C_FIB_REG4r, RTL9602C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Release force register */
        value = 0;
        if ((ret = reg_field_write(RTL9602C_WSDS_DIG_02r, RTL9602C_FRC_REG4_ENf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        value = 0;
        if ((ret = reg_field_write(RTL9602C_WSDS_DIG_02r, RTL9602C_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }

        /* Restart N-Way manually */
        value = 1;
        if ((ret = reg_field_write(RTL9602C_FIB_REG0r, RTL9602C_FP_CFG_FIB_RESTARTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_rtl9602c_port_phyForceModeAbility_get
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
dal_rtl9602c_port_phyForceModeAbility_get(
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl)
{
    int32   ret;
    rtk_enable_t enable;
    rtk_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFlowControl), RT_ERR_NULL_POINTER);

    if ((ret = dal_rtl9602c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        *pSpeed = phy_info.force_mode_speed[port];
        *pDuplex = phy_info.force_mode_duplex[port];
        *pFlowControl = phy_info.force_mode_flowControl[port];
    }
    else
    {
        if ((ret = phy_speed_get(port, pSpeed)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_duplex_get(port, pDuplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        *pFlowControl = ability.FC;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_rtl9602c_port_phyForceModeAbility_set
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
dal_rtl9602c_port_phyForceModeAbility_set(
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{

    int32   ret;
    rtk_enable_t enable;
    rtk_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_DUPLEX_END <= duplex), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= flowControl), RT_ERR_INPUT);

    if ((ret = dal_rtl9602c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    phy_info.force_mode_speed[port] = speed;
    phy_info.force_mode_duplex[port] = duplex;
    phy_info.force_mode_flowControl[port] = flowControl;

    if (DISABLED == enable)
    {
        if ((ret = phy_speed_set(port, speed)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_duplex_set(port, duplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (ENABLED == flowControl)
        {
            ability.FC = ENABLED;
            ability.AsyFC = ENABLED;
        }
        else
        {
            ability.FC = DISABLED;
            ability.AsyFC = DISABLED;
        }

        if ((ret = phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_rtl9602c_port_phyReg_get
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
dal_rtl9602c_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK(((page < HAL_MIIM_PAGE_ID_MIN()) && page != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* Check Link status */
    ret = hal_miim_read(port, page, reg, pData);
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pData=0x%x", *pData);

    return ret;
} /* end of dal_rtl9602c_port_phyReg_get */

/* Function Name:
 *      dal_rtl9602c_port_phyReg_set
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
dal_rtl9602c_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK(((page < HAL_MIIM_PAGE_ID_MIN()) && page != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((data > HAL_MIIM_DATA_MAX()), RT_ERR_INPUT);

    ret = hal_miim_write(port, page, reg, data);

    return ret;
} /* end of dal_rtl9602c_port_phyReg_set */

/* Function Name:
 *      dal_rtl9602c_port_phyMasterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 * Output:
 *      pMasterSlave        - pointer to the PHY master slave configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_phyMasterSlave_get(
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMasterSlave), RT_ERR_NULL_POINTER);

    *pMasterSlave = PORT_AUTO_MODE;
    return RT_ERR_OK;
}/* end of dal_rtl9602c_port_phyMasterSlave_get */

/* Function Name:
 *      dal_rtl9602c_port_phyMasterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_phyMasterSlave_set(
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(masterSlave >= PORT_MASTER_SLAVE_END, RT_ERR_INPUT);


    return RT_ERR_OK;
}/* end of dal_rtl9602c_port_phyMasterSlave_set */

/* Function Name:
 *      dal_rtl9602c_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      port - Port id.
 * Output:
 *      pTestMode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get test mode of PHY from register setting 9.15 to 9.13.
 */
int32
dal_rtl9602c_port_phyTestMode_get(rtk_port_t port, rtk_port_phy_test_mode_t *pTestMode)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTestMode), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = dal_rtl9602c_port_phyReg_get(port, 0xA41, 9, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pTestMode = (data & 0xE000) >> 13;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_phyTestMode_get */

/* Function Name:
 *      dal_rtl9602c_port_phyTestMode_set
 * Description:
 *      Set PHY in test mode.
 * Input:
 *      port - port id.
 *      testMode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      None.
 */
int32
dal_rtl9602c_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t testMode)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d,testMode=%d",port, testMode);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PHY_TEST_MODE_END <=testMode), RT_ERR_INPUT);

    /* function body */
    /* Test mode 2 & 3 are not supported */
    if( (PHY_TEST_MODE_2 == testMode) || (PHY_TEST_MODE_3 == testMode) )
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Set test mode */
    if((ret = dal_rtl9602c_port_phyReg_get(port, 0xA41, 9, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    data &= 0x1FFF;
    data |= (testMode << 13);

    if((ret = dal_rtl9602c_port_phyReg_set(port, 0xA41, 9, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_phyTestMode_set */

/* Function Name:
 *      dal_rtl9602c_port_cpuPortId_get
 * Description:
 *      Get CPU port id
 * Input:
 *      None
 * Output:
 *      pPort - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_cpuPortId_get(rtk_port_t *pPort)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    *pPort = (rtk_port_t)HAL_GET_CPU_PORT();
    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_cpuPortId_get */

/* Function Name:
 *      dal_rtl9602c_port_macRemoteLoopbackEnable_get
 * Description:
 *      Get the mac remote loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac remote loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
dal_rtl9602c_port_macRemoteLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_macRemoteLoopbackEnable_get */

/* Function Name:
 *      dal_rtl9602c_port_macRemoteLoopbackEnable_set
 * Description:
 *      Set the mac remote loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac remote loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
dal_rtl9602c_port_macRemoteLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_macRemoteLoopbackEnable_set */

/* Function Name:
 *      dal_rtl9602c_port_macLocalLoopbackEnable_get
 * Description:
 *      Get the mac local loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac local loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
dal_rtl9602c_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9602C_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)data ;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_macLocalLoopbackEnable_get */

/* Function Name:
 *      dal_rtl9602c_port_macLocalLoopbackEnable_set
 * Description:
 *      Set the mac local loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac local loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
dal_rtl9602c_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    data = (uint32)enable;
    if((ret = reg_array_field_write(RTL9602C_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_macLocalLoopbackEnable_set */

/* Function Name:
 *      dal_rtl9602c_port_adminEnable_get
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
dal_rtl9602c_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 data;
    uint32 link;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9602C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, RTL9602C_FORCE_LINK_ABLTYf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(ENABLED == data)
    {
        if((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_LINK_ABLTYf, &link)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if(PORT_LINKDOWN == link)
            *pEnable = DISABLED;
        else
            *pEnable = ENABLED;
    }
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_adminEnable_get */

/* Function Name:
 *      dal_rtl9602c_port_adminEnable_set
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
dal_rtl9602c_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(DISABLED == enable)
    {
        data = PORT_LINKDOWN;
        if((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        data = ENABLED;
        if ((ret = reg_array_field_write(RTL9602C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, RTL9602C_FORCE_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        data = DISABLED;
        if ((ret = reg_array_field_write(RTL9602C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, RTL9602C_FORCE_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_adminEnable_set */

/* Function Name:
 *      dal_rtl9602c_port_specialCongest_get
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSecond - congest timer (seconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_specialCongest_get(rtk_port_t port, uint32 *pSecond)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSecond), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9602C_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_CGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_specialCongest_get */

/* Function Name:
 *      dal_rtl9602c_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_specialCongest_set(rtk_port_t port, uint32 second)
{
    int32  ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((second > HAL_MAX_SPECIAL_CONGEST_SEC()), RT_ERR_INPUT);

    data = second;
    if((ret = reg_array_field_write(RTL9602C_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_CGST_SUST_TMR_LMTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if((ret = reg_array_field_write(RTL9602C_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_CGST_SUST_TMR_LMT_Hf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_specialCongest_set */

/* Function Name:
 *      dal_rtl9602c_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      pStatus - Congest status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_specialCongestStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(RTL9602C_SC_P_CTRL_1r, RTL9602C_CGST_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(regData & (0x01 << port))
        *pStatus = 1;
    else
        *pStatus = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_specialCongestStatus_clear(rtk_port_t port)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    regData = (0x01 << port);
    if((ret = reg_field_write(RTL9602C_SC_P_CTRL_1r, RTL9602C_CGST_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_greenEnable_get
 * Description:
 *      Get the statue of green feature of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_greenEnable_get */

/* Function Name:
 *      dal_rtl9602c_port_greenEnable_set
 * Description:
 *      Set the statue of green feature of the specific port
 * Input:
 *      port   - port id
 *      enable - status of green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_greenEnable_set */


/* Function Name:
 *      dal_rtl9602c_port_phyCrossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
dal_rtl9602c_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyCrossOverMode_get */

/* Function Name:
 *      dal_rtl9602c_port_phyCrossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
dal_rtl9602c_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_CROSSOVER_MODE_END <= mode), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyCrossOverMode_set */

/* Function Name:
 *      dal_rtl9602c_port_phyPowerDown_get
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
dal_rtl9602c_port_phyPowerDown_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    unsigned int data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = dal_rtl9602c_port_phyReg_get(port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (data & PowerDown_MASK) ? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyPowerDown_get */

/* Function Name:
 *      dal_rtl9602c_port_phyPowerDown_set
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
dal_rtl9602c_port_phyPowerDown_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    unsigned int data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* Check if it using LAN Serdes */
    if(0 == port)
    {
        /* Only port 0 support LAN Serdes */
        if((ret = reg_field_read(RTL9602C_HSG_SDS_CTRLr, RTL9602C_HSG_SDS_MODEf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
        if(0x1F != data)
        {
            /* The port is runing under LAN Serdes mode, can't configure PHY status */
            return RT_ERR_PORT_ID;
        }
    }

    if((ret = dal_rtl9602c_port_phyReg_get(port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(ENABLED == enable)
    {
        data |= PowerDown_MASK;
    }
    else
    {
        data &= ~(PowerDown_MASK);
    }

    if((ret = dal_rtl9602c_port_phyReg_set(port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    adminPhyConfig = enable;
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_phyPowerDown_set */

/* Function Name:
 *      dal_rtl9602c_port_phyPowerDown_tryset
 * Description:
 *      Try to set PHY power down state of the specified port.
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
 *      This API will try to enable/disable specified port's phy power
 * It might failed due to admin not allow enable/disable.
 * The admin allow enable/disable only after
 * rtk_port_phyPowerDown_set being called since boot
 * This API is used for CONFIG_SWITCH_INIT_LINKDOWN config
 * Without CONFIG_SWITCH_INIT_LINKDOWN, it works exectly the same as
 * rtk_port_phyPowerDown_set
 */
int32
dal_rtl9602c_port_phyPowerDown_tryset(rtk_port_t port, rtk_enable_t enable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    RT_PARAM_CHK((adminPhyConfig == DISABLED), RT_ERR_FAILED);
#endif

    return dal_rtl9602c_port_phyPowerDown_set(port, enable);
} /* end of dal_rtl9602c_port_phyPowerDown_tryset */

/* Function Name:
 *      dal_rtl9602c_port_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32
dal_rtl9602c_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_ETHER_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRtctResult), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /*end of dal_rtl9602c_port_rtctResult_get*/

/* Function Name:
 *      dal_rtl9602c_port_rtct_start
 * Description:
 *      Start RTCT for ports.
 *      When enable RTCT, the port won't transmit and receive normal traffic.
 * Input:
 *      pPortmask - the ports for RTCT test
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
dal_rtl9602c_port_rtct_start(rtk_portmask_t *pPortmask)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_rtct_start */


/* Function Name:
 *      dal_rtl9602c_port_macForceAbility_set
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
dal_rtl9602c_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_SPEED_10G <= macAbility.speed) && !((PORT_SPEED_2G5 == macAbility.speed) && (port == 0)), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_DUPLEX_END <= macAbility.duplex), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.linkFib1g), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_LINKSTATUS_END <= macAbility.linkStatus), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.txFc), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.rxFc), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.nwayAbility), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.masterMod), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.nwayFault), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.lpi_100m), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.lpi_giga), RT_ERR_INPUT);

    if(0 == port)
    {
        if(macAbility.speed != PORT_SPEED_2G5)
        {
            value = 0x0;
            if((ret = reg_field_write(RTL9602C_LINE_RATE_2500Mr, RTL9602C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
            {
                return ret;
            }
        }
        else
        {
            /* Change speed to 1G and add additional LINE_RATE_2500M configure */
            macAbility.speed = PORT_SPEED_1000M;
            value = 0x1;
            if((ret = reg_field_write(RTL9602C_LINE_RATE_2500Mr, RTL9602C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
            {
                return ret;
            }
            value = 0x3ffff;
            if((ret = reg_field_write(RTL9602C_LINE_RATE_2500Mr, RTL9602C_RATEf, &value)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }
    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SPEED_ABLTYf, &macAbility.speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_DUPLEX_ABLTYf, &macAbility.duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_FIB1G_ABLTYf, &macAbility.linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_LINK_ABLTYf, &macAbility.linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RXPAUSE_ABLTYf, &macAbility.rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_TXPAUSE_ABLTYf, &macAbility.txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_NWAY_ABLTYf, &macAbility.nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MST_MOD_ABLTYf, &macAbility.masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MST_FAULT_ABLTYf, &macAbility.nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_LPI_100_ABLTYf, &macAbility.lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_LPI_1000_ABLTYf, &macAbility.lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9602c_port_macForceAbility_get
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
dal_rtl9602c_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMacAbility), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SPEED_ABLTYf, &pMacAbility->speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }
    if((pMacAbility->speed == PORT_SPEED_1000M) && (0 == port))
    {
        if((ret = reg_field_read(RTL9602C_LINE_RATE_2500Mr, RTL9602C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        if(value)
        {
            pMacAbility->speed = PORT_SPEED_2G5;
        }
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_DUPLEX_ABLTYf, &pMacAbility->duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_FIB1G_ABLTYf, &pMacAbility->linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_LINK_ABLTYf, &pMacAbility->linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RXPAUSE_ABLTYf, &pMacAbility->rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_TXPAUSE_ABLTYf, &pMacAbility->txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_NWAY_ABLTYf, &pMacAbility->nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MST_MOD_ABLTYf, &pMacAbility->masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MST_FAULT_ABLTYf, &pMacAbility->nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_LPI_100_ABLTYf, &pMacAbility->lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_LPI_1000_ABLTYf, &pMacAbility->lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9602c_port_macForceAbilityState_set
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
dal_rtl9602c_port_macForceAbilityState_set(rtk_port_t port,rtk_enable_t state)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    if(state == ENABLED)
        data = 0xFFF;
    else
        data = 0;

    if((ret = reg_array_write(RTL9602C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9602c_port_macForceAbilityState_get
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
dal_rtl9602c_port_macForceAbilityState_get(rtk_port_t port,rtk_enable_t *pState)
{
    int32 ret;
    uint32 data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if((ret = reg_array_read(RTL9602C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(data)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
}

#define RTL9602C_GIGALITE_ENABLE_OFFSET    (8)
#define RTL9602C_GIGALITE_ENABLE_MASK      (0x1U<<RTL9602C_GIGALITE_ENABLE_OFFSET)

#define RTL9602C_GIGALITE_SPEED_ABILITY0_OFFSET    (9)
#define RTL9602C_GIGALITE_SPEED_ABILITY0_MASK      (0x1U<<RTL9602C_GIGALITE_SPEED_ABILITY0_OFFSET)

#define RTL9602C_GIGALITE_SPEED_ABILITY1_OFFSET    (10)
#define RTL9602C_GIGALITE_SPEED_ABILITY1_MASK      (0x1U<<RTL9602C_GIGALITE_SPEED_ABILITY1_OFFSET)

#define RTL9602C_GIGALITE_SPEED_ABILITY2_OFFSET    (11)
#define RTL9602C_GIGALITE_SPEED_ABILITY2_MASK      (0x1U<<RTL9602C_GIGALITE_SPEED_ABILITY2_OFFSET)

#define RTL9602C_GIGALITE_SPEED_ABILITY_ALL_MASK      (RTL9602C_GIGALITE_SPEED_ABILITY0_MASK||RTL9602C_GIGALITE_SPEED_ABILITY1_MASK|RTL9602C_GIGALITE_SPEED_ABILITY1_MASK)
/* Function Name:
 *      dal_rtl9602c_port_gigaLiteEnable_set
 * Description:
 *      set giga-lite enable state
 * Input:
 *      port   - port id
 *      enable - enable status of giga-lite
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
dal_rtl9602c_port_gigaLiteEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d,enable=%d",port, enable);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable)
    {
        RT_PARAM_CHK((port>4), RT_ERR_PORT_ID);
    }
    /* function body */

    /* get value from CHIP*/

    if ((ret = hal_miim_read(port, 0xa4a, 17, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(RTL9602C_GIGALITE_ENABLE_MASK);
    phyData0 = phyData0 | (enable << RTL9602C_GIGALITE_ENABLE_OFFSET);

    if ((ret = hal_miim_write(port, 0xa4a, 17, phyData0)) != RT_ERR_OK)
        return ret;



    if ((ret = hal_miim_read(port, 0xa42, 20, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(RTL9602C_GIGALITE_SPEED_ABILITY_ALL_MASK);
    phyData0 = phyData0 | (enable << RTL9602C_GIGALITE_SPEED_ABILITY0_OFFSET) | (enable << RTL9602C_GIGALITE_SPEED_ABILITY1_OFFSET) | (enable << RTL9602C_GIGALITE_SPEED_ABILITY2_OFFSET);

    if ((ret = hal_miim_write(port, 0xa42, 20, phyData0)) != RT_ERR_OK)
        return ret;



    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_gigaLiteEnable_set */

/* Function Name:
 *      dal_rtl9602c_port_gigaLiteEnable_get
 * Description:
 *      Get RGMII TX/RX delay
 * Input:
 *      port   - port id
 * Output:
 *      pEnable - enable status of giga-lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_gigaLiteEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if(port > 4)
    {
        *pEnable=DISABLED;
        return RT_ERR_OK;
    }

    if ((ret = hal_miim_read(port, 0xa4a, 17, &phyData0)) != RT_ERR_OK)
        return ret;

    if(phyData0 & (RTL9602C_GIGALITE_ENABLE_MASK))
        *pEnable=ENABLED;
    else
        *pEnable=DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_gigaLiteEnable_get */

/* Function Name:
 *      rtl9602c_raw_port_forceDmp_set
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
int32 rtl9602c_raw_port_forceDmp_set(rtk_enable_t state)
{
    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_field_write(RTL9602C_EN_FORCE_P_DMPr, RTL9602C_FORCE_MODEf,&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

} /* end of rtl9602c_raw_port_forceDmp_set */

/* Function Name:
 *      rtl9602c_raw_port_forceDmp_get
 * Description:
 *      Get forwarding force mode
 * Input:
 *      None
 * Output:
 *      pState  - enable/disable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 rtl9602c_raw_port_forceDmp_get(rtk_enable_t *pState)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_EN_FORCE_P_DMPr, RTL9602C_FORCE_MODEf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

} /* end of rtl9602c_raw_port_forceDmp_get */

/* Function Name:
 *      rtl9602c_raw_port_forceDmpMask_set
 * Description:
 *      Set force mode port mask
 * Input:
 *      port      - port id
 *      mask    - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_DSL_VC
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 */
int32 rtl9602c_raw_port_forceDmpMask_set(rtk_port_t port, rtk_portmask_t mask)
{
    int32 ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(mask), RT_ERR_PORT_MASK);
    if ((ret = reg_array_field_write(RTL9602C_FORCE_P_DMPr, REG_ARRAY_INDEX_NONE, port, RTL9602C_FORCE_PROT_MASKf, mask.bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

} /* end of rtl9602c_raw_port_forceDmpMask_set */

/* Function Name:
 *      rtl9602c_raw_port_forceDmpMask_get
 * Description:
 *      Get force mode port mask
 * Input:
 *      port      - port id
 * Output:
 *      pMask       - port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 rtl9602c_raw_port_forceDmpMask_get(rtk_port_t port, rtk_portmask_t  *pMask)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_FORCE_P_DMPr, REG_ARRAY_INDEX_NONE, port, RTL9602C_FORCE_PROT_MASKf, pMask->bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

} /* end of rtl9602c_raw_port_forceDmpMask_get */

/* Function Name:
 *      dal_rtl9602c_port_isolationEntry_get
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
dal_rtl9602c_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    if(mode) { }

    regAddr = RTL9602C_PISO_PORTr;
    if((ret = reg_array_field_read(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x000F;
    pExtPortmask->bits[0] = (regData & 0x07f0) >> 4;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_isolationEntry_get */

/* Function Name:
 *      dal_rtl9602c_port_isolationEntry_set
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
dal_rtl9602c_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    if(mode) { }

    regAddr = RTL9602C_PISO_PORTr;

    regData = ((pExtPortmask->bits[0] << 4) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9602C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_isolationEntry_set */

/* Function Name:
 *      dal_rtl9602c_port_isolationEntryExt_get
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
dal_rtl9602c_port_isolationEntryExt_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    regAddr = RTL9602C_PISO_EXTr;

    if((ret = reg_array_field_read(RTL9602C_PISO_EXTr, REG_ARRAY_INDEX_NONE, (int32)port, RTL9602C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x000F;
    pExtPortmask->bits[0] = (regData & 0x07f0) >> 4;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_isolationEntryExt_get */

/* Function Name:
 *      dal_rtl9602c_port_isolationEntryExt_set
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
dal_rtl9602c_port_isolationEntryExt_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    regAddr = RTL9602C_PISO_EXTr;

    regData = ((pExtPortmask->bits[0] << 4) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, REG_ARRAY_INDEX_NONE, (int32)port, RTL9602C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_isolationEntryExt_set */


/* Function Name:
 *      dal_rtl9602c_port_isolationIpmcLeaky_get
 * Description:
 *      Get the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pEnable   - status of port isolation leaky for ip multicast packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_rtl9602c_port_isolationIpmcLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_isolationIpmcLeaky_get */

/* Function Name:
 *      dal_rtl9602c_port_isolationIpmcLeaky_set
 * Description:
 *      Set the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 *      enable    - status of port isolation leaky for ip multicast packets
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
dal_rtl9602c_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(RTL9602C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_port_isolationIpmcLeaky_set */



/* Function Name:
 *      dal_rtl9602c_port_isolationPortLeaky_get
 * Description:
 *      Get the per port isolation leaky state for given type
 * Input:
 *      port      - port id
 *      type      - Packet type for isolation leaky.
 * Output:
 *      pEnable   - status of port isolation leaky for given leaky type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_rtl9602c_port_isolationPortLeaky_get(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d,type=%d",port, type);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LEAKY_IPMULTICAST !=type), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_array_field_read(RTL9602C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;
    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_isolationPortLeaky_get */

/* Function Name:
 *      dal_rtl9602c_port_isolationPortLeaky_set
 * Description:
 *      Set the per port isolation leaky state for given type
 * Input:
 *      port      - port id
 *      type      - Packet type for isolation leaky.
 *      enable    - status of port isolation leaky for given leaky type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_rtl9602c_port_isolationPortLeaky_set(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d,type=%d,enable=%d",port, type, enable);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LEAKY_IPMULTICAST !=type), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    val = (uint32)enable;
    if ((ret = reg_array_field_write(RTL9602C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_isolationPortLeaky_set */


static int32
_dal_rtl9602c_port_rmaPortIsolationLeakyEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);


    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
        switch(pRmaFrame->octet[5])
        {
            case 0xCC:
                regAddr = RTL9602C_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = RTL9602C_RMA_CTRL_SSTPr;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

        switch(pRmaFrame->octet[5])
        {
            case 0x00:
                regAddr = RTL9602C_RMA_CTRL00r;
                break;
            case 0x01:
                regAddr = RTL9602C_RMA_CTRL01r;
                break;
            case 0x02:
                regAddr = RTL9602C_RMA_CTRL02r;
                break;
            case 0x03:
                regAddr = RTL9602C_RMA_CTRL03r;
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
                regAddr = RTL9602C_RMA_CTRL04r;
                break;
            case 0x08:
                regAddr = RTL9602C_RMA_CTRL08r;
                break;
            case 0x0D:
                regAddr = RTL9602C_RMA_CTRL0Dr;
                break;
            case 0x0E:
                regAddr = RTL9602C_RMA_CTRL0Er;
                break;
            case 0x10:
                regAddr = RTL9602C_RMA_CTRL10r;
                break;
            case 0x11:
                regAddr = RTL9602C_RMA_CTRL11r;
                break;
            case 0x12:
                regAddr = RTL9602C_RMA_CTRL12r;
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
                regAddr = RTL9602C_RMA_CTRL13r;
                break;
            case 0x18:
                regAddr = RTL9602C_RMA_CTRL18r;
                break;
            case 0x1A:
                regAddr = RTL9602C_RMA_CTRL1Ar;
                break;
            case 0x20:
                regAddr = RTL9602C_RMA_CTRL20r;
                break;
            case 0x21:
                regAddr = RTL9602C_RMA_CTRL21r;
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
                regAddr = RTL9602C_RMA_CTRL22r;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_read(regAddr, RTL9602C_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of _dal_rtl9602c_port_rmaPortIsolationLeakyEnable_get */






static int32
_dal_rtl9602c_port_rmaPortIsolationLeakyEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
        switch(pRmaFrame->octet[5])
        {
            case 0xCC:
                regAddr = RTL9602C_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = RTL9602C_RMA_CTRL_SSTPr;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

        switch(pRmaFrame->octet[5])
        {
            case 0x00:
                regAddr = RTL9602C_RMA_CTRL00r;
                break;
            case 0x01:
                regAddr = RTL9602C_RMA_CTRL01r;
                break;
            case 0x02:
                regAddr = RTL9602C_RMA_CTRL02r;
                break;
            case 0x03:
                regAddr = RTL9602C_RMA_CTRL03r;
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
                regAddr = RTL9602C_RMA_CTRL04r;
                break;
            case 0x08:
                regAddr = RTL9602C_RMA_CTRL08r;
                break;
            case 0x0D:
                regAddr = RTL9602C_RMA_CTRL0Dr;
                break;
            case 0x0E:
                regAddr = RTL9602C_RMA_CTRL0Er;
                break;
            case 0x10:
                regAddr = RTL9602C_RMA_CTRL10r;
                break;
            case 0x11:
                regAddr = RTL9602C_RMA_CTRL11r;
                break;
            case 0x12:
                regAddr = RTL9602C_RMA_CTRL12r;
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
                regAddr = RTL9602C_RMA_CTRL13r;
                break;
            case 0x18:
                regAddr = RTL9602C_RMA_CTRL18r;
                break;
            case 0x1A:
                regAddr = RTL9602C_RMA_CTRL1Ar;
                break;
            case 0x20:
                regAddr = RTL9602C_RMA_CTRL20r;
                break;
            case 0x21:
                regAddr = RTL9602C_RMA_CTRL21r;
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
                regAddr = RTL9602C_RMA_CTRL22r;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_write(regAddr, RTL9602C_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_rtl9602c_port_rmaPortIsolationLeakyEnable_set */


/* Function Name:
 *      dal_rtl9602c_port_isolationLeaky_get
 * Description:
 *      Get the per port isolation leaky state for given type
 * Input:
 *      type      - Packet type for isolation leaky.
 * Output:
 *      pEnable   - status of port isolation leaky for given leaky type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_rtl9602c_port_isolationLeaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    rtk_mac_t rmaFrame;
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "type=%d",type);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((LEAKY_END <=type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    rmaFrame.octet[0] = 0x01;
    rmaFrame.octet[1] = 0x80;
    rmaFrame.octet[2] = 0xC2;
    rmaFrame.octet[3] = 0x00;
    rmaFrame.octet[4] = 0x00;

    /* function body */
    switch(type)
    {
        case LEAKY_BRG_GROUP:
        case LEAKY_FD_PAUSE:
        case LEAKY_SP_MCAST:
        case LEAKY_1X_PAE:
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
        case LEAKY_8021AB:
        case LEAKY_UNDEF_BRG_0F:
        case LEAKY_BRG_MNGEMENT:
        case LEAKY_UNDEFINED_11:
        case LEAKY_UNDEFINED_12:
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
        case LEAKY_UNDEFINED_18:
        case LEAKY_UNDEFINED_19:
        case LEAKY_UNDEFINED_1A:
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
        case LEAKY_GMRP:
        case LEAKY_GVRP:
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            rmaFrame.octet[5] = type;
            if (( ret = _dal_rtl9602c_port_rmaPortIsolationLeakyEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }

            break;


        case LEAKY_SSTP:
        case LEAKY_CDP:
            rmaFrame.octet[0] = 0x01;
            rmaFrame.octet[1] = 0x00;
            rmaFrame.octet[2] = 0x0C;
            rmaFrame.octet[3] = 0xCC;
            rmaFrame.octet[4] = 0xCC;

            if(LEAKY_CDP == type)
                rmaFrame.octet[5] = 0xCC;
            else
                rmaFrame.octet[5] = 0xCD;

            if (( ret = _dal_rtl9602c_port_rmaPortIsolationLeakyEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;

        case LEAKY_IGMP:
            if ((ret = reg_field_read(RTL9602C_IGMP_GLB_CTRLr, RTL9602C_PISO_LEAKYf, (uint32 *)&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            *pEnable = data;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;

    }


    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_isolationLeaky_get */

/* Function Name:
 *      dal_rtl9602c_port_isolationLeaky_set
 * Description:
 *      Set the per port isolation leaky state for given type
 * Input:
 *      type      - Packet type for isolation leaky.
 *      enable    - status of port isolation leaky for given leaky type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_rtl9602c_port_isolationLeaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    rtk_mac_t rmaFrame;
    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "type=%d",type);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((LEAKY_END <=type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    rmaFrame.octet[0] = 0x01;
    rmaFrame.octet[1] = 0x80;
    rmaFrame.octet[2] = 0xC2;
    rmaFrame.octet[3] = 0x00;
    rmaFrame.octet[4] = 0x00;

    /* function body */
    switch(type)
    {
        case LEAKY_BRG_GROUP:
        case LEAKY_FD_PAUSE:
        case LEAKY_SP_MCAST:
        case LEAKY_1X_PAE:
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
        case LEAKY_8021AB:
        case LEAKY_UNDEF_BRG_0F:
        case LEAKY_BRG_MNGEMENT:
        case LEAKY_UNDEFINED_11:
        case LEAKY_UNDEFINED_12:
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
        case LEAKY_UNDEFINED_18:
        case LEAKY_UNDEFINED_19:
        case LEAKY_UNDEFINED_1A:
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
        case LEAKY_GMRP:
        case LEAKY_GVRP:
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            rmaFrame.octet[5] = type;
            if (( ret = _dal_rtl9602c_port_rmaPortIsolationLeakyEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;

        case LEAKY_SSTP:
        case LEAKY_CDP:
            rmaFrame.octet[0] = 0x01;
            rmaFrame.octet[1] = 0x00;
            rmaFrame.octet[2] = 0x0C;
            rmaFrame.octet[3] = 0xCC;
            rmaFrame.octet[4] = 0xCC;

            if(LEAKY_CDP == type)
                rmaFrame.octet[5] = 0xCC;
            else
                rmaFrame.octet[5] = 0xCD;

            if (( ret = _dal_rtl9602c_port_rmaPortIsolationLeakyEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
            data = enable;
            if ((ret = reg_field_write(RTL9602C_IGMP_GLB_CTRLr, RTL9602C_PISO_LEAKYf, (uint32 *)&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;

    }


    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_isolationLeaky_set */

/* Function Name:
 *      dal_rtl9602c_port_serdesNWay_set
 * Description:
 *      Set Serdes N-Way Mode
 * Input:
 *      num   - serdes number
 *      cfg   - serdes n-way mode
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
dal_rtl9602c_port_serdesNWay_set(uint8 num, uint8 cfg)
{
    int ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d,cfg=%d",num, cfg);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((0 != num), RT_ERR_INPUT);
    RT_PARAM_CHK((LAN_SDS_NWAY_END <= cfg), RT_ERR_INPUT);

    /* function body */
    if(LAN_SDS_NWAY_AUTO == cfg)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    if((ret = reg_field_write(RTL9602C_HSG_SDS_REG2r, RTL9602C_SP_SDS_FRC_ANf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_serdesNWay_set */

/* Function Name:
 *      dal_rtl9602c_port_serdesNWay_get
 * Description:
 *      Get Serdes N-Way Mode
 * Input:
 *      num   - serdes number
 * Output:
 *      cfg   - serdes n-way mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_port_serdesNWay_get(uint8 num, uint8 *cfg)
{
    int ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d",num);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((0 != num), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == cfg), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9602C_HSG_SDS_REG2r, RTL9602C_SP_SDS_FRC_ANf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    if(0 == value)
    {
        *cfg = LAN_SDS_NWAY_AUTO;
    }
    else
    {
        *cfg = LAN_SDS_NWAY_FORCE;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_port_serdesNWay_get */
