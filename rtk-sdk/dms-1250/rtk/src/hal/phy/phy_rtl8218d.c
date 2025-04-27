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
 * $Revision: 75479 $
 * $Date: 2017-01-20 15:17:16 +0800 (Fri, 20 Jan 2017) $
 *
 * Purpose : PHY 8218D Driver APIs.
 *
 * Feature : PHY 8218D Driver APIs
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
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8218d.h>
#include <hal/phy/phy_rtl8218d_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <rtk/sds.h>
#include <osal/time.h>
#include <osal/memory.h>


/*
 * Symbol Definition
 */
#define PHY_8218D_MAX_LED                               36
#define PHY_8218D_MAX_MDI                               8

#define PHY_8218D_GREEN_EN_OFFSET                       0
#define PHY_8218D_GREEN_EN_MASK                         (0xFF << PHY_8218D_GREEN_EN_OFFSET)
#define PHY_8218D_GREEN_EN_PORT_OFFSET(phyPort)         ((phyPort & 0x7) + PHY_8218D_GREEN_EN_OFFSET)
#define PHY_8218D_GREEN_EN_PORT_MASK(phyPort)           (0x1 << PHY_8218D_GREEN_EN_PORT_OFFSET(phyPort))

#define PHY_8218D_RTCT_LENGTH(_phyData)         ((_phyData) * 100 / 80)
#define PHY_8218D_RTCT_ENABLE                   (0)
#define PHY_8218D_RTCT_CH_A                     (4)
#define PHY_8218D_RTCT_CH_B                     (5)
#define PHY_8218D_RTCT_CH_C                     (6)
#define PHY_8218D_RTCT_CH_D                     (7)
#define PHY_8218D_RTCT_RSV                      (8)
#define PHY_8218D_RTCT_DONE                     (15)

/* Serial LED Configuration register definition */
#define PHY_8218D_SERIAL_LED_CONFIG_PAGE                                0x280
#define PHY_8218D_SERIAL_LED_CONFIG_REG                                 16
#define PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET            8
#define PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK              (0xF << PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET)
#define PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET       6
#define PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK         (0x3 << PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET)
#define PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET       4
#define PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK         (0x3 << PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET)
#define PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET          3
#define PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK            (0x1 << PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET)

/* Serial LED PER LED Control register definition */
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET                    12
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_MDI_MASK                      (0xF << PHY_8218D_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET            11
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_MASK              (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET             10
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_SPEED_MASK               (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET              9
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_SPEED_MASK                (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET              7
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_ACT_MASK                (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET               6
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_ACT_MASK                 (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET                5
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_ACT_MASK                  (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET                 3
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_DUPLEX_MASK                   (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET              2
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_COLLISION_MASK                (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET                 1
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_TX_ACT_MASK                   (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET)
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET                 0
#define PHY_8218D_SERIAL_LED_PER_LED_CTRL_RX_ACT_MASK                   (0x1 << PHY_8218D_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET)


#define PHY_8218D_MDI_LB_MDI_VAL_GET(d)                 REG32_FIELD_GET(d, 10, (0x3<<10))
#define PHY_8218D_MDI_LB_MDI_VAL_SET(d, v)              REG32_FIELD_SET(d, v, 10, (0x3<<10))


typedef struct phy_rtl8218d_info_s
{
    uint8   mdi_val_org[RTK_MAX_NUM_OF_PORTS];
} phy_rtl8218d_info_t;

/*
 * Data Declaration
 */
phy_rtl8218d_info_t     rtl8218d_info[RTK_MAX_NUM_OF_UNIT];
#define PHY_RTL8218D_LEQ_DFE            0   /* 1: do DFE;  0: disable DFE */


typedef enum phy_8218d_regPtp_e
{
    REG_8218D_PTP_BASE              = 0x1600,
    REG_8218D_PTP_TIME_NSEC_L       = 0x1600,
    REG_8218D_PTP_TIME_NSEC_H       = 0x1601,
    REG_8218D_PTP_TIME_SEC_L        = 0x1602,
    REG_8218D_PTP_TIME_SEC_M        = 0x1603,
    REG_8218D_PTP_TIME_CFG_0        = 0x1604,
    REG_8218D_PTP_MAC_ADDR_L        = 0x1605,
    REG_8218D_PTP_MAC_ADDR_M        = 0x1606,
    REG_8218D_PTP_MAC_ADDR_H        = 0x1607,
    REG_8218D_PTP_OTAG_TPID         = 0x1608,
    REG_8218D_PTP_ITAG_TPID         = 0x1609,
    REG_8218D_PTP_TIME_NSEC_L_RO    = 0x160A,
    REG_8218D_PTP_TIME_NSEC_H_RO    = 0x160B,
    REG_8218D_PTP_TIME_SEC_L_RO     = 0x160C,
    REG_8218D_PTP_TIME_SEC_M_RO     = 0x160D,
    REG_8218D_PTP_TIME_CFG_1        = 0x160E,
    REG_8218D_PTP_TIME_INT_STS_P    = 0x160F,

    /* per-port register addr-offset: 0x10 */
    REG_8218D_PTP_TIME_TX_SID_PN    = 0x1610,
    REG_8218D_PTP_TIME_RX_SID_PN    = 0x1614,
    REG_8218D_PTP_TIME_NSEC_L_PN    = 0x1618,
    REG_8218D_PTP_TIME_NSEC_H_PN    = 0x1619,
    REG_8218D_PTP_TIME_SEC_L_PN     = 0x161A,
    REG_8218D_PTP_TIME_SEC_M_PN     = 0x161B,
    REG_8218D_PTP_TIME_CTRL_PN      = 0x161C,
    REG_8218D_PTP_TIME_SEC_H_PN      = 0x161D,

    REG_8218D_PTP_FREQ_LSB      = 0x16D4,
    REG_8218D_PTP_FREQ_MSB      = 0x16D5,
    REG_8218D_PTP_TIME_SEC_H        = 0x16D6,
    REG_8218D_PTP_TIME_SEC_H_RO  = 0x16D7,
    REG_8218D_PTP_OTAG_TPID_1      = 0x16D8,
    REG_8218D_PTP_ITAG_TPID_1       = 0x16D9,
    REG_8218D_PTP_OTAG_TPID_2      = 0x16DA,
    REG_8218D_PTP_ITAG_TPID_2       = 0x16DB,
    REG_8218D_PTP_OTAG_TPID_3      = 0x16DC,
    REG_8218D_PTP_ITAG_TPID_3       = 0x16DD,
} phy_8218d_regPtp_t;

typedef enum phy_8218d_regPtpField_e
{
    REG_FIELD_8218D_PTP_PHY_ETH_EN              = (0x1 << 8),
    REG_FIELD_8218D_PTP_PHY_UDP_EN              = (0x1 << 9),

    REG_FIELD_8218D_PTP_CMD_EXEC            = (0x1 << 15),
    REG_FIELD_8218D_PTP_MAC_RNG_MSK          = (0x3FF << 6),
    REG_FIELD_8218D_PTP_CMD_OP_MSK          = (0x3 << 4),
    REG_FIELD_8218D_PTP_CMD_OP_READ         = (0x0 << 4),
    REG_FIELD_8218D_PTP_CMD_OP_WRITE        = (0x1 << 4),
    REG_FIELD_8218D_PTP_CMD_OP_ADJ_INC      = (0x2 << 4),
    REG_FIELD_8218D_PTP_CMD_OP_ADJ_DEC      = (0x3 << 4),
    REG_FIELD_8218D_PTP_TIME_NSEC_H         = (0x3FFF << 0),

    REG_FIELD_8218D_PTP_CFG_TIMER_EN_FRC    = (0x1 << 2),
    REG_FIELD_8218D_PTP_CFG_TIMER_1588_EN   = (0x1 << 1),

    REG_FIELD_8218D_PTP_CFG_GPI_RISE_TRI   = (0x1 << 13),
    REG_FIELD_8218D_PTP_CFG_GPI_FALL_TRI   = (0x1 << 12),

    REG_FIELD_8218D_PTP_CFG_TIMER_RD_SRC_GPI   = (0x1 << 10),

} phy_8218d_regPtpField_t;



#define REG_8218D_PTP_OFFSET_PORT(port) ((port % 8) * 0x10)
#define REG_8218D_PTP_OFFSET_MSGTYPE(type) (type % 4)


rt_phyInfo_t phy_8218D_info =
{
    .phy_num    = PORT_NUM_IN_8218D,
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


/*
 * Function Declaration
 */
int32
phy_8218d_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      _phy_8218d_indr_read
 * Description:
 *      8218D indirect address read
 * Input:
 *      unit - unit id
 *      port - port id
 *      indr_addr - indirect address
 * Output:
 *      pData - data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218d_indr_read(uint32 unit, rtk_port_t port, uint32 indr_addr, uint32 *pData)
{
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 27, indr_addr)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, port, 0, 28, pData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_rtctGeStatus_convert
 * Description:
 *      Convert RTCT result status register data to SDK status.
 * Input:
 *      phyData - result status data of RTCT register
 * Output:
 *      pShort - short
 *      pOpen   - open
 *      pMismatch -  mismatch
 *      pLinedriver - 8218D not support
 *      pHiImpedance - hi-impedance
 *      pCross -  cross
 *      pPartialCross - partial cross
 *      pPairBusy - pair busy
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218d_rtctGeStatus_convert(uint32 phyData,
                                uint32 *pShort, uint32 *pOpen,
                                uint32 *pMismatch, uint32 *pLinedriver,
                                uint32 *pHiImpedance, uint32 *pCross,
                                uint32 *pPartialCross, uint32 *pPairBusy)
{
    if (phyData == 0x0060)
    {
        /* normal */
        return RT_ERR_OK;
    }
    else if (phyData == 0x0048)
    {
        *pOpen = 1;
    }
    else if (phyData == 0x0050)
    {
        *pShort = 1;
    }
    else if (phyData == 0x0042) /* Mis-Match_Open */
    {
        *pMismatch = 1;
    }
    else if (phyData == 0x0044) /* Mis-Match_short */
    {
        *pMismatch = 2;
    }
    else if (phyData == 0x0041)
    {
        *pHiImpedance = 1;
    }
    else if (phyData == 0x00C0)
    {
        *pPairBusy = 1;
    }
    else if (phyData == 0x0140)
    {
        *pCross = 1;
    }
    else if (phyData == 0x0340) /* Partial-Cross A+ Open, A- Normal */
    {
        *pPartialCross = 1;
    }
    else
    {
        /* unknown status */
        RT_ERR(RT_ERR_TIMEOUT, (MOD_HAL), "RTCT status data 0x%x", phyData);
        return RT_ERR_TIMEOUT;
    }


    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218d_serialLedModePageReg_get
 * Description:
 *      Get page and register number of a specific LED mode register
 * Input:
 *      unit - unit id
 *      port - base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218d_serialLedModePageReg_get(uint32 led_id, uint32 *pPage, uint32 *pReg)
{
    uint32 ledModeReg_00_05[] = {18, 19, 20, 21, 22, 23 }; /* LED 00~05 */
    uint32 ledModeReg[] = { 16, 17, 18, 19, 20, 21, 22, 23 }; /* LED 00~06, 14~21, 22~29, 30~35 */

    if (led_id >= PHY_8218D_MAX_LED)
    {
        return RT_ERR_INPUT;
    }

    if (led_id >= 30)
    {
        *pPage = 0x285;
        *pReg = ledModeReg[(led_id - 30)];
    }
    else if (led_id >= 22)
    {
        *pPage = 0x284;
        *pReg = ledModeReg[(led_id - 22)];
    }
    else if (led_id >= 14)
    {
        *pPage = 0x283;
        *pReg = ledModeReg[(led_id - 14)];
    }
    else if (led_id >= 6)
    {
        *pPage = 0x282;
        *pReg = ledModeReg[(led_id - 6)];
    }
    else
    {
        *pPage = 0x281;
        *pReg = ledModeReg_00_05[led_id];
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_ledMode_dump
 * Description:
 *      Dump LED registers
 * Input:
 *      unit - unit id
 *      port - base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218d_ledMode_dump(uint32 unit, rtk_port_t port)
{
    uint32  page, reg, data, blk;
    int     led_id, ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);
    for (led_id=0; led_id<36; led_id++)
    {
        osal_printf("LED%02u ", led_id);
        if ((ret = _phy_8218d_serialLedModePageReg_get(led_id, &page, &reg)) != RT_ERR_OK)
        {
            osal_printf(" getp page/reg error %X\n", ret);
            continue;
        }
        hal_miim_read(unit, port, page, reg, &data);
        osal_printf("(0x%X.%u) 0x%04X\n", page, reg, data);
    }
    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_loopback_remote_get
 * Description:
 *      Get remote loopback status
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      *pEna - 1: remote loopback enabled; 0: remote loopback disabled
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
_phy_8218d_loopback_remote_get(uint32 unit, rtk_port_t port, uint32 *pEna)
{
    int32 ret = RT_ERR_OK;
    uint32 basePort = 0, phyPort = 0;
    uint32 blk = 0, data;
    uint32 page[PORT_NUM_IN_8218D] = {0x322, 0x322, 0x322, 0x323, 0x324, 0x324, 0x324, 0x325};
    uint32 reg[PORT_NUM_IN_8218D] = {0x10, 0x13, 0x16, 0x11, 0x10, 0x13, 0x16, 0x11};

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218D)
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    ret = hal_miim_read(unit, basePort, page[phyPort], reg[phyPort], &data);
    *pEna = REG32_FIELD_GET(data, 10, (0x1 << 10));

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      _phy_8218d_loopback_remote_set
 * Description:
 *      Get remote loopback status
 * Input:
 *      unit - unit id
 *      port - port id
 *      ena - 1: enable remote loopback; 0: disable remote loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
_phy_8218d_loopback_remote_set(uint32 unit, rtk_port_t port, uint32 ena)
{
    int32 ret = RT_ERR_OK;
    uint32 basePort = 0, phyPort = 0;
    uint32 blk = 0, data;
    uint32 page[PORT_NUM_IN_8218D] = {0x322, 0x322, 0x322, 0x323, 0x324, 0x324, 0x324, 0x325};
    uint32 reg[PORT_NUM_IN_8218D] = {0x10, 0x13, 0x16, 0x11, 0x10, 0x13, 0x16, 0x11};


    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218D)
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    hal_miim_read(unit, basePort, page[phyPort], reg[phyPort], &data);
    if (ena == 0)
    {
        data = REG32_FIELD_SET(data, 0, 10, (0x1 << 10));
    }
    else
    {
        data = REG32_FIELD_SET(data, 1, 10, (0x1 << 10));
    }
    ret = hal_miim_write(unit, basePort, page[phyPort], reg[phyPort], data);

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}

int32
_phy_8218d_interrupt_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  basePort = 0;
    uint32  blk = 0;
    uint32  data = 0;

    /* per-port related init */

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 0), ret);

    /* Disable all interrupt types of interrupt enable status */
    RT_ERR_HDL(hal_miim_write(unit, port, 0xa42, 0x12, 0), ERR, ret);

    /* Clear ISR */
    RT_ERR_HDL(hal_miim_read(unit, port, 0xa43, 0x15, &data), ERR, ret);

    /* Disable RLFD enable (page 0xa44 reg 0x11 bit 15) */
    RT_ERR_HDL(hal_miim_read(unit, port, 0xa44, 0x11, &data), ERR, ret);
    data = REG32_FIELD_SET(data, 0, 15, (0x1 << 15));
    RT_ERR_HDL(hal_miim_write(unit, port, 0xa44, 0x11, data), ERR, ret);

    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, blk), ret);


    /* per-phy related init */

    basePort = HWP_PHY_BASE_MACID(unit, port);
    if (port == basePort)
    {
        RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

        /* Clear Top ISR */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x320, 0x12, &data), ERR, ret);

        RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, blk), ret);
    }

    return ret;

ERR:
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, blk), ret);
    return ret;
}


/* Function Name:
 *      phy_8218d_init
 * Description:
 *      Initialize PHY 8218D.
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
phy_8218d_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port = 0, data0, data1, data2, data;
    int32   ret;
    rt_serdesMode_t sdsMode;


    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get base port ID fail", unit, port);
        return RT_ERR_PORT_ID;
    }


    if (port == base_port)
    {
        if (HWP_UNIT_VALID_LOCAL(unit))
        {
            if ((phy_8218d_macIntfSerdesMode_get(unit, port, &sdsMode) == RT_ERR_OK) && (sdsMode == RTK_MII_XSGMII))
            {
                phy_8218d_macIntfSdsRxCali_start(unit, port);
            }

            if (HWP_PHY_MODEL_BY_PORT(unit, port) != RTK_PHYTYPE_RTL8218D_NMP)
            {
                hal_miim_write(unit, base_port, 0, 27, 0x805D);
                hal_miim_read(unit, base_port, 0, 28, &data0);
                hal_miim_write(unit, base_port, 0, 27, 0x8061);
                hal_miim_read(unit, base_port, 0, 28, &data1);
                hal_miim_write(unit, base_port, 0, 27, 0x804D);
                hal_miim_read(unit, base_port, 0, 28, &data2);
            }

            if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x8)) != RT_ERR_OK)
                return ret;

             /*Disable Serdes Nway*/
            if ((ret = hal_miim_write(unit, base_port, 0x400, 0x10, 0x9703)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, base_port, 0x500, 0x10, 0x9403)) != RT_ERR_OK)
                return ret;

             /* Set default value for phyESD checking in waMon */
            if ((ret = hal_miim_write(unit, base_port, 0x327, 0x10, 0x8218)) != RT_ERR_OK)
                return ret;

            if (HWP_PHY_MODEL_BY_PORT(unit, port) != RTK_PHYTYPE_RTL8218D_NMP)
            {
                data = 0;
                hal_miim_read(unit, base_port, 0x360, 0x10, &data);
                if (data == 0)
                {
                    if ((ret = hal_miim_write(unit, base_port, 0x360, 0x10, data0)) != RT_ERR_OK)
                        return ret;
                }

                data = 0;
                hal_miim_read(unit, base_port, 0x360, 0x11, &data);
                if (data == 0)
                {
                    if ((ret = hal_miim_write(unit, base_port, 0x360, 0x11, data1)) != RT_ERR_OK)
                        return ret;
                }

                data = 0;
                hal_miim_read(unit, base_port, 0x360, 0x12, &data);
                if (data == 0)
                {
                    if ((ret = hal_miim_write(unit, base_port, 0x360, 0x12, data2)) != RT_ERR_OK)
                        return ret;
                }
            }

            if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x0)) != RT_ERR_OK)
                return ret;
        } /* end if (HWP_UNIT_VALID_LOCAL) */
    }

