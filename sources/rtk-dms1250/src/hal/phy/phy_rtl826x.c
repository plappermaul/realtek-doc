/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * Purpose : PHY 826X Driver APIs.
 *
 * Feature : PHY 826X Driver APIs
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
#include <hal/phy/phy_rtl826x.h>
#include <hal/phy/phy_rtl826x_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/time.h>
#include <osal/memory.h>
#include <hwp/hwp_util.h>

/*
 * Symbol Definition
 */
#define PHY_826X_LOG    LOG_INFO
#define RT_EYEMON_LOG(fmt, args...)              RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "%s:%d:"fmt, __FUNCTION__, __LINE__, ##args)

#define PHY_826X_SERDES_MAX_RETRY                   10

#define PHY_826X_SDS_MODE_FIB1G                     0x4    /* 1000BASE-X/FIB1G (1.25G) */
#define PHY_826X_SDS_MODE_FIB100M                   0x5    /* 100BASE-FX/FIB100M (125M) */
#define PHY_826X_SDS_MODE_AUTO_1000BASEX_100BASEFX  0x7    /* 1000BASE-X/100BASE-FX auto-detection */
#define PHY_826X_SDS_MODE_USXGMII                   0xD    /* USXGMII (10.3125G)  */
#define PHY_826X_SDS_MODE_2500BASEX                 0x16   /* 2500BASE-X (3.125G) */
#define PHY_826X_SDS_MODE_10GBASER                  0x1A   /* 10GBASE-R (10.3125G w/o FEC, 10.3125G w/i FEC) */
#define PHY_826X_SDS_MODE_AUTO_10GBASER_SGMII       0x1B   /* 10GBASE-R/SGMII auto-detection */
#define PHY_826X_SDS_MODE_AUTO_10GBASER_1000BASEX   0x1C   /* 10GBASE-R/1000BASE-X auto-detection */
#define PHY_826X_SDS_MODE_OFF                       0x1F   /* Serdes OFF */

#define PHY_826X_SDS_MODE_SGMII                     0x2    /* SGMII */
#define PHY_826X_SDS_MODE_5000BASEX                 0x15   /* 5000BASE-X (6.25G) */

#define PHY_826X_SDS_SPD_TGR                        0
#define PHY_826X_SDS_SPD_5GR                        1
#define PHY_826X_SDS_SPD_5000X                      2
#define PHY_826X_SDS_SPD_2500X                      3
#define PHY_826X_SDS_SPD_1000X                      4

#define PHY_826X_PREEMPHASIS_GET_POST1(_data)    ((_data >> 26) & 0x3F)
#define PHY_826X_PREEMPHASIS_GET_POST2(_data)    ((_data >> 20) & 0x3F)
#define PHY_826X_PREEMPHASIS_GET_POST1_EN(_data) ((_data >> 19) & 0x1 )
#define PHY_826X_PREEMPHASIS_GET_POST2_EN(_data) ((_data >> 18) & 0x1 )
#define PHY_826X_PREEMPHASIS_GET_MAIN(_data)     ((_data >> 10) & 0x3F)
#define PHY_826X_PREEMPHASIS_GET_MAIN_EN(_data)  ((_data >>  9) & 0x1 )
#define PHY_826X_PREEMPHASIS_GET_PRE(_data)      ((_data >>  3) & 0x3F)
#define PHY_826X_PREEMPHASIS_GET_PRE_EN(_data)   ((_data >>  2) & 0x1 )

#define PHY_826X_PREEMPHASIS_SET_POST1(_data, _val)    do { _data = (_data & ~(0x3F << 26)) | ((_val& 0x3F) << 26); }while(0)
#define PHY_826X_PREEMPHASIS_SET_POST2(_data, _val)    do { _data = (_data & ~(0x3F << 20)) | ((_val& 0x3F) << 20); }while(0)
#define PHY_826X_PREEMPHASIS_SET_POST1_EN(_data, _val) do { _data = (_data & ~(0x1  << 19)) | ((_val& 0x1 ) << 19); }while(0)
#define PHY_826X_PREEMPHASIS_SET_POST2_EN(_data, _val) do { _data = (_data & ~(0x1  << 18)) | ((_val& 0x1 ) << 18); }while(0)
#define PHY_826X_PREEMPHASIS_SET_MAIN(_data, _val)     do { _data = (_data & ~(0x3F << 10)) | ((_val& 0x3F) << 10); }while(0)
#define PHY_826X_PREEMPHASIS_SET_MAIN_EN(_data, _val)  do { _data = (_data & ~(0x1  <<  9)) | ((_val& 0x1 ) <<  9); }while(0)
#define PHY_826X_PREEMPHASIS_SET_PRE(_data, _val)      do { _data = (_data & ~(0x3F <<  3)) | ((_val& 0x3F) <<  3); }while(0)
#define PHY_826X_PREEMPHASIS_SET_PRE_EN(_data, _val)   do { _data = (_data & ~(0x1  <<  2)) | ((_val& 0x1 ) <<  2); }while(0)

#define PHY_826X_EYE_SCAN_EN_PAGE         0x21
#define PHY_826X_EYE_SCAN_EN_REG          0x09
#define PHY_826X_EYE_SCAN_EN_BIT          3
#define PHY_826X_EYE_PI_PHASE_PAGE        0x21
#define PHY_826X_EYE_PI_PHASE_REG         0x0B
#define PHY_826X_EYE_REF_CTRL_PAGE        0x21
#define PHY_826X_EYE_REF_CTRL_REG         0x0A

#define PHY_826X_EYE_PAGE_BASE            0x2E
#define PHY_826X_EYE_PAGE_BASE_5GR        0x34
#define PHY_826X_EYE_PAGE_BASE_5GX        0x2C
#define PHY_826X_EYE_PAGE_BASE_2P5GX      0x28
#define PHY_826X_EYE_PAGE_BASE_1G         0x24

#define DEBUG_DUMP_MDELAY      0
#if DEBUG_DUMP_MDELAY
#define DEBUG_COUPLING_PRINT(_fmt, args...)  do {osal_printf(_fmt, ##args); osal_time_mdelay(DEBUG_DUMP_MDELAY);}while(0)
#define DEBUG_DSP_PRINT(_fmt, args...)  do {osal_printf(_fmt, ##args); osal_time_mdelay(DEBUG_DUMP_MDELAY);}while(0)
#else
#define DEBUG_COUPLING_PRINT     osal_printf
#define DEBUG_DSP_PRINT          osal_printf
#endif
/*
 * Data Declaration
 */
rt_phyInfo_t phy_8261_info =
{
    .phy_num    = PORT_NUM_IN_8261,
    .eth_type   = HWP_XGE,
    .isComboPhy = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .flags      = RTK_PHYINFO_FLAG_NONE,
};

rt_phyInfo_t phy_8264_info =
{
    .phy_num    = PORT_NUM_IN_8264,
    .eth_type   = HWP_XGE,
    .isComboPhy = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .flags      = RTK_PHYINFO_FLAG_NONE,
};

rt_phyInfo_t phy_8251_info =
{
    .phy_num    = PORT_NUM_IN_8261,
    .eth_type   = HWP_5GE,
    .isComboPhy = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .flags      = RTK_PHYINFO_FLAG_NONE,
};

rt_phyInfo_t phy_8254_info =
{
    .phy_num    = PORT_NUM_IN_8264,
    .eth_type   = HWP_5GE,
    .isComboPhy = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .flags      = RTK_PHYINFO_FLAG_NONE,
};

typedef struct phy_rtl826x_info_s
{
    uint32 sdsModeCfg[RTK_MAX_PORT_PER_UNIT];
    uint8  rtctCable[RTK_MAX_PORT_PER_UNIT];
    uint32 eyeParamTarget[RTK_MAX_PORT_PER_UNIT];
} phy_rtl826x_info_t;

phy_rtl826x_info_t    *rtl826x_info[RTK_MAX_NUM_OF_UNIT] = { NULL };
/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
int32 phy_826x_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID);
int32 phy_826x_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
int32 _phy_826x_rtct_cable_set(uint32 unit, rtk_port_t port, uint32 cableType);

int32 _phy_826x_chip_ver_get(uint32 unit, uint8 port, uint32 *ver)
{
    int32 ret = 0;
    uint32 rData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 30, 0x104, &rData)) != RT_ERR_OK)
            return ret;

    *ver = REG32_FIELD_GET(rData, 0, 0x000F);
    return ret;
}


rt_port_ethType_t _phy_826x_chip_type_get(uint32 unit, uint8 port)
{
    uint32 rData = 0;
    phy_common_general_reg_mmd_get(unit, port, 30, 0x103, &rData);

    switch (rData)
    {
        case 0x8291:
        case 0x8294:
        case 0x8261:
        case 0x8264:
            return HWP_XGE;
        case 0x8251:
        case 0x8254:
            return HWP_5GE;
        case 0x8224:
        default:
            return HWP_2_5GE;
    }
    return HWP_2_5GE;
}

int8
_phy_826x_is_single_port(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData;

#if 1
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x103, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData & 0xF) == 0x1)
    {
        return TRUE;
    }

    return FALSE;
#else
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x101, &phyData)) != RT_ERR_OK)
        return ret;
    if ((phyData & 0xF000) != (0xB000))
    {
        phyData &= (~(0xF000));
        phyData |= (0xB000);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x101, phyData)) != RT_ERR_OK)
            return ret;
    }

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x102, &phyData)) != RT_ERR_OK)
        return FALSE;

    if ((phyData & 0x60) == 0)
        return TRUE;
    else
        return FALSE;
#endif
}

int32
_phy_826x_indirect_read(uint32 unit, rtk_port_t port, uint32 indr_addr, uint32 *pData)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA436, indr_addr)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA438, &phyData)) != RT_ERR_OK)
        return ret;

    *pData = phyData;

    return RT_ERR_OK;
}

int32
_phy_826x_indirect_write(uint32 unit, rtk_port_t port, uint32 indr_addr, uint32 phyData)
{
    int32   ret = RT_ERR_OK;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA436, indr_addr)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA438, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_serdes_reg_write(uint32 unit, rtk_port_t port, uint32 page, uint32 reg, uint32 data)
{
    int32   ret = RT_ERR_OK;
    uint32  regData = 0, phyData = 0;

    regData |= (page & 0x3f);
    regData |= ((reg & 0x1f) << 6);
    regData |= (BIT_11 | BIT_15);
    phyData = data;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 321, phyData)) != RT_ERR_OK)
            return ret;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 323, regData)) != RT_ERR_OK)
            return ret;

    return ret;
}

int32
_phy_826x_serdes_reg_read(uint32 unit, rtk_port_t port, uint32 page, uint32 reg, uint32 *pData)
{
    int32   ret, i;
    uint32  regData = 0;
    uint32  phyData = 0;

    regData |= (page & 0x3f);
    regData |= ((reg & 0x1f) << 6);
    regData |= BIT_15;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 323, regData)) != RT_ERR_OK)
            return ret;

    for (i = 0; i < PHY_826X_SERDES_MAX_RETRY; i++)
    {
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 323, &regData)) != RT_ERR_OK)
                return ret;
        if ((regData & BIT_15) == 0)
        {
            break;
        }
        osal_time_udelay(10);
    }
    if (i == PHY_826X_SERDES_MAX_RETRY)
    {
        return RT_ERR_TIMEOUT;
    }

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 322, &phyData)) != RT_ERR_OK)
            return ret;

    *pData = phyData;
    return ret;
}

int32
_phy_826x_an_restart(uint32 unit, rtk_port_t port)
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
_phy_826x_dbgCount_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC800, 0x5A02)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1EE, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= BIT_1;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1EE, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x230, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= BIT_1;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x230, phyData)) != RT_ERR_OK)
        return ret;

    phy_826x_ctrl_set(unit, port, RTK_PHY_CTRL_COUNTER_CLEAR, 1);
    return ret;
}

int32
_phy_826x_interrupt_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_826x_ctrl_set(unit, port, RTK_PHY_CTRL_RAPID_LINK_FAULT_DETECT, 1)) != RT_ERR_OK)
        return ret;

    /* Disable all IMR*/
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE1, 0)) != RT_ERR_OK)
        return ret;
    /* Disable all SDS IMR*/
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE3, 0)) != RT_ERR_OK)
        return ret;

    /* Set SDS interrupt source to INT_PHY0 */
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE4, 0x1)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE0, 0x2F)) != RT_ERR_OK)
        return ret;

    /* rlfd_en */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (BIT_15);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
        return ret;

    /* rlfd_fr_en */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA448, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (BIT_7);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA448, phyData)) != RT_ERR_OK)
        return ret;

    /* clear status */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA43A, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE2, 0x3F)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_interrupt_enable_set(uint32 unit, rtk_port_t port, uint32 enaBitmap)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0, currentData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA424, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~(BIT_2 | BIT_3 | BIT_4 | BIT_9));
    if (enaBitmap & RTK_PHY_CTRL_INTR_NEXT_PAGE_RECV)
        phyData |= BIT_2;
    if (enaBitmap & RTK_PHY_CTRL_INTR_AN_COMPLETE)
        phyData |= BIT_3;
    if (enaBitmap & RTK_PHY_CTRL_INTR_LINK_CHANGE)
        phyData |= BIT_4;
    if (enaBitmap & RTK_PHY_CTRL_INTR_ALDPS_STATE_CHANGE)
        phyData |= BIT_9;
    if (currentData != phyData)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA424, phyData)) != RT_ERR_OK)
            return ret;
    }

    /* rlfd_en */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~BIT_15);
    phyData |= (enaBitmap & RTK_PHY_CTRL_INTR_RLFD)? (BIT_15) : (0);
    if (currentData != phyData)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
            return ret;
    }

    /* tm */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A0, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~BIT_11);
    phyData |= (enaBitmap & RTK_PHY_CTRL_INTR_TM_HIGH)? (BIT_11) : (0);
    if (currentData != phyData)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1A0, phyData)) != RT_ERR_OK)
            return ret;
    }
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19D, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~BIT_11);
    phyData |= (enaBitmap & RTK_PHY_CTRL_INTR_TM_HIGH)? (BIT_11) : (0);
    if (currentData != phyData)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x19D, phyData)) != RT_ERR_OK)
            return ret;
    }
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A1, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~BIT_11);
    phyData |= (enaBitmap & RTK_PHY_CTRL_INTR_TM_LOW)? (BIT_11) : (0);
    if (currentData != phyData)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1A1, phyData)) != RT_ERR_OK)
            return ret;
    }
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19F, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~BIT_11);
    phyData |= (enaBitmap & RTK_PHY_CTRL_INTR_TM_LOW)? (BIT_11) : (0);
    if (currentData != phyData)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x19F, phyData)) != RT_ERR_OK)
            return ret;
    }

    return ret;
}

int32
_phy_826x_interrupt_enable_get(uint32 unit, rtk_port_t port, uint32 *pEnaBitmap)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  bitmap = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA424, &phyData)) != RT_ERR_OK)
        return ret;
    if (phyData & BIT_2)
        bitmap |= RTK_PHY_CTRL_INTR_NEXT_PAGE_RECV;
    if (phyData & BIT_3)
        bitmap |= RTK_PHY_CTRL_INTR_AN_COMPLETE;
    if (phyData & BIT_4)
        bitmap |= RTK_PHY_CTRL_INTR_LINK_CHANGE;
    if (phyData & BIT_9)
        bitmap |= RTK_PHY_CTRL_INTR_ALDPS_STATE_CHANGE;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    if (phyData & BIT_15)
        bitmap |= RTK_PHY_CTRL_INTR_RLFD;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19D, &phyData)) != RT_ERR_OK)
        return ret;
    if (phyData & BIT_11)
        bitmap |= RTK_PHY_CTRL_INTR_TM_HIGH;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A1, &phyData)) != RT_ERR_OK)
        return ret;
    if (phyData & BIT_11)
        bitmap |= RTK_PHY_CTRL_INTR_TM_LOW;

    *pEnaBitmap = bitmap;
    return ret;
}

int32
_phy_826x_interrupt_mask_set(uint32 unit, rtk_port_t port, uint32 enaBitmap)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0, currentData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xE1, &currentData)) != RT_ERR_OK)
        return ret;
    phyData = currentData;
    phyData &= (~(BIT_0 | BIT_1 | BIT_3 | BIT_4));
    if (enaBitmap & RTK_PHY_CTRL_INTR_MASK_COMMON)
        phyData |= BIT_0;
    if (enaBitmap & RTK_PHY_CTRL_INTR_MASK_RLFD)
        phyData |= BIT_1;
    if (enaBitmap & RTK_PHY_CTRL_INTR_MASK_TM_LOW)
        phyData |= BIT_3;
    if (enaBitmap & RTK_PHY_CTRL_INTR_MASK_TM_HIGH)
        phyData |= BIT_4;

    if (currentData != phyData)
    {
        ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE1, phyData);
        return ret;
    }
    return ret;
}

int32
_phy_826x_interrupt_mask_get(uint32 unit, rtk_port_t port, uint32 *pEnaBitmap)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  bitmap = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xE1, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_0)
        bitmap |= RTK_PHY_CTRL_INTR_MASK_COMMON;
    if (phyData & BIT_1)
        bitmap |= RTK_PHY_CTRL_INTR_MASK_RLFD;
    if (phyData & BIT_3)
        bitmap |= RTK_PHY_CTRL_INTR_MASK_TM_LOW;
    if (phyData & BIT_4)
        bitmap |= RTK_PHY_CTRL_INTR_MASK_TM_HIGH;
    *pEnaBitmap = bitmap;
    return ret;
}

int32
_phy_826x_interrupt_status_get(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  bitmap = 0;

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

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xE2, &phyData)) != RT_ERR_OK)
        return ret;
    if (phyData & BIT_1)
        bitmap |= RTK_PHY_CTRL_INTR_RLFD;
    if (phyData & BIT_3)
        bitmap |= RTK_PHY_CTRL_INTR_TM_LOW;
    if (phyData & BIT_4)
        bitmap |= RTK_PHY_CTRL_INTR_TM_HIGH;

    //Clear
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xE2, 0x3F)) != RT_ERR_OK)
            return ret;

    *pStatus = bitmap;
    return ret;
}

int32
_phy_826x_synce_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);

    if (TRUE == is_singlePort)
    {
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_3));
        phyData |= (BIT_8);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_4 | BIT_12));
        phyData |= (BIT_7 | BIT_15);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_8 | BIT_9 | BIT_10 | BIT_11 | BIT_12 | BIT_15));
        phyData |= (BIT_5 | BIT_13);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if (port == HWP_PHY_BASE_MACID(unit, port))
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_4 | BIT_12));
            phyData |= (BIT_7 | BIT_15);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_4 | BIT_12));
            phyData |= (BIT_5 | BIT_13);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_3 | BIT_0));
        phyData |= (BIT_8);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_8 | BIT_9 | BIT_10 | BIT_11 | BIT_15));
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
            return ret;
    }
    return ret;
}

