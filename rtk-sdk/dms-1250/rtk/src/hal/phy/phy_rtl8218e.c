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
 * Purpose : PHY 8218E Driver APIs.
 *
 * Feature : PHY 8218E Driver APIs
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
#include <hal/phy/phy_rtl8218e.h>
#include <hal/phy/phy_rtl8218e_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <rtk/sds.h>
#include <osal/time.h>
#include <osal/memory.h>


/*
 * Symbol Definition
 */
#define PHY_8218E_MAX_LED                       36
#define PHY_8218E_MAX_MDI                       8

#define PHY_8218E_MAX_PPS_WIDTH                 630


#define PHY_8218E_RTCT_CABLE_FACTOR_5E          (7820)
#define PHY_8218E_RTCT_CABLE_FACTOR_6A          (7820)
#define PHY_8218E_RTCT_CABLE_FACTOR_6A_UU       (7820)
#define PHY_8218E_RTCT_CABLE_FACTOR             (PHY_8218E_RTCT_CABLE_FACTOR_5E)
#define PHY_8218E_RTCT_CABLE_OFFSET_5E          (236)
#define PHY_8218E_RTCT_CABLE_OFFSET_6A          (236)
#define PHY_8218E_RTCT_CABLE_OFFSET_6A_UU       (236)
#define PHY_8218E_RTCT_CABLE_OFFSET             (PHY_8218E_RTCT_CABLE_OFFSET_5E)
#define PHY_8218E_RTCT_ENABLE                   (0)
#define PHY_8218E_RTCT_CH_A                     (4)
#define PHY_8218E_RTCT_CH_B                     (5)
#define PHY_8218E_RTCT_CH_C                     (6)
#define PHY_8218E_RTCT_CH_D                     (7)
#define PHY_8218E_RTCT_RSV                      (8)
#define PHY_8218E_RTCT_DONE                     (15)

#define PHY_8218E_GREEN_EN_OFFSET               0
#define PHY_8218E_GREEN_EN_MASK                 (0xFF << PHY_8218E_GREEN_EN_OFFSET)
#define PHY_8218E_GREEN_EN_PORT_OFFSET(phyPort) ((phyPort & 0x7) + PHY_8218E_GREEN_EN_OFFSET)
#define PHY_8218E_GREEN_EN_PORT_MASK(phyPort)   (0x1 << PHY_8218E_GREEN_EN_PORT_OFFSET(phyPort))

/* Serial LED Configuration register definition */
#define PHY_8218E_SERIAL_LED_CONFIG_PAGE                            0x280
#define PHY_8218E_SERIAL_LED_CONFIG_REG                             16
#define PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET        8
#define PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK          (0xF << PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET)
#define PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET   6
#define PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK     (0x3 << PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET)
#define PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET   4
#define PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK     (0x3 << PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET)
#define PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET      3
#define PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK        (0x1 << PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET)

/* Serial LED PER LED Control register definition */
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET                12
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_MDI_MASK                  (0xF << PHY_8218E_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET        11
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_MASK          (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET         10
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_SPEED_MASK           (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET          9
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_SPEED_MASK            (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET          7
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_ACT_MASK            (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET           6
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_ACT_MASK             (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET            5
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_ACT_MASK              (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET             3
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_DUPLEX_MASK               (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET          2
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_COLLISION_MASK            (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET             1
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_TX_ACT_MASK               (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET)
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET             0
#define PHY_8218E_SERIAL_LED_PER_LED_CTRL_RX_ACT_MASK               (0x1 << PHY_8218E_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET)

#define PHY_8218E_MDI_LB_MDI_VAL_GET(d)         REG32_FIELD_GET(d, 10, (0x3 << 10))
#define PHY_8218E_MDI_LB_MDI_VAL_SET(d, v)      REG32_FIELD_SET(d, v, 10, (0x3 << 10))

typedef struct phy_rtl8218e_info_s
{
    uint8   mdi_val_org[RTK_MAX_NUM_OF_PORTS];
    uint8   rtctCable[RTK_MAX_PORT_PER_UNIT];
    uint32  cable_factor[RTK_MAX_PORT_PER_UNIT];
} phy_rtl8218e_info_t;


/*
 * Data Declaration
 */
phy_rtl8218e_info_t    *rtl8218e_info[RTK_MAX_NUM_OF_UNIT] = { NULL };

/* PTP Register Definition */
#define RTL8218E_PTP_0_PAGE                                         0x2c0
#define RTL8218E_PTP_1_PAGE                                         0x2c1
#define RTL8218E_PTP_2_PAGE                                         0x2c2
#define RTL8218E_PTP_3_PAGE                                         0x2c3
#define RTL8218E_PTP_4_PAGE                                         0x2c4
#define RTL8218E_PTP_5_PAGE                                         0x2c5
#define RTL8218E_PTP_8_PAGE                                         0x2c8
#define RTL8218E_PTP_9_PAGE                                         0x2c9
#define RTL8218E_PTP_A_PAGE                                         0x2ca
#define RTL8218E_PTP_B_PAGE                                         0x2cb
#define RTL8218E_PTP_C_PAGE                                         0x2cc
#define RTL8218E_PTP_D_PAGE                                         0x2cd
#define RTL8218E_PTP_E_PAGE                                         0x2ce
#define RTL8218E_PTP_F_PAGE                                         0x2cf
#define RTL8218E_PTP_MS1_C_PAGE                                     0x34c

#define RTL8218E_PTP_TMR_RG00_REG                                   0x10
#define RTL8218E_PTP_TMR_RG01_REG                                   0x11
#define RTL8218E_PTP_TMR_RG02_REG                                   0x12
#define RTL8218E_PTP_TMR_RG03_REG                                   0x13
#define RTL8218E_PTP_TMR_RG04_REG                                   0x14
#define RTL8218E_PTP_TMR_RG05_REG                                   0x15
#define RTL8218E_PTP_TMR_RG06_REG                                   0x16
#define RTL8218E_PTP_TMR_RG07_REG                                   0x17
#define RTL8218E_PTP_TMR_RG08_REG                                   0x10
#define RTL8218E_PTP_TMR_RG09_REG                                   0x11
#define RTL8218E_PTP_TMR_RG10_REG                                   0x12
#define RTL8218E_PTP_TMR_RG11_REG                                   0x13
#define RTL8218E_PTP_TMR_RG12_REG                                   0x14
#define RTL8218E_PTP_TMR_RG13_REG                                   0x15
#define RTL8218E_PTP_TMR_RG14_REG                                   0x16
#define RTL8218E_PTP_TMR_RG15_REG                                   0x17
#define RTL8218E_PTP_TMR_RG16_REG                                   0x10
#define RTL8218E_PTP_TMR_RG17_REG                                   0x11
#define RTL8218E_PTP_TMR_RG18_REG                                   0x12
#define RTL8218E_PTP_TMR_RG19_REG                                   0x13
#define RTL8218E_PTP_TMR_RG20_REG                                   0x14
#define RTL8218E_PTP_TMR_RG21_REG                                   0x15
#define RTL8218E_PTP_TMR_RG22_REG                                   0x16
#define RTL8218E_PTP_TMR_RG23_REG                                   0x17
#define RTL8218E_PTP_TMR_RG24_REG                                   0x10
#define RTL8218E_PTP_TMR_RG25_REG                                   0x11
#define RTL8218E_PTP_TMR_RG26_REG                                   0x12
#define RTL8218E_PTP_TMR_RG27_REG                                   0x13
#define RTL8218E_PTP_TMR_RG28_REG                                   0x14
#define RTL8218E_PTP_TMR_RG29_REG                                   0x15
#define RTL8218E_PTP_TMR_RG30_REG                                   0x16
#define RTL8218E_PTP_TMR_RG31_REG                                   0x17
#define RTL8218E_PTP_TMR_RG32_REG                                   0x10
#define RTL8218E_PTP_TMR_RG33_REG                                   0x11
#define RTL8218E_PTP_TMR_RG34_REG                                   0x12
#define RTL8218E_PTP_TMR_RG35_REG                                   0x13
#define RTL8218E_PTP_TMR_RG36_REG                                   0x14
#define RTL8218E_PTP_TMR_RG37_REG                                   0x15
#define RTL8218E_PTP_TMR_RG38_REG                                   0x16
#define RTL8218E_PTP_TMR_RG39_REG                                   0x17
#define RTL8218E_PTP_TMR_RG40_REG                                   0x10
#define RTL8218E_PTP_TMR_RG41_REG                                   0x11
#define RTL8218E_PTP_TMR_RG42_REG                                   0x12
#define RTL8218E_PTP_TMR_RG43_REG                                   0x13
#define RTL8218E_PTP_TMR_RG44_REG                                   0x14
#define RTL8218E_PTP_TMR_RG45_REG                                   0x15
#define RTL8218E_PTP_TMR_RG63_REG                                   0x17
#define RTL8218E_PTP_GPTP0_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP0_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP0_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP0_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP1_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP1_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP1_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP1_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP2_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP2_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP2_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP2_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP3_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP3_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP3_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP3_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP4_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP4_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP4_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP4_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP5_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP5_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP5_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP5_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP6_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP6_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP6_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP6_RG07_REG                                 0x17
#define RTL8218E_PTP_GPTP7_RG00_REG                                 0x10
#define RTL8218E_PTP_GPTP7_RG01_REG                                 0x11
#define RTL8218E_PTP_GPTP7_RG02_REG                                 0x12
#define RTL8218E_PTP_GPTP7_RG07_REG                                 0x17
#define RTL8218E_PTP_MS1_RG61_REG                                   0x11

#define RTL8218E_PTP_MAC_ADDR_L_REG_FIELD_MASK                      (0xFFFF << 0)
#define RTL8218E_PTP_MAC_ADDR_M_REG_FIELD_MASK                      (0xFFFF << 0)
#define RTL8218E_PTP_MAC_ADDR_H_REG_FIELD_MASK                      (0xFFFF << 0)
#define RTL8218E_PTP_RSV_REG_FIELD_MASK                             (0x3F << 10)
#define RTL8218E_PTP_CFG_MAC_RNG_REG_FIELD_MASK                     (0x3FF << 0)
#define RTL8218E_PTP_OTAG_TPID_0_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_ITAG_TPID_0_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_OTAG_TPID_1_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_ITAG_TPID_1_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_OTAG_TPID_2_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_ITAG_TPID_2_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_OTAG_TPID_3_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_ITAG_TPID_3_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_CFG_PTP_TIME_FREQ0_REG_FIELD_MASK              (0xFFFF << 0)
#define RTL8218E_PTP_RD_APPLY_FREQ_BUSY_REG_FIELD_MASK              (0x1 << 15)
#define RTL8218E_PTP_APPLY_FREQ_REG_FIELD_MASK                      (0x1 << 14)
#define RTL8218E_PTP_CFG_PTP_TIME_FREQ_RSV_REG_FIELD_MASK           (0x1 << 13)
#define RTL8218E_PTP_CFG_PTP_TIME_FREQ1_REG_FIELD_MASK              (0x1FFF << 0)
#define RTL8218E_PTP_CUR_PTP_TIME_FREQ0_REG_FIELD_MASK              (0xFFFF << 0)
#define RTL8218E_PTP_CUR_PTP_TIME_FREQ1_REG_FIELD_MASK              (0x1FFF << 0)
#define RTL8218E_PTP_CFG_PTP_TIME_NSEC_L_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_CFG_PTP_TIME_NSEC_RSV_REG_FIELD_MASK           (0x3 << 14)
#define RTL8218E_PTP_CFG_PTP_TIME_NSEC_H_REG_FIELD_MASK             (0x3FFF << 16)
#define RTL8218E_PTP_CFG_PTP_TIME_SEC_L_REG_FIELD_MASK              (0xFFFF << 0)
#define RTL8218E_PTP_CFG_PTP_TIME_SEC_M_REG_FIELD_MASK              (0xFFFF << 0)
#define RTL8218E_PTP_CFG_PTP_TIME_SEC_H_REG_FIELD_MASK              (0xFFFF << 0)
#define RTL8218E_PTP_PTP_TIME_RSV_REG_FIELD_MASK                    (0x1FFF << 3)
#define RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK                   (0x1 << 2)
#define RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK                    (0x3 << 0)
#define RTL8218E_PTP_RD_PTP_TIME_NSEC_L_REG_FIELD_MASK              (0xFFFF << 0)
#define RTL8218E_PTP_RD_PTP_TIME_NSEC_H_REG_FIELD_MASK              (0x3FFF << 16)
#define RTL8218E_PTP_RD_PTP_TIME_SEC_L_REG_FIELD_MASK               (0xFFFF << 0)
#define RTL8218E_PTP_RD_PTP_TIME_SEC_M_REG_FIELD_MASK               (0xFFFF << 0)
#define RTL8218E_PTP_RD_PTP_TIME_SEC_H_REG_FIELD_MASK               (0xFFFF << 0)
#define RTL8218E_PTP_CLKOUT_PTP_TIME_NSEC_L_REG_FIELD_MASK          (0xFFFF << 0)
#define RTL8218E_PTP_CLKOUT_PTP_RSV_REG_FIELD_MASK                  (0x3 << 14)
#define RTL8218E_PTP_CLKOUT_PTP_TIME_NSEC_H_REG_FIELD_MASK          (0x3FFF << 0)
#define RTL8218E_PTP_CLKOUT_PTP_TIME_SEC_L_REG_FIELD_MASK           (0xFFFF << 0)
#define RTL8218E_PTP_CLKOUT_PTP_TIME_SEC_M_REG_FIELD_MASK           (0xFFFF << 0)
#define RTL8218E_PTP_CLKOUT_PTP_TIME_SEC_H_REG_FIELD_MASK           (0xFFFF << 0)
#define RTL8218E_PTP_CFG_PULSE_RSV_REG_FIELD_MASK                   (0x1FFF << 3)
#define RTL8218E_PTP_CFG_PULSE_MODE_REG_FIELD_MASK                  (0x1 << 2)
#define RTL8218E_PTP_CFG_CLKOUT_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_RD_CLKOUT_RUN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_CFG_CLKOUT_HALF_PERIOD_NS_L_REG_FIELD_MASK     (0xFFFF << 0)
#define RTL8218E_PTP_CFG_CLKOUT_HALF_PERIOD_RSV_REG_FIELD_MASK      (0x3 << 14)
#define RTL8218E_PTP_CFG_CLKOUT_HALF_PERIOD_NS_H_REG_FIELD_MASK     (0x3FFF << 0)
#define RTL8218E_PTP_CFG_GPI_RSV_15_7_REG_FIELD_MASK                (0x1FF << 7)
#define RTL8218E_PTP_CFG_GPI_OP_REG_FIELD_MASK                      (0x7 << 4)
#define RTL8218E_PTP_CFG_GPI_RISE_TRIG_REG_FIELD_MASK               (0x1 << 3)
#define RTL8218E_PTP_CFG_GPI_FALL_TRIG_REG_FIELD_MASK               (0x1 << 2)
#define RTL8218E_PTP_CFG_GPI_RSV_1_0_REG_FIELD_MASK                 (0x3 << 0)
#define RTL8218E_PTP_CFG_PPS_RSV_REG_FIELD_MASK                     (0x1FF << 7)
#define RTL8218E_PTP_CFG_PPS_EN_REG_FIELD_MASK                      (0x1 << 6)
#define RTL8218E_PTP_CFG_PPS_WIDTH_REG_FIELD_MASK                   (0x3F << 0)
#define RTL8218E_PTP_RD_TX_TIMESTAMP_VALID_REG_FIELD_MASK           (0x1 << 15)
#define RTL8218E_PTP_RD_PORT_ID_REG_FIELD_MASK                      (0x3F << 8)
#define RTL8218E_PTP_RD_MSG_TYPE_REG_FIELD_MASK                     (0x3 << 6)
#define RTL8218E_PTP_RD_SEQ_ID_H_REG_FIELD_MASK                     (0x3F << 0)
#define RTL8218E_PTP_RD_SEQ_ID_L_REG_FIELD_MASK                     (0x3FF << 6)
#define RTL8218E_PTP_RD_TX_TIMESTAMP_SEC_H_REG_FIELD_MASK           (0x3F << 0)
#define RTL8218E_PTP_RD_TX_TIMESTAMP_SEC_L_REG_FIELD_MASK           (0x3 << 14)
#define RTL8218E_PTP_RD_TX_TIMESTAMP_NSEC_H_REG_FIELD_MASK          (0x3FFF << 0)
#define RTL8218E_PTP_RD_TX_TIMESTAMP_NSEC_L_REG_FIELD_MASK          (0xFFFF << 0)
#define RTL8218E_PTP_CFG_MIB_REQUEST_TYPE_REG_FIELD_MASK            (0x7F << 9)
#define RTL8218E_PTP_CFG_MIB_EN_REG_FIELD_MASK                      (0x1 << 8)
#define RTL8218E_PTP_RD_ISR_PTP_REG_FIELD_MASK                      (0x1 << 1)
#define RTL8218E_PTP_CFG_IMR_PTP_REG_FIELD_MASK                     (0x1 << 0)
#define RTL8218E_PTP_CFG_DBG_SEL_MODE_REG_FIELD_MASK                (0xFF << 8)
#define RTL8218E_PTP_CFG_DBG_SEL_PTP_REG_FIELD_MASK                 (0xF << 4)
#define RTL8218E_PTP_CFG_DBG_SEL_INST_REG_FIELD_MASK                (0xF << 0)
#define RTL8218E_PTP_CFG_CLK_RSV_REG_FIELD_MASK                     (0x3FFF << 2)
#define RTL8218E_PTP_CFG_EXT_CLK_SRC_REG_FIELD_MASK                 (0x1 << 1)
#define RTL8218E_PTP_CFG_CLK_SRC_REG_FIELD_MASK                     (0x1 << 0)
#define RTL8218E_PTP_CFG_CLKOUT_HALF_PERIOD_FS_L_REG_FIELD_MASK     (0xFFFF << 0)
#define RTL8218E_PTP_CFG_CLKOUT_RSV_REG_FIELD_MASK                  (0x7F << 9)
#define RTL8218E_PTP_CFG_CLKOUT_HALF_PERIOD_FS_H_REG_FIELD_MASK     (0x1FF << 0)
#define RTL8218E_PTP_PTP_VERSION_REG_FIELD_MASK                     (0xFFFF << 0)
#define RTL8218E_PTP_P0_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P0_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P0_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P0_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P0_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P0_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P0_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P0_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P0_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P0_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P0_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P0_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P1_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P1_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P1_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P1_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P1_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P1_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P1_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P1_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P1_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P1_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P1_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P1_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P2_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P2_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P2_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P2_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P2_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P2_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P2_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P2_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P2_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P2_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P2_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P2_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P3_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P3_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P3_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P3_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P3_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P3_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P3_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P3_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P3_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P3_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P3_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P3_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P4_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P4_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P4_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P4_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P4_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P4_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P4_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P4_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P4_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P4_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P4_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P4_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P5_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P5_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P5_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P5_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P5_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P5_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P5_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P5_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P5_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P5_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P5_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P5_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P6_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P6_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P6_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P6_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P6_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P6_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P6_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P6_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P6_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P6_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P6_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P6_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_P7_CFG_LINK_DELAY_L_REG_FIELD_MASK             (0x3FF << 6)
#define RTL8218E_PTP_P7_CFG_ALWAYS_TS_REG_FIELD_MASK                (0x1 << 5)
#define RTL8218E_PTP_P7_CFG_TRAN_REG_FIELD_MASK                     (0x1 << 4)
#define RTL8218E_PTP_P7_CFG_TX_TIME_W_REG_FIELD_MASK                (0x1 << 3)
#define RTL8218E_PTP_P7_CFG_RX_TIME_W_REG_FIELD_MASK                (0x1 << 2)
#define RTL8218E_PTP_P7_CFG_UDP_EN_REG_FIELD_MASK                   (0x1 << 1)
#define RTL8218E_PTP_P7_CFG_ETH_EN_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P7_CFG_LINK_DELAY_H_REG_FIELD_MASK             (0xFFFF << 0)
#define RTL8218E_PTP_P7_PTP_DUMMY_REG_FIELD_MASK                    (0x3FFF << 2)
#define RTL8218E_PTP_P7_CFG_V6_EVEN_REG_FIELD_MASK                  (0x1 << 1)
#define RTL8218E_PTP_P7_CFG_BYPASS_REG_FIELD_MASK                   (0x1 << 0)
#define RTL8218E_PTP_P7_RSV_REG_FIELD_MASK                          (0xFFFF << 0)
#define RTL8218E_PTP_CFG_PTP_CLK_OUT_E_REG_FIELD_MASK               (0x1 << 1)
#define RTL8218E_PTP_CFG_PTP_CLK_OUT_SEL_REG_FIELD_MASK             (0x1 << 0)

#define PAGE_8218E_PTP_OFFSET_PORT(port)        (port % 8)

#define REG_FIELD_8218E_PTP_TIME_CMD_READ       0
#define REG_FIELD_8218E_PTP_TIME_CMD_WRITE      1
#define REG_FIELD_8218E_PTP_TIME_CMD_ADJUST     2

rt_phyInfo_t phy_8218E_info =
{
    .phy_num    = PORT_NUM_IN_8218E,
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
phy_8218e_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


int32
_phy_8218e_indr_read(uint32 unit, rtk_port_t port, uint32 indr_addr, uint32 *pData)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = hal_miim_write(unit, port, 0, 27, indr_addr)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, port, 0, 28, &phyData)) != RT_ERR_OK)
        return ret;

    *pData = phyData;

    return RT_ERR_OK;
}

int32
_phy_8218e_indr_write(uint32 unit, rtk_port_t port, uint32 indr_addr, uint32 phyData)
{
    int32   ret = RT_ERR_OK;

    if ((ret = hal_miim_write(unit, port, 0, 27, indr_addr)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 0, 28, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_8218e_indr_bits_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      indr_addr,
    uint32      msb,
    uint32      lsb,
    uint32      val)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  mask = 0;

    if ((ret = _phy_8218e_indr_read(unit, port, indr_addr, &phyData)) != RT_ERR_OK)
        return ret;

    mask = UINT32_BITS_MASK(msb, lsb);
    phyData = REG32_FIELD_SET(phyData, val, lsb, mask);

    if ((ret = _phy_8218e_indr_write(unit, port, indr_addr, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
}


/* Function Name:
 *      phy_8218e_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pChip_rev_id    - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *pChip_rev_id)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;
    uint32      blk = 0;
    uint32      data = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, basePort, 0, 30, &blk), ret);

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 8), ret);
    RT_ERR_CHK(hal_miim_read(unit, basePort, 0x327, 0x15, &data), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, blk), ret);

    *pChip_rev_id = (uint16) REG32_FIELD_GET(data, 0, 0x3F);

    return RT_ERR_OK;
}


int32
_phy_8218e_serdes_autoNego_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    int32   ret = RT_ERR_OK;
    uint32  basePort = 0;
    uint32  blk = 0;
    uint32  phyData = 0;
    uint32  temp = 0;
    uint32  park_page = 0x1F;

    basePort = HWP_PHY_BASE_MACID(unit, port);

    RT_ERR_CHK(hal_miim_read(unit, basePort, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 8), ret);

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_SERDES_XSGMII_AN:
            RT_ERR_HDL(hal_miim_park_read(unit, basePort, 0x400, park_page, 0x12, &phyData), ERR, ret);
            temp = (phyData & BIT_8) ? (0) : (1);
            break;

        default:
            ret = RT_ERR_INPUT;
            goto ERR;
    }

    *pValue = temp;

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

int32
_phy_8218e_serdes_autoNego_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    int32   ret = RT_ERR_OK;
    uint32  basePort = 0;
    uint32  blk = 0;
    uint32  phyData = 0;
    uint32  park_page = 0x1F;

    basePort = HWP_PHY_BASE_MACID(unit, port);

    RT_ERR_CHK(hal_miim_read(unit, basePort, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 8), ret);

    /* N-way auto mode */
    if (value)
        phyData = 0x0;
    /* N-way force mode */
    else
        phyData = 0x1;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_SERDES_XSGMII_AN:
            RT_ERR_HDL(hal_miim_parkBits_write(unit, basePort, 0x400, park_page, 0x12, 9, 8, phyData), ERR, ret);
            break;

        default:
            ret = RT_ERR_INPUT;
            goto ERR;
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

int32
_phy_8218e_rtct_cable_commonParam_set(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  fix_page = 0x1F;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8194);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8185);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x25);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8186);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x2E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8187);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xE0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8188);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x3E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8189);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x80);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x818A);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x818B);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x4);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x818C);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x818D);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x3E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x818E);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x80);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8194);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x49);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8195);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8197);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8198);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xC);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x819A);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xE4);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x819B);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x819C);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x819D);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x819E);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x819F);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A0);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xE0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A1);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A2);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x4);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A3);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A4);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x4);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A5);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A6);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x50);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A7);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A8);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x50);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81A9);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x80);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81AA);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x28);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81AB);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81AC);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xE);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81AD);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81AE);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x46);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81AF);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B0);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xEA);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B1);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B2);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x4);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B3);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B4);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B5);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B6);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x2C);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B7);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B8);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xF6);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81B9);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81BA);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C3);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x16);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C4);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x80);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C5);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFD);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C6);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xCB);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C7);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C8);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFC);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81C9);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81CA);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xBE);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81CB);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x17);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81CC);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81CD);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFE);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81CE);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xB6);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81CF);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81D0);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x66);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81D1);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81D2);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xDC);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81DB);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xE3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81DC);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x5E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81DD);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81DE);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x97);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81DF);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E0);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFC);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E1);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E2);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xE8);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E3);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xD5);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E4);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xCE);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E5);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E6);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x89);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E7);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E8);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x24);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81E9);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81EA);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81EB);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xB);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81EC);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81ED);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81EE);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81EF);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F0);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F1);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F2);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F3);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xA);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F4);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x58);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F5);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0xFF);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F6);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x69);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F7);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F8);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0A);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81F9);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81FA);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x6);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81FB);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x4);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81FC);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0F);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81FD);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81FE);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x81FF);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8200);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8201);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8202);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x0);

    return ret;
}

