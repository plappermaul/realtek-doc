/*
 * Copyright (C) 2022 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PHY 8224 Driver APIs.
 *
 * Feature : PHY 8224 Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phy_rtl8224.h>
#include <hal/phy/phy_rtl8224_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/time.h>
#include <osal/memory.h>
#include <hwp/hwp_util.h>

/*
 * Symbol Definition
 */

#define PHY_8224_LOG    LOG_INFO

/*
 * Data Declaration
 */
rt_phyInfo_t phy_8224_info =
{
    .phy_num    = 1,
    .eth_type   = HWP_2_5GE,
    .isComboPhy = { 0, 0, 0, 0 },
    .flags      = RTK_PHYINFO_FLAG_NONE | RTK_PHYINFO_FLAG_NO_RES_REG | RTK_PHYINFO_FLAG_1G_MMD_CFG,

    .xGePhyLocalDuplexAbilityDev            = 31,
    .xGePhyLocalDuplexAbilityAddr           = 0xA400,
    .xGePhyLocalDuplexAbilityBit            = 8,

    .xGePhyLocal1000MSpeedAbilityDev        = 31,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0xA412,
    .xGePhyLocal1000MSpeedAbilityBit        = 9,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 31,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0xA414,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 11,
};

uint32 port_map_ledpad[RTL8224_PORT_NUM][RTL8224_PER_PORT_MAX_LED] =
{
   {0,   1,  2,  3},
   {6,   7,  8,  9},
   {12, 13, 14, 15},
   {18, 19, 20, 21}
};

uint32 port_tapbin_reg[RTK_MAX_NUM_OF_DEV][RTK_MAX_PORT_PER_UNIT];
uint32 port_tapbin_field[RTK_MAX_NUM_OF_DEV][RTK_MAX_PORT_PER_UNIT];


/*
 * Macro Declaration
 */

#define RTL8224_SWREG_ADDR(_addr)            ((_addr) & (0xffff))

/*
 * Function Declaration
 */
uint32
_phy_8224_mask(uint8 msb, uint8 lsb)
{
    uint32  val = 0;
    uint8   i = 0;

    for (i = lsb; i <= msb; i++)
    {
        val |= (1 << i);
    }
    return val;
}

int32
_phy_8224_mask_get(uint8 msb, uint8 lsb, uint32 *mask)
{
    if ((msb > 31) || (lsb > 31) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }
    *mask = _phy_8224_mask(msb, lsb);

    return RT_ERR_OK;
}

static int32 _phy_8224_intReg_get(uint32 unit, rtk_port_t port, uint32 reg_addr, uint8 msb, uint8 lsb, uint32 *pData)
{
    int32           ret;
    rtk_port_t      base_port = 0;
    uint32          addr;
    uint32          l_data = 0;
    uint32          h_data = 0;
    uint32          reg_data = 0;
    uint32          mask = 0;

    if ((msb > 31) || (lsb > 31) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }

    base_port = HWP_PHY_BASE_MACID(unit, port);

    /* Reg [15:0]*/
    addr = RTL8224_SWREG_ADDR(reg_addr);
    ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, addr, &l_data);
    if(ret != RT_ERR_OK)
        return ret;

    /* Reg [31:16]*/
    ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, (addr+1), &h_data);
    if(ret != RT_ERR_OK)
        return ret;

    ret = _phy_8224_mask_get(msb, lsb, &mask);
    if(ret != RT_ERR_OK)
        return ret;

    reg_data = ((h_data << 16) | (l_data));
    *pData = (((reg_data) & (mask)) >> lsb);

    return ret;
}

static int32 _phy_8224_intRegBit_get(uint32 unit, rtk_port_t port, uint32 reg_addr, uint8 bit, uint32 *pData)
{
    if (bit > 31)
    {
        return RT_ERR_FAILED;
    }
    return _phy_8224_intReg_get(unit, port, reg_addr, bit, bit, pData);
}


static int32 _phy_8224_intReg_set(uint32 unit, rtk_port_t port, uint32 reg_addr, uint8 msb, uint8 lsb, uint32 data)
{
    int32           ret;
    rtk_port_t      base_port = 0;
    uint32          addr;
    uint32          l_data = 0;
    uint32          h_data = 0;
    uint32          reg_data = 0;
    uint32          mask = 0;

    if ((msb > 31) || (lsb > 31) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }

    base_port = HWP_PHY_BASE_MACID(unit, port);

    /* Reg [15:0]*/
    addr = RTL8224_SWREG_ADDR(reg_addr);
    ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, addr, &l_data);
    if(ret != RT_ERR_OK)
        return ret;

    /* Reg [31:16]*/
    ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, (addr+1), &h_data);
    if(ret != RT_ERR_OK)
        return ret;

    ret = _phy_8224_mask_get(msb, lsb, &mask);
    if(ret != RT_ERR_OK)
        return ret;

    reg_data = ((h_data << 16) | (l_data));
    reg_data &= ~(mask);
    reg_data |= (((data) << lsb) & mask);

    l_data = (reg_data & 0xffff);
    h_data = ((reg_data >> 16)& 0xffff);

    ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, addr, l_data);
    if(ret != RT_ERR_OK)
        return ret;

    ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, (addr+1), h_data);
    if(ret != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}


static int32 _phy_8224_intRegBit_set(uint32 unit, rtk_port_t port, uint32 reg_addr, uint8 bit, uint32 data)
{
    if (bit > 31)
    {
        return RT_ERR_FAILED;
    }
    return _phy_8224_intReg_set(unit, port, reg_addr, bit, bit, data);
}

static int32 _phy_8224_sdsRegField_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint8 msb, uint8 lsb, uint32 data)
{
    int32           ret;
    rtk_port_t      base_port = 0;
    uint32          reg_data = 0;
    uint32          mask = 0;

    if ((msb > 15) || (lsb > 15) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }

    base_port = HWP_PHY_BASE_MACID(unit, port);

    phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, &reg_data);

    ret = _phy_8224_mask_get(msb, lsb, &mask);
    if(ret != RT_ERR_OK)
        return ret;

    reg_data &= ~(mask);
    reg_data |= (((data) << lsb) & mask);

    phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, reg_data);

    return RT_ERR_OK;
}

static int32 _phy_8224_sdsRegFieldBit_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint8 bit, uint32 data)
{
    int32           ret;

    ret = _phy_8224_sdsRegField_set(unit, port, sdsPage, sdsReg, bit, bit, data);

    return ret;
}

static int32 _phy_8224_sdsRegField_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint8 msb, uint8 lsb, uint32 *pData)
{
    int32           ret;
    rtk_port_t      base_port = 0;
    uint32          reg_data = 0;
    uint32          mask = 0;

    if ((msb > 15) || (lsb > 15) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }

    base_port = HWP_PHY_BASE_MACID(unit, port);

    phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, &reg_data);

    ret = _phy_8224_mask_get(msb, lsb, &mask);
    if(ret != RT_ERR_OK)
        return ret;

    *pData = (((reg_data) & (mask)) >> lsb);

    return RT_ERR_OK;
}

static int32 _phy_8224_sdsRegFieldBit_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint8 bit, uint32 *pData)
{
    int32           ret;

    ret = _phy_8224_sdsRegField_get(unit, port, sdsPage, sdsReg, bit, bit, pData);

    return ret;
}

static int32 _phy_8224_sram_get(uint32 unit, rtk_port_t port, uint32 sramAddr, uint32 *pData)
{
    int32           ret;

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xa436, sramAddr);
    if(ret != RT_ERR_OK)
        return ret;

    ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xa438, pData);
    if(ret != RT_ERR_OK)
        return ret;

    return ret;
}


int32
_phy_8224_interrupt_init(uint32 unit, rtk_port_t port)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData;
    rtk_port_t      base_port;
    uint8           port_offset;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    /* Clean GPHY Interrupt Pending bits*/
    /* Read and Clear*/
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA43A, &phyData)) != RT_ERR_OK)
        return ret;
    /* Clean ISR_EXT_GPHY */
    /* Write 1 to clear */
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_GPHY, port_offset, 1)) != RT_ERR_OK)
       return ret;


    if (port == base_port)
    {
        /* Disable IMR_EXT_GPHY */
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_IMR_EXT_GPHY, RTL8224_IMR_EXT_GPHY_3_0_MSB, RTL8224_IMR_EXT_GPHY_3_0_OFFSET, 0)) != RT_ERR_OK)
            return ret;

        /* Disable Thermal/PTP 1588/MACsec IMR */
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;

        /* Disable WOL IMR */
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_IMR_EXT_WOL, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;

        /* Disable RLFD Interrupt pin output */
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_IMR_EXT_RLFD, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_IMR_INT_RLFD, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;
        /* Enable RLFD IMR */
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_RLFD_CTRL_REG, RTL8224_RLFD_SEL_OFFSET, RTL8224_RLFD_EN_OFFSET, 1)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_CFG_PHY_POLL_CMD1_REG, RTL8224_CMD_RD_EN_3_0_MSB, RTL8224_CMD_RD_EN_3_0_LSB, 7)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
            return ret;
        phyData |= (0x1 << PHY_8224_RLFD_ENABLE_OFFSET);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
            return ret;

        /* Enable Global IMR for interrupt GPIO output, high level trigger*/
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_ISR_SW_INT_MODE, RTL8224_SWITCH_IE_OFFSET, RTL8224_SWITCH_IE_OFFSET, 1)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_ISR_SW_INT_MODE, RTL8224_SWITCH_INT_MODE_MSB, RTL8224_SWITCH_INT_MODE_OFFSET, PHY_8224_INT_MODE_LOW_LEVEL)) != RT_ERR_OK)
            return ret;
    }


    return ret;
}

int32
_phy_8224_gphyNormal_Intr_status_get(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0;
    uint32          bitmap = 0;
    rtk_port_t      base_port;
    uint8            port_offset;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;


    /* Read and Clear */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA43A, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_2)
        bitmap |= RTK_PHY_CTRL_INTR_NEXT_PAGE_RECV;
    if (phyData & BIT_3)
        bitmap |= RTK_PHY_CTRL_INTR_AN_COMPLETE;
    if (phyData & BIT_4)
        bitmap |= RTK_PHY_CTRL_INTR_LINK_CHANGE;
    if (phyData & BIT_9)
        bitmap |= RTK_PHY_CTRL_INTR_ALDPS_STATE_CHANGE;
    if (phyData & BIT_11)
        bitmap |= RTK_PHY_CTRL_INTR_FATAL_ERROR;

    /* Clean ISR_EXT_GPHY */
    /* Write 1 to clear */
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_GPHY, port_offset, 1)) != RT_ERR_OK)
       return ret;

    *pStatus = bitmap;
    return ret;
}

int32
_phy_8224_an_restart(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_12) /*AN is enabled*/
    {
        phyData |= BIT_9; /*AN restart*/
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 0, phyData)) != RT_ERR_OK)
            return ret;
    }
    return ret;
}

int32
_phy_8224_led_mode_set(uint32 unit, rtk_port_t port, uint32 led_index, uint32 mode)
{
    int32           ret = 0;
    uint32          set = 0;
    uint32          phyData = 0;
    uint32          reg_addr, msb_bit, lsb_bit;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (led_index > 4)
        return RT_ERR_INPUT;

    set = 0; /* used set 0*/

    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G           ) ? (PHY_8224_LED_MODE_2P5G           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_LITE      ) ? (PHY_8224_LED_MODE_2P5G_LITE      ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_1G             ) ? (PHY_8224_LED_MODE_1G             ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_500M           ) ? (PHY_8224_LED_MODE_500M           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_100M           ) ? (PHY_8224_LED_MODE_100M           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_LINK_EN        ) ? (PHY_8224_LED_MODE_LINK           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_LINK_FLASH     ) ? (PHY_8224_LED_MODE_LINK_FLASH     ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_RX_ACT         ) ? (PHY_8224_LED_MODE_RX_FLASH       ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_TX_ACT         ) ? (PHY_8224_LED_MODE_TX_FLASH       ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_DUPLEX         ) ? (PHY_8224_LED_MODE_DUPLEX         ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_TRAINING       ) ? (PHY_8224_LED_MODE_TRAINING       ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_MASTER         ) ? (PHY_8224_LED_MODE_MASTER         ) : (0);

    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_FLASH     ) ? (PHY_8224_LED_MODE_2P5G           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_FLASH     ) ? (PHY_8224_LED_MODE_ACT_FLASH      ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH) ? (PHY_8224_LED_MODE_2P5G_LITE      ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_FLASH     ) ? (PHY_8224_LED_MODE_ACT_FLASH      ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_1G_FLASH       ) ? (PHY_8224_LED_MODE_1G             ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_1G_FLASH       ) ? (PHY_8224_LED_MODE_ACT_FLASH      ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_500M_FLASH     ) ? (PHY_8224_LED_MODE_500M           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_500M_FLASH     ) ? (PHY_8224_LED_MODE_ACT_FLASH      ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_100M_FLASH     ) ? (PHY_8224_LED_MODE_100M           ) : (0);
    phyData |= (mode & RTK_PHY_CTRL_LED_MODE_100M_FLASH     ) ? (PHY_8224_LED_MODE_ACT_FLASH      ) : (0);


    reg_addr = (RTL8224_LED3_2_SET0_CTRL0_REG - (set * RTL8224_LED_SET_CTRL_OFFSET));
    switch (led_index)
    {
        case 0:
            lsb_bit = RTL8224_SET_LED0_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED0_SEL0_MSB;
        break;
        case 1:
            lsb_bit = RTL8224_SET_LED1_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED1_SEL0_MSB;
        break;
        case 2:
            lsb_bit = RTL8224_SET_LED2_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED2_SEL0_MSB;
        break;
        case 3:
            lsb_bit = RTL8224_SET_LED3_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED3_SEL0_MSB;
        break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, msb_bit, lsb_bit, phyData)) != RT_ERR_OK)
        return ret;

    lsb_bit = (RTL8224_LED0_SET_PSEL_FILED_LEN * port_offset);
    msb_bit = (RTL8224_LED0_SET_PSEL_FILED_LEN + lsb_bit);
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_LED_PORT_SET_SEL_CTRL, msb_bit, lsb_bit, set)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_8224_led_mode_get(uint32 unit, rtk_port_t port, uint32 led_index, uint32 *pMode)
{
    int32   ret = 0;
    uint32  tmp = 0;
    uint32  set = 0;
    uint32  phyData = 0;
    uint32  reg_addr, msb_bit, lsb_bit;
    uint32  flash_act = 0;

    if (led_index > 5)
        return RT_ERR_INPUT;

    set = 0; /* used set 0*/
    reg_addr = (RTL8224_LED3_2_SET0_CTRL0_REG - (set * RTL8224_LED_SET_CTRL_OFFSET));

    switch (led_index)
    {
        case 0:
            lsb_bit = RTL8224_SET_LED0_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED0_SEL0_MSB;
        break;
        case 1:
            lsb_bit = RTL8224_SET_LED1_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED1_SEL0_MSB;
        break;
        case 2:
            lsb_bit = RTL8224_SET_LED2_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED2_SEL0_MSB;
        break;
        case 3:
            lsb_bit = RTL8224_SET_LED3_SEL0_OFFSET;
            msb_bit = RTL8224_SET_LED3_SEL0_MSB;
        break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, msb_bit, lsb_bit, &phyData)) != RT_ERR_OK)
        return ret;

    if(phyData & PHY_8224_LED_MODE_ACT_FLASH)
        flash_act = 1;

    if(phyData & PHY_8224_LED_MODE_2P5G)
    {
        if(flash_act == 1)
            tmp |= RTK_PHY_CTRL_LED_MODE_2P5G_FLASH;
        else
            tmp |= RTK_PHY_CTRL_LED_MODE_2P5G;
    }
    if(phyData & PHY_8224_LED_MODE_2P5G_LITE)
    {
        if(flash_act == 1)
            tmp |= RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH;
        else
            tmp |= RTK_PHY_CTRL_LED_MODE_2P5G_LITE;
    }
    if(phyData & PHY_8224_LED_MODE_1G)
    {
        if(flash_act == 1)
            tmp |= RTK_PHY_CTRL_LED_MODE_1G_FLASH;
        else
            tmp |= RTK_PHY_CTRL_LED_MODE_1G;
    }
    if(phyData & PHY_8224_LED_MODE_500M)
    {
        if(flash_act == 1)
            tmp |= RTK_PHY_CTRL_LED_MODE_500M_FLASH;
        else
            tmp |= RTK_PHY_CTRL_LED_MODE_500M;
    }
    if(phyData & PHY_8224_LED_MODE_100M)
    {
        if(flash_act == 1)
            tmp |= RTK_PHY_CTRL_LED_MODE_100M_FLASH;
        else
            tmp |= RTK_PHY_CTRL_LED_MODE_100M;
    }
    tmp |= (phyData & PHY_8224_LED_MODE_RX_FLASH         ) ? (RTK_PHY_CTRL_LED_MODE_RX_ACT         ) : (0);
    tmp |= (phyData & PHY_8224_LED_MODE_TX_FLASH         ) ? (RTK_PHY_CTRL_LED_MODE_TX_ACT         ) : (0);
    tmp |= (phyData & PHY_8224_LED_MODE_DUPLEX           ) ? (RTK_PHY_CTRL_LED_MODE_DUPLEX         ) : (0);
    tmp |= (phyData & PHY_8224_LED_MODE_TRAINING         ) ? (RTK_PHY_CTRL_LED_MODE_MASTER         ) : (0);
    tmp |= (phyData & PHY_8224_LED_MODE_MASTER           ) ? (RTK_PHY_CTRL_LED_MODE_TRAINING       ) : (0);

    *pMode = tmp;
    return ret;
}