int32
_phy_826x_synce_clock_freq_set(uint32 unit, rtk_port_t port, uint32 synce_id, uint32 val)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  freq = 0;
    uint32  i = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);
    uint8   target_port;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (TRUE == is_singlePort)
    {
        if (synce_id != 0)
        {
            return RT_ERR_INPUT;
        }

        switch (val)
        {
            case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_50MHZ:
                freq = 0;
                break;
            case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_25MHZ:
                freq = 1;
                break;
            case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_8KHZ:
                return RT_ERR_NOT_ALLOWED;
            default:
                return RT_ERR_INPUT;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_0));
        phyData |= (freq == 1)? (BIT_0) : (0);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~(BIT_9));
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        switch (val)
        {
            case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_50MHZ:
                freq = 0;
                break;
            case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_25MHZ:
                freq = 1;
                break;
            case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_8KHZ:
                freq = 2;
                break;
            default:
                return RT_ERR_INPUT;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
            return ret;
        if (synce_id == 0)
        {
            phyData = REG32_FIELD_SET(phyData, freq, 0, 0x3);
        }
        else if (synce_id == 1)
        {
            phyData = REG32_FIELD_SET(phyData, freq, 8, 0x300);
        }
        else
        {
            return RT_ERR_INPUT;
        }
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
            return ret;

        for (i = 0; i < 4; i++)
        {
            if ((ret = hwp_get_port_by_baseport_offset(unit, port, i, &target_port)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, target_port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_9));
            if ((ret = phy_common_general_reg_mmd_set(unit, target_port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                return ret;
        }

    }
    return ret;
}

int32
_phy_826x_synce_clock_freq_get(uint32 unit, rtk_port_t port, uint32 synce_id, uint32 *pVal)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  freq = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (TRUE == is_singlePort)
    {
        if (synce_id != 0)
        {
            return RT_ERR_INPUT;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
            return ret;
        freq = (phyData & BIT_0)? (1) : (0);
    }
    else
    {
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
            return ret;

        switch (synce_id)
        {
            case 0:
                freq = REG32_FIELD_GET(phyData, 0, 0x3);
                break;
            case 1:
                freq = REG32_FIELD_GET(phyData, 8, 0x300);
                break;
            default:
                return RT_ERR_INPUT;
        }
    }

    switch (freq)
    {
        case 0:
            *pVal = RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_50MHZ;
            break;
        case 1:
            *pVal = RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_25MHZ;
            break;
        case 2:
            *pVal = RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_8KHZ;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}

int32
_phy_826x_synce_recovery_phy_set(uint32 unit, rtk_port_t port, uint32 synce_id, uint32 phy)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if (_phy_826x_is_single_port(unit, port) == TRUE)
        return RT_ERR_NOT_SUPPORTED;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (phy >= PORT_NUM_IN_8264)
        return RT_ERR_INPUT;

    switch(synce_id)
    {
        case 0:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, phy, 5, 0x60);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, phy, 2, 0xC);
            phyData = REG32_FIELD_SET(phyData, phy, 0, 0x3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case 1:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, phy, 13, 0x6000);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, phy, 10, 0xC00);
            phyData = REG32_FIELD_SET(phyData, phy, 8, 0x300);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}

int32
_phy_826x_synce_recovery_phy_get(uint32 unit, rtk_port_t port, uint32 synce_id, uint32 *phy)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if (_phy_826x_is_single_port(unit, port) == TRUE)
        return RT_ERR_NOT_SUPPORTED;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    switch (synce_id)
    {
        case 0:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            *phy = REG32_FIELD_GET(phyData, 5, 0x60);
            break;
        case 1:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            *phy = REG32_FIELD_GET(phyData, 13, 0x6000);
            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}

int32
_phy_826x_synce_idle_mode_set(uint32 unit, rtk_port_t port, uint32 synce_id, uint32 val)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  i = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);
    uint8   target_port;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (TRUE == is_singlePort)
    {
        if (synce_id != 0)
        {
            return RT_ERR_INPUT;
        }

        switch (val)
        {
            case RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~(BIT_2));
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            case RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~(BIT_2 | BIT_1));
                phyData |= (BIT_2);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            case RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_2 | BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                    return ret;
                break;
        }
    }
    else
    {
        if (synce_id > 1)
        {
            return RT_ERR_INPUT;
        }

        for (i = 0; i < 4; i++)
        {
            if ((ret = hwp_get_port_by_baseport_offset(unit, port, i, &target_port)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, target_port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_2));
            if ((ret = phy_common_general_reg_mmd_set(unit, target_port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                return ret;
        }

        switch (val)
        {
            case RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (synce_id == 0)? (~(BIT_3)) : (~(BIT_11));

                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            case RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (synce_id == 0)? (~(BIT_2)) : (~(BIT_10));
                phyData |= (synce_id == 0)? (BIT_3) : (BIT_11);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            case RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (synce_id == 0)? (BIT_2) : (BIT_10);
                phyData |= (synce_id == 0)? (BIT_3) : (BIT_11);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                    return ret;
                break;
        }
    }
    return ret;
}

int32
_phy_826x_synce_idle_mode_get(uint32 unit, rtk_port_t port, uint32 synce_id, uint32 *pVal)
{
    int32   ret = 0;
    uint32  phyData = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (TRUE == is_singlePort)
    {
        if (synce_id != 0)
        {
            return RT_ERR_INPUT;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
            return ret;
        if (phyData & BIT_2)
        {
            if (phyData & BIT_1)
            {
                *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH;
            }
            else
            {
                *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW;
            }
        }
        else
        {
            *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN;
        }
    }
    else
    {
        if (synce_id > 1)
        {
            return RT_ERR_INPUT;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
            return ret;

        if (synce_id == 0)
        {
            if (phyData & BIT_3)
            {
                if (phyData & BIT_2)
                {
                    *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH;
                }
                else
                {
                    *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW;
                }
            }
            else
            {
                *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN;
            }
        }
        else
        {
            if (phyData & BIT_11)
            {
                if (phyData & BIT_10)
                {
                    *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH;
                }
                else
                {
                    *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW;
                }
            }
            else
            {
                *pVal = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN;
            }
        }
    }
    return ret;
}

int32
_phy_826x_synce_enable_set(uint32 unit, rtk_port_t port, uint32 val)
{
    int32   ret = 0;
    uint32  phyData = 0;
    uint32  i = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);
    uint8   target_port;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (TRUE == is_singlePort)
    {
        if (0 == val)
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xbf98, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xbf98, phyData)) != RT_ERR_OK)
                return ret;
            return _phy_826x_synce_init(unit, port);
        }
        else
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_8 | BIT_10 | BIT_11));
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_5));
            phyData |= (BIT_15);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xbf98, &phyData)) != RT_ERR_OK)
                return ret;
            phyData |= (BIT_3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xbf98, phyData)) != RT_ERR_OK)
                return ret;
        }
    }
    else
    {
        if (0 == val)
        {
            return _phy_826x_synce_init(unit, port);
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                if ((ret = hwp_get_port_by_baseport_offset(unit, port, i, &target_port)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, target_port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~(BIT_10 | BIT_11));
                phyData |= (BIT_8);
                if ((ret = phy_common_general_reg_mmd_set(unit, target_port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                    return ret;
            }

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_7 | BIT_15));
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_5 | BIT_13));
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;

            for (i = 0; i < 4; i++)
            {
                if ((ret = hwp_get_port_by_baseport_offset(unit, port, i, &target_port)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, target_port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_15);
                if ((ret = phy_common_general_reg_mmd_set(unit, target_port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                    return ret;
            }
        }
    }
    return ret;
}

int32
_phy_826x_synce_pll_set(uint32 unit, rtk_port_t port, uint32 val)
{
    int32   ret = 0;
    uint32  phyData = 0;
    uint32  i = 0;
    uint32  is_singlePort = _phy_826x_is_single_port(unit, port);
    uint8   target_port;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if (TRUE == is_singlePort)
    {
        if (0 == val)
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xbf98, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xbf98, phyData)) != RT_ERR_OK)
                return ret;
            return _phy_826x_synce_init(unit, port);
        }
        else
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x0, 11, BIT_11);
            phyData = REG32_FIELD_SET(phyData, 0x0, 10, BIT_10);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x0, 5, BIT_5);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x1, 9, BIT_9);
            phyData = REG32_FIELD_SET(phyData, 0x1, 8, BIT_8);
            phyData = REG32_FIELD_SET(phyData, 0x0, 4, BIT_4);
            phyData = REG32_FIELD_SET(phyData, 0x1, 3, BIT_3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x1, 15, BIT_15);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xbf98, &phyData)) != RT_ERR_OK)
                return ret;
            phyData |= (BIT_3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xbf98, phyData)) != RT_ERR_OK)
                return ret;
        }
    }
    else
    {
        if (0 == val)
        {
            return _phy_826x_synce_init(unit, port);
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                if ((ret = hwp_get_port_by_baseport_offset(unit, port, i, &target_port)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, target_port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData = REG32_FIELD_SET(phyData, 0x0, 11, BIT_11);
                phyData = REG32_FIELD_SET(phyData, 0x0, 10, BIT_10);
                if ((ret = phy_common_general_reg_mmd_set(unit, target_port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                    return ret;
            }

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x0, 5, BIT_5);
            phyData = REG32_FIELD_SET(phyData, 0x0, 13, BIT_13);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x1, 9, BIT_9);
            phyData = REG32_FIELD_SET(phyData, 0x1, 8, BIT_8);
            phyData = REG32_FIELD_SET(phyData, 0x0, 4, BIT_4);
            phyData = REG32_FIELD_SET(phyData, 0x1, 3, BIT_3);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C8, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1C9, &phyData)) != RT_ERR_OK)
                return ret;
            phyData = REG32_FIELD_SET(phyData, 0x1, 7, BIT_7);
            phyData = REG32_FIELD_SET(phyData, 0x1, 4, BIT_4);
            phyData = REG32_FIELD_SET(phyData, 0x1, 15, BIT_15);
            phyData = REG32_FIELD_SET(phyData, 0x1, 12, BIT_12);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1C9, phyData)) != RT_ERR_OK)
                return ret;

            for (i = 0; i < 4; i++)
            {
                if ((ret = hwp_get_port_by_baseport_offset(unit, port, i, &target_port)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, target_port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData = REG32_FIELD_SET(phyData, 0x1, 15, BIT_15);
                if ((ret = phy_common_general_reg_mmd_set(unit, target_port, PHY_MMD_VEND1, 0x1CA, phyData)) != RT_ERR_OK)
                    return ret;
            }
        }
    }
    return ret;
}

int32
_phy_826x_synce_enable_get(uint32 unit, rtk_port_t port, uint32 *pVal)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_NOT_SUPPORTED;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1CA, &phyData)) != RT_ERR_OK)
        return ret;

    *pVal = (phyData & BIT_15) ? (1) : (0);

    return ret;
}

int32
_phy_826x_skew_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32* skew)
{
    int32  ret = 0;
    rtk_port_speed_t speed;
    uint32  phyData = 0;

    uint32  a_idx = 0;
    uint32  t_idx = 0;
    if ((ret = phy_common_c45_speedStatusResReg_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_SKEW_PAIR_B:
            if (speed == PORT_SPEED_1000M)
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA726, &phyData)) != RT_ERR_OK)
                    return ret;
                a_idx = REG32_FIELD_GET(phyData, 0, 0xF);
                t_idx = REG32_FIELD_GET(phyData, 4, 0xF0);
                *skew = (t_idx > a_idx) ? (t_idx - a_idx) : (a_idx - t_idx);
            }
            else
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 145, &phyData)) != RT_ERR_OK)
                    return ret;
                *skew = REG32_FIELD_GET(phyData, 8, 0x7F00);
            }
            break;
        case RTK_PHY_CTRL_SKEW_PAIR_C:
            if (speed == PORT_SPEED_1000M)
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA726, &phyData)) != RT_ERR_OK)
                    return ret;
                a_idx = REG32_FIELD_GET(phyData, 0, 0xF);
                t_idx = REG32_FIELD_GET(phyData, 8, 0xF00);
                *skew = (t_idx > a_idx) ? (t_idx - a_idx) : (a_idx - t_idx);
            }
            else
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 146, &phyData)) != RT_ERR_OK)
                    return ret;
                *skew = REG32_FIELD_GET(phyData, 8, 0x7F00);
            }
            break;
        case RTK_PHY_CTRL_SKEW_PAIR_D:
            if (speed == PORT_SPEED_1000M)
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA726, &phyData)) != RT_ERR_OK)
                    return ret;
                a_idx = REG32_FIELD_GET(phyData, 0, 0xF);
                t_idx = REG32_FIELD_GET(phyData, 12, 0xF000);
                *skew = (t_idx > a_idx) ? (t_idx - a_idx) : (a_idx - t_idx);
            }
            else
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 146, &phyData)) != RT_ERR_OK)
                    return ret;
                *skew = REG32_FIELD_GET(phyData, 0, 0x7F);
            }
            break;
        default:
            return RT_ERR_INPUT;
    }


    return ret;
}

int32
_phy_826x_temperature_get(uint32 unit, rtk_port_t port, uint32 *temp)
{
    int32  ret = 0;
    uint32 tmH = 0, tmL = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A8, &tmH)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A9, &tmL)) != RT_ERR_OK)
        return ret;

    *temp = (tmH << 3) | (tmL & 0x7);

    return ret;
}

int32
_phy_826x_temperature_threshold_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 val)
{
    int32  ret = 0;
    uint32 sign =0;      /* [18],1-bit */
    uint32 integer = 0;  /* [17:10],8-bit */
    uint32 decimalH = 0; /* [9:8],2-bit */
    uint32 decimalL = 0; /* [7:0],8-bit */
    uint32 phyData = 0;

    sign     = (val & 0x40000) >> 18;
    integer  = (val & 0x3FC00) >> 10;
    decimalH = (val & 0x00300) >> 8;
    decimalL = (val & 0x000FF);

    if (val & (~0x7FFFF))
    {
        RT_LOG(PHY_826X_LOG, (MOD_HAL|MOD_PHY), "U%u P%u 826xtemp threshold set t:%d v:0x%X\n", unit, port, ctrl_type, val);
        return RT_ERR_INPUT;
    }

    switch (ctrl_type)
    {
            case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A0, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0x7FF);
                phyData |= ((sign << 10) | (integer << 2) | decimalH);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1A0, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A2, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0xFF);
                phyData |= decimalL;
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1A2, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19D, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0x7FF);
                phyData |= ((sign << 10) | (integer << 2) | decimalH);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x19D, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19E, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0xFF);
                phyData |= decimalL;
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x19E, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A1, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0x7FF);
                phyData |= ((sign << 10) | (integer << 2) | decimalH);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1A1, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A2, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0xFF00);
                phyData |= decimalL;
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1A2, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19F, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0x7FF);
                phyData |= ((sign << 10) | (integer << 2) | decimalH);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x19F, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19E, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~0xFF00);
                phyData |= decimalL;
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x19E, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_INPUT;
    }
    return ret;
}

int32
_phy_826x_temperature_threshold_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32* val)
{
    int32  ret = 0;
    uint32 phyData1 = 0;
    uint32 phyData2 = 0;
    uint32 temp = 0;

    switch (ctrl_type)
    {
            case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A0, &phyData1)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A2, &phyData2)) != RT_ERR_OK)
                    return ret;
                temp = ((phyData1 & 0x07FF) << 8) | (phyData2 & 0x00FF);
                break;

            case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19D, &phyData1)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19E, &phyData2)) != RT_ERR_OK)
                    return ret;
                temp = ((phyData1 & 0x07FF) << 8) | (phyData2 & 0x00FF);
                break;

            case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A1, &phyData1)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1A2, &phyData2)) != RT_ERR_OK)
                    return ret;

                temp = ((phyData1 & 0x07FF) << 8) | (phyData2 & 0xFF00);
                break;

            case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19F, &phyData1)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x19E, &phyData2)) != RT_ERR_OK)
                    return ret;
                temp = ((phyData1 & 0x07FF) << 8) | (phyData2 & 0xFF00);
                break;
            default:
                return RT_ERR_INPUT;
    }

    *val = temp;
    return ret;
}

