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
 * Feature : PHY 8224 Driver APIs.
 *
 */
#ifndef __HAL_PHY_PHY_RTL8224_H__
#define __HAL_PHY_PHY_RTL8224_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8224_info;

/*
 * Macro Declaration
 */
#define RTL8224_PORT_NUM                            (4)
#define RTL8224_PER_PORT_MAX_LED                    (4)

#define RTL8224_INTER_REG_LSB                       (0)
#define RTL8224_INTER_REG_MSB                       (31)

#define RTL8224_PHY_MDI_REVERSE                     0xbb000a90
#define RTL8224_MDI_REVERSE_OFFSET                  (0)

#define RTL8224_PHY_TX_POLARITY_SWAP                0xbb000a94
#define RTL8224_PHY_TX_POLARITY_SWAP_OFFSET         (4)

#define RTL8224_CFG_PHY_POLL_CMD1_REG               (0xbb000ad8)
#define RTL8224_CMD_RD_EN_3_0_MSB                   (3)
#define RTL8224_CMD_RD_EN_3_0_LSB                   (0)


/* Internal Register */
/* Global interrupt control */
#define PHY_8224_INT_MODE_HIGH_LEVEL                (0)
#define PHY_8224_INT_MODE_LOW_LEVEL                 (1)
#define PHY_8224_INT_MODE_POS_EDGE                  (2)
#define PHY_8224_INT_MODE_NEG_EDGE                  (3)

#define RTL8224_ISR_SW_INT_MODE                     0xbb005f84
#define RTL8224_SWITCH_IE_OFFSET                    (6)
#define RTL8224_SWITCH_INT_MODE_OFFSET              (1)
#define RTL8224_SWITCH_INT_MODE_MSB                 (2)

/* GPHY interrupt control */
#define RTL8224_IMR_EXT_GPHY                        0xbb005f5c
#define RTL8224_IMR_EXT_GPHY_3_0_OFFSET             (0)
#define RTL8224_IMR_EXT_GPHY_3_0_MSB                (3)

#define RTL8224_ISR_EXT_GPHY                        0xbb005fb0
#define RTL8224_ISR_EXT_GPHY_OFFSET                 (0)
#define RTL8224_ISR_EXT_GPHY_MSB                    (3)

#define RTL8224_IMR_EXT_WOL                         0xbb005f68
#define RTL8224_IMR_EXT_PHYWOL_PORT_3_0_OFFSET      (9)
#define RTL8224_IMR_EXT_PHYWOL_PORT_3_0_MSB         (12)

#define RTL8224_IMR_INT_RLFD                        0xbb005f40
#define RTL8224_IMR_INT_RLFD_PORT_8_0_OFFSET        (0)

#define RTL8224_IMR_EXT_RLFD                        0xbb005f64
#define RTL8224_IMR_EXT_RLFD_PORT_8_0_OFFSET        (0)
#define RTL8224_IMR_EXT_RLFD_PORT_SHIFT             (4)


#define RTL8224_IMR_EXT_MISC                        0xbb005f78
#define RTL8224_IMR_EXT_TM_LOW_OFFSET               (1)
#define RTL8224_IMR_EXT_TM_HIGH_OFFSET              (0)
#define RTL8224_IMR_EXT_PTP1588_OFFSET              (18)
#define RTL8224_IMR_EXT_MACSECWRP_3_0_OFFSET        (19)
#define RTL8224_IMR_EXT_MACSECWRP_3_0_MSB           (22)

#define RTL8224_ISR_EXT_MISC                        0xbb005fcc
#define RTL8224_ISR_EXT_TM_LOW_OFFSET               (1)
#define RTL8224_ISR_EXT_TM_HIGH_OFFSET              (0)
#define RTL8224_ISR_EXT_PTP1588_OFFSET              (18)
#define RTL8224_ISR_EXT_MACSECWRP_3_0_OFFSET        (19)
#define RTL8224_ISR_EXT_MACSECWRP_3_0_MSB           (22)

#define RTL8224_ISR_EXT_WOL                         0xbb005fbc
#define RTL8224_ISR_EXT_PHYWOL_PORT_3_0_OFFSET      (9)

#define RTL8224_GPHY_INTR_STATUS_REG                (0xbb00a43a)
#define GPHY_AN_ERROR_BIT                           (0)
#define GPHY_AN_NEXT_PAGE_RECEIVE_BIT               (2)
#define GPHY_LINK_CHG_BIT                           (4)
#define GPHY_ALDPS_STS_CHG_BIT                      (9)
#define GPHY_FATEL_ERROR_BIT                        (11)

#define RTL8224_ISR_INT_TM_RLFD_REG                 (0xbb005f94)
#define RTL8224_ISR_INT_RLFD_PORT_8_0_OFFSET        (0)
#define RTL8224_ISR_INT_RLFD_PORT_8_0_MSB           (8)

#define RTL8224_ISR_EXT_TM_RLFD_REG                 (0xbb005fb8)
#define RTL8224_ISR_EXT_RLFD_PORT_8_0_OFFSET        (0)
#define RTL8224_ISR_EXT_RLFD_PORT_8_0_MSB           (8)
#define RTL8224_ISR_EXT_RLFD_PORT_SHIFT             (4)