int32
_phy_8224_led_config_blink_rate_set(uint32 unit, rtk_port_t port, uint32 value)
{
    int32   ret = 0;
    uint32  phyData = 0;

    switch (value)
    {
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS:
            phyData = 1;
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS:
            phyData = 2;
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS:
            phyData = 3;
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS:
            phyData = 4;
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_512MS:
            phyData = 5;
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_1024MS:
            phyData = 6;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_LED_GLB_CTRL_REG, RTL8224_BLINK_TIME_SEL_MSB, RTL8224_BLINK_TIME_SEL_OFFSET, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_8224_led_config_blink_rate_get(uint32 unit, rtk_port_t port, uint32 *pValue)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_LED_GLB_CTRL_REG, RTL8224_BLINK_TIME_SEL_MSB, RTL8224_BLINK_TIME_SEL_OFFSET, &phyData)) != RT_ERR_OK)
        return ret;

    switch (phyData)
    {
        case 1:
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS;
            break;
        case 2:
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS;
            break;
        case 3:
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS;
            break;
        case 4:
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS;
            break;
        case 5:
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_512MS;
            break;
        case 6:
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_1024MS;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}

int32
_phy_8224_led_config_active_low_set(uint32 unit, rtk_port_t port, uint32 index, uint32 value)
{
    int32           ret = 0;
    uint32          led_pad = 0;
    uint32          low_actve = 0;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (index > 5)
        return RT_ERR_INPUT;

    led_pad = port_map_ledpad[port_offset][index];

    if(value != 0)
        low_actve = 1;

    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_LED_GLB_ACTIVE_REG, led_pad, low_actve)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_8224_led_config_active_low_get(uint32 unit, rtk_port_t port, uint32 index, uint32 *pValue)
{
    int32           ret = 0;
    uint32          led_pad = 0;
    uint32          phyValue = 0;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (index > 5)
        return RT_ERR_INPUT;

    led_pad = port_map_ledpad[port_offset][index];

    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_LED_GLB_ACTIVE_REG, led_pad, &phyValue)) != RT_ERR_OK)
        return ret;

    *pValue = phyValue;
    return ret;
}

int32
_phy_8224_led_config_force_set(uint32 unit, rtk_port_t port, uint32 index, uint32 value)
{
    int32           ret = 0;
    uint32          reg_addr, regData = 0, regEN = 0;
    uint32          msb_bit, lsb_bit;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (index >= RTL8224_PER_PORT_MAX_LED)
        return RT_ERR_INPUT;

    lsb_bit = (RTL8224_SW_CTRL_LED_EN_OFFSET + (port_offset * RTL8224_SW_CTRL_LED_EN_FIELD_LEN));
    msb_bit = (lsb_bit + RTL8224_SW_CTRL_LED_EN_MSB);

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_LED_PORT_SW_EN_CTRL, msb_bit, lsb_bit, &regEN)) != RT_ERR_OK)
        return ret;

    switch (value)
    {
        case RTK_PHY_CTRL_LED_CFG_FORCE_OFF:
            regEN |= (1 << index);
            regData = RTL8224_SW_CTRL_MODE_OFF;
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_ON:
            regEN |= (1 << index);
            regData = RTL8224_SW_CTRL_MODE_LIGHT;
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_BLINK:
            regEN |= (1 << index);
            regData = RTL8224_SW_CTRL_BLINK_256MS;
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE:
            regEN &= ~(1 << index);
            regData = RTL8224_SW_CTRL_MODE_OFF;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_LED_PORT_SW_EN_CTRL, msb_bit, lsb_bit, regEN)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_LED_PORT_SW_CTRL + (RTL8224_LED_PORT_SW_CTRL_PORT_OFFSET * port_offset));
    lsb_bit = (RTL8224_SW_LED_MODE_OFFSET + (index * RTL8224_SW_LED_MODE_FIELD_LEN));
    msb_bit = (lsb_bit + RTL8224_SW_LED_MODE_MSB);

    if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, msb_bit, lsb_bit, regData)) != RT_ERR_OK)
        return ret;
    return ret;
}

int32
_phy_8224_led_config_force_get(uint32 unit, rtk_port_t port, uint32 index, uint32 *pValue)
{
    int32           ret = 0;
    uint32          reg_addr, regData = 0, regEN = 0;
    uint32          msb_bit, lsb_bit;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (index >= RTL8224_PER_PORT_MAX_LED)
        return RT_ERR_INPUT;

    lsb_bit = (RTL8224_SW_CTRL_LED_EN_OFFSET + (port_offset * RTL8224_SW_CTRL_LED_EN_FIELD_LEN));
    msb_bit = (lsb_bit + RTL8224_SW_CTRL_LED_EN_MSB);
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_LED_PORT_SW_EN_CTRL, msb_bit, lsb_bit, &regEN)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_LED_PORT_SW_CTRL + (RTL8224_LED_PORT_SW_CTRL_PORT_OFFSET * port_offset));
    lsb_bit = (RTL8224_SW_LED_MODE_OFFSET + (index * RTL8224_SW_LED_MODE_FIELD_LEN));
    msb_bit = (lsb_bit + RTL8224_SW_LED_MODE_MSB);

    if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, msb_bit, lsb_bit, &regData)) != RT_ERR_OK)
        return ret;

    switch (regData)
    {
        case RTL8224_SW_CTRL_MODE_OFF:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_OFF;
            break;
        case RTL8224_SW_CTRL_MODE_LIGHT:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_ON;
            break;
        case RTL8224_SW_CTRL_BLINK_256MS:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_BLINK;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if((regEN & (1 << index)) == 0)
        *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE;

    return ret;
}

/* value is 19 bit-lenght = (temperature * 1024), sign bit = 18 */
int32
_phy_8224_temperature_threshold_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 val)
{
    int32           ret = 0;
    uint32          sign =0;      /* [18],1-bit */
    uint32          integer = 0;  /* [17:10],8-bit */

    sign     = (val & 0x40000) >> 18;
    integer  = (val & 0x3FC00) >> 10;

    if (val & (~0x7FFFF))
    {
        RT_LOG(PHY_8224_LOG, (MOD_HAL|MOD_PHY), "U%u P%u 8224temp threshold set t:%d v:0x%X\n", unit, port, ctrl_type, val);
        return RT_ERR_INPUT;
    }

    switch (ctrl_type)
    {

        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_HIGH_THR_SIGN_OFFSET, sign)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_HIGH_THR_MSB, RTL8224_TM_HIGH_THR_LSB, integer)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_LOW_THR_SIGN_OFFSET, sign)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_LOW_THR_MSB, RTL8224_TM_LOW_THR_LSB, integer)) != RT_ERR_OK)
                return ret;

            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}

int32
_phy_8224_temperature_threshold_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32* val)
{
    int32  ret = 0;
    uint32 sign =0;
    uint32 integer = 0;
    uint32 temp = 0;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
            if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_HIGH_THR_SIGN_OFFSET, &sign)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_HIGH_THR_MSB, RTL8224_TM_HIGH_THR_LSB, &integer)) != RT_ERR_OK)
                return ret;

            temp = ((sign) << 8) | (integer & 0x00FF);
            break;

        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
            if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_LOW_THR_SIGN_OFFSET, &sign)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_LOW_THR_MSB, RTL8224_TM_LOW_THR_LSB, &integer)) != RT_ERR_OK)
                return ret;

            temp = ((sign) << 8) | (integer & 0x00FF);
            break;

        default:
            return RT_ERR_INPUT;
    }

    *val = temp;
    return ret;
}

int32
_phy_8224_dbgCount_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData;

    /* Disable MIB counter */
    phyData = 0;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_MIB_GLOBAL_CONFIG, 11, 11, phyData)) != RT_ERR_OK)
        return ret;

    /* Clear PHY 3~0 counter*/
    phyData = 0xf;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_MIB_GLOBAL_CONFIG, 3, 0, phyData)) != RT_ERR_OK)
        return ret;

    /* Enable MIB counter */
    phyData = 1;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_MIB_GLOBAL_CONFIG, 11, 11, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_8224_sdsAllAm_set
 * Description:
 *      clear alignment marker0-3, and each AM has m0-3
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      value     - m0/m1/m2 value
 * Output:
 *      NA
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsAllAm_set(uint32 unit, rtk_port_t port, uint32 value)
{
    rtk_port_t    base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    /* AM0 */
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG0_REG, PHY_8224_AM0_M0_HIGH_BIT, PHY_8224_AM0_M0_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG0_REG, PHY_8224_AM0_M1_HIGH_BIT, PHY_8224_AM0_M1_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG1_REG, PHY_8224_AM0_M2_HIGH_BIT, PHY_8224_AM0_M2_LOW_BIT, value);
    /* AM1 */
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG1_REG, PHY_8224_AM1_M0_HIGH_BIT, PHY_8224_AM1_M0_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG2_REG, PHY_8224_AM1_M1_HIGH_BIT, PHY_8224_AM1_M1_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG2_REG, PHY_8224_AM1_M2_HIGH_BIT, PHY_8224_AM1_M2_LOW_BIT, value);
    /* AM2 */
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG3_REG, PHY_8224_AM2_M0_HIGH_BIT, PHY_8224_AM2_M0_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG3_REG, PHY_8224_AM2_M1_HIGH_BIT, PHY_8224_AM2_M1_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG4_REG, PHY_8224_AM2_M2_HIGH_BIT, PHY_8224_AM2_M2_LOW_BIT, value);
    /* AM3 */
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG4_REG, PHY_8224_AM3_M0_HIGH_BIT, PHY_8224_AM3_M0_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG5_REG, PHY_8224_AM3_M1_HIGH_BIT, PHY_8224_AM3_M1_LOW_BIT, value);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PAGE, PHY_8224_AM_CFG5_REG, PHY_8224_AM3_M2_HIGH_BIT, PHY_8224_AM3_M2_LOW_BIT, value);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_sdsExtra_set
 * Description:
 *      SerDes setting for Realtek MAC
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      NA
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsExtra_set(uint32 unit, rtk_port_t port)
{
    rtk_port_t    base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PERIOD_PAGE, PHY_8224_TGR_PRO_0_REG17, PHY_8224_USXG_V2P1P2_RXOFF_HIGH_BIT, PHY_8224_USXG_V2P1P2_RXOFF_LOW_BIT, 0x1);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_NWAY_OPCODE_PAGE, PHY_8224_TGR_PRO_1_REG19, PHY_8224_USXG_RXDV_OFF_HIGH_BIT, PHY_8224_USXG_RXDV_OFF_LOW_BIT, 0x1);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_portPatch_backup
 * Description:
 *      Backup PHY port patch.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
phy_8224_portPatch_backup(uint32 unit)
{
    uint32  port = 0, regVal = 0;
    uint32  field;

    HWP_PORT_TRAVS(unit, port)
    {
        switch (HWP_PHY_MODEL_BY_PORT(unit, port))
        {
            case RTK_PHYTYPE_RTL8224:
                hal_miim_mmd_write(unit, port, 31, 0xa436, 0x8377);
                hal_miim_mmd_read(unit, port, 31, 0xa438, &regVal);
                port_tapbin_reg[unit][port] = regVal;
                field = ((regVal & RTL8224_TAPBIN_DEF_BITMASK) >> RTL8224_TAPBIN_DEF_OFFSET);
                port_tapbin_field[unit][port] = field;
                break;
            default:
                break;
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_portPatch_restore
 * Description:
 *      Restore PHY port patch.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
phy_8224_portPatch_restore(uint32 unit)
{
    uint32  port = 0, regVal = 0;
    uint32  field;

    HWP_PORT_TRAVS(unit, port)
    {
        switch (HWP_PHY_MODEL_BY_PORT(unit, port))
        {
            case RTK_PHYTYPE_RTL8224:
                hal_miim_mmd_write(unit, port, 31, 0xa436, 0x8377);
                hal_miim_mmd_write(unit, port, 31, 0xa438, port_tapbin_reg[unit][port]);
                field = port_tapbin_field[unit][port];
                regVal = ((field << 12) | (field << 8) | (field << 4) | (field)); 
                hal_miim_mmd_write(unit, port, 31, 0xbce0, regVal);
                break;
            default:
                break;
        }
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      phy_8224_init
 * Description:
 *      Initialize PHY.
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
phy_8224_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;

    /* Disable all interrupt */
    if ((ret = _phy_8224_interrupt_init(unit, port)) != RT_ERR_OK)
        return ret;

    /* Disable Thermal Sensor output latch*/
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_TM0_CTRL2, RTL8224_REG_EN_LATCH_OFFSET, 0)) != RT_ERR_OK)
        return ret;

    /* Initial MIB counter */
    if ((ret = _phy_8224_dbgCount_init(unit, port)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_intrEnable_set
 * Description:
 *      Set PHY interrupt enable state.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      enable  - Enable/disable state for specified interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData, bit_idx;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (enable == DISABLED)
        phyData = 0;
    else
        phyData = 1;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
        case RTK_PHY_INTR_STATUS_FATAL_ERROR:
            bit_idx = (RTL8224_IMR_EXT_GPHY_3_0_OFFSET + port_offset);
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IMR_EXT_GPHY, bit_idx, phyData);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            bit_idx = ((RTL8224_IMR_EXT_RLFD_PORT_8_0_OFFSET + port_offset) + RTL8224_IMR_EXT_RLFD_PORT_SHIFT);
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IMR_EXT_RLFD, bit_idx, phyData);
            break;
        case RTK_PHY_INTR_STATUS_TM_LOW:
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_IMR_EXT_TM_LOW_OFFSET, phyData);
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_LOWCMP_EN_OFFSET, phyData);
            break;
        case RTK_PHY_INTR_STATUS_TM_HIGH:
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_IMR_EXT_TM_HIGH_OFFSET, phyData);
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_HIGHCMP_EN_OFFSET, phyData);
            break;
        case RTK_PHY_INTR_STATUS_PTP1588:
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_IMR_EXT_PTP1588_OFFSET, phyData);
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
 *      phy_8224_intrEnable_get
 * Description:
 *      Get PHY interrupt enable state.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 * Output:
 *      pEnable - pointer to status of interrupt enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData, phyData1, bit_idx;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
        case RTK_PHY_INTR_STATUS_FATAL_ERROR:
            bit_idx = (RTL8224_IMR_EXT_GPHY_3_0_OFFSET + port_offset);
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IMR_EXT_GPHY, bit_idx, &phyData);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            bit_idx = ((RTL8224_IMR_EXT_RLFD_PORT_8_0_OFFSET + port_offset) + RTL8224_IMR_EXT_RLFD_PORT_SHIFT);
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IMR_EXT_RLFD, bit_idx, &phyData);
            break;
        case RTK_PHY_INTR_STATUS_TM_LOW:
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_IMR_EXT_TM_LOW_OFFSET, &phyData);
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_LOWCMP_EN_OFFSET, &phyData1);
            break;
        case RTK_PHY_INTR_STATUS_TM_HIGH:
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_IMR_EXT_TM_HIGH_OFFSET, &phyData);
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_TM0_CTRL3, RTL8224_TM_HIGHCMP_EN_OFFSET,&phyData1);
            break;
        case RTK_PHY_INTR_STATUS_PTP1588:
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IMR_EXT_MISC, RTL8224_IMR_EXT_PTP1588_OFFSET, &phyData);
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }

    *pEnable = (phyData) ? (ENABLED) : (DISABLED);
    return ret;
}

/* Function Name:
 *      phy_8224_intrStatus_get
 * Description:
 *      Get specified PHY interrupt status.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 * Output:
 *      pStatus - Pointer to output the value for interrupt status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The PHY interrupt status register is read-clear.
 */