int32
_phy_826x_serdes_mode_init(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;

    if(phyData & 0x1)
    {
        rtl826x_info[unit]->sdsModeCfg[port] = RTK_PHY_CTRL_SERDES_MODE_UNKNOWN;
    }
    else
    {
        rtl826x_info[unit]->sdsModeCfg[port] = RTK_PHY_CTRL_SERDES_MODE_USXGMII;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_init
 * Description:
 *      Initial the PHY for specific port.
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
phy_826x_init(uint32 unit, rtk_port_t port)
{
    int32  ret = RT_ERR_OK;
    int32  sz = sizeof(phy_rtl826x_info_t);

    if (rtl826x_info[unit] == NULL)
    {
        if ((rtl826x_info[unit] = osal_alloc(sz)) == NULL)
        {
            RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
        osal_memset(rtl826x_info[unit], 0, sz);
    }

    if ((ret = _phy_826x_serdes_mode_init(unit, port)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_interrupt_init(unit, port)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_dbgCount_init(unit, port)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_synce_init(unit, port)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_826x_ctrl_set(unit, port, RTK_PHY_CTRL_RTCT_CABLE_TYPE ,RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer to output phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32
phy_826x_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_autoNegoAbilityLocal_get
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
phy_826x_autoNegoAbilityLocal_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rt_port_ethType_t eth_type = _phy_826x_chip_type_get(unit,port);
    osal_memset(pAbility, 0x0, sizeof(rtk_port_phy_ability_t));
    pAbility->FC = 1;
    pAbility->AsyFC = 1;
    pAbility->Half_100 = 1;
    pAbility->Full_100 = 1;
    pAbility->Full_1000 = 1;

    if(eth_type == HWP_2_5GE)
    {
        pAbility->adv_2_5G = 1;
    }
    else if(eth_type == HWP_5GE)
    {
        pAbility->adv_2_5G = 1;
        pAbility->adv_5G = 1;
    }
    else
    {
        pAbility->adv_2_5G = 1;
        pAbility->adv_5G = 1;
        pAbility->adv_10GBase_T = 1;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_autoNegoAbility_get
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
phy_826x_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rt_port_ethType_t eth_type = _phy_826x_chip_type_get(unit,port);

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA412, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->Half_1000  = (phyData & BIT_8) ? (1) : (0);
    pAbility->Full_1000  = (phyData & BIT_9) ? (1) : (0);

    ret = phy_common_c45_autoNegoAbility_get(unit, port, pAbility);

    if (eth_type == HWP_2_5GE)
    {
        pAbility->adv_5G = 0;
        pAbility->adv_10GBase_T = 0;
    }
    else if (eth_type == HWP_5GE)
    {
        pAbility->adv_10GBase_T = 0;
    }

    return ret;
}

/* Function Name:
 *      phy_826x_autoNegoAbility_set
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
phy_826x_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rt_port_ethType_t eth_type = _phy_826x_chip_type_get(unit,port);

    if (pAbility->Half_10 || pAbility->Full_10)
    {
        return RT_ERR_INPUT;
    }

    if (pAbility->adv_10GBase_T && (eth_type != HWP_XGE))
    {
        return RT_ERR_INPUT;
    }
    if (pAbility->adv_5G && (eth_type == HWP_2_5GE))
    {
        return RT_ERR_INPUT;
    }

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 16, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_5 | BIT_6 | BIT_7 | BIT_8 | BIT_10 | BIT_11));
    phyData |= (pAbility->Half_100) ? (BIT_7) : (0);
    phyData |= (pAbility->Full_100) ? (BIT_8) : (0);
    phyData |= (pAbility->FC) ? (BIT_10) : (0);
    phyData |= (pAbility->AsyFC) ? (BIT_11) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 16, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_7 | BIT_8 | BIT_12));
    phyData |= (pAbility->adv_2_5G) ? (BIT_7) : (0);
    phyData |= (pAbility->adv_5G) ? (BIT_8) : (0);
    phyData |= (pAbility->adv_10GBase_T) ? (BIT_12) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA412, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_8 | BIT_9));
    phyData |= (pAbility->Half_1000) ? (BIT_8) : (0);
    phyData |= (pAbility->Full_1000) ? (BIT_9) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA412, phyData)) != RT_ERR_OK)
        return ret;

    ret = _phy_826x_an_restart(unit, port);
    return ret;
}

/* Function Name:
*      phy_826x_autoNegoAbilityPeer_get
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
phy_826x_autoNegoAbilityPeer_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
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
 *      phy_826x_duplex_get
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
phy_826x_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
        return ret;

    *pDuplex = (phyData & BIT_8) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;

    return ret;
}

/* Function Name:
 *      phy_826x_duplex_set
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
phy_826x_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex)
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
*      phy_826x_linkStatus_get
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
phy_826x_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;
    *pStatus = (phyData & BIT_2) ? PORT_LINKUP : PORT_LINKDOWN;
    return ret;
}

int32
_phy_826x_realTime_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA434, &phyData)) != RT_ERR_OK)
        return ret;
    *pStatus = (phyData & BIT_2) ? PORT_LINKUP : PORT_LINKDOWN;
    return ret;
}
/* Function Name:
 *      phy_826x_crossOverMode_get
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
phy_826x_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
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
 *      phy_826x_crossOverMode_set
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
phy_826x_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
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
 *      phy_826x_liteEnable_get
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
phy_826x_liteEnable_get(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rt_port_ethType_t eth_type = _phy_826x_chip_type_get(unit,port);

    if((eth_type == HWP_2_5GE) && ((mode == PORT_LITE_5G) || (mode == PORT_LITE_10G)))
    {
        return RT_ERR_INPUT;
    }
    else if(eth_type == HWP_5GE && (mode == PORT_LITE_10G))
    {
        return RT_ERR_INPUT;
    }

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
        case PORT_LITE_5G:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                return ret;
            *pEnable = (phyData & BIT_1) ? (ENABLED):(DISABLED);
            break;
        case PORT_LITE_10G:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                return ret;
            *pEnable = (phyData & BIT_2) ? (ENABLED):(DISABLED);
            break;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }
    return ret;
}

/* Function Name:
 *      phy_826x_liteEnable_set
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
phy_826x_liteEnable_set(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rt_port_ethType_t eth_type = _phy_826x_chip_type_get(unit,port);

    if (ENABLED == enable)
    {
        if ((eth_type == HWP_2_5GE) && ((mode == PORT_LITE_5G) || (mode == PORT_LITE_10G)))
        {
            return RT_ERR_INPUT;
        }
        else if (eth_type == HWP_5GE && (mode == PORT_LITE_10G))
        {
            return RT_ERR_INPUT;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
            return ret;
        phyData |= (BIT_2);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
            return ret;

        switch (mode)
        {
            case PORT_LITE_1G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_9);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_9);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA428, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_2P5G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5FA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5FA, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_5G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5FA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5FA, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_10G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_2);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~BIT_2);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
            return ret;

        switch (mode)
        {
            case PORT_LITE_1G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_9);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_9);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA428, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_2P5G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5FA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5FA, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_5G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5FA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5FA, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_LITE_10G:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_2);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                    return ret;
                break;
            default:
                return RT_ERR_NOT_SUPPORTED;
        }
    }

    ret = _phy_826x_an_restart(unit, port);
    return ret;
}

int32
_phy_826x_rtct_cable_set(uint32 unit, rtk_port_t port, uint32 cableType)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC010, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 9, 0x200);
    phyData = REG32_FIELD_SET(phyData, 0x0, 10, 0x400);
    phyData = REG32_FIELD_SET(phyData, 0x1, 13, 0x6000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC010, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC000, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC000, phyData)) != RT_ERR_OK)
        return ret;

    switch (cableType)
    {
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E:
            _phy_826x_indirect_write(unit, port, 0x825E, 0x2500);
            _phy_826x_indirect_write(unit, port, 0x8270, 0x000C);
            _phy_826x_indirect_write(unit, port, 0x8282, 0x1F40);
            _phy_826x_indirect_write(unit, port, 0x8263, 0x0004);
            _phy_826x_indirect_write(unit, port, 0x8278, 0x03D8);
            _phy_826x_indirect_write(unit, port, 0x8274, 0x0101);
            _phy_826x_indirect_write(unit, port, 0x8276, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x827E, 0x0040);
            _phy_826x_indirect_write(unit, port, 0x8280, 0x0100);
            _phy_826x_indirect_write(unit, port, 0x827C, 0x1F40);
            _phy_826x_indirect_write(unit, port, 0x827A, 0x0004);
            _phy_826x_indirect_write(unit, port, 0x8261, 0x3E80);
            _phy_826x_indirect_write(unit, port, 0x825F, 0x41a0);
            _phy_826x_indirect_write(unit, port, 0x8265, 0x00e4);
            _phy_826x_indirect_write(unit, port, 0x8266, 0x3e80);
            _phy_826x_indirect_write(unit, port, 0x828C, 0x0011);
            _phy_826x_indirect_write(unit, port, 0x828E, 0x002B);
            _phy_826x_indirect_write(unit, port, 0x8290, 0xFFF6);
            _phy_826x_indirect_write(unit, port, 0x8292, 0x0001);
            _phy_826x_indirect_write(unit, port, 0x829C, 0x09C0);
            _phy_826x_indirect_write(unit, port, 0x829E, 0xFEA1);
            _phy_826x_indirect_write(unit, port, 0x82A0, 0x00D2);
            _phy_826x_indirect_write(unit, port, 0x82A2, 0xFFBD);
            _phy_826x_indirect_write(unit, port, 0x82A4, 0x08E0);
            _phy_826x_indirect_write(unit, port, 0x82A6, 0xFF12);
            _phy_826x_indirect_write(unit, port, 0x82A8, 0x0059);
            _phy_826x_indirect_write(unit, port, 0x82AA, 0xFFF2);
            _phy_826x_indirect_write(unit, port, 0x82B4, 0xECCE);
            _phy_826x_indirect_write(unit, port, 0x82B6, 0x010A);
            _phy_826x_indirect_write(unit, port, 0x82B8, 0x0046);
            _phy_826x_indirect_write(unit, port, 0x82BA, 0xFFB7);
            _phy_826x_indirect_write(unit, port, 0x82BC, 0xECE0);
            _phy_826x_indirect_write(unit, port, 0x82BE, 0x01A8);
            _phy_826x_indirect_write(unit, port, 0x82C0, 0xFF27);
            _phy_826x_indirect_write(unit, port, 0x82C2, 0x0046);
            _phy_826x_indirect_write(unit, port, 0x82CC, 0x050D);
            _phy_826x_indirect_write(unit, port, 0x82CE, 0xFF98);
            _phy_826x_indirect_write(unit, port, 0x82D0, 0x0010);
            _phy_826x_indirect_write(unit, port, 0x82D2, 0x0006);
            _phy_826x_indirect_write(unit, port, 0x82D4, 0x01E0);
            _phy_826x_indirect_write(unit, port, 0x82D6, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x82D8, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x82DA, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x826D, 0x3805);
            _phy_826x_indirect_write(unit, port, 0x826E, 0x0310);
            _phy_826x_indirect_write(unit, port, 0x825F, 0x47E0);
            _phy_826x_indirect_write(unit, port, 0x8261, 0x47E0);
            _phy_826x_indirect_write(unit, port, 0x8266, 0x47E0);
            break;

        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A:
            _phy_826x_indirect_write(unit, port, 0x825E, 0x2500);
            _phy_826x_indirect_write(unit, port, 0x8270, 0x000C);
            _phy_826x_indirect_write(unit, port, 0x8282, 0x1F40);
            _phy_826x_indirect_write(unit, port, 0x8263, 0x0004);
            _phy_826x_indirect_write(unit, port, 0x8278, 0x03D8);
            _phy_826x_indirect_write(unit, port, 0x8274, 0x0101);
            _phy_826x_indirect_write(unit, port, 0x8276, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x827E, 0x0040);
            _phy_826x_indirect_write(unit, port, 0x8280, 0x0100);
            _phy_826x_indirect_write(unit, port, 0x827C, 0x1F40);
            _phy_826x_indirect_write(unit, port, 0x827A, 0x0004);
            _phy_826x_indirect_write(unit, port, 0x8261, 0x3E80);
            _phy_826x_indirect_write(unit, port, 0x825F, 0x41a0);
            _phy_826x_indirect_write(unit, port, 0x8265, 0x00e4);
            _phy_826x_indirect_write(unit, port, 0x8266, 0x3e80);
            _phy_826x_indirect_write(unit, port, 0x828C, 0x0011);
            _phy_826x_indirect_write(unit, port, 0x828E, 0x002B);
            _phy_826x_indirect_write(unit, port, 0x8290, 0xFFF6);
            _phy_826x_indirect_write(unit, port, 0x8292, 0x0001);
            _phy_826x_indirect_write(unit, port, 0x829C, 0x0ACB);
            _phy_826x_indirect_write(unit, port, 0x829E, 0xFF50);
            _phy_826x_indirect_write(unit, port, 0x82A0, 0x0028);
            _phy_826x_indirect_write(unit, port, 0x82A2, 0xFFFA);
            _phy_826x_indirect_write(unit, port, 0x82A4, 0x101C);
            _phy_826x_indirect_write(unit, port, 0x82A6, 0xFE86);
            _phy_826x_indirect_write(unit, port, 0x82A8, 0x0098);
            _phy_826x_indirect_write(unit, port, 0x82AA, 0xFFD8);
            _phy_826x_indirect_write(unit, port, 0x82B4, 0xEA1C);
            _phy_826x_indirect_write(unit, port, 0x82B6, 0x01B4);
            _phy_826x_indirect_write(unit, port, 0x82B8, 0xFF91);
            _phy_826x_indirect_write(unit, port, 0x82BA, 0x000F);
            _phy_826x_indirect_write(unit, port, 0x82BC, 0xE5E0);
            _phy_826x_indirect_write(unit, port, 0x82BE, 0x01B2);
            _phy_826x_indirect_write(unit, port, 0x82C0, 0xFFA4);
            _phy_826x_indirect_write(unit, port, 0x82C2, 0x000D);
            _phy_826x_indirect_write(unit, port, 0x82CC, 0x050D);
            _phy_826x_indirect_write(unit, port, 0x82CE, 0xFF98);
            _phy_826x_indirect_write(unit, port, 0x82D0, 0x0010);
            _phy_826x_indirect_write(unit, port, 0x82D2, 0x0006);
            _phy_826x_indirect_write(unit, port, 0x82D4, 0x01E0);
            _phy_826x_indirect_write(unit, port, 0x82D6, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x82D8, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x82DA, 0x0000);
            _phy_826x_indirect_write(unit, port, 0x826D, 0x3805);
            _phy_826x_indirect_write(unit, port, 0x826E, 0x0310);
            _phy_826x_indirect_write(unit, port, 0x825F, 0x47E0);
            _phy_826x_indirect_write(unit, port, 0x8261, 0x47E0);
            _phy_826x_indirect_write(unit, port, 0x8266, 0x47E0);
            break;

        default:
            return RT_ERR_INPUT;
    }
    rtl826x_info[unit]->rtctCable[port] = cableType;
    return ret;
}



/* Function Name:
 *      phy_826x_rtct_start
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
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      RTCT is not supported when port link at 10M.
 */
int32
phy_826x_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rtk_enable_t ena;

    if ((ret = phy_common_c45_enable_get(unit, port, &ena)) != RT_ERR_OK)
        return ret;

    if (ena == DISABLED)
    {
        return RT_ERR_OPER_DENIED;
    }

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~BIT_15); //[15]=0
    phyData |= (BIT_1); //[1]=1
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
        return ret;

    phyData |= (BIT_4 | BIT_5 | BIT_6 | BIT_7); //[7:4]=0xf
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x30, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x2, 0, 0x3);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x30, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (BIT_9);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA400, phyData)) != RT_ERR_OK)
        return ret;

    osal_time_mdelay(1000);

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x30, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x3);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x30, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (BIT_0); //[0]=1
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      _phy_826x_rtctStatus_convert
 * Description:
 *      Convert RTCT result status register data to SDK status.
 * Input:
 *      phyData - result status data of RTCT register
 * Output:
 *      pShort - short
 *      pOpen   - open
 *      pMismatch -  mismatch
 *      pLinedriver -  line driver
 *      pHiImpedance - not support
 *      pCross -  not support
 *      pPartialCross - not support
 *      pPairBusy - not support
 * Return:
 *      RT_ERR_OK         - OK
 *      RT_ERR_FAILED     - invalid parameter
 *      RT_ERR_NOT_FINISH - RTCT is not complete in this channel
 * Note:
 *      None
 */
int32
_phy_826x_rtctStatus_convert(uint32 phyData,
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
 *      phy_826x_rtctResult_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The result unit is cm
 */
int32
phy_826x_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rtk_port_linkStatus_t link;
    rtk_port_speed_t speed;
    uint32  cable_factor = 79;

    switch(rtl826x_info[unit]->rtctCable[port])
    {
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A:
            cable_factor = 77;
            break;
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E:
        default:
            cable_factor = 79;
            break;
    }

    if ((ret = phy_common_c45_linkStatus_get(unit, port, &link)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_c45_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));


    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData & BIT_15) == 0)
        return RT_ERR_PHY_RTCT_NOT_FINISH;

    pRtctResult->linkType = PORT_SPEED_1000M;

    if ((ret = _phy_826x_indirect_read(unit, port, 0x8028, &phyData)) != RT_ERR_OK)
        return ret;
    pRtctResult->un.ge_result.channelALen = (phyData*100)/cable_factor;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x8028, phyData);
    if ((ret = _phy_826x_indirect_read(unit, port, 0x802C, &phyData)) != RT_ERR_OK)
        return ret;
    pRtctResult->un.ge_result.channelBLen = (phyData*100)/cable_factor;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x802C, phyData);
    if ((ret = _phy_826x_indirect_read(unit, port, 0x8030, &phyData)) != RT_ERR_OK)
        return ret;
    pRtctResult->un.ge_result.channelCLen = (phyData*100)/cable_factor;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x8030, phyData);
    if ((ret = _phy_826x_indirect_read(unit, port, 0x8034, &phyData)) != RT_ERR_OK)
        return ret;
    pRtctResult->un.ge_result.channelDLen = (phyData*100)/cable_factor;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x8034, phyData);

    if ((ret = _phy_826x_indirect_read(unit, port, 0x8026, &phyData)) != RT_ERR_OK)
        return ret;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x8026, phyData);
    if ((ret = _phy_826x_rtctStatus_convert(phyData,
                                            &pRtctResult->un.ge_result.channelAShort,        &pRtctResult->un.ge_result.channelAOpen,
                                            &pRtctResult->un.ge_result.channelAMismatch,     &pRtctResult->un.ge_result.channelAPairBusy)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_indirect_read(unit, port, 0x802A, &phyData)) != RT_ERR_OK)
        return ret;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x802A, phyData);
    if ((ret = _phy_826x_rtctStatus_convert(phyData,
                                            &pRtctResult->un.ge_result.channelBShort,        &pRtctResult->un.ge_result.channelBOpen,
                                            &pRtctResult->un.ge_result.channelBMismatch,     &pRtctResult->un.ge_result.channelBPairBusy)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_indirect_read(unit, port, 0x802E, &phyData)) != RT_ERR_OK)
        return ret;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x802E, phyData);
    if ((ret = _phy_826x_rtctStatus_convert(phyData,
                                            &pRtctResult->un.ge_result.channelCShort,        &pRtctResult->un.ge_result.channelCOpen,
                                            &pRtctResult->un.ge_result.channelCMismatch,     &pRtctResult->un.ge_result.channelCPairBusy)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_indirect_read(unit, port, 0x8032, &phyData)) != RT_ERR_OK)
        return ret;
    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "%s[0x%04X]: 0x%04X\n", __FUNCTION__, 0x802A, phyData);
    if ((ret = _phy_826x_rtctStatus_convert(phyData,
                                            &pRtctResult->un.ge_result.channelDShort,        &pRtctResult->un.ge_result.channelDOpen,
                                            &pRtctResult->un.ge_result.channelDMismatch,     &pRtctResult->un.ge_result.channelDPairBusy)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_826x_intrStatus_get
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
phy_826x_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus)
{
    int32 ret = 0;
    uint32 status = 0;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            if ((ret = _phy_826x_interrupt_status_get(unit, port, &status)) != RT_ERR_OK)
                return ret;

            pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
            pStatus->statusValue = status;
            break;
        case RTK_PHY_INTR_RLFD:
            /* 826X's RLFD status is in RTK_PHY_INTR_COMMON */
            return RT_ERR_NOT_SUPPORTED;
        default:
            return RT_ERR_NOT_SUPPORTED;
    }
    return ret;
}