#define RTL8224_LED_SET_CTRL_OFFSET                 (8)

#define RTL8224_RLFD_CTRL_REG                       (0xbb006458)
#define RTL8224_RLFD_SEL_OFFSET                     (10)
#define RTL8224_RLFD_EN_OFFSET                      (9)

#define RTL8224_LED3_2_SET0_CTRL0_REG               (0xbb006544)
#define RTL8224_SET_LED2_SEL0_OFFSET                (0)
#define RTL8224_SET_LED2_SEL0_MSB                   (15)
#define RTL8224_SET_LED3_SEL0_OFFSET                (16)
#define RTL8224_SET_LED3_SEL0_MSB                   (31)

#define RTL8224_LED1_0_SET0_CTRL0_REG               (0xbb006548)
#define RTL8224_SET_LED0_SEL0_OFFSET                (0)
#define RTL8224_SET_LED0_SEL0_MSB                   (15)
#define RTL8224_SET_LED1_SEL0_OFFSET                (16)
#define RTL8224_SET_LED1_SEL0_MSB                   (31)

#define RTL8224_LED_PORT_SET_SEL_CTRL               (0xbb00654c)
#define RTL8224_LED0_SET_PSEL_OFFSET                (0)
#define RTL8224_LED0_SET_PSEL_FILED_LEN             (2)

/* LED Mode*/
#define PHY_8224_LED_MODE_2P5G                      (1 << 0)
#define PHY_8224_LED_MODE_2P5G_LITE                 (1 << 1)
#define PHY_8224_LED_MODE_1G                        (1 << 2)
#define PHY_8224_LED_MODE_500M                      (1 << 3)
#define PHY_8224_LED_MODE_100M                      (1 << 4)
#define PHY_8224_LED_MODE_10M                       (1 << 5)
#define PHY_8224_LED_MODE_LINK                      (1 << 6)
#define PHY_8224_LED_MODE_LINK_FLASH                (1 << 7)
#define PHY_8224_LED_MODE_ACT_FLASH                 (1 << 8)
#define PHY_8224_LED_MODE_RX_FLASH                  (1 << 9)
#define PHY_8224_LED_MODE_TX_FLASH                  (1 << 10)
#define PHY_8224_LED_MODE_COL_FLASH                 (1 << 11)
#define PHY_8224_LED_MODE_DUPLEX                    (1 << 12)
#define PHY_8224_LED_MODE_TRAINING                  (1 << 13)
#define PHY_8224_LED_MODE_MASTER                    (1 << 14)

#define RTL8224_LED_GLB_CTRL_REG                    (0xbb006520)
#define RTL8224_BLINK_TIME_SEL_OFFSET               (21)
#define RTL8224_BLINK_TIME_SEL_MSB                  (23)

#define RTL8224_LED_GLB_ACTIVE_REG                  (0xbb0065d8)
#define RTL8224_PAD_LED_ACTTIVE_OFFSET              (0)

#define RTL8224_LED_PORT_SW_EN_CTRL                 (0xbb006554)
#define RTL8224_SW_CTRL_LED_EN_OFFSET               (0)
#define RTL8224_SW_CTRL_LED_EN_FIELD_LEN            (4)
#define RTL8224_SW_CTRL_LED_EN_MSB                  (3)

#define RTL8224_LED_PORT_SW_CTRL_PORT_OFFSET        (4)

#define RTL8224_LED_PORT_SW_CTRL                    (0xbb00655c)
#define RTL8224_SW_LED_MODE_OFFSET                  (0)
#define RTL8224_SW_LED_MODE_FIELD_LEN               (3)
#define RTL8224_SW_LED_MODE_MSB                     (2)

#define RTL8224_SW_CTRL_MODE_OFF                    (0)
#define RTL8224_SW_CTRL_MODE_LIGHT                  (7)
#define RTL8224_SW_CTRL_BLINK_256MS                 (4)

/* PTP*/
#define RTL8224_PTP_TIME_TOD_DELAY_R                (0xbb007c20)
#define RTL8224_TOD_DELAY_MSB                       (15)
#define RTL8224_TOD_DELAY_LSB                       (0)

#define RTL8224_PTP_TIME_OP_DURATION_R              (0xbb007c24)
#define RTL8224_TIME_OP_DURATION_MSB                (9)
#define RTL8224_TIME_OP_DURATION_LSB                (0)

#define RTL8224_PTP_OTAG_CONFIG0_R                  (0xbb007c30)
#define RTL8224_OTAG_TPID_0_MSB                     (15)
#define RTL8224_OTAG_TPID_0_LSB                     (0)

#define RTL8224_PTP_OTAG_CONFIG1_R                  (0xbb007c34)
#define RTL8224_OTAG_TPID_1_MSB                     (15)
#define RTL8224_OTAG_TPID_1_LSB                     (0)

#define RTL8224_PTP_OTAG_CONFIG2_R                  (0xbb007c38)
#define RTL8224_OTAG_TPID_2_MSB                     (15)
#define RTL8224_OTAG_TPID_2_LSB                     (0)