int32
phy_8224_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus)
{
    int32           ret = 0;
    uint32          status = 0;
    uint32          phyData, bit_idx;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            /* Get GPHY */
            if ((ret = _phy_8224_gphyNormal_Intr_status_get(unit, port, &status)) != RT_ERR_OK)
                return ret;
            /* Get TM LOW */
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_LOW_OFFSET, &phyData);
            if (phyData != 0)
                status |= RTK_PHY_CTRL_INTR_TM_LOW;
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_LOW_OFFSET, phyData);
            /* Get TM HIGH */
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_HIGH_OFFSET, &phyData);
            if (phyData != 0)
                status |= RTK_PHY_CTRL_INTR_TM_HIGH;
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_HIGH_OFFSET, phyData);

            pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
            pStatus->statusValue = status;
            break;
        case RTK_PHY_INTR_RLFD:
            bit_idx = (port_offset + RTL8224_ISR_EXT_RLFD_PORT_SHIFT);
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_ISR_EXT_TM_RLFD_REG, bit_idx, &phyData);
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_TM_RLFD_REG, bit_idx, phyData);
            if (phyData != 0)
                status |= RTK_PHY_CTRL_INTR_RLFD;
            pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
            pStatus->statusValue = status;
            break;
        case RTK_PHY_INTR_TM_LOW:
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_LOW_OFFSET, &phyData);
            if (phyData != 0)
                status |= RTK_PHY_CTRL_INTR_TM_LOW;
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_LOW_OFFSET, phyData);
            pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
            pStatus->statusValue = status;
            break;
        case RTK_PHY_INTR_TM_HIGH:
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_HIGH_OFFSET, &phyData);
            if (phyData != 0)
                status |= RTK_PHY_CTRL_INTR_TM_HIGH;
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_MISC, RTL8224_ISR_EXT_TM_HIGH_OFFSET, phyData);
            pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
            pStatus->statusValue = status;
            break;
        case RTK_PHY_INTR_PTP1588:
            bit_idx = (RTL8224_ISR_EXT_PTP1588_OFFSET);
            ret = _phy_8224_intRegBit_get(unit, port, RTL8224_ISR_EXT_MISC, bit_idx, &phyData);
            if (phyData != 0)
                status |= RTK_PHY_CTRL_INTR_PTP1588;
            ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_MISC, bit_idx, phyData);
            pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
            pStatus->statusValue = status;
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }
    return ret;
}


/* Function Name:
*      phy_8224_linkStatus_get
* Description:
*      Get PHY link status from standard register (1.1.2).
* Input:
*      unit    - unit id
*      port    - port id
* Output:
*      pStatus - pointer to the link status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
* Note:
*      The Link Status bit (PMA/PMD status 1 register 1.1.2) has LL (Latching Low) attribute
*      for link failure. Please refer IEEE 802.3 for detailed.
*/
int32
phy_8224_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;
    *pStatus = (phyData & BIT_2) ? PORT_LINKUP : PORT_LINKDOWN;
    return ret;
}


/* Function Name:
 *      phy_8224_media_get
 * Description:
 *      Get PHY media type.
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
phy_8224_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8224_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto-negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto-negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA412, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->Full_1000  = (phyData & BIT_9) ? (1) : (0);

    ret = phy_common_c45_autoNegoAbility_get(unit, port, pAbility);
    return ret;
}

/* Function Name:
 *      phy_8224_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto-negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto-negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 16, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_5 | BIT_6 | BIT_7 | BIT_8 | BIT_10 | BIT_11));
    phyData |= (pAbility->Half_10) ? (BIT_5) : (0);
    phyData |= (pAbility->Full_10) ? (BIT_6) : (0);
    phyData |= (pAbility->Half_100) ? (BIT_7) : (0);
    phyData |= (pAbility->Full_100) ? (BIT_8) : (0);
    phyData |= (pAbility->FC) ? (BIT_10) : (0);
    phyData |= (pAbility->AsyFC) ? (BIT_11) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 16, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_7));
    phyData |= (pAbility->adv_2_5G) ? (BIT_7) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA412, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_9));
    phyData |= (pAbility->Full_1000) ? (BIT_9) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, phyData)) != RT_ERR_OK)
        return ret;

    ret = phy_common_c45_an_restart(unit, port);
    return ret;
}

/* Function Name:
*      phy_8224_autoNegoAbilityPeer_get
* Description:
*      Get ability advertisement from link partner of the specific port.
* Input:
*      unit - unit id
*      port - port id
* Output:
*      pAbility - pointer to PHY auto-negotiation ability
* Return:
*      RT_ERR_OK     - OK
*      RT_ERR_FAILED - invalid parameter
* Note:
*      None
*/
int32
phy_8224_autoNegoAbilityPeer_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{

    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    phy_common_c45_copperPeerAutoNegoAbility_get(unit, port, pAbility);

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA414, &phyData)) != RT_ERR_OK)
        return ret;
    pAbility->Full_1000 = (phyData & BIT_11) ? (1) : (0);
    pAbility->Half_1000 = (phyData & BIT_10) ? (1) : (0);

    return ret;
}

/* Function Name:
 *      phy_8224_autoNegoAbilityLocal_get
 * Description:
 *      Get all abilities of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to output the auto-negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224_autoNegoAbilityLocal_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    osal_memset(pAbility, 0x0, sizeof(rtk_port_phy_ability_t));
    pAbility->FC = 1;
    pAbility->AsyFC = 1;
    pAbility->Half_10 = 1;
    pAbility->Full_10 = 1;
    pAbility->Half_100 = 1;
    pAbility->Full_100 = 1;
    pAbility->Full_1000 = 1;
    pAbility->adv_2_5G = 1;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_duplex_get
 * Description:
 *      Get duplex of the specific port
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
phy_8224_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
        return ret;

    *pDuplex = (phyData & BIT_8) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;

    return ret;
}

/* Function Name:
 *      phy_8224_duplex_set
 * Description:
 *      Set duplex of the specific port
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
phy_8224_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (duplex == PORT_FULL_DUPLEX) ? (phyData | BIT_8) : (phyData & ~BIT_8);

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA400, phyData);
    return ret;
}

/* Function Name:
 *      phy_8224_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* 100M/1000M */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 60, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = (ENABLED == enable) ? (phyData | (BIT_1 | BIT_2)) : (phyData & ~(BIT_1 | BIT_2));
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 60, phyData)) != RT_ERR_OK)
        return ret;

    /* 2p5g */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 62, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = (ENABLED == enable) ? (phyData | (BIT_0)) : (phyData & ~(BIT_0));
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 62, phyData)) != RT_ERR_OK)
        return ret;

    /* 2p5g lite*/
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xa6d8, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = (ENABLED == enable) ? (phyData | (BIT_4)) : (phyData & ~(BIT_4));
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xa6d8, phyData)) != RT_ERR_OK)
        return ret;

    /* 500M */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = (ENABLED == enable) ? (phyData | (BIT_7)) : (phyData & (~BIT_7));
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA428, phyData)) != RT_ERR_OK)
        return ret;

    ret = _phy_8224_an_restart(unit, port);
    return ret;
}

/* Function Name:
 *      phy_8224_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rtk_enable_t ena0 = 0;
    rtk_enable_t ena1 = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 60, &phyData)) != RT_ERR_OK)
        return ret;
    ena0 = ((phyData & (BIT_1 | BIT_2)) == (BIT_1 | BIT_2)) ? ENABLED : DISABLED;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 62, &phyData)) != RT_ERR_OK)
        return ret;
    ena1 = ((phyData & (BIT_0)) == (BIT_0 )) ? ENABLED : DISABLED;

    if (ena0 == ena1)
        *pEnable = ena0;
    else
        *pEnable = DISABLED;
    return ret;
}

/* Function Name:
 *      phy_8224_eeepEnable_set
 * Description:
 *      Set enable status of EEEP function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA44A, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (ENABLED == enable) ? (phyData | (BIT_0 | BIT_1)) : (phyData & ~(BIT_0 | BIT_1));

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA44A, phyData)) != RT_ERR_OK)
        return ret;

    ret = _phy_8224_an_restart(unit, port);
    return ret;
}

/* Function Name:
 *      phy_8224_eeepEnable_get
 * Description:
 *      Get enable status of EEEP function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEEP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA44A, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_0) ? ENABLED : DISABLED;
    return ret;
}

/* Function Name:
 *      phy_8224_crossOverMode_set
 * Description:
 *      Set cross over(MDI/MDI-X) mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8224_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_8 | BIT_9));
    switch (mode)
    {
        case PORT_CROSSOVER_MODE_MDI:
            phyData |= (BIT_8 | BIT_9);
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            phyData |= BIT_9;
            break;
        case PORT_CROSSOVER_MODE_AUTO:
            break;
        default:
            return RT_ERR_FAILED;
    }

    ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA430, phyData);

    return ret;
}


/* Function Name:
 *      phy_8224_crossOverMode_get
 * Description:
 *      Get cross over(MDI/MDI-X) mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8224_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_9)
    {
        if (phyData & BIT_8)
            *pMode = PORT_CROSSOVER_MODE_MDI;
        else
            *pMode = PORT_CROSSOVER_MODE_MDIX;
    }
    else
        *pMode = PORT_CROSSOVER_MODE_AUTO;

    return ret;
}

/* Function Name:
 *      phy_8224_crossOverStatus_get
 * Description:
 *      Get cross over(MDI/MDI-X) status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8224_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA434, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData & (BIT_1)) == (BIT_1))
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
    else
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;

    return ret;
}

/* Function Name:
 *      phy_8224_liteEnable_set
 * Description:
 *      Set the status of Lite speed settings for the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      mode   - Lite speed mode
 *      enable - status of Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8224_liteEnable_set(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if (ENABLED == enable)
    {
        switch (mode)
        {
            case PORT_LITE_1G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_9);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA428, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_2P5G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        switch (mode)
        {
            case PORT_LITE_1G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_9);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA428, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_2P5G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_NOT_SUPPORTED;
        }
    }

    ret = _phy_8224_an_restart(unit, port);
    return ret;
}

/* Function Name:
 *      phy_8224_liteEnable_get
 * Description:
 *      Get the status of Lite speed settings for the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mode    - Lite speed mode
 * Output:
 *      pEnable - pointer to status of Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8224_liteEnable_get(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    switch (mode)
    {
        case PORT_LITE_1G:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
                return ret;
            *pEnable = (phyData & BIT_9) ? (ENABLED):(DISABLED);
            break;
        case PORT_LITE_2P5G:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                return ret;
            *pEnable = (phyData & BIT_0) ? (ENABLED):(DISABLED);
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }
    return ret;
}

/* Function Name:
 *      phy_8224_c45_ieeeTestMode_set
 * Description:
 *      Set test mode for PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      Not support 1G mode
 */
int32
phy_8224_c45_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    int32   ret;
    uint32  phyData = 0;


    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData)) != RT_ERR_OK)
        return ret;

    switch (pTestMode->mode)
    {
        case RTK_PORT_PHY_10G_TEST_MODE_NONE:
            break;
        case RTK_PORT_PHY_10G_TEST_MODE1:
            phyData = (BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE2:
            phyData = (BIT_14 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE3:
            phyData = (BIT_14 | BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE1:
            phyData = (BIT_15 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE2:
            phyData = (BIT_15 | BIT_11);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE3:
            phyData = (BIT_15 | BIT_12);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE4:
            phyData = (BIT_15 | BIT_12 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE5:
            phyData = (BIT_15 | BIT_12 | BIT_11);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE5:
            phyData = (BIT_15 | BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE6:
            phyData = (BIT_15 | BIT_14 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        case RTK_PORT_PHY_10G_TEST_MODE7:
            phyData = (BIT_15 | BIT_14 | BIT_13 | BIT_10);
            ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 132, phyData);
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portRefTime_set
 * Description:
 *      Set the reference time of PHY of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      timeStamp   - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret = RT_ERR_OK;
    uint32 nsec_h = 0;
    uint32 nsec_l = 0;
    uint32 rData = 0;
    uint32 sec_h = 0;
    uint32 sec_l = 0;
    uint32 sec_m = 0;
    WAIT_COMPLETE_VAR()

    /* adjust Timer of PHY */
    sec_l = (timeStamp.sec) & 0xFFFF;
    sec_m = ((timeStamp.sec) >> 16) & 0xFFFF;
    sec_h = ((timeStamp.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = timeStamp.nsec & 0xFFFF;
    nsec_h = timeStamp.nsec >> 16;

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_CMD_MSB, RTL8224_PTP_TIME_CMD_LSB, RTL8224_PTP_TIME_CMD_WRITE)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_NSEC1_R, RTL8224_CFG_TOD_VALID_OFFSET, 1)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_SEC0_R, RTL8224_CFG_PTP_TIME_SEC_L_MSB, RTL8224_CFG_PTP_TIME_SEC_L_LSB, sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_SEC1_R, RTL8224_CFG_PTP_TIME_SEC_M_MSB, RTL8224_CFG_PTP_TIME_SEC_M_LSB, sec_m)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_SEC2_R, RTL8224_CFG_PTP_TIME_SEC_H_MSB, RTL8224_CFG_PTP_TIME_SEC_H_LSB, sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_NSEC0_R, RTL8224_CFG_PTP_TIME_NSEC_L_MSB, RTL8224_CFG_PTP_TIME_NSEC_L_LSB, nsec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_NSEC1_R, RTL8224_CFG_PTP_TIME_NSEC_H_MSB, RTL8224_CFG_PTP_TIME_NSEC_H_LSB, nsec_h)) != RT_ERR_OK)
        return ret;

    if (exec != 0)
    {
        if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_EXEC_OFFSET, 1)) != RT_ERR_OK)
            return ret;
    }

    WAIT_COMPLETE(1000000)
    {
        if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_EXEC_OFFSET, &rData)) != RT_ERR_OK)
            return ret;
        if (rData == 0)
            break;
    }
    if(WAIT_COMPLETE_IS_TIMEOUT())
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "Timeout");
        return RT_ERR_TIMEOUT;
    }

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portRefTime_get
 * Description:
 *      Get the reference time of PHY of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pTimeStamp  - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret = RT_ERR_OK;
    uint32 nsec_h = 0;
    uint32 nsec_l = 0;
    uint32 rData = 0;
    uint32 sec_h = 0;
    uint32 sec_l = 0;
    uint32 sec_m = 0;

    WAIT_COMPLETE_VAR()

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_CMD_MSB, RTL8224_PTP_TIME_CMD_LSB, RTL8224_PTP_TIME_CMD_READ)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_EXEC_OFFSET, 1)) != RT_ERR_OK)
        return ret;

    WAIT_COMPLETE(1000000)
    {
        if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_EXEC_OFFSET, &rData)) != RT_ERR_OK)
            return ret;
        if (rData == 0)
            break;
    }
    if(WAIT_COMPLETE_IS_TIMEOUT())
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "Timeout");
        return RT_ERR_TIMEOUT;
    }

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_SEC_RD0_R, RTL8224_RD_PTP_TIME_SEC_L_MSB, RTL8224_RD_PTP_TIME_SEC_L_LSB, &sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_SEC_RD1_R, RTL8224_RD_PTP_TIME_SEC_M_MSB, RTL8224_RD_PTP_TIME_SEC_M_LSB, &sec_m)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_SEC_RD2_R, RTL8224_RD_PTP_TIME_SEC_H_MSB, RTL8224_RD_PTP_TIME_SEC_H_LSB, &sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_NSEC_RD0_R, RTL8224_RD_PTP_TIME_NSEC_L_MSB, RTL8224_RD_PTP_TIME_NSEC_L_LSB, &nsec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_NSEC_RD0_R, RTL8224_RD_PTP_TIME_NSEC_H_MSB, RTL8224_RD_PTP_TIME_NSEC_H_LSB, &nsec_h)) != RT_ERR_OK)
        return ret;

    pTimeStamp->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pTimeStamp->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of PHY of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      sign        - significant
 *      timeStamp   - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
