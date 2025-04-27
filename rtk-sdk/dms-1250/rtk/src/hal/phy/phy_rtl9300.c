/*
 * Copyright (C) 2016-2022 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 102478 $
 * $Date: 2019-12-10 18:49:50 +0800 (周二, 10 12月 2019) $
 *
 * Purpose : PHY 9300 intra serdes Driver APIs.
 *
 * Feature : PHY 9300 intra serdes Driver APIs
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl9300.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <dal/dal_construct.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9300.h>
#include <dal/longan/dal_longan_sds.h>
#include <dal/longan/dal_longan_port.h>
#include <dal/longan/dal_longan_stack.h>

/*
 * Symbol Definition
 */
#define PHY_PAGE_2              (2)
#define PHY_PAGE_3              (3)
#define PHY_PAGE_4              (4)

#define PHY_9300SDS_LINK_RETRY_CNT     3
#define LONGAN_PORT_STACK_DELAY     (10)

#undef SDS_FIELD_R
#undef SDS_FIELD_W
#define SDS_FIELD_R(_u, _s, _p, _r, _end, _start, _d) \
    dal_longan_sds_field_read(_u, _s, _p, _r, _end, _start, _d)
#define SDS_FIELD_W(_u, _s, _p, _r, _end, _start, _d) \
    dal_longan_sds_field_write(_u, _s, _p, _r, _end, _start, _d)

/*
 * Data Declaration
 */
static rtk_port_10gMedia_t  port10gMedia[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = {{0}};
static rtk_enable_t  portAdminEnable[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = {{0}};
static rtk_portmask_t port_ready[RTK_MAX_NUM_OF_UNIT];
static sds_mode_t sds_mode_g[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];

static rtk_enable_t portFiberRxEnable[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = {{0}};
static uint32 remoteFaultHandled[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = {{0}};

rt_phyInfo_t phy_9300sds_info =
{
    .phy_num    = PORT_NUM_IN_9300SDS,
    .eth_type   = HWP_XGE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};

/*
 * Function Declaration
 */
static int32 _phy_9300_serdes_init_10g_fiber(uint32 unit, uint8 sdsId);
static int32 _phy_9300_serdes_init_10g_dac(uint32 unit, uint8 sdsId);
static int32 _phy_9300_serdes_init_1g_fiber(uint32 unit, uint8 sdsId);
static int32 _phy_9300_serdes_init_100m_fiber(uint32 unit, uint8 sdsId);
static int32 _phy_9300_serdes_init_2_5g_fiber(uint32 unit, uint8 sdsId);
static int32 _phy_9300_speedStdReg_set(uint32 unit, rtk_port_t port, uint32 speed);
static int32 _phy_9300_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);
static int32 _phy_9300_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable, uint32 notify_status);

int32 phy_9300_fiberNwayForceLink_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern rtk_portmask_t stkPorts,stkPorts_get;


/* Function Name:
 *      phy_9300_media_get
 * Description:
 *      Get 9300 serdes PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. media type is PORT_MEDIA_FIBER
 */
int32
phy_9300_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    *pMedia = PORT_MEDIA_FIBER;

    return RT_ERR_OK;
} /* end of phy_9300_media_get */

/* Function Name:
 *      phy_9300_10gmedia_get
 * Description:
 *      Get 9300 10 serdes PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32
phy_9300_10gmedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia)
{
    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    *pMedia = port10gMedia[unit][port];

    return RT_ERR_OK;
} /* end of phy_9300_10gmedia_get */

/* Function Name:
 *      phy_9300_10gMedia_set
 * Description:
 *
 * Input:
 *      unit - unit id
 *      port - port id
 *      media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_9300_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    int    ret = RT_ERR_OK;
    uint32 speed;
    uint8  sdsId;

    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_10GMEDIA_END <= media), RT_ERR_INPUT);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    /* function body */
    port10gMedia[unit][port] = media;

    if(DISABLED == portAdminEnable[unit][port])
        return RT_ERR_OK;

#ifndef __BOOTLOADER__
    dal_longan_sds_10gEyeParam_load(unit, sdsId);
#endif

    if (media == PORT_10GMEDIA_FIBER_1G)
        speed = PORT_SPEED_1000M;
    else if (media == PORT_10GMEDIA_FIBER_100M)
        speed = PORT_SPEED_100M;
    else if (media == PORT_10GMEDIA_FIBER_2_5G)
        speed = PORT_SPEED_2_5G;
    else
        speed = PORT_SPEED_10G;
    _phy_9300_speedStdReg_set(unit, port, speed);

    RTK_PORTMASK_PORT_CLEAR(port_ready[unit], port);
#ifndef __BOOTLOADER__
    _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKDOWN,PORT_MEDIA_CHG);
#endif

    switch (media)
    {
        case PORT_10GMEDIA_NONE:
            ret =  dal_longan_sds_mode_set(unit, sdsId, RTK_MII_DISABLE);
            break;
        case PORT_10GMEDIA_FIBER_10G:
            ret = _phy_9300_serdes_init_10g_fiber(unit, sdsId);
            break;
        case PORT_10GMEDIA_DAC_50CM ... PORT_10GMEDIA_DAC_END:
            ret = _phy_9300_serdes_init_10g_dac(unit, sdsId);
            break;
        case PORT_10GMEDIA_FIBER_1G:
            ret = _phy_9300_serdes_init_1g_fiber(unit, sdsId);
            break;
        case PORT_10GMEDIA_FIBER_100M:
            ret = _phy_9300_serdes_init_100m_fiber(unit, sdsId);
            break;
        case PORT_10GMEDIA_FIBER_2_5G:
            ret = _phy_9300_serdes_init_2_5g_fiber(unit, sdsId);
            break;
        default:
            _dal_longan_port_macForceLink_set(unit, port, DISABLED, PORT_LINKDOWN,PORT_DEFAULT);
            return RT_ERR_INPUT;
    }

    osal_time_mdelay(50);

    if((media == PORT_10GMEDIA_FIBER_10G) \
        ||(PORT_10GMEDIA_FIBER_1G == media) \
        ||(PORT_10GMEDIA_FIBER_100M == media) \
        ||(PORT_10GMEDIA_FIBER_2_5G == media))
    {
        if (HAL_STACK_PORT(unit, port))
        {
            /* SS-822 */
            _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKUP,PORT_DEFAULT);
        }
        else
        {
            _dal_longan_port_macForceLink_set(unit, port, DISABLED, PORT_LINKDOWN,PORT_DEFAULT);
        }
    }

    return ret;
}   /* end of phy_9300_10gMedia_set */