#if defined(CONFIG_SDK_RTL8380)
    /* For 838x & 833x, Set PKTGEN RXPATH is from MAC TX & Enable PKTGEN RX for Phy error counter */
    if (HWP_8380_30_FAMILY(unit))
    {
        if ((ret = hal_miim_read(unit,  port,  0xc80,  16, &data)) != RT_ERR_OK)
            return ret;

        data &= ~0xF;
        data |= 0x1<<2;
        data |= 0x2<<0;
        if ((ret = hal_miim_write(unit,  port,  0xc80,  16, data)) != RT_ERR_OK);
            return ret;
    }
#endif

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        phy_8218d_greenEnable_set(unit, port, DISABLED);

        hal_miim_read(unit, port, 0xa43, 16, &data);
        rtl8218d_info[unit].mdi_val_org[port] = PHY_8218D_MDI_LB_MDI_VAL_GET(data);
    }

    if ((ret = _phy_8218d_interrupt_init(unit, port)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_greenEnable_get
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
 *      1. The RTL8218d is supported the per-port link-up green feature.
 */
int32
phy_8218d_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData, fixed_page, phy_port, base_port, val;

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D_NMP)
    {
        fixed_page = 0;

        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;

        if ((phyData >> 15) & 0x1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;
    }
    else
    {

        base_port = HWP_PHY_BASE_MACID(unit, port);
        phy_port = port - HWP_PHY_BASE_MACID(unit, port);
        if (phy_port >= PORT_NUM_IN_8218D)
        {
            return RT_ERR_PORT_ID;
        }

        if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x8)) != RT_ERR_OK)
            return ret;

        if ((ret = hal_miim_read(unit, base_port, 0x360, 0x13, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x0)) != RT_ERR_OK)
            return ret;

        val = REG32_FIELD_GET(phyData, PHY_8218D_GREEN_EN_PORT_OFFSET(phy_port), PHY_8218D_GREEN_EN_PORT_MASK(phy_port));
        if (val)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

        return RT_ERR_OK;
    }
} /* end of phy_8218d_greenEnable_get */

/* Function Name:
 *      phy_8218d_greenEnable_set
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
 *      1. The RTL8218d is supported the per-port link-up green feature.
 */
int32
phy_8218d_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value, fixed_page, data, data0, data1, data2, phy_port, base_port;
    int32   ret = RT_ERR_FAILED;

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D_NMP)
    {
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
            return ret;

        fixed_page = 0;

        /* get value from CHIP*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &value)) != RT_ERR_OK)
            return ret;

        if (ENABLED == enable)
        {
            value |= (0x1 << 15);
        }
        else
        {
            value &= ~(0x1 << 15);
        }

        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, fixed_page, 28, value)) != RT_ERR_OK)
            return ret;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;
    }
    else
    {
        base_port = HWP_PHY_BASE_MACID(unit, port);
        phy_port = port - base_port;
        if (phy_port >= PORT_NUM_IN_8218D)
        {
            return RT_ERR_PORT_ID;
        }


        if (ENABLED == enable)
        {
            if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x8)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, base_port, 0x360, 0x13, &data)) != RT_ERR_OK)
                return ret;
            data = REG32_FIELD_SET(data, 1, PHY_8218D_GREEN_EN_PORT_OFFSET(phy_port), PHY_8218D_GREEN_EN_PORT_MASK(phy_port));
            if ((ret = hal_miim_write(unit, base_port, 0x360, 0x13, data)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, base_port, 0x360, 0x10, &data0)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, base_port, 0x360, 0x11, &data1)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, base_port, 0x360, 0x12, &data2)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x0)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, port, 0, 27, 0x805D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 28, data0)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x8061)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 28, data1)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x804D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 28, data2)) != RT_ERR_OK)
                return ret;
        }
        else
        {
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x8065)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, 0, 28, &data0)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x8069)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, 0, 28, &data1)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x8055)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, 0, 28, &data2)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, port, 0, 27, 0x805D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 28, data0)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x8061)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 28, data1)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 27, 0x804D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 0, 28, data2)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x8)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, base_port, 0x360, 0x13, &data)) != RT_ERR_OK)
                return ret;
            data = REG32_FIELD_SET(data, 0, PHY_8218D_GREEN_EN_PORT_OFFSET(phy_port), PHY_8218D_GREEN_EN_PORT_MASK(phy_port));
            if ((ret = hal_miim_write(unit, base_port, 0x360, 0x13, data)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, base_port, 0, 30, 0x0)) != RT_ERR_OK)
                return ret;
        }

        return RT_ERR_OK;
    }
} /* end of phy_8218d_greenEnable_set */


#if !defined(__BOOTLOADER__)

/* Function Name:
 *      phy_8218d_media_get
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
 *      The media type is return PORT_MEDIA_COPPER.
 */
int32
phy_8218d_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
} /* end of phy_8218d_media_get */


/* Function Name:
 *      phy_8218d_autoNegoAbility_get
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
phy_8218d_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
    pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
    pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;

    return ret;
} /* end of phy_8218d_autoNegoAbility_get */

/* Function Name:
 *      phy_8218d_autoNegoAbility_set
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
phy_8218d_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
            | (pAbility->FC << Pause_R4_OFFSET)
            | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 &
            ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
    phyData4 = phyData4
            | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
            | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
            | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
            | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

    phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
    phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
               | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
        return ret;


    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8218d_autoNegoAbility_set */

/* Function Name:
 *      phy_8218d_speed_get
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
phy_8218d_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret;
    uint32  val;
    uint32  phyData0;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
              | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

    return ret;
} /* end of phy_8218d_speed_get */

/* Function Name:
 *      phy_8218d_speed_set
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
phy_8218d_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218d_speed_set */


/* Function Name:
 *      phy_8218d_rtctResult_get
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
 *      The result length unit is cm.
 *      8218D does not support LineDriver.
 */