phy_8224_ptp_portRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret = RT_ERR_OK;
    uint32 nsec_h = 0;
    uint32 nsec_l = 0;
    uint32 rData = 0;
    uint32 sec_h = 0;
    uint32 sec_l = 0;
    uint32 sec_m = 0;
    uint64 sec = 0;
    uint32 nsec = 0;
    WAIT_COMPLETE_VAR()

    if (sign == 0)
    {
        sec = timeStamp.sec;
		nsec = timeStamp.nsec;
    }
    else
    {
        /* adjust Timer of PHY */
        sec = 0 - (timeStamp.sec + 1);
        nsec = 1000000000 - timeStamp.nsec;
    }

    /* adjust Timer of PHY */
    sec_l = (sec) & 0xFFFF;
    sec_m = ((sec) >> 16) & 0xFFFF;
    sec_h = ((sec) >> 32) & 0xFFFF;

    /* convert nsec to 8nsec */
    nsec_l = (nsec) & 0xFFFF;
    nsec_h = ((nsec) >> 16);

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_CMD_MSB, RTL8224_PTP_TIME_CMD_LSB, RTL8224_PTP_TIME_CMD_ADJ)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_SEC0_R, RTL8224_CFG_PTP_TIME_SEC_L_MSB, RTL8224_CFG_PTP_TIME_SEC_L_LSB, sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_SEC1_R, RTL8224_CFG_PTP_TIME_SEC_M_MSB, RTL8224_CFG_PTP_TIME_SEC_M_LSB, sec_m)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_SEC2_R, RTL8224_CFG_PTP_TIME_SEC_H_MSB, RTL8224_CFG_PTP_TIME_SEC_H_LSB, sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_NSEC0_R, RTL8224_CFG_PTP_TIME_NSEC_L_MSB, RTL8224_CFG_PTP_TIME_NSEC_L_LSB, nsec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_NSEC1_R, RTL8224_CFG_PTP_TIME_NSEC_H_MSB, RTL8224_CFG_PTP_TIME_NSEC_H_LSB, nsec_h)) != RT_ERR_OK)
        return ret;

    if (exec != 0)
    {
        if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_EXEC_OFFSET, 1)) != RT_ERR_OK)
            return ret;
    }
    WAIT_COMPLETE(1000000)
    {
        if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TIME_CTRL_R, RTL8224_PTP_TIME_EXEC_OFFSET, &rData)) != RT_ERR_OK)
            return ret;
        if (rData == 0)
            break;
    }
    if(WAIT_COMPLETE_IS_TIMEOUT())
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "Timeout");
        return RT_ERR_TIMEOUT;
    }

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portRefTimeFreq_set
 * Description:
 *      Set the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      freq    - reference time frequency
 *      apply   - if the frequency is applied immediately
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portRefTimeFreq_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply)
{
    int32 ret = RT_ERR_OK;
    uint32 cfg_freq_h = 0;
    uint32 cfg_freq_l = 0;
    uint32 rData = 0;
    WAIT_COMPLETE_VAR()

    cfg_freq_l = freq & 0xFFFF;
    cfg_freq_h = (freq >> 16) & 0xFFFF;

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_FREQ0_R, RTL8224_CFG_PTP_TIME_FREQ0_MSB, RTL8224_CFG_PTP_TIME_FREQ0_LSB, cfg_freq_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_FREQ1_R, RTL8224_CFG_PTP_TIME_FREQ1_MSB, RTL8224_CFG_PTP_TIME_FREQ1_LSB, cfg_freq_h)) != RT_ERR_OK)
        return ret;

    if (apply != 0)
    {
        if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_FREQ1_R, RTL8224_CFG_PTP_TIME_FREQ1_EXEC_OFFSET, 1)) != RT_ERR_OK)
            return ret;
    }

    WAIT_COMPLETE(1000000)
    {
        if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TIME_FREQ1_R, RTL8224_CFG_PTP_TIME_FREQ1_EXEC_OFFSET, &rData)) != RT_ERR_OK)
            return ret;
        if (rData == 0)
            break;
    }
    if(WAIT_COMPLETE_IS_TIMEOUT())
    {
        RT_ERR(RT_ERR_TIMEOUT, (MOD_HAL|MOD_TIME), "Timeout");
        return RT_ERR_TIMEOUT;
    }
    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portRefTimeFreq_get
 * Description:
 *      Get the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pFreqCfg    - pointer to configured reference time frequency
 *      pFreqCur    - pointer to current reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur)
{
    int32 ret = RT_ERR_OK;
    uint32 cfg_freq_h = 0;
    uint32 cfg_freq_l = 0;
    uint32 cur_freq_h = 0;
    uint32 cur_freq_l = 0;

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_FREQ0_R, RTL8224_CFG_PTP_TIME_FREQ0_MSB, RTL8224_CFG_PTP_TIME_FREQ0_LSB, &cur_freq_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_FREQ1_R, RTL8224_CFG_PTP_TIME_FREQ1_MSB, RTL8224_CFG_PTP_TIME_FREQ1_LSB, &cur_freq_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CUR_TIME_FREQ0_R, RTL8224_CUR_PTP_TIME_FREQ0_MSB, RTL8224_CUR_PTP_TIME_FREQ0_LSB, &cfg_freq_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CUR_TIME_FREQ1_R, RTL8224_CUR_PTP_TIME_FREQ1_MSB, RTL8224_CUR_PTP_TIME_FREQ1_LSB, &cfg_freq_h)) != RT_ERR_OK)
        return ret;

    *pFreqCur = (cur_freq_h << 16) | cur_freq_l;
    *pFreqCfg = (cfg_freq_h << 16) | cfg_freq_l;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret = RT_ERR_OK;
    uint32          val = 0;
    uint32          reg_addr;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    val = (enable == ENABLED) ? 1 : 0;

    reg_addr = (RTL8224_P0_PORT_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
    if ((ret = _phy_8224_intRegBit_set(unit, port, reg_addr, RTL8224_P0_CFG_UDP_EN_OFFSET, val)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_set(unit, port, reg_addr, RTL8224_P0_CFG_ETH_EN_OFFSET, val)) != RT_ERR_OK)
        return ret;

    /*set bypass PTP pipe*/
    reg_addr = (RTL8224_P0_MISC_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
    val = (enable == ENABLED) ? 0 : 1;
    if ((ret = _phy_8224_intRegBit_set(unit, port, reg_addr, RTL8224_P0_CFG_BYPASS_OFFSET, val)) != RT_ERR_OK)
        return ret;

    return ret;
}


/* Function Name:
 *      phy_8224_ptp_portPtpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32           ret = RT_ERR_OK;
    uint32          bypass = 0;
    uint32          udp = 0;
    uint32          eth = 0;
    uint32          reg_addr;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_P0_PORT_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
    if ((ret = _phy_8224_intRegBit_get(unit, port, reg_addr, RTL8224_P0_CFG_UDP_EN_OFFSET, &udp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, reg_addr, RTL8224_P0_CFG_ETH_EN_OFFSET, &eth)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_P0_MISC_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
    if ((ret = _phy_8224_intRegBit_get(unit, port, reg_addr, RTL8224_P0_CFG_BYPASS_OFFSET, &bypass)) != RT_ERR_OK)
        return ret;

    if (eth == 1 && eth == udp && bypass == 0)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpTxInterruptStatus_get
 * Description:
 *      Get PTP TX timestamp FIFO non-empty interrupt status of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pIntrSts    - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts)
{
    int32 ret = RT_ERR_OK;
    uint32 status = 0;

    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_MIB_INTR_R, RTL8224_RD_ISR_PTP_OFFSET, &status)) != RT_ERR_OK)
        return ret;

    *pIntrSts = status;
    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpInterruptEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  intr_state;

    intr_state = ((enable == ENABLED) ? 1 : 0);

    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_MIB_INTR_R, RTL8224_CFG_IMR_PTP_OFFSET, intr_state)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpInterruptEnable_get
 * Description:
 *      Get PTP TX timestamp FIFO non-empty interrupt enable status of specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 enable = 0;

    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_MIB_INTR_R, RTL8224_CFG_IMR_PTP_OFFSET, &enable)) != RT_ERR_OK)
        return ret;

    *pEnable = (enable == 1) ? ENABLED : DISABLED;
    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpVlanTpid_set
 * Description:
 *      Set inner/outer TPID of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      type        - vlan type
 *      tpid_idx    - TPID index (INNER_VLAN: 0; OUTER_VLAN:0~3)
 *      tpid        - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpVlanTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid)
{
    int32 ret = RT_ERR_OK;
    uint32 val = tpid & 0xFFFF;

    if (OUTER_VLAN == type)
    {
        switch (tpid_idx)
        {
            case 0:
                if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_OTAG_CONFIG0_R, RTL8224_OTAG_TPID_0_MSB, RTL8224_OTAG_TPID_0_LSB, val)) != RT_ERR_OK)
                    return ret;
                break;
            case 1:
                if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_OTAG_CONFIG1_R, RTL8224_OTAG_TPID_1_MSB, RTL8224_OTAG_TPID_1_LSB, val)) != RT_ERR_OK)
                    return ret;
                break;
            case 2:
                if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_OTAG_CONFIG2_R, RTL8224_OTAG_TPID_2_MSB, RTL8224_OTAG_TPID_2_LSB, val)) != RT_ERR_OK)
                    return ret;
                break;
            case 3:
                if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_OTAG_CONFIG3_R, RTL8224_OTAG_TPID_3_MSB, RTL8224_OTAG_TPID_3_LSB, val)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_ENTRY_INDEX;
        }
    }
    else
    {
        switch (tpid_idx)
        {
            case 0:
                if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_ITAG_CONFIG0_R, RTL8224_ITAG_TPID_0_MSB, RTL8224_ITAG_TPID_0_LSB, val)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_ENTRY_INDEX;
        }
    }

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpVlanTpid_get
 * Description:
 *      Get inner/outer TPID of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      type        - vlan type
 *      tpid_idx    - TPID index (INNER_VLAN: 0; OUTER_VLAN:0~3)
 * Output:
 *      pTpid       - TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpVlanTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    int32 ret = RT_ERR_OK;
    uint32 tpid = 0;

    if (OUTER_VLAN == type)
    {
        switch (tpid_idx)
        {
            case 0:
                if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_OTAG_CONFIG0_R, RTL8224_OTAG_TPID_0_MSB, RTL8224_OTAG_TPID_0_LSB, &tpid)) != RT_ERR_OK)
                    return ret;
                break;
            case 1:
                if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_OTAG_CONFIG1_R, RTL8224_OTAG_TPID_1_MSB, RTL8224_OTAG_TPID_1_LSB, &tpid)) != RT_ERR_OK)
                    return ret;
                break;
            case 2:
                if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_OTAG_CONFIG2_R, RTL8224_OTAG_TPID_2_MSB, RTL8224_OTAG_TPID_2_LSB, &tpid)) != RT_ERR_OK)
                    return ret;
                break;
            case 3:
                if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_OTAG_CONFIG3_R, RTL8224_OTAG_TPID_3_MSB, RTL8224_OTAG_TPID_3_LSB, &tpid)) != RT_ERR_OK)
                    return ret;
                break;
            default:
               return RT_ERR_ENTRY_INDEX;
        }
    }
    else
    {
        switch (tpid_idx)
        {
            case 0:
                if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_ITAG_CONFIG0_R, RTL8224_ITAG_TPID_0_MSB, RTL8224_ITAG_TPID_0_LSB, &tpid)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_ENTRY_INDEX;
        }
    }
    *pTpid = tpid;
    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpOper_set
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit        - unit id
 *      port        - port ID
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    int32 ret = RT_ERR_OK;
    uint32 operation = 0;
    uint32 rising_edge = pOperCfg->rise_tri;
    uint32 falling_edge = pOperCfg->fall_tri;

    switch (pOperCfg->oper)
    {
        case TIME_OPER_START:
            operation = 0;
            break;
        case TIME_OPER_LATCH:
            operation = 1;
            break;
        case TIME_OPER_STOP:
            operation = 2;
            break;
        case TIME_OPER_CMD_EXEC:
            operation = 3;
            break;
        case TIME_OPER_FREQ_APPLY:
            operation = 4;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_OP_CTRL_R, RTL8224_CFG_GPI_OP_MSB, RTL8224_CFG_GPI_OP_LSB, operation)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_OP_CTRL_R, RTL8224_CFG_GPI_RISE_TRIG_OFFSET, ((rising_edge == ENABLED) ? 1 : 0))) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_TIME_OP_CTRL_R, RTL8224_CFG_GPI_FALL_TRIG_OFFSET, ((falling_edge == ENABLED) ? 1 : 0))) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpOper_get
 * Description:
 *      Get the PTP time operation configuration of specific port.
 * Input:
 *      unit        - unit id
 *      port        - port ID
 * Output:
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    int32 ret = RT_ERR_OK;
    uint32 falling_edge = 0;
    uint32 operation = 0;
    uint32 rising_edge = 0;

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_OP_CTRL_R, RTL8224_CFG_GPI_OP_MSB, RTL8224_CFG_GPI_OP_LSB, &operation)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TIME_OP_CTRL_R, RTL8224_CFG_GPI_RISE_TRIG_OFFSET, &rising_edge)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TIME_OP_CTRL_R, RTL8224_CFG_GPI_FALL_TRIG_OFFSET, &falling_edge)) != RT_ERR_OK)
        return ret;

    switch (operation)
    {
        case 0:
            pOperCfg->oper = TIME_OPER_START;
            break;
        case 1:
            pOperCfg->oper = TIME_OPER_LATCH;
            break;
        case 2:
            pOperCfg->oper = TIME_OPER_STOP;
            break;
        case 3:
            pOperCfg->oper = TIME_OPER_CMD_EXEC;
            break;
        case 4:
            pOperCfg->oper = TIME_OPER_FREQ_APPLY;
            break;
        default:
            return RT_ERR_FAILED;
    }

    pOperCfg->rise_tri = (rising_edge == 1) ? ENABLED : DISABLED;
    pOperCfg->fall_tri = (falling_edge == 1) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpLatchTime_get
 * Description:
 *      Get the PTP latched time of specific port.
 * Input:
 *      unit        - unit id
 *      port        - port ID
 * Output:
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime)
{
    int32 ret = RT_ERR_OK;
    uint32 nsec_h = 0;
    uint32 nsec_l = 0;
    uint32 sec_h = 0;
    uint32 sec_l = 0;
    uint32 sec_m = 0;


    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_SEC_RD0_R, RTL8224_RD_PTP_TIME_SEC_L_MSB, RTL8224_RD_PTP_TIME_SEC_L_LSB, &sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_SEC_RD1_R, RTL8224_RD_PTP_TIME_SEC_M_MSB, RTL8224_RD_PTP_TIME_SEC_M_LSB, &sec_m)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_SEC_RD2_R, RTL8224_RD_PTP_TIME_SEC_H_MSB, RTL8224_RD_PTP_TIME_SEC_H_LSB, &sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_NSEC_RD0_R, RTL8224_RD_PTP_TIME_NSEC_L_MSB, RTL8224_RD_PTP_TIME_NSEC_L_LSB, &nsec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_NSEC_RD0_R, RTL8224_RD_PTP_TIME_NSEC_H_MSB, RTL8224_RD_PTP_TIME_NSEC_H_LSB, &nsec_h)) != RT_ERR_OK)
        return ret;

    pLatchTime->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pLatchTime->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return ret;
}


/* Function Name:
 *      phy_8224_ptp_ptpTxTimestampFifo_get
 * Description:
 *      Get the top entry from PTP Tx timstamp FIFO on the dedicated port from the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pTimeEntry  - pointer buffer of TIME timestamp entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_ptpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 msg_type = 0;
    uint32 seq_id_h = 0;
    uint32 seq_id_l = 0;
    uint32 tx_port = 0;
    uint32 tx_timestamp_nsec_h = 0;
    uint32 tx_timestamp_nsec_l = 0;
    uint32 tx_timestamp_sec_h = 0;
    uint32 tx_timestamp_sec_l = 0;
    uint32 valid = 0;

    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD0_R, RTL8224_RD_TX_TIMESTAMP_VALID_OFFSET, &valid)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD0_R, RTL8224_RD_PORT_ID_MSB, RTL8224_RD_PORT_ID_LSB, &tx_port)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD0_R, RTL8224_RD_MSG_TYPE_MSB, RTL8224_RD_MSG_TYPE_LSB, &msg_type)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD0_R, RTL8224_RD_SEQ_ID_H_MSB, RTL8224_RD_SEQ_ID_H_LSB, &seq_id_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD1_R, RTL8224_RD_SEQ_ID_L_MSB, RTL8224_RD_SEQ_ID_L_LSB, &seq_id_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD1_R, RTL8224_RD_TX_TIMESTAMP_SEC_H_MSB, RTL8224_RD_TX_TIMESTAMP_SEC_H_LSB, &tx_timestamp_sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD2_R, RTL8224_RD_TX_TIMESTAMP_SEC_L_MSB, RTL8224_RD_TX_TIMESTAMP_SEC_L_LSB, &tx_timestamp_sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD2_R, RTL8224_RD_TX_TIMESTAMP_NSEC_H_MSB, RTL8224_RD_TX_TIMESTAMP_NSEC_H_LSB, &tx_timestamp_nsec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TX_TIMESTAMP_RD3_R, RTL8224_RD_TX_TIMESTAMP_NSEC_L_MSB, RTL8224_RD_TX_TIMESTAMP_NSEC_L_LSB, &tx_timestamp_nsec_l)) != RT_ERR_OK)
        return ret;

    pTimeEntry->valid = (uint8)valid;
    pTimeEntry->port = tx_port;

    switch (msg_type)
    {
        case 0:
            pTimeEntry->msg_type = PTP_MSG_TYPE_SYNC;
            break;
        case 1:
            pTimeEntry->msg_type = PTP_MSG_TYPE_DELAY_REQ;
            break;
        case 2:
            pTimeEntry->msg_type = PTP_MSG_TYPE_PDELAY_REQ;
            break;
        case 3:
            pTimeEntry->msg_type = PTP_MSG_TYPE_PDELAY_RESP;
            break;
        default:
            return RT_ERR_FAILED;
    }
    pTimeEntry->seqId = (seq_id_h << 10) | seq_id_l;
    pTimeEntry->txTime.sec = (tx_timestamp_sec_h << 2) | tx_timestamp_sec_l;
    pTimeEntry->txTime.nsec = (tx_timestamp_nsec_h << 16) | tx_timestamp_nsec_l;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_ptp1PPSOutput_set
 * Description:
 *      Set 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pulseWidth  - pointer to 1 PPS pulse width, unit: 10 ms
 *      enable      - enable 1 PPS output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_ptp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;

    if (pulseWidth > 0x3F)
    {
        return RT_ERR_INPUT;
    }

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_PPS_CTRL_R, RTL8224_CFG_PPS_WIDTH_MSB, RTL8224_CFG_PPS_WIDTH_LSB, pulseWidth)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_PPS_CTRL_R, RTL8224_CFG_PPS_EN_OFFSET, ((enable == ENABLED) ? 1 : 0))) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_ptp1PPSOutput_get
 * Description:
 *      Get 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPulseWidth - pointer to 1 PPS pulse width, unit: 10 ms
 *      pEnable     - pointer to 1 PPS output enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_ptp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 pps_en = 0;
    uint32 pps_width = 0;

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_PPS_CTRL_R, RTL8224_CFG_PPS_WIDTH_MSB, RTL8224_CFG_PPS_WIDTH_LSB, &pps_width)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_PPS_CTRL_R, RTL8224_CFG_PPS_EN_OFFSET, &pps_en)) != RT_ERR_OK)
        return ret;

    *pPulseWidth = pps_width;
    *pEnable = (pps_en == 1)? ENABLED : DISABLED;
    return ret;
}

/* Function Name:
 *      phy_8224_ptp_ptpClockOutput_set
 * Description:
 *      Set clock output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pClkOutput  - pointer to clock output configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_ptpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    int32 ret = RT_ERR_OK;
    uint32 clkout_en = 0;
    uint32 half_period_ns_h = 0;
    uint32 half_period_ns_l = 0;
    uint32 nsec_h = 0;
    uint32 nsec_l = 0;
    uint32 pulse_mode = 0;
    uint32 sec_h = 0;
    uint32 sec_l = 0;
    uint32 sec_m = 0;

    /* adjust Timer of PHY */
    sec_l = (pClkOutput->startTime.sec) & 0xFFFF;
    sec_m = ((pClkOutput->startTime.sec) >> 16) & 0xFFFF;
    sec_h = ((pClkOutput->startTime.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = pClkOutput->startTime.nsec & 0xFFFF;
    nsec_h = pClkOutput->startTime.nsec >> 16;

    half_period_ns_l = pClkOutput->halfPeriodNsec & 0xFFFF;
    half_period_ns_h = pClkOutput->halfPeriodNsec >> 16;

    pulse_mode = (pClkOutput->mode == PTP_CLK_OUT_PULSE) ? 1 : 0;
    clkout_en = (pClkOutput->enable == ENABLED) ? 1 : 0;

    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_NSEC0_R, RTL8224_CLKOUT_PTP_TIME_NSEC_L_MSB, RTL8224_CLKOUT_PTP_TIME_NSEC_L_LSB, nsec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_NSEC1_R, RTL8224_CLKOUT_PTP_TIME_NSEC_H_MSB, RTL8224_CLKOUT_PTP_TIME_NSEC_H_LSB, nsec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_SEC0_R, RTL8224_CLKOUT_PTP_TIME_SEC_L_MSB, RTL8224_CLKOUT_PTP_TIME_SEC_L_LSB, sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_SEC1_R, RTL8224_CLKOUT_PTP_TIME_SEC_M_MSB, RTL8224_CLKOUT_PTP_TIME_SEC_M_LSB, sec_m)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_SEC2_R, RTL8224_CLKOUT_PTP_TIME_SEC_H_MSB, RTL8224_CLKOUT_PTP_TIME_SEC_H_LSB, sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_HALF_PERD_NS_L_R, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_L_MSB, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_L_LSB, half_period_ns_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_CLKOUT_HALF_PERD_NS_H_R, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_H_MSB, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_H_LSB, half_period_ns_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_CLKOUT_CTRL_R, RTL8224_CFG_PULSE_MODE_OFFSET, pulse_mode)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_CLKOUT_CTRL_R, RTL8224_CFG_CLKOUT_EN_OFFSET, clkout_en)) != RT_ERR_OK)
        return ret;

    return ret;
}