/* Function Name:
 *      phy_826x_intrEnable_get
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
phy_826x_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32 bitmap = 0;

    if ((ret = _phy_826x_interrupt_enable_get(unit, port, &bitmap)) != RT_ERR_OK)
        return ret;

    *pEnable = (bitmap & (1 << phyIntr)) ? (ENABLED) : (DISABLED);
    return ret;
}

/* Function Name:
 *      phy_826x_intrEnable_set
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
phy_826x_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32 bitmap = 0;

    if ((ret = _phy_826x_interrupt_enable_get(unit, port, &bitmap)) != RT_ERR_OK)
        return ret;

    if (enable == DISABLED)
    {
        bitmap &= (~(1 << phyIntr));
    }
    else
    {
        bitmap |= (1 << phyIntr);
    }
    if ((ret = _phy_826x_interrupt_enable_set(unit, port, bitmap)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      phy_826x_intrMask_get
 * Description:
 *      Get PHY interrupt mask status.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt mask type
 * Output:
 *      pMask   - pointer to status of PHY interrupt mask
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
phy_826x_intrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask)
{
    int32 ret = 0;
    uint32 bitmap = 0;

    if ((ret = _phy_826x_interrupt_mask_get(unit, port, &bitmap)) != RT_ERR_OK)
        return ret;

    *pMask = (bitmap & (1 << phyIntr)) ? (1) : (0);
    return ret;
}

/* Function Name:
 *      phy_826x_intrMask_set
 * Description:
 *      Set PHY interrupt mask status.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      mask    - mask
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
phy_826x_intrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask)
{
    int32   ret = 0;
    uint32  bitmap = 0;

    if ((ret = _phy_826x_interrupt_mask_get(unit, port, &bitmap)) != RT_ERR_OK)
        return ret;

    if (mask == 0)
    {
        bitmap &= (~(1 << phyIntr));
    }
    else
    {
        bitmap |= (1 << phyIntr);
    }

    if ((ret = _phy_826x_interrupt_mask_set(unit, port, bitmap)) != RT_ERR_OK)
        return ret;
    return ret;
}

int32
_phy_826x_serdes_mode_usxgmii_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0xD, 0, 0x3FF);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_10gr_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC6, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1A, 0, 0x1F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC6, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_5gx_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC5, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x35, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC5, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_5gr_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC5, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1A, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC5, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x2, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_xfi_5g_adapt_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC5, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1A, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC5, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_xfi_5g_cpri_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC5, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1A, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC5, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_5g_off_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC5, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1F, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC5, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_2p5gx_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC4, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x36, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC4, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_xfi_2p5g_adapt_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC4, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1A, 0, 0x3F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC4, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 15, 0x8000);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x68, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_sgmii_set(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x105, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x1, 0, 0x1);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x105, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC3, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x2, 0, 0xF);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC3, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 5, 0x3E0);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC2, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A2, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0x0, 7, 0x80);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A2, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_set(uint32 unit, rtk_port_t port, uint32 mode)
{
    int32   ret = 0;

    switch (mode)
    {
        case RTK_PHY_CTRL_SERDES_MODE_USXGMII:
            if ((ret = _phy_826x_serdes_mode_usxgmii_set(unit, port)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SERDES_MODE_10GR_5GX_2P5GX_SGMII:
            if ((ret = _phy_826x_serdes_mode_10gr_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_5gx_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_2p5gx_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_sgmii_set(unit, port)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GADAPT_XFI2P5GADAPT_SGMII:
            if ((ret = _phy_826x_serdes_mode_10gr_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_xfi_5g_adapt_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_xfi_2p5g_adapt_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_sgmii_set(unit, port)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SERDES_MODE_10GR_5GR_2P5GX_SGMII:
            if ((ret = _phy_826x_serdes_mode_10gr_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_5g_off_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_2p5gx_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_sgmii_set(unit, port)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GCPRI_2P5GX_SGMII:
            if ((ret = _phy_826x_serdes_mode_10gr_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_5g_off_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_2p5gx_set(unit, port)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_mode_sgmii_set(unit, port)) != RT_ERR_OK)
                return ret;
            break;

        default:
            return RT_ERR_INPUT;
    }

    rtl826x_info[unit]->sdsModeCfg[port] = mode;
    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_get(uint32 unit, rtk_port_t port, uint32 *mode)
{
    if (rtl826x_info[unit])
        *mode = rtl826x_info[unit]->sdsModeCfg[port];
    else
        return RT_ERR_NOT_INIT;

    return RT_ERR_OK;
}

int32
_phy_826x_serdes_mode_update(uint32 unit, rtk_port_t port)
{
    int32  ret = 0;
    uint32 mode = RTK_PHY_CTRL_SERDES_MODE_UNKNOWN;
    rtk_port_linkStatus_t link;
    rtk_port_speed_t  speed;

    if ((ret = _phy_826x_serdes_mode_get(unit, port, &mode)) != RT_ERR_OK)
        return ret;

    if(RTK_PHY_CTRL_SERDES_MODE_IS_AUTO(mode))
        return RT_ERR_OK;

    if ((ret = _phy_826x_realTime_linkStatus_get(unit, port, &link)) != RT_ERR_OK)
        return ret;
    if (link == PORT_LINKDOWN)
        return RT_ERR_OK;

    if ((ret = phy_common_c45_speedStatusResReg_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    switch (mode)
    {
        case RTK_PHY_CTRL_SERDES_MODE_10GR_5GR_2P5GX_SGMII:
            switch (speed)
            {
                case PORT_SPEED_10G:
                    if ((ret = _phy_826x_serdes_mode_10gr_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                case PORT_SPEED_10G_LITE:
                case PORT_SPEED_5G:
                    if ((ret = _phy_826x_serdes_mode_5gr_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                case PORT_SPEED_5G_LITE:
                case PORT_SPEED_2_5G:
                    if ((ret = _phy_826x_serdes_mode_2p5gx_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                case PORT_SPEED_1000M:
                case PORT_SPEED_100M:
                    if ((ret = _phy_826x_serdes_mode_sgmii_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                default:
                    break;
            }
            break;

        case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GCPRI_2P5GX_SGMII:
            switch (speed)
            {
                case PORT_SPEED_10G:
                    if ((ret = _phy_826x_serdes_mode_10gr_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                case PORT_SPEED_10G_LITE:
                case PORT_SPEED_5G:
                    if ((ret = _phy_826x_serdes_mode_xfi_5g_cpri_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                case PORT_SPEED_5G_LITE:
                case PORT_SPEED_2_5G:
                    if ((ret = _phy_826x_serdes_mode_2p5gx_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                case PORT_SPEED_1000M:
                case PORT_SPEED_100M:
                    if ((ret = _phy_826x_serdes_mode_sgmii_set(unit, port)) != RT_ERR_OK)
                        return ret;
                    break;

                default:
                    break;
            }
            break;

        case RTK_PHY_CTRL_SERDES_MODE_UNKNOWN:
        default:
            return RT_ERR_NOT_INIT;
    }
    return RT_ERR_OK;
}

uint32
_phy_826x_debug_dump_addr_get(uint32 d1, uint32 d2)
{
    uint32 o1 = d1, o2 = 0;
    if (d2 >= 16 && d2 <=23)
    {
        o2 = ((d2 - 2*8)*2);
        if (d1 == 0)
        {
            o1 = 0xa42;
        }
    }
    else
    {
        if (d2 >= 0 && d2 <=7)
        {
            o2 = (d2 *2);
            o1 = 0xa40;
        }
        else if (d2 >= 8 && d2 <=15)
        {
            o2 = ((d2 - 1*8)*2);
            o1 = 0xa41;
        }
        else if (d2 >= 24 && d2 <=29)
        {
            o2 = ((d2 - 3*8)*2);
            o1 = 0xa43;
        }
    }
    return (o1 << 4) + o2;
}

uint32
_phy_826x_debug_dump_field_get(uint32 data, uint8 msb, uint8 lsb)
{
    uint32 mask = 0;
    uint8  i = 0;

    for (i = lsb; i <= msb; i++)
    {
        mask |= (1 << i);
    }

    return REG32_FIELD_GET(data, lsb, mask);
}

uint32
_phy_826x_debug_dump_field_set(uint32 data, uint8 msb, uint8 lsb, uint32 val)
{
    uint32 mask = 0;
    uint8  i = 0;

    for (i = lsb; i <= msb; i++)
    {
        mask |= (1U << i);
    }

    return REG32_FIELD_SET(data, val, lsb, mask);
}

int32
_phy_826x_debug_dump_mmd_set(uint32 unit, rtk_port_t port, uint32 mmd, uint32 addr, uint8 msb, uint8 lsb, uint32 val)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, mmd, addr, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = _phy_826x_debug_dump_field_set(phyData, msb, lsb, val);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, mmd, addr, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_debug_dump_mmd_get(uint32 unit, rtk_port_t port, uint32 mmd, uint32 addr, uint8 msb, uint8 lsb, uint32 *pData)
{
    int32   ret = 0;
    uint32  phyData = 0;

    *pData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, mmd, addr, &phyData)) != RT_ERR_OK)
        return ret;

    *pData = _phy_826x_debug_dump_field_get(phyData, msb, lsb);
    return RT_ERR_OK;
}

int32
_phy_826x_debug_dump_set(uint32 unit, rtk_port_t port, uint32 d1, uint32 d2, uint8 msb, uint8 lsb, uint32 data)
{
    int32  ret = 0;
    uint32 phyData = 0;
    uint32 addr = _phy_826x_debug_dump_addr_get(d1, d2);
    uint32 mmd = PHY_MMD_VEND2;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, mmd, addr, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = _phy_826x_debug_dump_field_set(phyData, msb, lsb, data);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, mmd, addr, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_debug_dump_get(uint32 unit, rtk_port_t port, uint32 d1, uint32 d2, uint8 msb, uint8 lsb, uint32 *pData)
{
    int32  ret = 0;
    uint32 phyData = 0;
    uint32 addr = _phy_826x_debug_dump_addr_get(d1, d2);
    uint32 mmd = PHY_MMD_VEND2;

    *pData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, mmd, addr, &phyData)) != RT_ERR_OK)
        return ret;
    *pData = _phy_826x_debug_dump_field_get(phyData, msb, lsb);
    return RT_ERR_OK;
}

int32
_phy_826x_debug_dump_u2sr_get(uint32 unit, rtk_port_t port, uint32 addr, uint32 *pData)
{
    int32  ret = 0;
    uint32 phyData = 0;
    if ((ret = _phy_826x_debug_dump_set(unit, port, 0xb87, 22, 15, 0, addr)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_826x_debug_dump_get(unit, port, 0xb87, 23, 15, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pData = _phy_826x_debug_dump_field_get(phyData, 15, 8);

    return RT_ERR_OK;
}


int32
_phy_826x_debug_dump_dsp_init(uint32 unit, rtk_port_t port, uint32 value)
{
    uint32  phyData = 0;
    uint32  item = RTK_PHY_CTRL_DEBUG_DUMP_IDX(value);

    switch (item)
    {
        case 7:
            _phy_826x_debug_dump_mmd_set(unit, port, 30, 76, 1, 1, 0);
            _phy_826x_debug_dump_set(unit, port, 0xa44, 17, 15, 15, 1);
            break;

        case 8:
            _phy_826x_debug_dump_get(unit, port, 0xA43, 1, 2, 2, &phyData);
            break;

        case 9:
            _phy_826x_debug_dump_get(unit, port, 0xce4, 17, 7, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xce4, 17, 7, 0, &phyData);
            break;

        case 18:
            _phy_826x_debug_dump_get(unit, port, 0xb03, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb0b, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb13, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb1b, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb03, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb0b, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb13, 21, 13, 0, &phyData);
            _phy_826x_debug_dump_get(unit, port, 0xb1b, 21, 13, 0, &phyData);
            break;

        case 21:
            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 9, 8, 1);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 15, 15, 0);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 15, 15, 1);
            break;

        default:
            return RT_ERR_OK;
    }
    return RT_ERR_OK;
}

int32
_phy_826x_debug_dump_s_sel_get(uint32 gv, uint32 sv)
{
    if (gv == 0)
    {
        return 0;
    }
    else if (gv == 1)
    {
        if (sv == 0)
            return 2;
        else if (sv == 1)
            return 0;
        else
            return 1;
    }
    else if (gv == 2)
    {
        if (sv == 0)
            return 1;
        else if (sv == 1)
            return 2;
        else
            return 0;
    }
    return 0;
}

int32
_phy_826x_debug_dump_dsp(uint32 unit, rtk_port_t port, uint32 value)
{
    uint32  tmp = 0;
    uint32  phyData = 0;
    uint32  outData = 0;
    uint32  ss = 0;
    uint8   retry = 0;

    uint8   *co = NULL;
    uint32  item = RTK_PHY_CTRL_DEBUG_DUMP_IDX(value);
    uint32  cnt = RTK_PHY_CTRL_DEBUG_DUMP_CNT(value);
    uint32  cc = 0;
    uint32  cs = 0;
    uint32  i = 0;
    uint32  j = 0;

    uint8 co35_2[] = { 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108 };
    uint8 co35_1[] = { 64, 68, 80, 84, 96, 100, 104, 108 };
    uint8 co35_0[] = { 64, 80, 96, 104 };
    uint8 co36_2[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44 };
    uint8 co36_1[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44 };
    uint8 co36_0[] = { 0, 8, 16, 24, 32, 40 };
    uint8 co37[] = { 48, 52, 56 };

    switch (item)
    {
        case 1:
            _phy_826x_debug_dump_get(unit, port, 0xae1, 18, 7, 0, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 2:
            _phy_826x_debug_dump_get(unit, port, 0xa60, 16, 7, 0, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 3:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 11, 11, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 4:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 2, 2, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 5:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 10, 9, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 6:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 5, 4, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 7:
            _phy_826x_debug_dump_get(unit, port, 0xa47, 21, 0, 0, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 8:
            _phy_826x_debug_dump_get(unit, port, 0xA43, 1, 2, 2, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 9:
            _phy_826x_debug_dump_get(unit, port, 0xce4, 17, 7, 0, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 10:
            _phy_826x_debug_dump_get(unit, port, 0xacb, 21, 9, 2, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 11:
            _phy_826x_debug_dump_get(unit, port, 0xbc6, 21, 2, 0, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 12:
            _phy_826x_debug_dump_get(unit, port, 0xa5d, 20, 15, 13, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 13:
            _phy_826x_debug_dump_get(unit, port, 0xa5d, 20, 12, 10, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 14:
            _phy_826x_debug_dump_mmd_get(unit, port, 30, 424, 15, 0, &phyData);
            outData = (phyData << 3);
            _phy_826x_debug_dump_mmd_get(unit, port, 30, 425, 2, 0, &phyData);
            outData = outData + phyData;
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 15:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb00, 22, 13, 9, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb08, 22, 13, 9, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb10, 22, 13, 9, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb18, 22, 13, 9, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 16:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb26, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb26, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb26, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb26, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);

            cc = 1; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb2E, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2E, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2E, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2E, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);

            cc = 2; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb36, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb36, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb36, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb36, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);

            cc = 3; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb3E, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3E, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3E, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3E, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            break;
        case 17:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xB20, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xB28, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xB30, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xB38, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 18:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb03, 21, 13, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb0b, 21, 13, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb13, 21, 13, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb1b, 21, 13, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 19:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb21, 16, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb21, 17, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb21, 18, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb21, 19, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);

            cc = 1; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb29, 16, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb29, 17, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb29, 18, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb29, 19, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);

            cc = 2; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb31, 16, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb31, 17, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb31, 18, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb31, 19, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);

            cc = 3; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb39, 16, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb39, 17, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb39, 18, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb39, 19, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            break;
        case 20:
            _phy_826x_debug_dump_set(unit, port, 0xd1a, 23, 3, 3, 0);
            _phy_826x_debug_dump_set(unit, port, 0xd1a, 23, 1, 0, 3);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 9, 8, 1);
            _phy_826x_debug_dump_set(unit, port, 0xd1a, 23, 3, 3, 1);
            while (1)
            {
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 23, 2, 2, &outData);
                if (outData == 1)
                {
                    break;
                }
                else
                {
                    retry++;
                    osal_time_mdelay(100);
                }
                if (retry == 10)
                {
                    break;
                }
            }
            if (retry < 30)
            {
                cc = 0; cs = 0;
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 19, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 20, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 21, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 22, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            }
            else
            {
                cc = 0; cs = 0;
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 19, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u(timeout)\n", unit, port, item, cnt, cc++, cs, i, outData);
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 20, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u(timeout)\n", unit, port, item, cnt, cc++, cs, i, outData);
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 21, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u(timeout)\n", unit, port, item, cnt, cc++, cs, i, outData);
                _phy_826x_debug_dump_get(unit, port, 0xd1a, 22, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u(timeout)\n", unit, port, item, cnt, cc++, cs, i, outData);
            }
            break;
        case 21:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_set(unit, port, 0xd05, 18, 10, 9, 0);
            _phy_826x_debug_dump_get(unit, port, 0xd06, 22, 11, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 18, 10, 9, 1);
            _phy_826x_debug_dump_get(unit, port, 0xd06, 22, 11, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 18, 10, 9, 2);
            _phy_826x_debug_dump_get(unit, port, 0xd06, 22, 11, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 18, 10, 9, 3);
            _phy_826x_debug_dump_get(unit, port, 0xd06, 22, 11, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);

            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 9, 8, 1);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 15, 15, 0);
            _phy_826x_debug_dump_set(unit, port, 0xd05, 17, 15, 15, 1);
            break;
        case 22:
            cc = 0; cs = 0;
            for (cc = 0; cc < 4; cc++)
            {
                _phy_826x_debug_dump_set(unit, port, 0xbd9, 17, 15, 0, 0x0051);
                _phy_826x_debug_dump_set(unit, port, 0xbda, 18, 15, 14, cc);
                _phy_826x_debug_dump_get(unit, port, 0xbd9, 20, 15, 7, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            }
            break;
        case 23:
            cc = 0; cs = 0;
            for (cc = 0; cc < 4; cc++)
            {
                _phy_826x_debug_dump_set(unit, port, 0xbd9, 17, 15, 0, 0x0052);
                _phy_826x_debug_dump_set(unit, port, 0xbda, 18, 15, 14, cc);
                _phy_826x_debug_dump_get(unit, port, 0xbd9, 20, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            }
            break;
        case 24:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb21, 20, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb21, 21, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb21, 22, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb21, 23, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 1; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb29, 20, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb29, 21, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb29, 22, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb29, 23, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 2; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb31, 20, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb31, 21, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb31, 22, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb31, 23, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 3; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb39, 20, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb39, 21, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb39, 22, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb39, 23, 15, 2, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            break;
        case 25:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb01, 23, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb09, 23, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb11, 23, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb19, 23, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 26:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb25, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb25, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb25, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb25, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 1; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb2d, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2d, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2d, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2d, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 2; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb35, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb35, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb35, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb35, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 3; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb3d, 18, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3d, 19, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3d, 20, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3d, 21, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            break;
        case 27:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb25, 22, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb25, 23, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb26, 16, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb26, 17, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 1; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb2d, 22, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2d, 23, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2e, 16, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2e, 17, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 2; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb35, 22, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb35, 23, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb36, 16, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb36, 17, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            cc = 3; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb3d, 22, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3d, 23, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3e, 16, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3e, 17, 15, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs++, i, outData);
            break;
        case 28:
            _phy_826x_debug_dump_get(unit, port, 0xaf0, 20, 15, 0, &phyData);
            outData = phyData;
            _phy_826x_debug_dump_get(unit, port, 0xaf0, 19, 15, 0, &phyData);
            outData = ((outData << 16) | (phyData & 0xFFFF));
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 29:
            _phy_826x_debug_dump_get(unit, port, 0xae0, 23, 3, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;

        case 30:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 10, 9, &tmp);
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 5, 4, &ss);
            ss = _phy_826x_debug_dump_s_sel_get(tmp, ss);

            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 13, 13, 0);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 12, 12, 1);

            for (cc = 0; cc <= 3; cc++)
            {
                for (i = 0; i <= 127; i++)
                {
                    _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((112 + cc) << 7) | i));
                    _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i + j, outData);
                }
            }
            if (ss == 2)
            {
                j = 128;
                for (cc = 0; cc <= 3; cc++)
                {
                    for (i = 0; i <= 127; i++)
                    {
                        _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((116 + cc) << 7) | i));
                        _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i + j, outData);
                    }
                }
            }
            break;
        case 31:
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 13, 13, 0);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 12, 12, 1);

            for (cc = 0; cc <= 3; cc++)
            {
                for (i = 0; i <= 15; i++)
                {
                    _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((128 + cc) << 7) | i));
                    _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
                }
            }
            break;
        case 32:
            for (cc = 0; cc <= 3; cc++)
            {
                for (i = 0; i <= 15; i++)
                {
                    _phy_826x_debug_dump_set(unit, port, 0xb44, 23, 15, 0, (BIT_7 | (i << 2) | cc));
                    _phy_826x_debug_dump_get(unit, port, 0xb45, 16, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, (outData >> 8));
                }
            }
            break;
        case 33:
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 13, 13, 0);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 12, 12, 1);

            for (cc = 0; cc <= 3; cc++)
            {
                _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((132 + cc) << 7) | BIT_1));
                _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);

            }
            break;
        case 34:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb20, 22, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb28, 22, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb30, 22, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb38, 22, 15, 1, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 35:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 10, 9, &tmp);
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 5, 4, &ss);
            ss = _phy_826x_debug_dump_s_sel_get(tmp, ss);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 13, 13, 0);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 12, 12, 1);

            switch (ss)
            {
                case 2:
                    co = co35_2;
                    tmp = 12;
                    break;
                case 1:
                    co = co35_1;
                    tmp = 8;
                    break;
                case 0:
                    co = co35_0;
                    tmp = 4;
                    break;
                default:
                    return RT_ERR_OK;
            }

            for (cc = 0; cc <= 3; cc++)
            {
                j = 0;
                for (cs = 0; cs < tmp; cs++)
                {
                    for (i = 0; i <= 127; i++)
                    {
                        _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((co[cs] + cc) << 7) | i));
                        _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, 0, i + j, outData);
                    }
                    j += i;
                }
            }
            break;
        case 36:
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 10, 9, &tmp);
            _phy_826x_debug_dump_get(unit, port, 0xa43, 26, 5, 4, &ss);
            ss = _phy_826x_debug_dump_s_sel_get(tmp, ss);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 13, 13, 0);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 12, 12, 1);
            switch (ss)
            {
                case 2:
                    co = co36_2;
                    tmp = 12;
                    break;
                case 1:
                    co = co36_1;
                    tmp = 12;
                    break;
                case 0:
                    co = co36_0;
                    tmp = 6;
                    break;
                default:
                    return RT_ERR_OK;
            }

            for (cc = 0; cc <= 3; cc++)
            {
                j = 0;
                for (cs = 0; cs < tmp; cs++)
                {
                    for (i = 0; i <= 127; i++)
                    {
                        _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((co[cs] + cc) << 7) | i));
                        _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, 0, i + j, outData);
                    }
                    j += i;
                }
            }
            break;
        case 37:
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 13, 13, 0);
            _phy_826x_debug_dump_set(unit, port, 0xac6, 22, 12, 12, 1);

            co = co37;
            tmp = 3;

            for (cc = 0; cc <= 3; cc++)
            {
                j = 0;
                for (cs = 0; cs < tmp; cs++)
                {
                    for (i = 0; i <= 127; i++)
                    {
                        _phy_826x_debug_dump_set(unit, port, 0xAC6, 20, 15, 0, (BIT_15 | ((co[cs] + cc) << 7) | i));
                        _phy_826x_debug_dump_get(unit, port, 0xAf1, 17, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, 0, i + j, outData);
                    }
                    j += i;
                }
            }
            break;
        case 38:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xbd6, 23, 2, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xbd6, 23, 5, 3, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xbd6, 23, 8, 6, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xbd6, 23, 11, 9, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 39:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A4F, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A50, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A51, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A52, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 40:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A47, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A48, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A49, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A4A, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A4B, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A4C, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A4D, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A4E, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 41:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A43, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A44, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A45, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A46, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 42:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A3B, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A3C, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A3D, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A3E, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A3F, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A40, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A41, &phyData); outData = (phyData << 8);
            _phy_826x_debug_dump_u2sr_get(unit, port, 0x8A42, &phyData); outData = (outData | phyData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 43:
            _phy_826x_debug_dump_get(unit, port, 0xad0, 21, 15, 2, &outData);
            DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc, cs, i, outData);
            break;
        case 44:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb20, 20, 8, 8, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb28, 20, 8, 8, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb30, 20, 8, 8, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb38, 20, 8, 8, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 45:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb22, 20, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2a, 20, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb32, 20, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3a, 20, 15, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 46:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb22, 22, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2a, 22, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb32, 22, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3a, 22, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        case 47:
            cc = 0; cs = 0;
            _phy_826x_debug_dump_get(unit, port, 0xb22, 23, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb2a, 23, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb32, 23, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            _phy_826x_debug_dump_get(unit, port, 0xb3a, 23, 14, 0, &outData); DEBUG_DSP_PRINT("%u,%u,%u,%u,%u,%u,%u,%u\n", unit, port, item, cnt, cc++, cs, i, outData);
            break;
        default:
            return RT_ERR_OK;
    }
    return RT_ERR_OK;
}

int32
_phy_826x_debug_coupling_dump(uint32 unit, rtk_port_t port, uint32 channel_bmp)
{
    uint32  tmp = 0;
    uint32  retry = 0;
    uint32  phyData = 0;
    uint32  outData = 0;
    uint32 cc = 0;
    _phy_826x_debug_dump_set(unit, port, 0xA5B, 18, 15, 15, 1);

    _phy_826x_debug_dump_set(unit, port, 0xA61, 16, 6, 6, 1);
    _phy_826x_debug_dump_set(unit, port, 0xA61, 16, 13, 13, 1);
    _phy_826x_debug_dump_set(unit, port, 0xA61, 16, 5, 2, 0);

    _phy_826x_debug_dump_set(unit, port, 0xA64, 16, 12, 12, 0);
    _phy_826x_debug_dump_set(unit, port, 0xA5D, 16, 3, 3, 0);
    _phy_826x_debug_dump_set(unit, port, 0xA5D, 18, 1, 1, 0);

    _phy_826x_debug_dump_set(unit, port, 0xACC, 21, 9, 0, 0x3FF);
    _phy_826x_debug_dump_set(unit, port, 0xA61, 16, 0, 0, 1);

    while (1)
    {
        _phy_826x_debug_dump_get(unit, port, 0xA40, 17, 2, 2, &outData);
        if (outData == 1)
        {
            break;
        }
        else
        {
            retry++;
            osal_time_mdelay(100);
        }
        if (retry > 10)
        {
            DEBUG_COUPLING_PRINT("Error: Link is down\n");
            break;
        }
    }

    _phy_826x_debug_dump_get(unit, port, 0xA43, 18, 5, 4, &outData);
    if (outData != 0x00)
    {
        tmp++;
    }
    _phy_826x_debug_dump_get(unit, port, 0xA43, 18, 10, 9, &phyData);
    if (phyData != 0x01)
    {
        tmp++;
    }
    if (tmp > 0)
    {
        DEBUG_COUPLING_PRINT("Error: Link speed incorrect(0x%X,0x%X)\n", outData, phyData);
        return RT_ERR_OK;
    }

    _phy_826x_debug_dump_set(unit, port, 0xd11, 22, 5, 3, 1);

    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 15, 15, 0);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 14, 14, 1);

    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 5, 5, 0);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 12, 12, 1);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 6, 5, 3);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 4, 3, 2);

    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 6, 5, 0);
    _phy_826x_debug_dump_set(unit, port, 0xd11, 19, 8, 8, 0);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 2, 0, 0);
    _phy_826x_debug_dump_set(unit, port, 0xd12, 19, 15, 0, 0);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 13, 13, 1);
    _phy_826x_debug_dump_set(unit, port, 0xd10, 18, 6, 5, 3);

    _phy_826x_debug_dump_set(unit, port, 0xbd2, 19, 4, 0, 15);
    _phy_826x_debug_dump_set(unit, port, 0xbd2, 19, 12, 8, 15);
    _phy_826x_debug_dump_set(unit, port, 0xbd2, 20, 4, 0, 15);
    _phy_826x_debug_dump_set(unit, port, 0xbd2, 20, 12, 8, 15);

    _phy_826x_debug_dump_set(unit, port, 0xbd2, 19, 7, 7, 1);
    _phy_826x_debug_dump_set(unit, port, 0xbd2, 19, 15, 15, 1);
    _phy_826x_debug_dump_set(unit, port, 0xbd2, 20, 7, 7, 1);
    _phy_826x_debug_dump_set(unit, port, 0xbd2, 20, 15, 15, 1);

    _phy_826x_debug_dump_set(unit, port, 0xBF0, 20, 4, 3, 0);

    DEBUG_COUPLING_PRINT("u,p,ch,ii,data,val\n");
    for (cc = 0; cc <= 3; cc++)
    {
        if (channel_bmp != 0 && (((1UL << cc) & channel_bmp) == 0))
        {
            continue;
        }
        _phy_826x_debug_dump_set(unit, port, 0xd11, 19, 11, 10, cc);

        _phy_826x_debug_dump_set(unit, port, 0xd11, 19, 8, 8, 1);
        _phy_826x_debug_dump_set(unit, port, 0xd11, 19, 8, 8, 0);

        for (tmp = 1 ; tmp <= 4096; tmp++)
        {
            _phy_826x_debug_dump_get(unit, port, 0xd11,20,15,0, &outData); DEBUG_COUPLING_PRINT("%u,%u,%u,%u,%u,0x%04X\n", unit, port, cc, tmp, 0, outData);
            _phy_826x_debug_dump_get(unit, port, 0xd11,21,15,0, &outData); DEBUG_COUPLING_PRINT("%u,%u,%u,%u,%u,0x%04X\n", unit, port, cc, tmp, 1, outData);
            _phy_826x_debug_dump_get(unit, port, 0xd15,20,15,0, &outData); DEBUG_COUPLING_PRINT("%u,%u,%u,%u,%u,0x%04X\n", unit, port, cc, tmp, 2, outData);
            _phy_826x_debug_dump_get(unit, port, 0xd15,21,15,0, &outData); DEBUG_COUPLING_PRINT("%u,%u,%u,%u,%u,0x%04X\n", unit, port, cc, tmp, 3, outData);
        }

    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_macIntfSerdesMode_get
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
phy_826x_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  sdsmode = 0;
    uint32  cprimode = 0;
    uint32  phyData = 0;
    rtk_port_linkStatus_t link = PORT_LINKDOWN;
    rtk_port_speed_t speed = PORT_SPEED_10G;

    if ((ret = _phy_826x_serdes_mode_get(unit, port, &sdsmode)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_realTime_linkStatus_get(unit, port, &link)) != RT_ERR_OK)
        return ret;

    if (PORT_LINKUP == link)
    {
        if ((ret = phy_common_c45_speedStatusResReg_get(unit, port, &speed)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u macIntfSerdesMode m:0x%04X, l:%u, s:%u\n", unit, port, sdsmode, link, speed);

    if ((ret = _phy_826x_serdes_reg_read(unit, port, 31, 22, &phyData)) != RT_ERR_OK)
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

        case 0b01101:
        {
            switch (speed)
            {
                case PORT_SPEED_1000M:
                    *pMode = RTK_MII_USXGMII_1G;
                    break;
                case PORT_SPEED_100M:
                    *pMode = RTK_MII_USXGMII_100M;
                    break;
                case PORT_SPEED_2_5G:
                case PORT_SPEED_5G_LITE:
                    *pMode = RTK_MII_USXGMII_2_5GSXGMII;
                    break;
                case PORT_SPEED_5G:
                case PORT_SPEED_10G_LITE:
                    *pMode = RTK_MII_USXGMII_5GSXGMII;
                    break;
                case PORT_SPEED_10G:
                default:
                    *pMode = RTK_MII_USXGMII_10GSXGMII;
                    break;
            }
        }
        break;

        case 0b10101:
            *pMode = RTK_MII_5GBASEX;
            break;

        case 0b10110:
            *pMode = RTK_MII_2500Base_X;
            break;

        case 0b11010:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 31, 19, &phyData)) != RT_ERR_OK)
                return ret;
            cprimode = (phyData & 0x1F);

            if (cprimode == 0x2)
            {
                *pMode = RTK_MII_5GR;
                break;
            }
            else if (cprimode == 0x1)
            {
                *pMode = RTK_MII_XFI_5G_CPRI;
                break;
            }

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x68, &phyData)) != RT_ERR_OK)
                return ret;
            if (phyData & BIT_15)
            {
                switch (speed)
                {
                    case PORT_SPEED_2_5G:
                    case PORT_SPEED_5G_LITE:
                        *pMode = RTK_MII_XFI_2P5G_ADAPT;
                        break;
                    case PORT_SPEED_5G:
                    case PORT_SPEED_10G_LITE:
                        *pMode = RTK_MII_XFI_5G_ADAPT;
                        break;
                    default:
                        *pMode = RTK_MII_10GR;
                        break;
                }
            }
            else
            {
                *pMode = RTK_MII_10GR;
            }

            break;

        default:
            *pMode = RTK_MII_DISABLE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_macIntfSerdesLinkStatus_get
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
phy_826x_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rt_serdesMode_t mode = 0;

    if ((ret = phy_826x_macIntfSerdesMode_get(unit, port, &mode)) != RT_ERR_OK)
        return ret;

    pStatus->sds_num=1;

    switch (mode)
    {
        case RTK_MII_5GBASEX:
        case RTK_MII_2500Base_X:
        case RTK_MII_SGMII:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 0x1, 29, &phyData)) != RT_ERR_OK)
                return ret;

            pStatus->link_status[0] = ((phyData & 0x111) == 0x111) ? (PORT_LINKUP) : (PORT_LINKDOWN);
            break;

        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_5GSXGMII:
        case RTK_MII_USXGMII_2_5GSXGMII:
        case RTK_MII_USXGMII_1G:
        case RTK_MII_USXGMII_100M:
        case RTK_MII_10GR:
        case RTK_MII_XFI_5G_ADAPT:
        case RTK_MII_XFI_5G_CPRI:
        case RTK_MII_XFI_2P5G_ADAPT:
        case RTK_MII_5GR:
        default:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 0x5, 0, &phyData)) != RT_ERR_OK)
                return ret;
            pStatus->link_status[0] = (phyData & BIT_12) ? (PORT_LINKUP) : (PORT_LINKDOWN);
            break;
    }

    return ret;
}

/* Function Name:
 *      phy_826x_speed_set
 * Description:
 *      Set speed of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      set speed to PORT_SPEED_5G or PORT_SPEED_2_5G will apply the Ultra-Link mode advertisement 5G or 2.5G at the same time.
 */