/* Function Name:
 *      _phy_8295r_speedStdReg_set
 * Description:
 *      Configure standard register
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      speed   - speed (rtk_port_speed_t)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_9300_speedStdReg_set(uint32 unit, rtk_port_t port, uint32 speed)
{
    uint32  speedSel;
    uint32  data;
    int32    ret;
    uint32  sdsId;

    /* force speed */
    switch (speed)
    {
      case PORT_SPEED_10G:
        return RT_ERR_OK;
      case PORT_SPEED_1000M:
        speedSel = 0x2;
        break;
      case PORT_SPEED_100M:
        speedSel = 0x1;
        break;
      case PORT_SPEED_2_5G:
        speedSel = 0x1;
        break;
      default:
        return RT_ERR_INPUT;
    }/* switch */

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sel=%u", unit, port, speedSel);

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
        return ret;

    data = data & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    data = data | (((speedSel & 2) << (SpeedSelection1_OFFSET - 1)) | ((speedSel & 1) << SpeedSelection0_OFFSET));

    if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG, data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;

}

/* Function Name:
 *      phy_9300_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    /* function body */
    switch(port10gMedia[unit][port])
    {
        case PORT_10GMEDIA_FIBER_1G:
            *pSpeed = PORT_SPEED_1000M;
            break;
        case PORT_10GMEDIA_FIBER_100M:
            *pSpeed = PORT_SPEED_100M;
            break;
        case PORT_10GMEDIA_FIBER_2_5G:
            *pSpeed = PORT_SPEED_2_5G;
            break;
        case PORT_10GMEDIA_FIBER_10G:
        case PORT_10GMEDIA_DAC_50CM ... PORT_10GMEDIA_DAC_END:
            *pSpeed = PORT_SPEED_10G;
            break;
        default:
            *pSpeed = PORT_SPEED_10G;
    }

    return RT_ERR_OK;
} /* end of phy_9300_speed_get */

/* Function Name:
 *      phy_9300_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_9300_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32               ret = RT_ERR_OK;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    switch(port10gMedia[unit][port])
    {
        case PORT_10GMEDIA_FIBER_10G:
        case PORT_10GMEDIA_DAC_50CM ... PORT_10GMEDIA_DAC_END:
            if (PORT_SPEED_10G == speed)
                ret =  RT_ERR_OK;
            else
                ret =  RT_ERR_PORT_NOT_SUPPORTED;
            break;
        case PORT_10GMEDIA_FIBER_1G:
            if (PORT_SPEED_1000M == speed)
                ret =  _phy_9300_speedStdReg_set(unit, port, speed);
            else
                ret =  RT_ERR_PORT_NOT_SUPPORTED;
            break;
        case PORT_10GMEDIA_FIBER_100M:
            if (PORT_SPEED_100M == speed)
                ret =  _phy_9300_speedStdReg_set(unit, port, speed);
            else
                ret =  RT_ERR_PORT_NOT_SUPPORTED;
            break;
        case PORT_10GMEDIA_FIBER_2_5G:
            if (PORT_SPEED_2_5G == speed)
                ret =  RT_ERR_OK;
            else
                ret =  RT_ERR_PORT_NOT_SUPPORTED;
            break;
        case  PORT_10GMEDIA_NONE:
            ret = RT_ERR_OK;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
} /* end of phy_9300_speed_set */

/* Function Name:
 *      phy_9300_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    uint32 data,speed;
    uint32 sdsId;
    int32   ret;

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if ((ret = phy_9300_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        *pDuplex = PORT_FULL_DUPLEX;
        return RT_ERR_OK;
    }

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
        return ret;

    data  = REG32_FIELD_GET(data,DuplexMode_OFFSET,DuplexMode_MASK);

    if(1 == data)
        *pDuplex  = PORT_FULL_DUPLEX;
    else
        *pDuplex = PORT_HALF_DUPLEX;

    return RT_ERR_OK;
} /* end of phy_9300_duplex_get */

/* Function Name:
 *      phy_9300_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex)
{
    uint32 data,speed;
    uint32 sdsId;
    int32 ret;

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if ((ret = phy_9300_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        if (PORT_FULL_DUPLEX != duplex)
            return RT_ERR_FAILED;

        return RT_ERR_OK;
    }

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
        return ret;
    if (PORT_HALF_DUPLEX == duplex)
        data = REG32_FIELD_SET(data, 0, DuplexMode_OFFSET, DuplexMode_MASK);
    else
        data = REG32_FIELD_SET(data, 1, DuplexMode_OFFSET, DuplexMode_MASK);

    if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,data)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
} /* end of phy_9300_duplex_set */