/* Function Name:
 *      phy_8224_ptp_ptpClockOutput_get
 * Description:
 *      Get clock output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pClkOutput  - pointer to clock output configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_ptpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    int32 ret = RT_ERR_OK;
    uint32 clkout_en = 0;
    uint32 clkout_run  = 0;
    uint32 half_period_ns_h = 0;
    uint32 half_period_ns_l = 0;
    uint32 nsec_h = 0;
    uint32 nsec_l = 0;
    uint32 pulse_mode = 0;
    uint32 sec_h = 0;
    uint32 sec_l = 0;
    uint32 sec_m = 0;

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_NSEC0_R, RTL8224_CLKOUT_PTP_TIME_NSEC_L_MSB, RTL8224_CLKOUT_PTP_TIME_NSEC_L_LSB, &nsec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_NSEC1_R, RTL8224_CLKOUT_PTP_TIME_NSEC_H_MSB, RTL8224_CLKOUT_PTP_TIME_NSEC_H_LSB, &nsec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_SEC0_R, RTL8224_CLKOUT_PTP_TIME_SEC_L_MSB, RTL8224_CLKOUT_PTP_TIME_SEC_L_LSB, &sec_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_SEC1_R, RTL8224_CLKOUT_PTP_TIME_SEC_M_MSB, RTL8224_CLKOUT_PTP_TIME_SEC_M_LSB, &sec_m)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_SEC2_R, RTL8224_CLKOUT_PTP_TIME_SEC_H_MSB, RTL8224_CLKOUT_PTP_TIME_SEC_H_LSB, &sec_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_HALF_PERD_NS_L_R, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_L_MSB, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_L_LSB, &half_period_ns_l)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_CLKOUT_HALF_PERD_NS_H_R, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_H_MSB, RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_H_LSB, &half_period_ns_h)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_CLKOUT_CTRL_R, RTL8224_CFG_PULSE_MODE_OFFSET, &pulse_mode)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_CLKOUT_CTRL_R, RTL8224_CFG_CLKOUT_EN_OFFSET, &clkout_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_CLKOUT_CTRL_R, RTL8224_RD_CLKOUT_RUN_OFFSET, &clkout_run)) != RT_ERR_OK)
        return ret;

    pClkOutput->startTime.sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pClkOutput->startTime.nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    pClkOutput->mode = (pulse_mode == 1) ? PTP_CLK_OUT_PULSE : PTP_CLK_OUT_REPEAT;
    pClkOutput->enable = (clkout_en == 1) ? ENABLED : DISABLED;
    pClkOutput->runing = (clkout_run == 1) ? 1 : 0;

    pClkOutput->halfPeriodNsec = (((half_period_ns_h & 0x3FFF) << 16) | (half_period_ns_l & 0xFFFF));

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpLinkDelay_set
 * Description:
 *      Set link delay for PTP p2p transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      linkDelay   - link delay (unit: nsec)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay)
{
    int32           ret = RT_ERR_OK;
    uint32          link_delay_h = (linkDelay >> 10) & 0xFFFF;
    uint32          link_delay_l = linkDelay & 0x3FF;
    uint32          reg_addr;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_P0_PORT_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET * port_offset));
    if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, RTL8224_P0_LINK_DELAY_L_MSB, RTL8224_P0_LINK_DELAY_L_LSB, link_delay_l)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_P0_LINK_DELAY_H_R + (RTL8224_P0_REG_PORT_OFFSET * port_offset));
    if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, RTL8224_P0_LINK_DELAY_H_MSB, RTL8224_P0_LINK_DELAY_H_LSB, link_delay_h)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpLinkDelay_get
 * Description:
 *      Get link delay for PTP p2p transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pLinkDelay  - pointer to link delay (unit: nsec)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay)
{
    int32           ret = RT_ERR_OK;
    uint32          link_delay_h = 0;
    uint32          link_delay_l = 0;
    uint32          reg_addr;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_P0_PORT_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET * port_offset));
    if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, RTL8224_P0_LINK_DELAY_L_MSB, RTL8224_P0_LINK_DELAY_L_LSB, &link_delay_l)) != RT_ERR_OK)
        return ret;

    reg_addr = (RTL8224_P0_LINK_DELAY_H_R + (RTL8224_P0_REG_PORT_OFFSET * port_offset));
    if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, RTL8224_P0_LINK_DELAY_H_MSB, RTL8224_P0_LINK_DELAY_H_LSB, &link_delay_h)) != RT_ERR_OK)
        return ret;

    *pLinkDelay = (link_delay_h << 10) | link_delay_l;
    return ret;
}

/* Function Name:
 *      phy_8224_ptp_portPtpOutputSigSel_set
 * Description:
 *      Set 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      outSigSel   - output pin signal selection configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel)
{
    int32 ret = RT_ERR_OK;

    switch (outSigSel)
    {
        case PTP_OUT_SIG_SEL_DISABLE:
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_PPS_OUT_SEL_OFFSET, 0)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_CLK_OUT_SEL_OFFSET, 0)) != RT_ERR_OK)
                return ret;
            break;

        case PTP_OUT_SIG_SEL_CLOCK:
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_PPS_OUT_SEL_OFFSET, 0)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_CLK_OUT_SEL_OFFSET, 1)) != RT_ERR_OK)
                return ret;
            break;

        case PTP_OUT_SIG_SEL_1PPS:
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_PPS_OUT_SEL_OFFSET, 1)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_CLK_OUT_SEL_OFFSET, 0)) != RT_ERR_OK)
                return ret;
            break;

        default:
            return RT_ERR_NOT_SUPPORTED;
    }
    return ret;
}


/* Function Name:
 *      phy_8224_ptp_portPtpOutputSigSel_get
 * Description:
 *      Get output pin signal selection configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pOutSigSel  - pointer to output pin signal selection configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8224_ptp_portPtpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel)
{
    int32   ret = RT_ERR_OK;
    uint32  pps_sel, clk_sel;

    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_PPS_OUT_SEL_OFFSET, &pps_sel)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_IO_MUX_SEL_1_R, RTL8224_PTP_CLK_OUT_SEL_OFFSET, &clk_sel)) != RT_ERR_OK)
        return ret;

    if ((pps_sel == 0) && (clk_sel == 0))
    {
        *pOutSigSel = PTP_OUT_SIG_SEL_DISABLE;
    }
    else if (clk_sel == 1)
    {
        *pOutSigSel = PTP_OUT_SIG_SEL_CLOCK;
    }
    else
    {
        *pOutSigSel = PTP_OUT_SIG_SEL_1PPS;
    }

    return ret;
}

/* Function Name:
 *      phy_8224_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side SerDes interface mode
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMode  - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  sdsmode = 0;
    uint32  usx_subMode = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x7b20, &phyData)) != RT_ERR_OK)
        return ret;

    sdsmode = (phyData & 0x1F);
    switch (sdsmode)
    {
        case 0b00010:
            *pMode = RTK_MII_SGMII;
            break;

        case 0b00100:
            *pMode = RTK_MII_1000BX_FIBER;
            break;

        case 0b00101:
            *pMode = RTK_MII_100BX_FIBER;
            break;

        case 0b00111:
            *pMode = RTK_MII_1000BX100BX_AUTO;
            break;

        case 0b01101:
        {
            usx_subMode = ((phyData & 0x7c00) >> 10);

            switch (usx_subMode)
            {
                case 0b00000:
                    *pMode = RTK_MII_USXGMII_10GSXGMII;
                    break;
                case 0b00010:
                    *pMode = RTK_MII_USXGMII_10GQXGMII;
                    break;
                default:
                    *pMode = RTK_MII_USXGMII_10GQXGMII;
                    break;
            }
        }
        break;

        case 0b10010:
            *pMode = RTK_MII_HISGMII;
            break;

        case 0b10110:
            *pMode = RTK_MII_2500Base_X;
            break;

        case 0b11010:
            *pMode = RTK_MII_10GR;
            break;

        case 0b11011:
            *pMode = RTK_MII_10GRSGMII_AUTO;
            break;

        case 0b11100:
            *pMode = RTK_MII_10GR1000BX_AUTO;
            break;
        default:
            *pMode = RTK_MII_DISABLE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side SerDes interface link status
 * Input:
 *      unit    - unit ID
 *      port    - port id
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0;
    rtk_port_t      base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    ret = phy_rtl8224_sdsReg_get(unit, base_port, 0x5, 0x0, &phyData);

    pStatus->sds_num=1;
    pStatus->link_status[0] = ((phyData & 0x1000) == 0x1000) ? (PORT_LINKUP) : (PORT_LINKDOWN);

    return ret;
}

/* Function Name:
 *      phy_8224_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~BIT_2);
    phyData |= (enable == DISABLED) ? (0) : (BIT_2);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA430, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_2) ? (ENABLED) : (DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_downSpeedEnable_set
 * Description:
 *      Set UTP down speed state of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
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
phy_8224_downSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_3));
    phyData |= (enable == DISABLED) ? (0) : (BIT_3);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_downSpeedEnable_get
 * Description:
 *      Get UTP down speed state of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8224_downSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_3) ? (ENABLED) : (DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_downSpeedStatus_get
 * Description:
 *      Get down speed status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pDownSpeedStatus - pointer to status of down speed.
 *                         TRUE: link is up due to down speed; FALSE: down speed is not performed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA4B0, &phyData)) != RT_ERR_OK)
        return ret;
    *pDownSpeedStatus = (phyData & BIT_12) ? (1) : (0);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_mdiLoopbackEnable_set
 * Description:
 *      Enable port MDI loopback for connecting with RJ45 loopback connector
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of MDI loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_10));
    phyData |= (enable == DISABLED) ? (0) : (BIT_10);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA430, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_mdiLoopbackEnable_get
 * Description:
 *      Enable port MDI loopback for connecting with RJ45 loopback connector
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of MDI loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_10) ? (ENABLED) : (DISABLED);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 * Output:
 *      pEyeParam - eye parameter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Only sdsId 0 is available.
 */
int32
phy_8224_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32 ret  = 0;

    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_POST1AMP_PAGE, PHY_8224_REG0_TX_POST1AMP_REG, PHY_8224_REG0_TX_POST1AMP_HIGH_BIT, PHY_8224_REG0_TX_POST1AMP_LOW_BIT, &pEyeParam->post_amp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_POST1EN_PAGE, PHY_8224_REG0_TX_POST1EN_REG, PHY_8224_REG0_TX_POST1EN_HIGH_BIT, PHY_8224_REG0_TX_POST1EN_LOW_BIT, &pEyeParam->post_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_MAINAMP_PAGE, PHY_8224_REG0_TX_MAINAMP_REG, PHY_8224_REG0_TX_MAINAMP_HIGH_BIT, PHY_8224_REG0_TX_MAINAMP_LOW_BIT, &pEyeParam->main_amp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_MAINEN_PAGE, PHY_8224_REG0_TX_MAINEN_REG, PHY_8224_REG0_TX_MAINEN_HIGH_BIT, PHY_8224_REG0_TX_MAINEN_LOW_BIT, &pEyeParam->main_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_PREAMP_PAGE, PHY_8224_REG0_TX_PREAMP_REG, PHY_8224_REG0_TX_PREAMP_HIGH_BIT, PHY_8224_REG0_TX_PREAMP_LOW_BIT, &pEyeParam->pre_amp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_PREEN_PAGE, PHY_8224_REG0_TX_PREEN_REG, PHY_8224_REG0_TX_PREEN_HIGH_BIT, PHY_8224_REG0_TX_PREEN_LOW_BIT, &pEyeParam->pre_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_get(unit, port, PHY_8224_REG0_TX_Z0_PAGE, PHY_8224_REG0_TX_Z0_REG, PHY_8224_REG0_TX_Z0_HIGH_BIT, PHY_8224_REG0_TX_Z0_LOW_BIT, &pEyeParam->impedance)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 *      pEyeParam - eye parameter.
 *                  impedance is not supported for configure.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Only sdsId 0 is available.
 */
