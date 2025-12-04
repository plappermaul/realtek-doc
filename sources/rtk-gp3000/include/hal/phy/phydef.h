/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : PHY symbol and data type definition in the SDK.
 *
 * Feature : PHY symbol and data type definition
 *
 */

#ifndef __HAL_PHY_PHYDEF_H__
#define __HAL_PHY_PHYDEF_H__

#include <common/rt_type.h>
#include <common/rt_autoconf.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/* PHY Adapt value */
#define PHY_PAGE_MAX                    (32)
#define PHY_REG_MAX                     (32)
#define PHY_REV_ID_MAX                  (15)    /* 0xF */

/* PHY Page Definition */
#define PHY_PAGE_0                      (0)
#define PHY_PAGE_8                      (8)
#define PHY_PAGE_31                     (31)

/* PHY Register 2 */
#define PHY_IDENT_OUI_03_18             (0x001C)

/* PHY Register 3 */
#define PHY_IDENT_OUI_19_24             (0x32)
#define PHY_IDENT_MODEL_TYPE_FE         (0)
#define PHY_IDENT_MODEL_TYPE_GE         (1)
#define PHY_IDENT_MODEL_TYPE_SER        (2)
#define PHY_IDENT_MODEL_ID_OFFSET       (4)

/* Internal PHY */
#define PHY_MODEL_ID_RTL8218B_INT       (0x23)

/* External PHY */
#define PHY_MODEL_ID_RTL8212_EXT        (0x11)
#define PHY_MODEL_ID_RTL8201_EXT        (0x01)


#define PHY_REV_NO_A                    (0)
#define PHY_REV_NO_B                    (1)
#define PHY_REV_NO_C                    (2)
#define PHY_REV_NO_D                    (3)
#define PHY_REV_NO_E                    (4)
#define PHY_REV_NO_F                    (5)
#define PHY_REV_NO_G                    (6)



/* PHY register definition */
#define PHY_CONTROL_REG                         0
#define PHY_STATUS_REG                          1
#define PHY_IDENTIFIER_1_REG                    2
#define PHY_IDENTIFIER_2_REG                    3
#define PHY_AN_ADVERTISEMENT_REG                4
#define PHY_AN_LINKPARTNER_REG                  5
#define PHY_1000_BASET_CONTROL_REG              9
#define PHY_1000_BASET_STATUS_REG               10
#define PHY_EXTENDED_STATUS_REG                 15
#define PHY_PAGE_SELECTION_REG                  31

/* PHY MII register */

/* Register 0: Control Register */
#define Reset_OFFSET                     (15)
#define Reset_MASK                       (0x1U<<Reset_OFFSET)
#define Loopback_OFFSET                  (14)
#define Loopback_MASK                    (0x1U<<Loopback_OFFSET)
#define SpeedSelection0_OFFSET           (13)
#define SpeedSelection0_MASK             (0x1U<<SpeedSelection0_OFFSET)
#define AutoNegotiationEnable_OFFSET     (12)
#define AutoNegotiationEnable_MASK       (0x1U<<AutoNegotiationEnable_OFFSET)
#define PowerDown_OFFSET                 (11)
#define PowerDown_MASK                   (0x1U<<PowerDown_OFFSET)
#define Isolate_OFFSET                   (10)
#define Isolate_MASK                     (0x1U<<Isolate_OFFSET)
#define RestartAutoNegotiation_OFFSET    (9)
#define RestartAutoNegotiation_MASK      (0x1U<<RestartAutoNegotiation_OFFSET)
#define DuplexMode_OFFSET                (8)
#define DuplexMode_MASK                  (0x1U<<DuplexMode_OFFSET)
#define CollisionTest_OFFSET             (7)
#define CollisionTest_MASK               (0x1U<<CollisionTest_OFFSET)
#define SpeedSelection1_OFFSET           (6)
#define SpeedSelection1_MASK             (0x1U<<SpeedSelection1_OFFSET)

