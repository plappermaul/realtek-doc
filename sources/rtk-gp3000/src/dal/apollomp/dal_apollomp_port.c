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
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <rtk/port.h>
#include <rtk/ponmac.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/dal_apollomp_port.h>
#include <dal/apollomp/raw/apollomp_raw_port.h>
#include <osal/lib.h>
#include <hal/common/miim.h>
#include <osal/time.h>
#include <hal/phy/phydef.h>

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/delay.h>
#endif
/*
 * Symbol Definition
 */

typedef struct dal_apollomp_phy_info_s {
    uint8   force_mode_speed[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_duplex[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_flowControl[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_pause[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_asy_pause[RTK_MAX_NUM_OF_PORTS];
} dal_apollomp_phy_info_t;

/*
 * Data Declaration
 */
static uint32    port_init = INIT_NOT_COMPLETED;

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
static uint32 adminPhyConfig = DISABLED;
#endif

static dal_apollomp_phy_info_t   phy_info;




/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_port_init
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
dal_apollomp_port_init(void)
{
    int32 ret;
    rtk_port_t port;
    rtk_portmask_t allPortmask;
    rtk_portmask_t allExtPortmask;
    rtk_port_phy_ability_t phy_ability;
    apollomp_raw_port_ability_t mac_ability;
    uint32 data;
    uint32  chipId,rev,subType  = 0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    port_init = INIT_COMPLETED;

    osal_memset(&phy_info, 0x00, sizeof(dal_apollomp_phy_info_t));

    HAL_GET_ALL_PORTMASK(allPortmask);
    HAL_GET_ALL_EXT_PORTMASK(allExtPortmask);


    if((ret = dal_apollomp_switch_version_get(&chipId,&rev,&subType)) != RT_ERR_OK)
    {
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        phy_ability.Half_10     = ENABLED;
        phy_ability.Full_10     = ENABLED;
        phy_ability.Half_100    = ENABLED;
        phy_ability.Full_100    = ENABLED;
        phy_ability.Half_1000   = ENABLED;
        phy_ability.Full_1000   = ENABLED;
        phy_ability.FC          = ENABLED;
        phy_ability.AsyFC       = ENABLED;
        if(HAL_IS_PHY_EXIST(port))
        {
#if 0 /* move to miim.c phy_autoNegoAbility_set and phy_speed_set */
            /* 9603 port3 is GiGA */
            if(3 == port)
            {
                switch(subType)
                {
                    case APPOLOMP_CHIP_SUB_TYPE_RTL9603:
                        phy_ability.Half_1000   = ENABLED;
                        phy_ability.Full_1000   = ENABLED;
                        break;
                    case APPOLOMP_CHIP_SUB_TYPE_RTL9602:
                        phy_ability.Half_1000   = DISABLED;
                        phy_ability.Full_1000   = DISABLED;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                /* 9602/9603 is FE */
                switch(subType)
                {
                    case APPOLOMP_CHIP_SUB_TYPE_RTL9602:
                    case APPOLOMP_CHIP_SUB_TYPE_RTL9603:
                        phy_ability.Half_1000   = DISABLED;
                        phy_ability.Full_1000   = DISABLED;
                        break;
                    default:
                        break;
                }
            }
#endif

            if((ret = dal_apollomp_port_phyAutoNegoAbility_set(port, &phy_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_phyAutoNegoEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_greenEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_phyCrossOverMode_set(port, PORT_CROSSOVER_MODE_AUTO)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            /*diable giga-lite by default*/
            if((ret = dal_apollomp_port_gigaLiteEnable_set(port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }



        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_port_macRemoteLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_macLocalLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_specialCongest_set(port, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationIpmcLeaky_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, port, &allPortmask, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationEntry_set(RTK_PORT_ISO_CFG_1, port, &allPortmask, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /* Force CPU port */
        if(port == HAL_GET_CPU_PORT())
        {
            osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
            mac_ability.speed           = PORT_SPEED_1000M;
            mac_ability.duplex          = PORT_FULL_DUPLEX;
            mac_ability.linkFib1g       = DISABLED;
            mac_ability.linkStatus      = PORT_LINKUP;
            mac_ability.txFc            = DISABLED;
            mac_ability.rxFc            = DISABLED;
            mac_ability.nwayAbility     = DISABLED;
            mac_ability.masterMod       = DISABLED;
            mac_ability.nwayFault       = DISABLED;
            mac_ability.lpi_100m        = DISABLED;
            mac_ability.lpi_giga        = DISABLED;
            if((ret = apollomp_raw_port_ForceAbility_set(port, &mac_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            data |= (0x01 << port);

            if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        else
        {
            osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
            mac_ability.linkStatus      = PORT_LINKDOWN;
            if((ret = apollomp_raw_port_ForceAbility_set(port, &mac_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
#if 0//defined(CONFIG_SWITCH_INIT_LINKDOWN)
            if((ret = dal_apollomp_port_adminEnable_set(port, DISABLED)) != RT_ERR_OK)
#else
            if((ret = dal_apollomp_port_adminEnable_set(port, ENABLED)) != RT_ERR_OK)
#endif
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

        }

        /* Disable extension port */
        if(port == HAL_GET_RGMII_PORT())
        {
            if((ret = dal_apollomp_port_macExtMode_set(port, EXT_MODE_DISABLE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_macExtRgmiiDelay_set(port, 0, 0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    HAL_SCAN_ALL_EXT_PORT(port)
    {
        if((ret = dal_apollomp_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_0, port, &allPortmask, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_1, port, &allPortmask, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_port_isolationCtagPktConfig_set(RTK_PORT_ISO_CFG_0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        port_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_port_isolationL34PktConfig_set(RTK_PORT_ISO_CFG_0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        port_init = INIT_NOT_COMPLETED;
        return ret;
    }

#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE) || defined(CONFIG_FIBER_FEATURE)
    /* Do nothing */
#else
    /* PON MAC select phy 4 enable*/
    /*default MAC4 init to UTP mode*/
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_CHIP_INF_SELr, APOLLOMP_PHY4_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }
    #if 0
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_UTP_FIBER_AUTODETr, APOLLOMP_CKGPHY_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }
    #endif
#endif

    data = 2;
    if ((ret = reg_field_write(APOLLOMP_IOPAD_CFGr, APOLLOMP_RGM_DPf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    data = 2;
    if ((ret = reg_field_write(APOLLOMP_IOPAD_CFGr, APOLLOMP_RGM_DNf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    data = 1;
    if ((ret = reg_field_write(APOLLOMP_IOPAD_CFGr, APOLLOMP_RGM_SEL33f, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    data = 1;
    if ((ret = reg_field_write(APOLLOMP_IOPAD_CFGr, APOLLOMP_DRI_EXCKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    data = 1;
    if ((ret = reg_field_write(APOLLOMP_IOPAD_CFGr, APOLLOMP_DRI_EXDTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      dal_apollomp_port_link_get
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
dal_apollomp_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_STATUSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    *pStatus = ((data == 1) ? PORT_LINKUP : PORT_LINKDOWN);
    return RT_ERR_OK;
} /* end of dal_apollomp_port_link_get */

/* Function Name:
 *      dal_apollomp_port_speedDuplex_get
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
dal_apollomp_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    uint8  getFromPhy = 0;
    int32  ret;
    uint32 force;
    uint32 link;
    uint32 speed;
    uint32 duplex;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);


    /* Due to link speed and duplex are not valid when UTP port MAC link down without MAC force enabled
     * Use the speed and duplex from PHY for this situation
     */

    /* UTP port only */
    if(!HAL_IS_CPU_PORT(port) && !HAL_IS_RGMII_PORT(port) && !HAL_IS_PON_PORT(port))
    {
        if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &force)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        /* Not forced */
        if(0 == (force & (0x01 << port)))
        {
            if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_STATUSf, &link)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
                return ret;
            }

            if(PORT_LINKDOWN == link)
            {
                getFromPhy = 1;
            }
        }
    }

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_STATUSf, &link)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if(0 == getFromPhy)
    {
        if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_SPDf, &speed)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_DUPLEXf, &duplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = hal_miim_read(port, 0xa43, 26, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
            return ret;
        }
        speed = (data & 0x30) >> 4;
        duplex = (data & 0x08) >> 3;
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
} /* end of dal_apollomp_port_speedDuplex_get */

/* Function Name:
 *      dal_apollomp_port_flowctrl_get
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
dal_apollomp_port_flowctrl_get(
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

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_TX_FCf, pTxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_RX_FCf, pRxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_flowctrl_get */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoEnable_get
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
dal_apollomp_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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
} /* end of dal_apollomp_port_phyAutoNegoEnable_get */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoEnable_set
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
dal_apollomp_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
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
} /* end of dal_apollomp_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoAbility_get
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
dal_apollomp_port_phyAutoNegoAbility_get(
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

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
} /* end of dal_apollomp_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoAbility_set
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
dal_apollomp_port_phyAutoNegoAbility_set(
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

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "dal_cypress_port_phyAutoNegoEnable_get(port=%d) failed!!", port);
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

    ret = dal_apollomp_ponmac_mode_get(&ponMode);
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
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_02r, APOLLOMP_FRC_REG4_ENf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        value = 0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_02r, APOLLOMP_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Configure pause field */
        if ((ret = reg_field_read(APOLLOMP_FIB_REG4r, APOLLOMP_TX_CFG_REG0805f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        if(1 == pAbility->FC)
        {
            /* Turn on pause by set bit [7] */
            value |= 0x4;
        }
        else
        {
            /* Turn on pause by clear bit [7] */
            value &= ~(0x4);
        }
        if(1 == pAbility->AsyFC)
        {
            /* Turn on pause by set bit [8] */
            value |= 0x8;
        }
        else
        {
            /* Turn on pause by clear bit [8] */
            value &= ~(0x8);
        }
        if ((ret = reg_field_write(APOLLOMP_FIB_REG4r, APOLLOMP_TX_CFG_REG0805f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Force select 100M registers */
        value = 1;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_02r, APOLLOMP_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Configure pause field */
        if ((ret = reg_field_read(APOLLOMP_FIB_REG4r, APOLLOMP_TX_CFG_REG1109f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        if(1 == pAbility->FC)
        {
            /* Turn on pause by set bit [10] */
            value |= 0x2;
        }
        else
        {
            /* Turn on pause by clear bit [10] */
            value &= ~(0x2);
        }
        if(1 == pAbility->AsyFC)
        {
            /* Turn on pause by set bit [11] */
            value |= 0x4;
        }
        else
        {
            /* Turn on pause by clear bit [11] */
            value &= ~(0x4);
        }

        if ((ret = reg_field_write(APOLLOMP_FIB_REG4r, APOLLOMP_TX_CFG_REG1109f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        /* Release force register */
        value = 0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_02r, APOLLOMP_FRC_REG4_ENf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
        value = 0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_02r, APOLLOMP_FRC_REG4_FIB100f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }

        /* Restart N-Way manually */
        value = 1;
        if ((ret = reg_field_write(APOLLOMP_FIB_REG0r, APOLLOMP_CFG_FIB_RESTARTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_apollomp_port_phyForceModeAbility_get
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
dal_apollomp_port_phyForceModeAbility_get(
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

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
} /* end of dal_apollomp_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_apollomp_port_phyForceModeAbility_set
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
dal_apollomp_port_phyForceModeAbility_set(
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

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
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
} /* end of dal_apollomp_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_apollomp_port_phyReg_get
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
dal_apollomp_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);


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
} /* end of dal_apollomp_port_phyReg_get */

/* Function Name:
 *      dal_apollomp_port_phyReg_set
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
dal_apollomp_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);


    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK(((page < HAL_MIIM_PAGE_ID_MIN()) && page != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((data > HAL_MIIM_DATA_MAX()), RT_ERR_INPUT);

    ret = hal_miim_write(port, page, reg, data);

    return ret;
} /* end of dal_apollomp_port_phyReg_set */

/* Function Name:
 *      dal_apollomp_port_phyMasterSlave_get
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
dal_apollomp_port_phyMasterSlave_get(
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMasterSlave), RT_ERR_NULL_POINTER);

    *pMasterSlave = PORT_AUTO_MODE;
    return RT_ERR_OK;
}/* end of dal_apollomp_port_phyMasterSlave_get */

/* Function Name:
 *      dal_apollomp_port_phyMasterSlave_set
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
dal_apollomp_port_phyMasterSlave_set(
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(masterSlave >= PORT_MASTER_SLAVE_END, RT_ERR_INPUT);


    return RT_ERR_OK;
}/* end of dal_apollomp_port_phyMasterSlave_set */

/* Function Name:
 *      dal_apollomp_port_phyTestMode_get
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
dal_apollomp_port_phyTestMode_get(rtk_port_t port, rtk_port_phy_test_mode_t *pTestMode)
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
    if((ret = dal_apollomp_port_phyReg_get(port, 0xA41, 9, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pTestMode = (data & 0xE000) >> 13;

    return RT_ERR_OK;
}   /* end of dal_apollomp_port_phyTestMode_get */

/* Function Name:
 *      dal_apollomp_port_phyTestMode_set
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
dal_apollomp_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t testMode)
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
    if((ret = dal_apollomp_port_phyReg_get(port, 0xA41, 9, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    data &= 0x1FFF;
    data |= (testMode << 13);

    if((ret = dal_apollomp_port_phyReg_set(port, 0xA41, 9, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_port_phyTestMode_set */

/* Function Name:
 *      dal_apollomp_port_cpuPortId_get
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
dal_apollomp_port_cpuPortId_get(rtk_port_t *pPort)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    *pPort = (rtk_port_t)HAL_GET_CPU_PORT();
    return RT_ERR_OK;
} /* end of dal_apollomp_port_cpuPortId_get */

/* Function Name:
 *      dal_apollomp_port_isolationEntry_get
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
dal_apollomp_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_P_MODE0_CTRLr : APOLLOMP_PISO_P_MODE1_CTRLr;
    if((ret = reg_array_field_read(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x007F;
    pExtPortmask->bits[0] = (regData & 0x1F80) >> 7;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntry_get */

/* Function Name:
 *      dal_apollomp_port_isolationEntry_set
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
dal_apollomp_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_P_MODE0_CTRLr : APOLLOMP_PISO_P_MODE1_CTRLr;
    regData = ((pExtPortmask->bits[0] << 7) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntry_set */

/* Function Name:
 *      dal_apollomp_port_isolationEntryExt_get
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
dal_apollomp_port_isolationEntryExt_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_EXT_MODE0_CTRLr : APOLLOMP_PISO_EXT_MODE1_CTRLr;
    if((ret = reg_array_field_read(regAddr, REG_ARRAY_INDEX_NONE, (int32)port, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x007F;
    pExtPortmask->bits[0] = (regData & 0x1F80) >> 7;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntryExt_get */

/* Function Name:
 *      dal_apollomp_port_isolationEntryExt_set
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
dal_apollomp_port_isolationEntryExt_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_EXT_MODE0_CTRLr : APOLLOMP_PISO_EXT_MODE1_CTRLr;
    regData = ((pExtPortmask->bits[0] << 7) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, REG_ARRAY_INDEX_NONE, (int32)port, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntryExt_set */

/* Function Name:
 *      dal_apollomp_port_isolationCtagPktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationCtagPktConfig_get(rtk_port_isoConfig_t *pMode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_PISO_CTRLr, APOLLOMP_CTAG_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pMode = (1 == regData) ? RTK_PORT_ISO_CFG_1 : RTK_PORT_ISO_CFG_0;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationCtagPktConfig_get */

/* Function Name:
 *      dal_apollomp_port_isolationCtagPktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationCtagPktConfig_set(rtk_port_isoConfig_t mode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((mode >= RTK_PORT_ISO_CFG_END), RT_ERR_INPUT);

    regData = (uint32)mode;
    if((ret = reg_field_write(APOLLOMP_PISO_CTRLr, APOLLOMP_CTAG_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationCtagPktConfig_set */

/* Function Name:
 *      dal_apollomp_port_isolationL34PktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationL34PktConfig_get(rtk_port_isoConfig_t *pMode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_PISO_CTRLr, APOLLOMP_L34_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pMode = (1 == regData) ? RTK_PORT_ISO_CFG_1 : RTK_PORT_ISO_CFG_0;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationL34PktConfig_get */

/* Function Name:
 *      dal_apollomp_port_isolationL34PktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationL34PktConfig_set(rtk_port_isoConfig_t mode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((mode >= RTK_PORT_ISO_CFG_END), RT_ERR_INPUT);

    regData = (uint32)mode;
    if((ret = reg_field_write(APOLLOMP_PISO_CTRLr, APOLLOMP_L34_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationL34PktConfig_set */


/* Function Name:
 *      dal_apollomp_port_isolationIpmcLeaky_get
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
dal_apollomp_port_isolationIpmcLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationIpmcLeaky_get */

/* Function Name:
 *      dal_apollomp_port_isolationIpmcLeaky_set
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
dal_apollomp_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationIpmcLeaky_set */



/* Function Name:
 *      dal_apollomp_port_isolationPortLeaky_get
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
dal_apollomp_port_isolationPortLeaky_get(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;
    return RT_ERR_OK;
}   /* end of dal_apollomp_port_isolationPortLeaky_get */

/* Function Name:
 *      dal_apollomp_port_isolationPortLeaky_set
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
dal_apollomp_port_isolationPortLeaky_set(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_port_isolationPortLeaky_set */


static int32
_dal_apollomp_port_rmaPortIsolationLeakyEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
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
                regAddr = APOLLOMP_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = APOLLOMP_RMA_CTRL_SSTPr;
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
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_read(regAddr, APOLLOMP_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of _dal_apollomp_port_rmaPortIsolationLeakyEnable_get */






static int32
_dal_apollomp_port_rmaPortIsolationLeakyEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
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
                regAddr = APOLLOMP_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = APOLLOMP_RMA_CTRL_SSTPr;
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
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_write(regAddr, APOLLOMP_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_apollomp_port_rmaPortIsolationLeakyEnable_set */


/* Function Name:
 *      dal_apollomp_port_isolationLeaky_get
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
dal_apollomp_port_isolationLeaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
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
            if (( ret = _dal_apollomp_port_rmaPortIsolationLeakyEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
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

            if (( ret = _dal_apollomp_port_rmaPortIsolationLeakyEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;

        case LEAKY_IGMP:
            if ((ret = reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_PISO_LEAKYf, (uint32 *)&data)) != RT_ERR_OK)
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
}   /* end of dal_apollomp_port_isolationLeaky_get */

/* Function Name:
 *      dal_apollomp_port_isolationLeaky_set
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
dal_apollomp_port_isolationLeaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
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
            if (( ret = _dal_apollomp_port_rmaPortIsolationLeakyEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
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

            if (( ret = _dal_apollomp_port_rmaPortIsolationLeakyEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
            data = enable;
            if ((ret = reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_PISO_LEAKYf, (uint32 *)&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                return ret;
            }

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;

    }


    return RT_ERR_OK;
}   /* end of dal_apollomp_port_isolationLeaky_set */

/* Function Name:
 *      dal_apollomp_port_macRemoteLoopbackEnable_get
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
dal_apollomp_port_macRemoteLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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
} /* end of dal_apollomp_port_macRemoteLoopbackEnable_get */

/* Function Name:
 *      dal_apollomp_port_macRemoteLoopbackEnable_set
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
dal_apollomp_port_macRemoteLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
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
} /* end of dal_apollomp_port_macRemoteLoopbackEnable_set */

/* Function Name:
 *      dal_apollomp_port_macLocalLoopbackEnable_get
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
dal_apollomp_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)data ;
    return RT_ERR_OK;
} /* end of dal_apollomp_port_macLocalLoopbackEnable_get */

/* Function Name:
 *      dal_apollomp_port_macLocalLoopbackEnable_set
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
dal_apollomp_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
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
    if((ret = reg_array_field_write(APOLLOMP_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_macLocalLoopbackEnable_set */

/* Function Name:
 *      dal_apollomp_port_adminEnable_get
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
dal_apollomp_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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

    if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(data & (0x01 << port) )
    {
        if((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_LINK_ABLTYf, &link)) != RT_ERR_OK)
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
} /* end of dal_apollomp_port_adminEnable_get */

/* Function Name:
 *      dal_apollomp_port_adminEnable_set
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
dal_apollomp_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(DISABLED == enable)
        data |= (0x01 << port);
    else
        data &= ~(0x01 << port);

    if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(DISABLED == enable)
    {
        data = 0;
        if((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_adminEnable_set */

/* Function Name:
 *      dal_apollomp_port_specialCongest_get
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
dal_apollomp_port_specialCongest_get(rtk_port_t port, uint32 *pSecond)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSecond), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_specialCongest_get */

/* Function Name:
 *      dal_apollomp_port_specialCongest_set
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
dal_apollomp_port_specialCongest_set(rtk_port_t port, uint32 second)
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
    if((ret = reg_array_field_write(APOLLOMP_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CGST_SUST_TMR_LMTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if((ret = reg_array_field_write(APOLLOMP_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CGST_SUST_TMR_LMT_Hf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_port_specialCongest_set */

/* Function Name:
 *      dal_apollomp_port_specialCongestStatus_get
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
dal_apollomp_port_specialCongestStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_SC_P_CTRL_1r, APOLLOMP_CGST_INDf, &regData)) != RT_ERR_OK)
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
 *      dal_apollomp_port_specialCongestStatus_get
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
dal_apollomp_port_specialCongestStatus_clear(rtk_port_t port)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    regData = (0x01 << port);
    if((ret = reg_field_write(APOLLOMP_SC_P_CTRL_1r, APOLLOMP_CGST_INDf, &regData)) != RT_ERR_OK)
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
dal_apollomp_port_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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
} /* end of dal_apollomp_port_greenEnable_get */

/* Function Name:
 *      dal_apollomp_port_greenEnable_set
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
dal_apollomp_port_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
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
} /* end of dal_apollomp_port_greenEnable_set */


/* Function Name:
 *      dal_apollomp_port_phyCrossOverMode_get
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
dal_apollomp_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
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
} /* end of dal_apollomp_port_phyCrossOverMode_get */

/* Function Name:
 *      dal_apollomp_port_phyCrossOverMode_set
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
dal_apollomp_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
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
} /* end of dal_apollomp_port_phyCrossOverMode_set */

/* Function Name:
 *      dal_apollomp_port_phyPowerDown_get
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
dal_apollomp_port_phyPowerDown_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    unsigned int data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = dal_apollomp_port_phyReg_get(port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (data & PowerDown_MASK) ? ENABLED : DISABLED;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyPowerDown_get */

/* Function Name:
 *      dal_apollomp_port_phyPowerDown_set
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
dal_apollomp_port_phyPowerDown_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    unsigned int data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = dal_apollomp_port_phyReg_get(port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
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

    if((ret = dal_apollomp_port_phyReg_set(port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    adminPhyConfig = enable;
#endif

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyPowerDown_set */

/* Function Name:
 *      dal_apollomp_port_phyPowerDown_tryset
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
dal_apollomp_port_phyPowerDown_tryset(rtk_port_t port, rtk_enable_t enable)
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

    return dal_apollomp_port_phyPowerDown_set(port, enable);
} /* end of dal_apollomp_port_phyPowerDown_tryset */

/* Function Name:
 *      dal_apollomp_port_enhancedFid_get
 * Description:
 *      Get port EFID
 * Input:
 *      port      - port id
 * Output:
 *      pEfid     - EFID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_apollomp_port_enhancedFid_get(rtk_port_t port, rtk_efid_t *pEfid)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEfid), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_L2_EFIDr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_EFIDf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEfid = (rtk_efid_t)data;
    return RT_ERR_OK;
} /* end of dal_apollomp_port_enhancedFid_get */

/* Function Name:
 *      dal_apollomp_port_enhancedFid_set
 * Description:
 *      Set port EFID
 * Input:
 *      port      - port id
 *      efid      - EFID
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
dal_apollomp_port_enhancedFid_set(rtk_port_t port, rtk_efid_t efid)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(efid > HAL_ENHANCED_FID_MAX(), RT_ERR_INPUT);

    data = (uint32)efid;
    if((ret = reg_array_field_write(APOLLOMP_L2_EFIDr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_EFIDf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_enhancedFid_set */


#define APOLLOMP_RTCT_DONE_OFFSET 15

static int32
_dal_apollomp_port_rtctResult_ge_get(uint32 index, uint32 rData, rtk_rtctResult_t *pRtctResult)
{
    switch(rData)
    {
        case 0x0060:
            pRtctResult->ge_result.channelNormal[index] = 1;
            break;
        case 0x0048:
            pRtctResult->ge_result.channelOpen[index] = 1;
            break;
        case 0x0050:
            pRtctResult->ge_result.channelShort[index] = 1;
            break;
        case 0x0042:
            pRtctResult->ge_result.channelMismatch[index] = 130;
            break;
        case 0x0044:
            pRtctResult->ge_result.channelMismatch[index] = 77;
            break;
        case 0x0041:
            pRtctResult->ge_result.channelHiImpedance[index] = 1;
            break;
    default:
        osal_printf("\nrtct invalid result:0x%x\n",rData);
        return RT_ERR_OK;
        break;
    }
    return RT_ERR_OK;
}


static int32 _dal_apollomp_port_rtctResult_Length_get(rtk_port_t port, rtk_port_speed_t speed,uint32 channel, int32 *testLen)
{
    uint32 index=0;
    uint32  wData, rData;
    int32 ret;
    uint32 pol;
    uint32 val;
    int32 coe_a,coe_b,coe_cf3;
    int32 len;
    int32 cb0, cf3;
    int32 cbPhyVal[] =  {0x0930, 0x0B30, 0x0D30, 0x0F30};
    int32 cf3PhyVal[] = {0x08B3, 0x0AB3, 0x0CB3, 0x0EB3};
    int32 testLength[30];

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_ETHER_PORT(port), RT_ERR_PORT_ID);
    if(PORT_SPEED_1000M==speed)
    {
        coe_a = -172;
        coe_b = 24;
        coe_cf3 = 100;/*must divid by 100*/
    }
    else
    {
        coe_a = -160;
        coe_b = 80;
        coe_cf3 = 100;/*must divid by 100*/
    }

    for(index = 0; index < 30; index++)
    {
        wData = cbPhyVal[channel];
        if((ret = hal_miim_write(port, 0xA84, 21, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        if((ret = hal_miim_read(port, 0xA84, 23, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }

        pol = ((rData & 0x2000) >> 13);
        val = (rData & 0x1FFF);
        cb0 = (val - (pol*8192)); /*need divid 8192 later*/

        wData = cf3PhyVal[channel];
        if((ret = hal_miim_write(port, 0xA84, 21, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        if((ret = hal_miim_read(port, 0xA84, 23, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        pol = ((rData & 0x8000) >> 15);
        val = (rData & 0x7FFF);
        cf3 = (val - (pol * 32768));  /*need divid 32768 later*/

        if(cf3==0 && cb0==0 )
            len =0;
        else
        {
            len = (coe_a*cb0/8192 + coe_a*coe_cf3*cf3/32768/100 + coe_b);
            if(len<=0)
                len =1;
        }
        testLength[index]=len;
    }

    /*sort testLength array*/
    {
        int32 n, c, d, swap;
        n = 30;
        for (c = 0 ; c < ( n - 1 ); c++)
        {
            for (d = 0 ; d < n - c - 1; d++)
            {
                if (testLength[d] > testLength[d+1]) /* For decreasing order use < */
                {
                    swap       = testLength[d];
                    testLength[d]   = testLength[d+1];
                    testLength[d+1] = swap;
                }
            }
        }
    }

    /*get entry 10~19*/
    {
        int32 totalLen=0;
        for(index = 10; index <= 19; index++)
        {
            totalLen= totalLen+testLength[index];
        }
        *testLen = totalLen/10;
    }

    return RT_ERR_OK;
}


rtk_rtctResult_t rtctResult[5];

/* Function Name:
 *      dal_apollomp_port_rtctResult_get
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
dal_apollomp_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_ETHER_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRtctResult), RT_ERR_NULL_POINTER);

    osal_memset(pRtctResult, 0x0, sizeof(rtk_rtctResult_t));

    memcpy(pRtctResult,&rtctResult[port],sizeof(rtk_rtctResult_t));

    return RT_ERR_OK;
}


/* Function Name:
 *      _dal_apollomp_port_currentRtctResult_get
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
static int32
_dal_apollomp_port_currentRtctResult_get(rtk_port_t port)
{
    int32 ret;
    uint32 i;
    uint32  wData, rData;
    rtk_port_speed_t speed;
    rtk_port_linkStatus_t linkStatus;
    rtk_port_duplex_t duplex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_ETHER_PORT(port), RT_ERR_PORT_ID);

    osal_memset(&rtctResult[port], 0x0, sizeof(rtk_rtctResult_t));
    rtctResult[port].linkType = PORT_SPEED_1000M;

    /*Get PHY speed, link status*/
    if((ret =  dal_apollomp_port_link_get(port, &linkStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    if((ret =  dal_apollomp_port_speedDuplex_get(port, &speed, &duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
    /*check length*/
    if(PORT_SPEED_10M != speed && linkStatus==PORT_LINKUP)
    {
        int checkLen;
        for(i = 0; i < 4; i++)
        {
            _dal_apollomp_port_rtctResult_Length_get(port,speed,i,&checkLen);
            rtctResult[port].ge_result.channelLen[i]=(uint32)checkLen;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
        mdelay(1000);
#endif

    }

    /*start RTCT check*/
    if((ret = hal_miim_write(port, 0xA42, 17, 0xF1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
                mdelay(10);
#endif
    i=0;
    do{
        /*Verify RTCT is finished*/
        if((ret = hal_miim_read(port, 0xA42, 17, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
        i++;
        if(i>1000000)
        {
            /*timeout disable rtct*/
            if((ret = hal_miim_write(port, 0xA42, 17, 0x0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
            osal_printf("\nRTCT timeout!\n");
            return RT_ERR_TIMEOUT;
        }
    }while(((rData >> APOLLOMP_RTCT_DONE_OFFSET) & 0x1) == 1);
#if defined(CONFIG_SDK_KERNEL_LINUX)
    mdelay(2000);
#endif

    /*Get result*/
    /*CH A ~ D*/
    wData = 0x802A;
    for(i = 0; i < 4; i++)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        mdelay(100);
#endif
        if((ret = hal_miim_write(port, 0xA43, 27, wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
        mdelay(100);
#endif
        if((ret = hal_miim_read(port, 0xA43, 28, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
        }
       if((ret =  _dal_apollomp_port_rtctResult_ge_get(i, rData, &rtctResult[port])) != RT_ERR_OK)
       {
            RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
            return ret;
       }
       wData = wData + 4;
    }

    if(PORT_SPEED_10M != speed && linkStatus==PORT_LINKUP)
    {

    }
    else
    {
        /*Get A ~ D Line Length*/
        wData = 0x802C;
        for(i = 0; i < 4; i++)
        {
            if(PORT_SPEED_10M != speed && linkStatus==PORT_LINKUP)
            {
                continue;
            }
            else
            {
                if((ret = hal_miim_write(port, 0xA43, 27, wData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                    return ret;
                }

                if((ret = hal_miim_read(port, 0xA43, 28, &rData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                    return ret;
                }
                rtctResult[port].ge_result.channelLen[i] = rData/80;
                wData = wData + 4;
            }
        }
    }

    /*Disable RTCT*/
    if((ret = hal_miim_write(port, 0xA42, 17, 0x0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /*end of _dal_apollomp_port_currentRtctResult_get*/




/* Function Name:
 *      dal_apollomp_port_rtct_start
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
dal_apollomp_port_rtct_start(rtk_portmask_t *pPortmask)
{
    int32 ret;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_INPUT);

    HAL_SCAN_ALL_PORT(port)
    {
        /*Enable RTCT*/
        if(RTK_PORTMASK_IS_PORT_SET((*pPortmask), port))
        {
            if((ret = _dal_apollomp_port_currentRtctResult_get(port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
                return ret;
            }
        }
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_port_rtct_start */


/* Function Name:
 *      dal_apollomp_port_macForceAbility_set
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
dal_apollomp_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
{
    int32 ret;
    apollomp_raw_port_ability_t ability;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);
    ability.duplex  = macAbility.duplex;
    ability.speed   = macAbility.speed;
    ability.linkFib1g= macAbility.linkFib1g;
    ability.linkStatus= macAbility.linkStatus;
    ability.txFc = macAbility.txFc;
    ability.rxFc = macAbility.rxFc;
    ability.nwayAbility= macAbility.nwayAbility;
    ability.masterMod = macAbility.masterMod;
    ability.nwayFault = macAbility.nwayFault;
    ability.lpi_100m = macAbility.lpi_100m;
    ability.lpi_giga = macAbility.lpi_giga;

    if((ret = apollomp_raw_port_ForceAbility_set(port,&ability))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_apollomp_port_macForceAbility_get
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
dal_apollomp_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
{
    int32 ret;
    apollomp_raw_port_ability_t ability;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK((NULL == pMacAbility), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_port_ForceAbility_get(port,&ability))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    pMacAbility->duplex = ability.duplex;
    pMacAbility->speed = ability.speed;
    pMacAbility->linkFib1g = ability.linkFib1g;
    pMacAbility->linkStatus = ability.linkStatus;
    pMacAbility->txFc = ability.txFc;
    pMacAbility->rxFc = ability.rxFc;
    pMacAbility->nwayAbility = ability.nwayAbility;
    pMacAbility->masterMod = ability.masterMod;
    pMacAbility->nwayFault = ability.nwayFault;
    pMacAbility->lpi_100m = ability.lpi_100m;
    pMacAbility->lpi_giga = ability.lpi_giga;


    return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_port_macForceAbility_set
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
dal_apollomp_port_macForceAbilityState_set(rtk_port_t port,rtk_enable_t state)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(state == ENABLED)
        data |= (0x01 << port);
    else
        data &= ~(0x01 << port);

    if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_apollomp_port_macForceAbilityState_get
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
dal_apollomp_port_macForceAbilityState_get(rtk_port_t port,rtk_enable_t *pState)
{
    int32 ret;
    uint32 data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pState = data & (0x01 << port) ? ENABLED : DISABLED ;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_port_macExtMode_set
 * Description:
 *      Set extension MAC mode
 * Input:
 *      port - the ports number
 *      ext_mode - the mode setting
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
dal_apollomp_port_macExtMode_set(rtk_port_t port, rtk_port_ext_mode_t ext_mode)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d,ext_mode=%d",port, ext_mode);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_RGMII_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((EXT_MODE_END <= ext_mode), RT_ERR_INPUT);
    RT_PARAM_CHK((EXT_MODE_RESERVED == ext_mode), RT_ERR_INPUT);

    /* function body */
    data = (uint32)ext_mode;
    if((ret = reg_field_write(APOLLOMP_MODE_EXTr, APOLLOMP_MODE_EXTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_port_macExtMode_set */

/* Function Name:
 *      dal_apollomp_port_macExtMode_get
 * Description:
 *      Get extension MAC mode
 * Input:
 *      port - the ports number
 * Output:
 *      pExt_mode - the mode setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_macExtMode_get(rtk_port_t port, rtk_port_ext_mode_t *pExt_mode)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_RGMII_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pExt_mode), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(APOLLOMP_MODE_EXTr, APOLLOMP_MODE_EXTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pExt_mode = (rtk_port_ext_mode_t)data;
    return RT_ERR_OK;
}   /* end of dal_apollomp_port_macExtMode_get */

/* Function Name:
 *      dal_apollomp_port_macExtRgmiiDelay_set
 * Description:
 *      Set RGMII TX/RX delay
 * Input:
 *      port - the ports number
 *      txDelay - the TX delay (0 ~ 1)
 *      rxDelay - the RX delay (0 ~ 7)
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
dal_apollomp_port_macExtRgmiiDelay_set(rtk_port_t port, uint32 txDelay, uint32 rxDelay)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d,txDelay=%d,rxDelay=%d",port, txDelay, rxDelay);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_RGMII_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_RGMII_TX_DELAY() < txDelay), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_RGMII_RX_DELAY() < rxDelay), RT_ERR_INPUT);

    /* function body */
    data = ((txDelay << 3) | rxDelay);
    if((ret = reg_field_write(APOLLOMP_EXT_RGMXFr, APOLLOMP_EXT_RGMXFf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_port_macExtRgmiiDelay_set */

/* Function Name:
 *      dal_apollomp_port_macExtRgmiiDelay_get
 * Description:
 *      Get RGMII TX/RX delay
 * Input:
 *      port - the ports number
 * Output:
 *      pTxDelay - the TX delay (0 ~ 1)
 *      pRxDelay - the RX delay (0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_macExtRgmiiDelay_get(rtk_port_t port, uint32 *pTxDelay, uint32 *pRxDelay)
{
    int32 ret;
    uint32 data=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_RGMII_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxDelay), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxDelay), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(APOLLOMP_EXT_RGMXFr, APOLLOMP_EXT_RGMXFf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pTxDelay = ((data >> 3) & 0x1);
    *pRxDelay = (data & 0x7);
    return RT_ERR_OK;
}   /* end of dal_apollomp_port_macExtRgmiiDelay_get */



#define APOLLOMP_GIGALITE_ENABLE_OFFSET    (8)
#define APOLLOMP_GIGALITE_ENABLE_MASK      (0x1U<<APOLLOMP_GIGALITE_ENABLE_OFFSET)

#define APOLLOMP_GIGALITE_EEE_ENABLE_OFFSET    (9)
#define APOLLOMP_GIGALITE_EEE_ENABLE_MASK      (0x1U<<APOLLOMP_GIGALITE_EEE_ENABLE_OFFSET)



#define APOLLOMP_GIGALITE_EEE_ABILITY_OFFSET    (7)
#define APOLLOMP_GIGALITE_EEE_ABILITY_MASK      (0x1U<<APOLLOMP_GIGALITE_EEE_ABILITY_OFFSET)


#define APOLLOMP_GIGALITE_SPEED_ABILITY0_OFFSET    (9)
#define APOLLOMP_GIGALITE_SPEED_ABILITY0_MASK      (0x1U<<APOLLOMP_GIGALITE_SPEED_ABILITY0_OFFSET)

#define APOLLOMP_GIGALITE_SPEED_ABILITY1_OFFSET    (10)
#define APOLLOMP_GIGALITE_SPEED_ABILITY1_MASK      (0x1U<<APOLLOMP_GIGALITE_SPEED_ABILITY1_OFFSET)

#define APOLLOMP_GIGALITE_SPEED_ABILITY2_OFFSET    (11)
#define APOLLOMP_GIGALITE_SPEED_ABILITY2_MASK      (0x1U<<APOLLOMP_GIGALITE_SPEED_ABILITY2_OFFSET)

#define APOLLOMP_GIGALITE_SPEED_ABILITY_ALL_MASK      (APOLLOMP_GIGALITE_SPEED_ABILITY0_MASK||APOLLOMP_GIGALITE_SPEED_ABILITY1_MASK|APOLLOMP_GIGALITE_SPEED_ABILITY1_MASK)
/* Function Name:
 *      dal_apollomp_port_gigaLiteEnable_set
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
dal_apollomp_port_gigaLiteEnable_set(rtk_port_t port, rtk_enable_t enable)
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

    phyData0 = phyData0 & ~(APOLLOMP_GIGALITE_ENABLE_MASK);
    phyData0 = phyData0 | (enable << APOLLOMP_GIGALITE_ENABLE_OFFSET);

#if 0 /*for gigalite EEE*/
    phyData0 = phyData0 & ~(APOLLOMP_GIGALITE_EEE_ENABLE_MASK);
    phyData0 = phyData0 | (enable << APOLLOMP_GIGALITE_EEE_ENABLE_OFFSET);
#endif

    if ((ret = hal_miim_write(port, 0xa4a, 17, phyData0)) != RT_ERR_OK)
        return ret;



    if ((ret = hal_miim_read(port, 0xa42, 20, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(APOLLOMP_GIGALITE_SPEED_ABILITY_ALL_MASK);
    phyData0 = phyData0 | (enable << APOLLOMP_GIGALITE_SPEED_ABILITY0_OFFSET) | (enable << APOLLOMP_GIGALITE_SPEED_ABILITY1_OFFSET) | (enable << APOLLOMP_GIGALITE_SPEED_ABILITY2_OFFSET);


#if 0 /*for gigalite EEE*/
    phyData0 = phyData0 & ~(APOLLOMP_GIGALITE_EEE_ABILITY_MASK);
    phyData0 = phyData0 | (enable << APOLLOMP_GIGALITE_EEE_ABILITY_OFFSET);
#endif

    if ((ret = hal_miim_write(port, 0xa42, 20, phyData0)) != RT_ERR_OK)
        return ret;



    return RT_ERR_OK;
}   /* end of dal_apollomp_port_gigaLiteEnable_set */

/* Function Name:
 *      dal_apollomp_port_gigaLiteEnable_get
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
dal_apollomp_port_gigaLiteEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
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

    if(phyData0 & (APOLLOMP_GIGALITE_ENABLE_MASK))
        *pEnable=ENABLED;
    else
        *pEnable=DISABLED;

    return RT_ERR_OK;
}   /* end of dal_apollomp_port_gigaLiteEnable_get */

/* Function Name:
 *      dal_apollomp_port_eeeEnable_set
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
dal_apollomp_port_eeeEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 rData, wData;
    uint16 phyData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(ENABLED == enable)
    {
        /* PHY page 0xa5d reg 16 bit 1 & 2 set to 1 */
        if((ret=apollomp_interPhy_read(port,0xa5d,16,&phyData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }
        phyData = phyData | 0x6;
        if((ret=apollomp_interPhy_write(port,0xa5d,16,phyData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }

        /* MAC(GPHY) set bit 6-9 to 1 */
        if ((ret = reg_array_read(APOLLOMP_P_EEECFGr, port, REG_ARRAY_INDEX_NONE, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        wData = rData | 0x3c0;
        if ((ret = reg_array_write(APOLLOMP_P_EEECFGr, port, REG_ARRAY_INDEX_NONE, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        /* PHY page 0xa5d reg 16 bit 1 & 2 set to 0 */
        if((ret=apollomp_interPhy_read(port,0xa5d,16,&phyData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }
        phyData = phyData & 0xFFF9;
        if((ret=apollomp_interPhy_write(port,0xa5d,16,phyData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }

        /* MAC0~4(GPHY0) set bit 6-9 to 0 */
        if ((ret = reg_array_read(APOLLOMP_P_EEECFGr, port, REG_ARRAY_INDEX_NONE, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        wData = rData & 0xFC3F;
        if ((ret = reg_array_write(APOLLOMP_P_EEECFGr, port, REG_ARRAY_INDEX_NONE, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_eeeEnable_set */

/* Function Name:
 *      dal_apollomp_port_eeeEnable_get
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
dal_apollomp_port_eeeEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 rData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* Use some of the configured bits to represent the enable state */
    if ((ret = reg_array_read(APOLLOMP_P_EEECFGr, port, REG_ARRAY_INDEX_NONE, &rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((rData & 0x3c0) == 0x3c0)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_eeeEnable_get */

