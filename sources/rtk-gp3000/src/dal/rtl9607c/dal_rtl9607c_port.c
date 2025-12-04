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
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <rtk/port.h>
#include <dal/rtl9607c/dal_rtl9607c_port.h>
#include <dal/rtl9607c/dal_rtl9607c_ponmac.h>
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#include <dal/rtl9607c/dal_rtl9607c_cpu.h>
#include <osal/lib.h>
#include <hal/common/miim.h>
#include <module/lan_sds/lan_sds.h>
#include <osal/time.h>
#include <ioal/mem32.h>
/*
 * Symbol Definition
 */

typedef struct dal_rtl9607c_phy_info_s {
    uint8   force_mode_speed[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_duplex[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_flowControl[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_pause[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_asy_pause[RTK_MAX_NUM_OF_PORTS];
} dal_rtl9607c_phy_info_t;

typedef struct dal_rtl9607c_green_patch_s
{
    uint16  addr;
    uint16  data;
} dal_rtl9607c_green_patch_t;

/*
 * Data Declaration
 */
static uint32    port_init = INIT_NOT_COMPLETED;
#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
static uint32 adminPhyConfig = DISABLED;
#endif

static dal_rtl9607c_phy_info_t   phy_info;
static uint32 greenEnable[RTK_MAX_NUM_OF_PORTS];
static dal_rtl9607c_green_patch_t  greenDisablePatchArray[] = {
    /* Green disable patch ver 20180425 */
    {0xa436,0x805d},
    {0xa438,0x1b33},
    {0xa436,0x8061},
    {0xa438,0x0822},
    {0xa436,0x804d},
    {0xa438,0x2444},
    {0xa436,0x8051},
    {0xa438,0x0823}
};
static dal_rtl9607c_green_patch_t  greenEnablePatchArray[] = {
    /* Green enable patch ver 20180425 */
    {0xa436,0x805d},
    {0xa438,0x0911},
    {0xa436,0x8061},
    {0xa438,0x0820},
    {0xa436,0x804d},
    {0xa438,0x2444},
    {0xa436,0x8051},
    {0xa438,0x0823}
};

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9607c_port_init
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
dal_rtl9607c_port_init(void)
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

    osal_memset(&phy_info, 0x00, sizeof(dal_rtl9607c_phy_info_t));

    HAL_GET_ALL_PORTMASK(allPortmask);
    HAL_GET_ALL_EXT_PORTMASK(allExtPortmask);

    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_PHY_EXIST(port)&& !HAL_IS_PON_PORT(port))
        {
            phy_ability.Half_10     = ENABLED;
            phy_ability.Full_10     = ENABLED;
            phy_ability.Half_100    = ENABLED;
            phy_ability.Full_100    = ENABLED;
            phy_ability.Half_1000   = ENABLED;
            phy_ability.Full_1000   = ENABLED;
            phy_ability.FC          = ENABLED;
            phy_ability.AsyFC       = ENABLED;
            if((ret = dal_rtl9607c_port_phyAutoNegoAbility_set(port, &phy_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_rtl9607c_port_phyAutoNegoEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_rtl9607c_port_greenEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_rtl9607c_port_phyCrossOverMode_set(port, PORT_CROSSOVER_MODE_AUTO)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl9607c_port_macRemoteLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9607c_port_macLocalLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9607c_port_specialCongest_set(port, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9607c_port_eeeEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }


        /* Force CPU port */
        if(HAL_IS_CPU_PORT(port))
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
            if((ret = dal_rtl9607c_port_macForceAbility_set(port, macAbility)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            data = 0xFFF;

            if((ret = reg_array_write(RTL9607C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        else if(HAL_IS_PON_PORT(port))
        {
            /* for gpon/epon mode, pon port is set by ponmac */
        }
        else
        {
            if((ret = dal_rtl9607c_port_adminEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
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

    RT_PORT6 = 6;
    RT_PORT7 = 7;

    RT_PORT_PON = 5;

    RT_PORT_CPU0 = 9;
    RT_PORT_CPU1 = 10;
    RT_PORT_CPU2 = 7;
    RT_PORT_MAX = 11;
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      dal_rtl9607c_port_link_get
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
dal_rtl9607c_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_P_LINK_STATUSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    *pStatus = ((data == 1) ? PORT_LINKUP : PORT_LINKDOWN);
    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_link_get */

/* Function Name:
 *      dal_rtl9607c_port_speedDuplex_get
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
dal_rtl9607c_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32  ret;
    uint32 speed01,speed23;
    uint32 duplex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_P_LINK_SPDf, &speed01)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_SPEED_ABLTY23f, &speed23)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }



    if ((ret = reg_array_field_read(RTL9607C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_P_DUPLEXf, &duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    *pSpeed = speed23 << 2 | speed01;

    *pDuplex = ((1 == duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);
    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_speedDuplex_get */

/* Function Name:
 *      dal_rtl9607c_port_flowctrl_get
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
dal_rtl9607c_port_flowctrl_get(
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

    if ((ret = reg_array_field_read(RTL9607C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_P_TX_FCf, pTxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_P_RX_FCf, pRxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_flowctrl_get */

/* Function Name:
 *      dal_rtl9607c_port_phyAutoNegoEnable_get
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
dal_rtl9607c_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "port:%d",port);
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_phyAutoNegoEnable_get */

/* Function Name:
 *      dal_rtl9607c_port_phyAutoNegoEnable_set
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
dal_rtl9607c_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
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
} /* end of dal_rtl9607c_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_rtl9607c_port_phyAutoNegoAbility_get
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
dal_rtl9607c_port_phyAutoNegoAbility_get(
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

    if ((ret = dal_rtl9607c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
} /* end of dal_rtl9607c_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_rtl9607c_port_phyAutoNegoAbility_set
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
dal_rtl9607c_port_phyAutoNegoAbility_set(
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

    if(!HAL_IS_PON_PORT(port))
    {
        if ((ret = dal_rtl9607c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
    }
    else
    {
        /* Since there is no actual PHY for PON port, just consider fiber */
        ret = dal_rtl9607c_ponmac_mode_get(&ponMode);
        /* Perform fiber flow control only for FIBER modes */
        if((RT_ERR_OK == ret) &&
           (PONMAC_MODE_GPON != ponMode) &&
           (PONMAC_MODE_EPON != ponMode))
        {
            /* for PON port, it is possible that the actual application is fiber
             * Configure fiber register too
             */
            /* Force select giga registers */
            value = 1;
            if ((ret = reg_field_write(RTL9607C_WSDS_DIG_02r, RTL9607C_FRC_REG4_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9607C_WSDS_DIG_02r, RTL9607C_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
            /* Configure pause field */
            if ((ret = reg_field_read(RTL9607C_FIB_REG4r, RTL9607C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
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
            if ((ret = reg_field_write(RTL9607C_FIB_REG4r, RTL9607C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
            /* Force select 100M registers */
            value = 1;
            if ((ret = reg_field_write(RTL9607C_WSDS_DIG_02r, RTL9607C_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
            /* Configure pause field */
            if ((ret = reg_field_read(RTL9607C_FIB_REG4r, RTL9607C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
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

            if ((ret = reg_field_write(RTL9607C_FIB_REG4r, RTL9607C_FP_CFG_FIB_TX_CFG_REGf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
            /* Release force register */
            value = 0;
            if ((ret = reg_field_write(RTL9607C_WSDS_DIG_02r, RTL9607C_FRC_REG4_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9607C_WSDS_DIG_02r, RTL9607C_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }

            /* Restart N-Way manually */
            value = 1;
            if ((ret = reg_field_write(RTL9607C_FIB_REG0r, RTL9607C_FP_CFG_FIB_RESTARTf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_rtl9607c_port_phyForceModeAbility_get
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
dal_rtl9607c_port_phyForceModeAbility_get(
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

    if ((ret = dal_rtl9607c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
} /* end of dal_rtl9607c_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_rtl9607c_port_phyForceModeAbility_set
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
dal_rtl9607c_port_phyForceModeAbility_set(
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

    if ((ret = dal_rtl9607c_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
} /* end of dal_rtl9607c_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_rtl9607c_port_phyReg_get
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
dal_rtl9607c_port_phyReg_get(
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
} /* end of dal_rtl9607c_port_phyReg_get */

/* Function Name:
 *      dal_rtl9607c_port_phyReg_set
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
dal_rtl9607c_port_phyReg_set(
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
} /* end of dal_rtl9607c_port_phyReg_set */

/* Function Name:
 *      dal_rtl9607c_port_phyMasterSlave_get
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
dal_rtl9607c_port_phyMasterSlave_get(
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMasterSlave), RT_ERR_NULL_POINTER);

    *pMasterSlave = PORT_AUTO_MODE;
    return RT_ERR_OK;
}/* end of dal_rtl9607c_port_phyMasterSlave_get */

/* Function Name:
 *      dal_rtl9607c_port_phyMasterSlave_set
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
dal_rtl9607c_port_phyMasterSlave_set(
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(masterSlave >= PORT_MASTER_SLAVE_END, RT_ERR_INPUT);


    return RT_ERR_OK;
}/* end of dal_rtl9607c_port_phyMasterSlave_set */

/* Function Name:
 *      dal_rtl9607c_port_phyTestMode_get
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
dal_rtl9607c_port_phyTestMode_get(rtk_port_t port, rtk_port_phy_test_mode_t *pTestMode)
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
    if((ret = dal_rtl9607c_port_phyReg_get(port, 0xA41, 9, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pTestMode = (data & 0xE000) >> 13;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_phyTestMode_get */

/* Function Name:
 *      dal_rtl9607c_port_phyTestMode_set
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
dal_rtl9607c_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t testMode)
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
    if((ret = dal_rtl9607c_port_phyReg_get(port, 0xA41, 9, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    data &= 0x1FFF;
    data |= (testMode << 13);

    if((ret = dal_rtl9607c_port_phyReg_set(port, 0xA41, 9, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_phyTestMode_set */

/* Function Name:
 *      dal_rtl9607c_port_cpuPortId_get
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
dal_rtl9607c_port_cpuPortId_get(rtk_port_t *pPort)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    *pPort = (rtk_port_t)HAL_GET_CPU_PORT();
    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_cpuPortId_get */

/* Function Name:
 *      dal_rtl9607c_port_macRemoteLoopbackEnable_get
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
dal_rtl9607c_port_macRemoteLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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
} /* end of dal_rtl9607c_port_macRemoteLoopbackEnable_get */

/* Function Name:
 *      dal_rtl9607c_port_macRemoteLoopbackEnable_set
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
dal_rtl9607c_port_macRemoteLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
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
} /* end of dal_rtl9607c_port_macRemoteLoopbackEnable_set */

/* Function Name:
 *      dal_rtl9607c_port_macLocalLoopbackEnable_get
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
dal_rtl9607c_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9607C_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)data ;
    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_macLocalLoopbackEnable_get */

/* Function Name:
 *      dal_rtl9607c_port_macLocalLoopbackEnable_set
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
dal_rtl9607c_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
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
    if((ret = reg_array_field_write(RTL9607C_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_macLocalLoopbackEnable_set */

/* Function Name:
 *      dal_rtl9607c_port_adminEnable_get
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
dal_rtl9607c_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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

    if((ret = reg_array_field_read(RTL9607C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_LINK_ABLTYf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(ENABLED == data)
    {
        if((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_LINK_ABLTYf, &link)) != RT_ERR_OK)
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
} /* end of dal_rtl9607c_port_adminEnable_get */

/* Function Name:
 *      dal_rtl9607c_port_adminEnable_set
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
dal_rtl9607c_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
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
        if((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        data = ENABLED;
        if ((ret = reg_array_field_write(RTL9607C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        data = DISABLED;
        if ((ret = reg_array_field_write(RTL9607C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_adminEnable_set */

/* Function Name:
 *      dal_rtl9607c_port_specialCongest_get
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
dal_rtl9607c_port_specialCongest_get(rtk_port_t port, uint32 *pSecond)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSecond), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9607C_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_CGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_specialCongest_get */

/* Function Name:
 *      dal_rtl9607c_port_specialCongest_set
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
dal_rtl9607c_port_specialCongest_set(rtk_port_t port, uint32 second)
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
    if((ret = reg_array_field_write(RTL9607C_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_CGST_SUST_TMR_LMTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if((ret = reg_array_field_write(RTL9607C_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_CGST_SUST_TMR_LMT_Hf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_specialCongest_set */

/* Function Name:
 *      dal_rtl9607c_port_specialCongestStatus_get
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
dal_rtl9607c_port_specialCongestStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(RTL9607C_SC_P_CTRL_1r, RTL9607C_CGST_INDf, &regData)) != RT_ERR_OK)
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
 *      dal_rtl9607c_port_specialCongestStatus_get
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
dal_rtl9607c_port_specialCongestStatus_clear(rtk_port_t port)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    regData = (0x01 << port);
    if((ret = reg_field_write(RTL9607C_SC_P_CTRL_1r, RTL9607C_CGST_INDf, &regData)) != RT_ERR_OK)
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
dal_rtl9607c_port_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PHY_EXIST(port), RT_ERR_NOT_ALLOWED);
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_NOT_ALLOWED);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* Reply software enable state */
    *pEnable = greenEnable[port];

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_greenEnable_get */

/* Function Name:
 *      dal_rtl9607c_port_greenEnable_set
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
dal_rtl9607c_port_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    uint32 i;
    int32 ret;
    dal_rtl9607c_green_patch_t *patchArray;
    uint32 patchSize;
    rtk_enable_t nwayEnable;
    uint32 chip, rev, subtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PHY_EXIST(port), RT_ERR_NOT_ALLOWED);
    RT_PARAM_CHK(HAL_IS_PON_PORT(port), RT_ERR_NOT_ALLOWED);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if(ENABLED == enable)
    {
        switch(rev)
        {
        case CHIP_REV_ID_0:
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
        patchArray = greenEnablePatchArray;
        patchSize = sizeof(greenEnablePatchArray)/sizeof(dal_rtl9607c_green_patch_t);
            break;
        case CHIP_REV_ID_C:
        default:
            patchArray = NULL;
            patchSize = 0;
            break;            
        }
    }
    else
    {
        switch(rev)
        {
        case CHIP_REV_ID_0:
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
        patchArray = greenDisablePatchArray;
        patchSize = sizeof(greenDisablePatchArray)/sizeof(dal_rtl9607c_green_patch_t);
            break;
        case CHIP_REV_ID_C:
        default:
            patchArray = NULL;
            patchSize = 0;
            break;            
        }
    }

    for(i = 0;i < patchSize;i++)
    {
        if((ret=rtl9607c_ocpInterPhy_write(port,
                              patchArray[i].addr,
                              patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            /* Don't care return since some port will be consider as PHY port */
            return ret;
        }
    }

    /* Use auto negotiation enable set to do re-nway */
    if ((ret = phy_autoNegoEnable_get(port, &nwayEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = phy_autoNegoEnable_set(port, nwayEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Update software enable state */
    greenEnable[port] = enable;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_greenEnable_set */


/* Function Name:
 *      dal_rtl9607c_port_phyCrossOverMode_get
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
dal_rtl9607c_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
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
} /* end of dal_rtl9607c_port_phyCrossOverMode_get */

/* Function Name:
 *      dal_rtl9607c_port_phyCrossOverMode_set
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
dal_rtl9607c_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
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
} /* end of dal_rtl9607c_port_phyCrossOverMode_set */

/* Function Name:
 *      dal_rtl9607c_port_phyPowerDown_get
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
dal_rtl9607c_port_phyPowerDown_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    unsigned int data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = dal_rtl9607c_port_phyReg_get(port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (data & PowerDown_MASK) ? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_phyPowerDown_get */

/* Function Name:
 *      dal_rtl9607c_port_phyPowerDown_set
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
dal_rtl9607c_port_phyPowerDown_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    unsigned int data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = dal_rtl9607c_port_phyReg_get(port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
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

    if((ret = dal_rtl9607c_port_phyReg_set(port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    adminPhyConfig = enable;
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_phyPowerDown_set */

/* Function Name:
 *      dal_rtl9607c_port_phyPowerDown_tryset
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
dal_rtl9607c_port_phyPowerDown_tryset(rtk_port_t port, rtk_enable_t enable)
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

    return dal_rtl9607c_port_phyPowerDown_set(port, enable);
} /* end of dal_rtl9607c_port_phyPowerDown_tryset */

/* Function Name:
 *      dal_rtl9607c_port_rtctResult_get
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Test result will be stored in ge_result.
 */
int32
dal_rtl9607c_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int ret;
    uint32 channel;
    uint32 value, data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_ETHER_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRtctResult), RT_ERR_NULL_POINTER);


    if((ret = dal_rtl9607c_port_phyReg_get(port, 0xa42, 17, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
    if(!(value & (1 << 15)))
    {
        /* Previous RTCT not complete */
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return RT_ERR_PHY_RTCT_NOT_FINISH;
    }

    /* Clear result */
    memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));

    pRtctResult->linkType = PORT_SPEED_1000M;

    /* Get channel results  */
    for(channel = 0;channel < 4 ; channel ++)
    {
        switch(channel)
        {
        case 0:
            value = 0x802b;
            break;
        case 1:
            value = 0x802f;
            break;
        case 2:
            value = 0x8033;
            break;
        case 3:
            value = 0x8037;
            break;
        default:
            break;
        }

        if((ret = dal_rtl9607c_port_phyReg_set(port, 0xa42, 27, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        if((ret = dal_rtl9607c_port_phyReg_get(port, 0xa42, 28, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }


        switch(data)
        {
        case 0x0048:
            pRtctResult->ge_result.channelOpen[channel] = 1;
            break;
        case 0x0050:
            pRtctResult->ge_result.channelShort[channel] = 1;
            break;
        case 0x0042:
        case 0x0044:
            pRtctResult->ge_result.channelMismatch[channel] = 1;
            break;
        default:
            break;
        }
    }

    /* Get channel cable length  */
    for(channel = 0;channel < 4 ; channel ++)
    {
        switch(channel)
        {
        case 0:
            value = 0x802d;
            break;
        case 1:
            value = 0x8031;
            break;
        case 2:
            value = 0x8035;
            break;
        case 3:
            value = 0x8039;
            break;
        default:
            break;
        }

        if((ret = dal_rtl9607c_port_phyReg_set(port, 0xa42, 27, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        if((ret = dal_rtl9607c_port_phyReg_get(port, 0xa42, 28, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        pRtctResult->ge_result.channelLen[channel] = data / 80;
    }

    return RT_ERR_OK;
} /*end of dal_rtl9607c_port_rtctResult_get*/

/* Function Name:
 *      dal_rtl9607c_port_rtct_start
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
 *      RT_ERR_PORT_LINKUP  - The specified test port is linked up
 * Note:
 *      None
 */
int32
dal_rtl9607c_port_rtct_start(rtk_portmask_t *pPortmask)
{
    int ret;
    uint32 port, value;
    rtk_port_linkStatus_t link;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    
    HAL_SCAN_ALL_ETH_PORT(port)
    {
        if(RTK_PORTMASK_IS_PORT_SET(*pPortmask, port))
        {
            if((ret = dal_rtl9607c_port_link_get(port, &link)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            if(PORT_LINKUP == link)
            {
                /* Cannot test under link up state */
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return RT_ERR_PORT_LINKUP;
            }

            if((ret = dal_rtl9607c_port_phyReg_get(port, 0xa42, 17, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            value &= ~(1 << 15);
            value |= (0xf << 4) | (0x1 << 0);
                
            if((ret = dal_rtl9607c_port_phyReg_set(port, 0xa42, 17, value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_rtct_start */


/* Function Name:
 *      dal_rtl9607c_port_macForceAbility_set
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
dal_rtl9607c_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
{
    int32 ret;
    uint32 speed01,speed23;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK((PORT_SPEED_END <= macAbility.speed), RT_ERR_INPUT);
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

    speed01 = macAbility.speed & 0x3;
    speed23 = macAbility.speed >> 2;

    if(port == HAL_GET_SWPBOLB_PORT())
    {

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_LINK_ABLTY_X11f, &macAbility.linkStatus)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
#if 0

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_SPEED01_ABLTY_X11f, &speed01)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_SPEED23_ABLTY_X11f, &speed23)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_DUPLEX_ABLTY_X11f, &macAbility.duplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_FIB1G_ABLTY_X11f, &macAbility.linkFib1g)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_RXPAUSE_ABLTY_X11f, &macAbility.rxFc)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_TXPAUSE_ABLTY_X11f, &macAbility.txFc)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_NWAY_ABLTY_X11f, &macAbility.nwayAbility)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_MST_MOD_ABLTY_X11f, &macAbility.masterMod)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_MST_FAULT_ABLTY_X11f, &macAbility.nwayFault)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_LPI_100_ABLTY_X11f, &macAbility.lpi_100m)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_write(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_LPI_1000_ABLTY_X11f, &macAbility.lpi_giga)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
#endif

        return RT_ERR_OK;
    }


    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);


    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_SPEED01_ABLTYf, &speed01)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_SPEED23_ABLTYf, &speed23)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_DUPLEX_ABLTYf, &macAbility.duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FIB1G_ABLTYf, &macAbility.linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_LINK_ABLTYf, &macAbility.linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_RXPAUSE_ABLTYf, &macAbility.rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_TXPAUSE_ABLTYf, &macAbility.txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_NWAY_ABLTYf, &macAbility.nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_MST_MOD_ABLTYf, &macAbility.masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_MST_FAULT_ABLTYf, &macAbility.nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_LPI_100_ABLTYf, &macAbility.lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_LPI_1000_ABLTYf, &macAbility.lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9607c_port_macForceAbility_get
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
dal_rtl9607c_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
{
    int32 ret;
    uint32 speed01,speed23;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);
    RT_PARAM_CHK((NULL == pMacAbility), RT_ERR_NULL_POINTER);

    if(port == HAL_GET_SWPBOLB_PORT())
    {
        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_SPEED01_ABLTY_X11f, &speed01)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_SPEED23_ABLTY_X11f, &speed23)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        pMacAbility->speed = (speed23 << 2) | speed01;

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_DUPLEX_ABLTY_X11f, &pMacAbility->duplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_FIB1G_ABLTY_X11f, &pMacAbility->linkFib1g)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_LINK_ABLTY_X11f, &pMacAbility->linkStatus)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_RXPAUSE_ABLTY_X11f, &pMacAbility->rxFc)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_TXPAUSE_ABLTY_X11f, &pMacAbility->txFc)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_NWAY_ABLTY_X11f, &pMacAbility->nwayAbility)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_MST_MOD_ABLTY_X11f, &pMacAbility->masterMod)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_MST_FAULT_ABLTY_X11f, &pMacAbility->nwayFault)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_LPI_100_ABLTY_X11f, &pMacAbility->lpi_100m)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        if ((ret = reg_field_read(RTL9607C_FORCE_P_ABLTY_X11r, RTL9607C_LPI_1000_ABLTY_X11f, &pMacAbility->lpi_giga)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }

        return RT_ERR_OK;
    }

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_SPEED01_ABLTYf, &speed01)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_SPEED23_ABLTYf, &speed23)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    pMacAbility->speed = (speed23 << 2) | speed01;

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_DUPLEX_ABLTYf, &pMacAbility->duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FIB1G_ABLTYf, &pMacAbility->linkFib1g)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_LINK_ABLTYf, &pMacAbility->linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_RXPAUSE_ABLTYf, &pMacAbility->rxFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_TXPAUSE_ABLTYf, &pMacAbility->txFc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_NWAY_ABLTYf, &pMacAbility->nwayAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_MST_MOD_ABLTYf, &pMacAbility->masterMod)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_MST_FAULT_ABLTYf, &pMacAbility->nwayFault)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_LPI_100_ABLTYf, &pMacAbility->lpi_100m)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_LPI_1000_ABLTYf, &pMacAbility->lpi_giga)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607c_port_macForceAbilityState_set
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
dal_rtl9607c_port_macForceAbilityState_set(rtk_port_t port,rtk_enable_t state)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    if(state == ENABLED)
        data = 0xBFFF;
    else
        data = 0;

    if((ret = reg_array_write(RTL9607C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607c_port_macForceAbilityState_get
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
dal_rtl9607c_port_macForceAbilityState_get(rtk_port_t port,rtk_enable_t *pState)
{
    int32 ret;
    uint32 data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if((ret = reg_array_read(RTL9607C_ABLTY_FORCE_MODEr, port, REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
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

#define RTL9607C_GIGALITE_ENABLE_OFFSET    (8)
#define RTL9607C_GIGALITE_ENABLE_MASK      (0x1U<<RTL9607C_GIGALITE_ENABLE_OFFSET)

#define RTL9607C_GIGALITE_SPEED_ABILITY0_OFFSET    (9)
#define RTL9607C_GIGALITE_SPEED_ABILITY0_MASK      (0x1U<<RTL9607C_GIGALITE_SPEED_ABILITY0_OFFSET)

#define RTL9607C_GIGALITE_SPEED_ABILITY1_OFFSET    (10)
#define RTL9607C_GIGALITE_SPEED_ABILITY1_MASK      (0x1U<<RTL9607C_GIGALITE_SPEED_ABILITY1_OFFSET)

#define RTL9607C_GIGALITE_SPEED_ABILITY2_OFFSET    (11)
#define RTL9607C_GIGALITE_SPEED_ABILITY2_MASK      (0x1U<<RTL9607C_GIGALITE_SPEED_ABILITY2_OFFSET)

#define RTL9607C_GIGALITE_SPEED_ABILITY_ALL_MASK      (RTL9607C_GIGALITE_SPEED_ABILITY0_MASK||RTL9607C_GIGALITE_SPEED_ABILITY1_MASK|RTL9607C_GIGALITE_SPEED_ABILITY1_MASK)
/* Function Name:
 *      dal_rtl9607c_port_gigaLiteEnable_set
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
dal_rtl9607c_port_gigaLiteEnable_set(rtk_port_t port, rtk_enable_t enable)
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

    phyData0 = phyData0 & ~(RTL9607C_GIGALITE_ENABLE_MASK);
    phyData0 = phyData0 | (enable << RTL9607C_GIGALITE_ENABLE_OFFSET);

    if ((ret = hal_miim_write(port, 0xa4a, 17, phyData0)) != RT_ERR_OK)
        return ret;



    if ((ret = hal_miim_read(port, 0xa42, 20, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(RTL9607C_GIGALITE_SPEED_ABILITY_ALL_MASK);
    phyData0 = phyData0 | (enable << RTL9607C_GIGALITE_SPEED_ABILITY0_OFFSET) | (enable << RTL9607C_GIGALITE_SPEED_ABILITY1_OFFSET) | (enable << RTL9607C_GIGALITE_SPEED_ABILITY2_OFFSET);

    if ((ret = hal_miim_write(port, 0xa42, 20, phyData0)) != RT_ERR_OK)
        return ret;



    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_gigaLiteEnable_set */

/* Function Name:
 *      dal_rtl9607c_port_gigaLiteEnable_get
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
dal_rtl9607c_port_gigaLiteEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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

    if(phyData0 & (RTL9607C_GIGALITE_ENABLE_MASK))
        *pEnable=ENABLED;
    else
        *pEnable=DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_gigaLiteEnable_get */

/* Function Name:
 *      rtl9607c_raw_port_forceDmp_set
 * Description:
 *      Set forwarding force mode
 * Input:
 *      port   - port id
 *      state     - enable/disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 */
int32 rtl9607c_raw_port_forceDmp_set(rtk_port_t port,rtk_enable_t state)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_array_field_write(RTL9607C_EN_FORCE_P_DMPr, port, REG_ARRAY_INDEX_NONE,RTL9607C_FORCE_MODEf,&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

} /* end of rtl9607c_raw_port_forceDmp_set */

/* Function Name:
 *      rtl9607c_raw_port_forceDmp_get
 * Description:
 *      Get forwarding force mode
 * Input:
 *      port   - port id
 * Output:
 *      pState  - enable/disable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 rtl9607c_raw_port_forceDmp_get(rtk_port_t port,rtk_enable_t *pState)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_EN_FORCE_P_DMPr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_MODEf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

} /* end of rtl9607c_raw_port_forceDmp_get */

/* Function Name:
 *      rtl9607c_raw_port_forceDmpMask_set
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
int32 rtl9607c_raw_port_forceDmpMask_set(rtk_port_t port, rtk_portmask_t mask)
{
    int32 ret;
    uint32  val;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(mask), RT_ERR_PORT_MASK);

    val = (uint32)mask.bits[0];
    if ((ret = reg_array_field_write(RTL9607C_FORCE_P_DMPr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_PROT_MASKf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

} /* end of rtl9607c_raw_port_forceDmpMask_set */

/* Function Name:
 *      rtl9607c_raw_port_forceDmpMask_get
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
int32 rtl9607c_raw_port_forceDmpMask_get(rtk_port_t port, rtk_portmask_t  *pMask)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_FORCE_P_DMPr, port, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_PROT_MASKf, pMask->bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

} /* end of rtl9607c_raw_port_forceDmpMask_get */

/* Function Name:
 *      dal_rtl9607c_port_isolationEntry_get
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
dal_rtl9607c_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    if(mode) { }

    regAddr = RTL9607C_PISO_PORTr;
    if((ret = reg_array_field_read(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x07FF;
    pExtPortmask->bits[0] = (regData & 0x1ffff800) >> 11;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_isolationEntry_get */

/* Function Name:
 *      dal_rtl9607c_port_isolationEntry_set
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
dal_rtl9607c_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    if(mode) { }

    regAddr = RTL9607C_PISO_PORTr;

    regData = ((pExtPortmask->bits[0] << 11) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9607C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_isolationEntry_set */

/* Function Name:
 *      dal_rtl9607c_port_isolationEntryExt_get
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
dal_rtl9607c_port_isolationEntryExt_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    regAddr = RTL9607C_PISO_EXTr;

    if((ret = reg_array_field_read(RTL9607C_PISO_EXTr, REG_ARRAY_INDEX_NONE, (int32)port, RTL9607C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x07FF;
    pExtPortmask->bits[0] = (regData & 0x1ffff800) >> 11;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_isolationEntryExt_get */

/* Function Name:
 *      dal_rtl9607c_port_isolationEntryExt_set
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
dal_rtl9607c_port_isolationEntryExt_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    regAddr = RTL9607C_PISO_EXTr;

    regData = ((pExtPortmask->bits[0] << 11) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, REG_ARRAY_INDEX_NONE, (int32)port, RTL9607C_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_isolationEntryExt_set */


/* Function Name:
 *      dal_rtl9607c_port_isolationIpmcLeaky_get
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
dal_rtl9607c_port_isolationIpmcLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9607C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_isolationIpmcLeaky_get */

/* Function Name:
 *      dal_rtl9607c_port_isolationIpmcLeaky_set
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
dal_rtl9607c_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(RTL9607C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_isolationIpmcLeaky_set */



/* Function Name:
 *      dal_rtl9607c_port_isolationPortLeaky_get
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
dal_rtl9607c_port_isolationPortLeaky_get(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(RTL9607C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;
    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_isolationPortLeaky_get */

/* Function Name:
 *      dal_rtl9607c_port_isolationPortLeaky_set
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
dal_rtl9607c_port_isolationPortLeaky_set(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(RTL9607C_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_isolationPortLeaky_set */


static int32
_dal_rtl9607c_port_rmaPortIsolationLeakyEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
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
                regAddr = RTL9607C_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = RTL9607C_RMA_CTRL_SSTPr;
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
                regAddr = RTL9607C_RMA_CTRL00r;
                break;
            case 0x01:
                regAddr = RTL9607C_RMA_CTRL01r;
                break;
            case 0x02:
                regAddr = RTL9607C_RMA_CTRL02r;
                break;
            case 0x03:
                regAddr = RTL9607C_RMA_CTRL03r;
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
                regAddr = RTL9607C_RMA_CTRL04r;
                break;
            case 0x08:
                regAddr = RTL9607C_RMA_CTRL08r;
                break;
            case 0x0D:
                regAddr = RTL9607C_RMA_CTRL0Dr;
                break;
            case 0x0E:
                regAddr = RTL9607C_RMA_CTRL0Er;
                break;
            case 0x10:
                regAddr = RTL9607C_RMA_CTRL10r;
                break;
            case 0x11:
                regAddr = RTL9607C_RMA_CTRL11r;
                break;
            case 0x12:
                regAddr = RTL9607C_RMA_CTRL12r;
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
                regAddr = RTL9607C_RMA_CTRL13r;
                break;
            case 0x18:
                regAddr = RTL9607C_RMA_CTRL18r;
                break;
            case 0x1A:
                regAddr = RTL9607C_RMA_CTRL1Ar;
                break;
            case 0x20:
                regAddr = RTL9607C_RMA_CTRL20r;
                break;
            case 0x21:
                regAddr = RTL9607C_RMA_CTRL21r;
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
                regAddr = RTL9607C_RMA_CTRL22r;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    if ((ret = reg_field_read(regAddr, RTL9607C_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of _dal_rtl9607c_port_rmaPortIsolationLeakyEnable_get */






static int32
_dal_rtl9607c_port_rmaPortIsolationLeakyEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
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
                regAddr = RTL9607C_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = RTL9607C_RMA_CTRL_SSTPr;
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
                regAddr = RTL9607C_RMA_CTRL00r;
                break;
            case 0x01:
                regAddr = RTL9607C_RMA_CTRL01r;
                break;
            case 0x02:
                regAddr = RTL9607C_RMA_CTRL02r;
                break;
            case 0x03:
                regAddr = RTL9607C_RMA_CTRL03r;
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
                regAddr = RTL9607C_RMA_CTRL04r;
                break;
            case 0x08:
                regAddr = RTL9607C_RMA_CTRL08r;
                break;
            case 0x0D:
                regAddr = RTL9607C_RMA_CTRL0Dr;
                break;
            case 0x0E:
                regAddr = RTL9607C_RMA_CTRL0Er;
                break;
            case 0x10:
                regAddr = RTL9607C_RMA_CTRL10r;
                break;
            case 0x11:
                regAddr = RTL9607C_RMA_CTRL11r;
                break;
            case 0x12:
                regAddr = RTL9607C_RMA_CTRL12r;
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
                regAddr = RTL9607C_RMA_CTRL13r;
                break;
            case 0x18:
                regAddr = RTL9607C_RMA_CTRL18r;
                break;
            case 0x1A:
                regAddr = RTL9607C_RMA_CTRL1Ar;
                break;
            case 0x20:
                regAddr = RTL9607C_RMA_CTRL20r;
                break;
            case 0x21:
                regAddr = RTL9607C_RMA_CTRL21r;
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
                regAddr = RTL9607C_RMA_CTRL22r;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    if ((ret = reg_field_write(regAddr, RTL9607C_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_rtl9607c_port_rmaPortIsolationLeakyEnable_set */


/* Function Name:
 *      dal_rtl9607c_port_isolationLeaky_get
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
dal_rtl9607c_port_isolationLeaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
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
            if (( ret = _dal_rtl9607c_port_rmaPortIsolationLeakyEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
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

            if (( ret = _dal_rtl9607c_port_rmaPortIsolationLeakyEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;

        case LEAKY_IGMP:
            if ((ret = reg_field_read(RTL9607C_IGMP_GLB_CTRLr, RTL9607C_PISO_LEAKYf, (uint32 *)&data)) != RT_ERR_OK)
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
}   /* end of dal_rtl9607c_port_isolationLeaky_get */

/* Function Name:
 *      dal_rtl9607c_port_isolationLeaky_set
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
dal_rtl9607c_port_isolationLeaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
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
            if (( ret = _dal_rtl9607c_port_rmaPortIsolationLeakyEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
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

            if (( ret = _dal_rtl9607c_port_rmaPortIsolationLeakyEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
            data = enable;
            if ((ret = reg_field_write(RTL9607C_IGMP_GLB_CTRLr, RTL9607C_PISO_LEAKYf, (uint32 *)&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;

    }


    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_isolationLeaky_set */


static int _rtl9607c_lan_sds0_indirectAccess(uint32 cmd, uint32 data)
{
    int ret;
    uint32 value, cnt = 1000;

    if((ret = reg_write(RTL9607C_WSDS_HSG0_DIG_02r, &data)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = reg_write(RTL9607C_WSDS_HSG0_DIG_01r, &cmd)) != RT_ERR_OK)
    {
        return ret;
    }

    while(1)
    {
        if((ret = reg_field_read(RTL9607C_WSDS_HSG0_DIG_01r, RTL9607C_CFG_SDS_MDX_REQf, &value)) != RT_ERR_OK)
        {
            return ret;
        }

        if(value == 0)
        {
            return RT_ERR_OK;
        }
        cnt --;
        if(cnt <= 0)
        {
            return RT_ERR_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds0_modeV1_set(uint32 mode)
{
    int ret;
    uint32 value;
    rtk_enable_t oldState;
    rtk_port_macAbility_t oldAbility, newAbility;

    /* Parameter check */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
        break;
    case LAN_SDS_MODE_GE_FE_PHY:
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Mode switch need additional protection
     * The complete switching steps are listed below
     * 1. MAC link down (force link down)
     * 2. Stop clock
     * 3. Mode configuration
     *     a. Serdes specific configurations
     * 4. Restore clock
     *     b. SDS_CFG set
     *     c. Switch related configurations
     * 5. MAC link restore (restore to original link state)
     */

    /* Enable common IP */
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = ioal_socMem32_read(0xB8000600, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value |= (1 << 29);
    if ((ret = ioal_socMem32_write(0xB8000600, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
#endif

    /* MAC link down (force link down) */
    if((ret = dal_rtl9607c_port_macForceAbility_get(6, &oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_get(6, &oldState)) != RT_ERR_OK)
    {
        return ret;
    }
    newAbility = oldAbility;
    newAbility.linkStatus = PORT_LINKDOWN;
    if((ret = dal_rtl9607c_port_macForceAbility_set(6, newAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(6, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Stop clock */
    value = 1;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Serdes specific configurations */
    value = 0x1f;
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        /* Based on FIB1G_SG0_Init_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x1
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6146)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0x0
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd98 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x2317)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x1A */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEE1C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x2010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0x7
         * ANA0F[11:10]: REG_CDR_KP2=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0x7020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC019, 0x0004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0xC */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xD4AC)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01[15:12]: REG_TXDRV_DAC_POST0=0x0
         * PAGE1_REG01[7:4]: REG_TXDRV_DAC_POST1=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x0)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x17)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        /* Based on SG0_Init_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x1
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6146)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0x0
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd98 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x2317)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x1A */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEE1C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x2010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0x7
         * ANA0F[11:10]: REG_CDR_KP2=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0x7020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xc019, 0x0004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xD4A8)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01[15:12]: REG_TXDRV_DAC_POST0=0x0
         * PAGE1_REG01[7:4]: REG_TXDRV_DAC_POST1=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x0)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x17)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        /* Based on HSG0_Init_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x0
         * ANA02[6]: REG_CMU_LDO_MODESEL=0x0
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6006)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0xFFF (13'h4095)
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x1fff)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd60 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x21e7)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x11 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x0011)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEF1C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x2010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0xC */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0xC020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xc019, 0x0004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0xA */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xd4aa)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01 [15:12]: REG_TXDRV_DAC_POST0=0x4
         * PAGE1_REG01 [7:4]: REG_TXDRV_DAC_POST1=0x8
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x4080)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x57)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3  */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Based on 2500BASE_X_HSG0_Init_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x0
         * ANA02[6]: REG_CMU_LDO_MODESEL=0x0
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6006)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0xFFF (13'h4095)
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x1fff)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd60 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x21e7)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x11 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x0011)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEF1C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x2010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0xC */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0xC020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xc019, 0x0004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0xA */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xd4aa)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01 [15:12]: REG_TXDRV_DAC_POST0=0x4
         * PAGE1_REG01 [7:4]: REG_TXDRV_DAC_POST1=0x8
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x4080)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x57)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3  */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* Restore clock */
    value = 0;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* SDS_CFG set */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        value = 0x4;
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        value = 0x2;
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x12;
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        value = 0x16;
        break;
    }
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Set switch ready, phy patch done */
    value = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr, RTL9607C_PATCH_PHY_DONEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch related configurations */
    value = 0x0;
    if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Configure LINE_RATE_2500M */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4ffff;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_RATEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* MAC/PHY mode configuration */
    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_HSGMII_PHY:
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x0;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    default:
        /* For other mode, the MAC/PHY mode is not used */
        break;
    }

    /* MAC link restore (restore to original link state) */
    if((ret = dal_rtl9607c_port_macForceAbility_set(6, oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(6, oldState)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds0_modeV2_set(uint32 mode)
{
    int ret;
    uint32 value;
    rtk_enable_t oldState;
    rtk_port_macAbility_t oldAbility, newAbility;

    /* Parameter check */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
        break;
    case LAN_SDS_MODE_GE_FE_PHY:
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Mode switch need additional protection
     * The complete switching steps are listed below
     * 1. MAC link down (force link down)
     * 2. Stop clock
     * 3. Mode configuration
     *     a. Serdes specific configurations
     * 4. Restore clock
     *     b. SDS_CFG set
     *     c. Switch related configurations
     * 5. MAC link restore (restore to original link state)
     */

#if defined(CONFIG_SDK_KERNEL_LINUX)
    /* Enable common IP */
    if ((ret = ioal_socMem32_read(0xB8000600, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value |= (1 << 29);
    if ((ret = ioal_socMem32_write(0xB8000600, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    if ((ret = ioal_socMem32_read(0xB8000508, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value |= (1 << 27) | (1 << 28);
    if ((ret = ioal_socMem32_write(0xB8000508, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
#endif

    /* MAC link down (force link down) */
    if((ret = dal_rtl9607c_port_macForceAbility_get(6, &oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_get(6, &oldState)) != RT_ERR_OK)
    {
        return ret;
    }
    newAbility = oldAbility;
    newAbility.linkStatus = PORT_LINKDOWN;
    if((ret = dal_rtl9607c_port_macForceAbility_set(6, newAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(6, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Stop clock */
    value = 1;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Serdes specific configurations */
    value = 0x1f;
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        /* Based on FIB1G_SG0_Init_Patch_20180126 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x400C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x1
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6146)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0x0
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd97 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x230f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x1A */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x0
         * ANA0D[5:2]: REG_RX_Z0_RX=0xA
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEE28)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1
         * ANA0E[4]: REG_TX_MODE=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x1010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0x7
         * ANA0F[11:10]: REG_CDR_KP2=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0x7020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC019, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0xC */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xD4AC)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01[15:12]: REG_TXDRV_DAC_POST0=0x0
         * PAGE1_REG01[7:4]: REG_TXDRV_DAC_POST1=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x0)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x17)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        /* Based on SG0_Init_Patch_20180126 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x400C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x1
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6146)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0x0
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd97 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x230f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x1A */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x0
         * ANA0D[5:2]: REG_RX_Z0_RX=0xA
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEE28)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1
         * ANA0E[4]: REG_TX_MODE=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x1010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0x7 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0x7020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xc019, 0x0004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xD4A8)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01[15:12]: REG_TXDRV_DAC_POST0=0x0
         * PAGE1_REG01[7:4]: REG_TXDRV_DAC_POST1=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x0)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x17)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        /* Based on HSG0_Init_Patch_20180126 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x400C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x0
         * ANA02[6]: REG_CMU_LDO_MODESEL=0x0
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6006)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0xFFF (13'h4095)
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x1fff)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd59 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x21df)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x11 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x0011)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x2
         * ANA0D[5:2]: REG_RX_Z0_RX=0xA
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEF28)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1
         * ANA0E[4]: REG_TX_MODE=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x1010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0xC */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0xC020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xc019, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0x7 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xd4a7)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01 [15:12]: REG_TXDRV_DAC_POST0=0x4
         * PAGE1_REG01 [7:4]: REG_TXDRV_DAC_POST1=0x8
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x4080)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x57)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3  */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Based on 2500BASE_X_HSG0_Init_Patch_20180126 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[3]: REG_CDR_RESET_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC000, 0x400C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x18
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xE058)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[8]: REG_CMU_CP_NEW_EN=0x0
         * ANA02[6]: REG_CMU_LDO_MODESEL=0x0
         * ANA02[2]: REG_CMU_CCO_BKVCO=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC002, 0x6006)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[11:0]: REG_CMU_FCODE_IN=0xFFF (13'h4095)
         * ANA04[13]: REG_CMU_EN_SSC=1'b0
         * ANA04[15:14]: REG_CMU_LDO_VREF=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x1fff)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]: REG_CMU_NCODE_IN=8'd59 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x21df)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]: REG_CMU_CP_ISEL=0x11 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x0011)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA08[15:4]: REG_CMU_TBASE_IN = 12'd793 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC008, 0x3191)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[4]: REG_OOBS_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x520c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[14:12]: REG_CMU_SR=0x1
         * ANA0A[10]: REG_CMU_SC=0x1
         * ANA0A[5]: REG_RX_EQ_SELREG=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0x9621)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[15]: REG_RX_PSAVE_SEL=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00C, 0x4000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[8:7]: REG_RX_SPDSEL=0x2
         * ANA0D[5:2]: REG_RX_Z0_RX=0xA
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xEF28)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[15]: REG_TX_MODE=0x1
         * ANA0E[4]: REG_TX_MODE=0x1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00E, 0x1010)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:12]: REG_CDR_KP1=0xC */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00F, 0xC020)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA1B[15:0]: REG_RX_RESERVED=0xFF06 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC01B, 0xFF06)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA19[15:9]: REG_FILTER_OUT=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xc019, 0x4004)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]: REG_TXDRV_DAC=0xc */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0xd4ac)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG01 [15:12]: REG_TXDRV_DAC_POST0=0x0
         * PAGE1_REG01 [7:4]: REG_TXDRV_DAC_POST1=0x0
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x0000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG02[15:6]: REG_TX_DEEMP_EN[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x17)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG03[1:0]: REG_RX_PI_IBSEL=0x3 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0xAB65)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG06[1:0]: REG_RX_EQ_DCGAIN=0x3  */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC106, 0x040b)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG07[12:0]:STEP_IN=13'd575 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC107, 0x023f)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* Restore clock */
    value = 0;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* SDS_CFG set */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        value = 0x4;
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        value = 0x2;
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x12;
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        value = 0x16;
        break;
    }
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Set switch ready, phy patch done */
    value = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr, RTL9607C_PATCH_PHY_DONEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch related configurations */
    value = 0x0;
    if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Configure LINE_RATE_2500M */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4ffff;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_RATEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* MAC/PHY mode configuration */
    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_HSGMII_PHY:
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x0;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    default:
        /* For other mode, the MAC/PHY mode is not used */
        break;
    }

    /* MAC link restore (restore to original link state) */
    if((ret = dal_rtl9607c_port_macForceAbility_set(6, oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(6, oldState)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds0_modeV3_set(uint32 mode)
{
    int ret;
    uint32 value;
    rtk_enable_t oldState;
    rtk_port_macAbility_t oldAbility, newAbility;

    /* Parameter check */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
        break;
    case LAN_SDS_MODE_GE_FE_PHY:
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Mode switch need additional protection
     * The complete switching steps are listed below
     * 1. MAC link down (force link down)
     * 2. Stop clock
     * 3. Mode configuration
     *     a. Serdes specific configurations
     * 4. Restore clock
     *     b. SDS_CFG set
     *     c. Switch related configurations
     * 5. MAC link restore (restore to original link state)
     */

#if defined(CONFIG_SDK_KERNEL_LINUX)
    /* Enable common IP */
    if ((ret = ioal_socMem32_read(0xB8000600, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value |= (1 << 29);
    if ((ret = ioal_socMem32_write(0xB8000600, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    if ((ret = ioal_socMem32_read(0xB8000508, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value |= (1 << 27) | (1 << 28);
    if ((ret = ioal_socMem32_write(0xB8000508, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
#endif

    /* MAC link down (force link down) */
    if((ret = dal_rtl9607c_port_macForceAbility_get(6, &oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_get(6, &oldState)) != RT_ERR_OK)
    {
        return ret;
    }
    newAbility = oldAbility;
    newAbility.linkStatus = PORT_LINKDOWN;
    if((ret = dal_rtl9607c_port_macForceAbility_set(6, newAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(6, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Serdes specific configurations */
    /* UTP_FIBER_AUTODET[8]: SDS_CMD_STOP_GLI_CLK=1 */
    value = 0x1;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    osal_time_mdelay(100);

    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        /* Based on 1000Base-X_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* Fiber 1G mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[31:30]=00 PLL/4 APHY speed mode selection for Serde port 6
         * CPU_PLL_CTRL_DUMMY[29]= 1 port6 1G
         */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x28
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xAC68)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL<4:0>=0x16 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[13]:REG_CMU_EN_SSC=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x6313)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[6]:REG_OOBS_RXIDLE_MANUAL=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x521c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_RX_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0xB628)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]:REG_TXDRV_DAC_DAT=0x4 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0x50A4)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG01[7:4]:REG_TXDRV_DAC_POST1=0x6 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x886A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE02_REG2[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x53)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG03[15:12]:REG_TX_TERM=0x2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0x2B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG04[6:0]:REG_FILTER_OUT<6:0>=0x0C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC104, 0x450C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG09[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE_REG0B[7:6]:REG_RX_PI_TRISEL[2:1]=0x2
         * PAGE_REG0B[3]:reg_sel_rxidle_sel=0
         * PAGE_REG0B[2]:REG_RX_PI_TRISEL[0]=0
         * PAGE_REG0B[0]:CPHY_BG_EN=1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC10B, 0x80A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Fiber 1G mode */
        value = 0x4;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        /* Based on SGMII_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* SGMII mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x2;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[31:30]=00 PLL/4 APHY speed mode selection for Serde port 6
         * CPU_PLL_CTRL_DUMMY[29]= 1 port6 SGMII
         */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x28
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xAC68)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL<4:0>=0x16 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[13]:REG_CMU_EN_SSC=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x6313)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[6]:REG_OOBS_RXIDLE_MANUAL=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x521c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_RX_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0xB628)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]:REG_TXDRV_DAC_DAT=0x2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0x50A2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG01[7:4]:REG_TXDRV_DAC_POST1=0x6 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x886A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE02_REG2[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x53)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG03[15:12]:REG_TX_TERM=0x2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0x2B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG04[6:0]:REG_FILTER_OUT<6:0>=0x0C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC104, 0x450C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG09[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE_REG0B[7:6]:REG_RX_PI_TRISEL[2:1]=0x2
         * PAGE_REG0B[3]:reg_sel_rxidle_sel=0
         * PAGE_REG0B[2]:REG_RX_PI_TRISEL[0]=0
         * PAGE_REG0B[0]:CPHY_BG_EN=1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC10B, 0x80A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* SGMII mode */
        value = 0x2;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        /* Based on HSGMII_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* HiSGMII mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x12;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[31:30]=01 PLL/2 APHY speed mode selection for Serde port 6
         * CPU_PLL_CTRL_DUMMY[29]= 0 port6 HiSGMII
         */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0x0c
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xAC4C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL<4:0>=0x16 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[13]:REG_CMU_EN_SSC=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]:REG_CMU_NCODE_IN=0X7B */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x63DB)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[6]:REG_OOBS_RXIDLE_MANUAL=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x521c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_RX_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0xB628)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]:REG_TXDRV_DAC_DAT=0x5 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0x50A5)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG01[7:4]:REG_TXDRV_DAC_POST1=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x888A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE02_REG2[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x53)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG03[15:12]:REG_TX_TERM=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0x8B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG04[6:0]:REG_FILTER_OUT<6:0>=0x0C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC104, 0x450C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG09[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE_REG0B[7:6]:REG_RX_PI_TRISEL[2:1]=0x2
         * PAGE_REG0B[3]:reg_sel_rxidle_sel=0
         * PAGE_REG0B[2]:REG_RX_PI_TRISEL[0]=0
         * PAGE_REG0B[0]:CPHY_BG_EN=1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC10B, 0x80A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HiSGMII mode */
        value = 0x12;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Based on 2500Base-X_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG0_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* 2500-BaseX mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x16;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[31:30]=01 PLL/2 APHY speed mode selection for Serde port 6
         * CPU_PLL_CTRL_DUMMY[29]= 0 port6 2.5G
         */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_USB3f, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA01[8:6]: REG_CDR_KI=0x1
         * ANA01[5:0]: REG_CDR_KP=0xc
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC001, 0xAC4C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL<4:0>=0x16 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[13]:REG_CMU_EN_SSC=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]:REG_CMU_NCODE_IN=0X7B */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC005, 0x63DB)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09[6]:REG_OOBS_RXIDLE_MANUAL=0 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC009, 0x521c)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_RX_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00A, 0xB628)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE1_REG00[3:0]:REG_TXDRV_DAC_DAT=0x5 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC100, 0x50A5)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG01[7:4]:REG_TXDRV_DAC_POST1=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC101, 0x888A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE02_REG2[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC102, 0x53)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG03[15:12]:REG_TX_TERM=0x8 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC103, 0x8B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG04[6:0]:REG_FILTER_OUT<6:0>=0x0C */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC104, 0x450C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE01_REG09[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* PAGE_REG0B[7:6]:REG_RX_PI_TRISEL[2:1]=0x2
         * PAGE_REG0B[3]:reg_sel_rxidle_sel=0
         * PAGE_REG0B[2]:REG_RX_PI_TRISEL[0]=0
         * PAGE_REG0B[0]:CPHY_BG_EN=1
         */
        if((ret = _rtl9607c_lan_sds0_indirectAccess(0xC10B, 0x80A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* 2500-BaseX mode */
        value = 0x16;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG0_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG0_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }
    /* UTP_FIBER_AUTODET[8]: SDS_CMD_STOP_GLI_CLK=0 */
    value = 0x0;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    osal_time_mdelay(100);

    /* Set switch ready, phy patch done */
    value = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr, RTL9607C_PATCH_PHY_DONEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch related configurations */
    value = 0x0;
    if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Configure LINE_RATE_2500M */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4ffff;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_0r, RTL9607C_RATEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* MAC/PHY mode configuration */
    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_HSGMII_PHY:
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x0;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    default:
        /* For other mode, the MAC/PHY mode is not used */
        break;
    }

    /* MAC link restore (restore to original link state) */
    if((ret = dal_rtl9607c_port_macForceAbility_set(6, oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(6, oldState)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds1_indirectAccess(uint32 cmd, uint32 data)
{
    int ret;
    uint32 value, cnt = 1000;

    if((ret = reg_write(RTL9607C_WSDS_HSG1_DIG_02r, &data)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = reg_write(RTL9607C_WSDS_HSG1_DIG_01r, &cmd)) != RT_ERR_OK)
    {
        return ret;
    }

    while(1)
    {
        if((ret = reg_field_read(RTL9607C_WSDS_HSG1_DIG_01r, RTL9607C_CFG_SDS_MDX_REQf, &value)) != RT_ERR_OK)
        {
            return ret;
        }

        if(value == 0)
        {
            return RT_ERR_OK;
        }
        cnt --;
        if(cnt <= 0)
        {
            return RT_ERR_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds1_modeV1_set(uint32 mode)
{
    int ret;
    uint32 value;
    rtk_enable_t oldState;
    rtk_port_macAbility_t oldAbility, newAbility;

    /* Parameter check */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
        break;
    case LAN_SDS_MODE_GE_FE_PHY:
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Mode switch need additional protection
     * The complete switching steps are listed below
     * 1. MAC link down (force link down)
     * 2. Stop clock
     * 3. Mode configuration
     *     a. Serdes specific configurations
     * 4. Restore clock
     *     b. SDS_CFG set
     *     c. Switch related configurations
     * 5. MAC link restore (restore to original link state)
     */

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = ioal_socMem32_read(0xB8000504, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value &= ~(1 << 14);
    if ((ret = ioal_socMem32_write(0xB8000504, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
#endif

    /* MAC link down (force link down) */
    if((ret = dal_rtl9607c_port_macForceAbility_get(7, &oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_get(7, &oldState)) != RT_ERR_OK)
    {
        return ret;
    }
    newAbility = oldAbility;
    newAbility.linkStatus = PORT_LINKDOWN;
    if((ret = dal_rtl9607c_port_macForceAbility_set(7, newAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(7, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Stop clock */
    value = 1;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Serdes specific configurations */
    value = 0x1f;
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        /* Based on FIB1G_SG1_Init_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1 
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xD
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x6828)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x0
         * ANA0E[6:4]: REG_TX_DRV_VLDON[2:0]=0x0
         * ANA0E[3:1]: REG_TX_DRV_VLDOP[2:0]=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[8:1]: SSC_N_CODE<7:0>=0x61 (8'b97) */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00f, 0x52C2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* #ANA20[12]: REG_FIB1G_CLKRD_SEL=0x1
         * #ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x3
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x9103)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x3 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x3000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        /* Based on SG1_Init_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xD
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x6828)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x0
         * ANA0E[6:4]: REG_TX_DRV_VLDON[2:0]=0x4
         * ANA0E[3:1]: REG_TX_DRV_VLDOP[2:0]=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x49)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[8:1]: SSC_N_CODE<7:0>=0x61 (8'b97) */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00f, 0x52C2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* #ANA20[12]: REG_FIB1G_CLKRD_SEL=0x1
         * #ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x3
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x9103)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x3 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x3000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        /* Based on HSG1_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xD
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x2
         * ANA06[1:0]: REG_REFCLK_THRESHOLD<1:0>=0x2
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x684A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x1
         * ANA0E[6:4]: REG_TX_DRV_VLDON<2:0>=0x0
         * ANA0E[3:1]: REG_TX_DRV_VLDOP<2:0>=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x81)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:15]: REG_CMU_SEL_CP<15:15>=0x1
         * ANA0F[8:1]: SSC_N_CODE<7:0>=0x7A
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00F, 0xD2F4)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x8102)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x1000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Based on 2500BASE_X_HSG1_Patch_20170914 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xD
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x2
         * ANA06[1:0]: REG_REFCLK_THRESHOLD<1:0>=0x2
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x684A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x1
         * ANA0E[6:4]: REG_TX_DRV_VLDON<2:0>=0x0
         * ANA0E[3:1]: REG_TX_DRV_VLDOP<2:0>=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x81)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:15]: REG_CMU_SEL_CP<15:15>=0x1
         * ANA0F[8:1]: SSC_N_CODE<7:0>=0x7A
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00F, 0xD2F4)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x8102)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x1000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* Restore clock */
    value = 0;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* SDS_CFG set */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        value = 0x4;
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        value = 0x2;
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x12;
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        value = 0x16;
        break;
    }
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Set switch ready, phy patch done */
    value = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr, RTL9607C_PATCH_PHY_DONEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch related configurations */
    value = 0x0;
    if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Configure LINE_RATE_2500M */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4ffff;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_RATEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }
    /* Disable port 7 trap insert tag */
    if((ret = dal_rtl9607c_cpu_trapInsertTagByPort_set(7, DISABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* MAC/PHY mode configuration */
    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_HSGMII_PHY:
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x0;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    default:
        /* For other mode, the MAC/PHY mode is not used */
        break;
    }

    /* MAC link restore (restore to original link state) */
    if((ret = dal_rtl9607c_port_macForceAbility_set(7, oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(7, oldState)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds1_modeV2_set(uint32 mode)
{
    int ret;
    uint32 value;
    rtk_enable_t oldState;
    rtk_port_macAbility_t oldAbility, newAbility;

    /* Parameter check */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
        break;
    case LAN_SDS_MODE_GE_FE_PHY:
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Mode switch need additional protection
     * The complete switching steps are listed below
     * 1. MAC link down (force link down)
     * 2. Stop clock
     * 3. Mode configuration
     *     a. Serdes specific configurations
     * 4. Restore clock
     *     b. SDS_CFG set
     *     c. Switch related configurations
     * 5. MAC link restore (restore to original link state)
     */

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = ioal_socMem32_read(0xB8000504, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value &= ~(1 << 14);
    if ((ret = ioal_socMem32_write(0xB8000504, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
#endif

    /* MAC link down (force link down) */
    if((ret = dal_rtl9607c_port_macForceAbility_get(7, &oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_get(7, &oldState)) != RT_ERR_OK)
    {
        return ret;
    }
    newAbility = oldAbility;
    newAbility.linkStatus = PORT_LINKDOWN;
    if((ret = dal_rtl9607c_port_macForceAbility_set(7, newAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(7, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Stop clock */
    value = 1;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Serdes specific configurations */
    value = 0x1f;
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        /* Based on FIB1G_SG1_Init_Patch_20180131 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1 
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[2:0]: REG_RX_PI_SLEW=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x8049)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xD
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x6828)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x0
         * ANA0E[6:4]: REG_TX_DRV_VLDON[2:0]=0x0
         * ANA0E[3:1]: REG_TX_DRV_VLDOP[2:0]=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[8:1]: SSC_N_CODE<7:0>=0x61 (8'b97) */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00f, 0x52C2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* #ANA20[12]: REG_FIB1G_CLKRD_SEL=0x1
         * #ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x5
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x9105)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x3 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x3000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        /* Based on SG1_Init_Patch_20180131 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[2:0]: REG_RX_PI_SLEW=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x8049)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xD
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x6828)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x0
         * ANA0E[6:4]: REG_TX_DRV_VLDON[2:0]=0x4
         * ANA0E[3:1]: REG_TX_DRV_VLDOP[2:0]=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x49)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[8:1]: SSC_N_CODE<7:0>=0x61 (8'b97) */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00f, 0x52C2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* #ANA20[12]: REG_FIB1G_CLKRD_SEL=0x1
         * #ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x5
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x9105)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x3 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x3000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        /* Based on HSG1_Patch_20190129 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x3
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f7)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* 
         * ANA04[15:12]: REG_ZO_RX=0xB
         * ANA04[2:0]: REG_RX_PI_SLEW=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0xb049)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xA
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x1
         * ANA06[1:0]: REG_REFCLK_THRESHOLD<1:0>=0x2
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x502A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x1
         * ANA0E[6:4]: REG_TX_DRV_VLDON<2:0>=0x2
         * ANA0E[3:1]: REG_TX_DRV_VLDOP<2:0>=0x2
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0xA5)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:15]: REG_CMU_SEL_CP<15:15>=0x1
         * ANA0F[8:1]: SSC_N_CODE<7:0>=0x7A
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00F, 0xD2F4)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x4 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x8104)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x1000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Based on 2500BASE_X_HSG1_Patch_20180131 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Analog SDS configuraations */
        /* ANA00[11]: REG_AMP_OFFSET_AUTO_K=0x0
         * ANA00[4]: PWR_PLLDRV=0x0
         * ANA00[0]: REG_CMU_BYPASS_PI=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc000, 0x8241)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA02[15:13]: REG_KI<2:0>=0x1
         * ANA02[3:1]: REG_KP1<2:0>=0x4
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc002, 0x26f9)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[9]:REG_TX_NLDO_TIE1=0x0
         * ANA03[8]:REG_TX_NLDO_TIE0=0x0
         * ANA03[6:4]: REG_KP2<2:0>=0x1
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x689d)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04[2:0]: REG_RX_PI_SLEW=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x8049)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[15:11]: REG_CMU_SEL_CP_I<4:0>=0xA
         * ANA06[7:5]: REG_CMU_SEL_R1<2:0>=0x1
         * ANA06[1:0]: REG_REFCLK_THRESHOLD<1:0>=0x2
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x502A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0B[15:3]: SSC_F_CODE<12:0>=0x0
         * ANA0B[1]: REG_RX_PIENSEL=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00b, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0C[14]: REG_RX_SEL_RXIDLE=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00c, 0x0800)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0E[11:7]: REG_TX_DEEMP_EM<4:0>=0x0
         * ANA0E[6:4]: REG_TX_DRV_VLDON<2:0>=0x0
         * ANA0E[3:1]: REG_TX_DRV_VLDOP<2:0>=0x0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00e, 0x1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0F[15:15]: REG_CMU_SEL_CP<15:15>=0x1
         * ANA0F[8:1]: SSC_N_CODE<7:0>=0x7A
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00F, 0xD2F4)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[5:0]: REG_EQ_AC_GAIN<5:0>=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0x8105)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[13:12]: REG_RATE_SEL=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x1000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_SDS_REG7[14]: SP_CFG_NEG_CLKWR_A2D=0x1 */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    /* Restore clock */
    value = 0;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* SDS_CFG set */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        value = 0x4;
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        value = 0x2;
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x12;
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        value = 0x16;
        break;
    }
    if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Set switch ready, phy patch done */
    value = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr, RTL9607C_PATCH_PHY_DONEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch related configurations */
    value = 0x0;
    if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Configure LINE_RATE_2500M */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4ffff;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_RATEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }
    /* Disable port 7 trap insert tag */
    if((ret = dal_rtl9607c_cpu_trapInsertTagByPort_set(7, DISABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* MAC/PHY mode configuration */
    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_HSGMII_PHY:
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x0;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    default:
        /* For other mode, the MAC/PHY mode is not used */
        break;
    }

    /* MAC link restore (restore to original link state) */
    if((ret = dal_rtl9607c_port_macForceAbility_set(7, oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(7, oldState)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int _rtl9607c_lan_sds1_modeV3_set(uint32 mode)
{
    int ret;
    uint32 value;
    rtk_enable_t oldState;
    rtk_port_macAbility_t oldAbility, newAbility;

    /* Parameter check */
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
        break;
    case LAN_SDS_MODE_GE_FE_PHY:
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Mode switch need additional protection
     * The complete switching steps are listed below
     * 1. MAC link down (force link down)
     * 2. Stop clock
     * 3. Mode configuration
     *     a. Serdes specific configurations
     * 4. Restore clock
     *     b. SDS_CFG set
     *     c. Switch related configurations
     * 5. MAC link restore (restore to original link state)
     */

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = ioal_socMem32_read(0xB8000504, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value &= ~(1 << 14);
    if ((ret = ioal_socMem32_write(0xB8000504, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
#endif

    /* MAC link down (force link down) */
    if((ret = dal_rtl9607c_port_macForceAbility_get(7, &oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_get(7, &oldState)) != RT_ERR_OK)
    {
        return ret;
    }
    newAbility = oldAbility;
    newAbility.linkStatus = PORT_LINKDOWN;
    if((ret = dal_rtl9607c_port_macForceAbility_set(7, newAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(7, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Serdes specific configurations */
    /* UTP_FIBER_AUTODET[8]: SDS_CMD_STOP_GLI_CLK=1 */
    value = 0x1;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
        /* Based on 1000Base-X_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* Fiber 1G mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[28:27]=00 PLL/4 APHY speed mode selection for Serde port 7
         * CPU_PLL_CTRL_DUMMY[26]= 1 port7 Fiber 1G
         */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[5:0]:REG_CDR_KP=0x28 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc001, 0xA868)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL=0x16 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc005, 0xE312)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc009, 0x521C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00A, 0xB668)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[3:0]:REG_TXDRV_DAC_DAT=0x4 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0xD4A4)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[7:4]:REG_TXDRV_DAC_POST1=0x6 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x886A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA22[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc102, 0x0063)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA23[15:12]:REG_TX_TERM=0x2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc103, 0x2B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA24[6:0]:REG_FILTER_OUT=0x0C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc104, 0x4F0C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA28[10:6]:REG_EQOUT_OFFSET[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc108, 0xF802)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA29[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA2B[15]:PWR_PLLDRV=0
         * ANA2B[7:6]:REG_RX_PI_TRISEL<2:1>=0x10
         * ANA2B[2]:REG_RX_PI_TRISEL<0>=0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc10B, 0x00A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Fiber 1G mode */
        value = 0x4;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        /* Based on SGMII_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* SGMII mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x2;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[28:27]=00 PLL/4 APHY speed mode selection for Serde port 7
         * CPU_PLL_CTRL_DUMMY[26]= 1 port7 SGMII
         */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[5:0]:REG_CDR_KP=0x28 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc001, 0xA868)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL=0x16 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc005, 0xE312)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc009, 0x521C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00A, 0xB668)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[3:0]:REG_TXDRV_DAC_DAT=0x2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0xD4A2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21[7:4]:REG_TXDRV_DAC_POST1=0x6 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x886A)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA22[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc102, 0x0063)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA23[15:12]:REG_TX_TERM=0x2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc103, 0x2B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA24[6:0]:REG_FILTER_OUT=0x0C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc104, 0x4F0C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA28[10:6]:REG_EQOUT_OFFSET[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc108, 0xF802)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA29[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA2B[15]:PWR_PLLDRV=0
         * ANA2B[7:6]:REG_RX_PI_TRISEL<2:1>=0x10
         * ANA2B[2]:REG_RX_PI_TRISEL<0>=0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc10B, 0x00A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* SGMII mode */
        value = 0x2;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
        /* Based on HSGMII_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* HiSGMII mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x12;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[28:27]=01 PLL/2 APHY speed mode selection for Serde port 7
         * CPU_PLL_CTRL_DUMMY[26]= 0 port7 HiSGMII
         */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[5:0]:REG_CDR_KP=0xc */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc001, 0xA84C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL=0x16 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]:N_CODE <7:0>=0x7b */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc005, 0xE3DA)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc009, 0x521C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00A, 0xB668)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[3:0]:REG_TXDRV_DAC_DAT=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0xD4A5)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x88AA)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA22[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc102, 0x0063)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA23[15:12]:REG_TX_TERM=0x8 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc103, 0x8B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA24[6:0]:REG_FILTER_OUT=0x0C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc104, 0x4F0C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA28[10:6]:REG_EQOUT_OFFSET[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc108, 0xF802)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA29[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA2B[15]:PWR_PLLDRV=0
         * ANA2B[7:6]:REG_RX_PI_TRISEL<2:1>=0x10
         * ANA2B[2]:REG_RX_PI_TRISEL<0>=0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc10B, 0x00A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HiSGMII mode */
        value = 0x12;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Based on 2500Base-X_Patch_20211214 */
        /* Release MDIO reset */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_WSDS_HSG1_ANA_00r, RTL9607C_CFG_MDIO_RSTBf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* COMBO SDS enable */
        /* 2500BaseX mode */
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_COMBO_SDS_ENf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x16;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* CPU_PLL_CTRL_DUMMY[28:27]=01 PLL/2 APHY speed mode selection for Serde port 7
         * CPU_PLL_CTRL_DUMMY[26]= 0 port7 2500BaseX
         */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SPDSEL_SGMII_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_CPU_PLL_CTRL_DUMMYr, RTL9607C_SGMII_SEL_PCIEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA01[5:0]:REG_CDR_KP=0xc */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc001, 0xA84C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA03[5:1]:REG_OOBS_SEN_VAL=0x16 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc003, 0x276D)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA04 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc004, 0x5000)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA05[10:3]:N_CODE <7:0>=0x7b */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc005, 0xE3DA)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA06[4:0]:REG_CMU_CP_ISEL =0x1C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc006, 0x001C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA09 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc009, 0x521C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0A[5]:REG_EQ_SELREG=1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00A, 0xB668)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA0D[5:2]:REG_RX_Z0_RX=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc00D, 0xE714)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA20[3:0]:REG_TXDRV_DAC_DAT=0x5 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc100, 0xD4A5)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA21 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc101, 0x88AA)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA22[15:6]:REG_TX_DEEMP_EN=0x1 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc102, 0x0063)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA23[15:12]:REG_TX_TERM=0x8 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc103, 0x8B66)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA24[6:0]:REG_FILTER_OUT=0x0C */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc104, 0x4F0C)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA28[10:6]:REG_EQOUT_OFFSET[4:0]=0x0 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc108, 0xF802)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA29[15:0]:REG_RX_RESERVED=0xF0F2 */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc109, 0xF0F2)) != RT_ERR_OK)
        {
            return ret;
        }
        /* ANA2B[15]:PWR_PLLDRV=0
         * ANA2B[7:6]:REG_RX_PI_TRISEL<2:1>=0x10
         * ANA2B[2]:REG_RX_PI_TRISEL<0>=0
         */
        if((ret = _rtl9607c_lan_sds1_indirectAccess(0xc10B, 0x00A1)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Reset SDS */
        value = 0x1F;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* 2500BaseX mode */
        value = 0x16;
        if((ret = reg_array_field_write(RTL9607C_SDS_CFGr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_CFG_SDS_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_REG0[11]: FP_CFG_FIB_PDOWN=0x0 */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_REG0r, RTL9607C_FP_CFG_FIB_PDOWNf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* HSG1_FIB_EXT_REG19[15:14]: FEP_CFG_TX_MODE=0x0 - silent start disable */
        value = 0x0;
        if((ret = reg_field_write(RTL9607C_HSG1_FIB_EXT_REG19r, RTL9607C_FEP_CFG_TX_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }
    /* UTP_FIBER_AUTODET[8]: SDS_CMD_STOP_GLI_CLK=0 */
    value = 0x0;
    if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_CMD_STOP_GLI_CLKf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Set switch ready, phy patch done */
    value = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr, RTL9607C_PATCH_PHY_DONEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch related configurations */
    value = 0x0;
    if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(mode)
    {
    case LAN_SDS_MODE_FIBER_1G:
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_SGMII_MAC:
        break;
    case LAN_SDS_MODE_HSGMII_PHY:
    case LAN_SDS_MODE_HSGMII_MAC:
    case LAN_SDS_MODE_2500BASEX:
    /* Move default here to prevent coverity dead code error */
    default:
        /* Configure LINE_RATE_2500M */
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_EN_RATE_2500Mf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x4ffff;
        if((ret = reg_field_write(RTL9607C_LINE_RATE_2500M_1r, RTL9607C_RATEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }
    /* Disable port 7 trap insert tag */
    if((ret = dal_rtl9607c_cpu_trapInsertTagByPort_set(7, DISABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    /* MAC/PHY mode configuration */
    switch(mode)
    {
    case LAN_SDS_MODE_SGMII_PHY:
    case LAN_SDS_MODE_HSGMII_PHY:
        value = 0x1;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    case LAN_SDS_MODE_SGMII_MAC:
    case LAN_SDS_MODE_HSGMII_MAC:
        value = 0x0;
        if((ret = reg_array_field_write(RTL9607C_UTP_FIBER_AUTODETr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_SDS_PHY_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    default:
        /* For other mode, the MAC/PHY mode is not used */
        break;
    }

    /* MAC link restore (restore to original link state) */
    if((ret = dal_rtl9607c_port_macForceAbility_set(7, oldAbility)) != RT_ERR_OK)
    {
        return ret;
    }
    if((ret = dal_rtl9607c_port_macForceAbilityState_set(7, oldState)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static uint8 lan_sds_mode=LAN_SDS_MODE_END;
static uint8 lan_sds1_mode=LAN_SDS_MODE_END;

/* Function Name:
 *      dal_rtl9607c_port_serdesMode_set
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
dal_rtl9607c_port_serdesMode_set(uint8 num, uint8 cfg)
{
    int32 ret;
    uint32 chip, rev, subtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d,cfg=%d",num, cfg);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((2 <=num), RT_ERR_INPUT);
    RT_PARAM_CHK((LAN_SDS_MODE_END <=cfg), RT_ERR_INPUT);

    /* function body */
    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
   
    if(num==0)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        switch(rev)
        {
        case CHIP_REV_ID_0:
        case CHIP_REV_ID_A:
            _rtl9607c_lan_sds0_modeV1_set(cfg);
            break;
        case CHIP_REV_ID_B:
            _rtl9607c_lan_sds0_modeV2_set(cfg);
            break;
        case CHIP_REV_ID_C:
        default:
            _rtl9607c_lan_sds0_modeV3_set(cfg);
            break;
        }
#endif
        lan_sds_mode = cfg;
    }
    else
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        switch(rev)
        {
        case CHIP_REV_ID_0:
        case CHIP_REV_ID_A:
            _rtl9607c_lan_sds1_modeV1_set(cfg);
            break;
        case CHIP_REV_ID_B:
            _rtl9607c_lan_sds1_modeV2_set(cfg);
            break;
        case CHIP_REV_ID_C:
        default:
            _rtl9607c_lan_sds1_modeV3_set(cfg);
            break;
        }
#endif
        lan_sds1_mode = cfg;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_serdesMode_set */



/* Function Name:
 *      dal_rtl9607c_port_serdesMode_get
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
dal_rtl9607c_port_serdesMode_get(uint8 num, uint8 *cfg)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d",num);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((2 <=num), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == cfg), RT_ERR_NULL_POINTER);

    /* function body */
    if(num==0)
    {
        *cfg = lan_sds_mode;
    }
    else
    {
        *cfg = lan_sds1_mode;
    }
    
    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_serdesMode_get */



/* Function Name:
 *      dal_rtl9607c_port_serdesNWay_set
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
dal_rtl9607c_port_serdesNWay_set(uint8 num, uint8 cfg)
{
    int ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d,cfg=%d",num, cfg);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((2 <=num), RT_ERR_INPUT);
    RT_PARAM_CHK((LAN_SDS_NWAY_END <=cfg), RT_ERR_INPUT);

    /* function body */
    if(num==0)
    {
        //lan_sds_nway_set(cfg);
        if(cfg==LAN_SDS_NWAY_AUTO)
            value = 0;
        else
            value = 1;

        if((ret = reg_field_write(RTL9607C_HSG0_SDS_REG2r, RTL9607C_SP_SDS_FRC_ANf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        //lan_sds1_nway_set(cfg);
        if(cfg==LAN_SDS_NWAY_AUTO)
            value = 0;
        else
            value = 1;

        if((ret = reg_field_write(RTL9607C_HSG1_SDS_REG2r, RTL9607C_SP_SDS_FRC_ANf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        return RT_ERR_OK;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_serdesNWay_set */

/* Function Name:
 *      dal_rtl9607c_port_serdesNWay_get
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
dal_rtl9607c_port_serdesNWay_get(uint8 num, uint8 *cfg)
{
    int ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "num=%d",num);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((2 <=num), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == cfg), RT_ERR_NULL_POINTER);

    /* function body */
    if(num==0)
    {
        //lan_sds_nway_get(&mode);
        if((ret = reg_field_read(RTL9607C_HSG0_SDS_REG2r, RTL9607C_SP_SDS_FRC_ANf, &value)) != RT_ERR_OK)
        {
            return ret;
        }

        if(value==0)
            *cfg=LAN_SDS_NWAY_AUTO;
        else
            *cfg=LAN_SDS_NWAY_FORCE;
    }
    else
    {
        //lan_sds1_nway_get(&mode);
        if((ret = reg_field_read(RTL9607C_HSG1_SDS_REG2r, RTL9607C_SP_SDS_FRC_ANf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
         if(value==0)
            *cfg=LAN_SDS_NWAY_AUTO;
        else
            *cfg=LAN_SDS_NWAY_FORCE;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_port_serdesNWay_get */

static int dal_rtl9607c_port_eeeEnable_patch(rtk_port_t port)
{
    int32 ret;
    uint32 wData;

    if((ret=rtl9607c_ocpInterPhy_write(port, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(port, 0xa438, 0x2735))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
    wData = 0x2aa00;
    if ((ret = reg_array_write(RTL9607C_PHY_GDAC_IB_10Mr, REG_ARRAY_INDEX_NONE, port, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int dal_rtl9607c_port_eeeDisable_patch(rtk_port_t port)
{
    int32 ret;
    uint32 wData;

    if((ret=rtl9607c_ocpInterPhy_write(port, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(port, 0xa438, 0x2733))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
    wData = 0x2ff00;
    if ((ret = reg_array_write(RTL9607C_PHY_GDAC_IB_10Mr, REG_ARRAY_INDEX_NONE, port, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607c_port_eeeEnable_set
 * Description:
 *      Set EEE enable/disable state
 * Input:
 *      port   - port id
 *      enable - enable status of EEE
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
dal_rtl9607c_port_eeeEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 rData, wData;
    rtk_enable_t nwayEnable;
    uint32 chip, rev, subtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(!HAL_IS_PHY_EXIST(port) || HAL_IS_PON_PORT(port) || HAL_IS_CPU_PORT(port) || HAL_IS_RGMII_PORT(port))
    {
        return RT_ERR_OK;
    }

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if(ENABLED == enable)
    {
        /* PHY page 0xa5d reg 16 bit 1 & 2 set to 1 */
        if((ret = hal_miim_read(port, 0xa5d, 16, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        wData = rData | ((1 << 2) | (1 << 1));
        if((ret = hal_miim_write(port, 0xa5d, 16, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }

        /* PHY page 0xa43 reg 25 bit 4 set to 1 */
        if((ret = hal_miim_read(port, 0xa43, 25, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        wData = rData | (1 << 4);
        if((ret = hal_miim_write(port, 0xa43, 25, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }

        switch(rev)
        {
        case CHIP_REV_ID_0:
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
            if((ret = dal_rtl9607c_port_eeeEnable_patch(port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
            break;
        case CHIP_REV_ID_C:
        default:
            break;
        }
    }
    else
    {
        /* PHY page 0xa5d reg 16 bit 1 & 2 set to 0 */
        if((ret = hal_miim_read(port, 0xa5d, 16, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        wData = rData & ~((1 << 2) | (1 << 1));
        if((ret = hal_miim_write(port, 0xa5d, 16, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        /* PHY page 0xa43 reg 25 bit 4 set to 0 */
        if((ret = hal_miim_read(port, 0xa43, 25, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        wData = rData & ~(1 << 4);
        if((ret = hal_miim_write(port, 0xa43, 25, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        switch(rev)
        {
        case CHIP_REV_ID_0:
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
            if((ret = dal_rtl9607c_port_eeeDisable_patch(port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
            break;
        case CHIP_REV_ID_C:
        default:
            break;
        }
    }

    /* Use auto negotiation enable set to do re-nway */
    if ((ret = phy_autoNegoEnable_get(port, &nwayEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if ((ret = phy_autoNegoEnable_set(port, nwayEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_eeeEnable_set */

/* Function Name:
 *      dal_rtl9607c_port_eeeEnable_get
 * Description:
 *      Get EEE enable/disable state
 * Input:
 *      port   - port id
 * Output:
 *      pEnable - enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_port_eeeEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 rData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(!HAL_IS_PHY_EXIST(port) || HAL_IS_PON_PORT(port) || HAL_IS_CPU_PORT(port) || HAL_IS_RGMII_PORT(port))
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    /* Use some of the configured bits to represent the enable state */
    /* Check if PHY page 0xa5d reg 16 bit 1 & 2 are 1 */
    if((ret = hal_miim_read(port, 0xa5d, 16, &rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((rData & 0x6) != 0)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_port_eeeEnable_get */