/* Register 1: Status Register */
#define _100Base_T4_OFFSET               (15)
#define _100Base_T4_MASK                 (0x1U<<_100Base_T4_OFFSET)
#define _100Base_TX_FD_OFFSET            (14)
#define _100Base_TX_FD_MASK              (0x1U<<_100Base_TX_FD_OFFSET )
#define _100Base_TX_HD_OFFSET            (13)
#define _100Base_TX_HD_MASK              (0x1U<<_100Base_TX_HD_OFFSET)
#define _10Base_T_FD_OFFSET              (12)
#define _10Base_T_FD_MASK                (0x1U<<_10Base_T_FD_OFFSET)
#define _10Base_T_HD_OFFSET              (11)
#define _10Base_T_HD_MASK                (0x1U<<_10Base_T_HD_OFFSET)
#define _100Base_T2_FD_OFFSET            (10)
#define _100Base_T2_FD_MASK              (0x1U<<_100Base_T2_FD_OFFSET)
#define _100Base_T2_HD_OFFSET            (9)
#define _100Base_T2_HD_MASK              (0x1U<<_100Base_T2_HD_OFFSET)
#define ExtendStatus_OFFSET              (8)
#define ExtendStatus_MASK                (0x1U<<ExtendStatus_OFFSET)
#define MFPreambleSuppression_OFFSET     (6)
#define MFPreambleSuppression_MASK       (0x1U<<MFPreambleSuppression_OFFSET)
#define AutoNegotiationComplete_OFFSET   (5)
#define AutoNegotiationComplete_MASK     (0x1U<<AutoNegotiationComplete_OFFSET )
#define RemoteFault_OFFSET               (4)
#define RemoteFault_MASK                 (0x1U<<RemoteFault_OFFSET)
#define AutoNegotiationAbility_OFFSET    (3)
#define AutoNegotiationAbility_MASK      (0x1U<<AutoNegotiationAbility_OFFSET )
#define LinkStatus_OFFSET                (2)
#define LinkStatus_MASK                  (0x1U<<LinkStatus_OFFSET)
#define JabberDetect_OFFSET              (1)
#define JabberDetect_MASK                (0x1U<<JabberDetect_OFFSET)
#define ExtendedCapability_OFFSET        (0)
#define ExtendedCapability_MASK          (0x1U<<ExtendedCapability_OFFSET)

/* Register 2: PHY Identifier 1 */
#define OUI_03_18_OFFSET                 (0)
#define OUI_03_18_MASK                   (0xFFFFU<<OUI_03_18_OFFSET)

/* Register 3: PHY Identifier 2 */
#define OUI_19_24_OFFSET                 (10)
#define OUI_19_24_MASK                   (0x3FU<<OUI_19_24_OFFSET)
#define ModelNumber_OFFSET               (4)
#define ModelNumber_MASK                 (0x3FU<<ModelNumber_OFFSET)
#define RevisionNumber_OFFSET            (0)
#define RevisionNumber_MASK              (0xFU<<RevisionNumber_OFFSET)

/* Register 4: Auto-Negotiation Advertisement */
#define NextPage_R4_OFFSET               (15)
#define NextPage_R4_MASK                 (0x1U<<NextPage_R4_OFFSET)
#define Acknowledge_R4_OFFSET            (14)
#define Acknowledge_R4_MASK              (0x1U<<Acknowledge_R4_OFFSET)
#define RemoteFault_R4_OFFSET            (13)
#define RemoteFault_R4_MASK              (0x1U<<RemoteFault_R4_OFFSET)
#define AsymmetricPause_R4_OFFSET        (11)
#define AsymmetricPause_R4_MASK          (0x1U<<AsymmetricPause_R4_OFFSET)
#define Pause_R4_OFFSET                  (10)
#define Pause_R4_MASK                    (0x1U<<Pause_R4_OFFSET)
#define _100Base_T4_R4_OFFSET            (9)
#define _100Base_T4_R4_MASK              (0x1U<<_100Base_T4_R4_OFFSET)
#define _100Base_TX_FD_R4_OFFSET         (8)
#define _100Base_TX_FD_R4_MASK           (0x1U<<_100Base_TX_FD_R4_OFFSET)
#define _100Base_TX_R4_OFFSET            (7)
#define _100Base_TX_R4_MASK              (0x1U<<_100Base_TX_R4_OFFSET)
#define _10Base_T_FD_R4_OFFSET           (6)
#define _10Base_T_FD_R4_MASK             (0x1U<<_10Base_T_FD_R4_OFFSET)
#define _10Base_T_R4_OFFSET              (5)
#define _10Base_T_R4_MASK                (0x1U<<_10Base_T_R4_OFFSET)
#define SelectorField_R4_OFFSET          (0)
#define SelectorField_R4_MASK            (0x1FU<<SelectorField_R4_OFFSET)