int32
phy_8218d_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, fixed_page;
    uint32  speed;


    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D_NMP)
    {
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);

    if (phyData & LinkStatus_MASK)
    {
        if (speed == PORT_SPEED_10M)
        {
            return RT_ERR_PORT_NOT_SUPPORTED;
        }
        /* If the port link is up,
         * return cable length from Channel Estimation
         */

        /* The Length is store in [7:0], and the unit is meter*/
        if ((ret = hal_miim_read(unit, port, 0xa88, 16, &phyData)) != RT_ERR_OK)
            return ret;

        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        if (speed == PORT_SPEED_1000M) /* 1000M */
        {
            pRtctResult->linkType = PORT_SPEED_1000M;
            pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
        }
        else /* 100M */
        {
            pRtctResult->linkType = PORT_SPEED_100M;
            pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
            pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
        }
    }
    else
    {
        fixed_page = 0xa42;
        if ((ret = hal_miim_read(unit, port, fixed_page, 17, &phyData)) != RT_ERR_OK)
            return ret;

        if(((phyData >> PHY_8218D_RTCT_DONE) & 0x1) != 0x1)
            return RT_ERR_PHY_RTCT_NOT_FINISH;

        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        pRtctResult->linkType = PORT_SPEED_1000M;

        if ((ret = _phy_8218d_indr_read(unit, port, 0x802d, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelALen = PHY_8218D_RTCT_LENGTH(phyData);

        if ((ret = _phy_8218d_indr_read(unit, port, 0x8031, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelBLen = PHY_8218D_RTCT_LENGTH(phyData);

        if ((ret = _phy_8218d_indr_read(unit, port, 0x8035, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelCLen = PHY_8218D_RTCT_LENGTH(phyData);

        if ((ret = _phy_8218d_indr_read(unit, port, 0x8039, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelDLen = PHY_8218D_RTCT_LENGTH(phyData);

        /* === Channel A Status ===*/
        if ((ret = _phy_8218d_indr_read(unit, port, 0x802b, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218d_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelAShort, &pRtctResult->un.ge_result.channelAOpen,
                                &pRtctResult->un.ge_result.channelAMismatch, &pRtctResult->un.ge_result.channelALinedriver,
                                &pRtctResult->un.ge_result.channelAHiImpedance, &pRtctResult->un.ge_result.channelACross,
                                &pRtctResult->un.ge_result.channelAPartialCross, &pRtctResult->un.ge_result.channelAPairBusy)) != RT_ERR_OK)
            return ret;

        /* === Channel B Status ===*/
        if ((ret = _phy_8218d_indr_read(unit, port, 0x802f, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218d_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelBShort, &pRtctResult->un.ge_result.channelBOpen,
                                &pRtctResult->un.ge_result.channelBMismatch, &pRtctResult->un.ge_result.channelBLinedriver,
                                &pRtctResult->un.ge_result.channelBHiImpedance, &pRtctResult->un.ge_result.channelBCross,
                                &pRtctResult->un.ge_result.channelBPartialCross, &pRtctResult->un.ge_result.channelBPairBusy)) != RT_ERR_OK)
            return ret;

        /* === Channel C Status ===*/
        if ((ret = _phy_8218d_indr_read(unit, port, 0x8033, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218d_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelCShort, &pRtctResult->un.ge_result.channelCOpen,
                                &pRtctResult->un.ge_result.channelCMismatch, &pRtctResult->un.ge_result.channelCLinedriver,
                                &pRtctResult->un.ge_result.channelCHiImpedance, &pRtctResult->un.ge_result.channelCCross,
                                &pRtctResult->un.ge_result.channelCPartialCross, &pRtctResult->un.ge_result.channelCPairBusy)) != RT_ERR_OK)
            return ret;

        /* === Channel D Status ===*/
        if ((ret = _phy_8218d_indr_read(unit, port, 0x8037, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218d_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelDShort, &pRtctResult->un.ge_result.channelDOpen,
                                &pRtctResult->un.ge_result.channelDMismatch, &pRtctResult->un.ge_result.channelDLinedriver,
                                &pRtctResult->un.ge_result.channelDHiImpedance, &pRtctResult->un.ge_result.channelDCross,
                                &pRtctResult->un.ge_result.channelDPartialCross, &pRtctResult->un.ge_result.channelDPairBusy)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_rtct_start
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
phy_8218d_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;
    uint32  speed;

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D_NMP)
    {
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);

    if (phyData & LinkStatus_MASK)
    {
        if (speed == PORT_SPEED_10M)
        {
            return RT_ERR_PORT_NOT_SUPPORTED;
        }

        return RT_ERR_OK;
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, 0xa42, 17, &phyData)) != RT_ERR_OK)
            return ret;
        //clear RTCT_rdy_bit
        phyData = REG32_FIELD_SET(phyData, 0, PHY_8218D_RTCT_DONE, (0x1 << PHY_8218D_RTCT_DONE));
        if ((ret = hal_miim_write(unit, port, 0xa42, 17, phyData)) != RT_ERR_OK)
            return ret;

        osal_time_udelay(1 * 1000); /* wait 1ms for chip reset the states */

        phyData &= ~ (1 << PHY_8218D_RTCT_RSV);
        phyData |= (1 << PHY_8218D_RTCT_CH_A);
        phyData |= (1 << PHY_8218D_RTCT_CH_B);
        phyData |= (1 << PHY_8218D_RTCT_CH_C);
        phyData |= (1 << PHY_8218D_RTCT_CH_D);
        phyData |= (1 << PHY_8218D_RTCT_ENABLE);

        if ((ret = hal_miim_write(unit, port, 0xa42, 17, phyData)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_eeeEnable_get
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
phy_8218d_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData, val;

    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
        return ret;


    if ((ret = hal_miim_mmd_read(unit, port, 7, 60, &phyData)) != RT_ERR_OK)
    {
        return ret;
    }

    val = REG32_FIELD_GET(phyData, 2, (0x1 << 2)); /* 1000M EEE ability */
    if (val)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218d_eeeEnable_get */

/* Function Name:
 *      phy_8218_eeeEnable_set
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
phy_8218d_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData, val;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t     an_enable;


    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
        return ret;

    phy_common_autoNegoEnable_get(unit, port, &an_enable);

    if ((ret = hal_miim_mmd_read(unit, port, 7, 60, &phyData)) != RT_ERR_OK)
    {
        return ret;
    }

    if (enable == ENABLED)
        val = 1;
    else
        val = 0;

    phyData = REG32_FIELD_SET(phyData, val, 2, (0x1 << 2)); /* 1000M EEE ability */
    phyData = REG32_FIELD_SET(phyData, val, 1, (0x1 << 1)); /* 100M EEE ability */
    ret = hal_miim_mmd_write(unit, port, 7, 60, phyData);
    if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        return ret;
    }

    if ((ret = hal_miim_read(unit, port, 0xa42, 20, &phyData)) != RT_ERR_OK)
    {
        return ret;
    }

    phyData = REG32_FIELD_SET(phyData, val, 7, (0x1 << 7)); /* 500M EEE ability */
    /* the enable status will depending on the chip support 500M or not */
    if ((ret = hal_miim_write(unit, port, 0xa42, 20, phyData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == an_enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phyData = REG32_FIELD_SET(phyData, 1, RestartAutoNegotiation_OFFSET, RestartAutoNegotiation_MASK);
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }

    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218d_eeeEnable_set */

/* Function Name:
 *      phy_8218d_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8218d_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, force_mode, mdi;

    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &phyData)) != RT_ERR_OK)
        return ret;

    if((phyData & (1 << 9)) != 0)
    {
        force_mode = 1;
        if((phyData & (1 << 8)) != 0)
            mdi = 1;
        else
            mdi = 0;
    }else{
        force_mode = 0;
    }

    if (force_mode)
    {
        if (mdi)
            *pMode = PORT_CROSSOVER_MODE_MDI;
        else
            *pMode = PORT_CROSSOVER_MODE_MDIX;
    }
    else
        *pMode = PORT_CROSSOVER_MODE_AUTO;

    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218d_crossOverMode_get */

/* Function Name:
 *      phy_8218d_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8218d_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;


    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &phyData)) != RT_ERR_OK)
        return ret;

    switch (mode)
    {
        case PORT_CROSSOVER_MODE_AUTO:
            phyData &= ~(1 << 9);
            break;
        case PORT_CROSSOVER_MODE_MDI:
            phyData |= (1 << 9);
            phyData |= (1 << 8);
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            phyData |= (1 << 9);
            phyData &= ~(1 << 8);
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_write(unit, port, 0xa43, 24, phyData)) != RT_ERR_OK)
        return ret;

    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218_crossOverMode_set */

/* Function Name:
 *      phy_8218d_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8218d_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;

    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0001)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 26, &phyData)) != RT_ERR_OK)
        return ret;

    if((phyData & (1 << 1)) != 0)
    {
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
    }else{
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;
    }

    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218d_crossOverStatus_get */

/* Function Name:
 *      phy_8218d_linkDownPowerSavingEnable_get
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
 *      1. The RTL8218d is supported the per-port link-down power saving
 */
int32
phy_8218d_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    /* For Link-Down Power Saving (per-port) */
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &value)) != RT_ERR_OK)
        return ret;

    if (((value >> 2) & 0x1) == 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
    return RT_ERR_OK;
} /* end of phy_8218d_linkDownPowerSavingEnable_get */

/* Function Name:
 *      phy_8218d_linkDownPowerSavingEnable_set
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
 *      1. The RTL8218d is supported the per-port link-down power saving
 */
int32
phy_8218d_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    /* For Link-Down Power Saving (per-port) */
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &value)) != RT_ERR_OK)
        return ret;

    value &= ~(0x1 << 2);
    if (ENABLED == enable)
    {
        value |= (0x1 << 2);
    }
    if ((ret = hal_miim_write(unit, port, 0xa43, 24, value)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218d_linkDownPowerSavingEnable_set */

/* Function Name:
 *      phy_8218d_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
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
phy_8218d_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  base_port = 0;
    uint32  phyData;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 30, 0x0008)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 31, 0x0266)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, base_port, 0xa42, 22, &phyData)) != RT_ERR_OK)
        return ret;
    if (enable)
        phyData |= (0x1 << (port-base_port+8));
    else
        phyData &= ~(0x1 << (port-base_port+8));
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 22, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218d_broadcastEnable_set */

/* Function Name:
 *      phy_8218d_broadcastID_set
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
phy_8218d_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID)
{
    int32   ret;
    uint32  base_port = 0;
    uint32  phyData;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 30, 0x0008)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 31, 0x0266)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, base_port, 0xa42, 22, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1f);
    phyData |= (broadcastID & 0x1f);
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 22, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 30, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218d_broadcastID_set */

/* Function Name:
 *      phy_8218d_gigaLiteEnable_get
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
 *      1. The RTL8218d is supported the per-port Giga Lite feature.
 */
int32
phy_8218d_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    /* Configure the GPHY page to copper */
   if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0001)) != RT_ERR_OK)
        return ret;

    /* For Giga Lite (per-port) */
   if ((ret = hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &value)) != RT_ERR_OK)
       return ret;

   if (((value >> 9) & 0x1) == 0x1)
       *pEnable = ENABLED;
   else
       *pEnable = DISABLED;

    /* Configure the GPHY page to auto */
   if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0000)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
} /* end of phy_8218d_gigaLiteEnable_get */

/* Function Name:
 *      phy_8218d_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218d is supported the per-port Giga Lite feature.
 */
int32
phy_8218d_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;


    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0001)) != RT_ERR_OK)
        return ret;

    /* bit[2]: configure 2-pair auto-downspeed */
    /* bit[9]: configure retry speed down to 500M */
    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &value)) != RT_ERR_OK)
        return ret;

    value &= ~(0x1 << 9);
    value &= ~(0x1 << 2);
    if (ENABLED == enable)
    {
        value |= (0x1 << 9);
        value |= (0x1 << 2);
    }
    if ((ret = hal_miim_write(unit, port, 0xa44, 17, value)) != RT_ERR_OK)
        return ret;


    /* For Giga Lite (per-port) */
    if ((ret = hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &value)) != RT_ERR_OK)
        return ret;

    value &= ~(0x1 << 9);
    if (ENABLED == enable)
    {
        value |= (0x1 << 9);
    }

    if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 20, value)) != RT_ERR_OK)
        return ret;

    /* Do the restart N-WAY */
    if ((ret = hal_miim_read(unit, port, 0, 0, &value)) != RT_ERR_OK)
        return ret;
    value |= (0x1UL<<9);
    if ((ret = hal_miim_write(unit, port, 0, 0, value)) != RT_ERR_OK)
        return ret;

    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0000)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
} /* end of phy_8218d_gigaLiteEnable_set */

/* Function Name:
 *      phy_8218d_downSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
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
phy_8218d_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 30, 1)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &phyData)) != RT_ERR_OK)
        goto ERR;

    if (phyData & (1 << 3))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = hal_miim_write(unit, port, 0, 30, 0)) != RT_ERR_OK)
        return ret;
    return ret;
ERR:
    hal_miim_write(unit, port, 0, 30, 0);
    return ret;
}

/* Function Name:
 *      phy_8218d_downSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
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
phy_8218d_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 30, 1)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &phyData)) != RT_ERR_OK)
        goto ERR;

    /* DownSpeed to 100M*/
    phyData &= ~(1 << 5);

    if (ENABLED == enable)
        phyData |= (1 << 3);
    else
        phyData &= ~(1 << 3);

    if ((ret = hal_miim_write(unit, port, 0xa44, 17, phyData)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, port, 0, 30, 0)) != RT_ERR_OK)
        return ret;

    return ret;
ERR:
    hal_miim_write(unit, port, 0, 30, 0);
    return ret;
}


/* Function Name:
 *      phy_8218d_downSpeedStatus_get
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
phy_8218d_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_read(unit, port, 0xa4b, 16, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & (1 << 12))
        *pDownSpeedStatus = TRUE;
    else
        *pDownSpeedStatus = FALSE;

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_ptpReg_access_start
 * Description:
 *      Prepare to do PTP register access for the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 * Note:
 *      None
 */
