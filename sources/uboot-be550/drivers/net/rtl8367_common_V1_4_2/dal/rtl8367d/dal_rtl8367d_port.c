/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
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
 * Purpose : RTK switch high-level API for RTL8367/RTL8367D
 * Feature : Here is a list of all functions and variables in Port module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_port.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>
#include <linux/string.h>

#define RTL8367D_PORT_SDS_MODE_DISABLE      0x1f
#define RTL8367D_PORT_SDS_MODE_SGMII        0x2
#define RTL8367D_PORT_SDS_MODE_HSGMII       0x12
#define RTL8367D_PORT_SDS_MODE_1000X        0x4
#define RTL8367D_PORT_SDS_MODE_100FX        0x5
#define RTL8367D_PORT_SDS_MODE_1000X_100FX  0x7
#define RTL8367D_PORT_SDS_MODE_FIBER_2P5G   0x16

#define RTL8367D_EXT_PORT_SPEED_10M         0x0
#define RTL8367D_EXT_PORT_SPEED_100M        0x1
#define RTL8367D_EXT_PORT_SPEED_1000M       0x2
#define RTL8367D_EXT_PORT_SPEED_500M        0x3
#define RTL8367D_EXT_PORT_SPEED_2500M       0x5

const rtk_uint32 HSGCfg[][2] = { {0x0427, 0x4E0C}, {0x0428, 0xAA00}, {0x0425, 0x5189}, {0x0424, 0x8414}, {0x0423, 0x1020}, {0x0410, 0x0002}, {0x0504, 0x051B}, {0x0421, 0x8E13}, {0x0422, 0x1140}};
const rtk_uint32 SGCfg[][2] = { {0x0427, 0x4E0C}, {0x0428, 0xAA00}, {0x0425, 0x5189}, {0x0424, 0x8414}, {0x0423, 0x1020}, {0x0410, 0x0002}, {0x0484, 0x011B}, {0x0421, 0x8E13}, {0x0422, 0x1140}};
const rtk_uint32 Fiber1000M100MCfg[][2] = { {0x0427, 0x4E0C}, {0x0428, 0xAA00}, {0x0425, 0x5189}, {0x0424, 0x8414}, {0x0423, 0x1020}, {0x0410, 0x0002}, {0x0484, 0x011B}, {0x0421, 0x8E13}, {0x0422, 0x1140}, {0x0040, 0x2100}, {0x0044, 0x0C01}, {0x0040, 0x1140}, {0x0044, 0x01A0}, {0x0001, 0x0F20}};
const rtk_uint32 Fiber1000MCfg[][2] = { {0x0427, 0x4E0C}, {0x0428, 0xAA00}, {0x0425, 0x5189}, {0x0424, 0x8414}, {0x0423, 0x1020}, {0x0410, 0x0002}, {0x0484, 0x011B}, {0x0421, 0x8E13}, {0x0422, 0x1140}, {0x0040, 0x1140}, {0x0044, 0x01A0}};
const rtk_uint32 Fiber100MCfg[][2] = { {0x0427, 0x4E0C}, {0x0428, 0xAA00}, {0x0425, 0x5189}, {0x0424, 0x8414}, {0x0423, 0x1020}, {0x0410, 0x0002}, {0x0484, 0x011B}, {0x0421, 0x8E13}, {0x0422, 0x1140}, {0x0040, 0x2100}, {0x0044, 0x0C01}};
const rtk_uint32 Fiber2P5GCfg[][2] = { {0x0427, 0x4E0C}, {0x0428, 0xAA00}, {0x0425, 0x5189}, {0x0424, 0x8414}, {0x0423, 0x1020}, {0x0410, 0x0002}, {0x0504, 0x051B}, {0x0421, 0x8E13}, {0x0422, 0x1140}, {0x0040, 0x1140}, {0x0044, 0x01A0}};
const rtk_uint32 SDSNctrlPatch[][2] =
{
    {0x6100, 0x2800},
    {0x6136, 0x0000},
    {0x6135, 0x7000},
    {0x6136, 0xc008},
    {0x6135, 0x7001},
    {0x6136, 0x405f},
    {0x6135, 0x7002},
    {0x6136, 0x1205},
    {0x6135, 0x7003},
    {0x6136, 0x2200},
    {0x6135, 0x7004},
    {0x6136, 0x4274},
    {0x6135, 0x7005},
    {0x6136, 0x4755},
    {0x6135, 0x7006},
    {0x6136, 0x2200},
    {0x6135, 0x7007},
    {0x6136, 0xc008},
    {0x6135, 0x7008},
    {0x6136, 0x6011},
    {0x6135, 0x7009},
    {0x6136, 0xac01},
    {0x6135, 0x700a},
    {0x6136, 0x0000},
    {0x6135, 0x700b},
    {0x6136, 0x8c01},
    {0x6135, 0x700c},
    {0x6136, 0x0000},
    {0x6135, 0x700d},
    {0x6136, 0x0800},
    {0x6135, 0x700e},
    {0x6136, 0xd301},
    {0x6135, 0x700f},
    {0x6136, 0xd4ff},
    {0x6135, 0x7010},
    {0x6136, 0xc007},
    {0x6135, 0x7011},
    {0x6136, 0x6000},
    {0x6135, 0x7012},
    {0x6136, 0x4000},
    {0x6135, 0x7013},
    {0x6136, 0x6000},
    {0x6135, 0x7014},
    {0x6136, 0x4000},
    {0x6135, 0x7015},
    {0x6136, 0xc008},
    {0x6135, 0x7016},
    {0x6136, 0x0800},
    {0x6135, 0x7017},
    {0x6136, 0xc008},
    {0x6135, 0x7018},
    {0x6136, 0x64f4},
    {0x6135, 0x7019},
    {0x6136, 0x120f},
    {0x6135, 0x701a},
    {0x6136, 0x64b4},
    {0x6135, 0x701b},
    {0x6136, 0xd000},
    {0x6135, 0x701c},
    {0x6136, 0xcf02},
    {0x6135, 0x701d},
    {0x6136, 0xce93},
    {0x6135, 0x701e},
    {0x6136, 0xcd4a},
    {0x6135, 0x701f},
    {0x6136, 0x1208},
    {0x6135, 0x7020},
    {0x6136, 0xd000},
    {0x6135, 0x7021},
    {0x6136, 0xcf20},
    {0x6135, 0x7022},
    {0x6136, 0xcee6},
    {0x6135, 0x7023},
    {0x6136, 0xcd02},
    {0x6135, 0x7024},
    {0x6136, 0x1208},
    {0x6135, 0x7025},
    {0x6136, 0xd000},
    {0x6135, 0x7026},
    {0x6136, 0xcf3f},
    {0x6135, 0x7027},
    {0x6136, 0xcd01},
    {0x6135, 0x7028},
    {0x6136, 0x1208},
    {0x6135, 0x7029},
    {0x6136, 0xd000},
    {0x6135, 0x702a},
    {0x6136, 0xcfcd},
    {0x6135, 0x702b},
    {0x6136, 0xcd00},
    {0x6135, 0x702c},
    {0x6136, 0x1208},
    {0x6135, 0x702d},
    {0x6136, 0xd000},
    {0x6135, 0x702e},
    {0x6136, 0xcf04},
    {0x6135, 0x702f},
    {0x6136, 0xcd02},
    {0x6135, 0x7030},
    {0x6136, 0x1208},
    {0x6135, 0x7031},
    {0x6136, 0xd004},
    {0x6135, 0x7032},
    {0x6136, 0xcf00},
    {0x6135, 0x7033},
    {0x6136, 0xcd01},
    {0x6135, 0x7034},
    {0x6136, 0x1208},
    {0x6135, 0x7035},
    {0x6136, 0xd000},
    {0x6135, 0x7036},
    {0x6136, 0xcfcd},
    {0x6135, 0x7037},
    {0x6136, 0xcd00},
    {0x6135, 0x7038},
    {0x6136, 0x1208},
    {0x6135, 0x7039},
    {0x6136, 0xcf00},
    {0x6135, 0x703a},
    {0x6136, 0xcd02},
    {0x6135, 0x703b},
    {0x6136, 0x1208},
    {0x6135, 0x703c},
    {0x6136, 0xcfcd},
    {0x6135, 0x703d},
    {0x6136, 0xcd00},
    {0x6135, 0x703e},
    {0x6136, 0x1208},
    {0x6135, 0x703f},
    {0x6136, 0xc008},
    {0x6135, 0x7040},
    {0x6136, 0x64f5},
    {0x6135, 0x7041},
    {0x6136, 0x120f},
    {0x6135, 0x7042},
    {0x6136, 0x64b5},
    {0x6135, 0x7043},
    {0x6136, 0xd000},
    {0x6135, 0x7044},
    {0x6136, 0xcf04},
    {0x6135, 0x7045},
    {0x6136, 0xce93},
    {0x6135, 0x7046},
    {0x6136, 0xcd4a},
    {0x6135, 0x7047},
    {0x6136, 0x1208},
    {0x6135, 0x7048},
    {0x6136, 0xd000},
    {0x6135, 0x7049},
    {0x6136, 0xcf20},
    {0x6135, 0x704a},
    {0x6136, 0xcee6},
    {0x6135, 0x704b},
    {0x6136, 0xcd02},
    {0x6135, 0x704c},
    {0x6136, 0x1208},
    {0x6135, 0x704d},
    {0x6136, 0xd000},
    {0x6135, 0x704e},
    {0x6136, 0xcf3f},
    {0x6135, 0x704f},
    {0x6136, 0xcd01},
    {0x6135, 0x7050},
    {0x6136, 0x1208},
    {0x6135, 0x7051},
    {0x6136, 0xd000},
    {0x6135, 0x7052},
    {0x6136, 0xcfcf},
    {0x6135, 0x7053},
    {0x6136, 0xcd00},
    {0x6135, 0x7054},
    {0x6136, 0x1208},
    {0x6135, 0x7055},
    {0x6136, 0xd000},
    {0x6135, 0x7056},
    {0x6136, 0xcf04},
    {0x6135, 0x7057},
    {0x6136, 0xcd02},
    {0x6135, 0x7058},
    {0x6136, 0x1208},
    {0x6135, 0x7059},
    {0x6136, 0xd004},
    {0x6135, 0x705a},
    {0x6136, 0xcf00},
    {0x6135, 0x705b},
    {0x6136, 0xcd01},
    {0x6135, 0x705c},
    {0x6136, 0x1208},
    {0x6135, 0x705d},
    {0x6136, 0xd000},
    {0x6135, 0x705e},
    {0x6136, 0xcfcf},
    {0x6135, 0x705f},
    {0x6136, 0xcd00},
    {0x6135, 0x7060},
    {0x6136, 0x1208},
    {0x6135, 0x7061},
    {0x6136, 0xcf00},
    {0x6135, 0x7062},
    {0x6136, 0xcd02},
    {0x6135, 0x7063},
    {0x6136, 0x1208},
    {0x6135, 0x7064},
    {0x6136, 0xcfcf},
    {0x6135, 0x7065},
    {0x6136, 0xcd00},
    {0x6135, 0x7066},
    {0x6136, 0x1208},
    {0x6135, 0x7067},
    {0x6136, 0xd000},
    {0x6135, 0x7068},
    {0x6136, 0xcf00},
    {0x6135, 0x7069},
    {0x6136, 0xce93},
    {0x6135, 0x706a},
    {0x6136, 0xcd4a},
    {0x6135, 0x706b},
    {0x6136, 0x1208},
    {0x6135, 0x706c},
    {0x6136, 0xd000},
    {0x6135, 0x706d},
    {0x6136, 0xcf00},
    {0x6135, 0x706e},
    {0x6136, 0xcee6},
    {0x6135, 0x706f},
    {0x6136, 0xcd02},
    {0x6135, 0x7070},
    {0x6136, 0x1208},
    {0x6135, 0x7071},
    {0x6136, 0xd000},
    {0x6135, 0x7072},
    {0x6136, 0xcf3f},
    {0x6135, 0x7073},
    {0x6136, 0xcd01},
    {0x6135, 0x7074},
    {0x6136, 0x1208},
    {0x6135, 0x7075},
    {0x6136, 0xd000},
    {0x6135, 0x7076},
    {0x6136, 0xcfcd},
    {0x6135, 0x7077},
    {0x6136, 0xcd00},
    {0x6135, 0x7078},
    {0x6136, 0x1208},
    {0x6135, 0x7079},
    {0x6136, 0xcfcf},
    {0x6135, 0x707a},
    {0x6136, 0x1208},
    {0x6135, 0x707b},
    {0x6136, 0x2200},
    {0x6135, 0x707c},
    {0x6102, 0x0000},
    {0x6101, 0x0420},
    {0x6100, 0x0000},
    {0x6102, 0x0000},
    {0x6104, 0x0200},
    {0x6103, 0x0400},
    {0x6101, 0x0000}
};