/* Function Name:
 *      phy_9300_enable_set
 * Description:
 *      Set interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data;
    uint32 sdsId;
    int32  ret;

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_OK;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    /* 1G reg */
    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
        return ret;
    if (enable == ENABLED)
        data = REG32_FIELD_SET(data, 0, PowerDown_OFFSET, PowerDown_MASK);
    else
        data = REG32_FIELD_SET(data, 1, PowerDown_OFFSET, PowerDown_MASK);

    if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,data)) != RT_ERR_OK)
        return ret;

    /* 10G reg */
    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_4, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
        return ret;
    if (enable == ENABLED)
        data = REG32_FIELD_SET(data, 0, PowerDown_OFFSET, PowerDown_MASK);
    else
        data = REG32_FIELD_SET(data, 1, PowerDown_OFFSET, PowerDown_MASK);

    if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_4, PHY_CONTROL_REG,data)) != RT_ERR_OK)
        return ret;

    portAdminEnable[unit][port] = enable;

    if (enable == ENABLED)
        phy_9300_10gMedia_set(unit, port, port10gMedia[unit][port]);
    else
    {
        _dal_longan_port_macForceLink_set(unit,  port, ENABLED, PORT_LINKDOWN,PORT_DEFAULT);
        dal_longan_sds_mode_set(unit, sdsId, RTK_MII_DISABLE);
    }

    return RT_ERR_OK;
} /* end of phy_9300_enable_set */

/* Function Name:
 *      phy_9300_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable -   auto negotiation status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data,speed;
    uint32 sdsId;
    int32   ret;

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if ((ret = phy_9300_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
        return ret;

    *pEnable  = REG32_FIELD_GET(data, AutoNegotiationEnable_OFFSET, AutoNegotiationEnable_MASK);

    return RT_ERR_OK;
} /* end of phy_9300_autoNegoEnable_get */

/* Function Name:
 *      phy_9300_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data,speed;
    uint32 sdsId;
    int32 ret;

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if ((ret = phy_9300_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        if(ENABLED == enable)
            return RT_ERR_PORT_NOT_SUPPORTED;

        return RT_ERR_OK;
    }

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
        return ret;

    if (enable == ENABLED)
        data = REG32_FIELD_SET(data, 1, AutoNegotiationEnable_OFFSET, AutoNegotiationEnable_MASK);
    else
        data = REG32_FIELD_SET(data, 0, AutoNegotiationEnable_OFFSET, AutoNegotiationEnable_MASK);

    if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_9300_autoNegoEnable_set */

/* Function Name:
 *      phy_9300_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32 data,speed;
    uint32 sdsId;
    int32  ret;

    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if ((ret = phy_9300_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    sdsId = HWP_PORT_SDSID(unit, port);

    osal_memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));

    if (PORT_SPEED_10G == speed)
    {
        /* get value from CHIP*/
        if((ret = dal_longan_sds_reg_get(unit, sdsId, 0x1f, 11,&data)) != RT_ERR_OK)
            return ret;

        pAbility->FC = (REG32_FIELD_GET(data, 2, (0x3 << 2))) ? ENABLED : DISABLED;
        pAbility->AsyFC = (REG32_FIELD_GET(data, 2, (0x3 << 2))) ? ENABLED : DISABLED;

        return RT_ERR_OK;
    }

    /* get value from CHIP*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_AN_ADVERTISEMENT_REG,&data)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (data & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
    pAbility->AsyFC = (data & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
    pAbility->Half_10 = 0;
    pAbility->Full_10 = 0;
    pAbility->Half_100 = 0;
    pAbility->Full_100 = 0;
    pAbility->Half_1000 = (data & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
    pAbility->Full_1000 = (data & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;

    return RT_ERR_OK;
} /* end of phy_9300_autoNegoAbility_get */


