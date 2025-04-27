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
 * $Revision: 103782 $
 * $Date: 2020-01-20 13:57:09 +0800 (Mon, 20 Jan 2020) $
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
#include <rtk/time.h>
#include <rtk/diag.h>

/*
 * Symbol Definition
 */

/* PHY Adapt value */
#define PHY_PAGE_MAX                    (32)
#define PHY_REG_MAX                     (32)
#define PHY_REV_ID_MAX                  (15)    /* 0xF */
#define MAX_PHY_NUM_OF_CHIP             (8)

/* PHY Page Definition */
#define PHY_PAGE_0                      (0)
#define PHY_PAGE_1                      (1)
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

/* Get PHY model number and revision number value from identifier register reg 2 */
#define PHY_IDENT_MODEL_NUM(_reg3_data)         ((_reg3_data & ModelNumber_MASK) >> ModelNumber_OFFSET)
#define PHY_IDENT_REV_NUM(_reg3_data)           ((_reg3_data & RevisionNumber_MASK) >> RevisionNumber_OFFSET)

#define PHY_MODEL_ID_RTL8208G           (0x08)
#define PHY_MODEL_ID_RTL8208D_EXT       (0x08)
#define PHY_MODEL_ID_RTL8208L           (0x08)
#define PHY_MODEL_ID_RTL8218B_INT       (0x24) /* For RTL8380M internal phy*/
#define PHY_MODEL_ID_RTL8208L_INT       (0x24) /* For RTL8330/RTL8332 internal phy */
#define PHY_MODEL_ID_RTL8218B_EXT       (0x18)
#define PHY_MODEL_ID_RTL8218D           (0x18)
#define PHY_MODEL_ID_RTL8218FB          (0x18)
#define PHY_MODEL_ID_RTL8214FC          (0x18)
#define PHY_MODEL_ID_RTL8218E           (0x18)
#define PHY_MODEL_ID_RTL8214C           (0x14)
#define PHY_MODEL_ID_RTL8214B           (0x14)
#define PHY_MODEL_ID_RTL8212B           (0x14)
#define PHY_MODEL_ID_RTL8214FB          (0x14)
#define PHY_MODEL_ID_RTL8295            (0x2B)
#define PHY_MODEL_ID_RTL8295R           (0x2B)
#define PHY_MODEL_ID_RTL8214QF          (0x2B)
#define PHY_MODEL_ID_RTL8224QF          (0x2B)
#define PHY_MODEL_ID_RTL8226            (0x03)
#define PHY_MODEL_ID_RTL8226B           (0x04)
#define PHY_MODEL_ID_RTL826X            (0x2F)
#define PHY_MODEL_ID_RTL8264B           (0x01)
#define PHY_MODEL_ID_RTL8224            (0x2D)
#define PHY_MODEL_ID_CUST1              (0xC001)
#define PHY_MODEL_ID_CUST2              (0xC002)
#define PHY_MODEL_ID_CUST3              (0xC003)
#define PHY_MODEL_ID_CUST4              (0xC004)
#define PHY_MODEL_ID_CUST5              (0xC005)
#define PHY_MODEL_ID_RTL8211FS          (0x11)
#define PHY_MODEL_ID_NULL               (0x0)


#define PHY_REV_NO_A                    (0)
#define PHY_REV_NO_B                    (1)
#define PHY_REV_NO_C                    (2)
#define PHY_REV_NO_D                    (3)
#define PHY_REV_NO_E                    (4)
#define PHY_REV_NO_F                    (5)
#define PHY_REV_NO_G                    (6)

#define PHY_FIBER_RX_STATUS_0           (0)
#define PHY_FIBER_RX_STATUS_1           (1)
#define PHY_FIBER_RX_STATUS_2           (2)

#define PHY_FIBER_TX_STATUS_0           (0)
#define PHY_FIBER_TX_STATUS_1           (1)


#define PHY_IDENT_RTL8208G  (PHY_MODEL_ID_RTL8208G << PHY_IDENT_MODEL_ID_OFFSET)|\
                            (PHY_REV_NO_A)

#define PHY_IDENT_RTL8208GB (PHY_MODEL_ID_RTL8208G << PHY_IDENT_MODEL_ID_OFFSET)|\
                            (PHY_REV_NO_B)

#define PHY_IDENT_RTL8214FB (PHY_MODEL_ID_RTL8214F << PHY_IDENT_MODEL_ID_OFFSET)|\
                            (PHY_REV_NO_B)

#define PHY_IDENT_RTL8214B  (PHY_MODEL_ID_RTL8214 << PHY_IDENT_MODEL_ID_OFFSET)|\
                            (PHY_REV_NO_B)

#define PHY_IDENT_RTL8218   (PHY_MODEL_ID_RTL8218 << PHY_IDENT_MODEL_ID_OFFSET)|\
                            (PHY_REV_NO_A)

#define PHY_IDENT_RTL8208D  (PHY_MODEL_ID_RTL8208D_INT << PHY_IDENT_MODEL_ID_OFFSET)|\
                            (PHY_REV_NO_A)

