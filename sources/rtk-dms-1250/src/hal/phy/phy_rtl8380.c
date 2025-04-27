/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : PHY 8380 intra serdes Driver APIs.
 *
 * Feature : PHY 8380 intra serdes Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8380.h>

#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>


#include <hal/mac/reg.h>
#include <hal/mac/serdes.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>

#include <ioal/mem32.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


rt_phyInfo_t phy_8380sds_info =
{
    .phy_num    = PORT_NUM_IN_8380SDS,
    .eth_type   = HWP_GE,
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

int32 phy_8380_fiberLinkUp_handler1(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8380_serdes_ge_mapperInit
 * Description:
 *      Initialize PHY 8380 serdes driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8380_serdes_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8380_SERDES_GE;
    pPhydrv->fPhydrv_init = phy_8380_init;
    pPhydrv->fPhydrv_enable_set = phy_8380_enable_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8380_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8380_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8380_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8380_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8380_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8380_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8380_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8380_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8380_speed_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8380_gigaLiteEnable_get;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8380_fiberOAMLoopBack_set;
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
    pPhydrv->fPhydrv_serdesFiberRx_check = phy_8380_serdesFiberRx_check;
    pPhydrv->fPhydrv_serdesFiberRx_reset = phy_8380_serdesFiberRx_reset;
    pPhydrv->fPhydrv_loopback_get = phy_8380_serdes_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8380_serdes_loopback_set;
    pPhydrv->fPhydrv_sds_get = phy_8380_sds_get;
    pPhydrv->fPhydrv_sds_set = phy_8380_sds_set;
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8380_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8380_fiberRxEnable_set;
    pPhydrv->fPhydrv_polar_get = phy_8380_polar_get;
    pPhydrv->fPhydrv_polar_set = phy_8380_polar_set;

    pPhydrv->fPhydrv_fiberLinkUp_handler1 = phy_8380_fiberLinkUp_handler1;
#endif
} /* end of phy_8380_serdes_ge_mapperInit*/

/* Function Name:
 *      phy_8380_reset
 * Description:
 *      Reset serdes
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
static int32
_phy_8380_reset(uint32 unit, rtk_port_t port)
{
    uint32 serdes_reg1;
    uint32 serdes_reg2;

    uint32 serdes_val;

    /*Serdes Reset*/
    if (24 == port)
    {
        serdes_reg1 = MAPLE_SDS4_REG0r;
        serdes_reg2 = MAPLE_SDS4_REG3r;
    }
    else  if (26 == port)
    {
        serdes_reg1 = MAPLE_SDS5_REG0r;
        serdes_reg2 = MAPLE_SDS5_REG3r;
    }
    else
     return RT_ERR_PORT_ID;

     /*Analog Reset*/
    serdes_val = 0;
    reg_field_write(unit, serdes_reg1, MAPLE_SDS_EN_RXf, &serdes_val);
    reg_field_write(unit, serdes_reg1, MAPLE_SDS_EN_TXf, &serdes_val);

    serdes_val = 1;
    reg_field_write(unit, serdes_reg1, MAPLE_SDS_EN_RXf, &serdes_val);
    reg_field_write(unit, serdes_reg1, MAPLE_SDS_EN_TXf, &serdes_val);

    /*Digital Reset*/
    serdes_val = 1;
    reg_field_write(unit, serdes_reg2, MAPLE_SOFT_RSTf, &serdes_val);

    serdes_val = 0;
    reg_field_write(unit, serdes_reg2, MAPLE_SOFT_RSTf, &serdes_val);

    return RT_ERR_OK;
}   /* end of _phy_8380_reset */