const rtk_uint32 RTCTPatch[][2] =
{
    {0xa436, 0x8160},
    {0xa438, 0x6de5},
    {0xa436, 0x8162},
    {0xa438, 0x9b52},
    {0xa436, 0x8164},
    {0xa438, 0x428f},
    {0xa436, 0x8166},
    {0xa438, 0xe961},
    {0xa436, 0x8174},
    {0xa438, 0x054b},
    {0xa436, 0x8176},
    {0xa438, 0x009a},
    {0xa436, 0x8178},
    {0xa438, 0xf5a9},
    {0xa436, 0x817a},
    {0xa438, 0xe69c},
    {0xa436, 0x8211},
    {0xa438, 0x2a9b},
    {0xa436, 0x8213},
    {0xa438, 0xa9cd},
    {0xa436, 0x8215},
    {0xa438, 0x7935},
    {0xa436, 0x8217},
    {0xa438, 0xaadf},
    {0xa436, 0x819f},
    {0xa438, 0xc313},
    {0xa436, 0x81b5},
    {0xa438, 0x1010},
    {0xa436, 0x81b7},
    {0xa438, 0xea03},
    {0xa436, 0x8186},
    {0xa438, 0x3501}
};

static rtk_api_ret_t _dal_rtl8367d_setAsicPHYOCPReg(rtk_uint32 phyNo, rtk_uint32 ocpAddr, rtk_uint32 ocpData )
{
    ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 busyFlag, checkCounter;
    rtk_uint32 ocpAddrPrefix, ocpAddr9_6, ocpAddr5_1;

    /*Check internal phy access busy or not*/
    retVal = rtl8367d_getAsicReg(RTL8367D_REG_INDRECT_ACCESS_STATUS, &busyFlag);
    if(retVal != RT_ERR_OK)
        return retVal;

    if(busyFlag)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_FPGA_VER_CEN, &regData)) != RT_ERR_OK)
        return retVal;

    /* OCP prefix */
    if (regData == 0)
        ocpAddrPrefix = ((ocpAddr & 0xFC00) >> 10); /* ASIC */
    else
        ocpAddrPrefix = 0; /* FPGA */

    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_GPHY_OCP_MSB_0, RTL8367D_CFG_CPU_OCPADR_MSB_MASK, ocpAddrPrefix)) != RT_ERR_OK)
        return retVal;

    /*prepare access data*/
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_INDRECT_ACCESS_WRITE_DATA, ocpData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /*prepare access address*/
    ocpAddr9_6 = ((ocpAddr >> 6) & 0x000F);
    ocpAddr5_1 = ((ocpAddr >> 1) & 0x001F);
    regData = RTL8367D_PHY_BASE | (ocpAddr9_6 << 8) | (phyNo << RTL8367D_PHY_OFFSET) | ocpAddr5_1;
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_INDRECT_ACCESS_ADDRESS, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /*Set WRITE Command*/
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_INDRECT_ACCESS_CTRL, RTL8367D_CMD_MASK | RTL8367D_RW_MASK);

    checkCounter = 100;
    while(checkCounter)
    {
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_INDRECT_ACCESS_STATUS, &busyFlag);
        if((retVal != RT_ERR_OK) || busyFlag)
        {
            checkCounter --;
            if(0 == checkCounter)
                return RT_ERR_BUSYWAIT_TIMEOUT;
        }
        else
        {
            checkCounter = 0;
        }
    }

    return retVal;
}

static rtk_api_ret_t _dal_rtl8367d_getAsicPHYOCPReg(rtk_uint32 phyNo, rtk_uint32 ocpAddr, rtk_uint32 *pRegData )
{
    ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 busyFlag,checkCounter;
    rtk_uint32 ocpAddrPrefix, ocpAddr9_6, ocpAddr5_1;

    /*Check internal phy access busy or not*/
    retVal = rtl8367d_getAsicReg(RTL8367D_REG_INDRECT_ACCESS_STATUS, &busyFlag);
    if(retVal != RT_ERR_OK)
        return retVal;

    if(busyFlag)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_FPGA_VER_CEN, &regData)) != RT_ERR_OK)
        return retVal;

    /* OCP prefix */
    if (regData == 0)
        ocpAddrPrefix = ((ocpAddr & 0xFC00) >> 10); /* ASIC */
    else
        ocpAddrPrefix = 0; /* FPGA */

    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_GPHY_OCP_MSB_0, RTL8367D_CFG_CPU_OCPADR_MSB_MASK, ocpAddrPrefix)) != RT_ERR_OK)
        return retVal;

    /*prepare access address*/
    ocpAddr9_6 = ((ocpAddr >> 6) & 0x000F);
    ocpAddr5_1 = ((ocpAddr >> 1) & 0x001F);
    regData = RTL8367D_PHY_BASE | (ocpAddr9_6 << 8) | (phyNo << RTL8367D_PHY_OFFSET) | ocpAddr5_1;
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_INDRECT_ACCESS_ADDRESS, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /*Set READ Command*/
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_INDRECT_ACCESS_CTRL, RTL8367D_CMD_MASK );
    if(retVal != RT_ERR_OK)
        return retVal;

    checkCounter = 100;
    while(checkCounter)
    {
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_INDRECT_ACCESS_STATUS, &busyFlag);
        if((retVal != RT_ERR_OK) || busyFlag)
        {
            checkCounter --;
            if(0 == checkCounter)
                return RT_ERR_FAILED;
        }
        else
        {
            checkCounter = 0;
        }
    }

    /*get PHY register*/
    retVal = rtl8367d_getAsicReg(RTL8367D_REG_INDRECT_ACCESS_READ_DATA, &regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    *pRegData = regData;
    return RT_ERR_OK;
}

static rtk_api_ret_t _dal_rtl8367d_setAsicPHYReg(rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 phyData )
{
    rtk_uint32 ocp_addr;

    if(phyAddr > RTL8367D_PHY_REGNOMAX)
        return RT_ERR_PHY_REG_ID;

    ocp_addr = 0xa400 + phyAddr*2;

    return _dal_rtl8367d_setAsicPHYOCPReg(phyNo, ocp_addr, phyData);
}

static rtk_api_ret_t _dal_rtl8367d_getAsicPHYReg(rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 *pRegData )
{
    rtk_uint32 ocp_addr;

    if(phyAddr > RTL8367D_PHY_REGNOMAX)
        return RT_ERR_PHY_REG_ID;

    ocp_addr = 0xa400 + phyAddr*2;

    return _dal_rtl8367d_getAsicPHYOCPReg(phyNo, ocp_addr, pRegData);
}

static rtk_api_ret_t _dal_rtl8367d_setAsicPortExtMode(rtk_uint32 id, rtk_uint32 mode)
{
    rtk_api_ret_t  retVal;
    rtk_uint32     mux;
    rtk_uint32     i;
    rtk_port_media_t media_type;

    if ((id >= 3) || (id == 0))
        return RT_ERR_OUT_OF_RANGE;

    if( (mode == MODE_EXT_TMII_MAC) || (mode == MODE_EXT_TMII_PHY) )
    {
        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_BYPASS_LINE_RATE, id, 1)) != RT_ERR_OK)
            return retVal;
    }
    else
    {
        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_BYPASS_LINE_RATE, id, 0)) != RT_ERR_OK)
            return retVal;
    }

    if (id == 1)
    {
        if (mode == MODE_EXT_DISABLE)
        {
            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_DISABLE)) != RT_ERR_OK)
                return retVal;

            if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_MAC6_SEL_SDS0_OFFSET, 0)) != RT_ERR_OK)
                return retVal;

            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 0)) != RT_ERR_OK)
                return retVal;
        }
        else if ((mode == MODE_EXT_SGMII) || (mode == MODE_EXT_HSGMII) || (mode == MODE_EXT_1000X_100FX) || (mode == MODE_EXT_1000X) || (mode == MODE_EXT_100FX) || (mode == MODE_EXT_FIBER_2P5G))
        {
            if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(UTP_PORT4, &media_type)) != RT_ERR_OK)
                return retVal;

            if (media_type == PORT_MEDIA_FIBER)
            {
                /* SDS0 already used by port 4 */
                return RT_ERR_PORT_ID;
            }

            if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_MAC6_SEL_SDS0_OFFSET, 1)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(SDSNctrlPatch) / (sizeof(rtk_uint32) * 2); i++)
            {
                if( (retVal = rtl8367d_setAsicReg(SDSNctrlPatch[i][0], SDSNctrlPatch[i][1])) != RT_ERR_OK)
                    return retVal;
            }

            switch (mode)
            {
                case MODE_EXT_SGMII:
                    for(i = 0; i < sizeof(SGCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, SGCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, SGCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                            return retVal;
                    }

                    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 0x1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_SGMII)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_HSGMII:
                    for(i = 0; i < sizeof(HSGCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, HSGCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, HSGCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                            return retVal;
                    }

                    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 0x1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_HSGMII)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_1000X_100FX:
                    for(i = 0; i < sizeof(Fiber1000M100MCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber1000M100MCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000M100MCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X_100FX)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_1000X:
                    for(i = 0; i < sizeof(Fiber1000MCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber1000MCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000MCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_100FX:
                    for(i = 0; i < sizeof(Fiber100MCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber100MCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber100MCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_100FX)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_FIBER_2P5G:
                    for(i = 0; i < sizeof(Fiber2P5GCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber2P5GCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber2P5GCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_FIBER_2P5G)) != RT_ERR_OK)
                        return retVal;

                    break;
                default:
                    return RT_ERR_INPUT;
            }

            if( (retVal = rtl8367d_setAsicReg(0x612F, 0x000C)) != RT_ERR_OK)
                return retVal;
        }
        else
            return RT_ERR_INPUT;
    }

    if (id == 2)
    {
        if (mode == MODE_EXT_DISABLE)
        {
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC7_SEL_EXT1_OFFSET, 0)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_DIGITAL_INTERFACE_SELECT, RTL8367D_SELECT_GMII_1_MASK, 0x0)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_DISABLE)) != RT_ERR_OK)
                return retVal;
        }
        else if ((mode == MODE_EXT_RGMII) || (mode == MODE_EXT_MII_MAC) || (mode == MODE_EXT_MII_PHY) || (mode == MODE_EXT_TMII_MAC) ||
                 (mode == MODE_EXT_TMII_PHY) || (mode == MODE_EXT_RMII_MAC) || (mode == MODE_EXT_RMII_PHY))
        {
            /* Configure RGMII DP, DN, E2, MODE */
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_SEL33_EXT1_OFFSET, 1)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_DRI_EXT1_RG_OFFSET, 1)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_DRI_EXT1_OFFSET, 1)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_SLR_EXT1_OFFSET, 1)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_CHIP_DEBUG1, RTL8367D_RG1_DN_MASK, 7)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_CHIP_DEBUG1, RTL8367D_RG1_DP_MASK, 5)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_EXT_TXC_DLY, RTL8367D_EXT1_RGMII_TX_DELAY_MASK, 0)) != RT_ERR_OK)
                return retVal;

            /* Configure RGMII/MII mux to port 7 if UTP_PORT4 is not RGMII mode */
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC4_SEL_EXT1_OFFSET, &mux)) != RT_ERR_OK)
                return retVal;

            if (mux == 0)
            {
                if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC7_SEL_EXT1_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
            }

            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_DIGITAL_INTERFACE_SELECT, RTL8367D_SELECT_GMII_1_MASK, mode)) != RT_ERR_OK)
                return retVal;

            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_DISABLE)) != RT_ERR_OK)
                return retVal;
        }
        else if ((mode == MODE_EXT_SGMII) || (mode == MODE_EXT_HSGMII) || (mode == MODE_EXT_1000X_100FX) || (mode == MODE_EXT_1000X) || (mode == MODE_EXT_100FX) || (mode == MODE_EXT_FIBER_2P5G))
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_DIGITAL_INTERFACE_SELECT, RTL8367D_SELECT_GMII_1_MASK, 0x0)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC7_SEL_EXT1_OFFSET, 0)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(SDSNctrlPatch) / (sizeof(rtk_uint32) * 2); i++)
            {
                if( (retVal = rtl8367d_setAsicReg(SDSNctrlPatch[i][0], SDSNctrlPatch[i][1])) != RT_ERR_OK)
                    return retVal;
            }

            switch (mode)
            {
                case MODE_EXT_SGMII:
                    for(i = 0; i < sizeof(SGCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, SGCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, SGCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CF)) != RT_ERR_OK)
                            return retVal;
                    }

                    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA12PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA33PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_SGMII)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_HSGMII:
                    for(i = 0; i < sizeof(HSGCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, HSGCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, HSGCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CF)) != RT_ERR_OK)
                            return retVal;
                    }

                    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA12PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA33PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_HSGMII)) != RT_ERR_OK)
                        return retVal;
                    break;
                case MODE_EXT_1000X_100FX:
                    for(i = 0; i < sizeof(Fiber1000M100MCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber1000M100MCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000M100MCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CF)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA12PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA33PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X_100FX)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_1000X:
                    for(i = 0; i < sizeof(Fiber1000MCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber1000MCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000MCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CF)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA12PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA33PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_100FX:
                    for(i = 0; i < sizeof(Fiber100MCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber100MCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber100MCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CF)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA12PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA33PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_100FX)) != RT_ERR_OK)
                        return retVal;

                    break;
                case MODE_EXT_FIBER_2P5G:
                    for(i = 0; i < sizeof(Fiber2P5GCfg) / (sizeof(rtk_uint32) * 2); i++)
                    {
                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber2P5GCfg[i][1])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber2P5GCfg[i][0])) != RT_ERR_OK)
                            return retVal;

                        if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CF)) != RT_ERR_OK)
                            return retVal;
                    }

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA12PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS1_MISC0, RTL8367D_PA33PC_EN_S1_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;

                    if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_FIBER_2P5G)) != RT_ERR_OK)
                        return retVal;

                    break;
                default:
                    return RT_ERR_INPUT;
            }

            if( (retVal = rtl8367d_setAsicReg(0x612F, 0x000C)) != RT_ERR_OK)
                return retVal;
        }
        else
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