/* Register 4: 1000Base-X Auto-Negotiation Advertisement */
#define _1000BaseX_NextPage_R4_OFFSET          (15)
#define _1000BaseX_NextPage_R4_MASK            (0x1U<<_1000BaseX_NextPage_R4_OFFSET)
#define _1000BaseX_RemoteFault_R4_OFFSET       (12)
#define _1000BaseX_RemoteFault_R4_MASK         (0x3U<<_1000BaseX_RemoteFault_R4_OFFSET)
#define _1000BaseX_AsymmetricPause_R4_OFFSET   (8)
#define _1000BaseX_AsymmetricPause_R4_MASK     (0x1U<<_1000BaseX_AsymmetricPause_R4_OFFSET)
#define _1000BaseX_Pause_R4_OFFSET             (7)
#define _1000BaseX_Pause_R4_MASK               (0x1U<<_1000BaseX_Pause_R4_OFFSET)
#define _1000BaseX_HalfDuplex_R4_OFFSET        (6)
#define _1000BaseX_HalfDuplex_R4_MASK          (0x1U<<_1000BaseX_HalfDuplex_R4_OFFSET)
#define _1000BaseX_FullDuplex_R4_OFFSET        (5)
#define _1000BaseX_FullDuplex_R4_MASK          (0x1U<<_1000BaseX_FullDuplex_R4_OFFSET)

/* Register 5: Auto-Negotiation Link Partner Ability */
#define NextPage_R5_OFFSET               (15)
#define NextPage_R5_MASK                 (0x1U<<NextPage_R5_OFFSET)
#define Acknowledge_R5_OFFSET            (14)
#define Acknowledge_R5_MASK              (0x1U<<Acknowledge_R5_OFFSET)
#define RemoteFault_R5_OFFSET            (13)
#define RemoteFault_R5_MASK              (0x1U<<RemoteFault_R5_OFFSET)
#define AsymmetricPause_R5_OFFSET        (11)
#define AsymmetricPause_R5_MASK          (0x1U<<AsymmetricPause_R5_OFFSET)
#define Pause_R5_OFFSET                  (10)
#define Pause_R5_MASK                    (0x1U<<Pause_R5_OFFSET)
#define _100Base_T4_R5_OFFSET            (9)
#define _100Base_T4_R5_MASK              (0x1U<<_100Base_T4_R5_OFFSET)
#define _100Base_TX_FD_R5_OFFSET         (8)
#define _100Base_TX_FD_R5_MASK           (0x1U<<_100Base_TX_FD_R5_OFFSET)
#define _100Base_TX_R5_OFFSET            (7)
#define _100Base_TX_R5_MASK              (0x1U<<_100Base_TX_R5_OFFSET)
#define _10Base_T_FD_R5_OFFSET           (6)
#define _10Base_T_FD_R5_MASK             (0x1U<<_10Base_T_FD_R5_OFFSET)
#define _10Base_T_R5_OFFSET              (5)
#define _10Base_T_R5_MASK                (0x1U<<_10Base_T_R5_OFFSET)
#define SelectorField_R5_OFFSET          (0)
#define SelectorField_R5_MASK            (0x1FU<<SelectorField_R5_OFFSET)

/* Register 5: 1000Base-X Auto-Negotiation Link Parnter Ability */
#define _1000BaseX_NextPage_R5_OFFSET          (15)
#define _1000BaseX_NextPage_R5_MASK            (0x1U<<_1000BaseX_NextPage_R5_OFFSET)
#define _1000BaseX_Acknowledge_R5_OFFSET       (14)
#define _1000BaseX_Acknowledge_R5_MASK         (0x1U<<_1000BaseX_Acknowledge_R5_OFFSET)
#define _1000BaseX_RemoteFault_R5_OFFSET       (12)
#define _1000BaseX_RemoteFault_R5_MASK         (0x3U<<_1000BaseX_RemoteFault_R5_OFFSET)
#define _1000BaseX_AsymmetricPause_R5_OFFSET   (8)
#define _1000BaseX_AsymmetricPause_R5_MASK     (0x1U<<_1000BaseX_AsymmetricPause_R5_OFFSET)
#define _1000BaseX_Pause_R5_OFFSET             (7)
#define _1000BaseX_Pause_R5_MASK               (0x1U<<_1000BaseX_Pause_R5_OFFSET)
#define _1000BaseX_HalfDuplex_R5_OFFSET        (6)
#define _1000BaseX_HalfDuplex_R5_MASK          (0x1U<<_1000BaseX_HalfDuplex_R5_OFFSET)
#define _1000BaseX_FullDuplex_R5_OFFSET        (5)
#define _1000BaseX_FullDuplex_R5_MASK          (0x1U<<_1000BaseX_FullDuplex_R5_OFFSET)
#define _1000BaseX_SelectorField_R5_OFFSET     (0)
#define _1000BaseX_SelectorField_R5_MASK       (0x1FU<<_1000BaseX_SelectorField_R5_OFFSET)