/* Function Name:
 *      phy_8380_init
 * Description:
 *      Initialize 8380 MAC internal serdes PHY.
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
phy_8380_init(uint32 unit, rtk_port_t port)
{
    uint32  reg0,reg1;
    uint32 val;
    int32   ret;

    if (24 == port)
    {
        reg0 = MAPLE_SDS4_REG4r;
        reg1 = MAPLE_SDS4_FIB_EXT_REG16r;
    }
    else if (26 == port)
    {
        reg0 = MAPLE_SDS5_REG4r;
        reg1 = MAPLE_SDS5_FIB_EXT_REG16r;
    }
    else
        return RT_ERR_PORT_ID;

    /* auto-nego state can link with link partner is force mode */
    val = 1;
    if ((ret = reg_field_write(unit, reg0, MAPLE_CFG_EN_LINK_FIB1Gf, &val)) != RT_ERR_OK)
        return ret;

    /* disable EEE */
    val = 0;
    if ((ret = reg_field_write(unit, reg1, MAPLE_EEE_RSG_FIB1Gf, &val)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_field_write(unit, reg1, MAPLE_EEE_STD_FIB1Gf, &val)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_field_write(unit, reg1, MAPLE_C1_PWRSAV_EN_FIB1Gf, &val)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_field_write(unit, reg1, MAPLE_C2_PWRSAV_EN_FIB1Gf, &val)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_field_write(unit, reg1, MAPLE_EEE_QUIET_FIB1Gf, &val)) != RT_ERR_OK)
        return ret;

    /*serdes nway*/
    val = 0;
    if ((ret = reg_field_write(unit, MAPLE_SMI_GLB_CTRLr, MAPLE_EX_PHY_MAN_24_27f, &val)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8380_init */

/* Function Name:
 *      phy_8380_enable_set
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
phy_8380_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 serdes_reg0;
    uint32 serdes_mode;

    uint32 serdes_reg1;
    uint32 serdes_pdwn;

    uint32 serdes_reg2 = 0;
    uint32 reg_val = 0;
    uint32 field_val = 0;

    /*Check PortId*/
    if ((24 != port) && (26 != port))
        return RT_ERR_PORT_ID;

     /*Serdes Mode*/
     serdes_reg0 = MAPLE_SDS_MODE_SELr;

      if (DISABLED == enable)
      {
        serdes_mode = 0x9;
      }
      else
      {
        serdes_reg2 = MAPLE_INT_MODE_CTRLr;
        reg_read(unit, serdes_reg2,&reg_val);
        if(24 == port)
        {
            reg_field_get(unit, serdes_reg2, MAPLE_SDS4_INTF_MODE_2_0f, &field_val, &reg_val);
        }else
        {
            reg_field_get(unit, serdes_reg2, MAPLE_SDS5_INTF_MODE_2_0f, &field_val, &reg_val);
        }
        serdes_mode = (0x2 == field_val) ? 0x2 : 0x4;
      }

      if (24 == port)
             reg_field_write(unit, serdes_reg0, MAPLE_SDS4_MODE_SELf, &serdes_mode);
      else
             reg_field_write(unit, serdes_reg0, MAPLE_SDS5_MODE_SELf, &serdes_mode);

      /*Always Power Up because u-boot may close it*/
      if (24 == port)
       serdes_reg1 = MAPLE_SDS4_FIB_REG0r;
      else
       serdes_reg1 = MAPLE_SDS5_FIB_REG0r;

      serdes_pdwn = 0x0;
      reg_field_write(unit, serdes_reg1, MAPLE_CFG_FIB_PDOWNf, &serdes_pdwn);

    _phy_8380_reset(unit, port);

    return RT_ERR_OK;
} /* end of phy_8380_enable_set */


#if !defined(__BOOTLOADER__)
/* Function Name:
 *      phy_8380_media_get
 * Description:
 *      Get 8380 serdes PHY media type.
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
phy_8380_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    *pMedia = PORT_MEDIA_FIBER;
    return RT_ERR_OK;
} /* end of phy_8380_media_get */

/* Function Name:
 *      phy_8380_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8380 is not supported the per-port Giga Lite feature.
 */
int32
phy_8380_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = DISABLED;
    return RT_ERR_OK;
} /* end of phy_8380_gigaLiteEnable_get */