static rtk_api_ret_t _dal_rtl8367d_getAsicPortExtMode(rtk_uint32 id, rtk_uint32 *pMode)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    switch (id)
    {
        case 1:
            if( (retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if( (retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    switch (regData)
    {
        case RTL8367D_PORT_SDS_MODE_SGMII:
            *pMode = MODE_EXT_SGMII;
            break;
        case RTL8367D_PORT_SDS_MODE_HSGMII:
            *pMode = MODE_EXT_HSGMII;
            break;
        case RTL8367D_PORT_SDS_MODE_1000X:
            *pMode = MODE_EXT_1000X;
            break;
        case RTL8367D_PORT_SDS_MODE_100FX:
            *pMode = MODE_EXT_100FX;
            break;
        case RTL8367D_PORT_SDS_MODE_1000X_100FX:
            *pMode = MODE_EXT_1000X_100FX;
            break;
        case RTL8367D_PORT_SDS_MODE_FIBER_2P5G:
            *pMode = MODE_EXT_FIBER_2P5G;
            break;
        case RTL8367D_PORT_SDS_MODE_DISABLE:
            if(id == 2)
            {
                if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_DIGITAL_INTERFACE_SELECT, RTL8367D_SELECT_GMII_1_MASK, pMode)) != RT_ERR_OK)
                    return retVal;
            }
            else
                *pMode = MODE_EXT_DISABLE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static rtk_api_ret_t _dal_rtl8367d_port_sgmiiLinkStatus_get(rtk_port_t port, rtk_data_t *pSignalDetect, rtk_data_t *pSync, rtk_port_linkStatus_t *pLink)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regValue;

    /* Check Port Valid */
    if(rtk_switch_isSgmiiPort(port) != RT_ERR_OK)
        return RT_ERR_PORT_ID;

    if(NULL == pSignalDetect)
        return RT_ERR_NULL_POINTER;

    if(NULL == pSync)
        return RT_ERR_NULL_POINTER;

    if(NULL == pLink)
        return RT_ERR_NULL_POINTER;

    switch (port)
    {
        case EXT_PORT0:
            if ((retVal = rtl8367d_setAsicReg(0x6601, 0x003D)) != RT_ERR_OK)
                break;

            if ((retVal = rtl8367d_setAsicReg(0x6600, 0x008D)) != RT_ERR_OK)
                break;

            if ((retVal = rtl8367d_getAsicReg(0x6602, &regValue)) != RT_ERR_OK)
                break;
            break;
        case EXT_PORT1:
            if ((retVal = rtl8367d_setAsicReg(0x6601, 0x003D)) != RT_ERR_OK)
                break;

            if ((retVal = rtl8367d_setAsicReg(0x6600, 0x008F)) != RT_ERR_OK)
                break;

            if ((retVal = rtl8367d_getAsicReg(0x6602, &regValue)) != RT_ERR_OK)
                break;
            break;
        default:
            retVal = RT_ERR_PORT_ID;
    }

    if (retVal == RT_ERR_OK)
    {
        *pSignalDetect = (regValue & 0x0100) ? 1 : 0;
        *pSync = (regValue & 0x0001) ? 1 : 0;
        *pLink = (regValue & 0x0010) ? 1 : 0;
    }

    return retVal;
}

static rtk_api_ret_t _dal_rtl8367d_port_sgmiiNway_set(rtk_port_t port, rtk_enable_t state)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regValue;
    rtk_uint32 serdesAddr;

    /* Check Port Valid */
    if(rtk_switch_isSgmiiPort(port) != RT_ERR_OK)
        return RT_ERR_PORT_ID;

    if(state >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    switch (port)
    {
        case EXT_PORT0:
            serdesAddr = 0x0D;
            break;
        case EXT_PORT1:
            serdesAddr = 0x0F;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    if ((retVal = rtl8367d_setAsicReg(0x6601, 0x0002)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(0x6600, 0x0080 | serdesAddr)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(0x6602, &regValue)) != RT_ERR_OK)
        return retVal;

    if(state)
        regValue |= 0x0200;
    else
        regValue &= ~0x0200;

    regValue |= 0x0100;

    if ((retVal = rtl8367d_setAsicReg(0x6602, regValue)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(0x6601, 0x0002)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(0x6600, 0x00C0 | serdesAddr)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

static rtk_api_ret_t _dal_rtl8367d_port_sgmiiNway_get(rtk_port_t port, rtk_enable_t *pState)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regValue;
    rtk_uint32 serdesAddr;

    /* Check Port Valid */
    if(rtk_switch_isSgmiiPort(port) != RT_ERR_OK)
        return RT_ERR_PORT_ID;

    if(NULL == pState)
        return RT_ERR_NULL_POINTER;

    switch (port)
    {
        case EXT_PORT0:
            serdesAddr = 0x0D;
            break;
        case EXT_PORT1:
            serdesAddr = 0x0F;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    if ((retVal = rtl8367d_setAsicReg(0x6601, 0x0002)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(0x6600, 0x0080 | serdesAddr)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(0x6602, &regValue)) != RT_ERR_OK)
        return retVal;

    if((regValue & 0x0300) == 0x0300)
        *pState = ENABLED;
    else if((regValue & 0x0300) == 0x0000)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
}

static rtk_api_ret_t _dal_rtl8367d_port_fiberAbility_set(rtk_port_t port, rtk_port_fiber_ability_t *pAbility)
{
    rtk_api_ret_t retVal;
    rtk_port_media_t media_type;
    rtk_mode_ext_t mode;
    rtk_port_mac_ability_t portAbility;
    rtk_uint32 regData;
    rtk_uint32 sdsId;
    rtk_uint32 i;

    /* Check Port Valid and the port is already configured to fiber mode*/
    if (port == UTP_PORT4)
    {
        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
            return retVal;

        if (media_type != PORT_MEDIA_FIBER)
            return RT_ERR_PORT_ID;

        sdsId = 0xD;
    }
    else if (port == EXT_PORT0)
    {
        if ((retVal = dal_rtl8367d_port_macForceLinkExt_get(port, &mode, &portAbility)) != RT_ERR_OK)
            return retVal;

        if ((mode != MODE_EXT_1000X_100FX) && (mode != MODE_EXT_1000X) && (mode != MODE_EXT_100FX) && (mode != MODE_EXT_FIBER_2P5G))
            return RT_ERR_PORT_ID;

        sdsId = 0xD;
    }
    else if (port == EXT_PORT1)
    {
        if ((retVal = dal_rtl8367d_port_macForceLinkExt_get(port, &mode, &portAbility)) != RT_ERR_OK)
            return retVal;

        if ((mode != MODE_EXT_1000X_100FX) && (mode != MODE_EXT_1000X) && (mode != MODE_EXT_100FX) && (mode != MODE_EXT_FIBER_2P5G))
            return RT_ERR_PORT_ID;

        sdsId = 0xF;
    }
    else
        return RT_ERR_PORT_ID;

    /* NULL pointer checking */
    if (pAbility == NULL)
        return RT_ERR_NULL_POINTER;

    /* UTP_PORT4 doesn't support 2.5G Fiber */
    if ((port == UTP_PORT4) && (pAbility->Full_2P5G == 1))
        return RT_ERR_PORT_ID;

    /* if 2.5G is set, all other speed should be cleared */
    if (pAbility->Full_2P5G == 1)
    {
        if ((pAbility->Full_1000 == 1) || (pAbility->Full_100 == 1))
            return RT_ERR_INPUT;
    }

    /* if Full_100 is the only speed set, AutoNegotiation & FC+AsyFC should be cleared */
    if ((pAbility->Full_100 == 1) && (pAbility->Full_1000 == 0) && (pAbility->Full_2P5G == 0))
    {
        if ((pAbility->AutoNegotiation == 1) || (pAbility->FC == 1) || (pAbility->AsyFC == 1))
            return RT_ERR_INPUT;
    }

    /* Speed ability & Flow Control */
    if (pAbility->Full_2P5G == 1)
    {
        for (i = 0; i < sizeof(Fiber2P5GCfg) / (sizeof(rtk_uint32) * 2); i++)
        {
            regData = Fiber2P5GCfg[i][1];
            if (regData == 0x01A0)
            {
                if (pAbility->AsyFC == 0)
                    regData &= ~(0x0001 << 8);

                if (pAbility->FC == 0)
                    regData &= ~(0x0001 << 7);
            }

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, regData)) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber2P5GCfg[i][0])) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x00C0 | sdsId))) != RT_ERR_OK)
                return retVal;
        }

        if (sdsId == 0xD)
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_FIBER_2P5G)) != RT_ERR_OK)
                return retVal;
        }
        else
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_FIBER_2P5G)) != RT_ERR_OK)
                return retVal;
        }
    }
    else if ((pAbility->Full_1000 == 1) && (pAbility->Full_100 == 1))
    {
        for(i = 0; i < sizeof(Fiber1000M100MCfg) / (sizeof(rtk_uint32) * 2); i++)
        {
            regData = Fiber1000M100MCfg[i][1];
            if (regData == 0x0C01)
            {
                if (pAbility->AsyFC == 0)
                    regData &= ~(0x0001 << 11);

                if (pAbility->FC == 0)
                    regData &= ~(0x0001 << 10);
            }

            if (regData == 0x01A0)
            {
                if (pAbility->AsyFC == 0)
                    regData &= ~(0x0001 << 8);

                if (pAbility->FC == 0)
                    regData &= ~(0x0001 << 7);
            }

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, regData)) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000M100MCfg[i][0])) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x00C0 | sdsId))) != RT_ERR_OK)
                return retVal;
        }

        if (sdsId == 0xD)
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X_100FX)) != RT_ERR_OK)
                return retVal;
        }
        else
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X_100FX)) != RT_ERR_OK)
                return retVal;
        }
    }
    else if(pAbility->Full_1000 == 1)
    {
        for(i = 0; i < sizeof(Fiber1000MCfg) / (sizeof(rtk_uint32) * 2); i++)
        {
            regData = Fiber1000MCfg[i][1];
            if (regData == 0x01A0)
            {
                if (pAbility->AsyFC == 0)
                    regData &= ~(0x0001 << 8);

                if (pAbility->FC == 0)
                    regData &= ~(0x0001 << 7);
            }

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, regData)) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000MCfg[i][0])) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x00C0 | sdsId))) != RT_ERR_OK)
                return retVal;
        }

        if (sdsId == 0xD)
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X)) != RT_ERR_OK)
                return retVal;
        }
        else
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X)) != RT_ERR_OK)
                return retVal;
        }
    }
    else if(pAbility->Full_100 == 1)
    {
        for(i = 0; i < sizeof(Fiber100MCfg) / (sizeof(rtk_uint32) * 2); i++)
        {
            regData = Fiber100MCfg[i][1];
            if (regData == 0x0C01)
            {
                if (pAbility->AsyFC == 0)
                    regData &= ~(0x0001 << 11);

                if (pAbility->FC == 0)
                    regData &= ~(0x0001 << 10);
            }

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, regData)) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber100MCfg[i][0])) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x00C0 | sdsId))) != RT_ERR_OK)
                return retVal;
        }

        if (sdsId == 0xD)
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_100FX)) != RT_ERR_OK)
                return retVal;
        }
        else
        {
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, RTL8367D_PORT_SDS_MODE_100FX)) != RT_ERR_OK)
                return retVal;
        }
    }

    /* Restart N-way & AutoNegotiation */
    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, 0x0040)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x0080 | sdsId))) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_SDS_INDACS_DATA, &regData)) != RT_ERR_OK)
        return retVal;

    /* AutoNegotiation */
    if (pAbility->AutoNegotiation == 1)
        regData |= (0x0001 << 12);
    else
        regData &= ~(0x0001 << 12);

    /* Restart N-way */
    regData |= (0x0001 << 9);

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, 0x0040)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x00C0 | sdsId))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