int32
phy_826x_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret = RT_ERR_OK;
    uint32  remote_loopback = 0;
    rt_port_ethType_t eth_type = _phy_826x_chip_type_get(unit,port);

    if((eth_type == HWP_2_5GE) && ((speed == PORT_SPEED_5G) || (speed == PORT_SPEED_10G)))
    {
        return RT_ERR_INPUT;
    }
    else if(eth_type == HWP_5GE && (speed == PORT_SPEED_10G))
    {
        return RT_ERR_INPUT;
    }

    if ((ret = phy_826x_ctrl_get(unit, port, RTK_PHY_CTRL_LOOPBACK_REMOTE, &remote_loopback)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_c45_speed_set(unit, port, speed)) != RT_ERR_OK)
        return ret;

    _phy_826x_an_restart(unit, port);

    if (remote_loopback != 0)
    {
        if ((ret = phy_826x_ctrl_set(unit, port, RTK_PHY_CTRL_LOOPBACK_REMOTE, remote_loopback)) != RT_ERR_OK)
        return ret;
    }

    return ret;
}

int32
_phy_826x_PRBS_TGR_set(uint32 unit, rtk_port_t port, rtk_sds_testMode_t testMode)
{
    int32 ret = RT_ERR_OK;
    uint32 phyData = 0;

    switch (testMode)
    {
        case RTK_SDS_TESTMODE_DISABLE:
            phyData = 0;
            break;
        case RTK_SDS_TESTMODE_PRBS7:
            phyData = 0x3000;
            break;
        case RTK_SDS_TESTMODE_PRBS15:
            phyData = 0x300;
            break;
        case RTK_SDS_TESTMODE_PRBS23:
            phyData = 0xC00;
            break;
        case RTK_SDS_TESTMODE_PRBS31:
            phyData = 0x30;
            break;
        default:
            return RT_ERR_INPUT;
    }
    if ((ret = _phy_826x_serdes_reg_write(unit, port, 5, 10, phyData)) != RT_ERR_OK)
        return ret;

    if (testMode == RTK_SDS_TESTMODE_DISABLE)
    {
        if ((ret = _phy_826x_serdes_reg_write(unit, port, 6, 0, 0x1F00)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = _phy_826x_serdes_reg_write(unit, port, 6, 0, 0x7F00)) != RT_ERR_OK)
            return ret;
    }
    if ((ret = _phy_826x_serdes_reg_write(unit, port, 6, 14, 0x01FF)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_826x_serdes_reg_write(unit, port, 6, 15, 0x01FF)) != RT_ERR_OK)
        return ret;
    return ret;
}

int32
_phy_826x_PRBS_set(uint32 unit, rtk_port_t port, rtk_sds_testMode_t testMode)
{
    int32 ret = RT_ERR_OK;
    uint32 phyData = 0;
    uint32 prbsSel = 0;
    rtk_enable_t enable = DISABLED;

    switch (testMode)
    {
        case RTK_SDS_TESTMODE_DISABLE:
            enable = DISABLED;
            break;
        case RTK_SDS_TESTMODE_PRBS7:
            prbsSel = 0x0;
            enable = ENABLED;
            break;
        case RTK_SDS_TESTMODE_PRBS9:
            prbsSel = 0x1;
            enable = ENABLED;
            break;
        case RTK_SDS_TESTMODE_PRBS11:
            prbsSel = 0x2;
            enable = ENABLED;
            break;
        case RTK_SDS_TESTMODE_PRBS15:
            prbsSel = 0x3;
            enable = ENABLED;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = _phy_826x_serdes_reg_read(unit, port, 1, 11, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, prbsSel, 8, (0x3 << 8));
    if ((ret = _phy_826x_serdes_reg_write(unit, port, 1, 11, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_serdes_reg_read(unit, port, 1, 12, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~BIT_11);
    phyData |= (enable == ENABLED)? (BIT_11) : (0);
    if ((ret = _phy_826x_serdes_reg_write(unit, port, 1, 12, phyData)) != RT_ERR_OK)
        return ret;
    return ret;
}

/* Function Name:
 *      phy_826x_sdsTestMode_set
 * Description:
 *      Set SerDes PRBS test mode.
 * Input:
 *      unit        - unit id
 *      port        - base mac ID number of the PHY
 *      sdsId       - SerDes id
 *      testMode    - test mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 * Note:
 *      None
 */
int32
phy_826x_sdsTestMode_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_testMode_t testMode)
{
    int32 ret = RT_ERR_OK;
    rt_serdesMode_t sdsMode;

    if ((ret = phy_826x_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (sdsId != 0)
    {
        return RT_ERR_INPUT;
    }

    switch (sdsMode)
    {
        case RTK_MII_5GBASEX:
        case RTK_MII_2500Base_X:
        case RTK_MII_SGMII:
            ret = _phy_826x_PRBS_set(unit, port, testMode);
            break;

        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_5GSXGMII:
        case RTK_MII_USXGMII_2_5GSXGMII:
        case RTK_MII_USXGMII_1G:
        case RTK_MII_USXGMII_100M:
        case RTK_MII_10GR:
        case RTK_MII_XFI_5G_ADAPT:
        case RTK_MII_XFI_5G_CPRI:
        case RTK_MII_XFI_2P5G_ADAPT:
        case RTK_MII_5GR:
        default:
            ret = _phy_826x_PRBS_TGR_set(unit, port, testMode);
            break;
    }
    return ret;
}

/* Function Name:
 *      phy_826x_sdsTestModeCnt_get
 * Description:
 *      Get SerDes PRBS test mode test pattern error counter.
 * Input:
 *      unit        - unit id
 *      port        - base mac ID number of the PHY
 *      sdsId       - SerDes id
 * Output:
 *      pCnt        - test pattern error counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 * Note:
 *      None
 */
int32
phy_826x_sdsTestModeCnt_get(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 *pCnt)
{
    int32 ret = RT_ERR_OK;
    rt_serdesMode_t sdsMode;
    uint32 phyData = 0;
    uint32 cnt = 0;
    if ((ret = phy_826x_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (sdsId != 0)
    {
        return RT_ERR_INPUT;
    }

    switch (sdsMode)
    {
        case RTK_MII_5GBASEX:
        case RTK_MII_2500Base_X:
        case RTK_MII_SGMII:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 1, 5, &phyData)) != RT_ERR_OK)
                return ret;
            cnt = REG32_FIELD_SET(cnt, phyData, 16, (0xFFFF << 16));
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 1, 4, &phyData)) != RT_ERR_OK)
                return ret;
            cnt = REG32_FIELD_SET(cnt, phyData, 0, (0xFFFF << 0));

            //clear counter
            if ((ret = _phy_826x_serdes_reg_write(unit, port, 1, 5, 0)) != RT_ERR_OK)
                return ret;
            if ((ret = _phy_826x_serdes_reg_write(unit, port, 1, 4, 0)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_5GSXGMII:
        case RTK_MII_USXGMII_2_5GSXGMII:
        case RTK_MII_USXGMII_1G:
        case RTK_MII_USXGMII_100M:
        case RTK_MII_10GR:
        case RTK_MII_XFI_5G_ADAPT:
        case RTK_MII_XFI_5G_CPRI:
        case RTK_MII_XFI_2P5G_ADAPT:
        case RTK_MII_5GR:
        default:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 5, 11, &phyData)) != RT_ERR_OK)
                return ret;
            cnt = phyData;
            break;
    }
    *pCnt = cnt;
    return ret;
}

