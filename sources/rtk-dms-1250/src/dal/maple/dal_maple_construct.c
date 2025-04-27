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
 * Purpose : Setup MAC Serdes parameters
 *
 * Feature : Setup MAC Serdes parameters functions
 *
 */
#include <dal/dal_construct.h>
#include <dal/maple/dal_maple_construct.h>
#include <hal/phy/phy_construct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <ioal/phy_reset.h>

/*
 * Macro Definition
 */
#define RTL8380_VERSION_NONE        0x0
#define RTL8380_VERSION_A           0x1
#define RTL8380_VERSION_B           0x2
#define RTL8380_VERSION_C           0x3


#define DAL_MAPLE_MAX_SDS_NUM       6

/*
 * Data Declaration
 */
static uint32 phy_ctrl_bak1[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
static uint32 phy_ctrl_bak2[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
static uint32 phy_ctrl_bak3[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];

static uint32 rtl8380_ver;
static void _dal_maple_construct_version_check(uint32 unit)
{
    /*Chip Version Control*/
    uint32 original_data_intRd;
    uint32 temp_chip_info;


    ioal_mem32_read(unit, 0x0058, &original_data_intRd);
    ioal_mem32_write(unit, 0x0058, original_data_intRd | 0x3);

    ioal_mem32_read(unit, 0x00D0, &temp_chip_info);

    ioal_mem32_write(unit, 0x0058, original_data_intRd);

    temp_chip_info &= 0x1f;

    if (temp_chip_info==0x2)
    {
        rtl8380_ver = RTL8380_VERSION_B;
    }
    else if (temp_chip_info==0x3)
    {
        rtl8380_ver = RTL8380_VERSION_C;
    }
    else
    {
        rtl8380_ver = RTL8380_VERSION_NONE;
    }
}

static void _dal_maple_construct_phyCmn_powerOn(uint32 unit, rtk_port_t mac_id)
{
        unsigned int  reg_val;
        RTK_MII_READ(unit,mac_id, 0, 0, &reg_val);
        reg_val &= ~(0x1 << 11);
        RTK_MII_WRITE(unit,mac_id, 0, 0, reg_val);
        return;
}

static void _dal_maple_construct_phyCmn_powerOff(uint32 unit, rtk_port_t mac_id)
{
        unsigned int  reg_val;
        RTK_MII_READ(unit,mac_id, 0, 0, &reg_val);
        reg_val |= (0x1 << 11);
        RTK_MII_WRITE(unit,mac_id, 0, 0, reg_val);
        return;
}


static void _dal_maple_construct_phyCmn_reset(uint32 unit, rtk_port_t mac_id)
{
        unsigned int  reg_val;
        RTK_MII_READ(unit,mac_id, 0, 0, &reg_val);
        reg_val |= (0x1 << 15);
        RTK_MII_WRITE(unit,mac_id, 0, 0, reg_val);
        return;
}

static void _dal_construct_rtl8380_serdesFiber_powerOnOff(uint32 unit, rtk_port_t mac_id, int val)
{
    uint32 serdes_reg0;
    uint32 serdes_offset;
    uint32 serdes_mode;
    uint32 serdes_reg1;
    uint32 serdes_reg2;
    uint32 serdes_reg3;
    uint32 serdes_val;

    if((24  != mac_id) && (26  != mac_id))
        return;

    /*Serdes Mode*/
    serdes_reg0 = 0x0028;
    if (24 == mac_id)
        serdes_offset = 5;
    else
        serdes_offset = 0;

    if (0x1 == val)
        serdes_mode = 0x4;
    else
        serdes_mode = 0x9;

    ioal_mem32_read(unit, serdes_reg0, &serdes_val);
    serdes_val &= ~(0x1f << serdes_offset);
    serdes_val |= (serdes_mode << serdes_offset);
    ioal_mem32_write(unit, serdes_reg0, serdes_val);

    /*Serdes Reset*/
    if (24 == mac_id)
    {
        serdes_reg1 = 0xef80;
        serdes_reg2 = 0xef8c;
        serdes_reg3 = 0xf800;
    }
    else
    {
        serdes_reg1 = 0xf180;
        serdes_reg2 = 0xf18c;
        serdes_reg3 = 0xf900;
    }

    /*Always do this no matter whether Power on of Power off*/
    ioal_mem32_read(unit, serdes_reg3, &serdes_val);
    serdes_val &= ~(0x1 << 11);
    ioal_mem32_write(unit, serdes_reg3, serdes_val);

    /*Analog Reset*/
    ioal_mem32_read(unit, serdes_reg1, &serdes_val);
    serdes_val &= ~(0x3 << 0);
    ioal_mem32_write(unit, serdes_reg1, serdes_val);

    ioal_mem32_read(unit, serdes_reg1, &serdes_val);
    serdes_val &= ~(0x3 << 0);
    serdes_val |= (0x3 << 0);
    ioal_mem32_write(unit, serdes_reg1, serdes_val);

    /*Digital Reset*/
    ioal_mem32_read(unit, serdes_reg2, &serdes_val);
    serdes_val |= (0x1 << 6);
    ioal_mem32_write(unit, serdes_reg2, serdes_val);

    ioal_mem32_read(unit, serdes_reg2, &serdes_val);
    serdes_val &= ~(0x1 << 6);
    ioal_mem32_write(unit, serdes_reg2, serdes_val);

    return;
}


#if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB))
/* Backup Phy power ctrl original value and trun off */
static void _phy_construct_turnoff_power_ctrl_common(uint32 unit, rtk_port_t mac_id)
{
    uint32 value1, value2;

    /* Backup original value */
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0xbc0);
    RTK_MII_READ(unit, mac_id, 0xfff, 19, &phy_ctrl_bak1[unit][mac_id]);
    RTK_MII_READ(unit, mac_id, 0xfff, 18, &phy_ctrl_bak2[unit][mac_id]);

    /*Turn off*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 18, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 19, 0x01d0);

    /*Check Value*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0xbc0);
    RTK_MII_READ(unit, mac_id, 0xfff, 19, &value1);
    RTK_MII_READ(unit, mac_id, 0xfff, 18, &value2);
    if(value1 != 0x01d0)
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "phy %d power ctrl 19 fail\n", mac_id);
    if(value2 != 0x0000)
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "phy %d power ctrl 18 fail\n", mac_id);

    return;
}
#endif

#if (defined(CONFIG_SDK_RTL8218B))
static void _phy_construct_turnoff_power_ctrl_18b(uint32 unit, rtk_port_t mac_id)
{
    _phy_construct_turnoff_power_ctrl_common(unit, mac_id);

    return;
}
#endif

#if (defined(CONFIG_SDK_RTL8214FC))
static void _phy_construct_turnoff_power_ctrl_14fc(uint32 unit, rtk_port_t mac_id)
{
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_READ(unit, mac_id, 0xfff, 0x1e, &phy_ctrl_bak3[unit][mac_id]);

    /*Force Copper*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1e, 0x0001);

    _phy_construct_turnoff_power_ctrl_common(unit, mac_id);

    return;
}
#endif

#if (defined(CONFIG_SDK_RTL8218FB))
static void _phy_construct_turnoff_power_ctrl_18fb(uint32 unit, rtk_port_t mac_id)
{
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_READ(unit, mac_id, 0xfff, 0x1e, &phy_ctrl_bak3[unit][mac_id]);

    /*Force Copper*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1e, 0x0001);

    _phy_construct_turnoff_power_ctrl_common(unit, mac_id);

    return;
}
#endif

static void _dal_maple_construct_phyPowerCtrl_off(uint32 unit)
{
    rtk_port_t mac_id;

    /* trun off all ports afe */
    HWP_PORT_TRAVS(unit, mac_id)
    {
        if (HWP_PHY_EXIST(unit, mac_id))
        {
            switch (HWP_PHY_MODEL_BY_PORT(unit, mac_id))
            {
#if (defined(CONFIG_SDK_RTL8218B))
                case RTK_PHYTYPE_RTL8218B:
                    _phy_construct_turnoff_power_ctrl_18b(unit, mac_id);
                    break;
#endif

#if (defined(CONFIG_SDK_RTL8214FC))
                case RTK_PHYTYPE_RTL8214FC:
                    _phy_construct_turnoff_power_ctrl_14fc(unit, mac_id);
                    break;
#endif

#if (defined(CONFIG_SDK_RTL8218FB))
                case RTK_PHYTYPE_RTL8218FB:
                    _phy_construct_turnoff_power_ctrl_18fb(unit, mac_id);
                    break;
#endif

                default:
                    break;
            }
        }
    }
    return;
}

#if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB))
/*Restore Analog Signal*/
static void _phy_construct_restore_power_ctrl_common(uint32 unit, rtk_port_t mac_id)
{
    uint32 value1, value2;

    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0xbc0);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 18, phy_ctrl_bak2[unit][mac_id]);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 19, phy_ctrl_bak1[unit][mac_id]);

    /*Check Value*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0xbc0);
    RTK_MII_READ(unit, mac_id, 0xfff, 18, &value2);
    RTK_MII_READ(unit, mac_id, 0xfff, 19, &value1);
    if(value1 != phy_ctrl_bak1[unit][mac_id])
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "phy %d power ctrl 19 fail\n", mac_id);
    if(value2 != phy_ctrl_bak2[unit][mac_id])
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "phy %d power ctrl 18 fail\n", mac_id);

    return;
}
#endif

#if (defined(CONFIG_SDK_RTL8218B))
static void _phy_construct_restore_power_ctrl_18b(uint32 unit, rtk_port_t mac_id)
{
    _phy_construct_restore_power_ctrl_common(unit, mac_id);

    return;
}
#endif

#if (defined(CONFIG_SDK_RTL8214FC))
static void _phy_construct_restore_power_ctrl_14fc(uint32 unit, rtk_port_t mac_id)
{
    /*Force Copper*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1e, 0x0001);

    _phy_construct_restore_power_ctrl_common(unit, mac_id);

    /*Enable Auto*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1e, phy_ctrl_bak3[unit][mac_id]);

    return;
}
#endif

#if (defined(CONFIG_SDK_RTL8218FB))
static void _phy_construct_restore_power_ctrl_18fb(uint32 unit, rtk_port_t mac_id)
{
    /*Force Copper*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1e, 0x0001);

    _phy_construct_restore_power_ctrl_common(unit, mac_id);

    /*Enable Auto*/
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1f, 0x0000);
    RTK_MII_WRITE(unit, mac_id, 0xfff, 0x1e, phy_ctrl_bak3[unit][mac_id]);

    return;
}
#endif