static rtk_api_ret_t _dal_rtl8367d_port_fiberAbility_get(rtk_port_t port, rtk_port_fiber_ability_t *pAbility)
{
    rtk_api_ret_t retVal;
    rtk_port_media_t media_type;
    rtk_mode_ext_t mode;
    rtk_port_mac_ability_t portAbility;
    rtk_uint32 regData;
    rtk_uint32 sdsId;
    rtk_uint32 sdsMode;

    /* Check Port Valid and the port is already configured to fiber mode*/
    if (port == UTP_PORT4)
    {
        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
            return retVal;

        if (media_type != PORT_MEDIA_FIBER)
            return RT_ERR_PORT_ID;

        sdsId = 0xD;
    }
    else if (port == EXT_PORT0)
    {
        if ((retVal = dal_rtl8367d_port_macForceLinkExt_get(port, &mode, &portAbility)) != RT_ERR_OK)
            return retVal;

        if ((mode != MODE_EXT_1000X_100FX) && (mode != MODE_EXT_1000X) && (mode != MODE_EXT_100FX) && (mode != MODE_EXT_FIBER_2P5G))
            return RT_ERR_PORT_ID;

        sdsId = 0xD;
    }
    else if (port == EXT_PORT1)
    {
        if ((retVal = dal_rtl8367d_port_macForceLinkExt_get(port, &mode, &portAbility)) != RT_ERR_OK)
            return retVal;

        if ((mode != MODE_EXT_1000X_100FX) && (mode != MODE_EXT_1000X) && (mode != MODE_EXT_100FX) && (mode != MODE_EXT_FIBER_2P5G))
            return RT_ERR_PORT_ID;

        sdsId = 0xF;
    }
    else
        return RT_ERR_PORT_ID;

    /* NULL pointer checking */
    if (pAbility == NULL)
        return RT_ERR_NULL_POINTER;

    memset(pAbility, 0x00, sizeof(rtk_port_fiber_ability_t));

    /* Speed */
    if ((port == UTP_PORT4) || (port == EXT_PORT0))
    {
        if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, &sdsMode)) != RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_SDS1_MISC0, RTL8367D_SDS1_MODE_MASK, &sdsMode)) != RT_ERR_OK)
            return retVal;
    }

    switch (sdsMode)
    {
        case RTL8367D_PORT_SDS_MODE_FIBER_2P5G:
            pAbility->Full_2P5G = 1;
            break;
        case RTL8367D_PORT_SDS_MODE_1000X_100FX:
            pAbility->Full_1000 = 1;
            pAbility->Full_100 = 1;
            break;
        case RTL8367D_PORT_SDS_MODE_1000X:
            pAbility->Full_1000 = 1;
            break;
        case RTL8367D_PORT_SDS_MODE_100FX:
            pAbility->Full_100 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    /* AutoNegotiation */
    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, 0x0040)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x0080 | sdsId))) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_SDS_INDACS_DATA, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData & 0x1000)
        pAbility->AutoNegotiation = 1;
    else
        pAbility->AutoNegotiation = 0;

    /* Flow Control */
    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, 0x0044)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, (0x0080 | sdsId))) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_SDS_INDACS_DATA, &regData)) != RT_ERR_OK)
        return retVal;

    if (sdsMode == RTL8367D_PORT_SDS_MODE_100FX)
    {
        if (regData & (0x0001 << 11))
            pAbility->AsyFC = 1;

        if(regData & (0x0001 << 10))
            pAbility->FC = 1;
    }
    else
    {
        if (regData & (0x0001 << 8))
            pAbility->AsyFC = 1;

        if(regData & (0x0001 << 7))
            pAbility->FC = 1;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyAutoNegoAbility_set
 * Description:
 *      Set ethernet PHY auto-negotiation desired ability.
 * Input:
 *      port        - port id.
 *      pAbility    - Ability structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      If Full_1000 bit is set to 1, the AutoNegotiation will be automatic set to 1. While both AutoNegotiation and Full_1000 are set to 0, the PHY speed and duplex selection will
 *      be set as following 100F > 100H > 10F > 10H priority sequence.
 */
rtk_api_ret_t dal_rtl8367d_port_phyAutoNegoAbility_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_api_ret_t       retVal;
    rtk_uint32          phyData;
    rtk_uint32          phyEnMsk0;
    rtk_uint32          phyEnMsk4;
    rtk_uint32          phyEnMsk9;
    rtk_port_media_t    media_type;
    rtk_port_fiber_ability_t fiberAbility;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    if (pAbility->Half_10 >= RTK_ENABLE_END || pAbility->Full_10 >= RTK_ENABLE_END ||
       pAbility->Half_100 >= RTK_ENABLE_END || pAbility->Full_100 >= RTK_ENABLE_END ||
       pAbility->Full_1000 >= RTK_ENABLE_END || pAbility->AutoNegotiation >= RTK_ENABLE_END ||
       pAbility->AsyFC >= RTK_ENABLE_END || pAbility->FC >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if (rtk_switch_isComboPort(port) == RT_ERR_OK)
    {
        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
            return retVal;

        if(media_type == PORT_MEDIA_FIBER)
        {
            memset(&fiberAbility, 0x00, sizeof(rtk_port_fiber_ability_t));
            fiberAbility.AutoNegotiation = pAbility->AutoNegotiation;
            fiberAbility.AsyFC = pAbility->AsyFC;
            fiberAbility.FC = pAbility->FC;
            fiberAbility.Full_1000 = pAbility->Full_1000;
            fiberAbility.Full_100 = pAbility->Full_100;
            return dal_rtl8367d_port_fiberAbility_set(port, &fiberAbility);
        }
        else if (media_type == PORT_MEDIA_RGMII)
            return RT_ERR_PORT_ID;
    }

    /*for PHY auto mode setup*/
    pAbility->AutoNegotiation = 1;

    phyEnMsk0 = 0;
    phyEnMsk4 = 0;
    phyEnMsk9 = 0;

    if (1 == pAbility->Half_10)
    {
        /*10BASE-TX half duplex capable in reg 4.5*/
        phyEnMsk4 = phyEnMsk4 | (1 << 5);

        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 & (~(1 << 13));
    }

    if (1 == pAbility->Full_10)
    {
        /*10BASE-TX full duplex capable in reg 4.6*/
        phyEnMsk4 = phyEnMsk4 | (1 << 6);
        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 & (~(1 << 13));

        /*Full duplex mode in reg 0.8*/
        phyEnMsk0 = phyEnMsk0 | (1 << 8);

    }

    if (1 == pAbility->Half_100)
    {
        /*100BASE-TX half duplex capable in reg 4.7*/
        phyEnMsk4 = phyEnMsk4 | (1 << 7);
        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 | (1 << 13);
    }


    if (1 == pAbility->Full_100)
    {
        /*100BASE-TX full duplex capable in reg 4.8*/
        phyEnMsk4 = phyEnMsk4 | (1 << 8);
        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 | (1 << 13);
        /*Full duplex mode in reg 0.8*/
        phyEnMsk0 = phyEnMsk0 | (1 << 8);
    }


    if (1 == pAbility->Full_1000)
    {
        /*1000 BASE-T FULL duplex capable setting in reg 9.9*/
        phyEnMsk9 = phyEnMsk9 | (1 << 9);

        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 | (1 << 6);
        phyEnMsk0 = phyEnMsk0 & (~(1 << 13));


        /*Auto-Negotiation setting in reg 0.12*/
        phyEnMsk0 = phyEnMsk0 | (1 << 12);

     }

    if (1 == pAbility->AutoNegotiation)
    {
        /*Auto-Negotiation setting in reg 0.12*/
        phyEnMsk0 = phyEnMsk0 | (1 << 12);
    }

    if (1 == pAbility->AsyFC)
    {
        /*Asymetric flow control in reg 4.11*/
        phyEnMsk4 = phyEnMsk4 | (1 << 11);
    }
    if (1 == pAbility->FC)
    {
        /*Flow control in reg 4.10*/
        phyEnMsk4 = phyEnMsk4 | (1 << 10);
    }

    /*1000 BASE-T control register setting*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_1000_BASET_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x0200)) | phyEnMsk9 ;

    if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_1000_BASET_CONTROL_REG, phyData)) != RT_ERR_OK)
        return retVal;

    /*Auto-Negotiation control register setting*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_AN_ADVERTISEMENT_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x0DE0)) | phyEnMsk4;
    if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_AN_ADVERTISEMENT_REG, phyData)) != RT_ERR_OK)
        return retVal;

    /*Control register setting and restart auto*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x3140)) | phyEnMsk0;
    /*If have auto-negotiation capable, then restart auto negotiation*/
    if (1 == pAbility->AutoNegotiation)
    {
        phyData = phyData | (1 << 9);
    }

    if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      port - Port id.
 * Output:
 *      pAbility - Ability structure
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get the capablity of specified PHY.
 */
rtk_api_ret_t dal_rtl8367d_port_phyAutoNegoAbility_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_api_ret_t       retVal;
    rtk_uint32          phyData0;
    rtk_uint32          phyData4;
    rtk_uint32          phyData9;
    rtk_port_media_t    media_type;
    rtk_port_fiber_ability_t fiberAbility;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    if (rtk_switch_isComboPort(port) == RT_ERR_OK)
    {
        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
            return retVal;

        if(media_type == PORT_MEDIA_FIBER)
        {
            memset(&fiberAbility, 0x00, sizeof(rtk_port_fiber_ability_t));
            if ((retVal = dal_rtl8367d_port_fiberAbility_get(port,  &fiberAbility)) != RT_ERR_OK)
                return retVal;

            memset(pAbility, 0x00, sizeof(rtk_port_phy_ability_t));
            pAbility->AutoNegotiation = fiberAbility.AutoNegotiation;
            pAbility->AsyFC = fiberAbility.AsyFC;
            pAbility->FC = fiberAbility.FC;
            pAbility->Full_1000 = fiberAbility.Full_1000;
            pAbility->Full_100 = fiberAbility.Full_100;
            return RT_ERR_OK;
        }
    }

    /*Control register setting and restart auto*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return retVal;

    /*Auto-Negotiation control register setting*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return retVal;

    /*1000 BASE-T control register setting*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return retVal;

    if (phyData9 & (1 << 9))
        pAbility->Full_1000 = 1;
    else
        pAbility->Full_1000 = 0;

    if (phyData4 & (1 << 11))
        pAbility->AsyFC = 1;
    else
        pAbility->AsyFC = 0;

    if (phyData4 & (1 << 10))
        pAbility->FC = 1;
    else
        pAbility->FC = 0;


    if (phyData4 & (1 << 8))
        pAbility->Full_100 = 1;
    else
        pAbility->Full_100 = 0;

    if (phyData4 & (1 << 7))
        pAbility->Half_100 = 1;
    else
        pAbility->Half_100 = 0;

    if (phyData4 & (1 << 6))
        pAbility->Full_10 = 1;
    else
        pAbility->Full_10 = 0;

    if (phyData4 & (1 << 5))
        pAbility->Half_10 = 1;
    else
        pAbility->Half_10 = 0;


    if (phyData0 & (1 << 12))
        pAbility->AutoNegotiation = 1;
    else
        pAbility->AutoNegotiation = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode.
 * Input:
 *      port        - port id.
 *      pAbility    - Ability structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      While both AutoNegotiation and Full_1000 are set to 0, the PHY speed and duplex selection will
 *      be set as following 100F > 100H > 10F > 10H priority sequence.
 *      This API can be used to configure combo port in fiber mode.
 *      The possible parameters in fiber mode are Full_1000 and Full 100.
 *      All the other fields in rtk_port_phy_ability_t will be ignored in fiber port.
 */
rtk_api_ret_t dal_rtl8367d_port_phyForceModeAbility_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
     rtk_api_ret_t      retVal;
     rtk_uint32         phyData;
     rtk_uint32         phyEnMsk0;
     rtk_uint32         phyEnMsk4;
     rtk_uint32         phyEnMsk9;
     rtk_port_media_t   media_type;
     rtk_port_fiber_ability_t fiberAbility;

     /* Check initialization state */
     RTK_CHK_INIT_STATE();

     /* Check Port Valid */
     RTK_CHK_PORT_IS_UTP(port);

     if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

     if (pAbility->Half_10 >= RTK_ENABLE_END || pAbility->Full_10 >= RTK_ENABLE_END ||
        pAbility->Half_100 >= RTK_ENABLE_END || pAbility->Full_100 >= RTK_ENABLE_END ||
        pAbility->Full_1000 >= RTK_ENABLE_END || pAbility->AutoNegotiation >= RTK_ENABLE_END ||
        pAbility->AsyFC >= RTK_ENABLE_END || pAbility->FC >= RTK_ENABLE_END)
         return RT_ERR_INPUT;

     if (rtk_switch_isComboPort(port) == RT_ERR_OK)
     {
         if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
             return retVal;

         if(media_type == PORT_MEDIA_FIBER)
         {
             memset(&fiberAbility, 0x00, sizeof(rtk_port_fiber_ability_t));
             fiberAbility.AutoNegotiation = pAbility->AutoNegotiation;
             fiberAbility.AsyFC = pAbility->AsyFC;
             fiberAbility.FC = pAbility->FC;
             fiberAbility.Full_1000 = pAbility->Full_1000;
             fiberAbility.Full_100 = pAbility->Full_100;
             return dal_rtl8367d_port_fiberAbility_set(port, &fiberAbility);
         }
     }

     if (1 == pAbility->Full_1000)
         return RT_ERR_INPUT;

     /*for PHY force mode setup*/
     pAbility->AutoNegotiation = 0;

     phyEnMsk0 = 0;
     phyEnMsk4 = 0;
     phyEnMsk9 = 0;

     if (1 == pAbility->Half_10)
     {
         /*10BASE-TX half duplex capable in reg 4.5*/
         phyEnMsk4 = phyEnMsk4 | (1 << 5);

         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 & (~(1 << 13));
     }

     if (1 == pAbility->Full_10)
     {
         /*10BASE-TX full duplex capable in reg 4.6*/
         phyEnMsk4 = phyEnMsk4 | (1 << 6);
         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 & (~(1 << 13));

         /*Full duplex mode in reg 0.8*/
         phyEnMsk0 = phyEnMsk0 | (1 << 8);

     }

     if (1 == pAbility->Half_100)
     {
         /*100BASE-TX half duplex capable in reg 4.7*/
         phyEnMsk4 = phyEnMsk4 | (1 << 7);
         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 | (1 << 13);
     }


     if (1 == pAbility->Full_100)
     {
         /*100BASE-TX full duplex capable in reg 4.8*/
         phyEnMsk4 = phyEnMsk4 | (1 << 8);
         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 | (1 << 13);
         /*Full duplex mode in reg 0.8*/
         phyEnMsk0 = phyEnMsk0 | (1 << 8);
     }

     if (1 == pAbility->AsyFC)
     {
         /*Asymetric flow control in reg 4.11*/
         phyEnMsk4 = phyEnMsk4 | (1 << 11);
     }
     if (1 == pAbility->FC)
     {
         /*Flow control in reg 4.10*/
         phyEnMsk4 = phyEnMsk4 | ((1 << 10));
     }

     /*1000 BASE-T control register setting*/
     if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_1000_BASET_CONTROL_REG, &phyData)) != RT_ERR_OK)
         return retVal;

     phyData = (phyData & (~0x0200)) | phyEnMsk9 ;

     if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_1000_BASET_CONTROL_REG, phyData)) != RT_ERR_OK)
         return retVal;

     /*Auto-Negotiation control register setting*/
     if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_AN_ADVERTISEMENT_REG, &phyData)) != RT_ERR_OK)
         return retVal;

     phyData = (phyData & (~0x0DE0)) | phyEnMsk4;
     if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_AN_ADVERTISEMENT_REG, phyData)) != RT_ERR_OK)
         return retVal;

     /*Control register setting and power off/on*/
     phyData = phyEnMsk0 & (~(1 << 12));
     phyData |= (1 << 11);   /* power down PHY, bit 11 should be set to 1 */
     if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
         return retVal;

     phyData = phyData & (~(1 << 11));   /* power on PHY, bit 11 should be set to 0*/
     if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
         return retVal;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      port - Port id.
 * Output:
 *      pAbility - Ability structure
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get the capablity of specified PHY.
 */