static int32
_phy_8218d_ptpReg_access_start(uint32 unit, rtk_port_t port)
{
    int32 ret;
    rtk_port_t port_base;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);


    if ((port_base = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_write(unit, port_base, 0, 30, 8)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_ptpReg_access_end
 * Description:
 *      Finish PTP register access for the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 * Note:
 *      None
 */
static int32
_phy_8218d_ptpReg_access_end(uint32 unit, rtk_port_t port)
{
    int32 ret;
    rtk_port_t port_base;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d",
           unit, port);


    if ((port_base = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_write(unit, port_base, 0, 30, 0)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218d_ptpReg_get
 * Description:
 *      Get PTP register data of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      reg_addr            - reg address
 * Output:
 *      pData               - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_phy_8218d_ptpReg_get(uint32 unit, rtk_port_t port, uint32 reg_addr, uint32 *pData)
{
    int32 ret;
    rtk_port_t port_base;
    uint32 page, reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, reg_addr=0x%x, reg=0x%x",
           unit, port, reg_addr);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if ((port_base = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    page = (reg_addr - (reg_addr % 8)) / 8;
    reg = ((reg_addr - REG_8218D_PTP_BASE) % 8) + 16;


    if ((ret = hal_miim_read(unit, port_base, page, reg, pData)) != RT_ERR_OK)
    {
        return ret;
    }


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pData=0x%x", *pData);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_ptpReg_set
 * Description:
 *      Set PTP register data of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      reg_addr           - reg address
 *      reg_val            - reg value
 *      reg_msk            - reg value mask
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
_phy_8218d_ptpReg_set(uint32 unit, uint32 port, uint32 reg_addr, uint32 reg_val, uint32 reg_msk)
{
    int32 ret;
    rtk_port_t port_base;
    uint32 page, reg, ori_val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, reg_addr=0x%x, reg_val=0x%x, reg_msk=0x%x",
            unit, port, reg_addr, reg_val, reg_msk);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if ((port_base = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    page = (reg_addr - (reg_addr % 8)) / 8;
    reg = ((reg_addr - REG_8218D_PTP_BASE) % 8) + 16;


    if ((reg_msk & 0xFFFF) != 0xFFFF)
    {
        if ((ret = hal_miim_read(unit, port_base, page, reg, &ori_val)) != RT_ERR_OK)
        {
            return ret;
        }

        reg_val = (ori_val & ~reg_msk) | (reg_val & reg_msk);
    }
    if ((ret = hal_miim_write(unit, port_base, page, reg, reg_val)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_ptpSwitchMacAddr_get
 * Description:
 *      Get the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
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
phy_8218d_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    int32 ret;
    uint32 mac_h, mac_m, mac_l;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_MAC_ADDR_H, &mac_h)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_MAC_ADDR_M, &mac_m)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_MAC_ADDR_L, &mac_l)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    pSwitchMacAddr->octet[0] = (uint8)(mac_h >> 8);
    pSwitchMacAddr->octet[1] = (uint8)(mac_h & 0xFF);
    pSwitchMacAddr->octet[2] = (uint8)(mac_m >> 8);
    pSwitchMacAddr->octet[3] = (uint8)(mac_m & 0xFF);
    pSwitchMacAddr->octet[4] = (uint8)(mac_l >> 8);
    pSwitchMacAddr->octet[5] = (uint8)(mac_l & 0xFF);

    return ret;
}

/* Function Name:
 *      phy_8218d_ptpSwitchMacAddr_set
 * Description:
 *      Set the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
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
phy_8218d_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    int32 ret;
    uint32 mac_h, mac_m, mac_l;

    mac_h = (pSwitchMacAddr->octet[0] << 8) | (pSwitchMacAddr->octet[1]);
    mac_m = (pSwitchMacAddr->octet[2] << 8) | (pSwitchMacAddr->octet[3]);
    mac_l = (pSwitchMacAddr->octet[4] << 8) | (pSwitchMacAddr->octet[5]);


    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_MAC_ADDR_H, mac_h, 0xFFFF)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_MAC_ADDR_M, mac_m, 0xFFFF)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_MAC_ADDR_L, mac_l, 0xFFFF)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpRefTime_get
 * Description:
 *      Get the reference time of PHY of the specified port.
 * Input:
 *      unit       - unit id
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpRefTime_get(uint32 unit, rtk_port_t port,
    rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 sec_l, sec_m, sec_h, nsec_l, nsec_h;
    uint32 reg_val, reg_msk;

    /* execute reading command, [15] = 0x1 executing, [13:12] = 0x0 read */
    reg_val = REG_FIELD_8218D_PTP_CMD_OP_READ;
    reg_msk = REG_FIELD_8218D_PTP_CMD_OP_MSK;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_CFG_0, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_val = REG_FIELD_8218D_PTP_CMD_EXEC;
    reg_msk = REG_FIELD_8218D_PTP_CMD_EXEC;

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_NSEC_H, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    do {
        if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_H, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & REG_FIELD_8218D_PTP_CMD_EXEC); /* busy watting */

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_L_RO, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_H_RO, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_SEC_L_RO, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_SEC_M_RO, &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_SEC_H_RO, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    pTimeStamp->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pTimeStamp->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpRefTime_set
 * Description:
 *      Set the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218d_ptpRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret;
    uint32 sec_l, sec_m, sec_h, nsec_l, nsec_h;
    uint32 reg_val, reg_msk;

    /* adjust Timer of PHY */
    sec_l = (timeStamp.sec) & 0xFFFF;
    sec_m = ((timeStamp.sec) >> 16) & 0xFFFF;
    sec_h = ((timeStamp.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = timeStamp.nsec & 0xFFFF;
    nsec_h = (timeStamp.nsec >> 16) & 0x3FFF;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_SEC_L, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_SEC_M, sec_m, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_SEC_H, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_NSEC_L, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_NSEC_H, nsec_h, 0x3FFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_val = REG_FIELD_8218D_PTP_CMD_OP_WRITE;
    reg_msk = REG_FIELD_8218D_PTP_CMD_OP_MSK;

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_CFG_0, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if(exec)
    {
        reg_val = REG_FIELD_8218D_PTP_CMD_EXEC;
        reg_msk = REG_FIELD_8218D_PTP_CMD_EXEC;

        if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_NSEC_H, reg_val, reg_msk)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }
    }

    /* busy watting */
    do {
        if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_H, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & REG_FIELD_8218D_PTP_CMD_EXEC);

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      port    - port id
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
phy_8218d_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret;
    uint32 sec_l, sec_m, sec_h, nsec_l, nsec_h;
    uint32 reg_val, reg_msk;

    /* adjust Timer of PHY */
    sec_l = (timeStamp.sec) & 0xFFFF;
    sec_m = ((timeStamp.sec) >> 16) & 0xFFFF;
    sec_h = ((timeStamp.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = (timeStamp.nsec) & 0xFFFF;
    nsec_h = ((timeStamp.nsec) >> 16) & 0x07FF;

    if(exec)
        nsec_h = nsec_h | REG_FIELD_8218D_PTP_CMD_EXEC;

    if (sign == 0)
    {
        reg_val = REG_FIELD_8218D_PTP_CMD_OP_ADJ_INC;
    } else {
        reg_val = REG_FIELD_8218D_PTP_CMD_OP_ADJ_DEC;
    }

    reg_msk = REG_FIELD_8218D_PTP_CMD_OP_MSK;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_CFG_0, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_SEC_L, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_SEC_M, sec_m, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_SEC_H, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_NSEC_L, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_NSEC_H, nsec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    /* busy watting */
    do {
        if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_H, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & REG_FIELD_8218D_PTP_CMD_EXEC);

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpRefTimeEnable_get
 * Description:
 *      Get the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 reg_addr, reg_val, reg_msk;

    reg_addr = REG_8218D_PTP_TIME_CFG_0;
    reg_msk = (REG_FIELD_8218D_PTP_CFG_TIMER_EN_FRC | REG_FIELD_8218D_PTP_CFG_TIMER_1588_EN);

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if (reg_val & reg_msk)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port    - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218d_ptpRefTimeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val, reg_msk;

    reg_addr = REG_8218D_PTP_TIME_CFG_0;
    reg_val = (enable == ENABLED)? \
        (REG_FIELD_8218D_PTP_CFG_TIMER_EN_FRC | REG_FIELD_8218D_PTP_CFG_TIMER_1588_EN) : 0;
    reg_msk = (REG_FIELD_8218D_PTP_CFG_TIMER_EN_FRC | REG_FIELD_8218D_PTP_CFG_TIMER_1588_EN);

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpRefTimeFreq_get
 * Description:
 *      Get the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pFreq  - pointer to reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
int32
phy_8218d_ptpRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_FREQ_LSB;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    *pFreq = reg_val;

    reg_addr = REG_8218D_PTP_FREQ_MSB;

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    *pFreq |= reg_val << 16;

    return RT_ERR_OK;
} /* end of phy_8218d_ptpRefTimeFreq_get */

/* Function Name:
 *      phy_8218d_ptpRefTimeFreq_set
 * Description:
 *      Set the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      freq   - reference time frequency
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
int32
phy_8218d_ptpRefTimeFreq_set(uint32 unit, rtk_port_t port, uint32 freq)
{
    int32 ret;
    uint32 reg_addr;

    reg_addr = REG_8218D_PTP_FREQ_LSB;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, freq, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_addr = REG_8218D_PTP_FREQ_MSB;

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, (freq>>16), 0x0FFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8218d_ptpRefTimeFreq_set */

/* Function Name:
 *      phy_8218d_ptpEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    if (reg_val & REG_FIELD_8218D_PTP_PHY_UDP_EN)
    {
        *pEnable = ENABLED;
    } else {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
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
phy_8218d_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);
    reg_val = (enable == ENABLED)? REG_FIELD_8218D_PTP_PHY_UDP_EN |REG_FIELD_8218D_PTP_PHY_ETH_EN  : 0;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, REG_FIELD_8218D_PTP_PHY_UDP_EN | REG_FIELD_8218D_PTP_PHY_ETH_EN)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpInterruptStatus_get
 * Description:
 *      Get PTP interrupt status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pIsPortIntr - port interrupt triggered status
 *      pIntrSts - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIsPortIntr, uint32 *pIntrSts)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_INT_STS_P;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if (reg_val & (1 << (port % 8)))
    {
        *pIsPortIntr = ENABLED;
    } else {
        *pIsPortIntr = DISABLED;
    }

    reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    *pIntrSts = reg_val & 0xFF;

    /*clear the interrupt*/
    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, 0xFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpInterruptEnable_get
 * Description:
 *      Get PTP interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CFG_1;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    if (reg_val & 0xFF)
    {
        *pEnable = ENABLED;
    } else {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpInterruptEnable_set
 * Description:
 *      Set PTP interrupt enable status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
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
phy_8218d_ptpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CFG_1;
    reg_val = (enable == ENABLED)? 0xFF  : 0;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, 0xFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_ptpRxTimestamp_get
 * Description:
 *      Get PTP Rx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpRxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 reg_addr, reg_val;
    uint32 sec_l, sec_m, sec_h, nsec_l, nsec_h;

    reg_addr = REG_8218D_PTP_TIME_RX_SID_PN + \
               REG_8218D_PTP_OFFSET_PORT(port) + \
               REG_8218D_PTP_OFFSET_MSGTYPE(identifier.msgType);

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (reg_val != identifier.sequenceId)
    {
        reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);
        reg_val = 0x1 << (4 + REG_8218D_PTP_OFFSET_MSGTYPE(identifier.msgType));
        if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }

        return RT_ERR_INPUT;
    }


    /* Get Rx Timestamp */
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_SEC_L_PN + REG_8218D_PTP_OFFSET_PORT(port), &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_SEC_M_PN + REG_8218D_PTP_OFFSET_PORT(port), &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_SEC_H_PN + REG_8218D_PTP_OFFSET_PORT(port), &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_NSEC_L_PN + REG_8218D_PTP_OFFSET_PORT(port), &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_NSEC_H_PN + REG_8218D_PTP_OFFSET_PORT(port), &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    /* Clear the pending status */
    reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);
    reg_val = 0x1 << (4 + REG_8218D_PTP_OFFSET_MSGTYPE(identifier.msgType));
    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    pTimeStamp->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pTimeStamp->nsec = ((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpTxTimestamp_get
 * Description:
 *      Get PTP Tx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpTxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 reg_addr, reg_val;
    uint32 sec_l, sec_m, sec_h, nsec_l, nsec_h;

    reg_addr = REG_8218D_PTP_TIME_TX_SID_PN + \
               REG_8218D_PTP_OFFSET_PORT(port) + \
               REG_8218D_PTP_OFFSET_MSGTYPE(identifier.msgType);

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (reg_val != identifier.sequenceId)
    {
        /* Clear the pending status */
        reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);
        reg_val = 0x1 << (REG_8218D_PTP_OFFSET_MSGTYPE(identifier.msgType));
        if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }

        return RT_ERR_NOT_FINISH;
    }

    /* Get Tx Timestamp */
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_SEC_L_PN + REG_8218D_PTP_OFFSET_PORT(port), &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_SEC_M_PN + REG_8218D_PTP_OFFSET_PORT(port), &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_SEC_H_PN + REG_8218D_PTP_OFFSET_PORT(port), &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_NSEC_L_PN + REG_8218D_PTP_OFFSET_PORT(port), &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, \
        REG_8218D_PTP_TIME_NSEC_H_PN + REG_8218D_PTP_OFFSET_PORT(port), &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    /* Clear the pending status */
    reg_addr = REG_8218D_PTP_TIME_CTRL_PN + REG_8218D_PTP_OFFSET_PORT(port);
    reg_val = 0x1 << (REG_8218D_PTP_OFFSET_MSGTYPE(identifier.msgType));
    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    pTimeStamp->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pTimeStamp->nsec = ((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpIgrTpid_get
 * Description:
 *      Get inner/outer TPID of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type  -  vlan type
 *      tpid_idx - TPID index
 * Output:
 *      pTpid   - TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218d_ptpIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    if (INNER_VLAN == type)
    {
        switch(tpid_idx)
        {
            case 0:
                reg_addr = REG_8218D_PTP_ITAG_TPID;
                break;
            case 1:
                reg_addr = REG_8218D_PTP_ITAG_TPID_1;
                break;
            case 2:
                reg_addr = REG_8218D_PTP_ITAG_TPID_2;
                break;
            case 3:
                reg_addr = REG_8218D_PTP_ITAG_TPID_3;
                break;
           default:
                reg_addr = REG_8218D_PTP_ITAG_TPID;
                break;
        }
    }
    else
    {
        switch(tpid_idx)
        {
            case 0:
                reg_addr = REG_8218D_PTP_OTAG_TPID;
                break;
            case 1:
                reg_addr = REG_8218D_PTP_OTAG_TPID_1;
                break;
            case 2:
                reg_addr = REG_8218D_PTP_OTAG_TPID_2;
                break;
            case 3:
                reg_addr = REG_8218D_PTP_OTAG_TPID_3;
                break;
           default:
                reg_addr = REG_8218D_PTP_OTAG_TPID;
                break;
        }
    }

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    *pTpid = reg_val;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpIgrTpid_set
 * Description:
 *      Set inner/outer TPID of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type  -  vlan type
 *      tpid_idx - TPID index
 *      tpid   - TPID
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
extern int32
phy_8218d_ptpIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    if (INNER_VLAN == type)
    {
        switch(tpid_idx)
        {
            case 0:
                reg_addr = REG_8218D_PTP_ITAG_TPID;
                break;
            case 1:
                reg_addr = REG_8218D_PTP_ITAG_TPID_1;
                break;
            case 2:
                reg_addr = REG_8218D_PTP_ITAG_TPID_2;
                break;
            case 3:
                reg_addr = REG_8218D_PTP_ITAG_TPID_3;
                break;
           default:
                reg_addr = REG_8218D_PTP_ITAG_TPID;
                break;
        }
    }
    else
    {
        switch(tpid_idx)
        {
            case 0:
                reg_addr = REG_8218D_PTP_OTAG_TPID;
                break;
            case 1:
                reg_addr = REG_8218D_PTP_OTAG_TPID_1;
                break;
            case 2:
                reg_addr = REG_8218D_PTP_OTAG_TPID_2;
                break;
            case 3:
                reg_addr = REG_8218D_PTP_OTAG_TPID_3;
                break;
           default:
                reg_addr = REG_8218D_PTP_OTAG_TPID;
                break;
        }
    }

    reg_val = tpid;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpSwitchMacRange_get
 * Description:
 *      Get MAC address range of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pRange - pointer to MAC address range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpSwitchMacRange_get(uint32 unit, rtk_port_t port, uint32 *pRange)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CFG_0;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    *pRange =  (reg_val >> 6) & 0x3FF;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpSwitchMacRange_set
 * Description:
 *      Set MAC address range of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pMac  -  pointer to MAC address
 *      range - MAC address range
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
phy_8218d_ptpSwitchMacRange_set(uint32 unit, rtk_port_t port, uint32 range)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CFG_0;

    reg_val =  range << 6;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, REG_FIELD_8218D_PTP_MAC_RNG_MSK)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpOper_get
 * Description:
 *      Get the PTP time operation configuration of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    int32 ret;
    uint32 reg_addr, reg_val;
    uint32 oper;

    reg_addr = REG_8218D_PTP_TIME_CFG_1;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    oper = (reg_val >> 14) & 0x3;
    switch(oper)
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
        default:
            return RT_ERR_FAILED;
    }

    pOperCfg->rise_tri = (reg_val & REG_FIELD_8218D_PTP_CFG_GPI_RISE_TRI) >> 13;
    pOperCfg->fall_tri = (reg_val & REG_FIELD_8218D_PTP_CFG_GPI_FALL_TRI) >> 12;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpOper_set
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218D_PTP_TIME_CFG_1;
    reg_val = 0;
    switch(pOperCfg->oper)
    {
        case TIME_OPER_START:
            reg_val = 0 << 14;
            break;
        case TIME_OPER_LATCH:
            reg_val = 1 << 14;
            break;
        case TIME_OPER_STOP:
            reg_val = 2 << 14;
            break;
        default:
            return RT_ERR_FAILED;
    }

    reg_val |= (pOperCfg->rise_tri == ENABLED)? REG_FIELD_8218D_PTP_CFG_GPI_RISE_TRI  : 0;
    reg_val |= (pOperCfg->fall_tri == ENABLED)? REG_FIELD_8218D_PTP_CFG_GPI_FALL_TRI  : 0;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, 0xF << 12)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if(TIME_OPER_LATCH == pOperCfg->oper)
    {
        reg_val = REG_FIELD_8218D_PTP_CFG_TIMER_RD_SRC_GPI;
        if ((ret = _phy_8218d_ptpReg_set(unit, port, reg_addr, reg_val, REG_FIELD_8218D_PTP_CFG_TIMER_RD_SRC_GPI)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            _phy_8218d_ptpReg_access_end(unit, port);
            return ret;
        }
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_ptpLatchTime_get
 * Description:
 *      Get the PTP latched time of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218d_ptpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime)
{
     int32 ret;
    uint32 sec_l, sec_m, sec_h, nsec_l, nsec_h;

    if ((ret = _phy_8218d_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_L_RO, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_NSEC_H_RO, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_SEC_L_RO, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_SEC_M_RO, &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_get(unit, port, REG_8218D_PTP_TIME_SEC_H_RO, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_set(unit, port, REG_8218D_PTP_TIME_CFG_1, 0, REG_FIELD_8218D_PTP_CFG_TIMER_RD_SRC_GPI)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        _phy_8218d_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218d_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    pLatchTime->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pLatchTime->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218d_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8218d_patch_set(unit, port);
    return ret;
} /* end of phy_8218d_patch_set */

#endif /* !defined(__BOOTLOADER__) */

int32 _phy_8218d_powerSavingMode_leave(uint32 unit, rtk_port_t port)
{
    int32  ret, wait_cnt = 0;
    uint32 phyData;
    uint32 org_phyData;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &org_phyData)) != RT_ERR_OK)
        return ret;

    phyData = org_phyData | (Reset_MASK);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
        return ret;

    while(1)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
            return ret;
        if((phyData & Reset_MASK) == 0)
        {
            break;
        }
        wait_cnt++;
        if (wait_cnt > 500)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, enable reset expire", unit, port);
            break;
        }
    }

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, org_phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}
/* Function Name:
 *      phy_8218d_enable_set
 * Description:
 *      Set PHY interface status of the specific port
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
phy_8218d_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    if (ENABLED == enable)
    {
        if ((ret = _phy_8218d_powerSavingMode_leave(unit, port)) != RT_ERR_OK)
              return ret;
    }

    if ((ret = phy_common_enable_set(unit, port, enable)) != RT_ERR_OK)
        return ret;


    return RT_ERR_OK;
} /* end of phy_common_enable_set */

void
_phy_8218d_ieeeTestModeAllPhyPort_disable(uint32 unit, rtk_port_t phyBasePort, uint32 phyPortCnt)
{
    uint32      phyPort;

    for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
    {
        hal_miim_write(unit, phyBasePort+phyPort,0xA41,9,0x0E00);  // Disable Test mode
    }
}

/* Function Name:
 *      phy_8218d_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS only support in mode 4
 */
int32
phy_8218d_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    uint32      phyBasePort = HWP_PHY_BASE_MACID(unit, port);
    uint32      phyPortCnt = 0;
    uint32      val, chn;

    osal_printf("RTL8218D Test Mode (PHYID: %u)\n", port);

    if ((pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS) && (pTestMode->mode != 4))
    {
        osal_printf("mode %u not support all-phy-port operation.\n", pTestMode->mode);
        return RT_ERR_NOT_ALLOWED;
    }

    switch (HWP_PHY_MODEL_BY_PORT(unit, port))
    {
      case RTK_PHYTYPE_RTL8218D:
      case RTK_PHYTYPE_RTL8218D_NMP:
        phyPortCnt = PORT_NUM_IN_8218D;
        break;
      default:
        osal_printf("port type not support.\n");
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    switch (pTestMode->mode)
    {
      case 1:
        {
            _phy_8218d_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            hal_miim_write(unit, port,0xA41,9,0x2E00); // Test mode 1
            return RT_ERR_OK;
        }
        break;

      case 4:
        {
            uint32      phyPort;

            //0=A,1=B,2=C,3=D
            chn = pTestMode->channel - PHY_TESTMODE_CHANNEL_A;
            if (chn > 3)
            {
                osal_printf(" Error test channel: %u\n", chn);
                return RT_ERR_INPUT;
            }

            osal_printf(" test channel: %u\n", chn);

            if (!(pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS))
            {
                /* single port test */
                for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
                {
                    hal_miim_write(unit, phyBasePort+phyPort,0xBC0,18,0x0000);
                    hal_miim_write(unit, phyBasePort+phyPort,0xBC0,19,0x01C0);
                    hal_miim_write(unit, phyBasePort+phyPort,0xA41,9,0x0E00);// Disable Test mode
                }

                val = 0x1 << chn;
                hal_miim_write(unit, port,0xBC0,18, val);
                hal_miim_write(unit, port,0xBC0,19,0x01D0);/*Enable get power control*/
                hal_miim_write(unit, port,0xA41,9,0x8E00);/*Enable Test Mode 4*/

                return RT_ERR_OK;
            }
            else
            {
                osal_printf("Not support all-phy-port operation.\n");
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
        }
        break;

    default:
        osal_printf("The mode (%u) is not suppoted.\n", pTestMode->mode);
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

}

/* Function Name:
 *      phy_8218d_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_rev_id - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218d_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *chip_rev_id)
{
    uint32              basePort, blk;
    int32               ret = RT_ERR_OK;
    uint32              data;

    basePort = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = hal_miim_read(unit, basePort, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, basePort, 0, 30, 8);
    hal_miim_read(unit, basePort, 0x327, 0x15, &data);
    hal_miim_write(unit, basePort, 0, 30, blk);

    *chip_rev_id = (uint16) REG32_FIELD_GET(data, 0, 0x3F);
    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_rtl8218d_sdsRx_rst
 * Description:
 *      Reset PHY's MAC interface SerDes
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
_phy_rtl8218d_sdsRx_rst(uint32 unit, rtk_port_t port)
{
    uint32  blk;
    int32   ret;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(phy_field_write(unit, port, 0x401, 0x16, 12, 12, 1), ERR, ret);
    RT_ERR_HDL(phy_field_write(unit, port, 0x501, 0x16, 12, 12, 1), ERR, ret);

    RT_ERR_HDL(phy_field_write(unit, port, 0x4b8, 0x15, 11, 11, 0), ERR, ret);
    RT_ERR_HDL(phy_field_write(unit, port, 0x4b8, 0x15, 9, 9, 1), ERR, ret);
    RT_ERR_HDL(phy_field_write(unit, port, 0x4b8, 0x15, 9, 9, 0), ERR, ret);

ERR:
    hal_miim_write(unit, port, 0, 30, blk);

    return ret;
}   /* end of _phy_rtl8218d_sdsRx_rst */


/* Function Name:
 *      _phy_8218d_leqCali_start
 * Description:
 *      start LEQ calibration
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
_phy_8218d_leqCali_start(uint32 unit, rtk_port_t port)
{
    int32       ret, i;
    uint32      cali = 0, blk, data, data_link0, data_link1;
    uint32      park_page = 0x1F;
    int32       retry, linkReChk;

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x13, 5, 5, 0);
    hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x11, 12, 12, 0);

    for (retry = 0; retry < 3; retry++)
    {
        /* disable LEQ */
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x11, 13, 13, 1);
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x11, 12, 12, 1);

        /* SerDes Reset */
        hal_miim_parkBits_write(unit, port, 0x400, park_page, 0x10, 1, 1, 0);
        hal_miim_parkBits_write(unit, port, 0x400, park_page, 0x10, 1, 1, 1);

        cali = 0;
        data_link0 = 0;
        data_link1 = 0;
        for (i = 0; i < 100; i++)
        {
            hal_miim_parkBits_write(unit, port, 0x403, park_page, 0x14, 15, 0, 0xC00B);
            osal_time_mdelay(1);
            data = 0;
            if (hal_miim_park_read(unit, port, 0x403, park_page, 0x15, &data) == RT_ERR_OK)
            {
                cali = REG32_FIELD_GET(data, 13, (0x1 << 13));
                if (cali == 1)
                {
                    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u port=%u data=%x i=%u", unit, port, data, i);
                    break;
                }
            }
        } /* end for */

        if (cali == 0)
        {
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u 8218D cali==0. (0x403,0x15)=0x%X (r=%d)", unit, port, data, retry);
            continue;
        }

        /* cali is 1, do further process */
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x11, 13, 13, 0);
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x11, 12, 12, 0);
        osal_time_mdelay(200);

        /* check SerDes link */
        for (linkReChk = 0; linkReChk < 5; linkReChk++)
        {
            hal_miim_read(unit, port, 0x407, 0x15, &data_link0);
            hal_miim_read(unit, port, 0x507, 0x15, &data_link1);
            if (((data_link0 & 0x1FF) == 0x1FF) && ((data_link1 & 0x1FF) == 0x1FF))
            {
                break;
            }
            osal_time_mdelay(150);
        } /* end for */

        if (((data_link0 & 0x1FF) == 0x1FF) && ((data_link1 & 0x1FF) == 0x1FF))
        {
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u port=%u link=0x%x,0x%x not up (r=%d)", unit, port, data_link0, data_link1, retry);
            break;
        }
        else
        {
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u port=%u link=0x%x,0x%x not up (r=%d)", unit, port, data_link0, data_link1, retry);
        }
    } /* end for (retry) */