int32
_phy_826x_preemphasis_set(uint32 unit, rtk_port_t port, uint32 param)
{
    int32   ret = 0;
    uint32  phyDataH = 0, phyDataL = 0;
    uint32  paramH = 0, paramL = 0;
    uint32  eye_page;

    /* preemphasis value:
       [31:26]:post1 amp;     [25:20]:post2 amp;
       [19]:post1 amp enable; [18]:post2 amp enable;
       [15:10]:main amp;      [9]: main amp enable;
       [8:3]:pre amp;         [2]: pre amp enable */
    paramH = (param >> 16) & 0xFFFC;
    paramL = param & 0xFFFC;

    switch (rtl826x_info[unit]->eyeParamTarget[port])
    {
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GR:
            eye_page = PHY_826X_EYE_PAGE_BASE_5GR;
            break;
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GX:
            eye_page = PHY_826X_EYE_PAGE_BASE_5GX;
            break;
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_2P5GX:
            eye_page = PHY_826X_EYE_PAGE_BASE_2P5GX;
            break;
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_1GX:
            eye_page = PHY_826X_EYE_PAGE_BASE_1G;
            break;
        default:
            eye_page = PHY_826X_EYE_PAGE_BASE;
            break;
    }


    if ((ret = _phy_826x_serdes_reg_read(unit, port, eye_page, 7, &phyDataL)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_826x_serdes_reg_read(unit, port, eye_page, 6, &phyDataH)) != RT_ERR_OK)
        return ret;

    phyDataH = (phyDataH & 0x3) |  paramH;
    phyDataL = (phyDataL & 0x3) |  paramL;

    if ((ret = _phy_826x_serdes_reg_write(unit, port, eye_page, 7, phyDataL)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_826x_serdes_reg_write(unit, port, eye_page, 6, phyDataH)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_preemphasis_get(uint32 unit, rtk_port_t port, uint32 *pParam)
{
    int32   ret = 0;
    uint32  phyDataH = 0, phyDataL = 0;
    uint32          eye_page;

    switch (rtl826x_info[unit]->eyeParamTarget[port])
    {
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GR:
            eye_page = PHY_826X_EYE_PAGE_BASE_5GR;
            break;
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GX:
            eye_page = PHY_826X_EYE_PAGE_BASE_5GX;
            break;
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_2P5GX:
            eye_page = PHY_826X_EYE_PAGE_BASE_2P5GX;
            break;
        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_1GX:
            eye_page = PHY_826X_EYE_PAGE_BASE_1G;
            break;
        default:
            eye_page = PHY_826X_EYE_PAGE_BASE;
            break;
    }

    /* preemphasis value:
       [31:26]:post1 amp;     [25:20]:post2 amp;
       [19]:post1 amp enable; [18]:post2 amp enable;
       [15:10]:main amp;      [9]: main amp enable;
       [8:3]:pre amp;         [2]: pre amp enable */

    if ((ret = _phy_826x_serdes_reg_read(unit, port, eye_page, 7, &phyDataL)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_826x_serdes_reg_read(unit, port, eye_page, 6, &phyDataH)) != RT_ERR_OK)
        return ret;

    *pParam = ((phyDataH & 0xFFFC) << 16) | (phyDataL & 0xFFFC);
    return ret;
}

int32
_phy_826x_led_mode_set(uint32 unit, rtk_port_t port, uint32 index, uint32 mode)
{
    int32   ret = 0;
    uint32  phyDataL = 0;
    uint32  phyDataH = 0;

    if (index > 5)
        return RT_ERR_INPUT;

    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_10G            ) ? (PHY_826X_LED_MODE_L_10G            ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_10G_LITE       ) ? (PHY_826X_LED_MODE_L_10G_LITE       ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_5G             ) ? (PHY_826X_LED_MODE_L_5G             ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_5G_LITE        ) ? (PHY_826X_LED_MODE_L_5G_LITE        ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G           ) ? (PHY_826X_LED_MODE_L_2P5G           ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_LITE      ) ? (PHY_826X_LED_MODE_L_2P5G_LITE      ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_1G             ) ? (PHY_826X_LED_MODE_L_1G             ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_500M           ) ? (PHY_826X_LED_MODE_L_500M           ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_100M           ) ? (PHY_826X_LED_MODE_L_100M           ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_10G_FLASH      ) ? (PHY_826X_LED_MODE_L_10G_FLASH      ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_10G_LITE_FLASH ) ? (PHY_826X_LED_MODE_L_10G_LITE_FLASH ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_5G_FLASH       ) ? (PHY_826X_LED_MODE_L_5G_FLASH       ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_5G_LITE_FLASH  ) ? (PHY_826X_LED_MODE_L_5G_LITE_FLASH  ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_FLASH     ) ? (PHY_826X_LED_MODE_L_2P5G_FLASH     ) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH) ? (PHY_826X_LED_MODE_L_2P5G_LITE_FLASH) : (0);
    phyDataL |= (mode & RTK_PHY_CTRL_LED_MODE_1G_FLASH       ) ? (PHY_826X_LED_MODE_L_1G_FLASH       ) : (0);

    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_500M_FLASH     ) ? (PHY_826X_LED_MODE_H_500M_FLASH     ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_100M_FLASH     ) ? (PHY_826X_LED_MODE_H_100M_FLASH     ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_RX_ACT         ) ? (PHY_826X_LED_MODE_H_RX_ACT         ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_TX_ACT         ) ? (PHY_826X_LED_MODE_H_TX_ACT         ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_LITE_FLASH     ) ? (PHY_826X_LED_MODE_H_LITE_FLASH     ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_LITE           ) ? (PHY_826X_LED_MODE_H_LITE           ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_DUPLEX         ) ? (PHY_826X_LED_MODE_H_DUPLEX         ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_MASTER         ) ? (PHY_826X_LED_MODE_H_MASTER         ) : (0);
    phyDataH |= (mode & RTK_PHY_CTRL_LED_MODE_TRAINING       ) ? (PHY_826X_LED_MODE_H_TRAINING       ) : (0);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, (21 + index * 2), phyDataL)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, (22 + index * 2), phyDataH)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_led_mode_get(uint32 unit, rtk_port_t port, uint32 index, uint32 *pMode)
{
    int32   ret = 0;
    uint32  phyDataL = 0;
    uint32  phyDataH = 0;
    uint32  tmp = 0;

    if (index > 5)
        return RT_ERR_INPUT;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, (21 + index * 2), &phyDataL)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, (22 + index * 2), &phyDataH)) != RT_ERR_OK)
        return ret;

    tmp |= (phyDataL & PHY_826X_LED_MODE_L_10G            ) ? (RTK_PHY_CTRL_LED_MODE_10G            ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_10G_LITE       ) ? (RTK_PHY_CTRL_LED_MODE_10G_LITE       ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_5G             ) ? (RTK_PHY_CTRL_LED_MODE_5G             ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_5G_LITE        ) ? (RTK_PHY_CTRL_LED_MODE_5G_LITE        ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_2P5G           ) ? (RTK_PHY_CTRL_LED_MODE_2P5G           ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_2P5G_LITE      ) ? (RTK_PHY_CTRL_LED_MODE_2P5G_LITE      ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_1G             ) ? (RTK_PHY_CTRL_LED_MODE_1G             ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_500M           ) ? (RTK_PHY_CTRL_LED_MODE_500M           ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_100M           ) ? (RTK_PHY_CTRL_LED_MODE_100M           ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_10G_FLASH      ) ? (RTK_PHY_CTRL_LED_MODE_10G_FLASH      ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_10G_LITE_FLASH ) ? (RTK_PHY_CTRL_LED_MODE_10G_LITE_FLASH ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_5G_FLASH       ) ? (RTK_PHY_CTRL_LED_MODE_5G_FLASH       ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_5G_LITE_FLASH  ) ? (RTK_PHY_CTRL_LED_MODE_5G_LITE_FLASH  ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_2P5G_FLASH     ) ? (RTK_PHY_CTRL_LED_MODE_2P5G_FLASH     ) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_2P5G_LITE_FLASH) ? (RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH) : (0);
    tmp |= (phyDataL & PHY_826X_LED_MODE_L_1G_FLASH       ) ? (RTK_PHY_CTRL_LED_MODE_1G_FLASH       ) : (0);

    tmp |= (phyDataH & PHY_826X_LED_MODE_H_500M_FLASH     ) ? (RTK_PHY_CTRL_LED_MODE_500M_FLASH     ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_100M_FLASH     ) ? (RTK_PHY_CTRL_LED_MODE_100M_FLASH     ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_RX_ACT         ) ? (RTK_PHY_CTRL_LED_MODE_RX_ACT         ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_TX_ACT         ) ? (RTK_PHY_CTRL_LED_MODE_TX_ACT         ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_LITE_FLASH     ) ? (RTK_PHY_CTRL_LED_MODE_LITE_FLASH     ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_LITE           ) ? (RTK_PHY_CTRL_LED_MODE_LITE           ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_DUPLEX         ) ? (RTK_PHY_CTRL_LED_MODE_DUPLEX         ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_MASTER         ) ? (RTK_PHY_CTRL_LED_MODE_MASTER         ) : (0);
    tmp |= (phyDataH & PHY_826X_LED_MODE_H_TRAINING       ) ? (RTK_PHY_CTRL_LED_MODE_TRAINING       ) : (0);

    *pMode = tmp;
    return ret;
}

int32
_phy_826x_led_config_flash_rate_set(uint32 unit, rtk_port_t port, uint32 value)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 17, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(BIT_4 | BIT_3);
    switch (value)
    {
        case RTK_PHY_CTRL_LED_CFG_FLASH_RATE_128MS:
            break;
        case RTK_PHY_CTRL_LED_CFG_FLASH_RATE_256MS:
            phyData |= (BIT_3);
            break;
        case RTK_PHY_CTRL_LED_CFG_FLASH_RATE_512MS:
            phyData |= (BIT_4);
            break;
        case RTK_PHY_CTRL_LED_CFG_FLASH_RATE_1024MS:
            phyData |= (BIT_4 | BIT_3);
            break;
        default:
            return RT_ERR_INPUT;
    }
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 17, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_led_config_flash_rate_get(uint32 unit, rtk_port_t port, uint32 *pValue)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 17, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & (BIT_4))
    {
        if (phyData & (BIT_3))
            *pValue = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_1024MS;
        else
            *pValue = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_512MS;
    }
    else
    {
        if (phyData & (BIT_3))
            *pValue = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_256MS;
        else
            *pValue = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_128MS;
    }
    return ret;
}

int32
_phy_826x_led_config_blink_rate_set(uint32 unit, rtk_port_t port, uint32 value)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 17, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(BIT_2 | BIT_1 | BIT_0);
    switch (value)
    {
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS:
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS:
            phyData |= (BIT_0);
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS:
            phyData |= (BIT_1);
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS:
            phyData |= (BIT_0 | BIT_1);
            break;
        default:
            return RT_ERR_INPUT;
    }
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 17, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_led_config_blink_rate_get(uint32 unit, rtk_port_t port, uint32 *pValue)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 17, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & (BIT_1))
    {
        if (phyData & (BIT_0))
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS;
        else
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS;
    }
    else
    {
        if (phyData & (BIT_0))
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS;
        else
            *pValue = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS;
    }
    return ret;
}

int32
_phy_826x_led_config_active_low_set(uint32 unit, rtk_port_t port, uint32 index, uint32 value)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if (index > 5)
        return RT_ERR_INPUT;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 18, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_8 << index));
    phyData |= (value == 0) ? (0) : (BIT_8 << index);

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 18, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_led_config_active_low_get(uint32 unit, rtk_port_t port, uint32 index, uint32 *pValue)
{
    int32   ret = 0;
    uint32  phyData = 0;

    if (index > 5)
        return RT_ERR_INPUT;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 18, &phyData)) != RT_ERR_OK)
        return ret;

    *pValue = (phyData & (BIT_8 << index))  ? (1) : (0);

    return ret;
}

int32
_phy_826x_led_config_force_set(uint32 unit, rtk_port_t port, uint32 index, uint32 value)
{
    int32  ret = 0;
    uint32 phyData0 = 0, phyData1 = 0;

    if (index > 5)
        return RT_ERR_INPUT;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 19, &phyData0)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 20, &phyData1)) != RT_ERR_OK)
        return ret;

    /* change data source to "UTP" for RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE; "cpu force" for other options */
    phyData0 &= (~(0x3 << (index * 2)));
    phyData0 |= (RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE == value) ? (0) : (0x2 << (index * 2));

    phyData1 &= (~(0x3 << (index * 2)));
    switch (value)
    {
        case RTK_PHY_CTRL_LED_CFG_FORCE_OFF:
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_ON:
            phyData1 |= (0x1 << (index * 2));
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_BLINK:
            phyData1 |= (0x2 << (index * 2));
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_FLASH:
            phyData1 |= (0x3 << (index * 2));
            break;
        case RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE:
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 19, phyData0)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 20, phyData1)) != RT_ERR_OK)
        return ret;

    return ret;
}

int32
_phy_826x_led_config_force_get(uint32 unit, rtk_port_t port, uint32 index, uint32 *pValue)
{
    int32  ret = 0;
    uint32 phyData0 = 0, phyData1 = 0;
    uint32 mode = 0;

    if (index > 5)
        return RT_ERR_INPUT;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 19, &phyData0)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 20, &phyData1)) != RT_ERR_OK)
        return ret;

    if ((phyData0 & (0x3 << (index * 2))) == 0)
    {
        *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE;
        return ret;
    }

    mode = ((phyData1 & (0x3 << (index * 2))) >> (index * 2));

    switch (mode)
    {
        case 0x0:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_OFF;
            break;
        case 0x1:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_ON;
            break;
        case 0x2:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_BLINK;
            break;
        case 0x3:
            *pValue = RTK_PHY_CTRL_LED_CFG_FORCE_FLASH;
            break;
        default:
            return RT_ERR_FAILED;
    }
    return ret;
}

int32
_phy_826x_loopback_remote_set(uint32 unit, rtk_port_t port, uint32 ena)
{
    int32   ret = 0;
    uint32  phyData = 0;
    rtk_port_speed_t speed;

    if ((ret =  phy_common_c45_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xBCE8, 0x0040)) != RT_ERR_OK)
                return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC800, 0x5A02)) != RT_ERR_OK)
                return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC416, 0x0550)) != RT_ERR_OK)
                return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC464, 0x0001)) != RT_ERR_OK)
                return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC45E, 0x2C84)) != RT_ERR_OK)
                return ret;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC45C, 0xAC61)) != RT_ERR_OK)
                return ret;

    if (ena != 0)
    {
        switch (speed)
        {
            case PORT_SPEED_100M:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xBCE8, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= REG32_FIELD_SET(phyData, 1, 13, 0x2000);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xBCE8, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC800, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData = REG32_FIELD_SET(phyData, 1, 4, 0x0030);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC800, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC416, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData = REG32_FIELD_SET(phyData, 3, 2, 0x000C);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC416, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            case PORT_SPEED_1000M:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC800, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData = REG32_FIELD_SET(phyData, 1, 4, 0x0030);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC800, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC416, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData = REG32_FIELD_SET(phyData, 3, 2, 0x000C);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC416, phyData)) != RT_ERR_OK)
                    return ret;
                break;

            default:
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC464, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC464, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC464, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC464, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC464, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_0);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC464, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC45E, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_15);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC45E, phyData)) != RT_ERR_OK)
                    return ret;
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC45C, &phyData)) != RT_ERR_OK)
                    return ret;
                 phyData &= (~BIT_13);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC45C, phyData)) != RT_ERR_OK)
                    return ret;
                break;
        }

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A1, &phyData)) != RT_ERR_OK)
            return ret;
        phyData |= (BIT_10);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A1, phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x291, &phyData)) != RT_ERR_OK)
            return ret;
        phyData |= (BIT_5);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x291, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A1, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~BIT_10);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x2A1, phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x291, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= (~BIT_5);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x291, phyData)) != RT_ERR_OK)
            return ret;
    }
    return ret;
}

int32
_phy_826x_serdes_loopback_set(uint32 unit, rtk_port_t port, uint32 value)
{
    int32   ret = 0;
    uint32  phyData = 0;
    rt_serdesMode_t mode;

    if ((ret = phy_826x_macIntfSerdesMode_get(unit, port, &mode)) != RT_ERR_OK)
        return ret;

    switch (mode)
    {
        case RTK_MII_5GBASEX:
        case RTK_MII_2500Base_X:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 2, 0, &phyData)) != RT_ERR_OK)
                return ret;
            if (value == 0)
            {
                phyData &= (~BIT_14);
            }
            else
            {
                phyData |= (BIT_14);
            }

            if ((ret = _phy_826x_serdes_reg_write(unit, port, 2, 0, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_MII_SGMII:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 0, 0, &phyData)) != RT_ERR_OK)
                return ret;
            if (value == 0)
            {
                phyData &= (~BIT_4);
            }
            else
            {
                phyData |= (BIT_4);
            }

            if ((ret = _phy_826x_serdes_reg_write(unit, port, 0, 0, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_5GSXGMII:
        case RTK_MII_USXGMII_2_5GSXGMII:
        case RTK_MII_USXGMII_1G:
        case RTK_MII_USXGMII_100M:
        case RTK_MII_10GR:
        case RTK_MII_XFI_5G_ADAPT:
        case RTK_MII_XFI_5G_CPRI:
        case RTK_MII_XFI_2P5G_ADAPT:
        case RTK_MII_5GR:
        default:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 4, 0, &phyData)) != RT_ERR_OK)
                return ret;
            if (value == 0)
            {
                phyData &= (~BIT_14);
            }
            else
            {
                phyData |= (BIT_14);
            }

            if ((ret = _phy_826x_serdes_reg_write(unit, port, 4, 0, phyData)) != RT_ERR_OK)
                return ret;
            break;
    }
    return ret;
}

int32
_phy_826x_serdes_loopback_get(uint32 unit, rtk_port_t port, uint32* pValue)
{
    int32   ret = 0;
    uint32  phyData = 0;
    rt_serdesMode_t mode;

    if ((ret = phy_826x_macIntfSerdesMode_get(unit, port, &mode)) != RT_ERR_OK)
        return ret;

    switch (mode)
    {
        case RTK_MII_5GBASEX:
        case RTK_MII_2500Base_X:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 2, 0, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_14) ? 1 : 0;
            break;

        case RTK_MII_SGMII:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 0, 0, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_4) ? 1 : 0;
            break;

        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_5GSXGMII:
        case RTK_MII_USXGMII_2_5GSXGMII:
        case RTK_MII_USXGMII_1G:
        case RTK_MII_USXGMII_100M:
        case RTK_MII_10GR:
        case RTK_MII_XFI_5G_ADAPT:
        case RTK_MII_XFI_5G_CPRI:
        case RTK_MII_XFI_2P5G_ADAPT:
        case RTK_MII_5GR:
        default:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 4, 0, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_14) ? 1 : 0;
            break;
    }
    return ret;
}