/* Function Name:
 *      phy_9300_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_enable_t    enable;
    uint32          data,speed;
    uint32          sdsId;
    int32           ret;

    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if (!HWP_10GE_PORT(unit, port))
        return RT_ERR_PORT_ID;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if ((ret = phy_9300_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    sdsId = HWP_PORT_SDSID(unit, port);

    if (PORT_SPEED_10G == speed)
    {
        /* get value from CHIP*/
        if((ret = dal_longan_sds_reg_get(unit, sdsId, 0x1f, 11,&data)) != RT_ERR_OK)
            return ret;

        if(pAbility->FC == ENABLED)
            data = REG32_FIELD_SET(data, 0x3, 2, 0x3 <<2);
        else
            data = REG32_FIELD_SET(data, 0x0, 2, 0x3 <<2);

        if((ret = dal_longan_sds_reg_set(unit, sdsId, 0x1f, 11,data)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;
    }

    /* get value from CHIP*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_AN_ADVERTISEMENT_REG,&data)) != RT_ERR_OK)
        return ret;

    data = data & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
    data = data
            | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
            | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
    data = data & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
    data = data | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
            | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

    if ((ret = phy_9300_autoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
        return ret;

    if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_2, PHY_AN_ADVERTISEMENT_REG,data)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if(ENABLED == enable)
    {
        if((ret = dal_longan_sds_reg_get(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG,&data)) != RT_ERR_OK)
            return ret;
        data = REG32_FIELD_SET(data, 1, RestartAutoNegotiation_OFFSET, RestartAutoNegotiation_MASK);

        if((ret = dal_longan_sds_reg_set(unit, sdsId, PHY_PAGE_2, PHY_CONTROL_REG, data)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of phy_9300_autoNegoAbility_set */

/* Function Name:
 *      phy_9300_init
 * Description:
 *      Initialize 9300 MAC internal serdes PHY.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_init(uint32 unit, rtk_port_t port)
{
    int32 ret = RT_ERR_OK;
    rtk_enable_t enable = ENABLED;

    if(HWP_10GE_SERDES_PORT(unit, port))
    {
        RTK_PORTMASK_PORT_CLEAR(port_ready[unit], port);
    }

    RT_ERR_CHK(_phy_9300_fiberRxEnable_get(unit, port, &enable), ret);
    portFiberRxEnable[unit][port] = enable;

    RT_ERR_CHK(phy_9300_fiberNwayForceLink_set(unit, port, ENABLED), ret);

    return RT_ERR_OK;
} /* end of phy_9300_init */

/* Function Name:
 *      phy_9300_linkChange_process
 * Description:
 *      For 10G SerDes link change process
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts)
{
    /* parameter check */
    RT_PARAM_CHK(!HWP_SERDES_PORT(unit, port), RT_ERR_PORT_ID);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,linkSts=%s", unit, port, ((linkSts==PORT_LINKUP)?"UP":"DOWN"));

    if (linkSts == PORT_LINKDOWN)
    {
        /* link change down process */
        /* clear status */
        if((PORT_10GMEDIA_FIBER_1G != port10gMedia[unit][port])&&(PORT_10GMEDIA_FIBER_100M != port10gMedia[unit][port]) \
            && (PORT_10GMEDIA_FIBER_2_5G != port10gMedia[unit][port]))
        {
            if((PORT_10GMEDIA_FIBER_10G != port10gMedia[unit][port])||(HAL_STACK_PORT(unit, port)))
            {
                _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKDOWN,PORT_DEFAULT);
            }
            RTK_PORTMASK_PORT_CLEAR(port_ready[unit], port);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_serdes_init_1g_fiber
 * Description:
 *
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_9300_serdes_init_1g_fiber(uint32 unit, uint8 sdsId)
{
    int32  ret;

    /* serdes mode 1G Fiber */
    if((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_1000BX_FIBER)) != RT_ERR_OK)
        return ret;

    /* CFG_LINKDW_SEL */
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x6, 13, 6, 6, 1), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_serdes_init_10g_fiber
 * Description:
 *
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_9300_serdes_init_10g_fiber(uint32 unit, uint8 sdsId)
{
    int32  ret;

    /* serdes mode 10G Base-R */
    if((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_10GR)) != RT_ERR_OK)
        return ret;

    /* CFG_LINKDW_SEL */
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x6, 13, 6, 6, 1), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_serdes_init_10g_dac
 * Description:
 *
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_9300_serdes_init_10g_dac(uint32 unit, uint8 sdsId)
{
    int32  ret;

    /* serdes mode 10G Base-R */
    if((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_10GR)) != RT_ERR_OK)
        return ret;

    /* CFG_LINKDW_SEL */
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x6, 13, 6, 6, 0), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_serdes_init_100m_fiber
 * Description:
 *
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_9300_serdes_init_100m_fiber(uint32 unit, uint8 sdsId)
{
    int32  ret;

    /* serdes mode 10G Base-R */
    if((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_100BX_FIBER)) != RT_ERR_OK)
        return ret;

    /* CFG_LINKDW_SEL */
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x6, 13, 6, 6, 1), ret);

    return RT_ERR_OK;
}
/* Function Name:
 *      _phy_9300_serdes_init_2_5g_fiber
 * Description:
 *
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_9300_serdes_init_2_5g_fiber(uint32 unit, uint8 sdsId)
{
    int32  ret;

    /* serdes mode 10G Base-R */
    if((ret = dal_longan_sds_mode_set(unit, sdsId, RTK_MII_2500Base_X)) != RT_ERR_OK)
        return ret;

    /* CFG_LINKDW_SEL */
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x6, 13, 6, 6, 1), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_serdes_loopback_get
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable           -loopback mode status;
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_9300_serdes_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32      data, val;
    int32       ret;
    uint32      sdsId;

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, 0, 0, &data)) != RT_ERR_OK)
        return ret;

    *pEnable = (REG32_FIELD_GET(data, 4, (0x1 << 4))) ? ENABLED : DISABLED; /* SP_CFG_DIG_LPK */

    /* verbose check on the other reg */
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 6, 1, &data)) != RT_ERR_OK)
        return ret;

    val = REG32_FIELD_GET(data, 2, (0x1 << 2)); /* FP_TGR1_CFG_AFE_LPK */
    if ( ((val == 0) && (*pEnable != DISABLED))
        ||((val != 0) && (*pEnable != ENABLED)))
    {
        RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,loopback: regs inconsisten!", unit, port);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_serdes_loopback_set
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_9300_serdes_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rt_serdesMode_t mode;
    uint32  data,val;
    int32   ret;
    uint32  sdsId;

    val = (ENABLED == enable) ? 1 : 0;
    sdsId = HWP_PORT_SDSID(unit, port);

    /*Fiber1G*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 0, 0, &data)) != RT_ERR_OK)
        return ret;

    data = REG32_FIELD_SET(data, val, 4, (0x1 << 4));   /* SP_CFG_DIG_LPK */
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 0, 0, data)) != RT_ERR_OK)
        return ret;

    /*10GR*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 6, 0x1 , &data)) != RT_ERR_OK)
        return ret;

    data = REG32_FIELD_SET(data, val, 2, (0x1 << 2));   /* FP_TGR1_CFG_AFE_LPK */
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 6, 0x1 , data)) != RT_ERR_OK)
        return ret;

    dal_longan_sds_mode_get( unit, HWP_PORT_SDSID(unit, port), &mode);
    dal_longan_sds_mode_set( unit, HWP_PORT_SDSID(unit, port), mode);
    osal_time_mdelay(1);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_serdesFiber10g_handeler
 * Description:
 *      Handle serdes fiber 10G and reset action
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
int32
_phy_9300_serdesFiber10g_handeler(uint32 unit, uint32 port, uint32 *pStatus)
{
    rtk_sds_linkSts_t sdsLink0,sdsLink1, sdsLink2;
    uint32  sdsId,rxIdle;

    sdsId = HWP_PORT_SDSID(unit, port);

    *pStatus = PHY_FIBER_RX_STATUS_0;

    if(RTK_PORTMASK_IS_PORT_SET(port_ready[unit], port))
        return RT_ERR_OK;

    dal_longan_sds_10gRxIdle_get(unit, sdsId, &rxIdle);
    if (rxIdle)
    {
        return RT_ERR_OK;
    }

    if(PORT_10GMEDIA_FIBER_10G == port10gMedia[unit][port])
    {
        dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink0);
        dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink1);
        dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink2);
        if ((sdsLink1.latch_sts == 1) && (sdsLink2.latch_sts == 1))
        {
            /* SS-822 */
            if(HAL_STACK_PORT(unit, port))
            {
                _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKUP,PORT_DEFAULT);
            }
            RTK_PORTMASK_PORT_SET(port_ready[unit], port);
            return RT_ERR_OK;
        }
    }

    *pStatus = PHY_FIBER_RX_STATUS_1;

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_serdesFiber1g_handeler
 * Description:
 *      Handle serdes fiber 1G and reset action
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
int32
_phy_9300_serdesFiber1g_handeler(uint32 unit, uint32 port, uint32 *pStatus)
{
    uint32  chk_loop, chk_error;
    uint32  reg_data;
    uint32  sdsId;
    int32 ret;

    chk_error = 0;
    *pStatus = PHY_FIBER_RX_STATUS_0;

    sdsId = HWP_PORT_SDSID(unit, port);

    /*chanel 0*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 1, 24, &reg_data)) != RT_ERR_OK)
        return ret;

    reg_data &= (~0x7);
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 1, 24, reg_data)) != RT_ERR_OK)
        return ret;
    /* clear counter*/
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 1, 2, 0)) != RT_ERR_OK)
        return ret;

    for(chk_loop = 0; chk_loop < 3; ++chk_loop)
    {
        /*check error counter*/
        if((ret = dal_longan_sds_reg_get(unit, sdsId, 1, 2, &reg_data)) != RT_ERR_OK)
            return ret;

        if (0xff <= reg_data)
        {
            chk_error++;
        }
        osal_time_usleep(200);
    }

    if (chk_error >= 2)
    {
        *pStatus = PHY_FIBER_RX_STATUS_1;
    }
    else
    {
        chk_error = 0;

        for(chk_loop = 0; chk_loop < 3; ++chk_loop)
        {
            if((ret = dal_longan_sds_reg_get(unit, sdsId, 1, 29, &reg_data)) != RT_ERR_OK)
                return ret;

             if (0x100 == (reg_data & 0x100))
             {
                if (0x10 != (reg_data & 0x10))
                {
                    ++chk_error;
                }
            }
        }

        if (3 == chk_error)
        {
            *pStatus = PHY_FIBER_RX_STATUS_2;
        }
    }

    return RT_ERR_OK;
}