/* Function Name:
 *      phy_8380_speed_get
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
phy_8380_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32 ret;
    uint32 value = 0;

    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);

    if(24 == port)
   {
         if ((ret = reg_read(unit, MAPLE_SDS4_FIB_REG0r
                             , &value)) != RT_ERR_OK)
         {
             RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
             return ret;
         }
   }
   else if(26 == port)
   {
        if ((ret = reg_read(unit, MAPLE_SDS5_FIB_REG0r
                            , &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
   }
   else
   {
         return RT_ERR_FAILED;
   }

    if ((((value >> 6) & 0x1) == 0) && (((value >> 13) & 0x1) == 1))
        *pSpeed = PORT_SPEED_100M;
    else
        *pSpeed = PORT_SPEED_1000M;

    return RT_ERR_OK;
} /* end of phy_8380_speed_get */

/* Function Name:
 *      phy_8380_speed_set
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
phy_8380_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32 ret;
    uint32 reg_idx;
    uint32 reg_val = 0;

   if(24 == port)
   {
    reg_idx = MAPLE_SDS4_FIB_REG0r;
   }
   else if(26 == port)
   {
    reg_idx = MAPLE_SDS5_FIB_REG0r;
   }
   else
   {
         return RT_ERR_PORT_ID;
   }

    if ((ret = reg_read(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    reg_val &= ~((1UL<<6) | (1UL<<13));
    reg_val |= (((speed & 2)>>1) << 6) | ((speed & 1) << 13);

    if ((ret = reg_write(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    _phy_8380_reset(unit, port);

    return RT_ERR_OK;
} /* end of phy_8380_speed_set */

/* Function Name:
 *      phy_8380_duplex_get
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
phy_8380_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    int32 ret;
    uint32 value = 0;

    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    if(24 == port)
    {
        if ((ret = reg_read(unit, MAPLE_SDS4_FIB_REG0r
                            , &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
   }
   else if(26 == port)
   {
        if ((ret = reg_read(unit, MAPLE_SDS5_FIB_REG0r
                            , &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
   }
   else
   {
         return RT_ERR_FAILED;
   }

    if (((value >> 8) & 0x1) == 1)
        *pDuplex = PORT_FULL_DUPLEX;
    else
        *pDuplex = PORT_HALF_DUPLEX;

    return RT_ERR_OK;
} /* end of phy_8380_duplex_get */

/* Function Name:
 *      phy_8380_duplex_set
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
phy_8380_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex)
{
    int32 ret;
    uint32 reg_idx;
    uint32 reg_val = 0;

   if(24 == port)
   {
    reg_idx = MAPLE_SDS4_FIB_REG0r;
   }
   else if(26 == port)
   {
    reg_idx = MAPLE_SDS5_FIB_REG0r;
   }
   else
   {
         return RT_ERR_PORT_ID;
   }

    if ((ret = reg_read(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if (PORT_HALF_DUPLEX == duplex)
        reg_val &= ~(0x1<<8);
    else
        reg_val |= (0x1<<8);

    if ((ret = reg_write(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    _phy_8380_reset(unit, port);

    return RT_ERR_OK;
} /* end of phy_8380_duplex_set */

/* Function Name:
 *      phy_8380_flowCtrl_get
 * Description:
 *      Get flow control status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable - pointer to flow control status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8380_flowCtrl_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 value = 0;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(24 == port)
    {
        if ((ret = reg_read(unit, MAPLE_SDS4_FIB_REG4r
                            , &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
    }
    else if(26 == port)
    {
        if ((ret = reg_read(unit, MAPLE_SDS5_FIB_REG4r
                            , &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
    }
    else
    {
         return RT_ERR_FAILED;
    }

    if (((value >> 7) & 0x3) == 0)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;
    return ret;

    return RT_ERR_OK;
} /* end of phy_8380_flowCtrl_get */