rtk_api_ret_t dal_rtl8367d_port_phyForceModeAbility_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_api_ret_t       retVal;
    rtk_uint32          phyData0;
    rtk_uint32          phyData4;
    rtk_uint32          phyData9;
    rtk_port_media_t    media_type;
    rtk_port_fiber_ability_t fiberAbility;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
     RTK_CHK_PORT_IS_UTP(port);

     if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

     if (rtk_switch_isComboPort(port) == RT_ERR_OK)
     {
         if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
             return retVal;

         if(media_type == PORT_MEDIA_FIBER)
         {
             memset(&fiberAbility, 0x00, sizeof(rtk_port_fiber_ability_t));
             if ((retVal = dal_rtl8367d_port_fiberAbility_get(port,  &fiberAbility)) != RT_ERR_OK)
                 return retVal;

             memset(pAbility, 0x00, sizeof(rtk_port_phy_ability_t));
             pAbility->AutoNegotiation = fiberAbility.AutoNegotiation;
             pAbility->AsyFC = fiberAbility.AsyFC;
             pAbility->FC = fiberAbility.FC;
             pAbility->Full_1000 = fiberAbility.Full_1000;
             pAbility->Full_100 = fiberAbility.Full_100;
             return RT_ERR_OK;
         }
     }

    /*Control register setting and restart auto*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return retVal;

    /*Auto-Negotiation control register setting*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return retVal;

    /*1000 BASE-T control register setting*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return retVal;

    if (phyData9 & (1 << 9))
        pAbility->Full_1000 = 1;
    else
        pAbility->Full_1000 = 0;

    if (phyData4 & (1 << 11))
        pAbility->AsyFC = 1;
    else
        pAbility->AsyFC = 0;

    if (phyData4 & ((1 << 10)))
        pAbility->FC = 1;
    else
        pAbility->FC = 0;


    if (phyData4 & (1 << 8))
        pAbility->Full_100 = 1;
    else
        pAbility->Full_100 = 0;

    if (phyData4 & (1 << 7))
        pAbility->Half_100 = 1;
    else
        pAbility->Half_100 = 0;

    if (phyData4 & (1 << 6))
        pAbility->Full_10 = 1;
    else
        pAbility->Full_10 = 0;

    if (phyData4 & (1 << 5))
        pAbility->Half_10 = 1;
    else
        pAbility->Half_10 = 0;


    if (phyData0 & (1 << 12))
        pAbility->AutoNegotiation = 1;
    else
        pAbility->AutoNegotiation = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyStatus_get
 * Description:
 *      Get ethernet PHY linking status
 * Input:
 *      port - Port id.
 * Output:
 *      linkStatus  - PHY link status
 *      speed       - PHY link speed
 *      duplex      - PHY duplex mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      API will return auto negotiation status of phy.
 */
rtk_api_ret_t dal_rtl8367d_port_phyStatus_get(rtk_port_t port, rtk_port_linkStatus_t *pLinkStatus, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phyData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if( (NULL == pLinkStatus) || (NULL == pSpeed) || (NULL == pDuplex) )
        return RT_ERR_NULL_POINTER;

    /*Get PHY resolved register*/
    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_RESOLVED_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    /*check link status*/
    if (phyData & (1<<2))
    {
        *pLinkStatus = 1;

        /*check link speed*/
        *pSpeed = (phyData&0x0030) >> 4;

        /*check link duplex*/
        *pDuplex = (phyData&0x0008) >> 3;
    }
    else
    {
        *pLinkStatus = 0;
        *pSpeed = 0;
        *pDuplex = 0;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_macForceLink_set
 * Description:
 *      Set port force linking configuration.
 * Input:
 *      port            - port id.
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can set Port/MAC force mode properties.
 */
rtk_api_ret_t dal_rtl8367d_port_macForceLink_set(rtk_port_t port, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtk_uint32 reg_data = 0;
    rtk_uint32 reg_data2;
    rtk_uint32 rtl8367d_speed;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if (pPortability->forcemode >1|| pPortability->speed >= PORT_SPEED_END || pPortability->duplex > 1 ||
       pPortability->link > 1 || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
        return RT_ERR_INPUT;

    switch (pPortability->speed)
    {
        case PORT_SPEED_10M:
            rtl8367d_speed = RTL8367D_EXT_PORT_SPEED_10M;
            break;
        case PORT_SPEED_100M:
            rtl8367d_speed = RTL8367D_EXT_PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000M:
            rtl8367d_speed = RTL8367D_EXT_PORT_SPEED_1000M;
            break;
        case PORT_SPEED_500M:
            rtl8367d_speed = RTL8367D_EXT_PORT_SPEED_500M;
            break;
        case PORT_SPEED_2500M:
            rtl8367d_speed = RTL8367D_EXT_PORT_SPEED_2500M;
            break;
        default:
            return RT_ERR_INPUT;
    }

    reg_data |= ((rtl8367d_speed & 0x0C) >> 2) << 12;
    reg_data |= pPortability->nway << 7;
    reg_data |= pPortability->txpause << 6;
    reg_data |= pPortability->rxpause << 5;
    reg_data |= pPortability->link << 4;
    reg_data |= pPortability->duplex << 2;
    reg_data |= rtl8367d_speed & 0x03;

    if(pPortability->forcemode)
        reg_data2 = 0xFFFF;
    else
        reg_data2 = 0;

    if((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC0_FORCE_SELECT + rtk_switch_port_L2P_get(port), reg_data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC0_FORCE_SELECT_EN + rtk_switch_port_L2P_get(port), reg_data2)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_macForceLink_get
 * Description:
 *      Get port force linking configuration.
 * Input:
 *      port - Port id.
 * Output:
 *      pPortability - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get Port/MAC force mode properties.
 */
rtk_api_ret_t dal_rtl8367d_port_macForceLink_get(rtk_port_t port, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtk_uint32 reg_data;
    rtk_uint32 reg_data2;
    rtk_uint32 rtl8367d_speed;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicReg(RTL8367D_REG_MAC0_FORCE_SELECT + rtk_switch_port_L2P_get(port), &reg_data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicReg(RTL8367D_REG_MAC0_FORCE_SELECT_EN + rtk_switch_port_L2P_get(port), &reg_data2)) != RT_ERR_OK)
        return retVal;

    if ((reg_data == 0x1000) && (reg_data2 == 0x3000) && (rtk_switch_isExtPort(port) == RT_ERR_OK))
    {
        pPortability->forcemode = 0;
        pPortability->nway      = 0;
        pPortability->txpause   = 0;
        pPortability->rxpause   = 0;
        pPortability->link      = 0;
        pPortability->duplex    = 0;
        return RT_ERR_OK;
    }

    pPortability->forcemode = (reg_data2 == 0) ? 0 : 1;
    pPortability->nway      = (reg_data >> 7) & 0x0001;
    pPortability->txpause   = (reg_data >> 6) & 0x0001;
    pPortability->rxpause   = (reg_data >> 5) & 0x0001;
    pPortability->link      = (reg_data >> 4) & 0x0001;
    pPortability->duplex    = (reg_data >> 2) & 0x0001;

    rtl8367d_speed          = (reg_data & 0x0003) | (((reg_data & 0x3000) >> 12) << 2);
    switch (rtl8367d_speed)
    {
        case RTL8367D_EXT_PORT_SPEED_10M:
            pPortability->speed = PORT_SPEED_10M;
            break;
        case RTL8367D_EXT_PORT_SPEED_100M:
            pPortability->speed = PORT_SPEED_100M;
            break;
        case RTL8367D_EXT_PORT_SPEED_1000M:
            pPortability->speed = PORT_SPEED_1000M;
            break;
        case RTL8367D_EXT_PORT_SPEED_500M:
            pPortability->speed = PORT_SPEED_500M;
            break;
        case RTL8367D_EXT_PORT_SPEED_2500M:
            pPortability->speed = PORT_SPEED_2500M;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_macForceLinkExt_set
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      port            - external port ID
 *      mode            - external interface mode
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface force mode properties.
 *      The external interface can be set to:
 *      - MODE_EXT_DISABLE,
 *      - MODE_EXT_RGMII,
 *      - MODE_EXT_MII_MAC,
 *      - MODE_EXT_MII_PHY,
 *      - MODE_EXT_TMII_MAC,
 *      - MODE_EXT_TMII_PHY,
 *      - MODE_EXT_GMII,
 *      - MODE_EXT_RMII_MAC,
 *      - MODE_EXT_RMII_PHY,
 *      - MODE_EXT_SGMII,
 *      - MODE_EXT_HSGMII,
 */
rtk_api_ret_t dal_rtl8367d_port_macForceLinkExt_set(rtk_port_t port, rtk_mode_ext_t mode, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtk_port_mac_ability_t ability;
    rtk_uint32 ext_id;
    rtk_port_media_t media_type;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    if (port == UTP_PORT4)
    {
        if (mode != MODE_EXT_RGMII)
            return RT_ERR_INPUT;

        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &media_type)) != RT_ERR_OK)
            return retVal;

        if (media_type != PORT_MEDIA_RGMII)
            return RT_ERR_PORT_ID;
    }
    else
    {
        RTK_CHK_PORT_IS_EXT(port);

        if ((port == EXT_PORT1) && (mode == MODE_EXT_RGMII))
        {
            if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(UTP_PORT4, &media_type)) != RT_ERR_OK)
                return retVal;

            if (media_type == PORT_MEDIA_RGMII)
                return RT_ERR_PORT_ID;
        }
    }

    if (NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if ((mode > MODE_EXT_100FX) && (mode != MODE_EXT_FIBER_2P5G))
        return RT_ERR_INPUT;

    if (mode == MODE_EXT_GMII)
        return RT_ERR_INPUT;

    if(mode == MODE_EXT_HSGMII)
    {
        if (pPortability->forcemode > 1 || pPortability->speed != PORT_SPEED_2500M || pPortability->duplex != PORT_FULL_DUPLEX ||
           pPortability->link >= PORT_LINKSTATUS_END || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
            return RT_ERR_INPUT;

        if(rtk_switch_isHsgPort(port) != RT_ERR_OK)
            return RT_ERR_PORT_ID;
    }
    else if (mode == MODE_EXT_RGMII)
    {
        if (pPortability->forcemode > 1 || pPortability->speed > PORT_SPEED_1000M || pPortability->duplex >= PORT_DUPLEX_END ||
           pPortability->link >= PORT_LINKSTATUS_END || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
            return RT_ERR_INPUT;
    }
    else if (mode != MODE_EXT_DISABLE)
    {
        if (pPortability->forcemode > 1 || pPortability->speed > PORT_SPEED_1000M || pPortability->duplex >= PORT_DUPLEX_END ||
           pPortability->link >= PORT_LINKSTATUS_END || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
            return RT_ERR_INPUT;
    }

    if ((port == UTP_PORT4) && (mode == MODE_EXT_RGMII))
        ext_id = 2;
    else
        ext_id = port - 15;

    /* Configure EXT port mode */
    if ((retVal = _dal_rtl8367d_setAsicPortExtMode(ext_id, mode)) != RT_ERR_OK)
        return retVal;

    /* Configure Ability */
    memset(&ability, 0x00, sizeof(rtk_port_mac_ability_t));
    if ((retVal = dal_rtl8367d_port_macForceLink_get(port, &ability)) != RT_ERR_OK)
        return retVal;

    if (pPortability->link == 0)
    {
        if (mode == MODE_EXT_FIBER_2P5G)
        {
            if((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC0_FORCE_SELECT + rtk_switch_port_L2P_get(port), 0x1000)) != RT_ERR_OK)
                return retVal;

            if((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC0_FORCE_SELECT_EN + rtk_switch_port_L2P_get(port), 0x3000)) != RT_ERR_OK)
                return retVal;
        }
        else
        {
            ability.forcemode = pPortability->forcemode;
            ability.duplex    = pPortability->duplex;
            ability.link      = pPortability->link;
            ability.nway      = pPortability->nway;
            ability.txpause   = pPortability->txpause;
            ability.rxpause   = pPortability->rxpause;
            ability.speed     = pPortability->speed;

            if ((retVal = dal_rtl8367d_port_macForceLink_set(port, &ability)) != RT_ERR_OK)
                return retVal;
        }
    }
    else
    {
        ability.forcemode = pPortability->forcemode;
        ability.duplex    = pPortability->duplex;
        ability.link      = pPortability->link;
        ability.nway      = pPortability->nway;
        ability.txpause   = pPortability->txpause;
        ability.rxpause   = pPortability->rxpause;
        ability.speed     = pPortability->speed;

        if ((retVal = dal_rtl8367d_port_macForceLink_set(port, &ability)) != RT_ERR_OK)
            return retVal;
    }


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_macForceLinkExt_get
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      port            - external port ID
 * Output:
 *      pMode           - external interface mode
 *      pPortability    - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get external interface force mode properties.
 */
rtk_api_ret_t dal_rtl8367d_port_macForceLinkExt_get(rtk_port_t port, rtk_mode_ext_t *pMode, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtk_port_mac_ability_t ability;
    rtk_uint32 ext_id;
    rtk_port_media_t media_type;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    if (port == UTP_PORT4)
        ext_id = 2;
    else
    {
        RTK_CHK_PORT_IS_EXT(port);
        ext_id = port - 15;
    }

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _dal_rtl8367d_getAsicPortExtMode(ext_id, (rtk_uint32 *)pMode)) != RT_ERR_OK)
        return retVal;

    if ( (ext_id == 1) && ((*pMode == MODE_EXT_1000X_100FX) || (*pMode == MODE_EXT_1000X) || (*pMode == MODE_EXT_100FX)) )
    {
        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(UTP_PORT4, &media_type)) != RT_ERR_OK)
            return retVal;

        if (media_type == PORT_MEDIA_FIBER)
        {
            *pMode = MODE_EXT_DISABLE;
        }
    }

    if ((ext_id == 2) && (*pMode == MODE_EXT_RGMII))
    {
        if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(UTP_PORT4, &media_type)) != RT_ERR_OK)
            return retVal;

        if (port == UTP_PORT4)
        {
            if (media_type != PORT_MEDIA_RGMII)
                *pMode = MODE_EXT_DISABLE;
        }
        else
        {
            if (media_type == PORT_MEDIA_RGMII)
                *pMode = MODE_EXT_DISABLE;
        }
    }

    memset(&ability, 0x00, sizeof(rtk_port_mac_ability_t));
    if ((retVal = dal_rtl8367d_port_macForceLink_get(port, &ability)) != RT_ERR_OK)
        return retVal;

    pPortability->forcemode = ability.forcemode;
    pPortability->duplex    = ability.duplex;
    pPortability->link      = ability.link;
    pPortability->nway      = ability.nway;
    pPortability->txpause   = ability.txpause;
    pPortability->rxpause   = ability.rxpause;
    pPortability->speed     = ability.speed;

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8367d_port_macStatus_get
 * Description:
 *      Get port link status.
 * Input:
 *      port - Port id.
 * Output:
 *      pPortstatus - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get Port/PHY properties.
 */