int32
_phy_8218e_rtct_cable_set(uint32 unit, rtk_port_t port, uint32 cableType)
{
    int32   ret = RT_ERR_OK;

    if (cableType == rtl8218e_info[unit]->rtctCable[port])
    {
        return RT_ERR_OK;
    }

    switch (cableType)
    {
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E:
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A:
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A_UU:
            ret = _phy_8218e_rtct_cable_commonParam_set(unit, port);
            break;

        default:
            return RT_ERR_INPUT;
    }

    rtl8218e_info[unit]->rtctCable[port] = cableType;
    return ret;
}

/* Function Name:
 *      phy_8218e_ctrl_get
 * Description:
 *      Get PHY settings for specific feature
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      ctrl_type   - setting type
 * Output:
 *      pValue      - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8218e_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    int32   ret = RT_ERR_OK;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_SERDES_XSGMII_AN:
            ret = _phy_8218e_serdes_autoNego_get(unit, port, ctrl_type, pValue);
            break;

        case RTK_PHY_CTRL_RTCT_CABLE_FACTOR:
            *pValue = rtl8218e_info[unit]->cable_factor[port];
            break;

        default:
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

    return ret;
}

/* Function Name:
 *      phy_8218e_ctrl_set
 * Description:
 *      Set PHY settings for specific feature
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      ctrl_type   - setting type
 *      value       - setting value
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
phy_8218e_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    int32   ret = RT_ERR_OK;

    switch (ctrl_type)
    {
        case RTK_PHY_CTRL_SERDES_XSGMII_AN:
            ret = _phy_8218e_serdes_autoNego_set(unit, port, ctrl_type, value);
            break;

        case RTK_PHY_CTRL_RTCT_CABLE_TYPE:
            ret = _phy_8218e_rtct_cable_set(unit, port, value);
            break;

        case RTK_PHY_CTRL_RTCT_CABLE_FACTOR:
            rtl8218e_info[unit]->cable_factor[port] = value;
            break;

        default:
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

    return ret;
}


int32
_phy_8218e_powerSavingMode_leave(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    int32   wait_cnt = 0;
    uint32  phyData = 0;
    uint32  org_phyData = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &org_phyData), ret);

    phyData = org_phyData | (Reset_MASK);
    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData), ret);
    while (1)
    {
        RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData), ret);
        if((phyData & Reset_MASK) == 0)
            break;

        wait_cnt++;
        if (wait_cnt > 500)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PORT), "unit=%d, port=%d, enable reset expire", unit, port);
            break;
        }
    }

    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, org_phyData), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_enable_set
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
phy_8218e_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret = RT_ERR_OK;
    rtk_enable_t    cur_en = DISABLED;

    if (ENABLED == enable)
    {
        RT_ERR_CHK(phy_common_enable_get(unit, port, &cur_en), ret);
        if (DISABLED == cur_en)
        {
            /* do powerSavingMode_leave when enable state change from disable to enable */
            if ((ret = _phy_8218e_powerSavingMode_leave(unit, port)) != RT_ERR_OK)
                return ret;
        }
    }

    RT_ERR_CHK(phy_common_enable_set(unit, port, enable), ret);

    return RT_ERR_OK;
}


void
_phy_8218e_ieeeTestModeAllPhyPort_disable(uint32 unit, rtk_port_t phyBasePort, uint32 phyPortCnt)
{
    rtk_port_t  phyPort = 0;

    for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
    {
        /* Disable Test mode */
        hal_miim_write(unit, (phyBasePort+phyPort), 0xA41, 9, 0x0E00);
    }
}

/* Function Name:
 *      phy_8218e_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pTestMode->mode     - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags    -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_NOT_ALLOWED          - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED   - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS only support in mode 4
 */
int32
phy_8218e_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    rtk_port_t  phyBasePort = HWP_PHY_BASE_MACID(unit, port);
    uint32      phyPortCnt = 0;
    uint32      value = 0;
    uint32      chn = 0;

    osal_printf("RTL8218E Test Mode (PHYID: %u)\n", port);

    if ((pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS) && (pTestMode->mode != RTK_PORT_PHY_1G_TEST_MODE4))
    {
        osal_printf("mode %u not support all-phy-port operation.\n", pTestMode->mode);
        return RT_ERR_NOT_ALLOWED;
    }

    switch (HWP_PHY_MODEL_BY_PORT(unit, port))
    {
        case RTK_PHYTYPE_RTL8218E:
            phyPortCnt = PORT_NUM_IN_8218E;
            break;
        default:
            osal_printf("port type not support.\n");
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

    switch (pTestMode->mode)
    {
        case RTK_PORT_PHY_1G_TEST_MODE1:
        {
            _phy_8218e_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            /* Test mode 1 */
            hal_miim_write(unit, port, 0xA41, 9, 0x2E00);
            return RT_ERR_OK;
        }

        case RTK_PORT_PHY_1G_TEST_MODE4:
        {
            uint32  phyPort = 0;

            /* 0=A,1=B,2=C,3=D */
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
                    hal_miim_write(unit, (phyBasePort+phyPort), 0xBC0, 18, 0x0000);
                    hal_miim_write(unit, (phyBasePort+phyPort), 0xBC0, 19, 0x01C0);
                    /* Disable Test mode */
                    hal_miim_write(unit, (phyBasePort+phyPort), 0xA41, 9, 0x0E00);
                }

                value = (0x1 << chn);
                hal_miim_write(unit, port, 0xBC0, 18, value);
                /* Enable get power control */
                hal_miim_write(unit, port, 0xBC0, 19, 0x01D0);
                /* Enable Test Mode 4 */
                hal_miim_write(unit, port, 0xA41, 9, 0x8E00);

                return RT_ERR_OK;
            }
            else
            {
                osal_printf("Not support all-phy-port operation.\n");
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
        }

        default:
            osal_printf("The mode (%u) is not suppoted.\n", pTestMode->mode);
            return RT_ERR_PORT_NOT_SUPPORTED;
    }
}