typedef enum phy_type_e {
    RTK_PHYTYPE_NONE = 0,       /* no phy connected */
    RTK_PHYTYPE_RTL8208D,
    RTK_PHYTYPE_RTL8208G,
    RTK_PHYTYPE_RTL8208L,
    RTK_PHYTYPE_RTL8208L_INT,   /* For RTL8330/RTL8332 internal PHY */
    RTK_PHYTYPE_RTL8212B,
    RTK_PHYTYPE_RTL8214FB,
    RTK_PHYTYPE_RTL8214B,
    RTK_PHYTYPE_RTL8214FC,
    RTK_PHYTYPE_RTL8214C,
    RTK_PHYTYPE_RTL8218B,
    RTK_PHYTYPE_RTL8218FB,
    RTK_PHYTYPE_RTL8218D,
    RTK_PHYTYPE_RTL8295R,
    RTK_PHYTYPE_RTL8214QF,
    RTK_PHYTYPE_RTL8224QF,
    RTK_PHYTYPE_RTL8218D_NMP,
    RTK_PHYTYPE_RTL8295R_C22,
    RTK_PHYTYPE_RTL8214QF_NC5,
    RTK_PHYTYPE_RTL8226,
    RTK_PHYTYPE_RTL8226B,
    RTK_PHYTYPE_RTL8218E,
    RTK_PHYTYPE_RTL8261,
    RTK_PHYTYPE_RTL8264,
    RTK_PHYTYPE_RTL8261I,
    RTK_PHYTYPE_RTL8264I,
    RTK_PHYTYPE_RTL8251,
    RTK_PHYTYPE_RTL8254,
    RTK_PHYTYPE_RTL8251I,
    RTK_PHYTYPE_RTL8254I,
    RTK_PHYTYPE_RTL8251L,
    RTK_PHYTYPE_RTL8254L,
    RTK_PHYTYPE_RTL8224,
    RTK_PHYTYPE_RTL8261B,
    RTK_PHYTYPE_RTL8264B,
    RTK_PHYTYPE_SERDES,
    RTK_PHYTYPE_CUST1,
    RTK_PHYTYPE_CUST2,
    RTK_PHYTYPE_CUST3,
    RTK_PHYTYPE_CUST4,
    RTK_PHYTYPE_CUST5,
    RTK_PHYTYPE_EXP_RTL8211FS,
    RTK_PHYTYPE_UNKNOWN,        /* phy connected, but unknown type */
    RTK_PHYTYPE_INVALID,        /* somehting wrong in hardware profile */
    RTK_PHYTYPE_END,
} phy_type_t;

#define RTK_PHYTYPE_STRING_NONE             "NONE"
#define RTK_PHYTYPE_STRING_RTL8208D         "8208D"
#define RTK_PHYTYPE_STRING_RTL8208G         "8208G"
#define RTK_PHYTYPE_STRING_RTL8208L         "8208L"
#define RTK_PHYTYPE_STRING_RTL8208L_INT     "8208L_INT"
#define RTK_PHYTYPE_STRING_RTL8212B         "8212B"
#define RTK_PHYTYPE_STRING_RTL8214FB        "8214FB"
#define RTK_PHYTYPE_STRING_RTL8214B         "8214B"
#define RTK_PHYTYPE_STRING_RTL8214FC        "8214FC"
#define RTK_PHYTYPE_STRING_RTL8214C         "8214C"
#define RTK_PHYTYPE_STRING_RTL8218B         "8218B"
#define RTK_PHYTYPE_STRING_RTL8218FB        "8218FB"
#define RTK_PHYTYPE_STRING_RTL8218D         "8218D"
#define RTK_PHYTYPE_STRING_RTL8295R         "8295R"
#define RTK_PHYTYPE_STRING_RTL8214QF        "8214QF"
#define RTK_PHYTYPE_STRING_RTL8224QF        "8224QF"
#define RTK_PHYTYPE_STRING_RTL8218D_NMP     "RTL8218D_NMP"
#define RTK_PHYTYPE_STRING_RTL8295R_C22     "RTL8295R_C22"
#define RTK_PHYTYPE_STRING_RTL8214QF_NC5    "RTL8214QF_NC5"
#define RTK_PHYTYPE_STRING_RTL8226          "RTL8226"
#define RTK_PHYTYPE_STRING_RTL8226B         "RTL8226B"
#define RTK_PHYTYPE_STRING_RTL8218E         "8218E"
#define RTK_PHYTYPE_STRING_RTL8261          "RTL8261"
#define RTK_PHYTYPE_STRING_RTL8264          "RTL8264"
#define RTK_PHYTYPE_STRING_RTL8261I          "RTL8261I"
#define RTK_PHYTYPE_STRING_RTL8264I          "RTL8264I"
#define RTK_PHYTYPE_STRING_RTL8251           "RTL8251"
#define RTK_PHYTYPE_STRING_RTL8254           "RTL8254"
#define RTK_PHYTYPE_STRING_RTL8251I          "RTL8251I"
#define RTK_PHYTYPE_STRING_RTL8254I          "RTL8254I"
#define RTK_PHYTYPE_STRING_RTL8251L          "RTL8251L"
#define RTK_PHYTYPE_STRING_RTL8254L          "RTL8254L"
#define RTK_PHYTYPE_STRING_RTL8224           "RTL8224"
#define RTK_PHYTYPE_STRING_RTL8261B         "RTL8261B/8261N"
#define RTK_PHYTYPE_STRING_RTL8264B         "RTL8264B"
#define RTK_PHYTYPE_STRING_SERDES           "SERDES"
#define RTK_PHYTYPE_STRING_CUST1            "CUST1"
#define RTK_PHYTYPE_STRING_CUST2            "CUST2"
#define RTK_PHYTYPE_STRING_CUST3            "CUST3"
#define RTK_PHYTYPE_STRING_CUST4            "CUST4"
#define RTK_PHYTYPE_STRING_CUST5            "CUST5"
#define RTK_PHYTYPE_STRING_EXP_RTL8211FS    "RTL8211FS"
#define RTK_PHYTYPE_STRING_UNKNOWN          "UNKNOWN"
#define RTK_PHYTYPE_STRING_INVALID          "INVALID"
#define RTK_PHYTYPE_STRING_END              "END"