int32
phy_8224_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32 ret  = 0;

    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_POST1AMP_PAGE, PHY_8224_REG0_TX_POST1AMP_REG, PHY_8224_REG0_TX_POST1AMP_HIGH_BIT, PHY_8224_REG0_TX_POST1AMP_LOW_BIT, pEyeParam->post_amp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_POST1EN_PAGE, PHY_8224_REG0_TX_POST1EN_REG, PHY_8224_REG0_TX_POST1EN_HIGH_BIT, PHY_8224_REG0_TX_POST1EN_LOW_BIT, pEyeParam->post_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_MAINAMP_PAGE, PHY_8224_REG0_TX_MAINAMP_REG, PHY_8224_REG0_TX_MAINAMP_HIGH_BIT, PHY_8224_REG0_TX_MAINAMP_LOW_BIT, pEyeParam->main_amp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_MAINEN_PAGE, PHY_8224_REG0_TX_MAINEN_REG, PHY_8224_REG0_TX_MAINEN_HIGH_BIT, PHY_8224_REG0_TX_MAINEN_LOW_BIT, pEyeParam->main_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_PREAMP_PAGE, PHY_8224_REG0_TX_PREAMP_REG, PHY_8224_REG0_TX_PREAMP_HIGH_BIT, PHY_8224_REG0_TX_PREAMP_LOW_BIT, pEyeParam->pre_amp)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_PREEN_PAGE, PHY_8224_REG0_TX_PREEN_REG, PHY_8224_REG0_TX_PREEN_HIGH_BIT, PHY_8224_REG0_TX_PREEN_LOW_BIT, pEyeParam->pre_en)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_REG0_TX_Z0_PAGE, PHY_8224_REG0_TX_Z0_REG, PHY_8224_REG0_TX_Z0_HIGH_BIT, PHY_8224_REG0_TX_Z0_LOW_BIT, pEyeParam->impedance)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_8224_eyeMon_scan_en(uint32 unit, uint32 port)
{
    int32   ret;

    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_SCAN_EN_PAGE, PHY_8224_EYE_SCAN_EN_REG, PHY_8224_EYE_SCAN_EN_HIGH_BIT, PHY_8224_EYE_SCAN_EN_LOW_BIT, 1)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_SCAN_EN_PAGE, PHY_8224_EYE_SCAN_EN_REG, PHY_8224_EYE_SCAN_EN_HIGH_BIT, PHY_8224_EYE_SCAN_EN_LOW_BIT, 0)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_SCAN_EN_PAGE, PHY_8224_EYE_SCAN_EN_REG, PHY_8224_EYE_SCAN_EN_HIGH_BIT, PHY_8224_EYE_SCAN_EN_LOW_BIT, 1)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_REF_CTRL_PAGE, PHY_8224_EYE_REF_CTRL_REG, PHY_8224_EYE_REF_CTRL_HIGH_BIT, PHY_8224_EYE_REF_CTRL_LOW_BIT, 0x3f)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_PI_PHASE_PAGE, PHY_8224_EYE_PI_PHASE_REG, PHY_8224_EYE_PI_PHASE_HIGH_BIT, PHY_8224_EYE_PI_PHASE_LOW_BIT, 0x3f)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_8224_eyeMon_pi_set(uint32 unit, uint32 port, uint32 x)
{
    int32   ret;

    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_PI_PHASE_PAGE, PHY_8224_EYE_PI_PHASE_REG, PHY_8224_EYE_PI_PHASE_HIGH_BIT, PHY_8224_EYE_PI_PHASE_LOW_BIT, x)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_8224_eyeMon_ref_set(uint32 unit, uint32 port, uint32 y)
{
    int32   ret;

    if ((ret = _phy_8224_sdsRegField_set(unit, port, PHY_8224_EYE_REF_CTRL_PAGE, PHY_8224_EYE_REF_CTRL_REG, PHY_8224_EYE_REF_CTRL_HIGH_BIT, PHY_8224_EYE_REF_CTRL_LOW_BIT, y)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_8224_eyeMon_MDIOCLK(uint32 unit, uint32 port, uint32 sdsPage, uint32 sdsReg)
{
    int32           ret;
    uint32          reg_data = 0;
    rtk_port_t      base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, &reg_data)) != RT_ERR_OK)
        return ret;
    reg_data = reg_data | 0x1;
    if ((ret = phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, reg_data)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, &reg_data)) != RT_ERR_OK)
        return ret;
    reg_data = reg_data & 0xFFFE;
    if ((ret = phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, reg_data)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, &reg_data)) != RT_ERR_OK)
        return ret;
    reg_data = reg_data | 0x1;
    if ((ret = phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, reg_data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_8224_eyeMon_DBGOUT(uint32 unit, uint32 port, uint32 degOut_en)
{
    int32           ret;
    uint32          reg_data = 0;
    rtk_port_t      base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    if(degOut_en != 0)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_ADR0_REG, 0x0010)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_ADR1_REG, 0x0010)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_ADR2_REG, 0x0010)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_ADR3_REG, 0x0010)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL0_REG, &reg_data)) != RT_ERR_OK)
            return ret;
        reg_data = REG32_FIELD_SET(reg_data, 0x0, 0x0, 0x0003);
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL0_REG, reg_data)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL1_REG, &reg_data)) != RT_ERR_OK)
            return ret;
        reg_data = REG32_FIELD_SET(reg_data, 0x1, 0x0, 0x0003);
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL1_REG, reg_data)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL2_REG, &reg_data)) != RT_ERR_OK)
            return ret;
        reg_data = REG32_FIELD_SET(reg_data, 0x2, 0x0, 0x0003);
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL2_REG, reg_data)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL3_REG, &reg_data)) != RT_ERR_OK)
            return ret;
        reg_data = REG32_FIELD_SET(reg_data, 0x3, 0x0, 0x0003);
        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_SEL3_REG, reg_data)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_set(unit, base_port, PHY_MMD_VEND1, PHY_8224_PAD_CTRL_REG, 0x0)) != RT_ERR_OK)
            return ret;

        /* Debug out */
        if ((ret = phy_rtl8224_sdsReg_set(unit, base_port, 0x1f, 0x2, 31)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8224_sdsRegField_set(unit, port, 0x36, 0x5, 14, 11, 0xc)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_sdsRegField_set(unit, port, 0x36, 0x12, 10, 5, 0x0)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

int32
_phy_8224_eyeMon_readOut(uint32 unit, uint32 port, uint32 *pData)
{
    int32           ret;
    uint32          reg_data = 0;
    rtk_port_t      base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    if ((ret = phy_common_general_reg_mmd_get(unit, base_port, PHY_MMD_VEND1, PHY_8224_DBG_CTRL_VAL_REG, &reg_data)) != RT_ERR_OK)
        return ret;

    *pData = reg_data;

    return RT_ERR_OK;
}


int32
_phy_8224_eyeMon_proc(uint32 unit, uint32 port, uint32 frameNum, phy_sds_eyeMon_hdlr_t pHdlr, void *pDb)
{
    uint32  x, y, frame;
    uint32  readDate;
    int32   ret;
    int32   retry_cnt, valid_flag;

    /* enable */
    ret = _phy_8224_eyeMon_scan_en(unit, port);

    /* set x axis */
    for (x = 0; x < RTK_EYE_MON_X_MAX; ++x)
    {
        ret = _phy_8224_eyeMon_pi_set(unit, port, x);

        /* set y axis */
        for (y = 0; y < RTK_EYE_MON_Y_MAX; ++y)
        {
            ret = _phy_8224_eyeMon_ref_set(unit, port, y);
            ret = _phy_8224_eyeMon_MDIOCLK(unit, port, 0x36, 0x19);
            ret = _phy_8224_eyeMon_DBGOUT(unit, port, 1);
            ret = _phy_8224_eyeMon_readOut(unit, port, &readDate);
            /* get val */
            for (frame = 0; frame < frameNum; ++frame)
            {
                retry_cnt = 0;
                valid_flag = 0;
                do
                {
                    ret = _phy_8224_eyeMon_MDIOCLK(unit, port, 0x36, 0x19);
                    if ((ret = _phy_8224_eyeMon_readOut(unit, port, &readDate)) != RT_ERR_OK)
                    {
                        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! x=%u, y=%u, frame=%u, ret=0x%X\n", unit, port, __FUNCTION__, x, y, frame, ret);
                        goto EXIT;
                    }

                    if (readDate == 0x0)
                    {
                        retry_cnt++;
                    }else{
                        valid_flag = 1;
                        retry_cnt = 0;
                    }

                    if (retry_cnt > 1)
                    {
                        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! x=%u, y=%u, frame=%u, readDate=0x%X\n", unit, port, __FUNCTION__, x, y, frame, readDate);
                        goto EXIT;
                    }
                }while(valid_flag != 1);

                pHdlr(x, y, frame, pDb, readDate);
            }
        }
    }

EXIT:
    return ret;
}

/* Function Name:
 *      phy_8224_eyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 *      frameNum- frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_eyeMonitor_start(uint32 unit, uint32 port, uint32 sdsId, uint32 frameNum)
{
    uint32  x, y, frame;
    uint32  *eyeData = NULL;
    uint32  data_size;
    int32   ret;

    if (sdsId != 0)
    {
        return RT_ERR_INPUT;
    }

    if (frameNum > RTK_EYE_MON_FRAME_MAX)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "frameNum %u exceed maxmum %u!", frameNum, RTK_EYE_MON_FRAME_MAX);
        return RT_ERR_FAILED;
    }

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "malloc %u fail!\n", data_size);
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    RT_LOG(LOG_INFO, (MOD_HAL | MOD_PHY), "uint:%u port:%u sdsId:%u frameNum:%u", unit, port, sdsId, frameNum);

    if ((ret = _phy_8224_eyeMon_proc(unit, port, frameNum, phy_common_eyeMonPixel_get, (void *)eyeData)) != RT_ERR_OK)
    {
        goto EXIT;
    }

    for (x = 0; x < RTK_EYE_MON_X_MAX; ++x)
    {
        for (y = 0; y < RTK_EYE_MON_Y_MAX; ++y)
        {
            for (frame = 0; frame < frameNum; ++frame)
            {
                osal_printf("[%d, %d : %d]\n", x, y, eyeData[RTK_EYE_MON_DATA_POS(frame, x, y)]);
            }
        }
    }

EXIT:
    if (eyeData)
    {
        osal_free(eyeData);
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_eyeMonitorInfo_get
 * Description:
 *      Get eye monitor height and width
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 *      frameNum- frame number
 * Output:
 *      pInfo   - eye monitor information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224_eyeMonitorInfo_get(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    rtk_eye_monBmap_t   *eye;
    int32           xAxis, yAxis;
    uint32          data_size, i;
    uint8           maxHeight, height;
    uint8           maxWidth, width;
    uint8           width_sample_pos[] = {(RTK_EYE_MON_Y_MAX - RTK_EYE_MON_YAXIS_CHK_OFST), (RTK_EYE_MON_Y_MAX + RTK_EYE_MON_YAXIS_CHK_OFST)};
    int32           ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PHY), "unit=%u,port=%u,sds=%u,frameNum=%u", unit, port, sds, frameNum);

    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    data_size = sizeof(rtk_eye_monBmap_t) * RTK_EYE_MON_X_MAX;
    if ((eye = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_MEM_ALLOC;
    }
    osal_memset(eye, 0, data_size);


    if ((ret = _phy_8224_eyeMon_proc(unit, port, frameNum, phy_common_eyeMonInfo_update, (void *)eye)) != RT_ERR_OK)
    {
        goto ERR;
    }

    maxHeight = 0;
    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
    {
        height = 0;
        for (yAxis = 0; yAxis < RTK_EYE_MON_ARXIS_Y_MAX; ++yAxis)
        {
            if (BITMAP_IS_SET(eye[xAxis].arr, yAxis))
            {
                if (maxHeight < height)
                {
                    maxHeight = height;
                }

                height = 0;
            }
            else
                ++height;
        }

        if (maxHeight < height)
        {
            maxHeight = height;
        }
    }

    pInfo->height = maxHeight;

    maxWidth = 0;
    for (i = 0; i < sizeof(width_sample_pos)/sizeof(uint8); ++i)
    {
        yAxis = width_sample_pos[i];
        width = 0;
        for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
        {
            if (BITMAP_IS_SET(eye[xAxis].arr, yAxis))
            {
                if (maxWidth < width)
                {
                    maxWidth = width;
                }

                width = 0;
            }
            else
                ++width;
        }

        if (maxWidth < width)
        {
            maxWidth = width;
        }
    }

    pInfo->width = maxWidth;

ERR:
    osal_free(eye);
    return ret;
}

/* Function Name:
 *      phy_8224_sdsReg_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - sds page id
 *      sdsReg  - sds reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsReg_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    rtk_port_t      base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    return phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, pData);

}

/* Function Name:
 *      phy_8224_sdsReg_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - sds page id
 *      sdsReg  - sds reg id
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsReg_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 data)
{
    rtk_port_t      base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    return phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, data);
}

/* Function Name:
 *      phy_8224_chipVer_get
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pVer    - chip version
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_chipVer_get(uint32 unit, rtk_port_t port, uint32 *pVer)
{
    int32       ret;
    uint32      regData;

    if ((ret = _phy_8224_intReg_set(unit, port, 0xbb00000c, 19, 16, 0xa)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_intReg_get(unit, port, 0xbb00000c, 31, 28, &regData)) != RT_ERR_OK)
        return ret;

    *pVer = regData;

    if ((ret = _phy_8224_intReg_set(unit, port, 0xbb00000c, 19, 16, 0x0)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_sdsOpCode_set
 * Description:
 *      Set SerDes Op Code.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      opCode  - SerDes op code
 * Output:
 *      NA
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsOpCode_set(uint32 unit, rtk_port_t port, uint32 opCode)
{
    rtk_port_t    base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    return _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_NWAY_OPCODE_PAGE, PHY_8224_NWAY_OPCODE_REG, PHY_8224_NWAY_OPCODE_HIGH_BIT, PHY_8224_NWAY_OPCODE_LOW_BIT, opCode);
}

/* Function Name:
 *      phy_8224_sdsAmPeriod_set
 * Description:
 *      Set SerDes AM Period.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      amPeriod  - SerDes AM Period
 * Output:
 *      NA
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsAmPeriod_set(uint32 unit, rtk_port_t port, uint32 amPeriod)
{
    rtk_port_t    base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    return _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PERIOD_PAGE, PHY_8224_AM_PERIOD_REG, PHY_8224_AM_PERIOD_HIGH_BIT, PHY_8224_AM_PERIOD_LOW_BIT, amPeriod);
}

/* Function Name:
 *      phy_8224_sdsAmPeriod_get
 * Description:
 *      Get SerDes AM Period.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      amPeriod  - SerDes AM Period
 * Output:
 *      NA
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      NA
 */
int32
phy_8224_sdsAmPeriod_get(uint32 unit, rtk_port_t port, uint32 *pAmPeriod)
{
    rtk_port_t    base_port = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);

    return _phy_8224_sdsRegField_get(unit, base_port, PHY_8224_AM_PERIOD_PAGE, PHY_8224_AM_PERIOD_REG, PHY_8224_AM_PERIOD_HIGH_BIT, PHY_8224_AM_PERIOD_LOW_BIT, pAmPeriod);
}

/* Function Name:
 *      phy_8224_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_NOT_FINISH - operation is not finished
 * Note:
 *      RTCT is not supported when port link at 10M.
 */
int32
phy_8224_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0, speed = 0, duplex = 0;
    uint32  tryTime = 1000;
    rtk_enable_t ena;


    if ((ret = phy_common_c45_enable_get(unit, port, &ena)) != RT_ERR_OK)
        return ret;

    if (ena == DISABLED)
    {
        return RT_ERR_OPER_DENIED;
    }

    /* Check the port is link up or not?
     * Due to bit 2 is LL(latching low), need to read twice to get current status
     */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_c45_speedDuplexStatusResReg_get(unit, port, &speed, &duplex);
    if (phyData & BIT_2)
    {
        if (speed == PORT_SPEED_10M)
        {
            return RT_ERR_PORT_NOT_SUPPORTED;
        }

        return RT_ERR_OK;
    }
    else
    {
	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
	        return ret;
	    phyData |= (BIT_9); //[9]=1
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA400, phyData)) != RT_ERR_OK)
	        return ret;
	
	    osal_time_mdelay(500);
	
	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	        return ret;
	    phyData &= (~BIT_15); //[15]=0
	    phyData |= (BIT_1); //[1]=1
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
	        return ret;
	
	    phyData |= (BIT_4 | BIT_5 | BIT_6 | BIT_7); //[7:4]=0xf
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
	        return ret;
	
	    phyData |= (BIT_0); //[0]=0x1
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
	        return ret;
	
	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	        return ret;
	
	    while((phyData & BIT_15) == 0)
	    {
	        osal_time_mdelay(10);
	        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	            return ret;
	        tryTime--;
	        if(tryTime == 0)
	            return RT_ERR_NOT_FINISH;
	    }
    }
    return ret;
}