/* Function Name:
 *      phy_8218e_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side serdes mode
 * Input:
 *      unit    - unit id
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
phy_8218e_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, 0x260, 18, &data), ERR, ret);
    switch (data & 0xF0)
    {
        case 0xD0:
            *pMode = RTK_MII_QSGMII;
            break;
        case 0xB0:
            *pMode = RTK_MII_XSGMII;
            break;
        case 0xF0:
            *pMode = RTK_MII_QUSGMII;
            break;
        case 0x90:
            *pMode = RTK_MII_OUSGMII;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_PORT), "unit %u port %u SDS mode:0x%X unknown type.", unit, port, (data & 0xF0));
            ret = RT_ERR_FAILED;
            goto ERR;
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      phy_8218e_greenEnable_get
 * Description:
 *      Get the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-up green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218e is supported the per-port link-up green feature.
 */
int32
phy_8218e_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;
    rtk_port_t  phyPort = 0;
    uint32      phyData = 0;
    uint32      value = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;
    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218E)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x8), ret);

    RT_ERR_CHK(hal_miim_read(unit, basePort, 0x360, 0x13, &phyData), ret);

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0), ret);

    value = REG32_FIELD_GET(phyData, PHY_8218E_GREEN_EN_PORT_OFFSET(phyPort), PHY_8218E_GREEN_EN_PORT_MASK(phyPort));
    if (value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of link-up  green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218e is supported the per-port link-up green feature.
 */
int32
phy_8218e_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;
    rtk_port_t  phyPort = 0;
    uint32      data = 0;
    uint32      data0 = 0;
    uint32      data1 = 0;
    uint32      data2 = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;
    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218E)
        return RT_ERR_PORT_ID;

    if (ENABLED == enable)
    {
        RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x8), ret);
        RT_ERR_CHK(hal_miim_read(unit, basePort, 0x360, 0x13, &data), ret);
        data = REG32_FIELD_SET(data, 1, PHY_8218E_GREEN_EN_PORT_OFFSET(phyPort), PHY_8218E_GREEN_EN_PORT_MASK(phyPort));
        RT_ERR_CHK(hal_miim_write(unit, basePort, 0x360, 0x13, data), ret);
        RT_ERR_CHK(hal_miim_read(unit, basePort, 0x360, 0x10, &data0), ret);
        RT_ERR_CHK(hal_miim_read(unit, basePort, 0x360, 0x11, &data1), ret);
        RT_ERR_CHK(hal_miim_read(unit, basePort, 0x360, 0x12, &data2), ret);
        RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0), ret);

        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x805D), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 28, data0), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x8061), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 28, data1), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x804D), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 28, data2), ret);
    }
    else
    {
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x8065), ret);
        RT_ERR_CHK(hal_miim_read(unit, port, 0, 28, &data0), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x8069), ret);
        RT_ERR_CHK(hal_miim_read(unit, port, 0, 28, &data1), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x8055), ret);
        RT_ERR_CHK(hal_miim_read(unit, port, 0, 28, &data2), ret);

        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x805D), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 28, data0), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x8061), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 28, data1), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 27, 0x804D), ret);
        RT_ERR_CHK(hal_miim_write(unit, port, 0, 28, data2), ret);

        RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x8), ret);
        RT_ERR_CHK(hal_miim_read(unit, basePort, 0x360, 0x13, &data), ret);
        data = REG32_FIELD_SET(data, 0, PHY_8218E_GREEN_EN_PORT_OFFSET(phyPort), PHY_8218E_GREEN_EN_PORT_MASK(phyPort));
        RT_ERR_CHK(hal_miim_write(unit, basePort, 0x360, 0x13, data), ret);
        RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0), ret);
    }

    return RT_ERR_OK;
}


#if !defined(__BOOTLOADER__)

/* Function Name:
 *      phy_8218e_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pMedia  - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The media type is return PORT_MEDIA_COPPER.
 */
int32
phy_8218e_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pAbility    - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218e_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData4 = 0;
    uint32          phyData9 = 0;
    rtk_enable_t    enable = DISABLED;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4), ret);

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9), ret);

    pAbility->Full_100 = (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100 = (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10 = (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10 = (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
    pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
    pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;

    return ret;
}

/* Function Name:
 *      phy_8218e_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pAbility    - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218e_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData0 = 0;
    uint32          phyData4 = 0;
    uint32          phyData9 = 0;
    rtk_enable_t    enable = DISABLED;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4), ret);

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
            | (pAbility->FC << Pause_R4_OFFSET)
            | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9), ret);

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

    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4), ret);

    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9), ret);

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0), ret);

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0), ret);
    }

    return ret;
}

/* Function Name:
 *      phy_8218e_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218e_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret = RT_ERR_OK;
    uint32  value = 0;
    uint32  phyData0 = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &value), ret);

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0), ret);

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
              | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

    return ret;
}

/* Function Name:
 *      phy_8218e_speed_set
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
phy_8218e_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData0 = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0), ret);

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0), ret);

    return ret;
}


/* Function Name:
 *      phy_8218e_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_patch_set(uint32 unit, rtk_port_t port)
{
    return (sub_phy_8218e_patch_set(unit, port));
}

/* Function Name:
 *      phy_8218e_patch_perPort_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_patch_perPort_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pRst_portmask)
{
    return (sub_phy_8218e_patch_perPort_set(unit, port, pRst_portmask));
}


int32
_phy_8218e_rtctChannelLen_convert(uint32 phyData, uint32 cable_factor, uint32 cable_offset)
{
    if (phyData <= cable_offset)
        return 0;

    return (((phyData - cable_offset) * 100 * 100) / cable_factor);
}

/* Function Name:
 *      _phy_8218e_rtctGeStatus_convert
 * Description:
 *      Convert RTCT result status register data to SDK status.
 * Input:
 *      phyData         - result status data of RTCT register
 * Output:
 *      pShort          - short
 *      pOpen           - open
 *      pMismatch       - mismatch
 *      pLinedriver     - not support
 *      pHiImpedance    - not support
 *      pCross          - not support
 *      pPartialCross   - not support
 *      pPairBusy       - not support
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218e_rtctGeStatus_convert(uint32 phyData,
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
    else if (phyData == 0x00C0) /* Interpair short */
    {
        *pShort = 2;
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
 *      phy_8218e_rtct_start
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
phy_8218e_rtct_start(uint32 unit, rtk_port_t port)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0;
    uint32          speed = 0;
    rtk_enable_t    ena = DISABLED;

    if ((ret = phy_common_enable_get(unit, port, &ena)) != RT_ERR_OK)
        return ret;
    if (DISABLED == ena)
        return RT_ERR_OPER_DENIED;

    if (RTK_PHY_CTRL_RTCT_CABLE_TYPE_NONE == rtl8218e_info[unit]->rtctCable[port])
    {
        if ((ret = phy_8218e_ctrl_set(unit, port, RTK_PHY_CTRL_RTCT_CABLE_TYPE, RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E)) != RT_ERR_OK)
            return ret;
    }

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & LinkStatus_MASK)
    {
        phy_common_speed_get(unit, port, &speed);
        if (PORT_SPEED_10M == speed)
            return RT_ERR_PORT_NOT_SUPPORTED;

        return RT_ERR_OK;
    }
    else
    {
        /* RTCT start */
        if ((ret = hal_miim_read(unit, port, 0xa42, 17, &phyData)) != RT_ERR_OK)
            return ret;

        /* Clear RTCT done */
        phyData = REG32_FIELD_SET(phyData, 0, PHY_8218E_RTCT_DONE, (0x1 << PHY_8218E_RTCT_DONE));
        if ((ret = hal_miim_write(unit, port, 0xa42, 17, phyData)) != RT_ERR_OK)
            return ret;

        osal_time_udelay(1 * 1000); /* wait 1ms for chip reset the states */

        phyData &= ~ (1 << PHY_8218E_RTCT_RSV);
        phyData |= (1 << PHY_8218E_RTCT_CH_A);
        phyData |= (1 << PHY_8218E_RTCT_CH_B);
        phyData |= (1 << PHY_8218E_RTCT_CH_C);
        phyData |= (1 << PHY_8218E_RTCT_CH_D);
        phyData |= (1 << PHY_8218E_RTCT_ENABLE);

        if ((ret = hal_miim_write(unit, port, 0xa42, 17, phyData)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_rtctResult_get
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
 *      8218E does not support LineDriver.
 */
int32
phy_8218e_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  speed = 0;
    uint32  cable_factor = 0;
    uint32  cable_offset = 0;

    /* Default cable factor and offset */
    switch (rtl8218e_info[unit]->rtctCable[port])
    {
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A:
            cable_factor = PHY_8218E_RTCT_CABLE_FACTOR_6A;
            cable_offset = PHY_8218E_RTCT_CABLE_OFFSET_6A;
            break;
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A_UU:
            cable_factor = PHY_8218E_RTCT_CABLE_FACTOR_6A_UU;
            cable_offset = PHY_8218E_RTCT_CABLE_OFFSET_6A_UU;
            break;
        case RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E:
        default:
            cable_factor = PHY_8218E_RTCT_CABLE_FACTOR_5E;
            cable_offset = PHY_8218E_RTCT_CABLE_OFFSET_5E;
            break;
    }

    /* Configured cable factor */
    if (rtl8218e_info[unit]->cable_factor[port] != 0)
        cable_factor = rtl8218e_info[unit]->cable_factor[port];

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & LinkStatus_MASK)
    {
        phy_common_speed_get(unit, port, &speed);

        if (PORT_SPEED_10M == speed)
            return RT_ERR_PORT_NOT_SUPPORTED;

        /*
         * If the port link is up,
         * return cable length from Channel Estimation
         */

        /* The Length is store in [7:0], and the unit is meter*/
        if ((ret = hal_miim_read(unit, port, 0xa88, 16, &phyData)) != RT_ERR_OK)
            return ret;

        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        if (PORT_SPEED_100M == speed)
        {
            pRtctResult->linkType = PORT_SPEED_100M;
            pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
            pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
        }
        else
        {
            pRtctResult->linkType = PORT_SPEED_1000M;
            pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
        }
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, 0xa42, 17, &phyData)) != RT_ERR_OK)
            return ret;

        if (((phyData >> PHY_8218E_RTCT_DONE) & 0x1) != 0x1)
            return RT_ERR_PHY_RTCT_NOT_FINISH;

        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        pRtctResult->linkType = PORT_SPEED_1000M;

        /* uc_sram_table: rtct_phase_cnt_0_h */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x802c, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelALen = _phy_8218e_rtctChannelLen_convert(phyData, cable_factor, cable_offset);

        /* uc_sram_table: rtct_phase_cnt_1_h */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x8030, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelBLen = _phy_8218e_rtctChannelLen_convert(phyData, cable_factor, cable_offset);

        /* uc_sram_table: rtct_phase_cnt_2_h */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x8034, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelCLen = _phy_8218e_rtctChannelLen_convert(phyData, cable_factor, cable_offset);

        /* uc_sram_table: rtct_phase_cnt_3_h */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x8038, &phyData)) != RT_ERR_OK)
            return ret;
        pRtctResult->un.ge_result.channelDLen = _phy_8218e_rtctChannelLen_convert(phyData, cable_factor, cable_offset);

        /* uc_sram_table: rtct_cable_status_0_h */
        /* === Channel A Status === */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x802a, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218e_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelAShort, &pRtctResult->un.ge_result.channelAOpen,
                                &pRtctResult->un.ge_result.channelAMismatch, &pRtctResult->un.ge_result.channelALinedriver,
                                &pRtctResult->un.ge_result.channelAHiImpedance, &pRtctResult->un.ge_result.channelACross,
                                &pRtctResult->un.ge_result.channelAPartialCross, &pRtctResult->un.ge_result.channelAPairBusy)) != RT_ERR_OK)
            return ret;

        /* uc_sram_table: rtct_cable_status_1_h */
        /* === Channel B Status === */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x802e, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218e_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelBShort, &pRtctResult->un.ge_result.channelBOpen,
                                &pRtctResult->un.ge_result.channelBMismatch, &pRtctResult->un.ge_result.channelBLinedriver,
                                &pRtctResult->un.ge_result.channelBHiImpedance, &pRtctResult->un.ge_result.channelBCross,
                                &pRtctResult->un.ge_result.channelBPartialCross, &pRtctResult->un.ge_result.channelBPairBusy)) != RT_ERR_OK)
            return ret;

        /* uc_sram_table: rtct_cable_status_2_h */
        /* === Channel C Status === */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x8032, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218e_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelCShort, &pRtctResult->un.ge_result.channelCOpen,
                                &pRtctResult->un.ge_result.channelCMismatch, &pRtctResult->un.ge_result.channelCLinedriver,
                                &pRtctResult->un.ge_result.channelCHiImpedance, &pRtctResult->un.ge_result.channelCCross,
                                &pRtctResult->un.ge_result.channelCPartialCross, &pRtctResult->un.ge_result.channelCPairBusy)) != RT_ERR_OK)
            return ret;

        /* uc_sram_table: rtct_cable_status_3_h */
        /* === Channel D Status === */
        if ((ret = _phy_8218e_indr_read(unit, port, 0x8036, &phyData)) != RT_ERR_OK)
            return ret;

        if ((ret = _phy_8218e_rtctGeStatus_convert(phyData,
                                &pRtctResult->un.ge_result.channelDShort, &pRtctResult->un.ge_result.channelDOpen,
                                &pRtctResult->un.ge_result.channelDMismatch, &pRtctResult->un.ge_result.channelDLinedriver,
                                &pRtctResult->un.ge_result.channelDHiImpedance, &pRtctResult->un.ge_result.channelDCross,
                                &pRtctResult->un.ge_result.channelDPartialCross, &pRtctResult->un.ge_result.channelDPairBusy)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_eeeEnable_get
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
phy_8218e_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;
    uint32  value = 0;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0001), ret);

    RT_ERR_CHK(hal_miim_mmd_read(unit, port, 7, 60, &phyData), ret);
    /* 1000M EEE ability */
    value = REG32_FIELD_GET(phyData, 2, (0x1 << 2));
    if (value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0000), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status of EEE
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
phy_8218e_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret = RT_ERR_OK;
    uint32          phyData = 0;
    uint32          value = 0;
    rtk_enable_t    an_enable = DISABLED;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0001), ret);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    RT_ERR_CHK(hal_miim_mmd_read(unit, port, 7, 60, &phyData), ret);
    /* 1000M EEE ability */
    phyData = REG32_FIELD_SET(phyData, value, 2, (0x1 << 2));
    /* 100M EEE ability */
    phyData = REG32_FIELD_SET(phyData, value, 1, (0x1 << 1));
    ret = hal_miim_mmd_write(unit, port, 7, 60, phyData);
    if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        return ret;

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa42, 20, &phyData), ret);
    /* 500M EEE ability */
    phyData = REG32_FIELD_SET(phyData, value, 7, (0x1 << 7));
    /* the enable status will depending on the chip support 500M or not */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 20, phyData), ret);

    /* Force re-autonegotiation if AN is on */
    phy_common_autoNegoEnable_get(unit, port, &an_enable);
    if (ENABLED == an_enable)
    {
        RT_ERR_CHK(hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData), ret);
        phyData = REG32_FIELD_SET(phyData, 1, RestartAutoNegotiation_OFFSET, RestartAutoNegotiation_MASK);
        RT_ERR_CHK(hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData), ret);
    }

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0000), ret);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pMode   - pointer to cross over mode
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
phy_8218e_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0001), ret);

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &phyData), ret);

    if (phyData & BIT_9)
    {
        if (phyData & BIT_8)
            *pMode = PORT_CROSSOVER_MODE_MDI;
        else
            *pMode = PORT_CROSSOVER_MODE_MDIX;
    }
    else
        *pMode = PORT_CROSSOVER_MODE_AUTO;

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0000), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mode    - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8218e_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0001), ret);

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &phyData), ret);

    switch (mode)
    {
        case PORT_CROSSOVER_MODE_MDI:
            phyData |= BIT_9;
            phyData |= BIT_8;
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            phyData |= BIT_9;
            phyData &= (~(BIT_8));
            break;
        case PORT_CROSSOVER_MODE_AUTO:
            phyData &= (~(BIT_9));
            break;
        default:
            return RT_ERR_INPUT;
    }

    RT_ERR_CHK(hal_miim_write(unit, port, 0xa43, 24, phyData), ret);

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0000), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pMode   - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8218e_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0001), ret);

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 26, &phyData), ret);

    if (phyData & BIT_1)
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
    else
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa42, 30, 0x0000), ret);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_linkDownPowerSavingEnable_get
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
 *      1. The RTL8218e is supported the per-port link-down power saving
 */