static int32
_phy_9300_fiberSymErr_check(uint32 unit, uint32 port, uint32 *pErr)
{
    uint32 sdsId;
    uint32 ret;
    rt_serdesMode_t mode;
    rtk_sds_symErr_t symErr0,symErr1;

    sdsId = HWP_PORT_SDSID(unit, port);

    *pErr = 0;

    dal_longan_sds_symErr_clear(unit, sdsId);
    dal_longan_sds_symErr_clear(unit, sdsId);

    dal_longan_sds_symErr_get(unit, sdsId, &symErr0);
    osal_time_mdelay(1);
    dal_longan_sds_symErr_get(unit, sdsId, &symErr1);

    if((ret = dal_longan_sds_mode_get(unit, sdsId, &mode))!= RT_ERR_OK)
        return ret;

    if (RTK_MII_1000BX_FIBER == mode)
    {
        if((symErr1.ch[0] - symErr0.ch[0] > 100)
                || (symErr1.ch[1] - symErr0.ch[1] > 100)
                || (symErr1.ch[0] >= 0xffff00)
                || (symErr1.ch[1] >= 0xffff00))
        {
            *pErr = 1;
        }
    }
    else if(RTK_MII_10GR == mode)
    {
        if(symErr1.blk_err > 0)
        {
            *pErr = 1;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_serdesFiberRx_check
 * Description:
 *      Handle serdes fiber 1G
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
int32
phy_9300_serdesFiberRx_check(uint32 unit, uint32 port, uint32 *pStatus)
{
    dal_longan_sds_rxCaliProcessSts_t sts;
    uint32 isErr = 0;
    int32  ret;

    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    if(portAdminEnable[unit][port] == DISABLED)
        return RT_ERR_OK;

    dal_longan_rxCaliProcessSts_get(unit, HWP_PORT_SDSID(unit, port), &sts);
    if(sts  != DAL_LONGAN_SDS_RXCALI_PROCESS_STS_IDLE)
    {
        return RT_ERR_OK;
    }

    switch(port10gMedia[unit][port])
    {
        case PORT_10GMEDIA_FIBER_1G:
        case PORT_10GMEDIA_FIBER_100M:
        case PORT_10GMEDIA_FIBER_2_5G:
            ret = _phy_9300_serdesFiber1g_handeler(unit, port, pStatus);
            break;
        case PORT_10GMEDIA_FIBER_10G:
        case PORT_10GMEDIA_DAC_50CM ... PORT_10GMEDIA_DAC_END:
            ret = _phy_9300_serdesFiber10g_handeler(unit, port, pStatus);
            break;
        default:
            return RT_ERR_OK;
    }

    if((ret = _phy_9300_fiberSymErr_check(unit, port, &isErr)) != RT_ERR_OK)
        return ret;

    if((isErr == 0) && (port10gMedia[unit][port] == PORT_10GMEDIA_FIBER_10G
                || port10gMedia[unit][port] == PORT_10GMEDIA_FIBER_1G))
    {
        *pStatus = PHY_FIBER_RX_STATUS_0;
    }

    return ret;
}

/* Function Name:
 *      _phy_9300_serdesFiber1g_reset
 * Description:
 *      Handle serdes fiber 1G
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
 int32
_phy_9300_serdesFiber1g_reset(uint32 unit, uint32 port)
{

    uint32 value;
    uint32 reg_data;
    int32   ret;
    uint32  sdsId;

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, 0, 0, &reg_data)) != RT_ERR_OK)
        return ret;

    value = reg_data & (~0x3);
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 0, 0, value)) != RT_ERR_OK)
        return ret;

    value |= 0x3;
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 0, 0, value)) != RT_ERR_OK)
        return ret;

    if((ret = dal_longan_sds_reg_set(unit, sdsId, 0, 0, reg_data)) != RT_ERR_OK)
        return ret;

    return  RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_serdesFiberRx_reset
 * Description:
 *      Handle serdes fiber 1G
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
int32
phy_9300_serdesFiberRx_reset(uint32 unit, uint32 port)
{
    dal_longan_sds_rxCaliProcessSts_t sts;
    rtk_sds_linkSts_t sdsLink;
    uint32 sdsId;
    uint32 rxCaliFlag = 0;
    int32  ret = RT_ERR_OK;

    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    dal_longan_rxCaliProcessSts_get(unit, sdsId, &sts);
    if(sts == DAL_LONGAN_SDS_RXCALI_PROCESS_STS_PROCESS)
    {
        return RT_ERR_OK;
    }

    switch(port10gMedia[unit][port])
    {
        case PORT_10GMEDIA_FIBER_1G:
        case PORT_10GMEDIA_FIBER_100M:
        case PORT_10GMEDIA_FIBER_2_5G:
            ret = _phy_9300_serdesFiber1g_reset(unit, port);
            break;
        case PORT_10GMEDIA_FIBER_10G:
            ret = dal_longan_sds_rx_rst(unit, sdsId);
            osal_time_usleep(1000*100);
            dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink);
            dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink);
            if(sdsLink.latch_sts == 1)
            {
                /* SS-822 */
                if(HAL_STACK_PORT(unit, port))
                {
                    _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKUP,PORT_DEFAULT);
                }
                RTK_PORTMASK_PORT_SET(port_ready[unit], port);
            }
            break;
        case PORT_10GMEDIA_DAC_50CM ... PORT_10GMEDIA_DAC_END:
            rxCaliFlag = 1;
            break;
        default:
            return ret;
    }

    if(rxCaliFlag == 1)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d dac rxCali", unit, port);
        /* the signal may be too weak to link up, do rx-cali here to try to link up the port */
        dal_longan_rxCaliProcessSts_set(unit, sdsId, DAL_LONGAN_SDS_RXCALI_PROCESS_STS_PROCESS);
        ret = dal_longan_sds_rxCali(unit, sdsId, PHY_9300SDS_LINK_RETRY_CNT);
        /* link check */
        {
            osal_time_usleep(1000*100);
            dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink);
            dal_longan_sds_linkSts_get(unit, sdsId, &sdsLink);
            if (sdsLink.latch_sts)
            {
                RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d Link up after cali", unit, port);
                /* mark the port as rx-cali is done.
                 * In this condition, the port will not need to process rx-cali again in link-up process.
                 */
                if (HAL_STACK_PORT(unit, port))
                {
                    /* SS-822 force linkup stack port*/
                    _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKUP,PORT_DEFAULT);
                }
                else
                {
                    _dal_longan_port_macForceLink_set(unit, port, DISABLED, PORT_LINKDOWN,PORT_DEFAULT);
                }

                RTK_PORTMASK_PORT_SET(port_ready[unit], port);
            }
        }
        dal_longan_rxCaliProcessSts_set(unit, sdsId, DAL_LONGAN_SDS_RXCALI_PROCESS_STS_IDLE);
    }
    return ret;
}