#if PHY_RTL8218D_LEQ_DFE
    {
        uint32      vthP=0, vthN=0;
        hal_miim_parkBits_write(unit, port, 0x486, park_page, 0x17, 13, 13, 0);
        hal_miim_parkBits_write(unit, port, 0x486, park_page, 0x17, 7, 7, 0);
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x15, 10, 10, 1);
        hal_miim_parkBits_write(unit, port, 0x485, park_page, 0x11, 11, 6, 0x20);
        hal_miim_parkBits_write(unit, port, 0x485, park_page, 0x11, 5, 0, 0xC);
        hal_miim_parkBits_read(unit, port, 0x47D, park_page, 0x16, 10, 8, &vthP);
        hal_miim_parkBits_read(unit, port, 0x47D, park_page, 0x16, 13, 11, &vthN);
        hal_miim_parkBits_write(unit, port, 0x4BA, park_page, 0x15, 14, 12, vthP);
        hal_miim_parkBits_write(unit, port, 0x4BA, park_page, 0x15, 11, 9, vthN);
        hal_miim_parkBits_write(unit, port, 0x486, park_page, 0x17, 13, 13, 1);
        hal_miim_parkBits_write(unit, port, 0x486, park_page, 0x10, 5, 0, 0x1F);
        hal_miim_parkBits_write(unit, port, 0x486, park_page, 0x17, 7, 7, 1);
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x15, 9, 9, 1);
        hal_miim_parkBits_write(unit, port, 0x4B8, park_page, 0x15, 9, 9, 0);
        osal_time_mdelay(200);
        hal_miim_parkBits_write(unit, port, 0x486, park_page, 0x17, 11, 8, 0);
    }
#endif /* PHY_RTL8218D_LEQ_DFE */

    /* read latch link to clear down state */
    hal_miim_park_read(unit, port, 0x407, park_page, 0x16, &data_link0);
    hal_miim_park_read(unit, port, 0x507, park_page, 0x16, &data_link1);


    data = 0;
    hal_miim_park_read(unit, port, 0x486, park_page, 0x10, &data);
    data = REG32_FIELD_SET(data, 0x4, 6, (0x3F << 6));    /* [11:6] */
    hal_miim_park_write(unit, port, 0x486, park_page, 0x10, data);

    data = 0;
    hal_miim_park_read(unit, port, 0x486, park_page, 0x11, &data);
    data = REG32_FIELD_SET(data, 0x4, 0, (0x3F << 0));    /* [5:0] */
    hal_miim_park_write(unit, port, 0x486, park_page, 0x11, data);


    /* clear symbol error */
    data = 0;
    hal_miim_park_write(unit, port, 0x404, park_page, 0x10, data);

    hal_miim_park_read(unit, port, 0x404, park_page, 0x11, &data);
    data = REG32_FIELD_SET(data, 0, 8, (0xFF << 8));    /* [15:8] */
    hal_miim_park_write(unit, port, 0x404, park_page, 0x11, data);


    hal_miim_write(unit, port, 0, 30, blk);
    return RT_ERR_OK;

}

/* Function Name:
 *      phy_8218d_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side serdes mode
 * Input:
 *      unit   - unit id
 *      port    - Base port ID of the PHY
 * Output:
 *      pMode   - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8218d_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode)
{
    int32       ret;
    uint32      data, blk;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, 0x260, 18, &data);
    hal_miim_write(unit, port, 0, 30, blk);

    switch (data & 0xF0)
    {
      case 0xD0:
        *pMode = RTK_MII_QSGMII;
        break;
      case 0xB0:
        *pMode = RTK_MII_XSGMII;
        break;
      default:
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_PORT), "unit %u port %u SDS mode:0x%X unknown type.", unit, port, (data & 0xF0));
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      phy_8218d_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base port ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218d_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    int32       ret = RT_ERR_OK;
    uint32      data, blk;
    rt_serdesMode_t sdsMode;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }
    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_read(unit, port, 0x260, 18, &data);
    switch (data & 0xF0)
    {
      case 0xD0:
        sdsMode = RTK_MII_QSGMII;
        break;
      case 0xB0:
        sdsMode = RTK_MII_XSGMII;
        break;
      default:
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_PORT), "unit %u port %u SDS mode:0x%X unknown type.", unit, port, (data & 0xF0));
        ret = RT_ERR_FAILED;
        goto EXIT;
    }

    data=0;
    hal_miim_read(unit, port, 0x407, 0x15, &data);
    if (sdsMode == RTK_MII_QSGMII)
    {
        pStatus->sds_num = 2;
        /* return link up when all channels are all up */
        pStatus->link_status[0] = ((data & 0xF0) == 0xF0) ? PORT_LINKUP : PORT_LINKDOWN;
        data=0;
        hal_miim_read(unit, port, 0x507, 0x15, &data);
        pStatus->link_status[1] = ((data & 0xF0) == 0xF0) ? PORT_LINKUP : PORT_LINKDOWN;
    }
    else
    {
        pStatus->sds_num = 1;
        /* return link up when all channels are all up */
        pStatus->link_status[0] = ((data & 0xF0) == 0xF0) ? PORT_LINKUP : PORT_LINKDOWN;
    }

EXIT:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


int32
_phy_8218d_ledModeData_set(rtk_phy_ledMode_t *pLedMode, uint32 *pData)
{
    uint32  val;

    if (pLedMode->mdi == RTK_PHY_LED_MODE_MDI_DISABLE)
    {
        val = 0xF;
    }
    else if (pLedMode->mdi <= 7)
    {
        val = pLedMode->mdi;
    }
    else
    {
        return RT_ERR_INPUT;
    }
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_MDI_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_1000M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_100M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_SPEED_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_10M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_SPEED_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_1000M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_1000M_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_100M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_100M_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_10M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_10M_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_DUPLEX) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_DUPLEX_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_COLLISION) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_COLLISION_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_TX_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_TX_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_RX_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8218D_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET, PHY_8218D_SERIAL_LED_PER_LED_CTRL_RX_ACT_MASK);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_ledMode_set
 * Description:
 *      Set LED mode for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 *      pLedMode - LED mode select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
phy_8218d_ledMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode)
{
    uint32  page, reg, data, blk;
    int32   ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = _phy_8218d_serialLedModePageReg_get(pLedMode->led_id, &page, &reg)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u invalid led index %u ", unit, port, pLedMode->led_id);
        return ret;
    }

    if ((pLedMode->mdi != RTK_PHY_LED_MODE_MDI_DISABLE) && (pLedMode->mdi >= PHY_8218D_MAX_MDI))
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u invalid mdi %u ", unit, port, pLedMode->mdi);
        return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, page, reg, &data);
    ret = _phy_8218d_ledModeData_set(pLedMode, &data);
    hal_miim_write(unit, port, page, reg, data);
    hal_miim_write(unit, port, 0, 30, blk);

    return ret;

}

/* Function Name:
 *      phy_ledCtrl_get
 * Description:
 *      Get configuration of LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pLedCtrl - LED control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
phy_8218d_ledCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    int32       ret;
    uint32      data, blk, val;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, PHY_8218D_SERIAL_LED_CONFIG_PAGE, PHY_8218D_SERIAL_LED_CONFIG_REG, &data);
    hal_miim_write(unit, port, 0, 30, blk);

    val = REG32_FIELD_GET(data, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_32MS;
        break;
      case 1:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_64MS;
        break;
      case 2:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_128MS;
        break;
      case 3:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_256MS;
        break;
      case 4:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_512MS;
        break;
      case 5:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_1024MS;
        break;
      case 6:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_48MS;
        break;
      case 7:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_96MS;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown blink_rate val %u ", unit, port, val);
        break;
    }

    val = REG32_FIELD_GET(data, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_8MS;
        break;
      case 1:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_16MS;
        break;
      case 2:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_32MS;
        break;
      case 3:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_64MS;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown burst_cycle val %u ", unit, port, val);
        break;
    }

    val = REG32_FIELD_GET(data, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS;
        break;
      case 1:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS;
        break;
      case 2:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS;
        break;
      case 3:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown clock_cycle val %u ", unit, port, val);
        break;
    }

    val = REG32_FIELD_GET(data, PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->active_low = RTK_PHY_LED_CTRL_ACTIVE_HIGH;
        break;
      case 1:
        pLedCtrl->active_low = RTK_PHY_LED_CTRL_ACTIVE_LOW;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown active_low val %u ", unit, port, val);
        break;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      phy_ledCtrl_set
 * Description:
 *      Configure LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 *      pLedCtrl - LED control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
phy_8218d_ledCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    int32       ret;
    uint32      data, blk, val_blink_rate, val_burst_cycle, val_clock_cycle, val_active_low;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    switch(pLedCtrl->blink_rate)
    {
      case RTK_PHY_LED_CTRL_BLINK_RATE_32MS:
        val_blink_rate = 0;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_64MS:
        val_blink_rate = 1;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_128MS:
        val_blink_rate = 2;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_256MS:
        val_blink_rate = 3;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_512MS:
        val_blink_rate = 4;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_1024MS:
        val_blink_rate = 5;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_48MS:
        val_blink_rate = 6;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_96MS:
        val_blink_rate = 7;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown blink_rate cfg %u ", unit, port, pLedCtrl->blink_rate);
        return RT_ERR_INPUT;
    }


    switch(pLedCtrl->burst_cycle)
    {
      case RTK_PHY_LED_CTRL_BURST_CYCLE_8MS:
        val_burst_cycle = 0;
        break;
      case RTK_PHY_LED_CTRL_BURST_CYCLE_16MS:
        val_burst_cycle = 1;
        break;
      case RTK_PHY_LED_CTRL_BURST_CYCLE_32MS:
        val_burst_cycle = 2;
        break;
      case RTK_PHY_LED_CTRL_BURST_CYCLE_64MS:
        val_burst_cycle = 3;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown burst_cycle cfg %u ", unit, port, pLedCtrl->burst_cycle);
        return RT_ERR_INPUT;
    }


    switch(pLedCtrl->clock_cycle)
    {
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS:
        val_clock_cycle = 0;
        break;
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS:
        val_clock_cycle = 1;
        break;
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS:
        val_clock_cycle = 2;
        break;
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS:
        val_clock_cycle = 3;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown clock_cycle cfg %u ", unit, port, pLedCtrl->clock_cycle);
        return RT_ERR_INPUT;
    }

    switch(pLedCtrl->active_low)
    {
      case RTK_PHY_LED_CTRL_ACTIVE_HIGH:
        val_active_low = 0;
        break;
      case RTK_PHY_LED_CTRL_ACTIVE_LOW:
        val_active_low = 1;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown active_low cfg %u ", unit, port, pLedCtrl->active_low);
        return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, PHY_8218D_SERIAL_LED_CONFIG_PAGE, PHY_8218D_SERIAL_LED_CONFIG_REG, &data);
    data = REG32_FIELD_SET(data, val_blink_rate, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK);
    data = REG32_FIELD_SET(data, val_burst_cycle, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK);
    data = REG32_FIELD_SET(data, val_clock_cycle, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK);
    data = REG32_FIELD_SET(data, val_active_low, PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET, PHY_8218D_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK);
    hal_miim_write(unit, port, PHY_8218D_SERIAL_LED_CONFIG_PAGE, PHY_8218D_SERIAL_LED_CONFIG_REG, data);
    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;

}



/* Function Name:
 *      phy_8218d_macIntfSdsRxCali_start
 * Description:
 *      Perform serdes RX calibration on PHY's MAC (system) interface
 *      It can only be processed when the interface is in XSGMII mode.
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8218d_macIntfSdsRxCali_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t    pollSts;

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D_NMP)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u not support", unit, port);
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u error port ID", unit, port);
        return RT_ERR_PORT_ID;
    }

    hal_miim_pollingWa_lock(unit, port, &pollSts);

    ret = _phy_8218d_leqCali_start(unit, port);

    hal_miim_pollingWa_unlock(unit, port, pollSts);
    return ret;
}


/* Function Name:
 *      phy_8218d_macIntfSerdes_reset
 * Description:
 *      Reset PHY's MAC (system) interface
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8218d_macIntfSerdes_reset(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  blk, data, park_page = 0x1F;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);
    /* serdes 0 reset */
    /* Bit[1]: SP_SDS_EN_RX; Bit[0]: SP_SDS_EN_TX */
    hal_miim_parkBits_write(unit, port, 0x400, park_page, 0x10, 1, 0, 0);
    hal_miim_parkBits_write(unit, port, 0x400, park_page, 0x10, 1, 0, 3);

    hal_miim_park_read(unit, port, 0x260, park_page, 18, &data);
    /* for qsgmii serdes 1 reset */
    if(0xD0 == (data & 0xF0))
    {
        hal_miim_parkBits_write(unit, port, 0x500, park_page, 0x10, 1, 0, 0);
        hal_miim_parkBits_write(unit, port, 0x500, park_page, 0x10, 1, 0, 3);
    }
    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;
}