#define RTK_PHYTYPE_STRING                         \
    (uint8 *)RTK_PHYTYPE_STRING_NONE,              \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8208D,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8208G,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8208L,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8208L_INT,      \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8212B,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8214FB,         \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8214B,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8214FC,         \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8214C,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8218B,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8218FB,         \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8218D,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8295R,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8214QF,         \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8224QF,         \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8218D_NMP,      \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8295R_C22,      \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8214QF_NC5,     \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8226,           \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8226B,           \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8218E,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8261,           \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8264,           \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8261I,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8264I,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8251 ,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8254 ,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8251I,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8254I,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8251L,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8254L,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8224 ,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8261B,          \
    (uint8 *)RTK_PHYTYPE_STRING_RTL8264B,          \
    (uint8 *)RTK_PHYTYPE_STRING_SERDES,            \
    (uint8 *)RTK_PHYTYPE_STRING_CUST1,             \
    (uint8 *)RTK_PHYTYPE_STRING_CUST2,             \
    (uint8 *)RTK_PHYTYPE_STRING_CUST3,             \
    (uint8 *)RTK_PHYTYPE_STRING_CUST4,             \
    (uint8 *)RTK_PHYTYPE_STRING_CUST5,             \
    (uint8 *)RTK_PHYTYPE_STRING_EXP_RTL8211FS,     \
    (uint8 *)RTK_PHYTYPE_STRING_UNKNOWN,           \
    (uint8 *)RTK_PHYTYPE_STRING_INVALID,           \
    (uint8 *)RTK_PHYTYPE_STRING_END                \


#define RTK_PHYINFO_FLAG_NONE                       (0x0)
#define RTK_PHYINFO_FLAG_1G_MMD_CFG                 (0x1 << 0)  /* for 10G/2.5G PHY, specify this flag to indicate to configre 1G duplex,speed,... MMD register */
#define RTK_PHYINFO_FLAG_NO_RES_REG                 (0x1 << 1)  /* not support RTK proprietary PHY status resolution register */
#define RTK_PHYINFO_FLAG_FORCE_RES                  (0x1 << 2)  /* Force RTK proprietary PHY status resolution register */


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

#define PHY_INT_REG_BASE                        16
#define PHY_INT_REG_NEXT                        8

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

/* Register 10: 1000Base-T Status Register */
#define MasterSlaveConfigurationFault_OFFSET            (15)
#define MasterSlaveConfigurationFault_MASK              (0x1U<<MasterSlaveConfigurationFault_OFFSET)
#define MasterSlaveConfigurationResolution_OFFSET       (14)
#define MasterSlaveConfigurationResolution_MASK         (0x1U<<MasterSlaveConfigurationResolution_OFFSET)
#define LocalReceiverStatus_OFFSET                      (13)
#define LocalReceiverStatus_MASK                        (0x1U<<LocalReceiverStatus_OFFSET)
#define LocalRemoteStatus_OFFSET                        (12)
#define LocalRemoteStatus_MASK                          (0x1U<<LocalRemoteStatus_OFFSET)
#define _1000Base_TLinkPartnerFullDuplex_OFFSET         (11)
#define _1000Base_TLinkPartnerFullDuplex_MASK           (0x1U<<_1000Base_TLinkPartnerFullDuplex_OFFSET)
#define _1000Base_TLinkPartnerHalfDuplex_OFFSET         (10)
#define _1000Base_TLinkPartnerHalfDuplex_MASK           (0x1U<<_1000Base_TLinkPartnerHalfDuplex_OFFSET)
#define IdleErrorCount_OFFSET                           (0)
#define IdleErrorCount_MASK                             (0xffU<<IdleErrorCount_OFFSET)

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