/* Function _phy_8224_rtctChannelLen_get:
 *      phy_826xb_rtct_start
 * Description:
 *      Calculate channel A-D length
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_NOT_FINISH - operation is not finished
 * Note:
 *      Length unit is meter
 */
int32
_phy_8224_rtctChannelLen_get(uint32 unit, rtk_port_t port, uint32 channel, uint32 *pChLen)
{
    int32       ret = RT_ERR_OK;
    uint32      channel_l, channel_h;
    uint32      cable_len;

    switch (channel)
    {
        case 0:
            if ((ret = _phy_8224_sram_get(unit, port, (0x802c - 0x4), &channel_h)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_sram_get(unit, port, (0x802d - 0x4), &channel_l)) != RT_ERR_OK)
                return ret;
            break;
        case 1:
            if ((ret = _phy_8224_sram_get(unit, port, (0x8030 - 0x4), &channel_h)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_sram_get(unit, port, (0x8031 - 0x4), &channel_l)) != RT_ERR_OK)
                return ret;
            break;
        case 2:
            if ((ret = _phy_8224_sram_get(unit, port, (0x8034 - 0x4), &channel_h)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_sram_get(unit, port, (0x8035 - 0x4), &channel_l)) != RT_ERR_OK)
                return ret;
            break;
        case 3:
            if ((ret = _phy_8224_sram_get(unit, port, (0x8038 - 0x4), &channel_h)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_sram_get(unit, port, (0x8039 - 0x4), &channel_l)) != RT_ERR_OK)
                return ret;
            break;
        default:
                return RT_ERR_OUT_OF_RANGE;
            break;
    }

    cable_len = ((channel_h & 0xff00)+((channel_l >> 8) & 0xff));
    if (cable_len >= RTL8224_RTCT_LEN_OFFSET)
        cable_len = cable_len - RTL8224_RTCT_LEN_OFFSET;
    else
        cable_len = 0;

    cable_len = (cable_len*10)/RTL8224_RTCT_LEN_CABLE_FACTOR;

    *pChLen = cable_len;

    return ret;
}


int32
_phy_8224_rtctStatus_convert(uint32 phyData,
                                uint32 *pShort, uint32 *pOpen,
                                uint32 *pMismatch, uint32 *pPairBusy)
{
    if (phyData & BIT_6)
    {
        if (phyData & BIT_5)
        {
            /* normal */
            return RT_ERR_OK;
        }
        else if (phyData & BIT_3)
        {
            *pOpen = 1;
        }
        else if (phyData & BIT_4)
        {
            *pShort = 1;
        }
        else if (phyData & BIT_0)
        {
            *pPairBusy = 1;
        }
        else if (phyData & BIT_7) /* Interpair short */
        {
            *pShort = 2;
        }
    }
    else
    {
        return RT_ERR_PHY_RTCT_NOT_FINISH;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8224_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 * Note:
 *      The result unit is cm
 */
int32
phy_8224_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0, speed = 0, duplex = 0;
    uint32          channel_status = 0;
    uint32          channel_length;

    osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));

    /* Check the port is link up or not?
     * Due to bit 2 is LL(latching low), need to read twice to get current status
     */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_c45_speedDuplexStatusResReg_get(unit, port, &speed, &duplex);
    if (phyData & BIT_2)
    {
        if (speed == PORT_SPEED_10M)
        {
            return RT_ERR_PORT_NOT_SUPPORTED;
        }
        /* If the port link is up,
         * return cable length from Channel Estimation
         */
        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        if (speed == PORT_SPEED_2_5G) /* 2.5GM */
        {
            pRtctResult->linkType = PORT_SPEED_2_5G;
            /* The channel A length is store in 0xb550 [7:0]*/
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb550, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
            /* The channel A length is store in 0xb550 [7:0]*/
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb552, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
            /* The channel A length is store in 0xb550 [7:0]*/
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb554, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
            /* The channel A length is store in 0xb550 [7:0]*/
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb556, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
        }
        else if (speed == PORT_SPEED_1000M) /* 1000M */
        {
            /* The Length is store in [7:0], and the unit is meter*/
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA880, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->linkType = PORT_SPEED_1000M;
            pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
        }
        else /* 100M */
        {
            /* The Length is store in [7:0], and the unit is meter*/
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA880, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->linkType = PORT_SPEED_100M;
            pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
            pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
        }
    }
    else
    {
	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	        return ret;
	
	    if ((phyData & BIT_15) == 0)
	        return RT_ERR_PHY_RTCT_NOT_FINISH;
	
	    pRtctResult->linkType = PORT_SPEED_1000M;
	
	    if ((ret = _phy_8224_rtctChannelLen_get(unit, port, 0, &channel_length)) != RT_ERR_OK)
	        return ret;
	    pRtctResult->un.ge_result.channelALen = (channel_length*100); /* RTCT length unit is cm */
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL A]: Length 0x%d\n", __FUNCTION__, channel_length);
	
	    if ((ret = _phy_8224_rtctChannelLen_get(unit, port, 1, &channel_length)) != RT_ERR_OK)
	        return ret;
	    pRtctResult->un.ge_result.channelBLen = (channel_length*100);  /* RTCT length unit is cm */
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL B]: Length 0x%d\n", __FUNCTION__, channel_length);
	
	    if ((ret = _phy_8224_rtctChannelLen_get(unit, port, 2, &channel_length)) != RT_ERR_OK)
	        return ret;
	    pRtctResult->un.ge_result.channelCLen = (channel_length*100);  /* RTCT length unit is cm */
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL C]: Length 0x%d\n", __FUNCTION__, channel_length);
	
	    if ((ret = _phy_8224_rtctChannelLen_get(unit, port, 3, &channel_length)) != RT_ERR_OK)
	        return ret;
	    pRtctResult->un.ge_result.channelDLen = (channel_length*100);  /* RTCT length unit is cm */
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL D]: Length 0x%d\n", __FUNCTION__, channel_length);
	
	
	
	    if ((ret = _phy_8224_sram_get(unit, port, 0x8026, &channel_status)) != RT_ERR_OK)
	        return ret;
	    if ((ret = _phy_8224_rtctStatus_convert(channel_status,
	                                            &pRtctResult->un.ge_result.channelAShort,        &pRtctResult->un.ge_result.channelAOpen,
	                                            &pRtctResult->un.ge_result.channelAMismatch,     &pRtctResult->un.ge_result.channelAPairBusy)) != RT_ERR_OK)
	        return ret;
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL A]: Status 0x%04X\n", __FUNCTION__, channel_status);
	
	    if ((ret = _phy_8224_sram_get(unit, port, 0x802A, &channel_status)) != RT_ERR_OK)
	        return ret;
	    if ((ret = _phy_8224_rtctStatus_convert(channel_status,
	                                            &pRtctResult->un.ge_result.channelBShort,        &pRtctResult->un.ge_result.channelBOpen,
	                                            &pRtctResult->un.ge_result.channelBMismatch,     &pRtctResult->un.ge_result.channelBPairBusy)) != RT_ERR_OK)
	        return ret;
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL B]: Status 0x%04X\n", __FUNCTION__, channel_status);
	
	    if ((ret = _phy_8224_sram_get(unit, port, 0x802E, &channel_status)) != RT_ERR_OK)
	        return ret;
	    if ((ret = _phy_8224_rtctStatus_convert(channel_status,
	                                            &pRtctResult->un.ge_result.channelCShort,        &pRtctResult->un.ge_result.channelCOpen,
	                                            &pRtctResult->un.ge_result.channelCMismatch,     &pRtctResult->un.ge_result.channelCPairBusy)) != RT_ERR_OK)
	        return ret;
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL C]: Status 0x%04X\n", __FUNCTION__, channel_status);
	
	    if ((ret = _phy_8224_sram_get(unit, port, 0x8032, &channel_status)) != RT_ERR_OK)
	        return ret;
	    if ((ret = _phy_8224_rtctStatus_convert(channel_status,
	                                            &pRtctResult->un.ge_result.channelDShort,        &pRtctResult->un.ge_result.channelDOpen,
	                                            &pRtctResult->un.ge_result.channelDMismatch,     &pRtctResult->un.ge_result.channelDPairBusy)) != RT_ERR_OK)
	        return ret;
	    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[CHANNEL D]: Status 0x%04X\n", __FUNCTION__, channel_status);
    }
    return ret;
}

int32
_phy_8224_dbgCounterReg_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint32 *pRegAddr)
{
    int32           ret = RT_ERR_OK;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    switch (type)
    {
        case PHY_DBG_CNT_RX:
            switch (port_offset)
            {
                case 0:
                    *pRegAddr = RTL8224_PHY0_RX_MIB_CNTR0;
                    break;
                case 1:
                    *pRegAddr = RTL8224_PHY1_RX_MIB_CNTR0;
                    break;
                case 2:
                    *pRegAddr = RTL8224_PHY2_RX_MIB_CNTR0;
                    break;
                case 3:
                    *pRegAddr = RTL8224_PHY3_RX_MIB_CNTR0;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        case PHY_DBG_CNT_RX_ERR:
            switch (port_offset)
            {
                case 0:
                    *pRegAddr = RTL8224_PHY0_RX_MIB_CNTR3;
                    break;
                case 1:
                    *pRegAddr = RTL8224_PHY1_RX_MIB_CNTR3;
                    break;
                case 2:
                    *pRegAddr = RTL8224_PHY2_RX_MIB_CNTR3;
                    break;
                case 3:
                    *pRegAddr = RTL8224_PHY3_RX_MIB_CNTR3;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        case PHY_DBG_CNT_RX_CRCERR:
            switch (port_offset)
            {
                case 0:
                    *pRegAddr = RTL8224_PHY0_RX_MIB_CNTR3;
                    break;
                case 1:
                    *pRegAddr = RTL8224_PHY1_RX_MIB_CNTR3;
                    break;
                case 2:
                    *pRegAddr = RTL8224_PHY2_RX_MIB_CNTR3;
                    break;
                case 3:
                    *pRegAddr = RTL8224_PHY3_RX_MIB_CNTR3;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8224_dbgCounter_get
 * Description:
 *      Get debug counter in PHY
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - counter type
 * Output:
 *      pCnt - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      NA
 */
int32
phy_8224_dbgCounter_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint64 *pCnt)
{
    int32   ret;
    uint64  cnt;
    uint32  cnt_32;
    uint32  regAddr;

    cnt = 0;
    cnt_32 = 0;

    switch (type)
    {
        case PHY_DBG_CNT_RX:
            if ((ret = _phy_8224_dbgCounterReg_get(unit, port, type, &regAddr)) != RT_ERR_OK)
                return ret;

            if ((ret = _phy_8224_intReg_get(unit, port, regAddr, 31, 0, &cnt_32)) != RT_ERR_OK)
                return ret;
            cnt |= (uint64)(cnt_32);
            break;
        case PHY_DBG_CNT_RX_ERR:
            if ((ret = _phy_8224_dbgCounterReg_get(unit, port, type, &regAddr)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intReg_get(unit, port, regAddr, 31, 0, &cnt_32)) != RT_ERR_OK)
                return ret;
            cnt |= (uint64)(cnt_32);
            break;
        case PHY_DBG_CNT_RX_CRCERR:
            if ((ret = _phy_8224_dbgCounterReg_get(unit, port, type, &regAddr)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_8224_intReg_get(unit, port, regAddr, 31, 16, &cnt_32)) != RT_ERR_OK)
                return ret;
            cnt = (uint64)(cnt_32);
            break;

        case PHY_DBG_CNT_LDPC_ERR:
            /* Not Support*/
            cnt = 0;
            break;
        default:
            return RT_ERR_INPUT;
    }
    *pCnt = cnt;
    return ret;
}

/* Function Name:
 *      phy_8224_ctrl_set
 * Description:
 *      Set PHY settings for specific feature
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8224_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData, msb_bit, lsb_bit;
    uint32          reg_addr;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_MDI_POLARITY_SWAP:
            lsb_bit = (port_offset*RTL8224_PHY_TX_POLARITY_SWAP_OFFSET);
            msb_bit = lsb_bit + (RTL8224_PHY_TX_POLARITY_SWAP_OFFSET - 1);
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PHY_TX_POLARITY_SWAP, msb_bit, lsb_bit, &phyData)) != RT_ERR_OK)
                return ret;

            phyData &= (~(BIT_0 | BIT_1 | BIT_2 | BIT_3));
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A)? (BIT_0) : (0);
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B)? (BIT_1) : (0);
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C)? (BIT_2) : (0);
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D)? (BIT_3) : (0);
            if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_TX_POLARITY_SWAP, msb_bit, lsb_bit, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_MDI_INVERSE:
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PHY_MDI_REVERSE, port_offset, port_offset, &phyData)) != RT_ERR_OK)
                return ret;

            if(value != 0) /*Need to reverse*/
                phyData = 1;
            else
                phyData = 0;

            if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_MDI_REVERSE, port_offset, port_offset, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_LED_1_MODE:
        case RTK_PHY_CTRL_LED_2_MODE:
        case RTK_PHY_CTRL_LED_3_MODE:
        case RTK_PHY_CTRL_LED_4_MODE:
            ret = _phy_8224_led_mode_set(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_MODE), value);
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE:
            ret = _phy_8224_led_config_blink_rate_set(unit, port, value);
            break;
        case RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_2_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_3_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_4_CFG_ACTIVE_LOW:
            ret = _phy_8224_led_config_active_low_set(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW), value);
            break;
        case RTK_PHY_CTRL_LED_1_CFG_FORCE:
        case RTK_PHY_CTRL_LED_2_CFG_FORCE:
        case RTK_PHY_CTRL_LED_3_CFG_FORCE:
        case RTK_PHY_CTRL_LED_4_CFG_FORCE:
            ret = _phy_8224_led_config_force_set(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_FORCE), value);
            break;
        case RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_0);
            phyData |= (value == 0)? (0):(BIT_0);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 0, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PTP_REFTIME_TOD_DELAY:
            if (value > 0xFFFF)
                return RT_ERR_OUT_OF_RANGE;
            if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_TOD_DELAY_R, RTL8224_TOD_DELAY_MSB, RTL8224_TOD_DELAY_LSB, value)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PTP_DURATION_THRESHOLD:
            if (value > 0x3FF)
                return RT_ERR_OUT_OF_RANGE;
            if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PTP_TIME_OP_DURATION_R, RTL8224_TIME_OP_DURATION_MSB, RTL8224_TIME_OP_DURATION_LSB, value)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PTP_PORT_ROLE:
            switch (value)
            {
                case RTK_PHY_CTRL_PTP_PORT_ROLE_NONE:
                    phyData = 0x0;
                    break;
                case RTK_PHY_CTRL_PTP_PORT_ROLE_BC_OC:
                    phyData = 0x1;
                    break;
                case RTK_PHY_CTRL_PTP_PORT_ROLE_E2E_TC:
                    phyData = 0x2;
                    break;
                case RTK_PHY_CTRL_PTP_PORT_ROLE_P2P_TC:
                    phyData = 0x3;
                    break;
                default:
                    return RT_ERR_INPUT;
            }
            reg_addr = (RTL8224_P0_PORT_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
            if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, RTL8224_P0_PTP_ROLE_MSB, RTL8224_P0_PTP_ROLE_LSB, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PTP_TX_IMBAL:
            if (value > 0xFFF)
                return RT_ERR_OUT_OF_RANGE;

            reg_addr = (RTL8224_P0_TX_IMBAL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
            if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, RTL8224_P0_TX_IMBAL_MSB, RTL8224_P0_TX_IMBAL_LSB, value)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PTP_RX_IMBAL:
            if (value > 0xFFF)
                return RT_ERR_OUT_OF_RANGE;

            reg_addr = (RTL8224_P0_RX_IMBAL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
            if ((ret = _phy_8224_intReg_set(unit, port, reg_addr, RTL8224_P0_RX_IMBAL_MSB, RTL8224_P0_RX_IMBAL_LSB, value)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PTP_CLOCK_SRC:
            if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_PTP_CLK_CTRL_R, RTL8224_CFG_CLK_SRC_OFFSET, ((value == RTK_PHY_CTRL_PTP_CLOCK_SRC_INT) ? 0 : 1))) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
            ret = _phy_8224_temperature_threshold_set(unit, port, ctrl_type, value);
            break;
        case RTK_PHY_CTRL_SERDES_TX_POLARITY:
            if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, 0x6, 0x2, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_14);
            phyData |= (value == 0) ? (0) : (BIT_14);
            if ((ret = phy_rtl8224_sdsReg_set(unit, port, 0x6, 0x2, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SERDES_RX_POLARITY:
            if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, 0x6, 0x2, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_13);
            phyData |= (value == 0) ? (0) : (BIT_13);
            if ((ret = phy_rtl8224_sdsReg_set(unit, port, 0x6, 0x2, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN_NFR:
            switch (value)
            {
                case RTK_PHY_CTRL_FAST_RETRAIN_NFR_ENABLE:
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData &= (~BIT_5);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
                        return ret;
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData &= (~BIT_0);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA6D8, phyData)) != RT_ERR_OK)
                        return ret;

                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA654, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_11);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA654, phyData)) != RT_ERR_OK)
                        return ret;

                    break;

                case RTK_PHY_CTRL_FAST_RETRAIN_NFR_DISABLE:
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_5);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
                        return ret;
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_0);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA6D8, phyData)) != RT_ERR_OK)
                        return ret;

                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA654, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData &= (~BIT_11);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA654, phyData)) != RT_ERR_OK)
                        return ret;

                    break;

                default:
                    return RT_ERR_INPUT;
            }
            ret = _phy_8224_an_restart(unit, port);
            break;
            case RTK_PHY_CTRL_FAST_RETRAIN:
                switch (value)
                {
                    case RTK_PHY_CTRL_FAST_RETRAIN_ENABLE:
                        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                            return ret;
                        phyData |= (BIT_5);
                        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
                            return ret;
                        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData)) != RT_ERR_OK)
                            return ret;
                        phyData |= (BIT_0);
                        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA6D8, phyData)) != RT_ERR_OK)
                            return ret;
                        break;

                    case RTK_PHY_CTRL_FAST_RETRAIN_DISABLE:
                        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                            return ret;
                        phyData &= (~BIT_5);
                        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
                            return ret;
                        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData)) != RT_ERR_OK)
                            return ret;
                        phyData &= (~BIT_0);
                        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA6D8, phyData)) != RT_ERR_OK)
                            return ret;
                        break;

                    default:
                        return RT_ERR_INPUT;
                }
                ret = _phy_8224_an_restart(unit, port);
                break;
            case RTK_PHY_CTRL_SERDES_USXGMII_AN:
                if ((ret = _phy_8224_sdsRegFieldBit_set(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH3_EN_BIT, value)) != RT_ERR_OK)
                    return ret;
                if ((ret = _phy_8224_sdsRegFieldBit_set(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH2_EN_BIT, value)) != RT_ERR_OK)
                    return ret;
                if ((ret = _phy_8224_sdsRegFieldBit_set(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH1_EN_BIT, value)) != RT_ERR_OK)
                    return ret;
                if ((ret = _phy_8224_sdsRegFieldBit_set(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH0_EN_BIT, value)) != RT_ERR_OK)
                    return ret;
                break;
            case RTK_PHY_CTRL_SERDES_USXGMII_AM_PERIOD:
                if ((ret = phy_8224_sdsAmPeriod_set(unit, port, value)) != RT_ERR_OK)
                    return ret;
                break;
            case RTK_PHY_CTRL_COUNTER_CLEAR:
                if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PHY_MIB_GLOBAL_CONFIG, 3, 0, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (0x1 << port_offset);
                if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_MIB_GLOBAL_CONFIG, 3, 0, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