rtk_api_ret_t dal_rtl8367d_port_macStatus_get(rtk_port_t port, rtk_port_mac_ability_t *pPortstatus)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 rtl8367d_speed;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pPortstatus)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_PORT0_STATUS+rtk_switch_port_L2P_get(port), &regData)) != RT_ERR_OK)
        return retVal;

    memset(pPortstatus, 0x00, sizeof(rtk_port_mac_ability_t));
    pPortstatus->link = (regData >> 4) & 0x0001;

    if (pPortstatus->link == 1)
    {
        pPortstatus->duplex    = (regData >> 2) & 0x0001;
        pPortstatus->nway      = (regData >> 7) & 0x0001;
        pPortstatus->txpause   = (regData >> 6) & 0x0001;
        pPortstatus->rxpause   = (regData >> 5) & 0x0001;

        rtl8367d_speed = (regData & 0x0003) | (((regData & 0x3000) >> 12) << 2);
        switch (rtl8367d_speed)
        {
            case RTL8367D_EXT_PORT_SPEED_10M:
                pPortstatus->speed = PORT_SPEED_10M;
                break;
            case RTL8367D_EXT_PORT_SPEED_100M:
                pPortstatus->speed = PORT_SPEED_100M;
                break;
            case RTL8367D_EXT_PORT_SPEED_1000M:
                pPortstatus->speed = PORT_SPEED_1000M;
                break;
            case RTL8367D_EXT_PORT_SPEED_500M:
                pPortstatus->speed = PORT_SPEED_500M;
                break;
            case RTL8367D_EXT_PORT_SPEED_2500M:
                pPortstatus->speed = PORT_SPEED_2500M;
                break;
            default:
                return RT_ERR_INPUT;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_macLocalLoopbackEnable_set
 * Description:
 *      Set Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      port    - Port id.
 *      enable  - Loopback state, 0:disable, 1:enable
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can enable/disable Local loopback in MAC.
 *      For UTP port, This API will also enable the digital
 *      loopback bit in PHY register for sync of speed between
 *      PHY and MAC. For EXT port, users need to force the
 *      link state by themself.
 */
rtk_api_ret_t dal_rtl8367d_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t   retVal;
    rtk_uint32      data;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_PORT0_MISC_CFG + (rtk_switch_port_L2P_get(port) << 5), RTL8367D_PORT0_MISC_CFG_MAC_LOOPBACK_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
        return retVal;

    if(rtk_switch_isUtpPort(port) == RT_ERR_OK)
    {
        if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, &data)) != RT_ERR_OK)
            return retVal;

        if(enable == ENABLED)
            data |= (0x0001 << 14);
        else
            data &= ~(0x0001 << 14);

        if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, data)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_macLocalLoopbackEnable_get
 * Description:
 *      Get Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      port    - Port id.
 * Output:
 *      pEnable  - Loopback state, 0:disable, 1:enable
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_PORT0_MISC_CFG + (rtk_switch_port_L2P_get(port) << 5), RTL8367D_PORT0_MISC_CFG_MAC_LOOPBACK_OFFSET, &regData)) != RT_ERR_OK)
        return retVal;

    *pEnable = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port.
 * Input:
 *      port    - port id.
 *      reg     - Register id
 *      regData - Register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can set PHY register data of the specific port.
 */
rtk_api_ret_t dal_rtl8367d_port_phyReg_set(rtk_port_t port, rtk_port_phy_reg_t reg, rtk_port_phy_data_t regData)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), reg, regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port.
 * Input:
 *      port    - Port id.
 *      reg     - Register id
 * Output:
 *      pData   - Register data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can get PHY register data of the specific port.
 */