#define RTL8224_PTP_OTAG_CONFIG3_R                  (0xbb007c3c)
#define RTL8224_OTAG_TPID_3_MSB                     (15)
#define RTL8224_OTAG_TPID_3_LSB                     (0)

#define RTL8224_PTP_ITAG_CONFIG0_R                  (0xbb007c40)
#define RTL8224_ITAG_TPID_0_MSB                     (15)
#define RTL8224_ITAG_TPID_0_LSB                     (0)

#define RTL8224_PTP_TIME_FREQ0_R                    (0xbb007c50)
#define RTL8224_CFG_PTP_TIME_FREQ0_MSB              (15)
#define RTL8224_CFG_PTP_TIME_FREQ0_LSB              (0)

#define RTL8224_PTP_TIME_FREQ1_R                    (0xbb007c54)
#define RTL8224_CFG_PTP_TIME_FREQ1_EXEC_OFFSET      (14)
#define RTL8224_CFG_PTP_TIME_FREQ1_MSB              (12)
#define RTL8224_CFG_PTP_TIME_FREQ1_LSB              (0)

#define RTL8224_PTP_CUR_TIME_FREQ0_R                (0xbb007c58)
#define RTL8224_CUR_PTP_TIME_FREQ0_MSB              (15)
#define RTL8224_CUR_PTP_TIME_FREQ0_LSB              (0)

#define RTL8224_PTP_CUR_TIME_FREQ1_R                (0xbb007c5c)
#define RTL8224_CUR_PTP_TIME_FREQ1_MSB              (12)
#define RTL8224_CUR_PTP_TIME_FREQ1_LSB              (0)

#define RTL8224_PTP_TIME_NSEC0_R                    (0xbb007c60)
#define RTL8224_CFG_PTP_TIME_NSEC_L_MSB             (15)
#define RTL8224_CFG_PTP_TIME_NSEC_L_LSB             (0)

#define RTL8224_PTP_TIME_NSEC1_R                    (0xbb007c64)
#define RTL8224_CFG_TOD_VALID_OFFSET                (15)
#define RTL8224_CFG_PTP_TIME_NSEC_H_MSB             (13)
#define RTL8224_CFG_PTP_TIME_NSEC_H_LSB             (0)

#define RTL8224_PTP_TIME_SEC0_R                     (0xbb007c68)
#define RTL8224_CFG_PTP_TIME_SEC_L_MSB              (15)
#define RTL8224_CFG_PTP_TIME_SEC_L_LSB              (0)

#define RTL8224_PTP_TIME_SEC1_R                     (0xbb007c6c)
#define RTL8224_CFG_PTP_TIME_SEC_M_MSB              (15)
#define RTL8224_CFG_PTP_TIME_SEC_M_LSB              (0)

#define RTL8224_PTP_TIME_SEC2_R                     (0xbb007c70)
#define RTL8224_CFG_PTP_TIME_SEC_H_MSB              (15)
#define RTL8224_CFG_PTP_TIME_SEC_H_LSB              (0)

#define RTL8224_PTP_TIME_CTRL_R                     (0xbb007c74)
#define RTL8224_PTP_TIME_EXEC_OFFSET                (2)
#define RTL8224_PTP_TIME_CMD_OFFSET                 (0)
#define RTL8224_PTP_TIME_CMD_LSB                    (0)
#define RTL8224_PTP_TIME_CMD_MSB                    (1)
#define RTL8224_PTP_TIME_CMD_ADJ                    (2)
#define RTL8224_PTP_TIME_CMD_WRITE                  (1)
#define RTL8224_PTP_TIME_CMD_READ                   (0)

#define RTL8224_PTP_TIME_NSEC_RD0_R                 (0xbb007c78)
#define RTL8224_RD_PTP_TIME_NSEC_L_MSB              (15)
#define RTL8224_RD_PTP_TIME_NSEC_L_LSB              (0)

#define RTL8224_PTP_TIME_NSEC_RD1_R                 (0xbb007c7c)
#define RTL8224_RD_PTP_TIME_NSEC_H_MSB              (13)
#define RTL8224_RD_PTP_TIME_NSEC_H_LSB              (0)

#define RTL8224_PTP_TIME_SEC_RD0_R                  (0xbb007c80)
#define RTL8224_RD_PTP_TIME_SEC_L_MSB               (15)
#define RTL8224_RD_PTP_TIME_SEC_L_LSB               (0)

#define RTL8224_PTP_TIME_SEC_RD1_R                  (0xbb007c84)
#define RTL8224_RD_PTP_TIME_SEC_M_MSB               (15)
#define RTL8224_RD_PTP_TIME_SEC_M_LSB               (0)

#define RTL8224_PTP_TIME_SEC_RD2_R                  (0xbb007c88)
#define RTL8224_RD_PTP_TIME_SEC_H_MSB               (15)
#define RTL8224_RD_PTP_TIME_SEC_H_LSB               (0)

