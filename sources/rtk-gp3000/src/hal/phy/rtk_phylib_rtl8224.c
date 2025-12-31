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
#include "rtk_phylib.h"
#include "rtk_phy.h"
#include "rtk_phylib_rtl8224.h"
#include "phy_rtl8224_patch.h"
#include "error.h"
#include "rtk_osal.h"

/*
 * Symbol Definition
 */
#define RTL8224_SWREG_ADDR(_addr)            ((_addr) & (0xffff))

rtk_port_t _phy_8224_base_port_get(uint32 unit, rtk_port_t port)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;

    return priv->basePort;
}

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
    int32           ret = RT_ERR_OK;
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

    base_port = _phy_8224_base_port_get(unit, port);

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

static int32 _phy_8224_intReg_set(uint32 unit, rtk_port_t port, uint32 reg_addr, uint8 msb, uint8 lsb, uint32 data)
{
    int32           ret = RT_ERR_OK;
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

    base_port = _phy_8224_base_port_get(unit, port);

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

    base_port = _phy_8224_base_port_get(unit, port);
    if ((ret = phy_8224_get_port_offset(base_port, port, &port_offset)) != RT_ERR_OK)
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
        if ((ret = _phy_8224_intReg_set(unit, port, 0xbb005f78, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, 0xbb005f68, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;

        /* Disable RLFD Interrupt pin output */
        if ((ret = _phy_8224_intReg_set(unit, port, 0xbb005f64, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, 0xbb005f40, RTL8224_INTER_REG_MSB, RTL8224_INTER_REG_LSB, 0)) != RT_ERR_OK)
            return ret;
        /* Enable RLFD IMR */
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_RLFD_CTRL_REG, RTL8224_RLFD_SEL_OFFSET, RTL8224_RLFD_EN_OFFSET, 1)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_CFG_PHY_POLL_CMD1_REG, RTL8224_CMD_RD_EN_3_0_MSB, RTL8224_CMD_RD_EN_3_0_LSB, 7)) != RT_ERR_OK)
            return ret;

        /* Enable Global IMR for interrupt GPIO output, high level trigger*/
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_ISR_SW_INT_MODE, RTL8224_SWITCH_IE_OFFSET, RTL8224_SWITCH_IE_OFFSET, 1)) != RT_ERR_OK)
            return ret;
        if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_ISR_SW_INT_MODE, RTL8224_SWITCH_INT_MODE_MSB, RTL8224_SWITCH_INT_MODE_OFFSET, PHY_8224_INT_MODE_LOW_LEVEL)) != RT_ERR_OK)
            return ret;
    }

    /*All ports*/
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (0x1 << PHY_8224_RLFD_ENABLE_OFFSET);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

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


int32
_phy_8224_common_c45_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_11) ? (DISABLED) : (ENABLED);
    return ret;
}

int32
_phy_8224_common_c45_speedDuplexStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, spd;

    *pSpeed = PORT_SPEED_10M;
    *pDuplex = PORT_HALF_DUPLEX;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA434, &phyData)) != RT_ERR_OK)
        return ret;

    /* [10:9,5:4] */
    spd = ((phyData & (0x3 << 9)) >> (9 - 2)) | ((phyData & (0x3 << 4)) >> 4);
    switch (spd)
    {
        case 0x0:
          *pSpeed = PORT_SPEED_10M;
          break;
        case 0x1:
          *pSpeed = PORT_SPEED_100M;
          break;
        case 0x2:
          *pSpeed = PORT_SPEED_1000M;
          break;
        case 0x3:
          *pSpeed = PORT_SPEED_500M;
          break;
        case 0x4:
          *pSpeed = PORT_SPEED_10G;
          break;
        case 0x5:
          *pSpeed = PORT_SPEED_2_5G;
          break;
        case 0x6:
          *pSpeed = PORT_SPEED_5G;
          break;
        case 0x7:
          *pSpeed = PORT_SPEED_2_5G_LITE;
          break;
        case 0x8:
          *pSpeed = PORT_SPEED_5G_LITE;
          break;
        case 0x9:
          *pSpeed = PORT_SPEED_10G_LITE;
          break;
        default:
          *pSpeed = PORT_SPEED_10M;
          break;
    }

    *pDuplex = (phyData & 0x8)? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;
    return RT_ERR_OK;
}