rtk_api_ret_t dal_rtl8367d_port_phyReg_get(rtk_port_t port, rtk_port_phy_reg_t reg, rtk_port_phy_data_t *pData)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), reg, pData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_port_phyOCPReg_set
 * Description:
 *      Set PHY OCP register
 * Input:
 *      port        - PHY ID
 *      ocpAddr     - OCP register address
 *      ocpData     - OCP Data.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_BUSYWAIT_TIMEOUT                 - Timeout
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_port_phyOCPReg_set(rtk_port_t port, rtk_uint32 ocpAddr, rtk_uint32 ocpData )
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if ((retVal = _dal_rtl8367d_setAsicPHYOCPReg(rtk_switch_port_L2P_get(port), ocpAddr, ocpData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyOCPReg_get
 * Description:
 *      Set PHY OCP register
 * Input:
 *      phyNo       - PHY ID
 *      ocpAddr     - OCP register address
 * Output:
 *      pRegData    - OCP data.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Null pointer
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_port_phyOCPReg_get(rtk_port_t port, rtk_uint32 ocpAddr, rtk_uint32 *pRegData )
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if (pRegData == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _dal_rtl8367d_getAsicPHYOCPReg(rtk_switch_port_L2P_get(port), ocpAddr, pRegData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port.
 * Input:
 *      port    - port id.
 *      enable  - Back pressure status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set the half duplex backpressure enable status of the specific port.
 *      The half duplex backpressure enable status of the port is as following:
 *      - DISABLE(Defer)
 *      - ENABLE (Backpressure)
 */
rtk_api_ret_t dal_rtl8367d_port_backpressureEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (port != RTK_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CFG_BACKPRESSURE, RTL8367D_LONGTXE_OFFSET, (enable == ENABLED) ? 0 : 1)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port.
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get the half duplex backpressure enable status of the specific port.
 *      The half duplex backpressure enable status of the port is as following:
 *      - DISABLE(Defer)
 *      - ENABLE (Backpressure)
 */
rtk_api_ret_t dal_rtl8367d_port_backpressureEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (port != RTK_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_CFG_BACKPRESSURE, RTL8367D_LONGTXE_OFFSET, &regData)) != RT_ERR_OK)
        return retVal;

    *pEnable = (regData == 1) ? DISABLED : ENABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port.
 * Input:
 *      port    - port id.
 *      enable  - Back pressure status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set port admin configuration of the specific port.
 *      The port admin configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtk_api_ret_t dal_rtl8367d_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32      data;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = _dal_rtl8367d_getAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, &data)) != RT_ERR_OK)
        return retVal;

    if (ENABLED == enable)
    {
        data &= 0xF7FF;
        data |= 0x0200;
    }
    else if (DISABLED == enable)
    {
        data |= 0x0800;
    }

    if ((retVal = _dal_rtl8367d_setAsicPHYReg(rtk_switch_port_L2P_get(port), PHY_CONTROL_REG, data)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_adminEnable_get
 * Description:
 *      Get port admin configurationof the specific port.
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get port admin configuration of the specific port.
 *      The port admin configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtk_api_ret_t dal_rtl8367d_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32      data;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _dal_rtl8367d_getAsicPHYReg(port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
        return retVal;

    if ( (data & 0x0800) == 0x0800)
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable = ENABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_isolation_set
 * Description:
 *      Set permitted port isolation portmask
 * Input:
 *      port         - port id.
 *      pPortmask    - Permit port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      This API set the port mask that a port can trasmit packet to of each port
 *      A port can only transmit packet to ports included in permitted portmask
 */
rtk_api_ret_t dal_rtl8367d_port_isolation_set(rtk_port_t port, rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    /* check port mask */
    RTK_CHK_PORTMASK_VALID(pPortmask);

    if ((retVal = rtk_switch_portmask_L2P_get(pPortmask, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_PORT_ISOLATION_PORT0_MASK + rtk_switch_port_L2P_get(port), pmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_isolation_get
 * Description:
 *      Get permitted port isolation portmask
 * Input:
 *      port - Port id.
 * Output:
 *      pPortmask - Permit port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API get the port mask that a port can trasmit packet to of each port
 *      A port can only transmit packet to ports included in permitted portmask
 */
rtk_api_ret_t dal_rtl8367d_port_isolation_get(rtk_port_t port, rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_PORT_ISOLATION_PORT0_MASK + rtk_switch_port_L2P_get(port), &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtk_switch_portmask_P2L_get(pmask, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_rgmiiDelayExt_set
 * Description:
 *      Set RGMII interface delay value for TX and RX.
 * Input:
 *      txDelay - TX delay value, 1 for delay 2ns and 0 for no-delay
 *      rxDelay - RX delay value, 0~7 for delay setup.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface 2 RGMII delay.
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.
 *      Note. This API should be called before rtk_port_macForceLinkExt_set().
 */
rtk_api_ret_t dal_rtl8367d_port_rgmiiDelayExt_set(rtk_port_t port, rtk_data_t txDelay, rtk_data_t rxDelay)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    if ((port != EXT_PORT1) && (port != UTP_PORT4))
        return RT_ERR_PORT_ID;

    if ((txDelay > 1) || (rxDelay > 7))
        return RT_ERR_INPUT;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_EXT1_RGMXF, &regData)) != RT_ERR_OK)
        return retVal;

    regData = (regData & 0xFFF0) | ((txDelay << 3) & 0x0008) | (rxDelay & 0x0007);

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_EXT1_RGMXF, regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_rgmiiDelayExt_get
 * Description:
 *      Get RGMII interface delay value for TX and RX.
 * Input:
 *      None
 * Output:
 *      pTxDelay - TX delay value
 *      pRxDelay - RX delay value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface 2 RGMII delay.
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for n0-delay, and 7 for maximum delay.
 */
rtk_api_ret_t dal_rtl8367d_port_rgmiiDelayExt_get(rtk_port_t port, rtk_data_t *pTxDelay, rtk_data_t *pRxDelay)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    if ((port != EXT_PORT1) && (port != UTP_PORT4))
        return RT_ERR_PORT_ID;

    if( (NULL == pTxDelay) || (NULL == pRxDelay) )
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_EXT1_RGMXF, &regData)) != RT_ERR_OK)
        return retVal;

    *pTxDelay = (regData & 0x0008) >> 3;
    *pRxDelay = regData & 0x0007;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyEnableAll_set
 * Description:
 *      Set all PHY enable status.
 * Input:
 *      enable - PHY Enable State.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set all PHY status.
 *      The configuration of all PHY is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtk_api_ret_t dal_rtl8367d_port_phyEnableAll_set(rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 data;
    rtk_uint32 port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    RTK_SCAN_ALL_LOG_PORT(port)
    {
        if(rtk_switch_isUtpPort(port) == RT_ERR_OK)
        {
            if ((retVal = _dal_rtl8367d_getAsicPHYReg(port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
            {
                return retVal;
            }
            if (ENABLED == enable)
            {
                data &= 0xF7FF;
                data |= 0x0200;
            }
            else
            {
                data |= 0x0800;
            }

            if ((retVal = _dal_rtl8367d_setAsicPHYReg(port, PHY_CONTROL_REG, data)) != RT_ERR_OK)
            {
                return retVal;
            }
        }
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8367d_port_phyEnableAll_get
 * Description:
 *      Get all PHY enable status.
 * Input:
 *      None
 * Output:
 *      pEnable - PHY Enable State.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API can set all PHY status.
 *      The configuration of all PHY is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtk_api_ret_t dal_rtl8367d_port_phyEnableAll_get(rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 data;
    rtk_uint32 port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    RTK_SCAN_ALL_LOG_PORT(port)
    {
        if(rtk_switch_isUtpPort(port) == RT_ERR_OK)
        {
            if ((retVal = _dal_rtl8367d_getAsicPHYReg(port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
                return retVal;

            if (data & 0x0800)
            {
                *pEnable = DISABLED;
                return RT_ERR_OK;
            }
        }
    }

    *pEnable = ENABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyComboPortMedia_set
 * Description:
 *      Set Combo port media type
 * Input:
 *      port    - Port id.
 *      media   - Media (COPPER or FIBER or AUTO)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
rtk_api_ret_t dal_rtl8367d_port_phyComboPortMedia_set(rtk_port_t port, rtk_port_media_t media)
{
    rtk_api_ret_t retVal;
    rtk_uint32 i;
    rtk_uint32 mux;
    rtk_port_media_t currentMedia;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    /* Check Combo Port ID */
    RTK_CHK_PORT_IS_COMBO(port);

    if ((retVal = dal_rtl8367d_port_phyComboPortMedia_get(port, &currentMedia)) != RT_ERR_OK)
        return retVal;

    if (media == currentMedia)
        return RT_ERR_OK;

    /* if SDS0 used by EXT_PORT0, can't configure combo = Fiber*/
    if (media == PORT_MEDIA_FIBER)
    {
        if( (retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, &regData)) != RT_ERR_OK)
            return retVal;

        if (regData != RTL8367D_PORT_SDS_MODE_DISABLE)
            return RT_ERR_INPUT;
    }

    /* Different media type, always set to copper then set to new media type */
    if (currentMedia == PORT_MEDIA_FIBER)
    {
        if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 0)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 0)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 0)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_DISABLE)) != RT_ERR_OK)
           return retVal;

        /* Clear Fiber mux setting */
        if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_UTP_FIB_DET, RTL8367D_FORCE_SEL_FIBER_MASK, 0)) != RT_ERR_OK)
            return retVal;

        /* Clear Force Link down MAC6 setting */
        if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC6_FORCE_SELECT, 0x0000)) != RT_ERR_OK)
           return retVal;

        if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC6_FORCE_SELECT_EN, 0x0000)) != RT_ERR_OK)
           return retVal;
    }
    else if (currentMedia == PORT_MEDIA_RGMII)
    {
        /* Disable RGMII */
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_DIGITAL_INTERFACE_SELECT, RTL8367D_SELECT_GMII_1_MASK, 0)) != RT_ERR_OK)
            return retVal;

        /* Clear MAC4 force ability */
        if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC4_FORCE_SELECT, 0)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC4_FORCE_SELECT_EN, 0)) != RT_ERR_OK)
            return retVal;

        /* Clear RGMII/MII mux setting */
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC4_SEL_EXT1_OFFSET, 0)) != RT_ERR_OK)
            return retVal;
    }

    /* Configure new medida type */
    if(media == PORT_MEDIA_FIBER)
    {
        for(i = 0; i < sizeof(SDSNctrlPatch) / (sizeof(rtk_uint32) * 2); i++)
        {
            if( (retVal = rtl8367d_setAsicReg(SDSNctrlPatch[i][0], SDSNctrlPatch[i][1])) != RT_ERR_OK)
                return retVal;
        }

        for(i = 0; i < sizeof(Fiber1000M100MCfg) / (sizeof(rtk_uint32) * 2); i++)
        {
            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_DATA, Fiber1000M100MCfg[i][1])) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_ADR, Fiber1000M100MCfg[i][0])) != RT_ERR_OK)
                return retVal;

            if( (retVal = rtl8367d_setAsicReg(RTL8367D_REG_SDS_INDACS_CMD, 0x00CD)) != RT_ERR_OK)
                return retVal;
        }

        if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_FIBER_CFG_2, RTL8367D_SDS_RX_DISABLE_MASK, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_UTP_FIB_DET, RTL8367D_FORCE_SEL_FIBER_MASK, 0x3)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA12PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_PA33PC_EN_S0_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SDS_MISC, RTL8367D_MAC6_SEL_SDS0_OFFSET, 0)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_SDS_MISC, RTL8367D_CFG_SDS_MODE_MASK, RTL8367D_PORT_SDS_MODE_1000X_100FX)) != RT_ERR_OK)
           return retVal;

        if( (retVal = rtl8367d_setAsicReg(0x612F, 0x000C)) != RT_ERR_OK)
           return retVal;

        /* Force Link down MAC6 */
        if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC6_FORCE_SELECT, 0x0000)) != RT_ERR_OK)
           return retVal;

        if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MAC6_FORCE_SELECT_EN, 0xFFFF)) != RT_ERR_OK)
           return retVal;

    }
    else if(media == PORT_MEDIA_RGMII)
    {
        /* Make sure RGMII/MII mux is not at port 7 */
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC7_SEL_EXT1_OFFSET, &mux)) != RT_ERR_OK)
            return retVal;

        if (mux == 1)
            return RT_ERR_INPUT;

        /* Configure RGMII DP, DN, E2, MODE */
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_SEL33_EXT1_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_DRI_EXT1_RG_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_DRI_EXT1_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_CHIP_DEBUG0, RTL8367D_SLR_EXT1_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_CHIP_DEBUG1, RTL8367D_RG1_DN_MASK, 7)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_CHIP_DEBUG1, RTL8367D_RG1_DP_MASK, 5)) != RT_ERR_OK)
            return retVal;

        /* Configure RGMII/MII mux to port 4 */
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC4_SEL_EXT1_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_DIGITAL_INTERFACE_SELECT, RTL8367D_SELECT_GMII_1_MASK, 1)) != RT_ERR_OK)
            return retVal;
    }
    else if (media == PORT_MEDIA_COPPER)
    {
        /* Do nothing */
    }
    else
        return RT_ERR_INPUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyComboPortMedia_get
 * Description:
 *      Get Combo port media type
 * Input:
 *      port    - Port id.
 * Output:
 *      pMedia  - Media (COPPER or FIBER or AUTO)
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
rtk_api_ret_t dal_rtl8367d_port_phyComboPortMedia_get(rtk_port_t port, rtk_port_media_t *pMedia)
{
    rtk_api_ret_t   retVal;
    rtk_uint32      data;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    /* Check Combo Port ID */
    RTK_CHK_PORT_IS_COMBO(port);

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_TOP_CON0, RTL8367D_MAC4_SEL_EXT1_OFFSET, &data)) != RT_ERR_OK)
        return retVal;

    if (data == 1)
        *pMedia = PORT_MEDIA_RGMII;
    else
    {
        if( (retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_UTP_FIB_DET, RTL8367D_FORCE_SEL_FIBER_MASK, &data)) != RT_ERR_OK)
            return retVal;

        if (data == 3)
            *pMedia = PORT_MEDIA_FIBER;
        else
            *pMedia = PORT_MEDIA_COPPER;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_rtctEnable_set
 * Description:
 *      Enable RTCT test
 * Input:
 *      pPortmask    - Port mask of RTCT enabled port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_MASK        - Invalid port mask.
 * Note:
 *      The API can enable RTCT Test
 */
rtk_api_ret_t dal_rtl8367d_port_rtctEnable_set(rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_port_t port;
    rtk_uint32 regData;
    rtk_uint32 i;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Mask Valid */
    RTK_CHK_PORTMASK_VALID_ONLY_UTP(pPortmask);

    RTK_PORTMASK_SCAN((*pPortmask), port)
    {
        /* Initialize RTCT */
        for(i = 0; i < sizeof(RTCTPatch) / (sizeof(rtk_uint32) * 2); i++)
        {
            if( (retVal = dal_rtl8367d_port_phyOCPReg_set(port, RTCTPatch[i][0], RTCTPatch[i][1])) != RT_ERR_OK)
                return retVal;
        }

        regData = 0x00F2; /*RTCT set to echo response mode*/
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa422, regData)) != RT_ERR_OK)
            return retVal;

        regData = 0x00F3;
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa422, regData)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_rtctDisable_set
 * Description:
 *      Disable RTCT test
 * Input:
 *      pPortmask    - Port mask of RTCT disabled port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_MASK        - Invalid port mask.
 * Note:
 *      The API can disable RTCT Test
 */
rtk_api_ret_t dal_rtl8367d_port_rtctDisable_set(rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_port_t port;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Mask Valid */
    RTK_CHK_PORTMASK_VALID_ONLY_UTP(pPortmask);

    RTK_PORTMASK_SCAN((*pPortmask), port)
    {
        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa422, &regData)) != RT_ERR_OK)
             return retVal;

         regData &= 0x7FFF;
         if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa422, regData)) != RT_ERR_OK)
             return retVal;

         regData |= 0x00F0;
         if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa422, regData)) != RT_ERR_OK)
             return retVal;

         regData &= ~0x0001;
         if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa422, regData)) != RT_ERR_OK)
             return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_rtctResult_get
 * Description:
 *      Get the result of RTCT test
 * Input:
 *      port        - Port ID
 * Output:
 *      pRtctResult - The result of RTCT result
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 *      RT_ERR_PHY_RTCT_NOT_FINISH  - Testing does not finish.
 * Note:
 *      The API can get RTCT test result.
 *      RTCT test may takes 4.8 seconds to finish its test at most.
 *      Thus, if this API return RT_ERR_PHY_RTCT_NOT_FINISH or
 *      other error code, the result can not be referenced and
 *      user should call this API again until this API returns
 *      a RT_ERR_OK.
 *      The result is stored at pRtctResult->ge_result
 *      pRtctResult->linkType is unused.
 *      The unit of channel length is 2.5cm. Ex. 300 means 300 * 2.5 = 750cm = 7.5M
 */