#ifdef CONFIG_SDK_RTL8218D
/* Function Name:
 *      phy_8218d_serdes_linkdown_chk
 * Description:
 *      check phy serdes linkdown status
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 *      uint32 * status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8218d_serdes_linkdown_chk(uint32 unit, rtk_port_t port, uint32 * status)
{
    uint32  phy_data0, phy_data1,phy_data2;
    uint32  phy_dataA, phy_dataB, phy_dataC;
    uint32  data, blk;
    uint32  p, chk_s0 = FALSE, chk_s1 = FALSE;


    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, 0x260, 18, &data);

    hal_miim_read(unit, port, 0x407, 0x15, &phy_data0);
    hal_miim_read(unit, port, 0x407, 0x15, &phy_data1);
    hal_miim_read(unit, port, 0x407, 0x15, &phy_data2);
    /*QSGMII*/
    if(0xd0 == (data & 0xF0))
    {
        hal_miim_read(unit, port, 0x507, 0x15, &phy_dataA);
        hal_miim_read(unit, port, 0x507, 0x15, &phy_dataB);
        hal_miim_read(unit, port, 0x507, 0x15, &phy_dataC);
    }
    hal_miim_write(unit, port, 0, 30, blk);

    if(0xd0 == (data & 0xF0))
    {
        for (p=port; p<(port+4); p++)
        {
            if (HWP_PORT_EXIST(unit, p) && (HWP_PHY_BASE_MACID(unit, p) == port) && (HWP_PORT_SDSID(unit, p) != HWP_NONE))
            {
                chk_s0 = TRUE;
                break;
            }
        }
        for (p=(port+4); p<(port+PORT_NUM_IN_8218D); p++)
        {
            if (HWP_PORT_EXIST(unit, p) && (HWP_PHY_BASE_MACID(unit, p) == port) && (HWP_PORT_SDSID(unit, p) != HWP_NONE))
            {
                chk_s1 = TRUE;
                break;
            }
        }

        *status = 0x0;
        if (chk_s0 == TRUE)
        {
            if((0xff != (phy_data1 & 0xff)) || (0xff != (phy_data2 & 0xff)))
            {
                *status = 0x1; /*phy serdes had been linkdown*/
            }
        }
        if (chk_s1 == TRUE)
        {
            if((0xff != (phy_dataB & 0xff)) || (0xff != (phy_dataC & 0xff)))
            {
                *status = 0x1; /*phy serdes had been linkdown*/
            }
        }
    }
    else
    {
      if((0x1ff == (phy_data1 & 0x1ff)) && (0x1ff == (phy_data2 & 0x1ff)))
      {
          *status = 0x0;
      }
      else
      {
           *status = 0x1;      /*phy serdes had been linkdown*/
      }
    }
    return RT_ERR_OK;
}
#endif


static int32
_phy_8218d_eye_dbg_out(uint32 unit, uint32 port)
{
    return RT_ERR_OK;
}   /* end of _phy_8218d_eye_dbg_out */

static int32
_phy_8218d_eye_scan_en(uint32 unit, uint32 port)
{
    uint32  EYE_SCAN_EN_PAGE = 0x4B8;
    uint32  EYE_SCAN_EN_REG = 0x12;
    uint32  EYE_SCAN_EN_BIT = 2;
    uint32  EYE_CK_SEL_PAGE = 0x4B8;
    uint32  EYE_CK_SEL_REG = 0x11;
    uint32  EYE_CK_SEL_BIT = 15;
    int32   ret;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 1), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 0), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 1), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_CK_SEL_PAGE, EYE_CK_SEL_REG,
            EYE_CK_SEL_BIT, EYE_CK_SEL_BIT, 0), ret);

    return RT_ERR_OK;
}   /* end of _phy_8218d_eye_scan_en */

static int32
_phy_8218d_eye_pi_set(uint32 unit, uint32 port, uint32 xAxis)
{
    uint32  EYE_PI_EN_PAGE = 0x4BA;
    uint32  EYE_PI_EN_REG = 0x17;
    uint32  EYE_PI_EN_BITHigh = 11;
    uint32  EYE_PI_EN_BITLow = 8;
    uint32  EYE_PI_ST_PAGE = 0x487;
    uint32  EYE_PI_ST_REG = 0x14;
    uint32  EYE_PI_ST_BITHigh = 15;
    uint32  EYE_PI_ST_BITLow = 0;
    uint32  piEn, piSt;
    uint32  piEnList[] = {0x9, 0xC, 0x6, 0x3};
    uint32  piStList[2][16] = {
                {0xFFFF, 0x7FFF, 0x3FFF, 0x1FFF,
                 0x0FFF, 0x07FF, 0x03FF, 0x01FF,
                 0x00FF, 0x007F, 0x003F, 0x001F,
                 0x000F, 0x0007, 0x0003, 0x0001},
                {0x0000, 0x8000, 0xC000, 0xE000,
                 0xF000, 0xF800, 0xFC00, 0xFE00,
                 0xFF00, 0xFF80, 0xFFC0, 0xFFE0,
                 0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE}};
    int32   ret;

    piEn = piEnList[xAxis / 16];
    piSt = piStList[(xAxis / 16) % 2][xAxis % 16];

    RT_ERR_CHK(phy_field_write(unit, port, EYE_PI_EN_PAGE, EYE_PI_EN_REG,
            EYE_PI_EN_BITHigh, EYE_PI_EN_BITLow, piEn), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_PI_ST_PAGE, EYE_PI_ST_REG,
            EYE_PI_ST_BITHigh, EYE_PI_ST_BITLow, piSt), ret);

    return RT_ERR_OK;
}   /* end of _phy_8218d_eye_pi_set */

static int32
_phy_8218d_eye_ref_set(uint32 unit, uint32 port, uint32 yAxis)
{
    uint32  EYE_REF_CTRL_PAGE = 0x485;
    uint32  EYE_REF_CTRL_REG = 0x12;
    uint32  EYE_REF_CTRL_BITHigh = 11;
    uint32  EYE_REF_CTRL_BITLow = 6;
    int32   ret;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_REF_CTRL_PAGE, EYE_REF_CTRL_REG,
            EYE_REF_CTRL_BITHigh, EYE_REF_CTRL_BITLow, yAxis), ret);

    return RT_ERR_OK;
}   /* end of _phy_8218d_eye_ref_set */

static int32
_phy_8218d_eye_mdioClk(uint32 unit, uint32 port)
{
    uint32  EYE_MDIOCLK_PAGE = 0x4B8;
    uint32  EYE_MDIOCLK_REG = 0x10;
    uint32  EYE_MDIOCLK_BIT = 15;
    int32   ret;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_MDIOCLK_PAGE, EYE_MDIOCLK_REG,
            EYE_MDIOCLK_BIT, EYE_MDIOCLK_BIT, 1), ret);
    RT_ERR_CHK(phy_field_write(unit, port, EYE_MDIOCLK_PAGE, EYE_MDIOCLK_REG,
            EYE_MDIOCLK_BIT, EYE_MDIOCLK_BIT, 0), ret);

    return RT_ERR_OK;
}   /* end of _phy_8218d_eye_mdioClk */

static int32
_phy_8218d_eyePointRead(uint32 unit, uint32 port, uint32 *val)
{
    uint32  readPage = 0x47D;
    uint32  readReg = 0x17;
    int32   retry = 100;
    int32   ret = RT_ERR_OK;


    RT_ERR_CHK(_phy_8218d_eye_mdioClk(unit, port), ret);
    RT_ERR_CHK(hal_miim_read(unit, port, readPage, readReg, val), ret);

    do {
        RT_ERR_CHK(_phy_8218d_eye_mdioClk(unit, port), ret);
        RT_ERR_CHK(hal_miim_read(unit, port, readPage, readReg, val), ret);
        retry--;
        if (retry <= 0)
        {
            ret = RT_ERR_BUSYWAIT_TIMEOUT;
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "unit %u macId %u readOut retry timeout.", unit, port);
            break;
        }
    } while (((*val >> 14) & 0x3) != 0x3);

    return ret;
}   /* end of _phy_8218d_eyePointRead */

/* Function Name:
 *      _phy_8218d_eyeMon_proc
 * Description:
 *      Trigger eye monitor process function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sds     - serdes ID
 *      frameNum- frame number
 *      pHdlr   - handler
 * Output:
 *      pDb     - output database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8218d_eyeMon_proc(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, phy_sds_eyeMon_hdlr_t pHdlr, void *pDb)
{
    uint32  blk;
    uint32  frame;
    uint32  val;
    int32   xAxis, yAxis;
    int32   ret;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    /* enable */
    RT_ERR_HDL(_phy_8218d_eye_dbg_out(unit, port), ERR, ret);
    RT_ERR_HDL(_phy_8218d_eye_scan_en(unit, port), ERR, ret);

    /* set x axis */
    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
    {
        RT_ERR_HDL(_phy_8218d_eye_pi_set(unit, port, xAxis), ERR, ret);

        /* set y axis */
        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; ++yAxis)
        {
            RT_ERR_HDL(_phy_8218d_eye_ref_set(unit, port, yAxis), ERR, ret);

            /* get val */
            for (frame = 0; frame < frameNum; ++frame)
            {
                if ((ret = _phy_8218d_eyePointRead(unit, port, &val)) != RT_ERR_OK)
                {
                    goto ERR;
                }
                pHdlr(xAxis, yAxis, frame, pDb, val);
            }
        }
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218d_eyeMonitor_start
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
phy_8218d_eyeMonitor_start(uint32 unit, uint32 port, uint32 sdsId, uint32 frameNum)
{
    uint32  xAxis, yAxis, frame;
    uint32  data_size;
    uint32  *eyeData = NULL;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,port=%d,sdsId=%d,frameNum=%d", unit, port, sdsId, frameNum);

    /* parameter check */
    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    if ((ret = _phy_8218d_eyeMon_proc(unit, port, sdsId, frameNum, phy_common_eyeMonPixel_get, (void *)eyeData)) != RT_ERR_OK)
    {
        goto ERR;
    }

    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; ++yAxis)
            for (frame = 0; frame < frameNum; ++frame)
                osal_printf("[%d, %d : %d]\n", xAxis, yAxis, eyeData[RTK_EYE_MON_DATA_POS(frame, xAxis, yAxis)]);

ERR:
    if (eyeData)
    {
        osal_free(eyeData);
    }

    return ret;
}