#define RTL8224_PTP_CLKOUT_NSEC0_R                  (0xbb007c8c)
#define RTL8224_CLKOUT_PTP_TIME_NSEC_L_MSB          (15)
#define RTL8224_CLKOUT_PTP_TIME_NSEC_L_LSB          (0)

#define RTL8224_PTP_CLKOUT_NSEC1_R                  (0xbb007c90)
#define RTL8224_CLKOUT_PTP_TIME_NSEC_H_MSB          (13)
#define RTL8224_CLKOUT_PTP_TIME_NSEC_H_LSB          (0)

#define RTL8224_PTP_CLKOUT_SEC0_R                   (0xbb007c94)
#define RTL8224_CLKOUT_PTP_TIME_SEC_L_MSB           (15)
#define RTL8224_CLKOUT_PTP_TIME_SEC_L_LSB           (0)

#define RTL8224_PTP_CLKOUT_SEC1_R                   (0xbb007c98)
#define RTL8224_CLKOUT_PTP_TIME_SEC_M_MSB           (15)
#define RTL8224_CLKOUT_PTP_TIME_SEC_M_LSB           (0)

#define RTL8224_PTP_CLKOUT_SEC2_R                   (0xbb007c9c)
#define RTL8224_CLKOUT_PTP_TIME_SEC_H_MSB           (15)
#define RTL8224_CLKOUT_PTP_TIME_SEC_H_LSB           (0)

#define RTL8224_PTP_CLKOUT_CTRL_R                   (0xbb007ca0)
#define RTL8224_CFG_PULSE_MODE_OFFSET               (2)
#define RTL8224_CFG_CLKOUT_EN_OFFSET                (1)
#define RTL8224_RD_CLKOUT_RUN_OFFSET                (0)

#define RTL8224_PTP_CLKOUT_HALF_PERD_NS_L_R         (0xbb007ca4)
#define RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_L_MSB     (15)
#define RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_L_LSB     (0)

#define RTL8224_PTP_CLKOUT_HALF_PERD_NS_H_R         (0xbb007ca8)
#define RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_H_MSB     (13)
#define RTL8224_CFG_CLKOUT_HALF_PERIOD_NS_H_LSB     (0)

#define RTL8224_PTP_TIME_OP_CTRL_R                  (0xbb007cac)
#define RTL8224_CFG_GPI_OP_MSB                      (6)
#define RTL8224_CFG_GPI_OP_LSB                      (4)
#define RTL8224_CFG_GPI_RISE_TRIG_OFFSET            (3)
#define RTL8224_CFG_GPI_FALL_TRIG_OFFSET            (2)

#define RTL8224_PTP_PPS_CTRL_R                      (0xbb007cb0)
#define RTL8224_CFG_PPS_EN_OFFSET                   (6)
#define RTL8224_CFG_PPS_WIDTH_MSB                   (5)
#define RTL8224_CFG_PPS_WIDTH_LSB                   (0)

#define RTL8224_PTP_TX_TIMESTAMP_RD0_R              (0xbb007cb4)
#define RTL8224_RD_TX_TIMESTAMP_VALID_OFFSET        (15)
#define RTL8224_RD_PORT_ID_MSB                      (13)
#define RTL8224_RD_PORT_ID_LSB                      (8)
#define RTL8224_RD_MSG_TYPE_MSB                     (7)
#define RTL8224_RD_MSG_TYPE_LSB                     (6)
#define RTL8224_RD_SEQ_ID_H_MSB                     (5)
#define RTL8224_RD_SEQ_ID_H_LSB                     (0)

#define RTL8224_PTP_TX_TIMESTAMP_RD1_R              (0xbb007cb8)
#define RTL8224_RD_SEQ_ID_L_MSB                     (15)
#define RTL8224_RD_SEQ_ID_L_LSB                     (6)
#define RTL8224_RD_TX_TIMESTAMP_SEC_H_MSB           (5)
#define RTL8224_RD_TX_TIMESTAMP_SEC_H_LSB           (0)

#define RTL8224_PTP_TX_TIMESTAMP_RD2_R              (0xbb007cbc)
#define RTL8224_RD_TX_TIMESTAMP_SEC_L_MSB           (15)
#define RTL8224_RD_TX_TIMESTAMP_SEC_L_LSB           (14)
#define RTL8224_RD_TX_TIMESTAMP_NSEC_H_MSB          (13)
#define RTL8224_RD_TX_TIMESTAMP_NSEC_H_LSB          (0)

#define RTL8224_PTP_TX_TIMESTAMP_RD3_R              (0xbb007cc0)
#define RTL8224_RD_TX_TIMESTAMP_NSEC_L_MSB          (15)
#define RTL8224_RD_TX_TIMESTAMP_NSEC_L_LSB          (0)

#define RTL8224_PTP_MIB_INTR_R                      (0xbb007cc4)
#define RTL8224_RD_ISR_PTP_OFFSET                   (1)
#define RTL8224_CFG_IMR_PTP_OFFSET                  (0)

#define RTL8224_PTP_CLK_CTRL_R                      (0xbb007ccc)
#define RTL8224_CFG_CLK_SRC_OFFSET                  (0)

#define RTL8224_P0_REG_PORT_OFFSET                  (0x20)