#define PHY_RESL_REG_REAL_TIME_JABBER_OFFSET                    0
#define PHY_RESL_REG_REAL_TIME_JABBER_MASK                      (0x1 << PHY_RESL_REG_REAL_TIME_JABBER_OFFSET)
#define PHY_RESL_REG_MDI_OFFSET                                 1
#define PHY_RESL_REG_MDI_MASK                                   (0x1 << PHY_RESL_REG_MDI_OFFSET)
#define PHY_RESL_REG_LINK_OFFSET                                2
#define PHY_RESL_REG_LINK_MASK                                  (0x1 << PHY_RESL_REG_LINK_OFFSET)
#define PHY_RESL_REG_DUPLEX_OFFSET                              3
#define PHY_RESL_REG_DUPLEX_MASK                                (0x1 << PHY_RESL_REG_DUPLEX_OFFSET)
#define PHY_RESL_REG_SPEED0_OFFSET                              4
#define PHY_RESL_REG_SPEED0_MASK                                (0x3 << PHY_RESL_REG_SPEED0_OFFSET)
#define PHY_RESL_REG_TX_PAUSE_OFFSET                            6
#define PHY_RESL_REG_TX_PAUSE_MASK                              (0x1 << PHY_RESL_REG_TX_PAUSE_OFFSET)
#define PHY_RESL_REG_RX_PAUSE_OFFSET                            7
#define PHY_RESL_REG_RX_PAUSE_MASK                              (0x1 << PHY_RESL_REG_RX_PAUSE_OFFSET)
#define PHY_RESL_REG_EEE_CAP_OFFSET                             8
#define PHY_RESL_REG_EEE_CAP_MASK                               (0x1 << PHY_RESL_REG_EEE_CAP_OFFSET)
#define PHY_RESL_REG_SPEED1_OFFSET                              9
#define PHY_RESL_REG_SPEED1_MASK                                (0x3 << PHY_RESL_REG_SPEED1_OFFSET)
#define PHY_RESL_REG_MASTER_SLAVE_OFFSET                        11
#define PHY_RESL_REG_MASTER_SLAVE_MASK                          (0x1 << PHY_RESL_REG_MASTER_SLAVE_OFFSET)
#define PHY_RESL_REG_NWAY_ENABLE_OFFSET                         12
#define PHY_RESL_REG_NWAY_ENABLE_MASK                           (0x1 << PHY_RESL_REG_NWAY_ENABLE_OFFSET)
#define PHY_RESL_REG_LDPS_STATE_OFFSET                          14
#define PHY_RESL_REG_LDPS_STATE_MASK                            (0x1 << PHY_RESL_REG_LDPS_STATE_OFFSET)


#define PHY_RESL_STS_MDI(status)       ((status & PHY_RESL_REG_MDI_MASK)?PORT_CROSSOVER_MODE_MDIX:PORT_CROSSOVER_MODE_MDI)
#define PHY_RESL_STS_LINK(status)      ((status & PHY_RESL_REG_LINK_MASK)?PORT_LINKUP:PORT_LINKDOWN)
#define PHY_RESL_STS_DUPLEX(status)    ((status & PHY_RESL_REG_DUPLEX_MASK)?PORT_FULL_DUPLEX:PORT_HALF_DUPLEX)
#define PHY_RESL_STS_TXPAUSE(status)   ((status & PHY_RESL_REG_TX_PAUSE_MASK)?1:0)
#define PHY_RESL_STS_RXPAUSE(status)   ((status & PHY_RESL_REG_RX_PAUSE_MASK)?1:0)
#define PHY_RESL_STS_EEECAP(status)    ((status & PHY_RESL_REG_EEE_CAP_MASK)?1:0)
#define PHY_RESL_STS_SPEED(status)      _phy_resolutionResult2Speed_get(status)

/*
 * Data Type Definition
 */

/* enum for supported phy driver */
typedef enum rt_supported_phydrv_e
{
    RT_PHYDRV_RTL8208G,
    RT_PHYDRV_RTL8208D,
    RT_PHYDRV_RTL8214FB,
    RT_PHYDRV_RTL8214B,
    RT_PHYDRV_RTL8212B,
    RT_PHYDRV_RTL8380_SERDES_GE,
    RT_PHYDRV_RTL8380_INT_GE,
    RT_PHYDRV_RTL8218B,
    RT_PHYDRV_RTL8218FB,
    RT_PHYDRV_RTL8218FB_MP,
    RT_PHYDRV_RTL8214FC,
    RT_PHYDRV_RTL8214FC_MP,
    RT_PHYDRV_RTL8218D,
    RT_PHYDRV_RTL8390_SERDES_GE,
    RT_PHYDRV_RTL8390_SERDES_10GE,
    RT_PHYDRV_RTL8214C,
    RT_PHYDRV_RTL9300_SERDES,
    RT_PHYDRV_RTL9300_INT_GE,
    RT_PHYDRV_RTL9310_SERDES,
    RT_PHYDRV_RTL8295R,
    RT_PHYDRV_RTL8214QF,
    RT_PHYDRV_RTL8224QF,
    RT_PHYDRV_RTL8226,
    RT_PHYDRV_RTL8226B,
    RT_PHYDRV_RTL8218E,
    RT_PHYDRV_RTL826X,
    RT_PHYDRV_RTL825XL,
    RT_PHYDRV_RTL8224,
    RT_PHYDRV_CUST1,
    RT_PHYDRV_CUST2,
    RT_PHYDRV_CUST3,
    RT_PHYDRV_CUST4,
    RT_PHYDRV_CUST5,
    RT_PHYDRV_EXP_RTL8211FS,
    RT_PHYDRV_END
} rt_supported_phydrv_t;