static void _dal_maple_construct_phyPowerCtrl_restore(uint32 unit)
{
    rtk_port_t mac_id;

    /* restore all ports */
    HWP_PORT_TRAVS(unit, mac_id)
    {
        if (HWP_PHY_EXIST(unit, mac_id))
        {
            switch (HWP_PHY_MODEL_BY_PORT(unit, mac_id))
            {
#if (defined(CONFIG_SDK_RTL8218B))
                case RTK_PHYTYPE_RTL8218B:
                    _phy_construct_restore_power_ctrl_18b(unit, mac_id);
                    break;
#endif

#if (defined(CONFIG_SDK_RTL8214FC))
                case RTK_PHYTYPE_RTL8214FC:
                    _phy_construct_restore_power_ctrl_14fc(unit, mac_id);
                    break;
#endif

#if (defined(CONFIG_SDK_RTL8218FB))
                case RTK_PHYTYPE_RTL8218FB:
                    _phy_construct_restore_power_ctrl_18fb(unit, mac_id);
                    break;
#endif

                default:
                    break;
            }
        }
    }
    return;
}


/* Function Name:
 *      dal_maple_construct_default_init
 * Description:
 *      Init chip default value
 * Input:
 *      unit - which unit
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_maple_construct_default_init(uint32 unit)
{

    return;
}


void dal_maple_construct_pollingPhy_disable(uint32 unit)
{
    CNSTRT_PRINT("%s()\n",__FUNCTION__);
    ioal_mem32_write(unit, 0xa17c, 0x00000000);
}

void dal_maple_construct_pollingPhy_enable(uint32 unit)
{
    rtk_portmask_t      portmask;
    uint32              port;
    uint32 val;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    osal_time_mdelay(1000);

    RTK_PORTMASK_RESET(portmask);
    HWP_PORT_TRAVS(unit, port)
    {
        if (HWP_ETHER_PORT(unit, port))
        {
            RTK_PORTMASK_PORT_SET(portmask, port);
        }
    }

    val = RTK_PORTMASK_WORD_GET(portmask, 0);

    ioal_mem32_write(unit, 0xa17c, val);

    if(RTL8380_VERSION_A == rtl8380_ver)
    {
        /*Bug: Only for Final Chip A-CUT  && 833X Series, mantis#0013048 */
        if((HWP_CHIP_ID(unit) == RTL8330M_CHIP_ID) || (HWP_CHIP_ID(unit) == RTL8332M_CHIP_ID))
        {

            ioal_mem32_read(unit, 0xa17c, &val);
            val &= 0xffffff;
            ioal_mem32_write(unit, 0xa17c, val);

            ioal_mem32_read(unit, 0xa100, &val);
            val &= ~0x80;
            ioal_mem32_write(unit, 0xa100, val);
        }
    }

    /*PHY PATCH OK*/
    ioal_mem32_read(unit, 0xa100, &val);
    val |= 0x8000;
    ioal_mem32_write(unit, 0xa100, val);
}

/* Function Name:
 *      dal_maple_construct_phy_reset
 * Description:
 *      Reset PHY.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_maple_construct_phy_reset(uint32 unit)
{
    int32   ret;
    rtk_port_t mac_idx;
    rtk_port_t  basePort = 0;
    uint32      phyIdx = 0;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    HWP_PHY_TRAVS(unit, phyIdx)
    {
        basePort = HWP_PHY_BASE_MACID_BY_IDX(unit, phyIdx);
        if (!HWP_PORT_EXIST(unit, basePort))
            continue;

        ret = ioal_phy_reset(unit, basePort);
        if (ret != RT_ERR_OK)
            break;
    }
    if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
    {
        /*reset all ports */
        HWP_PORT_TRAVS(unit,mac_idx)
        {
            if (!(HWP_PORT_EXIST(unit, mac_idx) && (HWP_PHY_EXIST(unit, mac_idx) || HWP_SERDES_PORT(unit, mac_idx))))
            {
                continue;
            }

            switch (HWP_PHY_MODEL_BY_PORT(unit, mac_idx))
            {
                case RTK_PHYTYPE_RTL8218E:
                    basePort = HWP_PHY_BASE_MACID(unit, mac_idx);
                    if (mac_idx == basePort)
                    {
                        RTK_MII_WRITE(unit, mac_idx, 0x0, 30, 0x8);
                        RTK_MII_WRITE(unit, mac_idx, 0x262, 16, 0x1);
                        RTK_MII_WRITE(unit, mac_idx, 0x0, 30, 0x0);
                        osal_time_mdelay(200);
                    }
                    break;
                default:
                    _dal_maple_construct_phyCmn_reset(unit, mac_idx);
                    break;
            }
        }
    }

    return;
} /* end of construct_rtl8380_phyReset */


/* Function Name:
 *      dal_maple_construct_phyExceptSerdesFiber_powerOn
 * Description:
 *      Power-On PHY except serdes fiber.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_maple_construct_phyExceptSerdesFiber_powerOn(uint32 unit)
{
    int i;
    rtk_port_t  mac_id;
    #if defined(CONFIG_SDK_RTL8214FC)
    int32 ret = RT_ERR_OK;
    #endif

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    /* power-on all ports */
    HWP_PORT_TRAVS(unit,i)
    {
        mac_id = i;

        if (!HWP_PHY_EXIST(unit, i) && !HWP_SERDES_PORT(unit, i))
            continue;

        switch (HWP_PHY_MODEL_BY_PORT(unit,i))
        {
            #if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
            case RTK_PHYTYPE_RTL8214FB:
            case RTK_PHYTYPE_RTL8214B:
            case RTK_PHYTYPE_RTL8212B:
                construct_rtl8214fb_phyPowerOn(unit, mac_id);
                break;
            #endif
            #if (defined(CONFIG_SDK_RTL8214FC))
            case RTK_PHYTYPE_RTL8214FC:
                    if((ret = phy_enable_set(unit, mac_id, ENABLED)) != RT_ERR_OK)
                    {
                        CNSTRT_PRINT("port %u enable failed\n",mac_id);
                    }
                break;
            #endif
            default:
                _dal_maple_construct_phyCmn_powerOn(unit, mac_id);
                break;
        }
        osal_time_udelay(20*1000);    /* delay 20mS */
    }

    return;
}/* end of dal_maple_construct_phyExceptSerdesFiber_powerOn */