/* Function Name:
 *      phy_8380_flowCtrl_set
 * Description:
 *      Set flow control status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - flow control status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8380_flowCtrl_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_val = 0;
    uint32 reg_idx;

    if(24 == port)
   {
    reg_idx = MAPLE_SDS4_FIB_REG4r;
   }
   else if(26 == port)
   {
    reg_idx = MAPLE_SDS5_FIB_REG4r;
   }
   else
   {
         return RT_ERR_PORT_ID;
   }

    if ((ret = reg_read(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    if (DISABLED == enable)
        reg_val &= ~(0x3<<7);
    else
        reg_val |= (0x3<<7);

    if ((ret = reg_write(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    return RT_ERR_OK;

} /* end of phy_8380_flowCtrl_set */

/* Function Name:
 *      phy_8380_autoNegoEnable_get
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
phy_8380_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 value = 0;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(24 == port)
   {
         if ((ret = reg_read(unit, MAPLE_SDS4_FIB_REG0r
                             , &value)) != RT_ERR_OK)
         {
             RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
             return ret;
         }
   }
   else if(26 == port)
   {
        if ((ret = reg_read(unit, MAPLE_SDS5_FIB_REG0r
                            , &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
   }
   else
   {
         return RT_ERR_FAILED;
   }

   if (((value >> 12) & 0x1) == 1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;


    return RT_ERR_OK;
} /* end of phy_8380_autoNegoEnable_get */


/* Function Name:
 *      phy_8380_autoNegoEnable_set
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
phy_8380_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_idx;
    uint32 reg_val = 0;

   if(24 == port)
   {
       reg_idx = MAPLE_SDS4_FIB_REG0r;
   }
   else if(26 == port)
   {
       reg_idx = MAPLE_SDS5_FIB_REG0r;
   }
   else
   {
       return RT_ERR_PORT_ID;
   }

    if ((ret = reg_read(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }


   if (DISABLED == enable)
   {
       reg_val &= ~(1UL<<12);
   }
   else
   {
       reg_val |= (1UL<<12);
   }


    if ((ret = reg_write(unit, reg_idx
                        , &reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8380_autoNegoEnable_set */