rtk_api_ret_t dal_rtl8367d_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData, finish = 1;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa422, &regData)) != RT_ERR_OK)
        return retVal;

    if((regData & 0x8000) == 0x8000)
    {
        /* Channel A */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x802b)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelAOpen       = (regData == 0x0048) ? 1 : 0;
        pRtctResult->result.ge_result.channelAShort      = (regData == 0x0050) ? 1 : 0;
        pRtctResult->result.ge_result.channelAMismatch   = ((regData == 0x0042) || (regData == 0x0044)) ? 1 : 0;
        pRtctResult->result.ge_result.channelALinedriver = (regData == 0x0041) ? 1 : 0;

        /* Channel B */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x802f)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelBOpen       = (regData == 0x0048) ? 1 : 0;
        pRtctResult->result.ge_result.channelBShort      = (regData == 0x0050) ? 1 : 0;
        pRtctResult->result.ge_result.channelBMismatch   = ((regData == 0x0042) || (regData == 0x0044)) ? 1 : 0;
        pRtctResult->result.ge_result.channelBLinedriver = (regData == 0x0041) ? 1 : 0;

        /* Channel C */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x8033)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelCOpen       = (regData == 0x0048) ? 1 : 0;
        pRtctResult->result.ge_result.channelCShort      = (regData == 0x0050) ? 1 : 0;
        pRtctResult->result.ge_result.channelCMismatch   = ((regData == 0x0042) || (regData == 0x0044)) ? 1 : 0;
        pRtctResult->result.ge_result.channelCLinedriver = (regData == 0x0041) ? 1 : 0;

        /* Channel D */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x8037)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelDOpen       = (regData == 0x0048) ? 1 : 0;
        pRtctResult->result.ge_result.channelDShort      = (regData == 0x0050) ? 1 : 0;
        pRtctResult->result.ge_result.channelDMismatch   = ((regData == 0x0042) || (regData == 0x0044)) ? 1 : 0;
        pRtctResult->result.ge_result.channelDLinedriver = (regData == 0x0041) ? 1 : 0;

        /* Channel A Length */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x802d)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

       pRtctResult->result.ge_result.channelALen = (regData / 2);

        /* Channel B Length */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x8031)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelBLen = (regData / 2);

        /* Channel C Length */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x8035)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelCLen = (regData / 2);

        /* Channel D Length */
        if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa436, 0x8039)) != RT_ERR_OK)
            return retVal;

        if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa438, &regData)) != RT_ERR_OK)
            return retVal;

        pRtctResult->result.ge_result.channelDLen = (regData / 2);
    }
    else
        finish = 0;

    if(finish == 0)
        return RT_ERR_PHY_RTCT_NOT_FINISH;
    else
        return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_sgmiiLinkStatus_get
 * Description:
 *      Get SGMII status
 * Input:
 *      port        - Port ID
 * Output:
 *      pSignalDetect   - Signal detect
 *      pSync           - Sync
 *      pLink           - Link
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can reset Serdes
 */
rtk_api_ret_t dal_rtl8367d_port_sgmiiLinkStatus_get(rtk_port_t port, rtk_data_t *pSignalDetect, rtk_data_t *pSync, rtk_port_linkStatus_t *pLink)
{
    rtk_api_ret_t retVal;
    rtk_uint32 nCtrlValue;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicReg(0x612f, &nCtrlValue)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, 0)) != RT_ERR_OK)
            return retVal;
    }

    if ((retVal = _dal_rtl8367d_port_sgmiiLinkStatus_get(port, pSignalDetect, pSync, pLink)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, nCtrlValue)) != RT_ERR_OK)
            return retVal;
    }

    return retVal;
}

/* Function Name:
 *      dal_rtl8367d_port_sgmiiNway_set
 * Description:
 *      Configure SGMII/HSGMII port Nway state
 * Input:
 *      port        - Port ID
 *      state       - Nway state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure SGMII/HSGMII port Nway state
 */
rtk_api_ret_t dal_rtl8367d_port_sgmiiNway_set(rtk_port_t port, rtk_enable_t state)
{
    rtk_api_ret_t retVal;
    rtk_uint32 nCtrlValue;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicReg(0x612f, &nCtrlValue)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, 0)) != RT_ERR_OK)
            return retVal;
    }

    if ((retVal = _dal_rtl8367d_port_sgmiiNway_set(port, state)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, nCtrlValue)) != RT_ERR_OK)
            return retVal;
    }

    return retVal;
}

/* Function Name:
 *      dal_rtl8367d_port_sgmiiNway_get
 * Description:
 *      Get SGMII/HSGMII port Nway state
 * Input:
 *      port        - Port ID
 * Output:
 *      pState      - Nway state
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get SGMII/HSGMII port Nway state
 */
rtk_api_ret_t dal_rtl8367d_port_sgmiiNway_get(rtk_port_t port, rtk_enable_t *pState)
{
    rtk_api_ret_t retVal;
    rtk_uint32 nCtrlValue;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicReg(0x612f, &nCtrlValue)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, 0)) != RT_ERR_OK)
            return retVal;
    }

    if ((retVal = _dal_rtl8367d_port_sgmiiNway_get(port, pState)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, nCtrlValue)) != RT_ERR_OK)
            return retVal;
    }

    return retVal;
}

/* Function Name:
 *      dal_rtl8367d_port_autoDos_set
 * Description:
 *      Set Auto Dos state
 * Input:
 *      type        - Auto DoS type
 *      state       - 1: Eanble(Drop), 0: Disable(Forward)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can set Auto Dos state
 */
rtk_api_ret_t dal_rtl8367d_port_autoDos_set(rtk_port_autoDosType_t type, rtk_enable_t state)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= AUTODOS_END)
        return RT_ERR_INPUT;

    if (state >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_DOS_CFG, RTL8367D_DROP_DAEQSA_OFFSET + type, (state == ENABLED) ? 1 : 0)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_autoDos_get
 * Description:
 *      Get Auto Dos state
 * Input:
 *      type        - Auto DoS type
 * Output:
 *      pState      - 1: Eanble(Drop), 0: Disable(Forward)
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_NULL_POINTER         - Null Pointer
 * Note:
 *      The API can get Auto Dos state
 */
rtk_api_ret_t dal_rtl8367d_port_autoDos_get(rtk_port_autoDosType_t type, rtk_enable_t *pState)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= AUTODOS_END)
        return RT_ERR_INPUT;

    if (pState == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_DOS_CFG, RTL8367D_DROP_DAEQSA_OFFSET + type, &regData)) != RT_ERR_OK)
        return retVal;

    *pState = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_port_fiberAbility_set
 * Description:
 *      Configure fiber port ability
 * Input:
 *      port        - Port ID
 *      pAbility    - Fiber port ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can configure fiber port ability
 */
rtk_api_ret_t dal_rtl8367d_port_fiberAbility_set(rtk_port_t port, rtk_port_fiber_ability_t *pAbility)
{
    rtk_api_ret_t retVal;
    rtk_uint32 nCtrlValue;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicReg(0x612f, &nCtrlValue)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, 0)) != RT_ERR_OK)
            return retVal;
    }

    if ((retVal = _dal_rtl8367d_port_fiberAbility_set(port, pAbility)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, nCtrlValue)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_port_fiberAbility_get
 * Description:
 *      Get fiber port ability
 * Input:
 *      port        - Port ID
 * Output:
 *      pAbility    - Fiber port ability
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get fiber port ability
 */
rtk_api_ret_t dal_rtl8367d_port_fiberAbility_get(rtk_port_t port, rtk_port_fiber_ability_t *pAbility)
{
    rtk_api_ret_t retVal;
    rtk_uint32 nCtrlValue;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicReg(0x612f, &nCtrlValue)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, 0)) != RT_ERR_OK)
            return retVal;
    }

    if ((retVal = _dal_rtl8367d_port_fiberAbility_get(port, pAbility)) != RT_ERR_OK)
        return retVal;

    if (nCtrlValue != 0x0000)
    {
        if ((retVal = rtl8367d_setAsicReg(0x612f, nCtrlValue)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyMdx_set
 * Description:
 *      Set PHY MDI/MDIX state
 * Input:
 *      port        - port ID
 *      mode        - PHY MDI/MDIX mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can set PHY MDI/MDIX state
 */
rtk_api_ret_t dal_rtl8367d_port_phyMdx_set(rtk_port_t port, rtk_port_phy_mdix_mode_t mode)
{
    rtk_uint32 regData;
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_IS_UTP(port);

    if ((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa430, &regData))!=RT_ERR_OK)
        return retVal;

    switch (mode)
    {
        case PHY_AUTO_CROSSOVER_MODE:
            regData &= ~(0x0001 << 9);
            break;
        case PHY_FORCE_MDI_MODE:
            regData |= (0x0001 << 9);
            regData |= (0x0001 << 8);
            break;
        case PHY_FORCE_MDIX_MODE:
            regData |= (0x0001 << 9);
            regData &= ~(0x0001 << 8);
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    if ((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xa430, regData))!=RT_ERR_OK)
        return retVal;

    /* Restart N-way */
    if ((retVal = dal_rtl8367d_port_phyReg_get(port, 0, &regData))!=RT_ERR_OK)
        return retVal;

    regData |= (0x0001 << 9);

    if ((retVal = dal_rtl8367d_port_phyReg_set(port, 0, regData))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyMdx_get
 * Description:
 *      Get PHY MDI/MDIX state
 * Input:
 *      port        - port ID
 * Output:
 *      pMode       - PHY MDI/MDIX mode
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can get PHY MDI/MDIX state
 */
rtk_api_ret_t dal_rtl8367d_port_phyMdx_get(rtk_port_t port, rtk_port_phy_mdix_mode_t *pMode)
{
    rtk_uint32 regData;
    rtk_api_ret_t retVal;

    if ((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa430, &regData))!=RT_ERR_OK)
        return retVal;

    if(regData & (0x0001 << 9))
    {
        if(regData & (0x0001 << 8))
            *pMode = PHY_FORCE_MDI_MODE;
        else
            *pMode = PHY_FORCE_MDIX_MODE;
    }
    else
        *pMode = PHY_AUTO_CROSSOVER_MODE;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyMdxStatus_get
 * Description:
 *      Get PHY MDI/MDIX status
 * Input:
 *      port        - port ID
 * Output:
 *      pStatus     - PHY MDI/MDIX status
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can get PHY MDI/MDIX status
 */
rtk_api_ret_t dal_rtl8367d_port_phyMdxStatus_get(rtk_port_t port, rtk_port_phy_mdix_status_t *pStatus)
{
    rtk_uint32 regData;
    rtk_api_ret_t retVal;

    if ((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa430, &regData))!=RT_ERR_OK)
        return retVal;

    if (regData & (0x0001 << 9))
    {
        if (regData & (0x0001 << 8))
            *pStatus = PHY_STATUS_FORCE_MDI_MODE;
        else
            *pStatus = PHY_STATUS_FORCE_MDIX_MODE;
    }
    else
    {
        if ((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xa434, &regData))!=RT_ERR_OK)
            return retVal;

        if (regData & (0x0001 << 1))
            *pStatus = PHY_STATUS_AUTO_MDI_MODE;
        else
            *pStatus = PHY_STATUS_AUTO_MDIX_MODE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyTestMode_set
 * Description:
 *      Set PHY in test mode.
 * Input:
 *      port    - port id.
 *      mode    - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 *      RT_ERR_NOT_ALLOWED      - The Setting is not allowed, caused by set more than 1 port in Test mode.
 * Note:
 *      Set PHY in test mode and only one PHY can be in test mode at the same time.
 *      It means API will return FAILED if other PHY is in test mode.
 *      This API only provide test mode 1 & 4 setup.
 */
rtk_api_ret_t dal_rtl8367d_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t mode)
{
    rtk_uint32          data, i;
    rtk_api_ret_t       retVal;

    RTK_CHK_PORT_IS_UTP(port);

    if(mode >= PHY_TEST_MODE_END)
        return RT_ERR_INPUT;

    if( (mode == PHY_TEST_MODE_2) || (mode == PHY_TEST_MODE_3) )
        return RT_ERR_INPUT;

    if (PHY_TEST_MODE_NORMAL != mode)
    {
        /* Other port should be Normal mode */
        RTK_SCAN_ALL_LOG_PORT(i)
        {
            if(rtk_switch_isUtpPort(i) == RT_ERR_OK)
            {
                if(i != port)
                {
                    if ((retVal = dal_rtl8367d_port_phyReg_get(i, 9, &data)) != RT_ERR_OK)
                        return retVal;

                    if((data & 0xE000) != 0)
                        return RT_ERR_NOT_ALLOWED;
                }
            }
        }
    }


    if ((retVal = dal_rtl8367d_port_phyReg_get(port, 9, &data)) != RT_ERR_OK)
        return retVal;

    data &= ~0xE000;
    data |= (mode << 13);
    if ((retVal = dal_rtl8367d_port_phyReg_set(port, 9, data)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      port - Port id.
 * Output:
 *      mode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
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
rtk_api_ret_t dal_rtl8367d_port_phyTestMode_get(rtk_port_t port, rtk_port_phy_test_mode_t *pMode)
{
    rtk_uint32      data;
    rtk_api_ret_t   retVal;

    RTK_CHK_PORT_IS_UTP(port);

    if (pMode == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8367d_port_phyReg_get(port, 9, &data)) != RT_ERR_OK)
        return retVal;

    *pMode = (data & 0xE000) >> 13;

    return RT_ERR_OK;
}

/* add Implement */
rtk_api_ret_t dal_rtl8367d_setAsicPHYOCPReg(rtk_uint32 phyNo, rtk_uint32 ocpAddr, rtk_uint32 ocpData)
{
	return _dal_rtl8367d_setAsicPHYOCPReg(phyNo, ocpAddr, ocpData);
}
rtk_api_ret_t dal_rtl8367d_getAsicPHYOCPReg(rtk_uint32 phyNo, rtk_uint32 ocpAddr, rtk_uint32 *pRegData)
{
	return _dal_rtl8367d_getAsicPHYOCPReg(phyNo, ocpAddr, pRegData);
}
rtk_api_ret_t dal_rtl8367d_setAsicPHYReg(rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 phyData)
{
	return _dal_rtl8367d_setAsicPHYReg(phyNo, phyAddr, phyData);
}
rtk_api_ret_t dal_rtl8367d_getAsicPHYReg(rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 *pRegData)
{
	return _dal_rtl8367d_getAsicPHYReg(phyNo, phyAddr, pRegData);
}