/* Function Name:
 *      phy_8218d_eyeMonitorInfo_get
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
phy_8218d_eyeMonitorInfo_get(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    rtk_bitmap_t    eye[RTK_EYE_MON_X_MAX][BITMAP_ARRAY_CNT(RTK_EYE_MON_ARXIS_Y_MAX)];
    int32           xAxis, yAxis;
    uint32          data_size, i;
    uint8           maxHeight, height;
    uint8           maxWidth, width;
    uint8           width_sample_pos[] = {(RTK_EYE_MON_Y_MAX - 10), (RTK_EYE_MON_Y_MAX + 10)};
    int32           ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PHY), "unit=%u,port=%u,sds=%u,frameNum=%u", unit, port, sds, frameNum);

    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    data_size = sizeof(rtk_bitmap_t) * RTK_EYE_MON_X_MAX * BITMAP_ARRAY_CNT(RTK_EYE_MON_ARXIS_Y_MAX);
    osal_memset(eye, 0, data_size);


    if ((ret = _phy_8218d_eyeMon_proc(unit, port, sds, frameNum, phy_common_eyeMonInfo_update, (void *)eye)) != RT_ERR_OK)
    {
        goto ERR;
    }

    maxHeight = 0;
    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
    {
        height = 0;
        for (yAxis = 0; yAxis < RTK_EYE_MON_ARXIS_Y_MAX; ++yAxis)
        {
            if (BITMAP_IS_SET(eye[xAxis], yAxis))
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
            if (BITMAP_IS_SET(eye[xAxis], yAxis))
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
    return ret;
}


/* Function Name:
 *      phy_8218d_mdiLoopbackEnable_get
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
phy_8218d_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  data = 0;

    hal_miim_read(unit, port, 0xa43, 16, &data);
    if (PHY_8218D_MDI_LB_MDI_VAL_GET(data) == 0x3)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_mdiLoopbackEnable_set
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
phy_8218d_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  data, val;

    if (enable == ENABLED)
    {
        val = 0x3;
    }
    else
    {
        val = rtl8218d_info[unit].mdi_val_org[port];
    }

    hal_miim_read(unit, port, 0xa43, 16, &data);
    data = PHY_8218D_MDI_LB_MDI_VAL_SET(data, val);
    hal_miim_write(unit, port, 0xa43, 16, data);
    return RT_ERR_OK;
}



/* Function Name:
 *      phy_8218d_sdsEyeParam_get
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
phy_8218d_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32   ret;
    uint32  data, blk, page;
    rt_serdesMode_t     sdsMode;

    osal_memset(pEyeParam, 0, sizeof(rtk_sds_eyeParam_t));
    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = phy_8218d_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (sdsMode == RTK_MII_XSGMII)
    {
        if (sdsId != 0)
        {
            return RT_ERR_INPUT;
        }
    }
    else
    {
        if ((sdsId != 0) && (sdsId != 1))
        {
            return RT_ERR_INPUT;
        }
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);

    if (sdsMode == RTK_MII_XSGMII)
    {
        /* pre amp enable[2], post amp enable[1] */
        hal_miim_read(unit, port, 0x4B8, 0x15, &data);
        pEyeParam->pre_en = REG32_FIELD_GET(data, 2, (0x1 << 2));
        pEyeParam->post_en = REG32_FIELD_GET(data, 1, (0x1 << 1));

        /* pre amp[9:5], post amp[4:0] */
        hal_miim_read(unit, port, 0x484, 0x11, &data);
        pEyeParam->pre_amp = REG32_FIELD_GET(data, 5, (0x1F << 5));
        pEyeParam->post_amp = REG32_FIELD_GET(data, 0, (0x1F << 0));

        /* main amp[9:5] */
        hal_miim_read(unit, port, 0x4BB, 0x16, &data);
        pEyeParam->main_amp = REG32_FIELD_GET(data, 5, (0x1F << 5));

        /* tx impedience [15:12] */
        hal_miim_read(unit, port, 0x4BD, 0x12, &data);
        pEyeParam->impedance = REG32_FIELD_GET(data, 12, (0xF << 12));

    }
    else if (sdsMode == RTK_MII_QSGMII)
    {
        page = (sdsId == 0) ? 0x4A8 : 0x5A8;
        /* pre amp enable[2], post amp enable[1] */
        hal_miim_read(unit, port, page, 0x15, &data);
        pEyeParam->pre_en = REG32_FIELD_GET(data, 2, (0x1 << 2));
        pEyeParam->post_en = REG32_FIELD_GET(data, 1, (0x1 << 1));

        page = (sdsId == 0) ? 0x484 : 0x584;
        /* pre amp[9:5], post amp[4:0] */
        hal_miim_read(unit, port, page, 0x11, &data);
        pEyeParam->pre_amp = REG32_FIELD_GET(data, 5, (0x1F << 5));
        pEyeParam->post_amp = REG32_FIELD_GET(data, 0, (0x1F << 0));

        page = (sdsId == 0) ? 0x4AB : 0x5AB;
        /* main amp[9:5] */
        hal_miim_read(unit, port, page, 0x16, &data);
        pEyeParam->main_amp = REG32_FIELD_GET(data, 5, (0x1F << 5));

        if (sdsId == 0)
        {
            /* tx impedience [15:12] */
            hal_miim_read(unit, port, 0x4BD, 0x12, &data);
            pEyeParam->impedance = REG32_FIELD_GET(data, 12, (0xF << 12));
        }
        else
        {
            pEyeParam->impedance = 0;
        }
    }

    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_sdsEyeParam_set
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
phy_8218d_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32   ret;
    uint32  data, blk, page;
    rt_serdesMode_t     sdsMode;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = phy_8218d_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (sdsMode == RTK_MII_XSGMII)
    {
        if (sdsId != 0)
        {
            return RT_ERR_INPUT;
        }
    }
    else
    {
        if ((sdsId != 0) && (sdsId != 1))
        {
            return RT_ERR_INPUT;
        }
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);


    if (sdsMode == RTK_MII_XSGMII)
    {
        /* pre amp enable[2], post amp enable[1] */
        hal_miim_read(unit, port, 0x4B8, 0x15, &data);
        data = REG32_FIELD_SET(data, pEyeParam->pre_en, 2, (0x1 << 2));
        data = REG32_FIELD_SET(data, pEyeParam->post_en, 1, (0x1 << 1));
        hal_miim_write(unit, port, 0x4B8, 0x15, data);

        /* pre amp[9:5], post amp[4:0] */
        hal_miim_read(unit, port, 0x484, 0x11, &data);
        data = REG32_FIELD_SET(data, pEyeParam->pre_amp, 5, (0x1F << 5));
        data = REG32_FIELD_SET(data, pEyeParam->post_amp, 0, (0x1F << 0));
        hal_miim_write(unit, port, 0x484, 0x11, data);

        /* main amp [9:5] */
        hal_miim_read(unit, port, 0x4BB, 0x16, &data);
        data = REG32_FIELD_SET(data, pEyeParam->main_amp, 5, (0x1F << 5));
        hal_miim_write(unit, port, 0x4BB, 0x16, data);
    }
    else if (sdsMode == RTK_MII_QSGMII)
    {
        page = (sdsId == 0) ? 0x4A8 : 0x5A8;
        /* pre amp enable[2], post amp enable[1] */
        hal_miim_read(unit, port, page, 0x15, &data);
        data = REG32_FIELD_SET(data, pEyeParam->pre_en, 2, (0x1 << 2));
        data = REG32_FIELD_SET(data, pEyeParam->post_en, 1, (0x1 << 1));
        hal_miim_write(unit, port, page, 0x15, data);

        page = (sdsId == 0) ? 0x484 : 0x584;
        /* pre amp[9:5], post amp[4:0] */
        hal_miim_read(unit, port, page, 0x11, &data);
        data = REG32_FIELD_SET(data, pEyeParam->pre_amp, 5, (0x1F << 5));
        data = REG32_FIELD_SET(data, pEyeParam->post_amp, 0, (0x1F << 0));
        hal_miim_write(unit, port, page, 0x11, data);

        page = (sdsId == 0) ? 0x4AB : 0x5AB;
        /* main amp [9:5] */
        hal_miim_read(unit, port, page, 0x16, &data);
        data = REG32_FIELD_SET(data, pEyeParam->main_amp, 5, (0x1F << 5));
        hal_miim_write(unit, port, page, 0x16, data);
    }

    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;
}


/*
 * PHY Interrupts.
 */
int32
_phy_8218d_intrNwayNextPageRecvEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port Nway Next Page Recv enable (page 0xa42 reg 0x12 bit 2). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 2))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8218d_intrNwayNextPageRecvEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port Nway Next Page Recv enable (page 0xa42 reg 0x12 bit 2). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 2, (0x1 << 2));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrANCompleteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port AN Complete enable (page 0xa42 reg 0x12 bit 3). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 3))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8218d_intrANCompleteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port AN Complete enable (page 0xa42 reg 0x12 bit 3). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 3, (0x1 << 3));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrLinkChangeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port Link Change enable (page 0xa42 reg 0x12 bit 4). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 4))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8218d_intrLinkChangeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port Link Change enable (page 0xa42 reg 0x12 bit 4). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 4, (0x1 << 4));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrAldpsStateChangeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port ALDPS State Change enable (page 0xa42 reg 0x12 bit 9). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 9))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8218d_intrAldpsStateChangeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port ALDPS State Change enable (page 0xa42 reg 0x12 bit 9). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 9, (0x1 << 9));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrCommonStatus_get(uint32 unit, uint32 basePort, rtk_phy_intrStatusVal_t *pStatus)
{
    int32           i = 0;
    uint32          data = 0;
    uint32          blk = 0;
    uint32          port = 0;
    rtk_portmask_t  portmask;

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    osal_memset(pStatus, 0, sizeof(rtk_phy_intrStatusVal_t));
    pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP_PORT_ARRAY;

    /* Read/Clear Top interrupt port status to get which ports occurred interrupt */
    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);
    hal_miim_read(unit, basePort, 0x320, 0x12, &data);
    hal_miim_write(unit, basePort, 0, 30, blk);
    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    for (i = 0; i < HWP_PHY_BASE_PHY_MAX(unit, basePort); i++)
    {
        if (data & (1 << i))
        {
            port = basePort + i;
            RTK_PORTMASK_PORT_SET(portmask, port);
        }
    }


    /*
     * Read common types of interrupts status, supporting following types,
     * a bit represents a interrupt type status of a port:
     * [2]: An Nway Next Page is received
     * [3]: Auto-Negotiation Completed
     * [4]: Link Status Changed
     * [9]: ALDPS State Changes
     */
    RTK_PORTMASK_SCAN(portmask, port)
    {
        hal_miim_read(unit, port, 0, 30, &blk);
        hal_miim_write(unit, port, 0, 30, 0);

        hal_miim_read(unit, port, 0xa43, 0x15, &data);
        i = port - basePort;
        pStatus->statusValueArray[i] |= (data & 0x4) ? (1 << RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV) : (0);
        pStatus->statusValueArray[i] |= (data & 0x8) ? (1 << RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE) : (0);
        pStatus->statusValueArray[i] |= (data & 0x10) ? (1 << RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE) : (0);
        pStatus->statusValueArray[i] |= (data & 0x200) ? (1 << RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE) : (0);

        hal_miim_write(unit, port, 0, 30, blk);
    }

    return RT_ERR_OK;
}

int32
_phy_8218d_intrIntpinCommonMask_get(uint32 unit, uint32 basePort, uint32 phyPort, uint32 *pMask)
{
    uint32 data = 0;

    /* Read the chip interrupt PIN IMR (page 0x320 reg 0x11). */
    hal_miim_read(unit, basePort, 0x320, 0x11, &data);
    data = REG32_FIELD_GET(data, phyPort, (0x1 << phyPort));
    *pMask = data;

    return RT_ERR_OK;
}

int32
_phy_8218d_intrIntpinCommonMask_set(uint32 unit, uint32 basePort, uint32 phyPort, uint32 mask)
{
    uint32 data = 0;

    if ((mask != 0) && (mask != 1))
    {
        return RT_ERR_INPUT;
    }

    /* Set the chip interrupt PIN IMR (page 0x320 reg 0x11). */
    hal_miim_read(unit, basePort, 0x320, 0x11, &data);
    data = REG32_FIELD_SET(data, mask, phyPort, (0x1 << phyPort));
    hal_miim_write(unit, basePort, 0x320, 0x11, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrRlfd_init(uint32 unit, rtk_port_t basePort)
{
    uint32 data = 0;

    hal_miim_write(unit, basePort, 0x320, 0x10, 0);
    hal_miim_write(unit, basePort, 0x320, 0x11, 0);
    hal_miim_write(unit, basePort, 0x320, 0x16, 0);
    hal_miim_write(unit, basePort, 0x320, 0x17, 0);

    /* Enable en_rlfd. */
    hal_miim_read(unit, basePort, 0x264, 0x16, &data);
    data = REG32_FIELD_SET(data, 0x3, 0, (0x3 << 0));
    hal_miim_write(unit, basePort, 0x264, 0x16, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrRlfdEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port RLFD enable (page 0xa44 reg 0x11 bit 15). */
    hal_miim_read(unit, port, 0xa44, 0x11, &data);

    if (data & (0x1 << 15))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8218d_intrRlfdEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set Per PHY port RLFD enable (page 0xa44 reg 0x11 bit 15). */
    hal_miim_read(unit, port, 0xa44, 0x11, &data);
    data = REG32_FIELD_SET(data, value, 15, (0x1 << 15));
    hal_miim_write(unit, port, 0xa44, 0x11, data);

    return RT_ERR_OK;
}

int32
_phy_8218d_intrRlfdStatus_get(uint32 unit, uint32 basePort, rtk_phy_intrStatusVal_t *pStatus)
{
    uint32 blk = 0;
    uint32 data = 0;

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_PORT_BITMAP;
    pStatus->statusValue = 0;

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    /* Read RLFD ISR. */
    hal_miim_read(unit, basePort, 0x320, 0x15, &data);
    data = REG32_FIELD_GET(data, 0, (0xFF << 0));
    pStatus->statusValue = data;

    hal_miim_write(unit, basePort, 0, 30, blk);
    return RT_ERR_OK;
}

int32
_phy_8218d_intrIntpinRlfdMask_get(uint32 unit, uint32 basePort, uint32 phyPort, uint32 *pMask)
{
    uint32 data = 0;

    /* Read the chip interrupt PIN IMR. */
    hal_miim_read(unit, basePort, 0x320, 0x14, &data);
    data = REG32_FIELD_GET(data, phyPort, (0x1 << phyPort));
    *pMask = data;

    return RT_ERR_OK;
}

int32
_phy_8218d_intrIntpinRlfdMask_set(uint32 unit, uint32 basePort, uint32 phyPort, uint32 mask)
{
    uint32 data = 0;

    if ((mask != 0) && (mask != 1))
    {
        return RT_ERR_INPUT;
    }

    /* Disable the chip interrupt SerDes IMR. */
    hal_miim_write(unit, basePort, 0x321, 0x12, 0x00);

    /* Enable the chip interrupt PIN IMR. */
    hal_miim_read(unit, basePort, 0x320, 0x14, &data);
    data = REG32_FIELD_SET(data, mask, phyPort, (0x1 << phyPort));
    hal_miim_write(unit, basePort, 0x320, 0x14, data);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_intr_init
 * Description:
 *      Initialize the type of PHY interrupt function of the specified PHY chip.
 * Input:
 *      unit    - unit id
 *      port    - base mac ID number of the PHY
 *      phyIntr - PHY interrupt type
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
 *      The port id is the PHY address (the corresponding MAC id of its 1st PHY port).
 */
int32
phy_8218d_intr_init(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr)
{
    int32 ret = RT_ERR_OK;
    uint32 blk = 0;
    uint32 data = 0;

    /* Get the basePort (this PHY's phy port 0's MAC ID). */
    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    /* Enable PHY interrupt pin, and set as open-drain mode */
    hal_miim_read(unit, port, 0x321, 0x17, &data);
    data = REG32_FIELD_SET(data, 1, 0, (0x1 << 0));
    data = REG32_FIELD_SET(data, 0, 1, (0x1 << 1));
    hal_miim_write(unit, port, 0x321, 0x17, data);

    /* Per interrupt type init. */
    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            /* No per interrupt type init needed. */
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218d_intrRlfd_init(unit, port);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218d_intrEnable_get
 * Description:
 *      Get the type of PHY interrupt enable status of the specified port.
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
phy_8218d_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 0);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
            ret = _phy_8218d_intrNwayNextPageRecvEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
            ret = _phy_8218d_intrANCompleteEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
            ret = _phy_8218d_intrLinkChangeEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
            ret = _phy_8218d_intrAldpsStateChangeEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            ret = _phy_8218d_intrRlfdEnable_get(unit, port, pEnable);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218d_intrEnable_set
 * Description:
 *      Set the type of PHY interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      enable  - status
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
phy_8218d_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    uint32 blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 0);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
            ret = _phy_8218d_intrNwayNextPageRecvEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
            ret = _phy_8218d_intrANCompleteEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
            ret = _phy_8218d_intrLinkChangeEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
            ret = _phy_8218d_intrAldpsStateChangeEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            ret = _phy_8218d_intrRlfdEnable_set(unit, port, enable);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218d_sdsLeq_get
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sdsId   - SerDes of the PHY
 * Output:
 *      pManual_en - pointer to manual LEQ config satus
 *      pLeq_val   - pointer to current LEQ value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8218d_sdsLeq_get(uint32 unit, rtk_port_t basePort, uint32 sdsId, rtk_enable_t *pManual_en, uint32 *pLeq_val)
{
    uint32  data, blk, data_reg0x11;
    int32   ret;

    if (sdsId != 0)
    {
        return RT_ERR_INPUT;
    }

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, basePort, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, basePort, 0, 30, 8);

    /* page 0x4b8 reg 0x11 [13] = 0 */
    hal_miim_read(unit, basePort, 0x4b8, 0x11, &data_reg0x11);
    if (REG32_FIELD_GET(data_reg0x11, 13, (0x1 << 13)) == 1)
    {
        *pManual_en = ENABLED;
    }
    else
    {
        *pManual_en = DISABLED;
    }

    /* page 0x486 reg 0x16 [13-9] */
    hal_miim_read(unit, basePort, 0x486, 0x16, &data);
    *pLeq_val = REG32_FIELD_GET(data, 9, (0x1F << 9));

    hal_miim_write(unit, basePort, 0, 30, blk);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_sdsLeq_set
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sdsId   - SerDes of the PHY
 *      manual_en - ENABLED: LEQ in manual-mode; DISABLED: LEQ is auto-mode.
 *      leq_val - Fixed LEQ value when manual_en is set to ENABLED;
 *                this field is not used in driver when manual_en set to DISABLED, just keep it set to 0.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */

int32
phy_8218d_sdsLeq_set(uint32 unit, rtk_port_t basePort, uint32 sdsId, rtk_enable_t manual_en, uint32 leq_val)
{
    uint32  data, blk, data_reg0x11;
    int32   ret;

    if (sdsId != 0)
    {
        return RT_ERR_INPUT;
    }

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, basePort, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, basePort, 0, 30, 8);

    if (manual_en == ENABLED)
    {
        /* page 0x4b8 reg 0x11 [13] = 0 */
        hal_miim_read(unit, basePort, 0x4b8, 0x11, &data_reg0x11);
        data_reg0x11 = REG32_FIELD_SET(data_reg0x11, 0, 13, (0x1 << 13));
        hal_miim_write(unit, basePort, 0x4b8, 0x11, data_reg0x11);

        /* page 0x486 reg 0x16 [13-9] = leq_val */
        hal_miim_read(unit, basePort, 0x486, 0x16, &data);
        data = REG32_FIELD_SET(data, leq_val, 9, (0x1F << 9));
        hal_miim_write(unit, basePort, 0x486, 0x16, data);

        /* page 0x4b8 reg 0x11 [13] = 1 */
        data_reg0x11 = REG32_FIELD_SET(data_reg0x11, 1, 13, (0x1 << 13));
        hal_miim_write(unit, basePort, 0x4b8, 0x11, data_reg0x11);
    }
    else
    {
        /* page 0x4b8 reg 0x11 [13] = 0 */
        hal_miim_read(unit, basePort, 0x4b8, 0x11, &data_reg0x11);
        data_reg0x11 = REG32_FIELD_SET(data_reg0x11, 0, 13, (0x1 << 13));
        hal_miim_write(unit, basePort, 0x4b8, 0x11, data_reg0x11);
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return RT_ERR_OK;

}


/* Function Name:
 *      _phy_8218d_sdsSts_dump
 *      phy_8218d_intrStatus_get
 * Description:
 *      Get specified PHY interrupt status.
 * Input:
 *      unit    - unit id
 *      port    - port id or base mac ID number of the PHY
 *      phyIntr - PHY interrupt type
 * Output:
 *      pStatus - interrupt triggered status for specified PHY interrupt
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
phy_8218d_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus)
{
    int32 ret = RT_ERR_OK;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8218d_intrCommonStatus_get(unit, port, pStatus);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218d_intrRlfdStatus_get(unit, port, pStatus);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
}

/* Function Name:
 *      phy_8218d_intrMask_get
 * Description:
 *      Get PHY interrupt mask status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
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
phy_8218d_intrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask)
{
    int32 ret = RT_ERR_OK;
    uint32 basePort = 0;
    uint32 phyPort = 0;
    uint32 blk = 0;

    /* Get the corresponding PHY chip indexed by basePort (this PHY's phy port 0's MAC ID). */
    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218D)
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8218d_intrIntpinCommonMask_get(unit, basePort, phyPort, pMask);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218d_intrIntpinRlfdMask_get(unit, basePort, phyPort, pMask);
            break;
            ret = RT_ERR_INPUT;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218d_intrMask_set
 * Description:
 *      Set PHY interrupt mask status of the specified port.
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
phy_8218d_intrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask)
{
    int32 ret = RT_ERR_OK;
    uint32 basePort = 0;
    uint32 phyPort = 0;
    uint32 blk = 0;

    /* Get the corresponding PHY chip indexed by basePort (this PHY's phy port 0's MAC ID). */
    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218D)
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8218d_intrIntpinCommonMask_set(unit, basePort, phyPort, mask);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218d_intrIntpinRlfdMask_set(unit, basePort, phyPort, mask);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218d_ctrl_get
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
 *      RT_ERR_PORT_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8218d_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    int32  ret = 0;

    switch (ctrl_type)
    {
      case RTK_PHY_CTRL_LOOPBACK_REMOTE:
        ret = _phy_8218d_loopback_remote_get(unit, port, pValue);
        break;
      default:
        return RT_ERR_PORT_NOT_SUPPORTED;
    } /* end switch */

    return ret;
}