#define RTL8224_P0_PORT_CTRL_R                      (0xbb007d20)
#define RTL8224_P0_LINK_DELAY_L_MSB                 (15)
#define RTL8224_P0_LINK_DELAY_L_LSB                 (6)
#define RTL8224_P0_CFG_ALWAYS_TS_OFFSET             (5)
#define RTL8224_P0_PTP_ROLE_MSB                     (3)
#define RTL8224_P0_PTP_ROLE_LSB                     (2)
#define RTL8224_P0_CFG_UDP_EN_OFFSET                (1)
#define RTL8224_P0_CFG_ETH_EN_OFFSET                (0)

#define RTL8224_P0_LINK_DELAY_H_R                   (0xbb007d24)
#define RTL8224_P0_LINK_DELAY_H_MSB                 (15)
#define RTL8224_P0_LINK_DELAY_H_LSB                 (0)

#define RTL8224_P0_MISC_CTRL_R                      (0xbb007d28)
#define RTL8224_P0_CFG_BYPASS_OFFSET                (0)

#define RTL8224_P0_TX_IMBAL_R                       (0xbb007d2c)
#define RTL8224_P0_TX_IMBAL_MSB                     (11)
#define RTL8224_P0_TX_IMBAL_LSB                     (0)

#define RTL8224_P0_RX_IMBAL_R                       (0xbb007d30)
#define RTL8224_P0_RX_IMBAL_MSB                     (11)
#define RTL8224_P0_RX_IMBAL_LSB                     (0)

#define RTL8224_TOD_INTR_R                          (0xbb007eb8)
#define RTL8224_ISR_TOD_OFFSET                      (1)
#define RTL8224_IMR_TOD_OFFSET                      (0)

#define RTL8224_IO_MUX_SEL_1_R                      (0xbb007f90)
#define RTL8224_PTP_PPS_OUT_SEL_OFFSET              (22)
#define RTL8224_PTP_CLK_OUT_SEL_OFFSET              (20)

/* Thermal */
#define RTL8224_TM0_CTRL2                           (0xbb0002d8)
#define RTL8224_REG_EN_LATCH_OFFSET                 (2)

#define RTL8224_TM0_CTRL3                           (0xbb0002dc)
#define RTL8224_TM_HIGHCMP_EN_OFFSET                (19)
#define RTL8224_TM_HIGH_THR_SIGN_OFFSET             (18)
#define RTL8224_TM_HIGH_THR_MSB                     (17)
#define RTL8224_TM_HIGH_THR_LSB                     (10)
#define RTL8224_TM_LOWCMP_EN_OFFSET                 (9)
#define RTL8224_TM_LOW_THR_SIGN_OFFSET              (8)
#define RTL8224_TM_LOW_THR_MSB                      (7)
#define RTL8224_TM_LOW_THR_LSB                      (0)

/* MIB */
#define RTL8224_PHY_MIB_GLOBAL_CONFIG               (0xbb0006F0)
#define RTL8224_PHY0_RX_MIB_CNTR0                   (0xbb000710)
#define RTL8224_PHY0_RX_MIB_CNTR1                   (0xbb000714)
#define RTL8224_PHY0_RX_MIB_CNTR2                   (0xbb000718)
#define RTL8224_PHY0_RX_MIB_CNTR3                   (0xbb00071c)
#define RTL8224_PHY0_TX_MIB_CNTR0                   (0xbb000730)
#define RTL8224_PHY0_TX_MIB_CNTR1                   (0xbb000734)
#define RTL8224_PHY0_TX_MIB_CNTR2                   (0xbb000738)
#define RTL8224_PHY0_TX_MIB_CNTR3                   (0xbb00073c)
#define RTL8224_PHY1_RX_MIB_CNTR0                   (0xbb000750)
#define RTL8224_PHY1_RX_MIB_CNTR1                   (0xbb000754)
#define RTL8224_PHY1_RX_MIB_CNTR2                   (0xbb000758)
#define RTL8224_PHY1_RX_MIB_CNTR3                   (0xbb00075c)
#define RTL8224_PHY1_TX_MIB_CNTR0                   (0xbb000770)
#define RTL8224_PHY1_TX_MIB_CNTR1                   (0xbb000774)
#define RTL8224_PHY1_TX_MIB_CNTR2                   (0xbb000778)
#define RTL8224_PHY1_TX_MIB_CNTR3                   (0xbb00077c)
#define RTL8224_PHY2_RX_MIB_CNTR0                   (0xbb000790)
#define RTL8224_PHY2_RX_MIB_CNTR1                   (0xbb000794)
#define RTL8224_PHY2_RX_MIB_CNTR2                   (0xbb000798)
#define RTL8224_PHY2_RX_MIB_CNTR3                   (0xbb00079c)
#define RTL8224_PHY2_TX_MIB_CNTR0                   (0xbb0007b0)
#define RTL8224_PHY2_TX_MIB_CNTR1                   (0xbb0007b4)
#define RTL8224_PHY2_TX_MIB_CNTR2                   (0xbb0007b8)
#define RTL8224_PHY2_TX_MIB_CNTR3                   (0xbb0007bc)
#define RTL8224_PHY3_RX_MIB_CNTR0                   (0xbb0007d0)
#define RTL8224_PHY3_RX_MIB_CNTR1                   (0xbb0007d4)
#define RTL8224_PHY3_RX_MIB_CNTR2                   (0xbb0007d8)
#define RTL8224_PHY3_RX_MIB_CNTR3                   (0xbb0007dc)
#define RTL8224_PHY3_TX_MIB_CNTR0                   (0xbb0007f0)
#define RTL8224_PHY3_TX_MIB_CNTR1                   (0xbb0007f4)
#define RTL8224_PHY3_TX_MIB_CNTR2                   (0xbb0007f8)
#define RTL8224_PHY3_TX_MIB_CNTR3                   (0xbb0007fc)