/* Function Name:
 *      phy_8380_autoNegoAbility_get
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
phy_8380_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32 ret;
    uint32 reg_idx;
    uint32  phy_data4 = 0;

    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    osal_memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));

    /* Fiber port read from intra serdes registers, not PHY */
   if(24 == port)
   {
    reg_idx = MAPLE_SDS4_FIB_REG4r;
   }
   else if(26 == port)
   {
    reg_idx = MAPLE_SDS5_FIB_REG4r;
   }
   else
   {
         return RT_ERR_PORT_ID;
   }

    if ((ret = reg_read(unit, reg_idx
                    , &phy_data4)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    pAbility->FC = (phy_data4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
    pAbility->AsyFC = (phy_data4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
    pAbility->Half_10 = 0;
    pAbility->Full_10 = 0;
    pAbility->Half_100 = 0;
    pAbility->Full_100 = 0;
    pAbility->Half_1000 = (phy_data4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
    pAbility->Full_1000 = (phy_data4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;

    return RT_ERR_OK;

} /* end of phy_8380_autoNegoAbility_get */

/* Function Name:
 *      phy_8380_autoNegoAbility_set
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
phy_8380_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_enable_t    enable;
    uint32          reg_idx;
    uint32         phy_data0 = 0;
    uint32         phy_data4= 0;
    int32           ret;

    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if(24 == port)
    {
        reg_idx = MAPLE_SDS4_FIB_REG4r;
    }
        else if(26 == port)
    {
        reg_idx = MAPLE_SDS5_FIB_REG4r;
    }
    else
    {
          return RT_ERR_PORT_ID;
    }


     if ((ret = reg_read(unit, reg_idx
                     , &phy_data4)) != RT_ERR_OK)
     {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
         return ret;
     }

     phy_data4 &= ~(0xF << 5);
     phy_data4 |= (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET) | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET);
     phy_data4 |= (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET) | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

     if ((ret = reg_write(unit, reg_idx
                     , &phy_data4)) != RT_ERR_OK)
     {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
         return ret;
     }

      /* get value from CHIP*/
     if ((ret = phy_8380_autoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
         return ret;


     if (ENABLED == enable)
     {
        if(24 == port)
        {
            reg_idx = MAPLE_SDS4_FIB_REG0r;
        }
        else if(26 == port)
        {
            reg_idx = MAPLE_SDS5_FIB_REG0r;
        }
        else
        {
              return RT_ERR_PORT_ID;
        }

        if ((ret = reg_read(unit, reg_idx
                        , &phy_data0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }

         phy_data0 |= (1 << 9);

         if ((ret = reg_write(unit, reg_idx
                             , &phy_data0)) != RT_ERR_OK)
         {
             RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
             return ret;
         }

     }

    return RT_ERR_OK;
} /* end of phy_8380_autoNegoAbility_set */


/* Function Name:
 *      phy_8380_fiberOAMLoopBack_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8380_fiberOAMLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  reg_val;
    int32    ret;

    ret = RT_ERR_OK;
    if((port != 24) && (port != 26))
          return RT_ERR_PORT_ID;

    if(enable == ENABLED)
    {
        /*Enable Loopback*/
        if(24 == port)
        {
            ioal_mem32_read(unit, 0xef80, &reg_val);
            reg_val |= (1UL<<4);
            ioal_mem32_write(unit, 0xef80, reg_val);
        }
        else
        {
            ioal_mem32_read(unit, 0xf180, &reg_val);
            reg_val |= (1UL<<4);
            ioal_mem32_write(unit, 0xf180, reg_val);
        }

        /*Delay until PHY Linkup*/
        osal_time_usleep(10 * 1000); /* delay 10mS */
    }
    else
    {
        /*Disable Loopback*/
        if(24 == port)
        {
            ioal_mem32_read(unit, 0xef80, &reg_val);
            reg_val &= ~(1UL<<4);
            ioal_mem32_write(unit, 0xef80, reg_val);
        }
        else
        {
            ioal_mem32_read(unit, 0xf180, &reg_val);
            reg_val &= ~(1UL<<4);
            ioal_mem32_write(unit, 0xf180, reg_val);
        }
    }
    return ret;
}   /* end of phy_8380_fiberOAMLoopBack_set */

/* Function Name:
 *      phy_8380_serdesFiberRx_check
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
phy_8380_serdesFiberRx_check(uint32 unit, uint32 port, uint32 *pStatus)
{
    uint32  reg, reg_data, new_reg;
    uint32  regData0, regData1;
    uint32  chk_loop, chk_error;

    reg_data = 0;
    chk_error = 0;
    *pStatus = PHY_FIBER_RX_STATUS_0;

    if (24 == port)
    {
        reg_write(unit, MAPLE_SDS4_EXT_REG24r, &reg_data);

        reg_field_read(unit, MAPLE_SDS4_FIB_REG0r, MAPLE_CFG_FIB_SPD_RD_00f, &regData0);
        reg_field_read(unit, MAPLE_SDS4_FIB_REG0r, MAPLE_CFG_FIB_SPD_RD_01f, &regData1);

        reg = MAPLE_SDS4_EXT_REG25r;
        new_reg = MAPLE_SDS4_EXT_REG29r;
    }
    else if (26 == port)
    {
        reg_field_read(unit, MAPLE_SDS5_FIB_REG0r, MAPLE_CFG_FIB_SPD_RD_00f, &regData0);
        reg_field_read(unit, MAPLE_SDS5_FIB_REG0r, MAPLE_CFG_FIB_SPD_RD_01f, &regData1);

        reg_write(unit, MAPLE_SDS5_EXT_REG24r, &reg_data);

        reg = MAPLE_SDS5_EXT_REG25r;
        new_reg = MAPLE_SDS5_EXT_REG29r;
    }
    else
        return RT_ERR_OK;

    if ((0 == regData0) && (1 == regData1))
    {
        /* read & clear */
        reg_field_read(unit, reg, MAPLE_MUX_SYMBOLERR_CNTf, &reg_data);

        for(chk_loop = 0; chk_loop < 3; ++chk_loop)
        {
            reg_field_read(unit, reg, MAPLE_MUX_SYMBOLERR_CNTf, &reg_data);
            if (0xFF == reg_data)
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
                reg_read(unit, new_reg, &reg_data);
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
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_serdesFiberRx_reset
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
phy_8380_serdesFiberRx_reset(uint32 unit, uint32 port)
{
    uint32  reg, reg_data;

    if (24 == port)
    {
        reg = MAPLE_SDS4_REG0r;
    }
    else if (26 == port)
    {
        reg = MAPLE_SDS5_REG0r;
    }
    else
        return RT_ERR_OK;

    reg_data = 0;
    reg_field_write(unit, reg, MAPLE_SDS_EN_RXf, &reg_data);

    osal_time_usleep(200);

    reg_data = 1;
    reg_field_write(unit, reg, MAPLE_SDS_EN_RXf, &reg_data);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_serdes_loopback_get
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
phy_8380_serdes_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 value = 0;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(24 == port)
   {
         if ((ret = reg_read(unit, MAPLE_SDS4_FIB_REG0r, &value)) != RT_ERR_OK)
         {
             RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
             return ret;
         }
   }
   else if(26 == port)
   {
        if ((ret = reg_read(unit, MAPLE_SDS5_FIB_REG0r, &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
            return ret;
        }
   }
   else
   {
         return RT_ERR_FAILED;
   }

   if (((value >> 14) & 0x1) == 1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_serdes_loopback_set
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
phy_8380_serdes_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_idx;
    uint32 reg_val = 0;

   if(24 == port)
   {
       reg_idx = MAPLE_SDS4_FIB_REG0r;
   }
   else if(26 == port)
   {
       reg_idx = MAPLE_SDS5_FIB_REG0r;
   }
   else
   {
       return RT_ERR_PORT_ID;
   }

    if ((ret = reg_read(unit, reg_idx ,&reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }


   if (DISABLED == enable)
   {
       reg_val &= ~(1UL<<14);
   }
   else
   {
       reg_val |= (1UL<<14);
   }

    if ((ret = reg_write(unit, reg_idx,&reg_val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_sds_get
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
phy_8380_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    uint32 sdsMode,field;
    int32   ret;

    if(24 == port)
    {
        field = MAPLE_SDS4_MODE_SELf;
    }
    else if(26 == port)
    {
        field = MAPLE_SDS5_MODE_SELf;
    }
    else
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_field_read(unit, MAPLE_SDS_MODE_SELr,field, &sdsMode))!= RT_ERR_OK)
        return ret;

    switch (sdsMode)
    {
        case 2:
            sdsCfg->sdsMode = RTK_MII_SGMII;
            break;
        case 4:
            sdsCfg->sdsMode = RTK_MII_1000BX_FIBER;
            break;
        case 9:
            sdsCfg->sdsMode = RTK_MII_DISABLE;
            break;
        default:
            return RTK_MII_NONE;
    }

    return RT_ERR_OK;
}

int32
_phy_8380_sds_sgmii_set(uint32 unit, rtk_port_t port)
{
    uint32 val,field0,field1;
    int32   ret;

    if(24 == port)
    {
        field0 = MAPLE_SDS4_MODE_SELf;
        field1 = MAPLE_SDS4_INTF_MODE_2_0f;
    }
    else if(26 == port)
    {
        field0 = MAPLE_SDS5_MODE_SELf;
        field1 = MAPLE_SDS5_INTF_MODE_2_0f;
    }
    else
    {
        return RT_ERR_PORT_ID;
    }

    val = 2;
    if ((ret = reg_field_write(unit, MAPLE_SDS_MODE_SELr, field0, &val)) != RT_ERR_OK)
        return ret;

    val = 2;
    if ((ret = reg_field_write(unit, MAPLE_INT_MODE_CTRLr, field1, &val)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
}   /* end of phy_8380_sds_sgmii_set */

int32
_phy_8380_sds_1000BX_set(uint32 unit, rtk_port_t port)
{
    uint32 val,field0,field1;
    int32   ret;

    if(24 == port)
    {
        field0 = MAPLE_SDS4_MODE_SELf;
        field1 = MAPLE_SDS4_INTF_MODE_2_0f;
    }
    else if(26 == port)
    {
        field0 = MAPLE_SDS5_MODE_SELf;
        field1 = MAPLE_SDS5_INTF_MODE_2_0f;
    }
    else
    {
        return RT_ERR_PORT_ID;
    }

    val = 4;
    if ((ret = reg_field_write(unit, MAPLE_SDS_MODE_SELr, field0, &val)) != RT_ERR_OK)
        return ret;

    val = 1;
    if ((ret = reg_field_write(unit, MAPLE_INT_MODE_CTRLr, field1, &val)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
}   /* end of _phy_8380_sds_1000BX_set */

/* Function Name:
 *      phy_8380_sds_set
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
phy_8380_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    int32   ret;

    switch (sdsCfg->sdsMode)
    {
        case RTK_MII_SGMII:
            ret = _phy_8380_sds_sgmii_set(unit, port);
            break;
        case RTK_MII_1000BX_FIBER:
            ret = _phy_8380_sds_1000BX_set(unit, port);
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}
/* Function Name:
 *      phy_8380_fiberRxEnable_get
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
phy_8380_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 sdsId;
    uint32 val = 0;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    if(4 == sdsId)
        reg_read(unit, MAPLE_SDS4_EXT_REG0r, &val);
    if(5 == sdsId)
        reg_read(unit, MAPLE_SDS5_EXT_REG0r, &val);
    val &= 0xfffffffc;
    if (val == 1)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_fiberRxEnable_set
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
phy_8380_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 sdsId,val = 0;

    if(HWP_CASCADE_PORT(unit, port))
        return RT_ERR_OK;

    sdsId = HWP_PORT_SDSID(unit, port);

    if(4 == sdsId)
    {
        if (enable == DISABLED)
        {
            reg_read(unit, MAPLE_SDS4_REG14r, &val);
            val |= (1 << 12);
            reg_write(unit, MAPLE_SDS4_REG14r, &val);
            reg_read(unit, MAPLE_SDS4_EXT_REG0r, &val);
            val |= (1 << 0);
            reg_write(unit, MAPLE_SDS4_EXT_REG0r, &val);
        }
        else
        {
            reg_read(unit, MAPLE_SDS4_REG14r, &val);
            val |= (1 << 12);
            reg_write(unit, MAPLE_SDS4_REG14r, &val);
            reg_read(unit, MAPLE_SDS4_EXT_REG0r, &val);
            val &= ~(1 << 0);
            reg_write(unit, MAPLE_SDS4_EXT_REG0r, &val);
        }
    }
    if(5 == sdsId)
    {
        if (enable == DISABLED)
        {
            reg_read(unit, MAPLE_SDS5_REG14r, &val);
            val |= (1 << 12);
            reg_write(unit, MAPLE_SDS5_REG14r, &val);
            reg_read(unit, MAPLE_SDS5_EXT_REG0r, &val);
            val |= (1 << 0);
            reg_write(unit, MAPLE_SDS5_EXT_REG0r, &val);
        }
        else
        {
            reg_read(unit, MAPLE_SDS5_REG14r, &val);
            val |= (1 << 12);
            reg_write(unit, MAPLE_SDS5_REG14r, &val);
            reg_read(unit, MAPLE_SDS5_EXT_REG0r, &val);
            val &= ~(1 << 0);
            reg_write(unit, MAPLE_SDS5_EXT_REG0r, &val);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_polar_get
 * Description:
 *      Get serdes port polarity configure
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
phy_8380_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    int32       ret = RT_ERR_OK;
    uint32      data = 0;
    uint32      sdsId = 0;
    uint32      polar_rx = 0;
    uint32      polar_tx = 0;
    uint32      sds_reg0[] = {
        MAPLE_SDS0_REG0r,
        MAPLE_SDS1_REG0r,
        MAPLE_SDS2_REG0r,
        MAPLE_SDS3_REG0r,
        MAPLE_SDS4_REG0r,
        MAPLE_SDS5_REG0r,
    };

    osal_memset(pPolarCtrl, 0, sizeof(rtk_port_phyPolarCtrl_t));

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = reg_read(unit, sds_reg0[sdsId], &data)) != RT_ERR_OK)
        return ret;

    reg_field_get(unit, sds_reg0[sdsId], MAPLE_INV_HSIf, &polar_rx, &data); /* INV_HSI */
    reg_field_get(unit, sds_reg0[sdsId], MAPLE_INV_HSOf, &polar_tx, &data); /* INV_HSO */
    pPolarCtrl->phy_polar_rx = polar_rx ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL;
    pPolarCtrl->phy_polar_tx = polar_tx ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_polar_set
 * Description:
 *      Configure serdes port polarity
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
phy_8380_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    int32       ret = RT_ERR_OK;
    uint32      data = 0;
    uint32      sdsId = 0;
    uint32      polar_rx = 0;
    uint32      polar_tx = 0;
    uint32      sds_reg0[] = {
        MAPLE_SDS0_REG0r,
        MAPLE_SDS1_REG0r,
        MAPLE_SDS2_REG0r,
        MAPLE_SDS3_REG0r,
        MAPLE_SDS4_REG0r,
        MAPLE_SDS5_REG0r,
    };

    polar_rx = (pPolarCtrl->phy_polar_rx == PHY_POLARITY_INVERSE) ? 1 : 0;
    polar_tx = (pPolarCtrl->phy_polar_tx == PHY_POLARITY_INVERSE) ? 1 : 0;

    sdsId = HWP_PORT_SDSID(unit, port);

    if((ret = reg_read(unit, sds_reg0[sdsId], &data)) != RT_ERR_OK)
        return ret;

    reg_field_set(unit, sds_reg0[sdsId], MAPLE_INV_HSIf, &polar_rx, &data); /* INV_HSI */
    reg_field_set(unit, sds_reg0[sdsId], MAPLE_INV_HSOf, &polar_tx, &data); /* INV_HSO */

    if((ret = reg_write(unit, sds_reg0[sdsId], &data)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8380_fiberLinkUp_handler1
 * Description:
 *      Handle fiber linkup
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 * Note:
 *      None
 */
int32 phy_8380_fiberLinkUp_handler1(uint32 unit, rtk_port_t port)
{
    uint32  reg_addr_rst, val = 0;

    /*Get link status */
    if(port == 24)
    {
        reg_addr_rst = MAPLE_SDS4_REG3r;
    }
    else
    {
        reg_addr_rst = MAPLE_SDS5_REG3r;
    }

    reg_read(unit, reg_addr_rst, &val);
    val |= (1 << 6);
    reg_write(unit, reg_addr_rst, &val);
    val &= ~(1 << 6);
    reg_write(unit, reg_addr_rst, &val);

    return RT_ERR_OK;
}   /* end of phy_8380_fiberLinkUp_handler1 */

#endif /* !defined(__BOOTLOADER__) */