int32
_phy_8224_tx_polarity_swap_set(rtk_port_t port, uint32 value)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    int32           ret = RT_ERR_OK;
    uint32          phyData, msb_bit, lsb_bit;
    uint32          unit = 0;
    rtk_port_t      base_port;
    uint8           port_offset = 0;

    base_port = priv->basePort;
    if ((ret = phy_8224_get_port_offset(base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;


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

    return ret;
}


int32
_phy_8224_mdi_reverse_set(rtk_port_t port, uint32 value)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    int32           ret = RT_ERR_OK;
    uint32          phyData;
    uint32          unit = 0;
    rtk_port_t      base_port;
    uint8           port_offset = 0;

    base_port = priv->basePort;
    if ((ret = phy_8224_get_port_offset(base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8224_intReg_get(unit, port, RTL8224_PHY_MDI_REVERSE, port_offset, port_offset, &phyData)) != RT_ERR_OK)
        return ret;
    /* PortOffset 0&1 --> 0: Reverse, 1: Normal */
    /* PortOffset 2&3 --> 0: Normal,  1: Reverse */
    if(value != 0) /*Need to reverse*/
    {
        switch(port_offset)
        {
            case 0:
            case 1:
                phyData = 0;
                break;
            case 2:
            case 3:
                phyData = 1;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }else{
        switch(port_offset)
        {
            case 0:
            case 1:
        phyData = 1;
                break;
            case 2:
            case 3:
        phyData = 0;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    if ((ret = _phy_8224_intReg_set(unit, port, RTL8224_PHY_MDI_REVERSE, port_offset, port_offset, phyData)) != RT_ERR_OK)
        return ret;


    return ret;
}

rtk_port_t
_phy_8224_get_phydevice_by_offset(int32 unit, rtk_port_t baseId, uint8 offset)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)baseId->priv;

    if(priv->memberPort[offset] != NULL)
    {
        return priv->memberPort[offset];
    }else{
        return NULL;
    }
}

static int32 _phy_8224_sdsRegField_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint8 msb, uint8 lsb, uint32 data)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    int32           ret;
    rtk_port_t      base_port;
    uint32          reg_data = 0;
    uint32          mask = 0;

    if ((msb > 15) || (lsb > 15) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }

    base_port = priv->basePort;

    phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, &reg_data);

    ret = _phy_8224_mask_get(msb, lsb, &mask);
    if(ret != RT_ERR_OK)
        return ret;

    reg_data &= ~(mask);
    reg_data |= (((data) << lsb) & mask);

    phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, reg_data);

    return RT_ERR_OK;
}