#define PHY_RTL8224_VER_A                           (0)
#define PHY_RTL8224_VER_B                           (1)
#define PHY_RTL8224_VER_C                           (2)

/* Top Register*/
#define PHY_8224_DBG_CTRL_VAL_REG                   (0xc0d0)
#define PHY_8224_DBG_CTRL_ADR0_REG                  (0xc0b0)
#define PHY_8224_DBG_CTRL_ADR1_REG                  (0xc0b4)
#define PHY_8224_DBG_CTRL_ADR2_REG                  (0xc0b8)
#define PHY_8224_DBG_CTRL_ADR3_REG                  (0xc0bc)

#define PHY_8224_DBG_CTRL_SEL0_REG                  (0xc0c0)
#define PHY_8224_DBG_CTRL_SEL1_REG                  (0xc0c4)
#define PHY_8224_DBG_CTRL_SEL2_REG                  (0xc0c8)
#define PHY_8224_DBG_CTRL_SEL3_REG                  (0xc0cc)
#define PHY_8224_PAD_CTRL_REG                       (0x34)


/*  SerDes Register*/
#define PHY_8224_EYE_SCAN_EN_PAGE                   (0x21)
#define PHY_8224_EYE_SCAN_EN_REG                    (0x11)
#define PHY_8224_EYE_SCAN_EN_HIGH_BIT               (0)
#define PHY_8224_EYE_SCAN_EN_LOW_BIT                (0)

#define PHY_8224_EYE_BIAS_ADJ_PAGE                  (0x2E)
#define PHY_8224_EYE_BIAS_ADJ_REG                   (0x0c)
#define PHY_8224_EYE_BIAS_ADJ_HIGH_BIT              (7)
#define PHY_8224_EYE_BIAS_ADJ_LOW_BIT               (4)

#define PHY_8224_EYE_PI_ADJ_PAGE                    (0x2E)
#define PHY_8224_EYE_PI_ADJ_REG                     (0x0D)
#define PHY_8224_EYE_PI_ADJ_HIGH_BIT                (5)
#define PHY_8224_EYE_PI_ADJ_LOW_BIT                 (2)

#define PHY_8224_EYE_PI_PHASE_PAGE                  (0x2F)
#define PHY_8224_EYE_PI_PHASE_REG                   (0x16)
#define PHY_8224_EYE_PI_PHASE_HIGH_BIT              (10)
#define PHY_8224_EYE_PI_PHASE_LOW_BIT               (5)

#define PHY_8224_EYE_REF_CTRL_PAGE                  (0x36)
#define PHY_8224_EYE_REF_CTRL_REG                   (0x10)
#define PHY_8224_EYE_REF_CTRL_HIGH_BIT              (5)
#define PHY_8224_EYE_REF_CTRL_LOW_BIT               (0)

#define PHY_8224_EYE_READ_PAGE                      (0x21)
#define PHY_8224_EYE_READ_REG                       (0x1b)
#define PHY_8224_EYE_READ_HIGH_BIT                  (7)
#define PHY_8224_EYE_READ_LOW_BIT                   (7)

#define PHY_8224_REG0_TX_PREAMP_PAGE                (0x2E)
#define PHY_8224_REG0_TX_PREAMP_REG                 (0x7)
#define PHY_8224_REG0_TX_PREAMP_HIGH_BIT            (8)
#define PHY_8224_REG0_TX_PREAMP_LOW_BIT             (3)

#define PHY_8224_REG0_TX_PREEN_PAGE                 (0x2E)
#define PHY_8224_REG0_TX_PREEN_REG                  (0x7)
#define PHY_8224_REG0_TX_PREEN_HIGH_BIT             (2)
#define PHY_8224_REG0_TX_PREEN_LOW_BIT              (2)

#define PHY_8224_REG0_TX_MAINAMP_PAGE               (0x2E)
#define PHY_8224_REG0_TX_MAINAMP_REG                (0x7)
#define PHY_8224_REG0_TX_MAINAMP_HIGH_BIT           (15)
#define PHY_8224_REG0_TX_MAINAMP_LOW_BIT            (10)

#define PHY_8224_REG0_TX_MAINEN_PAGE                (0x2E)
#define PHY_8224_REG0_TX_MAINEN_REG                 (0x7)
#define PHY_8224_REG0_TX_MAINEN_HIGH_BIT            (9)
#define PHY_8224_REG0_TX_MAINEN_LOW_BIT             (9)