int32
_phy_826x_reinit(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint8   port_offset = 0;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x145, 0x0001)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x145, 0x0000)) != RT_ERR_OK)
        return ret;

    osal_time_mdelay(100);

    if ((ret = hwp_get_offset_by_baseport_port(unit, HWP_PHY_BASE_MACID(unit, port), port, &port_offset)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_rtl826x_patch(unit, port, port_offset)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_826x_init(unit, port)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_ctrl_set
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
phy_826x_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    int32   ret = 0;
    uint32  phyData = 0;
    uint32  tmp = 0;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_LED_1_MODE:
        case RTK_PHY_CTRL_LED_2_MODE:
        case RTK_PHY_CTRL_LED_3_MODE:
        case RTK_PHY_CTRL_LED_4_MODE:
        case RTK_PHY_CTRL_LED_5_MODE:
        case RTK_PHY_CTRL_LED_6_MODE:
            ret = _phy_826x_led_mode_set(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_MODE), value);
            break;
        case RTK_PHY_CTRL_LED_CFG_FLASH_RATE:
            ret = _phy_826x_led_config_flash_rate_set(unit, port, value);
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE:
            ret = _phy_826x_led_config_blink_rate_set(unit, port, value);
            break;
        case RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_2_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_3_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_4_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_5_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_6_CFG_ACTIVE_LOW:
            ret = _phy_826x_led_config_active_low_set(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW), value);
            break;
        case RTK_PHY_CTRL_LED_1_CFG_FORCE:
        case RTK_PHY_CTRL_LED_2_CFG_FORCE:
        case RTK_PHY_CTRL_LED_3_CFG_FORCE:
        case RTK_PHY_CTRL_LED_4_CFG_FORCE:
        case RTK_PHY_CTRL_LED_5_CFG_FORCE:
        case RTK_PHY_CTRL_LED_6_CFG_FORCE:
            ret = _phy_826x_led_config_force_set(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_FORCE), value);
            break;

        case RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_0);
            phyData |= (value == 0)? (0):(BIT_0);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_PMAPMD, 0, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_LOOPBACK_REMOTE:
            ret = _phy_826x_loopback_remote_set(unit, port, value);
            break;

        case RTK_PHY_CTRL_RAPID_LINK_FAULT_DETECT:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_15);
            phyData |= (value == 0)? (0):(BIT_15);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_MDI_INVERSE:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x100, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_10);
            phyData |= (value == 0)? (0):(BIT_10);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x100, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_MDI_POLARITY_SWAP:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x31, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_0 | BIT_1 | BIT_2 | BIT_3));
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A)? (BIT_0) : (0);
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B)? (BIT_1) : (0);
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C)? (BIT_2) : (0);
            phyData |= (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D)? (BIT_3) : (0);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x31, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_PREAMBLE_RECOVERY:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x85, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_12);
            phyData |= (value == 0)? (0):(BIT_12);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x85, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SYNCE:
            ret = _phy_826x_synce_enable_set(unit, port, value);
            break;
        case RTK_PHY_CTRL_SYNCE_PLL:
            ret = _phy_826x_synce_pll_set(unit, port, value);
            break;
        case RTK_PHY_CTRL_SYNCE_0_CLOCK_FREQ:
        case RTK_PHY_CTRL_SYNCE_1_CLOCK_FREQ:
            ret = _phy_826x_synce_clock_freq_set(unit, port, (ctrl_type - RTK_PHY_CTRL_SYNCE_CLOCK_FREQ), value);
            break;
        case RTK_PHY_CTRL_SYNCE_0_RECOVERY_PHY:
        case RTK_PHY_CTRL_SYNCE_1_RECOVERY_PHY:
            ret = _phy_826x_synce_recovery_phy_set(unit, port, (ctrl_type - RTK_PHY_CTRL_SYNCE_RECOVERY_PHY), value);
            break;
        case RTK_PHY_CTRL_SYNCE_0_IDLE_MODE:
        case RTK_PHY_CTRL_SYNCE_1_IDLE_MODE:
            ret = _phy_826x_synce_idle_mode_set(unit, port, (ctrl_type - RTK_PHY_CTRL_SYNCE_IDLE_MODE), value);
            break;

        case RTK_PHY_CTRL_NBASET_802P3BZ_MASK:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5E8, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_13 | BIT_14));
            phyData |= (value == 0)? (0):(BIT_13 | BIT_14);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5E8, phyData)) != RT_ERR_OK)
                return ret;
            ret = _phy_826x_an_restart(unit, port);
            break;
        case RTK_PHY_CTRL_NBASET:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_15));
            phyData |= (value == 0)? (BIT_15):(0);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                return ret;
            ret = _phy_826x_an_restart(unit, port);
            break;

        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER:
            ret = _phy_826x_temperature_threshold_set(unit, port, ctrl_type, value);
            break;

        case RTK_PHY_CTRL_FAST_RETRAIN_NFR:
            switch (value)
            {
                case RTK_PHY_CTRL_FAST_RETRAIN_NFR_ENABLE:
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData &= (~BIT_14);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
                        return ret;

                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA654, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_11);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA654, phyData)) != RT_ERR_OK)
                        return ret;
                    break;

                case RTK_PHY_CTRL_FAST_RETRAIN_NFR_DISABLE:
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_14);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA5EA, phyData)) != RT_ERR_OK)
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
            ret = _phy_826x_an_restart(unit, port);
            break;

        case RTK_PHY_CTRL_FAST_RETRAIN:
            switch (value)
            {
                case RTK_PHY_CTRL_FAST_RETRAIN_ENABLE:
                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_6 | BIT_5 | BIT_1);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
                        return ret;

                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData |= (BIT_7 | BIT_1 | BIT_0);
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA6D8, phyData)) != RT_ERR_OK)
                        return ret;
                    break;

                case RTK_PHY_CTRL_FAST_RETRAIN_DISABLE:

                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData &= (~(BIT_6 | BIT_5 | BIT_1));
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)
                        return ret;

                    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA6D8, &phyData)) != RT_ERR_OK)
                        return ret;
                    phyData &= (~(BIT_7 | BIT_1 | BIT_0));
                    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA6D8, phyData)) != RT_ERR_OK)
                        return ret;
                    break;

                default:
                    return RT_ERR_INPUT;
            }
            ret = _phy_826x_an_restart(unit, port);
            break;

        case RTK_PHY_CTRL_COUNTER_CLEAR:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC802, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(0xFF));
            phyData |= (0x73);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xC802, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1EF, &phyData)) != RT_ERR_OK)
                return ret;
            phyData |= (BIT_13 | BIT_14 | BIT_15);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1EF, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1EF, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_13 | BIT_14 | BIT_15));

            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1EF, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x230, &phyData)) != RT_ERR_OK)
                return ret;
            phyData |= (BIT_6 | BIT_7 | BIT_8);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x230, phyData)) != RT_ERR_OK)
                return ret;

            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x230, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~(BIT_6 | BIT_7 | BIT_8));
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x230, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SERDES_MODE:
            ret = _phy_826x_serdes_mode_set(unit, port, value);
            break;
        case RTK_PHY_CTRL_SERDES_UPDTAE:
            ret = _phy_826x_serdes_mode_update(unit, port);
            break;
        case RTK_PHY_CTRL_SERDES_USXGMII_AN:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 7, 17, &phyData)) != RT_ERR_OK)
                return ret;
            tmp = (phyData & BIT_0) ? (1) : (0);
            if (tmp != value)
            {
                phyData &= (~BIT_0);
                phyData |= (value == 0) ? (0) : (BIT_0);
                if ((ret = _phy_826x_serdes_reg_write(unit, port, 7, 17, phyData)) != RT_ERR_OK)
                    return ret;
            }
            break;
        case RTK_PHY_CTRL_SERDES_SGMII_AN:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 0, 2, &phyData)) != RT_ERR_OK)
                return ret;
            tmp = (phyData & BIT_8) ? (0) : (1);
            if (tmp != value)
            {
                phyData &= (~(BIT_8 | BIT_9));
                phyData |= (value == 0) ? (BIT_8) : (0);
                if ((ret = _phy_826x_serdes_reg_write(unit, port, 0, 2, phyData)) != RT_ERR_OK)
                    return ret;
            }
            break;
        case RTK_PHY_CTRL_SERDES_BASEX_AN:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 2, 0, &phyData)) != RT_ERR_OK)
                return ret;
            tmp = (phyData & BIT_12) ? (1) : (0);
            if (tmp != value)
            {
                phyData &= (~BIT_12);
                phyData |= (value == 0) ? (0) : (BIT_12);
                if ((ret = _phy_826x_serdes_reg_write(unit, port, 2, 0, phyData)) != RT_ERR_OK)
                    return ret;
            }
            break;

        case RTK_PHY_CTRL_SNR_THRESHOLD_10G_MASTER:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x8188)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, value)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_10G_SLAVE:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x8152)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, value)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_5G_MASTER:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x811C)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, value)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_5G_SLAVE:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x80E6)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, value)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_MASTER:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x80B0)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, value)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_SLAVE:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x807A)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, value)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_1G:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA804, value)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_DEBUG_DUMP_DSP:
            ret = _phy_826x_debug_dump_dsp(unit, port, value);
            break;
        case RTK_PHY_CTRL_DEBUG_DUMP_DSP_INIT:
            ret = _phy_826x_debug_dump_dsp_init(unit, port, value);
            break;
        case RTK_PHY_CTRL_DEBUG_DUMP_COUPLING:
            ret = _phy_826x_debug_coupling_dump(unit, port, value);
            break;

        case RTK_PHY_CTRL_REINIT:
            ret = _phy_826x_reinit(unit, port);
            break;

        case RTK_PHY_CTRL_MIIM_BCAST:
            ret = phy_826x_broadcastEnable_set(unit, port, (value == 0)? (DISABLED):(ENABLED));
            break;

        case RTK_PHY_CTRL_MIIM_BCAST_PHYAD:
            ret = phy_826x_broadcastID_set(unit, port, value);
            break;

        case RTK_PHY_CTRL_SERDES_LOOPBACK_REMOTE:
            ret = _phy_826x_serdes_loopback_set(unit, port, value);
            break;

        case RTK_PHY_CTRL_SERDES_TX_POLARITY:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC1, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_7);
            phyData |= (value == 0) ? (0) : (BIT_7);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC1, phyData)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SERDES_RX_POLARITY:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC1, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= (~BIT_6);
            phyData |= (value == 0) ? (0) : (BIT_6);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0xC1, phyData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_RTCT_CABLE_TYPE:
            ret = _phy_826x_rtct_cable_set(unit, port, value);
            break;

        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE:
            rtl826x_info[unit]->eyeParamTarget[port] = value;
            ret = RT_ERR_OK;
            break;

        default:
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
 *      phy_826x_ctrl_get
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
phy_826x_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    int32  ret = 0;
    uint32 phyData = 0;
    uint32 tmp = 0;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_LED_1_MODE:
        case RTK_PHY_CTRL_LED_2_MODE:
        case RTK_PHY_CTRL_LED_3_MODE:
        case RTK_PHY_CTRL_LED_4_MODE:
        case RTK_PHY_CTRL_LED_5_MODE:
        case RTK_PHY_CTRL_LED_6_MODE:
            ret = _phy_826x_led_mode_get(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_MODE), pValue);
            break;
        case RTK_PHY_CTRL_LED_CFG_FLASH_RATE:
            ret = _phy_826x_led_config_flash_rate_get(unit, port, pValue);
            break;
        case RTK_PHY_CTRL_LED_CFG_BLINK_RATE:
            ret = _phy_826x_led_config_blink_rate_get(unit, port, pValue);
            break;
        case RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_2_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_3_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_4_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_5_CFG_ACTIVE_LOW:
        case RTK_PHY_CTRL_LED_6_CFG_ACTIVE_LOW:
            ret = _phy_826x_led_config_active_low_get(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW), pValue);
            break;
        case RTK_PHY_CTRL_LED_1_CFG_FORCE:
        case RTK_PHY_CTRL_LED_2_CFG_FORCE:
        case RTK_PHY_CTRL_LED_3_CFG_FORCE:
        case RTK_PHY_CTRL_LED_4_CFG_FORCE:
        case RTK_PHY_CTRL_LED_5_CFG_FORCE:
        case RTK_PHY_CTRL_LED_6_CFG_FORCE:
            ret = _phy_826x_led_config_force_get(unit, port, (ctrl_type - RTK_PHY_CTRL_LED_1_CFG_FORCE), pValue);
            break;

        case RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x0, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_0) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_LOOPBACK_REMOTE:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x2A1, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_10) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_RAPID_LINK_FAULT_DETECT:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_15) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_MDI_INVERSE:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x100, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_10) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_MDI_POLARITY_SWAP:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x31, &phyData)) != RT_ERR_OK)
                return ret;
            tmp = 0;
            tmp |= (phyData & BIT_0) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A) : (0);
            tmp |= (phyData & BIT_1) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B) : (0);
            tmp |= (phyData & BIT_2) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C) : (0);
            tmp |= (phyData & BIT_3) ? (RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D) : (0);
            *pValue = tmp;
            break;

        case RTK_PHY_CTRL_PREAMBLE_RECOVERY:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x85, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_12) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_SYNCE:
            ret = _phy_826x_synce_enable_get(unit, port, pValue);
            break;
        case RTK_PHY_CTRL_SYNCE_0_CLOCK_FREQ:
        case RTK_PHY_CTRL_SYNCE_1_CLOCK_FREQ:
            ret = _phy_826x_synce_clock_freq_get(unit, port, (ctrl_type - RTK_PHY_CTRL_SYNCE_CLOCK_FREQ), pValue);
            break;
        case RTK_PHY_CTRL_SYNCE_0_RECOVERY_PHY:
        case RTK_PHY_CTRL_SYNCE_1_RECOVERY_PHY:
            ret = _phy_826x_synce_recovery_phy_get(unit, port, (ctrl_type - RTK_PHY_CTRL_SYNCE_RECOVERY_PHY), pValue);
            break;
        case RTK_PHY_CTRL_SYNCE_0_IDLE_MODE:
        case RTK_PHY_CTRL_SYNCE_1_IDLE_MODE:
            ret = _phy_826x_synce_idle_mode_get(unit, port, (ctrl_type - RTK_PHY_CTRL_SYNCE_IDLE_MODE), pValue);
            break;

        case RTK_PHY_CTRL_NBASET_802P3BZ_MASK:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5E8, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_14) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_NBASET:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA5EA, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_15) ? (0) : (1);
            break;
        case RTK_PHY_CTRL_NBASET_STATUS:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA654, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_13) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_AN_COMPLETE:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, 7, 1, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_5) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_TEMP:
            if ((ret = _phy_826x_temperature_get(unit, port, pValue)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER:
        case RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER:
            ret = _phy_826x_temperature_threshold_get(unit, port, ctrl_type, pValue);
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
            if (phyData & (BIT_6 | BIT_5 | BIT_1))
            {
                *pValue = RTK_PHY_CTRL_FAST_RETRAIN_ENABLE;
            }
            else
            {
                *pValue = RTK_PHY_CTRL_FAST_RETRAIN_DISABLE;
            }
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN_LPCOUNT:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA620, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & 0xF800) >> 11;
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN_LDCOUNT:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA620, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & 0x07C0) >> 6;
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN_STATUS:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA65C, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_0) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_FAST_RETRAIN_NFR_STATUS:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA654, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_12) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_SERDES_MODE:
            ret = _phy_826x_serdes_mode_get(unit, port, pValue);
            break;
        case RTK_PHY_CTRL_SERDES_USXGMII_AN:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 7, 17, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_0) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_SERDES_SGMII_AN:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 0, 2, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_8) ? (0) : (1);
            break;
        case RTK_PHY_CTRL_SERDES_BASEX_AN:
            if ((ret = _phy_826x_serdes_reg_read(unit, port, 2, 0, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_12) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_SNR_CH0:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xD12A, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_CH1:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xD12C, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_CH2:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xD144, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_CH3:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xD146, pValue)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SNR_THRESHOLD_10G_MASTER:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x8188)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_10G_SLAVE:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x8152)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_5G_MASTER:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x811C)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_5G_SLAVE:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x80E6)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_MASTER:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x80B0)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_SLAVE:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, 0x807A)) != RT_ERR_OK)
                return ret;
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, pValue)) != RT_ERR_OK)
                return ret;
            break;
        case RTK_PHY_CTRL_SNR_THRESHOLD_1G:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA804, pValue)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_PHY_CTRL_SKEW_PAIR_B:
        case RTK_PHY_CTRL_SKEW_PAIR_C:
        case RTK_PHY_CTRL_SKEW_PAIR_D:
            ret = _phy_826x_skew_get(unit, port, ctrl_type, pValue);
            break;

        case RTK_PHY_CTRL_SERDES_LOOPBACK_REMOTE:
            ret = _phy_826x_serdes_loopback_get(unit, port, pValue);
            break;

        case RTK_PHY_CTRL_SERDES_TX_POLARITY:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC1, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_7) ? (1) : (0);
            break;
        case RTK_PHY_CTRL_SERDES_RX_POLARITY:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0xC1, &phyData)) != RT_ERR_OK)
                return ret;
            *pValue = (phyData & BIT_6) ? (1) : (0);
            break;

        case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE:
            *pValue = rtl826x_info[unit]->eyeParamTarget[port];
            ret = RT_ERR_OK;
            break;

        default:
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
 *      phy_826x_dbgCounter_get
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
 *      to count LDPC after link up, you should read PHY_DBG_CNT_LDPC_ERR once to read-clear when link up
 */
int32
phy_826x_dbgCounter_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint64 *pCnt)
{
    int32   ret;
    uint32  phyData = 0;
    uint64  cnt = 0;
    rtk_port_speed_t spd = 0;
    uint8   tg = 0;

    switch (type)
    {
        case PHY_DBG_CNT_RX:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC81A, &phyData)) != RT_ERR_OK)
                return ret;
            cnt |= (((uint64)(phyData & 0xFF)) << 32);
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC812, &phyData)) != RT_ERR_OK)
                return ret;
            cnt |= (((uint64)(phyData & 0xFFFF)) << 16);
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC810, &phyData)) != RT_ERR_OK)
                return ret;
            cnt |= ((uint64)(phyData & 0xFFFF));
            break;
        case PHY_DBG_CNT_RX_ERR:
            if ((ret = phy_common_c45_speedStatusResReg_get(unit, port, &spd)) != RT_ERR_OK)
            {
                spd = PORT_SPEED_10G;
            }
            switch (spd)
            {
                case PORT_SPEED_10M:
                case PORT_SPEED_100M:
                case PORT_SPEED_1000M:
                case PORT_SPEED_500M:
                    tg = 0;
                    break;
                default:
                    tg = 1;
                    break;
            }
            if (tg == 1)
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x230, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x230, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x24A, &phyData)) != RT_ERR_OK)
                    return ret;
                cnt |= (((uint64)(phyData & 0xFFFF)) << 32);
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x249, &phyData)) != RT_ERR_OK)
                    return ret;
                cnt |= (((uint64)(phyData & 0xFFFF)) << 16);
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x248, &phyData)) != RT_ERR_OK)
                    return ret;
                cnt |= ((uint64)(phyData & 0xFFFF));


                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x230, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x230, phyData)) != RT_ERR_OK)
                    return ret;
            }
            else
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1EE, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData &= (~BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1EE, phyData)) != RT_ERR_OK)
                    return ret;

                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1F7, &phyData)) != RT_ERR_OK)
                    return ret;
                cnt |= ((uint64)phyData);


                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x1EE, &phyData)) != RT_ERR_OK)
                    return ret;
                phyData |= (BIT_1);
                if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x1EE, phyData)) != RT_ERR_OK)
                    return ret;
            }
            break;
        case PHY_DBG_CNT_RX_CRCERR:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xC814, &phyData)) != RT_ERR_OK)
                return ret;
            cnt = ((uint64)(phyData & 0xFFFF));
            break;

        case PHY_DBG_CNT_LDPC_ERR:
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xCE42, &phyData)) != RT_ERR_OK)
                return ret;

            cnt = ((uint64)(phyData & 0xFF));
            break;
        default:
            return RT_ERR_INPUT;
    }
    *pCnt = cnt;
    return ret;
}

/* Function Name:
 *      phy_826x_greenEnable_get
 * Description:
 *      Get the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-up green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
phy_826x_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = _phy_826x_indirect_read(unit, port, 0x8010, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_7) ? (ENABLED) : (DISABLED);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up  green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
phy_826x_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = _phy_826x_indirect_read(unit, port, 0x8010, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~BIT_7);
    phyData |= (enable == DISABLED) ? (0) : (BIT_7);
    if ((ret = _phy_826x_indirect_write(unit, port, 0x8010, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_downSpeedEnable_get
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
phy_826x_downSpeedEnable_get(uint32 unit, rtk_port_t port,
                             rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_3) ? (ENABLED) : (DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_downSpeedEnable_set
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
phy_826x_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_2 | BIT_3));
    phyData |= (enable == DISABLED) ? (0) : (BIT_2 | BIT_3);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA442, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_826x_downSpeedStatus_get
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
phy_826x_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA442, &phyData)) != RT_ERR_OK)
        return ret;
    *pDownSpeedStatus = (phyData & BIT_3) ? (1) : (0);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_eeeEnable_get
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
phy_826x_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    rtk_enable_t ena0 = 0;
    rtk_enable_t ena1 = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 60, &phyData)) != RT_ERR_OK)
        return ret;
    ena0 = ((phyData & (BIT_1 | BIT_2 | BIT_3)) == (BIT_1 | BIT_2 | BIT_3)) ? ENABLED : DISABLED;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 62, &phyData)) != RT_ERR_OK)
        return ret;
    ena1 = ((phyData & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1)) ? ENABLED : DISABLED;

    if (ena0 == ena1)
        *pEnable = ena0;
    else
        *pEnable = DISABLED;
    return ret;
}

/* Function Name:
 *      phy_826x_eeeEnable_set
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
phy_826x_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 60, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (ENABLED == enable) ? (phyData | (BIT_1 | BIT_2 | BIT_3)) : (phyData & ~(BIT_1 | BIT_2 | BIT_3));

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 60, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_AN, 62, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (ENABLED == enable) ? (phyData | (BIT_0 | BIT_1)) : (phyData & ~(BIT_0 | BIT_1));

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_AN, 62, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA428, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = (ENABLED == enable) ? (phyData | (BIT_7)) : (phyData & (~BIT_7));
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA428, phyData)) != RT_ERR_OK)
        return ret;

    ret = _phy_826x_an_restart(unit, port);
    return ret;
}

/* Function Name:
 *      phy_826x_eeepEnable_get
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
phy_826x_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA432, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_9) ? ENABLED : DISABLED;
    return ret;
}

/* Function Name:
 *      phy_826x_eeepEnable_set
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
phy_826x_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA432, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (ENABLED == enable) ? (phyData | (BIT_9 | BIT_10)) : (phyData & ~(BIT_9 | BIT_10));

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA432, phyData)) != RT_ERR_OK)
        return ret;

    ret = _phy_826x_an_restart(unit, port);
    return ret;
}



/* Function Name:
 *      phy_826x_linkDownPowerSavingEnable_get
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
 *      1. The RTL826x is supported the per-port link-down power saving
 */