/* Function Name:
 *      construct_rtl8380_phyPowerOff
 * Description:
 *      Power-Off PHY.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_maple_construct_phy_powerOff(uint32 unit)
{
    rtk_port_t mac_id;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    ioal_mem32_write(unit, 0x0140, 0xC);

     /* power-off all ports */
    HWP_PORT_TRAVS(unit, mac_id)
    {
        if (HWP_SERDES_PORT(unit, mac_id))
        {
            _dal_construct_rtl8380_serdesFiber_powerOnOff(unit, mac_id, 0);
        }
        else if (HWP_PHY_EXIST(unit, mac_id))
        {

            switch (HWP_PHY_MODEL_BY_PORT(unit,mac_id))
            {
              #if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
              case RTK_PHYTYPE_RTL8214FB:
              case RTK_PHYTYPE_RTL8214B:
              case RTK_PHYTYPE_RTL8212B:
                construct_rtl8214fb_phyPowerOff(unit, mac_id);
                break;
              #endif
              #if (defined(CONFIG_SDK_RTL8214FC))
              case RTK_PHYTYPE_RTL8214FC:
                {
                    unsigned int reg_val;

                    /* fiber */
                    RTK_MII_WRITE(unit,mac_id, RTK_MII_MAXPAGE8380, 30, 3);
                    RTK_MII_READ(unit,mac_id, 0, 16, &reg_val);
                    RTK_MII_WRITE(unit,mac_id, 0, 16, reg_val | (0x1 << 11));

                    /* copper */
                    RTK_MII_WRITE(unit,mac_id, RTK_MII_MAXPAGE8380, 30, 1);
                    RTK_MII_READ(unit,mac_id, 0xa40, 16, &reg_val);
                    RTK_MII_WRITE(unit,mac_id, 0xa40, 16, reg_val | (0x1 << 11));

                    RTK_MII_WRITE(unit,mac_id, RTK_MII_MAXPAGE8380, 30, 0);
                }
                break;
              #endif
              #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208G))
              case RTK_PHYTYPE_RTL8208G:
              case RTK_PHYTYPE_RTL8208D:
              #endif
              #if (defined(CONFIG_SDK_RTL8214))
              case RTK_PHYTYPE_RTL8214:
              #endif
              default:
                _dal_maple_construct_phyCmn_powerOff(unit,mac_id);
                break;
            }/* end switch */
        }
    }/* end HWP_PORT_TRAVS */
    osal_time_udelay(20*1000);    /* delay 20mS */
    return;
} /* end of construct_rtl8380_phyPowerOff */

/* Function Name:
 *      dal_maple_construct_phyConfig_init
 * Description:
 *      PHY Configuration code that connect with RTL8380
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_maple_construct_phyConfig_init(uint32 unit)
{

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    _dal_maple_construct_version_check(unit);

    /* SS-1584 prevent from short link up */
    _dal_maple_construct_phyPowerCtrl_off(unit);

    phy_construct_config_init(unit);

    /*For LED on reason, not power on serdes-direct-fiber*/
    dal_maple_construct_phyExceptSerdesFiber_powerOn(unit);

    osal_time_mdelay(500);

    dal_maple_construct_phy_powerOff(unit);

    /* SS-1584 */
    _dal_maple_construct_phyPowerCtrl_restore(unit);

    return;
} /* end of dal_maple_construct_phyConfig_init */



static confcode_mac_regval_t rtl8380_mac_patch[] = {
    /*Patch for EEE, a timer cannot be zero*/
    {0xAA04, 0x5001411},
    {0xAA08, 0x5001417},
};

void dal_maple_construct_macConfig_init(uint32 unit)
{
    uint32 i, val;
    uint32 portId;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    /*Set Port28-CPU Port Egress Drop always enable*/
    ioal_mem32_read(unit, 0x6B1C+28*4, &val);
    val |= 1UL<<11;
    ioal_mem32_write(unit, 0x6B1C+28*4, val);


    /******************************Giga Ability & PortID***************************/

    /*MAC Patch*/
    for (i=0; i<(sizeof(rtl8380_mac_patch)/sizeof(confcode_mac_regval_t)); i++)
    {
        ioal_mem32_write(unit,rtl8380_mac_patch[i].reg, rtl8380_mac_patch[i].val);
        osal_time_mdelay(1);
    }

    /*Patch for disable Special Drain Out*/
    if((HWP_CHIP_ID(unit) == RTL8332M_CHIP_ID) || (HWP_CHIP_ID(unit) == RTL8382M_CHIP_ID))
    {
        for(i = 0; i <= 28; i++)
            ioal_mem32_write(unit,0xd57c+i*0x80,  0);
    }
    else if((HWP_CHIP_ID(unit) == RTL8330M_CHIP_ID) || (HWP_CHIP_ID(unit) == RTL8380M_CHIP_ID))
    {
        for(i = 8; i <= 28; i++)
            ioal_mem32_write(unit,0xd57c+i*0x80,  0);
    }
    else
    {
        /*Do nothing*/
    }

    /*begin patch mantis#0013049*/
    /************************disable two-pair down speed feature*************/
    /************************Added on 20130207 ***************************/
    for(portId = 0; portId < 29; portId++)
    {
        ioal_mem32_read(unit, 0xa104+4*portId, &val);
        val &= ~((1UL<<21) | (1UL<<24) | (1UL<<27));
        ioal_mem32_write(unit, 0xa104+4*portId, val);
    }
    /*end patch mantis#0013049*/

    return;
} /* end of dal_maple_construct_macConfig_init */



static confcode_mac_regval_t rtl8380_serdes_soft_reset_take[] = {
  {0x0034, 0x0000003f},{0x003c, 0x00000010},
  {0xe78c, 0x00007146},{0xe98c, 0x00007146},{0xeb8c, 0x00007146},{0xed8c, 0x00007146},
  {0xef8c, 0x00007146},{0xf18c, 0x00007146},
};

static confcode_mac_regval_t rtl8380_serdes_common_patch[] = {
  {0xf878, 0x0000071e},{0xf978, 0x0000071e},{0xe784, 0x00000F00},{0xe984, 0x00000F00},
  {0xeb84, 0x00000F00},{0xed84, 0x00000F00},{0xef84, 0x00000F00},{0xf184, 0x00000F00},
  {0xe788, 0x00007060},{0xe988, 0x00007060},{0xeb88, 0x00007060},{0xed88, 0x00007060},
  {0xef88, 0x00007060},{0xf188, 0x00007060},{0xef90, 0x0000074d},{0xf190, 0x0000074d},
};


/*******************************RTL8380 Version-A*********************************/
/*Serdes0,1*/
static confcode_mac_regval_t rtl8380_serdes01_rsgmiip_6275a[] = {
  {0x0ff8, 0xaaaaaaaf},{0xf388, 0x000085fa},{0xf488, 0x000085fa},{0xf398, 0x000020d8},
  {0xf498, 0x000020d8},{0xf3c4, 0x0000B7C9},{0xf4ac, 0x00001482},{0xf4a8, 0x000080c7},
  {0xf3c8, 0x0000ab8e},{0xf3ac, 0x00001482},{0xf380, 0x00004040},{0xf380, 0x00004000},
  {0xf480, 0x00004040},{0xf480, 0x00004000},{0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},
  {0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},
 };
static confcode_mac_regval_t rtl8380_serdes01_qsgmii_6275a[] = {
  {0x0ff8, 0xaaaaaaaf},{0xf388, 0x000085fa},{0xf488, 0x000085fa},{0xf398, 0x000020d8},
  {0xf498, 0x000020d8},{0xf3c4, 0x0000B7C9},{0xf4ac, 0x00001482},{0xf4a8, 0x000080c7},
  {0xf3c8, 0x0000ab8e},{0xf3ac, 0x00001482},{0xf380, 0x00004040},{0xf380, 0x00004000},
  {0xf480, 0x00004040},{0xf480, 0x00004000},{0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},
  {0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},
 };
static confcode_mac_regval_t rtl8380_serdes01_xsmii_6275a[] = {
  {0x0058, 0x00000003},{0x0ff8, 0xaaaaaaab},{0xf3c4, 0x0000b7c9},{0xf3c8, 0x0000838e},
  {0xf3cc, 0x00000a4b},{0xf3d0, 0x00007211},{0xf388, 0x000085fa},{0xf38c, 0x00008c6f},
  {0xf390, 0x0000dccc},{0xf394, 0x00000000},{0xf398, 0x000020d8},{0xf39c, 0x00000003},
  {0xf3a0, 0x000079aa},{0xf3a4, 0x00008c64},{0xf3a8, 0x000000c3},{0xf3ac, 0x00001482},
  {0xf460, 0x000014aa},{0xf464, 0x00004800},{0xf3b8, 0x0000f002},{0xf46c, 0x000004bf},
  {0xf4c4, 0x00004209},{0xf4c8, 0x0000c1f5},{0xf4cc, 0x00000000},{0xf4d0, 0x00000000},
  {0xf488, 0x000085fa},{0xf48c, 0x00000000},{0xf490, 0x0000dccc},{0xf494, 0x00000000},
  {0xf498, 0x000020d8},{0xf49c, 0x00000003},{0xf4a0, 0x000079aa},{0xf4a4, 0x00008c64},
  {0xf4a8, 0x000000c3},{0xf4ac, 0x00001482},{0xf560, 0x000014aa},{0xf564, 0x00004800},
  {0xf4b8, 0x0000f002},{0xf56c, 0x000004bf},{0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},
  {0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},{0x0058, 0x00000000},
 };