int32
phy_8218e_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* For Link-Down Power Saving (per-port) */
    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &phyData), ret);

    if (phyData & BIT_2)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218e is supported the per-port link-down power saving
 */
int32
phy_8218e_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* For Link-Down Power Saving (per-port) */
    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &phyData), ret);

    if (ENABLED == enable)
        phyData |= BIT_2;
    else
        phyData &= ~(BIT_2);

    RT_ERR_CHK(hal_miim_write(unit, port, 0xa43, 24, phyData), ret);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;
    uint32      phyData = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0008), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 31, 0x0266), ret);
    RT_ERR_CHK(hal_miim_read(unit, basePort, 0, 22, &phyData), ret);

    if (enable)
        phyData |= 0xFF80;
    else
        phyData &= ~(0xFF80);

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 22, phyData), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0000), ret);

    return ret;
}

/* Function Name:
 *      phy_8218e_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      broadcastID - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218e_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;
    uint32      phyData = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0008), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 31, 0x0266), ret);
    RT_ERR_CHK(hal_miim_read(unit, basePort, 0, 22, &phyData), ret);

    phyData &= ~(0x1f);
    phyData |= (broadcastID & 0x1f);

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 22, phyData), ret);
    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0x0000), ret);

    return ret;
}


/* Function Name:
 *      phy_8218e_gigaLiteEnable_get
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
 *      1. The RTL8218e is supported the per-port Giga Lite feature.
 */
int32
phy_8218e_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0001), ret);

    /* For Giga Lite (per-port) */
    RT_ERR_CHK(hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &phyData), ret);

    if (phyData & BIT_9)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0000), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_gigaLiteEnable_set
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
 *      1. The RTL8218e is supported the per-port Giga Lite feature.
 */
int32
phy_8218e_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    /* Configure the GPHY page to copper */
    RT_ERR_CHK(hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0001), ret);

    /*
     * bit[9]: configure retry speed down to 500M
     * bit[2]: configure 2-pair auto-downspeed
     */
    RT_ERR_CHK(hal_miim_read(unit, port, 0xa44, 17, &phyData), ret);
    if (ENABLED == enable)
        phyData |= (BIT_9 | BIT_2);
    else
        phyData &= ~(BIT_9| BIT_2);
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa44, 17, phyData), ret);

    /* For Giga Lite (per-port) */
    RT_ERR_CHK(hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &phyData), ret);
    if (ENABLED == enable)
        phyData |= BIT_9;
    else
        phyData &= ~(BIT_9);
    RT_ERR_CHK(hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 20, phyData), ret);

    /* Do the restart N-WAY */
    RT_ERR_CHK(hal_miim_read(unit, port, 0, 0, &phyData), ret);
    phyData |= BIT_9;
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 0, phyData), ret);

    /* Configure the GPHY page to auto */
    RT_ERR_CHK(hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 30, 0x0000), ret);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_downSpeedEnable_get
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
phy_8218e_downSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 1), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, 0xa44, 17, &phyData), ERR, ret);

    if (phyData & BIT_3)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

ERR:
    hal_miim_write(unit, port, 0, 30, 0);
    return ret;
}

/* Function Name:
 *      phy_8218e_downSpeedEnable_set
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
phy_8218e_downSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 1), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, 0xa44, 17, &phyData), ERR, ret);

    /* Enable auto-downspeed to 100M */
    if (ENABLED == enable)
        phyData |= BIT_3;
    else
        phyData &= ~(BIT_3);

    /* Disable DownSpeed 10M */
    phyData &= ~(BIT_5);

    RT_ERR_HDL(hal_miim_write(unit, port, 0xa44, 17, phyData), ERR, ret);

ERR:
    hal_miim_write(unit, port, 0, 30, 0);
    return ret;
}

/* Function Name:
 *      phy_8218e_downSpeedStatus_get
 * Description:
 *      Get down speed status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pDownSpeedStatus - pointer to status of down speed.
 *                         1: link is up due to down speed; 0: down speed is not performed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa4b, 16, &phyData), ret);

    if (phyData & BIT_12)
        *pDownSpeedStatus = 1;
    else
        *pDownSpeedStatus = 0;

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218e_serialLedModePageReg_get
 * Description:
 *      Get page and register number of a specific LED mode register
 * Input:
 *      unit    - unit id
 *      port    - base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218e_serialLedModePageReg_get(uint32 led_id, uint32 *pPage, uint32 *pReg)
{
    uint32 ledModeReg_00_05[] = {18, 19, 20, 21, 22, 23 };      /* LED 00~05 */
    uint32 ledModeReg[] = { 16, 17, 18, 19, 20, 21, 22, 23 };   /* LED 06~13, 14~21, 22~29, 30~35 */

    if (led_id >= PHY_8218E_MAX_LED)
        return RT_ERR_INPUT;

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

int32
_phy_8218e_ledMode_dump(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg = 0;
    uint32  blk = 0;
    uint32  data = 0;
    int32   led_id = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);
    for (led_id=0; led_id<36; led_id++)
    {
        osal_printf("LED%02u ", led_id);
        if ((ret = _phy_8218e_serialLedModePageReg_get(led_id, &page, &reg)) != RT_ERR_OK)
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

int32
_phy_8218e_ledModeData_set(rtk_phy_ledMode_t *pLedMode, uint32 *pData)
{
    uint32  value = 0;

    if (pLedMode->mdi == RTK_PHY_LED_MODE_MDI_DISABLE)
        value = 0xF;
    else if (pLedMode->mdi <= 7)
        value = pLedMode->mdi;
    else
        return RT_ERR_INPUT;

    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_MDI_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_1000M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_100M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_SPEED_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_10M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_SPEED_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_1000M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_1000M_ACT_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_100M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_100M_ACT_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_10M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_10M_ACT_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_DUPLEX) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_DUPLEX_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_COLLISION) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_COLLISION_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_TX_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_TX_ACT_MASK);

    value = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_RX_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, value, PHY_8218E_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET, PHY_8218E_SERIAL_LED_PER_LED_CTRL_RX_ACT_MASK);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_ledMode_set
 * Description:
 *      Set LED mode for PHY control LED
 * Input:
 *      unit        - unit ID
 *      port        - port ID
 *      pLedMode    - LED mode select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      None
 */
int32
phy_8218e_ledMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg = 0;
    uint32  blk = 0;
    uint32  data = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    if ((ret = _phy_8218e_serialLedModePageReg_get(pLedMode->led_id, &page, &reg)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u invalid led index %u ", unit, port, pLedMode->led_id);
        return ret;
    }

    if ((pLedMode->mdi != RTK_PHY_LED_MODE_MDI_DISABLE) && (pLedMode->mdi >= PHY_8218E_MAX_MDI))
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u invalid mdi %u ", unit, port, pLedMode->mdi);
        return RT_ERR_INPUT;
    }

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, page, reg, &data), ERR, ret);
    ret = _phy_8218e_ledModeData_set(pLedMode, &data);
    RT_ERR_HDL(hal_miim_write(unit, port, page, reg, data), ERR, ret);

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_ledCtrl_get
 * Description:
 *      Get configuration of LED for PHY control LED
 * Input:
 *      unit        - unit ID
 *      port        - port ID
 * Output:
 *      pLedCtrl    - LED control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      None
 */
int32
phy_8218e_ledCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;
    uint32  value = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, PHY_8218E_SERIAL_LED_CONFIG_PAGE, PHY_8218E_SERIAL_LED_CONFIG_REG, &data), ERR, ret);

    value = REG32_FIELD_GET(data, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK);
    switch (value)
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
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown blink_rate value %u ", unit, port, value);
            break;
    }

    value = REG32_FIELD_GET(data, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK);
    switch (value)
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
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown burst_cycle value %u ", unit, port, value);
            break;
    }

    value = REG32_FIELD_GET(data, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK);
    switch (value)
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
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown clock_cycle value %u ", unit, port, value);
            break;
    }

    value = REG32_FIELD_GET(data, PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK);
    switch (value)
    {
        case 0:
            pLedCtrl->active_low = RTK_PHY_LED_CTRL_ACTIVE_HIGH;
            break;
        case 1:
            pLedCtrl->active_low = RTK_PHY_LED_CTRL_ACTIVE_LOW;
            break;
        default:
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown active_low value %u ", unit, port, value);
            break;
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_ledCtrl_set
 * Description:
 *      Configure LED for PHY control LED
 * Input:
 *      unit        - unit ID
 *      port        - port ID
 *      pLedCtrl    - LED control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      None
 */
int32
phy_8218e_ledCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;
    uint32  val_blink_rate = 0;
    uint32  val_burst_cycle = 0;
    uint32  val_clock_cycle = 0;
    uint32  val_active_low = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    switch (pLedCtrl->blink_rate)
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

    switch (pLedCtrl->burst_cycle)
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

    switch (pLedCtrl->clock_cycle)
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

    switch (pLedCtrl->active_low)
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

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, PHY_8218E_SERIAL_LED_CONFIG_PAGE, PHY_8218E_SERIAL_LED_CONFIG_REG, &data), ERR, ret);
    data = REG32_FIELD_SET(data, val_blink_rate, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK);
    data = REG32_FIELD_SET(data, val_burst_cycle, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK);
    data = REG32_FIELD_SET(data, val_clock_cycle, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK);
    data = REG32_FIELD_SET(data, val_active_low, PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET, PHY_8218E_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK);
    RT_ERR_HDL(hal_miim_write(unit, port, PHY_8218E_SERIAL_LED_CONFIG_PAGE, PHY_8218E_SERIAL_LED_CONFIG_REG, data), ERR, ret);

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      phy_8218e_mdiLoopbackEnable_get
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
phy_8218e_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  data = 0;

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &data), ret);
    if (PHY_8218E_MDI_LB_MDI_VAL_GET(data) == 0x3)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_mdiLoopbackEnable_set
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
phy_8218e_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  data = 0;
    uint32  value = 0;

    if (ENABLED == enable)
        value = 0x3;
    else
        value = rtl8218e_info[unit]->mdi_val_org[port];

    RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &data), ret);
    data = PHY_8218E_MDI_LB_MDI_VAL_SET(data, value);
    RT_ERR_CHK(hal_miim_write(unit, port, 0xa43, 24, data), ret);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_macIntfSerdesLinkStatus_get
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
phy_8218e_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    int32           ret = RT_ERR_OK;
    uint32          blk = 0;
    uint32          data = 0;
    uint32          data1 = 0;
    rt_serdesMode_t sdsMode = RTK_MII_NONE;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, 0x260, 18, &data), ERR, ret);
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
            goto ERR;
    }

    data = 0;
    RT_ERR_HDL(hal_miim_read(unit, port, 0x407, 0x15, &data), ERR, ret);
    data1 = 0;
    RT_ERR_HDL(hal_miim_read(unit, port, 0x507, 0x15, &data1), ERR, ret);
    if (RTK_MII_QSGMII == sdsMode)
    {
        pStatus->sds_num = 2;
        /* return link up when all channels are all up */
        pStatus->link_status[0] = ((data & 0xF0) == 0xF0) ? PORT_LINKUP : PORT_LINKDOWN;
        pStatus->link_status[1] = ((data1 & 0xF0) == 0xF0) ? PORT_LINKUP : PORT_LINKDOWN;
    }
    else
    {
        pStatus->sds_num = 1;
        /* return link up when all channels are all up */
        if (((data & 0xF0) == 0xF0) && ((data1 & 0xF0) == 0xF0))
            pStatus->link_status[0] = PORT_LINKUP;
        else
            pStatus->link_status[0] = PORT_LINKDOWN;
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


static int32
_phy_8218e_eye_dbg_out(uint32 unit, uint32 port)
{
    return RT_ERR_OK;
}

static int32
_phy_8218e_eye_scan_en(uint32 unit, uint32 port)
{
    int32   ret = RT_ERR_OK;
    uint32  EYE_SCAN_EN_PAGE = 0x485;
    uint32  EYE_SCAN_EN_REG = 0x11;
    uint32  EYE_SCAN_EN_BIT = 0;
    uint32  EYE_CK_SEL_PAGE = 0x4B9;
    uint32  EYE_CK_SEL_REG = 0x15;
    uint32  EYE_CK_SEL_BITHigh = 10;
    uint32  EYE_CK_SEL_BITLow = 9;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 1), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 0), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 1), ret);

    RT_ERR_CHK(phy_field_write(unit, port, EYE_CK_SEL_PAGE, EYE_CK_SEL_REG,
            EYE_CK_SEL_BITHigh, EYE_CK_SEL_BITLow, 0), ret);

    return RT_ERR_OK;
}

static int32
_phy_8218e_eye_pi_set(uint32 unit, uint32 port, uint32 xAxis)
{
    int32   ret = RT_ERR_OK;
    uint32  EYE_PI_PHASE_PAGE = 0x485;
    uint32  EYE_PI_PHASE_REG = 0x12;
    uint32  EYE_PI_PHASE_BITHigh = 15;
    uint32  EYE_PI_PHASE_BITLow = 10;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_PI_PHASE_PAGE, EYE_PI_PHASE_REG,
            EYE_PI_PHASE_BITHigh, EYE_PI_PHASE_BITLow, xAxis), ret);

    return RT_ERR_OK;
}

static int32
_phy_8218e_eye_ref_set(uint32 unit, uint32 port, uint32 yAxis)
{
    int32   ret = RT_ERR_OK;
    uint32  EYE_REF_CTRL_PAGE = 0x485;
    uint32  EYE_REF_CTRL_REG = 0x12;
    uint32  EYE_REF_CTRL_BITHigh = 5;
    uint32  EYE_REF_CTRL_BITLow = 0;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_REF_CTRL_PAGE, EYE_REF_CTRL_REG,
            EYE_REF_CTRL_BITHigh, EYE_REF_CTRL_BITLow, yAxis), ret);

    return RT_ERR_OK;
}

static int32
_phy_8218e_eye_mdioClk(uint32 unit, uint32 port)
{
    int32   ret = RT_ERR_OK;
    uint32  EYE_MDIOCLK_PAGE = 0x485;
    uint32  EYE_MDIOCLK_REG = 0x11;
    uint32  EYE_MDIOCLK_BIT = 1;

    RT_ERR_CHK(phy_field_write(unit, port, EYE_MDIOCLK_PAGE, EYE_MDIOCLK_REG,
            EYE_MDIOCLK_BIT, EYE_MDIOCLK_BIT, 1), ret);
    RT_ERR_CHK(phy_field_write(unit, port, EYE_MDIOCLK_PAGE, EYE_MDIOCLK_REG,
            EYE_MDIOCLK_BIT, EYE_MDIOCLK_BIT, 0), ret);

    return RT_ERR_OK;
}

static int32
_phy_8218e_eyePointRead(uint32 unit, uint32 port, uint32 *value)
{
    int32   ret = RT_ERR_OK;
    uint32  readPage = 0x47D;
    uint32  readReg = 0x15;
    int32   retry = 100;

    RT_ERR_CHK(_phy_8218e_eye_mdioClk(unit, port), ret);
    RT_ERR_CHK(hal_miim_read(unit, port, readPage, readReg, value), ret);

    do {
        RT_ERR_CHK(_phy_8218e_eye_mdioClk(unit, port), ret);
        RT_ERR_CHK(hal_miim_read(unit, port, readPage, readReg, value), ret);
        retry--;
        if (retry <= 0)
        {
            ret = RT_ERR_BUSYWAIT_TIMEOUT;
            RT_ERR(ret, (MOD_HAL|MOD_SDS), "unit %u macId %u readOut retry timeout.", unit, port);
            break;
        }
    } while (((*value >> 14) & 0x3) != 0x3);

    return ret;
}

