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

#include "rtk_phylib.h"

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
//extern rt_phyInfo_t phy_8224_info;

/*
 * Macro Declaration
 */
#define RTL8224_PORT_NUM                            (4)
#define RTL8224_PER_PORT_MAX_LED                    (4)
#define RTL8224_LED_PAD_MAX_INDEX                   (29)


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

#define RTL8224_RLFD_CTRL_REG                       (0xbb006458)
#define RTL8224_RLFD_SEL_OFFSET                     (10)
#define RTL8224_RLFD_EN_OFFSET                      (9)

#define PHY_RTL8224_VER_A                           (0)
#define PHY_RTL8224_VER_B                           (1)
#define PHY_RTL8224_VER_C                           (2)

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

#define INVALID_BASE_PORT_PHY_ADDRESS               (255)
#define RTL8224_BASE_PORT_OFFSET                    (0)
#define RTL8224_MDI_PIN_SWAP                        (1)
#define RTL8224_MDI_PAIR_SWAP                       (0)
#define RT_ERR_PHY_RTCT_NOT_FINISH                  (-2)

struct rtl8224_priv_info
{
    rtk_phylib_phy_t phytype;
    struct phy_device * basePort;
    struct phy_device * memberPort[RTL8224_PORT_NUM];
    int32  is_basePort;
    int32  port_offset;
};


extern int32 phy_8224_chipVer_get(uint32 unit, rtk_port_t port, uint32 *pVer);
extern int32 phy_8224_sdsReg_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData);
extern int32 phy_8224_sdsReg_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 data);
extern int32 phy_8224_sdsOpCode_set(uint32 unit, rtk_port_t port, uint32 opCode);
extern int32 phy_8224_sdsAmPeriod_set(uint32 unit, rtk_port_t port, uint32 amPeriod);
extern int32 phy_8224_sdsAllAm_set(uint32 unit, rtk_port_t port, uint32 value);
extern int32 phy_8224_sdsExtra_set(uint32 unit, rtk_port_t port);
extern int phy_8224_get_port_offset(rtk_port_t baseId, rtk_port_t port, uint8 *offset);
extern int32 phy_8224_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
extern int32 phy_8224_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);
extern rtk_port_t _phy_8224_get_phydevice_by_offset(int32 unit, rtk_port_t baseId, uint8 offset);
extern int32 _phy_8224_tx_polarity_swap_set(rtk_port_t port, uint32 value);
extern int32 _phy_8224_mdi_reverse_set(rtk_port_t port, uint32 value);
extern int32 _phy_8224_common_c45_enable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);
extern int32 _phy_8224_common_c45_speedDuplexStatusResReg_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);
extern int32 _phy_8224_intr_enable(rtk_port_t port, uint32 enable);
extern int32 _phy_8224_intr_read_clear(rtk_port_t port, uint32 *pStatus);
extern int32 _phy_8224_interrupt_init(uint32 unit, rtk_port_t port);
extern int32 _phy_8224_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);


#endif /* __HAL_PHY_PHY_RTL8224_H__ */