/* definition phy driver structure */
typedef struct rt_phydrv_s
{
    int32   phydrv_index;
    int32   (*fPhydrv_init)(uint32, rtk_port_t);
    int32   (*fPhydrv_ctrl_get)(uint32, rtk_port_t, rtk_phy_ctrl_t, uint32 *);
    int32   (*fPhydrv_ctrl_set)(uint32, rtk_port_t, rtk_phy_ctrl_t, uint32);
    int32   (*fPhydrv_media_get)(uint32, rtk_port_t, rtk_port_media_t *);
    int32   (*fPhydrv_media_set)(uint32, rtk_port_t, rtk_port_media_t);
    int32   (*fPhydrv_autoNegoEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_autoNegoEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_autoNegoAbilityLocal_get)(uint32, rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*fPhydrv_autoNegoAbility_get)(uint32, rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*fPhydrv_autoNegoAbility_set)(uint32, rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*fPhydrv_duplex_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_duplex_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_speed_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_speed_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_enable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_enable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_rtctResult_get)(uint32, rtk_port_t, rtk_rtctResult_t *);
    int32   (*fPhydrv_rtct_start)(uint32, rtk_port_t);
    int32   (*fPhydrv_greenEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_greenEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_eeeEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_eeeEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_crossOverMode_get)(uint32, rtk_port_t, rtk_port_crossOver_mode_t *);
    int32   (*fPhydrv_crossOverMode_set)(uint32, rtk_port_t, rtk_port_crossOver_mode_t);
    int32   (*fPhydrv_crossOverStatus_get)(uint32, rtk_port_t, rtk_port_crossOver_status_t *);
    int32   (*fPhydrv_fiber_media_get)(uint32, rtk_port_t, rtk_port_fiber_media_t *);
    int32   (*fPhydrv_fiber_media_set)(uint32, rtk_port_t, rtk_port_fiber_media_t);
    int32   (*fPhydrv_linkDownPowerSavingEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_linkDownPowerSavingEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_broadcastEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_broadcastID_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_gigaLiteEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_gigaLiteEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_eeepEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_eeepEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_patch_set)(uint32, rtk_port_t);
    int32   (*fPhydrv_patch_perPort_set)(uint32, rtk_port_t, rtk_portmask_t *);
    int32   (*fPhydrv_downSpeedEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_downSpeedEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_downSpeedStatus_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_fiberDownSpeedEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_fiberDownSpeedEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_fiberNwayForceLinkEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_fiberNwayForceLinkEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_fiberOAMLoopBackEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_ptpSwitchMacAddr_get)(uint32, rtk_port_t, rtk_mac_t *);
    int32   (*fPhydrv_ptpSwitchMacAddr_set)(uint32, rtk_port_t, rtk_mac_t *);
    int32   (*fPhydrv_ptpRefTime_get)(uint32, rtk_port_t, rtk_time_timeStamp_t *);
    int32   (*fPhydrv_ptpRefTime_set)(uint32, rtk_port_t, rtk_time_timeStamp_t, uint32);
    int32   (*fPhydrv_ptpRefTimeAdjust_set)(uint32, rtk_port_t, uint32, rtk_time_timeStamp_t, uint32);
    int32   (*fPhydrv_ptpRefTimeEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_ptpRefTimeEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_ptpRefTimeFreq_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_ptpRefTimeFreq_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_ptpEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_ptpEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_ptpInterruptStatus_get)(uint32, rtk_port_t, uint32 *, uint32 *);
    int32   (*fPhydrv_ptpInterruptEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_ptpInterruptEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_ptpIgrTpid_get)(uint32, rtk_port_t, rtk_vlanType_t, uint32, uint32 *);
    int32   (*fPhydrv_ptpIgrTpid_set)(uint32, rtk_port_t, rtk_vlanType_t, uint32, uint32);
    int32   (*fPhydrv_ptpRxTimestamp_get)(uint32, rtk_port_t, rtk_time_ptpIdentifier_t, rtk_time_timeStamp_t *);
    int32   (*fPhydrv_ptpTxTimestamp_get)(uint32, rtk_port_t, rtk_time_ptpIdentifier_t, rtk_time_timeStamp_t *);
    int32   (*fPhydrv_ptpSwitchMacRange_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_ptpSwitchMacRange_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_ptpOper_get)(uint32 , rtk_port_t , rtk_time_operCfg_t *);
    int32   (*fPhydrv_ptpOper_set)(uint32 , rtk_port_t , rtk_time_operCfg_t *);
    int32   (*fPhydrv_ptpLatchTime_get)(uint32 , rtk_port_t , rtk_time_timeStamp_t *);
    int32   (*fPhydrv_ptpRefTimeFreqCfg_get)(uint32, rtk_port_t, uint32 *, uint32 *);
    int32   (*fPhydrv_ptpRefTimeFreqCfg_set)(uint32, rtk_port_t, uint32, uint32);
    int32   (*fPhydrv_ptpTxInterruptStatus_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_ptpTxTimestampFifo_get)(uint32, rtk_port_t, rtk_time_txTimeEntry_t *);
    int32   (*fPhydrv_ptp1PPSOutput_get)(uint32, rtk_port_t, uint32 *, rtk_enable_t *);
    int32   (*fPhydrv_ptp1PPSOutput_set)(uint32, rtk_port_t, uint32, rtk_enable_t);
    int32   (*fPhydrv_ptpClockOutput_get)(uint32, rtk_port_t, rtk_time_clkOutput_t *);
    int32   (*fPhydrv_ptpClockOutput_set)(uint32, rtk_port_t, rtk_time_clkOutput_t *);
    int32   (*fPhydrv_ptpOutputSigSel_get)(uint32, rtk_port_t, rtk_time_outSigSel_t *);
    int32   (*fPhydrv_ptpOutputSigSel_set)(uint32, rtk_port_t, rtk_time_outSigSel_t);
    int32   (*fPhydrv_ptpTransEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_ptpTransEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_ptpLinkDelay_get)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_ptpLinkDelay_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_reg_get)(uint32, rtk_port_t, uint32, uint32, uint32 *);
    int32   (*fPhydrv_reg_set)(uint32, rtk_port_t, uint32, uint32, uint32);
    int32   (*fPhydrv_reg_park_get)(uint32, rtk_port_t, uint32, uint32, uint32, uint32 *);
    int32   (*fPhydrv_reg_park_set)(uint32, rtk_port_t, uint32, uint32, uint32, uint32);
    int32   (*fPhydrv_reg_mmd_get)(uint32, rtk_port_t, uint32, uint32, uint32 *);
    int32   (*fPhydrv_reg_mmd_set)(uint32, rtk_port_t, uint32, uint32, uint32);
    int32   (*fPhydrv_reg_mmd_portmask_set)(uint32, rtk_portmask_t, uint32, uint32, uint32);
    int32   (*fPhydrv_reg_extParkPage_get)(uint32, rtk_port_t, uint32, uint32, uint32, uint32, uint32 *);
    int32   (*fPhydrv_reg_extParkPage_set)(uint32, rtk_port_t, uint32, uint32, uint32, uint32, uint32);
    int32   (*fPhydrv_reg_extParkPage_portmask_set)(uint32, rtk_portmask_t, uint32, uint32, uint32, uint32, uint32);
    int32   (*fPhydrv_masterSlave_get)(uint32, rtk_port_t, rtk_port_masterSlave_t *, rtk_port_masterSlave_t *);
    int32   (*fPhydrv_masterSlave_set)(uint32, rtk_port_t, rtk_port_masterSlave_t);
    int32   (*fPhydrv_loopback_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_loopback_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_fiberTxDis_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_fiberTxDisPin_set)(uint32, rtk_port_t, uint32);
    int32   (*fPhydrv_fiberRx_check)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_fiberRx_reset)(uint32, rtk_port_t);
    int32   (*fPhydrv_serdesFiberRx_check)(uint32, rtk_port_t, uint32 *);
    int32   (*fPhydrv_serdesFiberRx_reset)(uint32, rtk_port_t);
    int32   (*fPhydrv_fiberLinkUp_handler1)(uint32, rtk_port_t);
    int32   (*fPhydrv_fiberRxEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_fiberRxEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_10gMedia_get)(uint32, rtk_port_t, rtk_port_10gMedia_t *);
    int32   (*fPhydrv_10gMedia_set)(uint32, rtk_port_t, rtk_port_10gMedia_t);
    int32   (*fPhydrv_ieeeTestMode_set)(uint32, rtk_port_t, rtk_port_phyTestMode_t *);
    int32   (*fPhydrv_polar_get)(uint32, rtk_port_t, rtk_port_phyPolarCtrl_t *);
    int32   (*fPhydrv_polar_set)(uint32, rtk_port_t, rtk_port_phyPolarCtrl_t *);
    int32   (*fPhydrv_portEyeMonitor_start)(uint32, rtk_port_t, uint32, uint32);
    int32   (*fPhydrv_portEyeMonitorInfo_get)(uint32, rtk_port_t, uint32, uint32, rtk_sds_eyeMonInfo_t *);
    int32   (*fPhydrv_chipRevId_get)(uint32, rtk_port_t, uint16 *);
    int32   (*fPhydrv_portimageFlash_load)(uint32, rtk_port_t, uint32, uint8 *);
    int32   (*fPhydrv_sds_get)(uint32 , rtk_port_t , rtk_sdsCfg_t *);
    int32   (*fPhydrv_sds_set)(uint32 , rtk_port_t , rtk_sdsCfg_t *);
    int32   (*fPhydrv_sdsReg_get)(uint32, rtk_port_t, uint32, uint32, uint32 *);
    int32   (*fPhydrv_sdsReg_set)(uint32, rtk_port_t, uint32, uint32, uint32);
    int32   (*fPhydrv_sdsRxCaliStatus_get)(uint32 , rtk_port_t , uint32 , rtk_port_phySdsRxCaliStatus_t *);
    int32   (*fPhydrv_fiberUnidirEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_reset_set)(uint32, rtk_port_t);
    int32   (*fPhydrv_linkStatus_get)(uint32, rtk_port_t, rtk_port_linkStatus_t *);
    int32   (*fPhydrv_peerAutoNegoAbility_get)(uint32, rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*fPhydrv_macIntfSerdesMode_get)(uint32, rtk_port_t, rt_serdesMode_t *);
    int32   (*fPhydrv_ledMode_set)(uint32, rtk_port_t, rtk_phy_ledMode_t *);
    int32   (*fPhydrv_ledCtrl_get)(uint32, rtk_port_t, rtk_phy_ledCtrl_t *);
    int32   (*fPhydrv_ledCtrl_set)(uint32, rtk_port_t, rtk_phy_ledCtrl_t *);
    int32   (*fPhydrv_macIntfSerdesLinkStatus_get)(uint32, rtk_port_t, rtk_phy_macIntfSdsLinkStatus_t *);
    int32   (*fPhydrv_sdsEyeParam_get)(uint32, rtk_port_t, uint32, rtk_sds_eyeParam_t *);
    int32   (*fPhydrv_sdsEyeParam_set)(uint32, rtk_port_t, uint32, rtk_sds_eyeParam_t *);
    int32   (*fPhydrv_2pt5gLiteEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_2pt5gLiteEnable_set)(uint32, rtk_port_t, rtk_enable_t );
    int32   (*fPhydrv_mdiLoopbackEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fPhydrv_mdiLoopbackEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fPhydrv_intr_init)(uint32, rtk_port_t, rtk_phy_intr_t);
    int32   (*fPhydrv_intrEnable_get)(uint32, rtk_port_t, rtk_phy_intr_status_t, rtk_enable_t *);
    int32   (*fPhydrv_intrEnable_set)(uint32, rtk_port_t, rtk_phy_intr_status_t, rtk_enable_t);
    int32   (*fPhydrv_intrStatus_get)(uint32, rtk_port_t, rtk_phy_intr_t, rtk_phy_intrStatusVal_t *);
    int32   (*fPhydrv_intrMask_get)(uint32, rtk_port_t, rtk_phy_intr_t, uint32 *);
    int32   (*fPhydrv_intrMask_set)(uint32, rtk_port_t, rtk_phy_intr_t, uint32);
    int32   (*fPhydrv_sdsTestMode_set)(uint32, rtk_port_t, uint32, rtk_sds_testMode_t);
    int32   (*fPhydrv_sdsTestModeCnt_get)(uint32, rtk_port_t, uint32, uint32 *);
    int32   (*fPhydrv_sdsLeq_get)(uint32, rtk_port_t, uint32, rtk_enable_t *, uint32 *);
    int32   (*fPhydrv_sdsLeq_set)(uint32, rtk_port_t, uint32, rtk_enable_t, uint32);
    int32   (*fPhydrv_remoteFault_handle)(uint32, rtk_port_t);
    int32   (*fPhydrv_speedStatus_get)(uint32, rtk_port_t, rtk_port_speed_t *);
    int32   (*fPhydrv_liteEnable_get)(uint32, rtk_port_t, rtk_port_lite_mode_t, rtk_enable_t *);
    int32   (*fPhydrv_liteEnable_set)(uint32, rtk_port_t, rtk_port_lite_mode_t, rtk_enable_t );
    int32   (*fPhydrv_dbgCounter_get)(uint32, rtk_port_t, rtk_port_phy_dbg_cnt_t, uint64 *);
    int32   (*fPhydrv_speedDuplexStatus_get)(uint32, rtk_port_t, rtk_port_speed_t *, rtk_port_duplex_t *);

    int32   (*fPhydrv_macsec_reg_get)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 *pData);
    int32   (*fPhydrv_macsec_reg_set)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 data);
    int32   (*fPhydrv_macsec_port_cfg_set)(uint32 unit, rtk_port_t port, rtk_macsec_port_cfg_t *pPortcfg);
    int32   (*fPhydrv_macsec_port_cfg_get)(uint32 unit, rtk_port_t port, rtk_macsec_port_cfg_t *pPortcfg);
    int32   (*fPhydrv_macsec_sc_create)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, rtk_macsec_sc_t *pSc, uint32 *pSc_id);
    int32   (*fPhydrv_macsec_sc_get)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id, rtk_macsec_sc_t *pSc);
    int32   (*fPhydrv_macsec_sc_del)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id);
    int32   (*fPhydrv_macsec_sc_status_get)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id, rtk_macsec_sc_status_t *pSc_status);
    int32   (*fPhydrv_macsec_sa_create)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);
    int32   (*fPhydrv_macsec_sa_get)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);
    int32   (*fPhydrv_macsec_sa_del)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id, rtk_macsec_an_t an);
    int32   (*fPhydrv_macsec_sa_activate)(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sc_id, rtk_macsec_an_t an);
    int32   (*fPhydrv_macsec_rxsa_disable)(uint32 unit, rtk_port_t port, uint32 rxsc_id, rtk_macsec_an_t an);
    int32   (*fPhydrv_macsec_txsa_disable)(uint32 unit, rtk_port_t port, uint32 txsc_id);
    int32   (*fPhydrv_macsec_stat_clear)(uint32 unit, rtk_port_t port);
    int32   (*fPhydrv_macsec_stat_port_get)(uint32 unit, rtk_port_t port, rtk_macsec_stat_t stat, uint64 *pCnt);
    int32   (*fPhydrv_macsec_stat_txsa_get)(uint32 unit, rtk_port_t port, uint32 txsc_id, rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, uint64 *pCnt);
    int32   (*fPhydrv_macsec_stat_rxsa_get)(uint32 unit, rtk_port_t port, uint32 rxsc_id, rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, uint64 *pCnt);
    int32   (*fPhydrv_macsec_intr_status_get)(uint32 unit, rtk_port_t port, rtk_macsec_intr_status_t *pIntr_status);
} rt_phydrv_t;