/* Register 9: 1000Base-T Control Register */
#define TestMode_OFFSET                                 (13)
#define TestMode_MASK                                   (0x7U<<TestMode_OFFSET)
#define MasterSlaveManualConfigurationEnable_OFFSET     (12)
#define MasterSlaveManualConfigurationEnable_MASK       (0x1U<<MasterSlaveManualConfigurationEnable_OFFSET)
#define MasterSlaveConfigurationValue_OFFSET            (11)
#define MasterSlaveConfigurationValue_MASK              (0x1U<<MasterSlaveConfigurationValue_OFFSET)
#define PortType_OFFSET                                 (10)
#define PortType_MASK                                   (0x1U<<PortType_OFFSET)
#define _1000Base_TFullDuplex_OFFSET                    (9)
#define _1000Base_TFullDuplex_MASK                      (0x1U<<_1000Base_TFullDuplex_OFFSET)
#define _1000Base_THalfDuplex_OFFSET                    (8)
#define _1000Base_THalfDuplex_MASK                      (0x1U<<_1000Base_THalfDuplex_OFFSET)

/* Register 15: Extended Status */
#define _1000BaseX_FullDuplex_R15_OFFSET                (15)
#define _1000BaseX_FullDuplex_R15_MASK                  (0x1U<<_1000BaseX_FullDuplex_R15_OFFSET)
#define _1000BaseX_HalfDuplex_R15_OFFSET                (14)
#define _1000BaseX_HalfDuplex_R15_MASK                  (0x1U<<_1000BaseX_HalfDuplex_R15_OFFSET)
#define _1000Base_TFullDuplex_R15_OFFSET                (13)
#define _1000Base_TFullDuplex_R15_MASK                  (0x1U<<_1000Base_TFullDuplex_R15_OFFSET)
#define _1000Base_THalfDuplex_R15_OFFSET                (12)
#define _1000Base_THalfDuplex_R15_MASK                  (0x1U<<_1000Base_THalfDuplex_R15_OFFSET)

#define RTCT_CHECKBUSY_TIMES    12

/*
 * Data Type Definition
 */

/* enum for supported phy device */
typedef enum rt_supported_phy_e
{
    RT_PHY_RTL8218B,
    RT_PHY_END
}rt_supported_phy_t;

/* enum for supported phy driver */
typedef enum rt_supported_phydrv_e
{
    RT_PHYDRV_8218B_INT_GE,
    RT_PHYDRV_END
} rt_supported_phydrv_t;

typedef enum rt_phy_aflags_e
{
    RT_PHY_AFLAG_COMBO = 0,
    RT_PHY_AFLAG_FIBER,
} rt_phy_aflags_t;

#define PHY_AFLAG_NULL      (0)
#define PHY_AFLAG_COMBO     (1 << RT_PHY_AFLAG_COMBO)
#define PHY_AFLAG_FIBER     (1 << RT_PHY_AFLAG_FIBER)

/* definition phy driver structure */
typedef struct rt_phydrv_s
{
    int32   phydrv_index;
    int32   (*fPhydrv_init)(rtk_port_t);
    int32   (*fPhydrv_media_get)(rtk_port_t, rtk_port_media_t *);
    int32   (*fPhydrv_media_set)(rtk_port_t, rtk_port_media_t);
    int32   (*fPhydrv_autoNegoEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_autoNegoEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_autoNegoAbility_get)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*fPhydrv_autoNegoAbility_set)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*fPhydrv_duplex_get)(rtk_port_t, uint32 *);
    int32   (*fPhydrv_duplex_set)(rtk_port_t, uint32);
    int32   (*fPhydrv_speed_get)(rtk_port_t, uint32 *);
    int32   (*fPhydrv_speed_set)(rtk_port_t, uint32);
    int32   (*fPhydrv_enable_set)(rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_rtctResult_get)(rtk_port_t, rtk_rtctResult_t *);
    int32   (*fPhydrv_rtct_start)(rtk_port_t);
    int32   (*fPhydrv_greenEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_greenEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_eeeEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_eeeEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_crossOverMode_get)(rtk_port_t, rtk_port_crossOver_mode_t *);
    int32   (*fPhydrv_crossOverMode_set)(rtk_port_t, rtk_port_crossOver_mode_t);
    int32   (*fPhydrv_broadcastEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_broadcastID_set)(rtk_port_t, uint32);
} rt_phydrv_t;

typedef int32 (*phy_ident_f)(rtk_port_t port, uint32 model_id, uint32 rev_id);
/* definition phy control structure */
typedef struct rt_phyctrl_s
{
    phy_ident_f chk_func;
    uint32      phy_model_id;
    uint32      phy_rev_id;
    rt_phydrv_t *pPhydrv;
    uint32      phy_aflags;
} rt_phyctrl_t;

#endif /* __HAL_PHY_PHYDEF_H__ */