/* Function Name:
 *      phy_8218d_ctrl_set
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
 *      RT_ERR_PORT_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8218d_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    int32  ret = 0;

    switch (ctrl_type)
    {
      case RTK_PHY_CTRL_LOOPBACK_REMOTE:
        ret = _phy_8218d_loopback_remote_set(unit, port, value);
        break;
      default:
        return RT_ERR_PORT_NOT_SUPPORTED;
    } /* end switch */
    return ret;
}




/* Function Name:
 *      _phy_8218d_sdsSts_dump
 * Description:
 *      Dump serdes status
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8218d_sdsSts_dump(uint32 unit, rtk_port_t port)
{
    uint32  data = 0, data1 = 0, data2 = 0, data3 = 0;
    uint32  blk;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_read(unit, port, 0x407, 0x15, &data);
    hal_miim_read(unit, port, 0x507, 0x15, &data1);
    hal_miim_read(unit, port, 0x407, 0x16, &data2);
    hal_miim_read(unit, port, 0x507, 0x16, &data3);

    hal_miim_write(unit, port, 0, 30, blk);

    osal_printf("unit %u port %u sds sts\n", unit, port);
    osal_printf("  0x407,0x15=0x%x\n", data);
    osal_printf("  0x507,0x15=0x%x\n", data1);
    osal_printf("  0x407,0x16=0x%x\n", data2);
    osal_printf("  0x507,0x16=0x%x\n", data3);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218d_tap0_dump
 * Description:
 *      Dump tap0
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8218d_tap0_dump(uint32 unit, rtk_port_t port)
{
    uint32  data = 0;
    uint32  blk;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_write(unit, port, 0x4b8, 0x15, 0x5c0);
    hal_miim_write(unit, port, 0x485, 0x11, 0x400);
    hal_miim_read(unit, port, 0x47d, 0x16, &data);

    hal_miim_write(unit, port, 0, 30, blk);

    osal_printf("unit %u port %u tap0=0x%x\n", unit, port, data);

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218d_vth_dump
 * Description:
 *      Dump vth
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8218d_vth_dump(uint32 unit, rtk_port_t port)
{
    uint32  data = 0;
    uint32  blk;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_write(unit, port, 0x4b8, 0x15, 0x5c0);
    hal_miim_write(unit, port, 0x485, 0x11, 0x80c);
    hal_miim_read(unit, port, 0x47d, 0x16, &data);

    hal_miim_write(unit, port, 0, 30, blk);

    osal_printf("unit %u port %u vth=0x%x\n", unit, port, data);

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218d_tap1_dump
 * Description:
 *      Dump tap1
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8218d_tap1_dump(uint32 unit, rtk_port_t port)
{
    uint32  data = 0, data1 = 0;
    uint32  blk;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_write(unit, port, 0x4b8, 0x15, 0x5c0);
    hal_miim_write(unit, port, 0x485, 0x11, 0x401);
    hal_miim_read(unit, port, 0x47d, 0x16, &data);

    hal_miim_write(unit, port, 0x485, 0x11, 0x406);
    hal_miim_read(unit, port, 0x47d, 0x16, &data1);

    hal_miim_write(unit, port, 0, 30, blk);

    osal_printf("unit %u port %u tap1=0x%X, 0x%X\n", unit, port, data, data1);

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218d_sdsSymErr_dump
 * Description:
 *      dump symbol error count
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      sysErrCnt - symbol error counter
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8218d_sdsSymErr_dump(uint32 unit, rtk_port_t port)
{
    uint32  data = 0, data1 = 0, data2 = 0, data3 = 0, data_tmp;
    uint32  blk;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    hal_miim_read(unit, port, 0x404, 0x10, &data);
    hal_miim_read(unit, port, 0x404, 0x11, &data1);

    hal_miim_read(unit, port, 0x504, 0x10, &data2);
    hal_miim_read(unit, port, 0x504, 0x11, &data3);

    hal_miim_write(unit, port, 0x404, 0x10, 0);
    hal_miim_write(unit, port, 0x504, 0x10, 0);

    data_tmp = REG32_FIELD_SET(data1, 0, 8, (0xFF << 8));   /* [15:8] */
    hal_miim_write(unit, port, 0x404, 0x11, data_tmp);
    data_tmp = REG32_FIELD_SET(data3, 0, 8, (0xFF << 8));   /* [15:8] */
    hal_miim_write(unit, port, 0x504, 0x11, data_tmp);

    hal_miim_write(unit, port, 0, 30, blk);

    osal_printf("unit %u port %u serdes sym-err\n", unit, port);
    osal_printf("  0x404,0x10=0x%x\n", data);
    data_tmp = REG32_FIELD_GET(data1, 8, (0xFF << 8));    /* [15:8] */
    osal_printf("  0x404,0x11,[15:8]=0x%x\n", data_tmp);

    osal_printf("  0x504,0x10=0x%x\n", data2);
    data_tmp = REG32_FIELD_GET(data3, 8, (0xFF << 8));    /* [15:8] */
    osal_printf("  0x504,0x11,[15:8]=0x%x\n", data_tmp);

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218d_sdsLeq_dump
 * Description:
 *      dump Serdes LEQ value
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8218d_sdsLeq_dump(uint32 unit, rtk_port_t port)
{
    uint32  data = 0, blk;
    int32   ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    ret = hal_miim_read(unit, port, 0x47D, 0x15, &data);

    hal_miim_write(unit, port, 0, 30, blk);

    osal_printf("unit %u port %u LEQ\n", unit, port);
    osal_printf("  0x47D,0x15=0x%x\n", data);

    return ret;
}


/* Function Name:
 *      _phy_8218d_diag_dump
 * Description:
 *      dump 8218d status register
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */

int32
_phy_8218d_diag_dump(uint32 unit, uint32 port)
{
    uint32      ret;

    if ((ret = _phy_8218d_sdsSts_dump(unit, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8218d_sdsSts_dump Error:0x%x\n", ret);
    }

    if ((ret = _phy_8218d_sdsSymErr_dump(unit, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8218d_sdsSymErr_dump Error:0x%x\n", ret);
    }

    if ((ret = _phy_8218d_sdsLeq_dump(unit, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8218d_sdsLeq_dump Error:0x%x\n", ret);
    }

    if ((ret = _phy_8218d_tap0_dump(unit, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8218d_tap0VthTap1_dump Error:0x%x\n", ret);
    }

    if ((ret = _phy_8218d_vth_dump(unit, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8218d_vth_dump Error:0x%x\n", ret);
    }

    if ((ret = _phy_8218d_tap1_dump(unit, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8218d_tap1_dump Error:0x%x\n", ret);
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218Ddrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8218D driver.
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
phy_8218Ddrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8218D;
    pPhydrv->fPhydrv_init = phy_8218d_init;
    pPhydrv->fPhydrv_enable_set = phy_8218d_enable_set;
    pPhydrv->fPhydrv_chipRevId_get = phy_8218d_chipRevId_get;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218d_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_rtctResult_get = phy_8218d_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8218d_rtct_start;
    pPhydrv->fPhydrv_media_get = phy_8218d_media_get;
    pPhydrv->fPhydrv_ctrl_get = phy_8218d_ctrl_get;
    pPhydrv->fPhydrv_ctrl_set = phy_8218d_ctrl_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8218d_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8218d_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8218d_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8218d_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218d_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218d_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218d_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218d_eeeEnable_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218d_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218d_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218d_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218d_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218d_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8218d_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8218d_broadcastID_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8218d_gigaLiteEnable_get;
    pPhydrv->fPhydrv_gigaLiteEnable_set = phy_8218d_gigaLiteEnable_set;
    pPhydrv->fPhydrv_patch_set = phy_8218d_patch_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218d_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218d_downSpeedEnable_set;
    pPhydrv->fPhydrv_downSpeedStatus_get = phy_8218d_downSpeedStatus_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218d_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218d_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218d_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218d_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218d_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218d_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218d_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpRefTimeFreq_get = phy_8218d_ptpRefTimeFreq_get;
    pPhydrv->fPhydrv_ptpRefTimeFreq_set = phy_8218d_ptpRefTimeFreq_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218d_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218d_ptpEnable_set;
    pPhydrv->fPhydrv_ptpInterruptStatus_get = phy_8218d_ptpInterruptStatus_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_get = phy_8218d_ptpInterruptEnable_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_set = phy_8218d_ptpInterruptEnable_set;
    pPhydrv->fPhydrv_ptpIgrTpid_get = phy_8218d_ptpIgrTpid_get;
    pPhydrv->fPhydrv_ptpIgrTpid_set = phy_8218d_ptpIgrTpid_set;
    pPhydrv->fPhydrv_ptpSwitchMacRange_get = phy_8218d_ptpSwitchMacRange_get;
    pPhydrv->fPhydrv_ptpSwitchMacRange_set = phy_8218d_ptpSwitchMacRange_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218d_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218d_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_ptpOper_get = phy_8218d_ptpOper_get;
    pPhydrv->fPhydrv_ptpOper_set = phy_8218d_ptpOper_set;
    pPhydrv->fPhydrv_ptpLatchTime_get = phy_8218d_ptpLatchTime_get;
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
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_common_loopback_set;
    pPhydrv->fPhydrv_portEyeMonitor_start = phy_8218d_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get = phy_8218d_eyeMonitorInfo_get;
    pPhydrv->fPhydrv_reset_set = phy_common_reset_set;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get = phy_common_copperPeerAutoNegoAbility_get;
    pPhydrv->fPhydrv_macIntfSerdesMode_get = phy_8218d_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_ledMode_set = phy_8218d_ledMode_set;
    pPhydrv->fPhydrv_ledCtrl_get = phy_8218d_ledCtrl_get;
    pPhydrv->fPhydrv_ledCtrl_set = phy_8218d_ledCtrl_set;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8218d_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_get = phy_8218d_mdiLoopbackEnable_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_set = phy_8218d_mdiLoopbackEnable_set;
    pPhydrv->fPhydrv_sdsEyeParam_get = phy_8218d_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set = phy_8218d_sdsEyeParam_set;
    pPhydrv->fPhydrv_speedStatus_get = phy_common_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_common_speedDuplexStatusResReg_get;
    pPhydrv->fPhydrv_intr_init = phy_8218d_intr_init;
    pPhydrv->fPhydrv_intrEnable_get = phy_8218d_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set = phy_8218d_intrEnable_set;
    pPhydrv->fPhydrv_intrStatus_get = phy_8218d_intrStatus_get;
    pPhydrv->fPhydrv_intrMask_get = phy_8218d_intrMask_get;
    pPhydrv->fPhydrv_intrMask_set = phy_8218d_intrMask_set;
    pPhydrv->fPhydrv_sdsLeq_get = phy_8218d_sdsLeq_get;
    pPhydrv->fPhydrv_sdsLeq_set = phy_8218d_sdsLeq_set;
#endif

} /* end of phy_8218Bdrv_ge_mapperInit*/