/* Function Name:
 *      phy_9300_polar_get
 * Description:
 *      Get port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_9300_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    uint32      data, val;
    int32       ret;
    uint32      sdsId;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    osal_memset(pPolarCtrl, 0, sizeof(rtk_port_phyPolarCtrl_t));

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = dal_longan_sds_reg_get(unit, sdsId, 6, 2, &data)) != RT_ERR_OK)
        return ret;

    pPolarCtrl->phy_polar_rx = (REG32_FIELD_GET(data, 13, (0x1 << 13))) ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL; /* FP_TGR2_CFG_INV_HSI */
    pPolarCtrl->phy_polar_tx = (REG32_FIELD_GET(data, 14, (0x1 << 14))) ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL; /* FP_TGR2_CFG_INV_HSO */

    /* verbose check on the other reg */
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 0, 0, &data)) != RT_ERR_OK)
        return ret;

    val = REG32_FIELD_GET(data, 9, (0x1 << 9)); /* SP_INV_HSI */
    if (  ((val == 0) && (pPolarCtrl->phy_polar_rx != PHY_POLARITY_NORMAL))
        ||((val != 0) && (pPolarCtrl->phy_polar_rx != PHY_POLARITY_INVERSE)))
    {
        RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,polar: rx regs inconsisten!", unit, port);
    }

    val = REG32_FIELD_GET(data, 8, (0x1 << 8)); /* SP_INV_HSO */
    if (  ((val == 0) && (pPolarCtrl->phy_polar_tx != PHY_POLARITY_NORMAL))
        ||((val != 0) && (pPolarCtrl->phy_polar_tx != PHY_POLARITY_INVERSE)))
    {
        RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,polar: tx regs inconsisten!", unit, port);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_polar_set
 * Description:
 *      Configure 10GE port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_9300_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    uint32      data, rxVal, txVal;
    int32       ret;
    uint32      sdsId;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    rxVal = (pPolarCtrl->phy_polar_rx == PHY_POLARITY_INVERSE) ? 1 : 0;
    txVal = (pPolarCtrl->phy_polar_tx == PHY_POLARITY_INVERSE) ? 1 : 0;

    sdsId = HWP_PORT_SDSID(unit, port);

    /*10GR*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 6, 2, &data)) != RT_ERR_OK)
        return ret;

    data = REG32_FIELD_SET(data, rxVal, 13, (0x1 << 13));   /* FP_TGR2_CFG_INV_HSI */
    data = REG32_FIELD_SET(data, txVal, 14, (0x1 << 14));   /* FP_TGR2_CFG_INV_HSO */
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 6, 2, data)) != RT_ERR_OK)
        return ret;

    /*Fiber1G*/
    if((ret = dal_longan_sds_reg_get(unit, sdsId, 0, 0, &data)) != RT_ERR_OK)
        return ret;

    data = REG32_FIELD_SET(data, rxVal, 9, (0x1 << 9));   /* SP_INV_HSI */
    data = REG32_FIELD_SET(data, txVal, 8, (0x1 << 8));   /* SP_INV_HSO */
    if((ret = dal_longan_sds_reg_set(unit, sdsId, 0, 0, data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port from register
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
static int32
_phy_9300_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 sdsId;
    uint32 val;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    SDS_FIELD_R(unit, sdsId, 0x20, 2, 12, 12, &val);
    if (val == 1)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_9300_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    *pEnable = portFiberRxEnable[unit][port];

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_9300_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable register of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 *      notify_status - notify rx ready or not, only available when rx disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
static int32
_phy_9300_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable, uint32 notify_status)
{
    uint32 sdsId;
    uint32 lane0Sds;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);
    lane0Sds = (sdsId % 2) ? (sdsId - 1) : (sdsId);

    if (enable == DISABLED)
    {
        SDS_FIELD_W(unit, sdsId, 0x2e, 21, 4, 4, 1);
        SDS_FIELD_W(unit, sdsId, 0x28, 21, 4, 4, 1);
        SDS_FIELD_W(unit, sdsId, 0x24, 21, 4, 4, 1);

        if (notify_status == 1)
        {
            /* SS-1427 */
            SDS_FIELD_W(unit, lane0Sds, 0x20, 2, 13, 13, 0x1);
            SDS_FIELD_W(unit, sdsId, 0x20, 2, 13, 12, 0x3);
        }
        else
        {
            SDS_FIELD_W(unit, lane0Sds, 0x20, 2, 13, 13, 0x0);
            SDS_FIELD_W(unit, sdsId, 0x20, 2, 13, 12, 0x1);
        }

        SDS_FIELD_W(unit, sdsId, 0x2e, 21, 4, 4, 0);
        SDS_FIELD_W(unit, sdsId, 0x28, 21, 4, 4, 0);
        SDS_FIELD_W(unit, sdsId, 0x24, 21, 4, 4, 0);
    }
    else
    {
        /* SS-1427 */
        SDS_FIELD_W(unit, lane0Sds, 0x20, 2, 13, 13, 0x0);
        SDS_FIELD_W(unit, sdsId, 0x20, 2, 13, 12, 0x1);

        SDS_FIELD_W(unit, sdsId, 0x20, 2, 13, 12, 0x2);
        SDS_FIELD_W(unit, lane0Sds, 0x20, 2, 13, 13, 0x1);
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      phy_9300_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_9300_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    /* function body */
    portFiberRxEnable[unit][port] = enable;

    if(1 == remoteFaultHandled[unit][port])
        return RT_ERR_OK;

    RT_ERR_CHK(_phy_9300_fiberRxEnable_set(unit, port, enable, 0), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_rtl9300_sds_get
 * Description:
 *      Get PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
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
phy_rtl9300_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    uint32  sds;

    sds = HWP_PORT_SDSID(unit, port);

    sdsCfg->sdsMode  = sds_mode_g[unit][sds].sdsMode;

    return RT_ERR_OK;
}   /* end of phy_rtl9300_sds_get */

/* Function Name:
 *      phy_rtl9300_sds_set
 * Description:
 *      Set PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
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
phy_rtl9300_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    rtk_port_10gMedia_t media;
    uint32              sds;
    int32               ret;
    uint32              sds_off,sds_mode;
    reg_field_t sds_mode_sel[] =
    {
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS0_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS1_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS2_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS3_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS4_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS5_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS6_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS7_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS8_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS9_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS10_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS11_MODE_SELf},
    };


    media = port10gMedia[unit][port];
    sds = HWP_PORT_SDSID(unit, port);

    if (PORT_10GMEDIA_FIBER_1G == media)
    {
        switch (sdsCfg->sdsMode)
        {
            case RTK_MII_SGMII:
                /*Force mode disable*/
                SDS_FIELD_W(unit, sds, 0x1f, 9, 6, 6, 0);
                SDS_FIELD_W(unit, sds, 0x1f, 9, 11, 7, 0x1f);

                sds_mode = 0x2;
                if ((ret = reg_field_write(unit, sds_mode_sel[sds].reg, sds_mode_sel[sds].field, &sds_mode)) != RT_ERR_OK)
                    return ret;
                sds_mode_g[unit][sds].sdsMode = RTK_MII_SGMII;
                break;
            case RTK_MII_1000BX_FIBER:
                sds_off = 0x1f;
                if ((ret = reg_field_write(unit, sds_mode_sel[sds].reg, sds_mode_sel[sds].field, &sds_off)) != RT_ERR_OK)
                    return ret;

                sds_mode = 0x4;
                /*Force mode enable*/
                SDS_FIELD_W(unit, sds, 0x1f, 9, 6, 6, 1);
                SDS_FIELD_W(unit, sds, 0x1f, 9, 11, 7, sds_mode);
                sds_mode_g[unit][sds].sdsMode = RTK_MII_1000BX_FIBER;

                break;
            default:
                return RT_ERR_FAILED;
        }

    }
    else
    {
        return RT_ERR_FAILED;
    }


    return ret;
}   /* end of phy_rtl9300_sds_set */