/*Serdes2,3*/
static confcode_mac_regval_t rtl8380_serdes23_rsgmiip_6275a[] = {
  {0xf588, 0x000085fa},{0xf688, 0x000085fa},{0xf788, 0x000085fa},{0xf598, 0x000020d8},
  {0xf698, 0x000020d8},{0xf5c4, 0x0000B7C9},{0xf5c8, 0x0000ab8e},{0xf5ac, 0x00001482},
  {0xf6ac, 0x00001482},{0xf580, 0x00004040},{0xf580, 0x00004000},{0xf680, 0x00004040},
  {0xf680, 0x00004000},{0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},
  {0xf6a4, 0x00008c64},
 };
static confcode_mac_regval_t rtl8380_serdes23_qsgmii_6275a[] = {
  {0xf588, 0x000085fa},{0xf688, 0x000085fa},{0xf788, 0x000085fa},{0xf598, 0x000020d8},
  {0xf698, 0x000020d8},{0xf5c4, 0x0000B7C9},{0xf5c8, 0x0000ab8e},{0xf5ac, 0x00001482},
  {0xf6ac, 0x00001482},{0xf580, 0x00004040},{0xf580, 0x00004000},{0xf680, 0x00004040},
  {0xf680, 0x00004000},{0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},
  {0xf6a4, 0x00008c64},
 };
static confcode_mac_regval_t rtl8380_serdes23_xsmii_6275a[] = {
  {0xf5c4, 0x0000B7C9},{0xf5c8, 0x0000838e},{0xf5cc, 0x00000a4b},{0xf5d0, 0x00007211},
  {0xf588, 0x000085fa},{0xf58c, 0x00008c6f},{0xf590, 0x0000dccc},{0xf594, 0x00000000},
  {0xf598, 0x000020d8},{0xf59c, 0x00000003},{0xf5a0, 0x000079aa},{0xf5a4, 0x00008c64},
  {0xf5a8, 0x000000c3},{0xf5ac, 0x00001482},{0xf660, 0x000014aa},{0xf664, 0x00004800},
  {0xf5b8, 0x0000f002},{0xf66c, 0x000004bf},{0xf6c4, 0x00004209},{0xf6c8, 0x0000c1f5},
  {0xf6cc, 0x00000000},{0xf6d0, 0x00000000},{0xf688, 0x000085fa},{0xf68c, 0x00000000},
  {0xf690, 0x0000dccc},{0xf694, 0x00000000},{0xf698, 0x000020d8},{0xf69c, 0x00000003},
  {0xf6a0, 0x000079aa},{0xf6a4, 0x00008c64},{0xf6a8, 0x000000c3},{0xf6ac, 0x00001482},
  {0xf760, 0x000014aa},{0xf764, 0x00004800},{0xf6b8, 0x0000f002},{0xf76c, 0x000004bf},
  {0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},{0xf6a4, 0x00008c64},
 };


/*serdes4*/
static confcode_mac_regval_t rtl8380_serdes4_rsgmii_6275a[] = {
  {0xf7c4, 0x0000B7C9},{0xf7c8, 0x00001e5e},{0xf7cc, 0x00000a4b},{0xf7d0, 0x00007211},
  {0xf788, 0x000085fa},{0xf78c, 0x00008c6f},{0xf790, 0x0000dccc},{0xf794, 0x00000000},
  {0xf798, 0x000020d8},{0xf79c, 0x00000003},{0xf7a0, 0x000079aa},{0xf7a4, 0x00008c64},
  {0xf7a8, 0x000000c3},{0xf7ac, 0x00001482},{0xf860, 0x000014aa},{0xf864, 0x00004800},
  {0xf7b8, 0x0000f002},{0xf86c, 0x000004bf},{0xf7a4, 0x00008e64},{0xf7a4, 0x00008c64},
 };
static confcode_mac_regval_t rtl8380_serdes4_qsgmii_6275a[] = {
    {0xf798, 0x000020d8},{0xf7a8, 0x000058c7},{0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},
    {0xf7ac, 0x00001482},{0xf780, 0x00004040},{0xf780, 0x00004000},{0xf7a4, 0x00008e64},
    {0xf7a4, 0x00008c64},
};
static confcode_mac_regval_t rtl8380_serdes4_rsgmiip_6275a[] = {
    {0xf798, 0x000020d8},{0xf7a8, 0x000058c7},{0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},
    {0xf7ac, 0x00001482},{0xf780, 0x00004040},{0xf780, 0x00004000},{0xf7a4, 0x00008e64},
    {0xf7a4, 0x00008c64},
};
static confcode_mac_regval_t rtl8380_serdes4_fiber_6275a[] = {
    {0xf798, 0x000020d8},{0xf7a8, 0x000058c7},{0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},
    {0xf7ac, 0x00001482},{0xf780, 0x00004040},{0xf780, 0x00004000},{0xf7a4, 0x00008e64},
    {0xf7a4, 0x00008c64},{0xf878, 0x0000071e},{0xef90, 0x0000074d},
};
static confcode_mac_regval_t rtl8380_serdes4_nophy_6275a[] = {
    {0xf798, 0x000020d8},{0xf7a8, 0x000058c7},{0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},
    {0xf7ac, 0x00001482},{0xf780, 0x00004040},{0xf780, 0x00004000},{0xf7a4, 0x00008e64},
    {0xf7a4, 0x00008c64},{0xf878, 0x0000071e},{0xef90, 0x0000074d},
};


/*serdes5*/
static confcode_mac_regval_t rtl8380_serdes5_rsgmii_6275a[] = {
  {0xf8c4, 0x00004209},{0xf8c8, 0x0000c1f5},{0xf8cc, 0x00000000},{0xf8d0, 0x00000000},
  {0xf888, 0x000085fa},{0xf88c, 0x00000000},{0xf890, 0x0000dccc},{0xf894, 0x00000000},
  {0xf898, 0x000020d8},{0xf89c, 0x00000003},{0xf8a0, 0x000079aa},{0xf8a4, 0x00008c64},
  {0xf8a8, 0x000000c3},{0xf8ac, 0x00001482},{0xf960, 0x000014aa},{0xf964, 0x00004800},
  {0xf8b8, 0x0000f002},{0xf96c, 0x000004bf},{0xf8a4, 0x00008e64},{0xf8a4, 0x00008c64},
};
static confcode_mac_regval_t rtl8380_serdes5_fiber_6275a[] = {
  {0xf8c4, 0x00004209},{0xf8c8, 0x0000c1f5},{0xf8cc, 0x00000000},{0xf8d0, 0x00000000},
  {0xf888, 0x000088ff},{0xf88c, 0x00000000},{0xf890, 0x0000dccc},{0xf894, 0x00000000},
  {0xf898, 0x0000861b},{0xf89c, 0x00000003},{0xf8a0, 0x000079aa},{0xf8a4, 0x00008822},
  {0xf8a8, 0x000000c3},{0xf8ac, 0x00001482},{0xf960, 0x000014aa},{0xf964, 0x00000300},
  {0xf8b8, 0x0000f002},{0xf96c, 0x000004bf},{0xf8a4, 0x00008a22},{0xf8a4, 0x00008822},
  {0xf978, 0x0000071e},{0xf190, 0x0000074d},
};
static confcode_mac_regval_t rtl8380_serdes5_nophy_6275a[] = {
  {0xf8c4, 0x00004209},{0xf8c8, 0x0000c1f5},{0xf8cc, 0x00000000},{0xf8d0, 0x00000000},
  {0xf888, 0x000088ff},{0xf88c, 0x00000000},{0xf890, 0x0000dccc},{0xf894, 0x00000000},
  {0xf898, 0x0000861b},{0xf89c, 0x00000003},{0xf8a0, 0x000079aa},{0xf8a4, 0x00008822},
  {0xf8a8, 0x000000c3},{0xf8ac, 0x00001482},{0xf960, 0x000014aa},{0xf964, 0x00000300},
  {0xf8b8, 0x0000f002},{0xf96c, 0x000004bf},{0xf8a4, 0x00008a22},{0xf8a4, 0x00008822},
  {0xf978, 0x0000071e},{0xf190, 0x0000074d},
};