int32
phy_826x_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_2) ? (ENABLED) : (DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_linkDownPowerSavingEnable_set
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
 *      1. The RTL826x is supported the per-port link-down power saving
 */
int32
phy_826x_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
 *      phy_826x_broadcastEnable_set
 * Description:
 *      Set enable status of miim broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_826x_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;

    if (enable == DISABLED)
    {
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x9003, 0)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x9003, &phyData)) != RT_ERR_OK)
            return ret;
        phyData |= (BIT_15);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x9003, phyData)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      broadcastID   - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_826x_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID)
{
    int32 ret = 0;
    uint32  phyData = 0;

    if (broadcastID > 0x1F)
        return RT_ERR_OUT_OF_RANGE;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, 0x9003, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, broadcastID, 0, 0x1F);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, 0x9003, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_sdsEyeParam_get
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
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
int32
phy_826x_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32 ret  = 0;
    uint32 val = 0;

    if ((ret = _phy_826x_preemphasis_get(unit, port, &val)) != RT_ERR_OK)
        return ret;
    pEyeParam->post_amp  = PHY_826X_PREEMPHASIS_GET_POST1(val);
    pEyeParam->post2_amp = PHY_826X_PREEMPHASIS_GET_POST2(val);
    pEyeParam->post_en   = PHY_826X_PREEMPHASIS_GET_POST1_EN(val);
    pEyeParam->post2_en  = PHY_826X_PREEMPHASIS_GET_POST2_EN(val);
    pEyeParam->main_amp  = PHY_826X_PREEMPHASIS_GET_MAIN(val);
    pEyeParam->main_en   = PHY_826X_PREEMPHASIS_GET_MAIN_EN(val);
    pEyeParam->pre_amp   = PHY_826X_PREEMPHASIS_GET_PRE(val);
    pEyeParam->pre_en    = PHY_826X_PREEMPHASIS_GET_PRE_EN(val);
    pEyeParam->impedance = 0;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_826x_sdsEyeParam_set
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
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
int32
phy_826x_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32 ret  = 0;
    uint32 val = 0;
    PHY_826X_PREEMPHASIS_SET_POST1(val, pEyeParam->post_amp);
    PHY_826X_PREEMPHASIS_SET_POST2(val, pEyeParam->post2_amp);
    PHY_826X_PREEMPHASIS_SET_POST1_EN(val, pEyeParam->post_en);
    PHY_826X_PREEMPHASIS_SET_POST2_EN(val, pEyeParam->post2_en);
    PHY_826X_PREEMPHASIS_SET_MAIN(val, pEyeParam->main_amp);
    PHY_826X_PREEMPHASIS_SET_MAIN_EN(val, pEyeParam->main_en);
    PHY_826X_PREEMPHASIS_SET_PRE(val, pEyeParam->pre_amp);
    PHY_826X_PREEMPHASIS_SET_PRE_EN(val, pEyeParam->pre_en);
    ret = _phy_826x_preemphasis_set(unit, port, val);
    return ret;
}

/* Function Name:
 *      phy_826x_mdiLoopbackEnable_get
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
phy_826x_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    *pEnable = (phyData & BIT_10) ? (ENABLED) : (DISABLED);
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_826x_mdiLoopbackEnable_set
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
phy_826x_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret = 0;
    uint32  phyData = 0;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA430, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_10|BIT_11));
    phyData |= (enable == DISABLED) ? (0) : (BIT_10|BIT_11);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA430, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_eyeMon_scan_en(uint32 unit, uint32 port)
{

    uint32  phyData = 0;
    int32   ret;

    if ((ret = _phy_826x_serdes_reg_read(unit, port, PHY_826X_EYE_SCAN_EN_PAGE, PHY_826X_EYE_SCAN_EN_REG, &phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (1 << PHY_826X_EYE_SCAN_EN_BIT);
    if ((ret = _phy_826x_serdes_reg_write(unit, port, PHY_826X_EYE_SCAN_EN_PAGE, PHY_826X_EYE_SCAN_EN_REG, phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(1 << PHY_826X_EYE_SCAN_EN_BIT));
    if ((ret = _phy_826x_serdes_reg_write(unit, port, PHY_826X_EYE_SCAN_EN_PAGE, PHY_826X_EYE_SCAN_EN_REG, phyData)) != RT_ERR_OK)
        return ret;
    phyData |= (1 << PHY_826X_EYE_SCAN_EN_BIT);
    if ((ret = _phy_826x_serdes_reg_write(unit, port, PHY_826X_EYE_SCAN_EN_PAGE, PHY_826X_EYE_SCAN_EN_REG, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_eyeMon_pi_set(uint32 unit, uint32 port, uint32 x)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = _phy_826x_serdes_reg_read(unit, port, PHY_826X_EYE_PI_PHASE_PAGE, PHY_826X_EYE_PI_PHASE_REG, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, x, 0, 0x3F);
    if ((ret = _phy_826x_serdes_reg_write(unit, port, PHY_826X_EYE_PI_PHASE_PAGE, PHY_826X_EYE_PI_PHASE_REG, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_eyeMon_ref_set(uint32 unit, uint32 port, uint32 y)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = _phy_826x_serdes_reg_read(unit, port, PHY_826X_EYE_REF_CTRL_PAGE, PHY_826X_EYE_REF_CTRL_REG, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, y, 0, 0x3F);
    if ((ret = _phy_826x_serdes_reg_write(unit, port, PHY_826X_EYE_REF_CTRL_PAGE, PHY_826X_EYE_REF_CTRL_REG, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_eyeMon_DBGOUT(uint32 unit, uint32 port, uint32 eyeBasePage)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = _phy_826x_serdes_reg_write(unit, port, 0x1F, 0x02, 0x0034)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_826x_serdes_reg_read(unit, port, (eyeBasePage + 1), 0x08, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = REG32_FIELD_SET(phyData, 0xC, 8, 0x0F00);
    if ((ret = _phy_826x_serdes_reg_write(unit, port, (eyeBasePage + 1), 0x08, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_826x_eyeMon_MDIOCLK(uint32 unit, uint32 port, uint32 eyeBasePage)
{
    int32   ret;
    uint32  phyData = 0;

    if ((ret = _phy_826x_serdes_reg_read(unit, port, eyeBasePage, 0x1F, &phyData)) != RT_ERR_OK)
        return ret;
    phyData = phyData | 0x1;
    if ((ret = _phy_826x_serdes_reg_write(unit, port, eyeBasePage, 0x1F, phyData)) != RT_ERR_OK)
        return ret;
    phyData = phyData & 0xFFFE;
    if ((ret = _phy_826x_serdes_reg_write(unit, port, eyeBasePage, 0x1F, phyData)) != RT_ERR_OK)
        return ret;
    phyData = phyData | 0x1;
    if ((ret = _phy_826x_serdes_reg_write(unit, port, eyeBasePage, 0x1F, phyData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_826x_eyeMon_readOut(uint32 unit, uint32 port, uint32 *pdata)
{
    return _phy_826x_serdes_reg_read(unit, port, 0x1F, 0x14, pdata);
}

int32
_phy_826x_eyeMon_eyePointRead(uint32 unit, uint32 port, uint32 eyeBasePage, uint32 *val)
{
    int32   retry = 100, ret = RT_ERR_OK;

    do
    {
        _phy_826x_eyeMon_MDIOCLK(unit, port, eyeBasePage);
        _phy_826x_eyeMon_readOut(unit, port, val);

        retry--;
        if (retry <= 0)
        {
            ret = RT_ERR_BUSYWAIT_TIMEOUT;
            RT_ERR(ret, (MOD_HAL), "unit %u port %u readOut retry timeout. val=%x", unit, port, *val);
            break;
        }
    } while (((*val >> 14) & 0x3) != 0x3);

    return ret;
}

int32
_phy_826x_eyeMon_proc(uint32 unit, uint32 port, uint32 frameNum, phy_sds_eyeMon_hdlr_t pHdlr, void *pDb)
{
    uint32  x, y, frame;
    uint32  ReadBack;
    uint32  val;
    int32   ret;
    rt_serdesMode_t sds_mode;
    uint32          eye_page;

    if ((ret = phy_826x_macIntfSerdesMode_get(unit, port, &sds_mode)) != RT_ERR_OK)
        return ret;

    switch (sds_mode)
    {
        case RTK_MII_5GR:
            eye_page = PHY_826X_EYE_PAGE_BASE_5GR;
            break;
        case RTK_MII_5GBASEX:
            eye_page = PHY_826X_EYE_PAGE_BASE_5GX;
            break;
        case RTK_MII_2500Base_X:
            eye_page = PHY_826X_EYE_PAGE_BASE_2P5GX;
            break;
        case RTK_MII_SGMII:
            eye_page = PHY_826X_EYE_PAGE_BASE_1G;
            break;
        default:
            eye_page = PHY_826X_EYE_PAGE_BASE;
            break;
    }

    /* enable */
    ret = _phy_826x_eyeMon_scan_en(unit, port);

    /* set x axis */
    for (x = 0; x < RTK_EYE_MON_X_MAX; ++x)
    {
        ret = _phy_826x_eyeMon_pi_set(unit, port, x);

        /* set y axis */
        for (y = 0; y < RTK_EYE_MON_Y_MAX; ++y)
        {
            ret = _phy_826x_eyeMon_ref_set(unit, port, y);

            ret = _phy_826x_eyeMon_DBGOUT(unit, port, eye_page);
            ret = _phy_826x_eyeMon_MDIOCLK(unit, port, eye_page);
            ret = _phy_826x_eyeMon_readOut(unit, port, &ReadBack);
            /* get val */
            for (frame = 0; frame < frameNum; ++frame)
            {
                if ((ret = _phy_826x_eyeMon_eyePointRead(unit, port, eye_page, &val)) != RT_ERR_OK)
                {
                    goto EXIT;
                }
                pHdlr(x, y, frame, pDb, val);
            }
        }
    }

EXIT:
    return ret;
}

/* Function Name:
 *      phy_826x_eyeMonitor_start
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
phy_826x_eyeMonitor_start(uint32 unit, uint32 port, uint32 sdsId, uint32 frameNum)
{
    uint32  x, y, frame;
    uint32  *eyeData = NULL;
    uint32  data_size;
    int32   ret;

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_INPUT;
    }

    if (frameNum > RTK_EYE_MON_FRAME_MAX)
    {
        RT_EYEMON_LOG("frameNum %u exceed maxmum %u!", frameNum, RTK_EYE_MON_FRAME_MAX);
        return RT_ERR_FAILED;
    }

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        RT_EYEMON_LOG("malloc %u fail!\n", data_size);
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    RT_EYEMON_LOG("uint:%u port:%u sdsId:%u frameNum:%u", unit, port, sdsId, frameNum);

    if ((ret = _phy_826x_eyeMon_proc(unit, port, frameNum, phy_common_eyeMonPixel_get, (void *)eyeData)) != RT_ERR_OK)
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
 *      phy_826x_eyeMonitorInfo_get
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
phy_826x_eyeMonitorInfo_get(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
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


    if ((ret = _phy_826x_eyeMon_proc(unit, port, frameNum, phy_common_eyeMonInfo_update, (void *)eye)) != RT_ERR_OK)
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
 *      phy_826Xdrv_mapperInit
 * Description:
 *      Initialize PHY RTL8261/8264/8261I/8264I/8251/8254/8251I/8254I/8224 driver.
 * Input:
 *      phydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_826Xdrv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index                          = RT_PHYDRV_RTL826X;
    pPhydrv->fPhydrv_init                          = phy_826x_init;
    pPhydrv->fPhydrv_reset_set                     = phy_common_c45_reset_set;
    pPhydrv->fPhydrv_media_get                     = phy_826x_media_get;
    pPhydrv->fPhydrv_ctrl_get                      = phy_826x_ctrl_get;
    pPhydrv->fPhydrv_ctrl_set                      = phy_826x_ctrl_set;
    pPhydrv->fPhydrv_linkStatus_get                = phy_826x_linkStatus_get;
    pPhydrv->fPhydrv_enable_get                    = phy_common_c45_enable_get;
    pPhydrv->fPhydrv_enable_set                    = phy_common_c45_enable_set;
    pPhydrv->fPhydrv_autoNegoEnable_get            = phy_common_c45_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set            = phy_common_c45_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbilityLocal_get      = phy_826x_autoNegoAbilityLocal_get;
    pPhydrv->fPhydrv_autoNegoAbility_get           = phy_826x_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set           = phy_826x_autoNegoAbility_set;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get       = phy_826x_autoNegoAbilityPeer_get;
    pPhydrv->fPhydrv_speed_get                     = phy_common_c45_speed_get;
    pPhydrv->fPhydrv_speed_set                     = phy_826x_speed_set;
    pPhydrv->fPhydrv_speedStatus_get               = phy_common_c45_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get         = phy_common_c45_speedDuplexStatusResReg_get;
    pPhydrv->fPhydrv_duplex_get                    = phy_826x_duplex_get;
    pPhydrv->fPhydrv_duplex_set                    = phy_826x_duplex_set;
    pPhydrv->fPhydrv_masterSlave_get               = phy_common_c45_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set               = phy_common_c45_masterSlave_set;
    pPhydrv->fPhydrv_crossOverStatus_get           = phy_common_c45_crossOverStatus_get;
    pPhydrv->fPhydrv_crossOverMode_get             = phy_826x_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set             = phy_826x_crossOverMode_set;
    pPhydrv->fPhydrv_macIntfSerdesMode_get         = phy_826x_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get   = phy_826x_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_ieeeTestMode_set              = phy_common_c45_ieeeTestMode_set;
    pPhydrv->fPhydrv_rtctResult_get                = phy_826x_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start                    = phy_826x_rtct_start;
    pPhydrv->fPhydrv_intrStatus_get                = phy_826x_intrStatus_get;
    pPhydrv->fPhydrv_intrEnable_get                = phy_826x_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set                = phy_826x_intrEnable_set;
    pPhydrv->fPhydrv_intrMask_get                  = phy_826x_intrMask_get;
    pPhydrv->fPhydrv_intrMask_set                  = phy_826x_intrMask_set;
    pPhydrv->fPhydrv_reg_mmd_get                   = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set                   = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_sdsTestMode_set               = phy_826x_sdsTestMode_set;
    pPhydrv->fPhydrv_sdsTestModeCnt_get            = phy_826x_sdsTestModeCnt_get;
    pPhydrv->fPhydrv_dbgCounter_get                = phy_826x_dbgCounter_get;
    pPhydrv->fPhydrv_greenEnable_get               = phy_826x_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set               = phy_826x_greenEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get           = phy_826x_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set           = phy_826x_downSpeedEnable_set;
    pPhydrv->fPhydrv_downSpeedStatus_get           = phy_826x_downSpeedStatus_get;
    pPhydrv->fPhydrv_eeeEnable_get                 = phy_826x_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set                 = phy_826x_eeeEnable_set;
    pPhydrv->fPhydrv_eeepEnable_get                = phy_826x_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set                = phy_826x_eeepEnable_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_826x_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_826x_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_loopback_get                  = phy_common_c45_loopback_get;
    pPhydrv->fPhydrv_loopback_set                  = phy_common_c45_loopback_set;
    pPhydrv->fPhydrv_sdsEyeParam_get               = phy_826x_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set               = phy_826x_sdsEyeParam_set;
    pPhydrv->fPhydrv_mdiLoopbackEnable_get         = phy_826x_mdiLoopbackEnable_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_set         = phy_826x_mdiLoopbackEnable_set;
    pPhydrv->fPhydrv_portEyeMonitor_start          = phy_826x_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get        = phy_826x_eyeMonitorInfo_get;
}

/* Function Name:
 *      phy_825XLdrv_mapperInit
 * Description:
 *      Initialize PHY RTL8251L/8254L driver.
 * Input:
 *      phydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_825XLdrv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index                          = RT_PHYDRV_RTL825XL;
    pPhydrv->fPhydrv_init                          = phy_826x_init;
    pPhydrv->fPhydrv_reset_set                     = phy_common_c45_reset_set;
    pPhydrv->fPhydrv_media_get                     = phy_826x_media_get;
    pPhydrv->fPhydrv_ctrl_get                      = phy_826x_ctrl_get;
    pPhydrv->fPhydrv_ctrl_set                      = phy_826x_ctrl_set;
    pPhydrv->fPhydrv_linkStatus_get                = phy_826x_linkStatus_get;
    pPhydrv->fPhydrv_enable_get                    = phy_common_c45_enable_get;
    pPhydrv->fPhydrv_enable_set                    = phy_common_c45_enable_set;
    pPhydrv->fPhydrv_autoNegoEnable_get            = phy_common_c45_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set            = phy_common_c45_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbilityLocal_get      = phy_826x_autoNegoAbilityLocal_get;
    pPhydrv->fPhydrv_autoNegoAbility_get           = phy_826x_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set           = phy_826x_autoNegoAbility_set;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get       = phy_826x_autoNegoAbilityPeer_get;
    pPhydrv->fPhydrv_speed_get                     = phy_common_c45_speed_get;
    pPhydrv->fPhydrv_speed_set                     = phy_826x_speed_set;
    pPhydrv->fPhydrv_speedStatus_get               = phy_common_c45_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get         = phy_common_c45_speedDuplexStatusResReg_get;
    pPhydrv->fPhydrv_duplex_get                    = phy_826x_duplex_get;
    pPhydrv->fPhydrv_duplex_set                    = phy_826x_duplex_set;
    pPhydrv->fPhydrv_masterSlave_get               = phy_common_c45_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set               = phy_common_c45_masterSlave_set;
    pPhydrv->fPhydrv_crossOverStatus_get           = phy_common_c45_crossOverStatus_get;
    pPhydrv->fPhydrv_crossOverMode_get             = phy_826x_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set             = phy_826x_crossOverMode_set;
    pPhydrv->fPhydrv_liteEnable_get                = phy_826x_liteEnable_get;
    pPhydrv->fPhydrv_liteEnable_set                = phy_826x_liteEnable_set;
    pPhydrv->fPhydrv_macIntfSerdesMode_get         = phy_826x_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get   = phy_826x_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_ieeeTestMode_set              = phy_common_c45_ieeeTestMode_set;
    pPhydrv->fPhydrv_rtctResult_get                = phy_826x_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start                    = phy_826x_rtct_start;
    pPhydrv->fPhydrv_intrStatus_get                = phy_826x_intrStatus_get;
    pPhydrv->fPhydrv_intrEnable_get                = phy_826x_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set                = phy_826x_intrEnable_set;
    pPhydrv->fPhydrv_intrMask_get                  = phy_826x_intrMask_get;
    pPhydrv->fPhydrv_intrMask_set                  = phy_826x_intrMask_set;
    pPhydrv->fPhydrv_reg_mmd_get                   = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set                   = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_sdsTestMode_set               = phy_826x_sdsTestMode_set;
    pPhydrv->fPhydrv_sdsTestModeCnt_get            = phy_826x_sdsTestModeCnt_get;
    pPhydrv->fPhydrv_dbgCounter_get                = phy_826x_dbgCounter_get;
    pPhydrv->fPhydrv_greenEnable_get               = phy_826x_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set               = phy_826x_greenEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get           = phy_826x_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set           = phy_826x_downSpeedEnable_set;
    pPhydrv->fPhydrv_downSpeedStatus_get           = phy_826x_downSpeedStatus_get;
    pPhydrv->fPhydrv_eeeEnable_get                 = phy_826x_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set                 = phy_826x_eeeEnable_set;
    pPhydrv->fPhydrv_eeepEnable_get                = phy_826x_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set                = phy_826x_eeepEnable_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_826x_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_826x_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_loopback_get                  = phy_common_c45_loopback_get;
    pPhydrv->fPhydrv_loopback_set                  = phy_common_c45_loopback_set;
    pPhydrv->fPhydrv_sdsEyeParam_get               = phy_826x_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set               = phy_826x_sdsEyeParam_set;
    pPhydrv->fPhydrv_mdiLoopbackEnable_get         = phy_826x_mdiLoopbackEnable_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_set         = phy_826x_mdiLoopbackEnable_set;
    pPhydrv->fPhydrv_portEyeMonitor_start          = phy_826x_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get        = phy_826x_eyeMonitorInfo_get;
}