/* Function Name:
 *      phy_9300_fiberNwayForceLink_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_fiberNwayForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32 sdsId;
    uint32 data;
    int32  ret = RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    data = (enable == ENABLED ? 1:0);
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0, 4, 2, 2, data), ret);

    return RT_ERR_OK;
}
/* Function Name:
 *      phy_9300_fiberNwayForceLink_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_9300_fiberNwayForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t* enable)
{
    uint32 sdsId;
    uint32 data;
    int32  ret = RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    RT_ERR_CHK(SDS_FIELD_R(unit, sdsId, 0, 4, 2, 2, &data), ret);

    *enable = (data == 0x1 ? ENABLED:DISABLED);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_9300_remoteFault_handle
 * Description:
 *      Handle fiber remote fault
 * Input:
 *      unit      - unit id
 *      port      - port id
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
phy_9300_remoteFault_handle(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_data;
    uint32  sdsId = 0;
    uint32  rxIdle = 0;
    uint32  berNotify = 0;
    uint32  isUniDir = 0;
    uint32  isFiber = 0;
    rtk_port_10gMedia_t media = PORT_10GMEDIA_NONE;
    rtk_enable_t sdsLoopBack = DISABLED;

    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    sdsId = HWP_PORT_SDSID(unit, port);

    RT_ERR_CHK(phy_9300_10gmedia_get(unit, port, &media), ret);

    if(media == PORT_10GMEDIA_FIBER_10G)
    {
        isFiber = TRUE;

        RT_ERR_CHK(dal_longan_sds_fiberUnidirEnable_get(unit, sdsId, &isUniDir), ret);

        RT_ERR_CHK(dal_longan_sds_10gRxIdle_get(unit, sdsId, &rxIdle), ret);
        RT_ERR_CHK(dal_longan_sds_10gBerNotify_get(unit, sdsId, &berNotify), ret);

        RT_ERR_CHK(phy_9300_serdes_loopback_get(unit, port, &sdsLoopBack), ret);

        RT_ERR_CHK(SDS_FIELD_R(unit, sdsId, 0x2, 0x0, Loopback_OFFSET,
                Loopback_OFFSET, &reg_data), ret);

        if(reg_data == 1)
            sdsLoopBack = ENABLED;
    }
    else
    {
        isFiber = FALSE;
    }

    if(((rxIdle == 1 || berNotify == 0) && 
        (!isUniDir) && (isFiber) && (!sdsLoopBack)) &&
        (!remoteFaultHandled[unit][port]))
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_SDS), "Lock port %d isUni %d rxIdle %d berNotify %d handled %d", \
                port, isUniDir, rxIdle, berNotify, remoteFaultHandled[unit][port]);

        remoteFaultHandled[unit][port] = 1;

        RT_ERR_CHK(rtl9300_miim_pollingEnable_set(unit, port, DISABLED), ret);

        //force EQ_EN=0, DFE_EN=0
        RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x20, 0x3, 5, 2, 5), ret);

        //force BER_NOTIFY=1
        RT_ERR_CHK(_phy_9300_fiberRxEnable_set(unit, port, DISABLED, 1), ret);
    }
    else if(((rxIdle == 0 && berNotify == 1) ||
        (isUniDir ) || (!isFiber) || sdsLoopBack) &&
        (remoteFaultHandled[unit][port]))
    {
        //release BER_NOTIFY
        if(portFiberRxEnable[unit][port] == ENABLED)
        {
            RT_ERR_CHK(_phy_9300_fiberRxEnable_set(unit, port, ENABLED, 0), ret);
        }

        //release EQ_EN, DFE_EN
        RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x20, 0x3, 5, 2, 0), ret);

        RT_LOG(LOG_INFO, (MOD_HAL|MOD_SDS), "Release port %d isUni %d rxIdle %d berNotify %d handled %d", \
                port, isUniDir, rxIdle, berNotify, remoteFaultHandled[unit][port]);
        osal_time_mdelay(50);

        remoteFaultHandled[unit][port] = 0;

        RT_ERR_CHK(rtl9300_miim_pollingEnable_set(unit, port, ENABLED), ret);
    }

    return ret;
}   /* end of phy_9300_remoteFault_handle */