/*******************************RTL8380 Version-B*********************************/
/*Serdes0,1*/
static confcode_mac_regval_t rtl8380_serdes01_rsgmiip_6275b[] = {
  {0x0ff8, 0xaaaaaaaf},{0xf38c, 0x0000f46f},
  {0xf388, 0x000085fa},{0xf488, 0x000085fa},{0xf398, 0x000020d8},{0xf498, 0x000020d8},
  {0xf3c4, 0x0000B7C9},{0xf4ac, 0x00000482},{0xf4a8, 0x000080c7},{0xf3c8, 0x0000ab8e},
  {0xf3ac, 0x00000482},{0xf3cc, 0x000024ab},{0xf4c4, 0x00004208},{0xf4c8, 0x0000c208},
  {0xf464, 0x00000303},{0xf564, 0x00000303},{0xf3b8, 0x0000FCC2},{0xf4b8, 0x0000FCC2},
  {0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},{0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},
 };

static confcode_mac_regval_t rtl8380_serdes01_qsgmii_6275b[] = {
  {0x0ff8, 0xaaaaaaaf},{0xf38c, 0x0000f46f},
  {0xf388, 0x000085fa},{0xf488, 0x000085fa},{0xf398, 0x000020d8},{0xf498, 0x000020d8},
  {0xf3c4, 0x0000B7C9},{0xf4ac, 0x00000482},{0xf4a8, 0x000080c7},{0xf3c8, 0x0000ab8e},
  {0xf3ac, 0x00000482},{0xf3cc, 0x000024ab},{0xf4c4, 0x00004208},{0xf4c8, 0x0000c208},
  {0xf464, 0x00000303},{0xf564, 0x00000303},{0xf3b8, 0x0000FCC2},{0xf4b8, 0x0000FCC2},
  {0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},{0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},
  };

static confcode_mac_regval_t rtl8380_serdes01_qsgmii_6275b_8218d_e[] = {
  {0x0ff8, 0xaaaaaaaf},{0xf38c, 0x0000f46f},
  {0xf388, 0x000085fa},{0xf488, 0x000085fa},{0xf398, 0x000020d8},{0xf498, 0x000020d8},
  {0xf3c4, 0x0000B7C9},{0xf4ac, 0x00000482},{0xf4a8, 0x000080c7},{0xf3c8, 0x0000ab8e},
  {0xf3ac, 0x00000482},{0xf3cc, 0x000024ab},{0xf4c4, 0x00004208},{0xf4c8, 0x0000c208},
  {0xf464, 0x00002403},{0xf564, 0x00002403},{0xf3b8, 0x0000FCC2},{0xf4b8, 0x0000FCC2},
  {0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},{0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},
  };

static confcode_mac_regval_t rtl8380_serdes01_xsmii_6275b[] = {
  {0x0ff8, 0xaaaaaaab},{0xf3c4, 0x000003c7},{0xf388, 0x000085fa},{0xf38c, 0x00008c6f},
  {0xf390, 0x0000dccc},{0xf394, 0x00000000},{0xf398, 0x00003600},{0xf39c, 0x00000003},
  {0xf3a0, 0x000079aa},{0xf3a4, 0x00008c64},{0xf3a8, 0x000000c3},{0xf3ac, 0x00001482},
  {0xf460, 0x000014aa},{0xf464, 0x00004800},{0xf3b8, 0x0000f002},{0xf46c, 0x000004bf},
  {0xf488, 0x000085fa},{0xf48c, 0x00000000},{0xf490, 0x0000dccc},{0xf494, 0x00000000},
  {0xf498, 0x00003600},{0xf49c, 0x00000003},{0xf4a0, 0x000079aa},{0xf4a4, 0x00008c64},
  {0xf4a8, 0x000000c3},{0xf4ac, 0x00001482},{0xf560, 0x000014aa},{0xf564, 0x00004800},
  {0xf4b8, 0x0000f002},{0xf56c, 0x000004bf},{0xf3a4, 0x00008e64},{0xf3a4, 0x00008c64},
  {0xf4a4, 0x00008e64},{0xf4a4, 0x00008c64},
 };


/*Serdes2,3*/
static confcode_mac_regval_t rtl8380_serdes23_rsgmiip_6275b[] = {
  {0xf58c, 0x0000f46d},
  {0xf588, 0x000085fa},{0xf688, 0x000085fa},{0xf788, 0x000085fa},{0xf598, 0x000020d8},
  {0xf698, 0x000020d8},{0xf5c4, 0x0000B7C9},{0xf5c8, 0x0000ab8e},{0xf5ac, 0x00000482},
  {0xf6ac, 0x00000482},{0xf5cc, 0x000024ab},{0xf6c4, 0x00004208},{0xf6c8, 0x0000c208},
  {0xf664, 0x00000303},{0xf764, 0x00000303},{0xf5b8, 0x0000FCC2},{0xf6b8, 0x0000FCC2},
  {0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},{0xf6a4, 0x00008c64},
 };

static confcode_mac_regval_t rtl8380_serdes23_qsgmii_6275b[] = {
  {0xf58c, 0x0000f46d},
  {0xf588, 0x000085fa},{0xf688, 0x000085fa},{0xf788, 0x000085fa},{0xf598, 0x000020d8},
  {0xf698, 0x000020d8},{0xf5c4, 0x0000B7C9},{0xf5c8, 0x0000ab8e},{0xf5ac, 0x00000482},
  {0xf6ac, 0x00000482},{0xf5cc, 0x000024ab},{0xf6c4, 0x00004208},{0xf6c8, 0x0000c208},
  {0xf664, 0x00000303},{0xf764, 0x00000303},{0xf5b8, 0x0000FCC2},{0xf6b8, 0x0000FCC2},
  {0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},{0xf6a4, 0x00008c64},
};

static confcode_mac_regval_t rtl8380_serdes23_qsgmii_6275b_8218d_e[] = {
  {0xf58c, 0x0000f46d},
  {0xf588, 0x000085fa},{0xf688, 0x000085fa},{0xf788, 0x000085fa},{0xf598, 0x000020d8},
  {0xf698, 0x000020d8},{0xf5c4, 0x0000B7C9},{0xf5c8, 0x0000ab8e},{0xf5ac, 0x00000482},
  {0xf6ac, 0x00000482},{0xf5cc, 0x000024ab},{0xf6c4, 0x00004208},{0xf6c8, 0x0000c208},
  {0xf664, 0x00002403},{0xf764, 0x00002403},{0xf5b8, 0x0000FCC2},{0xf6b8, 0x0000FCC2},
  {0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},{0xf6a4, 0x00008c64},
};

static confcode_mac_regval_t rtl8380_serdes23_xsmii_6275b[] = {
  {0xf5c4, 0x000003c7},{0xf588, 0x000085fa},{0xf58c, 0x00008c6f},{0xf590, 0x0000dccc},
  {0xf594, 0x00000000},{0xf598, 0x00003600},{0xf59c, 0x00000003},{0xf5a0, 0x000079aa},
  {0xf5a4, 0x00008c64},{0xf5a8, 0x000000c3},{0xf5ac, 0x00001482},{0xf660, 0x000014aa},
  {0xf664, 0x00004800},{0xf5b8, 0x0000f002},{0xf66c, 0x000004bf},{0xf688, 0x000085fa},
  {0xf68c, 0x00000000},{0xf690, 0x0000dccc},{0xf694, 0x00000000},{0xf698, 0x00003600},
  {0xf69c, 0x00000003},{0xf6a0, 0x000079aa},{0xf6a4, 0x00008c64},{0xf6a8, 0x000000c3},
  {0xf6ac, 0x00001482},{0xf760, 0x000014aa},{0xf764, 0x00004800},{0xf6b8, 0x0000f002},
  {0xf76c, 0x000004bf},{0xf5a4, 0x00008e64},{0xf5a4, 0x00008c64},{0xf6a4, 0x00008e64},
  {0xf6a4, 0x00008c64},
 };

/*serdes4*/
/*2.5G*/
static confcode_mac_regval_t rtl8380_serdes4_rsgmii_6275b[] = {
  {0xf7c4, 0x0000086f},{0xf788, 0x000085fa},{0xf78c, 0x00008c6f},{0xf790, 0x0000dccc},
  {0xf794, 0x00000000},{0xf798, 0x00003600},{0xf79c, 0x00000003},{0xf7a0, 0x000079aa},
  {0xf7a4, 0x00008c64},{0xf7a8, 0x000000c3},{0xf7ac, 0x00001482},{0xf860, 0x000014aa},
  {0xf864, 0x00004800},{0xf7b8, 0x0000f002},{0xf86c, 0x000004bf},{0xf7a4, 0x00008e64},
  {0xf7a4, 0x00008c64},
 };