/* Function Name:
 *      _phy_8218e_eyeMon_proc
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
_phy_8218e_eyeMon_proc(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, phy_sds_eyeMon_hdlr_t pHdlr, void *pDb)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  frame = 0;
    uint32  value = 0;
    int32   xAxis = 0;
    int32   yAxis = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);

    /* enable */
    RT_ERR_HDL(_phy_8218e_eye_dbg_out(unit, port), ERR, ret);
    RT_ERR_HDL(_phy_8218e_eye_scan_en(unit, port), ERR, ret);

    /* set x axis */
    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
    {
        RT_ERR_HDL(_phy_8218e_eye_pi_set(unit, port, xAxis), ERR, ret);

        /* set y axis */
        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; ++yAxis)
        {
            RT_ERR_HDL(_phy_8218e_eye_ref_set(unit, port, yAxis), ERR, ret);

            /* get value */
            for (frame = 0; frame < frameNum; ++frame)
            {
                if ((ret = _phy_8218e_eyePointRead(unit, port, &value)) != RT_ERR_OK)
                {
                    goto ERR;
                }
                pHdlr(xAxis, yAxis, frame, pDb, value);
            }
        }
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_eyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      sdsId       - serdes ID
 *      frameNum    - frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_eyeMonitor_start(uint32 unit, uint32 port, uint32 sdsId, uint32 frameNum)
{
    int32   ret = RT_ERR_OK;
    uint32  xAxis = 0;
    uint32  yAxis = 0;
    uint32  frame = 0;
    uint32  data_size = 0;
    uint32  *eyeData = NULL;

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SDS), "unit=%d,port=%d,sdsId=%d,frameNum=%d", unit, port, sdsId, frameNum);

    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    if ((ret = _phy_8218e_eyeMon_proc(unit, port, sdsId, frameNum, phy_common_eyeMonPixel_get, (void *)eyeData)) != RT_ERR_OK)
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
 *      phy_8218e_eyeMonitorInfo_get
 * Description:
 *      Get eye monitor height and width
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      sdsId       - serdes ID
 *      frameNum    - frame number
 * Output:
 *      pInfo       - eye monitor information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218e_eyeMonitorInfo_get(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    int32               ret = RT_ERR_OK;
    rtk_eye_monBmap_t   *eye = NULL;
    int32               xAxis, yAxis;
    uint32              data_size, i;
    uint8               maxHeight, height;
    uint8               maxWidth, width;
    uint8               width_sample_pos[] = {(RTK_EYE_MON_Y_MAX - RTK_EYE_MON_YAXIS_CHK_OFST), (RTK_EYE_MON_Y_MAX + RTK_EYE_MON_YAXIS_CHK_OFST)};

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "unit=%u,port=%u,sds=%u,frameNum=%u", unit, port, sds, frameNum);

    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);


    data_size = sizeof(rtk_eye_monBmap_t) * RTK_EYE_MON_X_MAX;
    if ((eye = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_MEM_ALLOC;
    }
    osal_memset(eye, 0, data_size);

    if ((ret = _phy_8218e_eyeMon_proc(unit, port, sds, frameNum, phy_common_eyeMonInfo_update, (void *)eye)) != RT_ERR_OK)
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

    maxHeight = maxHeight * 3;
    if (maxHeight >= RTK_EYE_MON_ARXIS_Y_MAX)
        pInfo->height = RTK_EYE_MON_ARXIS_Y_MAX;
    else
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
 *      phy_8218e_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 * Output:
 *      pEyeParam   - eye parameter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
int32
phy_8218e_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32           ret = RT_ERR_OK;
    uint32          blk = 0;
    uint32          data = 0;
    uint32          page = 0;
    rt_serdesMode_t sdsMode = RTK_MII_NONE;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    if ((ret = phy_8218e_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
        return ret;

    if (RTK_MII_XSGMII == sdsMode)
    {
        if (sdsId != 0)
            return RT_ERR_INPUT;
    }
    else
    {
        if ((sdsId != 0) && (sdsId != 1))
            return RT_ERR_INPUT;
    }

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    osal_memset(pEyeParam, 0, sizeof(rtk_sds_eyeParam_t));
    if (RTK_MII_XSGMII == sdsMode)
    {
        /*
         * Pre EN   (S0:Page 0x4b8 reg 0x17 bit[2])
         * Pre AMP  (S0:Page 0x4b8 reg 0x17 bit[8:3])
         * Main AMP (S0:Page 0x4b8 reg 0x17 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4b8, 0x17, &data), ERR, ret);
        pEyeParam->pre_en = REG32_FIELD_GET(data, 2, (0x1 << 2));
        pEyeParam->pre_amp = REG32_FIELD_GET(data, 3, (0x3F << 3));
        pEyeParam->main_amp = REG32_FIELD_GET(data, 10, (0x3F << 10));

        /*
         * Post EN  (S0:Page 0x4b8 reg 0x16 bit[3])
         * Post AMP (S0:Page 0x4b8 reg 0x16 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4b8, 0x16, &data), ERR, ret);
        pEyeParam->post_en = REG32_FIELD_GET(data, 3, (0x1 << 3));
        pEyeParam->post_amp = REG32_FIELD_GET(data, 10, (0x3F << 10));

        /* tx impedience (S0:Page 0x4b9 reg 0x13 bit[10:7]) */
        hal_miim_read(unit, port, 0x4b9, 0x13, &data);
        pEyeParam->impedance = REG32_FIELD_GET(data, 7, (0xF << 7));
    }
    else if (RTK_MII_QSGMII == sdsMode)
    {
        page = (sdsId == 0) ? (0x4a8) : (0x5a8);

        /*
         * Pre EN   (S0:Page 0x4a8 reg 0x17 bit[2];     S1:Page 0x5a8 reg 0x17 bit[2])
         * Pre AMP  (S0:Page 0x4a8 reg 0x17 bit[8:3];   S1:Page 0x5a8 reg 0x17 bit[8:3])
         * Main AMP (S0:Page 0x4a8 reg 0x17 bit[15:10]; S1:Page 0x5a8 reg 0x17 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x17, &data), ERR, ret);
        pEyeParam->pre_en = REG32_FIELD_GET(data, 2, (0x1 << 2));
        pEyeParam->pre_amp = REG32_FIELD_GET(data, 3, (0x3F << 3));
        pEyeParam->main_amp = REG32_FIELD_GET(data, 10, (0x3F << 10));

        /*
         * Post EN  (S0:Page 0x4a8 reg 0x16 bit[3];     S1:Page 0x5a8 reg 0x16 bit[3])
         * Post AMP (S0:Page 0x4a8 reg 0x16 bit[15:10]; S1:Page 0x5a8 reg 0x16 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x16, &data), ERR, ret);
        pEyeParam->post_en = REG32_FIELD_GET(data, 3, (0x1 << 3));
        pEyeParam->post_amp = REG32_FIELD_GET(data, 10, (0x3F << 10));

        page = (sdsId == 0) ? (0x4b9) : (0x5a9);

        /* tx impedience (S0:Page 0x4b9 reg 0x13 bit[10:7]; S1:Page 0x5a9 reg 0x13 bit[10:7]) */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x13, &data), ERR, ret);
        pEyeParam->impedance = REG32_FIELD_GET(data, 7, (0xF << 7));
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit        - unit ID
 *      port        - Base mac ID of the PHY
 *      sdsId       - SerDes ID of the PHY
 *      pEyeParam   - eye parameter.
 *                    impedance is not supported for configure.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
int32
phy_8218e_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32           ret = RT_ERR_OK;
    uint32          blk = 0;
    uint32          data = 0;
    uint32          page = 0;
    rt_serdesMode_t sdsMode = RTK_MII_NONE;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    if ((ret = phy_8218e_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
        return ret;

    if (RTK_MII_XSGMII == sdsMode)
    {
        if (sdsId != 0)
            return RT_ERR_INPUT;
    }
    else
    {
        if ((sdsId != 0) && (sdsId != 1))
            return RT_ERR_INPUT;
    }

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    if (RTK_MII_XSGMII == sdsMode)
    {
        /*
         * Pre EN   (S0:Page 0x4b8 reg 0x17 bit[2])
         * Pre AMP  (S0:Page 0x4b8 reg 0x17 bit[8:3])
         * Main AMP (S0:Page 0x4b8 reg 0x17 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4b8, 0x17, &data), ERR, ret);
        data = REG32_FIELD_SET(data, pEyeParam->pre_en, 2, (0x1 << 2));
        data = REG32_FIELD_SET(data, pEyeParam->pre_amp, 3, (0x3F << 3));
        data = REG32_FIELD_SET(data, pEyeParam->main_amp, 10, (0x3F << 10));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x4b8, 0x17, data), ERR, ret);

        /*
         * Post EN  (S0:Page 0x4b8 reg 0x16 bit[3])
         * Post AMP (S0:Page 0x4b8 reg 0x16 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4b8, 0x16, &data), ERR, ret);
        data = REG32_FIELD_SET(data, pEyeParam->post_en, 3, (0x1 << 3));
        data = REG32_FIELD_SET(data, pEyeParam->post_amp, 10, (0x3F << 10));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x4b8, 0x16, data), ERR, ret);
    }
    else if (RTK_MII_QSGMII == sdsMode)
    {
        page = (sdsId == 0) ? (0x4a8) : (0x5a8);

        /*
         * Pre EN   (S0:Page 0x4a8 reg 0x17 bit[2];     S1:Page 0x5a8 reg 0x17 bit[2])
         * Pre AMP  (S0:Page 0x4a8 reg 0x17 bit[8:3];   S1:Page 0x5a8 reg 0x17 bit[8:3])
         * Main AMP (S0:Page 0x4a8 reg 0x17 bit[15:10]; S1:Page 0x5a8 reg 0x17 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x17, &data), ERR, ret);
        data = REG32_FIELD_SET(data, pEyeParam->pre_en, 2, (0x1 << 2));
        data = REG32_FIELD_SET(data, pEyeParam->pre_amp, 3, (0x3F << 3));
        data = REG32_FIELD_SET(data, pEyeParam->main_amp, 10, (0x3F << 10));
        RT_ERR_HDL(hal_miim_write(unit, port, page, 0x17, data), ERR, ret);

        /*
         * Post EN  (S0:Page 0x4a8 reg 0x16 bit[3];     S1:Page 0x5a8 reg 0x16 bit[3])
         * Post AMP (S0:Page 0x4a8 reg 0x16 bit[15:10]; S1:Page 0x5a8 reg 0x16 bit[15:10])
         */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x16, &data), ERR, ret);
        data = REG32_FIELD_SET(data, pEyeParam->post_en, 3, (0x1 << 3));
        data = REG32_FIELD_SET(data, pEyeParam->post_amp, 10, (0x3F << 10));
        RT_ERR_HDL(hal_miim_write(unit, port, page, 0x16, data), ERR, ret);
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      phy_8218e_sdsTestMode_set
 * Description:
 *      Set SerDes test mode.
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
phy_8218e_sdsTestMode_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_testMode_t testMode)
{
    int32           ret = RT_ERR_OK;
    uint32          blk = 0;
    rt_serdesMode_t sdsMode = RTK_MII_NONE;
    uint32          page = 0;
    uint32          data = 0;
    uint32          prbsSel = 0;
    rtk_enable_t    enable = DISABLED;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    if ((ret = phy_8218e_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
        return ret;

    if (RTK_MII_XSGMII == sdsMode)
    {
        if (sdsId != 0)
            return RT_ERR_INPUT;
    }
    else
    {
        if ((sdsId != 0) && (sdsId != 1))
            return RT_ERR_INPUT;
    }

    /* Determine PRBS select value and enable/disable. */
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

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    if (RTK_MII_XSGMII == sdsMode)
    {
        /*
         * SDS_EXT_REG11[9:8]: SEP_CFG_PRBS_SEL_1_0=0x0
         * 0:prbs7/1:prbs9/2:prbs11/3:prbs15
         */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x405, 0x13, &data), ERR, ret);
        data = REG32_FIELD_SET(data, prbsSel, 8, (0x3 << 8));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x405, 0x13, data), ERR, ret);

        /* SDS_EXT_REG12[11]: SEP_CFG_PRBS_ON=0x1 */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x405, 0x14, &data), ERR, ret);
        data = REG32_FIELD_SET(data, enable, 11, (0x1 << 11));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x405, 0x14, data), ERR, ret);
    }
    else if (RTK_MII_QSGMII == sdsMode)
    {
        page = (sdsId == 0) ? 0x404 : 0x504;

        /*
         * SDS_EXT_REG11[9:8]: SEP_CFG_PRBS_SEL_1_0=0x0
         * 0:prbs7/1:prbs9/2:prbs11/3:prbs15
         */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x13, &data), ERR, ret);
        data = REG32_FIELD_SET(data, prbsSel, 8, (0x3 << 8));
        RT_ERR_HDL(hal_miim_write(unit, port, page, 0x13, data), ERR, ret);

        /* SDS_EXT_REG12[11]: SEP_CFG_PRBS_ON=0x1 */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x14, &data), ERR, ret);
        data = REG32_FIELD_SET(data, enable, 11, (0x1 << 11));
        RT_ERR_HDL(hal_miim_write(unit, port, page, 0x14, data), ERR, ret);
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_sdsTestModeCnt_get
 * Description:
 *      Get SerDes test mode test pattern error counter.
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
phy_8218e_sdsTestModeCnt_get(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 *pCnt)
{
    int32           ret = RT_ERR_OK;
    uint32          blk = 0;
    rt_serdesMode_t sdsMode = RTK_MII_NONE;
    uint32          page = 0;
    uint32          data = 0;
    uint32          cnt = 0;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    if ((ret = phy_8218e_macIntfSerdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
        return ret;

    if (RTK_MII_XSGMII == sdsMode)
    {
        if (sdsId != 0)
            return RT_ERR_INPUT;
    }
    else
    {
        if ((sdsId != 0) && (sdsId != 1))
            return RT_ERR_INPUT;
    }

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    if (RTK_MII_XSGMII == sdsMode)
    {
        /*
         * SDS_EXT_PAGE reg05[15:00]: prbs_err_cnt[31:16]
         * SDS_EXT_PAGE reg04[15:00]: prbs_err_cnt[15:00]
         */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x404, 0x15, &data), ERR, ret);
        cnt = REG32_FIELD_SET(cnt, data, 16, (0xFFFF << 16));
        RT_ERR_HDL(hal_miim_read(unit, port, 0x404, 0x14, &data), ERR, ret);
        cnt = REG32_FIELD_SET(cnt, data, 0, (0xFFFF << 0));

        *pCnt = cnt;

        /* wr 16'b0 to clear */
        RT_ERR_HDL(hal_miim_write(unit, port, 0x404, 0x15, 0x0), ERR, ret);
        RT_ERR_HDL(hal_miim_write(unit, port, 0x404, 0x14, 0x0), ERR, ret);
    }
    else if (RTK_MII_QSGMII == sdsMode)
    {
        page = (sdsId == 0) ? 0x404 : 0x504;

        /*
         * SDS_EXT_PAGE reg05[15:00]: prbs_err_cnt[31:16]
         * SDS_EXT_PAGE reg04[15:00]: prbs_err_cnt[15:00]
         */
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x15, &data), ERR, ret);
        cnt = REG32_FIELD_SET(cnt, data, 16, (0xFFFF << 16));
        RT_ERR_HDL(hal_miim_read(unit, port, page, 0x14, &data), ERR, ret);
        cnt = REG32_FIELD_SET(cnt, data, 0, (0xFFFF << 0));

        *pCnt = cnt;

        /* wr 16'b0 to clear */
        RT_ERR_HDL(hal_miim_write(unit, port, page, 0x15, 0x0), ERR, ret);
        RT_ERR_HDL(hal_miim_write(unit, port, page, 0x14, 0x0), ERR, ret);
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      phy_8218e_sdsLeq_get
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit        - unit id
 *      port        - base port id of the PHY
 *      sdsId       - SerDes of the PHY
 * Output:
 *      pManual_en  - pointer to manual LEQ config satus
 *      pLeq_val    - pointer to current LEQ value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8218e_sdsLeq_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t *pManual_en, uint32 *pLeq_val)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;

    if (sdsId != 0)
        return RT_ERR_INPUT;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    /* page 0x4bc reg 0x10 [7] = 0 */
    RT_ERR_HDL(hal_miim_read(unit, port, 0x4bc, 0x10, &data), ERR, ret);
    if (REG32_FIELD_GET(data, 7, (0x1 << 7)) == 1)
        *pManual_en = ENABLED;
    else
        *pManual_en = DISABLED;

    /* page 0x4bc reg 0x10 [6-2] */
    RT_ERR_HDL(hal_miim_read(unit, port, 0x4bc, 0x10, &data), ERR, ret);
    *pLeq_val = REG32_FIELD_GET(data, 2, (0x1F << 2));

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_sdsLeq_set
 * Description:
 *      Set the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit        - unit id
 *      port        - base port id of the PHY
 *      sdsId       - SerDes of the PHY
 *      manual_en   - ENABLED: LEQ in manual-mode; DISABLED: LEQ is auto-mode.
 *      leq_val     - Fixed LEQ value when manual_en is set to ENABLED;
 *                    this field is not used in driver when manual_en set to DISABLED, just keep it set to 0.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */

int32
phy_8218e_sdsLeq_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t manual_en, uint32 leq_val)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;

    if (sdsId != 0)
        return RT_ERR_INPUT;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    if (manual_en == ENABLED)
    {
        /* page 0x4bc reg 0x10 [7] = 0 */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4bc, 0x10, &data), ERR, ret);
        data = REG32_FIELD_SET(data, 0, 7, (0x1 << 7));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x4bc, 0x10, data), ERR, ret);

        /* page 0x4bc reg 0x10 [6-2] = leq_val */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4bc, 0x10, &data), ERR, ret);
        data = REG32_FIELD_SET(data, leq_val, 2, (0x1F << 2));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x4bc, 0x10, data), ERR, ret);

        /* page 0x4bc reg 0x10 [7] = 1 */
        data = REG32_FIELD_SET(data, 1, 7, (0x1 << 7));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x4bc, 0x10, data), ERR, ret);
    }
    else
    {
        /* page 0x4bc reg 0x10 [7] = 0 */
        RT_ERR_HDL(hal_miim_read(unit, port, 0x4bc, 0x10, &data), ERR, ret);
        data = REG32_FIELD_SET(data, 0, 7, (0x1 << 7));
        RT_ERR_HDL(hal_miim_write(unit, port, 0x4bc, 0x10, data), ERR, ret);
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


/*
 * PHY Interrupts.
 */
int32
_phy_8218e_intrNwayNextPageRecvEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  data = 0;

    /* Get per PHY port Nway Next Page Recv enable (page 0xa42 reg 0x12 bit 2). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & BIT_2)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrNwayNextPageRecvEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  data = 0;
    uint32  value = 0;

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
_phy_8218e_intrANCompleteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  data = 0;

    /* Get per PHY port AN Complete enable (page 0xa42 reg 0x12 bit 3). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & BIT_3)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrANCompleteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  data = 0;
    uint32  value = 0;

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
_phy_8218e_intrLinkChangeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  data = 0;

    /* Get per PHY port Link Change enable (page 0xa42 reg 0x12 bit 4). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & BIT_4)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrLinkChangeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  data = 0;
    uint32  value = 0;

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
_phy_8218e_intrAldpsStateChangeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  data = 0;

    /* Get per PHY port ALDPS State Change enable (page 0xa42 reg 0x12 bit 9). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & BIT_9)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrAldpsStateChangeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  data = 0;
    uint32  value = 0;

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
_phy_8218e_intrCommonStatus_get(uint32 unit, uint32 basePort, rtk_phy_intrStatusVal_t *pStatus)
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
_phy_8218e_intrIntpinCommonMask_get(uint32 unit, uint32 basePort, uint32 phyPort, uint32 *pMask)
{
    uint32  data = 0;

    /* Read the chip interrupt PIN IMR (page 0x320 reg 0x11). */
    hal_miim_read(unit, basePort, 0x320, 0x11, &data);
    data = REG32_FIELD_GET(data, phyPort, (0x1 << phyPort));
    *pMask = data;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrIntpinCommonMask_set(uint32 unit, uint32 basePort, uint32 phyPort, uint32 mask)
{
    uint32  data = 0;

    if ((mask != 0) && (mask != 1))
        return RT_ERR_INPUT;

    /* Set the chip interrupt PIN IMR (page 0x320 reg 0x11). */
    hal_miim_read(unit, basePort, 0x320, 0x11, &data);
    data = REG32_FIELD_SET(data, mask, phyPort, (0x1 << phyPort));
    hal_miim_write(unit, basePort, 0x320, 0x11, data);

    return RT_ERR_OK;
}

int32
_phy_8218e_intrRlfd_init(uint32 unit, rtk_port_t basePort)
{
    uint32  data = 0;

    /* Enable en_rlfd. */
    hal_miim_read(unit, basePort, 0x264, 0x16, &data);
    data = REG32_FIELD_SET(data, 0x3, 0, (0x3 << 0));
    hal_miim_write(unit, basePort, 0x264, 0x16, data);

    return RT_ERR_OK;
}

int32
_phy_8218e_intrRlfdEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  data = 0;

    /* Get per PHY port RLFD enable (page 0xa44 reg 0x11 bit 15). */
    hal_miim_read(unit, port, 0xa44, 0x11, &data);

    if (data & BIT_15)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrRlfdEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  data = 0;
    uint32  value = 0;

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
_phy_8218e_intrRlfdStatus_get(uint32 unit, uint32 basePort, rtk_phy_intrStatusVal_t *pStatus)
{
    uint32  blk = 0;
    uint32  data = 0;

    pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_PORT_BITMAP;
    pStatus->statusValue = 0;

    /* Get the basePort (this PHY's phy port 0's MAC ID). */
    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
        return RT_ERR_PORT_ID;

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
_phy_8218e_intrIntpinRlfdMask_get(uint32 unit, uint32 basePort, uint32 phyPort, uint32 *pMask)
{
    uint32  data = 0;

    /* Read the chip interrupt PIN IMR. */
    hal_miim_read(unit, basePort, 0x320, 0x14, &data);
    data = REG32_FIELD_GET(data, phyPort, (0x1 << phyPort));
    *pMask = data;

    return RT_ERR_OK;
}

int32
_phy_8218e_intrIntpinRlfdMask_set(uint32 unit, uint32 basePort, uint32 phyPort, uint32 mask)
{
    uint32  data = 0;

    if ((mask != 0) && (mask != 1))
        return RT_ERR_INPUT;

    /* Disable the chip interrupt SerDes IMR. */
    hal_miim_write(unit, basePort, 0x321, 0x12, 0x00);

    /* Enable the chip interrupt PIN IMR. */
    hal_miim_read(unit, basePort, 0x320, 0x14, &data);
    data = REG32_FIELD_SET(data, mask, phyPort, (0x1 << phyPort));
    hal_miim_write(unit, basePort, 0x320, 0x14, data);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_intr_init
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
phy_8218e_intr_init(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;

    /* Get the basePort (this PHY's phy port 0's MAC ID). */
    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

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
            ret = _phy_8218e_intrRlfd_init(unit, port);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);

    return ret;
}

/* Function Name:
 *      phy_8218e_intrEnable_get
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
phy_8218e_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 0);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
            ret = _phy_8218e_intrNwayNextPageRecvEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
            ret = _phy_8218e_intrANCompleteEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
            ret = _phy_8218e_intrLinkChangeEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
            ret = _phy_8218e_intrAldpsStateChangeEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            ret = _phy_8218e_intrRlfdEnable_get(unit, port, pEnable);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_intrEnable_set
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
phy_8218e_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 0);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
            ret = _phy_8218e_intrNwayNextPageRecvEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
            ret = _phy_8218e_intrANCompleteEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
            ret = _phy_8218e_intrLinkChangeEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
            ret = _phy_8218e_intrAldpsStateChangeEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            ret = _phy_8218e_intrRlfdEnable_set(unit, port, enable);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_intrStatus_get
 * Description:
 *      Get specified PHY interrupt status.
 * Input:
 *      unit    - unit id
 *      port    - port id
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
phy_8218e_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus)
{
    int32   ret = RT_ERR_OK;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8218e_intrCommonStatus_get(unit, port, pStatus);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218e_intrRlfdStatus_get(unit, port, pStatus);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
}

/* Function Name:
 *      phy_8218e_intrMask_get
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
phy_8218e_intrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask)
{
    int32   ret = RT_ERR_OK;
    uint32  basePort = 0;
    uint32  phyPort = 0;
    uint32  blk = 0;

    /* Get the corresponding PHY chip indexed by basePort (this PHY's phy port 0's MAC ID). */
    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218E)
        return RT_ERR_PORT_ID;

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8218e_intrIntpinCommonMask_get(unit, basePort, phyPort, pMask);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218e_intrIntpinRlfdMask_get(unit, basePort, phyPort, pMask);
            break;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_intrMask_set
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
phy_8218e_intrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask)
{
    int32   ret = RT_ERR_OK;
    uint32  basePort = 0;
    uint32  phyPort = 0;
    uint32  blk = 0;

    /* Get the corresponding PHY chip indexed by basePort (this PHY's phy port 0's MAC ID). */
    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8218E)
        return RT_ERR_PORT_ID;

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8218e_intrIntpinCommonMask_set(unit, basePort, phyPort, mask);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = _phy_8218e_intrIntpinRlfdMask_set(unit, basePort, phyPort, mask);
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      _phy_8218e_ptpReg_access_start
 * Description:
 *      Prepare to do PTP register access for the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
static int32
_phy_8218e_ptpReg_access_start(uint32 unit, rtk_port_t port)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 8), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218e_ptpReg_access_end
 * Description:
 *      Finish PTP register access for the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
static int32
_phy_8218e_ptpReg_access_end(uint32 unit, rtk_port_t port)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_write(unit, basePort, 0, 30, 0), ret);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218e_ptpReg_get
 * Description:
 *      Get PTP register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - page
 *      reg     - register
 * Output:
 *      pData   - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_phy_8218e_ptpReg_get(uint32 unit, rtk_port_t port, uint32 page, uint32 reg, uint32 *pData)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_TIME), "unit=%d, port=%d, page=0x%03x, reg=0x%02x",
            unit, port, page, reg);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, basePort, page, reg, pData), ret);

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_TIME), "pData=0x%x", *pData);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218e_ptpReg_set
 * Description:
 *      Set PTP register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - page
 *      reg     - register
 *      reg_val - reg value
 *      reg_msk - reg value mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
_phy_8218e_ptpReg_set(uint32 unit, uint32 port, uint32 page, uint32 reg, uint32 reg_val, uint32 reg_msk)
{
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;
    uint32      ori_val = 0;

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_TIME), "unit=%d, port=%d, page=0x%03x, reg=0x%02x, reg_val=0x%x, reg_msk=0x%x",
            unit, port, page, reg, reg_val, reg_msk);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
        return RT_ERR_PORT_ID;

    if ((reg_msk & 0xFFFF) != 0xFFFF)
    {
        RT_ERR_CHK(hal_miim_read(unit, basePort, page, reg, &ori_val), ret);

        reg_val = (ori_val & (~reg_msk)) | (reg_val & reg_msk);
    }
    RT_ERR_CHK(hal_miim_write(unit, basePort, page, reg, reg_val), ret);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_ptpSwitchMacAddr_get
 * Description:
 *      Get the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pSwitchMacAddr  - point to the Switch MAC Address
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
phy_8218e_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    int32   ret = RT_ERR_OK;
    uint32  mac_h = 0;
    uint32  mac_m = 0;
    uint32  mac_l = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG02_REG, &mac_h)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG01_REG, &mac_m)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG00_REG, &mac_l)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
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
 *      phy_8218e_ptpSwitchMacAddr_set
 * Description:
 *      Set the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pSwitchMacAddr  - point to the Switch MAC Address
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
phy_8218e_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    int32   ret = RT_ERR_OK;
    uint32  mac_h = 0;
    uint32  mac_m = 0;
    uint32  mac_l = 0;

    mac_h = (pSwitchMacAddr->octet[0] << 8) | (pSwitchMacAddr->octet[1]);
    mac_m = (pSwitchMacAddr->octet[2] << 8) | (pSwitchMacAddr->octet[3]);
    mac_l = (pSwitchMacAddr->octet[4] << 8) | (pSwitchMacAddr->octet[5]);


    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG02_REG, mac_h, 0xFFFF)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG01_REG, mac_m, 0xFFFF)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG00_REG, mac_l, 0xFFFF)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpRefTime_get
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
phy_8218e_ptpRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret = RT_ERR_OK;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
    uint32  nsec_l = 0;
    uint32  nsec_h = 0;
    uint32  reg_val = 0;
    uint32  reg_msk = 0;

    /* execute reading command, [15] = 0x1 executing, [13:12] = 0x0 read */
    reg_val = REG_FIELD_8218E_PTP_TIME_CMD_READ;
    reg_msk = RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_val = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK;
    reg_msk = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK;

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    /* busy waiting */
    do
    {
        if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
            _phy_8218e_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK);

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG22_REG, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG23_REG, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG24_REG, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG25_REG, &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG26_REG, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    pTimeStamp->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pTimeStamp->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpRefTime_set
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
phy_8218e_ptpRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32   ret = RT_ERR_OK;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
    uint32  nsec_l = 0;
    uint32  nsec_h = 0;
    uint32  reg_val = 0;
    uint32  reg_msk = 0;

    /* adjust Timer of PHY */
    sec_l = (timeStamp.sec) & 0xFFFF;
    sec_m = ((timeStamp.sec) >> 16) & 0xFFFF;
    sec_h = ((timeStamp.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = timeStamp.nsec & 0xFFFF;
    nsec_h = timeStamp.nsec >> 16;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG18_REG, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG19_REG, sec_m, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG20_REG, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG16_REG, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG17_REG, nsec_h, 0x3FFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if(0 == exec)
	{
	    reg_val = REG_FIELD_8218E_PTP_TIME_CMD_WRITE;
	    reg_msk = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK | RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK;
	}
	else
	{
		reg_val = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK | REG_FIELD_8218E_PTP_TIME_CMD_WRITE;
	    reg_msk = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK | RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK;
	}

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    /* busy watting */
    do
    {
        if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
            _phy_8218e_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK);

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpRefTimeAdjust_set
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
phy_8218e_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32   ret = RT_ERR_OK;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
    uint32  nsec_l = 0;
    uint32  nsec_h = 0;
    uint32  reg_val = 0;
    uint32  reg_msk = 0;
    uint64  sec = 0;
    uint32  nsec = 0;

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


    sec_l = (sec) & 0xFFFF;
    sec_m = ((sec) >> 16) & 0xFFFF;
    sec_h = ((sec) >> 32) & 0xFFFF;

    /* convert nsec to 8nsec */
    nsec_l = (nsec) & 0xFFFF;
    nsec_h = ((nsec) >> 16);


    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG18_REG, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG19_REG, sec_m, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG20_REG, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG16_REG, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG17_REG, nsec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

	if(0 == exec)
	{
	    reg_val = REG_FIELD_8218E_PTP_TIME_CMD_ADJUST;
	    reg_msk = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK | RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK;
	}
	else
	{
		reg_val = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK | REG_FIELD_8218E_PTP_TIME_CMD_ADJUST;
	    reg_msk = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK | RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK;
	}

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    /* busy waiting */
    do
    {
        if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
            _phy_8218e_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK);

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpRefTimeFreqCfg_get
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
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
int32
phy_8218e_ptpRefTimeFreqCfg_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_1_PAGE, RTL8218E_PTP_TMR_RG13_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    *pFreqCfg = (reg_val & RTL8218E_PTP_CFG_PTP_TIME_FREQ1_REG_FIELD_MASK) << 16;

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_1_PAGE, RTL8218E_PTP_TMR_RG12_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    *pFreqCfg |= reg_val;

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_1_PAGE, RTL8218E_PTP_TMR_RG15_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    *pFreqCur = (reg_val & RTL8218E_PTP_CUR_PTP_TIME_FREQ1_REG_FIELD_MASK) << 16;

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_1_PAGE, RTL8218E_PTP_TMR_RG14_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    *pFreqCur |= reg_val;

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpRefTimeFreqCfg_set
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
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x10000000.
 *      If it is configured to 0x8000000, the tick frequency would be half of default.
 *      If it is configured to 0x20000000, the tick frequency would be one and half times of default.
 */
int32
phy_8218e_ptpRefTimeFreqCfg_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_1_PAGE, RTL8218E_PTP_TMR_RG12_REG, freq, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if (apply)
        reg_val = RTL8218E_PTP_APPLY_FREQ_REG_FIELD_MASK | ((freq>>16) & 0x1FFF);
    else
        reg_val = ((freq>>16) & 0x1FFF);

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_1_PAGE, RTL8218E_PTP_TMR_RG13_REG, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpEnable_get
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
phy_8218e_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg_val = 0;

    page = RTL8218E_PTP_8_PAGE + PAGE_8218E_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, page, RTL8218E_PTP_GPTP0_RG00_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    if (reg_val & RTL8218E_PTP_P0_CFG_ETH_EN_REG_FIELD_MASK)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpEnable_set
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
phy_8218e_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg_val = 0;

    page = RTL8218E_PTP_8_PAGE + PAGE_8218E_PTP_OFFSET_PORT(port);
    reg_val = (enable == ENABLED)? (RTL8218E_PTP_P7_CFG_TX_TIME_W_REG_FIELD_MASK | RTL8218E_PTP_P0_CFG_RX_TIME_W_REG_FIELD_MASK | RTL8218E_PTP_P0_CFG_UDP_EN_REG_FIELD_MASK | RTL8218E_PTP_P0_CFG_ETH_EN_REG_FIELD_MASK) : (0);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, page, RTL8218E_PTP_GPTP0_RG00_REG,
        reg_val, RTL8218E_PTP_P0_CFG_TX_TIME_W_REG_FIELD_MASK | RTL8218E_PTP_P0_CFG_RX_TIME_W_REG_FIELD_MASK | RTL8218E_PTP_P0_CFG_UDP_EN_REG_FIELD_MASK | RTL8218E_PTP_P0_CFG_ETH_EN_REG_FIELD_MASK)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpTxInterruptStatus_get
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
phy_8218e_ptpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_5_PAGE, RTL8218E_PTP_TMR_RG41_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if (reg_val & RTL8218E_PTP_RD_ISR_PTP_REG_FIELD_MASK)
        *pIntrSts = ENABLED;
    else
        *pIntrSts = DISABLED;

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpInterruptEnable_get
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
phy_8218e_ptpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_5_PAGE, RTL8218E_PTP_TMR_RG41_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    if (reg_val & RTL8218E_PTP_CFG_IMR_PTP_REG_FIELD_MASK)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpInterruptEnable_set
 * Description:
 *      Set TX timestamp FIFO non-empty interrupt enable status of specific port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
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
phy_8218e_ptpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    reg_val = (enable == ENABLED)? (RTL8218E_PTP_CFG_IMR_PTP_REG_FIELD_MASK) : (0);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_5_PAGE, RTL8218E_PTP_TMR_RG41_REG,
        reg_val, RTL8218E_PTP_CFG_IMR_PTP_REG_FIELD_MASK)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpTxTimestampFifo_get
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
phy_8218e_ptpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;
    uint32  reg_msk = 0;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