typedef int32 (*phy_ident_f)(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
typedef void  (*fPhydrv_mapperInit_f)(rt_phydrv_t* pPhydrv);

typedef struct rt_phyInfo_s
{
    uint32  phy_num;    /* PHY number in the chip, like 8218 is 8, 8214C is 4 */
    uint32  eth_type;
    uint32  macsec_sa_num; /* macsec SA entry number in the chip */
    uint32  isComboPhy[MAX_PHY_NUM_OF_CHIP];
    uint32  flags;      /* RTK_PHYINFO_FLAG_ */

    /* 10G/2.5G PHY MMD dev/address/bit of local duplex ability.
     * The value will apply to MAC for MAC-Polling-PHY when RTK_PHYINFO_FLAG_1G_MMD_CFG specified in flags
     */
    uint16  xGePhyLocalDuplexAbilityBit;
    uint16  xGePhyLocalDuplexAbilityDev;
    uint32  xGePhyLocalDuplexAbilityAddr;

    /* 10G/2.5G PHY MMD dev/address/bit of local 1000M speed ability.
     * The value will apply to MAC for MAC-Polling-PHY when RTK_PHYINFO_FLAG_1G_MMD_CFG specified in flags
     */
    uint16  xGePhyLocal1000MSpeedAbilityBit;
    uint16  xGePhyLocal1000MSpeedAbilityDev;
    uint32  xGePhyLocal1000MSpeedAbilityAddr;

    /* 10G/2.5G PHY MMD dev/address/bit of Link partner 1000M speed ability.
     * The value will apply to MAC for MAC-Polling-PHY when RTK_PHYINFO_FLAG_1G_MMD_CFG specified in flags
     */
    uint16  xGePhyLinkPartner1000MSpeedAbilityBit;
    uint16  xGePhyLinkPartner1000MSpeedAbilityDev;
    uint32  xGePhyLinkPartner1000MSpeedAbilityAddr;
} rt_phyInfo_t;

/* definition phy control structure */
typedef struct rt_phyctrl_s
{
    phy_ident_f chk_func;
    uint32      phy_model_id;
    uint32      phy_rev_id;
    phy_type_t  phyType;
    rt_phydrv_t *pPhydrv;
    fPhydrv_mapperInit_f mapperInit_func;
    uint32      drv_rev_id;
    rt_phyInfo_t *pPhyInfo;
} rt_phyctrl_t;


/* phy patch type0: per-chip*/
typedef struct phy_patch_array_type0_s
{
    rtk_port_t  phy;
    uint32  reg;
    uint32 val;
} phy_patch_array_type0_t;

/* phy patch type1: per-port*/
typedef struct phy_patch_array_type1_s
{
    uint32  reg;
    uint32 val;
} phy_patch_array_type1_t;

/* phy patch type2: per-chip specific bit */
typedef struct {
    uint32  reg;
    uint32  endBit;
    uint32  startBit;
    uint32  val;
} phy_patch_array_type2_t;

/* PHY Transformer RJ45 Impedance */
typedef enum rtk_phy_xfmrRj45Impd_e
{
    RTK_PHY_XFMRRJ45IMPD_75_OHM = 0,
    RTK_PHY_XFMRRJ45IMPD_95_OHM,
    RTK_PHY_XFMRRJ45IMPD_100_OHM,
    RTK_PHY_XFMRRJ45IMPD_END,
} rtk_phy_xfmrRj45Impd_t;



#endif /* __HAL_PHY_PHYDEF_H__ */