#define PHY_8224_REG0_TX_POST1AMP_PAGE              (0x2E)
#define PHY_8224_REG0_TX_POST1AMP_REG               (0x6)
#define PHY_8224_REG0_TX_POST1AMP_HIGH_BIT          (15)
#define PHY_8224_REG0_TX_POST1AMP_LOW_BIT           (10)

#define PHY_8224_REG0_TX_POST1EN_PAGE               (0x2E)
#define PHY_8224_REG0_TX_POST1EN_REG                (0x6)
#define PHY_8224_REG0_TX_POST1EN_HIGH_BIT           (3)
#define PHY_8224_REG0_TX_POST1EN_LOW_BIT            (3)

#define PHY_8224_REG0_TX_Z0_PAGE                    (0x2E)
#define PHY_8224_REG0_TX_Z0_REG                     (0xb)
#define PHY_8224_REG0_TX_Z0_HIGH_BIT                (10)
#define PHY_8224_REG0_TX_Z0_LOW_BIT                 (7)

#define PHY_8224_NWAY_OPCODE_PAGE                   (0x7)
#define PHY_8224_NWAY_OPCODE_REG                    (16)
#define PHY_8224_NWAY_OPCODE_HIGH_BIT               (7)
#define PHY_8224_NWAY_OPCODE_LOW_BIT                (0)

#define PHY_8224_NWAY_OPCODE_PAGE                   (0x7)
#define PHY_8224_TGR_PRO_1_REG19                    (19)
#define PHY_8224_USXG_RXDV_OFF_HIGH_BIT             (12)
#define PHY_8224_USXG_RXDV_OFF_LOW_BIT              (12)

#define PHY_8224_AM_PERIOD_PAGE                     (0x6)
#define PHY_8224_TGR_PRO_0_REG17                    (17)
#define PHY_8224_USXG_V2P1P2_RXOFF_HIGH_BIT         (13)
#define PHY_8224_USXG_V2P1P2_RXOFF_LOW_BIT          (13)

#define PHY_8224_AM_PERIOD_PAGE                     (0x6)
#define PHY_8224_AM_PERIOD_REG                      (18)
#define PHY_8224_AM_PERIOD_HIGH_BIT                 (15)
#define PHY_8224_AM_PERIOD_LOW_BIT                  (0)

#define PHY_8224_AM_PAGE                            (0x6)
#define PHY_8224_AM_CFG0_REG                        (19)
#define PHY_8224_AM0_M0_HIGH_BIT                    (7)
#define PHY_8224_AM0_M0_LOW_BIT                     (0)
#define PHY_8224_AM0_M1_HIGH_BIT                    (15)
#define PHY_8224_AM0_M1_LOW_BIT                     (8)

#define PHY_8224_AM_PAGE                            (0x6)
#define PHY_8224_AM_CFG1_REG                        (20)
#define PHY_8224_AM0_M2_HIGH_BIT                    (7)
#define PHY_8224_AM0_M2_LOW_BIT                     (0)
#define PHY_8224_AM1_M0_HIGH_BIT                    (15)
#define PHY_8224_AM1_M0_LOW_BIT                     (8)

#define PHY_8224_AM_PAGE                            (0x6)
#define PHY_8224_AM_CFG2_REG                        (21)
#define PHY_8224_AM1_M1_HIGH_BIT                    (7)
#define PHY_8224_AM1_M1_LOW_BIT                     (0)
#define PHY_8224_AM1_M2_HIGH_BIT                    (15)
#define PHY_8224_AM1_M2_LOW_BIT                     (8)

#define PHY_8224_AM_PAGE                            (0x6)
#define PHY_8224_AM_CFG3_REG                        (22)
#define PHY_8224_AM2_M0_HIGH_BIT                    (7)
#define PHY_8224_AM2_M0_LOW_BIT                     (0)
#define PHY_8224_AM2_M1_HIGH_BIT                    (15)
#define PHY_8224_AM2_M1_LOW_BIT                     (8)

#define PHY_8224_AM_PAGE                            (0x6)
#define PHY_8224_AM_CFG4_REG                        (23)
#define PHY_8224_AM2_M2_HIGH_BIT                    (7)
#define PHY_8224_AM2_M2_LOW_BIT                     (0)
#define PHY_8224_AM3_M0_HIGH_BIT                    (15)
#define PHY_8224_AM3_M0_LOW_BIT                     (8)

#define PHY_8224_AM_PAGE                            (0x6)
#define PHY_8224_AM_CFG5_REG                        (24)
#define PHY_8224_AM3_M1_HIGH_BIT                    (7)
#define PHY_8224_AM3_M1_LOW_BIT                     (0)
#define PHY_8224_AM3_M2_HIGH_BIT                    (15)
#define PHY_8224_AM3_M2_LOW_BIT                     (8)