//    uint32  nsec_l = 0;
//    uint32  nsec_h = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG37_REG, &reg_val)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    pTimeEntry->valid = (reg_val & RTL8218E_PTP_RD_TX_TIMESTAMP_VALID_REG_FIELD_MASK)? 1:0;
    pTimeEntry->port = (reg_val & RTL8218E_PTP_RD_PORT_ID_REG_FIELD_MASK) >> 8;
    pTimeEntry->msg_type = (reg_val & RTL8218E_PTP_RD_MSG_TYPE_REG_FIELD_MASK) >> 6;
    pTimeEntry->seqId = (reg_val & RTL8218E_PTP_RD_SEQ_ID_H_REG_FIELD_MASK) << 10;

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG38_REG, &reg_val)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    pTimeEntry->seqId |= (reg_val & RTL8218E_PTP_RD_SEQ_ID_L_REG_FIELD_MASK) >> 6;
    pTimeEntry->txTime.sec = (reg_val & RTL8218E_PTP_RD_TX_TIMESTAMP_SEC_H_REG_FIELD_MASK) << 2;

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG39_REG, &reg_val)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    pTimeEntry->txTime.sec |= (reg_val & RTL8218E_PTP_RD_TX_TIMESTAMP_SEC_L_REG_FIELD_MASK) >> 14;
    pTimeEntry->txTime.nsec = (reg_val & RTL8218E_PTP_RD_TX_TIMESTAMP_NSEC_H_REG_FIELD_MASK) << 16;

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_5_PAGE, RTL8218E_PTP_TMR_RG40_REG, &reg_val)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    pTimeEntry->txTime.nsec |= (reg_val & RTL8218E_PTP_RD_TX_TIMESTAMP_NSEC_L_REG_FIELD_MASK);

    /* read current time to calculate full TX time */
    reg_val = REG_FIELD_8218E_PTP_TIME_CMD_READ;
    reg_msk = RTL8218E_PTP_PTP_TIME_CMD_REG_FIELD_MASK;

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_val = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK;
    reg_msk = RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK;

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    /* busy waiting */
    do
    {
        if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG21_REG, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
            _phy_8218e_ptpReg_access_end(unit, port);
            return ret;
        }
    } while (reg_val & RTL8218E_PTP_PTP_TIME_EXEC_REG_FIELD_MASK);


    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG24_REG, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG25_REG, &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG26_REG, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((sec_l & 0xFF) >=  pTimeEntry->txTime.sec) /* 8 bits sec is not overflow yet */
    {
        pTimeEntry->txTime.sec |= ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFF00);
    }
    else
    {
        pTimeEntry->txTime.sec |= ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFF00);
        pTimeEntry->txTime.sec -= 0x100;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpIgrTpid_get
 * Description:
 *      Get inner/outer TPID of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      type        -  vlan type
 *      tpid_idx    - TPID index
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
 * Note:
 *      None
 */
extern int32
phy_8218e_ptpIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    int32   ret = RT_ERR_OK;
    uint32  page_addr = 0;
    uint32  reg_addr = 0;
    uint32  reg_val = 0;

    if (INNER_VLAN == type)
    {
        switch (tpid_idx)
        {
            case 0:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG05_REG;
                break;
            case 1:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG07_REG;
                break;
            case 2:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG09_REG;
                break;
            default:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG11_REG;
                break;
        }
    }
    else
    {
        switch (tpid_idx)
        {
            case 0:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG04_REG;
                break;
            case 1:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG06_REG;
                break;
            case 2:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG08_REG;
                break;
            default:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG10_REG;
                break;
        }
    }

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, page_addr, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    *pTpid = reg_val;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpIgrTpid_set
 * Description:
 *      Set inner/outer TPID of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      type        - vlan type
 *      tpid_idx    - TPID index
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
 * Note:
 *      None
 */