int32
_phy_8224_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
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

    _phy_8224_common_c45_speedDuplexStatusResReg_get(unit, port, &speed, &duplex);
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
            /* The channel A length is store in UC 0x8ff5 [15:8]*/
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xb87c, 0x8ff5)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb87e, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelALen = ((phyData & 0xFF00) >> 8)*100;

            /* The channel B length is store in UC 0x8ff6 [15:8]*/
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xb87c, 0x8ff6)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb87e, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelBLen = ((phyData & 0xFF00) >> 8)*100;

            /* The channel C length is store in UC 0x8ff7 [15:8]*/
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xb87c, 0x8ff7)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb87e, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelCLen = ((phyData & 0xFF00) >> 8)*100;

            /* The channel D length is store in UC 0x8ff8 [15:8]*/
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xb87c, 0x8ff8)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xb87e, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelDLen = ((phyData & 0xFF00) >> 8)*100;
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
_phy_8224_intr_enable(rtk_port_t port, uint32 enable)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData, bit_idx;
    rtk_port_t      base_port = 0;
    uint8           port_offset = 0;
    uint32          unit = 0;

    base_port = _phy_8224_base_port_get(unit, port);
    if ((ret = phy_8224_get_port_offset(base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;

    if (enable == DISABLED)
        phyData = 0;
    else
        phyData = 1;

    bit_idx = (RTL8224_IMR_EXT_GPHY_3_0_OFFSET + port_offset);
    ret = _phy_8224_intRegBit_set(unit, port, RTL8224_IMR_EXT_GPHY, bit_idx, phyData);

    return ret;
}

int32
_phy_8224_intr_read_clear(rtk_port_t port, uint32 *pStatus)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0;
    rtk_port_t      base_port;
    uint8           port_offset;
    uint32          unit = 0;

    base_port = _phy_8224_base_port_get(unit, port);
    if ((ret = phy_8224_get_port_offset(base_port, port, &port_offset)) != RT_ERR_OK)
        return ret;


    /* Read and Clear */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA43A, &phyData)) != RT_ERR_OK)
        return ret;

    /* Clean ISR_EXT_GPHY */
    /* Write 1 to clear */
    if ((ret = _phy_8224_intRegBit_set(unit, port, RTL8224_ISR_EXT_GPHY, port_offset, 1)) != RT_ERR_OK)
       return ret;

    *pStatus = phyData;
    return ret;
}

int32
phy_8224_sdsReg_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 data)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    rtk_port_t      base_port;

    base_port = priv->basePort;

    return phy_rtl8224_sdsReg_set(unit, base_port, sdsPage, sdsReg, data);
}


int32
phy_8224_sdsOpCode_set(uint32 unit, rtk_port_t port, uint32 opCode)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    rtk_port_t      base_port;

    base_port = priv->basePort;


    return _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_NWAY_OPCODE_PAGE, PHY_8224_NWAY_OPCODE_REG, PHY_8224_NWAY_OPCODE_HIGH_BIT, PHY_8224_NWAY_OPCODE_LOW_BIT, opCode);
}


int32
phy_8224_sdsAmPeriod_set(uint32 unit, rtk_port_t port, uint32 amPeriod)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    rtk_port_t      base_port;

    base_port = priv->basePort;


    return _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PERIOD_PAGE, PHY_8224_AM_PERIOD_REG, PHY_8224_AM_PERIOD_HIGH_BIT, PHY_8224_AM_PERIOD_LOW_BIT, amPeriod);
}

int32
phy_8224_sdsAllAm_set(uint32 unit, rtk_port_t port, uint32 value)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    rtk_port_t      base_port;

    base_port = priv->basePort;

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

int32
phy_8224_sdsExtra_set(uint32 unit, rtk_port_t port)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    rtk_port_t      base_port;

    base_port = priv->basePort;

    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_AM_PERIOD_PAGE, PHY_8224_TGR_PRO_0_REG17, PHY_8224_USXG_V2P1P2_RXOFF_HIGH_BIT, PHY_8224_USXG_V2P1P2_RXOFF_LOW_BIT, 0x1);
    _phy_8224_sdsRegField_set(unit, base_port, PHY_8224_NWAY_OPCODE_PAGE, PHY_8224_TGR_PRO_1_REG19, PHY_8224_USXG_RXDV_OFF_HIGH_BIT, PHY_8224_USXG_RXDV_OFF_LOW_BIT, 0x1);

    return RT_ERR_OK;
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

int
phy_8224_get_port_offset(rtk_port_t baseId, rtk_port_t port, uint8 *offset)
{
    *offset = (uint8)((port->mdio.addr) - (baseId->mdio.addr));
    if(*offset >= RTL8224_PORT_NUM)
        return -1;
    else
        return 0;
}

int32
phy_8224_sdsReg_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)port->priv;
    rtk_port_t      base_port;

    base_port = priv->basePort;

    return phy_rtl8224_sdsReg_get(unit, base_port, sdsPage, sdsReg, pData);

}