#define PHY_8224_NWAY_AN_PAGE                       (0x7)
#define PHY_8224_NWAY_AN_REG                        (17)
#define PHY_8224_QHSG_AN_CH3_EN_BIT                 (3)
#define PHY_8224_QHSG_AN_CH2_EN_BIT                 (2)
#define PHY_8224_QHSG_AN_CH1_EN_BIT                 (1)
#define PHY_8224_QHSG_AN_CH0_EN_BIT                 (0)

#define RTL8224_RTCT_LEN_OFFSET                     (620)
#define RTL8224_RTCT_LEN_CABLE_FACTOR               (780)

/* tapbin def bitmask */
#define RTL8224_TAPBIN_DEF_OFFSET                   (8)
#define RTL8224_TAPBIN_DEF_BITMASK                  (0x00000F00)

#define PHY_8224_RLFD_ENABLE_OFFSET                 (15)
/*
 * Function Declaration
 */

/* Function Name:
 *      phy_8224drv_mapperInit
 * Description:
 *      Initialize PHY 8224 driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_8224drv_mapperInit(rt_phydrv_t *pPhydrv);

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
extern int32
phy_8224_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8224_ctrl_set
 * Description:
 *      Set PHY specific settings.
 * Input:
 *      unit      - unit id
 *      port      - port id
  *     ctrl_type - setting type
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
extern int32
phy_8224_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value);

/* Function Name:
 *      phy_8224_ctrl_get
 * Description:
 *      Get PHY specific settings.
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
extern int32
phy_8224_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue);

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
extern int32
phy_8224_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable);

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
extern int32
phy_8224_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable);

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
extern int32
phy_8224_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus);

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
extern int32
phy_8224_linkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

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
extern int32
phy_8224_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
phy_8224_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
phy_8224_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode);

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
extern int32
phy_8224_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode);

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
extern int32
phy_8224_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);

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
extern int32
phy_8224_liteEnable_set(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t enable);

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
extern int32
phy_8224_liteEnable_get(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t *pEnable);

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
extern int32
phy_8224_c45_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);

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
extern int32
phy_8224_ptp_portRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec);

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
extern int32
phy_8224_ptp_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp);

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
extern int32
phy_8224_ptp_portRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec);

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
extern int32
phy_8224_ptp_portRefTimeFreq_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply);

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
extern int32
phy_8224_ptp_portRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur);

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
extern int32
phy_8224_ptp_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224_ptp_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
phy_8224_ptp_portPtpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts);


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
extern int32
phy_8224_ptp_portPtpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


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
extern int32
phy_8224_ptp_portPtpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
phy_8224_ptp_portPtpVlanTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid);

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
extern int32
phy_8224_ptp_portPtpVlanTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid);

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
extern int32
phy_8224_ptp_portPtpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

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
extern int32
phy_8224_ptp_portPtpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

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
extern int32
phy_8224_ptp_portPtpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime);

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
extern int32
phy_8224_ptp_ptpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry);

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
extern int32
phy_8224_ptp_ptp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable);


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
extern int32
phy_8224_ptp_ptp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable);

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
extern int32
phy_8224_ptp_ptpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput);

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
extern int32
phy_8224_ptp_ptpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput);

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
extern int32
phy_8224_ptp_portPtpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay);

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
extern int32
phy_8224_ptp_portPtpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay);

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
extern int32
phy_8224_ptp_portPtpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel);

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
extern int32
phy_8224_ptp_portPtpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel);

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
extern int32
phy_8224_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode);

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
extern int32
phy_8224_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus);

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
extern int32
phy_8224_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
phy_8224_downSpeedEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224_downSpeedEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


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
extern int32
phy_8224_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus);

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
extern int32
phy_8224_mdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


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
extern int32
phy_8224_mdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
phy_8224_eyeMonitor_start(uint32 unit, uint32 port, uint32 sdsId, uint32 frameNum);

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
extern int32
phy_8224_eyeMonitorInfo_get(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo);

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
extern int32
phy_8224_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);


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
extern int32
phy_8224_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

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
extern int32
phy_8224_sdsReg_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData);


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
extern int32
phy_8224_sdsReg_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 data);

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
extern int32
phy_8224_chipVer_get(uint32 unit, rtk_port_t port, uint32 *pVer);

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
extern int32
phy_8224_sdsOpCode_set(uint32 unit, rtk_port_t port, uint32 opCode);

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
extern int32
phy_8224_sdsAmPeriod_set(uint32 unit, rtk_port_t port, uint32 amPeriod);

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
extern int32
phy_8224_sdsAmPeriod_get(uint32 unit, rtk_port_t port, uint32 *pAmPeriod);

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
extern int32
phy_8224_rtct_start(uint32 unit, rtk_port_t port);

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
extern int32
phy_8224_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);

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
extern int32
phy_8224_dbgCounter_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint64 *pCnt);

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
extern int32
phy_8224_sdsAllAm_set(uint32 unit, rtk_port_t port, uint32 value);

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
extern int32
phy_8224_sdsExtra_set(uint32 unit, rtk_port_t port);

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
extern int32
phy_8224_portPatch_backup(uint32 unit);


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
extern int32
phy_8224_portPatch_restore(uint32 unit);

#endif /* __HAL_PHY_PHY_RTL8224_H__ */