/*5G*/
static confcode_mac_regval_t rtl8380_serdes4_qsgmii_6275b[] = {
  {0xf78c, 0x0000f46d},{0xf788, 0x000085fa},{0xf7ac, 0x00000482},{0xf798, 0x000020d8},
  {0xf7a8, 0x000058c7},{0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},{0xf864, 0x00000303},
  {0xf7b8, 0x0000FCC2},{0xf7a4, 0x00008e64},{0xf7a4, 0x00008c64},
};

/*5G*/
static confcode_mac_regval_t rtl8380_serdes4_rsgmiip_6275b[] = {
  {0xf78c, 0x0000f46d},{0xf788, 0x000085fa},{0xf7ac, 0x00000482},{0xf798, 0x000020d8},
  {0xf7a8, 0x000058c7},{0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},{0xf864, 0x00000303},
  {0xf7b8, 0x0000FCC2},{0xf7a4, 0x00008e64},{0xf7a4, 0x00008c64},
};


/*1.25G*/
static confcode_mac_regval_t rtl8380_serdes4_fiber_6275b[] = {
  {0xf788, 0x000085fa},{0xf7ac, 0x00001482},{0xf798, 0x000020d8},{0xf7a8, 0x000000c3},
  {0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},{0xf864, 0x00000303},{0xf84c, 0x00000001},
  {0xf7b8, 0x0000FCC2},{0xf7a8, 0x000000c3},{0xf7a4, 0x00008e66},{0xf7a4, 0x00008c66},
};

static confcode_mac_regval_t rtl8380_serdes4_nophy_6275b[] = {
  {0xf788, 0x000085fa},{0xf7ac, 0x00001482},{0xf798, 0x000020d8},{0xf7a8, 0x000000c3},
  {0xf7c4, 0x0000B7C9},{0xf7c8, 0x0000ab8e},{0xf864, 0x00000303},{0xf84c, 0x00000001},
  {0xf7b8, 0x0000FCC2},{0xf7a4, 0x00008e64},{0xf7a4, 0x00008c64},
};

/*serdes5*/
/*2.5G*/
static confcode_mac_regval_t rtl8380_serdes5_rsgmii_6275b[] = {
  {0xf888, 0x000085fa},{0xf88c, 0x00000000},{0xf890, 0x0000dccc},{0xf894, 0x00000000},
  {0xf898, 0x00003600},{0xf89c, 0x00000003},{0xf8a0, 0x000079aa},{0xf8a4, 0x00008c64},
  {0xf8a8, 0x000000c3},{0xf8ac, 0x00001482},{0xf960, 0x000014aa},{0xf964, 0x00004800},
  {0xf8b8, 0x0000f002},{0xf96c, 0x000004bf},{0xf8a4, 0x00008e64},{0xf8a4, 0x00008c64},
};

/*1.25G*/
static confcode_mac_regval_t rtl8380_serdes5_fiber_6275b[] = {
  {0xf888, 0x000085fa},{0xf88c, 0x00000000},{0xf890, 0x0000dccc},{0xf894, 0x00000000},
  {0xf898, 0x00003600},{0xf89c, 0x00000003},{0xf8a0, 0x000079aa},{0xf8a4, 0x00008c64},
  {0xf8a8, 0x000000c3},{0xf8ac, 0x00001482},{0xf960, 0x000014aa},{0xf964, 0x00000303},
  {0xf94c, 0x00000001},{0xf8b8, 0x0000f002},{0xf96c, 0x000004bf},{0xf8a8, 0x000000c3},
  {0xf8a4, 0x00008e66},{0xf8a4, 0x00008c66},
};

/*1.25G*/
static confcode_mac_regval_t rtl8380_serdes5_nophy_6275b[] = {
  {0xf888, 0x000085fa},{0xf88c, 0x00000000},{0xf890, 0x0000dccc},{0xf894, 0x00000000},
  {0xf898, 0x00003600},{0xf89c, 0x00000003},{0xf8a0, 0x000079aa},{0xf8a4, 0x00008c64},
  {0xf8a8, 0x000000c3},{0xf8ac, 0x00001482},{0xf960, 0x000014aa},{0xf964, 0x00000303},
  {0xf94c, 0x00000001},{0xf8b8, 0x0000f002},{0xf96c, 0x000004bf},{0xf8a4, 0x00008e64},
  {0xf8a4, 0x00008c64},
};


static confcode_mac_regval_t rtl8380_serdes_soft_reset_release[] = {
  {0xe78c, 0x00007106},{0xe98c, 0x00007106},{0xeb8c, 0x00007106},{0xed8c, 0x00007106},
  {0xef8c, 0x00007106},{0xf18c, 0x00007106},
};

/* Function Name:
 *      dal_maple_construct_serdesConfig_init
 * Description:
 *      Serdes Configuration code that connect with RTL8380
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */

void dal_maple_construct_serdesConfig_init(uint32 unit)
{
    uint32 i,sdsId;
    uint32 value;
    uint32 value_temp;
    uint32 sds_power_down_value;
    uint32 polar_rx = 0;
    uint32 polar_tx = 0;
    uint32 sds_reg0[] = {
        MAPLE_SDS0_REG0r,
        MAPLE_SDS1_REG0r,
        MAPLE_SDS2_REG0r,
        MAPLE_SDS3_REG0r,
        MAPLE_SDS4_REG0r,
        MAPLE_SDS5_REG0r,
    };

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    /* Back up serdes power down value */
     ioal_mem32_read(unit, 0x0034, &sds_power_down_value);

    /*serdes software reset take own ship*/
    for (i=0; i<(sizeof(rtl8380_serdes_soft_reset_take)/sizeof(confcode_mac_regval_t)); i++)
    {
        ioal_mem32_write(unit, rtl8380_serdes_soft_reset_take[i].reg,  rtl8380_serdes_soft_reset_take[i].val);
        osal_time_mdelay(1);
    }

    osal_time_mdelay(10);

    /* Serdes polarity settings */
    HWP_SDS_TRAVS(unit, sdsId)
    {
        polar_rx = (HWP_SDS_RX_POLARITY(unit, sdsId) == SERDES_POLARITY_CHANGE) ? 1 : 0;
        polar_tx = (HWP_SDS_TX_POLARITY(unit, sdsId) == SERDES_POLARITY_CHANGE) ? 1 : 0;

        reg_read(unit, sds_reg0[sdsId], &value);

        reg_field_set(unit, sds_reg0[sdsId], MAPLE_INV_HSIf, &polar_rx, &value); /* INV_HSI */
        reg_field_set(unit, sds_reg0[sdsId], MAPLE_INV_HSOf, &polar_tx, &value); /* INV_HSO */

        reg_write(unit, sds_reg0[sdsId], &value);
    }

    /*Serdes Common Patch*/
    for (i=0; i<(sizeof(rtl8380_serdes_common_patch)/sizeof(confcode_mac_regval_t)); i++)
    {
        ioal_mem32_write(unit, rtl8380_serdes_common_patch[i].reg,  rtl8380_serdes_common_patch[i].val);
        osal_time_mdelay(1);
    }

     /* Enable Internal Read/Write */
     ioal_mem32_write(unit, 0x0058, 0x00000003);


    /******************************MAC Serdes Interface Settings***************************/
    /*Set Serdes4 MAC Interface register*/
     ioal_mem32_read(unit, 0x005c, &value);

     value &= (~0x7);

    if(HWP_SDS_MODE(unit,4) == RTK_MII_1000BX_FIBER)
    {
         value |= 0x1;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_100BX_FIBER)
    {
         value |= 0x1;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_SGMII)
    {
         value |= 0x2;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII)
    {
         value |= 0x3;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_HISGMII)
    {
         value |= 0x4;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII_PLUS)
    {
         value |= 0x5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_QSGMII)
    {
         value |= 0x5;
    }
    else
    {
        /*Do nothing, means disable Serdes4*/
    }
    ioal_mem32_write(unit, 0x005c, value);


    /*Set Serdes5 MAC Interface register*/
     ioal_mem32_read(unit, 0x005c, &value);

     value &= (~0x38);
    if(HWP_SDS_MODE(unit,5) == RTK_MII_1000BX_FIBER)
    {
         value |= 0x1<<3;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_100BX_FIBER)
    {
         value |= 0x1<<3;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_SGMII)
    {
         value |= 0x2<<3;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_RSGMII)
    {
         value |= 0x3<<3;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_HISGMII)
    {
         value |= 0x4<<3;
    }
    else
    {
        /*Do nothing, means disable Serdes5*/
    }
    ioal_mem32_write(unit, 0x005c, value);

    /****************************** Serdes Module Settings***************************/
    /*Set Serdes0-Serdes5 Module Mode*/
    /*Serdes0-Serdes3*/
    value  = 0;
    for(i = 0; i < 4; i++)
    {
        if(HWP_SDS_MODE(unit,i) == RTK_MII_RSGMII_PLUS)
        {
             value |= 0x0<<(25-5*i);
        }
        else if(HWP_SDS_MODE(unit,i) == RTK_MII_QSGMII)
        {
             value |= 0x6<<(25-5*i);
        }
        else if(HWP_SDS_MODE(unit,i) == RTK_MII_XSMII)
        {
             value |= 0x9<<(25-5*i);
        }
        else
        {
            /*Do nothing, means disable this Serdes*/
        }
    }

    /*Serdes4*/
    if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII_PLUS)
    {
         value |= 0x0<<5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII)
    {
         value |= 0x1<<5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_SGMII)
    {
         value |= 0x2<<5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_1000BX_FIBER)
    {
         value |= 0x4<<5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_100BX_FIBER)
    {
         value |= 0x5<<5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_QSGMII)
    {
         value |= 0x6<<5;
    }
    else if(HWP_SDS_MODE(unit,4) == RTK_MII_HISGMII)
    {
         value |= 0x12<<5;
    }
    else
    {
        /*Do nothing, means disable this Serdes*/
    }

    /*Serdes5*/
    if(HWP_SDS_MODE(unit,5) == RTK_MII_RSGMII)
    {
         value |= 0x1<<0;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_SGMII)
    {
         value |= 0x2<<0;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_1000BX_FIBER)
    {
         value |= 0x4<<0;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_100BX_FIBER)
    {
         value |= 0x5<<0;
    }
    else if(HWP_SDS_MODE(unit,5) == RTK_MII_HISGMII)
    {
         value |= 0x12<<0;
    }
    else
    {
        /*Do nothing, means disable this Serdes*/
    }
    ioal_mem32_write(unit, 0x0028, value);


    if(RTL8380_VERSION_A == rtl8380_ver)
    {
        /* Serdes0, 1*/
        if(HWP_SDS_MODE(unit,0) == RTK_MII_RSGMII_PLUS)
        {
            /*Save 0xbb000FF8 bit4-bit32 from SOC*/
            ioal_mem32_read(unit, 0x0FF8, &value);
            value &= 0xFFFFFFF0;

            /*Save 0xbb000FF8 bit0-bit3 from Array*/
            value_temp = rtl8380_serdes01_rsgmiip_6275a[0].val;
            value_temp &= 0x0000000F;

            /*Reconstruct data and write back into array*/
            value |= value_temp;
            rtl8380_serdes01_rsgmiip_6275a[0].val = value;

            for (i=0; i<(sizeof(rtl8380_serdes01_rsgmiip_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes01_rsgmiip_6275a[i].reg,  rtl8380_serdes01_rsgmiip_6275a[i].val);

                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,0) == RTK_MII_QSGMII)
        {

            /*Save 0xbb000FF8 bit4-bit32 from SOC*/
            ioal_mem32_read(unit, 0x0FF8, &value);
            value &= 0xFFFFFFF0;

            /*Save 0xbb000FF8 bit0-bit3 from Array*/
            value_temp = rtl8380_serdes01_qsgmii_6275a[0].val;
            value_temp &= 0x0000000F;

            /*Reconstruct data and write back into array*/
            value |= value_temp;
            rtl8380_serdes01_qsgmii_6275a[0].val = value;

            for (i=0; i<(sizeof(rtl8380_serdes01_qsgmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes01_qsgmii_6275a[i].reg,  rtl8380_serdes01_qsgmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,0) == RTK_MII_XSMII)
        {

            /*Save 0xbb000FF8 bit4-bit32 from SOC*/
            ioal_mem32_read(unit, 0x0FF8, &value);
            value &= 0xFFFFFFF0;

            /*Save 0xbb000FF8 bit0-bit3 from Array*/
            value_temp = rtl8380_serdes01_xsmii_6275a[1].val;
            value_temp &= 0x0000000F;

            /*Reconstruct data and write back into array*/
            value |= value_temp;
            rtl8380_serdes01_xsmii_6275a[1].val = value;


            for (i=0; i<(sizeof(rtl8380_serdes01_xsmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes01_xsmii_6275a[i].reg,  rtl8380_serdes01_xsmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else
        {
            /*Do nothing*/
        }

        /* Serdes2,3*/
        if(HWP_SDS_MODE(unit,2) == RTK_MII_RSGMII_PLUS)
        {
            for (i=0; i<(sizeof(rtl8380_serdes23_rsgmiip_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit,rtl8380_serdes23_rsgmiip_6275a[i].reg,  rtl8380_serdes23_rsgmiip_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,2) == RTK_MII_QSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes23_qsgmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes23_qsgmii_6275a[i].reg,  rtl8380_serdes23_qsgmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,2) == RTK_MII_XSMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes23_xsmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes23_xsmii_6275a[i].reg,  rtl8380_serdes23_xsmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else
        {
            /*Do nothing*/
        }

        /* Serdes4*/
        if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII_PLUS)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_rsgmiip_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_rsgmiip_6275a[i].reg,  rtl8380_serdes4_rsgmiip_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_rsgmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_rsgmii_6275a[i].reg,  rtl8380_serdes4_rsgmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_SGMII)
        {
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_1000BX_FIBER)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_fiber_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_fiber_6275a[i].reg,  rtl8380_serdes4_fiber_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_100BX_FIBER)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_fiber_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit,rtl8380_serdes4_fiber_6275a[i].reg,  rtl8380_serdes4_fiber_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_QSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_qsgmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_qsgmii_6275a[i].reg,  rtl8380_serdes4_qsgmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_HISGMII)
        {
        }
        else
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_nophy_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_nophy_6275a[i].reg,  rtl8380_serdes4_nophy_6275a[i].val);
                osal_time_mdelay(1);
            }
        }

        /*Serdes5*/
        if(HWP_SDS_MODE(unit,5) == RTK_MII_RSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_rsgmii_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_rsgmii_6275a[i].reg,  rtl8380_serdes5_rsgmii_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,5) == RTK_MII_SGMII)
        {
        }
        else if(HWP_SDS_MODE(unit,5) == RTK_MII_1000BX_FIBER)
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_fiber_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_fiber_6275a[i].reg,  rtl8380_serdes5_fiber_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,5) == RTK_MII_100BX_FIBER)
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_fiber_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_fiber_6275a[i].reg,  rtl8380_serdes5_fiber_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,5) == RTK_MII_HISGMII)
        {
        }
        else
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_nophy_6275a)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_nophy_6275a[i].reg,  rtl8380_serdes5_nophy_6275a[i].val);
                osal_time_mdelay(1);
            }
        }
    }
    else
    {
        /* Serdes0, 1*/
        if(HWP_SDS_MODE(unit,0) == RTK_MII_RSGMII_PLUS)
        {
            /*Save 0xbb000FF8 bit4-bit32 from SOC*/
            ioal_mem32_read(unit, 0x0FF8, &value);
            value &= 0xFFFFFFF0;

            /*Save 0xbb000FF8 bit0-bit3 from Array*/
            value_temp = rtl8380_serdes01_rsgmiip_6275b[0].val;
            value_temp &= 0x0000000F;

            /*Reconstruct data and write back into array*/
            value |= value_temp;
            rtl8380_serdes01_rsgmiip_6275b[0].val = value;

            for (i=0; i<(sizeof(rtl8380_serdes01_rsgmiip_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes01_rsgmiip_6275b[i].reg,  rtl8380_serdes01_rsgmiip_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,0) == RTK_MII_QSGMII)
        {
            /*Save 0xbb000FF8 bit4-bit32 from SOC*/
            ioal_mem32_read(unit, 0x0FF8, &value);
            value &= 0xFFFFFFF0;

            if((HWP_SDS_ID2PHYMODEL(unit, 0) == RTK_PHYTYPE_RTL8218D) || (HWP_SDS_ID2PHYMODEL(unit, 0) == RTK_PHYTYPE_RTL8218E))
            {
                /*Save 0xbb000FF8 bit0-bit3 from Array*/
                value_temp = rtl8380_serdes01_qsgmii_6275b_8218d_e[0].val;
                value_temp &= 0x0000000F;

                /*Reconstruct data and write back into array*/
                value |= value_temp;
                rtl8380_serdes01_qsgmii_6275b_8218d_e[0].val = value;

                for (i=0; i<(sizeof(rtl8380_serdes01_qsgmii_6275b_8218d_e)/sizeof(confcode_mac_regval_t)); i++)
                {
                    ioal_mem32_write(unit, rtl8380_serdes01_qsgmii_6275b_8218d_e[i].reg,  rtl8380_serdes01_qsgmii_6275b_8218d_e[i].val);
                    osal_time_mdelay(1);
                }
            }
            else
            {
                /*Save 0xbb000FF8 bit0-bit3 from Array*/
                value_temp = rtl8380_serdes01_qsgmii_6275b[0].val;
                value_temp &= 0x0000000F;

                /*Reconstruct data and write back into array*/
                value |= value_temp;
                rtl8380_serdes01_qsgmii_6275b[0].val = value;

                for (i=0; i<(sizeof(rtl8380_serdes01_qsgmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
                {
                    ioal_mem32_write(unit, rtl8380_serdes01_qsgmii_6275b[i].reg,  rtl8380_serdes01_qsgmii_6275b[i].val);
                    osal_time_mdelay(1);
                }
            }
        }
        else if(HWP_SDS_MODE(unit,0) == RTK_MII_XSMII)
        {

            /*Save 0xbb000FF8 bit4-bit32 from SOC*/
            ioal_mem32_read(unit, 0x0FF8,&value);
            value &= 0xFFFFFFF0;

            /*Save 0xbb000FF8 bit0-bit3 from Array*/
            value_temp = rtl8380_serdes01_xsmii_6275b[0].val;
            value_temp &= 0x0000000F;

            /*Reconstruct data and write back into array*/
            value |= value_temp;
            rtl8380_serdes01_xsmii_6275b[0].val = value;

            for (i=0; i<(sizeof(rtl8380_serdes01_xsmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes01_xsmii_6275b[i].reg,  rtl8380_serdes01_xsmii_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else
        {
            /*Do nothing*/
        }

        /* Serdes2,3*/
        if(HWP_SDS_MODE(unit,2) == RTK_MII_RSGMII_PLUS)
        {
            for (i=0; i<(sizeof(rtl8380_serdes23_rsgmiip_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes23_rsgmiip_6275b[i].reg,  rtl8380_serdes23_rsgmiip_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,2) == RTK_MII_QSGMII)
        {
            if((HWP_SDS_ID2PHYMODEL(unit, 2) == RTK_PHYTYPE_RTL8218D) || (HWP_SDS_ID2PHYMODEL(unit, 2) == RTK_PHYTYPE_RTL8218E))
            {
                for (i=0; i<(sizeof(rtl8380_serdes23_qsgmii_6275b_8218d_e)/sizeof(confcode_mac_regval_t)); i++)
                {
                    ioal_mem32_write(unit, rtl8380_serdes23_qsgmii_6275b_8218d_e[i].reg,  rtl8380_serdes23_qsgmii_6275b_8218d_e[i].val);
                    osal_time_mdelay(1);
                }
            }
            else
            {
                for (i=0; i<(sizeof(rtl8380_serdes23_qsgmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
                {
                    ioal_mem32_write(unit, rtl8380_serdes23_qsgmii_6275b[i].reg,  rtl8380_serdes23_qsgmii_6275b[i].val);
                    osal_time_mdelay(1);
                }
            }
        }
        else if(HWP_SDS_MODE(unit,2) == RTK_MII_XSMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes23_xsmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes23_xsmii_6275b[i].reg,  rtl8380_serdes23_xsmii_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else
        {
            /*Do nothing*/
        }

        /* Serdes4*/
        if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII_PLUS)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_rsgmiip_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_rsgmiip_6275b[i].reg,  rtl8380_serdes4_rsgmiip_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_RSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_rsgmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_rsgmii_6275b[i].reg,  rtl8380_serdes4_rsgmii_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if((HWP_SDS_MODE(unit,4) == RTK_MII_1000BX_FIBER)||(HWP_SDS_MODE(unit,4) == RTK_MII_SGMII))
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_fiber_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_fiber_6275b[i].reg,  rtl8380_serdes4_fiber_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_100BX_FIBER)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_fiber_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_fiber_6275b[i].reg,  rtl8380_serdes4_fiber_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_QSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_qsgmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_qsgmii_6275b[i].reg,  rtl8380_serdes4_qsgmii_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,4) == RTK_MII_HISGMII)
        {
        }
        else
        {
            for (i=0; i<(sizeof(rtl8380_serdes4_nophy_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes4_nophy_6275b[i].reg,  rtl8380_serdes4_nophy_6275b[i].val);
                osal_time_mdelay(1);
            }
        }

        /*Serdes5*/
        if(HWP_SDS_MODE(unit,5) == RTK_MII_RSGMII)
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_rsgmii_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_rsgmii_6275b[i].reg,  rtl8380_serdes5_rsgmii_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        if((HWP_SDS_MODE(unit,5) == RTK_MII_1000BX_FIBER)||(HWP_SDS_MODE(unit,5) == RTK_MII_SGMII))
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_fiber_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_fiber_6275b[i].reg,  rtl8380_serdes5_fiber_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,5) == RTK_MII_100BX_FIBER)
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_fiber_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_fiber_6275b[i].reg,  rtl8380_serdes5_fiber_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
        else if(HWP_SDS_MODE(unit,5) == RTK_MII_HISGMII)
        {
        }
        else
        {
            for (i=0; i<(sizeof(rtl8380_serdes5_nophy_6275b)/sizeof(confcode_mac_regval_t)); i++)
            {
                ioal_mem32_write(unit, rtl8380_serdes5_nophy_6275b[i].reg,  rtl8380_serdes5_nophy_6275b[i].val);
                osal_time_mdelay(1);
            }
        }
    }

    /* Serdes reset*/
    for (i = 0; i < DAL_MAPLE_MAX_SDS_NUM; i++)
    {
        /*tx & rx reset*/
        ioal_mem32_read(unit, 0xE780 + i * 0x200, &value);
        value &= ~(3UL << 0);
        ioal_mem32_write(unit, 0xE780 + i * 0x200, value);
        osal_time_mdelay(1);
    }
    for (i = 0; i < DAL_MAPLE_MAX_SDS_NUM; i++)
    {
        ioal_mem32_read(unit, 0xE780 + i * 0x200, &value);
        value |= (3UL << 0);
        ioal_mem32_write(unit, 0xE780 + i * 0x200, value);
        osal_time_mdelay(1);
    }

    /*serdes software reset release own ship*/
    for (i=0; i<(sizeof(rtl8380_serdes_soft_reset_release)/sizeof(confcode_mac_regval_t)); i++)
    {
        ioal_mem32_write(unit, rtl8380_serdes_soft_reset_release[i].reg,  rtl8380_serdes_soft_reset_release[i].val);
        osal_time_mdelay(1);
    }

    /*************************MAC Serdes force linkdown Settings**********************/
    ioal_mem32_write(unit, 0x0034, sds_power_down_value);


    ioal_mem32_read(unit, 0x0034, &value);
    HWP_SDS_TRAVS(unit, sdsId)
    {
        value &= (~(0x1<<sdsId));

        if(RTK_MII_DISABLE == HWP_SDS_MODE(unit, sdsId))
        {
            value |= 0x1<<sdsId;
        }
    }
    ioal_mem32_write(unit, 0x0034, value);

    /*Power off Fiber to avoid led light*/
    if((HWP_SDS_MODE(unit,4) == RTK_MII_1000BX_FIBER) ||(HWP_SDS_MODE(unit,4) == RTK_MII_100BX_FIBER))
    {
        uint32 serdes_reg;
        uint32 serdes_val;
        serdes_reg = 0xf800;
        ioal_mem32_read(unit, serdes_reg, &serdes_val);
        serdes_val |= (1 << 11);
        ioal_mem32_write(unit, serdes_reg, serdes_val);
    }

    if((HWP_SDS_MODE(unit,5) == RTK_MII_1000BX_FIBER) ||(HWP_SDS_MODE(unit,5) == RTK_MII_100BX_FIBER))
    {
        uint32 serdes_reg;
        uint32 serdes_val;
        serdes_reg = 0xf900;
        ioal_mem32_read(unit, serdes_reg, &serdes_val);
        serdes_val |= (1 << 11);
        ioal_mem32_write(unit, serdes_reg, serdes_val);
    }

    return;
} /* end of dal_maple_construct_serdesConfig_init */