/* Function Name:
 *      phy_9300_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable         - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_9300_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    uint32  sdsId;
    uint32  val = 0;

    sdsId = HWP_PORT_SDSID(unit, port);

    RT_ERR_CHK(dal_longan_sds_fiberUnidirEnable_set(unit, sdsId, enable), ret);

    if (ENABLED == enable)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    //CFG_LINK_FAULT_LOCAL_OFF, for 10G
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x6, 29, 7, 7, val), ret);

    //FP_CFG_FIB_FRCTX Unidirectional enable, for 1G
    RT_ERR_CHK(SDS_FIELD_W(unit, sdsId, 0x2, 0, 5, 5, val), ret);

    RT_ERR_CHK(phy_9300_remoteFault_handle(unit, port), ret);

    return ret;
}   /* end of phy_9300_fiberUnidirEnable_set */

/* Function Name:
 *      phy_9300_serdes_mapperInit
 * Description:
 *      Initialize PHY 9300 serdes driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_9300_serdes_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL9300_SERDES;
    pPhydrv->fPhydrv_init = phy_9300_init;
    pPhydrv->fPhydrv_media_get = phy_9300_media_get;
    pPhydrv->fPhydrv_10gMedia_get = phy_9300_10gmedia_get;
    pPhydrv->fPhydrv_10gMedia_set = phy_9300_10gMedia_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_9300_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_9300_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_9300_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_9300_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_9300_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_9300_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_9300_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_9300_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_9300_enable_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_serdesFiberRx_check = phy_9300_serdesFiberRx_check;
    pPhydrv->fPhydrv_serdesFiberRx_reset = phy_9300_serdesFiberRx_reset;
    pPhydrv->fPhydrv_loopback_get = phy_9300_serdes_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_9300_serdes_loopback_set;
    pPhydrv->fPhydrv_polar_get = phy_9300_polar_get;
    pPhydrv->fPhydrv_polar_set = phy_9300_polar_set;
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_9300_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_9300_fiberRxEnable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_9300_fiberNwayForceLink_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_9300_fiberNwayForceLink_get;

    pPhydrv->fPhydrv_fiberUnidirEnable_set = phy_9300_fiberUnidirEnable_set;
    pPhydrv->fPhydrv_remoteFault_handle = phy_9300_remoteFault_handle;

    pPhydrv->fPhydrv_sds_get = phy_rtl9300_sds_get;
    pPhydrv->fPhydrv_sds_set = phy_rtl9300_sds_set;

} /* end of phy_9300_serdes_mapperInit*/