*      phy_8224_ctrl_get
* Description:
*      Get PHY settings for specific feature
* Input:
*      unit      - unit id
*      port      - port id
*      ctrl_type - setting type
* Output:
*      pValue    - pointer to setting value
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NOT_SUPPORTED
* Note:
*      None
*/
int32
phy_8224_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0, phyData1 = 0;
    uint32          tmp, msb_bit, lsb_bit;
    uint32          reg_addr;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hwp_get_offset_by_baseport_port(unit, base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_MDI_POLARITY_SWAP:
            lsb_bit = (port_offset*RTL8224_PHY_TX_POLARITY_SWAP_OFFSET);
            msb_bit = lsb_bit + (RTL8224_PHY_TX_POLARITY_SWAP_OFFSET - 1);
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PHY_TX_POLARITY_SWAP, msb_bit, lsb_bit, &phyData)) != RT_ERR_OK)
                return ret;

            tmp = 0;
            tmp |= (phyData & BIT_0) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A) : (0);
            tmp |= (phyData & BIT_1) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B) : (0);
            tmp |= (phyData & BIT_2) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C) : (0);
            tmp |= (phyData & BIT_3) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D) : (0);
            *pValue = tmp;
            break;
        case RTK_PHY_CTRL_MDI_INVERSE:
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PHY_MDI_REVERSE, port_offset, port_offset, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_LED_1_MODE:
        case RTK_PHY_CTRL_LED_2_MODE:
        case RTK_PHY_CTRL_LED_3_MODE:
        case RTK_PHY_CTRL_LED_4_MODE:
            ret = _phy_8224_led_mode_get(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_MODE), &phyData);
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE:
            ret = _phy_8224_led_config_blink_rate_get(unit, port, &phyData);
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_2_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_3_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_4_CFG_ACTIVE_LOW:
            ret = _phy_8224_led_config_active_low_get(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW), &phyData);
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_LED_1_CFG_FORCE:
        case RTK_PHY_CTRL_LED_2_CFG_FORCE:
        case RTK_PHY_CTRL_LED_3_CFG_FORCE:
        case RTK_PHY_CTRL_LED_4_CFG_FORCE:
            ret = _phy_8224_led_config_force_get(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_FORCE), &phyData);
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x0, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_0) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_PTP_REFTIME_TOD_DELAY:
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_TOD_DELAY_R, RTL8224_TOD_DELAY_MSB, RTL8224_TOD_DELAY_LSB, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = phyData;
            break;

        case RTK_PHY_CTRL_PTP_DURATION_THRESHOLD:
            if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PTP_TIME_OP_DURATION_R, RTL8224_TIME_OP_DURATION_MSB, RTL8224_TIME_OP_DURATION_LSB, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = phyData;
            break;

        case RTK_PHY_CTRL_PTP_PORT_ROLE:
            reg_addr = (RTL8224_P0_PORT_CTRL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
            if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, RTL8224_P0_PTP_ROLE_MSB, RTL8224_P0_PTP_ROLE_LSB, &phyData)) != RT_ERR_OK)
                return ret;
            switch (phyData)
            {
                case 0x0:
                    *pValue = RTK_PHY_CTRL_PTP_PORT_ROLE_NONE;
                    break;
                case 0x1:
                    *pValue = RTK_PHY_CTRL_PTP_PORT_ROLE_BC_OC;
                    break;
                case 0x2:
                    *pValue = RTK_PHY_CTRL_PTP_PORT_ROLE_E2E_TC;
                    break;
                case 0x3:
                    *pValue = RTK_PHY_CTRL_PTP_PORT_ROLE_P2P_TC;
                    break;
                default:
                    return RT_ERR_FAILED;
            }
            break;

        case RTK_PHY_CTRL_PTP_TX_IMBAL:
            reg_addr = (RTL8224_P0_TX_IMBAL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
            if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, RTL8224_P0_TX_IMBAL_MSB, RTL8224_P0_TX_IMBAL_LSB, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_PTP_RX_IMBAL:
            reg_addr = (RTL8224_P0_RX_IMBAL_R + (RTL8224_P0_REG_PORT_OFFSET*port_offset));
            if ((ret = _phy_8224_intReg_get(unit, port, reg_addr, RTL8224_P0_RX_IMBAL_MSB, RTL8224_P0_RX_IMBAL_LSB, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = phyData;
            break;

        case RTK_PHY_CTRL_PTP_CLOCK_SRC:
            if ((ret = _phy_8224_intRegBit_get(unit, port, RTL8224_PTP_CLK_CTRL_R, RTL8224_CFG_CLK_SRC_OFFSET, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData == 0) ? RTK_PHY_CTRL_PTP_CLOCK_SRC_INT : RTK_PHY_CTRL_PTP_CLOCK_SRC_EXT;
            break;
        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
            ret = _phy_8224_temperature_threshold_get(unit, port, ctrl_type, &phyData);
            *pValue = phyData;
            break;
        case RTK_PHY_CTRL_SERDES_TX_POLARITY:
            if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, 0x6, 0x2, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_14) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_SERDES_RX_POLARITY:
            if ((ret = phy_rtl8224_sdsReg_get(unit, base_port, 0x6, 0x2, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_13) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN_NFR:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA654, &phyData)) != RT_ERR_OK)
                return ret;
            if (phyData & BIT_11)
            {
                *pValue = RTK_PHY_CTRL_FAST_RETRAIN_NFR_ENABLE;
            }
            else
            {
                *pValue = RTK_PHY_CTRL_FAST_RETRAIN_NFR_DISABLE;
            }
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (BIT_5);
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData1)) != RT_ERR_OK)
                return ret;
            phyData1 &= (BIT_0);
            if ((phyData != 0) || (phyData1 != 0))
            {
                *pValue = RTK_PHY_CTRL_FAST_RETRAIN_ENABLE;
            }
            else
            {
                *pValue = RTK_PHY_CTRL_FAST_RETRAIN_DISABLE;
            }
            break;
        case RTK_PHY_CTRL_SERDES_USXGMII_AN:
            phyData1 = 0;
            if ((ret = _phy_8224_sdsRegFieldBit_get(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH3_EN_BIT, &phyData)) != RT_ERR_OK)
                return ret;
            phyData1 |= phyData;
            if ((ret = _phy_8224_sdsRegFieldBit_get(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH2_EN_BIT, &phyData)) != RT_ERR_OK)
                return ret;
            phyData1 |= phyData;
            if ((ret = _phy_8224_sdsRegFieldBit_get(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH1_EN_BIT, &phyData)) != RT_ERR_OK)
                return ret;
            phyData1 |= phyData;
            if ((ret = _phy_8224_sdsRegFieldBit_get(unit, port, PHY_8224_NWAY_AN_PAGE, PHY_8224_NWAY_AN_REG, PHY_8224_QHSG_AN_CH0_EN_BIT, &phyData)) != RT_ERR_OK)
                return ret;
            phyData1 |= phyData;
            if(phyData1 == 0)
                *pValue = DISABLED;
            else
                *pValue = ENABLED;
            break;
        case RTK_PHY_CTRL_SERDES_USXGMII_AM_PERIOD:
            if ((ret = phy_8224_sdsAmPeriod_get(unit, port, pValue)) != RT_ERR_OK)
                return ret;
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
 *      phy_8226drv_mapperInit
 * Description:
 *      Initialize PHY 8226B driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_8224drv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index                           = RT_PHYDRV_RTL8224;
    pPhydrv->fPhydrv_init                           = phy_8224_init;
    pPhydrv->fPhydrv_reset_set                      = phy_common_c45_reset_set;
    pPhydrv->fPhydrv_reg_mmd_get                    = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set                    = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_ctrl_get                       = phy_8224_ctrl_get;
    pPhydrv->fPhydrv_ctrl_set                       = phy_8224_ctrl_set;
    pPhydrv->fPhydrv_linkStatus_get                 = phy_8224_linkStatus_get;
    pPhydrv->fPhydrv_media_get                      = phy_8224_media_get;
    pPhydrv->fPhydrv_enable_get                     = phy_common_c45_enable_get;
    pPhydrv->fPhydrv_enable_set                     = phy_common_c45_enable_set;
    pPhydrv->fPhydrv_autoNegoEnable_get             = phy_common_c45_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set             = phy_common_c45_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get            = phy_8224_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set            = phy_8224_autoNegoAbility_set;
    pPhydrv->fPhydrv_autoNegoAbilityLocal_get       = phy_8224_autoNegoAbilityLocal_get;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get 	    = phy_8224_autoNegoAbilityPeer_get;
    pPhydrv->fPhydrv_duplex_get 		            = phy_8224_duplex_get;
    pPhydrv->fPhydrv_duplex_set 		            = phy_8224_duplex_set;
    pPhydrv->fPhydrv_speed_get 			            = phy_common_c45_speed_get;
    pPhydrv->fPhydrv_speed_set 			            = phy_common_c45_speed_set;
    pPhydrv->fPhydrv_speedStatus_get 		        = phy_common_c45_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get 	        = phy_common_c45_speedDuplexStatusResReg_get;
    pPhydrv->fPhydrv_masterSlave_get 		        = phy_common_c45_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set                = phy_common_c45_masterSlave_set;
    pPhydrv->fPhydrv_intrStatus_get                 = phy_8224_intrStatus_get;
    pPhydrv->fPhydrv_intrEnable_get                 = phy_8224_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set                 = phy_8224_intrEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get                  = phy_8224_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set                  = phy_8224_eeeEnable_set;
    pPhydrv->fPhydrv_eeepEnable_get                 = phy_8224_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set                 = phy_8224_eeepEnable_set;
    pPhydrv->fPhydrv_crossOverStatus_get            = phy_8224_crossOverStatus_get;
    pPhydrv->fPhydrv_crossOverMode_get              = phy_8224_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set              = phy_8224_crossOverMode_set;
    pPhydrv->fPhydrv_liteEnable_get                 = phy_8224_liteEnable_get;
    pPhydrv->fPhydrv_liteEnable_set                 = phy_8224_liteEnable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set               = phy_8224_c45_ieeeTestMode_set;
    pPhydrv->fPhydrv_loopback_get                   = phy_common_c45_loopback_get;
    pPhydrv->fPhydrv_loopback_set                   = phy_common_c45_loopback_set;
    pPhydrv->fPhydrv_macIntfSerdesMode_get          = phy_8224_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get    = phy_8224_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get  = phy_8224_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set  = phy_8224_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get            = phy_8224_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set            = phy_8224_downSpeedEnable_set;
    pPhydrv->fPhydrv_downSpeedStatus_get            = phy_8224_downSpeedStatus_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_get          = phy_8224_mdiLoopbackEnable_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_set          = phy_8224_mdiLoopbackEnable_set;
    pPhydrv->fPhydrv_portEyeMonitor_start           = phy_8224_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get         = phy_8224_eyeMonitorInfo_get;
    pPhydrv->fPhydrv_sdsEyeParam_get                = phy_8224_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set                = phy_8224_sdsEyeParam_set;
    pPhydrv->fPhydrv_sdsReg_get                     = phy_8224_sdsReg_get;
    pPhydrv->fPhydrv_sdsReg_set                     = phy_8224_sdsReg_set;
    pPhydrv->fPhydrv_rtctResult_get                 = phy_8224_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start                     = phy_8224_rtct_start;
    pPhydrv->fPhydrv_dbgCounter_get                 = phy_8224_dbgCounter_get;

#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_ptpRefTime_get                = phy_8224_ptp_portRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set                = phy_8224_ptp_portRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set          = phy_8224_ptp_portRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeFreqCfg_get         = phy_8224_ptp_portRefTimeFreq_get;
    pPhydrv->fPhydrv_ptpRefTimeFreqCfg_set         = phy_8224_ptp_portRefTimeFreq_set;
    pPhydrv->fPhydrv_ptpEnable_get                 = phy_8224_ptp_portPtpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set                 = phy_8224_ptp_portPtpEnable_set;
    pPhydrv->fPhydrv_ptpTxInterruptStatus_get      = phy_8224_ptp_portPtpTxInterruptStatus_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_get        = phy_8224_ptp_portPtpInterruptEnable_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_set        = phy_8224_ptp_portPtpInterruptEnable_set;
    pPhydrv->fPhydrv_ptpIgrTpid_get                = phy_8224_ptp_portPtpVlanTpid_get;
    pPhydrv->fPhydrv_ptpIgrTpid_set                = phy_8224_ptp_portPtpVlanTpid_set;
    pPhydrv->fPhydrv_ptpOper_get                   = phy_8224_ptp_portPtpOper_get;
    pPhydrv->fPhydrv_ptpOper_set                   = phy_8224_ptp_portPtpOper_set;
    pPhydrv->fPhydrv_ptpLatchTime_get              = phy_8224_ptp_portPtpLatchTime_get;
    pPhydrv->fPhydrv_ptpTxTimestampFifo_get        = phy_8224_ptp_ptpTxTimestampFifo_get;
    pPhydrv->fPhydrv_ptp1PPSOutput_get             = phy_8224_ptp_ptp1PPSOutput_get;
    pPhydrv->fPhydrv_ptp1PPSOutput_set             = phy_8224_ptp_ptp1PPSOutput_set;
    pPhydrv->fPhydrv_ptpClockOutput_get            = phy_8224_ptp_ptpClockOutput_get;
    pPhydrv->fPhydrv_ptpClockOutput_set            = phy_8224_ptp_ptpClockOutput_set;
    pPhydrv->fPhydrv_ptpOutputSigSel_get           = phy_8224_ptp_portPtpOutputSigSel_get;
    pPhydrv->fPhydrv_ptpOutputSigSel_set           = phy_8224_ptp_portPtpOutputSigSel_set;
    pPhydrv->fPhydrv_ptpLinkDelay_get              = phy_8224_ptp_portPtpLinkDelay_get;
    pPhydrv->fPhydrv_ptpLinkDelay_set              = phy_8224_ptp_portPtpLinkDelay_set;
#endif
} /* end of phy_8224drv_mapperInit*/