extern int32
phy_8218e_ptpIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid)
{
    int32   ret = RT_ERR_OK;
    uint32  page_addr, reg_addr = 0;
    uint32  reg_val = 0;

    if (INNER_VLAN == type)
    {
        switch (tpid_idx)
        {
            case 0:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG05_REG;
                break;
            case 1:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG07_REG;
                break;
            case 2:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG09_REG;
                break;
            default:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG11_REG;
                break;
        }
    }
    else
    {
        switch (tpid_idx)
        {
            case 0:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG04_REG;
                break;
            case 1:
                page_addr = RTL8218E_PTP_0_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG06_REG;
                break;
            case 2:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG08_REG;
                break;
            default:
                page_addr = RTL8218E_PTP_1_PAGE;
                reg_addr = RTL8218E_PTP_TMR_RG10_REG;
                break;
        }
    }

    reg_val = tpid;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, page_addr, reg_addr, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpSwitchMacRange_get
 * Description:
 *      Get MAC address range of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pRange  - pointer to MAC address range
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
phy_8218e_ptpSwitchMacRange_get(uint32 unit, rtk_port_t port, uint32 *pRange)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG03_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    *pRange = (reg_val >> 6) & 0x3FF;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpSwitchMacRange_set
 * Description:
 *      Set MAC address range of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      range   - MAC address range
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
phy_8218e_ptpSwitchMacRange_set(uint32 unit, rtk_port_t port, uint32 range)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    reg_val = range << 6;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_0_PAGE, RTL8218E_PTP_TMR_RG03_REG, reg_val, RTL8218E_PTP_CFG_MAC_RNG_REG_FIELD_MASK)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpOper_get
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
phy_8218e_ptpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;
    uint32  oper = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG35_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    oper = (reg_val & RTL8218E_PTP_CFG_GPI_OP_REG_FIELD_MASK) >> 4;
    switch (oper)
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

    pOperCfg->rise_tri = (reg_val & RTL8218E_PTP_CFG_GPI_RISE_TRIG_REG_FIELD_MASK) >> 3;
    pOperCfg->fall_tri = (reg_val & RTL8218E_PTP_CFG_GPI_FALL_TRIG_REG_FIELD_MASK) >> 2;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpOper_set
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
phy_8218e_ptpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    reg_val = 0;
    switch (pOperCfg->oper)
    {
        case TIME_OPER_START:
            reg_val = 0 << 4;
            break;
        case TIME_OPER_LATCH:
            reg_val = 1 << 4;
            break;
        case TIME_OPER_STOP:
            reg_val = 2 << 4;
            break;
        case TIME_OPER_CMD_EXEC:
            reg_val = 3 << 4;
            break;
        case TIME_OPER_FREQ_APPLY:
            reg_val = 4 << 4;
            break;
        default:
            return RT_ERR_FAILED;
    }

    reg_val |= (pOperCfg->rise_tri == ENABLED) ? (RTL8218E_PTP_CFG_GPI_RISE_TRIG_REG_FIELD_MASK) : (0);
    reg_val |= (pOperCfg->fall_tri == ENABLED) ? (RTL8218E_PTP_CFG_GPI_FALL_TRIG_REG_FIELD_MASK) : (0);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG35_REG, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpLatchTime_get
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
phy_8218e_ptpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime)
{
    int32   ret = RT_ERR_OK;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
    uint32  nsec_l = 0;
    uint32  nsec_h = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG22_REG, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_2_PAGE, RTL8218E_PTP_TMR_RG23_REG, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG24_REG, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG25_REG, &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG26_REG, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    pLatchTime->sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pLatchTime->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptp1PPSOutput_get
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
phy_8218e_ptp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG36_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if (reg_val & RTL8218E_PTP_CFG_PPS_EN_REG_FIELD_MASK)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    *pPulseWidth = reg_val & RTL8218E_PTP_CFG_PPS_WIDTH_REG_FIELD_MASK;

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptp1PPSOutput_set
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
phy_8218e_ptp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if (pulseWidth > PHY_8218E_MAX_PPS_WIDTH)
        return RT_ERR_OUT_OF_RANGE;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_val = (enable << 6) | (pulseWidth);

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG36_REG, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpClockOutput_get
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
phy_8218e_ptpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
    uint32  nsec_l = 0;
    uint32  nsec_h = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG27_REG, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG28_REG, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG29_REG, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG30_REG, &sec_m)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG31_REG, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    pClkOutput->startTime.sec = ((uint64)sec_h << 32) | ((uint64)sec_m << 16) | ((uint64)sec_l & 0xFFFF);
    pClkOutput->startTime.nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG32_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if (reg_val & RTL8218E_PTP_CFG_PULSE_MODE_REG_FIELD_MASK)
        pClkOutput->mode = PTP_CLK_OUT_PULSE;
    else
        pClkOutput->mode = PTP_CLK_OUT_REPEAT;

    if (reg_val & RTL8218E_PTP_CFG_CLKOUT_EN_REG_FIELD_MASK)
        pClkOutput->enable = ENABLED;
    else
        pClkOutput->enable = DISABLED;

    if (reg_val & RTL8218E_PTP_RD_CLKOUT_RUN_REG_FIELD_MASK)
        pClkOutput->runing = TRUE;
    else
        pClkOutput->runing = FALSE;


    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG33_REG, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG34_REG, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    pClkOutput->halfPeriodNsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptp1PPSOutput_set
 * Description:
 *      Set 1 PPS output configuration of the specified port.
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
phy_8218e_ptpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    int32   ret = RT_ERR_OK;
    uint32  sec_l = 0;
    uint32  sec_m = 0;
    uint32  sec_h = 0;
    uint32  nsec_l = 0;
    uint32  nsec_h = 0;
    uint32  reg_val = 0;

    /* adjust Timer of PHY */
    sec_l = (pClkOutput->startTime.sec) & 0xFFFF;
    sec_m = ((pClkOutput->startTime.sec) >> 16) & 0xFFFF;
    sec_h = ((pClkOutput->startTime.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = pClkOutput->startTime.nsec & 0xFFFF;
    nsec_h = pClkOutput->startTime.nsec >> 16;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG29_REG, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG30_REG, sec_m, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG31_REG, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG27_REG, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_3_PAGE, RTL8218E_PTP_TMR_RG28_REG, nsec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }


    nsec_l = pClkOutput->halfPeriodNsec & 0xFFFF;
    nsec_h = pClkOutput->halfPeriodNsec >> 16;

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG33_REG, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }
    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG34_REG, nsec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

	reg_val = (pClkOutput->mode << 2);

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG32_REG, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

	reg_val = (pClkOutput->mode << 2) | (pClkOutput->enable << 1);

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_4_PAGE, RTL8218E_PTP_TMR_RG32_REG, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpOutputSigSel_get
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
phy_8218e_ptpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, RTL8218E_PTP_MS1_C_PAGE, RTL8218E_PTP_MS1_RG61_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if (reg_val & RTL8218E_PTP_CFG_PTP_CLK_OUT_SEL_REG_FIELD_MASK)
        *pOutSigSel = PTP_OUT_SIG_SEL_1PPS;
    else
        *pOutSigSel = PTP_OUT_SIG_SEL_CLOCK;

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptp1PPSOutput_set
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
phy_8218e_ptpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel)
{
    int32   ret = RT_ERR_OK;
    uint32  reg_val = 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if (PTP_OUT_SIG_SEL_CLOCK == outSigSel)
    {
        /* bit 0 value 0 means select clock output mode */
        reg_val = RTL8218E_PTP_CFG_PTP_CLK_OUT_E_REG_FIELD_MASK | 0;
    }
    else
    {
        /* bit 0 value 1 means select 1PPS output mode */
        reg_val = RTL8218E_PTP_CFG_PTP_CLK_OUT_E_REG_FIELD_MASK | 1;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_MS1_C_PAGE, RTL8218E_PTP_MS1_RG61_REG, reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpTransEnable_get
 * Description:
 *      Get enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to PTP transparent clock enable status
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
phy_8218e_ptpTransEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg_val = 0;

    page = RTL8218E_PTP_8_PAGE + PAGE_8218E_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, page, RTL8218E_PTP_GPTP0_RG00_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    if (reg_val & RTL8218E_PTP_P0_CFG_TRAN_REG_FIELD_MASK)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpTransEnable_set
 * Description:
 *      Set enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - PTP transparent clock enable status
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
phy_8218e_ptpTransEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg_val = 0;

    page = RTL8218E_PTP_8_PAGE + PAGE_8218E_PTP_OFFSET_PORT(port);
    reg_val = (enable == ENABLED)? RTL8218E_PTP_P0_CFG_TRAN_REG_FIELD_MASK : 0;

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_set(unit, port, page, RTL8218E_PTP_GPTP0_RG00_REG,
        reg_val, RTL8218E_PTP_P0_CFG_TRAN_REG_FIELD_MASK)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpLinkDelay_get
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
phy_8218e_ptpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg_val = 0;

    page = RTL8218E_PTP_8_PAGE + PAGE_8218E_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_get(unit, port, page, RTL8218E_PTP_GPTP0_RG00_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    *pLinkDelay = (reg_val & RTL8218E_PTP_P0_CFG_LINK_DELAY_L_REG_FIELD_MASK) >> 6;


    if ((ret = _phy_8218e_ptpReg_get(unit, port, page, RTL8218E_PTP_GPTP0_RG01_REG, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    *pLinkDelay |= reg_val << 10;

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218e_ptpLinkDelay_set
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
phy_8218e_ptpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay)
{
    int32   ret = RT_ERR_OK;
    uint32  page = 0;
    uint32  reg_val = 0;

    page = RTL8218E_PTP_8_PAGE + PAGE_8218E_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
    {
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    reg_val = (linkDelay & 0x3FF) << 6; /* lower 10 bits */

    if ((ret = _phy_8218e_ptpReg_set(unit, port, page, RTL8218E_PTP_GPTP0_RG00_REG,
        reg_val, RTL8218E_PTP_P0_CFG_LINK_DELAY_L_REG_FIELD_MASK)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }


    reg_val = (linkDelay & (0xFFFF << 10)) >> 10; /* upper 16 bits */

    if ((ret = _phy_8218e_ptpReg_set(unit, port, page, RTL8218E_PTP_GPTP0_RG01_REG,
        reg_val, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
        _phy_8218e_ptpReg_access_end(unit, port);
        return ret;
    }

    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

#endif /* !defined(__BOOTLOADER__) */


/* Function Name:
 *      phy_8218e_macIntfSerdes_reset
 * Description:
 *      Reset PHY's MAC (system) interface
 * Input:
 *      unit        - unit id
 *      port        - base mac id
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
phy_8218e_macIntfSerdes_reset(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;
    uint32  park_page = 0x1F;

    if (port != HWP_PHY_BASE_MACID(unit, port))
        return RT_ERR_PORT_ID;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    /* serdes 0 reset */
    /* Bit[1]: SP_SDS_EN_RX; Bit[0]: SP_SDS_EN_TX */
    RT_ERR_HDL(hal_miim_parkBits_write(unit, port, 0x400, park_page, 0x10, 1, 0, 0), ERR, ret);
    RT_ERR_HDL(hal_miim_parkBits_write(unit, port, 0x400, park_page, 0x10, 1, 0, 3), ERR, ret);

    RT_ERR_HDL(hal_miim_park_read(unit, port, 0x260, park_page, 18, &data), ERR, ret);

    /* for QSGMII serdes 1 reset */
    if (0xD0 == (data & 0xF0))
    {
        RT_ERR_HDL(hal_miim_parkBits_write(unit, port, 0x500, park_page, 0x10, 1, 0, 0), ERR, ret);
        RT_ERR_HDL(hal_miim_parkBits_write(unit, port, 0x500, park_page, 0x10, 1, 0, 3), ERR, ret);
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8218e_serdes_linkdown_chk
 * Description:
 *      check phy serdes linkdown status
 * Input:
 *      unit        - unit id
 *      port        - base mac id
 * Output:
 *      pStatus     - pointer to serdes linkdown status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8218e_serdes_linkdown_chk(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    int32   ret = RT_ERR_OK;
    uint32  blk = 0;
    uint32  data = 0;
    uint32  phy_data0 = 0;
    uint32  phy_data1 = 0;
    uint32  phy_data2 = 0;
    uint32  phy_dataA = 0;
    uint32  phy_dataB = 0;
    uint32  phy_dataC = 0;
    uint32  p = 0;
    uint32  sds0_connected = FALSE;
    uint32  sds1_connected = FALSE;

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    RT_ERR_HDL(hal_miim_read(unit, port, 0x260, 18, &data), ERR, ret);

    /* XSGMII */
    RT_ERR_HDL(hal_miim_read(unit, port, 0x407, 0x15, &phy_data0), ERR, ret);
    RT_ERR_HDL(hal_miim_read(unit, port, 0x407, 0x15, &phy_data1), ERR, ret);
    RT_ERR_HDL(hal_miim_read(unit, port, 0x407, 0x15, &phy_data2), ERR, ret);
    /* QSGMII */
    if (0xd0 == (data & 0xF0))
    {
        RT_ERR_HDL(hal_miim_read(unit, port, 0x507, 0x15, &phy_dataA), ERR, ret);
        RT_ERR_HDL(hal_miim_read(unit, port, 0x507, 0x15, &phy_dataB), ERR, ret);
        RT_ERR_HDL(hal_miim_read(unit, port, 0x507, 0x15, &phy_dataC), ERR, ret);
    }

    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, blk), ret);

    *pStatus = 0x0;
    /* QSGMII */
    if (0xd0 == (data & 0xF0))
    {
        for (p=port; p<(port+4); p++)
        {
            if (HWP_PORT_EXIST(unit, p) && (HWP_PHY_BASE_MACID(unit, p) == port) && (HWP_PORT_SDSID(unit, p) != HWP_NONE))
            {
                sds0_connected = TRUE;
                break;
            }
        }
        for (p=(port+4); p<(port+PORT_NUM_IN_8218E); p++)
        {
            if (HWP_PORT_EXIST(unit, p) && (HWP_PHY_BASE_MACID(unit, p) == port) && (HWP_PORT_SDSID(unit, p) != HWP_NONE))
            {
                sds1_connected = TRUE;
                break;
            }
        }

        if (TRUE == sds0_connected)
        {
            if (((phy_data1 & 0xff) != 0xff) || ((phy_data2 & 0xff) != 0xff))
                *pStatus = 0x1;     /* phy serdes had been linkdown */
        }
        if (TRUE == sds1_connected)
        {
            if (((phy_dataB & 0xff) != 0xff) || ((phy_dataC & 0xff) != 0xff))
                *pStatus = 0x1;     /* phy serdes had been linkdown */
        }
    }
    else
    {
        if (((phy_data1 & 0x1ff) != 0x1ff) || ((phy_data2 & 0x1ff) != 0x1ff))
            *pStatus = 0x1;         /* phy serdes had been linkdown */
    }

ERR:
    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


int32
_phy_8218e_interrupt_init(uint32 unit, rtk_port_t port)
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

int32
_phy_8218e_ptp_init(uint32 unit, rtk_port_t port)
{
#if !defined(__BOOTLOADER__)
    int32       ret = RT_ERR_OK;
    rtk_port_t  basePort = 0;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

	if (basePort == port)
	{
		if ((ret = _phy_8218e_ptpReg_access_start(unit, port)) != RT_ERR_OK)
	    {
	        _phy_8218e_ptpReg_access_end(unit, port);
	        return ret;
	    }

		/* Init clock source to internal */
	    if ((ret = _phy_8218e_ptpReg_set(unit, port, RTL8218E_PTP_5_PAGE, RTL8218E_PTP_TMR_RG43_REG, 0, 0X3)) != RT_ERR_OK)
	    {
	        RT_ERR(RT_ERR_FAILED, (MOD_HAL|MOD_TIME), "");
	        _phy_8218e_ptpReg_access_end(unit, port);
	        return ret;
	    }

	    if ((ret = _phy_8218e_ptpReg_access_end(unit, port)) != RT_ERR_OK)
	    {
	        return ret;
	    }

		RT_ERR_CHK(phy_8218e_ptpRefTimeFreqCfg_set(unit, port, 0x10000000, 1), ret);
	}
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218e_init
 * Description:
 *      Initialize PHY 8218E.
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
phy_8218e_init(uint32 unit, rtk_port_t port)
{
    int32       ret = RT_ERR_OK;
    uint32      data = 0;

    if (rtl8218e_info[unit] == NULL)
    {
        if ((rtl8218e_info[unit] = osal_alloc(sizeof(phy_rtl8218e_info_t))) == NULL)
        {
            RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u, port=%u", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
        osal_memset(rtl8218e_info[unit], 0, sizeof(phy_rtl8218e_info_t));
    }

    /* Init info db to default */
    rtl8218e_info[unit]->mdi_val_org[port] = 0;
    rtl8218e_info[unit]->rtctCable[port] = RTK_PHY_CTRL_RTCT_CABLE_TYPE_NONE;
    rtl8218e_info[unit]->cable_factor[port] = 0;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        phy_8218e_greenEnable_set(unit, port, DISABLED);

        RT_ERR_CHK(hal_miim_read(unit, port, 0xa43, 24, &data), ret);
        rtl8218e_info[unit]->mdi_val_org[port] = PHY_8218E_MDI_LB_MDI_VAL_GET(data);
    }

    if ((ret = _phy_8218e_interrupt_init(unit, port)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8218e_ptp_init(unit, port)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218Edrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8218E driver.
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
phy_8218Edrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index                               = RT_PHYDRV_RTL8218E;
    pPhydrv->fPhydrv_init                               = phy_8218e_init;
    pPhydrv->fPhydrv_chipRevId_get                      = phy_8218e_chipRevId_get;
    pPhydrv->fPhydrv_ctrl_get                           = phy_8218e_ctrl_get;
    pPhydrv->fPhydrv_ctrl_set                           = phy_8218e_ctrl_set;
    pPhydrv->fPhydrv_enable_set                         = phy_8218e_enable_set;
    pPhydrv->fPhydrv_enable_get                         = phy_common_enable_get;
    pPhydrv->fPhydrv_ieeeTestMode_set                   = phy_8218e_ieeeTestMode_set;
    pPhydrv->fPhydrv_macIntfSerdesMode_get              = phy_8218e_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_greenEnable_get                    = phy_8218e_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set                    = phy_8218e_greenEnable_set;

#if !defined(__BOOTLOADER__)
    /* Standard register */
    pPhydrv->fPhydrv_reset_set                          = phy_common_reset_set;
    pPhydrv->fPhydrv_media_get                          = phy_8218e_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get                 = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set                 = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get                = phy_8218e_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set                = phy_8218e_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get                         = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set                         = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get                          = phy_8218e_speed_get;
    pPhydrv->fPhydrv_speed_set                          = phy_8218e_speed_set;
    pPhydrv->fPhydrv_masterSlave_get                    = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set                    = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get                       = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set                       = phy_common_loopback_set;
    pPhydrv->fPhydrv_linkStatus_get                     = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get            = phy_common_copperPeerAutoNegoAbility_get;

    /* Common HAL API */
    pPhydrv->fPhydrv_reg_get                            = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set                            = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get                       = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set                       = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get                        = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set                        = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set               = phy_common_reg_mmd_portmask_set;

    /* Private register */
    pPhydrv->fPhydrv_patch_set                          = phy_8218e_patch_set;
    pPhydrv->fPhydrv_patch_perPort_set                  = phy_8218e_patch_perPort_set;
    pPhydrv->fPhydrv_rtct_start                         = phy_8218e_rtct_start;
    pPhydrv->fPhydrv_rtctResult_get                     = phy_8218e_rtctResult_get;
    pPhydrv->fPhydrv_speedStatus_get                    = phy_common_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get              = phy_common_speedDuplexStatusResReg_get;
    pPhydrv->fPhydrv_eeeEnable_get                      = phy_8218e_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set                      = phy_8218e_eeeEnable_set;
    pPhydrv->fPhydrv_crossOverMode_get                  = phy_8218e_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set                  = phy_8218e_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get                = phy_8218e_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get      = phy_8218e_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set      = phy_8218e_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_broadcastEnable_set                = phy_8218e_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set                    = phy_8218e_broadcastID_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get                 = phy_8218e_gigaLiteEnable_get;
    pPhydrv->fPhydrv_gigaLiteEnable_set                 = phy_8218e_gigaLiteEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get                = phy_8218e_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set                = phy_8218e_downSpeedEnable_set;
    pPhydrv->fPhydrv_downSpeedStatus_get                = phy_8218e_downSpeedStatus_get;
    pPhydrv->fPhydrv_ledMode_set                        = phy_8218e_ledMode_set;
    pPhydrv->fPhydrv_ledCtrl_get                        = phy_8218e_ledCtrl_get;
    pPhydrv->fPhydrv_ledCtrl_set                        = phy_8218e_ledCtrl_set;
    pPhydrv->fPhydrv_mdiLoopbackEnable_get              = phy_8218e_mdiLoopbackEnable_get;
    pPhydrv->fPhydrv_mdiLoopbackEnable_set              = phy_8218e_mdiLoopbackEnable_set;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get        = phy_8218e_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_portEyeMonitor_start               = phy_8218e_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get             = phy_8218e_eyeMonitorInfo_get;
    pPhydrv->fPhydrv_sdsEyeParam_get                    = phy_8218e_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set                    = phy_8218e_sdsEyeParam_set;
    pPhydrv->fPhydrv_sdsTestMode_set                    = phy_8218e_sdsTestMode_set;
    pPhydrv->fPhydrv_sdsTestModeCnt_get                 = phy_8218e_sdsTestModeCnt_get;
    pPhydrv->fPhydrv_sdsLeq_get                         = phy_8218e_sdsLeq_get;
    pPhydrv->fPhydrv_sdsLeq_set                         = phy_8218e_sdsLeq_set;
    pPhydrv->fPhydrv_intr_init                          = phy_8218e_intr_init;
    pPhydrv->fPhydrv_intrEnable_get                     = phy_8218e_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set                     = phy_8218e_intrEnable_set;
    pPhydrv->fPhydrv_intrStatus_get                     = phy_8218e_intrStatus_get;
    pPhydrv->fPhydrv_intrMask_get                       = phy_8218e_intrMask_get;
    pPhydrv->fPhydrv_intrMask_set                       = phy_8218e_intrMask_set;

    pPhydrv->fPhydrv_ptpSwitchMacAddr_get               = phy_8218e_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set               = phy_8218e_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get                     = phy_8218e_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set                     = phy_8218e_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set               = phy_8218e_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeFreqCfg_get              = phy_8218e_ptpRefTimeFreqCfg_get;
    pPhydrv->fPhydrv_ptpRefTimeFreqCfg_set              = phy_8218e_ptpRefTimeFreqCfg_set;
    pPhydrv->fPhydrv_ptpEnable_get                      = phy_8218e_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set                      = phy_8218e_ptpEnable_set;
    pPhydrv->fPhydrv_ptpTxInterruptStatus_get           = phy_8218e_ptpTxInterruptStatus_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_get             = phy_8218e_ptpInterruptEnable_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_set             = phy_8218e_ptpInterruptEnable_set;
    pPhydrv->fPhydrv_ptpTxTimestampFifo_get             = phy_8218e_ptpTxTimestampFifo_get;
    pPhydrv->fPhydrv_ptpIgrTpid_get                     = phy_8218e_ptpIgrTpid_get;
    pPhydrv->fPhydrv_ptpIgrTpid_set                     = phy_8218e_ptpIgrTpid_set;
    pPhydrv->fPhydrv_ptpSwitchMacRange_get              = phy_8218e_ptpSwitchMacRange_get;
    pPhydrv->fPhydrv_ptpSwitchMacRange_set              = phy_8218e_ptpSwitchMacRange_set;
    pPhydrv->fPhydrv_ptpOper_get                        = phy_8218e_ptpOper_get;
    pPhydrv->fPhydrv_ptpOper_set                        = phy_8218e_ptpOper_set;
    pPhydrv->fPhydrv_ptpLatchTime_get                   = phy_8218e_ptpLatchTime_get;
    pPhydrv->fPhydrv_ptp1PPSOutput_get                  = phy_8218e_ptp1PPSOutput_get;
    pPhydrv->fPhydrv_ptp1PPSOutput_set                  = phy_8218e_ptp1PPSOutput_set;
    pPhydrv->fPhydrv_ptpClockOutput_get                 = phy_8218e_ptpClockOutput_get;
    pPhydrv->fPhydrv_ptpClockOutput_set                 = phy_8218e_ptpClockOutput_set;
    pPhydrv->fPhydrv_ptpOutputSigSel_get                = phy_8218e_ptpOutputSigSel_get;
    pPhydrv->fPhydrv_ptpOutputSigSel_set                = phy_8218e_ptpOutputSigSel_set;
    pPhydrv->fPhydrv_ptpTransEnable_get                 = phy_8218e_ptpTransEnable_get;
    pPhydrv->fPhydrv_ptpTransEnable_set                 = phy_8218e_ptpTransEnable_set;
    pPhydrv->fPhydrv_ptpLinkDelay_get                   = phy_8218e_ptpLinkDelay_get;
    pPhydrv->fPhydrv_ptpLinkDelay_set                   = phy_8218e_ptpLinkDelay_set;
#endif
}

